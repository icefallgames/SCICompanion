#include "stdafx.h"
#include "scii.h"

#include <numeric>

OperandType *GetOperandTypes(Opcode opcode)
{
	return OpArgTypes[static_cast<BYTE>(opcode)];
}

// Instructions that pop from the stack, and how many frames they pop
int scii::is_stackpop_op()
{
    switch (_bOpcode)
    {
    case Opcode::SEND:
    case Opcode::SELF:
        return (int)get_first_operand() / 2;    // pops this many things off the stack
    case Opcode::SUPER:
        return (int)get_second_operand() / 2;
    case Opcode::CALLK:
    case Opcode::CALLB:
        return (int)(get_second_operand() + 2) / 2;
    case Opcode::CALLE:
        return (int)(get_third_operand() + 2) / 2;
    case Opcode::BNOT:
    case Opcode::ADD:
    case Opcode::SUB:
    case Opcode::MUL:
    case Opcode::DIV:
    case Opcode::MOD:
    case Opcode::SHR:
    case Opcode::SHL:
    case Opcode::XOR:
    case Opcode::AND:
    case Opcode::OR:
    case Opcode::NEG:
    case Opcode::EQ:
    case Opcode::NE:
    case Opcode::GT:
    case Opcode::GE:
    case Opcode::LT:
    case Opcode::LE:
    case Opcode::UGT:
    case Opcode::UGE:
    case Opcode::ULT:
    case Opcode::ULE:
    case Opcode::TOSS:
        return 1;
    default:
        return false;
    }
    
}

// Instructions that push onto the stack.
bool scii::is_stackpush_op()
{
    switch (_bOpcode)
    {
    case Opcode::PUSH:
    case Opcode::PUSHI:
    case Opcode::DUP:
    case Opcode::PPREV:
    case Opcode::PTOS:
    case Opcode::IPTOS:
    case Opcode::DPTOS:
    case Opcode::LOFSS:
    case Opcode::PUSH0:
    case Opcode::PUSH1:
    case Opcode::PUSH2:
    case Opcode::PUSHSELF:
    case Opcode::LSG:
    case Opcode::LSL:
    case Opcode::LST:
    case Opcode::LSP:
    case Opcode::LSGI:
    case Opcode::LSLI:
    case Opcode::LSTI:
    case Opcode::LSPI:
        return true;
    default:
        return false;
    }
}



bool scii::is_acc_op()
{
    switch (_bOpcode)
    {
    case Opcode::BNOT:
    case Opcode::ADD:
    case Opcode::SUB:
    case Opcode::MUL:
    case Opcode::DIV:
    case Opcode::MOD:
    case Opcode::SHR:
    case Opcode::SHL:
    case Opcode::XOR:
    case Opcode::AND:
    case Opcode::OR:
    case Opcode::NEG:
    case Opcode::LDI:
    case Opcode::CALL:
    case Opcode::CALLK:
    case Opcode::CALLB:
    case Opcode::CALLE:
    case Opcode::SEND:
    case Opcode::CLASS:
    case Opcode::LEA:
    case Opcode::SELFID:
    case Opcode::PTOA:
    case Opcode::IPTOA:
    case Opcode::DPTOA:
    case Opcode::LOFSA:
    case Opcode::LAG:
    case Opcode::LAL:
    case Opcode::LAT:
    case Opcode::LAP:
    case Opcode::LAGI:
    case Opcode::LALI:
    case Opcode::LATI:
    case Opcode::LAPI:
        return true;
    default:
        return false;
    }
}


bool scii::is_cmp_op()
{
    return false;
/*#define Opcode::NOT							12
#define Opcode::EQ							13
#define Opcode::NE							14
#define Opcode::GT							15
#define Opcode::GE							16
#define Opcode::LT							17
#define Opcode::LE							18
#define Opcode::UGT							19
#define Opcode::UGE							20
#define Opcode::ULT							21
#define Opcode::ULE							22
*/

}

bool scii::is_branch_op()
{
    return false;
/*#define Opcode::BT							23      // branch on true
#define Opcode::BNT							24      // branch on not true
#define Opcode::JMP							25      // jump
*/
}


