#pragma once

namespace sci
{
    class Script;
}
class CSCOFile;
class IDecompilerConfig;

void AutoDetectVariableNames(sci::Script &script, const IDecompilerConfig *config, CSCOFile *mainSCO, CSCOFile *scriptSCO, bool &mainDirty);