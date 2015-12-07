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
#include "Sound.h"
#include "AppState.h"
#include "ResourceEntity.h"
#include "Audio.h"
#include "format.h"

#pragma comment( lib, "winmm.lib" )

using namespace std;

void InsertEvent(std::vector<SoundEvent> &events, size_t index, const SoundEvent &event)
{
    auto itNext = events.insert(events.begin() + index, event) + 1;
    if (itNext != events.end())
    {
        // Adjust this ones time delta.
        assert(itNext->wTimeDelta >= event.wTimeDelta);
        itNext->wTimeDelta -= event.wTimeDelta;
    }
}

SoundEvent RemoveEvent(std::vector<SoundEvent> &events, size_t index)
{
    SoundEvent event = events[index];
    events.erase(events.begin() + index);
    if (index < events.size())
    {
        events[index].wTimeDelta += event.wTimeDelta;
    }
    return event;
}

bool _GetDeltaTime(sci::istream &stream, DWORD *pw)
{
    *pw = 0;
    uint8_t b;
    stream >> b;
    while (stream.good())
    {
        if (b == 0xf8) // special marker
        {
            *pw += 240;
            stream >> b;
        }
        else
        {
            *pw += b;
            break;
        }
        ASSERT(b != 0xfc); // TODO: we don't handle this yet
    }
    return stream.good();
}

bool _IsStatusByte(uint8_t b)
{
    return ((b & 0x80) == 0x80); // Status byte if most significant bit is set.
}

void AssertNoDuplicateTracks(const SoundComponent &sound)
{
    std::unordered_set<int> types;
    for (const auto &track : sound.GetTrackInfos())
    {
        if (types.find((int)track.Type) != types.end())
        {
            assert(false);
        }
        else
        {
            types.insert((int)track.Type);
        }
    }

}

SoundEvent::Command _GetSoundCommand(uint8_t bStatus)
{
    return (SoundEvent::Command)(bStatus & 0xF0);
}

uint8_t _GetChannel(uint8_t bStatus)
{
    return bStatus & 0x0F; // Lower nibble
}

SoundChangeHint SoundComponent::_EnsureChannel15()
{
    SoundChangeHint hint = SoundChangeHint::None;
    bool found = false;
    for (auto &channelInfo : _allChannels)
    {
        if (channelInfo.Number == 15)
        {
            found = true;
            break;
        }
    }
    if (!found)
    {
        // Add channel 15, and add it to all tracks.
        ChannelInfo channel15 = {};
        channel15.Flags = 0;
        channel15.Poly = false;
        channel15.Priority = 0;
        channel15.Id = (int)_allChannels.size();
        channel15.Number = 15;
        _allChannels.push_back(channel15);

        // Since it didn't exist before, we know we can just add it to every track
        for (auto &track : _tracks)
        {
            track.ChannelIds.push_back(channel15.Id);
        }
        hint |= SoundChangeHint::Changed;
    }
    return hint;
}

SoundComponent::SoundComponent(const SoundTraits &traits) : Traits(traits)
{
    Reset();
}

void SoundComponent::Reset()
{
    _wDivision = SCI_PPQN; // by default
    TotalTicks = 0;
    _fCanSetTempo = false;
    _fReEvaluateCues = false;
    _fReEvaluateLoopPoint = false;
    _wTempoIfChanged = StandardTempo;
    LoopPoint = LoopPointNone;

    Cues.clear();
    _allChannels.clear();
    _tracks.clear();
}

void CuePoint::SetValue(uint8_t value)
{
    _value = value;
    if (_cumulative == NonCumulative)
    {
        // 126 is the max value for a non-cumulative cue.
        _value = min(126, _value);
    }
}
void CuePoint::SetType(Type type)
{
    _cumulative = type;
    if (_cumulative == NonCumulative)
    {
        // 126 is the max value for a non-cumulative cue.
        _value = min(126, _value);
    }
}

const TrackInfo *SoundComponent::GetTrackInfo(DeviceType device) const
{
    for (const auto &trackInfo : _tracks)
    {
        if (trackInfo.Type == (uint8_t)device)
        {
            return &trackInfo;
        }
    }
    return nullptr;
}

const ChannelInfo *SoundComponent::GetChannelInfo(DeviceType device, int channelNumber) const
{
    const TrackInfo *trackInfo = GetTrackInfo(device);
    if (trackInfo)
    {
        for (int channelId : trackInfo->ChannelIds)
        {
            if (_allChannels[channelId].Number == channelNumber)
            {
                return &_allChannels[channelId];
            }
        }
    }
    return nullptr;
}

bool SoundComponent::DoesDeviceHaveTracks(DeviceType device) const
{
    const TrackInfo *trackInfo = GetTrackInfo(device);
    if (trackInfo)
    {
        for (int channelId : trackInfo->ChannelIds)
        {
            if (_allChannels[channelId].Number != 15)
            {
                return true;
            }
        }
    }
    return false;
}

uint16_t SoundComponent::CalculateChannelMask(DeviceType device) const
{
    uint16_t wMask = 0;
    const TrackInfo *trackInfo = GetTrackInfo(device);
    if (trackInfo)
    {
        for (int channelId : trackInfo->ChannelIds)
        {
            uint8_t channelNumber = _allChannels[channelId].Number;
            wMask |= (0x0001 << channelNumber);
        }
    }
    return wMask;
}

bool SoundComponent::DoesDeviceChannelIdOn(DeviceType device, int channelId) const
{
    const TrackInfo *track = GetTrackInfo(device);
    return (track && find(track->ChannelIds.begin(), track->ChannelIds.end(), channelId) != track->ChannelIds.end());
}

SoundChangeHint SoundComponent::ToggleChannelId(DeviceType type, int channelId)
{
    bool on = DoesDeviceChannelIdOn(type, channelId);
    return SetChannelId(type, channelId, !on);
}

SoundChangeHint SoundComponent::SetChannelId(DeviceType type, int channelId, bool on)
{
    SoundChangeHint hint = SoundChangeHint::None;
    if (channelId < (int)_allChannels.size())
    {
        // Make a TrackInfo for this device, if none exists
        TrackInfo *track = nullptr;
        for (auto &temp : _tracks)
        {
            if (temp.Type == (uint8_t)type)
            {
                track = &temp;
                break;
            }
        }

        int channelNumber = _allChannels[channelId].Number;

        if (!track)
        {
            _tracks.emplace_back();
            track = &_tracks.back();
            track->Type = (uint8_t)type;
            hint |= SoundChangeHint::Changed;
            // We just added a track. If there is a channel 15, then add it to this track
            if (on && (channelNumber != 15))    // Because then we'll add it anyway...
            {
                for (size_t channelIdTemp = 0; channelIdTemp < _allChannels.size(); channelIdTemp++)
                {
                    if (_allChannels[channelIdTemp].Number == 15)
                    {
                        track->ChannelIds.push_back(channelIdTemp);
                        break;
                    }
                }
            }
        }

        if (on)
        {
            // If we're adding, we should fail if it already includes another channel with
            // the same number.
            bool canAdd = true;
            for (int channelId : track->ChannelIds)
            {
                if (_allChannels[channelId].Number == channelNumber)
                {
                    // this channel is already on (possibly from another channelid)
                    canAdd = false;
                    break;
                }
            }
            if (canAdd)
            {
                track->ChannelIds.push_back(channelId);
                hint |= SoundChangeHint::Changed;
            }
        }
        else
        {
            // If we're removing, this is easy, just erase
            for (size_t i = 0; i < track->ChannelIds.size(); i++)
            {
                if (track->ChannelIds[i] == channelId)
                {
                    track->ChannelIds.erase(track->ChannelIds.begin() + i);
                    hint |= SoundChangeHint::Changed;
                    break;
                }
            }
        }
    }
    AssertNoDuplicateTracks(*this);

    return hint;
}

