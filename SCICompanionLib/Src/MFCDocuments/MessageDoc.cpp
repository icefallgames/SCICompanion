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
#include "AudioCacheResourceSource.h"
#include "AudioMap.h"

using namespace std;

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
        _audioModified.clear();
        _originalTuplesWithAudio.clear();

        // If we're sourced from the cache files, we want to *only* include entries from the cached audiomap (i.e. not 
        // combine them with audio resources enumerated from resource.aud). Otherwise, you could delete an audio resource from
        // a message entry, and it would keeping "returning" when you re-opened the message resource (until you rebuilt audio files)
        std::unique_ptr<std::unordered_set<uint32_t>> restrictToTheseTuples;
        unique_ptr<ResourceBlob> amBlob = appState->GetResourceMap().Helper().MostRecentResource(ResourceType::AudioMap, GetResource()->ResourceNumber, ResourceEnumFlags::IncludeCacheFiles);
        if (amBlob && (amBlob->GetSourceFlags() == ResourceSourceFlags::AudioMapCache))
        {
            std::unique_ptr<ResourceEntity> amResource = CreateResourceFromResourceData(*amBlob);
            if (amResource)
            {
                restrictToTheseTuples = std::make_unique<std::unordered_set<uint32_t>>();
                for (const auto &entry : amResource->GetComponent<AudioMapComponent>().Entries)
                {
                    restrictToTheseTuples->insert(GetMessageTuple(entry));
                }
            }
        }

        // Get all the resources from the audio map
        std::unordered_map<uint32_t, std::unique_ptr<ResourceEntity>> _temporaryMap;
        int mapResourceNumber = GetResource()->ResourceNumber;
        auto resourceContainer = appState->GetResourceMap().Resources(ResourceTypeFlags::Audio, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::IncludeCacheFiles, mapResourceNumber);
        for (auto resource : *resourceContainer)
        {
            // Again, as mentioned above, we only want to take our audio resources from a single source: the cache files, or the actual game resources.
            if (!restrictToTheseTuples || (restrictToTheseTuples->find(resource->GetBase36()) != restrictToTheseTuples->end()))
            {
                _temporaryMap[resource->GetBase36()] = CreateResourceFromResourceData(*resource);
                _originalTuplesWithAudio.insert(resource->GetBase36());
            }
        }

        // Now assign them slots based on the text entry indices.
        // Using this system (instead of looking them up by tuples), since we want an easy way to keep
        // track of things when the user changes the message entry tuple.
        for (const TextEntry &entry : text.Texts)
        {
            _audioResources.push_back(std::move(_temporaryMap[GetMessageTuple(entry)]));
            _audioModified.push_back(false);
        }
        assert(_audioResources.size() == text.Texts.size());
    }
}


void CMessageDoc::PostSuccessfulSave(const ResourceEntity *pResource)
{
    // We need to figure out the series of changes required to save our modified audio resources.
    // We could do something very simple, which is just do an AppendResource for every modified entry.
    // The issue then, is that if the user changes the noun/verb/cond/seq of a tuple, we'll be saving it
    // under a new filename, and the old files will linger, never to be deleted.
    // We *could* something more intelligent, like mark some of the appended resources as
    // "moved". We know they moved by looking at the different tuple they are now associated with.
    // So we could potentially pass in a list of "old tuples" to the AudioCacheResourceSource. And if they
    // are not associated with any current entry in the audiomap, they can be deleted.
    std::set<uint32_t> currentTextEntryTuplesWithAudio;
    const TextComponent &text = pResource->GetComponent<TextComponent>();
    CResourceMap &map = appState->GetResourceMap();
    DeferResourceAppend defer(map);
    for (size_t i = 0; i < text.Texts.size(); i++)
    {
        const TextEntry &entry = text.Texts[i];
        uint32_t textEntryTuple = GetMessageTuple(entry);
        ResourceEntity *companionAudio = _audioResources[i].get();
        if (companionAudio)
        {
            currentTextEntryTuplesWithAudio.insert(textEntryTuple);
        }
        if (_audioModified[i])
        {
            if (companionAudio)
            {
                // Now endow this audio resource with up-to-date information based on the text entry
                companionAudio->SourceFlags = ResourceSourceFlags::AudioCache;
                companionAudio->Base36Number = textEntryTuple;
                map.AppendResource(*companionAudio);
            }
            else
            {
                // Probably deletion. We no longer know what the original tuple for this guy is at this point
                // (we *could* track that info... but it gets complicated, since it could have
                // been removed or added many times).
            }
        }
        else
        {
            assert(!companionAudio || (textEntryTuple == companionAudio->Base36Number));
        }
    }
    defer.Commit();

    // Ok, we've commited the modified/new entries.
    // Now, we want to delete any tuples that existed previously, which are *not* in the current set of text entry tuples
    std::vector<uint32_t> deletedTuples;
    for (uint32_t originalTuple : _originalTuplesWithAudio)
    {
        if (currentTextEntryTuplesWithAudio.find(originalTuple) == currentTextEntryTuplesWithAudio.end())
        {
            deletedTuples.push_back(originalTuple);
        }
    }
    // These tuples need to get deleted from the audio map too... so we should go through the proper channels.
    // We can't go through the CResourceMap, because that relies on having an exact identical version of the thing
    // we are deleting. All we want to do is invoke the AudioCacheResourceSource directly and tell it to delete things.
    int mapContext = pResource->ResourceNumber;
    std::unique_ptr<AudioCacheResourceSource> resourceSource = std::make_unique<AudioCacheResourceSource>(map.GetSCIVersion() , map.GetGameFolder(), mapContext, ResourceSourceAccessFlags::ReadWrite);
    resourceSource->RemoveEntries(mapContext, deletedTuples);

    // Keep our list of original tuples up-to-date
    _originalTuplesWithAudio.clear();
    for (auto &resource : _audioResources)
    {
        if (resource)
        {
            _originalTuplesWithAudio.insert(resource->Base36Number);
        }
    }

    // Finally, clear out the modified flags
    std::fill(_audioModified.begin(), _audioModified.end(), false);
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
        // "false" for modified is ok, since there is no audio resource yet.
        this->_audioModified.insert(this->_audioModified.begin() + index, false);
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
                this->_audioModified.erase(this->_audioModified.begin() + selected);
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
            this->_audioModified.resize(text.Texts.size(), false);
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
            _audioModified[selected] = true;
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
    _audioModified[_selectedIndex] = true;

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
