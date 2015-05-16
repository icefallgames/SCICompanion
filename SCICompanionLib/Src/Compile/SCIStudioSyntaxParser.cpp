
//
// SCISyntaxParser implementation
//
#include "stdafx.h"
#include "ScriptOMAll.h"
#include "SCIStudioSyntaxParser.h"
#include "PMachine.h"

using namespace sci;
using namespace std;

unordered_set<string> opcodeSet;
unordered_set<string> &GetOpcodeSet()
{
    if (opcodeSet.empty())
    {
        opcodeSet.insert(OpcodeNames, OpcodeNames + ARRAYSIZE(OpcodeNames));
    }
    return opcodeSet;
}

bool IsOpcode(const std::string &theString)
{
    GetOpcodeSet();
    return opcodeSet.find(theString) != opcodeSet.end();
}

const char *g_keywords[3] =
{
    "if",
    "while",
    "do",
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
Parser dot(CharP, ".");
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
    void Traverse(IExploreNodeContext *pContext, IExploreNode &en)  { throw std::exception("Not Implemented"); }

    void AddAndOr(bool fAnd) { _andOrs.push_back(fAnd); }

    SingleStatementVector &GetItems() { return _segments; }
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
void AddSendParamA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
		pContext->GetPrevSyntaxNode<SendCall>()->AddSendParam(pContext->StealSyntaxNode<SendParam>());
    }
}
void AddSendParamB(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
		pContext->GetPrevSyntaxNode<SendCall>()->AddSendParam(std::move(pContext->StealSyntaxNode<SendParam>()));
    }
}
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

void SetLabelA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->GetSyntaxNode<Asm>()->SetLabel(pContext->ScratchString());
    }
}

void SendParamIsMethod(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->GetSyntaxNode<SendParam>()->SetIsMethod(true);
    }
}



//
// Syntax node
//
// REVIEW: need to move this to SyntaxParser.h/.cpp, but I had trouble.
bool SyntaxNodeD(const Parser *pParser, SyntaxContext *pContext, streamIt &stream)
{
    // Make room on the stack for a statement
    pContext->PushSyntaxNode();
    // Then call our sub parser
    return pParser->_pa->Match(pContext, stream).Result();
}
void EndSyntaxNodeA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    // No matter what the state, pop a node off the stack.
    pContext->DeleteAndPopSyntaxNode();
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

// Uses/Includes
void AddUseA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->Script().AddUse(pContext->ScratchString());
    }
}
void AddIncludeA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->Script().AddInclude(pContext->ScratchString());
    }
}

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

void AddExportA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        unique_ptr<ExportEntry> entry = make_unique<ExportEntry>();
        entry->SetPosition(stream.GetPosition());
        entry->Slot = pContext->Integer;
        entry->Name = pContext->ScratchString();
        pContext->Script().GetExports().push_back(move(entry));
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

template<typename _T>
void SetStatementA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->CreateSyntaxNode<_T>(stream);
        pContext->GetSyntaxNode<_T>()->SetPosition(stream.GetPosition());
    }
}

// Set the name of a SyntaxNode
template<typename _T>
void SetStatementNameA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->CreateSyntaxNode<_T>(stream);
        pContext->GetSyntaxNode<_T>()->SetName(pContext->ScratchString());
    }
}
template<typename _T>
void SetNameA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->GetSyntaxNode<_T>()->SetName(pContext->ScratchString());
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

void ScriptNumberA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        if (pContext->PropertyValue.GetType() == ValueType::Number)
        {
			pContext->Script().SetScriptNumber(pContext->PropertyValue.GetNumberValue());
        }
        else
        {
			pContext->Script().SetScriptNumberDefine(pContext->PropertyValue.GetStringValue());
        }
    }
}

//
// Defines
//
void CreateDefineA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->CreateDefine();
        pContext->DefinePtr->SetScript(&pContext->Script());
    }
}
void DefineLabelA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->DefinePtr->SetLabel(pContext->ScratchString());
        pContext->DefinePtr->SetPosition(stream.GetPosition());
    }
    else
    {
        pContext->ReportError("Expected define label.", stream);
    }
}

void DefineValueA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->DefinePtr->SetValue(pContext->Integer);
    }
    else
    {
        pContext->ReportError("Expected integer value.", stream);
    }
}
void FinishDefineA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        if (pContext->ifDefDefineState != IfDefDefineState::False)
        {
            pContext->Script().AddDefine(move(pContext->DefinePtr));
        }
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

// Variable Declaration (e.g. foo[5])
void CreateVarDeclA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->StartVariableDecl(pContext->ScratchString());
    }
}
void VarDeclSizeA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->VariableDecl->SetSize(pContext->Integer);
    }
}

