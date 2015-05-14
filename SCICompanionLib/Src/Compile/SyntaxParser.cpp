#include "stdafx.h"
#include "SyntaxParser.h"

// Our parser global variable
SyntaxParser g_Parser;

bool SyntaxParser::Parse(sci::Script &script, CCrystalScriptStream &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pLog, bool fParseComments, SyntaxContext *pContext)
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
            if (pLog)
            {
	            // Someone either wants error logs:
	            fRet = _sci.Parse(script, stream.begin(), preProcessorDefines, pLog);
            }
            else
            {
	            // Or they are doing a partial compile (e.g. tooltips) and supply their own context.
	            fRet = _sci.Parse(script, stream.begin(), preProcessorDefines, *pContext);
            }
        }
    }
    else if (script.Language() == LangSyntaxCpp)
    {
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
