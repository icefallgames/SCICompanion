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
#include "stdafx.h"
#include "ChooseColorStatic.h"
#include "OnionSkinSettingsDialog.h"
#include "format.h"
#include "AppState.h"

using namespace std;

OnionSkinSettingsDialog::OnionSkinSettingsDialog(IOnionSkinCallback &callback, CWnd* pParent)
    : CExtResizableDialog(OnionSkinSettingsDialog::IDD, pParent), _initialized(false), _callback(callback)
{
    _crTintLeft = appState->_onionLeftTint;
    _crTintRight = appState->_onionRightTint;
    _opacityLeft = (appState->_onionLeftTint & 0xFF000000) >> 24;
    _opacityRight = (appState->_onionRightTint & 0xFF000000) >> 24;
    _onTopLeft = appState->_onionLeftOnTop;
    _onTopRight = appState->_onionRightOnTop;
    _onionWrap = appState->_onionWrap;
}

void OnionSkinSettingsDialog::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);

    if (!_initialized)
    {
        // Visuals
        DDX_Control(pDX, IDC_STATICTINTSTRENGTHLEFT, m_wndStatic1);
        DDX_Control(pDX, IDC_STATICTINTSTRENGTHRIGHT, m_wndStatic2);
        DDX_Control(pDX, IDCANCEL, m_wndCancel);
        DDX_Control(pDX, IDC_BUTTONCOLORLEFT, m_wndColorLeft);
        DDX_Control(pDX, IDC_BUTTONCOLORRIGHT, m_wndColorRight);
        DDX_Control(pDX, IDC_STATIC1, m_wndGroup1);
        DDX_Control(pDX, IDC_STATIC2, m_wndGroup2);

        DDX_Control(pDX, IDC_CHECKWRAP, m_wndCheckWrap);
        m_wndCheckWrap.SetCheck(_onionWrap ? BST_CHECKED : BST_UNCHECKED);
        DDX_Control(pDX, IDC_CHECKONTOPLEFT, m_wndCheckOnTopLeft);
        m_wndCheckOnTopLeft.SetCheck(_onTopLeft ? BST_CHECKED : BST_UNCHECKED);
        DDX_Control(pDX, IDC_CHECKONTOPRIGHT, m_wndCheckOnTopRight);
        m_wndCheckOnTopRight.SetCheck(_onTopRight ? BST_CHECKED : BST_UNCHECKED);

        DDX_Control(pDX, IDC_EDITOPACITYLEFT, m_wndEditOpacityLeft);
        DDX_Control(pDX, IDC_EDITOPACITYRIGHT, m_wndEditOpacityRight);

        DDX_Control(pDX, IDC_SLIDEROPACITYLEFT, m_wndSliderOpacityLeft);
        m_wndSliderOpacityLeft.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        m_wndSliderOpacityLeft.SetRange(0, 255, TRUE);
        m_wndSliderOpacityLeft.SetPos(0);

        DDX_Control(pDX, IDC_SLIDEROPACITYRIGHT, m_wndSliderOpacityRight);
        m_wndSliderOpacityRight.SetStyle(CExtSliderWnd::e_style_t::ES_PROFUIS);
        m_wndSliderOpacityRight.SetRange(0, 255, TRUE);
        m_wndSliderOpacityRight.SetPos(0);

        _PushToEdit();

        _initialized = true;
    }

    DDX_Slider(pDX, IDC_SLIDEROPACITYLEFT, _opacityLeft);
    DDX_Slider(pDX, IDC_SLIDEROPACITYRIGHT, _opacityRight);

    m_wndColorLeft.SetColorAndState(RGBQUADFromCOLORREF(_crTintLeft), TRUE);
    m_wndColorRight.SetColorAndState(RGBQUADFromCOLORREF(_crTintRight), TRUE);
    
    _PushToEdit();
    _onionWrap = m_wndCheckWrap.GetCheck() == BST_CHECKED;
    _onTopLeft = m_wndCheckOnTopLeft.GetCheck() == BST_CHECKED;
    _onTopRight = m_wndCheckOnTopRight.GetCheck() == BST_CHECKED;

    // Sync the UI values to the global ones
    appState->_onionLeftTint = (_crTintLeft & 0x00FFFFFF) | (_opacityLeft << 24);
    appState->_onionRightTint = (_crTintRight & 0x00FFFFFF) | (_opacityRight << 24);
    appState->_onionLeftOnTop = _onTopLeft;
    appState->_onionRightOnTop = _onTopRight;
    appState->_onionWrap = _onionWrap;
    // And notify our caller so they can update the UI
    _callback.OnOnionSkinChanged();
}

