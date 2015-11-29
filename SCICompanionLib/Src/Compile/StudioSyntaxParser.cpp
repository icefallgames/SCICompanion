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


//
// StudioSyntaxParser implementation
//
#include "stdafx.h"
#include "ScriptOMAll.h"
#include "StudioSyntaxParser.h"
#include "PMachine.h"
#include "Operators.h"
#include "OperatorTables.h"
#include "ParserActions.h"

using namespace sci;
using namespace std;

const char *g_keywords[4] =
{
    "if",
    "while",
    "do",
    "switch",
    // The below causes not and neg not to be resolved (scratch string is empty for unary ops) INVESTIGATE
    // Because I think we may have broken asm
    /*
    _T("asm"),
    _T("break"),
    _T("send"),
    _T("case"),
    _T("switch"),
    _T("properties"),
    _T("method"),
    _T("for"),
    _T("if"),
    _T("return"),
    _T("default"),
    _T("do"),
    _T("while"),
    _T("else"),
    _T("rest"),
    _T("super"),
    _T("or"),
    _T("and"),
    _T("not"),
    _T("neg"),
    _T("of"),
    _T("scriptNumber"), */
};

Parser char_p(const char *psz) { return Parser(CharP, psz); }
Parser keyword_p(const char *psz) { return Parser(KeywordP, psz); }
Parser operator_p(const char *psz) { return Parser(OperatorP, psz); }

Parser alphanum_p(AlphanumP);
Parser asmInstruction_p(AsmInstructionP);
Parser alphanum_p2(AlphanumPNoKeyword);
Parser alwaysmatch_p(AlwaysMatchP);
Parser alphanumopen_p(AlphanumOpenP);
Parser bracestring_p(BraceStringP);
Parser squotedstring_p(SQuotedStringP);
Parser quotedstring_p(QuotedStringP);
Parser integer_p(IntegerP);
Parser oppar(char_p("("));
Parser clpar(char_p(")"));
Parser opcurly(char_p("{"));
Parser clcurly(char_p("}"));
Parser semi(char_p(";"));
Parser opbracket(CharP, "[");
Parser clbracket(CharP, "]");
Parser pound(CharP, "#");
Parser ampersand(CharP, "&");
Parser atsign(CharP, "@");
Parser comma(CharP, ",");
Parser colon(CharP, ":");
Parser equalSign(CharP, "=");
Parser question(CharP, "?");

// SCIStudio syntax has a weird way to form conditional expressions that we don't want to pollute our object model.
class ConditionalExpressionSCIStudio : public SyntaxNode, public StatementsNode
{
    DECLARE_NODE_TYPE(NodeTypeConditionalExpressionSCIStudio)
public:
    ConditionalExpressionSCIStudio() {}
    ConditionalExpressionSCIStudio(const ConditionalExpressionSCIStudio &src) = delete;
    ConditionalExpressionSCIStudio& operator=(const ConditionalExpressionSCIStudio& src) = delete;

    // IOutputByteCode
    CodeResult OutputByteCode(CompileContext &context) const { throw std::exception("Not Implemented"); }
    void PreScan(CompileContext &context)  { throw std::exception("Not Implemented"); }
    void Traverse(IExploreNode &en)  { throw std::exception("Not Implemented"); }

    void AddAndOr(bool fAnd) { _andOrs.push_back(fAnd); }

    SyntaxNodeVector &GetItems() { return _segments; }
    std::vector<bool> &GetAndOrs() { return _andOrs; }

    void Accept(ISyntaxNodeVisitor &visitor) const override  { throw std::exception("Not Implemented"); }

private:
    std::vector<bool> _andOrs;   // true for and, false for or
};

//
// Our syntax context implementations
//
const sci::SyntaxNode *SyntaxContext::GetSyntaxNode(sci::NodeType type) const
{
    sci::SyntaxNode *pNode = nullptr;
    auto it = _statements._Get_container().rbegin();
    while (it != _statements._Get_container().rend())
    {
        if (*it)
        {
            if (type == (*it)->GetNodeType())
            {
                pNode = (*it).get();
                break;
            }
        }
        ++it;
    }
    return pNode;
}

