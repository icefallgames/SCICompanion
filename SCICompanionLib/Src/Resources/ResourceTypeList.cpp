// ResourceTypeList.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "ResourceTypeList.h"


// CResourceTypeList

IMPLEMENT_DYNAMIC(CResourceTypeList, CListBox)
CResourceTypeList::CResourceTypeList()
{
}

CResourceTypeList::~CResourceTypeList()
{
}

const int c_commands[] = { ID_SHOW_VIEWS, ID_SHOW_PICS, ID_SHOW_SCRIPTS, ID_SHOW_TEXTS, ID_SHOW_SOUNDS,
    ID_SHOW_VOCABS, ID_SHOW_FONTS, ID_SHOW_CURSORS, ID_SHOW_PATCHES, ID_SHOW_PALETTES, ID_SHOW_MESSAGES, ID_SHOW_HEAPS };


BEGIN_MESSAGE_MAP(CResourceTypeList, CListBox)
    ON_WM_CREATE()
    ON_CONTROL_REFLECT(LBN_DBLCLK, OnDblClick)
END_MESSAGE_MAP()


// CResourceTypeList message handlers

void CResourceTypeList::OnDblClick()
{
    int i = GetCurSel();
    if (i != LB_ERR)
    {
        // Open the resource manager frame to the right spot.
        UINT commandId = (UINT)GetItemData(i);
        AfxGetMainWnd()->PostMessage(WM_COMMAND, commandId);
    }
}

void CResourceTypeList::DrawItem(LPDRAWITEMSTRUCT pDrawItemStruct)
{
    CDC dcTmp;
    dcTmp.Attach(pDrawItemStruct->hDC);
    CRect rcItem = pDrawItemStruct->rcItem;
	CExtMemoryDC dc(&dcTmp, &rcItem);

    if (pDrawItemStruct->itemID != -1)
    {
        UINT commandId = (UINT)GetItemData(pDrawItemStruct->itemID);
        //UINT commandId = (UINT)pDrawItemStruct->itemData;
        CExtCmdItem *pCmdItem = g_CmdManager->CmdGetPtr(appState->_pszCommandProfile, commandId);
        ASSERT(pCmdItem); // Or else it didn't get registered.
        CExtCmdIcon *pCmdIcon = g_CmdManager->CmdGetIconPtr(appState->_pszCommandProfile, commandId);
        ASSERT(pCmdIcon);

        int nIconAreaWidth = 28;
	    bool bSelected = (pDrawItemStruct->itemState & ODS_SELECTED) != 0;
	    CExtPaintManager::PAINTMENUITEMDATA _pmid(
		    NULL,
		    rcItem,
		    rcItem,
		    nIconAreaWidth,
		    "",
		    "",
		    pCmdIcon,
		    false,
		    bSelected,
		    false,
		    false,
		    false,
		    true,
		    false,
		    false,
		    false,
		    0,
		    NULL
		    );    
        g_PaintManager->PaintMenuItem(dc, _pmid);

        PCSTR pszText = pCmdItem->m_sMenuText;

	    CRect rcMeasureText(0, 0, 0, 0);
	    dc.DrawText(pszText, (int)strlen(pszText), &rcMeasureText, DT_CALCRECT | DT_SINGLELINE | DT_LEFT);
	    CRect rcDrawText(rcItem);
	    rcDrawText.OffsetRect(0, (rcDrawText.Height() - rcMeasureText.Height()) / 2);
	    rcDrawText.left += (nIconAreaWidth+4);

	    CFont * pOldFont = dc.SelectObject(bSelected  ? &g_PaintManager->m_FontBold : &g_PaintManager->m_FontNormal);
	    int nOldBkMode = dc.SetBkMode(TRANSPARENT);
        COLORREF clrText = bSelected ? CExtPaintManager::CLR_3DHILIGHT_IN : CExtPaintManager::CLR_TEXT_OUT;
	    COLORREF clrOldText = dc.SetTextColor(g_PaintManager->GetColor(clrText, (CObject*)this));
	    dc.DrawText(pszText, &rcDrawText, DT_LEFT);
	    dc.SetTextColor(clrOldText);
	    dc.SetBkMode(nOldBkMode);
	    dc.SelectObject(pOldFont);
    }
	dc.__Flush();
	dcTmp.Detach();

}

void CResourceTypeList::MeasureItem(LPMEASUREITEMSTRUCT pmi)
{
    // We're fixed height.
    pmi->itemHeight = 30;
}


int CResourceTypeList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    ASSERT((lpCreateStruct->style & LBS_NODATA) == 0);
    if (__super::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Add the items to the list.
    for (int i = 0; i < ARRAYSIZE(c_commands); i++)
    {
        int iIndex = AddString(""); // Empty string is fine, since we custom draw.
        SetItemData(iIndex, c_commands[i]);
    }
    
    // That was easy.
    return 0;
}