#include "stdafx.h"
#include "ScriptOMAll.h"
#include "StudioSyntaxParser.h"
#include "SCISyntaxParser.h"
#include "ParserActions.h"
#include "Operators.h"
#include "OperatorTables.h"
#include "format.h"

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

// For better error reports:
vector<string> SCIStatementKeywords =
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
    "for",
    "return",
    "cond",
    "while",
    "define",
};

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
    "super",
    "mod",
    "or",
    "and",
    "not",
    "of",
    // "scriptNumber",  // This is special, because it's a value. So don't count it as a banned keyword.
    "public",
    "define",
    "&tmp",
    "&rest",
    "script#"
    // neg, send, case, do, default, export are also keywords, but for the Studio language.

    // The following are original Sierra constructs that are not supported yet.
    "extern"        // For linking public procedures
    "selectors"     // For the selector list
    "global"        // For global var declarations
    "classdef"      //
    "methods"       // Method forward declarations (also methods in classdef)
    "class#"        // In classdef
    "super#"        // In classdef
    "file#"         // Procedure forward declarations
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

// Same as above, but ignores extra keywords.
template<typename _It, typename _TContext>
bool AlphanumPNoKeywordOrTerm2(const ParserSCI *pParser, _TContext *pContext, _It &stream)
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

void InitEnumStartA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result()) { pContext->Integer = 0; }
}
void CreateEnumDefineA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        if (pContext->ifDefDefineState != IfDefDefineState::False)
        {
            auto define = make_unique<Define>(pContext->ScratchString(), pContext->Integer++);
            define->SetScript(&pContext->Script());
            define->SetPosition(stream.GetPosition());
            pContext->Script().AddDefine(move(define));
        }
    }
}
void RestructureNaryAssociativeOpA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // Restructure (a b c d e) into (a (b (c (d e))))
        // We want evaluation to procede left to right
        auto naryOp = pContext->StealSyntaxNode<NaryOp>();
        auto &statements = naryOp->GetStatements();
        size_t operandCount = statements.size();
        assert(operandCount >= 2);
        auto binaryOp = make_unique<BinaryOp>(naryOp->Operator);
        binaryOp->SetPosition(naryOp->GetPosition());
        auto it = statements.rbegin();
        binaryOp->SetStatement2(move(*it));
        ++it;
        for (; it != statements.rend(); ++it)
        {
            if (!binaryOp->GetStatement1())
            {
                binaryOp->SetStatement1(move(*it));
            }
            else
            {
                // Make a new op.
                auto newBinaryOp = make_unique<BinaryOp>(naryOp->Operator);
                newBinaryOp->SetStatement2(move(binaryOp));
                binaryOp = move(newBinaryOp);
                binaryOp->SetPosition(naryOp->GetPosition());
                binaryOp->SetStatement1(move(*it));
            }
        }
        
        // Now set this back in as the current syntax node.
        pContext->ReplaceSyntaxNode(move(binaryOp));
    }
}

const size_t MaxOpLength = 4;

// Takes the operators, sorts them, puts them into a double-null terminated string.
std::vector<char> GenerateOperatorStringWorker(const set<std::string> &ops)
{
    std::vector<char> result;
    // Organize these by group
    std::map<char, std::set<std::string>> temp;
    bool hasEmpty = false;
    for (auto op : ops)
    {
        assert(op.length() <= MaxOpLength);
        // TODO: Deal with empties
        hasEmpty = hasEmpty || !op[0];
        if (op[0])
        {
            temp[op[0]].insert(op.substr(1));
        }
    }
    // The size of our current level, plus an extra for "empty" (op that terminates at this level) and a sentinel indicating the end of this level.
    size_t relativeOffsetToNext = (temp.size() + (hasEmpty ? 1 : 0)) * 2 + 1;
    std::vector<std::vector<char>> subEntries;
    for (auto &entry : temp)
    {
        std::vector<char> subEntry = GenerateOperatorStringWorker(entry.second);
        result.push_back(entry.first);  // The char
        assert(relativeOffsetToNext < 128);
        result.push_back((char)relativeOffsetToNext);
        relativeOffsetToNext += subEntry.size();
        subEntries.push_back(subEntry);
    }
    if (hasEmpty)
    {
        // Deal with it last.
        result.push_back(' ');  // A space, indicating the end
        result.push_back(0);    // Zero, indicating no offset to next char list.
    }
    // Finally, the sentinel value indicating the end:
    result.push_back(0);
    // Now copy the sub entries
    for (auto &subEntry : subEntries)
    {
        copy(subEntry.begin(), subEntry.end(), back_inserter(result));
    }
    return result;
}

