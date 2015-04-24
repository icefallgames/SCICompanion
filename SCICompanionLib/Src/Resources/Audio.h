#pragma once

#include "Components.h"

enum class AudioFlags : uint8_t
{
    None = 0x00,
    DPCM = 0x01,        // Not supported
    Stereo = 0x04,      // Not supported
    SixteenBit = 0x10,  // Not supported
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
    uint32_t sizeExcludingHeader;
};
#include <poppack.h>

class ResourceEntity;

struct AudioComponent : public ResourceComponent
{
public:
    AudioComponent() : Frequency(0), Length(0), Flags(AudioFlags::None) {}

    AudioComponent *Clone() override
    {
        return new AudioComponent(*this);
    }

    std::vector<uint8_t> DigitalSamplePCM;
    uint16_t Frequency;
    uint16_t Length;
    AudioFlags Flags;
};

ResourceEntity *CreateAudioResource(SCIVersion version);
ResourceEntity *CreateDefaultAudioResource(SCIVersion version);
