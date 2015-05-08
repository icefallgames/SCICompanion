#pragma once

#include "ResourceEntityDocument.h"
#include "Text.h"

// CTextDoc document

class CTextDoc : public ResourceEntityDocument
{
	DECLARE_DYNCREATE(CTextDoc)

public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o

    // Takes ownership:
    void SetTextResource(std::unique_ptr<ResourceEntity> pText, int id = -1);

    // CUndoResource
    void v_OnUndoRedo();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

    virtual ResourceType _GetType() const
    {
        const ResourceEntity *pResource = static_cast<const ResourceEntity*>(GetResource());
        return pResource->GetType();
    }


	DECLARE_MESSAGE_MAP()
};
