#pragma once

#define TOTAL_KERNELS 				    114 // SCI0 only

#define TOTAL_OPCODES 				    128

#define TOTAL_SAID_ARGS 10

struct SaidToken
{
    char Character;
    BYTE Code;
};

extern SaidToken SaidArgs[TOTAL_SAID_ARGS];

struct KernelInfo
{
	char *Ret,*Name,*Params;
    const char *GetName() { return Name; }
};

extern KernelInfo KrnlInfo[TOTAL_KERNELS];

extern BYTE OpArgs[TOTAL_OPCODES*2];

extern char *OpcodeNames[];

enum OperandType
{
    otEMPTY =     0,
    otINT =       1,
    otVAR =       2,
    otPVAR =      3,
    otCLASS =     4,
    otPROP =      5,
    otSTRING =    6,
    otSAID =      7,
    otKERNEL =    8,
    otLABEL =     9,
    otPUBPROC =   10,
    otINT8 =      11,
    otINT16 =     12,
    otOFFS =      13,
    otUINT =      14,
};
extern OperandType OpArgTypes[TOTAL_OPCODES][3];

enum class Opcode : BYTE
{
	// Opcodes
	BNOT = 0,
	ADD = 1,
	SUB = 2,
	MUL = 3,
	DIV = 4,
	MOD = 5,
	SHR = 6,
	SHL = 7,
	XOR = 8,
	AND = 9,
	OR = 10,
	NEG = 11,
	NOT = 12,
	EQ = 13,
	NE = 14,
	GT = 15,
	GE = 16,
	LT = 17,
	LE = 18,
	UGT = 19,
	UGE = 20,
	ULT = 21,
	ULE = 22,
	BT = 23,      // branch on true
	BNT = 24,      // branch on not true
	JMP = 25,      // jump
	LDI = 26,      // load immediate data into accumulator
	PUSH = 27,      // push accumulator onto stack (1 byte)
	PUSHI = 28,          // push immediate onto stack
	TOSS = 29,      // get rid of top of stack
	DUP = 30,      // dupe stack top of stack
	LINK = 31,      // add n frames to stack
	CALL = 32,      // call local proc
	CALLK = 33,          // call kernel
	CALLB = 34,          // call public proc in main script
	CALLE = 35,          // call public proc in external script
	RET = 36,      // return (value goes in acc)
	SEND = 37,      // send
	CLASS = 40,          // load address of class # to accumulator
	SELF = 42,      // send to self
	SUPER = 43,          // send to super
	REST = 44,
	LEA = 45,  // load  address of a variable into the acc
	SELFID = 46,  // puts address of self into acc
	INDETERMINATE = 47,  // NOT A VALID OPCODE
	PPREV = 48,
	PTOA = 49,      // property index to acc
	ATOP = 50,      // acc to property index
	PTOS = 51,      // property index to stack
	STOP = 52,      // Stack to property index
	IPTOA = 53,          // Inc prop to acc
	DPTOA = 54,          // Dec prop to acc
	IPTOS = 55,          // Inc prop to stack
	DPTOS = 56,          // Dec prop to stack
	LOFSA = 57,      // Load offset (from pc) onto acc
	LOFSS = 58,      // Load offset (from pc) onto stack
	PUSH0 = 59,      // push 0 onto stack
	PUSH1 = 60,      // push 1 onto stack
	PUSH2 = 61,      // push 2 onto stack
	PUSHSELF = 62,      // push self onto stack
	LAG = 64,  // load global to acc
	LAL = 65,
	LAT = 66,
	LAP = 67,
	LSG = 68,  // load global to stack
	LSL = 69,
	LST = 70,
	LSP = 71,
	LAGI = 72,  // index global and load to acc
	LALI = 73,
	LATI = 74,
	LAPI = 75,
	LSGI = 76,  // index global and load to stack
	LSLI = 77,
	LSTI = 78,
	LSPI = 79,
	SAG = 80,  // store acc in global
	SAL = 81,
	SAT = 82,
	SAP = 83,
	SSG = 84,  // store stack in global
	SSL = 85,
	SST = 86,
	SSP = 87,
	SAGI = 88,  // store acc in global and index?
	SALI = 89,
	SATI = 90,
	SAPI = 91,
	SSGI = 92,  // store stack in global and index?
	SSLI = 93,
	SSTI = 94,
	SSPI = 95,
	pAG = 96,
	pAL = 97,
	pAT = 98,
	pAP = 99,
	pSG = 100,
	pSL = 101,
	pST = 102,
	pSP = 103,
	pAGI = 104,
	pALI = 105,
	pATI = 106,
	pAPI = 107,
	pSGI = 108,
	pSLI = 109,
	pSTI = 110,
	pSPI = 111,
	nAG = 112,
	nAL = 113,
	nAT = 114,
	nAP = 115,
	nSG = 116,
	nSL = 117,
	nST = 118,
	nSP = 119,
	nAGI = 120,
	nALI = 121,
	nATI = 122,
	nAPI = 123,
	nSGI = 124,
	nSLI = 125,
	nSTI = 126,
	nSPI = 127,
	LastOne = 127,
};



// Variable operands
// bit "-1" is operand size
// bit 0,1 -> type of variable
#define VO_GLOBAL           0x00
#define VO_LOCAL            0x01
#define VO_TEMP             0x02
#define VO_PARAM            0x03
// bit 2
#define VO_STACK            0x04
#define VO_ACC              0x00
// bit 3
#define VO_ACC_AS_INDEX_MOD 0x08
// bit 4,5
#define VO_LOAD             0x00 // -> to acc or stack
#define VO_STORE            0x10 // -> in variable
#define VO_INC_AND_LOAD     0x20
#define VO_DEC_AND_LOAD     0x30

// The lea instruction (load effective address into acc)
// bits 0-1  (actually bits 1-2)
#define LEA_GLOBAL           0x00 // REVIEW -> make them static const BYTE or something.
#define LEA_LOCAL            0x01
#define LEA_TEMP             0x02
#define LEA_PARAM            0x03
#define LEA_VARIABLEMASK     0x03
// bit 3     (actually bit 4)
#define LEA_ACC_AS_INDEX_MOD 0x08

