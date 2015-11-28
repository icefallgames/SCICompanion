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

namespace sci
{
    class WeakSyntaxNode;
}

class OutputSourceCodeBase : public sci::ISyntaxNodeVisitor
{
public:
    OutputSourceCodeBase(sci::SourceCodeWriter &out) : out(out) {}

protected:
    sci::SourceCodeWriter &out;

    template<typename _T>
    void Forward(_T &things)
    {
        bool first = true;
        for (const auto &thing : things)
        {
            if (!first && out.fInline)
            {
                // If these are all inline, add whitespace between them.
                out.out << " ";
            }
            thing->Accept(*this);
            first = false;
        }
    }

    template<typename _T>
    void Forward(_T &things, const char *pszSeparator)
    {
        bool first = true;
        for (const auto &thing : things)
        {
            if (!first && out.fInline)
            {
                out.out << pszSeparator;
            }
            thing->Accept(*this);
            first = false;
        }
    }

    template<typename _T>
    void ForwardAlwaysSeparate(_T &things, const char *pszSeparator)
    {
        bool first = true;
        for (const auto &thing : things)
        {
            if (!first)
            {
                out.out << pszSeparator;
            }
            thing->Accept(*this);
            first = false;
        }
    }

    template<typename _T>
    void ForwardOptionalSection(const char *sectionName, _T &things)
    {
        if (!things.empty())
        {
            out.out << "(" << sectionName;
            {
                DebugIndent indent(out);
                out.out << endl;
                for (const auto &thing : things)
                {
                    thing->Accept(*this);
                    if (out.fInline)
                    {
                        // If these are all inline, add whitespace between them.
                        out.out << " ";
                    }
                }
                out.out << endl;
            }
            out.out << ")";
        }
    }
    void Visit(const sci::WeakSyntaxNode &weakNode);
};