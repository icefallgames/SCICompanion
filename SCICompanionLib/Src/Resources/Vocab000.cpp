/***************************************************************************
    Copyright (c) 2015 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/

#include "stdafx.h"
#include "Vocab000.h"
#include "AppState.h"
#include "ResourceEntity.h"

using namespace std;
using namespace stdext;


bool IsValidVocabChar(TCHAR ch)
{
    return (((ch >= TEXT('a')) && (ch <= TEXT('z'))) || ((ch >= TEXT('A')) && (ch <= TEXT('Z'))) ||
            ((ch >= TEXT('0')) && (ch <= TEXT('9'))));
}

bool IsValidVocabString(PCTSTR pszWord, bool fShowUI)
{
    bool fRet = true;
    size_t cChars = lstrlen(pszWord);
    for (size_t i = 0; i < cChars; i++)
    {
        fRet = IsValidVocabChar(pszWord[i]);
        if (!fRet)
        {
            break;
        }
    }

    if (!fRet && fShowUI)
    {
        TCHAR szBuffer[MAX_PATH];
        StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), TEXT("Invalid word: %s.\nWords must contain only letters or numbers."), pszWord);
        AfxMessageBox(szBuffer, MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
    }

    return fRet;
}

PCTSTR FindValidVocabStringFromRight(PCTSTR pszWord)
{
    PCTSTR pszValid = nullptr;
    int cChars = static_cast<int>(lstrlen(pszWord));
    for (int i = (cChars - 1); i >= 0; i--)
    {
        if (IsValidVocabChar(pszWord[i]))
        {
            pszValid = &pszWord[i];
        }
        else
        {
            break;
        }
    }
    return pszValid;
}

WordClass GetWordClass(DWORD dwInfo) { return static_cast<WordClass>((dwInfo & 0x00FFF000) >> 12); }
Vocab000::WordGroup GetWordGroup(DWORD dwInfo) { return (dwInfo & 0x00000FFF); }
DWORD InfoFromClassAndGroup(WordClass dwClass, Vocab000::WordGroup dwGroup) { return dwGroup | (((uint32_t)dwClass) << 12); }

const TCHAR *g_rgszWordClasses[] =
{
    { TEXT("Number") },
    { TEXT("") },
    { TEXT("") },
    { TEXT("") },
    { TEXT("Preposition") },
    { TEXT("Article") },
    { TEXT("Qualifying Adjective") },
    { TEXT("Relative Pronoun") },
    { TEXT("Noun") },
    { TEXT("Indicative Verb") },
    { TEXT("Adverb") },
    { TEXT("Imperative Verb") },
};

void GetWordClassString(WordClass dwClass, std::string &str)
{
    for (int i = 0; i < ARRAYSIZE(g_rgszWordClasses); i++)
    {
        if ((int)dwClass & (0x1 << i))
        {
            if (!str.empty())
            {
                str += TEXT(" | ");
            }
            str += g_rgszWordClasses[i];
        }
    }
}

Vocab000::Vocab000()
{
    memset(_rgbGroups, 0x00, sizeof(_rgbGroups));
}

void Vocab000::_ReadWord(sci::istream &byteStream, char *pszBuffer, size_t cchBuffer, bool is900)
{
    uint8_t bCopyCount;
    byteStream >> bCopyCount;
    // Read the word first
    if (strlen(pszBuffer) >= bCopyCount)
    {
        // Move to the part after the letters that are the same between this and the last word.
        TCHAR *pszStartHere = pszBuffer + bCopyCount;
        cchBuffer -= bCopyCount;

        uint8_t bChar;
        while ((cchBuffer > 0) && (byteStream >> bChar).good())
        {
            *pszStartHere = is900 ? bChar : (0x7f & bChar);
            pszStartHere++;
            cchBuffer--;
            if (!is900 && (bChar & 0x80))
            {
                // This was the last char.
                if (cchBuffer > 0)
                {
                    *pszStartHere = 0; // Null terminate.
                }
                else
                {
                    // Ran over our buffer length.
                    // TODO: throw exception?
                }
                break;
            }
            else if (is900 && !bChar)
            {
                // This was the last char
                break;
            }
        }
    }
    else
    {
        return; // Bad data in the file.
    }

    if (byteStream.has_more_data())
    {
        // Ok, we have a word in our buffer.  Time to read the group and class
        DWORD dwInfo = 0;
        uint8_t b;
        byteStream >> b;
        dwInfo |= (((DWORD)b) << 16);
        byteStream >> b;
        dwInfo |= (((DWORD)b) << 8);
        byteStream >> b;
        if (byteStream.good())
        {
            dwInfo |= b;

            // Some vocabs have duplicate words (KQ1SCI, QFG2). Our Vocab resource format doesn't handle this properly, so ignore
            // this word if that's the case.
            if (_mapWordToGroup.find(pszBuffer) == _mapWordToGroup.end())
            {
                _words.push_back(pszBuffer);

                // Keep track of which groups have been used.
                Vocab000::WordGroup dwGroup = GetWordGroup(dwInfo);
                assert(dwGroup < ARRAYSIZE(_rgbGroups));
                _rgbGroups[dwGroup] = 0xff;

                // Now we have our class.
                _mapWordToGroup[pszBuffer] = dwGroup;
                _mapGroupToClass[dwGroup] = GetWordClass(dwInfo);

                group2words_map::iterator group2wordIt = _mapGroupToString.find(dwGroup);
                if (group2wordIt != _mapGroupToString.end())
                {
                    // Already exists.. tack it on
                    _mapGroupToString[dwGroup] = (group2wordIt->second + " | " + pszBuffer);
                }
                else
                {
                    // New one...
                    _mapGroupToString[dwGroup] = pszBuffer;
                }
            }
            else
            {
                // TODO: Possibly warn the user?
            }
        }
    }
}

void Vocab000::EnumGroups(groups_iterator &position, WordGroup &dwGroup, WordClass &dwClass, std::string &strWords) const
{
    dwGroup = position->first;
    strWords = position->second;
    dwClass = _mapGroupToClass.find(dwGroup)->second;
    ++position;
}

std::string Vocab000::GetGroupWordString(WordGroup dwGroup) const
{
    std::string strRet;
    group2words_map::const_iterator it = _mapGroupToString.find(dwGroup);
    if (it != _mapGroupToString.end())
    {
        strRet = _mapGroupToString.find(dwGroup)->second;
    }
    return strRet;
}

int CountMatchingCharacters(PCTSTR psz1, PCTSTR psz2)
{
    int cMatched = 0;
    while (*psz1 && (*psz1 == *psz2))
    {
        cMatched++;
        psz1++;
        psz2++;
    }
    return cMatched;
}

bool Vocab000::LookupWord(const std::string &word, WordGroup &dwGroup) const
{
    word2group_map::const_iterator it = _mapWordToGroup.lower_bound(word);
    if (it != _mapWordToGroup.end())
    {
        dwGroup = it->second;
        return true;
    }
    else
    {
        return false;
    }
}

Vocab000::WordGroup Vocab000::GroupFromString(PCTSTR pszString) const
{
    std::string strLower = pszString;
    std::transform(strLower.begin(), strLower.end(), strLower.begin(), 
                   (int(*)(int)) tolower);

    PCTSTR pszLower = strLower.c_str();
    // Try for an exact match, before searching.
    WordGroup dwGroup = { 0 };
    if (!LookupWord(strLower, dwGroup))
    {
        // This matches an incomplete string to the most likely group.
        // First we'll find the best matching word by looking through the alphabetically
        // ordered _words array, using a binary search.
        size_t iLowerBound = 0;
        size_t cWords = _words.size();
        size_t iCurrent = 0;
        int cMaxLettersMatched = 0;
        size_t iBest = (size_t)-1;
        int cCurrentMatched = 0;
        while (iLowerBound <= cWords)
        {
            size_t iPrev = iCurrent;
            iCurrent = (cWords + iLowerBound) / 2;

            int iComp = lstrcmp(pszLower, _words[iCurrent].c_str());
            cCurrentMatched = CountMatchingCharacters(pszLower, _words[iCurrent].c_str());
            if (cCurrentMatched > cMaxLettersMatched)
            {
                iBest = iCurrent;
                cMaxLettersMatched = cCurrentMatched;
            }
            if (iComp > 0)
            {
                // Look higher.
                iLowerBound = iCurrent + 1;
            }
            else if (iComp < 0)
            {
                // Look lower
                cWords = iCurrent - 1;
            }
            else
            {
                // Found an exact match!
                break;
            }
        }

        // If we already passed a better word, then use it.
        if (cMaxLettersMatched > cCurrentMatched)
        {
            assert(iBest != (size_t)-1);
            iCurrent = iBest;
        }

        // iCurrent is our best match.
        LookupWord(_words[iCurrent], dwGroup);

    }
    return dwGroup;
}

std::string Vocab000::Lookup(uint16_t wIndex) const
{
    group2words_map::const_iterator it = _mapGroupToString.find((WordGroup)wIndex);
    if (it != _mapGroupToString.end())
    {
        return it->second;
    }
    else
    {
        return "--invalid--";
    }
}

Vocab000::WordGroup Vocab000::_FindLargestEmptyGroup()
{
    DWORD dw = 1;
    // Start at 1.  Group 000 is not used.
    for (; dw < ARRAYSIZE(_rgbGroups); dw++)
    {
        if (_rgbGroups[dw] == 0)
        {
            break;
        }
    }
    WordGroup dwGroup = { dw };
    return dwGroup;
}

VocabChangeHint Vocab000::AddNewWord(PCTSTR pszWordIn, WordClass dwClass, BOOL fShowUI)
{
    std::string strLower = pszWordIn;
    transform(strLower.begin(), strLower.end(), strLower.begin(), tolower);
    VocabChangeHint hint = VocabChangeHint::None;
    TCHAR szBuffer[MAX_PATH];
    szBuffer[0] = 0;

    WordGroup dwGroupCurrent;
    if (!LookupWord(strLower, dwGroupCurrent))
    {
        hint = VocabChangeHint::Changed;
        Vocab000::WordGroup dwGroup = _FindLargestEmptyGroup();
        DWORD dwInfo = InfoFromClassAndGroup(dwClass, dwGroup);
        _rgbGroups[dwGroup] = 0xff;
        _InsertWord(strLower.c_str(), dwGroup);
        _mapGroupToClass[dwGroup] = dwClass;
        _FixupGroupToString(dwGroup);
    }
    else
    {
        StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), TEXT("Failed to add \"%s\": Word already exists!"), pszWordIn);
    }

    if (hint == VocabChangeHint::None)
    {
        assert(szBuffer[0]);
        AfxMessageBox(szBuffer, MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
    }

    return hint;
}

void Vocab000::_InsertWord(PCTSTR pszWord, WordGroup dwGroup)
{
    _mapWordToGroup[pszWord] = dwGroup;
    
    // Insert it into the ordered _words array, at the correct spot.
    vector<string>::iterator wordIt = _words.begin();
    for (; wordIt != _words.end(); ++wordIt)
    {
        if (lstrcmp(pszWord, wordIt->c_str()) < 0)
        {
            // We're done.  Insert it here.
            break;
        }
    }
    _words.insert(wordIt, pszWord);
}

VocabChangeHint Vocab000::AddSynonym(PCTSTR pszWordIn, PCTSTR pszOriginal)
{
    std::string strLower = pszWordIn;
    transform(strLower.begin(), strLower.end(), strLower.begin(), tolower);
    VocabChangeHint hint = VocabChangeHint::None;
    WordGroup dwGroup;
    if (LookupWord(pszOriginal, dwGroup))
    {
        hint = VocabChangeHint::Changed;
        _InsertWord(pszWordIn, dwGroup);
        _FixupGroupToString(dwGroup);
    }
    return hint;
}

//
// Similar to AddSynonym, but takes a group (and class) instead of a word.
// Fails if the word is already there.
//
VocabChangeHint Vocab000::AddWord(PCTSTR pszWord, DWORD dwInfo, BOOL fShowUI)
{
    std::string strLower = pszWord;
    transform(strLower.begin(), strLower.end(), strLower.begin(), tolower);
    VocabChangeHint hint = VocabChangeHint::None;
    WordGroup dwGroupDummy;
    if (!LookupWord(strLower, dwGroupDummy))
    {
        hint = VocabChangeHint::Changed;
        _InsertWord(strLower.c_str(), GetWordGroup(dwInfo));
        _mapGroupToClass[GetWordGroup(dwInfo)] = GetWordClass(dwInfo);
        _FixupGroupToString(GetWordGroup(dwInfo));
    }
    else if (fShowUI)
    {
        TCHAR szBuffer[MAX_PATH];
        StringCchPrintf(szBuffer, ARRAYSIZE(szBuffer), TEXT("Failed to add \"%s\": Word already exists!"), pszWord);
        AfxMessageBox(szBuffer, MB_ERRORFLAGS);
    }
    return hint;
}

VocabChangeHint Vocab000::SetGroupClass(WordGroup dwGroup, WordClass dwClass)
{
    VocabChangeHint hint = VocabChangeHint::None;
    if (_mapGroupToClass.find(dwGroup) != _mapGroupToClass.end())
    {
        // Make sure it exists before setting it.
        hint = VocabChangeHint::Changed;
        _mapGroupToClass[dwGroup] = dwClass;
    }
    return hint;
}

bool Vocab000::GetGroupClass(WordGroup dwGroup, WordClass *pdwClass) const
{
    bool success = false;
    auto groupClassIt = _mapGroupToClass.find(dwGroup);
    if (groupClassIt != _mapGroupToClass.end())
    {
        success = true;
        *pdwClass = groupClassIt->second;
    }
    return success;
}


//
// Ensures there is an entry for the group dwGroup
// This should be called after adding/removing items.
//
void Vocab000::_FixupGroupToString(WordGroup dwGroup)
{
    assert(dwGroup < ARRAYSIZE(_rgbGroups));
    std::string strGroup;
    size_t cMax = _words.size();
    for (size_t i = 0; i < cMax; i++)
    {
        WordGroup dwGroupWord;
        std::string strWord = _words[i];
        BOOL fTemp = LookupWord(strWord, dwGroupWord);
        assert(fTemp); // Else we're corrupt.
        if (dwGroup == dwGroupWord)
        {
            // This is a word from our group, add it to our string.
            if (strGroup.empty())
            {
                strGroup += strWord;
            }
            else
            {
                strGroup += TEXT(" | ");
                strGroup += strWord;
            }
        }
    }

    if (strGroup.empty())
    {
        // There are no more words from this group.  Remove the group.
        _rgbGroups[dwGroup] = 0;
        _mapGroupToString.erase(_mapGroupToString.find(dwGroup));
    }
    else
    {
        _mapGroupToString[dwGroup] = strGroup;
    }
}

VocabChangeHint Vocab000::RemoveWord(PCTSTR pszWord)
{
    VocabChangeHint hint = VocabChangeHint::None;
    std::string strLower = pszWord;
    transform(strLower.begin(), strLower.end(), strLower.begin(), tolower);
    
    WordGroup dwGroup;
    if (LookupWord(strLower, dwGroup))
    {
        hint = VocabChangeHint::Changed;
        // Found it.  Remove from word array.
        bool fFound = false;
        vector<string>::iterator wordIt = _words.begin();
        for (; wordIt != _words.end(); ++wordIt)
        {
            if (strLower == *wordIt)
            {
                // We're done.  Insert it here.
                fFound = true;
                break;
            }
        }
        assert(fFound); // Otherwise it means our _words and our _mapWordToGroup are out of sync!
        _words.erase(wordIt);

        // Remove it from the lookup table.
        _mapWordToGroup.erase(_mapWordToGroup.lower_bound(strLower));

        _FixupGroupToString(dwGroup);
    }
    return hint;
}

void VocabWriteTo(const ResourceEntity &resource, sci::ostream &byteStream, bool is900)
{
    const Vocab000 &vocab = resource.GetComponent<Vocab000>();

    // _words should already be sorted.

    // Skip past the offset table
    byteStream.FillByte(0, is900 ? Vocab000::AlphaIndexLength_900 : Vocab000::AlphaIndexLength);

    PCTSTR pszPreviousWord = TEXT("");
    size_t cWords = vocab._words.size();
    for (size_t i = 0; i < cWords; i++)
    {
        const std::string &strWord = vocab._words[i];
        PCTSTR pszWord = strWord.c_str();
        Vocab000::WordGroup dwGroup = vocab._mapWordToGroup.find(pszWord)->second;   // Must exist, or we're corrupt
        WordClass dwClass = vocab._mapGroupToClass.find(dwGroup)->second; // Must exist.
        DWORD dwInfo = InfoFromClassAndGroup(dwClass, dwGroup);

        if (pszWord[0] != pszPreviousWord[0])
        {
            if (is900)
            {
                if (pszWord[0] < 255) // For some reason, 255 is not allowed
                {
                    // We're at the beginning of a new set of letters.  Take note of the offset.
                    uint16_t *rgwOffsets = (uint16_t*)byteStream.GetInternalPointer();
                    assert(rgwOffsets[pszWord[0]] == 0);
                    assert(byteStream.tellp() < 0xffff);
                    rgwOffsets[pszWord[0]] = (uint16_t)byteStream.tellp();
                }
            }
            else
            {
                if ((pszWord[0] >= TEXT('a')) && (pszWord[0] <= TEXT('z')))
                {
                    // We're at the beginning of a new set of letters.  Take note of the offset.
                    uint16_t *rgwOffsets = (uint16_t*)byteStream.GetInternalPointer();
                    assert(rgwOffsets[pszWord[0] - TEXT('a')] == 0);
                    assert(byteStream.tellp() < 0xffff);
                    rgwOffsets[pszWord[0] - TEXT('a')] = (uint16_t)byteStream.tellp();
                }
            }
        }

        // Write the word.
        // How many characters are in common with the last word?
        uint8_t cSame = 0;
        PCTSTR pszPrevPtr = pszPreviousWord;
        PCTSTR pszCurrPtr = pszWord;
        while (pszPrevPtr[cSame] && (pszPrevPtr[cSame] == pszCurrPtr[cSame]))
        {
            cSame++;
        }

        pszPreviousWord = pszWord;

        // Byte indicating how many chars are in common with last word:
        byteStream.WriteByte(cSame);

        pszWord += cSame;
        int cRemaining = (int)lstrlen(pszWord);
        if (cRemaining > 1)
        {
            // Rest of the word, except for the last byte:
            byteStream.WriteBytes((uint8_t*)pszWord, cRemaining - 1);
        }
        else if (cRemaining == 0)
        {
            // This means that, for example, "foo" came after "foobar", which should not be allowed.
            assert(is900);  // Actually, this is allow for is900
            if (is900)
            {
                byteStream.WriteByte(0); // null term
            }
        }

        if (cRemaining)
        {
            // Write the last character, and signify it as such:
            uint8_t bLastCharacter = pszWord[cRemaining - 1];
            if (is900)
            {
                byteStream.WriteByte(bLastCharacter);
                byteStream.WriteByte(0); // numm term
            }
            else
            {
                bLastCharacter |= 0x80;
                byteStream.WriteByte(bLastCharacter);
            }
        }

        // Now write the class and group information.
        byteStream.WriteByte((uint8_t)(0xff & (dwInfo >> 16)));
        byteStream.WriteByte((uint8_t)(0xff & (dwInfo >> 8)));
        byteStream.WriteByte((uint8_t)(0xff & dwInfo));
    }
}

void VocabReadFrom(ResourceEntity &resource, sci::istream &byteStream, bool is900)
{
    Vocab000 &vocab = resource.GetComponent<Vocab000>();

    // Avoid needless reallocations.
    vocab._words.reserve(1000);

    // Skip the "alphabetical offset" table
    byteStream.skip(is900 ? Vocab000::AlphaIndexLength_900 : Vocab000::AlphaIndexLength);

    // Now we have started with the words.
    char sz[MAX_PATH];
    while (byteStream.has_more_data())
    {
        vocab._ReadWord(byteStream, sz, ARRAYSIZE(sz), is900);
    }
}

void VocabReadFrom_000(ResourceEntity &resource, sci::istream &byteStream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    VocabReadFrom(resource, byteStream, false);
}
void VocabReadFrom_900(ResourceEntity &resource, sci::istream &byteStream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    VocabReadFrom(resource, byteStream, true);
}

void VocabWriteTo_000(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag)
{
    VocabWriteTo(resource, byteStream, false);
}

void VocabWriteTo_900(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag)
{
    VocabWriteTo(resource, byteStream, true);
}

ResourceTraits vocabTraits_000 =
{
    ResourceType::Vocab,
    &VocabReadFrom_000,
    &VocabWriteTo_000,
    &NoValidationFunc,
    nullptr
};

ResourceTraits vocabTraits_900 =
{
    ResourceType::Vocab,
    &VocabReadFrom_900,
    &VocabWriteTo_900,
    &NoValidationFunc,
    nullptr
};

ResourceEntity *CreateVocabResource(SCIVersion version)
{
    std::unique_ptr<ResourceEntity> pResource = std::make_unique<ResourceEntity>(version.MainVocabResource == 0 ? vocabTraits_000 : vocabTraits_900);
    pResource->AddComponent(move(make_unique<Vocab000>()));
    return pResource.release();
}
