#pragma once

#include "Components.h"

enum class AudioMapVersion
{
    FiveBytes = 5,
    SixBytes = 6
};

class ResourceEntity;

struct AudioMapEntry
{
    uint16_t Number;
    uint32_t Offset;
};

struct AudioMapComponent : public ResourceComponent
{
public:
    AudioMapComponent() {}
    ~AudioMapComponent() {}

    ResourceComponent *Clone() const override
    {
        return new AudioMapComponent(*this);
    }

    std::vector<AudioMapEntry> Entries;
    AudioMapVersion Version;
};

ResourceEntity *CreateMapResource(SCIVersion version);