uint8_t _GetC(std::istream &midiFile)
{
    char c;
    midiFile.read(&c, 1);
    return c;
}

uint16_t _ReadBEWORD(std::istream &stream)
{
    uint16_t w = _GetC(stream);
    w = (w << 8) | _GetC(stream);
    return w;
}
DWORD _ReadBEDWORD(std::istream &stream)
{
    DWORD dw = _GetC(stream);
    dw = (dw << 8) | _GetC(stream);
    dw = (dw << 8) | _GetC(stream);
    dw = (dw << 8) | _GetC(stream);
    return dw;
}



DWORD ReadVarLen(std::istream &midiFile)
{
    DWORD value;
    uint8_t c;
    if ((value = _GetC(midiFile)) & 0x80)
    {
        value &= 0x7F;
        do
        {
            value = (value << 7) + ((c = _GetC(midiFile)) & 0x7F);
        } while (c & 0x80);
    }
    return value;
}


//
// Reads a midi file into the sound resource, and returns a uint16_t that is the channel mask
// of used channels.
//
uint16_t SoundComponent::_ReadMidiFileTrack(size_t nTrack, std::istream &midiFile, std::vector<SoundEvent> &events, std::vector<TempoEntry> &tempoChanges, DWORD &totalTicksOut)
{
    uint16_t wChannelMask = 0;
    DWORD dw = _ReadBEDWORD(midiFile);
    totalTicksOut = 0;
    while (dw != 0x4D54726B && midiFile.good())
    {
        // Skip this random chunk
        dw = _ReadBEDWORD(midiFile); // size of the chunk
        midiFile.seekg(dw, std::ios_base::cur);
        dw = _ReadBEDWORD(midiFile); // Next chunk id
    }
    if (midiFile.good())
    {
        // How long is it?
        DWORD cbBytes = _ReadBEDWORD(midiFile);

        // Read our track.
        uint8_t bLastStatus = 0;
        bool fSkip = false;
        DWORD dwMissingTime = 0;
        std::istream::pos_type curPosDebug = midiFile.tellg();
        std::istream::pos_type finalPos = midiFile.tellg() + (std::istream::pos_type)cbBytes;
        while (midiFile.good() && (midiFile.tellg() < finalPos))
        {
            SoundEvent event;
            // We start with time delta
            event.wTimeDelta = ReadVarLen(midiFile);
            totalTicksOut += event.wTimeDelta;
            event.wTimeDelta += dwMissingTime;
            fSkip = false;

            // Now the status byte
            uint8_t bStatus = _GetC(midiFile);
            if (bStatus & 0x80)
            {
                // High bit is set.  A new status.
                bLastStatus = bStatus;
            }
            else
            {
                // Re-use the last one...
                bStatus = bLastStatus;
                ASSERT((bLastStatus & 0xF0) != 0xF0);
                // Seek backwards one byte:
                midiFile.seekg(-1, std::ios_base::cur);
            }
            // Use last status
            ASSERT(bStatus);
            event.SetRawStatus(bStatus);

            // Read one or two bytes
            switch (bStatus & 0xF0)
            {
            case SoundEvent::NoteOff:
            case SoundEvent::NoteOn:
            case SoundEvent::KeyPressure:
            case SoundEvent::Control:
            case SoundEvent::PitchWheel:
                event.bParam1 = _GetC(midiFile);
                event.bParam2 = _GetC(midiFile);
                break;
            case SoundEvent::ProgramChange:
            case SoundEvent::Pressure:
                event.bParam1 = _GetC(midiFile);
                break;
            case SoundEvent::Special:
                if (bStatus == 0xFF)
                {
                    // Non-midi event (instrument, tempo, time sig, etc..)
                    uint8_t bType = _GetC(midiFile);
                    DWORD cbBytes = ReadVarLen(midiFile);
                    ASSERT(cbBytes < 0x0000ffff); // debug sanity check
                    // Skip it... (we may eventually use it, not sure yet)
                    if ((bType == 0x51) && (cbBytes == 3))
                    {
                        if (nTrack != 0)
                        {
                            appState->LogInfo("Found tempo event in track %d in midi file - ignoring\n", nTrack);
                        }
                        else
                        {
                            // This is a tempo change event.  
                            DWORD dwMSPerQN = (static_cast<DWORD>(_GetC(midiFile))) << 16;
                            dwMSPerQN |= (static_cast<DWORD>(_GetC(midiFile))) << 8;
                            dwMSPerQN |= _GetC(midiFile);
                            //TempoEntry tempoEntry = { (totalTicksOut + event.wTimeDelta), ((60000000 / dwMSPerQN)) };
                            //phil(); // what I changed it to:
                            TempoEntry tempoEntry = { (totalTicksOut), ((60000000 / dwMSPerQN)) };
                            if (tempoChanges.empty() && (tempoEntry.dwTicks != 0))
                            {
                                // Stick a 120bpm at position 0 if this is the first entry and its not a pos 0
                                TempoEntry tempo120 = { 0, 120 };
                                tempoChanges.push_back(tempo120);
                            }
                            tempoChanges.push_back(tempoEntry);
                        }
                        fSkip = true;
                    }
                    else
                    {
                        midiFile.seekg(cbBytes, std::ios_base::cur);
                        fSkip = true; // I guess?
                    }
                }
                else
                {
                    ASSERT((bStatus == 0xF0) || (bStatus == 0xF7));
                    DWORD cbBytes = ReadVarLen(midiFile);
                    ASSERT(cbBytes < 0x0000ffff); // debug sanity check
                    // Skip the data.  We don't handle these (other than using the timedelta)
                    midiFile.seekg(cbBytes, std::ios_base::cur);
                }
                break;
            default:
                ASSERT(FALSE);
                break;
            }
            if (event.GetChannel() != 15) // We don't support 15.  SCI uses it for cues and loops.
            {
                // Ensure this channel is in the mask.
                wChannelMask |= 0x1 << event.GetChannel();
            }
            if (fSkip)
            {
                dwMissingTime += event.wTimeDelta;
            }
            else
            {
                events.push_back(event);
                dwMissingTime = 0;
            }
        }
        // Assuming the midi file didn't lie...
        std::istream::pos_type curPos = midiFile.tellg();
        if (curPos != finalPos)
        {
            OutputDebugString("Corrupt MIDI file? Track length is wrong.\n");
            // Handle this by resetting it for reading the next track.
            midiFile.seekg(finalPos, std::ios_base::beg);
        }
    }

    if (GetKeyState(VK_SHIFT) & 0x8000)
    {
        // Stick a 120bpm at position 0 if this is the first entry and its not a pos 0
        TempoEntry tempo120 = { 0, 120 };
        tempoChanges.push_back(tempo120);

        TempoEntry tempoEntry = { (1500), 180 };
        tempoChanges.push_back(tempoEntry);
    }


    return wChannelMask;
}

