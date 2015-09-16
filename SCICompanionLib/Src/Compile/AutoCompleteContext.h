#pragma once

enum class AutoCompleteSourceType
{
    None = 0,
    Define =                0x00000001,
    TopLevelKeyword =       0x00000002,
    Selector =              0x00000004,
    ClassName =             0x00000008,
    Variable =              0x00000010,
    ScriptName =            0x00000020,
    Kernel =                0x00000040,
    Procedure =             0x00000080,
    ClassSelector =         0x00000100,
    Instance =              0x00000200,
};
DEFINE_ENUM_FLAGS(AutoCompleteSourceType, int)

enum class AutoCompleteIconIndex
{
    Unknown = 0,
    Class = 0,
    Script = 1,
    Procedure = 2,
    PublicProcedure = 3,
    Kernel = 4,
    Variable = 5,
    Define = 6,
    Keyword = 8,
    TopLevelKeyword = 9,
    Selector = 10,
};