#include "stdafx.h"
#include "resource.h"
#include "AudioEditDialog.h"
#include "AudioNegative.h"
#include "ResourceEntity.h"
#include "AudioProcessing.h"

// AudioEditDialog dialog

AudioEditDialog::AudioEditDialog(ResourceEntity &resource, CWnd* pParent /*=NULL*/)
    : AudioPlaybackUI<CExtResizableDialog>(AudioEditDialog::IDD, pParent),
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
    

    m_wndWaveformNegative.SetResource(&_negative->Audio);

    m_wndWaveformFinal.SetResource(_audio.get());

    SetWaveformElement(&m_wndWaveformFinal);

    _UpdatePlayback();
}

void AudioEditDialog::_UpdatePlayback()
{
    // TODO: Choose based on radio
    SetAudioResource(_audio.get());
}

BEGIN_MESSAGE_MAP(AudioEditDialog, AudioPlaybackUI<CExtResizableDialog>)
    ON_BN_CLICKED(IDC_BUTTON_PROCESS, &AudioEditDialog::OnBnClickedButtonProcess)
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
    ProcessSound(*_negative, *_audio);
    m_wndWaveformFinal.SetResource(_audio.get());
    _UpdatePlayback();
    _changed = true;
}
