// FontPreviewView.cpp : implementation of the CFontPreviewView class
//

#include "stdafx.h"
#include "AppState.h"
#include "FontPreviewView.h"
#include "Components.h"
#include "RasterOperations.h"
#include "Font.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFontPreviewView

IMPLEMENT_DYNCREATE(CFontPreviewView, CView)


BEGIN_MESSAGE_MAP(CFontPreviewView, CScrollingThing<CView>)
    ON_WM_SIZE()
    ON_WM_LBUTTONDOWN()
    ON_WM_SETCURSOR()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


CFontPreviewView::CFontPreviewView()
{
    // Scrolling:
    _yOrigin = 0;
    _xOrigin = 0;

    // Default size of a font.
    _cyFont = 0;
    _cxFont = 0;
    _iZoom = 3; // Get this from somewhere.

    _fDoubleBuf = FALSE;
    _pbitmapDoubleBuf = nullptr;

    _fontResource = nullptr;

    _previewLetters = "!@#$%^&*()_+0123456789-=~`ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz{}|[]\\:\";'<>?,./";
}

CFontPreviewView::~CFontPreviewView()
{
    _CleanDoubleBuffer();
}

void CFontPreviewView::_CleanDoubleBuffer()
{
    // Reset our double buffering, so we regenerate it on the next paint cycle.
    _fDoubleBuf = FALSE;
    delete _pbitmapDoubleBuf;
    _pbitmapDoubleBuf = FALSE;
}


void CFontPreviewView::_GenerateDoubleBuffer(CDC *pDC)
{
    ASSERT(!_pbitmapDoubleBuf);
    _pbitmapDoubleBuf = new CBitmap();
    if (_pbitmapDoubleBuf)
    {
        _fDoubleBuf = !!_pbitmapDoubleBuf->CreateCompatibleBitmap(pDC, _cxFont, _cyFont);
    }
}

void CFontPreviewView::OnLButtonDown(UINT nFlags, CPoint point)
{
    int nChar;
    // Try to match it to a letter.
    _OnDraw(NULL, TRUE, point, &nChar);
    if (nChar != -1)
    {
        // We hit something.
        CNewRasterResourceDocument *pDoc = GetDocument();
        if (pDoc)
        {
            pDoc->SetSelectedCel(nChar);
        }
    }
}


void CFontPreviewView::_OnDraw(CDC *pDC, BOOL fHitTestOnly, CPoint point, int *pnChar)
{
    // Allocate enough room for a 128x128 character
    std::unique_ptr<BYTE[]> pData;
    if (fHitTestOnly)
    {
        // Convert the client point to "preview points"
        point.Offset(_xOrigin, _yOrigin);
        point.x /= _iZoom;
        point.y /= _iZoom;
        *pnChar = -1;
    }
    else
    {
        pData = std::make_unique<BYTE[]>(128 * 128);
    }

    const ResourceEntity *fontResource = _GetFont();
    if (fontResource)
    {
        int x = 0;
        int y = 0;
        const FontComponent &fontComponent = fontResource->GetComponent<FontComponent>();
        const RasterComponent &raster = fontResource->GetComponent<RasterComponent>();
        int cyLine = (int)fontComponent.LineHeight;
        for (size_t i = 0; i < _previewLetters.length(); i++)
        {
            uint8_t bChar = (uint8_t)_previewLetters[i];
            const Cel &cel = raster.GetCelFallback(CelIndex(0, bChar));
            size16 size = cel.size;
            assert(size.cx <= 127);
            assert(size.cy <= 127);
            if (!fHitTestOnly)
            {
                ::CopyBitmapData(cel, pData.get(), cel.size);
            }
            if ((x + size.cx) > (_GetViewWidth() / _iZoom))
            {
                // Move to next line.
                y += cyLine;
                x = 0;
            }
            if (fHitTestOnly)
            {
                // Hit test
                CRect rectHT(CPoint(x, y), SizeToCSize(size));
                if (rectHT.PtInRect(point))
                {
                    // Hit it. 
                    assert(pnChar);
                    *pnChar = (int)bChar;
                    break;
                }
            }
            else
            {
                // Draw it
                SCIBitmapInfo bmi(cel.size.cx, cel.size.cy);
                StretchDIBits((HDC)*pDC, x, y, cel.size.cx, cel.size.cy, 0, 0, cel.size.cx, cel.size.cy, pData.get(), &bmi, DIB_RGB_COLORS, SRCCOPY);
            }
            x += size.cx;
        }
    }
}

