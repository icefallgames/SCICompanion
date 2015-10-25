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

//
// Pic Command classes
//
#pragma once

namespace sci
{
    class istream;
}

#include "PicCommandsCommon.h"
#include "Components.h" // Cel

// fwd decl
class ResourceEntity;

#define BUFFEROFFSET_NONSTD(cx, cy, x, y) (((cy) - 1) -(y))*cx+(x)

// Pic opcodes
const uint8_t PIC_OP_SET_COLOR = 0xf0;
const uint8_t  PIC_OP_DISABLE_VISUAL = 0xf1;
const uint8_t  PIC_OP_SET_PRIORITY = 0xf2;
const uint8_t  PIC_OP_DISABLE_PRIORITY = 0xf3;
const uint8_t  PIC_OP_RELATIVE_PATTERNS = 0xf4;
const uint8_t  PIC_OP_RELATIVE_MEDIUM_LINES = 0xf5;
const uint8_t  PIC_OP_RELATIVE_LONG_LINES = 0xf6;
const uint8_t  PIC_OP_RELATIVE_SHORT_LINES = 0xf7;
const uint8_t  PIC_OP_FILL = 0xf8;
const uint8_t  PIC_OP_SET_PATTERN = 0xf9;
const uint8_t  PIC_OP_ABSOLUTE_PATTERNS = 0xfa;
const uint8_t  PIC_OP_SET_CONTROL = 0xfb;
const uint8_t  PIC_OP_DISABLE_CONTROL = 0xfc;
const uint8_t  PIC_OP_RELATIVE_MEDIUM_PATTERNS = 0xfd;
const uint8_t  PIC_OP_OPX = 0xfe;
const uint8_t  PIC_END = 0xff;

const uint8_t  PIC_OPX_SET_PALETTE_ENTRY = 0x00;
const uint8_t  PIC_OPX_SET_PALETTE = 0x01;

const uint8_t  PIC_OPXSC1_DRAW_BITMAP = 0x01;
const uint8_t  PIC_OPXSC1_SET_PALETTE = 0x02;
const uint8_t  PIC_OPXSC1_SET_PRIORITY_BARS = 0x04;

static const int NumPriorityBars = 14;

typedef void (CALLBACK *PFNCLOSECOORDCALLBACK)(void *, CDC *, int, int, int, int);

//
// This class maintains the state of a picture, that PicCommands
// might want to change (e.g. current color, priority, etc...)
//
class ViewPort
{
public:
    ViewPort(uint8_t bPaletteToUse);
    ViewPort(const ViewPort& src) = default;
    ViewPort();
    ViewPort& operator=(const ViewPort& src) = default;

    void Reset(uint8_t bPaletteToUse);

    PicScreenFlags dwDrawEnable;
    uint8_t bPriorityValue;
    uint8_t bControlValue;
    EGACOLOR egaColor;

    // The following are just used for UI state.
    uint8_t bPaletteNumber;// new: which palette the color belongs to
    uint8_t bPaletteOffset;// new: which index in that palette that the color belongs to (or the actual index in VGA)

    uint8_t bPatternNR;
    uint8_t bPatternSize;
    EGACOLOR pPalettes[160]; // 40 times 4 bytes

    uint8_t bPaletteToDraw; // Global palette with which to draw
    uint8_t rgLocked[40];   // Whether or not the colours in palette 0 are locked.

    uint16_t bPriorityLines[NumPriorityBars];
};



struct PicData
{
    PicScreenFlags dwMapsToRedraw;
    uint8_t *pdataVisual;
    uint8_t *pdataPriority;
    uint8_t *pdataControl;
    uint8_t *pdataAux;
    bool isVGA;
    size16 size;
};

//
// This is specifically for serialization
//
struct SerializedPicState
{
    bool fInited;
    uint8_t bPatternSize;
    uint8_t wFlags;
};

struct sRECT
{
    int16_t left;
    int16_t top;
    int16_t right;
    int16_t bottom;
};

bool PtInSRect(sRECT *prc, int16_t x, int16_t y);

//
// COMMAND structures
//
struct LINECOMMAND
{
    int16_t xFrom;
    int16_t yFrom;
    int16_t xTo;
    int16_t yTo;
}; // 8 bytes

struct CIRCLECOMMAND
{
    int16_t xFrom;
    int16_t yFrom;
    int16_t xTo;
    int16_t yTo;
}; // 8 bytes

