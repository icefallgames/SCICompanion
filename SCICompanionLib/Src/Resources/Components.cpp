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

RasterComponent RasterComponent::CreateDegenerate()
{
    RasterComponent raster;
    Cel cel;
    ::CreateDegenerate(cel, 0);
    Loop loop;
    loop.Cels.push_back(cel);
    raster.Loops.push_back(loop);
    return raster;
}

uint8_t emptyMapping[] = { 0x0, 0xf };

RasterTraits emptyRasterTraits =
{
    RasterCaps::None,
    OriginStyle::None,
    0,
    0,
    PaletteType::EGA_Two,
    emptyMapping,
    g_egaColors,
    0,
    0,
    0,
    0,
};

RasterSettings emptyRasterSettings =
{
    1,  // Zoom
};

RasterComponent::RasterComponent() : RasterComponent(emptyRasterTraits, emptyRasterSettings) {}
Cel& RasterComponent::GetCel(CelIndex index)
{
    return Loops[index.loop].Cels[index.cel];
}
const Cel& RasterComponent::GetCel(CelIndex index) const
{
    return Loops[index.loop].Cels[index.cel];
}

const Cel& RasterComponent::GetCelFallback(CelIndex index) const
{
    uint16_t loopNumber = min(index.loop, (uint16_t)(LoopCount() - 1));
    uint16_t celNumber = min(index.cel, (uint16_t)(Loops[loopNumber].Cels.size() - 1));
    return Loops[loopNumber].Cels[celNumber];
}

void RasterComponent::ValidateCelIndex(int &loop, int &cel, bool wrap) const
{
    int cLoops = (int)Loops.size();
    if (wrap)
    {
        loop = loop % cLoops + cLoops; // Handle -ve
        loop %= cLoops;
    }
    else
    {
        loop = min(loop, cLoops - 1);
        loop = max(loop, 0);
    }

    int cCels = (int)Loops[loop].Cels.size();
    if (wrap)
    {
        cel = cel % cCels + cCels; // Handle -ve
        cel %= cCels;
    }
    else
    {
        cel = min(cel, cCels - 1);
        cel = max(cel, 0);
    }
}

int PaddedSize(size16 size)
{
    return CX_ACTUAL(size.cx) * size.cy;
}

size16 CSizeToSize(CSize size)
{
    return size16((uint16_t)size.cx, (uint16_t)size.cy);
}

CSize SizeToCSize(size16 size)
{
    return CSize(size.cx, size.cy);
}

point16 CPointToPoint(CPoint pt)
{
    assert(pt.x <= INT16_MAX && pt.x >= INT16_MIN);
    assert(pt.y <= INT16_MAX && pt.y >= INT16_MIN);
    return point16((int16_t)pt.x, (int16_t)pt.y);
}

point16 CPointToPointClamp(CPoint pt)
{
    pt.x = max(INT16_MIN, min(pt.x, INT16_MAX));
    pt.y = max(INT16_MIN, min(pt.y, INT16_MAX));
    return point16((int16_t)pt.x, (int16_t)pt.y);
}

CPoint PointToCPoint(point16 pt)
{
    return CPoint(pt.x, pt.y);
}