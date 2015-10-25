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
struct AudioNegativeComponent;
class ResourceEntity;
enum class AudioFlags : uint8_t;

enum class WaveRecordingFormat
{
    ElevenK8 = 0,
    ElevenK16 = 1,
    TwentyTwoK8 = 2,
    TwentyTwoK16 = 3,

    SixteenBit = 0x1,
    TwentyTwoK = 0x2,
};
DEFINE_ENUM_FLAGS(WaveRecordingFormat, uint32_t)

class AudioRecording
{
public:
    AudioRecording();
    ~AudioRecording();
    void SetAudio(const AudioRecording *sound);
    void Record(WaveRecordingFormat format);
    std::unique_ptr<ResourceEntity> Stop();
    void StopMonitor();
    void IdleUpdate();
    bool IsRecording();
    DWORD QueryPosition(DWORD scope);
    int GetLevel(); // 0 to 100

private:

    enum class RecordState
    {
        Stopped,
        Monitor,
        Recording
    };

    void _StartBuffer(RecordState state, WaveRecordingFormat format);
    void Cleanup(AudioComponent *audioResult = nullptr, AudioNegativeComponent *audioNeg = nullptr);

    HWAVEIN hWaveIn;
    AudioFlags _recordingFlags;
    AudioFlags _finalFormatFlags;
    uint16_t _recordingFreq;

    RecordState _state;

    // Monitoring
    int _lastLevelMonitored;    // 0 to 100
    uint32_t lastStreamPositionMonitored;

    WAVEHDR waveHeader;
    std::unique_ptr<uint8_t[]> _buffer;
};

extern AudioRecording g_audioRecording;
extern WaveRecordingFormat g_WaveRecordingFormat;