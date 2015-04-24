#include "stdafx.h"
#include "Disassembler.h"
#include "DisassembleHelper.h"
#include "scii.h"
#include "AppState.h"

using namespace std;

#define STATE_CALCBRANCHES 0
#define STATE_OUTPUT 1


void _GetVarType(std::ostream &out, Opcode bOpcode, uint16_t wIndex, IObjectFileScriptLookups *pOFLookups)
{
    // This is a 0-127 opcode.
    // Use the lowest two bits to determine the var type
    switch (static_cast<BYTE>(bOpcode)& 0x03)
    {
    case 0:
        // Get the global's name if possible
        if (pOFLookups)
        {
            out << pOFLookups->ReverseLookupGlobalVariableName(wIndex);
        }
        else
        {
            out << _GetGlobalVariableName(wIndex);
        }
        break;
    case 1:
        out << _GetLocalVariableName(wIndex);
        break;
    case 2:
        out << _GetTempVariableName(wIndex);
        break;
    case 3:
        if (wIndex == 0)
        {
            // parameter 0 is the count of parameters.
            out << "paramTotal";
        }
        else
        {
            out << _GetParamVariableName(wIndex);
        }
        break;
    }
}

int GetOperandSize(BYTE bOpcode, OperandType operandType)
{
    int cIncr = 0;
    switch (operandType)
    {
    case otEMPTY:
        cIncr = 0;
        break;
    case otVAR:
    case otPVAR:
    case otCLASS:
    case otPROP:
    case otSTRING:
    case otSAID:
    case otKERNEL:
    case otLABEL:
    case otPUBPROC:
    case otINT:
    case otUINT:
    case otOFFS:
        cIncr = (bOpcode & 1) ? 1 : 2;
        break;
    case otINT16:
        cIncr = 2;
        break;
    case otINT8:
        cIncr = 1;
        break;
    }
    return cIncr;
}

