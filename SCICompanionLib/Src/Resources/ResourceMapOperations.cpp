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
#include "ResourceMap.h"
#include "ResourceBlob.h"
#include "ResourceSources.h"
#include "AudioResourceSource.h"
#include "AudioCacheResourceSource.h"
#include "PatchResourceSource.h"
#include "ResourceMapOperations.h"
#include "resource.h"
#include "RemoveScriptDialog.h"
#include "ResourceContainer.h"
#include "AppState.h"

template<typename _TFileDescriptor>
std::unique_ptr<ResourceSource> _CreateResourceSource(const std::string &gameFolder, SCIVersion version, ResourceSourceFlags source)
{
    if (version.MapFormat == ResourceMapFormat::SCI0)
    {
        return std::make_unique<MapAndPackageSource<SCI0MapNavigator<RESOURCEMAPENTRY_SCI0>, _TFileDescriptor>>(version, MakeResourceHeaderReadWriter<RESOURCEHEADER_SCI0>(), gameFolder);
    }
    else if (version.MapFormat == ResourceMapFormat::SCI0_LayoutSCI1)
    {
        return std::make_unique<MapAndPackageSource<SCI0MapNavigator<RESOURCEMAPENTRY_SCI0_SCI1LAYOUT>, _TFileDescriptor>>(version, MakeResourceHeaderReadWriter<RESOURCEHEADER_SCI0>(), gameFolder);
    }
    else if (version.MapFormat == ResourceMapFormat::SCI1)
    {
        if (version.PackageFormat == ResourcePackageFormat::SCI2)
        {
            return std::make_unique<MapAndPackageSource<SCI1MapNavigator<RESOURCEMAPENTRY_SCI1>, _TFileDescriptor>>(version, MakeResourceHeaderReadWriter<RESOURCEHEADER_SCI2>(), gameFolder);
        }
        else
        {
            return std::make_unique<MapAndPackageSource<SCI1MapNavigator<RESOURCEMAPENTRY_SCI1>, _TFileDescriptor>>(version, MakeResourceHeaderReadWriter<RESOURCEHEADER_SCI1>(), gameFolder);
        }
    }
    else if (version.MapFormat == ResourceMapFormat::SCI11)
    {
        return std::make_unique<MapAndPackageSource<SCI1MapNavigator<RESOURCEMAPENTRY_SCI1_1>, _TFileDescriptor>>(version, MakeResourceHeaderReadWriter<RESOURCEHEADER_SCI1>(), gameFolder);
    }
    else if (version.MapFormat == ResourceMapFormat::SCI2)
    {
        return std::make_unique<MapAndPackageSource<SCI1MapNavigator<RESOURCEMAPENTRY_SCI1>, _TFileDescriptor>>(version, MakeResourceHeaderReadWriter<RESOURCEHEADER_SCI2_1>(), gameFolder);
    }
    return std::unique_ptr<ResourceSource>(nullptr);
}

std::unique_ptr<ResourceSource> CreateResourceSource(ResourceTypeFlags flagsHint, const GameFolderHelper &helper, ResourceSourceFlags source, ResourceSourceAccessFlags access, int mapContext)
{
    if (source == ResourceSourceFlags::ResourceMap)
    {
        return _CreateResourceSource<FileDescriptorResourceMap>(helper.GameFolder, helper.Version, source);
    }
    else if (source == ResourceSourceFlags::MessageMap)
    {
        return _CreateResourceSource<FileDescriptorMessageMap>(helper.GameFolder, helper.Version, source);
    }
    else if (source == ResourceSourceFlags::AltMap)
    {
        return _CreateResourceSource<FileDescriptorAltMap>(helper.GameFolder, helper.Version, source);
    }
    else if (source == ResourceSourceFlags::PatchFile)
    {
        return std::make_unique<PatchFilesResourceSource>(flagsHint, helper.Version, helper.GameFolder, ResourceSourceFlags::PatchFile);
    }
    else if ((source == ResourceSourceFlags::Aud) || (source == ResourceSourceFlags::Sfx))
    {
        return std::make_unique<AudioResourceSource>(helper, mapContext, access);
    }
    else if (source == ResourceSourceFlags::AudioCache)
    {
        // phil, passing null
        return std::make_unique<AudioCacheResourceSource>(&appState->GetResourceMap(), helper, mapContext, access);
    }
    else if (source == ResourceSourceFlags::AudioMapCache)
    {
        return std::make_unique<PatchFilesResourceSource>(ResourceTypeFlags::AudioMap, helper.Version, helper.GameFolder + pszAudioCacheFolder, ResourceSourceFlags::AudioMapCache);
    }
    return std::unique_ptr<ResourceSource>(nullptr);
}

