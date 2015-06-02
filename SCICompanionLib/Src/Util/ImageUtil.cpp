#include "stdafx.h"
#include "ImageUtil.h"
#include "View.h"
#include "PaletteOperations.h"

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

// TODO: mark unused colors and squash the bits
bool Save8BitBmpGdiP(const char *filename, const Cel &cel, const PaletteComponent &palette)
{
    // We need to flip the data, so make a copy of it and do that.
    sci::array<uint8_t> buffer(cel.Data.size());
    buffer.assign(&cel.Data[0], &cel.Data[cel.Data.size()]);
    FlipImageData(&buffer[0], cel.size.cx, cel.size.cy, CX_ACTUAL(cel.size.cx));

    // Now "squish" the palette (and modify the data) so all the used colors are at the beginning.
    RGBQUAD newPaletteColors[256];
    int usedColors = SquishPalette(&buffer[0], buffer.size(), palette, newPaletteColors);

    Bitmap bitmap(cel.size.cx, cel.size.cy, CX_ACTUAL(cel.size.cx), PixelFormat8bppIndexed, &buffer[0]);
    ColorPalette *pcp = (ColorPalette*)malloc(sizeof(ColorPalette) + usedColors * 4);
    pcp->Count = usedColors;
    pcp->Flags = 0;
    for (int i = 0; i < usedColors; i++)
    {
        RGBQUAD rgb = newPaletteColors[i];
        ARGB argb = (rgb.rgbRed << RED_SHIFT) | (rgb.rgbGreen << GREEN_SHIFT) | (rgb.rgbBlue << BLUE_SHIFT);
        pcp->Entries[i] = argb;
    }
    bitmap.SetPalette(pcp);
    free(pcp);

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
    bitmap.Save(unicodestr, &encoderClsid, nullptr);
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
    uint8_t usedColors = 0;
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
