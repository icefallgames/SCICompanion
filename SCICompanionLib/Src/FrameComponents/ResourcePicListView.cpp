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
// ResourcePicListView.cpp : implementation of the CResourcePicListCtrl class
//
// This is the listview that shows all the pics in a game.
//

#include "stdafx.h"
#include "AppState.h"
#include "PicDoc.h" // Since we create new ones.
#include "ResourceListDoc.h"
#include "ResourceListView.h"
#include "ResourcePicListView.h"
#include "QueueItems.h"
#include "PaletteOperations.h"
#include "ResourceBlob.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// A private message that indicates to us that the worker thread has something ready.
#define UWM_PICREADY     (WM_APP + 0)

// CResourcePicListCtrl

IMPLEMENT_DYNCREATE(CResourcePicListCtrl, CResourceListCtrl)

BEGIN_MESSAGE_MAP(CResourcePicListCtrl, CResourceListCtrl)
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
    ON_NOTIFY_REFLECT(NM_DBLCLK, OnItemDoubleClick)
    ON_MESSAGE(UWM_PICREADY, OnPicReady)
END_MESSAGE_MAP()

// CResourcePicListCtrl construction/destruction

const static int DefaultPicWidth = 320 / 2;

int _GetPicBitmapHeight()
{
    int defaultPicHeight = (appState->GetVersion().DefaultResolution == NativeResolution::Res640x480) ? 120 : 100;
    return appState->AspectRatioY(defaultPicHeight);
}

CResourcePicListCtrl::CResourcePicListCtrl()
{
    _iTokenImageIndex = 0;
    _himlPics = nullptr;
    _pQueue = nullptr;
}

CResourcePicListCtrl::~CResourcePicListCtrl()
{
    if (_pQueue)
    {
        _pQueue->Abort();
    }
}

PICWORKRESULT *PICWORKRESULT::CreateFromWorkItem(PICWORKITEM *pWorkItem)
{
    HBITMAP hbm = NULL;
    std::unique_ptr<ResourceEntity> picResource = CreateResourceFromResourceData(pWorkItem->blob);
    // Draw this pic!
    hbm = GetPicBitmap(PicScreen::Visual, picResource->GetComponent<PicComponent>(), picResource->TryGetComponent<PaletteComponent>(), DefaultPicWidth, _GetPicBitmapHeight());

    PICWORKRESULT *pResult = new PICWORKRESULT;
    if (pResult)
    {
        pResult->hbmp = hbm;
        pResult->nID = pWorkItem->blob.GetChecksum();
        pResult->iResourceNumber = pWorkItem->blob.GetNumber();
        pResult->iPackageNumber = pWorkItem->blob.GetPackageHint();
    }
    return pResult;
}

BOOL CResourcePicListCtrl::SetItemImage(int nItem , int nImageIndex)
{
    LVITEM item = { 0 };
    item.iItem = nItem;
    item.mask = LVIF_IMAGE;
    item.iImage = nImageIndex;
    return SetItem(&item);
}

LRESULT CResourcePicListCtrl::OnPicReady(WPARAM wParam, LPARAM lParam)
{
    PICWORKRESULT *pWorkResult;
    while (_pQueue->TakeWorkResult(&pWorkResult))
    {
        int cItems = GetItemCount();
        for (int i = 0; i < cItems; i++)
        {
            const ResourceBlob *pData = _GetResourceForItemMetadataOnly(i);
            if ((pData->GetChecksum() == pWorkResult->nID) &&
                (pData->GetNumber() == pWorkResult->iResourceNumber) &&
                (pData->GetPackageHint() == pWorkResult->iPackageNumber))
            {
                // this is a match
                int iIndex = ImageList_Add(_himlPics, pWorkResult->hbmp, NULL);
                if (iIndex != -1)
                {
                    SetItemImage(i, iIndex);
                    _UpdateStatusIfFlagsChanged(*pData, ResourceLoadStatusFlags::None, i);
                }
                break;
            }
        }
        delete pWorkResult;
    }
    return 0;
}

