#include "stdafx.h"
#include "ScriptOMAll.h"
#include "StudioSyntaxParser.h"
#include "SCISyntaxParser.h"
#include "ParserActions.h"
#include "Operators.h"
#include "OperatorTables.h"

using namespace sci;
using namespace std;

void NoCaseE(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (!match.Result())
    {
        pContext->ReportError("Expected case value or 'else' keyword.", stream);
    }
}



template<typename _It>
bool IntegerNonZeroP(const ParserSCI *pParser, SyntaxContext *pContext, _It &stream)
{
    bool fRet = IntegerExpandedP(pParser, pContext, stream);
    if (fRet)
    {
        fRet = (pContext->Integer != 0);
        if (!fRet)
        {
            pContext->ReportError("Expected non-zero integer.", stream);
        }
    }
    return fRet;
}

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
    bool hadAlpha = !!isalpha(ch);
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

// TODO: Replace our other alphanum things...

vector<string> SCIKeywords =
{
    "if",
    "asm",
    "break",
    "breakif",
    "continue",
    "contif",
    "repeat",
    "switch",
    "switchto",
    "properties",
    "procedure",
    "method",
    "for",
    "return",
    "cond",
    "while",
    "else",
    "rest",
    "super",
    "mod",
    "or",
    "and",
    "not",
    "of",
    "scriptNumber",
    "public",
    "define",
    "&tmp",
    "&rest",
    "script#"
    // neg, send, case, do, default, export are also keywords, but for the Studio language.
};

template<typename _It, typename _TContext>
bool AlphanumPNoKeywordOrTerm(const ParserSCI *pParser, _TContext *pContext, _It &stream)
{
    bool fRet = SelectorP(pParser, pContext, stream);
    if (fRet)
    {
        char chTerm = *stream;
        fRet = (chTerm != ':') && (chTerm != '?');
        if (fRet)
        {
            std::string &str = pContext->ScratchString();
            fRet = std::find(SCIKeywords.begin(), SCIKeywords.end(), str) == SCIKeywords.end();
            if (fRet && pContext->extraKeywords)
            {
                fRet = pContext->extraKeywords->find(str) == pContext->extraKeywords->end();
            }
        }
    }
    return fRet;
}

template<typename _It, typename _TContext>
bool AlphanumPSendTokenOrTerm(const ParserSCI *pParser, _TContext *pContext, _It &stream)
{
    bool fRet = SelectorP(pParser, pContext, stream);
    if (fRet)
    {
        char chTerm = *stream;
        fRet = (chTerm != ':') && (chTerm != '?');
        if (fRet)
        {
            std::string &str = pContext->ScratchString();
            if (str != "send" && str != "super")
            {
                fRet = std::find(SCIKeywords.begin(), SCIKeywords.end(), str) == SCIKeywords.end();
                if (fRet && pContext->extraKeywords)
                {
                    fRet = pContext->extraKeywords->find(str) == pContext->extraKeywords->end();
                }
            }
        }
    }
    return fRet;
}

// foo: or foo?, no whitespace allowed.
template<char terminator, typename _It>
bool SelectorP_Term(const ParserSCI *pParser, SyntaxContext *pContext, _It &stream)
{
    bool fRet = SelectorP(pParser, pContext, stream);
    fRet = fRet && (*(stream) == terminator);
    if (fRet)
    {
        ++stream; // Skip over our terminator
    }
    return fRet;
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

// Different from Studio syntax, since the operator must be followed by whitespace.
// That is, ++i is not allowed. It must be (++ i)
template<typename _It, typename _TContext, typename _CommentPolicy>
bool SCIOperatorP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    bool fRet = false;
    const char *psz = pParser->_psz;
    while (*psz && (*stream == *psz))
    {
        ++stream;
        ++psz;
    }
    if (*psz == 0)
    {
        pContext->ScratchString() = pParser->_psz;
        return !!isspace(*stream);
    }
    else
    {
        return false;
    }
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

template<typename _TBreakContinue>
void FinishBreakContinueIfA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // We should have an If statement. Add the current thing to its conditional.
        pContext->GetSyntaxNode<IfStatement>()->SetCondition(make_unique<ConditionalExpression>(move(pContext->StatementPtrReturn)));
        // And its contents should be a CodeBlock with a single Break/Continue statement.
        auto codeBlock = make_unique<CodeBlock>();
        auto breakOrContinue = make_unique<_TBreakContinue>();
        breakOrContinue->SetPosition(stream.GetPosition());
        codeBlock->AddStatement(move(breakOrContinue));
        pContext->GetSyntaxNode<IfStatement>()->SetStatement1(move(codeBlock));
    }
}

