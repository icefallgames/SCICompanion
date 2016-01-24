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
#include "CompiledScript.h"
#include "Disassembler.h"
#include "ResourceContainer.h"
#include "ResourceEntity.h"
#include "AudioMap.h"
#include "SoundUtil.h"
#include "PMachine.h"
#include "ResourceBlob.h"
#include "View.h"
#include "CodeInspector.h"

using namespace std;

ViewFormat CResourceMap::_DetectViewVGAVersion()
{
    ViewFormat viewFormat = _gameFolderHelper.Version.ViewFormat;
    // Enclose this in a try/catch block, as we need to be robust here.
    try
    {
        int remainingToCheck = 5;
        auto viewContainer = Resources(ResourceTypeFlags::View, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::ExcludePatchFiles);
        for (auto &blob : *viewContainer)
        {
            sci::istream stream = blob->GetReadStream();
            if (stream.GetDataSize() >= 16)
            {
                // Check the header sizes.
                uint16_t headerSize;
                stream >> headerSize;
                uint8_t loopHeaderSize, celHeaderSize;
                stream.seekg(12);
                stream >> loopHeaderSize;
                stream >> celHeaderSize;
                if ((headerSize >= 16) && (loopHeaderSize >= 16) && (celHeaderSize >= 32))
                {
                    remainingToCheck--;
                    if (remainingToCheck)
                    {
                        // Still possibly VGA1.1 views.
                        continue;
                    }
                }
            }
            break;
        }

        viewFormat = (remainingToCheck == 0) ? ViewFormat::VGA1_1 : ViewFormat::VGA1;
    }
    catch (std::exception)
    {

    }

    return viewFormat;
}

