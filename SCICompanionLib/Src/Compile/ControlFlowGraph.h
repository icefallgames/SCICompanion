#pragma once

#include "CFGNode.h"

struct NodeBlock
{
    NodeBlock(CFGNode *head, CFGNode *followNode, bool includeFollow, CFGNode *extraData);
    static int Compare(const NodeBlock &A, const NodeBlock &B);

    CFGNode *head;
    CFGNode *latch;
    CFGNode *extraData;
    NodeSet body;
};

class ControlFlowGraph
{
public:
    ControlFlowGraph() {}
    ControlFlowGraph(const ControlFlowGraph &src) = delete;
    ControlFlowGraph& operator=(const ControlFlowGraph &src) = delete;

    CFGNode *Generate(const std::string &name, code_pos start, code_pos end);

    const NodeSet &ControlStructures() const { return discoveredControlStructures; }
    MainNode *GetMain() const { return static_cast<MainNode*>(mainStructure); };


private:
    CFGNode *_EnsureExitNode(NodeSet &existingExitNodes, CFGNode *exitNodePredecessor, CFGNode *exitNodeSuccessor);
    CFGNode *_ReplaceIfStatementInWorkingSet(CFGNode *structure, CFGNode *ifHeader, CFGNode *ifFollowNode);
    void _ReplaceNodeInFollowNodes(CFGNode *newNode);
    void _ReplaceNodeInWorkingSet(CFGNode *parent, CFGNode *newNode);
    void _IdentifySwitchCases(CFGNode *switchNodeIn);
    void _FindAllCompoundConditions();
    void _ResolveBreaks();
    void _ResolveBreak(uint16_t loopFollowAddress, CFGNode *structure);
    void _ReconnectBreakNodeToSubsequentCode(CFGNode *structure, CFGNode *breakNode, uint16_t subsequentInstructionAddress);
    void _DoLoopTransforms();
    void _DoLoopTransform(CFGNode *loop);
    void _FindCompoundConditions(CFGNode *structure);
    void _FindAllIfStatements();
    void _FindIfStatements(DominatorMap &dominators, CFGNode *structure);
    CFGNode *_PartitionCode(code_pos start, code_pos end);
    CFGNode *_FindFollowNodeForStructure(CFGNode *structure);

    static CFGNode *_ProcessNaturalLoop(ControlFlowGraph &loopDetection, CFGNode *parent, const NodeBlock &backEdge);
    static CFGNode *_ProcessSwitch(ControlFlowGraph &loopDetection, CFGNode *parent, NodeBlock &switchBlock);
    static std::vector<NodeBlock> _FindSwitchBlocks(DominatorMap &dominators, DominatorMap &postDominators, CFGNode *structure);

    template<typename _TNode, typename... Args>
    _TNode *MakeStructuredNode(Args... args)
    {
        unique_ptr<_TNode> newNode = std::make_unique<_TNode>(args...);
        _TNode *ret = newNode.get();
        nodesOwner.push_back(move(newNode));
        ret->ArbitraryDebugIndex = debugIndex++;
        discoveredControlStructures.insert(ret);
        return ret;
    }

    template<typename _TNode, typename... Args>
    _TNode *MakeNode(Args... args)
    {
        unique_ptr<_TNode> newNode = std::make_unique<_TNode>(args...);
        _TNode *ret = newNode.get();
        nodesOwner.push_back(move(newNode));
        ret->ArbitraryDebugIndex = debugIndex++;
        return ret;
    }

    template<typename FuncFindBlocks, typename ProcessBlock>
    void _FindAllStructuresOf(FuncFindBlocks findBlocks, ProcessBlock processBlock)
    {
        // Make a copy, since we'll be modifying it (adding to it)
        NodeSet controlStructuresCopy = discoveredControlStructures;
        // The first time we call _FindAllStructuresOf, we'll just have a single discovered structure(the main one)
        // After that we'll also have whatever structures were discovered in the previous call.
        // We discover structures from the most inner to the most outer (in this particular call of the function)
        for (CFGNode *structure : controlStructuresCopy)
        {
            vector<NodeBlock> blocks;
            do
            {
                // 1) calculate dominators
                DominatorMap dominators = GenerateDominators(structure->children, (*structure)[SemId::Head]);
                assert(structure->MaybeGet(SemId::Tail));
                DominatorMap postDominators = GeneratePostDominators(structure->children, (*structure)[SemId::Tail]);

                // 2) Find the toss nodes, and follow them back to a dominator, bounding the switch statement.
                blocks = findBlocks(dominators, postDominators, structure);
                if (!blocks.empty())
                {
                    sort(blocks.begin(), blocks.end(),
                        [](const NodeBlock &one, const NodeBlock &two)
                    {
                        return NodeBlock::Compare(one, two) < 0;
                    }
                    );

                    // 3) Process all switch blocks that we can. Basically, the innermost ones that don't intersect. e,g, the ones that are "equal" to the innermost
                    // Each one we process has to be disjoint from all the other previous ones. I'm afraid we don't have a stable sort though.
                    // We can have: A == B, A == C, but B < C.
                    for (size_t i = 0; i < blocks.size(); i++)
                    {
                        NodeBlock &block = blocks[i];
                        // Compare to all previous
                        bool process = true;
                        if (i > 0)
                        {
                            for (size_t iPrev = 0; process && (iPrev < i); iPrev++)
                            {
                                process = (0 == NodeBlock::Compare(block, blocks[iPrev]));
                            }
                        }

                        if (process)
                        {
                            processBlock(*this, structure, block);
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                // 4) Repeat until no new switches found
            } while (!blocks.empty());
        }
    }


    std::vector<std::unique_ptr<CFGNode>> nodesOwner;

    NodeSet discoveredControlStructures;
    CFGNode *mainStructure;


};
