// ChooseColorStatic.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ChooseColorStatic.h"
#include "ChooseColorDialog.h"

// CChooseColorStatic

IMPLEMENT_DYNAMIC(CChooseColorStatic, CStatic)
CChooseColorStatic::CChooseColorStatic()
{
    _bHoverIndex = INVALID_COLORINDEX;
    _cRows = 0;
    _cColumns = 0;
    _fPrintIndex = FALSE;
    _fShowSelection = FALSE;
    _bSelectedColorIndex = 0;
    _bAuxSelectedColorIndex = 0;
    _fShowAuxSel = FALSE;
    memset(_pPalette, 0, sizeof(_pPalette));
    _fShowIndices = FALSE;
    _fSelectionNumbers = FALSE;
    _autoHandleSelection = false;
    _allowMultipleSelection = false;
    _focusIndex = INVALID_COLORINDEX;
    _showUnused = false;
    _showHover = true;
    _showActualUsedColors = false;
    _showSelectionBoxes = false;
}

CChooseColorStatic::~CChooseColorStatic()
{
}


BEGIN_MESSAGE_MAP(CChooseColorStatic, CStatic)
    ON_WM_DRAWITEM_REFLECT()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CChooseColorStatic::SetSelection(BYTE bIndex)
{
    _bSelectedColorIndex = bIndex;
    if (m_hWnd)
    {
        Invalidate(FALSE);
    }
}

void CChooseColorStatic::SetAuxSelection(BYTE bIndex)
{
    _bAuxSelectedColorIndex = bIndex;
    _fShowAuxSel = TRUE;
    if (m_hWnd)
    {
        Invalidate(FALSE);
    }
}

void CChooseColorStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    RECT rcClient;
    GetClientRect(&rcClient);
    CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    if (pDC)
    {

        CDC dcMem;
        if (dcMem.CreateCompatibleDC(pDC))
        {
            CBitmap bitmap;
            bitmap.CreateCompatibleBitmap(pDC, RECTWIDTH(rcClient), RECTHEIGHT(rcClient));
            if (dcMem.SelectObject(&bitmap))
            {
                // Now hand this memory DC off to the real drawing function.
                _DrawItem(&dcMem, RECTWIDTH(rcClient), RECTHEIGHT(rcClient));

                // Now blt back
                // Now we want to copy it back to the real dc.
                pDC->BitBlt(0, 0, RECTWIDTH(rcClient), RECTHEIGHT(rcClient), &dcMem, 0, 0, SRCCOPY);
            }
        }
    }
}

void CChooseColorStatic::SetActualUsedColors(bool *actualUsedColors)
{
    _showActualUsedColors = true;
    memcpy(_actualUsedColors, actualUsedColors, sizeof(bool) * (_cRows * _cColumns));
    Invalidate(FALSE);
}

void CChooseColorStatic::SetPalette(int cRows, int cColumns, const EGACOLOR *pPalette, int paletteColorCount, const RGBQUAD *pPaletteColors, bool dithered)
{
    _cRows = cRows; _cColumns = cColumns;
    _paletteSquareInset = (max(_cRows, _cColumns) <= 4) ? 2 : 0;

    if (pPalette != nullptr)
    {
        // Sometimes clients dont' need this (e.g. pen style)
        int count = _cRows * _cColumns;
        // Make a copy of the palette, since we cache this and it could go away.
        memcpy(_pPalette, pPalette, sizeof(*pPalette) * count);
        memcpy(_pPaletteColors, pPaletteColors, sizeof(*pPaletteColors) * paletteColorCount);
    }
    _dithered = dithered;
}

// CChooseColorStatic message handlers

