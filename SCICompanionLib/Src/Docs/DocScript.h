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
    class Script;
    class Comment;
    class SyntaxNode;
};

// Provides a mapping of script nodes (classes, functions, etc...) to their accompanying comments.
class DocScript
{
public:
    DocScript(const sci::Script &script);

    const sci::Script *GetScript();
    std::string GetComment(const sci::SyntaxNode *node);

private:
    const sci::Script *_script;
    std::unordered_map<const sci::SyntaxNode*, std::string> _nodeToComment;
};

std::string Indent(const std::string &in);