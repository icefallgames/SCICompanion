#pragma once

#include "Types.h"

struct CodeResult
{
public:
	CodeResult(WORD wBytes, SpeciesIndex wType) : _wBytes(wBytes), _type(wType) {}
	CodeResult(WORD wBytes) : _wBytes(wBytes), _type(DataTypeNone) {}
	CodeResult() : _wBytes(0), _type(DataTypeNone) {}
	WORD GetBytes() const { return _wBytes; }
	SpeciesIndex GetType() const { return _type; }
private:
	WORD _wBytes;           // Number of bytes pushed onto the stack
	SpeciesIndex _type;    // Data type
};

namespace sci
{
	enum class ValueType
	{
		String = 0x00000000,
		Said = 0x00000001,
		Number = 0x00000002,
		Token = 0x00000003,
		Selector = 0x00000004,
		Pointer = 0x00000005,
		None = 0x00000006,
		ResourceString = 0x00000007, // Not available in SCIStudio syntax
	};

}