sci::NodeType SyntaxContext::GetTopKnownNode() const
{
    sci::NodeType type = sci::NodeTypeUnknown;
    auto it = _statements._Get_container().rbegin();
    while (it != _statements._Get_container().rend())
    {
        if (*it)
        {
            type =(*it)->GetNodeType();
            break;
        }
        ++it;
    }
    return type;
}

//
// send call
//

void AddSendRestA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->GetPrevSyntaxNode<SendCall>()->SetSendRestHack(std::move(pContext->StealSyntaxNode<RestStatement>()));
    }
}
template<typename _T>
void SetLValueA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->GetPrevSyntaxNode<SendCall>()->SetLValue(move(pContext->StealSyntaxNode<LValue>()));
    }
}
void AddSingleSendParamA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // We have a SendCall to which we need to add a SendParam that has no parameters (e.g. just the selector)
        // e.g. the 'x' in  "(send gEgo:y(5)x)"
        unique_ptr<SendParam> pParam = std::make_unique<SendParam>();
        pParam->SetPosition(stream.GetPosition());
        pParam->SetName(pContext->ScratchString());
        // Now add this to the SendCall
        pContext->GetSyntaxNode<SendCall>()->AddSendParam(std::move(pParam));
    }
}

Parser generateSyntaxNodeD()
{
    Parser syntaxnode(SyntaxNodeD);
    return syntaxnode[EndSyntaxNodeA];
}
// Use this:
Parser syntaxnode_d = generateSyntaxNodeD();

bool RootNodeD(const Parser *pParser, SyntaxContext *pContext, streamIt &stream)
{
    // Make room on the stack for a statement
    //pContext->PushSyntaxNode();
    // Then call our sub parser
    bool fRet = pParser->_pa->Match(pContext, stream).Result();
    if (fRet)
    {
        // This becomes a root node.
    }
    return fRet;
}

Parser generateRootNodeD()
{
    Parser rootnode(RootNodeD);
    return rootnode;
    //return syntaxnode[EndSyntaxNodeA];
}
// Use this:
Parser rootnode_d = generateRootNodeD();

//
// Actions
//

void SetVersionA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        if ((pContext->Integer != 1) && (pContext->Integer != 2))
        {
            match.ChangeResult(false);
            pContext->ReportError("The only acceptable version values are 1 and 2.", stream);
        }
        pContext->Script().SyntaxVersion = pContext->Integer;
    }
}

// Statements
void StatementA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (!match.Result())
    {
        pContext->ReportError("Expected statement.", stream);
    }
    else if (match.Result())
    {
        // TODO: Add the statement to wherever.
    }
}

template<typename _TOpType>
_TOpType StudioNameToOperator(const std::string &name);

template<>
BinaryOperator StudioNameToOperator<BinaryOperator>(const std::string &name)
{
    return NameToOperator(name, studioNameToBinaryOp);
}

template<>
AssignmentOperator StudioNameToOperator<AssignmentOperator>(const std::string &name)
{
    return NameToOperator(name, studioNameToAssignmentOp);
}

template<>
UnaryOperator StudioNameToOperator<UnaryOperator>(const std::string &name)
{
    return NameToOperator(name, studioNameToUnaryOp);
}

template<typename _T, typename _TOperator>
void SetOperatorA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->CreateSyntaxNode<_T>(stream);
        _TOperator op = StudioNameToOperator<_TOperator>(pContext->ScratchString());
        assert(op != _TOperator::None);
        pContext->GetSyntaxNode<_T>()->Operator = op;
    }
}

