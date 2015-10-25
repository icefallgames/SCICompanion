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
#include "TalkerToViewMap.h"
#include "AppState.h"
#include "cpptoml.h"
#include "format.h"

using namespace std;
using namespace cpptoml;

TalkerToViewMap::TalkerToViewMap(const std::string &lipSyncFolder)
{
    _folder = lipSyncFolder;
    _filename = _folder + "\\talker_to_view.ini";

    try
    {
        ifstream file;
        file.open(_filename);
        if (file.is_open())
        {
            string line;
            while (std::getline(file, line))
            {
                size_t offset = 0;
                AdvancePastWhitespace(line, offset);
                string number = line.substr(offset);
                size_t afterNumber;
                uint16_t talkerNumber = (uint16_t)stoi(number, &afterNumber);
                offset += afterNumber;
                AdvancePastWhitespace(line, offset);
                if (offset < line.size())
                {
                    if (line[offset] == '=')
                    {
                        ++offset;

                        AdvancePastWhitespace(line, offset);
                        number = line.substr(offset);
                        size_t afterNumber;
                        uint16_t viewNumber = (uint16_t)stoi(number, &afterNumber);
                        offset += afterNumber;
                        AdvancePastWhitespace(line, offset);
                        number = line.substr(offset);
                        uint16_t loopNumber = (uint16_t)stoi(number, &afterNumber);

                        _talkerToViewLoop[talkerNumber] = std::pair<uint16_t, uint16_t>(viewNumber, loopNumber);
                    }
                }
            }
        }
    }
    catch (...)
    {

    }
}

bool TalkerToViewMap::TalkerToViewLoop(uint16_t talker, uint16_t &view, uint16_t &loop) const
{
    auto it = _talkerToViewLoop.find(talker);
    if (it != _talkerToViewLoop.end())
    {
        view = it->second.first;
        loop = it->second.second;
        return true;
    }
    return false;
}

void TalkerToViewMap::SetTalkerToViewLoop(uint16_t talker, uint16_t view, uint16_t loop)
{
    uint16_t currentView, currentLoop;
    if (TalkerToViewLoop(talker, currentView, currentLoop))
    {
        if ((view == currentView) && (loop == currentLoop))
        {
            return; // Nothing to change.
        }
    }

    _talkerToViewLoop[talker] = std::pair<uint16_t, uint16_t>(view, loop);

    // Save it
    if (!EnsureFolderExists(_folder, false))
    {
        return;
    }

    std::ofstream file;
    file.open(_filename, ios_base::out | ios_base::trunc);
    if (file.is_open())
    {
        for (auto &entry : _talkerToViewLoop)
        {
            file << fmt::format("{0} = {1} {2}\n", entry.first, entry.second.first, entry.second.second);
        }
    }
}
