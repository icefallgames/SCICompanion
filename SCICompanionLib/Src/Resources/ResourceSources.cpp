#include "stdafx.h"
#include "ResourceSources.h"
#include "AppState.h"
#include "format.h"
#include "Audio.h"  // For the audio source
#include "AudioMap.h"  // For the audio source
#include "ResourceEntity.h"
#include "Message.h"
#include "ResourceContainer.h"
#include "SoundUtil.h"
#include <limits>

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

void AudioResourceSource::_EnsureAudioMaps()
{
    // Right now we'll only ever get one audio map. However, I'd like to keep the multimap functionality for now...
    if (_mapContext == -1)
    {
        _mapContext = _version.AudioMapResourceNumber;
    }
    auto resourceContainer = appState->GetResourceMap().Resources(ResourceTypeFlags::Map, ResourceEnumFlags::MostRecentOnly);
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
    ResourceSourceFlags sourceFlagsTemp = (volumeToUse == AudioVolumeName::Aud) ? ResourceSourceFlags::Aud : ResourceSourceFlags::Sfx;
    std::string fullPath = _gameFolder + "\\" + ((volumeToUse == AudioVolumeName::Aud) ? "resource.aud" : "resource.sfx");
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
            assert(reader.peek() == 0x8e);
            headerEntry.PropertyBag[BlobKey::LipSyncDataSize] = mapEntry.ExtraData;
        }

        // To know how big we are, we need to read the audio header.
        // This comes after the lipsync data
        sci::istream audioReader = reader;      // Make a copy
        audioReader.skip(mapEntry.ExtraData);   // Lip sync data size
        AudioHeader audioHeader;
        audioReader >> audioHeader;
        headerEntry.cbDecompressed = audioHeader.headerSize + 2 + audioHeader.sizeExcludingHeader + mapEntry.ExtraData;
        headerEntry.cbCompressed = headerEntry.cbDecompressed;

        return reader;
    }
    return sci::istream(nullptr, 0);
}

