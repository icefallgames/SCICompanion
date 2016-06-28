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
#include "ResourceContainer.h" 
#include "GameFolderHelper.h"
#include "ResourceBlob.h"
#include "ResourceMapOperations.h"
#include "format.h"
#include "ResourceEntity.h"
#include "AudioResourceSource.h"
#include "AudioCacheResourceSource.h"
#include "PatchResourceSource.h"

using namespace std;

const std::string GameSection = "Game";
const std::string LanguageKey = "Language";
const std::string LanguageValueStudio = "sc";
const std::string LanguageValueSCI = "sci";
const std::string AspectRatioKey = "UseSierraAspectRatio";
const std::string PatchFileKey = "SaveToPatchFiles";
const std::string TrueValue = "true";
const std::string FalseValue = "false";

// Returns "n004" for input of 4
std::string default_reskey(int iNumber, uint32_t base36Number)
{
    if (base36Number == NoBase36)
    {
        return fmt::format("n{0:0>3}", iNumber);
    }
    else
    {
        // NNNNNVV.CCS
        return fmt::format("{0:0>3t}{1:0>2t}{2:0>2t}.{3:0>2t}{4:0>1t}",
            iNumber,
            (base36Number >> 8) & 0xff,
            (base36Number >> 16) & 0xff,
            (base36Number >> 0) & 0xff,
            ((base36Number >> 24) & 0xff) % 36      // If this number (sequence) is 36, we want it to wrap 0 zero. Seq 0 not allowed.
            );
    }
}

std::string GameFolderHelper::GetScriptFileName(WORD wScript) const
{
    std::string filename;
    std::string scriptTitle = GetIniString("Script", default_reskey(wScript, NoBase36), default_reskey(wScript, NoBase36).c_str());
    if (!scriptTitle.empty())
    {
        filename = GetScriptFileName(scriptTitle);
    }
    return filename;
}

//
// Given something like "main", returns "c:\foobar\mygame\src\main.sc"
//
std::string GameFolderHelper::GetScriptFileName(const std::string &name) const
{
    string filename = this->GameFolder;
    filename += "\\src\\";
    filename += name;
    // Append the default extension
    filename += ".sc";
    return filename;
}

std::string GameFolderHelper::GetScriptObjectFileName(const std::string &title) const
{
    std::string filename = this->GameFolder;
    if (!filename.empty())
    {
        filename += "\\src\\";
        filename += title;
        filename += ".sco";
    }
    return filename;
}

std::string GameFolderHelper::GetScriptObjectFileName(WORD wScript) const
{
    std::string filename;
    std::string scriptTitle = GetIniString("Script", default_reskey(wScript, NoBase36), default_reskey(wScript, NoBase36).c_str());
    if (!scriptTitle.empty())
    {
        filename = GetScriptObjectFileName(scriptTitle);
    }
    return filename;
}

std::string GameFolderHelper::GetIncludeFolder()
{
    string includeFolder;
    // The normal case...
    TCHAR szPath[MAX_PATH];
    if (GetModuleFileName(nullptr, szPath, ARRAYSIZE(szPath)))
    {
        PSTR pszFileName = PathFindFileName(szPath);
        *pszFileName = 0; // null it
        includeFolder += szPath;
    }
    includeFolder += "include";
    return includeFolder;
}

std::string GameFolderHelper::GetHelpFolder()
{
    string helpFolder;
    TCHAR szPath[MAX_PATH];
    if (GetModuleFileName(nullptr, szPath, ARRAYSIZE(szPath)))
    {
        PSTR pszFileName = PathFindFileName(szPath);
        *pszFileName = 0; // null it
        helpFolder += szPath;
    }
    helpFolder += "Help";
    return helpFolder;
}


//
// Returns an empty string (or pszDefault) if there is no key
//
std::string GameFolderHelper::GetIniString(const std::string &sectionName, const std::string &keyName, PCSTR pszDefault) const
{
    std::string strRet;
    char sz[200];
    if (GetPrivateProfileString(sectionName.c_str(), keyName.c_str(), nullptr, sz, (DWORD)ARRAYSIZE(sz), GetGameIniFileName().c_str()))
    {
        strRet = sz;
    }
    else
    {
        strRet = pszDefault;
    }
    return strRet;
}
bool GameFolderHelper::DoesSectionExistWithEntries(const std::string &sectionName)
{
    char sz[200];
    return (GetPrivateProfileSection(sectionName.c_str(), sz, (DWORD)ARRAYSIZE(sz), GetGameIniFileName().c_str()) > 0);
}
void GameFolderHelper::SetIniString(const std::string &sectionName, const std::string &keyName, const std::string &value) const
{
    WritePrivateProfileString(sectionName.c_str(), keyName.c_str(), value.empty() ? nullptr : value.c_str(), GetGameIniFileName().c_str());
}


