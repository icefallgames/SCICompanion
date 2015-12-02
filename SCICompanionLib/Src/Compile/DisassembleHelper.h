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

class ILookupNames;

std::string _GetTempVariableName(int index);
std::string _GetLocalVariableName(int index, uint16_t scriptNum);
std::string _GetGlobalVariableName(int index);
std::string _GetParamVariableName(int index);

struct CodeSection
{
    uint16_t begin;
    uint16_t end;
};

bool FindStartEndCode(std::set<uint16_t>::const_iterator codeStart, const std::set<uint16_t> &codePointersTO, const std::vector<CodeSection> &codeSections, CodeSection &sectionOut);

std::string SaidSequenceToString(std::vector<uint16_t> saidSequence, const ILookupNames *pWords);
std::string _FindPreferredWord(const std::string &words);