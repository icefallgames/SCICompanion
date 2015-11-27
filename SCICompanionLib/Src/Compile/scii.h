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
//
// SCI instruction (byte code)
//
#pragma once

#include <stack>

// Fwd declaration
enum OperandType : uint8_t;
enum class Opcode : uint8_t;

const OperandType *GetOperandTypes(const SCIVersion &version, Opcode opcode);

enum class BranchBlockIndex
{
    Default = 0,
    Success = 0,     // block for succeeded conditions
    Failure = 1,     // block for failed conditions
    Or = 2,          // For conditional expressions
    Break = 3,       // block for break statements
    PostElse = 4,    // block for the end of an if statement jumping to after the else.
    Continue = 5,    // block for continue statements
};

class scii
{
private:
    typedef std::list<scii>::iterator _code_pos;
public:
    // WORK ITEM: need to assert if we specify a bad opcode (e.g. acLOFSS) with an inappropriate constructor
    // WORK ITEM: put in operand # verification
    scii(const SCIVersion &version, Opcode bOpcode);
    scii(const SCIVersion &version, Opcode bOpcode, uint16_t w1);
    scii(const SCIVersion &version, Opcode bOpcode, uint16_t w1, uint16_t w2);
    scii(const SCIVersion &version, Opcode bOpcode, uint16_t w1, uint16_t w2, uint16_t w3);
    scii(const SCIVersion &version, Opcode bOpcode, _code_pos branch, bool fUndetermined);

    uint16_t size();
    uint16_t calc_size(_code_pos self, int *pfNeedToRedo);
    void set_final_branch_operands(_code_pos self);
    void reset_size();
    void set_branch_target(_code_pos offset, bool fForward);
    bool is_forward_branch();
    _code_pos get_branch_target();
    void absolute_offset(uint16_t wAbsolute);
    void update_first_operand(uint16_t wValue);
    void update_second_operand(uint16_t wValue);
    uint16_t get_operand(int i)  const;
    uint16_t get_first_operand() const;
    uint16_t get_second_operand() const;
    uint16_t get_third_operand() const;

    void output_code(std::vector<BYTE>&);

    Opcode get_opcode() const;
    Opcode set_opcode(Opcode opcode);

    // Get the final offset at which the instruction was written.
    uint16_t get_final_offset() const;
    uint16_t get_final_postop_offset() const;

    uint16_t get_final_offset_dontcare() const;

    // Used for decompilation
    void set_offset_and_size(uint16_t wOffset, uint16_t wSize);

#ifdef DEBUG
    void set_debug_info(int p);
#endif
    bool is_branch_determined();

    //
    // For decompilation
    // 
    int is_stackpop_op();
    bool is_stackpush_op();
    bool is_acc_op();
    void mark();
    bool is_marked();
    bool _is_branch_instruction();
    bool is_conditional_branch_instruction();

    static uint16_t GetInstructionSize(const SCIVersion &version, uint8_t rawOpcode);

private:
    bool _is_label_instruction();

	const OperandType *GetOperandTypes() const;
    uint16_t _wOperands[3];
    uint16_t _wSize;
    bool _fForceWord; // This instruction must be a uint16_t sized one on the next attempt at calculating size.
    bool _fUndetermined;
    bool _fForwardBranch;
    Opcode _bOpcode;
    int _pDebug;      // Extra info for debugging.

    _code_pos _itOffset;
    uint16_t _wFinalOffset;
    const SCIVersion *_version; // pointer instead of reference between we need to support assignment op.

    enum OPSIZE
    {
        Undefined = 0,
        Byte = 1,
        Word = 2,
    };
    OPSIZE _opSize;

    static uint16_t _get_instruction_size(const SCIVersion &version, Opcode bOpcode, OPSIZE opSize);
};

//
// Can't use vector + iterators, since iterators get invalidate when you add things.
// We use a list, so we can keep track of references to other parts of the code, while
// we delete and add things.
//

//
// A basic list doesn't quite provide enough functionality, so abstract it a little
// more via a class.
//
typedef std::list<scii>::iterator code_pos;

// Imminent fixups to be done when the next instruction is written
typedef std::vector<code_pos> fixup_todos;
typedef std::unordered_map<BranchBlockIndex, fixup_todos> fixup_todos_map;

// Record keeping of code_pos's that need to be moved to the above "todo" list when we leave a branch block

typedef std::stack<fixup_todos> fixup_frames;
typedef std::unordered_map<BranchBlockIndex, fixup_frames> fixup_frames_map;

// Is a map of "this target" was pointed at by "these sources"
typedef std::multimap<code_pos, code_pos> code_pos_multimap;

class scicode
{
public:
    scicode(const SCIVersion &version) : _version(version) {}
    ~scicode()
    {
        ASSERT(_insertionPoints.empty());
    }

