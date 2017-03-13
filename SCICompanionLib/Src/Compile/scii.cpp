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
#include "stdafx.h"
#include "scii.h"
#include "PMachine.h"

#include <numeric>
#include <format.h>

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

uint16_t scii::GetInstructionSize(const SCIVersion &version, uint8_t rawOpcode)
{
    Opcode opcode = RawToOpcode(version, rawOpcode);
    return _get_instruction_size(version, opcode, (rawOpcode & 1) ? Byte : Word);
}

uint16_t scii::GetInstructionArgumentSize(const SCIVersion &version, uint8_t rawOpcode)
{
    return GetInstructionSize(version, rawOpcode) - 1;
}

uint16_t scii::_get_instruction_size(const SCIVersion &version, Opcode bOpcode, OPSIZE opSize)
{
    assert(opSize != Undefined);
    const OperandType *argTypes = ::GetOperandTypes(version, bOpcode);
    uint16_t wSize = 1; // for the opcode
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
        case otUINT16:
            wSize += 2;
            break;
        case otINT8:
        case otUINT8:
            wSize++;
            break;
        }
    }
    return wSize;
}


uint16_t scii::size()
{
    assert(_opSize != Undefined);
    assert(_wSize > 0);
    return _wSize;
}

//
// Calculate the distance from one piece of code to another.
// There is some redundancy here, since from/to together, imply whether or not
// this is a forward or backward branch.  However, that's hard to determine.
// Backward jumps return negative numbers.
//
uint16_t distance(const SCIVersion &version, code_pos from, code_pos to, bool fForward, int *pfNeedToRedo)
{
    uint16_t wCodeDistance = 0;
    int iDebug = 0;
    while (iDebug < 100 && (from != to))
    {
        wCodeDistance += (*from).calc_size(from, pfNeedToRedo);
        ++from;
    }
    if (!fForward)
    {
        assert(wCodeDistance > 0);
        wCodeDistance = (~wCodeDistance) + 1; // 2's complement
    }
    return wCodeDistance;
}

const OperandType *scii::GetOperandTypes() const
{
    return ::GetOperandTypes(*_version, _bOpcode);
}

void scii::set_final_branch_operands(code_pos self)
{
    assert(_opSize != Undefined);
    assert(_fUndetermined == false);
    if (_is_label_instruction())
    {
        assert(GetOperandTypes()[0] == otLABEL);
        uint16_t wCodeDistance = 0;
        int fRedoDummy = 0;
        if (_fForwardBranch)
        {
            // Forward...
            code_pos curPos = self;
            curPos++;
            _wOperands[0] = distance(*_version, curPos, _itOffset, _fForwardBranch, &fRedoDummy);
        }
        else
        {
            // Backward...
            code_pos curPos = _itOffset;
            self++; // We're jumping back from *after* our current pos (reverse jmps are a little longer than fwds)
            _wOperands[0] = distance(*_version, _itOffset, self, _fForwardBranch, &fRedoDummy);
        }
        assert(!fRedoDummy);
    }
}

