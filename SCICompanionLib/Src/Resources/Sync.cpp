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
#include "stdafx.h"
#include "Sync.h"
#include "ResourceEntity.h"
#include "AppState.h"
#include "format.h"

using namespace std;

bool SyncFromFile(SyncComponent &sync, const std::string &filename)
{
    sync.Entries.clear();
    sync.RawData.clear();
    sync.RawDataBinary.clear();
    std::ifstream file(filename, std::ios_base::in);
    bool success = file.is_open();
    if (success)
    {
        bool isTick = true;
        std::string token;
        while (file >> token)
        {
            uint16_t value = (uint16_t)string_to_int(token);
            if (isTick)
            {
                sync.Entries.emplace_back(value, 0);
            }
            else
            {
                sync.Entries.back().Cel = value;
            }
            isTick = !isTick;
        }
    }
    return success;
}
bool SyncToFile(const SyncComponent &sync, const std::string &filename)
{
    std::ofstream file(filename, std::ios_base::out);
    bool success = file.is_open();
    if (success)
    {
        for (auto &entry : sync.Entries)
        {
            file << fmt::format("{0}\t{1}\n", entry.Tick, entry.Cel);
        }
    }
    return success;
}
uint32_t SyncEstimateSize(const SyncComponent &sync)
{
    return 2 + 2 + 4 * sync.Entries.size();
}

void SyncWriteTo(const ResourceEntity &resource, sci::ostream &byteStream)
{
    uint16_t resourceMarker = 0x008e;
    byteStream << resourceMarker;
    const SyncComponent &sync = resource.GetComponent<SyncComponent>();

    for (auto &entry : sync.Entries)
    {
        byteStream << entry.Tick;
        byteStream << entry.Cel;
    }
    uint16_t endMarker = 0xffff;
    byteStream << endMarker;

    // TODO: There is extra data after this, enclosed in {{}}. Raw lip-sync data?
}

void SyncReadFrom(ResourceEntity &resource, sci::istream &stream)
{
    SyncComponent &sync = resource.GetComponent<SyncComponent>();
    SyncEntry entry;
    do
    {
        stream >> entry.Tick;
        if (entry.Tick != 0xffff)
        {
            stream >> entry.Cel;
            sync.Entries.push_back(entry);
        }
    } while (entry.Tick != 0xffff);

    // TODO: There is extra data after this, enclosed in {{}}. Raw lip-sync data?
}

uint16_t SyncComponent::GetMaxTicks() const
{
    uint16_t maxTicks = 0;
    if (!Entries.empty())
    {
        maxTicks = Entries.back().Tick;
    }
    return maxTicks;
}

uint16_t SyncComponent::GetCelAtTick(uint16_t tickPosition) const
{
    uint16_t lastCel = Entries.empty() ? 0 : Entries[0].Cel;
    for (const auto &entry : Entries)
    {
        if (tickPosition < entry.Tick)
        {
            break; // last cel is what we want.
        }
        lastCel = entry.Cel;
    }
    return lastCel;
}

bool SyncComponent::operator == (const SyncComponent &other) const
{
    return Entries == other.Entries &&
        RawData == other.RawData &&
        RawDataBinary == other.RawDataBinary;
}
bool SyncComponent::operator!=(const SyncComponent &other) const
{
    return !(*this == other);
}

bool SyncEntry::operator == (const SyncEntry &other) const
{
    return Tick == other.Tick &&
        Cel == other.Cel;
}
bool SyncEntry::operator!=(const SyncEntry &other) const
{
    return !(*this == other);
}