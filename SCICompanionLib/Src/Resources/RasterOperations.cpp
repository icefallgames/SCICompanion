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
#include "PaletteOperations.h"
#include "ResourceEntity.h"
#include <random>

// Ok, try resizing a view resource
//
// Realloc the bits in a CelInfo due to a change in size.
// Nothing changes if the size didn't change.
// fCopy indicates whether or not to copy over the old bits.
// Note: the pBits in pCel are deleted and reallocated.
// Ensure that they are valid or NULL
// fForce says whether or not to do it even if the size didn't change.
//

bool IsValidLoopCel(const RasterComponent &raster, CelIndex celIndex)
{
    return
        celIndex.loop < (int)raster.Loops.size() &&
        celIndex.cel < (int)raster.Loops[celIndex.loop].Cels.size();
}

// Returns true if we had to clamp
bool ClampSize(const RasterComponent &raster, size16 &size)
{
    size16 original = size;
    size.cx = min(raster.Traits.MaxWidth, size.cx);
    size.cx = max(1, size.cx);
    size.cy = min(raster.Traits.MaxHeight, size.cy);
    size.cy = max(1, size.cy);
    return (original != size);
}

const Cel &GetCel(const RasterComponent &raster, int loop, int cel)
{
    return raster.Loops[loop].Cels[cel];
}

Cel &GetCel(RasterComponent &raster, int loop, int cel)
{
    return raster.Loops[loop].Cels[cel];
}

void ReallocBits(
    Cel &cel,
    size16 newSize, bool fCopy, bool fForce, bool fFill, uint8_t bColor, RasterResizeFlags flags)
{
    if ((newSize != cel.size) || fForce)
    {
        size_t cItems = PaddedSize(newSize);
        sci::array<uint8_t> newBits(cItems);
        if (fCopy || fFill)
        {
            if (fFill)
            {
                // Fill the new bitmap with the transparent colour
                newBits.fill(bColor);
            }
            if (fCopy)
            {
                uint8_t *pBitsNew = &newBits[0];   // Access to raw data
                uint8_t *pBits = &cel.Data[0];      // Access to old raw data

                // Copy from the old bitmap (y = 0 is at the bottom)
                int yEnd = min(newSize.cy, cel.size.cy);
                int cbToCopy = min(newSize.cx, cel.size.cx);
                int dy = newSize.cy - cel.size.cy;
                int xDestOffset = 0;
                int xSrcOffset = 0;

                if (RasterResizeFlags::Stretch == flags)
                {
                    // We're stretching.
                    for (int y = 0; y < newSize.cy; y++)
                    {
                        uint8_t *pLineDest = pBitsNew + CX_ACTUAL(newSize.cx) * y;
                        // Careful of overflow (should be ok, views don't tend to be bigger than 256):
                        uint8_t *pLineSrc = pBits + CX_ACTUAL(cel.size.cx) * (y * cel.size.cy / newSize.cy);
                        for (int x = 0; x < newSize.cx; x++)
                        {
                            *(pLineDest + x) = *(pLineSrc + x * cel.size.cx / newSize.cx);
                        }
                    }
                }
                else
                {
                    // Normal case
                    if (RasterResizeFlags::AnchorBottomRight == flags)
                    {
                        // We anchor on the bottom right.
                        dy = 0;
                        int dx = newSize.cx - cel.size.cx;
                        if (dx > 0)
                        {
                            // Getting bigger.
                            xDestOffset = dx;
                        }
                        else
                        {
                            xSrcOffset = -dx;
                        }
                    } // Otherwise we anchor on the top left.
                    uint8_t *pDest = (dy > 0) ? (pBitsNew + dy * CX_ACTUAL(newSize.cx)) : pBitsNew;
                    uint8_t *pSrc = (dy < 0) ? (pBits + (-dy) * CX_ACTUAL(cel.size.cx)) : pBits;
                    for (int y = 0; y < yEnd; y++)
                    {
                        CopyMemory(pDest + y * CX_ACTUAL(newSize.cx) + xDestOffset,
                            pSrc + y * CX_ACTUAL(cel.size.cx) + xSrcOffset,
                            cbToCopy);
                    }
                }
            }
        }
        // Do the replacement.
        cel.Data.swap(newBits);
        cel.size = newSize;
    }
    else
    {
        //ASSERT(fCopy == FALSE); // We could fix this - but at the moment, we aren't copying in this case.
    }
}

void ScaleInPlace(Cel &cel, int scaleFactor)
{
    size16 newSize = size16(cel.size.cx * scaleFactor, cel.size.cy * scaleFactor);
    ReallocBits(cel, newSize, true, false, false, 0, RasterResizeFlags::Stretch);
}



void ReallocBits(
    RasterComponent &raster,
    CelIndex celIndex,
    size16 newSize, bool fCopy, bool fForce, bool fFill, uint8_t bColor, RasterResizeFlags flags,
    bool validate)
{
    // Validate sizes
    if (validate)
    {
        ClampSize(raster, newSize);
    }
    ReallocBits(raster.GetCel(celIndex), newSize, fCopy, fForce, fFill, bColor, flags);
}

