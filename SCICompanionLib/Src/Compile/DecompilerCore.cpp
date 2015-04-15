#include "stdafx.h"
#include "AppState.h"
#include "DecompilerCore.h"
#include "PMachine.h"
#include "ScriptOMAll.h"
#include "scii.h"
#include "DisassembleHelper.h"
#include "ControlFlowGraph.h"
#include <iterator>

#define DEBUG_DECOMPILER 1

using namespace sci;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const char InvalidLookupError[] = "LOOKUP_ERROR";
const char RestParamName[] = "params";
const char SelfToken[] = "self";

ValueType _ScriptObjectTypeToPropertyValueType(ICompiledScriptSpecificLookups::ObjectType type)
{
    switch (type)
    {
    case ICompiledScriptSpecificLookups::ObjectTypeSaid:
        return ValueType::Token; // since it already includes the ' '
        break;
    case ICompiledScriptSpecificLookups::ObjectTypeString:
        return ValueType::String;
        break;
    case ICompiledScriptSpecificLookups::ObjectTypeClass:
        return ValueType::Token;
        break;
    }
    return ValueType::Token;
}

//
// Represents whether an instruction consumes or generates stack or accumulator
// In general, values will be 0 or 1, except for cStackConsume which could be
// much larger (e.g. for send or call instruction)
//
struct Consumption
{
    Consumption() 
    {
        cAccConsume = 0;
        cStackConsume = 0;
        cAccGenerate = 0;
        cStackGenerate = 0;
    }

    int cAccConsume;
    int cStackConsume;
    int cAccGenerate;
    int cStackGenerate;
};

bool _IsVOIndexed(Opcode bOpcode)
{
    return !!(static_cast<BYTE>(bOpcode) & 0x08);
}
bool _IsVOPureStack(Opcode bOpcode)
{
    return !!(static_cast<BYTE>(bOpcode) & 0x04);
}
bool _IsVOStack(Opcode bOpcode)
{
    // It's a stack operation if it says it's a stack operation,
    // or if the accumulator is being used as an index.
    // WARNING PHIL: true for store, but maybe no load???
    return _IsVOPureStack(bOpcode) || _IsVOIndexed(bOpcode);
}
bool _IsVOStoreOperation(Opcode bOpcode)
{
    return ((static_cast<BYTE>(bOpcode) & 0x30) == 0x10);
}
bool _IsVOIncremented(Opcode bOpcode)
{
    return ((static_cast<BYTE>(bOpcode) & 0x30) == VO_INC_AND_LOAD);
}
bool _IsVODecremented(Opcode bOpcode)
{
    return ((static_cast<BYTE>(bOpcode) & 0x30) == VO_DEC_AND_LOAD);
}

//
// A currently abandoned attempt at writing a decompiler.  Not even close to being done.
// 

//
// De-compiler notes
//
// We'll to, in DisassembleFunction, arrange them into scii instructions.  Point the branches to each other.
// Each instruction needs to claim whether it puts something onto the stack or acc.
// Some things start new lines, line assignments, sends, calls.
// Could look for patterns. e.g.
// *IF-ELSE*
//      blah
//      bnt A
//      code
//      jmp B
//  A:  code
//  B:  code
//
//  *DO*
//  A:  code
//      code
//      bt A
//
//  *WHILE*
//      code
//  A:  bnt B
//      code
//      jmp A:
//
//
// We don't have goto statements, so jmps are a problem.
//  I think they only happen in if and switches and loops.
//
// We could have tables of common param names (init/client) and var names ?
//
//
//

std::string _GetPublicProcedureName(WORD wScript, WORD wIndex)
{
    std::stringstream ss;
    ss << "proc" << wScript << "_" << wIndex;
    return ss.str();
}


typedef std::list<scii>::reverse_iterator rcode_pos;

struct Fixup
{
    code_pos branchInstruction;
    WORD wTarget;
    bool fForward;
};

code_pos get_cur_pos(std::list<scii> &code)
{
    code_pos pos = code.end();
    --pos;
    return pos;
}

// 
// pBegin/pEnd - bounding pointers for the raw byte code
// wBaseOffset - byte offset in script file where pBegin is (used to calculate absolute code offsets)
// code        - (out) list of sci instructions.
//
void _ConvertToInstructions(std::list<scii> &code, const BYTE *pBegin, const BYTE *pEnd, WORD wBaseOffset)
{
    std::unordered_map<WORD, code_pos> referenceToCodePos;
    std::vector<Fixup> branchTargetsToFixup;
    std::set<uint16_t> branchTargets;

    code_pos undetermined = code.end();

    WORD wReferencePosition = 0;
    const BYTE *pCur = pBegin;
    while (pCur < pEnd)
    {
        const BYTE *pThisInstruction = pCur;
        BYTE bRawOpcode = *pCur;
        bool bByte = (*pCur) & 1;
        Opcode bOpcode = static_cast<Opcode>(bRawOpcode >> 1);
        ASSERT(bOpcode <= Opcode::LastOne);
        ++pCur; // Advance past opcode.
        WORD wOperands[3];
        ZeroMemory(wOperands, sizeof(wOperands));
        int cIncr = 0;
        for (int i = 0; i < 3; i++)
        {
            OperandType opType = OpArgTypes[static_cast<BYTE>(bOpcode)][i];
            cIncr = GetOperandSize(bRawOpcode, opType);
            switch (cIncr)
            {
            case 1:
                // We may need to sign-extend this.
                if ((opType == otINT) || (opType == otINT8))
                {
                    wOperands[i] = (uint16_t)(int16_t)(int8_t)*pCur;
                }
                else
                {
                    wOperands[i] = (WORD)*pCur;
                }
                break;
            case 2:
                wOperands[i] = *((WORD*)pCur); // REVIEW
                break;
            default:
                break;
            }
            pCur += cIncr;
            if (cIncr == 0) // No more operands
            {
                break;
            }
        }
        // Add the instruction - use the constructor that takes all arguments, even if
        // not all are valid.
        if ((bOpcode == Opcode::BNT) || (bOpcode == Opcode::BT) || (bOpcode == Opcode::JMP))
        {
            // +1 because its the operand start pos.
            WORD wTarget = CalcOffset(wReferencePosition + 1, wOperands[0], bByte, bRawOpcode);
            code.push_back(scii(bOpcode, undetermined, true));
            bool fForward = (wTarget > wReferencePosition);
            Fixup fixup = { get_cur_pos(code), wTarget, fForward };
            branchTargetsToFixup.push_back(fixup);
            branchTargets.insert(wTarget);
        }
        else
        {
            code.push_back(scii(bOpcode, wOperands[0], wOperands[1], wOperands[2]));
        }

        // Store the position of the instruction we just added:
        referenceToCodePos[wReferencePosition] = get_cur_pos(code);
        // Store the actual offset in the instruction itself:
        WORD wSize = (WORD)(pCur - pThisInstruction);
        get_cur_pos(code)->set_offset_and_size(wReferencePosition + wBaseOffset, wSize);

        // Attempt to detect the end of the function. If we encounter a return staetment and it's equal to or beyond anything in branchTargetsToFixup,
        // then we have reached the end.
        if (bOpcode == Opcode::RET)
        {
            if (branchTargets.empty() || ((*branchTargets.rbegin()) <= wReferencePosition))
            {
                // We've reached the end
                break;
            }
        }


        wReferencePosition += wSize;
    }

    // Now fixup any branches.
    for (size_t i = 0; i < branchTargetsToFixup.size(); i++)
    {
        Fixup &fixup = branchTargetsToFixup[i];
        std::unordered_map<WORD, code_pos>::iterator it = referenceToCodePos.find(fixup.wTarget);
        if (it != referenceToCodePos.end())
        {
            fixup.branchInstruction->set_branch_target(it->second, fixup.fForward);
        }
        else
        {
            assert(false && "Branch to an invalid spot.");
        }
    }

    // Special hack... function code is placed at even intervals.  That means there might be an extra bogus
    // instruction at the end, just after a ret statement (often it is Opcode::BNOT, which is 0).  If the instruction
    // before the end is a ret instruction, remove the last instruction (unless it's also a ret - since sometimes
    // functions will end with two RETs, both of which are jump targets).
    if (code.size() > 1)
    {
        code_pos theEnd = code.end();
        --theEnd; // Last instruction
        code_pos maybeRET = theEnd;
        --maybeRET;
        if ((maybeRET->get_opcode() == Opcode::RET) && (theEnd->get_opcode() != Opcode::RET))
        {
            code.erase(theEnd);
        }
    }
}

struct BranchBlock
{
    code_pos begin;
    code_pos end;
};

bool IsDelineatingInstruction(code_pos pos)
{
    Opcode bOpcode = pos->get_opcode();
    return (bOpcode == Opcode::BNT) || (bOpcode == Opcode::BT) || (bOpcode == Opcode::JMP);
}
bool IsBranchInstruction(code_pos pos)
{
    return IsDelineatingInstruction(pos);
}

bool _IsVariableUse(code_pos pos, VarScope varScope, WORD &wIndex)
{
    bool fRet = false;
    Opcode bOpcode = pos->get_opcode();
    if (OpArgTypes[static_cast<BYTE>(bOpcode)][0] == otVAR)
    {
        // It's a variable opcode.
        wIndex = pos->get_first_operand();
        fRet = (static_cast<VarScope>(static_cast<BYTE>(bOpcode) & 0x03) == varScope);
    }
    else if (bOpcode == Opcode::LEA)
    {
        // The "load effective address" instruction
        WORD wType = pos->get_first_operand();
        fRet = (static_cast<VarScope>(static_cast<BYTE>(bOpcode) & 0x03) == varScope);
        if (fRet)
        {
            wIndex = pos->get_second_operand();
        }
    }
    return fRet;
}

std::string _GetSuitableParameterName(FunctionBase &function, WORD iIndex)
{
    if ((function.GetName() == "changeState") && (iIndex == 1))
    {
        return "newState";
    }
    if ((function.GetName() == "handleEvent") && (iIndex == 1))
    {
        return "pEvent";
    }
    std::stringstream ss;
    ss << _GetParamVariableName(iIndex);
    return ss.str();
}

void _FigureOutParameters(FunctionBase &function, FunctionSignature &signature, std::list<scii> &code)
{
    WORD wBiggest = 0;
    for (code_pos pos = code.begin(); pos != code.end(); ++pos)
    {
        WORD wIndex;
        if (_IsVariableUse(pos, VarScope::Param, wIndex)) // 3 -> param
        {
            wBiggest = max(wIndex, wBiggest);
        }
    }
    if (wBiggest) // Parameters start at index 1, so 0 means no parameters
    {
        for (WORD i = 1; i <= wBiggest; i++)
        {
            std::unique_ptr<FunctionParameter> pParam = std::make_unique<FunctionParameter>();
            pParam->SetName(_GetSuitableParameterName(function, i));
            pParam->SetDataType("var"); // Generic for now... we could try to detect things in the future.
            signature.AddParam(std::move(pParam), false);   // false -> hard to detect if optional or not... serious code analysis req'd
        }
    }
}

//
// Scans the code for local variable usage, and adds "temp0, temp1, etc..." variables to function.
//
template<typename _TVarHolder>
void _FigureOutTempVariables(DecompileLookups &lookups, _TVarHolder &function, VarScope varScope, std::list<scii> &code)
{
    // Look for the link instruction.
    WORD cTotalVariableRoom = 0;
    for (code_pos pos = code.begin(); pos != code.end(); ++pos)
    {
        if (pos->get_opcode() == Opcode::LINK)
        {
            cTotalVariableRoom = pos->get_first_operand();
            break;
        }
    }

    if (cTotalVariableRoom)
    {
        // So, I don't know if I can put the things in initializers. That would involve extra work.
        // So I won't bother (I'll just leave the assignment ops)
        std::string methodTrackingName = GetMethodTrackingName(function.GetOwnerClass(), function);
        auto &localUsage = lookups.GetTempUsage(methodTrackingName);
        vector<VariableRange> varRanges;
        CalculateVariableRanges(localUsage, cTotalVariableRoom, varRanges);

        // Ok, now I have the ranges. Without doing extra work to find assignment opcodes that come at
        // the begining of the function, I'll just avoid putting any initializers in the variable decls.
        for (VariableRange &varRange : varRanges)
        {
            unique_ptr<VariableDecl> tempVar = std::make_unique<VariableDecl>();
            tempVar->SetDataType("var"); // For now...
            tempVar->SetName(_GetTempVariableName(varRange.index));
            tempVar->SetSize(varRange.arraySize);
            function.AddVariable(move(tempVar));
        }
    }
}

std::string _indent(int iIndent)
{
    std::string theFill;
    theFill.insert(theFill.begin(), iIndent, ' ');
    return theFill;
}


// Fwd decl
class CodeShape;

//
// Attempt at generating an instruction tree.  Used to turn a flat sequence of
// instructions into a tree so that, for example, a send call at the root, would
// have each of its parameters.
//
// e.g. if (5 == gEgo.x)
// would be something like:
//
//   bnt
//     eq?
//        ldi 5
//        push
//          send
//            push0
//            pushi
//
class CodeNode
{
public:
    CodeNode(code_pos pos) : _pos(pos), _targetLabel(0), _previousNodeWeak(nullptr) {}

