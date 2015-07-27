#pragma once

class RGBSpatial
{
public:
    RGBSpatial(int colorCount, const RGBQUAD *colors);

private:

    int _counts[8 * 8 * 8];
    //int _colors[];
    uint8_t _indexPool[256];
};