HBITMAP GetBitmap(
    const Cel &cel,
    const PaletteComponent *palette,
    int cx, int cy, BitmapScaleOptions scaleOptions, uint8_t bgFillColor, bool greyScale)
{
    uint16_t height = cel.size.cy;
    uint16_t width = cel.size.cx;

    HBITMAP hbm = nullptr;
    CDC dc;
    if (dc.CreateCompatibleDC(nullptr))
    {
        int iZoomIn = 1;
        int iZoomOut = 1;
        uint8_t bTransparent = 0;
        const uint8_t *pBits = &cel.Data[0];

        // Figure out how much we can zoom.  If a dimension is 0, consider it 1. (avoid / by 0)
        if (IsFlagSet(scaleOptions, BitmapScaleOptions::AllowMag))
        {
            iZoomIn = min((width ? (cx / width) : cx), (height ? (cy / height) : cy));
            iZoomIn = max(1, iZoomIn);
            iZoomIn = min(4, iZoomIn);
        }
        if (IsFlagSet(scaleOptions, BitmapScaleOptions::AllowMin))
        {
            // If a dimension is double or more than the requested, then we can zoom out.
            int xFactor = width / cx;
            int yFactor = height / cy;
            int minFactor = min(xFactor, yFactor);
            if (minFactor > 1)
            {
                iZoomOut = minFactor;
            }
        }

        const RGBQUAD *paletteColors = g_egaColors;
        int paletteCount = ARRAYSIZE(g_egaColors);
        if (palette)
        {
            paletteColors = palette->Colors;
            paletteCount = ARRAYSIZE(palette->Colors);
        }
        else if (greyScale)
        {
            paletteColors = g_greyScale16;
            paletteCount = ARRAYSIZE(g_greyScale16);
        }

        SCIBitmapInfo bmi(cx, cy, paletteColors, paletteCount);
        uint8_t *pBitsDest;
        hbm = CreateDIBSection((HDC)dc, &bmi, DIB_RGB_COLORS, (void**)&pBitsDest, NULL, 0);
        if (hbm)
        {
            // Fill with bg color.
            memset(pBitsDest, bgFillColor, CX_ACTUAL(cx) * cy);

            if ((iZoomIn == 1) && (iZoomOut == 1))
            {
                int centerXDest = max((cx - width) / 2, 0);
                int centerXSrc = max((width - cx) / 2, 0);
                // If zoom factor == 1, copy directly to the bmp
                int yOffset = height - min(cy, height); // yOffset -> if there's not enough room, copy the TOP of the view.
                for (int y = 0; y < min(cy, height); y++)
                {
                    // Copy a scan line.
                    memcpy(pBitsDest + y * CX_ACTUAL(cx) + centerXDest, pBits + (y + yOffset) * CX_ACTUAL(width) + centerXSrc, min(cx, width));
                }
            }
            else if (iZoomIn > 1)
            {
                int centerX = max((cx - (width * iZoomIn)) / 2, 0);
                // Otherwise, copy a magnified view.
                for (int y = 0; y < min(cy, height); y++)
                {
                    // Copy iZoom scanlines.
                    const uint8_t *pSrc = pBits + y * CX_ACTUAL(width);
                    for (int x = 0; x < min(cx, width); x++)
                    {
                        uint8_t b = *pSrc;
                        for (int yy = 0; yy < iZoomIn; yy++)
                        {
                            // For each row, set it at the left edge
                            uint8_t *pDest = pBitsDest + ((y * iZoomIn) + yy) * CX_ACTUAL(cx) + x * iZoomIn + centerX;
                            for (int xx = 0; xx < iZoomIn; xx++)
                            {
                                *pDest = b;
                                pDest++;
                            }

                        }
                        pSrc++; // Increment to the next column of the source
                    }
                }
            }
            else
            {
                int centerX = max((width - cx * iZoomOut) / 2, 0);
                // Copy a minified view. We don't do any filtering here, so this isn't
                // "compatible" with dithering.
                assert(iZoomOut > 1);
                for (int y = 0; y < cy; y++)
                {
                    // Copy every other iZoomOut scan line
                    const uint8_t *pSrcLine = pBits + y * CX_ACTUAL(width) * iZoomOut + centerX;
                    uint8_t *pDest = pBitsDest + y  * CX_ACTUAL(cx);
                    for (int x = 0; x < cx; x++)
                    {
                        *pDest = *(pSrcLine + x * iZoomOut);
                        pDest++;
                    }
                }
            }
        }
    }
    return hbm;
}

HBITMAP GetBitmap(
    const RasterComponent &raster,
    const PaletteComponent *palette,
    CelIndex celIndex,
    int cx, int cy, BitmapScaleOptions scaleOptions)
{
    if (IsValidLoopCel(raster, celIndex))
    {
        return GetBitmap(raster.GetCel(celIndex), palette, cx, cy, scaleOptions,
            (raster.Traits.PaletteType == PaletteType::VGA_256) ? 0xff : 0xf,
            (raster.Traits.PaletteType == PaletteType::EGA_SixteenGreyscale));
    }
    return nullptr;
}

void CopyBitmapData(
    const Cel &cel,
    uint8_t *pData,
    size16 size)
{
    assert(size == cel.size);
    memcpy(pData, &cel.Data[0], PaddedSize(cel.size));
}

// Copy to a position in large thing
void CopyBitmapData(
    const Cel &cel,
    uint8_t *pData,
    int xDest,
    int yDest,
    int stride,
    bool flip)
{
    int cy = (int)cel.size.cy;
    if (flip)
    {
        for (int y = 0; y < cy; y++)
        {
            int destIndex = (yDest + (cy - y - 1)) * stride + xDest;
            memcpy(pData + destIndex, &cel.Data[y * CX_ACTUAL(cel.size.cx)], cel.size.cx);
        }
    }
    else
    {
        for (int y = 0; y < cy; y++)
        {
            int destIndex = (yDest + y) * stride + xDest;
            memcpy(pData + destIndex, &cel.Data[y * CX_ACTUAL(cel.size.cx)], cel.size.cx);
        }
    }
}

void CopyBitmapData(
    const RasterComponent &raster,
    CelIndex index,
    uint8_t *pData,
    size16 size)
{
    CopyBitmapData(raster.GetCel(index), pData, size);
}

RasterChange InsertCel(
    RasterComponent &raster,
    CelIndex index,
    const Cel &cel)
{
    std::vector<Cel> &cels = raster.Loops[index.loop].Cels;
    cels.insert(cels.begin() + index.cel, cel);
    return RasterChange(RasterChangeHint::Loop, index);
}

void RemoveCelFromLoop(
    RasterComponent &raster,
    CelIndex celIndex)
{
    std::vector<Cel> &cels = raster.Loops[celIndex.loop].Cels;
    cels.erase(cels.begin() + celIndex.cel);
}

void MoveFromTo(
    RasterComponent &raster,
    int loopIndex,
    int celFrom, int celTo)
{
    // Assumes the cel indices are valid
    // (celTo can point to one after)
    std::vector<Cel> &cels = raster.Loops[loopIndex].Cels;
    Cel celTemp = cels[celFrom];
    cels.insert(cels.begin() + celTo, celTemp);
    // Now remove the from, which could be before or after
    if (celFrom > celTo)
    {
        cels.erase(cels.begin() + (celFrom + 1));
    }
    else
    {
        cels.erase(cels.begin() + celFrom);
    }

    // A little heavy weight, but oh well... alternately, we could check all mirrors and repeat what we did above
    UpdateMirrors(raster, loopIndex);
}

void CopyMirrored(
    Cel &cel,
    const Cel &celOrig)
{
    // And now the bits
    int cxActual = CX_ACTUAL(cel.size.cx);
    const uint8_t *pSrcBits = &celOrig.Data[0];
    uint8_t *pBits = &cel.Data[0];
    // Copy them over in reverse order
    for (int y = 0; y < celOrig.size.cy; y++)
    {
        // Start the original at the beginning of the line
        const uint8_t *pLineOrig = pSrcBits + (y * cxActual);
        // ...and the mirror at the end.
        uint8_t *pLineMirror = pBits + (y * cxActual) + (celOrig.size.cx - 1);
        for (int x = 0; x < celOrig.size.cx; x++)
        {
            *pLineMirror = *pLineOrig;
            ++pLineOrig;
            --pLineMirror;
        }
    }
}

void CopyMirrored(
    RasterComponent &raster,
    int loopIndex,
    int celIndex,
    const Cel &celOrig)
{
    CopyMirrored(GetCel(raster, loopIndex, celIndex), celOrig);
}

void CreateDegenerate(Cel &cel, uint8_t bColor)
{
    // Dummy 1x1 cel 
    cel.size = size16(1, 1);
    cel.Data.allocate(PaddedSize(cel.size));
    cel.Data.fill(bColor);
}

