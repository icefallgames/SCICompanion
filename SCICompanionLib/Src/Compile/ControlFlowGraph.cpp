#include "stdafx.h"
#include "scii.h"
#include "ControlFlowGraph.h"
#include "DecompilerCore.h"
#include "ControlFlowNode.h"
#include "TarjanAlgorithm.h"
#include "ControlFlowGraphViz.h"
#include "StlUtil.h"
#include "format.h"
#include "SortedVector.h"

using namespace sci;
using namespace std;

sorted_vector<ControlFlowNode*> fooTest;

bool EndsWithConditionalBranch(ControlFlowNode *node)
{
    return node->Type == CFGNodeType::CompoundCondition ||
        node->endsWith(Opcode::BNT) ||
        node->endsWith(Opcode::BT);
}

NodeSet _CollectNodesBetween(ControlFlowNode *head, ControlFlowNode *tail)
{
    NodeSet collection;
    collection.insert(head);

    stack<ControlFlowNode*> leStack;
    leStack.push(tail);
    while (!leStack.empty())
    {
        // I hate that it's two lines of code to pop from a std::stack
        ControlFlowNode *d = pop_ptr(leStack);
        if (!collection.contains(d))
        {
            collection.insert(d);
            // Add all predecessors of b, as long as they are not head.
            for (ControlFlowNode *pred : d->Predecessors())
            {
                if (pred != head)
                {
                    leStack.push(pred);
                }
            }
        }
    }
    return collection;
}

void _CollectMoreNodesByAddress(NodeSet &nodeSet, ControlFlowNode *head, ControlFlowNode *tail, ControlFlowNode *parentToGatherMore)
{
    // If we have an infinite loop with a break exit, it won't be detected by following the tail to the head.
    // Let's take advantage of the fact that SCI code is sequential, and find other nodes between tail and head address
    uint16_t tailAddress = tail->GetStartingAddress();
    uint16_t headAddress = head->GetStartingAddress();
    for (ControlFlowNode *child : parentToGatherMore->Children())
    {
        if (!nodeSet.contains(child))
        {
            uint16_t childStartingAddress = child->GetStartingAddress();
            if ((childStartingAddress > headAddress) && (childStartingAddress < tailAddress))
            {
                nodeSet.insert(child);
            }
        }
    }
}

// A back edge is smaller if it is contained within another.
NodeBlock::NodeBlock(ControlFlowNode *head, ControlFlowNode *followNode, bool includeFollow, ControlFlowNode *parentToGatherMore) : head(head), latch(followNode)
{
    body = _CollectNodesBetween(head, followNode);
    if (parentToGatherMore)
    {
        _CollectMoreNodesByAddress(body, head, followNode, parentToGatherMore);
    }
    if (!includeFollow)
    {
        body.erase(followNode);
    }
}

int NodeBlock::Compare(const NodeBlock &A, const NodeBlock &B)
{
    if (A.head != B.head)
    {
        // The node block children are not complete, due to the prescence of break statements.
        // So I can't use the below code (e.g. SRDialog::doit in the SCI0 template game)
        /*
        if (includes(A.body.begin(), A.body.end(), B.body.begin(), B.body.end()))
        {
            // B is nested in A
            return 1;
        }
        else if (includes(B.body.begin(), B.body.end(), A.body.begin(), A.body.end()))
        {
            // A is nested in B
            return -1;
        }*/

        // Instead, take advantage of the fact that SCI addresses are sequential.
        uint16_t aStart = A.head->GetStartingAddress();
        uint16_t aEnd = A.latch->GetStartingAddress();
        uint16_t bStart = B.head->GetStartingAddress();
        uint16_t bEnd = B.latch->GetStartingAddress();
        if ((bStart > aStart) && (bEnd <= aEnd))
        {
            // B is nested in A
            return 1;
        }
        else if ((aStart > bStart) && (aEnd <= bEnd))
        {
            // A is nested in B
            return -1;
        }

        // Assert that we don't have partial overlaps, because that's a structure we don't intend to handle.
        if (aStart > bStart)
        {
            assert(aStart > bEnd);
        }
        else if (bStart < aStart)
        {
            assert(bStart > aEnd);
        }
    }
    else
    {
        // The same NodeBlock.
        // If two loops share the same head, but a different tail, I don't know how to handle that.
        // http://www.cs.cmu.edu/afs/cs/academic/class/15745-s01/www/lectures/lect0124.txt
        // Inner Loops and Loops with the same header: http://nptel.ac.in/courses/106108052/module9/control-flow-ana-2.pdf
        // We basically insert a dummy node and combine them.
        // Hmm, this happens with do-whiles with compound conditions. But not the way SCIStudio or SCICompanion compiles them.
        assert(B.latch == A.latch);
        return 0;
    }
    // Disjoint, so they are equal
    return 0;
}

void RemoveSetFromSet(ControlFlowNode *parent, const NodeSet &subset)
{
    for (ControlFlowNode *node : subset)
    {
        parent->EraseChild(node);
    }
}

ControlFlowNode *ControlFlowGraph::_EnsureExitNode(NodeSet &existingExitNodes, ControlFlowNode *exitNodePredecessor, ControlFlowNode *exitNodeSuccessor)
{
    uint16_t exitAddress = exitNodeSuccessor->GetStartingAddress();
    ControlFlowNode *reuseExitNode = nullptr;
    for (ControlFlowNode *existingExitNode : existingExitNodes)
    {
        if (static_cast<ExitNode*>(existingExitNode)->startingAddressForExit == exitAddress)
        {
            reuseExitNode = existingExitNode;
            break;
        }
    }

    if (!reuseExitNode)
    {
        reuseExitNode = MakeNode<ExitNode>();
        static_cast<ExitNode*>(reuseExitNode)->startingAddressForExit = exitAddress;
        existingExitNodes.insert(reuseExitNode);
    }

    reuseExitNode->InsertPredecessor(exitNodePredecessor);
    return reuseExitNode;
}

    // This is different from _ReplaceNodeInWorkingSet mainly in that the if node has two exits.
ControlFlowNode *ControlFlowGraph::_ReplaceIfStatementInWorkingSet(ControlFlowNode *structure, ControlFlowNode *ifHeader, ControlFlowNode *ifFollowNode)
{
    // The if statement is defined by header and followNode
    // Follow node is not included in the if statement (since it may be used by many if statements)
    // We *should* be guaranteed that the nodes we replace aren't part of any other if statement.

    // To gather the nodes, we'll look at the two branchess of ifHeader, and follow things until we hit the follow node
    // This will represent the set of nodes included in the if statement.
    // At this point, we can probably assert that each node we encounter (other than the header) only has a
    // single successor. There shouldn't be any more branching at this point, other than the if statements
    // that we are collected. And we go from the most nested to the least.
    ControlFlowNode *ifNode = MakeStructuredNode<IfNode>();
    ExitNode *exitPoint = MakeNode<ExitNode>();
    exitPoint->startingAddressForExit = ifFollowNode->GetStartingAddress();
    (*ifNode)[SemId::Head] = ifHeader;
    // No tail for if node
    stack<ControlFlowNode*> toProcess;
    toProcess.push(ifHeader);
    NodeSet exitPredecessors;
    while (!toProcess.empty())
    {
        ControlFlowNode *node = pop_ptr(toProcess);

        if (ifNode->Children().contains(node))
        {
            // Castle of Dr Brain, script 325
            throw ControlFlowException(ifHeader, "Encountered same node in two if paths.");
            // Should only encounter things ONCE in the 1 or two if paths
        }

        ifNode->InsertChild(node);
        for (ControlFlowNode *succ : node->Successors())
        {
            if (succ == ifFollowNode)
            {
                // We've reached the end. The exit node should have the current guy
                // as one of its predecessors
                exitPredecessors.insert(node);
            }
            else
            {
                toProcess.push(succ);
            }
        }
    }

    exitPoint->AssignPredecessors(exitPredecessors);
    if (exitPoint->Predecessors().size() != 2)
    {
        throw ControlFlowException(ifHeader, "Unable to insert if node. Exit needs to predecessors.");
    }

    // Now remove the children from the parent structure
    for (ControlFlowNode *ifChild : ifNode->Children())
    {
        assert(structure->MaybeGet(SemId::Follow) != ifChild); // This should NEVER happen, since we don't include the follow node in the if node.
        structure->EraseChild(ifChild);
    }
    ifNode->InsertChild(exitPoint);
    structure->NotifyChildrenReplacedBy(ifNode);

    // The header's predecessors become the if's predecessors, and the header has no predeceesor
    ifNode->AssignPredecessors(ifHeader->Predecessors());
    ifHeader->ClearPredecessors();

    // The follow node should have the ifNode as its predecessor, and not any of the if node's children
    NodeSet followNodePreds = ifFollowNode->Predecessors();
    for (ControlFlowNode *followPred : followNodePreds)
    {
        if (ifNode->Children().contains(followPred))
        {
            ifFollowNode->ErasePredecessor(followPred);
        }
    }
    ifFollowNode->InsertPredecessor(ifNode);

    structure->InsertChild(ifNode);

    // We can determine information about if it's just an if, or and if-else, simply by
    // seeing if one of the header node's 2 Successors() is the exit point.
    ControlFlowNode *thenNode, *elseNode;
    GetThenAndElseBranches(ifHeader, &thenNode, &elseNode);
    (*ifNode)[SemId::Then] = thenNode;
    (*ifNode)[SemId::Else] = elseNode;

    return ifNode;
}

