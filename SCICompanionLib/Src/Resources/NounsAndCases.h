#pragma once

#include "Components.h"
#include "MessageHeaderFile.h"
#include "MessageSource.h"

class NounsAndCasesComponent : public ResourceComponent
{
public:
    NounsAndCasesComponent(const std::string &msgFolder, int messageNumber);
    void Commit(int messageNumber);

    NounsAndCasesComponent *Clone() const override
    {
        return new NounsAndCasesComponent(*this);
    }

    MessageSource &GetNouns();
    const MessageSource &GetNouns() const;
    MessageSource &GetCases();
    const MessageSource &GetCases() const;

private:
    MessageHeaderFile _headerFile;

    std::string _GetMessageFile();
    std::string _msgFolder;
    int _messageNumber;
};