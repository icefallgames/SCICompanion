#include "stdafx.h"
#include "CFGNode.h"
#include "ControlFlowGraphViz.h"
#include "format.h"

using namespace std;

const char *colors[] = 
{
    "black",
    "red",
    "blue",
    "green",
    "purple",
    "orange",
};

class GraphVisualizer
{
    string _CreateGraphNodeId(const CFGNode *node, int index, const CFGNode *head = nullptr)
    {
        string name;
        switch (node->Type)
        {
            case CFGNodeType::RawCode:
            {
                const RawCodeNode *rawCodeNode = static_cast<const RawCodeNode *>(node);
                name = fmt::format("n_{:04x}_{}", (rawCodeNode->start->get_final_offset_dontcare()), index);
                break;
            }

            case CFGNodeType::Loop:
                name = fmt::format("loop_{}", node->ArbitraryDebugIndex);
                break;

            case CFGNodeType::Case:
                name = fmt::format("case_{}", node->ArbitraryDebugIndex);
                break;

            case CFGNodeType::Switch:
                name = fmt::format("switch_{}", node->ArbitraryDebugIndex);
                break;

            case CFGNodeType::CompoundCondition:
            {
                const CompoundConditionNode *ccNode = static_cast<const CompoundConditionNode *>(node);
                name = fmt::format((ccNode->condition == ConditionType::And) ? "and_{}" : "or_{}", node->ArbitraryDebugIndex);
                break;
            }
            case CFGNodeType::Exit:
                name = fmt::format("exit_{}", node->ArbitraryDebugIndex);
                break;
               
            case CFGNodeType::Invert:
                name = fmt::format("inv_{}", node->ArbitraryDebugIndex);
                break;

            case CFGNodeType::If:
                name = fmt::format("if_{}", node->ArbitraryDebugIndex);
                break;

            default:
                assert(false);
                break;
        }

        return name;
    }

    void _CreateHumanReadableLabel(stringstream &ss, const CFGNode *node)
    {
        ss << " [label =\"";

        switch (node->Type)
        {
            case CFGNodeType::RawCode:
            {
                const RawCodeNode *rawCodeNode = static_cast<const RawCodeNode *>(node);
                code_pos cur = rawCodeNode->start;
                ss << fmt::format("{:04x}:\\n", cur->get_final_offset_dontcare());
                while (cur != rawCodeNode->end)
                {
                    Opcode opcode = cur->get_opcode();
                    const char *pszOpcode = OpcodeNames[static_cast<BYTE>(opcode)];
                    ss << pszOpcode << "\\n";
                    ++cur;
                }
            }
            break;

            case CFGNodeType::Loop:
            {
                ss << "Loop " << node->ArbitraryDebugIndex;
            }
            break;

            case CFGNodeType::Case:
            {
                ss << "Case " << node->ArbitraryDebugIndex;
            }
            break;

            case CFGNodeType::Switch:
            {
                ss << "Switch " << node->ArbitraryDebugIndex;
            }
            break;

            case CFGNodeType::Invert:
            {
                ss << "Invert " << node->ArbitraryDebugIndex;
            }
            break;

            case CFGNodeType::Exit:
            {
                ss << "Exit:" << fmt::format("{:04x}", static_cast<const ExitNode*>(node)->startingAddressForExit);
            }
            break;

            case CFGNodeType::CompoundCondition:
            {
                const CompoundConditionNode *ccNode = static_cast<const CompoundConditionNode *>(node);
                ss << (ccNode->isFirstTermNegated ? "!" : "") << "X " << ((ccNode->condition == ConditionType::And) ? "and" : "or") << " Y " << node->ArbitraryDebugIndex;
            }
            break;

            case CFGNodeType::Main:
            {
                ss << "Main " << node->ArbitraryDebugIndex;
            }
            break;

            case CFGNodeType::If:
            {
                ss << "If " << node->ArbitraryDebugIndex;
            }
            break;


            default:
                assert(false);
                break;

        }

        ss << "\"]";
    }

    // For reading purposes
    void _CreateLabel(stringstream &ss, const CFGNode *node, int index, int colorIndex, const CFGNode *head = nullptr)
    {
        colorIndex %= ARRAYSIZE(colors);

        ss << "\t" << _CreateGraphNodeId(node, index, head);

        _CreateHumanReadableLabel(ss, node);

        ss << " [color=" << colors[colorIndex] << "];\n";
    }

public:
    void CFGVisualize(const std::string &name, NodeSet &discoveredControlStructures)
    {
        // Visualize it, until we know it's right
        std::stringstream ss;
        ss << "digraph code {\n";

        int graphIndex = 0;
        for (CFGNode *currentParent : discoveredControlStructures)
        {
            int colorIndex = currentParent->ArbitraryDebugIndex;
            for (auto node : currentParent->children)
            {
                if (discoveredControlStructures.find(node) != discoveredControlStructures.end())
                {
                    // This is a "parent", so color it like its children.
                    _CreateLabel(ss, node, graphIndex, node->ArbitraryDebugIndex);
                }
                else
                {
                    _CreateLabel(ss, node, graphIndex, colorIndex);
                }

                // Construct the graph from the predecessors
                for (auto &pred : node->Predecessors())
                {
                    ss << "\t" << _CreateGraphNodeId(pred, graphIndex) << " -> " << _CreateGraphNodeId(node, graphIndex);
                    ss << " [color=" << colors[colorIndex % ARRAYSIZE(colors)] << "];\n";
                }

                // If it's the head, but a fake entry point
                if (node == currentParent->MaybeGet(SemId::Head))
                {
                    // Special case, no predecessor. This is an entry point.
                    string nodeLabel = fmt::format("ENTRY_{}", graphIndex);
                    ss << "\t" << nodeLabel << " -> " << _CreateGraphNodeId(node, graphIndex);
                    ss << ";\n";

                    // Name it
                    ss << "\t" << nodeLabel;
                    _CreateHumanReadableLabel(ss, currentParent);
                    ss << ";\n";
                }
            }
            graphIndex++;
        }
        ss << "}\n";
        ShowTextFile(ss.str().c_str(), name + ".txt");
    }
};


void CFGVisualize(const std::string &name, NodeSet &discoveredControlStructures)
{
    GraphVisualizer viz;
    viz.CFGVisualize(name, discoveredControlStructures);
}