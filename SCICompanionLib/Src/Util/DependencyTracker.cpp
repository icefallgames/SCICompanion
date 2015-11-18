#include "stdafx.h"
#include "DependencyTracker.h"
#include "ScriptOM.h"

// Simple dependency tracker for script's include and uses.

DependencyTracker::DependencyTracker(BOOL fTrackHeaderFiles) : _fTrackHeaderFiles(fTrackHeaderFiles) {}

void DependencyTracker::Clear()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _scriptToDependencies.clear();
    _dependenciesToScript.clear();
}

void DependencyTracker::_AppDependency(const std::string &scriptKey, const std::string &dependency)
{
    std::string depLower = dependency;
    std::transform(depLower.begin(), depLower.end(), depLower.begin(), tolower);
    _scriptToDependencies[scriptKey].push_back(depLower);
    _dependenciesToScript[depLower].push_back(scriptKey);
}

void DependencyTracker::ProcessScript(sci::Script &script)
{
    std::string scriptKey = script.GetScriptId().GetTitle();
    std::transform(scriptKey.begin(), scriptKey.end(), scriptKey.begin(), tolower);

    std::lock_guard<std::mutex> lock(_mutex);

    // Remove old uses and includes
    for (auto &uses : _scriptToDependencies[scriptKey])
    {
        // These are the things the script depends on. Remove the script from the list
        auto &scriptList = _dependenciesToScript[uses];
        scriptList.erase(std::remove(scriptList.begin(), scriptList.end(), scriptKey), scriptList.end());
    }
    _scriptToDependencies[scriptKey].clear();

    // Add in new ones
    for (auto &theInclude : script.GetIncludes())
    {
        _AppDependency(scriptKey, theInclude);
    }
    for (auto &theUse : script.GetUses())
    {
        _AppDependency(scriptKey, theUse);
    }
}

void DependencyTracker::NotifyScriptFileChanged(const ScriptId &scriptId)
{
    std::string key;
    if (scriptId.IsHeader())
    {
        key = scriptId.GetFileName();
    }
    else
    {
        key = scriptId.GetTitle();
        std::transform(key.begin(), key.end(), key.begin(), tolower);
        // In addition to other scripts needing to recompile, this one does too:
        _dirtyScripts.insert(key);
    }
    NotifyHeaderFileChanged(key);
}

void DependencyTracker::ClearScript(const ScriptId &scriptId)
{
    std::string key = scriptId.GetTitle();
    std::transform(key.begin(), key.end(), key.begin(), tolower);
    _dirtyScripts.erase(key);
}

void DependencyTracker::NotifyHeaderFileChanged(const std::string &filename)
{
    if (_fTrackHeaderFiles)
    {
        std::string headerLower = filename;
        std::lock_guard<std::mutex> lock(_mutex);
        std::transform(headerLower.begin(), headerLower.end(), headerLower.begin(), tolower);
        std::copy(_dependenciesToScript[headerLower].begin(), _dependenciesToScript[headerLower].end(), std::inserter(_dirtyScripts, _dirtyScripts.begin()));
    }
}

void DependencyTracker::GetScriptsToRecompile(std::unordered_set<std::string> &scriptsToRecompile, bool clear)
{
    scriptsToRecompile = _dirtyScripts;
    std::lock_guard<std::mutex> lock(_mutex);
    if (clear)
    {
        _dirtyScripts.clear();
    }
}