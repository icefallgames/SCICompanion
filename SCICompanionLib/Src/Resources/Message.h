#pragma once

#include "Components.h"

// We re-use TextComponent

enum class MessageChangeHint
{
    None = 0,
    Selection = 0x00000080,
    Changed = 0x00000100,
    NounsChanged = 0x00000200,
    ConditionsChanged = 0x00000400,
    VerbsChanged = 0x00000800,
    TalkersChanged = 0x00001000,
    AllMessageFiles = 0x00001e00
};

DEFINE_ENUM_FLAGS(MessageChangeHint, uint32_t)

ResourceEntity *CreateMessageResource(SCIVersion version);
ResourceEntity *CreateDefaultMessageResource(SCIVersion version);