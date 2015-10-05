// TextDoc.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "MessageDoc.h"
#include "Text.h"
#include "Message.h"
#include "MessageSource.h"
#include "MessageHeaderFile.h"
#include "format.h"
#include "NounsAndCases.h"
#include "ResourceContainer.h"
#include "PerfTimer.h"
#include "ResourceMap.h"
#include "Sync.h"

using namespace std;

// CMessageDoc

IMPLEMENT_DYNCREATE(CMessageDoc, CResourceDocument)

CMessageDoc::CMessageDoc() : _selectedIndex(-1)
{
}

void CMessageDoc::SetSelectedIndex(int index, bool force)
{
    if (force || (index != _selectedIndex))
    {
        _selectedIndex = index;
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(MessageChangeHint::Selection));
    }
}

void CMessageDoc::_PreloadAudio()
{
    PerfTimer timer("preloadAudio");
    if (appState->GetVersion().HasSyncResources)
    {
        const TextComponent &text = GetResource()->GetComponent<TextComponent>();
        _audioResources.clear();
        _modified.clear();

        // Get all the resources from the audio map
        std::unordered_map<uint32_t, std::unique_ptr<ResourceEntity>> _temporaryMap;
        int mapResourceNumber = GetResource()->ResourceNumber;
        auto resourceContainer = appState->GetResourceMap().Resources(ResourceTypeFlags::Audio, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::IncludeCacheFiles, mapResourceNumber);
        for (auto resource : *resourceContainer)
        {
            _temporaryMap[resource->GetBase36()] = CreateResourceFromResourceData(*resource);
        }

        // Now assign them slots based on the text entry indices.
        // Using this system (instead of looking them up by tuples), since we want an easy way to keep
        // track of things when the user changes the message entry tuple.
        for (const TextEntry &entry : text.Texts)
        {
            _audioResources.push_back(std::move(_temporaryMap[GetMessageTuple(entry)]));
            _modified.push_back(false);
        }
        assert(_audioResources.size() == text.Texts.size());
    }
}


void CMessageDoc::PostSuccessfulSave(const ResourceEntity *pResource)
{
    // We need to figure out the series of changes required to save our modified audio resources.

    /*
    // We only support adding now, this is kind of hacky.
    // (Supporting changing of the base36 number is an optimization we can do.)
    CResourceMap &map = appState->GetResourceMap();

    {
        DeferResourceAppend defer(map);
        for (uint32_t tuple : _newAudioSidecarResources)
        {
            ResourceEntity &resource = *_audioSidecarResources[tuple];
            // We *only* support saving to the audio cachhe.
            resource.SourceFlags = ResourceSourceFlags::AudioCache;
            map.AppendResource(resource);
        }
        defer.Commit();
    }
    */
    // Clear out the modified flags
    std::fill(_modified.begin(), _modified.end(), false);
}

void CMessageDoc::AddEntry(const TextEntry &entry)
{
    int index = GetSelectedIndex();
    index++;

    ApplyChangesWithPost<TextComponent>(
        [index, &entry](TextComponent &text)
    {
        text.Texts.insert(text.Texts.begin() + index, entry);
        return WrapHint(MessageChangeHint::Changed);
    },
        [index, this](ResourceEntity &resource)
    {
        this->_audioResources.insert(this->_audioResources.begin() + index, nullptr);
        this->_modified.insert(this->_modified.begin() + index, false);
    }
    );
    // Now select it
    SetSelectedIndex(index);
}

void CMessageDoc::DeleteCurrentEntry()
{
    int selected = GetSelectedIndex();
    if (selected != -1)
    {
        bool ok = true;
        if (_audioResources[selected])
        {
            ok = (IDYES == AfxMessageBox("This entry contains an associated audio resources. This will delete the audio resource too. Continue?", MB_YESNO | MB_ICONWARNING));
        }

        if (ok)
        {
            int newSelected = selected;
            ApplyChangesWithPost<TextComponent>(
                [selected, &newSelected](TextComponent &text)
            {
                TextChangeHint hint = text.DeleteString(selected);
                newSelected = max(0, min(selected, (int)(text.Texts.size() - 1)));
                return WrapHint(hint);
            },
                [selected, &newSelected, this](ResourceEntity &resource)
            {
                this->_audioResources.erase(this->_audioResources.begin() + selected);
                this->_modified.erase(this->_modified.begin() + selected);
            }
            );
            SetSelectedIndex(newSelected, true);
        }
    }
}

const char c_szMessageTxtFilter[] = "txt files (*.txt)|*.txt|All Files|*.*|";

