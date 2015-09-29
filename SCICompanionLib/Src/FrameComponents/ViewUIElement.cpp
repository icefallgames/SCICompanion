#include "stdafx.h"
#include "ViewUIElement.h"
#include "AppState.h"
#include "Components.h"
#include "RasterOperations.h"
#include "PaletteOperations.h"

using namespace std;

CRect GetCelRect(const Cel &cel)
{
    CRect rect;
    rect.left = cel.placement.x - cel.size.cx / 2;
    rect.right = rect.left + cel.size.cx;
    rect.bottom = cel.placement.y + 1;  // TODO: "adjust for SCI0 early"
    rect.top = rect.bottom - cel.size.cy;

    return rect;
}

IMPLEMENT_DYNAMIC(ViewUIElement, CStatic)

// ViewUIElement control

ViewUIElement::~ViewUIElement() {}

ViewUIElement::ViewUIElement()
    : CStatic(),
    _fDoubleBuffer(false),
    _nCel(0),
    _nLoop(0),
    _fInitialized(false),
    _iZoom(1),
    _bgColor(RGB(255, 0, 255)),
    _isBackgroundPatterned(false)
{
}

CSize ViewUIElement::_RecalcSizeNeeded()
{
    CSize sizeMax;

    if (_viewResource)
    {
        // Pretend that we're drawing at (0, 0)
        _rectFullBounds.SetRectEmpty();
        for (const Cel &cel : _viewResource->GetComponent<RasterComponent>().Loops[_nLoop].Cels)
        {
            CRect celRect = GetCelRect(cel);
            CRect newRect;
            newRect.UnionRect(&_rectFullBounds, &celRect);
            _rectFullBounds = newRect;
        }

        sizeMax = _rectFullBounds.Size();
        sizeMax.cy = appState->AspectRatioY(sizeMax.cy);

        // Try to fit the thing intelligently in our space.
        _iZoom = min(_sizeAnimate.cx / sizeMax.cx, _sizeAnimate.cy / sizeMax.cy);
        _iZoom = max(_iZoom, 1);
        sizeMax.cx *= _iZoom;
        sizeMax.cy *= _iZoom;
    }
    return sizeMax;
}

BEGIN_MESSAGE_MAP(ViewUIElement, CStatic)
    ON_WM_SIZE()
    ON_WM_DRAWITEM_REFLECT()
END_MESSAGE_MAP()

void ViewUIElement::OnSize(UINT nType, int cx, int cy)
{
    _sizeAnimate.cx = cx;
    _sizeAnimate.cy = cy;
    _sizeWeDrawIn = _RecalcSizeNeeded();

    __super::OnSize(nType, cx, cy);
}

const Cel &ViewUIElement::GetCel()
{
    assert(_viewResource);
    return _viewResource->GetComponent<RasterComponent>().Loops[_nLoop].Cels[_nCel];
}

void ViewUIElement::SetLoop(int nLoop)
{
    _nLoop = nLoop;
    _sizeWeDrawIn = _RecalcSizeNeeded();
    Invalidate(FALSE);
}

void ViewUIElement::SetCel(int nCel)
{
    _nCel = nCel;
    Invalidate(FALSE);
}

void ViewUIElement::SetBackground(COLORREF color) { _bgColor = color; Invalidate(FALSE); }

void ViewUIElement::SetPatterned(bool isPatterned) { _isBackgroundPatterned = isPatterned; Invalidate(FALSE); }

void ViewUIElement::SetResource(const ResourceEntity *view, const PaletteComponent *optionalPalette)
{
    _viewResource = view;
    _optionalPalette = optionalPalette;
    _sizeWeDrawIn = _RecalcSizeNeeded();
    Invalidate(FALSE);
}

