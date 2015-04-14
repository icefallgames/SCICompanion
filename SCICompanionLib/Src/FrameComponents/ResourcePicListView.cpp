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

CResourcePicListCtrl::CResourcePicListCtrl()
{
    _himlPics = NULL;
    _pQueue = NULL;
    //_bitmapHighlight.LoadBMP_Resource(MAKEINTRESOURCE(IDB_PICTHUMB_HIGHLIGHT));
}

CResourcePicListCtrl::~CResourcePicListCtrl()
{
    if (_pQueue)
    {
        _pQueue->Abort();
        _pQueue->Release();
    }
}

PICWORKRESULT *PICWORKRESULT::CreateFromWorkItem(PICWORKITEM *pWorkItem)
{
    HBITMAP hbm = NULL;
    std::unique_ptr<ResourceEntity> picResource = CreateResourceFromResourceData(pWorkItem->blob);
    // Draw this pic!
    hbm = GetPicBitmap(PicScreen::Visual, picResource->GetComponent<PicComponent>(), picResource->TryGetComponent<PaletteComponent>(), sPIC_WIDTH / 2, sPIC_HEIGHT / 2);

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
            ResourceBlob *pData = _GetResourceForItem(i);
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
                ResourceBlob *pData = (ResourceBlob*)pItem->lParam;
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
    _himlPics = ImageList_Create(sPIC_WIDTH / 2, sPIC_HEIGHT / 2, ILC_COLOR8 | ILC_MASK, cItems, 30);
    
    if (_himlPics)
    {
        CImageList *pTemp = CImageList::FromHandle(_himlPics);
        if (pTemp)
        {
            HIMAGELIST himlOld = (HIMAGELIST)SetImageList(pTemp, LVSIL_NORMAL);
        }
        // pTemp is a temporary object that we don't need to delete.

        // Add a token image.  Create an 8bpp bitmap that is blank.
        HBITMAP hbm = CreateBitmap(sPIC_WIDTH / 2, sPIC_HEIGHT / 2, 1, 8, NULL);
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
    _pQueue = new QueueItems<PICWORKITEM, PICWORKRESULT>(GetSafeHwnd(), UWM_PICREADY);
    if (_pQueue)
    {
        if (!_pQueue->Init())
        {
            _pQueue->Release();
            _pQueue = NULL;
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

    const ResourceBlob *pData = (const ResourceBlob*)pItem->lParam;
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
