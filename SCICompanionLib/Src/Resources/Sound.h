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

#include "Components.h"

class ResourceEntity;
struct AudioComponent;
enum class BlobKey;

namespace sci
{
    class istream;
}

// When played at 120bpm (our default), the time division for SCI games is 30.
// (60 ticks per second, and a quarter note lasts half a second for 120 bpm)
#define SCI_PPQN 30

enum class DeviceType
{
    // SCI0 - these can be OR'd together
    RolandMT32 = 0x01,
    YamahaFB01 = 0x02,
    Adlib = 0x04,
    Casio = 0x08,
    Tandy1000 = 0x10,
    PCSpeaker = 0x20,
    AmigaMac = 0x40,
    NewGM = 0x80,

    // SCI1 - these cannot.
    SCI1_Adlib = 0x00,
    SCI1_GM = 0x07,
    SCI1_GameBlaster = 0x09,
    SCI1_RolandGM = 0x0c,
    SCI1_PCSpeaker = 0x12,
    SCI1_Tandy = 0x13,
    SCI1_AmigaMac = 0x06,
    SCI1_Unknown08 = 0x08,
    SCI1_Unkonwn0b = 0x0b,

    // Not really a device
    Digital = 0xff
};

DEFINE_ENUM_FLAGS(DeviceType, uint8_t)

//
// A single event in an SCI sound resource
//
class SoundEvent
{
public:
    SoundEvent() : wTimeDelta(0), bParam1(0), bParam2(0), _bStatus(0){}
    SoundEvent(DWORD timeDelta, uint8_t param1, uint8_t param2, uint8_t status) :
        wTimeDelta(timeDelta), bParam1(param1), bParam2(param2), _bStatus(status)
    {}

    enum Command
    {
        NoteOff = 0x80,
        NoteOn = 0x90,
        KeyPressure = 0xA0,
        Control = 0xB0,
        ProgramChange = 0xC0,
        Pressure = 0xD0,
        PitchWheel = 0xE0,
        Special = 0xF0,
    };

    uint8_t GetChannel() const  // -> NOTE: we should remove all sound events that specify channel 15!!!! It is used by the interpreter for looping and cuing.
    {
        if (GetCommand() != Special)
        {
            return (_bStatus & 0x0F);
        }
        else
        {
            return 0x0F; // Special applies to all channels
        }
    }
    Command GetCommand()  const { return (Command)(0xF0 & _bStatus); }
    uint8_t GetRawStatus() const { return _bStatus; }
    void SetRawStatus(uint8_t bStatus) { _bStatus = bStatus; }
    bool IsEffectiveNoteOff() const
    {
        return ((GetCommand() == NoteOff) || ((GetCommand() == NoteOn) && (bParam2 == 0)));
    }

    bool operator==(const SoundEvent &other);
    bool operator!=(const SoundEvent &other);

    DWORD wTimeDelta;
    uint8_t bParam1;
    uint8_t bParam2;

private:
    uint8_t _bStatus;
};

enum class SoundChangeHint
{
    None = 0,
    // Sound is ready.
    Changed = 0x00000100,
    // Sound synth changed
    DeviceChanged = 0x00000200,
    // The list of cues changed
    CueChanged = 0x00000400,
    // Chosen cue changed
    ActiveCueChanged = 0x00000800,
    SelectedChannelChanged = 0x00001000,
};

DEFINE_ENUM_FLAGS(SoundChangeHint, uint32_t)

class CuePoint
{
public:
    enum Type
    {
        Cumulative,
        NonCumulative,
    };
    CuePoint(Type cumulative, DWORD dwTickPos, uint8_t value) : _cumulative(cumulative),
        _dwTickPos(dwTickPos), _value(value) {}
    DWORD GetTickPos() const { return _dwTickPos; }
    uint8_t GetValue() const { return _value; }
    Type GetType() const { return _cumulative; }
    void SetTickPos(DWORD dwTickPos) { _dwTickPos = dwTickPos; }
    void SetValue(uint8_t value);
    void SetType(Type type);
private:
    DWORD _dwTickPos;
    uint8_t _value;
    Type _cumulative;
};

struct ChannelInfo
{
    int Id; // Simply the index in its containing vector
    bool Poly;
    uint8_t Priority;
    uint8_t Number;
    uint8_t Flags;
    std::vector<SoundEvent> Events;
};

struct TrackInfo
{
    TrackInfo() : HasDigital(false), Type(0) {}

    uint8_t Type;                   // Device type
    std::vector<int> ChannelIds;
    bool HasDigital;
};


struct SoundTraits
{
    bool CanEditChannelMask;
};

const uint8_t SCI0ChannelCount = 15;

struct TempoEntry
{
    DWORD dwTicks;
    DWORD dwTempo;

    bool operator<(const TempoEntry &test) const
    {
        return dwTempo < test.dwTempo;
    }
};

static const uint16_t StandardTempo = 120;

struct SoundComponent : public ResourceComponent
{
public:
    SoundComponent *Clone() const override
    {
        return new SoundComponent(*this);
    }
    void Reset();