    CodeNode(const CodeNode &src) = delete;
    CodeNode& operator=(const CodeNode& src) = delete;

    void SetCode(code_pos pos) { _pos = pos; }
    code_pos GetCode() const { return _pos; }
    void AddChildToFront(std::unique_ptr<CodeNode> pNode)
    {
        // It appears we always add to the beginning. So go to the first guy
        // and set the new node as his previous
        if (!_children.empty())
        {
            _children[0]->_SetPrevious(pNode.get());
        }

        _children.insert(_children.begin(), std::move(pNode));
    }
    size_t GetChildCount() { return _children.size(); }
    CodeNode *Child(size_t i) { return _children[i].get(); }

    std::vector<std::unique_ptr<CodeNode>>::iterator begin() { return _children.begin(); }
    std::vector<std::unique_ptr<CodeNode>>::iterator end() { return _children.end(); }

    // For debugging
    void SetLabel(char c) { if (!_label.empty()) { _label += ","; }  _label += c; }
    void SetTargetLabel(char c) { _targetLabel = c; }

    void Print(std::ostream &os, int iIndent) const
    {
        os << _indent(iIndent);
        if (!_label.empty())
        {
            os << _label << ":";
        }
        os << OpcodeNames[static_cast<BYTE>(_pos->get_opcode())];
        if (_targetLabel)
        {
           os << "--> " << _targetLabel; 
        }
        os << "\n";

        for (auto &child : _children)
        {
            child->Print(os, iIndent + 2);
        }
    }

    CodeNode *GetPrevious() { return _previousNodeWeak; }

private:
    void _SetPrevious(CodeNode *previous)
    {
        assert(_previousNodeWeak == nullptr);
        _previousNodeWeak = previous;
    }
    CodeNode *_previousNodeWeak;
    code_pos _pos;										// The instruction
    std::vector<std::unique_ptr<CodeNode>> _children;   // The child code nodes
    std::string _label;									// For debugging
    char _targetLabel;									// For debugging
};

typedef std::vector<std::unique_ptr<CodeNode>>::iterator codenode_it;

Consumption _GetInstructionConsumption(scii &inst)
{
    Opcode bOpcode = inst.get_opcode();
    ASSERT(bOpcode <= Opcode::LastOne);
    int cEatStack = 0;
    bool fChangesAcc = false;
    bool fEatsAcc = false;
    bool fPutsOnStack = false;
    switch (bOpcode)
    {
    case Opcode::SELF:
        cEatStack = inst.get_first_operand() / 2;
        fChangesAcc = true;
        break;

    case Opcode::SEND:
        cEatStack = inst.get_first_operand() / 2;
        fChangesAcc = true;
        fEatsAcc = true;
        break;

    case Opcode::SUPER:
        cEatStack = inst.get_second_operand() / 2;
        fChangesAcc = true;
        break;

    case Opcode::BNOT:
    case Opcode::NOT:
    case Opcode::NEG:
        fChangesAcc = true;
        fEatsAcc = true;
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
        fChangesAcc = true;
        fEatsAcc = true;
        cEatStack = 1;
        break;

    case Opcode::BT:
    case Opcode::BNT:
        fEatsAcc = true;
        break;

    case Opcode::RET:
        // Wreaks havoc
        // fEatsAcc = true; // But not always intentional...
        // Because we don't know the intent of the code, we can't really do anything here.
        // The acc could be valid, or garbage. So we can just do the "doesn't affect anything"
        // statement before the return here and call it good. SCISTudio and SICompanion both compile ok with that
        // (though SCICompanion gives warnings).
        // e.g.
        //	return (5)
        // will become
        //	5				// SCICompanion gives warning here
        //	return
        break;

    case Opcode::JMP:
    case Opcode::LINK:
        break;

    case Opcode::LDI:
        fChangesAcc = true;
        break;

    case Opcode::PUSH:
        fEatsAcc = true;
        fPutsOnStack = true;
        break;

    case Opcode::PUSHI:
    case Opcode::PUSH0:
    case Opcode::PUSH1:
    case Opcode::PUSH2:
        fPutsOnStack = true;
        break;

    case Opcode::PUSHSELF:
        fPutsOnStack = true;
        break;

    case Opcode::TOSS:
        //fChangesAcc = true; // doesn't touch the acc, by definition (toss)
        cEatStack = 1;
        break;

    case Opcode::DUP:
        fPutsOnStack = true;
        break;

    case Opcode::CALL:
    case Opcode::CALLK:
    case Opcode::CALLB:
        cEatStack = inst.get_second_operand() / 2;
        cEatStack++; // the number didn't include the # of instructions push.
        fChangesAcc = true;
        break;

    case Opcode::CALLE:
        //fEatsAcc = true;
        cEatStack = inst.get_third_operand() / 2;
        cEatStack++; // Also a parameter count.
        fChangesAcc = true;
        break;

    case Opcode::CLASS:
    case Opcode::SELFID:
        fChangesAcc = true;
        break;

    case Opcode::PPREV:
        fPutsOnStack = true;
        break;

    case Opcode::REST:
        // Doesn't really affect anything
        break;

    case Opcode::LEA:
        fEatsAcc = !!(static_cast<BYTE>(bOpcode) & LEA_ACC_AS_INDEX_MOD);
        fChangesAcc = true;
        break;

    case Opcode::PTOA:
    case Opcode::IPTOA:
    case Opcode::DPTOA:
        fChangesAcc = true;
        break;

    case Opcode::ATOP:
        fEatsAcc = true;
        fChangesAcc = true; 
        // Not technically, but it leaves the value in the accumulator, so
        // it's what people should look at.
        break;

    case Opcode::PTOS:
    case Opcode::IPTOS:
    case Opcode::DPTOS:
        fPutsOnStack = true;
        break;

    case Opcode::STOP:
        cEatStack = 1;
        break;

    case Opcode::LOFSA:
        fChangesAcc = true;
        break;

    case Opcode::LOFSS:
        fPutsOnStack = true;
        break;

    default:

//sali -> store acc in local, indexed by acc
//ac is local4
        assert((bOpcode >= Opcode::LAG) && (bOpcode <= Opcode::LastOne));
        // TODO: use our defines/consts
        if (_IsVOStoreOperation(bOpcode))
        {
            // Store operation
            if (_IsVOPureStack(bOpcode))
            {
                cEatStack = 1;
            }
            else
            {
                if (_IsVOStack(bOpcode))
                {
                    cEatStack = 1;
                }
                fEatsAcc = true;
                fChangesAcc = true; // Not really, but leaves a valid thing in the acc.
            }
        }
        else
        {
            // Load operation
            if (_IsVOPureStack(bOpcode))
            {
                fPutsOnStack = true;
            }
            else
            {
                fChangesAcc = true;
            }
        }
        if (_IsVOIndexed(bOpcode))
        {
            fEatsAcc = true; // index is in acc
        }
        break;

    }

    Consumption cons;
    if (fEatsAcc)
    {
        cons.cAccConsume++;
    }
    if (fChangesAcc)
    {
        cons.cAccGenerate++;
    }
    cons.cStackConsume = cEatStack;
    if (fPutsOnStack)
    {
        cons.cStackGenerate++;
    }

    return cons;
}

bool _IsJumpToPush(code_pos posJump, CodeNode *pNodePush)
{
    return (posJump->get_branch_target() == pNodePush->GetCode());
}

// fwd decl
void _InstructionTreeHelper(CodeNode &parent, code_pos begin, code_pos &end);

//
// end -> next instruction before push tree defined by pNodePush
//
code_pos _CheckForTernary(code_pos end, code_pos begin, CodeNode *pNodePush)
{
    code_pos endReturn = end;
    if ((end->get_opcode() == Opcode::JMP) && _IsJumpToPush(end, pNodePush))
    {
        // Good
        --end;
        if (end != begin)
        {
            // Now we're looking for some kind of accumulator thing.
            Consumption cons = _GetInstructionConsumption(*end);
            if (cons.cAccGenerate == 1)
            {
                ASSERT(cons.cStackGenerate == 0);
                std::unique_ptr<CodeNode> pFirstValue = std::make_unique<CodeNode>(end);
                _InstructionTreeHelper(*pFirstValue, begin, end);
                
                // Now see if we have a bnt
                if (end->get_opcode() == Opcode::BNT)
                {
                    // This is it.
                    std::unique_ptr<CodeNode> pCondition = std::make_unique<CodeNode>(end);
                    _InstructionTreeHelper(*pCondition, begin, end);

                    // Now we're going to do someting special.  Take the original push,
                    // and add some crap to it.
                    ASSERT(pNodePush->GetChildCount() == 1); // Should just be the "second" value.
                    // push
                    //      ldi (2)
                    //
                    // We want it to be:
                    // push
                    //      bnt
                    //          ???
                    //      ldi (1)
                    //      ldi (2)
                    pNodePush->AddChildToFront(std::move(pFirstValue));
                    pNodePush->AddChildToFront(std::move(pCondition));

                    // Update the end...
                    endReturn = end;
                }
            }
        }
    }
    return endReturn;
}

// Works backward from branchInstruction to beginning (inclusive) until the sequence of instructions leading up to the branch
// has been satisfied. If it fails, it returns false.
bool _ObtainInstructionSequence(code_pos endingInstruction, code_pos beginning, code_pos &beginningOfBranchInstructionSequence)
{
    bool success = true;

    code_pos beforeBeginning = beginning;
    --beforeBeginning; // Since beginning is inclusive.
    code_pos cur = endingInstruction;
    beginningOfBranchInstructionSequence = cur;
    Consumption consTemp = _GetInstructionConsumption(*endingInstruction);
    int cStackConsume = consTemp.cStackConsume;
    int cAccConsume = consTemp.cAccConsume;
    --cur;
    while (success && (cur != beforeBeginning) && (cStackConsume || cAccConsume))
    {
        Consumption consTemp = _GetInstructionConsumption(*cur);
        if (consTemp.cAccGenerate)
        {
            if (cAccConsume)
            {
                cAccConsume -= consTemp.cAccGenerate;
            }
            // else... is acc generate ok even if we're not consuming?
        }
        if (consTemp.cStackGenerate)
        {
            if (cStackConsume)
            {
                cStackConsume -= consTemp.cStackGenerate;
            }
            else
            {
                // Something put something on the stack and we didn't need it.
                // We can't do anything useful here.
                success = false;
            }
        }

        if (consTemp.cAccConsume || consTemp.cStackConsume)
        {
            // This instruction we just got consumes stuff. We need to recurse here and
            // have it eat up its stuff.
            // Afterwards, if success is true, cur will point to the beginning of its thing
            // and we can keep going (if necessary)
            success = _ObtainInstructionSequence(cur, beginning, cur);
        }

        beginningOfBranchInstructionSequence = cur;

        --cur;
    }
    return success;
}

// end is where you start looking.
void _InstructionTreeHelper(CodeNode &parent, code_pos begin, code_pos &end)
{
    if (end != begin)
    {
        // 1) Ok, now check what this thing eats
        // 2) Work backwards and add children until that hunger is satisfied.
        Consumption consTemp = _GetInstructionConsumption(*end);
        int cStackConsume = consTemp.cStackConsume;
        int cAccConsume = consTemp.cAccConsume;

        --end;

        bool fHadAPush = false;
        CodeNode *pNodePush = nullptr;
        bool fHadAJump = false;
        bool fHadAccumulatorThing = false;

        code_pos posAnomalyBegin = begin;;

        // Now we try to bring the stack and acc balance to zero.
        while((end != begin) && ((cStackConsume > 0) || (cAccConsume > 0)))
        {
            Consumption cons = _GetInstructionConsumption(*end);

            // REVIEW -> consider only doing acc stuff for the first thing..?
            cStackConsume -= cons.cStackGenerate;
            cAccConsume -= cons.cAccGenerate;
            // Bound the acc consumption at 0... many things put stuff into the acc, but it doesn't mean it
            // should be used.
            cAccConsume = max(0, cAccConsume); 
            // On the other hand, the stack should never go below 0 if the code is good - we probably need to handle and bail
            // once we've got the analysis down pat
            if (cStackConsume < 0)
            {
                // We have an issue here.  This could arise in a case like this:
                // ldi 5
                // pushi 6
                // push   // uses the value in the accumulator, which is 5
                // 
                // We need to ignore the pushi 6 then, but only for right here....
                if (posAnomalyBegin == begin)
                {
                    // This was our first anomoly.  When we exit the function end should be set to this.
                    posAnomalyBegin = end;
                }
                cStackConsume = 0;
                // ignore this instruction - we took note of it in posAnomaly;
                --end;

                // REVIEW: We may need a better model... we're essentially skipping instructions.  They should be put
                // back into a queue to be used somewhere.  Think of the following:
                // class
                // <switch statement stuff where each case puts something in the acc>
                // toss
                // push  //-> we'll let the toss do its thing and become a switch statement, and accidentally
                //       //   eat the class before the switch statement.
                // Ok -> I guess this is unrelated.  Do we need a special hack for this???
            }
            else
            {
                // ASSERT for now, though we'll need to deal with this:
                ASSERT((cons.cStackConsume >= 0) && (cons.cAccConsume >= 0));

                std::unique_ptr<CodeNode> pChild = std::make_unique<CodeNode>(end);

                // If it consumes anything, then call recursively
                if (cons.cStackConsume || cons.cAccConsume)
                {
                    _InstructionTreeHelper(*pChild, begin, end);

                    // REVIEW HACK
                    if (pChild->GetCode()->get_opcode() == Opcode::PUSH)
                    {
                        end = _CheckForTernary(end, begin, pChild.get());
                    }
                }
                else
                {
                    --end;
                }
                parent.AddChildToFront(std::move(pChild));
            }
        }

        if (posAnomalyBegin != begin)
        {
            // We had to skip some instructions - so reset end to point to where we
            // had to start skipping.
            end = posAnomalyBegin;
        }
    }
}