template<typename _TBreakContinue>
_TBreakContinue *GetBreakContinueInIf(SyntaxContext *pContext)
{
    // Get a break inside an if then clause
    auto codeBlock = SafeSyntaxNode<CodeBlock>(pContext->GetSyntaxNode<IfStatement>()->GetStatement1());
    return SafeSyntaxNode<_TBreakContinue>(codeBlock->GetStatements()[0].get());
}

template<typename _TBreakContinue>
_TBreakContinue *GetBreakContinue(SyntaxContext *pContext)
{
    return pContext->GetSyntaxNode<_TBreakContinue>();
}

template<typename _TBreakContinue, _TBreakContinue *(*_TGetFunc)(SyntaxContext *)>
void SetLevelsA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        _TGetFunc(pContext)->Levels = pContext->Integer;
    }
}

void FinishClassProcedureA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // Add it to the script's procedures, not the class' methods.
        auto proc = pContext->GetFunctionAsProcedure();
        proc->SetOwnerClass(pContext->ClassPtr.get());
        pContext->Script().AddProcedure(move(proc));
    }
    else
    {
        pContext->ReportError("Expected method declaration.", stream);
    }
}

template<typename _T>
void SetStatementAsConditionA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // Take the current statement, and put into the next statements conditional expression
        pContext->GetSyntaxNode<_T>()->SetCondition(make_unique<ConditionalExpression>(move(pContext->StatementPtrReturn)));
    }
}

template<typename _T>
void AddCodeBlockA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // Time to add a code block.  First, get it.
        pContext->GetSyntaxNode<_T>()->SetCodeBlock(pContext->StealStatementReturn<CodeBlock>());
    }
}
void AddLooperCodeBlockA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // Time to add a code block.  First, get it.
        pContext->GetSyntaxNode<ForLoop>()->SetLooper(pContext->StealStatementReturn<CodeBlock>());
    }
}

template<typename _T>
void SetCaseA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // This is the variable description we need to add to the assignment thing:
        pContext->GetSyntaxNode<_T>()->AddCase(pContext->StealStatementReturn<CaseStatement>());
    }
}

