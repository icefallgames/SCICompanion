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

void SyncWriteTo(const ResourceEntity &resource, sci::ostream &byteStream);
void SyncReadFrom(ResourceEntity &resource, sci::istream &stream);
