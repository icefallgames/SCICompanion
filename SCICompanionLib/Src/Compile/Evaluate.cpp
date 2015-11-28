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
#include "CompileContext.h"
#include "PMachine.h"
#include "Operators.h"

Opcode GetInstructionForBinaryOperator(BinaryOperator op);

// For evaluating compile time constants

using namespace sci;
using namespace std;

bool PropertyValueBase::Evaluate(ILookupDefine &context, uint16_t &result) const
{
    bool isSimpleValue = false;
    if (GetType() == ValueType::Number)
    {
        isSimpleValue = true;
        result = _numberValue;
    }
    else if (GetType() == ValueType::Token)
    {
        isSimpleValue = context.LookupDefine(_stringValue, result);
    }
    return isSimpleValue;
}

bool ComplexPropertyValue::Evaluate(ILookupDefine &context, uint16_t &result) const
{
    bool isSimpleValue = false;
    if (!_pArrayInternal)
    {
        isSimpleValue = __super::Evaluate(context, result);
    }
    return isSimpleValue;
}

bool EvalBinaryOp(Opcode opcode, uint16_t aUnsigned, uint16_t bUnsigned, uint16_t &result)
{
    bool success = true;
    // Most SCI operations are signed
    int16_t a = (int16_t)aUnsigned;
    int16_t b = (int16_t)bUnsigned;
    
    switch (opcode)
    {
        case Opcode::EQ:
            result = (a == b);
            break;
        case Opcode::NE:
            result = (a != b);
            break;
        case Opcode::UGE:
            result = (aUnsigned >= bUnsigned);
            break;
        case Opcode::GE:
            result = (a >= b);
            break;
        case Opcode::UGT:
            result = (aUnsigned > bUnsigned);
            break;
        case Opcode::SHR:
            result = (aUnsigned >> bUnsigned);
            break;
        case Opcode::GT:
            result = (a > b);
            break;
        case Opcode::ULE:
            result = (aUnsigned <= bUnsigned);
            break;
        case Opcode::LE:
            result = (a <= b);
            break;
        case Opcode::ULT:
            result = (aUnsigned < bUnsigned);
            break;
        case Opcode::SHL:
            result = (aUnsigned << bUnsigned);
            break;
        case Opcode::LT:
            result = (a < b);
            break;
        case Opcode::ADD:
            result = (a + b);
            break;
        case Opcode::SUB:
            result = (a - b);
            break;
        case Opcode::MUL:
            result = (a * b);
            break;
        case Opcode::DIV:
            result = (b == 0) ? 0 : (a / b);
            break;
        case Opcode::MOD:
            result = (b == 0) ? 0 : (a & b);
            break;
        case Opcode::AND:
            result = (aUnsigned & bUnsigned);
            break;
        case Opcode::OR:
            result = (aUnsigned | bUnsigned);
            break;
        case Opcode::XOR:
            result = (aUnsigned ^ bUnsigned);
            break;
        default:
            success = false;
            break;
    }
    return success;
}

bool BinaryOp::Evaluate(ILookupDefine &context, uint16_t &result) const
{
    uint16_t valueA;
    bool good = _statement1->Evaluate(context, valueA);
    if (good)
    {
        uint16_t valueB;
        good = _statement2->Evaluate(context, valueB);
        if (good)
        {
            good = EvalBinaryOp(GetInstructionForBinaryOperator(Operator), valueA, valueB, result);
        }
    }
    return good;
}

bool CodeBlock::Evaluate(ILookupDefine &context, uint16_t &result) const
{
    bool good = false;
    if (!GetStatements().empty())
    {
        good = GetStatements().back()->Evaluate(context, result);
    }
    return good;
}
bool NaryOp::Evaluate(ILookupDefine &context, uint16_t &result) const
{
    // Result will be TRUE or FALSE
    assert(IsRelational(Operator));
    bool good = true;
    result = 1;
    for (size_t i = 1; result && good && (i < _segments.size()); i++)
    {
        uint16_t valueA;
        bool good = _segments[i - 1]->Evaluate(context, valueA);
        if (good)
        {
            uint16_t valueB;
            good = _segments[i]->Evaluate(context, valueB);
            if (good)
            {
                good = EvalBinaryOp(GetInstructionForBinaryOperator(Operator), valueA, valueB, result);
            }
        }
    }
    return good;
}

bool ConditionalExpression::Evaluate(ILookupDefine &context, uint16_t &result) const
{
    bool good = false;
    if (_segments.size() == 1)
    {
        good = _segments[0]->Evaluate(context, result);
    }
    return good;
}