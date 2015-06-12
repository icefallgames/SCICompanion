// AnimateDialog.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "AnimateDialog.h"
#include "Components.h"
#include "RasterOperations.h"
#include "PaletteOperations.h"
using namespace std;

// CAnimateDialog dialog

CAnimateDialog::CAnimateDialog(CWnd* pParent /*=NULL*/, RasterComponent &raster, PaletteComponent *palette)
    : CExtResizableDialog(CAnimateDialog::IDD, pParent), _raster(raster), _palette(palette)
{
    _fDoubleBuffer = false;
    _nCel = 0;

    _rectDialogSize.SetRectEmpty();
    _fInitialized = false;
    _iZoom = 1;
    _ptOrigin = CPoint(0, 0);
    _rectFullBounds.SetRectEmpty();
    _sizeWeDrawIn.SetSize(0, 0);
    _fAnimating = true;
}

int CAnimateDialog::_GetCelCount()
{
    int celCount = 0;
    if (_nLoop < (int)_raster.Loops.size())
    {
        celCount = (int)_raster.Loops[_nLoop].Cels.size();
    }
    return celCount;
}

CRect GetCelRect(const Cel &cel)
{
    CRect rect;
    rect.left = cel.placement.x - cel.size.cx / 2;
    rect.right = rect.left + cel.size.cx;
    rect.bottom = cel.placement.y + 1;  // TODO: "adjust for SCI0 early"
    rect.top = rect.bottom - cel.size.cy;

    return rect;
}

CSize CAnimateDialog::_RecalcSizeNeeded()
{
    // Pretend that we're drawing at (0, 0)
    _rectFullBounds.SetRectEmpty();
    for (const Cel &cel : _raster.Loops[_nLoop].Cels)
    {
        CRect celRect = GetCelRect(cel);
        CRect newRect;
        newRect.UnionRect(&_rectFullBounds, &celRect);
        _rectFullBounds = newRect;
    }

    CSize sizeMax = _rectFullBounds.Size();

    // Try to fit the thing intelligently in our space.
    _iZoom = min(_sizeAnimate.cx / sizeMax.cx, _sizeAnimate.cy / sizeMax.cy);
    _iZoom = max(_iZoom, 1);
    sizeMax.cx *= _iZoom;
    sizeMax.cy *= _iZoom;
    return sizeMax;
}

//
// Called once at the beginning (or later, if the view data changes)
//
void CAnimateDialog::_AutoSize()
{
    CSize sizeMax = _RecalcSizeNeeded();

    CRect rect;
    m_wndAnimate.GetClientRect(&rect);
    int dx = sizeMax.cx - rect.Width();
    int dy = sizeMax.cy - rect.Height();

    if ((dx > 0) || (dy > 0))
    {
        CRect rectDialog;
        GetClientRect(&rectDialog);
        if (dx > 0)
        {
            rectDialog.right += dx;
        }
        if (dy > 0)
        {
            rectDialog.left += dy;
        }
        MoveWindow(&rectDialog);
    }
}

void CAnimateDialog::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_ANIMATE, m_wndAnimate);
    DDX_Control(pDX, IDC_SLIDER, m_wndSlider);
    DDX_Control(pDX, IDC_BUTTONPLAY, m_wndButton);
    CRect rectOrig;
    m_wndAnimate.GetClientRect(&rectOrig);
    _sizeAnimate.cx = rectOrig.Width();
    _sizeAnimate.cy = rectOrig.Height();
    _sizeWeDrawIn = _RecalcSizeNeeded();

    GetClientRect(&_rectDialogSize);

    _AutoSize();

    SetWindowText(_strTitle);
    SetTimer(ANIMATE_TIMER, 200, NULL);

    _fInitialized = true;

    m_wndSlider.SetRange(0, _raster.Loops[_nLoop].Cels.size() - 1);
    _UpdateButton();

    DDX_Control(pDX, IDOK, m_wndOK);
}

void CAnimateDialog::_UpdateButton()
{
    // Update command ui doesn't work here, so set the text directly.
    m_wndButton.SetWindowText(_fAnimating ? TEXT("Pause") : TEXT("Play"));
}

void CAnimateDialog::OnPlay()
{
    _fAnimating = !_fAnimating;
    _UpdateButton();
}


BEGIN_MESSAGE_MAP(CAnimateDialog, CExtResizableDialog)
    ON_WM_SIZE()
    ON_WM_DRAWITEM()
    ON_WM_TIMER()
    ON_WM_HSCROLL()
    ON_COMMAND(IDC_BUTTONPLAY, OnPlay)
END_MESSAGE_MAP()