BEGIN_MESSAGE_MAP(OnionSkinSettingsDialog, CExtResizableDialog)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_BUTTONCOLORLEFT, &OnionSkinSettingsDialog::OnBnClickedButtoncolorLeft)
    ON_BN_CLICKED(IDC_BUTTONCOLORRIGHT, &OnionSkinSettingsDialog::OnBnClickedButtoncolorRight)
    ON_BN_CLICKED(IDC_CHECKWRAP, &OnionSkinSettingsDialog::OnCheckClicked)
    ON_BN_CLICKED(IDC_CHECKONTOPLEFT, &OnionSkinSettingsDialog::OnCheckClicked)
    ON_BN_CLICKED(IDC_CHECKONTOPRIGHT, &OnionSkinSettingsDialog::OnCheckClicked)
    ON_EN_KILLFOCUS(IDC_EDITOPACITYLEFT, &OnionSkinSettingsDialog::OnEnKillfocusEditOpacityLeft)
    ON_EN_KILLFOCUS(IDC_EDITOPACITYRIGHT, &OnionSkinSettingsDialog::OnEnKillfocusEditOpacityRight)
END_MESSAGE_MAP()

void OnionSkinSettingsDialog::_PushToEdit()
{
    m_wndEditOpacityLeft.SetWindowText(fmt::format("{0}", m_wndSliderOpacityLeft.GetPos()).c_str());
    m_wndEditOpacityRight.SetWindowText(fmt::format("{0}", m_wndSliderOpacityRight.GetPos()).c_str());
}

int GetNumber(CExtEdit &wndEdit);

void OnionSkinSettingsDialog::_OnEnKillfocusEditOpacity(CExtSliderWnd &slider, CExtEdit &edit)
{
    slider.SetPos(GetNumber(edit));
    UpdateData(TRUE);
}
void OnionSkinSettingsDialog::OnEnKillfocusEditOpacityLeft()
{
    _OnEnKillfocusEditOpacity(m_wndSliderOpacityLeft, m_wndEditOpacityLeft);
}
void OnionSkinSettingsDialog::OnEnKillfocusEditOpacityRight()
{
    _OnEnKillfocusEditOpacity(m_wndSliderOpacityRight, m_wndEditOpacityRight);
}

void OnionSkinSettingsDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pWnd)
{
    UpdateData(TRUE);
}

void OnionSkinSettingsDialog::_OnBnClickedButtoncolorLeft(COLORREF &color)
{
    CExtColorDlg colorDialog(color, color);
    colorDialog.m_strCaption = "Choose tint color";
    if (IDOK == colorDialog.DoModal())
    {
        color = colorDialog.m_clrNew;
    }
    UpdateData(TRUE);
}

void OnionSkinSettingsDialog::OnBnClickedButtoncolorLeft()
{
    _OnBnClickedButtoncolorLeft(_crTintLeft);
}
void OnionSkinSettingsDialog::OnBnClickedButtoncolorRight()
{
    _OnBnClickedButtoncolorLeft(_crTintRight);
}
void OnionSkinSettingsDialog::OnCheckClicked()
{
    UpdateData(TRUE);
}

