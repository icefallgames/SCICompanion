#pragma once

#include <mmsystem.h>

struct AudioComponent;

class AudioPlayback
{
public:
    AudioPlayback();
    ~AudioPlayback();
    void SetAudio(const AudioComponent *sound);
    void Play();
    void Stop();
    void IdleUpdate();
    bool IsPlaying();
    DWORD QueryPosition(DWORD scope);

private:
    void Cleanup();

    HWAVEOUT hWaveOut;
    WAVEHDR waveHeader;
    const AudioComponent *_sound;
};

extern AudioPlayback g_audioPlayback;