// Returns the total ticks
DWORD CombineSoundEvents(const std::vector<std::vector<SoundEvent> > &channels, std::vector<SoundEvent> &results)
{
    // We'll need to store a position for each channel.  Let's initialize them to zero.
    std::vector<size_t> _channelPos;
    _channelPos.insert(_channelPos.begin(), channels.size(), 0);
    // And keep a running count of the time delta for each track.
    std::vector<DWORD> _channelTimeDelta;
    _channelTimeDelta.insert(_channelTimeDelta.begin(), channels.size(), 0);

    // And here's our overall time counter.
    DWORD dwTimeDelta = 0;
    DWORD dwLastTimeDelta = 0;

    while (true)
    {
        // Find the next event, time-wise
        DWORD dwClosest = (DWORD)(-1);
        size_t bestChannel = channels.size(); // out of bounds
        for (size_t i = 0; i < channels.size(); i++)
        {
            const std::vector<SoundEvent> &channelData = channels[i];
            if (_channelPos[i] < channelData.size())
            {
                // Still events remaining in this track, so it's under consideration.
                ASSERT((_channelTimeDelta[i] + channelData[_channelPos[i]].wTimeDelta) >= dwTimeDelta);
                DWORD dwTimeToNextEvent = (_channelTimeDelta[i] + channelData[_channelPos[i]].wTimeDelta) - dwTimeDelta;
                if (dwTimeToNextEvent < dwClosest)
                {
                    bestChannel = i; // This is the best track so far.
                    dwClosest = dwTimeToNextEvent;
                }
            }
        }
        if (bestChannel < channels.size())
        {
            // Add this event.
            size_t posInChannel = _channelPos[bestChannel];
            SoundEvent event = channels[bestChannel][posInChannel];
            // Now update things.
            _channelTimeDelta[bestChannel] += event.wTimeDelta;
            // This is now our new time delta
            dwTimeDelta = _channelTimeDelta[bestChannel];
            assert(dwTimeDelta < 0xf0000000);
            _channelPos[bestChannel]++; // We took one event from this track.

            // Fixup the event time before adding it.
            assert(dwTimeDelta >= dwLastTimeDelta);
            event.wTimeDelta = dwTimeDelta - dwLastTimeDelta;
            results.push_back(event);
            dwLastTimeDelta = dwTimeDelta;
        }
        else
        {
            break; // No more events left.
        }
    }
    return dwLastTimeDelta;
}

//
// Adjust the timing of each event so that it sounds the same as it currently does,
// but using a 30 PPQN time division and 120bpm (this is what the SCI interpreter uses by default)
void SoundComponent::_NormalizeToSCITempo()
{
    DWORD maxLastTimeNew = 0;

    for (auto &channelInfo : _allChannels)
    {
        DWORD dwTimeOrig = 0;
        DWORD dwLastTimeOrig = 0;
        DWORD dwLastTimeNew = 0;
        vector<SoundEvent> &events = channelInfo.Events;
        for (size_t i = 0; i < events.size(); i++)
        {
            SoundEvent &event = events[i];
            dwTimeOrig = dwLastTimeOrig + event.wTimeDelta;
            DWORD temp = (DWORD)MulDiv(MulDiv(dwTimeOrig, SCI_PPQN, (int)_wDivision), StandardTempo, _wTempoIfChanged);
            assert(temp >= dwLastTimeNew);
            event.wTimeDelta = temp - dwLastTimeNew;
            dwLastTimeNew += event.wTimeDelta;
            dwLastTimeOrig = dwTimeOrig;
        }
        maxLastTimeNew = max(maxLastTimeNew, dwLastTimeNew);
    }
    _wDivision = SCI_PPQN;
    _wTempoIfChanged = StandardTempo;
    TotalTicks = maxLastTimeNew;
}

void RemoveTempoChanges(std::vector<SoundEvent> &events, const std::vector<TempoEntry> &tempoChanges, uint16_t &tempoOut, DWORD &ticksOut)
{
    tempoOut = StandardTempo;

    if (!tempoChanges.empty())
    {
        // Find the fastest tempo, and set that as our tempo.
        TempoEntry entry = *max_element(tempoChanges.begin(), tempoChanges.end());
        tempoOut = static_cast<uint16_t>(entry.dwTempo);
        // Now adjust events
        if (tempoChanges.size() > 1)
        {
            ticksOut = 0; // We're going to recalculate this.

            // Grab the first tempo
            DWORD dwOriginalTicks = 0;
            DWORD dwOldTempo = tempoChanges[0].dwTempo;
            DWORD dwNewTempo = tempoOut;
            size_t tempoIndex = 0;
            DWORD dwTotalTicksAtOldTempo = 0;
            DWORD dwTotalTicksAtNewTempo = 0;
            for (size_t i = 0; i < events.size(); i++)
            {
                dwOriginalTicks += events[i].wTimeDelta;
                if ((tempoIndex < tempoChanges.size()) &&
                    (dwOriginalTicks >= tempoChanges[tempoIndex].dwTicks))
                {
                    // We have a new tempo starting here.
                    dwOldTempo = tempoChanges[tempoIndex].dwTempo;
                    tempoIndex++;
                    dwTotalTicksAtOldTempo = 0;
                    dwTotalTicksAtNewTempo = 0;
                }
                DWORD dwTotalTicksAtNewTempoTemp = (dwTotalTicksAtOldTempo + events[i].wTimeDelta) * dwNewTempo / dwOldTempo;
                assert(dwTotalTicksAtNewTempoTemp >= dwTotalTicksAtOldTempo);
                events[i].wTimeDelta = dwTotalTicksAtNewTempoTemp - dwTotalTicksAtOldTempo;
                ticksOut += events[i].wTimeDelta;
                assert(events[i].wTimeDelta < 65000); // Just some sanity check for overflows.
                dwTotalTicksAtNewTempo = dwTotalTicksAtNewTempoTemp;
            }
        }
    }
}

void _RemoveEvent(size_t index, std::vector<SoundEvent> &events)
{
    if (index + 1 < events.size())
    {
        // First add this event's time to the next:
        events[index + 1].wTimeDelta += events[index].wTimeDelta;
    }
    //events.erase(&events[index]);
    events.erase(events.begin() + index);
}

bool predTicks(CuePoint &cue1, CuePoint &cue2)
{
    return (cue1.GetTickPos() < cue2.GetTickPos());
}

