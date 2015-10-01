#pragma once

#include "Components.h"

struct SyncEntry
{
    SyncEntry() : SyncEntry(0, 0) {}
    SyncEntry(uint16_t ticks, uint16_t cel) : Tick(ticks), Cel(cel) {}

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

    uint16_t GetMaxTicks() const;
    uint16_t GetCelAtTick(uint16_t tickPosition) const;

    std::vector<SyncEntry> Entries;

    std::string RawData;    // Present in some Sierra games in some resources. We read but don't write.
    std::vector<uint8_t> RawDataBinary;
};

void SyncWriteTo(const ResourceEntity &resource, sci::ostream &byteStream);
void SyncReadFrom(ResourceEntity &resource, sci::istream &stream);