void CChooseColorStatic::_DrawItem(CDC *pDC, int cx, int cy)
{
    // Call CExtLabel's background, which will paint the gradient (or whatever) background
    //OnEraseBackground(*pDC, CRect(0, 0, cx, cy));
    pDC->FillSolidRect(0, 0, cx, cy, g_PaintManager->GetColor(COLOR_BTNFACE));

    if (_pPalette)
    {
        _DrawSelection(pDC);

        // Divide things up into _cRows rows and _cColumms columns.
        int iPaletteIndex = 0;

        for (iPaletteIndex = 0; iPaletteIndex < (_cRows * _cColumns); iPaletteIndex++)
        {
            assert(iPaletteIndex < 256); // Since we cast to a BYTE
            // We use an int for this loop, because there could be up to 256 colors in this static.

            RECT rectSquare;
            _MapIndexToRect((BYTE)iPaletteIndex, &rectSquare);

            // Is this the current selected one?  Highlight it.

            // Figure out the right color - do color 1 for now.
            RGBQUAD color1;
            RGBQUAD color2;
            if (_dithered)
            {
                EGACOLOR egaColor = _pPalette[iPaletteIndex];
                color1 = EGA_TO_RGBQUAD(egaColor.color1);
                color2 = EGA_TO_RGBQUAD(egaColor.color2);
            }
            else
            {
                uint8_t colorIndex = _pPalette[iPaletteIndex].ToByte();
                color1 = _pPaletteColors[colorIndex];
                color2 = _pPaletteColors[colorIndex];
            }

            CBitmap bm;
            if (CreateDCCompatiblePattern(color1, color2, 4, 4,  pDC, &bm))
            {
                CBrush brushPat;
                if (brushPat.CreatePatternBrush(&bm))
                {
                    pDC->FillRect(&rectSquare, &brushPat);
                }
            }

            _DrawIndex(pDC, (BYTE)iPaletteIndex);

            if (_fSelectionNumbers && (!_fShowAuxSel || (_bSelectedColorIndex != _bAuxSelectedColorIndex)))
            {
                // Only show selection numbers when we're not showing auxiliary selection, or when we
                // are, and the two selections are different.
                _DrawTextAtIndex(pDC, _bSelectedColorIndex, TEXT("A"));
                if (_fShowAuxSel)
                {
                    _DrawTextAtIndex(pDC, _bAuxSelectedColorIndex, TEXT("B"));
                }
            }
        }

        _DrawUnused(pDC);

        _DrawActualUsedColors(pDC);

        _Draw0x3Colors(pDC);

        _DrawMultiSelection(pDC);

        if (_showSelectionBoxes)
        {
            RECT rectHover;
            _MapIndexToRect((uint8_t)_bSelectedColorIndex, &rectHover);
            // Draw a box
            pDC->DrawFocusRect(&rectHover);
            // make it a little bigger.
            InflateRect(&rectHover, 1, 1);
            pDC->DrawFocusRect(&rectHover);
            if (_fShowAuxSel)
            {
                // Draw a thinner one for the aux selection.
                _MapIndexToRect((uint8_t)_bAuxSelectedColorIndex, &rectHover);
                pDC->DrawFocusRect(&rectHover);
            }
        }

        _DrawHover(pDC);

    }
}

void CChooseColorStatic::_DrawHover(CDC *pDC)
{
    // Draw the hovered box.
    if (_showHover && (_bHoverIndex != (uint16_t)INVALID_COLORINDEX))
    {
        RECT rectHover;
        _MapIndexToRect((uint8_t)_bHoverIndex, &rectHover);
        // Draw a box
        pDC->DrawFocusRect(&rectHover);
        // make it a little bigger.
        InflateRect(&rectHover, 1, 1);
        pDC->DrawFocusRect(&rectHover);
    }
}

void CChooseColorStatic::_DrawSelection(CDC *pDC)
{
    if (_fShowSelection)
    {
        for (int i = 0; i < (_fShowAuxSel ? 2 : 1); i++)
        {
            BYTE bSelectedIndex = (i == 0) ? _bSelectedColorIndex : _bAuxSelectedColorIndex;
            RECT rectSquare;
            _MapIndexToRect(bSelectedIndex, &rectSquare);

            RECT rectHighlight = rectSquare;
            InflateRect(&rectHighlight, 2, 2);
            CBrush brushSolid;
            brushSolid.CreateSolidBrush(RGB(0, 0, 0));
            pDC->FillRect(&rectHighlight, &brushSolid);

            InflateRect(&rectHighlight, -1, -1);

            CBrush brushSolidWhite;
            brushSolidWhite.CreateSolidBrush(RGB(255, 255, 255));
            pDC->FillRect(&rectHighlight, &brushSolidWhite);
        }
    }
}

