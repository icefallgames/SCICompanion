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
    0
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