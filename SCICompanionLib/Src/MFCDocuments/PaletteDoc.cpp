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
