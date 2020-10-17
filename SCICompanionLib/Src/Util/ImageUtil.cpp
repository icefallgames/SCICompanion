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
#include "stdafx.h"
#include "ImageUtil.h"
#include "View.h"
#include "PaletteOperations.h"
#include "gif_lib.h"
#include "VGADither.h"
#include "RGBOctree.h"
#include "ResourceBlob.h"
#include "GameFolderHelper.h"

using namespace Gdiplus;

// Default to bmp files by default, since those are the only ones that have consistent behavior end-to-end.
extern char g_szGdiplus8BitSaveFilter[] = "Bitmap files(*.bmp)|*.bmp|GIF files(*.gif)|*.gif|PNG files(*.png)|*.png|TIFF files(*.tif;*.tiff)|*.tif;*.tiff|All Files|*.*|";

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    ImageCodecInfo* pImageCodecInfo = NULL;

    GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // Failure

    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;  // Failure

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

//image / bmp
//image / jpeg
//image / gif
//image / tiff
//image / png

struct
{
    const char *fileExt;
    const WCHAR *imageFormat;
}
g_fileExtToFormat[] =
{
    { ".bmp", L"image/bmp" },
    { ".gif", L"image/gif" },
    { ".tif", L"image/tiff" },
    { ".png", L"image/png" },
};

std::unique_ptr<Bitmap> CelAndPaletteToBitmap(const Cel &cel, const PaletteComponent &palette, bool squishPalette)
{
    // We need to flip the data, so make a copy of it and do that.
    sci::array<uint8_t> buffer(cel.Data.size());
    buffer.assign(&cel.Data[0], &cel.Data[cel.Data.size()]);
    FlipImageData(&buffer[0], cel.size.cx, cel.size.cy, CX_ACTUAL(cel.size.cx));

    RGBQUAD newPaletteColors[256];
    int usedColors = 256;
    if (squishPalette)
    {
        // Now "squish" the palette (and modify the data) so all the used colors are at the beginning.
        usedColors = SquishPalette(&buffer[0], buffer.size(), palette, newPaletteColors);
    }
    else
    {
        memcpy(newPaletteColors, palette.Colors, sizeof(newPaletteColors));
    }

    // Allocate the bitmap. We can't just give it a pointer to the buffer, because we'd need to keep that buffer
    // alive during the lifetime of the Bitmap. So we'll need to copy again!
    std::unique_ptr<Bitmap> bitmap = std::make_unique<Bitmap>(cel.size.cx, cel.size.cy, PixelFormat8bppIndexed);
    Gdiplus::Rect rect(0, 0, cel.size.cx, cel.size.cy);
    Gdiplus::BitmapData bitmapData;
    if (Ok == bitmap->LockBits(&rect, ImageLockModeRead | ImageLockModeWrite, PixelFormat8bppIndexed, &bitmapData))
    {
        memcpy(bitmapData.Scan0, &buffer[0], PaddedSize(cel.size));

        ColorPalette *pcp = (ColorPalette*)malloc(sizeof(ColorPalette) + usedColors * 4);
        pcp->Count = usedColors;
        pcp->Flags = 0;
        for (int i = 0; i < usedColors; i++)
        {
            RGBQUAD rgb = newPaletteColors[i];
            ARGB argb = (rgb.rgbRed << RED_SHIFT) | (rgb.rgbGreen << GREEN_SHIFT) | (rgb.rgbBlue << BLUE_SHIFT) | (0xff << ALPHA_SHIFT);
            pcp->Entries[i] = argb;
        }
        bitmap->SetPalette(pcp);
        free(pcp);

        bitmap->UnlockBits(&bitmapData);
    }


    return bitmap;
}

// TODO: mark unused colors and squash the bits
bool Save8BitBmpGdiP(const char *filename, const Cel &cel, const PaletteComponent &palette, bool squishPalette)
{
    std::unique_ptr<Bitmap> bitmap = CelAndPaletteToBitmap(cel, palette, squishPalette);

    // Find the encoder we want
    const char *extension = PathFindExtension(filename);
    const WCHAR *encoder = nullptr;
    for (int i = 0; i < ARRAYSIZE(g_fileExtToFormat); i++)
    {
        if (0 == strncmp(g_fileExtToFormat[i].fileExt, extension, lstrlenA(g_fileExtToFormat[i].fileExt)))
        {
            encoder = g_fileExtToFormat[i].imageFormat;
            break;
        }
    }

    // In case we couldn't identify it...
    std::string hadToAddExt;
    if (encoder == nullptr)
    {
        encoder = g_fileExtToFormat[0].imageFormat;
        hadToAddExt = filename;
        hadToAddExt += g_fileExtToFormat[0].fileExt;
        filename = hadToAddExt.c_str();
    }

    CLSID encoderClsid;
    GetEncoderClsid(encoder, &encoderClsid);

    // GDI+ only deals with unicode.
    int a = lstrlenA(filename);
    BSTR unicodestr = SysAllocStringLen(nullptr, a);
    MultiByteToWideChar(CP_ACP, 0, filename, a, unicodestr, a);
    bitmap->Save(unicodestr, &encoderClsid, nullptr);
    //... when done, free the BSTR
    SysFreeString(unicodestr);

    return true;
}

