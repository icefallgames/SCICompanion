#include "stdafx.h"
#include "ToolTipDialog.h"
#include "AppState.h"

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