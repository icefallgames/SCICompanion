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
#include "AppState.h"
#include "RasterView.h"
#include "DrawHelper.h"
#include "ResourceDocument.h"
#include "RasterOperations.h"
#include "Components.h"
#include "NewRasterResourceDocument.h"
#include "ResourceEntityDocument.h"
#include "CObjectWrap.h"
#include "PaletteOperations.h"
#include "PaletteEditorDialog.h"
#include "ImageUtil.h"
#include "ConvertFromToPaletteDialog.h"
#include "ColorShifterDialog.h"
#include "PicCommands.h"
#include "BayerMatrix.h"
#include "BitmapToVGADialog.h"
#include "ImageUtil.h"
#include "ClipboardUtil.h"
#include "CustomMessageBox.h"
#include "OnionSkinSettingsDialog.h"
#include "RotateArbitraryDialog.h"

using namespace std;

// Thickness of the sizers around the image:
#define SIZER_SIZE 6
#define DIAGONALSPAN 12 // How many pixels from the end does the "diagonal" start?

#define COLOR_WARNINGBACKGROUND RGB(192, 0, 0)
#define COLOR_NORMALBACKGROUND RGB(0, 0, 128)

CExtBitmap g_AffectAllBitmap;


//
// Makes a rect bounded by the two points, no matter where the points are in relation
// to each other.
//
void _MakeGoodRect(CRect &rect, CPoint &ptStart, CPoint &ptEnd)
{
    rect = CRect(min(ptStart.x, ptEnd.x), min(ptStart.y, ptEnd.y), max(ptStart.x, ptEnd.x), max(ptStart.y, ptEnd.y));
}

void _GetSizerRectVert(const CSize &sizeNewZoom, CRect &rect)
{
    rect = CRect(0, sizeNewZoom.cy, sizeNewZoom.cx + SIZER_SIZE - DIAGONALSPAN, sizeNewZoom.cy + SIZER_SIZE);
}

void _GetSizerRectHorz(const CSize &sizeNewZoom, CRect &rect)
{
    rect = CRect(sizeNewZoom.cx, 0, sizeNewZoom.cx + SIZER_SIZE, sizeNewZoom.cy + SIZER_SIZE - DIAGONALSPAN);
}

void _GetSizerRectDiag1(const CSize &sizeNewZoom, CRect &rect)
{
    rect = CRect(sizeNewZoom.cx + SIZER_SIZE - DIAGONALSPAN, sizeNewZoom.cy,
                   sizeNewZoom.cx + SIZER_SIZE,
                   sizeNewZoom.cy + SIZER_SIZE);
}

void _GetSizerRectDiag2(const CSize &sizeNewZoom, CRect &rect)
{
    rect = CRect(sizeNewZoom.cx, sizeNewZoom.cy + SIZER_SIZE - DIAGONALSPAN,
                   sizeNewZoom.cx + SIZER_SIZE,
                   sizeNewZoom.cy);
}

CRasterView::SelectionManager::SelectionManager()
{
    _fSelection = false;
}

CRasterView::SelectionManager::~SelectionManager()
{
    _GenerateSelectionBits(0, CSize(0,0));
}


bool CRasterView::SelectionManager::HasSelection()
{
    return _fSelection;
}

//
// Our drawing tools, and GDI, operate with the origin at the top left.
// But when manipulating the bits directly, the origin is at the bottom left.
//
CRect CRasterView::SelectionManager::_GetBottomOriginRect(CSize sizeCel, const RECT *prc)
{
    CRect rectBL(prc);
    rectBL.top = sizeCel.cy - prc->bottom;
    rectBL.bottom = sizeCel.cy - prc->top;

    // Clamp the rect to the cel size.
    rectBL.IntersectRect(&rectBL, CRect(CPoint(0, 0), sizeCel));
    return rectBL;
}

void CRasterView::SelectionManager::_GenerateSelectionBits(int cCels, CSize size)
{
    // Remove old
    _iMain = -1;
    _selectionBits.clear();
    _fSelection = false;

    // Create new
    _selectionBits.resize(cCels);
    _sizeSelectionBits = size;
    if (cCels)
    {
        for (size_t i = 0; i < _selectionBits.size(); i++)
        {
            // Note we don't use CX_ACTUAL here!
			_selectionBits[i].reset(new uint8_t[size.cx * size.cy]);
        }
    }
}

uint8_t *CRasterView::SelectionManager::GetMainSelection(CSize &sizeOut)
{
    uint8_t *pData = nullptr;
    if (_iMain != -1)
    {
        pData = _selectionBits[_iMain].get();
        sizeOut = _sizeSelectionBits;
    }
    return pData;
}

const std::vector<std::unique_ptr<uint8_t[]>> &CRasterView::SelectionManager::GetSelectionBits(CSize &sizeOut)
{
    sizeOut = _sizeSelectionBits;
    return _selectionBits;
}

//
// Lifts the selection from each of the cels
// If fClear is TRUE, it replaces the lifted selection with the transparent colour.
// If fGrabBits is TRUE, it stashes the selection bits off.
//
void CRasterView::SelectionManager::LiftSelection(CRect rectSelection, int iMain, int cCels, std::vector<CelData> &celData, BOOL fClear, BOOL fGrabBits) 
{
    assert(!rectSelection.IsRectEmpty());
    // Allocate a temporary buffer into which we copy the bits.
    size_t cb = rectSelection.Width() * rectSelection.Height(); // Off by 1?
    _GenerateSelectionBits(cCels, CSize(rectSelection.Width(), rectSelection.Height()));
    if (fGrabBits)
    {
        _iMain = iMain;
        for (int i = 0; i < cCels; i++)
        {
            uint8_t bTransparent = celData[i].GetTransparentColor();
            DWORD iOffsetTemp = 0;
            // Copy the data out to a temp buffer, and fill the empty space with the
            // transparent color.

            celData[i].CalcOffset(celData[iMain].GetPlacement(), celData[iMain].GetSize(), rectSelection.TopLeft(), rectSelection.BottomRight());
            CRect rcAdjustedSelection(celData[i]._point1, celData[i]._point2);
            CRect rectVertInvert = _GetBottomOriginRect(SizeToCSize(celData[i].GetSize()), &rcAdjustedSelection);
            // Set these after we've "clamped" the rect, since otherwise we could run out of bounds.
            int xLeft = rectVertInvert.left;
            int cxLine = rectVertInvert.Width();
            // This rect could be "clamped", e.g. smaller than rectSelection.  So we should fill
            // _selectionBits[i] with the transparent color first (so we have no invalid data).
            FillMemory(_selectionBits[i].get(), cb, bTransparent);
            for (int y = (rectVertInvert.bottom - 1); y >= rectVertInvert.top; y--)
            {
                assert(iOffsetTemp < cb);
                uint8_t *pSrc = celData[i].GetDataPtr() + xLeft + CX_ACTUAL(celData[i].GetSize().cx) * y;
                if (fGrabBits)
                {
                    memcpy(_selectionBits[i].get() + iOffsetTemp, pSrc, cxLine);
                    iOffsetTemp += cxLine;
                }

                if (fClear)
                {
                    // Clean out the spot in the view
                    memset(pSrc, bTransparent, cxLine);
                }
            }
        }
        _fSelection = true;
    }
}

//
// This converts a CX_ACTUAL(cx) * cy bitmap to a cx * cy selection-style bitmap,
// and flips it vertically.
//
void _ConvertDrawnDIBBitsToSelectionBits(const uint8_t *pBits, int cxSource, CSize sizeDest, uint8_t *pBitsOut)
{
    for (int y = 0; y < sizeDest.cy; y++)
    {
        memcpy(pBitsOut + y * sizeDest.cx, pBits + (sizeDest.cy - 1 - y) * CX_ACTUAL(cxSource), sizeDest.cx);
    }
}

// This does the opposite of the above
void _ConvertSelectionBitsToCelData(const uint8_t *pBits, CSize size, uint8_t *pBitsOut)
{
    for (int y = 0; y < size.cy; y++)
    {
        memcpy(pBitsOut + y * CX_ACTUAL(size.cx), pBits + (size.cy - 1 - y) * size.cx, size.cx);
    }
}

CRect CRasterView::SelectionManager::PasteCel(const std::vector<const Cel*> &celsPaste, int cCels, CSize sizeMain)
{
    assert(!celsPaste.empty());
    CRect rect;
    // Get rid of any selection.
    ClearSelection();
    _sizeSelectionBits = CSize(min(sizeMain.cx, celsPaste.front()->size.cx), min(sizeMain.cy, celsPaste.front()->size.cy));
    _GenerateSelectionBits(cCels, _sizeSelectionBits);
    rect = CRect(0, 0, _sizeSelectionBits.cx, _sizeSelectionBits.cy);
    // Apply our new selection;
    for (int i = 0; i < cCels; i++)
    {
        size_t index = min((size_t)i, celsPaste.size() - 1);
        _ConvertDrawnDIBBitsToSelectionBits(&celsPaste[index]->Data[0], celsPaste[index]->size.cx, _sizeSelectionBits, _selectionBits[i].get());
    }
    _fSelection = true;
    return rect;
}

void CRasterView::SelectionManager::DrawSelection(CRect rectSelection, int iIndex, BOOL fTransparent, CelData &celData)
{
    // rectSelection has already been adjusted for iIndex, so no need to do a CalcOffset

    // Clamp iIndex to our array size.
    iIndex = max(0, iIndex);
    iIndex = min((int)_selectionBits.size() - 1, iIndex);
    // It is legit to call with a bad index, since we don't clear selection when a different
    // number of cels is chosen.

    CSize sizeData = SizeToCSize(celData.GetSize());
    // Intersect the view and selection rects.
    CRect rectView(CPoint(0, 0), sizeData);
    CRect rectIntersect;
    if (rectIntersect.IntersectRect(&rectView, &rectSelection))
    {
        // Write start from this column in the view:
        int xLeftView = rectIntersect.left;
        // Read starting from this column in the selection bits:
        int xLeftSrc = rectIntersect.left - rectSelection.left;
        // Read/Write this many columns:
        int cxLine = rectIntersect.Width();
        // Similarly for the scanlines:
        int yTopView = rectIntersect.top;
        int yTopSrc = rectIntersect.top - rectSelection.top;
        int cyRows = rectIntersect.Height();

        uint8_t bTransparent = celData.GetTransparentColor();
        for (int dy = 0; dy < cyRows; dy++)
        {
            // Copy from this point:
            uint8_t *pSrc = _selectionBits[iIndex].get() + xLeftSrc + (dy + yTopSrc) * rectSelection.Width();
            // To this point:
            uint8_t *pDest = celData.GetDataPtr() + xLeftView + (sizeData.cy - 1 - (dy + yTopView)) * CX_ACTUAL(sizeData.cx);
            // assert we aren't going to have a BO.
            // No longer works after doing multicel stuff

            if (fTransparent)
            {
                // Only copy non-transparent bits.
                for (int i = 0; i < cxLine; i++)
                {
                    if (*pSrc != bTransparent)
                    {
                        *pDest = *pSrc;
                    }
                    pDest++; pSrc++;
                }
            }
            else
            {
                // easy
                memcpy(pDest, pSrc, cxLine);
            }
        }
    }
}

// CRasterView

IMPLEMENT_DYNCREATE(CRasterView, CView)

CRasterView::CRasterView()
{
    if (g_AffectAllBitmap.IsEmpty())
    {
        g_AffectAllBitmap.LoadBMP_Resource(MAKEINTRESOURCE(IDB_BITMAPAFFECTALL));
        assert(!g_AffectAllBitmap.IsEmpty());
    }

    _iMainIndex = -1;

    _colorEditor = COLOR_NORMALBACKGROUND;

    _cWorkingCels = 0;

    _bRandomNR = 0;

    // Scrolling:
    _yOrigin = 0;
    _xOrigin = 0;

    _dwIndex = 0;

    _pbitmapDoubleBuf = NULL;
    _fDoubleBuf = false;

    // We don't know our real size until we have a cel.
    _cxViewZoom = 0;
    _cyViewZoom = 0;
    _sizeView.cx = 0;
    _sizeView.cy = 0;

    // This will get updated in OnInitialUpdate
    // Actually no, we need to wait until we have the resource ready, which comes after.
    _iZoomFactor = 1;
    _obtainedZoom = false;

    _currentTool = Off;

    _fCapturing = false;
    _fAux = false;

    _fSizerCapturing = false;
    _onSizerType = None;

    _fPreviewPen = false;

    _fDraggingSelection = false;
    _fSelectionLifted = false;
    _rectSelection = CRect(0, 0, 0, 0);
    _iRubberBandFrameNumber = 0;

    _sizeScratch1 = size16(0, 0);

    _fHotSpot = false;
    _fDithered = false;
    _ditherStrength = 8;
    _InitPatternBrush();
}

int CRasterView::_GetViewScreenHeight() const
{
    int height = _cyViewZoom;
    if (appState->_fUseOriginalAspectRatioCached)
    {
        height = appState->AspectRatioY(height);
    }
    return height;
}

int CRasterView::_GetDragScreenHeight() const
{
    int height = _sizeNew.cy * _iZoomFactor;
    if (appState->_fUseOriginalAspectRatioCached)
    {
        height = appState->AspectRatioY(height);
    }
    return height;
}

void CRasterView::_CleanUpViewData()
{
    _celData.clear();
    _iMainIndex = -1;
}


CRasterView::~CRasterView()
{
    _DestroyDoubleBuffer();
    _CleanUpViewData();
}

BEGIN_MESSAGE_MAP(CRasterView, CScrollingThing<CView>)
    ON_COMMAND_EX(ID_PENTOOL, OnDrawCommand)
    ON_COMMAND_EX(ID_REPLACE, OnDrawCommand)
    ON_COMMAND_EX(ID_ZOOM, OnDrawCommand)
    ON_COMMAND_EX(ID_LINE, OnDrawCommand)
    ON_COMMAND_EX(ID_FILL, OnDrawCommand)
    ON_COMMAND_EX(ID_EYEDROPPER, OnDrawCommand)
    ON_COMMAND_EX(ID_SETTRANSPARENCY, OnDrawCommand)
    ON_COMMAND_EX(ID_DRAWRECT, OnDrawCommand)
    ON_COMMAND_EX(ID_DRAWRECTSOLID, OnDrawCommand)
    ON_COMMAND_EX(ID_DRAWOVAL, OnDrawCommand)
    ON_COMMAND_EX(ID_DRAWOVALSOLID, OnDrawCommand)
    ON_COMMAND_EX(ID_DRAWROUNDEDRECT, OnDrawCommand)
    ON_COMMAND_EX(ID_DRAWROUNDEDRECTSOLID, OnDrawCommand)
    ON_COMMAND_EX(ID_DRAWOFF, OnDrawCommand)
    ON_COMMAND_EX(ID_SELECT, OnDrawCommand)
    ON_COMMAND_EX(ID_SETPLACEMENT, OnDrawCommand)
    ON_COMMAND(ID_EDIT_COPY, OnCopyPic)
    ON_COMMAND(ID_EDIT_PASTE, OnPaste)
    ON_COMMAND(ID_EDIT_PASTETRANSPARENT, OnPasteTransparent)
    ON_COMMAND(ID_VIEW_PASTESPECIAL, OnPasteSpecial)
    ON_COMMAND(ID_EDIT_CUT, OnCut)
    ON_COMMAND(ID_EDIT_DELETE, OnDelete)
    ON_COMMAND(ID_EDIT_SELECT_ALL, OnSelectAll)
    ON_COMMAND(ID_DITHER, OnDither)
    ON_COMMAND(ID_FLIPHORZ, OnFlipHorz)
    ON_COMMAND(ID_FLIPVERT, OnFlipVert)
    ON_COMMAND(ID_SHIFTPIXELS_LEFT, OnShiftLeft)
    ON_COMMAND(ID_SHIFTPIXELS_RIGHT, OnShiftRight)
    ON_COMMAND(ID_SHIFTPIXELS_UP, OnShiftUp)
    ON_COMMAND(ID_SHIFTPIXELS_DOWN, OnShiftDown)
    ON_COMMAND(ID_INVERT, OnInvert)
    ON_COMMAND(ID_GREYSCALE, OnGreyScale)
    ON_COMMAND(ID_VIEW_ZOOMIN, _OnZoomLClick)
    ON_COMMAND(ID_VIEW_ZOOMOUT, _OnZoomRClick)
    ON_COMMAND(ID_VIEW_EDITPALETTE, EditVGAPalette)
    ON_COMMAND(ID_VIEW_REMOVEEMBEDDEDPALETTE, RemoveVGAPalette)
    ON_COMMAND(ID_VIEW_REMAPPALETTE, RemapPalette)
    ON_COMMAND(ID_VIEW_SHIFTCOLORS, ShiftColors)
    ON_COMMAND(ID_VIEW_LEFTONIONSKIN, ToggleLeftOnion)
    ON_COMMAND(ID_VIEW_RIGHTONIONSKIN, ToggleRightOnion)
    ON_COMMAND(ID_VIEW_ONIONSKINSETTINGS, OnOnionSkinSettings)
    ON_COMMAND(ID_ROTATE_CW, OnRotate90CW)
    ON_COMMAND(ID_ROTATE_CCW, OnRotate90CCW)
    ON_COMMAND(ID_ROTATE_180, OnRotate180)
    ON_COMMAND(ID_ROTATE_ARBITRARY, OnRotateArbitrary)
    ON_WM_RBUTTONDOWN()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_RBUTTONDBLCLK()
    ON_WM_MOUSEMOVE()
    ON_WM_KEYDOWN()
    ON_WM_KEYUP()
    ON_WM_ERASEBKGND()
    ON_WM_CREATE()
    ON_WM_TIMER()
    ON_WM_KILLFOCUS()
