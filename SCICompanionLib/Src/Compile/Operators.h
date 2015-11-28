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

enum class BinaryOperator
{
    None,
    Equal,
    NotEqual,
    UnsignedGreaterEqual,
    GreaterEqual,
    UnsignedGreaterThan,
    GreaterThan,
    UnsignedLessEqual,
    LessEqual,
    UnsignedLessThan,
    LessThan,
    ShiftRight,
    ShiftLeft,
    Add,
    Subtract,
    Multiply,
    Divide,
    Mod,
    BinaryAnd,
    BinaryOr,
    ExclusiveOr,
    LogicalAnd,
    LogicalOr,
};

enum class AssignmentOperator
{
    None,
    Assign,
    Add,
    Subtract,
    Multiply,
    Divide,
    Mod,
    BinaryAnd,
    BinaryOr,
    ExclusiveOr,
    ShiftRight,
    ShiftLeft,
};

enum class UnaryOperator
{
    None,
    BinaryNot,
    LogicalNot,
    Negate,
    Increment,
    Decrement
};

template<typename _TOperator>
_TOperator NameToOperator(const std::string &name, const std::vector<std::pair<std::string, _TOperator>> &table)
{
    auto itFind = find_if(table.begin(), table.end(),
        [&name](const std::pair<std::string, _TOperator> &entry) { return name == entry.first; }
    );
    if (itFind != table.end())
    {
        return itFind->second;
    }
    return _TOperator::None;
}

template<typename _TOperator>
std::string OperatorToName(_TOperator op, const std::vector<std::pair<std::string, _TOperator>> &table)
{
    auto itFind = find_if(table.begin(), table.end(),
        [op](const std::pair<std::string, _TOperator> &entry) { return op == entry.second; }
    );
    if (itFind != table.end())
    {
        return itFind->first;
    }
    return "INVALID_OPERATOR";
}

bool IsRelational(BinaryOperator op);