#pragma once

struct ControlFlowNode;

DominatorMap GenerateDominators(ControlFlowNode *parent, ControlFlowNode *n0);
DominatorMap GeneratePostDominators(ControlFlowNode *parent, ControlFlowNode *n0);

bool IsReachable(ControlFlowNode *head, ControlFlowNode *tail);
NodeSet CollectNodesBetween(ControlFlowNode *head, ControlFlowNode *tail, NodeSet possible);
std::map<ControlFlowNode*, ControlFlowNode*> CalculateImmediateDominators(const DominatorMap &dominators);
std::map<ControlFlowNode*, ControlFlowNode*> CalculateImmediatePostDominators(const DominatorMap &postDominators);