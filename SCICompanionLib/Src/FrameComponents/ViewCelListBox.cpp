#include "stdafx.h"
#include "ViewCelListBox.h"
#include "NewRasterResourceDocument.h"
#include "RasterOperations.h"
#include "PaletteOperations.h"
#include "format.h"
#include "AppState.h"

using namespace std;

const int ScaleOne = 4;
const int CaptionHeight = 20;   // REVIEW: Really should be DPI-aware

IMPLEMENT_DYNCREATE(ViewCelListBox, CView)

CelIndex g_NoIndex(-1, -1);

const int MinDragDelta = 3;
const int InsertMargin = 20;    // Hit-testing for insert bar

struct LeftToRight : public DirectionalityBase
{
    int A(const CSize &size) override { return size.cx; }
    int B(const CSize &size) override { return size.cy; }
    int A(const CPoint &point) override { return point.x; }
    int B(const CPoint &point) override { return point.y; }
    int ItemCount(const CelIndex &index, const RasterComponent &raster) override { return (int)raster.Loops[index.loop].Cels.size(); }
    int ItemIndex(const CelIndex &index) override { return index.cel; }
    CSize Bounds(int itemCount) override { return CSize(itemCount, 1); }
    CSize GetItemSize(const CelIndex &index, const RasterComponent &raster, int itemIndex) override
    {
        const Cel &cel = raster.GetCel(CelIndex(index.loop, itemIndex));
        return CSize(cel.size.cx, cel.size.cy);
    }
    CSize CalcBitmapSize(const CelIndex &index, const RasterComponent &raster) override
    {
        CSize size;
        const Loop &loop = raster.Loops[index.loop];
        for (const Cel &cel : loop.Cels)
        {
            size.cx += cel.size.cx;
            size.cy = max(size.cy, (int)cel.size.cy);
        }
        return size;
    }
    CSize GetBitmapData(const CelIndex &index, const RasterComponent &raster, CBitmap &bitmap, uint8_t *dibBits) override
    {
        CSize size(1, 1);
        const Loop &loop = raster.Loops[index.loop];
        int x = 0;
        for (const Cel &cel : loop.Cels)
        {
            CopyBitmapData(cel, dibBits, x, 0, CX_ACTUAL(bitmap.GetBitmapDimension().cx), true);
            size.cx = max(size.cx, (int)cel.size.cx);
            size.cy = max(size.cy, (int)cel.size.cy);
            x += cel.size.cx;
        }
        return size;
    }
    CPoint IncrementSourcePos(CSize size) override { return CPoint(size.cx, 0); }
    CPoint DimensionMultiplier() override  { return CPoint(1, 0); }
    CPoint CenteringMultiplier() override { return CPoint(1, 1); }
    void SetIndex(CelIndex &celIndex, int index) override { celIndex.cel = index; }
    bool IsLoops() override { return false; }
    int GetLogicalScale(int scale) override { return scale; }
    std::string GetCaption(const CelIndex &index) override
    {
        return fmt::format("Cels for loop {0}", index.loop);
    }
};

