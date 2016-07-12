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

struct ToolTipResult
{
    ToolTipResult() : vocabWordInfo(0xffffffff), possibleResourceNumber(0xffff) {}

    // Is the tooltip empty (e.g. no result)
    bool empty() { return strTip.empty(); }

    // The tip
    std::string strTip;

    // Line number, text, and scriptid for "goto" information
    int iLineNumber;
    ScriptId scriptId;
    std::string strBaseText;
    std::string helpURL; // For kernels
    DWORD vocabWordInfo; // For saids
    uint16_t possibleResourceNumber;

    std::string OriginalText;
};
