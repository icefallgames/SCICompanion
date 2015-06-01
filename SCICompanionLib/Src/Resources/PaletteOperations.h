#pragma once

#include "Components.h"

class GameFolderHelper;

enum class PaletteChangeHint
{
    None = 0,
    Changed = 0x00000100,
};
DEFINE_ENUM_FLAGS(PaletteChangeHint, uint32_t)

enum class PaletteCompression
{
    Unknown,
    None,
    Header,
};

enum class PaletteEntryType
{
    Unknown,
    ThreeByte,
    FourByte,
};


struct PaletteComponent : public ResourceComponent
{
    PaletteComponent();
    PaletteComponent(const PaletteComponent &src) = default;
    PaletteComponent& operator=(const PaletteComponent &src) = default;
    ResourceComponent *Clone() const override
    {
        return new PaletteComponent(*this);
    }

    bool operator==(const PaletteComponent &src);
    bool operator!=(const PaletteComponent &src);

    ~PaletteComponent()
    {
    }

    uint8_t Mapping[256];
    // The rgbReserved component of RGBQUAD appears to represent
    // "priority" for palette colors (for when palettes are combined).
    // The global palette typically has 0x1 set for its values.
    // Individual palettes for pics/views tend to have 0x0 set for values they
    // don't use (to be overriden by the global palette), or 0x3 for values
    // they do use.
    RGBQUAD Colors[256];

    PaletteCompression Compression;
    PaletteEntryType EntryType;
    void MergeFromOther(const PaletteComponent *globalPalette);
};

void ReadPalette(PaletteComponent &palette, sci::istream &byteStream);
void WritePalette(sci::ostream &byteStream, const PaletteComponent &palette);
void WritePaletteShortForm(sci::ostream &byteStream, const PaletteComponent &palette);

HBITMAP CreateBitmapFromPaletteResource(const ResourceEntity *prb, SCIBitmapInfo *pbmi, uint8_t **ppBitsDest, COLORREF *transparentColor = nullptr, const std::vector<const Cel*> *imageCels = nullptr);
ResourceEntity *CreatePaletteResource(SCIVersion version);

void LoadPALFile(const std::string &filename, PaletteComponent &palette);