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
#include "ConvertFromToPaletteDialog.h"
#include "format.h"

using namespace std;

ConvertFromToPaletteDialog::ConvertFromToPaletteDialog(CWnd* pParent /*=NULL*/)
    : CExtResizableDialog(ConvertFromToPaletteDialog::IDD, pParent), From(-1), To(-1)
{
}

void ConvertFromToPaletteDialog::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBOFROM, m_wndComboFrom);
    DDX_Control(pDX, IDC_COMBOTO, m_wndComboTo);

    DDX_Control(pDX, IDOK, m_wndOk);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_STATIC1, m_wndStatic1);
    DDX_Control(pDX, IDC_STATIC2, m_wndStatic2);
}

BOOL ConvertFromToPaletteDialog::OnInitDialog()
{
    __super::OnInitDialog();

    _paletteChoices = appState->GetResourceMap().GetPaletteList();
    for (int choice : _paletteChoices)
    {
        string paletteNumber = fmt::format("{0}", choice);
        m_wndComboFrom.InsertString(-1, paletteNumber.c_str());
        m_wndComboTo.InsertString(-1, paletteNumber.c_str());
    }

    return TRUE;
}

void ConvertFromToPaletteDialog::OnOK()
{
    int fromIndex = m_wndComboFrom.GetCurSel();
    int toIndex = m_wndComboTo.GetCurSel();
    if ((fromIndex != CB_ERR) && (toIndex != CB_ERR) && (fromIndex != toIndex))
    {
        From = _paletteChoices[fromIndex];
        To = _paletteChoices[toIndex];
        __super::OnOK();
    }
    else
    {
        AfxMessageBox("Two different palette choices need to be made.", MB_OK | MB_ICONWARNING);
    }
}

BEGIN_MESSAGE_MAP(ConvertFromToPaletteDialog, CExtResizableDialog)
END_MESSAGE_MAP()

