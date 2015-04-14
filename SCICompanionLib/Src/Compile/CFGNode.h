#pragma once
#include "scii.h"

enum CFGNodeType
{
    Main,
    RawCode,
    Loop,
    Switch,
    Case,
    Exit,
    CompoundCondition,
    If,
    Invert,
};

// Ways to classify certain nodes
enum class SemId
{
    Head,
    Tail,
    Follow, // This may not be a good idea, since the follow node is by definition not in the children.
    Latch,
    First,
    Second,
    Then,
    Else
};

enum class SemanticTags
{
    LoopBreak,
    DefaultCase,
    CaseCondition,
};

struct CFGNode;
struct RawCodeNode;
struct LoopNode;
struct SwitchNode;
struct CaseNode;
struct IfNode;
struct CompoundConditionNode;
struct InvertNode;
struct MainNode;

class ICFGNodeVisitor
{
public:
    virtual void Visit(const RawCodeNode &rawCodeNode) = 0;
    virtual void Visit(const LoopNode &loopNode) = 0;
    virtual void Visit(const SwitchNode &switchNode) = 0;
    virtual void Visit(const CaseNode &caseNode) = 0;
    virtual void Visit(const CompoundConditionNode &conditionNode) = 0;
    virtual void Visit(const InvertNode &invert) = 0;
    virtual void Visit(const IfNode &ifNode) = 0;
    virtual void Visit(const MainNode &ifNode) = 0;
    virtual void Visit(const CFGNode &node) = 0;
};

struct CompareCFGNodesByAddress {
    bool operator() (const CFGNode* lhs, const CFGNode* rhs) const;
};

struct CFGNode;
typedef std::set<CFGNode*> NodeSet;

struct CFGNode
{
    CFGNode(CFGNodeType type, std::initializer_list<SemId> semanticChildrenIds) : Type(type), ArbitraryDebugIndex(0)
    {
        // Everyone has a head (for now)
        semanticChildren[SemId::Head] = nullptr;

        for (auto semId : semanticChildrenIds)
        {
            semanticChildren[semId] = nullptr;
        }
    }

    virtual ~CFGNode() {}

    uint16_t GetStartingAddress() const;

    bool contains(CFGNode *potentialChild)
    {
        return children.find(potentialChild) != children.end();
    }

    virtual bool contains(Opcode opcode) { return false; }
    virtual bool startsWith(Opcode opcode) { return false; }
    virtual bool endsWith(Opcode opcode) { return false; }
    virtual scii getLastInstruction() { assert(false); return scii(Opcode::INDETERMINATE); }

    int ArbitraryDebugIndex;
    std::string DebugId;

    CFGNode(const CFGNode &node) = delete;
    CFGNode &operator=(const CFGNode &node) = delete;

    bool ContainsTag(SemanticTags tag) { return Tags.find(tag) != Tags.end(); }
    std::set<SemanticTags> Tags;

    // Only valid for structured nodes, but so common that we'll just keep
    // it in the base class for now.
    NodeSet children;
    std::map<SemId, CFGNode*> semanticChildren;

    CFGNode *&operator[](SemId semId)
    {
        assert(semanticChildren.find(semId) != semanticChildren.end());
        return semanticChildren[semId];
    }

    CFGNode *operator[](SemId semId) const
    {
        return semanticChildren.at(semId);
    }

    CFGNode *MaybeGet(SemId semId) const
    {
        CFGNode *child = nullptr;
        if (semanticChildren.find(semId) != semanticChildren.end())
        {
            child = semanticChildren.at(semId);
        }
        return child;
    }

    void NotifyChildrenReplacedBy(CFGNode *newNode)
    {
        for (auto &pair : semanticChildren)
        {
            if (pair.first == SemId::Follow)
            {
                // Assert it is never part of our children.
                assert(newNode->children.find(pair.second) == newNode->children.end());
                assert(children.find(pair.second) == children.end());
            }

            if (pair.second && (newNode->children.find(pair.second) != newNode->children.end()))
            {
                pair.second = newNode;
            }
        }
    }

    CFGNodeType Type;

    const NodeSet &Predecessors() { return predecessors; }
    const NodeSet &Successors() { return successors; }

