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

void RenderLetter(RasterComponent &raster, int nCel, CDC *pDC, char ch, bool fCalc, uint16_t *pwMaxCharHeight, int *piAbove, int *piBelow)
{
    bool writingFont = false;
    CRect rect(0, 0, 0, 0);
    if (pDC->DrawText(&ch, 1, &rect, DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE))
    {
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

            // Make a bitmap.
            SCIBitmapInfo bmi(rect.Width(), rect.Height());
            uint8_t *pBits;
            HBITMAP hDib = CreateDIBSection((HDC)*pDC, &bmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
            if (hDib)
            {
                // Set the whole bitmap to white.
                memset(pBits, 0x0F, CX_ACTUAL(rect.Width()) * rect.Height());
                HGDIOBJ hOldBmp = pDC->SelectObject(hDib);
                // Black is our 'on'.
                COLORREF colorTextOld = pDC->SetTextColor(RGB(0, 0, 0));
                COLORREF colorOld = pDC->SetBkColor(RGB(255, 255, 255));
                int iBkMode = pDC->SetBkMode(OPAQUE);
                if (pDC->DrawText(&ch, 1, &rect, DT_NOPREFIX | DT_SINGLELINE))
                {
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
                                CopyMemory(&cel.Data[0] + (y - rect.top) * CX_ACTUAL(cel.size.cx),
                                    pBits + y * CX_ACTUAL(cel.size.cx),
                                    CX_ACTUAL(cel.size.cx));
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

void InitFontFromLOGFONT(RasterComponent &raster, FontComponent &fontComponent, const LOGFONT *pLogFont)
{
    uint16_t maxCharHeight = 0;
    CDC dc;
    if (dc.CreateCompatibleDC(NULL))
    {
        CFont font;
        if (font.CreateFontIndirect(pLogFont))
        {
            int iAboveMin = 127;
            int iBelowMin = 127;
            HGDIOBJ hFontOld = dc.SelectObject(&font);

            // Figure out how much space above and below the letters we can remove.
            for (int i = 0; i < LetterCount; i++)
            {
                int iAbove, iBelow;
                RenderLetter(raster, i, &dc, (char)i, true, &maxCharHeight, &iAbove, &iBelow);
                iAboveMin = min(iAboveMin, iAbove);
                iBelowMin = min(iBelowMin, iBelow);
            }
            maxCharHeight -= iAboveMin + iBelowMin;
            // TODO: work out a better calculation here:
            fontComponent.LineHeight = maxCharHeight;
            for (int i = 0; i < LetterCount; i++)
            {
                RenderLetter(raster, i, &dc, (char)i, false, &maxCharHeight, &iAboveMin, &iBelowMin);
            }
            dc.SelectObject(hFontOld);
        }
    }
}
