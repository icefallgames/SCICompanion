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
#include "Polygon.h"
#include "ImageUtil.h"
#include "format.h"

using namespace std;

#define ISVALIDPALETTEINDEX(x) (x < 40)
#define MAKE_VALID_PALETTE_INDEX(x) (min((x), 39))
#define PALETTE(x) (pPalettes + x * 40)
#define CURPALETTE (pPalettes + bCurrentPalette * 40)

struct PatternInfo
{
    PatternInfo() : bPatternNR(0), bPatternCode(0), bPatternSize(0) {}
    uint8_t bPatternNR;
    uint8_t bPatternCode;
    uint8_t bPatternSize;
};

struct AbsoluteCoords
{
    uint16_t x, y;
};

struct RelativeCoords
{
    RelativeCoords(const AbsoluteCoords &absCoords) : xPrev(absCoords.x), yPrev(absCoords.y), x(0), y(0) {}
    void Next() { xPrev = x; yPrev = y; }
    uint16_t xPrev, yPrev;
    uint16_t x, y;
};

sci::istream & operator>>(sci::istream &stream, AbsoluteCoords &coords)
{
    uint8_t bCoordinatePrefix;
    stream >> bCoordinatePrefix;
    uint8_t bx, by;
    stream >> bx;
    stream >> by;
    coords.x = (uint16_t)bx;
    coords.y = (uint16_t)by;
    coords.x |= (((uint16_t)(bCoordinatePrefix & 0xf0)) << 4);
    coords.y |= (((uint16_t)(bCoordinatePrefix & 0x0f)) << 8);
    return stream;
}

sci::istream & operator>>(sci::istream &stream, RelativeCoords &coords)
{
    uint8_t bInput;
    stream >> bInput;
    coords.x = coords.xPrev;
    coords.y = coords.yPrev;
    if (bInput & 0x80)
    {
        coords.x -= ((bInput >> 4) & 0x7);
    }
    else
    {
        coords.x += ((bInput >> 4) & 0x7);
    }

    if (bInput & 0x08)
    {
        coords.y -= (bInput & 0x7);
    }
    else
    {
        coords.y += (bInput & 0x7);
    }
    return stream;
}


//
// Because we're dealing with unsigned bytes, we need to do some sepcial stuff
//
uint16_t _CalcRelativeX(uint16_t xOld, uint8_t bTemp)
{
    // Convert to unsigned
    int8_t cUnsigned = (int8_t)bTemp;
    // Then sign-extend it.
    int16_t i16Temp = (int16_t)cUnsigned;
    return ((int16_t)xOld) + i16Temp;
}

// Methods for reading pic resource data
void _ReadRelativePatterns(vector<PicCommand> &picCommands, sci::istream &stream, PatternInfo &patterns, bool supportsPenCommands)
{
    if (patterns.bPatternCode &  PATTERN_FLAG_USE_PATTERN)
    {
        uint8_t bTemp;
        stream >> bTemp;
        patterns.bPatternNR = (bTemp >> 1) & 0x7f;
    }

    AbsoluteCoords absCoords;
    stream >> absCoords;

    if (supportsPenCommands)
    {
        picCommands.push_back(PicCommand::CreatePattern(absCoords.x,
            absCoords.y,
            patterns.bPatternSize,
            patterns.bPatternNR,
            (patterns.bPatternCode & PATTERN_FLAG_USE_PATTERN) != 0,
            (patterns.bPatternCode & PATTERN_FLAG_RECTANGLE) != 0));
    }

    // Moved from below...
    RelativeCoords relCoords(absCoords);

    uint8_t bTemp;
    while (stream.has_more_data() && stream.peek(bTemp) && (bTemp < 0xf0))
    {
        if (patterns.bPatternCode & PATTERN_FLAG_USE_PATTERN)
        {
            uint8_t bTemp;
            stream >> bTemp;
            patterns.bPatternNR = (bTemp >> 1) & 0x7f;
        }
        // Oops, this shouldn't be here. Coordinates are relative from the most recent,
        // not the original.
        //RelativeCoords relCoords(absCoords);
        stream >> relCoords;
        if (supportsPenCommands)
        {
            picCommands.push_back(
                PicCommand::CreatePattern(
                relCoords.x,
                relCoords.y,
                patterns.bPatternSize,
                patterns.bPatternNR,
                (patterns.bPatternCode & PATTERN_FLAG_USE_PATTERN) != 0,
                (patterns.bPatternCode & PATTERN_FLAG_RECTANGLE) != 0));
        }
        relCoords.Next();
    }
}

void _ReadAbsolutePatterns(vector<PicCommand> &picCommands, sci::istream &stream, PatternInfo &patterns, bool supportsPenCommands)
{
    uint8_t bTemp;
    while (stream.has_more_data() && stream.peek(bTemp) && (bTemp < 0xf0))
    {
        if (patterns.bPatternCode & PATTERN_FLAG_USE_PATTERN)
        {
            stream >> bTemp;
            patterns.bPatternNR = (bTemp >> 1) & 0x7f;
        }
        AbsoluteCoords absCoords;
        stream >> absCoords;

        if (supportsPenCommands)
        {
            picCommands.push_back(
                PicCommand::CreatePattern(
                absCoords.x,
                absCoords.y,
                patterns.bPatternSize,
                patterns.bPatternNR,
                (patterns.bPatternCode & PATTERN_FLAG_USE_PATTERN) != 0,
                (patterns.bPatternCode & PATTERN_FLAG_RECTANGLE) != 0));
        }
    }

}

