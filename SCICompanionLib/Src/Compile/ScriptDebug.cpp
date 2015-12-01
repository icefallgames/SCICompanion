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
#include "OutputStudioSourceCode.h"
#include "OutputCPPSourceCode.h"
#include "OutputSCISourceCode.h"
#include "StringUtil.h"
#include "OperatorTables.h"

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
    // REVIEW: This is really language specific, but we're using the SCIStudio names.
    return OperatorToString(OperatorToName(Operator, studioNameToBinaryOp));
}

std::string UnaryOp::ToString() const
{
    // REVIEW: This is really language specific, but we're using the SCIStudio names.
    return OperatorToString(OperatorToName(Operator, studioNameToUnaryOp));
}


//
// Comment tracking
//
CommentTracker::CommentTracker(Script &script) : _comments(script.GetComments()), _commentIndex(0)
{
}

bool CommentTracker::_OutputCommentHelper(const Comment &comment, SourceCodeWriter &out)
{
    if (comment.CommentType == CommentType::Positioned)
    {
        out.IndentToCommentColumn();
    }
    else
    {
        // Just our standard indent.
        Indent(out);
    }
    out.out << comment.GetName();

    // Always go non-inline here, since a comment may comment out everything to the end of the line.
    {
        Inline outline(out, false);
        out.EnsureNewLine();
    }

    ++_commentIndex;
    return true;
}

std::string CommentTracker::GetFirstComment() const
{
    if (!_comments.empty() && _comments[0]->GetPosition().Line() == 0)
    {
        return _comments[0]->GetSanitizedText();
    }
    return "";
}

void CommentTracker::OutputInitialComment(SourceCodeWriter &out)
{
    if (_commentIndex < _comments.size())
    {
        if (_comments[_commentIndex]->GetPosition().Line() == 0)
        {
            _OutputCommentHelper(*_comments[_commentIndex], out);
        }
    }
}

bool CommentTracker::Sync(const sci::SyntaxNode *pNode, SourceCodeWriter &out, int incrementLine)
{
    bool outputComment = false;
    if (_commentIndex < _comments.size())
    {
        LineCol commentPosition = _comments[_commentIndex]->GetEndPosition();
        LineCol nodePosition = pNode->GetPosition(); // Note: can't use end position, it's not consistently set.
        nodePosition = LineCol(nodePosition.Line() + incrementLine, nodePosition.Column());
        while ((_commentIndex < _comments.size()) && (commentPosition < nodePosition))
        {
            outputComment = _OutputCommentHelper(*_comments[_commentIndex], out);

            if (_commentIndex < _comments.size())
            {
                commentPosition = _comments[_commentIndex]->GetEndPosition();
            }
        }
    }
    return outputComment;
}

SourceCodeWriter::SourceCodeWriter(std::stringstream &ss, LangSyntax syntax, Script *pScript) :
    out(ss),
    lang(syntax),
    iIndent(0),
    fInline(false),
    fLast(false),
    fUseBrackets(false),
    fExpandCodeBlock(false),
    pszNewLine("\n"),
    lastNewLineLength(0),
    disallowedTokens(nullptr),
    fAlwaysExpandCodeBlocks(false),
    defaultInlineCommentColumn(40),
    indentChar(' '),
    indentAmount(4)
{
    if (pScript)
    {
        pComments = std::make_unique<CommentTracker>(*pScript);
    }
}

void Script::OutputSourceCode(SourceCodeWriter &out) const
{
    if (out.lang == LangSyntaxStudio)
    {
        OutputSourceCode_SCIStudio(*this, out);
    }
    else if (out.lang == LangSyntaxCpp)
    {
        OutputSourceCode_CPP(*this, out);
    }
    else if (out.lang == LangSyntaxSCI)
    {
        OutputSourceCode_SCI(*this, out);
    }
}

void ClassDefinition::OutputSourceCode(SourceCodeWriter &out) const
{
    if (out.lang == LangSyntaxStudio)
    {
        OutputSourceCode_SCIStudio(*this, out);
    }
    else if (out.lang == LangSyntaxCpp)
    {
        OutputSourceCode_CPP(*this, out);
    }
    else if (out.lang == LangSyntaxSCI)
    {
        OutputSourceCode_SCI(*this, out);
    }
}

void MethodDefinition::OutputSourceCode(SourceCodeWriter &out) const
{
    if (out.lang == LangSyntaxStudio)
    {
        OutputSourceCode_SCIStudio(*this, out);
    }
    else if (out.lang == LangSyntaxCpp)
    {
        OutputSourceCode_CPP(*this, out);
    }
    else if (out.lang == LangSyntaxSCI)
    {
        OutputSourceCode_SCI(*this, out);
    }
}

void ClassProperty::OutputSourceCode(SourceCodeWriter &out) const
{
    if (out.lang == LangSyntaxStudio)
    {
        OutputSourceCode_SCIStudio(*this, out);
    }
    else if (out.lang == LangSyntaxCpp)
    {
        OutputSourceCode_CPP(*this, out);
    }
    else if (out.lang == LangSyntaxSCI)
    {
        OutputSourceCode_SCI(*this, out);
    }
}
