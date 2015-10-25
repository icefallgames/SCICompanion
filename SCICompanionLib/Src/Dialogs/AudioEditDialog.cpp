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
#include "stdafx.h"
#include "resource.h"
#include "AudioEditDialog.h"
#include "AudioNegative.h"
#include "ResourceEntity.h"
#include "AudioProcessing.h"

// AudioEditDialog dialog

AudioEditDialog::AudioEditDialog(ResourceEntity &resource, CWnd* pParent /*=NULL*/)
    : AudioPlaybackUI<ToolTipDialog>(AudioEditDialog::IDD, pParent),
    _negative(std::make_unique<AudioNegativeComponent>(resource.GetComponent<AudioNegativeComponent>())),
    _audio(std::make_unique<AudioComponent>(resource.GetComponent<AudioComponent>())),
    _audioProcessingSettingsUI(_negative->Settings),
    _changed(false),
    m_wndWaveformFinal("Final"),
    m_wndWaveformNegative("Original recording"),
    _resource(resource)

{
}

AudioEditDialog::~AudioEditDialog()
{
}

BOOL AudioEditDialog::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();

    OnInitDialogHelper();
    return fRet;
}

void AudioEditDialog::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);
    ShowSizeGrip(FALSE);

    _audioProcessingSettingsUI.DoDataExchangeHelper(pDX);
    DoDataExchangeHelper(pDX);

    // Visuals
    DDX_Control(pDX, IDC_GROUP2, m_wndGroup2);
    DDX_Control(pDX, IDOK, m_wndOk);
    DDX_Control(pDX, IDCANCEL, m_wndCancel);
    DDX_Control(pDX, IDC_WAVEFORM, m_wndWaveformNegative);
    DDX_Control(pDX, IDC_WAVEFORM2, m_wndWaveformFinal);
    DDX_Control(pDX, IDC_BUTTON_PROCESS, m_wndButtonProcess);
    DDX_Control(pDX, IDC_RADIO_NEGATIVE, m_wndCheckNegative);
    DDX_Control(pDX, IDC_RADIO_FINAL, m_wndCheckFinal);
    DDX_Control(pDX, IDC_COMBO_BIT, m_wndComboBit);

    if (!pDX->m_bSaveAndValidate)
    {
        m_wndCheckFinal.SetCheck(BST_CHECKED);
        m_wndButtonProcess.SetIcon(IDI_REFRESH, 0, 0, 0, 16, 16);
        m_wndComboBit.SetCurSel(IsFlagSet(_audio->Flags, AudioFlags::SixteenBit) ? 1 : 0);

        m_wndWaveformNegative.SetResource(&_negative->Audio);

        m_wndWaveformFinal.SetResource(_audio.get());

        SetWaveformElement(&m_wndWaveformFinal);

        _UpdatePlayback();
    }
}

void AudioEditDialog::_UpdatePlayback()
{
    bool finalChecked = m_wndCheckFinal.GetCheck() == BST_CHECKED;
    if (finalChecked)
    {
        SetAudioComponents(_audio.get());
    }
    else
    {
        SetAudioComponents(&_negative->Audio);
    }
    SetWaveformElement(finalChecked ? &m_wndWaveformFinal : &m_wndWaveformNegative);
    m_wndWaveformFinal.SetSelected(finalChecked);
    m_wndWaveformNegative.SetSelected(!finalChecked);
}

BEGIN_MESSAGE_MAP(AudioEditDialog, AudioPlaybackUI<ToolTipDialog>)
    ON_BN_CLICKED(IDC_BUTTON_PROCESS, &AudioEditDialog::OnBnClickedButtonProcess)
    ON_BN_CLICKED(IDC_RADIO_NEGATIVE, &AudioEditDialog::OnBnClickedRadioNegative)
    ON_BN_CLICKED(IDC_RADIO_FINAL, &AudioEditDialog::OnBnClickedRadioFinal)
    ON_CBN_SELCHANGE(IDC_COMBO_BIT, &AudioEditDialog::OnCbnSelchangeComboBit)
    ON_CBN_SELCHANGE(IDC_COMBO_PRESET2, &AudioEditDialog::OnCbnSelchangeComboPreset)
END_MESSAGE_MAP()


// AudioEditDialog message handlers

void AudioEditDialog::OnOK()
{
    if (_changed)
    {
        // TODO: Audio playback needs to operate on a copy of AudioComponent, the one we'll be processing.
        // Also, AudioNegativeComponent needs to be a copy (since we modify settings).
        _resource.GetComponent<AudioComponent>() = *_audio;
        _resource.GetComponent<AudioNegativeComponent>() = *_negative;
    }
    __super::OnOK();
}


void AudioEditDialog::OnBnClickedButtonProcess()
{
    UpdateData(TRUE);   // So that audio processing settings gets updated from the UI.
    ProcessSound(*_negative, *_audio, _audio->Flags);
    m_wndWaveformFinal.SetResource(_audio.get());
    _UpdatePlayback();
    _changed = true;
}


void AudioEditDialog::OnBnClickedRadioNegative()
{
    m_wndCheckFinal.SetCheck(BST_UNCHECKED);
    _UpdatePlayback();
}


void AudioEditDialog::OnBnClickedRadioFinal()
{
    m_wndCheckNegative.SetCheck(BST_UNCHECKED);
    _UpdatePlayback();
}


void AudioEditDialog::OnCbnSelchangeComboBit()
{
    int curSel = m_wndComboBit.GetCurSel();
    if (curSel != CB_ERR)
    {
        if (curSel == 0)
        {
            _audio->Flags &= ~(AudioFlags::SixteenBit | AudioFlags::Signed);
        }
        else
        {
            _audio->Flags |= (AudioFlags::SixteenBit | AudioFlags::Signed);
        }
        // Process, so that the data in the audio matches the bit depth we just set.
        OnBnClickedButtonProcess();
    }
}

void AudioEditDialog::OnCbnSelchangeComboPreset()
{
    UpdateData(TRUE);
    _audioProcessingSettingsUI.OnCbnSelchangeComboPreset();
    UpdateData(FALSE);
}
