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
#include "Components.h"
#include "RasterOperations.h"
#include "View.h"
#include "ResourceEntity.h"
#include "PaletteOperations.h"
#include "AppState.h"
#include "format.h"
#include "ImageUtil.h"

using namespace std;

int g_debugCelRLE;

uint8_t g_egaPaletteMapping[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
uint8_t g_vgaPaletteMapping[256];



bool IsMirror(uint16_t nLoop, uint16_t mask)
{
    return (0 != (((mask) >> (nLoop)) & 1));
}

enum class GNPResult
{
    Done = 0,   // pColor is not set
    NewLine = 1,// pColor is set
    Ok = 2,    // pColor is set
};


void ReadImageData(sci::istream &byteStream, Cel &cel, bool isVGA)
{
    ReadImageData(byteStream, cel, isVGA, byteStream);
}

const size_t ReasonableLimit = 640 * 480;

template<typename _TCalcFunc>
void ReadImageDataWorker(sci::istream &byteStreamRLE, Cel &cel, bool isVGA, sci::istream &byteStreamLiteral, _TCalcFunc calcFunc)
{
    uint8_t scratchBuffer[128];
    uint8_t *scratchPointer = nullptr;

    // For some reason, the bitmap with needs to be rounded up to fit on a DWORD boundary.
    int cxActual = CX_ACTUAL(cel.size.cx);
    int cxRemainingOnThisLine = cel.size.cx;
    int cxLine = cel.size.cx;
    int cxPadding = cxActual - cxLine;

    // Bitmap buffer:
    int cBufferSizeRemaining = cxActual * (int)cel.size.cy;
    size_t dataSize = cBufferSizeRemaining;

    if (dataSize > ReasonableLimit)
    {
        throw std::exception("Corrupt raster resource.");
    }

    // The image starts from the bottom left, not the top left:
    int y = cel.size.cy - 1;
    int x = 0;

    calcFunc(y, byteStreamRLE, byteStreamLiteral);

    // Create our buffer
    cel.Data.allocate(max(1, dataSize));
    int cCount = 0;
    int cxRemainingForThisCommand = 0;
    while (byteStreamRLE.good() && (cBufferSizeRemaining > 0))
    {
        uint8_t b;
        if (cxRemainingForThisCommand == 0)
        {
            // Read more data.
            byteStreamRLE >> b;
            if (!isVGA)
            {
                uint8_t color = (b & 0x0f);
                cCount = ((b & 0xf0) >> 4);
                memset(scratchBuffer, color, cCount);
                scratchPointer = scratchBuffer;
            }
            else
            {
                // VGA has some encoding
                cCount = (b & 0x3f);
                switch (b >> 6)
                {
                    case 0x01:
                        // Copy next cCount + 64 bytes as is
                        cCount += 64;
                        // fall through...
                    case 0x00:
                        // Copy next cCount bytes as is
                        byteStreamLiteral.read_data(scratchBuffer, cCount);
                        break;
                    case 0x02:
                        uint8_t color;
                        byteStreamLiteral >> color;
                        // Set next cCount bytes to color
                        memset(scratchBuffer, color, cCount);
                        break;
                    case 0x03:
                        // Set next cCount bytes to transparent
                        memset(scratchBuffer, cel.TransparentColor, cCount);
                        break;
                }
            }
            scratchPointer = scratchBuffer;
            cxRemainingForThisCommand = cCount;
        }

        assert(y >= 0);

        int cAmountToFill;
        // Fill up to the end of this command.
        if (cxRemainingForThisCommand < cxRemainingOnThisLine)
        {
            cAmountToFill = cxRemainingForThisCommand;
            cxRemainingForThisCommand = 0;
            int pos = y * cxActual + x;
            memcpy(&cel.Data[pos], scratchPointer, cAmountToFill);
            x += cAmountToFill;
            cxRemainingOnThisLine -= cAmountToFill;
            cBufferSizeRemaining -= cAmountToFill;
            scratchPointer += cAmountToFill;
        }
        else
        {
            // We are going to fill up to the end of the line, and pad the rest
            cAmountToFill = cxLine - x;
            memcpy(&cel.Data[y * cxActual + x], scratchBuffer, cAmountToFill);

            cBufferSizeRemaining -= cAmountToFill;

            // Pad:
            //fill_n(cel.Data.begin() + , cxPadding, cel.TransparentColor);
            cel.Data.fill((y * cxActual + x + cAmountToFill), cxPadding, cel.TransparentColor);

            cBufferSizeRemaining -= cxPadding;

            // Move to the next line:
            x = 0;
            y -= 1;

            calcFunc(y, byteStreamRLE, byteStreamLiteral);

            cxRemainingForThisCommand = cxRemainingForThisCommand - cxRemainingOnThisLine;

            // This hits for Laura Bow 2
            // assert(cxRemainingForThisCommand == 0); // If this never hits, it turns out we don't need to support filling across scanlines.
            cxRemainingOnThisLine = cel.size.cx;
        }
    }
}

void CalcNothing(int y, sci::istream &byteStreamRLE, sci::istream &byteStreamLiteral) {}

void ReadImageData(sci::istream &byteStreamRLE, Cel &cel, bool isVGA, sci::istream &byteStreamLiteral)
{
    ReadImageDataWorker(byteStreamRLE, cel, isVGA, byteStreamLiteral, CalcNothing);
}

// RLE, followed by Literal
void CalculateSCI2RowOffsets(Cel &cel, sci::istream byteStreamRLE, sci::istream byteStreamLiteral, sci::ostream &byteStreamOffsets)
{
    uint32_t orig = byteStreamOffsets.tellp();

    uint32_t baseRLE = byteStreamRLE.tellg();
    uint32_t baseLiteral = byteStreamLiteral.tellg();

    sci::istream byteStreamRLE1 = byteStreamRLE;
    sci::istream byteStreamLiteral1 = byteStreamLiteral;
    ReadImageDataWorker(byteStreamRLE1, cel, true, byteStreamLiteral1,
        [&cel, baseRLE, &byteStreamOffsets](int y, sci::istream &byteStreamRLE, sci::istream &byteStreamLiteral)
    {
        if (y >= 0)
        {
            byteStreamOffsets << (byteStreamRLE.tellg() - baseRLE);
        }
    }
    );

    sci::istream byteStreamRLE2 = byteStreamRLE;
    sci::istream byteStreamLiteral2 = byteStreamLiteral;
    ReadImageDataWorker(byteStreamRLE2, cel, true, byteStreamLiteral2,
        [&cel, baseLiteral, &byteStreamOffsets](int y, sci::istream &byteStreamRLE, sci::istream &byteStreamLiteral)
    {
        if (y >= 0)
        {
            byteStreamOffsets << (byteStreamLiteral.tellg() - baseLiteral);
        }
    }
    );

    // Assert we write the correct amount of row offsets.
    assert((byteStreamOffsets.tellp() - orig) == (cel.size.cy * 2 * 4));
}

const uint16_t ReasonableCelWidth = 320;
const uint16_t ReasonableCelHeight = 200;

void ReadCelFrom(ResourceEntity &resource, sci::istream byteStream, Cel &cel, bool isVGA)
{
    // Width and height
    byteStream >> cel.size.cx;
    byteStream >> cel.size.cy;

    // Ensure the size is reasonable (unsure if this is a real limit), so we don't go
    // allocating huge amounts of memory.
    if (ClampSize(resource.GetComponent<RasterComponent>(), cel.size))
    {
        appState->LogInfo("Corrupt view resource: (%d,%d).", resource.ResourceNumber, resource.PackageNumber);
        throw std::exception("Invalid cel size.");
    }

    // x and y placement (right, down)
    uint16_t placement;
    byteStream >> placement;
    cel.placement.x = (char)(placement & 0x00ff);
    cel.placement.y = (char)((placement & 0xff00) >> 8);

    // Transparent colour
    byteStream >> cel.TransparentColor;

    // There is one more byte here for VGA
    if (isVGA)
    {
        uint8_t unknown;
        byteStream >> unknown;
    }

    g_debugCelRLE = resource.ResourceNumber;
    ReadImageData(byteStream, cel, isVGA);
}

const uint16_t ReasonableCelCount = 128;

void ReadLoopFrom(ResourceEntity &resource, sci::istream byteStream, Loop &loop, bool isVGA)
{
    assert(loop.Cels.empty());
    uint16_t nCels;
    byteStream >> nCels;
    bool fZeroCels = (nCels == 0);
    if (fZeroCels)
    {
        nCels = 1;
        // Handle corrupt views (e.g. LSL2, view 225, loop 3) by
        // creating a dummy loop with 1 1x1 cel
    }
    if (nCels > ReasonableCelCount)
    {
        throw std::exception("Too many cels.");
    }
    loop.Cels.assign(nCels, Cel()); // Just a bunch of empty ones.
    byteStream >> loop.UnknownData; // Skip 2 bytes (function unknown) - store just in case

    if (fZeroCels)
    {
        CreateDegenerate(loop.Cels[0], loop.Cels[0].TransparentColor);
    }
    else
    {
        for (uint16_t i = 0; i < nCels; i++)
        {
            uint16_t nOffset;
            byteStream >> nOffset;
            sci::istream byteStreamCel(byteStream);
            byteStreamCel.seekg(nOffset);
            ReadCelFrom(resource, byteStreamCel, loop.Cels[i], isVGA);
        }
    }
    assert(loop.Cels.size() == nCels); // Ensure cel count is right.
}

uint8_t GetPixel(const uint8_t *p, int x, int y, int cxActual)
{
    return (*((p)+((y)* (cxActual)) + (x)));
}

// The sole purpose of this is to ignore the padding at the end of each scanline.
GNPResult GetNextPixel(const uint8_t *pData, int &x, int &y, int cx, uint8_t *pColor)
{
    GNPResult iResult = GNPResult::Ok;
    if (x >= cx)
    {
        x = 0;
        y--;
        iResult = GNPResult::NewLine;
    }

    if (y < 0)
    {
        iResult = GNPResult::Done;
    }

    if (iResult != GNPResult::Done)
    {
        *pColor = GetPixel(pData, x, y, CX_ACTUAL(cx));
    }
    ++x;
    return iResult;
}

const int ValuableIdenticalColorsThreshold = 2;

// Returns the next pixel color, and how many pixels after that are identical
int GetNextIdenticalPixelCount(const uint8_t *pData, int x, int y, int cx, uint8_t *pColor)
{
    int count = 0;
    if (GetNextPixel(pData, x, y, cx, pColor) == GNPResult::Ok)
    {
        count++;
        uint8_t temp;
        while ((GetNextPixel(pData, x, y, cx, &temp) == GNPResult::Ok) && (*pColor == temp) && (count < 0x3f))
        {
            count++;
        }
    }
    return count;
}

// Sierra actually handles 127 (63 + 64) here, but SV.exe barfs on it, so limit ourselves to 63.
const int MaxDifferentPixelCount = 63;

// The way this works is:
//  - sequences of 2 are allowed in these sequences (since it would end up
//      costing the same just to keep going), unless they are immediately
//      followed by another sequence of 2.
int GetNextDifferentPixelCount(const uint8_t *pData, int x, int y, int cx, uint8_t transparent)
{
    int count = 0;
    uint8_t previousColor;
    if (GetNextPixel(pData, x, y, cx, &previousColor) == GNPResult::Ok)
    {
        count++;
        uint8_t nextColor;
        int identicalColorCount = 1;
        int previousIdenticalColorCountSequence = 0;
        while ((count < MaxDifferentPixelCount) && (GetNextPixel(pData, x, y, cx, &nextColor) == GNPResult::Ok))
        {
            count++;
            if (nextColor == previousColor)
            {
                identicalColorCount++;

                if ((identicalColorCount >= 3) || (identicalColorCount >= 2 && (previousColor == transparent)))
                {
                    // 3 in a row. Time to bail.
                    count -= identicalColorCount;
                    assert(count > 0);
                    break;
                }
                // Hmm, this makes it a bit worse. I'm close to the "offical size" with this algorithm
                // but not quite.
                // The remaining difference might just be in palette size.
                /*
                if ((identicalColorCount >= 2) && (previousIdenticalColorCountSequence >= 2))
                {
                // 2 in a row right after 2 in a row. Time to bail.
                count -= (identicalColorCount + previousIdenticalColorCountSequence);
                assert(count > 0);
                break;
                }*/
            }
            else
            {
                previousIdenticalColorCountSequence = identicalColorCount;
                identicalColorCount = 1;
                previousColor = nextColor;
            }
        }
    }
    return count;
}

// Advances x and y by amount, and returns the color at x and y
void AdvanceXY(const uint8_t *pData, int amount, int &x, int &y, int cx, uint8_t *pColor)
{
    assert(y >= 0);
    *pColor = GetPixel(pData, x, y, CX_ACTUAL(cx));
    int lines = amount / cx;
    int columns = amount % cx;
    y -= (amount / cx);
    x += columns;
    if (x >= cx)
    {
        x %= cx;
        y--;
    }
    assert((y >= 0) || ((y == -1) && (x == 0)));
}

void WriteImageData(sci::ostream &byteStream, const Cel &cel, bool isVGA)
{
    WriteImageData(byteStream, cel, isVGA, byteStream, true);
}

void WriteImageData(sci::ostream &rleStream, const Cel &cel, bool isVGA, sci::ostream &literalStream, bool writeZero)
{
    // Now the image data
    // cxActual is how wide our bitmap data is (a boundary of 4 pixels - 32 bits)
    int cxActual = CX_ACTUAL(cel.size.cx);

    // Start at the bottom
    int y = cel.size.cy - 1;
    int x = 0;

    uint8_t cCount = 0;
    uint8_t cColorPrev = 0xff; // invalid colour value (hmm, but not for VGA)

    // For some reason, image data always starts with a 0x00
    // REVIEW: With VGA1.1 it definitely does not. Still need to check VGA1.0
    // REVIEW: SCI Viewer seems to expect something there though, and when it decodes the pic it is off by one.
    //  (putting the zero in definitely makes SCI hang though)
    if (writeZero)
    {
        rleStream.WriteByte(0);
    }

    const uint8_t *bits = &cel.Data[0];

    if (isVGA)
    {
        int count = 1;  // Anything but zero
        while (count)
        {
            uint8_t identicalColor;
            count = GetNextIdenticalPixelCount(bits, x, y, cel.size.cx, &identicalColor);
            if (count > 1)
            {
                assert(count <= 0x3f);
                uint8_t byte = (uint8_t)count;
                if (identicalColor == cel.TransparentColor)
                {
                    // Sequence of transparent color
                    byte |= (0x3 << 6);
                    rleStream.WriteByte(byte);
                }
                else
                {
                    // Sequence of identical color
                    byte |= (0x2 << 6);
                    rleStream.WriteByte(byte);
                    literalStream.WriteByte(identicalColor);
                }
                uint8_t dummy;
                AdvanceXY(bits, count, x, y, cel.size.cx, &dummy);
            }
            else
            {
                count = GetNextDifferentPixelCount(bits, x, y, cel.size.cx, cel.TransparentColor);
                if (count > 0x3f)
                {
                    uint8_t byte = (uint8_t)(count - 64);
                    byte |= (0x1 << 6);
                    rleStream.WriteByte(byte);
                }
                else if (count > 0)
                {
                    rleStream.WriteByte((uint8_t)count);
                }
                // Now copy over that many bits.
                for (int i = 0; i < count; i++)
                {
                    uint8_t color;
                    AdvanceXY(bits, 1, x, y, cel.size.cx, &color);
                    literalStream.WriteByte(color);
                }
            }
        }
    }
    else
    {
        // EGA
        GNPResult result = GetNextPixel(bits, x, y, cel.size.cx, &cColorPrev);
        cCount = 1;

        uint8_t cColor;
        while (result != GNPResult::Done)
        {
            // Look at the next pixel.
            result = GetNextPixel(bits, x, y, cel.size.cx, &cColor);
            if ((result == GNPResult::Done) || (result == GNPResult::NewLine) || (cCount == 15) || (cColor != cColorPrev))
            {
                // (It is not valid to test cColor if iResult is GNP_DONE)
                assert((result == GNPResult::Done) || (cColor < 16));
                assert(cCount < 16);

                // Write out our data
                uint8_t b = ((cCount << 4) | cColorPrev);
                rleStream.WriteByte(b);
                cCount = 0;
            }
            cColorPrev = cColor;
            cCount++;
        }
    }
}

void WriteCelTo(const ResourceEntity &resource, sci::ostream &byteStream, const Cel &cel, bool isVGA)
{
    // Preliminary data (size, placement, transparent colour)
    byteStream.WriteWord(cel.size.cx);
    byteStream.WriteWord(cel.size.cy);
    // Convert to uint8_t before converting to uint16_t, to avoid sign extension problems.
    uint16_t wPlacement = (uint8_t)cel.placement.x;
    wPlacement |= (((uint16_t)(uint8_t)cel.placement.y) << 8);
    byteStream.WriteWord(wPlacement);
    byteStream.WriteByte(cel.TransparentColor);

    if (isVGA)
    {
        byteStream.WriteByte(0); // Mystery byte in VGA
    }

    WriteImageData(byteStream, cel, isVGA);
}

void WriteLoopTo(const ResourceEntity &resource, sci::ostream &byteStream, const Loop &loop, bool isVGA)
{
    // # of cels
    int cCels = (int)loop.Cels.size();
    byteStream.WriteWord((uint16_t)cCels);
    // 2 unknown bytes
    byteStream.WriteWord(loop.UnknownData);

    // Cel offsets:
    uint16_t wOffsets = 0;

    std::vector<uint16_t> rgOffsetsDummy(cCels, 0);
    // Stash this address for later;
    wOffsets = (uint16_t)byteStream.tellp();
    byteStream.WriteBytes((uint8_t*)&rgOffsetsDummy[0], sizeof(uint16_t) * rgOffsetsDummy.size());

    for (int i = 0; i < cCels; i++)
    {
        // Set the offset:
        // We need to recalc this each time (Based on iIndex), since the pData
        // in the serializer can change (be re-alloced).
        uint16_t *pOffsets = (uint16_t*)(byteStream.GetInternalPointer() + wOffsets);
        pOffsets[i] = ((uint16_t)(byteStream.tellp()));
        WriteCelTo(resource, byteStream, loop.Cels[i], isVGA);
    }
}


const uint16_t ReasonableLoopCount = 50;

void ReadPalette(ResourceEntity &resource, sci::istream &stream)
{
    // TODO: the palette reading code has a limit (bytes remaining) how to enforce
    resource.AddComponent(move(make_unique<PaletteComponent>()));
    ReadPalette(resource.GetComponent<PaletteComponent>(), stream);
}

void PostReadProcessing(ResourceEntity &resource, RasterComponent &raster)
{
    // Validate the view. Ensure that each loops has some cels, and that mirrors are valid loop numbers.
    for (size_t i = 0; i < raster.Loops.size(); i++)
    {
        Loop &loop = raster.Loops[i];
        if (loop.Cels.size() == 0)
        {
            appState->LogInfo("Empty loop found: view: %d, loop %d.", resource.ResourceNumber, i);
            // Make degenerate
            loop.Cels.push_back(Cel());
            CreateDegenerate(loop.Cels[0], loop.Cels[0].TransparentColor);
        }
        if (loop.IsMirror && (loop.MirrorOf >= (uint8_t)raster.Loops.size()))
        {
            throw std::exception("Invalid mirror.");
        }
    }
}

// Helpers for view resources
// We'll assume resource and package number are already taken care of, because they are common to all.
void ViewReadFromVersioned(ResourceEntity &resource, sci::istream &byteStream, bool isVGA)
{
    RasterComponent &raster = resource.GetComponent<RasterComponent>();
    uint16_t nLoopCountWord;
    uint16_t mirrorMask = 0;
    byteStream >> nLoopCountWord;

    // Perform some adjustments here. Actually, only the lower 8 bits is used for the loop count.
    uint16_t nLoops = nLoopCountWord & 0x00ff;
    bool hasPalette = (nLoopCountWord & 0x8000) != 0;
    bool isCompressed = (nLoopCountWord & 0x4000) != 0;

    if ((nLoops == 0) || nLoops > ReasonableLoopCount)
    {
        throw std::exception("None, or too many loops - corrupt resource?");
    }
    raster.Loops.assign(nLoops, Loop()); // Just empty ones for now
    byteStream >> mirrorMask;

    uint16_t viewVersion;
    byteStream >> viewVersion;
    uint16_t paletteOffset;
    byteStream >> paletteOffset;

    if (hasPalette)
    {
        assert(paletteOffset != 0x100 && "Unimplemented");
        sci::istream streamPalette(byteStream);
        streamPalette.seekg(paletteOffset);
        ReadPalette(resource, streamPalette);
    }

    std::unique_ptr<uint16_t[]> rgOffsets = std::make_unique<uint16_t[]>(nLoops);
    for (uint16_t i = 0; i < nLoops; i++)
    {
        byteStream >> rgOffsets[i]; // Read offset.
        if (!IsMirror(i, mirrorMask))
        {
            // Make a copy of the stream so we don't lose our position in this one.
            sci::istream streamLoop(byteStream);
            streamLoop.seekg(rgOffsets[i]);
            ReadLoopFrom(resource, streamLoop, raster.Loops[i], isVGA);
        }
    }

    // Now that we got the data from all "originals", repeat again for mirrors.
    for (uint16_t iMirror = 0; iMirror < nLoops; iMirror++)
    {
        if (IsMirror(iMirror, mirrorMask))
        {
            assert(rgOffsets[iMirror]); // Ensure we read an offset.
            // Find out of which "original" it is a mirror.
            for (uint16_t iOrig = 0; iOrig < nLoops; iOrig++)
            {
                if (!IsMirror(iOrig, mirrorMask))
                {
                    if (rgOffsets[iOrig] == rgOffsets[iMirror])
                    {
                        MirrorLoopFrom(raster.Loops[iMirror], (uint8_t)iOrig, raster.Loops[iOrig]);
                        break;
                    }
                }
            }
        }
    }

    PostReadProcessing(resource, raster);
}


#include <pshpack1.h>
struct LoopHeader_VGA11
{
    uint8_t mirrorInfo;
    uint8_t isMirror;
    uint8_t celCount;
    uint8_t dummy1;             // 0xff (typical)
    uint32_t dummy2, dummy3;    // 0x3fffffff, 0x00000000   (typical)
    uint32_t celOffsetAbsolute;
};

enum Sci32ViewNativeResolution : uint8_t
{
    Res_320x200 = 0,
    Res_640x480 = 1,
    Res_640x400 = 2,
};

struct ViewHeader_VGA11
{
    uint16_t headerSize;
    uint8_t loopCount;
    uint8_t scaleFlags;
    uint8_t always1;
    uint8_t sci32ScaleRes;
    uint16_t totalNumberOfCels; // Number of cels for which there is data (e.g. excludes mirrored loops)
    uint32_t paletteOffset;
    uint8_t loopHeaderSize;
    uint8_t celHeaderSize;
    size16 nativeResolution;    // In SQ6, at least, these are the screen dimensions (640x480). Interesting.
};
#include <poppack.h>

void ReadCelFromVGA11(sci::istream &byteStream, Cel &cel, bool isPic)
{
    CelHeader_VGA11 celHeader;
    byteStream >> celHeader;

    // Investigation
    /*
    OutputDebugString(fmt::format("something:{:x}\n", celHeader.perRowOffsets).c_str());
    {
        sci::istream moo(byteStream);
        moo.seekg(celHeader.perRowOffsets);
        int count = 20;
        while (count && moo.getBytesRemaining() > 0)
        {
            uint32_t byte;
            moo >> byte;
            OutputDebugString(fmt::format(" 0x{0:x}", byte).c_str());
            count--;
        }
        OutputDebugString("\n");
    }*/

    // LB_Dagger, view 86 has zero here. It's corrupted. All others have 0xa
    // assert(celHeader.always_0xa == 0xa);

    bool hasSplitRLEAndLiteral = (celHeader.always_0xa == 0xa) || (celHeader.always_0xa == 0x8a);
    if (hasSplitRLEAndLiteral)
    {
        assert(celHeader.offsetLiteral && celHeader.offsetRLE);
    }
    else
    {
        assert(celHeader.offsetRLE && !celHeader.offsetLiteral);
    }

    cel.size = celHeader.size;
    cel.placement = celHeader.placement;
    cel.TransparentColor = celHeader.transparentColor;

    // RLE are the encoding "instructions", while Literal is the raw data it reads from
    assert(celHeader.offsetRLE != 0);
    byteStream.seekg(celHeader.offsetRLE);
    if (celHeader.offsetLiteral == 0)
    {
        // Just copy the bits directly. AFAIK this is only for LB_Dagger views 86, 456 and 527
        size_t dataSize = celHeader.size.cx * celHeader.size.cy; // Not sure if padding happens?
        cel.Data.allocate(max(1, dataSize));
        byteStream.read_data(&cel.Data[0], dataSize);
        FlipImageData(&cel.Data[0], celHeader.size.cx, celHeader.size.cy, celHeader.size.cx);
    }
    else
    {
        ReadImageData(byteStream, cel, true, sci::istream(byteStream, celHeader.offsetLiteral));
    }
}

void ReadLoopFromVGA(ResourceEntity &resource, sci::istream &byteStream, Loop &loop, int nLoop, uint8_t celHeaderSize, bool isSCI2)
{
    g_debugCelRLE = resource.ResourceNumber;


    LoopHeader_VGA11 loopHeader;
    byteStream >> loopHeader;

    if (loopHeader.mirrorInfo == 0xff)
    {
        // Some games violate this assumption, but it's probably just a bug
        // Fails for SQ6, 2292, loop 1. Not much we can do, we'll just mark it as not a mirror. View is corrupt.
        // assert(loopHeader.isMirror == 0);
    }
    else
    {
        // Some games violate this assumption, but it's probably just a bug
        // KQ6, 138, loop 4, it's a mirror of 1
        // KQ6, 302, loop 1, it's a mirror of 0
        //assert(loopHeader.isMirror == 1);
    }

    loop.IsMirror = (loopHeader.mirrorInfo != 0xff);

    if (loop.IsMirror)
    {
        loop.MirrorOf = loopHeader.mirrorInfo;
    }

    if (!loop.IsMirror)
    {
        loop.Cels.assign(loopHeader.celCount, Cel());
        for (uint16_t i = 0; i < loopHeader.celCount; i++)
        {
            sci::istream streamCel(byteStream);
            streamCel.seekg(loopHeader.celOffsetAbsolute + celHeaderSize * i);
            ReadCelFromVGA11(streamCel, loop.Cels[i], false);
        }
    }
}

void ViewWriteToVGA11_2_Helper(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag, bool isVGA2)
{
    const RasterComponent &raster = resource.GetComponent<RasterComponent>();

    // The general format we'll be using is like this:
    // [ViewHeader]
    // [LoopHeader0 ... LoopHeadern]
    // [CelHeader0_0 ... CelHeadern_n]
    // [palette]
    // [CelRLE0 ... CelRLEn]
    // [CelLiteral0 ... CelLiteraln]

    // Saving is easier if we first just write to different streams, then combine them.
    // Palette is easy
    sci::ostream paletteStream;
    const PaletteComponent *palette = resource.TryGetComponent<PaletteComponent>();
    if (palette)
    {
        //WritePalette(paletteStream, *palette);
        WritePaletteShortForm(paletteStream, *palette);
    }
    uint32_t paletteSize = paletteStream.GetDataSize();

    // Now let's calculate how much space is used up by the headers
    uint32_t headersSize = sizeof(ViewHeader_VGA11);
    headersSize += raster.LoopCount() * sizeof(LoopHeader_VGA11);
    for (const Loop &loop : raster.Loops)
    {
        if (!loop.IsMirror)
        {
            headersSize += loop.Cels.size() * sizeof(CelHeader_VGA11);
        }
    }

    // We still can't write the headers because we don't know the image data size, and the
    // cel headers contain absolute offsets to the litewral image data, which comes after the
    // variable size rle data. We'll need to keep track of the offsets.
    vector<vector<CelHeader_VGA11>> prelimCelHeaders;
    // Now we write the actual data. In VGA1.1, this is split into two separate sections
    sci::ostream celRLEData;
    sci::ostream celRawData;

    std::unique_ptr<sci::ostream> celRowOffsets = isVGA2 ? std::make_unique<sci::ostream>() : nullptr;

    uint16_t celDataCount = 0;
    for (int nLoop = 0; nLoop < raster.LoopCount(); nLoop++)
    {
        const Loop &loop = raster.Loops[nLoop];
        if (!loop.IsMirror)
        {
            vector<CelHeader_VGA11> celHeaders;
            for (int nCel = 0; nCel < (int)loop.Cels.size(); nCel++)
            {
                CelHeader_VGA11 celHeader = {};
                celHeader.offsetRLE = celRLEData.tellp();
                celHeader.offsetLiteral = celRawData.tellp();
                WriteImageData(celRLEData, loop.Cels[nCel], true, celRawData, false);
                celHeader.rleCelDataSize = celRLEData.tellp() - celHeader.offsetRLE;
                celHeader.totalCelDataSize = (celRawData.tellp() - celHeader.offsetLiteral) + celHeader.rleCelDataSize;
                celHeaders.push_back(celHeader);

                celDataCount++;
            }
            prelimCelHeaders.push_back(celHeaders);
        }
    }

    // Now we can write the headers.
    ViewHeader_VGA11 header = { 0 };
    header.headerSize = (uint16_t)sizeof(ViewHeader_VGA11) - 2;   // - 2 because headerSize word is not included.
    header.loopCount = (uint8_t)raster.LoopCount();
    header.scaleFlags = raster.ScaleFlags;
    header.always1 = 1;
    header.sci32ScaleRes = 0;
    header.totalNumberOfCels = celDataCount;
    header.paletteOffset = palette ? headersSize : 0;             // We will write this right after the headers.
    header.loopHeaderSize = (uint8_t)sizeof(LoopHeader_VGA11);
    header.celHeaderSize = (uint8_t)sizeof(CelHeader_VGA11);
    header.nativeResolution = NativeResolutionToStoredSize(raster.Resolution);
    byteStream << header;

    // Now the loop headers
    uint32_t celHeaderStart = sizeof(ViewHeader_VGA11) + raster.LoopCount() * sizeof(LoopHeader_VGA11);
    uint32_t curCelHeaderOffset = celHeaderStart;
    for (const Loop &loop : raster.Loops)
    {
        LoopHeader_VGA11 loopHeader = { 0 };
        loopHeader.celCount = (uint8_t)loop.Cels.size();
        loopHeader.mirrorInfo = loop.MirrorOf;
        loopHeader.isMirror = loop.IsMirror ? 0x1 : 0x0;
        loopHeader.celOffsetAbsolute = curCelHeaderOffset;
        loopHeader.dummy1 = 0xff;       // Don't know what this is, but this is a typical value.
        loopHeader.dummy2 = 0x03ffffff; // Again, a typical value
        byteStream << loopHeader;
        if (!loop.IsMirror)
        {
            curCelHeaderOffset += sizeof(CelHeader_VGA11) * loop.Cels.size();
        }
    }
    assert(byteStream.tellp() == celHeaderStart);   // Assuming our calculations were correct

    std::unique_ptr<sci::ostream> rowOffsetStream;
    if (isVGA2)
    {
        rowOffsetStream = std::make_unique<sci::ostream>();
    }

    // Now the cel headers
    uint32_t rleImageDataBaseOffset = headersSize + paletteSize;
    uint32_t literalImageDataBaseOffset = rleImageDataBaseOffset + celRLEData.tellp();
    uint32_t rowOffsetOffsetBase = literalImageDataBaseOffset + celRawData.tellp();
    int realLoopIndex = 0;
    for (const Loop &loop : raster.Loops)
    {
        if (!loop.IsMirror)
        {
            for (int i = 0; i < (int)loop.Cels.size(); i++)
            {
                const Cel &cel = loop.Cels[i];
                CelHeader_VGA11 celHeader = prelimCelHeaders[realLoopIndex][i];
                celHeader.size = cel.size;
                celHeader.always_0xa = isVGA2 ? 0x8a : 0xa;
                celHeader.placement = cel.placement;
                celHeader.transparentColor = cel.TransparentColor;

                // SCI2 views contain a section at the end of the view that lists,
                // for each row of each cel in the view, the (32-bit) relative offsets in
                // the rle data and the literal data where this data begins.
                //
                // For example, if there were two cels, the data would look like:
                // offsetA:[C0_R0_offsetRLE][C0_R1_offsetRLE] ... [C0_Rn_offsetRLE]
                //         [C0_R0_offsetLiteral][C0_R1_offsetLiteral] ... [C0_Rn_offsetLiteral]
                // offsetB:[C1_R0_offsetRLE][C1_R1_offsetRLE] ... [C1_Rn_offsetRLE]
                //         [C1_R0_offsetLiteral][C1_R1_offsetLiteral] ... [C1_Rn_offsetLiteral]
                //
                // Cel #0's header's perRowOffset field would point to OffsetA, and Cel#1's would 
                // point to offsetB
                //
                // For each cel, therefore, there will this extra (celHeight * 4 * 2) bytes of data.

                if (isVGA2)
                {
                    // Read our cel data back so as to calculate offsets to the rows.
                    sci::istream rleData = sci::istream_from_ostream(celRLEData);
                    rleData.seekg(celHeader.offsetRLE);
                    sci::istream literalData = sci::istream_from_ostream(celRawData);
                    literalData.seekg(celHeader.offsetLiteral);
                    uint32_t rowOffsetOffset = rowOffsetStream->tellp();
                    // Terrible hack, copying cel!
                    Cel celTemp = {};
                    celTemp.size = cel.size;
                    celTemp.placement = cel.placement;
                    celTemp.TransparentColor = cel.TransparentColor;
                    CalculateSCI2RowOffsets(celTemp, rleData, literalData, *rowOffsetStream);
                    celHeader.perRowOffsets = rowOffsetOffset + rowOffsetOffsetBase;
                }

                celHeader.offsetRLE += rleImageDataBaseOffset;
                celHeader.offsetLiteral += literalImageDataBaseOffset;
                byteStream << celHeader;
            }
            realLoopIndex++;
        }
    }

    // Now the palette
    assert(byteStream.tellp() == headersSize);          // Since that's where we said we'll write the palette.
    sci::transfer(sci::istream_from_ostream(paletteStream), byteStream, paletteSize);

    // Now the image data
    assert(rleImageDataBaseOffset == byteStream.tellp());  // Since that's where we said we'll write the image data.
    sci::transfer(sci::istream_from_ostream(celRLEData), byteStream, celRLEData.GetDataSize());
    assert(literalImageDataBaseOffset == byteStream.tellp());
    sci::transfer(sci::istream_from_ostream(celRawData), byteStream, celRawData.GetDataSize());

    if (isVGA2)
    {
        sci::transfer(sci::istream_from_ostream(*rowOffsetStream), byteStream, celRawData.GetDataSize());
    }

    // Done!
}

void ViewWriteToVGA11(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag)
{
    ViewWriteToVGA11_2_Helper(resource, byteStream, propertyBag, false);
}

void ViewWriteToVGA2(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag)
{
    ViewWriteToVGA11_2_Helper(resource, byteStream, propertyBag, true);
}

void ViewReadFromVGA11Helper(ResourceEntity &resource, sci::istream &byteStream, const std::map<BlobKey, uint32_t> &propertyBag, bool isSCI2)
{
    RasterComponent &raster = resource.GetComponent<RasterComponent>();

    // VGA1.1 is quite different from the other versions.
    ViewHeader_VGA11 header;
    byteStream >> header;
    header.headerSize += 2;
    assert(header.headerSize >= 16);
    // Nope, it appears to be used to identify views with no image data encoding (just raw). e.g. LB_Dagger, view 86
    //assert(header.always1 == 0x1);
    raster.Resolution = StoredSizeToNativeResolution(header.nativeResolution);
    raster.ScaleFlags = header.scaleFlags;
    bool isScaleable = (raster.ScaleFlags != 0x01);

    // TODO: Turn these into an exception
    assert(header.loopHeaderSize >= sizeof(LoopHeader_VGA11));   // 16
    assert(header.celHeaderSize >= sizeof(CelHeader_VGA11));     // 32

    // 36 is common in SCI1.1, 52 is common in SCI2. But sometimes we'll see 36 in SCI2.
    assert((header.celHeaderSize == 36) || (header.celHeaderSize == 52));

    if (header.paletteOffset)
    {
        sci::istream streamPalette(byteStream);
        streamPalette.seekg(header.paletteOffset);
        ReadPalette(resource, streamPalette);
    }

    raster.Loops.assign(header.loopCount, Loop()); // Just empty ones for now
    for (int i = 0; i < header.loopCount; i++)
    {
        sci::istream streamLoop(byteStream);
        streamLoop.seekg(header.headerSize + (header.loopHeaderSize * i));  // Start of this loop's data
        ReadLoopFromVGA(resource, streamLoop, raster.Loops[i], i, header.celHeaderSize, isSCI2);
    }

    // Now fill in mirrors. They seem setup a little differently than in earlier versions of views,
    // so we aren't re-using code.
    for (Loop &loop : raster.Loops)
    {
        if (loop.IsMirror && (loop.MirrorOf < header.loopCount))
        {
            assert(loop.Cels.empty());
            Loop &origLoop = raster.Loops[loop.MirrorOf];
            for (size_t i = 0; i < origLoop.Cels.size(); i++)
            {
                // Make new empty cels, and for each one, do a "sync mirror state" if the original.
                loop.Cels.push_back(Cel());
                SyncCelMirrorState(loop.Cels[i], origLoop.Cels[i]);
            }
        }
    }

    PostReadProcessing(resource, raster);
}

void ViewReadFromVGA11(ResourceEntity &resource, sci::istream &byteStream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    ViewReadFromVGA11Helper(resource, byteStream, propertyBag, false);
}
void ViewReadFromVGA2(ResourceEntity &resource, sci::istream &byteStream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    ViewReadFromVGA11Helper(resource, byteStream, propertyBag, true);
}

void ViewReadFromEGA(ResourceEntity &resource, sci::istream &byteStream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    ViewReadFromVersioned(resource, byteStream, false);
}

void ViewReadFromVGA(ResourceEntity &resource, sci::istream &byteStream, const std::map<BlobKey, uint32_t> &propertyBag)
{
    ViewReadFromVersioned(resource, byteStream, true);
}

void ViewWriteTo(const ResourceEntity &resource, sci::ostream &byteStream, bool isVGA)
{
    const RasterComponent &raster = resource.GetComponent<RasterComponent>();
    uint16_t cLoops = (uint16_t)raster.Loops.size();
    // The wMirrorMask is used only during serialization.  Update it now.
    uint16_t wMirrorMask = 0;
    for (uint16_t i = 0; i < cLoops; i++)
    {
        if (raster.Loops[i].IsMirror)
        {
            wMirrorMask |= (1 << i);
        }
    }

    const PaletteComponent *palette = resource.TryGetComponent<PaletteComponent>();
    assert(isVGA || (palette == nullptr));

    // # of loops
    uint16_t loopCountWord = (uint16_t)cLoops;
    if (palette)
    {
        loopCountWord |= 0x8000; // Indicate this has a palette
    }
    byteStream.WriteWord(loopCountWord);
    // Mirror mask:
    byteStream.WriteWord((uint16_t)wMirrorMask);

    //byteStream.WriteBytes((uint8_t*)&raster.UnknownData, sizeof(raster.UnknownData));
    //assert(sizeof(raster.UnknownData) == 4);
    byteStream.WriteWord(0); // "version", not sure if this is used
    uint32_t paletteOffsetPosition = byteStream.tellp();
    byteStream.WriteWord(0); // This is where the palette offset might go.

    // Loop offsets:
    uint16_t wOffsets = 0;
    // Just need to take up some space for the time being.
    std::unique_ptr<uint16_t[]> rgOffsetsDummy = std::make_unique<uint16_t[]>(cLoops);
    // Stash this address for later;
    wOffsets = (uint16_t)(byteStream.tellp());
    ZeroMemory(rgOffsetsDummy.get(), sizeof(uint16_t) * cLoops);
    byteStream.WriteBytes((uint8_t*)rgOffsetsDummy.get(), sizeof(uint16_t) * cLoops);

    // First the unmirrored ones:
    for (uint16_t i = 0; i < cLoops; i++)
    {
        if (!raster.Loops[i].IsMirror)
        {
            // Set the offset:
            // (Recalc this each time, since byteStream.pData can change)
            uint16_t *pOffsets = (uint16_t*)(byteStream.GetInternalPointer() + wOffsets);
            pOffsets[i] = ((uint16_t)(byteStream.tellp()));

            // Check if this is a mirrored loop, and if so, ignore it (write the same offset as
            // the one which mirrors it.
            WriteLoopTo(resource, byteStream, raster.Loops[i], isVGA);  // isVGA: false
        }
    }

    // Then their mirrors:
    uint16_t *pOffsets = (uint16_t*)(byteStream.GetInternalPointer() + wOffsets);
    for (uint16_t i = 0; i < cLoops; i++)
    {
        if (raster.Loops[i].IsMirror)
        {
            // Set the offset - use the same offset of the cel of which we are a mirror.
            assert(raster.Loops[i].MirrorOf < cLoops);
            pOffsets[i] = pOffsets[raster.Loops[i].MirrorOf];
            assert(pOffsets[i]);
            // That's it, we're done.
        }
    }

    // Write the palette at the end
    if (palette)
    {
        if (byteStream.tellp() > 0xffff)
        {
            throw std::exception("View resource is too large");
        }
        // Write the offset to the palette
        *(reinterpret_cast<uint16_t*>(byteStream.GetInternalPointer() + paletteOffsetPosition)) = (uint16_t)byteStream.tellp();
        WritePalette(byteStream, *palette);
    }
}

void ViewWriteToEGA(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag)
{
    ViewWriteTo(resource, byteStream, false);
}

void ViewWriteToVGA(const ResourceEntity &resource, sci::ostream &byteStream, std::map<BlobKey, uint32_t> &propertyBag)
{
    ViewWriteTo(resource, byteStream, true);
}


RasterTraits viewRasterTraits =
{
    RasterCaps::Transparency | RasterCaps::Placement | RasterCaps::Mirror | RasterCaps::Reorder | RasterCaps::Resize | RasterCaps::Animate | RasterCaps::EightBitPlacement,
    OriginStyle::BottomCenter,
    320,
    200,
    PaletteType::EGA_Sixteen,
    g_egaPaletteMapping,
    g_egaColors,
    0,
    nullptr,
    false,
    0xf,    // EGA white
    0x0,    // EGA black
};

RasterTraits viewRasterTraitsVGA1 =
{
    RasterCaps::Transparency | RasterCaps::Placement | RasterCaps::Mirror | RasterCaps::Reorder | RasterCaps::Resize | RasterCaps::Animate | RasterCaps::EightBitPlacement,
    OriginStyle::BottomCenter,
    320,        // These numbers are arbitrary. They are just used to guard against large corrupt cel sizes causing us to allocate tons of memory.
    200,        // However, some Sierra games have unusually large cels (e.g. Longbow, view 80, is 193 pixels high)
    PaletteType::VGA_256,
    g_vgaPaletteMapping,
    nullptr,    // There is no hard-coded palette for VGA.
    0,
    nullptr,
    false,
    0x7,    // 7th color...
    0x0,    // Tends to be black
};

RasterTraits viewRasterTraitsVGA1_1 =
{
    RasterCaps::Transparency | RasterCaps::Placement | RasterCaps::Mirror | RasterCaps::Reorder | RasterCaps::Resize | RasterCaps::Animate,
    OriginStyle::BottomCenter,
    320,        // These numbers are arbitrary. They are just used to guard against large corrupt cel sizes causing us to allocate tons of memory.
    200,        // However, some Sierra games have unusually large cels (e.g. Longbow, view 80, is 193 pixels high)
    PaletteType::VGA_256,
    g_vgaPaletteMapping,
    nullptr,    // There is no hard-coded palette for VGA.
    0,
    nullptr,
    true,
    0x7,    // 7th color...
    0x0,    // Tends to be black
};

RasterSettings viewRasterSettings =
{
    4   // Zoom 
};

ResourceTraits viewTraitsEGA =
{
    ResourceType::View,
    &ViewReadFromEGA,
    &ViewWriteToEGA,
    &NoValidationFunc,
    nullptr
};

ResourceTraits viewTraitsVGA =
{
    ResourceType::View,
    &ViewReadFromVGA,
    &ViewWriteToVGA,
    &NoValidationFunc,
    nullptr
};

ResourceTraits viewTraitsVGA11 =
{
    ResourceType::View,
    &ViewReadFromVGA11,
    &ViewWriteToVGA11,
    &NoValidationFunc,
    nullptr
};

ResourceTraits viewTraitsVGA2 =
{
    ResourceType::View,
    &ViewReadFromVGA2,
    &ViewWriteToVGA2,
    &NoValidationFunc,
    nullptr
};

ResourceEntity *CreateViewResource(SCIVersion version)
{
    ResourceTraits *ptraits = &viewTraitsEGA;
    if (version.ViewFormat == ViewFormat::VGA1)
    {
        ptraits = &viewTraitsVGA;
    }
    else if (version.ViewFormat == ViewFormat::VGA1_1)
    {
        ptraits = &viewTraitsVGA11;
    }
    else if (version.ViewFormat == ViewFormat::VGA2)
    {
        ptraits = &viewTraitsVGA2;
    }

    RasterTraits *prasterTraits = &viewRasterTraits;
    if (version.ViewFormat == ViewFormat::VGA1)
    {
        prasterTraits = &viewRasterTraitsVGA1;
    }
    else if (version.ViewFormat == ViewFormat::VGA1_1)
    {
        prasterTraits = &viewRasterTraitsVGA1_1;
    }
    else if (version.ViewFormat == ViewFormat::VGA2)
    {
        prasterTraits = &viewRasterTraitsVGA1_1;
    }

    std::unique_ptr<ResourceEntity> pResource = std::make_unique<ResourceEntity>(*ptraits);
    pResource->AddComponent(move(make_unique<RasterComponent>(*prasterTraits, viewRasterSettings)));
    return pResource.release();
}

ResourceEntity *CreateDefaultViewResource(SCIVersion version)
{
    std::unique_ptr<ResourceEntity> pResource(CreateViewResource(version));
    // Make a view with one loop that has one cel.
    RasterComponent &raster = pResource->GetComponent<RasterComponent>();
    raster.Resolution = version.DefaultResolution;
    Loop loop;
    Cel cel;
    cel.TransparentColor = 0x3;
    loop.Cels.push_back(cel);
    raster.Loops.push_back(loop);
    ::FillEmpty(raster, CelIndex(0, 0), size16(16, 16));
    return pResource.release();
}


