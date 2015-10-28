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

#include "BaseColorDialog.h"
#include "ChooseColorStatic.h"
#include "ColorDialogCallback.h"
// CChoosePenStyleDialog dialog
struct PenStyle;

class CChoosePenStyleDialog : public CBaseColorDialog, public IColorDialogCallback
{
public:
    CChoosePenStyleDialog(CWnd* pParent = NULL);  // standard constructor

    // IColorDialogCallback
    void OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick);

    void SetPenStyle(const PenStyle *pPenStyle);
    void GetPenStyle(PenStyle *pPenStyle);

	virtual ~CChoosePenStyleDialog();

// Dialog Data
	enum { IDD = IDD_DIALOGPATTERN };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

    CChooseBrushStatic m_wndStatic;
    int _iUseRandomNR;
    const EGACOLOR *_pPalette;
    BYTE _bChoice;

    CExtButton m_wndCancel;
    CExtLabel m_wndStatic1;
    CExtCheckBox m_wndCheck;
};