void CAnimateDialog::OnSize(UINT nType, int cx, int cy)
{
    CRect rectDialogSize;
    GetClientRect(&rectDialogSize);

    if (_fInitialized)
    {
        int dx = rectDialogSize.Width() - _rectDialogSize.Width();
        int dy = rectDialogSize.Height() - _rectDialogSize.Height();

        if (dx || dy)
        {
            CRect rectAnimateScreen;
            m_wndAnimate.GetWindowRect(&rectAnimateScreen);
            ScreenToClient(&rectAnimateScreen);
            rectAnimateScreen.right = rectAnimateScreen.left + rectAnimateScreen.Width() + dx;
            rectAnimateScreen.bottom = rectAnimateScreen.top + rectAnimateScreen.Height() + dy;
            m_wndAnimate.MoveWindow(&rectAnimateScreen, TRUE);

            _sizeAnimate.cx = rectAnimateScreen.Width();
            _sizeAnimate.cy = rectAnimateScreen.Height();

            _sizeWeDrawIn = _RecalcSizeNeeded();

            int rgid[] = { IDOK, IDC_BUTTONPLAY };
            for (int i = 0; i < ARRAYSIZE(rgid); i++)
            {
                CWnd *pOk = GetDlgItem(rgid[i]);
                if (pOk)
                {
                    CRect rectOkScreen;
                    pOk->GetWindowRect(&rectOkScreen);
                    ScreenToClient(&rectOkScreen);
                    rectOkScreen.OffsetRect(dx, dy);
                    pOk->MoveWindow(&rectOkScreen, TRUE);
                }
            }

            CWnd *pSlider = GetDlgItem(IDC_SLIDER);
            if (pSlider)
            {
                CRect rectScreen;
                pSlider->GetWindowRect(&rectScreen);
                ScreenToClient(&rectScreen);
                rectScreen.OffsetRect(0, dy);
                pSlider->SetWindowPos(NULL, rectScreen.left, rectScreen.top, rectScreen.Width() + dx, rectScreen.Height(), SWP_NOZORDER);
            }

            m_wndAnimate.Invalidate(FALSE);
        }
    }
    _rectDialogSize = rectDialogSize;

    __super::OnSize(nType, cx, cy);
}

void CAnimateDialog::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == ANIMATE_TIMER)
    {
        if ((_raster.Loops.size() > 0) && _fAnimating)
        {
            int cCels = (int)_raster.Loops[_nLoop].Cels.size();
            _nCel++;
            _nCel %= cCels;
            m_wndSlider.SetPos(_nCel);
            m_wndAnimate.Invalidate(FALSE);
        }
    }
    else
    {
        __super::OnTimer(nIDEvent);
    }
}


void CAnimateDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pWnd)
{
    switch (nSBCode)
    {
    case SB_THUMBTRACK:
        _nCel = nPos;
        _fAnimating = false;
        _UpdateButton();
        m_wndAnimate.Invalidate(FALSE);
        break;

    case SB_LEFT:
    case SB_ENDSCROLL:
    case SB_LINELEFT:
    case SB_LINERIGHT:
    case SB_PAGELEFT:
    case SB_PAGERIGHT:
    case SB_RIGHT:
        _nCel = m_wndSlider.GetPos();
        _fAnimating = false;
        _UpdateButton();
        m_wndAnimate.Invalidate(FALSE);
        break;

    case SB_THUMBPOSITION:
        // We update constantly, so no need to do anything here.
        break;
    }
}

Cel &CAnimateDialog::GetCel()
{
    return _raster.Loops[_nLoop].Cels[_nCel];
}

//
// REVIEW: we're still off by one here, in some cases (due to placement)
//
void CAnimateDialog::_OnDraw(CDC *pDC, LPRECT prc)
{
    // First fill with a transparent background
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

    Cel &cel = GetCel();
    size_t cbViewData = CX_ACTUAL(cel.size.cx) * cel.size.cy;

    std::unique_ptr<BYTE[]> viewData = std::make_unique<BYTE[]>(cbViewData);
    ::CopyBitmapData(_raster, CelIndex(_nLoop, _nCel), viewData.get(), cel.size);

    RGBQUAD *palette = g_egaColors;
    int paletteCount = ARRAYSIZE(g_egaColors);
    if (_palette)
    {
        palette = _palette->Colors;
        paletteCount = ARRAYSIZE(_palette->Colors);
    }
    RGBQUAD transRGB = palette[cel.TransparentColor];
    COLORREF transparentColorRef = RGB(transRGB.rgbRed, transRGB.rgbGreen, transRGB.rgbBlue);
    SCIBitmapInfo bmi(cel.size.cx, cel.size.cy, palette, paletteCount);
    int cxDest = cel.size.cx * _iZoom;
    int cyDest = cel.size.cy * _iZoom;

    // _rectFullBounds defines the entire area a sequence of cels would occupy, assuming it is
    // drawn at (0, 0). Zoom is not taken into account.
    // _sizeWeDrawIn is the size of this rect, with zoom taken into account.
    // Here we want to draw a white square where the bound are.
    CPoint upperLeftBounds((RECTWIDTH(*prc) - _sizeWeDrawIn.cx) / 2, RECTHEIGHT(*prc) - _sizeWeDrawIn.cy);
    CBrush brush;
    brush.CreateSolidBrush(RGB(255, 255, 255));
    CRect rectFoo(upperLeftBounds.x, upperLeftBounds.y, upperLeftBounds.x + _sizeWeDrawIn.cx, upperLeftBounds.y + _sizeWeDrawIn.cy);

    // Now where do we draw the actual view?
    CRect celRect = GetCelRect(cel);
    upperLeftBounds += CPoint((celRect.left - _rectFullBounds.left) * _iZoom, (celRect.top - _rectFullBounds.top) * _iZoom);

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

void CAnimateDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if (nIDCtl == IDC_ANIMATE)
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
    else
    {
        __super::OnDrawItem(nIDCtl, lpDrawItemStruct);
    }
}

void CAnimateDialog::_GenerateDoubleBuffer(CDC *pDC, LPRECT prc)
{
    if (!_fDoubleBuffer || (_sizeDoubleBuf != _sizeAnimate))
    {
        _sizeDoubleBuf = _sizeAnimate;
        _pbitmapDoubleBuf = make_unique<CBitmap>();
        _fDoubleBuffer = !!_pbitmapDoubleBuf->CreateCompatibleBitmap(pDC, _sizeDoubleBuf.cx, _sizeDoubleBuf.cy);
    }
}
