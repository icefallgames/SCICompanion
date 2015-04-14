#pragma once

class IExtractProgress
{
public:
    // Return false to abort
    virtual bool SetProgress(const std::string &info, int amountDone, int totalAmount) = 0;
};

void ExtractAllResources(SCIVersion version, const std::string &destinationFolder, bool extractPicImages, bool extractViewImages, bool disassembleScripts, IExtractProgress *progress = nullptr);