#include "stdafx.h"
#include "TokenDatabase.h"
#include "CodeAutoComplete.h"

const size_t MaxWordLength = 255;

ACTreeLeaf::ACTreeLeaf(AutoCompleteSourceType sourceType, std::string original) : Original(original), SourceType(sourceType), Lower(original)
{
    std::transform(Lower.begin(), Lower.end(), Lower.begin(), ::tolower);
    if (Lower.size() > MaxWordLength)
    {
        Lower = Lower.substr(0, MaxWordLength);
    }
}

bool operator<(const ACTreeLeaf &one, const ACTreeLeaf &two)
{
    return one.Lower < two.Lower;
}


size_t CommonPrefix(const std::string &one, const std::string &two, bool &firstLetterSame)
{
    size_t length = min(one.size(), two.size());
    firstLetterSame = false;
    for (size_t i = 0; i < length; i++)
    {
        if (one[i] != two[i])
        {
            return i;
        }
        firstLetterSame = true;
    }
    firstLetterSame = (length > 0);
    return length;
}

void TokenDatabase::BuildDatabase(std::multiset<ACTreeLeaf> &originalsIn)
{
    _data.EnsureCapacity(20000);
    _data.reset();
    _originals.clear();

    std::string previous;
    for (const ACTreeLeaf &leaf : originalsIn)
    {
        // Count chars in common with previous
        bool firstLetterSame;
        size_t commonWithPrev = CommonPrefix(previous, leaf.Lower, firstLetterSame);
        if (!firstLetterSame)
        {
            // New entry in the letter table.
            _offsets[leaf.Lower[0]] = _data.GetDataSize();
        }

        _data.WriteByte((uint8_t)commonWithPrev);
        size_t currentSize = leaf.Lower.size();
        _data.WriteByte((uint8_t)(currentSize - commonWithPrev));
        for (size_t i = commonWithPrev; i < currentSize; i++)
        {
            _data.WriteByte((uint8_t)leaf.Lower[i]);
        }
        // Finally, the id.
        _data.WriteWord((uint16_t)_originals.size());
        _originals.push_back(leaf);

        previous = leaf.Lower;
    }
    _data.WriteByte(0); // 0 chars in common
    // Done
}

void TokenDatabase::GetAutoCompleteChoices(const std::string &prefix, AutoCompleteSourceType sourceTypes, std::vector<AutoCompleteChoice> &choices)
{
    if (!prefix.empty())
    {
        sci::istream stream = istream_from_ostream(_data);
        size_t prefixLength = prefix.size();
        auto itOffset = _offsets.find(prefix[0]);
        if (itOffset != _offsets.end())
        {
            size_t offset = itOffset->second;
            stream.seekg(offset);
            assert(0 == stream.peek()); // 0 chars in common with previous.
            uint8_t charsInCommon;
            stream >> charsInCommon;
            size_t matchingChars = 0; // This can only go up, then go down.

            char buffer[MaxWordLength];
            do
            {
                uint8_t additionalChars;
                stream >> additionalChars;

                stream.read_data(reinterpret_cast<uint8_t*>(buffer + charsInCommon), additionalChars);

                // Figure out how many characters match the prefix.
                size_t total = charsInCommon + additionalChars;
                while (matchingChars < prefixLength)
                {
                    if (buffer[matchingChars] == prefix[matchingChars])
                    {
                        matchingChars++;
                    }
                    else
                    {
                        break;
                    }
                }
                if (matchingChars == prefixLength)
                {
                    // This is a match, unless we have less now
                    if (buffer[matchingChars - 1] != prefix[matchingChars - 1])
                    {
                        // We're done. Nothing more will match
                        break;
                    }
                    uint16_t index;
                    stream >> index;

                    const ACTreeLeaf &leaf = _originals[index];
                    AutoCompleteSourceType type = leaf.SourceType;
                    if ((type & sourceTypes) != AutoCompleteSourceType::None)
                    {
                        AutoCompleteIconIndex icon = AutoCompleteIconIndex::Unknown;
                        switch (type)
                        {
                            case AutoCompleteSourceType::Define:
                                icon = AutoCompleteIconIndex::Define;
                                break;
                            case AutoCompleteSourceType::TopLevelKeyword:
                                icon = AutoCompleteIconIndex::TopLevelKeyword;
                                break;
                            case AutoCompleteSourceType::ClassName:
                                icon = AutoCompleteIconIndex::Class;
                                break;
                            case AutoCompleteSourceType::Selector:
                                icon = AutoCompleteIconIndex::Selector;
                                break;
                            case AutoCompleteSourceType::Procedure:
                                icon = AutoCompleteIconIndex::PublicProcedure;
                                break;
                            case AutoCompleteSourceType::Kernel:
                                icon = AutoCompleteIconIndex::Kernel;
                                break;
                            case AutoCompleteSourceType::ScriptName:
                                icon = AutoCompleteIconIndex::Script;
                                break;
                            case AutoCompleteSourceType::Variable:
                                icon = AutoCompleteIconIndex::Variable;
                                break;
                        }
                        choices.emplace_back(leaf.Original, leaf.Lower, icon);
                    }

                    

                }
                else
                {
                    stream.skip(sizeof(uint16_t));
                }

                
                stream >> charsInCommon;
            } while (charsInCommon);

            // Go through the entries until we have one where prefixLength chars are matching.
        }
    }
}