// If someone had a follow node that is inside newNode's children, the follow node should now be
// set to newNode - as long as that someone isn't a child of newNode.
// I haven't hit this scenario yet, but it seems plausible.
void ControlFlowGraph::_ReplaceNodeInFollowNodes(ControlFlowNode *newNode)
{
    for (ControlFlowNode *structure : discoveredControlStructures)
    {
        if (!newNode->Children().contains(structure))
        {
            // structure is NOT a child of new node. That's important, because otherwise the follow node of
            // structure would essentially be set to struture's parent.
            ControlFlowNode *oldFollowNode = structure->MaybeGet(SemId::Follow);
            if (oldFollowNode && (newNode->Children().contains(oldFollowNode)))
            {
                (*structure)[SemId::Follow] = newNode;
                // Let's also assert that the followNode was the head of newNode
                if (oldFollowNode != (*newNode)[SemId::Head])
                {
                    throw ControlFlowException(newNode, "Unable to replace node in follow nodes");
                }
           }
        }
    }
}

// newNode must have its head and tail set, and containedNodes must be the new children of newNode (including head and tail)
// newNode will be assigned the chidren in containedNodes
// dominators will be assigned to the new dominators for the workingSetNodes.
void ControlFlowGraph::_ReplaceNodeInWorkingSet(ControlFlowNode *parent, ControlFlowNode *newNode)
{
    ControlFlowNode *existingSingleExit = newNode->MaybeGet(SemId::Tail);

    // Given a new node, make it "contain" the containedNodes.
    // Remove the contained nodes from the nodesWorkingSet
    // Update node predecessors and dominators so that everything is ship shape.
    RemoveSetFromSet(parent, newNode->Children());

    // REVIEW: We also need to deal with constructs that stash their own things, head and tail.
    // head and tail are basically duplicate state, which sucks. Maybe we should reconsider have head and tail?
    // REVIEW: If we had node subclasses, they could each be responsible for updating their various nodes.
    parent->NotifyChildrenReplacedBy(newNode);

    NodeSet exitPoints;

    // The easiest way to update the dominators is to just recalculate them entirely (possibly expensive though).
    // That way, we just need to update the predecessors
    // For any node in the working set, if they have as one of their predecessors a node in conatinedNodes,
    // then remove that node and replace it with newNode.
    for (ControlFlowNode *workingSetNode : parent->Children())
    {
        NodeSet nodesToBeRemoved;
        set_intersection(workingSetNode->Predecessors().begin(), workingSetNode->Predecessors().end(),
            newNode->Children().begin(), newNode->Children().end(),
            inserter(nodesToBeRemoved, nodesToBeRemoved.begin()));
        if (!nodesToBeRemoved.empty())
        {
            for (ControlFlowNode* eraseMe : nodesToBeRemoved)
            {
                // an outside node had a newNode child as its predecssor... 
                workingSetNode->ErasePredecessor(eraseMe);

                // Now we need to ensure that we have an exit point. To avoid linking the exit nodes to the outside
                // (which is a possibility, but then our graph traversal algorithms would need to be exitpoint-aware),
                // we'll give the exit node an address that it exits to
                if (!existingSingleExit)
                {
                    newNode->InsertChild(_EnsureExitNode(exitPoints, eraseMe, workingSetNode));
                }
            }
            // and redirect the outside node's predecessor to be the newNode
            workingSetNode->InsertPredecessor(newNode);
        }
    }

    // This new nodes predecessors should be the same as the predecessors of its head that are
    // outside our control structure
    for (ControlFlowNode *predNode : (*newNode)[SemId::Head]->Predecessors())
    {
        if (!newNode->Children().contains(predNode))
        {
            newNode->InsertPredecessor(predNode);
        }
    }

    // Now actually insert ourselves
    parent->InsertChild(newNode);

    // Now remove any predecessors from the loopNode's head that are not in its children. We don't the loops nodes attached to the
    // outside world. They loopnode itself is the one that is now attached to the outside work.
    // Then we'll also add a token entry node.
    NodeSet predecessorsCopy = (*newNode)[SemId::Head]->Predecessors();
    for (ControlFlowNode *pred : predecessorsCopy)
    {
        if (!newNode->Children().contains(pred))
        {
            (*newNode)[SemId::Head]->ErasePredecessor(pred);
        }
    }

    if (exitPoints.size() == 1)
    {
        // Make this the tail, if we don't already have one?
        // assert(!newNode->MaybeGet(SemId::Tail));
        // Nope, even if we have one
        // If we DO have one though (e.g. in switch), we should ensure the exit point
        // has a single predeceesor which is the current tail
        ControlFlowNode *singleExit = *exitPoints.begin();
        ControlFlowNode *currentTail = newNode->MaybeGet(SemId::Tail);
        if (currentTail)
        {
            assert((singleExit->Predecessors().size() == 1) && ((*singleExit->Predecessors().begin()) == currentTail));
        }
        (*newNode)[SemId::Tail] = singleExit;
    }
}

bool IsNotAllowedAsPredecessor(code_pos pos)
{
    return pos->get_opcode() == Opcode::JMP || pos->get_opcode() == Opcode::RET;
}

int debugIndex = 1;

ControlFlowNode *_GetFirstPredecessorOrNull(ControlFlowNode *node)
{
    return node->Predecessors().empty() ? nullptr : *node->Predecessors().begin();
}

void ControlFlowGraph::_IdentifySwitchCases(ControlFlowNode *switchNodeIn)
{
    ControlFlowNode *switchNode = static_cast<SwitchNode*>(switchNodeIn);
    ControlFlowNode *toss = (*switchNode)[SemId::Tail];
    // Find the first case. It should be right after the head's only successor
    ControlFlowNode *switchHead = (*switchNode)[SemId::Head];
    assert(switchHead->Successors().size() == 1);
    ControlFlowNode *firstCase = *switchHead->Successors().begin();

    if (!firstCase->startsWith(Opcode::DUP) || !firstCase->endsWith(Opcode::BNT))
    {
        throw ControlFlowException(firstCase, "Unexpected structure for first case in switch");
    }

    // Discover all the cases.
    NodeSet caseHeads;
    ControlFlowNode *currentCase = firstCase;
    while (currentCase->startsWith(Opcode::DUP) && currentCase->endsWith(Opcode::BNT))
    {
        caseHeads.insert(currentCase);
        currentCase->Tags.insert(SemanticTags::CaseCondition);
        // Find where this guy branches
        ControlFlowNode *thenNode, *elseNode;
        GetThenAndElseBranches(currentCase, &thenNode, &elseNode);
        // Assuming it's a BNT, the else node will be the next cast
        currentCase = elseNode;
    }
    // We maybe have a final defaultc case, assuming currentCase isn't the toss
    if (currentCase != toss)
    {
        caseHeads.insert(currentCase);
        // I don't think it makes sense to tag this. The header node has no special significance.
        // currentCase->Tags.insert(SemanticTags::DefaultCase);
    }

    // Now we need to find the case tails and collect the nodes between head/tail. Then create the case nodes
    // We can look at the toss predecessors and see which ones are dominated by which cases.
    // Nope, that is actually not sufficient. The first case node dominate ALL toss preds.
    DominatorMap dominators = GenerateDominators(switchNodeIn, switchHead);
    NodeSet caseNodes;
    for (ControlFlowNode *caseHead : caseHeads)
    {
        ControlFlowNode *newCaseNode = MakeStructuredNode<CaseNode>();
        (*newCaseNode)[SemId::Head] = caseHead;

        newCaseNode->InsertChild(caseHead);
        // Find a statement that will dominate all others in the case. For the default,
        // this is the case head. For other cases, it's the first statement after the case head
        ControlFlowNode *firstStatementInCase, *elseNode;
        if (!caseHead->ContainsTag(SemanticTags::CaseCondition))
        {
            newCaseNode->Tags.insert(SemanticTags::DefaultCase); // Mark the actual case...
            firstStatementInCase = caseHead;
        }
        else
        {
            GetThenAndElseBranches(caseHead, &firstStatementInCase, &elseNode);
        }

        NodeSet exitPoints;
        for (ControlFlowNode *tossPred : toss->Predecessors())
        {
            if (dominators.IsADominatedByB(tossPred, firstStatementInCase))
            {
                // Collect the items here
                NodeSet newChildren = CollectNodesBetween(firstStatementInCase, tossPred, switchNodeIn->Children());
                for (auto &newChild : newChildren)
                {
                    newCaseNode->InsertChild(newChild);
                }
                exitPoints.insert(tossPred);
            }
        }
        if (exitPoints.size() == 1)
        {
            (*newCaseNode)[SemId::Tail] = *exitPoints.begin();
        }
        else
        {
            // Multiple exit points, which is possible with the final or default case. So let's
            // add an exit point node
            ExitNode *singleExitNode = MakeNode<ExitNode>();
            singleExitNode->startingAddressForExit = toss->GetStartingAddress();
            for (ControlFlowNode *exitPoint : exitPoints)
            {
                singleExitNode->InsertPredecessor(exitPoint);
            }
            (*newCaseNode)[SemId::Tail] = singleExitNode;
            newCaseNode->InsertChild(singleExitNode);
        }
        caseNodes.insert(newCaseNode);
    }

    // Now replace them in the graph
    // Don't use _ReplaceNodeInWorkingSet, because we'll be very deliberate about how the cases connect to the switch
    toss->ClearPredecessors();
    assert(switchHead->Successors().size() == 1);   // It won't after this though:
    for (ControlFlowNode *newCaseNode : caseNodes)
    {
        (*newCaseNode)[SemId::Head]->ClearPredecessors();
        toss->InsertPredecessor(newCaseNode);
        newCaseNode->InsertPredecessor(switchHead);
        RemoveSetFromSet(switchNode, newCaseNode->Children());
        switchNode->InsertChild(newCaseNode);
        // REVIEW: Is there more to do with semantic nodes?
    }
}

