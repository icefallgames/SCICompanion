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
#include "Pic.h"
#include "PicCommands.h"
#include "PicOperations.h"
#include "PicDrawManager.h"
#include "AppState.h"
#include "ResourceEntity.h"
#include "DontShowAgainDialog.h"
#include "PaletteOperations.h"
#include "View.h"
#include "PicCommands.h"

using namespace std;

void _InsertCommands(PicComponent &pic, ptrdiff_t iStart, size_t cCount, PicCommand *pCommands)
{
    size_t iSize = pic.commands.size();
    if (iStart == -1)
    {
        iStart = iSize; // at the end
    }

    if ((iStart <= (ptrdiff_t)iSize) && (iStart >= 0))
    {
        //commands.insert(&commands[iStart], pCommands, pCommands + cCount);
        pic.commands.insert(pic.commands.begin() + iStart, pCommands, pCommands + cCount);
    }
}

// public operations

size_t GetCommandCount(const PicComponent &pic) { return pic.commands.size(); }

HBITMAP GetPicBitmap(PicScreen screen, const PicComponent &pic, const PaletteComponent *palette, int cx, int cy, SCIBitmapInfo *pbmi, uint8_t **pBitsDest)
{
    return PicDrawManager(&pic, palette).CreateBitmap(screen, PicPosition::Final, pic.Size, cx, cy, pbmi, pBitsDest);
}

EGACOLOR g_egaColorChooserPalette[] =
{
    { 0, 0 },
    { 1, 1 },
    { 2, 2 },
    { 3, 3 },
    { 4, 4 },
    { 5, 5 },
    { 6, 6 },
    { 7, 7 },
    { 8, 8 },
    { 9, 9 },
    { 10, 10 },
    { 11, 11 },
    { 12, 12 },
    { 13, 13 },
    { 14, 14 },
    { 15, 15 },
};

/////////////////////////////B//////G/////R///////
RGBQUAD g_egaColors[16] = { { 0x00, 0x00, 0x00, 0x01 },
    { 0xA0, 0x00, 0x00, 0x01 },
    { 0x00, 0xA0, 0x00, 0x01 },
    { 0xA0, 0xA0, 0x00, 0x01 },
    { 0x00, 0x00, 0xA0, 0x01 },
    { 0xA0, 0x00, 0xA0, 0x01 },
    { 0x00, 0x50, 0xA0, 0x01 },
    { 0xA0, 0xA0, 0xA0, 0x01 },
    { 0x50, 0x50, 0x50, 0x01 },
    { 0xff, 0x50, 0x50, 0x01 },
    { 0x50, 0xff, 0x00, 0x01 },
    { 0xff, 0xff, 0x50, 0x01 },
    { 0x50, 0x50, 0xff, 0x01 },
    { 0xff, 0x50, 0xff, 0x01 },
    { 0x50, 0xff, 0xff, 0x01 },
    { 0xff, 0xff, 0xff, 0x01 },
};

/////////////////////////////B//////G/////R///////
RGBQUAD g_egaColorsPlusOne[17] = { { 0x00, 0x00, 0x00, 0x01 },
{ 0xA0, 0x00, 0x00, 0x01 },
{ 0x00, 0xA0, 0x00, 0x01 },
{ 0xA0, 0xA0, 0x00, 0x01 },
{ 0x00, 0x00, 0xA0, 0x01 },
{ 0xA0, 0x00, 0xA0, 0x01 },
{ 0x00, 0x50, 0xA0, 0x01 },
{ 0xA0, 0xA0, 0xA0, 0x01 },
{ 0x50, 0x50, 0x50, 0x01 },
{ 0xff, 0x50, 0x50, 0x01 },
{ 0x50, 0xff, 0x00, 0x01 },
{ 0xff, 0xff, 0x50, 0x01 },
{ 0x50, 0x50, 0xff, 0x01 },
{ 0xff, 0x50, 0xff, 0x01 },
{ 0x50, 0xff, 0xff, 0x01 },
{ 0xff, 0xff, 0xff, 0x01 },
{ 0xff, 0x00, 0xff, 0x01 },
};

RGBQUAD g_egaColorsExtended[256];
RGBQUAD g_egaColorsMixed[256];

RGBQUAD g_continuousPriorityColors[256];

COLORREF g_egaColorsCR[16] = { RGB(0x00, 0x00, 0x00),
RGB(0x00, 0x00, 0xA0),
RGB(0x00, 0xA0, 0x00),
RGB(0x00, 0xA0, 0xA0),
RGB(0xA0, 0x00, 0x00),
RGB(0xA0, 0x00, 0xA0),
RGB(0xA0, 0x50, 0x00),
RGB(0xA0, 0xA0, 0xA0),
RGB(0x50, 0x50, 0x50),
RGB(0x50, 0x50, 0xff),
RGB(0x50, 0xff, 0x00),
RGB(0x50, 0xff, 0xff),
RGB(0xff, 0x50, 0x50),
RGB(0xff, 0x50, 0xff),
RGB(0xff, 0xff, 0x50),
RGB(0xff, 0xff, 0xff),
};


