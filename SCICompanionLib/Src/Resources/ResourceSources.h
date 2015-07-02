#pragma once

#include "ResourceBlob.h"

// This file describes various resource sources:
// (1) resource.map/resource.xxx
// (2) message.map/resource.msg
// (3) solitary files (e.g. 017.p56)

enum class AppendBehavior
{
    Append,
    Replace,
};

typedef ResourceHeaderAgnostic(*ReadResourceHeaderFunc)(sci::istream &byteStream, SCIVersion version, ResourceSourceFlags sourceFlags, uint16_t packageHint);
typedef void(*WriteResourceHeaderFunc)(sci::ostream &byteStream, const ResourceHeaderAgnostic &header);

template<typename _VersionHeader>
ResourceHeaderAgnostic ReadResourceHeader(sci::istream &byteStream, SCIVersion version, ResourceSourceFlags sourceFlags, uint16_t packageHint)
{
    _VersionHeader rh;
    byteStream >> rh;
    if (!byteStream.good())
    {
        // TODO: This is a failure, but corrupted data possibly... just continue and log it? Instead of ending
        // Or make an empty resource.
        throw std::exception("corrupted resource!");
    }
    ResourceHeaderAgnostic rhAgnostic = rh.ToAgnostic(version, sourceFlags, packageHint);
    if ((rhAgnostic.cbCompressed == 0) || (rhAgnostic.cbDecompressed == 0))
    {
        throw std::exception("corrupted resource!");
    }
    return rhAgnostic;
}

template<typename _VersionHeader>
void WriteResourceHeader(sci::ostream &byteStream, const ResourceHeaderAgnostic &header)
{
    _VersionHeader rh;
    rh.FromAgnostic(header);
    byteStream << rh;
}

struct ResourceHeaderReadWrite
{
    ReadResourceHeaderFunc reader;
    WriteResourceHeaderFunc writer;
};

template<typename _VersionHeader>
ResourceHeaderReadWrite MakeResourceHeaderReadWriter()
{
    return
    {
        &ReadResourceHeader<_VersionHeader>,
        &WriteResourceHeader<_VersionHeader>,
    };
}

struct IteratorState
{
    IteratorState() : mapIndex(0), lookupTableIndex(0), mapStreamOffset(0) {}

    friend bool operator==(const IteratorState &one, const IteratorState &two);
    friend bool operator!=(const IteratorState &one, const IteratorState &two);

    size_t mapIndex;            // Support for enumerating multiple *.map files in one iteration
    size_t lookupTableIndex;    // In >SCI1 there is a lookuptable at the beginning of resource.map
    uint32_t mapStreamOffset;   // Current offset within resource.map
};

// Main base class for expressing resource sources
class ResourceSource
{
public:
    virtual ~ResourceSource() {}
    void Something(IteratorState &boop) {}

    virtual bool ReadNextEntry(ResourceTypeFlags typeFlags, IteratorState &state, ResourceMapEntryAgnostic &entry, std::vector<uint8_t> *optionalRawData = nullptr) = 0;
    virtual sci::istream GetHeaderAndPositionedStream(const ResourceMapEntryAgnostic &mapEntry, ResourceHeaderAgnostic &headerEntry) = 0;
    virtual sci::istream GetPositionedStreamAndResourceSizeIncludingHeader(const ResourceMapEntryAgnostic &mapEntry, uint32_t &size) = 0;

    virtual void RemoveEntry(const ResourceMapEntryAgnostic &mapEntry) = 0;
    virtual void RebuildResources() = 0;
    virtual AppendBehavior AppendResources(const std::vector<ResourceBlob> &entries) = 0;
};

typedef std::vector<std::unique_ptr<ResourceSource>> ResourceSourceArray;

struct SourceTraits
{
    const char *MapFormat;
    const char *VolumeFormat;
};

extern SourceTraits resourceMapSourceTraits;
extern SourceTraits messageMapSourceTraits;

struct FileDescriptorBase
{
    FileDescriptorBase(const std::string &gameFolder, SourceTraits &traits, ResourceSourceFlags sourceFlags) : _gameFolder(gameFolder), _traits(traits), SourceFlags(sourceFlags) {}

    const SourceTraits &_traits;
    const ResourceSourceFlags SourceFlags;
    const std::string _gameFolder;

