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

#include "StudioSyntaxParser.h"

template<typename _TParser>
void FunctionStatementA(MatchResult &match, const _TParser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->FunctionPtr->AddStatement(move(pContext->StatementPtrReturn));
    }
}

template<typename _TParser>
void AddUseA(MatchResult &match, const _TParser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->Script().AddUse(pContext->ScratchString());
    }
}

template<typename _TParser>
void AddIncludeA(MatchResult &match, const _TParser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->Script().AddInclude(pContext->ScratchString());
    }
}