std::string GenerateOperatorString(const set<std::string> &ops)
{
    auto result = GenerateOperatorStringWorker(ops);
    return std::string(result.begin(), result.end());
}

template<typename _TContext>
bool SCIOptimizedOperatorP(const ParserSCI *pParser, _TContext *pContext, streamIt &stream)
{
    const char *currentdb = pParser->_psz;
    char finalOperator[MaxOpLength + 1];
    char *buildOperator = finalOperator;
    char ch;
    while (ch = *stream)
    {
        const char *currentDbStart = currentdb;

        //    not at end     not equal to char      and isn't end of op 
        while (*currentdb && (ch != *currentdb) && (*currentdb != ' ' || !isspace(ch)))
        {
            currentdb += 2;
        }
        if (*currentdb)
        {
            if (*currentdb == ' ')
            {
                *buildOperator = 0;
                pContext->ScratchString() = finalOperator;
                return true;
            }
            *buildOperator++ = ch;
            assert((buildOperator - finalOperator) <= MaxOpLength);
            // Adjust offset and continue with the next character
            int offset = *(currentdb + 1);
            assert(offset);
            currentdb = currentDbStart + offset;
        }
        else
        {
            // Not a match
            return false;
        }
        ++stream;
    }
    return false;
}


ParserSCI SCISyntaxParser::char_p(const char *psz) { return ParserSCI(CharP, psz); }
ParserSCI SCISyntaxParser::operator_p(const char *psz) { return ParserSCI(SCIOptimizedOperatorP, psz); }
ParserSCI SCISyntaxParser::keyword_p(const char *psz) { return ParserSCI(KeywordP, psz); }
ParserSCI SCISyntaxParser::generateSyntaxNodeD()
{
    ParserSCI syntaxnode(SyntaxNodeD);
    return syntaxnode[EndSyntaxNodeA];
}


template<typename _It, typename _TContext, typename _CommentPolicy>
bool QuotedStringSCIP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    return _ReadStringSCI<_TContext, _It, '"', '"'>(pContext, stream, pContext->ScratchString());
}

template<typename _It, typename _TContext, typename _CommentPolicy>
bool BraceStringSCIP(const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, _It &stream)
{
    return _ReadStringSCI<_TContext, _It, '{', '}'>(pContext, stream, pContext->ScratchString());
}

template<typename _It, typename _TContext, typename _CommentPolicy>
void ValueErrorE(MatchResult &match, const ParserBase<_TContext, _It, _CommentPolicy> *pParser, _TContext *pContext, const _It &stream)
{
    if (!match.Result())
    {
        pContext->ReportError("Expected an expression.", stream);
    }
}

void CreateSynA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->CreateSynonym();
        pContext->SynonymPtr->MainWord = pContext->ScratchString();
        pContext->SynonymPtr->SetPosition(stream.GetPosition());
    }
    else
    {
        pContext->ReportError("Expected word.", stream);
    }
}
void AddSynA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->SynonymPtr->Synonyms.push_back(pContext->ScratchString());
    }
    else
    {
        pContext->ReportError("Expected word.", stream);
    }
}
void FinishSynA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->Script().AddSynonym(std::move(pContext->SynonymPtr));
    }
}

