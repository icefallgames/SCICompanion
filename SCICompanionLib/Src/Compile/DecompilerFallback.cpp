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
#include "stdafx.h"
#include "scii.h"
#include "DecompilerFallback.h"
#include "ScriptOMAll.h"
#include "DecompilerCore.h"
#include "format.h"
#include "AppState.h"

using namespace std;
using namespace sci;

const char InvalidLookupError[] = "LOOKUP_ERROR";

// We want to generate a function with a vardecl and an asmblock. We won't be able to do var name detection, but we should have var usage detection.
// Actually, that's handled automatically afterwards, so we can ignore link

set<uint16_t> CalcBranchLabels(code_pos cur, code_pos end)
{
    set<uint16_t> labels;
    while (cur != end)
    {
        if (cur->_is_branch_instruction())
        {
            code_pos target  = cur->get_branch_target();
            labels.insert(target->get_final_offset());
        }
        ++cur;
    }
    return labels;
}

string _GetLabelName(uint16_t offset)
{
    return fmt::format("code_{:04x}", offset);
}

void _AddSyntaxNode(StatementsNode &statementsNode, unique_ptr<SyntaxNode> syntaxNode)
{
    unique_ptr<SingleStatement> pStatement = std::make_unique<SingleStatement>();
    pStatement->SetSyntaxNode(move(syntaxNode));
    statementsNode.AddStatement(std::move(pStatement));
}

void _AddString(StatementsNode &statementsNode, const string &token, ValueType type)
{
    _AddSyntaxNode(statementsNode,
        make_unique<PropertyValue>(
        token, type)
        );
}

void _AddToken(StatementsNode &statementsNode, const string &token)
{
    _AddSyntaxNode(statementsNode,
        make_unique<PropertyValue>(
        token, ValueType::Token)
        );
}

void _AddNumber(StatementsNode &statementsNode, PropertyValue **ppValue, uint16_t number)
{
    unique_ptr<PropertyValue> value = make_unique<PropertyValue>(number);
    *ppValue = value.get();
    _AddSyntaxNode(statementsNode, move(value));
}

struct CallFrame
{
    CallFrame() : abandoned(false) {}
    CallFrame(Opcode opcode, int cParams) : opcode(opcode), cParams(cParams), abandoned(false)  {}

    Opcode opcode;
    int cParams;
    bool abandoned;

    std::vector<PropertyValue*> trackValues;

    bool IsSend()
    {
        return opcode == Opcode::SEND || opcode == Opcode::SELF || opcode == Opcode::SUPER;
    }
};

class CallFrameManager
{
public:
    CallFrameManager(DecompileLookups &lookups) : _lookups(lookups) {}

    void Push(code_pos code)
    {
        Consumption consumption = _GetInstructionConsumption(*code, &_lookups);
        assert(consumption.cStackConsume);
        callFrames.emplace(code->get_opcode(), consumption.cStackConsume);
    }

    // Property value may be null. Opcode may be any
    void MaybeReportValue(code_pos code, PropertyValue *value)
    {
        Consumption consumption = _GetInstructionConsumption(*code, &_lookups);
        if (consumption.cStackGenerate && !callFrames.empty())
        {
            callFrames.top().trackValues.push_back(value);
            callFrames.top().cParams--;
            if (callFrames.top().cParams == 0)
            {
                if (callFrames.top().IsSend() && !callFrames.top().abandoned)
                {
                    AssignSelectors(callFrames.top());
                }
                callFrames.pop();
            }
        }
        else if (code->_is_branch_instruction() && !callFrames.empty())
        {
            // Branching messes us up, abandon this frame
            callFrames.top().abandoned = true;
        }
    }