void CFontPreviewView::OnDraw(CDC *pDC)
{
    RECT rcClient;
    GetClientRect(&rcClient);
    CDC dcMem;
    if (dcMem.CreateCompatibleDC(pDC))
    {
        int cxVisible = min(_GetViewWidth(), RECTWIDTH(rcClient));
        int cyVisible = min(_GetViewHeight(), RECTHEIGHT(rcClient));
        if (!_fDoubleBuf)
        {
            _GenerateDoubleBuffer(pDC);
        }

        if (_fDoubleBuf)
        {
            ASSERT(_pbitmapDoubleBuf);
            HGDIOBJ hgdiObj = dcMem.SelectObject(_pbitmapDoubleBuf);

            // Fill background with white.
            dcMem.FillSolidRect(0, 0, _cxFont, _cyFont, RGB(0xff, 0xff, 0xff));

            // Draw the picture.
            _OnDraw(&dcMem);

            int sourceHeight = _cyFont * _iZoom;

            // Now blt back to the real DC.
            pDC->StretchBlt(-_xOrigin, -_yOrigin, _GetViewWidth(), _GetViewHeight(), &dcMem, 0, 0, _GetViewWidth() / _iZoom, sourceHeight / _iZoom, SRCCOPY);

            dcMem.SelectObject(hgdiObj);
        }
    }

    // Finish off by drawing a navy background around the font.
    CBrush brush(RGB(0, 0, 128));
    int cyBottom = RECTHEIGHT(rcClient) - _GetViewHeight();
    if (cyBottom > 0)
    {
        CRect rc(CPoint(0, _GetViewHeight()), CSize(RECTWIDTH(rcClient), cyBottom));
        pDC->FillRect(&rc, &brush);
    }
    int cxLeft = RECTWIDTH(rcClient) -  _GetViewWidth();
    if (cxLeft > 0)
    {
        CRect rc(CPoint(_GetViewWidth(), 0), CSize(cxLeft, RECTHEIGHT(rcClient)));
        pDC->FillRect(&rc, &brush);
    }
    __super::OnDraw(pDC);
}

void CFontPreviewView::OnSize(UINT nType, int cx, int cy)
{
    _RecalcHeight();
    __super::OnSize(nType, cx, cy);
}

BOOL CFontPreviewView::OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message)
{
    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
    return TRUE;
}

// We take our resource from a document, or from something set directly on it.
const ResourceEntity *CFontPreviewView::_GetFont()
{
    const ResourceEntity *pfr = _fontResource;
    if (pfr == NULL)
    {
        // Try to get from the document.
        CNewRasterResourceDocument *pDoc = GetDocument();
        if (pDoc)
        {
            pfr = pDoc->GetResource();
        }
    }
    return pfr;
}

void CFontPreviewView::_RecalcHeight()
{
    _CleanDoubleBuffer();

    // Then look at our client width.  Our width always stays the same, so we just have a vertical scrollbar
    CRect rect;
    GetClientRect(&rect);
    _cxFont = rect.Width();
    _cxPreview = rect.Width();

    const ResourceEntity *fontResource = _GetFont();
    if (fontResource)
    {
        // From these values, calculate the height.
        int x = 0;
        int y = 0;
        const RasterComponent &raster = fontResource->GetComponent<RasterComponent>();
        const FontComponent &font = fontResource->GetComponent<FontComponent>();
        int cyLine = (int)font.LineHeight;
        for (size_t i = 0; i < _previewLetters.length(); i++)
        {
            uint8_t bChar = (uint8_t)_previewLetters[i];
            const Cel &cel = raster.GetCelFallback(CelIndex(0, bChar));

            assert(cel.size.cx <= 127);
            assert(cel.size.cy <= 127);
            if ((x + cel.size.cx) >(_GetViewWidth() / _iZoom))
            {
                // Move to next line.
                y += cyLine;
                x = 0;
            }
            x += cel.size.cx;
        }

        _cyFont = y + cyLine;
        _InvalidateScrollStuff();
    }
}

void CFontPreviewView::_SyncLetters()
{
    // Try to get from the document.
    CNewRasterResourceDocument *pDoc = GetDocument();
    if (pDoc)
    {
        _previewLetters = pDoc->GetPreviewLetters();
    }
    // We must do a _RecalcHeight (which determines how high we are) before
    // we invalidate the scroll stuff (which needs to know how high we say we are).
    _RecalcHeight();
    _InvalidateScrollStuff();
}

void CFontPreviewView::OnInitialUpdate()
{
    __super::OnInitialUpdate();
    _SyncLetters();
}

void CFontPreviewView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
    // We have a new thing.
    __super::OnUpdate(pSender, lHint, pHint);

    RasterChangeHint hint = GetHint<RasterChangeHint>(pHint);

    // Refresh in all cases.
    if (hint != RasterChangeHint::None)
    {
        if (IsFlagSet(hint, RasterChangeHint::SampleText | RasterChangeHint::NewView))
        {
            _SyncLetters();
        }

        // Need to redraw the pic
        Invalidate(FALSE);
        _CleanDoubleBuffer(); // size migth have changed.
    }
}

//
// prevent flicker
//
BOOL CFontPreviewView::OnEraseBkgnd(CDC *pDC)
{
    return TRUE;
}

int CFontPreviewView::_GetViewHeight()
{
    return appState->AspectRatioY(_cyFont * _iZoom);
}

// CFontPreviewView diagnostics

#ifdef _DEBUG
void CFontPreviewView::AssertValid() const
{
    __super::AssertValid();
}

void CFontPreviewView::Dump(CDumpContext& dc) const
{
    __super::Dump(dc);
}

CNewRasterResourceDocument* CFontPreviewView::GetDocument() const // non-debug version is inline
{
    ASSERT(!m_pDocument || m_pDocument->IsKindOf(RUNTIME_CLASS(CNewRasterResourceDocument)));
    return static_cast<CNewRasterResourceDocument*>(m_pDocument);
}
#endif //_DEBUG