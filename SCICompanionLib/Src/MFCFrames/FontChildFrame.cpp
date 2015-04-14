// EditViewChildFrame.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "FontChildFrame.h"
#include "RasterView.h"
#include "ViewCelChooser.h"
#include "FontPreviewView.h"

// CEditFontChildFrame

IMPLEMENT_DYNCREATE(CEditFontChildFrame, CMDITabChildWnd)

CEditFontChildFrame::CEditFontChildFrame()
{
    _pViewMainView = NULL;
    _pFontPreviewView = NULL;
}

CEditFontChildFrame::~CEditFontChildFrame()
{
}

BEGIN_MESSAGE_MAP(CEditFontChildFrame, CMDITabChildWnd)
    ON_WM_CREATE()
    ON_COMMAND_EX(IDR_TOOLBARDRAWVIEW, OnBarCheck)
    ON_UPDATE_COMMAND_UI(IDR_TOOLBARDRAWVIEW, OnUpdateControlBarMenu)
END_MESSAGE_MAP()

void CEditFontChildFrame::HookUpNonViews(CDocument *pDoc)
{
}


int CEditFontChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}

BOOL CEditFontChildFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
    // This is the code for a static splitter.
    CRect cr;
    GetClientRect(&cr);
    int heightTop = cr.Height() / 3;
    int heightChooser = cr.Height() * 2 / 3;
    CSize paneSize;

    int rc;
    m_wndSplitter.CreateStatic(this, 2, 1);

    rc = m_wndSplitterTop.CreateStatic(&m_wndSplitter, 1, 2,  WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndSplitter.IdFromRowCol(0, 0));
    if (rc == FALSE)
    {
        return rc;
    }

    paneSize.SetSize(cr.Width(), heightChooser);
    rc = m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CEditViewCelChooser), paneSize, pContext);
    if (rc == FALSE)
    {
        return rc;
    }

    paneSize.SetSize(cr.Width() / 5, heightTop);
    rc = m_wndSplitterTop.CreateView(0, 0, RUNTIME_CLASS(CRasterView), paneSize, pContext);
    if (rc == FALSE)
    {
        return rc;
    }
    _pViewMainView = (CRasterView*)m_wndSplitterTop.GetPane(0, 0);

    paneSize.SetSize(cr.Width() * 4 / 5, heightTop);
    rc = m_wndSplitterTop.CreateView(0, 1, RUNTIME_CLASS(CFontPreviewView), paneSize, pContext);
    if (rc == FALSE)
    {
        return rc;
    }

    // This does nothing
    m_wndSplitter.SetRowInfo(0, heightTop, 20);

    return rc;
}

BOOL CEditFontChildFrame::OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo)
{
    BOOL fRet = __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
    if (!fRet)
    {
        // If unhandled, give it the main (even if it was non-active).
        // NOTE: if it was active, it was already asked to handle it above.
        // We could optimize for that case here, if desired.
        if (_pViewMainView)
        {
            fRet = _pViewMainView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
        }

        if (!fRet)
        {
            // Give to CommandType view?

        }
    }
    return fRet;
}


// CEditFontChildFrame message handlers
