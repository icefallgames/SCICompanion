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
#include "Text.h"
#include "ResourceEntity.h"

using namespace std;

bool TextEntry::operator == (const TextEntry &other) const
{
    return Noun == other.Noun &&
        Verb == other.Verb &&
        Condition == other.Condition &&
        Sequence == other.Sequence &&
        Style == other.Style &&
        Text == other.Text &&
        Talker == other.Talker;
}
bool TextEntry::operator!=(const TextEntry &other) const
{
    return !(*this == other);
}

int TextComponent::AddString(const std::string &theString)
{
    TextEntry entry = { 0 };
    entry.Text = theString;
    Texts.push_back(entry);
    return (int)(Texts.size() - 1); // Index of added string
}

int TextComponent::AddStringDedupe(const std::string &theString)
{
    for (size_t i = 0; i < Texts.size(); i++)
    {
        if (Texts[i].Text == theString)
        {
            // This string already exists.  Just re-use it.
            return (int)i;
        }
    }
    return AddString(theString);
}

TextChangeHint TextComponent::SetStringAt(int iIndex, const std::string &theString)
{
    TextChangeHint hint = (theString != Texts[iIndex].Text) ? TextChangeHint::Changed : TextChangeHint::None;
    Texts[iIndex].Text = theString;
    return hint;
}

TextChangeHint TextComponent::MoveStringUp(int iIndex)
{
    TextChangeHint hint = TextChangeHint::None;
    if ((iIndex > 0) && (iIndex < (int)Texts.size()))
    {
        hint = TextChangeHint::Changed;
        std::swap(Texts[iIndex - 1], Texts[iIndex]);
    }
    return hint;
}

TextChangeHint TextComponent::MoveStringDown(int iIndex)
{
    TextChangeHint hint = TextChangeHint::None;
    if (Texts.size() > 1)
    {
        if (iIndex < (int)(Texts.size() - 1))
        {
            hint = TextChangeHint::Changed;
            std::swap(Texts[iIndex + 1], Texts[iIndex]);
        }
    }
    return hint;
}

TextChangeHint TextComponent::DeleteString(int iIndex)
{
    Texts.erase(Texts.begin() + iIndex);
    return TextChangeHint::Changed;
}

// ILookupNames
std::string TextComponent::Lookup(uint16_t wName) const
{
    std::string ret;
    if (wName < Texts.size())
    {
        ret = Texts[wName].Text;
    }
    return ret;
}

void TextWriteTo(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag)
{
    const TextComponent &text = resource.GetComponent<TextComponent>();
    // Note: this function is not unicode aware
    for (size_t i = 0; i < text.Texts.size(); i++)
    {
        const string &str = text.Texts[i].Text;
        byteStream.WriteBytes((uint8_t*)str.c_str(), (int)str.length() + 1);
    }
}

void TextReadFrom(ResourceEntity &resource, sci::istream &byteStream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    TextComponent &text = resource.GetComponent<TextComponent>();
    assert(text.Texts.empty());
    text.Flags = MessagePropertyFlags::None;

    while (byteStream.has_more_data())
    {
        string str;
        byteStream >> str;
        if (byteStream.good())
        {
            TextEntry entry = { 0 };
            entry.Text = str;
            text.Texts.push_back(entry);
        }
    }
}

ResourceTraits textTraits =
{
    ResourceType::Text,
    &TextReadFrom,
    &TextWriteTo,
    &NoValidationFunc,
    nullptr
};

ResourceEntity *CreateTextResource(SCIVersion version)
{
    std::unique_ptr<ResourceEntity> pResource = std::make_unique<ResourceEntity>(textTraits);
    pResource->AddComponent(move(make_unique<TextComponent>()));
    return pResource.release();
}

ResourceEntity *CreateDefaultTextResource(SCIVersion version)
{
    // Nothing different.
    return CreateTextResource(version);
}