    void AssignSelectors(const CallFrame &frame)
    {
        int cParamsRemaining = 0;
        bool lookingForSelector = true;
        bool lookingForParamCount = false;
        int i = (int)frame.trackValues.size() - 1;
        while (i >= 0) // Iterate backward
        {
            PropertyValue *value = frame.trackValues[i];
            if (lookingForSelector)
            {
                // This should be a selector
                if (value && value->GetType() == ValueType::Number)
                {
                    string selector = _lookups.LookupSelectorName(value->GetNumberValue());
                    if (!selector.empty())
                    {
                        value->SetValue(selector, ValueType::Selector);
                    }
                }
                else
                {
                    break;  // Bail, we can't determine
                }
                lookingForSelector = false;
                lookingForParamCount = true;
            }
            else if (lookingForParamCount)
            {
                if (value && value->GetType() == ValueType::Number)
                {
                    cParamsRemaining = value->GetNumberValue();
                }
                else
                {
                    break; // bail, we can't determine
                }
                lookingForParamCount = false;
            }
            else
            {
                cParamsRemaining--;
                if (cParamsRemaining == 0)
                {
                    lookingForSelector = true;
                }
            }

        }
    }

private:
    DecompileLookups &_lookups;
    stack<CallFrame> callFrames;
};

void DisassembleFallback(FunctionBase &func, code_pos start, code_pos end, DecompileLookups &lookups)
{
    // We'll need to keep track of send frames so we can detect which ones are selectors.
    // etc... Then later we can go back and convert selectors into selectors.
    // Going through branches may mess things up, but this information on what is a selector
    // is nice to have, not essential.
    CallFrameManager callFrames(lookups);

    // We process backwards, so add asm statements to a temporary thing first.
    StatementsNode statements;

    set<uint16_t> labels = CalcBranchLabels(start, end);
    code_pos cur = end;
    --cur;
    while (cur != start)
    {
        if (cur->get_opcode() != Opcode::LINK)
        {
            PropertyValue *valueWeak = nullptr;

            unique_ptr<Asm> asmStatement = make_unique<Asm>();

            // Assign a label if appropriate
            uint16_t offset = cur->get_final_offset();
            if (contains(labels, offset))
            {
                asmStatement->SetLabel(_GetLabelName(offset));
            }

            // Instruction name
            Opcode opcode = cur->get_opcode();

            // Instruction parameters for those with them
            switch (opcode)
            {
                case Opcode::BT:
                case Opcode::BNT:
                case Opcode::JMP:
                {
                    _AddToken(*asmStatement, _GetLabelName(cur->get_branch_target()->get_final_offset()));
                    break;
                }

                case Opcode::LDI:
                case Opcode::PUSHI:
                {
                    _AddNumber(*asmStatement, &valueWeak, cur->get_first_operand());
                }
                break;

                // Turn these into PUSHI's so they can easily be tracked
                case Opcode::PUSH0:
                {
                    opcode = Opcode::PUSHI;
                    _AddNumber(*asmStatement, &valueWeak, 0);
                    break;
                }
                case Opcode::PUSH1:
                {
                    opcode = Opcode::PUSHI;
                    _AddNumber(*asmStatement, &valueWeak, 1);
                    break;
                }
                case Opcode::PUSH2:
                {
                    opcode = Opcode::PUSHI;
                    _AddNumber(*asmStatement, &valueWeak, 2);
                    break;
                }

                case Opcode::CALLK:
                {
                    _AddToken(*asmStatement, lookups.LookupKernelName(cur->get_first_operand()));
                    _AddNumber(*asmStatement, &valueWeak, cur->get_second_operand());
                    callFrames.Push(cur);
                    break;
                }

                case Opcode::CALL:
                {
                    uint16_t procOffset = cur->get_final_postop_offset() + cur->get_first_operand();
                    _AddToken(*asmStatement, _GetProcNameFromScriptOffset(procOffset));
                    _AddNumber(*asmStatement, &valueWeak, cur->get_second_operand());
                    callFrames.Push(cur);
                    // Track this call so that we can say whether or not a local proc "belongs" to a class.
                    lookups.TrackProcedureCall(procOffset);
                    break;
                }

                case Opcode::CALLB:
                {
                    _AddToken(*asmStatement, _GetBaseProcedureName(cur->get_first_operand()));
                    _AddNumber(*asmStatement, &valueWeak, cur->get_second_operand());
                    callFrames.Push(cur);
                    break;
                }

                case Opcode::CALLE:
                {
                    _AddToken(*asmStatement, _GetPublicProcedureName(cur->get_first_operand(), cur->get_second_operand()));
                    _AddNumber(*asmStatement, &valueWeak, cur->get_third_operand());
                    callFrames.Push(cur);
                    break;
                }

                case Opcode::SEND:
                case Opcode::SELF:
                {
                    _AddNumber(*asmStatement, &valueWeak, cur->get_first_operand());
                    callFrames.Push(cur);
                    break;
                }

                case Opcode::SUPER:
                {
                    _AddToken(*asmStatement, lookups.LookupClassName(cur->get_first_operand()));
                    _AddNumber(*asmStatement, &valueWeak, cur->get_second_operand());
                    callFrames.Push(cur);
                    break;
                }

                case Opcode::CLASS:
                {
                    _AddToken(*asmStatement, lookups.LookupClassName(cur->get_first_operand()));
                    break;
                }

                case Opcode::REST:
                {
                    _AddToken(*asmStatement, _GetParamVariableName(cur->get_first_operand()));
                    break;
                }

                case Opcode::LEA:
                {
                    VarScope varScope;
                    uint16_t varIndex;
                    _AddString(*asmStatement,
                        _GetVariableNameFromCodePos(*cur, lookups, &varScope, &varIndex),
                        ValueType::Pointer
                        );
                    lookups.TrackVariableUsage(varScope, varIndex, true); // Let's consider this indexed.
                    break;
                }

                case Opcode::PTOS:    // property index to stack
                case Opcode::PTOA:    // property index to acc
                case Opcode::STOP:    // stack to property index
                case Opcode::ATOP:    // acc to property index
                case Opcode::IPTOA:   // Inc prop to acc
                case Opcode::DPTOA:   // Dec prop to acc
                case Opcode::IPTOS:   // Inc prop to stack
                case Opcode::DPTOS:   // Dec prop to stack
                {
                    _AddToken(*asmStatement, lookups.LookupPropertyName(cur->get_first_operand()));
                    break;
                }

                case Opcode::LOFSA:
                case Opcode::LOFSS:
                {
                    // The first operand specifies an offset from the start of the next instruction.
                    // In SCI0 is this a relative offset from the post operation program counter.
                    // In SCI1 it appears to be an absolute offset.
                    ICompiledScriptSpecificLookups::ObjectType type;
                    SCIVersion version = appState->GetVersion();
                    uint16_t wName = version.lofsaOpcodeIsAbsolute ?
                        cur->get_first_operand() :
                        (cur->get_first_operand() + cur->get_final_postop_offset());
                    std::string name = InvalidLookupError;
                    lookups.LookupScriptThing(wName, type, name);
                    if (name.empty())
                    {
                        name = InvalidLookupError;
                        appState->LogInfo("Unable to find symbol for %d.", wName);
                    }
                    _AddString(*asmStatement, name, _ScriptObjectTypeToPropertyValueType(type));
                    break;
                }

                default:
                {
                    if ((opcode >= Opcode::LAG) && (opcode <= Opcode::LastOne))
                    {
                        VarScope varScope;
                        uint16_t varIndex;
                        _AddToken(*asmStatement,
                            _GetVariableNameFromCodePos(*cur, lookups, &varScope, &varIndex));
                        bool isIndexed = _IsVOIndexed(opcode);
                        lookups.TrackVariableUsage(varScope, varIndex, isIndexed);
                    }
                    break;
                }
            }
            asmStatement->SetName(OpcodeNames[static_cast<BYTE>(opcode)]);  // Do if after the above, because we may have changed it.
            callFrames.MaybeReportValue(cur, valueWeak);
            _AddSyntaxNode(statements, move(asmStatement));
        }
        --cur;
    }

    // Add them in reverse
    unique_ptr<AsmBlock> asmBlock = make_unique<AsmBlock>();
    while (!statements.GetStatements().empty())
    {
        asmBlock->AddStatement(move(statements.GetStatements().back()));
        statements.GetStatements().pop_back();
    }

    // Finally add the asm block to the function
    _AddSyntaxNode(func, move(asmBlock));
}