// Remove extraneous space, and adjust placement so it's the same.
// REVIEW: Only works horizontally for now.
bool IsTx(const Cel &cel, int x, int y)
{
    const uint8_t *dataDest = &cel.Data[0];
    return (cel.TransparentColor == *(dataDest + x + y * cel.GetStride()));
}
RasterChangeHint ShrinkWrapCel(RasterComponent &raster, CelIndex celIndex)
{
    Cel &cel = raster.GetCel(celIndex);
    int leftEmptySpace = 0;
    bool encounteredSolid = false;
    for (int x = 0; !encounteredSolid && (x < cel.size.cx); x++)
    {
        for (int y = 0; !encounteredSolid && (y < cel.size.cy); y++)
        {
            encounteredSolid = !IsTx(cel, x, y);
        }
        if (!encounteredSolid)
        {
            leftEmptySpace++;
        }
    }

    int rightEmptySpace = 0;
    encounteredSolid = false;
    for (int x = cel.size.cx - 1; !encounteredSolid && (x >= 0); x--)
    {
        for (int y = 0; !encounteredSolid && (y < cel.size.cy); y++)
        {
            encounteredSolid = !IsTx(cel, x, y);
        }
        if (!encounteredSolid)
        {
            rightEmptySpace++;
        }
    }

    // This is the offset into the thingy from top left:
    int placementCoordX = cel.size.cx / 2 - cel.placement.x;

    RasterChangeHint hint = RasterChangeHint::None;
    // Trim from left:
    size16 newLeftSize = cel.size;
    if (leftEmptySpace > 0)
    {
        placementCoordX -= leftEmptySpace; // Because it's now less offset.
        newLeftSize.cx -= leftEmptySpace;
        SetSize(raster, celIndex, newLeftSize, RasterResizeFlags::AnchorBottomRight);
        hint |= RasterChangeHint::Loop;
    }
    if (rightEmptySpace > 0)
    {
        size16 newRightSize = newLeftSize;
        newRightSize.cx -= rightEmptySpace;
        SetSize(raster, celIndex, newRightSize, RasterResizeFlags::Normal);
        hint |= RasterChangeHint::Loop;
    }

    // Update the placement
    cel.placement.x = (int16_t)(cel.size.cx / 2 - placementCoordX);

    return hint;
}

void SyncCelMirrorState(Cel &celMirror, const Cel &celOrig)
{
    // Not valid anymore, with VGA
    // assert(celOrig.TransparentColor < 16);

    celMirror.TransparentColor = celOrig.TransparentColor;
    celMirror.placement.x = -celOrig.placement.x; // Note that we invert x here!  It's a mirror!
    celMirror.placement.y = celOrig.placement.y;

    // Copy bones and x flip!
    celMirror.Bones = celOrig.Bones;
    for (auto &bone : celMirror.Bones)
    {
        bone.x = -bone.x;
    }

    ReallocBits(celMirror, celOrig.size, false, false, false, 0, RasterResizeFlags::Normal);
    CopyMirrored(celMirror, celOrig);
}

const uint8_t UpdateFromMirror = 0xff;

RasterChange MirrorLoopFrom(Loop &loop, uint8_t nOriginal, const Loop &orig)
{
    // Reallocate all cels.
    loop.Cels.clear();
    if (nOriginal != UpdateFromMirror)
    {
        loop.MirrorOf = nOriginal;
        loop.IsMirror = true;
    }
    for (size_t i = 0; i < orig.Cels.size(); i++)
    {
        // Make new empty cels, and for each one, do a "sync mirror state" if the original.
        loop.Cels.push_back(Cel());
        assert(loop.Cels.size() == (i + 1));
        SyncCelMirrorState(loop.Cels[i], orig.Cels[i]);
    }
    return RasterChange(RasterChangeHint::NewView);
}

RasterChange MakeMirrorOf(RasterComponent &raster, int nLoop, int nOriginal)
{
    int cLoops = (int)raster.Loops.size();
    if ((nLoop < cLoops) && (nOriginal < cLoops))
    {
        Loop &loop = raster.Loops[nLoop];
        if (nOriginal == -1)
        {
            loop.IsMirror = false;
            loop.MirrorOf = 0xff;
        }
        else
        {
            // Ensure that the thing we're making nLoop a mirror of, is not itself a mirror.
            // This would be a bug in our code.
            assert(!raster.Loops[nOriginal].IsMirror);
            MirrorLoopFrom(loop, (uint8_t)nOriginal, raster.Loops[nOriginal]);
        }
    }
    return RasterChange(RasterChangeHint::NewView);
}


void UpdateMirrors(RasterComponent &raster, int nLoop)
{
    UpdateMirrors(raster, CelIndex(nLoop, 0xffff));
}

// cel 0xffff means update the entire loop
void UpdateMirrors(RasterComponent &raster, CelIndex celIndex)
{
    int nLoopIgnore = -1;
    uint16_t nLoop = celIndex.loop;
    uint16_t nCel = celIndex.cel;

    // Are we a mirror of anything?  If so, update the source.
    if (raster.Loops[nLoop].IsMirror)
    {
        uint8_t bSource = raster.Loops[nLoop].MirrorOf;
        if (nCel == 0xffff)
        {
            // Replace the entire loop.
            MirrorLoopFrom(raster.Loops[bSource], UpdateFromMirror, raster.Loops[nLoop]);
        }
        else
        {
            SyncCelMirrorState(raster.Loops[bSource].Cels[nCel], raster.Loops[nLoop].Cels[nCel]);
        }
        // Now, is anyone else (but us) a mirror of this source?  See comment below.
        nLoopIgnore = nLoop;
        nLoop = bSource;
    }

    // Is anything a mirror of nLoop?  nLoop can either be the loop passed in (in the
    // case when we ourselves we're not a mirror), or can be the source loop (if we we're
    // a mirror).
    for (int i = 0; i < (int)raster.Loops.size(); i++)
    {
        if ((i != nLoop) && (i != nLoopIgnore)) // If it's not us...
        {
            if (raster.Loops[i].IsMirror && (raster.Loops[i].MirrorOf == (uint8_t)nLoop))
            {
                if (nCel == 0xffff)
                {
                    MirrorLoopFrom(raster.Loops[i], static_cast<uint8_t>(nLoop), raster.Loops[nLoop]);
                }
                else
                {
                    SyncCelMirrorState(raster.Loops[i].Cels[nCel], raster.Loops[nLoop].Cels[nCel]);
                }
            }
        }
    }
}

// Everything except data.
Cel InheritFromTemplate(const Cel &celTemplate)
{
    Cel cel;
    cel.TransparentColor = celTemplate.TransparentColor;
    cel.size = celTemplate.size;
    cel.placement = celTemplate.placement;
    return cel;
}

