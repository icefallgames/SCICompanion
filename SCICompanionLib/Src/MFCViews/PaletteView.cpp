#include "stdafx.h"
#include "PaletteView.h"
#include "PaletteDoc.h"
#include "resource.h"

// CPaletteView

IMPLEMENT_DYNCREATE(CPaletteView, CFormView)

CPaletteView::CPaletteView() : PaletteEditorCommon< CExtWA < CExtWS < CExtAFV < CFormView >  > >  >(IDD_PALETTE_EDITOR_EMBEDDED, nullptr)
{
}

CPaletteView::~CPaletteView()
{
}

void CPaletteView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
    UpdateCommon(pHint);
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

void CPaletteView::OnInitialUpdate()
{
    SetDocument(GetDocument());
    __super::OnInitialUpdate();
}

CPaletteDoc* CPaletteView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPaletteDoc)));
    return (CPaletteDoc*)m_pDocument;
}
#endif //_DEBUG


// CPaletteView message handlers
