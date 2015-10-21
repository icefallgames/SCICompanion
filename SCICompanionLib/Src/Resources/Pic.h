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

ResourceEntity *CreatePicResource(SCIVersion version);
ResourceEntity *CreateDefaultPicResource(SCIVersion version);