void FlipImageData(uint8_t *data, int cx, int cy, int stride)
{
    sci::array<uint8_t> buffer(cx);
    for (int y = 0; y < (cy / 2); y++)
    {
        int line1 = y;
        int line2 = cy - 1 - y;
        memcpy(&buffer[0], data + (line1 * stride), cx);
        memcpy(data + line1 * stride, data + line2 * stride, cx);
        memcpy(data + line2 * stride, &buffer[0], cx);
    }
}

// Compress the palette so all the unused colors are at the beginning.
// Returns the count of used colors, and the new values in results[256]
int SquishPalette(uint8_t *data, size_t dataSize, const PaletteComponent &palette, RGBQUAD *results)
{
    uint8_t mapOldToNew[256] = { };
    int usedColors = 0;
    for (int i = 0; i < 256; i++)
    {
        if (palette.Colors[i].rgbReserved != 0x0)
        {
            mapOldToNew[i] = usedColors;
            results[usedColors] = palette.Colors[i];
            usedColors++;
        }
    }

    for (size_t i = 0; i < dataSize; i++)
    {
        data[i] = mapOldToNew[data[i]];
    }
    
    return usedColors;
}

int CountActualUsedColorsWorker(const Cel &cel, bool *used)
{
    const uint8_t *data = &cel.Data[0];
    for (int y = cel.size.cy - 1; y >= 0; y--)
    {
        int line = y * CX_ACTUAL(cel.size.cx);
        for (int x = cel.size.cx - 1; x >= 0; x--)
        {
            used[data[line + x]] = true;
        }
    }
    return std::count(used, used + 256, true);
}

int CountActualUsedColors(const Cel &cel, bool *used)
{
    memset(used, 0, 256);
    return CountActualUsedColorsWorker(cel, used);
}

int CountActualUsedColors(const std::vector<const Cel*> &cels, bool *used)
{
    memset(used, 0, 256);
    for (const Cel *cel : cels)
    {
        CountActualUsedColorsWorker(*cel, used);
    }
    return std::count(used, used + 256, true);
}


// Our gifloader is much better at extracting palettes from gifs than gdi+, so try it first.
// REVIEW: We could use this functionality to obtain palettes from multi-frame gifs for pic backgrounds.
/*
std::unique_ptr<Gdiplus::Bitmap> _MaybeCreateBitmapFromGIF(const std::string &filename)
{
    std::unique_ptr<Gdiplus::Bitmap> bitmap;
    if (0 == lstrcmpi(".gif", PathFindExtension(filename.c_str())))
    {
        // Try our gif loader, which understands palettes better than gdip
        std::vector<Cel> cels;
        PaletteComponent palette;
        if (GetCelsAndPaletteFromGIFFile(filename.c_str(), cels, palette))
        {

        }
    }
    return bitmap;
}
*/

// Note that GDI+ doesn't support RLE bitmaps files.
// By default, SV.exe exports RLE bitmaps, but you can tell it in Preferences to output
// them uncompressed.
std::unique_ptr<PaletteComponent> GetPaletteFromImage(Gdiplus::Bitmap &bitmap, int *numberOfUsedEntriesOut)
{
    std::unique_ptr<PaletteComponent> originalPalette;
    PixelFormat pixelFormat = bitmap.GetPixelFormat();
    if ((pixelFormat & PixelFormat8bppIndexed) == PixelFormat8bppIndexed)
    {
        INT paletteSize = bitmap.GetPaletteSize();
        ColorPalette* cp = (ColorPalette*)malloc(paletteSize);
        Status status = bitmap.GetPalette(cp, paletteSize);
        if (status == Ok)
        {
            int numberOfPaletteEntries = (int)cp->Count;
            if (numberOfUsedEntriesOut)
            {
                *numberOfUsedEntriesOut = numberOfPaletteEntries;
            }
            originalPalette = std::make_unique<PaletteComponent>();
            for (int i = 0; i < numberOfPaletteEntries; i++)
            {
                ARGB sourcePaletteEntry = cp->Entries[i];
                RGBQUAD rgb;
                rgb.rgbRed = 0xff & (sourcePaletteEntry >> RED_SHIFT);
                rgb.rgbGreen = 0xff & (sourcePaletteEntry >> GREEN_SHIFT);
                rgb.rgbBlue = 0xff & (sourcePaletteEntry >> BLUE_SHIFT);
                rgb.rgbReserved = 0x1; // Or 0x3??? REVIEW
                originalPalette->Colors[i] = rgb;
            }
            RGBQUAD unused = { 0 };
            for (int i = numberOfPaletteEntries; i < 256; i++)
            {
                originalPalette->Colors[i] = unused;
            }
        }
        free(cp);
    }
    return originalPalette;
}