void CResourcePicListCtrl::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
    LV_ITEM *pItem = &((LV_DISPINFO*)pNMHDR)->item; 

    if( pItem->mask & LVIF_IMAGE ) 
    { 
        if (_iView == LVS_ICON)
        {
            if (_pQueue)
            {
                ResourceBlob *pData = _GetResourceForItemRealized(pItem->lParam);
                unique_ptr<PICWORKITEM> pWorkItem = make_unique<PICWORKITEM>();
                pWorkItem->blob = *pData;
                _pQueue->GiveWorkItem(move(pWorkItem));
                pItem->iImage = _iTokenImageIndex; // Done!
                pItem->mask |= LVIF_DI_SETITEM; // So we don't ask for it again.
            }
        }
        // Otherwise, in report view, we don't provide it.
    } 
    *pResult = 0; 
}

void CResourcePicListCtrl::_RegenerateImages()
{
    int count = this->GetItemCount();
    _OnInitListView(count);
    // Say that we want to be called back for images.
    for (int i = 0; i < count; i++)
    {
        this->SetItemImage(i, I_IMAGECALLBACK);
    }
}

void CResourcePicListCtrl::_OnInitListView(int cItems)
{
    // Put an imagelist in the view, for our pic previews.

    // Destroy any old list
    if (_himlPics)
    {
        ImageList_Destroy(_himlPics);
        _himlPics = NULL;
    }

    // Note: this color depth must match that which we get back from the pic resource 
    // Also: ILC_MASK must be specified, because of the overlay
    _himlPics = ImageList_Create(DefaultPicWidth, _GetPicBitmapHeight(), ILC_COLOR24 | ILC_MASK, cItems, 30);
    
    if (_himlPics)
    {
        CImageList *pTemp = CImageList::FromHandle(_himlPics);
        if (pTemp)
        {
            HIMAGELIST himlOld = (HIMAGELIST)SetImageList(pTemp, LVSIL_NORMAL);
        }
        // pTemp is a temporary object that we don't need to delete.

        // Add a token image.  Create an 8bpp bitmap that is blank.
        HBITMAP hbm = CreateBitmap(DefaultPicWidth, _GetPicBitmapHeight(), 1, 8, NULL);
        if (hbm)
        {
            _iTokenImageIndex = ImageList_Add(_himlPics, hbm, NULL);
            DeleteObject(hbm);
        }

        // Load an overlay image to use for "unused" items
        HBITMAP hbmOverlay = LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_OVERLAYOLD));
        if (hbmOverlay)
        {
            int iOverlayImageIndex = ImageList_AddMasked(_himlPics, hbmOverlay, RGB(255, 0, 255));
            ImageList_SetOverlayImage(_himlPics, iOverlayImageIndex, OVERLAYINDEX_UNUSED);
            DeleteObject(hbmOverlay);
        }
    }

    // Prepare our worker thread.
    if (_pQueue)
    {
        _pQueue->Abort();
    }
    _pQueue = std::make_shared<QueueItems<PICWORKITEM, PICWORKRESULT>>(GetSafeHwnd(), UWM_PICREADY);
    if (_pQueue)
    {
        if (!_pQueue->Init())
        {
            _pQueue = nullptr;
        }
    }

    // LVS_EX_BORDERSELECT, supported on IE 4.0 or later. -> Removed, as it causes problems in details view
    // LVS_EX_DOUBLEBUFFER, supported on XP or later.
    SetExtendedStyle(GetExtendedStyle() | 0x00010000 | LVS_EX_INFOTIP);
}


void CResourcePicListCtrl::_PrepareLVITEM(LVITEM *pItem)
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


// CResourcePicListCtrl diagnostics

#ifdef _DEBUG
void CResourcePicListCtrl::AssertValid() const
{
    __super::AssertValid();
}

void CResourcePicListCtrl::Dump(CDumpContext& dc) const
{
    __super::Dump(dc);
}

#endif //_DEBUG


// CResourcePicListCtrl message handlers
