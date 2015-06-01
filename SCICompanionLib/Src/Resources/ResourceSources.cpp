#include "stdafx.h"
#include "ResourceSources.h"
#include "AppState.h"
#include "format.h"
#include "Audio.h"  // For the audio source
#include "AudioMap.h"  // For the audio source

using namespace std;

SourceTraits resourceMapSourceTraits =
{
    "resource.map",
    "resource.{:03d}"
};
SourceTraits messageMapSourceTraits =
{
    "message.map",
    "resource.msg"
};

const char *folderFileFormat = "{0}\\{1}";
const char *folderFileFormatBak = "{0}\\{1}.bak";
std::string FileDescriptorBase::_GetMapFilename() const
{
    return fmt::format(folderFileFormat, _gameFolder, _traits.MapFormat);
}

std::string FileDescriptorBase::_GetVolumeFilename(int volume) const
{
    return fmt::format(folderFileFormat, _gameFolder, fmt::format(_traits.VolumeFormat, volume));
}
std::string FileDescriptorBase::_GetMapFilenameBak() const
{
    return fmt::format(folderFileFormatBak, _gameFolder, _traits.MapFormat);
}
std::string FileDescriptorBase::_GetVolumeFilenameBak(int volume) const
{
    return fmt::format(folderFileFormatBak, _gameFolder, fmt::format(_traits.VolumeFormat, volume));
}

