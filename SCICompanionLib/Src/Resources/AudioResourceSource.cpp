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
#include "AudioResourceSource.h"
#include "format.h"
#include "ResourceEntity.h"
#include "ResourceContainer.h"
#include "AudioMap.h"
#include "Message.h"
#include "SoundUtil.h"
#include "Audio.h"
#include "ResourceBlob.h"

using namespace std;

void AudioResourceSource::_EnsureAudioMaps()
{
    // Right now we'll only ever get one audio map. However, I'd like to keep the multimap functionality for now...
    if (_mapContext == -1)
    {
        _mapContext = _version.AudioMapResourceNumber;
    }
    auto resourceContainer = _helper.Resources(ResourceTypeFlags::AudioMap, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::AddInDefaultEnumFlags);
    for (auto &blob : *resourceContainer)
    {
        if ((_mapContext == -1) || (blob->GetNumber() == _mapContext))
        {
            _audioMaps.push_back(CreateResourceFromResourceData(*blob));
        }
    }
}

bool AudioResourceSource::ReadNextEntry(ResourceTypeFlags typeFlags, IteratorState &state, ResourceMapEntryAgnostic &entry, std::vector<uint8_t> *optionalRawData)
{

    // Read 6 byte entry (for resource.aud. For resource.sfx it would be 5 bytes)
    // Early SCI1.1 65535.MAP structure (uses RESOURCE.AUD):
    // =========
    // 6-byte entries:
    // w nEntry
    // dw offset

    // Late SCI1.1 65535.MAP structure (uses RESOURCE.SFX):
    // =========
    // 5-byte entries:
    // w nEntry
    // tb offset (cumulative)

    // For LSL6, which uses resource.sfx, there is no 65535.map file. It's in the resource map, in a map resource.
    // So we'll need to do some funky thing where we "recursively" call for that resource. Or, perhaps a better thing,
    // it so have a GetAudioMap65535 thingy, just like we have GetPalette999. And it either grabs it from the patch resource
    // or from the actual resource map. How about that, easy. Then we'll either look for resource.aud or resource.aux to know how
    // we should read it.
    // LB_Dagger has it as 0.map in the resource map
    // QF3 has a 65535.map in resource.map, but I don't see the actual audio files anywhere.

    // To iterate through audio maps, we'll need 2 indices:
    //  - index into audiomap component     (16 bits)
    //  - index into audiomap entry         (16 bits)

    uint32_t audioMapIndex = ((state.mapStreamOffset & 0xffff0000) >> 16);
    uint32_t audioMapEntryIndex = (state.mapStreamOffset & 0x0ffff);
    bool found = false;
    while (!found && (audioMapIndex < _audioMaps.size()))
    {
        const AudioMapComponent &audioMap = _audioMaps[audioMapIndex]->GetComponent<AudioMapComponent>();
        while (!found && (audioMapEntryIndex < audioMap.Entries.size()))
        {
            const AudioMapEntry &mapEntry = audioMap.Entries[audioMapEntryIndex];
            // Use ExtraData to store the SyncSize.
            entry.ExtraData = mapEntry.SyncSize;
            entry.Number = mapEntry.Number;
            entry.Offset = mapEntry.Offset;
            entry.PackageNumber = 0;
            entry.Type = ResourceType::Audio;
            entry.Base36Number = IsMainAudioMap(audioMap.Version) ? NoBase36 : GetMessageTuple(mapEntry);
            audioMapEntryIndex++;
            found = true;
        }
        if (!found)
        {
            audioMapIndex++;
            audioMapEntryIndex = 0;
        }
    }

    uint32_t newStreamOffset = audioMapEntryIndex;
    newStreamOffset |= audioMapIndex << 16;
    state.mapStreamOffset = newStreamOffset;

    return found;
}

std::string AudioResourceSource::_GetAudioVolumePath(bool bak, AudioVolumeName volumeToUse, ResourceSourceFlags *sourceFlags)
{
    return GetAudioVolumePath(_gameFolder, bak, volumeToUse, sourceFlags);
}

AudioVolumeName AudioResourceSource::_GetVolumeToUse(uint32_t base36Number)
{
    return GetVolumeToUse(_version, base36Number);
}