void AudioResourceSource::RemoveEntry(const ResourceMapEntryAgnostic &mapEntry) 
{
    assert(IsFlagSet(_access, ResourceSourceAccessFlags::ReadWrite));

    std::unique_ptr<sci::streamOwner> streamOwner = _GetAudioVolume(mapEntry.Base36Number);
    if (streamOwner)
    {
        auto it = find_if(_audioMaps.begin(), _audioMaps.end(),
            [&mapEntry](const std::unique_ptr<ResourceEntity> &audioMap)
        {
            // Either the entry has no base36 number and this is the main audio map, or
            // is *does* have a base 36 number and this its resource number matches that of the audiomap.
            return (audioMap->ResourceNumber == mapEntry.Number) || ((mapEntry.Base36Number == NoBase36) && IsMainAudioMap(audioMap->GetComponent<AudioMapComponent>().Version));
        }
        );
        if (it != _audioMaps.end())
        {
            // Just re-write the audio map without this entry. We'll leave it in the actual file until rebuild.

            // Remove the entry corresponding to mapEntry.
            AudioMapComponent &audioMap = (*it)->GetComponent<AudioMapComponent>();
            audioMap.Entries.erase(
                std::remove_if(audioMap.Entries.begin(), audioMap.Entries.end(),
                [&mapEntry](AudioMapEntry &entry) { return (entry.Number == mapEntry.Number) && (mapEntry.Base36Number == GetMessageTuple(entry)); }
                ),
                audioMap.Entries.end()
                );

            appState->GetResourceMap().SaveAudioMap65535(audioMap, _mapContext);
        }
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

// TODO: Get rid of this?
void AudioResourceSource::_Finalize(AudioMapComponent &newAudioMap, sci::ostream &newVolumeStream, uint32_t base36Number)
{
    AudioVolumeName volumeToUse = _GetVolumeToUse(base36Number);
    // TODO: Save to bak. Then write resource map thingy. Then swap to real
    // Now let's save the stream to the bak file
    string bakPath = _GetAudioVolumePath(true, volumeToUse);
    {
        ScopedFile bakFile(bakPath, GENERIC_WRITE, 0, CREATE_ALWAYS);
        bakFile.Write(newVolumeStream.GetInternalPointer(), newVolumeStream.GetDataSize());
    }

    // This is risky since these operations are not atomic. To minimize issues, we should try to open the
    // file for write first (e.g. test write). And only then saveAudioMap65535
    testopenforwrite(_GetAudioVolumePath(false, volumeToUse));

    // Then we'll save our new map.
    appState->GetResourceMap().SaveAudioMap65535(newAudioMap, _mapContext);

    // If that caused no problems, then finally do the swap of the file, and hopefully that worked.
    deletefile(_GetAudioVolumePath(false, volumeToUse));
    movefile(bakPath, _GetAudioVolumePath(false, volumeToUse));
}

AppendBehavior AudioResourceSource::AppendResources(const std::vector<ResourceBlob> &entries)
{
    assert(IsFlagSet(_access, ResourceSourceAccessFlags::ReadWrite));
    // Audio resource files tend to be large (>300MB for CD-based games), so we won't rewrite them.
    // Instead, we'll append to the end of resource.aud/sfx and then update the offsets in the audio maps.
    // Finally we re-write the audio maps.
    // This will result in audio files that are ever-expanding, so the user will need to rebuild resources
    // every once in a while.

    for (const ResourceBlob &blob : entries)
    {
        auto it = find_if(_audioMaps.begin(), _audioMaps.end(),
            [&blob](std::unique_ptr<ResourceEntity> &audioMap)
        {
            // Either the entry has no base36 number and this is the main audio map, or
            // is *does* have a base 36 number and this its resource number matches that of the audiomap.
            return (audioMap->ResourceNumber == blob.GetNumber()) || ((blob.GetBase36() == NoBase36) && IsMainAudioMap(audioMap->GetComponent<AudioMapComponent>().Version));
        }
        );
        if (it != _audioMaps.end())
        {
            uint32_t messageTuple = blob.GetBase36();
            // 1) Get the audio volume, positioned at the end
            ScopedFile audioVolumeStream(_GetAudioVolumePath(false, _GetVolumeToUse(messageTuple), &_sourceFlags), GENERIC_WRITE, 0, OPEN_ALWAYS);
            // 2) Take note of the offset.
            uint32_t offset = audioVolumeStream.SeekToEnd();
            // 3) Write the resource data, which consists of an optional lip sync section, and the audio data. Then close the file.
            sci::istream audioStream = blob.GetReadStream();
            audioVolumeStream.Write(audioStream.GetInternalPointer(), audioStream.GetDataSize());
            audioVolumeStream.Close();
            
            // 4) Create our new AudioMapEntry 
            AudioMapEntry newEntry;
            newEntry.Number = blob.GetNumber();
            newEntry.Offset = offset;
            SetMessageTuple(newEntry, blob.GetBase36());
            uint32_t lipSyncSize;
            if (blob.GetKeyValue(BlobKey::LipSyncDataSize, lipSyncSize))
            {
                assert(lipSyncSize < (std::numeric_limits<uint16_t>::max)());
                newEntry.SyncSize = static_cast<uint16_t>(lipSyncSize);
            }

            // 5) Find an existing entry and replace it, or add a new one.
            int x = 0; // TODO: Main audio map doens't have tuples... in that case, just a matching number will do. Check this.
            AudioMapComponent &audioMap = (*it)->GetComponent<AudioMapComponent>();
            uint16_t resNumber = (uint16_t)blob.GetNumber();
            auto itFind = find_if(audioMap.Entries.begin(), audioMap.Entries.end(),
                [messageTuple, resNumber](AudioMapEntry &entry) { return GetMessageTuple(entry) == messageTuple && entry.Number == resNumber; }
                );
            if (itFind != audioMap.Entries.end())
            {
                // Reassign.
                *itFind = newEntry;
            }
            else
            {
                // Add
                audioMap.Entries.push_back(newEntry);
            }
            
            // 6) Save the audio map 
            int y = 0; // TOdo: Check main audio map is ok here.
            appState->GetResourceMap().SaveAudioMap65535(audioMap, resNumber);
        }
    }
    // Even though we appended, technically there's no way to get at the old info. Should we return Replace?
    // I think returning Append is ok, because then the user will know that they should rebuild resources.
    // Even though a refresh would make the thing vanish.
    return AppendBehavior::Append;
}

AppendBehavior AudioResourceSource::AppendResourcesOLD(const std::vector<ResourceBlob> &entries)
{
    assert(IsFlagSet(_access, ResourceSourceAccessFlags::ReadWrite));

    for (const ResourceBlob &blob : entries)
    {
        auto it = find_if(_audioMaps.begin(), _audioMaps.end(),
            [&blob](const std::unique_ptr<ResourceEntity> &audioMap)
        {
            // Either the entry has no base36 number and this is the main audio map, or
            // is *does* have a base 36 number and this its resource number matches that of the audiomap.
            return (audioMap->ResourceNumber == blob.GetNumber()) || ((blob.GetBase36() == NoBase36) && IsMainAudioMap(audioMap->GetComponent<AudioMapComponent>().Version));
        }
        );
        if (it != _audioMaps.end())
        {
            std::unique_ptr<sci::streamOwner> streamOwner = _GetAudioVolume(blob.GetHeader().Base36Number);

            const AudioMapComponent &audioMap = (*it)->GetComponent<AudioMapComponent>();

            sci::istream oldReader = streamOwner->getReader();
            unique_ptr<AudioMapComponent> newAudioMap = make_unique<AudioMapComponent>(audioMap);
            newAudioMap->Entries.clear();

            sci::ostream newVolumeStream;
            newVolumeStream.EnsureCapacity(streamOwner->getReader().GetDataSize());  // Avoid some re-allocs

            // Copy over the old entries, minus the new one
            set<uint16_t> removeThese;
            removeThese.insert(blob.GetNumber());
            _CopyWithoutThese(audioMap, *newAudioMap, oldReader, newVolumeStream, removeThese);

            // Append our new resource
            AudioMapEntry newEntry;
            newEntry.Number = blob.GetNumber();
            newEntry.Offset = newVolumeStream.tellp();
            newAudioMap->Entries.push_back(newEntry);
            transfer(blob.GetReadStream(), newVolumeStream, blob.GetCompressedLength());

            // Release the file before writing:
            streamOwner.reset(nullptr);
            _Finalize(*newAudioMap, newVolumeStream, blob.GetHeader().Base36Number);
        }
    }
    return AppendBehavior::Replace;
}

AudioResourceSource::~AudioResourceSource() {}