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
// NoFlickerStatic.cpp : implementation file
//

#include "stdafx.h"
#include "NoFlickerStatic.h"
#include "AppState.h"

// CExtNoFlickerStatic, for images.

BEGIN_MESSAGE_MAP(CExtNoFlickerStatic, CStatic)
    ON_WM_DRAWITEM_REFLECT()
    ON_WM_DRAWITEM()
END_MESSAGE_MAP()

// CNoFlickerStatic message handlers

bool CExtNoFlickerStatic::FromBitmap(HBITMAP hbmp, int cx, int cy, bool correctForAspectRatio)
{
    _cx = cx;
    _cy = cy;
    _correctForAspectRatio = correctForAspectRatio;
    Invalidate(); // New bitmap
    return _bitmap.FromBitmap(hbmp);
}

void CExtNoFlickerStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CRect rcClient;
    GetClientRect(&rcClient);
    CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    if (pDC)
    {
        // background
        int saveHandle = pDC->SaveDC();
        if (!_bitmap.IsEmpty())
        {
            // keep aspect ratio.
            int iMaxZoom = rcClient.Width() / _cx;
            iMaxZoom = min(iMaxZoom, rcClient.Height() / _cy);
            iMaxZoom = max(1, iMaxZoom); // But at least one.
            iMaxZoom = min(5, iMaxZoom); // But no more than 5, say
            int cxDest = iMaxZoom * _cx;
            int cyDest = iMaxZoom * _cy;
            // If it didn't fit, then scale down...
            bool fNeedSmoothing = false;
            if (cxDest > rcClient.Width())
            {
                cyDest = cyDest * rcClient.Width() / cxDest;
                cxDest = rcClient.Width();
                fNeedSmoothing = true;
            }
            if (cyDest > rcClient.Height())
            {
                cxDest = cxDest * rcClient.Height() / cyDest;
                cyDest = rcClient.Height();
                ASSERT(cxDest <= rcClient.Width());
                fNeedSmoothing = true;
            }

            if (_correctForAspectRatio)
            {
                int cxDestNew = appState->InverseAspectRatioY(cxDest);
                if (cxDestNew != cxDest)
                {
                    fNeedSmoothing = true;
                    cxDest = cxDestNew;
                }
            }

            int x = (rcClient.Width() - cxDest) / 2;
            int y = (rcClient.Height() - cyDest) / 2;
            CRect rcDest(x, y, x + cxDest, y + cyDest);
            int nOldMode;
            if (fNeedSmoothing)
            {
                nOldMode = pDC->SetStretchBltMode(HALFTONE);
            }
            _bitmap.Draw(lpDrawItemStruct->hDC, rcDest);
            if (fNeedSmoothing)
            {
                pDC->SetStretchBltMode(nOldMode);
            }
            pDC->ExcludeClipRect(&rcDest);
        }
        // Call CExtLabel's background, which will paint the gradient (or whatever) background
        OnEraseBackground(*pDC, rcClient);
        pDC->RestoreDC(saveHandle);
    }
}
