#include "stdafx.h"
#include "Font.h"
#include "RasterOperations.h"
#include "ResourceEntity.h"

using namespace std;

const uint16_t MaxLineHeight = 128;

Cel celTemplate(size16(0, 0), point16(0, 0), 0xf);

uint16_t FontTraits::ValidateCharCount(uint16_t charCount) const
{
    charCount = max(1, charCount);
    if (!SupportsExtendedCharSet)
    {
        charCount = SCI0LetterCount;
    }
    return charCount;
}

void ReadLetter(RasterComponent &raster, int nLetter, sci::istream &byteStream)
{
    uint8_t cx;
    byteStream >> cx;
    uint8_t cy;
    byteStream >> cy;
    ::InsertCel(raster, CelIndex(0, nLetter), celTemplate);
    Cel &cel = raster.Loops[0].Cels[nLetter];
    // Allocate enough room for the font character
    // We'll use one byte per pixel, like we do for views, even though the 
    // font is stored as a 1-bit-per-pixel resource.
    ::FillEmpty(raster, CelIndex(0, nLetter), size16(cx, cy));

    // Calcualte how many bytes to read from the resource, for each line:
    int cBytesToReadPerLine = (((int)cel.size.cx) + 7) / 8;

    // Now transfer from the resource into our datastructure.
    for (int y = 0; byteStream.good() && (y < (int)cel.size.cy); y++)
    {
        int cBitsRemainingThisLine = cx;

        // Start of the line:
        uint8_t *pLineDest = &cel.Data[0] + ((int)cel.size.cy - 1 - y) * CX_ACTUAL((int)cel.size.cx);
        // Figure out how many bytes to read.  A byte holds up to 8 pixel values.

        for (int iByte = 0; iByte < cBytesToReadPerLine; iByte++)
        {
            uint8_t bValue;
            byteStream >> bValue;
            // Read a byte.  Now convert it into up to
            for (int iBit = 7; (iBit >= 0) && cBitsRemainingThisLine; iBit--, cBitsRemainingThisLine--)
            {
                // Black (0x00) for on, and white (0x0f) for off
                *pLineDest = ((bValue >> iBit) & 1) ? 0x00 : 0x0f;
                pLineDest++;
            }
        }
    }
}

// Helpers for view resources
// We'll assume resource and package number are already taken care of, because they are common to all.
void FontReadFrom(ResourceEntity &resource, sci::istream &byteStream)
{
    RasterComponent &raster = resource.GetComponent<RasterComponent>();
    FontComponent &font = resource.GetComponent<FontComponent>();

    assert(raster.Loops.size() == 0);
    raster.Loops.push_back(Loop());

    uint16_t wDummy;
    uint16_t cChars;
    byteStream >> wDummy;           // The resource starts with a word (unknown use)
    byteStream >> cChars;           // Number of chars (128)
    byteStream >> font.LineHeight;

    // Some validation
    // TODO: report a status error 
    cChars = min(256, cChars);
    // 1 - 128 seems reasonable for line height
    font.LineHeight = max(1, font.LineHeight);
    font.LineHeight = min(MaxLineHeight, font.LineHeight);

    for (uint16_t i = 0; i < cChars; i++)
    {
        uint16_t wOffset;
        byteStream >> wOffset;
        sci::istream byteStreamLetter(byteStream);
        byteStreamLetter.seekg(wOffset);
        ReadLetter(raster, i, byteStreamLetter);
    }
}

#define ROUND_UP_BYTE(cx) (((cx) + 7) / 8)

void WriteLetter(const RasterComponent &raster, sci::ostream &byteStream, uint16_t iChar)
{
    const Cel &cel = raster.Loops[0].Cels[iChar];
    // The Font Resource format at
    // http://freesci.linuxgames.com/scihtml/x2345.html is incorrect.
    // Width comes before Height.
    byteStream.WriteByte((uint8_t)cel.size.cx);
    byteStream.WriteByte((uint8_t)cel.size.cy);
    // This is how many bytes we store for each line (one byte per pixel)
    int cBytes = CX_ACTUAL(cel.size.cx);
    int cx = (int)cel.size.cx;

    // This is how many bytes we'll write out (one bit per pixel)
    int cBytesToWrite = ROUND_UP_BYTE((uint8_t)cel.size.cx);

    // Now go through each line
    for (int y = 0; y < (int)cel.size.cy; y++)
    {
        // The resource is flipped vertically, so start reading from the bottom, while writing to the "top"
        const uint8_t *pBitsLine = &cel.Data[0] + CX_ACTUAL(cx) * ((int)cel.size.cy - y - 1);

        // Write a byte at a time.
        for (int i = 0; i < cBytesToWrite; i++)
        {
            uint8_t bOut = 0;
            int jEnd = 8 * (i + 1);
            int iShift = 7;
            for (int j = 8 * i; (j < jEnd) && (j < cx); j++, iShift--)
            {
                if (*(pBitsLine + j) == 0)
                {
                    // If it's 0 (black), then consider it on.
                    bOut |= (1 << iShift);
                }
            }
            byteStream.WriteByte(bOut);
        }
    }
}

