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
#pragma once

#include "ScriptOM.h"
#include "Operators.h"

//
// This file contains less commonly used script objects that aren't
// directly referenced by any other common script objects
// (speeds up compile time for those that include "ScriptOM.h")
//

extern std::string g_restLastParamSentinel;

namespace sci
{

    //
    // A single send param.
    // 
    class SendParam : public SyntaxNode, public StatementsNode, public NamedNode
    {
        DECLARE_NODE_TYPE(NodeTypeSendParam)
    public:
        SendParam(const std::string &name, bool isMethodCall) : StatementsNode(), NamedNode(){ _fIsMethodCall = isMethodCall; _innerName = name; }
        SendParam(const std::string &name) : SendParam(name, false) {}
        SendParam() : SendParam("") {}

		SendParam(const SendParam &src) = delete;
		SendParam& operator=(const SendParam& src) = delete;

		bool HasValue() const { return !_segments.empty(); }
		std::string GetSelectorName() const { return _innerName; }
		const SyntaxNodeVector &GetSelectorParams() const { return _segments; }
        
        void SetIsMethod(bool fMethod) { _fIsMethodCall = fMethod; }
        bool IsMethod() const { return _fIsMethodCall; }

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en);

        void Accept(ISyntaxNodeVisitor &visitor) const override;