WORD scii::_get_instruction_size(Opcode bOpcode, OPSIZE opSize)
{
    ASSERT(opSize != Undefined);
	OperandType *argTypes = ::GetOperandTypes(bOpcode);
    WORD wSize = 1; // for the opcode
    bool fDone = false;
    for (int i = 0; !fDone && i < 3; i++)
    {
        switch (argTypes[i])
        {
        case otEMPTY:
            fDone = true;
            break;
        case otVAR:
        case otPVAR:
        case otCLASS:
        case otPROP:
        case otSTRING:
        case otSAID:
        case otKERNEL:
        case otPUBPROC:
        case otINT:
        case otUINT:
        case otOFFS:
        case otLABEL:
            wSize += ((opSize == Word) ? 2 : 1);
            break;

        case otINT16:
            wSize += 2;
            break;
        case otINT8:
            wSize++;
            break;
        }
    }
    return wSize;
}


WORD scii::size()
{
    ASSERT(_opSize != Undefined);
    ASSERT(_wSize > 0);
    return _wSize;
}

//
// Calculate the distance from one piece of code to another.
// There is some redundancy here, since from/to together, imply whether or not
// this is a forward or backward branch.  However, that's hard to determine.
// Backward jumps return negative numbers.
//
WORD distance(code_pos from, code_pos to, bool fForward, int *pfNeedToRedo)
{
    WORD wCodeDistance = 0;
    int iDebug = 0;
    while (iDebug < 100 && (from != to))
    {
        wCodeDistance += (*from).calc_size(from, pfNeedToRedo);
        ++from;
    }
    if (!fForward)
    {
        ASSERT(wCodeDistance > 0);
        wCodeDistance = (~wCodeDistance) + 1; // 2's complement
    }
    return wCodeDistance;
}

OperandType *scii::GetOperandTypes()
{
	return ::GetOperandTypes(_bOpcode);
}

void scii::set_final_branch_operands(code_pos self)
{
    ASSERT(_opSize != Undefined);
    ASSERT(_fUndetermined == false);
    if (_is_label_instruction())
    {
		ASSERT(GetOperandTypes()[0] == otLABEL);
        WORD wCodeDistance = 0;
        int fRedoDummy = 0;
        if (_fForwardBranch)
        {
            // Forward...
            code_pos curPos = self;
            curPos++;
            _wOperands[0] = distance(curPos, _itOffset, _fForwardBranch, &fRedoDummy);
        }
        else
        {
            // Backward...
            code_pos curPos = _itOffset;
            self++; // We're jumping back from *after* our current pos (reverse jmps are a little longer than fwds)
            _wOperands[0] = distance(_itOffset, self, _fForwardBranch, &fRedoDummy);                            
        }
        ASSERT(!fRedoDummy);
    }
}

