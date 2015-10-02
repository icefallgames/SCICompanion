#pragma once

#include "Audio.h"
#include "AudioPlayback.h"
#include "AudioRecording.h"
#include "SoundUtil.h"
#include "Sync.h"
#include "ViewUIElement.h"
#include "AudioWaveformUI.h"

enum class WaveRecordingFormat;
class ViewUIElement;

template<typename T>
class AudioPlaybackUI : public T
{
public:
    AudioPlaybackUI();
    AudioPlaybackUI(int id, CWnd *parent);
    virtual ~AudioPlaybackUI();
    void SetAudioResource(const ResourceEntity *audio);
    void SetMouthElement(ViewUIElement *_pwndMouth);
    void SetWaveformElement(AudioWaveformUI *_pwndWaveform);

protected:
    // Call these things:
    void DoDataExchangeHelper(CDataExchange* pDX);    // DDX/DDV support
    void OnInitDialogHelper();
    void AutomaticStop();

    virtual void OnNewResourceCreated(std::unique_ptr<ResourceEntity> audioResource, const std::string &name) {};

protected:
    afx_msg void OnTimer(UINT_PTR nIDEvent);


private:
    afx_msg void OnPlay();
    afx_msg void OnStop();
    afx_msg void OnRecord();
    afx_msg void OnBrowse();
    afx_msg void OnExport();
    void _UpdatePlayState();
    DECLARE_MESSAGE_MAP()
    bool _IsPlaying();
    bool _IsRecording();

    CExtEdit m_wndInfo;
    CExtButton m_wndPlay;
    CExtButton m_wndStop;
    CExtButton m_wndRecord;
    CExtLabel m_wndStaticRec;
    CExtCheckBox m_wndHalfSpeed;
    CExtSliderWnd m_wndSlider;
    CExtCheckBox m_wndAutoPreview;
    CExtLabel m_wndDuration;
    CExtEdit m_wndDescription;
    CExtButton m_wndBrowse;
    CExtButton m_wndExport;
    CExtLabel m_wndTitle;
    CExtComboBox m_wndWaveType;

    ViewUIElement *_pwndMouth;
    const ResourceEntity *_audio;

    AudioWaveformUI *_pwndWaveform;

    AudioPlayback _audioPlayback;

    int _recordingFormat;
    CFont _marlettFont;
};



//
// Audio previewer
//
BEGIN_TEMPLATE_MESSAGE_MAP(AudioPlaybackUI, T, T)
    ON_BN_CLICKED(IDC_BUTTON_PLAY2, OnPlay)
    ON_BN_CLICKED(IDC_BUTTON_STOP, OnStop)
    ON_BN_CLICKED(IDC_BUTTON_RECORD, OnRecord)
    ON_BN_CLICKED(IDC_BUTTONBROWSE, OnBrowse)
    ON_BN_CLICKED(IDC_BUTTON_EXPORT, OnExport)
    ON_WM_TIMER()
END_MESSAGE_MAP()

#define AUDIO_TIMER 5004

template <typename T>
AudioPlaybackUI<T>::AudioPlaybackUI() : _recordingFormat((int)g_WaveRecordingFormat)
{
}

template <typename T>
AudioPlaybackUI<T>::~AudioPlaybackUI()
{
    g_WaveRecordingFormat = (WaveRecordingFormat)_recordingFormat;
}

template <typename T>
AudioPlaybackUI<T>::AudioPlaybackUI(int id, CWnd *parent) : T(id, parent), _recordingFormat((int)g_WaveRecordingFormat)
{
}

template <typename T>
void AudioPlaybackUI<T>::SetMouthElement(ViewUIElement *pwndMouth)
{
    _pwndMouth = pwndMouth;
}

template <typename T>
void AudioPlaybackUI<T>::SetWaveformElement(AudioWaveformUI *pwndWaveform)
{
    _pwndWaveform = pwndWaveform;
}

template <typename T>
void AudioPlaybackUI<T>::SetAudioResource(const ResourceEntity *audio)
{
    _audio = audio;

    std::string durationString = "Duration: ";
    std::string info;
    if (_audio)
    {
        const AudioComponent *audioComp = _audio->TryGetComponent<AudioComponent>();
        if (audioComp)
        {
            durationString += GetAudioLength(*audioComp);
            _audioPlayback.SetAudio(audioComp);
            // Put some information in the channels window
            info = fmt::format("{0}Hz, {1} bytes. {2} {3}",
                audioComp->Frequency,
                audioComp->GetLength(),
                IsFlagSet(audioComp->Flags, AudioFlags::SixteenBit) ? "16-bit" : "8-bit",
                IsFlagSet(audioComp->Flags, AudioFlags::DPCM) ? "DPCM" : "");
        }

        if (m_wndAutoPreview && (m_wndAutoPreview.GetCheck() == BST_CHECKED))
        {
            OnPlay();
        }
        else
        {
            OnStop();
        }
    }
    else
    {
        OnStop();
    }
    m_wndInfo.SetWindowTextA(info.c_str());
    m_wndDuration.SetWindowText(durationString.c_str());

}

