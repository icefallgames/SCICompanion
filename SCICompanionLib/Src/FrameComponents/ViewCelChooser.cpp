// EditViewCelChooser.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ViewCelChooser.h"
#include "ResourceDocument.h"
#include "Components.h"
#include "ResourceEntity.h"
#include "ResourceEntityDocument.h"
#include "RasterOperations.h"
#include "NewRasterResourceDocument.h"
#include "PaletteOperations.h"

// CEditViewCelChooser

IMPLEMENT_DYNCREATE(CEditViewCelChooser, CListView)

CNewRasterResourceDocument *CEditViewCelChooser::_GetDoc()
{
    return static_cast<CNewRasterResourceDocument*>(GetDocument());
}

CEditViewCelChooser::CEditViewCelChooser()
{
    _sizeHiml = CSize(0, 0);
    _himlCels = NULL;
    _fSetViewMode = FALSE;

    _iDragIndex = -1;

    // Enable drag only if comctl32 v6 is loaded.
    // REVIEW: it would be better if we could check this control explicitly.
	DWORD dwVersion = GetDllVersion("comctl32.dll");
	DWORD dwCompare = PACKVERSION(6, 0);
	_fDragEnabled = dwVersion >= dwCompare;
}

CEditViewCelChooser::~CEditViewCelChooser()
{
    // listview frees _himlCels
}

BEGIN_MESSAGE_MAP(CEditViewCelChooser, CListView)
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
    ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBeginDrag)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


void CEditViewCelChooser::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    // If an item was selected, select its corresponding cel.
    NMLISTVIEW *pnmlv = (NMLISTVIEW*)pNMHDR;
    if (pnmlv->uChanged & LVIF_STATE)
    {
        if (pnmlv->uNewState & LVIS_SELECTED)
        {
            if (_GetDoc())
            {
                _GetDoc()->SetSelectedCel(pnmlv->iItem);
            }
        }
    }
}

void CEditViewCelChooser::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
    LV_ITEM *pItem = &((LV_DISPINFO*)pNMHDR)->item; 

    if( pItem->mask & LVIF_IMAGE ) 
    { 
        ASSERT_VALID(_GetDoc());

        int nLoop = _GetDoc()->GetSelectedLoop();

        RasterComponent &raster = _GetDoc()->GetResource()->GetComponent<RasterComponent>();
        // Draw this pic!
        CBitmap bitmap;
        bitmap.Attach(GetBitmap(raster, _GetDoc()->GetCurrentPaletteComponent(), CelIndex(nLoop, pItem->iItem), _sizeHiml.cx, _sizeHiml.cy, BitmapScaleOptions::AllowMag));
        if ((HBITMAP)bitmap)
        {
            CListCtrl &list = GetListCtrl();
            int iImagelistIndex = (int)list.GetItemData(pItem->iItem);
            if (iImagelistIndex == -1)
            {
                // It's a new one.
                int iIndex = ImageList_Add(_himlCels, (HBITMAP)bitmap, NULL);
                if (iIndex != -1)
                {
                    pItem->iImage = iIndex; // Done!
                    pItem->mask |= LVIF_DI_SETITEM; // So we don't ask for it again.
                    list.SetItemData(pItem->iItem, iIndex); // So we remember for next time.
                }
            }
            else
            {
                BOOL result = ImageList_Replace(_himlCels, iImagelistIndex, (HBITMAP)bitmap, NULL);
                pItem->iImage = iImagelistIndex;
                pItem->mask |= LVIF_DI_SETITEM; // So we don't ask for it again.
            }
        }
    } 
    *pResult = 0; 
}

#define DRAG_OFFSET 0

void CEditViewCelChooser::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
    if (_fDragEnabled)
    {
        // Only allow dragging if there is more than one cel.
        int cCels = 0;
        CResourceDocument *pDoc = static_cast<CResourceDocument*>(GetDocument());
        if (pDoc)
        {
            RasterComponent &raster = static_cast<const ResourceEntity*>(pDoc->GetResource())->GetComponent<RasterComponent>();
            int nLoop = _GetDoc()->GetSelectedLoop();
            cCels = raster.CelCount(nLoop);
        }

        if (cCels > 1)
        {
            // Begin a drag op
            NMLISTVIEW *pnmlv = (NMLISTVIEW*)pNMHDR;
            // Create a drag image
            CPoint pt(8, 8);
            CListCtrl &listCtl = GetListCtrl();
            _pDragImage.reset( listCtl.CreateDragImage(pnmlv->iItem, &pt));

            if (_pDragImage)
            {
                _pDragImage->BeginDrag(0, CPoint(8, 8));
                _pDragImage->DragEnter(GetDesktopWindow(), pnmlv->ptAction);
                SetCapture();
                _iDragIndex = pnmlv->iItem;
            }
        }
    }
}

