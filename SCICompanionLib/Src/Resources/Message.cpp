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
#include "Message.h"
#include "Text.h"
#include "ResourceEntity.h"
#include "format.h"
#include "NounsAndCases.h"
#include "ResourceSourceFlags.h"

using namespace std;

void MessageReadFrom_2102(TextComponent &messageComponent, sci::istream &byteStream)
{
    uint16_t messageCount;
    byteStream >> messageCount;

    for (int i = 0; i < messageCount; i++)
    {
        TextEntry message = { 0 };
        byteStream >> message.Noun;
        byteStream >> message.Verb;
        uint16_t textOffset;
        byteStream >> textOffset;
        sci::istream textStream = byteStream;
        textStream.seekg(textOffset);
        textStream >> message.Text;

        messageComponent.Texts.push_back(message);
    }
}

void MessageReadFrom_3411(TextComponent &messageComponent, sci::istream &byteStream)
{
    byteStream.skip(2);     // ptr to first byte past text data (?)
    uint16_t messageCount;
    byteStream >> messageCount;

    for (int i = 0; i < messageCount; i++)
    {
        TextEntry message = { 0 };
        byteStream >> message.Noun;
        byteStream >> message.Verb;
        byteStream >> message.Condition;
        byteStream >> message.Sequence;
        byteStream >> message.Talker;
        uint16_t textOffset;
        byteStream >> textOffset;
        byteStream.skip(3); // Unknown?
        sci::istream textStream = byteStream;
        textStream.seekg(textOffset);
        textStream >> message.Text;

        messageComponent.Texts.push_back(message);
    }
}

void MessageReadFrom_4000(TextComponent &messageComponent, sci::istream &byteStream)
{
    uint16_t offsetToAfterResource;
    byteStream >> offsetToAfterResource;
    byteStream >> messageComponent.MysteryNumber;

    uint16_t messageCount;
    byteStream >> messageCount;

    // "MysteryNumber" is a number that is roughly the number of messages, but sometimes a little more. Occasionally a lot more.
    // Sometimes its zero (KQ6, 95 and 916)

    for (int i = 0; i < messageCount; i++)
    {
        TextEntry message = { 0 };
        byteStream >> message.Noun;
        byteStream >> message.Verb;
        byteStream >> message.Condition;
        byteStream >> message.Sequence;
        byteStream >> message.Talker;
        uint16_t textOffset;
        byteStream >> textOffset;
        byteStream >> message.Style;
        // "unknown" seems to be either 0x00000000 or 0x01000000
        // 0x00000000 seems to indicate some markup.
        // It's 4 bytes, not 3 like on http://wiki.scummvm.org/index.php/SCI/Specifications/SCI_in_action/The_message_subsystem
        // An empty string can be x01000108
        // Hmm, not necessarily. Markup can appear whereever. It seems like fonts are included.
        // At any rate, presumably it's useful.

        sci::istream textStream = byteStream;
        textStream.seekg(textOffset);
        textStream >> message.Text;

        messageComponent.Texts.push_back(message);
    }
}

void MessageWriteTo_4000(const TextComponent &messageComponent, sci::ostream &byteStream)
{
    uint32_t fillThisIn = byteStream.tellp();
    byteStream.WriteWord(0); // We'll fill this in later
    uint32_t startCount = byteStream.tellp();
    byteStream << messageComponent.MysteryNumber;

    byteStream.WriteWord((uint16_t)messageComponent.Texts.size());

    uint16_t textOffset = (uint16_t)(byteStream.tellp() + (5 + 2 + 4) * messageComponent.Texts.size());
    for (const TextEntry &entry : messageComponent.Texts)
    {
        byteStream << entry.Noun;
        byteStream << entry.Verb;
        byteStream << entry.Condition;
        byteStream << entry.Sequence;
        byteStream << entry.Talker;
        byteStream << textOffset;
        byteStream << entry.Style;
        textOffset += (uint16_t)(entry.Text.length() + 1);
    }

    for (const TextEntry &entry : messageComponent.Texts)
    {
        byteStream << entry.Text;
    }

    uint16_t totalSize = (uint16_t)byteStream.tellp();
    uint16_t offsetToEnd = (uint16_t)(byteStream.tellp() - startCount);
    *(reinterpret_cast<uint16_t*>(byteStream.GetInternalPointer() + fillThisIn)) = offsetToEnd;
    // NOTE: This may need to be padded to WORD boundary
}

