/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
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
