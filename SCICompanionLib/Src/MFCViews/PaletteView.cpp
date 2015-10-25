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
#include "stdafx.h"
#include "PaletteView.h"
#include "PaletteDoc.h"
#include "resource.h"

// CPaletteView

IMPLEMENT_DYNCREATE(CPaletteView, CFormView)

CPaletteView::CPaletteView() : PaletteEditorCommon< CExtWA < CExtWS < CExtAFV < CFormView >  > >  >(nullptr, IDD_PALETTE_EDITOR_EMBEDDED, nullptr)
{
}

CPaletteView::~CPaletteView()
{
}

void CPaletteView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
    UpdateCommon(pHint);
}

void CPaletteView::OnInitialUpdate()
{
    SetDocument(GetDocument());
    __super::OnInitialUpdate();
}


// CPaletteView diagnostics

#ifdef _DEBUG
void CPaletteView::AssertValid() const
{
	CFormView::AssertValid();
}

void CPaletteView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}

CPaletteDoc* CPaletteView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPaletteDoc)));
    return (CPaletteDoc*)m_pDocument;
}
#endif //_DEBUG


// CPaletteView message handlers
