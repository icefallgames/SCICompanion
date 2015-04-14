#include "stdafx.h"
#include "AudioPlayback.h"
#include "Sound.h"

// Our global instance
AudioPlayback g_audioPlayback;

AudioPlayback::AudioPlayback() : hWaveOut(nullptr), waveHeader()
{

}

AudioPlayback::~AudioPlayback()
{
    // REVIEW: I think I need to unprepare too
    if (hWaveOut)
    {
        waveOutClose(hWaveOut);
        hWaveOut = nullptr;
    }
}

void AudioPlayback::Play(const SoundComponent &sound)
{
    if (!hWaveOut)
    {
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
            // Next, waveOutPrepareHeader


            // Then, waveOutWrite
            waveHeader.lpData = const_cast<LPSTR>(reinterpret_cast<const char*>(&sound.DigitalSamplePCM[0]));
            waveHeader.dwBufferLength = sound.DigitalSamplePCM.size();
            waveHeader.dwLoops = 10;
            result = waveOutPrepareHeader(hWaveOut, &waveHeader, sizeof(waveHeader));
            if (result == MMSYSERR_NOERROR)
            {
                result = waveOutPrepareHeader(hWaveOut, &waveHeader, sizeof(waveHeader));
            }


            // Close it for now
           // waveOutClose(hWaveOut);
            //hWaveOut = nullptr;
        }
    }
}