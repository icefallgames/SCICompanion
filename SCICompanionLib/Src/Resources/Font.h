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
