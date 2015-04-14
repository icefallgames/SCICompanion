#pragma once

#include "PaletteOperations.h"
#include "PaletteEditorCommon.h"

class CPaletteDoc;

class CPaletteView : public PaletteEditorCommon< CExtWA < CExtWS < CExtAFV < CFormView >  > > >
{
	DECLARE_DYNCREATE(CPaletteView)

protected:
	CPaletteView();           // protected constructor used by dynamic creation
	virtual ~CPaletteView();

public:
    CPaletteDoc* GetDocument() const;

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
    void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint) override;
    void OnInitialUpdate() override;

};



#ifndef _DEBUG  // debug version in VocabView.cpp
inline CPaletteDoc* CPaletteView::GetDocument() const
{ return reinterpret_cast<CPaletteDoc*>(m_pDocument); }
#endif
