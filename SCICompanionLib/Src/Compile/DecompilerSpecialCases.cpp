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
#include "ScriptOMAll.h"
#include "DecompilerCore.h" 
#include "DecompilerConfig.h" 
#include <format.h>

using namespace std;
using namespace sci;

bool IsStatementImmediateValue(const SyntaxNode &statement, WORD &wValue)
{
    const PropertyValue *pValue = SafeSyntaxNode<PropertyValue>(&statement);
    bool fRet = (pValue && (pValue->GetType() == ValueType::Number));
    if (fRet)
    {
        wValue = pValue->GetNumberValue();
    }
    return fRet;
}

// If the statement is an immediate value that maps to a selector, returns the PropertyValue for the immediate.
PropertyValue *GetPropertyValueMaybeSelector(SyntaxNode &statement, DecompileLookups &lookups, string &selector)
{
    PropertyValue *pValue = SafeSyntaxNode<PropertyValue>(&statement);
    bool fRet = (pValue && (pValue->GetType() == ValueType::Number));
    if (fRet)
    {
        selector = lookups.LookupSelectorName(pValue->GetNumberValue());
        if (!selector.empty())
        {
            return pValue;
        }
    }
    return nullptr;
}

bool IsValueAnObject(uint16_t num)
{
    return num > 1000;
}

// Print params seem to have been pretty consistent through SCI0/SCI1 games.
struct PrintParamInfo
{
    string selectorName;
    int paramCount;
};
PrintParamInfo c_PrintParams[] =
{
    { "mode", 1 },
    { "width", 1 },
    { "title", 1 },
    { "font", 1 },
    { "button", 2 },
    { "time", 1 },
    { "at", 2 },
    { "draw", 0 },
    { "edit", 2 },
    { "window", 1 },
};

bool _DoesProcedureStartWithTextTuple(ProcedureCall &proc, DecompileLookups &lookups, bool replace, std::string &text)
{
    bool isTextTuple = false;
    if (proc.GetStatements().size() >= 2)
    {
        const SyntaxNode *pOne = proc.GetParameter(0);
        const SyntaxNode *pTwo = proc.GetParameter(1);
        WORD wOne;
        WORD wTwo;
        size_t parameterIndex = 1;

        if (pOne && IsStatementImmediateValue(*pOne, wOne) &&
            pTwo && IsStatementImmediateValue(*pTwo, wTwo))
        {
            // If the first two parameters are immediates, and the first one matches the current
            // script number, then assume it's a text resource.
            if (wOne == lookups.GetScriptNumber())
            {
                text = lookups.LookupTextResource(wTwo);
                if (!text.empty())
                {
                    isTextTuple = true;
                    if (replace)
                    {
                        unique_ptr<PropertyValue> pNewValue = std::make_unique<PropertyValue>();
                        pNewValue->SetValue(text, ValueType::ResourceString);

                        auto &parameters = proc.GetStatements();
                        // Erase the second parameter
                        parameters.erase(parameters.begin() + 1);
                        // Replace the first one with our new string
                        parameters[0] = std::move(pNewValue);
                    }
                }
            }
        }
    }
    return isTextTuple;
}

void _MassagePrint(ProcedureCall &proc, DecompileLookups &lookups)
{
    size_t parameterIndex = 1;
    std::string dummy;
    if (_DoesProcedureStartWithTextTuple(proc, lookups, false, dummy))
    {
        parameterIndex++;
    }

    // Now go through and look for immediate values that we can turn into selectors
    while (parameterIndex < proc.GetStatements().size())
    {
        SyntaxNode *param = proc.GetParameter(parameterIndex);
        string selectorName;
        PropertyValue *maybeSelector = GetPropertyValueMaybeSelector(*param, lookups, selectorName);
        if (maybeSelector)
        {
            auto it = find_if(c_PrintParams, c_PrintParams + ARRAYSIZE(c_PrintParams),
                [&selectorName](PrintParamInfo &info) { return info.selectorName == selectorName; }
                );
            if (it != (c_PrintParams + ARRAYSIZE(c_PrintParams)))
            {
                parameterIndex += (it->paramCount + 1);
            }
            else if ((selectorName == "icon") || (selectorName == "dispose"))
            {
                parameterIndex++;
                // Is the next parameter an object?
                SyntaxNode *maybeAnObject = proc.GetParameter(parameterIndex);
                uint16_t immValue;
                bool nextParamIsObject = (maybeAnObject && IsStatementImmediateValue(*maybeAnObject, immValue) && IsValueAnObject(immValue));
                if (selectorName == "icon")
                {
                    parameterIndex += (nextParamIsObject ? 1 : 3);
                }
                else // dispose
                {
                    parameterIndex += (nextParamIsObject ? 1 : 0);
                }
            }
            else
            {
                // We don't know what this is, so just bail...
                break;
            }
            maybeSelector->SetValue(selectorName, ValueType::Selector);
        }
        else
        {
            break;
        }
    }
}

