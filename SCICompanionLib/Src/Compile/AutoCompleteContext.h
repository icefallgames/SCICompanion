#pragma once

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