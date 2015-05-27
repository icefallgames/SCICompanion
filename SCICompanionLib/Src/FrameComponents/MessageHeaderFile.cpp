#include "stdafx.h"
#include "MessageHeaderFile.h"
#include "format.h"

using namespace std;

MessageHeaderFile::MessageHeaderFile(const std::string &filepath, const std::string &title, const std::vector<std::string> &sourcesOptional) : _filePath(filepath), _title(title)
{
    _Load(sourcesOptional);
}

const char c_szComment[] = "//";
const char c_szDefine[] = "(define";

void AdvancePastWhitespace(const std::string &line, size_t &offset)
{
    while (offset < line.size())
    {
        if (line[offset] == '\t' || line[offset] == ' ')
        {
            offset++;
        }
        else
        {
            break;
        }
    }
}

void AdvanceToWhitespace(const std::string &line, size_t &offset)
{
    while (offset < line.size())
    {
        if (line[offset] == '\t' || line[offset] == ' ')
        {
            break;
        }
        offset++;
    }
}

void MessageHeaderFile::_Load(const std::vector<std::string> &sourcesOptional)
{
    unique_ptr<MessageSource> currentSource;
    // For speed, we won't bother with our standard parser here. Also, we need comments.
    ifstream file;
    file.open(_filePath);
    string line;
    while (std::getline(file, line))
    {
        if (line.compare(0, ARRAYSIZE(c_szComment) - 1, c_szComment) == 0)
        {
            // Read tokens like CASES and NOUNS
            size_t offset = ARRAYSIZE(c_szComment);
            AdvancePastWhitespace(line, offset);
            size_t afterName = offset;
            AdvanceToWhitespace(line, afterName);
            string token = line.substr(offset, afterName - offset);
            if (find(sourcesOptional.begin(), sourcesOptional.end(), token) != sourcesOptional.end())
            {
                // Add any current source to the list
                if (currentSource)
                {
                    assert(_sources.find(currentSource->Name) == _sources.end());
                    _sources[currentSource->Name] = move(currentSource);
                }
                // And start a new one
                currentSource = make_unique<MessageSource>();
                currentSource->Name = token;
            }
        }
        else if (line.compare(0, ARRAYSIZE(c_szDefine) - 1, c_szDefine) == 0)
        {
            if (currentSource == nullptr)
            {
                currentSource = make_unique<MessageSource>();
            }

            size_t offset = ARRAYSIZE(c_szDefine);
            AdvancePastWhitespace(line, offset);
            size_t afterName = offset;
            AdvanceToWhitespace(line, afterName);
            string name = line.substr(offset, afterName - offset);
            AdvancePastWhitespace(line, afterName);
            // This should be a number
            string number = line.substr(afterName);
            size_t afterNumber;
            uint16_t value = (uint16_t)stoi(number, &afterNumber);
            // TODO: We could assert we find a closing bracket.

            currentSource->AddDefine(name, value);
        }
    }

    if (currentSource)
    {
        _sources[currentSource->Name] = move(currentSource);
    }
}

MessageSource *MessageHeaderFile::GetMessageSource(const std::string &name)
{
    // Note: this will create an empty one if none exists.
    return _sources[name].get();
}

MessageSource *MessageHeaderFile::GetMessageSource()
{
    assert(_sources.size() == 1);
    return _sources.begin()->second.get();
}