SoundEvent _MakeCueEvent(CuePoint cue, DWORD dwTicksPrior)
{
    SoundEvent event;
    if (cue.GetType() == CuePoint::Cumulative)
    {
        event.SetRawStatus(SoundEvent::Control | 15); // REVIEW: should we OR in 15?
        ASSERT(cue.GetTickPos() >= dwTicksPrior);
        event.bParam1 = 0x60;
        event.bParam2 = cue.GetValue(); // Unclear if there is a limit here...
    }
    else
    {
        event.SetRawStatus(SoundEvent::ProgramChange | 15);
        event.bParam1 = cue.GetValue();
        ASSERT(event.bParam1 < 127);
    }
    assert(cue.GetTickPos() >= dwTicksPrior);
    event.wTimeDelta = cue.GetTickPos() - dwTicksPrior;
    return event;
}

bool SoundEvent::operator == (const SoundEvent &other)
{
    return _bStatus == other._bStatus &&
        bParam1 == other.bParam1 &&
        bParam2 == other.bParam2 &&
        wTimeDelta == other.wTimeDelta;
}
bool SoundEvent::operator!=(const SoundEvent &other)
{
    return !(*this == other);
}

const SoundEvent g_Channel16Mandatory0(0, 0, 0, 0xcf);
const SoundEvent g_Channel16Mandatory1(0, 0x50, 0x7f, 0xbf);
const SoundEvent g_Channel16Mandatory2(0, 0x0a, 0x40, 0xbf);

void _EnsureCh15Preamble(vector<SoundEvent> &events)
{
    // If we have a loop point or cues, these events are mandatory, or else the SCI interpreter will hang when playing the sound.
    DWORD ticks = 0;
    size_t i = 0;
    while ((i < events.size()) && (events[i].wTimeDelta == 0))
    {
        bool remove =
            (events[i] == g_Channel16Mandatory0) ||
            (events[i] == g_Channel16Mandatory1) ||
            (events[i] == g_Channel16Mandatory2);
        if (remove)
        {
            RemoveEvent(events, i);
        }
        else
        {
            i++;
        }
    }
    events.insert(events.begin(), g_Channel16Mandatory0);
    events.insert(events.begin() + 1, g_Channel16Mandatory1);
    events.insert(events.begin() + 2, g_Channel16Mandatory2);
}

// The user can't edit midi directly, so we only need to do this when importing midi files.
void EnsureChannelPreamble(ChannelInfo &channel)
{
    // Midi channels must start with the following events in this order:
    //  Program change
    //  Volume
    //  Pan
    // SCI apparently doesn't look at the actual midi codes, but just plucks out the values.

    // Look for these events, up to a note event. Then stick them in front, or manufacture missing ones.
    std::vector<SoundEvent> &events = channel.Events;
    SoundEvent eventProgramChange;
    SoundEvent eventVolume;
    SoundEvent eventPan;
    bool done = false;
    int foundCount = 0;
    for (size_t index = 0; !done && (foundCount < 3) && (index < events.size());)
    {
        SoundEvent::Command command = events[index].GetCommand();
        switch (command)
        {
            case SoundEvent::ProgramChange:
                if (eventProgramChange.GetCommand() != SoundEvent::ProgramChange)
                {
                    eventProgramChange = RemoveEvent(events, index);
                    foundCount++;
                    continue;
                }
                break;
            case SoundEvent::Control:
            {
                switch (events[index].bParam1)
                {
                    case 7:
                        if (eventVolume.GetCommand() != SoundEvent::Control)
                        {
                            eventVolume = RemoveEvent(events, index);
                            foundCount++;
                            continue;
                        }
                        break;
                    case 10:
                        if (eventPan.GetCommand() != SoundEvent::Control)
                        {
                            eventPan = RemoveEvent(events, index);
                            foundCount++;
                            continue;
                        }
                        break;

                }
            }
            case SoundEvent::NoteOn:
                done = true;    // Don't look past the first note
                break;
        }
        index++;
    }

    // Fill in defaults for events we didn't find.
    eventProgramChange.wTimeDelta = 0;
    if (eventProgramChange.GetCommand() != SoundEvent::ProgramChange)
    {
        eventProgramChange.SetRawStatus(SoundEvent::ProgramChange | channel.Number);
        eventProgramChange.bParam1 = 0; // Program 0 I guess?
    }
    eventVolume.wTimeDelta = 0;
    if (eventVolume.GetCommand() != SoundEvent::Control)
    {
        eventVolume.SetRawStatus(SoundEvent::Control | channel.Number);
        eventVolume.bParam1 = 7;
        eventVolume.bParam2 = 127;
    }
    eventPan.wTimeDelta = 0;
    if (eventPan.GetCommand() != SoundEvent::Control)
    {
        eventPan.SetRawStatus(SoundEvent::Control | channel.Number);
        eventPan.bParam1 = 10;
        eventPan.bParam2 = 64;
    }

    InsertEvent(events, 0, eventProgramChange);
    InsertEvent(events, 1, eventVolume);
    InsertEvent(events, 2, eventPan);
}

void SoundComponent::_ProcessBeforeSaving()
{
    // REVIEW: I'm not sure all this complexity is needed anymore, now that we store things in different
    // channels.
    for (auto &channel : _allChannels)
    {
        if (channel.Number == 15)
        {
            vector<SoundEvent> &events = channel.Events;

            // If we're saving channel 15, always gen the preamble, even if channel 15 is empty.
            _EnsureCh15Preamble(events);
        }
    }

    // SCI1 will hang if we put in tracks with no channels.
    _tracks.erase(remove_if(_tracks.begin(), _tracks.end(),
        [](TrackInfo &track) { return track.ChannelIds.empty(); }
    ), _tracks.end());
}

