#pragma once

#include <mmsystem.h>

struct AudioComponent;

class AudioPlayback
{
public:
    AudioPlayback();
    ~AudioPlayback();
    void SetAudio(const AudioComponent *sound);
    void Play(int slowDown = 1);
    void Stop();
    void IdleUpdate();
    bool IsPlaying();
    DWORD QueryPosition(DWORD scope);
    uint32_t QueryStreamPosition();

private:
    void Cleanup();

    HWAVEOUT hWaveOut;
    WAVEHDR waveHeader;
    const AudioComponent *_sound;
};