RasterChange InsertCel(RasterComponent &raster, CelIndex celIndex, bool before, bool dupeNewCels)
{
    if (IsValidLoopCel(raster, celIndex))
    {
        Loop &loop = raster.Loops[celIndex.loop];
        if (loop.Cels.size() >= 256)
        {
            // "too many cels".  SCI Companion doesn't support more than 128.
        }
        else
        {
            int nCelTemplate = celIndex.cel;

            // Before doing anything, prepare a new cel, based on the attributes of the template cel
            const Cel &celTemplate = loop.Cels[nCelTemplate];
            // We can just copy if we're duping new cels.. otherwise, we "inherit"
            Cel newCel = dupeNewCels ? celTemplate : InheritFromTemplate(celTemplate);
            if (!dupeNewCels)
            {
                // Continue our inheritation!  Fill with transparent colour at least.
                ReallocBits(newCel, celTemplate.size, false, true, true, celTemplate.TransparentColor, RasterResizeFlags::Normal);
            }

            int nCelInsert = nCelTemplate;
            if (!before)
            {
                nCelInsert++;
            }
            loop.Cels.insert(loop.Cels.begin() + nCelInsert, newCel);
            UpdateMirrors(raster, celIndex.loop);
        }
    }
    return RasterChange(RasterChangeHint::Loop, celIndex);
}

Loop LoopFromCelTemplate(const Cel &celTemplate)
{
    Loop newLoop;
    newLoop.Cels.push_back(celTemplate);
    return newLoop;
}

RasterChange InsertLoop(RasterComponent &raster, int nLoop, bool before)
{
    if (nLoop < (int)raster.Loops.size())
    {
        Cel &celTemplate = raster.Loops[nLoop].Cels[0]; // First cel of previous loop.
        if (!before)
        {
            nLoop++;
        }
        // Make a new loop, with one cel that is a template of the previous loops first cel.
        raster.Loops.insert(raster.Loops.begin() + nLoop, LoopFromCelTemplate(celTemplate));
    }
    return RasterChange(RasterChangeHint::NewView);
}

RasterChange MoveCelFromTo(RasterComponent &raster, int nLoop, int celFrom, int celTo)
{
    RasterChangeHint hint = RasterChangeHint::None;
    if ((celFrom != celTo) && ((celFrom + 1) != celTo)) // Those would be no-ops
    {
        Loop &loop = raster.Loops[nLoop];
        // Assumes the cel indices are valid
        // (celTo can point to one after)
        Cel celTemp = loop.Cels[celFrom];
        loop.Cels.insert(loop.Cels.begin() + celTo, celTemp);
        // Now remove the from, which could be before or after
        if (celFrom > celTo)
        {
            loop.Cels.erase(loop.Cels.begin() + (celFrom + 1));
        }
        else
        {
            loop.Cels.erase(loop.Cels.begin() + celFrom);
        }
        UpdateMirrors(raster, nLoop);
        hint = RasterChangeHint::NewView;
    }
    return RasterChange(hint);
}

RasterChange MoveLoopFromTo(RasterComponent &raster, int nLoopFrom, int nLoopTo)
{
    RasterChangeHint hint = RasterChangeHint::None;
    if ((nLoopFrom != nLoopTo) && ((nLoopFrom + 1) != nLoopTo)) // Those would be no-ops
    {
        Loop theLoop = raster.Loops[nLoopFrom];
        if (nLoopTo > nLoopFrom)
        {
            nLoopTo--;
        }

        raster.Loops.erase(raster.Loops.begin() + nLoopFrom);
        // We don't need to "update mirrors", but we do need to update the index to which things point
        // Let's do it in two stages. First, after have removed the from loop
        for (Loop &loop : raster.Loops)
        {
            if (loop.IsMirror)
            {
                if (loop.MirrorOf == nLoopFrom)
                {
                    loop.MirrorOf = 255; // Temporary
                }
                else if (loop.MirrorOf > nLoopFrom)
                {
                    loop.MirrorOf--;    // Points to one less
                } // else doesn't change
            }
        }

        // Now insert in the new position
        raster.Loops.insert(raster.Loops.begin() + nLoopTo, theLoop);

        for (Loop &loop : raster.Loops)
        {
            if (loop.IsMirror)
            {
                if (loop.MirrorOf == 255)
                {
                    loop.MirrorOf = nLoopTo;
                }
                else if (loop.MirrorOf >= nLoopTo)
                {
                    loop.MirrorOf++;
                }// else doesn't change
            }
        }

        hint = RasterChangeHint::NewView;
    }
    return RasterChange(hint);
}

RasterChange SetBitmapData(RasterComponent &raster, CelIndex celIndex, const uint8_t *data)
{
    if (IsValidLoopCel(raster, celIndex))
    {
        Cel &cel = raster.GetCel(celIndex);
        cel.Data.assign(data, data + PaddedSize(cel.size));
        UpdateMirrors(raster, celIndex);
    }
    return RasterChange(RasterChangeHint::Cel, celIndex);
}

RasterChange SetGroupBitmapData(RasterComponent &raster, int cCels, CelIndex *rgdwIndex, const uint8_t* const* ppData)
{
    for (int i = 0; i < cCels; i++)
    {
        SetBitmapData(raster, CelIndex(rgdwIndex[i]), ppData[i]);
    }
    return (cCels > 1) ? RasterChange(RasterChangeHint::Loop, rgdwIndex[0]) : RasterChange(RasterChangeHint::Cel, rgdwIndex[0]);
}

RasterChange SetSize(RasterComponent &raster, CelIndex celIndex, size16 newSize, RasterResizeFlags resizeFlags)
{
    if (IsValidLoopCel(raster, celIndex))
    {
        Cel &cel = raster.GetCel(celIndex);
        ReallocBits(raster, celIndex, newSize, true, false, true, cel.TransparentColor, resizeFlags, true);
        UpdateMirrors(raster, celIndex);
    }
    return RasterChange(RasterChangeHint::Cel, celIndex);
}

RasterChange SetGroupSize(RasterComponent &raster, int cCels, CelIndex *rgdwIndex, const size16 *rgSizes, RasterResizeFlags resizeFlags)
{
    for (int i = 0; i < cCels; i++)
    {
        ::SetSize(raster, CelIndex(rgdwIndex[i]),
            rgSizes[i],
            resizeFlags);
    }
    return (cCels > 1) ? RasterChange(RasterChangeHint::Loop) : RasterChange(RasterChangeHint::Cel, rgdwIndex[0]);
}

#define TWOPI 6.283184

void TransformRotate(double xOrigSource, double yOrigSource, double xOrigDest, double yOrigDest, double sinTheta, double cosTheta, int &x, int &y)
{
    double xOut = x - xOrigSource + 0.51; // I don't use 0.5 because that would be "unstable" when rounding at the end.
    double yOut = y - yOrigSource + 0.51;
    double xOut2 = xOut * cosTheta - yOut * sinTheta;
    double yOut2 = xOut * sinTheta + yOut * cosTheta;
    xOut2 += (double)xOrigDest - 0.51;
    yOut2 += (double)yOrigDest - 0.51;
    x = (int)round(xOut2);
    y = (int)round(yOut2);
}

