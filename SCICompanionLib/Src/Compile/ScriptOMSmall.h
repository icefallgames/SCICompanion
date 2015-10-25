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