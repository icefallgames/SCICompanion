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
        _audioSidecarResources.clear();
        int mapResourceNumber = GetResource()->ResourceNumber;
        auto resourceContainer = appState->GetResourceMap().Resources(ResourceTypeFlags::Audio, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::IncludeCacheFiles, mapResourceNumber);
        for (auto resource : *resourceContainer)
        {
            _audioSidecarResources[resource->GetBase36()] = CreateResourceFromResourceData(*resource);
        }
    }
}

void CMessageDoc::AddNewAudioResource(std::unique_ptr<ResourceEntity> audioResource)
{
    _newAudioSidecarResources.insert(audioResource->Base36Number);
    _audioSidecarResources[audioResource->Base36Number] = std::move(audioResource);

    SetModifiedFlag(TRUE);
    UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(MessageChangeHint::ItemChanged));
}

bool CMessageDoc::SetSyncComponent(uint32_t base36Number, std::unique_ptr<SyncComponent> sync)
{
    bool changed = false;
    auto it = _audioSidecarResources.find(base36Number);
    if (it != _audioSidecarResources.end())
    {
        ResourceEntity *entity = it->second.get();
        SyncComponent *existingSync = entity->TryGetComponent<SyncComponent>();
        if (existingSync && sync)
        {
            changed = *existingSync != *sync;
            if (changed)
            {
                entity->RemoveComponent<SyncComponent>();
                entity->AddComponent<SyncComponent>(std::move(sync));
            }
        }
        else if (!existingSync && sync)
        {
            changed = true;
            entity->AddComponent<SyncComponent>(std::move(sync));
        }
        else if (existingSync && !sync)
        {
            changed = true;
            entity->RemoveComponent<SyncComponent>();
        }
    }

    if (changed)
    {
        _newAudioSidecarResources.insert(base36Number);
        SetModifiedFlag(TRUE);
        UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(MessageChangeHint::ItemChanged));
    }
    return changed;
}

void CMessageDoc::PostSuccessfulSave(const ResourceEntity *pResource)
{
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

    _newAudioSidecarResources.clear();
}

ResourceEntity *CMessageDoc::FindAudioResource(uint32_t base36Number)
{
    auto it = _audioSidecarResources.find(base36Number);
    if (it == _audioSidecarResources.end())
    {
        return nullptr;
    }
    else
    {
        return (*it).second.get();
    }
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
