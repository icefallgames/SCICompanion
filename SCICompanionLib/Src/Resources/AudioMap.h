#pragma once

#include "Components.h"

enum class AudioMapVersion
{
    FiveBytes = 5,
    SixBytes = 6,
    EightBytes = 8,
    SyncMapEarly = 10,
    SyncMapLate = 11,
};

bool IsMainAudioMap(AudioMapVersion version);

class ResourceEntity;

struct AudioMapTraits
{
    AudioMapTraits(const AudioMapTraits &src) = delete;
    AudioMapTraits& operator=(const AudioMapTraits &src) = delete;

    int MainAudioMapResourceNumber;

};

struct AudioMapEntry
{
    AudioMapEntry() : Number(0), Offset(0), Noun(0xff), Verb(0xff), Condition(0xff), Sequence(0xff), SyncSize(0) {}

    uint16_t Number;
    uint32_t Offset;

    // For SyncMapEarly we have these extra 6 bytes:
    uint8_t Noun;
    uint8_t Verb;
    uint8_t Condition;
    uint8_t Sequence;
    uint16_t SyncSize;
};

struct AudioMapComponent : public ResourceComponent
{
public:
    AudioMapComponent();
    AudioMapComponent(const AudioMapComponent &src) = default;
    AudioMapComponent(const AudioMapTraits &traits);
    ~AudioMapComponent() {}

    ResourceComponent *Clone() const override
    {
        return new AudioMapComponent(*this);
    }

    std::vector<AudioMapEntry> Entries;
    AudioMapVersion Version;

    const AudioMapTraits &Traits;
};

ResourceEntity *CreateMapResource(SCIVersion version);