void TransformRotateMinMax(double xOrigSource, double yOrigSource, double xOrigDest, double yOrigDest, double sinTheta, double cosTheta, int x, int y, int &xMin, int &yMin, int &xMax, int &yMax)
{
    TransformRotate(0, 0, 0, 0, sinTheta, cosTheta, x, y);
    xMin = min(xMin, x);
    yMin = min(yMin, y);
    xMax = max(xMax, x);
    yMax = max(yMax, y);
}

void SetPixelAt(Cel &cel, int x, int y, uint8_t value)
{
    cel.Data[y * cel.GetStride() + x] = value;
}
uint8_t GetPixelAt(const Cel &cel, int x, int y)
{
    return cel.Data[y * cel.GetStride() + x];
}

Cel Scale2X(const Cel &orig)
{
    size16 origSize = orig.size;
    origSize.cx *= 2;
    origSize.cy *= 2;
    Cel destCel(size16(origSize), point16(orig.placement), orig.TransparentColor);
    destCel.Data.allocate(destCel.size.cy * destCel.GetStride());

    for (int y = 0; y < (int)orig.size.cy; y++)
    {
        for (int x = 0; x < (int)orig.size.cx; x++)
        {
            uint8_t p = GetPixelAt(orig, x, y);
            uint8_t a = (y > 0) ? GetPixelAt(orig, x, y - 1) : p;
            uint8_t b = (x < (orig.size.cx - 1)) ? GetPixelAt(orig, x + 1, y) : p;
            uint8_t c = (x > 0) ? GetPixelAt(orig, x - 1, y) : p;
            uint8_t d = (y < (orig.size.cy - 1)) ? GetPixelAt(orig, x, y + 1) : p;

            SetPixelAt(destCel, x * 2, y * 2, (c == a && c != d && a != b ? a : p));
            SetPixelAt(destCel, x * 2 + 1, y * 2, (a == b && a != c &&  b!= d ? b : p));
            SetPixelAt(destCel, x * 2, y * 2 + 1, (d == c && d != b && c != a ? c : p));
            SetPixelAt(destCel, x * 2 + 1, y * 2 + 1, (b == d && b != a && d != c ? d : p));
        }
    }
    return destCel;
}


void RotateSingle(RasterComponent &raster, CelIndex index, int degress, bool validate)
{
    double sinTheta = sin(degress * TWOPI / 360.0);
    double cosTheta = cos(degress * TWOPI / 360.0);
    // Figure out the new size needed by transforming the four corners.
    Cel &cel = raster.GetCel(index);
    Cel celOld = cel; // We make a copy, since we replace
    int xMin = INT_MAX;
    int yMin = INT_MAX;
    int xMax = INT_MIN;
    int yMax = INT_MIN;
    int x = 0;
    int y = 0;
    TransformRotateMinMax(0, 0, 0, 0, sinTheta, cosTheta, x, y, xMin, yMin, xMax, yMax);
    x = celOld.size.cx;
    y = 0;
    TransformRotateMinMax(0, 0, 0, 0, sinTheta, cosTheta, x, y, xMin, yMin, xMax, yMax);
    x = celOld.size.cx;
    y = celOld.size.cy;
    TransformRotateMinMax(0, 0, 0, 0, sinTheta, cosTheta, x, y, xMin, yMin, xMax, yMax);
    x = 0;
    y = celOld.size.cy;
    TransformRotateMinMax(0, 0, 0, 0, sinTheta, cosTheta, x, y, xMin, yMin, xMax, yMax);
    int cx = xMax - xMin;
    int cy = yMax - yMin;

    ReallocBits(raster, index, size16((uint16_t)cx, (uint16_t)cy), true, false, true, cel.TransparentColor, RasterResizeFlags::Normal, validate);
    // SetSize(raster, index, size16((uint16_t)cx, (uint16_t)cy), RasterResizeFlags::Normal);

    // In case it was too big:
    // REVIEW: invalid now.
    cx = raster.GetCel(index).size.cx;
    cy = raster.GetCel(index).size.cy;

    // Now invert the transformation, and copy the bits over.
    sinTheta = -sinTheta;
    uint8_t *dataDest = &cel.Data[0];
    const uint8_t *dataSource = &celOld.Data[0];
    double xOrigSource = (double)celOld.size.cx * 0.5;
    double yOrigSource = (double)celOld.size.cy * 0.5;
    double xOrigDest = (double)cel.size.cx * 0.5;
    double yOrigDest = (double)cel.size.cy * 0.5;
    for (int yDest = 0; yDest < cy; yDest++)
    {
        for (int xDest = 0; xDest < cx; xDest++)
        {
            int xSource = xDest;
            int ySource = yDest;
            TransformRotate(xOrigDest, yOrigDest, xOrigSource, yOrigSource, sinTheta, cosTheta, xSource, ySource);
            uint8_t color = celOld.TransparentColor;
            if ((xSource >= 0) && (xSource < celOld.size.cx) && (ySource >= 0) && (ySource < celOld.size.cy))
            {
                color = *(dataSource + ySource * celOld.GetStride() + xSource);
            }
            *(dataDest + yDest * cel.GetStride() + xDest) = color;
        }
    }
}

// Rotation based on RotSprite algorithm: https://en.wikipedia.org/wiki/Pixel-art_scaling_algorithms#RotSprite
RasterChange RotateGroup(RasterComponent &raster, int cCels, CelIndex *rgdwIndex, int degress)
{
    degress = -degress; // Since y is flipped.
    for (int i = 0; i < cCels; i++)
    {
        Cel &cel = raster.GetCel(rgdwIndex[i]);

        // Ping pong scale up 8x
        cel = Scale2X(cel);
        cel = Scale2X(cel);
        cel = Scale2X(cel);

        // Rotate
        RotateSingle(raster, rgdwIndex[i], degress, false);

        // Shrink
        size16 origRotatedSize(cel.size.cx / 8, cel.size.cy / 8);
        SetSize(raster, rgdwIndex[i], origRotatedSize, RasterResizeFlags::Stretch);

        UpdateMirrors(raster, rgdwIndex[i]);
    }
    return (cCels > 1) ? RasterChange(RasterChangeHint::Loop) : RasterChange(RasterChangeHint::Cel, rgdwIndex[0]);
}


RasterChange RotateGroup_OLD(RasterComponent &raster, int cCels, CelIndex *rgdwIndex, int degress)
{
    degress = -degress; // Since y is flipped.
    for (int i = 0; i < cCels; i++)
    {
        RotateSingle(raster, rgdwIndex[i], degress, true);
        UpdateMirrors(raster, rgdwIndex[i]);
    }
    return (cCels > 1) ? RasterChange(RasterChangeHint::Loop) : RasterChange(RasterChangeHint::Cel, rgdwIndex[0]);
}

