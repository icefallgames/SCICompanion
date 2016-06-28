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

enum class ResourceSourceFlags
{
    Invalid = 0,
    MessageMap = 0x0001,
    PatchFile = 0x0002,
    Aud = 0x0004,
    Sfx = 0x0008,
    AudioCache = 0x0010,    // Our special audio folder
    AudioMapCache = 0x0020, // 
    AltMap = 0x0040,
    ResourceMap = 0x0080,
};

enum class ResourceLoadStatusFlags : uint8_t
{
    None = 0x00,
    DecompressionFailed = 0x01,
    ResourceCreationFailed = 0x02,
    Corrupted = 0x04,
    Delayed = 0x08,
};

enum class BlobKey
{
    LipSyncDataSize,
};

DEFINE_ENUM_FLAGS(ResourceLoadStatusFlags, uint8_t)