struct PATTERNCOMMAND
{
    int16_t x;
    int16_t y;
    uint8_t bPatternSize;
    uint8_t bPatternNR;
    uint8_t wFlags; // 
}; // 8 bytes

struct FILLCOMMAND
{
    int16_t x;
    int16_t y;
}; // 4 bytes

struct SETVISUALCOMMAND
{
    uint8_t bColor;
    bool isVGA;
    // For EGA:
    uint8_t GetPaletteNumber() const { assert(!isVGA);  return (GET_PALDEX(bColor)); }
    uint8_t GetPaletteIndex() const{ assert(!isVGA);  return (GET_PALCOL(bColor)); }
}; // 2 bytes

struct SETPRIORITYCOMMAND
{
    uint8_t bPriorityValue;
}; // 1 byte

struct SETCONTROLCOMMAND
{
    uint8_t bControlValue;
}; // 1 byte

struct SETPALETTECOMMAND
{
    // So as not to bloat all commands, we allocate the palette here,
    // instead of having an embedded array.
    EGACOLOR *pPalette;
    uint8_t bPaletteNumber;
}; // 8 bytes (packed)

struct SETPALETTEENTRYCOMMAND
{
    EGACOLOR color;
    uint8_t bPaletteNumber;
    uint8_t bOffset;
}; // 4 bytes (packed)

struct SETVISUALCOMMAND_SCI1
{
    uint8_t bPaletteIndex;
}; // 1 byte

struct SETPRIORITYBARSCOMMAND_SCI1
{
    uint16_t *pPriorityLines;
    bool is16Bit;
    bool isVGA;
}; // 8 bytes

struct DRAWVISUALBITMAPCOMMAND_SCI1
{
    Cel *pCel;
    bool mirrored;
    bool isVGA;
};  // 8 bytes

#define SCI1_PALETTE_LENGTH 256


//
// PicCommand
// This class represents the commands that make up an SCI picture.
//
// Due to the need for efficiency in manipulating thousands of commands in a single
// picture, these objects generally don't get allocated.  e.g. we have arrays of PicCommand, not
// PicCommand*, as the memory allocations hurt performance.
//
// So the PicCommand class contains a union of all the possible data required for a
// command.  However, the setPalette command is unique in that it requires much more
// data (a whole palette).  So we allocate a palette in this object (palette commands
// are infrequent, so perf is not an issue).  Since we have an allocated object here, we
// need a destructor, copy constructor, and an = operator overload.
//
class PicCommand
{
public:

    //
    // command types
    // 
    // These serve as indexes into function pointer arrays
    enum CommandType : uint8_t
    {
        Line =           0x00000000,
        Pattern =        0x00000001,
        Fill =           0x00000002,
        SetVisual =      0x00000003,
        SetPriority =    0x00000004,
        SetControl =     0x00000005,
        DisableVisual =  0x00000006,
        DisablePriority =0x00000007,
        DisableControl = 0x00000008,
        SetPalette =     0x00000009,
        SetPaletteEntry =0x0000000a,
        SetPriorityBars =0x0000000b,
        DrawBitmap =     0x0000000c,
        Circle =         0x0000000d,
        CommandTypeMax = 0x0000000d,
        None =           0xff, // Just for convenience
    };

    PicCommand();
    PicCommand(const PicCommand& src);
    // For deserialization from clipboard:
    bool Initialize(sci::istream &byteStream);
    // Serialization for clipboard
    void SerializeForClipboard(sci::ostream *pSerial) const;
    ~PicCommand();
    PicCommand& operator=(const PicCommand& src);

    void _MaybeDeepCopy(const PicCommand &src);

    // We achieve our polymorphism via unions. This avoids us having to allocate
    // a chunk of memory for each command (only certain commands with extra data
    // require it).
    // The down size is that we need to implement our own custom assignment and copy constructors.
    union 
    {
        LINECOMMAND drawLine;
        CIRCLECOMMAND circle;
        PATTERNCOMMAND drawPattern;
        FILLCOMMAND fill;
        SETVISUALCOMMAND setVisual;
        SETPRIORITYCOMMAND setPriority;
        SETCONTROLCOMMAND setControl;
        SETPALETTECOMMAND setPalette;
        SETPALETTEENTRYCOMMAND setPaletteEntry;
        SETPRIORITYBARSCOMMAND_SCI1 setPriorityBars;
        DRAWVISUALBITMAPCOMMAND_SCI1 drawVisualBitmap;
    };

