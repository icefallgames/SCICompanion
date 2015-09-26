#pragma once

#include "Components.h"

struct SyncEntry
{
    uint16_t Tick;
    uint16_t Cel;
};

struct SyncComponent : public ResourceComponent
{
public:
    ResourceComponent *Clone() const override
    {
        return new SyncComponent(*this);
    }

    std::vector<SyncEntry> Entries;
};

ResourceEntity *CreateSyncResource(SCIVersion version);
ResourceEntity *CreateDefaultSyncResource(SCIVersion version);
