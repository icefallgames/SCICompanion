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

// PicCommandSidePane.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "PicCommandSidePane.h"
#include "PicDoc.h"
#include "Pic.h"
#include "PicOperations.h"
#include "CObjectWrap.h"
#include "PaletteOperations.h"
#include "ImageUtil.h"
#include "format.h"
#include "WindowsUtil.h"
#include "PicCommands.h"

const int CommandTabIndex = 0;
const int PolyTabIndex = 1;

char c_szPolyNamePrefix[] = "P_";

// PicCommandSidePane dialog

PicCommandSidePane::PicCommandSidePane(bool showPalette, CWnd* pParent /*=NULL*/)
    : CExtDialogFwdCmd(showPalette ? PicCommandSidePane::IDD_VGA : PicCommandSidePane::IDD, pParent)
{
    _fAttached = FALSE;
    _iUserSelectedPos = LB_ERR;
    _iCurrentPicPos = LB_ERR;
    _showPalette = showPalette;

    // Load our accelerators
	HINSTANCE hInst = AfxFindResourceHandle(MAKEINTRESOURCE(IDR_ACCELERATORPICCOMMANDS), RT_ACCELERATOR);
	_hAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_ACCELERATORPICCOMMANDS));
}

PicCommandSidePane::~PicCommandSidePane()
{
}

#define STR_NOSCRIPT "no script"

void PicCommandSidePane::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

    DDX_Control(pDX, ID_EDIT_COPY, m_wndCopy);
    DDX_Control(pDX, ID_CROPCOMMANDS, m_wndCrop);
    DDX_Control(pDX, ID_EDIT_DELETE, m_wndDelete);
    DDX_Control(pDX, IDC_GOTOSCRIPT, m_wndGotoScript);
    m_wndGotoScript.m_bUseStdCommandNotification = true;
    m_wndGotoScript.SetWindowText(STR_NOSCRIPT);
    m_wndGotoScript.EnableWindow(FALSE);

    DDX_Control(pDX, IDC_LISTCOMMANDS, m_wndListCommands);

    if (_showPalette)
    {
        DDX_Control(pDX, IDC_STATIC2, m_wndPalette);
        DDX_Control(pDX, ID_PIC_EDITPALETTE, m_wndEditPaletteButton);
        m_wndEditPaletteButton.SetIcon(IDI_PALETTEVGA, 0, 0, 0, 24, 24);
        DDX_Control(pDX, IDC_COMBO_PALETTE, m_wndComboPaletteChoices);

        DDX_Control(pDX, ID_EDIT_IMPORTPICBACKGROUND, m_wndSetBackground);

        DDX_Control(pDX, IDC_TABWHICHLIST, m_wndTabWhichList);

        DDX_Control(pDX, IDC_LISTPOLYGONS, m_wndListPolygons);
        DDX_Control(pDX, IDC_STATICPOLYTYPE, m_wndStaticPolyType);
        DDX_Control(pDX, IDC_COMBOPOLYTYPE, m_wndComboPolyType);
        DDX_Control(pDX, IDC_CHECKSHOWPOLYS, m_wndCheckShowPolys);
        DDX_Control(pDX, IDC_EDIT_POLYPOINTS, m_wndEditPolyPoints);
        DDX_Control(pDX, ID_UPLOAD, m_wndUploadPointsButton);
        m_wndUploadPointsButton.SetIcon(IDI_UPLOAD, 0, 0, 0, 24, 24);
        DDX_Control(pDX, ID_UPLOADNAME, m_wndUploadNameButton);
        m_wndUploadNameButton.SetIcon(IDI_UPLOAD, 0, 0, 0, 24, 24);
        DDX_Control(pDX, IDC_STATICNAME, m_wndStaticPolyName);
        DDX_Control(pDX, IDC_EDITNAME, m_wndEditPolyName);

        // Show polygons by default
        TCITEM tcitem = {};
        tcitem.mask = TCIF_TEXT;
        tcitem.pszText = "Commands";
        m_wndTabWhichList.InsertItem(CommandTabIndex, &tcitem);
        tcitem.pszText = "Polygons";
        m_wndTabWhichList.InsertItem(PolyTabIndex, &tcitem);
        m_wndTabWhichList.SetCurSel(PolyTabIndex);
        _ShowPolyOrCommands();
    }
}


