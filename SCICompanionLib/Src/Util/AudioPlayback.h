#pragma once

#include <mmsystem.h>

struct AudioComponent;

class AudioPlayback
{
public:
    AudioPlayback();
    ~AudioPlayback();
    void Play(const AudioComponent &sound);
    void IdleUpdate();

private:
    void Cleanup();

    HWAVEOUT hWaveOut;
    WAVEHDR waveHeader;
};

extern AudioPlayback g_audioPlayback;