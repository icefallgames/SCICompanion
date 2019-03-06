/***************************************************************************
    Copyright (c) 2019 Philip Fortier

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
#include "Resample.h"
#include "ImageUtil.h"

byte RGBFormat::ToByte(const PaletteComponent &palette)
{
    RGBQUAD color;
    color.rgbRed = (byte)max(0, min(255, (int)(_r * 255.0f)));
    color.rgbGreen = (byte)max(0, min(255, (int)(_g * 255.0f)));
    color.rgbBlue = (byte)max(0, min(255, (int)(_b * 255.0f)));
    return FindBestPaletteIndexNoTx(palette, color);
}