void _ReadRelativeMediumPatterns(vector<PicCommand> &picCommands, sci::istream &stream, PatternInfo &patterns, bool supportsPenCommands)
{
    uint8_t bTemp;
    if ((patterns.bPatternCode & PATTERN_FLAG_USE_PATTERN) != 0)
    {
        stream >> bTemp;
        patterns.bPatternNR = (bTemp >> 1) & 0x7f;
    }
    AbsoluteCoords absCoords;
    stream >> absCoords;

    if (supportsPenCommands)
    {
        picCommands.push_back(
            PicCommand::CreatePattern(
            absCoords.x,
            absCoords.y,
            patterns.bPatternSize,
            patterns.bPatternNR,
            (patterns.bPatternCode & PATTERN_FLAG_USE_PATTERN) != 0,
            (patterns.bPatternCode & PATTERN_FLAG_RECTANGLE) != 0));
    }

    while (stream.has_more_data() && stream.peek(bTemp) && (bTemp < 0xf0))
    {
        if (patterns.bPatternCode & PATTERN_FLAG_USE_PATTERN)
        {
            stream >> bTemp;
            patterns.bPatternNR = (bTemp >> 1) & 0x7f;
        }
        stream >> bTemp;
        uint16_t x, y;
        if (bTemp & 0x80)
        {
            y = absCoords.y - (bTemp & 0x7f);
        }
        else
        {
            y = absCoords.y + bTemp;
        }
        stream >> bTemp;
        x = _CalcRelativeX(absCoords.x, bTemp);

        if (supportsPenCommands)
        {
            picCommands.push_back(
                PicCommand::CreatePattern(
                x,
                y,
                patterns.bPatternSize,
                patterns.bPatternNR,
                (patterns.bPatternCode & PATTERN_FLAG_USE_PATTERN) != 0,
                (patterns.bPatternCode & PATTERN_FLAG_RECTANGLE) != 0));
        }
        // There is a bug in the SCI spec at
        // http://freesci.linuxgames.com/scihtml/x2396.html
        // Where xOld and yOld are not updated here.
        absCoords.x = x;
        absCoords.y = y;
    }
}

void _ReadRelativeMediumLines(vector<PicCommand> &picCommands, sci::istream &stream)
{
    AbsoluteCoords absCoords;
    stream >> absCoords;
    uint8_t bInput;
    while (stream.peek(bInput) && (bInput < 0xf0))
    {
        uint8_t bTemp;
        stream >> bTemp;
        uint16_t x, y;
        if (bTemp & 0x80)
        {
            y = absCoords.y - (bTemp & 0x7f);
        }
        else
        {
            y = absCoords.y + bTemp;
        }
        stream >> bTemp;
        x = _CalcRelativeX(absCoords.x, bTemp);
        picCommands.push_back(PicCommand::CreateLine(absCoords.x, absCoords.y, x, y));
        absCoords.x = x;
        absCoords.y = y;
    }
}

void _ReadRelativeLongLines(vector<PicCommand> &picCommands, sci::istream &stream)
{
    AbsoluteCoords absCoordsOne;
    stream >> absCoordsOne;
    uint8_t bInput;
    while (stream.peek(bInput) && (bInput < 0xf0))
    {
        AbsoluteCoords absCoordsTwo;
        stream >> absCoordsTwo;
        picCommands.push_back(PicCommand::CreateLine(absCoordsOne.x, absCoordsOne.y, absCoordsTwo.x, absCoordsTwo.y));
        absCoordsOne.x = absCoordsTwo.x;
        absCoordsOne.y = absCoordsTwo.y;
    }

}

void _ReadRelativeShortLines(vector<PicCommand> &picCommands, sci::istream &stream)
{
    AbsoluteCoords absCoords;
    stream >> absCoords;
    RelativeCoords relCoords(absCoords);
    uint8_t bInput;
    while (stream.peek(bInput) && (bInput < 0xf0))
    {
        stream >> relCoords;
        picCommands.push_back(PicCommand::CreateLine(relCoords.xPrev, relCoords.yPrev, relCoords.x, relCoords.y));
        relCoords.Next();
    }
}

void _ReadFill(vector<PicCommand> &picCommands, sci::istream &stream)
{
    uint8_t bTemp;
    while (stream.has_more_data() && stream.peek(bTemp) && (bTemp < 0xf0))
    {
        AbsoluteCoords absCoords;
        stream >> absCoords;
        picCommands.push_back(PicCommand::CreateFill(absCoords.x, absCoords.y));
    }

}

void _ReadSetPaletteEntry(vector<PicCommand> &picCommands, sci::istream &stream)
{
    uint8_t bTemp;
    while (stream.has_more_data() && stream.peek(bTemp) && (bTemp < 0xf0))
    {
        uint8_t bIndex, bColor;
        stream >> bIndex;
        stream >> bColor;
        uint8_t bPaletteNum = bIndex / 40;
        uint8_t bOffset = bIndex % 40;
        if (bPaletteNum < 4)
        {
            EGACOLOR color;
            // High bits are color1
            color.color1 = bColor >> 4;
            // Low bits are color 2
            color.color2 = bColor & 0xf;
            picCommands.push_back(PicCommand::CreateSetPaletteEntry(bPaletteNum, bOffset, color));
        }
        else
        {
            REPORTERROR("Bad palette number.");
        }
    }
}

void _ReadSetPalette(vector<PicCommand> &picCommands, sci::istream &stream)
{
    uint8_t bPaletteNum;
    stream >> bPaletteNum;
    EGACOLOR palette[40];
    EGACOLOR *pPalette = palette;
    for (int i = 0; i < ARRAYSIZE(palette); i++)
    {
        uint8_t bColor;
        stream >> bColor;
        EGACOLOR colorToSet;
        colorToSet.color1 = (bColor >> 4);
        colorToSet.color2 = (bColor & 0xf);
        *pPalette = colorToSet;
        pPalette++;
    }
    picCommands.push_back(PicCommand::CreateSetPalette(bPaletteNum, palette));
}


//
// Attempts to determine if this is a SCI0 or SCI1 pic
//
#define PIC_SCI0 0
#define PIC_SCI1 1
int SniffPicType(const uint8_t *pdata, size_t cbData)
{
    int iRet = PIC_SCI0;
    if (cbData >= 2)
    {
        if ((pdata[0] == 0xFE) && (pdata[1] == 0x02))
        {
            iRet = PIC_SCI1;
        }
    }
    return iRet;
}