//
// Copies of XP-specific stuff.
// 
/*
typedef struct 
{
    UINT cbSize;
    DWORD dwFlags;
    int iItem;
    DWORD dwReserved;
} LVINSERTMARK, * LPLVINSERTMARK;
#define LVM_INSERTMARKHITTEST               (LVM_FIRST + 168)
#define LVM_SETINSERTMARK                   (LVM_FIRST + 166)
#define LVM_GETINSERTMARK                   (LVM_FIRST + 167)
#define LVM_SETINSERTMARKCOLOR                 (LVM_FIRST + 170)
#define LVIM_AFTER                          0x00000001
*/
void CEditViewCelChooser::OnMouseMove(UINT nFlags, CPoint point)
{
    if (_iDragIndex != -1)
    {
        CPoint pt = point;
        ClientToScreen(&pt);
        CWnd *pWnd = WindowFromPoint(pt);
        if (pWnd)
        {
            CPoint ptClient = pt;
            pWnd->ScreenToClient(&ptClient);

            LVINSERTMARK lvim = { 0 };
            lvim.cbSize = sizeof(lvim);
            if (GetListCtrl().SendMessage(LVM_INSERTMARKHITTEST, (WPARAM)&ptClient, (LPARAM)&lvim))
            {
                LVINSERTMARK lvimCur = { 0 };
                lvimCur.cbSize = sizeof(lvimCur);
                if (GetListCtrl().SendMessage(LVM_GETINSERTMARK, 0, (LPARAM)&lvimCur))
                {
                    if ((lvimCur.iItem != lvim.iItem) || (lvimCur.dwFlags != lvim.dwFlags))
                    {
                        GetListCtrl().SendMessage(LVM_SETINSERTMARK, 0, (LPARAM)&lvim);
                    }
                }
            }
        }
        _pDragImage->DragMove(pt);
        _pDragImage->DragShowNolock(FALSE);
        _pDragImage->DragShowNolock(TRUE);
    }
    __super::OnMouseMove(nFlags, point);
}

void CEditViewCelChooser::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (_iDragIndex != -1)
    {
        ReleaseCapture();

        _pDragImage->DragLeave(GetDesktopWindow());
        _pDragImage->EndDrag();
		_pDragImage.reset(nullptr);

        CPoint pt = point;
        ClientToScreen(&pt);
        CWnd *pDropWnd = WindowFromPoint(pt);
        if (pDropWnd)
        {
            if (pDropWnd->GetSafeHwnd() == GetListCtrl().GetSafeHwnd())
            {
                // Reorder!
                LVINSERTMARK lvim = { 0 };
                lvim.cbSize = sizeof(lvim);
                if (GetListCtrl().SendMessage(LVM_INSERTMARKHITTEST, (WPARAM)&point, (LPARAM)&lvim))
                {
                    ResourceEntityDocument *pDoc = static_cast<ResourceEntityDocument*>(GetDocument());
                    int nLoop = _GetDoc()->GetSelectedLoop();
                    CelIndex dwIndexFrom = CelIndex(nLoop, _iDragIndex);
                    int nCelTo = lvim.iItem;
                    if (lvim.dwFlags & LVIM_AFTER)
                    {
                        nCelTo++;
                    }
                    CelIndex dwIndexTo = CelIndex(nLoop, nCelTo);

                    if (pDoc)
                    {
                        pDoc->ApplyChanges<RasterComponent>(
                            [&](RasterComponent &raster)
                            {
                                return WrapRasterChange(MoveLoopFromTo(raster, nLoop, _iDragIndex, nCelTo));
                            }
                            );
                    }
                }
            }
        }

        _iDragIndex = -1;

        // Remove any insert mark.
        LVINSERTMARK lvim;
        lvim.cbSize = sizeof(lvim);
        lvim.iItem = -1;
        GetListCtrl().SendMessage(LVM_SETINSERTMARK, 0, (LPARAM)&lvim);
    }
    __super::OnLButtonUp(nFlags, point);
}

