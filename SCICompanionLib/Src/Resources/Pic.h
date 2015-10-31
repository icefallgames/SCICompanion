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
#include "PicCommands.h"

struct PicTraits
{
    PicTraits(const PicTraits &src) = delete;
    PicTraits& operator=(const PicTraits &src) = delete;

    bool IsVGA;
    bool AllowMultipleBitmaps;
    bool CanChangePriorityLines;
    bool SixteenBitPri;
    bool SupportsPenCommands;
    bool SupportsControlScreen;
    bool SupportsVectorCommands;
    bool ContinuousPriority;
};

struct PicComponent : ResourceComponent
{
    PicComponent();
    PicComponent(const PicComponent &src) = default;
    PicComponent& operator=(const PicComponent &src) = default;
    PicComponent(const PicTraits *traits);
    ResourceComponent *Clone() const override
    {
        return new PicComponent(*this);
    }

    std::vector<PicCommand> commands;
    size16 Size;

    const PicTraits *Traits;
};

template<typename TPoint>
TPoint GameResolutionToScreenResolution(TPoint point, NativeResolution resolution)
{
    switch (resolution)
    {
        case NativeResolution::Res640x400:
            point.x *= 2;
            point.y *= 2;
            break;
        case NativeResolution::Res640x480:
            point.x *= 2;
            point.y = point.y * 12 / 5;
            break;
    }
    return point;
}

template<typename TPoint>
TPoint ScreenResolutionToGameResolution(TPoint point, NativeResolution resolution)
{
    switch (resolution)
    {
        case NativeResolution::Res640x400:
            point.x /= 2;
            point.y /= 2;
            break;
        case NativeResolution::Res640x480:
            point.x /= 2;
            point.y = point.y * 5 / 12;
            break;
    }
    return point;
}


ResourceEntity *CreatePicResource(SCIVersion version);
ResourceEntity *CreateDefaultPicResource(SCIVersion version);