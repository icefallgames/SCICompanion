#include "stdafx.h"
#include "resource.h"
#include "AudioProcessingSettingsUI.h"

struct NoisePreset
{
    NoiseSettings Settings;
    std::string Description;
};

NoiseSettings noop = { 0, 0, 0, 0, 0 };

NoisePreset noNoiseGate =
{
    { 0, 0, 0, -99, -99 }, "No noise gate"
};
NoisePreset minimalNoiseGate =
{
    { 15, 25, 25, -38, -42 }, "Minimal"
};
NoisePreset weakNoiseGate =
{
    { 15, 15, 25, -26, -32 }, "Weak"
};
NoisePreset strongNoiseGate =
{
    { 15, 25, 50, -18, -22}, "Strong"
};
NoisePreset moderateNoiseGate =
{
    { 15, 25, 50, -22, -28 }, "Moderate"
};

NoisePreset *noisePresets[] =
{
    &noNoiseGate,
    &minimalNoiseGate,
    &weakNoiseGate,
    &moderateNoiseGate,
    &strongNoiseGate
};

void AudioProcessingSettingsUI::DoDataExchangeHelper(CDataExchange* pDX)
{
    DDX_Text(pDX, IDC_EDIT_TRIMLEFT, _settings.TrimLeftMS);
    DDV_MinMaxInt(pDX, _settings.TrimLeftMS, 0, 1000);
    DDX_Text(pDX, IDC_EDIT_TRIMRIGHT, _settings.TrimRightMS);
    DDV_MinMaxInt(pDX, _settings.TrimRightMS, 0, 1000);

    DDX_Text(pDX, IDC_EDIT_ATTACK, _settings.Noise.AttackTimeMS);
    DDV_MinMaxInt(pDX, _settings.Noise.AttackTimeMS, 0, 300);
    DDX_Text(pDX, IDC_EDIT_RELEASE, _settings.Noise.ReleaseTimeMS);
    DDV_MinMaxInt(pDX, _settings.Noise.ReleaseTimeMS, 0, 300);
    DDX_Text(pDX, IDC_EDIT_HOLD, _settings.Noise.HoldTimeMS);
    DDV_MinMaxInt(pDX, _settings.Noise.HoldTimeMS, 0, 300);
    DDX_Text(pDX, IDC_EDIT_OPENTHR, _settings.Noise.OpenThresholdDB);
    DDV_MinMaxInt(pDX, _settings.Noise.OpenThresholdDB, -99, 0);
    DDX_Text(pDX, IDC_EDIT_CLOSETHR, _settings.Noise.CloseThresholdDB);
    DDV_MinMaxInt(pDX, _settings.Noise.CloseThresholdDB, -99, 0);

    DDX_Check(pDX, IDC_CHECK_AUTOGAIN, _settings.AutoGain);
    DDX_Check(pDX, IDC_CHECK_AUDIODITHER, _settings.AudioDither);
    DDX_Check(pDX, IDC_CHECK_DETECTSTARTEND, _settings.DetectStartEnd);
    DDX_Check(pDX, IDC_CHECK_COMPRESSION, _settings.Compression);

    // Visuals
    DDX_Control(pDX, IDC_STATIC_ATTACK, m_wndLabel1);
    DDX_Control(pDX, IDC_STATIC_RELEASE, m_wndLabel2);
    DDX_Control(pDX, IDC_STATIC_HOLD, m_wndLabel3);
    DDX_Control(pDX, IDC_STATIC_OPENTHR, m_wndLabel4);
    DDX_Control(pDX, IDC_STATIC_CLOSETHR, m_wndLabel5);
    DDX_Control(pDX, IDC_STATIC_TRIM, m_wndLabel6);
    DDX_Control(pDX, IDC_STATIC_PRESET, m_wndLabel6);
    DDX_Control(pDX, IDC_CHECK_AUTOGAIN, m_wndCheck1);
    DDX_Control(pDX, IDC_CHECK_AUDIODITHER, m_wndCheck3);
    DDX_Control(pDX, IDC_CHECK_DETECTSTARTEND, m_wndCheck2);
    DDX_Control(pDX, IDC_CHECK_COMPRESSION, m_wndCheck4);
    DDX_Control(pDX, IDC_COMBO_PRESET2, m_wndComboPreset);

    if (m_wndComboPreset.GetCount() == 0)
    {
        for (NoisePreset *preset : noisePresets)
        {
            m_wndComboPreset.AddString(preset->Description.c_str());
        }
    }
}

void AudioProcessingSettingsUI::OnCbnSelchangeComboPreset()
{
    int curSel = m_wndComboPreset.GetCurSel();
    if (curSel != CB_ERR)
    {
        _settings.Noise = noisePresets[curSel]->Settings;
    }
}
