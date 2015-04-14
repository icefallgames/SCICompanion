#include "stdafx.h"
#include "RasterOperations.h"
#include "Cursor.h"
#include "Components.h"

using namespace std;

// The EGA color we use for transparency (aqua)
const BYTE CursorTransparent = 0x03;
const int CursorDimensions = 16;

// Helpers for view resources
// We'll assume resource and package number are already taken care of, because they are common to all.
void CursorReadFromVersioned(ResourceEntity &resource, sci::istream &byteStream, bool isSCI0)
{
    // We only ever have one cel and one loop
    Cel &cel = resource.GetComponent<RasterComponent>().Loops[0].Cels[0];

    uint16_t wxHot;
    uint16_t wyHot;
    byteStream >> wxHot;
    byteStream >> wyHot;

    if (isSCI0)
    {
        // Only two choices of hot spot in SCI0, based on the upper byte of 'y'
        cel.placement.x = (0x00ff & wyHot) ? 8 : 0;
        cel.placement.y = cel.placement.x;
    }
    else
    {
        cel.placement.x = wxHot;
        cel.placement.y = wyHot;
    }

    // Now read the masks
    for (int iRow = 0; byteStream.good() && (iRow < 16); iRow++)
    {
        uint16_t w16bits;
        byteStream >> w16bits;
        for (int i = 0; i < CursorDimensions; i++)
        {
            int iIndex = CursorDimensions * (CursorDimensions - 1 - iRow) + i;
            // Just set as an indicator, 0x00 or 0xff
            cel.Data[iIndex] = (w16bits & (0x1 << (CursorDimensions - 1 - i))) ? 0xff : 0x00;
        }
    }

    // Now the white/black values
    for (int iRow = 0; byteStream.good() && (iRow < CursorDimensions); iRow++)
    {
        uint16_t w16bits;
        byteStream >> w16bits;
        for (int i = 0; i < CursorDimensions; i++)
        {
            int iIndex = CursorDimensions * (CursorDimensions - 1 - iRow) + i;
            bool bSet = (w16bits & (0x1 << (CursorDimensions - 1 - i))) != 0;
            if (cel.Data[iIndex] == 0x00)
            {
                cel.Data[iIndex] = bSet ? 0x0f : 0x00;
            }
            else
            {
                cel.Data[iIndex] = bSet ? (isSCI0 ? 0x0f : 0x07) : CursorTransparent;
            }
        }
    }
}

void CursorReadFromSCI0(ResourceEntity &resource, sci::istream &byteStream)
{
    CursorReadFromVersioned(resource, byteStream, true);
}

void CursorReadFromSCI1(ResourceEntity &resource, sci::istream &byteStream)
{
    CursorReadFromVersioned(resource, byteStream, false);
}

void CursorWriteTo(const ResourceEntity &resource, sci::ostream &byteStream)
{
    // We only ever have one cel and one loop
    Cel &cel = resource.GetComponent<RasterComponent>().Loops[0].Cels[0];

    byteStream.WriteWord(0);
    // The hot spot
    // REVIEW: some cursors have 480 and 320 in the first two spots.  Do we need to preserve that?
    byteStream.WriteWord(cel.placement.y == 8 ? 0x00ff : 0x0000);

    uint16_t rgwMask[16];
    ZeroMemory(rgwMask, sizeof(rgwMask));
    uint16_t rgwWhiteOrBlack[16];
    ZeroMemory(rgwWhiteOrBlack, sizeof(rgwWhiteOrBlack));
    for (int iRow = 0; iRow < CursorDimensions; iRow++)
    {
        for (int iCol = 0; iCol < CursorDimensions; iCol++)
        {
            int iIndex = CursorDimensions * (CursorDimensions - 1 - iRow) + iCol;
            switch (cel.Data[iIndex])
            {
            case 0x0f: // White
                rgwWhiteOrBlack[iRow] |= (1 << (CursorDimensions - 1 - iCol));
                break;

            case 0x00: // Black
                // Nothing to set, it's already 0
                break;

            default:   // Transparent
                assert(cel.Data[iIndex] == CursorTransparent);
                // Set the bit to indicate it's transparrent
                rgwMask[iRow] |= (1 << (CursorDimensions - 1 - iCol));
                break;
            }
        }
    }

    byteStream.WriteBytes((uint8_t*)rgwMask, sizeof(rgwMask));
    byteStream.WriteBytes((uint8_t*)rgwWhiteOrBlack, sizeof(rgwWhiteOrBlack));
}

uint8_t cursorPaletteMapping0[] ={ 0, 0xf, 0x3, 0x3 };

RasterTraits cursorRasterTraitsSCI0 =
{
    RasterCaps::Transparency | RasterCaps::SCI0CursorPlacement | RasterCaps::EightBitPlacement,
    OriginStyle::TopLeft,
    16,
    16,
    PaletteType::EGA_Four,
    cursorPaletteMapping0,
    g_egaColors,
    0,
    nullptr
};

uint8_t cursorPaletteMapping1[] = { 0, 0xf, 0x7, 0x3 }; // REVIEW: Not sure if this is right... 
// Because this is definitely wrong:
// http://sierrahelp.com/SCI/Documentation/SCISpecifications/13-CursorResource.html

RasterTraits cursorRasterTraitsSCI1 =
{
    RasterCaps::Transparency | RasterCaps::Placement | RasterCaps::EightBitPlacement,
    OriginStyle::TopLeft,
    16,
    16,
    PaletteType::EGA_Four,
    cursorPaletteMapping1,
    g_egaColors,
    0,
    nullptr
};

RasterSettings cursorRasterSettings =
{
    6   // Zoom 
};

ResourceTraits cursorTraitsEGA =
{
    ResourceType::Cursor,
    &CursorReadFromSCI0,
    &CursorWriteTo,
    &NoValidationFunc
};

ResourceTraits cursorTraitsVGA =
{
    ResourceType::Cursor,
    &CursorReadFromSCI1,
    nullptr,
    &NoValidationFunc
};

ResourceEntity *CreateCursorResource(SCIVersion version)
{
    std::unique_ptr<ResourceEntity> pResource = std::make_unique<ResourceEntity>(version.GrayScaleCursors ? cursorTraitsVGA : cursorTraitsEGA);
    pResource->AddComponent(move(make_unique<RasterComponent>((version.GrayScaleCursors) ? cursorRasterTraitsSCI1 : cursorRasterTraitsSCI0, cursorRasterSettings)));

    // Always just make a single 16x16 thing.
    RasterComponent &raster = pResource->GetComponent<RasterComponent>();
    Loop loop;
    Cel cel;
    cel.TransparentColor = CursorTransparent;
    loop.Cels.push_back(cel);
    raster.Loops.push_back(loop);
    ::FillEmpty(raster, CelIndex(0, 0), size16(16, 16));

    return pResource.release();
}

ResourceEntity *CreateDefaultCursorResource(SCIVersion version)
{
    return CreateCursorResource(version);
}