//
// REVIEW: we're still off by one here, in some cases (due to placement)
//
void ViewUIElement::_OnDraw(CDC *pDC, LPRECT prc)
{
    if (!_fInitialized)
    {
        _fInitialized = true;
        CRect rect;
        GetClientRect(&rect);
        OnSize(0, rect.Width(), rect.Height());
    }

    // First fill with a transparent background
    if (_isBackgroundPatterned)
    {
        CBitmap bm;
        RGBQUAD a = { 204, 204, 204, 0 };
        RGBQUAD b = { 255, 255, 255, 0 };
        if (CreateDCCompatiblePattern(a, b, 8, 8, pDC, &bm))
        {
            CBrush brushPat;
            if (brushPat.CreatePatternBrush(&bm))
            {
                pDC->FillRect(prc, &brushPat);
            }
        }
    }
    else
    {
        pDC->FillSolidRect(prc, _bgColor);
    }

    if (_viewResource)
    {
        const Cel &cel = GetCel();
        size_t cbViewData = CX_ACTUAL(cel.size.cx) * cel.size.cy;

        std::unique_ptr<BYTE[]> viewData = std::make_unique<BYTE[]>(cbViewData);
        ::CopyBitmapData(_viewResource->GetComponent<RasterComponent>(), CelIndex(_nLoop, _nCel), viewData.get(), cel.size);

        const RGBQUAD *palette = g_egaColors;
        int paletteCount = ARRAYSIZE(g_egaColors);
        if (_optionalPalette)
        {
            palette = _optionalPalette->Colors;
            paletteCount = ARRAYSIZE(_optionalPalette->Colors);
        }
        else
        {
            const PaletteComponent *viewPalette = _viewResource->TryGetComponent<PaletteComponent>();
            if (viewPalette)
            {
                palette = viewPalette->Colors;
                paletteCount = ARRAYSIZE(_optionalPalette->Colors);
            }
        }
        RGBQUAD transRGB = palette[cel.TransparentColor];
        COLORREF transparentColorRef = RGB(transRGB.rgbRed, transRGB.rgbGreen, transRGB.rgbBlue);
        SCIBitmapInfo bmi(cel.size.cx, cel.size.cy, palette, paletteCount);
        int cxDest = cel.size.cx * _iZoom;
        int cyDest = appState->AspectRatioY(cel.size.cy * _iZoom);

        // _rectFullBounds defines the entire area an overlapped sequence of cels would occupy, assuming it is
        // drawn at (0, 0). Zoom is not taken into account.
        // _sizeWeDrawIn is the size of this rect, with zoom taken into account.
        // Here we want to draw a white square where the bound are.
        CPoint upperLeftBounds((RECTWIDTH(*prc) - _sizeWeDrawIn.cx) / 2, RECTHEIGHT(*prc) - _sizeWeDrawIn.cy);
        CBrush brush;
        brush.CreateSolidBrush(RGB(255, 255, 255));
        CRect rectFoo(upperLeftBounds.x, upperLeftBounds.y, upperLeftBounds.x + _sizeWeDrawIn.cx, upperLeftBounds.y + _sizeWeDrawIn.cy);

        // Now where do we draw the actual view?
        CRect celRect = GetCelRect(cel);
        upperLeftBounds += CPoint((celRect.left - _rectFullBounds.left) * _iZoom, appState->AspectRatioY((celRect.top - _rectFullBounds.top) * _iZoom));

        CDC dcMem;
        if (dcMem.CreateCompatibleDC(pDC))
        {
            BITMAPV5HEADER bih = {};
            bih.bV5Size = sizeof(bih);
            bih.bV5Width = cel.size.cx;
            bih.bV5Height = cel.size.cy;
            bih.bV5Planes = 1;
            bih.bV5BitCount = 8;
            bih.bV5Compression = BI_RGB;
            bih.bV5CSType = LCS_WINDOWS_COLOR_SPACE;
            bih.bV5Intent = LCS_GM_BUSINESS;
            HBITMAP hbmpTemp = CreateDIBitmap(*pDC, (BITMAPINFOHEADER *)&bih, CBM_INIT, viewData.get(), &bmi, DIB_RGB_COLORS);
            if (hbmpTemp)
            {
                HGDIOBJ hOld = dcMem.SelectObject(hbmpTemp);
                // Note that we really want to use a transparent palette *index*, not an rgb color. Oh well, this will work for most cases.
                TransparentBlt((HDC)*pDC, upperLeftBounds.x, upperLeftBounds.y, cxDest, cyDest, dcMem, 0, 0, cel.size.cx, cel.size.cy, transparentColorRef);
                dcMem.SelectObject(hOld);

                DeleteObject(hbmpTemp);
            }
        }
    }
}

void ViewUIElement::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    LPRECT prc = &lpDrawItemStruct->rcItem;
    CDC dc;
    dc.Attach(lpDrawItemStruct->hDC);

    _GenerateDoubleBuffer(&dc, prc);

    CDC dcMem;
    if (dcMem.CreateCompatibleDC(&dc))
    {
        if (_pbitmapDoubleBuf)
        {
            HGDIOBJ hOldBmp = dcMem.SelectObject(_pbitmapDoubleBuf.get());

            // Now we're ready to draw.
            _OnDraw(&dcMem, prc);

            // Copy into the final buffer:
            dc.StretchBlt(0, 0, RECTWIDTH(*prc), RECTHEIGHT(*prc), &dcMem, 0, 0, RECTWIDTH(*prc), RECTHEIGHT(*prc), SRCCOPY);

            dcMem.SelectObject(hOldBmp);
        }
    }

    dc.Detach();
}

void ViewUIElement::_GenerateDoubleBuffer(CDC *pDC, LPRECT prc)
{
    if (!_fDoubleBuffer || (_sizeDoubleBuf != _sizeAnimate))
    {
        _sizeDoubleBuf = _sizeAnimate;
        _pbitmapDoubleBuf = make_unique<CBitmap>();
        _fDoubleBuffer = !!_pbitmapDoubleBuf->CreateCompatibleBitmap(pDC, _sizeDoubleBuf.cx, _sizeDoubleBuf.cy);
    }
}

