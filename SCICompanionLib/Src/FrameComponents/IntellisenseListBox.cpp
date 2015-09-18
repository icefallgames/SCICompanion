// IntellisenseListBox.cpp : implementation file
//

//
// Listbox that is populated with intellisense results.
// Intellisense is currently disabled as a result of moving to a different parser.
//

#include "stdafx.h"
#include "AppState.h"
#include "IntellisenseListBox.h"
#include "ScriptView.h"
#include "AutoCompleteContext.h"


// CIntellisenseListBox

IMPLEMENT_DYNAMIC(CIntellisenseListBox, CListBox)

CIntellisenseListBox::CIntellisenseListBox()
{
    _pClient = NULL;
}


CIntellisenseListBox::~CIntellisenseListBox()
{
}

int CIntellisenseListBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Get a nice font (menu font) to use.
    NONCLIENTMETRICS metrics = { 0 };
    metrics.cbSize = sizeof(metrics);
    if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0))
    {
        m_font.CreateFontIndirect(&metrics.lfMenuFont);
        SetFont(&m_font);
    }

    // Load an image list.
    _imageList.Create(IDB_CLASSBROWSER, 16, 0, RGB(255, 0, 255));
    return 0;
}

void CIntellisenseListBox::OnDestroy()
{

}

BOOL CIntellisenseListBox::GetSelectedText(CString &strText)
{
    BOOL fRet = (GetCurSel() != LB_ERR);
    if (fRet)
    {
         GetText(GetCurSel(), strText);
    }
    return fRet;
       
}

BEGIN_MESSAGE_MAP(CIntellisenseListBox, CListBox)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblClick)
END_MESSAGE_MAP()


void CIntellisenseListBox::OnDblClick()
{
    if (_pClient)
    {
        _pClient->OnACDoubleClick();
    }
}

// Map some wellknown defines to colours
const PCTSTR c_szColorDefines[] =
{
    TEXT("clBLACK"),
    TEXT("clNAVY"),
    TEXT("clGREEN"),
    TEXT("clTEAL"),
    TEXT("clMAROON"),
    TEXT("clPURPLE"),
    TEXT("clBROWN"),
    TEXT("clSILVER"),
    TEXT("clGREY"),
    TEXT("clBLUE"),
    TEXT("clLIME"),
    TEXT("clCYAN"),
    TEXT("clRED"),
    TEXT("clFUCHSIA"),
    TEXT("clYELLOW"),
    TEXT("clWHITE"),
};

// Map some wellknown defines to colours
const PCTSTR c_szControlDefines[] =
{
    TEXT("ctlBLACK"),
    TEXT("ctlNAVY"),
    TEXT("ctlGREEN"),
    TEXT("ctlTEAL"),
    TEXT("ctlMAROON"),
    TEXT("ctlPURPLE"),
    TEXT("ctlBROWN"),
    TEXT("ctlSILVER"),
    TEXT("ctlGREY"),
    TEXT("ctlBLUE"),
    TEXT("ctlLIME"),
    TEXT("ctlCYAN"),
    TEXT("ctlRED"),
    TEXT("ctlFUCHSIA"),
    TEXT("ctlYELLOW"),
    TEXT("ctlWHITE"),
};
// STLCLEANUP
BOOL _MapDefineToColor(PCTSTR pszDefine, COLORREF *pcr)
{
    BOOL fRet = FALSE;
    int i = 0;
    for (i = 0; i < ARRAYSIZE(c_szColorDefines); i++)
    {
        if (0 == lstrcmp(c_szColorDefines[i], pszDefine))
        {
            *pcr = EGA_TO_COLORREF(i);
            fRet = TRUE;
            break;
        }
    }
    if (!fRet)
    {
        for (i = 0; i < ARRAYSIZE(c_szControlDefines); i++)
        {
            if (0 == lstrcmp(c_szControlDefines[i], pszDefine))
            {
                *pcr = EGA_TO_COLORREF(i);
                fRet = TRUE;
                break;
            }
        }
    }
    return fRet;
}



// CIntellisenseListBox message handlers


void CIntellisenseListBox::DrawItem(LPDRAWITEMSTRUCT pDrawItemStruct)
{
    CDC dc;
    dc.Attach(pDrawItemStruct->hDC);    

    // Save these values to restore them when done drawing.
    COLORREF crOldTextColor = dc.GetTextColor();
    COLORREF crOldBkColor = dc.GetBkColor();
    int iMode = dc.GetBkMode();

    CString str;
    GetText(pDrawItemStruct->itemID, str);
    CRect rc = pDrawItemStruct->rcItem;

    COLORREF color;
    if (_MapDefineToColor(str, &color))
    {
        // Draw a colour
        int iOldMode = dc.SetBkMode(TRANSPARENT);
        dc.Rectangle(rc.left, rc.top + 1, rc.left + 16, rc.top + 14);
        dc.FillSolidRect(rc.left + 1, rc.top + 2, 14, 11, color);
        dc.SetBkMode(iOldMode);
    }
    else
    {
        // Draw a little icon next to the guy.
        _imageList.Draw(&dc, (int)pDrawItemStruct->itemData, CPoint(rc.left, rc.top), ILD_NORMAL);
    }
    rc.left += 18;

    // Now draw the text
    if ((pDrawItemStruct->itemAction | ODA_SELECT) && (pDrawItemStruct->itemState & ODS_SELECTED))
    {
        // Show something for the selected it.
        dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
        dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
        dc.FillSolidRect(&rc, ::GetSysColor(COLOR_HIGHLIGHT));
    }
    else
    {
        if ((int)AutoCompleteIconIndex::Keyword == (int)pDrawItemStruct->itemData)
        {
            // Highlight keywords in blue.
            dc.SetTextColor(RGB(0, 0, 255));
        }
        dc.FillSolidRect(&rc, crOldBkColor);
    }

    rc.left += 3; // Offset a little more.
    dc.DrawText(str, &rc, DT_SINGLELINE);

    dc.SetTextColor(crOldTextColor);
    dc.SetBkColor(crOldBkColor);
    dc.SetBkMode(iMode);
    dc.Detach();
}

int CIntellisenseListBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{
    return 0;
}

void CIntellisenseListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    // We're fixed height.
}

void CIntellisenseListBox::Show(CPoint pt)
{
    int cy = 150;
    CDC *pDC = GetDC();
    if (pDC)
    {
        CRect rc;
        int iHeightLine = pDC->DrawText(TEXT("M"), -1, &rc, DT_CALCRECT);
        int iFudge = 6;
        cy = min(150, iHeightLine * GetCount() + iFudge);
        ReleaseDC(pDC);
    }

    // Before showing ourselves, figure out how big we should be.
    DWORD swpFlags = SWP_NOZORDER | SWP_NOACTIVATE;
    if (IsWindowVisible())
    {
        swpFlags |= SWP_NOMOVE;
    }
    SetWindowPos(NULL, pt.x, pt.y, 210, cy, swpFlags);

    if (!IsWindowVisible())
    {
        ShowWindow(SW_SHOWNOACTIVATE);
    }
}

void CIntellisenseListBox::Hide()
{
    ShowWindow(SW_HIDE);
}

void CIntellisenseListBox::Highlight(PCTSTR pszText)
{
    CRect rcClient;
    GetClientRect(&rcClient);
    int cyItem = GetItemHeight(0);
    int iIndex = FindString(-1, pszText);
    // Center the item.
    int cItemsVisible = rcClient.Height() / cyItem;
    SetCurSel(iIndex);
    iIndex -= (cItemsVisible / 2);
    iIndex = max(0, iIndex);
    SetTopIndex(iIndex);
}


