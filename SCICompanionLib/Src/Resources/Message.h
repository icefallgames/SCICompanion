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

struct TextComponent;

template<typename T>
uint32_t GetMessageTuple(const T& entry)
{
    uint32_t tuple = entry.Condition | (entry.Noun << 8) | (entry.Verb << 16) | (entry.Sequence << 24);
    return tuple;
}

// SSVVNNCC

template<typename T>
void SetMessageTuple(T& entry, uint32_t base36Number)
{
    entry.Condition = base36Number & 0xff;
    entry.Noun = (base36Number >> 8) & 0xff;
    entry.Verb = (base36Number >> 16) & 0xff;
    entry.Sequence = (base36Number >> 24) & 0xff;
}

// We re-use TextComponent

enum class MessageChangeHint
{
    None = 0,
    ItemChanged = 0x00000040,           // Something about a particular item changed
    Selection = 0x00000080,         // Current selection changed
    Changed = 0x00000100,           // The resource overall changed (e.g. items added or removed)
    NounsChanged = 0x00000200,
    ConditionsChanged = 0x00000400,
    VerbsChanged = 0x00000800,
    TalkersChanged = 0x00001000,
    AllMessageFiles = 0x00001e00
};

DEFINE_ENUM_FLAGS(MessageChangeHint, uint32_t)

ResourceEntity *CreateMessageResource(SCIVersion version);
ResourceEntity *CreateDefaultMessageResource(SCIVersion version);
ResourceEntity *CreateNewMessageResource(SCIVersion version, uint16_t msgVersion);
uint16_t CheckMessageVersion(sci::istream &byteStream);
void ExportMessageToFile(const TextComponent &message, const std::string &filename);
void ImportMessageFromFile(TextComponent &message, const std::string &filename);