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
#include "stdafx.h"
#include "ScriptOMAll.h"

using namespace sci;
using namespace std;

class PerformTraverse : public std::unary_function<SyntaxNode*, void>
{
public:
    PerformTraverse(IExploreNode &en) : _en(en) {}
    void operator()(SyntaxNode* pNode) const
    {
        pNode->Traverse(_en);
    }
private:
    IExploreNode &_en;
};

template<typename T>
void ForwardTraverse(const T &container, IExploreNode &en)
{
    for_each(container.begin(), container.end(), PerformTraverse(en));
}

template<typename T>
class PerformTraverse2
{
public:
	PerformTraverse2(IExploreNode &en) : _en(en) {}
	void operator()(const unique_ptr<T>& pNode) const
	{
		pNode->Traverse(_en);
	}
private:
	IExploreNode &_en;
};

template<typename T>
void ForwardTraverse2(const vector<unique_ptr<T>> &container, IExploreNode &en)
{
	for_each(container.begin(), container.end(), PerformTraverse2<T>(en));
}


class PerformTraverse3 : public std::unary_function<SyntaxNode*, void>
{
public:
	PerformTraverse3(IExploreNode &en) : _en(en) {}
	void operator()(SyntaxNode &node) const
	{
		node.Traverse(_en);
	}
private:
	IExploreNode &_en;
};

template<typename T>
void ForwardTraverse3(T &container, IExploreNode &en)
{
	for_each(container.begin(), container.end(), PerformTraverse3( en));
}


void FunctionBase::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
    ForwardTraverse2(_signatures, en);
    ForwardTraverse2(_tempVars, en);
	ForwardTraverse2(_segments, en);
}

void FunctionSignature::Traverse(IExploreNode &en)
{
    ExploreNodeBlock enb(en, *this);
    ForwardTraverse2(_params, en);
}

void FunctionParameter::Traverse(IExploreNode &en)
{
    ExploreNodeBlock enb(en, *this);
}

void CodeBlock::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
	ForwardTraverse2(_segments, en);
}
void NaryOp::Traverse(IExploreNode &en)
{
    ExploreNodeBlock enb(en, *this);
    ForwardTraverse2(_segments, en);
}
void Cast::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
	_statement1->Traverse(en);
}
void SendCall::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
    // One of three forms of send params.
    ForwardTraverse2(_params, en);
	if (GetTargetName().empty())
    {
        if (!_object3 || _object3->GetName().empty())
        {
			_statement1->Traverse(en);
        }
        else
        {
			_object3->Traverse(en);
        }
    }
}
void ProcedureCall::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
	ForwardTraverse2(_segments, en);
}
void ConditionalExpression::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
	ForwardTraverse2(_segments, en);
}
void SwitchStatement::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
	_statement1->Traverse(en);
    ForwardTraverse2(_cases, en);
}
void CondStatement::Traverse(IExploreNode &en)
{
    ExploreNodeBlock enb(en, *this);
    if (_statement1) { _statement1->Traverse(en); }
    ForwardTraverse2(_clausesTemp, en);
}
void ForLoop::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
	GetInitializer()->Traverse(en);
	_innerCondition->Traverse(en);
    _looper->Traverse(en);
	ForwardTraverse2(_segments, en);
}
void WhileLoop::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
	_innerCondition->Traverse(en);
	ForwardTraverse2(_segments, en);
}
void Script::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
    ForwardTraverse2(_procedures, en);
    ForwardTraverse2(_classes, en);
    ForwardTraverse2(_scriptVariables, en);
    ForwardTraverse2(_scriptStringDeclarations, en);
}

void VariableDecl::Traverse(IExploreNode &en)
{
    ExploreNodeBlock enb(en, *this);
}

void DoLoop::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
	_innerCondition->Traverse(en);
	ForwardTraverse2(_segments, en);
}
void Assignment::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
	_variable->Traverse(en);
	_statement1->Traverse(en);
}
void ClassProperty::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
}
void ClassDefinition::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
    ForwardTraverse2(_methods, en);
    ForwardTraverse2(_properties, en);
}
void SendParam::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
	ForwardTraverse2(_segments, en);
}
void LValue::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
    if (HasIndexer())
    {
		_indexer->Traverse(en);
    }
}
void ReturnStatement::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
	if (_statement1) _statement1->Traverse(en);
}
void CaseStatementBase::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
    if (!IsDefault() && _statement1) _statement1->Traverse(en);
	ForwardTraverse2(_segments, en);
}
void UnaryOp::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
	_statement1->Traverse(en);
}
void BinaryOp::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
	_statement1->Traverse(en);
	_statement2->Traverse(en);
}
void IfStatement::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
	_innerCondition->Traverse(en);
	_statement1->Traverse(en);
    if (_statement2)
    {
        _statement2->Traverse(en);
    }
}
void ComplexPropertyValue::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
    if (_pArrayInternal)
    {
        _pArrayInternal->Traverse(en);
    }
}
void PropertyValue::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
}
void BreakStatement::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
}
void ContinueStatement::Traverse(IExploreNode &en)
{
    ExploreNodeBlock enb(en, *this);
}
void RestStatement::Traverse(IExploreNode &en)
{
	ExploreNodeBlock enb(en, *this);
}
void Asm::Traverse(IExploreNode &en)
{
    ExploreNodeBlock enb(en, *this);
    ForwardTraverse2(_segments, en);
}
void AsmBlock::Traverse(IExploreNode &en)
{
    ExploreNodeBlock enb(en, *this);
    ForwardTraverse2(_segments, en);
}
void WeakSyntaxNode::Traverse(IExploreNode &en)
{
    assert(false);
}