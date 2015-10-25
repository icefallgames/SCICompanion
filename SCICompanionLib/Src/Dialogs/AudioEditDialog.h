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

#include "AudioProcessingSettingsUI.h"
#include "AudioWaveformUI.h"
#include "AudioPlaybackUI.h"
#include "ToolTipDialog.h"

class ResourceEntity;

class AudioEditDialog : public AudioPlaybackUI<ToolTipDialog>
{
public:
    AudioEditDialog(ResourceEntity &resource, CWnd* pParent = NULL);   // standard constructor
    virtual ~AudioEditDialog();

    // Dialog Data
    enum { IDD = IDD_AUDIOEDIT };

protected:
    BOOL OnInitDialog() override;

private:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();

    void _UpdatePlayback();

    DECLARE_MESSAGE_MAP()

    // Visuals
    CExtGroupBox m_wndGroup2;
    CExtButton m_wndOk;
    CExtButton m_wndCancel;
    CExtButton m_wndButtonProcess;
    CExtRadioButton m_wndCheckNegative, m_wndCheckFinal;
    CExtComboBox m_wndComboBit;

    AudioWaveformUI m_wndWaveformNegative;
    AudioWaveformUI m_wndWaveformFinal;

    std::unique_ptr<AudioComponent> _audio;
    std::unique_ptr<AudioNegativeComponent> _negative;
    ResourceEntity &_resource;

    bool _changed;

    AudioProcessingSettingsUI _audioProcessingSettingsUI;

public:
    afx_msg void OnBnClickedButtonProcess();
    afx_msg void OnBnClickedRadioNegative();
    afx_msg void OnBnClickedRadioFinal();
    afx_msg void OnCbnSelchangeComboBit();
    afx_msg void OnCbnSelchangeComboPreset();
};
