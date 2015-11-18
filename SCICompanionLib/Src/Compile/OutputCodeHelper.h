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

#include "ScriptOMAll.h"

//
// Handles an indentation scope
//
const int cIndent = 4;
class DebugIndent
{
public:
    DebugIndent(sci::SourceCodeWriter &out) : _out(out)
    {
        _out.iIndent += cIndent;
    }
    ~DebugIndent()
    {
        _out.iIndent -= cIndent;
    }
private:
    sci::SourceCodeWriter &_out;
};

bool IsNonAlphaOperator(const std::string &op);
void _OutputVariableAndSize(sci::ISyntaxNodeVisitor &visitor, sci::SourceCodeWriter &out, const std::string &type, const std::string &name, WORD wSize, const sci::SingleStatementVector &initValues);
std::string UnescapeString(std::string src);

std::string CleanToken(const std::string &src, std::unordered_set<std::string> *disallowedList = nullptr);

class BracketScope
{
public:
    BracketScope(sci::SourceCodeWriter &out, bool useBrackets) : _out(out)
    {
        _oldUseBrackets = _out.fUseBrackets;
        _out.fUseBrackets = useBrackets;
    }
    ~BracketScope()
    {
        _out.fUseBrackets = _oldUseBrackets;
    }
private:
    sci::SourceCodeWriter &_out;
    bool _oldUseBrackets;
};

//
// Handles a bracket scope (enforce order of operations)
//
class OutputBrackets
{
public:
    OutputBrackets(sci::SourceCodeWriter &out) : _out(out)
    {
        if (_out.fUseBrackets)
        {
            _out.out << "(";
        }
    }
    ~OutputBrackets()
    {
        if (_out.fUseBrackets)
        {
            _out.out << ")";
        }
    }
private:
    sci::SourceCodeWriter &_out;
};


//
// Handles an "inline" scope (don't put statements on new lines)
//
class Inline
{
public:
    Inline(sci::SourceCodeWriter &out, bool fInline) : _out(out)
    {
        _fOld = _out.fInline;
        _out.fInline = fInline;
    }
    ~Inline()
    {
        _out.fInline = _fOld;
    }
private:
    bool _fOld;
    sci::SourceCodeWriter &_out;
};


//
// Indicates that order of operations must be made explicit (or not)
//
class ExplicitOrder
{
public:
    ExplicitOrder(sci::SourceCodeWriter &out, bool fUseBrackets) : _out(out)
    {
        _fOld = _out.fUseBrackets;
        _out.fUseBrackets = fUseBrackets;
    }
    ~ExplicitOrder()
    {
        _out.fUseBrackets = _fOld;
    }
private:
    bool _fOld;
    sci::SourceCodeWriter &_out;
};

void Indent(sci::SourceCodeWriter &out);

void NewLine(sci::SourceCodeWriter &out);

//
// Indicates that order of operations must be made explicit (or not)
//
class ExpandNextCodeBlock
{
public:
    ExpandNextCodeBlock(sci::SourceCodeWriter &out, bool fExpand) : _out(out)
    {
        _fOld = _out.fExpandCodeBlock;
        _out.fExpandCodeBlock = fExpand;
    }
    ~ExpandNextCodeBlock()
    {
        _out.fExpandCodeBlock = _fOld;
    }
private:
    bool _fOld;
    sci::SourceCodeWriter &_out;
};

//
// Adds sci::SourceCodeWriter.iIndent spaces to the stream, if we're putting things on new lines.
//
class DebugLine
{
public:
    DebugLine(sci::SourceCodeWriter &out) : _out(out)
    {
        if (!_out.fInline)
        {
            Indent(_out);
        }
    }
    ~DebugLine()
    {
        if (!_out.fInline)
        {
            _out.EnsureNewLine();
        }
    }
private:
    sci::SourceCodeWriter &_out;
};

template<typename _TStream>
void _OutputNumber(_TStream &out, WORD wNum, bool fHex, bool fNegate)
{
    if (fNegate && (wNum > (std::numeric_limits<int16_t>::max)()))
    {
        assert(!fHex);
        wNum = (~wNum) + 1; // two's complement
        out << "-";
    }
    out << (fHex ? std::hex : std::dec);
    if (fHex)
    {
        out << "$" << std::setw(4) << std::setfill('0') << wNum;
    }
    else
    {
        out << wNum;
    }
    // Don't contaminate stream state:
    out << std::dec;
}

class CheckForReturnValue : public sci::IExploreNode
{
public:
    std::string GetSugggestReturnValue() const { return _retValue.empty() ? "void" : _retValue; }

private:
    void ExploreNode(sci::IExploreNodeContext *pContext, sci::SyntaxNode &node, sci::ExploreNodeState state)
    {
        if (state == sci::ExploreNodeState::Pre)
        {
            if (_retValue.empty() && (node.GetNodeType() == sci::NodeTypeReturn))
            {
                sci::ReturnStatement &returnStatement = static_cast<sci::ReturnStatement&>(node);
                const sci::SingleStatement *pRetValue = returnStatement.GetValue();
                if (pRetValue && (pRetValue->GetSyntaxNode() != nullptr))
                {
                    // TODO: we could do a little more deduction and determine the actual type.
                    _retValue = "var";
                }
            }
        }
    }

    std::string _retValue;
};

std::string _DeduceReturnType(sci::FunctionBase &function);

void EndStatement(sci::SourceCodeWriter &out);