void CChooseColorStatic::_DrawUnused(CDC *pDC)
{
    if (_showUnused)
    {
        std::vector<CPoint> points;
        std::vector<DWORD> pointCounts;
        points.reserve(200);
        pointCounts.reserve(100);

        for (int y = 0; y < _cRows; y++)
        {
            for (int x = 0; x < _cColumns; x++)
            {
                int index = x + (y * _cColumns);
                if (_pPaletteColors[index].rgbReserved == 0x0)
                {
                    CRect rect;
                    _MapIndexToRect((uint8_t)index, &rect);
                    InflateRect(&rect, -2, -2);
                    points.emplace_back(rect.left, rect.top);
                    points.emplace_back(rect.right, rect.bottom);
                    points.emplace_back(rect.right - 1, rect.top);
                    points.emplace_back(rect.left - 1, rect.bottom);
                    pointCounts.push_back(2);
                    pointCounts.push_back(2);
                }
            }
        }

        if (!points.empty())
        {
            CPen pen(PS_SOLID, 1, RGB(16, 16, 16));
            HGDIOBJ hOld = pDC->SelectObject(pen);
            pDC->PolyPolyline(&points[0], &pointCounts[0], (int)pointCounts.size());
            pDC->SelectObject(hOld);
        }
    }
}

// Draw an underline under these colors
void CChooseColorStatic::_Draw0x3Colors(CDC *pDC)
{
    std::vector<CPoint> points;
    std::vector<DWORD> pointCounts;
    points.reserve(200);
    pointCounts.reserve(100);
    for (int y = 0; y < _cRows; y++)
    {
        for (int x = 0; x < _cColumns; x++)
        {
            int index = x + (y * _cColumns);
            if (_pPaletteColors[index].rgbReserved == 0x3)
            {
                CRect rect;
                _MapIndexToRect((uint8_t)index, &rect);
                points.emplace_back(rect.left, rect.bottom - 1);
                points.emplace_back(rect.right,rect.bottom - 1);
                pointCounts.push_back(2);
            }
        }
    }

    if (!points.empty())
    {
        CPen pen(PS_SOLID, 1, RGB(0, 0, 64));
        HGDIOBJ hOld = pDC->SelectObject(pen);
        pDC->PolyPolyline(&points[0], &pointCounts[0], (int)pointCounts.size());
        pDC->SelectObject(hOld);
    }
}

void CChooseColorStatic::_DrawActualUsedColors(CDC *pDC)
{
    if (_showActualUsedColors)
    {
        std::vector<CPoint> points;
        std::vector<DWORD> pointCounts;
        points.reserve(200);
        pointCounts.reserve(100);
        for (int y = 0; y < _cRows; y++)
        {
            for (int x = 0; x < _cColumns; x++)
            {
                int index = x + (y * _cColumns);
                if (_actualUsedColors[index])
                {
                    CRect rect;
                    _MapIndexToRect((uint8_t)index, &rect);
                    CPoint center = rect.CenterPoint();
                    points.emplace_back(center.x, center.y);
                    points.emplace_back(center.x + 1, center.y);
                    pointCounts.push_back(2);
                }
            }
        }

        if (!points.empty())
        {
            CPen pen(PS_SOLID, 1, RGB(240, 240, 240));
            HGDIOBJ hOld = pDC->SelectObject(pen);
            pDC->PolyPolyline(&points[0], &pointCounts[0], (int)pointCounts.size());
            pDC->SelectObject(hOld);
        }
    }
}

