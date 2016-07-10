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
#include "ImageUtil.h"
#include <format.h>

EGACOLOR egaColorWheel[] =
{
    { 4, 7 },
    { 6, 7 },
    { 6, 7 },
    { 6, 7 },
    { 2, 7 },
    { 3, 6 },
    { 3, 6 },
    { 3, 6 },
    { 3, 7 },
    { 3, 7 },
    { 1, 7 },
    { 3, 5 },
    { 3, 5 },
    { 5, 7 },
    { 5, 7 },
    { 5, 7 },
    { 4, 15 },
    { 6, 15 },
    { 6, 15 },
    { 6, 15 },
    { 6, 15 },
    { 2, 15 },
    { 2, 15 },
    { 3, 15 },
    { 10, 13 },
    { 10, 13 },
    { 1, 15 },
    { 1, 15 },
    { 1, 15 },
    { 5, 15 },
    { 5, 15 },
    { 5, 15 },
    { 4, 8 },
    { 6, 8 },
    { 6, 8 },
    { 6, 8 },
    { 6, 8 },
    { 2, 8 },
    { 2, 8 },
    { 2, 8 },
    { 3, 8 },
    { 3, 8 },
    { 3, 8 },
    { 1, 8 },
    { 1, 6 },
    { 5, 8 },
    { 5, 6 },
    { 5, 6 },
    { 8, 12 },
    { 8, 12 },
    { 8, 14 },
    { 10, 12 },
    { 10, 12 },
    { 10, 12 },
    { 10, 12 },
    { 10, 12 },
    { 8, 11 },
    { 8, 11 },
    { 8, 11 },
    { 8, 9 },
    { 6, 9 },
    { 9, 12 },
    { 8, 13 },
    { 8, 13 },
    { 3, 12 },
    { 3, 12 },
    { 3, 12 },
    { 1, 14 },
    { 1, 14 },
    { 1, 14 },
    { 5, 10 },
    { 6, 11 },
    { 4, 11 },
    { 4, 11 },
    { 4, 11 },
    { 4, 11 },
    { 4, 11 },
    { 2, 13 },
    { 2, 13 },
    { 2, 13 },
    { 0, 12 },
    { 2, 12 },
    { 2, 12 },
    { 0, 14 },
    { 4, 10 },
    { 4, 10 },
    { 4, 10 },
    { 4, 10 },
    { 0, 11 },
    { 2, 9 },
    { 2, 9 },
    { 0, 9 },
    { 4, 9 },
    { 0, 13 },
    { 0, 13 },
    { 1, 12 },
    { 0, 12 },
    { 0, 12 },
    { 4, 14 },
    { 6, 10 },
    { 6, 10 },
    { 6, 10 },
    { 8, 10 },
    { 8, 10 },
    { 9, 10 },
    { 9, 10 },
    { 9, 10 },
    { 9, 10 },
    { 9, 10 },
    { 1, 13 },
    { 0, 13 },
    { 0, 13 },
    { 4, 12 },
    { 6, 12 },
    { 6, 14 },
    { 2, 14 },
    { 2, 14 },
    { 2, 14 },
    { 2, 14 },
    { 2, 11 },
    { 3, 11 },
    { 3, 9 },
    { 3, 9 },
    { 1, 9 },
    { 5, 9 },
    { 5, 13 },
    { 4, 13 },
    { 5, 12 },
    { 4, 12 },
    { 4, 12 },
    { 4, 12 },
    { 4, 12 },
    { 4, 12 },
    { 7, 10 },
    { 7, 10 },
    { 7, 10 },
    { 1, 11 },
    { 1, 11 },
    { 1, 11 },
    { 1, 11 },
    { 1, 11 },
    { 6, 13 },
    { 6, 13 },
    { 6, 13 },
    { 7, 12 },
    { 7, 12 },
    { 5, 14 },
    { 7, 14 },
    { 3, 14 },
    { 3, 14 },
    { 3, 14 },
    { 3, 14 },
    { 7, 11 },
    { 5, 11 },
    { 5, 11 },
    { 7, 9 },
    { 3, 13 },
    { 7, 13 },
    { 7, 13 },
    { 7, 13 },
    { 0, 4 },
    { 0, 6 },
    { 0, 6 },
    { 0, 6 },
    { 0, 6 },
    { 0, 2 },
    { 0, 2 },
    { 2, 3 },
    { 0, 3 },
    { 0, 3 },
    { 0, 3 },
    { 0, 3 },
    { 0, 3 },
    { 0, 5 },
    { 0, 5 },
    { 0, 5 },
    { 4, 4 },
    { 4, 6 },
    { 6, 6 },
    { 2, 4 },
    { 2, 6 },
    { 2, 2 },
    { 0, 10 },
    { 1, 10 },
    { 1, 2 },
    { 1, 3 },
    { 0, 1 },
    { 1, 1 },
    { 1, 5 },
    { 1, 4 },
    { 4, 5 },
    { 4, 5 },
    { 12, 12 },
    { 12, 12 },
    { 12, 12 },
    { 12, 12 },
    { 2, 10 },
    { 2, 10 },
    { 10, 10 },
    { 3, 10 },
    { 3, 3 },
    { 9, 11 },
    { 9, 11 },
    { 9, 11 },
    { 9, 11 },
    { 5, 5 },
    { 12, 13 },
    { 12, 13 },
    { 12, 12 },
    { 12, 12 },
    { 12, 14 },
    { 14, 14 },
    { 10, 14 },
    { 10, 14 },
    { 10, 14 },
    { 10, 11 },
    { 11, 11 },
    { 9, 11 },
    { 9, 11 },
    { 9, 9 },
    { 9, 13 },
    { 13, 13 },
    { 12, 13 },
    { 12, 13 },
    { 13, 14 },
    { 12, 15 },
    { 12, 15 },
    { 14, 15 },
    { 14, 15 },
    { 11, 14 },
    { 10, 15 },
    { 11, 15 },
    { 11, 15 },
    { 11, 15 },
    { 9, 15 },
    { 11, 13 },
    { 11, 13 },
    { 13, 15 },
    { 13, 15 },
    { 13, 15 },
    { 0, 0 },
    { 0, 8 },
    { 8, 8 },
    { 8, 8 },
    { 0, 7 },
    { 2, 5 },
    { 3, 4 },
    { 7, 8 },
    { 7, 7 },
    { 7, 7 },
    { 0, 15 },
    { 8, 15 },
    { 9, 14 },
    { 11, 12 },
    { 7, 15 },
    { 15, 15 },
};

RGBQUAD dummy[256] = {}; // Unneeded for rendering, since we're dithering.

CChooseColorAdvancedDialog::CChooseColorAdvancedDialog(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CChooseColorAdvancedDialog::IDD, pParent)
{
    _bChoice = 0;
    // 256 things here.
    m_wndStatic.ShowSelection(TRUE);
    m_wndStatic.SetPalette(16, 16, egaColorWheel, ARRAYSIZE(egaColorWheel), dummy);
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
    // Find the best matching one in the colorwheel.
    for (int i = 0; i < ARRAYSIZE(egaColorWheel); i++)
    {
        if ((color.color1 == egaColorWheel[i].color1) &&
            (color.color2 == egaColorWheel[i].color2))
        {
            _bChoice = (byte)i;
            break;
        }
        if ((color.color1 == egaColorWheel[i].color2) &&
            (color.color2 == egaColorWheel[i].color1))
        {
            // Almost the same, but colors are flipped... keep looking.
            _bChoice = (byte)i;
        }
    }

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
        this->_bChoice = EGACOLOR_TO_BYTE(egaColorWheel[bIndex]);
        EndDialog(IDOK);
    }
}

