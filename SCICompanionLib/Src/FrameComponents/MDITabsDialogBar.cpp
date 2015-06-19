// CMDITabsDialogBar.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MDITabsDialogBar.h"
#include "ResourceDocument.h"
#include "AppState.h"
#include "MDITabChildWnd.h"
#include "ScriptDocument.h"

// CMDITabsDialogBar

//
// Includes code from "Ownerdraw Tab Controls - Borders and All" on codeproject.com, by .dan.g.
//


BEGIN_MESSAGE_MAP(CMDITabsDialogBar, CTabCtrl)
    ON_MESSAGE(WM_SIZEPARENT, OnSizeParent)
    ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelChange)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

CMDITabsDialogBar::CMDITabsDialogBar()
{
    _hoverIndex = -1;
    _closeBitmap.LoadBMP_Resource(MAKEINTRESOURCE(IDB_CLOSE_INACTIVE));
    _closeBitmapActive.LoadBMP_Resource(MAKEINTRESOURCE(IDB_CLOSE_ACTIVE));
    _fMouseWithin = false;
    _overCloseIcon = -1;
    _capturing = false;
}

void CMDITabsDialogBar::AddTab(CFrameWnd *pFrame, MDITabType iType)
{
    // REVIEW: this is a bit of a hack.  The tab control uses its font to resize itself.
    // However, we use prof-uis paint manager's font to draw.  This font can change (and be deleted)
    // when system settings change.  But we don't know when to update it here.  It's fine for painting,
    // but bad for measuring.  So just set it again whenever we add an item.
    SetFont(&g_PaintManager->m_FontBoldBC);

    CString strTitle = pFrame->GetTitle();
    int cItems = GetItemCount();
    // Except for the first item, add at index 1 (game explorer tab always comes first)
    int iIndex = cItems > 0 ? 1 : 0;
    InsertItem(TCIF_TEXT | TCIF_PARAM, iIndex, strTitle, iType, (LPARAM)pFrame);
}

void CMDITabsDialogBar::RemoveTab(CFrameWnd *pFrame)
{
    // Remove it from the tab control
    int i = 0;
    for ( ; i < GetItemCount(); i++)
    {
        TCITEM tcitem = { 0 };
        tcitem.mask = TCIF_PARAM;
        if (GetItem(i, &tcitem))
        {
            if (tcitem.lParam == (LPARAM)pFrame)
            {
                break;
            }
        }
    }
    if (i < GetItemCount())
    {
        DeleteItem(i);
    }

    // Remove it from the travellog
    _travelLog.remove(pFrame);
    if (pFrame == _pActiveTab)
    {
        _pActiveTab = nullptr;
    }
    // -> the newly active tab should get set in OnActivateTab
}

void CMDITabsDialogBar::OnActivateTab(CFrameWnd *pFrame)
{
    if (!_fNavigatingViaTravelLog)
    {
        // Remove it if its already in the list.
        _travelLog.remove(pFrame);
        // And add it back at the end.
        _travelLog.push_back(pFrame);
    }
    _pActiveTab = pFrame;
}

CFrameWnd *CMDITabsDialogBar::_GetNextPrev(bool fNext)
{
    CFrameWnd *pNext = nullptr;
    if (_pActiveTab)
    {
        travellog_list::iterator it = find(_travelLog.begin(), _travelLog.end(), _pActiveTab);
        if (it != _travelLog.end())
        {
            if (fNext)
            {
                if ((++it) != _travelLog.end())
                {
                    pNext = *it;
                }
            }
            else
            {
                if (it != _travelLog.begin())
                {
                    pNext = *(--it);
                }
            }
        }
    }
    return pNext;
}

bool CMDITabsDialogBar::CanGoBack()
{
    return (_GetNextPrev(false) != NULL);
}

bool CMDITabsDialogBar::CanGoForward()
{
    return (_GetNextPrev(true) != NULL);
}

void CMDITabsDialogBar::_GoBackForward(bool fForward)
{
    CFrameWnd *pBack = _GetNextPrev(fForward);
    if (pBack)
    {
        _fNavigatingViaTravelLog = true;
        CMDIChildWnd *pActive = static_cast<CMDIChildWnd*>(pBack);
        pActive->MDIActivate();
        _fNavigatingViaTravelLog = false;
    }
}

void CMDITabsDialogBar::GoBack()
{
    _GoBackForward(false);
}
void CMDITabsDialogBar::GoForward()
{
    _GoBackForward(true);
}

