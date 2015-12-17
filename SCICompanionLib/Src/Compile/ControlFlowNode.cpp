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
#include "ControlFlowNode.h"
#include "format.h"
#include "StlUtil.h"
#include "PMachine.h"

using namespace std;

scii ControlFlowNode::getLastInstruction() { assert(false); return scii(sciVersion0, Opcode::INDETERMINATE); }

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
    else if (Type == CFGNodeType::FakeBreak)
    {
        address = 0;
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

bool MaybeGetThenAndElseBranches(ControlFlowNode *node, ControlFlowNode **thenNode, ControlFlowNode **elseNode)
{
    *thenNode = nullptr;
    *elseNode = nullptr;
    if (node->Successors().size() == 2)
    {
        GetThenAndElseBranches(node, thenNode, elseNode);
        return (thenNode && elseNode);
    }
    return false;
}

// Given a node that represents a raw code branch or a compound condition, returns
// which of its two successor nodes is the "then"  branch, and which is the "else" 
void GetThenAndElseBranches(ControlFlowNode *node, ControlFlowNode **thenNode, ControlFlowNode **elseNode)
{
    *thenNode = nullptr;
    *elseNode = nullptr;
    if (node->Successors().size() != 2)
    {
        throw ControlFlowException(node, "Expected node with two successors (then/else)");
    }
    
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

        // This didn't seem to work. i.e. the negation didn't work. CueObj is a good starting point.
        if (((*thenNode)->Type == CFGNodeType::Exit) && ((*elseNode)->Type != CFGNodeType::Exit))
        {
            // Then is an exit node, but else is not. That's a problem for things like while loops. Swap them:
            std::swap(*thenNode, *elseNode);
            ccNode->thenBranch = (*thenNode)->GetStartingAddress();
            // And then invert the condition by changing the operation and negating the terms (DeMorgan's law)
            ccNode->condition = (ccNode->condition == ConditionType::And) ? ConditionType::Or : ConditionType::And;
            // And negate the terms
            ccNode->isFirstTermNegated = !ccNode->isFirstTermNegated;
            ccNode->isSecondTermNegated = !ccNode->isSecondTermNegated;
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

RawCodeNode::RawCodeNode(code_pos start) : ControlFlowNode(nullptr, CFGNodeType::RawCode, {}), start(start)
{
    DebugId = fmt::format("{:04x}:{}", start->get_final_offset_dontcare(), OpcodeToName(start->get_opcode()));
}


bool std::less<ControlFlowNode*>::operator() (const ControlFlowNode* lhs, const ControlFlowNode* rhs) const
{
    uint16_t lAddress = lhs->GetStartingAddress();
    uint16_t rAddress = rhs->GetStartingAddress();
    if (lAddress == rAddress)
    {
        if (lhs->Type == rhs->Type)
        {
            return lhs < rhs;
        }
        else
        {
            return lhs->Type < rhs->Type;
        }
    }
    else
    {
        return lAddress < rAddress;
    }
}