SCIBitmapInfo::SCIBitmapInfo(int cx, int cy, const RGBQUAD *pPalette, int count)
{
    bmiHeader.biSize = sizeof(bmiHeader);
    bmiHeader.biWidth = cx;
    bmiHeader.biHeight = cy;
    bmiHeader.biPlanes = 1;
    bmiHeader.biBitCount = 8;
    bmiHeader.biCompression = BI_RGB;
    bmiHeader.biSizeImage = 0; // 0 is ok for BI_RGB
    bmiHeader.biXPelsPerMeter = 0; // ???
    bmiHeader.biYPelsPerMeter = 0; // ??? 
    bmiHeader.biClrUsed = count;
    bmiHeader.biClrImportant = count;
    if (pPalette == nullptr)
    {
        pPalette = g_egaColors;
        count = 16;
    }
    CopyMemory(&bmiColors, pPalette, sizeof(*pPalette) * count);
}

SCIBitmapInfoEx::SCIBitmapInfoEx(int cx, int cy)
{
    bmiHeader.biSize = sizeof(bmiHeader);
    bmiHeader.biWidth = cx;
    bmiHeader.biHeight = cy;
    bmiHeader.biPlanes = 1;
    bmiHeader.biBitCount = 8;
    bmiHeader.biCompression = BI_RGB;
    bmiHeader.biSizeImage = 0; // 0 is ok for BI_RGB
    bmiHeader.biXPelsPerMeter = 0;
    bmiHeader.biYPelsPerMeter = 0;
    bmiHeader.biClrUsed = 256;
    bmiHeader.biClrImportant = 256;
    for (int i = 0; i < ARRAYSIZE(_colors); i += 16)
    {
        CopyMemory(&bmiColors + i, g_egaColors, sizeof(g_egaColors));
    }
}

bool _IsSet(PicCommand *pCommand)
{
    switch (pCommand->type)
    {
        case PicCommand::SetVisual:
        case PicCommand::SetPriority:
        case PicCommand::SetControl:
            return true;
    }
    return false;
}

BOOL _GetOppositeCommandType(PicCommand *pCommand, PicCommand::CommandType *pType)
{
    BOOL fRet = FALSE;
    switch (pCommand->type)
    {
    case PicCommand::SetVisual:
        *pType = PicCommand::DisableVisual;
        fRet = TRUE;
        break;
    case PicCommand::SetPriority:
        *pType = PicCommand::DisablePriority;
        fRet = TRUE;
        break;
    case PicCommand::SetControl:
        *pType = PicCommand::DisableControl;
        fRet = TRUE;
        break;
    case PicCommand::DisableVisual:
        *pType = PicCommand::SetVisual;
        fRet = TRUE;
        break;
    case PicCommand::DisablePriority:
        *pType = PicCommand::SetPriority;
        fRet = TRUE;
        break;
    case PicCommand::DisableControl:
        *pType = PicCommand::SetControl;
        fRet = TRUE;
        break;
    }
    return fRet;
}

ptrdiff_t InsertCommands(PicComponent &pic, ptrdiff_t iStart, size_t cCount, PicCommand *pCommands)
{
    if (iStart == -1)
    {
        iStart = pic.commands.size();
    }
    // Special case:
    // If we are inserting a toggle visual/priority/control, then if the previous command
    // is the opposite, just delete it.
    PicCommand::CommandType type;
    if ((cCount == 1) &&
        (iStart > 0) &&
        ((iStart - 1) < (int)pic.commands.size()) &&
        _GetOppositeCommandType(pCommands, &type) &&
        (pic.commands[iStart - 1].type == type))

    {
        // delete instead.
        RemoveCommand(pic, iStart - 1);
        ptrdiff_t delta = -1;
        if (_IsSet(pCommands))
        {
            // If this is a set though, we do need to insert, not just delete the prev.
            _InsertCommands(pic, iStart - 1, cCount, pCommands);
            delta++;
        }
        return delta;
    }
    else
    {
        _InsertCommands(pic, iStart, cCount, pCommands);
        return cCount;
    }
}

void RemoveCommand(PicComponent &pic, ptrdiff_t iCommandIndex)
{
    RemoveCommandRange(pic, iCommandIndex, iCommandIndex);
}

void RemoveCommandRange(PicComponent &pic, ptrdiff_t iStart, ptrdiff_t iEnd)
{
    if (iStart == -1)
    {
        iStart = pic.commands.size() - 1;
    }

    if (iEnd == -1)
    {
        iEnd = pic.commands.size() - 1;
    }

    if ((iStart >= 0) && (iStart < (ptrdiff_t)pic.commands.size()))
    {
        size_t iHowMany = iEnd - iStart + 1;
        pic.commands.erase(pic.commands.begin() + iStart, pic.commands.begin() + (iStart + iHowMany));
    }
}


