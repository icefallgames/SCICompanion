#pragma once

#include "MessageSource.h"

class MessageHeaderFile
{
public:
    MessageHeaderFile(const std::string &filepath, const std::string &title, const std::vector<std::string> &sourcesOptional);
    MessageHeaderFile::MessageHeaderFile(const std::string &folderPath, int resourceNumber, const std::vector<std::string> &sourcesOptional);
    MessageHeaderFile() = default;
    MessageSource *GetMessageSource(const std::string &name);
    const MessageSource *MessageHeaderFile::GetMessageSource(const std::string &name) const;
    MessageSource *GetMessageSource();
    void Commit(int resourceNumber = -1);

    std::string GetBackingFile() const;

private:
    void _Load(const std::vector<std::string> &sourcesOptional);
    std::unordered_map<std::string, MessageSource> _sources;


    int _resourceNumber;
    std::string _folderPath;

    std::string _filePath;
    std::string _title;
};