    // Write instructions to the stream
	void inst(Opcode bOpcode)
    {
        _insertInstruction(scii(_version, bOpcode));
        _checkBranchResolution();
    }
	void inst(Opcode bOpcode, uint16_t w1)
    {
        _insertInstruction(scii(_version, bOpcode, w1));
        _checkBranchResolution();
    }
	void inst(Opcode bOpcode, uint16_t w1, uint16_t w2)
    {
        _insertInstruction(scii(_version, bOpcode, w1, w2));
        _checkBranchResolution();
    }
	void inst(Opcode bOpcode, uint16_t w1, uint16_t w2, uint16_t w3)
    {
        _insertInstruction(scii(_version, bOpcode, w1, w2, w3));
        _checkBranchResolution();
    }
	bool inst(Opcode bOpcode, code_pos branch, BranchBlockIndex index = BranchBlockIndex::Default)
    {
        bool fUndetermined = (branch == get_undetermined());
        _insertInstruction(scii(_version, bOpcode, branch, fUndetermined));
        // Check branch resolution now, before we possibly add ourselves to the resolution list.
        // Otherwise, *we'll* be counted!
        _checkBranchResolution();
        if (fUndetermined)
        {
            //ASSERT(_branchFixupFrames.size() > 0); // If we have branch instruction, better have a branch frame.
            // Track this by placing the current instruction in our list of branch fixups
            // (so we can come back to it)
            if (_fixupFrames[index].size() > 0)
            {
                _fixupFrames[index].top().push_back(get_cur_pos());
            }
            else
            {
                // An internal compiler error? Well, it means we had an else statement not preceded by an if.
                return false;
            }
        }
        return true;
    }
#ifdef DEBUG
    void set_debug_info(int p) { (*get_cur_pos()).set_debug_info(p); }
#endif

    code_pos get_cur_pos()
    {
        assert(!_code.empty());
        return get_cur_pos_dangerous();
    }

    // Callers need to realize this will get back invalid code_pos if
    // there is not yet any code.
    code_pos get_cur_pos_dangerous()
    {
        if (_insertionPoints.empty())
        {
            //return --_code.end();

			// We kind of rely on this to put us in front...
			//return code_pos();
			// How about this?
			// return _code.end();
			// Nope, that didn't work. We have two things. _insertionPoints and _code. _code can be non empty while _insertionPoints is not.
			if (_code.empty())
			{
				return _code.end();		// A bogus sentinel value
			}
			else
			{
				return --_code.end();	// This should work... but is it what we want?
			} // i.e. why is _insertionPoints empty?
        }
        else
        {
            // "current position" is just before the insertion point.
            code_pos temp = _insertionPoints.top();
            return --temp;
        }
    }
    code_pos get_beginning() { return _code.begin(); }
    // Use the "end" as a marker for undetermined
    code_pos get_undetermined() { return _code.end(); }
    code_pos get_end() { return _code.end(); }

    // Put these around any code blocks that may generate branches.
    // Supplemental is needed when we have overlapping branch blocks.
	void enter_branch_block(BranchBlockIndex index = BranchBlockIndex::Default);
	void leave_branch_block(BranchBlockIndex index = BranchBlockIndex::Default);
	bool in_branch_block(BranchBlockIndex index);

    uint16_t calc_size();
    uint16_t offset_of(code_pos target);
    void write_code(std::vector<BYTE> &output);
    bool has_dangling_branches(bool &fAllBranchesAreReturns);
    bool empty() { return _code.empty(); }
	void fixup_offsets(const std::unordered_map<uint16_t, uint16_t> &fixups);
    
    // Insert new instructions here, instead of at the end.
    void push_code_insertion_point(code_pos pos)
    {
        _insertionPoints.push(pos);
    }
    void pop_code_insertion_point()
    {
        _insertionPoints.pop();
    }

    // Helper function for scii.set_branch_target, which automatically figures out the branch direction.
    void set_call_target(code_pos thisInstruction, code_pos callsHere);

    void enter_continue_frame(code_pos pos) { _continueFrames.push_back(pos); }
    void leave_continue_frame() { _continueFrames.pop_back(); }
    bool get_continue_target(uint16_t levels, code_pos &target)
    {
        if (levels <= _continueFrames.size())
        {
            target = _continueFrames[_continueFrames.size() - levels];
            return true;
        }
        return false;
    }

private:

    void _insertInstruction(const scii &inst);
    void _checkBranchResolution();
    bool _areAllPriorInstructionsReturns(const fixup_todos &todos);

    std::list<scii> _code;
    fixup_frames_map _fixupFrames;

    // Map of "stack frames" to todo-lists of branch instructions that need targets.
    fixup_todos_map _fixupTodos;

    bool _fInsertionPoint;
    std::stack<code_pos> _insertionPoints;
    code_pos_multimap _targetToSources;

    std::vector<code_pos> _continueFrames;

    const SCIVersion &_version;
};

// This craziness is so that code_pos can be used in a multimap.
// We use the node pointer as a < comparator.  Just something consistent but meaningless.
bool operator<(const code_pos &_Right, const code_pos &_Left);

