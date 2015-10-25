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

#include "AudioProcessingSettings.h"

struct AudioProcessingSettingsUI
{
public:
    AudioProcessingSettingsUI(AudioProcessingSettings &settings) : _settings(settings) {}

    void DoDataExchangeHelper(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnCbnSelchangeComboPreset();

private:
    AudioProcessingSettings &_settings;

    // Visuals
    CExtLabel m_wndLabel1;
    CExtLabel m_wndLabel2;
    CExtLabel m_wndLabel3;
    CExtLabel m_wndLabel4;
    CExtLabel m_wndLabel5;
    CExtLabel m_wndLabel6;
    CExtLabel m_wndLabel7;
    CExtCheckBox m_wndCheck1;
    CExtCheckBox m_wndCheck3;
    CExtCheckBox m_wndCheck2;
    CExtCheckBox m_wndCheck4;
    CExtComboBox m_wndComboPreset;
};