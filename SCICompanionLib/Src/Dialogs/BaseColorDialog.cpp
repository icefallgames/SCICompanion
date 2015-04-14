// BaseColorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "BaseColorDialog.h"


// CBaseColorDialog dialog

CBaseColorDialog::CBaseColorDialog(UINT nID, CWnd* pParent)
    : CExtResizableDialog(nID, pParent)
{
    _fUseTrack = FALSE;

    RECT rc = { 0 };
    m_wndDummyOwner.CreateEx(WS_EX_NOACTIVATE, TEXT("STATIC"), TEXT("SCI popup owner"), WS_POPUP, rc, NULL, 0);
    m_wndDummyOwner.EnableWindow(FALSE); // Disable it, to trick MFC

    // Trick MFC - use the desktop window as the owner, so it doesn't disable the app.
    // (so we can click away, deactivate the dialog and close it)
    this->m_pParentWnd = &m_wndDummyOwner;//CWnd::FromHandle(::GetDesktopWindow());
}

CBaseColorDialog::~CBaseColorDialog()
{
}

BEGIN_MESSAGE_MAP(CBaseColorDialog, CExtResizableDialog)
    ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CBaseColorDialog message handlers


void CBaseColorDialog::OnActivate(UINT nState, CWnd *pWndOther, BOOL bMinimized)
{
    // If we're being de-activated, and we haven't already been "ended", then end
    // will cancel now.  This makes us act like a popup.
    if (!_fEnded && (nState == WA_INACTIVE))
    {
        // We'd better set this, otherwise we'll end up in an infinite loop.  We'll continue
        // to be make inactivate via EndDialog.
        _fEnded = TRUE;

        // Don't set IDCANCEL if someone already ended us.
        EndDialog(IDCANCEL);
    }
}

BOOL CBaseColorDialog::OnInitDialog()
{
    _fEnded = FALSE;
    BOOL bRet = __super::OnInitDialog();
    //ShowSizeGrip(FALSE);
    _SetPosition();
    return bRet;
}


void CBaseColorDialog::_SetPosition()
{
    if (_fUseTrack)
    {
        // Calculate the most appropriate position for the left-top corner of control
        // By default, at left and bottom of the caller
        CPoint pt(_rcTrack.left, _rcTrack.bottom);
        RECT rcCtrl;
        GetClientRect(&rcCtrl);

        // Alignment at right if necessary
        if (pt.x + RECTWIDTH(rcCtrl) > ::GetSystemMetrics (SM_CXVIRTUALSCREEN))
        {
            pt.x = _rcTrack.right - RECTWIDTH(rcCtrl);
        }
        // Alignment at top if necessary
        if (pt.y + RECTHEIGHT(rcCtrl) > ::GetSystemMetrics(SM_CYVIRTUALSCREEN))
        {
            pt.y = _rcTrack.top - RECTHEIGHT(rcCtrl);
        }
        // Adjustments to keep control into screen
        if (pt.x + RECTWIDTH(rcCtrl) > ::GetSystemMetrics(SM_CXVIRTUALSCREEN))
        {
            pt.x = ::GetSystemMetrics (SM_CXSCREEN) - RECTWIDTH(rcCtrl);
        }
        if (pt.y + RECTHEIGHT(rcCtrl) > ::GetSystemMetrics(SM_CYVIRTUALSCREEN))
        {
            pt.y = ::GetSystemMetrics (SM_CYSCREEN)- RECTHEIGHT(rcCtrl);
        }
        if (pt.x < 0)
        {
            pt.x = 0;
        }
        if (pt.y < 0)
        {
            pt.y = 0;
        }

        RECT rcNew = { pt.x, pt.y, pt.x + RECTWIDTH(rcCtrl), pt.y + RECTHEIGHT(rcCtrl) };
        MoveWindow(&rcNew);
    }
}