BEGIN_MESSAGE_MAP(PicCommandSidePane, CExtDialogFwdCmd)
    ON_WM_DRAWITEM()
    ON_LBN_SELCHANGE(IDC_LISTCOMMANDS, OnSelChange)
    ON_LBN_SELCHANGE(IDC_LISTPOLYGONS, OnPolySelChange)
    ON_COMMAND(ID_CROPCOMMANDS, OnCropCommands)
    // The pic document's ID_EDIT_COPY copies the pic background to the clipboard,
    // so we handle our own.
    // However, the pic document's ID_EDIT_PASTE handles commands on the clipboard.
    ON_COMMAND(ID_EDIT_COPY, OnCopyCommands)
    ON_COMMAND(ID_EDIT_DELETE, OnDeleteCommands)
    ON_COMMAND(ID_EDIT_CUT, OnCutCommands)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_COMMAND(IDC_GOTOSCRIPT, OnGotoScript)
    ON_BN_CLICKED(IDC_CHECKSHOWPOLYS, OnBnClickedShowPolys)
    ON_BN_CLICKED(ID_UPLOAD, PushEditPointsToPoly)
    ON_BN_CLICKED(ID_UPLOADNAME, PushNameToPoly)
    ON_CBN_SELCHANGE(IDC_COMBOPOLYTYPE, OnCbnSelchangeComboPolyType)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TABWHICHLIST, OnTcnSelchangeTabWhichList)
    ON_CBN_SELCHANGE(IDC_COMBO_PALETTE, OnPaletteSelection)
END_MESSAGE_MAP()

void PicCommandSidePane::_ShowPolyOrCommands()
{
    if (m_wndListPolygons.GetSafeHwnd())
    {
        bool showPolys = (m_wndTabWhichList.GetCurSel() == 1);
        int cmdCommand = showPolys ? SW_HIDE : SW_SHOW;
        int cmdPoly = showPolys ? SW_SHOW : SW_HIDE;
        m_wndListCommands.ShowWindow(cmdCommand);
        m_wndCopy.ShowWindow(cmdCommand);
        m_wndCrop.ShowWindow(cmdCommand);
        m_wndListPolygons.ShowWindow(cmdPoly);
        m_wndStaticPolyType.ShowWindow(cmdPoly);
        m_wndComboPolyType.ShowWindow(cmdPoly);
        m_wndCheckShowPolys.ShowWindow(cmdPoly);
        m_wndEditPolyPoints.ShowWindow(cmdPoly);
        m_wndUploadPointsButton.ShowWindow(cmdPoly);
        m_wndStaticPolyName.ShowWindow(cmdPoly);
        m_wndEditPolyName.ShowWindow(cmdPoly);
        m_wndUploadNameButton.ShowWindow(cmdPoly);
    }
}

void PicCommandSidePane::OnTcnSelchangeTabWhichList(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;
    _ShowPolyOrCommands();
    _OnUpdateCommands();
}

void PicCommandSidePane::OnBnClickedShowPolys()
{
    if (GetDocument())
    {
        GetDocument()->SetShowPolygons(m_wndCheckShowPolys.GetCheck() == BST_CHECKED);
    }
}

void PicCommandSidePane::PushNameToPoly()
{
    const SCIPolygon *polygon = _GetCurrentPolygon();
    int polyIndex = GetDocument()->GetCurrentPolygonIndex();
    if (polygon)
    {
        CString nameTemp;
        m_wndEditPolyName.GetWindowText(nameTemp);
        string name;
        if (!nameTemp.IsEmpty())
        {
            name = c_szPolyNamePrefix;
            name += (PCSTR)nameTemp;
        }
        // If empty, restore to default...

        if (name != polygon->Name)
        {
            // They are different - push the points to the polygon
            GetDocument()->ApplyChanges<PolygonComponent>(
                [&name, polyIndex](PolygonComponent &polygonComponent)
            {
                SCIPolygon *thePoly = polygonComponent.GetAt(polyIndex);
                thePoly->Name = name;
                return WrapHint(PicChangeHint::PolygonsChanged);
            }
            );
        }
    }
}

void PicCommandSidePane::PushEditPointsToPoly()
{
    const SCIPolygon *polygon = _GetCurrentPolygon();
    if (polygon)
    {
        // Set the points of the polygon
        CString editText;
        m_wndEditPolyPoints.GetWindowTextA(editText);

        stringstream ss((PCSTR)editText);
        vector<int16_t> numbers;
        while (1)
        {
            int16_t n;
            ss >> n;
            if (!ss)
            {
                break;
            }
            numbers.push_back(n);
        }

        vector<point16> points;
        for (size_t i = 0; (i + 1) < numbers.size(); i += 2)
        {
            points.emplace_back(numbers[i], numbers[i + 1]);
        }

        int polyIndex = GetDocument()->GetCurrentPolygonIndex();
        if ((points.size() >= 3) && (polygon->Points() != points))
        {
            // They are different - push the points to the polygon
            GetDocument()->ApplyChanges<PolygonComponent>(
                [&points, polyIndex](PolygonComponent &polygonComponent)
            {
                SCIPolygon *thePoly = polygonComponent.GetAt(polyIndex);
                thePoly->Points() = points;
                FixupPolygon(*thePoly);
                return WrapHint(PicChangeHint::PolygonsChanged);
            }
            );
        }
    }
}

