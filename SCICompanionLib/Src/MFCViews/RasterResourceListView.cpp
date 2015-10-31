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

//
// RasterResourceListView.cpp : implementation of the RasterResourceListView class
//
// This is the listview that shows all the pics in a game.
//

#include "stdafx.h"
#include "AppState.h"
#include "ResourceListDoc.h"
#include "ResourceListView.h"
#include "RasterResourceListView.h"
#include "GameExplorerView.h"
#include "ResourceEntity.h"
#include "RasterOperations.h"
#include "PaletteOperations.h"
#include "ResourceBlob.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define VIEW_IMAGE_SIZE 64

// A private message that indicates to us that the worker thread has something ready.
#define UWM_IMAGEREADY     (WM_APP + 0)

// CResourceListListView

IMPLEMENT_DYNCREATE(CRasterResourceListCtrl, CResourceListCtrl)

BEGIN_MESSAGE_MAP(CRasterResourceListCtrl, CResourceListCtrl)
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
    ON_NOTIFY_REFLECT(NM_DBLCLK, OnItemDoubleClick)
    ON_MESSAGE(UWM_IMAGEREADY, OnImageReady)
END_MESSAGE_MAP()

// CResourceListListView construction/destruction

CRasterResourceListCtrl::CRasterResourceListCtrl()
{
    _himlPics = nullptr;
    _iCorruptBitmapIndex = 0;
    _iTokenImageIndex = 0;
    _pQueue = nullptr;
    _iLastImageReadyHint = -1;
}

CRasterResourceListCtrl::~CRasterResourceListCtrl()
{
    if (_pQueue)
    {
        _pQueue->Abort();
    }
}

void _StretchForAspectRatio(CWnd *pwnd, CBitmap &bitmap)
{
    CDC *pDC = pwnd->GetDC();
    if (pDC)
    {
        CDC dcMemDest, dcMemSource;
        if (dcMemDest.CreateCompatibleDC(pDC) && dcMemSource.CreateCompatibleDC(pDC))
        {
            CSize size = bitmap.GetBitmapDimension();
            CBitmap stretchedBmp;
            int cyNew = appState->AspectRatioY(size.cy);
            if (stretchedBmp.CreateCompatibleBitmap(pDC, size.cx, cyNew))
            {
                dcMemSource.SelectObject(&bitmap);
                dcMemDest.SelectObject(&stretchedBmp);
                dcMemDest.SetStretchBltMode(HALFTONE);
                dcMemDest.StretchBlt(0, 0, size.cx, cyNew, &dcMemSource, 0, 0, size.cx, size.cy, SRCCOPY);
                bitmap.DeleteObject();
                bitmap.Attach(stretchedBmp.Detach());
                bitmap.SetBitmapDimension(size.cx, cyNew);
            }
        }
        pwnd->ReleaseDC(pDC);
    }
}

LRESULT CRasterResourceListCtrl::OnImageReady(WPARAM wParam, LPARAM lParam)
{
    VIEWWORKRESULT *pWorkResult;
    while (_pQueue->TakeWorkResult(&pWorkResult))
    {
        LVFINDINFO findInfo = {};
        findInfo.flags |= LVFI_PARAM | LVFI_WRAP;
        findInfo.lParam = pWorkResult->lParam;
        // REVIEW: might want to sync this with the way listview does things
        int i = FindItem(&findInfo, _iLastImageReadyHint);
        if (i != -1)
        {
            const ResourceBlob *pData = _GetResourceForItemMetadataOnly(i);
            if ((pData->GetChecksum() == pWorkResult->nID) &&
                (pData->GetNumber() == pWorkResult->iResourceNumber) &&
                (pData->GetPackageHint() == pWorkResult->iPackageNumber))
            {
                CBitmap bitmap;
                bitmap.Attach(pWorkResult->hbmp);
                pWorkResult->hbmp = nullptr;    // Owned by CBitmap now

                // Stretch the image if we're using the original aspect ratio.
                if (appState->_fUseOriginalAspectRatio)
                {
                    bitmap.SetBitmapDimension(VIEW_IMAGE_SIZE, VIEW_IMAGE_SIZE);
                    _StretchForAspectRatio(this, bitmap);
                }

                // this is a match
                int iIndex = ImageList_Add(_himlPics, bitmap, nullptr);
                if (iIndex != -1)
                {
                    SetItemImage(i, iIndex);
                    _UpdateStatusIfFlagsChanged(*pData, ResourceLoadStatusFlags::None, i);
                }
            }
        }
        _iLastImageReadyHint = i;
        delete pWorkResult;
    }
    return 0;
}