bool HasPrecedessorInSet(ControlFlowNode *node, const NodeSet &theSet)
{
    for (ControlFlowNode *pred : node->Predecessors())
    {
        if (theSet.contains(pred))
        {
            return true;
        }
    }
    return false;
}

ControlFlowNode *ControlFlowGraph::_FindFollowNodeForStructure(ControlFlowNode *structure)
{
    ControlFlowNode *follow = nullptr;
    // Find the follow node. This isn't trivial. We can't just ask for the post dominator of the
    // header - that won't work in the case of compound conditions (which are still unresolved at this point)
    // We can leverage a tautology of SCI compilers - code always proceeds forward in address. So we can
    // find the furthest branch of any of our children, then try to find the node that matches it.

    stack<ControlFlowNode*> toProcess;
    toProcess.push(structure);
    uint16_t maxAddress = 0;
    //uint16_t maxAddressDebug = 0;
    while (!toProcess.empty())
    {
        ControlFlowNode *node = pop_ptr(toProcess);
        if (node->Type == CFGNodeType::RawCode)
        {
            scii lastInstruction = node->getLastInstruction();
            // We used to have this ^^^ code and it worked. But it always returns false
            // and the case was never hit. Fixing it, and asserting it makes no diff
            if (lastInstruction._is_branch_instruction())
            {
                uint16_t target = lastInstruction.get_branch_target()->get_final_offset();
                maxAddress = max(maxAddress, target);
            }
            // Also potentially handle non-loops, by just using the address after the last code address
            uint16_t postFinalAddress = (static_cast<RawCodeNode*>(node))->end->get_final_offset();
            assert(postFinalAddress != 0xffff);
            maxAddress = max(postFinalAddress, maxAddress);

         //   maxAddressDebug = max(postFinalAddress, maxAddress); // temp
        }
        else
        {
            for (ControlFlowNode *child : node->Children())
            {
                toProcess.push(child);
            }
        }
    }

    assert(maxAddress);
   // assert(maxAddress == maxAddressDebug && "Investigate... we might be ok, but it's different than before, so understand it");

    // Now we need to follow the hierarchy down to the right level. This is not trivial. If callers *only* use this function after
    // they have replaced themselves in the tree, then we can find the node with a starting address equal to maxAddress, and who
    // has structure as a predecessor.
    // Ok, I've changed this. Callers should *only* use this prior to replacing themselves in the tree.
    toProcess.push(mainStructure);
    while (!toProcess.empty())
    {
        ControlFlowNode *node = pop_ptr(toProcess);
        if (node->GetStartingAddress() == maxAddress)
        {
            if (HasPrecedessorInSet(node, structure->Children()))
            {
                follow = node;
                break;
            }
        }
        for (ControlFlowNode *child : node->Children())
        {
            toProcess.push(child);
        }
    }

    if (!follow)
    {
        throw ControlFlowException(structure, "Can't find follow node for structure");
    }
    
    return follow;
}

void CollectMoreChildren(ControlFlowNode *structure, const DominatorMap &dominators)
{
    ControlFlowNode *followNode = (*structure)[SemId::Follow];
    ControlFlowNode *head = (*structure)[SemId::Head];
    stack<ControlFlowNode*> toProcess;
    toProcess.push(followNode);
    while (!toProcess.empty())
    {
        ControlFlowNode *node = pop_ptr(toProcess);
        for (ControlFlowNode *pred : node->Predecessors())
        {
            const NodeSet &predDoms = dominators.at(pred);
            if ((pred != head) && (predDoms.contains(head)))
            {
                // It's a predecessor that's dominated by the structure header. It's definitely
                // one of its children. Avoid needless processing by only adding it if it's not already
                // in children
                if (!structure->Children().contains(pred))
                {
                    structure->InsertChild(pred);
                    toProcess.push(pred);
                }
            }
        }
    }
}

// Returns the loop node, already replaced in the parent's chidlren
ControlFlowNode *ControlFlowGraph::_ProcessNaturalLoop(ControlFlowGraph &loopDetection, ControlFlowNode *parent, const NodeBlock &backEdge)
{
    loopDetection._decompilerResults.AddResult(DecompilerResultType::Update, fmt::format("{0} Loop:{1:04x}", loopDetection._statusMessagePrefix, backEdge.head->GetStartingAddress()));

    LoopNode *loopNode = loopDetection.MakeStructuredNode<LoopNode>();
    loopNode->SetChildren(backEdge.body);
    (*loopNode)[SemId::Head] = backEdge.head;           // This is the start, or the branch
    (*loopNode)[SemId::Latch] = backEdge.latch;         // This is the latch node... could be multiple ones though. REVIEW: I think we fixed that with CommonLatchNode

    ControlFlowNode *followNode = loopDetection._FindFollowNodeForStructure(loopNode);
    (*loopNode)[SemId::Follow] = followNode;
    // Now let's collect more children. Any predecessors of the child who are dominated by our header node should be included.
    // This will collect things like breaks, etc... that were not collected when we went from the latch node to the header to
    // collect children.
    DominatorMap dominators = GenerateDominators(parent, (*parent)[SemId::Head]);
    CollectMoreChildren(loopNode, dominators);

    loopDetection._ReplaceNodeInWorkingSet(parent, loopNode);
    loopDetection._ReplaceNodeInFollowNodes(loopNode);

    // Also assert that we have a tail that is an exit point. If not, it might mean we think we have >1 exits.
    // This could be indicative of a goto (or mid-identified break)
    ControlFlowNode *tail = loopNode->MaybeGet(SemId::Tail);
    if (!tail || (tail->Type != CFGNodeType::Exit))
    {
        throw ControlFlowException(loopNode, "No loop tail, or not an exit. Multiple loop exits?");
    }

    return loopNode;
}

ControlFlowNode *ControlFlowGraph::_ProcessSwitch(ControlFlowGraph &loopDetection, ControlFlowNode *parent, NodeBlock &switchBlock)
{
    loopDetection._decompilerResults.AddResult(DecompilerResultType::Update, fmt::format("{0} Switch:{1:04x}", loopDetection._statusMessagePrefix, switchBlock.head->GetStartingAddress()));

    // Reconstruct the nodes in switchBlock into a switch statement, and replace it in the parent's children.
    SwitchNode *switchNode = loopDetection.MakeStructuredNode<SwitchNode>();
    (*switchNode)[SemId::Tail] = switchBlock.latch;    // Note: This is the tail, not actually a latch node.
    (*switchNode)[SemId::Head] = switchBlock.head;
    switchNode->SetChildren(switchBlock.body);

    // Now, in the working set, replace the guys that are part of the switch with a single switch node:
    loopDetection._ReplaceNodeInWorkingSet(parent, switchNode);
    loopDetection._ReplaceNodeInFollowNodes(switchNode);

    // Now of course, we need to break down our cases
    loopDetection._IdentifySwitchCases(switchNode);

    return switchNode;
}