bool GetCelsAndPaletteFromGIFFile(const char *filename, std::vector<Cel> &cels, std::vector<PaletteComponent> &palettes, PaletteComponent &globalPalette)
{
    int errorCode;
    GifFileType *fileType = DGifOpenFileName(filename, &errorCode);
    bool success = (DGifSlurp(fileType) == GIF_OK);
    if (success)
    {
        int bottom = fileType->Image.Top + fileType->Image.Height;
        int center = fileType->Image.Left + fileType->Image.Width / 2;  // Assume this is the center.

        // Get the palette.
        memset(globalPalette.Colors, 0, sizeof(globalPalette.Colors));
        if (fileType->SColorMap)
        {
            for (int i = 0; i < fileType->SColorMap->ColorCount; i++)
            {
                globalPalette.Colors[i].rgbRed = fileType->SColorMap->Colors[i].Red;
                globalPalette.Colors[i].rgbBlue = fileType->SColorMap->Colors[i].Blue;
                globalPalette.Colors[i].rgbGreen = fileType->SColorMap->Colors[i].Green;
                globalPalette.Colors[i].rgbReserved = 0x1;    // Or 0x3? REVIEW
            }
        }

        for (int i = 0; i < fileType->ImageCount; i++)
        {
            SavedImage &savedImage = fileType->SavedImages[i];

            bool initializeWithPrevious = false;

            // This cel has its own palette
            if (savedImage.ImageDesc.ColorMap)
            {
                palettes.emplace_back();
                PaletteComponent &celPalette = palettes.back();
                for (int i = 0; i < savedImage.ImageDesc.ColorMap->ColorCount; i++)
                {
                    celPalette.Colors[i].rgbRed = savedImage.ImageDesc.ColorMap->Colors[i].Red;
                    celPalette.Colors[i].rgbBlue = savedImage.ImageDesc.ColorMap->Colors[i].Blue;
                    celPalette.Colors[i].rgbGreen = savedImage.ImageDesc.ColorMap->Colors[i].Green;
                    celPalette.Colors[i].rgbReserved = 0x1;    // Or 0x3? REVIEW
                }
            }

            cels.emplace_back();
            Cel &cel = cels.back();

            GraphicsControlBlock gcb;
            uint8_t transparentColor = fileType->SBackGroundColor;
            if (GIF_ERROR != DGifSavedExtensionToGCB(fileType, i, &gcb))
            {
                // REVIEW: We may want to support DISPOSE_PREVIOUS too - which could be done by going further back.
                initializeWithPrevious = (gcb.DisposalMode == DISPOSE_DO_NOT) && (i > 0);
                transparentColor = gcb.TransparentColor;
            }

            if (initializeWithPrevious)
            {
                // NOTE: This doesn't work when each cel has its own palette. We'd need to go and find the closest RGB color.
                cel = cels[i - 1]; // That's easy
            }
            else
            {
                cel.size = size16((uint16_t)fileType->SWidth, (uint16_t)fileType->SHeight);
                cel.Data.allocate(CX_ACTUAL(cel.size.cx) * cel.size.cy);
                cel.Data.fill(transparentColor);
            }
            cel.TransparentColor = transparentColor;

            int top = savedImage.ImageDesc.Top;
            int left = savedImage.ImageDesc.Left;
            for (int y = 0; y < savedImage.ImageDesc.Height; y++)
            {
                int yUpsideDown = savedImage.ImageDesc.Height - y - 1;
                int yDest = (y + cel.size.cy - top - savedImage.ImageDesc.Height);
                if ((yDest >= 0) && (yDest < cel.size.cy))
                {
                    uint8_t *dest = &cel.Data[yDest * CX_ACTUAL(cel.size.cx) + left];
                    uint8_t *src = savedImage.RasterBits + yUpsideDown * savedImage.ImageDesc.Width;
                    for (int x = 0; x < savedImage.ImageDesc.Width; x++, dest++, src++)
                    {
                        uint8_t sourceValue = *src;
                        if (sourceValue != transparentColor)
                        {
                            *dest = *src;
                        }
                    }
                }
            }

            int centerCurrent = savedImage.ImageDesc.Left + savedImage.ImageDesc.Width / 2;
            cel.placement.x = (int16_t)(centerCurrent - center);
            cel.placement.y = (int16_t)(savedImage.ImageDesc.Top + savedImage.ImageDesc.Height - bottom);

        }
    }

    DGifCloseFile(fileType, &errorCode);
    return success;
}

char appExtensionBlockData[] = "NETSCAPE2.0";
char continuationBlockData[] = "\1\0\0";

