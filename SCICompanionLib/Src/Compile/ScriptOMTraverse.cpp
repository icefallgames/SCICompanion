#include "stdafx.h"
#include "ScriptOMAll.h"

using namespace sci;
using namespace std;

class PerformTraverse : public std::binary_function<SyntaxNode*, IExploreNodeContext*, void>
{
public:
    PerformTraverse(IExploreNodeContext *pContext, IExploreNode &en) : _pContext(pContext), _en(en) {}
    void operator()(SyntaxNode* pNode) const
    {
        pNode->Traverse(_pContext, _en);
    }
private:
    IExploreNodeContext *_pContext;
    IExploreNode &_en;
};

template<typename T>
void ForwardTraverse(const T &container, IExploreNodeContext *pContext, IExploreNode &en)
{
    for_each(container.begin(), container.end(), PerformTraverse(pContext, en));
}

template<typename T>
class PerformTraverse2
{
public:
	PerformTraverse2(IExploreNodeContext *pContext, IExploreNode &en) : _pContext(pContext), _en(en) {}
	void operator()(const unique_ptr<T>& pNode) const
	{
		pNode->Traverse(_pContext, _en);
	}
private:
	IExploreNodeContext *_pContext;
	IExploreNode &_en;
};

template<typename T>
void ForwardTraverse2(const vector<unique_ptr<T>> &container, IExploreNodeContext *pContext, IExploreNode &en)
{
	for_each(container.begin(), container.end(), PerformTraverse2<T>(pContext, en));
}


class PerformTraverse3 : public std::binary_function<SyntaxNode*, IExploreNodeContext*, void>
{
public:
	PerformTraverse3(IExploreNodeContext *pContext, IExploreNode &en) : _pContext(pContext), _en(en) {}
	void operator()(SyntaxNode &node) const
	{
		node.Traverse(_pContext, _en);
	}
private:
	IExploreNodeContext *_pContext;
	IExploreNode &_en;
};

template<typename T>
void ForwardTraverse3(T &container, IExploreNodeContext *pContext, IExploreNode &en)
{
	for_each(container.begin(), container.end(), PerformTraverse3(pContext, en));
}


void FunctionBase::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
    ForwardTraverse2(_signatures, pContext, en);
    ForwardTraverse2(_tempVars, pContext, en);
	ForwardTraverse2(_segments, pContext, en);
}

void FunctionSignature::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
    ExploreNodeBlock enb(en, pContext, *this);
    ForwardTraverse2(_params, pContext, en);
}

void FunctionParameter::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
    ExploreNodeBlock enb(en, pContext, *this);
}

void CodeBlock::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
	ForwardTraverse2(_segments, pContext, en);
}
void Cast::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
	_statement1->Traverse(pContext, en);
}
void SendCall::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
    // One of three forms of send params.
    ForwardTraverse2(_params, pContext, en);
	if (GetTargetName().empty())
    {
        if (!_object3 || _object3->GetName().empty())
        {
			_statement1->Traverse(pContext, en);
        }
        else
        {
			_object3->Traverse(pContext, en);
        }
    }
}
void ProcedureCall::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
	ForwardTraverse2(_segments, pContext, en);
}
void ConditionalExpression::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
	ForwardTraverse2(_segments, pContext, en);
}
void SwitchStatement::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
	_statement1->Traverse(pContext, en);
    ForwardTraverse2(_cases, pContext, en);
}
void ForLoop::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
	GetInitializer()->Traverse(pContext, en);
	_innerCondition->Traverse(pContext, en);
    _looper->Traverse(pContext, en);
	ForwardTraverse2(_segments, pContext, en);
}
void WhileLoop::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
	_innerCondition->Traverse(pContext, en);
	ForwardTraverse2(_segments, pContext, en);
}
void Script::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
    ForwardTraverse2(_procedures, pContext, en);
    ForwardTraverse2(_classes, pContext, en);
    ForwardTraverse2(_scriptVariables, pContext, en);
    ForwardTraverse2(_scriptStringDeclarations, pContext, en);
}

void VariableDecl::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
    ExploreNodeBlock enb(en, pContext, *this);
}

void DoLoop::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
	_innerCondition->Traverse(pContext, en);
	ForwardTraverse2(_segments, pContext, en);
}
void Assignment::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
	_variable->Traverse(pContext, en);
	_statement1->Traverse(pContext, en);
}

void SingleStatement::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
    if (_pThing.get() && (GetType() != NodeTypeUnknown))
    {
        _pThing->Traverse(pContext, en);
    }
}
void ClassProperty::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
}
void ClassDefinition::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
    ForwardTraverse2(_methods, pContext, en);
    ForwardTraverse3(_properties, pContext, en);
}
void SendParam::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
	ForwardTraverse2(_segments, pContext, en);
}
void LValue::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
    if (HasIndexer())
    {
		_indexer->Traverse(pContext, en);
    }
}
void ReturnStatement::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
	if (_statement1) _statement1->Traverse(pContext, en);
}
void CaseStatement::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
	if (!_fDefault) _statement1->Traverse(pContext, en);
	ForwardTraverse2(_segments, pContext, en);
}
void UnaryOp::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
	_statement1->Traverse(pContext, en);
}
void BinaryOp::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
	_statement1->Traverse(pContext, en);
	_statement2->Traverse(pContext, en);
}
void CppIfStatement::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
	_innerCondition->Traverse(pContext, en);
	_statement1->Traverse(pContext, en);
    if (_statement2)
    {
        _statement2->Traverse(pContext, en);
    }
}
void ComplexPropertyValue::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
    if (_pArrayInternal)
    {
        _pArrayInternal->Traverse(pContext, en);
    }
}
void PropertyValue::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
}
void BreakStatement::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
}
void RestStatement::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
	ExploreNodeBlock enb(en, pContext, *this);
}
void Asm::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
    ExploreNodeBlock enb(en, pContext, *this);
}
void AsmBlock::Traverse(IExploreNodeContext *pContext, IExploreNode &en)
{
    ExploreNodeBlock enb(en, pContext, *this);
    ForwardTraverse2(_segments, pContext, en);
}