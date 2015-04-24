#include "stdafx.h"
#include "AudioPlayback.h"
#include "Audio.h"

// Our global instance
AudioPlayback g_audioPlayback;

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

void AudioPlayback::Play(const AudioComponent &sound)
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

    assert(!hWaveOut);
    WORD blockAlign = 1; 
    WAVEFORMATEX waveFormat = { 0 };
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = 1;   // mono
    waveFormat.nSamplesPerSec = sound.Frequency;
    waveFormat.nAvgBytesPerSec = sound.Frequency * blockAlign;
    waveFormat.nBlockAlign = blockAlign;
    waveFormat.wBitsPerSample = 8; // right?
    waveFormat.cbSize = 0;

    // WAVE_MAPPED_DEFAULT_COMMUNICATION_DEVICE is only available on win7, so go without it
    MMRESULT result = waveOutOpen(&hWaveOut, WAVE_MAPPER, &waveFormat, 0, 0, CALLBACK_NULL);
    if (result == MMSYSERR_NOERROR)
    {
        waveHeader.lpData = const_cast<LPSTR>(reinterpret_cast<const char*>(&sound.DigitalSamplePCM[0]));
        waveHeader.dwBufferLength = sound.DigitalSamplePCM.size();
        waveHeader.dwLoops = 10;
        result = waveOutPrepareHeader(hWaveOut, &waveHeader, sizeof(waveHeader));
        if (result == MMSYSERR_NOERROR)
        {
            result = waveOutWrite(hWaveOut, &waveHeader, sizeof(waveHeader));
        }
    }
}