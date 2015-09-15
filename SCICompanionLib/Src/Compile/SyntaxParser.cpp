#include "stdafx.h"
#include "SyntaxParser.h"

// Our parser global variable
SyntaxParser g_Parser;

bool SyntaxParser::ParseAC(sci::Script &script, CCrystalScriptStream::const_iterator &streamIt, std::unordered_set<std::string> preProcessorDefines, SyntaxContext *pContext)
{
    bool fRet = false;
    if (script.Language() == LangSyntaxSCIStudio)
    {
        _sci.Load();
        if (script.IsHeader())
        {
        }
        else
        {
            fRet = _sci.Parse(script, streamIt, preProcessorDefines, *pContext);
        }
    }
    return fRet;
}

bool SyntaxParser::Parse(sci::Script &script, CCrystalScriptStream &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pLog, bool fParseComments, SyntaxContext *pContext, bool addCommentsToOM)
{
    bool fRet = false;
    if (script.Language() == LangSyntaxSCIStudio)
    {
        _sci.Load();
        if (script.IsHeader())
        {
            fRet = _sci.ParseHeader(script, stream.begin(), preProcessorDefines, pLog);
        }
        else
        {
            if (pContext)
            {
                // Someone is doing a partial compile (e.g. tooltips) and supply their own context.
                fRet = _sci.Parse(script, stream.begin(), preProcessorDefines, *pContext);
            }
            else
            {
                // Or maybe someone either wants error logs:
                fRet = _sci.Parse(script, stream.begin(), preProcessorDefines, pLog, addCommentsToOM);
            }
        }
    }
    else if (script.Language() == LangSyntaxCpp)
    {
        // This code path is only used for the kernels. CPP-based syntax is not currently supported in SCI Companion.
        _cpp.Load();

        // Tokenize it first.
        CrystalScriptTokenStream tokenStream(stream);
        tokenStream.Tokenize(fParseComments ? &script : nullptr);
        if (script.IsHeader())
        {
            fRet = _cpp.ParseHeader(script, tokenStream.begin(), pLog);
        }
        else
        {
            fRet = _cpp.Parse(script, tokenStream.begin(), pLog);
        }
    }
    else
    {
        ASSERT(FALSE);
    }
    return fRet;
}
