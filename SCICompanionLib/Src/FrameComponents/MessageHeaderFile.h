#pragma once

#include "MessageSource.h"

class MessageHeaderFile
{
public:
    MessageHeaderFile(const std::string &filepath, const std::string &title, const std::vector<std::string> &sourcesOptional);
    MessageSource *GetMessageSource(const std::string &name);
    MessageSource *GetMessageSource();

private:
    void _Load(const std::vector<std::string> &sourcesOptional);
    std::unordered_map<std::string, std::unique_ptr<MessageSource>> _sources;
    std::string _filePath;
    std::string _title;
};