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
// ColoredToolTip.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ColoredToolTip.h"


// CColoredToolTip


IMPLEMENT_DYNAMIC(CColoredToolTip, CWnd)
CColoredToolTip::CColoredToolTip()
{
    SetColor(GetSysColor(COLOR_INFOBK));

    // Get a nice font (menu font) to use.
    NONCLIENTMETRICS metrics = { 0 };
    metrics.cbSize = sizeof(metrics);
    if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0))
    {
        m_font.CreateFontIndirect(&metrics.lfMenuFont);

        metrics.lfMenuFont.lfWeight = 700; // Bold
        m_fontBold.CreateFontIndirect(&metrics.lfMenuFont);
    }

    _nFormatFlags = 0;

    _hBitmapWeak = NULL;
}

CColoredToolTip::~CColoredToolTip()
{
}

void CColoredToolTip::SetColor(COLORREF cr)
{
    _cr = cr;
    _brush.DeleteObject();
    _brush.CreateSolidBrush(_cr);
}


#define TOOLTIP_MARGINS 4
#define TOOLTIP_OFFSET_Y 10 // should really be a drawtext
#define LEFT_MARGIN 2
#define TOPBOTTOM_MARGIN 1
#define Y_OFFSET 3


void CColoredToolTip::Show(CPoint ptScreen, std::string pszText)
{
    if (_strOne != pszText)
    {
        Invalidate(FALSE);  // make sure to repaint when text updates.
        _strOne = pszText;
    }
    _strTwo = TEXT("");
    _strThree = TEXT("");
    _nFormatFlags = 0;

    // Show the static!
    CDC *pDC = GetDC();
    if (pDC)
    {
        CRect rc(0, 0, 0, 0);
        HGDIOBJ hOld = pDC->SelectObject(&m_font);
        pDC->DrawText(_strOne.c_str(), &rc, DT_CALCRECT);
        pDC->SelectObject(hOld);
        rc.OffsetRect(ptScreen.x, ptScreen.y);
        SetWindowPos(NULL, rc.left, rc.top + TOOLTIP_OFFSET_Y, rc.Width() + TOOLTIP_MARGINS, rc.Height() + TOOLTIP_MARGINS, SWP_NOZORDER | SWP_NOACTIVATE);
        ShowWindow(SW_SHOWNOACTIVATE);
        ReleaseDC(pDC);
    }
}

void CColoredToolTip::Show(CPoint ptScreen, HBITMAP hBitmap, CSize size)
{
    _hBitmapWeak = hBitmap;
    _size = size;

    if (!IsWindowVisible())
    {
        CDC *pDC = GetDC();
        if (pDC)
        {
            // Add some margins
            SetWindowPos(NULL, ptScreen.x, ptScreen.y - Y_OFFSET, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE);
            ShowWindow(SW_SHOWNOACTIVATE);
            ReleaseDC(pDC);
        }
    }
    else
    {
        SetWindowPos(NULL, ptScreen.x, ptScreen.y - Y_OFFSET, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
        Invalidate();
    }
}


void CColoredToolTip::Show(CPoint pt, std::string strMethod, const std::vector<std::string> &strParams, size_t cParams)
{
    _strOne = strMethod + "(";
    _strTwo = "";
    _strThree = "";
    _nFormatFlags = DT_SINGLELINE;
    BOOL fBeyondLast = (cParams >= strParams.size());
    for (size_t i = 0; i < strParams.size(); i++)
    {
        BOOL fLast = (i == strParams.size()- 1);
        std::string *pStrAddTo;
        if (fLast && fBeyondLast)
        {
            pStrAddTo = &_strTwo;
        }
        else if (i < cParams)
        {
             pStrAddTo = &_strOne;
        }
        else if (i == cParams)
        {
            pStrAddTo = &_strTwo;
        }
        else
        {
            pStrAddTo = &_strThree;
        }
        *pStrAddTo += strParams[i];
        if (i < (strParams.size() - 1))
        {
            *pStrAddTo += " ";
        }
    }
    _strThree += ")";

    if (!IsWindowVisible())
    {
        CDC *pDC = GetDC();
        if (pDC)
        {
            // Calculate size.
            CSize size;
            CRect rc;
            rc.SetRectEmpty();
            pDC->DrawText(_strOne.c_str(), -1, &rc, DT_CALCRECT | _nFormatFlags);
            size.cy = rc.Height();
            size.cx = rc.Width();
            rc.SetRectEmpty();
            pDC->DrawText(_strTwo.c_str(), -1, &rc, DT_CALCRECT | _nFormatFlags);
            size.cx += rc.Width() * 5 / 4; // Because it's bold
            rc.SetRectEmpty();
            pDC->DrawText(_strThree.c_str(), -1, &rc, DT_CALCRECT | _nFormatFlags);
            size.cx += rc.Width();

            // Add some margins
            size.cy += TOPBOTTOM_MARGIN * 2;
            size.cx += LEFT_MARGIN * 2;
            SetWindowPos(NULL, pt.x, pt.y - TOPBOTTOM_MARGIN - Y_OFFSET, size.cx, size.cy, SWP_NOZORDER | SWP_NOACTIVATE);
            ShowWindow(SW_SHOWNOACTIVATE);
            ReleaseDC(pDC);
        }
    }
    else
    {
        Invalidate();
    }
}


void CColoredToolTip::Hide()
{
    ShowWindow(SW_HIDE);
}


BEGIN_MESSAGE_MAP(CColoredToolTip, CWnd)
    ON_WM_PAINT()
END_MESSAGE_MAP()


void CColoredToolTip::OnPaint()
{
	// standard paint routine
    RECT rcClient;
    GetClientRect(&rcClient);
	CPaintDC dc(this);

    if (_hBitmapWeak)
    {
        // We have a bitmap
        CDC dcMem;
        dcMem.CreateCompatibleDC(&dc);
        HGDIOBJ hOld = dcMem.SelectObject(_hBitmapWeak);
        dc.BitBlt(0, 0, _size.cx, _size.cy, &dcMem, 0, 0, SRCCOPY);
        dcMem.SelectObject(hOld);
    }
    else
    {
        // Just some text
        HGDIOBJ hOld = dc.SelectObject(&m_font);

        dc.FillSolidRect(&rcClient, _cr);

        rcClient.left += LEFT_MARGIN;
        rcClient.top += TOPBOTTOM_MARGIN;
        COLORREF crTextOld = dc.SetTextColor(GetSysColor(COLOR_INFOTEXT));
        CRect rcSegment;
        dc.DrawText(_strOne.c_str(), -1, &rcSegment, _nFormatFlags | DT_CALCRECT);

        dc.DrawText(_strOne.c_str(), -1, &rcClient, _nFormatFlags);
        dc.SetTextColor(crTextOld);

        rcClient.left += rcSegment.Width();
        dc.SelectObject(&m_fontBold);
        dc.DrawText(_strTwo.c_str(), -1, &rcSegment, _nFormatFlags | DT_CALCRECT);
        dc.DrawText(_strTwo.c_str(), -1, &rcClient, _nFormatFlags);
        dc.SelectObject(&m_font);

        rcClient.left += rcSegment.Width();
        dc.DrawText(_strThree.c_str(), -1, &rcClient, _nFormatFlags);

        dc.SelectObject(hOld);
    }
}

