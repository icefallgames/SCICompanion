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
#include "GameFolderHelper.h"
#include "ResourceEntity.h"
#include "TalkerToViewMap.h"

class RunLogic;
class DebuggerThread;
class PostBuildThread;
struct Vocab000;
struct AudioMapComponent;
struct PaletteComponent;
class MessageSource;
class MessageHeaderFile;
class AppState;

// FWD declaration
class ResourceContainer;
class ResourceBlob;
class ResourceRecency;
class ResourceEntity;
class GlobalCompiledScriptLookups;
class IResourceMapEvents;
enum class ResourceSaveLocation : uint16_t;

//
// REVIEW: CResourceMap needs to be protected with a critical section
//

class ISCIAppServices
{
public:
    virtual void OnGameFolderUpdate() = 0;
    virtual void SetRecentlyInteractedView(int number) = 0;
};


//
// This lets you operate on anything involving the resource.map, and is basically
// where we go to learn information about the currently loaded game.
//
class CResourceMap
{
public:
    CResourceMap(ISCIAppServices *appServices, ResourceRecency *resourceRecency);
    ~CResourceMap();

    RunLogic &GetRunLogic();

    // ResourceBlob: the raw resource bits already in a ready-to-save format.
    // ResourceEntity: a runtime version of a resource that we can edit.
    HRESULT AppendResource(const ResourceBlob &resource);
    HRESULT AppendResourceAskForNumber(ResourceBlob &resource, bool warnOnOverwrite);
    void AppendResourceAskForNumber(ResourceEntity &resource);
    void AppendResourceAskForNumber(ResourceEntity &resource, const std::string &name, bool warnOnOverwrite = false);
    bool AppendResource(const ResourceEntity &resource, int *pChecksum = nullptr);
    bool AppendResource(const ResourceEntity &resource, int packageNumber, int resourceNumber, const std::string &name, uint32_t base36Header = NoBase36, int *pChecksum = nullptr);

    int SuggestResourceNumber(ResourceType type);
    void AssignName(const ResourceBlob &resource);
    void AssignName(ResourceType iType, int iResourceNumber, uint32_t base36Number, PCTSTR pszName);

    ResourceSaveLocation GetDefaultResourceSaveLocation();

    // The main functions for enumerating resources.
    std::unique_ptr<ResourceContainer> Resources(ResourceTypeFlags types, ResourceEnumFlags flags, int mapContext = -1);
    std::unique_ptr<ResourceBlob> MostRecentResource(ResourceType type, int number, bool getName, uint32_t base36Number = NoBase36, int mapContext = -1);
    bool DoesResourceExist(ResourceType type, int number, std::string *retrieveName = nullptr, ResourceSaveLocation location = ResourceSaveLocation::Default) const;

    void AddSync(IResourceMapEvents *pSync);
    void RemoveSync(IResourceMapEvents *pSync);
    void NotifyToReloadResourceType(ResourceType iType);
    void NotifyToRegenerateImages();

    void DeleteResource(const ResourceBlob *pResource);

    void SetGameFolder(const std::string &gameFolder);

    TalkerToViewMap &GetTalkerToViewMap();