// Script variable
void CreateScriptVarA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->CreateScriptVariable();
        pContext->VariableDeclPtr->SetName(pContext->VariableDecl->GetName());
		pContext->VariableDeclPtr->SetSize(pContext->VariableDecl->GetSize());
        pContext->VariableDeclPtr->SetIsUnspecifiedSize(pContext->VariableDecl->IsUnspecifiedSize());
        pContext->VariableDeclPtr->SetScript(&pContext->Script());
        pContext->VariableDeclPtr->SetPosition(stream.GetPosition());
    }
    else
    {
        pContext->ReportError("Expected variable declaration.", stream);
    }
}
void ScriptVarInitA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // Add a value to the script variable.
        pContext->VariableDeclPtr->AddSimpleInitializer(pContext->PropertyValue);
    }
    else
    {
        pContext->ReportError("Script variables cannot be initialized with this type of value.", stream);
    }
}
void FinishScriptVarA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->Script().AddVariable(move(pContext->VariableDeclPtr));
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

// Function temp vars
void StartFunctionTempVarA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // Just make sure we clear out any value (in case its used for initialization).
		// Actually this is no longer true. Variables aren't zero-initialized.
		// We actually need to track whether or not the variable has an initial value. Temp variables (i.e. in functions)
		// can only have initial values if they are not arrays (and the initial values are set explicitly by assignments in byte code)
		// I guess this is an SCIStudio limitation, but we'll keep the same syntax.
		pContext->PropertyValueWasSet = false;	// So we'll actually say it was not set.
        pContext->PropertyValue.Zero();
    }
}
void FinishFunctionTempVarA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
		pContext->VariableDecl->SetPosition(stream.GetPosition());
		// REVIEW
        // Use the current PropertyValue for initialization (it not used, it was zero'd out in StartFunctionTempVarA)
		if (pContext->PropertyValueWasSet)
		{
			pContext->FunctionPtr->AddVariable(move(pContext->VariableDecl), pContext->PropertyValue);
		}
		else
		{
			pContext->FunctionPtr->AddVariable(move(pContext->VariableDecl));
		}
    }
}

// Function
void CreateProcedureA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->CreateProcedure();
    }
}
void CreateMethodA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->CreateMethod();
        pContext->FunctionPtr->SetOwnerClass(pContext->ClassPtr.get());
    }
}
void ProcedurePublicA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        static_cast<ProcedurePtr>(pContext->FunctionPtr.get())->SetPublic(true);
    }
}
void FunctionNameA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->FunctionPtr->SetName(pContext->ScratchString());
        pContext->FunctionPtr->SetScript(&pContext->Script());
        pContext->FunctionPtr->SetPosition(stream.GetPosition());
    }
    else
    {
        pContext->ReportError("Expected function name.", stream);
    }
}
void FunctionParameterA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->FunctionPtr->AddParam(pContext->ScratchString());
    }
}
void FunctionStatementA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->FunctionPtr->AddStatement(move(pContext->StatementPtrReturn));
    }
}
void FunctionCloseA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    pContext->FunctionPtr->SetEndPosition(stream.GetPosition());
    if (!match.Result() && pParser->_psz) // Check for _psz here... might have ')', or nothing.
    {
        GeneralE(match, pParser, pContext, stream);
    }
}
void FinishProcedureA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
		pContext->Script().AddProcedure(move(pContext->GetFunctionAsProcedure()));
    }
}
void ProcedureClassA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        static_cast<ProcedurePtr>(pContext->FunctionPtr.get())->SetClass(pContext->ScratchString());
    }
    else
    {
        pContext->ReportError("Expected class name.", stream);
    }
}


template<typename _It>
void IdentifierE(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const _It &stream)
{
    if (!match.Result())
    {
        pContext->ReportError("Expected keyword.", stream);
    }
}

void PropValueIntA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
		pContext->PropertyValueWasSet = true;
        pContext->PropertyValue.SetValue(pContext->Integer, pContext->HexInt);
        if (pContext->NegInt)
        {
			pContext->PropertyValue.Negate();
        }
		pContext->PropertyValue.SetPosition(stream.GetPosition());
    }
}
template<ValueType type>
void PropValueStringA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
		pContext->PropertyValueWasSet = true;
        pContext->PropertyValue.SetValue(pContext->ScratchString(), type);
        pContext->PropertyValue.SetPosition(stream.GetPosition());
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