ParserSCI SCISyntaxParser::char_p(const char *psz) { return ParserSCI(CharP, psz); }
ParserSCI SCISyntaxParser::operator_p(const char *psz) { return ParserSCI(SCIOperatorP, psz); }
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
    alphanumAsmLabel_p(AlphanumP),
    //selector_p(SelectorP),
    selector_send_p(SelectorP_Term<':'>),
    propget_p(SelectorP_Term<'?'>),
    filename_p(FilenameP),
    asmInstruction_p(AsmInstructionP),
    alphanumNK_p(AlphanumPNoKeywordOrTerm),
    alphanumSendToken_p(AlphanumPSendTokenOrTerm),
    alwaysmatch_p(AlwaysMatchP),
    bracestring_p(BraceStringP),
    squotedstring_p(SQuotedStringP),
    quotedstring_p(QuotedStringP),
    integer_p(IntegerExpandedP),
    integerNonZero_p(IntegerNonZeroP),
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
    immediateValue = integer_p[PropValueIntA] | alphanumNK_p[PropValueStringA<ValueType::Token>] | bracestring_p[PropValueStringA<ValueType::Token>];
    string_immediateValue = integer_p[PropValueIntA] | alphanumNK_p[PropValueStringA<ValueType::Token>] | quotedstring_p[{ PropValueStringA<ValueType::String>, ParseAutoCompleteContext::Block}];
    string_immediateValue2 = integer_p[PropValueIntA] | alphanumNK_p[PropValueStringA<ValueType::Token>] | quotedstring_p[{PropValueStringA<ValueType::String>, ParseAutoCompleteContext::Block}] | bracestring_p[PropValueStringA<ValueType::Token>];

    // Top level constructs
    include = keyword_p("include") >> (quotedstring_p | filename_p)[{AddIncludeA, ParseAutoCompleteContext::Block}];
    use = keyword_p("use") >> (quotedstring_p | filename_p)[{AddUseA, ParseAutoCompleteContext::ScriptName}];
    scriptNum = keyword_p("script#") >> immediateValue[{ScriptNumberA, ParseAutoCompleteContext::DefineValue }];

    define = keyword_p("define")[CreateDefineA] >> alphanumNK_p[DefineLabelA] >> integer_p[DefineValueA];

    general_token = (alphanumNK_p | bracestring_p)[{nullptr, ParseAutoCompleteContext::None, "TOKEN"}];

    selector_literal = pound >> alphanumNK_p[{nullptr, ParseAutoCompleteContext::Selector}];

    pointer = atsign;

    export_entry = general_token[{AddExportA, ParseAutoCompleteContext::Export}] >> integer_p;
    exports = keyword_p("public") >> *export_entry;

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
        keyword_p("break")[SetStatementA<BreakStatement>] >>
        -integerNonZero_p[SetLevelsA<BreakStatement, GetBreakContinue<BreakStatement>>];

    breakif_statement =
        keyword_p("breakif")[SetStatementA<IfStatement>]
        >> statement[FinishBreakContinueIfA<BreakStatement>] >>
        -integerNonZero_p[SetLevelsA<BreakStatement, GetBreakContinueInIf<BreakStatement>>];

    continue_statement =
        keyword_p("continue")[SetStatementA<ContinueStatement>] >>
        -integerNonZero_p[SetLevelsA<ContinueStatement, GetBreakContinue<ContinueStatement>>];

    contif_statement =
        keyword_p("contif")[SetStatementA<IfStatement>]
        >> statement[FinishBreakContinueIfA<ContinueStatement>] >>
        -integerNonZero_p[SetLevelsA<ContinueStatement, GetBreakContinueInIf<ContinueStatement>>];

    bare_code_block =
        alwaysmatch_p[StartStatementA]
        >> (alwaysmatch_p[SetStatementA<CodeBlock>]
        >> *statement[AddStatementA<CodeBlock>])[FinishStatementA];

    wrapped_code_block =
        oppar[StartStatementA]
        >> (alwaysmatch_p[SetStatementA<CodeBlock>]
        >> *statement[AddStatementA<CodeBlock>] >> clpar)[FinishStatementA];

    if_statement =
        keyword_p("if")[SetStatementA<IfStatement>]
        >> statement[SetStatementAsConditionA<IfStatement>] // Condition
        >> bare_code_block[StatementBindTo1stA<IfStatement, errThen>]          // Code
        >> -(keyword_p("else")      // Optional else, followed by more code
        >> bare_code_block[StatementBindTo2ndA<IfStatement, errElse>]);

    while_loop =
        keyword_p("while")[SetStatementA<WhileLoop>]
        >> statement[SetStatementAsConditionA<WhileLoop>]
        >> *statement[AddStatementA<WhileLoop>];

    for_loop =
        keyword_p("for")[SetStatementA<ForLoop>]
        >> wrapped_code_block[AddCodeBlockA<ForLoop>]
        >> statement[SetStatementAsConditionA<ForLoop>]
        >> wrapped_code_block[AddLooperCodeBlockA]
        >> *statement[AddStatementA<ForLoop>];

    case_statement =
        alwaysmatch_p[StartStatementA]
        >> (oppar[SetStatementA<CaseStatement>]
        >> 
            (keyword_p("else")[SetDefaultCaseA] |
            statement[StatementBindTo1stA<CaseStatement, errCaseArg>])[NoCaseE]    // REVIEW: does this need to be constant?
        >> *statement[AddStatementA<CaseStatement>]
        >> clpar[GeneralE]
        )[FinishStatementA];

    switch_statement =
        keyword_p("switch")[SetStatementA<SwitchStatement>]
        >> statement[StatementBindTo1stA<SwitchStatement, errSwitchArg>]
        >> *case_statement[SetCaseA<SwitchStatement>];

    // Nearly identical parsing to a switch statement. We re-work it to ifs in post-processing.
    cond_statement =
        keyword_p("cond")[SetStatementA<CondStatement>]
        >> *case_statement[SetCaseA<CondStatement>];

    switchto_case_statement =
        alwaysmatch_p[StartStatementA]
        >> (oppar[SetStatementA<CaseStatement>]
        >> *statement[AddStatementA<CaseStatement>]
        >> clpar[GeneralE]
        )[FinishStatementA];

    switchto_statement =
        keyword_p("switchto")[SetStatementA<SwitchStatement>]
        >> statement[StatementBindTo1stA<SwitchStatement, errSwitchArg>]
        >> *switchto_case_statement[SetCaseA<SwitchStatement>];

    // (SomeProc param1 param2 param3)
    procedure_call = alphanumNK_p[SetStatementNameA<ProcedureCall>] >> *statement[AddStatementA<ProcedureCall>];

    // posn: x y z
    send_param_call = selector_send_p[SetStatementNameA<SendParam>] >> alwaysmatch_p[SendParamIsMethod] >> *statement[AddStatementA<SendParam>];

    // expression selectorA: one two three, selectorB: one two
    // expression selectorA?
    send_call = (alwaysmatch_p[SetStatementA<SendCall>] // Simple form, e.g. gEgo
        >> ((alphanumSendToken_p[SetNameA<SendCall>]) | statement[StatementBindTo1stA<SendCall, errSendObject>])) // Expression, e.g. [clients 4], or (GetTheGuy)
        >>
        (propget_p[AddSimpleSendParamA] |             // Single prop get
        (syntaxnode_d[send_param_call[AddSendParamA]] % comma)      // Or a series regular ones separated by comma
        )
        ;

    // Operators
    binary_operator = operator_p("u>=") | operator_p(">=") | operator_p("u>") | operator_p(">>") |
        operator_p(">") | operator_p("u<=") | operator_p("<=") |
        operator_p("u<") | operator_p("!=") | operator_p("<<") |
        operator_p("<") | operator_p("==") | operator_p("+") |
        operator_p("-") | operator_p("*") | operator_p("/") |
        operator_p("mod") | operator_p("&") | operator_p("|") |
        operator_p("^");

    unary_operator = operator_p("~") | operator_p("not") | operator_p("-") |
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
        >> -alphanumNK_p[SetStatementNameA<RestStatement>];

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

    asm_arg =
        alwaysmatch_p[StartStatementA]
        >> value[FinishStatementA]
        >> !(colon | question);    // If colon, then it's the next line's label, if question it's actaully an opcode (e.g. le? ne?)

    asm_label =
        alphanumAsmLabel_p
        >> colon[SetLabelA];

    asm_statement =
        alwaysmatch_p[StartStatementA]
        >> (alwaysmatch_p[SetStatementA<Asm>]
        >> -asm_label                                               // Optional label
        >> asmInstruction_p[SetNameA<Asm>]                                // Instruction name
        >> -(asm_arg[AddStatementA<Asm>] % comma[GeneralE]))[FinishStatementA];         // command separated values

    asm_block = oppar
        >> keyword_p("asm")[SetStatementA<AsmBlock>]
        >> alwaysmatch_p[SetOpcodesExtraKeywordsA]
        >> *asm_statement[AddStatementA<AsmBlock>]
        >> alwaysmatch_p[RemoveExtraKeywordsA]
        >> clpar[GeneralE];

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
        if_statement |
        while_loop |
        for_loop |
        cond_statement |
        switchto_statement |
        switch_statement |
        breakif_statement |
        break_statement |
        continue_statement |
        contif_statement |
        code_block |
        send_call |             // Send has to come before procedure. Because procedure will match (foo sel:)
        procedure_call |
        asm_block
        ) >>
        clpar)
        | rest_statement
        | value
        )[{FinishStatementA, ParseAutoCompleteContext::Value}];


    // An array initializer
    array_init = opbracket >> *(string_immediateValue2[ScriptVarInitAutoExpandA]) >> clbracket;  // [0 0 $4c VIEW_EGO]

    // Variable declaration, with optional array size (array size must be numeric!)
    // A   or  [A 6]   or [A MY_ARRAY_SIZE]
    var_decl =
        (opbracket >> alphanumNK_p[CreateVarDeclA] >> (integer_p[VarDeclSizeA] | alphanumNK_p[VarDeclSizeConstantA])[VarDeclSizeErrorA] >> clbracket) |
        alphanumNK_p[CreateVarDeclA];

    script_var = keyword_p("local")
        >> *((var_decl[CreateScriptVarA] >> -(equalSign[GeneralE] >> (string_immediateValue2[ScriptVarInitA] | array_init)))[FinishScriptVarA]);

    // StartFunctionTempVarA is needed to reset "was initializer value set". There are no initializer values for function variables in this syntax.
    function_var_decl = keyword_p("&tmp")[StartFunctionTempVarA] >> +(var_decl[FinishFunctionTempVarA]);

    property_decl = alphanumNK_p[{CreateClassPropertyA, ParseAutoCompleteContext::ClassSelector}]
        >> statement[FinishClassPropertyStatementA];

    // (procname param param &tmp tmp tmp tmp)
    procedure_base = oppar
        >> alphanumNK_p[FunctionNameA]
        >> *alphanumNK_p[FunctionParameterA]
        >> *function_var_decl
        >> clpar[GeneralE]
        >> *statement[FunctionStatementA];

    procedure_decl = keyword_p("procedure")[CreateProcedureA] >> procedure_base[{FunctionCloseA, ParseAutoCompleteContext::Block}];

    // Very similar to procedure_base, but with some autocomplete differences:
    method_base = oppar
        >> alphanumNK_p[{FunctionNameA, ParseAutoCompleteContext::Selector}]
        >> *alphanumNK_p[{FunctionParameterA, ParseAutoCompleteContext::Block}]
        >> *function_var_decl
        >> clpar[GeneralE]
        >> *statement[FunctionStatementA];

    method_decl = keyword_p("method")[{CreateMethodA, ParseAutoCompleteContext::ClassLevelKeyword}] >> method_base[FunctionCloseA];

    // The properties thing in a class or instance
    properties_decl = oppar >> keyword_p("properties")[{nullptr, ParseAutoCompleteContext::ClassLevelKeyword}] >> *property_decl >> clpar;

    classbase_decl =
        alphanumNK_p[ClassNameA]
        >> -(keyword_p("of")[GeneralE] >> alphanumNK_p[{ClassSuperA, ParseAutoCompleteContext::SuperClass}])
        >> -properties_decl
        >> *(oppar >> 
            (method_decl[FinishClassMethodA] |
            procedure_decl[FinishClassProcedureA]) >> clpar);

    instance_decl = keyword_p("instance")[CreateClassA<true>] >> classbase_decl[ClassCloseA];

    class_decl = keyword_p("class")[CreateClassA<false>] >> classbase_decl[ClassCloseA];

    // TODO:
    entire_script = *(oppar[GeneralE]
        >> (include
        | use
        | define[FinishDefineA]
        | instance_decl[FinishClassA]
        | class_decl[FinishClassA]
        | procedure_decl[FinishProcedureA]
        | exports
        | scriptNum
        | script_var)[{IdentifierE, ParseAutoCompleteContext::TopLevelKeyword}]
        >> clpar[GeneralE]);
}

