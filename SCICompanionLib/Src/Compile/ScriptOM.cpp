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

// ScriptOM.cpp : implementation file
//
#include "stdafx.h"
#include "ScriptOMAll.h"
#include "SCIProps.h"
#include "CompiledScript.h"
#include "CompileContext.h"
#include "StringUtil.h"

using namespace sci;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

std::string g_restLastParamSentinel = "REST_LAST_PARAM_SENTINEL";

bool Script::IsExport(const std::string &name) const
{
    return find_if(_exports.begin(), _exports.end(), [&name](const std::unique_ptr<ExportEntry> &entry) { return entry->Name == name; }) != _exports.end();
}

ResolvedToken Script::LookupVariableName(CompileContext &context, const std::string &str, WORD &wIndex, SpeciesIndex &dataType) const
{
    ResolvedToken tokenType = ResolvedToken::Unknown;
    // Find the index of a script variable
    wIndex = 0;
    for (auto &scriptVar : _scriptVariables)
    {
        if (scriptVar->GetName() == str)
        {
            tokenType = ResolvedToken::ScriptVariable;
            dataType = context.LookupTypeSpeciesIndex(scriptVar->GetDataType(), scriptVar.get());
            break;
        }
        wIndex += scriptVar->GetSize();
    }

    // Check for script string declarations
    if (tokenType == ResolvedToken::Unknown)
    {
        wIndex = 0xFFFF; // Unused for this scenario
        for (auto &stringDecl : _scriptStringDeclarations)
        {
            if (stringDecl->GetName() == str)
            {
                tokenType = ResolvedToken::ScriptString;
                dataType = DataTypeString;
                break;
            }
        }
    }
    return tokenType;
}


//
// Used to create a new class programmatically.
//
void ClassDefinition::CreateNew(const Script *pScript, PCTSTR pszName, PCTSTR pszSpecies)
{
	_innerName = pszName;
    _superClass = pszSpecies;
    _fInstance = true;
    _fPublic = false;
}

bool ClassDefinition::SetProperty(PCTSTR pszName, PropertyValue value)
{
    bool fFound = false;
    for (size_t i = 0; !fFound && i < _properties.size(); i++)
    {
        fFound = (_properties[i]->GetName() == pszName);
        if (fFound)
        {
            _properties[i]->SetValue(value);
        }
    }
    if (!fFound)
    {
        _properties.push_back(make_unique<ClassProperty>(pszName, value));
        fFound = true;
    }
    return fFound;
}

bool ClassDefinition::GetPropertyConst(PCTSTR pszName, PropertyValue &value) const
{
    bool fFound = false;
    // STLCLEANUP
    for (size_t i = 0; !fFound && i < _properties.size(); i++)
    {
        fFound = (_properties[i]->GetName() == pszName);
        if (fFound)
        {
            const PropertyValue *valueTemp = _properties[i]->TryGetValue();
            fFound = (valueTemp != nullptr);
            if (fFound)
            {
                value = *valueTemp;
            }
            else
            {
                const PropertyValueBase *valueBaseTemp = _properties[i]->TryGetValue2();
                if (valueBaseTemp)
                {
                    fFound = true;
                    value = *valueBaseTemp;
                }
            }
        }
    }
    if (!fFound)
    {
        // Is it a pseudo prop?
        PFNCLASSTOPSEUDOPROP pfnClassToProp;
        if (IsPseudoProp(pszName, &pfnClassToProp))
        {
            fFound = (*pfnClassToProp)(this, pszName, value);
        }
    }
    return fFound;
}

bool ClassDefinition::GetProperty(PCTSTR pszName, PropertyValue &value)
{
    return GetPropertyConst(pszName, value);
}

ResolvedToken FunctionSignature::LookupVariableName(CompileContext &context, const std::string &str, WORD &wIndex, SpeciesIndex &dataType) const
{
    // Is it a parameter?
    wIndex = 0;
    for (auto &functionParam : _params)
    {
        if (functionParam->GetName() == str)
        {
            wIndex++; // since position 0 was taken up by "paramTotal"
            dataType = context.LookupTypeSpeciesIndex(functionParam->GetDataType(), this);
            return ResolvedToken::Parameter;
        }
        wIndex++;
    }

    if (str == g_restLastParamSentinel)
    {
        wIndex++;
        dataType = DataTypeAny;
        return ResolvedToken::Parameter;
    }

    return ResolvedToken::Unknown;
}

