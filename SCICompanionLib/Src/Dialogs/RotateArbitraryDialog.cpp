/***************************************************************************
Copyright (c) 2017 Philip Fortier

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
***************************************************************************/
// RotateArbitraryDialog.cpp : implementation file
//

#include "stdafx.h"
#include "RotateArbitraryDialog.h"
#include "format.h"

using namespace std;

RotateArbitraryDialog::RotateArbitraryDialog(int degrees, CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(RotateArbitraryDialog::IDD, pParent), _degrees(degrees)
{
}

void RotateArbitraryDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    ShowSizeGrip(FALSE);

    DDX_Control(pDX, IDC_EDITDEGREES, m_wndEditDegrees);
    m_wndEditDegrees.LimitText(3);
    m_wndEditDegrees.SetWindowText(fmt::format("{0}", _degrees).c_str());

    // Visuals
    DDX_Control(pDX, IDOK, m_wndOk);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_STATIC1, m_wndLabel1);
}

void RotateArbitraryDialog::OnOK()
{
    CString str;
    m_wndEditDegrees.GetWindowText(str);
    _degrees = StrToInt(str);
    __super::OnOK();
}