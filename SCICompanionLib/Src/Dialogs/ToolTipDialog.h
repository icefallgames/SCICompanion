#pragma once

#include "Resource.h"

class ToolTipDialog : public CExtResizableDialog
{
public:
    ToolTipDialog(UINT id, CWnd* pParent = nullptr);   // standard constructor

protected:
    BOOL OnInitDialog();
    BOOL PreTranslateMessage(MSG *pMsg);

    DECLARE_MESSAGE_MAP()

    CToolTipCtrl m_wndToolTip;
};