WORD scii::calc_size(code_pos self, int *pfNeedToRedo)
{
    ASSERT(_fUndetermined == false); // Better not have any undertermined branches
    if (_opSize == Undefined)
    {
		OperandType *argTypes = GetOperandTypes();
        bool fDone = false;
        OPSIZE opSizeCalculated = _fForceWord ? Word : Byte; // Optimistic - unless we already tried this.
        if (_is_label_instruction())
        {
            // These instructions require calculating the distance to somewhere, to
            // determine their size.  As a result they could cause re-entrancy.  If we don't set a size
            // right now, we could end up in an infinite loop.  So let's be optimistic and assume we're a
            // Byte-based instruction.
            _opSize = opSizeCalculated;
            _wSize = _get_instruction_size(_bOpcode, _opSize);
        }
        if (opSizeCalculated == Byte) // Only makes sense to do the expensive calculation if we don't know for sure that we're a Word
        {
            for (int i = 0; !fDone && i < 3; i++)
            {
                // Treat everything as signed...
                switch (argTypes[i])
                {
                case otEMPTY:
                    fDone = true;
                    break;
                case otVAR:
                case otPVAR:
                case otCLASS:
                case otPROP:
                case otSTRING:
                case otSAID:
                case otKERNEL:
                case otPUBPROC:
                case otINT:
                case otUINT:
                    // These are variable length parameters.  If we have a big one, we'll need to be a word.
                    if (_wOperands[i] > 127)
                    {
                        // WORK ITEM: Maybe optimize for signed values... So -ve numbers can be smaller
                        opSizeCalculated = Word; // No way around it (unless we can optimize to 255 for some cases?)
                        fDone = true;
                    }
                    break;

                case otOFFS:
                    // itOffset
                    // Offsets are likely to need to be WORDs
                    opSizeCalculated = Word;
                    fDone = true;
                    break;

                case otLABEL:
                    {
                        ASSERT(_is_label_instruction());
                        WORD wCodeDistance = 0;
                        if (_fForwardBranch)
                        {
                            // Forward...
                            code_pos curPos = self;
                            curPos++;
                            wCodeDistance = distance(curPos, _itOffset, _fForwardBranch, pfNeedToRedo);
                        }
                        else
                        {
                            // Backward...
                            code_pos curPos = _itOffset;
                            self++; // We're jumping back from *after* our current pos (reverse jmps are a little longer than fwds)
                            wCodeDistance = distance(_itOffset, self, _fForwardBranch, pfNeedToRedo);                            
                        }
                        
                        if ((wCodeDistance > 127) && (wCodeDistance <= 0xff80))
                        {
                            if (_opSize == Byte)
                            {
                                (*pfNeedToRedo)++;
                                _fForceWord = true;
                            }
                            opSizeCalculated = Word;
                            fDone = true;
                        }
                    }
                    break;

                case otINT16:
                    opSizeCalculated = Word;
                    break;
                case otINT8:
                    break;
                }
            }
        }

        // Now actually calculate the size.
        // Set _wSize and _opSize together
        if (_is_label_instruction())
        {
            if (_opSize < opSizeCalculated)
            {
                // Our optimistic guess was wrong.  Let this instruction know that next time it needs
                // to be a Word-sized one.
                _fForceWord = true;
                (*pfNeedToRedo)++;
            }
            else
            {
                ASSERT(_wSize == _get_instruction_size(_bOpcode, opSizeCalculated));
            }
        }
        _wSize = _get_instruction_size(_bOpcode, opSizeCalculated);
        _opSize = opSizeCalculated; 
    }
    return _wSize;
}



void push_wordIt(std::vector<BYTE> &output, WORD w)
{
    // big-endian
    output.push_back(w & 0xff);
    output.push_back(w >> 8);
}

#define WORD_OP(x) ((x)<<1)
#define BYTE_OP(x) (((x)<<1) | 1)

//
// LOFSA and LOFSS instructions:
//  Initially, the otOFFS is an index that will point to an absolute offset in the script resource.
//  Once the absolute index is resolved, we do a replace
//  When it comes time to output the code, we'll turn that absolute offset into a relative offset.
//

void scii::output_code(std::vector<BYTE> &output)
{
    _wFinalOffset = (WORD)output.size();
    ASSERT(_bOpcode != Opcode::INDETERMINATE); // Any invalid instructions must be replaced before writing the code.
	BYTE bOpcodeTemp = static_cast<BYTE>(_bOpcode);
	output.push_back((_opSize == Byte) ? BYTE_OP(bOpcodeTemp) : WORD_OP(bOpcodeTemp));
    bool fDone = false;
	OperandType *argTypes = GetOperandTypes();
    for (int i = 0; !fDone && i < 3; i++)
    {
        // Treat everything as signed...
        bool bByte = false;
        switch (argTypes[i])
        {
        case otEMPTY:
            fDone = true;
            break;
        case otVAR:
        case otPVAR:
        case otCLASS:
        case otPROP:
        case otSTRING:
        case otSAID:
        case otKERNEL:
        case otPUBPROC:
        case otINT:
        case otUINT:
        case otOFFS:
        case otLABEL:
            bByte = (_opSize == Byte);
            break;

        case otINT16:
            bByte = false;
            break;
        case otINT8:
            bByte = true;
            break;
        }
    
        WORD wOperand = _wOperands[i];
        if (argTypes[i] == otOFFS)
        {
            // Turn the absolute offset into a relative offset.
            ASSERT(_opSize == Word); // For now...
            // This will be an absolute index.  We need to turn it into a relative one.
            WORD wTo = _wOperands[i];
            WORD wFrom = _wFinalOffset + _wSize; // post instruction PC
            wOperand = wTo - wFrom;
        }

        if (!fDone)
        {
            if (bByte)
            {
                // Temporarily commented this out since we aren't resolving jmps properly yet.
                //ASSERT(_wOperands[i] < 128);
                output.push_back((BYTE)wOperand);
            }
            else
            {
                push_wordIt(output, wOperand);
            }
        }
    }    
}


