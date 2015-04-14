#pragma once

HBITMAP BitmapToRGB24Bitmap(const BITMAPINFO *pbmi, int desiredWidth, int desiredHeight, void **pDIBBits);
std::unique_ptr<uint8_t[]> QuantizeImage(void *pDIBBits24, int width, int stride, int height, const RGBQUAD *globalPalette, RGBQUAD *imagePaletteResult, int transparentIndex);