void PicWriteTo(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag)
{
    // Write out the palette first, if any
    const PaletteComponent *pPalette = resource.TryGetComponent<PaletteComponent>();
    if (pPalette)
    {
        byteStream.WriteByte(0xfe); // Special opcode
        byteStream.WriteByte(0x02); // PIC_OPX_VGA_SET_PALETTE
        WritePalette(byteStream, *pPalette);
    }

    const PicComponent &pic = resource.GetComponent<PicComponent>();
    SerializeAllCommands_SCI0_SCI1(&byteStream, pic.commands, pic.commands.size());
}

void AddSetPriorityBarsCommand(sci::istream &byteStream, PicComponent &pic, bool is16BitPri, bool isVGA)
{
    uint16_t priBars[NumPriorityBars];
    for (int i = 0; i < ARRAYSIZE(priBars); i++)
    {
        if (is16BitPri)
        {
            byteStream >> priBars[i];
        }
        else
        {
            uint8_t priEntry;
            byteStream >> priEntry;
            priBars[i] = priEntry;
        }
    }
    pic.commands.push_back(PicCommand());
    pic.commands.back().CreateSetPriorityBars(priBars, is16BitPri, isVGA);
}


void ReadEmbeddedView(sci::istream &byteStream, PicComponent &pic, bool isVGA)
{
    AbsoluteCoords absCoords;
    byteStream >> absCoords;
    uint16_t size;
    byteStream >> size;
    DWORD finalPosition = byteStream.tellg() + size;
    Cel cel;
    byteStream >> cel.size.cx;
    byteStream >> cel.size.cy;
    byteStream.skip(2);
    byteStream >> cel.TransparentColor; // TODO: SCI1.1 ignores this and uses white always.
    byteStream.skip(1);
    cel.placement.x = absCoords.x;
    cel.placement.y = absCoords.y;
    ReadImageData(byteStream, cel, isVGA);

    pic.commands.push_back(PicCommand());
    pic.commands.back().CreateDrawVisualBitmap(cel, isVGA);

    // Should invesigate why this hits. Or does it not matter?
    // Should we just advance the thing??
    //assert(finalPosition == byteStream.tellg());
    byteStream.seekg(finalPosition);
}

void PicReadExtendedFunctionSCI1(ResourceEntity &resource, PicComponent &pic, sci::istream &byteStream)
{
    uint8_t bTemp;
    byteStream >> bTemp;

    switch (bTemp)
    {
        case 0x00:  // PIC_OPX_VGA_SET_PALETTE_ENTRIES
            assert(false && "Not implemented");
            break;

        case 0x1:   // PIC_OPX_VGA_EMBEDDED_VIEW
        {
            ReadEmbeddedView(byteStream, pic, true);
        }
        break;

        case 0x02: // PIC_OPX_VGA_SET_PALETTE
            // Add a palette component and read from it
            resource.AddComponent(move(make_unique<PaletteComponent>()));
            {
                ReadPalette(resource.GetComponent<PaletteComponent>(), byteStream);
            }
            break;

        case 0x3: // PIC_OPX_VGA_PRIORITY_TABLE_EQDIST
            uint16_t lowBand, highBand;
            byteStream >> lowBand;
            byteStream >> highBand;
            break;

        case 0x4: // PIC_OPX_VGA_PRIORITY_TABLE_EXPLICIT
            AddSetPriorityBarsCommand(byteStream, pic, false, true);
            break;

        default:
            assert(false && "Not implemented");
            break;
    }
}

void PicReadExtendedFunctionSCI0(ResourceEntity &resource, PicComponent &pic, sci::istream &byteStream)
{
    uint8_t bTemp;
    byteStream >> bTemp;
    switch (bTemp)
    {
        case 0x00: // PIC_OPX_SET_PALETTE_ENTRY
            // Not available in SCI1
            _ReadSetPaletteEntry(pic.commands, byteStream);
            break;
        case 0x01: // PIC_OPX_SET_PALETTE 
            // SCI1: draw visual bitmap
            _ReadSetPalette(pic.commands, byteStream);
            break;
        case 0x02: // PIC_OPX_MONO0 
            // assert(false); // Hits in KQ1VGA
            byteStream.skip(41);
            break;
        case 0x03: // PIC_OPX_MONO1 
        case 0x05: // PIC_OPX_MONO3 
            // assert(false); // Hits in LB
            byteStream.skip(1);
            break;
        case 0x04: // PIC_OPX_MONO2 
        case 0x06: // PIC_OPX_MONO4
            // assert(false); // Hits in LB
            break;
        case 0x07: // PIC_OPX_EMBEDDED_VIEW
            ReadEmbeddedView(byteStream, pic, false);
            break;
        case 0x08: //  PIC_OPX_SET_PRIORITY_TABLE
            AddSetPriorityBarsCommand(byteStream, pic, false, false);
            break;
    }
}