//
// The size of the entire piece of code, guaranteed to return something with a WORD boundary.
//
WORD scicode::calc_size()
{
    for(scii &instruction : _code)
    {
        if (!instruction.is_branch_determined())
        {
            // Hopefully we resolved this at an earlier point and produced an error.
            // assert(false);
            instruction.set_branch_target(_code.begin(), false);
        }
    }

    WORD wDistance = 0;
    int fNeedToRedo = false;
    do
    {
        wDistance = 0;
        code_pos curPos = _code.begin();
        fNeedToRedo = 0;
        while (curPos != _code.end())
        {
            wDistance += (*curPos).calc_size(curPos, &fNeedToRedo);
            ++curPos;
        }
        if (fNeedToRedo)
        {
            for_each(_code.begin(), _code.end(), std::mem_fun_ref(&scii::reset_size));
        }
    } while (fNeedToRedo);

    // One final pass, to set the branch instructions.
    code_pos curPos = _code.begin();
    while (curPos != _code.end())
    {
        curPos->set_final_branch_operands(curPos);
        ++curPos;
    }
    return wDistance;
}

WORD scicode::offset_of(code_pos target)
{
    WORD wDistance = 0;
    code_pos curPos = _code.begin();
    while (curPos != target)
    {
        wDistance += (*curPos).size();
        curPos++;
    }
    return wDistance;
}


void scicode::write_code(std::vector<BYTE> &output)
{
    for(scii &instruction : _code)
    {
        instruction.output_code(output);
    }
}


//
// Go through the code, and for any LOFSS or LOFSA instruction, resolve its index.
//
void scicode::fixup_offsets(const std::unordered_map<WORD, WORD> &fixups)
{
    for(scii &instruction : _code)
    {
        Opcode bOpcode = instruction.get_opcode();
        if ((bOpcode == Opcode::LOFSS) || (bOpcode == Opcode::LOFSA))
        {
            auto fixupIt = fixups.find(instruction.get_first_operand());
            if (fixupIt != fixups.end())
            {
                instruction.update_first_operand(fixupIt->second);
            }
        }
    }
}

void scicode::enter_branch_block(int index)
{
    fixup_todos emptyList;
    _fixupFrames[index].push(emptyList);
}
void scicode::leave_branch_block(int index)
{
    fixup_todos &todos = _fixupTodos[index];
    fixup_frames &frames = _fixupFrames[index];
    todos.insert(todos.end(), frames.top().begin(), frames.top().end());
    frames.pop(); // We're done with this frame - it's been moved to the todo list
}
bool scicode::in_branch_block(int index)
{
    // Ask if there is a block frame of this type
    return !_fixupFrames[index].empty();
}

void scicode::set_call_target(code_pos thisInstruction, code_pos callsHere)
{
    // Need to figure out if this is a branch forward or backward.
    code_pos curPos = thisInstruction;
    while ((curPos != callsHere) && (curPos != _code.end()))
    {
        curPos++;
    }
    bool fForward = (curPos == callsHere);
    (*thisInstruction).set_branch_target(callsHere, fForward);
}

