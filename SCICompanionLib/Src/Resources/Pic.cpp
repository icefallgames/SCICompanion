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
    SerializeAllCommands(&byteStream, pic.commands, pic.commands.size());
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
                pic.commands.push_back(PicCommand::CreateSetPriority(bColorCode & 0xf));
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
    uint8_t unknown1;
    uint8_t priBandCount;
    uint8_t hasCel;
    uint8_t unknown2;
    uint16_t unkonwn3;
    uint32_t unknown4;
    uint32_t vectorDataSize;
    uint32_t vectorDataOffset;
    uint32_t unknown6;
    uint32_t unknown7;
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
#include <poppack.h>

void PicWriteToVGA11(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag)
{
    const PicComponent &pic = resource.GetComponent<PicComponent>();
    const PaletteComponent *palette = resource.TryGetComponent<PaletteComponent>();

    PicHeader_VGA11 header = {};
    header.headerSize = sizeof(PicHeader_VGA11) - sizeof(header.headerSize);
    header.unknown1 = 0x10;
    header.unkonwn3 = 0x00a0;
    header.unknown4 = 0x0046D8F0;

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

    header.hasCel = 0;
    for (const PicCommand &command : pic.commands)
    {
        if (command.type == PicCommand::CommandType::DrawBitmap)
        {
            header.hasCel = 0x1;
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
    SerializeAllCommands(&byteStream, commands, commands.size());
    header.vectorDataSize = byteStream.tellp() - header.vectorDataOffset;

    // Now write the actual header:
    byteStream.seekp(0);
    byteStream << header;
}

void PicReadFromVGA11(ResourceEntity &resource, sci::istream &byteStream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    PicComponent &pic = resource.GetComponent<PicComponent>();
    PicHeader_VGA11 header;
    byteStream >> header;

    assert(header.headerSize == 0x26);
    static_assert(sizeof(PicHeader_VGA11) == 0x28, "PicHeader wrong size");

    // Let's learn stuff:
    assert(header.unknown1 == 0x10);
    assert(header.unknown2 == 0x0);
    assert(header.unkonwn3 == 0x00a0);
    assert(header.unknown4 == 0x0046D8F0);  // Some kind of version marker?
    assert(header.unknown6 == 0x0);
    assert(header.unknown7 == 0x0);
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

    if (header.hasCel)
    {
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
    false,
    false,
    false,
    false,
    true,
};
PicTraits picTraitsVGA_1_0 =
{
    true,
    true,
    true,
    false,
    true,
};
PicTraits picTraitsVGA_1_1 =
{
    true,
    false,
    true,
    true,
    false,
};

ResourceTraits picResourceTraitsEGA =
{
    ResourceType::Pic,
    &PicReadFromEGA,
    &PicWriteTo,
    &PicValidateEGA,
    nullptr
};

ResourceTraits picResourceTraitsVGA =
{
    ResourceType::Pic,
    &PicReadFromVGA,
    &PicWriteTo,
    &PicValidateVGA,
    &PicWritePolygons,
};

ResourceTraits picResourceTraitsVGA11 =
{
    ResourceType::Pic,
    &PicReadFromVGA11,
    PicWriteToVGA11,
    &PicValidateVGA,
    &PicWritePolygons,
};

PicComponent::PicComponent() : PicComponent(&picTraitsEGA) {}

PicComponent::PicComponent(const PicTraits *traits) : Traits(traits), Size(size16(DEFAULT_PIC_WIDTH, DEFAULT_PIC_HEIGHT)) {}

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

    // Finally, turn off the screens.
    pic.commands.push_back(PicCommand::CreateDisableVisual());
    pic.commands.push_back(PicCommand::CreateDisablePriority());
    pic.commands.push_back(PicCommand::CreateDisableControl());

    return pResource.release();
}