CScriptDocument *CMDITabsDialogBar::ActivateScript(ScriptId script)
{
    CScriptDocument *pSDRet = NULL;
    // Find a script that matches this, if any
    for (int i = 0 ; !pSDRet && i < GetItemCount(); i++)
    {
        CMDITabChildWnd *pActive;
        CScriptDocument *pSD = static_cast<CScriptDocument*>(_GetIfKindOf(i, RUNTIME_CLASS(CScriptDocument), &pActive));
        if (pSD && (pSD->GetScriptId() == script))
        {
            pSDRet = pSD;
            pActive->MDIActivate();
            pActive->SetFocus();
        }
    }
    return pSDRet;
}

CDocument *CMDITabsDialogBar::_GetIfKindOf(int i, const CRuntimeClass* pClass, CMDITabChildWnd **pActive) const
{
    CDocument *pDoc = NULL;
    TCITEM tcitem = { 0 };
    tcitem.mask = TCIF_PARAM;
    if (GetItem(i, &tcitem))
    {
        *pActive = reinterpret_cast<CMDITabChildWnd*>(tcitem.lParam);
        CDocument *pDocQ = (*pActive)->GetActiveDocument();
        if (pDocQ->IsKindOf(pClass))
        {
            pDoc = pDocQ;
        }
    }  
    return pDoc;
}

CScriptDocument *CMDITabsDialogBar::GetOpenScriptDocument(WORD wNum)
{
    // Find a script that matches this, if any
    for (int i = 0; i < GetItemCount(); i++)
    {
        CMDITabChildWnd *pActive;
        CScriptDocument *pRD = static_cast<CScriptDocument*>(_GetIfKindOf(i, RUNTIME_CLASS(CScriptDocument), &pActive));
        if (pRD && (pRD->GetScriptId().GetResourceNumber() == wNum))
        {
            return pRD;
        }
    }
    return nullptr;
}

CResourceDocument *CMDITabsDialogBar::ActivateResourceDocument(ResourceType type, WORD wNum)
{
    CResourceDocument *pRDRet = NULL;
    // Find a script that matches this, if any
    for (int i = 0 ; !pRDRet && i < GetItemCount(); i++)
    {
        CMDITabChildWnd *pActive;
        CResourceDocument *pRD = static_cast<CResourceDocument*>(_GetIfKindOf(i, RUNTIME_CLASS(CResourceDocument), &pActive));
        if (pRD && (pRD->GetType() == type) && (pRD->GetNumber() == (int)wNum))
        {
            if (appState->_resourceRecency.IsResourceMostRecent(pRD))
            {
                pRDRet = pRD;
                pActive->MDIActivate();
                pActive->SetFocus();
            }
        }
    }
    return pRDRet;
}

int _GetBitmapIndex(MDITabType tabType)
{
    int i = 0;
    int tt = static_cast<int>(tabType);
    while (tt)
    {
        tt >>= 1;
        ++i;
    }
    return i;
}

// MDITabs message handlers


const double c_rgHueAdjust[] =
{
    0.0,    // none
    0.0,    // game
    0.4,    // view
    0.0,    // pic
    -1.0,   // script
    -0.8,    // vocab
    -0.3,    // font
    0.8,    // cursor
    0.3,    // text
    -0.6,   // sound
    0.0,    // room explorer
    0.6,    // palette
    0.3,    // message
};

const double c_rgSatAdjust[] =
{
    0.0,    // none
    -1.0,    // game
    0.0,    // view
    0.0,    // pic
    0.0,   // script
    0.0,    // vocab
    0.0,    // font
    0.0,    // cursor
    0.0,    // text
    0.0,   // sound
    0.0,    // room explorer
    0.0,    // palette
    0.0,    // message
};

const double c_rgLumAdjust[] =
{
    0.0,    // none
    0.0,    // game
    0.0,    // view
    0.0,    // pic
    0.0,   // script
    0.0,    // vocab
    0.0,    // font
    0.0,    // cursor
    0.0,    // text
    0.0,   // sound
    0.0,    // room explorer
    0.5,    // palette
    0.0,    // message
};


int CMDITabsDialogBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;
    _fNavigatingViaTravelLog = false;

    ASSERT(ARRAYSIZE(c_rgHueAdjust) == ARRAYSIZE(_tabBitmap));
    for (int i = 0; i < ARRAYSIZE(c_rgHueAdjust); ++i)
    {
        _tabBitmap[i].LoadBMP_Resource(MAKEINTRESOURCE(IDB_TABPIC));
        _tabBitmap[i].AdjustHLS(COLORREF(-1L), RGB(0, 0, 0), c_rgHueAdjust[i], c_rgLumAdjust[i], c_rgSatAdjust[i], NULL);

        _tabBitmapNS[i].LoadBMP_Resource(MAKEINTRESOURCE(IDB_TABPICNS));
        _tabBitmapNS[i].AdjustHLS(COLORREF(-1L), RGB(0, 0, 0), c_rgHueAdjust[i], c_rgLumAdjust[i], c_rgSatAdjust[i], NULL);
    }
   return 0;
}

void CMDITabsDialogBar::OnSelChange(NMHDR *pnmhdr, LRESULT *lResult)
{
    int iActive = GetCurSel();
    if (iActive != -1)
    {
        TCITEM tcitem;
        tcitem.mask = TCIF_PARAM;
        if (GetItem(iActive, &tcitem))
        {
            CMDIChildWnd *pActive = reinterpret_cast<CMDIChildWnd*>(tcitem.lParam);
            //pActive->ActivateFrame(); // This glitches maximized windows, so use MDIActivate
            pActive->MDIActivate();
        }
    }
}

void CMDITabsDialogBar::OnUpdateTitles()
{
    // See comment in AddTab...
    SetFont(&g_PaintManager->m_FontBoldBC);

    // Determine the active child frame:
    CFrameWnd *pActive = NULL;
    CFrameWnd *pFrame = (CFrameWnd*)AfxGetMainWnd();
    if (pFrame)
    {
        pActive = pFrame->GetActiveFrame();
    }
    int iActive = -1;

    // Now go through and update titles, and selection.
    int i = 0;
    for ( ; i < GetItemCount(); i++)
    {
        TCITEM tcitem = { 0 };
        tcitem.mask = TCIF_PARAM;
        if (GetItem(i, &tcitem))
        {
            CFrameWnd *pFrame = (CFrameWnd*)tcitem.lParam;
            if (pFrame)
            {
                if (pFrame == pActive)
                {
                    iActive = i;
                }

                CString strTitle;
                pFrame->GetWindowText(strTitle);
                TCHAR szTitleCurrent[MAX_PATH];
                tcitem.mask = TCIF_TEXT;
                tcitem.pszText = szTitleCurrent;
                tcitem.cchTextMax = ARRAYSIZE(szTitleCurrent);
                if (GetItem(i, &tcitem))
                {
                    // Actually, always add the space! We don't want our tabs changing size
                    //if (pFrame == pActive)
                    {
                        strTitle += "      ";
                    }
                    if (0 != strcmp(szTitleCurrent, strTitle))
                    {
                        StringCchCopy(szTitleCurrent, ARRAYSIZE(szTitleCurrent), strTitle);
                        tcitem.mask = TCIF_TEXT;
                        tcitem.pszText = szTitleCurrent;
                        SetItem(i, &tcitem);
                    }
                }
            }
        }
    }

    // Make sure the active one is selected.
    if (GetCurSel() != iActive)
    {
        SetCurSel(iActive);
    }
}


// CMDITabsDialogBar message handlers

//
// This makes the bar the entire width of the frame, and a fixed height.
//
LRESULT CMDITabsDialogBar::OnSizeParent(WPARAM wParam, LPARAM lParam)
{
    ASSERT_VALID(this);
    wParam; // unused
    AFX_SIZEPARENTPARAMS *pLayout = (AFX_SIZEPARENTPARAMS *) lParam;
    ASSERT(pLayout != NULL);
    CRect rcFrameRest = &pLayout->rect;
    DWORD dwWndStyle = GetStyle();
    if((dwWndStyle & WS_VISIBLE) == 0)
    {
        return 0;
    }
    int nHeight = 26;
    CRect rcOwnLayout(rcFrameRest);
    pLayout->rect.top += nHeight;
    rcOwnLayout.bottom = rcOwnLayout.top + nHeight;
    pLayout->sizeTotal.cy += nHeight;
    if (pLayout->hDWP != NULL)
    {
        ::AfxRepositionWindow(pLayout, m_hWnd, &rcOwnLayout);
    }
    return 0;
}