// 
// Fix up scripts so that they conform to standards. Differences in the SCI syntax make
// it difficult to get the script OM in its final state just in the parsing phase.
//
// - Propagate "public" from exports section to the individual procedures/instances
// - Identify switchtos and auto-number their cases
// - Transform cond into if-elseif-else
//
void PostProcessScript(ICompileLog *pLog, Script &script)
{
    // Push public down to instances and procedures
    EnumScriptElements<ProcedureDefinition>(script,
        [&script](ProcedureDefinition &proc)
    {
        proc.SetPublic(script.IsExport(proc.GetName()));
    }
        );

    EnumScriptElements<ClassDefinition>(script,
        [&script](ClassDefinition &instance)
    {
        if (instance.IsInstance())
        {
            instance.SetPublic(script.IsExport(instance.GetName()));
        }
    }
        );

    // Re-work conds into if-elses.
    EnumScriptElements<CondStatement>(script,
        [pLog, &script](CondStatement &cond)
    {
        // TODO: Enforce that else needs to be last.
        // TODO: What if only one else?
        assert(!cond.GetStatement1());
        if (!cond._clausesTemp.empty())
        {
            std::unique_ptr<CodeBlock> elseCodeBlock;
            auto it = cond._clausesTemp.rbegin();
            if ((*it)->IsDefault())
            {
                elseCodeBlock = make_unique<CodeBlock>(move((*it)->GetStatements()));
                ++it;
            }
            std::unique_ptr<IfStatement> currentIf;
            while (it != cond._clausesTemp.rend())
            {
                auto &clause = *it;
                if (clause->IsDefault())
                {
                    pLog->ReportResult(CompileResult("The else clause must be the last clause in a cond.", script.GetScriptId(), clause->GetPosition().Line()));
                    break;
                }
                std::unique_ptr<IfStatement> newIf = make_unique<IfStatement>();
                if (currentIf)
                {
                    // Put this in our new if's else block.
                    newIf->SetStatement2(make_unique<CodeBlock>(move(currentIf)));
                }
                currentIf = move(newIf);
                // Grab the statement1 of the CaseStatement and put it in the if condition.
                currentIf->SetCondition(make_unique<ConditionalExpression>(move(clause->GetStatement1Internal())));
                // Then transfer all the statements into a CodeBlock for the if.
                currentIf->SetStatement1(make_unique<CodeBlock>(move(clause->GetStatements())));
                if (elseCodeBlock)
                {
                    // This goes into the final if.
                    currentIf->SetStatement2(move(elseCodeBlock));
                }
                ++it;
            }

            // Case where there is just a single else, just copy statements into a codeblock.
            if (elseCodeBlock)
            {
                assert(!currentIf);
                cond.SetStatement1(move(elseCodeBlock));
            }
            else
            {
                assert(!elseCodeBlock);
                cond.SetStatement1(move(currentIf));
            }
        }

        cond._clausesTemp.clear();
    }
    );

    // Identify switchto statements and auto-enumerate the cases. If no case statements have values and none are default,
    // it's a switchto
    EnumScriptElements<SwitchStatement>(script,
        [](SwitchStatement &switchStatement)
    {
        if (all_of(switchStatement._cases.begin(), switchStatement._cases.end(),
            [](std::unique_ptr<CaseStatement> &caseStatement) { return !caseStatement->IsDefault() && (caseStatement->GetCaseValue() == nullptr); }))
        {
            uint16_t value = 0;
            for (auto &caseStatement : switchStatement._cases)
            {
                caseStatement->SetCaseValue(std::make_unique<PropertyValue>(value));
                value++;
            }
        }
    }
        );
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
        PostProcessScript(pError, script);
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
        PostProcessScript(nullptr, script);
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
    else
    {
        PostProcessScript(pError, script);
    }
    return fRet;
}