vector<NodeBlock> ControlFlowGraph::_FindSwitchBlocks(DominatorMap &dominators, DominatorMap &postDominators, ControlFlowNode *structure)
{
    ControlFlowNode *header = (*structure)[SemId::Head];
    const NodeSet &allNodes = structure->Children();
    vector<NodeBlock> switchBlocks;
    for (ControlFlowNode *maybeToss : allNodes)
    {
        // Switches don't necessarily have a "pure" shape in a control flow graph like loops do.
        // In SCI, switches finish with a TOSS instruction, so let's specifically look for that.
        if (maybeToss->startsWith(Opcode::TOSS))
        {
            auto &tossDominators = dominators.at(maybeToss);
            ControlFlowNode *pred = _GetFirstPredecessorOrNull(maybeToss);
            while (pred)
            {
                if (tossDominators.contains(pred))
                {
                    // We found it. This pred node dominates the toss. Assert that there's DUP instruction in here.
                    // If we find that in some cases there isn't, we'll need to adjust our algorithm.
                    if (!pred->contains(Opcode::DUP))
                    {
                        throw ControlFlowException(pred, "Encountered toss instruction but no switch.");
                    }

                    // Now, the thing we're duping comes from the previous node. But the previous node
                    // might actually contain a whole sequence of instructions, only the last of which
                    // is important for the switch statement. Maybe that's ok though. As long as we're
                    // aware that it's not *just* the value that's being switched on.
                    if (pred->Predecessors().size() != 1)
                    {
                        throw ControlFlowException(pred, "Switch value with embedded branch not yet supported.");

                    }
                    ControlFlowNode *switchHead = _GetFirstPredecessorOrNull(pred);
                    switchBlocks.emplace_back(switchHead, maybeToss, true, nullptr);
                    break;
                }
                pred = _GetFirstPredecessorOrNull(pred);
            }
        }
    }
    return switchBlocks;
}


void ControlFlowGraph::_FindAllCompoundConditions()
{
    // Make a copy, since we'll be modifying it (adding to it)
    NodeSet controlStructuresCopy = discoveredControlStructures;
    // The first time we call _FindAllStructuresOf, we'll just have a single discovered structure(the main one)
    // After that we'll also have whatever structures were discovered in the previous call.
    // We discover structures from the most inner to the most outer (in this particular call of the function)
    for (ControlFlowNode *structure : controlStructuresCopy)
    {
        _FindCompoundConditions(structure);
        if (_decompilerResults.IsAborted())
        {
            break;
        }
    }
}

    // We want to navigate down the hierarchy of loops (stopping at any loop)
    // and look for nodes that end in unconditional jumps to the loop follow.
    // Then, we'll mark them at "break", and reconnect them to the subsequent node in
    // memory (that is a sibling of the current structure)
void ControlFlowGraph::_ResolveBreaks()
{
    bool changes = true;
    while (changes)
    {
        changes = false;
        for (ControlFlowNode *structure : discoveredControlStructures)
        {
            if (structure->Type == CFGNodeType::Loop)
            {
                changes = _ResolveBreak((*structure)[SemId::Follow]->GetStartingAddress(), structure);
                if (changes)
                {
                    break;
                }
            }
        }
    }
}

void ControlFlowGraph::_RestructureBreaks()
{
    bool changes = true;
    while (changes)
    {
        changes = false;
        for (ControlFlowNode *structure : discoveredControlStructures)
        {
            if (structure->Type == CFGNodeType::Loop)
            {
                // Ignore the loop condition, whether it's head or latch
                ControlFlowNode *ignore = nullptr;
                if ((*structure)[SemId::Head]->Successors().size() > 1)
                {
                    ignore = (*structure)[SemId::Head];
                }
                else if ((*structure)[SemId::Latch]->Successors().size() > 1)
                {
                    ignore = (*structure)[SemId::Latch];
                }
                changes = _RestructureBreak((*structure)[SemId::Follow]->GetStartingAddress(), ignore, structure);
                if (changes)
                {
                    break;
                }
            }
        }
    }
}

bool ControlFlowGraph::_RestructureBreak(uint16_t loopFollowAddress, ControlFlowNode *ignore, ControlFlowNode *structure)
{
    bool changes = false;
    // In order to detect our if statements, we want breaks to be in the form of
    // if (something)   // bnt
    //      break;      // jmp, which we mark as a break, and reconnect to "more code"
    // more code
    //
    // But a common pattern is to have:
    // if (something)   // bt to exit
    // more code
    // 
    // So let's see if we can restructure to the first case
    // We want to ignore the loop condition though, as that could have bt's to exit.
    for (ControlFlowNode *node : structure->Children())
    {
        if ((node != ignore) && (node->Type == CFGNodeType::RawCode) && (node->endsWith(Opcode::BT)))
        {
            code_pos followingCode = (static_cast<RawCodeNode*>(node))->end;
            code_pos end = followingCode;
            --end;
            if (end->get_branch_target()->get_final_offset() == loopFollowAddress)
            {
                changes = true;

                ControlFlowNode *exitNode, *subsequentCodeNode;
                GetThenAndElseBranches(node, &exitNode, &subsequentCodeNode);

                // Ok, this is a true branch to exit. Now we're going to do some crazy stuff
                end->set_opcode(Opcode::BNT);                   // Yup, switch to a BNT
                end->set_branch_target(followingCode, true);    // And it now branches to the following code instead of exit

                // Make a new node that will be the break
                FakeBreakNode *newJmp = MakeNode<FakeBreakNode>();
                newJmp->Tags.insert(SemanticTags::LoopBreak);
                structure->InsertChild(newJmp);

                exitNode->ErasePredecessor(node);
                newJmp->InsertPredecessor(node);
                subsequentCodeNode->InsertPredecessor(newJmp);
                assert(subsequentCodeNode->Predecessors().size() >= 2);

                //assert(exitNode->Predecessors().size() > 0); // Otherwise we need to remove it from children. Look in _Reconnect* to figure this out.
                if (exitNode->Predecessors().empty())
                {
                    structure->EraseChild(exitNode);
                }
                
                break;
            }
        }
    }

    if (!changes)
    {
        // Now recurse, as long as its not another loop
        for (ControlFlowNode *child : structure->Children())
        {
            if ((child != ignore) && (child->Type != CFGNodeType::Loop))
            {
                changes = _RestructureBreak(loopFollowAddress, nullptr, child);
                if (changes)
                {
                    break;
                }
            }
        }
    }
    return changes;
}

bool ControlFlowGraph::_ResolveBreak(uint16_t loopFollowAddress, ControlFlowNode *structure)
{
    bool changes = false;
    // We're looking for nodes that end in an unconditional jump to the loop follow address
    for (ControlFlowNode *node : structure->Children())
    {
        if (node->Type == CFGNodeType::RawCode)
        {
            if (node->endsWith(Opcode::JMP) && !node->ContainsTag(SemanticTags::LoopBreak)) // Not already identified
            {
                scii inst = node->getLastInstruction();
                // Conveniently, end provides us with the address of the next instructino.
                code_pos end = static_cast<RawCodeNode*>(node)->end;
                if (inst.get_branch_target()->get_final_offset() == loopFollowAddress)
                {
                    // This jump goes to the loop follow address
                    node->Tags.insert(SemanticTags::LoopBreak);
                    _ReconnectBreakNodeToSubsequentCode(structure, node, end);
                    changes = true;
                    break;
                }
            }
        }
    }

    if (!changes)
    {
        // Now recurse, as long as its not another loop
        for (ControlFlowNode *child : structure->Children())
        {
            if (child->Type != CFGNodeType::Loop)
            {
                changes = _ResolveBreak(loopFollowAddress, child);
                if (changes)
                {
                    break;
                }
            }
        }
    }
    return changes;
}

