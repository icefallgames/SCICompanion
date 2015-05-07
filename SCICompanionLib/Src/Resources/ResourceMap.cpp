
#include "stdafx.h"
#include "ResourceMap.h"
#include "AppState.h"
#include "ResourceRecency.h"
#include "SaveResourceDialog.h"
#include "RemoveScriptDialog.h"
#include "ClassBrowser.h"
#include "View.h"
#include "Cursor.h"
#include "Font.h"
#include "Pic.h"
#include "PicOperations.h"
#include "Text.h"
#include "Sound.h"
#include "Vocab000.h"
#include "PaletteOperations.h"
#include "Message.h"
#include "Audio.h"
#include "AudioMap.h"
#include "ResourceEntity.h"
#include "ResourceSources.h"
#include "CompiledScript.h"
#include "Disassembler.h"
#include "ResourceMapOperations.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HRESULT copyfile(const string &destination, const string &source)
{
    return CopyFile(destination.c_str(), source.c_str(), FALSE) ? S_OK : ResultFromLastError();
}

void deletefile(const string &filename)
{
    if (PathFileExists(filename.c_str()))
    {
        if (!DeleteFile(filename.c_str()))
        {
            std::string details = "Deleting ";
            details += filename;
            throw std::exception(GetMessageFromLastError(details).c_str());
        }
    }
}

HRESULT SetFilePositionHelper(HANDLE hFile, DWORD dwPos)
{
    HRESULT hr;
    if (INVALID_SET_FILE_POINTER != SetFilePointer(hFile, dwPos, nullptr, FILE_BEGIN))
    {
        hr = S_OK;
    }
    else
    {
        hr = ResultFromLastError();
    }
    return hr;
}



HRESULT WriteResourceMapTerminatingBits(HANDLE hFileMap)
{
    HRESULT hr;
    // Write the terminating bits.
    DWORD cbWritten;
    RESOURCEMAPENTRY_SCI0 entryTerm;
    memset(&entryTerm, 0xff, sizeof(entryTerm));
    if (WriteFile(hFileMap, &entryTerm, sizeof(entryTerm), &cbWritten, nullptr) && (cbWritten == sizeof(entryTerm)))
    {
        hr = S_OK;
    }
    else
    {
        hr = ResultFromLastError();
    }
    return hr;
}

HRESULT TestForReadOnly(const string &filename)
{
    HRESULT hr = S_OK;
    DWORD dwAttribs = GetFileAttributes(filename.c_str());
    if (INVALID_FILE_ATTRIBUTES == dwAttribs)
    {
        hr = ResultFromLastError();
    }
    else if (dwAttribs & FILE_ATTRIBUTE_READONLY)
    {
        hr = HRESULT_FROM_WIN32(ERROR_FILE_READ_ONLY);
    }
    return hr;
}

HRESULT TestDelete(const string &filename)
{
    HRESULT hr = S_OK;
    HANDLE hFile = CreateFile(filename.c_str(), DELETE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
    }
    else
    {
        hr = ResultFromLastError();
    }
    return hr;
}

bool IsValidPackageNumber(int iPackageNumber)
{
    return (iPackageNumber >= 0) && (iPackageNumber < 63);
}


ResourceTypeFlags ResourceTypeToFlag(ResourceType dwType)
{
    return (ResourceTypeFlags)(1 << (int)dwType);
}

ResourceType ResourceFlagToType(ResourceTypeFlags dwFlags)
{
    uint32_t dwType = (uint32_t)dwFlags;
    int iShifts = 0;
    while (dwType > 1)
    {
        dwType = dwType >> 1;
        iShifts++;
    }
    return (ResourceType)iShifts;
}

template<typename _TMapEntry, typename _THeaderEntry>
HRESULT RebuildResources(SCIVersion version, BOOL fShowUI)
{
    // Enumerate resources and write the ones we have not already encountered.
    std::unique_ptr<ResourceSource> resourceSource = CreateResourceSource(appState->GetResourceMap().GetGameFolder(), appState->GetVersion(), ResourceSourceFlags::ResourceMap);
    try
    {
        resourceSource->RebuildResources();
    }
    catch (std::exception &e)
    {
        AfxMessageBox(e.what(), MB_OK | MB_ICONWARNING);
    }

    // TODO: Also support rebuilding message map, it is simple. But how do we know if we have them?
    return S_OK;
}

//
// CResourceMap
// Helper class for managing resources.
//
CResourceMap::CResourceMap()
{
    _paletteListNeedsUpdate = true;
    _skipVersionSniffOnce = false;
    _pVocab000 = nullptr;
    _cDeferAppend = 0;
    _gameFolderHelper.Language = LangSyntaxUnknown;
    _gameFolderHelper.Version = sciVersion0;    // By default
	// This is a pointer because we don't want a dependency on it in the header file.
	_classBrowser = std::make_unique<SCIClassBrowser>();

    memset(_emptyPalette.Colors, 0, sizeof(_emptyPalette.Colors));
}