uint16_t CheckMessageVersion(sci::istream &byteStream)
{
    uint16_t msgVersion;
    byteStream >> msgVersion;
    return msgVersion;
}

void MessageReadFrom(ResourceEntity &resource, sci::istream &byteStream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    TextComponent &message = resource.GetComponent<TextComponent>();

    byteStream >> message.msgVersion;
    byteStream.skip(2);
    if (message.msgVersion <= 0x835)        // 2101
    {
        message.Flags = MessagePropertyFlags::Noun | MessagePropertyFlags::Verb;
        MessageReadFrom_2102(message, byteStream);
    }
    else if (message.msgVersion <= 0xd53)   // 3411
    {
        message.Flags = MessagePropertyFlags::Noun | MessagePropertyFlags::Verb | MessagePropertyFlags::Condition | MessagePropertyFlags::Sequence | MessagePropertyFlags::Talker;
        MessageReadFrom_3411(message, byteStream);
    }
    else
    {
        message.Flags = MessagePropertyFlags::Noun | MessagePropertyFlags::Verb | MessagePropertyFlags::Condition | MessagePropertyFlags::Sequence | MessagePropertyFlags::Talker;
        MessageReadFrom_4000(message, byteStream);
    }
}

void MessageWriteTo(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag)
{
    const TextComponent &message = resource.GetComponent<TextComponent>();
    byteStream << message.msgVersion;
    byteStream.WriteWord(0);    // Unknown
    if (message.msgVersion <= 0x835)
    {
        assert(false && "not implemented");
        //MessageWriteTo_2102(message, byteStream);
    }
    else if (message.msgVersion <= 0xd53)
    {
        assert(false && "not implemented");
        //MessageWriteTo_3411(message, byteStream);
    }
    else
    {
        MessageWriteTo_4000(message, byteStream);
    }
}

std::vector<std::string> split(const std::string& value, char separator)
{
    std::vector<std::string> result;
    std::string::size_type p = 0;
    std::string::size_type q;
    while ((q = value.find(separator, p)) != std::string::npos)
    {
        result.emplace_back(value, p, q - p);
        p = q + 1;
    }
    result.emplace_back(value, p);
    return result;
}

void ExportMessageToFile(const TextComponent &message, const std::string &filename)
{
    ofstream file;
    file.open(filename, ios_base::out | ios_base::trunc);
    if (file.is_open())
    {
        for (const auto &entry : message.Texts)
        {
            string firstPart = fmt::format("{0}\t{1}\t{2}\t{3}\t{4}\t", (int)entry.Noun, (int)entry.Verb, (int)entry.Condition, (int)entry.Sequence, (int)entry.Talker);
            file << firstPart;
            // Split by line to match SV.exe's output
            vector<string> lines = split(entry.Text, '\n');
            int lineNumber = 0;
            for (const string &line : lines)
            {
                if (lineNumber > 0)
                {
                    file << "\t\t\t\t\t"; // "Empty stuff" before next line (matches SV.exe's output)
                }
                file << line;
                file << endl;
                lineNumber++;
            }
        }
    }
}

void ConcatWithTabs(const vector<string> &pieces, size_t pos, string &text)
{
    while (pos < pieces.size())
    {
        text += "\t";
        text += pieces[pos];
        pos++;
    }
}

void ImportMessageFromFile(TextComponent &message, const std::string &filename)
{
    ifstream file;
    file.open(filename, ios_base::in);
    if (file.is_open())
    {
        string line;
        while (std::getline(file, line))
        {
            vector<string> linePieces = split(line, '\t');
            if (linePieces.size() >= 6)
            {
                // If the first 5 are empty, then it's an extension of the previous line
                bool empty = true;
                for (int i = 0; empty && (i < 5); i++)
                {
                    empty = linePieces[i].empty();
                }
                if (!empty)
                {
                    TextEntry entry = {};
                    entry.Noun = (uint8_t)stoi(linePieces[0]);
                    entry.Verb = (uint8_t)stoi(linePieces[1]);
                    entry.Condition = (uint8_t)stoi(linePieces[2]);
                    entry.Sequence = (uint8_t)stoi(linePieces[3]);
                    entry.Talker = (uint8_t)stoi(linePieces[4]);
                    entry.Text = linePieces[5];
                    ConcatWithTabs(linePieces, 6, entry.Text);
                    message.Texts.push_back(entry);
                }
                else if (!message.Texts.empty())
                {
                    // Append to previous
                    message.Texts.back().Text += "\n";
                    message.Texts.back().Text += linePieces[5];
                    ConcatWithTabs(linePieces, 6, message.Texts.back().Text);
                }
            }
        }
    }
}

