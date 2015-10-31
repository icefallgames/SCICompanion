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
#include "PhonemeEditor.h"
#include "AppState.h"
#include "RasterOperations.h"
#include "View.h"
#include "PhonemeMap.h"
#include "ExtPaintManager.h"
#include "format.h"
#include "PaletteOperations.h"

using namespace std;

const int ImageBarWidth = 60;
const int PhonemeStartX = 70;

IMPLEMENT_DYNAMIC(PhonemeEditor, CStatic)

std::unordered_map<std::string, std::string> CreatePhonemeToExampleMap();

PhonemeEditor::~PhonemeEditor() {}

PhonemeEditor::PhonemeEditor(IPhonemeMapNotify *notify, const ResourceEntity *viewResource, int view, int loop, PhonemeMap &map)
    : CStatic(),
    _viewResource(viewResource),
    _nLoop(loop),
    _nView(view),
    _map(map),
    _phonemeWidth(0),
    _phonemeHeight(0),
    _capturing(false),
    _highlightIndex(-1, -1),
    _dragging(false),
    _notify(notify),
    _dragOverIndex(-1, -1)
{
    
    const RasterComponent &raster = _viewResource->GetComponent<RasterComponent>();
    _nLoop = min(_nLoop, raster.LoopCount() - 1);   // Just in case

    // Prepare our cache
    size_t celCount = raster.Loops[_nLoop].Cels.size();
    _cache.resize(celCount);
    _rowYs.resize(celCount + 1);
    for (const auto &entry : map.Entries())
    {
        uint16_t cel = min((uint16_t)entry.second, (uint16_t)celCount);
        cel = min(cel, (uint16_t)(_cache.size() - 1));
        _cache[cel].push_back(entry.first);
    }

    _phonemeToExample = CreatePhonemeToExampleMap();
}

BEGIN_MESSAGE_MAP(PhonemeEditor, CStatic)
    ON_WM_DRAWITEM_REFLECT()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

CRect PhonemeEditor::_GetIndexRect(CPoint index)
{
    CRect rc;
    if ((index.y >= 0) && (index.x >= 0))
    {
        rc.left = PhonemeStartX + index.x * _phonemeWidth + (_phonemeWidth / 5);    // /5 -> offset a bit left so it appears more centered.
        rc.right = rc.left + _phonemeWidth;
        rc.top = _rowYs[index.y];
        int rowHeight = _rowYs[index.y + 1] - rc.top;
        int yCenter = (rowHeight - _phonemeHeight) / 2;
        rc.top += yCenter;
        rc.bottom = rc.top + _phonemeHeight;
    }
    return rc;
}

CRect PhonemeEditor::_GetRowRect(int row)
{
    CRect rc;
    if (row >= 0)
    {
        GetClientRect(&rc);
        rc.top = _rowYs[row];
        rc.bottom = _rowYs[row + 1];
    }
    return rc;
}

void PhonemeEditor::_InvalidateRow(int row)
{
    CRect rc = _GetRowRect(row);
    InvalidateRect(&rc, FALSE);
}
void PhonemeEditor::_InvalidateIndex(CPoint pt)
{
    CRect rc = _GetIndexRect(pt);
    InvalidateRect(&rc, FALSE);
}

void PhonemeEditor::_ShowPhonemeTipAtCursor(CPoint pt)
{
    std::string phoneme = _cache[_dragSourceIndex.y][_dragSourceIndex.x];
    std::string tooltipText = fmt::format("{0} ({1})", phoneme, _phonemeToExample[phoneme]);
    CDC *pDC = GetDC();
    if (pDC)
    {
        CRect rcMeasure;
        pDC->DrawText(tooltipText.c_str(), &rcMeasure, DT_SINGLELINE | DT_CALCRECT);
        int x = pt.x - rcMeasure.Width() / 2;
        int y = pt.y;
        CRect rc;
        GetWindowRect(&rc);
        _toolTip.Show(CPoint(x, y) + rc.TopLeft(), tooltipText.c_str());
    }
}

