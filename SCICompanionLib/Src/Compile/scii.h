//
// SCI instruction (byte code)
//
#pragma once

#include <stack>
#include "PMachine.h"

OperandType *GetOperandTypes(Opcode opcode);

class scii
{
private:
    typedef std::list<scii>::iterator _code_pos;
public:
    // WORK ITEM: need to assert if we specify a bad opcode (e.g. acLOFSS) with an inappropriate constructor
    // WORK ITEM: put in operand # verification
	scii(Opcode bOpcode) : scii(bOpcode, (WORD)0xffff) {}

	scii(Opcode bOpcode, WORD w1) : scii(bOpcode, w1, (WORD)0xffff) {}

	scii(Opcode bOpcode, WORD w1, WORD w2) : scii(bOpcode, w1, w2, (WORD)0xffff) {}

    scii(Opcode bOpcode, WORD w1, WORD w2, WORD w3)
    {
        _opSize = Undefined;
        _wSize = 0;
        _fForceWord = false;
        _fUndetermined = false;
        _bOpcode = bOpcode;
        ASSERT(_bOpcode <= Opcode::LastOne);
        _wOperands[0] = w1;
        _wOperands[1] = w2;
        _wOperands[2] = w3;
        _wFinalOffset = 0xffff;
        ASSERT(!_is_branch_instruction());
#ifdef DEBUG
		_pDebug = 0;
#endif
    }
	scii(Opcode bOpcode, _code_pos branch, bool fUndetermined)
    {
        _opSize = Undefined;
        _wSize = 0;
        _fForceWord = false;
        _fUndetermined = fUndetermined;
        _bOpcode = bOpcode;
		ASSERT(_bOpcode <= Opcode::LastOne);
        _itOffset = branch;
        // Assume a backward branch for now...
        // (if branch is .end(), then we'll need to fix it up later anyhow, via set_branch_target)
        _fForwardBranch = false;
        _wFinalOffset = 0xffff;
#ifdef DEBUG
		_pDebug = 0;
#endif
    }

    WORD size();
    WORD calc_size(_code_pos self, int *pfNeedToRedo);
    void set_final_branch_operands(_code_pos self);
    void reset_size() { _wSize = 0; _opSize = Undefined; }

    void set_branch_target(_code_pos offset, bool fForward)
    {
        _itOffset = offset;
        _fForwardBranch = fForward;
        _fUndetermined = false;
        ASSERT(_is_label_instruction());
    }

    bool is_forward_branch()
    {
        return _fForwardBranch;
    }

    _code_pos get_branch_target()
    {
        ASSERT(_is_label_instruction());
        return _itOffset;
    }
    
    void absolute_offset(WORD wAbsolute)
    {
		ASSERT((_bOpcode == Opcode::LOFSS) || (_bOpcode == Opcode::LOFSA) || (_bOpcode == Opcode::CALL));
        // This is the absolute offset to a string or said (not yet the relative one, but it's progress)
        _wOperands[0] = wAbsolute;
        // When we actually write the instruction to a stream, we'll need to diff the current location
        // with this one, and write the relative offset.
    }

    void update_first_operand(WORD wValue)
    {
        _wOperands[0] = wValue;
    }
    void update_second_operand(WORD wValue)
    {
        _wOperands[1] = wValue;
    }
    WORD get_operand(int i)  const { ASSERT(OpArgTypes[static_cast<BYTE>(_bOpcode)][i] != otEMPTY); return _wOperands[i]; }
    WORD get_first_operand() const
    {
        return _wOperands[0];
    }
    WORD get_second_operand() const
    {
        return _wOperands[1];
    }
    WORD get_third_operand() const
    {
        return _wOperands[2];
    }

    void output_code(std::vector<BYTE>&);

    Opcode get_opcode() const { return _bOpcode; }
    Opcode set_opcode(Opcode opcode) { return _bOpcode = opcode; }

    // Get the final offset at which the instruction was written.
    WORD get_final_offset() const  { assert(_wFinalOffset != 0xffff); return _wFinalOffset; }
    WORD get_final_postop_offset() const { return get_final_offset() + _wSize; }