struct TopToBottom : public DirectionalityBase
{
    int A(const CSize &size) override { return size.cy; }
    int B(const CSize &size) override { return size.cx; }
    int A(const CPoint &point) override { return point.y; }
    int B(const CPoint &point) override { return point.x; }
    int ItemCount(const CelIndex &index, const RasterComponent &raster) override { return raster.LoopCount(); }
    int ItemIndex(const CelIndex &index) override { return index.loop; }
    CSize Bounds(int itemCount) override { return CSize(1, itemCount); }
    CSize GetItemSize(const CelIndex &index, const RasterComponent &raster, int itemIndex) override
    {
        const Loop &loop = raster.Loops[itemIndex];
        // This is the size of the item in our source bitmap. Along y it should be max, along x it should be the accum
        CSize size;
        for (const Cel &cel : loop.Cels)
        {
            size.cx += cel.size.cx;
            size.cy = max(size.cy, cel.size.cy);
        }
        return size;
    }
    CSize CalcBitmapSize(const CelIndex &index, const RasterComponent &raster) override
    {
        CSize size;
        for (int i = 0; i < raster.LoopCount(); i++)
        {
            CSize sizeLoop = GetItemSize(index, raster, i);
            size.cx = max(size.cx, sizeLoop.cx);
            size.cy += sizeLoop.cy;
        }
        return size;
    }
    CSize GetBitmapData(const CelIndex &index, const RasterComponent &raster, CBitmap &bitmap, uint8_t *dibBits) override
    {
        // We need to return the largest of our guys, e.g. "individual item size"
        CSize size;
        int stride = CX_ACTUAL(bitmap.GetBitmapDimension().cx);
        int y = 0;
        for (const Loop &loop : raster.Loops)
        {
            int maxCy = 0;
            uint8_t bgColor = loop.Cels[0].TransparentColor;
            for (const Cel &cel : loop.Cels)
            {
                maxCy = max(maxCy, (int)cel.size.cy);
            }

            // Set the background color, to fill in the gaps
            memset(dibBits + stride * y, bgColor, stride * maxCy);

            int x = 0;
            for (const Cel &cel : loop.Cels)
            {
                int yOffset = (maxCy - cel.size.cy) / 2;
                CopyBitmapData(cel, dibBits, x, y + yOffset, stride, true);
                x += cel.size.cx;
            }
            size.cx = max(size.cx, x);
            size.cy = max(size.cy, maxCy);
            y += maxCy;
        }
        return size;
    }
    CPoint IncrementSourcePos(CSize size) override { return CPoint(0, size.cy); }
    CPoint DimensionMultiplier() override  { return CPoint(0, 1); }
    CPoint CenteringMultiplier() override { return CPoint(0, 1); }
    void SetIndex(CelIndex &celIndex, int index) override { celIndex.loop = index; }
    bool IsLoops() override { return true; }
    int GetLogicalScale(int scale) { return appState->AspectRatioY(scale); }
    std::string GetCaption(const CelIndex &index) override
    {
        return "Loops";
    }
};

ViewCelListBox::ViewCelListBox() : _dibBits(nullptr), _scale(ScaleOne), _capture(false), _inDrag(false), _dragItemIndex(-1), _insertIndex(-1)
{
}

void ViewCelListBox::SetHorizontal(bool horizontal)
{
    if (horizontal)
    {
        _useThemeBackground = false;
        _dir = make_unique<LeftToRight>();
    }
    else
    {
        _useThemeBackground = true;
        _dir = make_unique<TopToBottom>();
    }
    _drawOffset.y = CaptionHeight;
}

BEGIN_MESSAGE_MAP(ViewCelListBox, CView)
    ON_WM_ERASEBKGND()
    ON_WM_SETCURSOR()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

BOOL ViewCelListBox::OnEraseBkgnd(CDC *pdc)
{
    return TRUE;
}

// REVIEW: This piece of code should be re-usable
void ViewCelListBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    __super::OnKeyDown(nChar, nRepCnt, nFlags);

    if (_GetDoc())
    {
        CPoint point(0, 0);
        switch (nChar)
        {
            case VK_RIGHT:
            {
                point.x++;
                break;
            }
            case VK_LEFT:
            {
                point.x--;
                break;
            }
            case VK_UP:
            {
                point.y--;
                break;
            }
            case VK_DOWN:
            {
                point.y++;
                break;
            }
            default:
            {
                __super::OnKeyDown(nChar, nRepCnt, nFlags);
            }
        }

        if ((point.x != 0) || (point.y != 0))
        {
            _GetDoc()->MoveSelectedCel(point);
        }
    }
}

void ViewCelListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
    __super::OnLButtonDown(nFlags, point);
    int index;
    if (_HitTest(point, false, index))
    {
        CNewRasterResourceDocument *pDoc = _GetDoc();
        if (pDoc)
        {
            CelIndex celIndex = pDoc->GetSelectedIndex();
            _dir->SetIndex(celIndex, index);
            pDoc->SetSelectedLoop(celIndex.loop);
            pDoc->SetSelectedCel(celIndex.cel);
        }

        _capture = true;
        SetCapture();
        _capturePoint = point;
        _dragItemIndex = index;
    }
}

void ViewCelListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
    __super::OnLButtonUp(nFlags, point);

    if (_capture)
    {
        if (_inDrag)
        {
            if ((_dragItemIndex != -1) && (_insertIndex != -1))
            {
                // We insert before this index.
                if (_GetDoc())
                {
                    bool isLoop = _dir->IsLoops();
                    CelIndex selectedCelIndex = _GetDoc()->GetSelectedIndex();
                    _GetDoc()->ApplyChanges<RasterComponent>(
                        [isLoop, selectedCelIndex, this](RasterComponent &raster)
                    {
                        if (isLoop)
                        {
                            return WrapRasterChange(MoveLoopFromTo(raster, _dragItemIndex, _insertIndex));
                        }
                        else
                        {
                            return WrapRasterChange(MoveCelFromTo(raster, selectedCelIndex.loop, _dragItemIndex, _insertIndex));
                        }
                    }
                    );
                }
            }
        }
        ReleaseCapture();
        _capture = false;
        _inDrag = false;
        _dragItemIndex = -1;
        _insertIndex = -1;
        _dragImage.reset(nullptr);
        Invalidate(FALSE);
    }
}

int ViewCelListBox::_ScaleY(int value) const
{
    return appState->AspectRatioY(value * _scale);
}

void ViewCelListBox::CreateDragImage()
{
    assert(_dragItemIndex != -1);
    if (_GetDoc())
    {
        CelIndex selectedCelIndex = _GetDoc()->GetSelectedIndex();
        const RasterComponent &raster = _GetDoc()->GetResource()->GetComponent<RasterComponent>();

        CDC *pDC = GetDC();
        if (pDC)
        {
            CDC dcMem, dcMem2;
            if (dcMem.CreateCompatibleDC(pDC) && dcMem2.CreateCompatibleDC(pDC))
            {
                _dragImage = std::make_unique<CBitmap>();

                CSize imageSize = _dir->GetItemSize(_GetDoc()->GetSelectedIndex(), raster, _dragItemIndex);

                imageSize.cx = imageSize.cx * max(_scale, ScaleOne) / ScaleOne;
                imageSize.cy = max(_ScaleY(imageSize.cy), _ScaleY(ScaleOne)) / ScaleOne;
                BOOL fResult = _dragImage->CreateCompatibleBitmap(pDC, imageSize.cx, imageSize.cy);
                _dragImage->SetBitmapDimension(imageSize.cx, imageSize.cy);
                HGDIOBJ hOld = dcMem.SelectObject(*_dragImage);
                HGDIOBJ hOld2 = dcMem2.SelectObject(*_sciBitmap);

                CPoint srcPos;
                for (int i = 0; i <= _dragItemIndex; i++)
                {
                    CSize itemSize = _dir->GetItemSize(_GetDoc()->GetSelectedIndex(), raster, i);
                    if (i == _dragItemIndex)
                    {
                        fResult = StretchBlt(dcMem, 0, 0, imageSize.cx, imageSize.cy,
                            dcMem2, srcPos.x, srcPos.y, itemSize.cx, itemSize.cy, SRCCOPY);
                        break;
                    }
                    srcPos += _dir->IncrementSourcePos(itemSize);
                }

                dcMem.SelectObject(hOld);
                dcMem2.SelectObject(hOld2);
            }

            ReleaseDC(pDC);
        }
    }
}

void ViewCelListBox::OnMouseMove(UINT nFlags, CPoint point)
{
    __super::OnMouseMove(nFlags, point);

    if (_capture)
    {
        if (!_inDrag)
        {
            if ((abs(point.x - _capturePoint.x) > MinDragDelta) ||
                (abs(point.y - _capturePoint.y) > MinDragDelta))
            {
                CreateDragImage();
                _inDrag = true;
            }
        }

        if (_inDrag)
        {
            _HitTest(point, true, _insertIndex);
            _currentDragPoint = point;
            RedrawWindow();
        }
    }
}

