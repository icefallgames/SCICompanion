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
#pragma once

#include "CompiledScript.h"
#include "Disassembler.h"
#include "DisassembleHelper.h"
#include "PMachine.h"
#include "scii.h"

template<typename _TInspect>
bool InspectCode(SCIVersion version, const uint8_t *pBegin, const uint8_t *pEnd, uint16_t wBaseOffset, _TInspect analyzeInstruction)
{
    try
    {
        const uint8_t *pCur = pBegin;
        uint16_t wOffset = wBaseOffset;
        while (pCur < pEnd) // Possibility of read AVs here, but we catch exceptions.
        {
            uint8_t bRawOpcode = *pCur;
            Opcode opcode = RawToOpcode(version, bRawOpcode);
            // Advance past opcode
            pCur++;
            wOffset++;
            uint16_t wOperandsRaw[3];
            for (int i = 0; i < 3; i++)
            {
                int cIncr = GetOperandSize(bRawOpcode, GetOperandTypes(version, opcode)[i], pCur);
                if (cIncr == 0)
                {
                    break;
                }
                wOperandsRaw[i] = (cIncr == 2) ? *((uint16_t*)pCur) : *pCur;

                pCur += cIncr;
                wOffset += cIncr;
            }
            if (!analyzeInstruction(opcode, wOperandsRaw, wOffset))
            {
                return false;
            }
        }
    }
    catch (...) {}

    return true;
}

template<typename _TInspect>
bool InspectFunction(const CompiledScript &script, uint16_t wCodeOffsetTO, std::set<uint16_t> &sortedCodePointersTO, _TInspect analyzeInstruction)
{
    std::set<uint16_t>::const_iterator codeStartIt = sortedCodePointersTO.find(wCodeOffsetTO);
    if (codeStartIt != sortedCodePointersTO.end()) // Shouldn't happen, but this is a sensitive code path in version detection, needs to be robust.
    {
        CodeSection section;
        if (FindStartEndCode(codeStartIt, sortedCodePointersTO, script._codeSections, section))
        {
            const BYTE *pBegin = &script.GetRawBytes()[section.begin];
            const BYTE *pEnd = &script.GetRawBytes()[section.end];
            if (!InspectCode(script.GetVersion(), pBegin, pEnd, wCodeOffsetTO, analyzeInstruction))
            {
                return false;
            }
        }
    }
    return true;
}

template<typename _TInspect>
bool InspectScriptCode(const CompiledScript &script, ICompiledScriptLookups *pLookups, const std::string &objectFilter, const std::string &methodFilter, _TInspect analyzeInstruction)
{
    // Make an index of code pointers by looking at the object methods
    std::set<uint16_t> codePointersTO;
    for (auto &object : script._objects)
    {
        const std::vector<uint16_t> &methodPointersTO = object->GetMethodCodePointersTO();
        codePointersTO.insert(methodPointersTO.begin(), methodPointersTO.end());
    }


    // and the exports
    for (size_t i = 0; i < script._exportsTO.size(); i++)
    {
        uint16_t wCodeOffset = script._exportsTO[i];
        // Export offsets could point to objects too - we're only interested in code pointers, so
        // check that it's within bounds.
        if (script.IsExportAProcedure(wCodeOffset))
        {
            codePointersTO.insert(wCodeOffset);
        }
    }

    // and finally, the most difficult of all, we'll need to scan though for any call calls...
    // those would be our internal procs
    std::set<uint16_t> internalProcOffsetsTO = script.FindInternalCallsTO();//_rgRawCode, _wCodePosTO, _wCodeLength);
    // Before adding these though, remove any exports from the internalProcOffsets.
    for (const auto &exporty : script._exportsTO)
    {
        std::set<uint16_t>::iterator internalsIndex = find(internalProcOffsetsTO.begin(), internalProcOffsetsTO.end(), exporty);
        if (internalsIndex != internalProcOffsetsTO.end())
        {
            // Remove this guy.
            internalProcOffsetsTO.erase(internalsIndex);
        }
    }
    // Now add the internal guys to the full list
    codePointersTO.insert(internalProcOffsetsTO.begin(), internalProcOffsetsTO.end());

    const std::vector<BYTE> &scriptResource = script.GetRawBytes();

    for (auto &object : script._objects)
    {
        if (objectFilter.empty() || (objectFilter == object->GetName()))
        {
            const std::vector<uint16_t> &functionSelectors = object->GetMethods();
            const std::vector<uint16_t> &functionOffsetsTO = object->GetMethodCodePointersTO();

            // Methods
            assert(functionSelectors.size() == functionOffsetsTO.size());
            for (size_t i = 0; i < functionSelectors.size(); i++)
            {
                std::string name = pLookups->LookupSelectorName(functionSelectors[i]);
                if (methodFilter.empty() || (methodFilter == name))
                {
                    // Now the code.
                    std::set<uint16_t>::const_iterator functionIndex = find(codePointersTO.begin(), codePointersTO.end(), functionOffsetsTO[i]);
                    if (functionIndex != codePointersTO.end())
                    {
                        CodeSection section;
                        if (FindStartEndCode(functionIndex, codePointersTO, script._codeSections, section))
                        {
                            const BYTE *pStartCode = &scriptResource[section.begin];
                            const BYTE *pEndCode = &scriptResource[section.end];
                            if (!InspectCode(object->GetVersion(), pStartCode, pEndCode, functionOffsetsTO[i], analyzeInstruction))
                            {
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }

    if (objectFilter.empty())
    {
        // Now the exported procedures.
        for (size_t i = 0; i < script._exportsTO.size(); i++)
        {
            // _exportsTO, in addition to containing code pointers for public procedures, also
            // contain the Rm/Room class.  Filter these out by ignoring code pointers which point outside
            // the codesegment.
            if (script.IsExportAProcedure(script._exportsTO[i]))
            {
                if (!InspectFunction(script, script._exportsTO[i], codePointersTO, analyzeInstruction))
                {
                    return false;
                }
            }
        }

        // Now the internal procedures (REVIEW - possibly overlap with exported ones)
        std::set<uint16_t>::iterator it = internalProcOffsetsTO.begin();
        while (it != internalProcOffsetsTO.end())
        {
            if (!InspectFunction(script, *it, codePointersTO, analyzeInstruction))
            {
                return false;
            }
            it++;
        }
    }
    return true;
}