    WORD get_final_offset_dontcare() const  { return _wFinalOffset; }

    // Used for decompilation
    void set_offset_and_size(WORD wOffset, WORD wSize) { _wFinalOffset = wOffset; _wSize = wSize; }

#ifdef DEBUG
    void set_debug_info(int p) { _pDebug = p; }
#endif
    bool is_branch_determined() { return !_fUndetermined; }

    //
    // For decompilation
    // 
    int is_stackpop_op();
    bool is_stackpush_op();
    bool is_acc_op();
    void mark() { }
    bool is_marked() { return false; }

    bool _is_branch_instruction()
    {
        return (_bOpcode == Opcode::BNT) || (_bOpcode == Opcode::BT) || (_bOpcode == Opcode::JMP);
    }

    bool is_conditional_branch_instruction()
    {
        return (_bOpcode == Opcode::BNT) || (_bOpcode == Opcode::BT);
    }

private:
    bool _is_label_instruction()
    {
		return (_bOpcode == Opcode::BNT) || (_bOpcode == Opcode::BT) || (_bOpcode == Opcode::JMP) || (_bOpcode == Opcode::CALL);
    }

	OperandType *GetOperandTypes();
    WORD _wOperands[3];
    WORD _wSize;
    bool _fForceWord; // This instruction must be a WORD sized one on the next attempt at calculating size.
    bool _fUndetermined;
    bool _fForwardBranch;
    Opcode _bOpcode;
    int _pDebug;      // Extra info for debugging.

    _code_pos _itOffset;
    WORD _wFinalOffset;

    enum OPSIZE
    {
        Undefined = 0,
        Byte = 1,
        Word = 2,
    };
    OPSIZE _opSize;

    static WORD _get_instruction_size(Opcode bOpcode, OPSIZE opSize);
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
typedef std::unordered_map<int, fixup_todos> fixup_todos_map;

// Record keeping of code_pos's that need to be moved to the above "todo" list when we leave a branch block

typedef std::stack<fixup_todos> fixup_frames;
typedef std::unordered_map<int, fixup_frames> fixup_frames_map;

// Is a map of "this target" was pointed at by "these sources"
typedef std::multimap<code_pos, code_pos> code_pos_multimap;

class scicode
{
public:
    scicode() {}
    ~scicode()
    {
        ASSERT(_insertionPoints.empty());
    }

    // Write instructions to the stream
	void inst(Opcode bOpcode)
    {
        _insertInstruction(scii(bOpcode));
        _checkBranchResolution();
    }
	void inst(Opcode bOpcode, WORD w1)
    {
        _insertInstruction(scii(bOpcode, w1));
        _checkBranchResolution();
    }
	void inst(Opcode bOpcode, WORD w1, WORD w2)
    {
        _insertInstruction(scii(bOpcode, w1, w2));
        _checkBranchResolution();
    }
	void inst(Opcode bOpcode, WORD w1, WORD w2, WORD w3)
    {
        _insertInstruction(scii(bOpcode, w1, w2, w3));
        _checkBranchResolution();
    }
	bool inst(Opcode bOpcode, code_pos branch, int index = 0)
    {
        bool fUndetermined = (branch == get_undetermined());
        _insertInstruction(scii(bOpcode, branch, fUndetermined));
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
    void enter_branch_block(int index = 0);
    void leave_branch_block(int index = 0);
    bool in_branch_block(int index);

    WORD calc_size();
    WORD offset_of(code_pos target);
    void write_code(std::vector<BYTE> &output);
    bool has_dangling_branches(bool &fAllBranchesAreReturns);
    bool empty() { return _code.empty(); }
	void fixup_offsets(const std::unordered_map<WORD, WORD> &fixups);
    
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
};

// This craziness is so that code_pos can be used in a multimap.
// We use the node pointer as a < comparator.  Just something consistent but meaningless.
bool operator<(const code_pos &_Right, const code_pos &_Left);

