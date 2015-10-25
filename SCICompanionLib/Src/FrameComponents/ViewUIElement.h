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
#pragma once

// ViewUIElement, A control that shows a view

struct PaletteComponent;
struct RasterComponent;

class ViewUIElement : public CStatic
{
    DECLARE_DYNAMIC(ViewUIElement)

public:
    ViewUIElement();
    virtual ~ViewUIElement();

    void SetResource(const ResourceEntity *view, const PaletteComponent *optionalPalette = nullptr);

    void SetLoop(int nLoop);
    int GetLoop() const {
        return _nLoop;
    }
    void SetCel(int nCel, bool updateNow = false);
    void SetBackground(COLORREF color);
    void SetPatterned(bool isPatterned);
    void SetFillArea(bool fillArea);
    CSize GetSizeNeeded() { return _sizeWeDrawIn; }

private:
    void PreSubclassWindow() override;
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;

    void _OnDraw(CDC *pDC, LPRECT prc);
    void _GenerateDoubleBuffer(CDC *pDC, LPRECT prc);
    CSize _RecalcSizeNeeded();
    const Cel &GetCel();
    void _ValidateLoopCel();

    DECLARE_MESSAGE_MAP()

    const ResourceEntity *_viewResource;
    const PaletteComponent *_optionalPalette;
    std::unique_ptr<PaletteComponent> _paletteHolder;

    int _nLoop;
    CSize _sizeAnimate;     // size of us
    CSize _sizeWeDrawIn;    // The area within us in which we draw
    CRect _rectFullBounds;  // Not zoomed
    int _iZoom;
    CPoint _ptOrigin;
    std::unique_ptr<CBitmap> _pbitmapDoubleBuf;
    CSize _sizeDoubleBuf;
    int _nCel;

    bool _isBackgroundPatterned;
    COLORREF _bgColor;

    bool _fillArea;
};