CResourceMap::~CResourceMap()
{
    ASSERT(_syncs.empty()); // They should remove themselves.
    _classBrowser->Reset();
    ASSERT(_cDeferAppend == 0);
}

//
// Adds the name of the resource to the game.ini file.
//
void CResourceMap::AssignName(const ResourceBlob &resource)
{
    // Assign the name of the item.
    std::string keyName = default_reskey(resource.GetNumber());
    std::string name = resource.GetName();
    if (!name.empty() && (0 != lstrcmpi(keyName.c_str(), name.c_str())))
    {
        Helper().SetIniString(g_resourceInfo[(int)resource.GetType()].pszTitleDefault, keyName, name);
    }
}

void CResourceMap::AssignName(ResourceType type, int iResourceNumber, PCTSTR pszName)
{
    // Assign the name of the item.
    std::string keyName = default_reskey(iResourceNumber);
    if (pszName && *pszName && (0 != lstrcmpi(keyName.c_str(), pszName)))
    {
        Helper().SetIniString(g_resourceInfo[(int)type].pszTitleDefault, keyName, pszName);
    }
}

void CResourceMap::BeginDeferAppend()
{
    ASSERT((_cDeferAppend > 0) || _deferredResources.empty());
    ++_cDeferAppend;
}
void CResourceMap::AbandonAppend()
{
    // Abandon all the resources we were piling up.
    if (_cDeferAppend)
    {
        --_cDeferAppend;
        if (_cDeferAppend == 0)
        {
            _deferredResources.clear();
        }
    }
}
HRESULT CResourceMap::EndDeferAppend()
{
    HRESULT hr = S_OK;
    if (_cDeferAppend)
    {
        --_cDeferAppend;
        if (_cDeferAppend == 0)
        {
            if (!_deferredResources.empty())
            {
                // TODO: Assert that all defereed resources come from the same source.
                // Or rather we could support from multiple sources. The defer append only happens during
                // "compile all". Which actually might eventually support auto-generating message resources, so this is
                // probably something we'll need to handle.

                ResourceSourceFlags sourceFlags = _deferredResources[0].GetSourceFlags();
                // Enumerate resources and write the ones we have not already encountered.
                std::unique_ptr<ResourceSource> resourceSource = CreateResourceSource(_gameFolderHelper.GameFolder, _gameFolderHelper.Version, sourceFlags);

                try
                {
                    resourceSource->AppendResources(_deferredResources);
                }
                catch (std::exception &e)
                {
                    AfxMessageBox(e.what(), MB_OK | MB_ICONWARNING);
                }

                bool reload[NumResourceTypes] = {};
                for (ResourceBlob &blob : _deferredResources)
                {
                    AssignName(blob);
                    reload[(int)blob.GetType()] = true;
                }

                _deferredResources.clear();

                // Tell the views that might have changed, to reload:
                for (int iType = 0; iType < ARRAYSIZE(reload); iType++)
                {
                    if (reload[iType])
                    {
                        NotifyToResourceResourceType((ResourceType)iType);
                    }
                }
            }
        }
    }
    return hr;
}

//
// Suggest an unused resource number for this type.
//
int CResourceMap::SuggestResourceNumber(ResourceType type)
{
    int iNumber = 0;
    // Figure out a number to suggest...
    vector<bool> rgNumbers(1000, false);

    auto resourceContainer = Resources(ResourceTypeToFlag(type), ResourceEnumFlags::MostRecentOnly);
    for (auto &blobIt = resourceContainer->begin(); blobIt != resourceContainer->end(); ++blobIt)
    {
        int iThisNumber = blobIt.GetResourceNumber();
        if (iThisNumber >= 0 && iThisNumber < (int)rgNumbers.size())
        {
            rgNumbers[iThisNumber] = true;
        }
    }

    // Find the first one that is still false
    // (iterators on c style arrays are pointers to those arrays)
    auto result = find(rgNumbers.begin(), rgNumbers.end(), false);
    iNumber = (int)(result - rgNumbers.begin());
    return iNumber;
}

bool CResourceMap::IsResourceCompatible(const ResourceBlob &resource)
{
    SCIVersion resourceVersion = resource.GetVersion();
    SCIVersion usVersion = GetSCIVersion();
    if (resourceVersion == usVersion)
    {
        return true;
    }

    if (resource.GetType() == ResourceType::View)
    {
        return resourceVersion.ViewFormat == usVersion.ViewFormat;
    }

    if (resource.GetType() == ResourceType::Pic)
    {
        return resourceVersion.PicFormat == usVersion.PicFormat;
    }

    if (resource.GetType() == ResourceType::Palette)
    {
        return usVersion.HasPalette;
    }

    if (resource.GetType() == ResourceType::Sound)
    {
        return resourceVersion.SoundFormat == usVersion.SoundFormat;
    }

    if (resource.GetType() == ResourceType::Script)
    {
        return resourceVersion.lofsaOpcodeIsAbsolute == usVersion.lofsaOpcodeIsAbsolute;
    }

    return true; // I guess?
}

