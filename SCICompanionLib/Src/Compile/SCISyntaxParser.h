#pragma once

#include "ScriptOM.h"
#include "CrystalScriptStream.h"
#include "ParserCommon.h"
#include "AutoCompleteContext.h"
#include "StudioSyntaxParser.h"
#include <stack>
#include <deque>

// Our parser...
typedef ParserBase<SyntaxContext, streamIt, EatCommentCpp> ParserSCI;

class SCISyntaxParser
{
public:
    SCISyntaxParser();

    bool Parse(sci::Script &script, CCrystalScriptStream::const_iterator &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pError, bool addCommentsToOM);
    bool Parse(sci::Script &script, CCrystalScriptStream::const_iterator &stream, std::unordered_set<std::string> preProcessorDefines, SyntaxContext &context);
    bool ParseHeader(sci::Script &script, CCrystalScriptStream::const_iterator &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pError);
    void Load();

private:
    bool _fLoaded;


    ParserSCI entire_script;

    ParserSCI include;
    ParserSCI scriptNum;



    // TODO:
    ParserSCI entire_header;

    // Utility
    static ParserSCI char_p(const char *psz);
    static ParserSCI keyword_p(const char *psz);
    static ParserSCI operator_p(const char *psz);

    ParserSCI alphanum_p;
    ParserSCI asmInstruction_p;
    ParserSCI alphanum_p2;// (AlphanumPNoKeyword);
    ParserSCI alwaysmatch_p;
    ParserSCI alphanumopen_p;
    ParserSCI bracestring_p;
    ParserSCI squotedstring_p;
    ParserSCI quotedstring_p;
    ParserSCI oppar;
    ParserSCI clpar;
};

