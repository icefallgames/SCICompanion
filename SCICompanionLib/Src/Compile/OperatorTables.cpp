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
#include "OperatorTables.h"

// Mappings for strings <-> operators for various languages.

using namespace std;

// These all obey the associative property, so an n-ary set of these
// can be constructed using a tree of binary operations.
// e.g. (+ 1 2 3)  is equivalent to (+ 1 (+ 2 3))
vector<BinaryOperator> allowMultiple =
{
    BinaryOperator::LogicalAnd,
    BinaryOperator::LogicalOr,
    BinaryOperator::Add,
    BinaryOperator::Multiply,
    BinaryOperator::ExclusiveOr,
    BinaryOperator::BinaryAnd,
    BinaryOperator::BinaryOr,
};

// These are n-ary operators that are more complicated to implement (i.e. they can't
// be constructed from nested binary operations and require special treatment by the
// compiler).
vector<BinaryOperator> relational =
{
    BinaryOperator::Equal,
    BinaryOperator::NotEqual,
    BinaryOperator::UnsignedGreaterEqual,
    BinaryOperator::GreaterEqual,
    BinaryOperator::UnsignedGreaterThan,
    BinaryOperator::GreaterThan,
    BinaryOperator::UnsignedLessEqual,
    BinaryOperator::LessEqual,
    BinaryOperator::UnsignedLessThan,
    BinaryOperator::LessThan,
};

bool IsRelational(BinaryOperator op)
{
    return find(relational.begin(), relational.end(), op) != relational.end();
}

bool AllowMultiple(BinaryOperator op)
{
    return find(allowMultiple.begin(), allowMultiple.end(), op) != allowMultiple.end();
}

// SCI Studio syntax operators
vector<pair<string, BinaryOperator>> studioNameToBinaryOp =
{
    { "==", BinaryOperator::Equal },
    { "<>", BinaryOperator::NotEqual },
    { ">=u", BinaryOperator::UnsignedGreaterEqual },
    { ">=", BinaryOperator::GreaterEqual },
    { ">u", BinaryOperator::UnsignedGreaterThan },
    { ">>", BinaryOperator::ShiftRight },
    { ">", BinaryOperator::GreaterThan },
    { "<=u", BinaryOperator::UnsignedLessEqual },
    { "<=", BinaryOperator::LessEqual },
    { "<u", BinaryOperator::UnsignedLessThan },
    { "<<", BinaryOperator::ShiftLeft },
    { "<", BinaryOperator::LessThan },
    { "+", BinaryOperator::Add },
    { "-", BinaryOperator::Subtract },
    { "*", BinaryOperator::Multiply },
    { "/", BinaryOperator::Divide },
    { "%", BinaryOperator::Mod },
    { "&", BinaryOperator::BinaryAnd },
    { "|", BinaryOperator::BinaryOr },
    { "^", BinaryOperator::ExclusiveOr },
    { "and", BinaryOperator::LogicalAnd },
    { "or", BinaryOperator::LogicalOr },
};

vector<pair<string, AssignmentOperator>> studioNameToAssignmentOp =
{
    { "=", AssignmentOperator::Assign },
    { "+=", AssignmentOperator::Add },
    { "-=", AssignmentOperator::Subtract },
    { "*=", AssignmentOperator::Multiply },
    { "/=", AssignmentOperator::Divide },
    { "%=", AssignmentOperator::Mod },
    { "&=", AssignmentOperator::BinaryAnd },
    { "|=", AssignmentOperator::BinaryOr },
    { "^=", AssignmentOperator::ExclusiveOr },
    { ">>=", AssignmentOperator::ShiftRight },
    { "<<=", AssignmentOperator::ShiftLeft },
};

vector<pair<string, UnaryOperator>> studioNameToUnaryOp =
{
    { "not", UnaryOperator::LogicalNot },
    { "bnot", UnaryOperator::BinaryNot },
    { "++", UnaryOperator::Increment },
    { "--", UnaryOperator::Decrement },
    { "neg", UnaryOperator::Negate },
};


// Sierra Script syntax operators
vector<pair<string, BinaryOperator>> sciNameToBinaryOp =
{
    { "==", BinaryOperator::Equal },
    { "!=", BinaryOperator::NotEqual },
    { "u>=", BinaryOperator::UnsignedGreaterEqual },
    { ">=", BinaryOperator::GreaterEqual },
    { "u>", BinaryOperator::UnsignedGreaterThan },
    { ">>", BinaryOperator::ShiftRight },
    { ">", BinaryOperator::GreaterThan },
    { "u<=", BinaryOperator::UnsignedLessEqual },
    { "<=", BinaryOperator::LessEqual },
    { "u<", BinaryOperator::UnsignedLessThan },
    { "<<", BinaryOperator::ShiftLeft },
    { "<", BinaryOperator::LessThan },
    { "+", BinaryOperator::Add },
    { "-", BinaryOperator::Subtract },
    { "*", BinaryOperator::Multiply },
    { "/", BinaryOperator::Divide },
    { "mod", BinaryOperator::Mod },
    { "&", BinaryOperator::BinaryAnd },
    { "|", BinaryOperator::BinaryOr },
    { "^", BinaryOperator::ExclusiveOr },
    { "and", BinaryOperator::LogicalAnd },
    { "or", BinaryOperator::LogicalOr },
};

vector<pair<string, AssignmentOperator>> sciNameToAssignmentOp =
{
    { "=", AssignmentOperator::Assign },
    { "+=", AssignmentOperator::Add },
    { "-=", AssignmentOperator::Subtract },
    { "*=", AssignmentOperator::Multiply },
    { "/=", AssignmentOperator::Divide },
    { "mod=", AssignmentOperator::Mod },
    { "&=", AssignmentOperator::BinaryAnd },
    { "|=", AssignmentOperator::BinaryOr },
    { "^=", AssignmentOperator::ExclusiveOr },
    { ">>=", AssignmentOperator::ShiftRight },
    { "<<=", AssignmentOperator::ShiftLeft },
};

vector<pair<string, UnaryOperator>> sciNameToUnaryOp =
{
    { "not", UnaryOperator::LogicalNot },
    { "~", UnaryOperator::BinaryNot },
    { "++", UnaryOperator::Increment },
    { "--", UnaryOperator::Decrement },
    { "-", UnaryOperator::Negate },
};