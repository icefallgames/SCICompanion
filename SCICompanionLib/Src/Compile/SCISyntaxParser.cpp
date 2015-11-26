#include "stdafx.h"
#include "ScriptOMAll.h"
#include "StudioSyntaxParser.h"
#include "SCISyntaxParser.h"
#include "ParserActions.h"
#include "Operators.h"
#include "OperatorTables.h"

using namespace sci;
using namespace std;

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

// Selectors, as far as I can tell, look like this:
// A-Za-z0-9_-
// But they must have at least one letter and not start with a number
template<typename _It>
bool SelectorP(const ParserSCI *pParser, SyntaxContext *pContext, _It &stream)
{
    bool fRet = false;
    std::string &str = pContext->ScratchString();
    str.clear();
    char ch = *stream;
    bool hadAlpha = isalpha(ch);
    if (hadAlpha || (ch == '_') || (ch == '-'))     // First character must be a letter or _ or -
    {
        fRet = true;
        str += ch;
        ch = *(++stream);
        while (isalnum(ch) || (ch == '_') || (ch == '-'))  // Then any alphanumeric character is fine.
        {
            hadAlpha = hadAlpha || isalpha(ch);
            fRet = true;
            str += ch;
            ch = *(++stream);
        }
    }
    return fRet && hadAlpha;
}

// foo: or foo?, no whitespace allowed.
template<char terminator, typename _It>
bool SelectorP_Term(const ParserSCI *pParser, SyntaxContext *pContext, _It &stream)
{
    bool fRet = SelectorP(pParser, pContext, stream);
    return fRet && (*(++stream) == terminator);
}

template<typename _TOpType>
_TOpType SCINameToOperator(const std::string &name);

template<>
BinaryOperator SCINameToOperator<BinaryOperator>(const std::string &name)
{
    return NameToOperator(name, sciNameToBinaryOp);
}

template<>
AssignmentOperator SCINameToOperator<AssignmentOperator>(const std::string &name)
{
    return NameToOperator(name, sciNameToAssignmentOp);
}

template<>
UnaryOperator SCINameToOperator<UnaryOperator>(const std::string &name)
{
    return NameToOperator(name, sciNameToUnaryOp);
}

template<typename _T, typename _TOperator>
void SetOperatorA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->CreateSyntaxNode<_T>(stream);
        _TOperator op = SCINameToOperator<_TOperator>(pContext->ScratchString());
        assert(op != _TOperator::None);
        pContext->GetSyntaxNode<_T>()->Operator = op;
    }
}

void SetRepeatStatementA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->CreateSyntaxNode<WhileLoop>(stream);
        pContext->GetSyntaxNode<WhileLoop>()->SetPosition(stream.GetPosition());
        // A repeat loop is just a while (TRUE) loop
        // 1 = TRUE
        pContext->GetSyntaxNode<WhileLoop>()->SetCondition(make_unique<ConditionalExpression>(make_unique<PropertyValue>(1)));
    }
}

void FinishBreakIfA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // We should have an If statement. Add the current thing to its conditional.
        pContext->GetSyntaxNode<IfStatement>()->SetCondition(make_unique<ConditionalExpression>(move(pContext->StatementPtrReturn)));
        // And its contents should be a CodeBlock with a single Break statement.
        auto codeBlock = make_unique<CodeBlock>();
        codeBlock->AddStatement(make_unique<BreakStatement>());
        pContext->GetSyntaxNode<IfStatement>()->SetStatement1(move(codeBlock));
    }
}