void SaveCelsAndPaletteToGIFFile(const char *filename, const std::vector<Cel> &cels, int colorCount, const RGBQUAD *colors, const uint8_t *paletteMapping, uint8_t transparentIndex, const GIFConfiguration &config)
{
    int error;
    GifFileType *fileType = EGifOpenFileName(filename, false, &error);
    if (fileType)
    {
        fileType->ImageCount = (int)cels.size();
        fileType->SWidth = 0;
        fileType->SHeight = 0;
        CRect rectEntire;

        // We keep our own storage for the SavedImages. Unfortunately giflib's memory management model seems to be
        // completely broken, so we have to hack around it.
        std::unique_ptr<SavedImage[]> savedImages = std::make_unique<SavedImage[]>(fileType->ImageCount);
        std::vector<std::unique_ptr<GifByteType[]>> rasterData;

        fileType->SavedImages = savedImages.get();
        memset(fileType->SavedImages, 0, sizeof(SavedImage) * fileType->ImageCount);
        for (const Cel &cel : cels)
        {
            CRect rect = GetCelRect(cel);
            // Not sure if I call UnionRect on the same rect as one of the sources... :-(
            CRect rectEntireTemp;
            rectEntireTemp.UnionRect(&rect, &rectEntire);
            rectEntire = rectEntireTemp;
        }

        fileType->SWidth = rectEntire.Width();
        fileType->SHeight = rectEntire.Height();

        // Offset it so it's at 0/0
        CPoint offset(-rectEntire.left, -rectEntire.top);
        rectEntire.OffsetRect(offset);

        fileType->SColorResolution = 8;
        fileType->Image.Interlace = false;
        // Not sure what these 4 things are? When loading gifs they are different than SWidth, SHEight
        fileType->Image.Left = rectEntire.left;
        fileType->Image.Top = rectEntire.top;
        fileType->Image.Width = rectEntire.Width();
        fileType->Image.Height = rectEntire.Height();

        GifColorType gifColors[256];
        for (int i = 0; i < colorCount; i++)
        {
            RGBQUAD rgb = colors[paletteMapping[i]];
            gifColors[i].Red = rgb.rgbRed;
            gifColors[i].Green = rgb.rgbGreen;
            gifColors[i].Blue = rgb.rgbBlue;
        }
        fileType->SColorMap = GifMakeMapObject(colorCount, gifColors);
        fileType->SColorMap->SortFlag = true;   // SV.exe includes it, so I will too.

        // Now the images
        int result = GIF_OK;
        for (size_t i = 0; (result == GIF_OK) && (i < cels.size()); i++)
        {
            const Cel &cel = cels[i];
            CRect celRect = GetCelRect(cel);
            celRect.OffsetRect(offset);
            fileType->SavedImages[i].ExtensionBlockCount = 0;
            fileType->SavedImages[i].ImageDesc.ColorMap = nullptr;
            fileType->SavedImages[i].ImageDesc.Interlace = false;
            fileType->SavedImages[i].ImageDesc.Left = celRect.left;
            fileType->SavedImages[i].ImageDesc.Top = celRect.top;
            fileType->SavedImages[i].ImageDesc.Width = celRect.Width();
            fileType->SavedImages[i].ImageDesc.Height = celRect.Height();

            rasterData.push_back(std::make_unique<GifByteType[]>(cel.size.cx * cel.size.cy));
            fileType->SavedImages[i].RasterBits = rasterData.back().get();

            for (int y = 0; y < cel.size.cy; y++)
            {
                int yUpsideDown = fileType->SavedImages[i].ImageDesc.Height - y - 1;
                const uint8_t *src = &cel.Data[y * CX_ACTUAL(cel.size.cx)];
                uint8_t *dest = fileType->SavedImages[i].RasterBits + yUpsideDown * fileType->SavedImages[i].ImageDesc.Width;
                memcpy(dest, src, fileType->SavedImages[i].ImageDesc.Width);
            }

            // Just copying what SV.exe does
            // First image index has a APPLICATION_EXT_FUNC_CODE block, which indicates looping.
            if (i == 0)
            {
                result = GifAddExtensionBlock(&fileType->SavedImages[i].ExtensionBlockCount,
                    &fileType->SavedImages[i].ExtensionBlocks,
                    APPLICATION_EXT_FUNC_CODE,
                    sizeof(appExtensionBlockData) - 1,
                    reinterpret_cast<unsigned char*>(appExtensionBlockData));

                if (result == GIF_OK)
                {
                    result = GifAddExtensionBlock(&fileType->SavedImages[i].ExtensionBlockCount,
                        &fileType->SavedImages[i].ExtensionBlocks,
                        CONTINUE_EXT_FUNC_CODE,
                        sizeof(continuationBlockData) - 1,
                        reinterpret_cast<unsigned char*>(continuationBlockData));
                }
            }

            if (result == GIF_OK)
            {
                int frameTime = config.FrameTime;
                if (i == 0)
                {
                    frameTime = config.FirstFrameTime;
                }
                else if (i == (cels.size() - 1))
                {
                    frameTime = config.LastFrameTime;
                }
                GraphicsControlBlock gcb;
                gcb.DelayTime = frameTime;
                gcb.DisposalMode = 2;
                gcb.TransparentColor = cel.TransparentColor;
                gcb.UserInputFlag = false;
                result = EGifGCBToSavedExtension(&gcb, fileType, i);
            }
        }

        if (result == GIF_OK)
        {
            result = EGifSpew(fileType);
            // Amazingly, this deallocates internal storage for nearly everything, except for the SavedImage data.
            // We can't delete the SavedImage data afterwards, because the fileType struct contents will be garbage.
            // We can't delete them before, because they are required for EGifSpew. What a silly API.
        }
        if (result != GIF_OK)
        {
            // If an error happens, then I have to close the file? What on earth?
            int closeError;
            EGifCloseFile(fileType, &closeError);
        }
        
        if (result != GIF_OK)
        {
            AfxMessageBox("There was an error writing the gif.", MB_OK | MB_ICONWARNING);
        }

        // Even though we stored our own SavedImage and the raster data inside it, we (necessarily) didn't manage
        // the memory for the extension blocks in our SavedImages. So we need to free them with giflib now.
        // Yuck.
        for (size_t i = 0; i < cels.size(); i++)
        {
            SavedImage &savedImageTemp = savedImages[i];
            GifFreeExtensions(&savedImageTemp.ExtensionBlockCount, &savedImageTemp.ExtensionBlocks);
        }
    }
    else
    {
        AfxMessageBox("There was an error opening the gif file for writing.", MB_OK | MB_ICONWARNING);
    }
}

