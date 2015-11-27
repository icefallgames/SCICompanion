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

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

// trim from start
static inline std::string &ltrim(std::string &s, char c) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [c](char test) { return c != test; }));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s, char c) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [c](char test) { return c != test; }).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s, char c) {
    return ltrim(rtrim(s, c), c);
}

std::vector<std::string> Lineify(const std::string &in);
bool IsLineEmpty(const std::string &line);

// If all non-empty lines have a tab, removes the tab at the beginning of each line
template<char _TWhiteSpace>
std::string Unindent(const std::string &in, int *minTabCountOut = nullptr)
{
    std::vector<std::string> lines = Lineify(in);
    int minTabCount = (std::numeric_limits<int>::max)();
    for (auto &line : lines)
    {
        if (!IsLineEmpty(line))
        {
            int tabCount = std::find_if(line.begin(), line.end(), [](char c) { return c != _TWhiteSpace;  }) - line.begin();
            minTabCount = min(tabCount, minTabCount);
        }
    }

    std::string output;
    if ((minTabCount > 0) && (minTabCount != (std::numeric_limits<int>::max)()))
    {
        for (auto &line : lines)
        {
            if (!IsLineEmpty(line))
            {
                std::string lineUnindented(line.begin() + minTabCount, line.end());
                output += lineUnindented;
            }
            output += '\n';
        }
    }
    else
    {
        output = in;
    }

    if (minTabCount == (std::numeric_limits<int>::max)())
    {
        minTabCount = 0;
    }

    if (minTabCountOut)
    {
        *minTabCountOut = minTabCount;
    }

    return output;
}

std::string Indent(const std::string &in);
