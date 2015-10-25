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

#include "Components.h"
#include "MessageHeaderFile.h"
#include "MessageSource.h"

class NounsAndCasesComponent : public ResourceComponent
{
public:
    NounsAndCasesComponent(const std::string &msgFolder, int messageNumber);
    void Commit(int messageNumber);

    NounsAndCasesComponent *Clone() const override
    {
        return new NounsAndCasesComponent(*this);
    }

    MessageSource &GetNouns();
    const MessageSource &GetNouns() const;
    MessageSource &GetCases();
    const MessageSource &GetCases() const;

private:
    MessageHeaderFile _headerFile;

    std::string _GetMessageFile();
    std::string _msgFolder;
    int _messageNumber;
};