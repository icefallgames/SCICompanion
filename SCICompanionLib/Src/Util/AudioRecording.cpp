#include "stdafx.h"
#include "AudioRecording.h"
#include "Audio.h"

// Our global instance
AudioRecording g_audioRecording;
WaveRecordingFormat g_WaveRecordingFormat = WaveRecordingFormat::TwentyTwoK8;

#define BUFFER_SIZE (8 * 1024 * 1024)

AudioRecording::AudioRecording() : hWaveIn(nullptr), waveHeader()
{
    _buffer = std::make_unique<uint8_t[]>(BUFFER_SIZE); // 8 MB for now. Workaround is just to browse for wav.
}

AudioRecording::~AudioRecording()
{
    Cleanup();
}

void AudioRecording::Cleanup(AudioComponent *audio)
{
    if (hWaveIn)
    {
        MMRESULT result = waveInReset(hWaveIn);
        if (result == MMSYSERR_NOERROR)
        {
            if (audio && (waveHeader.dwBytesRecorded > 0))
            {
                audio->Frequency = _recordingFreq;
                audio->Flags = _recordingFlags;
                audio->DigitalSamplePCM.clear();
                std::copy(&_buffer[0], &_buffer[waveHeader.dwBytesRecorded], std::back_inserter(audio->DigitalSamplePCM));
            }
        }

        waveInUnprepareHeader(hWaveIn, &waveHeader, sizeof(waveHeader));
        waveInClose(hWaveIn);
        hWaveIn = nullptr;
        memset(&waveHeader, 0, sizeof(waveHeader)); // Just for good measure
    }
}

DWORD AudioRecording::QueryPosition(DWORD scope)
{
    DWORD pos = 0;
    /*
    if (hWaveOut && _sound)
    {
        MMTIME mmTime = {};
        mmTime.wType = TIME_BYTES;
        if (MMSYSERR_NOERROR == waveOutGetPosition(hWaveOut, &mmTime, sizeof(mmTime)))
        {
            pos = (scope * mmTime.u.cb / _sound->DigitalSamplePCM.size());
        }
    }*/
    return pos;
}

bool AudioRecording::IsRecording()
{
    return hWaveIn != nullptr;
}

void AudioRecording::IdleUpdate()
{
    if (hWaveIn)
    {
        // If we're finished playing, then close
        if (waveHeader.dwFlags & WHDR_DONE)
        {
            Cleanup();
        }
    }
}

std::unique_ptr<AudioComponent> AudioRecording::Stop()
{
    std::unique_ptr<AudioComponent> audio = std::make_unique<AudioComponent>();
    Cleanup(audio.get());
    if (audio->Frequency == 0)
    {
        audio.reset(nullptr);   // Error
    }
    return audio;
}

void AudioRecording::Record(WaveRecordingFormat format)
{
    if (hWaveIn)
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

    WORD blockAlign = IsFlagSet(format, WaveRecordingFormat::SixteenBit) ? 2 : 1;
    _recordingFreq = IsFlagSet(format, WaveRecordingFormat::TwentyTwoK) ? 22050 : 11025;
    _recordingFlags = AudioFlags::None;
    if (IsFlagSet(format, WaveRecordingFormat::SixteenBit))
    {
        _recordingFlags = AudioFlags::SixteenBit;
    }
    WAVEFORMATEX waveFormat = { 0 };
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = 1;   // mono, always
    waveFormat.nSamplesPerSec = _recordingFreq;
    waveFormat.nAvgBytesPerSec = _recordingFreq * blockAlign;
    waveFormat.nBlockAlign = blockAlign;
    waveFormat.wBitsPerSample = 8 * blockAlign;
    waveFormat.cbSize = 0;


    // WAVE_MAPPER just a selects any device I guess...
    MMRESULT result = waveInOpen(&hWaveIn, WAVE_MAPPER, &waveFormat, 0, 0, CALLBACK_NULL);
    if (result == MMSYSERR_NOERROR)
    {
        bool success = false;

        waveHeader.lpData = const_cast<LPSTR>(reinterpret_cast<const char*>(&_buffer[0]));
        waveHeader.dwBufferLength = BUFFER_SIZE;
        waveHeader.dwLoops = 0;
        result = waveInPrepareHeader(hWaveIn, &waveHeader, sizeof(waveHeader));
        if (result == MMSYSERR_NOERROR)
        {
            result = waveInAddBuffer(hWaveIn, &waveHeader, sizeof(waveHeader));
            if (result == MMSYSERR_NOERROR)
            {
                result = waveInStart(hWaveIn);
                if (result == MMSYSERR_NOERROR)
                {
                    success = true;
                }
            }

            if (!success)
            {
                waveInUnprepareHeader(hWaveIn, &waveHeader, sizeof(waveHeader));
            }
        }

        if (!success)
        {
            waveInClose(hWaveIn);
            hWaveIn = nullptr;
        }
    }
}