#ifdef FORCE_REDRAW
	ON_WM_SETFOCUS()
#endif
    ON_WM_SETCURSOR()
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_COORDS, OnIndicatorCoords)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_RASTERSIZE, OnIndicatorRasterSize)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_PIXELCOLOR, OnIndicatorPixelColor)
    ON_UPDATE_COMMAND_UI(ID_PENTOOL, OnUpdateAllViewCommands)
    ON_UPDATE_COMMAND_UI(ID_REPLACE, OnUpdateAllViewCommands)
    ON_UPDATE_COMMAND_UI(ID_LINE, OnUpdateAllViewCommands)
    ON_UPDATE_COMMAND_UI(ID_FILL, OnUpdateAllViewCommands)
    ON_UPDATE_COMMAND_UI(ID_EYEDROPPER, OnUpdateAllViewCommands)
    ON_UPDATE_COMMAND_UI(ID_SETTRANSPARENCY, OnUpdateAllViewCommands)
    ON_UPDATE_COMMAND_UI(ID_DRAWOFF, OnUpdateAllViewCommands)
    ON_UPDATE_COMMAND_UI(ID_ZOOM, OnUpdateAllViewCommands)
    ON_UPDATE_COMMAND_UI(ID_DRAWRECT, OnUpdateAllViewCommands)
    ON_UPDATE_COMMAND_UI(ID_DRAWRECTSOLID, OnUpdateAllViewCommands)
    ON_UPDATE_COMMAND_UI(ID_DRAWOVAL, OnUpdateAllViewCommands)
    ON_UPDATE_COMMAND_UI(ID_DRAWOVALSOLID, OnUpdateAllViewCommands)
    ON_UPDATE_COMMAND_UI(ID_SELECT, OnUpdateAllViewCommands)
    ON_UPDATE_COMMAND_UI(ID_SETPLACEMENT, OnUpdateAllViewCommands)
    ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateSelectAll)
    ON_UPDATE_COMMAND_UI(ID_DITHER, OnUpdateDither)
    ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateDelete)
    ON_UPDATE_COMMAND_UI(ID_INVERT, OnUpdateEGAOnly)
    ON_UPDATE_COMMAND_UI(ID_GREYSCALE, OnUpdateEGAOnly)
    ON_UPDATE_COMMAND_UI(ID_VIEW_EDITPALETTE, OnUpdateIsVGA)
    ON_UPDATE_COMMAND_UI(ID_VIEW_REMOVEEMBEDDEDPALETTE, OnUpdateHasVGAPalette)
    ON_UPDATE_COMMAND_UI(ID_VIEW_REMAPPALETTE, OnUpdateIsVGA)
    ON_UPDATE_COMMAND_UI(ID_VIEW_SHIFTCOLORS, OnUpdateAlwaysOn)
    ON_UPDATE_COMMAND_UI(ID_VIEW_LEFTONIONSKIN, OnUpdateLeftOnion)
    ON_UPDATE_COMMAND_UI(ID_VIEW_RIGHTONIONSKIN, OnUpdateRightOnion)
END_MESSAGE_MAP()


int CRasterView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Remove any cursor associated with this window class, so we can change the cursor as we
    // change tools
    SetClassLongPtr(m_hWnd, GCL_HCURSOR, NULL);

    return 0;
}

void CRasterView::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == RubberBandTimer)
    {
        _iRubberBandFrameNumber++;
        if (_rectSelection.IsRectEmpty())
        {
            _KillRubberBandTimer();
        }
        _InvalidateViewArea();
    }
    else if (nIDEvent == HotSpotTimer)
    {
        _fHotSpot = !_fHotSpot;
        _InvalidateViewArea();
    }
}


void CRasterView::_LockDoc(bool fLock)
{
    CNewRasterResourceDocument *pDoc = GetDoc();
    if (pDoc)
    {
        pDoc->LockResource(fLock);
    }
}

const RasterComponent *CRasterView::_GetRaster() const
{
    const RasterComponent *raster = nullptr;
    CResourceDocument *pDoc = static_cast<CResourceDocument*>(GetDocument());
    if (pDoc)
    {
        const ResourceEntity *pResource = GetDoc()->GetResource();
        if (pResource)
        {
            raster = &pResource->GetComponent<RasterComponent>();
        }
    }
    return raster;
}

const Cel *CRasterView::_GetSelectedCel() const
{
    return _GetCel(_dwIndex);
}
const Cel *CRasterView::_GetPreviousCel(CelIndex &indexOut) const
{
    return _GetAdjacentCel(indexOut, -1);
}
const Cel *CRasterView::_GetNextCel(CelIndex &indexOut) const
{
    return _GetAdjacentCel(indexOut, 1);
}
const Cel *CRasterView::_GetAdjacentCel(CelIndex &indexOut, int offset) const
{
    const Cel *cel = nullptr;
    const RasterComponent *raster = _GetRaster();
    if (raster)
    {
        int nCel = _dwIndex.cel;
        int nLoop = _dwIndex.loop;
        nCel += offset;
        raster->ValidateCelIndex(nLoop, nCel, !!appState->_onionWrap);
        indexOut = CelIndex(nLoop, nCel);
        // If the index didn't change, it means we weren't wrapping  and were at the end (or there was only one cel).
        // In that case, it doesn't count as adjacent. So return null;
        if (indexOut.index != _dwIndex.index)
        {
            cel = &raster->GetCel(indexOut);
        }
    }
    return cel;
}
const Cel *CRasterView::_GetCel(CelIndex index) const
{
    const Cel *cel = nullptr;
    const RasterComponent *raster = _GetRaster();
    if (raster)
    {
        cel = &raster->GetCel(index);
    }
    return cel;
}

void CRasterView::_GrabSourceData()
{
    if (!_fPreviewPen)
    {
        const RasterComponent *raster = _GetRaster();
        if (raster)
        {
            for (int i = 0; i < _cWorkingCels; i++)
            {
                // Get a copy from the source, to start with.
                CelData &celData = _celData[i];
                CopyBitmapData(*raster, CelIndex(celData.Loop(), celData.Cel()), celData.GetDataPtr(), celData.GetSize());
            }
        }
    }
}

void CRasterView::_CommitSourceData()
{
    if (!_fPreviewPen)
    {
        ResourceEntityDocument *pDoc = static_cast<ResourceEntityDocument*>(GetDocument());
        if (pDoc)
        {
            // And put it back into the source.
            std::vector<uint8_t*> rgpData(_cWorkingCels);
            for (int i = 0; i < _cWorkingCels; i++)
            {
                rgpData[i] = _celData[i].GetDataPtr();
            }

            pDoc->ApplyChanges<RasterComponent>(
                [&](RasterComponent &raster)
            {
                return WrapRasterChange(SetGroupBitmapData(raster, _cWorkingCels, _rgdwGroups, &rgpData[0]));
            }
            );
        }
    }
}

//
// Invalidate just the view area (not the whole background)
// Useful if we know that size didn't change.
//
void CRasterView::_InvalidateViewArea()
{
    CRect rect(0, 0, _cxViewZoom + SIZER_SIZE, _GetViewScreenHeight() + SIZER_SIZE);
    InvalidateRect(&rect, FALSE);
}

void CRasterView::_EnsureScratchBuffer1(size16 size)
{
    if (size != _sizeScratch1)
    {
        _sizeScratch1 = size;
        _pBitsScratch1 = std::make_unique<uint8_t[]>(PaddedSize(_sizeScratch1));
    }
}


void CRasterView::_RefreshViewData()
{
    _CleanUpViewData();

    if (GetDoc())
    {

        if (!_obtainedZoom)
        {
            _iZoomFactor = GetDoc()->GetDefaultZoom();
            _obtainedZoom = true;
        }

        _dwIndex = GetDoc()->GetSelectedIndex();
        _SyncColor(GetDoc());

        const Cel *activeCel = _GetSelectedCel();
        if (activeCel)
        {
            size16 size = activeCel->size;
            bool fSizedChanged = !!(size != _sizeView);

            assert(!_fSizerCapturing);
            _sizeNew = size;
            _sizeView = size;
            _cxViewZoom = _sizeView.cx * _iZoomFactor;
            _cyViewZoom = _sizeView.cy * _iZoomFactor;

            if (fSizedChanged)
            {
                // If size changed, we'd better redraw all, and invalidate our scrolling
                _InvalidateScrollStuff();
                Invalidate(FALSE);
            }
            else
            {
                // If no size change, then just invalidate the view area
                _InvalidateViewArea();
            }

            _DestroyDoubleBuffer();

            // Do the ugly work of allocating our buffers
            int cWorkingCelsOld = _cWorkingCels;
            _cWorkingCels = GetDoc()->GetSelectedGroup(_rgdwGroups, ARRAYSIZE(_rgdwGroups));
            if (_cWorkingCels)
            {
                _celData.resize(_cWorkingCels);
                for (int i = 0; i < _cWorkingCels; i++)
                {
                    CelIndex dwIndexTemp = _rgdwGroups[i];
                    const Cel *cel = _GetCel(dwIndexTemp);
                    _celData[i].SetSize(dwIndexTemp, cel->placement, cel->size, cel->TransparentColor);

                    if (_rgdwGroups[i].index == _dwIndex.index)
                    {
                        _iMainIndex = i;
                    }
                }
            }
        }
    }
}

void CRasterView::_SyncColor(CNewRasterResourceDocument *pDoc)
{
    _color = pDoc->GetViewColor();
    _alternateColor = pDoc->GetAlternateViewColor();
    const ResourceEntity *pResource = pDoc->GetResource();
    if (pResource)
    {
        const PaletteComponent *paletteComponent = pDoc->GetCurrentPaletteComponent();

        if (paletteComponent)
        {
            memcpy(_palette, paletteComponent->Colors, 256 * sizeof(*_palette));
            _paletteCount = 256;
        }
        else
        {
            memcpy(_palette, g_egaColors, 16 * sizeof(*_palette));
            _paletteCount = 16;
        }
    }
}

void CRasterView::OnInitialUpdate()
{
    __super::OnInitialUpdate();
    CNewRasterResourceDocument *pDoc = GetDoc();
    if (pDoc)
    {
        _SyncColor(pDoc);
    }
}

void CRasterView::OnOnionSkinChanged()
{
    _InvalidateViewArea();
}

void CRasterView::OnColorShift()
{
    CNewRasterResourceDocument *pDoc = GetDoc();
    if (pDoc)
    {
        pDoc->UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(RasterChangeHint::NewView));
    }
}

void CRasterView::OnVGAPaletteChanged()
{
    CNewRasterResourceDocument *pDoc = GetDoc();
    if (pDoc)
    {
        GetDoc()->SetPaletteChoice(GetDoc()->GetPaletteChoice(), true);
        pDoc->UpdateAllViewsAndNonViews(nullptr, 0, &WrapHint(RasterChangeHint::NewView | RasterChangeHint::Color));
    }
}

void CRasterView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
    RasterChangeHint hint = GetHint<RasterChangeHint>(pHint);;

    // We have a new thing.
    __super::OnUpdate(pSender, lHint, pHint);

    if (IsFlagSet(hint, RasterChangeHint::Color))
    {
        _SyncColor(GetDoc());
        hint &= ~RasterChangeHint::Color;
    }

    if (IsFlagSet(hint, RasterChangeHint::PenStyle))
    {
        _penBitmap.DeleteObject();
        hint &= ~RasterChangeHint::PenStyle;
    }

    if (IsFlagSet(hint, RasterChangeHint::CelSelection | RasterChangeHint::LoopSelection))
    {
        // Make sure to commit any selection before moving to another cel...
        _CommitSelectionBits();
        // Don't strip these flags from lHint
    }

    // We pretty much refresh completely in all other cases:
    if (hint != RasterChangeHint::None)
    {
        _RefreshViewData();
    }

    if (IsFlagSet(hint, RasterChangeHint::ApplyToAll))
    {
        _colorEditor = GetDoc()->GetApplyToAllCels() ? COLOR_WARNINGBACKGROUND : COLOR_NORMALBACKGROUND;
        // Redraw because our background changed.
        Invalidate(FALSE);
    }
}




// CRasterView drawing

uint8_t CRasterView::_GetCurrentTransparentColor()
{
    uint8_t bTransparent = 0;
    CNewRasterResourceDocument *pDoc = GetDoc();
    if (pDoc)
    {
        CelIndex dwIndex = pDoc->GetSelectedIndex();
        bTransparent = _GetTransparentColor(dwIndex);
    }
    return bTransparent;
}

uint8_t CRasterView::_GetTransparentColor(CelIndex dwIndex)
{
    uint8_t bTransparent = 0;
    const Cel *cel = _GetCel(dwIndex);
    if (cel)
    {
        bTransparent = cel->TransparentColor;
    }
    return bTransparent;
}


//  
// Lifts data off the cels, which becomes our current selection
// 
void CRasterView::_LiftSelection()
{
    _GrabSourceData();
    _selectionManager.LiftSelection(_rectSelection, _iMainIndex, _cWorkingCels, _celData, TRUE, TRUE);
    _fSelectionLifted = true;
    _CommitSourceData();
}

void CRasterView::_GetRidOfSelection()
{
    _selectionManager.ClearSelection();
    _rectSelection.SetRectEmpty();
    _KillRubberBandTimer();
}

void CRasterView::_CommitSelectionBits()
{
    if (_selectionManager.HasSelection() && !_rectSelection.IsRectEmpty())
    {
        _GrabSourceData();
        for (int i = 0; i < _cWorkingCels; i++)
        {
            _celData[i].CalcOffset(_celData[_iMainIndex].GetPlacement(), _celData[_iMainIndex].GetSize(), CPoint(0, 0), CPoint(0, 0));
            CRect rectSelectionInCel = _rectSelection;
            rectSelectionInCel.OffsetRect(_celData[i]._point1);
            _OnDrawSelectionBits(&rectSelectionInCel, _fSelectionTransparent, i);
        }
        _CommitSourceData();
        _GetRidOfSelection();
    }
}

//
// Copy the selected bits from one spot to another, and fill the gap with the transparent
// colour.
// Operates directly on pData, which is of size sizeData;
// After this function returns, *prcSelection has been offset by pointDiff.
// If fTransparent is supplied, then the current cel's transparent colour is used
// to draw the selection transparently.
//
void CRasterView::_OnDrawSelectionBits(const RECT *prcSelection, BOOL fTransparent, int iIndex)
{
    _selectionManager.DrawSelection(CRect(prcSelection), iIndex, fTransparent, _celData[iIndex]);
}

