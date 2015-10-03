#pragma once

#include "ResourceSources.h"

struct AudioMapComponent;

// ResourceSource for SCI1.1 audio resources
class AudioResourceSource : public ResourceSource
{
public:
    AudioResourceSource(SCIVersion version, const std::string &gameFolder, int mapContext, ResourceSourceAccessFlags access) :
        _gameFolder(gameFolder),
        _version(version),
        _mapContext(mapContext),
        _access(access)
    {
        _EnsureAudioMaps();
    }
    ~AudioResourceSource();

    AudioResourceSource& operator=(AudioResourceSource &src) = delete;

    bool ReadNextEntry(ResourceTypeFlags typeFlags, IteratorState &state, ResourceMapEntryAgnostic &entry, std::vector<uint8_t> *optionalRawData = nullptr) override;
    sci::istream GetHeaderAndPositionedStream(const ResourceMapEntryAgnostic &mapEntry, ResourceHeaderAgnostic &headerEntry) override;

    sci::istream GetPositionedStreamAndResourceSizeIncludingHeader(const ResourceMapEntryAgnostic &mapEntry, uint32_t &size) override
    {
        assert(false && "Not implemented");
        return sci::istream(nullptr, 0);
    }

    void RemoveEntry(const ResourceMapEntryAgnostic &mapEntry) override;
    AppendBehavior AppendResources(const std::vector<ResourceBlob> &entries) override;
    void RebuildResources() override {}

private:
    void _EnsureAudioMaps();
    AudioVolumeName _GetVolumeToUse(uint32_t base36Number);
    void _Finalize(AudioMapComponent &newAudioMap, sci::ostream &newVolumeStream, uint32_t base36Number);
    void _CopyWithoutThese(const AudioMapComponent &audioMap, AudioMapComponent &newAudioMap, sci::istream &oldReader, sci::ostream &newVolumeStream, const std::set<uint16_t> &removeThese);
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
};

