#pragma once

namespace sci
{
    class Script;
}
class CSCOFile;

void AutoDetectVariableNames(sci::Script &script, CSCOFile *mainSCO, CSCOFile *scriptSCO, bool &mainDirty);