void PhonemeEditor::_ShowPhonemeTip(CPoint index)
{
    std::string phoneme = _cache[index.y][index.x];
    std::string tooltipText = fmt::format("{0} ({1})", phoneme, _phonemeToExample[phoneme]);
    CDC *pDC = GetDC();
    if (pDC)
    {
        CRect rcMeasure;
        pDC->DrawText(tooltipText.c_str(), &rcMeasure, DT_SINGLELINE | DT_CALCRECT);
        CRect rcPhoneme = _GetIndexRect(index);
        int xCenter = (rcMeasure.Width() - rcPhoneme.Width()) / 2;
        int x = rcPhoneme.left - xCenter;
        int y = rcPhoneme.top - 28;
        CRect rc;
        GetWindowRect(&rc);
        _toolTip.Show(CPoint(x, y) + rc.TopLeft(), tooltipText.c_str());
    }
}

void PhonemeEditor::_SetHighlight(CPoint index)
{
    if (index != _highlightIndex)
    {
        // Invalidate both indices
        _InvalidateIndex(_highlightIndex);
        _InvalidateIndex(index);
        _highlightIndex = index;

        if (!_toolTip)
        {
            _toolTip.CreateEx(0, _T("STATIC"), nullptr, SS_NOTIFY | SS_OWNERDRAW | WS_POPUP | WS_CLIPSIBLINGS | WS_BORDER, 0, 0, 100, 100, GetSafeHwnd(), nullptr);
            assert(_toolTip);
        }

        if ((_highlightIndex.y != -1) && (_highlightIndex.x != -1))
        {
            _toolTip.SetColor(GetSysColor(COLOR_INFOBK));
            _ShowPhonemeTip(index);
        }
        else
        {
            _toolTip.Hide();
        }
    }
}

void PhonemeEditor::_SetDragOverIndex(CPoint index, bool force)
{
    if (force || (_dragOverIndex.y != index.y))
    {
        _InvalidateRow(index.y);
        _InvalidateRow(_dragOverIndex.y);
    }
    _dragOverIndex = index;
}

const int DragDelta = 3;

void PhonemeEditor::OnMouseMove(UINT nFlags, CPoint point)
{
    CPoint ptScreen;
    ClientToScreen(&ptScreen);

    if (_capturing)
    {
        if (!_dragging)
        {
            if ((abs(point.x - _dragSourcePoint.x) > DragDelta) ||
                (abs(point.x - _dragSourcePoint.x) > DragDelta))
            {
                _dragging = true;
                _toolTip.SetColor(RGB(255, 196, 196));
            }
        }

        if (_dragging)
        {
            _SetDragOverIndex(_HitTest(point));
            _ShowPhonemeTipAtCursor(point);
        }
    }
    else
    {
        _hoverPointCurrent = point;
        _SetHighlight(_HitTest(point));
    }
}

void PhonemeEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
    CPoint index = _HitTest(point);
    if ((index.y != -1) && (index.x != -1))
    {
        // We hit a phoneme
        _dragSourceIndex = index;
        _capturing = true;
        _dragSourcePoint = point;
        _SetHighlight(CPoint(-1, -1));
        SetCapture();
    }
}
void PhonemeEditor::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (_capturing)
    {
        _capturing = false;
        _dragging = false;
        _toolTip.Hide();
        ReleaseCapture();

        _SetDragOverIndex(_HitTest(point), true);
        if ((_dragOverIndex.y != -1) && (_dragOverIndex.y != _dragSourceIndex.y))
        {
            // Move this phoneme
            auto it = _cache[_dragSourceIndex.y].begin() + _dragSourceIndex.x;
            std::string phoneme = *it;
            _cache[_dragSourceIndex.y].erase(it);
            int insertPoint = _dragOverIndex.x;
            if (insertPoint == -1)
            {
                insertPoint = _cache[_dragOverIndex.y].size();
            }
            _cache[_dragOverIndex.y].insert(_cache[_dragOverIndex.y].begin() + insertPoint, phoneme);
            _InvalidateRow(_dragOverIndex.y);
            _InvalidateRow(_dragSourceIndex.y);

            // Update the phoneme map
            _map.SetCel(phoneme, (uint16_t)_dragOverIndex.y);

            // And notify the dialog...
            _notify->OnMapUpdated();

            _SetDragOverIndex(CPoint(-1, -1), true);
        }
    }
}
// Not sure if I need this yet
//ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
// LRESULT  CMDITabsDialogBar::OnMouseLeave(WPARAM wParam, LPARAM lParam)


