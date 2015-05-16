#include "stdafx.h" 
#include "ScriptOMAll.h"
#include "DecompilerCore.h" 

using namespace std;
using namespace sci;


bool IsStatementImmediateValue(const SingleStatement &statement, WORD &wValue)
{
    const PropertyValue *pValue = statement.CastSyntaxNode<PropertyValue>();
    bool fRet = (pValue && (pValue->GetType() == ValueType::Number));
    if (fRet)
    {
        wValue = pValue->GetNumberValue();
    }
    return fRet;
}

// If the statement is an immediate value that maps to a selector, returns the PropertyValue for the immediate.
PropertyValue *GetPropertyValueMaybeSelector(SingleStatement &statement, DecompileLookups &lookups, string &selector)
{
    PropertyValue *pValue = statement.CastSyntaxNode<PropertyValue>();
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
    const SingleStatement *pOne = proc.GetParameter(0);
    const SingleStatement *pTwo = proc.GetParameter(1);
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
                unique_ptr<SingleStatement> pNewStatement = std::make_unique<SingleStatement>();
                unique_ptr<PropertyValue> pNewValue = std::make_unique<PropertyValue>();
                pNewValue->SetValue(text, ValueType::String);
                pNewStatement->SetSyntaxNode(std::move(pNewValue));
                SingleStatementVector parameters;
                parameters.push_back(std::move(pNewStatement));
                proc.StealParams(parameters); // Do the swap
            }
            parameterIndex++;
        }
    }

    // Now go through and look for immediate values that we can turn into selectors
    while (parameterIndex < proc.GetStatements().size())
    {
        SingleStatement *param = proc.GetParameter(parameterIndex);
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
                SingleStatement *maybeAnObject = proc.GetParameter(parameterIndex);
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

void MassageProcedureCalls(DecompileLookups &lookups, sci::Script &script)
{
    // Now the actual calls, which could be to any script
    MassageProcedureCallsWorker massageProcCalls(lookups);
    script.Traverse(&massageProcCalls, massageProcCalls);
}
