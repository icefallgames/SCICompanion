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
#include "ScriptOM.h"
#include "ScriptOMAll.h"
#include "ScriptMakerHelper.h"
#include "Operators.h"

using namespace sci;
using namespace std;

void _SetSendVariableTarget(SendCall &send, const std::string &target)
{
    unique_ptr<LValue> lValue = make_unique<LValue>();
    lValue->SetName(target);
    send.SetLValue(move(lValue));
}

unique_ptr<SyntaxNode> _MakeNumberStatement(int16_t w)
{
    unique_ptr<ComplexPropertyValue> pValue = std::make_unique<ComplexPropertyValue>();
    pValue->SetValue((uint16_t)w);
    if (w < 0)
    {
        pValue->Negate();
    }
    return unique_ptr<SyntaxNode>(move(pValue));
}

unique_ptr<SyntaxNode> _MakeTokenStatement(const string &token)
{
    unique_ptr<ComplexPropertyValue> pValue = std::make_unique<ComplexPropertyValue>();
    pValue->SetValue(token, ValueType::Token);
    return unique_ptr<SyntaxNode>(move(pValue));
}

void _AddAssignment(MethodDefinition &method, const string &lvalueName, const string &assigned)
{
    unique_ptr<Assignment> pEquals = std::make_unique<Assignment>();
    pEquals->Operator = AssignmentOperator::Assign;
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