ParserSCI SCISyntaxParser::char_p(const char *psz) { return ParserSCI(CharP, psz); }
ParserSCI SCISyntaxParser::operator_p(const char *psz) { return ParserSCI(OperatorP, psz); }
ParserSCI SCISyntaxParser::keyword_p(const char *psz) { return ParserSCI(KeywordP, psz); }
ParserSCI SCISyntaxParser::generateSyntaxNodeD()
{
    ParserSCI syntaxnode(SyntaxNodeD);
    return syntaxnode[EndSyntaxNodeA];
}

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
    selector_p(SelectorP),
    selector_send_p(SelectorP_Term<':'>),
    propget_p(SelectorP_Term<'?'>),
    filename_p(FilenameP),
    asmInstruction_p(AsmInstructionP),
    alphanum_p2(AlphanumPNoKeyword),
    alwaysmatch_p(AlwaysMatchP),
    bracestring_p(BraceStringP),
    squotedstring_p(SQuotedStringP),
    quotedstring_p(QuotedStringP),
    integer_p(IntegerExpandedP),
    syntaxnode_d(generateSyntaxNodeD())
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

    general_token = (alphanum_p2 | bracestring_p)[{nullptr, ParseAutoCompleteContext::None, "TOKEN"}];

    selector_literal = pound >> selector_p[{nullptr, ParseAutoCompleteContext::Selector}];

    pointer = atsign;
    
    rvalue_variable =
        (opbracket >> general_token[ComplexValueStringA<ValueType::Token>] >> statement[ComplexValueIndexerA] >> clbracket) |
        general_token[ComplexValueStringA<ValueType::Token>];

    value =
        alwaysmatch_p[SetStatementA<ComplexPropertyValue>]
        >> (integer_p[ComplexValueIntA]
        | quotedstring_p[{ComplexValueStringA<ValueType::String>, ParseAutoCompleteContext::Block}]
        | squotedstring_p[{ComplexValueStringA<ValueType::Said>, ParseAutoCompleteContext::Block}]
        | (-pointer[ComplexValuePointerA] >> rvalue_variable)
        | selector_literal[ComplexValueStringA<ValueType::Selector>]);

    repeat_statement =
        keyword_p("repeat")[SetRepeatStatementA]
        >> *statement[AddStatementA<WhileLoop>];

    return_statement =
        keyword_p("return")[SetStatementA<ReturnStatement>]
        >> -statement[StatementBindTo1stA<ReturnStatement, nullptr>];

    break_statement =
        keyword_p("break")[SetStatementA<BreakStatement>];

    breakif_statement =
        keyword_p("breakif")[SetStatementA<IfStatement>] 
        >> statement[FinishBreakIfA];

    // Operators
    binary_operator = operator_p("u>=") | operator_p(">=") | operator_p("u>") | operator_p(">>") |
        operator_p(">") | operator_p("u<=") | operator_p("<=") |
        operator_p("u<") | operator_p("!=") | operator_p("<<") |
        operator_p("<") | operator_p("==") | operator_p("+") |
        operator_p("-") | operator_p("*") | operator_p("/") |
        operator_p("mod") | operator_p("&") | operator_p("|") |
        operator_p("^");

    unary_operator = keyword_p("~") | keyword_p("not") | keyword_p("-") |
        operator_p("++") | operator_p("--");

    assignment_operator = operator_p("+=") | operator_p("-=") | operator_p("*=") |
        operator_p("/=") | operator_p("mod=") | operator_p("&=") |
        operator_p("|=") | operator_p("^=") | operator_p(">>=") |
        operator_p("<<=") | operator_p("=");
    
    // blarg    or   [blarg statement]
    lvalue = (opbracket >> general_token[{SetStatementNameA<LValue>, ParseAutoCompleteContext::LValue}] >> statement[LValueIndexerA] >> clbracket) |
        general_token[{SetStatementNameA<LValue>, ParseAutoCompleteContext::LValue}];

    rest_statement =
        keyword_p("&rest")[SetStatementA<RestStatement>]
        >> -alphanum_p[SetStatementNameA<RestStatement>];

    code_block =
        oppar[SetStatementA<CodeBlock>]
        >> *statement[AddStatementA<CodeBlock>]
        >> clpar[GeneralE];

    assignment =
        assignment_operator[SetOperatorA<Assignment, AssignmentOperator>]
        >> syntaxnode_d[lvalue[AssignmentVariableA]]
        >> statement[StatementBindTo1stA<Assignment, errArgument>];

    // + 5 view
    binary_operation =
        binary_operator[SetOperatorA<BinaryOp, BinaryOperator>]
        >> statement[StatementBindTo1stA<BinaryOp, errArgument>]
        >> statement[StatementBindTo2ndA<BinaryOp, errBinaryOp>];

    // ~ signal
    unary_operation =
        unary_operator[SetOperatorA<UnaryOp, UnaryOperator>]
        >> statement[StatementBindTo1stA<UnaryOp, errArgument>];
    
    // All possible statements.
    statement = alwaysmatch_p[StartStatementA]
        >>
        (
            (oppar >>
            // Add more here
            (
            repeat_statement |
            assignment |
            unary_operation |       // REVIEW: Must come before proc call?
            binary_operation |
            return_statement |
            rest_statement |
            breakif_statement |
            break_statement |
            code_block
            ) >>
            clpar)
        | value
        )[{FinishStatementA, ParseAutoCompleteContext::Value}];


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
        >> clpar[GeneralE]
        >> *statement[FunctionStatementA];

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