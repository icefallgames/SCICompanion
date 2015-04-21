#include "stdafx.h"
#include "scii.h"
#include "ControlFlowGraph.h"
#include "DecompilerCore.h"
#include "CFGNode.h"
#include "TarjanAlgorithm.h"
#include "ControlFlowGraphViz.h"
#include "StlUtil.h"

#define VISUALIZE_FLOW 1

using namespace sci;
using namespace std;

bool EndsWithConditionalBranch(CFGNode *node)
{
    return node->Type == CFGNodeType::CompoundCondition ||
        node->endsWith(Opcode::BNT) ||
        node->endsWith(Opcode::BT);
}

template<class T> inline
bool contains(const T& container, const typename T::reference value)
{
    return container.find(value) != container.end();
}

NodeSet _CollectNodesBetween(CFGNode *head, CFGNode *tail)
{
    NodeSet collection;
    collection.insert(head);

    stack<CFGNode*> leStack;
    leStack.push(tail);
    while (!leStack.empty())
    {
        // I hate that it's two lines of code to pop from a std::stack
        CFGNode *d = pop_ptr(leStack);
        if (collection.find(d) == collection.end())
        {
            collection.insert(d);
            // Add all predecessors of b, as long as they are not head.
            for (CFGNode *pred : d->Predecessors())
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

// A back edge is smaller if it is contained within another.
NodeBlock::NodeBlock(CFGNode *head, CFGNode *followNode, bool includeFollow, CFGNode *extraData) : head(head), latch(followNode), extraData(extraData)
{
    body = _CollectNodesBetween(head, followNode);
    if (extraData)
    {
        // This is such a hack.
        NodeSet extraNodes = _CollectNodesBetween(head, extraData);
        for (CFGNode *extra : extraNodes)
        {
            body.insert(extra);
        }
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
        assert(B.latch == A.latch);
        return 0;
    }
    // Disjoint, so they are equal
    return 0;
}

void RemoveSetFromSet(NodeSet &superSet, NodeSet &subset)
{
    for (CFGNode *node : subset)
    {
        superSet.erase(node);
    }
}

CFGNode *ControlFlowGraph::_EnsureExitNode(NodeSet &existingExitNodes, CFGNode *exitNodePredecessor, CFGNode *exitNodeSuccessor)
{
    uint16_t exitAddress = exitNodeSuccessor->GetStartingAddress();
    CFGNode *reuseExitNode = nullptr;
    for (CFGNode *existingExitNode : existingExitNodes)
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
CFGNode *ControlFlowGraph::_ReplaceIfStatementInWorkingSet(CFGNode *structure, CFGNode *ifHeader, CFGNode *ifFollowNode)
{
    // The if statement is defined by header and followNode
    // Follow node is not included in the if statement (since it may be used by many if statements)
    // We *should* be guaranteed that the nodes we replace aren't part of any other if statement.

    // To gather the nodes, we'll look at the two branchess of ifHeader, and follow things until we hit the follow node
    // This will represent the set of nodes included in the if statement.
    // At this point, we can probably assert that each node we encounter (other than the header) only has a
    // single successor. There shouldn't be any more branching at this point, other than the if statements
    // that we are collected. And we go from the most nested to the least.
    CFGNode *ifNode = MakeStructuredNode<IfNode>();
    ExitNode *exitPoint = MakeNode<ExitNode>();
    exitPoint->startingAddressForExit = ifFollowNode->GetStartingAddress();
    (*ifNode)[SemId::Head] = ifHeader;
    // No tail for if node
    stack<CFGNode*> toProcess;
    toProcess.push(ifHeader);
    NodeSet exitPredecessors;
    while (!toProcess.empty())
    {
        CFGNode *node = pop_ptr(toProcess);
        assert(ifNode->children.find(node) == ifNode->children.end());  // Should only encounter things ONCE in the 1 or two if paths
        ifNode->children.insert(node);
        for (CFGNode *succ : node->Successors())
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
    assert(exitPoint->Predecessors().size() == 2);

    // Now remove the children from the parent structure
    for (CFGNode *ifChild : ifNode->children)
    {
        assert(structure->MaybeGet(SemId::Follow) != ifChild); // This should NEVER happen, since we don't include the follow node in the if node.
        structure->children.erase(ifChild);
    }
    ifNode->children.insert(exitPoint);
    structure->NotifyChildrenReplacedBy(ifNode);

    // The header's predecessors become the if's predecessors, and the header has no predeceesor
    ifNode->AssignPredecessors(ifHeader->Predecessors());
    ifHeader->ClearPredecessors();

    // The follow node should have the ifNode as its predecessor, and not any of the if node's children
    NodeSet followNodePreds = ifFollowNode->Predecessors();
    for (CFGNode *followPred : followNodePreds)
    {
        if (ifNode->children.find(followPred) != ifNode->children.end())
        {
            ifFollowNode->ErasePredecessor(followPred);
        }
    }
    ifFollowNode->InsertPredecessor(ifNode);

    structure->children.insert(ifNode);

    // We can determine information about if it's just an if, or and if-else, simply by
    // seeing if one of the header node's 2 Successors() is the exit point.
    CFGNode *thenNode, *elseNode;
    GetThenAndElseBranches(ifHeader, &thenNode, &elseNode);
    (*ifNode)[SemId::Then] = thenNode;
    (*ifNode)[SemId::Else] = elseNode;

    return ifNode;
}

// If someone had a follow node that is inside newNode's children, the follow node should now be
// set to newNode - as long as that someone isn't a child of newNode.
// I haven't hit this scenario yet, but it seems plausible.
void ControlFlowGraph::_ReplaceNodeInFollowNodes(CFGNode *newNode)
{
    for (CFGNode *structure : discoveredControlStructures)
    {
        if (newNode->children.find(structure) == newNode->children.end())
        {
            // structure is NOT a child of new node. That's important, because otherwise the follow node of
            // structure would essentially be set to struture's parent.
            CFGNode *oldFollowNode = structure->MaybeGet(SemId::Follow);
            if (oldFollowNode && (newNode->children.find(oldFollowNode) != newNode->children.end()))
            {
                (*structure)[SemId::Follow] = newNode;
                // Let's also assert that the followNode was the head of newNode
                assert(oldFollowNode == (*newNode)[SemId::Head]);
            }
        }
    }
}

// newNode must have its head and tail set, and containedNodes must be the new children of newNode (including head and tail)
// newNode will be assigned the chidren in containedNodes
// dominators will be assigned to the new dominators for the workingSetNodes.
void ControlFlowGraph::_ReplaceNodeInWorkingSet(CFGNode *parent, CFGNode *newNode)
{
    CFGNode *existingSingleExit = newNode->MaybeGet(SemId::Tail);

    // Given a new node, make it "contain" the containedNodes.
    // Remove the contained nodes from the nodesWorkingSet
    // Update node predecessors and dominators so that everything is ship shape.
    RemoveSetFromSet(parent->children, newNode->children);

    // REVIEW: We also need to deal with constructs that stash their own things, head and tail.
    // head and tail are basically duplicate state, which sucks. Maybe we should reconsider have head and tail?
    // REVIEW: If we had node subclasses, they could each be responsible for updating their various nodes.
    parent->NotifyChildrenReplacedBy(newNode);

    NodeSet exitPoints;

    // The easiest way to update the dominators is to just recalculate them entirely (possibly expensive though).
    // That way, we just need to update the predecessors
    // For any node in the working set, if they have as one of their predecessors a node in conatinedNodes,
    // then remove that node and replace it with newNode.
    for (CFGNode *workingSetNode : parent->children)
    {
        NodeSet nodesToBeRemoved;
        set_intersection(workingSetNode->Predecessors().begin(), workingSetNode->Predecessors().end(),
            newNode->children.begin(), newNode->children.end(),
            inserter(nodesToBeRemoved, nodesToBeRemoved.begin()));
        if (!nodesToBeRemoved.empty())
        {
            for (CFGNode* eraseMe : nodesToBeRemoved)
            {
                // an outside node had a newNode child as its predecssor... 
                workingSetNode->ErasePredecessor(eraseMe);

                // Now we need to ensure that we have an exit point. To avoid linking the exit nodes to the outside
                // (which is a possibility, but then our graph traversal algorithms would need to be exitpoint-aware),
                // we'll give the exit node an address that it exits to
                if (!existingSingleExit)
                {
                    newNode->children.insert(_EnsureExitNode(exitPoints, eraseMe, workingSetNode));
                }
            }
            // and redirect the outside node's predecessor to be the newNode
            workingSetNode->InsertPredecessor(newNode);
        }
    }

    // This new nodes predecessors should be the same as the predecessors of its head that are
    // outside our control structure
    for (CFGNode *predNode : (*newNode)[SemId::Head]->Predecessors())
    {
        if (newNode->children.find(predNode) == newNode->children.end())
        {
            newNode->InsertPredecessor(predNode);
        }
    }

    // Now actually insert ourselves
    parent->children.insert(newNode);

    // Now remove any predecessors from the loopNode's head that are not in its children. We don't the loops nodes attached to the
    // outside world. They loopnode itself is the one that is now attached to the outside work.
    // Then we'll also add a token entry node.
    NodeSet predecessorsCopy = (*newNode)[SemId::Head]->Predecessors();
    for (CFGNode *pred : predecessorsCopy)
    {
        if (newNode->children.find(pred) == newNode->children.end())
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
        CFGNode *singleExit = *exitPoints.begin();
        CFGNode *currentTail = newNode->MaybeGet(SemId::Tail);
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

CFGNode *_GetFirstPredecessorOrNull(CFGNode *node)
{
    return node->Predecessors().empty() ? nullptr : *node->Predecessors().begin();
}

void ControlFlowGraph::_IdentifySwitchCases(CFGNode *switchNodeIn)
{
    CFGNode *switchNode = static_cast<SwitchNode*>(switchNodeIn);
    CFGNode *toss = (*switchNode)[SemId::Tail];
    // Find the first case. It should be right after the head's only successor
    CFGNode *switchHead = (*switchNode)[SemId::Head];
    assert(switchHead->Successors().size() == 1);
    CFGNode *firstCase = *switchHead->Successors().begin();
    assert(firstCase->startsWith(Opcode::DUP) && firstCase->endsWith(Opcode::BNT));
    // Discover all the cases.
    NodeSet caseHeads;
    CFGNode *currentCase = firstCase;
    while (currentCase->startsWith(Opcode::DUP) && currentCase->endsWith(Opcode::BNT))
    {
        caseHeads.insert(currentCase);
        currentCase->Tags.insert(SemanticTags::CaseCondition);
        // Find where this guy branches
        CFGNode *thenNode, *elseNode;
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
    DominatorMap dominators = GenerateDominators(switchNodeIn->children, switchHead);
    NodeSet caseNodes;
    for (CFGNode *caseHead : caseHeads)
    {
        CFGNode *newCaseNode = MakeStructuredNode<CaseNode>();
        (*newCaseNode)[SemId::Head] = caseHead;

        newCaseNode->children.insert(caseHead);
        // Find a statement that will dominate all others in the case. For the default,
        // this is the case head. For other cases, it's the first statement after the case head
        CFGNode *firstStatementInCase, *elseNode;
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
        for (CFGNode *tossPred : toss->Predecessors())
        {
            if (dominators.IsADominatedByB(tossPred, firstStatementInCase))
            {
                // Collect the items here
                NodeSet newChildren = CollectNodesBetween(firstStatementInCase, tossPred, switchNodeIn->children);
                newCaseNode->children.insert(newChildren.begin(), newChildren.end());
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
            for (CFGNode *exitPoint : exitPoints)
            {
                singleExitNode->InsertPredecessor(exitPoint);
            }
            (*newCaseNode)[SemId::Tail] = singleExitNode;
            newCaseNode->children.insert(singleExitNode);
        }
        caseNodes.insert(newCaseNode);
    }

    // Now replace them in the graph
    // Don't use _ReplaceNodeInWorkingSet, because we'll be very deliberate about how the cases connect to the switch
    toss->ClearPredecessors();
    assert(switchHead->Successors().size() == 1);   // It won't after this though:
    for (CFGNode *newCaseNode : caseNodes)
    {
        (*newCaseNode)[SemId::Head]->ClearPredecessors();
        toss->InsertPredecessor(newCaseNode);
        newCaseNode->InsertPredecessor(switchHead);
        RemoveSetFromSet(switchNode->children, newCaseNode->children);
        switchNode->children.insert(newCaseNode);
        // REVIEW: Is there more to do with semantic nodes?
    }
}

bool HasPrecedessorInSet(CFGNode *node, NodeSet &theSet)
{
    for (CFGNode *pred : node->Predecessors())
    {
        if (theSet.find(pred) != theSet.end())
        {
            return true;
        }
    }
    return false;
}

CFGNode *ControlFlowGraph::_FindFollowNodeForStructure(CFGNode *structure)
{
    CFGNode *follow = nullptr;
    // Find the follow node. This isn't trivial. We can't just ask for the post dominator of the
    // header - that won't work in the case of compound conditions (which are still unresolved at this point)
    // We can leverage a tautology of SCI compilers - code always proceeds forward in address. So we can
    // find the furthest branch of any of our children, then try to find the node that matches it.

    stack<CFGNode*> toProcess;
    toProcess.push(structure);
    uint16_t maxAddress = 0;
    //uint16_t maxAddressDebug = 0;
    while (!toProcess.empty())
    {
        CFGNode *node = pop_ptr(toProcess);
        if (node->Type == CFGNodeType::RawCode)
        {
            scii lastInstruction = node->getLastInstruction();
            //if (lastInstruction.is_branch_op())
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
            for (CFGNode *child : node->children)
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
        CFGNode *node = pop_ptr(toProcess);
        if (node->GetStartingAddress() == maxAddress)
        {
            if (HasPrecedessorInSet(node, structure->children))
            {
                follow = node;
                break;
            }
        }
        for (CFGNode *child : node->children)
        {
            toProcess.push(child);
        }
    }

    assert(follow);
    return follow;
}

void CollectMoreChildren(CFGNode *structure, const DominatorMap &dominators)
{
    CFGNode *followNode = (*structure)[SemId::Follow];
    CFGNode *head = (*structure)[SemId::Head];
    stack<CFGNode*> toProcess;
    toProcess.push(followNode);
    while (!toProcess.empty())
    {
        CFGNode *node = pop_ptr(toProcess);
        for (CFGNode *pred : node->Predecessors())
        {
            const NodeSet &predDoms = dominators.at(pred);
            if ((pred != head) && (predDoms.find(head) != predDoms.end()))
            {
                // It's a predecessor that's dominated by the structure header. It's definitely
                // one of its children. Avoid needless processing by only adding it if it's not already
                // in children
                if (structure->children.find(pred) == structure->children.end())
                {
                    structure->children.insert(pred);
                    toProcess.push(pred);
                }
            }
        }
    }
}

// Returns the loop node, already replaced in the parent's chidlren
CFGNode *ControlFlowGraph::_ProcessNaturalLoop(ControlFlowGraph &loopDetection, CFGNode *parent, const NodeBlock &backEdge)
{
    LoopNode *loopNode = loopDetection.MakeStructuredNode<LoopNode>();
    loopNode->children = backEdge.body;
    (*loopNode)[SemId::Head] = backEdge.head;           // This is the start, or the branch
    (*loopNode)[SemId::Latch] = backEdge.latch;         // This is the latch node... could be multiple ones though.

    CFGNode *followNode = loopDetection._FindFollowNodeForStructure(loopNode);
    (*loopNode)[SemId::Follow] = followNode;
    // Now let's collect more children. Any predecessors of the child who are dominated by our header node should be included.
    // This will collect things like breaks, etc... that were not collected when we went from the latch node to the header to
    // collect children.
    DominatorMap dominators = GenerateDominators(parent->children, (*parent)[SemId::Head]);
    CollectMoreChildren(loopNode, dominators);

    loopDetection._ReplaceNodeInWorkingSet(parent, loopNode);
    loopDetection._ReplaceNodeInFollowNodes(loopNode);

    // Also assert that we have a tail that is an exit point. If not, it might mean we think we have >1 exits.
    // This could be indicative of a goto (or mid-identified break)
    CFGNode *tail = loopNode->MaybeGet(SemId::Tail);
    assert(tail && (tail->Type == CFGNodeType::Exit));

    return loopNode;
}

CFGNode *ControlFlowGraph::_ProcessSwitch(ControlFlowGraph &loopDetection, CFGNode *parent, NodeBlock &switchBlock)
{
    // Reconstruct the nodes in switchBlock into a switch statement, and replace it in the parent's children.
    SwitchNode *switchNode = loopDetection.MakeStructuredNode<SwitchNode>();
    (*switchNode)[SemId::Tail] = switchBlock.latch;    // Note: This is the tail, not actually a latch node.
    (*switchNode)[SemId::Head] = switchBlock.head;
    switchNode->children = switchBlock.body;

    // Now, in the working set, replace the guys that are part of the switch with a single switch node:
    loopDetection._ReplaceNodeInWorkingSet(parent, switchNode);
    loopDetection._ReplaceNodeInFollowNodes(switchNode);

    // Now of course, we need to break down our cases
    loopDetection._IdentifySwitchCases(switchNode);

    return switchNode;
}

vector<NodeBlock> ControlFlowGraph::_FindSwitchBlocks(DominatorMap &dominators, DominatorMap &postDominators, CFGNode *structure)
{
    CFGNode *header = (*structure)[SemId::Head];
    NodeSet &allNodes = structure->children;
    vector<NodeBlock> switchBlocks;
    for (CFGNode *maybeToss : allNodes)
    {
        // Switches don't necessarily have a "pure" shape in a control flow graph like loops do.
        // In SCI, switches finish with a TOSS instruction, so let's specifically look for that.
        if (maybeToss->startsWith(Opcode::TOSS))
        {
            auto &tossDominators = dominators.at(maybeToss);
            CFGNode *pred = _GetFirstPredecessorOrNull(maybeToss);
            while (pred)
            {
                if (tossDominators.find(pred) != tossDominators.end())
                {
                    // We found it. This pred node dominates the toss. Assert that there's DUP instruction in here.
                    // If we find that in some cases there isn't, we'll need to adjust our algorithm.
                    assert(pred->contains(Opcode::DUP));

                    // Now, the thing we're duping comes from the previous node. But the previous node
                    // might actually contain a whole sequence of instructions, only the last of which
                    // is important for the switch statement. Maybe that's ok though. As long as we're
                    // aware that it's not *just* the value that's being switched on.
                    assert(pred->Predecessors().size() == 1); // Otherwise I don't know what we'd do. Ternary?
                    CFGNode *switchHead = _GetFirstPredecessorOrNull(pred);
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
    for (CFGNode *structure : controlStructuresCopy)
    {
        _FindCompoundConditions(structure);
    }
}

    // We want to navigate down the hierarchy of loops (stopping at any loop)
    // and look for nodes that end in unconditional jumps to the loop follow.
    // Then, we'll mark them at "break", and reconnect them to the subsequent node in
    // memory (that is a sibling of the current structure)
void ControlFlowGraph::_ResolveBreaks()
{
    for (CFGNode *structure : discoveredControlStructures)
    {
        if (structure->Type == CFGNodeType::Loop)
        {
            _ResolveBreak((*structure)[SemId::Follow]->GetStartingAddress(), structure);
        }
    }
}

void ControlFlowGraph::_ResolveBreak(uint16_t loopFollowAddress, CFGNode *structure)
{
    // We're looking for nodes that end in an unconditional jump to the loop follow address
    for (CFGNode *node : structure->children)
    {
        if ((node->Type == CFGNodeType::RawCode) && node->endsWith(Opcode::JMP))
        {
            scii inst = node->getLastInstruction();
            // Conveniently, end provides us with the address of the next instructino.
            code_pos end = static_cast<RawCodeNode*>(node)->end;
            if (inst.get_branch_target()->get_final_offset() == loopFollowAddress)
            {
                node->Tags.insert(SemanticTags::LoopBreak);
                _ReconnectBreakNodeToSubsequentCode(structure, node, end->get_final_offset());
            }
        }
    }

    // Now recurse, as long as its not another loop
    for (CFGNode *child : structure->children)
    {
        if (child->Type != CFGNodeType::Loop)
        {
            _ResolveBreak(loopFollowAddress, child);
        }
    }
}

void ControlFlowGraph::_ReconnectBreakNodeToSubsequentCode(CFGNode *structure, CFGNode *breakNode, uint16_t subsequentInstructionAddress)
{
    assert(breakNode->Successors().size() == 1);
    CFGNode *currentSuccessor = *breakNode->Successors().begin();
    currentSuccessor->ErasePredecessor(breakNode);
    // Now find the node in structure that has the right starting address
    // We should find it in the this control structure. It may be an exit node. If we don't find
    // it, then we probably need to add an exit node.
    for (CFGNode *child : structure->children)
    {
        if (child->GetStartingAddress() == subsequentInstructionAddress)
        {
            child->InsertPredecessor(breakNode);
            return;
        }
    }
    assert(false && "Unable to re-connect break statement. Need to add exit node to control structure?");
}

void ControlFlowGraph::_DoLoopTransforms()
{
    for (CFGNode *structure : discoveredControlStructures)
    {
        if (structure->Type == CFGNodeType::Loop)
        {
            _DoLoopTransform(structure);
        }
    }
}

void ControlFlowGraph::_DoLoopTransform(CFGNode *loop)
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
    CFGNode *latch = (*loop)[SemId::Latch];
    if ((latch->Type == CFGNodeType::RawCode) && latch->startsWith(Opcode::JMP))
    {
        if (latch->Predecessors().size() == 1)
        {
            CFGNode *branchNode = *latch->Predecessors().begin();
            CFGNode *otherBranchDestination = GetOtherBranch(branchNode, latch);
            if (otherBranchDestination) // A while might not have one
            {
                // And if it did, it wouldn't be an exit node
                if (otherBranchDestination->Type == CFGNodeType::Exit)
                {
                    uint16_t address1 = otherBranchDestination->GetStartingAddress();
                    uint16_t address2 = (*loop)[SemId::Follow]->GetStartingAddress();
                    assert(address1 == address2);
                    CFGNode *invert = MakeStructuredNode<InvertNode>();
                    invert->children.insert(latch);
                    invert->children.insert(branchNode);
                    (*invert)[SemId::Head] = branchNode;    // We just need to invert this
                    _ReplaceNodeInWorkingSet(loop, invert);
                    _ReplaceNodeInFollowNodes(invert);

                    // And the exit should have the new invert as a predecessor
                    assert(contains(otherBranchDestination->Predecessors(), invert));
                }
            }
        }
    }
}

void ControlFlowGraph::_FindCompoundConditions(CFGNode *structure)
{
    bool changesMade = true;
    while (changesMade)
    {
        changesMade = false;
        stack<CFGNode*> toVisit;
        NodeSet visited;
        toVisit.push((*structure)[SemId::Head]);
        visited.insert((*structure)[SemId::Head]);
        while (!toVisit.empty())
        {
            CFGNode *possibleStart = pop_ptr(toVisit);
            // This shouldn't result in an infinite loop, since this should be an acyclic graph at this point.
            // Hmm nope, because I do this inside of while loops... although perhaps I should have removed the loop
            // cyckes?
            for (CFGNode *successor : possibleStart->Successors())
            {
                if ((successor->Type != CFGNodeType::Exit) && (visited.find(successor) == visited.end()))
                {
                    toVisit.push(successor);
                    visited.insert(successor);
                }
            }

            if (possibleStart->Successors().size() == 2)
            {
                // Two way node. Check to see if one of these ends in a branch
                auto it = possibleStart->Successors().begin();
                auto itNext = it;
                itNext++;
                CFGNode *branchNode = nullptr;
                CFGNode *other = nullptr;
                if (EndsWithConditionalBranch(*it))
                {
                    // Does the branch node only have a single successor that is the start node?
                    CFGNode *possibleBranch = *it;
                    if ((possibleBranch->Predecessors().size() == 1) && (*possibleBranch->Predecessors().begin() == possibleStart))
                    {
                        branchNode = possibleBranch;
                        other = *itNext;
                    }
                }

                if (!branchNode && EndsWithConditionalBranch(*itNext))
                {
                    // Does the branch node only have a single successor that is the start node?
                    CFGNode *possibleBranch = *itNext;
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
                    if (branchNode->Successors().find(other) != branchNode->Successors().end())
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
                        CFGNode *firstThenNode, *firstElseNode, *secondThenNode, *secondElseNode;
                        GetThenAndElseBranches(possibleStart, &firstThenNode, &firstElseNode);
                        GetThenAndElseBranches(branchNode, &secondThenNode, &secondElseNode);
                        CompoundConditionNode *newConditionNode = nullptr;
                        if (firstElseNode == secondElseNode)
                        {
                            assert(firstThenNode == branchNode);
                            // This is (X and Y)
                            newConditionNode = MakeStructuredNode<CompoundConditionNode>(ConditionType::And);
                            newConditionNode->isFirstTermNegated = false;
                            newConditionNode->thenBranch = secondThenNode->GetStartingAddress();
                        }
                        else if (firstThenNode == secondThenNode)
                        {
                            assert(firstElseNode == branchNode);
                            // This is (X or Y)
                            newConditionNode = MakeStructuredNode<CompoundConditionNode>(ConditionType::Or);
                            newConditionNode->isFirstTermNegated = false;
                            newConditionNode->thenBranch = firstThenNode->GetStartingAddress();
                        }
                        else if (firstElseNode == secondThenNode)
                        {
                            assert(firstThenNode == branchNode);
                            // This is (!X or Y);
                            newConditionNode = MakeStructuredNode<CompoundConditionNode>(ConditionType::Or);
                            newConditionNode->isFirstTermNegated = true;
                            newConditionNode->thenBranch = secondThenNode->GetStartingAddress();
                        }
                        else if (firstThenNode == secondElseNode)
                        {
                            assert(firstElseNode == branchNode);
                            // This is (!X and Y)
                            newConditionNode = MakeStructuredNode<CompoundConditionNode>(ConditionType::And);
                            newConditionNode->isFirstTermNegated = true;
                            newConditionNode->thenBranch = secondThenNode->GetStartingAddress();
                        }

                        (*newConditionNode)[SemId::Head] = possibleStart;
                        (*newConditionNode)[SemId::First] = possibleStart;
                        (*newConditionNode)[SemId::Second] = branchNode;
                        newConditionNode->children.insert(possibleStart);
                        newConditionNode->children.insert(branchNode);

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

vector<NodeBlock> _FindBackEdges(DominatorMap &dominators, DominatorMap &postDominators, CFGNode *structure)
{
    map<CFGNode*, CFGNode*> immediatePostDominators = CalculateImmediatePostDominators(postDominators);

    CFGNode *header = (*structure)[SemId::Head];
    NodeSet &allNodes = structure->children;
    vector<NodeBlock> backEdges;
    for (CFGNode *node : allNodes)
    {
        for (CFGNode *pred : node->Predecessors())
        {
            auto &predsDoms = dominators.at(pred);
            // Does node dominate its predecessor? If so, pred -> node is a back edge
            if (predsDoms.find(node) != predsDoms.end())
            {
                backEdges.emplace_back(node, pred, true, nullptr);
            }
        }
    }
    return backEdges;
}

void _GetPostOrder(NodeSet &visited, CFGNode *node, vector<CFGNode*> &ordered)
{
    // Mark ourselves as visited
    visited.insert(node);
    // Visit children who are not visited
    for (CFGNode *succ : node->Successors())
    {
        if (visited.find(succ) == visited.end())
        {
            _GetPostOrder(visited, succ, ordered);
        }
    }
    // Then add ourselves
    ordered.push_back(node);
}

vector<CFGNode*> _GetPostOrder(CFGNode *structure)
{
    vector<CFGNode*> ordered;
    NodeSet visited;
    _GetPostOrder(visited, (*structure)[SemId::Head], ordered);
    assert(ordered.size() == structure->children.size());
    return ordered;
}

CFGNode *_FindMaxNodeDominatedByMWithTwoOrMoreInEdges(DominatorMap &dominators, const map<CFGNode*, CFGNode*> &immediateDominators, vector<CFGNode*> &postOrdered, CFGNode *m, CFGNode *dontGoBeyondThis)
{
    auto endIt = find(postOrdered.begin(), postOrdered.end(), dontGoBeyondThis);
    auto it = postOrdered.begin();
    while (it != endIt)
    {
        // Work the graph from the "bottom up" until dontGoBeyondThis
        CFGNode *possibleFollow = *it;
        if (possibleFollow->Predecessors().size() >= 2)
        {
            NodeSet &dominatorsForNode = dominators.at(possibleFollow);
            if (dominatorsForNode.find(m) != dominatorsForNode.end())
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
    for (CFGNode *structure : controlStructuresCopy)
    {
        DominatorMap dominators = GenerateDominators(structure->children, (*structure)[SemId::Head]);
        _FindIfStatements(dominators, structure);
    }
}

void ControlFlowGraph::_FindIfStatements(DominatorMap &dominators, CFGNode *structure)
{
    if ((structure->Type == CFGNodeType::CompoundCondition) || (structure->Type == CFGNodeType::Switch) || (structure->Type == CFGNodeType::Invert))
    {
        // This seems hacky. And it might interfere with ternary operators?
        return;
    }

    CFGNode *loopHeader = nullptr;
    CFGNode *loopLatch = nullptr;
    if (structure->Type == CFGNodeType::Loop)
    {
        loopHeader = (*structure)[SemId::Head];
        loopLatch = (*structure)[SemId::Latch];
    }

    vector<CFGNode*> postOrdered = _GetPostOrder(structure);
    vector<NodeBlock> ifBlocks;
    NodeSet unresolvedNodes;      // Not sure what the purpose of this is. Perhaps just to track situations where we can't resolve?
    map<CFGNode*, CFGNode*> followerOf; // header -> follower map

    map<CFGNode*, CFGNode*> immediateDominators = CalculateImmediateDominators(dominators);

    for (CFGNode *node : postOrdered)
    {
        // Note: We need to be careful not to identify case conditions as if statements. However, our CaseNodes are structured
        // so that the branch node only has a single branch. So they wont'be detected here:
        if ((node->Successors().size() == 2) && (node != loopHeader) && (node != loopLatch))
        {
            assert(!node->ContainsTag(SemanticTags::CaseCondition));
            // Look for furthest node whose immediate dominator is m and who has 2 or more in edges.
            // i.e. the "follow" node of the if.
            CFGNode *n = _FindMaxNodeDominatedByMWithTwoOrMoreInEdges(dominators, immediateDominators, postOrdered, node, node);
            if (n)
            {
                followerOf[node] = n;
                NodeSet nowResolved;
                for (CFGNode *unresolved : unresolvedNodes)
                {
                    // Note that the algorithm here just automatically resolves all unresolved guys, and
                    // assigns them a follower of n:
                    // http://www.labri.fr/perso/fleury/download/papers/binary_analysis/cifuentes96structuring.pdf
                    // This doesn't work at all. Instead, I think we need to repeat the test... so we'll do that.
                    CFGNode *nRepeat = _FindMaxNodeDominatedByMWithTwoOrMoreInEdges(dominators, immediateDominators, postOrdered, node, unresolved);
                    if (nRepeat)
                    {
                        assert(nRepeat == n);   // Otherwise we still have bugs...
                        nowResolved.insert(unresolved);
                        followerOf[unresolved] = nRepeat;
                    }
                }
                for (CFGNode *resolved : nowResolved)
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

    assert(unresolvedNodes.empty());    // Otherwise, it's a branch we can't detect?

    // So now we have a list of branch nodes and their follower. We want to replace this with a
    // new if node (excluding the follower, however). We need to do this from the most nested to the least
    // nester. We can do this by using the post-order we have already calculated.
    for (CFGNode *possibleHead : postOrdered)
    {
        auto itPair = followerOf.find(possibleHead);
        if (itPair != followerOf.end())
        {
            CFGNode *headThatWillTurnIntoIf = itPair->first;
            CFGNode *ifFollowNode = itPair->second;
            CFGNode *newIfNode = _ReplaceIfStatementInWorkingSet(structure, headThatWillTurnIntoIf, ifFollowNode);
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

CFGNode *ControlFlowGraph::_PartitionCode(code_pos start, code_pos end)
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
            // We are given the predecessor code_pos. We need to find CFGNode that contains it.
            // Probably not the most efficient, but we can walk backward until we find a mapping from
            // this code_pos to a CFGNode. We know that at least the start will have one, so we'll
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
    CFGNode *headerNode = posToNode[start];
    NodeSet finalNodes;
    for (auto &pair : posToNode)
    {
        finalNodes.insert(pair.second);
    }

    // Make a node to encompass everything:
    mainStructure = MakeStructuredNode<MainNode>();
    mainStructure->children = finalNodes;
    (*mainStructure)[SemId::Head] = headerNode;
    // Find the tail node...
    for (CFGNode *possibleTailNode : finalNodes)
    {
        if (possibleTailNode->Successors().empty())
        {
            (*mainStructure)[SemId::Tail] = possibleTailNode;
            break;
        }
    }
    assert((*mainStructure)[SemId::Tail]);
    ExitNode *exitNode = MakeNode<ExitNode>();
    exitNode->InsertPredecessor(mainStructure);  // NOT main's tail
    (*mainStructure)[SemId::Follow] = exitNode;

    return mainStructure;
}

void _PruneDegenerateNodes(CFGNode *structure)
{
    // This can happen with some kinds of code generate, where, for example,
    // we have two jmp instructions in a row (e.g. a break directly in a while loop)
    // This causes multiple entry points to our tree, and messes us up. Remove them.
    CFGNode *head = (*structure)[SemId::Head];
    bool changes = true;
    while (changes)
    {
        changes = false;
        for (CFGNode *child : structure->children)
        {
            if ((child != head) && (child->Predecessors().size() == 0))
            {
                set<CFGNode*> degenSuccessorsCopy = child->Successors();
                for (CFGNode *degenSuccessor : degenSuccessorsCopy)
                {
                    degenSuccessor->ErasePredecessor(child);
                }
                assert(child->Successors().size() == 0);
                OutputDebugString("Pruned node\n");
                structure->children.erase(child);
                changes = true;
                break;
            }
        }
    }
}

CFGNode *ControlFlowGraph::Generate(const std::string &name, code_pos start, code_pos end)
{
    _RepairBranches(start, end);

    CFGNode *main = _PartitionCode(start, end);

    // I had removed this because it prevented loop break detection. But I think I need it
    // back (e.g LB, script 255). I forget what problems it caused unfortunately (I changed the way I identify loop breaks,
    // so maybe it causes no problems). The nodes being pruned really have no chance of affecting code.
    _PruneDegenerateNodes(main);

    DominatorMap dominators = GenerateDominators(main->children, (*main)[SemId::Head]);

#ifdef VISUALIZE_FLOW
    CFGVisualize(name + "_raw", discoveredControlStructures);
#endif

    // Yeah, we're calculating dominators a second time here, but that's ok.
    // The above case is only for visualization.
    
    _FindAllStructuresOf(_FindBackEdges, _ProcessNaturalLoop);
    _FindAllStructuresOf(_FindSwitchBlocks, _ProcessSwitch);
    _FindAllCompoundConditions();
    _DoLoopTransforms();
    _ResolveBreaks();
    _FindAllIfStatements();

#ifdef VISUALIZE_FLOW
    CFGVisualize(name + "_loop", discoveredControlStructures);
#endif

    return main;
}
