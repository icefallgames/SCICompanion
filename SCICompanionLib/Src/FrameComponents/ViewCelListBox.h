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

class CNewRasterResourceDocument;
union CelIndex;
struct RasterComponent;

struct DirectionalityBase
{
    virtual int A(const CSize &size) = 0;
    virtual int B(const CSize &size) = 0;
    virtual int A(const CPoint &point) = 0;
    virtual int B(const CPoint &point) = 0;
    virtual int ItemCount(const CelIndex &index, const RasterComponent &raster) = 0;
    virtual int ItemIndex(const CelIndex &index) = 0;
    virtual CSize Bounds(int itemCount) = 0;
    virtual CSize GetItemSize(const CelIndex &index, const RasterComponent &raster, int itemIndex) = 0;
    virtual CSize CalcBitmapSize(const CelIndex &index, const RasterComponent &raster) = 0;
    virtual CSize GetBitmapData(const CelIndex &index, const RasterComponent &raster, CBitmap &bitmap, uint8_t *dibBits) = 0;
    virtual CPoint IncrementSourcePos(CSize size) = 0;
    virtual CPoint DimensionMultiplier() = 0;
    virtual CPoint CenteringMultiplier() = 0;
    virtual void SetIndex(CelIndex &celIndex, int index) = 0;
    virtual bool IsLoops() = 0;
    virtual int GetLogicalScale(int scale) = 0;
    virtual std::string GetCaption(const CelIndex &index) = 0;
    virtual ~DirectionalityBase() {}
};

class ViewCelListBox : public CView
{
    DECLARE_DYNCREATE(ViewCelListBox)

public:
    ViewCelListBox();
    void SetHorizontal(bool horizontal);
    void OnDraw(CDC* pDC) override;
    void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint) override;
    BOOL OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message);

protected:
    DECLARE_MESSAGE_MAP()

    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point);
    void OnMouseMove(UINT nFlags, CPoint point);
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    BOOL OnEraseBkgnd(CDC *pdc);

    // temp
    void OnTimer(UINT_PTR nIDEvent);

private:

    bool  _HitTest(CPoint point, bool calcInsertIndex, int &index);
    void _EnsureSCIBitmap(CSize size, bool clear = false);
    CSize _CalcRequiredBitmapSize();
    void _TransferToBitmap();
    void CreateDragImage();
    CNewRasterResourceDocument *_GetDoc();
    int _ScaleY(int value) const;

    std::unique_ptr<DirectionalityBase> _dir;
    std::unique_ptr<CBitmap> _sciBitmap;
    CSize _individualImageSize;
    // If the scale is actually scale / 4, then we don't need
    // to supply scrollbars (makes everything easier)
    int _scale;
    CPoint _drawOffset;
    uint8_t *_dibBits;

    bool _capture;
    bool _inDrag;
    CPoint _capturePoint;
    CPoint _currentDragPoint;
    std::unique_ptr<CBitmap> _dragImage;
    int _dragItemIndex; // Set on mouse down, not drag start.
    int _insertIndex;

    bool _useThemeBackground;
};