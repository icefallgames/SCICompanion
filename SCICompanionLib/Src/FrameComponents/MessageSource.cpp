#include "stdafx.h"
#include "MessageSource.h"

using namespace std;

MessageSource::MessageSource() : _dirty(false)
{

}

void MessageSource::SetName(size_t index, const std::string &newName)
{
    // TODO: check it doesn't already exist
    _defines[index].first = newName;
    _dirty = true;
}

void MessageSource::SetValue(size_t index, uint16_t newValue)
{
    // TODO: check it doesn't already exist
    _defines[index].second = newValue;
    _dirty = true;
}

int MessageSource::IndexOf(const std::string &name) const
{
    auto it = find_if(_defines.begin(), _defines.end(),
        [&name](const MessageDefine &define) { return name == define.first; });
    int index = it - _defines.begin();
    if (index > (int) _defines.size())
    {
        index = -1;
    }
    return index;
}

int MessageSource::IndexOf(uint16_t value) const
{
    auto it = find_if(_defines.begin(), _defines.end(),
        [value](const MessageDefine &define) { return value == define.second; });
    int index = it - _defines.begin();
    if (index > (int)_defines.size())
    {
        index = -1;
    }
    return index;
}

std::string MessageSource::ValueToName(uint16_t value) const
{
    auto it = find_if(_defines.begin(), _defines.end(),
        [value](const MessageDefine &define) { return value == define.second; });
    if (it != _defines.end())
    {
        return it->first;
    }
    return "";
}

uint16_t MessageSource::NameToValue(const std::string &name) const
{
    auto it = find_if(_defines.begin(), _defines.end(),
        [&name](const MessageDefine &define) { return name == define.first; });
    if (it != _defines.end())
    {
        return it->second;
    }
    return 0;
}

size_t MessageSource::AddDefine(const std::string &newName, uint16_t newValue)
{
    _defines.emplace_back(newName, newValue);
    return _defines.size() - 1;
}

void MessageSource::Commit()
{
    if (_dirty)
    {
        _dirty = false;
    }
}