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

// Encapsulates operations that only depend on gamefolder and SCI version.
// We can carry this to a background thread, for instace, and not have to
// interact with the ResourceMap object.

class ResourceRecency;
class ResourceBlob;

extern const std::string GameSection;
extern const std::string LanguageKey;
extern const std::string LanguageValueStudio;
extern const std::string LanguageValueSCI;

class GameFolderHelper
{
public:
    GameFolderHelper() {}
    GameFolderHelper(const GameFolderHelper &orig) = default;
    GameFolderHelper &operator=(const GameFolderHelper &other) = default;

    std::string GetScriptFileName(const std::string &name) const;
    std::string GetScriptFileName(WORD wScript) const;
    std::string GetScriptObjectFileName(const std::string &title) const;
    std::string GetScriptObjectFileName(WORD wScript) const;
    std::string GameFolderHelper::GetSrcFolder(const std::string *prefix = nullptr) const;
    std::string GameFolderHelper::GetMsgFolder(const std::string *prefix = nullptr) const;
    std::string GameFolderHelper::GetSubFolder(const std::string &subFolder) const;
    std::string GameFolderHelper::GetLipSyncFolder() const;
    std::string GameFolderHelper::GetPolyFolder(const std::string *prefix = nullptr) const;
    std::string GetGameIniFileName() const;
    std::string GetIniString(const std::string &sectionName, const std::string &keyName, PCSTR pszDefault = "") const;
    std::string GetIncludeFolder() const;
    void SetIniString(const std::string &sectionName, const std::string &keyName, const std::string &value) const;
    LangSyntax GetDefaultGameLanguage() const { return Language; }
    ScriptId GetScriptId(const std::string &name) const;
    std::string FigureOutName(ResourceType type, int iResourceNum, uint32_t base36Number) const;
    std::unique_ptr<ResourceContainer> Resources(ResourceTypeFlags types, ResourceEnumFlags enumFlags, ResourceRecency *pRecency = nullptr, int mapContext = -1) const;
    std::unique_ptr<ResourceBlob> GameFolderHelper::MostRecentResource(ResourceType type, int number, ResourceEnumFlags flags, uint32_t base36Number = NoBase36, int mapContext = -1) const;

    // Members
    SCIVersion Version;
    std::string GameFolder;
    LangSyntax Language;

private:
    std::string _GetSubfolder(const char *key, const std::string *prefix = nullptr) const;
};

