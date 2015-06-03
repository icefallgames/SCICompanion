#pragma once

#include "Components.h"

class ResourceEntity;

namespace sci
{
    class istream;
}

// When played at 120bpm (our default), the time division for SCI games is 30.
// (60 ticks per second, and a quarter note lasts half a second for 120 bpm)
#define SCI_PPQN 30

enum class DeviceType
{
    RolandMT32 = 0x01,
    YamahaFB01 = 0x02,
    Adlib = 0x04,
    Casio = 0x08,
    Tandy1000 = 0x10,
    PCSpeaker = 0x20,
    DevUnknown = 0x40,
    NewGM = 0x80,
};

DEFINE_ENUM_FLAGS(DeviceType, uint8_t)

//
// A single event in an SCI sound resource
//
class SoundEvent
{
public:

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
    uint8_t Type;
    std::vector<int> ChannelIds;
};

struct SoundComponent : public ResourceComponent
{
public:
    SoundComponent *Clone() const override
    {
        return new SoundComponent(*this);
    }

    friend void SoundWriteTo(const ResourceEntity &resource, sci::ostream &byteStream);
    friend void SoundWriteToWorker(const SoundComponent &sound, sci::ostream &byteStream);
    friend void SoundReadFrom(ResourceEntity &resource, sci::istream &byteStream);
    friend void SoundReadFrom_SCI1(ResourceEntity &resource, sci::istream &stream);
    friend void SoundReadFrom_SCI1OLD(ResourceEntity &resource, sci::istream &stream);
    friend void SoundReadFrom_SCI0(ResourceEntity &resource, sci::istream &stream);
    friend SoundChangeHint InitializeFromMidi(SoundComponent &sound, const std::string &filename);
    friend void ReadChannel(sci::istream &stream, std::vector<SoundEvent> &events, DWORD &totalTicks, SoundComponent &sound, int *mustBeChannel = nullptr);
    friend void ScanAndReadDigitalSample(ResourceEntity &resource, sci::istream stream);
    friend void ConvertSCI0ToNewFormat(SoundComponent &sound);

    // For SetChannelMask
    static const uint16_t AllChannelsMask = 0x7FFF;

    // For loop points
    static const DWORD LoopPointNone = ((DWORD)-1);

    static const uint16_t StandardTempo = 120;

    SoundComponent();

    static std::string GetDeviceName(DeviceType device);

    SoundChangeHint AddCuePoint(CuePoint cue);
    SoundChangeHint DeleteCue(size_t index);
    SoundChangeHint SetCue(size_t index, CuePoint cue);
    SoundChangeHint SetLoopPoint(DWORD dwTicks);
    uint16_t GetChannelMask(DeviceType device) const;
    SoundChangeHint SetChannelMask(DeviceType device, uint16_t wChannels);
    uint16_t GetTimeDivision() const { return _wDivision; }
    bool CanChangeTempo() const { return _fCanSetTempo; }

    SoundChangeHint SetTempo(uint16_t wTempo) { _wTempoIfChanged = wTempo; return SoundChangeHint::CueChanged; }
    uint16_t GetTempo() const { return _wTempoIfChanged; }

    const std::vector<SoundEvent> &GetEvents() const { return Events; }
    std::vector<SoundEvent> &GetEvents() { return Events; }
    const std::vector<CuePoint> &GetCuePoints() const { return Cues; }
    DWORD GetLoopPoint() const { return LoopPoint; }
    DWORD GetTotalTicks() const { return max(1, TotalTicks); }   // 0 causes div by zero errors }


    // New sound model
    std::vector<ChannelInfo> &GetChannelInfos() { return _allChannels; }
    std::vector<TrackInfo> &GetTrackInfos() { return _tracks; }
    const std::vector<ChannelInfo> &GetChannelInfos() const  { return _allChannels; }
    const std::vector<TrackInfo> &GetTrackInfos() const  { return _tracks; }

private:
    uint16_t _channels[15];
    uint16_t _wDivision;
    DWORD LoopPoint;       // A single loop-point (tick position)
    DWORD TotalTicks;    // Total length in ticks
    std::vector<SoundEvent> Events;
    std::vector<CuePoint> Cues;

    struct TempoEntry
    {
        DWORD dwTicks;
        DWORD dwTempo;

        bool operator<(const TempoEntry &test) const
        {
            return dwTempo < test.dwTempo;
        }
    };

    void _CombineTracks(const std::vector<std::vector<SoundEvent> > &tracks);
    uint16_t _ReadMidiFileTrack(size_t nTrack, std::istream &midiFile, std::vector<SoundEvent> &events, std::vector<TempoEntry> &tempoChanges);
    void _RationalizeCuesAndLoops();
    void _NormalizeToSCITempo();
    void _RemoveTempoChanges(const std::vector<TempoEntry> &tempoChanges);

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

DWORD _ReadBEDWORD(std::istream &stream);
uint16_t _ReadBEWORD(std::istream &stream);

ResourceEntity *CreateSoundResource(SCIVersion version);
ResourceEntity *CreateDefaultSoundResource(SCIVersion version);

