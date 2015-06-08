#pragma once
#include "Components.h"

const int SCI0LetterCount = 128;
const int CharTableOffset = 6;

struct FontTraits
{
    FontTraits(const FontTraits &src) = delete;
    FontTraits& operator=(const FontTraits &src) = delete;

    bool SupportsExtendedCharSet;

    uint16_t ValidateCharCount(uint16_t charCount) const;
};


struct FontComponent : ResourceComponent
{
    FontComponent(const FontComponent &src) = default;
    FontComponent(const FontTraits &traits) : Traits(traits), LineHeight(8) {}
    ResourceComponent *Clone() const override
    {
        return new FontComponent(*this);
    }

    uint16_t LineHeight;

    const FontTraits &Traits;
};

ResourceEntity *CreateFontResource(SCIVersion version);
ResourceEntity *CreateDefaultFontResource(SCIVersion version);
