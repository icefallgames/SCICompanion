#include "stdafx.h"
#include "NounsAndCases.h"
#include "format.h"

using namespace std;

NounsAndCasesComponent::NounsAndCasesComponent(const std::string &msgFolder, int messageNumber) : _messageNumber(messageNumber), _msgFolder(msgFolder),
    _headerFile(fmt::format("{0}\\{1}.shm", msgFolder, messageNumber), fmt::format("{0}.shm", messageNumber), initializer_list<string>({ "NOUNS", "CASES" }))
{
}

MessageSource &NounsAndCasesComponent::GetNouns()
{
    return *_headerFile.GetMessageSource("NOUNS");
}
const MessageSource &NounsAndCasesComponent::GetNouns() const
{
    return *_headerFile.GetMessageSource("NOUNS");
}
MessageSource &NounsAndCasesComponent::GetCases()
{
    return *_headerFile.GetMessageSource("CASES");
}
const MessageSource &NounsAndCasesComponent::GetCases() const
{
    return *_headerFile.GetMessageSource("CASES");
}

// REVIEW: Why does it pass a nmssage number?
void NounsAndCasesComponent::Commit(int messageNumber)
{
    _headerFile.Commit();
}
