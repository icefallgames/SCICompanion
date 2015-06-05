#include "stdafx.h"
#include "Sound.h"
#include "AppState.h"
#include "ResourceEntity.h"
#include "Audio.h"
#include "format.h"

#pragma comment( lib, "winmm.lib" )

using namespace std;

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


SoundEvent::Command _GetSoundCommand(uint8_t bStatus)
{
    return (SoundEvent::Command)(bStatus & 0xF0);
}

uint8_t _GetChannel(uint8_t bStatus)
{
    return bStatus & 0x0F; // Lower nibble
}

SoundComponent::SoundComponent(const SoundTraits &traits) : Traits(traits)
{
    _wDivision = SCI_PPQN; // by default
    TotalTicks = 0;
    _fCanSetTempo = false;
    _fReEvaluateCues = false;
    _fReEvaluateLoopPoint = false;
    _wTempoIfChanged = StandardTempo;
    LoopPoint = LoopPointNone;
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

SoundChangeHint SoundComponent::SetChannelMask(DeviceType device, uint16_t wChannels)
{
    TrackInfo newTrackInfo;
    newTrackInfo.Type = (uint8_t)device;
    for (uint8_t i = 0; i < ChannelCount; ++i)
    {
        if (wChannels & (0x0001 << i))
        {
            // This channel is used. Find this channel info
            for (auto &channelInfo : _allChannels)
            {
                if (channelInfo.Number == i)
                {
                    newTrackInfo.ChannelIds.push_back(channelInfo.Id);
                    break;
                }
            }
        }
    }
    // Now find the track info for this guy. If none, add a new one.
    bool found = false;
    for (TrackInfo &trackInfo : _tracks)
    {
        if (trackInfo.Type == newTrackInfo.Type)
        {
            // Replace
            trackInfo = newTrackInfo;
            found = true;
            break;
        }
    }
    if (!found)
    {
        // else add
        _tracks.push_back(newTrackInfo);
    }
    
    return SoundChangeHint::Changed;
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
                            TempoEntry tempoEntry = { (totalTicksOut + event.wTimeDelta), ((60000000 / dwMSPerQN)) };
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
    return wChannelMask;
}

// Returns the total ticks
DWORD CombineSoundEvents(const std::vector<std::vector<SoundEvent> > &tracks, std::vector<SoundEvent> &results)
{
    // We'll need to store a position for each track.  Let's initialize them to zero.
    std::vector<size_t> _trackPos;
    _trackPos.insert(_trackPos.begin(), tracks.size(), 0);
    // And keep a running count of the time delta for each track.
    std::vector<DWORD> _trackTimeDelta;
    _trackTimeDelta.insert(_trackTimeDelta.begin(), tracks.size(), 0);

    // And here's our overall time counter.
    DWORD dwTimeDelta = 0;
    DWORD dwLastTimeDelta = 0;

    while (true)
    {
        // Find the next event, time-wise
        DWORD dwClosest = (DWORD)(-1);
        size_t bestTrack = tracks.size(); // out of bounds
        for (size_t i = 0; i < tracks.size(); i++)
        {
            const std::vector<SoundEvent> &track = tracks[i];
            if (_trackPos[i] < track.size())
            {
                // Still events remaining in this track, so it's under consideration.
                ASSERT((_trackTimeDelta[i] + track[_trackPos[i]].wTimeDelta) >= dwTimeDelta);
                DWORD dwTimeToNextEvent = (_trackTimeDelta[i] + track[_trackPos[i]].wTimeDelta) - dwTimeDelta;
                if (dwTimeToNextEvent < dwClosest)
                {
                    bestTrack = i; // This is the best track so far.
                    dwClosest = dwTimeToNextEvent;
                }
            }
        }
        if (bestTrack < tracks.size())
        {
            // Add this event.
            size_t posInTrack = _trackPos[bestTrack];
            SoundEvent event = tracks[bestTrack][posInTrack];
            // Now update things.
            _trackTimeDelta[bestTrack] += event.wTimeDelta;
            // This is now our new time delta
            dwTimeDelta = _trackTimeDelta[bestTrack];
            _trackPos[bestTrack]++; // We took one event from this track.

            // Fixup the event time before adding it.
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
            event.wTimeDelta = (DWORD)MulDiv(MulDiv(dwTimeOrig, SCI_PPQN, (int)_wDivision), StandardTempo, _wTempoIfChanged) - dwLastTimeNew;
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
    return (cue1.GetTickPos() <= cue2.GetTickPos());
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
    event.wTimeDelta = cue.GetTickPos() - dwTicksPrior;
    return event;
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
                    for (size_t i = 0; i < events.size(); i++)
                    {
                        SoundEvent &event = events[i];
                        if ((dwTotalTicks + event.wTimeDelta) >= LoopPoint)
                        {
                            // The loop point is in between this and the last event.
                            SoundEvent loopEvent;
                            loopEvent.SetRawStatus(SoundEvent::ProgramChange | 15);
                            loopEvent.bParam1 = 127;
                            // Time delta is the looppoint, minus the total ticks to the previous event.
                            loopEvent.wTimeDelta = LoopPoint - dwTotalTicks;
                            if (loopEvent.wTimeDelta)
                            {
                                // Adjust the current event's time delta
                                event.wTimeDelta -= loopEvent.wTimeDelta;
                            }
                            //Events.insert(&Events[i], loopEvent);
                            events.insert(events.begin() + i, loopEvent);
                            fInserted = true;
                            break; // Break instead of using a loop variable, since insert invalidated 'event'
                        }
                        dwTotalTicks += event.wTimeDelta; // ... and so otherwise we could crash here.
                    }
                    ASSERT(fInserted);
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
                // Now add cue points back in.
                // However, first we need to sort our cue point array.
                sort(Cues.begin(), Cues.end(), predTicks);

                DWORD dwTotalTicks = 0;
                std::vector<CuePoint>::iterator cueIt = Cues.begin();
                for (size_t i = 0; (cueIt != Cues.end()) && (i < events.size()); i++)
                {
                    bool fDoneWithCuesHere = false;
                    while (!fDoneWithCuesHere)
                    {
                        fDoneWithCuesHere = true;
                        SoundEvent &event = events[i];
                        DWORD wTimeDelta = event.wTimeDelta; // stash this, because event might become invalid.
                        if ((dwTotalTicks + event.wTimeDelta) >= cueIt->GetTickPos())
                        {
                            // The cue is in between this and the last event.
                            SoundEvent cueEvent = _MakeCueEvent(*cueIt, dwTotalTicks);
                            if (cueEvent.wTimeDelta)
                            {
                                // Adjust the current event's time delta
                                event.wTimeDelta -= cueEvent.wTimeDelta;
                            }
                            //Events.insert(&Events[i], cueEvent); // we may have invalidated SoundEvent &event now.
                            events.insert(events.begin() + i, cueEvent); // we may have invalidated SoundEvent &event now.
                            ++cueIt;    // Go to the next cue.
                            ++i;        // We just added one to the events, so double-increment this.
                            if (cueIt != Cues.end())
                            {
                                // Perhaps there is another cue point to be inserted in this spot?
                                fDoneWithCuesHere = false;
                            }
                        }
                        else
                        {
                            fDoneWithCuesHere = true;
                        }
                        dwTotalTicks += wTimeDelta;
                    }
                }
                ASSERT(cueIt == Cues.end()); // Otherwise we missed one.  Cues can't be past the end of the ticks, so we should
                // have used all of them.
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
    Cues.push_back(cue);
    return SoundChangeHint::CueChanged;
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
    if (LoopPoint != dwTicks)
    {
        LoopPoint = dwTicks;
        _fReEvaluateLoopPoint = true;
        return SoundChangeHint::Changed;
    }
    return SoundChangeHint::None;
}

void WriteChannelStream(const vector<SoundEvent> &events, sci::ostream &byteStream, uint8_t channelNumber = 0xff)
{
    // Now the events.
    uint8_t bLastStatus = 0;
    DWORD dwAddToLastTimeDelta = 0;
    // Don't allow more than about 64K
    for (size_t i = 0; i < events.size() && (byteStream.tellp() < 0x0000fe00); i++)
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
    vector<SoundEvent> events;

    // We should only ever have one channel of each for SCI0
    vector<vector<SoundEvent>> allEvents;
    allEvents.reserve(16);
    for (auto &channel : sound._allChannels)
    {
        allEvents.push_back(channel.Events);
    }
    CombineSoundEvents(allEvents, events);

    // Special crazy case for sound resources.
    if (sound._fReEvaluateLoopPoint || sound._fReEvaluateCues || ((sound._wTempoIfChanged != StandardTempo) || (sound._wDivision != SCI_PPQN)))
    {
        // Make a copy of ourselves.
        SoundComponent soundCopy = sound;
        soundCopy._RationalizeCuesAndLoops();
        // And serialize that instead.
        SoundWriteToWorker(soundCopy, byteStream);
    }
    else
    {
        byteStream.WriteByte(0); // Digital sample -> no

        // Channel information:
        for (size_t i = 0; i < ChannelCount; i++)
        {
            // Each word tells us which devices use this channel.
            // We can leave the lower byte blank (it should be ignored), and the upper 8 bits indicate which devices.
            uint16_t channelMask = 0;
            for (const auto &trackInfo : sound._tracks)
            {
                for (int channelId : trackInfo.ChannelIds)
                {
                    if (sound._allChannels[channelId].Number == i)
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
}

void SoundWriteTo(const ResourceEntity &resource, sci::ostream &byteStream)
{
    SoundComponent &sound = resource.GetComponent<SoundComponent>();
    SoundWriteToWorker(sound, byteStream);
}

void SoundWriteToWorker_SCI1(const SoundComponent &sound, sci::ostream &byteStream)
{
    // Special crazy case for sound resources.
    if (sound._fReEvaluateLoopPoint || sound._fReEvaluateCues || ((sound._wTempoIfChanged != StandardTempo) || (sound._wDivision != SCI_PPQN)))
    {
        // Make a copy of ourselves.
        SoundComponent soundCopy = sound;
        soundCopy._RationalizeCuesAndLoops();
        // And serialize that instead.
        SoundWriteToWorker_SCI1(soundCopy, byteStream);
    }
    else
    {
        // Track [Type] 
        //  Channel Channel Channel  [unknown][dataOffset][dataSize]
        sci::ostream channelStream;
        vector<pair<uint16_t, uint16_t>> channelOffsetAndSize;
        for (auto &channelInfo : sound.GetChannelInfos())
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
        for (auto &trackInfo : sound.GetTrackInfos())
        {
            baseOffset += 2;                                            // For type, and last channel marker
            baseOffset += (uint16_t)(trackInfo.ChannelIds.size() * 6);  // 6 bytes per channel
        }

        const uint8_t endMarker = 0xff;
        for (auto &trackInfo : sound.GetTrackInfos())
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
}

void SoundWriteTo_SCI1(const ResourceEntity &resource, sci::ostream &byteStream)
{
    SoundComponent &sound = resource.GetComponent<SoundComponent>();
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
                            sound.LoopPoint = sound.TotalTicks;
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
                assert((event.GetChannel() == *mustBeChannel) || (event.GetChannel() == 0x0F));
            }

            events.push_back(event);
        }
    }
}

void SoundReadFrom_SCI1(ResourceEntity &resource, sci::istream &stream)
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
                    uint8_t polyAndPrio;
                    channelStream >> polyAndPrio;
                    channelInfo.Poly = ((polyAndPrio & 0xf) != 0);
                    channelInfo.Priority = polyAndPrio >> 4;
                    dataOffset += 2;
                    dataSize -= 2;

                    if (channelInfo.Number == 0xfe)
                    {
                        channelInfo.Flags = 0;
                        // Digital chanenl 
                        int x = 0;
                    }
                    else
                    {
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

void ConvertSCI0ToNewFormat(const vector<SoundEvent> &events, SoundComponent &sound, uint16_t *channels)
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
    for (int channelNumber = 0; channelNumber < ChannelCount; channelNumber++)
    {
        uint16_t mask = channels[channelNumber];
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

    // Roland always has channel 9
    tracksToUsedChannelNumbers[(uint8_t)DeviceType::RolandMT32].insert(9);

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
            if ((event.GetChannel() == channelNumber) || (event.GetChannel() == 15))
            {
                SoundEvent newEvent = event;
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
}

void SoundReadFrom_SCI0(ResourceEntity &resource, sci::istream &stream)
{
    SoundComponent &sound = resource.GetComponent<SoundComponent>();

    uint8_t b;
    stream >> b; // Digital sample flag - we don't care about this.
    // Apprently if it's 2, we need to add a digital sample data channel.

    uint16_t channels[ChannelCount];
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

ResourceTraits soundResTraits =
{
    ResourceType::Sound,
    &SoundReadFrom_SCI0,
    &SoundWriteTo,
    &NoValidationFunc
};

// REVIEW: We don't even know how to read SCI1 sounds yet.
ResourceTraits soundResTraitsSCI1 =
{
    ResourceType::Sound,
    &SoundReadFrom_SCI1,
    &SoundWriteTo_SCI1,
    &NoValidationFunc
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