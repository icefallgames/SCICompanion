#pragma once

#include "CompiledScript.h"

//void AnalyzeInstruction(uint8_t opcode, const uint16_t *operands);
typedef void(*AnalyzeInstructionPtr)(Opcode opcode, const uint16_t *operands, uint16_t currentPCOffset);


void DisassembleObject(const CompiledScript &script, 
    const CompiledObjectBase &object,
    std::ostream &out,
    ICompiledScriptLookups *pLookups,
    IObjectFileScriptLookups *pOFLookups,
    const ICompiledScriptSpecificLookups *pScriptThings,
    const std::vector<uint8_t> &scriptResource,
    const std::vector<CodeSection> &codeSections,
    std::set<uint16_t> &codePointers,
    AnalyzeInstructionPtr analyzeInstruction = nullptr);

void DisassembleScript(const CompiledScript &script, std::ostream &out, ICompiledScriptLookups *pLookups, IObjectFileScriptLookups *pOFLookups, const ILookupNames *pWords, const std::string *pMessage = nullptr, AnalyzeInstructionPtr analyzeInstruction = nullptr);