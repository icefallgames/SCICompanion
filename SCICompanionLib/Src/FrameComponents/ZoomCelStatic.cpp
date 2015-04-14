#include "stdafx.h"
#include "ZoomCelStatic.h"
#include "PicDoc.h"
#include "format.h"

IMPLEMENT_DYNAMIC(ZoomCelStatic, CStatic)
ZoomCelStatic::ZoomCelStatic(int zoom, bool drawCrossHairs, PicPosition picPosition) : _zoom(zoom), _cursorPos(0, 0), _currentZoomScreen(PicScreen::Visual), _be(nullptr), _pdm(nullptr), _drawCrossHairs(drawCrossHairs), _picPosition(picPosition)
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

void ZoomCelStatic::_Update()
{
    // Hack
    if (!_drawCrossHairs)
    {
        _cursorPos.x = sPIC_WIDTH / 2;
        _cursorPos.y = sPIC_HEIGHT / 2;
    }
    else
    {
        _cursorPos = _be->GetCursorPos();
    }
    RedrawWindow();
}

void ZoomCelStatic::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    RECT rcClient;
    GetClientRect(&rcClient);
    CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    if (pDC)
    {
        int width = rcClient.right - rcClient.left;
        int xCenter = (rcClient.right + rcClient.left) / 2;
        int height = rcClient.bottom - rcClient.top;
        int yCenter = (rcClient.bottom + rcClient.top) / 2;
        int xTopLeftPixel = xCenter - _zoom / 2;
        int yTopLeftPixel = yCenter - _zoom / 2;
        int xTopLeftDest = xTopLeftPixel - _cursorPos.x * _zoom;
        int yTopLeftDest = yTopLeftPixel - _cursorPos.y * _zoom;

        // TODO: get actual bitmap dimensions
        int cxBitmap = 320;
        int cyBitmap = 190;

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
            const uint8_t *screenBits = _pdm->GetPicBits(_currentZoomScreen, _picPosition);
            StretchDIBits(*pDC, xTopLeftDest, yTopLeftDest, cxBitmap * _zoom, cyBitmap * _zoom, 0, 0, cxBitmap, cyBitmap, screenBits, pbmi, DIB_RGB_COLORS, SRCCOPY);
            delete pbmi;

            pDC->ExcludeClipRect(xTopLeftDest, yTopLeftDest, xTopLeftDest + cxBitmap * _zoom, yTopLeftDest + cyBitmap * _zoom);
        }


        // Fill back ground where we didn't draw anything
        CBrush solidBrush;
        solidBrush.CreateSolidBrush(RGB(128, 128, 128));
        pDC->FillRect(&rcClient, &solidBrush);
    }
}