//
// Looks up a temp var or a parameter name, and provides the index
//
ResolvedToken FunctionBase::LookupVariableName(CompileContext &context, const std::string &str, WORD &wIndex, SpeciesIndex &dataType) const
{
    // Is it a temp variable?
    wIndex = 0;
    // Temporary hack
    if (str == "paramTotal")
    {
        dataType = DataTypeInt; // paramTotal is a numerical value
        return ResolvedToken::Parameter;
    }

    for (auto &tempVar : _tempVars)
    {
        if (tempVar->GetName() == str)
        {
            // Found it.  Index should already be correct.
            dataType = context.LookupTypeSpeciesIndex(tempVar->GetDataType(), tempVar.get());
            return ResolvedToken::TempVariable;
        }
        // Keep track of the "index".  Increase it by the size of the temp var (e.g. arrays have size > 1)
        wIndex += tempVar->GetSize();
    }

    wIndex = 0;
    ResolvedToken token = ResolvedToken::Unknown;
	for (auto &signature : _signatures)
	{
		token = signature->LookupVariableName(context, str, wIndex, dataType);
		if (token != ResolvedToken::Unknown)
		{
			break;
		}
	}
    return token;
}


std::string PropertyValueBase::ToString() const
{
    TCHAR szDesc[MAX_PATH];
    szDesc[0] = 0;
    switch (_type)
    {
    case ValueType::Selector:
    case ValueType::Pointer:
        StringCchCopy(szDesc, ARRAYSIZE(szDesc), (_type == ValueType::Selector) ? "#" : "@");
        // fall through...
    case ValueType::String:
    case ValueType::ResourceString:
    case ValueType::Said:
    case ValueType::Token:
        StringCchCat(szDesc, ARRAYSIZE(szDesc), _stringValue.c_str());
        break;
    case ValueType::Number:
        StringCchPrintf(szDesc, ARRAYSIZE(szDesc), _fHex ? TEXT("$%04x") : TEXT("%d"), _numberValue);
        break;
    case ValueType::None:
        StringCchCopy(szDesc, ARRAYSIZE(szDesc), TEXT("Undefined"));
        break;
    default:
        ASSERT(false);
        break;
    }
    return szDesc;
}

std::string FunctionBase::ToString() const
{
    TCHAR szDesc[MAX_PATH * 2];
	StringCchPrintf(szDesc, ARRAYSIZE(szDesc), TEXT("%s("), _innerName.c_str());
    // TODO
    /*for (size_t i = 0; i < _params.size(); i++)
    {
        StringCchCat(szDesc, ARRAYSIZE(szDesc), (PCTSTR)_params[i]->GetName().c_str());
    }*/
    StringCchCat(szDesc, ARRAYSIZE(szDesc), TEXT(")"));
    return szDesc;
}

Script::Script(PCTSTR pszFilePath, PCTSTR pszFileName) : SyntaxVersion(1)
{
    _scriptId = ScriptId(pszFileName, pszFilePath);
}
Script::Script(ScriptId script) : _scriptId(script), SyntaxVersion(1)
{
}
Script::Script() : SyntaxVersion(1)
{
}

void Script::SetScriptNumber(WORD wNumber)
{
    _scriptId.SetResourceNumber(wNumber);
}

ConditionalExpression::ConditionalExpression(std::unique_ptr<SyntaxNode> statement)
{
    AddStatement(std::move(statement));
}

//
// Destructors for script objects.
//
void SendCall::TransferParamsFrom(SendCall *pSend)
{
    ASSERT(pSend->_params.size() == 1); // Used for coalescing of "unit" send calls... so should only be one param.
    AddSendParam(move(pSend->_params[0]));
    pSend->_params.clear(); // Since we transfered....
}
void SendCall::SimplifySendObject()
{
	if (_statement1 != nullptr)
	{
		// Attempt to turn a complex object into a simple name.
        const PropertyValue *pValue = SafeSyntaxNode<PropertyValue>(_statement1.get());
		if (pValue && (pValue->GetType() == ValueType::Token))
		{
			_innerName = pValue->GetStringValue();
		}
	}
}

PropertyValueBase::PropertyValueBase(const PropertyValueBase& src)
{
    _numberValue = src.GetNumberValue();
    _stringValue = src._stringValue;
    _type = src._type;
    _fHex = src._fHex;
    _fNegate = src._fNegate;
	SetPosition(src.GetPosition());
	SetEndPosition(src.GetEndPosition());
}
PropertyValueBase& PropertyValueBase::operator=(const PropertyValueBase& src)
{
    if (this != &src)
    {
        _type = src._type;
        _stringValue = src._stringValue;
        _numberValue = src._numberValue;
        _fHex = src._fHex;
        _fNegate = src._fNegate;
        ISourceCodePosition::operator=(src);
    }
    return (*this);
}
bool PropertyValueBase::operator==(const PropertyValueBase& value2) const
{
    bool fRet = (this == &value2);
    if (!fRet)
    {
        fRet = (this->_type == value2._type) &&
                (this->_stringValue == value2._stringValue) &&
                (this->_numberValue == value2._numberValue) &&
                (this->_fNegate == value2._fNegate);
    }
    return fRet;
}
bool PropertyValueBase::operator!=(const PropertyValueBase& value)
{
    return !(*this == value);
}

