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
#include "scii.h"
#include "SortedVector.h"

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
    CommonLatch,
    FakeBreak,
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

struct ControlFlowNode;
struct RawCodeNode;
struct LoopNode;
struct SwitchNode;
struct CaseNode;
struct IfNode;
struct CompoundConditionNode;
struct InvertNode;
struct MainNode;

class DominatorMap;

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
    virtual void Visit(const MainNode &mainNode) = 0;
    virtual void Visit(const ControlFlowNode &node) = 0;
};

struct CompareCFGNodesByAddress {
    bool operator() (const ControlFlowNode* lhs, const ControlFlowNode* rhs) const;
};

struct ControlFlowNode;

// Using a sorted vector (instead of a set) provides about a 10x speed up in our scenario.
// It comes with a few disadvantages, like iterators are invalidated when items are added/removed
// (which may be a good thing). It's possible we still have bugs regarding this.
#define USE_VECTOR_NODESET 1

#ifdef USE_VECTOR_NODESET
struct NodeSet : public sorted_vector<ControlFlowNode*>
{
    bool contains(ControlFlowNode* node) const
    {
        return find(node) != end();
    }
};
#else
struct NodeSet : public std::set < ControlFlowNode* >
{
    bool contains(ControlFlowNode* node) const
    {
        return find(node) != end();
    }
};
#endif


struct ControlFlowNode
{
    ControlFlowNode(CFGNodeType type, std::initializer_list<SemId> semanticChildrenIds) : Type(type), ArbitraryDebugIndex(0), dirty(true), postDirty(true)
    {
        // Everyone has a head (for now)
        semanticChildren[SemId::Head] = nullptr;

        for (auto semId : semanticChildrenIds)
        {
            semanticChildren[semId] = nullptr;
        }
    }

    virtual ~ControlFlowNode() {}

    uint16_t GetStartingAddress() const;

    bool contains(ControlFlowNode *potentialChild)
    {
        return children.contains(potentialChild);
    }

    virtual bool contains(Opcode opcode) { return false; }
    virtual bool startsWith(Opcode opcode) { return false; }
    virtual bool endsWith(Opcode opcode) { return false; }
    virtual scii getLastInstruction() { assert(false); return scii(sciVersion0, Opcode::INDETERMINATE); }

    int ArbitraryDebugIndex;
    std::string DebugId;

    ControlFlowNode(const ControlFlowNode &node) = delete;
    ControlFlowNode &operator=(const ControlFlowNode &node) = delete;

    bool ContainsTag(SemanticTags tag) const { return Tags.find(tag) != Tags.end(); }
    std::set<SemanticTags> Tags;

    std::map<SemId, ControlFlowNode*> semanticChildren;

    ControlFlowNode *&operator[](SemId semId)
    {
        assert(semanticChildren.find(semId) != semanticChildren.end());
        return semanticChildren[semId];
    }

    ControlFlowNode *operator[](SemId semId) const
    {
        return semanticChildren.at(semId);
    }

    ControlFlowNode *MaybeGet(SemId semId) const
    {
        ControlFlowNode *child = nullptr;
        if (semanticChildren.find(semId) != semanticChildren.end())
        {
            child = semanticChildren.at(semId);
        }
        return child;
    }

    void NotifyChildrenReplacedBy(ControlFlowNode *newNode)
    {
        for (auto &pair : semanticChildren)
        {
            if (pair.first == SemId::Follow)
            {
                // Assert it is never part of our children.
                assert(!newNode->children.contains(pair.second));
                assert(!children.contains(pair.second));
            }

            if (pair.second && (newNode->children.contains(pair.second)))
            {
                pair.second = newNode;
            }
        }
    }

    CFGNodeType Type;

    const NodeSet &Predecessors() { return predecessors; }
    const NodeSet &Successors() { return successors; }

