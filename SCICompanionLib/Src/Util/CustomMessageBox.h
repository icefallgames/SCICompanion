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

class MessageBoxCustomization
{
public:
    MessageBoxCustomization() = delete;
    MessageBoxCustomization(const MessageBoxCustomization &src) = delete;
    MessageBoxCustomization(const std::vector<std::pair<int, std::string>> &subs);
    ~MessageBoxCustomization();

    std::vector<std::pair<int, std::string>> Substitutions;
    
    // TODO: Add more as needed
    static const int Yes = 6;
    static const int No = 7;

private:
    bool _unhook;
};