// Classes
template<bool fInstance>
void CreateClassA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->CreateClass();
        pContext->ClassPtr->SetInstance(fInstance);
        pContext->ClassPtr->SetScript(&pContext->Script());
        pContext->ClassPtr->SetPosition(stream.GetPosition());
    }
}
void FinishClassA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->Script().AddClass(move(pContext->ClassPtr));
    }
}
void ClassPublicA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->ClassPtr->SetPublic(true);
    }
}
void ClassNameA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->ClassPtr->SetName(pContext->ScratchString().c_str());
    }
}
void ClassSuperA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->ClassPtr->SetSuperClass(pContext->ScratchString());
    }
}
void ClassCloseA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    pContext->ClassPtr->SetEndPosition(stream.GetPosition()); // set the closing position no matter what
    if (!match.Result() && pParser->_psz)
    {
        GeneralE(match, pParser, pContext, stream);
    }
}
void FinishClassMethodA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
		pContext->ClassPtr->AddMethod(move(pContext->GetFunctionAsMethod()));
    }
    else
    {
        pContext->ReportError("Expected method declaration.", stream);
    }
}

// Class properties
void CreateClassPropertyA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->CreateClassProperty();
		pContext->ClassProp.SetName(pContext->ScratchString());
		pContext->ClassProp.SetPosition(stream.GetPosition());
    }
}
void FinishClassPropertyA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->ClassProp.SetValue(pContext->PropertyValue);
		pContext->ClassPtr->AddProperty(pContext->ClassProp);
    }
}

// l-value
void LValueIndexerA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
		pContext->GetSyntaxNode<LValue>()->SetIndexer(move(pContext->StatementPtrReturn));
        // SetIndexer uses copy semantics, so we need to delete the source:
        //delete pContext->StatementPtrReturn;
        //pContext->ClearStatementReturn();
    }
    else
    {
        pContext->ReportError("Expected array index.", stream);
    }
}

// Assignments
void AssignmentVariableA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        // This is the variable description we need to add to the assignment thing:
		pContext->GetPrevSyntaxNode<Assignment>()->SetVariable(move(pContext->StealSyntaxNode<LValue>()));
    }
    else
    {
        pContext->ReportError("Expected variable.", stream);
    }
}

template<typename _T, char const* error>
void StatementBindTo1stA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
		pContext->GetSyntaxNode<_T>()->SetStatement1(move(pContext->StatementPtrReturn));
    }
    else if (error)
    {
        pContext->ReportError(error, stream);
    }
}
template<typename _T, char const* error>
void StatementBindTo2ndA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
		pContext->GetSyntaxNode<_T>()->SetStatement2(move(pContext->StatementPtrReturn));
    }
    else
    {
        std::string statementName = pContext->GetSyntaxNode<_T>()->ToString();
        statementName += ":";
        statementName += error;
        pContext->ReportError(statementName.c_str(), stream);
    }
}

// Add a statement to a SyntaxNode
template<typename _T>
void AddStatementA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->GetSyntaxNode<_T>()->AddStatement(move(pContext->StatementPtrReturn));
    }
}

void StartStatementA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->PushSyntaxNode();
    }
}
void FinishStatementA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->FinishStatement(true);
        assert(pContext->StatementPtrReturn); // This is our "return value"
        pContext->StatementPtrReturn->SetPosition(stream.GetPosition()); // Not the most accurate position, but good enough.
    }
    else
    {
        pContext->FinishStatement(false);
    }
}

// asm
void SetOpcodesExtraKeywordsA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    pContext->extraKeywords = &GetOpcodeSet();
}
void RemoveExtraKeywordsA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    pContext->extraKeywords = nullptr;
}