void SoundComponent::_RationalizeCuesAndLoops()
{
    // REVIEW: I'm not sure all this complexity is needed anymore, now that we store things in different
    // channels.
    for (auto &channel : _allChannels)
    {
        if (channel.Number == 15)
        {
            vector<SoundEvent> &events = channel.Events;

            if (_fReEvaluateLoopPoint)
            {
                // First remove any loop points
                for (size_t i = 0; i < events.size();)
                {
                    SoundEvent &event = events[i];
                    assert(event.GetChannel() == 15);
                    if ((event.GetCommand() == SoundEvent::ProgramChange) &&
                        (event.bParam1 == 127))
                    {
                        // This is a loop point.  Remove it. Don't increment the loop counter,
                        // since the vector is one smaller now.
                        _RemoveEvent(i, events);
                    }
                    else
                    {
                        i++;
                    }
                }

                if (LoopPoint != SoundComponent::LoopPointNone)
                {
                    // Now add it back in.
                    DWORD dwTotalTicks = 0;
                    bool fInserted = false;
                    size_t i = 0;
                    for (; i < events.size(); i++)
                    {
                        SoundEvent &event = events[i];
                        if ((dwTotalTicks + event.wTimeDelta) >= LoopPoint)
                        {
                            // The loop point is in between this and the last event.
                            break;
                        }
                        dwTotalTicks += event.wTimeDelta; // ... and so otherwise we could crash here.
                    }

                    SoundEvent loopEvent;
                    loopEvent.SetRawStatus(SoundEvent::ProgramChange | 15);
                    loopEvent.bParam1 = 127;
                    // Time delta is the looppoint, minus the total ticks to the previous event.
                    loopEvent.wTimeDelta = LoopPoint - dwTotalTicks;
                    if (loopEvent.wTimeDelta && (i < events.size()))
                    {
                        // Adjust the current event's time delta
                        assert(events[i].wTimeDelta >= loopEvent.wTimeDelta);
                        events[i].wTimeDelta -= loopEvent.wTimeDelta;
                    }
                    events.insert(events.begin() + i, loopEvent);
                }
            }

            if (_fReEvaluateCues)
            {
                // First remove any cue points
                for (size_t i = 0; i < events.size();)
                {
                    SoundEvent &event = events[i];
                    assert(event.GetChannel() == 15);

                    if (((event.GetCommand() == SoundEvent::ProgramChange) &&
                        (event.bParam1 < 127))
                        ||
                        ((event.GetCommand() == SoundEvent::Control) &&
                        (event.bParam1 == 0x60)))
                    {
                        // This is a cue point.  Remove it. Don't increment the loop counter,
                        // since the vector is one smaller now.
                        _RemoveEvent(i, events);
                    }
                    else
                    {
                        i++;
                    }
                }

                vector<CuePoint> cuesCopy = Cues;
                // Now add cue points back in.
                // However, first we need to sort our cue point array.
                sort(cuesCopy.begin(), cuesCopy.end(), predTicks);
                // If we have any cues, we need to place another cue at zero with value zero. Otherwise, the interpreter hangs
                if (!cuesCopy.empty() && (cuesCopy[0].GetTickPos() != 0))
                {
                    // Insert a non-cumulative cue with value zero at time zero
                    cuesCopy.insert(cuesCopy.begin(), CuePoint(CuePoint::Type::NonCumulative, 0, 0));
                }

                DWORD dwTotalTicks = 0;
                std::vector<CuePoint>::iterator cueIt = cuesCopy.begin();
                size_t cueIndex = 0;
                for (; (cueIt != cuesCopy.end()) && (cueIndex < events.size()); cueIndex++)
                {
                    bool fDoneWithCuesHere = false;
                    while (!fDoneWithCuesHere)
                    {
                        fDoneWithCuesHere = true;
                        SoundEvent &event = events[cueIndex];
                        DWORD wTimeDelta = event.wTimeDelta; // stash this, because event might become invalid.
                        if ((dwTotalTicks + event.wTimeDelta) >= cueIt->GetTickPos())
                        {
                            // The cue is in between this and the last event.
                            SoundEvent cueEvent = _MakeCueEvent(*cueIt, dwTotalTicks);
                            if (cueEvent.wTimeDelta)
                            {
                                // Adjust the current event's time delta
                                assert(event.wTimeDelta >= cueEvent.wTimeDelta);
                                event.wTimeDelta -= cueEvent.wTimeDelta;
                            }
                            //Events.insert(&Events[i], cueEvent); // we may have invalidated SoundEvent &event now.
                            events.insert(events.begin() + cueIndex, cueEvent); // we may have invalidated SoundEvent &event now.
                            ++cueIt;        // Go to the next cue.
                            ++cueIndex;     // We just added one to the events, so double-increment this.
                            if (cueIt != cuesCopy.end())
                            {
                                // Perhaps there is another cue point to be inserted in this spot?
                                fDoneWithCuesHere = false;
                            }
                            dwTotalTicks += cueEvent.wTimeDelta;
                        }
                        else
                        {
                            fDoneWithCuesHere = true;
                            dwTotalTicks += wTimeDelta;
                        }
                    }
                }
                // Any more at the end:
                while (cueIt != cuesCopy.end())
                {
                    SoundEvent cueEvent = _MakeCueEvent(*cueIt, dwTotalTicks);
                    if (cueEvent.wTimeDelta && (cueIndex < events.size()))
                    {
                        // Adjust the current event's time delta
                        assert(events[cueIndex].wTimeDelta >= cueEvent.wTimeDelta);
                        events[cueIndex].wTimeDelta -= cueEvent.wTimeDelta;
                    }
                    events.insert(events.begin() + cueIndex, cueEvent);
                    dwTotalTicks += cueEvent.wTimeDelta;
                    ++cueIt;
                    ++cueIndex;
                }
            }
        }
    }
    _fReEvaluateCues = false;
    _fReEvaluateLoopPoint = false;

    if ((_wTempoIfChanged != StandardTempo) || (_wDivision != SCI_PPQN))
    {
        _NormalizeToSCITempo();
        _fCanSetTempo = false;
    }

}

SoundChangeHint SoundComponent::AddCuePoint(CuePoint cue)
{
    SoundChangeHint hint = SoundChangeHint::None;
    hint |= _EnsureChannel15();
    Cues.push_back(cue);
    return hint | SoundChangeHint::CueChanged;
}

SoundChangeHint SoundComponent::DeleteCue(size_t index)
{
    if (index < Cues.size())
    {
        //Cues.erase(&Cues[index]);
        Cues.erase(Cues.begin() + index);
        _fReEvaluateCues = true;
        return SoundChangeHint::CueChanged;
    }
    return SoundChangeHint::None;
}

SoundChangeHint SoundComponent::SetCue(size_t index, CuePoint cue)
{
    if (index < Cues.size())
    {
        Cues[index] = cue;
        _fReEvaluateCues = true;
        return SoundChangeHint::CueChanged;
    }
    return SoundChangeHint::None;
}

SoundChangeHint SoundComponent::SetLoopPoint(DWORD dwTicks)
{
    SoundChangeHint hint = SoundChangeHint::None;
    if (dwTicks != LoopPointNone)
    {
        hint |= _EnsureChannel15();
    }
    if (LoopPoint != dwTicks)
    {
        LoopPoint = dwTicks;
        _fReEvaluateLoopPoint = true;
        hint |= SoundChangeHint::Changed;
    }
    return hint;
}

const uint32_t SizeLimit = 0xfe00;

void WriteChannelStream(const vector<SoundEvent> &events, sci::ostream &byteStream, uint8_t channelNumber = 0xff)
{
    // Now the events.
    uint8_t bLastStatus = 0;
    DWORD dwAddToLastTimeDelta = 0;
    // Don't allow more than about 64K
    for (size_t i = 0; i < events.size() && (byteStream.tellp() < SizeLimit); i++)
    {
        const SoundEvent &event = events[i];

        assert((channelNumber = 0xff) || (event.GetChannel() == 0x15) || (event.GetChannel() == channelNumber));

        if ((event.GetCommand() == SoundEvent::Special) && (event.GetRawStatus() != 0xFC))  // 0xFC = stop bit.
        {
            // Ignore this event except for the time.
            dwAddToLastTimeDelta += event.wTimeDelta;
        }
        else
        {
            DWORD dwTimeDelta = event.wTimeDelta + dwAddToLastTimeDelta;
            // First the time delta...
            while (dwTimeDelta >= 240)
            {
                byteStream.WriteByte(0xf8);
                dwTimeDelta -= 240;
            }
            byteStream.WriteByte((uint8_t)dwTimeDelta);

            // Then the status byte if it's different from the previous one.
            uint8_t bCurrentStatus = event.GetRawStatus();
            if (bLastStatus != bCurrentStatus)
            {
                byteStream.WriteByte(bCurrentStatus);
                bLastStatus = bCurrentStatus;
            }

            switch (event.GetCommand())
            {
                case SoundEvent::NoteOff:
                case SoundEvent::NoteOn:
                case SoundEvent::KeyPressure:
                case SoundEvent::Control:
                case SoundEvent::PitchWheel:
                    byteStream.WriteByte(event.bParam1);
                    byteStream.WriteByte(event.bParam2);
                    break;
                case SoundEvent::ProgramChange:
                case SoundEvent::Pressure:
                    byteStream.WriteByte(event.bParam1);
                    break;
                case SoundEvent::Special:
                    assert(event.GetRawStatus() == 0xFC);
                    // no params
                    break;
                default:
                    assert(false);
                    break;
            }
            dwAddToLastTimeDelta = 0;
        }
    }
    if (bLastStatus != 0xFC)
    {
        // Stick a stop status byte in if there wasn't one already.
        byteStream.WriteByte(0); // time delta
        byteStream.WriteByte(0xFC);
    }
}