bool _AllowCommand(bool supportsPen, bool isVGA, bool canChangePriLines, const PicCommand &command)
{
    if (!supportsPen && (command.type == PicCommand::CommandType::Pattern))
    {
        return false;
    }
    if (isVGA && ((command.type == PicCommand::CommandType::SetPalette) || (command.type == PicCommand::CommandType::SetPaletteEntry)))
    {
        return false;
    }
    if (canChangePriLines && (command.type == PicCommand::CommandType::SetPriorityBars))
    {
        return false;
    }
    return true;
}

void GetCommandsFromRaw(const uint8_t *bits, size_t cb, const PicComponent *picOptional, std::vector<PicCommand> &commands)
{
    bool supportsPen = !picOptional || (picOptional->Traits->SupportsPenCommands);
    bool isVGA = picOptional && (picOptional->Traits->IsVGA);
    bool canChangePriLines = !picOptional || (picOptional->Traits->CanChangePriorityLines);

    // Create a byte stream with this data.
    sci::ostream stream;
    stream.WriteBytes(bits, (int)cb);
    sci::istream reader = istream_from_ostream(stream);
    bool fOk = true;
    while (fOk)
    {
        PicCommand command;
        fOk = command.Initialize(reader);
        if (fOk && _AllowCommand(supportsPen, isVGA, canChangePriLines, command))
        {
            commands.push_back(command);
        }
    }
}

// If pic is provided, only pastes commands supported by that pic.
bool CopyRangeFromClipboard(HWND hwnd, const PicComponent *picOptional, std::vector<PicCommand> &commands)
{
    bool success = false;
    UINT uFormat = appState->GetCommandClipboardFormat();
    if (IsClipboardFormatAvailable(uFormat))
    {
        OpenClipboardGuard openClipboard(hwnd);
        if (openClipboard.IsOpen())
        {
            HGLOBAL hMem = GetClipboardData(uFormat);
            // We don't need to GlobalFree this.
            if (hMem)
            {
                size_t cb = GlobalSize(hMem);
                GlobalLockGuard<BYTE*> globalLock(hMem);
                BYTE *pBits = globalLock.Object;
                if (pBits)
                {
                    GetCommandsFromRaw(pBits, cb, picOptional, commands);
                    success = true;
                }
            }
        }
    }
    return success;
}

bool CopyRangeToClipboard(const std::vector<PicCommand> &commands, ptrdiff_t iStart, ptrdiff_t iEnd)
{
    bool success = false;
    if ((iStart >= 0) && (iEnd < (ptrdiff_t)commands.size()))
    {
        UINT uFormat = appState->GetCommandClipboardFormat();
        if (uFormat)
        {
            OpenClipboardGuard clipBoard(AfxGetMainWnd()->GetSafeHwnd());
            if (clipBoard.IsOpen())
            {
                HANDLE hClip = NULL;
                GlobalAllocGuard allocGuard(CopiedCommands_AllocAndFillMemory(&commands[0] + iStart, iEnd - iStart + 1));
                if (allocGuard.Global)
                {
                    // Empty the clipboard - otherwise, it could have both commands and a bitmap on it,
                    // and the piceditor will prefer the bitmap (tracing image).
                    EmptyClipboard();

                    hClip = SetClipboardData(uFormat, allocGuard.Global);
                    if (hClip)
                    {
                        // Clipboard has taken ownership
                        allocGuard.RelinquishOwnership();
                    }
                }
            }
        }
    }
    return success;
}

const PicCommand &GetCommandAt(const PicComponent &pic, ptrdiff_t iIndex)
{
    return pic.commands[iIndex];
}

// Interesting operations.
void Draw(const PicComponent &pic, PicData &data, ViewPort &state, ptrdiff_t iPos)
{
    Draw(pic, data, state, 0, iPos);
}

void Draw(const PicComponent &pic, PicData &data, ViewPort &state, ptrdiff_t iStart, ptrdiff_t iEnd)
{
    if (iEnd == -1)
    {
        iEnd = pic.commands.size();
    }
    iEnd = min((ptrdiff_t)pic.commands.size(), iEnd);
    for (ptrdiff_t i = iStart; i < iEnd; i++)
    {
        const PicCommand &command = pic.commands[i];
        command.Draw(&data, state);
    }
}

size_t GetLastChangedSpot(const PicComponent &pic, PicData &data, ViewPort &state, int x, int y)
{
    // Now process all our pic commands!
    uint8_t bColor = 0xf; // White, for visual.  Note - this won't work well for dithered colors.  We need to expand our radius
    size_t iLastTimeItChanged = -1; // Never changed.
    size_t iPos = pic.commands.size();
    for (size_t i = 0; i < iPos; i++)
    {
        pic.commands[i].Draw(&data, state);
        if (data.pdataVisual[BUFFEROFFSET_NONSTD(data.size.cx, data.size.cy, x, y)] != bColor)
        {
            iLastTimeItChanged = i;
            bColor = data.pdataVisual[BUFFEROFFSET_NONSTD(data.size.cx, data.size.cy, x, y)];
        }
    }
    return iLastTimeItChanged;
}
