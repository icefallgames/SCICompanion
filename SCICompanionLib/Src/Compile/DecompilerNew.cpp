#include "stdafx.h"
#include "ControlFlowNode.h"
#include "DecompilerCore.h"
#include "DecompilerNew.h"
#include "ScriptOMAll.h"
#include "GameFolderHelper.h"
#include "format.h"
#include "DecompilerResults.h"

using namespace std;
using namespace sci;
// Can we make a ConsumptionNode that pretends like it's a tree?


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
    Break,
    NeedsAccumulator,
    FailedToGetAccumulator,
    FailedToGetStack,
    NeedsStack,
    ZeroNode,
    TrueNode,
    ShortCircuitInstruction,
    FunctionBody,
};

// For debugging purposes
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
    "Break",
    "NeedsAccumulator",
    "FailedToGetAccumulator",
    "FailedToGetStack",
    "NeedsStack",
    "ZeroNode",
    "TrueNode",
    "ShortCircuitInstruction",
    "FunctionBody",
};

struct ConsumptionNode;
Consumption _GetInstructionConsumption(ConsumptionNode &node, DecompileLookups &lookups);

struct ConsumptionNode
{
    ConsumptionNode() : _hasPos(false), _chunkType(ChunkType::None), _parentWeak(nullptr) {}

    bool _hasPos;
    code_pos pos;
    ChunkType _chunkType;
    ConsumptionNode *_parentWeak;

    unique_ptr<ConsumptionNode> Clone()
    {
        unique_ptr<ConsumptionNode> clone = make_unique<ConsumptionNode>();
        clone->_hasPos = _hasPos;
        clone->pos = pos;
        clone->_chunkType = _chunkType;
        for (auto &child : children)
        {
            clone->children.push_back(move(child->Clone()));
            assert(clone->children.back()->_parentWeak == nullptr);
            clone->children.back()->_parentWeak = clone.get();
        }
        assert(_parentWeak); // Don't want to be cloning top-levl eguy
        return clone;
    }

    code_pos GetCode() const
    {
        assert(_hasPos);
        return pos;
    }
    ChunkType GetType() const
    {
        return _chunkType;
    }

    int GetMyIndex() const
    {
        return _parentWeak->GetIndexOf(this);
    }

    int GetIndexOf(const ConsumptionNode *child) const
    {
        for (size_t i = 0; i < children.size(); i++)
        {
            if (children[i].get() == child)
            {
                return (int)i;
            }
        }
        assert(false && "Corrupt hierarchy");
        return 0;
    }

    ConsumptionNode *GetChild(ChunkType type) const
    {
        for (auto &child : children)
        {
            if (child->GetType() == type)
            {
                return child.get();
            }
        }
        return nullptr;
    }

    void SetPos(code_pos pos) { this->pos = pos; _hasPos = true; }
    void SetType(ChunkType type) { this->_chunkType = type; _hasPos = false; }

    ConsumptionNode *PrependChild()
    {
        std::unique_ptr<ConsumptionNode> newNode = make_unique<ConsumptionNode>();
        ConsumptionNode *returnValue = newNode.get();
        PrependChild(move(newNode));
        return returnValue;
    }
    void PrependChild(std::unique_ptr<ConsumptionNode> chunk)
    {
        chunk->_parentWeak = this;
        children.insert(children.begin(), move(chunk));
    }

    void Print(std::ostream &os, int iIndent) const
    {
        os << hex;
        os << _indent2(iIndent);
        if (_hasPos)
        {
            os << OpcodeNames[static_cast<BYTE>(pos->get_opcode())] << "  [" << setw(4) << setfill('0') << pos->get_final_offset_dontcare() << "]";
        }
        else
        {
            os << "[" << chunkTypeNames[(int)_chunkType] << "]";
        }
        os << "\n";

        for (auto &child : children)
        {
            child->Print(os, iIndent + 2);
        }
    }

    const std::vector<std::unique_ptr<ConsumptionNode>> &Children() { return children; }

    std::unique_ptr<ConsumptionNode> ReplaceChild(size_t index, unique_ptr<ConsumptionNode> replacement)
    {
        std::unique_ptr<ConsumptionNode> returnValue = move(children[index]);
        children[index] = move(replacement);
        children[index]->_parentWeak = this;
        returnValue->_parentWeak = nullptr;
        return returnValue;
    }

    void InsertChild(size_t index, unique_ptr<ConsumptionNode> replacement)
    {
        children.insert(children.begin() + index, move(replacement));
        children[index]->_parentWeak = this;
    }

    // If lookups is provided, then we will replace stolen nodes with NeedsStack, if necessary.
    // This is a recent change, so I've scoped it only to where I encountered this bug (switch statements, SQ4, script 376)
    std::unique_ptr<ConsumptionNode> StealChild(size_t index, DecompileLookups *lookups = nullptr)
    {
        std::unique_ptr<ConsumptionNode> stolen = move(children[index]);
        children.erase(children.begin() + index);
        stolen->_parentWeak = nullptr;

        // What if we needed this?
        bool replace = false;
        if (lookups)
        {
            if (_GetInstructionConsumption(*stolen, *lookups).cStackGenerate)
            {
                if (_GetInstructionConsumption(*this, *lookups).cStackConsume)
                {
                    // We just lost a stack that we were relying on, so we'll need a replacement.
                    // We'll insert it at the beginning. So like, stuff "shifts down".
                    ConsumptionNode *replacementChild = PrependChild();
                    replacementChild->SetType(ChunkType::NeedsStack);
                }
            }
        }

        return stolen;
    }
    size_t GetChildCount() { return children.size(); }
    ConsumptionNode *Child(int i) { return children[i].get(); }

private:
    // Children might be stored backward for now, we'll see
    std::vector<std::unique_ptr<ConsumptionNode>> children;
};

class ConsumptionNodeException : public std::exception
{
public:
    ConsumptionNodeException(const ConsumptionNode *node, const std::string &message) : message(message), node(node) {}

    const ConsumptionNode *node;
    std::string message;
};

struct ContextFrame
{
    const ControlFlowNode *node;
    ConsumptionNode *Parent;
    int cStackConsume;
    int cAccConsume;
};

class CodeChunkEnumContext
{
public:
    CodeChunkEnumContext(DecompileLookups &lookups) : _lookups(lookups) {}
    DecompileLookups &_lookups;

    stack<ContextFrame> frames;
    ConsumptionNode *Current;

    ContextFrame &Frame() {
        return frames.top();
    }

    bool CanPopBeyond()
    {
        // Not sure how I'll handle this. It's possible each frame might
        // need a stack of ControlFlowNode's, and we check if we're in it. Or rather, if we're in
        // it, then we can pop. If we're not, it's someone else's frame.

        // REVIEW: What we have now isn't enough. Consider the case where we had to
        // bounce back out of RawCodeNode in order to get more instructions, while deep inside.
        // and now we're in an if statement say... how will we ever bounce back up? I think instead
        // we have the notion of a parent ControlFlowNode (in the visiting thing), and we shouldn't bounce beyond that or something?
        return
            (frames.size() > 1) 
            && (Frame().node == nullptr);
    }

    size_t GetLevelCount() { return frames.size(); }

    void PopFrame()
    {
        frames.pop();
        Current = (frames.size() > 1) ? frames.top().Parent : nullptr;
    }

    void AddStructured(ChunkType type)
    {
        Current = Frame().Parent->PrependChild();
        Current->SetType(type);
    }

    void MaybeAddNeedsAcc()
    {
        if (Frame().cAccConsume > 0)
        {
            AddStructured(ChunkType::NeedsAccumulator);
        }
        if (Frame().cStackConsume > 0)
        {
            // SQ5, script 999, proc 999_4. SQ5, script 995, InvI::showSelf
            // Let's just see what happens if we put multiple "NeedsStack" here
            while (Frame().cStackConsume > 0)
            {
                AddStructured(ChunkType::NeedsStack);
                Frame().cStackConsume--;
            }
            //if (Frame().cStackConsume > 1)
            //{
                //throw ConsumptionNodeException(Frame().Parent, "Needs more than 1 stack instruction");
            //}
            //assert(Frame().cStackConsume == 1); // Otherwise we'll need to take note. I suppose this could happen in a send.
        }
    }

    void PushStructured(ChunkType type, const ControlFlowNode *ownerNode)
    {
        Current = Frame().Parent->PrependChild();
        Current->SetType(type);
        assert(Frame().cAccConsume == 0);
        // Generally structured nodes don't consume anything
        // (though we might change that to support re-using previous acc values?)
        PushFrame(0, 0, ownerNode);
    }

    void StartInstruction(code_pos code)
    {
        Current = Frame().Parent->PrependChild();
        Current->SetPos(code);
        assert(Frame().cStackConsume == 0);
        assert(Frame().cAccConsume == 0);
        Consumption consumption = _GetInstructionConsumption(*code, &_lookups);
        if (consumption.cAccConsume || consumption.cStackConsume)
        {
            // This instruction needs stuff... recurse
            PushFrame(consumption.cAccConsume, consumption.cStackConsume, nullptr);
        }
    }

