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
#include "Version.h"
#include "AudioMap.h"

// The actual version structure is determined by inspecting the resource map and other game files.
// The following "default" versions are just examples. They are currently used to associate with sample
// resources available to add to games.
SCIVersion sciVersion0 =
{
    ResourceMapFormat::SCI0,
    ResourcePackageFormat::SCI0,
    SoundFormat::SCI0,
    CompressionFormat::SCI0,
    false,
    ViewFormat::EGA,
    PicFormat::EGA,
    false,
    false,
    false,
    0,
    1,
    false,
    false,
    AudioVolumeName::None,
    false,
    MessageMapSource::Included,
    false,
    true,
    false,
    65535,
    AudioMapVersion::None,
    AudioMapVersion::None,
    false,
    NativeResolution::Res320x200,
    KernelSet::Provided,
};

// e.g. QFG2 EGA. Not checked for accuracy yet. Only used in pic unit tests.
SCIVersion sciVersion1_EarlyEGA =
{
    ResourceMapFormat::SCI0,
    ResourcePackageFormat::SCI0,
    SoundFormat::SCI0,
    CompressionFormat::SCI1,
    true,
    ViewFormat::EGA,
    PicFormat::EGA,
    false,
    true,
    false,
    900,
    0,
    false,
    false,
    AudioVolumeName::None,
    false,
    MessageMapSource::Included,
    true,
    true,
    false,
    65535,
    AudioMapVersion::None,
    AudioMapVersion::None,
    false,
    NativeResolution::Res320x200,
    KernelSet::Provided,
};


SCIVersion sciVersion1_Early =
{
    ResourceMapFormat::SCI0,
    ResourcePackageFormat::SCI0,
    SoundFormat::SCI1,
    CompressionFormat::SCI1,
    true,
    ViewFormat::VGA1,
    PicFormat::VGA1,
    false,
    true,
    false,
    900,
    0,
    false,
    false,
    AudioVolumeName::None,
    false,
    MessageMapSource::Included,
    true,
    true,
    false,
    65535,
    AudioMapVersion::None,
    AudioMapVersion::None,
    false,
    NativeResolution::Res320x200,
    KernelSet::SCI0SCI1,
};

// Not tested, but used for Longbow pics in unittests,
SCIVersion sciVersion1_Mid =
{
    ResourceMapFormat::SCI1,
    ResourcePackageFormat::SCI1,
    SoundFormat::SCI1,
    CompressionFormat::SCI1,
    true,
    ViewFormat::VGA1,
    PicFormat::VGA1,
    false,
    true,
    false,
    900,
    0,
    false,
    false,
    AudioVolumeName::None,
    false,
    MessageMapSource::Included,
    true,
    true,
    false,
    65535,
    AudioMapVersion::None,
    AudioMapVersion::None,
    false,
    NativeResolution::Res320x200,
    KernelSet::SCI0SCI1,
};

// Note sure what game this is for (VGA1_1???)
SCIVersion sciVersion1_Late =
{
    ResourceMapFormat::SCI1,
    ResourcePackageFormat::SCI1,
    SoundFormat::SCI1,
    CompressionFormat::SCI1,
    true,
    ViewFormat::VGA1,
    PicFormat::VGA1_1,
    false,
    true,
    false,
    900,
    0,
    false,
    false,
    AudioVolumeName::None,
    false,
    MessageMapSource::Included,
    true,
    true,
    false,
    65535,
    AudioMapVersion::None,
    AudioMapVersion::None,
    false,
    NativeResolution::Res320x200,
    KernelSet::SCI0SCI1,
};

SCIVersion sciVersion1_1 =
{
    ResourceMapFormat::SCI11,
    ResourcePackageFormat::SCI11,
    SoundFormat::SCI1,
    CompressionFormat::SCI1,
    true,
    ViewFormat::VGA1_1,
    PicFormat::VGA1_1,
    false,
    true,
    true,
    900,
    0,
    false,
    true,
    AudioVolumeName::Aud,
    true,
    MessageMapSource::Included,
    true,
    false,
    false,
    65535,
    AudioMapVersion::FiveBytes,
    AudioMapVersion::SyncMapEarly,
    false,
    NativeResolution::Res320x200,
    KernelSet::SCI0SCI1,
};

SCIVersion sciVersion2 =
{
    ResourceMapFormat::SCI2,
    ResourcePackageFormat::SCI2,
    SoundFormat::SCI1,
    CompressionFormat::SCI1,
    true,
    ViewFormat::VGA2,
    PicFormat::VGA2,
    false,
    true,
    true,
    900,
    0,
    false,
    true,
    AudioVolumeName::Aud,
    true,
    MessageMapSource::Included,
    true,
    false,
    false,
    65535,
    AudioMapVersion::FiveBytes,
    AudioMapVersion::SyncMapLate,
    false,
    NativeResolution::Res640x480,
    KernelSet::SCI21,
};

bool SCIVersion::operator == (const SCIVersion &src) const
{
    return 0 == memcmp(this, &src, sizeof(*this));
}
bool SCIVersion::operator != (const SCIVersion &src) const
{
    return 0 != memcmp(this, &src, sizeof(*this));
}

bool IsVersionCompatible(ResourceType type, SCIVersion versionA, SCIVersion versionB)
{
    // If identical versions, then fine.
    if (versionA == versionB)
    {
        return true;
    }

    // But depending on type, we may be more lenient.
    switch (type)
    {
        case ResourceType::View:
            return versionA.ViewFormat == versionB.ViewFormat &&
                versionA.sci11Palettes == versionB.sci11Palettes;

        case ResourceType::Pic:
            return versionA.PicFormat == versionB.PicFormat &&
                versionA.sci11Palettes == versionB.sci11Palettes;

        case ResourceType::Palette:
            return versionA.sci11Palettes == versionB.sci11Palettes;

        case ResourceType::Font:
            return versionA.FontExtendedChars == versionB.FontExtendedChars;

        case ResourceType::Cursor:
            return versionA.ViewFormat == versionB.ViewFormat;
    }
    return false;
}


size16 NativeResolutionToStoredSize(NativeResolution resolution)
{
    if (resolution == NativeResolution::Res640x480)
    {
        return size16(640, 480);
    }
    else if (resolution == NativeResolution::Res640x400)
    {
        return size16(640, 400);
    }
    return size16(0, 0);    // Default
}
NativeResolution StoredSizeToNativeResolution(size16 storedSize)
{
    return storedSize.cx == 640 ?
        (storedSize.cy == 480 ? NativeResolution::Res640x480 : NativeResolution::Res640x400) :
        NativeResolution::Res320x200;
}