void CResourceMap::AppendResourceAskForNumber(ResourceEntity &resource)
{
    // Invoke dialog to suggest/ask for a resource number
    SaveResourceDialog srd;
    srd.Init(-1, SuggestResourceNumber(resource.GetType()));
    if (IDOK == srd.DoModal())
    {
        // Assign it.
        resource.ResourceNumber = srd.GetResourceNumber();
        resource.PackageNumber = srd.GetPackageNumber();
        AppendResource(resource);
    }
}

//
// Ask the user where to save the resource... and then save it.
//
HRESULT CResourceMap::AppendResourceAskForNumber(ResourceBlob &resource)
{
    if (resource.GetVersion() != GetSCIVersion())
    {
        if (IDNO == AfxMessageBox("The version of the resource being added does not match the version of the game.\nAdding it might cause the game to be corrupted.\nDo you want to go ahead anyway?", MB_YESNO))
        {
            return E_FAIL;
        }
    }
    // Invoke dialog to suggest/ask for a resource number
    SaveResourceDialog srd;
    srd.Init(-1, SuggestResourceNumber(resource.GetType()));
    if (IDOK == srd.DoModal())
    {
        // Assign it.
        resource.SetNumber(srd.GetResourceNumber());
        resource.SetPackage(srd.GetPackageNumber());
        resource.SetName(nullptr);

        // Save it.
        return AppendResource(resource);
    }
    else
    {
        return E_FAIL; // User cancelled.
    }
}

HRESULT CResourceMap::AppendResource(const ResourceBlob &resource)
{
    HRESULT hr;
    if (_cDeferAppend)
    {
        // If resource appends are deferred, then just add it to a list.
        _deferredResources.push_back(resource);
        hr = S_OK;
    }
    else
    {
        // Enumerate resources and write the ones we have not already encountered.
        std::unique_ptr<ResourceSource> resourceSource = CreateResourceSource(_gameFolderHelper.GameFolder, _gameFolderHelper.Version, resource.GetSourceFlags());
        std::vector<ResourceBlob> blobs;
        blobs.push_back(resource);

        try
        {
            resourceSource->AppendResources(blobs);
        }
        catch (std::exception &e)
        {
            AfxMessageBox(e.what(), MB_OK | MB_ICONWARNING);
        }

        hr = S_OK;

        AssignName(resource);

        if (SUCCEEDED(hr))
        {
            if (resource.GetType() == ResourceType::Script)
            {
                // We'll need to re-gen this:
                _globalCompiledScriptLookups.reset(nullptr);
            }

            // pResource is only valid for the length of this call.  Nonetheless, call our syncs
			for_each(_syncs.begin(), _syncs.end(), bind2nd(mem_fun(&ISyncResourceMap::OnResourceAdded), &resource));
            if (resource.GetType() == ResourceType::Palette)
            {
                _paletteListNeedsUpdate = true;
            }
        }

#if 0
        if (FAILED(hr))
        {
            // Prepare error.
            TCHAR szError[MAX_PATH];
            szError[0] = 0;
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, HRESULT_CODE(hr), 0, szError, ARRAYSIZE(szError), nullptr);
            if (hr == E_ACCESSDENIED)
            {
                StringCchCat(szError, ARRAYSIZE(szError), TEXT("\nThe file may be in use. Maybe the game is running?"));
            }

            TCHAR szMessage[MAX_PATH];
            StringCchPrintf(szMessage, ARRAYSIZE(szMessage), TEXT("There was an error writing the resource: 0x%x\n%s"), hr, szError);
            AfxMessageBox(szMessage, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL, 0);
        }
#endif
    }
    return hr;
}

bool CResourceMap::AppendResource(const ResourceEntity &resource, int *pChecksum)
{
    return AppendResource(resource, resource.PackageNumber, resource.ResourceNumber, pChecksum);
}

bool CResourceMap::AppendResource(const ResourceEntity &resource, int packageNumber, int resourceNumber, int *pChecksum)
{
    bool success = false;
    if (resource.PerformChecks())
    {
        sci::ostream serial;
        resource.WriteTo(serial);
        if (ValidateResourceSize(serial.tellp(), resource.GetType()))
        {
            ResourceBlob data;
            sci::istream readStream = istream_from_ostream(serial);
            data.CreateFromBits(nullptr, resource.GetType(), &readStream, packageNumber, resourceNumber, _gameFolderHelper.Version, resource.SourceFlags);
            success = SUCCEEDED(AppendResource(data));
            if (pChecksum)
            {
                *pChecksum = data.GetChecksum();
            }
        }
    }
    return success;
}