// Print(100 44) becomes Print("Hello there")
void _SubstituteTextTuples(ProcedureCall &proc, DecompileLookups &lookups, std::string &text, bool replace)
{
    if (lookups.GetDecompilerConfig()->IsTextResourceTupleProcedure(proc.GetName()))
    {
        _DoesProcedureStartWithTextTuple(proc, lookups, replace, text);
    }
}

//
// Try to massage the parameters so that it makes more sense to the user.
//
void _MassageProcedureCall(ProcedureCall &proc, DecompileLookups &lookups)
{
    if (proc.GetName() == "Print")
    {
        _MassagePrint(proc, lookups);
    }
}

class MassageProcedureCallsWorker : public IExploreNode
{
public:
    MassageProcedureCallsWorker(DecompileLookups &lookups) : _lookups(lookups) {}

    void ExploreNode(SyntaxNode &node, ExploreNodeState state) override
    {
        if (state == ExploreNodeState::Pre)
        {
            ProcedureCall *procCall = SafeSyntaxNode<ProcedureCall>(&node);
            if (procCall)
            {
                std::string text;
                _SubstituteTextTuples(*procCall, _lookups, text, _lookups.SubstituteTextTuples);
                if (!text.empty() && !_lookups.SubstituteTextTuples)
                {
                    // Add a comment after the Print (or whatever) statement
                    // (Assuming no nested ones, so only need to track one at a time)
                    _nextToPutInComment = text;
                }
                _MassageProcedureCall(*procCall, _lookups);
            }

            _callStack.push_back(&node);
        }
        else if (state == ExploreNodeState::Post)
        {
            if (!_nextToPutInComment.empty())
            {
                // Stick in the next statements node we find.
                // Sucks that I have to do it this way. I'd rather to be able to just
                // cast directly to a StatementsNode.
                StatementsNode *statements = nullptr;
                switch (_callStack.back()->GetNodeType())
                {
                    case NodeTypeFunction:
                        statements = static_cast<FunctionBase*>(_callStack.back());
                        break;
                    case NodeTypeWhileLoop:
                        statements = static_cast<WhileLoop*>(_callStack.back());
                        break;
                    case NodeTypeDoLoop:
                        statements = static_cast<DoLoop*>(_callStack.back());
                        break;
                    case NodeTypeCodeBlock:
                        statements = static_cast<CodeBlock*>(_callStack.back());
                        break;
                    case NodeTypeForLoop:
                        statements = static_cast<ForLoop*>(_callStack.back());
                        break;
                }
                if (statements)
                {
                    statements->GetStatements().push_back(std::make_unique<Comment>(_nextToPutInComment, CommentType::Indented));
                    _nextToPutInComment.clear();
                }
            }

            _callStack.pop_back();
        }
    }

private:
    std::string _nextToPutInComment;
    std::vector<SyntaxNode*> _callStack;
    DecompileLookups &_lookups;
};

void MassageProcedureCalls(DecompileLookups &lookups, sci::Script &script)
{
    // Now the actual calls, which could be to any script
    MassageProcedureCallsWorker massageProcCalls(lookups);
    script.Traverse(massageProcCalls);
}