BOOL CEditViewCelChooser::_RegenImageList(int cCountHint)
{
    BOOL fRet = FALSE;
    // Destroy any old list
    if (_himlCels)
    {
        ImageList_Destroy(_himlCels);
        _himlCels = NULL;
    }

    // ILC_MASK is essential for the drag image to work.
    // _himlCels = ImageList_Create(_sizeHiml.cx, _sizeHiml.cy, ILC_COLOR8 | ILC_MASK, cCountHint, 30);
    // Note: this color depth must match that which we get back from the resource
    // REVIEW: Since I started supporting switching paletters, ILC_COLOR8 imagelists just generate blank imagelists,
    // even though all images I supply to the imagelist use the same palette. Not sure why.
    // Switching to ILC_COLOR24 addresses the issue.
    _himlCels = ImageList_Create(_sizeHiml.cx, _sizeHiml.cy, ILC_COLOR24 | ILC_MASK, cCountHint, 30);
    
    if (_himlCels)
    {
        CImageList *pTemp = CImageList::FromHandle(_himlCels);
        if (pTemp)
        {
            HIMAGELIST himlOld = (HIMAGELIST)GetListCtrl().SetImageList(pTemp, LVSIL_NORMAL);
        }
        // pTemp is a temporary object that we don't need to delete.
        fRet = TRUE;
    }
    return fRet;
}

CSize CEditViewCelChooser::_CalcMaxSize()
{
    CSize sizeMax(0, 0);
    CNewRasterResourceDocument *pDoc = _GetDoc();
    if (pDoc)
    {
        int nLoop = pDoc->GetSelectedLoop();

        // Find out if the max size of the cels has changed.
        RasterComponent &raster = pDoc->GetResource()->GetComponent<RasterComponent>();
        int cCels = raster.CelCount(nLoop);
        for (int i = 0; i < cCels; i++)
        {
            const Cel &cel = raster.GetCel(CelIndex(nLoop, i));
            CSize size(cel.size.cx, cel.size.cy);
            sizeMax.cx = max(sizeMax.cx, size.cx);
            sizeMax.cy = max(sizeMax.cy, size.cy);
        }
    }
    return sizeMax;
}

const int AtLeastThisSize = 64;

void CEditViewCelChooser::_RefreshView(bool forceRegen)
{
    CResourceDocument *pDoc = static_cast<CResourceDocument*>(GetDocument());
    if (pDoc)
    {
        int nLoop = _GetDoc()->GetSelectedLoop();

        // Find out if the max size of the cels has changed.
        CSize sizeMax = _CalcMaxSize();
        // Make it so it's not so terribly tiny:
        int maxDimension = max(1, max(sizeMax.cx, sizeMax.cy));
        if (maxDimension < AtLeastThisSize)
        {
            int factor = (AtLeastThisSize + maxDimension - 1) / maxDimension;
            sizeMax.cx *= factor;
            sizeMax.cy *= factor;
        }
        
        RasterComponent &raster = static_cast<const ResourceEntity*>(pDoc->GetResource())->GetComponent<RasterComponent>();
        int cCels = raster.CelCount(nLoop);

        BOOL fOk = TRUE;
        int cImageListItems;
        if (sizeMax != _sizeHiml)
        {
            _sizeHiml = sizeMax;
            // Use a smaller than normal icon spacing:
            // REVIEW: this doesn't work well when we have fonts (2 lines of text)
            // GetListCtrl().SetIconSpacing(sizeMax.cx + 10, sizeMax.cy + 10);
            fOk = _RegenImageList(cCels);
            cImageListItems = 0; // Pretend
        }
        else
        {
            cImageListItems = ImageList_GetImageCount(_himlCels);
        }

        if (fOk)
        {
            // Adjust the number of items in the listview.
            // We want to keep our position, so just re-use items.
            CListCtrl &list = GetListCtrl();
            int cItems = list.GetItemCount();
            int i = 0;
            for (; i < cCels; i++)
            {
                
                TCHAR szBuf[10];
                *szBuf = 0;

                RasterComponent &raster = static_cast<const ResourceEntity*>(pDoc->GetResource())->GetComponent<RasterComponent>();
                if (raster.Traits.GetItemLabelFunc)
                {
                    raster.Traits.GetItemLabelFunc(szBuf, ARRAYSIZE(szBuf), i);
                }
                else
                {
                    StringCchPrintf(szBuf, ARRAYSIZE(szBuf), "%d", i);
                }
                if (i < cItems)
                {
                    if (i >= cImageListItems)
                    {
                        // There is no image in the imagelist for this one.  Reset param to -1.
                        list.SetItem(i, 0, LVIF_IMAGE | LVIF_PARAM, szBuf, I_IMAGECALLBACK, 0, 0, -1);
                    }
                    else
                    {
                        list.SetItem(i, 0, LVIF_IMAGE, NULL, I_IMAGECALLBACK, 0, 0, NULL);
                    }
                }
                else
                {
                    list.InsertItem(LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT, i, szBuf, 0, 0, I_IMAGECALLBACK, -1);
                }
            }

            // Now delete the rest, they are not necessary
            // Delete items from the end first
            for (int j = cItems - 1; j >= i; j--)
            {
                list.DeleteItem(j);
            }
        }
    }
}