ResourcePackageFormat CResourceMap::_DetectPackageFormat()
{
    // Some early bailouts:
    if (_gameFolderHelper.Version.MapFormat == ResourceMapFormat::SCI11)
    {
        return ResourcePackageFormat::SCI11;
    }

    FileDescriptorResourceMap resourceMapFileDescriptor(_gameFolderHelper.GameFolder);
    
    ResourcePackageFormat packageFormat = ResourcePackageFormat::SCI0;
    // Assume there is always a resource.000 or resource.001
    uint32_t dataReadLimit = 0x100000;
    HANDLE hFile = CreateFile(resourceMapFileDescriptor._GetVolumeFilename(0).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        hFile = CreateFile(resourceMapFileDescriptor._GetVolumeFilename(1).c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    }
    if (hFile != INVALID_HANDLE_VALUE)
    {
        ScopedHandle holder;
        holder.hFile = hFile;
        sci::streamOwner streamHolder(holder.hFile);
        sci::istream byteStream = streamHolder.getReader();
        bool sci1Aligned = false;

        while ((byteStream.getBytesRemaining() >= 9) && (byteStream.tellg() < dataReadLimit))
        {
            // [type/resid], or [type,resid]
            byteStream.skip((packageFormat == ResourcePackageFormat::SCI0) ? 2 : 3);
            uint16_t packedSize, unpackedSize, compression;
            byteStream >> packedSize;
            byteStream >> unpackedSize;
            byteStream >> compression;

            int offs = (packageFormat < ResourcePackageFormat::SCI11) ? 4 : 0;
            // Check for impossibilities
            bool tryNext =
                (compression > 20) ||                                                       // invalid compression for any version (up to SCI1.1)
                (unpackedSize < (packedSize - offs)) ||                                     // uncompressed is smaller than packed
                ((compression == 0) && (unpackedSize != (packedSize - offs))) ||            // no compression and uncompressed isn't the same as compressed
                ((packageFormat < ResourcePackageFormat::SCI11) && (compression > 4));      // an early format with compression > 4
            if (tryNext)
            {
                if (packageFormat == ResourcePackageFormat::SCI0)
                {
                    packageFormat = ResourcePackageFormat::SCI1;
                }
                else if (packageFormat == ResourcePackageFormat::SCI1)
                {
                    packageFormat = ResourcePackageFormat::SCI11;
                }
                else if ((packageFormat == ResourcePackageFormat::SCI11) && !sci1Aligned)
                {
                    // Some sci1s are aligned on word boundaries (e.g. QFG1-VGA)
                    sci1Aligned = true;
                }
                else
                {
                    if (_gameFolderHelper.Version.MapFormat != ResourceMapFormat::SCI0)
                    {
                        // Even though this is not supported yet...
                        packageFormat = ResourcePackageFormat::SCI2;
                    }
                    break;
                }

                // Start from the beginning again
                byteStream.seekg(0);
            }
            else
            {
                // Keep going to the next resource
                if (packageFormat < ResourcePackageFormat::SCI11)
                {
                    byteStream.seekg(packedSize - 4, ios_base::cur);
                }
                else if (packageFormat == ResourcePackageFormat::SCI11)
                {
                    byteStream.seekg(sci1Aligned && ((9 + packedSize) % 2) ? packedSize + 1 : packedSize, ios_base::cur);
                }
                else
                {
                    assert(false);
                }
            }
        }
    }
    return packageFormat;
}

bool CResourceMap::_HasEarlySCI0Scripts()
{
    bool hasEarly = false;
    // Look at script 0
    std::unique_ptr<ResourceBlob> data = MostRecentResource(ResourceType::Script, 0, false);
    if (data)
    {
        sci::istream byteStream = data->GetReadStream();
        uint32_t offset = 2;
        while (byteStream.GetDataSize() > offset)
        {
            byteStream.seekg(offset);
            uint16_t objectType;
            byteStream >> objectType;

            if (objectType == 0)
            {
                offset += 2;
                hasEarly = (offset == byteStream.GetDataSize());
                break;  // Reached the end
            }

            if (objectType >= 17)
            {
                break;  // Invalid type
            }

            uint16_t skip;
            byteStream >> skip;

            if (skip < 2)
            {
                break;  // Invalid size
            }

            offset += skip;
        }
    }
    return hasEarly;
}

void CResourceMap::_DetectSoundType()
{
    // We'll mirror (a slightly simplified version of) what ScummVM does here, which
    // is to look for certain kernel calls in sound::play
    // First, we need to find where the Sound object lives.
    try
    {
        GlobalCompiledScriptLookups scriptLookups;
        if (scriptLookups.Load(_gameFolderHelper))
        {
            uint16_t species;
            if (scriptLookups.GetGlobalClassTable().LookupSpeciesCompiledName("Sound", species))
            {
                uint16_t soundScript;
                if (scriptLookups.GetGlobalClassTable().GetSpeciesScriptNumber(species, soundScript))
                {
                    std::vector<CompiledScript*> allScripts = scriptLookups.GetGlobalClassTable().GetAllScripts();
                    for (auto compiledScript : allScripts)
                    {
                        if (compiledScript->GetScriptNumber() == soundScript)
                        {
                            SoundFormat soundFormat = SoundFormat::SCI0;
                            uint16_t pushiParam = 0;
                            bool foundTarget = false;
                            InspectScriptCode(*compiledScript,
                                &scriptLookups,
                                "Sound",
                                "play",
                                [&pushiParam, &foundTarget, &soundFormat](Opcode opcode, const uint16_t *operands, uint16_t currentPCOffset)
                            {
                                if (opcode == Opcode::PUSHI)
                                {
                                    pushiParam = operands[0];
                                }
                                else if (opcode == Opcode::CALLK)
                                {
                                    uint16_t kernelIndex = operands[0];
                                    if (kernelIndex == 45) // DoSound (SCI1)
                                    {
                                        if (pushiParam == 1)
                                        {
                                            soundFormat = SoundFormat::SCI0;
                                        }
                                        else
                                        {
                                            soundFormat = SoundFormat::SCI1;
                                        }
                                        return false; // Done
                                    }
                                }
                                return true; // To keep going.
                            }
                            );
                            _gameFolderHelper.Version.SoundFormat = soundFormat;
                            break;
                        }
                    }
                }
            }
        }
    }
    catch (std::exception)
    {
    }
}

KernelSet CResourceMap::_DetectKernelSet()
{
    KernelSet kernelSet = KernelSet::Provided;
    if (_gameFolderHelper.Version.PackageFormat >= ResourcePackageFormat::SCI2)
    {
        // Set it to something reasonable in case something goes wrong.
        kernelSet = KernelSet::SCI2;

        // Figure out SCI2 kernel is being used. We'll mirror what ScummVM does here, which
        // is to look for the kernel call in Sound::play().
        // First, we need to find where the Sound object lives.
        try
        {
            GlobalCompiledScriptLookups scriptLookups;
            if (scriptLookups.Load(_gameFolderHelper))
            {
                uint16_t species;
                if (scriptLookups.GetGlobalClassTable().LookupSpeciesCompiledName("Sound", species))
                {
                    uint16_t soundScript;
                    if (scriptLookups.GetGlobalClassTable().GetSpeciesScriptNumber(species, soundScript))
                    {
                        std::vector<CompiledScript*> allScripts = scriptLookups.GetGlobalClassTable().GetAllScripts();
                        for (auto compiledScript : allScripts)
                        {
                            if (compiledScript->GetScriptNumber() == soundScript)
                            {
                                InspectScriptCode(*compiledScript,
                                    &scriptLookups,
                                    "Sound",
                                    "play",
                                    [&kernelSet](Opcode opcode, const uint16_t *operands, uint16_t currentPCOffset)
                                {
                                    if (opcode == Opcode::CALLK)
                                    {
                                        if (operands[0] == 0x40)
                                        {
                                            kernelSet = KernelSet::SCI2;
                                            return false;
                                        }
                                        else if (operands[0] == 0x75)
                                        {
                                            kernelSet = KernelSet::SCI21;
                                            return false;
                                        }
                                    }
                                    return true;
                                }
                                    );

                                break;
                            }
                        }
                    }
                }
            }
        }
        catch (std::exception)
        { }
    }
    else
    {
        if (DoesResourceExist(ResourceType::Vocab, VocabKernelNames))
        {
            // The kernels are listed in a vocab resource (typical for SCI0).
            kernelSet = KernelSet::Provided;
        }
        else
        {
            // SCI0 and SCI1 use roughly the same kernel functions, except that
            // SCI1 has more.
            kernelSet = KernelSet::SCI0SCI1;
        }
    }
    return kernelSet;
}

ResourceMapFormat CResourceMap::_DetectMapFormat()
{
    ResourceMapFormat mapFormat = ResourceMapFormat::SCI0;

    FileDescriptorResourceMap resourceMapFileDescriptor(_gameFolderHelper.GameFolder);
    std::unique_ptr<sci::streamOwner> streamHolder = resourceMapFileDescriptor.OpenMap();
    sci::istream byteStream = streamHolder->getReader();

    bool mustBeSCI0 = false;
    // Detect SCI0 by checking to see if the last 6 bytes are 0xff
    {
        sci::istream byteStreamSCI0 = byteStream;
        byteStreamSCI0.seekg(-6, std::ios_base::end);
        uint8_t ffs[6];
        byteStreamSCI0.read_data(ffs, sizeof(ffs));
        if (byteStreamSCI0.good())
        {
            mustBeSCI0 = std::count(ffs, ffs + sizeof(ffs), 0xff) == 6;
        }
    }

    // To try to detect SCI1, read at least 7 RESOURCEMAPPREENTRY_SCI1's, or until we have a 0xff
    bool couldBeSCI1 = true;
    bool couldBeSCI2 = false;
    int remainingEntries = 7;
    std::vector<uint16_t> offsets;
    while (byteStream.good() && couldBeSCI1 && (remainingEntries > 0))
    {
        RESOURCEMAPPREENTRY_SCI1 sci1Header;
        byteStream >> sci1Header;
        offsets.push_back(sci1Header.wOffset);
        if (sci1Header.bType == 0xff)
        {
            break;
        }
        if (sci1Header.bType < 0x80)
        {
            couldBeSCI1 = false;
            couldBeSCI2 = true;
            break;
        }
        if ((sci1Header.bType < 0x80) || (sci1Header.bType > 0x91))
        {
            couldBeSCI1 = false;
            break;
        }
        remainingEntries--;
    }

    if (couldBeSCI1 && (remainingEntries == 0))
    {
        // If nothing is conclusive below, just leave at sciVersion1_Late for now
        mapFormat = ResourceMapFormat::SCI1;

        // Check the spacing of resource map offsets. This can help us determine
        // whether or not this SCI1 map is 5 or 6 byte entries
        for (size_t i = 1; i < offsets.size(); i++)
        {
            int diff = offsets[i] - offsets[i - 1];
            bool sci1Multiple = 0 == (diff % sizeof(RESOURCEMAPENTRY_SCI1));       // 6 bytes
            bool sci11Multiple = 0 == (diff % sizeof(RESOURCEMAPENTRY_SCI1_1));    // 5 bytes
            if (sci1Multiple && !sci11Multiple)
            {
                mapFormat = ResourceMapFormat::SCI1;
                break;
            }
            else if (!sci1Multiple && sci11Multiple)
            {
                mapFormat = ResourceMapFormat::SCI11;
                break;
            }
        }
    }
    else
    {
        if (couldBeSCI2 && !mustBeSCI0)
        {
            mapFormat = ResourceMapFormat::SCI2;
        }
        else
        {
            // It's SCI0, in that it doesn't have a list of "pre-entries". But the format of the entries themselves could be "SCI1".
            // Check by interpreting them as SCI0 and seeing if we can open all the volume files we encounter.
            sci::istream byteStreamSCI0 = streamHolder->getReader();
            sci::istream byteStreamSCI1 = streamHolder->getReader();
            std::set<int> volumesSCI0;
            std::set<int> volumesSCI1;
            static_assert(sizeof(RESOURCEMAPENTRY_SCI0) == sizeof(RESOURCEMAPENTRY_SCI0_SCI1LAYOUT), "Mismatched resource map entry size.");
            while (byteStreamSCI0.good())
            {
                RESOURCEMAPENTRY_SCI0 mapEntrySCI0;
                byteStreamSCI0 >> mapEntrySCI0;
                if (mapEntrySCI0.iType == 0x1f)
                {
                    break; // EOF marker
                }
                volumesSCI0.insert(mapEntrySCI0.iPackageNumber);
                RESOURCEMAPENTRY_SCI0_SCI1LAYOUT mapEntrySCI1;
                byteStreamSCI1 >> mapEntrySCI1;
                volumesSCI1.insert(mapEntrySCI1.iPackageNumber);
            }

            // If any of these don't exist, assume that we have SCI1 layout
            bool probablyNotSCI0 = false;
            for (int volume : volumesSCI0)
            {
                FileDescriptorResourceMap resourceMapFileDescriptor(_gameFolderHelper.GameFolder);
                if (!PathFileExists(resourceMapFileDescriptor._GetVolumeFilename(volume).c_str()))
                {
                    probablyNotSCI0 = true;
                    break;
                }
            }

            // Let's verify first though (in case someone just deleted a package file)
            if (probablyNotSCI0)
            {
                for (int volume : volumesSCI1)
                {
                    FileDescriptorResourceMap resourceMapFileDescriptor(_gameFolderHelper.GameFolder);
                    if (!PathFileExists(resourceMapFileDescriptor._GetVolumeFilename(volume).c_str()))
                    {
                        // Game is corrupt...
                        probablyNotSCI0 = false;
                        break;
                    }
                }
            }

            if (probablyNotSCI0)
            {
                mapFormat = ResourceMapFormat::SCI0_LayoutSCI1;
            }
        }
    }
    return mapFormat;
}

enum LofsaType
{
    Unknown,
    Absolute,
    Relative
};

bool CResourceMap::_DetectLofsaFormat()
{
    LofsaType lofsaType = LofsaType::Unknown;

    // Don't load exports, because loading the export table depends on lofsaAbsolute being correct (IsExportWide).
    GlobalCompiledScriptLookups scriptLookups;

    auto scriptContainer = Resources(ResourceTypeFlags::Script, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::ExcludePatchFiles);
    bool continueSearch = true;
    for (auto &blobIt = scriptContainer->begin(); continueSearch && (blobIt != scriptContainer->end()); ++blobIt)
    {
        CompiledScript compiledScript(blobIt.GetResourceNumber(), CompiledScriptFlags::DontLoadExports);
        if (compiledScript.Load(Helper(), _gameFolderHelper.Version, blobIt.GetResourceNumber(), (*blobIt)->GetReadStream()))
        {
            continueSearch = InspectScriptCode(
                compiledScript,
                &scriptLookups,
                "",
                "",
                [&compiledScript, &lofsaType](Opcode opcode, const uint16_t *operands, uint16_t currentPCOffset)
            {
                if ((opcode == Opcode::LOFSA) || (opcode == Opcode::LOFSS))
                {
                    uint16_t absoluteOffset = operands[0];
                    int16_t finalRelativeOffset = (int16_t)currentPCOffset + (int16_t)operands[0];

                    if (absoluteOffset > (uint16_t)(compiledScript.GetRawBytes().size()))
                    {
                        // Out of bounds as an absolute offset, so it must be a relative signed offset.
                        lofsaType = LofsaType::Relative;
                        return false;
                    }
                    else
                    {
                        if ((finalRelativeOffset < 0) || (finalRelativeOffset >= (int16_t)(compiledScript.GetRawBytes().size())))
                        {
                            // Interpreting it as a relative signed offset put us outside the script.
                            lofsaType = LofsaType::Absolute;
                            return false;
                        }
                    }
                }
                return true;
            });
        }
    }

    bool lofsaAbsolute;
    if (lofsaType == LofsaType::Absolute)
    {
        lofsaAbsolute = true;
    }
    else if (lofsaType == LofsaType::Relative)
    {
        lofsaAbsolute = false;
    }
    else
    {
        // We couldn't figure it out. Guess.
        // The 1990 Christmas Card demo needs this (since it has limited code). AFAIK, there are no
        // other games that will hit this and force us to guess (or implement something more complicated).
        lofsaAbsolute = false; 
    }

    return lofsaAbsolute;
}

bool CResourceMap::_DetectIsExportWide()
{
    if (_gameFolderHelper.Version.MapFormat <= ResourceMapFormat::SCI0_LayoutSCI1)
    {
        return false;   // Not wide for SCI0
    }
    if (_gameFolderHelper.Version.SeparateHeapResources)
    {
        return false;   // Never for separate heap resource games
    }
    // Now we're left with the middle ones, which sometimes were 32-bit.
    bool isWide = false;
    auto blob = MostRecentResource(ResourceType::Script, 0, false);
    if (blob)
    {
        sci::istream byteStream = blob->GetReadStream();
        isWide = CompiledScript::DetectIfExportsAreWide(_gameFolderHelper.Version, byteStream);
    }
    return isWide;
}

void CResourceMap::_SniffSCIVersion()
{
    if (_skipVersionSniffOnce)
    {
        _skipVersionSniffOnce = false;
        return;
    }

    // Just as a start...
    _gameFolderHelper.Version = sciVersion0;

    // Audio volume name is easy
    std::string fullPathAud = _gameFolderHelper.GameFolder + "\\" + "resource.aud";
    std::string fullPathSFX = _gameFolderHelper.GameFolder + "\\" + "resource.sfx";
    _gameFolderHelper.Version.AudioVolumeName = AudioVolumeName::None;
    if (PathFileExists(fullPathAud.c_str()))
    {
        _gameFolderHelper.Version.AudioVolumeName = AudioVolumeName::Aud;
    }

    if (PathFileExists(fullPathSFX.c_str()))
    {
        if (_gameFolderHelper.Version.AudioVolumeName == AudioVolumeName::Aud)
        {
            _gameFolderHelper.Version.AudioVolumeName = AudioVolumeName::Both;
        }
        else
        {
            _gameFolderHelper.Version.AudioVolumeName = AudioVolumeName::Sfx;
        }
    }
    // Some games (e.g. SQ6) have wave files as the audio format
    if (_gameFolderHelper.Version.AudioVolumeName != AudioVolumeName::None)
    {
        AudioVolumeName volNameMain = GetVolumeToUse(_gameFolderHelper.Version, NoBase36);
        std::string mainAudioVolumePath = GetAudioVolumePath(_gameFolderHelper.GameFolder, false, volNameMain);
        if (HasWaveHeader(mainAudioVolumePath))
        {
            _gameFolderHelper.Version.AudioIsWav = true;
        }
    }

    // Is there a message file?
    std::string fullPathMessageMap = _gameFolderHelper.GameFolder + "\\" + "message.map";
    std::string fullPathAltMap = _gameFolderHelper.GameFolder + "\\" + "altres.map";
    _gameFolderHelper.Version.MessageMapSource = MessageMapSource::Included;
    if (PathFileExists(fullPathMessageMap.c_str()))
    {
        _gameFolderHelper.Version.MessageMapSource = MessageMapSource::MessageMap;
    }
    else if (PathFileExists(fullPathAltMap.c_str()))
    {
        _gameFolderHelper.Version.MessageMapSource = MessageMapSource::AltResMap;
    }

    _gameFolderHelper.Version.MapFormat = _DetectMapFormat();

    _gameFolderHelper.Version.PackageFormat = _DetectPackageFormat();

    // Use resource.000 as the default package file, or resource.001 if no resource.000 found.
    FileDescriptorResourceMap resourceMapFileDescriptor(_gameFolderHelper.GameFolder);
    _gameFolderHelper.Version.DefaultVolumeFile = resourceMapFileDescriptor.DoesVolumeExist(0) ? 0 : 1;

    // See if this is a version that uses hep files
    // (this may always correspond to ResourceMapFormat::SCI11, but I'm not positive)
    // Nope! PQ1-VGA is ResourceMapFormat::SCI1, but has separate heap resources.
    if (_gameFolderHelper.Version.MapFormat >= ResourceMapFormat::SCI1)
    {
        auto hepContainer = Resources(ResourceTypeFlags::Heap, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::ExcludePatchFiles);
        for (auto &blobIt = hepContainer->begin(); blobIt != hepContainer->end(); ++blobIt)
        {
            _gameFolderHelper.Version.SeparateHeapResources = true;
            break;
        }
    }

    // More about messages...
    _gameFolderHelper.Version.SupportsMessages = (_gameFolderHelper.Version.MessageMapSource != MessageMapSource::Included) || _gameFolderHelper.Version.SeparateHeapResources;
    if (_gameFolderHelper.Version.MessageMapSource == MessageMapSource::Included)
    {
        // Still might support messages... PQ1VGA... well actually, that has a separate message.map file. But... still possible.
        if (_gameFolderHelper.Version.MapFormat >= ResourceMapFormat::SCI1)
        {
            auto msgContainer = Resources(ResourceTypeFlags::Message, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::ExcludePatchFiles);
            for (auto &blobIt = msgContainer->begin(); blobIt != msgContainer->end(); ++blobIt)
            {
                _gameFolderHelper.Version.SupportsMessages = true;
                break;
            }
        }
    }



    // Now that we've determined a resource map format, we can iterate through them.
    // Now see if we can load a palette. If so, then we'll assume we have VGA pics and views
    auto paletteContainer = Resources(ResourceTypeFlags::Palette, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::ExcludePatchFiles);
    // I use a for loop (instead of ranged for), because I don't want to dereference the iterator until needed
    // Dereferencing the iterator will decompress the resource, which we don't want yet (since we're not sure
    // of our decompression)
    for (auto &blobIt = paletteContainer->begin(); blobIt != paletteContainer->end(); ++blobIt)
    {
        _gameFolderHelper.Version.HasPalette = true;
        break;
    }

    // Let's test the first few views for compression format. If any of them use formats > 2, then
    // we know this is not SCI0 compression formats.
    // If any of them are huffman, this is probably an "early SCI1" EGA game but which uses the newer compression formats.
    int remainingToCheck = 10;
    auto viewContainer = Resources(ResourceTypeFlags::View, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::ExcludePatchFiles);
    for (auto &blobIt = viewContainer->begin(); remainingToCheck && (blobIt != viewContainer->end()); ++blobIt)
    {
        if (blobIt.GetResourceHeader().CompressionMethod >= 2)
        {
            // Though 2 is a valid compression method for CompressionFormat::SCI0, it is apparently not
            // used with views.
            _gameFolderHelper.Version.CompressionFormat = CompressionFormat::SCI1;
            break;
        }
        remainingToCheck--;
    }

    // Prescence of a palette does not necessarily indicate a VGA game. Some SCI1 EGA games have palettes left in them from their conversion from VGA.
    // So instead, we'll check the 2nd byte of a view resource, which for early SCI1 games will be 0x80 if it's VGA.
    // Note that this needs to come *after* the compression format detection above.
    if (_gameFolderHelper.Version.HasPalette)
    {
        bool mustBeVGA = (_gameFolderHelper.Version.MapFormat >= ResourceMapFormat::SCI11) || _gameFolderHelper.Version.SeparateHeapResources;
        if (!mustBeVGA)
        {
            auto viewContainer = Resources(ResourceTypeFlags::View, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::ExcludePatchFiles);
            for (auto &view : *viewContainer)
            {
                if (view->GetDecompressedLength() >= 2)
                {
                    uint8_t secondByte = view->GetData()[1];
                    // VGA views should have the second byte as 0x80
                    mustBeVGA = (secondByte == 0x80);
                    break;
                }
            }
        }

        if (mustBeVGA)
        {
            _gameFolderHelper.Version.PicFormat = PicFormat::VGA1;
            _gameFolderHelper.Version.ViewFormat = ViewFormat::VGA1; // We'll get more specific on this later.
        }
    }

    if (_gameFolderHelper.Version.PackageFormat == ResourcePackageFormat::SCI2)
    {
        _gameFolderHelper.Version.PicFormat = PicFormat::VGA2;
    }

    bool needSoundAutoDetect = true;

    // Let's get more specific on view formats.
    if (_gameFolderHelper.Version.ViewFormat != ViewFormat::EGA)
    {
        if (_gameFolderHelper.Version.PackageFormat == ResourcePackageFormat::SCI2)
        {
            // If we have an SCI2 package format, that's a good indication we have VGA2 views
            // They are close to VGA1_1, but not quite.
            _gameFolderHelper.Version.ViewFormat = ViewFormat::VGA2; 
        }
        else
        {
            _gameFolderHelper.Version.ViewFormat = _DetectViewVGAVersion();
        }

        // ASSUMPTION: All VGA games uses SCI1 sound.
        _gameFolderHelper.Version.SoundFormat = SoundFormat::SCI1;
        needSoundAutoDetect = false;
    }
    else
    {
        // The reverse is not true though. For instance, Hoyle is EGA and uses SCI1 sounds.
        // We'll make another assumption for SCI0 though, just to ensure any fanmade games don't have improper version detection.
        // If the resource map and package format are SCI0, we'll assume SCI0 sound.
        if ((_gameFolderHelper.Version.MapFormat <= ResourceMapFormat::SCI0) &&
            (_gameFolderHelper.Version.PackageFormat <= ResourcePackageFormat::SCI0))
        {
            _gameFolderHelper.Version.SoundFormat = SoundFormat::SCI0;
            needSoundAutoDetect = false;
        }
    }

    _gameFolderHelper.Version.GrayScaleCursors = (_gameFolderHelper.Version.ViewFormat == ViewFormat::EGA) ? false : true;

    // As long as not EGA, detect picture format. Look at the first picture and see if it starts with 0x0026 (header size)
    if (_gameFolderHelper.Version.PicFormat != PicFormat::EGA)
    {
        auto picContainer = Resources(ResourceTypeFlags::Pic, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::ExcludePatchFiles);
        for (auto &pic : *picContainer)
        {
            sci::istream stream = pic->GetReadStream();
            uint16_t headerSize;
            stream >> headerSize;
            if (headerSize == 0x26)
            {
                _gameFolderHelper.Version.PicFormat = PicFormat::VGA1_1;
            }
            break;
        }
    }

    // This is a big assumptino for now, it mighjt fall apart in some cases. But from what I've noticed,
    // lofsa changed behavior after SCI0. There may be some SCI0s with new script interpreter though,
    // or SCI1 resource map games with the old?
    // Yup, we midjudge SQ4 here. It should use the new lofsa, but because it uses SCI0 resmap, we say old.
    // ScummVM analyzes the script, which is the correct thing to do.
    // We can short circuit if ResourceMapFormat is SCI11 or higher. Or if it's SCI0 and not VGA?
    // Otherwise, load up script 0, and start poking through the opcodes of the Game subclass.
    // TODO
    if ((_gameFolderHelper.Version.MapFormat <= ResourceMapFormat::SCI0) && (_gameFolderHelper.Version.ViewFormat == ViewFormat::EGA))
    {
        // "early" SCI0
        _gameFolderHelper.Version.lofsaOpcodeIsAbsolute = false;
    }
    else if (_gameFolderHelper.Version.MapFormat >= ResourceMapFormat::SCI11)
    {
        _gameFolderHelper.Version.lofsaOpcodeIsAbsolute = true;
    }
    else
    {
        _gameFolderHelper.Version.lofsaOpcodeIsAbsolute = _DetectLofsaFormat();
    }

    _gameFolderHelper.Version.IsExportWide = _DetectIsExportWide();

    // Which is the parser vocab? If resource 0 is present it's 0. Otherwise it's 900 (or none).
    _gameFolderHelper.Version.MainVocabResource = (MostRecentResource(ResourceType::Vocab, 0, false)) ? 0 : 900;
    _gameFolderHelper.Version.HasSaidVocab = DoesResourceExist(ResourceType::Vocab, _gameFolderHelper.Version.MainVocabResource);

    if (_gameFolderHelper.Version.MapFormat == ResourceMapFormat::SCI0)
    {
        _gameFolderHelper.Version.HasOldSCI0ScriptHeader = _HasEarlySCI0Scripts();
    }

    // Not sure about this, but it seems reasonable. Another clue, I think, is if there is more than just one global palette.
    _gameFolderHelper.Version.sci11Palettes = (_gameFolderHelper.Version.ViewFormat == ViewFormat::VGA1_1);

    // ASSUMPTION: VGA views means fonts can have extended chars. I don't know this for sure,
    // we might need some other detection mechanism.
    _gameFolderHelper.Version.FontExtendedChars = _gameFolderHelper.Version.ViewFormat != ViewFormat::EGA;

    // If we find a map resource that isn't number 65535, then assume that this game has seq resources
    _gameFolderHelper.Version.HasSyncResources = false;
    if (_gameFolderHelper.Version.AudioVolumeName != AudioVolumeName::None)
    {
        std::unique_ptr<ResourceBlob> audioMap65535, audioMap0, audioMapOther;
        std::unique_ptr<ResourceBlob> audioMapMain, audioMapBase36;
        // Usually the audio is map 65535. Sometimes (LB2, non-CD version) it is 0.
        bool found65535 = false;
        bool found0 = false;
        auto audContainer = Resources(ResourceTypeFlags::AudioMap, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::ExcludePatchFiles);
        for (auto &blobIt = audContainer->begin(); blobIt != audContainer->end(); ++blobIt)
        {
            if (blobIt.GetResourceNumber() == 65535)
            {
                found65535 = true;
                audioMap65535 = std::move(*blobIt);
            }
            else if (blobIt.GetResourceNumber() == 0)
            {
                found0 = true;
                audioMap0 = std::move(*blobIt);
            }
            else
            {
                // We found something *other* than 0 or 65535
                _gameFolderHelper.Version.HasSyncResources = true;
                audioMapOther = std::move(*blobIt);
            }
        }
        // Now, if we didn't find either 65535 or 0, that's fine. 65535 may be a patch file.
        if (found65535 || !found0)
        {
            _gameFolderHelper.Version.AudioMapResourceNumber = 65535;
            audioMapMain = std::move(audioMap65535);
            audioMapBase36 = audioMapOther ? std::move(audioMapOther) : std::move(audioMap0);
        }
        else
        {
            // AFAIK, 0 will never be a patch file (but I could be wrong?)
            _gameFolderHelper.Version.AudioMapResourceNumber = 0;
            audioMapMain = std::move(audioMap0);
            audioMapBase36 = audioMapOther ? std::move(audioMapOther) : std::move(audioMap65535);
        }

        // Try to figure out the format of audio maps, in case we need to make an empty one from scratch
        try
        {
            if (audioMapMain)
            {
                std::unique_ptr<ResourceEntity> audioMap = CreateResourceFromResourceData(*audioMapMain, false);
                _gameFolderHelper.Version.MainAudioMapVersion = audioMap->GetComponent<AudioMapComponent>().Version;
            }
            if (audioMapBase36)
            {
                std::unique_ptr<ResourceEntity> audioMap = CreateResourceFromResourceData(*audioMapBase36, false);
                _gameFolderHelper.Version.Base36AudioMapVersion = audioMap->GetComponent<AudioMapComponent>().Version;
            }
        }
        catch (...)
        {

        }
    }

    // Detect resolution (SCI2 and above only... this is an expensive test, since we need to decompress views)
    if (_gameFolderHelper.Version.PackageFormat >= ResourcePackageFormat::SCI2)
    {
        // For SCI2 and above package formats.
        try
        {
            auto viewContainer = Resources(ResourceTypeFlags::View, ResourceEnumFlags::MostRecentOnly | ResourceEnumFlags::ExcludePatchFiles);
            for (auto &viewBlob : *viewContainer)
            {
                // REVIEW: Could test a few instead of just one. The 2nd one from KQ7 would show 320x200, so if the first one were deleted
                // we'd get the wrong result
                std::unique_ptr<ResourceEntity> view = CreateResourceFromResourceData(*viewBlob, false);
                RasterComponent &raster = view->GetComponent<RasterComponent>();
                _gameFolderHelper.Version.DefaultResolution = raster.Resolution;
                break;
            }
        }
        catch (...)
        {

        }
    }

    if (needSoundAutoDetect)
    {
        _DetectSoundType();
    }

    _gameFolderHelper.Version.Kernels = _DetectKernelSet();
}