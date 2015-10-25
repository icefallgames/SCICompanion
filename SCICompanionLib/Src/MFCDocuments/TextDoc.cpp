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
