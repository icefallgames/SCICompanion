#include "stdafx.h"
#include "CFGNode.h"
#include "DecompilerCore.h"
#include "DecompilerNew.h"

using namespace std;
// Can we make a CodeNode that pretends like it's a tree?


std::string _indent2(int iIndent)
{
    std::string theFill;
    theFill.insert(theFill.begin(), iIndent, ' ');
    return theFill;
}


enum class ChunkType
{
    None,
    If,
    Then,
    Else,
    Condition,
    Do,
    While,
    LoopBody,
    And,
    Or,
    First,
    FirstNegated,
    Second,
    Invert,
    Switch,
    Case,
    DefaultCase,
    CaseCondition,
    CaseBody,
    SwitchValue,
};

const char *chunkTypeNames[] =
{
    "None",
    "If",
    "Then",
    "Else",
    "Condition",
    "Do",
    "While",
    "LoopBody",
    "And",
    "Or",
    "First",
    "First_N",
    "Second",
    "Invert",
    "Switch",
    "Case",
    "DefaultCase",
    "CaseCondition",
    "CaseBody",
    "SwitchValue",
};

struct CodeChunk
{
    CodeChunk() : _hasPos(false), _chunkType(ChunkType::None) {}

    bool _hasPos;
    code_pos pos;
    ChunkType _chunkType;

    void SetPos(code_pos pos) { this->pos = pos; _hasPos = true; }
    void SetType(ChunkType type) { this->_chunkType = type; _hasPos = false; }

    // Children might be stored backward for now, we'll see
    std::vector<std::unique_ptr<CodeChunk>> Children;


    void Print(std::ostream &os, int iIndent) const
    {
        os << _indent2(iIndent);
        if (_hasPos)
        {
            os << OpcodeNames[static_cast<BYTE>(pos->get_opcode())];
        }
        else
        {
            os << "[" << chunkTypeNames[(int)_chunkType] << "]";
        }
        os << "\n";

        for (auto &child : Children)
        {
            child->Print(os, iIndent + 2);
        }
    }
};

struct ContextFrame
{
    const CFGNode *node;
    CodeChunk *Parent;
    int cStackConsume;
    int cAccConsume;
};

class CodeChunkEnumContext
{
public:
    stack<ContextFrame> frames;
    CodeChunk *Current;

    ContextFrame &Frame() {
        return frames.top();
    }

    bool CanPopBeyond()
    {
        // Not sure how I'll handle this. It's possible each frame might
        // need a stack of CFGNode's, and we check if we're in it. Or rather, if we're in
        // it, then we can pop. If we're not, it's someone else's frame.

        // REVIEW: What we have now isn't enough. Consider the case where we had to
        // bounce back out of RawCodeNode in order to get more instructions, while deep inside.
        // and now we're in an if statement say... how will we ever bounce back up? I think instead
        // we have the notion of a parent CFGNode (in the visiting thing), and we shouldn't bounce beyond that or something?
        return
            (frames.size() > 1) 
            && (Frame().node == nullptr);
    }

    size_t GetLevelCount() { return frames.size(); }

    void PopFrame()
    {
        Current = frames.top().Parent;
        frames.pop();
    }

    void PushStructured(ChunkType type, const CFGNode *ownerNode)
    {
        Frame().Parent->Children.push_back(move(make_unique<CodeChunk>()));
        Current = Frame().Parent->Children.back().get();
        Current->SetType(type);
        assert(Frame().cStackConsume == 0);
        assert(Frame().cAccConsume == 0);
        // Generally structured nodes don't consume anything
        // (though we might change that to support re-using previous acc values?)
        PushFrame(0, 0, ownerNode);
    }

    void StartInstruction(code_pos code)
    {
        Frame().Parent->Children.push_back(move(make_unique<CodeChunk>()));
        Frame().Parent->Children.back()->SetPos(code);
        Current = Frame().Parent->Children.back().get();
        assert(Frame().cStackConsume == 0);
        assert(Frame().cAccConsume == 0);
        Consumption consumption = _GetInstructionConsumption(*code);
        if (consumption.cAccConsume || consumption.cStackConsume)
        {
            // This instruction needs stuff... recurse
            PushFrame(consumption.cAccConsume, consumption.cStackConsume, nullptr);
        }
    }

