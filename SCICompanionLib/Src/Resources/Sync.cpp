#include "stdafx.h"
#include "Sync.h"
#include "ResourceEntity.h"
#include "AppState.h"

using namespace std;

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