void PicCommandSidePane::OnCbnSelchangeComboPolyType()
{
    int selection = m_wndComboPolyType.GetCurSel();
    if (selection != -1)
    {
        int currentPolyIndex = GetDocument()->GetCurrentPolygonIndex();
        GetDocument()->ApplyChanges<PolygonComponent>(
            [currentPolyIndex, selection](PolygonComponent &polygonComponent)
        {
            SCIPolygon *polygon = polygonComponent.GetAt(currentPolyIndex);
            polygon->Type = (PolygonType)selection;
            FixupPolygon(*polygon);
            return WrapHint(PicChangeHint::PolygonsChanged);
        }
            );
    }
}

BOOL PicCommandSidePane::OnEraseBkgnd(CDC *pDC)
{
    return __super::OnEraseBkgnd(pDC);
}


// PicCommandSidePane message handlers

void PicCommandSidePane::OnCropCommands()
{
    // Delete from the current position to the end.
    GetDocument()->RemoveCommandRange(_iCurrentPicPos, -1);
}

void PicCommandSidePane::OnDeleteCommands()
{
    if (m_wndListCommands.IsWindowVisible())
    {
        // Cut, don't copy
        _OnDelete(TRUE, FALSE);
    }
    else
    {
        CPicDoc *pDoc = GetDocument();
        if (pDoc)
        {
            int currentPoly = pDoc->GetCurrentPolygonIndex();
            pDoc->ApplyChanges<PolygonComponent>(
                [currentPoly](PolygonComponent &polygonComponent)
            {
                polygonComponent.DeletePolygon(currentPoly);
                return WrapHint(PicChangeHint::PolygonsChanged);
            }
                );
        }
    }
}

void PicCommandSidePane::OnCutCommands()
{
    // Cut, and copy
    _OnDelete(TRUE, TRUE);
}

void PicCommandSidePane::OnCopyCommands()
{
    // Copy, don't cut
    _OnDelete(FALSE, TRUE);
}

bool PicCommandSidePane::_ShowingPolygons()
{
    if (m_wndTabWhichList.GetSafeHwnd())
    {
        return m_wndTabWhichList.GetCurSel() == PolyTabIndex;
    }
    else
    {
        return false;
    }
}

void PicCommandSidePane::_OnUpdateCommands()
{
    bool showingPolys = _ShowingPolygons();
    BOOL areCommandsSelected = !showingPolys && (m_wndListCommands.SendMessage(LB_GETSELCOUNT, 0, 0) > 0);
    BOOL arePolysSelected = showingPolys && (m_wndListPolygons.GetCurSel() != -1);
    BOOL enableDelete = areCommandsSelected || arePolysSelected;
    if (m_wndCopy.IsWindowEnabled() != areCommandsSelected)
    {
        m_wndCopy.EnableWindow(areCommandsSelected);
    }
    // These calls are actually really expensive (profuis), so only do it when changed
    if (m_wndDelete.IsWindowEnabled() != enableDelete)
    {
        m_wndDelete.EnableWindow(enableDelete);
    }
}

//
// To properly handle commands we do two things here:
// 1) Process our own accelerators
//      - The main frame's accelerator table doesn't get used in dialogs in control bars,
//        so we need to do our own.  So, e.g. Ctrl-Z becomes ID_EDIT_UNDO
// 2) Fwd any commands we receive to our frame
//      if we don't handle them ourselves.
//      - they don't bubble up naturally in dialogs in control bars.
//      - we do this by inheriting from CExtDialogFwdCmd
//
BOOL PicCommandSidePane::PreTranslateMessage(MSG* pMsg)
{
    BOOL fRet = FALSE;
    if (GetFocus() == static_cast<CWnd*>(&m_wndEditPolyPoints))
    {
        fRet = HandleEditBoxCommands(pMsg, m_wndEditPolyPoints);
    }
    if (!fRet && (GetFocus() == static_cast<CWnd*>(&m_wndEditPolyName)))
    {
        fRet = HandleEditBoxCommands(pMsg, m_wndEditPolyName);
        if (!fRet)
        {
            if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
            {
                PushNameToPoly();
                fRet = TRUE;
            }
        }
    }
    if (!fRet && _hAccel && (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST))
    {
        fRet = ::TranslateAccelerator(GetSafeHwnd(), _hAccel, pMsg);
    }
    if (!fRet)
    {
        fRet = __super::PreTranslateMessage(pMsg);
    }
    return fRet;
}