std::unique_ptr<sci::streamOwner> AudioResourceSource::_GetAudioVolume(uint32_t base36Number)
{
    if (IsFlagSet(_access, ResourceSourceAccessFlags::ReadWrite))
    {
        // If we want to eventually write using this same ResourceSource, then use the version of streamOwner that copies the file.
        ScopedFile scoped(_GetAudioVolumePath(false, _GetVolumeToUse(base36Number), &_sourceFlags), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
        return std::make_unique<sci::streamOwner>(scoped.hFile);
    }
    else
    {
        // We can use a memory mapped file for optimum performance.
        return std::make_unique<sci::streamOwner>(_GetAudioVolumePath(false, _GetVolumeToUse(base36Number), &_sourceFlags));
    }
}

sci::streamOwner *AudioResourceSource::_EnsureReadOnlyAudioVolume(uint32_t base36Number)
{
    AudioVolumeName volumeToUse = _GetVolumeToUse(base36Number);

    if ((volumeToUse == AudioVolumeName::Sfx) && !_volumeStreamOwnerSfx)
    {
        _volumeStreamOwnerSfx = _GetAudioVolume(base36Number);
    }
    if ((volumeToUse == AudioVolumeName::Aud) && !_volumeStreamOwnerAud)
    {
        _volumeStreamOwnerAud = _GetAudioVolume(base36Number);
    }
    return (volumeToUse == AudioVolumeName::Sfx) ? _volumeStreamOwnerSfx.get() : _volumeStreamOwnerAud.get();
}

sci::istream AudioResourceSource::GetHeaderAndPositionedStream(const ResourceMapEntryAgnostic &mapEntry, ResourceHeaderAgnostic &headerEntry)
{
    sci::streamOwner *streamOwner = _EnsureReadOnlyAudioVolume(mapEntry.Base36Number);
    if (streamOwner)
    {
        sci::istream reader = streamOwner->getReader();
        reader.seekg(mapEntry.Offset);
        headerEntry.Number = mapEntry.Number;
        headerEntry.Base36Number = mapEntry.Base36Number;
        headerEntry.PackageHint = mapEntry.PackageNumber;
        headerEntry.CompressionMethod = 0;
        headerEntry.SourceFlags = _sourceFlags;
        headerEntry.Version = _version;
        headerEntry.Type = mapEntry.Type;
        headerEntry.cbDecompressed = 0;

        if (mapEntry.ExtraData > 0)
        {
            // Identifier for lipsync data.
            // assert(reader.peek() == 0x8e);
            // Actually, this could be raw lipsync data, like in KQ6 CD, 270, NN:0x12, VV:0x1b, SS:0x02, CC: 0x00
            // We'd need to extend ExtraData to two values in order to support this.
            headerEntry.PropertyBag[BlobKey::LipSyncDataSize] = mapEntry.ExtraData;
        }

        // To know how big we are, we need to read the audio header.
        // This comes after the lipsync data
        sci::istream audioReader = reader;      // Make a copy
        audioReader.skip(mapEntry.ExtraData);   // Lip sync data size

        if (_version.AudioIsWav && (_mapContext == _version.AudioMapResourceNumber))
        {
            uint32_t fileSize = GetWaveFileSizeIncludingHeader(audioReader);
            headerEntry.cbDecompressed = fileSize + mapEntry.ExtraData;
            headerEntry.cbCompressed = headerEntry.cbDecompressed;
        }
        else
        {
            AudioHeader audioHeader;
            audioReader >> audioHeader;
            headerEntry.cbDecompressed = audioHeader.headerSize + 2 + audioHeader.sizeExcludingHeader + mapEntry.ExtraData;
            headerEntry.cbCompressed = headerEntry.cbDecompressed;
        }
        return reader;
    }
    return sci::istream(nullptr, 0);
}

// Appending and removing entries is done through AudioCacheResourceSource, which will then
// repackage files on demand into the audio resource packages.
void AudioResourceSource::RemoveEntry(const ResourceMapEntryAgnostic &mapEntry)
{
    throw std::exception("not implemented");
}

AppendBehavior AudioResourceSource::AppendResources(const std::vector<const ResourceBlob*> &blobs)
{
    throw std::exception("not implemented");
    return AppendBehavior::Append;
}

AudioResourceSource::~AudioResourceSource() {}