std::unique_ptr<ResourceContainer> CResourceMap::Resources(ResourceTypeFlags types, ResourceEnumFlags enumFlags)
{
    ResourceRecency *pRecency = nullptr;
    if ((enumFlags & ResourceEnumFlags::CalculateRecency) != ResourceEnumFlags::None)
    {
        pRecency = &appState->_resourceRecency;
        for (int i = 0; i < NumResourceTypes; i++)
        {
            if ((int)types & (1 << i))
            {
                // This resource type is being re-enumerated.
                pRecency->ClearResourceType(i);
            }
        }
    }

    return Helper().Resources(types, enumFlags, pRecency);
}

std::unique_ptr<ResourceBlob> CResourceMap::MostRecentResource(ResourceType type, int number, bool getName)
{
    ResourceEnumFlags flags = ResourceEnumFlags::None;
    if (getName)
    {
        flags |= ResourceEnumFlags::NameLookups;
    }
    return Helper().MostRecentResource(type, number, flags);
}

void CResourceMap::PurgeUnnecessaryResources()
{
    HRESULT hr = RebuildResources<RESOURCEMAPENTRY_SCI0, RESOURCEHEADER_SCI0>(_gameFolderHelper.Version, TRUE);
    if (SUCCEEDED(hr))
    {
        // Refresh everything.
        for_each(_syncs.begin(), _syncs.end(), bind2nd(mem_fun(&ISyncResourceMap::OnResourceMapReloaded), false));
    }
}

void CResourceMap::NotifyToResourceResourceType(ResourceType iType)
{
	for_each(_syncs.begin(), _syncs.end(), bind2nd(mem_fun(&ISyncResourceMap::OnResourceTypeReloaded), iType));
    if (iType == ResourceType::Palette)
    {
        _paletteListNeedsUpdate = true;
    }

    if (iType == ResourceType::Script)
    {
        // We'll need to re-gen this:
        _globalCompiledScriptLookups.reset(nullptr);
    }
}

//
// Add a listener for events.
//
void CResourceMap::AddSync(ISyncResourceMap *pSync)
{
    _syncs.push_back(pSync);
}

//
// Before your object gets destroyed, it MUST remove itself
//
void CResourceMap::RemoveSync(ISyncResourceMap *pSync)
{
    // We wouldn't expect to remove a sync that isn't there, so we can just erase
    // whatever we find (which should be a valid iterator)
    _syncs.erase(find(_syncs.begin(), _syncs.end(), pSync));
}

//
// Deletes the resource specified by pData, using the resource number, type and bits to compare.
//
void CResourceMap::DeleteResource(const ResourceBlob *pData)
{
    try
    {
        ::DeleteResource(*this, *pData);
    }
    catch (std::exception &e)
    {
        AfxMessageBox(e.what(), MB_OK | MB_ICONWARNING);
    }

    // Call our syncs, so they update.
    if (pData->GetType() == ResourceType::Script)
    {
        // We'll need to re-gen this:
        _globalCompiledScriptLookups.reset(nullptr);
    }

    for_each(_syncs.begin(), _syncs.end(), bind2nd(mem_fun(&ISyncResourceMap::OnResourceDeleted), pData));
    if (pData->GetType() == ResourceType::Palette)
    {
        _paletteListNeedsUpdate = true;
    }
}

//
// Returns an empty string (or pszDefault) if there is no key
//
std::string GetIniString(const std::string &iniFileName, PCTSTR pszSectionName, const std::string &keyName, PCSTR pszDefault)
{
    std::string strRet;
    char sz[200];
    if (GetPrivateProfileString(pszSectionName, keyName.c_str(), nullptr, sz, (DWORD)ARRAYSIZE(sz), iniFileName.c_str()))
    {
        strRet = sz;
    }
    else
    {
        strRet = pszDefault;
    }
    return strRet;
}

std::string CResourceMap::GetGameFolder() const
{
    return _gameFolderHelper.GameFolder;
}

LangSyntax CResourceMap::GetGameLanguage()
{
    if (_gameFolderHelper.Language == LangSyntaxUnknown)
    {
        if (_gameFolderHelper.GetIniString("Game", "Language", "sc") == "scp")
        {
            _gameFolderHelper.Language = LangSyntaxCpp;
        }
        else
        {
            _gameFolderHelper.Language = LangSyntaxSCIStudio;
        }
    }
    return _gameFolderHelper.Language;
}

//
// Gets the include folder that has read-only headers
//
std::string CResourceMap::GetIncludeFolder()
{
    std::string includeFolder = _includeFolderOverride;
    if (includeFolder.empty())
    {
        // The normal case...
        TCHAR szPath[MAX_PATH];
        if (GetModuleFileName(nullptr, szPath, ARRAYSIZE(szPath)))
        {
            PSTR pszFileName = PathFindFileName(szPath);
            *pszFileName = 0; // null it
            includeFolder += szPath;
        }
    }
    includeFolder += "include";
    return includeFolder;
}