bool ValidateMessage(const ResourceEntity &resource)
{
    // Check for duplicate tuples
    const TextComponent &text = resource.GetComponent<TextComponent>();
    unordered_map<uint32_t, const TextEntry *> tuples;
    for (const auto &entry : text.Texts)
    {
        uint32_t tuple = GetMessageTuple(entry);
        if (tuples.find(tuple) != tuples.end())
        {
            string message = fmt::format("Entries must be distinct. The following entries have the same noun/verb/condition/sequence:\n{0}\n{1}",
                entry.Text,
                tuples[tuple]->Text
                );

            AfxMessageBox(message.c_str(), MB_OK | MB_ICONWARNING);
            return false;
        }
        else
        {
            tuples[tuple] = &entry;
        }
    }

    // Check for sequences beginning with something other than 1.
    std::map<uint32_t, bool> tuplesHaveSeq;
    for (auto &entry : text.Texts)
    {
        uint32_t tuple = entry.Noun + (entry.Verb << 8) + (entry.Condition << 16);
        tuplesHaveSeq[tuple] = tuplesHaveSeq[tuple] || (entry.Sequence == 1);
    }
    for (auto &pair : tuplesHaveSeq)
    {
        if (!pair.second)
        {
            string message = fmt::format("The following mesaage: (noun:{0}, verb:{1}, cond:{2}) has a sequence that doesn't begin at 1. Save anyway?", (pair.first & 0xff), (pair.first >> 8) & 0xff, (pair.first >> 16) & 0xff);
            if (IDNO == AfxMessageBox(message.c_str(), MB_YESNO | MB_ICONWARNING))
            {
                return false;
            }
            break;
        }
    }

    return true;
}

void MessageWriteNounsAndCases(const ResourceEntity &resource, int resourceNumber)
{
    NounsAndCasesComponent *nounsAndCases = const_cast<NounsAndCasesComponent*>(resource.TryGetComponent<NounsAndCasesComponent>());
    if (nounsAndCases)
    {
        // Use the provided resource number instead of that in the ResourceEntity, since it may
        // be -1
        nounsAndCases->Commit(resourceNumber);
    }
}


ResourceTraits messageTraits =
{
    ResourceType::Message,
    &MessageReadFrom,
    &MessageWriteTo,
    &ValidateMessage,
    &MessageWriteNounsAndCases,
};

ResourceEntity *CreateMessageResource(SCIVersion version)
{
    std::unique_ptr<ResourceEntity> pResource = std::make_unique<ResourceEntity>(messageTraits);
    pResource->AddComponent(move(make_unique<TextComponent>()));
    switch (version.MessageMapSource)
    {
        case MessageMapSource::Included:
            pResource->SourceFlags = ResourceSourceFlags::ResourceMap;
            break;
        case MessageMapSource::MessageMap:
            pResource->SourceFlags = ResourceSourceFlags::MessageMap;
            break;
        case MessageMapSource::AltResMap:
            pResource->SourceFlags = ResourceSourceFlags::AltMap;
            break;
        default:
            assert(false);
            break;
    }
    return pResource.release();
}

ResourceEntity *CreateDefaultMessageResource(SCIVersion version)
{
    // Nothing different.
    return CreateMessageResource(version);
}

ResourceEntity *CreateNewMessageResource(SCIVersion version, uint16_t msgVersion)
{
    ResourceEntity *resource = CreateMessageResource(version);
    TextComponent &text = resource->GetComponent<TextComponent>();
    text.msgVersion = msgVersion;
    text.Flags = MessagePropertyFlags::Noun | MessagePropertyFlags::Verb;
    if (msgVersion > 0x835)
    {
        text.Flags |= MessagePropertyFlags::Condition | MessagePropertyFlags::Sequence | MessagePropertyFlags::Talker;
    }
    return resource;
}