    friend void SoundWriteTo(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag);
    friend void SoundWriteToWorker(const SoundComponent &sound, sci::ostream &byteStream);
    friend void SoundWriteToWorker_SCI1(const SoundComponent &sound, const AudioComponent *audio, sci::ostream &byteStream);
    friend void SoundReadFrom_SCI1(ResourceEntity &resource, sci::istream &stream, const std::map<BlobKey, uint32_t> &propertyBag);
    friend void SoundReadFrom_SCI0(ResourceEntity &resource, sci::istream &stream, const std::map<BlobKey, uint32_t> &propertyBag);
    friend SoundChangeHint InitializeFromMidi(SCIVersion version, std::vector<DeviceType> devices, SoundComponent &sound, const std::string &filename);
    friend void ReadChannel(sci::istream &stream, std::vector<SoundEvent> &events, DWORD &totalTicks, SoundComponent &sound, int *mustBeChannel = nullptr);
    friend void ScanAndReadDigitalSample(ResourceEntity &resource, sci::istream stream);
    friend void ConvertSCI0ToNewFormat(const std::vector<SoundEvent> &events, SoundComponent &sound, uint16_t *channels);

    // For loop points
    static const DWORD LoopPointNone = ((DWORD)-1);

    SoundComponent(const SoundTraits &traits);

    static std::string GetDeviceName(DeviceType device);

    SoundChangeHint AddCuePoint(CuePoint cue);
    SoundChangeHint DeleteCue(size_t index);
    SoundChangeHint SetCue(size_t index, CuePoint cue);
    SoundChangeHint SetLoopPoint(DWORD dwTicks);
    uint16_t CalculateChannelMask(DeviceType device) const;
    uint16_t GetTimeDivision() const { return _wDivision; }
    bool CanChangeTempo() const { return _fCanSetTempo; }

    SoundChangeHint SetTempo(uint16_t wTempo) { _wTempoIfChanged = wTempo; return SoundChangeHint::CueChanged; }
    uint16_t GetTempo() const { return _wTempoIfChanged; }

    const std::vector<CuePoint> &GetCuePoints() const { return Cues; }
    DWORD GetLoopPoint() const { return LoopPoint; }
    DWORD GetTotalTicks() const { return max(1, TotalTicks); }   // 0 causes div by zero errors }

    // New sound model
    SoundChangeHint SetChannelId(DeviceType type, int channelId, bool on);
    SoundChangeHint ToggleChannelId(DeviceType type, int channelId);
    std::vector<ChannelInfo> &GetChannelInfos() { return _allChannels; }
    const std::vector<ChannelInfo> &GetChannelInfos() const  { return _allChannels; }
    const std::vector<TrackInfo> &GetTrackInfos() const  { return _tracks; }
    std::vector<TrackInfo> &GetTrackInfos() { return _tracks; }
    const TrackInfo *GetTrackInfo(DeviceType device) const;
    const ChannelInfo *GetChannelInfo(DeviceType device, int channelNumber) const;
    bool DoesDeviceHaveTracks(DeviceType device) const;
    bool DoesDeviceChannelIdOn(DeviceType device, int channelId) const;
    const SoundTraits &Traits;

private:
    SoundChangeHint _EnsureChannel15();

    uint16_t _wDivision;
    DWORD LoopPoint;       // A single loop-point (tick position)
    DWORD TotalTicks;    // Total length in ticks
    std::vector<CuePoint> Cues;

    uint16_t _ReadMidiFileTrack(size_t nTrack, std::istream &midiFile, std::vector<SoundEvent> &events, std::vector<TempoEntry> &tempoChanges, DWORD &totalTicksOut);
    void _RationalizeCuesAndLoops();
    void _ProcessBeforeSaving();
    void _NormalizeToSCITempo();

    // We don't allow setting the tempo on SCI resources (only MIDI imports)
    // The resolution isn't there.
    bool _fCanSetTempo;
    bool _fReEvaluateCues;      // Do we need to much around with the cues when saving?
    bool _fReEvaluateLoopPoint; // Do we need to muck around with loop point when saving?
    uint16_t _wTempoIfChanged;      // The user can change the tempo at which imported midi files
    // are played.  Since the SCI interpreter always plays at 120bpm,
    // we need to adjust the actual events when we save.


    // Work on a different sound model.
    std::vector<ChannelInfo> _allChannels;
    std::vector<TrackInfo> _tracks;
};

DWORD CombineSoundEvents(const std::vector<std::vector<SoundEvent> > &tracks, std::vector<SoundEvent> &results);
void RemoveTempoChanges(std::vector<SoundEvent> &events, const std::vector<TempoEntry> &tempoChanges, uint16_t &tempoOut, DWORD &ticksOut);
void EnsureChannelPreamble(ChannelInfo &channel);
DWORD _ReadBEDWORD(std::istream &stream);
uint16_t _ReadBEWORD(std::istream &stream);

ResourceEntity *CreateSoundResource(SCIVersion version);
ResourceEntity *CreateDefaultSoundResource(SCIVersion version);

