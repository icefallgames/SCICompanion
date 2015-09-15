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

