
#include "stdafx.h"
#include <math.h>
#include "ResourceEntity.h"
#include "Components.h"
#include "RasterOperations.h"
#include "View.h"
#include <limits>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// As long as these values are unsigned, there's no need to check for 0
#define CHECK_RECT(cx, cy, x,y)\
    ( x < cx && y < cy )

#define BYTE_FROM_PALETTE_AND_OFFSET(num, offset)   ((uint8_t) (((num) * PALETTE_SIZE) + (offset)))
#define BYTE_FROM_EGACOLOR(color)                   ((uint8_t) ((color).color1 + ((color).color2 << 4)))

#define GET_PALETTE(palettes, number) ((palettes) + (number) * PALETTE_SIZE)

//
// This file contains code that is based on the ditherfill, pen and line-drawing code from SCISTudio3,
// by Brian Provinciano.
//

extern const EGACOLOR *g_defaultPalette = (EGACOLOR*)g_defaultPaletteInit;
extern SIZE_T cbDefaultPalette = sizeof(g_defaultPaletteInit);

extern uint8_t g_defaultPriBands[14] = { 43, 53, 64, 74, 85, 96, 106, 117, 127, 138, 149, 159, 170, 180 };
uint16_t g_defaultPriBands16Bit[14] = { 43, 53, 64, 74, 85, 96, 106, 117, 127, 138, 149, 159, 170, 180 };

//
// Given a y-coordinate, returns the priority at that position.
//
uint8_t PriorityFromY(uint16_t y, const ViewPort &picState)
{
    uint8_t bPriority = 0;
    for (bPriority = 0; bPriority < ARRAYSIZE(picState.bPriorityLines); bPriority++)
    {
        if (y < picState.bPriorityLines[bPriority])
            break;
    }
    return bPriority;
}

//
// Helpers for converting an arbitrary index to and from pattern info.
// The arbitrary index is used for the position of a square in a dialog that
// represents the different types of patterns.
//
void PatternInfoFromIndex(uint8_t bIndex, PenStyle *pPenStyle)
{
    // Pattern size resets every 8.
    pPenStyle->bPatternSize = bIndex % 8;
    // fPattern alterates every 8, from solid->pattern->solid->pattern
    pPenStyle->fPattern = ((bIndex / 8) % 2) != 0;
    pPenStyle->bPatternNR = 0;
    // fRectangle is true for >= 16
    pPenStyle->fRectangle = (bIndex / 16) != 0;
}

uint8_t IndexFromPatternInfo(const PenStyle *pPenStyle)
{
    uint8_t bIndex = 0;
    if (pPenStyle->fRectangle)
    {
        bIndex += 16;
    }
    if (pPenStyle->fPattern)
    {
        bIndex += 8;
    }
    bIndex += pPenStyle->bPatternSize;
    return bIndex;
}

uint8_t c_SizeToOpcodeLine[] = { PIC_OP_RELATIVE_SHORT_LINES, PIC_OP_RELATIVE_MEDIUM_LINES, PIC_OP_RELATIVE_LONG_LINES };
uint8_t c_SizeToOpcodePen[] =  { PIC_OP_RELATIVE_PATTERNS, PIC_OP_RELATIVE_MEDIUM_PATTERNS, PIC_OP_ABSOLUTE_PATTERNS };

uint8_t OpcodeFromCommandTypeAndSize(uint8_t bType, DRAWSIZE size)
{
    if (bType == PicCommand::Line)
    {
        return c_SizeToOpcodeLine[size];
    }
    else if (bType == PicCommand::Pattern)
    {
        return c_SizeToOpcodePen[size];
    }
    assert(false);
    return PIC_END;
}

DRAWSIZE DrawSizeFromCoords(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    DRAWSIZE dsReturn = DS_LARGE;
    __int16 dx = abs(((__int16)x1) - (__int16)x2);
    __int16 dy = abs(((__int16)y1) - (__int16)y2);

    if ((dx <= 6) && (dy <= 7))  // This keeps the byte below 0xf0
    {
        // Each coord is within 6 or 7 pixels of the previous
        dsReturn = DS_SMALL;
    }
    else if ((dx <= 0x7f) && (dy <= 0x6f))
    {
        // x is within 127 pixels (128 is ok for +ve, but who cares) and y is within 111
        dsReturn = DS_MEDIUM;
    }
    return dsReturn;
}



//
// Looks at the x,y of the next command, and sees which of three categories it falls
// into.  If the command is of a different type, it returns DS_LARGE.
//
DRAWSIZE DrawSizeFromNextCommand(PicCommand *pCommand, PicCommand *pNextCommand)
{
    DRAWSIZE dsReturn = DS_LARGE;
    if ((pCommand->type == PicCommand::Line) && (pNextCommand->type == PicCommand::Line) &&
        (pCommand->drawLine.xTo == pNextCommand->drawLine.xFrom) &&
        (pCommand->drawLine.yTo == pNextCommand->drawLine.yFrom))
    {
        dsReturn = DrawSizeFromCoords(pNextCommand->drawLine.xFrom,
                                      pNextCommand->drawLine.yFrom,
                                      pNextCommand->drawLine.xTo,
                                      pNextCommand->drawLine.yTo);
    }
    else if ((pCommand->type == PicCommand::Pattern) && (pNextCommand->type == PicCommand::Pattern))
    {
        dsReturn = DrawSizeFromCoords(pCommand->drawPattern.x,
                                      pCommand->drawPattern.y,
                                      pNextCommand->drawPattern.x,
                                      pNextCommand->drawPattern.y);
    }
    return dsReturn;
}




//
// Get name functions
//
void LineCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf);
void PatternCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf);
void FillCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf);
void SetVisualCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf);
void SetPriorityCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf);
void SetControlCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf);
void DisableVisualCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf);
void DisablePriorityCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf);
void DisableControlCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf);
void SetPaletteCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf);
void SetPaletteEntryCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf);
void SetPriorityBarsCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf);
void DrawVisualBitmap_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf);
void CircleCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf);

DRAW_FUNCTION g_DrawFunctions[] =
{
    LineCommand_Draw,
    PatternCommand_Draw,
    FillCommand_Draw,
    SetVisualCommand_Draw,
    SetPriorityCommand_Draw,
    SetControlCommand_Draw,
    DisableVisualCommand_Draw,
    DisablePriorityCommand_Draw,
    DisableControlCommand_Draw,
    SetPaletteCommand_Draw,
    SetPaletteEntryCommand_Draw,
    SetPriorityBarsCommand_Draw,
    DrawVisualBitmap_Draw,
    CircleCommand_Draw,
};





void _WriteColor(sci::ostream *pSerial, EGACOLOR color)
{
    pSerial->WriteByte(color.color1 | (color.color2 << 4));
}

void _WriteAbsCoordinate(sci::ostream *pSerial, uint16_t x, uint16_t y)
{
    // Coordinate prefix | x / 256 | y / 256
    // Coord prefix byte is xxxxyyyy, where xxxx = x % 256, yyyy = y % 256.
    pSerial->WriteByte((uint8_t) ( ((x >> 4) & 0x00f0) | (y >> 8))); // It is redundant to & y with 0x000f
    pSerial->WriteByte((uint8_t) (x & 0x00ff));
    pSerial->WriteByte((uint8_t) (y & 0x00ff));
}

void _WriteMediumCoordinate(sci::ostream *pSerial, uint16_t xOld, uint16_t yOld, uint16_t x, uint16_t y)
{
    // x | y
    uint8_t byte;
    if (y < yOld)
    {
        byte = 0x80 | (uint8_t)(yOld - y);
    }
    else
    {
        byte = (uint8_t)(y - yOld);
    }
    assert(byte < 0xf0); // Since 0xf0 signifies the next opcode.
    pSerial->WriteByte(byte);

    byte = (uint8_t)(((__int16)x) - (__int16)xOld);
    pSerial->WriteByte(byte);
}

void _WriteSmallCoordinate(sci::ostream *pSerial, uint16_t xOld, uint16_t yOld, uint16_t x, uint16_t y)
{
    // x,y
    // where xxxxyyyy, and leftmost bits of each indicate -ve or +ve
    uint8_t byte = 0;
    if (x < xOld)
    {
        byte |= (((xOld - x) << 4) | 0x80);
    }
    else
    {
        byte |= ((x - xOld) << 4);
    }
    if (y < yOld)
    {
        byte |= ((yOld - y) | 0x08);
    }
    else
    {
        byte |= (y - yOld);
    }

    pSerial->WriteByte(byte);
}

void _WriteCoordinate(sci::ostream *pSerial, DRAWSIZE ds, uint16_t xOld, uint16_t yOld, uint16_t x, uint16_t y)
{
    switch (ds)
    {
    case DS_SMALL:
        _WriteSmallCoordinate(pSerial, xOld, yOld, x, y);
        break;
    case DS_MEDIUM:
        _WriteMediumCoordinate(pSerial, xOld, yOld, x, y);
        break;
    case DS_LARGE:
        _WriteAbsCoordinate(pSerial, x, y); // Don't need the old coord
        break;
    }
    assert("Error writing coordinate: invalid drawsize");
}


SERIALIZE_FUNCTION g_SerializeFunctions[] =
{
    LineCommand_Serialize,
    PatternCommand_Serialize,
    FillCommand_Serialize,
    SetVisualCommand_Serialize,
    SetPriorityCommand_Serialize,
    SetControlCommand_Serialize,
    DisableVisualCommand_Serialize,
    DisablePriorityCommand_Serialize,
    DisableControlCommand_Serialize,
    SetPaletteCommand_Serialize,
    SetPaletteEntryCommand_Serialize,
    SetPriorityBarsCommand_Serialize,
    DrawVisualBitmap_Serialize,
    CircleCommand_Serialize,
};

GETNAME_FUNCTION g_GetNameFunctions[] =
{
    LineCommand_GetName,
    PatternCommand_GetName,
    FillCommand_GetName,
    SetVisualCommand_GetName,
    SetPriorityCommand_GetName,
    SetControlCommand_GetName,
    DisableVisualCommand_GetName,
    DisablePriorityCommand_GetName,
    DisableControlCommand_GetName,
    SetPaletteCommand_GetName,
    SetPaletteEntryCommand_GetName,
    SetPriorityBarsCommand_GetName,
    DrawVisualBitmap_GetName,
    CircleCommand_GetName,
};

static_assert(ARRAYSIZE(g_GetNameFunctions) == ARRAYSIZE(g_SerializeFunctions), "Different number of PicCommand functions");
static_assert(ARRAYSIZE(g_GetNameFunctions) == ARRAYSIZE(g_DrawFunctions), "Different number of PicCommand functions");

void PicCommand::Draw(PicData *pData, ViewPort &state) const
{
    g_DrawFunctions[type](this, pData, &state);
}


void PicCommand::GetName(TCHAR *pszBuf, size_t cchBuf) const
{
    g_GetNameFunctions[type](this, pszBuf, cchBuf);
}

void Command_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState)
{
    g_SerializeFunctions[pCommand->type](pSerial, pCommand, pCommandPrev, pCommandNext, dsPrev, pds, pState);
}