void SoundWriteToWorker(const SoundComponent &sound, sci::ostream &byteStream)
{
    // Make a copy of ourselves.
    SoundComponent soundCopy = sound;
    soundCopy._ProcessBeforeSaving();

    // Special crazy case for sound resources.
    if (sound._fReEvaluateLoopPoint || sound._fReEvaluateCues || ((sound._wTempoIfChanged != StandardTempo) || (sound._wDivision != SCI_PPQN)))
    {
        soundCopy._RationalizeCuesAndLoops();
    }

    vector<SoundEvent> events;
    // We should only ever have one channel of each for SCI0
    vector<vector<SoundEvent>> allEvents;
    allEvents.reserve(16);
    for (auto &channel : soundCopy._allChannels)
    {
        allEvents.push_back(channel.Events);
    }
    CombineSoundEvents(allEvents, events);

    byteStream.WriteByte(0); // Digital sample -> no

    // Channel information:
    for (size_t i = 0; i < SCI0ChannelCount; i++)
    {
        // Each word tells us which devices use this channel.
        // We can leave the lower byte blank (it should be ignored), and the upper 8 bits indicate which devices.
        uint16_t channelMask = 0;
        for (const auto &trackInfo : soundCopy._tracks)
        {
            for (int channelId : trackInfo.ChannelIds)
            {
                if (soundCopy._allChannels[channelId].Number == i)
                {
                    // This channel is used for this device.
                    channelMask |= (trackInfo.Type << 8);
                }
            }
        }
        byteStream.WriteWord(channelMask);
    }
    byteStream.WriteWord(0); // Digital sample offset - we don't care about this for SCI0.
        
    WriteChannelStream(events, byteStream);
}

void SoundWriteTo(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag)
{
    const SoundComponent &sound = resource.GetComponent<SoundComponent>();
    SoundWriteToWorker(sound, byteStream);
}

void SoundWriteToWorker_SCI1(const SoundComponent &soundIn, sci::ostream &byteStream)
{
    // Make a copy of ourselves.
    SoundComponent soundCopy = soundIn;
    soundCopy._ProcessBeforeSaving();

    // Special crazy case for sound resources.
    if (soundIn._fReEvaluateLoopPoint || soundIn._fReEvaluateCues || ((soundIn._wTempoIfChanged != StandardTempo) || (soundIn._wDivision != SCI_PPQN)))
    {
        soundCopy._RationalizeCuesAndLoops();
    }

    AssertNoDuplicateTracks(soundCopy);

    // Track [Type] 
    //  Channel Channel Channel  [unknown][dataOffset][dataSize]
    sci::ostream channelStream;
    vector<pair<uint16_t, uint16_t>> channelOffsetAndSize;
    for (auto &channelInfo : soundCopy.GetChannelInfos())
    {
        uint16_t offset = (uint16_t)channelStream.tellp();
        uint8_t number = channelInfo.Number & 0xf;
        uint8_t flags = 0;
        if (channelInfo.Number == 9)
        {
            // REVIEW: There are also flags 1 and 4 here, which we don't yet support.
            flags |= 2;
        }
        number |= (flags << 4);
        channelStream << number;
        uint8_t polyAndPrio = channelInfo.Priority << 4;
        polyAndPrio |= channelInfo.Poly ? 0x1 : 0;
        channelStream << polyAndPrio;

        // Write the event stream.
        WriteChannelStream(channelInfo.Events, channelStream, channelInfo.Number);

        // Keep track of offset and size.
        uint16_t dataSize = (uint16_t)(channelStream.tellp() - offset);
        channelOffsetAndSize.emplace_back(offset, dataSize);
    }

    // Do tracks. First, we need to know ahead of time how much space all the tracks will take up.
    // It is:
    uint16_t baseOffset = 1;    // For final track marker
    for (auto &trackInfo : soundCopy.GetTrackInfos())
    {
        baseOffset += 2;                                            // For type, and last channel marker
        baseOffset += (uint16_t)(trackInfo.ChannelIds.size() * 6);  // 6 bytes per channel
    }

    const uint8_t endMarker = 0xff;
    for (auto &trackInfo : soundCopy.GetTrackInfos())
    {
        byteStream << trackInfo.Type;
        for (int channelId : trackInfo.ChannelIds)
        {
            uint16_t unknown = 0;
            byteStream << unknown;
            byteStream << (uint16_t)(channelOffsetAndSize[channelId].first + baseOffset);
            byteStream << channelOffsetAndSize[channelId].second;
        }
        byteStream << endMarker; // End of channels
    }
    byteStream << endMarker; // End of tracks

    assert(byteStream.tellp() == baseOffset);

    // Append the channel data.
    sci::transfer(sci::istream_from_ostream(channelStream), byteStream, channelStream.tellp());
}

void SoundWriteTo_SCI1(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag)
{
    const SoundComponent &sound = resource.GetComponent<SoundComponent>();
    SoundWriteToWorker_SCI1(sound, byteStream);
}