//
// Gets the include folder that has read-only headers
//
std::string CResourceMap::GetTemplateFolder()
{
    return GetExeSubFolder("TemplateGame");
}

//
// Gets the samples folder 
//
std::string CResourceMap::GetSamplesFolder()
{
    return GetExeSubFolder("Samples");
}

std::string CResourceMap::GetDecompilerFolder()
{
    return GetExeSubFolder("Decompiler");
}

std::string CResourceMap::GetIncludePath(const std::string &includeFileName)
{
    std::string includeFolder = GetIncludeFolder();
    if (!includeFolder.empty())
    {
        includeFolder += "\\";
        includeFolder += includeFileName;
        if (PathFileExists(includeFolder.c_str()))
        {
            return includeFolder;
        }
    }
    includeFolder = Helper().GetSrcFolder();
    includeFolder += "\\";
    includeFolder += includeFileName;
    if (PathFileExists(includeFolder.c_str()))
    {
        return includeFolder;
    }
    return "";
}

#ifdef DOCSUPPORT
std::string CResourceMap::GetDocPath(const std::string &fileName)
{
    std::string docPath;
    TCHAR szPath[MAX_PATH];
    if (GetModuleFileName(nullptr, szPath, ARRAYSIZE(szPath)))
    {
        PSTR pszFileName = PathFindFileName(szPath);
        *pszFileName = 0; // null it
        docPath = szPath;
        docPath += "docs\\";
        docPath += fileName;
    }
    return docPath;
}
#endif

HRESULT CResourceMap::GetGameIni(PTSTR pszBuf, size_t cchBuf)
{
    HRESULT hr = E_FAIL;
    if (!_gameFolderHelper.GameFolder.empty())
    {
        hr = StringCchPrintf(pszBuf, cchBuf, TEXT("%s\\%s"), _gameFolderHelper.GameFolder.c_str(), TEXT("game.ini"));
    }
    return hr;
}

SCIVersion &CResourceMap::GetSCIVersion()
{
    return _gameFolderHelper.Version;
}

//
// Perf: we're opening and closing the file each time.  We could do this once.
//
std::string FigureOutResourceName(const std::string &iniFileName, ResourceType type, int iNumber)
{
    std::string name;
    if ((size_t)type < ARRAYSIZE(g_resourceInfo))
    {
        std::string keyName = default_reskey(iNumber);
        name = GetIniString(iniFileName, GetResourceInfo(type).pszTitleDefault, keyName, keyName.c_str());
    }
    return name;
}

// Someone on the forums started hitting problems once they had over 350 scripts. Our size was 5000.
// This allows 4x that. Should be high enough.
const DWORD c_IniSectionMaxSize = 20000;

HRESULT CResourceMap::GetScriptNumber(ScriptId script, WORD &wScript)
{
    wScript = 0xffff;
    TCHAR szGameIni[MAX_PATH];
    HRESULT hr = GetGameIni(szGameIni, ARRAYSIZE(szGameIni));
    if (SUCCEEDED(hr))
    {
        hr = E_INVALIDARG;
		DWORD cchBuf = c_IniSectionMaxSize;
		std::unique_ptr<char[]> szNameValues = std::make_unique<char[]>(cchBuf);

		DWORD nLength = GetPrivateProfileSection(TEXT("Script"), szNameValues.get(), cchBuf, szGameIni);
		if (nLength > 0 && ((cchBuf - 2) != nLength)) // returns (cchNameValues - 2) in case of insufficient buffer 
        {
            TCHAR *psz = szNameValues.get();
            while(*psz && (FAILED(hr)))
            {
                size_t cch = strlen(psz);
                char *pszEq = StrChr(psz, TEXT('='));
                CString strTitle = script.GetTitle().c_str();
                if (pszEq && (0 == strTitle.CompareNoCase(pszEq + 1)))
                {
                    // We have a match in script name... find the number
                    TCHAR *pszNumber = StrChr(psz, TEXT('n'));
                    if (pszNumber)
                    {
                        wScript = (WORD)StrToInt(pszNumber + 1);
                        ASSERT(script.GetResourceNumber() == InvalidResourceNumber ||
                               script.GetResourceNumber() == wScript);
                        hr = S_OK;
                    }
                }
                // Advance to next string.
                psz += (cch + 1);
            }
        }
    }

    if (FAILED(hr) && script.GetResourceNumber() != InvalidResourceNumber)
    {
        wScript = script.GetResourceNumber();
        hr = S_OK;
    }

    return hr;
}

const Vocab000 *CResourceMap::GetVocab000()
{
    ResourceEntity *pResource = GetVocabResourceToEdit();
    if (pResource)
    {
        return pResource->TryGetComponent<Vocab000>();
    }
    return nullptr;
}

