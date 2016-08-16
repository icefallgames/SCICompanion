/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
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

		RECT rcScreen;
		rcScreen.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
		rcScreen.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
		rcScreen.right = rcScreen.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
		rcScreen.bottom = rcScreen.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);

        // Alignment at right if necessary
        if (pt.x + RECTWIDTH(rcCtrl) > rcScreen.right)
        {
            pt.x = _rcTrack.right - RECTWIDTH(rcCtrl);
        }
        // Alignment at top if necessary
        if (pt.y + RECTHEIGHT(rcCtrl) > rcScreen.bottom)
        {
            pt.y = _rcTrack.top - RECTHEIGHT(rcCtrl);
        }

        // Adjustments to keep control into screen
        if (pt.x + RECTWIDTH(rcCtrl) > rcScreen.right)
        {
            pt.x = rcScreen.right - RECTWIDTH(rcCtrl);
        }
        if (pt.y + RECTHEIGHT(rcCtrl) > rcScreen.bottom)
        {
            pt.y = rcScreen.bottom - RECTHEIGHT(rcCtrl);
        }
        if (pt.x < rcScreen.left)
        {
            pt.x = rcScreen.left;
        }
        if (pt.y < rcScreen.top)
        {
            pt.y = rcScreen.top;
        }

        RECT rcNew = { pt.x, pt.y, pt.x + RECTWIDTH(rcCtrl), pt.y + RECTHEIGHT(rcCtrl) };
        MoveWindow(&rcNew);
    }
}


