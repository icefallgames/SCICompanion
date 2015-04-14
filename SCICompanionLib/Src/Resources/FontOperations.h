#pragma once
#include "Font.h"

struct RasterComponent;
struct FontComponent;
void InitFontFromLOGFONT(RasterComponent &raster, FontComponent &font, const LOGFONT *pLogFont);
