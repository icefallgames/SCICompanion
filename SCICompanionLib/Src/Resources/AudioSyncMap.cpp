#include "stdafx.h"
#include "AudioSyncMap.h"
#include "ResourceEntity.h"

using namespace std;


void SequenceMapReadFrom(ResourceEntity &resource, sci::istream &stream)
{
    AudioSyncMapComponent &map = resource.GetComponent<AudioSyncMapComponent>();
    AudioSyncMapEntry entry;
    stream >> entry;
    while (stream.good() && (entry.AudioOffset != 0xffffffff))
    {
        map.Entries.push_back(entry);
        stream >> entry;
    }
}

ResourceTraits sequenceMapTraits =
{
    ResourceType::Map,
    &SequenceMapReadFrom,
    nullptr,
    &NoValidationFunc,
    nullptr
};

ResourceEntity *CreateSequenceMapResource(SCIVersion version)
{
    std::unique_ptr<ResourceEntity> pResource = std::make_unique<ResourceEntity>(sequenceMapTraits);
    pResource->AddComponent(move(make_unique<AudioSyncMapComponent>()));
    return pResource.release();
}
