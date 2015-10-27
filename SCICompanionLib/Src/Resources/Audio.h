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

enum class AudioFlags : uint8_t
{
    // The values here appear to be wrong: http://wiki.multimedia.cx/index.php?title=Sierra_Audio
    // This has the correct values: https://github.com/scummvm/scummvm/blob/master/engines/sci/sound/audio.cpp
    //Stereo = 0x04,      // Not supported
    //SixteenBit = 0x10,  // Not supported
    None = 0x00,
    DPCM = 0x01,
    Unknown = 0x02,
    SixteenBit = 0x04,
    Signed = 0x08,
};
DEFINE_ENUM_FLAGS(AudioFlags, uint8_t)

// http://wiki.multimedia.cx/index.php?title=Sierra_Audio
#include <pshpack1.h>
struct AudioHeader
{
    uint8_t resourceType;
    uint8_t headerSize;
    uint32_t audioType; // SOL
    uint16_t sampleRate;
    AudioFlags flags;
    // REVIEW this is actually 16 bits:
    int32_t sizeExcludingHeader;
};
#include <poppack.h>

class ResourceEntity;

struct AudioComponent : public ResourceComponent
{
public:
    AudioComponent() : Frequency(0), Flags(AudioFlags::None), IsClipped(false) {}

    AudioComponent(const AudioComponent &src) = default;
    AudioComponent& operator=(const AudioComponent &src) = default;

    ResourceComponent *Clone() const override
    {
        return new AudioComponent(*this);
    }

    uint32_t GetLength() const { return (uint32_t)DigitalSamplePCM.size(); }
    uint32_t GetLengthInTicks() const;
    uint32_t GetBytesPerSecond() const;

    void ScanForClipped();

    std::vector<uint8_t> DigitalSamplePCM;
    uint16_t Frequency; // Samples per second
    AudioFlags Flags;
    bool IsClipped;
};

ResourceEntity *CreateAudioResource(SCIVersion version);
ResourceEntity *CreateWaveAudioResource(SCIVersion version);
ResourceEntity *CreateDefaultAudioResource(SCIVersion version);
uint32_t AudioEstimateSize(const ResourceEntity &resource);
std::string GetAudioLength(const AudioComponent &audio);