//
// Given a bunch of pasted PICCOMMANDs', figure out the visual bounding rect that contains all the commands.
//
void PastedCommands_GetBounds(size16 displaySize, const PicCommand *pCommand, size_t cCommands, sRECT *prc)
{
    prc->left = displaySize.cx - 1;
    prc->right = 0;
    prc->top = displaySize.cy - 1;
    prc->bottom = 0;

    const PicCommand *pCurrent = pCommand;
    for (size_t i = 0; i < cCommands; i++)
    {
        switch (pCurrent->type)
        {
        case PicCommand::Line:
            prc->left = min(prc->left, pCurrent->drawLine.xFrom);
            prc->left = min(prc->left, pCurrent->drawLine.xTo);
            prc->right = max(prc->right, pCurrent->drawLine.xFrom);
            prc->right = max(prc->right, pCurrent->drawLine.xTo);
            prc->top = min(prc->top, pCurrent->drawLine.yFrom);
            prc->top = min(prc->top, pCurrent->drawLine.yTo);
            prc->bottom = max(prc->bottom, pCurrent->drawLine.yFrom);
            prc->bottom = max(prc->bottom, pCurrent->drawLine.yTo);
            break;

        case PicCommand::Pattern:
            prc->left = min(prc->left, pCurrent->drawPattern.x);
            prc->right = max(prc->right, pCurrent->drawPattern.x);
            prc->top = min(prc->top, pCurrent->drawPattern.y);
            prc->bottom = max(prc->bottom, pCurrent->drawPattern.y);
            break;

        case PicCommand::Fill:
            prc->left = min(prc->left, pCurrent->fill.x);
            prc->right = max(prc->right, pCurrent->fill.x);
            prc->top = min(prc->top, pCurrent->fill.y);
            prc->bottom = max(prc->bottom, pCurrent->fill.y);
            break;
        case PicCommand::Circle:
            prc->left = min(prc->left, pCurrent->circle.xFrom);
            prc->left = min(prc->left, pCurrent->circle.xTo);
            prc->right = max(prc->right, pCurrent->circle.xFrom);
            prc->right = max(prc->right, pCurrent->circle.xTo);
            prc->top = min(prc->top, pCurrent->circle.yFrom);
            prc->top = min(prc->top, pCurrent->circle.yTo);
            prc->bottom = max(prc->bottom, pCurrent->circle.yFrom);
            prc->bottom = max(prc->bottom, pCurrent->circle.yTo);
        default:
            // Not a coordinate command
            break;
        }
        pCurrent++;
    }

    // The rect bottom and right need to be "off the edge"
    prc->bottom += 1;
    prc->right += 1;

    // TODO: Give it a min size.
}


bool PastedCommands_ContainDrawCommands(const PicCommand *pCommands, size_t cCommands)
{
    bool fContain = FALSE;
    for (size_t i = 0; (!fContain) && (i < cCommands); i++)
    {
        switch (pCommands[i].type)
        {
        case PicCommand::Line:
        case PicCommand::Pattern:
        case PicCommand::Fill:
        case PicCommand::Circle:
            fContain = TRUE;
            break;
        }
    }
    return fContain;
}

//
// Returns false if the coordiante is out of bounds.
//
bool Coord_Adjust(size16 size, uint16_t *px, uint16_t *py, const PICCOMMAND_ADJUST *pAdjust)
{
    int x = (int)*px;
    int y = (int)*py;

    // Flip.
    if (pAdjust->fHFlip)
    {
        x = pAdjust->rcBounds.right - (x - pAdjust->rcBounds.left); 
    }
    if (pAdjust->fVFlip)
    {
        y = pAdjust->rcBounds.bottom - (y - pAdjust->rcBounds.top); 
    }

    int dx = x - pAdjust->rcBounds.left;
    int dy = y - pAdjust->rcBounds.top;
    if (RECTWIDTH(pAdjust->rcBounds) > 0)
    {
        dx = dx * RECTWIDTH(pAdjust->rcNew) / RECTWIDTH(pAdjust->rcBounds);
    }
    x = dx + pAdjust->rcNew.left;
    if (RECTHEIGHT(pAdjust->rcBounds) > 0)
    {
        dy = dy * RECTHEIGHT(pAdjust->rcNew) / RECTHEIGHT(pAdjust->rcBounds);
    }
    y = dy + pAdjust->rcNew.top;

    // Now rotate them.
    if (pAdjust->iAngle != 0)
    {
        // TODO: 
        // 1) Find the rotation center, based on the stretching done above (rcNew/rcBounds)
        double xCenter = (double)((pAdjust->rcNew.right + pAdjust->rcNew.left) / 2);
        double yCenter = (double)((pAdjust->rcNew.bottom + pAdjust->rcNew.top) / 2);
        double xOrig = ((double)x) - xCenter;
        double yOrig = ((double)y) - yCenter;
        double fAngle = ((double)pAdjust->iAngle) / 360.0 * 6.28;
        double xNew = (cos(fAngle) * xOrig - sin(fAngle) * yOrig) + xCenter;
        double yNew = (sin(fAngle) * xOrig + cos(fAngle) * yOrig) + yCenter;
        x = (int)xNew;
        y = (int)yNew;
    }

    bool fRet = ((x >= 0) && (y >= 0) && (x < size.cx) && (y < size.cy));

    // Truncation/overflow is ok, since we've checked the bounds above.
    *px = (uint16_t)x;
    *py = (uint16_t)y;

    return fRet;
}

//
// Returns FALSE if the command is out of bounds.
//
bool Command_Adjust(size16 size, PicCommand *pCommand, const PICCOMMAND_ADJUST *pAdjust)
{
    bool fRet = TRUE;
    __int16 dx = (__int16)(pAdjust->rcNew.left - pAdjust->rcBounds.left);
    __int16 dy = (__int16)(pAdjust->rcNew.top - pAdjust->rcBounds.top);
    switch (pCommand->type)
    {
    case PicCommand::Line:
        // REVIEW: could clip the line instead of completely removing it, when it goes out of bounds.
        // Would need to use bresenham algorithm.
        fRet = Coord_Adjust(size, &pCommand->drawLine.xFrom, &pCommand->drawLine.yFrom, pAdjust);
        if (fRet)
        {
            fRet = Coord_Adjust(size, &pCommand->drawLine.xTo, &pCommand->drawLine.yTo, pAdjust);
        }
        break;
    case PicCommand::Circle:
        // REVIEW: could clip the line instead of completely removing it, when it goes out of bounds.
        // Would need to use bresenham algorithm.
        fRet = Coord_Adjust(size, &pCommand->circle.xFrom, &pCommand->circle.yFrom, pAdjust);
        if (fRet)
        {
            fRet = Coord_Adjust(size, &pCommand->circle.xTo, &pCommand->circle.yTo, pAdjust);
        }
        break;
    case PicCommand::Pattern:
        fRet = Coord_Adjust(size, &pCommand->drawPattern.x, &pCommand->drawPattern.y, pAdjust);
        break;

    case PicCommand::Fill:
        fRet = Coord_Adjust(size, &pCommand->fill.x, &pCommand->fill.y, pAdjust);
        break;
    }
    return fRet;
}

//
// Adjusts the commands by the offset.
// Commands that are out of bounds are removed from pCommandsIn.  The new number of commands is *pcCommandsOut.
//
void PastedCommands_Adjust(size16 size, std::vector<PicCommand> &commandsIn, const PICCOMMAND_ADJUST *pAdjust)
{
    for (size_t i = 0; i < commandsIn.size(); i++)
    {
        if (Command_Adjust(size, &commandsIn[i], pAdjust))
        {
            // We're good
        }
        else
        {
            // This was out of bounds - remove it.
            //commandsIn.erase(&commandsIn[i]);
			commandsIn.erase(commandsIn.begin() + i);
        }
    }
}

HGLOBAL CopiedCommands_AllocAndFillMemory(const PicCommand *pCommands, size_t cCommands)
{
    sci::ostream serial;
    for (size_t i = 0; i < cCommands; i++)
    {
        pCommands->SerializeForClipboard(&serial);
        pCommands++; // Go to next command
    }

    GlobalAllocGuard globalAlloc(GMEM_MOVEABLE, serial.tellp());
    if (globalAlloc.Global)
    {
        GlobalLockGuard<uint8_t*> globalLock(globalAlloc);
        if (globalLock.Object)
        {
            memcpy(globalLock.Object, serial.GetInternalPointer(), serial.tellp());
            globalLock.Unlock();
        }
        else
        {
            // Didn't work
            globalAlloc.Free();
        }
    }
    return globalAlloc.RelinquishOwnership();
}


bool PtInSRect(sRECT *prc, uint16_t x, uint16_t y)
{
    return ( (((__int16)x) >= prc->left) && (((__int16)x) < prc->right) && (((__int16)y) >= prc->top) && (((__int16)y) < prc->bottom));
}

void Command_DrawWithOffset(const PicCommand *pCommandIn, PicData *pData, ViewPort *pState, const PICCOMMAND_ADJUST *pAdjust)
{
    PicCommand commandAdjusted = *pCommandIn; // class copy, since we don't want to modify pCommandIn (should be const).
    if (Command_Adjust(pData->size, &commandAdjusted, pAdjust))
    {
        g_DrawFunctions[pCommandIn->type](&commandAdjusted, pData, pState);
    }
}


//
// Writes the picture out to a file
// 
void SerializeAllCommands(sci::ostream *pSerial, const std::vector<PicCommand> &commands, size_t cCommands)
{
    PicCommand commandPrev;
    DRAWSIZE ds = DS_SMALL; // Initial value doesn't matter.
    SerializedPicState picState = { 0 };
    for (size_t i = 0; i < cCommands; i++)
    {
        const PicCommand &command = commands[i];
        Command_Serialize(pSerial, &command, (i == 0) ? nullptr : &commandPrev, (i == (cCommands - 1)) ? nullptr : &commands[i + 1], ds, &ds, &picState);
        commandPrev = command;
    }
    
    // Terminate the command stream.
    pSerial->WriteByte(PIC_END);
}

ViewPort::ViewPort(uint8_t bPaletteToUse)
{
    Reset(bPaletteToUse);
}

ViewPort::ViewPort()
{
    ViewPort(static_cast<uint8_t>(0));
}

void ViewPort::Reset(uint8_t bPaletteToUse)
{
    dwDrawEnable = PicScreenFlags::None;
    bPriorityValue = 0;
    bControlValue = 0;
    egaColor.color1 = 0;
    egaColor.color2 = 0;
    bPaletteNumber = 0;
    bPaletteOffset = 0;
    bPatternNR = 0;
    bPatternSize = 0;
    bPaletteToDraw = bPaletteToUse;

    // Set all palettes to default:
    memcpy(GET_PALETTE(pPalettes, 0), g_defaultPalette, cbDefaultPalette);
    memcpy(GET_PALETTE(pPalettes, 1), g_defaultPalette, cbDefaultPalette);
    memcpy(GET_PALETTE(pPalettes, 2), g_defaultPalette, cbDefaultPalette);
    memcpy(GET_PALETTE(pPalettes, 3), g_defaultPalette, cbDefaultPalette);

    // The locked colours.
    memset(rgLocked, 0, sizeof(rgLocked));

    std::copy(g_defaultPriBands, g_defaultPriBands + ARRAYSIZE(g_defaultPriBands), bPriorityLines);
}

