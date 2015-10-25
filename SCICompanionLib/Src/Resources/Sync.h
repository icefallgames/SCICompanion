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

struct SyncEntry
{
    SyncEntry() : SyncEntry(0, 0) {}
    SyncEntry(uint16_t ticks, uint16_t cel) : Tick(ticks), Cel(cel) {}

    bool operator==(const SyncEntry &other) const;
    bool operator!=(const SyncEntry &other) const;

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

    bool operator==(const SyncComponent &other) const;
    bool operator!=(const SyncComponent &other) const;

    std::vector<SyncEntry> Entries;     // The main data
    std::string RawData;                // Present in some Sierra games in some resources. We read but don't write.
    std::vector<uint8_t> RawDataBinary; // Present in some Sierra games (alternate format)
    // Update the == operators if you add anymore..
};

void SyncWriteTo(const ResourceEntity &resource, sci::ostream &byteStream);
void SyncReadFrom(ResourceEntity &resource, sci::istream &stream);
uint32_t SyncEstimateSize(const SyncComponent &sync);
bool SyncFromFile(SyncComponent &sync, const std::string &filename);
bool SyncToFile(const SyncComponent &sync, const std::string &filename);