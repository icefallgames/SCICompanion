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

class AppState;

class PhonemeMap
{
public:
    PhonemeMap(const std::string &filename);
    
    uint16_t PhonemeToCel(const std::string &phoneme) const;
    void SetCel(const std::string &phoneme, uint16_t cel);
    const std::string &GetErrors() const { return _errors; }
    const std::string &GetFilespec() const { return _filespec; }
    const std::string &GetFileContents() const { return _fileContents; }
    bool IsEmpty() const;
    
    bool HasErrors() const { return !_errors.empty(); }

    const std::map<std::string, int> &Entries() const { return _phonemeToCel; }

private:
    std::map<std::string, int> _phonemeToCel;

    std::string _errors;
    std::string _filespec;
    std::string _fileContents;
};

bool SaveForViewLoop(const PhonemeMap &map, AppState *appState, int view, int loop, std::string &errors);
std::unique_ptr<PhonemeMap> LoadPhonemeMapForViewLoop(AppState *appState, int view, int loop);
std::string GetPhonemeMapPath(AppState *appState, int view, int loop);
std::string GetPhonemeMapFilespec(AppState *appState, int view, int loop);
