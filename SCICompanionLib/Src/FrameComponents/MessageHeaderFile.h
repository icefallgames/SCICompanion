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

#include "MessageSource.h"

class MessageHeaderFile
{
public:
    MessageHeaderFile(const std::string &filepath, const std::string &title, const std::vector<std::string> &sourcesOptional);
    MessageHeaderFile::MessageHeaderFile(const std::string &folderPath, int resourceNumber, const std::vector<std::string> &sourcesOptional);
    MessageHeaderFile() = default;
    MessageSource *GetMessageSource(const std::string &name);
    const MessageSource *MessageHeaderFile::GetMessageSource(const std::string &name) const;
    MessageSource *GetMessageSource();
    void Commit(int resourceNumber = -1);

    std::string GetBackingFile() const;

private:
    void _Load(const std::vector<std::string> &sourcesOptional);
    std::unordered_map<std::string, MessageSource> _sources;


    int _resourceNumber;
    std::string _folderPath;

    std::string _filePath;
    std::string _title;
};