void CRasterView::_OnDrawSelectionRect(RECT *prc, RECT *prcRectToDrawOut)
{
    *prcRectToDrawOut = CRect(); // empty it out.

    if (!IsRectEmpty(prc))
    {
        // We have a selection rect.

        if (_fDraggingSelection)
        {
            // The user is dragging it around... adjust it.
            CPoint pointDiff = _ptCurrentHover - _ptStartCapture;
            OffsetRect(prc, pointDiff.x, pointDiff.y);
            // After this, prc is now thte new spot before drawing the selection rect.
        }

        if (_selectionManager.HasSelection())
        {
            // Draw our selection in here, if any.
            _OnDrawSelectionBits(CRect(prc), _fSelectionTransparent, _iMainIndex);
        }

        *prcRectToDrawOut = *prc;
    }
    else
    {
        // The user is in the process of drawing a selection rect.
        if (_fCapturing && (_currentTool == Select))
        {
            CRect rectInProgress;
            _MakeGoodRect(rectInProgress, _ptCurrentHover, _ptStartCapture);
            *prcRectToDrawOut = rectInProgress;
        }
    }
}

void CRasterView::_OnDrawSizers(CDC *pDC, CPoint &ptWhatsLeft)
{
    CSize sizeNewZoom(_sizeNew.cx * _iZoomFactor, _GetDragScreenHeight());

    CRect rectVert;
    _GetSizerRectVert(sizeNewZoom, rectVert);
    pDC->DrawFrameControl(&rectVert, DFC_BUTTON, DFCS_BUTTONPUSH);
    CRect rectHorz;
    _GetSizerRectHorz(sizeNewZoom, rectHorz);
    pDC->DrawFrameControl(&rectHorz, DFC_BUTTON, DFCS_BUTTONPUSH);
    
    CRect rectDiag1;
    _GetSizerRectDiag1(sizeNewZoom, rectDiag1);
    pDC->DrawEdge(&rectDiag1, EDGE_RAISED, BF_MIDDLE);
    CRect rectDiag2;
    _GetSizerRectDiag2(sizeNewZoom, rectDiag2);
    pDC->DrawEdge(&rectDiag2, EDGE_RAISED, BF_MIDDLE);


    ptWhatsLeft.x = sizeNewZoom.cx + SIZER_SIZE;
    ptWhatsLeft.y = sizeNewZoom.cy + SIZER_SIZE;
}

void CRasterView::_OnBltResized(CDC *pDCDest, CDC *pDCSrc)
{
    // Hmm... the user may have shrunk the view smaller or larger than it actually is.
    assert(_sizeView.cx * _iZoomFactor == _cxViewZoom);
    int cxSrc = _sizeView.cx;
    int cySrc = _sizeView.cy;
    int cxDest = _cxViewZoom;
    int cyDest = _GetViewScreenHeight();
    if (_sizeNew.cx < _sizeView.cx)
    {
        // It is not as wide.
        cxSrc = _sizeNew.cx;
        cxDest = cxSrc * _iZoomFactor;
    }
    if (_sizeNew.cy < _sizeView.cy)
    {
        // It is not as high.
        cySrc = _sizeNew.cy;
        cyDest = _GetDragScreenHeight();
    }

    pDCDest->StretchBlt(-_xOrigin, -_yOrigin, cxDest, cyDest, pDCSrc, 0, 0, cxSrc, cySrc, SRCCOPY);

    // If it has been made wider or higher, we need to fill the void with something.
    // Use the transparent colour.
    CBrush brush;
    brush.CreateHatchBrush(HS_FDIAGONAL, RGB(0, 180, 128));
    COLORREF colorBKOld = pDCDest->SetBkColor(RGB(0, 0, 0));
    if (_sizeNew.cx > _sizeView.cx)
    {
        CRect rect(_cxViewZoom, 0, _sizeNew.cx * _iZoomFactor, _GetDragScreenHeight());
        rect.OffsetRect(-_xOrigin, -_yOrigin);
        pDCDest->FillRect(&rect, &brush);
    }
    if (_sizeNew.cy > _sizeView.cy)
    {
        CRect rect(0, _GetViewScreenHeight(), _sizeNew.cx * _iZoomFactor, _GetDragScreenHeight());
        rect.OffsetRect(-_xOrigin, -_yOrigin);
        pDCDest->FillRect(&rect, &brush);
    }
    pDCDest->SetBkColor(colorBKOld);
}

void CRasterView::OnDraw(CDC* pDC)
{
    RECT rcClient;
    GetClientRect(&rcClient);
    CDC dcMem;
    if (dcMem.CreateCompatibleDC(pDC))
    {
        int cxVisible = min(_cxViewZoom, RECTWIDTH(rcClient));
        int cyVisible = min(_GetViewScreenHeight(), RECTHEIGHT(rcClient));
        if (!_fDoubleBuf)
        {
            _GenerateDoubleBuffer(pDC);
        }
        
        if (_fDoubleBuf)
        {
            // Draw the things that are in "view coordinates":
            assert(_pbitmapDoubleBuf);
            HGDIOBJ hgdiObj = dcMem.SelectObject(_pbitmapDoubleBuf.get());
            _OnDraw(&dcMem);
            _OnBltResized(pDC, &dcMem);
            dcMem.SelectObject(hgdiObj);
        }
    }

    // Draw the sizers.
    CPoint ptWhatsLeft;
    _OnDrawSizers(pDC, ptWhatsLeft);
   

    // Finish off by drawing a navy background around the pic.
    CRect rect1;
    CRect rect2;
    CBrush brush(_colorEditor);
    int cyBottom = RECTHEIGHT(rcClient) - ptWhatsLeft.y;
    if (cyBottom > 0)
    {
        rect1.SetRect(CPoint(0, ptWhatsLeft.y), CPoint(RECTWIDTH(rcClient), ptWhatsLeft.y + cyBottom));
    }
    int cxLeft = RECTWIDTH(rcClient) - ptWhatsLeft.x;
    if (cxLeft > 0)
    {
        rect2.SetRect(CPoint(ptWhatsLeft.x, 0), CPoint(ptWhatsLeft.x + cxLeft, RECTHEIGHT(rcClient)));
    }

    CNewRasterResourceDocument *pDoc = GetDoc();
    bool fApplyToAll = pDoc && pDoc->GetApplyToAllCels();
    if (fApplyToAll)
    {
        // Paint a warning bitmap
        // REVIEW: is it ok to apply a clip rect and not restore?
        pDC->IntersectClipRect(&rect1);
        pDC->IntersectClipRect(&rect2);
        CRgn region1;
        region1.CreateRectRgn(rect1.left, rect1.top, rect1.right, rect1.bottom);
        CRgn region2;
        region2.CreateRectRgn(rect2.left, rect2.top, rect2.right, rect2.bottom);
        CRgn region3;
        region3.CreateRectRgn(0, 0, 0, 0);
        region3.CombineRgn(&region1, &region2, RGN_OR);
        pDC->SelectClipRgn(&region3, RGN_OR);
        g_AffectAllBitmap.DrawSkinParts(*pDC, rcClient, CRect(0, 0, 0, 0), CExtBitmap::__EDM_TILE); 
    }
    else
    {
        // A basic fill
        pDC->FillRect(&rect1, &brush);
        pDC->FillRect(&rect2, &brush);
    }

    __super::OnDraw(pDC);
}

void CRasterView::_DestroyDoubleBuffer()
{
    _pbitmapDoubleBuf = nullptr;
    _fDoubleBuf = false;
}

void CRasterView::_GenerateDoubleBuffer(CDC *pDC)
{
    assert(!_pbitmapDoubleBuf);
    _pbitmapDoubleBuf = std::make_unique<CBitmap>();
    _fDoubleBuf = (_pbitmapDoubleBuf->CreateCompatibleBitmap(pDC, _sizeView.cx, _sizeView.cy) != FALSE);
}

OnionSkinFrameOptions _defaultOptions;

OnionSkinFrameOptions& CRasterView::_LeftOnionOptions()
{
    CNewRasterResourceDocument *pDoc = GetDoc();
    if (pDoc)
    {
        return pDoc->GetOnionSkin().Left;
    }
    else
    {
        return _defaultOptions;
    }
}
OnionSkinFrameOptions& CRasterView::_RightOnionOptions()
{
    CNewRasterResourceDocument *pDoc = GetDoc();
    if (pDoc)
    {
        return pDoc->GetOnionSkin().Right;
    }
    else
    {
        return _defaultOptions;
    }
}

bool CRasterView::_IsCaptureTool()
{
    switch (_currentTool)
    {
    case Line:
    case RoundedRect:
    case RoundedRectSolid:
    case RectNormal:
    case RectSolid:
    case Oval:
    case OvalSolid:
    case Select:
    case Pen:
    case Replace:
        return true;
    default:
        return false;
    }
}

void CRasterView::_InitPatternBrush()
{
    // We need four lines for each thing.
    uint16_t wFoo[4];   // Four lines

    for (int y = 0; y < 4; y++)
    {
        uint16_t rowBits = 0;
        for (int x = 0; x < 4; x++)
        {
            if (BayerMatrix[y][x] <= _ditherStrength)
            {
                // This bit is on
                rowBits |= (0x1 << x);
            }
        }
        // Fill up the stride
        rowBits |= (rowBits << 4);
        wFoo[y] = rowBits | (rowBits << 8);
    }

    _bitmapBrush.DeleteObject();
    _brushPattern.DeleteObject();
    // Create a monochrome bitmap. 
    // WORD wFoo[4] = { 0xaaaa, 0x5555, 0xaaaa, 0x5555 };
    if (_bitmapBrush.CreateBitmap(4, 4, 1, 1, (uint8_t*)wFoo))
    {
        _brushPattern.CreatePatternBrush(&_bitmapBrush);
    }
}

BOOL CRasterView::_CanResize()
{
    CNewRasterResourceDocument *pDoc = GetDoc();
    if (pDoc)
    {
        RasterCaps caps = pDoc->GetComponent<RasterComponent>().Traits.Caps;
        return (caps & RasterCaps::Resize) != RasterCaps::None;
    }
    return FALSE;
}

//
// When a drawing command happens at a certain point, that may be at a different location
// in another cel (if we're drawing on multiple cels at a time).  So, we need to adjust each
// point based on the origins of each of the cels.  That's what this method does.
//
void CRasterView::_PrepareCelOffsets(const CPoint &ptMain1, const CPoint &ptMain2)
{
    assert(_cWorkingCels == _celData.size());
    point16 ptPlacementMain = _celData[_iMainIndex].GetPlacement();
    size16 sizeMain = _celData[_iMainIndex].GetSize();
    for (size_t i = 0; i < _celData.size(); i++)
    {
        _celData[i].CalcOffset(ptPlacementMain, sizeMain, ptMain1, ptMain2);
    }
}

void CRasterView::_DrawCaptureTool(CDC *pDC)
{
    CSCIDrawHelper helper(pDC, _GetMainViewData(), SizeToCSize(_sizeView), _palette, _paletteCount);
    _DrawCaptureToolHelper(&helper.dc, _ptStartCapture, _ptCurrentHover);
}

void CRasterView::_ApplyCaptureTool()
{
    CPoint ptStart = _ptStartCapture;
    CPoint ptEnd = _ptCurrentHover;
    _PrepareCelOffsets(ptStart, ptEnd);
    for (int i = 0; i < _cWorkingCels; i++)
    {
        CSCIDrawHelper helper(nullptr, _celData[i].GetDataPtr(), SizeToCSize(_celData[i].GetSize()), _palette, _paletteCount);
        _DrawCaptureToolHelper(&helper.dc, _celData[i]._point1, _celData[i]._point2);
    }
}

COLORREF CRasterView::_SCIColorToCOLORREF(uint8_t color)
{
    color %= _paletteCount;
    RGBQUAD rgb = _palette[color];
    return RGB(rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue);
}

int CRasterView::_GetPenWidth()
{
    int penWidth = 1;
    if (GetDoc())
    {
        penWidth = GetDoc()->GetPenStyle().bPatternSize + 1;
        // +1 since pattern size starts at zero.
    }
    return penWidth;
}

void CRasterView::_DrawCaptureToolHelper(CDC *pDC, CPoint ptStart, CPoint ptEnd)
{
    int penWidth = _GetPenWidth();
    // For commands that support two "origins" of drawing, set that up now.
    if (GetKeyState(VK_SHIFT) & 0x8000)
    {
        // In the "SHIFT" case, the origin is _ptStartCapture
        int dx = ptEnd.x - ptStart.x;
        int dy = ptEnd.y - ptStart.y;
        ptStart.x = ptStart.x - dx;
        ptStart.y = ptStart.y - dy;
    }

    // Some handy defaults:
    LOGBRUSH logbrush = { 0 };

    // Herein lies the issue.
    logbrush.lbColor = _SCIColorToCOLORREF(_fAux ? _color : _alternateColor);
    logbrush.lbStyle = BS_SOLID;

    // For dithered stuff:
    LOGBRUSH logBrushDither = { BS_PATTERN, 0, (ULONG_PTR)(HBITMAP)_bitmapBrush };
    CPen pen;
    int crTextOld, crBkOld;
    int nPenEndCaps = (_currentTool == Line) ? PS_ENDCAP_ROUND : PS_ENDCAP_FLAT;
    if (_fDithered)
    {
        pen.CreatePen(PS_GEOMETRIC | nPenEndCaps, penWidth, &logBrushDither, 0, nullptr);
        crTextOld = pDC->SetTextColor(_SCIColorToCOLORREF(_color));
        crBkOld = pDC->SetBkColor(_SCIColorToCOLORREF(_alternateColor));
    }
    else
    {
        pen.CreatePen(PS_GEOMETRIC | nPenEndCaps, penWidth, &logbrush, 0, nullptr);
    }

    CRect rect;
    _MakeGoodRect(rect, ptStart, ptEnd);

    CBrush brushHollow;
    LOGBRUSH logBrush = { BS_HOLLOW, 0, 0 };
    brushHollow.CreateBrushIndirect(&logBrush);

    CBrush brushSolid;
    brushSolid.CreateSolidBrush(_SCIColorToCOLORREF(_fAux ? _color : _alternateColor));

    switch (_currentTool)
    {
    case Line:
        {
            HGDIOBJ hOld = pDC->SelectObject(&pen);
            pDC->MoveTo(ptStart);
            pDC->LineTo(ptEnd);
            pDC->SelectObject(hOld);
        }
        break;
    case RoundedRect:
        {
            HGDIOBJ hOldPen = pDC->SelectObject(&pen);
            HGDIOBJ hOldBrush = pDC->SelectObject(&brushHollow);
            CPoint ptEllipse(8, 8);
            pDC->RoundRect(&rect, ptEllipse);
            pDC->SelectObject(hOldBrush);
            pDC->SelectObject(hOldPen);
        }
        break;
    case RoundedRectSolid:
        {
            HGDIOBJ hOldPen = pDC->SelectObject(&pen);
            HGDIOBJ hOldBrush = pDC->SelectObject(_fDithered ? &_brushPattern : &brushSolid);
            CPoint ptEllipse(8, 8);
            pDC->RoundRect(&rect, ptEllipse);
            pDC->SelectObject(hOldBrush);
            pDC->SelectObject(hOldPen);
        }
        break;
    case RectNormal:
        {
            HGDIOBJ hOldPen = pDC->SelectObject(&pen);
            HGDIOBJ hOldBrush = pDC->SelectObject(&brushHollow);
            pDC->Rectangle(&rect);
            pDC->SelectObject(hOldBrush);
            pDC->SelectObject(hOldPen);
        }
        break;
    case RectSolid:
        {
            HGDIOBJ hOldPen = pDC->SelectObject(&pen);
            HGDIOBJ hOldBrush = pDC->SelectObject(_fDithered ? &_brushPattern : &brushSolid);
            pDC->Rectangle(&rect);
            pDC->SelectObject(hOldBrush);
            pDC->SelectObject(hOldPen);
        }
        break;
    case Oval:
        {
            HGDIOBJ hOldPen = pDC->SelectObject(&pen);
            HGDIOBJ hOldBrush = pDC->SelectObject(&brushHollow);
            pDC->Ellipse(&rect);
            pDC->SelectObject(hOldBrush);
            pDC->SelectObject(hOldPen);
        }
        break;
    case OvalSolid:
        {
            HGDIOBJ hOldPen = pDC->SelectObject(&pen);
            HGDIOBJ hOldBrush = pDC->SelectObject(_fDithered ? &_brushPattern : &brushSolid);
            pDC->Ellipse(&rect);
            pDC->SelectObject(hOldBrush);
            pDC->SelectObject(hOldPen);
        }
        break;
    }

    if (_fDithered)
    {
        // Put back the old colours
        pDC->SetTextColor(crTextOld);
        pDC->SetBkColor(crBkOld);
    }
}

