#pragma once

#include "MdiTabChildWnd.h"
#include "RasterSidePane.h"

// fwd decl
class CRasterView;

// CEditViewChildFrame frame with splitter

class CEditViewChildFrame : public CMDITabChildWnd
{
	DECLARE_DYNCREATE(CEditViewChildFrame)
protected:
	CEditViewChildFrame();           // protected constructor used by dynamic creation
	virtual ~CEditViewChildFrame();
    virtual MDITabType GetTabType() { return TAB_VIEW; }

public:
    void HookUpNonViews(CDocument *pDoc);
    BOOL OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo);

protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
    void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

	CExtSplitterWnd m_wndSplitter;
    CExtSplitterWnd m_wndSplitterLeft;

    CRasterView *_pViewMainView;
    bool _splitterCreated;
    CSize _lastSize;
};


