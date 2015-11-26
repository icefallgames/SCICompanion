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
#include "ParserCommon.h"

// DOS alt key codes are arranged according to the QWERTY keyboard
const int AltKeys[26] = {
    30, 48, 46, 32, 18, 33, 34, 35, 23,		// a - i
    36, 37, 38, 50, 49, 24, 25, 16, 19,		// j - r
    31, 20, 22, 47, 17, 45, 21, 44			//  s - z
};

// Returns the value of a numeric of hex digit.
// ch is assumed to be 0-9,a-f,A-F
int charToI(char ch)
{
    if (ch >= '0' && ch <= '9')
    {
        return (int)(ch - '0');
    }
    // Assume it's hex if we got here
    ASSERT((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'));
    if (ch >= 'a')
    {
        return (ch - 'a' + 10);
    }
    else
    {
        return (ch - 'A' + 10);
    }
}

