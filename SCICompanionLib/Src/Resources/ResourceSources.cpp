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
#include "ResourceSources.h"
#include "format.h"
#include "ResourceEntity.h"
#include "ResourceContainer.h"
#include <limits>
#include "ResourceBlob.h"

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
SourceTraits altMapSourceTraits =
{
    "altres.map",
    "altres.{:03d}"
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

bool IsResourceCompatible(const SCIVersion &usVersion, const SCIVersion &resourceVersion, ResourceType type)
{
    if (resourceVersion == usVersion)
    {
        return true;
    }

    if (type == ResourceType::View)
    {
        return resourceVersion.ViewFormat == usVersion.ViewFormat;
    }

    if (type == ResourceType::Pic)
    {
        return resourceVersion.PicFormat == usVersion.PicFormat;
    }

    if (type == ResourceType::Palette)
    {
        return usVersion.HasPalette;
    }

    if (type == ResourceType::Sound)
    {
        return resourceVersion.SoundFormat == usVersion.SoundFormat;
    }

    if (type == ResourceType::Script)
    {
        return resourceVersion.lofsaOpcodeIsAbsolute == usVersion.lofsaOpcodeIsAbsolute;
    }

    return true; // I guess?
}

bool IsResourceCompatible(const SCIVersion &version, const ResourceBlob &blob)
{
    return IsResourceCompatible(version, blob.GetVersion(), blob.GetType());
}