// Complex properties
void ComplexValueIntA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        ComplexPropertyValue *pValue = pContext->GetSyntaxNode<ComplexPropertyValue>();
        pValue->SetValue(pContext->Integer, pContext->HexInt);
        if (pContext->NegInt)
        {
            pValue->Negate();
        }
    }
}
void ComplexValuePointerA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->GetSyntaxNode<ComplexPropertyValue>()->SetType(ValueType::Pointer);
    }
}
template<ValueType type>
void ComplexValueStringA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        ValueType typeToUse = type;
        ComplexPropertyValue *pValue = pContext->GetSyntaxNode<ComplexPropertyValue>();
        if (pValue->GetType() == ValueType::Pointer)
        {
            ASSERT(typeToUse == ValueType::Token);
            typeToUse = ValueType::Pointer; // It was already decided it was a pointer.
        }
        pValue->SetValue(pContext->ScratchString(), typeToUse);
    }
}
void ComplexValueIndexerA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->GetSyntaxNode<ComplexPropertyValue>()->SetIndexer(move(pContext->StatementPtrReturn));
    }
    else
    {
        pContext->ReportError("Expected array index.", stream);
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

// switch/case
void SetDefaultCaseA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (match.Result())
    {
        pContext->GetSyntaxNode<CaseStatement>()->SetDefault(true);
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

template<char const* error>
void ErrorA(MatchResult &match, const Parser *pParser, SyntaxContext *pContext, const streamIt &stream)
{
    if (!match.Result())
    {
        pContext->ReportError(error, stream);
    }
}

// Denoted as extern, so they can be used as function template parameters.
extern char const errBinaryOp[] = "Expected second argument.";
extern char const errCaseArg[] = "Expected case argument.";
extern char const errSwitchArg[] = "Expected switch argument.";
extern char const errSendObject[] = "Expected send object.";
extern char const errArgument[] = "Expected argument.";
extern char const errInteger[] = "Expected integer value.";

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

    string_immediateValue = integer_p[PropValueIntA] | alphanum_p[PropValueStringA<ValueType::Token>] | quotedstring_p[PropValueStringA<ValueType::String>];
    string_immediateValue2 = integer_p[PropValueIntA] | alphanum_p[PropValueStringA<ValueType::Token>] | quotedstring_p[PropValueStringA<ValueType::String>] | bracestring_p[PropValueStringA<ValueType::Token>];

    general_token = alphanum_p2 | bracestring_p;

    pointer = atsign;

    selector = pound >> general_token;

    // Value, one of the following forms:
    // -45 | gEgo[4] | "fwef" | 'wef' | {fwe} | $c000 | #foo | @bar
    // REVIEW: might we want to allow [] after {fwe} ? 
    simple_value =
        integer_p[PropValueIntA]
        | quotedstring_p[PropValueStringA<ValueType::String>]
        | squotedstring_p[PropValueStringA<ValueType::Said>];
        //| bracestring_p[PropValueStringA<ValueTypeInternalString>];

    value =
        alwaysmatch_p[SetStatementA<ComplexPropertyValue>]
        >> (integer_p[ComplexValueIntA]
            | quotedstring_p[ComplexValueStringA<ValueType::String>]
            | squotedstring_p[ComplexValueStringA<ValueType::Said>]
            //| bracestring_p[ComplexValueStringA<ValueTypeInternalString>]
            //| (-pointer[ComplexValuePointerA] >> alphanum_p[ComplexValueStringA<ValueType::Token>] >> -(opbracket >> statement[ComplexValueIndexerA] >> clbracket))
            | (-pointer[ComplexValuePointerA] >> general_token[ComplexValueStringA<ValueType::Token>] >> -(opbracket >> statement[ComplexValueIndexerA] >> clbracket))
            | selector[ComplexValueStringA<ValueType::Selector>]);

    property_value =
        simple_value
        | alphanum_p[PropValueStringA<ValueType::Token>];

    // moveSpeed 5
    property_decl = general_token[CreateClassPropertyA] >> property_value[FinishClassPropertyA];  // Allows for {move-cnt} 6

    // The properties thing in a class or instance
    properties_decl = oppar >> keyword_p("properties") >> *property_decl >> clpar;

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
    var_decl = alphanum_p[CreateVarDeclA] >> -(opbracket >> integer_p[VarDeclSizeA] >> clbracket);

    // General code pieces
    // (we need to put the token on the statement stack, since it's possible there could be another one in "statement"
    variable = general_token[SetStatementNameA<LValue>] >> -(opbracket >> statement[LValueIndexerA] >> clbracket);

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
        >> keyword_p("if")[SetStatementA<CppIfStatement>]
        >> oppar[GeneralE]
        >> syntaxnode_d[conditional[FinishConditionA<CppIfStatement>]]
        >> clpar
        >> code_block_no_paren[StatementBindTo1stA<CppIfStatement, errArgument>]
        >> clpar
        >> -
            (
            oppar
            >> keyword_p("else")
            >> code_block_no_paren[StatementBindTo2ndA<CppIfStatement, errBinaryOp>]
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
            |    keyword_p("case") >> statement[StatementBindTo1stA<CaseStatement, errCaseArg>])
        >> *statement[AddStatementA<CaseStatement>]
        >> clpar;

    switch_statement = oppar
        >> keyword_p("switch")[SetStatementA<SwitchStatement>]
        //>> oppar
        >> statement[StatementBindTo1stA<SwitchStatement, errSwitchArg>]
        //>> clpar
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

    send_param_call = general_token[SetStatementNameA<SendParam>]
        >> oppar[SendParamIsMethod] >> *statement[AddStatementA<SendParam>] >> clpar;

    // Note: SCIStudio requires the : come right after the text (if no send keyword), so we use +colon
    send_call = 
           (
              ((keyword_p("send")[SetStatementA<SendCall>])
                    >> ((syntaxnode_d[variable[SetLValueA<SendCall>]])                                // send flakes[4]:x
                        | (oppar >> statement[StatementBindTo1stA<SendCall, errSendObject>] >> clpar)) >> +colon)  // send (= gEgo foo):x
           |  (general_token[SetStatementNameA<SendCall>] >> +colon)                                // theMan:
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
        >> -( syntaxnode_d[rest_statement[AddSendRestA]]  // This SCIStudio syntax is strange, but the template game does it
              | general_token[AddSingleSendParamA]); 

    // Generic set of code inside parentheses
    code_block =
            oppar[SetStatementA<CodeBlock>]
        >>  *statement[AddStatementA<CodeBlock>]
        >>  clpar[GeneralE];

    // = view 1
    assignment =
            assignment_operator[SetStatementNameA<Assignment>]
        >>  syntaxnode_d[variable[AssignmentVariableA]]
        >>  statement[StatementBindTo1stA<Assignment, errArgument>];

    // + 5 view
    binary_operation =
            binary_operator[SetStatementNameA<BinaryOp>]
        >>  statement[StatementBindTo1stA<BinaryOp, errArgument>]
        >>  statement[StatementBindTo2ndA<BinaryOp, errBinaryOp>];

    // ? (exp) a b
    /*
    ternary_expression = 
            question[StartTernary]
            >> oppar[GeneralE]
            >> syntaxnode_d[conditional[FinishConditionA<CppIfStatement>]]
            >> clpar[GeneralE]
            >> statement[StatementBindTo1stA<CppIfStatement, errArgument>]
            >> statement[StatementBindTo2ndA<CppIfStatement, errBinaryOp>];
            */

    // bnot done
    unary_operation =
            unary_operator[SetStatementNameA<UnaryOp>]
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
        | code_block)[FinishStatementA];

    function_var_decl_begin = oppar >> keyword_p("var");

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
        >> alphanum_p[FunctionNameA]
        >> *alphanum_p[FunctionParameterA]
        >> clpar[GeneralE]
        >> function_var_decl
        >> *statement[FunctionStatementA];

    procedure_base = oppar
        >> alphanum_p[FunctionNameA]
        >> *alphanum_p[FunctionParameterA]
        >> clpar[GeneralE]
        >> -(keyword_p("of") >> alphanum_p[ProcedureClassA])
        >> function_var_decl
        >> *statement[FunctionStatementA];

    method_decl = oppar >> keyword_p("method")[CreateMethodA] >> method_base >> clpar[FunctionCloseA];

    // Classes
    classbase_decl = -keyword_p("public")[ClassPublicA]
        >> general_token[ClassNameA]
        >> -(keyword_p("of")[GeneralE] >> alphanum_p[ClassSuperA])
        >> -properties_decl
        >> *method_decl[FinishClassMethodA];

    instancebase_decl = -keyword_p("public")[ClassPublicA]
        >> general_token[ClassNameA]
        >> keyword_p("of")[GeneralE]
        >> alphanum_p[ClassSuperA]
        >> -properties_decl
        >> *method_decl[FinishClassMethodA];

    // Main script pieces.
    include = keyword_p("include") >> quotedstring_p[AddIncludeA];

    use = keyword_p("use") >> quotedstring_p[AddUseA];
    version = keyword_p("version") >> integer_p[SetVersionA];

    export_entry = integer_p >> alphanum_p[AddExportA];
    exports = keyword_p("exports") >> *export_entry;

    define = keyword_p("define")[CreateDefineA] >> alphanum_p[DefineLabelA] >> integer_p[DefineValueA];

    scriptNum = keyword_p("script") >> immediateValue[ScriptNumberA];

    instance_decl = keyword_p("instance")[CreateClassA<true>] >> instancebase_decl[ClassCloseA];

    class_decl = keyword_p("class")[CreateClassA<false>] >> classbase_decl[ClassCloseA];

    procedure_decl = keyword_p("procedure")[CreateProcedureA] >> -keyword_p("public")[ProcedurePublicA] >> procedure_base[FunctionCloseA];

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
            | script_string)[IdentifierE]
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
    OutputDebugString(error.c_str());
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
bool SCISyntaxParser::Parse(Script &script, streamIt &stream, std::unordered_set<std::string> preProcessorDefines, ICompileLog *pError)
{
    SyntaxContext context(stream, script, preProcessorDefines);
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
    SyntaxContext context(stream, script, preProcessorDefines);
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