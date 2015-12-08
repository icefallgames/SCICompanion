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
#include "Sync.h"
#include "ResourceEntity.h"
#include "ResourceSources.h"
#include "CompiledScript.h"
#include "Disassembler.h"
#include "ResourceMapOperations.h"
#include "MessageHeaderFile.h"
#include "MessageSource.h"
#include "format.h"
#include "ResourceMapEvents.h"
#include "DebuggerThread.h"
#include "RunLogic.h"
#include "ResourceBlob.h"
#include "DependencyTracker.h"

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
    try
    {
        // Do the audio stuff first, because it will end up adding new audio maps to the game's resources
        // (and RebuildResource should clean out the old ones)
        if (version.AudioVolumeName != AudioVolumeName::None)
        {
            std::unique_ptr<ResourceSource> resourceSource = CreateResourceSource(appState->GetResourceMap().Helper(), ResourceSourceFlags::AudioCache);
            resourceSource->RebuildResources(true);
        }

        // Enumerate resources and write the ones we have not already encountered.
        std::unique_ptr<ResourceSource> resourceSource = CreateResourceSource(appState->GetResourceMap().Helper(), ResourceSourceFlags::ResourceMap);
        resourceSource->RebuildResources(true);
        if (version.MessageMapSource != MessageMapSource::Included)
        {
            ResourceSourceFlags sourceFlags = (version.MessageMapSource == MessageMapSource::MessageMap) ? ResourceSourceFlags::MessageMap : ResourceSourceFlags::AltMap;
            std::unique_ptr<ResourceSource> messageSource = CreateResourceSource(appState->GetResourceMap().Helper(), ResourceSourceFlags::MessageMap);
            messageSource->RebuildResources(true);
        }
    }
    catch (std::exception &e)
    {
        AfxMessageBox(e.what(), MB_OK | MB_ICONWARNING);
    }
    return S_OK;
}

//
// CResourceMap
// Helper class for managing resources.
//
CResourceMap::CResourceMap(AppState &appState) : _appState(appState)
{
    _runLogic = std::make_unique<RunLogic>();
    _paletteListNeedsUpdate = true;
    _skipVersionSniffOnce = false;
    _pVocab000 = nullptr;
    _cDeferAppend = 0;
    _gameFolderHelper.Language = LangSyntaxUnknown;
    _gameFolderHelper.Version = sciVersion0;    // By default
    _dependencyTracker = std::make_unique<DependencyTracker>(_appState._fTrackHeaderFiles);
	// This is a pointer because we don't want a dependency on it in the header file.
	_classBrowser = std::make_unique<SCIClassBrowser>(*_dependencyTracker);
    _deferredResources.reserve(300);            // So we don't need to resize much it when adding
    _emptyPalette = std::make_unique<PaletteComponent>();
    memset(_emptyPalette->Colors, 0, sizeof(_emptyPalette->Colors));
}

CResourceMap::~CResourceMap()
{
    assert(_syncs.empty()); // They should remove themselves.
    assert(_cDeferAppend == 0);
}

//
// Adds the name of the resource to the game.ini file.
//
void CResourceMap::AssignName(const ResourceBlob &resource)
{
    // Assign the name of the item.
    std::string keyName = default_reskey(resource.GetNumber(), resource.GetHeader().Base36Number);
    std::string name = resource.GetName();
    if (!name.empty() && (0 != lstrcmpi(keyName.c_str(), name.c_str())))
    {
        Helper().SetIniString(g_resourceInfo[(int)resource.GetType()].pszTitleDefault, keyName, name);
    }
}

