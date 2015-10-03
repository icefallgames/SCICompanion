#pragma once

#include <mmsystem.h>

struct AudioComponent;
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
    std::unique_ptr<AudioComponent> Stop();
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
    void Cleanup(AudioComponent *audioResult = nullptr);

    HWAVEIN hWaveIn;
    AudioFlags _recordingFlags;
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