    private:
        bool _fIsMethodCall;
    };
    typedef std::vector<SendParam*> SendParamVector;

    //
    // A variable that appears in code (name and possibly array indexer)
    //
    class LValue : public SyntaxNode, public NamedNode
    {
        DECLARE_NODE_TYPE(NodeTypeLValue)
    public:
        LValue() : NamedNode() { }
        LValue(const std::string &name) : NamedNode(name){ }

		LValue(LValue &src) = delete;
		LValue& operator=(const LValue& src) = delete;

        const SyntaxNode *GetIndexer() const { return _indexer.get(); }
        bool HasIndexer() const { return _indexer.get() != nullptr; }

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en);

        void SetIndexer(std::unique_ptr<SyntaxNode> indexer) { _indexer = std::move(indexer); }
        
        void Accept(ISyntaxNodeVisitor &visitor) const override;

    private:
        std::unique_ptr<SyntaxNode> _indexer;
    };

    //
    // The rest keyword, which takes as a parameter a method parameter
    //
    class RestStatement : public SyntaxNode, public NamedNode
    {
        DECLARE_NODE_TYPE(NodeTypeRest)
    public:
        RestStatement() : NamedNode() {}
		RestStatement(const RestStatement &src) = delete;
		RestStatement& operator=(const RestStatement& src) = delete;

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        
        void Traverse(IExploreNode &en);
        void Accept(ISyntaxNodeVisitor &visitor) const override;

    };

    //
    // Represents a casting operation
    // 
    class Cast : public SyntaxNode, public OneStatementNode, public TypedNode
    {
        DECLARE_NODE_TYPE(NodeTypeCast)
    public:
        Cast() : OneStatementNode(), TypedNode() {};
        Cast(const Cast &src) = delete;
		Cast& operator=(const Cast &src) = delete;

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context) { _statement1->PreScan(context); }
        void Traverse(IExploreNode &en);
        void Accept(ISyntaxNodeVisitor &visitor) const override;

    };

    //
    // A send call e.g. (send gEgo:x)
    //
    class SendCall : public SyntaxNode, public OneStatementNode, public NamedNode
    {
        DECLARE_NODE_TYPE(NodeTypeSendCall)
    public:
        SendCall() : OneStatementNode(), NamedNode(), _fRestHack(false) {};
        SendCall(const SendCall &src) = delete;
		SendCall& operator=(const SendCall& src) = delete;

        //const LValue &GetObject() const { return _object; }
		std::string GetObject() const { return _innerName.empty() ? (_object3 ?  _object3->GetName() : "") : _innerName; }

		// Need to see how this is used.
		const std::vector<std::unique_ptr<SendParam>> &GetParams() const { return _params; }

        void TransferParamsFrom(SendCall *pSend);
        void SimplifySendObject();

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en);

        void SetLValue(std::unique_ptr<LValue> var) { _object3 = std::move(var); }
		void AddSendParam(std::unique_ptr<SendParam> pParam) { _params.push_back(std::move(pParam)); }
		void SetSendRestHack(std::unique_ptr<RestStatement> sendRest) { _rest = std::move(sendRest); _fRestHack = true; }
        
		const std::string &GetTargetName() const { return _innerName; }

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        std::unique_ptr<LValue> _object3;            // (send theFlakes[i]:x)
        std::unique_ptr<RestStatement> _rest;

    private:
		std::vector<std::unique_ptr<SendParam>> _params; // The actual send params (selectors)
        // Hack for SCIStudio template game: (???)
        bool _fRestHack;
    };

    //
    // A call to a procedure
    //
    class ProcedureCall : public SyntaxNode, public NamedNode, public StatementsNode
    {
        DECLARE_NODE_TYPE(NodeTypeProcedureCall)
    public:
		ProcedureCall() {}
        ProcedureCall(const std::string &name) : NamedNode(name) {}
        ProcedureCall(const ProcedureCall &src) = delete;
		ProcedureCall& operator=(const ProcedureCall& src) = delete;

        // Needed for syntax tree adjustment.
        void StealParams(SyntaxNodeVector &stolen) { stolen.swap(_segments); }

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en);
        

        void Accept(ISyntaxNodeVisitor &visitor) const override;

		SyntaxNode *GetParameter(size_t i) { return (i < _segments.size()) ? _segments[i].get() : nullptr; }
    };

    class ReturnStatement : public SyntaxNode, public OneStatementNode
    {
        DECLARE_NODE_TYPE(NodeTypeReturn)
    public:
        ReturnStatement() : OneStatementNode() {}

		ReturnStatement(const ReturnStatement &src) = delete;
		ReturnStatement& operator=(const ReturnStatement& src) = delete;

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context) { if (_statement1) _statement1->PreScan(context); }
        void Traverse(IExploreNode &en);
        

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        const SyntaxNode *GetValue() const { return _statement1.get(); }

    };

    class ForLoop : public SyntaxNode, public StatementsNode, public ConditionNode, public CodeBlockNode
    {
        DECLARE_NODE_TYPE(NodeTypeForLoop)
    public:
        ForLoop() : StatementsNode(), ConditionNode(), CodeBlockNode() {};
        ForLoop(const ForLoop &src) = delete;
		ForLoop& operator=(const ForLoop& src) = delete;
        
        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context); 
        void Traverse(IExploreNode &en);
        
        void SetLooper(std::unique_ptr<CodeBlock> block) { _looper = std::move(block); }
		CodeBlock *GetInitializer() const { return _list.get(); }

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        std::unique_ptr<CodeBlock> _looper;
    private:
    };

    class WhileLoop : public SyntaxNode, public StatementsNode, public ConditionNode
    {
        DECLARE_NODE_TYPE(NodeTypeWhileLoop)
    public:
		WhileLoop() {}
        WhileLoop(const WhileLoop &src) = delete;
		WhileLoop& operator=(const WhileLoop& src) = delete;

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en);
        

        void Accept(ISyntaxNodeVisitor &visitor) const override;

    };

    class DoLoop : public SyntaxNode, public ConditionNode, public StatementsNode
    {
        DECLARE_NODE_TYPE(NodeTypeDoLoop)
    public:
		DoLoop() {}
        DoLoop(const DoLoop &src) = delete;
		DoLoop& operator=(const DoLoop& src) = delete;

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en);
        

        void Accept(ISyntaxNodeVisitor &visitor) const override;

    };

    class BreakStatement : public SyntaxNode
    {
        DECLARE_NODE_TYPE(NodeTypeBreak)
    public:
        BreakStatement() : Levels(1) {}

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void Traverse(IExploreNode &en);

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        uint16_t Levels;
    };

    class ContinueStatement : public SyntaxNode
    {
        DECLARE_NODE_TYPE(NodeTypeContinue)
    public:
        ContinueStatement() : Levels(1) {}
        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void Traverse(IExploreNode &en);

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        uint16_t Levels;
    };

    class CaseStatementBase : public SyntaxNode, public StatementsNode, public OneStatementNode
    {
    public:
        CaseStatementBase() { _fDefault = false; }

        const SyntaxNode *GetCaseValue() const { return _statement1.get(); }
        void SetCaseValue(std::unique_ptr<SyntaxNode> caseValue) { SetStatement1(std::move(caseValue)); }
        SyntaxNode *GetCaseValue() { return _statement1.get(); }
        const SyntaxNodeVector &GetCodeSegments() const { return _segments; }
		SyntaxNodeVector &GetCodeSegments() { return _segments; }
        bool IsDefault() const { return _fDefault; }

        // IOutputByteCode
        void Traverse(IExploreNode &en);
        void PreScan(CompileContext &context);

        void SetDefault(bool fDefault) { _fDefault = fDefault; }
    private:
        CaseStatementBase(const CaseStatementBase &src) = delete;
        CaseStatementBase& operator=(const CaseStatementBase& src) = delete;

        bool _fDefault;
    };

    class CaseStatement : public CaseStatementBase
    {
        DECLARE_NODE_TYPE(NodeTypeCase)

    public:
        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;

        void Accept(ISyntaxNodeVisitor &visitor) const override;
    };

    class SwitchStatement : public SyntaxNode, public OneStatementNode
    {
        DECLARE_NODE_TYPE(NodeTypeSwitch)
    public:
        SwitchStatement() : OneStatementNode() {}

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en);

        
		void AddCase(std::unique_ptr<CaseStatement> pCase) { _cases.push_back(std::move(pCase)); }

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        std::vector<std::unique_ptr<CaseStatement>> _cases;

    private:
		SwitchStatement(const SwitchStatement &src) = delete;
		SwitchStatement& operator=(const SwitchStatement& src) = delete;

    };


    // CondStatement is a special case. cond's just degenerate to if's. But during parsing,
    // it's easier to treat them similar to switch statements. In fact, the parsing is almost identical.
    // In script post-processing, we'll convert the internal like of CaseStatements into an if
    // structure.
    class CondStatement : public SyntaxNode, public OneStatementNode
    {
        DECLARE_NODE_TYPE(NodeTypeCond)
    public:
        CondStatement() {}

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en);

        void AddCase(std::unique_ptr<CaseStatement> pCase) { _clausesTemp.push_back(std::move(pCase)); }

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        std::vector<std::unique_ptr<CaseStatement>> _clausesTemp;
        // The if goes in the OneStatementNode

    private:
        CondStatement(const CondStatement &src) = delete;
        CondStatement& operator=(const CondStatement& src) = delete;
    };


    //
    // Assignment statement (e.g. += foo 1)
    //
    class Assignment : public SyntaxNode, public OneStatementNode
    {
        DECLARE_NODE_TYPE(NodeTypeAssignment)
    public:
        Assignment() : OneStatementNode(), Operator(AssignmentOperator::None) {}

		Assignment(const Assignment &src) = delete;
		Assignment& operator=(const Assignment& src) = delete;

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en);

        void SetVariable(std::unique_ptr<LValue> var) { _variable = std::move(var); }

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        std::unique_ptr<LValue> _variable;

        AssignmentOperator Operator;
    private:
    };

    //
    // A binary operation (e.g. + x 6)
    //
    class BinaryOp : public SyntaxNode, public OneStatementNode, public TwoStatementNode
    {
        DECLARE_NODE_TYPE(NodeTypeBinaryOperation)
    public:
        BinaryOp() : Operator(BinaryOperator::None) {}
        BinaryOp(BinaryOperator op) : Operator(op) {}

		BinaryOp(const BinaryOp &src) = delete;
		BinaryOp& operator=(const BinaryOp& src) = delete;

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const override;
        void PreScan(CompileContext &context)  override { _statement1->PreScan(context); _statement2->PreScan(context); }
        void Traverse(IExploreNode &en) override;
        bool Evaluate(ILookupDefine &context, uint16_t &result, CompileContext *reportError) const override;
        virtual std::string ToString() const;

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        BinaryOperator Operator;

    private:
        CodeResult _OutputByteCodeAnd(CompileContext &context) const;
        CodeResult _OutputByteCodeOr(CompileContext &context) const;
    };

    // N-ary operation.
    // (+ 1 2 3 4 5)    -> This is actually converted to a BinaryOperation before compilation.
    // (< 1 2 3 4 5)    -> This is kept as an N-ary operation, as there is no easy conversion to binary.
    class NaryOp : public SyntaxNode, public StatementsNode
    {
        DECLARE_NODE_TYPE(NodeTypeNaryOperation)
    public:
        NaryOp() : Operator(BinaryOperator::None) {}

        NaryOp(const BinaryOp &src) = delete;
        NaryOp& operator=(const BinaryOp& src) = delete;

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const override;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en) override;
        bool Evaluate(ILookupDefine &context, uint16_t &result, CompileContext *reportError) const override;

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        BinaryOperator Operator;
    };

    //
    // A unary operation (e.g. ++x)
    //
    class UnaryOp : public SyntaxNode, public OneStatementNode
    {
        DECLARE_NODE_TYPE(NodeTypeUnaryOperation)
    public:
        UnaryOp() : OneStatementNode(), Operator(UnaryOperator::None) {}

        UnaryOp(const UnaryOp &src) = delete;
        UnaryOp& operator=(const UnaryOp& src) = delete;

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context) { _statement1->PreScan(context); }
        void Traverse(IExploreNode &en);
        
        virtual std::string ToString() const;

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        UnaryOperator Operator;
    };

    //
    // If statement (SCI only)
    //
    class IfStatement : public SyntaxNode, public ConditionNode, public OneStatementNode, public TwoStatementNode
    {
        DECLARE_NODE_TYPE(NodeTypeIf)
    public:
        IfStatement() : OneStatementNode(), TwoStatementNode(), ConditionNode(), _fTernary(false) {}

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context) ;
        void Traverse(IExploreNode &en);
        

        // Causes output to be of the form _condition ? _if : _else
        void MakeTernary() { _fTernary = true; }
        bool HasElse() const { return !!_statement2; }

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        bool _fTernary;             // Currently for decompilation only - never used for compilation.

    private:
		IfStatement(const IfStatement &src) = delete;
		IfStatement& operator=(const IfStatement& src) = delete;
    };

    //
    // Series of code segments, surrounded by ()
    //
    class AsmBlock : public SyntaxNode, public StatementsNode
    {
        DECLARE_NODE_TYPE(NodeTypeAsmBlock)
    public:
        AsmBlock() {}
        AsmBlock(const CodeBlock &src) = delete;
        AsmBlock& operator=(const CodeBlock& src) = delete;

        const SyntaxNodeVector &GetList() const { return _segments; }

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en);

        void Accept(ISyntaxNodeVisitor &visitor) const override;
    };

    // This can be used to substitute or inject new nodes into a tree temporarily.
    // Each node tends to have unique ownership of its children, so normally this
    // can't be done.
    class WeakSyntaxNode : public SyntaxNode
    {
        DECLARE_NODE_TYPE(NodeTypeWeak)
    public:
        WeakSyntaxNode() : WeakNode(nullptr) {}
        WeakSyntaxNode(const SyntaxNode *node) : WeakNode(node) {}

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en);

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        const SyntaxNode *WeakNode;
    };

    // Label: optional label
    // Name: instruction
    // Statements: arguments
    class Asm : public SyntaxNode, public NamedNode, public StatementsNode
    {
        DECLARE_NODE_TYPE(NodeTypeAsm)
    public:
        Asm() : NamedNode(), StatementsNode() {}

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en);

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        const std::string &GetLabel() const { return _label; }
        void SetLabel(const std::string &label) {
            _label = label;
        }
        
    private:
        std::string _label;

        Asm(const Asm &src) = delete;
        Asm& operator=(const Asm &src) = delete;

    };

    // classdefs in the classdef file
    class ClassDefDeclaration : public SyntaxNode, public NamedNode
    {
        DECLARE_NODE_TYPE(NodeTypeClassDefDeclaration)
    public:
        ClassDefDeclaration() : NamedNode(), ScriptNumber(0xffff), ClassNumber(0xffff), SuperNumber(0xffff) {}

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const { return 0; }
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en);

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        uint16_t ScriptNumber;  // script#
        uint16_t ClassNumber;   // class#
        uint16_t SuperNumber;   // super#
        std::string File;       // file#
        std::vector<std::pair<std::string, uint16_t>> Properties;
        std::vector<std::string> Methods;

    private:

        ClassDefDeclaration(const ClassDefDeclaration &src) = delete;
        ClassDefDeclaration& operator=(const ClassDefDeclaration &src) = delete;
    };

    class SelectorDeclaration : public SyntaxNode, public NamedNode
    {
        DECLARE_NODE_TYPE(NodeTypeSelector)
    public:
        SelectorDeclaration() : NamedNode(), Index(0xffff) {}

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const { return 0; }
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en);
        void Accept(ISyntaxNodeVisitor &visitor) const override;

        uint16_t Index;
    };

    class GlobalDeclaration : public SyntaxNode
    {
        DECLARE_NODE_TYPE(NodeTypeGlobal)
    public:
        GlobalDeclaration() : Index(0xffff) {}

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const { return 0; }
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en);
        void Accept(ISyntaxNodeVisitor &visitor) const override;

        VariableDecl VarDecl;
        uint16_t Index;
    };

    class ExternDeclaration : public SyntaxNode, public NamedNode
    {
        DECLARE_NODE_TYPE(NodeTypeExtern)
    public:
        ExternDeclaration() : NamedNode(), Index(0xffff) {}

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const { return 0; }
        void PreScan(CompileContext &context);
        void Traverse(IExploreNode &en);
        void Accept(ISyntaxNodeVisitor &visitor) const override;

        PropertyValue ScriptNumber;
        uint16_t Index;
    };

}; //namespace sci

void ConvertAndOrSequenceIntoTree(sci::SyntaxNodeVector &statements, std::vector<bool> andOrs);