    CommandType type;

    // Create functions
    static PicCommand CreateLine(int16_t xFrom, int16_t yFrom, int16_t xTo, int16_t yTo)
    {
        PicCommand command;
        command._CreateLine(xFrom, yFrom, xTo, yTo);
        return command;
    }
    static PicCommand CreatePattern(int16_t x, int16_t y, uint8_t bPatternSize, uint8_t bPatternNR, bool fPattern, bool fRectangle)
    {
        PicCommand command;
        command._CreatePattern(x, y, bPatternSize, bPatternNR, fPattern, fRectangle);
        return command;
    }
    static PicCommand CreateFill(int16_t x, int16_t y)
    {
        PicCommand command;
        command._CreateFill(x, y);
        return command;
    }
    static PicCommand CreateSetVisual(uint8_t bPaletteNumber, uint8_t bPaletteIndex)
    {
        PicCommand command;
        command._CreateSetVisual(bPaletteNumber, bPaletteIndex);
        return command;
    }
    static PicCommand CreateSetVisual(uint8_t bColor)
    {
        PicCommand command;
        command._CreateSetVisualVGA(bColor);
        return command;
    }
    static PicCommand CreateSetPriority(uint8_t bPriorityValue)
    {
        PicCommand command;
        command._CreateSetPriority(bPriorityValue);
        return command;
    }
    static PicCommand CreateSetControl(uint8_t bControlValue)
    {
        PicCommand command;
        command._CreateSetControl(bControlValue);
        return command;
    }
    static PicCommand CreateDisableVisual()
    {
        PicCommand command;
        command._CreateDisableVisual();
        return command;
    }
    static PicCommand CreateDisablePriority()
    {
        PicCommand command;
        command._CreateDisablePriority();
        return command;
    }
    static PicCommand CreateDisableControl()
    {
        PicCommand command;
        command._CreateDisableControl();
        return command;
    }
    static PicCommand CreateSetPalette(uint8_t bPaletteNumber, const EGACOLOR *pPalette)
    {
        PicCommand command;
        command._CreateSetPalette(bPaletteNumber, pPalette);
        return command;
    }
    static PicCommand CreateSetPaletteEntry(uint8_t bPaletteNumber, uint8_t bOffset, EGACOLOR color)
    {
        PicCommand command;
        command._CreateSetPaletteEntry(bPaletteNumber, bOffset, color);
        return command;
    }
    void CreateSetPriorityBars(const uint16_t *pBars, bool is16Bit, bool isVGA);
    void CreateDrawVisualBitmap(const Cel &cel, bool isVGA);
    void CreateCircle(int16_t xFrom, int16_t yFrom, int16_t xTo, int16_t yTo);

    // Drawing functions, etc...
    void Draw(PicData *pData, ViewPort &state) const;
    void GetName(TCHAR *pszBuf, size_t cchBuf) const;

private:
    void _CreateLine(int16_t xFrom, int16_t yFrom, int16_t xTo, int16_t yTo);
    void _CreatePattern(int16_t x, int16_t y, uint8_t bPatternSize, uint8_t bPatternNR, bool fPattern, bool fRectangle);
    void _CreateFill(int16_t x, int16_t y);
    void _CreateSetVisual(uint8_t bPaletteNumber, uint8_t bPaletteIndex);
    void _CreateSetVisualVGA(uint8_t bColor);
    void _CreateSetPriority(uint8_t bPriorityValue);
    void _CreateSetControl(uint8_t bControlValue);
    void _CreateDisableVisual();
    void _CreateDisablePriority();
    void _CreateDisableControl();
    void _CreateSetPalette(uint8_t bPaletteNumber, const EGACOLOR *pPalette);
    void _CreateSetPaletteEntry(uint8_t bPaletteNumber, uint8_t bOffset, EGACOLOR color);
    void _CreateCircle(int16_t xFrom, int16_t yFrom, int16_t xTo, int16_t yTo);

    bool _IsEmpty();
    void _CleanUp();
};

void PatternInfoFromIndex(uint8_t bIndex, PenStyle *pPenStyle);
uint8_t IndexFromPatternInfo(const PenStyle *pPenStyle);