void DeleteResource(CResourceMap &resourceMap, const ResourceBlob &data)
{
    const GameFolderHelper &helper = resourceMap.Helper();

    ResourceTypeFlags typeFlags = ResourceTypeToFlag(data.GetType());
    IteratorState iteratorState;

    // We know we want the message map now.
    bool encounteredOne = false;
    bool isLastOne = true;

    // Since our AudioResourceSource no longer supports removing or adding entries, we need to use the AudioCacheResourceSource instead.
    // That will pull the audio resources out of resource.aud (or whatever), into the cache files directory. Then we'll use the cache
    // files source to delete it.
    // This is a big hacky. Appending resources doesn't have this problem, because we have no way to create new audio resources that
    // don't have ResourceSourceFlags::AudioCache
    ResourceSourceFlags sourcFlags = data.GetSourceFlags();
    if ((sourcFlags == ResourceSourceFlags::Aud) || (sourcFlags == ResourceSourceFlags::Sfx))
    {
        sourcFlags = ResourceSourceFlags::AudioCache;
    }

    // This is the thing that changes based on version and messagemap or blah.
    std::unique_ptr<ResourceSource> resourceSource = CreateResourceSource(ResourceTypeToFlag(data.GetType()), resourceMap.Helper(), sourcFlags, ResourceSourceAccessFlags::ReadWrite);
    if (resourceSource)
    {
        ResourceMapEntryAgnostic mapEntry;
        std::unique_ptr<ResourceMapEntryAgnostic> mapEntryToRemove;
        while (resourceSource->ReadNextEntry(typeFlags, iteratorState, mapEntry, nullptr))
        {
            if ((mapEntry.Number == data.GetNumber()) &&
                (mapEntry.PackageNumber == data.GetPackageHint()))
            {
                if (encounteredOne)
                {
                    // At least two of these resources left.
                    isLastOne = false;
                    if (mapEntryToRemove)
                    {
                        // No point in continuing iterating, we've done our job and found the info we need.
                        break;
                    }
                }
                encounteredOne = true;

                if (!mapEntryToRemove)
                {
                    // If this is a match, now we need to get it from the actual volume and compare bits.
                    ResourceHeaderAgnostic header;
                    sci::istream volumeStream = resourceSource->GetHeaderAndPositionedStream(mapEntry, header);
                    if ((header.cbCompressed == data.GetCompressedLength()) &&
                        (header.cbDecompressed == data.GetDecompressedLength()) &&
                        (header.CompressionMethod == data.GetEncoding()))
                    {
                        // Let's compare some bits to see if it's really identical
                        // Either cbCompressed is the same as decompressed (no compression), or else the ResourceBlob should have compressed data in it for us to compare with.
                        const uint8_t *data1;
                        const uint8_t *data2 = volumeStream.GetInternalPointer() + volumeStream.tellg();
                        size_t dataLength;
                        if (header.CompressionMethod != 0)
                        {
                            data1 = data.GetDataCompressed();
                            dataLength = header.cbCompressed;
                        }
                        else
                        {
                            assert(header.cbCompressed == header.cbDecompressed);
                            data1 = data.GetData();
                            dataLength = header.cbDecompressed;
                        }
                        if (volumeStream.getBytesRemaining() >= dataLength)
                        {
                            if (data1 && data2 && (0 == memcmp(data1, data2, header.cbCompressed)))
                            {
                                // Finally yes, they are identical. We know which one to remove.
                                mapEntryToRemove = std::make_unique<ResourceMapEntryAgnostic>(mapEntry);
                            }
                        }
                    }
                }
            }
        }

        // Do the actual remove
        if (!mapEntryToRemove)
        {
            AfxMessageBox(TEXT("Resource not found."), MB_ERRORFLAGS);
        }
        else
        {
            resourceSource->RemoveEntry(*mapEntryToRemove);
        }

        if (mapEntryToRemove && isLastOne && (data.GetType() == ResourceType::Script))
        {
            // TODO: If this is the "last" of this resource, we have extra work to do.
            CRemoveScriptDialog dialog(static_cast<WORD>(data.GetNumber()));
            if (IDOK == dialog.DoModal())
            {
                // Remove it from the ini
                std::string iniKey = default_reskey(data.GetNumber(), data.GetHeader().Base36Number);
                std::string scriptTitle = helper.GetIniString("Script", iniKey);
                ScriptId scriptId = resourceMap.Helper().GetScriptId(scriptTitle);
                // First, remove from the [Script] section
                WritePrivateProfileString("Script", iniKey.c_str(), nullptr, helper.GetGameIniFileName().c_str());
                // Second, remove from the [Language] section
                WritePrivateProfileString("Language", scriptTitle.c_str(), nullptr, helper.GetGameIniFileName().c_str());

                if (dialog.AlsoDelete())
                {
                    // Remove the heap too
                    std::unique_ptr<ResourceBlob> theHeapOne = resourceMap.MostRecentResource(ResourceType::Heap, data.GetNumber(), false);
                    if (theHeapOne)
                    {
                        DeleteResource(resourceMap, *theHeapOne);
                    }

                    if (!DeleteFile(scriptId.GetFullPath().c_str()))
                    {
                        char szMessage[MAX_PATH * 2];
                        char szReason[200];
                        FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, (DWORD)GetLastError(), 0, szReason, ARRAYSIZE(szReason), nullptr);
                        StringCchPrintf(szMessage, ARRAYSIZE(szMessage), "Couldn't delete script file.\n%s", szReason);
                        AfxMessageBox(szMessage, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
                    }
                }
            }
        }
    }
}
