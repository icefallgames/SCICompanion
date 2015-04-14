#pragma once

#include <mmsystem.h>

struct SoundComponent;

class AudioPlayback
{
public:
    AudioPlayback();
    ~AudioPlayback();
    void Play(const SoundComponent &sound);

private:
    HWAVEOUT hWaveOut;
    WAVEHDR waveHeader;
};

extern AudioPlayback g_audioPlayback;