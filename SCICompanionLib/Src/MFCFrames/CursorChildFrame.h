#pragma once

#include "MdiTabChildWnd.h"
#include "RasterSidePane.h"

// fwd decl
class CRasterView;

// CEditFontChildFrame frame with splitter

class CCursorChildFrame : public CMDITabChildWnd
{
	DECLARE_DYNCREATE(CCursorChildFrame)
protected:
	CCursorChildFrame();           // protected constructor used by dynamic creation
	virtual ~CCursorChildFrame();
    virtual MDITabType GetTabType() { return TAB_CURSOR; }

public:
    void HookUpNonViews(CDocument *pDoc);
    BOOL OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo);

protected:
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

    CRasterView *_pViewMainView;
};