inline void _PlotPixI(int p, PicData *pData, uint16_t x, uint16_t y, PicScreenFlags dwDrawEnable, EGACOLOR color, uint8_t bPriorityValue, uint8_t bControlValue)
{
    if (IsFlagSet(pData->dwMapsToRedraw, PicScreenFlags::Visual) && IsFlagSet(dwDrawEnable, PicScreenFlags::Visual))
    {
        pData->pdataVisual[p] = ((x^y) & 1)? color.color1 : color.color2;
    }
    if (IsFlagSet(pData->dwMapsToRedraw, PicScreenFlags::Priority) && IsFlagSet(dwDrawEnable, PicScreenFlags::Priority))
    {
        pData->pdataPriority[p] = bPriorityValue;
    }
    if (IsFlagSet(pData->dwMapsToRedraw, PicScreenFlags::Control) && IsFlagSet(dwDrawEnable, PicScreenFlags::Control))
    {
        pData->pdataControl[p] = bControlValue;
    }
}

struct PlotEGA
{
    static uint8_t Plot(uint16_t x, uint16_t y, EGACOLOR color)
    {
        return ((x^y) & 1) ? color.color1 : color.color2;
    }

    // Guard against someone doing a fill with pure white, since this algorithm will hang in that case.
    // Hero's quest does this, when some pictures are drawn with some palettes.
    static bool EarlyBail(PicScreenFlags dwDrawEnable, EGACOLOR color)
    {
        return (IsFlagSet(dwDrawEnable, PicScreenFlags::Visual) && ((color.color1 == 0xf) && (color.color2 == 0xf)));
    }

    static const uint8_t White = 0xf;

    typedef EGACOLOR PixelType;
};

struct PlotVGA
{
    static uint8_t Plot(uint16_t x, uint16_t y, uint8_t color)
    {
        return color;
    }

    static bool EarlyBail(PicScreenFlags dwDrawEnable, uint8_t color)
    {
        return false;
    }
    static const uint8_t White = 0xff;

    typedef uint8_t PixelType;
};

template<typename _TFormat>
inline void _PlotPix(PicData *pData, uint16_t x, uint16_t y, PicScreenFlags dwDrawEnable, typename _TFormat::PixelType color, uint8_t bPriorityValue, uint8_t bControlValue)
{
    if( x >= pData->size.cx || y >= pData->size.cy)
    {
        return;
    }

    int p = BUFFEROFFSET_NONSTD(pData->size.cx, pData->size.cy, x, y);
    //_PlotPixI(p, pData, x, y, dwDrawEnable, color, bPriorityValue, bControlValue);
    // Duplicate the code from _PlotPixI here for speed (perf increase of ~5%?)
    if (IsFlagSet(pData->dwMapsToRedraw, PicScreenFlags::Visual) && IsFlagSet(dwDrawEnable, PicScreenFlags::Visual))
    {
        pData->pdataVisual[p] = _TFormat::Plot(x, y, color);
    }
    if (IsFlagSet(pData->dwMapsToRedraw, PicScreenFlags::Priority) && IsFlagSet(dwDrawEnable, PicScreenFlags::Priority))
    {
        pData->pdataPriority[p] = bPriorityValue;
    }
    if (IsFlagSet(pData->dwMapsToRedraw, PicScreenFlags::Control) && IsFlagSet(dwDrawEnable, PicScreenFlags::Control))
    {
        pData->pdataControl[p] = bControlValue;
    }

    pData->pdataAux[p] |= (uint8_t)dwDrawEnable;
}

void _PlotPixNonStd(int cx, int cy, PicData *pData, uint16_t x, uint16_t y, PicScreenFlags dwDrawEnable, EGACOLOR color, uint8_t bPriorityValue, uint8_t bControlValue)
{
    if( x >= cx || y >= cy)
    {
        return;
    }

    int p = BUFFEROFFSET_NONSTD(cx, cy, x, y);
    _PlotPixI(p, pData, x, y, dwDrawEnable, color, bPriorityValue, bControlValue);
}



#define LINEMACRO(pData, startx, starty, deltalinear, deltanonlinear, linearvar, nonlinearvar, \
                  linearend, nonlinearstart, linearmod, nonlinearmod) \
   x = (startx); y = (starty); \
   incrNE = ((deltalinear) > 0)? (deltalinear) : -(deltalinear); \
   incrNE <<= 1; \
   deltanonlinear <<= 1; \
   incrE = ((deltanonlinear) > 0) ? -(deltanonlinear) : (deltanonlinear);  \
   d = nonlinearstart-1;  \
   while (linearvar != (linearend)) { \
     _PlotPix<_TFormat>(pData, x,y, dwDrawEnable, color, bPriorityValue, bControlValue); \
     linearvar += linearmod; \
     if ((d+=incrE) < 0) { \
       d += incrNE; \
       nonlinearvar += nonlinearmod; \
     }; \
   }; \
   _PlotPix<_TFormat>(pData, x, y, dwDrawEnable, color, bPriorityValue, bControlValue);

template<typename _TFormat>
void _DitherLine(PicData *pData, uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd, typename _TFormat::PixelType color, uint8_t bPriorityValue, uint8_t bControlValue, PicScreenFlags dwDrawEnable)
{
    int dx, dy, incrE, incrNE, d, finalx, finaly;
    int x = (int)xStart;
    int y = (int)yStart;
    dx = (int)xEnd - (int)x;
    dy = (int)yEnd - (int)y;
    finalx = (int)xEnd;
    finaly = (int)yEnd;

    dx = abs(dx);
    dy = abs(dy);

    if (dx > dy) {
        if (finalx < x) {
            if (finaly < y) { /* llu == left-left-up */
                LINEMACRO(pData, x, y, dx, dy, x, y, finalx, dx, -1, -1);
            } else {         /* lld */
                LINEMACRO(pData, x, y, dx, dy, x, y, finalx, dx, -1, 1);
            }
        } else { /* x1 >= x */
            if (finaly < y) { /* rru */
                LINEMACRO(pData, x, y, dx, dy, x, y, finalx, dx, 1, -1);
            } else {         /* rrd */
                LINEMACRO(pData, x, y, dx, dy, x, y, finalx, dx, 1, 1);
            }
        }
    } else { /* dx <= dy */
        if (finaly < y) {
            if (finalx < x) { /* luu */
                LINEMACRO(pData, x, y, dy, dx, y, x, finaly, dy, -1, -1);
            } else {         /* ruu */
                LINEMACRO(pData, x, y, dy, dx, y, x, finaly, dy, -1, 1);
            }
        } else { /* y1 >= y */
            if (finalx < x) { /* ldd */
                LINEMACRO(pData, x, y, dy, dx, y, x, finaly, dy, 1, -1);
            } else {         /* rdd */
                LINEMACRO(pData, x, y, dy, dx, y, x, finaly, dy, 1, 1);
            }
        }
    }
}


uint8_t circles[][30] = { /* bitmaps for circle patterns */
{0x80},
{0x4e, 0x40},
{0x73, 0xef, 0xbe, 0x70},
{0x38, 0x7c, 0xfe, 0xfe, 0xfe, 0x7c, 0x38, 0x00},
{0x1c, 0x1f, 0xcf, 0xfb, 0xfe, 0xff, 0xbf, 0xef, 0xf9, 0xfc, 0x1c},
{0x0e, 0x03, 0xf8, 0x7f, 0xc7, 0xfc, 0xff, 0xef, 0xfe, 0xff, 0xe7,
    0xfc, 0x7f, 0xc3, 0xf8, 0x1f, 0x00},
{0x0f, 0x80, 0xff, 0x87, 0xff, 0x1f, 0xfc, 0xff, 0xfb, 0xff, 0xef,
    0xff, 0xbf, 0xfe, 0xff, 0xf9, 0xff, 0xc7, 0xff, 0x0f, 0xf8,
    0x0f, 0x80},
{0x07, 0xc0, 0x1f, 0xf0, 0x3f, 0xf8, 0x7f, 0xfc, 0x7f, 0xfc, 0xff,
    0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0xff, 0xfe, 0x7f,
    0xfc, 0x7f, 0xfc, 0x3f, 0xf8, 0x1f, 0xf0, 0x07, 0xc0}};
uint8_t junq[32] = { /* random-looking fill pattern */
0x20, 0x94, 0x02, 0x24, 0x90, 0x82, 0xa4, 0xa2, 0x82, 0x09, 0x0a, 0x22,
0x12, 0x10, 0x42, 0x14, 0x91, 0x4a, 0x91, 0x11, 0x08, 0x12, 0x25, 0x10,
0x22, 0xa8, 0x14, 0x24, 0x00, 0x50, 0x24, 0x04};
uint8_t junqindex[128] = { /* starting points for junq fill */
0x00, 0x18, 0x30, 0xc4, 0xdc, 0x65, 0xeb, 0x48,
0x60, 0xbd, 0x89, 0x05, 0x0a, 0xf4, 0x7d, 0x7d,
0x85, 0xb0, 0x8e, 0x95, 0x1f, 0x22, 0x0d, 0xdf,
0x2a, 0x78, 0xd5, 0x73, 0x1c, 0xb4, 0x40, 0xa1,
0xb9, 0x3c, 0xca, 0x58, 0x92, 0x34, 0xcc, 0xce,
0xd7, 0x42, 0x90, 0x0f, 0x8b, 0x7f, 0x32, 0xed,
0x5c, 0x9d, 0xc8, 0x99, 0xad, 0x4e, 0x56, 0xa6,
0xf7, 0x68, 0xb7, 0x25, 0x82, 0x37, 0x3a, 0x51,
0x69, 0x26, 0x38, 0x52, 0x9e, 0x9a, 0x4f, 0xa7,
0x43, 0x10, 0x80, 0xee, 0x3d, 0x59, 0x35, 0xcf,
0x79, 0x74, 0xb5, 0xa2, 0xb1, 0x96, 0x23, 0xe0,
0xbe, 0x05, 0xf5, 0x6e, 0x19, 0xc5, 0x66, 0x49,
0xf0, 0xd1, 0x54, 0xa9, 0x70, 0x4b, 0xa4, 0xe2,
0xe6, 0xe5, 0xab, 0xe4, 0xd2, 0xaa, 0x4c, 0xe3,
0x06, 0x6f, 0xc6, 0x4a, 0xa4, 0x75, 0x97, 0xe1
};

//
// fPattern:        pattern or solid?
// fRectangle:      rect or circle?
//
template<typename _TFormat>
void _DrawPattern(PicData *pData, uint16_t x, uint16_t y, typename _TFormat::PixelType color, uint8_t bPriorityValue, uint8_t bControlValue, PicScreenFlags dwDrawEnable, bool fPattern, uint8_t bPatternSize, uint8_t bPatternNR, bool fRectangle)
{
    uint16_t wSize = (uint16_t)bPatternSize;

    uint16_t xMax = pData->size.cx - 1;
    uint16_t ýMax = pData->size.cy - 1;

    // Fix up x and y
    if (x < wSize)
    {
        x = wSize;
    }
    if ((x + wSize) > xMax)
    {
        x = xMax - wSize;
    }
    if (y < wSize)
    {
        y = wSize;
    }
    if ((y + wSize) > ýMax)
    {
        y = ýMax - wSize;
    }

    if (bPatternNR < ARRAYSIZE(junqindex))
    {
        uint8_t junqbit = junqindex[bPatternNR];
        if (fRectangle)
        {
            uint16_t k, l;
            for (l = y - wSize; l <= y + wSize; l++)
            {
                for (k = x - wSize; k <= (x + wSize + 1); k++)
                {
                    if (fPattern)
                    {
                        if ( (junq[junqbit>>3] >> (7-(junqbit & 7))) & 1)
                        {
                            _PlotPix<_TFormat>(pData, k, l, dwDrawEnable, color, bPriorityValue, bControlValue);
                        }
                        junqbit++;
                        if (junqbit == 0xff)
                        {
                            junqbit = 0;
                        }
                    }
                    else
                    {
                        _PlotPix<_TFormat>(pData, k, l, dwDrawEnable, color, bPriorityValue, bControlValue);
                    }
                }
            }
        }
        else
        {
            uint16_t k, l;
            // Circle
            int circlebit = 0;
            for (l = y - wSize; l <= y + wSize; l++)
            {
                for (k = x- wSize; k <= x + wSize + 1; k++)
                {
                    if ((circles[bPatternSize][circlebit>>3] >> (7-(circlebit & 7))) & 1)
                    {
                        if (fPattern)
                        {
                            if ((junq[junqbit>>3] >> (7-(junqbit & 7))) & 1)
                            {
                                _PlotPix<_TFormat>(pData, k, l, dwDrawEnable, color, bPriorityValue, bControlValue);
                            }
                            junqbit++;
                            if (junqbit == 0xff)
                            {
                                junqbit = 0;
                            }
                        }
                        else
                        {
                            _PlotPix<_TFormat>(pData, k, l, dwDrawEnable, color, bPriorityValue, bControlValue);
                        }
                    }
                    circlebit++;
                }
            }
        }
    }
    else
    {
        REPORTERROR(TEXT("bPatternNR was too high"));
    }
}

