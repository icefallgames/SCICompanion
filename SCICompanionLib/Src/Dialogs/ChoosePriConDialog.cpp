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
// ChoosePriConDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ChoosePriConDialog.h"
#include "PaletteOperations.h"

// CChoosePriConDialog dialog

CChoosePriConDialog::CChoosePriConDialog(const PaletteComponent *optionalPalette, CWnd* pParent /*=NULL*/)
    : CBaseColorDialog(optionalPalette ? CChoosePriConDialog::IDDVGA : CChoosePriConDialog::IDD, pParent)
{
    // 16 things here (or 256)
    m_wndStatic.ShowSelection(TRUE);
    if (optionalPalette)
    {
        _isVGA = true;
        m_wndStatic.ShowIndices(FALSE);
        m_wndStatic.SetPalette(16, 16, reinterpret_cast<const EGACOLOR *>(optionalPalette->Mapping), ARRAYSIZE(optionalPalette->Mapping), optionalPalette->Colors, false);
    }
    else
    {
        _isVGA = false;
        m_wndStatic.ShowIndices(TRUE);
        m_wndStatic.SetPalette(2, 8, g_egaColorChooserPalette, ARRAYSIZE(g_egaColors), g_egaColors, true);// just provide a basic palette
    }
    _rgStatusText = NULL;
}

CChoosePriConDialog::~CChoosePriConDialog()
{
}

void CChoosePriConDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseColorDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHOOSECOLORSTATIC, m_wndStatic);
    m_wndStatic.SetCallback(this);
    DDX_Control(pDX, IDC_DIALOGDESCRIPTION, m_wndStaticCaption);

    DDX_Control(pDX, IDC_STATICSTATUS, m_wndStaticStatus);
    if (_rgStatusText == NULL)
    {
        m_wndStaticStatus.ShowWindow(SW_HIDE);
    }

    GetDlgItem(IDC_CHECK_OPTION)->ShowWindow(SW_HIDE);

    // Set the caption.
    TCHAR szBuf[MAX_PATH];
    LoadString(AfxGetResourceHandle(), _nID, szBuf, ARRAYSIZE(szBuf));
    m_wndStaticCaption.SetWindowText(szBuf);

    // Visuals
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
}


BEGIN_MESSAGE_MAP(CChoosePriConDialog, CBaseColorDialog)
END_MESSAGE_MAP()


// CChoosePriConDialog message handlers

void CChoosePriConDialog::OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick)
{
    if (fLeftClick)
    {
        // We only have one child - it must have been it.
        SetColor(bIndex);
        _fEnded = TRUE;
        UpdateData(TRUE);
        EndDialog(IDOK);
    }
}

void CChoosePriConDialog::OnColorHover(BYTE bIndex)
{
    if (_rgStatusText)
    {
        if (_isVGA && (bIndex < 16))
        {
            m_wndStaticStatus.SetWindowText(_rgStatusText[bIndex]);
        }
        else
        {
            m_wndStaticStatus.SetWindowText("");
        }
    }
}