#pragma once

#include "Components.h"

class ResourceEntity;

#pragma pack(push, 1)
struct AudioSyncMapEntry
{
    uint8_t Noun;
    uint8_t Verb;
    uint8_t Condition;
    uint8_t Sequence;
    uint32_t AudioOffset;
    uint16_t SyncOffset;    // Added to AudioOffset. If 0, there is no sync.
};
#pragma pack(pop)

struct AudioSyncMapComponent : public ResourceComponent
{
public:
    AudioSyncMapComponent() {}
    ~AudioSyncMapComponent() {}

    ResourceComponent *Clone() const override
    {
        return new AudioSyncMapComponent(*this);
    }

    std::vector<AudioSyncMapEntry> Entries;
};

ResourceEntity *CreateSequenceMapResource(SCIVersion version);
