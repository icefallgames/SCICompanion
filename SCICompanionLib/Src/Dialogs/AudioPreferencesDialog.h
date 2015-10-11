#pragma once

#include "AudioProcessingSettingsUI.h"
#include "ToolTipDialog.h"

class AudioPreferencesDialog : public ToolTipDialog
{
public:
    AudioPreferencesDialog(CWnd* pParent = NULL);   // standard constructor
    virtual ~AudioPreferencesDialog();

    // Dialog Data
    enum { IDD = IDD_AUDIOPREFERENCES };

private:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnOK();
    BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

    // Visuals
    CExtGroupBox m_wndGroup2;

    // Audio:
    CExtButton m_wndOk;
    CExtButton m_wndCancel;

    CComboBox m_wndMIDIDevices;
    CExtLabel m_wndStatic1;
    CExtLabel m_wndStatic2;
    CExtButton m_wndButton1;
    CExtButton m_wndButton2;

    AudioProcessingSettingsUI _audioProcessingSettingsUI;

public:
    afx_msg void OnCbnSelchangeComboMididevice();
    afx_msg void OnBnClickedButtonRepackage();
    afx_msg void OnBnClickedButtonClearAudioCache();
    afx_msg void OnCbnSelchangeComboPreset();
};