void CResourceMap::AssignName(ResourceType type, int iResourceNumber, uint32_t base36Number, PCTSTR pszName)
{
    // Assign the name of the item.
    std::string keyName = default_reskey(iResourceNumber, base36Number);
    std::string newValue;
    if (pszName)
    {
        newValue = pszName;
    }
    if (0 != lstrcmpi(keyName.c_str(), newValue.c_str()))
    {
        Helper().SetIniString(g_resourceInfo[(int)type].pszTitleDefault, keyName, newValue);
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
                // probably something we'll need to handle. And now also audiomaps

                // Bucketize the resources by resourcesource and map context.
                std::map<uint64_t, std::vector<const ResourceBlob*>> keyToIndices;
                for (size_t i = 0; i < _deferredResources.size(); i++)
                {
                    uint32_t mapContext = (uint32_t)((_deferredResources[i].GetBase36() == NoBase36) ? -1 : _deferredResources[i].GetNumber());
                    uint64_t bucketKey = (uint64_t)_deferredResources[i].GetSourceFlags() | (((uint64_t)mapContext) << 32);
                    keyToIndices[bucketKey].push_back(&_deferredResources[i]);
                }

                for (auto &pair : keyToIndices)
                {
                    // For each bucket, create a resourcesource and save them.
                    std::vector<const ResourceBlob*> &blobsForThisSource = pair.second;
                    ResourceSourceFlags sourceFlags = blobsForThisSource[0]->GetSourceFlags();

                    int mapContext = (blobsForThisSource[0]->GetBase36() == NoBase36) ? -1 : blobsForThisSource[0]->GetNumber();
                    // Enumerate resources and write the ones we have not already encountered.
                    std::unique_ptr<ResourceSource> resourceSource = CreateResourceSource(_gameFolderHelper, sourceFlags, ResourceSourceAccessFlags::ReadWrite, mapContext);

                    try
                    {
                        // We're going to tell the relevant folks to reload completely, so we
                        // don't care about the return value here (replace or append)
                        resourceSource->AppendResources(blobsForThisSource);
                    }
                    catch (std::exception &e)
                    {
                        AfxMessageBox(e.what(), MB_OK | MB_ICONWARNING);
                    }
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
                        NotifyToReloadResourceType((ResourceType)iType);
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

void CResourceMap::StartDebuggerThread(int optionalResourceNumber)
{
    AbortDebuggerThread();
    _debuggerThread = CreateDebuggerThread(Helper().GameFolder, optionalResourceNumber);
}
 
void CResourceMap::RepackageAudio(bool force)
{
    // Rebuild any out-of-date audio resources. This should nearly be a no-op if none are out of data.
    // TODO: This could be slow, so provide some kind of UI feedback?
    if (GetSCIVersion().AudioVolumeName != AudioVolumeName::None)
    {
        std::unique_ptr<ResourceSource> resourceSource = CreateResourceSource(Helper(), ResourceSourceFlags::AudioCache);
        resourceSource->RebuildResources(force);    // false -> don't force.
    }
}

DependencyTracker &CResourceMap::GetDependencyTracker()
{
    return *_dependencyTracker;
}

void CResourceMap::AbortDebuggerThread()
{
    if (_debuggerThread)
    {
        _debuggerThread->Abort();
        _debuggerThread.reset();
    }
}

void CResourceMap::AppendResourceAskForNumber(ResourceEntity &resource)
{
    AppendResourceAskForNumber(resource, "", false);
}

void CResourceMap::AppendResourceAskForNumber(ResourceEntity &resource, const std::string &name, bool warnOnOverwrite)
{
    // Invoke dialog to suggest/ask for a resource number
    SaveResourceDialog srd(warnOnOverwrite, resource.GetType());
    srd.Init(-1, SuggestResourceNumber(resource.GetType()), name);
    if (IDOK == srd.DoModal())
    {
        // Assign it.
        resource.ResourceNumber = srd.GetResourceNumber();
        resource.PackageNumber = srd.GetPackageNumber();
        AssignName(resource.GetType(), resource.ResourceNumber, NoBase36, srd.GetName().c_str());
        AppendResource(resource);
    }
}

//
// Ask the user where to save the resource... and then save it.
//
HRESULT CResourceMap::AppendResourceAskForNumber(ResourceBlob &resource, bool warnOnOverwrite)
{
    if (!IsVersionCompatible(resource.GetType(), resource.GetVersion(), GetSCIVersion()))
    {
        if (IDNO == AfxMessageBox("The version of the resource being added does not match the version of the game.\nAdding it might cause the game to be corrupted.\nDo you want to go ahead anyway?", MB_YESNO))
        {
            return E_FAIL;
        }
    }
    // Invoke dialog to suggest/ask for a resource number
    SaveResourceDialog srd(warnOnOverwrite, resource.GetType());
    srd.Init(-1, SuggestResourceNumber(resource.GetType()), resource.GetName());
    if (IDOK == srd.DoModal())
    {
        // Assign it.
        resource.SetNumber(srd.GetResourceNumber());
        resource.SetPackage(srd.GetPackageNumber());
        resource.SetName(nullptr);
        if (!srd.GetName().empty())
        {
            resource.SetName(srd.GetName().c_str());
        }

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
        if (resource.GetType() == ResourceType::View)
        {
            _appState.SetRecentlyInteractedView(resource.GetNumber());
        }

        // Enumerate resources and write the ones we have not already encountered.
        int mapContext = (resource.GetBase36() == NoBase36) ? -1 : resource.GetNumber();
        std::unique_ptr<ResourceSource> resourceSource = CreateResourceSource(_gameFolderHelper, resource.GetSourceFlags(), ResourceSourceAccessFlags::ReadWrite, mapContext);
        std::vector<const ResourceBlob*> blobs;
        blobs.push_back(&resource);

        AppendBehavior appendBehavior = AppendBehavior::Append;
        hr = E_FAIL;
        try
        {
            appendBehavior = resourceSource->AppendResources(blobs);
            hr = S_OK;
        }
        catch (std::exception &e)
        {
            AfxMessageBox(e.what(), MB_OK | MB_ICONWARNING);
        }

        AssignName(resource);

        if (SUCCEEDED(hr))
        {
            if (resource.GetType() == ResourceType::Script)
            {
                // We'll need to re-gen this:
                _globalCompiledScriptLookups.reset(nullptr);
            }

            if (resource.GetType() == ResourceType::Palette)
            {
                _paletteListNeedsUpdate = true;
                if (resource.GetNumber() == 999)
                {
                    _pPalette999.reset(nullptr);
                }
            }

            // pResource is only valid for the length of this call.  Nonetheless, call our syncs
            for (auto &sync : _syncs)
            {
                sync->OnResourceAdded(&resource, appendBehavior);
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
    return AppendResource(resource, resource.PackageNumber, resource.ResourceNumber, resource.Base36Number, pChecksum);
}

bool CResourceMap::AppendResource(const ResourceEntity &resource, int packageNumber, int resourceNumber, uint32_t base36Number, int *pChecksum)
{
    bool success = false;
    if (resource.PerformChecks())
    {
        ResourceBlob data;
        sci::ostream serial;
        resource.WriteTo(serial, true, resourceNumber, data.GetPropertyBag());
        if (ValidateResourceSize(Helper().Version, serial.tellp(), resource.GetType()))
        {
            sci::istream readStream = istream_from_ostream(serial);
            data.CreateFromBits(nullptr, resource.GetType(), &readStream, packageNumber, resourceNumber, base36Number, _gameFolderHelper.Version, resource.SourceFlags);
            success = SUCCEEDED(AppendResource(data));
            if (pChecksum)
            {
                *pChecksum = data.GetChecksum();
            }
        }
    }
    return success;
}

std::unique_ptr<ResourceContainer> CResourceMap::Resources(ResourceTypeFlags types, ResourceEnumFlags enumFlags, int mapContext)
{
    ResourceRecency *pRecency = nullptr;
    if ((enumFlags & ResourceEnumFlags::CalculateRecency) != ResourceEnumFlags::None)
    {
        pRecency = &_appState._resourceRecency;
        for (int i = 0; i < NumResourceTypes; i++)
        {
            if ((int)types & (1 << i))
            {
                // This resource type is being re-enumerated.
                pRecency->ClearResourceType(i);
            }
        }
    }

    return Helper().Resources(types, enumFlags, pRecency, mapContext);
}

bool CResourceMap::DoesResourceExist(ResourceType type, int number, std::string *retrieveName)
{
    // NOTE: We exclude path files right now. We could push this functionality into the ResourceSources to optimize.
    ResourceEnumFlags enumFlags = ResourceEnumFlags::ExcludePatchFiles;
    if (retrieveName)
    {
        enumFlags |= ResourceEnumFlags::NameLookups;
    }
    auto &resourceContainer = Resources(ResourceTypeToFlag(type), enumFlags);
    for (auto &blobIt = resourceContainer->begin(); blobIt != resourceContainer->end(); ++blobIt)
    {
        if (blobIt.GetResourceNumber() == number)
        {
            if (retrieveName)
            {
                *retrieveName = (*blobIt)->GetName();
            }
            return true;
        }
    }
    return false;
}

std::unique_ptr<ResourceBlob> CResourceMap::MostRecentResource(ResourceType type, int number, bool getName, uint32_t base36Number, int mapContext)
{
    ResourceEnumFlags flags = ResourceEnumFlags::None;
    if (getName)
    {
        flags |= ResourceEnumFlags::NameLookups;
    }
    return Helper().MostRecentResource(type, number, flags, base36Number, mapContext);
}

void CResourceMap::PurgeUnnecessaryResources()
{
    HRESULT hr = RebuildResources<RESOURCEMAPENTRY_SCI0, RESOURCEHEADER_SCI0>(_gameFolderHelper.Version, TRUE);
    if (SUCCEEDED(hr))
    {
        // Refresh everything.
        for_each(_syncs.begin(), _syncs.end(), bind2nd(mem_fun(&IResourceMapEvents::OnResourceMapReloaded), false));
    }
}

void CResourceMap::NotifyToRegenerateImages()
{
    for_each(_syncs.begin(), _syncs.end(), mem_fun(&IResourceMapEvents::OnImagesInvalidated));
}

void CResourceMap::NotifyToReloadResourceType(ResourceType iType)
{
	for_each(_syncs.begin(), _syncs.end(), bind2nd(mem_fun(&IResourceMapEvents::OnResourceTypeReloaded), iType));
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
void CResourceMap::AddSync(IResourceMapEvents *pSync)
{
    _syncs.push_back(pSync);
}

//
// Before your object gets destroyed, it MUST remove itself
//
void CResourceMap::RemoveSync(IResourceMapEvents *pSync)
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
    // Early bail out. Without palette 999, we'll mis-identify the SCI type, and things will be bad
    // (and also... we won't have a global palette)
    if ((pData->GetType() == ResourceType::Palette) && (pData->GetNumber() == 999))
    {
        AfxMessageBox("Palette 999 is the global palette and cannot be deleted.", MB_OK | MB_ICONERROR);
        return;
    }

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

    for_each(_syncs.begin(), _syncs.end(), bind2nd(mem_fun(&IResourceMapEvents::OnResourceDeleted), pData));
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

void CResourceMap::_SetGameLanguage()
{
    if (_gameFolderHelper.Language == LangSyntaxUnknown)
    {
        std::string languageValue = _gameFolderHelper.GetIniString(GameSection, LanguageKey, LanguageValueStudio.c_str());
        if (languageValue == "scp")
        {
            // We have left this turd in from old game.inis. We don't support cpp as a default game language,
            // so let's just convert it to Studio.
            _gameFolderHelper.Language = LangSyntaxStudio;
        }
        else if (languageValue == LanguageValueSCI)
        {
            _gameFolderHelper.Language = LangSyntaxSCI;
        }
        else
        {
            _gameFolderHelper.Language = LangSyntaxStudio;
        }
    }
}

//
// Gets the include folder that has read-only headers
//
std::string CResourceMap::GetIncludeFolder()
{
    std::string includeFolder = _includeFolderOverride;
    if (includeFolder.empty())
    {
        return _gameFolderHelper.GetIncludeFolder();
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

std::string CResourceMap::GetTopLevelSamplesFolder()
{
    return GetExeSubFolder("Samples");
}

//
// Gets the samples folder 
//
std::string CResourceMap::GetSamplesFolder()
{
    std::string folder = GetExeSubFolder("Samples");
    if (GetSCIVersion().MapFormat == ResourceMapFormat::SCI0)
    {
        folder += "\\SCI0";
    }
    else
    {
        folder += "\\SCI1.1";
    }
    return folder;
}


//
// Gets the objects folder 
//
std::string CResourceMap::GetObjectsFolder()
{
    std::string folder = GetExeSubFolder("Objects");
    if (GetSCIVersion().MapFormat == ResourceMapFormat::SCI0)
    {
        folder += "\\SCI0";
    }
    else
    {
        folder += "\\SCI1.1";
    }
    return folder;
}

std::string CResourceMap::GetDecompilerFolder()
{
    return GetExeSubFolder("Decompiler");
}

bool hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else {
        return false;
    }
}

std::string CResourceMap::GetIncludePath(const std::string &includeFileName)
{
    if (hasEnding(includeFileName, ".shm"))
    {
        return Helper().GetMsgFolder() + "\\" + includeFileName;
    }
    else if (hasEnding(includeFileName, ".shp"))
    {
        return Helper().GetPolyFolder() + "\\" + includeFileName;
    }
    else
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
}


HRESULT CResourceMap::GetGameIni(PTSTR pszBuf, size_t cchBuf)
{
    HRESULT hr = E_FAIL;
    if (!_gameFolderHelper.GameFolder.empty())
    {
        hr = StringCchPrintf(pszBuf, cchBuf, TEXT("%s\\%s"), _gameFolderHelper.GameFolder.c_str(), TEXT("game.ini"));
    }
    return hr;
}

const SCIVersion &CResourceMap::GetSCIVersion() const
{
    return _gameFolderHelper.Version;
}

void CResourceMap::SetVersion(const SCIVersion &version)
{
    _gameFolderHelper.Version = version;
}

//
// Perf: we're opening and closing the file each time.  We could do this once.
//
std::string FigureOutResourceName(const std::string &iniFileName, ResourceType type, int iNumber, uint32_t base36Number)
{
    std::string name;
    if ((size_t)type < ARRAYSIZE(g_resourceInfo))
    {
        std::string keyName = default_reskey(iNumber, base36Number);
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
        const PaletteComponent *pc = GetPalette999();
        if (pc)
        {
            paletteReturn = make_unique<PaletteComponent>(*pc);
        }
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
    const PaletteComponent *paletteEmbedded = resource.TryGetComponent<PaletteComponent>();
    if (!paletteEmbedded)
    {
        paletteEmbedded = _emptyPalette.get();
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

void CResourceMap::SaveAudioMap65535(const AudioMapComponent &newAudioMap, int mapContext)
{
    // Best idea is to get the existing one, modify, then save. That way we don't need to figure out where it came from.
    int number = mapContext;
    if (mapContext == -1)
    {
        number = newAudioMap.Traits.MainAudioMapResourceNumber;
    }
    std::unique_ptr<ResourceEntity> entity = CreateResourceFromNumber(ResourceType::AudioMap, number, NoBase36, mapContext);
    
    // Assign the new component to it.
    entity->RemoveComponent<AudioMapComponent>();
    entity->AddComponent<AudioMapComponent>(std::make_unique<AudioMapComponent>(newAudioMap));
    AppendResource(*entity);
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
    return true;    // Now supported for all.
}

MessageSource *CResourceMap::GetVerbsMessageSource(bool reload)
{
    if (!_verbsHeaderFile || reload)
    {
        string messageFilename = "Verbs.sh";
        string messageFilePath = fmt::format("{0}\\{1}", _appState.GetResourceMap().Helper().GetSrcFolder(), messageFilename);
        _verbsHeaderFile = make_unique<MessageHeaderFile>(messageFilePath, messageFilename, initializer_list<string>({ "VERBS" }));
    }
    return _verbsHeaderFile->GetMessageSource();
}

MessageSource *CResourceMap::GetTalkersMessageSource(bool reload)
{
    if (!_talkersHeaderFile || reload)
    {
        string messageFilename = "Talkers.sh";
        string messageFilePath = fmt::format("{0}\\{1}", _appState.GetResourceMap().Helper().GetSrcFolder(), messageFilename);
        _talkersHeaderFile = make_unique<MessageHeaderFile>(messageFilePath, messageFilename, initializer_list<string>({}));
    }
    return _talkersHeaderFile->GetMessageSource();
}

RunLogic &CResourceMap::GetRunLogic()
{
    return *_runLogic;
}

//
// Called when we open a new game.
//
void CResourceMap::SetGameFolder(const string &gameFolder)
{
    _dependencyTracker->Clear();
    _runLogic->SetGameFolder(gameFolder);
    _gameFolderHelper.GameFolder = gameFolder;
    _talkerToView = TalkerToViewMap(Helper().GetLipSyncFolder());
    ClearVocab000();
    _pPalette999.reset(nullptr);                    // REVIEW: also do this if global palette is edited.
    _globalCompiledScriptLookups.reset(nullptr);
    _gameFolderHelper.Language = LangSyntaxUnknown;
    _talkersHeaderFile.reset(nullptr);
    _verbsHeaderFile.reset(nullptr);
    if (!gameFolder.empty())
    {
        try
        {
            _SetGameLanguage();

            // We get here when we close documents.
            _SniffSCIVersion();

            // Send initial load notification
            for_each(_syncs.begin(), _syncs.end(), bind2nd(mem_fun(&IResourceMapEvents::OnResourceMapReloaded), true));

            _paletteListNeedsUpdate = true;
        }
        catch (std::exception &e)
        {
            AfxMessageBox(fmt::format("Unable to open resource map: {0}", e.what()).c_str(), MB_OK | MB_ICONWARNING);
            _gameFolderHelper.GameFolder = "";
            AfxThrowUserException();
        }
    }

    AbortDebuggerThread();

    _appState.OnGameFolderUpdate();
}

TalkerToViewMap &CResourceMap::GetTalkerToViewMap()
{
    return _talkerToView;
}

// Returns null if it doesn't exist.
std::unique_ptr<ResourceEntity> CResourceMap::CreateResourceFromNumber(ResourceType type, int number, uint32_t base36Number, int mapContext)
{
    std::unique_ptr<ResourceEntity> pResource;
    unique_ptr<ResourceBlob> data = MostRecentResource(type, number, true, base36Number, mapContext);
    // This can legitimately fail. For instance, a script that hasn't yet been compiled.
    if (data)
    {
        pResource = CreateResourceFromResourceData(*data);
    }
    return pResource;
}

template<typename TCreateFunc, typename TFallbackFunc>
std::unique_ptr<ResourceEntity> CreateResourceHelper(const ResourceBlob &data, TCreateFunc createFunction, TFallbackFunc fallbackFunc, bool fallbackOnException)
{
    std::unique_ptr<ResourceEntity> pResourceReturn(createFunction(data.GetVersion()));
    try 
    { 
        pResourceReturn->InitFromResource(&data); 
    } 
    catch (std::exception &theException)
    {
        if (!fallbackOnException)
        {
            throw;
        }
        // Just create the default one 
        appState->LogInfo(theException.what());
        data.AddStatusFlags(ResourceLoadStatusFlags::ResourceCreationFailed);
        pResourceReturn.reset(fallbackFunc(data.GetVersion()));
        pResourceReturn->ResourceNumber = data.GetNumber();
        pResourceReturn->PackageNumber = data.GetPackageHint();
    }
    return pResourceReturn;
}

void DoNothing(ResourceEntity &resource) {}

//
// Given a ResourceBlob, this creates the SCI resource represented by the data, and hands back
// a ResourceEntity.
// If there is an exception creating the resource, a default one is handed back.
//
std::unique_ptr<ResourceEntity> CreateResourceFromResourceData(const ResourceBlob &data, bool fallbackOnException)
{
    switch (data.GetType())
    {
        case ResourceType::View:
            return CreateResourceHelper(data, CreateViewResource, CreateDefaultViewResource, fallbackOnException);
        case ResourceType::Font:
            return CreateResourceHelper(data, CreateFontResource, CreateDefaultFontResource, fallbackOnException);
        case ResourceType::Cursor:
            return CreateResourceHelper(data, CreateCursorResource, CreateDefaultCursorResource, fallbackOnException);
        case ResourceType::Text:
            return CreateResourceHelper(data, CreateTextResource, CreateDefaultTextResource, fallbackOnException);
        case ResourceType::Sound:
            return CreateResourceHelper(data, CreateSoundResource, CreateDefaultSoundResource, fallbackOnException);
        case ResourceType::Vocab:
            return CreateResourceHelper(data, CreateVocabResource, CreateVocabResource, fallbackOnException);
        case ResourceType::Pic:
            return CreateResourceHelper(data, CreatePicResource, CreateDefaultPicResource, fallbackOnException);
        case ResourceType::Palette:
            return CreateResourceHelper(data, CreatePaletteResource, CreatePaletteResource, fallbackOnException);
        case ResourceType::Message:
            return CreateResourceHelper(data, CreateMessageResource, CreateDefaultMessageResource, fallbackOnException);
        case ResourceType::Audio:
            if (data.GetVersion().AudioIsWav && (data.GetBase36() == NoBase36))
            {
                return CreateResourceHelper(data, CreateWaveAudioResource, CreateDefaultAudioResource, fallbackOnException);
            }
            else
            {
                return CreateResourceHelper(data, CreateAudioResource, CreateDefaultAudioResource, fallbackOnException);
            }
        case ResourceType::AudioMap:
            return CreateResourceHelper(data, CreateMapResource, CreateMapResource, fallbackOnException);
        default:
        assert(false);
        break;
    }
    return nullptr;
}

void CResourceMap::SetGameLanguage(LangSyntax lang)
{
    Helper().SetIniString(GameSection, LanguageKey, (lang == LangSyntaxSCI) ? LanguageValueSCI : LanguageValueStudio);
    _gameFolderHelper.Language = lang;
    _SetGameLanguage();
}