//
// Draw a flickering pixel where the "hot spot" of the image is.
//
void CRasterView::_DrawHotSpot(CDC *pDC)
{
    const Cel *activeCel = _GetSelectedCel();
    if (activeCel)
    {
        CPoint pointPlacement = PointToCPoint(activeCel->placement);
        // Determine the origin style
        CPoint point;
        CNewRasterResourceDocument *pDoc = GetDoc();
        if (pDoc)
        {
            OriginStyle originStyle = pDoc->GetComponent<RasterComponent>().Traits.OriginStyle;
            switch (originStyle)
            {
            case OriginStyle::BottomCenter:
                point = CPoint(_sizeView.cx / 2, _sizeView.cy);
                point -= pointPlacement;
                point.y = _sizeView.cy - point.y;
                break;
                    
            case OriginStyle::TopLeft:
                point = CPoint(0, 0);
                point += pointPlacement;
                point.y = _sizeView.cy - point.y - 1;
                break;
            }
        }

        CRect rectView(0, 0, _sizeView.cx, _sizeView.cy);
        if (rectView.PtInRect(point))
        {
            // Inverted for some reason...
            point.y = _sizeView.cy - point.y - 1;
            CPen pen;
            pen.CreatePen(PS_SOLID, 1, _fHotSpot ? RGB(255, 255, 255) : RGB(0, 0, 0));
            HGDIOBJ oldPen = pDC->SelectObject(pen);
            pDC->MoveTo(point);
            pDC->LineTo(point + CPoint(0, 1)); // Anywhere...
            pDC->SelectObject(oldPen);
        }
    }
}

void _EnsureBuffer(CDC *pDC, std::unique_ptr<CBitmap> &buffer, size16 size)
{
    CSize sizeExisting;
    if (buffer)
    {
        sizeExisting = buffer->GetBitmapDimension();
    }
    if ((sizeExisting.cx != size.cx) || (sizeExisting.cy != size.cy))
    {
        buffer = std::make_unique<CBitmap>();
        buffer->CreateCompatibleBitmap(pDC, size.cx, size.cy);
    }
}

void DashFromTo(std::vector<POINT> &points, const CPoint &from, const CPoint &to, const CPoint &direction, int &remaining)
{
    CPoint current = from;
    CPoint startLine = current;
    while (current != to)
    {
        if (remaining == 0)
        {
            startLine = current;
        }
        else if (remaining == 2)
        {
            // Draw a line
            points.push_back(startLine);
            points.push_back(current);
        }
        current += direction;
        remaining++;
        remaining %= 3;
    }
    if (remaining != 0)
    {
        points.push_back(startLine);
        points.push_back(current);
    }
}

void CRasterView::_DrawDashedSelectionRect(CDC *pDC, const CRect &rect)
{
    // Incredibly, GDI offers no way to control pen stoke length. One dot is always 4 pixels, which isn't suitable for our purposes.
    // So we draw a ton of individual lines instead.

    static bool polyPointsInitialized = false;
    static DWORD polyPoints[500];
    if (!polyPointsInitialized)
    {
        std::fill(begin(polyPoints), end(polyPoints), 2);
        polyPointsInitialized = true;
    }

    std::vector<POINT> points;
    points.reserve(200);
    int count = 0;
    _iRubberBandFrameNumber %= 3;
    // Two on and one off
    int remaining =  2 - _iRubberBandFrameNumber;
    DashFromTo(points, rect.TopLeft(), CPoint(rect.right - 1, rect.top), CPoint(1, 0), remaining);
    DashFromTo(points, CPoint(rect.right - 1, rect.top), rect.BottomRight() - CPoint(1, 1) , CPoint(0, 1), remaining);
    DashFromTo(points, rect.BottomRight() - CPoint(1, 1), CPoint(rect.left, rect.bottom - 1), CPoint(-1, 0), remaining);
    DashFromTo(points, CPoint(rect.left, rect.bottom - 1), rect.TopLeft(), CPoint(0, -1), remaining);

    if (!points.empty())
    {
        CPen selectionPen;
        selectionPen.CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        HGDIOBJ oldPen = pDC->SelectObject(selectionPen);
        int oldRop = pDC->SetROP2(R2_XORPEN);
        pDC->PolyPolyline(&points[0], polyPoints, min((int)(points.size() / 2), ARRAYSIZE(polyPoints)));
        pDC->SetROP2(oldRop);

        pDC->SelectObject(oldPen);
    }
}

void CRasterView::_DrawOnionSkin(RGBQUAD *pixels, RGBQUAD tint, const OnionSkinFrameOptions &options, const CelData &onion, const Cel &main)
{
    uint8_t sourceAlpha = tint.rgbReserved;
    size16 onionSize = onion.GetSize();
    CPoint pt;
    size16 sizeDest = main.size;
    CPoint sourceToDest = -onion.CalcOffset(main.placement, sizeDest, pt);
    sourceToDest.y = (sizeDest.cy - onionSize.cy) -sourceToDest.y;
    uint8_t tx = onion.GetTransparentColor();
    for (int ySrc = 0; ySrc < onionSize.cy; ySrc++)
    {
        const uint8_t *source = onion.GetDataPtr() + ySrc * CX_ACTUAL(onionSize.cx);
        int yDest = ySrc + sourceToDest.y;
        if (yDest >= 0 && yDest < sizeDest.cy)
        {
            RGBQUAD *dest = pixels + yDest * sizeDest.cx;
            for (int xSrc = 0; xSrc < onionSize.cx; xSrc++)
            {
                int xDest = xSrc + sourceToDest.x;
                if (xDest >= 0 && xDest < sizeDest.cx)
                {
                    uint8_t sourcePixel = *(source + xSrc);
                    if (sourcePixel != tx)
                    {
                        RGBQUAD destColor = *(dest + xDest);
                        RGBQUAD sourceColor = _palette[sourcePixel];
                        destColor.rgbBlue = sourceColor.rgbBlue * tint.rgbBlue * sourceAlpha / 255 / 255 + destColor.rgbBlue * (255 - sourceAlpha) / 255;
                        destColor.rgbGreen = sourceColor.rgbGreen * tint.rgbGreen * sourceAlpha / 255 / 255 + destColor.rgbGreen * (255 - sourceAlpha) / 255;
                        destColor.rgbRed = sourceColor.rgbRed * tint.rgbRed * sourceAlpha / 255 / 255 + destColor.rgbRed * (255 - sourceAlpha) / 255;
                        *(dest + xDest) = destColor;
                    }
                }
            }
        }
    }
}

#define GetAValue(rgb)      (LOBYTE((rgb)>>24))

RGBQUAD RGBAQUADFromCOLORREF(COLORREF color)
{
    RGBQUAD quad;
    quad.rgbBlue = GetBValue(color);
    quad.rgbRed = GetRValue(color);
    quad.rgbGreen = GetGValue(color);
    quad.rgbReserved = GetAValue(color);
    return quad;
}

void CRasterView::_OnDraw(CDC* pDC)
{
    const Cel *activeCel = _GetSelectedCel();
    if (activeCel)
    {
        // So patterned things line up nicely across cels
        // pDC->SetBrushOrg(activeCel->placement.x, activeCel->size.cy + activeCel->placement.y);
        // Doesn't seem to work...

        if (_GetMainViewData())
        {
#ifdef DEBUG
            assert(activeCel->size == _sizeView);
            // Otherwise, we'll have a buffer overrun in _pViewData
#endif
            if (!_fPreviewPen)
            {
                // Get a copy from the source, to start with.
                CopyBitmapData(*_GetRaster(), CelIndex(_dwIndex), _GetMainViewData(), _sizeView);
            }
        }

        // Draw selection rect. Now, however, this only draws selection bits, and returns to us
        // the rect in which selection should be drawn (so we can draw it after).
        CRect rectSelectionCopy = _rectSelection;
        CRect selectionRectToDrawAfter;
        _OnDrawSelectionRect(&rectSelectionCopy, &selectionRectToDrawAfter);

        if (_fCapturing)
        {
            // This is drawn using GDI - how does it not get overwritten???
            // Ah, we copy our bits in... I get it.
            _DrawCaptureTool(pDC);
        }

        const OnionSkinOptions &onionOptions = GetDoc()->GetOnionSkin();
        CelIndex leftIndex;
        const Cel *leftCel = _GetPreviousCel(leftIndex);
        CelIndex rightIndex;
        const Cel *rightCel = _GetNextCel(rightIndex);
        if ((onionOptions.Left.Enabled && leftCel) || (onionOptions.Right.Enabled && rightCel))
        {
            // If onion skin is enabled, we need to control drawing more precisely than GDI will let us.
            // So we'll make a 32-bit DIB and copy the pixels to it manually (doing palette lookup)
            BITMAPINFO bmi = { 0 };
            bmi.bmiHeader.biSize = sizeof(BITMAPINFO);
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            bmi.bmiHeader.biWidth = _sizeView.cx;
            bmi.bmiHeader.biHeight = _sizeView.cy;
            bmi.bmiHeader.biPlanes = 1;
            RGBQUAD *pixels;
            HBITMAP dib = CreateDIBSection(*pDC, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&pixels), nullptr, 0);
            if (dib)
            {
                // Fill the background with the transparent color, so that we have the optional of not drawing transparent parts when
                // we draw the main view (so the onion skin will show though)
                std::fill(pixels, pixels + _sizeView.cx * _sizeView.cy, _palette[activeCel->TransparentColor]);

                CelIndex prev = _dwIndex;
                const OnionSkinFrameOptions &onionFrameLeft = onionOptions.Left;
                const OnionSkinFrameOptions &onionFrameRight = onionOptions.Right;

                CelData leftOnionBits;
                if (onionOptions.Left.Enabled && leftCel)
                {
                    size16 celSize = leftCel->size;
                    leftOnionBits.SetSize(leftIndex, leftCel->placement, leftCel->size, leftCel->TransparentColor);
                    CopyBitmapData(*_GetRaster(), leftIndex, leftOnionBits.GetDataPtr(), leftOnionBits.GetSize());
                }
                CelData rightOnionBits;
                if (onionOptions.Right.Enabled && rightCel)
                {
                    size16 celSize = rightCel->size;
                    rightOnionBits.SetSize(rightIndex, rightCel->placement, rightCel->size, rightCel->TransparentColor);
                    CopyBitmapData(*_GetRaster(), rightIndex, rightOnionBits.GetDataPtr(), rightOnionBits.GetSize());
                }

                if (onionFrameLeft.Enabled && !appState->_onionLeftOnTop)
                {
                    _DrawOnionSkin(pixels, RGBAQUADFromCOLORREF(appState->_onionLeftTint), onionFrameLeft, leftOnionBits, *activeCel);
                }
                if (onionFrameRight.Enabled && !appState->_onionRightOnTop)
                {
                    _DrawOnionSkin(pixels, RGBAQUADFromCOLORREF(appState->_onionRightTint), onionFrameRight, rightOnionBits, *activeCel);
                }

                uint8_t *sourceData = _GetMainViewData();
                for (int y = 0; y < _sizeView.cy; y++)
                {
                    uint8_t *sourceLine = sourceData + (y * CX_ACTUAL(_sizeView.cx));
                    RGBQUAD *destLine = pixels + (y * _sizeView.cx);
                    for (int x = 0; x < _sizeView.cx; x++)
                    {
                        uint8_t sourceByte = *(sourceLine + x);
                        if (sourceByte != activeCel->TransparentColor)
                        {
                            *(destLine + x) = _palette[sourceByte];
                        }
                    }
                }

                if (onionFrameLeft.Enabled && appState->_onionLeftOnTop)
                {
                    _DrawOnionSkin(pixels, RGBAQUADFromCOLORREF(appState->_onionLeftTint), onionFrameLeft, leftOnionBits, *activeCel);
                }
                if (onionFrameRight.Enabled && appState->_onionRightOnTop)
                {
                    _DrawOnionSkin(pixels, RGBAQUADFromCOLORREF(appState->_onionRightTint), onionFrameRight, rightOnionBits, *activeCel);
                }

                CDC dcMem;
                if (dcMem.CreateCompatibleDC(pDC))
                {
                    HGDIOBJ hOld = dcMem.SelectObject(dib);
                    StretchBlt(*pDC, 0, 0, _sizeView.cx, _sizeView.cy, dcMem, 0, 0, _sizeView.cx, _sizeView.cy, SRCCOPY);
                    dcMem.SelectObject(hOld);
                }

                DeleteObject(dib);
            }
        }
        else
        {
            // Just a straight copy from our raw bits to the buffer.
            // - it involves palette conversion, so it could be expensive.
            SCIBitmapInfo bmi(_sizeView.cx, _sizeView.cy, _palette, _paletteCount);
            StretchDIBits((HDC)*pDC, 0, 0, _sizeView.cx, _sizeView.cy, 0, 0, _sizeView.cx, _sizeView.cy, _GetMainViewData(), &bmi, DIB_RGB_COLORS, SRCCOPY);
        }

        // Layers on top of our buffer. Selection rect and other stuff.

        if (!selectionRectToDrawAfter.IsRectEmpty())
        {
            _DrawDashedSelectionRect(pDC, selectionRectToDrawAfter);
        }

        if (_currentTool == SetPlacement)
        {
            _DrawHotSpot(pDC);
        }
    }
}

void CRasterView::_OnChangeTool(ViewToolType newTool)
{
    if (newTool != _currentTool)
    {
        if (_currentTool == Select)
        {
            _CommitSelectionBits();
            // Get rid of the selection in case _CommitSelectionBits doesn't do it.
            // _CommitSelectionBits only does stuff if there was an actual lifted selection.
            // _GetRidOfSelection will kill the rubber band timer and zero out the selection rect.
            _GetRidOfSelection();
            _InvalidateViewArea();
        }

        if (newTool == SetPlacement)
        {
            _StartHotSpotTimer();
            _InvalidateViewArea();
        }
        if (_currentTool == SetPlacement)
        {
            _KillHotSpotTimer();
            _InvalidateViewArea();
        }
        _currentTool = newTool;
    }
}

const key_value_pair<UINT, CRasterView::ViewToolType> CRasterView::c_viewIDToTool[] =
{
    { ID_REPLACE, Replace },
    { ID_PENTOOL, Pen },
    { ID_LINE, Line },
    { ID_FILL, Fill },
    { ID_ZOOM, Zoom },
    { ID_DRAWOFF, Off },
    { ID_DRAWRECT, RectNormal },
    { ID_DRAWRECTSOLID, RectSolid },
    { ID_DRAWOVAL, Oval },
    { ID_DRAWOVALSOLID, OvalSolid },
    { ID_DRAWROUNDEDRECT, RoundedRect },
    { ID_DRAWROUNDEDRECTSOLID, RoundedRectSolid },
    { ID_SELECT, Select },
    { ID_SETPLACEMENT, SetPlacement },
    { ID_EYEDROPPER, EyeDropper },
    { ID_SETTRANSPARENCY, SetTransparency },
    // Add more here...
};
CRasterView::ViewToolType CRasterView::_IDToViewTool(UINT nID)
{
    return LookupKeyValue(c_viewIDToTool, ARRAYSIZE(c_viewIDToTool), nID, Off);
}

const key_value_pair<CRasterView::ViewToolType, int> CRasterView::c_viewToolToCursor[] =
{
    { Pen, IDC_CURSORPEN },
    { Fill, IDC_CURSORFILL },
    { Line, IDC_CURSORLINE },
    { Zoom, IDC_CURSORZOOM },
    { Select, IDC_CURSORLINE },
    { SetPlacement, IDC_CURSORPLACEMENT },
    { Replace, IDC_CURSORPEN },
    { Oval, IDC_CURSORSHAPE },
    { OvalSolid, IDC_CURSORSHAPE },
    { RectNormal, IDC_CURSORSHAPE },
    { RectSolid, IDC_CURSORSHAPE },
    { RoundedRect, IDC_CURSORSHAPE },
    { RoundedRectSolid, IDC_CURSORSHAPE },
    { EyeDropper, IDC_CURSOREYEDROPPER },
    { SetTransparency, IDC_CURSOREYEDROPPER }
};
int CRasterView::_ViewIDToCursor(ViewToolType vt)
{
    return LookupKeyValue(c_viewToolToCursor, ARRAYSIZE(c_viewToolToCursor), vt, -1);
}