void CMDITabsDialogBar::DrawItem(LPDRAWITEMSTRUCT pdis)
{
    CRect rc = pdis->rcItem;
    int nTabIndex = pdis->itemID;
    bool fSelected = (nTabIndex == GetCurSel());
    ODA_DRAWENTIRE;
	char label[64];
	TCITEM tci;
	tci.mask = TCIF_TEXT | TCIF_PARAM;
	tci.pszText = label;
	tci.cchTextMax = ARRAYSIZE(label);
	if (GetItem(nTabIndex, &tci))
    {
        CMDITabChildWnd *pActive = reinterpret_cast<CMDITabChildWnd*>(tci.lParam);

        CDC dc;
        dc.Attach(pdis->hDC);

        int iIndex = _GetBitmapIndex(pActive->GetTabType());
        CExtBitmap &bitmapToUse = fSelected ? _tabBitmap[iIndex] : _tabBitmapNS[iIndex];
        CRect rcSrc(CPoint(0, 0), bitmapToUse.GetSize());
        CRect rcPadding(0, 0, 0, 0);


        // bitmapToUse.AlphaBlendSkinParts(pdis->hDC, rc, rcSrc, rcPadding, /*__EXT_BMP_FLAG_PREMULTIPLIED_RGB_CHANNELS |*/ CExtBitmap::__EDM_STRETCH, true, true, 0xFF);
        CRect rcUsable = rc;
        rcUsable.left += 1;
        rcUsable.top += 1;
        if (fSelected)
        {
            rcUsable.OffsetRect(-1, -1);
            rcUsable.bottom++;
        }
        COLORREF basicColor = fSelected ? RGB(0, 255, 0) : RGB(0, 144, 0);
        COLORREF accentColor = CExtBitmap::stat_HLS_Adjust(basicColor, c_rgHueAdjust[iIndex], c_rgLumAdjust[iIndex], c_rgSatAdjust[iIndex]);
        COLORREF backgroundColor = g_PaintManager->GetColor(COLOR_3DFACE);
        if (!fSelected)
        {
            backgroundColor = CExtBitmap::stat_HLS_Adjust(backgroundColor, 0.0, -0.08, 0.0);
        }

        dc.FillSolidRect(rc, backgroundColor);

        CRect rcGradient = rcUsable;
        // Blend between our main color and the background color for the top part of the gradient (so the gradient isn't so extreme)
        COLORREF blended = CExtBitmap::stat_RGB_Blend(accentColor, backgroundColor, fSelected ? 196 : 128);
        TRIVERTEX vertices[2] =
        {
            { rcGradient.left, rcGradient.top, GetRValue(blended) * 256, GetGValue(blended) * 256, GetBValue(blended) * 256, 65280 },
            { rcGradient.right, rcGradient.bottom, GetRValue(backgroundColor) * 256, GetGValue(backgroundColor) * 256, GetBValue(backgroundColor) * 256, 65280 },
        };
        GRADIENT_RECT gradRects[1] = { 0, 1 };
        dc.GradientFill(vertices, ARRAYSIZE(vertices), gradRects, ARRAYSIZE(gradRects), GRADIENT_FILL_RECT_V);

        // Use a different font decoration depending on if this is the most recent version of this item.
        bool fNotMostRecent = false;
        CDocument *pDocAny = pActive->GetActiveDocument();
        if (pDocAny && pDocAny->IsKindOf(RUNTIME_CLASS(CResourceDocument)))
        {
            CResourceDocument *pDoc = static_cast<CResourceDocument*>(pDocAny);
            fNotMostRecent = !appState->_resourceRecency.IsResourceMostRecent(pDoc);
        }

        // use _overCloseIcon?

        if (fNotMostRecent)
        {
            // Draw a diagonal line across the thing.
            dc.MoveTo(rc.TopLeft());
            dc.LineTo(rc.BottomRight());
        }

        // Draw a close icon
        if ((pActive == _pActiveTab) || (_hoverIndex == nTabIndex))
        {
            int top = (rc.Height() - 16) / 2 + 1;
            RECT rcClose = { rc.right - 18, top, rc.right - 2, top + 16 };
            if (_fMouseWithin && (_overCloseIcon == nTabIndex))
            {
                _closeBitmapActive.AlphaBlend(pdis->hDC, rcClose);
            }
            else
            {
                _closeBitmap.AlphaBlend(pdis->hDC, rcClose);
            }
        }

        rcUsable.OffsetRect(4, 1); // indent

        COLORREF crText = g_PaintManager->GetColor(COLOR_BTNTEXT);
        //COLORREF crText = RGB(0, 0, 0); // g_PaintManager->PAINTPUSHBUTTONDATA::m_clrForceTextNormal; // green in release, black in debug!

        int nOldText = dc.SetTextColor(crText);
        int nOldBk = dc.SetBkMode(TRANSPARENT);
        dc.DrawText(tci.pszText, -1, &rcUsable, DT_SINGLELINE);
        dc.SetBkMode(nOldBk);
        dc.SetTextColor(nOldText);

        dc.Detach();
    }
}


