#pragma once

#include "ResourceEntityDocument.h"
#include "MessageSource.h"
#include "Message.h"

class MessageSource;
class MessageHeaderFile;

class CMessageDoc : public ResourceEntityDocument
{
    DECLARE_DYNCREATE(CMessageDoc)

public:
    CMessageDoc();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o

    // Takes ownership:
    void SetMessageResource(std::unique_ptr<ResourceEntity> pText, int id = -1);

    // CUndoResource
    void v_OnUndoRedo();

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    void SetSelectedIndex(int index, bool force = false);
    int GetSelectedIndex() const { return _selectedIndex; }

    // Audio sidecar support:
    ResourceEntity *FindAudioResource(uint32_t base36Number);
    void AddNewAudioResource(std::unique_ptr<ResourceEntity> audioResource);

protected:
    void PostSuccessfulSave(const ResourceEntity *pResource) override;

    ResourceType _GetType() const override
    {
        const ResourceEntity *pResource = static_cast<const ResourceEntity*>(GetResource());
        return pResource->GetType();
    }

private:
    void _PreloadAudio();

    int _selectedIndex;

    // Audio sidecare stuff.
    std::unordered_map<uint32_t, std::unique_ptr<ResourceEntity>> _audioSidecarResources;
    std::vector<ResourceEntity*> _newAudioSidecarResources; // temp
    std::unordered_map<uint32_t, std::unique_ptr<ResourceEntity>> _syncSidecarResources;

    DECLARE_MESSAGE_MAP()
};

const MessageSource *GetMessageSourceFromType(CMessageDoc *pDoc, MessageSourceType sourceType, bool reload);