void ReadChannel(sci::istream &stream, std::vector<SoundEvent> &events, DWORD &totalTicks, SoundComponent &sound, int *mustBeChannel)
{
    // Get delta time.
    bool fDone = false;
    totalTicks = 0;
    while (!fDone && stream.good())
    {
        SoundEvent event;
        // Get the delta time
        _GetDeltaTime(stream, &event.wTimeDelta);
        uint8_t bStatus;
        if (stream.good())
        {
            totalTicks += event.wTimeDelta;
            stream >> bStatus;
        }
        if (stream.good())
        {
            if (_IsStatusByte(bStatus))
            {
                event.SetRawStatus(bStatus);
            }
            else
            {
                // Repeat last status.
                if (!events.empty())
                {
                    event.SetRawStatus(events.back().GetRawStatus());
                }
                // else TODO: error conditions

                // And back up one byte
                stream.seekg(stream.tellg() - 1);
            }

            switch (event.GetCommand())
            {
            case SoundEvent::NoteOff:
            case SoundEvent::NoteOn:
            case SoundEvent::KeyPressure:
            case SoundEvent::Control:
            case SoundEvent::PitchWheel:
                stream >> event.bParam1;
                stream >> event.bParam2;
                if ((event.GetCommand() == SoundEvent::Control) && (event.bParam1 == 0x60))
                {
                    CuePoint cp(CuePoint::Cumulative, totalTicks, event.bParam2);
                    sound.Cues.push_back(cp);
                }
                break;

            case SoundEvent::ProgramChange:
            case SoundEvent::Pressure:
                stream >> event.bParam1;
                if (event.GetCommand() == SoundEvent::ProgramChange)
                {
                    if (event.GetChannel() == 15)
                    {
                        if (event.bParam1 < 127)
                        {
                            CuePoint cp(CuePoint::NonCumulative, totalTicks, event.bParam1);
                            sound.Cues.push_back(cp);
                        }
                        else if (event.bParam1 == 127)
                        {
                            sound.LoopPoint = totalTicks;
                        }
                        // else nothign
                    }
                    // fall through
                }
                break;

            case SoundEvent::Special:
                switch (bStatus)
                {
                case 0xF0:
                {
                    // Eat bytes until 0xF7
                    bool fNotEnd = true;
                    while (stream.good() && fNotEnd)
                    {
                        uint8_t bByte;
                        stream >> bByte;
                        fNotEnd = (bByte != 0xF7);
                    }
                    if (stream.good())
                    {
                        stream.seekg(stream.tellg() - 1); // Go back one.
                    }
                }
                break;
                case 0xFC:
                    fDone = true;
                    break;
                default:
                    assert(false); // TODO: deal with this: improper sound
                    break;
                }
                break;
            }

            if (mustBeChannel)
            {
                // Castle of Dr Brain hits this.
                /*
                if ((event.GetChannel() != *mustBeChannel) && (event.GetChannel() != 0x0F))
                {
                    throw std::exception("Corrupt audio resource.");
                }*/
            }

            if (!fDone)
            {
                // Don't add the last stop byte (we'll add it back in when writing)
                events.push_back(event);
            }
        }
    }
}

void SoundReadFrom_SCI1(ResourceEntity &resource, sci::istream &stream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    SoundComponent &sound = resource.GetComponent<SoundComponent>();
    sound.TotalTicks = 0;
    int trackCount = 0;
    sci::istream trackCountStream = stream;
    uint8_t marker;
    trackCountStream >> marker;
    while (trackCountStream.good() && (marker != 0xff))
    {
        trackCount++;
        uint8_t channelMarker;
        trackCountStream >> channelMarker;
        while (channelMarker != 0xff)
        {
            trackCountStream.skip(5);
            trackCountStream >> channelMarker;
        }
        trackCountStream >> marker;
    }

    map<uint16_t, int> dataOffsetToChannelId;
    map<uint16_t, uint16_t> dataOffsetToSize;   // Temporary, to verify one offset always has same size

    bool encounteredChannel15 = false;

    for (int trackNumber = 0; trackNumber < trackCount; trackNumber++)
    {
        sound._tracks.emplace_back();
        TrackInfo &track = sound._tracks.back();

        stream >> track.Type;
        
        int channelCount = 0;
        sci::istream channelCountStream = stream;
        channelCountStream >> marker;
        while (channelCountStream.good() && (marker != 0xff))
        {
            channelCount++;
            channelCountStream.skip(5);
            channelCountStream >> marker;
        }

        if (track.Type != 0xf0) // Digital track, not supported
        {
            int channelNumber = 0;
            while (channelCount)
            {
                uint16_t unknown, dataOffset, dataSize; 
                stream >> unknown;
                stream >> dataOffset;
                stream >> dataSize;

                auto it = dataOffsetToChannelId.find(dataOffset);
                if (it != dataOffsetToChannelId.end())
                {
                    // We already processed this channel, just add the id to the track
                    assert(dataOffsetToSize[dataOffset] == dataSize);
                    track.ChannelIds.push_back(it->second);
                }
                else
                {
                    sound._allChannels.emplace_back();
                    ChannelInfo &channelInfo = sound._allChannels.back();
                    channelInfo.Id = (int)(sound._allChannels.size() - 1);
                    sci::istream channelStream = stream;
                    channelStream.seekg(dataOffset);
                    channelStream >> channelInfo.Number;
                    if (channelInfo.Number == 0xfe) // Digital channel
                    {
                        resource.AddComponent(move(make_unique<AudioComponent>()));
                        AudioComponent &audio = resource.GetComponent<AudioComponent>();

                        channelStream >> audio.Frequency;
                        uint16_t sampleSize, offset, end;
                        channelStream >> sampleSize;
                        channelStream >> offset;
                        channelStream >> end;
                        // TODO: Somehow indicate a digital channel.

                        if (sampleSize)
                        {
                            audio.DigitalSamplePCM.assign(sampleSize, 0);
                            channelStream.read_data(&audio.DigitalSamplePCM[0], audio.DigitalSamplePCM.size());
                        }
                    }
                    else
                    {

                        uint8_t polyAndPrio;
                        channelStream >> polyAndPrio;
                        channelInfo.Poly = ((polyAndPrio & 0xf) != 0);
                        channelInfo.Priority = polyAndPrio >> 4;

                        channelInfo.Flags = channelInfo.Number >> 4;
                        channelInfo.Number = channelInfo.Number & 0xf;

                        if (channelInfo.Number == 9)
                        {
                            channelInfo.Flags |= 2;
                        } 

                        // Flag 0x1:    Channel offset start is 0 instead of 10 (?) (haven't encountered this yet)
                        // Flag 0x2:    Prevent re-mapping (commonly set on 9)
                        // Flag 0x4:    Start muted (?)
                    }

                    assert((!encounteredChannel15 || (channelInfo.Number != 15)) && "Encountered two channel 15s");
                    encounteredChannel15 = (channelInfo.Number == 15);

                    // Now we're ready to read the channel data?
                    int chanNum = channelInfo.Number;
                    DWORD totalTicks;
                    ReadChannel(channelStream, channelInfo.Events, totalTicks, sound, &chanNum);
                    sound.TotalTicks = max(sound.TotalTicks, totalTicks);

                    dataOffsetToChannelId[dataOffset] = channelInfo.Id;
                    dataOffsetToSize[dataOffset] = dataSize;
                    track.ChannelIds.push_back(channelInfo.Id);
                }
                channelCount--;
            }
        }
        else
        {
            // The first byte of the 0xF0 track's channel list is priority
            // TODO: What is this? Is this cues?

            stream.skip(6);
        }
        stream.skip(1); // Skip ff that closes channels list.
    }

    AssertNoDuplicateTracks(sound);
}

void ScanAndReadDigitalSample(ResourceEntity &resource, sci::istream stream)
{
    resource.AddComponent(move(make_unique<AudioComponent>()));
    AudioComponent &audio = resource.GetComponent<AudioComponent>();

    stream.seekg(0);
    uint32_t offset = 0;
    while (!offset && (stream.getBytesRemaining() > 0))
    {
        uint8_t marker;
        stream >> marker;
        if (marker == 0xfc)
        {
            // Sometimes two 0xfc's in a row
            if ((stream.getBytesRemaining() > 0) && (stream.peek() == 0xfc))
            {
                stream.skip(1);
            }
            offset = stream.tellg();
        }
    }

    if (offset)
    {
        stream.skip(14);
        stream >> audio.Frequency;
        stream.skip(16);
        uint16_t length;
        stream >> length;
        stream.skip(10);
        // Now comes the wave data.
        assert(stream.getBytesRemaining() == length);
        audio.DigitalSamplePCM.assign(stream.getBytesRemaining(), 0);
        stream.read_data(&audio.DigitalSamplePCM[0], audio.DigitalSamplePCM.size());
    }
}

