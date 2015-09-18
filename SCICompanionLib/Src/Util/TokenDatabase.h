#pragma once
#include "AutoCompleteContext.h"

class AutoCompleteChoice;

struct ACTreeLeaf
{
    ACTreeLeaf(AutoCompleteSourceType sourceType, std::string original);

    AutoCompleteSourceType SourceType;
    std::string Lower;
    std::string Original;
};

bool operator<(const ACTreeLeaf &one, const ACTreeLeaf &two);
bool operator==(const ACTreeLeaf &one, const ACTreeLeaf &two);

class TokenDatabase
{
public:
    void BuildDatabase(std::multiset<ACTreeLeaf> &originals);
    void GetAutoCompleteChoices(const std::string &prefix, AutoCompleteSourceType sourceTypes, std::vector<AutoCompleteChoice> &choices);

private:
    std::vector<ACTreeLeaf> _originals;
    sci::ostream _data;
    // [chars in common with prev][add'l char length][remaining chars][index into _originals (2 bytes)]
    std::unordered_map<char, size_t> _offsets;  // Into data
};