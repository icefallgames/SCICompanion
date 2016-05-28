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

enum class DecompilerResultType
{
    Update, // A minor update
    Important, // Important status
    Warning,
    Error
};

class IDecompilerResults
{
public:
    virtual void AddResult(DecompilerResultType type, const std::string &message) = 0;
    virtual bool IsAborted() = 0;
    virtual void InformStats(bool functionSuccessful, int byteCount) = 0;
    virtual void SetGlobalVarsUpdated(const std::vector<std::pair<std::string, std::string>> &mainDirtyRenames) = 0;
};