    void InsertPredecessor(ControlFlowNode *node)
    {
        dirty = true;
        postDirty = true;
        if (predecessors.insert(node).second)
        {
            // If true, it was added anew
            node->successors.insert(this);
        }
    }
    void ClearPredecessors()
    {
        dirty = true;
        postDirty = true;
        NodeSet copy = predecessors;
        for (ControlFlowNode *node : copy)
        {
            ErasePredecessor(node);
        }
    }
    void ErasePredecessor(ControlFlowNode *node)
    {
        dirty = true;
        postDirty = true;
        predecessors.erase(node);
        node->successors.erase(this);
    }
    void AssignPredecessors(const NodeSet &newPredecessors)
    {
        dirty = true;
        postDirty = true;
        ClearPredecessors();
        for (ControlFlowNode *node : newPredecessors)
        {
            InsertPredecessor(node);
        }
    }

    // Visitor pattern for double-dispatch
    virtual void Accept(ICFGNodeVisitor &visitor) const = 0;

    void InsertChild(ControlFlowNode *node)
    {
        dirty = true;
        postDirty = true;
        children.insert(node);
    }
    void EraseChild(ControlFlowNode *node)
    {
        dirty = true;
        postDirty = true;
        children.erase(node);
    }
    const NodeSet &Children() const
    {
        return children;
    }
    void SetChildren(const NodeSet &newChildren)
    {
        dirty = true;
        postDirty = true;
        children = newChildren;
    }

    std::unique_ptr<DominatorMap> dominators;
    std::unique_ptr<DominatorMap> postDominators;
    bool dirty;
    bool postDirty;

private:

    // Only valid for structured nodes, but so common that we'll just keep
    // it in the base class for now.
    NodeSet children;

    NodeSet successors;
    NodeSet predecessors;
};

class ControlFlowException : public std::exception
{
public:
    ControlFlowException(ControlFlowNode *node, const std::string &message) : message(message), node(node) {}

    ControlFlowNode *node;
    std::string message;
};

struct ExitNode : public ControlFlowNode
{
    ExitNode() : ControlFlowNode(CFGNodeType::Exit, {}), startingAddressForExit(0xffff) {}
    void Accept(ICFGNodeVisitor &visitor) const { visitor.Visit(*this); }

    uint16_t startingAddressForExit;
};

struct StructuredNode : public ControlFlowNode
{
    StructuredNode(CFGNodeType type, std::initializer_list<SemId> semanticChildren) : ControlFlowNode(type, semanticChildren) {}
    // has children...
};

enum class ConditionType { And, Or };

struct CompoundConditionNode : public StructuredNode
{
    CompoundConditionNode(ConditionType condition) : condition(condition), thenBranch(0xffff), isFirstTermNegated(false), StructuredNode(CFGNodeType::CompoundCondition, { SemId::First, SemId::Second }) {}
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

struct CommonLatchNode : public ControlFlowNode
{
    CommonLatchNode() : ControlFlowNode(CFGNodeType::CommonLatch, {}), tokenStartingAddress(0) {}
    void Accept(ICFGNodeVisitor &visitor) const { visitor.Visit(*this); }

    uint16_t tokenStartingAddress;  // Don't use this for anything real?
};

struct FakeBreakNode : public ControlFlowNode
{
    FakeBreakNode() : ControlFlowNode(CFGNodeType::FakeBreak, {}) {}
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
struct RawCodeNode : public ControlFlowNode
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

void GetThenAndElseBranches(ControlFlowNode *node, ControlFlowNode **thenNode, ControlFlowNode **elseNode);
bool MaybeGetThenAndElseBranches(ControlFlowNode *node, ControlFlowNode **thenNode, ControlFlowNode **elseNode);
ControlFlowNode *GetOtherBranch(ControlFlowNode *branchNode, ControlFlowNode *branch1);
bool IsThenBranch(ControlFlowNode *branchNode, ControlFlowNode *target);

class DominatorMap : public std::map < ControlFlowNode*, NodeSet >
{
public:
    bool IsADominatedByB(ControlFlowNode *a, ControlFlowNode *b) const
    {
        return at(a).contains(b);
    }
};

ControlFlowNode *GetFirstSuccessorOrNull(ControlFlowNode *node);