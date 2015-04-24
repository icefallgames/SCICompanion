#pragma once

class ILookupNames;

std::string _GetTempVariableName(int index);
std::string _GetLocalVariableName(int index);
std::string _GetGlobalVariableName(int index);
std::string _GetParamVariableName(int index);

struct CodeSection
{
    uint16_t begin;
    uint16_t end;
};

bool FindStartEndCode(std::set<uint16_t>::const_iterator codeStart, const std::set<uint16_t> &codePointersTO, const std::vector<CodeSection> &codeSections, CodeSection &sectionOut);

std::string SaidSequenceToString(std::vector<uint16_t> saidSequence, const ILookupNames *pWords);