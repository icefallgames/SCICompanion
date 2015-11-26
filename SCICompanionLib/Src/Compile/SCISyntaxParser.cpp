#include "stdafx.h"
#include "ScriptOMAll.h"
#include "StudioSyntaxParser.h"
#include "SCISyntaxParser.h"
#include "ParserActions.h"

using namespace sci;

// Remains to be seen if we actually need these.
class CondClauseStatement : public CaseStatementBase
{
    DECLARE_NODE_TYPE(NodeTypeCondClause)

public:
    // IOutputByteCode
    CodeResult OutputByteCode(CompileContext &context) const;

    void Accept(ISyntaxNodeVisitor &visitor) const override;
};

class CondStatement : public SyntaxNode, public OneStatementNode
{
    DECLARE_NODE_TYPE(NodeTypeCond)
public:
    // IOutputByteCode
    CodeResult OutputByteCode(CompileContext &context) const;
    void PreScan(CompileContext &context);
    void Traverse(IExploreNode &en);

    void AddClause(std::unique_ptr<CondClauseStatement> pCase) { _clauses.push_back(std::move(pCase)); }

    void Accept(ISyntaxNodeVisitor &visitor) const override;

    std::vector<std::unique_ptr<CondClauseStatement>> _clauses;

private:
    CondStatement(const CondStatement &src) = delete;
    CondStatement& operator=(const CondStatement& src) = delete;
};

class BreakIfStatement : public SyntaxNode, public OneStatementNode
{
    DECLARE_NODE_TYPE(NodeTypeBreakIf)
public:
    // IOutputByteCode
    CodeResult OutputByteCode(CompileContext &context) const;
    void PreScan(CompileContext &context);
    void Traverse(IExploreNode &en);

    void Accept(ISyntaxNodeVisitor &visitor) const override;
};

class RepeatStatement : public SyntaxNode, public StatementsNode
{
    DECLARE_NODE_TYPE(NodeTypeRepeat)
public:
    // IOutputByteCode
    CodeResult OutputByteCode(CompileContext &context) const;
    void PreScan(CompileContext &context);
    void Traverse(IExploreNode &en);

    void Accept(ISyntaxNodeVisitor &visitor) const override;
};

template<typename _It>
void IdentifierE(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const _It &stream)
{
    if (!match.Result())
    {
        pContext->ReportError("Expected keyword.", stream);
    }
}

ParserSCI SCISyntaxParser::char_p(const char *psz) { return ParserSCI(CharP, psz); }
ParserSCI SCISyntaxParser::operator_p(const char *psz) { return ParserSCI(OperatorP, psz); }
ParserSCI SCISyntaxParser::keyword_p(const char *psz) { return ParserSCI(KeywordP, psz); }

SCISyntaxParser::SCISyntaxParser() :
    oppar(char_p("(")),
    clpar(char_p(")")),
    alphanum_p(AlphanumP),
    asmInstruction_p(AsmInstructionP),
    alphanum_p2(AlphanumPNoKeyword),
    alwaysmatch_p(AlwaysMatchP),
    alphanumopen_p(AlphanumOpenP),
    bracestring_p(BraceStringP),
    squotedstring_p(SQuotedStringP),
    quotedstring_p(QuotedStringP)
{
}

//
// The constructor sets up the parse tree
//
void SCISyntaxParser::Load()
{
    if (_fLoaded)
    {
        return;
    }
    _fLoaded = true;

    include = keyword_p("include") >> quotedstring_p[{AddIncludeA, ParseAutoCompleteContext::Block}];

    // TODO:
    entire_script = *(oppar[GeneralE]
    >> (include
        | scriptNum)[{IdentifierE, ParseAutoCompleteContext::TopLevelKeyword}]
        >> clpar[GeneralE]);
    }

//
// This does the parsing.
//
bool SCISyntaxParser::Parse(Script &script, streamIt &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pError, bool addCommentsToOM)
{
    SyntaxContext context(stream, script, preProcessorDefines, addCommentsToOM);
    bool fRet = false;
    if (entire_script.Match(&context, stream).Result() && (*stream == 0)) // Needs a full match
    {
        fRet = true;
    }
    else
    {
        // With regards to syntax errors - there can really only be one, because we can't
        // recover afterwards.
        std::string strError = "  [Error]: ";
        strError += context.GetErrorText();
        streamIt errorPos = context.GetErrorPosition();
        ScriptId scriptId(script.GetPath().c_str());
        if (pError)
        {
            // Add one to line#, since editor lines are 1-based
            pError->ReportResult(CompileResult(strError, scriptId, errorPos.GetLineNumber() + 1, errorPos.GetColumnNumber(), CompileResult::CRT_Error));
        }
    }
    return fRet;
}

bool SCISyntaxParser::Parse(Script &script, streamIt &stream, std::unordered_set<std::string> preProcessorDefines, SyntaxContext &context)
{
    bool fRet = false;
    if (entire_script.Match(&context, stream).Result() && (*stream == 0)) // Needs a full match
    {
        fRet = true;
    }
    return fRet;
}

bool SCISyntaxParser::ParseHeader(Script &script, streamIt &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pError)
{
    SyntaxContext context(stream, script, preProcessorDefines, false);
    bool fRet = entire_header.Match(&context, stream).Result() && (*stream == 0);
    if (!fRet)
    {
        std::string strError = context.GetErrorText();
        streamIt errorPos = context.GetErrorPosition();
        ScriptId scriptId(script.GetPath().c_str());
        if (pError)
        {
            pError->ReportResult(CompileResult(strError, scriptId, errorPos.GetLineNumber() + 1, errorPos.GetColumnNumber(), CompileResult::CRT_Error));
        }
    }
    return fRet;
}