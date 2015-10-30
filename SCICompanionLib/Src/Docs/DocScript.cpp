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
#include "DocScript.h"
#include "ScriptOMAll.h"

struct CommentInfo
{
    int startLine;
    int endLine;
    std::string comment;
};

//
// Predicate for sorting syntax nodes by position.
//
bool SortByPosition(sci::SyntaxNode *p1, sci::SyntaxNode *p2)
{
    return p1->GetPosition() < p2->GetPosition();
}

const sci::Script *DocScript::GetScript()
{
    return _script;
}

std::string DocScript::GetComment(const sci::SyntaxNode *node)
{
    return _nodeToComment[node];
}

// If all non-empty lines have a tab, removes the tab at the beginning of each line
std::string Untabify(const std::string &in)
{
    // Find min number of tabs
    auto it = in.begin();
    int minTabCount = (std::numeric_limits<int>::max)();
    do
    {
        // Skip empty lines.
        while ((it != in.end()) && (*it == '\n'))
        {
            ++it;
        }

        if (it != in.end())
        {
            // Now we're at the beginning of a non-empty line
            int tabCount = 0;
            while ((it != in.end()) && (*it == '\t'))
            {
                tabCount++;
                ++it;
            }
            minTabCount = min(minTabCount, tabCount);
            // Move to next line...
            while ((it != in.end()) && (*it != '\n'))
            {
                ++it;
            }
        }
    } while ((minTabCount > 0) && (it != in.end()));

    // Remove from each line!
    std::string output;
    if (minTabCount > 0)
    {
        it = in.begin();
        while (it != in.end())
        {
            // Skip empty lines.
            while ((it != in.end()) && (*it == '\n'))
            {
                ++it;
            }

            if (it != in.end())
            {
                it += minTabCount;
                // Find the end of the line
                auto itEndLine = it;
                while ((itEndLine != in.end()) && (*itEndLine != '\n'))
                {
                    ++itEndLine;
                }
                std::copy(it, itEndLine, std::back_inserter(output));
                output.push_back('\n');
                it = itEndLine;
            }
        };
    }
    return output;
}

DocScript::DocScript(const sci::Script &script)
{
    _script = &script;

    std::vector<CommentInfo> comments;

    // Coalesce adjacent comments and strip //, */.
    for (auto &comment : script.GetComments())
    {
        size_t lastIndex = comments.size() - 1;
        int start = comment->GetLineNumber();
        if (!comments.empty() && (comments[lastIndex].endLine + 1 == start))
        {
            // Append to the last one.
            comments[lastIndex].comment += "\r\n";
            comments[lastIndex].comment += comment->GetSanitizedText();
            comments[lastIndex].endLine = comment->GetEndLineNumber();
        }
        else
        {
            CommentInfo commentInfo = { comment->GetLineNumber(), comment->GetEndLineNumber(), Untabify(comment->GetSanitizedText()) };
            comments.push_back(commentInfo);
        }
    }

    // Assemble the list of syntax nodes
    std::vector<sci::SyntaxNode*> nodes;
    for (auto &classDef : script.GetClasses())
    {
        // Add the class
        nodes.push_back(classDef.get());

        // Now add all its methods
        const sci::MethodVector &methods = classDef->GetMethods();
        transform(methods.begin(), methods.end(), back_inserter(nodes),
            [](const std::unique_ptr<sci::MethodDefinition> &method) { return method.get(); }
        );

        // And the properties (they can have comments too)
        sci::ClassPropertyVector &properties = classDef->GetPropertiesNC();
        transform(properties.begin(), properties.end(), back_inserter(nodes),
            [](const std::unique_ptr<sci::ClassProperty> &prop) { return prop.get(); }
        );

    }
    // Now the procedures.
    const sci::ProcedureVector &procs = script.GetProcedures();
    transform(procs.begin(), procs.end(), back_inserter(nodes),
        [](const std::unique_ptr<sci::ProcedureDefinition> &proc) { return proc.get(); }
    );

    // And script variables - but only for main
    if (script.GetScriptNumber() == 0)
    {
        const sci::VariableDeclVector &vars = script.GetScriptVariables();
        transform(vars.begin(), vars.end(), back_inserter(nodes),
            [](const std::unique_ptr<sci::VariableDecl> &var) { return var.get(); }
        );
    }

    // Now we'll sort by position.
    sort(nodes.begin(), nodes.end(), SortByPosition);

    // Now both the SyntaxNodes and the comments are in order.
    // Let's look at end syntax node.
    std::vector<CommentInfo>::const_iterator commentIt = comments.begin();
    // Special case: is there a comment right at the top of the file?  Assign it to the script.
    if ((commentIt != comments.end()) && (commentIt->startLine == 0))
    {
        _nodeToComment[&script] = commentIt->comment;
    }
    for (sci::SyntaxNode *pNode : nodes)
    {
        int line = pNode->GetPosition().Line();
        // Is there a comment immediately above it?
        while ((commentIt != comments.end()) && ((commentIt->startLine == line) || (commentIt->endLine < line)))
        {
            if ((commentIt->startLine == line) || (commentIt->endLine == (line - 1)))
            {
                // It's right above the syntax node, or next to it.
                // Parse it into lines.
                _nodeToComment[pNode] = commentIt->comment;
                ++commentIt;
                break;
            }
            ++commentIt;
        }
    }
}