bool GetCelsAndPaletteFromGdiplus(Gdiplus::Bitmap &bitmap, uint8_t transparentColor, std::vector<Cel> &cels, PaletteComponent &palette)
{
    std::unique_ptr<PaletteComponent> tempPalette = GetPaletteFromImage(bitmap, nullptr);
    bool success = (tempPalette != nullptr);
    if (success)
    {
        success = false;
        UINT cx = bitmap.GetWidth();
        UINT cy = bitmap.GetHeight();
        Gdiplus::Rect rect(0, 0, cx, cy);

        Gdiplus::BitmapData bitmapData;
        if (Ok == bitmap.LockBits(&rect, ImageLockModeRead, PixelFormat8bppIndexed, &bitmapData))
        {
            success = true;
            cels.emplace_back();
            Cel &cel = cels.back();
            cel.size = size16((uint16_t)cx, (uint16_t)cy);
            cel.TransparentColor = transparentColor;
            uint8_t *pDIBBits8 = (uint8_t *)bitmapData.Scan0;
            cel.Data.allocate(PaddedSize(cel.size));
            cel.Data.assign(pDIBBits8, pDIBBits8 + cel.Data.size());

            bitmap.UnlockBits(&bitmapData); // REVIEW: Not exception-safe

            // Transfer the palette now too.
            palette = *tempPalette;
        }
    }
    return success;
}

bool DoPalettesMatch(const PaletteComponent &paletteA, const PaletteComponent &paletteB)
{
    bool match = true;
    // Ignore the "used" bits
    for (size_t i = 0; match && (i < ARRAYSIZE(paletteA.Colors)); i++)
    {
        match = (paletteA.Colors[i].rgbRed == paletteB.Colors[i].rgbRed) &&
            (paletteA.Colors[i].rgbGreen == paletteB.Colors[i].rgbGreen) &&
            (paletteA.Colors[i].rgbBlue == paletteB.Colors[i].rgbBlue);
    }
    return match;
}

const uint8_t AlphaThreshold = 128;

// Compare the difference of two RGB values, weigh by CCIR 601 luminosity:
double GetColorDistanceCCIR(RGBQUAD one, RGBQUAD two)
{
    int r1 = one.rgbRed;
    int g1 = one.rgbGreen;
    int b1 = one.rgbBlue;
    int r2 = two.rgbRed;
    int g2 = two.rgbGreen;
    int b2 = two.rgbBlue;
    double luma1 = (r1 * 299 + g1 * 587 + b1 * 114) / (255.0 * 1000);
    double luma2 = (r2 * 299 + g2 * 587 + b2 * 114) / (255.0 * 1000);
    double lumadiff = luma1 - luma2;
    double diffR = (r1 - r2) / 255.0, diffG = (g1 - g2) / 255.0, diffB = (b1 - b2) / 255.0;
    return (diffR*diffR*0.299 + diffG*diffG*0.587 + diffB*diffB*0.114)*0.75
        + lumadiff*lumadiff;
}

template<typename _TCompare>
uint8_t _FindBestPaletteIndexMatch(_TCompare compare, uint8_t transparentColor, bool excludeTransparentIndexFromMatching, RGBQUAD desiredColor, int colorCount, const uint8_t *mapping, const RGBQUAD *colors)
{
    int bestIndex = 1;  // Just something that's not zero, so we can determine when we failed.
    double bestDistance = 1000000000000000.0;
    for (int i = 0; i < colorCount; i++)
    {
        RGBQUAD paletteColor = colors[mapping[i]];
        if ((!excludeTransparentIndexFromMatching || (i != (int)transparentColor)) &&
            (paletteColor.rgbReserved != 0x0))
        {
            double distance = compare(desiredColor, paletteColor);
            if (distance < bestDistance)
            {
                bestIndex = i;
                bestDistance = distance;
            }
        }
    }
    return (uint8_t)bestIndex;
}

