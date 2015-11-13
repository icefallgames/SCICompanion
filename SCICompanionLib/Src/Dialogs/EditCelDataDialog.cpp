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
#include "AppState.h"
#include "EditCelDataDialog.h"
#include "View.h"
#include "format.h"

using namespace std;

EditCelDataDialog::EditCelDataDialog(int16_t &priority, Cel &cel, CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(EditCelDataDialog::IDD, pParent),
    _cel(cel),
    _priority(priority)
{
}

EditCelDataDialog::~EditCelDataDialog()
{
}

void EditCelDataDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    ShowSizeGrip(FALSE);

    DDX_Control(pDX, IDC_EDITPRI, m_wndEditPriority);
    m_wndEditPriority.LimitText(5);
    m_wndEditPriority.SetWindowText(fmt::format("{}", _priority).c_str());
    DDX_Control(pDX, IDC_EDITX, m_wndEditX);
    m_wndEditX.LimitText(4);
    m_wndEditX.SetWindowText(fmt::format("{}", _cel.placement.x).c_str());
    DDX_Control(pDX, IDC_EDITY, m_wndEditY);
    m_wndEditY.LimitText(4);
    m_wndEditY.SetWindowText(fmt::format("{}", _cel.placement.y).c_str());

    // Visuals
    DDX_Control(pDX, IDOK, m_wndOk);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_STATIC1, m_wndLabel1);
    DDX_Control(pDX, IDC_STATIC2, m_wndLabel2);
    DDX_Control(pDX, IDC_STATIC3, m_wndLabel3);
}

BEGIN_MESSAGE_MAP(EditCelDataDialog, CDialog)
END_MESSAGE_MAP()

void EditCelDataDialog::OnOK()
{
    CString str;
    m_wndEditX.GetWindowText(str);
    _cel.placement.x = (int16_t)StrToInt(str);
    m_wndEditY.GetWindowText(str);
    _cel.placement.y = (int16_t)StrToInt(str);
    m_wndEditPriority.GetWindowText(str);
    _priority = (int16_t)StrToInt(str);
    __super::OnOK();
}