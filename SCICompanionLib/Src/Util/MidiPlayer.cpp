#include "stdafx.h"
#include "MidiPlayer.h"

using namespace std;

//
// Midi helper
//
MidiPlayer::MidiPlayer()
{
    _handle = NULL;
    ZeroMemory(&_midiHdr, sizeof(_midiHdr));
    _pRealData = NULL;
    _device = DeviceType::RolandMT32;
    _fPlaying = false;
    _wTotalTime = 0;
    _wTimeDivision = SCI_PPQN;
    _wTempo = 120;
    _fStoppingStream = false;
    _dwCookie = 0;
}

MidiPlayer::~MidiPlayer()
{
    _ClearHeaders();
    if (_handle)
    {
        midiStreamClose(_handle);
    }
}

void PrintMidiOutErrorMsg(unsigned long err)
{
#define BUFFERSIZE 120
    char	buffer[BUFFERSIZE];
    char msg[200];
    if (!(err = midiOutGetErrorText(err, &buffer[0], BUFFERSIZE)))
    {
        StringCchPrintf(msg, ARRAYSIZE(msg), "%s\r\n", &buffer[0]);
        OutputDebugString(msg);
    }
    else if (err == MMSYSERR_BADERRNUM)
    {
        OutputDebugString("Strange error number returned!\r\n");
    }
    else
    {
        OutputDebugString("Specified pointer is invalid!\r\n");
    }
}

void MidiPlayer::_SetTempoAndDivision()
{
    if (_handle)
    {
        if (!_fPlaying)
        {
            MIDIPROPTIMEDIV prop;
            /* Set the stream device's Time Division */
            prop.cbStruct = sizeof(MIDIPROPTIMEDIV);
            prop.dwTimeDiv = _wTimeDivision;
            unsigned long err = midiStreamProperty(_handle, (LPBYTE)&prop, MIDIPROP_SET | MIDIPROP_TIMEDIV);
            if (err)
            {
                // We failed
                OutputDebugString("An error setting time division!\n");
            }
        } // We can't set this while we're playing.

        // But tempo is no problem...
        MIDIPROPTEMPO   tempo;
        tempo.cbStruct = sizeof(MIDIPROPTEMPO);
        tempo.dwTempo = 60000000 / _wTempo; // 60 million microseconds / bpm
        unsigned long err = midiStreamProperty(_handle, (LPBYTE)&tempo, MIDIPROP_SET | MIDIPROP_TEMPO);
        if (err)
        {
            OutputDebugString("An error setting the tempo!\n");
        }
    }
}

bool MidiPlayer::_Init()
{
    if (_handle == nullptr)
    {
        UINT streamno = 0;
        if (midiStreamOpen(&_handle, &streamno, 1, reinterpret_cast<DWORD_PTR>(s_MidiOutProc), reinterpret_cast<DWORD_PTR>(this), CALLBACK_FUNCTION))
        {
            OutputDebugString("Failed to open midi stream\n");
            assert(_handle == nullptr);
        }
    }
    return (_handle != nullptr);
}

DWORD MidiPlayer::QueryPosition(DWORD scope)
{
    DWORD pos = 0;
    if (_handle)
    {
        if (_fPlaying)
        {
            MMTIME time;
            time.wType = TIME_TICKS;
            if (!midiStreamPosition(_handle, &time, sizeof(time)))
            {
                // ticks ellapsed on current 64k chunk
                // plus the tick count when we started
                _dwCurrentTickPos = time.u.ticks + _dwCurrentChunkTickStart;
                pos = _dwCurrentTickPos * scope / _wTotalTime;
            }
        }
        else
        {
            pos = _dwCurrentTickPos * scope / _wTotalTime;
        }
    }
    return pos;
}

DWORD MidiPlayer::QueryPosition()
{
    return QueryPosition(_wTotalTime);
}

// 3 DWORDs per event, approx 64k (a little less)
#define MAX_STREAM_EVENTS (65532 / (3 * sizeof(DWORD)))