bool ViewCelListBox::_HitTest(CPoint point, bool calcInsertIndex, int &index)
{
    index = -1;
    CNewRasterResourceDocument *pDoc = _GetDoc();
    if (pDoc)
    {
        CelIndex selectedCelIndex = pDoc->GetSelectedIndex();
        const RasterComponent &raster = pDoc->GetResource()->GetComponent<RasterComponent>();
        int itemCount = _dir->ItemCount(selectedCelIndex, raster);

        if (calcInsertIndex)
        {
            // Must be near the item boundaries.
            int xLogical = (_dir->A(point) - _dir->A(_drawOffset));
            int scale = _dir->GetLogicalScale(_dir->A(_individualImageSize) * _scale);
            int xIndex = xLogical / (scale / ScaleOne);
            int xLogicalRecalc = xIndex * (scale / ScaleOne);
            int xLogicalRecalcNext = (xIndex + 1) * (scale / ScaleOne);
            bool close = false;
            close = (abs(xLogical - xLogicalRecalc) < InsertMargin);
            if (!close && (abs(xLogical - xLogicalRecalcNext) < InsertMargin))
            {
                xIndex++;
                close = true;
            }
            if (close && (xIndex >= 0) && (xIndex <= itemCount))
            {
                index = xIndex;
                return true;
            }
        }
        else
        {
            // Must be on the item itself.
            CSize boundMul = _dir->Bounds(itemCount);
            CRect rect(_drawOffset.x, _drawOffset.y, _drawOffset.x + boundMul.cx * _individualImageSize.cx * _scale / ScaleOne, _drawOffset.y + _ScaleY(boundMul.cy * _individualImageSize.cy) / ScaleOne);
            if (PtInRect(&rect, point))
            {
                int xIndex = (_dir->A(point) - _dir->A(_drawOffset)) / (_dir->GetLogicalScale(_dir->A(_individualImageSize) * _scale) / ScaleOne);
                index = xIndex;
                return true;
            }
        }
    }
    return false;
}

