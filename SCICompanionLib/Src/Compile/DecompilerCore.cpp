#include "stdafx.h"
#include "DecompilerCore.h"
#include "PMachine.h"
#include "ScriptOMAll.h"
#include "scii.h"
#include "DisassembleHelper.h"
#include "ControlFlowGraph.h"
#include "DecompilerNew.h"
#include "DecompilerFallback.h"
#include "format.h"
#include <iterator>

#define DEBUG_DECOMPILER 1

using namespace sci;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool GetBinaryOpFromAssignment(std::string &strOp);

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

std::string _GetPublicProcedureName(WORD wScript, WORD wIndex)
{
    std::stringstream ss;
    ss << "proc" << wScript << "_" << wIndex;
    return ss.str();
}

std::string _GetBaseProcedureName(WORD wIndex)
{
    return _GetPublicProcedureName(0, wIndex);
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
// Returns the end.
const BYTE *_ConvertToInstructions(std::list<scii> &code, const BYTE *pBegin, const BYTE *pEnd, WORD wBaseOffset)
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

    return pCur;
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
        uint16_t wType = pos->get_first_operand();
        fRet = (static_cast<VarScope>(static_cast<BYTE>((wType >> 1))& 0x03) == varScope);
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
        else if (pos->get_opcode() == Opcode::REST)
        {
            // REST usage also indicates params.
            uint16_t restIndex = pos->get_first_operand();
            wBiggest = max(restIndex, wBiggest);
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

Consumption _GetInstructionConsumption(scii &inst, DecompileLookups *lookups)
{
    Opcode bOpcode = inst.get_opcode();
    ASSERT(bOpcode <= Opcode::LastOne);
    int cEatStack = 0;
    bool fChangesAcc = false;
    bool fEatsAcc = false;
    bool fPutsOnStack = false;
    bool fEatsPrev = false;
    bool fChangesPrev = false;
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
        fChangesAcc = true;
        fEatsAcc = true;
        cEatStack = 1;
        break;

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
        fChangesPrev = true;
        fChangesAcc = true;
        fEatsAcc = true;
        cEatStack = 1;
        break;

    case Opcode::BT:
    case Opcode::BNT:
        fEatsAcc = true;
        break;

    case Opcode::RET:
        if (lookups && lookups->FunctionDecompileHints.ReturnsValue)
        {
            fEatsAcc = true;
        }
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
        fEatsPrev = true;
        break;

    case Opcode::REST:
        // Doesn't really affect anything
        break;

    case Opcode::LEA:
        fEatsAcc = !!(static_cast<BYTE>(inst.get_first_operand() >> 1) & LEA_ACC_AS_INDEX_MOD);
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
    if (fEatsPrev)
    {
        cons.cPrevConsume++;
    }
    if (fChangesPrev)
    {
        cons.cPrevGenerate++;
    }

    return cons;
}

// Works backward from branchInstruction to beginning (inclusive) until the sequence of instructions leading up to the branch
// has been satisfied. If it fails, it returns false.
//
// This fails in situations where it shouldn't need to. For instance, in SQ5, script 801, offset 0x34a
// sat	temp[$4]                // This is the acc we needed
// pushi	$b4; 180, check     // this was for a compare operation after callk
// pushi	$4; cel             // first stack push for callk
// lst	temp[$5]
// lst	temp[$6]
// lst	temp[$3]                // Oh... no acc, it's another stack... don't need a stack, so I bail
// push                         // Here's a stack, but this needs acc
// callk	kernel_63, $8       // I need 5 stacks
//
// We'd need to implement some kind of instruction borrowing system to get this to work.
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

ControlFlowNode *GetFirstSuccessorOrNull(ControlFlowNode *node)
{
    assert(node->Successors().size() <= 1);
    return node->Successors().empty() ? nullptr : *node->Successors().begin();
}

class DetermineHexValues : public IExploreNode, public IExploreNodeContext
{
public:
    DetermineHexValues(FunctionBase &func)
    {
        useHex.push(true);
        func.Traverse(this, *this);
    }

    void ExploreNode(IExploreNodeContext *pContext, SyntaxNode &node, ExploreNodeState state) override
    {
        // Set property values to hex if we should:
        if (state == ExploreNodeState::Pre)
        {
            PropertyValue *propValue = SafeSyntaxNode<PropertyValue>(&node);
            if (propValue && useHex.top())
            {
                propValue->_fHex = true;
            }
        }

        // Push a "hex context" if we are in a bitwise operation.
        string operation;
        BinaryOp *binaryOp = SafeSyntaxNode<BinaryOp>(&node);
        if (binaryOp)
        {
            operation = binaryOp->GetName();
        }
        UnaryOp *unaryOp = SafeSyntaxNode<UnaryOp>(&node);
        if (unaryOp)
        {
            operation = unaryOp->GetName();
        }
        Assignment *assignment = SafeSyntaxNode<Assignment>(&node);
        if (assignment)
        {
            operation = assignment->GetName();
            GetBinaryOpFromAssignment(operation);
        }

        if (operation == "|" || operation == "&" || operation == "^" || operation == "bnot" || operation == ">>" || operation == "<<")
        {
            if (state == ExploreNodeState::Pre)
            {
                useHex.push(true);
            }
            else if (state == ExploreNodeState::Post)
            {
                useHex.pop();
            }
        }
        else
        {
            // Ignore certain "insignificant" syntax nodes
            if (node.GetNodeType() != NodeTypeStatement)
            {
                if (state == ExploreNodeState::Pre)
                {
                    useHex.push(false);
                }
                else if (state == ExploreNodeState::Post)
                {
                    useHex.pop();
                }
            }
        }
    }

private:
    stack<bool> useHex;
};

class DetermineNegativeValues : public IExploreNode, public IExploreNodeContext
{
public:
    DetermineNegativeValues(FunctionBase &func)
    {
        useNeg.push(true);
        func.Traverse(this, *this);
    }

    void ExploreNode(IExploreNodeContext *pContext, SyntaxNode &node, ExploreNodeState state) override
    {
        // Set property values to hex if we should:
        if (state == ExploreNodeState::Pre)
        {
            PropertyValue *propValue = SafeSyntaxNode<PropertyValue>(&node);
            if (propValue && useNeg.top())
            {
                if ((propValue->GetType() == ValueType::Number) && (propValue->GetNumberValue() > 32768))
                {
                    propValue->_fNegate = true;
                }
            }
        }

        // Push a "neg context" if we are in a signed comparison
        string operation;
        BinaryOp *binaryOp = SafeSyntaxNode<BinaryOp>(&node);
        if (binaryOp)
        {
            operation = binaryOp->GetName();
        }
        Assignment *assignment = SafeSyntaxNode<Assignment>(&node);
        if (assignment)
        {
            operation = assignment->GetName();
            GetBinaryOpFromAssignment(operation);
        }

        if (operation == "+" || operation == "-" || operation == "<" || operation == ">" || operation == "<=" || operation == ">=")
        {
            if (state == ExploreNodeState::Pre)
            {
                useNeg.push(true);
            }
            else if (state == ExploreNodeState::Post)
            {
                useNeg.pop();
            }
        }
        else
        {
            // Ignore certain "insignificant" syntax nodes
            if (node.GetNodeType() != NodeTypeStatement)
            {
                if (state == ExploreNodeState::Pre)
                {
                    useNeg.push(false);
                }
                else if (state == ExploreNodeState::Post)
                {
                    useNeg.pop();
                }
            }
        }
    }

private:
    stack<bool> useNeg;
};

void _DetermineIfFunctionReturnsValue(std::list<scii> code, DecompileLookups &lookups)
{
    // Look for return statements and see if they have any statements without side effects before them.
    code_pos cur = code.end();
    --cur;
    while (!lookups.FunctionDecompileHints.ReturnsValue && (cur != code.begin()))
    {
        if (cur->get_opcode() == Opcode::RET)
        {
            --cur;
            Opcode opcode = cur->get_opcode();
            switch (opcode)
            {
                // These instructions don't really have any effect other than
                // to put something in the accumulator. If they came right before a ret,
                // then it's safe to assume this function returns a value.
                // To be more complete, we could follow branches, as this is a common pattern that
                // won't be caught by us:
                // (if (blah)
                //     5
                // (else 4)
                // return

                case Opcode::LDI:
                case Opcode::SELFID:
                case Opcode::BNOT:
                case Opcode::NOT:
                case Opcode::NEG:
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
                case Opcode::PTOA:
                case Opcode::LOFSA:
                    lookups.FunctionDecompileHints.ReturnsValue = true;
                    break;

                default:
                    if ((opcode >= Opcode::LAG) && (opcode <= Opcode::LastOne))
                    {
                        if (!_IsVOStoreOperation(opcode) && !_IsVOPureStack(opcode))
                        {
                            lookups.FunctionDecompileHints.ReturnsValue = true;
                        }
                    }
                    // TODO: We could also check for zero parameter sends for known property selectors.
            }
        }
        else
        {
            --cur;
        }
    }
}

void _TrackExternalScriptUsage(std::list<scii> code, DecompileLookups &lookups)
{
    code_pos cur = code.end();
    --cur;
    while (cur != code.begin())
    {
        Opcode opcode = cur->get_opcode();
        switch (opcode)
        {
            case Opcode::CALLB:
                lookups.TrackUsingScript(0);
                break;

            case Opcode::CALLE:
                lookups.TrackUsingScript(cur->get_first_operand());
                break;

            case Opcode::CLASS:
            {
                uint16_t classIndex = cur->get_first_operand();
                uint16_t scriptNumber;
                if (lookups.GetSpeciesScriptNumber(classIndex, scriptNumber))
                {
                    lookups.TrackUsingScript(scriptNumber);
                }
                break;
            }

            case Opcode::LEA:
            {
                VarScope scope;
                _GetVariableNameFromCodePos(cur, lookups, &scope);
                if (scope == VarScope::Global)
                {
                    lookups.TrackUsingScript(0);
                }
                break;
            }

            default:
            {
                if ((opcode >= Opcode::LAG) && (opcode <= Opcode::LastOne))
                {
                    VarScope scope;
                    _GetVariableNameFromCodePos(cur, lookups, &scope);
                    if (scope == VarScope::Global)
                    {
                        lookups.TrackUsingScript(0);
                    }
                }
            }
                break;
        }

        --cur;
    }
}

// pEnd can be teh end of script data. I have added autodetection support.
void DecompileRaw(FunctionBase &func, DecompileLookups &lookups, const BYTE *pBegin, const BYTE *pEnd, WORD wBaseOffset)
{
    lookups.EndowWithFunction(&func);

    // Take the raw data, and turn it into a list of scii instructions, and make sure the branch targets point to code_pos's
    std::list<scii> code;
    const BYTE *discoveredEnd = _ConvertToInstructions(code, pBegin, pEnd, wBaseOffset);

    // Insert a no-op at the beginning of code (so we can get an iterator to point to a spot before code)
    code.insert(code.begin(), scii(Opcode::INDETERMINATE));

    // Do some early things
    _DetermineIfFunctionReturnsValue(code, lookups);

    // Construct the function -> for now use procedure, but really should be method or proc
    unique_ptr<FunctionSignature> pSignature = std::make_unique<FunctionSignature>();
    _FigureOutParameters(func, *pSignature, code);
    func.AddSignature(std::move(pSignature));

    _TrackExternalScriptUsage(code, lookups);

    string className = func.GetOwnerClass() ? func.GetOwnerClass()->GetName() : "";
    string messageDescription = fmt::format("{0} {1}::{2}: Analyzing control flow", func.GetOwnerScript()->GetName(), className, func.GetName());
    lookups.DecompileResults().AddResult(DecompilerResultType::Update, messageDescription);

    ControlFlowGraph cfg(messageDescription, lookups.DecompileResults(), GetMethodTrackingName(func.GetOwnerClass(), func, true), lookups.DebugControlFlow, lookups.pszDebugFilter);
    bool success = cfg.Generate(code.begin(), code.end());
    if (success && !lookups.DecompileResults().IsAborted())
    {
        const NodeSet &controlStructures = cfg.ControlStructures();
        MainNode *mainNode = cfg.GetMain();
        lookups.DecompileResults().AddResult(DecompilerResultType::Update, fmt::format("{0} {1}::{2}: Generating code", func.GetOwnerScript()->GetName(), className, func.GetName()));
        messageDescription = fmt::format("{0} {1}::{2}: Instruction consumption", func.GetOwnerScript()->GetName(), className, func.GetName());
        success = OutputNewStructure(messageDescription, func, *mainNode, lookups);
    }
    
    if (!success && !lookups.DecompileResults().IsAborted())
    {
        // Disassemble the function instead.
        // First though, we need to remove all code:
        func.GetStatements().clear();
        lookups.ResetOnFailure();

        lookups.DecompileResults().AddResult(DecompilerResultType::Important, fmt::format("Falling back to disassembly for {0}", func.GetName()));
        DisassembleFallback(func, code.begin(), code.end(), lookups);
    }

    // Give some statistics.
    lookups.DecompileResults().InformStats(success, discoveredEnd - pBegin);

    if (!lookups.DecompileResults().IsAborted())
    {
        DetermineHexValues determineHexValues(func);
        DetermineNegativeValues determinedNegValues(func);

        _FigureOutTempVariables(lookups, func, VarScope::Temp, code);

        lookups.ResolveRestStatements();

        lookups.EndowWithFunction(nullptr);

        func.PruneExtraneousReturn();
    }
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

//
// Try to massage the parameters so that it makes more sense to the user.
// TODO: Replace with a more generic mechanism
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

// Both for variable opcodes, and Opcode::LEA
std::string _GetVariableNameFromCodePos(code_pos pos, DecompileLookups &lookups, VarScope *pVarType, WORD *pwIndexOut)
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
                ss << _GetGlobalVariableName(wIndex);
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
bool DecompileLookups::LookupSpeciesPropertyListAndValues(WORD wIndex, std::vector<WORD> &props, std::vector<CompiledVarValue> &values)
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
        // Disable this and come up with a better solution that does this automatically
        /*
        // Supply some defaults.  These may be different for different games.
        if (wIndex < ARRAYSIZE(s_defaults))
        {
            result = s_defaults[wIndex];
        }*/
    }
    return result;
}
std::string DecompileLookups::ReverseLookupPublicExportName(WORD wScript, WORD wIndex)
{
    std::string ret = _pOFLookups->ReverseLookupPublicExportName(wScript, wIndex);
    if (ret.empty())
    {
        // This will be true if there are no .sco files.
        ret = _GetPublicProcedureName(wScript, wIndex);
    }
    return ret;
}
std::string DecompileLookups::LookupPropertyName(WORD wPropertyIndex)
{
    if (_pPropertyNames)
    {
        _requestedProperty = true;
        return _pPropertyNames->LookupPropertyName(this, wPropertyIndex);
    }
    else
    {
        return "PROPERTY-ACCESS-IN-NON-METHOD";
    }
}
bool DecompileLookups::LookupScriptThing(WORD wName, ICompiledScriptSpecificLookups::ObjectType &type, std::string &name) const
{
    return _pScriptThings->LookupObjectName(wName, type, name);
}

bool DecompileLookups::GetSpeciesScriptNumber(uint16_t species, uint16_t &scriptNumber)
{
    return _pLookups->GetGlobalClassTable().GetSpeciesScriptNumber(species, scriptNumber);
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

void DecompileLookups::_CategorizeSelectors()
{
    for (auto &script : _pLookups->GetGlobalClassTable().GetAllScripts())
    {
        for (auto &object : script->GetObjects())
        {
            for (uint16_t propSelector : object->GetProperties())
            {
                _propertySelectors.insert(propSelector);
            }
            for (uint16_t methodSelector : object->GetMethods())
            {
                _methodSelectors.insert(methodSelector);
            }
        }
    }
}

bool DecompileLookups::IsPropertySelectorOnly(uint16_t selector) const
{
    return (_propertySelectors.find(selector) != _propertySelectors.end()) &&
        (_methodSelectors.find(selector) == _methodSelectors.end());
}

void DecompileLookups::SetPosition(sci::SyntaxNode *pNode)
{
    pNode->SetPosition(_fakePosition);
    _fakePosition = LineCol(_fakePosition.Line() + 1, 0);
}

const ILookupPropertyName *DecompileLookups::GetPossiblePropertiesForProc(uint16_t localProcOffset)
{
    auto it = _localProcToPropLookups.find(localProcOffset);
    if (it != _localProcToPropLookups.end())
    {
        return it->second;
    }
    return nullptr;
}

void DecompileLookups::EndowWithFunction(sci::FunctionBase *pFunc)
{
    _pFunc = pFunc;
    FunctionDecompileHints.Reset();
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

const ClassDefinition *DecompileLookups::GetClassContext() const
{
    const MethodDefinition *method = SafeSyntaxNode<MethodDefinition>(_pFunc);
    if (method)
    {
        return method->GetOwnerClass();
    }
    return nullptr;
}

void DecompileLookups::TrackVariableUsage(VarScope varScope, WORD wIndex, bool isIndexed)
{
    map<WORD, bool> *pFunctionVarUsage = nullptr;
    if ((varScope == VarScope::Local) || ((GetScriptNumber() == 0) && (varScope == VarScope::Global)))
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

// REVIEW: Stuff that is tracked while decompiling should really be put elsewhere
// (i.e. not in DEcompileLookups, which should be read-only)
void DecompileLookups::ResetOnFailure()
{
    // This holds weak pointers to objects in the script, so clear those out.
    _restStatementTrack.clear();
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

void DecompileLookups::TrackProcedureCall(uint16_t offset)
{
    auto it = _localProcToPropLookups.find(offset);
    if (it == _localProcToPropLookups.end())
    {
        _localProcToPropLookups[offset] = _pPropertyNames;
    }
    else
    {
        // This procedure is called from multiple contexts (e.g. multiple
        // objects or other procedures) so we can't say it belongs to one object.
        if (it->second != _pPropertyNames)
        {
            _localProcToPropLookups[offset] = nullptr;
        }
    }
}

void CalculateVariableRanges(const std::map<WORD, bool> &usage, WORD variableCount, vector<VariableRange> &varRanges)
{
    // (1) This first part of the code attempts to figure out which variables are arrays and which are not.
    // For the global script, we assume none of the variables are arrays. We can't determine their usage from just
    // main itself, because all scripts use them.
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
            // This var index when never directly used.
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
                // Actually, let's make an indexed one.
                isCurrentIndexed = true;
                currentVarRange.index = i;
                hasVariableInProcess = true;
                //varRanges.push_back(VariableRange() = { i, 1 });
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

void AddLocalVariablesToScript(sci::Script &script, const CompiledScript &compiledScript, DecompileLookups &lookups, const std::vector<CompiledVarValue> &localVarValues)
{
    // Based on what we find in lookups, we should be able to deduce what is an array and what is not.
    // And we should be able to initialize things too. Default values are zero.
    vector<VariableRange> varRanges;
    if (script.GetScriptNumber() == 0)
    {
        for (size_t i = 0; i < localVarValues.size(); i++)
        {
            varRanges.push_back({ (uint16_t)i, 1 });
        }

    }
    else
    {
        CalculateVariableRanges(lookups.GetLocalUsage(), static_cast<WORD>(localVarValues.size()), varRanges);
    }

    // The next step is to supply values and initializers
    // Script local variables are zero-initialized by default. So we don't need to assign anything to them if the value is zero.
    for (VariableRange &varRange : varRanges)
    {
        unique_ptr<VariableDecl> localVar = std::make_unique<VariableDecl>();
        localVar->SetDataType("var"); // For now...
        localVar->SetName((script.GetScriptNumber() == 0) ? _GetGlobalVariableName(varRange.index) : _GetLocalVariableName(varRange.index));
        localVar->SetSize(varRange.arraySize);

        int wStart = varRange.index;
        int wEnd = varRange.index + varRange.arraySize;
        // Now we need to determine if we are setting any initializers.
        // We need to provide initializers all the way up to the last non-zero value.
        WORD firstAllZeroesFromHereIndex = wStart;
        for (int i = wEnd - 1; i >= wStart; i--)
        {
            if (localVarValues[i].value != 0)
            {
                firstAllZeroesFromHereIndex = static_cast<WORD>(i + 1);
                break;
            }
        }

        // We need to supply values from varRange.index to firstAllZeroesFromHereIndex
        for (WORD w = varRange.index; w < firstAllZeroesFromHereIndex; w++)
        {
            PropertyValue value;
            uint16_t propValue = localVarValues[w].value;
            if (localVarValues[w].isString)
            {
                value.SetValue(compiledScript.GetStringFromOffset(propValue), ValueType::String);
            }
            else
            {
                value.SetValue(propValue);
            }
            localVar->AddSimpleInitializer(value);
        }

        script.AddVariable(move(localVar));
    }
}
