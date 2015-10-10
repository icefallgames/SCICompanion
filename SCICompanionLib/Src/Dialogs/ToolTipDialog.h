#pragma once

#include "Resource.h"

class ToolTipDialog : public CExtResizableDialog
{
public:
    ToolTipDialog();
    ToolTipDialog(UINT id, CWnd* pParent = nullptr);   // standard constructor

    DECLARE_MESSAGE_MAP()

protected:
    BOOL OnInitDialog();
    BOOL PreTranslateMessage(MSG *pMsg);

    
    CToolTipCtrl m_wndToolTip;
};