void CRasterView::_UpdateCursor()
{
    // First check if we're on the sizers:
    OnSizerType onSizerType = _OnSizer(_ptCurrentHoverClient);
    if ((onSizerType == None) || (!_CanResize()))
    {
        // Ok, we're not sizing (or not resizeable).  What are we doing?
        if (!_rectSelection.IsRectEmpty() && _rectSelection.PtInRect(_ptCurrentHover))
        {
            // Make the cursor a cross.
            SetCursor(LoadCursor(NULL, IDC_SIZEALL));
        }
        else
        {
            CPoint ptView = _MapClientPointToPic(_ptCurrentHoverClient);
            // Get the current cursor.
            int idResource = _ViewIDToCursor(_currentTool);
            if ((idResource != -1) && _PointInView(ptView))
            {
                SetCursor(LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(idResource)));
            }
            else
            {
                // No cursor for current tool, or else we're outside the view.
                SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
            }
        }
    }
    else
    {
        PSTR iCursor = MAKEINTRESOURCE(IDC_ARROW);  // Just some default value
        RasterResizeFlags anchor = RasterResizeFlags::Normal;
        if (GetKeyState(VK_CONTROL) & 0x8000)
        {
            anchor = RasterResizeFlags::AnchorBottomRight;
        }
        if (GetKeyState(VK_SHIFT) & 0x8000)
        {
            anchor = RasterResizeFlags::Stretch; // Stretch overrides.
        }

        switch (onSizerType)
        {
        case Horizontal:
            switch (anchor)
            {
            case RasterResizeFlags::Normal:
                iCursor = MAKEINTRESOURCE(IDC_CURSORRESIZEWE_TL);
                break;
            case RasterResizeFlags::AnchorBottomRight:
                iCursor = MAKEINTRESOURCE(IDC_CURSORRESIZEWE_BR);
                break;
            case RasterResizeFlags::Stretch:
                iCursor = MAKEINTRESOURCE(IDC_CURSORRESIZEWE_STRETCH);
                break;
            }
            break;
        case Vertical:
            switch (anchor)
            {
            case RasterResizeFlags::Normal:
                iCursor = MAKEINTRESOURCE(IDC_CURSORRESIZENS_TL);
                break;
            case RasterResizeFlags::AnchorBottomRight:
                iCursor = MAKEINTRESOURCE(IDC_CURSORRESIZENS_BR);
                break;
            case RasterResizeFlags::Stretch:
                iCursor = MAKEINTRESOURCE(IDC_CURSORRESIZENS_STRETCH);
                break;
            }
            break;
        case Diagonal:
            switch (anchor)
            {
            case RasterResizeFlags::Normal:
                iCursor = MAKEINTRESOURCE(IDC_CURSORRESIZENSWE_TL);
                break;
            case RasterResizeFlags::AnchorBottomRight:
                iCursor = MAKEINTRESOURCE(IDC_CURSORRESIZENSWE_BR);
                break;
            case RasterResizeFlags::Stretch:
                iCursor = MAKEINTRESOURCE(IDC_CURSORRESIZENSWE_STRETCH);
                break;
            }
            break;
        }
        SetCursor(LoadCursor(AfxGetInstanceHandle(), iCursor));
       
    }
}

//
// Make big
//
void CRasterView::_OnZoomLClick()
{
    if (_iZoomFactor < 16)
    {
        _iZoomFactor++;
        _cxViewZoom = _sizeView.cx * _iZoomFactor;
        _cyViewZoom = _sizeView.cy * _iZoomFactor;
        _InvalidateScrollStuff();
        Invalidate(FALSE);
        CNewRasterResourceDocument *pDoc = GetDoc();
        if (pDoc)
        {
            pDoc->SetDefaultZoom(_iZoomFactor);
        }
    }
    // Otherwise, make it no bigger
}

//
// Make small
//
void CRasterView::_OnZoomRClick()
{
    if (_iZoomFactor > 1)
    {
        _iZoomFactor--;
        _cxViewZoom = _sizeView.cx * _iZoomFactor;
        _cyViewZoom = _sizeView.cy * _iZoomFactor;
        _InvalidateScrollStuff();
        Invalidate(FALSE);
        CNewRasterResourceDocument *pDoc = GetDoc();
        if (pDoc)
        {
            pDoc->SetDefaultZoom(_iZoomFactor);
        }
    }
    // Otherwise, make it no smaller
}

void CRasterView::_OnEyeDrop(CPoint point, bool fUseForeground, bool setTransparency)
{
    CNewRasterResourceDocument *pDoc = GetDoc();
    if (pDoc)
    {
        CelData &celData = _celData[_iMainIndex];
        // For some reason I have to invert the y
        uint8_t *pSrc = celData.GetDataPtr() + point.x + CX_ACTUAL(celData.GetSize().cx) * (celData.GetSize().cy - point.y - 1);

        if (setTransparency)
        {
            pDoc->ApplyChanges<RasterComponent>(
                [&](RasterComponent &raster)
            {
                return WrapRasterChange(SetGroupTransparentColor(raster, _cWorkingCels, _rgdwGroups, *pSrc));
            }
            );
        }
        else
        {
            if (fUseForeground)
            {
                pDoc->SetViewColor(*pSrc);
            }
            else
            {
                pDoc->SetAlternateViewColor(*pSrc);
            }
        }
    }
}

void CRasterView::_OnFillLClick(CPoint point, bool fUseForeground)
{
    _GrabSourceData();
    for (int i = 0; i < _cWorkingCels; i++)
    {
        CSCIDrawHelper helper(nullptr, _celData[i].GetDataPtr(), SizeToCSize(_celData[i].GetSize()), _palette, _paletteCount);

        CBrush brush;
        HGDIOBJ hOld;
        int crTextOld, crBkOld;
        if (_fDithered)
        {
            crTextOld = helper.dc.SetTextColor(_SCIColorToCOLORREF(_color));
            crBkOld = helper.dc.SetBkColor(_SCIColorToCOLORREF(_alternateColor));
            hOld = helper.dc.SelectObject(&_brushPattern);
        }
        else
        {
            brush.CreateSolidBrush(_SCIColorToCOLORREF(fUseForeground ? _color : _alternateColor));
            hOld = helper.dc.SelectObject(&brush);
        }
        COLORREF color = helper.dc.GetPixel(point);
        helper.dc.ExtFloodFill(point.x, point.y, color, FLOODFILLSURFACE );
        helper.dc.SelectObject(hOld);
        if (_fDithered)
        {
            helper.dc.SetTextColor(crTextOld);
            helper.dc.SetBkColor(crBkOld);
        }
    }
    _CommitSourceData();
}


void CRasterView::_DrawDitheredPen(CDC *pDC, CPoint point)
{
    LOGBRUSH logBrushDither = { BS_PATTERN, 0, (ULONG_PTR)(HBITMAP)_bitmapBrush };
    CPen pen(PS_GEOMETRIC, _GetPenWidth(), &logBrushDither, 0, NULL);
    int crTextOld = pDC->SetTextColor(_SCIColorToCOLORREF(_color));
    int crBkOld = pDC->SetBkColor(_SCIColorToCOLORREF(_alternateColor));
    HGDIOBJ hOldPen = pDC->SelectObject(&pen);
    pDC->MoveTo(point);
    pDC->LineTo(point);
    pDC->SelectObject(hOldPen);
    pDC->SetTextColor(crTextOld);
    pDC->SetBkColor(crBkOld);
}

bool CRasterView::_EnsurePenBitmap()
{
    PenStyle penStyle = GetDoc()->GetPenStyle();
    if (penStyle.fRandomNR)
    {
        // If random, keep redo-ing it.
        _bRandomNR = (rand() % 128);
        _penBitmap.DeleteObject();
    }

    if (!(HBITMAP)_penBitmap && GetDoc())
    {
        CreatePatternBitmap(_penBitmap, penStyle.bPatternSize, _bRandomNR, penStyle.fRectangle, penStyle.fPattern);
    }
    return (HBITMAP)_penBitmap != nullptr;
}

// ISSUES:
// BS_PATTERN brush can't be drawn transparent :-(
// Pens: PS_GEOMTRIC only works with round endcaps?
// Pens: no transparency either!
// Pens: pens always have same origin. -> SetBrushOrg fixes this.
void CRasterView::_DrawPen(CDC *pDC, CPoint point, uint8_t color, uint8_t alternateColor, BOOL fUseForeground)
{
    PenStyle penStyle = GetDoc()->GetPenStyle();
    if (_EnsurePenBitmap())
    {
        int penWidth = penStyle.bPatternSize * 2 + 1;
        int crTextOld = pDC->SetTextColor(_SCIColorToCOLORREF(fUseForeground ? color : alternateColor));
        CDC dcMem;
        dcMem.CreateCompatibleDC(pDC);
        HGDIOBJ hOldBitmap = dcMem.SelectObject(_penBitmap);
        pDC->TransparentBlt(point.x - penStyle.bPatternSize, point.y - penStyle.bPatternSize, penWidth, penWidth, &dcMem, 0, 0, penWidth, penWidth, RGB(255, 255, 255));
        dcMem.SelectObject(hOldBitmap);
        pDC->SetTextColor(crTextOld);
    }
}

void CRasterView::_OnReplaceTool(CPoint point, BOOL fReplaceAll, bool fLeftClick)
{
    _PrepareCelOffsets(point, point);
    for (int i = 0; i < _cWorkingCels; i++)
    {
        _EnsureScratchBuffer1(_celData[i].GetSize());
        if (fReplaceAll)
        {
            // Our mask is all white.
            FillMemory(_pBitsScratch1.get(), CX_ACTUAL(_sizeScratch1.cx) * _sizeScratch1.cy, 0x0f); // White.
        }
        else
        {
            // Use the pen to determine which colours to replace.
            // Fill mask buffer with 0 before we start drawing into it.
            FillMemory(_pBitsScratch1.get(), CX_ACTUAL(_sizeScratch1.cx) * _sizeScratch1.cy, 0);
            {
                CSCIDrawHelper helper(nullptr, _pBitsScratch1.get(), SizeToCSize(_sizeScratch1), _palette, _paletteCount);

                uint8_t white = 0xf;
                _DrawPen(&helper.dc, _celData[i]._point1, white, 0, TRUE);
            }
        }

        // Now we have our mask.  Replace colours in the original, based on that mask:
        assert(_sizeScratch1 == _celData[i].GetSize());
        uint8_t *pCurrentVD = _celData[i].GetDataPtr();
        uint8_t *pCurrentScratch = _pBitsScratch1.get();
        uint8_t bColor1 = fLeftClick ? _color : _alternateColor;
        uint8_t bColor2 = fLeftClick ? _alternateColor : _color;
        for (int j = 0; j < (CX_ACTUAL(_sizeScratch1.cx) * _sizeScratch1.cy); j++, pCurrentVD++, pCurrentScratch++)
        {
            if ((*pCurrentVD == bColor1) && (*pCurrentScratch == 0xf))
            {
                // Replace this pixel.
                *pCurrentVD = bColor2;
            }
        }
    }
}

void CRasterView::_OnPenClick(CPoint point, bool fUseForeground)
{
    _GrabSourceData();
    {
        if (_currentTool == Replace)
        {
            _OnReplaceTool(point, FALSE, fUseForeground);
        }
        else
        {
            _PrepareCelOffsets(point, point);
            for (int i = 0; i < _cWorkingCels; i++)
            {
                CSCIDrawHelper helper(NULL, _celData[i].GetDataPtr(), _celData[i].GetSize(), _palette, _paletteCount);
                if (_fDithered)
                {
                    _DrawDitheredPen(&helper.dc, _celData[i]._point1);
                }
                else
                {
                    _DrawPen(&helper.dc, _celData[i]._point1, _color, _alternateColor, fUseForeground);
                }
            }
        }
    }
    _CommitSourceData();
}


void CRasterView::_OnSetPlacementLClick(CPoint ptView)
{
    CNewRasterResourceDocument *pDoc = GetDoc();
    if (pDoc)
    {
        CPoint pointOrigin;
        CPoint pointPlacement;
        OriginStyle originStyle = pDoc->GetComponent<RasterComponent>().Traits.OriginStyle;
        switch (originStyle)
        {
        case OriginStyle::BottomCenter:
            pointOrigin = CPoint(_sizeView.cx / 2, _sizeView.cy - 1);
            pointPlacement = pointOrigin - ptView;
            break;
            
        case OriginStyle::TopLeft:
            pointOrigin = CPoint(0, 0);
            pointPlacement = pointOrigin + ptView;
            break;
        }

        pDoc->ApplyChanges<RasterComponent>(
            [&](RasterComponent &raster)
            {
                point16 thePoint = CPointToPointClamp(pointPlacement);
                return WrapRasterChange(SetGroupPlacement(raster, _cWorkingCels, _rgdwGroups, thePoint.x, thePoint.y));
            }
            );
    }
}

CNewRasterResourceDocument *CRasterView::GetDoc() const
{
    return static_cast<CNewRasterResourceDocument*>(GetDocument());
}

void CRasterView::_OnCaptureToolButtonDown(CPoint point, bool fAux)
{
    if (!_fCapturing)
    {
        if (_currentTool == Select)
        {
            // There are two choices here:
            // If the user has clicked inside an existing selection rect, we will begin a 
            // drag selection operation.
            if (_rectSelection.PtInRect(point))
            {
                _fDraggingSelection = true;
            }
            else
            {
                // Otherwise, the user is creating a new selection rect.  Reset the selection rect.
                // But first, commit any old bits.
                if (!_rectSelection.IsRectEmpty())
                {
                    _CommitSelectionBits();
                }
                _rectSelection.SetRectEmpty();
            }
        }

        if (_IsPenTool())
        {
            _GrabSourceData();
            // From this point onward (until the user lets go)
            // indicate we're previewing the pen, so we don't keep updating from the source.
            _fPreviewPen = true;
            _OnPenClick(point, fAux);
            _InvalidateViewArea();
        }

        _ptStartCapture = point;
        _ptCurrentHover = point;
        SetCapture();
        _fCapturing = true;
        _fAux = fAux;
    }
}

void CRasterView::_OnCaptureToolButtonUp()
{
    if (_fCapturing)
    {
        if (_currentTool == Select)
        {
            if (_fDraggingSelection)
            {
                // Move the selection rect to its final place:
                CPoint pointDiff = _ptCurrentHover - _ptStartCapture;
                _rectSelection.OffsetRect(pointDiff);

                _fDraggingSelection = false;
            }
            else
            {
                // The user just made a new selection rect.
                // Just set the official _rectSelection.
                // If the user hasn't dragged anywhere, then the _rectSelection will end up
                // being empty.
                _MakeGoodRect(_rectSelection, _ptStartCapture, _ptCurrentHover);
                _StartRubberBandTimer();

                // This is a new selection, it has not been "lifted" off the picture yet.
                _fSelectionLifted = false;
                _fSelectionTransparent = false;
            }
            _InvalidateViewArea();
        }
        else
        {
            // Tools other than selection affect the actual data
            if (_IsPenTool())
            {
                _fPreviewPen = FALSE;
                _CommitSourceData();
            }
            else
            {
                _GrabSourceData();
                _ApplyCaptureTool();
                _CommitSourceData();
            }
        }

        _fCapturing = false;
        ReleaseCapture();
    }
}

BOOL CRasterView::OnDrawCommand(UINT nID)
{
    _OnDrawCommand(_IDToViewTool(nID));
    return TRUE;
}

void CRasterView::_OnDrawCommand(ViewToolType type)
{
    _OnChangeTool(type);
    _UpdateCursor();
    
}

