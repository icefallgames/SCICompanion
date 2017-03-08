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
#include "ChooseColorDialog.h"


BOOL g_ShowIndices = FALSE;

// CChooseColorDialog dialog

CChooseColorDialog::CChooseColorDialog(CWnd* pParent /*=NULL*/)
	: CBaseColorDialog(CChooseColorDialog::IDD, pParent)
{
    _pPalette = NULL;
    _bChoice = 0;

    // 40 things here.
    m_wndStatic.SetPalette(5, 8, nullptr, 0, nullptr);
    m_wndStatic.ShowSelection(TRUE);
}

CChooseColorDialog::~CChooseColorDialog()
{
}



void CChooseColorDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseColorDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHOOSECOLORSTATIC, m_wndStatic);
    m_wndStatic.SetCallback(this);
    DDX_Control(pDX, IDC_CHECKINDICES, m_wndShowIndices);
    m_wndShowIndices.SetCheck(g_ShowIndices);

    // Visuals
    DDX_Control(pDX, IDC_DIALOGDESCRIPTION, m_wndDescription);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
}

void CChooseColorDialog::Init(const EGACOLOR *pPalette)
{
    _pPalette = pPalette;
    m_wndStatic.SetPalette(5, 8, _pPalette, ARRAYSIZE(g_egaColors), g_egaColors);
    m_wndStatic.ShowIndices(g_ShowIndices);
}

BEGIN_MESSAGE_MAP(CChooseColorDialog, CBaseColorDialog)
    ON_WM_KEYDOWN()
    ON_WM_KILLFOCUS()
    ON_WM_ACTIVATE()
    ON_BN_CLICKED(IDC_CHECKINDICES, OnClickShowIndices)
END_MESSAGE_MAP()

void CChooseColorDialog::OnClickShowIndices()
{
    g_ShowIndices = m_wndShowIndices.GetCheck() == BST_CHECKED;
    m_wndStatic.ShowIndices(g_ShowIndices);
    m_wndStatic.Invalidate(FALSE);
}


void CChooseColorDialog::OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick)
{
    if (fLeftClick)
    {
        // We only have one child - it must have been it.
        SetColor(bIndex);
        _fEnded = TRUE;
        EndDialog(IDOK);
    }
}

