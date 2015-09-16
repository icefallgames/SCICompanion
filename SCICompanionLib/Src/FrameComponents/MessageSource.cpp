#include "stdafx.h"
#include "MessageSource.h"
#include "MessageHeaderFile.h"
#include "format.h"

using namespace std;

MessageSource::MessageSource(MessageHeaderFile *file) : _dirty(false), _file(file)
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
    if (index >= (int) _defines.size())
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
    if (index >= (int)_defines.size())
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

std::string MessageSource::GetBackingFile() const
{
    std::string path;
    if (_file)
    {
        path = _file->GetBackingFile();
    }
    return path;
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

void MessageSource::DeleteDefine(size_t index)
{
    if (index < _defines.size())
    {
        _defines.erase(_defines.begin() + index);
        _dirty = true;
    }
}

size_t MessageSource::AddDefine(const std::string &newName, uint16_t newValue)
{
    _defines.emplace_back(newName, newValue);
    _dirty = true;
    return _defines.size() - 1;
}

void MessageSource::Commit()
{
    if (_dirty)
    {
        _file->Commit();
        _dirty = false;
    }
}

std::unique_ptr<MessageHeaderFile> GetMessageFile(const std::string &messageFolder, int scriptNumber)
{
    string messageFilename = fmt::format("{0}.shm", scriptNumber);
    string messageFilePath = fmt::format("{0}\\{1}", messageFolder, messageFilename);
    return make_unique<MessageHeaderFile>(messageFilePath, messageFilename, initializer_list<string>({ "NOUNS", "CASES" }));
}