BOOL CRasterResourceListCtrl::SetItemImage(int nItem, int nImageIndex)
{
    LVITEM item = { 0 };
    item.iItem = nItem;
    item.mask = LVIF_IMAGE;
    item.iImage = nImageIndex;
    return SetItem(&item);
}

// Finds the smallest cel that is at least as big in each dimension as "dimensions".
CelIndex _FindBestPreviewCel(int dimensions, RasterComponent &raster)
{
    CelIndex bestCelIndex;
    int minGap = dimensions;
    for (int nLoop = 0; nLoop < raster.LoopCount(); nLoop++)
    {
        Loop &loop = raster.Loops[nLoop];
        for (int nCel = 0; nCel < (int)loop.Cels.size(); nCel++)
        {
            Cel &cel = loop.Cels[nCel];
            int minDimension = min(cel.size.cx, cel.size.cy);
            if ((dimensions - minDimension) < minGap)
            {
                bestCelIndex = CelIndex(nLoop, nCel);
                minGap = dimensions - minDimension;
            }
            if (minDimension >= dimensions)
            {
                // Good enough, return
                return bestCelIndex;
            }
        }
    }
    return bestCelIndex;
}

VIEWWORKRESULT *VIEWWORKRESULT::CreateFromWorkItem(VIEWWORKITEM *pWorkItem)
{
    HBITMAP hbmp = nullptr;

    std::unique_ptr<ResourceEntity> pEntity = CreateResourceFromResourceData(pWorkItem->blob);
    if (pEntity)
    {
        RasterComponent &raster = pEntity->GetComponent<RasterComponent>();
        CBitmap bitmap;
        std::unique_ptr<PaletteComponent> palette;
        if (raster.Traits.PaletteType == PaletteType::VGA_256)
        {
            palette = appState->GetResourceMap().GetMergedPalette(*pEntity, 999);
        }
        CelIndex previewCel = CelIndex(0, raster.Traits.PreviewCel);
        if (raster.Traits.PreviewCel == 0)
        {
            // Take 0 as an indication that we should use any cel
            previewCel = _FindBestPreviewCel(VIEW_IMAGE_SIZE, raster);
        }
        hbmp = GetBitmap(raster, palette.get(), previewCel, VIEW_IMAGE_SIZE, VIEW_IMAGE_SIZE, BitmapScaleOptions::AllowMag | BitmapScaleOptions::AllowMin);
    }

    VIEWWORKRESULT *pResult = new VIEWWORKRESULT;
    pResult->hbmp = hbmp;
    pResult->nID = pWorkItem->blob.GetChecksum();
    pResult->iResourceNumber = pWorkItem->blob.GetNumber();
    pResult->iPackageNumber = pWorkItem->blob.GetPackageHint();
    pResult->lParam = pWorkItem->lParam;
    return pResult;
}

void CRasterResourceListCtrl::_RegenerateImages()
{
    int count = this->GetItemCount();
    _OnInitListView(count);
    // Say that we want to be called back for images.
    for (int i = 0; i < count; i++)
    {
        this->SetItemImage(i, I_IMAGECALLBACK);
    }
}

void CRasterResourceListCtrl::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLISTVIEW *pnmlv = (NMLISTVIEW*)pNMHDR;
    if (pnmlv->uChanged & LVIF_STATE)
    {
        if (pnmlv->uNewState & LVIS_SELECTED)
        {
            // Set the "global selected view"
            // (use for InsertObject dialog, or the fake ego in the pic editor)
            auto resource = _GetResourceForItemMetadataOnly(pnmlv->iItem);
            if (resource->GetType() == ResourceType::View)
            {
                appState->SetRecentlyInteractedView(resource->GetNumber());
            }
        }
    }
    __super::OnItemClick(pNMHDR, pResult); // Our base class does some processing...
}

void CRasterResourceListCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
    LV_ITEM *pItem = &((LV_DISPINFO*)pNMHDR)->item; 

    if( pItem->mask & LVIF_IMAGE ) 
    { 
        if (_iView == LVS_ICON)
        {
            if (_pQueue)
            {
                ResourceBlob *pData = _GetResourceForItemRealized(pItem->lParam);
                std::unique_ptr<VIEWWORKITEM> pWorkItem = std::make_unique<VIEWWORKITEM>();
                pWorkItem->blob = *pData;
                pWorkItem->lParam = pItem->lParam;
                _pQueue->GiveWorkItem(move(pWorkItem));
                pItem->iImage = _iTokenImageIndex; // Done!
                pItem->mask |= LVIF_DI_SETITEM; // So we don't ask for it again.
            }
        }
        // Otherwise, in report view, we don't provide it.
    } 
    
    *pResult = 0; 
}



