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

#include "Sound.h"

class MidiPlayer
{
public:
    MidiPlayer();
    ~MidiPlayer();
    DWORD SetSound(const SoundComponent &sound, uint16_t wTempo);
    void Play();
    void Pause();
    void Stop();
    bool CanPlay();
    bool CanPause();
    bool CanStop();
    DWORD QueryPosition(DWORD scope);
    DWORD QueryPosition();
    void SetTempo(uint16_t wTempo) { _wTempo = wTempo; _SetTempoAndDivision(); }
    void SetDevice(DeviceType device) { _device = device; }
    void CueTickPosition(DWORD dwTicks);
    void CuePosition(DWORD dwTicks, DWORD scope);
    bool IsPlaying() { return _fPlaying; }
    void Reset();

private:
    void _Reset();
    bool _Init();
    void _SetTempoAndDivision();
    void _ClearHeaders();
    void static CALLBACK s_MidiOutProc(HMIDIOUT hmo, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
    void _OnStreamDone();
    void _CuePosition(DWORD dwEventIndex, DWORD ticks = 0xffffffff);

    HMIDISTRM _handle;
    MIDIHDR _midiHdr;
    DWORD _cRemainingStreamEvents; // In case it didn't fit into a 64k chunk
    DWORD _cTotalStreamEvents;
    DWORD *_pRealData; // Full data for the 64k chunk in _midiHdr.
    std::vector<DWORD> _accumulatedStreamTicks;   // Corresponds to _midiHdr.lpData / 3
    DWORD _dwCurrentChunkTickStart;
    DWORD _dwCurrentTickPos;
    DeviceType _device;
    bool _fPlaying;
    bool _fQueuedUp;
    bool _fStoppingStream;
    DWORD _wTotalTime;
    uint16_t _wTempo;
    uint16_t _wTimeDivision;
    DWORD _dwLoopPoint;
    DWORD _dwCookie;

    std::unique_ptr<SoundComponent> _soundCache;
    uint16_t _tempoCache;
};

extern MidiPlayer g_midiPlayer;