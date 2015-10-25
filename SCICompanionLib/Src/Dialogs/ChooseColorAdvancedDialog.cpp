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
// ChooseColorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ChooseColorAdvancedDialog.h"

CChooseColorAdvancedDialog::CChooseColorAdvancedDialog(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CChooseColorAdvancedDialog::IDD, pParent)
{
    _bChoice = 0;

    // 256 things here.
    m_wndStatic.ShowSelection(TRUE);

    // Fill palette.
    for (int color1 = 0; color1 < 16; color1++)
    {
        for (int color2 = 0; color2 < 16; color2++)
        {
            _allColorsPalette[color1 * 16 + color2].color1 = color1;
            _allColorsPalette[color1 * 16 + color2].color2 = color2;
        }
    }
    m_wndStatic.SetPalette(16, 16, _allColorsPalette, ARRAYSIZE(g_egaColors), g_egaColors);
}

CChooseColorAdvancedDialog::~CChooseColorAdvancedDialog()
{
}

EGACOLOR CChooseColorAdvancedDialog::GetColor()
{
    EGACOLOR color;
    color.color1 = _bChoice >> 4;
    color.color2 = _bChoice & 0x0f;
    return color;
}

void CChooseColorAdvancedDialog::SetColor(EGACOLOR color)
{
    _bChoice = (color.color1 << 4) | color.color2;
    m_wndStatic.SetSelection(_bChoice);
}

void CChooseColorAdvancedDialog::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
    ShowSizeGrip(FALSE);
    DDX_Control(pDX, IDC_CHOOSECOLORSTATIC, m_wndStatic);
    m_wndStatic.SetCallback(this);

    DDX_Control(pDX, IDC_DIALOGDESCRIPTION, m_wndDesc);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
}


BEGIN_MESSAGE_MAP(CChooseColorAdvancedDialog, CExtResizableDialog)
END_MESSAGE_MAP()


void CChooseColorAdvancedDialog::OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick)
{
    if (fLeftClick)
    {
        // We only have one child - it must have been it.
        this->_bChoice = bIndex;
        EndDialog(IDOK);
    }
}