void _GenerateInstructionTree(CodeNode &root, code_pos begin, code_pos &end)
{
    if (end != begin) // At least on instruction
    {
        _InstructionTreeHelper(root, begin, end);
    }
}

class PreferLValue
{
public:
	PreferLValue(DecompileLookups &lookups, bool prefer) : _lookups(lookups)
	{
		_preferLValueOld = lookups.PreferLValue;
		lookups.PreferLValue = true;
	}
	~PreferLValue()
	{
		_lookups.PreferLValue = _preferLValueOld;
	}
private:
	bool _preferLValueOld;
	DecompileLookups &_lookups;
};

class BreakExit
{
public:
    BreakExit(DecompileLookups &lookups, codenode_it breakExit) : _lookups(lookups)
    {
        _breakExitOld = lookups.BreakExit;
        lookups.BreakExit = breakExit;
    }
    ~BreakExit()
    {
        _lookups.BreakExit = _breakExitOld;
    }
private:
    codenode_it _breakExitOld;
    DecompileLookups &_lookups;
};

// fwd decl
std::unique_ptr<SyntaxNode> _CodeNodeToSyntaxNode(CodeNode &node, DecompileLookups &lookups);
std::unique_ptr<SyntaxNode> _CodeNodeToSyntaxNode2(CodeNode &node, DecompileLookups &lookups);
void _ApplySyntaxNodeToCodeNode(CodeNode &node, StatementsNode &statementsNode, DecompileLookups &lookups);
void _ApplySyntaxNodeToCodeNode1(CodeNode &node, OneStatementNode &statementsNode, DecompileLookups &lookups);
void _ApplySyntaxNodeToCodeNode2(CodeNode &node, TwoStatementNode &statementsNode, DecompileLookups &lookups);

// Determines if a code_pos is within a CodeNode...
// TODO: really should just check if a CodeNode *starts* with a code_pos
bool _ScanCodeNodeForCodePos(CodeNode *pNode, code_pos pos)
{
    bool fRet = (pNode->GetCode() == pos);
    for (size_t i = 0; !fRet && (i < pNode->GetChildCount()); i++)
    {
        fRet = _ScanCodeNodeForCodePos(pNode->Child(i), pos);
    }
    return fRet;
}

//
// Scans a series of CodeNodes to find one that contains with code_pos.
// (note: should really be the one that starts with the code_pos, and by starts with
//  we don't mean it's at the root of the hierarchy, but simply the first instruction
//  that appears in code)
// If none are found, then it returns end.
// If one is found, it doesn't returns the codenode_it with the code_pos (because there
// may be no codenode_it), but the 'root' codenode_it.
//
codenode_it _FindCodeNodeWithCodePos(codenode_it begin, codenode_it end, code_pos pos)
{
    while ((begin != end) && !_ScanCodeNodeForCodePos((*begin).get(), pos))
    {
        ++begin;
    }
    return begin;
}

//
// Defines the "shape" of code, such as if statements, switch statements, loops, etc....
//
class CodeShape
{
public:
    CodeShape() = default;
    CodeShape(const CodeShape &src) = delete;
    CodeShape &operator=(const CodeShape &src) = delete;

    // philtest
    virtual ~CodeShape()
    {

    }

    virtual unique_ptr<SingleStatement> DoIt(DecompileLookups &lookups) = 0;
    virtual void Print(std::ostream &os, int iIndent) const = 0;
};

typedef std::vector<std::unique_ptr<CodeShape>> CodeShapeVector;

// Fwd decl
void _PopulateCodeShapes(CodeShapeVector &shapes, codenode_it begin, codenode_it end, DecompileLookups &lookups);

void _ProcessCodeNodesIntoStatements(StatementsNode &statements, codenode_it begin, codenode_it end, DecompileLookups &lookups)
{
    CodeShapeVector shapes;
    _PopulateCodeShapes(shapes, begin, end, lookups);

    for (auto &codeShape : shapes)
    {
        unique_ptr<SingleStatement> pStatement(codeShape->DoIt(lookups));
        statements.AddStatement(std::move(pStatement));
    }
}

class CodeTypeNormal : public CodeShape
{
public:
    CodeTypeNormal(codenode_it code) : _code(code) {}

    CodeTypeNormal(const CodeTypeNormal &src) = delete;
    CodeTypeNormal &operator=(const CodeTypeNormal &src) = delete;

    virtual unique_ptr<SingleStatement> DoIt(DecompileLookups &lookups)
    {
        unique_ptr<SingleStatement> pStatement(new SingleStatement);
        pStatement->SetSyntaxNode(std::move(_CodeNodeToSyntaxNode(*(*_code), lookups)));
        return pStatement;
    }
    virtual void Print(std::ostream &os, int iIndent) const
    {
        (*_code)->Print(os, iIndent);
    }

private:
    codenode_it _code;
};

class CodeTypeSwitch : public CodeShape
{
public:
    CodeTypeSwitch() : _fSwitch(false) { }
    CodeTypeSwitch(const CodeTypeSwitch &src) = delete;
    CodeTypeSwitch &operator=(const CodeTypeSwitch &src) = delete;

    void SetSwitch(codenode_it switchPivot) { _switch = switchPivot; _fSwitch = true; }
    void AddValue(bool isDefault, codenode_it switchValue, codenode_it begin, codenode_it end)
    {
        switchvalue_t value = { isDefault, switchValue, begin, end };
        _values.push_back(value);
    }

    virtual unique_ptr<SingleStatement> DoIt(DecompileLookups &lookups)
    {
        unique_ptr<SingleStatement> pStatement = std::make_unique<SingleStatement>();
        if (_fSwitch)
        {
            unique_ptr<SwitchStatement> pSwitch = std::make_unique<SwitchStatement>();

            // The switch pivot:
            _ApplySyntaxNodeToCodeNode1(*(*_switch), *pSwitch, lookups);

            // The cases:
            for (switchvalue_t &section : _values)
            {
                unique_ptr<CaseStatement> pCase(new CaseStatement);
                pCase->SetDefault(section.isDefault);
                if (!section.isDefault)
                {
                    // Case value:
                    CodeNode *pNodeValue = (*section.value).get();
                    CodeNode *pNodeSomething = _IsolateCaseValue(*pNodeValue);
                    _ApplySyntaxNodeToCodeNode1(*pNodeSomething, *pCase, lookups);
                }

                // The code in the case:
                _ProcessCodeNodesIntoStatements(*pCase, section.begin, section.end, lookups);

                pSwitch->AddCase(std::move(pCase));
            }

            pStatement->SetSyntaxNode(std::move(pSwitch));
        }
        else
        {
            unique_ptr<Comment> pSwitch = std::make_unique<Comment>();
            pSwitch->SetName("ERROR");
            pStatement->SetSyntaxNode(std::move(pSwitch));
        }
        return pStatement;
    }
    virtual void Print(std::ostream &os, int iIndent) const
    {
        os << _indent(iIndent) << "[switch]\n";
        if (_fSwitch)
        {
            (*_switch)->Print(os, iIndent + 2);
        }
        else
        {
            os << "ERROR\n";
        }
        for (size_t i = 0; i < _values.size(); ++i)
        {
            if (_values[i].isDefault)
            {
                os << _indent(iIndent + 2) << "[default]\n";
                codenode_it it = _values[i].begin;
                while (it != _values[i].end)
                {
                    (*it)->Print(os, iIndent + 4);
                    ++it;
                }
            }
            else
            {
                os << _indent(iIndent + 2) << "[case]\n";
                (*_values[i].value)->Print(os, iIndent + 4);
                codenode_it it = _values[i].begin;
                while (it != _values[i].end)
                {
                    (*it)->Print(os, iIndent + 4);
                    ++it;
                }
            }
        }
    }

private:
    CodeNode *_IsolateCaseValue(CodeNode &node)
    {
        // This should consist of:
        // bnt
        //   eq?
        //     dup
        //     *something*
        // We're really only interested in the *someting*
        CodeNode *pNodeRet = &node;
        if ((node.GetCode()->get_opcode() == Opcode::BNT) && (node.GetChildCount() >= 1))
        {
            CodeNode *pNodeEq = node.Child(0);
            if ((pNodeEq->GetCode()->get_opcode() == Opcode::EQ) && (pNodeEq->GetChildCount() >= 2))
            {
                if (pNodeEq->Child(0)->GetCode()->get_opcode() == Opcode::DUP)
                {
                    pNodeRet = pNodeEq->Child(1);
                }
            }
        }
        return pNodeRet;
    }

    bool _fSwitch;
    codenode_it _switch;
    struct switchvalue_t
    {
        bool isDefault;		// If true, pivot is ignored
        codenode_it value;  // The pivot 
        codenode_it begin;  // Beginning of code for this value
        codenode_it end;    // End of code for this value
    };
    std::vector<switchvalue_t> _values;
};

//
// Handle if statements, as well as while loops
//
class CodeTypeIfElse : public CodeShape
{
public:
    CodeTypeIfElse() : _fElse(false), _fWhile(false) {}
    CodeTypeIfElse(const CodeTypeIfElse &src) = delete;
    CodeTypeIfElse &operator=(const CodeTypeIfElse &src) = delete;

    void AddCondition(codenode_it condition)
    {
        _conditions.push_back(condition);
    }

    void SetIf(codenode_it ifBegin, codenode_it ifEnd)
    {
        _ifBegin = ifBegin;
        _ifEnd = ifEnd;
    }

    void SetWhile(bool fWhile, codenode_it breakExit)
    {
        _fWhile = fWhile;
        _breakExit = breakExit;
    }

    void SetAndsOrs(vector<bool> andsAndOrs)
    {
        _andsAndOrs = andsAndOrs;
    }

    void SetElse(codenode_it elseBegin, codenode_it elseEnd)
    {
        _fElse = true;
        _elseBegin = elseBegin;
        _elseEnd = elseEnd;
    }

    virtual unique_ptr<SingleStatement> DoIt(DecompileLookups &lookups)
    {
        unique_ptr<SingleStatement> pStatement = std::make_unique<SingleStatement>();
        if (_fWhile)
        {
            ASSERT(!_fElse);
            unique_ptr<WhileLoop> pWhile = std::make_unique<WhileLoop>();
            _ApplyConditions(*pWhile, lookups);
            BreakExit breakExit(lookups, _breakExit);
            _ProcessCodeNodesIntoStatements(*pWhile, _ifBegin, _ifEnd, lookups);
            pStatement->SetSyntaxNode(std::move(pWhile));
        }
        else
        {
            unique_ptr<CppIfStatement> pIf = std::make_unique<CppIfStatement>();

            // The conditions - they are all OR'd together
            _ApplyConditions(*pIf, lookups);

            _ApplyMainCodeBlock(*pIf, lookups);

            // The else
            if (_fElse)
            {
                unique_ptr<CodeBlock> pElseBlock = std::make_unique<CodeBlock>();
                _ProcessCodeNodesIntoStatements(*pElseBlock, _elseBegin, _elseEnd, lookups);
                unique_ptr<SingleStatement> temp = make_unique<SingleStatement>();
                temp->SetSyntaxNode(move(pElseBlock));
                pIf->SetStatement2(move(temp));
            }

            pStatement->SetSyntaxNode(std::move(pIf));
        }
        return pStatement;
    }

    virtual void Print(std::ostream &os, int iIndent) const
    {
        os << _indent(iIndent) << (_fWhile ? "[while]\n" : "[if]\n");
        for (size_t i = 0; i < _conditions.size(); ++i)
        {
            (*_conditions[i])->Print(os, iIndent + 2);
        }
        os << _indent(iIndent) << "[then]\n";
        codenode_it it = _ifBegin;
        while (it != _ifEnd)
        {
            (*it)->Print(os, iIndent + 2);
            ++it;
        }

        if (_fElse)
        {
            os << _indent(iIndent) << "[else]\n";
            it = _elseBegin;
            while (it != _elseEnd)
            {
                (*it)->Print(os, iIndent + 2);
                ++it;
            }
        }
    }