    void InsertPredecessor(CFGNode *node)
    {
        if (predecessors.insert(node).second)
        {
            // If true, it was added anew
            node->successors.insert(this);
        }
    }
    void ClearPredecessors()
    {
        NodeSet copy = predecessors;
        for (CFGNode *node : copy)
        {
            ErasePredecessor(node);
        }
    }
    void ErasePredecessor(CFGNode *node)
    {
        predecessors.erase(node);
        node->successors.erase(this);
    }
    void AssignPredecessors(const NodeSet &newPredecessors)
    {
        ClearPredecessors();
        for (CFGNode *node : newPredecessors)
        {
            InsertPredecessor(node);
        }
    }

    // Visitor pattern for double-dispatch
    virtual void Accept(ICFGNodeVisitor &visitor) const = 0;


private:
    NodeSet successors;
    NodeSet predecessors;
};


struct ExitNode : public CFGNode
{
    ExitNode() : CFGNode(CFGNodeType::Exit, {}), startingAddressForExit(0xffff) {}
    void Accept(ICFGNodeVisitor &visitor) const { visitor.Visit(*this); }

    uint16_t startingAddressForExit;
};

struct StructuredNode : public CFGNode
{
    StructuredNode(CFGNodeType type, std::initializer_list<SemId> semanticChildren) : CFGNode(type, semanticChildren) {}
    // has children...
};

enum class ConditionType { And, Or };

struct CompoundConditionNode : public StructuredNode
{
    CompoundConditionNode(ConditionType condition) : condition(condition), StructuredNode(CFGNodeType::CompoundCondition, { SemId::First, SemId::Second }) {}
    void Accept(ICFGNodeVisitor &visitor) const { visitor.Visit(*this); }

    uint16_t thenBranch;
    bool isFirstTermNegated;
    // First term is head, second term is tail.

    ConditionType condition;
};

struct InvertNode : public StructuredNode
{
    InvertNode() : StructuredNode(CFGNodeType::Invert, { SemId::Tail }) {}
    void Accept(ICFGNodeVisitor &visitor) const { visitor.Visit(*this); }
};

struct IfNode : public StructuredNode
{
    IfNode() : StructuredNode(CFGNodeType::If, { SemId::Then, SemId::Else, SemId::Follow }) {}
    void Accept(ICFGNodeVisitor &visitor) const { visitor.Visit(*this); }
};
struct LoopNode : public StructuredNode
{
    LoopNode() : StructuredNode(CFGNodeType::Loop, { SemId::Latch, SemId::Follow, SemId::Tail }) {}
    void Accept(ICFGNodeVisitor &visitor) const { visitor.Visit(*this); }
};
struct CaseNode : public StructuredNode
{
    CaseNode() : StructuredNode(CFGNodeType::Case, { SemId::Tail }) {}
    void Accept(ICFGNodeVisitor &visitor) const { visitor.Visit(*this); }
};
struct SwitchNode : public StructuredNode
{
    SwitchNode() : StructuredNode(CFGNodeType::Switch, { SemId::Tail }) {}
    void Accept(ICFGNodeVisitor &visitor) const { visitor.Visit(*this); }
};
struct MainNode : public StructuredNode
{
    MainNode() : StructuredNode(CFGNodeType::Main, { SemId::Tail, SemId::Follow }) {}
    void Accept(ICFGNodeVisitor &visitor) const { visitor.Visit(*this); }
};
struct RawCodeNode : public CFGNode
{
    RawCodeNode(code_pos start);
    void Accept(ICFGNodeVisitor &visitor) const { visitor.Visit(*this); }

    bool contains(Opcode opcode) override
    {
        return (find_if(start, end,
            [opcode](scii &inst) { return inst.get_opcode() == opcode; }) != end);
    }

    bool startsWith(Opcode opcode) override
    {
        return start->get_opcode() == opcode;
    }

    bool endsWith(Opcode opcode) override
    {
        code_pos lastInstruction = end;
        --lastInstruction;
        return (lastInstruction->get_opcode() == opcode);
    }

    scii getLastInstruction() override
    {
        code_pos lastInstruction = end;
        return *(--lastInstruction);
    }

    code_pos start;
    code_pos end;
};

void GetThenAndElseBranches(CFGNode *node, CFGNode **thenNode, CFGNode **elseNode);
CFGNode *GetOtherBranch(CFGNode *branchNode, CFGNode *branch1);

class DominatorMap : public std::map < CFGNode*, NodeSet >
{
public:
    bool IsADominatedByB(CFGNode *a, CFGNode *b) const
    {
        return at(a).find(b) != at(a).end();
    }
};
