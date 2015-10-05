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
    AudioComponent() : Frequency(0), Flags(AudioFlags::None) {}

    ResourceComponent *Clone() const override
    {
        return new AudioComponent(*this);
    }

    uint32_t GetLength() const { return (uint32_t)DigitalSamplePCM.size(); }
    uint32_t GetLengthInTicks() const;
    uint32_t GetBytesPerSecond() const;

    std::vector<uint8_t> DigitalSamplePCM;
    uint16_t Frequency; // Samples per second
    AudioFlags Flags;
};

ResourceEntity *CreateAudioResource(SCIVersion version);
ResourceEntity *CreateDefaultAudioResource(SCIVersion version);
uint32_t AudioEstimateSize(const ResourceEntity &resource);
std::string GetAudioLength(const AudioComponent &audio);