    // Used to identify a while loop - we try to get the codenode_it that
    // represents the first condition in a loop
    codenode_it GetBeginningOfConditions(codenode_it currentNotYetAdded)
    {
        if (_conditions.empty())
        {
            return currentNotYetAdded;
        }
        else
        {
            return _conditions[0];
        }
    }

private:
    void _ApplyMainCodeBlock(OneStatementNode &statementNode, DecompileLookups &lookups)
    {
        // The if
        unique_ptr<CodeBlock> pIfBlock = std::make_unique<CodeBlock>();
        _ProcessCodeNodesIntoStatements(*pIfBlock, _ifBegin, _ifEnd, lookups);
        unique_ptr<SingleStatement> temp = make_unique<SingleStatement>();
        temp->SetSyntaxNode(move(pIfBlock));
        statementNode.SetStatement1(move(temp));
    }

    void _ApplyConditions(ConditionNode &conditionNode, DecompileLookups &lookups)
    {
        // The conditions - they are all OR'd together
        unique_ptr<ConditionalExpression> pCondition = std::make_unique<ConditionalExpression>();

        SingleStatementVector statements;
        for (size_t i = 0; i < _conditions.size(); ++i)
        {
            unique_ptr<SingleStatement> pStatement = std::make_unique<SingleStatement>();
            pStatement->SetSyntaxNode(std::move(_CodeNodeToSyntaxNode(*(*_conditions[i]), lookups)));
            statements.push_back(std::move(pStatement));
        }
        ConvertAndOrSequenceIntoTree(statements, _andsAndOrs);
        assert(statements.size() == 1);
        pCondition->AddStatement(move(statements[0]));
        conditionNode.SetCondition(move(pCondition));
    }

    std::vector<codenode_it> _conditions;
    vector<bool> _andsAndOrs;
    codenode_it _ifBegin;
    codenode_it _ifEnd;
    bool _fElse;
    codenode_it _elseBegin;
    codenode_it _elseEnd;
    bool _fWhile;
    codenode_it _breakExit;
};

// Given a CodeShape vector, looks at the CodeNode iterators and turns them into CodeShapes...
void _PopulateCodeShapes(CodeShapeVector &shapes, codenode_it begin, codenode_it end, DecompileLookups &lookups)
{
    codenode_it it = begin;

    // Keep an if/else code shape around, as these could be complex - we can't complete them in one go.
    unique_ptr<CodeTypeIfElse> pShapeIfElse = std::make_unique<CodeTypeIfElse>();

    std::vector<bool> andsAndOrs;
    code_pos btTarget;

    while (it != end)
    {
        auto &pNode = *it;

        // Or statements are generally represented by
        // (a or b or c or d)
        //  bt->cpde
        //  bt->code
        //  bt->code
        //  bnt->exit
        //  "code"
        //  "exit"
        // i.e. this would be 4 ors.
        // So if we encounter a BT, let's keep iterating forward until we encounter a BNT.
        // And we'll ensure that each BT points to a spot right after the BNT
        if (pNode->GetCode()->get_opcode() == Opcode::BT)
        {
            if (!andsAndOrs.empty())
            {
                // Ensure we're all branching to the same thing.
                ASSERT(btTarget == pNode->GetCode()->get_branch_target());
            }
            btTarget = pNode->GetCode()->get_branch_target();
            andsAndOrs.push_back(false);
            // (We're still gonna have a problem if we have a while loop with ands and ors in it)
            // TODO: Ensure this is the same Opcode::BNT as what we choose in our if statement...
            pShapeIfElse->AddCondition(it);
            ++it;
        }
        else if (pNode->GetCode()->get_opcode() == Opcode::BNT)
        {
            // Start of a normal if statement (or possibly while loop)
            bool fElse = false;
            bool fWhileLoop = false;
            code_pos posTarget = pNode->GetCode()->get_branch_target();
            // Try to match the target up to the beginning of another codenode.
            codenode_it itIf = it;
            codenode_it itCondition = it;
            ++itIf;
            codenode_it itIfBegin = itIf;
            codenode_it itIfEnd = end;  // Just in case we never hit the target (it's outside of our scope)
            codenode_it itElseBegin = itIf;
            codenode_it itElseEnd = end;// Just in case we never hit the target (it's outside of our scope)
            codenode_it itWhereWeBranchTo = itIf;
            while (itWhereWeBranchTo != end)
            {
                // find the place where we jump to, and treat the rest like an else.
                // Well it's possible the place we jump to is outside of us (in which case the whole thing is an if (or while)), and no else.
                // So in that case, itWhereWeBranchTo will be equal to end
                if (_ScanCodeNodeForCodePos((*itWhereWeBranchTo).get(), posTarget))
                {
                    break;
                }
                ++itWhereWeBranchTo;
            }

            // Found where this jumps to.  The end of the +ve if clause.
            // Now back up and see if this is an else...
            itIfEnd = itWhereWeBranchTo;
            codenode_it itMaybeJump = itIfEnd;
            --itMaybeJump;
            if ((*itMaybeJump)->GetCode()->get_opcode() == Opcode::JMP)
            {
                // REVIEW -> I just deleted this.  it makes while loops inside if statements become if staetments, because we remove the
                // jmp.... it's not even the if statement's jmp. I guess we could try to see if it's a fwd or backward jmp and remove it selectively?
                // --itIfEnd; // If this was a jump, don't include it in the +ve branch



                code_pos posTarget = (*itMaybeJump)->GetCode()->get_branch_target();
                // Now find out where it jumped to.

                // First check if this is a while loop, by seeing if it jumps to the
                // condition of our "if statement"
                // NOTE: when we have an && in our while statement, we'll end up with an if and a while... oh oh
                // then our while detection will be broken - PHIL
                if (_ScanCodeNodeForCodePos((*pShapeIfElse->GetBeginningOfConditions(itCondition)).get(), posTarget))
                {
                    fWhileLoop = true;
                }
                else
                {
                    if (lookups.BreakExit._Ptr && (posTarget == (*lookups.BreakExit)->GetCode()))
                    {
                        // This jump points to the break exit. I guess we don't care right at this point.
                    }

                    codenode_it itElse = itWhereWeBranchTo;
                    itElseBegin = itElse;
                    while (itElse != end)
                    {
                        if (_ScanCodeNodeForCodePos((*itElse).get(), posTarget))
                        {
                            break;
                        }
                        ++itElse;
                    }
                    // NOTE: If we didn't find the target of the Opcode::JMP in our thing, then
                    // we're going to still assume it was an else, and the entire rest of our thing
                    // is the else clause.  When we have nested if statements, the Opcode::JMPs at the end of
                    // the if clauses don't jump right after the else clauses... they'll jump outside
                    // our begin/end.
                    // NOTE: We may need to revisit this logic when dealing with loops - and check to make
                    // sure the jmp wasn't to somewhere before us.
                    // This was the target branch.
                    fElse = true;
                    itElseEnd = itElse;
                }
            }

            pShapeIfElse->AddCondition(itCondition);
            pShapeIfElse->SetIf(itIfBegin, itIfEnd);
            pShapeIfElse->SetWhile(fWhileLoop, itIfEnd);
            pShapeIfElse->SetAndsOrs(andsAndOrs);
            andsAndOrs.clear();
            it = itIfEnd;
            if (fElse)
            {
                // Give it an else too.
                pShapeIfElse->SetElse(itElseBegin, itElseEnd);
                it = itElseEnd; // This needs to point here...
            }
            shapes.push_back(std::move(pShapeIfElse));
            pShapeIfElse = std::make_unique<CodeTypeIfElse>(); // Put a new blank one in...
        }
        else if (pNode->GetCode()->get_opcode() == Opcode::TOSS)
        {
            assert(andsAndOrs.empty());

            unique_ptr<CodeTypeSwitch> pSwitch = std::make_unique<CodeTypeSwitch>();
            // This generally indicates a switch statement.  Furthermore, all the children are within the switch
            // statement, unlike ifs for example.  So we don't need to explore outside this codenode's children.
            // There should be a whole bunch of children.  The first one will be a push.
            codenode_it itChild = pNode->begin();
            if (itChild != pNode->end())
            {
                Consumption cons = _GetInstructionConsumption(*(*itChild)->GetCode());
                if (cons.cStackGenerate) // Could be Opcode::PUSH, or something like Opcode::PTOS
                {
                    // This is our switch pivot then.
                    pSwitch->SetSwitch(itChild);
                }
                else
                {
                    pSwitch->SetSwitch(itChild); // REVIEW, oh well...
                    assert(false);
                }
                ++itChild;
            }

            // Now iterate through the cases.
            while (itChild != pNode->end())
            {
                // Now we expect a series of bnt instructions finally ending in our toss.
                Opcode bOpcode = (*itChild)->GetCode()->get_opcode();
                if (bOpcode == Opcode::BNT)
                {
                    codenode_it switchValue = itChild; // 1
                    code_pos posTarget = (*itChild)->GetCode()->get_branch_target();
                    ++itChild;
                    codenode_it caseBegin = itChild; // 2
                    // Scan for this target... it's either going to be the next bnt, or the toss. The toss is above us though. So the toss will result in "end"
                    codenode_it caseEnd = _FindCodeNodeWithCodePos(itChild, pNode->end(), posTarget);
                    bool isLastCase = (caseEnd == pNode->end());

                    pSwitch->AddValue(false, switchValue, caseBegin, caseEnd);
                    itChild = caseEnd;

                    if (isLastCase)
                    {
                        assert(posTarget->get_opcode() == Opcode::TOSS);
                    }
                }
                else
                {
                    // Presumably this is the default case.
                    codenode_it caseBegin = itChild; 
                    codenode_it caseEnd = pNode->end();
                    pSwitch->AddValue(true, codenode_it(), caseBegin, caseEnd);
                    itChild = caseEnd;
                }
            }

            shapes.push_back(std::move(pSwitch));
            ++it;
        }
        else
        {
            // TODO: ASSERT that we aren't in the process of doing if statements
            assert(andsAndOrs.empty());

            // Normal for now...
            shapes.push_back(std::move(std::make_unique<CodeTypeNormal>(it)));
            ++it;
        }
    }

}

bool _FindNodeAndApplyLabel(char &label, code_pos target, codenode_it begin, codenode_it end)
{
    bool fRet = false;
    while (!fRet && (begin != end))
    {
        fRet = ((*begin)->GetCode() == target);
        if (fRet)
        {
            (*begin)->SetLabel(label);
            ++label;
        }
        else
        {
            fRet = _FindNodeAndApplyLabel(label, target, (*begin)->begin(), (*begin)->end());
        }
        ++begin;
    }
    return fRet;
}

// For debugging
void _ApplyBranchLabelsWorker(char &label, codenode_it begin, codenode_it end, codenode_it beginMaster, codenode_it endMaster)
{
    while (begin != end)
    {
        if (IsBranchInstruction((*begin)->GetCode()))
        {
            (*begin)->SetTargetLabel(label);
            // Find its target.
            if (!_FindNodeAndApplyLabel(label, (*begin)->GetCode()->get_branch_target(), beginMaster, endMaster))
            {
                (*begin)->SetTargetLabel('?');
            }
        }
        // Go to the children:
        _ApplyBranchLabelsWorker(label, (*begin)->begin(), (*begin)->end(), beginMaster, endMaster);
        ++begin;
    }
}
void _ApplyBranchLabels(char &label, codenode_it begin, codenode_it end)
{
    _ApplyBranchLabelsWorker(label, begin, end, begin, end);
}

CFGNode *GetFirstSuccessorOrNull(CFGNode *node)
{
    assert(node->Successors().size() <= 1);
    return node->Successors().empty() ? nullptr : *node->Successors().begin();
}

class FillInFunction : public ICFGNodeVisitor
{
public:
    FillInFunction(FunctionBase &func, const map<const RawCodeNode*, CodeNode*> &graphNodeToCodeNode, DecompileLookups &lookups) :
        _func(func), _graphNodeToCodeNode(graphNodeToCodeNode), _lookups(lookups)
    {
        _statementsContext.push(&func);
    }

    unique_ptr<SingleStatement> StatementFromCodeNode(CodeNode *node)
    {
        // REVIEW: _CodeNodeToSyntaxNode is not sufficient right now really. It handles some branching.
        // I doubt it properly handles break statements.
        unique_ptr<SingleStatement> pStatement = make_unique<SingleStatement>();
        pStatement->SetSyntaxNode(move(_CodeNodeToSyntaxNode(*node, _lookups)));
        return pStatement;
    }

    void AddStatementFromCodeNode(CodeNode *child)
    {
        _statementsContext.top()->AddStatement(move(StatementFromCodeNode(child)));
    }

    // The top level code node doesn't represent any code. Just a convenient container.
    void AddStatementsFromTopLevelCodeNode(CodeNode *codeNode)
    {
        for (size_t i = 0; i < codeNode->GetChildCount(); i++)
        {
            CodeNode *child = codeNode->Child(i);
            AddStatementFromCodeNode(child);
        }
    }

    void AddStatement(unique_ptr<SyntaxNode> syntaxNode)
    {
        unique_ptr<SingleStatement> pStatement(new SingleStatement);
        pStatement->SetSyntaxNode(move(syntaxNode));
        _statementsContext.top()->AddStatement(move(pStatement));
    }

