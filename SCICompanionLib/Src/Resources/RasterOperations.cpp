#include "stdafx.h"

#include "Components.h"
#include "RasterOperations.h"
#include "PaletteOperations.h"
#include "ResourceEntity.h"

// Ok, try resizing a view resource
//
// Realloc the bits in a CelInfo due to a change in size.
// Nothing changes if the size didn't change.
// fCopy indicates whether or not to copy over the old bits.
// Note: the pBits in pCel are deleted and reallocated.
// Ensure that they are valid or NULL
// fForce says whether or not to do it even if the size didn't change.
//

bool IsValidLoopCel(RasterComponent &raster, CelIndex celIndex)
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

void ReallocBits(
    RasterComponent &raster,
    CelIndex celIndex,
    size16 newSize, bool fCopy, bool fForce, bool fFill, uint8_t bColor, RasterResizeFlags flags)
{
    // Validate sizes
    ClampSize(raster, newSize);
    ReallocBits(raster.GetCel(celIndex), newSize, fCopy, fForce, fFill, bColor, flags);
}

HBITMAP GetBitmap(
    Cel &cel,
    const PaletteComponent *palette,
    int cx, int cy, BitmapScaleOptions scaleOptions, uint8_t bgFillColor)
{
    uint16_t height = cel.size.cy;
    uint16_t width = cel.size.cx;

    HBITMAP hbm = NULL;
    CDC dc;
    if (dc.CreateCompatibleDC(NULL))
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
    RasterComponent &raster,
    const PaletteComponent *palette,
    CelIndex celIndex,
    int cx, int cy, BitmapScaleOptions scaleOptions)
{
    return GetBitmap(raster.GetCel(celIndex), palette, cx, cy, scaleOptions,
        (raster.Traits.PaletteType == PaletteType::VGA_256) ? 0xff : 0xf
        );
    
}

void CopyBitmapData(
    const Cel &cel,
    uint8_t *pData,
    size16 size)
{
    assert(size == cel.size);
    memcpy(pData, &cel.Data[0], PaddedSize(cel.size));
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

void SyncCelMirrorState(Cel &celMirror, const Cel &celOrig)
{
    // Not valid anymore, with VGA
    // assert(celOrig.TransparentColor < 16);

    celMirror.TransparentColor = celOrig.TransparentColor;
    celMirror.placement.x = -celOrig.placement.x; // Note that we invert x here!  It's a mirror!
    celMirror.placement.y = celOrig.placement.y;

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
            // This is all there is to do.
            loop.IsMirror = false;
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
        if (loop.Cels.size() >= 128)
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

RasterChange MoveLoopFromTo(RasterComponent &raster, int nLoop, int celFrom, int celTo)
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
    return RasterChange(RasterChangeHint::NewView);
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
        ReallocBits(raster, celIndex, newSize, true, false, true, cel.TransparentColor, resizeFlags);
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

RasterChange FillEmpty(RasterComponent &raster, CelIndex celIndex, size16 newSize)
{
    if (IsValidLoopCel(raster, celIndex))
    {
        Cel &cel = raster.GetCel(celIndex);
        ReallocBits(raster, celIndex, newSize, false, true, true, cel.TransparentColor, RasterResizeFlags::Normal);
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

RasterChange SetPlacement(RasterComponent &raster, CelIndex celIndex, int16_t x, int16_t y)
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
        y = max(INT8_MIN, min(y, INT8_MAX));
    }

    if (IsValidLoopCel(raster, celIndex))
    {
        Cel &cel = raster.GetCel(celIndex);
        cel.placement.x = x;
        cel.placement.y = y;
        UpdateMirrors(raster, celIndex);
    }
    return RasterChange(RasterChangeHint::Cel, celIndex);
}

RasterChange SetGroupPlacement(RasterComponent &raster, int cCels, CelIndex *rgdwIndex, int16_t x, int16_t y)
{
    for (int i = 0; i < cCels; i++)
    {
        SetPlacement(raster, rgdwIndex[i], x, y);
    }
    return (cCels > 1) ? RasterChange(RasterChangeHint::Loop) : RasterChange(RasterChangeHint::Cel, rgdwIndex[0]);
}

HBITMAP CreateBitmapFromResource(const ResourceEntity &resource, const PaletteComponent *optionalPalette, SCIBitmapInfo *pbmi, uint8_t **ppBitsDest)
{
    HBITMAP hbmpRet = nullptr;

    const RasterComponent &raster = resource.GetComponent<RasterComponent>();

    // Generate our bitmap.
    // First, figure out the size
    int cLoops = (int)raster.Loops.size();
    int cx = 0;         // Total width
    int cy = 0;         // Total height
    int cBytesMax = 0;  // Max buffer size we need for an individual bitmap.
    std::vector<int> loopHeights;   // Height of each row
    // Flip the order of the loops, since we're drawing the bitmaps upside down, essetially.
    for (int nLoop = cLoops - 1; nLoop >= 0; nLoop--)
    {
        const Loop &loop = raster.Loops[nLoop];
        int cxLoop = 0;
        int cyLoop = 0;
        int cCels = (int)loop.Cels.size();
        for (int nCel = 0; nCel < cCels; nCel++)
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
            for (int nLoop = cLoops - 1; nLoop >= 0; nLoop--)
            {
                const Loop &loop = raster.Loops[nLoop];
                int cCels = (int)loop.Cels.size();;
                int left = 0;
                for (int nCel = 0; nCel < cCels; nCel++)
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
                top += loopHeights[cLoops - nLoop - 1];
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
    out << cel;
    out.WriteBytes(&cel.Data[0], PaddedSize(cel.size));
}
void DeserializeCelRuntime(sci::istream &in, Cel &cel)
{
    assert(cel.Data.empty());
    in >> cel;
    memset(&cel.Data, 0, sizeof(cel.Data)); // A bit of a hack
    cel.Data.allocate(PaddedSize(cel.size));
    in.read_data(&cel.Data[0], cel.Data.size());
}
