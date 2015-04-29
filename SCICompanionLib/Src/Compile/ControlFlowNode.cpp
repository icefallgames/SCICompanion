#include "stdafx.h"
#include "ControlFlowNode.h"
#include "format.h"
#include "StlUtil.h"

using namespace std;

// Safe to call on anything. If it doesn't apply, it returns null.
ControlFlowNode *GetOtherBranch(ControlFlowNode *branchNode, ControlFlowNode *branch1)
{
    ControlFlowNode *other = nullptr;
    if (branchNode->Successors().size() == 2)
    {
        ControlFlowNode *thenNode, *elseNode;
        GetThenAndElseBranches(branchNode, &thenNode, &elseNode);
        if (thenNode == branch1)
        {
            other = elseNode;
        }
        else if (elseNode == branch1)
        {
            other = thenNode;
        }
        else
        {
            assert(false && "branch1 is not a successor of branchNode");
        }
    }
    return other;
}

bool IsThenBranch(ControlFlowNode *branchNode, ControlFlowNode *target)
{
    ControlFlowNode *thenNode, *elseNode;
    GetThenAndElseBranches(branchNode, &thenNode, &elseNode);
    return (target->GetStartingAddress() == thenNode->GetStartingAddress());
}

uint16_t ControlFlowNode::GetStartingAddress() const
{
    uint16_t address = 0xffff;
    if (Type == CFGNodeType::RawCode)
    {
        address = (static_cast<const RawCodeNode*>(this))->start->get_final_offset_dontcare();
    }
    else if (Type == CFGNodeType::Exit)
    {
        address = (static_cast<const ExitNode*>(this))->startingAddressForExit;
    }
    else if (Type == CFGNodeType::CommonLatch)
    {
        address = (static_cast<const CommonLatchNode*>(this))->tokenStartingAddress;
    }
    else
    {
        assert((*this)[SemId::Head]);
        address = (*this)[SemId::Head]->GetStartingAddress();
    }
    return address;
}

ControlFlowNode *AdvanceToExit(ControlFlowNode *node)
{
    while (node && node->Type != CFGNodeType::Exit)
    {
        node = GetFirstSuccessorOrNull(node);
    }
    return node;
}

// Given a node that represents a raw code branch or a compound condition, returns
// which of its two successor nodes is the "then"  branch, and which is the "else" 
void GetThenAndElseBranches(ControlFlowNode *node, ControlFlowNode **thenNode, ControlFlowNode **elseNode)
{
    *thenNode = nullptr;
    *elseNode = nullptr;
    assert((node->Successors().size() == 2) && "Asking for Then/Else branches on a node without two Successors()");
    auto it = node->Successors().begin();
    ControlFlowNode *one = *it;
    ControlFlowNode *two = *(++it);

    if (node->Type == CFGNodeType::CompoundCondition)
    {
        CompoundConditionNode *ccNode = static_cast<CompoundConditionNode *>(node);
        uint16_t trueAddress = ccNode->thenBranch;
        if (trueAddress == one->GetStartingAddress())
        {
            *elseNode = two;
            *thenNode = one;
        }
        else
        {
            assert(trueAddress == two->GetStartingAddress());
            *elseNode = one;
            *thenNode = two;
        }
    }
    else if (node->Type == CFGNodeType::RawCode)
    {
        scii lastInstruction = node->getLastInstruction();
        if (lastInstruction.get_opcode() == Opcode::BNT)
        {
            uint16_t target = lastInstruction.get_branch_target()->get_final_offset();
            if (target == one->GetStartingAddress())
            {
                *elseNode = one;
                *thenNode = two;
            }
            else
            {
                if (target != two->GetStartingAddress())
                {
                    throw ControlFlowException(node, "Inconsistent then/else branches. Possible \"continue\" statement?");
                }
                *elseNode = two;
                *thenNode = one;
            }
        }
        else if (lastInstruction.get_opcode() == Opcode::BT)
        {
            uint16_t target = lastInstruction.get_branch_target()->get_final_offset();
            if (target == one->GetStartingAddress())
            {
                *elseNode = two;
                *thenNode = one;
            }
            else
            {
                assert(target == two->GetStartingAddress());
                *elseNode = one;
                *thenNode = two;
            }
        }
    }
    else if (node->Type == CFGNodeType::Invert)
    {
        ControlFlowNode *invThen, *invElse;
        GetThenAndElseBranches((*node)[SemId::Head], &invThen, &invElse);
        // Find the exit nodes
        invThen = AdvanceToExit(invThen);
        invElse = AdvanceToExit(invElse);

        if (invThen->GetStartingAddress() == one->GetStartingAddress())
        {
            assert(invElse->GetStartingAddress() == two->GetStartingAddress());
            *thenNode = two;
            *elseNode = one;
        }
        else
        {
            assert(invElse->GetStartingAddress() == one->GetStartingAddress());
            assert(invThen->GetStartingAddress() == two->GetStartingAddress());
            *thenNode = one;
            *elseNode = two;
        }
    }
}

RawCodeNode::RawCodeNode(code_pos start) : ControlFlowNode(CFGNodeType::RawCode, {}), start(start)
{
    DebugId = fmt::format("{:04x}:{}", start->get_final_offset_dontcare(), OpcodeNames[static_cast<BYTE>(start->get_opcode())]);
}

bool CompareCFGNodesByAddress::operator() (const ControlFlowNode* lhs, const ControlFlowNode* rhs) const
{
    uint16_t lAddress = lhs->GetStartingAddress();
    uint16_t rAddress = rhs->GetStartingAddress();
    if (lAddress == rAddress)
    {
        return lhs < rhs;
    }
    else
    {
        return lAddress < rAddress;
    }
}