    void FollowChainOfCodeNodesAddToContext(CFGNode *node, CFGNode *toInclusive = nullptr)
    {
        while (node)
        {
            node->Accept(*this);
            if (node->ContainsTag(SemanticTags::LoopBreak))
            {
                // We've already identified break statements during control flow, so we can
                // be confident this is one without looking at its destination.
                AddStatement(move(make_unique<BreakStatement>()));
            }
            if (node == toInclusive)
            {
                break;
            }
            node = GetFirstSuccessorOrNull(node);
        }
    }

    unique_ptr<SingleStatement> FollowChainOfCodeNodes(CFGNode *node, bool singleNodeOnly)
    {
        // Start with a code block. If there is only a single statement by the end, then just
        // return that statement
        unique_ptr<CodeBlock> block = make_unique<CodeBlock>();
        _statementsContext.push(block.get());
        FollowChainOfCodeNodesAddToContext(node, singleNodeOnly ? node : nullptr);
        _statementsContext.pop();
        if (block->GetStatements().size() == 1)
        {
            return move(block->GetStatements()[0]);
        }
        else if (block->GetStatements().empty())
        {
            return nullptr;
        }

        unique_ptr<SingleStatement> single = make_unique<SingleStatement>();
        single->SetSyntaxNode(move(block));
        return single;
    }

    void Visit(const RawCodeNode &rawCodeNode)
    {
        CodeNode *codeNode = _graphNodeToCodeNode.at(&rawCodeNode);
        AddStatementsFromTopLevelCodeNode(codeNode);
    }

    void ApplyCondition(ConditionNode &conditionStatement, CFGNode *node)
    {
        unique_ptr<ConditionalExpression> condExp = make_unique<ConditionalExpression>();
        _statementsContext.push(condExp.get());
        node->Accept(*this);
        _statementsContext.pop();
        conditionStatement.SetCondition(move(condExp));
    }

    void Visit(const LoopNode &loopNode)
    {
        // Let's detect the loop type
        CFGNode *latch = loopNode[SemId::Latch];
        CFGNode *head = loopNode[SemId::Head];
        if ((latch->Successors().size() == 2) && (head->Successors().size() == 1))
        {
            // It's a do loop. The condition is the latch.
            unique_ptr<DoLoop> doLoop = make_unique<DoLoop>();
            ApplyCondition(*doLoop, latch);

            // Then the rest of the stuff is statements
            _statementsContext.push(doLoop.get());
            assert(latch->Predecessors().size() == 1);
            CFGNode *end = *latch->Predecessors().begin();
            FollowChainOfCodeNodesAddToContext(head, end);
            _statementsContext.pop();

            AddStatement(move(doLoop));
        }
        else if ((latch->Successors().size() == 1) && (head->Successors().size() == 2))
        {
            // It's a while. The condition is the head.
            unique_ptr<WhileLoop> whileLoop = make_unique<WhileLoop>();
            ApplyCondition(*whileLoop, head);

            CFGNode *thenNode, *elseNode;
            GetThenAndElseBranches(head, &thenNode, &elseNode);
            // Then the rest of the stuff is statements
            _statementsContext.push(whileLoop.get());
            FollowChainOfCodeNodesAddToContext(thenNode, latch);
            _statementsContext.pop();

            AddStatement(move(whileLoop));
        }
    }

    unique_ptr<SingleStatement> ExtractCaseValue(CodeNode *codeNode)
    {
        assert(codeNode->GetChildCount() == 1);
        // This should have a certain structure.
        CodeNode *bntCodeNode = codeNode->Child(0);
        // The bnt should have one child, an eq? operator
        assert(bntCodeNode->GetChildCount() == 1);
        CodeNode *eqCodeNode = bntCodeNode->Child(0);
        // Then, the eq should have two children. The first will be for the DUP, the second
        // will be for the load immediate.
        assert(eqCodeNode->GetChildCount() == 2);
        CodeNode *immediate = eqCodeNode->Child(1);
        unique_ptr<SingleStatement> pStatement = make_unique<SingleStatement>();
        pStatement->SetSyntaxNode(move(_CodeNodeToSyntaxNode(*immediate, _lookups)));
        return pStatement;
    }

    void _ProcessCase(SwitchStatement &switchStatement, CFGNode &caseNode)
    {
        unique_ptr<CaseStatement> caseStatement = make_unique<CaseStatement>();
        _statementsContext.push(caseStatement.get());
        CFGNode *caseHead = caseNode[SemId::Head];
        // The children of the caseNode should form a chain, except the first one is the case value (possibly)
        if (caseHead->ContainsTag(SemanticTags::CaseCondition))
        {
            assert(!caseNode.ContainsTag(SemanticTags::DefaultCase));
            caseStatement->SetDefault(false);
            // For the case value, we should assert that the final instruction is bnt.
            assert(caseHead->Type == CFGNodeType::RawCode);
            if (caseHead->Type == CFGNodeType::RawCode)
            {
                caseStatement->SetStatement1(move(ExtractCaseValue(_graphNodeToCodeNode.at(static_cast<RawCodeNode*>(caseHead)))));
            }
            FollowChainOfCodeNodesAddToContext(GetFirstSuccessorOrNull(caseHead));
        }
        else if (caseNode.ContainsTag(SemanticTags::DefaultCase))
        {
            // REVIEW: Do we need to manually put this at the end?
            caseStatement->SetDefault(true);
            FollowChainOfCodeNodesAddToContext(caseHead);
        }
        else
        {
            assert(false);
        }
        _statementsContext.pop();
        switchStatement.AddCase(move(caseStatement));
    }

    void Visit(const SwitchNode &switchNode)
    {
        // We need to be aware that there is some code in the leadup and tail
        // that isn't specifically related to the switch.
        // So we want to output all but the last codenode
        CFGNode *switchHead = switchNode[SemId::Head];
        CFGNode *switchTail = switchNode[SemId::Tail];
        // Currently we're making assumptions here. Theoretically you could imagine a conditional expression
        // in a switch header, but SCISTudio-compiled code will never have that (not allowed)
        assert((switchHead->Type == CFGNodeType::RawCode) && (switchTail->Type == CFGNodeType::RawCode));
        if ((switchHead->Type == CFGNodeType::RawCode) && (switchTail->Type == CFGNodeType::RawCode))
        {
            CodeNode *headCodeNode = _graphNodeToCodeNode.at(static_cast<RawCodeNode*>(switchHead));
            for (size_t i = 0; i < headCodeNode->GetChildCount() - 1; i++)
            {
                AddStatementFromCodeNode(headCodeNode->Child(i));
            }
            CodeNode *switchValue = headCodeNode->Child(headCodeNode->GetChildCount() - 1);
            unique_ptr<SwitchStatement> switchStatement = make_unique<SwitchStatement>();
            switchStatement->SetStatement1(move(StatementFromCodeNode(switchValue)));

            // Now the cases, which are the immediate successors of the head
            vector<CFGNode*> sortedCases;
            copy(switchHead->Successors().begin(), switchHead->Successors().end(), back_inserter(sortedCases));
            sort(sortedCases.begin(), sortedCases.end(), CompareCFGNodesByAddress());
            for (CFGNode *caseNode : sortedCases)
            {
                _ProcessCase(*switchStatement, *caseNode);
            }

            AddStatement(move(switchStatement));

            CodeNode *tailCodeNode = _graphNodeToCodeNode.at(static_cast<RawCodeNode*>(switchTail));
            // We want to skip the toss. Assert that the first one is the toss though, before we skip it
            assert(tailCodeNode->Child(0)->GetCode()->get_opcode() == Opcode::TOSS);
            for (size_t i = 1; i < tailCodeNode->GetChildCount(); i++)
            {
                AddStatementFromCodeNode(tailCodeNode->Child(i));
            }
        }

            
    }

    void Visit(const CaseNode &caseNode)
    {
        assert(false); // Handled in switch
    }

    void Visit(const CompoundConditionNode &conditionNode)
    {
        ConditionType type = conditionNode.condition;
        bool isFirstTermNegated = conditionNode.isFirstTermNegated;
        
        // We'll create a binary operation
        unique_ptr<BinaryOp> binaryOp = make_unique<BinaryOp>();
        binaryOp->SetName((type == ConditionType::And) ? "&&" : "||");

        unique_ptr<SingleStatement> x = FollowChainOfCodeNodes(conditionNode[SemId::First], true);
        unique_ptr<SingleStatement> y = FollowChainOfCodeNodes(conditionNode[SemId::Second], true);
        if (isFirstTermNegated)
        {
            unique_ptr<SingleStatement> negated = make_unique<SingleStatement>();
            unique_ptr<UnaryOp> unary = make_unique<UnaryOp>();
            unary->SetName("!");
            unary->SetStatement1(move(x));
            negated->SetSyntaxNode(move(unary));
            binaryOp->SetStatement1(move(negated));
        }
        else
        {
            binaryOp->SetStatement1(move(x));
        }

        binaryOp->SetStatement2(move(y));
        AddStatement(move(binaryOp));
    }

    void Visit(const InvertNode &invert)
    {
        // Currently this is just used for do loops
        invert[SemId::Head]->Accept(*this);
    }

    void Visit(const IfNode &ifNode)
    {
        // Make an if statement
        unique_ptr<CppIfStatement> ifStatement = make_unique<CppIfStatement>();
        CFGNode *ifHead = ifNode[SemId::Head];
        CFGNode *thenNode, *elseNode;
        GetThenAndElseBranches(ifHead, &thenNode, &elseNode);

        // First the condition
        ApplyCondition(*ifStatement, ifHead);

        // Now the then
        assert(thenNode);
        ifStatement->SetStatement1(move(FollowChainOfCodeNodes(thenNode, false)));

        if (elseNode)
        {
            ifStatement->SetStatement2(move(FollowChainOfCodeNodes(elseNode, false)));
        }

        // Finish off
        AddStatement(move(ifStatement));
    }

    void Visit(const MainNode &ifNode)
    {
        // Just a sequence of nodes from end to end. Should be a straight line.
        CFGNode *currentNode = ifNode[SemId::Head];
        while (currentNode)
        {
            currentNode->Accept(*this);
            currentNode = GetFirstSuccessorOrNull(currentNode);
        }
    }

    void Visit(const CFGNode &node) {}

private:
    const map<const RawCodeNode*, CodeNode*> &_graphNodeToCodeNode;
    FunctionBase &_func;
    DecompileLookups &_lookups;

    stack<StatementsNode*> _statementsContext;
};


// pEnd can be teh end of script data. I have added autodetection support.
void DecompileRaw(FunctionBase &func, DecompileLookups &lookups, const BYTE *pBegin, const BYTE *pEnd, WORD wBaseOffset)
{
    lookups.EndowWithFunction(&func);

    // Take the raw data, and turn it into a list of scii instructions, and make sure the branch targets point to code_pos's
    std::list<scii> code;
    _ConvertToInstructions(code, pBegin, pEnd, wBaseOffset);

    // Insert a no-op at the beginning of code (so we can get an iterator to point to a spot before code)
    code.insert(code.begin(), scii(Opcode::INDETERMINATE));

    ControlFlowGraph cfg;
    cfg.Generate(func.GetName(), code.begin(), code.end());

    const NodeSet &controlStructures = cfg.ControlStructures();
    MainNode *mainNode = cfg.GetMain();

    // Create a CodeNode for each RawCodeNode structure
    vector<unique_ptr<CodeNode>> codeNodes;
    map<const RawCodeNode*, CodeNode*> graphNodeToCodeNode;
    for (CFGNode *structure : controlStructures)
    {
        for (CFGNode *child : structure->children)
        {
            if (child->Type == CFGNodeType::RawCode)
            {
                RawCodeNode *rawCodeNode = static_cast<RawCodeNode*>(child);
                code_pos end = rawCodeNode->end;
                end--;
                code_pos start = rawCodeNode->start;
                if (start->get_opcode() != Opcode::INDETERMINATE)
                {
                    start--;    // That's a bit hacky.
                }
                codeNodes.push_back(move(make_unique<CodeNode>(end)));
                //codeNodes.emplace_back(end); // why doesn't this work?
                graphNodeToCodeNode[static_cast<RawCodeNode*>(child)] = codeNodes.back().get();

                while (end != start)
                {
                    unique_ptr<CodeNode> codeNode = make_unique<CodeNode>(end);
                    _GenerateInstructionTree(*codeNode, start, end);
                    codeNodes.back()->AddChildToFront(move(codeNode));
                }
            }
        }
    }

    // Construct the function -> for now use procedure, but really should be method or proc
    unique_ptr<FunctionSignature> pSignature = std::make_unique<FunctionSignature>();
    _FigureOutParameters(func, *pSignature, code);
    func.AddSignature(std::move(pSignature));

    // Now visit the main structure. Is there a way that this needs to be done?
    // I don't think we can use the visit thing.

    /*
    // Make a function and add statements!
    for (auto &codeShape : codeShapes)
    {
        unique_ptr<SingleStatement> pStatement(codeShape->DoIt(lookups));
        func.AddStatement(std::move(pStatement));
    }*/
    FillInFunction fillInFunction(func, graphNodeToCodeNode, lookups);
    fillInFunction.Visit(*mainNode);

    _FigureOutTempVariables(lookups, func, VarScope::Temp, code);

    lookups.ResolveRestStatements();

    lookups.EndowWithFunction(nullptr);

    func.PruneExtraneousReturn();
}