BOOL PicCommandSidePane::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();

    // Set up anchoring for resize
    AddAnchor(IDC_LISTCOMMANDS, CPoint(0, 0), CPoint(100, 100));
    // This doesn't seem to work for this control, and causes weird resizing.
    //AddAnchor(IDC_GOTOSCRIPT, CPoint(100, 0), CPoint(100, 0));        
    if (GetDlgItem(IDC_LISTPOLYGONS))
    {
        AddAnchor(IDC_LISTPOLYGONS, CPoint(0, 0), CPoint(100, 100));
        AddAnchor(IDC_STATICPOLYTYPE, CPoint(0, 0), CPoint(0, 0));
        AddAnchor(IDC_COMBOPOLYTYPE, CPoint(0, 0), CPoint(100, 0));
        AddAnchor(IDC_EDITNAME, CPoint(0, 0), CPoint(100, 0));
        AddAnchor(IDC_CHECKSHOWPOLYS, CPoint(0, 0), CPoint(100, 0));
        AddAnchor(IDC_EDIT_POLYPOINTS, CPoint(0, 100), CPoint(100, 100));
        AddAnchor(ID_UPLOAD, CPoint(100, 100), CPoint(100, 100));
        AddAnchor(ID_UPLOADNAME, CPoint(100, 0), CPoint(100, 0));
    }
    // Hide the sizing grip
    ShowSizeGrip(FALSE);

    if (_showPalette)
    {
        AddAnchor(IDC_STATIC2, CPoint(0, 100), CPoint(100, 100));
        AddAnchor(ID_PIC_EDITPALETTE, CPoint(0, 100), CPoint(0, 100));
        AddAnchor(IDC_COMBO_PALETTE, CPoint(0, 100), CPoint(100, 100));
    }

    return fRet;
}


void PicCommandSidePane::OnSelChange()
{
    // Selection changed.  Time to move the position.
    const PicComponent *pepic = _GetEditPic();
    if (pepic)
    {
        m_wndListCommands.Invalidate(FALSE);
        _iUserSelectedPos = (int)m_wndListCommands.SendMessage(LB_GETCURSEL, 0, 0);
        if (_iUserSelectedPos != LB_ERR)
        {
            if ((size_t)_iUserSelectedPos > GetCommandCount(*pepic))
            {
                _iUserSelectedPos = -1;
            }
            _pDoc->SeekToPos(_iUserSelectedPos);
        }
    }
}

void PicCommandSidePane::OnPolySelChange()
{
    CPicDoc *pDoc = GetDocument();
    if (pDoc)
    {
        // TODO: Clicking on whitespace causes last item to be selected, but not highlighted.
        // Fix: http://stackoverflow.com/questions/21032850/losing-selecteditem-when-clicking-on-a-empty-space-in-a-listbox
        int curSel = (int)m_wndListPolygons.SendMessage(LB_GETCURSEL, 0, 0);
        pDoc->SetCurrentPolygonIndex(curSel);
    }
}