std::unique_ptr<PaletteComponent> CResourceMap::GetPalette(int fallbackPaletteNumber)
{
    std::unique_ptr<PaletteComponent> paletteReturn;
    if (fallbackPaletteNumber == 999)
    {
        paletteReturn = make_unique<PaletteComponent>(*GetPalette999());
    }
    else
    {
        std::unique_ptr<ResourceEntity> paletteFallback = CreateResourceFromNumber(ResourceType::Palette, fallbackPaletteNumber);
        if (paletteFallback && paletteFallback->TryGetComponent<PaletteComponent>())
        {
            paletteReturn = make_unique<PaletteComponent>(paletteFallback->GetComponent<PaletteComponent>());
        }
    }
    return paletteReturn;
}

std::unique_ptr<PaletteComponent> CResourceMap::GetMergedPalette(const ResourceEntity &resource, int fallbackPaletteNumber)
{
    assert((_gameFolderHelper.Version.ViewFormat != ViewFormat::EGA) || (_gameFolderHelper.Version.PicFormat != PicFormat::EGA));
    std::unique_ptr<PaletteComponent> paletteReturn;
    PaletteComponent *paletteEmbedded = resource.TryGetComponent<PaletteComponent>();
    if (!paletteEmbedded)
    {
        paletteEmbedded = &_emptyPalette;
    }

    // Clone the embedded palette first - REVIEW: make_unique arg forwarding doesn't work with copy constructor. No object copy happens.
    paletteReturn = make_unique<PaletteComponent>(*paletteEmbedded);
    if (fallbackPaletteNumber == 999)
    {
        paletteReturn->MergeFromOther(GetPalette999());
    }
    else
    {
        std::unique_ptr<ResourceEntity> paletteFallback = CreateResourceFromNumber(ResourceType::Palette, fallbackPaletteNumber);
        if (paletteFallback)
        {
            paletteReturn->MergeFromOther(paletteFallback->TryGetComponent<PaletteComponent>());
        }
    }
    return paletteReturn;
}

const AudioMapComponent *CResourceMap::GetAudioMap65535()
{
    if (!_pAudioMap65535)
    {
        // In some games this is a "patch file", in others it's embedded in resource.map
        _pAudioMap65535 = CreateResourceFromNumber(ResourceType::Map, 65535);
    }

    return _pAudioMap65535 ? _pAudioMap65535->TryGetComponent<AudioMapComponent>() : nullptr;
}

void CResourceMap::SaveAudioMap65535(const AudioMapComponent &newAudioMap)
{
    GetAudioMap65535(); // Just ensure we have one
    // Assign the new component to it.
    _pAudioMap65535->GetComponent<AudioMapComponent>() = newAudioMap;
    assert(_pAudioMap65535->SourceFlags == ResourceSourceFlags::PatchFile && "Need to support embedded audio maps");
    AppendResource(*_pAudioMap65535);

    _pAudioMap65535.reset(nullptr);
}

const PaletteComponent *CResourceMap::GetPalette999()
{
    PaletteComponent *globalPalette = nullptr;
    if (_gameFolderHelper.Version.HasPalette)
    {
        if (!_pPalette999)
        {
            _pPalette999 = CreateResourceFromNumber(ResourceType::Palette, 999);
        }
        if (_pPalette999)
        {
            globalPalette = _pPalette999->TryGetComponent<PaletteComponent>();
        }
    }
    return globalPalette;
}

std::vector<int> CResourceMap::GetPaletteList()
{
    if (_paletteListNeedsUpdate)
    {
        _paletteListNeedsUpdate = false;
        _paletteList.clear();
        auto paletteContainer = Resources(ResourceTypeFlags::Palette, ResourceEnumFlags::MostRecentOnly);
        bool has999 = false;
        for (auto it = paletteContainer->begin(); it != paletteContainer->end(); ++it)
        {
            if (it.GetResourceNumber() == 999)
            {
                has999 = true;
            }
            else
            {
                _paletteList.push_back(it.GetResourceNumber());
            }
        }

        // Sort and put 999 first.
        std::sort(_paletteList.begin(), _paletteList.end());
        if (has999)
        {
            _paletteList.insert(_paletteList.begin(), 999);
        }
    }
    return _paletteList;
}

GlobalCompiledScriptLookups *CResourceMap::GetCompiledScriptLookups()
{
    if (!_globalCompiledScriptLookups)
    {
        _globalCompiledScriptLookups = make_unique<GlobalCompiledScriptLookups>();
        if (!_globalCompiledScriptLookups->Load(Helper()))
        {
            // Warning... (happens in LB Dagger)
        }
    }
    return _globalCompiledScriptLookups.get();
}

ResourceEntity *CResourceMap::GetVocabResourceToEdit()
{
    if (!_pVocab000)
    {
        _pVocab000 = CreateResourceFromNumber(ResourceType::Vocab, _gameFolderHelper.Version.MainVocabResource);
    }
    return _pVocab000.get();
}