DWORD MidiPlayer::SetSound(const SoundComponent &sound, uint16_t wInitialTempo)
{
    _ClearHeaders();
    _wTimeDivision = sound.GetTimeDivision();
    _dwLoopPoint = sound.GetLoopPoint();

    // Combine events into one.
    // First, get the channels that apply to this device.
    vector<vector<SoundEvent>> eventChannels;
    for (const auto &trackInfo : sound.GetTrackInfos())
    {
        if (trackInfo.Type == (uint8_t)_device)
        {
            for (int channelId : trackInfo.ChannelIds)
            {
                for (const auto &channelInfo : sound.GetChannelInfos())
                {
                    if (channelInfo.Id == channelId)
                    {
                        // A bit expensive, since we're copying all events, but it should be fine.
                        eventChannels.push_back(channelInfo.Events);
                        break;
                    }
                }
            }
            break;
        }
    }

    vector<SoundEvent> finalEvents;
    DWORD totalTicks = CombineSoundEvents(eventChannels, finalEvents);

    if (!finalEvents.empty() && _Init())
    {
        // Set up the stream.
        // Put new data in.
        // Accumulated ticks will need to be at most this big:
        _accumulatedStreamTicks.resize(finalEvents.size());
        DWORD cbSize = (DWORD)(finalEvents.size() * 3) * sizeof(DWORD);
        unsigned long *stuff = new unsigned long[cbSize / sizeof(DWORD)];
        _pRealData = stuff;

        DWORD cbEventsUsed = 0;
        _wTotalTime = 0;
        DWORD dwAccumulatedTicks = 0;
        for (size_t i = 0; i < finalEvents.size(); ++i)
        {
            DWORD dwThisTimeDelta = finalEvents[i].wTimeDelta;
            *stuff++ = dwThisTimeDelta;
            dwAccumulatedTicks += dwThisTimeDelta;
            _accumulatedStreamTicks[cbEventsUsed] = dwAccumulatedTicks;
            *stuff++ = 0;
            *stuff++ = (MEVT_SHORTMSG << 24) | finalEvents[i].GetRawStatus() | (((DWORD)finalEvents[i].bParam1) << 8) | (((DWORD)finalEvents[i].bParam2) << 16);
            _wTotalTime += finalEvents[i].wTimeDelta; // Not quite right - there could be empty space at the end.
            cbEventsUsed++;
        }

        SetTempo(wInitialTempo);
        _cTotalStreamEvents = cbEventsUsed;
        _CuePosition(0, 0);
    }
    return ++_dwCookie;
}

void MidiPlayer::Play()
{
    if (!_accumulatedStreamTicks.empty())
    {
        unsigned int err = midiStreamRestart(_handle);
        if (!err)
        {
            _fPlaying = true;
        }
        else
        {
            PrintMidiOutErrorMsg(err);
        }
    }
}

void MidiPlayer::Pause()
{
    unsigned int err = midiStreamPause(_handle);
    if (!err)
    {
        _fPlaying = false;
    }
    else
    {
        PrintMidiOutErrorMsg(err);
    }
}

void MidiPlayer::Stop()
{
    Pause();
    // Return to the beginning.
    _CuePosition(0, 0);
}

bool MidiPlayer::CanPlay()
{
    return (!_fPlaying);
}
bool MidiPlayer::CanPause()
{
    return (_fPlaying);
}
bool MidiPlayer::CanStop()
{
    return QueryPosition(3000) != 0;
}

void MidiPlayer::_ClearHeaders()
{
    if (_handle)
    {
        // Stop and unhook the old data
        if (_pRealData)
        {
            _fStoppingStream = true;
            midiOutReset((HMIDIOUT)_handle);
            _fStoppingStream = false;

            // Unprepare it
            if (_fQueuedUp)
            {
                midiOutUnprepareHeader((HMIDIOUT)_handle, &_midiHdr, sizeof(_midiHdr));
                _fQueuedUp = false;
            }

            delete[] _pRealData;
            _midiHdr.lpData = NULL;
            _pRealData = NULL;
        }
    }
}

