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

