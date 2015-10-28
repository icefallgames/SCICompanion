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
#include "AudioRecording.h"
#include "Audio.h"
#include "AppState.h"
#include "AudioProcessing.h"
#include "AudioNegative.h"
#include "ResourceSourceFlags.h"

// Our global instance
AudioRecording g_audioRecording;
WaveRecordingFormat g_WaveRecordingFormat = WaveRecordingFormat::TwentyTwoK8;

#define BUFFER_SIZE (8 * 1024 * 1024)

AudioRecording::AudioRecording() : hWaveIn(nullptr), waveHeader(), _state(RecordState::Stopped), _lastLevelMonitored(0), lastStreamPositionMonitored(0)
{
    _buffer = std::make_unique<uint8_t[]>(BUFFER_SIZE); // 8 MB for now. Workaround is just to browse for wav.
}

AudioRecording::~AudioRecording()
{
    Cleanup();
}

void AudioRecording::Cleanup(AudioComponent *audio, AudioNegativeComponent *audioNeg)
{
    if (hWaveIn)
    {
        MMRESULT result = waveInReset(hWaveIn);
        if (result == MMSYSERR_NOERROR)
        {
            if (_state == RecordState::Recording)
            {
                if (audioNeg && (waveHeader.dwBytesRecorded > 0))
                {
                    // Fill in the audio negative.
                    audioNeg->Audio.Frequency = _recordingFreq;
                    audioNeg->Audio.Flags = _recordingFlags;
                    uint8_t *rawData = reinterpret_cast<uint8_t*>(waveHeader.lpData);
                    audioNeg->Audio.DigitalSamplePCM.clear();
                    audioNeg->Audio.DigitalSamplePCM.reserve(waveHeader.dwBytesRecorded);
                    std::copy(rawData, rawData + waveHeader.dwBytesRecorded, std::back_inserter(audioNeg->Audio.DigitalSamplePCM));
                    // Use the default settings:
                    audioNeg->Settings = *appState->_audioProcessing;

                    audio->Flags = _finalFormatFlags;
                    audio->Frequency = _recordingFreq;
                    audio->DigitalSamplePCM.clear();

                    audioNeg->Audio.ScanForClipped();
                    ProcessSound(*audioNeg, *audio, audio->Flags);
                }
            }
        }

        waveInUnprepareHeader(hWaveIn, &waveHeader, sizeof(waveHeader));
        waveInClose(hWaveIn);
        hWaveIn = nullptr;
        memset(&waveHeader, 0, sizeof(waveHeader)); // Just for good measure

        lastStreamPositionMonitored = 0;
    }
    _state = RecordState::Stopped;
}

const uint32_t MonitorMilliseconds = 30;

