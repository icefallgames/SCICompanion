#pragma once

#include "MdiTabChildWnd.h"
#include "RasterSidePane.h"

// fwd decl
class CRasterView;
class CFontPreviewView;

// CEditFontChildFrame frame with splitter

class CEditFontChildFrame : public CMDITabChildWnd
{
	DECLARE_DYNCREATE(CEditFontChildFrame)
protected:
	CEditFontChildFrame();           // protected constructor used by dynamic creation
	virtual ~CEditFontChildFrame();
    virtual MDITabType GetTabType() { return TAB_FONT; }

public:
    void HookUpNonViews(CDocument *pDoc);
    BOOL OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo);

protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

	CExtSplitterWnd m_wndSplitter;
    CExtSplitterWnd m_wndSplitterTop;
    
    CRasterView *_pViewMainView;
    CFontPreviewView *_pFontPreviewView;
};