//
// Copies pBitsSelection to the clipboard.  The size is assumed to be the "non-actual"
// size (not aligned to 32 bit boundaries)
//
void CRasterView::_OnCopySelectionBitsToClipboard()
{
    CSize size;
    uint8_t *pBitsSelection = _selectionManager.GetMainSelection(size);
    if (pBitsSelection)
    {
        CWindowDC windowDC(this);
        CDC dcMem;
        if (dcMem.CreateCompatibleDC(&windowDC))
        {
            CBitmap bitmap;
            // Make a temporary buffer, since our selection buffer is "upside down", plus it doesn't include
            // the padding bits at the end (32-bit scanline alignment)
            std::unique_ptr<uint8_t[]> pBitsTemp = std::make_unique<uint8_t[]>(CX_ACTUAL(size.cx) * size.cy);
            // Copy it over, flipping it.
            for (int y = 0; y < size.cy; y++)
            {
                memcpy(pBitsTemp.get() + (size.cy - 1 - y) * CX_ACTUAL(size.cx), pBitsSelection + (y * size.cx), size.cx);
            }

            // Create the bitmap we'll put in the clipboard, and select it into our DC.
            // Useful NUGGET: the dc passed in here must be something like the actual screen DC, or the DC for our
            // hwnd - NOT a memory DC that was created as being "compatible" with the screen DC.  Otherwise you get
            // a monochrome bitmap!
            if (bitmap.CreateCompatibleBitmap(&windowDC, size.cx, size.cy))
            {
                HGDIOBJ hOldBitmap = dcMem.SelectObject(&bitmap);

                // Now copy our DIB into it.
                SCIBitmapInfo bmi(size.cx, size.cy, _palette, _paletteCount);
                StretchDIBits((HDC)dcMem, 0, 0, size.cx, size.cy, 0, 0, size.cx, size.cy, pBitsTemp.get(), &bmi, DIB_RGB_COLORS, SRCCOPY);

                // And now to the clipboard:
                SetClipboardData(CF_BITMAP, (HBITMAP)bitmap.Detach());
                dcMem.SelectObject(hOldBitmap);
            }
        }
    }

    // Also copy to our custom format. We can copy all our selected cels (if there are multiple)
    const auto &allBits = _selectionManager.GetSelectionBits(size);
    std::vector<Cel> selectedCels;
    for (const auto &bits : allBits)
    {
        Cel cel = {};
        selectedCels.push_back(cel);
        selectedCels.back().size = size16((uint16_t)size.cx, (uint16_t)size.cy);
        selectedCels.back().Data.allocate(selectedCels.back().GetDataSize());
        _ConvertSelectionBitsToCelData(bits.get(), size, &selectedCels.back().Data[0]);
    }
    _CopyCelDataToClipboard(selectedCels);
}

// Copy our custom format to the clipboard so we can nicely handle copying among cels/views with the same VGA palette
void CRasterView::_CopyCelDataToClipboard(const std::vector<Cel> &cels)
{
    // Size, data, palettecount, palette
    sci::ostream serial;
    serial << (uint32_t)cels.size();
    for (const Cel &cel : cels)
    {
        SerializeCelRuntime(serial, cel);
    }
    serial << _paletteCount;
    // Now calculate used entries.
    bool used[256];
    vector<const Cel*> celPointersTemp;
    transform(cels.begin(), cels.end(), back_inserter(celPointersTemp), [](const Cel &cel) { return &cel; });
    CountActualUsedColors(celPointersTemp, used);
    for (int i = 0; i < _paletteCount; i++)
    {
        serial << used[i];
    }
    for (int i = 0; i < _paletteCount; i++)
    {
        serial << _palette[i];
    }

    SetClipboardDataFromStream(appState->CelDataClipboardFormat, serial);
}

// If the clipboard data's palette matches our current one, then this returns a Cel (or collection of cels) describing the data.
// We can then apply it to our current selection bits.
unique_ptr<vector<unique_ptr<Cel>>> CRasterView::_GetClipboardDataIfPaletteMatches()
{
    unique_ptr<vector<unique_ptr<Cel>>> celsReturn;
    ProcessClipboardDataIfAvailable(
        appState->CelDataClipboardFormat,
        this,
        [&celsReturn, this](sci::istream &byteStream)
    {
        unique_ptr<vector<unique_ptr<Cel>>> celsTemp = make_unique<vector<unique_ptr<Cel>>>();
        uint32_t celCount;
        byteStream >> celCount;
        for (uint32_t i = 0; i < celCount; i++)
        {
            std::unique_ptr<Cel> cel = std::make_unique<Cel>();
            DeserializeCelRuntime(byteStream, *cel);
            celsTemp->push_back(move(cel));
        }
        int paletteCount;
        byteStream >> paletteCount;
        std::unique_ptr<bool[]> usedEntries = std::make_unique<bool[]>(paletteCount);
        // Only check against palette entries that are actually used. So if a view has the first 64
        // colors in common with another view, and only entries from those 64 colors are used, we
        // will still succeed.
        for (int i = 0; i < paletteCount; i++)
        {
            byteStream >> usedEntries[i];
        }
        if (paletteCount == _paletteCount)
        {
            bool match = true;
            for (int i = 0; match && (i < paletteCount); i++)
            {
                RGBQUAD paletteValue;
                byteStream >> paletteValue;
                if (usedEntries[i])
                {
                    match = (paletteValue.rgbBlue == _palette[i].rgbBlue) &&
                        (paletteValue.rgbGreen == _palette[i].rgbGreen) &&
                        (paletteValue.rgbRed == _palette[i].rgbRed);
                }
            }
            if (match)
            {
                celsReturn = move(celsTemp);
            }
        }
    }
    );
    return celsReturn;
}

void CRasterView::OnCopyPic()
{
    OpenClipboardGuard clipBoard(this);
    if (clipBoard.IsOpen())
    {
        if (EmptyClipboard())
        {
            if (!_rectSelection.IsRectEmpty())
            {
                BOOL fLifted = FALSE;
                if (!_selectionManager.HasSelection())
                {
                    _GrabSourceData(); // Grab new source data since it may be tainted by rubber band.
                    fLifted = TRUE;
                    // Grab some data (but don't clear underneath)
                    _selectionManager.LiftSelection(_rectSelection, _iMainIndex, _cWorkingCels, _celData, FALSE, TRUE);
                }
                if (_selectionManager.HasSelection())
                {
                    _OnCopySelectionBitsToClipboard();
                    if (fLifted)
                    {
                        _selectionManager.ClearSelection();
                    }
                }
            }
            else if (_pbitmapDoubleBuf)
            {
                // Quick and dirty: use the bitmap we generated for double-buffering.
                // We'll lose it to the clipboard, so we detach it.
                SetClipboardData(CF_BITMAP, (HBITMAP)_pbitmapDoubleBuf->Detach());

                // Reset our double buffering, so we regenerate it on the next paint cycle.
                _DestroyDoubleBuffer();

                // Also do this
                const Cel *selectedCel = _GetSelectedCel();
                if (selectedCel)
                {
                    _CopyCelDataToClipboard({ *selectedCel });
                }
            }
        }
    }
}

void CRasterView::OnPaste()
{
    _OnPaste(false, false);
}

void CRasterView::OnPasteTransparent()
{
    _OnPaste(true, false);
}

void CRasterView::OnPasteSpecial()
{
    // We don't need a OnPasteSpecialTransparent, because nothing from the clipboard will ever have alpha.
    _OnPaste(false, true);
}

// Note: this function can result in changes to the sizes in _celData!
void CRasterView::_EnsureCelsLargeEnoughForPaste(size16 requiredSize)
{
    std::vector<size_t> resizeIndices;
    for (size_t i = 0; i < (size_t)_cWorkingCels; i++)
    {
        size16 currentSize = _celData[i].GetSize();
        if ((currentSize.cx < requiredSize.cx) ||
            (currentSize.cy < requiredSize.cy))
        {
            resizeIndices.push_back(i);
        }
    }
    if (!resizeIndices.empty())
    {
        if (IDYES == AfxMessageBox("The pasted image is larger than the cel. Grow the cel so it can fit?", MB_YESNO | MB_ICONWARNING))
        {
            // Delete the entire current cel
            GetDoc()->ApplyChanges<RasterComponent>(
                [&, requiredSize](RasterComponent &raster)
            {
                for (size_t i = 0; i < (size_t)_cWorkingCels; i++)
                {
                    size16 currentSize = _celData[i].GetSize();
                    size16 newSize = { max(currentSize.cx, requiredSize.cx), max(currentSize.cy, requiredSize.cy) };
                    SetSize(raster, _celData[i].GetIndex(), newSize, RasterResizeFlags::Normal);
                }
                return WrapRasterChange(RasterChangeHint::NewView); // Since we changed a lot
            }
            );
        }
    }
}

void CRasterView::_OnPaste(bool fTransparent, bool provideOptions)
{
    bool success = false;
    unique_ptr<vector<unique_ptr<Cel>>> cels = _GetClipboardDataIfPaletteMatches();
    if (cels && !cels->empty() && !provideOptions)
    {
        success = true;
        // All size should be the same if this was generated by us, so just use the first size
        _EnsureCelsLargeEnoughForPaste((*cels)[0]->size);
        vector<const Cel*> celPointers;
        std::transform(cels->begin(), cels->end(), back_inserter(celPointers), [](unique_ptr<Cel> &cel) { return cel.get(); });
        _rectSelection = _selectionManager.PasteCel(celPointers, _cWorkingCels, SizeToCSize(_celData[_iMainIndex].GetSize()));
    }
    else if (IsClipboardFormatAvailable(CF_BITMAP))
    {
        if (_iMainIndex != -1)
        {
            OpenClipboardGuard guard(this);
            if (guard.IsOpen())
            {
                HBITMAP hBmp = (HBITMAP)GetClipboardData(CF_BITMAP);
                std::unique_ptr<Gdiplus::Bitmap> pBitmap(Gdiplus::Bitmap::FromHBITMAP(hBmp, nullptr));
                if (pBitmap)
                {
                    size16 imageSize((uint16_t)pBitmap->GetWidth(), (uint16_t)pBitmap->GetHeight());
                    _EnsureCelsLargeEnoughForPaste(imageSize);

                    if (provideOptions)
                    {

                        // Invoke the import image dialog to offer options
                        PaletteComponent anyPalette;
                        int colorCount;
                        bool fixedPalette;
                        uint8_t transparentColor = _celData[_iMainIndex].GetTransparentColor();
                        GetDoc()->PrepareEGAOrVGAPalette(anyPalette, colorCount, fixedPalette);
                        // Invoke the Import Image dialog
                        CBitmapToVGADialog dialog(
                            move(pBitmap),
                            nullptr,
                            &anyPalette,
                            fixedPalette,
                            colorCount,
                            false,
                            imageSize,
                            transparentColor,
                            PaletteAlgorithm::MatchExisting,    // REQUIRED for fixed palette. So be careful if changing this.
                            DefaultPaletteUsage::UsedColors,
                            "Paste image");

                        if (dialog.DoModal() == IDOK)
                        {
                            auto &finalCel = dialog.GetFinalResult();
                            auto finalPalette = dialog.GetFinalResultPalette();

                            // If the user changed the palette, we'll need to add one.
                            const PaletteComponent *optionalNewPalette = nullptr;
                            if (!fixedPalette && (*finalPalette != anyPalette))
                            {
                                optionalNewPalette = finalPalette.get();
                            }

                            // The user may have changed the transparent color too.
                            uint8_t newTransparentColor = finalCel->TransparentColor;
                            GetDoc()->ApplyChanges<RasterComponent>(
                                [&, optionalNewPalette, newTransparentColor, transparentColor](RasterComponent &raster)
                            {
                                RasterChangeHint hint = RasterChangeHint::None;
                                if (newTransparentColor != transparentColor)
                                {
                                    hint |= RasterChangeHint::NewView;
                                    SetGroupTransparentColor(raster, _cWorkingCels, _rgdwGroups, newTransparentColor);
                                }
                                if (optionalNewPalette)
                                {
                                    hint |= RasterChangeHint::NewView;
                                }
                                return WrapRasterChange(hint);
                            },
                                // Apply the new palette if needed.
                                [optionalNewPalette](ResourceEntity &resource)
                            {
                                if (optionalNewPalette)
                                {
                                    AfxMessageBox("A new palette is being applied to this entire view.", MB_OK | MB_ICONWARNING);
                                    resource.AddComponent<PaletteComponent>(std::make_unique<PaletteComponent>(*optionalNewPalette));
                                }
                            }
                            );

                            success = true;
                            _rectSelection = _selectionManager.PasteCel(vector<const Cel*> {finalCel.get()}, _cWorkingCels, SizeToCSize(_celData[_iMainIndex].GetSize()));
                        }
                    }
                    else
                    {
                        // Just paste in without allowing the user any options
                        pBitmap->RotateFlip(Gdiplus::RotateNoneFlipY);

                        bool usableColors[256];
                        assert(_paletteCount <= ARRAYSIZE(usableColors));
                        for (size_t i = 0; i < (size_t)_paletteCount; i++)
                        {
                            // The ones marked as "fixed" are usable
                            usableColors[i] = (_palette[i].rgbReserved != 0);
                        }

                        // Pasted images from external source will not have any transparency. The question is:
                        // do we include the view cel's current transparent color in the colors we map to?
                        // If we do, then it breaks people trying to paste from a color key VGA editor.
                        // If we don't, then people paste from other sources might get surprise transparent areas in their images, and
                        // they'll need to search for another color to use as a Tx color.
                        // Here's what we'll do:
                        //  - first try it without excluding the Tx color.
                        //  - if nothing mapped to the Tx color, we're done. If something did, then ask if they want to try again.
                        BitmapConvertStatus convertStatus = BitmapConvertStatus::None;
                        std::unique_ptr<Cel> finalResult =
                            GdiPlusBitmapToCel(
                            *pBitmap,
                            false,  // No dither
                            true,   // gamma corrected
                            DitherAlgorithm::None,  // No alpha dither
                            ColorMatching::RGB,
                            128,    // alpha threshold
                            _celData[_iMainIndex].GetTransparentColor(),
                            false,   // Allow the transparent color.
                            _paletteCount,
                            usableColors,
                            _palette,
                            g_vgaPaletteMapping,
                            convertStatus
                            );
                        if (IsFlagSet(convertStatus, BitmapConvertStatus::MappedToTransparentColor))
                        {
                            vector<pair<int, string>> buttons = { { MessageBoxCustomization::Yes, "Try again" }, { MessageBoxCustomization::No, "This is OK"} };
                            MessageBoxCustomization custom(buttons);
                            string message = fmt::format("Some of the pasted image's colors mapped to the current view cel's transparent color ({0}). These areas will appears as transparent.\nTry again without mapping to the transparent color?", (uint32_t)_celData[_iMainIndex].GetTransparentColor());
                            if (IDYES == AfxMessageBox(message.c_str(), MB_YESNO))
                            {
                                finalResult =
                                    GdiPlusBitmapToCel(
                                    *pBitmap,
                                    false,  // No dither
                                    true,   // gamma corrected
                                    DitherAlgorithm::None,  // No alpha dither
                                    ColorMatching::RGB,
                                    128,    // alpha threshold
                                    _celData[_iMainIndex].GetTransparentColor(),
                                    true,   // Now exclude the Tx color
                                    _paletteCount,
                                    usableColors,
                                    _palette,
                                    g_vgaPaletteMapping,
                                    convertStatus
                                    );
                            }

                        }

                        if (finalResult)
                        {
                            success = true;
                            _rectSelection = _selectionManager.PasteCel(vector<const Cel*> {finalResult.get()} , _cWorkingCels, SizeToCSize(_celData[_iMainIndex].GetSize()));
                        }
                    }
                }
            }
        }
    }
    if (success && !_rectSelection.IsRectEmpty())
    {
        _fSelectionTransparent = fTransparent;
        _fSelectionLifted = true;
        // And finally change to the selection tool
        _OnChangeTool(Select);
        // And really finally, invalidate our canvas
        _InvalidateViewArea();
        _StartRubberBandTimer();
    }
}