void ControlFlowGraph::_ReconnectBreakNodeToSubsequentCode(ControlFlowNode *structure, ControlFlowNode *breakNode, code_pos subsequentcode)
{
    uint16_t subsequentInstructionAddress = subsequentcode->get_final_offset();

    assert(breakNode->Successors().size() == 1);
    ControlFlowNode *currentSuccessor = *breakNode->Successors().begin();
    currentSuccessor->ErasePredecessor(breakNode);

    if (currentSuccessor->Predecessors().size() == 0)
    {
        assert(currentSuccessor->Type == CFGNodeType::Exit);
        // If we don't remove this guy from children, our dominator and postorder code will
        // be confused
        structure->EraseChild(currentSuccessor);
        // But we still need to know about the exit node.
    }

    // Now find the node in structure that has the right starting address
    // We should find it in the this control structure. It may be an exit node. If we don't find
    // it, then we probably need to add an exit node.
    for (ControlFlowNode *child : structure->Children())
    {
        if (child->GetStartingAddress() == subsequentInstructionAddress)
        {
            child->InsertPredecessor(breakNode);
            return;
        }
    }
    // Oh, but we may have removed the exit node from the children because it had no more preds
    // (and that would have confused dominator calcs).
    // So check for an exit node.
    ControlFlowNode *previouslyRemovedExit = (*structure)[SemId::Tail];
    if (previouslyRemovedExit && (previouslyRemovedExit->GetStartingAddress() == subsequentInstructionAddress))
    {
        previouslyRemovedExit->InsertPredecessor(breakNode);
        structure->InsertChild(previouslyRemovedExit);
        return;
    }

    // We could get here in the case where we have two subsequent jmp instructions (with no one branching to the second one), and
    // we would have pruned out the degenerate second branch node. In that case, subsequentcode should be a jmp, and we can
    // increment it and try once more.
    if (subsequentcode->get_opcode() == Opcode::JMP)
    {
        ++subsequentcode;
        subsequentInstructionAddress = subsequentcode->get_final_offset();
        for (ControlFlowNode *child : structure->Children())
        {
            if (child->GetStartingAddress() == subsequentInstructionAddress)
            {
                child->InsertPredecessor(breakNode);
                return;
            }
        }
    }

    // TODO: Clean this up, we're repeating code from above.
    if (previouslyRemovedExit && (previouslyRemovedExit->GetStartingAddress() == subsequentInstructionAddress))
    {
        previouslyRemovedExit->InsertPredecessor(breakNode);
        structure->InsertChild(previouslyRemovedExit);
        return;
    }

    throw ControlFlowException(structure, "Unable to re-connect break statement. Need to add exit node to control structure?");
}

void ControlFlowGraph::_DoLoopTransforms()
{
    // Make a copy, since we'll be potentially changing discoveredControlStructures.
    vector<ControlFlowNode*> loops;
    copy_if(discoveredControlStructures.begin(), discoveredControlStructures.end(), back_inserter(loops),
        [](const ControlFlowNode *node) { return node->Type == CFGNodeType::Loop; });

    for (ControlFlowNode *structure : loops)
    {
        if (structure->Type == CFGNodeType::Loop)
        {
            _DoLoopTransform(structure);
        }
        if (_decompilerResults.IsAborted())
        {
            break;
        }
    }
}

void ControlFlowGraph::_DoLoopTransform(ControlFlowNode *loop)
{
    // While loop: 2-way header node, and 1-way latch node
    // Do loop: 2-way latch node and non-conditional header node
    //
    // However, they don't have that shape yet.
    //
    // Some problems we have:
    //  - the latch node for do-whiles is just a jmp instruction. We need to merge this with its
    //      predecessor, which should be a 2-way node that is that actual condition.
    //      bnt A
    //  B: jmp to loop beginning
    //  A: outside of loop
    //
    //     is equivalent to:
    //      bt to loop beginning
    //  We need to be careful not to mis-identify while loops (we are only concered with dos)
    ControlFlowNode *latch = (*loop)[SemId::Latch];
    ControlFlowNode *head = (*loop)[SemId::Head];
    if (head->Successors().size() <= 1) // Weed out the while loops.
    {
        if ((latch->Type == CFGNodeType::RawCode) && latch->startsWith(Opcode::JMP))
        {
            if (latch->Predecessors().size() == 1)
            {
                ControlFlowNode *branchNode = *latch->Predecessors().begin();
                // It could be an empty while loop too... check against this by seeing if our supposed branch
                // node is the header.
                if (branchNode != (*loop)[SemId::Head])
                {
                    ControlFlowNode *otherBranchDestination = GetOtherBranch(branchNode, latch);
                    if (otherBranchDestination) // A while might not have one
                    {
                        // And if it did, it wouldn't be an exit node
                        if (otherBranchDestination->Type == CFGNodeType::Exit)
                        {
                            uint16_t address1 = otherBranchDestination->GetStartingAddress();
                            uint16_t address2 = (*loop)[SemId::Follow]->GetStartingAddress();
                            assert(address1 == address2);
                            ControlFlowNode *invert = MakeStructuredNode<InvertNode>();
                            invert->InsertChild(latch);
                            invert->InsertChild(branchNode);
                            (*invert)[SemId::Head] = branchNode;    // We just need to invert this
                            _ReplaceNodeInWorkingSet(loop, invert);
                            _ReplaceNodeInFollowNodes(invert);

                            // And the exit should have the new invert as a predecessor
                            assert(otherBranchDestination->Predecessors().contains(invert));
                        }
                    }
                }
            }
        }
    }
}

void ControlFlowGraph::_FindCompoundConditions(ControlFlowNode *structure)
{
    bool changesMade = true;
    while (changesMade)
    {
        changesMade = false;
        stack<ControlFlowNode*> toVisit;
        NodeSet visited;
        toVisit.push((*structure)[SemId::Head]);
        visited.insert((*structure)[SemId::Head]);
        while (!toVisit.empty())
        {
            ControlFlowNode *possibleStart = pop_ptr(toVisit);
            // This shouldn't result in an infinite loop, since this should be an acyclic graph at this point.
            // Hmm nope, because I do this inside of while loops... although perhaps I should have removed the loop
            // cyckes?
            for (ControlFlowNode *successor : possibleStart->Successors())
            {
                if ((successor->Type != CFGNodeType::Exit) && (!visited.contains(successor)))
                {
                    toVisit.push(successor);
                    visited.insert(successor);
                }
            }

            // Sometimes possibleStart's successors contain itself (e.g. SQ5, script 850), and we would misidentify
            // this as a compound condition (it would instead be a loop)
            if ((possibleStart->Successors().size() == 2) && !possibleStart->Successors().contains(possibleStart))
            {
                // Two way node. Check to see if one of these ends in a branch
                auto it = possibleStart->Successors().begin();
                auto itNext = it;
                itNext++;
                ControlFlowNode *branchNode = nullptr;
                ControlFlowNode *other = nullptr;
                if (EndsWithConditionalBranch(*it))
                {
                    // Does the branch node only have a single successor that is the start node?
                    ControlFlowNode *possibleBranch = *it;
                    if ((possibleBranch->Predecessors().size() == 1) && (*possibleBranch->Predecessors().begin() == possibleStart))
                    {
                        branchNode = possibleBranch;
                        other = *itNext;
                    }
                }

                if (!branchNode && EndsWithConditionalBranch(*itNext))
                {
                    // Does the branch node only have a single successor that is the start node?
                    ControlFlowNode *possibleBranch = *itNext;
                    if ((possibleBranch->Predecessors().size() == 1) && (*possibleBranch->Predecessors().begin() == possibleStart))
                    {
                        branchNode = possibleBranch;
                        other = *it;
                    }
                }

                // REVIEW: FIX: there are two possible branches with conditionals... we only check one, and it might
                // be the OTHER that has a single entry.... so we'll miss it.

                if (branchNode)
                {
                    // The only remaining thing to check is that this branch node also branches to "other".
                    if (branchNode->Successors().contains(other))
                    {
                        // We found a compound condition.
                        // MakeNewNode(CFGNodeType::And);
                        // TODO: With the help of those 4 graphs, and GetThenAndElseBranches, we can
                        // make the correct compound condition, then then assign its thenBranch.
                        // Once that's done, we'll write the function to handle ifs.
                        // REVIEW: Will this catch whiles too?

                        // From http://www.labri.fr/perso/fleury/download/papers/binary_analysis/cifuentes96structuring.pdf
                        // First case: A || B
                        // Shall we express everything in terms of BT?
                        ControlFlowNode *firstThenNode, *firstElseNode, *secondThenNode, *secondElseNode;
                        GetThenAndElseBranches(possibleStart, &firstThenNode, &firstElseNode);
                        GetThenAndElseBranches(branchNode, &secondThenNode, &secondElseNode);
                        CompoundConditionNode *newConditionNode = nullptr;
                        bool throwException = false;
                        if (firstElseNode == secondElseNode)
                        {
                            throwException = (firstThenNode != branchNode);
                            // This is (X and Y)
                            newConditionNode = MakeStructuredNode<CompoundConditionNode>(ConditionType::And);
                            newConditionNode->isFirstTermNegated = false;
                            newConditionNode->thenBranch = secondThenNode->GetStartingAddress();
                        }
                        else if (firstThenNode == secondThenNode)
                        {
                            throwException = (firstElseNode != branchNode);
                            // This is (X or Y)
                            newConditionNode = MakeStructuredNode<CompoundConditionNode>(ConditionType::Or);
                            newConditionNode->isFirstTermNegated = false;
                            newConditionNode->thenBranch = firstThenNode->GetStartingAddress();
                        }
                        else if (firstElseNode == secondThenNode)
                        {
                            throwException = (firstThenNode != branchNode);
                            // This is (!X or Y);
                            newConditionNode = MakeStructuredNode<CompoundConditionNode>(ConditionType::Or);
                            newConditionNode->isFirstTermNegated = true;
                            newConditionNode->thenBranch = secondThenNode->GetStartingAddress();
                        }
                        else if (firstThenNode == secondElseNode)
                        {
                            throwException = (firstElseNode != branchNode);
                            // This is (!X and Y)
                            newConditionNode = MakeStructuredNode<CompoundConditionNode>(ConditionType::And);
                            newConditionNode->isFirstTermNegated = true;
                            newConditionNode->thenBranch = secondThenNode->GetStartingAddress();
                        }

                        if (throwException)
                        {
                            throw ControlFlowException(possibleStart, "Unable to resolve compound condition");
                        }

                        (*newConditionNode)[SemId::Head] = possibleStart;
                        (*newConditionNode)[SemId::First] = possibleStart;
                        (*newConditionNode)[SemId::Second] = branchNode;
                        newConditionNode->InsertChild(possibleStart);
                        newConditionNode->InsertChild(branchNode);

                        // Now we want to remove possibleStart and branchNode from the tree, and replace with newConditionNode
                        _ReplaceNodeInWorkingSet(structure, newConditionNode);
                        _ReplaceNodeInFollowNodes(newConditionNode);

                        changesMade = true;
                        break;  // Restart anew
                    }
                }
            }
        }
    }
}