RasterChange FillEmpty(RasterComponent &raster, CelIndex celIndex, size16 newSize)
{
    if (IsValidLoopCel(raster, celIndex))
    {
        Cel &cel = raster.GetCel(celIndex);
        ReallocBits(raster, celIndex, newSize, false, true, true, cel.TransparentColor, RasterResizeFlags::Normal, true);
        UpdateMirrors(raster, celIndex);
    }
    return RasterChange(RasterChangeHint::Cel, celIndex);
}

RasterChange SetTransparentColor(RasterComponent &raster, CelIndex celIndex, uint8_t color)
{
    if (IsValidLoopCel(raster, celIndex))
    {
        Cel &cel = raster.GetCel(celIndex);
        cel.TransparentColor = color;
        UpdateMirrors(raster, celIndex);
    }
    return RasterChange(RasterChangeHint::Cel, celIndex);
}

RasterChange SetGroupTransparentColor(RasterComponent &raster, int cCels, CelIndex *rgdwIndex, uint8_t color)
{
    for (int i = 0; i < cCels; i++)
    {
        ::SetTransparentColor(raster, CelIndex(rgdwIndex[i]), color);
    }
    return (cCels > 1) ? RasterChange(RasterChangeHint::Loop) : RasterChange(RasterChangeHint::Cel, rgdwIndex[0]);
}

RasterChange ApplyCelsToLoop(RasterComponent &raster, int nLoop, const std::vector<Cel> &finalCels)
{
    Loop &loop = raster.Loops[nLoop];
    loop.Cels = finalCels;
    UpdateMirrors(raster, nLoop);
    return RasterChange(RasterChangeHint::NewView);
}

RasterChange ReplaceCel(RasterComponent &raster, CelIndex celIndex, const Cel &celReplacement)
{
    if (IsValidLoopCel(raster, celIndex))
    {
        Cel &cel = raster.GetCel(celIndex);
        cel = celReplacement;
        UpdateMirrors(raster, celIndex);
    }
    return RasterChange(RasterChangeHint::NewView);
}

RasterChange SetPlacement(RasterComponent &raster, CelIndex celIndex, int16_t x, int16_t y, int boneIndex)
{
    if (IsFlagSet(raster.Traits.Caps, RasterCaps::SCI0CursorPlacement))
    {
        // For SCI0, hotspot can only be at 0, 0 or 8, 8
        if ((x < 4) && (y < 4))
        {
            x = 0;
        }
        else
        {
            x = 8;
        }
        y = x;
    }
    else if (IsFlagSet(raster.Traits.Caps, RasterCaps::EightBitPlacement))
    {
        x = max(INT8_MIN, min(x, INT8_MAX));
        // -ve y placements are not allowed:
        y = max(0, min(y, INT8_MAX));
    }

    if (IsValidLoopCel(raster, celIndex))
    {
        Cel &cel = raster.GetCel(celIndex);
        if (boneIndex == -1)
        {
            cel.placement.x = x;
            cel.placement.y = y;
        }
        else
        {
            if (boneIndex < (int)cel.Bones.size())
            {
                cel.Bones[boneIndex].x = x;
                cel.Bones[boneIndex].y = y;
            }
        }
        UpdateMirrors(raster, celIndex);
    }
    return RasterChange(RasterChangeHint::Cel, celIndex);
}

RasterChange SetGroupPlacement(RasterComponent &raster, int cCels, CelIndex *rgdwIndex, int16_t x, int16_t y, int boneIndex)
{
    for (int i = 0; i < cCels; i++)
    {
        SetPlacement(raster, rgdwIndex[i], x, y, boneIndex);
    }
    return (cCels > 1) ? RasterChange(RasterChangeHint::Loop) : RasterChange(RasterChangeHint::Cel, rgdwIndex[0]);
}

HBITMAP CreateBitmapFromResource(const ResourceEntity &resource, const PaletteComponent *optionalPalette, SCIBitmapInfo *pbmi, uint8_t **ppBitsDest)
{
    return CreateBitmapFromResource(resource, CelIndex(-1, -1), optionalPalette, pbmi, ppBitsDest);
}

HBITMAP CreateBitmapFromResource(const ResourceEntity &resource, CelIndex celIndex, const PaletteComponent *optionalPalette, SCIBitmapInfo *pbmi, uint8_t **ppBitsDest)
{
    HBITMAP hbmpRet = nullptr;

    const RasterComponent &raster = resource.GetComponent<RasterComponent>();

    int startLoop = (celIndex.loop == 0xffff) ? 0 : celIndex.loop;
    int endLoop = (celIndex.loop == 0xffff) ? raster.LoopCount() : (celIndex.loop + 1);

    int startCelBase = -1;
    int endCelBase = -1;
    if (celIndex.loop != 0xffff)
    {
        startCelBase = (celIndex.cel == 0xffff) ? 0 : celIndex.cel;
        endCelBase = (celIndex.cel == 0xffff) ? (int)raster.Loops[celIndex.loop].Cels.size() : (celIndex.cel + 1);
    }

    // Generate our bitmap.
    // First, figure out the size
    int cx = 0;         // Total width
    int cy = 0;         // Total height
    int cBytesMax = 0;  // Max buffer size we need for an individual bitmap.
    std::vector<int> loopHeights;   // Height of each row
    // Flip the order of the loops, since we're drawing the bitmaps upside down, essetially.
    for (int nLoop = endLoop - 1; nLoop >= startLoop; nLoop--)
    {
        const Loop &loop = raster.Loops[nLoop];
        int cxLoop = 0;
        int cyLoop = 0;
        int startCel = (startCelBase == -1) ? 0 : startCelBase;
        int endCel = (endCelBase == -1) ? (int)loop.Cels.size() : endCelBase;
        for (int nCel = startCel; nCel < endCel; nCel++)
        {
            const Cel &cel = loop.Cels[nCel];
            cBytesMax = max(cBytesMax, PaddedSize(cel.size));
            cyLoop = max(cel.size.cy, cyLoop);
            cxLoop += cel.size.cx + 1;
        }
        cx = max(cx, cxLoop);
        cy += cyLoop + 1;
        loopHeights.push_back(cyLoop + 1);
    }

    // Subtract one from the border
    cx--;
    cy--;

    // Now we have a bitmap size.
    CDC dc;
    if (dc.CreateCompatibleDC(nullptr))
    {
        // For EGA (16 color), we'll dupe the 16 colors 16 times, to allow for using the
        // high nibble to stash extra information for export.
        const RGBQUAD *paletteEntries = g_egaColorsExtended;
        int paletteCount = ARRAYSIZE(g_egaColorsExtended);
        if (optionalPalette)
        {
            // For VGA, we don't have this "luxury"
            paletteEntries = optionalPalette->Colors;
            paletteCount = ARRAYSIZE(optionalPalette->Colors);
        }
            
        SCIBitmapInfo bmi(cx, cy, paletteEntries, paletteCount);

        BYTE *pBitsDest;
        CBitmap bitmap;
        if (bitmap.Attach(CreateDIBSection((HDC)dc, &bmi, DIB_RGB_COLORS, (void**)&pBitsDest, NULL, 0)))
        {
            memset(pBitsDest, 0x0, CX_ACTUAL(cx) * cy); // Fill with black
            std::unique_ptr<BYTE[]> buffer = std::make_unique<BYTE[]>(cBytesMax);
            int top = 0;
            for (int nLoop = endLoop - 1; nLoop >= startLoop; nLoop--)
            {
                const Loop &loop = raster.Loops[nLoop];
                int startCel = (startCelBase == -1) ? 0 : startCelBase;
                int endCel = (endCelBase == -1) ? (int)loop.Cels.size() : endCelBase;
                int left = 0;
                for (int nCel = startCel; nCel < endCel; nCel++)
                {
                    const Cel &cel = loop.Cels[nCel];
                    CopyBitmapData(cel, buffer.get(), cel.size);   // 1 for 1
                    // Copy to a temp buffer.
                    // Now copy to our actual bitmap.
                    int bottom = top + cel.size.cy;
                    for (int y = top; y < bottom; y++)
                    {
                        BYTE *pDest = pBitsDest + (CX_ACTUAL(cx) * y) + left;
                        BYTE *pSrc = buffer.get() + (y - top) * CX_ACTUAL(cel.size.cx);
                        CopyMemory(pDest, pSrc, cel.size.cx);
                        assert((pSrc - buffer.get()) < cBytesMax);
                        assert((pDest - pBitsDest) < (CX_ACTUAL(cx) * cy));
                    }
                    left += cel.size.cx + 1;
                }
                top += loopHeights[endLoop - nLoop - 1];
            }
            hbmpRet = (HBITMAP)bitmap.Detach();
            *pbmi = bmi;
            *ppBitsDest = pBitsDest;
        }
    }
    return hbmpRet;
}

