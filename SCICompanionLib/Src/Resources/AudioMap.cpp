#include "stdafx.h"
#include "AudioMap.h"
#include "ResourceEntity.h"

using namespace std;

void AudioMapWriteTo(const ResourceEntity &resource, sci::ostream &byteStream)
{
    uint8_t ff[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    const AudioMapComponent &map = resource.GetComponent<AudioMapComponent>();
    if (map.Version == AudioMapVersion::FiveBytes)
    {
        uint32_t prevOffset = 0;
        for (const AudioMapEntry &entry : map.Entries)
        {
            byteStream << entry.Number;
            uint32_t cumOffset = entry.Offset - prevOffset;
            byteStream.WriteWord((uint16_t)cumOffset);
            byteStream.WriteByte((uint8_t)(cumOffset >> 16));
            prevOffset = entry.Offset;
        }
        byteStream.WriteBytes(ff, 5);
    }
    else if (map.Version == AudioMapVersion::SixBytes)
    {
        for (const AudioMapEntry &entry : map.Entries)
        {
            byteStream << entry.Number;
            byteStream << entry.Offset;
        }
        byteStream.WriteBytes(ff, 6);
    }
    else if (map.Version == AudioMapVersion::EightBytes)
    {
        for (const AudioMapEntry &entry : map.Entries)
        {
            byteStream << entry.Number;
            uint16_t ffff = 0xffff;
            byteStream << ffff;
            byteStream << entry.Offset;
        }
        byteStream.WriteBytes(ff, 8);
    }
}

void AudioMapReadFrom(ResourceEntity &resource, sci::istream &stream)
{
    AudioMapComponent &map = resource.GetComponent<AudioMapComponent>();

    // Detect if it's 5 or 6 byte entries
    int ffCount = 0;
    const uint8_t *data = stream.GetInternalPointer() + stream.GetDataSize() - 1;
    for (size_t i = 0; i < stream.GetDataSize(); i++)
    {
        if (*data == 0xff)
        {
            ffCount++;
            data--;
        }
        else
        {
            break;
        }
    }
    if (resource.ResourceNumber == 65535)
    {
        if (ffCount >= 6)
        {
            map.Version = AudioMapVersion::SixBytes;
        }
        else if (ffCount == 5)
        {
            map.Version = AudioMapVersion::FiveBytes;
        }
        else
        {
            throw new std::exception("Unknown audio map format");
        }
    }
    else if (resource.ResourceNumber == 0)
    {
        if (ffCount >= 8)
        {
            map.Version = AudioMapVersion::EightBytes;
        }
        else
        {
            throw new std::exception("Unknown audio map format");
        }
    }

    AudioMapEntry entry = {};
    while (stream.getBytesRemaining() > (uint32_t)map.Version)
    {
        stream >> entry.Number;
        if (map.Version == AudioMapVersion::SixBytes)
        {
            stream >> entry.Offset;
        }
        else if (map.Version == AudioMapVersion::FiveBytes)
        {
            // 24 bit value that is a cumulative offset
            uint16_t low;
            uint8_t high;
            stream >> low;
            stream >> high;
            uint32_t add = low + (high << 16);
            entry.Offset += add;
        }
        else if (map.Version == AudioMapVersion::EightBytes)
        {
            stream.skip(2); // This should be 0xffff
            stream >> entry.Offset;
        }

        if (entry.Number != 0xffff)
        {
            map.Entries.push_back(entry);
        }
    }
}

ResourceTraits audioMapTraits =
{
    ResourceType::Map,
    &AudioMapReadFrom,
    &AudioMapWriteTo,
    &NoValidationFunc,
    nullptr
};

ResourceEntity *CreateMapResource(SCIVersion version)
{
    std::unique_ptr<ResourceEntity> pResource = std::make_unique<ResourceEntity>(audioMapTraits);
    pResource->AddComponent(move(make_unique<AudioMapComponent>()));
    return pResource.release();
}
