// EditViewChildFrame.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ViewChildFrame.h"
#include "RasterView.h"
#include "ViewCelChooser.h"

// CEditViewChildFrame

IMPLEMENT_DYNCREATE(CEditViewChildFrame, CMDITabChildWnd)

CEditViewChildFrame::CEditViewChildFrame()
{
    _pViewMainView = NULL;
}

CEditViewChildFrame::~CEditViewChildFrame()
{
}

BEGIN_MESSAGE_MAP(CEditViewChildFrame, CMDITabChildWnd)
    ON_WM_CREATE()
END_MESSAGE_MAP()



void CEditViewChildFrame::HookUpNonViews(CDocument *pDoc)
{

}


int CEditViewChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}

BOOL CEditViewChildFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
    // This is the code for a static splitter.
    CRect cr;
    GetClientRect(&cr);
    int widthUnit = cr.Width() / 3;
    CSize paneSize;

    int rc;
    m_wndSplitter.CreateStatic(this, 1, 2);

    paneSize.SetSize(widthUnit * 2, cr.Height());

    rc = m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CRasterView), paneSize, pContext);
    if (rc == FALSE)
    {
        return rc;
    }
    _pViewMainView = (CRasterView*)m_wndSplitter.GetPane(0, 0);

    paneSize.SetSize(widthUnit, widthUnit);
    rc = m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CEditViewCelChooser), paneSize, pContext);

    return rc;
}

BOOL CEditViewChildFrame::OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo)
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


// CEditViewChildFrame message handlers
