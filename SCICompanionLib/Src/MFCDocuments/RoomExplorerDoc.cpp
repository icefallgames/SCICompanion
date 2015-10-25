/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
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
