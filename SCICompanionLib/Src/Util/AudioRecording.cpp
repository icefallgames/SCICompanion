#include "stdafx.h"
#include "AudioRecording.h"
#include "Audio.h"
#include "AppState.h"
#include "AudioProcessing.h"

// Our global instance
AudioRecording g_audioRecording;
WaveRecordingFormat g_WaveRecordingFormat = WaveRecordingFormat::TwentyTwoK8;

#define BUFFER_SIZE (8 * 1024 * 1024)


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
    for (int i = 0; i < totalFloats; i++)
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


AudioRecording::AudioRecording() : hWaveIn(nullptr), waveHeader(), _state(RecordState::Stopped), _lastLevelMonitored(0), lastStreamPositionMonitored(0)
{
    _buffer = std::make_unique<uint8_t[]>(BUFFER_SIZE); // 8 MB for now. Workaround is just to browse for wav.
}

AudioRecording::~AudioRecording()
{
    Cleanup();
}

void ProcessSound(uint16_t freq, const int16_t *data, int samples, std::vector<int16_t> &result)
{
    // Trim the ends.
    /*
    uint32_t bytesPerSecond = audio->GetBytesPerSecond();
    uint32_t bytesToRemoveOffBack = min(bytesPerSecond * appState->_audioTrimRight / 1000, audio->DigitalSamplePCM.size());
    audio->DigitalSamplePCM.resize(audio->DigitalSamplePCM.size() - bytesToRemoveOffBack);
    uint32_t bytesToRemoveOffFront = min(bytesPerSecond * appState->_audioTrimLeft / 1000, audio->DigitalSamplePCM.size());
    audio->DigitalSamplePCM.erase(audio->DigitalSamplePCM.begin(), audio->DigitalSamplePCM.begin() + bytesToRemoveOffFront);
    */

    std::vector<float> buffer;
    SixteenBitToFloat(data, samples, buffer);

    if (!buffer.empty())
    {
        ApplyNoiseGate(&buffer[0], buffer.size(), freq);
    }

    FloatToSixteenBit(buffer, result);
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
                    audio->Flags = _finalFormatFlags;
                    audio->DigitalSamplePCM.clear();

                    std::vector<int16_t> processedSound;
                    ProcessSound(audio->Frequency, reinterpret_cast<int16_t*>(waveHeader.lpData), waveHeader.dwBytesRecorded / 2, processedSound);

                    if (!processedSound.empty())
                    {
                        if (IsFlagSet(_finalFormatFlags, AudioFlags::SixteenBit))
                        {
                            uint8_t *asBytes = reinterpret_cast<uint8_t*>(&processedSound[0]);
                            std::copy(asBytes, asBytes + processedSound.size() * 2, std::back_inserter(audio->DigitalSamplePCM));
                        }
                        else
                        {
                            // We always record in 16bit. Now we'll reduce to 8 bit.
                            // We want [-128,128] to map to [127.5,128.5]
                            DWORD samples = processedSound.size();
                            audio->DigitalSamplePCM.reserve(samples);
                            for (DWORD i = 0; i < samples; i++)
                            {
                                // I think this is more accurate:
                                uint32_t unsigned16 = processedSound[i] + 32768 + 128;
                                unsigned16 = min(unsigned16, 65535);
                                uint8_t unsigned8 = (uint8_t)(unsigned16 / 256);
                                audio->DigitalSamplePCM.push_back(unsigned8);

                                // But this flattens out noise around 0 better.
                                //int8_t signed8 = sixteenBitBuffer[i] / 256;
                                //audio->DigitalSamplePCM.push_back(signed8 + 128);
                            }
                        }
                    }
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

std::unique_ptr<AudioComponent> AudioRecording::Stop()
{
    std::unique_ptr<AudioComponent> audio;
    // If we're just monitoring, don't stop...
    if (_state == RecordState::Recording)
    {
        audio = std::make_unique<AudioComponent>();
        Cleanup(audio.get());
        if (audio->Frequency == 0)
        {
            audio.reset(nullptr);   // Error
        }
    }
    return audio;
}

void AudioRecording::_StartBuffer(AudioRecording::RecordState state, WaveRecordingFormat format)
{
    _recordingFreq = IsFlagSet(format, WaveRecordingFormat::TwentyTwoK) ? 22050 : 11025;
    _finalFormatFlags = AudioFlags::None;
    if (IsFlagSet(format, WaveRecordingFormat::SixteenBit))
    {
        _finalFormatFlags = AudioFlags::SixteenBit;
    }

    // We'll always record in 16 bit, then reduce the bit depth later. Recording in 8 bit is noisy, silence
    // oscillates between values 128 and 129. It should be 128.
    WORD blockAlign = 2;
    _recordingFlags = AudioFlags::SixteenBit;

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


