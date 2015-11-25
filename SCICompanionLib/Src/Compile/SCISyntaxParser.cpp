#include "stdafx.h"
#include "ScriptOMAll.h"

using namespace sci;

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
