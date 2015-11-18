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

namespace sci
{
    class Script;
}

class DependencyTracker
{
public:
    DependencyTracker(BOOL fTrackHeaderFiles);
    void ProcessScript(sci::Script &script);
    void NotifyHeaderFileChanged(const std::string &filename);
    void NotifyScriptFileChanged(const ScriptId &scriptId);
    void GetScriptsToRecompile(std::unordered_set<std::string> &scriptsToRecompile, bool clear);
    void ClearScript(const ScriptId &scriptId);

    void Clear();

private:
    void _Clear();
    void _AppDependency(const std::string &scriptKey, const std::string &dependency);

    // For header files (includes), the full filename is used (e.g. sci.sh)
    // For script files, just the use is used (e.g. controls)
    std::unordered_map<std::string, std::vector<std::string>> _scriptToDependencies;
    std::unordered_map<std::string, std::vector<std::string>> _dependenciesToScript;

    std::unordered_set<std::string> _dirtyScripts;

    std::mutex _mutex;

    BOOL &_fTrackHeaderFiles;
};