//
// REVIEW: This sucks - I had to make a full copy of this function, just to allow it to draw to something smaller
// than 320 x 200.  (don't want to pass extra params to _PlotPix due to perf)
//
void DrawPatternInRect(int cx, int cy, PicData *pData, uint16_t x, uint16_t y, EGACOLOR color, uint8_t bPriorityValue, uint8_t bControlValue, PicScreenFlags dwDrawEnable, const PenStyle *pPenStyle)
{
    uint16_t wSize = (uint16_t)pPenStyle->bPatternSize;

    // Fix up x and y
    if (x < wSize)
    {
        x = wSize;
    }
    if ((x + wSize) > (cx - 1))
    {
        x = (cx - 1) - wSize;
    }
    if (y < wSize)
    {
        y = wSize;
    }
    if ((y + wSize) > (cy - 1))
    {
        y = (cy - 1) - wSize;
    }

    if (pPenStyle->bPatternNR < ARRAYSIZE(junqindex))
    {
        uint8_t junqbit = junqindex[pPenStyle->bPatternNR];
        if (pPenStyle->fRectangle)
        {
            uint16_t k, l;
            for (l = y - wSize; l <= y + wSize; l++)
            {
                for (k = x - wSize; k <= (x + wSize + 1); k++)
                {
                    if (pPenStyle->fPattern)
                    {
                        if ( (junq[junqbit>>3] >> (7-(junqbit & 7))) & 1)
                        {
                            _PlotPixNonStd(cx, cy, pData, k, l, dwDrawEnable, color, bPriorityValue, bControlValue);
                        }
                        junqbit++;
                        if (junqbit == 0xff)
                        {
                            junqbit = 0;
                        }
                    }
                    else
                    {
                        _PlotPixNonStd(cx, cy, pData, k, l, dwDrawEnable, color, bPriorityValue, bControlValue);
                    }
                }
            }
        }
        else
        {
            uint16_t k, l;
            // Circle
            int circlebit = 0;
            for (l = y - wSize; l <= y + wSize; l++)
            {
                for (k = x- wSize; k <= x + wSize + 1; k++)
                {
                    if ((circles[pPenStyle->bPatternSize][circlebit>>3] >> (7-(circlebit & 7))) & 1)
                    {
                        if (pPenStyle->fPattern)
                        {
                            if ((junq[junqbit>>3] >> (7-(junqbit & 7))) & 1)
                            {
                                _PlotPixNonStd(cx, cy, pData, k, l, dwDrawEnable, color, bPriorityValue, bControlValue);
                            }
                            junqbit++;
                            if (junqbit == 0xff)
                            {
                                junqbit = 0;
                            }
                        }
                        else
                        {
                            _PlotPixNonStd(cx, cy, pData, k, l, dwDrawEnable, color, bPriorityValue, bControlValue);
                        }
                    }
                    circlebit++;
                }
            }
        }
    }
    else
    {
        REPORTERROR(TEXT("bPatternNR was too high"));
    }
}

CelIndex MakeViewResourceIndexFromCelAndLoop(const ResourceEntity *pvr, int nLoop, int nCel)
{
    const RasterComponent &raster = pvr->GetComponent<RasterComponent>();
    nLoop = nLoop % raster.Loops.size();
    if (nLoop < 0)
    {
        nLoop = raster.Loops.size() + nLoop;
        assert(nLoop > 0);
    }
    const Loop &loop = raster.Loops[nLoop];
    nCel = nCel % loop.Cels.size();
    if (nCel < 0)
    {
        nCel = loop.Cels.size() + nCel;
        assert(nCel > 0);
    }
    return CelIndex(nLoop, nCel);
}

int mod(int x, int m) {
    return (x%m + m) % m;
}

const Cel &GetCel(const ResourceEntity *pvr, int &nLoop, int &nCel)
{
    const RasterComponent &raster = pvr->GetComponent<RasterComponent>();
    nLoop = mod(nLoop, (int)raster.Loops.size());
    nCel = mod(nCel, (int)raster.Loops[nLoop].Cels.size());
    return raster.Loops[nLoop].Cels[nCel];
}

//
// Give a view, and its position, returns the point that is at the
// center of the view.
//
CPoint FindCenterOfView(uint16_t xEgo, uint16_t yEgo, const ResourceEntity *pvr, int nLoop, int nCel)
{
    const Cel &cel = GetCel(pvr, nLoop, nCel);
    int xiEgo = (int)xEgo + cel.placement.x;
    int yiEgo = (int)yEgo + cel.placement.y;
    return CPoint(xiEgo, yiEgo - cel.size.cy / 2);
}

#define YSTEP 2 // We may want to allow the user to change this.
CRect GetViewBoundsRect(uint16_t xEgo, uint16_t yEgo, const ResourceEntity *pvr, int nLoop, int nCel)
{
    const Cel &cel = GetCel(pvr, nLoop, nCel);
    CRect rect;
    rect.left = xEgo - cel.placement.x - (cel.size.cx / 2);
    rect.right = rect.left + cel.size.cx;
    rect.bottom = yEgo + 1;
    rect.top = rect.bottom - YSTEP;
    return rect;
}

//
// Determines if an object bounded by rect, can be there, based on the control mask.
// rect is inclusive (e.g. includes both left and right columns, and top and bottom lines)
//
bool CanBeHere(size16 size, const uint8_t *pdataControl, const CRect &rect, uint16_t wControlMask)
{
    CRect rcClipped = rect;
    rcClipped.left = max(rcClipped.left, 0);
    rcClipped.top = max(rcClipped.top, 0);
    rcClipped.bottom = min(rcClipped.bottom, size.cy - 1);
    rcClipped.right = min(rcClipped.right, size.cx - 1);
    for (int y = rcClipped.top; y <= rcClipped.bottom; y++)
    {
        for (int x = rcClipped.left; x <= rcClipped.right; x++)
        {
            int p = BUFFEROFFSET_NONSTD(size.cx, size.cy, x, y);
            assert(pdataControl[p] <= 15);
            if (wControlMask & (1 << (uint16_t)pdataControl[p]))
            {
                return false;
            }
        }
    }
    return true;
}

bool HitTestView(uint16_t xCursor, uint16_t yCursor, uint16_t xEgo, uint16_t yEgo, const ResourceEntity *pvr, int nLoop, int nCel)
{
    const Cel &cel = GetCel(pvr, nLoop, nCel);
    int xiEgo = (int)xEgo + cel.placement.x;
    int yiEgo = (int)yEgo + cel.placement.y;
    if ((xiEgo >= 0) && (yiEgo >= 0))
    {
        return HitTestEgoBox(xCursor, yCursor, (uint16_t)xiEgo, (uint16_t)yiEgo, cel.size.cx, cel.size.cy);
    }
    else
    {
        return FALSE;
    }
}

void GetViewRect(CRect *prc, uint16_t xEgo, uint16_t yEgo, const ResourceEntity *pvr, int nLoop, int nCel)
{
    const Cel &cel = GetCel(pvr, nLoop, nCel);
    int xiEgo = (int)xEgo + cel.placement.x;
    int yiEgo = (int)yEgo + cel.placement.y;

    prc->top = yiEgo - cel.size.cy;
    prc->bottom = yiEgo;
    prc->left = xiEgo - cel.size.cx / 2;
    prc->right = xiEgo + cel.size.cx / 2;
}


//
// y is at the bottom of ego, x is in the middle (just like the game)
//
bool HitTestEgoBox(uint16_t xCursor, uint16_t yCursor, uint16_t xEgo, uint16_t yEgo, uint16_t cx, uint16_t cy)
{
    uint16_t cxHalf = cx / 2;

    if (yCursor > yEgo)
    {
        return FALSE;
    }

    uint16_t ySafeTop;
    if (yEgo < cy)
    {
        ySafeTop = 0;
    }
    else
    {
        ySafeTop = yEgo - cy;
    }
    if (yCursor < ySafeTop)
    {
        return FALSE;
    }

    if (xCursor > (cxHalf + xEgo))
    {
        return FALSE;
    }

    uint16_t xSafeLeft;
    if (xEgo < cxHalf)
    {
        xSafeLeft = 0;
    }
    else
    {
        xSafeLeft = xEgo - cxHalf;
    }
    if (xCursor < xSafeLeft)
    {
        return FALSE;
    }

    return TRUE;
}

void DrawImageWithPriority(size16 displaySize, uint8_t *pdataDisplay, const uint8_t *pdataPriority, uint8_t bEgoPriority, int xLeft, int yTop, uint16_t cx, uint16_t cy, const uint8_t *pImageData, uint8_t transparent, bool fShowOutline)
{
    int xRight = xLeft + cx;
    int yBottom = yTop + cy;
    int yView = 0;
    for (int yPic = yTop; yPic < yBottom; yPic++, yView++)
    {
        if ((yPic < displaySize.cy) && (yPic >= 0))
        {
            int xViewLeft = 0;
            if (xLeft < 0)
            {
                // If we're off the left of the pic, don't copy from the beginning of the view's row
                xViewLeft = -xLeft;
            }
            int xViewRight = cx;
            if (xRight >= displaySize.cx)
            {
                xViewRight -= (xRight - displaySize.cx);
            }

            int xFirstPixel = -1;
            int xLastPixel = -1;
            int xPic = max(0, xLeft);
            for (int xView = xViewLeft; xView < xViewRight; xView++, xPic++)
            {
                int pPicOffset = BUFFEROFFSET_NONSTD(displaySize.cx, displaySize.cy, xPic, yPic);
                // We can draw something.
                //int pViewOffset = BUFFEROFFSET_NONSTD(CX_ACTUAL(cx), cy, xView, yView + 1);
                int pViewOffset = BUFFEROFFSET_NONSTD(CX_ACTUAL(cx), cy, xView, yView);
                uint8_t bView = *(pImageData + pViewOffset);
                if (bView != transparent)
                {
                    bool fShowing = (pdataPriority != nullptr) ? ((*(pdataPriority + pPicOffset)) <= bEgoPriority) : true;
                    if (fShowing)
                    {
                        // Copy a pixel!
                        *(pdataDisplay + pPicOffset) = bView;
                        xLastPixel = -1;
                    }
                    else
                    {
                        if (fShowOutline)
                        {
                            xLastPixel = xPic;
                            if (xFirstPixel == -1)
                            {
                                xFirstPixel = xPic;
                            }
                        }
                    }
                }
            }
            if (fShowOutline)
            {
                if ((xFirstPixel >= 0) && (xFirstPixel < displaySize.cx))
                {
                    int pPicOffset = BUFFEROFFSET_NONSTD(displaySize.cx, displaySize.cy, xFirstPixel, yPic);
                    *(pdataDisplay + pPicOffset) = bEgoPriority;
                }
                if ((xLastPixel >= 0) && (xLastPixel < displaySize.cx))
                {
                    int pPicOffset = BUFFEROFFSET_NONSTD(displaySize.cx, displaySize.cy, xLastPixel, yPic);
                    *(pdataDisplay + pPicOffset) = bEgoPriority;
                }

            }
        }
    }
}

