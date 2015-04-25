#pragma once

#include "Components.h"
#include "interfaces.h"

enum class TextChangeHint
{
    None = 0,
    // Text is ready
    Changed = 0x00000100,
};

DEFINE_ENUM_FLAGS(TextChangeHint, uint32_t)

enum class MessagePropertyFlags : uint16_t
{
    None = 0,
    Noun = 0x0001,
    Verb = 0x0002,
    Condition = 0x0004,
    Sequence = 0x0008,
    Talker = 0x0010
};

DEFINE_ENUM_FLAGS(MessagePropertyFlags, uint16_t)

// These are used for both message and text resources.
struct TextEntry
{
    uint8_t Noun;
    uint8_t Verb;
    uint8_t Condition;
    uint8_t Sequence;
    uint8_t Talker;
    uint32_t Style;  // Just a guess...
    std::string Text;
};

struct TextComponent : public ResourceComponent, public ILookupNames
{
    TextComponent() { }
    TextComponent(const TextComponent &src) = default;
    ResourceComponent *Clone() const override
    {
        return new TextComponent(*this);
    }

    typedef std::vector<TextEntry>::iterator iterator;
    typedef std::vector<TextEntry> container_type;
    typedef std::vector<TextEntry>::const_iterator const_iterator;

    container_type Texts;

    // Support for message resources:
    MessagePropertyFlags Flags;
    uint16_t msgVersion;
    uint16_t MysteryNumber;

    // Used by the compiler (resource tuples) (why? oh... that is for automatic resource tuples, got it...)
    int AddStringDedupe(const std::string &theString);
    int AddString(const std::string &theString);
    // Set string at (used by the editor).  Returns TRUE if it was modified.
    TextChangeHint SetStringAt(int iIndex, const std::string &theString);
    TextChangeHint MoveStringUp(int iIndex);
    TextChangeHint MoveStringDown(int iIndex);
    TextChangeHint DeleteString(int iIndex);

    // ILookupNames
    std::string Lookup(uint16_t wName) const;
};

ResourceEntity *CreateTextResource(SCIVersion version);
ResourceEntity *CreateDefaultTextResource(SCIVersion version);
