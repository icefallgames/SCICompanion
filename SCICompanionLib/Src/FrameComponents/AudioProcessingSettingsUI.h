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
    CExtComboBox m_wndComboPreset;
};