uint16_t scii::calc_size(code_pos self, int *pfNeedToRedo)
{
    assert(_fUndetermined == false); // Better not have any undertermined branches
    if (_opSize == Undefined)
    {
		const OperandType *argTypes = GetOperandTypes();
        bool fDone = false;
        OPSIZE opSizeCalculated = _fForceWord ? Word : Byte; // Optimistic - unless we already tried this.
        if (_is_label_instruction())
        {
            // These instructions require calculating the distance to somewhere, to
            // determine their size.  As a result they could cause re-entrancy.  If we don't set a size
            // right now, we could end up in an infinite loop.  So let's be optimistic and assume we're a
            // Byte-based instruction.
            _opSize = opSizeCalculated;
            _wSize = _get_instruction_size(*_version, _bOpcode, _opSize);
        }
        if (opSizeCalculated == Byte) // Only makes sense to do the expensive calculation if we don't know for sure that we're a Word
        {
            bool encounteredVariableSizeOperand = false;

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
                case otUINT:
                    encounteredVariableSizeOperand = true;
                    // These are variable length parameters.  If we have a big one, we'll need to be a word.
                    if (_wOperands[i] > 127)
                    {
                        // WORK ITEM: Maybe optimize for signed values... So -ve numbers can be smaller
                        opSizeCalculated = Word; // No way around it (unless we can optimize to 255 for some cases?)
                        fDone = true;
                    }
                    break;

                case otINT:
                {
                    encounteredVariableSizeOperand = true;
                    int16_t signedOperand = (int16_t)_wOperands[i];
                    if ((signedOperand > 127) || (signedOperand < -128))
                    {
                        opSizeCalculated = Word; // No way around it (unless we can optimize to 255 for some cases?)
                        fDone = true;
                    }
                }
                    break;

                case otOFFS:
                    // itOffset
                    // Offsets are likely to need to be WORDs
                    // In SCI1.1 this is mandatory. The offsets are pointers
                    // into the heap resources, and we have a relocation table that assumes
                    // these pointers are all words.
                    opSizeCalculated = Word;
                    fDone = true;
                    break;

                case otLABEL:
                    {
                        encounteredVariableSizeOperand = true;
                        assert(_is_label_instruction());
                        uint16_t wCodeDistance = 0;
                        if (_fForwardBranch)
                        {
                            // Forward...
                            code_pos curPos = self;
                            curPos++;
                            wCodeDistance = distance(*_version, curPos, _itOffset, _fForwardBranch, pfNeedToRedo);
                        }
                        else
                        {
                            // Backward...
                            code_pos curPos = _itOffset;
                            self++; // We're jumping back from *after* our current pos (reverse jmps are a little longer than fwds)
                            wCodeDistance = distance(*_version, _itOffset, self, _fForwardBranch, pfNeedToRedo);
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
                case otUINT16:
                    opSizeCalculated = Word;
                    break;
                case otINT8:
                case otUINT8:
                    break;
                }
            }

            // For guys with no variable size operands, use the word-sized versions.
            // This is an attempt to make SCI1 work. The byte-sized pushSelf is a _file_ opcode in SCI1.
            // http://sourceforge.net/p/scummvm/bugs/5113/
            if (!encounteredVariableSizeOperand)
            {
                opSizeCalculated = Word;
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
                assert(_wSize == _get_instruction_size(*_version, _bOpcode, opSizeCalculated));
            }
        }
        _wSize = _get_instruction_size(*_version, _bOpcode, opSizeCalculated);
        _opSize = opSizeCalculated; 
    }
    return _wSize;
}



void push_wordIt(std::vector<BYTE> &output, uint16_t w)
{
    // big-endian
    output.push_back(w & 0xff);
    output.push_back(w >> 8);
}

//
// LOFSA and LOFSS instructions:
//  SCI0: Initially, the otOFFS is an index that will point to an absolute offset in the script resource.
//  Once the absolute index is resolved, we do a replace
//  When it comes time to output the code, we'll turn that absolute offset into a relative offset.
// Note: SCI01+ games use a relative offset instead.
// See SCIVersion::lofsaOpcodeIsAbsolute

void scii::output_code(ITrackCodeSink &trackCodeSink, std::vector<BYTE> &output)
{
    _wFinalOffset = (uint16_t)output.size();
    assert(_bOpcode != Opcode::INDETERMINATE); // Any invalid instructions must be replaced before writing the code.
    output.push_back(OpcodeToRaw(*_version, _bOpcode, (_opSize != Byte)));
    bool fDone = false;
	const OperandType *argTypes = GetOperandTypes();
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
        case otUINT16:
            bByte = false;
            break;
        case otINT8:
        case otUINT8:
            bByte = true;
            break;
        }
    
        uint16_t wOperand = _wOperands[i];
        if (argTypes[i] == otOFFS)
        {
            trackCodeSink.WroteCodeSink(wOperand, (uint16_t)output.size());
        }
        else if (argTypes[i] == otINT)
        {
            int16_t signedOperand = (int16_t)wOperand;
            assert(!bByte || ((signedOperand >= -128) && (signedOperand <= 127)));
            if (bByte)
            {
                wOperand = (uint16_t)((int8_t)signedOperand);
            }
        }

        if (!fDone)
        {
            if (bByte)
            {
                // Temporarily commented this out since we aren't resolving jmps properly yet.
                //assert(_wOperands[i] < 128);
                output.push_back((uint8_t)wOperand);
            }
            else
            {
                push_wordIt(output, wOperand);
            }
        }
    }    
}


