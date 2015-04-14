#pragma once

class CExtDialogFwdCmd : public CExtResizableDialog
{
public:
    CExtDialogFwdCmd(UINT nID, CWnd *pParent = NULL) : CExtResizableDialog(nID, pParent)
    {
    }
    BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};