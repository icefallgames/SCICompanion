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
#include "RGBOctree.h"

RGBSpatial::RGBSpatial(int colorCount, const RGBQUAD *colors)
{
    memset(_counts, 0, sizeof(_counts));
    // Insert them into the tree.

    for (int i = 0; i < colorCount; i++)
    {
        RGBQUAD rgb = colors[i];
        int box = (rgb.rgbRed >> 5) | ((rgb.rgbRed >> 2) & 0x38) | (((uint16_t)rgb.rgbBlue << 1) & 0x01c0);
        _counts[box]++;
    }

}
