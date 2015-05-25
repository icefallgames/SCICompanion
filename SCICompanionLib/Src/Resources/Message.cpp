#include "stdafx.h"
#include "Message.h"
#include "Text.h"
#include "ResourceEntity.h"
#include "format.h"

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
    //string String = static_cast<ostringstream*>(&(ostringstream() << (someNumber - messageCount)))->str();
    //String += "\n";
    //OutputDebugString(String.c_str());

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

void MessageReadFrom(ResourceEntity &resource, sci::istream &byteStream)
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

void MessageWriteTo(const ResourceEntity &resource, sci::ostream &byteStream)
{
    TextComponent &message = resource.GetComponent<TextComponent>();
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

ResourceTraits messageTraits =
{
    ResourceType::Message,
    &MessageReadFrom,
    &MessageWriteTo,
    &NoValidationFunc       // REVIEW: Some validation: make sure no out of sequence stuff, though maybe we could sort by seq
};

ResourceEntity *CreateMessageResource(SCIVersion version)
{
    std::unique_ptr<ResourceEntity> pResource = std::make_unique<ResourceEntity>(messageTraits);
    pResource->AddComponent(move(make_unique<TextComponent>()));
    return pResource.release();
}

ResourceEntity *CreateDefaultMessageResource(SCIVersion version)
{
    // Nothing different.
    return CreateMessageResource(version);
}

