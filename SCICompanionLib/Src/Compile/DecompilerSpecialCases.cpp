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

void _MassagePrint(ProcedureCall &proc, DecompileLookups &lookups)
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
            std::string text = lookups.LookupTextResource(wTwo);
            if (!text.empty())
            {
                unique_ptr<PropertyValue> pNewValue = std::make_unique<PropertyValue>();
                pNewValue->SetValue(text, ValueType::String);
                SyntaxNodeVector parameters;
                parameters.push_back(std::move(pNewValue));
                proc.StealParams(parameters); // Do the swap
            }
            parameterIndex++;
        }
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

class MassageProcedureCallsWorker : public IExploreNodeContext, public IExploreNode
{
public:
    MassageProcedureCallsWorker(DecompileLookups &lookups) : _lookups(lookups) {}

    void ExploreNode(IExploreNodeContext *pContext, SyntaxNode &node, ExploreNodeState state) override
    {
        if (state == ExploreNodeState::Pre)
        {
            ProcedureCall *procCall = SafeSyntaxNode<ProcedureCall>(&node);
            if (procCall)
            {
                _MassageProcedureCall(*procCall, _lookups);
            }
        }
    }

private:
    DecompileLookups &_lookups;
};

#if TEST_REST_USAGE
int g_goodRest = 0;
int g_badRest = 0;

class TestForRestWorker : public IExploreNodeContext, public IExploreNode
{
public:
    TestForRestWorker(DecompileLookups &lookups) : _lookups(lookups) {}

    void ExploreNode(IExploreNodeContext *pContext, SyntaxNode &node, ExploreNodeState state) override
    {
        if (state == ExploreNodeState::Pre)
        {
            switch (node.GetNodeType())
            {
                case NodeTypeRest:
                {
                    RestStatement &rest = static_cast<RestStatement&>(node);
                    // temp test is rest.GetName always the last of the func params?
                    size_t size = _functionContext->GetSignatures()[0]->GetParams().size();
                    if (_functionContext->GetSignatures()[0]->GetParams()[size - 1]->GetName() != rest.GetName())
                    {
                        g_goodRest++;
                    }
                    else
                    {
                        g_badRest++;
                    }

                }
                break;

                case NodeTypeFunction:
                {
                    _functionContext = static_cast<FunctionBase*>(&node);
                }
                break;
            }
        }
        else if (state == ExploreNodeState::Post)
        {
            switch (node.GetNodeType())
            {
                case NodeTypeFunction:
                {
                    _functionContext = nullptr;
                }
                break;
            }
        }
    }

private:
    FunctionBase *_functionContext;
    DecompileLookups &_lookups;
};
#endif


void MassageProcedureCalls(DecompileLookups &lookups, sci::Script &script)
{
    // Now the actual calls, which could be to any script
    MassageProcedureCallsWorker massageProcCalls(lookups);
    script.Traverse(&massageProcCalls, massageProcCalls);
}

#if TEST_REST_USAGE
void TestForRest(DecompileLookups &lookups, sci::Script &script)
{
    // Now the actual calls, which could be to any script
    TestForRestWorker massageProcCalls(lookups);
    script.Traverse(&massageProcCalls, massageProcCalls);
}
#endif