#pragma once

struct PaletteComponent;
struct Cel;

bool Save8BitBmpGdiP(const char *filename, const Cel &cel, const PaletteComponent &palette);
void FlipImageData(uint8_t *data, int cx, int cy, int stride);
int SquishPalette(uint8_t *data, size_t dataSize, const PaletteComponent &palette, RGBQUAD *results);
int CountActualUsedColors(const Cel &cel, bool *used);
int CountActualUsedColors(const std::vector<const Cel*> &cels, bool *used);