void DecompileRawOLD(FunctionBase &func, DecompileLookups &lookups, const BYTE *pBegin, const BYTE *pEnd, WORD wBaseOffset)
{
    lookups.EndowWithFunction(&func);

    // Take the raw data, and turn it into a list of scii instructions, and make sure the branch targets point to code_pos's
    std::list<scii> code;
    _ConvertToInstructions(code, pBegin, pEnd, wBaseOffset);

    // Now make them hierarchical.
    std::vector<std::unique_ptr<CodeNode>> codeNodes;

    // Insert a no-op at the beginning of code (so we can get an iterator to point to a spot before code)
    code.insert(code.begin(), scii(Opcode::INDETERMINATE));

    code_pos pos = code.end();
    --pos; // Go to last instruction
    code_pos start = code.begin();

    while (pos != start)
    {
        unique_ptr<CodeNode> pRoot = std::make_unique<CodeNode>(pos);
        _GenerateInstructionTree(*pRoot, start, pos);
        // Insert at beginning, since we're going backwards:
        codeNodes.insert(codeNodes.begin(), std::move(pRoot));
    }

    char label = 'A';
    _ApplyBranchLabels(label, codeNodes.begin(), codeNodes.end());

    // Now we have a hierarchical thing.  Separate into branch blocks.  We may need to do this over and over.
    CodeShapeVector codeShapes;

    _PopulateCodeShapes(codeShapes, codeNodes.begin(), codeNodes.end(), lookups);

    // Debug out
    unique_ptr<Comment> pComment = std::make_unique<Comment>();
    std::stringstream ss;

#ifdef DEBUG_DECOMPILER

    ss << "/*\n";
    ss << "Raw code nodes:\n";
    for (auto &codeNode : codeNodes)
    {
        codeNode->Print(ss, 2);
    }

    ss << "*/\n\nCode shapes:\n/*\n";
    for (auto &codeShape : codeShapes)
    {
        codeShape->Print(ss, 2);
        ss << "------------\n";
    }
    ss << "*/\n";
#endif

    pComment->SetName(ss.str());
    lookups.SetPosition(pComment.get());
    func.GetOwnerScript()->AddComment(std::move(pComment));
    lookups.SetPosition(&func); // Make the function after, so the comment is spit out.


    // Construct the function -> for now use procedure, but really should be method or proc
    unique_ptr<FunctionSignature> pSignature = std::make_unique<FunctionSignature>();
    _FigureOutParameters(func, *pSignature, code);
    func.AddSignature(std::move(pSignature));
    // Make a function and add statements!
    for (auto &codeShape : codeShapes)
    {
        unique_ptr<SingleStatement> pStatement(codeShape->DoIt(lookups));
        func.AddStatement(std::move(pStatement));
    }
    _FigureOutTempVariables(lookups, func, VarScope::Temp, code);

    lookups.ResolveRestStatements();

    lookups.EndowWithFunction(nullptr);

    func.PruneExtraneousReturn();
}

bool _IsPrintProcedure(WORD wScript, WORD wIndex)
{
    if (wScript == 255 && wIndex == 0)
    {
        return true; // True for space quest
    }
    return false;
}

bool IsStatementImmediateValue(const SingleStatement &statement, WORD &wValue)
{
    const PropertyValue *pValue = statement.CastSyntaxNode<PropertyValue>();
    bool fRet = (pValue && (pValue->GetType() == ValueType::Number));
    if (fRet)
    {
        wValue = pValue->GetNumberValue();
    }
    return fRet;
}

bool _MaybeAProperty(const std::string &selector)
{
    static bool g_fInited = false;
    static std::set<std::string> g_PropertyMap;
    static const std::string c_rgTendsToBeProperty[] =
    {
        "x",
        "y",
        "z",
        "view",
        "cel",
        "loop",
        "script",
        "number",
        "timer",
        "keep",
        "initialized",
        "picture",
        "style",
        "horizon",
        "controls",
        "north",
        "east",
        "west",
        "south",
        "curPic",
        "picAngle",
        "vanishingX",
        "vanishingY",
        "priority",
        "underbits",
        "heading",
        "yStep",
        "xStep",
        "signal",
        "nsTop",
        "nsBottom",
        "nsLeft",
        "nsRight",
        "lsTop",
        "lsBottom",
        "lsLeft",
        "lsRight",
        "brTop",
        "brBottom",
        "brLeft",
        "brRight",
        "cycleSpeed",
        "cycler",
        "illegalBits",
        "xLast",
        "yLast",
        "moveSpeed",
        "blocks",
        "baseSetter",
        "mover",
        "looper",
        "viewer",
        "avoider",
        "edgeHit",
        "elements",
        "size",
        "text",
        "window",
        "theItem",
        "time",
        "busy",
        "seconds",
        "lastSeconds",
        "cycles",
        "higher",
        "lower",
        "normal",
        "minimum",
        "maximum",
        "description",
        "owner",
        "said",
        "curEvent",
        "mapKeyToDir",
        "inputLineAddr",
        "prompt",
        "prevDir",
        "echo",
        "canInput",
        "alterEgo",
        "species",
    };

    if (!g_fInited)
    {
        g_fInited = true;
        g_PropertyMap.insert(&c_rgTendsToBeProperty[0], &c_rgTendsToBeProperty[ARRAYSIZE(c_rgTendsToBeProperty) - 1]);
    }
    return (g_PropertyMap.find(selector) != g_PropertyMap.end());
}

//
// Try to massage the parameters so that it makes more sense to the user.
//
void _MassageProcedureCall(ProcedureCall &proc, DecompileLookups &lookups, code_pos pos)
{
    if (pos->get_opcode() == Opcode::CALLE)
    {
        if (_IsPrintProcedure(pos->get_first_operand(), pos->get_second_operand()))
        {
            proc.SetName("Print");

            const SingleStatement *pOne = proc.GetParameter(0);
            const SingleStatement *pTwo = proc.GetParameter(1);
            WORD wOne;
            WORD wTwo;
            if (pOne && IsStatementImmediateValue(*pOne, wOne) &&
                pTwo && IsStatementImmediateValue(*pTwo, wTwo))
            {
                // If the first two parameters are immediates, and the first one matches the current
                // script number, then assume it's a text resource.
                if (wOne == lookups.GetScriptNumber())
                {
                    std::string text = lookups.LookupTextResource(wTwo);
                    if (!text.empty())
                    {
                        unique_ptr<SingleStatement> pNewStatement = std::make_unique<SingleStatement>();
                        unique_ptr<PropertyValue> pNewValue = std::make_unique<PropertyValue>();
                        pNewValue->SetValue(text, ValueType::String);
                        pNewStatement->SetSyntaxNode(std::move(pNewValue));
                        SingleStatementVector parameters;
                        parameters.push_back(std::move(pNewStatement));
                        proc.StealParams(parameters); // Do the swap
                    }
                }
            }
        }
    }
    else if (pos->get_opcode() == Opcode::CALLK)
    {
        if (proc.GetName() == "Load")
        {
            SingleStatement *pOne = proc.GetParameter(0);
            if (pOne)
            {
                PropertyValue *pValue = pOne->CastSyntaxNode<PropertyValue>();
                if (pValue && (pValue->GetType() == ValueType::Number))
                {
                    static const std::string rgResourceTypes[] = 
                    {
                        "rsVIEW", "rsPIC", "rsSCRIPT", "rsTEXT", "rsSOUND", "rsMEMORY", "rsVOCAB", "rsFONT", "rsCURSOR", "rsPATCH"
                    };
                    if ((pValue->GetNumberValue() >= 0x0080) && pValue->GetNumberValue() < (0x0080 + ARRAYSIZE(rgResourceTypes)))
                    {
                        pValue->SetValue(rgResourceTypes[pValue->GetNumberValue() - 0x0080], ValueType::Token);
                    }
                }
            }
        }
    }
}

// Returns the token string in a PropertyValue, or an empty string
std::string _PluckTokenFromStatement(SingleStatement &statement)
{
    std::string ret;
    const PropertyValue *pValue = statement.CastSyntaxNode<PropertyValue>();
    if (pValue && (pValue->GetType() == ValueType::Token))
    {
        ret = pValue->GetStringValue();
    }
    return ret;
}

WORD _GetImmediateFromCodeNode(CodeNode &node, CodeNode *pNodePrevious = nullptr, bool assertIfNone = false, bool *foundOut = nullptr)
{
    bool found = true;
    WORD w = 0;
    code_pos pos = node.GetCode();
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
            CodeNode nodeTemp(posFlatPrevious);
            w = _GetImmediateFromCodeNode(nodeTemp);
        }
        break;

    default:
        found = false;
        assert(!assertIfNone);
        break;
    }
    if (foundOut)
    {
        *foundOut = found;
    }
    return w;
}

void _ApplySyntaxNodeToCodeNode(CodeNode &node, StatementsNode &statementsNode, DecompileLookups &lookups)
{
    unique_ptr<SingleStatement> pStatement = std::make_unique<SingleStatement>();
    pStatement->SetSyntaxNode(std::move(_CodeNodeToSyntaxNode(node, lookups)));
    statementsNode.AddStatement(std::move(pStatement));
}

void _ApplySyntaxNodeToCodeNode1(CodeNode &node, OneStatementNode &statementsNode, DecompileLookups &lookups)
{
    unique_ptr<SingleStatement> pStatement = std::make_unique<SingleStatement>();
    pStatement->SetSyntaxNode(std::move(_CodeNodeToSyntaxNode(node, lookups)));
    statementsNode.SetStatement1(move(pStatement));
}

void _ApplySyntaxNodeToCodeNode2(CodeNode &node, TwoStatementNode &statementsNode, DecompileLookups &lookups)
{
    unique_ptr<SingleStatement> pStatement = std::make_unique<SingleStatement>();
    pStatement->SetSyntaxNode(std::move(_CodeNodeToSyntaxNode(node, lookups)));
    statementsNode.SetStatement2(move(pStatement));
}

// Both for variable opcodes, and Opcode::LEA
std::string _GetVariableNameFromCodePos(code_pos pos, DecompileLookups &lookups, VarScope *pVarType = nullptr, WORD *pwIndexOut = nullptr)
{
    std::string name;
    BYTE bThing;
    WORD wIndex;
    if (pos->get_opcode() == Opcode::LEA)
    {
        bThing = (BYTE)pos->get_first_operand();
        bThing >>= 1;
        wIndex = pos->get_second_operand();
    }
    else
    {
        bThing = (BYTE)pos->get_opcode();
        wIndex = pos->get_first_operand();
        assert(bThing >= 64);
    }
    std::stringstream ss;
    switch (bThing & 0x3)
    {
    case VO_GLOBAL:
        {
            // global
            name = lookups.ReverseLookupGlobalVariableName(wIndex);
            if (name.empty())
            {
                ss << "global" << setw(4) << setfill('0') << wIndex;
                name = ss.str();
            }
        }
        break;
    case VO_LOCAL:
        // local
        ss << _GetLocalVariableName(wIndex);
        name = ss.str();
        break;
    case VO_TEMP:
        // temp
        ss << _GetTempVariableName(wIndex);
        name = ss.str();
        break;
    case VO_PARAM:
        // param
        name = lookups.LookupParameterName(wIndex);
        break;
    }

    if (pwIndexOut)
    {
        *pwIndexOut = wIndex;
    }
    if (pVarType)
    {
        *pVarType = static_cast<VarScope>(bThing & 0x3);
    }

    return name;
}

