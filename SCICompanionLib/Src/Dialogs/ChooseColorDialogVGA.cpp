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
#include "ChooseColorDialogVGA.h"
#include "PaletteOperations.h"

// CChooseColorDialogVGA dialog

CChooseColorDialogVGA::CChooseColorDialogVGA(const PaletteComponent &palette, CWnd* pParent)
    : CBaseColorDialog(CChooseColorDialogVGA::IDD, pParent), _palette(palette)
{
    _bChoice = 0;

    // 256 things here.
    m_wndStatic.SetPalette(16, 16, reinterpret_cast<const EGACOLOR*>(palette.Mapping), ARRAYSIZE(palette.Mapping), palette.Colors, false);
    m_wndStatic.ShowSelection(TRUE);
}

void CChooseColorDialogVGA::DoDataExchange(CDataExchange* pDX)
{
    CBaseColorDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHOOSECOLORSTATIC, m_wndStatic);
    m_wndStatic.SetCallback(this);

    // Visuals
    DDX_Control(pDX, IDC_DIALOGDESCRIPTION, m_wndDescription);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
}

BEGIN_MESSAGE_MAP(CChooseColorDialogVGA, CBaseColorDialog)
    ON_WM_KEYDOWN()
    ON_WM_KILLFOCUS()
    ON_WM_ACTIVATE()
END_MESSAGE_MAP()

void CChooseColorDialogVGA::OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick)
{
    if (fLeftClick)
    {
        // We only have one child - it must have been it.
        SetColor(bIndex);
        _fEnded = TRUE;
        EndDialog(IDOK);
    }
}