uint8_t FindBestPaletteIndex(const PaletteComponent &currentPalette, RGBQUAD color)
{
    return _FindBestPaletteIndexMatch(GetColorDistanceRGB, 0xff, true, color, 256, currentPalette.Mapping, currentPalette.Colors);;
}
uint8_t FindBestPaletteIndexNoTx(const PaletteComponent &currentPalette, RGBQUAD color)
{
    return _FindBestPaletteIndexMatch(GetColorDistanceRGB, 0xff, false, color, 256, currentPalette.Mapping, currentPalette.Colors);;
}

void ConvertCelToNewPalette(Cel &cel, const PaletteComponent &currentPalette, uint8_t transparentColor, bool egaDither, int colorCount, const uint8_t *paletteMapping, const RGBQUAD *colors)
{
    int height = cel.size.cy;
    int width = cel.size.cx;
    for (int y = 0; y < height; y++)
    {
        uint8_t *pBitsRow = &cel.Data[0] + ((height - y - 1) * CX_ACTUAL(width));
        for (int x = 0; x < width; x++)
        {
            uint8_t value = *(pBitsRow + x);
            uint8_t *setValuePointer = pBitsRow + x;
            if (value == cel.TransparentColor)
            {
                *setValuePointer = transparentColor;
            }
            else
            {
                RGBQUAD rgbExisting = currentPalette.Colors[value];
                // find closest match.
                uint8_t bestMatch = _FindBestPaletteIndexMatch(GetColorDistanceRGB, transparentColor, true, rgbExisting, colorCount, paletteMapping, colors);;
                *setValuePointer = bestMatch;
            }
        }
    }

    cel.TransparentColor = transparentColor;
}

std::unique_ptr<RGBQUAD[]> ConvertGdiplusToRaw(Gdiplus::Bitmap &bitmap)
{
    std::unique_ptr<RGBQUAD[]> data;
    UINT cx = bitmap.GetWidth();
    UINT cy = bitmap.GetHeight();
    Gdiplus::Rect rect(0, 0, cx, cy);
    Gdiplus::BitmapData bitmapData;
    if (Ok == bitmap.LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bitmapData))
    {
        data = std::make_unique<RGBQUAD[]>(cx * cy);
        uint8_t *pDIBBits24 = (uint8_t *)bitmapData.Scan0;
        for (UINT y = 0; y < cy; y++)
        {
            for (UINT x = 0; x < cx; x++)
            {
                uint8_t *source = pDIBBits24 + (x * 4) + y * bitmapData.Stride;
                RGBQUAD rgb = { *source, *(source + 1), *(source + 2), *(source+ 3) };
                data[y * cx + x] = rgb;
            }
        }
        bitmap.UnlockBits(&bitmapData);
    }
    return data;
}

// This assumes cutout alpha.
void RGBToPalettized(ColorMatching colorMatching, uint8_t *sciData, const RGBQUAD *dataOrig, int cx, int cy, bool performDither, bool gammaCorrected, int colorCount, const uint8_t *paletteMapping, const RGBQUAD *paletteColors, uint8_t transparentColor, bool excludeTransparentIndexFromMatch, BitmapConvertStatus &convertStatus)
{
    ErrorDiffusionDither<RGBQUAD, FloydSteinberg> dither(cx, cy);
    for (int y = 0; y < cy; y++)
    {
        const RGBQUAD *origRow = dataOrig + y * cx;
        uint8_t *destRow = sciData + y * CX_ACTUAL(cx);
        for (int x = 0; x < cx; x++)
        {
            RGBQUAD rgbOrig = gammaCorrected ? _ToLinear(origRow[x]) : origRow[x];
            rgbOrig = dither.ApplyErrorAt(rgbOrig, x, y);
            if (rgbOrig.rgbReserved == 0xff)
            {
                uint8_t bestMatch =
                    colorMatching == ColorMatching::RGB ?
                    _FindBestPaletteIndexMatch(GetColorDistanceRGB, transparentColor, excludeTransparentIndexFromMatch, rgbOrig, colorCount, paletteMapping, paletteColors) :
                    _FindBestPaletteIndexMatch(GetColorDistanceCCIR, transparentColor, excludeTransparentIndexFromMatch, rgbOrig, colorCount, paletteMapping, paletteColors);
                destRow[x] = bestMatch;
                if (performDither)
                {
                    dither.PropagateError(rgbOrig, paletteColors[bestMatch], x, y);
                }
                if (bestMatch == transparentColor)
                {
                    convertStatus |= BitmapConvertStatus::MappedToTransparentColor;
                }
            }
            else
            {
                destRow[x] = transparentColor;
                // And no need to propagate error...
            }
        }
    }
}

