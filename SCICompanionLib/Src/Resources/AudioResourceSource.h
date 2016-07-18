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

#include "ResourceSources.h"
#include "GameFolderHelper.h"

struct AudioMapComponent;
class ResourceBlob;

// ResourceSource for SCI1.1 audio resources. They are a special case, because the "map files" for these
// are in a custom format, and may exist embedded in the main resource package, or as standalone files.
// (we are agnostic to its location)
class AudioResourceSource : public ResourceSource
{
public:
    AudioResourceSource(const GameFolderHelper &helper, int mapContext, ResourceSourceAccessFlags access) :
        _gameFolder(helper.GameFolder),
        _version(helper.Version),
        _mapContext(mapContext),
        _access(access),
        _helper(helper)
    {
        _EnsureAudioMaps();
    }
    ~AudioResourceSource();

    AudioResourceSource& operator=(AudioResourceSource &src) = delete;

    bool ReadNextEntry(ResourceTypeFlags typeFlags, IteratorState &state, ResourceMapEntryAgnostic &entry, std::vector<uint8_t> *optionalRawData = nullptr) override;
    sci::istream GetHeaderAndPositionedStream(const ResourceMapEntryAgnostic &mapEntry, ResourceHeaderAgnostic &headerEntry) override;

    sci::istream GetPositionedStreamAndResourceSizeIncludingHeader(const ResourceMapEntryAgnostic &mapEntry, uint32_t &size, bool &includesHeader) override
    {
        assert(false && "Not implemented");
        return sci::istream(nullptr, 0);
    }

    void RemoveEntry(const ResourceMapEntryAgnostic &mapEntry) override;
    AppendBehavior AppendResources(const std::vector<const ResourceBlob*> &blobs) override;
    void RebuildResources(bool force, ResourceSource &source, std::map<ResourceType, RebuildStats> &stats) override {}

private:
    void _EnsureAudioMaps();
    AudioVolumeName _GetVolumeToUse(uint32_t base36Number);
    std::string _GetAudioVolumePath(bool bak, AudioVolumeName volumeName, ResourceSourceFlags *sourceFlags = nullptr);
    sci::streamOwner *_EnsureReadOnlyAudioVolume(uint32_t base36Number);
    std::unique_ptr<sci::streamOwner> _GetAudioVolume(uint32_t base36Number);

    std::string _gameFolder;
    SCIVersion _version;
    int _mapContext;
    ResourceSourceAccessFlags _access;
    ResourceSourceFlags _sourceFlags;

    std::vector<std::unique_ptr<ResourceEntity>> _audioMaps;

    // Use memory mapped files, because these volumes tend to be large (several hundred MB)
    std::unique_ptr<sci::streamOwner> _volumeStreamOwnerSfx;
    std::unique_ptr<sci::streamOwner> _volumeStreamOwnerAud;

    GameFolderHelper _helper;
};