void CChooseColorStatic::_DrawMultiSelection(CDC *pDC)
{
    if (_fShowSelection && _allowMultipleSelection)
    {
        std::vector<CPoint> outerPoints;
        std::vector<CPoint> innerPoints;
        std::vector<DWORD> pointCounts;
        outerPoints.reserve(200);
        innerPoints.reserve(200);
        pointCounts.reserve(100);
        for (int y = 0; y < _cRows; y++)
        {
            for (int x = 0; x < _cColumns; x++)
            {
                bool aboveSelected = (y > 0) ? (_multipleSelection[x + (y - 1) * _cColumns]) : false;
                bool belowSelected = (y < (_cColumns - 1)) ? (_multipleSelection[x + (y + 1) * _cColumns]) : false;
                bool leftSelected = (x > 0) ? (_multipleSelection[x - 1 + y * _cColumns]) : false;
                bool rightSelected = (x < (_cRows - 1)) ? (_multipleSelection[x + 1 + y * _cColumns]) : false;
                int index = x + (y * _cColumns);
                if (_multipleSelection[index])
                {
                    CRect rect;
                    _MapIndexToRect((uint8_t)index, &rect);
                    InflateRect(&rect, 1, 1);
                    if (!aboveSelected)
                    {
                        outerPoints.emplace_back(rect.left, rect.top);
                        outerPoints.emplace_back(rect.right, rect.top);
                        innerPoints.emplace_back(rect.left + 1, rect.top + 1);
                        innerPoints.emplace_back(rect.right - 1, rect.top + 1);
                        pointCounts.push_back(2);
                    }
                    if (!belowSelected)
                    {
                        outerPoints.emplace_back(rect.left, rect.bottom - 1);
                        outerPoints.emplace_back(rect.right, rect.bottom - 1);
                        innerPoints.emplace_back(rect.left + 1, rect.bottom - 2);
                        innerPoints.emplace_back(rect.right - 1, rect.bottom - 2);
                        pointCounts.push_back(2);
                    }
                    if (!leftSelected)
                    {
                        outerPoints.emplace_back(rect.left, rect.top);
                        outerPoints.emplace_back(rect.left, rect.bottom);
                        innerPoints.emplace_back(rect.left + 1, rect.top + 1);
                        innerPoints.emplace_back(rect.left + 1, rect.bottom - 1);
                        pointCounts.push_back(2);
                    }
                    if (!rightSelected)
                    {
                        outerPoints.emplace_back(rect.right - 1, rect.top);
                        outerPoints.emplace_back(rect.right - 1, rect.bottom);
                        innerPoints.emplace_back(rect.right - 2, rect.top + 1);
                        innerPoints.emplace_back(rect.right - 2, rect.bottom - 1);
                        pointCounts.push_back(2);
                    }
                }
            }
        }

        if (!outerPoints.empty())
        {
            CPen penBlack(PS_SOLID, 1, RGB(0, 0, 0));
            CPen penWhite(PS_SOLID, 1, RGB(255, 255, 255));
            HGDIOBJ hOld = pDC->SelectObject(penBlack);
            pDC->PolyPolyline(&outerPoints[0], &pointCounts[0], (int)pointCounts.size());
            pDC->SelectObject(penWhite);
            pDC->PolyPolyline(&innerPoints[0], &pointCounts[0], (int)pointCounts.size());
            pDC->SelectObject(hOld);
        }
    }
}

COLORREF _GetGoodTextColor(RGBQUAD color)
{
    DWORD dwAvg = ((DWORD)color.rgbBlue) + ((DWORD)color.rgbGreen) + ((DWORD)color.rgbRed);
    dwAvg /= 3;
    if (dwAvg < 170)
    {
        return RGB(220, 255, 220);
    }
    else
    {
        return RGB(110, 128, 110);
    }
}

//
// Draw a number in this square
//
void CChooseColorStatic::_DrawIndex(CDC *pDC, BYTE bIndex)
{
    if (_fShowIndices)
    {
        TCHAR szBuf[3];
        StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%02d"), bIndex);
        _DrawTextAtIndex(pDC, bIndex, szBuf);
    }
}

