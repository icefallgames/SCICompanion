#include "stdafx.h"
#include "resource.h"
#include "AudioProcessingSettingsUI.h"

void AudioProcessingSettingsUI::DoDataExchangeHelper(CDataExchange* pDX)
{
    DDX_Text(pDX, IDC_EDIT_TRIMLEFT, _settings.TrimLeftMS);
    DDV_MinMaxInt(pDX, _settings.TrimLeftMS, 0, 1000);
    DDX_Text(pDX, IDC_EDIT_TRIMRIGHT, _settings.TrimRightMS);
    DDV_MinMaxInt(pDX, _settings.TrimRightMS, 0, 1000);

    DDX_Text(pDX, IDC_EDIT_ATTACK, _settings.NoiseAttackTimeMS);
    DDV_MinMaxInt(pDX, _settings.NoiseAttackTimeMS, 0, 300);
    DDX_Text(pDX, IDC_EDIT_RELEASE, _settings.NoiseReleaseTimeMS);
    DDV_MinMaxInt(pDX, _settings.NoiseReleaseTimeMS, 0, 300);
    DDX_Text(pDX, IDC_EDIT_HOLD, _settings.NoiseHoldTimeMS);
    DDV_MinMaxInt(pDX, _settings.NoiseHoldTimeMS, 0, 300);
    DDX_Text(pDX, IDC_EDIT_OPENTHR, _settings.NoiseOpenThresholdDB);
    DDV_MinMaxInt(pDX, _settings.NoiseOpenThresholdDB, -99, 0);
    DDX_Text(pDX, IDC_EDIT_CLOSETHR, _settings.NoiseCloseThresholdDB);
    DDV_MinMaxInt(pDX, _settings.NoiseCloseThresholdDB, -99, 0);

    DDX_Check(pDX, IDC_CHECK_AUTOGAIN, _settings.AutoGain);
    DDX_Check(pDX, IDC_CHECK_DETECTSTARTEND, _settings.DetectStartEnd);

    // Visuals
    DDX_Control(pDX, IDC_STATIC_ATTACK, m_wndLabel1);
    DDX_Control(pDX, IDC_STATIC_RELEASE, m_wndLabel2);
    DDX_Control(pDX, IDC_STATIC_HOLD, m_wndLabel3);
    DDX_Control(pDX, IDC_STATIC_OPENTHR, m_wndLabel4);
    DDX_Control(pDX, IDC_STATIC_CLOSETHR, m_wndLabel5);
    DDX_Control(pDX, IDC_STATIC_TRIM, m_wndLabel6);
    DDX_Control(pDX, IDC_CHECK_AUTOGAIN, m_wndCheck1);
    DDX_Control(pDX, IDC_CHECK_DETECTSTARTEND, m_wndCheck2);
}