void PicReadFromSCI0_SCI1(ResourceEntity &resource, sci::istream &byteStream, bool isVGA)
{
    PicComponent &pic = resource.GetComponent<PicComponent>();
    bool supportsPenCommands = pic.Traits->SupportsPenCommands;

    // See how many bytes long the stream is:
    int cBytes = (int)byteStream.GetDataSize();
    // Because we are going to set the default size of our array to that.
    // A random sampling of SCI apps showed that the number of bytes of resource per pic command
    // was 2.95 to 4.75, with an average of around 4.  Here we'll assume 3 bytes per command, and we'll
    // set the growby size to that.  That might seem excessive, but all we really want to do is allocate
    // that much memory at first, without increasing the "current index" of the array (MFC doesn't allow
    // you to specify both an allocated size, and an array size).  We could, if we want, decrease the growby
    // size after our first allocation.
    pic.commands.reserve(cBytes / 2);

    PatternInfo patterns;
    bool fDone = false;
    while (!fDone && byteStream.good() && byteStream.has_more_data())
    {
        uint8_t bOpcode;
        byteStream >> bOpcode;
        uint8_t bColorCode;
        switch (bOpcode)
        {
            case PIC_OP_SET_COLOR:
                byteStream >> bColorCode;
                // assert(bColorCode == MAKE_VALID_PALETTE_INDEX(bColorCode));
                // MAKE_VALID_PALETTE_INDEX(bColorCode);
                if (isVGA)
                {
                    pic.commands.push_back(PicCommand::CreateSetVisual(bColorCode));
                }
                else
                {
                    pic.commands.push_back(PicCommand::CreateSetVisual(GET_PALDEX(bColorCode), GET_PALCOL(bColorCode)));
                }
                break;

            case PIC_OP_DISABLE_VISUAL:
                pic.commands.push_back(PicCommand::CreateDisableVisual());
                break;

            case PIC_OP_SET_PRIORITY:
                byteStream >> bColorCode;
                pic.commands.push_back(PicCommand::CreateSetPriority((int16_t)(bColorCode & 0xf)));
                break;

            case PIC_OP_DISABLE_PRIORITY:
                pic.commands.push_back(PicCommand::CreateDisablePriority());
                break;

            case PIC_OP_RELATIVE_PATTERNS:
                _ReadRelativePatterns(pic.commands, byteStream, patterns, supportsPenCommands);
                break;

            case PIC_OP_RELATIVE_MEDIUM_LINES:
                _ReadRelativeMediumLines(pic.commands, byteStream);
                break;

            case PIC_OP_RELATIVE_LONG_LINES:
                _ReadRelativeLongLines(pic.commands, byteStream);
                break;

            case PIC_OP_RELATIVE_SHORT_LINES:
                _ReadRelativeShortLines(pic.commands, byteStream);
                break;

            case PIC_OP_FILL:
                _ReadFill(pic.commands, byteStream);
                break;

            case PIC_OP_SET_PATTERN:
                byteStream >> patterns.bPatternCode;
                // There is no command of ours that is associated with this.
                // They are encoded into each CDrawPatternPicCommand.
                // Just make sure we keep track of these values.
                if (supportsPenCommands)
                {
                    patterns.bPatternCode &= 0x37;
                    patterns.bPatternSize = patterns.bPatternCode & 0x7;
                }
                break;

            case PIC_OP_ABSOLUTE_PATTERNS:
                _ReadAbsolutePatterns(pic.commands, byteStream, patterns, supportsPenCommands);
                break;

            case PIC_OP_SET_CONTROL:
                byteStream >> bColorCode;
                pic.commands.push_back(PicCommand::CreateSetControl(bColorCode));
                break;

            case PIC_OP_DISABLE_CONTROL:
                pic.commands.push_back(PicCommand::CreateDisableControl());
                break;

            case PIC_OP_RELATIVE_MEDIUM_PATTERNS:
                _ReadRelativeMediumPatterns(pic.commands, byteStream, patterns, supportsPenCommands);
                break;

            case PIC_OP_OPX:
                if (!isVGA)
                {
                    PicReadExtendedFunctionSCI0(resource, pic, byteStream);
                }
                else
                {
                    // Note that this only exists for SCI 1.0.
                    // SCI 1.1 doesn't appear to have any "extended functions",
                    // the palette and bitmap are set separately from the pic command opcodes.
                    // (The implication being that there can only be one bitmap per pic)
                    PicReadExtendedFunctionSCI1(resource, pic, byteStream);
                }
                break;

            case PIC_END:
            {
                fDone = true;
            }
            break;
        }
    }
    // TODO: Throw exception is bytestream not good? Or should it do it for us.
}

//
// And this for existing ones.
//
void PicReadFromEGA(ResourceEntity &resource, sci::istream &byteStream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    PicReadFromSCI0_SCI1(resource, byteStream, false);
}
void PicReadFromVGA(ResourceEntity &resource, sci::istream &byteStream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    PicReadFromSCI0_SCI1(resource, byteStream, true);
}

#include <pshpack1.h>
struct PicHeader_VGA11
{
    uint16_t headerSize;
    uint8_t numPriorities;
    uint8_t priBandCount;
    uint8_t celCount;
    uint8_t unknown2;
    uint16_t xVanish;
    uint16_t yVanish;
    uint16_t viewAngle;
    uint32_t vectorDataSize;
    uint32_t vectorDataOffset;
    uint32_t priCelOffset;
    uint32_t ctlCelOffset;
    uint32_t paletteOffset;
    uint32_t celHeaderOffset;
    uint32_t unknown8;
};

struct PicCelHeader_VGA11
{
    CelHeader_VGA11 celHeader;
    uint16_t ohFourOh1; // 0x0401
    uint32_t totalCelDataSize;
};

struct PicHeader_VGA2
{
    uint16_t headerSize;
    uint8_t celCount;
    uint8_t hasCompressedCels;
    uint16_t celHeaderSize;
    uint32_t paletteOffset;
    uint16_t width;
    uint16_t height;
};

struct PicCelHeader_VGA2
{
    size16 size;
    point16 placement;
    uint8_t transparentColor;
    uint8_t compressed;
    uint16_t dataFlags;
    uint32_t totalDataSize;
    uint32_t controlDataSize;
    uint32_t paletteOffset;
    uint32_t controlOffset;
    uint32_t colorOffset;
    uint32_t rowTableOffset;
    int16_t priority;
    point16 relativePlacement;
};

struct Temp
{
    uint16_t a;
    uint8_t  b;
    uint8_t c;
    uint16_t d;
};



#include <poppack.h>

const uint16_t DefaultViewAngle = 70;
const uint16_t DefaultXVanish = 160;
const uint16_t DefaultYVanish = 55536;

