#include "stdafx.h"
#include "VGADither.h"

using namespace std;

struct RGBError
{
    RGBError() : r(0), g(0), b(0) {}
    int16_t r;
    int16_t g;
    int16_t b;

    RGBError operator*(int16_t m)
    {
        RGBError error = *this;
        error.r *= m;
        error.g *= m;
        error.b *= m;
        return error;
    }
    RGBError &operator+=(const RGBError &error)
    {
        r += error.r;
        g += error.g;
        b += error.b;
        return *this;
    }
};

uint8_t ClampTo8(int16_t in)
{
    return (uint8_t)min(max(in, 0), 255);
}

VGADither::VGADither(int cx, int cy)
{
    _cxE = cx + 2;   // Room for below left and right
    _cyE = cy + 1;   // Room for below
    _error = make_unique<RGBError[]>(_cxE * _cyE);
}

RGBQUAD VGADither::ApplyErrorAt(RGBQUAD rgb, int x, int y)
{
    RGBError errorAccum = _error[y * _cxE + x];
    rgb.rgbGreen = ClampTo8((int16_t)rgb.rgbGreen + (errorAccum.g / 16));
    rgb.rgbRed = ClampTo8((int16_t)rgb.rgbRed + (errorAccum.r / 16));
    rgb.rgbBlue = ClampTo8((int16_t)rgb.rgbBlue + (errorAccum.b / 16));
    return rgb;
}

void VGADither::PropagateError(RGBQUAD rgbOrig, RGBQUAD rgbChosen, int x, int y)
{
    // Propagate the error to nearby pixels using the floyd-steinberg algorithm
    RGBError errorBase;
    errorBase.r = (int16_t)rgbOrig.rgbRed - (int16_t)rgbChosen.rgbRed;
    errorBase.g = (int16_t)rgbOrig.rgbGreen - (int16_t)rgbChosen.rgbGreen;
    errorBase.b = (int16_t)rgbOrig.rgbBlue - (int16_t)rgbChosen.rgbBlue;
    _error[y * _cxE + x + 1] += errorBase * 7;        // 7 to the right
    _error[(y + 1) * _cxE + x - 1] += errorBase * 3;  // 3 below the left (CRAP, WE OVERWRITE PREV LINE)
    _error[(y + 1) * _cxE + x] += errorBase * 5;      // 5 below
    _error[(y + 1) * _cxE + x + 1] += errorBase * 1;  // 1 below to the right
}

// Define destructor explicitly, so that we don't need to include RGBError in header.
VGADither::~VGADither() {}