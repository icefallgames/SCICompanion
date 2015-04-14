// TextDoc.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "TextDoc.h"

using namespace std;

// CTextDoc

IMPLEMENT_DYNCREATE(CTextDoc, CResourceDocument)

void CTextDoc::SetTextResource(std::unique_ptr<ResourceEntity> pText, int id)
{
    _checksum = id;
    AddFirstResource(move(pText));
    _UpdateTitle();
    UpdateAllViews(nullptr, 0, &WrapHint(TextChangeHint::Changed));
}

BEGIN_MESSAGE_MAP(CTextDoc, TCLASS_2(CUndoResource, CResourceDocument, ResourceEntity))
END_MESSAGE_MAP()

void CTextDoc::v_OnUndoRedo()
{
    UpdateAllViews(nullptr, 0, &WrapHint(TextChangeHint::Changed));
}

// CTextDoc diagnostics

#ifdef _DEBUG
void CTextDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTextDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CTextDoc serialization

void CTextDoc::Serialize(CArchive& ar)
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


// CTextDoc commands
