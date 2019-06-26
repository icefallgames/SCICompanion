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

class ResourceContainer;
enum class ResourceEnumFlags : uint16_t;
enum class ResourceSaveLocation : uint16_t;
enum class ResourceSourceFlags : int;

// Encapsulates operations that only depend on gamefolder and SCI version.
// We can carry this to a background thread, for instace, and not have to
// interact with the ResourceMap object.

class ResourceRecency;
class ResourceBlob;

extern const std::string GameSection;
extern const std::string LanguageKey;
extern const std::string LanguageValueStudio;
extern const std::string LanguageValueSCI;

enum class ResourceSaveLocation : uint16_t
{
    Default,
    Package,
    Patch,
};

class GameFolderHelper
{
public:
    GameFolderHelper() {}
    GameFolderHelper(const GameFolderHelper &orig) = default;
    GameFolderHelper &operator=(const GameFolderHelper &other) = default;

    std::string GetScriptFileName(const std::string &name) const;
    std::string GetScriptFileName(uint16_t wScript) const;
    std::string GetScriptObjectFileName(const std::string &title) const;
    std::string GetScriptObjectFileName(uint16_t wScript) const;
    std::string GetScriptDebugFileName(uint16_t wScript) const;
    std::string GetScriptSymbolFileName(uint16_t wScript) const;
    std::string GameFolderHelper::GetViewScriptFilename(int viewNumber) const;
    std::string GameFolderHelper::GetSrcFolder(const std::string *prefix = nullptr) const;
    std::string GameFolderHelper::GetChaiFolder(const std::string *prefix = nullptr) const;
    std::string GameFolderHelper::GetMsgFolder(const std::string *prefix = nullptr) const;
    std::string GameFolderHelper::GetPicClipsFolder() const;
    std::string GameFolderHelper::GetSubFolder(const std::string &subFolder) const;
    std::string GameFolderHelper::GetLipSyncFolder() const;
    std::string GameFolderHelper::GetPolyFolder(const std::string *prefix = nullptr) const;
    std::string GetGameIniFileName() const;
    std::string GetIniString(const std::string &sectionName, const std::string &keyName, PCSTR pszDefault = "") const;
    bool GetIniBool(const std::string &sectionName, const std::string &keyName, bool value = false) const;
    bool DoesSectionExistWithEntries(const std::string &sectionName);
    static std::string GetIncludeFolder();
    static std::string GetHelpFolder();
    void SetIniString(const std::string &sectionName, const std::string &keyName, const std::string &value) const;
    LangSyntax GetDefaultGameLanguage() const { return Language; }
    ScriptId GetScriptId(const std::string &name) const;
    std::string FigureOutName(ResourceType type, int iResourceNum, uint32_t base36Number) const;
    std::unique_ptr<ResourceContainer> Resources(ResourceTypeFlags types, ResourceEnumFlags enumFlags, ResourceRecency *pRecency = nullptr, int mapContext = -1) const;
    std::unique_ptr<ResourceBlob> GameFolderHelper::MostRecentResource(ResourceType type, int number, ResourceEnumFlags flags, uint32_t base36Number = NoBase36, int mapContext = -1) const;
    bool DoesResourceExist(ResourceType type, int number, std::string *retrieveName, ResourceSaveLocation location) const;

    bool GetUseSierraAspectRatio(bool defaultValue) const;
    void SetUseSierraAspectRatio(bool useSierra) const;
	bool GetUndither() const;
	void SetUndither(bool undither) const;
    void SetFixedVGAPalette(bool vgaView) const;

    bool GetGenerateDebugInfo() const;
    bool GetFixedVGAPalette() const;

    ResourceSaveLocation GetResourceSaveLocation(ResourceSaveLocation location) const;
    void SetResourceSaveLocation(ResourceSaveLocation location) const;
    ResourceEnumFlags GetDefaultEnumFlags() const;
    ResourceSourceFlags GetDefaultSaveSourceFlags() const;

    bool IsResourceCompatible(const ResourceBlob &resource) const;

    // Members
    SCIVersion Version;
    std::string GameFolder;
    LangSyntax Language;

private:
    std::string _GetSubfolder(const char *key, const std::string *prefix = nullptr) const;
};