void PhonemeEditor::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    LPRECT prc = &lpDrawItemStruct->rcItem;
    CDC dc;
    dc.Attach(lpDrawItemStruct->hDC);

    CRect rcDoubleBuf(prc->left, prc->top, prc->left + ImageBarWidth, prc->bottom);
    _GenerateDoubleBuffer(&dc, &rcDoubleBuf);

    CDC dcMem;
    if (dcMem.CreateCompatibleDC(&dc))
    {
        if (_pbitmapDoubleBuf)
        {
            HGDIOBJ hOldBmp = dcMem.SelectObject(_pbitmapDoubleBuf.get());
             
            // Copy into the final buffer:
            dc.StretchBlt(0, 0, _sizeDoubleBuf.cx, _sizeDoubleBuf.cy, &dcMem, 0, 0, _sizeDoubleBuf.cx, _sizeDoubleBuf.cy, SRCCOPY);

            const Loop &loop = _viewResource->GetComponent<RasterComponent>().Loops[_nLoop];

            CRect rcMeasure = {};
            dc.DrawText("MMM", &rcMeasure, DT_SINGLELINE | DT_CALCRECT);
            int textWidth = RECTWIDTH(rcMeasure);
            _phonemeWidth = textWidth;
            _phonemeHeight = RECTHEIGHT(rcMeasure);
            std::unique_ptr<int[]> countPerCel = std::make_unique<int[]>(loop.Cels.size());

            CRect rcMain = *prc;
            rcMain.left = ImageBarWidth;
            dc.FillSolidRect(&rcMain, g_PaintManager->GetColor(COLOR_BTNFACE));

            COLORREF crTextOld = dc.GetTextColor();

            for (size_t row = 0; row < _cache.size(); row++)
            {
                for (size_t column = 0; column < _cache[row].size(); column++)
                {
                    CPoint index = CPoint(column, row);
                    dc.SetTextColor((index == _highlightIndex) ? RGB(255, 255, 255) : RGB(0, 0, 0));
                    CRect rc = _GetIndexRect(index);
                    dc.DrawText(_cache[row][column].c_str(), &rc, DT_SINGLELINE);
                }

                // Highlight the selected row
                if (_dragging && (row == _dragOverIndex.y))
                {
                    CPen pen(PS_SOLID, 1, RGB(255, 0, 0));
                    HGDIOBJ hOldPen = dc.SelectObject(pen);
                    CRect rcRow = _GetRowRect(row);
                    rcRow.left = ImageBarWidth;
                    rcRow.right--;
                    rcRow.bottom--;
                    dc.MoveTo(rcRow.left, rcRow.top);
                    dc.LineTo(rcRow.right, rcRow.top);
                    dc.LineTo(rcRow.right, rcRow.bottom);
                    dc.LineTo(rcRow.left, rcRow.bottom);
                    dc.LineTo(rcRow.left, rcRow.top);
                    dc.SelectObject(hOldPen);
                }
            }

            dc.SetTextColor(crTextOld);

            dcMem.SelectObject(hOldBmp);
        }
    }

    dc.Detach();
}

CPoint PhonemeEditor::_HitTest(CPoint pt)
{
    int xOffset = pt.x - PhonemeStartX;
    int xIndex = xOffset / _phonemeWidth;

    int yIndex = -1;
    for (size_t row = 1; row <= _cache.size(); row++)
    {
        if (pt.y < _rowYs[row])
        {
            yIndex = row - 1;
            break;
        }
    }

    CPoint index(-1, yIndex);
    if (yIndex != -1)
    {
        if (xIndex < (int)_cache[yIndex].size())
        {
            index.x = xIndex;
        }
        if (xIndex < -1)
        {
            index.x = -1;
        }
    }
    assert(index.x >= -1);
    return index;
}

