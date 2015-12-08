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

// Manages our different profiles for running the game from within SCI Companion.
// (e.g. DOSBox, ScummVM, custom).

class RunLogic
{
public:
    RunLogic();
    void SetGameFolder(const std::string &folder);

    bool RunGame(std::string &error, HANDLE &hProcess);

    std::vector<std::string> GetExecutableProfiles();
    void SetExecutableProfile(const std::string &option);
    std::string GetExecutableProfile();
    std::string GetExe(const std::string &profile);
    std::string GetParams(const std::string &profile);
    void SetExe(const std::string &profile, const std::string &exe);
    void SetParams(const std::string &profile, const std::string &params);

private:
    bool _WriteProfileString(const std::string &key, const std::string &value);
    std::string _ReadProfileString(const std::string &key, const std::string &defaultValue);

    std::string _gameFolder;
    std::string _gameIni;

    std::string _defaultProfile;

    std::unordered_map<std::string, std::string> _profileToExeKey;
    std::unordered_map<std::string, std::string> _profileToParamsKey;
    std::unordered_map<std::string, std::string> _profileToDefaultParams;
    std::unordered_map<std::string, std::string> _profileToDefaultExe;
};