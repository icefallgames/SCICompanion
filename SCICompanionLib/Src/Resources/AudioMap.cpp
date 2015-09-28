#include "stdafx.h"
#include "AudioMap.h"
#include "ResourceEntity.h"

using namespace std;

void AudioMapWriteTo(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag)
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
    else if (map.Version == AudioMapVersion::SyncMapEarly)
    {
        for (const AudioMapEntry &entry : map.Entries)
        {
            byteStream << entry.Noun;
            byteStream << entry.Verb;
            byteStream << entry.Condition;
            byteStream << entry.Sequence;
            byteStream << entry.Offset;
            byteStream << entry.SyncSize;
        }
        byteStream.WriteBytes(ff, 10);
    }
    else
    {
        throw std::exception("unimplemented");
    }
}

AudioMapVersion _DetermineAudioMapVersion(int resourceNumber, int mainAudioMapResourceNumber, sci::istream &stream)
{
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

    if (mainAudioMapResourceNumber == resourceNumber)
    {
        if (resourceNumber == 65535)
        {
            if (ffCount >= 6)
            {
                return AudioMapVersion::SixBytes;
            }
            else if (ffCount == 5)
            {
                return AudioMapVersion::FiveBytes;
            }
        }
        else if (resourceNumber == 0)
        {
            // LB2 (non-CD), and apparently Mother Goose SCI 1.1 remake.
            if (ffCount >= 8)
            {
                return AudioMapVersion::EightBytes;
            }
        }
    }
    else
    {
        // This is a map with verb/noun/cond/seq sync information.
        // It maps a message tuple to an audio (speech) resource, along with possible sync info.
        if (ffCount >= 11)
        {
            return AudioMapVersion::SyncMapLate;
        }
        else if (ffCount >= 10)
        {
            return AudioMapVersion::SyncMapEarly;
        }
    }

    throw new std::exception("Unknown audio map format");
}

void AudioMapReadFrom(ResourceEntity &resource, sci::istream &stream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    AudioMapComponent &map = resource.GetComponent<AudioMapComponent>();

    map.Version = _DetermineAudioMapVersion(resource.ResourceNumber, map.Traits.MainAudioMapResourceNumber, stream);

    uint32_t cumOffset = 0;
    if (map.Version == AudioMapVersion::SyncMapLate)
    {
        stream >> cumOffset;
    }

    while (stream.getBytesRemaining() > (uint32_t)map.Version)
    {
        AudioMapEntry entry;
        switch (map.Version)
        {
            case AudioMapVersion::SixBytes:
            {
                stream >> entry.Number;
                stream >> entry.Offset;
            }
                break;
            case AudioMapVersion::FiveBytes:
            {
                stream >> entry.Number;
                // 24 bit value that is a cumulative offset
                uint16_t low;
                uint8_t high;
                stream >> low;
                stream >> high;
                uint32_t add = low + (high << 16);
                cumOffset += add;
                entry.Offset = cumOffset;
            }
                break;

            case AudioMapVersion::EightBytes:
            {
                stream >> entry.Number;
                stream.skip(2); // This should be 0xffff
                stream >> entry.Offset;
            }
                break;

            case AudioMapVersion::SyncMapEarly:
            {
                stream >> entry.Noun;
                stream >> entry.Verb;
                stream >> entry.Condition;
                stream >> entry.Sequence;
                stream >> entry.Offset;
                stream >> entry.SyncSize;
                entry.Sequence &= 0x3f; // Top two bits not used
                entry.Number = resource.ResourceNumber;
            }
                break;

            case AudioMapVersion::SyncMapLate:
            {
                stream >> entry.Noun;
                stream >> entry.Verb;
                stream >> entry.Condition;
                stream >> entry.Sequence;
                // 3 bytes
                uint8_t bytes[3];
                stream.read_data(bytes, sizeof(bytes));
                uint32_t offset = bytes[0] + (bytes[1] << 8) + (bytes[2] << 16);
                cumOffset += offset;
                entry.Offset = cumOffset;

                if (entry.Sequence & 0x80)
                {
                    stream >> entry.SyncSize;
                }
                if (entry.Sequence & 0x40)
                {
                    uint16_t rawLipSyncDataSize;
                    stream >> rawLipSyncDataSize;
                    // REVIEW: This is bogus, but good enough for now (until we support raw lip sync data)
                    entry.SyncSize += rawLipSyncDataSize;
                }
                entry.Sequence &= 0x3f; // Top two bits not used, except as above
                entry.Number = resource.ResourceNumber;
            }
                break;

            default:
                assert(false);
                break;
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

bool IsMainAudioMap(AudioMapVersion version)
{
    return version <= AudioMapVersion::EightBytes;
}

AudioMapTraits audioMapTraits0 =
{ 0 };

AudioMapTraits audioMapTraits65535 =
{ 65535 };

AudioMapComponent::AudioMapComponent() : AudioMapComponent(audioMapTraits65535) {}
AudioMapComponent::AudioMapComponent(const AudioMapTraits &traits) : Traits(traits) {}

ResourceEntity *CreateMapResource(SCIVersion version)
{
    AudioMapTraits *mapTraits = &audioMapTraits0;
    if (version.AudioMapResourceNumber == 65535)
    {
        mapTraits = &audioMapTraits65535;
    }
    else if (version.AudioMapResourceNumber == 0)
    {
        mapTraits = &audioMapTraits0;
    }
    else
    {
        throw std::exception("Unknown audio map resource number.");
    }

    std::unique_ptr<ResourceEntity> pResource = std::make_unique<ResourceEntity>(audioMapTraits);
    pResource->AddComponent(move(make_unique<AudioMapComponent>(*mapTraits)));
    return pResource.release();
}