//
// Synonyms
// 
void CreateSynonymA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->CreateSynonym();
        pContext->SynonymPtr->MainWord = pContext->ScratchString();
    }
    else
    {
        pContext->ReportError("Expected said string.", stream);
    }
}
void FinishSynonymA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->SynonymPtr->Replacement = pContext->ScratchString();
        pContext->Script().AddSynonym(std::move(pContext->SynonymPtr));
    }
    else
    {
        pContext->ReportError("Expected said string.", stream);
    }
}

void EvaluateIfDefA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->EvaluateIfDefScratch(stream, pContext->ScratchString());
    }
}
void EvaluateEndIfA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->EndIf(stream);
    }
}

void FailedVarDecl(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (!match.Result())
    {
        // We went down a "var" path, but had an error something.
        // We should jsut assert that we have an error
        assert(!pContext->GetErrorText().empty());
    }
}

// Script string
void ScriptStringInitA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // Add a value to the script variable.
        pContext->VariableDeclPtr->AddSimpleInitializer(pContext->PropertyValue);
    }
    else
    {
        pContext->ReportError("Script strings can only be initialized with strings or numbers.", stream);
    }
}
void FinishScriptStringA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->Script().AddStringDeclaration(move(pContext->VariableDeclPtr));
    }
}

void NoCaseE(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (!match.Result())
    {
        pContext->ReportError("Expected 'case' or 'default' keyword.", stream);
    }
}

template<int _Version>
void EnableScriptVersionA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // If we matched, only match if version number is correct
        match.ChangeResult(_Version == pContext->Script().SyntaxVersion);
    }
}

void ClassPublicA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->ClassPtr->SetPublic(true);
    }
}

//
// Conditional expression
//
template<bool fAnd>
void ConditionOperatorA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->GetSyntaxNode<ConditionalExpressionSCIStudio>()->AddAndOr(fAnd);
    }
}

template<typename _T>
void FinishConditionA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        std::unique_ptr<ConditionalExpressionSCIStudio> temp = move(pContext->StealSyntaxNode<ConditionalExpressionSCIStudio>());
        // Turn this into a tree and make a real ConditionalExpression
        // A bit of a hack: the parser will end up adding an andOr even if the follow term doesn't succeed.
        // In that case, we need to trim.
        vector<bool> andOrs = temp->GetAndOrs();
        if (andOrs.size() == temp->GetItems().size())
        {
            andOrs.pop_back();
        }
        ConvertAndOrSequenceIntoTree(temp->GetItems(), andOrs);
        unique_ptr<ConditionalExpression> condExp = make_unique<ConditionalExpression>();
        condExp->AddStatement(move(temp->GetItems()[0]));
        pContext->GetPrevSyntaxNode<_T>()->SetCondition(move(condExp));
    }
}


template<typename _T>
void FinishCondition2A(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        std::unique_ptr<ConditionalExpressionSCIStudio> temp = move(pContext->StealSyntaxNode<ConditionalExpressionSCIStudio>());
        // Turn this into a tree and make a real ConditionalExpression
        // A bit of a hack: the parser will end up adding an andOr even if the follow term doesn't succeed.
        // In that case, we need to trim.
        vector<bool> andOrs = temp->GetAndOrs();
        if (andOrs.size() == temp->GetItems().size())
        {
            andOrs.pop_back();
        }
        ConvertAndOrSequenceIntoTree(temp->GetItems(), andOrs);
        pContext->GetPrevSyntaxNode<_T>()->AddStatement(move(temp->GetItems()[0]));
    }
}


//
// CodeBlock
//
template<typename _T>
void AddCodeBlockA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // Time to add a code block.  First, get it.
        pContext->GetPrevSyntaxNode<_T>()->SetCodeBlock(move(pContext->StealSyntaxNode<CodeBlock>()));
    }
}
void AddLooperCodeBlockA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // Time to add a code block.  First, get it.
        pContext->GetPrevSyntaxNode<ForLoop>()->SetLooper(move(pContext->StealSyntaxNode<CodeBlock>()));
    }
}