//
// If fCut is true, the commands are deleted from the pic
// If fCopy is true, the commands are copied to the clipboard
//
void PicCommandSidePane::_OnDelete(bool fCut, bool fCopy)
{
    const PicComponent *pepic = _GetEditPic();
    if (pepic)
    {
        int cItems = (int)m_wndListCommands.SendMessage(LB_GETCOUNT, 0, 0);
		std::vector<int> rgiSelected(cItems, 0);
        int iCaretIndexPrev = (int)m_wndListCommands.SendMessage(LB_GETCARETINDEX, 0, 0);
        int iCaretAdjustment = 0;

        int cSelectedItems = (int)m_wndListCommands.SendMessage(LB_GETSELITEMS, cItems, (LPARAM)&rgiSelected[0]);
        if (cSelectedItems > 0)
        {
            // Note, we can't remove the "last one" (actually the first one).
            // Subtract 1 from the item index for every call

            // REVIEW: I don't think this will work too well if there are 
            // multiple ranges, since we'll keep getting selection changes, etc...
            // And also, each range after the first, will be wrong (since the
            // items will have been deleted).
            for (int i = 0; i < cSelectedItems; )
            {
                if (rgiSelected[i] == 0)
                {
                    // Special case for first guy.  If it's item 0, it doesn't count.
                    // Hopefully item 0 would only be included at the beginning.
                    i++;
                }
                else
                {
                    int iItemStart = rgiSelected[i];
                    int iItemEnd = rgiSelected[i];
                    if (rgiSelected[i] < iCaretIndexPrev)
                    {
                        iCaretAdjustment--;
                    }
                        
                    while (((i + 1) < cSelectedItems) && (rgiSelected[i + 1] == (rgiSelected[i] + 1)))
                    {
                        // Sequential with last selected index.
                        i++;
                        iItemEnd = rgiSelected[i];

                        if (rgiSelected[i] < iCaretIndexPrev)
                        {
                            iCaretAdjustment--;
                        }
                    }

                    if (fCopy)
                    {
                        CopyRangeToClipboard(pepic->commands, iItemStart - 1, iItemEnd - 1);
                    }

                    if (fCut)
                    {
                        GetDocument()->RemoveCommandRange(iItemStart - 1, iItemEnd - 1);
                    }

                    i++;
                }
            }
        }

        if (fCut)
        {
            // Adjust the caret if we removed commands
            if (iCaretIndexPrev != LB_ERR)
            {
                // Set selection to where the caret index was.
                m_wndListCommands.SendMessage(LB_SELITEMRANGE, TRUE, MAKELONG(iCaretIndexPrev + iCaretAdjustment, iCaretIndexPrev + iCaretAdjustment));
                m_wndListCommands.SendMessage(LB_SETCARETINDEX, iCaretIndexPrev + iCaretAdjustment, 0);
            }
        }
    }
}

string g_PolyTypes[] =
{
    "Total",
    "Nearest",
    "Barred",
    "Contained",
};

