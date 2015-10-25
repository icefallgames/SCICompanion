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

#include "ChooseColorStatic.h"
#include "BaseColorDialog.h"
#include "ColorDialogCallback.h"
// CChoosePriConDialog dialog

class CChoosePriConDialog : public CBaseColorDialog, public IColorDialogCallback
{
public:
    CChoosePriConDialog(const PaletteComponent *optionalPalette, CWnd* pParent = NULL);   // standard constructor
	virtual ~CChoosePriConDialog();

    // IColorDialogCallback
    void OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick);
    void OnColorHover(BYTE bIndex);

    void SetCaptionID(UINT nID) { _nID = nID; }
    BYTE GetColor() { return _bChoice; }
    void SetColor(BYTE bChoice) { _bChoice = bChoice; m_wndStatic.SetSelection(bChoice); }

    // Provide a 16 entry text array here.
    void SetStatusText(const char **rgStatusText) { _rgStatusText = rgStatusText; }

// Dialog Data
	enum
    {
        IDD = IDD_DIALOGPRICON,
        IDDVGA = IDD_DIALOGCHOOSECOLORVGA
    };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

    CChooseColorStatic m_wndStatic;
    CExtLabel m_wndStaticCaption;
	//CStatic m_wndStaticCaption;
    CExtLabel m_wndStaticStatus;
	//CStatic m_wndStaticStatus;

    // Visuals
    CExtButton m_wndCancel;
	//CButton m_wndCancel;
    CExtCheckBox m_wndOption;
	//CButton m_wndOption;

    UINT _nID;
    uint8_t _bChoice;
    const char **_rgStatusText;
    bool _isVGA;
};