void CRasterResourceListCtrl::_OnInitListView(int cItems)
{
    // Put an imagelist in the view, for our pic previews.

    // Destroy any old list
    if (_himlPics)
    {
        ImageList_Destroy(_himlPics);
        _himlPics = NULL;
    }

    CSize sizeImages(VIEW_IMAGE_SIZE, appState->AspectRatioY(VIEW_IMAGE_SIZE));
    // Note: this color depth must match that which we get back from the view resource
    // Also: ILC_MASK must be specified, because of the overlay
    _himlPics = ImageList_Create(sizeImages.cx, sizeImages.cy, ILC_COLOR24 | ILC_MASK, cItems, 30);
    
    if (_himlPics)
    {
        CImageList *pTemp = CImageList::FromHandle(_himlPics);
        if (pTemp)
        {
            HIMAGELIST himlOld = (HIMAGELIST)SetImageList(pTemp, LVSIL_NORMAL);
        }
        // pTemp is a temporary object that we don't need to delete.

        // Add a token image.  Create an 8bpp bitmap that is blank.
        HBITMAP hbm = CreateBitmap(sizeImages.cx, sizeImages.cy, 1, 8, NULL);
        if (hbm)
        {
            _iTokenImageIndex = ImageList_Add(_himlPics, hbm, NULL);
            DeleteObject(hbm);
        }

        // Load an overlay image to use for "unused" items
        HBITMAP hbmOverlay = LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_OVERLAYOLD40));
        if (hbmOverlay)
        {
            int iOverlayImageIndex = ImageList_AddMasked(_himlPics, hbmOverlay, RGB(255, 0, 255));
            ImageList_SetOverlayImage(_himlPics, iOverlayImageIndex, OVERLAYINDEX_UNUSED);
            DeleteObject(hbmOverlay);
        }

        HBITMAP hbmCorrupt = LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_CORRUPTRESOURCE));
        if (hbmCorrupt)
        {
            _iCorruptBitmapIndex = ImageList_Add(_himlPics, hbmCorrupt, NULL);
            DeleteObject(hbmCorrupt);
        }
    }

    // Prepare our worker thread.
    if (_pQueue)
    {
        _pQueue->Abort();
    }
    _pQueue = std::make_shared<QueueItems<VIEWWORKITEM, VIEWWORKRESULT>>(GetSafeHwnd(), UWM_IMAGEREADY);
    if (_pQueue)
    {
        if (!_pQueue->Init())
        {
            _pQueue = nullptr;
        }
    }

    // Adjust the icon spacing so things don't look too spread out.
    CSize sizeSpacing(sizeImages.cx + 20, sizeImages.cy + 30);
    SetIconSpacing(sizeSpacing);

    // LVS_EX_BORDERSELECT, supported on IE 4.0 or later. -> Removed, as it causes problems in details view
    // LVS_EX_DOUBLEBUFFER, supported on XP or later.
    SetExtendedStyle(GetExtendedStyle() | 0x00010000 | LVS_EX_INFOTIP);

}

void CRasterResourceListCtrl::_PrepareLVITEM(LVITEM *pItem)
{
    __super::_PrepareLVITEM(pItem);

    // Say that we want to be called back for images.
    pItem->mask |= LVIF_IMAGE;
    pItem->iImage = I_IMAGECALLBACK;

    const ResourceBlob *pData = _GetResourceForItemMetadataOnly(pItem->lParam);
    if (pData)
    {
        // Add an overlay that indicates it is not the most recent.
        if (!appState->_resourceRecency.IsResourceMostRecent(pData))
        {
            pItem->mask |= LVIF_STATE;
            pItem->state = INDEXTOOVERLAYMASK(OVERLAYINDEX_UNUSED);
            pItem->stateMask = LVIS_OVERLAYMASK;
        }
    }
}

// CResourceListListView diagnostics

#ifdef _DEBUG
void CRasterResourceListCtrl::AssertValid() const
{
    __super::AssertValid();
}

void CRasterResourceListCtrl::Dump(CDumpContext& dc) const
{
    __super::Dump(dc);
}

#endif //_DEBUG

// CResourceListListView message handlers


 
 