void FinishCaseA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // This is the variable description we need to add to the assignment thing:
        pContext->GetPrevSyntaxNode<SwitchStatement>()->AddCase(std::move(pContext->StealSyntaxNode<CaseStatement>()));
    }
}

char const errBinaryOp[] = "Expected second argument.";
char const errCaseArg[] = "Expected case value.";
char const errSwitchArg[] = "Expected switch argument.";
char const errSendObject[] = "Expected send object.";
char const errArgument[] = "Expected argument.";
char const errInteger[] = "Expected integer literal.";
char const errThen[] = "Expected then clause.";
char const errVarName[] = "Expected variable name.";
char const errElse[] = "Expected else clause.";

//
// The constructor sets up the parse tree
//
void StudioSyntaxParser::Load()
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

    general_token = (alphanum_p2 | bracestring_p)[{nullptr, ParseAutoCompleteContext::None, "TOKEN"}];

    pointer = atsign;

    selector = pound >> general_token[{nullptr, ParseAutoCompleteContext::Selector}];

    // Value, one of the following forms:
    // -45 | gEgo[4] | "fwef" | 'wef' | {fwe} | $c000 | #foo | @bar
    // REVIEW: might we want to allow [] after {fwe} ? 
    simple_value =
        integer_p[PropValueIntA]
        | quotedstring_p[{PropValueStringA<ValueType::String>, ParseAutoCompleteContext::Block}]
        | squotedstring_p[{PropValueStringA<ValueType::Said>, ParseAutoCompleteContext::Block}];

    value =
        alwaysmatch_p[SetStatementA<ComplexPropertyValue>]
        >> (integer_p[ComplexValueIntA]
        | quotedstring_p[{ComplexValueStringA<ValueType::String>, ParseAutoCompleteContext::Block}]
        | squotedstring_p[{ComplexValueStringA<ValueType::Said>, ParseAutoCompleteContext::Block}]
            | (-pointer[ComplexValuePointerA] >> general_token[ComplexValueStringA<ValueType::Token>] >> -(opbracket >> statement[ComplexValueIndexerA] >> clbracket))
            | selector[ComplexValueStringA<ValueType::Selector>]);

    // For now only binary ops are allowed
    property_value_expanded =
        alwaysmatch_p[EnableScriptVersionA<2>] >>
        statement;

    property_value =
        simple_value
        | alphanum_p2[{PropValueStringA<ValueType::Token>, ParseAutoCompleteContext::DefineValue}];

    property_decl = general_token[{CreateClassPropertyA, ParseAutoCompleteContext::ClassSelector}] >>
         (property_value[FinishClassPropertyA]
         | property_value_expanded[FinishClassPropertyStatementA])[ExpectedProperyValueE]
        ;

    // The properties thing in a class or instance
    properties_decl = oppar >> keyword_p("properties")[{nullptr, ParseAutoCompleteContext::ClassLevelKeyword}] >> *property_decl >> clpar;

    // An array initializer
    array_init = oppar >> *(string_immediateValue2[ScriptVarInitA]) >> clpar;  // (0 0 $4c VIEW_EGO)

    // Array initializer for script strings
    string_array_init = oppar >> *(string_immediateValue[ScriptStringInitA]) >> clpar;  // (0 0 "Hello" $20 FALSE)

    // Operators
    binary_operator = operator_p(">=u") | operator_p(">=") | operator_p(">u") | operator_p(">>") |
            operator_p(">") | operator_p("<=u") | operator_p("<=") |
            operator_p("<u") | operator_p("<>") | operator_p("<<") |
            operator_p("<") | operator_p("==") | operator_p("+") |
            operator_p("-") | operator_p("*") | operator_p("/") |
            operator_p("%") | operator_p("&") | operator_p("|") |
            operator_p("^");

    unary_operator = keyword_p("bnot") | keyword_p("not") | keyword_p("neg") | 
            operator_p("++") | operator_p("--");

    assignment_operator = operator_p("+=") | operator_p("-=") | operator_p("*=") |
            operator_p("/=") | operator_p("%=") | operator_p("&=") |
            operator_p("|=") | operator_p("^=") | operator_p(">>=") |
            operator_p("<<=") | operator_p("=");

    // Variable declaration, with optional array size (array size must be numeric!)
    var_decl = alphanum_p[CreateVarDeclA] >> -(opbracket >> (integer_p[VarDeclSizeA] | alphanum_p2[VarDeclSizeConstantA])[VarDeclSizeErrorA] >> clbracket);

    // General code pieces
    // (we need to put the token on the statement stack, since it's possible there could be another one in "statement"
    variable = general_token[{SetStatementNameA<LValue>, ParseAutoCompleteContext::LValue}] >> -(opbracket >> statement[LValueIndexerA] >> clbracket);

    // Only used by "conditional".  This mimics the syntax used by SCI Studio, even though it's questionable.
    // Basically, (1 and (2 or 3)) turns into (1 and 2 or 3).  Within a conditional, parentheses are ignored essentially.
    base_conditional_v1 =
        ((oppar >> base_conditional_v1 >> clpar) | statement[AddStatementA<ConditionalExpressionSCIStudio>])
         % (keyword_p("and")[ConditionOperatorA<true>] | keyword_p("or")[ConditionOperatorA<false>]);

    // Requires a stack frame to be pushed for it!
    conditional_v1 =
        alwaysmatch_p[SetStatementA<ConditionalExpressionSCIStudio>]
        >> base_conditional_v1;

    base_conditional_v2 =
        ((oppar >> syntaxnode_d[conditional[FinishCondition2A<ConditionalExpressionSCIStudio>]] >> clpar) | statement[AddStatementA<ConditionalExpressionSCIStudio>])
        % (keyword_p("and")[ConditionOperatorA<true>] | keyword_p("or")[ConditionOperatorA<false>]);

    conditional_v2 =
        alwaysmatch_p[SetStatementA<ConditionalExpressionSCIStudio>]
        >> base_conditional_v2;

    conditional = (alwaysmatch_p[EnableScriptVersionA<1>] >> conditional_v1) | (alwaysmatch_p[EnableScriptVersionA<2>] >> conditional_v2);
        
    do_loop = oppar
        >> keyword_p("do")[SetStatementA<DoLoop>]
        //>> syntaxnode_d[code_block[AddCodeBlockA<DoLoop>]]
        >> *statement[AddStatementA<DoLoop>]
        >> clpar
        >> keyword_p("while")[GeneralE<streamIt>]
        >> oppar
        >> syntaxnode_d[conditional[FinishConditionA<DoLoop>]]
        >> clpar;

    while_loop = oppar
        >> keyword_p("while")[SetStatementA<WhileLoop>]
        >> oppar
        >> syntaxnode_d[conditional[FinishConditionA<WhileLoop>]]
        >> clpar
        >> *statement[AddStatementA<WhileLoop>]
        >> clpar;

    code_block_no_paren =
        alwaysmatch_p[StartStatementA]
        >> (alwaysmatch_p[SetStatementA<CodeBlock>]
        >> *statement[AddStatementA<CodeBlock>])[FinishStatementA];
       
    
    if_statement = oppar
        >> keyword_p("if")[SetStatementA<IfStatement>]
        >> oppar[GeneralE]
        >> syntaxnode_d[conditional[FinishConditionA<IfStatement>]]
        >> clpar
        >> code_block_no_paren[StatementBindTo1stA<IfStatement, errArgument>]
        >> clpar
        >> -
            (
            oppar
            >> keyword_p("else")
            >> code_block_no_paren[StatementBindTo2ndA<IfStatement, errBinaryOp>]
            >> clpar
            );
    
    for_loop =
           oppar
        >> keyword_p("for")[SetStatementA<ForLoop>]
        >> syntaxnode_d[code_block[AddCodeBlockA<ForLoop>]]
        >> oppar >> syntaxnode_d[conditional[FinishConditionA<ForLoop>]] >> clpar
        >> syntaxnode_d[code_block[AddLooperCodeBlockA]]
        >> *statement[AddStatementA<ForLoop>]
        >> clpar;

    // Requires syntax node.
    case_statement = oppar[SetStatementA<CaseStatement>]
        >>      (keyword_p("default")[SetDefaultCaseA]
            |    keyword_p("case") >> statement[StatementBindTo1stA<CaseStatement, errCaseArg>])[NoCaseE]
        >> *statement[AddStatementA<CaseStatement>]
        >> clpar;

    switch_statement = oppar
        >> keyword_p("switch")[SetStatementA<SwitchStatement>]
        >> statement[StatementBindTo1stA<SwitchStatement, errSwitchArg>]
        >> *syntaxnode_d[case_statement[FinishCaseA]]
        >> clpar;

    break_statement = keyword_p("break")[SetStatementA<BreakStatement>];

    rest_statement =
            keyword_p("rest")[SetStatementA<RestStatement>]
        >>  alphanum_p[SetStatementNameA<RestStatement>];

    return_statement =
            keyword_p("return")[SetStatementA<ReturnStatement>]
        >>  -statement[StatementBindTo1stA<ReturnStatement, nullptr>];

    // Force the oppar to come right after the alphanum_token, to eliminate ambiguity
    // = gWnd clBlack (send gEgo:x)        = gWnd Print("foo")         (= button (+ 5 5))
    procedure_call = alphanumopen_p[SetStatementNameA<ProcedureCall>] >> *statement[AddStatementA<ProcedureCall>] >> clpar;

    send_param_call = general_token[{SetStatementNameA<SendParam>, ParseAutoCompleteContext::Selector, "SELECTOR_NAME" }]
        >> oppar[SendParamIsMethod] >> *statement[AddStatementA<SendParam>] >> clpar;

    // Note: SCIStudio requires the : come right after the text (if no send keyword), so we use +colon
    send_call =
        (
        (
        ((keyword_p("send")[SetStatementA<SendCall>])
        >> ((syntaxnode_d[variable[SetLValueA<SendCall>]])                                // send flakes[4]:x
        | (oppar >> statement[StatementBindTo1stA<SendCall, errSendObject>] >> clpar)) >> +colon)  // send (= gEgo foo):x
        | (general_token[{SetStatementNameA<SendCall>, ParseAutoCompleteContext::None, "SEND_TARGET" }] >> +colon)                                // theMan:
        )
        >>
        *(
        // In addition to regular "send param calls", we also need to support the case when it appears in brackets,
        // as in "(send theItem:(checkState(1)))" in Controls.sc
        syntaxnode_d[send_param_call[AddSendParamA]]
        | (oppar >> syntaxnode_d[send_param_call[AddSendParamA]] >> clpar)
        )
        // final single send param is allowed to have no parameters - or it could be a rest statement
        // e.g. "(send thing:foo(4) rest params)", instead of "(send thing:foo(4 rest params))" like it should be.
        >> -(syntaxnode_d[rest_statement[AddSendRestA]]  // This SCIStudio syntax is strange, but the template game does it
        | general_token[AddSingleSendParamA])
        )[{nullptr, ParseAutoCompleteContext::None, "SEND_CALL"}];

    // Generic set of code inside parentheses
    code_block =
            oppar[SetStatementA<CodeBlock>]
        >>  *statement[AddStatementA<CodeBlock>]
        >>  clpar[GeneralE];

    // = view 1
    assignment =
            assignment_operator[SetOperatorA<Assignment, AssignmentOperator>]
        >>  syntaxnode_d[variable[AssignmentVariableA]]
        >>  statement[StatementBindTo1stA<Assignment, errArgument>];

    // + 5 view
    binary_operation =
            binary_operator[SetOperatorA<BinaryOp, BinaryOperator>]
        >>  statement[StatementBindTo1stA<BinaryOp, errArgument>]
        >>  statement[StatementBindTo2ndA<BinaryOp, errBinaryOp>];

    // bnot done
    unary_operation =
            unary_operator[SetOperatorA<UnaryOp, UnaryOperator>]
        >>  statement[StatementBindTo1stA<UnaryOp, errArgument>];

    asm_arg =
        alwaysmatch_p[StartStatementA]
        >> value[FinishStatementA]
        >> !(colon | question);    // If colon, then it's the next line's label, if question it's actaully an opcode (e.g. le? ne?)

    asm_label =
        alphanum_p
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
    statement = alwaysmatch_p[StartStatementA] >>
        (do_loop
        | send_call
        | return_statement
        | for_loop
        | while_loop
        | if_statement
        | break_statement
        | switch_statement
        | rest_statement
        | unary_operation   // Must come before precedure_call, or else not() would be treated as a proc
        | procedure_call    // Must come before value, since foo() would also be matched by value foo
                            // ...but it must go after keywords like send( ...
        | value             // Must come before binary_operation, or else binary_operation would match "-1 ???"
                            // ...but must come after unary_operation, since some unary operations are alphanum (e.g. not)
        | assignment
        | binary_operation
        | asm_block
        //| ternary_expression
        | code_block)[{FinishStatementA, ParseAutoCompleteContext::Value}];

    function_var_decl_begin = oppar >> keyword_p("var")[{nullptr, ParseAutoCompleteContext::Value}];

    function_var_decl_inner =
        ((var_decl[StartFunctionTempVarA] >> -(equalSign >> immediateValue[ErrorA<errInteger>]))[FinishFunctionTempVarA] % comma[GeneralE])
        >> clpar[GeneralE]
        ;

    // REVIEW: this is the fix for eigen's bug (var x y), but we still generate a confusing error....
    // var identifier in functions
    function_var_decl =
        (function_var_decl_begin >> function_var_decl_inner) |
        (!function_var_decl_begin)[FailedVarDecl];

    method_base = oppar
        >> alphanum_p[{FunctionNameA, ParseAutoCompleteContext::Selector}]
        >> *alphanum_p[{FunctionParameterA, ParseAutoCompleteContext::Block}]
        >> clpar[GeneralE]
        >> function_var_decl
        >> *statement[FunctionStatementA];

    procedure_base = oppar
        >> alphanum_p[FunctionNameA]
        >> *alphanum_p[FunctionParameterA]
        >> clpar[GeneralE]
        // ::Value for autocomplete context, since it will stick around until the functino body
        >> -(keyword_p("of")[{nullptr, ParseAutoCompleteContext::Value}] >> alphanum_p[{ProcedureClassA, ParseAutoCompleteContext::SuperClass}])
        >> function_var_decl
        >> *statement[FunctionStatementA];

    method_decl = oppar >> keyword_p("method")[{CreateMethodA, ParseAutoCompleteContext::ClassLevelKeyword}] >> method_base >> clpar[FunctionCloseA];

    // Classes
    classbase_decl = -keyword_p("public")[ClassPublicA]
        >> general_token[ClassNameA]
        >> -(keyword_p("of")[GeneralE] >> alphanum_p[{ClassSuperA, ParseAutoCompleteContext::SuperClass}])
        >> -properties_decl
        >> *method_decl[FinishClassMethodA];

    instancebase_decl = -keyword_p("public")[ClassPublicA]
        >> general_token[ClassNameA]
        >> keyword_p("of")[GeneralE]
        >> alphanum_p[{ClassSuperA, ParseAutoCompleteContext::SuperClass}]
        >> -properties_decl
        >> *method_decl[FinishClassMethodA];

    // Main script pieces.
    include = keyword_p("include") >> quotedstring_p[{AddIncludeA, ParseAutoCompleteContext::Block}];

    use = keyword_p("use") >> quotedstring_p[{AddUseA, ParseAutoCompleteContext::ScriptName}];
    version = keyword_p("version") >> integer_p[SetVersionA];

    export_entry = integer_p >> general_token[{AddExportA, ParseAutoCompleteContext::Export}];
    exports = keyword_p("exports") >> *export_entry;

    define = keyword_p("define")[CreateDefineA] >> alphanum_p[DefineLabelA] >> integer_p[DefineValueA];

    scriptNum = keyword_p("script") >> immediateValue[{ScriptNumberA, ParseAutoCompleteContext::DefineValue }];

    instance_decl = keyword_p("instance")[CreateClassA<true>] >> instancebase_decl[ClassCloseA];

    class_decl = keyword_p("class")[CreateClassA<false>] >> classbase_decl[ClassCloseA];

    procedure_decl = keyword_p("procedure")[CreateProcedureA] >> -keyword_p("public")[ProcedurePublicA] >> procedure_base[{FunctionCloseA, ParseAutoCompleteContext::Block}];

    synonyms = keyword_p("synonyms") >> *(squotedstring_p[CreateSynonymA] >> equalSign[GeneralE] >> squotedstring_p[FinishSynonymA]);

    script_var = keyword_p("local")
        >> *((var_decl[CreateScriptVarA] >> -(equalSign[GeneralE] >> (string_immediateValue2[ScriptVarInitA] | array_init)))[FinishScriptVarA]);

    script_string = keyword_p("string") >> *((var_decl[CreateScriptVarA] >> -(equalSign[GeneralE] >> (string_immediateValue[ScriptStringInitA] | string_array_init)))[FinishScriptStringA]);
  
    // The actual script grammer - rules that contain multiple entities (e.g. local, synonyms),
    // have their finishing actions defined on those entities themselves, rather than here.
    entire_script = *(oppar[GeneralE]
        >> (version
            | include
            | use
            | (alwaysmatch_p[EnableScriptVersionA<2>] >> exports)
            | define[FinishDefineA]
            | instance_decl[FinishClassA]
            | class_decl[FinishClassA]
            | procedure_decl[FinishProcedureA]
            | scriptNum
            | synonyms
            | script_var
            | script_string)[{IdentifierE, ParseAutoCompleteContext::TopLevelKeyword}]
        >> clpar[GeneralE]);

    // And for headers, only defines and includes are allowed. And also #ifdef!
    entire_header = *
        (
        (keyword_p("#ifdef") >> alphanum_p[EvaluateIfDefA])
        | keyword_p("#endif")[EvaluateEndIfA]
        | (oppar[GeneralE] >> (include | define[FinishDefineA])[IdentifierE] >> clpar[GeneralE])
        );
}