std::string GameFolderHelper::GetGameIniFileName() const
{
    string filename = this->GameFolder;
    filename += "\\game.ini";
    return filename;
}

std::string GameFolderHelper::GetSubFolder(const std::string &subFolder) const
{
    return _GetSubfolder(subFolder.c_str());
}

std::string GameFolderHelper::_GetSubfolder(const char *key, const std::string *prefix) const
{
    std::string srcFolder = this->GameFolder;
    if (!srcFolder.empty())
    {
        srcFolder += "\\";
        if (prefix)
        {
            srcFolder += *prefix;
            srcFolder += "\\";
        }
        srcFolder += key;
    }
    return srcFolder;
}

std::string GameFolderHelper::GetSrcFolder(const std::string *prefix) const
{
    return _GetSubfolder("src", prefix);
}

std::string GameFolderHelper::GetLipSyncFolder() const
{
    return _GetSubfolder("lipsync");
}

std::string GameFolderHelper::GetMsgFolder(const std::string *prefix) const
{
    return _GetSubfolder("msg", prefix);
}

std::string GameFolderHelper::GetPolyFolder(const std::string *prefix) const
{
    return _GetSubfolder("poly", prefix);
}

//
// Returns the script identifier for something "main", or "rm001".
//
ScriptId GameFolderHelper::GetScriptId(const std::string &name) const
{
    return ScriptId(GetScriptFileName(name).c_str());
}

bool GameFolderHelper::GetUseSierraAspectRatio(bool defaultValue) const
{
    std::string value = GetIniString(GameSection, AspectRatioKey, defaultValue ? TrueValue.c_str() : FalseValue.c_str());
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return value == TrueValue;
}
void GameFolderHelper::SetUseSierraAspectRatio(bool useSierra) const
{
    SetIniString(GameSection, AspectRatioKey, useSierra ? TrueValue : FalseValue);
}

ResourceSaveLocation GameFolderHelper::GetResourceSaveLocation(ResourceSaveLocation location) const
{
    if (location == ResourceSaveLocation::Default)
    {
        std::string value = GetIniString(GameSection, PatchFileKey, FalseValue.c_str());
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        return (value == TrueValue) ? ResourceSaveLocation::Patch : ResourceSaveLocation::Package;
    }
    return location;
}

void GameFolderHelper::SetResourceSaveLocation(ResourceSaveLocation location) const
{
    SetIniString(GameSection, PatchFileKey, (location == ResourceSaveLocation::Patch) ? TrueValue : FalseValue);
}

ResourceEnumFlags GameFolderHelper::GetDefaultEnumFlags() const
{
    ResourceSaveLocation saveLocation = GetResourceSaveLocation(ResourceSaveLocation::Default);
    return (saveLocation == ResourceSaveLocation::Patch) ? ResourceEnumFlags::ExcludePackagedFiles : ResourceEnumFlags::None;
}

ResourceSourceFlags GameFolderHelper::GetDefaultSaveSourceFlags() const
{
    ResourceSaveLocation saveLocation = GetResourceSaveLocation(ResourceSaveLocation::Default);
    return (saveLocation == ResourceSaveLocation::Patch) ? ResourceSourceFlags::PatchFile : ResourceSourceFlags::ResourceMap;
}

//
// Perf: we're opening and closing the file each time.  We could do this once.
//
std::string GameFolderHelper::FigureOutName(ResourceType type, int iNumber, uint32_t base36Number) const
{
    std::string name;
    if ((size_t)type < ARRAYSIZE(g_resourceInfo))
    {
        std::string keyName = default_reskey(iNumber, base36Number);
        name = GetIniString(GetResourceInfo(type).pszTitleDefault, keyName, keyName.c_str());
    }
    return name;
}

