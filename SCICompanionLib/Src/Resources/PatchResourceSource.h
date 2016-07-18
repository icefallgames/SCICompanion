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
class ResourceBlob;

// ResourceSource for isolated patch files
class PatchFilesResourceSource : public ResourceSource
{
public:
    PatchFilesResourceSource(SCIVersion version, const std::string &gameFolder, ResourceSourceFlags sourceFlags) :
        _gameFolder(gameFolder),
        _gameFolderSpec(gameFolder + "\\*.*"),
        _hFind(INVALID_HANDLE_VALUE),
        _version(version),
        _stillMore(true),
        _sourceFlags(sourceFlags)
    {}
    ~PatchFilesResourceSource();

    bool ReadNextEntry(ResourceTypeFlags typeFlags, IteratorState &state, ResourceMapEntryAgnostic &entry, std::vector<uint8_t> *optionalRawData = nullptr) override;
    sci::istream GetHeaderAndPositionedStream(const ResourceMapEntryAgnostic &mapEntry, ResourceHeaderAgnostic &headerEntry) override;
    sci::istream GetPositionedStreamAndResourceSizeIncludingHeader(const ResourceMapEntryAgnostic &mapEntry, uint32_t &size, bool &includesHeader) override;

    void RemoveEntry(const ResourceMapEntryAgnostic &mapEntry) override;
    AppendBehavior AppendResources(const std::vector<const ResourceBlob*> &blobs) override;
    void RebuildResources(bool force, ResourceSource &source, std::map<ResourceType, RebuildStats> &stats) override {} // Nothing to do here.

private:
    HANDLE _hFind;
    bool _stillMore;
    std::string _gameFolder;
    std::string _gameFolderSpec;
    WIN32_FIND_DATA _findData;
    SCIVersion _version;
    ResourceSourceFlags _sourceFlags;

    // Muck
    int _nextIndex;
    std::unordered_map<int, std::string> _indexToFilename;
    std::unordered_map<int, std::unique_ptr<sci::streamOwner>> _streamHolder;
};
