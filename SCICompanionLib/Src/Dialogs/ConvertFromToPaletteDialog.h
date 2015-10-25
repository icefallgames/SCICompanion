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
#include "afxwin.h"

class ConvertFromToPaletteDialog : public CExtResizableDialog
{
public:
    ConvertFromToPaletteDialog(CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
    enum { IDD = IDD_CONVERTTOPALETTE };

    int From;
    int To;

protected:
    void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
    BOOL OnInitDialog() override;
    void OnOK() override;

    DECLARE_MESSAGE_MAP()
private:
    std::vector<int> _paletteChoices;

    CExtComboBox m_wndComboFrom;
    CExtComboBox m_wndComboTo;
    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;
    CExtButton m_wndOk;
    CExtButton m_wndCancel;
};
