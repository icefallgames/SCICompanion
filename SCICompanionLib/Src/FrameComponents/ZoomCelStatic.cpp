#include "stdafx.h"
#include "ZoomCelStatic.h"
#include "PicDoc.h"
#include "format.h"
#include "AppState.h"

IMPLEMENT_DYNAMIC(ZoomCelStatic, CStatic)
ZoomCelStatic::ZoomCelStatic(int zoom, bool drawCrossHairs, bool ensureAllVisible, PicPosition picPosition) : _zoom(zoom), _cursorPos(0, 0), _currentZoomScreen(PicScreen::Visual), _be(nullptr), _pdm(nullptr), _drawCrossHairs(drawCrossHairs), _ensureAllVisible(ensureAllVisible), _picPosition(picPosition)
{
}

ZoomCelStatic::~ZoomCelStatic()
{
}

void ZoomCelStatic::SetCurrentScreen(PicScreen screen)
{
    if (_currentZoomScreen != screen)
    {
        _currentZoomScreen = screen;
        _Update();
    }
}

BEGIN_MESSAGE_MAP(ZoomCelStatic, CStatic)
    ON_WM_DRAWITEM_REFLECT()
END_MESSAGE_MAP()

void ZoomCelStatic::SetDrawManager(PicDrawManager *pdm)
{
    _pdm = pdm;
}

void ZoomCelStatic::OnUpdateBitmapEditor(IBitmapEditor *be)
{
    _be = be;   // This should never change
    _cursorPos = be->GetCursorPos();
    _Update();
}

CSize ZoomCelStatic::_GetBitmapSize()
{
    if (_be)
    {
        return _be->GetBitmapSize();
    }
    return CSize(DEFAULT_PIC_WIDTH, DEFAULT_PIC_HEIGHT);
}

void ZoomCelStatic::_Update()
{
    CSize size = _GetBitmapSize();
    // Hack
    if (!_drawCrossHairs)
    {
        _cursorPos.x = size.cx / 2;
        _cursorPos.y = size.cy / 2;
    }
    else
    {
        if (_be)
        {
            _cursorPos = _be->GetCursorPos();
        }
    }
    RedrawWindow();
}

const int ZoomGranularity = 16;

void ZoomCelStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CRect rcClient;
    GetClientRect(&rcClient);
    CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

    if (pDC)
    {
        int saveHandle = pDC->SaveDC();

        CSize size = _GetBitmapSize();
        int cxBitmap = size.cx;
        int cyBitmap = size.cy;
        int cyBitmapDest = appState->AspectRatioY(cyBitmap);

        // We might have to go lower than zoom = 1
        int zoom16 = ZoomGranularity * _zoom;
        bool restoreStretchBltMode = false;
        int prevBltMode = BLACKONWHITE;
        if (_ensureAllVisible)
        {
            zoom16 = min(zoom16, rcClient.Width() * ZoomGranularity / cxBitmap);
            zoom16 = min(zoom16, rcClient.Height() * ZoomGranularity / cyBitmapDest);
            if (zoom16 < ZoomGranularity)
            {
                restoreStretchBltMode = true;
                prevBltMode = pDC->SetStretchBltMode(HALFTONE);
            }
        }

        int width = rcClient.right - rcClient.left;
        int xCenter = (rcClient.right + rcClient.left) / 2;
        int height = rcClient.bottom - rcClient.top;
        int yCenter = (rcClient.bottom + rcClient.top) / 2;
        int xTopLeftPixel = xCenter - zoom16 / (2 * ZoomGranularity);
        int yTopLeftPixel = yCenter - appState->AspectRatioY(zoom16) / (2 * ZoomGranularity);
        int xTopLeftDest = xTopLeftPixel - _cursorPos.x * zoom16 / ZoomGranularity;
        int yTopLeftDest = yTopLeftPixel - appState->AspectRatioY(_cursorPos.y * zoom16 / ZoomGranularity);

        // Draw cross hairs.
        if (_drawCrossHairs)
        {
            CPen blackPen;
            blackPen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
            HGDIOBJ hOldPen = pDC->SelectObject(blackPen);
            pDC->MoveTo(rcClient.left, yCenter - 1);
            pDC->LineTo(rcClient.right, yCenter - 1);
            pDC->MoveTo(rcClient.left, yCenter + 1);
            pDC->LineTo(rcClient.right, yCenter + 1);
            pDC->MoveTo(xCenter - 1, rcClient.top);
            pDC->LineTo(xCenter - 1, rcClient.bottom);
            pDC->MoveTo(xCenter + 1, rcClient.top);
            pDC->LineTo(xCenter + 1, rcClient.bottom);
            CPen whitePen;
            whitePen.CreatePen(PS_SOLID, 1, RGB(196, 196, 196));
            pDC->SelectObject(whitePen);
            pDC->MoveTo(rcClient.left, yCenter);
            pDC->LineTo(rcClient.right, yCenter);
            pDC->MoveTo(xCenter, rcClient.top);
            pDC->LineTo(xCenter, rcClient.bottom);
            pDC->SelectObject(hOldPen);

            pDC->ExcludeClipRect(xCenter - 1, rcClient.top, xCenter + 2, rcClient.bottom);
            pDC->ExcludeClipRect(rcClient.left, yCenter - 1, rcClient.right, yCenter + 2);
        }

        if (_pdm)
        {

            // REVIEW: We rely on our host to ensure good performance by saying up front that we need
            // PostPlugin.
            BITMAPINFO *pbmi;
            _pdm->GetBitmapInfo(_currentZoomScreen, &pbmi);
            const uint8_t *screenBits = _pdm->GetPicBits(_currentZoomScreen, _picPosition, size16((uint16_t)size.cx, (uint16_t)size.cy));
            StretchDIBits(*pDC, xTopLeftDest, yTopLeftDest, cxBitmap * zoom16 / ZoomGranularity, cyBitmapDest * zoom16 / ZoomGranularity, 0, 0, cxBitmap, cyBitmap, screenBits, pbmi, DIB_RGB_COLORS, SRCCOPY);
            delete pbmi;

            pDC->ExcludeClipRect(xTopLeftDest, yTopLeftDest, xTopLeftDest + cxBitmap * zoom16 / ZoomGranularity, yTopLeftDest + cyBitmapDest * zoom16 / ZoomGranularity);
        }

        if (restoreStretchBltMode)
        {
            pDC->SetStretchBltMode(prevBltMode);
        }

        // Fill back ground where we didn't draw anything
        CBrush solidBrush;
        solidBrush.CreateSolidBrush(RGB(128, 128, 128));
        pDC->FillRect(&rcClient, &solidBrush);

        // Since we modified the clip rect
        // REVIEW: This is still causing painitng issues elsewhere, even though we restore the DC.
        pDC->RestoreDC(saveHandle);
    }
}
