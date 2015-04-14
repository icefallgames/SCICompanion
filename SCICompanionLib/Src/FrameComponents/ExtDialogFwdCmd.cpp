#include "stdafx.h"
#include "ExtDialogFwdCmd.h"

BOOL CExtDialogFwdCmd::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    BOOL fRet = __super::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
    if (!fRet)
    {
        CFrameWnd *pFrame = GetTopLevelFrame();
        if (pFrame)
        {
            CFrameWnd *pFrameActive = pFrame->GetActiveFrame();
            if (pFrameActive)
            {
                fRet = pFrameActive->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
            }
        }
    }
    return fRet;
}