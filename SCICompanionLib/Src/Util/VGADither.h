#pragma once

struct RGBError;

class VGADither
{
public:
    VGADither(int cx, int cy);
    ~VGADither();
    RGBQUAD ApplyErrorAt(RGBQUAD rgb, int x, int y);
    void PropagateError(RGBQUAD rgbOrig, RGBQUAD rgbChosen, int x, int y);

private:
    std::unique_ptr<RGBError[]> _error;
    int _cxE;
    int _cyE;
};