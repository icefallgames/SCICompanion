#pragma once

class ResourceContainer;
enum class ResourceEnumFlags : uint16_t;

// Encapsulates operations that only depend on gamefolder and SCI version.
// We can carry this to a background thread, for instace, and not have to
// interact with the ResourceMap object.

// Returns "n004" for 4.
std::string default_reskey(int iNumber, uint32_t base36Number);

class ResourceRecency;

class GameFolderHelper
{
public:
    GameFolderHelper() {}
    GameFolderHelper(const GameFolderHelper &orig) = default;
    GameFolderHelper &operator=(const GameFolderHelper &other) = default;

    std::string GetScriptFileName(const std::string &name, LangSyntax lang = LangSyntaxUnknown) const;
    std::string GetScriptFileName(WORD wScript, LangSyntax lang = LangSyntaxUnknown) const;
    std::string GetScriptObjectFileName(const std::string &title, LangSyntax lang = LangSyntaxUnknown) const;
    std::string GetScriptObjectFileName(WORD wScript, LangSyntax lang = LangSyntaxUnknown) const;
    std::string GameFolderHelper::GetSrcFolder() const;
    std::string GameFolderHelper::GetMsgFolder() const;
    std::string GameFolderHelper::GetLipSyncFolder() const;
    std::string GameFolderHelper::GetPolyFolder() const;
    std::string GetGameIniFileName() const;
    std::string GetIniString(const std::string &sectionName, const std::string &keyName, PCSTR pszDefault = "") const;
    std::string GetIncludeFolder() const;
    void SetIniString(const std::string &sectionName, const std::string &keyName, const std::string &value) const;
    LangSyntax GetGameLanguage() const { return Language; }
    ScriptId GetScriptId(const std::string &name) const;
    std::string FigureOutName(ResourceType type, int iResourceNum, uint32_t base36Number) const;
    std::unique_ptr<ResourceContainer> Resources(ResourceTypeFlags types, ResourceEnumFlags enumFlags, ResourceRecency *pRecency = nullptr, int mapContext = -1) const;
    std::unique_ptr<ResourceBlob> GameFolderHelper::MostRecentResource(ResourceType type, int number, ResourceEnumFlags flags, uint32_t base36Number = NoBase36, int mapContext = -1) const;

    // Members
    SCIVersion Version;
    std::string GameFolder;
    LangSyntax Language;
};