void CRasterView::OnUpdateAllViewCommands(CCmdUI *pCmdUI)
{
    // Most commands are enabled by default.
    switch (pCmdUI->m_nID)
    {
    case ID_SETPLACEMENT:
        {
            CNewRasterResourceDocument *pDoc = static_cast<CNewRasterResourceDocument*>(GetDocument());
            pCmdUI->Enable(pDoc &&
                IsFlagSet(pDoc->GetComponent<RasterComponent>().Traits.Caps, RasterCaps::Placement | RasterCaps::SCI0CursorPlacement)
                );
        }
        break;
    default:
        pCmdUI->Enable(TRUE);
        break;
    }

    // Which one is checked though?
    ViewToolType tool = _IDToViewTool(pCmdUI->m_nID);
    int iCheck = (tool == _currentTool);
    pCmdUI->SetCheck(iCheck);
}


//
// Deletes any current selection and copies it to the clipboard
//
void CRasterView::OnCut()
{
    if (!_fSizerCapturing && !_rectSelection.IsRectEmpty())
    {
        _OnCutOrDeleteSelection(TRUE);
    }
}

//
// If fCut is TRUE, then the selection is removed and copied to the clipboard.
// Otherwise, it is just removed.
//
void CRasterView::_OnCutOrDeleteSelection(BOOL fCut)
{
    // Copy over the current selection.
    _GrabSourceData();
    _selectionManager.LiftSelection(_rectSelection, _iMainIndex, _cWorkingCels, _celData, TRUE, fCut);
    _CommitSourceData();

    CSize sizeOut;
    uint8_t *pBitsMainSelection = _selectionManager.GetMainSelection(sizeOut);
    if (pBitsMainSelection)
    {
        // Copy this to the clipboard.
        OpenClipboardGuard clipBoard(this);
        if (clipBoard.IsOpen())
        {
            if (EmptyClipboard())
            {
                _OnCopySelectionBitsToClipboard();
            }
        }
    }

    // And clean up our selection state.
    _GetRidOfSelection();
}

//
// Deletes the current selection, or the whole cel.
//
void CRasterView::OnDelete()
{
    if (!_fSizerCapturing)
    {
        CNewRasterResourceDocument *pDoc = static_cast<CNewRasterResourceDocument*>(GetDocument());
        if (pDoc)
        {
            if (_rectSelection.IsRectEmpty())
            {
                // Delete the entire current cel
                pDoc->ApplyChanges<RasterComponent>(
                    [&](RasterComponent &raster)
                {
                    return WrapRasterChange(RemoveCel(raster, pDoc->GetSelectedIndex()));
                }
                );
            }
            else
            {
                _OnCutOrDeleteSelection(FALSE);
            }
        }
    }
}

void CRasterView::OnSelectAll()
{
    if (_rectSelection.IsRectEmpty())
    {
        CPoint pt1(0, 0);
        CPoint pt2(_sizeView.cx, _sizeView.cy);
        _MakeGoodRect(_rectSelection, pt1, pt2);
        // Change to the selection tool
        _OnChangeTool(Select);
        // And really finally, invalidate our canvas
        _InvalidateViewArea();
        _StartRubberBandTimer();
    }
}

void CRasterView::OnUpdateSelectAll(CCmdUI *pCmdUI)
{
    // Enable this if we don't already have a selection.
    pCmdUI->Enable(_rectSelection.IsRectEmpty());
}

void CRasterView::OnDither()
{
    _fDithered = !_fDithered;
}

void CRasterView::OnUpdateDither(CCmdUI *pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetCheck(_fDithered);
}

void CRasterView::OnUpdateEGAOnly(CCmdUI *pCmdUI)
{
    const RasterComponent *raster = _GetRaster();
    pCmdUI->Enable(raster && (raster->Traits.PaletteType != PaletteType::VGA_256));
}

void CRasterView::OnUpdateIsVGA(CCmdUI *pCmdUI)
{
    const RasterComponent *raster = _GetRaster();
    pCmdUI->Enable(raster && (raster->Traits.PaletteType == PaletteType::VGA_256));
}

void CRasterView::OnUpdateAlwaysOn(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(TRUE);
}

void CRasterView::OnUpdateLeftOnion(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck((GetDoc() && GetDoc()->GetOnionSkin().Left.Enabled) ? BST_CHECKED : BST_UNCHECKED);
}

void CRasterView::OnUpdateRightOnion(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(TRUE);
    pCmdUI->SetCheck((GetDoc() && GetDoc()->GetOnionSkin().Right.Enabled) ? BST_CHECKED : BST_UNCHECKED);
}

void CRasterView::ToggleLeftOnion()
{
    if (GetDoc())
    {
        GetDoc()->GetOnionSkin().Left.Enabled = !GetDoc()->GetOnionSkin().Left.Enabled;
        _InvalidateViewArea();
    }
}
void CRasterView::ToggleRightOnion()
{
    if (GetDoc())
    {
        GetDoc()->GetOnionSkin().Right.Enabled = !GetDoc()->GetOnionSkin().Right.Enabled;
        _InvalidateViewArea();
    }
}
void CRasterView::OnOnionSkinSettings()
{
    OnionSkinSettingsDialog dialog(*this);
    dialog.DoModal();
}

void CRasterView::OnRotate90CW()
{
    _OnRotate(90);
}
void CRasterView::OnRotate90CCW()
{
    _OnRotate(-90);
}
void CRasterView::OnRotate180()
{
    _OnRotate(180);
}

// Store the most recently used rotation for convenience.
int static defaultRotation = 45;

void CRasterView::OnRotateArbitrary()
{
    RotateArbitraryDialog dialog(defaultRotation);
    if (IDOK == dialog.DoModal())
    {
        _OnRotate(dialog.GetDegrees());
        defaultRotation = dialog.GetDegrees();
    }
}

void CRasterView::_OnRotate(int degrees)
{
    GetDoc()->ApplyChanges<RasterComponent>(
        [this, degrees](RasterComponent &raster)
    {
        return WrapRasterChange(
            RotateGroup(raster, _cWorkingCels, _rgdwGroups, degrees)
        );
    }
    );
}

void CRasterView::OnUpdateHasVGAPalette(CCmdUI *pCmdUI)
{
    BOOL enable = FALSE;
    if (GetDoc())
    {
        const ResourceEntity *pResource = GetDoc()->GetResource();
        enable = (pResource && pResource->TryGetComponent<PaletteComponent>());
    }
    pCmdUI->Enable(enable);
}

void CRasterView::OnUpdateDelete(CCmdUI *pCmdUI)
{
    BOOL fCanDelete = FALSE;
    CNewRasterResourceDocument *pDoc = GetDoc();
    if (pDoc)
    {
        fCanDelete = pDoc->CanDeleteCels();
    }
    pCmdUI->Enable(fCanDelete);
}

void CRasterView::OnIndicatorPixelColor(CCmdUI *pCmdUI)
{
    TCHAR szBuf[100];
    szBuf[0] = 0;
    // Show the pixel color if in range
    if ((_ptCurrentHover.x < _sizeView.cx) && (_ptCurrentHover.y < _sizeView.cy))
    {
        const RasterComponent *pRaster = _GetRaster();
        const Cel *pCel = _GetSelectedCel();
        if (pCel && pRaster)
        {
            uint8_t color = pCel->Data[_ptCurrentHover.x + (pCel->size.cy - _ptCurrentHover.y - 1) * CX_ACTUAL(_sizeView.cx)];
            RGBQUAD rgb;
            if (pRaster->Traits.PaletteType == PaletteType::VGA_256)
            {
                rgb = _palette[color];
            }
            else
            {
                rgb = g_egaColors[color % 16];
            }

            StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("Color: %03d (R=%d, G=%d, B=%d)"), color, rgb.rgbRed, rgb.rgbGreen, rgb.rgbBlue);
        }
    }

    pCmdUI->Enable();
    pCmdUI->SetText(szBuf);
}

void CRasterView::OnIndicatorRasterSize(CCmdUI *pCmdUI)
{
    TCHAR szBuf[100];
    if (_fSizerCapturing)
    {
        // Show the resized size
        StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("Dimensions: %3dx%3d"), _sizeNew.cx, _sizeNew.cy);
    }
    else
    {
        // Show the dimensions of the thing
        StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("Dimensions: %3dx%3d"), _sizeView.cx, _sizeView.cy);
    }
    pCmdUI->Enable();
    pCmdUI->SetText(szBuf);
}

void CRasterView::OnIndicatorCoords(CCmdUI *pCmdUI)
{
    TCHAR szBuf[100];
    // Show the mouse coords if in range
    if ((_ptCurrentHover.x < _sizeView.cx) && (_ptCurrentHover.y < _sizeView.cy))
    {
        StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("Coords: (%3d,%3d)"), _ptCurrentHover.x, _ptCurrentHover.y);
    }
    else
    {
        szBuf[0] = 0;
    }
    pCmdUI->Enable();
    pCmdUI->SetText(szBuf);
}

CRect CRasterView::_GetEffectArea(int nCel) const
{
    CSize sizeView = _celData[nCel].GetCSize();
    if (_rectSelection.IsRectEmpty())
    {
        // The effect applies to all
        return CRect(0, 0, sizeView.cx, sizeView.cy);
    }
    // Just the selection
    CRect rectReturn = _rectSelection;
    // Clip it to the cel size.
    rectReturn.bottom = min(rectReturn.bottom, sizeView.cy);
    rectReturn.right = min(rectReturn.right, sizeView.cx);
    return rectReturn;
}

void CRasterView::OnFlipHorz()
{
    _GrabSourceData();
    for (int i = 0; i < _cWorkingCels; i++)
    {
        CRect rectEffect = _GetEffectArea(i);
        CSize sizeView = _celData[i].GetCSize();
        int top = sizeView.cy - rectEffect.bottom;
        int bottom = sizeView.cy - rectEffect.top;
        for (int y = top; y < bottom; y++)
        {
            for (int x = 0; x < rectEffect.Width() / 2; x++)
            {
                uint8_t *pLine = _ViewOffset(i, y) + rectEffect.left;
                int xRight = rectEffect.Width() - x - 1;
                int xLeft = x;
                uint8_t bTemp = pLine[x];
                pLine[x] = pLine[xRight];
                pLine[xRight] = bTemp;
            }
        }
    }
    _CommitSourceData();
}

void CRasterView::OnShiftLeft()
{
    _OnShift(-1, 0);
}

void CRasterView::OnShiftRight()
{
    _OnShift(1, 0);
}

void CRasterView::OnShiftUp()
{
    _OnShift(0, -1);
}

void CRasterView::OnShiftDown()
{
    _OnShift(0, 1);
}


void CRasterView::_OnShift(int dx, int dy)
{
    _GrabSourceData();
    for (int i = 0; i < _cWorkingCels; i++)
    {
        CRect rectEffect = _GetEffectArea(i);
        CSize sizeView = _celData[i].GetCSize();

        // Copy into a temp buffer
        CelData tempData(_celData[i]);
        int cx = _celData[i].Width();
        int cy = _celData[i].Height();
        for (int y = rectEffect.top; y < rectEffect.bottom; y++)
        {
            for (int x = rectEffect.left; x < rectEffect.right; x++)
            {
                int xDest = x + dx;
                if (xDest >= rectEffect.right)
                {
                    xDest -= rectEffect.Width();
                }
                if (xDest < rectEffect.left)
                {
                    xDest += rectEffect.Width();
                }
                int yDest = y + dy;
                if (yDest >= rectEffect.bottom)
                {
                    yDest -= rectEffect.Height();
                }
                if (yDest < rectEffect.top)
                {
                    yDest += rectEffect.Height();
                }
                uint8_t *dest = tempData.GetDataPtr() + CX_ACTUAL(cx) * (cy - yDest - 1) + xDest;
                uint8_t *source = _celData[i].GetDataPtr() + CX_ACTUAL(cx) * (cy - y - 1) + x;
                *dest = *source;
            }
        }
        // Copy back
        _celData[i] = tempData;
    }
    _CommitSourceData();

}

void CRasterView::OnFlipVert()
{
    _GrabSourceData();
    for (int i = 0; i < _cWorkingCels; i++)
    {
        CRect rectEffect = _GetEffectArea(i);
        CSize sizeView = _celData[i].GetCSize();
		std::unique_ptr<uint8_t[]> pTemp = std::make_unique<uint8_t[]>(sizeView.cx);
        int bottom = rectEffect.Height() / 2 + rectEffect.top;
        for (int y = rectEffect.top; y < bottom; y++)
        {
            int yTop = sizeView.cy - y - 1;
            int yBottom = sizeView.cy - (rectEffect.bottom - (y - rectEffect.top) - 1) - 1;

            assert(yBottom < sizeView.cy);
            assert(y < sizeView.cy);

            memcpy(pTemp.get(), _ViewOffset(i, yTop) + rectEffect.left, rectEffect.Width());
            memcpy(_ViewOffset(i, yTop) + rectEffect.left, _ViewOffset(i, yBottom) + rectEffect.left, rectEffect.Width());
            memcpy(_ViewOffset(i, yBottom) + rectEffect.left, pTemp.get(), rectEffect.Width());
        }
    }
    _CommitSourceData();
}

void CRasterView::OnInvert()
{
    // TODO: Use effect area, instead of all.
    _GrabSourceData();
    for (int i = 0; i < _cWorkingCels; i++)
    {
        CSize sizeCel = _celData[i].GetCSize();
        for (int y = 0; y < sizeCel.cy; y++)
        {
            uint8_t *pLine = _ViewOffset(i, y);
            for (int x = 0; x < sizeCel.cx; x++)
            {
                assert(pLine[x] <= 0xf);
                pLine[x] = 0xf - pLine[x];
            }
        }
    }
    _CommitSourceData();
}

void CRasterView::OnGreyScale()
{
    // TODO: Use effect area.
    _GrabSourceData();
    for (int i = 0; i < _cWorkingCels; i++)
    {
        CSize sizeCel = _celData[i].GetCSize();
        for (int y = 0; y < sizeCel.cy; y++)
        {
            uint8_t *pLine = _ViewOffset(i, y);
            for (int x = 0; x < sizeCel.cx; x++)
            {

                assert((pLine + x) <
                    (_celData[i].GetDataPtr() + CX_ACTUAL(sizeCel.cx) * sizeCel.cy));
                switch (pLine[x])
                {
                case 0xe:
                case 0xd:
                case 0xc:
                case 0xb:
                    // Light grey
                    pLine[x] = 0x7;
                    break;
                case 0x0:
                case 0xf:
                case 0x7:
                case 0x8:
                    // No change
                    break;
                default:
                    // Dark grey
                    pLine[x] = 0x8;
                    break;
                }
            }
        }
    }
    _CommitSourceData();
}

void CRasterView::OnRButtonDown(UINT nFlags, CPoint point)
{
    CPoint ptView = _MapClientPointToPic(point);

    // Don't do anything if this is off the picture.
    if (!_PointInView(ptView))
    {
        return;
    }

    switch(_currentTool)
    {
    case Zoom:
        _OnZoomRClick();
        break;

    case Fill:
        _OnFillLClick(ptView, false);
        break;

    case EyeDropper:
    case SetTransparency:
        _OnEyeDrop(ptView, false, (_currentTool == SetTransparency));
        break;

    default:
        {
            if (_IsCaptureTool())
            {
                _OnCaptureToolButtonDown(ptView, false);
            }
            break;
        }
    }
}

void CRasterView::OnLButtonDown(UINT nFlags, CPoint point)
{
    CPoint ptView = _MapClientPointToPic(point);

    if (!_PointInView(ptView))
    {
        // Grab the sizers.
        // Note: this isn't quite correct.  The danger is that the sizer grips will
        // fall into the view area and thus be inaccessible.  However, as soon as they
        // do that, the bitmap should be re-sized.  So this shouldn't ever happen.
        OnSizerType onSizerType = _OnSizer(point);
        if ((onSizerType != None) && _CanResize())
        {
            // The user pressed down on a sizer.
            _ptStartSizerCapture = ptView;
            _fSizerCapturing = true;
            _LockDoc(true);
            _onSizerType = onSizerType;
            SetCapture();
            _sizeOrig = _sizeNew;

            // Get rid of any selection that currently exists.
            _GetRidOfSelection();
        }
    }
    else
    {
        switch(_currentTool)
        {
        case Zoom:
            _OnZoomLClick();
            break;

        case Fill:
            _OnFillLClick(ptView, true);
            break;

        case SetPlacement:
            _OnSetPlacementLClick(ptView);
            break;

        case EyeDropper:
        case SetTransparency:
            _OnEyeDrop(ptView, true, (_currentTool == SetTransparency));
            break;

        default:
            if (_IsCaptureTool())
            {
                _OnCaptureToolButtonDown(ptView, true);
            }
            break;
        }
    }
}