std::unique_ptr<ResourceContainer> GameFolderHelper::Resources(ResourceTypeFlags types, ResourceEnumFlags enumFlags, ResourceRecency *pRecency, int mapContext) const
{
    if (IsFlagSet(enumFlags, ResourceEnumFlags::AddInDefaultEnumFlags))
    {
        enumFlags |= GetDefaultEnumFlags();
    }

    // If audio or sync resources are requested, we can't also have maps.
    if (IsFlagSet(types, ResourceTypeFlags::Audio))
    {
        ClearFlag(types, ResourceTypeFlags::AudioMap);
    }

    // Resources can come from various sources.
    std::unique_ptr<ResourceSourceArray> mapAndVolumes = std::make_unique<ResourceSourceArray>();

    if (!GameFolder.empty())
    {
        if (IsFlagSet(enumFlags, ResourceEnumFlags::IncludeCacheFiles))
        {
            // Our audio cache files take precedence
            if (IsFlagSet(types, ResourceTypeFlags::Audio))
            {
                mapAndVolumes->push_back(move(make_unique<AudioCacheResourceSource>(*this, mapContext, ResourceSourceAccessFlags::Read)));
            }

            // Audiomaps can come from the cache files folder too... but we can re-use PatchFilesResourceSource for this
            if (IsFlagSet(types, ResourceTypeFlags::AudioMap))
            {
                mapAndVolumes->push_back(move(std::make_unique<PatchFilesResourceSource>(Version, GameFolder + pszAudioCacheFolder, ResourceSourceFlags::AudioMapCache)));
            }
        }

        // First, any stray files...
        if (!IsFlagSet(enumFlags, ResourceEnumFlags::ExcludePatchFiles) && (mapContext == -1))
        {
            mapAndVolumes->push_back(move(std::make_unique<PatchFilesResourceSource>(Version, GameFolder, ResourceSourceFlags::PatchFile)));
        }

        // Add readers for message map files, if requested
        if (IsFlagSet(types, ResourceTypeFlags::Message) && !IsFlagSet(enumFlags, ResourceEnumFlags::ExcludePackagedFiles) && (mapContext == -1))
        {
            FileDescriptorBase *fd = nullptr;
            FileDescriptorMessageMap messageMap(GameFolder);
            FileDescriptorAltMap altMap(GameFolder);
            if (Version.MessageMapSource == MessageMapSource::MessageMap)
            {
                fd = &messageMap;
            }
            else if (Version.MessageMapSource == MessageMapSource::AltResMap)
            {
                fd = &altMap;
            }
            if (fd && fd->DoesMapExist())
            {
                mapAndVolumes->push_back(move(CreateResourceSource(*this, fd->SourceFlags)));
            }
        }
        
        if (IsFlagSet(types, ResourceTypeFlags::Audio) && !IsFlagSet(enumFlags, ResourceEnumFlags::ExcludePackagedFiles))
        {
            mapAndVolumes->push_back(move(make_unique<AudioResourceSource>(*this, mapContext, ResourceSourceAccessFlags::Read)));
        }

        // Now the standard resource maps
        if (!IsFlagSet(enumFlags, ResourceEnumFlags::ExcludePackagedFiles) && (mapContext == -1))
        {
            mapAndVolumes->push_back(move(CreateResourceSource(*this, ResourceSourceFlags::ResourceMap)));
        }
    }

    std::unique_ptr<ResourceContainer> resourceContainer(
        new ResourceContainer(
        GameFolder,
        move(mapAndVolumes),
        types,
        enumFlags,
        pRecency)
        );

    return resourceContainer;
}

std::unique_ptr<ResourceBlob> GameFolderHelper::MostRecentResource(ResourceType type, int number, ResourceEnumFlags flags, uint32_t base36Number, int mapContext) const
{
    std::unique_ptr<ResourceBlob> returnBlob;
    ResourceEnumFlags enumFlags = flags | ResourceEnumFlags::MostRecentOnly;
    auto &resourceContainer = Resources(ResourceTypeToFlag(type), enumFlags, nullptr, mapContext);
    for (auto &blobIt = resourceContainer->begin(); blobIt != resourceContainer->end(); ++blobIt)
    {
        if ((blobIt.GetResourceNumber() == number) && (blobIt.GetResourceHeader().Base36Number == base36Number))
        {
            returnBlob = move(*blobIt);
            break;
        }
    }
    return returnBlob;
}

