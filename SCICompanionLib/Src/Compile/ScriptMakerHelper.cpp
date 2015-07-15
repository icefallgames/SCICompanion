#include "stdafx.h"
#include "ScriptOM.h"
#include "ScriptOMAll.h"
#include "ScriptMakerHelper.h"

using namespace sci;
using namespace std;

void _SetSendVariableTarget(SendCall &send, const std::string &target)
{
    unique_ptr<LValue> lValue = make_unique<LValue>();
    lValue->SetName(target);
    send.SetLValue(move(lValue));
}

unique_ptr<SingleStatement> _MakeNumberStatement(int16_t w)
{
    unique_ptr<ComplexPropertyValue> pValue = std::make_unique<ComplexPropertyValue>();
    pValue->SetValue((uint16_t)w);
    if (w < 0)
    {
        pValue->Negate();
    }
    unique_ptr<SingleStatement> temp = make_unique<SingleStatement>();
    temp->SetSyntaxNode(move(pValue));
    return temp;
}

unique_ptr<SingleStatement> _MakeTokenStatement(const string &token)
{
    unique_ptr<ComplexPropertyValue> pValue = std::make_unique<ComplexPropertyValue>();
    pValue->SetValue(token, ValueType::Token);
    unique_ptr<SingleStatement> temp = make_unique<SingleStatement>();
    temp->SetSyntaxNode(std::move(pValue));
    return temp;
}

void _AddAssignment(MethodDefinition &method, const string &lvalueName, const string &assigned)
{
    unique_ptr<Assignment> pEquals = std::make_unique<Assignment>();
    pEquals->SetName("=");
    unique_ptr<LValue> lvalue = make_unique<LValue>();
    lvalue->SetName(lvalueName);
    pEquals->SetVariable(move(lvalue));
    pEquals->SetStatement1(std::move(_MakeTokenStatement(assigned)));
    _AddStatement(method, std::move(pEquals));
}

void _AddBasicSwitch(MethodDefinition &method, const string &switchValue, const string &case0Comments)
{
    unique_ptr<SwitchStatement> pSwitch = std::make_unique<SwitchStatement>();

    // Make the switch value and add it to the swtich
    pSwitch->SetStatement1(std::move(_MakeTokenStatement(switchValue)));

    // Make the case statement and add it to the switch
    unique_ptr<CaseStatement> pCase = std::make_unique<CaseStatement>();
    pCase->SetStatement1(move(_MakeNumberStatement(0)));
    _AddComment(*pCase, case0Comments);
    pSwitch->AddCase(move(pCase));
    pCase.release();

    // Add the switch to the method
    _AddStatement(method, std::move(pSwitch));
}

// parameter may be empty.
void _AddSendCall(MethodDefinition &method, const string &objectName, const string &methodName, const string &parameter, bool isVariable)
{
    unique_ptr<SendCall> pSend = std::make_unique<SendCall>();
    if (isVariable)
    {
        _SetSendVariableTarget(*pSend, objectName);
    }
    else
    {
        pSend->SetName(objectName);
    }

    // Create the send param to add to the send call
    unique_ptr<SendParam> pParam = std::make_unique<SendParam>();
    pParam->SetName(methodName);
    pParam->SetIsMethod(true);

    if (!parameter.empty())
    {
        // Add the parameter to the sendparam.
        unique_ptr<ComplexPropertyValue> pValue = std::make_unique<ComplexPropertyValue>();
        pValue->SetValue(parameter, ValueType::Token);
        _AddStatement(*pParam, std::move(pValue));
    }

    pSend->AddSendParam(move(pParam));
    pParam.release();
    _AddStatement(method, std::move(pSend));
}