void SaveScratchStringA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->ScratchString2() = pContext->ScratchString();
    }
}
void SaveGlobalVarIndexA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->Integer2 = pContext->Integer;
        pContext->PropertyValueWasSet = false;  // Also reset this.
    }
}
void AddExternDeclA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        auto externDecl = make_unique<ExternDeclaration>();
        externDecl->SetName(pContext->ScratchString2());
        assert(pContext->PropertyValueWasSet);
        externDecl->ScriptNumber = pContext->PropertyValue;
        externDecl->Index = pContext->Integer;
        externDecl->SetPosition(stream.GetPosition());
        pContext->Script().Externs.push_back(move(externDecl));
    }
}
void AddSelectorDeclA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        auto selectorDecl = make_unique<SelectorDeclaration>();
        selectorDecl->SetName(pContext->ScratchString());
        selectorDecl->Index = pContext->Integer;
        selectorDecl->SetPosition(stream.GetPosition());
        pContext->Script().Selectors.push_back(move(selectorDecl));
    }
}

void AddClassDefDeclA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->Script().ClassDefs.push_back(pContext->StealSyntaxNode<ClassDefDeclaration>());
    }
}
void SetScriptNumberA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->GetSyntaxNode<ClassDefDeclaration>()->ScriptNumber = pContext->Integer;
    }
    else
    {
        pContext->ReportError(errInteger, stream);
    }
}
void SetClassNumberA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->GetSyntaxNode<ClassDefDeclaration>()->ClassNumber = pContext->Integer;
    }
    else
    {
        pContext->ReportError(errInteger, stream);
    }
}
void SetSuperNumberA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->GetSyntaxNode<ClassDefDeclaration>()->SuperNumber = pContext->Integer;
    }
    else
    {
        pContext->ReportError(errInteger, stream);
    }
}
void SetFileA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->GetSyntaxNode<ClassDefDeclaration>()->File = pContext->ScratchString();
    }
    else
    {
        pContext->ReportError(errFileNameString, stream);
    }
}
void AddClassDefPropA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result()) { pContext->GetSyntaxNode<ClassDefDeclaration>()->Properties.push_back({ pContext->ScratchString(), pContext->Integer }); }
}
void AddClassDefMethodA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result()) { pContext->GetSyntaxNode<ClassDefDeclaration>()->Methods.push_back(pContext->ScratchString()); }
}
void AddGlobalEntryA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        auto globalEntry = std::make_unique<GlobalDeclaration>();
        globalEntry->Index = pContext->Integer2;
        if (pContext->PropertyValueWasSet)
        {
            globalEntry->InitialValue = std::make_unique<PropertyValue>(pContext->PropertyValue);
        }
        globalEntry->SetName(pContext->ScratchString2());
        pContext->Script().Globals.push_back(move(globalEntry));
    }
}
void AddMethodFwdA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->ClassPtr->MethodForwards.push_back(pContext->ScratchString());
    }
}
void AddProcedureFwdA(MatchResult &match, const ParserSCI *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->Script().ProcedureForwards.push_back(pContext->ScratchString());
    }
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
    selector_send_p(SelectorP_Term<':'>),
    propget_p(SelectorP_Term<'?'>),
    filename_p(FilenameP),
    asmInstruction_p(AsmInstructionP),
    alphanumNK_p(AlphanumPNoKeywordOrTerm),
    alphanumNK_p2(AlphanumPNoKeywordOrTerm2),
    alphanumSendToken_p(AlphanumPSendTokenOrTerm),
    alphanum_p(AlphanumP),
    alwaysmatch_p(AlwaysMatchP),
    bracestring_p(BraceStringSCIP),
    squotedstring_p(SQuotedStringP),
    quotedstring_p(QuotedStringSCIP),
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

    // Some defaults.
    ParseACChannels ch1StartStatement = SetChannel(NoChannels, ParseAutoCompleteChannel::One, ParseAutoCompleteContext::StartStatementExtras);
    ParseACChannels ch1Block = SetChannel(NoChannels, ParseAutoCompleteChannel::One, ParseAutoCompleteContext::Block);
    ParseACChannels ch2PureValue = SetChannel(NoChannels, ParseAutoCompleteChannel::Two, ParseAutoCompleteContext::PureValue);
    ParseACChannels ch2Block = SetChannel(NoChannels, ParseAutoCompleteChannel::Two, ParseAutoCompleteContext::Block);
    ParseACChannels ch3Selector = SetChannel(NoChannels, ParseAutoCompleteChannel::Three, ParseAutoCompleteContext::Selector);
    ParseACChannels ch3Keyword = SetChannel(NoChannels, ParseAutoCompleteChannel::Three, ParseAutoCompleteContext::ClassLevelKeyword);
    ParseACChannels ch3Block = SetChannel(NoChannels, ParseAutoCompleteChannel::Three, ParseAutoCompleteContext::Block);
    ParseACChannels ch4Rest = SetChannel(NoChannels, ParseAutoCompleteChannel::Four, ParseAutoCompleteContext::Rest);
    ParseACChannels ch4Else = SetChannel(NoChannels, ParseAutoCompleteChannel::Four, ParseAutoCompleteContext::Else);
    ParseACChannels ch4Block = SetChannel(NoChannels, ParseAutoCompleteChannel::Four, ParseAutoCompleteContext::Block);

    // (xxx             -> procname, pure value or keyword
    // (something xxx   -> pure value or selector name 
    ParseACChannels acStartStatement = ch1StartStatement | ch2PureValue | ch3Keyword | ch4Block;
    ParseACChannels acJustAValue = ch1Block | ch2PureValue; // Don't block channel 3, because parent parser might want to add in selectors, not sure about ch4
    ParseACChannels acInSendOrProcCall = ch1Block | ch2PureValue | ch3Selector | ch4Rest;
    // The else in an if statement is special because it doesn't need to be enclosed in ()
    ParseACChannels acElseAddin = ch4Else;
    // We could do more for else here, like removing it from the keywords list, and then manually adding
    // it back for case statements in switch or cond statements.

    // An integer or plain alphanumeric token
    immediateValue = integer_p[PropValueIntA<errInteger>] | alphanumNK_p[PropValueStringA<ValueType::Token, errNoKeywordOrSelector>];
    string_immediateValue = integer_p[PropValueIntA<errInteger>] | alphanumNK_p[PropValueStringA<ValueType::Token, errNoKeywordOrSelector>] | quotedstring_p[{PropValueStringA<ValueType::ResourceString>, ParseAutoCompleteContext::Block}] | bracestring_p[PropValueStringA<ValueType::String>];

    // Top level constructs
    include = keyword_p("include") >> (quotedstring_p | filename_p)[{AddIncludeA, ParseAutoCompleteContext::Block}];
    use = keyword_p("use") >> (quotedstring_p | filename_p)[{AddUseA, ParseAutoCompleteContext::ScriptName}];
    scriptNum = keyword_p("script#") >> immediateValue[{ScriptNumberA, ParseAutoCompleteContext::DefineValue }];

    define = keyword_p("define")[CreateDefineA] >> alphanumNK_p[DefineLabelA] >> integer_p[DefineValueA];

    enumStatement = keyword_p("enum")[InitEnumStartA] >> -integer_p[ErrorA<errInteger>] >> *alphanumNK_p[CreateEnumDefineA];

    general_token = (alphanumNK_p)[{nullptr, ParseAutoCompleteContext::None, "TOKEN"}];

    // Matches #posn
    // Also matches #posn? or #posn: for backwards compatibility.
    selector_literal = pound >> (alphanumNK_p2[{nullptr, ParseAutoCompleteContext::Selector}] | selector_send_p);

    pointer = atsign;

    export_entry = general_token[{nullptr, ParseAutoCompleteContext::Export}] >> integer_p[AddExportA];
    exports = keyword_p("public") >> *export_entry;

    synonym_entry = oppar >> alphanum_p[CreateSynA] >> *alphanum_p[AddSynA] >> clpar;
    synonyms = keyword_p("synonyms") >> *synonym_entry[FinishSynA];

    rvalue_variable =
        (opbracket >> general_token[ComplexValueStringA<ValueType::Token, errVarName>] >> statement[ComplexValueIndexerA] >> clbracket) |
        general_token[ComplexValueStringA<ValueType::Token>];

    value =
        alwaysmatch_p[SetStatementA<ComplexPropertyValue>]
        >> (integer_p[ComplexValueIntA]
        | keyword_p("argc")[ComplexValueParamTotalA]
        | quotedstring_p[{ComplexValueStringA<ValueType::ResourceString>, ParseAutoCompleteContext::Block}]
        | squotedstring_p[{ComplexValueStringA<ValueType::Said>, ParseAutoCompleteContext::Block}]
        | bracestring_p[{ComplexValueStringA<ValueType::String>, ParseAutoCompleteContext::Block}]
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
        >> bare_code_block[{StatementBindTo1stA<IfStatement, errThen>, acElseAddin }]          // Code
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
    procedure_call = alphanumNK_p[SetStatementNameA<ProcedureCall>] >> (*statement[AddStatementA<ProcedureCall>])[{nullptr, acInSendOrProcCall}];

    // posn: x y z
    send_param_call = selector_send_p[SetStatementNameA<SendParam>] >> alwaysmatch_p[SendParamIsMethod] >> *statement[AddStatementA<SendParam>];

    // expression selectorA: one two three, selectorB: one two
    // expression selectorA?
    send_call = (alwaysmatch_p[SetStatementA<SendCall>] // Simple form, e.g. gEgo
        >> ((alphanumSendToken_p[SetNameA<SendCall>]) | statement[StatementBindTo1stA<SendCall, errSendObject>])) // Expression, e.g. [clients 4], or (GetTheGuy)
        >>
        (propget_p[AddSimpleSendParamA] |             // Single prop get
        (syntaxnode_d[send_param_call[AddSendParamA]] % -comma[GeneralE])      // Or a series regular ones separated by optional comma
        )[{nullptr, acInSendOrProcCall}]              // AC stuff that's inside a send call
        ;

    // Operators
    // These are binary-only operators
    binaryOps = GenerateOperatorString({ ">>", "<<", "-", "/", "mod" });
    binary_operator = operator_p(binaryOps.c_str());

    // n-ary operators that are associative
    naryAssocOps = GenerateOperatorString({ "*", "+", "&", "|", "^", "and", "or" });
    naryassoc_operator = operator_p(naryAssocOps.c_str());

    // n-ary comparison operators
    naryCompareOps = GenerateOperatorString({ "u>=", ">=", "u>", ">", "u<=", "<=", "u<", "!=", "<", "==" });
    narycompare_operator = operator_p(naryCompareOps.c_str());

    unaryOps = GenerateOperatorString({"~", "not", "-", "++", "--" });
    unary_operator = operator_p(unaryOps.c_str());

    assignmentOps = GenerateOperatorString({ "+=", "-=", "*=", "/=", "mod=", "&=", "|=", "^=", ">>=", "<<=", "=" });
    assignment_operator = operator_p(assignmentOps.c_str());

    // blarg    or   [blarg statement]
    lvalue = (opbracket >> general_token[{SetStatementNameA<LValue>, ParseAutoCompleteContext::LValue}] >> statement[LValueIndexerA] >> clbracket) |
        keyword_p("argc")[SetStatementNameToParamTotalA<LValue>] |
        general_token[{SetStatementNameA<LValue>, ParseAutoCompleteContext::LValue}];

    rest_statement =
        keyword_p("&rest")[SetStatementA<RestStatement>]
        >> -alphanumNK_p[SetStatementNameA<RestStatement>];

    assignment =
        assignment_operator[SetOperatorA<Assignment, AssignmentOperator>]
        >> syntaxnode_d[lvalue[AssignmentVariableA]]
        >> statement[StatementBindTo1stA<Assignment, errArgument>];

    // + 5 view
    binary_operation =
        binary_operator[SetOperatorA<BinaryOp, BinaryOperator>]
        >> statement[StatementBindTo1stA<BinaryOp, errArgument>]
        >> statement[StatementBindTo2ndA<BinaryOp, errBinaryOp>];

    // (+ 1 2 3 4 5)
    // These are parsed temporarily into a NaryOp node, then restructured into next BinaryOp nodes for maximum re-usability.
    // At least two parameters are required.
    naryassoc_operation =
        (naryassoc_operator[SetOperatorA<NaryOp, BinaryOperator>]
        >> statement[AddStatementA<NaryOp>]
        >> ++statement[AddStatementA<NaryOp>])[RestructureNaryAssociativeOpA];

    // (< 3 5 9 10)
    narycompare_operation =
        (narycompare_operator[SetOperatorA<NaryOp, BinaryOperator>]
        >> statement[AddStatementA<NaryOp>]
        >> ++statement[AddStatementA<NaryOp>]);

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

    asm_block = keyword_p("asm")[SetStatementA<AsmBlock>]
        >> alwaysmatch_p[SetOpcodesExtraKeywordsA]
        >> *asm_statement[AddStatementA<AsmBlock>]
        >> alwaysmatch_p[RemoveExtraKeywordsA];

    // All possible statements.
    statement = alwaysmatch_p[StartStatementA]
        >>
        (
        (oppar >>
        // Add more here
        (
        repeat_statement |
        assignment |
        binary_operation |
        unary_operation |       // Due to -, unary must come after binary.
        naryassoc_operation |
        narycompare_operation |
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
        asm_block |
        send_call |             // Send has to come before procedure. Because procedure will match (foo sel:)
        procedure_call
        )[{nullptr, acStartStatement}] >>
        clpar)
        | (rest_statement
        | value)[{ValueErrorE, acJustAValue }]
        )[FinishStatementA];


    // An array initializer
    array_init = opbracket >> *(string_immediateValue[ScriptVarInitAutoExpandA]) >> clbracket;  // [0 0 $4c VIEW_EGO]

    // Variable declaration, with optional array size (array size must be numeric!)
    // A   or  [A 6]   or [A MY_ARRAY_SIZE]
    var_decl =
        (opbracket >> alphanumNK_p[CreateVarDeclA] >> (integer_p[VarDeclSizeA] | alphanumNK_p[VarDeclSizeConstantA])[VarDeclSizeErrorA] >> clbracket) |
        alphanumNK_p[CreateVarDeclA];

    script_var = keyword_p("local")
        >> *((var_decl[CreateScriptVarA] >> -(equalSign[GeneralE] >> (string_immediateValue[ScriptVarInitA] | array_init)))[FinishScriptVarA]);

    // StartFunctionTempVarA is needed to reset "was initializer value set". There are no initializer values for function variables in this syntax.
    function_var_decl = keyword_p("&tmp")[{ StartFunctionTempVarA, ParseAutoCompleteContext::Temp }] >> ++(var_decl[FinishFunctionTempVarA]);

    property_decl = alphanumNK_p[{CreateClassPropertyA, ParseAutoCompleteContext::ClassSelector}]
        >> statement[FinishClassPropertyStatementA];

    // (procname param0 param1 &tmp tmp0 tmp1 tmp2)
    procedure_base = oppar
        >> alphanumNK_p[FunctionNameA]
        >> *alphanumNK_p[FunctionParameterA]
        >> *function_var_decl
        >> clpar[GeneralE]
        >> *statement[FunctionStatementA];

    procedure_decl = keyword_p("procedure")[CreateProcedureA] >> procedure_base[{FunctionCloseA, ParseAutoCompleteContext::Block}];

    // Unsupported (unneeded) methods forward declaration
    // (methods
    //      m1
    //      m2
    //      m3
    // )
    methods_fwd = keyword_p("methods")[{nullptr, ParseAutoCompleteContext::ClassLevelKeyword}] >> *alphanumNK_p[AddMethodFwdA];

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
        // Though our official syntax is "of", we'll also support kindof for Sierra compatibility.
        >> -((keyword_p("of")[GeneralE] | keyword_p("kindof")[GeneralE]) >> alphanumNK_p[{ClassSuperA, ParseAutoCompleteContext::SuperClass}])
        >> -properties_decl
        >> *(oppar[GeneralE] >> 
            (
            methods_fwd |
            method_decl[FinishClassMethodA] |
            procedure_decl[FinishClassProcedureA]) >> clpar);

    instance_decl = keyword_p("instance")[CreateClassA<true>] >> classbase_decl[ClassCloseA];

    class_decl = keyword_p("class")[CreateClassA<false>] >> classbase_decl[ClassCloseA];

    // Non-code items:
    class_def =
        keyword_p("classdef")[SetStatementA<ClassDefDeclaration>] >> alphanumNK_p[SetNameA<ClassDefDeclaration>]; // TODO

    extern_entry =
        general_token[SaveScratchStringA] >> immediateValue >> integer_p[AddExternDeclA];

    extern_section =
        keyword_p("extern") >> *extern_entry;

    selector_entry =
        general_token >> integer_p[AddSelectorDeclA];

    selector_section =
        keyword_p("selectors") >> *selector_entry;

    class_def_attributes =
        (keyword_p("script#") >> integer_p[SetScriptNumberA]) |
        (keyword_p("class#") >> integer_p[SetClassNumberA]) |
        (keyword_p("super#") >> integer_p[SetSuperNumberA]) |
        (keyword_p("file#") >> quotedstring_p[SetFileA]);

    class_def_internal = alphanumNK_p[SetStatementNameA<ClassDefDeclaration>] >>
        *class_def_attributes >>
        oppar[GeneralE] >> keyword_p("properties") >> *(alphanumNK_p >> integer_p)[AddClassDefPropA] >> clpar[GeneralE] >>
        oppar[GeneralE] >> keyword_p("methods") >> *alphanumNK_p[AddClassDefMethodA] >> clpar[GeneralE];

    class_def =
        keyword_p("classdef") >> syntaxnode_d[class_def_internal[AddClassDefDeclA]];

    global_entry =
        general_token[SaveScratchStringA] >> integer_p[SaveGlobalVarIndexA] >> -(equalSign[GeneralE] >> immediateValue);

    global_section =
        keyword_p("global") >> *global_entry[AddGlobalEntryA];

    // Unsupported (unneeded) procedure fwd declaration. Same keyword as an actual procedure
    // definition, but no opening paren after (so that's how we distinguish). In-class procs are also declared here.
    // (procedure
    //      GetStatus
    //      SomeOtherProc
    //      SaveFile
    // )
    procedures_fwd =
        keyword_p("procedure") >> *alphanumNK_p[AddProcedureFwdA];

    entire_script = *(oppar[GeneralE]
        >> (include
        | use
        | define[FinishDefineA]
        | enumStatement
        | instance_decl[FinishClassA]
        | class_decl[FinishClassA]
        | procedure_decl[FinishProcedureA]
        | synonyms
        | exports
        | scriptNum

        // The following are not yet supported, but we'll include them to possibly
        // "reserve" for future use.
        | extern_section
        | global_section
        | selector_section
        | class_def
        | procedures_fwd

        | script_var)[{IdentifierE, ParseAutoCompleteContext::TopLevelKeyword}]
        >> clpar[GeneralE]);

    // And for headers, only defines, includes are allowed. And also #ifdef!
    // REVIEW: This is kind of a hack.
    entire_header = *
        (
        (keyword_p("#ifdef") >> alphanumNK_p[EvaluateIfDefA])
        | keyword_p("#endif")[EvaluateEndIfA]
        |
        (oppar[GeneralE] >>
        (include |
        define[FinishDefineA] |
        enumStatement

        // The following are not yet supported, but we'll include them to possibly
        // "reserve" for future use.
        | extern_section
        | global_section
        | selector_section
        | class_def
        | procedures_fwd

        )[IdentifierE] >>
        clpar[GeneralE])
        );

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
        // This could be a class too (not just instance). The main game class is public.
        instance.SetPublic(script.IsExport(instance.GetName()));
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
                    if (pLog)
                    {
                        pLog->ReportResult(CompileResult("The else clause must be the last clause in a cond.", script.GetScriptId(), clause->GetPosition().Line()));
                    }
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

    // Report warnings if any un-implemented constructs are used.
    std::vector<std::string> unimplementedWarnings;
    if (!script.Externs.empty())
    {
        unimplementedWarnings.push_back("extern statements(s)");
    }
    if (!script.Globals.empty())
    {
        unimplementedWarnings.push_back("global statements(s)");
    }
    if (!script.ClassDefs.empty())
    {
        unimplementedWarnings.push_back("classdef statements(s)");
    }
    if (!script.Selectors.empty())
    {
        unimplementedWarnings.push_back("selectors statements(s)");
    }
    if (!script.ProcedureForwards.empty())
    {
        unimplementedWarnings.push_back("procedure fwd declaration(s)");
    }
    bool hasMethodFwdDeclarations = false;
    for (auto &theClass : script.GetClasses())
    {
        hasMethodFwdDeclarations = hasMethodFwdDeclarations || !theClass->MethodForwards.empty();
    }
    if (hasMethodFwdDeclarations)
    {
        unimplementedWarnings.push_back("methods fwd declaration(s)");
    }
    if (pLog)
    {
        for (auto &warning : unimplementedWarnings)
        {
            std::string text = warning + "ignored - not implemented";
            pLog->ReportResult(CompileResult(text, script.GetScriptId(), 1, 0, CompileResult::CompileResultType::CRT_Warning));
        }
    }
}