int AudioRecording::GetLevel()
{
    // Gets recent "levels" for the current recording/monitoring.
    if (hWaveIn)
    {
        // REVIEW: Don't overlap things... i.e. don't run the algorihtm over the same set of
        // bytes. This is really only a problem if we have multiple clients asking for the levels.
        // Currently we don't.

        uint32_t bytesRecorded = waveHeader.dwBytesRecorded;
        assert(bytesRecorded >= lastStreamPositionMonitored);
        if (waveHeader.dwBytesRecorded > lastStreamPositionMonitored)
        {
            uint32_t bytesToExamine = (waveHeader.dwBytesRecorded - lastStreamPositionMonitored);
            int blockAlign = IsFlagSet(_recordingFlags, AudioFlags::SixteenBit) ? 2 : 1;

            // We only want to go back a max of MonitorMilliseconds though...
            uint32_t maxBytesToExamine = MonitorMilliseconds * _recordingFreq * blockAlign / 1000;
            maxBytesToExamine = maxBytesToExamine / 2 * 2;  // Always make it even, in case of 16 bit.
            bytesToExamine = min(bytesToExamine, maxBytesToExamine);

            uint32_t samplesToExamine = bytesToExamine / blockAlign;
            if (blockAlign == 1)
            {
                // Easy
                uint8_t maxValue = 0;
                uint8_t minValue = 255;
                for (size_t i = (bytesRecorded - samplesToExamine); i < bytesRecorded; i++)
                {
                    uint8_t value = waveHeader.lpData[i];
                    maxValue = max(maxValue, value);
                    minValue = min(minValue, value);
                }
                int maxValueFinal = (int)maxValue - 128;
                int minValueFinal = (int)minValue - 128;
                _lastLevelMonitored = max(abs(maxValueFinal), abs(minValueFinal));
                _lastLevelMonitored = _lastLevelMonitored * 100 / 128;
            }
            else
            {
                // 16 bit
                int16_t maxValue = (std::numeric_limits<int16_t>::min)();
                int16_t minValue = (std::numeric_limits<int16_t>::max)();

                assert((bytesRecorded % 2) == 0);
                const int16_t *sixteenBitBuffer = reinterpret_cast<const int16_t*>(waveHeader.lpData);
                uint32_t samplesRecorded = bytesRecorded / 2;
                for (size_t i = (samplesRecorded - samplesToExamine); i < samplesRecorded; i++)
                {
                    int16_t value = sixteenBitBuffer[i];
                    maxValue = max(maxValue, value);
                    minValue = min(minValue, value);
                }
                _lastLevelMonitored = max(abs(maxValue), abs(minValue));
                _lastLevelMonitored = _lastLevelMonitored * 100 / (256 * 128);
            }
            
        }

        lastStreamPositionMonitored = waveHeader.dwBytesRecorded;
    }
    else
    {
        // Start monitoring, we'll get a value next time.
        _StartBuffer(RecordState::Monitor, WaveRecordingFormat::TwentyTwoK8);
    }
    return _lastLevelMonitored;
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
    return (hWaveIn != nullptr) && (_state == RecordState::Recording);  // As opposed to monitoring.
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


void AudioRecording::StopMonitor()
{
    if (_state == RecordState::Monitor)
    {
        Cleanup();
    }
}

std::unique_ptr<ResourceEntity> AudioRecording::Stop()
{
    std::unique_ptr<ResourceEntity> resource;
    // If we're just monitoring, don't stop...
    if (_state == RecordState::Recording)
    {
        resource.reset(CreateDefaultAudioResource(appState->GetVersion()));
        resource->AddComponent<AudioNegativeComponent>(std::make_unique<AudioNegativeComponent>());
        resource->SourceFlags = ResourceSourceFlags::AudioCache;
        Cleanup(resource->TryGetComponent<AudioComponent>(), resource->TryGetComponent<AudioNegativeComponent>());
        if (resource->GetComponent<AudioComponent>().Frequency == 0)
        {
            resource.reset(nullptr);   // Error
        }
    }
    return resource;
}

void AudioRecording::_StartBuffer(AudioRecording::RecordState state, WaveRecordingFormat format)
{
    _recordingFreq = IsFlagSet(format, WaveRecordingFormat::TwentyTwoK) ? 22050 : 11025;
    _finalFormatFlags = AudioFlags::None;
    if (IsFlagSet(format, WaveRecordingFormat::SixteenBit))
    {
        _finalFormatFlags = AudioFlags::SixteenBit | AudioFlags::Signed;
    }

    // We'll always record in 16 bit, then reduce the bit depth later. Recording in 8 bit is noisy, silence
    // oscillates between values 128 and 129. It should be 128.
    WORD blockAlign = 2;
    _recordingFlags = AudioFlags::SixteenBit | AudioFlags::Signed;

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

        if (success)
        {
            _state = state;
        }
        else
        {
            waveInClose(hWaveIn);
            hWaveIn = nullptr;
        }
    }
    lastStreamPositionMonitored = 0;
}

void AudioRecording::Record(WaveRecordingFormat format)
{
    if (hWaveIn)
    {
        // Stop any existing recording.
        Cleanup();
    }

    _StartBuffer(RecordState::Recording, format);

}