void ViewCelListBox::OnDraw(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);

    bool drewBackground = false;
    string caption;
    CNewRasterResourceDocument *pDoc = _GetDoc();
    if (pDoc)
    {
        // Each time we draw, we copy the cel data anew into our dibsection.
        // Then we StretchBlt the necessary areas to the screen.
        // For palettized images, GDI always uses nearest neighbor
        // interpolation, so we don't need to set StretchBltMode.
        _TransferToBitmap();

        CDC dcMem;
        if (dcMem.CreateCompatibleDC(pDC))
        {
            int saveDC = pDC->SaveDC();

            CRect rectDragClip;
            if (_dragImage)
            {
                CSize dragSize = _dragImage->GetBitmapDimension();
                dcMem.SelectObject(*_dragImage);
                // AlphaBlend doesn't handle palettized bitmaps correctly (0 is always transparent), and we'd also need
                // to double-buffer if we used it (to avoid flicker)
                //AlphaBlend(*pDC, _currentDragPoint.x, _currentDragPoint.y, dragSize.cx, dragSize.cy, dcMem, 0, 0, dragSize.cx, dragSize.cy, blendFunction);
                StretchBlt(*pDC, _currentDragPoint.x, _currentDragPoint.y, dragSize.cx, dragSize.cy, dcMem, 0, 0, dragSize.cx, dragSize.cy, SRCCOPY);

                rectDragClip = CRect(_currentDragPoint.x, _currentDragPoint.y, _currentDragPoint.x + dragSize.cx, _currentDragPoint.y + dragSize.cy);
                
                pDC->ExcludeClipRect(&rectDragClip);
            }

            dcMem.SelectObject(*_sciBitmap);

            CelIndex celIndex = pDoc->GetSelectedIndex();

            // This flickers, on drag - we'll need to double buffer.
            caption = _dir->GetCaption(celIndex);
            CRect rcCaption = rect;
            rcCaption.bottom = rcCaption.top + CaptionHeight;
            CRect rcCaptionText = rcCaption;
            rcCaptionText.left += 5;
            CExtPaintManager::PAINTGRIPPERDATA pgd(
                this,
                rcCaption,
                rcCaptionText,
                false,  // active
                true,   // floating
                false,  // horizontal
                false,  // sidebar  ??
                caption.c_str(),
                false
                );
            g_PaintManager->PaintGripper(*pDC, pgd);
            pDC->ExcludeClipRect(&rcCaption);

            const RasterComponent &raster = pDoc->GetResource()->GetComponent<RasterComponent>();
            const Loop &loop = raster.Loops[celIndex.loop];

            CPoint srcPos;
            CPoint itemIndices;

            CPoint centMult = _dir->CenteringMultiplier();

            std::vector<CRect> clipRects;
            int itemCount = _dir->ItemCount(celIndex, raster);
            for (int i = 0; i < itemCount; i++)
            {
                CSize itemSize = _dir->GetItemSize(celIndex, raster, i);
                CRect destRect(_drawOffset.x, _drawOffset.y, _drawOffset.x + itemSize.cx * _scale / ScaleOne, _drawOffset.y + _ScaleY(itemSize.cy) / ScaleOne);
                destRect.OffsetRect(itemIndices.x * _individualImageSize.cx * _scale / ScaleOne, _ScaleY(itemIndices.y * _individualImageSize.cy) / ScaleOne);
                destRect.OffsetRect(centMult.x * ((_individualImageSize.cx * _scale / ScaleOne) - (itemSize.cx * _scale / ScaleOne)) / 2, centMult.y * ((_ScaleY(_individualImageSize.cy) / ScaleOne) - (_ScaleY(itemSize.cy) / ScaleOne)) / 2);
                StretchBlt(*pDC, destRect.left, destRect.top, destRect.Width(), destRect.Height(),
                    dcMem, srcPos.x, srcPos.y, itemSize.cx, itemSize.cy, SRCCOPY);

                // Things get chunk in some cases if we apply the clip rects now. So just do it before we fill the background.
                clipRects.push_back(destRect);

                srcPos += _dir->IncrementSourcePos(itemSize);
                itemIndices += _dir->DimensionMultiplier();
            }

            for (CRect &clipRect : clipRects)
            {
                pDC->ExcludeClipRect(&clipRect);
            }

            COLORREF background;
            if (_useThemeBackground)
            {
                background = g_PaintManager->GetColor(COLOR_BTNFACE);
            }
            else
            {
                background = pDoc->SCIColorToCOLORREF(loop.Cels[0].TransparentColor);
            }
            pDC->FillSolidRect(&rect, background);
            drewBackground = true;

            pDC->RestoreDC(saveDC);
            if (!rectDragClip.IsRectEmpty())
            {
                pDC->ExcludeClipRect(&rectDragClip);
            }

            // Selection rect
            CBrush solidBrush(RGB(254, 170, 70));
            CRect rectSelection(_drawOffset.x, _drawOffset.y, _drawOffset.x + _individualImageSize.cx * _scale / ScaleOne, _drawOffset.y + _ScaleY(_individualImageSize.cy) / ScaleOne);
            int offsetX = _individualImageSize.cx * _scale * _dir->ItemIndex(celIndex) * _dir->DimensionMultiplier().x / ScaleOne;
            int offsetY = _ScaleY(_individualImageSize.cy) * _dir->ItemIndex(celIndex) * _dir->DimensionMultiplier().y / ScaleOne;
            rectSelection.OffsetRect(offsetX, offsetY);
            pDC->FrameRect(&rectSelection, &solidBrush);
            rectSelection.InflateRect(-1, -1);
            pDC->FrameRect(&rectSelection, &solidBrush);

            // Insert rect
            if (_insertIndex != -1)
            {
                CRect insertRect(_drawOffset.x, _drawOffset.y, _drawOffset.x + 1, _drawOffset.y + 1);
                CPoint dimMul = _dir->DimensionMultiplier();
                insertRect.right += (_individualImageSize.cx * _scale / ScaleOne) * dimMul.y;
                insertRect.bottom += (_ScaleY(_individualImageSize.cy) / ScaleOne) * dimMul.x;

                insertRect.OffsetRect(dimMul.x* _insertIndex * _individualImageSize.cx * _scale / ScaleOne, dimMul.y * _ScaleY(_insertIndex * _individualImageSize.cy) / ScaleOne);

                HGDIOBJ hOldBrush = pDC->SelectObject(GetStockObject(BLACK_BRUSH));
                pDC->Rectangle(insertRect);
                insertRect.InflateRect(_dir->DimensionMultiplier().x, _dir->DimensionMultiplier().y);
                pDC->SelectObject(GetStockObject(WHITE_BRUSH));
                pDC->Rectangle(insertRect);
                pDC->SelectObject(hOldBrush);
            }

            pDC->RestoreDC(saveDC);
        }
    }
    if (!drewBackground)
    {
        pDC->FillSolidRect(&rect, RGB(255, 128, 128));
    }
}

