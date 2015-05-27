#pragma once

#include "ResourceEntityDocument.h"
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

    MessageSource *GetNounMessageSource();
    MessageSource *GetConditionMessageSource();

    void SetSelectedIndex(int index);
    int GetSelectedIndex() const { return _selectedIndex; }

protected:

    virtual ResourceType _GetType() const
    {
        const ResourceEntity *pResource = static_cast<const ResourceEntity*>(GetResource());
        return pResource->GetType();
    }

    std::unique_ptr<MessageHeaderFile> _messageHeaderFile;
    int _selectedIndex;

    DECLARE_MESSAGE_MAP()
};
