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
// ColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ColorButton.h"
#include "format.h"
#include "PicCommands.h"
#include <math.h>

CExtBitmap g_bitmapHighlight;
CExtBitmap g_bitmapHighlightX;

// CColorPickerButton

IMPLEMENT_DYNAMIC(CColorPickerButton, CButton)
CColorPickerButton::CColorPickerButton()
{
    _pFactory = nullptr;
    if (g_bitmapHighlight.IsEmpty())
    {
        g_bitmapHighlight.LoadBMP_Resource(MAKEINTRESOURCE(IDB_VPCHIGHLIGHT));
        g_bitmapHighlightX.LoadBMP_Resource(MAKEINTRESOURCE(IDB_VPCHIGHLIGHTX));
    }
}

CColorPickerButton::~CColorPickerButton()
{
}


BEGIN_MESSAGE_MAP(CColorPickerButton, CButton)
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CColorPickerButton message handlers


void CColorPickerButton::OnLButtonDown(UINT nFlags, CPoint point)
{
    __super::OnLButtonDown(nFlags, point);
    if (_pFactory)
    {
        RECT rc;
        GetWindowRect(&rc);
        _pFactory->InvokeDialog(GetDlgCtrlID(), &rc);
    }
    int x= 0;
}

void CColorPickerButton::SetColorAndState(RGBQUAD color, BOOL bOn)
{
    _color1 = color;
    _color2 = color;
    _bOn = bOn;
    Invalidate(FALSE);
}

void CColorPickerButton::SetColorAndState(EGACOLOR color, BOOL bOn)
{
    if ((_color.color1 != color.color1) || (_color.color2 != color.color2) || (_bOn != bOn))
    {
        _color = color; _bOn = bOn;
        _color1 = EGA_TO_RGBQUAD(color.color1);
        _color2 = EGA_TO_RGBQUAD(color.color2);
        Invalidate(FALSE);
    }
}

RGBQUAD _Desaturate(RGBQUAD color)
{
    RGBQUAD colorRet;
    colorRet.rgbBlue = (BYTE)((((WORD)color.rgbBlue) + 256) / 3);
    colorRet.rgbRed = (BYTE)((((WORD)color.rgbRed) + 256) / 3);
    colorRet.rgbGreen = (BYTE)((((WORD)color.rgbGreen) + 256) / 3);
    return colorRet;
}

RGBQUAD _Redden(RGBQUAD color)
{
    RGBQUAD colorRet;
    colorRet.rgbBlue = (BYTE)(((WORD)color.rgbBlue / 2));
    colorRet.rgbGreen = (BYTE)(((WORD)color.rgbGreen / 2));
    colorRet.rgbRed = (BYTE)(((WORD)color.rgbRed) * 3);
    if (colorRet.rgbRed < 200)
    {
        colorRet.rgbRed = 200;
    }
    return colorRet;
}

RGBQUAD _Combine(RGBQUAD color1, RGBQUAD color2)
{
    RGBQUAD colorRet;
    colorRet.rgbBlue = (BYTE)((((WORD)color1.rgbBlue) + ((WORD)color2.rgbBlue)) / 2);
    colorRet.rgbRed = (BYTE)((((WORD)color1.rgbRed) + ((WORD)color2.rgbRed)) / 2);
    colorRet.rgbGreen = (BYTE)((((WORD)color1.rgbGreen) + ((WORD)color2.rgbGreen)) / 2);
    return colorRet;
}

RGBQUAD _Darker(RGBQUAD color)
{
    RGBQUAD colorRet;
    colorRet.rgbBlue = (color.rgbBlue) / 2;
    colorRet.rgbRed = (color.rgbRed) / 2;
    colorRet.rgbGreen = (color.rgbGreen) / 2;
    return colorRet;
}

RGBQUAD _Lighter(RGBQUAD color)
{
    RGBQUAD colorRet;
    colorRet.rgbBlue = (((color.rgbBlue)) +  ((255 - color.rgbBlue) / 2));
    colorRet.rgbRed = (((color.rgbRed)) + ((255 - color.rgbRed) / 2));
    colorRet.rgbGreen = (((color.rgbGreen)) +  ((255 - color.rgbGreen) / 2));
    return colorRet;
}

COLORREF _GetTextColor(RGBQUAD color)
{
    DWORD dwAvg = ((DWORD)color.rgbBlue) + ((DWORD)color.rgbGreen) + ((DWORD)color.rgbRed);
    dwAvg /= 3;
    if (dwAvg < 200)
    {
        return RGB(220, 255, 220);
    }
    else
    {
        return RGB(110, 128, 110);
    }
}

