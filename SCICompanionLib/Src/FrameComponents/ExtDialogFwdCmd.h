#pragma once

#include "ToolTipDialog.h"

class CExtDialogFwdCmd : public ToolTipDialog
{
public:
    CExtDialogFwdCmd(UINT nID, CWnd *pParent = NULL) : ToolTipDialog(nID, pParent)
    {
    }
    BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};