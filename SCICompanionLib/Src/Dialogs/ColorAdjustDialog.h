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
#include "PaletteOperations.h"
#include "resource.h"
#include "ColorButton.h"

struct PaletteComponent;

class ColorAdjustDialog : public CExtResizableDialog
{
public:
    ColorAdjustDialog(PaletteComponent &palette, IVGAPaletteDefinitionCallback *callback, bool *selection, CWnd* pParent = nullptr);  // standard constructor

    // Dialog Data
    enum { IDD = IDD_PALETTEADJUST };

protected:
    void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
    void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pWnd);
    void _SyncPalette();
    void _PushToEdit();

    DECLARE_MESSAGE_MAP()

    // Visuals
    CExtButton m_wndOk;
    CExtButton m_wndCancel;

    // Tint color
    CColorPickerButton m_wndColor;

    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;
    CExtLabel m_wndStatic3;
    CExtLabel m_wndStatic5;

    CExtEdit m_wndEditHue;
    CExtEdit m_wndEditSaturation;
    CExtEdit m_wndEditBrightness;
    CExtEdit m_wndEditTintStrength;

    CExtSliderWnd m_wndSliderHue;
    CExtSliderWnd m_wndSliderSaturation;
    CExtSliderWnd m_wndSliderBrightness;
    CExtSliderWnd m_wndSliderTintStrength;

    bool _initialized;
    PaletteComponent &_palette;
    PaletteComponent _paletteOriginal;
    bool *_selection;
    IVGAPaletteDefinitionCallback *_callback;

    int _hue;
    int _saturation;
    int _brightness;
    int _tintStrength;
    COLORREF _crTint;

public:
    afx_msg void OnEnKillfocusEdithue();
    afx_msg void OnEnKillfocusEditsaturation();
    afx_msg void OnEnKillfocusEditbrightness();
    afx_msg void OnBnClickedButtoncolor();
    afx_msg void OnEnKillfocusEdittintstrength();
};