void CRasterView::OnLButtonUp(UINT nFlags, CPoint point)
{
    CPoint ptView = _MapClientPointToPic(point);

    if (_fSizerCapturing)
    {
        _fSizerCapturing = false;
        _LockDoc(false);
        ReleaseCapture();

        // If the size changed...
        if (_sizeNew != _sizeView)
        {
            // Commit any selection bits now, before we resize.
            _CommitSelectionBits();

            // Make the new size be the actual size.
            CNewRasterResourceDocument *pDoc = GetDoc();
            if (pDoc)
            {
                // Size multiple cels relatively, not absolutely.
                // FEATURE: if we're stretching (VK_SHIFT), then maybe the sizeRelative should
                // scale according to the cel's size
                size16 sizeRelative = _sizeNew - _sizeView;
                std::vector<size16> rgSizes(_cWorkingCels); // Temp buffer for sizes
                for (int i = 0; i < _cWorkingCels; i++)
                {
                    rgSizes[i] = _celData[i].GetSize() + sizeRelative;
                }

                // If the ctrl-key is held down, anchor on the bottom right.
                RasterResizeFlags flags = RasterResizeFlags::Normal;
                if (GetKeyState(VK_CONTROL) & 0x8000)
                {
                    flags = RasterResizeFlags::AnchorBottomRight;
                }
                else if (GetKeyState(VK_SHIFT)& 0x8000)
                {
                    flags = RasterResizeFlags::Stretch;
                }

                pDoc->ApplyChanges<RasterComponent>(
                    [&](RasterComponent &raster)
                {
                    return WrapRasterChange(SetGroupSize(raster, _cWorkingCels, _rgdwGroups, &rgSizes[0], (RasterResizeFlags)flags));
                }
                    );
                // This should automatically cause an update that will set our _sizeView variables.
            }
        }
    }
    else
    {
        if (_IsCaptureTool())
        {
            _OnCaptureToolButtonUp();
        }
    }
}

#ifdef FORCE_REDRAW
void CRasterView::OnSetFocus(CWnd *pOldWnd)
{
	RedrawWindow(nullptr, nullptr, RDW_UPDATENOW);
}
#endif

void CRasterView::OnKillFocus(CWnd *pNewWnd)
{
    if (_fSizerCapturing)
    {
        _fSizerCapturing = false;
        _LockDoc(false);
        // Reset our size change:
        _sizeNew = _sizeView;
        ReleaseCapture();
        Invalidate(FALSE);
    }
    if (_fCapturing)
    {
        _fDraggingSelection = false;
        _fCapturing = false;
        ReleaseCapture();
        _InvalidateViewArea();
    }
}

BOOL CRasterView::OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message)
{
    _UpdateCursor();
    return TRUE;
}

void CRasterView::OnRButtonUp(UINT nFlags, CPoint point)
{
    CPoint ptView = _MapClientPointToPic(point);
    if (_IsCaptureTool())
    {
        _OnCaptureToolButtonUp();
    }
}

void CRasterView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    if (_currentTool == Zoom)
    {
        // Don't do anything if this is off the picture.
        if (_PointInView(_MapClientPointToPic(point)))
        {
            _OnZoomLClick();
        }
    }
    else if (_currentTool == Select)
    {
        // If someone double clicks on a selection rect (or anywhere) while there
        // is selection, commit the bits.
        _CommitSelectionBits();
    }
    else if (_currentTool == Replace)
    {
        // The replace colour command - double clicking replaces all colour in the cel
        _GrabSourceData();
        _OnReplaceTool(_MapClientPointToPic(point), TRUE, true);
        _CommitSourceData();
    }
}

void CRasterView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
    if (_currentTool == Zoom)
    {
        // Don't do anything if this is off the picture.
        if (_PointInView(_MapClientPointToPic(point)))
        {
            _OnZoomRClick();
        }
    }
    else if (_currentTool == Replace)
    {
        // The replace colour command - double clicking replaces all colour in the cel
        _GrabSourceData();
        _OnReplaceTool(_MapClientPointToPic(point), TRUE, false);
        _CommitSourceData();
    }
}

//
// This ensures the point is within the bounds of the view.
// Using GDI convention, this is actually allows one pixel below
// and to the right of the view bounds.
//
void CRasterView::_EnsurePointWithinBounds(CPoint &pointView)
{
    pointView.x = min(pointView.x, _sizeView.cx);
    pointView.y = min(pointView.y, _sizeView.cy);
    pointView.x = max(pointView.x, 0);
    pointView.y = max(pointView.y, 0);
}

void CRasterView::_EnsureRectWithinBounds(CRect &rect)
{
    int cx = rect.Width();
    int cy = rect.Height();
    assert(cx <= _sizeView.cx);
    assert(cy <= _sizeView.cy);

    if (rect.left < 0)
    {
        rect.left = 0;
        rect.right = rect.left + cx;
    }
    else if (rect.left > _sizeView.cx)
    {
        rect.right = _sizeView.cx;
        rect.left = _sizeView.cx - cx;
    }
    if (rect.top < 0)
    {
        rect.top = 0;
        rect.bottom = rect.top + cy;
    }
    else if (rect.left > _sizeView.cy)
    {
        rect.bottom = _sizeView.cy;
        rect.top = rect.bottom - cy;
    }
}

void _ValidateSize(size16 &size)
{
    size.cx = max(1, size.cx);
    size.cy = max(1, size.cy);
}

void CRasterView::OnMouseMove(UINT nFlags, CPoint point)
{
    // Adjust to pic coords.
    _ptCurrentHoverClient = point;
    _ptCurrentHover = _MapClientPointToPic(point);

    if (_fSizerCapturing)
    {
        // We're in capture.  Figure out the diff.
        assert(_onSizerType != None);
        switch (_onSizerType)
        {
        case Diagonal:
        case Horizontal:
            _sizeNew.cx = (uint16_t)max(0, _ptCurrentHover.x - _ptStartSizerCapture.x + _sizeOrig.cx);
            if (_onSizerType == Horizontal)
            {
                break;
            }
        case Vertical:
            _sizeNew.cy = (uint16_t)max(0, _ptCurrentHover.y - _ptStartSizerCapture.y + _sizeOrig.cy);
            break;
        }

        _ValidateSize(_sizeNew);

        Invalidate(FALSE);
    }
    else
    {
        // Figure out which sizer we're hovered over (if any)
        OnSizerType onSizerType = _OnSizer(point);
        if (onSizerType != _onSizerType)
        {
            _onSizerType = onSizerType;
            // TODO -> invalidate if we want hover effects
        }

        if (_fCapturing)
        {
            // Ensure _ptCurrentHover is within bounds.
            _EnsurePointWithinBounds(_ptCurrentHover);

            // FEATURE: we could be more selective with this too
            // Just invalidate the region that covers our from-to
            // CPU spikes to 100% when moving around here.
            _InvalidateViewArea();

            // Note: we're not invalidating the bitmap here...

            if (_fDraggingSelection && !_fSelectionLifted)
            {
                CPoint pointDiff = _ptCurrentHover - _ptStartCapture;
                if ((pointDiff.x != 0) || (pointDiff.y != 0))
                {
                    // The user has moved the selection at least one pixel.
                    _LiftSelection();
                }
            }

            if (_IsPenTool())
            {
                _OnPenClick(_ptCurrentHover, _fAux);
                _InvalidateViewArea();
            }
        }
    }
    _UpdateCursor();
}

void CRasterView::_OnMouseWheel(UINT nFlags, BOOL fForward, CPoint pt, short nNotches)
{
    // This is cool.
    if (nFlags & MK_CONTROL)
    {
        // Shift held down.  Do a zoom:
        if (fForward)
        {
            _OnZoomLClick();
        }
        else
        {
            _OnZoomRClick();
        }
    }
}

CRasterView::OnSizerType CRasterView::_OnSizer(CPoint &ptClient)
{
    CSize sizeNewZoom;
    sizeNewZoom.cx = _sizeNew.cx * _iZoomFactor;
    sizeNewZoom.cy = _GetDragScreenHeight();
    CRect rectVert;
    _GetSizerRectVert(sizeNewZoom, rectVert);
    CRect rectHorz;
    _GetSizerRectHorz(sizeNewZoom, rectHorz);
    CRect rectDiag1;
    _GetSizerRectDiag1(sizeNewZoom, rectDiag1);
    CRect rectDiag2;
    _GetSizerRectDiag1(sizeNewZoom, rectDiag2);

    if (PtInRect(&rectVert, ptClient))
    {
        return Vertical;
    }
    else if (PtInRect(&rectHorz, ptClient))
    {
        return Horizontal;
    }
    else if (PtInRect(&rectDiag1, ptClient) || PtInRect(&rectDiag2, ptClient))
    {
        return Diagonal;
    }
    return None;
}

void CRasterView::_CheckControlShift(UINT nChar)
{
    // REVIEW: this should go in the frame, if we want it to work if the view doesn't have focus.
    if ((nChar == VK_CONTROL) || (nChar == VK_SHIFT))
    {
        _UpdateCursor();
    }
}

void CRasterView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    _CheckControlShift(nChar);
    __super::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CRasterView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    _CheckControlShift(nChar);
    if (GetDoc())
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
        case VK_OEM_MINUS:
        case VK_OEM_PLUS:
            if (_fDithered)
            {
                _ditherStrength += (nChar == VK_OEM_PLUS) ? 1 : -1;
                _ditherStrength = max(1, _ditherStrength);
                _ditherStrength = min(15, _ditherStrength);
                _InitPatternBrush();
            }
            break;
        default:
            {
                __super::OnKeyDown(nChar, nRepCnt, nFlags);
            }
        }

        if ((point.x != 0) || (point.y != 0))
        {
            GetDoc()->MoveSelectedCel(point, true);
        }
    }
}

//
// Prevent flicker
//
BOOL CRasterView::OnEraseBkgnd(CDC *pDC)
{
    return TRUE;
}


//
// This one takes into account origins
//
CPoint CRasterView::_MapClientPointToPic(CPoint ptScreen)
{
    return CPoint((ptScreen.x + _xOrigin) * _sizeView.cx / _cxViewZoom,
        (ptScreen.y + _yOrigin) * _sizeView.cy / _GetViewScreenHeight());
}

void CRasterView::EditVGAPalette()
{
    CNewRasterResourceDocument *pDoc = GetDoc();
    if (pDoc)
    {
        pDoc->SwitchToEmbeddedPalette();
        const ResourceEntity *pResource = GetDoc()->GetResource();
        if (pResource)
        {
            const RasterComponent &raster = pResource->GetComponent<RasterComponent>();
            if (raster.Traits.PaletteType == PaletteType::VGA_256)
            {
                std::vector<const Cel*> cels;
                for (const Loop &loop : raster.Loops)
                {
                    for (const Cel &cel : loop.Cels)
                    {
                        cels.push_back(&cel);
                    }
                }

                const PaletteComponent *paletteToAdd = nullptr;
                const PaletteComponent *palette = pResource->TryGetComponent<PaletteComponent>();
                if (palette == nullptr)
                {
                    paletteToAdd = pDoc->GetCurrentPaletteComponent();
                    if (paletteToAdd)
                    {
                        if (IDYES == AfxMessageBox("There is no palette embedded in this view. Would you like to add one?", MB_YESNO))
                        {
                            palette = paletteToAdd;
                        }
                    }
                }

                if (palette)
                {
                    // We invoke the dialog in ApplyChanges, with a pre-function that optionally adds a palette.
                    GetDoc()->ApplyChanges<PaletteComponent>(
                        [this, &cels, paletteToAdd](PaletteComponent &palette)
                    {
                        PaletteComponent orig = palette;
                        RasterChangeHint hint = RasterChangeHint::None;
                        PaletteEditorDialog paletteEditor(this, palette, cels, true);
                        if (IDOK == paletteEditor.DoModal())
                        {
                            PaletteComponent paletteResult = paletteEditor.GetPalette();
                            // If we were told to add a palette, or the palette is different, then this is actually a change.
                            if (paletteToAdd || (paletteResult != orig))
                            {
                                hint |= RasterChangeHint::NewView | RasterChangeHint::PaletteChoice;
                            }
                        }
                        return WrapHint(hint);
                    },
                        [paletteToAdd](ResourceEntity &resource)    // pre-function
                    {
                        if (paletteToAdd)
                        {
                            resource.AddComponent(std::make_unique<PaletteComponent>(*paletteToAdd));
                        }
                    },
                        true    // Preview
                        );


                    if (paletteToAdd)
                    {
                        GetDoc()->SetPaletteChoice(0, true);    // Switch to embedded palette.
                        GetDoc()->RefreshPaletteOptions();
                    }
                    else
                    {
                        GetDoc()->SetPaletteChoice(GetDoc()->GetPaletteChoice(), true);
                    }
                }
            }
        }
    }
}

void _RemapPaletteWorker(RasterComponent &raster, const PaletteComponent &from, const PaletteComponent &to)
{
    int nLoop = 0;
    for (Loop &loop : raster.Loops)
    {
        if (!loop.IsMirror)
        {
            for (Cel &cel : loop.Cels)
            {
                ConvertCelToNewPalette(cel, from, cel.TransparentColor, false, 256, to.Mapping, to.Colors);
            }
            UpdateMirrors(raster, nLoop);
        }
        nLoop++;
    }
}

void CRasterView::RemapPalette()
{
    CNewRasterResourceDocument *pDoc = GetDoc();
    if (pDoc)
    {
        pDoc->ApplyChanges<RasterComponent>(
            [](RasterComponent &raster)
        {
            RasterChangeHint hint = RasterChangeHint::None;
            ConvertFromToPaletteDialog remapPaletteDialog;
            if (IDOK == remapPaletteDialog.DoModal())
            {
                std::unique_ptr<ResourceEntity> fromPalette = appState->GetResourceMap().CreateResourceFromNumber(ResourceType::Palette, remapPaletteDialog.From);
                std::unique_ptr<ResourceEntity> toPalette = appState->GetResourceMap().CreateResourceFromNumber(ResourceType::Palette, remapPaletteDialog.To);
                _RemapPaletteWorker(raster, fromPalette->GetComponent<PaletteComponent>(), toPalette->GetComponent<PaletteComponent>());
                hint |= RasterChangeHint::NewView;
            }
            return WrapHint(hint);
        });
        
        // TODO: switch to new palette in document.
    }
}

void DoNothing(ResourceEntity &resource);

void CRasterView::ShiftColors()
{
    CNewRasterResourceDocument *pDoc = GetDoc();
    if (pDoc)
    {
        const PaletteComponent *paletteComponent = pDoc->GetCurrentPaletteComponent();
        std::unique_ptr<PaletteComponent> egaPalette;
        int paletteCount = _paletteCount;
        if (!paletteComponent)
        {
            egaPalette = std::make_unique<PaletteComponent>();
            memcpy(egaPalette->Colors, _palette, _paletteCount * sizeof(*_palette));
            paletteComponent = egaPalette.get();
        }
        pDoc->PreviewChanges<RasterComponent>(
            [paletteComponent, pDoc, paletteCount, this](RasterComponent &raster)
        {
            RasterChangeHint hint = RasterChangeHint::None;
            ColorShifterDialog dialog(*paletteComponent, paletteCount, raster, pDoc->GetSelectedIndex(), *this);
            if (IDOK == dialog.DoModal())
            {
                hint |= RasterChangeHint::NewView;
            }
            return WrapHint(hint);
        }
            );
    }
}

void CRasterView::RemoveVGAPalette()
{
    CNewRasterResourceDocument *pDoc = GetDoc();
    if (pDoc)
    {
        pDoc->ApplyChanges<RasterComponent>(
            [](RasterComponent &raster)
        {
            return WrapHint(RasterChangeHint::NewView);
        },
            [](ResourceEntity &resource)
        {
            resource.RemoveComponent<PaletteComponent>();
        }
        );

    }
}

// CRasterView diagnostics

#ifdef _DEBUG
void CRasterView::AssertValid() const
{
	CView::AssertValid();
}

void CRasterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG


// CRasterView message handlers
