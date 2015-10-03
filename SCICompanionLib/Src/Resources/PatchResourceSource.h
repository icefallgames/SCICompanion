#pragma once

#include "ResourceSources.h"

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

    sci::istream GetPositionedStreamAndResourceSizeIncludingHeader(const ResourceMapEntryAgnostic &mapEntry, uint32_t &size) override
    {
        assert(false && "Not implemented");
        return sci::istream(nullptr, 0);
    }

    void RemoveEntry(const ResourceMapEntryAgnostic &mapEntry) override;
    AppendBehavior AppendResources(const std::vector<ResourceBlob> &entries) override;
    void RebuildResources() override {} // Nothing to do here.

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
