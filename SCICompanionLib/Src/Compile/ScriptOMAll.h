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

//
// This file contains less commonly used script objects that aren't
// directly referenced by any other common script objects
// (speeds up compile time for those that include "ScriptOM.h")
//

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
		const SingleStatementVector &GetSelectorParams() const { return _segments; }
        
        void SetIsMethod(bool fMethod) { _fIsMethodCall = fMethod; }
        bool IsMethod() const { return _fIsMethodCall; }

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);

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

        const SingleStatement *GetIndexer() const { return _indexer.get(); }
        bool HasIndexer() const { return _indexer.get() != nullptr; }

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);

        void SetIndexer(std::unique_ptr<SingleStatement> indexer) { _indexer = std::move(indexer); }
        
        void Accept(ISyntaxNodeVisitor &visitor) const override;

    private:
        std::unique_ptr<SingleStatement> _indexer;
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
        
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);
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
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);
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
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);

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
        void StealParams(SingleStatementVector &stolen) { stolen.swap(_segments); }

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);
        

        void Accept(ISyntaxNodeVisitor &visitor) const override;

		SingleStatementPtr GetParameter(size_t i) { return (i < _segments.size()) ? _segments[i].get() : nullptr; }
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
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);
        

        void Accept(ISyntaxNodeVisitor &visitor) const override;

		const SingleStatement *GetValue() const { return _statement1.get(); }

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
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);
        
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
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);
        

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
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);
        

        void Accept(ISyntaxNodeVisitor &visitor) const override;

    };

    class BreakStatement : public SyntaxNode
    {
        DECLARE_NODE_TYPE(NodeTypeBreak)
    public:
        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);

        void Accept(ISyntaxNodeVisitor &visitor) const override;

    };

    class CaseStatement : public SyntaxNode, public StatementsNode, public OneStatementNode
    {
        DECLARE_NODE_TYPE(NodeTypeCase)
    public:
        CaseStatement() { _fDefault = false; }

		const SingleStatement *GetCaseValue() const { return _statement1.get(); }
        SingleStatement *GetCaseValue() { return _statement1.get(); }
        const SingleStatementVector &GetCodeSegments() const { return _segments; }
		SingleStatementVector &GetCodeSegments() { return _segments; }
        bool IsDefault() const { return _fDefault; }

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);
        
        void SetDefault(bool fDefault) { _fDefault = fDefault; }

        void Accept(ISyntaxNodeVisitor &visitor) const override;


    private:
		CaseStatement(const CaseStatement &src) = delete;
		CaseStatement& operator=(const CaseStatement& src) = delete;

        bool _fDefault;
    };

    class SwitchStatement : public SyntaxNode, public OneStatementNode
    {
        DECLARE_NODE_TYPE(NodeTypeSwitch)
    public:
        SwitchStatement() : OneStatementNode() {}

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);

        
		void AddCase(std::unique_ptr<CaseStatement> pCase) { _cases.push_back(std::move(pCase)); }

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        std::vector<std::unique_ptr<CaseStatement>> _cases;

    private:
		SwitchStatement(const SwitchStatement &src) = delete;
		SwitchStatement& operator=(const SwitchStatement& src) = delete;

    };

    //
    // Assignment statement (e.g. += foo 1)
    //
    class Assignment : public SyntaxNode, public NamedNode, public OneStatementNode
    {
        DECLARE_NODE_TYPE(NodeTypeAssignment)
    public:
        Assignment() : NamedNode(), OneStatementNode() {}

		Assignment(const Assignment &src) = delete;
		Assignment& operator=(const Assignment& src) = delete;

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);

        void SetVariable(std::unique_ptr<LValue> var) { _variable = std::move(var); }
        

		const std::string GetAssignmentOp() const { return _innerName; }

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        std::unique_ptr<LValue> _variable;

    private:
    };

    //
    // A binary operation (e.g. + x 6)
    //
    class BinaryOp : public SyntaxNode, public NamedNode, public OneStatementNode, public TwoStatementNode
    {
        DECLARE_NODE_TYPE(NodeTypeBinaryOperation)
    public:
        BinaryOp() : NamedNode(), OneStatementNode(), TwoStatementNode() {}

		BinaryOp(const BinaryOp &src) = delete;
		BinaryOp& operator=(const BinaryOp& src) = delete;

		const std::string &GetOpName() const { return _innerName; }

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const override;
        void PreScan(CompileContext &context)  override { _statement1->PreScan(context); _statement2->PreScan(context); }
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en) override;
        bool Evaluate(CompileContext &context, uint16_t &result) override;
        virtual std::string ToString() const;

        void Accept(ISyntaxNodeVisitor &visitor) const override;

    private:
        CodeResult _OutputByteCodeAnd(CompileContext &context) const;
        CodeResult _OutputByteCodeOr(CompileContext &context) const;
    };

    //
    // A unary operation (e.g. ++x)
    //
    class UnaryOp : public SyntaxNode, public NamedNode, public OneStatementNode
    {
        DECLARE_NODE_TYPE(NodeTypeUnaryOperation)
    public:
        UnaryOp() : NamedNode(), OneStatementNode() {}

		UnaryOp(const UnaryOp &src) = delete;
		UnaryOp& operator=(const UnaryOp& src) = delete;

		const std::string &GetOpName() const { return _innerName; }

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context) { _statement1->PreScan(context); }
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);
        
        virtual std::string ToString() const;

        void Accept(ISyntaxNodeVisitor &visitor) const override;

    };

    //
    // If statement (SCI only)
    //
    class CppIfStatement : public SyntaxNode, public ConditionNode, public OneStatementNode, public TwoStatementNode
    {
        DECLARE_NODE_TYPE(NodeTypeCppIf)
    public:
        CppIfStatement() : OneStatementNode(), TwoStatementNode(), ConditionNode(), _fTernary(false) {}

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context) ;
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);
        

        // Causes output to be of the form _condition ? _if : _else
        void MakeTernary() { _fTernary = true; }
		bool HasElse() const { return _statement2.get() && _statement2->GetSyntaxNode(); }

        void Accept(ISyntaxNodeVisitor &visitor) const override;

        bool _fTernary;             // Currently for decompilation only - never used for compilation.

    private:
		CppIfStatement(const CppIfStatement &src) = delete;
		CppIfStatement& operator=(const CppIfStatement& src) = delete;
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

        const SingleStatementVector &GetList() const { return _segments; }

        // IOutputByteCode
        CodeResult OutputByteCode(CompileContext &context) const;
        void PreScan(CompileContext &context);
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);

        void Accept(ISyntaxNodeVisitor &visitor) const override;
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
        void Traverse(IExploreNodeContext *pContext, IExploreNode &en);

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

}; //namespace sci

void ConvertAndOrSequenceIntoTree(sci::SingleStatementVector &statements, std::vector<bool> andOrs);