void PicReadFromVGA11(ResourceEntity &resource, sci::istream &byteStream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    PicComponent &pic = resource.GetComponent<PicComponent>();
    PicHeader_VGA11 header;
    byteStream >> header;

    assert(header.headerSize == 0x26);
    static_assert(sizeof(PicHeader_VGA11) == 0x28, "PicHeader wrong size");

    // Let's learn stuff:
    assert(header.numPriorities == 16);
    assert(header.unknown2 == 0x0);
    assert(header.xVanish == DefaultXVanish);
    assert(header.yVanish == DefaultYVanish);
    assert(header.viewAngle == DefaultViewAngle);
    assert(header.priCelOffset == 0x0);
    assert(header.ctlCelOffset == 0x0);
    assert(header.unknown8 == 0x0);

    // Priority bands are at offset 40.
    // Make a priority band set command
    byteStream.seekg(40);
    AddSetPriorityBarsCommand(byteStream, pic, true, true);

    if (header.paletteOffset)
    {
        resource.AddComponent(move(make_unique<PaletteComponent>()));
        byteStream.seekg(header.paletteOffset);
        ReadPalette(resource.GetComponent<PaletteComponent>(), byteStream);
    }

    if (header.celCount)
    {
        assert(header.celCount == 1);
        assert(header.paletteOffset != 0);
        Cel celTemp;
        byteStream.seekg(header.celHeaderOffset);
        ReadCelFromVGA11(byteStream, celTemp, true);
        // "plug it in" to our system by making a drawing command for it, just like SCI 1.0 VGA would do
        // SCI 1.1 pics can be 200 pixels high:
        pic.Size.cy = max(DEFAULT_PIC_HEIGHT, celTemp.size.cy);
        pic.Size.cy = min(sPIC_HEIGHT_MAX, pic.Size.cy);
        pic.Size.cx = max(DEFAULT_PIC_WIDTH, celTemp.size.cx);
        pic.Size.cx = min(sPIC_WIDTH_MAX, pic.Size.cx);
        pic.commands.push_back(PicCommand());
        pic.commands.back().CreateDrawVisualBitmap(celTemp, true);
    }

    byteStream.seekg(header.vectorDataOffset);
    PicReadFromSCI0_SCI1(resource, byteStream, true);
}

void PicWriteToVGA11(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag)
{
    const PicComponent &pic = resource.GetComponent<PicComponent>();
    const PaletteComponent *palette = resource.TryGetComponent<PaletteComponent>();

    PicHeader_VGA11 header = {};
    header.headerSize = sizeof(PicHeader_VGA11) - sizeof(header.headerSize);
    header.numPriorities = 16;
    header.xVanish = DefaultXVanish;
    header.yVanish = DefaultYVanish;
    header.viewAngle = DefaultViewAngle;

    // Our file is generally organized like this:
    // [header]
    // [pribands]
    // [celdata]
    // [palette]
    // [vectordata]

    byteStream << header; // Write it temporarily... we'll need to re-write it later

    // Now the priority bands. Search in the pic commands for a pri band guy and serialize it.
    bool found = false;
    for (const PicCommand &command : pic.commands)
    {
        if (command.type == PicCommand::CommandType::SetPriorityBars)
        {
            byteStream.WriteBytes((const uint8_t *)command.setPriorityBars.pPriorityLines, NumPriorityBars * sizeof(*command.setPriorityBars.pPriorityLines));
            header.priBandCount = NumPriorityBars;
            found = true;
            break;
        }
    }

    uint32_t mysteryBytes = 0;  // Seems to have various values.
    byteStream << mysteryBytes;

    assert(found);  // We should have prevented pic saving if there wasn't a set pri bars command.

    header.celCount = 0;
    for (const PicCommand &command : pic.commands)
    {
        if (command.type == PicCommand::CommandType::DrawBitmap)
        {
            header.celCount = 1;
            header.celHeaderOffset = byteStream.tellp();
            const Cel &cel = *command.drawVisualBitmap.pCel;
            sci::ostream celRLEData;
            sci::ostream celRawData;
            PicCelHeader_VGA11 picCelHeader = {};
            WriteImageData(celRLEData, cel, true, celRawData, false);
            // We go: [celheader][RLE][literal]
            picCelHeader.celHeader.always_0xa = 0xa;
            picCelHeader.celHeader.rleCelDataSize = celRLEData.GetDataSize();
            picCelHeader.celHeader.totalCelDataSize = celRLEData.GetDataSize() + celRawData.GetDataSize();
            picCelHeader.celHeader.offsetRLE = byteStream.tellp() + sizeof(PicCelHeader_VGA11);
            picCelHeader.celHeader.offsetLiteral = picCelHeader.celHeader.offsetRLE + picCelHeader.celHeader.rleCelDataSize;
            picCelHeader.celHeader.paletteOffset = palette ? (picCelHeader.celHeader.offsetLiteral + celRawData.GetDataSize()) : 0;
            picCelHeader.celHeader.size = cel.size;
            picCelHeader.celHeader.placement = cel.placement;
            picCelHeader.celHeader.transparentColor = cel.TransparentColor;
            // These extra fields exist in the pic cel header:
            picCelHeader.ohFourOh1 = 0x0401;
            picCelHeader.totalCelDataSize = picCelHeader.celHeader.totalCelDataSize;

            // Write it
            byteStream << picCelHeader;
            sci::transfer(sci::istream_from_ostream(celRLEData), byteStream, celRLEData.GetDataSize());
            sci::transfer(sci::istream_from_ostream(celRawData), byteStream, celRawData.GetDataSize());

            break;
        }
    }

    if (palette)
    {
        header.paletteOffset = byteStream.tellp();
        WritePaletteShortForm(byteStream, *palette);
    }

    header.vectorDataOffset = byteStream.tellp();
    // Make a copy of the commands without some of them
    vector<PicCommand> commands;
    commands.reserve(pic.commands.size());
    for (const PicCommand &command : pic.commands)
    {
        if ((command.type != PicCommand::CommandType::DrawBitmap) &&
            (command.type != PicCommand::CommandType::SetPriorityBars))
        {
            commands.push_back(command);
        }
    }
    SerializeAllCommands_SCI0_SCI1(&byteStream, commands, commands.size());
    header.vectorDataSize = byteStream.tellp() - header.vectorDataOffset;

    // Now write the actual header:
    byteStream.seekp(0);
    byteStream << header;
}

