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
#include "SyntaxParser.h"
#include "StudioSyntaxParser.h"
#include "CPPSyntaxParser.h"
#include "SCISyntaxParser.h"

// Our parser global variables
StudioSyntaxParser g_studio;
CPPSyntaxParser g_cpp;
SCISyntaxParser g_sci;

void InitializeSyntaxParsers()
{
    g_sci.Load();
    g_studio.Load();
    g_cpp.Load();
}

bool SyntaxParser_ParseAC(sci::Script &script, CCrystalScriptStream::const_iterator &streamIt, std::unordered_set<std::string> preProcessorDefines, SyntaxContext *pContext)
{
    bool fRet = false;

    if (script.Language() == LangSyntaxStudio)
    {
        if (script.IsHeader())
        {
        }
        else
        {
            fRet = g_studio.Parse(script, streamIt, preProcessorDefines, *pContext);
        }
    }
    else if (script.Language() == LangSyntaxSCI)
    {
        if (script.IsHeader())
        {
        }
        else
        {
            fRet = g_sci.Parse(script, streamIt, preProcessorDefines, *pContext);
        }
    }
    return fRet;
}

bool SyntaxParser_Parse(sci::Script &script, CCrystalScriptStream &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pLog, bool fParseComments, SyntaxContext *pContext, bool addCommentsToOM)
{
    bool fRet = false;
    if (script.Language() == LangSyntaxStudio)
    {
        if (script.IsHeader())
        {
            fRet = g_studio.ParseHeader(script, stream.begin(), preProcessorDefines, pLog);
        }
        else
        {
            if (pContext)
            {
                // Someone is doing a partial compile (e.g. tooltips) and supply their own context.
                fRet = g_studio.Parse(script, stream.begin(), preProcessorDefines, *pContext);
            }
            else
            {
                // Or maybe someone either wants error logs:
                fRet = g_studio.Parse(script, stream.begin(), preProcessorDefines, pLog, addCommentsToOM);
            }
        }
    }
    else if (script.Language() == LangSyntaxCpp)
    {
        // This code path is only used for the kernels. CPP-based syntax is not currently supported in SCI Companion.
        // Tokenize it first.
        CrystalScriptTokenStream tokenStream(stream);
        tokenStream.Tokenize(fParseComments ? &script : nullptr);
        if (script.IsHeader())
        {
            fRet = g_cpp.ParseHeader(script, tokenStream.begin(), pLog);
        }
        else
        {
            fRet = g_cpp.Parse(script, tokenStream.begin(), pLog);
        }
    }
    else if (script.Language() == LangSyntaxSCI)
    {
        if (script.IsHeader())
        {
            fRet = g_sci.ParseHeader(script, stream.begin(), preProcessorDefines, pLog);
        }
        else
        {
            if (pContext)
            {
                // Someone is doing a partial compile (e.g. tooltips) and supply their own context.
                fRet = g_sci.Parse(script, stream.begin(), preProcessorDefines, *pContext);
            }
            else
            {
                // Or maybe someone either wants error logs:
                fRet = g_sci.Parse(script, stream.begin(), preProcessorDefines, pLog, addCommentsToOM);
            }
        }

    }
    else
    {
        assert(false);
    }
    return fRet;
}