void CMessageDoc::ImportMessage()
{
    ApplyChanges<TextComponent>(
        [&](TextComponent &text)
    {
        MessageChangeHint hint = MessageChangeHint::None;
        CFileDialog fileDialog(TRUE, nullptr, nullptr, OFN_HIDEREADONLY | OFN_NOCHANGEDIR, c_szMessageTxtFilter);
        if (IDOK == fileDialog.DoModal())
        {
            CString strFileName = fileDialog.GetPathName();
            ImportMessageFromFile(text, (PCSTR)strFileName);
            hint |= MessageChangeHint::Changed;

            // Fill with empty spots:
            this->_audioResources.resize(text.Texts.size());
            this->_modified.resize(text.Texts.size(), false);
        }
        return WrapHint(hint);
    }
    );
}

void CMessageDoc::ExportMessage()
{
    const ResourceEntity *resource = GetResource();
    if (resource)
    {
        CFileDialog fileDialog(FALSE, nullptr, fmt::format("{0}.txt", resource->ResourceNumber).c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, c_szMessageTxtFilter);
        if (IDOK == fileDialog.DoModal())
        {
            CString strFileName = fileDialog.GetPathName();
            ExportMessageToFile(resource->GetComponent<TextComponent>(), (PCSTR)strFileName);
        }
    }
}

void CMessageDoc::SetEntry(const TextEntry &newEntry)
{
    int selected = GetSelectedIndex();
    if (selected != -1)
    {
        bool tupleChanged = false;
        ApplyChanges<TextComponent>(
            [selected, newEntry, &tupleChanged](TextComponent &text)
        {
            MessageChangeHint hint = MessageChangeHint::None;
            if (newEntry != text.Texts[selected])
            {
                tupleChanged = GetMessageTuple(newEntry) != GetMessageTuple(text.Texts[selected]);
                text.Texts[selected] = newEntry;
                hint |= MessageChangeHint::ItemChanged;
            }
            return WrapHint(hint);
        }
        );

        if (tupleChanged)
        {
            // Not really necessary, we could detect this easily...
            _modified[selected] = true;
        }
    }
}

ResourceEntity *CMessageDoc::GetAudioResource()
{
    ResourceEntity *resource = nullptr;
    if (_selectedIndex != -1)
    {
        resource = _audioResources[_selectedIndex].get();
    }
    return resource;
}

ResourceEntity *CMessageDoc::GetAudioResource(int index)
{
    ResourceEntity *resource = nullptr;
    if (index >= 0 && index < (int)_audioResources.size())
    {
        resource = _audioResources[index].get();
    }
    return resource;
}

void CMessageDoc::SetAudioResource(std::unique_ptr<ResourceEntity> audioResource)
{
    assert(_selectedIndex != -1);
    _audioResources[_selectedIndex] = std::move(audioResource);

    SetModifiedFlag(TRUE);
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(MessageChangeHint::ItemChanged));
}

void CMessageDoc::SetMessageResource(std::unique_ptr<ResourceEntity> pMessage, int id)
{
    _checksum = id;

    if (pMessage)
    {
        // Add a nouns/cases component
        pMessage->AddComponent<NounsAndCasesComponent>(
            std::make_unique<NounsAndCasesComponent>(appState->GetResourceMap().Helper().GetMsgFolder(), pMessage->ResourceNumber)
            );
    }

    AddFirstResource(move(pMessage));
    _UpdateTitle();

    _PreloadAudio();

    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(MessageChangeHint::Changed | MessageChangeHint::AllMessageFiles));
}

BEGIN_MESSAGE_MAP(CMessageDoc, TCLASS_2(CUndoResource, CResourceDocument, ResourceEntity))
END_MESSAGE_MAP()

void CMessageDoc::v_OnUndoRedo()
{
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(MessageChangeHint::Changed));
}

const MessageSource *GetMessageSourceFromType(CMessageDoc *pDoc, MessageSourceType sourceType, bool reload)
{
    if (pDoc)
    {
        const ResourceEntity *resource = pDoc->GetResource();
        switch (sourceType)
        {
            case MessageSourceType::Conditions:
                return resource ? &resource->GetComponent<NounsAndCasesComponent>().GetCases() : nullptr;
            case MessageSourceType::Verbs:
                return appState->GetResourceMap().GetVerbsMessageSource(reload);
            case MessageSourceType::Talkers:
                return appState->GetResourceMap().GetTalkersMessageSource(reload);
            case MessageSourceType::Nouns:
                return resource ? &resource->GetComponent<NounsAndCasesComponent>().GetNouns() : nullptr;
        }
    }
    return nullptr;
}

// Since we modify audio resources in a significant way, we prevent undos if audio is supported for messages.
bool CMessageDoc::v_PreventUndos() const
{
    return appState->GetVersion().HasSyncResources;
}


// CMessageDoc diagnostics

#ifdef _DEBUG
void CMessageDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CMessageDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG


// CMessageDoc serialization

void CMessageDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // TODO: add storing code here
    }
    else
    {
        // TODO: add loading code here
    }
}


// CMessageDoc commands
