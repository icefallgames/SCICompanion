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
};
DEFINE_ENUM_FLAGS(AutoCompleteSourceType, int)

enum class AutoCompleteType
{
    None,
    Value,
    TopLevelKeyword,
    Selector,
    ClassName
};

class AutoCompleteContext
{
public:
    AutoCompleteContext(const std::string &text, AutoCompleteType type) : _text(text), _type(type)
    {
    }

    const std::string &GetText() { return _text; }
    AutoCompleteType GetType() { return _type; }

private:
    std::string _text;
    AutoCompleteType _type;
};