BOOL CreateDCCompatiblePattern(RGBQUAD color1, RGBQUAD color2, int width, int height, CDC *pDC, CBitmap *pbm)
{
    std::unique_ptr<RGBQUAD[]> HatchBits = std::make_unique<RGBQUAD[]>(width * height);
    int halfW = width / 2;
    int halfH = height / 2;
    for (int y = 0; y < height; y++)
    {
        bool on = ((y / halfH) == 0);
        for (int x = 0; x < width; x++)
        {
            if (x == halfW)
            {
                on = !on;
            }
            // Without pulling this out into an "index" variable, the behavior is not correct in optimized builds.
            // MSVC bug?
            int index = y * width + x;
            HatchBits[index] = on ? color1 : color2;
        }
    }

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = height;
    bmi.bmiHeader.biPlanes = 1;

    CDC dcMem;
    BOOL bRet = dcMem.CreateCompatibleDC(pDC);
    if (bRet)
    {
        bRet = pbm->CreateCompatibleBitmap(pDC, width, height);
        if (bRet)
        {
            HGDIOBJ hObj = dcMem.SelectObject(pbm);
            bRet = StretchDIBits((HDC)dcMem, 0, 0, width, height, 0, 0, width, height, HatchBits.get(), &bmi, DIB_RGB_COLORS, SRCCOPY);
            dcMem.SelectObject(hObj);
        }        
    }
    return bRet;
}

void CColorPickerButton::_OnDraw(HDC hdc, RECT *prc)
{
    CDC *pDC = CDC::FromHandle(hdc);
    if (pDC)
    {
        RGBQUAD color1 = _color1;
        RGBQUAD color2 = _color2;
        if (!_bOn)
        {
            color1 = _Desaturate(color1);
            color2 = _Desaturate(color2);
        }

        CBitmap bm;
        if (CreateDCCompatiblePattern(color1, color2, 4, 4, pDC, &bm))
        {
            CBrush brushPat;
            if (brushPat.CreatePatternBrush(&bm))
            {
                CRect rcShrunk(prc->left + 1, prc->top + 1, prc->right - 1, prc->bottom - 1);
                pDC->FillRect(&rcShrunk, &brushPat);
            }
        }
       
        int iModePrev = pDC->SetBkMode(TRANSPARENT);
        RGBQUAD colorCombine = _Combine(_color1, _color2);
        RGBQUAD colorTL = _Lighter(colorCombine);
        RGBQUAD colorBR = _Darker(colorCombine);

        // Draw an x maybe
        if (!_bOn)
        {
            RGBQUAD colorRedder = _Redden(colorCombine);
            CPen penX(PS_SOLID, 1, RGB_TO_COLORREF(colorRedder));
            HGDIOBJ hgdiObjOld = pDC->SelectObject(&penX);
            pDC->MoveTo(prc->left, prc->bottom);
            pDC->LineTo(prc->right, prc->top);
            pDC->MoveTo(prc->left, prc->top);
            pDC->LineTo(prc->right, prc->bottom);
            pDC->SelectObject(hgdiObjOld);
        }

        // Draw some text.
        int iColorPrev = pDC->SetTextColor(_GetTextColor(colorCombine));
        CString text;
        GetWindowText(text);
        pDC->DrawText(text, prc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        pDC->SetTextColor(iColorPrev);


        pDC->SetBkMode(iModePrev);
    }
}


// CColorPickerButton message handlers

void CColorPickerButton::DrawItem(LPDRAWITEMSTRUCT pdis)
{
    _OnDraw(pdis->hDC, &pdis->rcItem);
}





//
// CBrushButton
//

IMPLEMENT_DYNAMIC(CBrushButton, CColorPickerButton)
CBrushButton::CBrushButton()
{
}

CBrushButton::~CBrushButton()
{
}

void CBrushButton::SetPenStyle(const PenStyle &penStyle)
{
    if (penStyle != _penStyle)
    {
        _penStyle = penStyle;
        Invalidate(FALSE);
    }
}

void CBrushButton::_OnDraw(HDC hdc, RECT *prc)
{
    // Fill with the background color.
    CDC *pDC = CDC::FromHandle(hdc);
    if (pDC)
    {
        // Ask the pattern to draw itself
        RECT rcLocal = *prc;

        bool enabled = !!IsWindowEnabled();

        // Black border.
        pDC->FillSolidRect(rcLocal.left, rcLocal.top, RECTWIDTH(rcLocal), RECTHEIGHT(rcLocal), enabled ? RGB(0, 0, 0) : RGB(196, 196, 196));

        if (enabled)
        {
            // Pen style is always black.
            EGACOLOR color;
            color.color1 = 0x0;
            color.color2 = 0x0;

            InflateRect(&rcLocal, -1, -1); // Shink for the borders.

            int cMin = 16;

            SCIBitmapInfo bmi(cMin, cMin);
            std::unique_ptr<BYTE[]> dataBrush = std::make_unique<BYTE[]>(cMin * cMin);
            // Fill with white.
            memset(dataBrush.get(), 0x0f, cMin * cMin);

            // Fake a PicData to draw into, with a visual brush.
            PicData data = { PicScreenFlags::Visual, dataBrush.get(), nullptr, nullptr, nullptr, false, false, size16(DEFAULT_PIC_WIDTH, DEFAULT_PIC_HEIGHT) };

            DrawPatternInRect(cMin, cMin, &data, 8, 8, color, 0, 0, PicScreenFlags::Visual, &_penStyle);

            int nOldMode = pDC->SetStretchBltMode(HALFTONE);
            StretchDIBits((HDC)*pDC, rcLocal.left, rcLocal.top, RECTWIDTH(rcLocal), RECTHEIGHT(rcLocal),
                0, 0, cMin, cMin, dataBrush.get(), &bmi, DIB_RGB_COLORS, SRCCOPY);
            pDC->SetStretchBltMode(nOldMode);
        }
    }
}
