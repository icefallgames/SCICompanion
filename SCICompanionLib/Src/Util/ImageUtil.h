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
#pragma once

struct PaletteComponent;
struct Cel;
class ResourceBlob;
class GameFolderHelper;

namespace Gdiplus
{
    class Bitmap;
}

char g_szGdiplus8BitSaveFilter[];

enum class ColorMatching
{
    RGB = 0,
    CCIR = 1,
};

struct GIFConfiguration
{
    GIFConfiguration() : FrameTime(10), FirstFrameTime(10), LastFrameTime(10) {}
    GIFConfiguration(int frameTime, int firstFrameTime, int lastFrameTime) : FrameTime(frameTime), FirstFrameTime(firstFrameTime), LastFrameTime(lastFrameTime) {}

    int FrameTime; // In units of 0.01s.
    int FirstFrameTime;
    int LastFrameTime;
};

enum class DitherAlgorithm
{
    None = 0,
    FloydSteinberg = 1,
    JarvisJudiceNinke = 2,
    OrderedBayer = 3,
};

RGBQUAD _ToSRGB(RGBQUAD color);
RGBQUAD _ToLinear(RGBQUAD color);

enum class BitmapConvertStatus : uint32_t
{
    None = 0,
    MappedToTransparentColor = 0x0001,
};
DEFINE_ENUM_FLAGS(BitmapConvertStatus, uint32_t)

CRect GetCelRect(const Cel &cel);
bool Save8BitBmpGdiP(const char *filename, const Cel &cel, const PaletteComponent &palette, bool squishPalette = true);
std::unique_ptr<Gdiplus::Bitmap> CelAndPaletteToBitmap(const Cel &cel, const PaletteComponent &palette, bool squishPalette);
void FlipImageData(uint8_t *data, int cx, int cy, int stride);
int SquishPalette(uint8_t *data, size_t dataSize, const PaletteComponent &palette, RGBQUAD *results);
int CountActualUsedColors(const Cel &cel, bool *used);
int CountActualUsedColors(const std::vector<const Cel*> &cels, bool *used);
std::unique_ptr<PaletteComponent> GetPaletteFromImage(Gdiplus::Bitmap &bitmap, int *numberOfUsedEntriesOut = nullptr);
bool GetCelsAndPaletteFromGIFFile(const char *filename, std::vector<Cel> &cels, std::vector<PaletteComponent> &palette, PaletteComponent &globalPalette);
void SaveCelsAndPaletteToGIFFile(const char *filename, const std::vector<Cel> &cels, int colorCount, const  RGBQUAD *colors, const uint8_t *paletteMapping, uint8_t transparentIndex, const GIFConfiguration &config);
bool GetCelsAndPaletteFromGdiplus(Gdiplus::Bitmap &bitmap, uint8_t transparentColor, std::vector<Cel> &cels, PaletteComponent &palette);
bool DoPalettesMatch(const PaletteComponent &paletteA, const PaletteComponent &paletteB);
void ConvertCelToNewPalette(Cel &cel, const PaletteComponent &currentPalette, uint8_t transparentColor, bool ditherImages, int colorCount, const uint8_t *paletteMapping, const RGBQUAD *colors);
std::unique_ptr<RGBQUAD[]> ConvertGdiplusToRaw(Gdiplus::Bitmap &bitmap);
void RGBToPalettized(ColorMatching colorMatching, uint8_t *sciData, const RGBQUAD *dataOrig, int cx, int cy, bool performDither, bool gammaCorrected, int colorCount, const uint8_t *paletteMapping, const RGBQUAD *paletteColors, uint8_t transparentColor, bool excludeTransparentIndexFromMatch, BitmapConvertStatus &convertStatus);
void CutoutAlpha(DitherAlgorithm ditherAlgorithm, RGBQUAD *data, int cx, int cy, uint8_t alphaThreshold);
std::string GetGdiplusStatusString(Gdiplus::Status status);
HBITMAP Create32bbpBitmap(const Cel &cel, const RGBQUAD *palette, int paletteSize);
HBITMAP Create32bbpBitmap(const uint8_t *pData, int cxStride, int cx, int cy, uint8_t transparentColor, const RGBQUAD *palette, int paletteSize);
uint8_t FindBestPaletteIndex(const PaletteComponent &currentPalette, RGBQUAD color);
uint8_t FindBestPaletteIndexNoTx(const PaletteComponent &currentPalette, RGBQUAD color);

// Methods for encoding an SCI resource inside a windows .bmp file.
bool EncodeResourceInBitmap(const ResourceBlob &blob, const BITMAPINFO &info, uint8_t *pBits);
std::unique_ptr<ResourceBlob> Load8BitBmp(const GameFolderHelper &helper, SCIVersion version, const std::string &filename);
RGBQUAD _CombineGamma(RGBQUAD color1, RGBQUAD color2);