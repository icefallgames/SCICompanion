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
#include "NounsAndCases.h"
#include "format.h"

using namespace std;

NounsAndCasesComponent::NounsAndCasesComponent(const std::string &msgFolder, int messageNumber) : _messageNumber(messageNumber), _msgFolder(msgFolder),
    _headerFile(msgFolder, messageNumber, initializer_list<string>({ "NOUNS", "CASES" }))
{

}

MessageSource &NounsAndCasesComponent::GetNouns()
{
    return *_headerFile.GetMessageSource("NOUNS");
}
const MessageSource &NounsAndCasesComponent::GetNouns() const
{
    return *_headerFile.GetMessageSource("NOUNS");
}
MessageSource &NounsAndCasesComponent::GetCases()
{
    return *_headerFile.GetMessageSource("CASES");
}
const MessageSource &NounsAndCasesComponent::GetCases() const
{
    return *_headerFile.GetMessageSource("CASES");
}

// REVIEW: Why does it pass a nmssage number?
void NounsAndCasesComponent::Commit(int messageNumber)
{
    _messageNumber = messageNumber;
    _headerFile.Commit(messageNumber);
}