template <typename T>
void AudioPlaybackUI<T>::DoDataExchangeHelper(CDataExchange* pDX)
{
    DDX_Control(pDX, IDC_EDIT_SAMPLEBIT, m_wndInfo);
    DDX_Control(pDX, IDC_BUTTON_PLAY2, m_wndPlay);
    if (GetDlgItem(IDC_CHECK_HALFSPEED))
    {
        DDX_Control(pDX, IDC_CHECK_HALFSPEED, m_wndHalfSpeed);
    }
    if (GetDlgItem(IDC_BUTTON_RECORD))
    {
        DDX_Control(pDX, IDC_BUTTON_RECORD, m_wndRecord);
        DDX_Control(pDX, IDC_COMBO_WAVEFORMAT, m_wndWaveType);
        DDX_CBIndex(pDX, IDC_COMBO_WAVEFORMAT, _recordingFormat);
    }
    DDX_Control(pDX, IDC_BUTTON_STOP, m_wndStop);
    if (GetDlgItem(IDC_SLIDER))
    {
        DDX_Control(pDX, IDC_SLIDER, m_wndSlider);
    }
    if (GetDlgItem(IDC_CHECK_AUTOPREV))
    {
        DDX_Control(pDX, IDC_CHECK_AUTOPREV, m_wndAutoPreview);
    }
    DDX_Control(pDX, IDC_STATIC_DURATION, m_wndDuration);
    if (GetDlgItem(IDC_BUTTONBROWSE))
    {
        DDX_Control(pDX, IDC_BUTTONBROWSE, m_wndBrowse);
    }
    if (GetDlgItem(IDC_BUTTON_EXPORT))
    {
        DDX_Control(pDX, IDC_BUTTON_EXPORT, m_wndExport);
    }

    if (this->GetDlgItem(IDC_EDIT_DESCRIPTION))
    {
        DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_wndDescription);
        m_wndDescription.SetWindowText("To add audio resources to the game, drag .wav files into the view. Files should be uncompressed 8 or 16 bit audio (8 bit is best for loops), 22050Hz or less.\r\nSCI only supports monaural sounds. The left channel will be used for stereo .wav files.");
    }

    if (this->GetDlgItem(IDC_EDIT_DESCRIPTION))
    {
        DDX_Control(pDX, IDC_STATICTITLE, m_wndTitle);
    }
}

// A stop caused programatically, not by the user pressing stop.
template <typename T>
void AudioPlaybackUI<T>::AutomaticStop()
{
    _audioPlayback.Stop();
    _UpdatePlayState();
}

template <typename T>
void AudioPlaybackUI<T>::OnInitDialogHelper()
{
    CRect rc;
    GetClientRect(&rc);

    CDC *pDC = GetDC();
    {
        LOGFONT logFont = { 0 };
        StringCchCopy(logFont.lfFaceName, ARRAYSIZE(logFont.lfFaceName), "Marlett");
        logFont.lfHeight = -MulDiv(10, GetDeviceCaps((HDC)*pDC, LOGPIXELSY), 72);
        logFont.lfWeight = FW_NORMAL;
        logFont.lfItalic = FALSE;
        logFont.lfCharSet = DEFAULT_CHARSET;
        logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
        logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
        logFont.lfQuality = DEFAULT_QUALITY;
        logFont.lfPitchAndFamily = FIXED_PITCH;
        _marlettFont.CreateFontIndirect(&logFont);
        m_wndPlay.SetFont(&_marlettFont);
        m_wndStop.SetFont(&_marlettFont);
        if (m_wndRecord)
        {
            m_wndRecord.SetFont(&_marlettFont);
        }
    }

    if (m_wndSlider)
    {
        m_wndSlider.SetRange(0, 100);
    }

    _UpdatePlayState();

    ReleaseDC(pDC);
}

template <typename T>
bool AudioPlaybackUI<T>::_IsPlaying()
{
    return _audioPlayback.IsPlaying();
}

template <typename T>
bool AudioPlaybackUI<T>::_IsRecording()
{
    return g_audioRecording.IsRecording();
}