int gMAxPri = 0;

// Note: KQ7 pics 100 and 14000 are busted. Appears to be a problem with the resources themselves, and they are not used in-game.
void ReadPicCelFromVGA2(sci::istream &byteStream, Cel &cel, int16_t &priority, bool is640)
{
    PicCelHeader_VGA2 celHeader;
    byteStream >> celHeader;

    assert(celHeader.transparentColor == 255);
    assert(celHeader.placement == point16(0, 0));
    assert(celHeader.compressed == 0 ||
        (celHeader.compressed == 138 && celHeader.colorOffset != 0));
    assert(celHeader.dataFlags == 0);
    assert(celHeader.paletteOffset == 0);
    assert(celHeader.rowTableOffset == 0 || (celHeader.colorOffset != 0));

    cel.size = celHeader.size;

    // REVIEW: SQ6 pic 545 renders incorrectly. The cel width is 650 (despite the pic being 640).
    // There are 10 pixels of black between each line.

    // Perhaps relativePlacement is affected by mirroring and placement is not?
    // NOTE: ScummVM ignores celHeader.placement, saying it's sometimes garbage.
    cel.placement = celHeader.placement + celHeader.relativePlacement;

    // This appears to be needed for 640 wide pics?
    if (is640)
    {
        cel.placement.x *= 2;
        cel.placement.y = (int16_t)((int)cel.placement.y * 12 / 5);
    }

    cel.TransparentColor = celHeader.transparentColor;
    priority = celHeader.priority;

    if (priority != 1000)
    {
        gMAxPri = max(priority, gMAxPri);
    }

    byteStream.seekg(celHeader.controlOffset);
    if ((celHeader.colorOffset == 0) || (celHeader.compressed == 0))
    {
        size_t dataSize = celHeader.size.cx * celHeader.size.cy; // Not sure if padding happens?
        cel.Data.allocate(max(1, dataSize));
        byteStream.read_data(&cel.Data[0], dataSize);
        FlipImageData(&cel.Data[0], celHeader.size.cx, celHeader.size.cy, celHeader.size.cx);
        cel.Stride32 = false;
    }
    else
    {
        ReadImageData(byteStream, cel, true, sci::istream(byteStream, celHeader.colorOffset));
        cel.Stride32 = true;
    }
}

void PicReadFromVGA2(ResourceEntity &resource, sci::istream &byteStream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    PicComponent &pic = resource.GetComponent<PicComponent>();
    PicHeader_VGA2 header;
    byteStream >> header;

    assert(header.headerSize == 0xe);
    static_assert(sizeof(PicHeader_VGA2) == 0xe, "PicHeader wrong size");
    assert(header.celHeaderSize == sizeof(PicCelHeader_VGA2));

    // This is defined for some games (SQ6, which is 640x480), but not others like Gabriel Knight.
    // So in addition to this, we'll take into account the individual cels' widths.
    pic.Size = size16(header.width, header.height);
    pic.Size.cx = max(pic.Size.cx, 320);
    pic.Size.cy = max(pic.Size.cy, 200);

    uint32_t base = byteStream.tellg();


    // Investigation:
    byteStream.seekg(base + header.celCount * header.celHeaderSize);
    Temp temp;
    byteStream >> temp;


    for (uint8_t cel = 0; cel < header.celCount; cel++)
    {
        byteStream.seekg(base + cel * header.celHeaderSize);
        Cel picCel;
        int16_t priority;
        ReadPicCelFromVGA2(byteStream, picCel, priority, pic.Size.cx >= 640);

        pic.Size.cx = max(pic.Size.cx, picCel.size.cx);
        pic.Size.cy = max(pic.Size.cy, picCel.size.cy);

        // SCI32 games appear to have arbitrary priority levels
        // KQ7, GK, QG4: -1000 is the background, 1000 is always in front, and others roughly correspond to the y value of objects, in the range 0-199
        // SQ6: doesn't seem to have any -1000 or 1000. Question is: does it allow it?
        // OutputDebugString(fmt::format("pri: {0}\n", priority).c_str());
        pic.commands.push_back(PicCommand());
        pic.commands.back().CreateDrawVisualBitmap(picCel, true, priority);
    }

    if (header.paletteOffset)
    {
        resource.AddComponent(move(make_unique<PaletteComponent>()));
        byteStream.seekg(header.paletteOffset);
        ReadPalette(resource.GetComponent<PaletteComponent>(), byteStream);
    }
}