    std::string GetGameFolder() const;
    std::string GetIncludeFolder();
    std::string GetIncludePath(const std::string &includeFileName);
    std::string GetTemplateFolder();
    std::string GetSamplesFolder();
    std::string GetTopLevelSamplesFolder();
    std::string GetObjectsFolder();
    std::string GetDecompilerFolder();
    bool IsGameLoaded() { return !_gameFolderHelper.GameFolder.empty(); }
    HRESULT GetGameIni(PTSTR pszBuf, size_t cchBuf);
    HRESULT GetScriptNumber(ScriptId script, WORD &wScript);
    const SCIVersion &GetSCIVersion() const;
    void SetVersion(const SCIVersion &version);
    const GameFolderHelper &Helper() const { return _gameFolderHelper; }
    const Vocab000 *GetVocab000();
    const PaletteComponent *GetPalette999();
    void SaveAudioMap65535(const AudioMapComponent &newAudioMap, int mapContext);
    GlobalCompiledScriptLookups *GetCompiledScriptLookups();
    std::vector<int> GetPaletteList();
    std::unique_ptr<PaletteComponent> GetPalette(int fallbackPalette);
    std::unique_ptr<PaletteComponent> GetMergedPalette(const ResourceEntity &resource, int fallbackPalette);
    ResourceEntity *GetVocabResourceToEdit();
    void ClearVocab000();
    std::unique_ptr<ResourceEntity> CreateResourceFromNumber(ResourceType type, int wNumber, uint32_t base36Number = NoBase36, int mapContext = -1);
    void GetAllScripts(std::vector<ScriptId> &scripts);
	void GetNumberToNameMap(std::unordered_map<WORD, std::string> &scos);
    void SetGameLanguage(LangSyntax language);
    void SetIncludeFolderForTest(const std::string &folder) { _includeFolderOverride = folder; }
    bool CanSaveResourcesToMap();
    void SkipNextVersionSniff() { _skipVersionSniffOnce = true; }

    MessageSource *GetVerbsMessageSource(bool reload = false);
    MessageSource *GetTalkersMessageSource(bool reload = false);

    bool IsResourceCompatible(const ResourceBlob &resource);

    void StartDebuggerThread(int optionalResourceNumber);
    void AbortDebuggerThread();

    void StartPostBuildThread();
    void AbortPostBuildThread();
    void PokeResourceMapReloaded();

    void RepackageAudio(bool force = false);

private:
    void _SniffGameLanguage();
    void _SniffSCIVersion();

    void BeginDeferAppend();
    HRESULT EndDeferAppend();
    void AbandonAppend();
    friend class DeferResourceAppend;

    // Member variables

    TalkerToViewMap _talkerToView;

    std::vector<IResourceMapEvents*> _syncs;

    ResourceRecency *_resourceRecency;
    ISCIAppServices *_appServices;

    // Useful resources to cache
    std::unique_ptr<ResourceEntity> _pVocab000;
    std::unique_ptr<ResourceEntity> _pPalette999;
    std::unique_ptr<PaletteComponent> _emptyPalette;
    std::vector<int> _paletteList;
    bool _paletteListNeedsUpdate;

    std::unique_ptr<MessageHeaderFile> _verbsHeaderFile;
    std::unique_ptr<MessageHeaderFile> _talkersHeaderFile;


    std::unique_ptr<GlobalCompiledScriptLookups> _globalCompiledScriptLookups;

    // Defer appending resources when you are appending a lot (E.g. during compiling).
    BOOL _cDeferAppend;
    std::vector<ResourceBlob> _deferredResources;

    GameFolderHelper _gameFolderHelper;

    bool _skipVersionSniffOnce;                     // Skip version sniffing when loading a game the next time.

    std::string _includeFolderOverride;             // For unit-testing

    std::shared_ptr<DebuggerThread> _debuggerThread;
    std::shared_ptr<PostBuildThread> _postBuildThread;

    std::unique_ptr<RunLogic> _runLogic;
};

//
// Defer the actual writing of resources so it happens in one big batch at the end.
//
class DeferResourceAppend
{
public:
    DeferResourceAppend(CResourceMap &map, bool fDoIt = true) : _map(map)
    {
        if (fDoIt)
        {
            _map.BeginDeferAppend();
        }
        _fDoIt = fDoIt;
    }
    HRESULT Commit()
    {
        if (_fDoIt)
        {
            return _map.EndDeferAppend();
        }
        else
        {
            return S_OK;
        }
    }
    ~DeferResourceAppend()
    {
        if (_fDoIt)
        {
            _map.AbandonAppend();
        }
    }
private:
    CResourceMap &_map;
    bool _fDoIt;
};
