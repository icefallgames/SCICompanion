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
#include "PaletteDefinitionCallback.h"
#include "Pic.h"
#include "PicDoc.h"

struct PicComponent;
struct PaletteComponent;

extern volatile int g_fChecked;
extern volatile int g_fPreview;

class CPaletteDefinitionDialog : public CExtResizableDialog, public IColorDialogCallback, public IVGAPaletteDefinitionCallback
{
public:
    CPaletteDefinitionDialog(IEGAPaletteDefinitionCallback &callback, PicComponent &pic, ptrdiff_t pos, uint8_t paletteNumber, CWnd* pParent = nullptr);  // standard constructor

    // IColorDialogCallback
    void OnColorClick(BYTE bIndex, int nID, BOOL fLeftClick);
    void OnColorHover(BYTE bIndex) {};

    // IVGAPaletteDefinitionCallback
    void OnVGAPaletteChanged() override;

	virtual ~CPaletteDefinitionDialog();

    bool GetChanged() { return _changed; }

// Dialog Data
	enum { IDD = IDD_DEFINEPALETTE2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    void OnTabChange(NMHDR *pnmhdr, LRESULT *ples);
    void OnCheckClick();
    void OnPreviewToggle();
    void OnAdvancedClick();
    void ApplyPreview();
    void ApplyChanges();
    void OnOK() override;
    BOOL GetWriteEntirePalette() { return g_fChecked; }
    EGACOLOR *GetCurrentPalettePtr() { return &_palette[_iCurPalette * 40]; }

	DECLARE_MESSAGE_MAP()

    CChooseColorStatic m_wndStaticPalette;
    CChooseColorStatic m_wndStaticColors;
    CTabCtrl m_wndTab;
    CExtCheckBox m_wndCheck;
    CExtCheckBox m_wndPreview;
    CExtButton m_wndAdjust;

    // Visuals
    CExtButton m_wndAdvanced;
    CExtButton m_wndOK;
    CExtButton m_wndCancel;

    EGACOLOR _palette[4 * 40];

    int _iCurPalette; // from 0 to 3
    BOOL _bSelection;

    IEGAPaletteDefinitionCallback &_callback;
    PicComponent &_pic;
    PicComponent _copy;
    ptrdiff_t _position;
    ViewPort _viewport;
    bool _changed;

    std::unique_ptr<PaletteComponent> paletteComponentEGA;

public:
    afx_msg void OnBnClickedButtonadjust();
};