// Turns the alpha channel into a discrete on/off channel.
template<typename _TAlgorithm>
void CutoutAlpha(RGBQUAD *data, int cx, int cy, uint8_t alphaThreshold)
{
    _TAlgorithm dither(cx, cy);
    for (int y = 0; y < cy; y++)
    {
        for (int x = 0; x < cx; x++)
        {
            uint8_t alphaOrig = data[y * cx + x].rgbReserved;
            uint8_t alpha = 0;
            // Prevent completely transparent pixels from becoming opaque.
            if (alphaOrig)
            { 
                alpha = dither.ApplyErrorAt(alphaOrig, x, y);
            }
            bool isTransparent = alpha < alphaThreshold;
            data[y * cx + x].rgbReserved = isTransparent ? 0x00 : 0xff;
            dither.PropagateError(alpha, isTransparent ? 0x00 : 0xff, x, y);
        }
    }
}

void CutoutAlpha(DitherAlgorithm ditherAlgorithm, RGBQUAD *data, int cx, int cy, uint8_t alphaThreshold)
{
    switch (ditherAlgorithm)
    {
        case DitherAlgorithm::FloydSteinberg:
            CutoutAlpha<ErrorDiffusionDither<uint8_t, FloydSteinberg>>(data, cx, cy, alphaThreshold);
            break;
        case DitherAlgorithm::JarvisJudiceNinke:
            CutoutAlpha<ErrorDiffusionDither<uint8_t, JarvisJudiceNinke>>(data, cx, cy, alphaThreshold);
            break;
        case DitherAlgorithm::OrderedBayer:
            CutoutAlpha<OrderedDither<uint8_t>>(data, cx, cy, alphaThreshold);
            break;
        case DitherAlgorithm::None:
            CutoutAlpha<NoDither<uint8_t>>(data, cx, cy, alphaThreshold);
            break;
    }
}

std::vector<std::string> g_gdiplusStatuses =
{
    "Ok",
    "GenericError",
    "InvalidParameter",
    "OutOfMemory",
    "ObjectBusy",
    "InsufficientBuffer",
    "NotImplemented",
    "Win32Error",
    "WrongState",
    "Aborted",
    "FileNotFound",
    "ValueOverflow",
    "AccessDenied",
    "UnknownImageFormat",
    "FontFamilyNotFound",
    "FontStyleNotFound",
    "NotTrueTypeFont",
    "UnsupportedGdiplusVersion",
    "GdiplusNotInitialized",
    "PropertyNotFound",
    "PropertyNotSupported"
};


std::string GetGdiplusStatusString(Gdiplus::Status status)
{
    std::string error;
    size_t index = (size_t)status;
    if (index < g_gdiplusStatuses.size())
    {
        error = g_gdiplusStatuses[index];
    }
    return error;
}

// Creates 32bpp version of a cel, with alpha = 0 in transparent pixels.
HBITMAP Create32bbpBitmap(const Cel &cel, const RGBQUAD *palette, int paletteSize)
{
    return Create32bbpBitmap(&cel.Data[0], cel.GetStride(), cel.size.cx, cel.size.cy, cel.TransparentColor, palette, paletteSize);
}

HBITMAP Create32bbpBitmap(const uint8_t *pData, int cxStride, int cx, int cy, uint8_t transparentColor, const RGBQUAD *palette, int paletteSize)
{
    std::unique_ptr<RGBQUAD[]> pixels = std::make_unique<RGBQUAD[]>(cx * cy);
    RGBQUAD transparent = {};
    for (int y = 0; y < cy; y++)
    {
        int rowSource = cy - y - 1;
        for (int x = 0; x < cx; x++)
        {

            uint8_t value = pData[rowSource * cxStride + x];
            if (value == transparentColor)
            {
                pixels[x + y * cx] = transparent;
            }
            else
            {
                RGBQUAD color = palette[value % paletteSize];
                color.rgbReserved = 0xff; // alpha 255
                pixels[x + y * cx] = color;
            }
        }
    }

    return CreateBitmap(cx, cy, 1, 32, &pixels[0]);
}






//
// What follows is code that allows us to stash a resource in a windows .bmp file.
//

//
// data: data to be encoded
// pBits: where it is put
// pBitsEnd: check to avoid overflow -> points to just past the end of the buffer at pBits
//
BYTE *_EncodeByteInHighNibble(BYTE *pBits, BYTE data, const BYTE *pBitsEnd)
{
    BYTE *pBitsReturn = NULL;
    if (pBits < pBitsEnd)
    {
        ASSERT((*pBits & 0xf0) == 0);
        *pBits |= (data & 0xf0);

        pBits++;
        if (pBits < pBitsEnd)
        {
            ASSERT((*pBits & 0xf0) == 0);
            *pBits |= ((data & 0x0f) << 4);
            pBitsReturn = pBits + 1;
        }
    }
    return pBitsReturn;
}


