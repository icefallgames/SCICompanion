#pragma once

#include "Components.h"
#include "interfaces.h"
// FWD declarations
class ResourceBlob;

namespace sci
{
    class istream;
}

enum class VocabChangeHint
{
    None = 0,
    // Vocab is ready
    Changed = 0x00000100,
};

DEFINE_ENUM_FLAGS(VocabChangeHint, uint32_t)

enum class WordClass : uint16_t
{
    Unknown = 0x000,
    Number = 0x001,
    Special1 = 0x002,
    Special2 = 0x004,
    Special3 = 0x008,
    Proposition = 0x010,
    Article = 0x020,
    QualifyingAdjective = 0x040,
    RelativePronoun = 0x080,
    Noun = 0x100,
    IndicativeVerb = 0x200,
    Adverb = 0x400,
    ImperativeVerb = 0x800
};

DEFINE_ENUM_FLAGS(WordClass, uint16_t)

struct Vocab000 : public ResourceComponent, public ILookupNames
{
public:
    friend void VocabReadFrom(ResourceEntity &resource, sci::istream &byteStream, bool is900);
    friend void VocabWriteTo(const ResourceEntity &resource, sci::ostream &byteStream);

    typedef DWORD WordGroup;

    typedef std::unordered_map<std::string, WordGroup> word2group_map;
	typedef std::unordered_map<WordGroup, std::string> group2words_map;
	typedef std::unordered_map<WordGroup, WordClass> group2class_map;

    Vocab000();
    Vocab000(const Vocab000 &src) = default;
    ResourceComponent *Clone() const override
    {
        return new Vocab000(*this);
    }

    VocabChangeHint AddNewWord(PCTSTR pszWord, WordClass dwClass, BOOL fShowUI);
    VocabChangeHint AddSynonym(PCTSTR pszWord, PCTSTR pszOriginal);
    VocabChangeHint AddWord(PCTSTR pszWord, DWORD dwInfo, BOOL fShowUI);
    VocabChangeHint SetGroupClass(WordGroup dwGroup, WordClass dwClass);
    bool GetGroupClass(WordGroup dwGroup, WordClass *pdwClass);
    VocabChangeHint RemoveWord(PCTSTR pszWord);

    HRESULT QuickSave();

    typedef group2words_map::const_iterator groups_iterator;
    void EnumGroups(groups_iterator &position, WordGroup &dwGroup, WordClass &dwClass, std::string &strWords) const;
    std::string GetGroupWordString(WordGroup dwGroup) const;
    groups_iterator GroupsBegin() const { return _mapGroupToString.begin(); }
    groups_iterator GroupsEnd() const { return _mapGroupToString.end(); }
    std::vector<std::string> &GetWords() { return _words; }
    bool LookupWord(const std::string &word, WordGroup &dwGroup) const;
    WordGroup GroupFromString(PCTSTR pszString);

    // ILookupNames
    std::string Lookup(uint16_t wIndex) const;

private:
    static const uint16_t AlphaIndexLength = (26 * 2); // 26 letters, 2 bytes for each offset.
    static const uint16_t AlphaIndexLength_900 = (255 * 2);

    void _InsertWord(PCTSTR pszWord, WordGroup dwGroup);
    WordGroup _FindLargestEmptyGroup();
    void _FixupGroupToString(WordGroup dwGroup);
    void _ReadWord(sci::istream &byteStream, char *pszBuffer, size_t cchBuffer);

    // Map from word to class
    word2group_map _mapWordToGroup;
    group2words_map _mapGroupToString;
    group2class_map _mapGroupToClass;
    uint8_t _rgbGroups[0x1000]; // 4096 entries, one for each group.

    // This is just the raw vocab
    std::vector<std::string> _words;
};

bool IsValidVocabString(PCTSTR pszWord, bool fShowUI);
bool IsValidVocabChar(TCHAR ch);
PCTSTR FindValidVocabStringFromRight(PCTSTR pszWord);
void GetWordClassString(WordClass dwClass, std::string &str);
WordClass GetWordClass(DWORD dwInfo);
Vocab000::WordGroup GetWordGroup(DWORD dwInfo);
DWORD InfoFromClassAndGroup(WordClass dwClass, Vocab000::WordGroup dwGroup);

ResourceEntity *CreateVocabResource(SCIVersion version);