    std::string _GetMapFilename() const;
    std::string _GetVolumeFilename(int volume) const;
    std::string _GetMapFilenameBak() const;
    std::string _GetVolumeFilenameBak(int volume) const;

    std::unique_ptr<sci::streamOwner> OpenMap() const
    {
        ScopedFile scoped(_GetMapFilename(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
        return std::make_unique<sci::streamOwner>(scoped.hFile);
    }

    bool DoesMapExist() const
    {
        return !!PathFileExists(_GetMapFilename().c_str());
    }

    std::unique_ptr<sci::streamOwner> OpenVolume(int volumeNumber) const
    {
        ScopedFile scoped(_GetVolumeFilename(volumeNumber), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
        return std::make_unique<sci::streamOwner>(scoped.hFile);
    }

    bool DoesVolumeExist(int volumeNumber) const
    {
        return !!PathFileExists(_GetVolumeFilename(volumeNumber).c_str());
    }

    void WriteAndReplaceMapAndVolumes(const sci::ostream &mapStream, const std::unordered_map<int, sci::ostream> &volumeWriteStreams) const
    {
        // TODO: Verify we can write to the orignal files. Or do we need to bother? We'll produce nice error messages anyway.
        // The only time it might be necessary is for .scr and .hep files, since we need those to both succeed or both fail

        {
            // Write the volumes to their bak files.
            for (const auto &volumeStream : volumeWriteStreams)
            {
                ScopedFile holderPackage(_GetVolumeFilenameBak(volumeStream.first), GENERIC_WRITE, 0, CREATE_ALWAYS);
                holderPackage.Write(volumeStream.second.GetInternalPointer(), volumeStream.second.GetDataSize());
            }

            // Now the map
            ScopedFile holderMap(_GetMapFilenameBak(), GENERIC_WRITE, 0, CREATE_ALWAYS);
            holderMap.Write(mapStream.GetInternalPointer(), mapStream.GetDataSize());
        }

        // Move the volumes over
        for (const auto &volumeStream : volumeWriteStreams)
        {
            std::string package_name = _GetVolumeFilename( volumeStream.first);
            deletefile(package_name);
            movefile(_GetVolumeFilenameBak( volumeStream.first), package_name);
        }

        // Nothing to do at this point if it fails.
        std::string resmap_name = _GetMapFilename();
        deletefile(resmap_name);
        movefile(_GetMapFilenameBak(), resmap_name);
    }
};

struct FileDescriptorResourceMap : public FileDescriptorBase
{
    FileDescriptorResourceMap(const std::string &gameFolder) : FileDescriptorBase(gameFolder, resourceMapSourceTraits, ResourceSourceFlags::ResourceMap) {}
};
struct FileDescriptorMessageMap : public FileDescriptorBase
{
    FileDescriptorMessageMap(const std::string &gameFolder) : FileDescriptorBase(gameFolder, messageMapSourceTraits, ResourceSourceFlags::MessageMap) {}
};

bool IsResourceCompatible(const ResourceBlob &blob);

// Use for both resource.map and message.map
template<typename _TNavigator, typename _FileDescriptor>
class MapAndPackageSource : public ResourceSource, public _TNavigator, public _FileDescriptor
{
public:
    MapAndPackageSource(SCIVersion version, ResourceHeaderReadWrite headerReadWrite, const std::string &gameFolder) :
        _headerReadWrite(headerReadWrite),
        _version(version),
        _FileDescriptor(gameFolder)
        {}

    bool ReadNextEntry(ResourceTypeFlags typeFlags, IteratorState &state, ResourceMapEntryAgnostic &entry, std::vector<uint8_t> *optionalRawData) override
    {
        return NavAndReadNextEntry(typeFlags, GetMapStream(), state, entry, optionalRawData);
    }

    sci::istream GetHeaderAndPositionedStream(const ResourceMapEntryAgnostic &mapEntry, ResourceHeaderAgnostic &headerEntry) override
    {
        sci::istream packageByteStream = _GetVolumeStream(mapEntry.PackageNumber);
        if (!packageByteStream.good())
        {
            // TODO: This is a failure, but corrupted data possibly... just continue and log it? Instead of ending
            // Or make an empty resource.
            throw std::exception("corrupted resource!");
        }

        packageByteStream.seekg(mapEntry.Offset);

        headerEntry = (*_headerReadWrite.reader)(packageByteStream, _version, this->SourceFlags, mapEntry.PackageNumber);

        return packageByteStream;
    }

    virtual sci::istream GetPositionedStreamAndResourceSizeIncludingHeader(const ResourceMapEntryAgnostic &mapEntry, uint32_t &size) override
    {
        sci::istream packageByteStream = _GetVolumeStream(mapEntry.PackageNumber);
        if (!packageByteStream.good())
        {
            throw std::exception("corrupted resource!");
        }

        packageByteStream.seekg(mapEntry.Offset);
        ResourceHeaderAgnostic headerEntry = (*_headerReadWrite.reader)(packageByteStream, _version, this->SourceFlags, mapEntry.PackageNumber);
        uint32_t headerSize = packageByteStream.tellg() - mapEntry.Offset;
        size = headerSize + headerEntry.cbCompressed;
        packageByteStream.seekg(mapEntry.Offset);
        return packageByteStream;
    }

    virtual void RemoveEntry(const ResourceMapEntryAgnostic &mapEntryToRemove) override
    {
        std::unordered_map<int, sci::ostream> volumeStreamWrites;

        // Find the missing chunk in the volume stream.
        ResourceHeaderAgnostic rh;
        sci::istream volumeStream = GetHeaderAndPositionedStream(mapEntryToRemove, rh);
        uint32_t offsetOfRemovedEntry = mapEntryToRemove.Offset;
        uint32_t originalOffsetOfEntryAfterRemovedEntry = volumeStream.tellg() + rh.cbCompressed;
        _TNavigator::EnsureResourceAlignment(originalOffsetOfEntryAfterRemovedEntry);

        // Deleting the entry from the resource map itself is the job of the navigator.
        // First though, the job we can do here is the following:
        // Copy over the volume with the resource in question removed:
        volumeStream.seekg(0);
        transfer(volumeStream, volumeStreamWrites[mapEntryToRemove.PackageNumber], offsetOfRemovedEntry);
        // Ensure alignment for when we're writing the next resource here.
        _TNavigator::EnsureResourceAlignment(volumeStreamWrites[mapEntryToRemove.PackageNumber]);
        // We need to figure out how much to adjust the offsets of the resources that follow the one we removed
        uint32_t sizeofSectionRemoved = originalOffsetOfEntryAfterRemovedEntry - volumeStreamWrites[mapEntryToRemove.PackageNumber].tellp();

        // The point we read from needs to be aligned potentially... the next resource might not start
        // right there.
        volumeStream.seekg(originalOffsetOfEntryAfterRemovedEntry);
        transfer(volumeStream, volumeStreamWrites[mapEntryToRemove.PackageNumber], volumeStream.getBytesRemaining());

        // Now copy over every resource map entry except the one for the resource in question.
        // And other entries may need to be modified.
        bool found = false;
        IteratorState state;
        ResourceMapEntryAgnostic entryExisting;
        sci::ostream mapStreamWrite1;
        sci::ostream mapStreamWrite2;
        sci::istream mapStream = GetMapStream();
        mapStream.seekg(0);
        while (NavAndReadNextEntry(ResourceTypeFlags::All, mapStream, state, entryExisting, nullptr))
        {
            // Three cases:
            //  1) if this is the one we're deleting, skip it (don't write the entry)
            //  2) if this is an entry pointing to the same volume as the one we're deleting, AND it's offset is after
            //      the one we're deleting, modify the offset and write it
            //  3) otherwise write the entry as is
            if (entryExisting == mapEntryToRemove)
            {
                found = true; // 1
            }
            else
            {
                if ((entryExisting.PackageNumber == mapEntryToRemove.PackageNumber) && (entryExisting.Offset > mapEntryToRemove.Offset))
                {
                    entryExisting.Offset -= sizeofSectionRemoved; // 2
                }
                WriteEntry(entryExisting, mapStreamWrite1, mapStreamWrite2, false);
            }
        }

        assert(found);

        // Combine the two write streams. Or rather, append stream 2 to the end of stream 1.
        FinalizeMapStreams(mapStreamWrite1, mapStreamWrite2);
        
        // Now we have mapStreamWrite1 and volumeStreamWrite that have the needed data.
        // Let's ask the _FileDescriptor to replace things.
        this->WriteAndReplaceMapAndVolumes(mapStreamWrite1, volumeStreamWrites);
    }

    void RebuildResources() override
    {
        IteratorState iteratorState;

        // Resource tracking
        std::unordered_set<int> encounteredResources[NumResourceTypes];

        int rebuildPackageNumber = _version.DefaultVolumeFile;
        std::unordered_map<int, sci::ostream> volumeWriteStreams;
        // This is a large amount of data, so minimize reallocations.
        volumeWriteStreams[rebuildPackageNumber].EnsureCapacity(4 * 1024 * 1024);

        // Our two map streams:
        sci::ostream mapStreamWrite1;
        sci::ostream mapStreamWrite2;

        ResourceMapEntryAgnostic entryExisting;
        while (ReadNextEntry(ResourceTypeFlags::All, iteratorState, entryExisting, nullptr))
        {
            int type = (int)entryExisting.Type;
            if (type < ARRAYSIZE(encounteredResources))
            {
                if (encounteredResources[type].find(entryExisting.Number) == encounteredResources[type].end())
                {
                    // Add it
                    encounteredResources[type].insert(entryExisting.Number);

                    // Take note of the offset of the volume we're writing to
                    _TNavigator::EnsureResourceAlignment(volumeWriteStreams[rebuildPackageNumber]);
                    uint32_t newResourceOffset = volumeWriteStreams[rebuildPackageNumber].tellp();

                    // Copy the resource data over
                    // We don't really care about the headerEntry. All we need to know is the position and size of the data
                    // we want to copy. The position is given by the mapentry offset, and the size is the cbCompressed plus the
                    // header size. Hmm, but we don't know the header size. I guess we could have another function that is
                    uint32_t totalResourceSize;
                    sci::istream volumneReadStream = GetPositionedStreamAndResourceSizeIncludingHeader(entryExisting, totalResourceSize);
                    // Now transfer this to the write stream
                    transfer(volumneReadStream, volumeWriteStreams[rebuildPackageNumber], totalResourceSize);

                    // Then write this entry to the map, after modifying our map header's offset accordingly 
                    entryExisting.Offset = newResourceOffset;
                    entryExisting.PackageNumber = rebuildPackageNumber;
                    WriteEntry(entryExisting, mapStreamWrite1, mapStreamWrite2, false);
                }
            }
        }

        // Combine the two write streams. Or rather, append stream 2 to the end of stream 1.
        FinalizeMapStreams(mapStreamWrite1, mapStreamWrite2);

        // Now we have mapStreamWrite1 and volumeStreamWrite that have the needed data.
        // Let's ask the _FileDescriptor to replace things.
        this->WriteAndReplaceMapAndVolumes(mapStreamWrite1, volumeWriteStreams);
    }

    virtual ::AppendBehavior AppendResources(const std::vector<ResourceBlob> &blobs)
    {
        // For this, we append the resource data to the end of the volume file.
        // We could have any number of volumes being saved to, so we'll use a map.
        std::unordered_map<int, sci::ostream> volumeWriteStreams;

        sci::ostream mapStreamWriteMain;
        sci::ostream mapStreamWriteSecondary;

        // Now append our resources to the volume. We'll need to ask the navigator to write the version specific header,
        // then we can write the raw data.
        for (const ResourceBlob &blob : blobs)
        {
            assert(IsResourceCompatible(blob));
            ResourceHeaderAgnostic header = blob.GetHeader();

            // Possibly copy over a new volume, if we haven't written to it yet
            if (volumeWriteStreams.find(header.PackageHint) == volumeWriteStreams.end())
            {
                sci::istream volumeReadStream = _GetVolumeStream(blob.GetPackageHint());
                volumeReadStream.seekg(0);
                transfer(volumeReadStream, volumeWriteStreams[header.PackageHint], volumeReadStream.getBytesRemaining());
            }

            // Take note of the offset so we can create a map entry
            _TNavigator::EnsureResourceAlignment(volumeWriteStreams[header.PackageHint]);
            uint32_t resourceOffset = volumeWriteStreams[header.PackageHint].tellp();

            // Write the map entry
            ResourceMapEntryAgnostic newMapEntry;
            newMapEntry.Number = (uint16_t)header.Number;
            newMapEntry.Type = header.Type;
            newMapEntry.PackageNumber = (uint8_t)header.PackageHint;
            newMapEntry.Offset = resourceOffset;
            WriteEntry(newMapEntry, mapStreamWriteMain, mapStreamWriteSecondary, true);

            // Write the header to the volume
            header.CompressionMethod = 0; // We never write with compression, currently
            (*_headerReadWrite.writer)(volumeWriteStreams[header.PackageHint], blob.GetHeader());
            
            // Follow the volume header with the actual resource data
            transfer(blob.GetReadStream(), volumeWriteStreams[header.PackageHint], blob.GetDecompressedLength());
        }

        // Now we need to follow up with the rest of the map entries. For SCI0, we could just copy over the original resource map.
        // However, SCI1 has a more complex structure requiring us to write entries one by one.
        ResourceMapEntryAgnostic existingMapEntry;
        IteratorState iteratorState;
        while (ReadNextEntry(ResourceTypeFlags::All, iteratorState, existingMapEntry, nullptr))
        {
            WriteEntry(existingMapEntry, mapStreamWriteMain, mapStreamWriteSecondary, false);
        }

        // Combine the two write streams. Or rather, append stream 2 to the end of stream 1.
        FinalizeMapStreams(mapStreamWriteMain, mapStreamWriteSecondary);

        // Now we have mapStreamWrite1 and volumeStreamWrite that have the needed data.
        // Let's ask the _FileDescriptor to replace things.
        this->WriteAndReplaceMapAndVolumes(mapStreamWriteMain, volumeWriteStreams);

        return _TNavigator::AppendBehavior;
    }

protected:
    sci::istream _GetVolumeStream(int volumeNumber)
    {
        auto result = _volumeStreams.find(volumeNumber);
        if (result != _volumeStreams.end())
        {
            return result->second->getReader();
        }

        _volumeStreams[volumeNumber] = _FileDescriptor::OpenVolume(volumeNumber);
        return _volumeStreams.find(volumeNumber)->second->getReader();
    }

    sci::istream &GetMapStream()
    {
        if (!_map)
        {
            _map = move(_FileDescriptor::OpenMap());
            _mapStream = std::make_unique<sci::istream>(_map->getReader());
        }
        return *_mapStream;
    }

private:
    ResourceHeaderReadWrite _headerReadWrite;
    SCIVersion _version;

    std::unique_ptr<sci::streamOwner> _map;
    std::unique_ptr<sci::istream> _mapStream;
    std::unordered_map<int, std::unique_ptr<sci::streamOwner>> _volumeStreams;
};

// ResourceSource for isolated patch files
class PatchFilesResourceSource : public ResourceSource
{
public:
    PatchFilesResourceSource(SCIVersion version, const std::string &gameFolder) :
        _gameFolder(gameFolder),
        _gameFolderSpec(gameFolder + "\\*.*"),
        _hFind(INVALID_HANDLE_VALUE),
        _version(version),
        _stillMore(true)
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

    // Muck
    int _nextIndex;
    std::unordered_map<int, std::string> _indexToFilename;
    std::unordered_map<int, std::unique_ptr<sci::streamOwner>> _streamHolder;
};

struct AudioMapComponent;

// ResourceSource for SCI1.1 audio resources
class AudioResourceSource : public ResourceSource
{
public:
    AudioResourceSource(SCIVersion version, const std::string &gameFolder) :
        _gameFolder(gameFolder),
        _version(version)
    {}

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
    void _Finalize(AudioMapComponent &newAudioMap, sci::ostream &newVolumeStream);
    void _CopyWithoutThese(const AudioMapComponent &audioMap, AudioMapComponent &newAudioMap, sci::istream &oldReader, sci::ostream &newVolumeStream, const std::set<uint16_t> &removeThese);
    std::string _GetAudioVolumePath(bool bak, ResourceSourceFlags *sourceFlags = nullptr);
    void _EnsureAudioVolume();

    std::string _gameFolder;
    SCIVersion _version;
    ResourceSourceFlags _sourceFlags;
    std::unique_ptr<sci::streamOwner> _volumeStreamOwner;
};


