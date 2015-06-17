#include "stdafx.h"
#include "GameFolderHelper.h"
#include "ResourceBlob.h"
#include "ResourceMapOperations.h"

using namespace std;

// Returns "n004" for input of 4
std::string default_reskey(int iNumber)
{
    stringstream ss;
    ss << "n" << setfill('0') << setw(3) << iNumber;
    return ss.str();
}

std::string GameFolderHelper::GetScriptFileName(WORD wScript, LangSyntax lang) const
{
    std::string filename;
    std::string scriptTitle = GetIniString("Script", default_reskey(wScript), default_reskey(wScript).c_str());
    if (!scriptTitle.empty())
    {
        filename = GetScriptFileName(scriptTitle, lang);
    }
    return filename;
}

//
// Given something like "main", returns "c:\foobar\mygame\src\main.sc"
//
std::string GameFolderHelper::GetScriptFileName(const std::string &name, LangSyntax lang) const
{
    string filename = this->GameFolder;
    filename += "\\src\\";
    filename += name;
    if (lang == LangSyntaxUnknown)
    {
        lang = (GetIniString("Language", name, "sc") == "scp") ? LangSyntaxCpp : LangSyntaxSCIStudio;
    }
    // Figure out what language the script is in (default is .sc), and append the default extension
    if (lang == LangSyntaxCpp)
    {
        filename += ".scp";
    }
    else
    {
        filename += ".sc";
    }
    return filename;
}

std::string GameFolderHelper::GetScriptObjectFileName(const std::string &title, LangSyntax lang) const
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

std::string GameFolderHelper::GetScriptObjectFileName(WORD wScript, LangSyntax lang) const
{
    std::string filename;
    std::string scriptTitle = GetIniString("Script", default_reskey(wScript), default_reskey(wScript).c_str());
    if (!scriptTitle.empty())
    {
        filename = GetScriptObjectFileName(scriptTitle, lang);
    }
    return filename;
}

std::string GameFolderHelper::GetIncludeFolder() const
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
void GameFolderHelper::SetIniString(const std::string &sectionName, const std::string &keyName, const std::string &value) const
{
    WritePrivateProfileString(sectionName.c_str(), keyName.c_str(), value.c_str(), GetGameIniFileName().c_str());
}

std::string GameFolderHelper::GetGameIniFileName() const
{
    string filename = this->GameFolder;
    filename += "\\game.ini";
    return filename;
}

std::string GameFolderHelper::GetSrcFolder() const
{
    std::string srcFolder = this->GameFolder;
    if (!srcFolder.empty())
    {
        srcFolder += "\\src";
    }
    return srcFolder;
}

std::string GameFolderHelper::GetMsgFolder() const
{
    std::string srcFolder = this->GameFolder;
    if (!srcFolder.empty())
    {
        srcFolder += "\\msg";
    }
    return srcFolder;
}

std::string GameFolderHelper::GetPolyFolder() const
{
    std::string srcFolder = this->GameFolder;
    if (!srcFolder.empty())
    {
        srcFolder += "\\poly";
    }
    return srcFolder;
}

//
// Returns the script identifier for something "main", or "rm001".
//
ScriptId GameFolderHelper::GetScriptId(const std::string &name) const
{
    return ScriptId(GetScriptFileName(name).c_str());
}


//
// Perf: we're opening and closing the file each time.  We could do this once.
//
std::string GameFolderHelper::FigureOutName(ResourceType type, int iNumber) const
{
    std::string name;
    if ((size_t)type < ARRAYSIZE(g_resourceInfo))
    {
        std::string keyName = default_reskey(iNumber);
        name = GetIniString(GetResourceInfo(type).pszTitleDefault, keyName, keyName.c_str());
    }
    return name;
}

std::unique_ptr<ResourceContainer> GameFolderHelper::Resources(ResourceTypeFlags types, ResourceEnumFlags enumFlags, ResourceRecency *pRecency) const
{
    // Resources can come from various sources.
    std::unique_ptr<ResourceSourceArray> mapAndVolumes = std::make_unique<ResourceSourceArray>();

    if (!GameFolder.empty())
    {
        // First, any stray files...
        if (!IsFlagSet(enumFlags, ResourceEnumFlags::ExcludePatchFiles))
        {
            mapAndVolumes->push_back(move(std::make_unique<PatchFilesResourceSource>(Version, GameFolder)));
        }

        // Add readers for message map files, if requrested
        if (IsFlagSet(types, ResourceTypeFlags::Message) && Version.SeparateMessageMap)
        {
            FileDescriptorMessageMap messageMap(GameFolder);
            if (messageMap.DoesMapExist())
            {
                mapAndVolumes->push_back(move(CreateResourceSource(GameFolder, Version, ResourceSourceFlags::MessageMap)));
            }
        }
        
        if (IsFlagSet(types, ResourceTypeFlags::Audio))
        {
            mapAndVolumes->push_back(move(make_unique<AudioResourceSource>(Version, GameFolder)));
        }

        // Now the standard resource maps
        mapAndVolumes->push_back(move(CreateResourceSource(GameFolder, Version, ResourceSourceFlags::ResourceMap)));
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

std::unique_ptr<ResourceBlob> GameFolderHelper::MostRecentResource(ResourceType type, int number, ResourceEnumFlags flags) const
{
    std::unique_ptr<ResourceBlob> returnBlob;
    ResourceEnumFlags enumFlags = flags | ResourceEnumFlags::MostRecentOnly;
    auto &resourceContainer = Resources(ResourceTypeToFlag(type), enumFlags);
    for (auto &blobIt = resourceContainer->begin(); blobIt != resourceContainer->end(); ++blobIt)
    {
        if (blobIt.GetResourceNumber() == number)
        {
            returnBlob = move(*blobIt);
            break;
        }
    }
    return returnBlob;
}