    void AddInstructionToCurrent(code_pos code)
    {
        Current->Children.push_back(move(make_unique<CodeChunk>()));
        Current->Children.back()->SetPos(code);

        Consumption consumption = _GetInstructionConsumption(*code);
        if (consumption.cAccConsume || consumption.cStackConsume)
        {
            // This instruction we just got consumes stuff. We need to recurse here and
            // have it eat up its stuff.
            Current = Current->Children.back().get();
            PushFrame(consumption.cAccConsume, consumption.cStackConsume, nullptr);
        }

    }

    void ConsumeAccForStructuredNode()
    {
        // A structured node like an if counts as generating one acc
        // REVIEW: Only if there is an else also... ?
        if (Frame().cAccConsume)
        {
            Frame().cAccConsume--;
        }
    }

    void PopBackUpMax()
    {
        // Pop all the nodes we can
        while ((Frame().cAccConsume == 0) && (Frame().cStackConsume == 0))
        {
            if (CanPopBeyond())
            {
                PopFrame();
            }
            else
            {
                break;
            }
        }
    }

    void PushMain(const MainNode *node)
    {
        PushFrame(0, 0, node);
    }

private:
    void PushFrame(int cAccConsume, int cStackConsume, const CFGNode *node = nullptr)
    {
        ContextFrame frame;
        frame.node = node;
        frame.cAccConsume = cAccConsume;
        frame.cStackConsume = cStackConsume;
        frame.Parent = Current;
        frames.push(frame);
    }
};

CFGNode *GetFirstPredecessorOrNull(CFGNode *node)
{
    assert(node->Predecessors().size() <= 1);
    return node->Predecessors().empty() ? nullptr : *node->Predecessors().begin();
}
CFGNode *GetFirstSuccOrNull(CFGNode *node)
{
    assert(node->Successors().size() <= 1);
    return node->Successors().empty() ? nullptr : *node->Successors().begin();
}

// This assembles our CFGNode tree into a tree of nodes that consume the accumulator and stack,
// with children that produce the acc/stack values that they consume.
// We can do this consume tree just inside the RawCodeNodes, because sometimes control
// structures take part in the tree. For instance, an if statement or switch statement
// can be embedded in and expression, since they "return" values in the accumulator.
class EnumerateCodeChunks : public ICFGNodeVisitor
{
public:
    EnumerateCodeChunks(CodeChunkEnumContext &context) : _context(context) {}

    void Visit(const RawCodeNode &rawCodeNode) override
    {
        code_pos cur = rawCodeNode.end;
        --cur;
        code_pos start = rawCodeNode.start;
        if (start->get_opcode() != Opcode::INDETERMINATE)
        {
            --start;
        }
        while (cur != start)
        {
            if ((_context.Frame().cAccConsume == 0) && (_context.Frame().cStackConsume == 0))
            {
                if (_context.CanPopBeyond())
                {
                    _context.PopFrame();
                }
                else
                {
                    // We can't pop up any more. Time for a new instruction in the current frame.
                    _context.StartInstruction(cur);
                    --cur;
                }
            }
            else
            {
                Consumption consTemp = _GetInstructionConsumption(*cur);
                if (consTemp.cAccGenerate)
                {
                    if (_context.Frame().cAccConsume)
                    {
                        _context.Frame().cAccConsume -= consTemp.cAccGenerate;
                    }
                }
                if (consTemp.cStackGenerate)
                {
                    if (_context.Frame().cStackConsume)
                    {
                        _context.Frame().cStackConsume -= consTemp.cStackGenerate;
                    }
                    else
                    {
                        // Something put something on the stack and we didn't need it.
                        // We can't do anything useful here.
                        assert(false); // INVESTIGATE
                    }
                }

                _context.AddInstructionToCurrent(cur);
                --cur;
            }
        }

        _context.PopBackUpMax();
    }

