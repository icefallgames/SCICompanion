#pragma once

#include "Pic.h"
struct PaletteComponent;

bool IsSame(const PicComponent &pic, const PicComponent *pPic);

//
// Caller needs to free HBITMAP
//
HBITMAP GetPicBitmap(PicScreen screen, const PicComponent &pic, const PaletteComponent *palette, int cx, int cy, SCIBitmapInfo *pbmi = nullptr, uint8_t **pBitsDest = nullptr);

// Insert/remove commands from pic
ptrdiff_t InsertCommands(PicComponent &pic, ptrdiff_t iStart, size_t cCount, PicCommand *pCommands);
void RemoveCommand(PicComponent &pic, ptrdiff_t iCommandIndex);
void RemoveCommandRange(PicComponent &pic, ptrdiff_t iStart, ptrdiff_t iEnd);
HRESULT CopyRangeToClipboard(const PicComponent &pic, ptrdiff_t iStart, ptrdiff_t iEnd);

//void ResetResourceAndFileNumbers() { _iFileNumber = -1; _iResourceNumber = -1; }
size_t GetCommandCount(const PicComponent &pic);
const PicCommand &GetCommandAt(const PicComponent &pic, ptrdiff_t iIndex);

// Interesting operations.
void Draw(const PicComponent &pic, PicData &data, ViewPort &state, ptrdiff_t iPos);
void Draw(const PicComponent &pic, PicData &data, ViewPort &state, ptrdiff_t iStart, ptrdiff_t iEnd);
size_t GetLastChangedSpot(const PicComponent &pic, PicData &data, ViewPort &state, int x, int y);