vector<NodeBlock> _FindBackEdges(DominatorMap &dominators, DominatorMap &postDominators, ControlFlowNode *structure)
{
    map<ControlFlowNode*, ControlFlowNode*> immediatePostDominators = CalculateImmediatePostDominators(postDominators);

    ControlFlowNode *header = (*structure)[SemId::Head];
    const NodeSet &allNodes = structure->Children();
    vector<NodeBlock> backEdges;
    for (ControlFlowNode *node : allNodes)
    {
        for (ControlFlowNode *pred : node->Predecessors())
        {
            auto &predsDoms = dominators.at(pred);
            // Does node dominate its predecessor? If so, pred -> node is a back edge
            if (predsDoms.contains(node))
            {
                backEdges.emplace_back(node, pred, true, structure);
            }
        }
    }
    return backEdges;
}

// Return true if we made no changes
bool _CheckForSameHeader(ControlFlowGraph &cfg, ControlFlowNode &parent, vector<NodeBlock> &blocks)
{
    bool noChanges = true;
    // Look for any blocks with the same header but different latches. If we find them, introduce a new
    // common latch node.
    // We hit this scenario, for instance, in do-while loops with compound conditions.

    // Just some kind of order.
    sort(blocks.begin(), blocks.end(),
        [](const NodeBlock &A, const NodeBlock &B)
    {
        // Return true if one is less than two
        uint16_t aStart = A.head->GetStartingAddress();
        uint16_t aEnd = A.latch->GetStartingAddress();
        uint16_t bStart = B.head->GetStartingAddress();
        uint16_t bEnd = B.latch->GetStartingAddress();
        if (aStart < bStart)
        {
            return true;
        }
        if (bStart < aStart)
        {
            return false;
        }
        if (aEnd < bEnd)
        {
            return true;
        }
        return false;
    }
    );

    // Now ones with the same head should be adjacent in the vector, making it easier
    // to process.
    for (size_t i = 0; i < blocks.size(); )
    {
        vector<NodeBlock*> blocksWithSameHeader;
        blocksWithSameHeader.push_back(&blocks[i]);
        i++;
        for (; i < blocks.size(); i++)
        {
            if (blocks[i].head == blocksWithSameHeader[0]->head)
            {
                blocksWithSameHeader.push_back(&blocks[i]);
            }
            else
            {
                break;
            }
        }
        if (blocksWithSameHeader.size() > 1)
        {
            noChanges = false;
            // Create a common latch node. It will have our latch nodes as predecessors
            CommonLatchNode *common = cfg.MakeNode<CommonLatchNode>();
            parent.InsertChild(common);
            for (NodeBlock *block : blocksWithSameHeader)
            {
                // The "starting address" of the common latch will be the max starting address of the original latches.
                // This doesn't really make sense, but we need something.
                // REVIEW: This causes problems (SQ5, 26, ScrollableInventory::retreat). It confuses if statement resolution
                // because the 2 successors of branch may point to the same "starting address".
                common->tokenStartingAddress = max(common->tokenStartingAddress, block->latch->GetStartingAddress());
                common->InsertPredecessor(block->latch);
                block->head->ErasePredecessor(block->latch);
            }
            blocksWithSameHeader[0]->head->InsertPredecessor(common);
            break; // Only do one set of these at a time.
        }
    }
    return noChanges;
}

void _GetPostOrder(NodeSet &visited, ControlFlowNode *node, vector<ControlFlowNode*> &ordered)
{
    // Mark ourselves as visited
    visited.insert(node);
    // Visit children who are not visited
    for (ControlFlowNode *succ : node->Successors())
    {
        if (!visited.contains(succ))
        {
            _GetPostOrder(visited, succ, ordered);
        }
    }
    // Then add ourselves
    ordered.push_back(node);
}

vector<ControlFlowNode*> _GetPostOrder(ControlFlowNode *structure)
{
    vector<ControlFlowNode*> ordered;
    NodeSet visited;
    _GetPostOrder(visited, (*structure)[SemId::Head], ordered);
    assert(ordered.size() == structure->Children().size());
    return ordered;
}

ControlFlowNode *_FindMaxNodeDominatedByMWithTwoOrMoreInEdges(DominatorMap &dominators, const map<ControlFlowNode*, ControlFlowNode*> &immediateDominators, vector<ControlFlowNode*> &postOrdered, ControlFlowNode *m, ControlFlowNode *dontGoBeyondThis)
{
    auto endIt = find(postOrdered.begin(), postOrdered.end(), dontGoBeyondThis);
    auto it = postOrdered.begin();
    while (it != endIt)
    {
        // Work the graph from the "bottom up" until dontGoBeyondThis
        ControlFlowNode *possibleFollow = *it;
        if (possibleFollow->Predecessors().size() >= 2)
        {
            NodeSet &dominatorsForNode = dominators.at(possibleFollow);
            if (dominatorsForNode.contains(m))
            {
                if (immediateDominators.at(possibleFollow) == m)
                {
                    return possibleFollow;
                }
            }
        }

        ++it;
    }
    return nullptr;
}

void ControlFlowGraph::_FindAllIfStatements()
{
    // Make a copy, since we'll be modifying it (adding to it)
    NodeSet controlStructuresCopy = discoveredControlStructures;
    // The first time we call _FindAllStructuresOf, we'll just have a single discovered structure(the main one)
    // After that we'll also have whatever structures were discovered in the previous call.
    // We discover structures from the most inner to the most outer (in this particular call of the function)
    for (ControlFlowNode *structure : controlStructuresCopy)
    {
        DominatorMap dominators = GenerateDominators(structure, (*structure)[SemId::Head]);
        _FindIfStatements(dominators, structure);
        if (_decompilerResults.IsAborted())
        {
            break;
        }
    }
}

// A loop either has a condition in the head or latch. We'll return which ever that is.
ControlFlowNode *_GetTrueLoopLatchOrHead(ControlFlowNode *loop)
{
    // Check the then/else nodes. Whichever (head or latch) has one branch that points
    // to the loop tail (exit) is the one that is the condition.
    ControlFlowNode *thenNode, *elseNode;
    if (MaybeGetThenAndElseBranches((*loop)[SemId::Head], &thenNode, &elseNode))
    {
        if ((thenNode == (*loop)[SemId::Tail]) || (elseNode == (*loop)[SemId::Tail]))
        {
            return (*loop)[SemId::Head];
        }
    }

    if (MaybeGetThenAndElseBranches((*loop)[SemId::Latch], &thenNode, &elseNode))
    {
        if ((thenNode == (*loop)[SemId::Tail]) || (elseNode == (*loop)[SemId::Tail]))
        {
            return (*loop)[SemId::Latch];
        }
    }

    return nullptr;
}

