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

#include "SCIStudioSyntaxParser.h"
#include "CPPSyntaxParser.h"

class SyntaxParser
{
public:
	bool Parse(sci::Script &script, CCrystalScriptStream &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pLog = nullptr, bool fParseComments = false, SyntaxContext *pContext = nullptr, bool addCommentsToOM = false);
    bool ParseAC(sci::Script &script, CCrystalScriptStream::const_iterator &streamIt, std::unordered_set<std::string> preProcessorDefines, SyntaxContext *pContext);

private:
    SCISyntaxParser _sci;
    CPPSyntaxParser _cpp;
};

extern SyntaxParser g_Parser;