// SCI2 pics looks like:
// [PicHeader]
// [n * celHeaders]
// 6 bytes I don't know
// [PaletteData]
// [RLE 0]  // (compressed only)
// ...
// [RLE n]
// [Literal 0]
// ...
// [Literal n]
// [Unknown data 0] // (compressed only) // 8 * number of rows
// ...
// [Unknown data n]
const uint32_t UnknownByteCount = 6;
const uint8_t CompressionTag = 138;
const uint32_t UncompressedTag = 6;
void PicWriteToVGA2(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag)
{
    const PicComponent &pic = resource.GetComponent<PicComponent>();
    const auto &commands = pic.commands;
    PicHeader_VGA2 header = {};
    header.celCount = (uint8_t)std::count_if(commands.begin(), commands.end(), [](const PicCommand& command) { return command.type == PicCommand::CommandType::DrawBitmap; });
    
    // I can get compressed working ok to load images in SCICompanion or SV.exe, but not Sierra games. That mystery data at the
    // end of the resource must be related to compression.
    bool compressed = false;

    header.hasCompressedCels = compressed ? 1 : 0;
    header.headerSize = sizeof(header);
    header.celHeaderSize = sizeof(PicCelHeader_VGA2);
    bool is640 = false;
    if (pic.Size.cx > 320)
    {
        is640 = true;
        header.width = pic.Size.cx;
        header.height = pic.Size.cy;
    } // Otherwise we leave them at zero. I guess?
    
    const PaletteComponent *palette = resource.TryGetComponent<PaletteComponent>();
    if (palette)
    {
        header.paletteOffset = sizeof(header) + (header.celCount * header.celHeaderSize) + UnknownByteCount;
    }

    byteStream << header;

    // We'll come back and write the actual cel headers. For now, just fill in gaps
    uint32_t offsetOfCelHeaders = byteStream.tellp();
    byteStream.FillByte(0, (header.celCount * header.celHeaderSize));
    byteStream.FillByte(0, UnknownByteCount);

    // Then the palette
    if (palette)
    {
        assert(header.paletteOffset == byteStream.tellp());
        WritePaletteSCI2(byteStream, *palette);
    }

    // Now write out the actual data. We can write the RLE directly to the byteStream, but we'll need to write the literal to another one temporarily
    sci::ostream celLiteralData;
    if (compressed)
    {
        celLiteralData.EnsureCapacity(100000); // Something fairly large.
    }
    std::vector<PicCelHeader_VGA2> celHeaders;
    for (auto &command : commands)
    {
        if (command.type == PicCommand::CommandType::DrawBitmap)
        {
            PicCelHeader_VGA2 celHeader = {};
            celHeader.compressed = compressed ? CompressionTag : 0;
            Cel *pCel = command.drawVisualBitmap.pCel;
            celHeader.size = pCel->size;
            celHeader.relativePlacement = pCel->placement;

            // Need to adjust by resolution properly.
            if (is640)
            {
                celHeader.relativePlacement.x /= 2;
                int placementY = (int)round((float)(int)celHeader.relativePlacement.y * 5.0f / 12.0f);
                celHeader.relativePlacement.y = (int16_t)placementY;
            }

            celHeader.priority = command.drawVisualBitmap.priority;
            celHeader.transparentColor = pCel->TransparentColor;
            assert(celHeader.transparentColor == 255);          // Required for SCI2, I think

            if (compressed)
            {
                celHeader.colorOffset = celLiteralData.tellp();   // This will need to be adjusted later
                celHeader.controlOffset = byteStream.tellp();
                celHeader.rowTableOffset = 0;                    // ? Do we need to write this?
                WriteImageData(byteStream, *pCel, true, celLiteralData, false);
                celHeader.controlDataSize = byteStream.tellp() - celHeader.controlOffset;
                celHeader.totalDataSize = celHeader.controlDataSize + (celLiteralData.tellp() - celHeader.colorOffset);
            }
            else
            {
                celHeader.controlOffset = byteStream.tellp();
                celHeader.totalDataSize = celHeader.size.cx * celHeader.size.cy;
                celHeader.controlDataSize = UncompressedTag;
                // The image is flipped, and without any kind of padding (e.g. stride = size.cx).
                for (int y = celHeader.size.cy - 1; y >= 0; y--)
                {
                    byteStream.WriteBytes(&pCel->Data[y * pCel->GetStride()], celHeader.size.cx);
                }
            }
            celHeaders.push_back(celHeader);
        }
    }

    // Offset the colorOffset to make it an absolute value
    uint32_t baseOffsetForLiteral = byteStream.tellp();
    byteStream.seekp(offsetOfCelHeaders);
    for (auto &celHeader : celHeaders)
    {
        if (compressed)
        {
            celHeader.colorOffset += baseOffsetForLiteral;
        }
        byteStream << celHeader;
    }

    byteStream.seekp(baseOffsetForLiteral); // Back to the end
    assert(byteStream.GetDataSize() == baseOffsetForLiteral);
    if (compressed)
    {
        sci::transfer(sci::istream_from_ostream(celLiteralData), byteStream, celLiteralData.GetDataSize());
    }
    // Now just those pesky random mystery data.
}

bool PicValidateEGA(const ResourceEntity &resource)
{
    const PicComponent &pic = resource.GetComponent<PicComponent>();

    // Perform a little validation.
    if (!appState->_fDontCheckPic)
    {
        DWORD fPalettePresent = 0;
        for (size_t i = 0; i < pic.commands.size(); i++)
        {
            const PicCommand &command = pic.commands[i];
            if (command.type == PicCommand::SetPalette)
            {
                fPalettePresent |= 1 << command.setPalette.bPaletteNumber;
            }
        }
        if (fPalettePresent != 0xf)
        {
            // Warn the user
            CDontShowAgainDialog dialog(TEXT("This pic is missing a Set Palette command for one or more of the four palettes.\nThis may lead to unpredictable results when the game draws the pic."),
                appState->_fDontCheckPic);
            dialog.DoModal();
        }
    }
    return true; // Always save anyway...
}

bool PicValidateVGA(const ResourceEntity &resource)
{
    bool ok = true;
    const PicComponent &pic = resource.GetComponent<PicComponent>();

    if (!pic.Traits->SupportsPenCommands)
    {
        bool found = false;
        for (size_t i = 0; !found && (i < pic.commands.size()); i++)
        {
            PicCommand::CommandType type = pic.commands[i].type;
            found = (type == PicCommand::Pattern);
        }
        if (found)
        {
            AfxMessageBox("This pic contains pen commands, which are not supported by this version of pic resources.", MB_ICONWARNING | MB_OK);
        }
    }


    // Perform a little validation.
    if (!appState->_fDontCheckPic)
    {
        bool found = false;
        for (size_t i = 0; !found && (i < pic.commands.size()); i++)
        {
            found = (pic.commands[i].type == PicCommand::SetPriorityBars);
        }
        if (!found)
        {
            // Warn the user
            CDontShowAgainDialog dialog(TEXT("This pic is missing a Set Priority Bars command\nThis may lead to unpredictable results when the game draws the pic."),
                appState->_fDontCheckPic);
            dialog.DoModal();
        }
    }

    return ok; // Always save anyway...
}

