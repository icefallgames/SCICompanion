#include "stdafx.h"
#include "VGADither.h"

using namespace std;

uint8_t ClampTo8(int16_t in)
{
    return (uint8_t)min(max(in, 0), 255);
}

RGBError CalculateError(RGBQUAD rgbOrig, RGBQUAD rgbChosen)
{
    RGBError errorBase;
    errorBase.r = (int16_t)rgbOrig.rgbRed - (int16_t)rgbChosen.rgbRed;
    errorBase.g = (int16_t)rgbOrig.rgbGreen - (int16_t)rgbChosen.rgbGreen;
    errorBase.b = (int16_t)rgbOrig.rgbBlue - (int16_t)rgbChosen.rgbBlue;
    return errorBase;
}

int16_t CalculateError(uint8_t orig, uint8_t chosen)
{
    return (int16_t)orig - (int16_t)chosen;
}

RGBQUAD AdjustWithError(RGBQUAD orig, RGBError accError, int16_t factor)
{
    orig.rgbGreen = ClampTo8((int16_t)orig.rgbGreen + (accError.g / factor));
    orig.rgbRed = ClampTo8((int16_t)orig.rgbRed + (accError.r / factor));
    orig.rgbBlue = ClampTo8((int16_t)orig.rgbBlue + (accError.b / factor));
    return orig;
}
uint8_t AdjustWithError(uint8_t orig, int16_t accError, int16_t factor)
{
    return ClampTo8((int16_t)orig + (accError / factor));
}

int16_t ClampToSpan(int16_t in, int factor)
{
    return min(128 * factor, max(in, -128 * factor));
}
RGBError ClampToSpan(RGBError in, int factor)
{
    // TODO: Don't want color shifting, so we should do a smarter clamp.
    in.b = ClampToSpan(in.b, factor);
    in.g = ClampToSpan(in.g, factor);
    in.r = ClampToSpan(in.r, factor);
    return in;
}

const OffsetAndWeight FloydSteinberg::Matrix[4] =
{
    { 1, 0, 7 },
    { -1, 1, 3 },
    { 0, 1, 5 },
    { 1, 1, 1 },
};

const OffsetAndWeight JarvisJudiceNinke::Matrix[12] =
{
    { 1, 0, 7 },
    { 2, 0, 5 },
    { -2, 1, 3 },
    { -1, 1, 5 },
    { 0, 1, 7 },
    { 1, 1, 5 },
    { 2, 1, 3 },
    { -2, 2, 1 },
    { -1, 2, 3 },
    { 0, 2, 5 },
    { 1, 2, 3 },
    { 2, 2, 1 },
};

int16_t BayerMatrix[4][4] =
{
    { 1, 9, 3, 11 },
    { 13, 5, 15, 7 },
    { 4, 12, 2, 10 },
    { 16, 8, 14, 6 },
};