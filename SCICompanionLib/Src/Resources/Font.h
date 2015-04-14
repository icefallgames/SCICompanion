#pragma once
#include "Components.h"

const int LetterCount = 128;
const int CharTableOffset = 6;

struct FontComponent : ResourceComponent
{
    FontComponent() { LineHeight = 8; } // Just something, so it's not garbage.
    FontComponent(const FontComponent &src) = default;
    ResourceComponent *Clone() override
    {
        return new FontComponent(*this);
    }

    uint16_t LineHeight;
};

ResourceEntity *CreateFontResource(SCIVersion version);
ResourceEntity *CreateDefaultFontResource(SCIVersion version);