void DisassembleCode(SCIVersion version, std::ostream &out, ICompiledScriptLookups *pLookups, IObjectFileScriptLookups *pOFLookups, const ICompiledScriptSpecificLookups *pScriptThings, const ILookupPropertyName *pPropertyNames, const BYTE *pBegin, const BYTE *pEnd, uint16_t wBaseOffset, AnalyzeInstructionPtr analyzeInstruction)
{
    try
    {
        set<uint16_t> codeLabelOffsets; // Keep track of places that are branched to.
        for (int state = 0; state < 2; state++)
        {
            const BYTE *pCur = pBegin;
            uint16_t wOffset = wBaseOffset;
            auto currentLabelOffset = codeLabelOffsets.begin(); // for STATE_CALCBRANCHES
            while (pCur < pEnd) // Possibility of read AVs here, but we catch exceptions.
            {
                BYTE bRawOpcode = *pCur;
                Opcode bOpcode = static_cast<Opcode>(bRawOpcode >> 1);

                ASSERT(bOpcode <= Opcode::LastOne);
                const char *pszOpcode = OpcodeNames[static_cast<BYTE>(bOpcode)];
                bool bByte = (*pCur) & 1; // Is this a "byte" opcode or a "word" opcode.
                bool fDone = false;

                char szBuf[50];
                size_t cchBufLimit = 30;
                ASSERT(cchBufLimit < ARRAYSIZE(szBuf));
                if (state == STATE_OUTPUT)
                {
                    if ((currentLabelOffset != codeLabelOffsets.end()) && (*currentLabelOffset == wOffset))
                    {
                        // We're at label.
                        out << endl << "        code_" << setw(4) << setfill('0') << wOffset << endl;
                        ++currentLabelOffset;
                    }
                    out << "  " << setw(4) << setfill('0') << wOffset << ":";
                    int indent = 22;
                    const BYTE *pCurTemp = pCur; // skip past opcode
                    for (int i = -1; i < 3; i++)
                    {
                        int cIncr = (i == -1) ? 1 : GetOperandSize(bRawOpcode, GetOperandTypes(bOpcode)[i]);
                        if (cIncr == 0)
                        {
                            break;
                        }
                        else
                        {
                            uint16_t wOperandTemp = (cIncr == 2) ? *((uint16_t*)pCurTemp) : *pCurTemp;
                            out << setw((cIncr == 1) ? 2 : 4);
                            out << setfill('0') << wOperandTemp << " ";
                            pCurTemp += cIncr;
                            indent -= (cIncr == 1) ? 3 : 5; // How many chars were written...
                        }
                    }
                    ASSERT(indent > 0);
                    out << setw(indent) << setfill(' ') << pszOpcode << " "; // Indent x chars, and print opcode
                }


                pCur++; // skip past opcode
                wOffset++;
                uint16_t wOperandStart = wOffset;

                if (state == STATE_CALCBRANCHES)
                {
                    // Keep track of the targets of branch instructions
                    if ((bOpcode == Opcode::BNT) || (bOpcode == Opcode::BT) || (bOpcode == Opcode::JMP))
                    {
                        // This is a branch instruction.  Figure out the offset.
                        // The relative offset is either a byte or word, and is calculated post instruction
                        // (hence we add 1 or 2 to our calculation)
                        codeLabelOffsets.insert(CalcOffset(wOperandStart, (bByte ? ((uint16_t)*pCur) : (*((uint16_t*)pCur))), bByte, bRawOpcode));
                    }
                }

                uint16_t wOperandsRaw[3];
                uint16_t wOperands[3];
                for (int i = 0; !fDone && i < 3; i++)
                {
                    szBuf[0] = 0;
                    int cIncr = GetOperandSize(bRawOpcode, GetOperandTypes(bOpcode)[i]);
                    if (cIncr == 0)
                    {
                        break;
                    }
                    if (state == STATE_OUTPUT)
                    {
                        wOperandsRaw[i] = (cIncr == 2) ? *((uint16_t*)pCur) : *pCur;
                        wOperands[i] = wOperandsRaw[i];
                        switch (GetOperandTypes(bOpcode)[i])
                        {
                        case otINT:
                        case otUINT:
                        case otINT16:
                        case otINT8:
                            out << wOperands[i];
                            break;
                        case otKERNEL:
                            out << pLookups->LookupKernelName(wOperands[i]);
                            break;
                        case otPUBPROC:
                            out << "procedure_" << setw(4) << setfill('0') << wOperands[i];
                            break;
                        case otSAID:
                            out << "said_" << setw(4) << setfill('0') << wOperands[i];
                            break;
                        case otOFFS:
                            // This is a bit of a hack here.  We're making the assumption that a otOFFS parameter
                            // is the one and only parameter for this opcode.  CalcOffset makes this assumption
                            // in order to calculate the offset.
                            assert(GetOperandTypes(bOpcode)[i + 1] == otEMPTY);
                            if (version.lofsaOpcodeIsAbsolute)
                            {
                                wOperands[i] = wOperandsRaw[i]; // Unnecessary, but reinforces the point.
                            }
                            else
                            {
                                wOperands[i] = CalcOffset(wOperandStart, wOperandsRaw[i], bByte, bRawOpcode);
                            }
                            out << "$" << setw(4) << setfill('0') << wOperands[i];
                            break;
                        case otEMPTY:
                            break;

                        case otPROP:
                            // This value is an offset from the beginning of this object's species
                            // So, get the species, and then divide this value by 2, and use it as an index into its
                            // selector thang.
                            //
                            if (pPropertyNames)
                            {
                                out << pPropertyNames->LookupPropertyName(pLookups, wOperands[i]);
                            }
                            else
                            {
                                out << " // (property opcode in procedure)";
                            }
                            break;

                        case otCLASS:
                            out << pLookups->LookupClassName(wOperands[i]);
                            break;

                        case otVAR:
                            _GetVarType(out, bOpcode, wOperands[i], pOFLookups);
                            break;

                        case otLABEL:
                            // This is a relative position from the post pc
                            out << ((bOpcode == Opcode::CALL) ? "proc" : "code") << "_" << setw(4) << setfill('0') << CalcOffset(wOperandStart, wOperands[i], bByte, bRawOpcode);
                            break;

                        default:
                            out << "$" << setw(4) << setfill('0') << wOperands[i];
                            break;
                        }
                        out << " ";
                    }
                    pCur += cIncr;
                    wOffset += cIncr;
                }

                if (analyzeInstruction && (state == STATE_OUTPUT))
                {
                    (*analyzeInstruction)(bOpcode, wOperandsRaw, wOffset);
                }

                if (state == STATE_OUTPUT)
                {
                    // Time for comments (for some instructions)
                    szBuf[0] = 0;
                    switch (bOpcode)
                    {
                    case Opcode::LINK:
                        out << "// (var $" << wOperands[0] << ")";
                        break;

                    case Opcode::LOFSS:
                    case Opcode::LOFSA:
                        // This is an offset... it could be an interesting one, like a string or said.
                        ICompiledScriptSpecificLookups::ObjectType type;
                        out << "// " << pScriptThings->LookupObjectName(wOperands[0], type);
                        break;

                    case Opcode::PUSHI:
                        out << "// $" << wOperands[0] << " " << pLookups->LookupSelectorName(wOperands[0]);
                        break;
                        // could do it for push0, push1, etc..., but it's just clutter, and rarely the intention.

                    case Opcode::CALLE:
                    case Opcode::CALLB:
                        // Try to get the public export name.
                        if (pOFLookups)
                        {
                            uint16_t wScript;
                            uint16_t wIndex;
                            if (Opcode::CALLB == bOpcode)
                            {
                                wScript = 0;
                                wIndex = wOperands[0];
                            }
                            else
                            {
                                wScript = wOperands[0];
                                wIndex = wOperands[1];
                            }
                            out << "// " << pOFLookups->ReverseLookupPublicExportName(wScript, wIndex) << " ";
                        }
                        break;

                    }

                    out << endl;
                    switch (bOpcode)
                    {
                    case Opcode::SEND:
                    case Opcode::CALL:
                    case Opcode::CALLB:
                    case Opcode::CALLE:
                    case Opcode::CALLK:
                    case Opcode::SELF:
                    case Opcode::SUPER:
                        // Add another carriage return after these instructions
                        out << endl;
                        break;
                    }
                }
            }
        }
    }
    catch (...)
    {
        // In case we read more than there was.
        appState->LogInfo("Error while disassembling script.");
    }
}