//
// Specifies the 3 types of x,y encoding for drawing commands
//
enum DRAWSIZE
{
    DS_SMALL     = 0,   // x,y in one byte  (max 8,8)
    DS_MEDIUM    = 1,   // x,y in two bytes (max 128,111)
    DS_LARGE     = 2,   // x,y in three bytes (max 319, 189)
};

//
// Draw functions
//
void LineCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState);
void LineCommand_DrawOnly(const PicCommand *pCommand, PicData *pData, const ViewPort *pState);
void PatternCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState);
void PatternCommand_Draw_DrawOnly(const PicCommand *pCommand, PicData *pData, const ViewPort *pState);
void PatternCommand_Draw_StateOnly(const PicCommand *pCommand, PicData *pData, ViewPort *pState);
void FillCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState);
void SetVisualCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState);
void SetPriorityCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState);
void SetControlCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState);
void DisableVisualCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState);
void DisablePriorityCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState);
void DisableControlCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState);
void SetPaletteCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState);
void SetPaletteEntryCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState);
void SetPriorityBarsCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState);
void DrawVisualBitmap_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState);
void CircleCommand_Draw(const PicCommand *pCommand, PicData *pData, ViewPort *pState);
void CircleCommand_DrawOnly(const PicCommand *pCommand, PicData *pData, const ViewPort *pState);

//
// Fancy pants stuff
//
COLORREF Command_GetColor(const PicCommand *pCommand);


//
// Serialize functions
//
// pCommandPrev and pCommandNext are optional parameters.
// dsPrev is only valid if there is a pCommandPrev of the same type.
//
void LineCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState);
void PatternCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState);
void FillCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState);
void SetVisualCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState);
void SetPriorityCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState);
void SetControlCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState);
void DisableVisualCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState);
void DisablePriorityCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState);
void DisableControlCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState);
void SetPaletteCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState);
void SetPaletteEntryCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState);
void SetVisualCommandSCI1_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState);
void SetPriorityBarsCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState);
void DrawVisualBitmap_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState);
void CircleCommand_Serialize(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState);

void SerializeAllCommands(sci::ostream *pSerial, const std::vector<PicCommand> &commands, size_t cCommands);

//
// Special one for post-processing in the image.
//
void LineCommand_DrawOverride(PicCommand *pCommand, PicData *pData, EGACOLOR color, uint8_t bPriorityValue, uint8_t bControlValue, PicScreenFlags dwDrawEnable);
void LineCommand_DrawEx(int cx, int cy, PicCommand *pCommand, uint8_t *pData, EGACOLOR color);

// Serialize functions (to be added later)

struct PICCOMMAND_ADJUST
{
    RECT rcBounds; // Original bounds
    RECT rcNew;
    bool fVFlip;    // Flip vertical
    bool fHFlip;    // Flip horizontal
    int iAngle;
};

void PastedCommands_GetBounds(size16 displaySize, const PicCommand *pCommand, size_t cCommands, sRECT *prc);
void Command_DrawWithOffset(const PicCommand *pCommandIn, PicData *pData, ViewPort *pState, const PICCOMMAND_ADJUST *pAdjust);
void PastedCommands_Adjust(size16 displaySize, std::vector<PicCommand> &commandsIn, const PICCOMMAND_ADJUST *pAdjust);
HGLOBAL CopiedCommands_AllocAndFillMemory(const PicCommand *pCommands, size_t cCommands);
bool PastedCommands_ContainDrawCommands(const PicCommand *pCommands, size_t cCommands);

typedef void (*DRAW_FUNCTION)(const PicCommand *pCommand, PicData *pData, ViewPort *pState);
typedef void (*GETNAME_FUNCTION)(const PicCommand *pCommand, TCHAR *pszBuf, size_t cchBuf);
typedef void (*SERIALIZE_FUNCTION)(sci::ostream *pSerial, const PicCommand *pCommand, const PicCommand *pCommandPrev, const PicCommand *pCommandNext, DRAWSIZE dsPrev, DRAWSIZE *pds, SerializedPicState *pState);

const uint8_t g_defaultPaletteInit[] = 
{ (0), (0x11), (0x22), (0x33), (0x44), (0x55), (0x66), (0x77),
      (0x88), (0x99), (0xaa), (0xbb), (0xcc), (0xdd), (0xee), (0x88),
      (0x88), (0x01), (0x02), (0x03), (0x04), (0x05), (0x06), (0x88),
      (0x88), (0xf9), (0xfa), (0xfb), (0xfc), (0xfd), (0xfe), (0xff),
      (0x08), (0x91), (0x2a), (0x3b), (0x4c), (0x5d), (0x6e), (0x88) };