//
// Draws a view (represented by pvr) onto a pic (represented by pdataDisplay and pdataPriority)
//
void DrawViewWithPriority(size16 displaySize, uint8_t *pdataDisplay, const uint8_t *pdataPriority, uint8_t bEgoPriority, uint16_t xIn, uint16_t yIn, const ResourceEntity *pvr, int nLoop, int nCel, bool fShowOutline)
{
    const Cel &cel = GetCel(pvr, nLoop, nCel);
    uint8_t bTransparent = cel.TransparentColor;

    yIn += cel.placement.y;
    xIn += cel.placement.x;
   
    int xLeft = xIn - (cel.size.cx / 2);
    int yTop = yIn - cel.size.cy + 1;

    std::unique_ptr<uint8_t[]> data = std::make_unique<uint8_t[]>(CX_ACTUAL(cel.size.cx) * cel.size.cy);
    CopyBitmapData(pvr->GetComponent<RasterComponent>(), CelIndex(nLoop, nCel), data.get(), cel.size);

    DrawImageWithPriority(displaySize, pdataDisplay, pdataPriority, bEgoPriority, xLeft, yTop, cel.size.cx, cel.size.cy, &data[0], bTransparent, fShowOutline);
}


void DrawBoxWithPriority(size16 picSize, uint8_t *pdataDisplay, const uint8_t *pdataPriority, uint8_t bEgoPriority, uint16_t xIn, uint16_t yIn, uint16_t cx, uint16_t cy)
{
    int xMax = picSize.cx - 1;
    int yMax = picSize.cy - 1;

    uint16_t cxHalf = cx / 2;
    uint16_t xLeft;
    if (xIn < cxHalf)
    {
        xLeft = 0;
    }
    else
    {
        xLeft = xIn - cxHalf;
    }
    uint16_t xRight = xIn + cxHalf;
    if (xRight >= xMax)
    {
        xRight = xMax;
    }

    uint16_t yBottom = yIn;
    if (yBottom > yMax)
    {
        yBottom = yMax;
    }
    uint16_t ySafeTop;
    if (yIn < cy)
    {
        ySafeTop = 0;
    }
    else
    {
        ySafeTop = yIn - cy;
    }

    for (uint16_t x = xLeft; x <= xRight; x++)
    {
        for (uint16_t y = ySafeTop; y <= yBottom; y++)
        {
            int p = BUFFEROFFSET_NONSTD(picSize.cx, picSize.cy, x, y);
            if ( (*(pdataPriority + p)) <= bEgoPriority)
            {
                // Then draw it.  Avoid using _PlotPix, since we need an aux map for that.
                if ( ((x - xLeft) < 2) || ((xRight - x) < 2) || ((y - ySafeTop) < 2) || ((yBottom - y) < 2))
                {
                    pdataDisplay[p] = 0x0f; // White
                }
                else
                {
                    pdataDisplay[p] = bEgoPriority; // the priority colour
                }

            }
        }
    }
}

#define EMPTY ((__int32)-1)

// Big global buffer.
// We need to take care to serialize access to this.
// It shouldn't be a problem if all drawing is done sequentially, and on the foreground thread.
sPOINT g_buf[BMPSIZE_MAX + 1];
sPOINT g_pEmpty={EMPTY,EMPTY};

#define GET_PIX_VISUAL(cx, cy, x, y) (*((pdata->pdataVisual) + BUFFEROFFSET_NONSTD(cx, cy, x, y)))
#define GET_PIX_AUX(cx, cy, x, y) (*((pdata->pdataAux) +  BUFFEROFFSET_NONSTD(cx, cy, x, y)))

//
// BUG in FILL_BOUNDS and DitherFill:
// when you fill, normally white is empty, even if the aux screen is marked there.
// when you fill with a half white color, you need to REALLY mark the aux screen, such that even a white
// spot it considered filled.
//
// Questionable whether we need to fix this or not, it only affects really large areas.
//

#define FILL_BOUNDS(cx, cy, fx, fy, white) ((((uint8_t)dwDrawEnable) & GET_PIX_AUX((cx), (cy), (fx), (fy))) && \
                  !(IsFlagSet(dwDrawEnable, PicScreenFlags::Visual) && (GET_PIX_VISUAL((cx), (cy), (fx), (fy))==(white))))


inline bool qstore(int displayByteSize, __int16 x,__int16 y, __int32 *prpos)
{
   if ((*prpos) == displayByteSize)
   {
       return FALSE;
   }
   g_buf[*prpos].x = x;
   g_buf[*prpos].y = y;
   (*prpos)++;
   return TRUE;
}

inline sPOINT qretrieve(__int32 *prpos)
{
   if (!*prpos)
   {
       return g_pEmpty;
   }

   (*prpos)--;
   return g_buf[*prpos];
}


#define OK_TO_FILL(cx, cy, x,y,white) ( CHECK_RECT((cx), (cy), (x),(y)) && !FILL_BOUNDS((cx), (cy), (x),(y),(white)) )

int g_commands = 0;


#define MOVING_RIGHT 0x00000001
#define MOVING_LEFT  0x00000002


CRITICAL_SECTION g_csDither = { 0 };
bool g_fDitherCritSecInited = FALSE;

void InitDitherCritSec()
{
    assert(!g_fDitherCritSecInited);
    InitializeCriticalSection(&g_csDither);
    g_fDitherCritSecInited = TRUE;
}

void DeleteDitherCritSec()
{
    if (g_fDitherCritSecInited)
    {
        DeleteCriticalSection(&g_csDither);
        g_fDitherCritSecInited = FALSE;
    }
}

template<typename _TFormat>
void _DitherFill(PicData *pdata, uint16_t x, uint16_t y, typename  _TFormat::PixelType color, uint8_t bPriorityValue, uint8_t bControlValue, PicScreenFlags dwDrawEnable)
{
    if (!g_fDitherCritSecInited)
    {
        return;
    }

    // Guard against someone doing a fill with pure white, since this algorithm will hang in that case.
    // Hero's quest does this, when some pictures are drawn with some palettes.
    if (_TFormat::EarlyBail(dwDrawEnable, color))
    {
        return;
    }

    // If no screen is being drawn to, bail.
    if (dwDrawEnable == PicScreenFlags::None)
    {
        return;
    }

    // Guard access to the buffers we use for dithering.
    CGuard guard(&g_csDither);

    g_commands =0;

    sPOINT p;
    __int32 rpos, spos;

    int cx = pdata->size.cx;
    int cy = pdata->size.cy;
    int xMax = cx - 1;
    int yMax = cy - 1;
    int displayByteSize = cx * cy;

    if (!CHECK_RECT(cx, cy, x, y))
    {
        return;
    }

    if (FILL_BOUNDS(cx, cy, x, y, _TFormat::White))
    {
        return;
    }

    __int16 x1, y1;

    rpos = spos = 0;

    if (!qstore(displayByteSize, x, y, &rpos))
    {
        return;
    }

    for (;;)
    {
        p = qretrieve(&rpos);
        x1 = p.x;
        y1 = p.y;

        if ((x1 == EMPTY) || (y1 == EMPTY))
        {
            return;
        }
        else
        {
            if (OK_TO_FILL(cx, cy, x1, y1, _TFormat::White))
            {
                // PERF: maybe inline this call?
                _PlotPix<_TFormat>(pdata, (uint16_t)x1, (uint16_t)y1, dwDrawEnable, color, bPriorityValue, bControlValue);

                // PERF: Tried removing OK_TO_FILL here, but it made it worse
                // (It's technically uncessary)
                if ((y1 != 0) && OK_TO_FILL(cx, cy, x1, y1 - 1, _TFormat::White))
                {
                    if (!qstore(displayByteSize, x1, y1 - 1, &rpos)) break;
                }
                if ((x1 != 0) && OK_TO_FILL(cx, cy, x1 - 1, y1, _TFormat::White))
                {  
                    if (!qstore(displayByteSize, x1 - 1, y1, &rpos)) break;
                }
                if ((x1 != xMax) && OK_TO_FILL(cx, cy, x1 + 1, y1, _TFormat::White))
                { 
                    if (!qstore(displayByteSize, x1 + 1, y1, &rpos)) break;;
                }
                if ((y1 != yMax) && OK_TO_FILL(cx, cy, x1, y1 + 1, _TFormat::White))
                {
                    if (!qstore(displayByteSize, x1, y1 + 1, &rpos)) break;;
                }

                g_commands += 4;
            }

        }
    }
}




//
// DRAW LINE
//
void PicCommand::_CreateLine(uint16_t xFrom, uint16_t yFrom, uint16_t xTo, uint16_t yTo)
{
    assert(_IsEmpty());
    type = Line;
    drawLine.xFrom = xFrom;
    drawLine.yFrom = yFrom;
    drawLine.xTo = xTo;
    drawLine.yTo = yTo;
}

void LineCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState)
{
    LineCommand_DrawOnly(pCommand, pData, pState);
}

//
// This doesn't update the picstate.
//
void LineCommand_DrawOnly(const PicCommand *pCommand, PicData *pData, const ViewPort *pState)
{
    if (pData->isVGA)
    {
        _DitherLine<PlotVGA>(pData,
            pCommand->drawLine.xFrom, pCommand->drawLine.yFrom, pCommand->drawLine.xTo, pCommand->drawLine.yTo,
            pState->bPaletteOffset, pState->bPriorityValue, pState->bControlValue, pState->dwDrawEnable);
    }
    else
    {
        _DitherLine<PlotEGA>(pData,
            pCommand->drawLine.xFrom, pCommand->drawLine.yFrom, pCommand->drawLine.xTo, pCommand->drawLine.yTo,
            pState->egaColor, pState->bPriorityValue, pState->bControlValue, pState->dwDrawEnable);
    }
}

//
// Override for drawing lines on the buffers, w/o the line actually being part of the pic.
//
void LineCommand_DrawOverride(PicCommand *pCommand, PicData *pData, EGACOLOR color, uint8_t bPriorityValue, uint8_t bControlValue, PicScreenFlags dwDrawEnable)
{
    _DitherLine<PlotEGA>(pData,
        pCommand->drawLine.xFrom, pCommand->drawLine.yFrom, pCommand->drawLine.xTo, pCommand->drawLine.yTo,
        color, bPriorityValue, bControlValue, dwDrawEnable);
}


void LineCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf)
{
    StringCchPrintf(pszBuf, cchBuf, "Line: %dx%d - %dx%d", pCommand->drawLine.xFrom, pCommand->drawLine.yFrom, pCommand->drawLine.xTo, pCommand->drawLine.yTo);
}

void LineCommand_TODO(PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf)
{
    *pszBuf = 0;
}

void LineCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState)
{
    // This is complex.
    // To draw a line.
    // Get the current DRAWSIZE for this line.  Check if the this one is equal to or smaller
    // than the previous.  If so, add to it.  Otherwise, start a new command.
    DRAWSIZE ds = DrawSizeFromCoords(pCommand->drawLine.xFrom, pCommand->drawLine.yFrom,
                                     pCommand->drawLine.xTo, pCommand->drawLine.yTo);

    // We need to start a new command if the previous one was a different kind of command (or none)
    bool bStartNew = (!pCommandPrev) || (pCommand->type != pCommandPrev->type);

    // Or if the line ended on a different than this one begins.
    if (!bStartNew)
    {
        bStartNew = ((pCommand->drawLine.xFrom != pCommandPrev->drawLine.xTo) ||
                    (pCommand->drawLine.yFrom != pCommandPrev->drawLine.yTo));
    }

    if (!bStartNew)
    {
        // It was the same kind of command.  Maybe we can keep adding to it.  We can do so
        // if it our DRAWSIZE is less than or equal to the previous DRAWSIZE.
        // But how can we be sure of the previous DRAWSIZE?  It might expost itself as smaller than
        // it really was, since the thing before it could have been bigger.  So it needs to be
        // passed in as a param.
        bStartNew = (ds > dsPrev);
    }

    if (bStartNew)
    {
        // Start a new command.
        pSerial->WriteByte(OpcodeFromCommandTypeAndSize(PicCommand::Line, ds));
        
        _WriteAbsCoordinate(pSerial, pCommand->drawLine.xFrom, pCommand->drawLine.yFrom);
        _WriteCoordinate(pSerial, ds, pCommand->drawLine.xFrom, pCommand->drawLine.yFrom, pCommand->drawLine.xTo, pCommand->drawLine.yTo);
        *pds = ds; // Pass out the DRAWSIZE we used.
    }
    else
    {
        // Continue from the old command.
        _WriteCoordinate(pSerial, dsPrev, pCommand->drawLine.xFrom, pCommand->drawLine.yFrom, pCommand->drawLine.xTo, pCommand->drawLine.yTo);
        *pds = dsPrev;
    }
}







//
// PATTERN
//
void PicCommand::_CreatePattern(uint16_t x, uint16_t y, uint8_t bPatternSize, uint8_t bPatternNR, bool fPattern, bool fRectangle)
{
    assert(_IsEmpty());
    assert(bPatternNR < 128);

    type = Pattern;
    drawPattern.x = x;
    drawPattern.y = y;
    drawPattern.bPatternSize = bPatternSize;
    drawPattern.bPatternNR = bPatternNR;

    // Even though the SCI pic resource specifies that PIC_OP_SET_PATTERN is a separate command, we integrate
    // it into each draw-pattern command.  The avoids the user falling into the trap of drawing a pattern
    // with a particular fSolid state, and then going back earlier in the pic and changing it (if that is
    // allowed, this confuses the rendering engine (it may crash, hang, or draw incorrectly) - so this editor will not allow that).
    drawPattern.wFlags = 0;
    if (fRectangle)
    {
        drawPattern.wFlags |= PATTERN_FLAG_RECTANGLE;
    }

    if (fPattern)
    {
        drawPattern.wFlags |= PATTERN_FLAG_USE_PATTERN;        
    }
}

// This version doesn't update the pState.  It is used for previewing the pen tool.
void PatternCommand_Draw_DrawOnly(const PicCommand *pCommand, PicData *pData, const ViewPort *pState)
{
    if (pData->isVGA)
    {
        _DrawPattern<PlotVGA>(pData,
            pCommand->drawPattern.x, pCommand->drawPattern.y,
            pState->bPaletteOffset, pState->bPriorityValue, pState->bControlValue, pState->dwDrawEnable,
            (pCommand->drawPattern.wFlags & PATTERN_FLAG_USE_PATTERN) != 0,
            pCommand->drawPattern.bPatternSize,
            pCommand->drawPattern.bPatternNR,
            (pCommand->drawPattern.wFlags & PATTERN_FLAG_RECTANGLE) != 0);
    }
    else
    {
        _DrawPattern<PlotEGA>(pData,
            pCommand->drawPattern.x, pCommand->drawPattern.y,
            pState->egaColor, pState->bPriorityValue, pState->bControlValue, pState->dwDrawEnable,
            (pCommand->drawPattern.wFlags & PATTERN_FLAG_USE_PATTERN) != 0,
            pCommand->drawPattern.bPatternSize,
            pCommand->drawPattern.bPatternNR,
            (pCommand->drawPattern.wFlags & PATTERN_FLAG_RECTANGLE) != 0);
    }
}

// This version *only* updates state.
void PatternCommand_Draw_StateOnly(const PicCommand *pCommand, PicData *pData, ViewPort *pState)
{
    // Update some state variables (even though they are part of the command, the view may want to know)
    pState->bPatternSize = pCommand->drawPattern.bPatternSize;
    //pState->bPatternCode = pCommand->drawPattern.wFlags;
    pState->bPatternNR = pState->bPatternNR;
}

void PatternCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState)
{
    PatternCommand_Draw_DrawOnly(pCommand, pData, pState);
    PatternCommand_Draw_StateOnly(pCommand, pData, pState);
}

void PatternCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf)
{
    StringCchPrintf(pszBuf, cchBuf, "Pen: %dx%d %s-%s(%d)", pCommand->drawPattern.x, pCommand->drawPattern.y,
                    pCommand->drawPattern.wFlags & PATTERN_FLAG_USE_PATTERN ? TEXT("Pat") : TEXT("Sld"),
                    pCommand->drawPattern.wFlags & PATTERN_FLAG_RECTANGLE ? TEXT("Rect") : TEXT("Circ"),    
                    pCommand->drawPattern.bPatternSize);
}

void PatternCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState)
{
    // We need to start a new command if the previous one was a different kind of command;
    bool bStartNew = (!pCommandPrev) || (pCommand->type != pCommandPrev->type);

    if (!bStartNew)
    {
        // Also, we need to make sure that the previous pattern size and stuff, is equal to ours
        if ((pCommandPrev->drawPattern.wFlags != pCommand->drawPattern.wFlags) ||
            (pCommandPrev->drawPattern.bPatternSize != pCommand->drawPattern.bPatternSize))
        {
            bStartNew = TRUE;
        }
    }

    if (!bStartNew)
    {
        // We can be able to continue from the last pattern command, if the previous DRAWSIZE is more
        // than the current (or undefined)
        DRAWSIZE ds = DrawSizeFromCoords(pCommandPrev->drawPattern.x, pCommandPrev->drawPattern.y,
                                         pCommand->drawPattern.x, pCommand->drawPattern.y);
        bStartNew = (ds > dsPrev);
    }

    if (bStartNew)
    {
        // Start a new command.
        // We need to figure out what the next command is though, before doing this.
        // Because we have no idea of the DRAWSIZE to use.
        DRAWSIZE dsThis;
        if (!pCommandNext || (pCommand->type != pCommandNext->type))
        {
            dsThis = DS_LARGE; // Well, we're just one point.
        }
        else
        {
            dsThis = DrawSizeFromCoords(pCommand->drawPattern.x, pCommand->drawPattern.y,
                                        pCommandNext->drawPattern.x, pCommandNext->drawPattern.y);
        }

        // Start the command.  First we'll set the pattern.
        // We can optimize this by realizing what the current pattern size is
        if (!pState->fInited ||
            (pState->wFlags != pCommand->drawPattern.wFlags) ||
            (pState->bPatternSize != pCommand->drawPattern.bPatternSize))
        {

            pState->fInited = TRUE;
            pState->wFlags = pCommand->drawPattern.wFlags;
            pState->bPatternSize = pCommand->drawPattern.bPatternSize;
            pSerial->WriteByte(PIC_OP_SET_PATTERN);
            pSerial->WriteByte((pCommand->drawPattern.wFlags) | (pCommand->drawPattern.bPatternSize));
        }

        // Write the opcode
        pSerial->WriteByte(OpcodeFromCommandTypeAndSize(PicCommand::Pattern, dsThis));

        // If we set the use pattern flag, we also need to write a byte here.
        if (pCommand->drawPattern.wFlags & PATTERN_FLAG_USE_PATTERN)
        {
            // Write the NR byte
            pSerial->WriteByte(pCommand->drawPattern.bPatternNR);
        }

        *pds = dsThis;
        // And we always start with an absolute coord, despite our DRAWSIZE
        _WriteAbsCoordinate(pSerial, pCommand->drawPattern.x, pCommand->drawPattern.y);
    }
    else
    {
        // Continue from the last one.

        if (pCommand->drawPattern.wFlags & PATTERN_FLAG_USE_PATTERN)
        {
            // Write the NR byte
            pSerial->WriteByte(pCommand->drawPattern.bPatternNR);
        }

        _WriteCoordinate(pSerial, dsPrev, pCommandPrev->drawPattern.x, pCommandPrev->drawPattern.y,
                                pCommand->drawPattern.x, pCommand->drawPattern.y);
    }
}




//
// FILL
//
void PicCommand::_CreateFill(uint16_t x, uint16_t y)
{
    assert(_IsEmpty());
    type = Fill;
    fill.x = x;
    fill.y = y;
}

void FillCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState)
{
    if (pData->isVGA)
    {
        _DitherFill<PlotVGA>(pData, pCommand->fill.x, pCommand->fill.y, pState->bPaletteOffset, pState->bPriorityValue, pState->bControlValue, pState->dwDrawEnable);
    }
    else
    {
        _DitherFill<PlotEGA>(pData, pCommand->fill.x, pCommand->fill.y, pState->egaColor, pState->bPriorityValue, pState->bControlValue, pState->dwDrawEnable);
    }
}

void FillCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf)
{
    StringCchPrintf(pszBuf, cchBuf, "Fill: %dx%d", pCommand->fill.x, pCommand->fill.y);    
}

void FillCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState)
{
    if (!pCommandPrev || (pCommand->type != pCommandPrev->type))
    {
        // The last command was of a different type, so write the opcode.
        pSerial->WriteByte(PIC_OP_FILL);
    }

    // Then the write the absolute coordinate.
    _WriteAbsCoordinate(pSerial, pCommand->fill.x, pCommand->fill.y);
}


//
// VISUAL SET
//
void PicCommand::_CreateSetVisual(uint8_t bPaletteNumber, uint8_t bPaletteIndex)
{
    assert(_IsEmpty());
    type = SetVisual;
    setVisual.isVGA = false;
    setVisual.bColor = BYTE_FROM_PALETTE_AND_OFFSET(bPaletteNumber, bPaletteIndex);
}

void PicCommand::_CreateSetVisualVGA(uint8_t bColor)
{
    assert(_IsEmpty());
    type = SetVisual;
    setVisual.isVGA = true;
    setVisual.bColor = bColor;
}

void SetVisualCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState)
{
    if (pCommand->setVisual.isVGA)
    {
        pState->bPaletteOffset = pCommand->setVisual.bColor;
    }
    else
    {
        // Obey SCI game behaviour of ignoring bPalette if not zero
        uint8_t bPaletteToUse = pCommand->setVisual.GetPaletteNumber();
        if (bPaletteToUse == 0)
        {
            bPaletteToUse = pState->bPaletteToDraw;
        }
        EGACOLOR *pPalette = GET_PALETTE(pState->pPalettes, bPaletteToUse);

        // Update the ViewPort
        pState->bPaletteNumber = pCommand->setVisual.GetPaletteNumber(); // This matters for UI state -> note, it is not necessarily bPaletteToUse.
        // bPaletteToUse represents the palette to use when drawing stuff right now, but the palette number
        // encoded in the command, might be different.
        pState->bPaletteOffset = pCommand->setVisual.GetPaletteIndex();  // This matters for UI state
        // Set the color we are actually going to draw, given current picstate:
        if (pState->rgLocked[pState->bPaletteOffset])
        {
            // Palette 0 has this index "locked".  So use palette 0 for drawing for this colour.
            pState->egaColor = pState->pPalettes[pCommand->setVisual.GetPaletteIndex()];
        }
        else
        {
            // The "normal" case
            pState->egaColor = pPalette[pCommand->setVisual.GetPaletteIndex()];
        }
    }
    pState->dwDrawEnable |= PicScreenFlags::Visual;
}

void SetVisualCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf)
{
    if (pCommand->setVisual.isVGA)
    {
        StringCchPrintf(pszBuf, cchBuf, "Set Visual: %d", pCommand->setVisual.bColor);;
    }
    else
    {
        StringCchPrintf(pszBuf, cchBuf, "Set Visual: %d,%d", pCommand->setVisual.GetPaletteNumber(), pCommand->setVisual.GetPaletteIndex());
    }
}

void SetVisualCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState)
{
    pSerial->WriteByte(PIC_OP_SET_COLOR);
    if (pCommand->setVisual.isVGA)
    {
        pSerial->WriteByte(pCommand->setVisual.bColor);
    }
    else
    {
        pSerial->WriteByte(BYTE_FROM_PALETTE_AND_OFFSET(pCommand->setVisual.GetPaletteNumber(), pCommand->setVisual.GetPaletteIndex()));
    }
}



void PicCommand::_CreateSetPriority(uint8_t bPriorityValue)
{
    assert(_IsEmpty());
    type = SetPriority;
    setPriority.bPriorityValue = bPriorityValue;
}

void SetPriorityCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState)
{
    pState->bPriorityValue = pCommand->setPriority.bPriorityValue;
    pState->dwDrawEnable |= PicScreenFlags::Priority;
}

void SetPriorityCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf)
{
    StringCchPrintf(pszBuf, cchBuf, "Set Priority: %d", pCommand->setPriority.bPriorityValue);
}

void SetPriorityCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState)
{
    pSerial->WriteByte(PIC_OP_SET_PRIORITY);
    pSerial->WriteByte(pCommand->setPriority.bPriorityValue);
}




void PicCommand::_CreateSetControl(uint8_t bControlValue)
{
    assert(_IsEmpty());
    type = SetControl;
    setControl.bControlValue = bControlValue;
}

void SetControlCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState)
{
    pState->bControlValue = pCommand->setControl.bControlValue;
    pState->dwDrawEnable |= PicScreenFlags::Control;
}

void SetControlCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf)
{
    StringCchPrintf(pszBuf, cchBuf, "Set Control: %d", pCommand->setControl.bControlValue); 
}

void SetControlCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState)
{
    pSerial->WriteByte(PIC_OP_SET_CONTROL);
    pSerial->WriteByte(pCommand->setControl.bControlValue);
}




void PicCommand::_CreateDisableVisual()
{
    assert(_IsEmpty());
    type = DisableVisual;
}

void DisableVisualCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState)
{
    pState->dwDrawEnable &= ~PicScreenFlags::Visual;
}

void DisableVisualCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf)
{
    StringCchPrintf(pszBuf, cchBuf, "Disable Visual");
}

void DisableVisualCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState)
{
    pSerial->WriteByte(PIC_OP_DISABLE_VISUAL);
}





void PicCommand::_CreateDisablePriority()
{
    assert(_IsEmpty());
    type = DisablePriority;
}

void DisablePriorityCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState)
{
    pState->dwDrawEnable &= ~PicScreenFlags::Priority;
}


void DisablePriorityCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf)
{
    StringCchPrintf(pszBuf, cchBuf, "Disable Priority");
}

void DisablePriorityCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState)
{
    pSerial->WriteByte(PIC_OP_DISABLE_PRIORITY);
}




void PicCommand::_CreateDisableControl()
{
    assert(_IsEmpty());
    type = DisableControl;
}

void DisableControlCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState)
{
    pState->dwDrawEnable &= ~PicScreenFlags::Control;
}

void DisableControlCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf)
{
    StringCchPrintf(pszBuf, cchBuf, "Disable Control");
}

void DisableControlCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState)
{
    pSerial->WriteByte(PIC_OP_DISABLE_CONTROL);
}


//
// PALETTES
//
void PicCommand::_CreateSetPalette(uint8_t bPaletteNumber, const EGACOLOR *pPalette)
{
    assert(_IsEmpty());
    type = SetPalette;
    setPalette.bPaletteNumber = bPaletteNumber;
    setPalette.pPalette = new EGACOLOR[PALETTE_SIZE];
    assert(sizeof(uint8_t) == sizeof(EGACOLOR));
    if (setPalette.pPalette)
    {
        // If we're experiencing memory stress, then we won't set the palette
        // here, and the pic will be corrupted. Oh well.
        CopyMemory(setPalette.pPalette, pPalette, PALETTE_SIZE);
    }
}

void SetPaletteCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState)
{
    EGACOLOR *pCurrentPalette = GET_PALETTE(pState->pPalettes, pCommand->setPalette.bPaletteNumber);
    if (pCommand->setPalette.pPalette)
    {
        // Could have failed due to memory stress
        CopyMemory(pCurrentPalette, pCommand->setPalette.pPalette, PALETTE_SIZE);

        if (pCommand->setPalette.bPaletteNumber == pState->bPaletteNumber)
        {
            // It's possible that the current color being used was changed by the palette modification
            // just made.
            pState->egaColor = pCurrentPalette[pState->bPaletteOffset];
        }
    }
}

void SetPaletteCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf)
{
    StringCchPrintf(pszBuf, cchBuf, "Set palette: %d", pCommand->setPalette.bPaletteNumber);
}

void SetPaletteCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState)
{
    // PIC_OP_OPX | PIC_OPX_SET_PALETTE | bPaletteNum | pPalette
    pSerial->WriteByte(PIC_OP_OPX);
    pSerial->WriteByte(PIC_OPX_SET_PALETTE);

    pSerial->WriteByte(pCommand->setPalette.bPaletteNumber);
    pSerial->WriteBytes((uint8_t*)pCommand->setPalette.pPalette, 40);
}


void PicCommand::_CreateSetPaletteEntry(uint8_t bPaletteNumber, uint8_t bOffset, EGACOLOR color)
{
    type = SetPaletteEntry;
    assert(bPaletteNumber < 4);
    setPaletteEntry.bPaletteNumber = bPaletteNumber;
    assert(bOffset < PALETTE_SIZE);
    setPaletteEntry.bOffset = bOffset;
    setPaletteEntry.color = color;
}

void SetPaletteEntryCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState)
{
    EGACOLOR *pPalette = GET_PALETTE(pState->pPalettes, pCommand->setPaletteEntry.bPaletteNumber);
    pPalette[pCommand->setPaletteEntry.bOffset] = pCommand->setPaletteEntry.color;

    if (pCommand->setPaletteEntry.bPaletteNumber == pState->bPaletteNumber)
    {
        // It's possible that the current color being used was changed by the palette modification
        // just made.
        // ACTUALLY - SCI DOES NOT DO THIS.
        /*
        if (pCommand->setPaletteEntry.bOffset == pState->bPaletteOffset)
        {
            // Yup.  Update the color variable in ViewPort.
            pState->color = pPalette[pState->bPaletteOffset];
        }*/
    }

    if (pCommand->setPaletteEntry.bPaletteNumber == 0)
    {
        // Lock this color in palette 0.
        pState->rgLocked[pCommand->setPaletteEntry.bOffset] = 0xff;
    }
}

void SetPaletteEntryCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf)
{
    StringCchPrintf(pszBuf, cchBuf, "Set palette entry: %d,%d", pCommand->setPaletteEntry.bPaletteNumber, pCommand->setPaletteEntry.bOffset);
}

void SetPaletteEntryCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState)
{
    // PIC_OP_OPX | PIC_OPX_SET_PALETTE_ENTRY | [ (bPaletteNum,offset), color] * n
    if (!pCommandPrev || (pCommandPrev->type != pCommand->type))
    {
        pSerial->WriteByte(PIC_OP_OPX);
        pSerial->WriteByte(PIC_OPX_SET_PALETTE_ENTRY);
        // The optimization here is that we don't write the opcodes if the last command was the same
    }
    pSerial->WriteByte(BYTE_FROM_PALETTE_AND_OFFSET(pCommand->setPaletteEntry.bPaletteNumber, pCommand->setPaletteEntry.bOffset));
    pSerial->WriteByte(BYTE_FROM_EGACOLOR(pCommand->setPaletteEntry.color));
}


void PicCommand::CreateSetPriorityBars(const uint16_t *pBars, bool is16Bit, bool isVGA)
{
    assert(_IsEmpty());
    type = SetPriorityBars;
    setPriorityBars.pPriorityLines = new uint16_t[NumPriorityBars];
    setPriorityBars.is16Bit = is16Bit;
    setPriorityBars.isVGA = isVGA;
    CopyMemory(setPriorityBars.pPriorityLines, pBars, NumPriorityBars * sizeof(*setPriorityBars.pPriorityLines));
}

void SetPriorityBarsCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState)
{
    // Just transfer the data? 
    memcpy(pState->bPriorityLines, pCommand->setPriorityBars.pPriorityLines, sizeof(pState->bPriorityLines));
}

void SetPriorityBarsCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf)
{
    StringCchPrintf(pszBuf, cchBuf, "Set priority bars");
}

void SetPriorityBarsCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState)
{
    if (pCommand->setPriorityBars.is16Bit)
    {
        assert(false && "This is SCI1.1, we shouldn't reach here.");
        pSerial->WriteBytes((const uint8_t *)pCommand->setPriorityBars.pPriorityLines, NumPriorityBars * sizeof(*pCommand->setPriorityBars.pPriorityLines));
    }
    else
    {
        pSerial->WriteByte(0xfe);   // special opcode
        // The opcode is different in EGA vs VGA
        pSerial->WriteByte(pCommand->setPriorityBars.isVGA ? 0x4 : 0x8);
        for (int i = 0; i < NumPriorityBars; i++)
        {
            pSerial->WriteByte((uint8_t)pCommand->setPriorityBars.pPriorityLines[i]);
        }
    }
}

//
// REVIEW: This algorithm doesn't seem to work (or else our decompression is wrong)
//
void PicCommand::CreateDrawVisualBitmap(const Cel &cel, bool isVGA)
{
    assert(_IsEmpty());
    type = DrawBitmap;
    drawVisualBitmap.pCel = new Cel(cel);
    drawVisualBitmap.isVGA = isVGA;
}