void CMDITabsDialogBar::DrawItemBorder(LPDRAWITEMSTRUCT lpdis)
{
	BOOL bSelected = (lpdis->itemID == (UINT)GetCurSel());
    BOOL bBackTabs = FALSE;

	CRect rItem(lpdis->rcItem);
	CDC* pDC = CDC::FromHandle(lpdis->hDC); 

	if (bSelected || bBackTabs)
	{
		rItem.bottom += bSelected ? -1 : 1;
		// edges
        pDC->FillRect(CRect(rItem.left, rItem.top, rItem.left + 1, rItem.bottom), &g_PaintManager->m_brushLightestDefault);
		pDC->FillRect(CRect(rItem.left, rItem.top, rItem.right, rItem.top + 1), &g_PaintManager->m_brushLightestDefault);
		pDC->FillRect(CRect(rItem.right - 1, rItem.top, rItem.right, rItem.bottom), &g_PaintManager->m_brushDarkestDefault);
	}
	else // draw simple dividers
	{
		pDC->FillRect(CRect(rItem.left - 1, rItem.top, rItem.left, rItem.bottom), &g_PaintManager->m_brushDarkestDefault);
		pDC->FillRect(CRect(rItem.right - 1, rItem.top, rItem.right, rItem.bottom), &g_PaintManager->m_brushDarkestDefault);
	}
}

void CMDITabsDialogBar::DrawMainBorder(LPDRAWITEMSTRUCT lpdis)
{
    // This ends up being a thin line under stuff
    CRect rBorder(lpdis->rcItem);
    CDC* pDC = CDC::FromHandle(lpdis->hDC); 
    //pDC->FillSolidRect(&rBorder, RGB(255, 128, 50));
    pDC->FillSolidRect(&rBorder, g_PaintManager->GetColor(COLOR_3DFACE));
}

void CMDITabsDialogBar::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// prepare dc
    HGDIOBJ hOldFont = dc.SelectObject(g_PaintManager->m_FontBoldBC);

	DRAWITEMSTRUCT dis;
	dis.CtlType = ODT_TAB;
	dis.CtlID = GetDlgCtrlID();
	dis.hwndItem = GetSafeHwnd();
	dis.hDC = dc.GetSafeHdc();
	dis.itemAction = ODA_DRAWENTIRE;

	// draw the rest of the border
	CRect rClient, rPage;
	GetClientRect(&dis.rcItem);
	rPage = dis.rcItem;
	AdjustRect(FALSE, rPage);
	dis.rcItem.top = rPage.top - 2;

	DrawMainBorder(&dis);

	// paint the tabs first and then the borders
	int nTab = GetItemCount();
	int nSel = GetCurSel();

	if (!nTab) // no pages added
		return;

	while (nTab--)
	{
		if (nTab != nSel)
		{
			dis.itemID = nTab;
			dis.itemState = 0;

			VERIFY(GetItemRect(nTab, &dis.rcItem));
			DrawItem(&dis);
			//DrawItemBorder(&dis);
		}
	}

	// now selected tab
    if (nSel != -1)
    {
	    dis.itemID = nSel;
	    dis.itemState = ODS_SELECTED;

	    VERIFY(GetItemRect(nSel, &dis.rcItem));
	    DrawItem(&dis);
	    //DrawItemBorder(&dis);    
    }
    dc.SelectObject(hOldFont);
}

