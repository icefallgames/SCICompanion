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
#include "FontOperations.h"
#include "RasterOperations.h"
#include "Components.h"

using namespace std;

bool IsLineEmpty(const BYTE *pBits, int cCount)
{
    while (cCount > 0)
    {
        if (*pBits != 0x0f) // White
        {
            return false;
        }
        cCount--;
        pBits++;
    }
    return true;
}

void RenderLetter(RasterComponent &raster, int nCel, CDC *pDC, int scale, char ch, bool fCalc, uint16_t *pwMaxCharHeight, int *piAbove, int *piBelow)
{
    if (fCalc)
    {
        *piAbove = 127; // We'll work down from here.
        *piBelow = 127;
    }
    
    bool writingFont = false;
    CRect rect(0, 0, 0, 0);
    if (pDC->DrawText(&ch, 1, &rect, DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE))
    {
        // Divide by scale, but round up.
        if (scale != 1)
        {
            rect.right = rect.right / scale + ((rect.right % scale) ? 1 : 0);
            rect.bottom = rect.bottom / scale + ((rect.bottom % scale) ? 1 : 0);
        }

        // REVIEW: this doesn't work so well for italic fonts.  DrawText "underreports" the width, and
        // so we don't give enough space to render the character.  However, if we add the extra space (say even
        // a pixel) the letters end up to far from each other.
        // We could offer a trimming function as some sort of solution.
        if ((rect.Width() <= 127) && (rect.Height() <= 127))
        {
            if (fCalc)
            {
                *pwMaxCharHeight = max(*pwMaxCharHeight, rect.Height());
            }

            BITMAPINFO bmi = {};
            BITMAPINFOHEADER &bmiHeader = bmi.bmiHeader;
            bmiHeader.biSize = sizeof(bmiHeader);
            bmiHeader.biWidth = rect.Width() * scale;
            bmiHeader.biHeight = rect.Height() * scale;
            bmiHeader.biPlanes = 1;
            bmiHeader.biBitCount = 32;
            bmiHeader.biCompression = BI_RGB;
            bmiHeader.biSizeImage = 0; // 0 is ok for BI_RGB
            bmiHeader.biXPelsPerMeter = 0; // ???
            bmiHeader.biYPelsPerMeter = 0; // ??? 

            // Make a bitmap.
            uint8_t *pBits;
            HBITMAP hDib = CreateDIBSection((HDC)*pDC, &bmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
            if (hDib)
            {
                // Set the whole bitmap to white.
                //memset(pBits, 0x0F, CX_ACTUAL(rect.Width()) * rect.Height());
                HGDIOBJ hOldBmp = pDC->SelectObject(hDib);

                CRect rectScaled(0, 0, bmiHeader.biWidth, bmiHeader.biHeight);
                pDC->FillRect(rectScaled, WHITE_BRUSH);

                // Black is our 'on'.
                COLORREF colorTextOld = pDC->SetTextColor(RGB(0, 0, 0));
                COLORREF colorOld = pDC->SetBkColor(RGB(255, 255, 255));
                //int iBkMode = pDC->SetBkMode(OPAQUE);
                int iBkMode = pDC->SetBkMode(TRANSPARENT);
                if (pDC->DrawText(&ch, 1, &rectScaled, DT_NOPREFIX | DT_SINGLELINE))
                {
                    // Now that we've drawn it, scale it down
                    if (scale != 1)
                    {
                        BITMAPINFO bmiScaled = bmi;
                        bmi.bmiHeader.biWidth = rect.Width();
                        bmi.bmiHeader.biHeight = rect.Height();
                        uint8_t *pBitsScaled;
                        HBITMAP hDibScaled = CreateDIBSection((HDC)*pDC, &bmi, DIB_RGB_COLORS, (void**)&pBitsScaled, NULL, 0);

                        CDC dcMem;
                        dcMem.CreateCompatibleDC(pDC);
                        HGDIOBJ hOldBmpScaled = dcMem.SelectObject(hDibScaled);
                        dcMem.SetStretchBltMode(HALFTONE);
                        dcMem.StretchBlt(0, 0, rect.Width(), rect.Height(), pDC, 0, 0, rect.Width() * scale, rect.Height() * scale, SRCCOPY);
                        dcMem.SelectObject(hOldBmpScaled);

                        // "replace" with the scaled down bitmap.
                        pDC->SelectObject(hOldBmp);
                        DeleteObject(hDib);
                        hDib = hDibScaled;
                        hOldBmp = pDC->SelectObject(hDib);
                        pBits = pBitsScaled;
                    }

                    // Now it's time to suck the pixels out.  The Font Resource format at
                    // http://freesci.linuxgames.com/scihtml/x2345.html is incorrect.
                    // Width comes before Height.
                    if (fCalc)
                    {
                        *piAbove = 127; // We'll work down from here.
                        *piBelow = 127;

                        // Count how many empty lines there are above and below.
                        int iAbove = 0;
                        for (int y = (rect.Height() - 1); y >= 0; y--)
                        {
                            uint8_t *pLine = ((uint8_t*)pBits) + y * CX_ACTUAL(rect.Width());
                            if (IsLineEmpty(pLine, rect.Width()))
                            {
                                iAbove++;
                            }
                            else
                            {
                                break;
                            }
                        }

                        int iBelow = 0;
                        for (int y = 0; y < rect.Height(); y++)
                        {
                            uint8_t *pLine = ((uint8_t*)pBits) + y * CX_ACTUAL(rect.Width());
                            if (IsLineEmpty(pLine, rect.Width()))
                            {
                                iBelow++;
                            }
                            else
                            {
                                break;
                            }
                        }
                        //assert(iAbove != 0); // REVIEW: was this important???
                        *piBelow = min(*piBelow, iBelow);
                        *piAbove = min(*piAbove, iAbove);
                    }
                    else
                    {
                        // Write the font.
                        // But first, adjust the rect for iAbove and iBelow.  Note, these are switched around, because
                        // the letter is printed upside down.
                        rect.top += *piBelow;
                        rect.bottom -= *piAbove;
                        // If it still has some height, write it (otherwise, we'll write an empty char later).
                        if (rect.Height() > 0)
                        {
                            writingFont = TRUE;
                            ::FillEmpty(raster, CelIndex(0, nCel), CSizeToSize(rect.Size()));
                            for (int y = (rect.bottom - 1); y >= rect.top; y--)
                            {
                                Cel &cel = raster.Loops[0].Cels[nCel];
                                for (int x = 0; x < CX_ACTUAL(cel.size.cx); x++)
                                {
                                    uint8_t *dest = &cel.Data[0] + (y - rect.top) * CX_ACTUAL(cel.size.cx) + x;
                                    uint8_t *srcP = pBits + (y * cel.size.cx + x) * 4;
                                    uint8_t src = *srcP;
                                    src = (uint8_t)((0.2125 * *(srcP + 2)) + (0.7152 * *(srcP + 1)) + (0.0722 * *(srcP + 0)));
                                    //RGBQUAD color = *(RGBQUAD*)src;

                                    // We support 16 levels - use top 4 bits
                                    *dest = (uint8_t)(src >> 4);
                                }
                            }
                        }
                    }
                }
                pDC->SetBkMode(iBkMode);
                pDC->SetBkColor(colorOld);
                pDC->SetTextColor(colorTextOld);
                pDC->SelectObject(hOldBmp);
                DeleteObject(hDib);
            }
        }
    }

    if (!writingFont && !fCalc)
    {
        // For some reason, this character could not be drawn (invisible, etc...).
        // In it's place, we'll have a 1x1 character.
        ::CreateDegenerate(raster.Loops[0].Cels[nCel], 0);
    }
}

void InitFontFromLOGFONT(RasterComponent &raster, FontComponent &fontComponent, const LOGFONT &logFont, bool scaleUp)
{
    uint16_t maxCharHeight = 0;

    LOGFONT logFontLocal = logFont;
    int scale = 1;
    if (scaleUp)
    {
        scale = 2; // For now... eventually we'll support more maybe?
        logFontLocal.lfHeight *= scale;
        logFontLocal.lfWidth *= scale;
    }

    CDC dc;
    if (dc.CreateCompatibleDC(NULL))
    {
        CFont font;
        if (font.CreateFontIndirect(&logFontLocal))
        {
            int iAboveMin = 127;
            int iBelowMin = 127;

            // "In addition, you must select a TrueType font into a screen DC prior to using it in a DIBSection, otherwise antialiasing does not occur."
            // -> still doesn't work. Only cleartype works.
            HGDIOBJ hScreenFontOld = SelectObject(GetDC(nullptr), font);


            HGDIOBJ hFontOld = dc.SelectObject(&font);

            // Figure out how much space above and below the letters we can remove.
            for (int i = 0; i < raster.CelCount(0); i++)
            {
                int iAbove, iBelow;
                RenderLetter(raster, i, &dc, scale, (char)i, true, &maxCharHeight, &iAbove, &iBelow);
                iAboveMin = min(iAboveMin, iAbove);
                iBelowMin = min(iBelowMin, iBelow);
            }
            maxCharHeight -= iAboveMin + iBelowMin;
            // TODO: work out a better calculation here:
            fontComponent.LineHeight = maxCharHeight;
            for (int i = 0; i < raster.CelCount(0); i++)
            {
                RenderLetter(raster, i, &dc, scale, (char)i, false, &maxCharHeight, &iAboveMin, &iBelowMin);
            }
            dc.SelectObject(hFontOld);
            SelectObject(GetDC(nullptr), hScreenFontOld);
        }
    }
}