void PicWritePolygons(const ResourceEntity &resource, int resourceNumber)
{
    PolygonComponent *polygonComponent = const_cast<PolygonComponent*>(resource.TryGetComponent<PolygonComponent>());
    if (polygonComponent)
    {
        // Use the provided resource number instead of that in the ResourceEntity, since it may
        // be -1
        polygonComponent->Commit(resourceNumber);
    }
}

PicTraits picTraitsEGA =
{
    false,  // IsVGA
    false,  // AllowMultipleBitmaps
    false,  // CanChangePriorityLines
    false,  // SixBitPri
    true,   // SupportsPenCommands
    true,   // SupportsControlScreen
    true,   // SupportsVectorCommands
    false,  // ContinuousPriority
};
PicTraits picTraitsVGA_1_0 =
{
    true,
    true,
    true,
    false,
    true,
    true,
    true,
    false
};
PicTraits picTraitsVGA_1_1 =
{
    true,
    false,
    true,
    true,
    false,
    true,
    true,
    false
};
PicTraits picTraitsVGA_2 =
{
    true,
    true,
    false,
    true,
    false,
    false,
    false,
    true
};
// TODO: We'll need one for games with 640x480

ResourceTraits picResourceTraitsEGA =
{
    ResourceType::Pic,
    PicReadFromEGA,
    PicWriteTo,
    PicValidateEGA,
    nullptr
};

ResourceTraits picResourceTraitsVGA =
{
    ResourceType::Pic,
    PicReadFromVGA,
    PicWriteTo,
    PicValidateVGA,
    PicWritePolygons,
};

ResourceTraits picResourceTraitsVGA11 =
{
    ResourceType::Pic,
    PicReadFromVGA11,
    PicWriteToVGA11,
    PicValidateVGA,
    PicWritePolygons,
};

ResourceTraits picResourceTraitsVGA2 =
{
    ResourceType::Pic,
    PicReadFromVGA2,
    PicWriteToVGA2,
    NoValidationFunc,
    PicWritePolygons,
};

int g_PicIds = 0;

bool IsSame(const PicComponent *one, const PicComponent *two)
{
    return one && two && (one->UniqueId == two->UniqueId);
}

PicComponent::PicComponent() : PicComponent(&picTraitsEGA) {}

PicComponent::PicComponent(const PicTraits *traits) : Traits(traits), Size(size16(DEFAULT_PIC_WIDTH, DEFAULT_PIC_HEIGHT)), UniqueId(g_PicIds++)  {}

ResourceEntity *CreatePicResource(SCIVersion version)
{
    PicTraits *picTraits = &picTraitsEGA;
    ResourceTraits *ptraits = &picResourceTraitsEGA;
    if (version.PicFormat == PicFormat::VGA1)
    {
        ptraits = &picResourceTraitsVGA;
        picTraits = &picTraitsVGA_1_0;
    }
    else if (version.PicFormat == PicFormat::VGA1_1)
    {
        ptraits = &picResourceTraitsVGA11;
        picTraits = &picTraitsVGA_1_1;
    }
    else if (version.PicFormat == PicFormat::VGA2)
    {
        ptraits = &picResourceTraitsVGA2;
        picTraits = &picTraitsVGA_2;
    }

    std::unique_ptr<ResourceEntity> pResource = std::make_unique<ResourceEntity>(*ptraits);
    pResource->AddComponent(move(make_unique<PicComponent>(picTraits)));
    return pResource.release();
}

ResourceEntity *CreateDefaultPicResource(SCIVersion version)
{
    std::unique_ptr<ResourceEntity> pResource(CreatePicResource(version));
    // Make a view with one loop that has one cel.
    PicComponent &pic = pResource->GetComponent<PicComponent>();

    // Reserve space for about 166 commands.
    pic.commands.reserve(500 / 2);

    if (version.PicFormat == PicFormat::EGA)
    {
        // Now Prepopulate the pic with palettes, pen styles/sizes and screen on/offs.
        // Otherwise, the SCI interpreter may use values from the previous picture.

        // First the palettes:
        for (uint8_t b = 0; b < 4; b++)
        {
            pic.commands.push_back(PicCommand::CreateSetPalette(b, g_defaultPalette));
        }
        // We actually don't need to set the pen style - it will be set with
        // the first pen command.
    }
    else if (version.PicFormat == PicFormat::VGA1)
    {
        // I don't think we need anything else
    }
    else if (version.PicFormat == PicFormat::VGA1_1)
    {
        // We need to have a set priority bars command
        PicCommand setPriBarsCommand;
        setPriBarsCommand.CreateSetPriorityBars(g_defaultPriBands16Bit, pic.Traits->SixteenBitPri, pic.Traits->IsVGA);
        pic.commands.insert(pic.commands.begin(), setPriBarsCommand);
    }
    else if (version.PicFormat == PicFormat::VGA2)
    {
        switch (version.DefaultResolution)
        {
            case NativeResolution::Res320x200:
                pic.Size = size16(320, 200);
                break;
            case NativeResolution::Res640x400:
                pic.Size = size16(640, 400);
                break;
            case NativeResolution::Res640x480:
                pic.Size = size16(640, 480);
                break;
            default:
                assert(false);
                break;
        }
    }

    // Finally, turn off the screens.
    pic.commands.push_back(PicCommand::CreateDisableVisual());
    pic.commands.push_back(PicCommand::CreateDisablePriority());
    pic.commands.push_back(PicCommand::CreateDisableControl());

    return pResource.release();
}