template <typename T>
void AudioPlaybackUI<T>::_UpdatePlayState()
{
    if (_IsPlaying() || _IsRecording())
    {
        SetTimer(AUDIO_TIMER, 33, NULL);
    }
    else
    {
        KillTimer(AUDIO_TIMER);
    }
    m_wndPlay.EnableWindow(_audio && !_IsPlaying());
    m_wndStop.EnableWindow(_IsRecording() || (_audio && _IsPlaying()));
}

template <typename T>
void AudioPlaybackUI<T>::OnPlay()
{
    const AudioComponent *audio = _audio->TryGetComponent<AudioComponent>();
    if (audio && (audio->Frequency != 0))
    {
        int slowDown = 1;
        if (m_wndHalfSpeed && (m_wndHalfSpeed.GetCheck() == BST_CHECKED))
        {
            slowDown = 2;
        }
        _audioPlayback.Stop();
        _audioPlayback.Play(slowDown);
        _UpdatePlayState();
        if (_pwndWaveform)
        {
            _pwndWaveform->SetStreamPosition(0);
        }
    }
}

template <typename T>
void AudioPlaybackUI<T>::OnRecord()
{
    _audioPlayback.Stop();
    int curSel = m_wndWaveType.GetCurSel();
    if (curSel != CB_ERR)
    {
        _recordingFormat = curSel;
    }
    g_audioRecording.Record((WaveRecordingFormat)_recordingFormat);
    _UpdatePlayState();
}

template <typename T>
void AudioPlaybackUI<T>::OnBrowse()
{
    CFileDialog fileDialog(TRUE, nullptr, nullptr, 0, "WAV files (*.wav)|*.wav|All Files|*.*|");
    fileDialog.m_ofn.lpstrTitle = "Add wav to game";
    if (IDOK == fileDialog.DoModal())
    {
        std::string filename = (PCSTR)fileDialog.GetPathName();
        try
        {
            OnNewResourceCreated(WaveResourceFromFilename(filename), _NameFromFilename(filename.c_str()));
        }
        catch (std::exception &e)
        {
            AfxMessageBox(e.what(), MB_OK | MB_ICONWARNING);
        }
    }
}

template <typename T>
void AudioPlaybackUI<T>::OnExport()
{
    CFileDialog fileDialog(FALSE, ".wav", nullptr, 0, "WAV files (*.wav)|*.wav|All Files|*.*|");
    fileDialog.m_ofn.lpstrTitle = "Export as .wav";
    if (IDOK == fileDialog.DoModal())
    {
        std::string filename = (PCSTR)fileDialog.GetPathName();
        try
        {
            WriteWaveFile(filename, _audio->GetComponent<AudioComponent>());
        }
        catch (std::exception &e)
        {
            AfxMessageBox(e.what(), MB_OK | MB_ICONWARNING);
        }
    }
}

template <typename T>
void AudioPlaybackUI<T>::OnStop()
{
    _audioPlayback.Stop();

    // If we're recording notify about it.
    std::unique_ptr<AudioComponent> newAudio = g_audioRecording.Stop();
    if (newAudio)
    {
        std::unique_ptr<ResourceEntity> resource(CreateDefaultAudioResource(appState->GetVersion()));
        resource->RemoveComponent<AudioComponent>();
        resource->AddComponent(std::move(newAudio));
        OnNewResourceCreated(std::move(resource), "recording");
        assert(_audio); // They should have set it back on us!
    }

    _UpdatePlayState();
}

template <typename T>
void AudioPlaybackUI<T>::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == AUDIO_TIMER)
    {
        if (_audioPlayback.IsPlaying())
        {
            if (m_wndSlider)
            {
                m_wndSlider.SetPos(_audioPlayback.QueryPosition(100));
            }
            if (_pwndWaveform)
            {
                _pwndWaveform->SetStreamPosition(_audioPlayback.QueryStreamPosition());
            }
            _audioPlayback.IdleUpdate();

            const SyncComponent *syncComponent = _audio->TryGetComponent<SyncComponent>();
            if (_pwndMouth && syncComponent)
            {
                uint16_t tickPosition = (uint16_t)_audioPlayback.QueryPosition(_audio->GetComponent<AudioComponent>().GetLengthInTicks());
                uint16_t cel = syncComponent->GetCelAtTick(tickPosition);
                if (!_audioPlayback.IsPlaying() && !syncComponent->Entries.empty())
                {
                    // We've stopped playing, do the last one.
                    cel = syncComponent->Entries.back().Cel;
                }
                if (cel != 0xffff)
                {
                    _pwndMouth->SetCel(cel, true);
                }
            }

        }
        if (!_IsPlaying())
        {
            _UpdatePlayState();
        }
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}