extern const EGACOLOR *g_defaultPalette;

void InitDitherCritSec();
void DeleteDitherCritSec();

//
// For the fake ego feature.
//
void DrawBoxWithPriority(size16 picSize, uint8_t *pdataDisplay, const uint8_t *pdataPriority, uint8_t bEgoPriority, int16_t x, int16_t y, uint16_t cx, uint16_t cy);
void DrawViewWithPriority(size16 displaySize, uint8_t *pdataDisplay, const uint8_t *pdataPriority, uint8_t bEgoPriority, int16_t xIn, int16_t yIn, const ResourceEntity *pvr, int nLoop, int nCel, bool fShowOutline = FALSE);
bool HitTestEgoBox(int16_t xCursor, int16_t yCursor, int16_t xEgo, int16_t yEgo, uint16_t cx, uint16_t cy);
bool HitTestView(int16_t xCursor, int16_t yCursor, int16_t xEgo, int16_t yEgo, const ResourceEntity *pvr, int nLoop, int nCel);
void GetViewRect(CRect *prc, int16_t xEgo, int16_t yEgo, const ResourceEntity *pvr, int nLoop, int nCel);
CPoint FindCenterOfView(int16_t xEgo, int16_t yEgo, const ResourceEntity *pvr, int nLoop, int nCel);
CRect GetViewBoundsRect(int16_t xEgo, int16_t yEgo, const ResourceEntity *pvr, int nLoop, int nCel);
bool CanBeHere(size16 displaySize, const uint8_t *pdataPriority, const CRect &rect, uint16_t wControlMask = 0x8000);
void DrawPatternInRect(int cx, int cy, PicData *pData, int16_t x, int16_t y, EGACOLOR color, uint8_t bPriorityValue, uint8_t bControlValue, PicScreenFlags dwDrawEnable, const PenStyle *pPenStyle);

bool CreatePatternBitmap(CBitmap &bitmapOut, uint8_t patternSize, uint8_t patternNR, bool rectangle, bool pattern);

//
// Each command shall have the following:
// A way to draw a particular screen onto a hdc.
// void DrawCommand(HDC hdc, int iScreen, ViewPort *pState)
//
// A way to draw itself into a piece of bitmap data
// void DrawCommand(uint8_t *pdataVisual, uint8_t *pdataPriority, uint8_t *pdataControl, uint8_t *pdataAux, ViewPort *pState)
//
// A way to create itself from data.
// static HRESULT FromBuffer(uint8_t *pPicData, PicCommand *pPrevious, **ppCommand);
// 
// A method that returns its type
// uint8_t GetType()
//
// A way to serialize itself.
// **pNext is the next guy to call serialize on.
// If this method is called, the command knows it needs to place a new opcode
// HRESULT Serialize(uint8_t *pPicData, PicCommand *pPrevious, PicCommand **pNext)
//
// if this one is called (signature varies with type of command), then it knows it
// doesn't need to start a new opcode
// HRESULT SerializeWithHint(uint8_t *pPicData, uint16_t xPreviousTo, uint16_t yPreviousTo, PicCommand **pNext)
//
// Of course, we can't call SerializeWithHint unless we first call this, to ensure the same opcode:
// void UseSameOpcode(uint16_t xTo, uint16_t yTo, uint8_t *pOpcode)  // opcode is one of 3 things
// 
// If we see that the next command is not of the right type, then just

//


//
// Scenario 1:
// Loading from a databuffer:
// - We look at the opcode, and then instantiate the command base off of it.  We'll do the 
//   reading, and then construct the command based on the results.  i.e.
//   CDrawLinePicCommand(uint16_t xFrom, uint16_t yFrom, uint16_t xTo, uint16_t yTo);
// - Then, as we find multiple instances of the command, we will keep instantiating those.
//
// Scenario 2:
// Drawing
// We'll keep calling DrawCommand sequentially, for all the pic commands.
//
// Scenario 3:
// Persisting to buffer.
// We start with the first one, and ask it to persist itself.  It knows to persist its
// opcode, if the previous command is not of the same type.
//
//

extern uint16_t g_defaultPriBands16Bit[14];