void DisassembleObject(const CompiledObjectBase &object,
    std::ostream &out,
    ICompiledScriptLookups *pLookups,
    IObjectFileScriptLookups *pOFLookups,
    const ICompiledScriptSpecificLookups *pScriptThings,
    const std::vector<BYTE> &scriptResource,
    const std::vector<CodeSection> &codeSections,
    std::set<uint16_t> &codePointersTO,
    AnalyzeInstructionPtr analyzeInstruction)
{
    out << "// " << setw(4) << setfill('0') << object.GetPosInResource() << endl;
    out << (object.IsInstance() ? "(instance " : "(class ");

    if (object.IsPublic)
    {
        out << "public";
    }

    // Header
    out << object.GetName() << " of " << pLookups->LookupClassName(object.GetSuperClass()) << endl;

    // Properties (skip the first 4)
    out << "    (properties" << endl;
    vector<uint16_t> propertySelectorList;
    if (pLookups->LookupSpeciesPropertyList(object.GetSpecies(), propertySelectorList))
    {
        const vector<uint16_t> &propertyValues = object.GetPropertyValues();
        size_t selectorCount = propertySelectorList.size();
        size_t valueCount = propertyValues.size();
        if (valueCount != selectorCount)
        {
            out << "// Problem with properties. Species has " << (int)selectorCount << " but instance has " << (int)valueCount << "." << endl;
        }
        for (size_t i = object.GetNumberOfDefaultSelectors(); i < min(selectorCount, valueCount); i++)
        {
            out << "        " << pLookups->LookupSelectorName(propertySelectorList[i]) << " $" << propertyValues[i] << endl;
        }
        out << "    )" << endl;
    }
    else
    {
        out << "// Can't get properties for class" << endl;
    }

    const vector<uint16_t> &functionSelectors = object.GetMethods();
    const vector<uint16_t> &functionOffsetsTO = object.GetMethodCodePointersTO();

    // Methods
    assert(functionSelectors.size() == functionOffsetsTO.size());
    for (size_t i = 0; i < functionSelectors.size(); i++)
    {
        out << "    (method (" << pLookups->LookupSelectorName(functionSelectors[i]) << ") // method_" << setw(4) << setfill('0') << functionOffsetsTO[i] << endl;

        // Now the code.
        set<uint16_t>::const_iterator functionIndex = find(codePointersTO.begin(), codePointersTO.end(), functionOffsetsTO[i]);
        if (functionIndex != codePointersTO.end())
        {
            CodeSection section;
            if (FindStartEndCode(functionIndex, codePointersTO, codeSections, section))
            {
                const BYTE *pStartCode = &scriptResource[section.begin];
                const BYTE *pEndCode = &scriptResource[section.end];
                DisassembleCode(object.GetVersion(), out, pLookups, pOFLookups, pScriptThings, &object, pStartCode, pEndCode, functionOffsetsTO[i], analyzeInstruction);
            }
            else
            {
                out << "CORRUPT SCRIPT" << endl;
            }

        }
        out << "    )" << endl << endl;
    }
    out << ")" << endl;
}

const char c_indent[] = "    ";


void DisassembleFunction(const CompiledScript &script, std::ostream &out, ICompiledScriptLookups *pLookups, IObjectFileScriptLookups *pOFLookups, uint16_t wCodeOffsetTO, set<uint16_t> &sortedCodePointersTO)
{
    out << "(procedure proc_" << setw(4) << setfill('0') << wCodeOffsetTO << endl;

    set<uint16_t>::const_iterator codeStartIt = sortedCodePointersTO.find(wCodeOffsetTO);
    ASSERT(codeStartIt != sortedCodePointersTO.end());
    CodeSection section;
    if (FindStartEndCode(codeStartIt, sortedCodePointersTO, script._codeSections, section))
    {
        const BYTE *pBegin = &script.GetRawBytes()[section.begin];
        const BYTE *pEnd = &script.GetRawBytes()[section.end];
        DisassembleCode(script.GetVersion(), out, pLookups, pOFLookups, &script, nullptr, pBegin, pEnd, wCodeOffsetTO, nullptr);
    }
    else
    {
        out << "CORRUPT SCRIPT" << endl;
    }
    out << ')' << endl << endl;
}