// Given a loop and a deleted loop, this returns the new loop
// index.
int MakeNewLoopNumber(int nLoop, int nDeletedLoop)
{
    return (nLoop < nDeletedLoop) ? nLoop : nLoop - 1;
}

RasterChange SetScalable(RasterComponent &raster, bool scalable)
{
    raster.ScaleFlags = scalable ? 0x0 : 0x1;
    return RasterChangeHint::NewView;
}
bool IsScalable(const RasterComponent &raster)
{
    return raster.Traits.SupportsScaling && raster.ScaleFlags != 0x1;
}

RasterChange RemoveLoop(RasterComponent &raster, int nLoopRemoved)
{
    // Can only delete a loop if there is more than one.
    bool canRemove = (nLoopRemoved < (int)raster.Loops.size()) && (raster.Loops.size() > 1);
    if (canRemove)
    {
        // Now we need to update all the mirror references.
        // Anyone who was a mirror of nLoop is now no longer a mirror.
        // Anyone who was a mirror of some other loop, now needs that number adjusted.
        for (int i = 0; i < (int)raster.Loops.size(); i++)
        {
            if (raster.Loops[i].IsMirror)
            {
                int nMirrorOf = (int)raster.Loops[i].MirrorOf;
                if (nMirrorOf == nLoopRemoved)
                {
                    raster.Loops[i].IsMirror = false;
                }
                else
                {
                    raster.Loops[i].MirrorOf = (uint8_t)MakeNewLoopNumber(nMirrorOf, nLoopRemoved); // Mirror # adjusted.
                }
            }
        }
        raster.Loops.erase(raster.Loops.begin() + nLoopRemoved);
    }
    return RasterChange(canRemove ? RasterChangeHint::NewView : RasterChangeHint::None);
}

bool IsLastLoop(const RasterComponent &raster, int nLoop)
{
    bool fRet = (raster.Loops.size() == 1); // 1 loop left
    if (!fRet)
    {
        // 2 loops left, but we're a mirror, so it's essentially the last loop
        fRet = (raster.Loops.size() == 2) && raster.Loops[nLoop].IsMirror;
    }
    if (!fRet)
    {
        // One last possibility.  All other loops are mirrors of ourself.
        size_t i = 0;
        for (; i < raster.Loops.size(); ++i)
        {
            if (nLoop != (int)i)
            {
                if (!raster.Loops[i].IsMirror || (nLoop != raster.Loops[i].MirrorOf))
                {
                    break; // Nope...
                }
            }
        }
        fRet = (i == raster.Loops.size()); // true, if loop completed.
    }
    return fRet;
}

RasterChange RemoveCel(RasterComponent &raster, CelIndex celIndex)
{
    if (IsValidLoopCel(raster, celIndex))
    {
        Loop &loop = raster.Loops[celIndex.loop];
        if ((loop.Cels.size() == 1) && IsLastLoop(raster, celIndex.loop))
        {
            // Can't allow deleting.  Last cel of last loop.
            return RasterChange(RasterChangeHint::None);
        }
        else
        {
            RemoveCelFromLoop(raster, celIndex);
            if (loop.Cels.size() < 1)
            {
                // That was the last cel in the loop... remove the loop.
                // And leave any mirrors as they were (e.g. don't delete the last cel in the mirrors)
                RemoveLoop(raster, celIndex.loop);
            }
            else
            {
                // Update the mirrors
                UpdateMirrors(raster, celIndex.loop);
            }
        }
    }
    return RasterChange(RasterChangeHint::Loop, celIndex);
}


void SerializeCelRuntime(sci::ostream &out, const Cel &cel)
{
    out << cel.placement;
    out << cel.size;
    out << cel.TransparentColor;
    out << cel.Data.size();
    // Ignore bones for now?
    out.WriteBytes(&cel.Data[0], PaddedSize(cel.size));
}

void DeserializeCelRuntime(sci::istream &in, Cel &cel)
{
    assert(cel.Data.empty());

    in >> cel.placement;
    in >> cel.size;
    in >> cel.TransparentColor;
    size_t dataSize;
    in >> dataSize;
    cel.Data.allocate(dataSize);
    in.read_data(&cel.Data[0], cel.Data.size());
    cel.Bones.clear();
}

// Modulo that works with -ve numbers that can be more negative than the divisor
// 10, 3    -> 1, as expected.
// -10, 3   -> needs to be 2
int UMod(int value, int divisor)
{
    value -= (divisor * (value / divisor));
    if (value < 0)
    {
        value += divisor;
    }
    return value;
}

