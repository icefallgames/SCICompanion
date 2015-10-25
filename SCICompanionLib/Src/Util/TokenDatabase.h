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
#include "AutoCompleteContext.h"

class AutoCompleteChoice;

struct ACTreeLeaf
{
    ACTreeLeaf(AutoCompleteSourceType sourceType, std::string original);

    AutoCompleteSourceType SourceType;
    std::string Lower;
    std::string Original;
};

bool operator<(const ACTreeLeaf &one, const ACTreeLeaf &two);
bool operator==(const ACTreeLeaf &one, const ACTreeLeaf &two);

class TokenDatabase
{
public:
    void BuildDatabase(std::multiset<ACTreeLeaf> &originals);
    void GetAutoCompleteChoices(const std::string &prefix, AutoCompleteSourceType sourceTypes, std::vector<AutoCompleteChoice> &choices);

private:
    std::vector<ACTreeLeaf> _originals;
    sci::ostream _data;
    // [chars in common with prev][add'l char length][remaining chars][index into _originals (2 bytes)]
    std::unordered_map<char, size_t> _offsets;  // Into data
};