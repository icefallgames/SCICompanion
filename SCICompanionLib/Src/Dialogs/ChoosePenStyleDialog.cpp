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
#include "ChoosePenStyleDialog.h"
#include "PicCommandsCommon.h"

// CChoosePenStyleDialog dialog

CChoosePenStyleDialog::CChoosePenStyleDialog(CWnd* pParent /*=NULL*/)
	: CBaseColorDialog(CChoosePenStyleDialog::IDD, pParent)
{
    _pPalette = NULL;
    _bChoice = 0;

    // 32 things here.
    m_wndStatic.SetPalette(4, 8, nullptr, 0, nullptr);
    m_wndStatic.ShowSelection(TRUE);
}

CChoosePenStyleDialog::~CChoosePenStyleDialog()
{
}

void CChoosePenStyleDialog::SetPenStyle(const PenStyle *pPenStyle)
{
    _bChoice = IndexFromPatternInfo(pPenStyle);
    _iUseRandomNR = pPenStyle->fRandomNR ? 1 : 0; // This will be bound to the check box.
    m_wndStatic.SetSelection(_bChoice);
}

void CChoosePenStyleDialog::GetPenStyle(PenStyle *pPenStyle)
{
    PatternInfoFromIndex(_bChoice, pPenStyle);
    pPenStyle->fRandomNR = (_iUseRandomNR != 0);
}

void CChoosePenStyleDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseColorDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHOOSEBRUSHSTATIC, m_wndStatic);
    m_wndStatic.SetCallback(this);
    DDX_Check(pDX, IDC_USERANDOMPATTERNS, _iUseRandomNR);

    m_wndStatic.SetSelection(_bChoice);

    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_STATIC1, m_wndStatic1);
    DDX_Control(pDX, IDC_USERANDOMPATTERNS, m_wndCheck);
}


BEGIN_MESSAGE_MAP(CChoosePenStyleDialog, CBaseColorDialog)
    ON_WM_KEYDOWN()
    ON_WM_KILLFOCUS()
    ON_WM_ACTIVATE()
END_MESSAGE_MAP()


void CChoosePenStyleDialog::OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick)
{
    if (fLeftClick)
    {
        // We only have one child - it must have been it.
        _bChoice = bIndex;
        _fEnded = TRUE;

        // We need to call this manually, since we don't have an ok button.
        UpdateData(TRUE);

        EndDialog(IDOK);
    }
}