void ControlFlowGraph::_FindIfStatements(DominatorMap &dominators, ControlFlowNode *structure)
{
    if ((structure->Type == CFGNodeType::CompoundCondition) || (structure->Type == CFGNodeType::Switch) || (structure->Type == CFGNodeType::Invert))
    {
        // This seems hacky. And it might interfere with ternary operators?
        return;
    }

    ControlFlowNode *loopLatchOrHead = nullptr;
    if (structure->Type == CFGNodeType::Loop)
    {
        loopLatchOrHead = _GetTrueLoopLatchOrHead(structure);
    }

    vector<ControlFlowNode*> postOrdered = _GetPostOrder(structure);
    vector<NodeBlock> ifBlocks;
    NodeSet unresolvedNodes;      // Not sure what the purpose of this is. Perhaps just to track situations where we can't resolve?
    map<ControlFlowNode*, ControlFlowNode*> followerOf; // header -> follower map

    map<ControlFlowNode*, ControlFlowNode*> immediateDominators = CalculateImmediateDominators(dominators);

    for (ControlFlowNode *node : postOrdered)
    {
        // Note: We need to be careful not to identify case conditions as if statements. However, our CaseNodes are structured
        // so that the branch node only has a single branch. So they wont'be detected here:
        if ((node->Successors().size() == 2) && (node != loopLatchOrHead))
        {
            assert(!node->ContainsTag(SemanticTags::CaseCondition));
            // Look for furthest node whose immediate dominator is m and who has 2 or more in edges.
            // i.e. the "follow" node of the if.
            ControlFlowNode *n = _FindMaxNodeDominatedByMWithTwoOrMoreInEdges(dominators, immediateDominators, postOrdered, node, node);
            if (n)
            {
                followerOf[node] = n;
                NodeSet nowResolved;
                for (ControlFlowNode *unresolved : unresolvedNodes)
                {
                    // Note that the algorithm here just automatically resolves all unresolved guys, and
                    // assigns them a follower of n:
                    // http://www.labri.fr/perso/fleury/download/papers/binary_analysis/cifuentes96structuring.pdf
                    // This doesn't work at all. Instead, I think we need to repeat the test... so we'll do that.
                    ControlFlowNode *nRepeat = _FindMaxNodeDominatedByMWithTwoOrMoreInEdges(dominators, immediateDominators, postOrdered, node, unresolved);
                    if (nRepeat)
                    {
                        assert(nRepeat == n);   // Otherwise we still have bugs...
                        nowResolved.insert(unresolved);
                        followerOf[unresolved] = nRepeat;
                    }
                }
                for (ControlFlowNode *resolved : nowResolved)
                {
                    unresolvedNodes.erase(resolved);
                }
            }
            else
            {
                unresolvedNodes.insert(node);
            }
        }
    }

    if (!unresolvedNodes.empty())
    {
        throw ControlFlowException(*unresolvedNodes.begin(), "Unable to resolve if branches");
    }
    
    // So now we have a list of branch nodes and their follower. We want to replace this with a
    // new if node (excluding the follower, however). We need to do this from the most nested to the least
    // nester. We can do this by using the post-order we have already calculated.
    for (ControlFlowNode *possibleHead : postOrdered)
    {
        auto itPair = followerOf.find(possibleHead);
        if (itPair != followerOf.end())
        {
            ControlFlowNode *headThatWillTurnIntoIf = itPair->first;
            ControlFlowNode *ifFollowNode = itPair->second;
            ControlFlowNode *newIfNode = _ReplaceIfStatementInWorkingSet(structure, headThatWillTurnIntoIf, ifFollowNode);
            // We need to do one special thing here. This is an artifact of us storing child references elsewhere :-(
            // If any of the follow nodes was equal to the header node of the if we just created, we need to repoint
            // them to the new if.
            for (auto &headAndFoloder : followerOf)
            {
                if (headAndFoloder.second == headThatWillTurnIntoIf)
                {
                    headAndFoloder.second = newIfNode;
                }
            }
        }
    }
}

void _RepairBranches(code_pos start, code_pos end)
{
    // Some scripts have bt instructions that jump right to bnts (or vice versa). This is basically
    // a no-op, and it messes with our conditional detections
    code_pos cur = start;
    ++cur;
    while (cur != end)
    {
        if (cur->get_opcode() == Opcode::BNT)
        {
            code_pos target = cur->get_branch_target();
            if (target->get_opcode() == Opcode::BT)
            {
                ++target;
                cur->set_branch_target(target, cur->is_forward_branch());
            }
        }
        else if (cur->get_opcode() == Opcode::BT)
        {
            code_pos target = cur->get_branch_target();
            if (target->get_opcode() == Opcode::BNT)
            {
                ++target;
                cur->set_branch_target(target, cur->is_forward_branch());
            }
        }
        ++cur;
    }
}

// A pattern we see (e.g. ScriptSync in SQ4) is the following:
//  bnt LoopHead
//  jmp LoopExit
//  Jmp LoopHead
// This confuses us, and as long as no one jumps to the 2nd instruction, we can restructure it like:
//  bnt A
//  jmp LoopExit
//A:jmp LoopHead
// Which will cause us to identify this as a loop with no tail condition, which ends in an (if (foo) break)
void _FixupConfusingBranches(code_pos start, code_pos end, const std::string &statusPrefix, IDecompilerResults &results)
{
    // First, track all branch targets
    vector<uint16_t> branchTargets;
    code_pos cur = start;
    while (cur != end)
    {
        if (cur->_is_branch_instruction())
        {
            branchTargets.push_back(cur->get_branch_target()->get_final_offset_dontcare());
        }
        ++cur;
    }

    cur = start;
    while (cur != end)
    {
        if (cur->is_conditional_branch_instruction() && cur->get_branch_target()->get_final_offset_dontcare() < cur->get_final_offset_dontcare())
        {
            uint16_t loopHead = cur->get_branch_target()->get_final_offset_dontcare();
            // We have a bt or bnt that branches backwards.
            code_pos theBranch = cur;
            ++cur;
            if (cur->get_opcode() == Opcode::JMP)
            {
                ++cur;
                if ((cur->get_opcode() == Opcode::JMP) && (cur->get_branch_target()->get_final_offset_dontcare() == loopHead))
                {
                    // This jump goes to the same place as the bnt. So turn the bnt into a forward branch.
                    theBranch->set_branch_target(cur, true);
                    results.AddResult(DecompilerResultType::Important, fmt::format("Restructured branches in {0}", statusPrefix));
                }
            }

        }
        ++cur;
    }
}

