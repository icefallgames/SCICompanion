#pragma once

#include "Codec.h"

// From the scumm engine
// GNU GPL Version 2 or later.

void reorderView(byte *src, BoundsCheckedArray<BYTE> dest);
void reorderPic(byte *src, byte *dest, int dsize);