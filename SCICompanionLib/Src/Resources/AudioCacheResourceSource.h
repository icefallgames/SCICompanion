#pragma once

#include "ResourceSources.h"

struct AudioMapComponent;
class ResourceEntity;

extern const char *pszAudioCacheFolder;

// ResourceSource for SCI1.1 audio resources
class AudioCacheResourceSource : public ResourceSource
{
public:
    AudioCacheResourceSource(SCIVersion version, const std::string &gameFolder, int mapContext, ResourceSourceAccessFlags access);
    ~AudioCacheResourceSource();

    AudioCacheResourceSource& operator=(AudioCacheResourceSource &src) = delete;

    bool ReadNextEntry(ResourceTypeFlags typeFlags, IteratorState &state, ResourceMapEntryAgnostic &entry, std::vector<uint8_t> *optionalRawData = nullptr) override;
    sci::istream GetHeaderAndPositionedStream(const ResourceMapEntryAgnostic &mapEntry, ResourceHeaderAgnostic &headerEntry) override;

    sci::istream GetPositionedStreamAndResourceSizeIncludingHeader(const ResourceMapEntryAgnostic &mapEntry, uint32_t &size) override
    {
        assert(false && "Not implemented");
        return sci::istream(nullptr, 0);
    }

    void RemoveEntry(const ResourceMapEntryAgnostic &mapEntry) override;
    AppendBehavior AppendResources(const std::vector<const ResourceBlob*> &blobs) override;
    void RebuildResources(bool force) override;

    // A way to call RemoveEntry directly, for more efficiency.
    void AudioCacheResourceSource::RemoveEntries(int number, const std::vector<uint32_t> tuples);

    void Clear();

private:
    std::unique_ptr<ResourceEntity> _PrepareForAddOrRemove();
    void _EnsureEnumInitialized();

    SCIVersion _version;
    int _mapContext;
    ResourceSourceAccessFlags _access;
    ResourceSourceFlags _sourceFlags;

    // Enumeration
    std::vector<uint32_t> _audioTuplesPresent;          // base 36
    std::unordered_set<uint32_t> _syncTuplesPresent;
    std::unique_ptr<ResourceEntity> _audioMap;
    std::vector<int> _audioFilesPresent;          // regular

    std::unordered_map<uint64_t, std::unique_ptr<sci::streamOwner>> _streamHolders;
    std::unordered_map<uint64_t, std::unique_ptr<sci::ostream>> _streamHolders2;

    bool _enumInitialized;
    std::string _gameFolder;
    std::string _cacheFolder;
    std::string _cacheSubFolderForEnum;
};