//
// Queues the player to a particular "percentage"
//
void MidiPlayer::CuePosition(DWORD dwTicks, DWORD scope)
{
    dwTicks = min(scope, dwTicks);
    dwTicks = _wTotalTime * dwTicks / scope;
    //dwTicks = _accumulatedStreamTicks[_cTotalStreamEvents - 1] * dwTicks / scope;
    // Find the spot to cue.
    size_t i = 0;
    for (; i < _accumulatedStreamTicks.size(); i++)
    {
        if (_accumulatedStreamTicks[i] >= dwTicks)
        {
            break;
        }
    }
    if (i >= _accumulatedStreamTicks.size())
    {
        i = 0;
    }
    _CuePosition((DWORD)i, dwTicks);
}
void MidiPlayer::CueTickPosition(DWORD dwTicks)
{
    // Find the spot to cue.
    size_t i = 0;
    for (; i < _accumulatedStreamTicks.size(); i++)
    {
        if (_accumulatedStreamTicks[i] >= dwTicks)
        {
            break;
        }
    }
    if (i >= _accumulatedStreamTicks.size())
    {
        i = 0;
    }
    _CuePosition((DWORD)i, dwTicks);
}

//
// Queues the player to a particular index in the events.
//
void MidiPlayer::_CuePosition(DWORD dwEventIndex, DWORD ticks)
{
    if (_pRealData)
    {
        if (_fQueuedUp)
        {
            _fStoppingStream = true;
            midiOutReset((HMIDIOUT)_handle);
            _fStoppingStream = false;
            midiOutUnprepareHeader((HMIDIOUT)_handle, &_midiHdr, sizeof(_midiHdr));
            _fQueuedUp = false;
        }

        ZeroMemory(&_midiHdr, sizeof(_midiHdr));
        _midiHdr.lpData = (LPSTR)(&_pRealData[dwEventIndex * 3]);
        // How long is it?
        DWORD cEvents;
        if (_cTotalStreamEvents > (dwEventIndex + MAX_STREAM_EVENTS))
        {
            // We can't play all
            _cRemainingStreamEvents = _cTotalStreamEvents - (dwEventIndex + MAX_STREAM_EVENTS);
            cEvents = MAX_STREAM_EVENTS;
        }
        else
        {
            _cRemainingStreamEvents = 0;
            cEvents = _cTotalStreamEvents - dwEventIndex;
        }

        _dwCurrentChunkTickStart = 0;
        if (dwEventIndex < (DWORD)_accumulatedStreamTicks.size())
        {
            _dwCurrentChunkTickStart = _accumulatedStreamTicks[dwEventIndex];
        }
        _dwCurrentTickPos = (ticks == 0xffffffff) ? _dwCurrentChunkTickStart : ticks;
        _midiHdr.dwBufferLength = _midiHdr.dwBytesRecorded = (cEvents * 3 * sizeof(DWORD));
        _midiHdr.dwFlags = 0;
        unsigned long err = midiOutPrepareHeader((HMIDIOUT)_handle, &_midiHdr, sizeof(MIDIHDR));
        if (!err)
        {
            _fQueuedUp = true; // assuming midiStreamOut works
            /* Queue the Stream of messages. Output doesn't actually start
            until we later call midiStreamRestart().
            */
            err = midiStreamOut(_handle, &_midiHdr, sizeof(MIDIHDR));
            if (!err)
            {
            }
            else
            {
                PrintMidiOutErrorMsg(err);
            }
        }
        else
        {
            PrintMidiOutErrorMsg(err);
        }
        if (!err && _fPlaying)
        {
            // We're good, so play it now
            err = midiStreamRestart(_handle);
            if (err)
            {
                PrintMidiOutErrorMsg(err);
            }
        }
    }
}

void MidiPlayer::_OnStreamDone()
{
    if (!_fStoppingStream)
    {
        // There is more to this stream... keep going.
        if (_cRemainingStreamEvents)
        {
            _CuePosition(_cTotalStreamEvents - _cRemainingStreamEvents);
        }
        else
        {
            if (_dwLoopPoint != SoundComponent::LoopPointNone)
            {
                // Cue the loop point.
                CueTickPosition(_dwLoopPoint);
            }
            else
            {
                Stop();
            }
        }
    }
}

void CALLBACK MidiPlayer::s_MidiOutProc(HMIDIOUT hmo, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    if (wMsg == MM_MOM_DONE)
    {
        (reinterpret_cast<MidiPlayer*>(dwInstance))->_OnStreamDone();
    }
}

MidiPlayer g_midiPlayer;