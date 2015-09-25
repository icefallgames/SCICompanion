#pragma once

#include "Audio.h"
#include "AudioPlayback.h"
#include "SoundUtil.h"

template<typename T>
class AudioPlaybackUI : public T
{
public:
    AudioPlaybackUI();
    AudioPlaybackUI(int id, CWnd *parent);
    void SetAudioResource(const ResourceEntity *audio);


protected:
    // Call these things:
    void DoDataExchangeHelper(CDataExchange* pDX);    // DDX/DDV support
    void OnInitDialogHelper();

private:
    afx_msg void OnPlay();
    afx_msg void OnStop();
    afx_msg void OnBrowse();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    void _UpdatePlayState();
    DECLARE_MESSAGE_MAP()
    bool _IsPlaying();

    CExtEdit m_wndInfo;
    CExtButton m_wndPlay;
    CExtButton m_wndStop;
    CExtSliderWnd m_wndSlider;
    CExtCheckBox m_wndAutoPreview;
    CExtLabel m_wndDuration;
    CExtEdit m_wndDescription;
    CExtButton m_wndBrowse;
    CExtLabel m_wndTitle;

    const ResourceEntity *_audio;

    CFont _marlettFont;
};



//
// Audio previewer
//
BEGIN_TEMPLATE_MESSAGE_MAP(AudioPlaybackUI, T, T)
    ON_BN_CLICKED(IDC_BUTTON_PLAY2, OnPlay)
    ON_BN_CLICKED(IDC_BUTTON_STOP, OnStop)
    ON_BN_CLICKED(IDC_BUTTONBROWSE, OnBrowse)
    ON_WM_TIMER()
END_MESSAGE_MAP()

#define AUDIO_TIMER 5004

template <typename T>
AudioPlaybackUI<T>::AudioPlaybackUI() 
{
}

template <typename T>
AudioPlaybackUI<T>::AudioPlaybackUI(int id, CWnd *parent) : T(id, parent)
{
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
            g_audioPlayback.SetAudio(audioComp);
            // Put some information in the channels window
            info = fmt::format("{0}Hz, {1} bytes. {2} {3}",
                audioComp->Frequency,
                audioComp->GetLength(),
                IsFlagSet(audioComp->Flags, AudioFlags::SixteenBit) ? "16-bit" : "8-bit",
                IsFlagSet(audioComp->Flags, AudioFlags::DPCM) ? "DPCM" : "");
        }
        if (m_wndAutoPreview.GetCheck() == BST_CHECKED)
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
    DDX_Control(pDX, IDC_BUTTON_STOP, m_wndStop);
    DDX_Control(pDX, IDC_SLIDER, m_wndSlider);
    DDX_Control(pDX, IDC_CHECK_AUTOPREV, m_wndAutoPreview);
    DDX_Control(pDX, IDC_STATIC_DURATION, m_wndDuration);
    DDX_Control(pDX, IDC_BUTTONBROWSE, m_wndBrowse);

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
    }

    m_wndSlider.SetRange(0, 100);

    _UpdatePlayState();

    ReleaseDC(pDC);
}

template <typename T>
bool AudioPlaybackUI<T>::_IsPlaying()
{
    return g_audioPlayback.IsPlaying();
}

template <typename T>
void AudioPlaybackUI<T>::_UpdatePlayState()
{
    if (_IsPlaying())
    {
        SetTimer(AUDIO_TIMER, 100, NULL);
    }
    else
    {
        KillTimer(AUDIO_TIMER);
    }
    m_wndPlay.EnableWindow(_audio && !_IsPlaying());
    m_wndStop.EnableWindow(_audio && _IsPlaying());
}

template <typename T>
void AudioPlaybackUI<T>::OnPlay()
{
    const AudioComponent *audio = _audio->TryGetComponent<AudioComponent>();
    if (audio && (audio->Frequency != 0))
    {
        g_audioPlayback.Stop();
        g_audioPlayback.Play();
        _UpdatePlayState();
    }
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
            AddWaveFileToGame(filename);
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
    g_audioPlayback.Stop();
    _UpdatePlayState();
}

template <typename T>
void AudioPlaybackUI<T>::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == AUDIO_TIMER)
    {
        if (g_audioPlayback.IsPlaying())
        {
            m_wndSlider.SetPos(g_audioPlayback.QueryPosition(100));
            g_audioPlayback.IdleUpdate();
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


