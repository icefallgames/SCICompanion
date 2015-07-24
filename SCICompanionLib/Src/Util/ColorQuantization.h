#pragma once

HBITMAP BitmapToRGB32Bitmap(const BITMAPINFO *pbmi, int desiredWidth, int desiredHeight, void **pDIBBits);
std::unique_ptr<uint8_t[]> QuantizeImage(const RGBQUAD *data, int width, int height, const RGBQUAD *globalPalette, RGBQUAD *imagePaletteResult, int transparentIndex);