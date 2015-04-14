// RoomExplorerDoc.cpp : implementation file
//

#include "stdafx.h"
//#include "SCIComp.h"
#include "RoomExplorerDoc.h"


// CRoomExplorerDoc

IMPLEMENT_DYNCREATE(CRoomExplorerDoc, CDocument)

CRoomExplorerDoc::CRoomExplorerDoc()
{
}

BOOL CRoomExplorerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CRoomExplorerDoc::~CRoomExplorerDoc()
{
}


BEGIN_MESSAGE_MAP(CRoomExplorerDoc, CDocument)
END_MESSAGE_MAP()


// CRoomExplorerDoc diagnostics

#ifdef _DEBUG
void CRoomExplorerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRoomExplorerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CRoomExplorerDoc serialization

void CRoomExplorerDoc::Serialize(CArchive& ar)
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


// CRoomExplorerDoc commands