void PropertyValueBase::SetValue(int iValue, IntegerFlags flags)
{
    ASSERT(iValue <= 65536);
    _numberValue = static_cast<WORD>(iValue);
    _type = sci::ValueType::Number;
    _fHex = IsFlagSet(flags, IntegerFlags::Hex);
    _fNegate = IsFlagSet(flags, IntegerFlags::Negative);
    _stringValue.clear();
}

ComplexPropertyValue::ComplexPropertyValue(ComplexPropertyValue& src) : PropertyValueBase(src)
{
    // Transfer arrayinternal.
    _pArrayInternal = std::move(src._pArrayInternal);
}

ComplexPropertyValue& ComplexPropertyValue::operator=(ComplexPropertyValue& src)
{
    if (this != &src)
    {
        PropertyValueBase::operator=(src);
        // Transfer arrayinternal.
		_pArrayInternal = std::move(src._pArrayInternal);
    }
    return (*this);
}

VariableDecl::VariableDecl() : StatementsNode(), TypedNode(), _size(1), _unspecifiedSize(false)
{
}
void VariableDecl::AddSimpleInitializer(const PropertyValue &value)
{
    // Add a copy of the property value
    AddStatement(make_unique<PropertyValue>(value));
}
ClassProperty::ClassProperty(const std::string &str, WORD wValue) : NamedNode(), TypedNode()
{
    _innerName = str;
    PropertyValue value;
    value.SetValue(wValue);
    SetValue(value);
}

ClassProperty::ClassProperty(const std::string &str, const std::string &value) : NamedNode(), TypedNode()
{
	_innerName = str;
    PropertyValue valueTemp;
    valueTemp.SetValue(value, ValueType::Token);
    SetValue(valueTemp);
}

ClassProperty::ClassProperty(const std::string &str, const PropertyValue &value) : NamedNode(), TypedNode()
{
    _innerName = str;
    SetValue(value);
}

void ClassProperty::SetValue(const PropertyValue &value)
{
    _statement1 = make_unique<PropertyValue>(value);
}

const PropertyValueBase *ClassProperty::TryGetValue2() const
{
    const PropertyValueBase *value = TryGetValue();
    if (!value)
    {
        ComplexPropertyValue *cValue = SafeSyntaxNode<ComplexPropertyValue>(_statement1.get());
        if (cValue && !cValue->GetIndexer())
        {

            value = cValue;
        }
    }
    return value;
}

const PropertyValue *ClassProperty::TryGetValue() const
{
    PropertyValue *value = nullptr;
    if (_statement1)
    {
        value = SafeSyntaxNode<PropertyValue>(_statement1.get());
    }
    return value;
}

FunctionParameter::FunctionParameter() : NamedNode(), TypedNode() {}
FunctionParameter::FunctionParameter(const std::string &str) : NamedNode(), TypedNode()
{
    _innerName = str;
}

FunctionSignature::FunctionSignature() : TypedNode(), ScriptSite(), _fMoreParameters(false), _iOptional(NoOptional) {  }

void FunctionSignature::AddParam(const std::string &name)
{
	_params.push_back(move(make_unique<FunctionParameter>(name)));
}
void FunctionSignature::AddParam(std::unique_ptr<FunctionParameter> pParam, bool fOptional)
{
    _params.push_back(move(pParam));
    if (fOptional && (_iOptional == NoOptional))
    {
        _iOptional = _params.size() - 1; // This is the beginning of optional parameters.
    }
    ASSERT(fOptional || (_iOptional == NoOptional));
}


FunctionBase::FunctionBase() : NamedNode(), StatementsNode(), ScriptSite() { _pOwnerClass = nullptr; }
FunctionBase::FunctionBase(const std::string &str) : NamedNode(), StatementsNode(), ScriptSite()
{
    _innerName = str;
}

void FunctionBase::AddParam(const std::string &param)
{
    // This is for "backwards compatibility" with SCI syntax only...
    // Most uses should go directly through FunctionSignature.
    ASSERT(!_signatures.empty()); // Whomever calls AddParam had better add a signature.
    _signatures[0]->AddParam(param);
}
void FunctionBase::AddVariable(unique_ptr<VariableDecl> pVar, PropertyValue value)
{
	std::unique_ptr<PropertyValue> pValue = std::make_unique<PropertyValue>(value);
    pVar->AddStatement(std::move(pValue));
    _tempVars.push_back(move(pVar));
}

