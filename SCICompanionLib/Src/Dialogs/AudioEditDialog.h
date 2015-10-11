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