void SyntaxContext::ReportError(std::string error, streamIt pos)
{
#ifdef PARSE_DEBUG
    if (ParseDebug)
    {
        OutputDebugString(error.c_str());
        OutputDebugString("\n");
    }
#endif
    // Prefer already-reported errors at the same spot
    // Otherwise prefer errors at a later position (presumably it means more
    // stuff got successfully parsed)
    if ((_beginning < pos) || _error.empty())
    {
        _error = error;
        _beginning = pos;
    }
}

//
// This does the parsing.
//
bool StudioSyntaxParser::Parse(Script &script, streamIt &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pError, bool addCommentsToOM)
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

bool StudioSyntaxParser::Parse(Script &script, streamIt &stream, std::unordered_set<std::string> preProcessorDefines, SyntaxContext &context)
{
    bool fRet = false;
    if (entire_script.Match(&context, stream).Result() && (*stream == 0)) // Needs a full match
    {
        fRet = true;
    }
    return fRet;
}

bool StudioSyntaxParser::ParseHeader(Script &script, streamIt &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pError)
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

unordered_set<string> PreProcessorDefinesFromSCIVersion(SCIVersion version)
{
    unordered_set<string> defines;
    // Only two versions supported for now.
    if (version.SeparateHeapResources)
    {
        defines.insert("SCI_1_1");
    }
    else
    {
        defines.insert("SCI_0");
    }
    return defines;
}