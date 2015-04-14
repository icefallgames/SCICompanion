//
// Pic Command classes
//
#pragma once

struct PenStyle
{
    PenStyle() : bPatternSize(0), bPatternNR(0), fRandomNR(true), fRectangle(false), fPattern(false) {}
    bool fPattern;
    bool fRectangle;
    bool fRandomNR;
    uint8_t bPatternSize;
    uint8_t bPatternNR;
};

bool operator==(const PenStyle &one, const PenStyle &two);
bool operator!=(const PenStyle &one, const PenStyle &two);

void PatternInfoFromIndex(BYTE bIndex, PenStyle *pPenStyle);
uint8_t IndexFromPatternInfo(const PenStyle *pPenStyle);

