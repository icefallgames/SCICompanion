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

#include "ControlFlowNode.h"
#include "DecompilerResults.h"

struct NodeBlock
{
    NodeBlock(ControlFlowNode *head, ControlFlowNode *followNode, bool includeFollow, ControlFlowNode *parentToGatherMore);
    static int Compare(const NodeBlock &A, const NodeBlock &B);

    ControlFlowNode *head;
    ControlFlowNode *latch;
    NodeSet body;
};

class ControlFlowGraph
{
public:
    ControlFlowGraph(const std::string &statusMessagePrefix, IDecompilerResults &decompilerResults, const std::string &contextName, bool debug, PCSTR pszDebugFilter) : _decompilerResults(decompilerResults), _contextName(contextName), _statusMessagePrefix(statusMessagePrefix), _debug(debug), _pszDebugFilter(pszDebugFilter) {}
    ControlFlowGraph(const ControlFlowGraph &src) = delete;
    ControlFlowGraph& operator=(const ControlFlowGraph &src) = delete;

    bool Generate(code_pos start, code_pos end);

    const NodeSet &ControlStructures() const { return discoveredControlStructures; }
    MainNode *GetMain() const { return static_cast<MainNode*>(mainStructure); };

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


private:
    void _ThrowIfAborted();
    ControlFlowNode *_EnsureExitNode(NodeSet &existingExitNodes, ControlFlowNode *exitNodePredecessor, ControlFlowNode *exitNodeSuccessor);
    ControlFlowNode *_ReplaceIfStatementInWorkingSet(ControlFlowNode *structure, ControlFlowNode *ifHeader, ControlFlowNode *ifFollowNode);
    void _ReplaceNodeInFollowNodes(ControlFlowNode *newNode);
    void _ReplaceNodeInWorkingSet(ControlFlowNode *parent, ControlFlowNode *newNode);
    void _IdentifySwitchCases(ControlFlowNode *switchNodeIn);
    void _FindAllCompoundConditions();
    void _ResolveBreaks();
    bool _ResolveBreak(uint16_t loopFollowAddress, ControlFlowNode *structure);
    void _RestructureBreaks();
    bool _RestructureBreak(uint16_t loopFollowAddress, ControlFlowNode *ignore, ControlFlowNode *structure);
    void _ReconnectBreakNodeToSubsequentCode(ControlFlowNode *structure, ControlFlowNode *breakNode, code_pos subsequentcode);
    void _DoLoopTransforms();
    void _DoLoopTransform(ControlFlowNode *loop);
    void _FindCompoundConditions(ControlFlowNode *structure);
    void _FindAllIfStatements();
    void _FindIfStatements(DominatorMap &dominators, ControlFlowNode *structure);
    ControlFlowNode *_PartitionCode(code_pos start, code_pos end);
    ControlFlowNode *_FindFollowNodeForStructure(ControlFlowNode *structure);

    static ControlFlowNode *_ProcessNaturalLoop(ControlFlowGraph &loopDetection, ControlFlowNode *parent, const NodeBlock &backEdge);
    static ControlFlowNode *_ProcessSwitch(ControlFlowGraph &loopDetection, ControlFlowNode *parent, NodeBlock &switchBlock);
    static std::vector<NodeBlock> _FindSwitchBlocks(DominatorMap &dominators, DominatorMap &postDominators, ControlFlowNode *structure);

    template<typename FuncFindBlocks, typename PossiblyRearrangeAndProceed, typename ProcessBlock>
    void _FindAllStructuresOf(FuncFindBlocks findBlocks, PossiblyRearrangeAndProceed possiblyRearrangeAndProceed, ProcessBlock processBlock)
    {
        // Make a copy, since we'll be modifying it (adding to it)
        NodeSet controlStructuresCopy = discoveredControlStructures;
        // The first time we call _FindAllStructuresOf, we'll just have a single discovered structure(the main one)
        // After that we'll also have whatever structures were discovered in the previous call.
        // We discover structures from the most inner to the most outer (in this particular call of the function)
        for (ControlFlowNode *structure : controlStructuresCopy)
        {
            vector<NodeBlock> blocks;
            do
            {
                // 1) calculate dominators
                DominatorMap dominators = GenerateDominators(structure, (*structure)[SemId::Head]);
                assert(structure->MaybeGet(SemId::Tail));
                DominatorMap postDominators = GeneratePostDominators(structure, (*structure)[SemId::Tail]);

                // 2) Find the toss nodes, and follow them back to a dominator, bounding the switch statement.
                blocks = findBlocks(dominators, postDominators, structure);
                if (!blocks.empty())
                {
                    if (possiblyRearrangeAndProceed(*this, *structure, blocks))
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
                }
                // 4) Repeat until no new switches found
            } while (!blocks.empty() && !_decompilerResults.IsAborted());
        }
    }


    std::vector<std::unique_ptr<ControlFlowNode>> nodesOwner;

    NodeSet discoveredControlStructures;
    ControlFlowNode *mainStructure;

    std::string _contextName;
    IDecompilerResults &_decompilerResults;
    std::string _statusMessagePrefix;

    bool _debug;
    PCSTR _pszDebugFilter;
};
