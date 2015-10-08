#include "stdafx.h"
#include "resource.h"
#include "AudioEditDialog.h"
#include "AudioNegative.h"
#include "ResourceEntity.h"
#include "AudioProcessing.h"

// AudioEditDialog dialog

AudioEditDialog::AudioEditDialog(ResourceEntity &resource, CWnd* pParent /*=NULL*/)
    : AudioPlaybackUI<CExtResizableDialog>(AudioEditDialog::IDD, pParent),
    _audioProcessingSettingsUI(resource.GetComponent<AudioNegativeComponent>().Settings),
    _resource(resource),
    _changed(false)
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
    

    const AudioNegativeComponent *audioNegative = _resource.TryGetComponent<AudioNegativeComponent>();
    m_wndWaveformNegative.SetResource(&audioNegative->Audio);

    m_wndWaveformFinal.SetResource(_resource.TryGetComponent<AudioComponent>());

    SetWaveformElement(&m_wndWaveformFinal);

    SetAudioResource(&_resource);
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
    }
    __super::OnOK();
}


void AudioEditDialog::OnBnClickedButtonProcess()
{
    UpdateData(TRUE);   // So that audio processing settings gets updated from the UI.
    ProcessSound(_resource.GetComponent<AudioNegativeComponent>(), _resource.GetComponent<AudioComponent>());
    m_wndWaveformFinal.SetResource(_resource.TryGetComponent<AudioComponent>());
    _changed = true;
}