ControlFlowNode *ControlFlowGraph::_PartitionCode(code_pos start, code_pos end)
{
    // I feel this would be so much faster if code_pos were just an index into a vector of instructions
    // But apparently I made code_pos a list iterator because we insert instructions not always at the end.

    // Map from code_pos to the node that starts that guy.
    map<code_pos, RawCodeNode*> posToNode;
    // List of predecessors for a node, where the key is the "start" code_pos of the node
    map<code_pos, vector<code_pos>> predecessors;

    // First, let's break our sequence of instructions into natural boundaries based on branching instructions
    // "intervals"
    // We'll do the following:
    //  - any target of a branch indicates a boundary
    //      - this includes the subsequent instruction in a conditional branch
    //  - a ret instruction indicates a boundary (after the ret)
    //  - furthermore, the set of instructions leading up to a conditional branch calculation
    //      also forms a unit (but only if they are well-formed). This may not be essential, but
    //      it might make it easier to identify loop patterns and such.
    code_pos trueBeginning = start;
    ++trueBeginning;    // Since start is just a token guy at the front.

    posToNode[start] = MakeNode<RawCodeNode>(start);
    posToNode[trueBeginning] = MakeNode<RawCodeNode>(trueBeginning);
    code_pos prev = start;
    code_pos cur = start;
    ++cur;
    while (cur != end)
    {
        if (cur->_is_branch_instruction())
        {
            // First make splits where there are targets:
            // A branch instruction... its target will signify the beginning of a node
            code_pos target = cur->get_branch_target();
            if (posToNode.find(target) == posToNode.end())
            {
                posToNode[target] = MakeNode<RawCodeNode>(target);
            }
            // And we are a predecessor of this code_pos:
            predecessors[target].push_back(cur);

            // The next instruction will also signify a new node.
            code_pos alternative = cur;
            ++alternative;
            if (posToNode.find(alternative) == posToNode.end())
            {
                posToNode[alternative] = MakeNode<RawCodeNode>(alternative);
            }
            // However, we are not necessarily a target of it (if it's just a jump, then
            // there is no way from cur to alternative)
            if (cur->is_conditional_branch_instruction())
            {
                predecessors[alternative].push_back(cur);
            }

            // Next, we'll split at the boundary between the instructinos leading up to the
            // branch and those before.
            // REVIEW: Without doing this, two nested if statements with instructions in between would be mis-identified
            // as a compound condition.
            if (cur->is_conditional_branch_instruction())
            {
                code_pos beginningOfBranchInstructionSequence;
                if (_ObtainInstructionSequence(cur, trueBeginning, beginningOfBranchInstructionSequence))
                {
                    if (posToNode.find(beginningOfBranchInstructionSequence) == posToNode.end())
                    {
                        posToNode[beginningOfBranchInstructionSequence] = MakeNode<RawCodeNode>(beginningOfBranchInstructionSequence);
                    }
                    code_pos pred = beginningOfBranchInstructionSequence;
                    --pred;
                    if (!IsNotAllowedAsPredecessor(pred))
                    {
                        predecessors[beginningOfBranchInstructionSequence].push_back(pred);
                    }
                }
            }
        }
        else if (cur->get_opcode() == Opcode::RET)
        {
            // TODO. This may not be necessary, as someone will always jump to right after the RET.
            // So we could write a loop to assert this afterwards.
        }
        else if (cur->get_opcode() == Opcode::TOSS)
        {
            // Our switch detection relies on breaking after a TOSS in one case: When one switch
            // immediately follows another. If we didn't break after the TOSS, then the tail of the
            // first would overlap with the header of the second.
            // So let's break after here.
            // The next instruction will also signify a new node.
            code_pos afterTOSS = cur;
            ++afterTOSS;
            if (posToNode.find(afterTOSS) == posToNode.end())
            {
                posToNode[afterTOSS] = MakeNode<RawCodeNode>(afterTOSS);
            }
        }
        prev = cur;
        ++cur;
    }

    // CFGNodes have their "start" filled in, but not their end (or predecessors)
    // Let's do that noe
    for (auto &pair : posToNode)
    {
        code_pos cur = pair.first;
        assert(pair.first == pair.second->start);
        code_pos prev = cur;
        ++cur;
        while (cur != end)
        {
            if (posToNode.find(cur) != posToNode.end())
            {
                // This is the begining of another node, so it must be the end of this node.
                // So we have found the end. However, we should also check that the found node
                // has us as one of its predecessors. This would have only happened if we encounted
                // a branch instruction here.
                std::vector<code_pos> &predsForFoundNode = predecessors[cur];
                if (prev->get_opcode() != Opcode::JMP)
                {
                    if (!prev->_is_branch_instruction())
                    {
                        predsForFoundNode.push_back(prev);
                    }
                    else
                    {
                        assert(find(predsForFoundNode.begin(), predsForFoundNode.end(), prev) != predsForFoundNode.end());
                    }
                }

                break;
            }
            prev = cur;
            ++cur;
        }
        pair.second->end = cur;
    }

    // Now CFGNodes have their begin and end filled in. All that remains is that we fill in the predecessors.
    for (auto &pair : posToNode)
    {
        for (code_pos predecessorPos : predecessors[pair.first])
        {
            // We are given the predecessor code_pos. We need to find ControlFlowNode that contains it.
            // Probably not the most efficient, but we can walk backward until we find a mapping from
            // this code_pos to a ControlFlowNode. We know that at least the start will have one, so we'll
            // find one eventually.
            code_pos cur = predecessorPos;
            auto curIt = posToNode.find(cur);
            while (curIt == posToNode.end())
            {
                --cur;
                curIt = posToNode.find(cur);
            }
            pair.second->InsertPredecessor(curIt->second);
        }
    }

    // Now we should be able to generate a set of nodes based on posToNode.
    ControlFlowNode *headerNode = posToNode[start];
    NodeSet finalNodes;
    for (auto &pair : posToNode)
    {
        // Some nodes represent inaccessible code (e.g. final ret in proc990_0 in SQ5). Don't add them.
        if ((pair.second->Successors().size() != 0) || (pair.second->Predecessors().size() != 0))
        {
            finalNodes.insert(pair.second);
        }
    }

    // Make a node to encompass everything:
    mainStructure = MakeStructuredNode<MainNode>();
    mainStructure->SetChildren(finalNodes);
    (*mainStructure)[SemId::Head] = headerNode;
#if SQ5_990_ISSUE
    ControlFlowNode *nodeWithLastCodePos = nullptr;
#endif
    // Find the tail node...
    for (ControlFlowNode *possibleTailNode : finalNodes)
    {
#if SQ5_990_ISSUE
        if (possibleTailNode->GetStartingAddress() != 0xffff)
        {
            if (!nodeWithLastCodePos || (possibleTailNode->GetStartingAddress() > nodeWithLastCodePos->GetStartingAddress()))
            {
                nodeWithLastCodePos = possibleTailNode;
            }
        }
#endif
        if (possibleTailNode->Successors().empty())
        {
            (*mainStructure)[SemId::Tail] = possibleTailNode;
            break;
        }
    }

#if SQ5_990_ISSUE
    // The function might end in a loop (see proc990_0 in SQ5), so if there is no tail, then use the one with the
    // highest starting address
    // REVIEW: There is still a problem here. We are essentially broken when the only exit from a loop is a ret instruction.
    // Our whole follow logic doesn't work in that case. Addressing the issue this way just causes a cascade of errors.
    if (!(*mainStructure)[SemId::Tail])
    {
        (*mainStructure)[SemId::Tail] = nodeWithLastCodePos;
    }
#endif
    assert((*mainStructure)[SemId::Tail]);
    ExitNode *exitNode = MakeNode<ExitNode>();
    exitNode->InsertPredecessor(mainStructure);  // NOT main's tail
    (*mainStructure)[SemId::Follow] = exitNode;

    return mainStructure;
}

void _PruneDegenerateNodes(ControlFlowNode *structure)
{
    // This can happen with some kinds of code generate, where, for example,
    // we have two jmp instructions in a row (e.g. a break directly in a while loop)
    // This causes multiple entry points to our tree, and messes us up. Remove them.
    ControlFlowNode *head = (*structure)[SemId::Head];
    bool changes = true;
    while (changes)
    {
        changes = false;
        for (ControlFlowNode *child : structure->Children())
        {
            if ((child != head) && (child->Predecessors().size() == 0))
            {
                NodeSet degenSuccessorsCopy = child->Successors();
                for (ControlFlowNode *degenSuccessor : degenSuccessorsCopy)
                {
                    degenSuccessor->ErasePredecessor(child);
                }
                assert(child->Successors().size() == 0);
                structure->EraseChild(child);
                changes = true;
                break;
            }
        }
    }
}

bool _DoNothing(ControlFlowGraph &cfg, ControlFlowNode &parent, vector<NodeBlock> &blocks)
{
    return true;
}

bool ControlFlowGraph::Generate(code_pos start, code_pos end)
{
    try
    {
        _RepairBranches(start, end);
        _FixupConfusingBranches(start, end, _contextName, _decompilerResults);

        bool showFile = _debug && (!_pszDebugFilter || PathMatchSpec(_contextName.c_str(), _pszDebugFilter));

        if (showFile)
        {
            int x = 0;
        }

        ControlFlowNode *main = _PartitionCode(start, end);

        // I had removed this because it prevented loop break detection. But I think I need it
        // back (e.g LB, script 255). I forget what problems it caused unfortunately (I changed the way I identify loop breaks,
        // so maybe it causes no problems). The nodes being pruned really have no chance of affecting code.
        _PruneDegenerateNodes(main);

        DominatorMap dominators = GenerateDominators(main, (*main)[SemId::Head]);

        if (showFile)
        {
            CFGVisualize(_contextName + "_raw", discoveredControlStructures);
        }

        // Yeah, we're calculating dominators a second time here, but that's ok.
        // The above case is only for visualization.

        if (!_decompilerResults.IsAborted())
        {
            _FindAllStructuresOf(_FindBackEdges, _CheckForSameHeader, _ProcessNaturalLoop);
        }
        if (!_decompilerResults.IsAborted())
        {
            _FindAllStructuresOf(_FindSwitchBlocks, _DoNothing, _ProcessSwitch);
        }

        if (!_decompilerResults.IsAborted())
        {
            _FindAllCompoundConditions();
        }
        if (!_decompilerResults.IsAborted())
        {
            _DoLoopTransforms();
        }
        if (!_decompilerResults.IsAborted())
        {
            _RestructureBreaks();
        }
        if (!_decompilerResults.IsAborted())
        {
            _ResolveBreaks();
        }

        // TODO: Move back to after if
        if (showFile)
        {
            CFGVisualize(_contextName + "_loop", discoveredControlStructures);
        }


        if (!_decompilerResults.IsAborted())
        {
            _FindAllIfStatements();
        }
    }
    catch (ControlFlowException &e)
    {
        // For now we'll consider these warnings?
        if (e.node)
        {
            uint16_t address = e.node->GetStartingAddress();
            string message = fmt::format("{0}: {1}: {2} at {3:04x}", _statusMessagePrefix, e.message, e.node->Type, address);
            _decompilerResults.AddResult(DecompilerResultType::Warning, message);
        }
        else
        {
            _decompilerResults.AddResult(DecompilerResultType::Warning, e.message);
        }
        return false;
    }
    return true;
}
