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
#include "stdafx.h"
#include "ToolTipDialog.h"
#include "AppState.h"

ToolTipDialog::ToolTipDialog() : CExtResizableDialog()
{

}

ToolTipDialog::ToolTipDialog(UINT id, CWnd* pParent) : CExtResizableDialog(id, pParent)
{

}

BEGIN_MESSAGE_MAP(ToolTipDialog, CExtResizableDialog)
END_MESSAGE_MAP()

BOOL ToolTipDialog::OnInitDialog()
{
    BOOL bResult = CDialog::OnInitDialog();
    m_wndToolTip.Create(this);
    m_wndToolTip.Activate(appState->_fShowToolTips);
    CWnd *pWndChild = GetWindow(GW_CHILD);
    CString strToolTip;
    while (pWndChild)
    {
        int nID = pWndChild->GetDlgCtrlID();
        if (strToolTip.LoadString(nID))
        {
            m_wndToolTip.AddTool(pWndChild, strToolTip);
        }
        pWndChild = pWndChild->GetWindow(GW_HWNDNEXT);
    }
    return bResult;
}

BOOL ToolTipDialog::PreTranslateMessage(MSG *pMsg)
{
    if (appState->_fShowToolTips &&
        pMsg->message >= WM_MOUSEFIRST &&
        pMsg->message <= WM_MOUSELAST)
    {
        MSG msg;
        ::CopyMemory(&msg, pMsg, sizeof(MSG));
        HWND hWndParent = ::GetParent(msg.hwnd);
        while (hWndParent && hWndParent != m_hWnd)
        {
            msg.hwnd = hWndParent;
            hWndParent = ::GetParent(hWndParent);
        }
        if (msg.hwnd)
        {
            m_wndToolTip.RelayEvent(&msg);
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}