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
#include "stdafx.h"
#include "AudioPlayback.h"
#include "Audio.h"

AudioPlayback::AudioPlayback() : hWaveOut(nullptr), waveHeader()
{

}

AudioPlayback::~AudioPlayback()
{
    Cleanup();
}

void AudioPlayback::Cleanup()
{
    if (hWaveOut)
    {
        waveOutUnprepareHeader(hWaveOut, &waveHeader, sizeof(waveHeader));
        // Now we can free our data. TODO
        waveOutReset(hWaveOut);
        waveOutClose(hWaveOut);
        hWaveOut = nullptr;
        memset(&waveHeader, 0, sizeof(waveHeader)); // Just for good measure
    }
}

DWORD AudioPlayback::QueryPosition(DWORD scope)
{
    DWORD pos = 0;
    if (hWaveOut && _sound)
    {
        MMTIME mmTime = {};
        mmTime.wType = TIME_BYTES;
        if (MMSYSERR_NOERROR == waveOutGetPosition(hWaveOut, &mmTime, sizeof(mmTime)))
        {
            pos = (scope * mmTime.u.cb / _sound->DigitalSamplePCM.size());
        }
    }
    return pos;
}

uint32_t AudioPlayback::QueryStreamPosition()
{
    uint32_t pos = 0;
    if (hWaveOut && _sound)
    {
        MMTIME mmTime = {};
        mmTime.wType = TIME_BYTES;
        if (MMSYSERR_NOERROR == waveOutGetPosition(hWaveOut, &mmTime, sizeof(mmTime)))
        {
            pos = mmTime.u.cb;
        }
    }
    return pos;
}


void AudioPlayback::SetAudio(const AudioComponent *sound)
{
    _sound = sound;
}

bool AudioPlayback::IsPlaying()
{
    return hWaveOut != nullptr;
}

void AudioPlayback::IdleUpdate()
{
    if (hWaveOut)
    {
        // If we're finished playing, then close
        if (waveHeader.dwFlags & WHDR_DONE)
        {
            Cleanup();
        }
    }
}

void AudioPlayback::Stop()
{
    Cleanup();
}

void AudioPlayback::Play(int slowDown)
{
    if (hWaveOut)
    {
        // If we're finished playing, then close
        if (waveHeader.dwFlags & WHDR_DONE)
        {
            Cleanup();
        }
        else
        {
            // We're busy.
            return;
        }
    }

    if (_sound && !_sound->DigitalSamplePCM.empty())
    {
        uint16_t freq = _sound->Frequency;
        freq /= slowDown;

        assert(!hWaveOut);
        WORD blockAlign = IsFlagSet(_sound->Flags, AudioFlags::SixteenBit) ? 2 : 1;
        WAVEFORMATEX waveFormat = { 0 };
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;
        waveFormat.nChannels = 1;   // mono
        waveFormat.nSamplesPerSec = freq;
        waveFormat.nAvgBytesPerSec = freq * blockAlign;
        waveFormat.nBlockAlign = blockAlign;
        waveFormat.wBitsPerSample = 8 * blockAlign;
        waveFormat.cbSize = 0;

        // WAVE_MAPPED_DEFAULT_COMMUNICATION_DEVICE is only available on win7, so go without it
        MMRESULT result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, CALLBACK_NULL);
        if (result == MMSYSERR_NOERROR)
        {
            waveHeader.lpData = const_cast<LPSTR>(reinterpret_cast<const char*>(&_sound->DigitalSamplePCM[0]));
            waveHeader.dwBufferLength = _sound->DigitalSamplePCM.size();
            waveHeader.dwLoops = 10;
            result = waveOutPrepareHeader(hWaveOut, &waveHeader, sizeof(waveHeader));
            if (result == MMSYSERR_NOERROR)
            {
                result = waveOutWrite(hWaveOut, &waveHeader, sizeof(waveHeader));
            }
        }
    }
}