void FontWriteTo(const ResourceEntity &resource, sci::ostream &byteStream)
{
    RasterComponent &raster = resource.GetComponent<RasterComponent>();
    FontComponent &font = resource.GetComponent<FontComponent>();

    // First prepare the header.
    byteStream.WriteWord(0);
    // The number of characters
    uint16_t cChars = (uint16_t)raster.Loops[0].Cels.size();
    assert((cChars == 128) || font.Traits.SupportsExtendedCharSet);  // This is all that is supported in SCI0
    byteStream.WriteWord(cChars);
    // The height of each line
    byteStream.WriteWord(font.LineHeight);

    std::unique_ptr<uint16_t[]> rgwOffsets(new uint16_t[cChars]);
    // Write some dumb data to take up space.
    byteStream.WriteBytes((uint8_t*)rgwOffsets.get(), cChars * sizeof(uint16_t));
    int iAbove = 127;
    int iBelow = 127;

    for (uint16_t i = 0; i < cChars; i++)
    {
        rgwOffsets[i] = (uint16_t)(byteStream.tellp());
        WriteLetter(raster, byteStream, i);
    }

    // Now copy the offset table directly into the sci::istream;
    memcpy(byteStream.GetInternalPointer() + CharTableOffset, rgwOffsets.get(), sizeof(uint16_t) * cChars);
}

EGACOLOR egaFontPalette[] =
{
    { 0, 0 },
    { 15, 15 },
};

void GetCharacterLabel(PTSTR  pszLabel, size_t cch, int nCel)
{
    if (nCel < 32)
    {
        StringCchPrintf(pszLabel, cch, TEXT("(%d)"), nCel);
    }
    else
    {
        StringCchPrintf(pszLabel, cch, TEXT("%c (%d)"), nCel, nCel);
    }
}

uint8_t fontPaletteMapping[] = { 0, 0xf };

RasterTraits fontRasterTraits =
{
    RasterCaps::Resize,
    OriginStyle::None,
    127,
    127,
    PaletteType::EGA_Two,
    fontPaletteMapping,
    g_egaColors,
    65,         // Preview cel
    &GetCharacterLabel,
    false
};

RasterSettings fontRasterSettings =
{
    6   // Zoom 
};

ResourceTraits fontResTraits =
{
    ResourceType::Font,
    &FontReadFrom,
    &FontWriteTo,
    &NoValidationFunc,
    nullptr
};

FontTraits fontTraitsSCI0 =
{
    false
};
FontTraits fontTraitsSCI1 =
{
    true
};

ResourceEntity *CreateFontResource(SCIVersion version)
{
    FontTraits *fontTraits = &fontTraitsSCI0;
    if (version.FontExtendedChars)
    {
        fontTraits = &fontTraitsSCI1;
    }
    std::unique_ptr<ResourceEntity> pResource = std::make_unique<ResourceEntity>(fontResTraits);
    pResource->AddComponent(move(make_unique<RasterComponent>(fontRasterTraits, fontRasterSettings)));
    pResource->AddComponent(move(make_unique<FontComponent>(*fontTraits)));
    return pResource.release();
}

ResourceEntity *CreateDefaultFontResource(SCIVersion version)
{
    std::unique_ptr<ResourceEntity> pResource(CreateFontResource(version));
    // Make a view with one loop that has one cel.
    RasterComponent &raster = pResource->GetComponent<RasterComponent>();
    Loop loop;
    Cel cel;
    cel.TransparentColor = 0xf;
    loop.Cels.push_back(cel);
    raster.Loops.push_back(loop);
    ::FillEmpty(raster, CelIndex(0, 0), size16(8, 8));
    for (int nCel = 1; nCel < SCI0LetterCount; nCel++)
    {
        ::InsertCel(raster, CelIndex(0, nCel - 1), false, true);
    }
    return pResource.release();
}