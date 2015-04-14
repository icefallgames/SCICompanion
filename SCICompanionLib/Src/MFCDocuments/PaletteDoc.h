#pragma once

#include "ResourceEntityDocument.h"
#include "PaletteOperations.h"

// CPaletteDoc document

class CPaletteDoc : public ResourceEntityDocument
{
    DECLARE_DYNCREATE(CPaletteDoc)

public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o

    // Takes ownership:
    void SetResource(std::unique_ptr<ResourceEntity> pPalette, int id = -1);

    // CUndoResource
    void v_OnUndoRedo();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

    virtual ResourceType _GetType() const { return ResourceType::Palette; }

	DECLARE_MESSAGE_MAP()
};