void CResourceMap::ClearVocab000()
{
    _pVocab000.reset(nullptr);
}

HRESULT GetFilePositionHelper(HANDLE hFile, DWORD *pdwPos)
{
    HRESULT hr = S_OK;
    *pdwPos = SetFilePointer(hFile, 0, nullptr, FILE_CURRENT);
    if (*pdwPos == INVALID_SET_FILE_POINTER)
    {
        // Might have failed...
        hr = ResultFromLastError();
    }
    return hr;
}

HRESULT GetResourceMapType(HANDLE hFile, DWORD *pdwType)
{
    HRESULT hr = S_OK;
    *pdwType = RESOURCEMAPTYPE_SCI0;
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwOldPos;
        hr = GetFilePositionHelper(hFile, &dwOldPos);
        if (SUCCEEDED(hr))
        {
            // Set to beginning
            hr = SetFilePositionHelper(hFile, 0);
        }

        if (SUCCEEDED(hr))
        {
            BYTE byte;
            DWORD dwRead;
            if (ReadFile(hFile, &byte, 1, &dwRead, nullptr) && dwRead == sizeof(byte))
            {
                if (byte == 0x80)
                {
                    // SCI1 resource.
                    *pdwType = RESOURCEMAPTYPE_SCI1;
                }
                // Restore file position
                hr = SetFilePositionHelper(hFile, dwOldPos);
            }
            else
            {
                hr = ResultFromLastError();
            }
        }
    }
    return hr;
}

void CResourceMap::SetScriptLanguage(ScriptId script, LangSyntax language)
{
    Helper().SetIniString("Language", script.GetTitle(), (language == LangSyntaxCpp) ? "scp" : "sc");
}

void CResourceMap::GetAllScripts(std::vector<ScriptId> &scripts)
{
    TCHAR szIniFile[MAX_PATH];
    if (SUCCEEDED(GetGameIni(szIniFile, ARRAYSIZE(szIniFile))))
    {
		DWORD cchBuf = c_IniSectionMaxSize;
		std::unique_ptr<char[]> szNameValues = std::make_unique<char[]>(cchBuf);
        DWORD nLength =  GetPrivateProfileSection(TEXT("Script"), szNameValues.get(), cchBuf, szIniFile);
        if (nLength > 0 && ((cchBuf - 2) != nLength)) // returns (cchBuf - 2) in case of insufficient buffer 
        {
            TCHAR *psz = szNameValues.get();
            while(*psz)
            {
                // The format is
                // n000=ScriptName
                size_t cch = strlen(psz);
                char *pszNumber = StrChr(psz, TEXT('n'));
                if (pszNumber)
                {
                    char *pszEq = StrChr(pszNumber, TEXT('='));
                    if (pszEq)
                    {
                        // Add this script...
                        ScriptId scriptId(_gameFolderHelper.GetScriptFileName(pszEq + 1));
                        // Isolate the number.
                        *pszEq = 0;     // n123
                        pszNumber++;    // 123
                        scriptId.SetResourceNumber(StrToInt(pszNumber));
                        scripts.push_back(scriptId);
                    }
                }
                // Advance to next string.
                psz += (cch + 1);
            }
        }
    }
}

void CResourceMap::GetNumberToNameMap(std::unordered_map<WORD, std::string> &scos)
{
    TCHAR szIniFile[MAX_PATH];
    if (SUCCEEDED(GetGameIni(szIniFile, ARRAYSIZE(szIniFile))))
    {
		DWORD cchBuf = c_IniSectionMaxSize;
		std::unique_ptr<char[]> szNameValues = std::make_unique<char[]>(cchBuf);
        DWORD nLength =  GetPrivateProfileSection(TEXT("Script"), szNameValues.get(), cchBuf, szIniFile);
        if (nLength > 0 && ((cchBuf - 2) != nLength)) // returns (cchBuf - 2) in case of insufficient buffer 
        {
            TCHAR *psz = szNameValues.get();
            while(*psz)
            {
                // The format is
                // n000=ScriptName
                size_t cch = strlen(psz);
                char *pszNumber = StrChr(psz, TEXT('n'));
                if (pszNumber)
                {
                    ++pszNumber; // Advance past the 'n'
                    char *pszEq = StrChr(pszNumber, TEXT('='));
                    if (pszEq)
                    {
                        scos[(WORD)StrToInt(pszNumber)] = pszEq + 1;
                    }
                }
                // Advance to next string.
                psz += (cch + 1);
            }
        }
    }
}

bool CResourceMap::CanSaveResourcesToMap()
{
    //return (GetSCIVersion().MapFormat == ResourceMapFormat::SCI0);
    return true;    // Now supported for all.
}

