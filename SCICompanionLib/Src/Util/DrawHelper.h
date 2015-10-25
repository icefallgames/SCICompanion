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

#pragma once

#include "Components.h"

//
// Provides an easy way to use GDI drawing commands directly on
// SCI bitmaps 
//
// Usage:
// {
//      CSCIDrawHelper helper(NULL, _pData, size);
//      SomeGdiFunction(helper.dc, param, param, param);
// }
// pData must be at least (CX_ACTUAL(size.cx) * size.cy) bytes
//
class CSCIDrawHelper
{
public:
    CSCIDrawHelper(CDC *pdc, BYTE *pData, const size16 &size, const RGBQUAD *pPalette = nullptr, int count = 0) : CSCIDrawHelper(pdc, pData, SizeToCSize(size), pPalette, count) {}
    CSCIDrawHelper(CDC *pdc, BYTE *pData, const CSize &size, const RGBQUAD *pPalette = nullptr, int count = 0);
    ~CSCIDrawHelper();

    // Public member for folks to use to draw into.
    CDC dc;

private:
    HBITMAP _hDib;
    void *_pBits;       // Our buffer.  This belongs to the _hDib, which we delete.
    BYTE *_pData;       // The buffer we affect in the end.  We don't delete this.
    HGDIOBJ _hgdiObjOld;
    CSize _size;
};
