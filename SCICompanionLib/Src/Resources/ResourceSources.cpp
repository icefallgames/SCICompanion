#include "stdafx.h"
#include "ResourceSources.h"
#include "AppState.h"
#include "format.h"
#include "Audio.h"  // For the audio source
#include "AudioMap.h"  // For the audio source
#include "ResourceEntity.h"
#include "Message.h"

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
    const auto &audioMaps = appState->GetResourceMap().GetAudioMaps();
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

    // To iterate through audio maps, we'll need 3 indices:
    //  - index into audiomap component     (15 bits)
    //  - index into audiomap entry         (16 bits)
    //  - index indicating audio or sync    (1 bit)

    uint32_t audioMapIndex = ((state.mapStreamOffset & 0xfffe0000) >> 17);
    uint32_t audioMapEntryIndex = ((state.mapStreamOffset & 0x1fffe) >> 1);
    uint32_t audioOrSyncIndex = (state.mapStreamOffset & 0x1);
    bool found = false;
    while (!found && (audioMapIndex < audioMaps.size()))
    {
        const AudioMapComponent &audioMap = audioMaps[audioMapIndex]->GetComponent<AudioMapComponent>();
        while (!found && (audioMapEntryIndex < audioMap.Entries.size()))
        {
            const AudioMapEntry &mapEntry = audioMap.Entries[audioMapEntryIndex];
            if (!IsMainAudioMap(audioMap.Version))
            {
                // Audio map with optional sync information.
                // If this AudioMapEntry has two parts (audio and sync), then audioOrSyncIndex = 0 means audio and audioOrSyncIndex == 1 means sync
                if (audioOrSyncIndex == 0)
                {
                    entry.Number = mapEntry.Number;
                    entry.Offset = mapEntry.Offset + mapEntry.SyncSize;
                    entry.ExtraData = 0;
                    entry.PackageNumber = 0;
                    entry.Type = ResourceType::Audio;
                    entry.Base36Number = GetMessageTuple(mapEntry);

                    if (mapEntry.SyncSize)
                    {
                        // This also has a sync resource
                        audioOrSyncIndex++;
                    }
                    else
                    {
                        // It doesn't...
                        audioOrSyncIndex = 0;
                        audioMapEntryIndex++;
                    }
                    found = true;
                }
                else if (audioOrSyncIndex == 1)
                {
                    assert(mapEntry.SyncSize > 0);
                    entry.Number = mapEntry.Number;
                    entry.Offset = mapEntry.Offset;
                    entry.ExtraData = 0;
                    entry.PackageNumber = 0;
                    entry.Type = ResourceType::Sync;
                    entry.Base36Number = GetMessageTuple(mapEntry);

                    audioOrSyncIndex = 0;
                    audioMapEntryIndex++;
                    found = true;
                }
            }
            else
            {
                // Straightforward main audio map
                entry.Number = mapEntry.Number;
                entry.Offset = mapEntry.Offset;
                entry.ExtraData = 0;
                entry.PackageNumber = 0;
                entry.Type = ResourceType::Audio;
                entry.Base36Number = NoBase36;
                audioMapEntryIndex++;
                audioOrSyncIndex = 0;
                found = true;
            }
        }
        if (!found)
        {
            audioMapIndex++;
            audioMapEntryIndex = 0;
        }
    }

    uint32_t newStreamOffset = audioOrSyncIndex;
    newStreamOffset |= audioMapEntryIndex << 1;
    newStreamOffset |= audioMapIndex << 17;
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
    AudioVolumeName volumeToUse = _version.AudioVolumeName;
    if (_version.AudioVolumeName == AudioVolumeName::Both)
    {
        // When both are present, base36-indexed audio is in Aud, and regular audio is in Sfx
        volumeToUse = (base36Number == NoBase36) ? AudioVolumeName::Sfx : AudioVolumeName::Aud;
    }
    return volumeToUse;
}

sci::streamOwner *AudioResourceSource::_EnsureAudioVolume(uint32_t base36Number)
{
    AudioVolumeName volumeToUse = _GetVolumeToUse(base36Number);

    if ((volumeToUse == AudioVolumeName::Sfx) && !_volumeStreamOwnerSfx)
    {
        ScopedFile scoped(_GetAudioVolumePath(false, volumeToUse, &_sourceFlags), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
        _volumeStreamOwnerSfx = std::make_unique<sci::streamOwner>(scoped.hFile);
    }
    if ((volumeToUse == AudioVolumeName::Aud) && !_volumeStreamOwnerAud)
    {
        ScopedFile scoped(_GetAudioVolumePath(false, volumeToUse, &_sourceFlags), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
        _volumeStreamOwnerAud = std::make_unique<sci::streamOwner>(scoped.hFile);
    }
    return (volumeToUse == AudioVolumeName::Sfx) ? _volumeStreamOwnerSfx.get() : _volumeStreamOwnerAud.get();
}

sci::istream AudioResourceSource::GetHeaderAndPositionedStream(const ResourceMapEntryAgnostic &mapEntry, ResourceHeaderAgnostic &headerEntry)
{
    sci::streamOwner *streamOwner = _EnsureAudioVolume(mapEntry.Base36Number);
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
    sci::streamOwner *streamOwner = _EnsureAudioVolume(mapEntry.Base36Number);
    if (streamOwner)
    {
        const auto &audioMaps = appState->GetResourceMap().GetAudioMaps();
        auto it = find_if(audioMaps.begin(), audioMaps.end(),
            [](const std::unique_ptr<ResourceEntity> &audioMap) { return IsMainAudioMap(audioMap->GetComponent<AudioMapComponent>().Version); }
        );
        if (it != audioMaps.end())
        {
            const AudioMapComponent &audioMap = (*it)->GetComponent<AudioMapComponent>();

            sci::istream oldReader = streamOwner->getReader();
            unique_ptr<AudioMapComponent> newAudioMap = make_unique<AudioMapComponent>(audioMap);
            newAudioMap->Entries.clear();

            sci::ostream newVolumeStream;
            newVolumeStream.EnsureCapacity(streamOwner->getReader().GetDataSize());  // Avoid some re-allocs

            // Copy over the old entries, minus the new one
            set<uint16_t> removeThese;
            removeThese.insert(mapEntry.Number);
            _CopyWithoutThese(audioMap, *newAudioMap, oldReader, newVolumeStream, removeThese);

            _Finalize(*newAudioMap, newVolumeStream, mapEntry.Base36Number);
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
    appState->GetResourceMap().SaveAudioMap65535(newAudioMap);

    // If that caused no problems, then finally do the swap of the file, and hopefully that worked.
    deletefile(_GetAudioVolumePath(false, volumeToUse));
    movefile(bakPath, _GetAudioVolumePath(false, volumeToUse));
}

AppendBehavior AudioResourceSource::AppendResources(const std::vector<ResourceBlob> &entries)
{
    for (const ResourceBlob &blob : entries)
    {
        const auto &audioMaps = appState->GetResourceMap().GetAudioMaps();
        auto it = find_if(audioMaps.begin(), audioMaps.end(),
            [](const std::unique_ptr<ResourceEntity> &audioMap) { return IsMainAudioMap(audioMap->GetComponent<AudioMapComponent>().Version); }
        );
        if (it != audioMaps.end())
        {
            sci::streamOwner *streamOwner = _EnsureAudioVolume(blob.GetHeader().Base36Number);

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

            _Finalize(*newAudioMap, newVolumeStream, blob.GetHeader().Base36Number);
        }
    }
    return AppendBehavior::Replace;
}
