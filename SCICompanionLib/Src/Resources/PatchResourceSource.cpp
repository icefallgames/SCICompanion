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
#include "PatchResourceSource.h"
#include "ResourceBlob.h"

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
        headerEntry.Base36Number = mapEntry.Base36Number;
        headerEntry.Type = mapEntry.Type;
        headerEntry.CompressionMethod = 0;
        headerEntry.Version = _version;

        readStream.seekg(mapEntry.Offset);
        headerEntry.cbDecompressed = readStream.getBytesRemaining();
        headerEntry.cbCompressed = readStream.getBytesRemaining();
        headerEntry.SourceFlags = _sourceFlags;
        headerEntry.PackageHint = 0;    // No package.

        return readStream;
    }
    return sci::istream(nullptr, 0); // Empty stream....
}

void PatchFilesResourceSource::RemoveEntry(const ResourceMapEntryAgnostic &mapEntry)
{
    std::string filename = GetFileNameFor(mapEntry.Type, mapEntry.Number, mapEntry.Base36Number, _version);
    std::string fullPath = _gameFolder + "\\" + filename;
    deletefile(fullPath);
}

AppendBehavior PatchFilesResourceSource::AppendResources(const std::vector<const ResourceBlob*> &blobs)
{
    for (const ResourceBlob *blob : blobs)
    {
        std::string filename = GetFileNameFor(*blob);
        std::string fullPath = _gameFolder + "\\" + filename;
        std::string bakPath = _gameFolder + "\\" + filename + ".bak";
        // Write to the bak file
        {
            ScopedFile file(bakPath, GENERIC_WRITE, 0, CREATE_ALWAYS);
            blob->SaveToHandle(file.hFile, true);
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