bool PatchFilesResourceSource::ReadNextEntry(ResourceTypeFlags typeFlags, IteratorState &state, ResourceMapEntryAgnostic &entry, std::vector<uint8_t> *optionalRawData)
{
    if (_stillMore && (_hFind == INVALID_HANDLE_VALUE))
    {
        _hFind = FindFirstFile(_gameFolderSpec.c_str(), &_findData);
    }

    _stillMore = _stillMore && (_hFind != INVALID_HANDLE_VALUE);
    bool foundOne = false;
    while (_stillMore && !foundOne)
    {
        // For now just hard code pics and views
        if (PathMatchSpec(_findData.cFileName, g_szResourceSpec))
        {
            int number = ResourceNumberFromFileName(_findData.cFileName);
            if (number != -1)
            {
                // We need a valid number.
                // We do need to peek open the file right now.
                ScopedHandle patchFile;
                std::string fullPath = _gameFolder + "\\" + _findData.cFileName;
                patchFile.hFile = CreateFile(fullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                if (patchFile.hFile != INVALID_HANDLE_VALUE)
                {
                    // Read the first two bytes. The first is the type, the next is the offset.
                    uint8_t word[2];
                    DWORD cbRead;
                    if (ReadFile(patchFile.hFile, &word, sizeof(word), &cbRead, nullptr) && (cbRead == sizeof(word)))
                    {
                        ResourceType type = (ResourceType)(word[0] & 0x7f);
                        if (IsFlagSet(typeFlags, ResourceTypeToFlag(type)))
                        {
                            entry.Number = number;
                            entry.Offset = GetResourceOffsetInFile(word[1]) + 2;    // For the word we just read.
                            entry.Type = type;
                            entry.ExtraData = _nextIndex;
                            entry.PackageNumber = 0;

                            // This is hokey, but we need a way to know the filename for an item
                            _indexToFilename[_nextIndex] = _findData.cFileName;
                            _nextIndex++;
                            foundOne = true;
                        }
                    }
                }
            }
        }

        _stillMore = !!FindNextFile(_hFind, &_findData);
    }

    if (!_stillMore)
    {
        FindClose(_hFind);
        _hFind = INVALID_HANDLE_VALUE;
    }

    return _stillMore || foundOne;
}

sci::istream PatchFilesResourceSource::GetHeaderAndPositionedStream(const ResourceMapEntryAgnostic &mapEntry, ResourceHeaderAgnostic &headerEntry)
{
    std::string fileName = _indexToFilename[mapEntry.ExtraData];    // We used package number as a transport vessel for our arbitrary data
    assert(!fileName.empty());
    ScopedHandle patchFile;
    std::string fullPath = _gameFolder + "\\" + fileName;
    patchFile.hFile = CreateFile(fullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (patchFile.hFile != INVALID_HANDLE_VALUE)
    {
        auto streamHolder = std::make_unique<sci::streamOwner>(patchFile.hFile);
        sci::istream readStream = streamHolder->getReader();
        // We need to be owners of this stream data.
        _streamHolder[mapEntry.ExtraData] = move(streamHolder);

        // Now fill in the headerEntry
        headerEntry.Number = mapEntry.Number;
        headerEntry.Type = mapEntry.Type;
        headerEntry.CompressionMethod = 0;
        headerEntry.Version = _version;

        readStream.seekg(mapEntry.Offset);
        headerEntry.cbDecompressed = readStream.getBytesRemaining();
        headerEntry.cbCompressed = readStream.getBytesRemaining();
        headerEntry.SourceFlags = ResourceSourceFlags::PatchFile;
        headerEntry.PackageHint = 0;    // No package.

        return readStream;
    }
    return sci::istream(nullptr, 0); // Empty stream....
}

void PatchFilesResourceSource::RemoveEntry(const ResourceMapEntryAgnostic &mapEntry) 
{
    std::string filename = GetFileNameFor(mapEntry.Type, mapEntry.Number, _version);
    std::string fullPath = _gameFolder + "\\" + filename;
    deletefile(fullPath);
}

AppendBehavior PatchFilesResourceSource::AppendResources(const std::vector<ResourceBlob> &blobs)
{
    for (const ResourceBlob &blob : blobs)
    {
        std::string filename = GetFileNameFor(blob.GetType(), blob.GetNumber(), blob.GetVersion());
        std::string fullPath = _gameFolder + "\\" + filename;
        std::string bakPath = _gameFolder + "\\" + filename + ".bak";
        // Write to the bak file
        {
            ScopedFile file(bakPath, GENERIC_WRITE, 0, CREATE_ALWAYS);
            blob.SaveToHandle(file.hFile, TRUE);
        }
        // move it to the main guy
        deletefile(fullPath);
        movefile(bakPath, fullPath);
    }
    return AppendBehavior::Replace;
}

PatchFilesResourceSource::~PatchFilesResourceSource()
{
    if (_hFind != INVALID_HANDLE_VALUE)
    {
        FindClose(_hFind);
    }
}

bool IsResourceCompatible(const ResourceBlob &blob)
{
    return appState->GetResourceMap().IsResourceCompatible(blob);
}

bool AudioResourceSource::ReadNextEntry(ResourceTypeFlags typeFlags, IteratorState &state, ResourceMapEntryAgnostic &entry, std::vector<uint8_t> *optionalRawData)
{
    const AudioMapComponent *audioMap = appState->GetResourceMap().GetAudioMap65535();
    if (audioMap)
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

        if (state.mapStreamOffset < audioMap->Entries.size())
        {
            AudioMapEntry mapEntry = audioMap->Entries[state.mapStreamOffset];
            state.mapStreamOffset++;
            entry.Number = mapEntry.Number;
            entry.Offset = mapEntry.Offset;
            entry.ExtraData = 0;
            entry.PackageNumber = 0;
            entry.Type = ResourceType::Audio;
            return true;
        }
    }
    return false;
}

std::string AudioResourceSource::_GetAudioVolumePath(bool bak, ResourceSourceFlags *sourceFlags)
{
    ResourceSourceFlags sourceFlagsTemp = (_version.AudioVolumeName == AudioVolumeName::Aud) ? ResourceSourceFlags::Aud : ResourceSourceFlags::Sfx;
    std::string fullPath = _gameFolder + "\\" + ((_version.AudioVolumeName == AudioVolumeName::Aud) ? "resource.aud" : "resource.sfx");
    if (bak)
    {
        fullPath += ".bak";
    }
    
    if (sourceFlags)
    {
        *sourceFlags = sourceFlagsTemp;
    }
    return fullPath;
}
void AudioResourceSource::_EnsureAudioVolume()
{
    if (!_volumeStreamOwner)
    {
        ScopedFile scoped(_GetAudioVolumePath(false, &_sourceFlags), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
        _volumeStreamOwner = std::make_unique<sci::streamOwner>(scoped.hFile);
    }
}

sci::istream AudioResourceSource::GetHeaderAndPositionedStream(const ResourceMapEntryAgnostic &mapEntry, ResourceHeaderAgnostic &headerEntry)
{
    _EnsureAudioVolume();

    if (_volumeStreamOwner)
    {
        sci::istream reader = _volumeStreamOwner->getReader();
        reader.seekg(mapEntry.Offset);
        headerEntry.Number = mapEntry.Number;
        headerEntry.PackageHint = mapEntry.PackageNumber;
        headerEntry.CompressionMethod = 0;
        headerEntry.SourceFlags = _sourceFlags;
        headerEntry.Version = _version;
        headerEntry.Type = ResourceType::Audio;
        headerEntry.cbDecompressed = 0;
        AudioHeader audioHeader;
        reader >> audioHeader;
        reader.seekg(-(int)sizeof(audioHeader), std::ios_base::cur);
        headerEntry.cbDecompressed = audioHeader.headerSize + 2 + audioHeader.sizeExcludingHeader;
        headerEntry.cbCompressed = headerEntry.cbDecompressed;

        return reader;
    }
    return sci::istream(nullptr, 0);
}

void AudioResourceSource::RemoveEntry(const ResourceMapEntryAgnostic &mapEntry) 
{
    _EnsureAudioVolume();
    const AudioMapComponent *audioMap = appState->GetResourceMap().GetAudioMap65535();
    if (audioMap)
    {
        sci::istream oldReader = _volumeStreamOwner->getReader();
        unique_ptr<AudioMapComponent> newAudioMap = make_unique<AudioMapComponent>(*audioMap);
        newAudioMap->Entries.clear();

        sci::ostream newVolumeStream;
        newVolumeStream.EnsureCapacity(_volumeStreamOwner->getReader().GetDataSize());  // Avoid some re-allocs

        // Copy over the old entries, minus the new one
        set<uint16_t> removeThese;
        removeThese.insert(mapEntry.Number);
        _CopyWithoutThese(*audioMap, *newAudioMap, oldReader, newVolumeStream, removeThese);

        _Finalize(*newAudioMap, newVolumeStream);
    }
}

void AudioResourceSource::_CopyWithoutThese(const AudioMapComponent &audioMap, AudioMapComponent &newAudioMap, sci::istream &oldReader, sci::ostream &newVolumeStream, const set<uint16_t> &removeThese)
{
    uint32_t offset = 0;
    for (size_t i = 0; i < audioMap.Entries.size(); i++)
    {
        const AudioMapEntry &oldEntry = audioMap.Entries[i];
        AudioMapEntry copiedEntry = oldEntry;
        if (removeThese.find(copiedEntry.Number) == removeThese.end())
        {
            // Copy over. But how do we know the size? Look at the next offset, or the file size
            uint32_t size;
            if (i == (audioMap.Entries.size() - 1))
            {
                size = oldReader.GetDataSize() - oldEntry.Offset;
            }
            else
            {
                size = audioMap.Entries[i + 1].Offset - oldEntry.Offset;
            }
            copiedEntry.Offset = newVolumeStream.tellp();
            oldReader.seekg(oldEntry.Offset);
            transfer(oldReader, newVolumeStream, size);
            newAudioMap.Entries.push_back(copiedEntry);
        }
        else
        {
            // Skip copying this one over right here.
        }
    }
}

void AudioResourceSource::_Finalize(AudioMapComponent &newAudioMap, sci::ostream &newVolumeStream)
{
    // TODO: Save to bak. Then write resource map thingy. Then swap to real
    // Now let's save the stream to the bak file
    string bakPath = _GetAudioVolumePath(true);
    {
        ScopedFile bakFile(bakPath, GENERIC_WRITE, 0, CREATE_ALWAYS);
        bakFile.Write(newVolumeStream.GetInternalPointer(), newVolumeStream.GetDataSize());
    }

    // This is risky since these operations are not atomic. To minimize issues, we should try to open the
    // file for write first (e.g. test write). And only then saveAudioMap65535
    testopenforwrite(_GetAudioVolumePath(false));

    // Then we'll save our new map.
    appState->GetResourceMap().SaveAudioMap65535(newAudioMap);

    // If that caused no problems, then finally do the swap of the file, and hopefully that worked.
    deletefile(_GetAudioVolumePath(false));
    movefile(bakPath, _GetAudioVolumePath(false));
}

AppendBehavior AudioResourceSource::AppendResources(const std::vector<ResourceBlob> &entries)
{
    _EnsureAudioVolume();
    for (const ResourceBlob &blob : entries)
    {
        const AudioMapComponent *audioMap = appState->GetResourceMap().GetAudioMap65535();
        if (audioMap)
        {
            sci::istream oldReader = _volumeStreamOwner->getReader();
            unique_ptr<AudioMapComponent> newAudioMap = make_unique<AudioMapComponent>(*audioMap);
            newAudioMap->Entries.clear();

            sci::ostream newVolumeStream;
            newVolumeStream.EnsureCapacity(_volumeStreamOwner->getReader().GetDataSize());  // Avoid some re-allocs

            // Copy over the old entries, minus the new one
            set<uint16_t> removeThese;
            removeThese.insert(blob.GetNumber());
            _CopyWithoutThese(*audioMap, *newAudioMap, oldReader, newVolumeStream, removeThese);

            // Append our new resource
            AudioMapEntry newEntry;
            newEntry.Number = blob.GetNumber();
            newEntry.Offset = newVolumeStream.tellp();
            newAudioMap->Entries.push_back(newEntry);
            transfer(blob.GetReadStream(), newVolumeStream, blob.GetCompressedLength());

            _Finalize(*newAudioMap, newVolumeStream);
        }
    }
    return AppendBehavior::Replace;
}