//
// The size of the entire piece of code, guaranteed to return something with a uint16_t boundary.
//
uint16_t scicode::calc_size()
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

    uint16_t wDistance = 0;
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

uint16_t scicode::offset_of(code_pos target)
{
    uint16_t wDistance = 0;
    code_pos curPos = _code.begin();
    while (curPos != target)
    {
        wDistance += (*curPos).size();
        curPos++;
    }
    return wDistance;
}


void scicode::write_code(ITrackCodeSink &trackCodeSink, std::vector<uint8_t> &output, std::vector<uint8_t> *debugInfoOpt)
{
    if (debugInfoOpt)
    {
        push_word(*debugInfoOpt, (uint16_t)_code.size());
        for (scii &instruction : _code)
        {
            // Line number for every instruction
            push_word(*debugInfoOpt, (uint16_t)output.size());
            push_word(*debugInfoOpt, (uint16_t)instruction.LineNumber);
            instruction.output_code(trackCodeSink, output);
        }
    }
    else
    {
        for (scii &instruction : _code)
        {
            instruction.output_code(trackCodeSink, output);
        }
    }
}


void scicode::enter_branch_block(BranchBlockIndex index)
{
    fixup_todos emptyList;
    _fixupFrames[index].push_back(emptyList);
}
void scicode::leave_branch_block(BranchBlockIndex index)
{
    fixup_todos &todos = _fixupTodos[index];
    fixup_frames &frames = _fixupFrames[index];
    todos.insert(todos.end(), frames.back().begin(), frames.back().end());
    frames.pop_back(); // We're done with this frame - it's been moved to the todo list
}
bool scicode::in_branch_block(BranchBlockIndex index, uint16_t levels)
{
    // Ask if there is a block frame of this type
    auto &frames = _fixupFrames[index];
    return frames.size() >= levels;
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
    code_pos insertSpot = _code.insert(insertHere, inst);

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
        if (pos != _code.begin()) // Otherwise we deref invalid memory (we should never start a fnc with a branch instruction). Can happen if there are script errors.
        {
            --preceding;
            if (preceding->get_opcode() != Opcode::RET)
            {
                return false;
            }
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


scii::scii(const SCIVersion &version, Opcode bOpcode, int lineNumber) : scii(version, bOpcode, (uint16_t)0xffff, lineNumber) {}

scii::scii(const SCIVersion &version, Opcode bOpcode, uint16_t w1, int lineNumber) : scii(version, bOpcode, w1, (uint16_t)0xffff, lineNumber) {}

scii::scii(const SCIVersion &version, Opcode bOpcode, uint16_t w1, uint16_t w2, int lineNumber) : scii(version, bOpcode, w1, w2, (uint16_t)0xffff, lineNumber) {}

scii::scii(const SCIVersion &version, Opcode bOpcode, uint16_t w1, uint16_t w2, uint16_t w3, int lineNumber) : _version(&version), LineNumber(lineNumber)
{
    _opSize = Undefined;
    _wSize = 0;
    _fForceWord = false;
    _fUndetermined = false;
    _bOpcode = bOpcode;
    assert(_bOpcode <= Opcode::LastOne);
    _wOperands[0] = w1;
    _wOperands[1] = w2;
    _wOperands[2] = w3;
    _wFinalOffset = 0xffff;
    assert(!_is_branch_instruction());
#ifdef DEBUG
    _pDebug = 0;
#endif
}

scii::scii(const SCIVersion &version, Opcode bOpcode, _code_pos branch, bool fUndetermined, int lineNumber) : _version(&version), LineNumber(lineNumber)
{
    _opSize = Undefined;
    _wSize = 0;
    _fForceWord = false;
    _fUndetermined = fUndetermined;
    _bOpcode = bOpcode;
    assert(_bOpcode <= Opcode::LastOne);
    _itOffset = branch;
    // Assume a backward branch for now...
    // (if branch is .end(), then we'll need to fix it up later anyhow, via set_branch_target)
    _fForwardBranch = false;
    _wFinalOffset = 0xffff;
#ifdef DEBUG
    _pDebug = 0;
#endif
}

void scii::reset_size() { _wSize = 0; _opSize = Undefined; }

void scii::set_branch_target(_code_pos offset, bool fForward)
{
    _itOffset = offset;
    _fForwardBranch = fForward;
    _fUndetermined = false;
    assert(_is_label_instruction());
}

bool scii::is_forward_branch()
{
    return _fForwardBranch;
}

scii::_code_pos scii::get_branch_target()
{
    assert(_is_label_instruction());
    return _itOffset;
}

void scii::absolute_offset(uint16_t wAbsolute)
{
    assert((_bOpcode == Opcode::LOFSS) || (_bOpcode == Opcode::LOFSA) || (_bOpcode == Opcode::CALL));
    // This is the absolute offset to a string or said (not yet the relative one, but it's progress)
    _wOperands[0] = wAbsolute;
    // When we actually write the instruction to a stream, we'll need to diff the current location
    // with this one, and write the relative offset.
}

void scii::update_first_operand(uint16_t wValue)
{
    _wOperands[0] = wValue;
}
void scii::update_second_operand(uint16_t wValue)
{
    _wOperands[1] = wValue;
}
uint16_t scii::get_operand(int i)  const { assert(GetOperandTypes()[i] != otEMPTY); return _wOperands[i]; }
uint16_t scii::get_first_operand() const
{
    return _wOperands[0];
}
uint16_t scii::get_second_operand() const
{
    return _wOperands[1];
}
uint16_t scii::get_third_operand() const
{
    return _wOperands[2];
}

Opcode scii::get_opcode() const { return _bOpcode; }
Opcode scii::set_opcode(Opcode opcode) { return _bOpcode = opcode; }

// Get the final offset at which the instruction was written.
uint16_t scii::get_final_offset() const  { assert(_wFinalOffset != 0xffff); return _wFinalOffset; }
uint16_t scii::get_final_postop_offset() const { return get_final_offset() + _wSize; }

uint16_t scii::get_final_offset_dontcare() const  { return _wFinalOffset; }

// Used for decompilation
void scii::set_offset_and_size(uint16_t wOffset, uint16_t wSize) { _wFinalOffset = wOffset; _wSize = wSize; }

#ifdef DEBUG
void scii::set_debug_info(int p) { _pDebug = p; }
#endif
bool scii::is_branch_determined() { return !_fUndetermined; }

void scii::mark() { }
bool scii::is_marked() { return false; }

bool scii::_is_branch_instruction()
{
    return (_bOpcode == Opcode::BNT) || (_bOpcode == Opcode::BT) || (_bOpcode == Opcode::JMP);
}

bool scii::is_conditional_branch_instruction()
{
    return (_bOpcode == Opcode::BNT) || (_bOpcode == Opcode::BT);
}

bool scii::_is_label_instruction()
{
    return (_bOpcode == Opcode::BNT) || (_bOpcode == Opcode::BT) || (_bOpcode == Opcode::JMP) || (_bOpcode == Opcode::CALL);
}



// This craziness is so that code_pos can be used in a multimap.
// We use the node pointer as a < comparator.  Just something consistent but meaningless.
bool operator<(const code_pos &_Right, const code_pos &_Left)
{
    return _Right._Mynode() < _Left._Mynode();
}

