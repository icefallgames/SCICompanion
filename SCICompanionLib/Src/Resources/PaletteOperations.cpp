#include "stdafx.h"
#include "PaletteOperations.h"
#include "ResourceEntity.h"
#include "AppState.h"
#include "ImageUtil.h"

using namespace std;

void LoadPALFile(const std::string &filename, PaletteComponent &palette)
{
    ScopedFile file(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
    sci::streamOwner owner(file.hFile);
    sci::istream byteStream = owner.getReader();
    uint32_t riff, pal, dataSize, chunk, chunkSize;
    byteStream >> riff;
    byteStream >> dataSize;
    byteStream >> pal;
    byteStream >> chunk;
    byteStream >> chunkSize;
    uint16_t palVersion, palEntries;
    byteStream >> palVersion;
    byteStream >> palEntries;

    for (uint16_t i = 0; i < palEntries; i++)
    {
        byteStream >> palette.Colors[i].rgbRed;
        byteStream >> palette.Colors[i].rgbGreen;
        byteStream >> palette.Colors[i].rgbBlue;
        byteStream >> palette.Colors[i].rgbReserved;
        palette.Colors[i].rgbReserved = 0x1; // Actually, set this to used.
    }
    // Finish off with empties
    for (uint16_t i = palEntries; i < 256; i++)
    {
        palette.Colors[i].rgbRed = 0;
        palette.Colors[i].rgbGreen = 0;
        palette.Colors[i].rgbBlue = 0;
        palette.Colors[i].rgbReserved = 0;
    }
}

PaletteComponent::PaletteComponent() : Compression(PaletteCompression::Unknown), EntryType(PaletteEntryType::Unknown)
{
    // 1-to-1 mapping by default
    for (int i = 0; i < ARRAYSIZE(Mapping); i++)
    {
        Mapping[i] = (uint8_t)i;
    }
}

void PaletteComponent::MergeFromOther(const PaletteComponent *globalPalette)
{
    if (globalPalette)
    {
        for (int i = 0; i < ARRAYSIZE(Colors); i++)
        {
            // I thought it was a priority thing, but it looks like any rgbReserved that isn't
            // zero in the main palette will take precedence (e.g. KQ6 view 105)
            if (Colors[i].rgbReserved == 0)
            //if (globalPalette->Colors[i].rgbReserved > Colors[i].rgbReserved)
            {
                // Global has priority...
                Colors[i] = globalPalette->Colors[i];
            }
        }
    }
}

const int SquareSize = 12;

void DrawUnderline(uint8_t *bits, int cx, int cy, int x, int y, int border, uint8_t value)
{
    int rowSpan = cx;
    // Make a little square with the value from the palette
    int line = SquareSize - 1;
    int rowOffset = (y * SquareSize + line + border) * rowSpan;
    int columnOffset = x * SquareSize + border;
    memset(bits + rowOffset + columnOffset, value, SquareSize);
}

void DrawSquare(uint8_t *bits, int cx, int cy, int x, int y, int border, uint8_t value)
{
    int rowSpan = cx;
    // Make a little square with the value from the palette
    for (int line = 0; line < SquareSize; line++)
    {
        int rowOffset = (y * SquareSize + line + border) * rowSpan;
        int columnOffset = x * SquareSize + border;
        memset(bits + rowOffset + columnOffset, value, SquareSize);
    }
}

void DrawDot(uint8_t *bits, int cx, int cy, int x, int y, int border, uint8_t value)
{
    int rowSpan = cx;
    int line = -border + SquareSize / 2;
    int column = x * SquareSize + SquareSize / 2;
    int rowOffset = (y * SquareSize + line + border) * rowSpan;
    *(bits + rowOffset + column) = value;
}

// If imageCels are provided, we look into which palette entries are ACTUALLY used, instead of just going off the palette data.
HBITMAP CreateBitmapFromPaletteResource(const ResourceEntity *prb, SCIBitmapInfo *pbmi, uint8_t **ppBitsDest, COLORREF *transparentColor, const std::vector<const Cel*> *imageCels)
{
    HBITMAP hbmpRet = nullptr;
    const PaletteComponent &palette = prb->GetComponent<PaletteComponent>();

    const int border = 0;

    int cx = SquareSize * 16 + 2 * border;
    int cy = SquareSize * 16 + 2 * border;

    bool reallyUsed[256] = { 0 };
    int reallyUsedCount = 0;
    if (imageCels && !imageCels->empty())
    {
        reallyUsedCount = CountActualUsedColors(*imageCels, reallyUsed);
    }

    CDC dc;
    if (dc.CreateCompatibleDC(nullptr))
    {
        SCIBitmapInfo bmi(cx, cy, palette.Colors, ARRAYSIZE(palette.Colors));
        bmi.bmiHeader.biHeight = -bmi.bmiHeader.biHeight;
        uint8_t *pBitsDest;
        CBitmap bitmap;

        // Allow for callers to specify a "transparent" color to render unused palette entries.
        uint8_t transparentIndex = 0;
        RGBQUAD transparent = { GetBValue(*transparentColor), GetGValue(*transparentColor), GetRValue(*transparentColor), 0 };
        // Scan for the first unused entry and set its color to this
        for (int i = 0; i < 256; i++)
        {
            if (transparentColor)
            {
                if (bmi.bmiColors[i].rgbReserved == 0x0)
                {
                    // This entry is unused. Replace it with the transparent color and use this index to
                    // render unused entries.
                    bmi.bmiColors[i] = transparent;
                    transparentIndex = (uint8_t)i;
                }
            }
        }

        if (bitmap.Attach(CreateDIBSection((HDC)dc, &bmi, DIB_RGB_COLORS, (void**)&pBitsDest, nullptr, 0)))
        {
            for (int y = 0; y < 16; y++)
            {
                for (int x = 0; x < 16; x++)
                {
                    int paletteIndex = x + y * 16;
                    uint8_t value = palette.Mapping[paletteIndex];
                    // If this one isn't used, make it transparent.
                    if (transparentColor)
                    {
                        if (palette.Colors[paletteIndex].rgbReserved == 0x0)
                        {
                            value = transparentIndex;
                        }
                    }
                    DrawSquare(pBitsDest, cx, cy, x, y, border, value);
                    if (palette.Colors[paletteIndex].rgbReserved == 0x3)
                    {
                        DrawUnderline(pBitsDest, cx, cy, x, y, border, 0);
                    }
                }
            }

            if (imageCels)
            {
                // "highlight" which colors are actually used.
                for (int y = 0; y < 16; y++)
                {
                    for (int x = 0; x < 16; x++)
                    {
                        int paletteIndex = x + y * 16;
                        if (reallyUsed[paletteIndex])
                        {
                            DrawDot(pBitsDest, cx, cy, x, y, border, 255);
                        }
                    }
                }
            }

            hbmpRet = (HBITMAP)bitmap.Detach();
            *pbmi = bmi;
            *ppBitsDest = pBitsDest;
        }
    }

    return hbmpRet;
}

void PaletteWriteTo_SCI10(const ResourceEntity &resource, sci::ostream &byteStream)
{
    WritePalette(byteStream, resource.GetComponent<PaletteComponent>());
}

void PaletteWriteTo_SCI11(const ResourceEntity &resource, sci::ostream &byteStream)
{
    WritePaletteShortForm(byteStream, resource.GetComponent<PaletteComponent>());
}


#define SCI_PAL_FORMAT_VARIABLE 0
#define SCI_PAL_FORMAT_CONSTANT 1

RGBQUAD black = { 0, 0, 0, 0 };

void WritePalette(sci::ostream &byteStream, const PaletteComponent &palette)
{
    // We currently only support writing out the full palette.
    byteStream.WriteBytes(palette.Mapping, ARRAYSIZE(palette.Mapping) * sizeof(*palette.Mapping));
    // There seem to be 4 bytes of zeroes following that:
    byteStream.WriteWord(0);
    byteStream.WriteWord(0);
    // Unfortunately we need to swizzle RGBQUAD 
    for (int i = 0; i < ARRAYSIZE(palette.Colors); i++)
    {
        byteStream.WriteByte(palette.Colors[i].rgbReserved);
        byteStream.WriteByte(palette.Colors[i].rgbRed);
        byteStream.WriteByte(palette.Colors[i].rgbGreen);
        byteStream.WriteByte(palette.Colors[i].rgbBlue);
    }
}

#include <pshpack1.h>
struct PaletteHeader
{
    uint8_t marker;         // Or header? It's usually 0xE
    uint8_t garbage[9];     // Honestly looks like garbage, pieces of ascii text sometimes.
    uint16_t always1;
    uint8_t always0;
    uint16_t offsetToEndOfPaletteData; // post offset offset
    uint32_t globalPaletteMarker;   // ascii \0999
    uint32_t moreGarbage;   // Stuff in here, but I don't know what it is
    uint16_t always0_B;
    uint8_t palColorStart;
    uint8_t unknown3, unknown4, unknown5;
    uint16_t palColorCount;
    uint8_t unknown7;
    uint8_t palFormat;
    uint32_t unknown8;
};
#include <poppack.h>

void WritePaletteShortForm(sci::ostream &byteStream, const PaletteComponent &palette)
{
    // This is the one with the 37 byte header.
    assert(sizeof(PaletteHeader) == 37);

    PaletteHeader header = { 0 };
    header.marker = 0xe;
    header.palColorStart = 0;
    header.palColorCount = 256;
    header.palFormat = SCI_PAL_FORMAT_VARIABLE; // Make it easy on ourselves for now, so we don't need to interpret rgbReserved.
    header.always1 = 1;
    header.globalPaletteMarker = 0x00393939;    // This isn't always true, but...
    header.offsetToEndOfPaletteData = ((header.palFormat == SCI_PAL_FORMAT_VARIABLE) ? 4 : 3) * header.palColorCount + 22;

    header.unknown7 = 0x3; // Needed for SQ5 views to work????

    byteStream << header;

    for (uint16_t i = header.palColorStart; i < header.palColorCount; i++)
    {
        byteStream.WriteByte(palette.Colors[i].rgbReserved);
        byteStream.WriteByte(palette.Colors[i].rgbRed);
        byteStream.WriteByte(palette.Colors[i].rgbGreen);
        byteStream.WriteByte(palette.Colors[i].rgbBlue);
    }
}

void ReadPalette(PaletteComponent &palette, sci::istream &byteStream)
{
    uint32_t startPosition = byteStream.tellg();
    int end = 0;
    if (byteStream.getBytesRemaining() >= 37)   // This is a special case for pic 0 in SQ5.
    {
        PaletteHeader header;
        byteStream >> header;

        uint16_t palOffset;

        //if ((first == 0 && second == 1) || (first == 0 && second == 0 && palColorCount == 0))
        // [00],[01], the beginning of a palette color mapping table.
        if ((header.marker == 0x0 && header.garbage[0] == 0x1) || (header.marker == 0 && header.palColorCount == 0))
        {
            header.palFormat = SCI_PAL_FORMAT_VARIABLE;
            palOffset = 260;
            header.palColorStart = 0;
            header.palColorCount = 256;
            palette.Compression = PaletteCompression::None;
        }
        else
        {
            palette.Compression = PaletteCompression::Header;
            //assert(header.always1 == 1);
            //assert(header.always0 == 0);
            //assert(header.always0_B == 0);
            //assert(header.unknown8 == 0);
            palOffset = 37;
        }

        byteStream.seekg(startPosition + palOffset);

        end = (header.palColorStart + header.palColorCount);

        if (end <= ARRAYSIZE(palette.Colors))
        {
            if (header.palFormat == SCI_PAL_FORMAT_CONSTANT)
            {
                palette.EntryType = PaletteEntryType::ThreeByte;
                for (int entry = 0; entry < header.palColorStart; entry++)
                {
                    palette.Colors[entry] = black;
                }
                for (int entry = header.palColorStart; entry < end; entry++)
                {
                    palette.Colors[entry].rgbReserved = 1;
                    byteStream >> palette.Colors[entry].rgbRed;
                    byteStream >> palette.Colors[entry].rgbGreen;
                    byteStream >> palette.Colors[entry].rgbBlue;
                }
            }
            else if (header.palFormat == SCI_PAL_FORMAT_VARIABLE)
            {
                palette.EntryType = PaletteEntryType::FourByte;
                for (int entry = 0; entry < header.palColorStart; entry++)
                {
                    palette.Colors[entry] = black;
                }
                for (int entry = header.palColorStart; entry < end; entry++)
                {
                    byteStream >> palette.Colors[entry].rgbReserved;
                    byteStream >> palette.Colors[entry].rgbRed;
                    byteStream >> palette.Colors[entry].rgbGreen;
                    byteStream >> palette.Colors[entry].rgbBlue;
                }
            }
            else
            {
                throw std::exception("Invalid palette.");
            }
        }
        else
        {
            appState->LogInfo("Corrupt palette.");
            end = 0; // So we fill in with black below...
        }
    }

    for (int entry = end; entry < ARRAYSIZE(palette.Colors); entry++)
    {
        palette.Colors[entry] = black;
    }
}

void PaletteReadFrom(ResourceEntity &resource, sci::istream &byteStream)
{
    ReadPalette(resource.GetComponent<PaletteComponent>(), byteStream);
}

ResourceTraits paletteTraits_SCI10 =
{
    ResourceType::Palette,
    &PaletteReadFrom,
    &PaletteWriteTo_SCI10,
    &NoValidationFunc
};

ResourceTraits paletteTraits_SCI11 =
{
    ResourceType::Palette,
    &PaletteReadFrom,
    PaletteWriteTo_SCI11,
    &NoValidationFunc
};

ResourceEntity *CreatePaletteResource(SCIVersion version)
{
    std::unique_ptr<ResourceEntity> pResource = std::make_unique<ResourceEntity>(version.sci11Palettes ? paletteTraits_SCI11 : paletteTraits_SCI10);
    pResource->AddComponent(move(make_unique<PaletteComponent>()));
    return pResource.release();
}
