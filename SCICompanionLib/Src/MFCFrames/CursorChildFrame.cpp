// EditViewChildFrame.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "CursorChildFrame.h"
#include "RasterView.h"

// CCursorChildFrame

IMPLEMENT_DYNCREATE(CCursorChildFrame, CMDITabChildWnd)

CCursorChildFrame::CCursorChildFrame()
{
    _pViewMainView = NULL;
}

CCursorChildFrame::~CCursorChildFrame()
{
}

BEGIN_MESSAGE_MAP(CCursorChildFrame, CMDITabChildWnd)
    ON_WM_CREATE()
END_MESSAGE_MAP()



void CCursorChildFrame::HookUpNonViews(CDocument *pDoc)
{
}


int CCursorChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}

BOOL CCursorChildFrame::OnCmdMsg(UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo)
{
    BOOL fRet = __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
    if (!fRet)
    {
        // If unhandled, give it the main (even if it was non-active).
        // NOTE: if it was active, it was already asked to handle it above.
        // We could optimize for that case here, if desired.
        if (GetActiveView())
        {
            fRet = GetActiveView()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
        }

        if (!fRet)
        {
            // Give to CommandType view?

        }
    }
    return fRet;
}


// CCursorChildFrame message handlers
