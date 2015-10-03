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
        return std::make_unique<MapAndPackageSource<SCI1MapNavigator<RESOURCEMAPENTRY_SCI1>, _TFileDescriptor>>(version, MakeResourceHeaderReadWriter<RESOURCEHEADER_SCI1>(), gameFolder);
    }
    else if (version.MapFormat == ResourceMapFormat::SCI11)
    {
        return std::make_unique<MapAndPackageSource<SCI1MapNavigator<RESOURCEMAPENTRY_SCI1_1>, _TFileDescriptor>>(version, MakeResourceHeaderReadWriter<RESOURCEHEADER_SCI1>(), gameFolder);
    }
    return std::unique_ptr<ResourceSource>(nullptr);
}

std::unique_ptr<ResourceSource> CreateResourceSource(const std::string &gameFolder, SCIVersion version, ResourceSourceFlags source, ResourceSourceAccessFlags access, int mapContext)
{
    if (source == ResourceSourceFlags::ResourceMap)
    {
        return _CreateResourceSource<FileDescriptorResourceMap>(gameFolder, version, source);
    }
    else if (source == ResourceSourceFlags::MessageMap)
    {
        return _CreateResourceSource<FileDescriptorMessageMap>(gameFolder, version, source);
    }
    else if (source == ResourceSourceFlags::PatchFile)
    {
        return std::make_unique<PatchFilesResourceSource>(version, gameFolder, ResourceSourceFlags::PatchFile);
    }
    else if ((source == ResourceSourceFlags::Aud) || (source == ResourceSourceFlags::Sfx))
    {
        return std::make_unique<AudioResourceSource>(version, gameFolder, mapContext, access);
    }
    else if (source == ResourceSourceFlags::AudioCache)
    {
        return std::make_unique<AudioCacheResourceSource>(version, gameFolder, mapContext, access);
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

    // This is the thing that changes based on version and messagemap or blah.
    std::unique_ptr<ResourceSource> resourceSource = CreateResourceSource(resourceMap.GetGameFolder(), data.GetVersion(), data.GetSourceFlags(), ResourceSourceAccessFlags::ReadWrite);
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
                            if (0 == memcmp(data1, data2, header.cbCompressed))
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