    void Visit(const LoopNode &loopNode) override
    {
        size_t levelCount = _context.GetLevelCount();
        _context.ConsumeAccForStructuredNode();

        // Let's detect the loop type
        CFGNode *latch = loopNode[SemId::Latch];
        CFGNode *head = loopNode[SemId::Head];
        if ((latch->Successors().size() == 2) && (head->Successors().size() == 1))
        {
            _context.PushStructured(ChunkType::Do, &loopNode);
            // It's a do loop. The condition is the latch.

            // Condition
            _context.PushStructured(ChunkType::Condition, &loopNode);
            latch->Accept(*this);
            _context.PopFrame();

            // then... this is a bit more complicated since we follow a chain
            _context.PushStructured(ChunkType::LoopBody, &loopNode);
            assert(latch->Predecessors().size() == 1);
            _FollowForwardChain(head, *latch->Predecessors().begin());
            _context.PopFrame();
        }
        else if ((latch->Successors().size() == 1) && (head->Successors().size() == 2))
        {
            _context.PushStructured(ChunkType::While, &loopNode);
            // It's a while. The condition is the head.

            // Condition
            _context.PushStructured(ChunkType::Condition, &loopNode);
            head->Accept(*this);
            _context.PopFrame();

            CFGNode *thenNode, *elseNode;
            GetThenAndElseBranches(head, &thenNode, &elseNode);
            _context.PushStructured(ChunkType::LoopBody, &loopNode);
            _FollowForwardChain(thenNode, latch);
            _context.PopFrame();
        }

        _context.PopFrame();
        assert(levelCount == _context.GetLevelCount());
        _context.PopBackUpMax();
    }

    void Visit(const SwitchNode &switchNode) override
    {
        // TODO: Need to sort out pre-amble and tail and such, because right now the switch is just one big blob.
        // This presents an issue with accumulator consumption.
        // Or perhaps we can just process the tail without entering a new context.
        CFGNode *switchTail = switchNode[SemId::Tail];
        switchTail->Accept(*this);
        // After processing the tail, we MUST have a toss that we just processed.
        assert(_context.Frame().cStackConsume == 1);
        if (_context.Frame().cStackConsume)
        {
            _context.Frame().cStackConsume--;
        }
        // Pop back up 
        _context.PopBackUpMax();

        // Now we can do the actual cases, and consume any necessary acc
        size_t levelCount = _context.GetLevelCount();
        _context.ConsumeAccForStructuredNode();

        _context.PushStructured(ChunkType::Switch, &switchNode);

        // We'll add the cases here. They are immediate successors of the head.
        vector<CFGNode*> sortedCases;
        CFGNode *switchHead = switchNode[SemId::Head];
        copy(switchHead->Successors().begin(), switchHead->Successors().end(), back_inserter(sortedCases));
        sort(sortedCases.begin(), sortedCases.end(), CompareCFGNodesByAddress());
        for (CFGNode *caseNode : sortedCases)
        {
            caseNode->Accept(*this);
        }

        // And now the switch head.
        // Now we need to process the switch value. But it's special.... because we may have a pre-amble.
        // We basically want to process just enough to get back to net zero. And then beyond that,
        // we should switch "out" of the switch node.
        // OR... we could just move out of the switch node right now:
        _context.PopFrame();
        assert(levelCount == _context.GetLevelCount());

        // And then process the switch value? Then during code gen, we'll know to look prior to the switch statement?
        switchHead->Accept(*this);

        _context.PopBackUpMax();
    }

    void Visit(const CaseNode &caseNode) override
    {
        size_t levelCount = _context.GetLevelCount();
        // Cases don't need to do acc consumption.

        _context.PushStructured(ChunkType::Case, &caseNode);

        // There could be a case condition.
        CFGNode *caseHead = caseNode[SemId::Head];
        CFGNode *caseBody = caseHead;
        if (caseHead->ContainsTag(SemanticTags::CaseCondition))
        {
            assert(!caseNode.ContainsTag(SemanticTags::DefaultCase));
            assert(caseHead->Type == CFGNodeType::RawCode);
            
            _context.PushStructured(ChunkType::CaseCondition, &caseNode);
            caseHead->Accept(*this);
            _context.PopFrame();
            caseBody = GetFirstSuccOrNull(caseHead);
        }

        _context.PushStructured(ChunkType::CaseBody, &caseNode);
        _FollowForwardChain(caseBody);
        _context.PopFrame();

        _context.PopFrame();
        assert(levelCount == _context.GetLevelCount());
        _context.PopBackUpMax();
    }