// Looks for a rest instruction at index (and tracks it and adds to the SendParam). Returns true if it found one.
bool _MaybeConsumeRestInstruction(SendParam *pSendParam, int index, CodeNode &node, DecompileLookups &lookups)
{
    if (index >= (int)node.GetChildCount())
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

#if 0
std::unique_ptr<SyntaxNode> _CodeNodeToSyntaxNode2(code_pos &pos, code_pos end, DecompileLookups &lookups, Consumption &consumption)
{
    // We process at least one instruction, and up to what is needed in consumption. We can return null, indicating we didn't produce what was desired.

    bool preferLValue = lookups.PreferLValue;
    PreferLValue resetPreferLValue(lookups, false); // Reset it, because if it's true it's not normal.

    Opcode bOpcode = pos->get_opcode();
    --pos;
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
                    uint16_t theValue = pos->get_first_operand();
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
            if (node.GetChildCount() >= 1)
            {
                if (node.GetChildCount() == 3)
                {
                    // The unusual case... our hack for a ternary operator.
                    unique_ptr<CppIfStatement> pTernary = std::make_unique<CppIfStatement>();
                    pTernary->MakeTernary();
                    CodeNode *pConditionNode = node.Child(0);
                    // REVIEW - laura bow, script 30 has Opcode::CLASS here... ?
                    ASSERT(pConditionNode->GetCode()->get_opcode() == Opcode::BNT);
                    unique_ptr<ConditionalExpression> condition = make_unique<ConditionalExpression>();
                    _ApplySyntaxNodeToCodeNode(*pConditionNode, *condition.get(), lookups);
                    pTernary->SetCondition(move(condition));
                    _ApplySyntaxNodeToCodeNode1(*node.Child(1), *pTernary, lookups);
                    _ApplySyntaxNodeToCodeNode2(*node.Child(2), *pTernary, lookups);
                    return unique_ptr<SyntaxNode>(move(pTernary));
                }
                else
                {
                    // Fwd to the first child (should be what gets in to the accumulator)
                    return _CodeNodeToSyntaxNode(*node.Child(0), lookups);
                }
            }
            break;

        case Opcode::CALLK:   // kernel, # of params
        case Opcode::CALLB:   // mainproc index, # of params
        case Opcode::CALLE:   // script, index, # of params
        case Opcode::CALL:    // offset, # of params
        {
            WORD cParams = (bOpcode == Opcode::CALLE) ? pos->get_third_operand() : pos->get_second_operand();
            cParams /= 2; // bytes -> params
            cParams += 1; // +1 because there is a parameter count first.
            std::stringstream ss;
            switch (bOpcode)
            {
                case Opcode::CALLK:
                    ss << lookups.LookupKernelName(pos->get_first_operand());
                    break;
                case Opcode::CALLB:
                    ss << "proc000_" << pos->get_first_operand();
                    break;
                case Opcode::CALLE:
                    ss << _GetPublicProcedureName(pos->get_first_operand(), pos->get_second_operand());
                    break;
                case Opcode::CALL:
                    ss << _GetProcNameFromScriptOffset(pos->get_final_postop_offset() + pos->get_first_operand());
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

            _MassageProcedureCall(*pProcCall, lookups, pos);
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
                // TODO - actually you can send to any super class... the first operand says which
                sendCall->SetName("super");
            }
            Consumption cons = _GetInstructionConsumption(*pos);
            WORD cStackPushesLeft = cons.cStackConsume;
            WORD cAccLeft = (bOpcode == Opcode::SEND) ? 1 : 0;
            size_t i = 0;
            WORD cParamsLeft = 0;
            bool fLookingForSelector = true;
            unique_ptr<SendParam> sendParam;

            while ((cAccLeft || cStackPushesLeft) && (i < node.GetChildCount()))
            {
                CodeNode *pPreviousChild = i ? node.Child(i - 1) : nullptr;
                if (cAccLeft)
                {
                    Consumption consAcc = _GetInstructionConsumption(*node.Child(i)->GetCode());
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
                    Consumption consAcc = _GetInstructionConsumption(*node.Child(i)->GetCode());
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
                                    assert(false);
                                }
                            }
                            else
                            {
                                sendParam->SetName(lookups.LookupSelectorName(wValue));
                                sendParam->SetIsMethod(!_MaybeAProperty(sendParam->GetName()));
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

            sendCall->SimplifySendObject();
            return unique_ptr<SyntaxNode>(move(sendCall));
        }
        break;

        case Opcode::BNT:
        case Opcode::BT:
            // REVIEW -> ternary operator is a common thing (e.g. space quest script 005)
            // How can we do things that cross branch boundaries?  We could look for this pattern:
            //      eq?
            //      bnt A
            //      *something*
            //      jmp B
            // A:   *something*
            // B:   push
            //

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
                CodeNode *pCodeNodeStack = node.Child(0);
                CodeNode *pCodeNodeAcc = node.Child(1);
                Consumption cons = _GetInstructionConsumption(*pCodeNodeStack->GetCode());
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
                CodeNode *pCodeNodeAcc = node.Child(0);
                Consumption cons = _GetInstructionConsumption(*pCodeNodeAcc->GetCode());
                ASSERT(cons.cAccGenerate);
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
            WORD wType = pos->get_first_operand();
            unique_ptr<PropertyValue> pValue = std::make_unique<PropertyValue>();
            VarScope varScope;
            WORD wVarIndex;
            pValue->SetValue(_GetVariableNameFromCodePos(pos, lookups, &varScope, &wVarIndex), ValueType::Pointer);
            lookups.TrackVariableUsage(varScope, wVarIndex, true);  // Let's consider this indexed.
            assert((wType & LEA_ACC_AS_INDEX_MOD) == 0); // REVIEW TODO
            return unique_ptr<SyntaxNode>(move(pValue));
        }
        break;

        case Opcode::CLASS:
        case Opcode::PUSHSELF:
        {
            std::string className = (bOpcode == Opcode::CLASS) ? lookups.LookupClassName(pos->get_first_operand()) : SelfToken;
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
            SCIVersion version = appState->GetVersion();
            uint16_t wName = version.lofsaOpcodeIsAbsolute ?
                pos->get_first_operand() :
                (pos->get_first_operand() + pos->get_final_postop_offset());
            std::string name = lookups.LookupScriptThing(wName, type);
            if (name.empty())
            {
                name = InvalidLookupError;
                appState->LogInfo("Unable to find symbol for %d.", wName);
            }
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
            }
            break;

        case Opcode::JMP:
            // Already handled by branching code
            // OH BUT IS IT
            if (lookups.BreakExit._Ptr)
            {
                if (pos->get_branch_target() == (*lookups.BreakExit)->GetCode())
                {
                    // This jump points to the break exit.
                    return unique_ptr<SyntaxNode>(new BreakStatement());
                }
            }

            return unique_ptr<SyntaxNode>(new Comment());
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
                    lValue->SetName(_GetVariableNameFromCodePos(pos, lookups, &varScope, &wVarIndex));
                    bool isIndexed = _IsVOIndexed(bOpcode);
                    lookups.TrackVariableUsage(varScope, wVarIndex, isIndexed);
                    if (isIndexed) // The accumulator is used as an indexer.
                    {
                        for (size_t i = 0; i < node.GetChildCount(); i++)
                        {
                            Consumption cons = _GetInstructionConsumption(*node.Child(i)->GetCode());
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
                        Consumption cons = _GetInstructionConsumption(*node.Child(i)->GetCode());
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
                    pValue->SetValue(_GetVariableNameFromCodePos(pos, lookups, &varScope, &wIndex), ValueType::Token);
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
    
    // Up next: Opcode::TOSS
    unique_ptr<Comment> pComment = std::make_unique<Comment>();
    pComment->SetName("ERROR_UNIMPLEMENTED_OPCODE");
    return unique_ptr<SyntaxNode>(move(pComment));
}
#endif


std::unique_ptr<SyntaxNode> _CodeNodeToSyntaxNode(CodeNode &node, DecompileLookups &lookups)
{
    bool preferLValue = lookups.PreferLValue;
    PreferLValue resetPreferLValue(lookups, false); // Reset it, because if it's true it's not normal.

    code_pos pos = node.GetCode();
    Opcode bOpcode = pos->get_opcode();
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
                uint16_t theValue = pos->get_first_operand();
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
        if (node.GetChildCount() >= 1)
        {
            if (node.GetChildCount() == 3)
            {
                // The unusual case... our hack for a ternary operator.
                unique_ptr<CppIfStatement> pTernary = std::make_unique<CppIfStatement>();
                pTernary->MakeTernary();
                CodeNode *pConditionNode = node.Child(0);
                // REVIEW - laura bow, script 30 has Opcode::CLASS here... ?
                ASSERT(pConditionNode->GetCode()->get_opcode() == Opcode::BNT);
                unique_ptr<ConditionalExpression> condition = make_unique<ConditionalExpression>();
                _ApplySyntaxNodeToCodeNode(*pConditionNode, *condition.get(), lookups);
                pTernary->SetCondition(move(condition));
                _ApplySyntaxNodeToCodeNode1(*node.Child(1), *pTernary, lookups);
                _ApplySyntaxNodeToCodeNode2(*node.Child(2), *pTernary, lookups);
                return unique_ptr<SyntaxNode>(move(pTernary));
            }
            else
            {
                // Fwd to the first child (should be what gets in to the accumulator)
                return _CodeNodeToSyntaxNode(*node.Child(0), lookups);
            }
        }
        break;

    case Opcode::CALLK:   // kernel, # of params
    case Opcode::CALLB:   // mainproc index, # of params
    case Opcode::CALLE:   // script, index, # of params
    case Opcode::CALL:    // offset, # of params
        {
            WORD cParams = (bOpcode == Opcode::CALLE) ? pos->get_third_operand() : pos->get_second_operand();
            cParams /= 2; // bytes -> params
            cParams += 1; // +1 because there is a parameter count first.
            std::stringstream ss;
            switch (bOpcode)
            {
            case Opcode::CALLK:
                ss << lookups.LookupKernelName(pos->get_first_operand());
                break;
            case Opcode::CALLB:
                ss << "proc000_" << pos->get_first_operand();
                break;
            case Opcode::CALLE:
                ss << _GetPublicProcedureName(pos->get_first_operand(), pos->get_second_operand());
                break;
            case Opcode::CALL:
                ss << _GetProcNameFromScriptOffset(pos->get_final_postop_offset() + pos->get_first_operand());
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

            _MassageProcedureCall(*pProcCall, lookups, pos);
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
                // TODO - actually you can send to any super class... the first operand says which
                sendCall->SetName("super");
            }
            Consumption cons = _GetInstructionConsumption(*pos);
            WORD cStackPushesLeft = cons.cStackConsume;
            WORD cAccLeft = (bOpcode == Opcode::SEND) ? 1 : 0;
            size_t i = 0;
            WORD cParamsLeft = 0;
            bool fLookingForSelector = true;
            unique_ptr<SendParam> sendParam;

            while ((cAccLeft || cStackPushesLeft) && (i < node.GetChildCount()))
            {
                CodeNode *pPreviousChild = i ? node.Child(i - 1) : nullptr;
                if (cAccLeft)
                {
                    Consumption consAcc = _GetInstructionConsumption(*node.Child(i)->GetCode());
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
                    Consumption consAcc = _GetInstructionConsumption(*node.Child(i)->GetCode());
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
                                    assert(false);
                                }
                            }
                            else
                            {
                                sendParam->SetName(lookups.LookupSelectorName(wValue));
                                sendParam->SetIsMethod(!_MaybeAProperty(sendParam->GetName()));
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

            sendCall->SimplifySendObject();
            return unique_ptr<SyntaxNode>(move(sendCall));
        }
        break;

    case Opcode::BNT:
    case Opcode::BT:
        // REVIEW -> ternary operator is a common thing (e.g. space quest script 005)
        // How can we do things that cross branch boundaries?  We could look for this pattern:
        //      eq?
        //      bnt A
        //      *something*
        //      jmp B
        // A:   *something*
        // B:   push
        //

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
            CodeNode *pCodeNodeStack = node.Child(0);
            CodeNode *pCodeNodeAcc = node.Child(1);
            Consumption cons = _GetInstructionConsumption(*pCodeNodeStack->GetCode());
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
            CodeNode *pCodeNodeAcc = node.Child(0);
            Consumption cons = _GetInstructionConsumption(*pCodeNodeAcc->GetCode());
            ASSERT(cons.cAccGenerate);
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
            WORD wType = pos->get_first_operand();
            unique_ptr<PropertyValue> pValue = std::make_unique<PropertyValue>();
            VarScope varScope;
            WORD wVarIndex;
            pValue->SetValue(_GetVariableNameFromCodePos(pos, lookups, &varScope, &wVarIndex), ValueType::Pointer);
            lookups.TrackVariableUsage(varScope, wVarIndex, true);  // Let's consider this indexed.
            assert((wType & LEA_ACC_AS_INDEX_MOD) == 0); // REVIEW TODO
            return unique_ptr<SyntaxNode>(move(pValue));
        }
        break;

    case Opcode::CLASS:
    case Opcode::PUSHSELF:
        {
            std::string className = (bOpcode == Opcode::CLASS) ? lookups.LookupClassName(pos->get_first_operand()) : SelfToken;
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
            SCIVersion version = appState->GetVersion();
            uint16_t wName = version.lofsaOpcodeIsAbsolute ?
                pos->get_first_operand() :
                (pos->get_first_operand() + pos->get_final_postop_offset());
            std::string name = lookups.LookupScriptThing(wName, type);
            if (name.empty())
            {
                name = InvalidLookupError;
                appState->LogInfo("Unable to find symbol for %d.", wName);
            }
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
        }
        break;

    case Opcode::JMP:
        // Already handled by branching code
        // OH BUT IS IT
        if (lookups.BreakExit._Ptr)
        {
            if (pos->get_branch_target() == (*lookups.BreakExit)->GetCode())
            {
                // This jump points to the break exit.
                return unique_ptr<SyntaxNode>(new BreakStatement());
            }
        }

        return unique_ptr<SyntaxNode>(new Comment());
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
                lValue->SetName(_GetVariableNameFromCodePos(pos, lookups, &varScope, &wVarIndex));
                bool isIndexed = _IsVOIndexed(bOpcode);
                lookups.TrackVariableUsage(varScope, wVarIndex, isIndexed);
                if (isIndexed) // The accumulator is used as an indexer.
                {
                    for (size_t i = 0; i < node.GetChildCount(); i++)
                    {
                        Consumption cons = _GetInstructionConsumption(*node.Child(i)->GetCode());
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
                    Consumption cons = _GetInstructionConsumption(*node.Child(i)->GetCode());
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
                pValue->SetValue(_GetVariableNameFromCodePos(pos, lookups, &varScope, &wIndex), ValueType::Token);
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

    // Up next: Opcode::TOSS
    unique_ptr<Comment> pComment = std::make_unique<Comment>();
    pComment->SetName("ERROR_UNIMPLEMENTED_OPCODE");
    return unique_ptr<SyntaxNode>(move(pComment));
}



std::string DecompileLookups::LookupSelectorName(WORD wIndex)
{
    return _pLookups->LookupSelectorName(wIndex);
}
std::string DecompileLookups::LookupKernelName(WORD wIndex)
{
    return _pLookups->LookupKernelName(wIndex);
}
std::string DecompileLookups::LookupClassName(WORD wIndex)
{
    std::string ret = _pLookups->LookupClassName(wIndex);
    if (ret.empty())
    {
        ret = _pPrivateSpecies->LookupClassName(wIndex);
    }
    return ret;
}
bool DecompileLookups::LookupSpeciesPropertyList(WORD wIndex, std::vector<WORD> &props)
{
    bool fRet = _pLookups->LookupSpeciesPropertyList(wIndex, props);
    if (!fRet)
    {
        fRet = _pPrivateSpecies->LookupSpeciesPropertyList(wIndex, props);
    }
    return fRet;
}
bool DecompileLookups::LookupSpeciesPropertyListAndValues(WORD wIndex, std::vector<WORD> &props, std::vector<WORD> &values)
{
    bool fRet = _pLookups->LookupSpeciesPropertyListAndValues(wIndex, props, values);
    if (!fRet)
    {
        fRet = _pPrivateSpecies->LookupSpeciesPropertyListAndValues(wIndex, props, values);
    }
    return fRet;
}
std::string DecompileLookups::ReverseLookupGlobalVariableName(WORD wIndex)
{
    static std::string s_defaults[] = {
        "gEgo",
        "gGame",
        "gRoom",
        "gSpeed",
        "gQuitGame",
        "gCast",
        "gRegions",
        "gLocales",
        "gTimers",
        "gSounds",  // gInv, SQ
        "gInv",     // gAddToPics...
        "gAddToPics",   // something passed to OwnedBy
        "gFeatures",
        "gSFeatures",
        "gRoomNumberExit",
        "gPreviousRoomNumber",
        "gRoomNumber",
        "gDebugOnExit",
        "gScore",
        "gMaxScore",
    };


    std::string result = _pOFLookups->ReverseLookupGlobalVariableName(wIndex);
    if (result.empty())
    {
        // Supply some defaults.  These may be different for different games.
        if (wIndex < ARRAYSIZE(s_defaults))
        {
            result = s_defaults[wIndex];
        }
    }
    return result;
}
std::string DecompileLookups::ReverseLookupPublicExportName(WORD wScript, WORD wIndex)
{
    std::string ret = _pOFLookups->ReverseLookupPublicExportName(wScript, wIndex);
    if (ret.empty())
    {
        // This will be true if there are no .sco/.scop files.
        ret = _GetPublicProcedureName(wScript, wIndex);
    }
    return ret;
}
std::string DecompileLookups::LookupPropertyName(WORD wPropertyIndex)
{
    if (_pPropertyNames)
    {
        return _pPropertyNames->LookupPropertyName(this, wPropertyIndex);
    }
    else
    {
        return "PROPERTY-ACCESS-IN-NON-METHOD";
    }
}
std::string DecompileLookups::LookupScriptThing(WORD wName, ICompiledScriptSpecificLookups::ObjectType &type) const
{
    return _pScriptThings->LookupObjectName(wName, type);
}
std::string DecompileLookups::LookupParameterName(WORD wIndex)
{
    if (wIndex)
    {
        const FunctionSignature &signature = *_pFunc->GetSignatures()[0];
        size_t iRealIndex = (wIndex - 1);
        ASSERT(iRealIndex < signature.GetParams().size()); // Since it was us who analyzed the code and added the right # of params
        return signature.GetParams()[iRealIndex]->GetName();
    }
    else
    {
        return "paramTotal"; // parameter 0 is the count of params
    }
}

std::string DecompileLookups::LookupTextResource(WORD wIndex) const
{
    std::string ret;
    if (_pTextResource)
    {
        ret = _pTextResource->Lookup(wIndex);
    }
    return ret;
}

void DecompileLookups::SetPosition(sci::SyntaxNode *pNode)
{
    pNode->SetPosition(_fakePosition);
    _fakePosition = LineCol(_fakePosition.Line() + 1, 0);
}

void DecompileLookups::EndowWithFunction(sci::FunctionBase *pFunc)
{
    _pFunc = pFunc;
    if (_pFunc)
    {
        _functionTrackingName = GetMethodTrackingName(_pFunc->GetOwnerClass(), *_pFunc);
    }
    else
    {
        _functionTrackingName.clear();
        _restStatementTrack.clear();
    }
}

void DecompileLookups::TrackVariableUsage(VarScope varScope, WORD wIndex, bool isIndexed)
{
    map<WORD, bool> *pFunctionVarUsage = nullptr;
    if (varScope == VarScope::Local)
    {
        pFunctionVarUsage = &_localVarUsage;
    }
    else if (varScope == VarScope::Temp)
    {
        auto &it = _tempVarUsage.find(_functionTrackingName);
        if (it != _tempVarUsage.end())
        {
            pFunctionVarUsage = &(*it).second;
        }
        else
        {
            _tempVarUsage[_functionTrackingName] = map<WORD, bool>();
            pFunctionVarUsage = &_tempVarUsage[_functionTrackingName];
        }
    }

    if (pFunctionVarUsage)
    {
        // We can't just blindly set isIndexed. If a variable is used in both manners, we'll treat
        // it as indexed. So check if something is already there.
        auto &findIt = pFunctionVarUsage->find(wIndex);
        if (findIt != pFunctionVarUsage->end())
        {
            (*pFunctionVarUsage)[wIndex] = isIndexed || findIt->second;
        }
        else
        {
            (*pFunctionVarUsage)[wIndex] = isIndexed;
        }
    }
}

void DecompileLookups::TrackRestStatement(sci::RestStatement *rest, uint16_t index)
{
    _restStatementTrack.emplace_back(rest, index);//.push_back(std::pair < RestStatement)
}

void DecompileLookups::ResolveRestStatements()
{
    for (auto &pair : _restStatementTrack)
    {
        uint16_t varIndex = pair.second;
        FunctionSignature &signature = *_pFunc->GetSignaturesNC()[0];
        size_t iRealIndex = (varIndex - 1);
        if (iRealIndex < signature.GetParams().size())
        {
            // There is already a named parameter for this guy. Use this name for the rest statement.
            pair.first->SetName(signature.GetParams()[iRealIndex]->GetName());
        }
        else
        {
            // The more common case. Use "params"
            pair.first->SetName(RestParamName);
            // Now, we need this in the function signature too. 
            // We shouldn't ever need to add more than one parameter.
            // REVIEW: This probably shouldn't be an assert, since it could theoretically happen.
            // But I'd like to leave it in for now until I know the code is correct
            assert(iRealIndex == signature.GetParams().size());
            signature.AddParam(RestParamName);
        }
    }
}

void CalculateVariableRanges(const std::map<WORD, bool> &usage, WORD variableCount, vector<VariableRange> &varRanges)
{
    // (1) This first part of the code attempts to figure out which variables are arrays and which are not.
    VariableRange currentVarRange = { 0, 0 };
    bool hasVariableInProcess = false;
    bool isCurrentIndexed = false;
    auto &usageIterator = usage.cbegin();
    auto &endUsage = usage.cend();
    for (WORD i = 0; i < variableCount; i++)
    {
        if ((usageIterator != endUsage) && (usageIterator->first == i))
        {
            // We start a new variable here
            if (hasVariableInProcess)
            {
                // If we have something in process, add it now
                currentVarRange.arraySize = (i - currentVarRange.index);
                ASSERT(isCurrentIndexed || (currentVarRange.arraySize == 1));
                varRanges.push_back(currentVarRange);
            }

            // Now start the new one
            currentVarRange.index = i;
            isCurrentIndexed = usageIterator->second;
            hasVariableInProcess = true;

            ++usageIterator;
        }
        else
        {
            if (hasVariableInProcess)
            {
                // This is a new one, unless we're indexed
                if (!isCurrentIndexed)
                {
                    currentVarRange.arraySize = 0;
                    varRanges.push_back(currentVarRange);

                    // But start a new one. isCurrentIndexed stays false.
                    // Actually, let it be indexed. Otherwise unused arrays take
                    // up tons of noise.
                    isCurrentIndexed = true;
                    currentVarRange.index = i;
                    hasVariableInProcess = true;
                }
                // else if indexed, just continue
            }
            else
            {
                // Just make a single one
                varRanges.push_back(VariableRange() = { i, 1 });
            }
        }
    }

    // Need one at the end too
    if (hasVariableInProcess)
    {
        currentVarRange.arraySize = variableCount - currentVarRange.index;
        varRanges.push_back(currentVarRange);
    }
}

void AddLocalVariablesToScript(sci::Script &script, DecompileLookups &lookups, const std::vector<WORD> &localVarValues)
{
    // Based on what we find in lookups, we should be able to deduce what is an array and what is not.
    // And we should be able to initialize things too. Default values are zero.
    vector<VariableRange> varRanges;
    CalculateVariableRanges(lookups.GetLocalUsage(), static_cast<WORD>(localVarValues.size()), varRanges);

    // The next step is to supply values and initializers
    // Script local variables are zero-initialized by default. So we don't need to assign anything to them if the value is zero.
    for (VariableRange &varRange : varRanges)
    {
        unique_ptr<VariableDecl> localVar = std::make_unique<VariableDecl>();
        localVar->SetDataType("var"); // For now...
        localVar->SetName(_GetLocalVariableName(varRange.index));
        localVar->SetSize(varRange.arraySize);

        int wStart = varRange.index;
        int wEnd = varRange.index + varRange.arraySize;
        // Now we need to determine if we are setting any initializers.
        // We need to provide initializers all the way up to the last non-zero value.
        WORD firstAllZeroesFromHereIndex = wStart;
        for (int i = wEnd - 1; i >= wStart; i--)
        {
            if (localVarValues[i] != 0)
            {
                firstAllZeroesFromHereIndex = static_cast<WORD>(i + 1);
                break;
            }
        }

        // We need to supply values from varRange.index to firstAllZeroesFromHereIndex
        for (WORD w = varRange.index; w < firstAllZeroesFromHereIndex; w++)
        {
            PropertyValue value;
            value.SetValue(localVarValues[w]);
            localVar->AddSimpleInitializer(value);
        }

        script.AddVariable(move(localVar));
    }
}

// TODOs:
//
// Try to identify places where constants may be used... e.g.
// (send gEgo.onControl(FALSE)) != ctlCREEN)
//
// Give names to things which have certain things called on them... e.g.
// param1.claimed  -->  pEvent.claimed
// Or just do obvious things for changeState and handleEvent....
//
//
// Apply popular defines to things, e.g.
// Load(rsVIEW, 123);
// or, if this is possible,
// gEgo.onControl(FALSE) == ctlGREEN
// ignoreControl(ctrlGREEN);
// ignoreActors(TRUE)
//
// Assume stuff for the Print thing.... so we can do selectors....
// And for methods like "eachElementDo"
//
//
// Know what the Print procedure is for all games... store it somewhere??? e.g. SQ3 is proc255_0.
// Detect if it has two parameters, the first which matches the script number... if so, then go load
// the text resource for this, and replace that in the thing.
//
// Have a visitor pattern that looks at binary ops that are & or |, and makes any child property values be hex

// REVIEW - features TODO
//
// How to handle/detect break statements?
// How to handle && in a while loop?
//
//
// REVIEW -bugs
// Internal strings are doubled???? 

//
// REVIEW BUGS in SCRIPTS
// 92,91,90 has the pprev:     
/*
    bnt--> B
      le?
        push0
        send
          pushi
          push0
          lofsa
    bnt--> C
      B:not
        le?
          pprev
          ldi
    bnt--> D
      gt?
        push
          send
            pushi
            push0
            lag
        ldi



I think this was the pattern for
(if (<= 0 findScript.state() 3)
)
*/
// We might be able to fake it by copying the tree under pprev
//	look backwards for a branch instruction, and find what the accumulator
//	would have been
//
//
//
// Script 75 has an issue in rm075::doit, where the stuff at the end doesn't work.
// It is nested ternary operators
//
// Script 52 has an LEA
//
//
// We have problems with this in the main script... 
// EXPORTED procedure #12 ()
/*
(procedure proc_1278
  1278:81 19              lag  
  127a:30 0005            bnt code_1282 
  127d:39 5b            pushi 5b // $5b dispose
  127f:76               push0 
  1280:4a 04             send 4 


        code_1282
  1282:48                 ret 
)
*/
// and with procedure 15... which has OnControl(dup)

// BUG:
// Conquests of Camelot, script 005, treasurerActions::handleEvent, lots of degenerate if statements.
//                       script 005, pouch::handleEvent, same thing... some new kind of branching structure.