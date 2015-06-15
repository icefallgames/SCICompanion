#include "stdafx.h"
#include "SoundOperations.h"
#include "format.h"

std::string GetSoundLength(const SoundComponent &sound)
{
    DWORD ticks = sound.GetTotalTicks();
    uint16_t tempo = max(1, sound.GetTempo());
    int seconds = (int)round((double)(ticks * 30) / (double)(60 * SCI_PPQN));
    return fmt::format("{0:02}:{1:02}", seconds / 60, seconds % 60);
}

// TODO: Ideally we want to be able to load multiple midi files into one resource (to assign to different devices). However,
// there needs to be more work done with the timing/tempo situation. For now, we'll just completely replace a track.
SoundChangeHint InitializeFromMidi(SCIVersion version, std::vector<DeviceType> devices, SoundComponent &sound, const std::string &filename)
{
    uint16_t channels[SCI0ChannelCount];
    FillMemory(channels, sizeof(channels), 0);

    try
    {
        std::ifstream midiFile;
        midiFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        midiFile.open(filename.c_str(), std::ios::in | std::ios::binary);
        if (midiFile.is_open())
        {
            DWORD dw = _ReadBEDWORD(midiFile);
            if (dw == 0x4D546864)
            {
                dw = _ReadBEDWORD(midiFile);
                if (dw == 6)
                {
                    uint16_t wFormat = _ReadBEWORD(midiFile);
                    if ((wFormat == 0) || (wFormat == 1))
                    {
                        // Clear EVERYTHING out
                        sound.Reset();

                        uint16_t wNumTracks = _ReadBEWORD(midiFile);;
                        sound._wDivision = _ReadBEWORD(midiFile);
                        assert((sound._wDivision & 0x8000) == 0); // TODO: support SMPTE

                        // Now we expect tracks.  Each track will consist of an event vector.
                        std::vector<std::vector<SoundEvent> > tracks;
                        // Put in wNumTracks empty tracks:
                        std::vector<SoundEvent> emptyTrack;
                        tracks.insert(tracks.begin(), wNumTracks, emptyTrack);
                        // Read each track
                        std::vector<TempoEntry> tempoChanges;
                        uint16_t wChannelMask = 0;
                        DWORD maxTotalTicks = 0;
                        for (uint16_t i = 0; i < wNumTracks && midiFile.good(); i++)
                        {
                            DWORD totalTrackTicks;
                            wChannelMask |= sound._ReadMidiFileTrack(i, midiFile, tracks[i], tempoChanges, totalTrackTicks);
                            maxTotalTicks = max(maxTotalTicks, totalTrackTicks);
                        }

                        sound.TotalTicks = maxTotalTicks;

                        uint16_t channelsSCI0[SCI0ChannelCount];
                        for (int i = 0; i < ARRAYSIZE(channelsSCI0); i++)
                        {
                            // Which channels are used? Need to pass this to ConvertSCI0ToNewFormat
                            if (wChannelMask & (0x1 << i))
                            {
                                channelsSCI0[i] = 0xff00;
                            }
                            else
                            {
                                channelsSCI0[i] = 0;
                            }
                        }

                        std::vector<SoundEvent> combined;
                        CombineSoundEvents(tracks, combined);                   // Combine
                        uint16_t tempo;
                        DWORD totalTicks = sound.GetTotalTicks();
                        RemoveTempoChanges(combined, tempoChanges, tempo, totalTicks);
                        sound._wTempoIfChanged = tempo;
                        sound.TotalTicks = totalTicks;
                        ConvertSCI0ToNewFormat(combined, sound, (version.SoundFormat == SoundFormat::SCI0) ? channelsSCI0 : nullptr);      // Uncombine

                        for (auto &channelInfo : sound.GetChannelInfos())
                        {
                            if (channelInfo.Number != 15)
                            {
                                EnsureChannelPreamble(channelInfo);
                            }
                            for (auto &device : devices)
                            {
                                sound.SetChannelId(device, channelInfo.Id, true);
                            }
                        }
                    }
                    else
                    {
                        AfxMessageBox("Only type 0 and type 1 midi files are supported.", MB_ERRORFLAGS);
                    }
                }
                else
                {
                    AfxMessageBox("Wrong chunk size", MB_ERRORFLAGS);
                }
            }
            else
            {
                AfxMessageBox("Not a midi file", MB_ERRORFLAGS);
            }
        }
        sound._fCanSetTempo = true;
    }
    catch (std::ifstream::failure e)
    {
        std::string message = fmt::format("Error opening {0}: {1}", filename, e.what());
        AfxMessageBox(message.c_str(), MB_ICONWARNING | MB_OK);
    }

    return SoundChangeHint::Changed;
}