void ConvertSCI0ToNewFormat(const vector<SoundEvent> &events, SoundComponent &sound, uint16_t *channelsSCI0)
{
    // Given these:
    //  uint16_t _channels[15];
    //  std::vector<SoundEvent> Events;
    //
    // Turn them into these:
    //  std::vector<ChannelInfo> _allChannels;
    //  std::vector<TrackInfo> _tracks;

    map<uint8_t, set<int>> tracksToUsedChannelNumbers;
    set<int> usedChannelNumbers;
    if (channelsSCI0)
    {
        for (int channelNumber = 0; channelNumber < SCI0ChannelCount; channelNumber++)
        {
            uint16_t mask = channelsSCI0[channelNumber];
            mask >>= 8; // We're only interested in the upper 8 bits
            for (uint8_t trackOrder = 0; trackOrder < 8; trackOrder++)
            {
                if (mask & 0x1)
                {
                    tracksToUsedChannelNumbers[0x1 << trackOrder].insert(channelNumber);
                    usedChannelNumbers.insert(channelNumber);
                }
                mask >>= 1;
            }
        }

        bool channel15 = false;
        for (const auto &soundEvent : events)
        {
            if (soundEvent.GetChannel() == 15)
            {
                channel15 = true;
                usedChannelNumbers.insert(15);
                break;
            }
        }
    }
    else
    {
        // SCI1
        for (const SoundEvent &event : events)
        {
            usedChannelNumbers.insert(event.GetChannel());
        }
    }
    // usedChannelNumbers.insert(15); // No, never! Not needed in base case. Causes issues if we have no cues/loops

    // Roland always has channel 9
    if (channelsSCI0)
    {
        tracksToUsedChannelNumbers[(uint8_t)DeviceType::RolandMT32].insert(9);
    }

    // Construct the channels and put the separated sound events into them
    map<int, int> channelNumberToId;
    for (int channelNumber : usedChannelNumbers)
    {
        sound._allChannels.emplace_back();
        ChannelInfo &channel = sound._allChannels.back();
        channel.Id = (int)(sound._allChannels.size() - 1);
        channel.Number = channelNumber;
        channelNumberToId[channel.Number] = channel.Id;
        DWORD ticksSoFar = 0;
        DWORD prevTicks = 0;
        for (const SoundEvent &event : events)
        {
            ticksSoFar += event.wTimeDelta;
            if (event.GetChannel() == channelNumber)
            {
                SoundEvent newEvent = event;
                assert(ticksSoFar >= prevTicks);
                newEvent.wTimeDelta = ticksSoFar - prevTicks;
                prevTicks = ticksSoFar;
                channel.Events.push_back(newEvent);
            }
        }
    }

    // Now build the tracks and associated channels with them
    for (auto &trackAndChannelNumbers : tracksToUsedChannelNumbers)
    {
        sound._tracks.emplace_back();
        TrackInfo &track = sound._tracks.back();
        track.Type = trackAndChannelNumbers.first;
        for (int channelNumber : trackAndChannelNumbers.second)
        {
            int channelId = -1;
            for (auto &channelInfo : sound.GetChannelInfos())
            {
                if (channelInfo.Number == channelNumber)
                {
                    channelId = channelInfo.Id;
                    break;
                }
            }
            if (channelId != -1)
            {
                track.ChannelIds.push_back(channelId);
            }
            else
            {
                assert(channelNumber == 9); // So it's because of the Roland thing.
            }
        }
    }

    AssertNoDuplicateTracks(sound);
}

void SoundReadFrom_SCI0(ResourceEntity &resource, sci::istream &stream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    SoundComponent &sound = resource.GetComponent<SoundComponent>();

    uint8_t b;
    stream >> b; // Digital sample flag - we don't care about this.
    // Apprently if it's 2, we need to add a digital sample data channel.

    uint16_t channels[SCI0ChannelCount];
    for (int i = 0; i < ARRAYSIZE(channels); ++i)
    {
        stream >> channels[i];
    }
    uint16_t wSoundEffectOffset;
    stream >> wSoundEffectOffset;

    vector<SoundEvent> events;
    ReadChannel(stream, events, sound.TotalTicks, sound);

    if (!stream.good())
    {
        appState->LogInfo("Corrupt sound resource.\n");
    }
    else
    {
        if (b == 0x2)
        {
            ScanAndReadDigitalSample(resource, stream);
        }
    }

    ConvertSCI0ToNewFormat(events, sound, channels);
}

bool ValidateSoundResource(const ResourceEntity &resource)
{
    bool save = true;
    const SoundComponent &sound = resource.GetComponent<SoundComponent>();
    
    for (const auto &cuePoint : sound.GetCuePoints())
    {
        if ((cuePoint.GetTickPos() != 0) && (cuePoint.GetValue() == 0))
        {
            save = (IDYES == AfxMessageBox("Sound contains a cumulative cue with a zero value after time 0. This won't trigger anything. Save anyway?", MB_ICONWARNING | MB_YESNO));
            break;
        }
    }

    if (save)
    {
        if (sound.GetTrackInfos().empty())
        {
            save = (IDYES == AfxMessageBox("No channels have been enabled for any sound devices, so no music will play. Save anyway?", MB_ICONWARNING | MB_YESNO));
        }
    }

    return save;
}

ResourceTraits soundResTraits =
{
    ResourceType::Sound,
    &SoundReadFrom_SCI0,
    &SoundWriteTo,
    &ValidateSoundResource,
    nullptr
};

// REVIEW: We don't even know how to read SCI1 sounds yet.
ResourceTraits soundResTraitsSCI1 =
{
    ResourceType::Sound,
    &SoundReadFrom_SCI1,
    &SoundWriteTo_SCI1,
    &ValidateSoundResource,
    nullptr
};

SoundTraits soundTraitsSCI0 =
{
    true,
};

SoundTraits soundTraitsSCI1 =
{
    false,
};

ResourceEntity *CreateSoundResource(SCIVersion version)
{
    SoundTraits *ptraits = &soundTraitsSCI0;
    if (version.SoundFormat == SoundFormat::SCI1)
    {
        ptraits = &soundTraitsSCI1;
    }
    std::unique_ptr<ResourceEntity> pResource = std::make_unique<ResourceEntity>((version.SoundFormat == SoundFormat::SCI0) ? soundResTraits : soundResTraitsSCI1);
    pResource->AddComponent(move(make_unique<SoundComponent>(*ptraits)));
    return pResource.release();
}

ResourceEntity *CreateDefaultSoundResource(SCIVersion version)
{
    // Nothing different.
    return CreateSoundResource(version);
}