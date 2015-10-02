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
    if (hWaveIn)
    {
        // TODO: Don't overlap things... i.e. don't run the algorihtm over the same set of
        // bytes... might need to adjust things abit.


        // Use the value from the current recording.
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
                throw std::exception("implement this when done with 8 bit");
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


