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

//#include "BaseColorDialog.h"
#include "ChooseColorStatic.h"
#include "ColorDialogCallback.h"

extern uint8_t g_nearestColorSource;

class NearestColorsDialog : public CExtResizableDialog
{
public:
    NearestColorsDialog(CWnd* pParent = NULL);  // standard constructor

                                               // IColorDialogCallback
    //void OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick);

    //void Init(const EGACOLOR *pPalette);
    //BYTE GetColor() { return _bChoice; }
    //void SetColor(BYTE bChoice) { _bChoice = bChoice; m_wndStatic.SetSelection(bChoice); }

    virtual ~NearestColorsDialog();

    // Dialog Data
    enum { IDD = IDD_DIALOGNEARESTCOLORS };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    //void OnClickShowIndices();

    DECLARE_MESSAGE_MAP()

    void _SyncColor();

    CChooseColorStatic m_wndStatic;

    void OnBnClickedDitherCheck();

    // Visuals
    CExtLabel m_wndDescription;
    CExtButton m_wndCancel;
    CExtCheckBox m_wndDither;

    void OnTimer(UINT_PTR nIDEvent);

    EGACOLOR _myPalette[8 * 2];
    RGBQUAD _myPaletteRGB[8 * 2];
};