// For error reporting:
template<typename _It>
void ExtractSomeToken(std::string &str, _It &stream)
{
    char ch = *stream;
    while (ch && !isspace(ch) && (ch != '(') && (ch != ')'))
    {
        str += ch;
        ch = *(++stream);
    }
}


//
// This does the parsing.
//
bool SCISyntaxParser::Parse(Script &script, streamIt &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pError, bool addCommentsToOM, bool collectComments)
{
    SyntaxContext context(stream, script, preProcessorDefines, addCommentsToOM, collectComments);
    bool fRet = false;

#ifdef PARSE_DEBUG
    context.ParseDebug = true;
#endif

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

        strError += fmt::format(" ({}, {})", errorPos.GetLineNumber(), errorPos.GetColumnNumber());

        // We can maybe improve the error by extracting a token here and seeing if it's a keyword.
        std::string maybeKeyword;
        streamIt errorPosCopy = errorPos;
        ExtractSomeToken(maybeKeyword, errorPosCopy);
        if (!maybeKeyword.empty())
        {
            if (std::find(SCIStatementKeywords.begin(), SCIStatementKeywords.end(), maybeKeyword) != SCIStatementKeywords.end())
            {
                strError += fmt::format(" (Statements must being with a parenthesis: \"({0}\").", maybeKeyword);
            }
            else if (IsOperator(maybeKeyword, sciNameToBinaryOp) || IsOperator(maybeKeyword, sciNameToAssignmentOp) || IsOperator(maybeKeyword, sciNameToUnaryOp))
            {
                strError += fmt::format(" (Operator expressions must begin with a parenthesis: \"({0}\").", maybeKeyword);
            }
            else if (maybeKeyword == "else")
            {
                strError += ": \"else\" cannot appear here.";
            }
            // Maybe more?
            else
            {
                strError += fmt::format(": \"{0}\"", maybeKeyword);
            }
        }

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

bool SCISyntaxParser::ParseHeader(Script &script, streamIt &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pError, bool collectComments)
{
    SyntaxContext context(stream, script, preProcessorDefines, false, collectComments);
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