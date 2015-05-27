#include "stdafx.h"
#include "MessageSource.h"

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