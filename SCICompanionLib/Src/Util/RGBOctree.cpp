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
