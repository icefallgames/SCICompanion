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
#include "AppState.h"
#include "OutputCodeHelper.h"
#include "OutputSCIStudioSourceCode.h"
#include "OutputCPPSourceCode.h"

using namespace sci;
using namespace std;


std::string OperatorToString(const std::string &op)
{
    std::stringstream output;
    output << "operator '" << op << "'";
    return output.str();
}

std::string BinaryOp::ToString() const
{
    return OperatorToString(GetOpName());
}

std::string UnaryOp::ToString() const
{
    return OperatorToString(GetOpName());
}


//
// Comment tracking
//
CommentTracker::CommentTracker(Script &script) : _comments(script.GetComments()), _commentIndex(0)
{
}

void CommentTracker::Sync(const sci::SyntaxNode *pNode, SourceCodeWriter &out)
{
    if (_commentIndex < _comments.size())
    {
        LineCol commentPosition = _comments[_commentIndex]->GetEndPosition();
        LineCol nodePosition = pNode->GetPosition(); // Note: can't use end position, it's not consistently set.
        while ((_commentIndex < _comments.size()) && (commentPosition < nodePosition))
        {
            out.out << _comments[_commentIndex]->GetName();
            NewLine(out);
            ++_commentIndex;
            if (_commentIndex < _comments.size())
            {
                commentPosition = _comments[_commentIndex]->GetEndPosition();
                nodePosition = pNode->GetPosition();
            }
        }
    }
}

void Script::OutputSourceCode(SourceCodeWriter &out) const
{
    if (out.lang == LangSyntaxSCIStudio)
    {
        OutputSourceCode_SCIStudio(*this, out);
    }
    else if (out.lang == LangSyntaxCpp)
    {
        OutputSourceCode_CPP(*this, out);
    }
}

void ClassDefinition::OutputSourceCode(SourceCodeWriter &out) const
{
    if (out.lang == LangSyntaxSCIStudio)
    {
        OutputSourceCode_SCIStudio(*this, out);
    }
    else if (out.lang == LangSyntaxCpp)
    {
        OutputSourceCode_CPP(*this, out);
    }
}

void MethodDefinition::OutputSourceCode(SourceCodeWriter &out) const
{
    if (out.lang == LangSyntaxSCIStudio)
    {
        OutputSourceCode_SCIStudio(*this, out);
    }
    else if (out.lang == LangSyntaxCpp)
    {
        OutputSourceCode_CPP(*this, out);
    }
}

void ClassProperty::OutputSourceCode(SourceCodeWriter &out) const
{
    if (out.lang == LangSyntaxSCIStudio)
    {
        OutputSourceCode_SCIStudio(*this, out);
    }
    else if (out.lang == LangSyntaxCpp)
    {
        OutputSourceCode_CPP(*this, out);
    }
}