void AdvancedRasterCopyCel(const AdvancedRasterCopyInfo &copyInfo, const Cel &source, Cel &dest)
{
    if ((source.size.cx > dest.size.cx) || (source.size.cy > dest.size.cy))
    {
        // We don't handle this... only smaller source is ok.
        return;
    }

    for (int y = 0; y < source.size.cy; y++)
    {
        for (int x = 0; x < source.size.cx; x++)
        {
            byte data = source.Data[x + y * source.GetStride()];

            if (!copyInfo.honorTx || (data != source.TransparentColor))
            {
                int xDest = x + copyInfo.xOffset;
                int yDest = y + copyInfo.yOffset;
                if (!copyInfo.xWrap)
                {
                    xDest = max(xDest, copyInfo.xMarginLeft);
                    xDest = min(xDest, dest.size.cx - copyInfo.xMarginRight - 1);
                }
                if (!copyInfo.yWrap)
                {
                    yDest = max(yDest, copyInfo.yMarginTop);
                    yDest = min(yDest, dest.size.cy - copyInfo.yMarginBottom - 1);
                }
                xDest = UMod(xDest, dest.size.cx);
                yDest = UMod(yDest, dest.size.cy);

                int destIndex = xDest + yDest * dest.GetStride();
                byte destData = dest.Data[destIndex];

                // Do blend op.
                switch (copyInfo.blendOp)
                {
                case AdvancedRasterOp::Replace:
                    destData = data;
                    break;
                case AdvancedRasterOp::Add:
                    destData = (byte)min(255, (int)data + (int)destData);
                    break;
                case AdvancedRasterOp::Sub:
                    destData = (byte)max(0, (int)data + (int)destData);
                    break;
                case AdvancedRasterOp::Min:
                    destData = min(destData, data);
                    break;
                case AdvancedRasterOp::Max:
                    destData = max(destData, data);
                    break;
                case AdvancedRasterOp::Avg:
                    destData = (byte)(((int)destData + (int)data) / 2);
                    break;
                case AdvancedRasterOp::Mult:
                    destData = (byte)(255.0f * ((float)destData / 255.0f) * ((float)data / 255.0f));
                    break;

                case AdvancedRasterOp::Blend:
                    // We're just a single value, so we're assuming we're white and the value is alpha. So:
                    destData = data + (byte)((int)destData * (255 - (int)data) / 255);
                    break;
                }

                dest.Data[destIndex] = destData;
            }
        }
    }
}

RasterChange AdvancedRasterCopy(const AdvancedRasterCopyInfo &copyInfo, const RasterComponent &source, CelIndex celIndexSource, int cCels, RasterComponent &dest, CelIndex celIndexDest)
{
    std::random_device rd;
    std::mt19937 mt(rd());

    // offset
    // greyscale or not
    // blending op
    const std::vector<Cel> &celsSource = source.Loops[celIndexSource.loop].Cels;
    std::vector<Cel> &celsDest = dest.Loops[celIndexDest.loop].Cels;
    int sourceIndex = celIndexSource.cel;
    int destIndex = celIndexDest.cel;
    if (copyInfo.randomCel)
    {
        std::uniform_int_distribution<int32_t> distribution(0, celsDest.size() - 1);
        destIndex += distribution(mt);
    }

    AdvancedRasterCopyInfo localCopyInfo = copyInfo;
    if (copyInfo.randomOffset)
    {
        // Assume xOffset and yOffset are 0 in this case
        localCopyInfo.xOffset = 0;
        localCopyInfo.yOffset = 0;

        size16 sourceSize = celsSource[0].size;
        size16 destSize = celsDest[0].size;
        if (!localCopyInfo.xWrap)
        {
            // We need a bounds!
            int xMin = copyInfo.xMarginLeft;
            int xMax = max(xMin, destSize.cx - copyInfo.xMarginRight - sourceSize.cx);

            std::uniform_int_distribution<int32_t> distribution(xMin, xMax);
            localCopyInfo.xOffset += distribution(mt);
        }
        else
        {
            std::uniform_int_distribution<int32_t> distribution(0, destSize.cx - 1);
            localCopyInfo.xOffset += distribution(mt);

            // playing around
            //std::poisson_distribution<int> boop(destSize.cx / 2);
            //localCopyInfo.xOffset += boop(mt);
        }

        if (!localCopyInfo.yWrap)
        {
            // We need a bounds!
            int yMin = copyInfo.yMarginBottom; // y is flipped
            int yMax = max(yMin, destSize.cy - copyInfo.yMarginTop - sourceSize.cy);

            std::uniform_int_distribution<int32_t> distribution(yMin, yMax);
            localCopyInfo.yOffset += distribution(mt);
        }
        else
        {
            std::uniform_int_distribution<int32_t> distribution(0, destSize.cy - 1);
            localCopyInfo.yOffset += distribution(mt);
        }
    }

    float xFrameOffset = copyInfo.xFrameOffset;
    float yFrameOffset = copyInfo.yFrameOffset;

    if (copyInfo.randomFrameOffset)
    {
        if ((copyInfo.xFrameOffset + copyInfo.yFrameOffset) > 0)
        {
            float magnitude = sqrtf(copyInfo.xFrameOffset * copyInfo.xFrameOffset + copyInfo.yFrameOffset * copyInfo.yFrameOffset);
            // Choose a random angle
            std::uniform_int_distribution<int32_t> distribution(copyInfo.minAngle, copyInfo.maxAngle);
            int angle = distribution(mt);

            xFrameOffset = cosf((float)angle / 360.0f * 3.141592f * 2.0f) * magnitude;
            yFrameOffset = sinf((float)angle / 360.0f * 3.141592f * 2.0f) * magnitude;
        }
    }

    float xBaseOffset = (float)localCopyInfo.xOffset;
    float yBaseOffset = (float)localCopyInfo.yOffset;
    for (int i = 0; i < cCels; i++)
    {
        sourceIndex %= celsSource.size();
        destIndex %= celsDest.size();

        localCopyInfo.xOffset = (int)roundf(xBaseOffset + xFrameOffset * (float)i);
        localCopyInfo.yOffset = (int)roundf(yBaseOffset + yFrameOffset * (float)i);

        AdvancedRasterCopyCel(localCopyInfo, celsSource[sourceIndex], celsDest[destIndex]);

        // Wrap cels:
        sourceIndex++;
        destIndex++;
    }

    UpdateMirrors(dest, celIndexDest.loop);

    return RasterChange(RasterChangeHint::Loop, celIndexDest);
}

// Note: we ignore transparency
RasterChange QuantizeCel(RasterComponent &raster, CelIndex celIndex, int numLevels, bool gammaCorrected)
{
    Cel &cel = raster.GetCel(celIndex);

    for (byte &data : cel.Data)
    {
        double fData = (double)data / 255.0f;
        if (gammaCorrected)
        {
            // Helps with darker color bands
            fData = pow(fData, 1.0 / 2.2);
        }
        fData *= (double)numLevels; // now between 0 and numLevels FP
        fData = std::round(fData); // now integer between 0 and numLevels.
        fData /= (double)numLevels; // back to 0-1
        if (gammaCorrected)
        {
            fData = pow(fData, 2.2);
        }
        fData *= 255.0;
        data = (byte)min(255, fData);
    }

    UpdateMirrors(raster, celIndex);

    return RasterChange(RasterChangeHint::Loop, celIndex);
}