MethodDefinition::MethodDefinition() { _fPrivate = false; }

ProcedureDefinition::ProcedureDefinition() { _public = false; }

ClassDefinition::ClassDefinition() : _fPublic(false)
{
    _properties.reserve(24); // performance
}

void ClassDefinition::AddProperty(const std::string &name, uint16_t value)
{
    _properties.push_back(make_unique<ClassProperty>(name, value));
}

ScriptSite::ScriptSite()
{
    _pScript = nullptr;
}

std::string Comment::GetSanitizedText() const
{
    if (_innerName.empty())
    {
        return _innerName;
    }
	if ((_innerName[0] == '/') && (_innerName[1] == '/'))
    {
        // Remove the endline and any spaces
        return trim(trim(_innerName.substr(2), '\n'), ' ');
    }
    else if (_innerName[0] == ';')
    {
        // Remove the endline and any spaces
        return trim(trim(_innerName.substr(1), '\n'), ' ');
    }
    else if ((_innerName[0] == '/') && (_innerName[1] == '*'))
    {
        size_t len = _innerName.length();
        assert(len >= 4);
        std::string temp = _innerName.substr(2, len - 4);
        // Now trim any extra stars off the ends
        temp = trim(temp, '*');
        // And any space characters: REVIEW: This should really be anything up to the \n on both sides
        return trim(temp, ' ');
    }
    else
    {
        // Maybe it's alreayd sanitized (e.g. the polygon headers we output)
        return _innerName;
    }
}

CodeBlock::CodeBlock(SyntaxNodeVector statements) : StatementsNode(std::move(statements)) {}

CodeBlock::CodeBlock(std::unique_ptr<SyntaxNode> statement) { _segments.push_back(std::move(statement)); }

StatementsNode::StatementsNode(SyntaxNodeVector statements) : _segments(std::move(statements))
{

}

void SourceCodeWriter::IndentToCommentColumn()
{
    ptrdiff_t origPosition = (ptrdiff_t)out.tellp();
    ptrdiff_t origPositionG = (ptrdiff_t)out.tellg();
    ptrdiff_t position  = origPosition - 1;
    ptrdiff_t currentCharCount = 0;
    out.seekg(position);
    while ((position >= 0) && (out.peek() != '\n'))
    {
        currentCharCount++;
        position--;
        out.seekg(position);
    }

    ptrdiff_t numSpacesToIndex = max(1, defaultInlineCommentColumn - currentCharCount);

    // Restore
    out.seekg(origPositionG);

    std::string spaces;
    spaces.append(numSpacesToIndex, ' ');
    out << spaces;
}

void SourceCodeWriter::EnsureNewLine(const sci::SyntaxNode *lastNodeWritten)
{
    if (!fInline)
    {
        if (out.tellp() > lastNewLineLength)
        {
            // If we're going to a new line, check to see if we have comments we need to output
            bool needNewLine = true;
            if (lastNodeWritten && pComments)
            {
                // TODO: Need some whitespace. Do we tab out to a certain point?
                needNewLine = !pComments->Sync(lastNodeWritten, *this, 1);
            }
            if (needNewLine)
            {
                out << pszNewLine;
            }
            lastNewLineLength = out.tellp();
        }
        // Otherwise we had just added a line.
    }
}

// Visitor pattern, with enter/leave wrapper.
void Script::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void ClassDefinition::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void FunctionParameter::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void FunctionSignature::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void PropertyValue::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void ComplexPropertyValue::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void Define::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void ClassProperty::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void VariableDecl::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void MethodDefinition::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void ProcedureDefinition::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void Synonym::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void CodeBlock::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void ConditionalExpression::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void Comment::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void SendParam::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void LValue::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void RestStatement::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void Cast::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void SendCall::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void ProcedureCall::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void ReturnStatement::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void ForLoop::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void WhileLoop::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void DoLoop::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void BreakStatement::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void ContinueStatement::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void CaseStatement::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void SwitchStatement::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void CondStatement::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void Assignment::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void BinaryOp::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void NaryOp::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void UnaryOp::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void IfStatement::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void Asm::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void AsmBlock::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void ExportEntry::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
void WeakSyntaxNode::Accept(ISyntaxNodeVisitor &visitor) const { visitor.Enter(*this); visitor.Visit(*this); visitor.Leave(*this); }
