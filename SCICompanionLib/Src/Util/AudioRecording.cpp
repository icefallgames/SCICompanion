#include "stdafx.h"
#include "AudioRecording.h"
#include "Audio.h"

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

void AudioRecording::Cleanup(AudioComponent *audio)
{
    if (hWaveIn)
    {
        MMRESULT result = waveInReset(hWaveIn);
        if (result == MMSYSERR_NOERROR)
        {
            if (_state == RecordState::Recording)
            {
                if (audio && (waveHeader.dwBytesRecorded > 0))
                {
                    audio->Frequency = _recordingFreq;
                    audio->Flags = _recordingFlags;
                    audio->DigitalSamplePCM.clear();
                    std::copy(&_buffer[0], &_buffer[waveHeader.dwBytesRecorded], std::back_inserter(audio->DigitalSamplePCM));
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
                int16_t maxValue = (std::numeric_limits<int16_t>::max)();
                int16_t minValue = (std::numeric_limits<int16_t>::min)();

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

static uint16_t s_threshold = 512;

int NoiseGate(int sample)
{
    if (sample > s_threshold || sample < -s_threshold)
    {
        sample = sample;
    }
    else sample = 0;
    return sample;
}

// Needs some tweaking.
static float AttackTime = 0.015f;
static float ReleaseTime = 0.025f;
static float HoldTime = 0.1f;
static float OpenThresholdDb = -26.0f;
static float CloseThresholdDb = -32.0f;

float dbToRms(float db)
{
    return pow(10.0f, db / 20.0f);
}

void ApplyNoiseGate(float *buffer, int totalFloats, float sampleRate)
{
    float attenuation = 0.0f;
    float level = 0.0f;
    float heldTime = 0.0f;
    bool isOpen = false;

    float OpenThreshold = dbToRms(OpenThresholdDb);
    float CloseThreshold = dbToRms(CloseThresholdDb);

    const float SAMPLE_RATE_F = sampleRate;
    const float dtPerSample = 1.0f / SAMPLE_RATE_F;

    // Convert configuration times into per-sample amounts
    const float attackRate = 1.0f / (AttackTime * SAMPLE_RATE_F);
    const float releaseRate = 1.0f / (ReleaseTime * SAMPLE_RATE_F);

    // Determine level decay rate. We don't want human voice (75-300Hz) to cross the close
    // threshold if the previous peak crosses the open threshold.
    const float thresholdDiff = OpenThreshold - CloseThreshold;
    const float minDecayPeriod = (1.0f / 75.0f) * SAMPLE_RATE_F;
    const float decayRate = thresholdDiff / minDecayPeriod;

    // We can't use SSE as the processing of each sample depends on the processed
    // result of the previous sample.
    for (int i = 0; i < totalFloats; i ++)
    {
        // Get current input level
        float curLvl = abs(buffer[i]);

        // Test thresholds
        if (curLvl > OpenThreshold && !isOpen)
            isOpen = true;
        if (level < CloseThreshold && isOpen)
        {
            heldTime = 0.0f;
            isOpen = false;
        }

        // Decay level slowly so human voice (75-300Hz) doesn't cross the close threshold
        // (Essentially a peak detector with very fast decay)
        level = max(level, curLvl) - decayRate;

        // Apply gate state to attenuation
        if (isOpen)
            attenuation = min(1.0f, attenuation + attackRate);
        else
        {
            heldTime += dtPerSample;
            if (heldTime > HoldTime)
                attenuation = max(0.0f, attenuation - releaseRate);
        }

        // Attenuate!
        buffer[i] *= attenuation;
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

    if (audio)
    {
        // convert to float
        std::vector<float> buffer;
        buffer.reserve(audio->DigitalSamplePCM.size());
        std::transform(audio->DigitalSamplePCM.begin(), audio->DigitalSamplePCM.end(), std::back_inserter(buffer),
            [](uint8_t byte) { return ((float)((int)byte - 128)) / 128.0f; }
            );

        ApplyNoiseGate(&buffer[0], buffer.size(), audio->Frequency);
    
        // Push it back
        std::transform(buffer.begin(), buffer.end(), audio->DigitalSamplePCM.begin(),
            [](float value)
        {
            float temp = (value * 128.0f) + 128.0f;
            int temp2 = (int)temp;
            return (uint8_t)(min(max(temp2, 0), 255));
        }
        );

    }

    return audio;
}

void AudioRecording::_StartBuffer(AudioRecording::RecordState state, WaveRecordingFormat format)
{
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