void scicode::_insertInstruction(const scii &inst)
{
    // We may not always be inserting at the end...
    code_pos insertHere = _insertionPoints.empty() ? get_end() : _insertionPoints.top();

    // Were there any branch targets that pointed to here?  We'll need to fix them up again.
    bool fReFixup = _targetToSources.find(insertHere) != _targetToSources.end();

    // Insert the instruction.
    // PERF: linked list allocations show up as about 1% in total compile times.
    // Not much, but we could perhaps improve by using the boost fast_pool_allocator?
    _code.insert(insertHere, inst);

    if (fReFixup)
    {
        std::vector<code_pos> rereFixups;
        // These need to point to the current position (the instruction we just wrote) instead
        code_pos curPos = get_cur_pos();
        code_pos_multimap::iterator it = _targetToSources.lower_bound(insertHere);
        code_pos_multimap::iterator itLast = _targetToSources.upper_bound(insertHere);
        while (it != itLast)
        {
            // Repoint it to here:
            it->second->set_branch_target(curPos, true); // Assume always a forward branch.

            // Add a new entry (to replace the old) that maps this target to its source
            // We'll need to batch these up till the end though (or else infinite loop)
            rereFixups.push_back(it->second);

            ++it;
        }

        // Remove all old entries that we just re-mapped.
        _targetToSources.erase(insertHere);

        // Add the sources back for a different target.
        // We do indeed need to do this.  The following code exposes it:
        /*
        while (1)
        {
            if ((!(temp0 = super.doit(temp0))) || (temp0 == -1) || (temp0 == btnHandle))
            {
                break;
            }  // -> without it, here we'll end up jumping to somewhere
            (temp0.cast(DButton).value).cast(InvI).showSelf();
        }*/
        for(code_pos &foo : rereFixups)
        {
            _targetToSources.insert(make_pair(curPos, foo));
        }
    }

}

void scicode::_checkBranchResolution()
{
    for(fixup_todos_map::value_type &todoValue : _fixupTodos)
    {
        fixup_todos &todo = todoValue.second;
        // Now for a particular type of fixup, we have a list of code_pos's that need to be processed.
        for(code_pos &fixupIndex : todo)
        {
            fixupIndex->set_branch_target(get_cur_pos(), true);

            // Track it in case we end up pointing it somewhere else.
            _targetToSources.insert(make_pair(get_cur_pos(), fixupIndex));
        }
        todo.clear();
    }
}

//
// Returns true if all branch instructions that are looking for targets
// are preceded by a return instruction.
//
bool scicode::_areAllPriorInstructionsReturns(const fixup_todos &todos)
{
    bool fRet = true;
    code_pos pos;
    
    for(code_pos pos : todos)
    {
        code_pos preceding = pos;
        ASSERT(pos != _code.begin()); // Otherwise we deref invalid memory (we should never start a fnc with a branch instruction)
        --preceding;
        if (preceding->get_opcode() != Opcode::RET)
        {
            return false;
        }
    }
    return fRet;
}

bool scicode::has_dangling_branches(bool &fAllBranchesPrecededByReturns)
{
    fAllBranchesPrecededByReturns = true;
#ifdef DEBUG_COMPILE
    char sz[100];
#endif
    bool fDangle = false;
    fixup_todos_map::const_iterator fixupIt = _fixupTodos.begin();
    while (fixupIt != _fixupTodos.end())
    {
        if (!fixupIt->second.empty())
        {
            fDangle = true;
#ifdef DEBUG_COMPILE
            if (fDangle)
            {
                StringCchPrintf(sz, ARRAYSIZE(sz), "%d todos left for %d", fixupIt->second.size(), fixupIt->first);
                OutputDebugString(sz);
            }
#endif
        }
        if (!_areAllPriorInstructionsReturns(fixupIt->second))
        {
            fAllBranchesPrecededByReturns = false;
        }
        ++fixupIt;
    }
    fixup_frames_map::const_iterator fixupFrameIt = _fixupFrames.begin();
    while (fixupFrameIt != _fixupFrames.end())
    {
        if (!fixupFrameIt->second.empty())
        {
            fDangle = true;
#ifdef DEBUG_COMPILE
            if (fDangle)
            {
                StringCchPrintf(sz, ARRAYSIZE(sz), "%d frames left for %d", fixupFrameIt->second.size(), fixupFrameIt->first);
                OutputDebugString(sz);
            }
#endif
        }
        ++fixupFrameIt;
    }
    return fDangle;
}


// This craziness is so that code_pos can be used in a multimap.
// We use the node pointer as a < comparator.  Just something consistent but meaningless.
bool operator<(const code_pos &_Right, const code_pos &_Left)
{
    return _Right._Mynode() < _Left._Mynode();
}

