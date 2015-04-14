#pragma once

struct CFGNode;

DominatorMap GenerateDominators(NodeSet N, CFGNode *n0);
DominatorMap GeneratePostDominators(NodeSet N, CFGNode *n0);

bool IsReachable(CFGNode *head, CFGNode *tail);
NodeSet CollectNodesBetween(CFGNode *head, CFGNode *tail, NodeSet possible);
std::map<CFGNode*, CFGNode*> CalculateImmediateDominators(const DominatorMap &dominators);
std::map<CFGNode*, CFGNode*> CalculateImmediatePostDominators(const DominatorMap &postDominators);