    void Visit(const CompoundConditionNode &conditionNode) override
    {
        size_t levelCount = _context.GetLevelCount();
        _context.ConsumeAccForStructuredNode();

        ConditionType type = conditionNode.condition;
        bool isFirstTermNegated = conditionNode.isFirstTermNegated;

        _context.PushStructured((type == ConditionType::And) ? ChunkType::And : ChunkType::Or, &conditionNode);

        CFGNode *first = conditionNode[SemId::First];
        // X
        _context.PushStructured(isFirstTermNegated ? ChunkType::FirstNegated : ChunkType::First, &conditionNode);
        first->Accept(*this);
        _context.PopFrame();

        CFGNode *second = conditionNode[SemId::Second];
        // Y
        _context.PushStructured(ChunkType::Second, &conditionNode);
        second->Accept(*this);
        _context.PopFrame();

        _context.PopFrame();
        assert(levelCount == _context.GetLevelCount());
        _context.PopBackUpMax();
    }

    void Visit(const InvertNode &invert) override
    {
        size_t levelCount = _context.GetLevelCount();
        _context.ConsumeAccForStructuredNode();

        _context.PushStructured(ChunkType::Invert, &invert);
        invert[SemId::Head]->Accept(*this);
        _context.PopFrame();
        assert(levelCount == _context.GetLevelCount());
        _context.PopBackUpMax();
    }

    void _FollowForwardChain(CFGNode *node, CFGNode *inclusiveEnd = nullptr)
    {
        // We want to go backwards
        vector<CFGNode*> nodes;
        while (node != nullptr)
        {
            nodes.push_back(node);
            if (inclusiveEnd == node)
            {
                break;
            }
            node = GetFirstSuccOrNull(node);
        }

        std::reverse(nodes.begin(), nodes.end());
        for (CFGNode *current : nodes)
        {
            current->Accept(*this);
        }
    }

    void Visit(const IfNode &ifNode) override
    {
        size_t levelCount = _context.GetLevelCount();
        _context.ConsumeAccForStructuredNode();

        // Adds a child with an if
        _context.PushStructured(ChunkType::If, &ifNode);

        // Now we need to add three children:
        //  - condition
        //  - then
        //  - else
        CFGNode *ifCondition = ifNode[SemId::Head];
        CFGNode *thenNode, *elseNode;
        GetThenAndElseBranches(ifCondition, &thenNode, &elseNode);

        // Condition
        _context.PushStructured(ChunkType::Condition, &ifNode);
        ifCondition->Accept(*this);
        _context.PopFrame();

        // then... this is a bit more complicated since we follow a chain
        _context.PushStructured(ChunkType::Then, &ifNode);
        _FollowForwardChain(thenNode);
        _context.PopFrame();

        if (elseNode && (elseNode->Type != CFGNodeType::Exit))
        {
            _context.PushStructured(ChunkType::Else, &ifNode);
            _FollowForwardChain(elseNode);
            _context.PopFrame();
        }

        _context.PopFrame();
        assert(levelCount == _context.GetLevelCount());
        _context.PopBackUpMax();
    }

    void Visit(const MainNode &main) override
    {
        _context.PushMain(&main);
        CFGNode *node = main[SemId::Tail];
        while (node)
        {
            node->Accept(*this);
            node = GetFirstPredecessorOrNull(node);
        }
        _context.PopFrame();

        // For now, print out mainChunk
    }
    void Visit(const CFGNode &node)override {}

private:
    CodeChunkEnumContext &_context;
};

void OutputNewStructure(const std::string &name, MainNode &main)
{
    CodeChunkEnumContext context;
    unique_ptr<CodeChunk> mainChunk = make_unique<CodeChunk>();
    context.Current = mainChunk.get();
    EnumerateCodeChunks enumCodeChunks(context);
    enumCodeChunks.Visit(main);

    std::stringstream ss;
    mainChunk->Print(ss, 0);
    ShowTextFile(ss.str().c_str(), name + "_chunks.txt");
}