struct BMP_ENCODEDRESOURCE_HEADER
{
    WORD wSize; // Excluding header...
    WORD wType; // 0x80 | type
};

//
// Encodes a resource in bitmap by using the unused high nibble in each byte.
//
bool EncodeResourceInBitmap(const ResourceBlob &blob, const BITMAPINFO &info, BYTE *pBits)
{
    const BYTE *pResourceByte = blob.GetData();
    DWORD cb = blob.GetLength();
    BMP_ENCODEDRESOURCE_HEADER header = { (WORD)cb, 0x80 | (int)blob.GetType() };
    cb += sizeof(header);

    DWORD dwSizeBmp = (info.bmiHeader.biBitCount / 8) * CX_ACTUAL(info.bmiHeader.biWidth) * info.bmiHeader.biHeight;
    if (dwSizeBmp / 2 >= cb)
    {
        BYTE *pBitmapByte = pBits;
        BYTE *pBitmapByteEnd = pBits + dwSizeBmp;

        // Yucky way to do header.
        BYTE *pHeader = (BYTE*)&header;
        pBitmapByte = _EncodeByteInHighNibble(pBitmapByte, (*pHeader), pBitmapByteEnd);
        pHeader++;
        pBitmapByte = _EncodeByteInHighNibble(pBitmapByte, (*pHeader), pBitmapByteEnd);
        pHeader++;
        pBitmapByte = _EncodeByteInHighNibble(pBitmapByte, (*pHeader), pBitmapByteEnd);
        pHeader++;
        pBitmapByte = _EncodeByteInHighNibble(pBitmapByte, (*pHeader), pBitmapByteEnd);

        cb = blob.GetLength();
        while (pBitmapByte && cb)
        {
            pBitmapByte = _EncodeByteInHighNibble(pBitmapByte, (*pResourceByte), pBitmapByteEnd);
            pResourceByte++;
            cb--;
        }
        return true;
    }
    else
    {
        // It won't fit!
        return false;
    }
}



const BYTE *_DecodeByteInHighNibble(const BYTE *pBits, BYTE *pByte, DWORD cbToRead, const BYTE *pBitsEnd)
{
    const BYTE *pBitsReturn = NULL;
    while (cbToRead)
    {
        if (pBits < pBitsEnd)
        {
            *pByte = *pBits & 0xf0;
            pBits++;
            if (pBits < pBitsEnd)
            {
                *pByte |= ((*pBits & 0xf0) >> 4);
                pBits++;
                pBitsReturn = pBits;
            }
        }
        pByte++;
        cbToRead--;
    }
    return pBitsReturn;
}

//
// Try to load a resource from a bitmap
// 
std::unique_ptr<ResourceBlob> Load8BitBmp(const GameFolderHelper &helper, SCIVersion version, const std::string &filename)
{
    std::unique_ptr<ResourceBlob> pBlob;
    std::ifstream bmpFile(filename.c_str(), std::ios::in | std::ios::binary);
    if (bmpFile.is_open())
    {
        BITMAPFILEHEADER bmfh;
        bmpFile.read((char *)&bmfh, sizeof(bmfh));

        DWORD dwId = bmfh.bfReserved1;
        dwId |= (bmfh.bfReserved2 << 16);

        if (dwId == SCIResourceBitmapMarker)
        {
            // It's one of our secret resources!
            bmpFile.seekg(bmfh.bfOffBits, std::ios_base::beg);

            DWORD cbBitmap = (bmfh.bfSize - (bmfh.bfOffBits - sizeof(bmfh)));

            if (cbBitmap < 0xffffff) // Check to make sure we don't allocate anything crazy
            {
                std::unique_ptr<BYTE[]> data = std::make_unique<BYTE[]>(cbBitmap);
                bmpFile.read((char*)data.get(), cbBitmap);

                BMP_ENCODEDRESOURCE_HEADER header;
                const BYTE *pData = data.get();
                const BYTE *pDataEnd = pData + cbBitmap;
                pData = _DecodeByteInHighNibble(pData, (BYTE*)&header, sizeof(header), pDataEnd);

                if (header.wSize <= (cbBitmap - sizeof(header))) // Sanity check
                {
                    ResourceType type = static_cast<ResourceType>(header.wType & ~0x0080);

                    // This is the buffer that contains our extracted data.
                    std::unique_ptr<BYTE[]> extractedData = std::make_unique<BYTE[]>(header.wSize);

                    // Read it.
                    pData = _DecodeByteInHighNibble(pData, extractedData.get(), header.wSize, pDataEnd);
                    if (pData)
                    {
                        sci::istream stream(extractedData.get(), header.wSize);
                        pBlob = std::make_unique<ResourceBlob>();
                        if (FAILED(pBlob->CreateFromBits(helper, nullptr, type, &stream, -1, -1, NoBase36, version, ResourceSourceFlags::ResourceMap)))
                        {
                            pBlob.reset(nullptr);
                        }
                    }
                }
            }
        }
    }
    return pBlob;
}