void PolygonListBox::DrawItem(DRAWITEMSTRUCT *pDrawItemStruct)
{
    PicCommandSidePane *pParent = static_cast<PicCommandSidePane*>(GetParent());
    CPicDoc* pDoc = pParent->GetDocument();
    if (pDoc)
    {
        const PolygonComponent *polygonSource = pDoc->GetPolygonComponent();
        if (polygonSource)
        {
            int currentPoly = pDoc->GetCurrentPolygonIndex();

            CDC *pDC = CDC::FromHandle(pDrawItemStruct->hDC);

            // REVIEW: We need to use this font when measuring too.
            HGDIOBJ hFontOld = pDC->SelectObject(&g_PaintManager->m_FontNormalBC);
            // Save these values to restore them when done drawing.
            COLORREF crOldTextColor = pDC->GetTextColor();
            COLORREF crOldBkColor = pDC->GetBkColor();
            int iMode = pDC->GetBkMode();

            // If this item is selected, set the background color 
            // and the text color to appropriate values. Also, erase
            // rect by filling it with the background color.
            bool bSelected = (pDrawItemStruct->itemAction | ODA_SELECT) && (pDrawItemStruct->itemState & ODS_SELECTED);
            if (bSelected)
            {
                pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
                pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
                pDC->FillSolidRect(&pDrawItemStruct->rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
            }
            else
            {
                pDC->FillSolidRect(&pDrawItemStruct->rcItem, crOldBkColor);
            }

            if (pDrawItemStruct->itemID != -1)
            {
                const SCIPolygon *polygon = polygonSource->GetAt(pDrawItemStruct->itemID);

                string text = fmt::format("{0}: {1}", (polygon->Name.empty() ? "Default" : polygon->Name), g_PolyTypes[(int)polygon->Type]);
                pDC->DrawText(
                    text.c_str(),
                    -1,
                    &pDrawItemStruct->rcItem,
                    DT_SINGLELINE | DT_LEFT);
            }

            pDC->SelectObject(hFontOld);
            pDC->SetTextColor(crOldTextColor);
            pDC->SetBkColor(crOldBkColor);
            pDC->SetBkMode(iMode);
        }
    }
}

void PicCommandListBox::DrawItem(DRAWITEMSTRUCT *pDrawItemStruct)
{
    PicCommandSidePane *pParent = static_cast<PicCommandSidePane*>(GetParent());
    CPicDoc* pDoc = pParent->GetDocument();
    if (pDoc)
    {
        const PicComponent *ppic = pDoc->GetPic();
        if (ppic)
        {
            ASSERT(pDrawItemStruct->CtlType == ODT_LISTBOX);
            CDC dc;
            dc.Attach(pDrawItemStruct->hDC);

            // REVIEW: We need to use this font when measuring too.
            HGDIOBJ hFontOld = dc.SelectObject(&g_PaintManager->m_FontNormalBC);

            // Save these values to restore them when done drawing.
            COLORREF crOldTextColor = dc.GetTextColor();
            COLORREF crOldBkColor = dc.GetBkColor();
            int iMode = dc.GetBkMode();

            // If this item is selected, set the background color 
            // and the text color to appropriate values. Also, erase
            // rect by filling it with the background color.
            bool bSelected = (pDrawItemStruct->itemAction | ODA_SELECT) && (pDrawItemStruct->itemState & ODS_SELECTED);
            if (bSelected)
            {
                dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
                dc.SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
                dc.FillSolidRect(&pDrawItemStruct->rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
            }
            else
            {
                dc.FillSolidRect(&pDrawItemStruct->rcItem, crOldBkColor);
            }

            // If this item has the focus, draw a greenbackground around the
            // item's rect - unless it was selected.
            if (!bSelected && (pDrawItemStruct->itemAction | ODA_FOCUS) &&
                (pDrawItemStruct->itemState & ODS_FOCUS))
            {
                CBrush br(RGB(200, 255, 200));
                dc.FrameRect(&pDrawItemStruct->rcItem, &br);
            }

            // If this is the current pos, draw a caret.
            if (pDrawItemStruct->itemID == pParent->GetCurrentPosition())
            {
                // Draw the top of a "caret"
                int y = pDrawItemStruct->rcItem.bottom - 1;
                CPen br1(PS_SOLID, 1, RGB(0, 128, 0));
                HGDIOBJ hGdiObj = dc.SelectObject(&br1);
                dc.MoveTo(CPoint(pDrawItemStruct->rcItem.left, y));
                dc.LineTo(CPoint(pDrawItemStruct->rcItem.right - 1, y));

                y--;
                CPen br2(PS_SOLID, 1, RGB(0, 230, 0));
                dc.SelectObject(&br2);
                dc.MoveTo(CPoint(pDrawItemStruct->rcItem.left, y));
                dc.LineTo(CPoint(pDrawItemStruct->rcItem.right - 1, y));

                dc.SelectObject(hGdiObj);
            }
            else if (pDrawItemStruct->itemID == (pParent->GetCurrentPosition() + 1))
            {
                // FEATURE: Draw the bottom of a "caret"
            }


            if (pDrawItemStruct->itemID != -1)
            {
                TCHAR szName[256];
                if (pDrawItemStruct->itemID == 0)
                {
                    StringCchCopy(szName, ARRAYSIZE(szName), TEXT("Start"));
                }
                else
                {
                    // -1, since we have an additional entry at the top.
                    const PicCommand &command = GetCommandAt(*ppic, pDrawItemStruct->itemID - 1);
                    ASSERT(command.type != PicCommand::None);
                    command.GetName(szName, ARRAYSIZE(szName));

                    if (!bSelected)
                    {
                        // For items that aren't selected, color them accordingly.
                        dc.SetTextColor(Command_GetColor(&command));
                    }
                }

                dc.SetBkMode(TRANSPARENT);

                // Draw the text.
                dc.DrawText(
                    szName,
                    -1,
                    &pDrawItemStruct->rcItem,
                    DT_CENTER | DT_SINGLELINE | DT_VCENTER);
            }

            // Reset the background color and the text color back to their
            // original values.
            dc.SelectObject(hFontOld);
            dc.SetTextColor(crOldTextColor);
            dc.SetBkColor(crOldBkColor);
            dc.SetBkMode(iMode);
            dc.Detach();
        }
    }
}

void PicCommandSidePane::_UpdateItemCount()
{
    const PicComponent *ppic = _GetEditPic();
    if (ppic)
    {
        // Store off the top index
        int iTopIndex = (int)m_wndListCommands.SendMessage(LB_GETTOPINDEX, 0, 0);
        // Add one to the command count, so we have an item that represents "the beginning"
        m_wndListCommands.SendMessage(LB_SETCOUNT, (WPARAM)GetCommandCount(*ppic) + 1, 0);
        // Restore the top index.
        m_wndListCommands.SendMessage(LB_SETTOPINDEX, iTopIndex, 0);
    }
}

void PicCommandSidePane::_SyncPaletteChoice()
{
    if (_showPalette && _pDoc)
    {
        int index = _pDoc->GetPaletteChoice();
        m_wndComboPaletteChoices.SetCurSel(index);
    }
}

void PicCommandSidePane::_UpdatePolyItemCount()
{
    if (m_wndListPolygons.GetSafeHwnd() && _pDoc)
    {
        const PolygonComponent *polygonSource = _pDoc->GetPolygonComponent();
        if (polygonSource)
        {
            // Store off the selection
            int selection = (int)m_wndListPolygons.GetCurSel();
            m_wndListPolygons.SendMessage(LB_SETCOUNT, polygonSource->Polygons().size(), 0);
            // Restore selection
            m_wndListPolygons.SetCurSel(selection);
        }
    }
}

void PicCommandSidePane::_SyncPolyTypeCombo()
{
    const SCIPolygon *polygon = _GetCurrentPolygon();
    if (polygon)
    {
        m_wndComboPolyType.EnableWindow(TRUE);
        m_wndComboPolyType.SetCurSel((int)polygon->Type);
        if (polygon->Name.length() > 2)
        {
            m_wndEditPolyName.SetWindowText(polygon->Name.substr(2, polygon->Name.length() - 2).c_str());
        }
    }
    else
    {
        m_wndComboPolyType.SetCurSel(CB_ERR);
        m_wndComboPolyType.EnableWindow(FALSE);
        m_wndEditPolyName.SetWindowText("");
    }
}

const SCIPolygon *PicCommandSidePane::_GetCurrentPolygon()
{
    const SCIPolygon *polygon = nullptr;
    CPicDoc *pDoc = GetDocument();
    if (pDoc)
    {
        int index = pDoc->GetCurrentPolygonIndex();
        if (index != -1)
        {
            const PolygonComponent *source = pDoc->GetPolygonComponent();
            if (source)
            {
                polygon = source->GetAt(index);
            }
        }
    }
    return polygon;
}

void PicCommandSidePane::_UpdatePaletteChoices()
{
    if (_showPalette && _pDoc)
    {
        m_wndComboPaletteChoices.SetRedraw(FALSE);
        m_wndComboPaletteChoices.ResetContent();
        std::vector<int> &paletteChoices = _pDoc->GetPaletteChoices();
        for (int index = 0; index < (int)paletteChoices.size(); index++)
        {
            m_wndComboPaletteChoices.AddString(_pDoc->GetPaletteChoiceName(index).c_str());
        }
        m_wndComboPaletteChoices.SetCurSel(_pDoc->GetPaletteChoice());
        m_wndComboPaletteChoices.SetRedraw(TRUE);
        OnPaletteSelection();
    }
}

void PicCommandSidePane::_UpdatePalette()
{
    if (_showPalette && _pDoc)
    {
        const PaletteComponent *palette = _pDoc->GetCurrentPaletteComponent();
        if (palette)
        {
            // Find bitmaps
            vector<const Cel*> cels;
            const PicComponent *pepic = _GetEditPic();
            if (pepic)
            {
                for (const PicCommand &command : pepic->commands)
                {
                    if (command.type == PicCommand::CommandType::DrawBitmap)
                    {
                        cels.push_back(command.drawVisualBitmap.pCel);
                    }
                }
            }

            CBitmap bitmapPalette;
            SCIBitmapInfo bmiPalette;
            uint8_t *pBitsDest;
            COLORREF background = g_PaintManager->GetColor(COLOR_3DFACE);
            bitmapPalette.Attach(CreateBitmapFromPaletteComponent(*palette, &bmiPalette, &pBitsDest, &background, &cels));
            m_wndPalette.FromBitmap((HBITMAP)bitmapPalette, bmiPalette.bmiHeader.biWidth, abs(bmiPalette.bmiHeader.biHeight), false);
        }
    }
}

void PicCommandSidePane::UpdateNonView(CObject *pObject)
{
    PicChangeHint hint = GetHint<PicChangeHint>(pObject);
    if (IsFlagSet(hint, PicChangeHint::NewPic))
    {
        // Remove all items
        m_wndListCommands.SetRedraw(FALSE);
        m_wndListCommands.SendMessage(LB_RESETCONTENT, 0, 0);
        _UpdateItemCount();
        m_wndListCommands.SetRedraw(TRUE);
        _UpdatePaletteChoices();
        // Selection changed.  Show a new pic.

        hint |= PicChangeHint::EditPicPos | PicChangeHint::PolygonsChanged | PicChangeHint::Palette | PicChangeHint::PreviewPalette; // New pic, so we'll update pos changed too.
    }

    if (IsFlagSet(hint, PicChangeHint::EditPicInvalid))
    {
        // Something about the editpic changed (i.e. someone added a command)
        // Redraw ourselves.
        // But first - we need to figure out how commands there are.
        _UpdateItemCount();
        _UpdatePalette();
        hint |= PicChangeHint::PolygonsChanged;
        Invalidate(FALSE);
    }

    if (IsFlagSet(hint, PicChangeHint::Palette))
    {
        _UpdatePalette();
        Invalidate(FALSE);
    }

    if (IsFlagSet(hint, PicChangeHint::EditPicPos))
    {
        // The position of the editpic changed.  We should set our selected item.
        // Selection changed.  Show a new pic.
        const PicComponent *ppic = _GetEditPic();
        if (ppic)
        {
            size_t cCommands = GetCommandCount(*ppic);
            size_t iPos = _pDoc->GetDrawManager().GetPos();
            if (iPos == -1)
            {
                iPos = cCommands;
            }
            _iCurrentPicPos = (int)iPos;
            m_wndListCommands.SendMessage(LB_SETCARETINDEX, iPos, 0);
            // We need to redraw everything... when the focus rect changes, the control
            // doesn't seem to send out a drawitem message for the item it was removed from.
            m_wndListCommands.Invalidate(FALSE);
        }
    }

    if (_showPalette)
    {
        if (IsFlagSet(hint, PicChangeHint::PreviewPalette))
        {
            _SyncPaletteChoice();
            _UpdatePalette();
        }

        if (IsFlagSet(hint, PicChangeHint::PolygonsChanged))
        {
            _UpdatePolyItemCount();
            hint |= PicChangeHint::PolygonChoice;
        }

        if (IsFlagSet(hint, PicChangeHint::PolygonChoice))
        {
            _SyncPolyChoice();
        }
    }

    _OnUpdateCommands();
}

void PicCommandSidePane::_SyncPolyChoice()
{
    if (_showPalette && GetDocument())
    {
        int index = GetDocument()->GetCurrentPolygonIndex();
        m_wndListPolygons.SetCurSel(index);
        m_wndCheckShowPolys.SetCheck(GetDocument()->GetShowPolygons() ? BST_CHECKED : BST_UNCHECKED);

        std::string name;
        std::stringstream ss;
        const PolygonComponent *polygonSource = GetDocument()->GetPolygonComponent();
        if (polygonSource)
        {
            const SCIPolygon *polygon = polygonSource->GetAt(index);
            if (polygon)
            {
                for (auto &point : polygon->Points())
                {
                    if (ss.tellp())
                    {
                        ss << " ";
                    }
                    ss << point.x << " " << point.y;
                }
                name = polygon->Name;
            }
        }
        m_wndEditPolyPoints.SetWindowText(ss.str().c_str());
        m_wndEditPolyName.SetWindowText(name.c_str());
        m_wndEditPolyName.EnableWindow(index != -1);
        m_wndUploadNameButton.EnableWindow(index != -1);
        m_wndUploadPointsButton.EnableWindow(index != -1);
        _SyncPolyTypeCombo();
    }
}

const PicComponent *PicCommandSidePane::_GetEditPic()
{
    const PicComponent *pepic = nullptr;
    // Selection changed.  Show a new pic.
    CPicDoc* pDoc = GetDocument();
    if (pDoc)
    {
        pepic = pDoc->GetPic();
    }
    return pepic;
}

void PicCommandSidePane::SetDocument(CDocument *pDoc)
{
    _pDoc = static_cast<CPicDoc*>(pDoc);
    UpdateNonView(&WrapHint(PicChangeHint::NewPic));
    if (_pDoc)
    {
        _pDoc->AddNonViewClient(this);

        _SyncPolyChoice();

        // Update the script link.
        std::string text = "script: ";
        if (_pDoc->GetNumber() != -1)
        {
            text += appState->GetResourceMap().Helper().FigureOutName(ResourceType::Script, _pDoc->GetNumber(), NoBase36);
            m_wndGotoScript.SetWindowText(text.c_str());
            m_wndGotoScript.EnableWindow(TRUE);
        }
        else
        {
            m_wndGotoScript.EnableWindow(FALSE);
        }
    }
}

void PicCommandSidePane::OnPaletteSelection()
{
    if (_pDoc)
    {
        int curSel = m_wndComboPaletteChoices.GetCurSel();
        if (curSel != CB_ERR)
        {
            _pDoc->SetPaletteChoice(curSel, false);
        }
    }
}

void PicCommandSidePane::OnGotoScript()
{
    appState->OpenScript(_pDoc->GetNumber());
}