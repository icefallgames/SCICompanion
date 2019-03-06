#pragma once

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

#include "PaletteOperations.h"

struct ResampleSettings
{
    ResampleSettings(int xWrap, int yWrap, int xBlur, int yBlur) : fXWrap(xWrap), fYWrap(yWrap), fXBlur(xBlur), fYBlur(yBlur) {}

    ResampleSettings() : fXWrap(0), fYWrap(0), fXBlur(0), fYBlur(0) {}

    int fXWrap;
    int fYWrap;
    int fXBlur;
    int fYBlur;
};

struct GreyFormat
{
    GreyFormat(float f) : _value(f) {}
    static GreyFormat ToFormat(byte b, const PaletteComponent &palette)
    {
        return GreyFormat((float)b / 255.0f);
    }
    static GreyFormat Default()
    {
        return GreyFormat(0);
    }
    byte ToByte(const PaletteComponent &palette)
    {
        return (byte)min(255, max(0, (int)(_value * 255.0f)));
    }

    void Scale(float scale)
    {
        _value *= scale;
    }
    void Add(const GreyFormat &other)
    {
        _value += other._value;
    }

    float _value;
};


struct RGBFormat
{
    RGBFormat(float r, float g, float b) : _r(r), _g(g), _b(b) {}
    static RGBFormat ToFormat(byte b, const PaletteComponent &palette)
    {
        RGBQUAD rgb = palette.Colors[b];
        return RGBFormat((float)rgb.rgbRed / 255.0f, (float)rgb.rgbGreen / 255.0f, (float)rgb.rgbBlue / 255.0f);
    }
    static RGBFormat Default()
    {
        return RGBFormat(0, 0, 0);
    }
    byte ToByte(const PaletteComponent &palette);

    void Scale(float scale)
    {
        _r *= scale;
        _g *= scale;
        _b *= scale;
    }
    void Add(const RGBFormat &other)
    {
        _r += other._r;
        _g += other._g;
        _b += other._b;
    }

    float _r, _g, _b;
};


template<class _TColorFormat, int _KernelSize>
void BlurCel(const Cel &celSource, Cel &cel, std::array<float, _KernelSize> &kernel, const ResampleSettings &settings, const PaletteComponent &palette)
{
    int k = _KernelSize / 2;
    int upperK = _KernelSize - k - 1;
    Cel celFirstPass = celSource;
    int width = celSource.size.cx;
    int widthMult = celSource.size.cx * k;
    int heightMult = celSource.size.cy * k;
    int height = celSource.size.cy;

    if (settings.fXBlur)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int indexDest = y * cel.GetStride() + x;
                _TColorFormat value = _TColorFormat::Default();
                for (int i = -k; i <= upperK; i++)
                {
                    int xTemp = (x + i);
                    xTemp = settings.fXWrap ? ((xTemp + widthMult) % width) : min((width - 1), max(0, xTemp));
                    int indexSource = y * cel.GetStride() + xTemp;
                    _TColorFormat temp = _TColorFormat::ToFormat(celSource.Data[indexSource], palette);
                    temp.Scale(kernel[i + k]);
                    value.Add(temp);
                }
                celFirstPass.Data[indexDest] = value.ToByte(palette);
            }
        }
    }
    else
    {
        celFirstPass = celSource;
    }

    if (settings.fYBlur)
    {
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                int indexDest = y * cel.GetStride() + x;
                _TColorFormat value = _TColorFormat::Default();
                for (int i = -k; i <= upperK; i++)
                {
                    int yTemp = y + i;
                    yTemp = settings.fYWrap ? ((yTemp + heightMult) % height) : min((height - 1), max(0, yTemp));
                    int indexSource = yTemp * cel.GetStride() + x;
                    _TColorFormat temp = _TColorFormat::ToFormat(celFirstPass.Data[indexSource], palette);
                    temp.Scale(kernel[i + k]);
                    value.Add(temp);

                }
                cel.Data[indexDest] = value.ToByte(palette);
            }
        }
    }
    else
    {
        cel = celFirstPass;
    }
}