    void AddInstructionToCurrent(code_pos code)
    {
        ConsumptionNode *newChild = Current->PrependChild();
        newChild->SetPos(code);

        Consumption consumption = _GetInstructionConsumption(*code, &_lookups);
        if (consumption.cAccConsume || consumption.cStackConsume)
        {
            // This instruction we just got consumes stuff. We need to recurse here and
            // have it eat up its stuff.
            Current = newChild;
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
    void PushFrame(int cAccConsume, int cStackConsume, const ControlFlowNode *node = nullptr)
    {
        ContextFrame frame;
        frame.node = node;
        frame.cAccConsume = cAccConsume;
        assert(frame.cAccConsume <= 1);
        frame.cStackConsume = cStackConsume;
        assert(frame.cStackConsume <= 255);  // Any higher probably indicates a corrupt script.
        frame.Parent = Current;
        frames.push(frame);
    }
};

ControlFlowNode *GetFirstPredecessorOrNull(ControlFlowNode *node)
{
    assert(node->Predecessors().size() <= 1);
    return node->Predecessors().empty() ? nullptr : *node->Predecessors().begin();
}
ControlFlowNode *GetFirstSuccOrNull(ControlFlowNode *node)
{
    assert(node->Successors().size() <= 1);
    return node->Successors().empty() ? nullptr : *node->Successors().begin();
}

// This assembles our ControlFlowNode tree into a tree of nodes that consume the accumulator and stack,
// with children that produce the acc/stack values that they consume.
// We can't do this consume tree just inside the RawCodeNodes, because sometimes control
// structures take part in the tree. For instance, an if statement or switch statement
// can be embedded in and expression, since they "return" values in the accumulator.
class EnumerateCodeChunks : public ICFGNodeVisitor
{
private:
    // Scope guard for pushing/popping frames for structured nodes.
    class StructuredFrame
    {
    public:
        StructuredFrame(CodeChunkEnumContext &context, ChunkType type, const ControlFlowNode &node) : _context(context)
        {
            _context.PushStructured(type, &node);
            _level = _context.GetLevelCount();
        }
        ~StructuredFrame()
        {
            if (_context.GetLevelCount() > _level)
            {
                // We need to force pop some things
                while (_context.GetLevelCount() > _level)
                {
                    _context.MaybeAddNeedsAcc();
                    _context.PopFrame();
                }
            }
            else 
            {
                assert(_context.GetLevelCount() == _level);
            }
            _context.PopFrame();
        }
    private:
        CodeChunkEnumContext &_context;
        size_t _level;
    };

    DecompileLookups &_lookups;

public:
    EnumerateCodeChunks(CodeChunkEnumContext &context, DecompileLookups &lookups) : _context(context), _lookups(lookups) {}

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
                Consumption consTemp = _GetInstructionConsumption(*cur, &_lookups);
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
                        // So don't process this instruction, and back up a frame. If we are in need
                        // of an accumulator value, take note of that here.
                        assert(_context.Frame().cAccConsume > 0);
                        _context.AddStructured(ChunkType::NeedsAccumulator);
                        assert(_context.CanPopBeyond());
                        _context.PopFrame();
                        continue;
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
        ControlFlowNode *latch = loopNode[SemId::Latch];
        ControlFlowNode *head = loopNode[SemId::Head];
        if ((latch->Successors().size() == 2) && (head->Successors().size() == 1))
        {
            StructuredFrame structuredFrame(_context, ChunkType::Do, loopNode);
            // It's a do loop. The condition is the latch.
            // We use do-whiles. So if the condition is true, then we should go back
            // to the header. If the "then" branch of the latch isn't the header,
            // then we should invert the condition.
            if (IsThenBranch(latch, head))
            {
                StructuredFrame structuredFrame(_context, ChunkType::Condition, loopNode);
                latch->Accept(*this);
            }
            else
            {
                StructuredFrame structuredFrame(_context, ChunkType::Condition, loopNode);
                {
                    StructuredFrame structuredFrame(_context, ChunkType::Invert, loopNode);
                    latch->Accept(*this);
                }
            }

            // then... this is a bit more complicated since we follow a chain
            {
                StructuredFrame structuredFrame(_context, ChunkType::LoopBody, loopNode);
                assert(latch->Predecessors().size() == 1);
                _FollowForwardChain(head, *latch->Predecessors().begin());
            }
        }
        else if ((latch->Successors().size() == 1) && (head->Successors().size() == 2))
        {
            StructuredFrame structuredFrame(_context, ChunkType::While, loopNode);
            // It's a while. The condition is the head.
            // REVIEW: Do we need to do the same kind of thing we do in the do loop above, wrt inverting?

            // Condition
            {
                StructuredFrame structuredFrame(_context, ChunkType::Condition, loopNode);
                head->Accept(*this);
            }

            ControlFlowNode *thenNode, *elseNode;
            GetThenAndElseBranches(head, &thenNode, &elseNode);
            {
                StructuredFrame structuredFrame(_context, ChunkType::LoopBody, loopNode);
                _FollowForwardChain(thenNode, latch);
            }
        }
        else
        {
            // It must be an infinite loop. We'll use a while(TRUE) for this.
            StructuredFrame structuredFrame(_context, ChunkType::While, loopNode);

            // Condition
            {
                StructuredFrame structuredFrame(_context, ChunkType::Condition, loopNode);
                _context.AddStructured(ChunkType::TrueNode);
            }

            {
                StructuredFrame structuredFrame(_context, ChunkType::LoopBody, loopNode);
                _FollowForwardChain(head, latch);
            }
        }

        assert(levelCount == _context.GetLevelCount());
        _context.PopBackUpMax();
    }

    void Visit(const SwitchNode &switchNode) override
    {
        ControlFlowNode *switchTail = switchNode[SemId::Tail];
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
        vector<ControlFlowNode*> sortedCases;
        ControlFlowNode *switchHead = switchNode[SemId::Head];
        copy(switchHead->Successors().begin(), switchHead->Successors().end(), back_inserter(sortedCases));
        sort(sortedCases.rbegin(), sortedCases.rend(), CompareCFGNodesByAddress()); // Sort in reverse
        for (ControlFlowNode *caseNode : sortedCases)
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

        // And then process the switch value? Then during code gen, we'll know to look prior to the switch statement for the value?
        switchHead->Accept(*this);

        _context.PopBackUpMax();
    }

    void Visit(const CaseNode &caseNode) override
    {
        size_t levelCount = _context.GetLevelCount();
        // Cases don't need to do acc consumption.

        _context.PushStructured(ChunkType::Case, &caseNode);

        // There could be a case condition.
        ControlFlowNode *caseHead = caseNode[SemId::Head];
        ControlFlowNode *caseBody = caseHead;
        if (caseHead->ContainsTag(SemanticTags::CaseCondition))
        {
            assert(!caseNode.ContainsTag(SemanticTags::DefaultCase));
            assert(caseHead->Type == CFGNodeType::RawCode);

            {
                StructuredFrame structuredFrame(_context, ChunkType::CaseCondition, caseNode);
                caseHead->Accept(*this);
            }
            caseBody = GetFirstSuccOrNull(caseHead);
        }

        {
            StructuredFrame structuredFrame(_context, ChunkType::CaseBody, caseNode);
            _FollowForwardChain(caseBody);
        }

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

        ControlFlowNode *first = conditionNode[SemId::First];
        // X
        {
            StructuredFrame structuredFrame(_context, isFirstTermNegated ? ChunkType::FirstNegated : ChunkType::First, conditionNode);
            first->Accept(*this);
        }

        ControlFlowNode *second = conditionNode[SemId::Second];
        // Y
        {
            StructuredFrame structuredFrame(_context, ChunkType::Second, conditionNode);
            second->Accept(*this);
        }

        _context.PopFrame();
        assert(levelCount == _context.GetLevelCount());
        _context.PopBackUpMax();
    }

    void Visit(const InvertNode &invert) override
    {
        size_t levelCount = _context.GetLevelCount();
        _context.ConsumeAccForStructuredNode();

        {
            StructuredFrame structuredFrame(_context, ChunkType::Invert, invert);
            invert[SemId::Head]->Accept(*this);
        }

        assert(levelCount == _context.GetLevelCount());
        _context.PopBackUpMax();
    }

    void _FollowForwardChain(ControlFlowNode *node, ControlFlowNode *inclusiveEnd = nullptr)
    {
        // We want to go backwards
        vector<ControlFlowNode*> nodes;
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
        for (ControlFlowNode *current : nodes)
        {
            if (current->ContainsTag(SemanticTags::LoopBreak))
            {
                // We've already identified break statements during control flow, so we can
                // be confident this is one without looking at its destination.
                _context.AddStructured(ChunkType::Break);
            }
            else
            {
                current->Accept(*this);
            }
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
        ControlFlowNode *ifCondition = ifNode[SemId::Head];
        ControlFlowNode *thenNode, *elseNode;
        GetThenAndElseBranches(ifCondition, &thenNode, &elseNode);

        // then... this is a bit more complicated since we follow a chain
        {
            StructuredFrame structuredFrame(_context, ChunkType::Then, ifNode);
            _FollowForwardChain(thenNode);
        }

        if (elseNode && (elseNode->Type != CFGNodeType::Exit))
        {
            StructuredFrame structuredFrame(_context, ChunkType::Else, ifNode);
            _FollowForwardChain(elseNode);
        }

        // Condition
        {
            StructuredFrame structuredFrame(_context, ChunkType::Condition, ifNode);
            ifCondition->Accept(*this);
        }

        _context.PopFrame();
        assert(levelCount == _context.GetLevelCount());

        _context.PopBackUpMax();
    }

    void Visit(const MainNode &main) override
    {
        _context.PushMain(&main);
        ControlFlowNode *node = main[SemId::Tail];
        while (node)
        {
            node->Accept(*this);
            node = GetFirstPredecessorOrNull(node);
        }
        _context.PopFrame();

        // For now, print out mainChunk
    }
    void Visit(const ControlFlowNode &node)override {}

private:
    CodeChunkEnumContext &_context;
};

// fwd decl
std::unique_ptr<SyntaxNode> _CodeNodeToSyntaxNode(ConsumptionNode &node, DecompileLookups &lookups);

void _ApplySyntaxNodeToCodeNode(ConsumptionNode &node, StatementsNode &statementsNode, DecompileLookups &lookups)
{
    unique_ptr<SingleStatement> pStatement = std::make_unique<SingleStatement>();
    pStatement->SetSyntaxNode(std::move(_CodeNodeToSyntaxNode(node, lookups)));
    statementsNode.AddStatement(std::move(pStatement));
}

void _ApplySyntaxNodeToCodeNode1(ConsumptionNode &node, OneStatementNode &statementsNode, DecompileLookups &lookups)
{
    unique_ptr<SingleStatement> pStatement = std::make_unique<SingleStatement>();
    pStatement->SetSyntaxNode(std::move(_CodeNodeToSyntaxNode(node, lookups)));
    statementsNode.SetStatement1(move(pStatement));
}

void _ApplySyntaxNodeToCodeNode2(ConsumptionNode &node, TwoStatementNode &statementsNode, DecompileLookups &lookups)
{
    unique_ptr<SingleStatement> pStatement = std::make_unique<SingleStatement>();
    pStatement->SetSyntaxNode(std::move(_CodeNodeToSyntaxNode(node, lookups)));
    statementsNode.SetStatement2(move(pStatement));
}

void _ApplySyntaxNodeToCodeNodeConditionNode(ConsumptionNode &node, ConditionNode &conditionNode, DecompileLookups &lookups)
{
    unique_ptr<ConditionalExpression> condExp = make_unique<ConditionalExpression>();
    _ApplySyntaxNodeToCodeNode(node, *condExp, lookups);
    conditionNode.SetCondition(move(condExp));
}

void _ApplyChildren(ConsumptionNode &node, StatementsNode &statementsNode, DecompileLookups &lookups)
{
    for (auto &child : node.Children())
    {
        _ApplySyntaxNodeToCodeNode(*child, statementsNode, lookups);
    }
}


std::unique_ptr<SyntaxNode> _CodeNodeToSyntaxNode2(ConsumptionNode &node, DecompileLookups &lookups)
{
    switch (node._chunkType)
    {
        case ChunkType::Break:
        {
            return unique_ptr<SyntaxNode>(new BreakStatement());
        }

        case ChunkType::If:
        {
            unique_ptr<CppIfStatement> ifStatement = make_unique<CppIfStatement>();
            _ApplySyntaxNodeToCodeNodeConditionNode(*node.GetChild(ChunkType::Condition), *ifStatement, lookups);
            _ApplySyntaxNodeToCodeNode1(*node.GetChild(ChunkType::Then), *ifStatement, lookups);
            if (node.GetChild(ChunkType::Else))
            {
                _ApplySyntaxNodeToCodeNode2(*node.GetChild(ChunkType::Else), *ifStatement, lookups);
            }
            return unique_ptr<SyntaxNode>(move(ifStatement));
        }

        case ChunkType::Condition:
            if (node.GetChildCount() != 1)
            {
                throw ConsumptionNodeException(&node, "Condition has too many children. Could be due to bad partition before if.");
            }
            return _CodeNodeToSyntaxNode(*node.Child(0), lookups);

        case ChunkType::And:
        case ChunkType::Or:
        {
            unique_ptr<BinaryOp> binaryOp = make_unique<BinaryOp>();
            if (node.GetChild(ChunkType::FirstNegated))
            {
                assert(node.GetChild(ChunkType::FirstNegated)->GetChildCount() == 1);
                unique_ptr<SingleStatement> negated = make_unique<SingleStatement>();
                unique_ptr<UnaryOp> unaryOp = make_unique<UnaryOp>();
                unaryOp->SetName("not");
                _ApplySyntaxNodeToCodeNode1(*node.GetChild(ChunkType::FirstNegated)->Child(0), *unaryOp, lookups);
                negated->SetSyntaxNode(move(unaryOp));
                binaryOp->SetStatement1(move(negated));
            }
            else
            {
                if (node.GetChild(ChunkType::First)->GetChildCount() != 1)
                {
                    throw ConsumptionNodeException(node.GetChild(ChunkType::First), "Too many children for condition node.");
                }
                _ApplySyntaxNodeToCodeNode1(*node.GetChild(ChunkType::First)->Child(0), *binaryOp, lookups);
            }

            if (node.GetChild(ChunkType::Second)->GetChildCount() != 1)
            {
                throw ConsumptionNodeException(node.GetChild(ChunkType::Second), "Too many children for condition node.");
            }
            _ApplySyntaxNodeToCodeNode2(*node.GetChild(ChunkType::Second)->Child(0), *binaryOp, lookups);
            binaryOp->SetName((node._chunkType == ChunkType::And) ? "&&" : "||");
            return unique_ptr<SyntaxNode>(move(binaryOp));
        }
        
        case ChunkType::Then:
        case ChunkType::Else:
        case ChunkType::CaseBody:
        {
            unique_ptr<CodeBlock> codeBlock = std::make_unique<CodeBlock>();
            _ApplyChildren(node, *codeBlock, lookups);
            return unique_ptr<SyntaxNode>(move(codeBlock));
        }
            
        case ChunkType::While:
        {
            unique_ptr<WhileLoop> whileLoop = make_unique<WhileLoop>();
            _ApplySyntaxNodeToCodeNodeConditionNode(*node.GetChild(ChunkType::Condition), *whileLoop, lookups);
            _ApplyChildren(*node.GetChild(ChunkType::LoopBody), *whileLoop, lookups);
            return unique_ptr<SyntaxNode>(move(whileLoop));
        }

        case ChunkType::Do:
        {
            unique_ptr<DoLoop> doLoop = make_unique<DoLoop>();
            _ApplySyntaxNodeToCodeNodeConditionNode(*node.GetChild(ChunkType::Condition), *doLoop, lookups);
            _ApplyChildren(*node.GetChild(ChunkType::LoopBody), *doLoop, lookups);
            return unique_ptr<SyntaxNode>(move(doLoop));
        }

        case ChunkType::Invert:
        {
            unique_ptr<UnaryOp> unaryOp = make_unique<UnaryOp>();
            unaryOp->SetName("not");
            if (node.GetChildCount() != 1)
            {
                throw ConsumptionNodeException(&node, "Too many children for Invert node.");
            }
            _ApplySyntaxNodeToCodeNode1(*node.Child(0), *unaryOp, lookups);
            return unique_ptr<SyntaxNode>(move(unaryOp));
        }

        case ChunkType::Switch:
        {
            unique_ptr<SwitchStatement> switchStatement = make_unique<SwitchStatement>();
            
            for (auto &child : node.Children())
            {
                if (child->GetType() == ChunkType::Case)
                {
                    // A case should have two children. The case body and (optionally) a condition
                    unique_ptr<CaseStatement> caseStatement = make_unique<CaseStatement>();
                    ConsumptionNode *cond = child->GetChild(ChunkType::CaseCondition);
                    if (cond)
                    {
                        assert(cond->GetChildCount() == 1);
                        _ApplySyntaxNodeToCodeNode1(*cond->Child(0), *caseStatement, lookups);
                    }
                    else
                    {
                        caseStatement->SetDefault(true);
                    }
                    ConsumptionNode *body = child->GetChild(ChunkType::CaseBody);
                    _ApplySyntaxNodeToCodeNode(*body, *caseStatement, lookups);
                    switchStatement->AddCase(move(caseStatement));
                }
                else if (child->GetType() == ChunkType::SwitchValue)
                {
                    assert(child->GetChildCount() == 1);
                    _ApplySyntaxNodeToCodeNode1(*child->Child(0), *switchStatement, lookups);
                }
            }

            return unique_ptr<SyntaxNode>(move(switchStatement));
        }

        case ChunkType::ZeroNode:
        {
            // This could mean FALSE, or NULL, we don't know which. So just use 0.
            unique_ptr<PropertyValue> valueTemp = make_unique<PropertyValue>();
            valueTemp->SetValue(0, IntegerFlags::IFNone);
            return unique_ptr<SyntaxNode>(move(valueTemp));
        }

        case ChunkType::TrueNode:
        {
            // This could mean FALSE, or NULL, we don't know which. So just use 0.
            unique_ptr<PropertyValue> valueTemp = make_unique<PropertyValue>();
            valueTemp->SetValue("TRUE", ValueType::Token);
            return unique_ptr<SyntaxNode>(move(valueTemp));
        }

        case ChunkType::NeedsAccumulator:
        {
            unique_ptr<PropertyValue> valueTemp = make_unique<PropertyValue>();
            valueTemp->SetValue("ERROR_NEED_ACC", ValueType::Token);
            return unique_ptr<SyntaxNode>(move(valueTemp));
        }
    }

    assert(false);
    std::unique_ptr<Comment> comment = std::make_unique<Comment>();
    comment->SetName("UNIMPLEMENTED CONTROL STRUCTURE");
    return unique_ptr<SyntaxNode>(comment.release());
}

WORD _GetImmediateFromCodeNode(ConsumptionNode &node, ConsumptionNode *pNodePrevious = nullptr, bool assertIfNone = false, bool *foundOut = nullptr)
{
    bool found = true;
    WORD w = 0;
    if (node._hasPos)
    {
        code_pos pos = node.pos;
        switch (pos->get_opcode())
        {
            case Opcode::LDI:
                w = pos->get_first_operand();
                break;

            case Opcode::PUSHI:
                w = pos->get_first_operand();
                break;

            case Opcode::PUSH0:
                w = 0;
                break;

            case Opcode::PUSH1:
                w = 1;
                break;

            case Opcode::PUSH2:
                w = 2;
                break;

            case Opcode::PUSH:
                // REVIEW, hits too often..... ldi jmp ldi push
                //ASSERT(node.GetChildCount());
                break;

            case Opcode::DUP:
                if (pNodePrevious)
                {
                    w = _GetImmediateFromCodeNode(*pNodePrevious);
                }
                else
                {
                    code_pos posFlatPrevious = pos;
                    --posFlatPrevious;
                    // REVIEW: could crash...
                    ConsumptionNode nodeTemp;
                    nodeTemp.SetPos(posFlatPrevious);
                    w = _GetImmediateFromCodeNode(nodeTemp);
                }
                break;

            default:
                found = false;
                assert(!assertIfNone);
                break;
        }
    }
    if (foundOut)
    {
        *foundOut = found;
    }
    return w;
}

const char SelfToken[] = "self";
const char InvalidLookupError[] = "LOOKUP_ERROR";
const char RestParamName[] = "params";


// Looks for a rest instruction at index (and tracks it and adds to the SendParam). Returns true if it found one.
bool _MaybeConsumeRestInstruction(SendParam *pSendParam, int index, ConsumptionNode &node, DecompileLookups &lookups)
{
    if (index >= (int)node.GetChildCount())
    {
        return false;
    }
    if (!node.Child(index)->_hasPos)
    {
        return false;
    }

    code_pos restInstruction = node.Child(index)->GetCode();
    bool foundRest = (restInstruction->get_opcode() == Opcode::REST);
    if (foundRest)
    {
        std::unique_ptr<RestStatement> rest = std::make_unique<RestStatement>();
        // rest->SetName(RestParamName);
        // We'll set the name at a later time, because it might already have a name.

        // We need to tell the function that
        // someone used a rest instruction with this index:
        uint16_t parameterIndex = restInstruction->get_first_operand();
        assert(parameterIndex > 0); // otherwise, I don't know what I'm doing.
        // The rest statement will take on its name if so. Otherwise we'll use "params"
        lookups.TrackRestStatement(rest.get(), parameterIndex);

        unique_ptr<SingleStatement> pStatement = std::make_unique<SingleStatement>();
        pStatement->SetSyntaxNode(std::move(rest));
        pSendParam->AddStatement(std::move(pStatement));
    }
    return foundRest;
}

void _ProcessLEA(PropertyValueBase &value, const scii &inst, DecompileLookups &lookups)
{
    VarScope varScope;
    WORD wVarIndex;
    value.SetValue(_GetVariableNameFromCodePos(inst, lookups, &varScope, &wVarIndex), ValueType::Pointer);
    lookups.TrackVariableUsage(varScope, wVarIndex, true);  // Let's consider this indexed.
}

Consumption _GetInstructionConsumption(ConsumptionNode &node, DecompileLookups &lookups)
{
    if (node._hasPos)
    {
        return _GetInstructionConsumption(*node.GetCode(), &lookups);
    }
    else
    {
        // A control structure
        Consumption consumption = {};
        consumption.cAccGenerate = 1;
        return consumption;
    }
}

void MorphInstructionIntoShortCircuit(scii &inst)
{
    switch (inst.get_opcode())
    {
        case Opcode::SAG:
            inst.set_opcode(Opcode::LAG);
            break;
        case Opcode::SAP:
            inst.set_opcode(Opcode::LAP);
            break;
        case Opcode::SAT:
            inst.set_opcode(Opcode::LAT);
            break;
        case Opcode::SAL:
            inst.set_opcode(Opcode::LAL);
            break;
        default:
            assert(false && "Unexpected short circuit");
    }
}

std::unique_ptr<SyntaxNode> _CodeNodeToSyntaxNode(ConsumptionNode &node, DecompileLookups &lookups)
{
    bool preferLValue = lookups.PreferLValue;
    PreferLValue resetPreferLValue(lookups, false); // Reset it, because if it's true it's not normal.

    if (!node._hasPos)
    {
        return _CodeNodeToSyntaxNode2(node, lookups);
    }

    scii inst = *node.pos;
    if (node.GetType() == ChunkType::ShortCircuitInstruction)
    {
        MorphInstructionIntoShortCircuit(inst);
    }

    Opcode bOpcode = inst.get_opcode();
    switch (bOpcode)
    {
        case Opcode::LDI:
        case Opcode::PUSHI:
        case Opcode::PUSH0:
        case Opcode::PUSH1:
        case Opcode::PUSH2:
        {
            unique_ptr<PropertyValue> value = std::make_unique<PropertyValue>();
            switch (bOpcode)
            {
                case Opcode::PUSH0:
                    value->SetValue(0);
                    break;
                case Opcode::PUSH1:
                    value->SetValue(1);
                    break;
                case Opcode::PUSH2:
                    value->SetValue(2);
                    break;
                default: // LDI and PUSHI
                    uint16_t theValue = inst.get_first_operand();
                    value->SetValue(theValue);
                    if (theValue == 65535)
                    {
                        // This was probably intended to be -1
                        value->Negate();
                    }
                    break;
            }
            return unique_ptr<SyntaxNode>(move(value));
        }
        break;

        case Opcode::PUSH:
            assert(node.Children().size() == 1);
            {
                // Fwd to the first child (should be what gets in to the accumulator)
                return _CodeNodeToSyntaxNode(*node.Children()[0], lookups);
            }
            break;

        case Opcode::CALLK:   // kernel, # of params
        case Opcode::CALLB:   // mainproc index, # of params
        case Opcode::CALLE:   // script, index, # of params
        case Opcode::CALL:    // offset, # of params
        {
            WORD cParams = (bOpcode == Opcode::CALLE) ? inst.get_third_operand() : inst.get_second_operand();
            cParams /= 2; // bytes -> params
            cParams += 1; // +1 because there is a parameter count first.
            std::stringstream ss;
            switch (bOpcode)
            {
                case Opcode::CALLK:
                    ss << lookups.LookupKernelName(inst.get_first_operand());
                    break;
                case Opcode::CALLB:
                    ss << _GetBaseProcedureName(inst.get_first_operand());
                    break;
                case Opcode::CALLE:
                    ss << _GetPublicProcedureName(inst.get_first_operand(), inst.get_second_operand());
                    break;
                case Opcode::CALL:
                    ss << _GetProcNameFromScriptOffset(inst.get_final_postop_offset() + inst.get_first_operand());
                    // Track this call so that we can say whether or not a local proc "belongs" to a class.
                    lookups.TrackProcedureCall(inst.get_final_postop_offset() + inst.get_first_operand());
                    break;
            }
            unique_ptr<ProcedureCall> pProcCall = std::make_unique<ProcedureCall>();
            pProcCall->SetName(ss.str());
            for (WORD i = 0; i < node.GetChildCount(); ++i)
            {
                if (i < cParams)
                {
                    if (i == 0)
                    {
                        // The first one should just be the number of parameters.
                        WORD cParamsCheck = _GetImmediateFromCodeNode(*node.Child(i));
                        ASSERT((cParamsCheck + 1) == cParams);
                    }
                    else
                    {
                        _ApplySyntaxNodeToCodeNode(*node.Child(i), *pProcCall, lookups);
                    }
                }
                else
                {
                    if (i == cParams)
                    {
                        // This might be a rest instruction.
                        _ApplySyntaxNodeToCodeNode(*node.Child(i), *pProcCall, lookups);
                    }
                    else
                    {
                        assert(false);  // How would we get here?
                    }
                }
            }

            return unique_ptr<SyntaxNode>(move(pProcCall));
        }
        break;

        case Opcode::SELF:
        case Opcode::SEND:
        case Opcode::SUPER:
        {
            unique_ptr<SendCall> sendCall = std::make_unique<SendCall>();
            if (bOpcode == Opcode::SELF)
            {
                sendCall->SetName(SelfToken);
            }
            else if (bOpcode == Opcode::SUPER)
            {
                // Actually you can send to any super class... the first operand says which. Does anyone use this?
                // Let's assert that they don't
                const sci::ClassDefinition *classDefinition = lookups.GetClassContext();
                assert(classDefinition);
                if (classDefinition)
                {
                    uint16_t species = inst.get_first_operand();
                    std::string superClassContext = classDefinition->GetSuperClass();
                    std::string superClassStated = lookups.LookupClassName(species);
                    assert(superClassContext == superClassStated);
                } // If we're in a proc without ownership, we can't assert anything. TODO insert comment warning about this.
                
                sendCall->SetName("super");
            }
            Consumption cons = _GetInstructionConsumption(inst, &lookups);
            WORD cStackPushesLeft = cons.cStackConsume;
            WORD cAccLeft = (bOpcode == Opcode::SEND) ? 1 : 0;
            size_t i = 0;
            WORD cParamsLeft = 0;
            bool fLookingForSelector = true;
            unique_ptr<SendParam> sendParam;

            while ((cAccLeft || cStackPushesLeft) && (i < node.GetChildCount()))
            {
                ConsumptionNode *pPreviousChild = i ? node.Child(i - 1) : nullptr;
                if (cAccLeft)
                {
                    Consumption consAcc = _GetInstructionConsumption(*node.Child(i), lookups);
                    if (consAcc.cAccGenerate)
                    {
                        // We want an LValue instead of a property value. Tell _CodeNodeToSyntaxNode this.
                        PreferLValue preferLValue(lookups, true);
                        unique_ptr<SyntaxNode> pSendObject = _CodeNodeToSyntaxNode(*node.Child(i), lookups);
                        const LValue *pValue = SafeSyntaxNode<LValue>(pSendObject.get());
                        if (pValue)
                        {
                            unique_ptr<LValue> lValue(static_cast<LValue*>(pSendObject.release()));
                            sendCall->SetLValue(move(lValue));
                        }
                        else
                        {
                            unique_ptr<SingleStatement> temp = make_unique<SingleStatement>();
                            temp->SetSyntaxNode(move(pSendObject));
                            sendCall->SetStatement1(move(temp));
                        }
                        --cAccLeft;
                    }
                }

                if (cStackPushesLeft)
                {
                    Consumption consAcc = _GetInstructionConsumption(*node.Child(i), lookups);
                    if (consAcc.cStackGenerate)
                    {
                        if (fLookingForSelector)
                        {
                            sendParam.reset(new SendParam);
                            fLookingForSelector = false;
                            bool found;
                            WORD wValue = _GetImmediateFromCodeNode(*node.Child(i), pPreviousChild, false, &found);
                            if (!found)
                            {
                                // Occasionally selectors can be variables.
                                std::unique_ptr<SyntaxNode> syntaxNode = _CodeNodeToSyntaxNode(*node.Child(i), lookups);
                                const LValue *pLValue = SafeSyntaxNode<LValue>(syntaxNode.get());
                                if (pLValue)
                                {
                                    sendParam->SetName(pLValue->GetName());
                                }
                                else
                                {
                                    throw ConsumptionNodeException(node.Child(i), "Expected selector.");
                                }
                            }
                            else
                            {
                                sendParam->SetName(lookups.LookupSelectorName(wValue));
                                sendParam->SetIsMethod(!lookups.IsPropertySelectorOnly(wValue));
                            }
                        }
                        else if (cParamsLeft)
                        {
                            --cParamsLeft;
                            _ApplySyntaxNodeToCodeNode(*node.Child(i), *sendParam, lookups); // Hmm, we were passing the wrong previous node here before... will that matter?
                            if (cParamsLeft == 0)
                            {
                                if (_MaybeConsumeRestInstruction(sendParam.get(), i + 1, node, lookups))
                                {
                                    i++;
                                }
                                sendCall->AddSendParam(std::move(sendParam));
                                fLookingForSelector = true;
                            }
                        }
                        else
                        {
                            // Must be a param count
                            cParamsLeft = _GetImmediateFromCodeNode(*node.Child(i), pPreviousChild);
                            if (cParamsLeft == 0)
                            {
                                if (_MaybeConsumeRestInstruction(sendParam.get(), i + 1, node, lookups))
                                {
                                    i++;
                                }
                                sendCall->AddSendParam(std::move(sendParam));
                                fLookingForSelector = true;
                            }
                        }
                        --cStackPushesLeft;
                    }
                }

                ++i; // Always increment i
                // TODO: warn if we didn't do anything in this loop?  Unused instruction???
            }

            assert((!sendCall->GetObjectA().empty() || sendCall->GetStatement1()) && "Send call with no object");
            sendCall->SimplifySendObject();
            return unique_ptr<SyntaxNode>(move(sendCall));
        }
        break;

        case Opcode::BNT:
        case Opcode::BT:
            assert(node.GetChildCount() == 1);
            if (node.GetChildCount() >= 1)
            {
                // This should have one child, which is the condition...
                return _CodeNodeToSyntaxNode(*node.Child(0), lookups);
            }
            break;

        case Opcode::SUB:
        case Opcode::MUL:
        case Opcode::DIV:
        case Opcode::MOD:
        case Opcode::SHR:
        case Opcode::SHL:
        case Opcode::XOR:
        case Opcode::AND:
        case Opcode::OR:
        case Opcode::ADD:
        case Opcode::EQ:
        case Opcode::GT:
        case Opcode::LT:
        case Opcode::LE:
        case Opcode::NE:
        case Opcode::GE:
        case Opcode::UGT:
        case Opcode::UGE:
        case Opcode::ULT:
        case Opcode::ULE:
            if (node.GetChildCount() >= 2)
            {
                unique_ptr<BinaryOp> binaryOp = std::make_unique<BinaryOp>();
                // The child that generates the stack push should be the first statement.
                ConsumptionNode *pCodeNodeStack = node.Child(0);
                ConsumptionNode *pCodeNodeAcc = node.Child(1);
                Consumption cons = _GetInstructionConsumption(*pCodeNodeStack, lookups);
                if (!cons.cStackGenerate)
                {
                    // Ooops... the other one must have been the stack-generating one.
                    swap(pCodeNodeStack, pCodeNodeAcc);
                }
                _ApplySyntaxNodeToCodeNode1(*pCodeNodeStack, *binaryOp, lookups);
                _ApplySyntaxNodeToCodeNode2(*pCodeNodeAcc, *binaryOp, lookups);
                binaryOp->SetName(GetBinaryOperatorForInstruction(bOpcode, LangSyntaxSCIStudio));
                return unique_ptr<SyntaxNode>(move(binaryOp));
            }
            break;

        case Opcode::BNOT:
        case Opcode::NOT:
        case Opcode::NEG:
            if (node.GetChildCount() >= 1)
            {
                unique_ptr<UnaryOp> unaryOp = std::make_unique<UnaryOp>();
                ConsumptionNode *pCodeNodeAcc = node.Child(0);
                Consumption cons = _GetInstructionConsumption(*pCodeNodeAcc, lookups);
                assert(cons.cAccGenerate);
                _ApplySyntaxNodeToCodeNode1(*pCodeNodeAcc, *unaryOp, lookups);
                unaryOp->SetName(GetUnaryOperatorForInstruction(bOpcode, LangSyntaxSCIStudio));
                return unique_ptr<SyntaxNode>(move(unaryOp));
            }
            break;

        case Opcode::REST:
        {
            unique_ptr<PropertyValue> pValue = std::make_unique<PropertyValue>();
            pValue->SetValue("rest", ValueType::Token);
            return unique_ptr<SyntaxNode>(move(pValue));
        }
        break;

        case Opcode::LEA:
        {
            WORD wType = inst.get_first_operand();
            bool hasIndexer = ((wType >> 1) & LEA_ACC_AS_INDEX_MOD) == LEA_ACC_AS_INDEX_MOD;
            if (hasIndexer)
            {
                unique_ptr<ComplexPropertyValue> pValue = std::make_unique<ComplexPropertyValue>();

                assert(node.GetChildCount() == 1);
                unique_ptr<SyntaxNode> pIndexerNode = _CodeNodeToSyntaxNode(*node.Child(0), lookups);
                unique_ptr<SingleStatement> temp = make_unique<SingleStatement>();
                temp->SetSyntaxNode(move(pIndexerNode));
                pValue->SetIndexer(move(temp));

                _ProcessLEA(*pValue, inst, lookups);
                return unique_ptr<SyntaxNode>(move(pValue));
            }
            else
            {
                unique_ptr<PropertyValue> pValue = std::make_unique<PropertyValue>();
                _ProcessLEA(*pValue, inst, lookups);
                return unique_ptr<SyntaxNode>(move(pValue));
            }
        }
        break;

        case Opcode::CLASS:
        case Opcode::PUSHSELF:
        {
            std::string className = (bOpcode == Opcode::CLASS) ? lookups.LookupClassName(inst.get_first_operand()) : SelfToken;
            if (!className.empty())
            {
                unique_ptr<PropertyValue> value = std::make_unique<PropertyValue>();
                value->SetValue(className, ValueType::Token);
                return unique_ptr<SyntaxNode>(move(value));
            }
        }
        break;

        case Opcode::LOFSA:
        case Opcode::LOFSS:
        {
            // The first operand specifies an offset from the start of the next instruction.
            // In SCI0 is this a relative offset from the post operation program counter.
            // In SCI1 it appears to be an absolute offset.
            ICompiledScriptSpecificLookups::ObjectType type;
            SCIVersion version = lookups.Helper.Version;
            uint16_t wName = version.lofsaOpcodeIsAbsolute ?
                inst.get_first_operand() :
                (inst.get_first_operand() + inst.get_final_postop_offset());
            std::string name = InvalidLookupError;
            // SQ4, main, there is a lofsa that points into the middle of a string.
            lookups.LookupScriptThing(wName, type, name);
            unique_ptr<PropertyValue> value = std::make_unique<PropertyValue>();
            value->SetValue(name, _ScriptObjectTypeToPropertyValueType(type));
            return unique_ptr<SyntaxNode>(move(value));
        }
        break;

        case Opcode::ATOP:    // acc to property index
        case Opcode::STOP:    // Stack to property index
        {
            unique_ptr<Assignment> pAssignment = std::make_unique<Assignment>();
            if (node.GetChildCount())
            {
                _ApplySyntaxNodeToCodeNode1(*node.Child(0), *pAssignment, lookups);
            }
            // Now this is a property... find out which.
            WORD wPropertyIndex = node.GetCode()->get_first_operand();
            unique_ptr<LValue> lValue = make_unique<LValue>();
            lValue->SetName(lookups.LookupPropertyName(wPropertyIndex));
            pAssignment->SetVariable(move(lValue));
            pAssignment->SetName("=");
            return unique_ptr<SyntaxNode>(move(pAssignment));
        }
        break;

        case Opcode::PTOS:    // property index to stack
        case Opcode::PTOA:    // property index to acc
        case Opcode::IPTOA:   // Inc prop to acc
        case Opcode::DPTOA:   // Dec prop to acc
        case Opcode::IPTOS:   // Inc prop to stack
        case Opcode::DPTOS:   // Dec prop to stack
        {
            unique_ptr<PropertyValue> pValue = std::make_unique<PropertyValue>();
            ASSERT(node.GetChildCount() == 0);
            WORD wPropertyIndex = node.GetCode()->get_first_operand();
            pValue->SetValue(lookups.LookupPropertyName(wPropertyIndex), ValueType::Token);
            bool fIncrement = (bOpcode == Opcode::IPTOA) || (bOpcode == Opcode::IPTOS);
            bool fDecrement = (bOpcode == Opcode::DPTOA) || (bOpcode == Opcode::DPTOS);
            if (fIncrement || fDecrement)
            {
                // Phil - TODO - copied code with below..
                unique_ptr<UnaryOp> pUnary = std::make_unique<UnaryOp>();
                pUnary->SetName(fIncrement ? "++" : "--");
                unique_ptr<SingleStatement> temp = make_unique<SingleStatement>();
                temp->SetSyntaxNode(std::move(pValue));
                pUnary->SetStatement1(std::move(temp));
                return unique_ptr<SyntaxNode>(move(pUnary));
            }
            else
            {
                if (preferLValue && (pValue->GetType() == ValueType::Token))
                {
                    // Come callers want an LValue instead
                    unique_ptr<LValue> lValue = make_unique<LValue>();
                    lValue->SetName(pValue->GetStringValue());
                    return unique_ptr<SyntaxNode>(move(lValue));
                }
                else
                {
                    return unique_ptr<SyntaxNode>(move(pValue));
                }
            }
        }
        break;

        case Opcode::DUP:
            assert(false); // In what conditions do we hit this? Should already be handled in switch statements.
            /*
            if (node.GetPrevious())
            {
                return _CodeNodeToSyntaxNode(*node.GetPrevious(), lookups);
            }
            else
            {
                // TODO: Walk backwards until we have an instruction that puts something on the stack.
                // This may not work in all cases (if we pass a branch, etc...)
                appState->LogInfo("WARNING: Possible incorrect logic.");
                // How do we handle this one?
                // assert(false);
            }*/
            break;

        case Opcode::JMP:
            // Return empty comment.
            return unique_ptr<SyntaxNode>(new Comment());
            // Already handled by branching code
            // OH BUT IS IT
            // I don't think this is needed anymore
            /*
            if (lookups.BreakExit._Ptr)
            {
                if (pos->get_branch_target() == (*lookups.BreakExit)->GetCode())
                {
                    // This jump points to the break exit.
                    return unique_ptr<SyntaxNode>(new BreakStatement());
                }
            }

            return unique_ptr<SyntaxNode>(new Comment());*/
            break;

        case Opcode::PPREV:
            // This is a special one. In our pre-processing, we should have found the "previous" accumulator
            // value that PPREV needs and inserted it as a child.
            assert(node.GetChildCount() == 1);
            if (node.GetChildCount() > 0)
            {
                return _CodeNodeToSyntaxNode(*node.Child(0), lookups);
            }
            break;

        case Opcode::LINK:
            return unique_ptr<SyntaxNode>(new Comment());
            break;

        case Opcode::RET:
        {
            unique_ptr<ReturnStatement> pReturn = std::make_unique<ReturnStatement>();
            if (node.GetChildCount())
            {
                _ApplySyntaxNodeToCodeNode1(*node.Child(0), *pReturn, lookups);
            }
            return unique_ptr<SyntaxNode>(move(pReturn));
            // TODO: Difficult to determine if the code actually meant to return anything...
            // we'll need to check if the previous thing was "intentional" or not...
            // We could have trouble with branching...
        }
        break;

        case Opcode::SELFID:
        {
            unique_ptr<PropertyValue> pv = make_unique<PropertyValue>();
            // REVIEW: Given that different syntaxes might use different keywords
            // for "self", it is questionable to have it as a string token here.
            pv->SetValue(SelfToken, ValueType::Token);
            return unique_ptr<SyntaxNode>(move(pv));
        }
        break;

        default:
            if ((bOpcode >= Opcode::LAG) && (bOpcode <= Opcode::LastOne))
            {
                // This could be a load or store operation.
                if (_IsVOStoreOperation(bOpcode))
                {
                    // Store operation: Assignment (LValue/PropertyValue)
                    unique_ptr<Assignment> pAssignment = std::make_unique<Assignment>();
                    pAssignment->SetName("=");

                    // The variable name
                    unique_ptr<LValue> lValue = make_unique<LValue>();
                    WORD wVarIndex;
                    VarScope varScope;
                    lValue->SetName(_GetVariableNameFromCodePos(inst, lookups, &varScope, &wVarIndex));
                    bool isIndexed = _IsVOIndexed(bOpcode);
                    lookups.TrackVariableUsage(varScope, wVarIndex, isIndexed);
                    if (isIndexed) // The accumulator is used as an indexer.
                    {
                        for (size_t i = 0; i < node.GetChildCount(); i++)
                        {
                            Consumption cons = _GetInstructionConsumption(*node.Child(i), lookups);
                            if (cons.cAccGenerate)
                            {
                                unique_ptr<SyntaxNode> pIndexerNode = _CodeNodeToSyntaxNode(*node.Child(i), lookups);
                                unique_ptr<SingleStatement> temp = make_unique<SingleStatement>();
                                temp->SetSyntaxNode(move(pIndexerNode));
                                lValue->SetIndexer(move(temp));
                                break;
                            }
                        }
                    }
                    pAssignment->SetVariable(move(lValue));

                    // Is assigned our child:
                    ASSERT(node.GetChildCount() >= 1);
                    for (size_t i = 0; i < node.GetChildCount(); i++)
                    {
                        Consumption cons = _GetInstructionConsumption(*node.Child(i), lookups);
                        if ((_IsVOStack(bOpcode) && cons.cStackGenerate) || (!_IsVOStack(bOpcode) && cons.cAccGenerate))
                        {
                            _ApplySyntaxNodeToCodeNode1(*node.Child(i), *pAssignment, lookups);
                        }
                    }
                    return unique_ptr<SyntaxNode>(move(pAssignment));
                }
                else
                {
                    // Load operation - make PropertyValue
                    std::unique_ptr<PropertyValueBase> pValue;

                    // Does it have an indexer?
                    bool isIndexed = _IsVOIndexed(bOpcode);
                    if (isIndexed)
                    {
                        pValue.reset(new ComplexPropertyValue);
                        // Then it should have a child
                        if (node.GetChildCount() >= 1)
                        {
                            unique_ptr<SingleStatement> pStatement = std::make_unique<SingleStatement>();
                            pStatement->SetSyntaxNode(std::move(_CodeNodeToSyntaxNode(*node.Child(0), lookups)));
                            static_cast<ComplexPropertyValue*>(pValue.get())->SetIndexer(std::move(pStatement));
                        }
                        else
                        {
                            ASSERT(FALSE); // REVIEW, TODO 
                        }
                    }
                    else
                    {
                        pValue.reset(new PropertyValue);
                    }

                    VarScope varScope;
                    WORD wIndex;
                    pValue->SetValue(_GetVariableNameFromCodePos(inst, lookups, &varScope, &wIndex), ValueType::Token);
                    lookups.TrackVariableUsage(varScope, wIndex, isIndexed);

                    // If it has an incrementer or decrementer, wrap it in a unary operator first.
                    bool fIncrement = _IsVOIncremented(bOpcode);
                    bool fDecrement = _IsVODecremented(bOpcode);
                    if (fIncrement || fDecrement)
                    {
                        unique_ptr<UnaryOp> pUnary = std::make_unique<UnaryOp>();
                        pUnary->SetName(fIncrement ? "++" : "--");
                        unique_ptr<SingleStatement> temp = make_unique<SingleStatement>();
                        temp->SetSyntaxNode(move(pValue));
                        pUnary->SetStatement1(std::move(temp));
                        return unique_ptr<SyntaxNode>(move(pUnary));
                    }
                    else
                    {
                        if (preferLValue && (pValue->GetType() == ValueType::Token))
                        {
                            // Come callers want an LValue instead
                            unique_ptr<LValue> lValue = make_unique<LValue>();
                            lValue->SetName(pValue->GetStringValue());
                            if (isIndexed)
                            {
                                lValue->SetIndexer(move(static_cast<ComplexPropertyValue*>(pValue.get())->StealIndexer()));
                            }
                            return unique_ptr<SyntaxNode>(move(lValue));
                        }
                        else
                        {
                            return unique_ptr<SyntaxNode>(move(pValue));
                        }
                    }
                }
            }
            break;
    }

    // This happens when a toss is used in weird circumstances, for instance.
    // e.g. SQ5, rightDome::doVerb.
    throw ConsumptionNodeException(&node, "Unexpected opcode.");
}

ConsumptionNode *_FindChunk(ConsumptionNode *chunk, code_pos code)
{
    ConsumptionNode *found = nullptr;
    if (chunk->_hasPos && chunk->GetCode() == code)
    {
        found = chunk;
    }
    if (!found)
    {
        for (auto &child : chunk->Children())
        {
            found = _FindChunk(child.get(), code);
            if (found)
            {
                break;
            }
        }
    }
    return found;
}


unique_ptr<ConsumptionNode> StealNode(ConsumptionNode *nodeToSteal, DecompileLookups &lookups)
{
    return nodeToSteal->_parentWeak->StealChild(nodeToSteal->GetMyIndex(), &lookups);
}

bool IsOpcodeWeCanShortCircuit(Opcode opcode)
{
    switch (opcode)
    {
        case Opcode::SAT:
        case Opcode::SAL:
        case Opcode::SAG:
        case Opcode::SAP:
            return true;
    }
    return false;
}

unique_ptr<ConsumptionNode> StealNodeOrReuseAcc(ConsumptionNode *nodeToSteal, DecompileLookups &lookups)
{
    // If the node we want to steal is a sa* operation (e.g. the accumulator is written to a variable, and acc retains the value)
    // then we can - instead of stealing - replace it with a la* operation (load var to acc)
    // If we reach here, We should be guaranteed that the acc is unchanged.
    if (nodeToSteal->_hasPos && IsOpcodeWeCanShortCircuit(nodeToSteal->GetCode()->get_opcode()))
    {
        unique_ptr<ConsumptionNode> newNode = make_unique<ConsumptionNode>();
        newNode->SetType(ChunkType::ShortCircuitInstruction);
        newNode->SetPos(nodeToSteal->GetCode());
        return newNode;
    }
    else
    {
        // Just steal
        return nodeToSteal->_parentWeak->StealChild(nodeToSteal->GetMyIndex(), &lookups);
    }
}

unique_ptr<ConsumptionNode> CloneNode(ConsumptionNode *nodeToSteal, DecompileLookups &lookups)
{
    return nodeToSteal->Clone();
}

template<typename _FuncCreate>
void ReplaceNodeWithNode(ConsumptionNode *nodeToBeReplaced, ConsumptionNode *nodeToStealOrClone, _FuncCreate funcCreate, DecompileLookups &lookups)
{
    unique_ptr<ConsumptionNode> stolen = funcCreate(nodeToStealOrClone, lookups);
    nodeToBeReplaced->_parentWeak->ReplaceChild(nodeToBeReplaced->GetMyIndex(), move(stolen));
}

bool GeneratesStack(const Consumption &consumption) { return consumption.cStackGenerate == 1; }
bool GeneratesAcc(const Consumption &consumption) { return consumption.cAccGenerate == 1; }

bool IsNodeStructureWithInstructionSequence(ConsumptionNode *node)
{
    switch (node->GetType())
    {
        case ChunkType::None:
        case ChunkType::Then:
        case ChunkType::Else:
        case ChunkType::LoopBody:
        case ChunkType::CaseBody:
        case ChunkType::FunctionBody:
            return true;
    }
    return false;
}

// The above one includes None, which I don't want in all cases?
bool IsNodeStructureWithInstructionSequence2(ConsumptionNode *node)
{
    switch (node->GetType())
    {
        case ChunkType::FunctionBody:
        case ChunkType::Then:
        case ChunkType::Else:
        case ChunkType::LoopBody:
        case ChunkType::CaseBody:
            return true;
    }
    return false;
}

bool SkipGuaranteedExecutions(ConsumptionNode *&child, ConsumptionNode *&parent)
{
    if ((parent->GetType() == ChunkType::Condition) && (parent->_parentWeak->GetType() == ChunkType::If))
    {
        parent = parent->_parentWeak->_parentWeak;
        child = child->_parentWeak->_parentWeak;
        return true;
    }
    else if ((parent->GetType() == ChunkType::First) &&
        ((parent->_parentWeak->GetType() == ChunkType::And) || (parent->_parentWeak->GetType() == ChunkType::Or)))
    {
        parent = parent->_parentWeak->_parentWeak;
        child = child->_parentWeak->_parentWeak;
        return true;
    }
    else
    {
        return false;
    }
}

// When looking for an accumulator value or stack value to steal, we can generally only proceed up through
// nodes that have instruction sequences.
// However, we can proceed up through other structured nodes if we can guarantee that
// the node we're in will always be executed after the parent node. For instance, a [condition] node in an
// [if] statement will always be executed once the code before the if is reached. The same is true for
// the [first] condition in a compound condition (but not the second).
bool SkipStructuredLevels(ConsumptionNode *&child, ConsumptionNode *&parent)
{
    while (parent && !IsNodeStructureWithInstructionSequence2(parent))
    {
        if (!SkipGuaranteedExecutions(child, parent))
        {
            // We can't proceed up anymore
            return false;
        }
    }
    return true;
}

template<typename _Func, typename _FuncCreate>
bool TryToStealOrCloneSomething(ConsumptionNode *originalChild, DecompileLookups &lookups, _Func satisfiesNeeds, _FuncCreate funcCreate)
{
    bool success = false;
    bool done = false;
    // Go up the parent chain and move back.
    // If we hit a structured node, we might need to stop, depending what it is.
    ConsumptionNode *child = originalChild->_parentWeak;
    ConsumptionNode *parent = child->_parentWeak;
    while (!done && parent)
    {
        int index = parent->GetIndexOf(child); // Start with previous peer of parent
        for (int i = index - 1; i >= 0; i--)
        {
            Consumption consumption = _GetInstructionConsumption(*parent->Child(i), lookups);
            if (satisfiesNeeds(consumption))
            {
                // This is it.
                ReplaceNodeWithNode(originalChild, parent->Child(i), funcCreate, lookups);
                success = true;
                done = true;
                break;
            }
        }

        if (!done)
        {
            // REVIEW: This may need to be in a loop when we make this more general.
            // Go up another level.
            child = parent;
            parent = child->_parentWeak;
            done = !SkipStructuredLevels(child, parent);
        }
    }
    return success;
}

// In later SCI, DUP is commonly as a one byte optimization when passing parameters, if two
// parameters in a row have the same value. DUPs can't be handled with our standard instruction consumption
// model, so handle them now by duplicating the previous stack-generating node.
void _ResolveDUPs(ConsumptionNode *root, ConsumptionNode *chunk, DecompileLookups &lookups)
{
    for (int i = 0; i < (int)chunk->GetChildCount(); i++)
    {
        ConsumptionNode *child = chunk->Child(i);
        if (child->_hasPos && (child->GetCode()->get_opcode() == Opcode::DUP))
        {
            bool found = false;
            // Look backward for a stack generator. Use ints because signed.
            for (int j = i - 1; !found && (j >= 0); j--)
            {
                Consumption consumption = _GetInstructionConsumption(*chunk->Child(j), lookups);
                found = (consumption.cStackGenerate == 1);
                if (found)
                {
                    // Found it. Replace our dup with this:
                    unique_ptr<ConsumptionNode> clone = chunk->Child(j)->Clone();
                    chunk->ReplaceChild(i, move(clone));
                }
            }
            if (!found)
            {
                found = TryToStealOrCloneSomething(child, lookups, GeneratesStack, CloneNode);
            }
            if (!found)
            {
                throw ConsumptionNodeException(chunk, "Couldn't find DUP source");
            }
        }
    }

    for (auto &child : chunk->Children())
    {
        _ResolveDUPs(root, child.get(), lookups);
    }
}

bool _LiftOutFromConditionsWorker(ConsumptionNode *root, ConsumptionNode *chunk, DecompileLookups &lookups)
{
    bool changes = false;

    switch (chunk->GetType())
    {
        case ChunkType::Condition:
        case ChunkType::First:
        case ChunkType::Second:
        case ChunkType::FirstNegated:
            if (chunk->GetChildCount() > 1)
            {
                ConsumptionNode *child = chunk->Child(0);
                ConsumptionNode *parent = chunk;
                // Let's see if we can lift the first child out.
                if (SkipStructuredLevels(child, parent) && child)
                {
                    // We can move chunk->Child(0) to before child in parent.
                    unique_ptr<ConsumptionNode> stolen = chunk->StealChild(0);
                    parent->InsertChild(parent->GetIndexOf(child), move(stolen));
                    changes = true;
                }
            }
            break;
    }

    if (!changes)
    {
        for (auto &child : chunk->Children())
        {
            changes = _LiftOutFromConditionsWorker(root, child.get(), lookups);
            if (changes)
            {
                break;
            }
        }
    }
    return changes;
}

// We can end up with multiple children of Conditions or First/Second nodes in a And/Or. Ideally we only
// want one. When we did our control flow analysis, we didn't always partition the same that we do now when
// construction the consumption node tree.
// (= temp0 5)
// (if (== ACC 4)
//    // code
// )
void _LiftOutFromConditions(ConsumptionNode *root, ConsumptionNode *chunk, DecompileLookups &lookups)
{
    bool changes = false;
    do
    {
        changes = _LiftOutFromConditionsWorker(root, chunk, lookups);
    } while (changes);
}

bool NoPreviousChildrenProduceAcc(ConsumptionNode *parent, ConsumptionNode *child, DecompileLookups &lookups)
{
    bool success = true;
    for (int i = (int)child->GetMyIndex() - 1; success && (i >= 0); i--)
    {
        success = (_GetInstructionConsumption(*parent->Child(i), lookups).cAccGenerate == 0);
    }
    return success;
}

bool CanLiftOut(ConsumptionNode *&child, ConsumptionNode *&parent, DecompileLookups &lookups)
{
    bool canLifeOut = false;
    bool done = false;
    // We can lift something out if:
    // - this assignment is a guaranteed execution (e.g. in a And/Or->First or an If->Condition)
    // - or the first in a "None" node
    // - we can satisfy the above while going up to an ordered instruction sequence.
    while (parent && !done)
    {
        if (parent->GetType() == ChunkType::None)
        {
            // We can't lift it out here, but we can keep going up as long as all the children before child
            // don't produce accumulators.
            if (NoPreviousChildrenProduceAcc(parent, child, lookups))
            {
                parent = parent->_parentWeak;
                child = child->_parentWeak;
            }
            else
            {
                done = true;
            }
        }
        else if (!SkipGuaranteedExecutions(child, parent))
        {
            // We'd better be at an instruction sequence node, otherwise we're done
            if (IsNodeStructureWithInstructionSequence2(parent))
            {
                // We can lift it out here.
                canLifeOut = true;
            }
            done = true;
        }
    }
    return canLifeOut;
}

bool _LiftOutAssignmentsWorker(ConsumptionNode *root, ConsumptionNode *chunk, DecompileLookups &lookups)
{
    bool changes = false;

    if (chunk->_hasPos &&
        IsOpcodeWeCanShortCircuit(chunk->GetCode()->get_opcode()) &&
        chunk->_parentWeak)
    {
        // See if we can pull this assignment out into an ordered instruction sequence.
        ConsumptionNode *parent = chunk->_parentWeak;
        ConsumptionNode *child = chunk;
        if (!IsNodeStructureWithInstructionSequence2(parent) && CanLiftOut(child, parent, lookups))
        {
            assert(child->_parentWeak == parent);
            assert(parent != chunk->_parentWeak);   // We should have gone up at least one level.

            // Now parent/child should point to the structure where we can lift this out.
            unique_ptr<ConsumptionNode> needsAccStub = make_unique<ConsumptionNode>();
            needsAccStub->SetType(ChunkType::NeedsAccumulator);
            unique_ptr<ConsumptionNode> nodeToLiftOut = chunk->_parentWeak->ReplaceChild(chunk->GetMyIndex(), move(needsAccStub));
            // Move it out:
            parent->InsertChild(child->GetMyIndex(), move(nodeToLiftOut));
            changes = true;
        }
    }

    if (!changes)
    {
        for (auto &child : chunk->Children())
        {
            changes = _LiftOutAssignmentsWorker(root, child.get(), lookups);
            if (changes)
            {
                break;
            }
        }
    }
    return changes;
}

void _LiftOutAssignments(ConsumptionNode *root, ConsumptionNode *chunk, DecompileLookups &lookups)
{
    bool changes = false;
    do
    {
        changes = _LiftOutAssignmentsWorker(root, chunk, lookups);
    } while (changes);
}


void _ResolvePPrevs(ConsumptionNode *root, ConsumptionNode *chunk, DecompileLookups &lookups)
{
    for (size_t i = 0; i < chunk->GetChildCount(); i++)
    {
        ConsumptionNode *child = chunk->Child(i);
        Consumption consumption = _GetInstructionConsumption(*child, lookups);
        if (consumption.cPrevConsume)
        {
            // Work backward to find a prev generator, then an acc generator.
            code_pos current = child->GetCode();
            --current;
            bool lookingForPrev = true;
            while (current->get_opcode() != Opcode::INDETERMINATE)
            {
                Consumption consumption = _GetInstructionConsumption(*current, &lookups);
                if (lookingForPrev && consumption.cPrevGenerate)
                {
                    lookingForPrev = false;
                }
                else if (!lookingForPrev && consumption.cAccGenerate)
                {
                    // Found the thing from the accumulator what we want to push.
                    ConsumptionNode *toClone = _FindChunk(root, current);
                    assert(toClone);
                    if (toClone)
                    {
                        unique_ptr<ConsumptionNode> theClone = toClone->Clone();
                        // Now we want to replace the prev with a push, and then put the cloned thing
                        // as the child of the push. Actually, let's not replace it. Let's just add
                        // the clone as a child of it.
                        assert(child->GetChildCount() == 0);
                        child->PrependChild(move(theClone));
                    }
                    break;
                }
                --current;
            }
        }
    }
    for (auto &child : chunk->Children())
    {
        _ResolvePPrevs(root, child.get(), lookups);
    }
}

unique_ptr<ConsumptionNode> _LookBackwardsAndFindAndCloneAccGenerator(ConsumptionNode *root, code_pos inclusiveStart, DecompileLookups &lookups)
{
    code_pos current = inclusiveStart;
    while (current->get_opcode() != Opcode::INDETERMINATE)
    {
        Consumption consumption = _GetInstructionConsumption(*current, &lookups);
        if (consumption.cAccGenerate)
        {
            // Found it. 
            ConsumptionNode *toClone = _FindChunk(root, current);
            assert(toClone);
            if (toClone)
            {
                return toClone->Clone();
            }
            break;
        }
        --current;
    }
    return nullptr;
}

// Returns true if changes were made
bool _ResolveNeededAccWorker(ConsumptionNode *root, ConsumptionNode *chunk, DecompileLookups &lookups)
{
    bool changes = false;
    // Go through the children in reverse order, because we might have multiple NeedsStack, and these need
    // to be processed backwards so the final ordering is correct.
    for (int i = (int)chunk->GetChildCount() - 1; !changes && (i >= 0); i--)
    {
        ConsumptionNode *child = chunk->Child(i);
        if (child->GetType() == ChunkType::NeedsAccumulator)
        {
            // We have two choices where we need an accumulator value. We can clone or steal.
            // We can only steal if we are in a place that is guarateed to execute from where we
            // currently are. For instance, if we're in an if Condition (without a compound condition)
            // we can steal from any code before the [If] structure that are peers of the if.
            // Let's start with a simple targeted case first.
            if (TryToStealOrCloneSomething(child, lookups, GeneratesAcc, StealNodeOrReuseAcc))
            {
                changes = true;
            }
            else
            {
                // Go back and find one to clone.
                code_pos start = chunk->GetCode();    // chunk, not child. Since a ChunkType::NeedsAccumulator node has no code, just a parent that needs it.
                --start;
                unique_ptr<ConsumptionNode> theClone = _LookBackwardsAndFindAndCloneAccGenerator(root, start, lookups);
                assert(theClone && "Couldn't find acc generator");
                if (theClone)
                {
                    chunk->ReplaceChild(i, move(theClone));
                }
                else
                {
                    assert(false && "Needed acc, but couldn't find it");
                    child->SetType(ChunkType::FailedToGetAccumulator);
                }
                // Cloning doesn't count as changing the tree, so just keep going.
            }
        }
        else if (child->GetType() == ChunkType::NeedsStack)
        {
            // We can only steal stack, never clone
            if (TryToStealOrCloneSomething(child, lookups, GeneratesStack, StealNode))
            {
                changes = true;
            }
            else
            {
                child->SetType(ChunkType::FailedToGetStack);
                throw ConsumptionNodeException(chunk, "Needed stack value, but couldn't find it");
            }
        }
    }

    if (!changes)
    {
        for (auto &child : chunk->Children())
        {
            changes = _ResolveNeededAccWorker(root, child.get(), lookups);
            if (changes)
            {
                break;
            }
        }
    }
    return changes;
}

void _ResolveNeededAcc(ConsumptionNode *root, ConsumptionNode *chunk, DecompileLookups &lookups)
{
    bool changes = false;
    do
    {
        changes = _ResolveNeededAccWorker(root, chunk, lookups);
    } while (changes);
}

unique_ptr<ConsumptionNode> _FindAndStealSwitchValue(vector<pair<ConsumptionNode*, size_t>> &frames, DecompileLookups &lookups)
{
    // Climb the stack and look for previous guy.
    ptrdiff_t frameIndex = frames.size() - 1;
    while (frameIndex >= 0)
    {
        // Start with the previous one at each frame.
        ptrdiff_t index = (ptrdiff_t)frames[frameIndex].second - 1;
        ConsumptionNode *parent = frames[frameIndex].first;
        while (index >= 0)
        {
            Consumption consumption = _GetInstructionConsumption(*parent->Child(index), lookups);
            assert(consumption.cAccGenerate == 0 && (consumption.cStackGenerate == 1));
            // If not ^^^ then maybe we need to go back more?
            return parent->StealChild(index, &lookups);
        }
        frameIndex--;
    }
    assert(false && "Couldn't find switch value");
    return nullptr;
}

void _FixupIfs(ConsumptionNode *root, ConsumptionNode *chunk, DecompileLookups &lookups)
{
    // If an if is used for consumption (e.g. an only child of an instruction that consumes acc)
    // then we need to ensure it has an else branch. e.g. SQ5, script 951.
    for (size_t i = 0; i < chunk->GetChildCount(); i++)
    {
        if (chunk->Child(i)->GetType() == ChunkType::If)
        {
            Consumption consumptionParent = _GetInstructionConsumption(*chunk, lookups);
            if (consumptionParent.cAccConsume)
            {
                // Ensure the if has an else
                ConsumptionNode *ifNode = chunk->Child(i);
                ConsumptionNode *elseNode = ifNode->GetChild(ChunkType::Else);
                if (elseNode == nullptr)
                {
                    // We actually don't need to scan backwards. We know that we came here as the result of
                    // a failed bnt operation. Therefore all we want is a FALSE in the accumulator.
                    unique_ptr<ConsumptionNode> newElse = make_unique<ConsumptionNode>();
                    newElse->SetType(ChunkType::Else);
                    unique_ptr<ConsumptionNode> zeroNode = make_unique<ConsumptionNode>();
                    zeroNode->SetType(ChunkType::ZeroNode);
                    newElse->PrependChild(move(zeroNode));
                    ifNode->PrependChild(move(newElse));
                }
            }
        }
    }

    for (auto &child : chunk->Children())
    {
        _FixupIfs(root, child.get(), lookups);
    }
}

void _FixupSwitches(ConsumptionNode *root, DecompileLookups &lookups)
{
    // Iteratively fixup switches.
    bool changes = true;
    while (changes)
    {
        changes = false;
        vector<pair<ConsumptionNode*, size_t>> frames;
        frames.push_back(pair<ConsumptionNode*, size_t>(root, 0));
        while (!frames.empty())
        {
            size_t index = frames.back().second;
            ConsumptionNode *current = frames.back().first;
            if (index < current->GetChildCount())
            {
                ConsumptionNode *child = current->Child(index);
                if (child->GetType() == ChunkType::Switch)
                {
                    if (!child->GetChild(ChunkType::SwitchValue))
                    {
                        // No switch value. Let's find it. All we need to do is to go up
                        // the parent chain and find a previous sibling of our parent that generates
                        // an accumulator value.
                        unique_ptr<ConsumptionNode> stolen = _FindAndStealSwitchValue(frames, lookups);
                        ConsumptionNode *switchValue = child->PrependChild(); // Add a new node that will be the switch value.
                        switchValue->SetType(ChunkType::SwitchValue);
                        switchValue->PrependChild(move(stolen));
                        changes = true;
                        break;
                    }
                }
                frames.push_back(pair<ConsumptionNode*, size_t>(child, 0));
            }
            else
            {
                frames.pop_back();
                if (!frames.empty())
                {
                    frames.back().second++;
                }
            }
        }
    }
}

void _RemoveDoubleInverts(ConsumptionNode *chunk)
{
    for (size_t i = 0; i < chunk->GetChildCount(); i++)
    {
        if (chunk->Child(i)->GetType() == ChunkType::Invert)
        {
            if (chunk->Child(i)->Child(0)->GetType() == ChunkType::Invert)
            {
                unique_ptr<ConsumptionNode> newChild = chunk->Child(i)->Child(0)->StealChild(0);
                chunk->ReplaceChild(i, move(newChild));
            }
        }
    }

    for (auto &child : chunk->Children())
    {
        _RemoveDoubleInverts(child.get());
    }
}

void _RemoveTOSS(ConsumptionNode *chunk)
{
    for (size_t i = 0; i < chunk->GetChildCount(); )
    {
        ConsumptionNode *child = chunk->Child(i);
        if (child->GetType() == ChunkType::Switch)
        {
            if (i < (chunk->GetChildCount() - 1))
            {
                // Look for a toss instruction and get rid of it.
                ConsumptionNode *next = chunk->Child(i + 1);
                assert((next->_hasPos) && (next->GetCode()->get_opcode() == Opcode::TOSS));
                unique_ptr<ConsumptionNode> deleteMe = chunk->StealChild(i + 1);
            }
        }
        i++;
    }
    for (auto &child : chunk->Children())
    {
        _RemoveTOSS(child.get());
    }
}

void _RestructureCaseHeaders(ConsumptionNode *chunk, DecompileLookups &lookups)
{
    if (chunk->GetType() == ChunkType::CaseCondition)
    {
        // This should look like this:
        // [CaseCondition]
        //   bnt
        //      eq ?
        //        dup
        //         ldi   // Something that puts stuff in the accumulator
        //
        // We want to restructure it so it looks like:
        // [CaseCondition]
        //   ldi   // Something that puts stuff in the accumulator
        //
        ConsumptionNode *bnt = chunk->Child(0);
        ConsumptionNode *eq = bnt->Child(0);
        ConsumptionNode *dup = eq->Child(0);
        unique_ptr<ConsumptionNode> putInAcc = eq->StealChild(1);
        assert(_GetInstructionConsumption(*putInAcc, lookups).cAccGenerate == 1);
        unique_ptr<ConsumptionNode> deleteMe = chunk->StealChild(0);
        assert(chunk->GetChildCount() == 0);
        chunk->PrependChild(move(putInAcc));
    }

    for (auto &child : chunk->Children())
    {
        _RestructureCaseHeaders(child.get(), lookups);
    }
}

bool OutputNewStructure(const std::string &messagePrefix, sci::FunctionBase &func, MainNode &main, DecompileLookups &lookups)
{
    CodeChunkEnumContext context(lookups);
    unique_ptr<ConsumptionNode> mainChunk = make_unique<ConsumptionNode>();
    mainChunk->SetType(ChunkType::FunctionBody);
    context.Current = mainChunk.get();

    string debugTrackName = GetMethodTrackingName(func.GetOwnerClass(), func, true);
    bool showFile = lookups.DebugInstructionConsumption && (!lookups.pszDebugFilter || (PathMatchSpec(debugTrackName.c_str(), lookups.pszDebugFilter)));

    try
    {
        EnumerateCodeChunks enumCodeChunks(context, lookups);
        enumCodeChunks.Visit(main);

        if (showFile)
        {
            std::stringstream ss;
            mainChunk->Print(ss, 0);
            ShowTextFile(ss.str().c_str(), debugTrackName + "_chunks_raw.txt");
        }

        _RemoveDoubleInverts(mainChunk.get());
        _RemoveTOSS(mainChunk.get());
        _FixupSwitches(mainChunk.get(), lookups);
        _FixupIfs(mainChunk.get(), mainChunk.get(), lookups);
        _LiftOutFromConditions(mainChunk.get(), mainChunk.get(), lookups);

        _LiftOutAssignments(mainChunk.get(), mainChunk.get(), lookups);

        _ResolveNeededAcc(mainChunk.get(), mainChunk.get(), lookups);
        _ResolvePPrevs(mainChunk.get(), mainChunk.get(), lookups);
        _RestructureCaseHeaders(mainChunk.get(), lookups);
        _ResolveDUPs(mainChunk.get(), mainChunk.get(), lookups);    // Must follow case restructure

        if (showFile)
        {
            std::stringstream ss;
            mainChunk->Print(ss, 0);
            ShowTextFile(ss.str().c_str(), debugTrackName + "_chunks_final.txt");
        }

        // Now fill it in
        for (auto &child : mainChunk->Children())
        {
            _ApplySyntaxNodeToCodeNode(*child, func, lookups);
        }

    }
    catch (ConsumptionNodeException &e)
    {
        if (showFile)
        {
            std::stringstream ss;
            mainChunk->Print(ss, 0);
            ShowTextFile(ss.str().c_str(), debugTrackName + "_chunks.txt");
        }

        string message;
        if (e.node->_hasPos)
        {
            message = fmt::format("{0}: {1}: {2} at {3:04x}", messagePrefix, e.message, (int)e.node->GetType(), e.node->GetCode()->get_final_offset_dontcare());
        }
        else
        {
            message = fmt::format("{0}: {1}: {2}", messagePrefix, e.message, (int)e.node->GetType());
        }
        lookups.DecompileResults().AddResult(DecompilerResultType::Warning, message);
        return false;
    }
    return true;
}