//
// Called when we open a new game.
//
void CResourceMap::SetGameFolder(const string &gameFolder)
{
    _gameFolderHelper.GameFolder = gameFolder;
    ClearVocab000();
    _pPalette999.reset(nullptr);                    // REVIEW: also do this if global palette is edited.
    _globalCompiledScriptLookups.reset(nullptr);
    _pAudioMap65535.reset(nullptr);
    _gameFolderHelper.Language = LangSyntaxUnknown;
    if (!gameFolder.empty())
    {
        // We get here when we close documents.
        _SniffSCIVersion();
        // Send initial load notification
        for_each(_syncs.begin(), _syncs.end(), bind2nd(mem_fun(&ISyncResourceMap::OnResourceMapReloaded), true));

        _paletteListNeedsUpdate = true;
    }
}

// Returns null if it doesn't exist.
std::unique_ptr<ResourceEntity> CResourceMap::CreateResourceFromNumber(ResourceType type, WORD number)
{
    std::unique_ptr<ResourceEntity> pResource;
    unique_ptr<ResourceBlob> data = MostRecentResource(type, number, true);
    // This can legitimately fail. For instance, a script that hasn't yet been compiled.
    if (data)
    {
        pResource = CreateResourceFromResourceData(*data);
    }
    return pResource;
}


#define DECLARE_CREATE_RESOURCE_WITH_CREATE_FUNCTION(type, createFunction, fallbackCreateFunction, postProcessFunction) \
    case type: \
        { \
            pResourceReturn.reset(createFunction(data.GetVersion())); \
            try \
            { \
                pResourceReturn->InitFromResource(&data); \
                postProcessFunction(*pResourceReturn); \
            } \
            catch (std::exception &theException) \
            { \
                /* Just create the default one */  \
                appState->LogInfo(theException.what()); \
                data.AddStatusFlags(ResourceLoadStatusFlags::ResourceCreationFailed); \
                pResourceReturn.reset(fallbackCreateFunction(data.GetVersion())); \
                pResourceReturn->ResourceNumber = data.GetNumber(); \
                pResourceReturn->PackageNumber = data.GetPackageHint(); \
            } \
        } \
        break; \


void DoNothing(ResourceEntity &resource)
{

}

//
// Given a ResourceBlob, this creates the SCI resource represented by the data, and hands back
// a ResourceEntity.
// If there is an exception creating the resource, a default one is handed back.
//
std::unique_ptr<ResourceEntity> CreateResourceFromResourceData(const ResourceBlob &data)
{
    std::unique_ptr<ResourceEntity> pResourceReturn;
    switch (data.GetType())
    {
        DECLARE_CREATE_RESOURCE_WITH_CREATE_FUNCTION(ResourceType::View, CreateViewResource, CreateDefaultViewResource, DoNothing)
        DECLARE_CREATE_RESOURCE_WITH_CREATE_FUNCTION(ResourceType::Font, CreateFontResource, CreateDefaultFontResource, DoNothing)
        DECLARE_CREATE_RESOURCE_WITH_CREATE_FUNCTION(ResourceType::Cursor, CreateCursorResource, CreateDefaultCursorResource, DoNothing)
        DECLARE_CREATE_RESOURCE_WITH_CREATE_FUNCTION(ResourceType::Text, CreateTextResource, CreateDefaultTextResource, DoNothing)
        DECLARE_CREATE_RESOURCE_WITH_CREATE_FUNCTION(ResourceType::Sound, CreateSoundResource, CreateDefaultSoundResource, DoNothing)
        DECLARE_CREATE_RESOURCE_WITH_CREATE_FUNCTION(ResourceType::Vocab, CreateVocabResource, CreateVocabResource, DoNothing)
        DECLARE_CREATE_RESOURCE_WITH_CREATE_FUNCTION(ResourceType::Pic, CreatePicResource, CreateDefaultPicResource, DoNothing)
        DECLARE_CREATE_RESOURCE_WITH_CREATE_FUNCTION(ResourceType::Palette, CreatePaletteResource, CreatePaletteResource, DoNothing)
        DECLARE_CREATE_RESOURCE_WITH_CREATE_FUNCTION(ResourceType::Message, CreateMessageResource, CreateDefaultMessageResource, DoNothing)
        DECLARE_CREATE_RESOURCE_WITH_CREATE_FUNCTION(ResourceType::Audio, CreateAudioResource, CreateDefaultAudioResource, DoNothing)
        DECLARE_CREATE_RESOURCE_WITH_CREATE_FUNCTION(ResourceType::Map, CreateMapResource, CreateMapResource, DoNothing)
    default:
        assert(false);
        break;
    }
    return pResourceReturn;
}

void CResourceMap::SetGameLanguage(LangSyntax lang)
{
    Helper().SetIniString("Game", "Language", (lang == LangSyntaxCpp) ? "scp" : "sc");
    _gameFolderHelper.Language = GetGameLanguage();
}
