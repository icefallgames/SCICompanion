/***************************************************************************
Copyright (c) 2017 Philip Fortier

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

#include "resource.h"
#include "ColorButton.h"
#include "OnionSkinSettingsCallback.h"

struct PaletteComponent;

class OnionSkinSettingsDialog : public CExtResizableDialog
{
public:
    OnionSkinSettingsDialog(IOnionSkinCallback &callback, CWnd* pParent = nullptr);  // standard constructor

    enum { IDD = IDD_ONIONSKINSETTINGS };

protected:
    void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
    void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pWnd);
    void _PushToEdit();

    DECLARE_MESSAGE_MAP()

    // Visuals
    CExtButton m_wndCancel;

    // Tint color
    CColorPickerButton m_wndColorLeft;
    CColorPickerButton m_wndColorRight;

    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;

    CExtEdit m_wndEditOpacityLeft;
    CExtEdit m_wndEditOpacityRight;

    CExtSliderWnd m_wndSliderOpacityLeft;
    CExtSliderWnd m_wndSliderOpacityRight;

    CExtCheckBox m_wndCheckOnTopLeft;
    CExtCheckBox m_wndCheckOnTopRight;
    CExtCheckBox m_wndCheckWrap;

    CExtGroupBox m_wndGroup1;
    CExtGroupBox m_wndGroup2;

    bool _initialized;

    BOOL _onTopLeft;
    BOOL _onTopRight;
    int _opacityLeft;
    int _opacityRight;
    COLORREF _crTintLeft;
    COLORREF _crTintRight;
    BOOL _onionWrap;

    IOnionSkinCallback &_callback;

public:
    void _OnBnClickedButtoncolorLeft(COLORREF &color);
    afx_msg void OnBnClickedButtoncolorLeft();
    afx_msg void OnBnClickedButtoncolorRight();
    afx_msg void OnCheckClicked();

    void _OnEnKillfocusEditOpacity(CExtSliderWnd &slider, CExtEdit &edit);
    afx_msg void OnEnKillfocusEditOpacityLeft();
    afx_msg void OnEnKillfocusEditOpacityRight();
};
