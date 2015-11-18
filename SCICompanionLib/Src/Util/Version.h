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
#pragma once

// Don't change the numbers here without updating CGameVersionDialog
// They correspond to radio button groups.
enum class ResourceMapFormat : uint8_t
{
    SCI0 = 0,
    SCI0_LayoutSCI1 = 1,
    SCI1 = 2,
    SCI11 = 3,
    SCI2 = 4,
};

enum class ResourcePackageFormat : uint8_t
{
    SCI0 = 0,
    SCI1 = 1,
    SCI11 = 2,
    SCI2 = 3,
};

enum class SoundFormat : uint8_t
{
    SCI0 = 0,
    SCI1 = 1,
};

enum class CompressionFormat : uint8_t
{
    SCI0 = 0,
    SCI1 = 1,
};

enum class ViewFormat : uint8_t
{
    EGA = 0,
    VGA1 = 1,
    VGA1_1 = 2,
    VGA2 = 3,
};

enum class PicFormat : uint8_t
{
    EGA = 0,
    VGA1 = 1,
    VGA1_1 = 2,
    VGA2 = 3,
};

enum class AudioVolumeName : uint8_t
{
    None = 0,
    Aud = 1,
    Sfx = 2,
    Both = 3,   // In which case, base36 audio is in .aud.
};

enum class MessageMapSource : uint8_t
{
    Included = 0,
    MessageMap = 1,
    AltResMap = 2,
};

enum class AudioMapVersion;

enum class NativeResolution : uint8_t
{
    Res320x200 = 0,
    Res640x400 = 1,
    Res640x480 = 2,
};

enum class KernelSet : uint8_t
{
    Provided = 0,   // e.g. in vocab.999
    SCI0SCI1 = 1,
    SCI2 = 2,
    SCI21 = 3,
};

size16 NativeResolutionToStoredSize(NativeResolution resolution);
NativeResolution StoredSizeToNativeResolution(size16 storedSize);

struct SCIVersion
{
    ResourceMapFormat MapFormat;
    ResourcePackageFormat PackageFormat;
    SoundFormat SoundFormat;
    CompressionFormat CompressionFormat;
    bool HasPalette;
    ViewFormat ViewFormat;
    PicFormat PicFormat;
    bool GrayScaleCursors;
    bool lofsaOpcodeIsAbsolute;
    bool SeparateHeapResources;
    uint16_t MainVocabResource;
    int DefaultVolumeFile;          // 0 or 1
    bool HasOldSCI0ScriptHeader;
    bool sci11Palettes;
    AudioVolumeName AudioVolumeName;
    bool SupportsMessages;
    MessageMapSource MessageMapSource;
    bool FontExtendedChars;
    bool HasSaidVocab;
    bool HasSyncResources;
    int AudioMapResourceNumber;
    AudioMapVersion MainAudioMapVersion;
    AudioMapVersion Base36AudioMapVersion;
    bool AudioIsWav;
    NativeResolution DefaultResolution;
    KernelSet Kernels;

    bool operator==(const SCIVersion &src);
    bool operator!=(const SCIVersion &src);

    bool IsExportWide() const
    {
        // SCI1 games had 32bit exports instead of 16bit.
        return lofsaOpcodeIsAbsolute && !SeparateHeapResources;
    }

    bool IsMapAppend() const
    {
        // In SCI0, we can append resources and have many versions of a single resource.
        // This is not possible in SCI1 due to the way the map is structured.
        return MapFormat < ResourceMapFormat::SCI1;
    }

    int GetMaximumResourceNumber() const
    {
        // REVIEW: Not sure about these numbers.
        return SeparateHeapResources ? 16384 : 999;
    }
};

// Detection possibilities:
// - resource map is easy. Once figured out, then test for palette? If so, then VGA
extern SCIVersion sciVersion0;
extern SCIVersion sciVersion1_Late;
extern SCIVersion sciVersion1_1;

bool IsVersionCompatible(ResourceType type, SCIVersion versionA, SCIVersion versionB);
