#include "stdafx.h"
#include "Sync.h"
#include "ResourceEntity.h"
#include "AppState.h"

using namespace std;

void SyncWriteTo(const ResourceEntity &resource, sci::ostream &byteStream)
{
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
