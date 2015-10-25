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