void ViewCelListBox::_TransferToBitmap()
{
    _EnsureSCIBitmap(_CalcRequiredBitmapSize());
    _individualImageSize = CSize(1, 1);
    CNewRasterResourceDocument *pDoc = _GetDoc();
    CSize numberOfGuys(1, 1);
    if (pDoc)
    {
        CelIndex celIndex = pDoc->GetSelectedIndex();
        const RasterComponent &raster = pDoc->GetResource()->GetComponent<RasterComponent>();
        _individualImageSize = _dir->GetBitmapData(celIndex, raster, *_sciBitmap, _dibBits);
        numberOfGuys = _dir->Bounds(_dir->ItemCount(celIndex, raster));
    }


    // Draw as large as we can while still fitting in the client bounds.
    CRect clientRect;
    GetClientRect(clientRect);
    clientRect.top += _drawOffset.y;
    clientRect.left += _drawOffset.x;
    CSize sizeBitmap = _sciBitmap->GetBitmapDimension();
    _scale = ScaleOne * clientRect.Width() / (numberOfGuys.cx * _individualImageSize.cx);
    _scale = min(_scale, ScaleOne * clientRect.Height() / appState->AspectRatioY(numberOfGuys.cy * _individualImageSize.cy));
    _scale = max(1, _scale);
}

CSize ViewCelListBox::_CalcRequiredBitmapSize()
{
    CSize size;
    CNewRasterResourceDocument *pDoc = _GetDoc();
    if (pDoc)
    {
        CelIndex celIndex = pDoc->GetSelectedIndex();
        const RasterComponent &raster = pDoc->GetResource()->GetComponent<RasterComponent>();

        size = _dir->CalcBitmapSize(celIndex, raster);
    }
    return size;
}

BOOL ViewCelListBox::OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message)
{
    SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
    return TRUE;
}

void ViewCelListBox::_EnsureSCIBitmap(CSize size, bool clear)
{
    if (clear)
    {
        _sciBitmap.reset(nullptr);
        _dibBits = nullptr;
    }
    if (_sciBitmap)
    {
        CSize sizeBitmap = _sciBitmap->GetBitmapDimension();
        if ((sizeBitmap.cx < size.cx) || (sizeBitmap.cy < size.cy))
        {
            _sciBitmap.reset(nullptr);
            _dibBits = nullptr;
        }
    }
    if (!_sciBitmap)
    {
        const PaletteComponent *palette = _GetDoc()->GetCurrentPaletteComponent();
        SCIBitmapInfo bmi(size.cx, -size.cy, palette ? palette->Colors : nullptr, palette ? ARRAYSIZE(palette->Colors) : 0);
        _sciBitmap = make_unique<CBitmap>();
        _sciBitmap->Attach(CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&_dibBits), nullptr, 0));
        _sciBitmap->SetBitmapDimension(size.cx, size.cy);
    }
}

void ViewCelListBox::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
    RasterChangeHint hint = GetHint<RasterChangeHint>(pHint);
    if (_GetDoc())
    {
        hint &= ~(RasterChangeHint::Color | RasterChangeHint::PenStyle);    // Remove stuff we don't care about
        if (IsFlagSet(hint, RasterChangeHint::PaletteChoice))
        {
            _EnsureSCIBitmap(_CalcRequiredBitmapSize(), true);
        }
        if (hint != RasterChangeHint::None)
        {
            Invalidate(FALSE);
        }
    }
}

CNewRasterResourceDocument *ViewCelListBox::_GetDoc()
{
    return static_cast<CNewRasterResourceDocument*>(GetDocument());
}
