#include "stdafx.h"
#include "ExtTabControl.h"

BEGIN_MESSAGE_MAP(ExtTabControl, CTabCtrl)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void ExtTabControl::DrawItem(LPDRAWITEMSTRUCT pdis)
{
    bool isBottom = (TCS_BOTTOM & GetStyle()) != 0;

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
        CDC dc;
        dc.Attach(pdis->hDC);

        CRect rcPadding(0, 0, 0, 0);
        CRect rcUsable = rc;
        rcUsable.left += 1;
        rcUsable.top += 1;
        if (fSelected)
        {
            rcUsable.OffsetRect(-1, -1);
            rcUsable.bottom++;
        }
        COLORREF accentColor = g_PaintManager->GetColor(COLOR_3DFACE);  // TODO
        COLORREF backgroundColor = g_PaintManager->GetColor(COLOR_3DFACE);
        CRect rcGradient = rcUsable;
        TRIVERTEX *vertices;
        int vertexCount;
        GRADIENT_RECT *gradRects;
        int gradRectCount;
        if (!fSelected)
        {
            backgroundColor = CExtBitmap::stat_HLS_Adjust(backgroundColor, 0.0, -0.14, 0.0);
            accentColor = CExtBitmap::stat_HLS_Adjust(accentColor, 0.0, -0.22, 0.0);

            int dx = rcGradient.Width() * 3 / 10;
            int dy = rcGradient.Height() * 3 / 10;

            // Blend between our main color and the background color for the top part of the gradient (so the gradient isn't so extreme)
            TRIVERTEX selectedVertices[3] =
            {
                { rcGradient.left, rcGradient.top, GetRValue(accentColor) * 256, GetGValue(accentColor) * 256, GetBValue(accentColor) * 256, 65280 },
                // -1 because using right again seems to cause some div by zero and mess up GDI
                { rcGradient.right - 1, rcGradient.top + dy, GetRValue(backgroundColor) * 256, GetGValue(backgroundColor) * 256, GetBValue(backgroundColor) * 256, 65280 },
                { rcGradient.right, rcGradient.bottom, GetRValue(backgroundColor) * 256, GetGValue(backgroundColor) * 256, GetBValue(backgroundColor) * 256, 65280 },
            };
            vertices = selectedVertices;
            vertexCount = ARRAYSIZE(selectedVertices);

            GRADIENT_RECT selectedGradRects[2] = { { 0, 1 }, { 1, 2 } };
            gradRects = selectedGradRects;
            gradRectCount = ARRAYSIZE(selectedGradRects);
        }
        else
        {
            backgroundColor = CExtBitmap::stat_HLS_Adjust(backgroundColor, 0.0, -0.12, 0.0);

            CPoint topLeft = rcGradient.TopLeft();
            CPoint bottomRight = rcGradient.BottomRight();
            if (!isBottom)
            {
                std::swap(topLeft, bottomRight);
            }
            // Blend between our main color and the background color for the top part of the gradient (so the gradient isn't so extreme)
            TRIVERTEX selectedVertices[2] =
            {
                { topLeft.x, topLeft.y, GetRValue(accentColor) * 256, GetGValue(accentColor) * 256, GetBValue(accentColor) * 256, 65280 },
                { bottomRight.x, bottomRight.y, GetRValue(backgroundColor) * 256, GetGValue(backgroundColor) * 256, GetBValue(backgroundColor) * 256, 65280 },
            };
            vertices = selectedVertices;
            vertexCount = ARRAYSIZE(selectedVertices);

            GRADIENT_RECT selectedGradRects[1] = { 0, 1 };
            gradRects = selectedGradRects;
            gradRectCount = ARRAYSIZE(selectedGradRects);
        }

        dc.FillSolidRect(rc, backgroundColor);

        // TODO: Turn these upside down if upside down...
        dc.GradientFill(vertices, vertexCount, gradRects, gradRectCount, GRADIENT_FILL_RECT_V);

        // Use a different font decoration depending on if this is the most recent version of this item.
        bool fNotMostRecent = false;

        rcUsable.OffsetRect(4, 1); // indent

        COLORREF crText = g_PaintManager->GetColor(fSelected ? COLOR_BTNTEXT : COLOR_BTNSHADOW);

        int nOldText = dc.SetTextColor(crText);
        int nOldBk = dc.SetBkMode(TRANSPARENT);
        dc.DrawText(tci.pszText, -1, &rcUsable, DT_SINGLELINE);
        dc.SetBkMode(nOldBk);
        dc.SetTextColor(nOldText);

        dc.Detach();
    }
}

void ExtTabControl::DrawItemBorder(LPDRAWITEMSTRUCT lpdis)
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

void ExtTabControl::DrawMainBorder(LPDRAWITEMSTRUCT lpdis)
{
    // This ends up being a thin line under stuff
    CRect rBorder(lpdis->rcItem);
    CDC* pDC = CDC::FromHandle(lpdis->hDC);
    //pDC->FillSolidRect(&rBorder, RGB(255, 128, 50));
    pDC->FillSolidRect(&rBorder, g_PaintManager->GetColor(COLOR_3DFACE));
}

void ExtTabControl::OnPaint()
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

BOOL ExtTabControl::OnEraseBkgnd(CDC *pDC)
{
    // REVIEW: this is a bit of a hack.  The tab control uses its font to resize itself.
    // However, we use prof-uis paint manager's font to draw.  This font can change (and be deleted)
    // when system settings change.  But we don't know when to update it. So do it here.
    SetFont(&g_PaintManager->m_FontBoldBC);

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