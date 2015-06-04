#include "stdafx.h"
#include "SoundOperations.h"
#include "format.h"

SoundChangeHint InitializeFromMidi(SoundComponent &sound, const std::string &filename)
{
    uint16_t channels[ChannelCount];
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

                        uint16_t channels[ChannelCount];
                        for (int i = 0; i < ARRAYSIZE(channels); i++)
                        {
                            // Which channels are used? Need to pass this to ConvertSCI0ToNewFormat
                            if (wChannelMask & (0x1 << i))
                            {
                                channels[i] = 0xff00;
                            }
                            else
                            {
                                channels[i] = 0;
                            }
                        }

                        std::vector<SoundEvent> combined;
                        CombineSoundEvents(tracks, combined);                   // Combine
                        uint16_t tempo;
                        DWORD totalTicks = sound.GetTotalTicks();
                        RemoveTempoChanges(combined, tempoChanges, tempo, totalTicks);
                        sound._wTempoIfChanged = tempo;
                        sound.TotalTicks = totalTicks;
                        ConvertSCI0ToNewFormat(combined, sound, channels);      // Uncombine

                        // By default, turn off all channels for all synths except GM
                        // GM is the "new standard" for fan-made games
                        sound.SetChannelMask(DeviceType::NewGM, wChannelMask);
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
