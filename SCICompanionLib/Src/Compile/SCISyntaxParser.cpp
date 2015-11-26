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

ParserSCI SCISyntaxParser::char_p(const char *psz) { return ParserSCI(CharP, psz); }
ParserSCI SCISyntaxParser::operator_p(const char *psz) { return ParserSCI(OperatorP, psz); }
ParserSCI SCISyntaxParser::keyword_p(const char *psz) { return ParserSCI(KeywordP, psz); }

SCISyntaxParser::SCISyntaxParser() :
    oppar(char_p("(")),
    clpar(char_p(")")),
    opbracket(char_p("[")),
    clbracket(char_p("]")),
    pound(char_p("#")),
    atsign(char_p("@")),
    comma(char_p(",")),
    colon(char_p(":")),
    equalSign(char_p("=")),
    question(char_p("?")),
    alphanum_p(AlphanumP),
    filename_p(FilenameP),
    asmInstruction_p(AsmInstructionP),
    alphanum_p2(AlphanumPNoKeyword),
    alwaysmatch_p(AlwaysMatchP),
    bracestring_p(BraceStringP),
    squotedstring_p(SQuotedStringP),
    quotedstring_p(QuotedStringP),
    integer_p(IntegerExpandedP)
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

    // An integer or plain alphanumeric token
    immediateValue = integer_p[PropValueIntA] | alphanum_p[PropValueStringA<ValueType::Token>] | bracestring_p[PropValueStringA<ValueType::Token>];
    string_immediateValue = integer_p[PropValueIntA] | alphanum_p[PropValueStringA<ValueType::Token>] | quotedstring_p[{ PropValueStringA<ValueType::String>, ParseAutoCompleteContext::Block}];
    string_immediateValue2 = integer_p[PropValueIntA] | alphanum_p[PropValueStringA<ValueType::Token>] | quotedstring_p[{PropValueStringA<ValueType::String>, ParseAutoCompleteContext::Block}] | bracestring_p[PropValueStringA<ValueType::Token>];

    // Top level constructs
    include = keyword_p("include") >> (quotedstring_p | filename_p)[{AddIncludeA, ParseAutoCompleteContext::Block}];
    use = keyword_p("use") >> (quotedstring_p | filename_p)[{AddUseA, ParseAutoCompleteContext::ScriptName}];
    scriptNum = keyword_p("script") >> immediateValue[{ScriptNumberA, ParseAutoCompleteContext::DefineValue }];

    define = keyword_p("define")[CreateDefineA] >> alphanum_p[DefineLabelA] >> integer_p[DefineValueA];

    // An array initializer
    array_init = opbracket >> *(string_immediateValue2[ScriptVarInitAutoExpandA]) >> clbracket;  // [0 0 $4c VIEW_EGO]

    // Variable declaration, with optional array size (array size must be numeric!)
    // A   or  [A 6]   or [A MY_ARRAY_SIZE]
    var_decl =
        (opbracket >> alphanum_p[CreateVarDeclA] >> (integer_p[VarDeclSizeA] | alphanum_p2[VarDeclSizeConstantA])[VarDeclSizeErrorA] >> clbracket) |
        alphanum_p[CreateVarDeclA];

    script_var = keyword_p("local")
        >> *((var_decl[CreateScriptVarA] >> -(equalSign[GeneralE] >> (string_immediateValue2[ScriptVarInitA] | array_init)))[FinishScriptVarA]);

    // StartFunctionTempVarA is needed to reset "was initializer value set". There are no initializer values for function variables in this syntax.
    function_var_decl = keyword_p("&tmp")[StartFunctionTempVarA] >> +(var_decl[FinishFunctionTempVarA]);

    // (procname param param &tmp tmp tmp tmp)
    procedure_base = oppar
        >> alphanum_p[FunctionNameA]
        >> *alphanum_p[FunctionParameterA]
        >> *function_var_decl
        >> clpar[GeneralE];
        //>> *statement[FunctionStatementA];

    procedure_decl = keyword_p("procedure")[CreateProcedureA] >> procedure_base[{FunctionCloseA, ParseAutoCompleteContext::Block}];

    // TODO:
    entire_script = *(oppar[GeneralE]
    >> (include
        | use
        | define[FinishDefineA]
        | procedure_decl[FinishProcedureA]
        | scriptNum
        | script_var)[{IdentifierE, ParseAutoCompleteContext::TopLevelKeyword}]
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