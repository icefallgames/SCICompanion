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

extern PaletteComponent g_egaDummyPalette;

void ReadPalette(PaletteComponent &palette, sci::istream &byteStream);
void WritePalette(sci::ostream &byteStream, const PaletteComponent &palette);
void WritePaletteShortForm(sci::ostream &byteStream, const PaletteComponent &palette);
void WritePaletteSCI2(sci::ostream &byteStream, const PaletteComponent &palette);

HBITMAP CreateBitmapFromPaletteResource(const ResourceEntity *prb, SCIBitmapInfo *pbmi, uint8_t **ppBitsDest, COLORREF *transparentColor = nullptr, const std::vector<const Cel*> *imageCels = nullptr);
HBITMAP CreateBitmapFromPaletteComponent(const PaletteComponent &palette, SCIBitmapInfo *pbmi, uint8_t **ppBitsDest, COLORREF *transparentColor = nullptr, const std::vector<const Cel*> *imageCels = nullptr);
ResourceEntity *CreatePaletteResource(SCIVersion version);

void LoadPALFile(const std::string &filename, PaletteComponent &palette, int startIndex);
void SavePALFile(const std::string &filename, PaletteComponent &palette, int startIndex, int count);