void PhonemeEditor::_EnsureSCIBitmap(CSize size)
{
    if (_sciBitmap)
    {
        CSize sizeBitmap = _sciBitmap->GetBitmapDimension();
        if ((sizeBitmap.cx < size.cx) || (sizeBitmap.cy < size.cy))
        {
            _sciBitmap.reset(nullptr);
            _dibBits = nullptr;
        }
    }

    if (!_sciBitmap && _viewResource)
    {
        std::unique_ptr<PaletteComponent> palette = appState->GetResourceMap().GetMergedPalette(*_viewResource, 999);
        SCIBitmapInfo bmi(size.cx, -size.cy, palette ? palette->Colors : nullptr, palette ? ARRAYSIZE(palette->Colors) : 0);
        _sciBitmap = make_unique<CBitmap>();
        _sciBitmap->Attach(CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&_dibBits), nullptr, 0));
        _sciBitmap->SetBitmapDimension(size.cx, size.cy);
    }
}


void PhonemeEditor::_DrawCels(CDC *pDC, LPRECT prc)
{
    CDC dcMem;
    if (dcMem.CreateCompatibleDC(pDC))
    {
        HGDIOBJ hOldBmp = dcMem.SelectObject(_pbitmapDoubleBuf.get());

        // Background:
        dcMem.FillSolidRect(prc, RGB(196, 196, 196));

        if (_viewResource)
        {
            const RasterComponent &raster = _viewResource->GetComponent<RasterComponent>();
            if (_nLoop < raster.LoopCount())
            {
                const Loop &loop = raster.Loops[_nLoop];
                int maxY = 0;
                int maxX = 0;
                int celCount = (int)loop.Cels.size();
                for (const Cel &cel : loop.Cels)
                {
                    maxX = max(cel.size.cx, maxX);
                    maxY = max(cel.size.cy, maxY);
                }

                // We want to spread our guys out vertically.
                int height = RECTHEIGHT(*prc) * maxX / ImageBarWidth;
                height = max(height, maxY * celCount);  // At least this high.

                int perItemHeight = min(height / celCount, maxY);

                _EnsureSCIBitmap(CSize(maxX, height));

                int yCenterOffset = (perItemHeight - maxY) / 2;
                int row = 0;
                for (const Cel &cel : loop.Cels)
                {
                    int y = row * height / celCount;
                    y += yCenterOffset;
                    CopyBitmapData(cel, _dibBits, 0, y, CX_ACTUAL(_sciBitmap->GetBitmapDimension().cx), true);
                    row++;
                }

                CDC dcMem2;
                if (dcMem2.CreateCompatibleDC(pDC))
                {
                    HGDIOBJ hold2 = dcMem2.SelectObject(*_sciBitmap);
                    StretchBlt(dcMem, 0, 0, _sizeDoubleBuf.cx, _sizeDoubleBuf.cy,
                        dcMem2, 0, 0, maxX, height, SRCCOPY);
                    dcMem2.SelectObject(hold2);
                }
            }
        }

        dcMem.SelectObject(hOldBmp);
    }
}

void PhonemeEditor::_GenerateDoubleBuffer(CDC *pDC, LPRECT prc)
{
    CSize sizeCurrent = CSize(RECTWIDTH(*prc), RECTHEIGHT(*prc));
    if (!_pbitmapDoubleBuf || (_sizeDoubleBuf != sizeCurrent))
    {
        _sizeDoubleBuf = sizeCurrent;
        for (size_t row = 0; row < _rowYs.size(); row++)
        {
            _rowYs[row] = row * _sizeDoubleBuf.cy / _cache.size();
        }
        _pbitmapDoubleBuf = make_unique<CBitmap>();
        // Hope this succeededs...
        if (_pbitmapDoubleBuf->CreateCompatibleBitmap(pDC, _sizeDoubleBuf.cx, _sizeDoubleBuf.cy))
        {
            _DrawCels(pDC, prc);
        }
    }
}