void DrawVisualBitmap_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState)
{
    if (pCommand->drawVisualBitmap.pCel)
    {
        size16 displaySize = pData->size;
        Cel &cel = *pCommand->drawVisualBitmap.pCel;
        // Optimization
#if CANT_DO_BECAUSE_OF_TRANS_COLOR
        if ((cel.size.cx == pData->size.cx) && (cel.size.cy == pData->size.cy))
        {
            // Just bit copy everything
            memcpy(pData->pdataVisual, &cel.Data[0], CX_ACTUAL(cel.size.cx) * cel.size.cy);
        }
        else
#endif
        {
            // Copy line by line.
            DrawImageWithPriority(
                displaySize,
                pData->pdataVisual,
                pData->pdataPriority,
                pState->bPriorityValue,
                cel.placement.x,
                cel.placement.y,
                cel.size.cx,
                cel.size.cy,
                &cel.Data[0],
                cel.TransparentColor,
                false);
        }

        // Fill in the aux thing (otherwise stuff like LSL6, pic 320 is wrong).
        for (int y = max(0, cel.placement.y); y < min(pData->size.cy, (cel.placement.y + cel.size.cy)); y++)
        {
            for (int x = max(0, cel.placement.x); x < min(pData->size.cx, (cel.placement.x + cel.size.cx)); x++)
            {
                pData->pdataAux[BUFFEROFFSET_NONSTD(displaySize.cx, displaySize.cy, x, y)] |= (uint8_t)PicScreenFlags::Visual;
            }
        }

    }
}

void DrawVisualBitmap_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf)
{
    StringCchPrintf(pszBuf, cchBuf, "Image (%dx%d),T:%d", pCommand->drawVisualBitmap.pCel->size.cx, pCommand->drawVisualBitmap.pCel->size.cy, pCommand->drawVisualBitmap.pCel->TransparentColor);
}

void DrawVisualBitmap_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState)
{
    pSerial->WriteByte(0xfe); // Special opcode
    // The opcode is different in VGA vs EGA
    pSerial->WriteByte(pCommand->drawVisualBitmap.isVGA ? 0x1 : 0x7);

    Cel *pCel = pCommand->drawVisualBitmap.pCel;
    _WriteAbsCoordinate(pSerial, pCel->placement.x, pCel->placement.y); // REVIEW: This means no -ve?
    uint32_t currentOffset = pSerial->tellp(); // Store this place, because we'll need to write the size in.
    pSerial->WriteWord(0);
    pSerial->WriteWord(pCel->size.cx);
    pSerial->WriteWord(pCel->size.cy);
    pSerial->WriteWord(0);  // Skip 2 (unknown)
    pSerial->WriteByte(pCel->TransparentColor);
    WriteImageData(*pSerial, *pCel, pCommand->drawVisualBitmap.isVGA);
    // Write the size back at the beginning.
    uint32_t finalSize = pSerial->tellp() - currentOffset - sizeof(uint16_t);
    //if (finalSize > (std::numeric_limits<uint16_t>::max()))
    if (finalSize > 0xffff)
    {
        throw std::exception("Resource too large");
    }
    *(reinterpret_cast<uint16_t*>(pSerial->GetInternalPointer() + currentOffset)) = (uint16_t)finalSize;
}


//
// Circle command
//
void PicCommand::CreateCircle(uint16_t xFrom, uint16_t yFrom, uint16_t xTo, uint16_t yTo)
{
    assert(_IsEmpty());
    type = Circle;
    circle.xFrom = xFrom;
    circle.yFrom = yFrom;
    circle.xTo = xTo;
    circle.yTo = yTo;
}

void CircleCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState)
{
    CircleCommand_DrawOnly(pCommand, pData, pState);
}

//
// Helper that constructs a series of line commands to simulate a circle.
//
void _CircleHelper(const PicCommand *pCommand, std::vector<PicCommand> &temp)
{
    float x1 = static_cast<float>(pCommand->circle.xFrom);
    float x2 = static_cast<float>(pCommand->circle.xTo);
    float y1 = static_cast<float>(pCommand->circle.yFrom);
    float y2 = static_cast<float>(pCommand->circle.yTo);
    float PI = 3.1515926538f;
    float t, a, b, tinc, centx, centy;
    a = abs(0.5f * (x2 - x1));
    b = abs(0.5f * (y2 - y1));
    tinc = PI * 2.0f / (a + b);
    temp.reserve(static_cast<size_t>((a + b)) + 1);
    centx = ((x1 + x2) + 0.5f) * 0.5f;
    centy = ((y1 + y2) + 0.5f) * 0.5f;

    float xOrig = centx + a;
    float yOrig = centy;
    float xFrom = xOrig;
    float yFrom = yOrig;
    for(t = 0; t < PI * 2.0f; t += tinc)
    {
        float xTo = centx + a * cos(t);
        float yTo = centy - b * sin(t);
        temp.push_back(PicCommand::CreateLine(static_cast<uint16_t>(xFrom), static_cast<uint16_t>(yFrom), static_cast<uint16_t>(xTo), static_cast<uint16_t>(yTo)));
        xFrom = xTo;
        yFrom = yTo;
    }
    // Close it off...
    temp.push_back(PicCommand::CreateLine(static_cast<uint16_t>(xFrom), static_cast<uint16_t>(yFrom), static_cast<uint16_t>(xOrig), static_cast<uint16_t>(yOrig)));
}

void CircleCommand_DrawOnly(const PicCommand *pCommand, PicData *pData, const ViewPort *pState)
{
    std::vector<PicCommand> temp;
    _CircleHelper(pCommand, temp);
    size_t cTemps = temp.size();
    for (size_t i = 0; i < cTemps; i++)
    {
        PicCommand &command = temp[i];
        LineCommand_DrawOnly(&command, pData, pState);
    }
}

void CircleCommand_GetName(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf)
{
    StringCchPrintf(pszBuf, cchBuf, "Circle: %dx%d - %dx%d", pCommand->circle.xFrom, pCommand->circle.yFrom, pCommand->circle.xTo, pCommand->circle.yTo);
}
void CircleCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState)
{
    std::vector<PicCommand> temp;
    _CircleHelper(pCommand, temp);

    PicCommand commandPrev;
    size_t cTemps = temp.size();
    *pds = dsPrev;
    for (size_t i = 0; i < cTemps; i++)
    {
        const PicCommand &command = temp[i];
        Command_Serialize(pSerial, &command, (i == 0) ? nullptr : &commandPrev, (i == (cTemps - 1)) ? pCommandNext : &temp[i + 1], *pds, pds, pState);
        commandPrev = command;
    }
}






//
// Choose a nice color for commands to show them in the list.
//
COLORREF Command_GetColor(const PicCommand *pCommand)
{
    // By default, things are black.
    COLORREF color;
    switch (pCommand->type)
    {
    case PicCommand::SetPalette:
    case PicCommand::SetPaletteEntry:
        color = RGB(180, 0, 0);
        break;
    case PicCommand::SetVisual:
    case PicCommand::SetPriority:
    case PicCommand::SetControl:
        color = RGB(0, 0, 180);
        break;
    case PicCommand::DisableVisual:
    case PicCommand::DisablePriority:
    case PicCommand::DisableControl:
        color = RGB(128, 128, 128);
        break;

    case PicCommand::Fill:
        color = RGB(180, 0, 180);
        break;

    case PicCommand::Line:
        color = RGB(0, 180, 0);
        break;

    default:
        color = RGB(0, 0, 0);
        break;
    }
    return color;
}

PicCommand::PicCommand()
{
    memset(this, 0, sizeof(PicCommand));
    type = None;
}

// Do a deep copy of certain commands
void PicCommand::_MaybeDeepCopy(const PicCommand &src)
{
    if (src.type == SetPalette)
    {
        // Reallocate the palette
        setPalette.pPalette = new EGACOLOR[PALETTE_SIZE];
        memcpy(setPalette.pPalette, src.setPalette.pPalette, sizeof(EGACOLOR) * PALETTE_SIZE);
    }
    else if (src.type == DrawBitmap)
    {
        drawVisualBitmap.pCel = new Cel(*src.drawVisualBitmap.pCel);
    }
    else if (src.type == SetPriorityBars)
    {
        setPriorityBars.pPriorityLines = new uint16_t[NumPriorityBars];
        memcpy(setPriorityBars.pPriorityLines, src.setPriorityBars.pPriorityLines, NumPriorityBars * sizeof(*setPriorityBars.pPriorityLines));
    }
}

// Copy constructor
PicCommand::PicCommand(const PicCommand& src)
{
    memcpy(this, &src, sizeof(PicCommand));
    _MaybeDeepCopy(src);
}

// For deserialization from clipboard:
bool PicCommand::Initialize(sci::istream &byteStream)
{
    _CleanUp();
    memset(this, 0, sizeof(PicCommand));
    byteStream >> *this;
    if (byteStream.good())
    {
        assert(type <= CommandTypeMax); // That we have a valid command!
        if (type == SetPalette)
        {
            // Allocate the palette
            setPalette.pPalette = new EGACOLOR[PALETTE_SIZE];
            byteStream.read_data((uint8_t*)setPalette.pPalette, sizeof(*setPalette.pPalette) * PALETTE_SIZE);
        }
        else if (type == SetPriorityBars)
        {
            setPriorityBars.pPriorityLines = new uint16_t[NumPriorityBars];
            byteStream.read_data((uint8_t*)setPriorityBars.pPriorityLines, sizeof(*setPriorityBars.pPriorityLines) * NumPriorityBars);
        }
        else if (type == DrawBitmap)
        {
            drawVisualBitmap.pCel = new Cel();
            DeserializeCelRuntime(byteStream, *drawVisualBitmap.pCel);
        }
    }
    return byteStream.good();
}

// Serialization for clipboard
void PicCommand::SerializeForClipboard(sci::ostream *pSerial) const
{
    pSerial->WriteBytes((uint8_t*)this, sizeof(PicCommand));
    if (type == SetPalette)
    {
        pSerial->WriteBytes((uint8_t*)setPalette.pPalette, sizeof(*setPalette.pPalette) * PALETTE_SIZE);
    }
    else if (type == SetPriorityBars)
    {
        pSerial->WriteBytes((uint8_t*)setPriorityBars.pPriorityLines, sizeof(*setPriorityBars.pPriorityLines) * NumPriorityBars);
    }
    else if (type == DrawBitmap)
    {
        SerializeCelRuntime(*pSerial, *drawVisualBitmap.pCel);
    }
}

PicCommand::~PicCommand()
{
    _CleanUp();
}

PicCommand& PicCommand::operator=(const PicCommand& src)
{
    if (this != &src)
    {
        _CleanUp(); // Delete any existing data.
        memcpy(this, &src, sizeof(PicCommand));
        _MaybeDeepCopy(src);
    }
    return(*this);
}

bool PicCommand::_IsEmpty()
{
    // Check some things that would indicate this command is empty
    return (type == None) && (setPalette.pPalette == nullptr);
}

void PicCommand::_CleanUp()
{
    // Clean up any allocated data
    switch (type)
    {
    case SetPalette:
        delete[] setPalette.pPalette;
        setPalette.pPalette = nullptr;
        break;
    case SetPriorityBars:
        delete[] setPriorityBars.pPriorityLines;
        setPriorityBars.pPriorityLines = nullptr;
        break;
    case DrawBitmap:
        delete drawVisualBitmap.pCel;
        drawVisualBitmap.pCel = nullptr;
        break;
    }
    type = None; // Set command type to none
}

bool operator==(const PenStyle &one, const PenStyle &two)
{
    return one.fPattern == two.fPattern &&
        one.fRectangle == two.fRectangle &&
        one.fRandomNR == two.fRandomNR &&
        one.bPatternSize == two.bPatternSize &&
        one.bPatternNR == two.bPatternNR;
}

bool operator!=(const PenStyle &one, const PenStyle &two)
{
    return !(one == two);
}