BOOL CMDITabsDialogBar::OnEraseBkgnd(CDC *pDC)
{
	CRect rClient, rTab, rTotalTab, rBkgnd, rEdge;
	int nTab, nTabHeight = 0;

	__super::OnEraseBkgnd(pDC);

	// calc total tab width
	GetClientRect(rClient);
	nTab = GetItemCount();
	rTotalTab.SetRectEmpty();

	while (nTab--)
	{
		GetItemRect(nTab, rTab);
		rTotalTab.UnionRect(rTab, rTotalTab);
	}

	nTabHeight = rTotalTab.Height();

	// add a bit
	rTotalTab.InflateRect(2, 3);
	rEdge = rTotalTab;

	// then if background color is set, paint the visible background
	// area of the tabs in the bkgnd color
	// note: the mfc code for drawing the tabs makes all sorts of assumptions
	// about the background color of the tab control being the same as the page
	// color - in some places the background color shows thru' the pages!!
	// so we must only paint the background color where we need to, which is that
	// portion of the tab area not excluded by the tabs themselves

    //CBrush *pBrush = &g_PaintManager->m_brushLighterDefault;
    CBrush solid(g_PaintManager->GetColor(COLOR_3DFACE));
    CBrush *pBrush = &solid;
	
	// full width of tab ctrl above top of tabs
	rBkgnd = rClient;
	rBkgnd.bottom = rTotalTab.top + 3;
    pDC->FillRect(&rBkgnd, pBrush);
	
	// width of tab ctrl visible bkgnd including bottom pixel of tabs to left of tabs
	rBkgnd = rClient;
	rBkgnd.right = 2;
	rBkgnd.bottom = rBkgnd.top + (nTabHeight + 2);
    pDC->FillRect(&rBkgnd, pBrush);
	
	// to right of tabs
	rBkgnd = rClient;
	rBkgnd.left += rTotalTab.Width() - 2;
	rBkgnd.bottom = rBkgnd.top + (nTabHeight + 2);
    pDC->FillRect(&rBkgnd, pBrush);

	return TRUE;
}

// Returns true if the point is over the close "button" on the currently selected tab
int CMDITabsDialogBar::IsOverClose(CPoint point, int *pindex)
{
    int overClose = -1;
    TCHITTESTINFO hitTest;
    hitTest.pt = point;
    *pindex = HitTest(&hitTest);
    //if ((*pindex != -1) && (*pindex == GetCurSel()))
    if (*pindex != -1)
    {
        CRect rcItem;
        GetItemRect(*pindex, &rcItem);
        // Shrink it so it's just the last 18 pixels
        rcItem.left = rcItem.right - 18;
        if (rcItem.PtInRect(point))
        {
            overClose = *pindex;
        }
    }
    return overClose;
}

void CMDITabsDialogBar::OnMouseMove(UINT nFlags, CPoint point)
{
    int hoverIndex;
    int overClose = IsOverClose(point, &hoverIndex);
    if (hoverIndex != _hoverIndex)
    {
        CRect invalidateRect;
        if (_hoverIndex != -1)
        {
            GetItemRect(_hoverIndex, &invalidateRect);
            InvalidateRect(&invalidateRect);
        }
        if (hoverIndex != -1)
        {
            GetItemRect(hoverIndex, &invalidateRect);
            InvalidateRect(&invalidateRect);
        }
        _hoverIndex = hoverIndex;
    }

    bool invalidate = overClose != _overCloseIcon;
    _overCloseIcon = overClose;

    if (!_fMouseWithin)
    {
        // Track mouseleaves.
        TRACKMOUSEEVENT eventTrack;
        eventTrack.cbSize = sizeof(eventTrack);
        eventTrack.dwFlags = TME_LEAVE;
        eventTrack.hwndTrack = m_hWnd;
        bool fMouseWithin = !!TrackMouseEvent(&eventTrack);
        if (fMouseWithin != _fMouseWithin)
        {
            _fMouseWithin = fMouseWithin;
            invalidate = true;
        }
    }

    if (invalidate)
    {
        Invalidate();
    }

    __super::OnMouseMove(nFlags, point);
}

LRESULT  CMDITabsDialogBar::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    if (_fMouseWithin)
    {
        _hoverIndex = -1;
        _fMouseWithin = false;
        Invalidate();
    }
    return 0;
}   

void CMDITabsDialogBar::OnLButtonDown(UINT nFlags, CPoint point)
{
    int index;
    int overClose = IsOverClose(point, &index);
    if (overClose != -1)
    {
        _capturing = true;
        SetCapture();
    }

    __super::OnLButtonDown(nFlags, point);
}

void CMDITabsDialogBar::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (_capturing)
    {
        int index;
        int overClose = IsOverClose(point, &index);
        if (overClose != -1)
        {
            // Close this document
            CDocument *pDoc = NULL;
            TCITEM tcitem = { 0 };
            tcitem.mask = TCIF_PARAM;
            if (GetItem(index, &tcitem))
            {
                CMDITabChildWnd *pActive = reinterpret_cast<CMDITabChildWnd*>(tcitem.lParam);
                pActive->PostMessageA(WM_CLOSE);
            }
        }
        _capturing = false;
        ReleaseCapture();
    }

    __super::OnLButtonUp(nFlags, point);
}
