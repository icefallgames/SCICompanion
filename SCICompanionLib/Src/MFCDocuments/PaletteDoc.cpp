// TextDoc.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "PaletteDoc.h"

using namespace std;

// CPaletteDoc

IMPLEMENT_DYNCREATE(CPaletteDoc, CResourceDocument)

void CPaletteDoc::SetResource(std::unique_ptr<ResourceEntity> pPalette, int id)
{
    _checksum = id;
    AddFirstResource(move(pPalette));
    _UpdateTitle();
    UpdateAllViews(nullptr, 0, &WrapHint(PaletteChangeHint::Changed));
}

BEGIN_MESSAGE_MAP(CPaletteDoc, TCLASS_2(CUndoResource, CResourceDocument, ResourceEntity))
END_MESSAGE_MAP()

void CPaletteDoc::v_OnUndoRedo()
{
    UpdateAllViews(nullptr, 0, &WrapHint(PaletteChangeHint::Changed));
}

// CPaletteDoc diagnostics

#ifdef _DEBUG
void CPaletteDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPaletteDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CPaletteDoc serialization

void CPaletteDoc::Serialize(CArchive& ar)
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


// CPaletteDoc commands
