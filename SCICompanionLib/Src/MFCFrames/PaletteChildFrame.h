#pragma once


// CPaletteChildFrame frame

#include "MDITabChildWnd.h"
#include "PaletteEditorDialog.h"

class CPaletteChildFrame : public CMDITabChildWnd
{
    DECLARE_DYNCREATE(CPaletteChildFrame)
public:
    void HookUpNonViews(CDocument *pDoc) override;

protected:
    CPaletteChildFrame();           // protected constructor used by dynamic creation
    virtual ~CPaletteChildFrame();
    virtual MDITabType GetTabType() { return TAB_PALETTE; }

protected:
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
   

    CToolBar m_wndToolBar;
};