void CChooseColorStatic::_DrawTextAtIndex(CDC *pDC, BYTE bIndex, PCTSTR psz)
{
    RECT rectSquare;
    _MapIndexToRect(bIndex, &rectSquare);

    int iBkMode = pDC->SetBkMode(TRANSPARENT);

    // Note: we're just using color1 for sampling here.  Currently the "drawindex"
    // feature is only used for non-dithered colours (e.g. priority and control)
    COLORREF colorOld = pDC->SetTextColor(_GetGoodTextColor(EGA_TO_RGBQUAD(_pPalette[bIndex].color1)));

    pDC->DrawText(psz, -1, &rectSquare, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    pDC->SetBkMode(iBkMode);
    pDC->SetTextColor(colorOld);
}

//
// Returns a number between 0 and ((_cRows * _cColumns) - 1), or INVALID_COLORINDEX
// if the point did not lie on a square.
//
int CChooseColorStatic::_MapPointToIndex(CPoint point)
{
    int bRet = INVALID_COLORINDEX;

    RECT rc;
    GetClientRect(&rc); // Adjust a bit.
    InflateRect(&rc, -_paletteSquareInset, -_paletteSquareInset);

    if ((_cColumns > 0) && (_cRows > 0))
    {
        int cxSquare = RECTWIDTH(rc) / _cColumns;
        int cySquare = RECTHEIGHT(rc) / _cRows;

        int x = point.x - _paletteSquareInset;
        int y = point.y - _paletteSquareInset;
        int iColumn = x / cxSquare;
        int iRow = y / cySquare;
        if ((iColumn < _cColumns) && (iRow < _cRows))
        {
            bRet = ((iRow * _cColumns) + iColumn);
        }
    }
    return bRet;
}

void CChooseColorStatic::_MapRowColToRect(int iRow, int iColumn, RECT *prcOut)
{
    RECT rc;
    GetClientRect(&rc);
    InflateRect(&rc, -_paletteSquareInset, -_paletteSquareInset); // Adjust a bit.
    int cxSquare = RECTWIDTH(rc) / _cColumns;
    int cySquare = RECTHEIGHT(rc) / _cRows;
    prcOut->left = iColumn * cxSquare + _paletteSquareInset;
    prcOut->top = iRow * cySquare + _paletteSquareInset;
    prcOut->right = (iColumn + 1) * cxSquare;
    prcOut->bottom = (iRow + 1) * cySquare;
}


void CChooseColorStatic::_MapIndexToRect(BYTE bIndex, RECT *prc)
{
    if (bIndex < (_cColumns * _cRows))
    {
        int iRow = bIndex / _cColumns;
        int iColumn = bIndex % _cColumns;
        _MapRowColToRect(iRow, iColumn, prc);
    }
    else
    {
        // make it zero
        ZeroMemory(prc, sizeof(*prc));
    }
}

void CChooseColorStatic::_OnButtonDown(UINT nFlags, CPoint point, BOOL fLeft)
{
    int colorIndex = _MapPointToIndex(point);
    if (_autoHandleSelection)
    {
        if (fLeft && (colorIndex != INVALID_COLORINDEX))
        {
            if (_allowMultipleSelection)
            {
                _bSelectedColorIndex = (uint8_t)colorIndex; // Temporary

                bool control = !!(nFlags & MK_CONTROL);
                bool shift = !!(nFlags & MK_SHIFT);
                if (control)
                {
                    // Toggle.
                    if (!shift)
                    {
                        _multipleSelection[colorIndex] = !_multipleSelection[colorIndex];
                    }
                }
                else
                {
                    // Clear all
                    memset(_multipleSelection, 0, sizeof(bool) * (_cRows * _cColumns));
                }

                if (shift && (_focusIndex != INVALID_COLORINDEX))
                {
                    // Select the range from _focusIndex to colorIndex
                    for (int i = min(_focusIndex, colorIndex); i <= max(_focusIndex, colorIndex); i++)
                    {
                        _multipleSelection[i] = true;
                    }
                }
                else if (!control)
                {
                    _multipleSelection[colorIndex] = true;
                }
            }
            else
            {
                SetSelection((uint8_t)colorIndex);
            }
            Invalidate(FALSE);
        }
    }

    _focusIndex = colorIndex;

    if (_pCallback)
    {
        if (colorIndex != INVALID_COLORINDEX)
        {
            _pCallback->OnColorClick((uint8_t)colorIndex, this->GetDlgCtrlID(), fLeft);
        }
    }
}

void CChooseColorStatic::OnLButtonDown(UINT nFlags, CPoint point)
{
    _OnButtonDown(nFlags, point, TRUE);
}

void CChooseColorStatic::OnRButtonDown(UINT nFlags, CPoint point)
{
    _OnButtonDown(nFlags, point, FALSE);
}


void CChooseColorStatic::OnMouseMove(UINT nFlags, CPoint point)
{
    int bHoverIndex = _MapPointToIndex(point);
    if ((uint16_t)bHoverIndex != _bHoverIndex)
    {
        _bHoverIndex = bHoverIndex;
        if (_pCallback)
        {
            _pCallback->OnColorHover((uint8_t)_bHoverIndex);
        }
        RedrawWindow();
        //Invalidate(FALSE);
    }
}


void CChooseColorStatic::OnPaletteUpdated()
{
    // Someone changed the palette from under us.
    Invalidate(FALSE);
}

void CChooseColorStatic::SetSelection(bool *multipleSelection)
{
    _allowMultipleSelection = true;
    memcpy(_multipleSelection, multipleSelection, sizeof(bool) * (_cRows * _cColumns));
    Invalidate(FALSE);
}

void CChooseColorStatic::GetMultipleSelection(bool *multipleSelection)
{
    memcpy(multipleSelection, _multipleSelection, sizeof(bool) * (_cRows * _cColumns));
}



//
// CChooseBrushStatic
//
IMPLEMENT_DYNAMIC(CChooseBrushStatic, CChooseColorStatic)
CChooseBrushStatic::CChooseBrushStatic()
{
    _bHoverIndex = INVALID_COLORINDEX;
    _cRows = 0;
    _cColumns = 0;
    _fPrintIndex = FALSE;
    _fShowSelection = FALSE;
    _bSelectedColorIndex = 0;
    memset(_pPalette, 0, sizeof(_pPalette));
}

CChooseBrushStatic::~CChooseBrushStatic()
{
}

BEGIN_MESSAGE_MAP(CChooseBrushStatic, CChooseColorStatic)
    ON_WM_DRAWITEM_REFLECT()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


#define PATTERN_PAINT_SIZE 16
#define PATTERN_BMP_SIZE (PATTERN_PAINT_SIZE * PATTERN_PAINT_SIZE)

RGBQUAD RGBQUADFromCOLORREF(COLORREF color)
{
    RGBQUAD quad;
    quad.rgbBlue = GetBValue(color);
    quad.rgbRed = GetRValue(color);
    quad.rgbGreen = GetGValue(color);
    quad.rgbReserved = 0;
    return quad;
}

void CChooseBrushStatic::_DrawItem(CDC *pDC, int cx, int cy)
{
    // Fill in the background
    pDC->FillSolidRect(0, 0, cx, cy, g_PaintManager->GetColor(COLOR_BTNFACE));

    _DrawSelection(pDC);

    // Create a bitmap large enough for one brush.
    EGACOLOR color = { 0x00, 0x00 }; // Black

    BYTE dataBrush[PATTERN_BMP_SIZE];
    // Fake a PicData to draw into, with a visual brush.
    PicData data = { PicScreenFlags::Visual, (BYTE*)&dataBrush, nullptr, nullptr, nullptr, false };

    for (BYTE bPaletteIndex = 0; bPaletteIndex < (_cRows * _cColumns); bPaletteIndex++)
    {
        // Fill the bitmap with white each time.
        memset(&dataBrush, 0x0f, sizeof(dataBrush)); // Fill with white

        // Figure out what we want to draw (growing larger from left to right)
        // solid circles
        // pattern circles
        // solid squares
        // pattern squares

        PenStyle penStyle;
        PatternInfoFromIndex(bPaletteIndex, &penStyle);

        // Ask the pattern to draw itself
        
        DrawPatternInRect(PATTERN_PAINT_SIZE, PATTERN_PAINT_SIZE, &data, 8, 8, color, 0, 0, PicScreenFlags::Visual, &penStyle);

        // Now draw into the DC.
        RECT rectSquare;
        _MapIndexToRect(bPaletteIndex, &rectSquare);
        
        SCIBitmapInfo bmi(PATTERN_PAINT_SIZE, PATTERN_PAINT_SIZE);
        // Adjust the colors a little for the current scheme...
        bmi._colors[0xe] = RGBQUADFromCOLORREF(g_PaintManager->GetColor(COLOR_BTNFACE));
#pragma warning(suppress: 6386)
        bmi._colors[-1] = RGBQUADFromCOLORREF(g_PaintManager->GetColor(COLOR_BTNTEXT)); // REVIEW: Not sure why I'm using -1
        StretchDIBits((HDC)*pDC, rectSquare.left, rectSquare.top, RECTWIDTH(rectSquare), RECTHEIGHT(rectSquare),
            0, 0, PATTERN_PAINT_SIZE, PATTERN_PAINT_SIZE, &dataBrush, &bmi, DIB_RGB_COLORS, SRCCOPY);                    
    }

    _DrawHover(pDC);
}