void DisassembleScript(const CompiledScript &script, std::ostream &out, ICompiledScriptLookups *pLookups, IObjectFileScriptLookups *pOFLookups, const ILookupNames *pWords, const std::string *pMessage, AnalyzeInstructionPtr analyzeInstruction)
{
    // (script x)
    if (pMessage)
    {
        out << *pMessage << endl;
    }
    out << "(script " << script.GetScriptNumber() << ")" << endl << endl;

    // Internal strings
    out << "(string" << endl;
    out << hex;
    assert(script._strings.size() == script._stringsOffset.size());
    for (size_t i = 0; i < script._strings.size(); i++)
    {
        out << c_indent << "string_" << setw(4) << setfill('0') << script._stringsOffset[i] << " \"" << script._strings[i] << "\"" << endl;
    }
    out << ")" << endl << endl;


    // Prepare the saids.
    out << "(said" << endl;
    script.PopulateSaidStrings(pWords);
    for (size_t i = 0; i < script._saidStrings.size(); i++)
    {
        //    said_0x03EC 'look/tree'
        out << c_indent << "said_" << setw(4) << setfill('0') << script._saidsOffset[i] << " " << script._saidStrings[i] << endl;
    }
    out << ")" << endl << endl;


    // Synonyms
    if (script._synonyms.size() > 0)
    {
        out << "(synonym" << endl;
        for (const auto &syn : script._synonyms)
        {
            out << c_indent << pWords->Lookup(syn.first) << " = " << pWords->Lookup(syn.second).c_str() << endl;
        }
        out << ")" << endl << endl;
    }

    // Local variables
    out << "(local" << endl;
    for (size_t i = 0; i < script._localVars.size(); i++)
    {
        out << c_indent << _GetLocalVariableName((int)i) << " = $" << setw(4) << setfill('0') << script._localVars[i] << endl;
    }
    out << ")" << endl << endl;


    // Now its time for code.
    // Make an index of code pointers by looking at the object methods
    set<uint16_t> codePointersTO;
    for (auto &object : script._objects)
    {
        const vector<uint16_t> &methodPointersTO = object->GetMethodCodePointersTO();
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
    set<uint16_t> internalProcOffsetsTO = script.FindInternalCallsTO();//_rgRawCode, _wCodePosTO, _wCodeLength);
    // Before adding these though, remove any exports from the internalProcOffsets.
    for (const auto &exporty : script._exportsTO)
    {
        set<uint16_t>::iterator internalsIndex = find(internalProcOffsetsTO.begin(), internalProcOffsetsTO.end(), exporty);
        if (internalsIndex != internalProcOffsetsTO.end())
        {
            // Remove this guy.
            internalProcOffsetsTO.erase(internalsIndex);
        }
    }
    // Now add the internal guys to the full list
    codePointersTO.insert(internalProcOffsetsTO.begin(), internalProcOffsetsTO.end());
    // Now we know the length of each code segment (assuming none overlap)

    // Spit out code segments - first, the objects (instances, classes)
    for (auto &object : script._objects)
    {
        DisassembleObject(*object, out, pLookups, pOFLookups, &script, script.GetRawBytes(), script._codeSections, codePointersTO, analyzeInstruction);
        out << endl;
    }
    out << endl;

    // Now the exported procedures.
    for (size_t i = 0; i < script._exportsTO.size(); i++)
    {
        // _exportsTO, in addition to containing code pointers for public procedures, also
        // contain the Rm class.  Filter these out by ignoring code pointers which point outside
        // the codesegment.
        if (script.IsExportAProcedure(script._exportsTO[i]))
        {
            std::string strProcName = "";
            if (pOFLookups)
            {
                strProcName = pOFLookups->ReverseLookupPublicExportName(script.GetScriptNumber(), (uint16_t)i);
            }
            out << dec;
            out << "// EXPORTED procedure #" << (int)i << " (" << strProcName << ")" << endl;
            out << hex;
            DisassembleFunction(script, out, pLookups, pOFLookups, script._exportsTO[i], codePointersTO);
        }
    }
    out << endl;

    // Now the internal procedures (REVIEW - possibly overlap with exported ones)
    set<uint16_t>::iterator it = internalProcOffsetsTO.begin();
    while (it != internalProcOffsetsTO.end())
    {
        DisassembleFunction(script, out, pLookups, pOFLookups, *it, codePointersTO);
        it++;
    }
}