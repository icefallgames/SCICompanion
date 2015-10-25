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

class TalkerToViewMap
{
public:
    TalkerToViewMap() {}
    TalkerToViewMap(const std::string &talker);
    bool TalkerToViewLoop(uint16_t talker, uint16_t &view, uint16_t &loop) const;
    void SetTalkerToViewLoop(uint16_t talker, uint16_t view, uint16_t loop);

private:
    std::string _filename;
    std::string _folder;
    std::unordered_map<uint16_t, std::pair<uint16_t, uint16_t>> _talkerToViewLoop;
};