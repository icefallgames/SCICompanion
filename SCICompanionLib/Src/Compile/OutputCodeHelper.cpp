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
#include "OutputCodeHelper.h"

void _OutputVariableAndSize(sci::ISyntaxNodeVisitor &visitor, sci::SourceCodeWriter &out, const std::string &type, const std::string &name, WORD wSize, const sci::SyntaxNodeVector &initValues)
{
    out.out << name;
    if (wSize > 1)
    {
        out.out << "[" << wSize << "]"; // array
    }

    if (!initValues.empty())
    {
        if (wSize > 1)
        {
            out.out << " = (";
            for (auto &initValue : initValues)
            {
                Inline inln(out, true);
                initValue->Accept(visitor);
                out.out << " ";
            }
            out.out << ")";
        }
        else
        {
            out.out << " = ";
            initValues[0]->Accept(visitor);

        }
    }
}

std::string UnescapeString(std::string src)
{
    size_t thePos;
    while ((thePos = src.find('\n')) != std::string::npos)
    {
        src.replace(thePos, 1, "\\n");
    }
    while ((thePos = src.find('\t')) != std::string::npos)
    {
        src.replace(thePos, 1, "\\t");
    }

    // Escape double-quote characters.
    size_t i = 0;
    while ((thePos = src.find('"', i)) != std::string::npos)
    {

        src.replace(thePos, 1, "\\\"");
        i = thePos + 2;
    }
    return src;
}

bool IsNonAlphaOperator(const std::string &op)
{
    return (op == "--") || (op == "++") || (op == "!");
}


void Indent(sci::SourceCodeWriter &out)
{
    std::string spaces;
    spaces.append(out.iIndent, ' ');
    out.out << spaces;
}

void NewLine(sci::SourceCodeWriter &out)
{
    if (!out.fInline)
    {
        out.NewLine();
        Indent(out);
    }
}

std::string _DeduceReturnType(sci::FunctionBase &function)
{
    CheckForReturnValue en;
    function.Traverse(nullptr, en);
    return en.GetSugggestReturnValue();
}

void EndStatement(sci::SourceCodeWriter &out)
{
    if (out.fInline)
    {
    }
    else
    {
        out.out << ";";
    }
}

// Wrap the string in braces if it doesn't start with A-Za-z_ and stuff...
std::string CleanToken(const std::string &src, std::unordered_set<std::string> *disallowedList)
{
    bool ok = true;
    // Special hack: sometimes said strings come through here
    bool isSaid = (!src.empty() && src[0] == '\'' && src.back() == '\'');
    if (!isSaid)
    {
        for (size_t i = 0; ok && (i < src.length()); i++)
        {
            if ((i == 0) && !isalpha(src[i]) && (src[i] != '_'))
            {
                ok = false;
            }
            else if ((i > 0) && !isalnum(src[i]) && (src[i] != '_'))
            {
                ok = false;
            }
        }
    }

    if (ok && disallowedList)
    {
        // Ok if not present in disallowed list.
        ok = (disallowedList->find(src) == disallowedList->end());
    }

    if (!ok)
    {
        std::string braceString = "{";
        braceString += src;
        braceString += "}";
        return braceString;
    }
    else
    {
        return src;
    }
}

DetectIfWentNonInline::DetectIfWentNonInline(sci::SourceCodeWriter &out) : _out(out), WentInline(false) { out.detectNonInLine.push_back(this); }
DetectIfWentNonInline::~DetectIfWentNonInline() { _out.detectNonInLine.pop_back(); }

Inline::Inline(sci::SourceCodeWriter &out, bool fInline) : _out(out)
{
    _fOld = _out.fInline;
    _out.fInline = fInline;
    if (!fInline)
    {
        for (auto &detect : out.detectNonInLine)
        {
            detect->WentInline = true;
        }
    }
}
Inline::~Inline()
{
    _out.fInline = _fOld;
}