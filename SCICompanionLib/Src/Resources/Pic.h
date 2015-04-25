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
};

struct PicComponent : ResourceComponent
{
    PicComponent();
    PicComponent(const PicComponent &src) = default;
    PicComponent(const PicTraits &traits) : Traits(traits) {}
    ResourceComponent *Clone() const override
    {
        return new PicComponent(*this);
    }

    int _uniqueId;
    std::vector<PicCommand> commands;

    const PicTraits &Traits;
};

ResourceEntity *CreatePicResource(SCIVersion version);
ResourceEntity *CreateDefaultPicResource(SCIVersion version);