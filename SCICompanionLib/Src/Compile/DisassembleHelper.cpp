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
#include "DisassembleHelper.h"
#include "interfaces.h"
#include "PMachine.h"

using namespace std;


string _GetVariableName(const char *format, int index)
{
    char buff[100];
    sprintf_s(buff, format, index);
    return string(buff);
}
string _GetTempVariableName(int index)
{
    return _GetVariableName("temp%d", index);
}
string _GetLocalVariableName(int index, uint16_t scriptNum)
{
    return (scriptNum == 0) ?
        _GetGlobalVariableName(index) :
    _GetVariableName("local%d", index);
}
string _GetGlobalVariableName(int index)
{
    return _GetVariableName("global%d", index);
}
string _GetParamVariableName(int index)
{
    return _GetVariableName("param%d", index);
}

bool _IsPreferredWord(const std::string &word)
{
    return ((word == "look") ||
        (word == "get") ||
        (word == "talk") ||
        (word == "climb") ||
        (word == "man")
        );

}

const char c_szSeparator[] = " | ";

//
// Returns the preferred word in a " | " separated string.
// e.g. returns "get" for "acquire | get | obtain"
//
std::string _FindPreferredWord(const std::string &words)
{
    std::string word;
    size_t iStart = 0;
    size_t iEndOfFirst = words.find(c_szSeparator, iStart);
    while (iEndOfFirst != string::npos)
    {
        // Guess it's a loner
        if (_IsPreferredWord(words.substr(iStart, iEndOfFirst - iStart)))
        {
            word = words.substr(iStart, iEndOfFirst - iStart);
            break;
        }
        else if (word.empty())
        {
            word = words.substr(iStart, iEndOfFirst - iStart); // Use the first one otherwise.
        }
        iStart = iEndOfFirst + strlen(c_szSeparator);
        iEndOfFirst = words.find(c_szSeparator, iStart);
    }
    if (word.empty())
    {
        word = words; // loner
    }
    return word;
}

string SaidSequenceToString(vector<uint16_t> saidSequence, const ILookupNames *pWords)
{
    string strRet = "";
    for (size_t i = 0; i < saidSequence.size(); i++)
    {
        uint16_t wOp = saidSequence[i];
        if ((wOp >= 0x00f0) && (wOp <= 0x00f9))
        {
            strRet += SaidArgs[wOp - 0xf0].Character;
        }
        else
        {
            // Must be a word.
            std::string strWords = pWords->Lookup(wOp);
            // This is a little hokey -> this is " | " separated.  Let's just take the first word.
            strRet += _FindPreferredWord(strWords);
        }
    }
    return strRet;
}


//
// Returns the bounds of a function starting at codeStart, in terms of a CodeSection
// codeStart is an iterator into codePointersTo, which is a list of beginnings of functions, methods, etc...
//
bool FindStartEndCode(set<uint16_t>::const_iterator codeStart, const set<uint16_t> &codePointersTO, const vector<CodeSection> &codeSections, CodeSection &sectionOut)
{
    sectionOut.begin = *codeStart;
    // That was easy.  Now find the CodeSection this is in.
    CodeSection containingSection;
    bool fFound = false;
    for (size_t i = 0; !fFound && i < codeSections.size(); i++)
    {
        if ((sectionOut.begin >= codeSections[i].begin) && (sectionOut.begin < codeSections[i].end))
        {
            fFound = true;
            containingSection = codeSections[i];
        }
    }
    if (fFound)
    {
        ++codeStart;
        if (codeStart != codePointersTO.end())
        {
            sectionOut.end = min(*codeStart, containingSection.end);
        }
        else
        {
            sectionOut.end = containingSection.end;
        }
    }
    return fFound;
}