BOOL CEditViewCelChooser::PreCreateWindow(CREATESTRUCT& cs)
{
    // We are single select
    cs.style |= LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_AUTOARRANGE;
	return __super::PreCreateWindow(cs);
}

void CEditViewCelChooser::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
    RasterChangeHint hint = GetHint<RasterChangeHint>(pHint);

    __super::OnUpdate(pSender, lHint, pHint);

    if (!_fSetViewMode)
    {
        LONG_PTR dwStyle = GetWindowLongPtr(m_hWnd, GWL_STYLE); 
        dwStyle &= ~LVS_TYPEMASK;
        dwStyle |= LVS_ICON;
        SetWindowLongPtr(m_hWnd, GWL_STYLE, (LONG)dwStyle);
        _fSetViewMode = TRUE;

        // Use "border select"
        GetListCtrl().SetExtendedStyle(0x00008000);
    }

    if (_GetDoc())
    {
        int nLoop = _GetDoc()->GetSelectedLoop();

        int nCel = _GetDoc()->GetSelectedCel();

        if (IsFlagSet(hint, RasterChangeHint::LoopSelection | RasterChangeHint::NewView | RasterChangeHint::Palette))
        {
            _RefreshView(IsFlagSet(hint, RasterChangeHint::Palette));
            Invalidate(FALSE);

            // If this is a new one, then add in RasterChangeHint::CelSelection
            if (IsFlagSet(hint, RasterChangeHint::NewView))
            {
                hint |= RasterChangeHint::CelSelection;
            }
        }
        
        if (IsFlagSet(hint, RasterChangeHint::Loop))
        {
            if (UnwrapObject<CelIndex>(pHint).loop == nLoop)
            {
                _RefreshView(false);
                Invalidate(FALSE);
            }
        }
        else if (IsFlagSet(hint, RasterChangeHint::Cel))
        {
            // Doesn't matter which loop.
            if (UnwrapObject<CelIndex>(pHint).cel == nCel)
            {
                // Update just this cel.  Except if cel size changed, then update everything.
                CSize sizeMax = _CalcMaxSize();
                if (sizeMax != _sizeHiml)
                {
                    _RefreshView(false);
                }
                else
                {
                    // Just the internals of the cel changed.  Simply get a new image for it.
                    RasterComponent &raster = _GetDoc()->GetResource()->GetComponent<RasterComponent>();
                    const PaletteComponent *palette = _GetDoc()->GetCurrentPaletteComponent();
                    CBitmap bitmap;
                    bitmap.Attach(GetBitmap(raster, palette, CelIndex(nLoop, nCel), _sizeHiml.cx, _sizeHiml.cy, BitmapScaleOptions::AllowMag));
                    if ((HBITMAP)bitmap)
                    {
                        int iImageIndex = (int)GetListCtrl().GetItemData(nCel);
                        BOOL result = ImageList_Replace(_himlCels, iImageIndex, (HBITMAP)bitmap, NULL);
                    }
                }
                Invalidate(FALSE);
            }
        }
        else if (IsFlagSet(hint, RasterChangeHint::CelSelection))
        {
            // Select it in the listview, and bring it into view
            GetListCtrl().SetItemState(nCel, LVIS_SELECTED, LVIS_SELECTED);
            GetListCtrl().EnsureVisible(nCel, FALSE);
        }
    }
}


// CEditViewCelChooser diagnostics

#ifdef _DEBUG
void CEditViewCelChooser::AssertValid() const
{
	CListView::AssertValid();
}

void CEditViewCelChooser::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG


// CEditViewCelChooser message handlers
