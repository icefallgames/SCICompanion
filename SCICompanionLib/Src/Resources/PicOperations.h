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

#include "Pic.h"
struct PaletteComponent;
class PicCommand;

bool IsSame(const PicComponent &pic, const PicComponent *pPic);

//
// Caller needs to free HBITMAP
//
HBITMAP GetPicBitmap(PicScreen screen, const PicComponent &pic, const PaletteComponent *palette, int cx, int cy, SCIBitmapInfo *pbmi = nullptr, uint8_t **pBitsDest = nullptr);

// Insert/remove commands from pic
ptrdiff_t InsertCommands(PicComponent &pic, ptrdiff_t iStart, size_t cCount, PicCommand *pCommands);
void RemoveCommand(PicComponent &pic, ptrdiff_t iCommandIndex);
void RemoveCommandRange(PicComponent &pic, ptrdiff_t iStart, ptrdiff_t iEnd);
bool CopyRangeToClipboard(const std::vector<PicCommand> &commands, ptrdiff_t iStart, ptrdiff_t iEnd);
bool CopyRangeFromClipboard(HWND hwnd, const PicComponent *picOptional, std::vector<PicCommand> &commands);
void GetCommandsFromRaw(const uint8_t *bits, size_t cb, const PicComponent *picOptional, std::vector<PicCommand> &commands);

//void ResetResourceAndFileNumbers() { _iFileNumber = -1; _iResourceNumber = -1; }
size_t GetCommandCount(const PicComponent &pic);
const PicCommand &GetCommandAt(const PicComponent &pic, ptrdiff_t iIndex);

// Interesting operations.
void Draw(const PicComponent &pic, PicData &data, ViewPort &state, ptrdiff_t iPos);
void Draw(const PicComponent &pic, PicData &data, ViewPort &state, ptrdiff_t iStart, ptrdiff_t iEnd);
size_t GetLastChangedSpot(const PicComponent &pic, PicData &data, ViewPort &state, int x, int y);
