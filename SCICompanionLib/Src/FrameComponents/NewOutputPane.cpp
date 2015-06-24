#include "stdafx.h"
#include "NewOutputPane.h"
#include "AppState.h"

using namespace std;

// Corresponds to OutputPaneType
const PCSTR c_szTabNames[] =
{
    "Compiler",
    "Find Results",
    "Debug Output",
};

NewOutputPane::NewOutputPane(CWnd* pParent /*=NULL*/)
    : CExtDialogFwdCmd(NewOutputPane::IDD, pParent)
{

}

void NewOutputPane::ClearResults(OutputPaneType type)
{
    std::vector<CompileResult> &results = _results[(int)type];
    results.clear();
    _SyncCombo();

    // Use default Prof-UIS font...
    // HACK: See comment in CMDITabsDialogBar::AddTab.
    m_wndListBox.SetFont(&g_PaintManager->m_FontNormal);
}

void NewOutputPane::_SyncCombo()
{
    std::vector<CompileResult> &results = _results[_GetCurrentType()];
    m_wndListBox.SendMessage(LB_SETCOUNT, (WPARAM)results.size(), 0);
    m_wndListBox.Invalidate();
}

void NewOutputPane::AddBatch(OutputPaneType type, std::vector<CompileResult> &compileResults)
{
    std::vector<CompileResult> &results = _results[(int)type];
    copy(compileResults.begin(), compileResults.end(), back_inserter(results));
    _SyncCombo();
}

int NewOutputPane::_GetCurrentType()
{
    return m_wndTabs.GetCurSel();
}

void NewOutputPane::FinishAdd(OutputPaneType type)
{
    _SyncCombo();
}

void NewOutputPane::ShowPage(OutputPaneType type)
{
    m_wndTabs.SetCurSel((int)type);
    _SyncTabSelection();
}

void NewOutputPane::_SyncTabSelection()
{
    // Clear selection when we switch tabs
    m_wndListBox.SetSel(-1, FALSE);
    _SyncCombo();
}

void NewOutputPane::DoDataExchange(CDataExchange* pDX)
{
    __super::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_TAB1, m_wndTabs);
    DDX_Control(pDX, IDC_LIST1, m_wndListBox);

    AddAnchor(IDC_TAB1, CPoint(0, 100), CPoint(100, 100));
    AddAnchor(IDC_LIST1, CPoint(0, 0), CPoint(100, 100));

    for (int i = 0; i < ARRAYSIZE(c_szTabNames); i++)
    {
        TCITEM tcitem = {};
        tcitem.mask = TCIF_TEXT;
        tcitem.pszText = const_cast<char*>(c_szTabNames[i]);
        m_wndTabs.InsertItem(i, &tcitem);
    }
    m_wndTabs.SetCurSel(0);
}

BOOL NewOutputPane::OnInitDialog()
{
    BOOL fRet = __super::OnInitDialog();
    ShowSizeGrip(FALSE);
    return fRet;
}

BEGIN_MESSAGE_MAP(NewOutputPane, CExtDialogFwdCmd)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &NewOutputPane::OnTcnSelchangeTab1)
    ON_WM_DRAWITEM()
    ON_LBN_DBLCLK(IDC_LIST1, &NewOutputPane::OnLbnDblclkList1)
    // Can't get these to work here, so we'll use TranslateAccelerator
    //ON_COMMAND(ID_EDIT_COPY, OnCopy)
    //ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateCopy)
END_MESSAGE_MAP()


void NewOutputPane::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;
    _SyncTabSelection();
}

void NewOutputPane::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    if (nIDCtl == IDC_LIST1)
    {
        DrawItem(lpDrawItemStruct);
    }
    else
    {
        __super::OnDrawItem(nIDCtl, lpDrawItemStruct);
    }
}

void NewOutputPane::DrawItem(DRAWITEMSTRUCT *pDrawItemStruct)
{
    assert(pDrawItemStruct->CtlType == ODT_LISTBOX);
    assert(pDrawItemStruct->CtlID == IDC_LIST1);

    CDC *pDC = CDC::FromHandle(pDrawItemStruct->hDC);

    //HGDIOBJ hFontOld = pDC->SelectObject(&g_PaintManager->m_FontNormalBC);
    HGDIOBJ hFontOld = pDC->SelectObject(&g_PaintManager->m_FontNormal);
    COLORREF crOldTextColor = pDC->GetTextColor();
    int iMode = pDC->GetBkMode();
    COLORREF crOldBkColor = pDC->GetBkColor();
    pDC->FillSolidRect(&pDrawItemStruct->rcItem, crOldBkColor);

    std::vector<CompileResult> &results = _results[_GetCurrentType()];

    if ((pDrawItemStruct->itemID != -1) && (pDrawItemStruct->itemID < (int)results.size()))
    {
        CompileResult &result = results[pDrawItemStruct->itemID];

        if (pDrawItemStruct->itemState & ODS_SELECTED)
        {
            pDC->SetTextColor(g_PaintManager->GetColor(COLOR_HIGHLIGHTTEXT));
            pDC->SetBkColor(g_PaintManager->GetColor(COLOR_HIGHLIGHT));
            pDC->SetBkMode(OPAQUE);
        }
        else
        {
            pDC->SetTextColor(RGB(0, 0, 0));
            pDC->SetBkMode(TRANSPARENT);
        }

        // Draw the text.
        pDC->DrawText(
            result.GetMessageA().c_str(),
            -1,
            &pDrawItemStruct->rcItem,
            DT_SINGLELINE | DT_LEFT);
    }

    pDC->SelectObject(hFontOld);
    pDC->SetTextColor(crOldTextColor);
    pDC->SetBkColor(crOldBkColor);
    pDC->SetBkMode(iMode);
}

void NewOutputPane::OnLbnDblclkList1()
{
    int iIndex = m_wndListBox.GetCurSel();
    std::vector<CompileResult> &results = _results[_GetCurrentType()];
    if ((iIndex != LB_ERR) && (iIndex < (int)results.size()))
    {
        CompileResult &result = results[iIndex];
        if (result.CanGotoScript())
        {
            ScriptId script = result.GetScript();
            appState->OpenScriptAtLine(script, result.GetLineNumber());
        }
        else if (result.GetResourceType() != ResourceType::None)
        {
            appState->OpenMostRecentResourceAt(result.GetResourceType(), (uint16_t)result.GetLineNumber(), result.GetColumn());
        }
    }
}

/*
void NewOutputPane::OnUpdateCopy(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(m_wndListBox.GetSelCount() > 0);
}*/

void NewOutputPane::OnCopy()
{
    std::stringstream output;
    std::vector<CompileResult> &results = _results[_GetCurrentType()];
    int cSelectedItems = m_wndListBox.GetSelCount();
    vector<int> selectedIndices;
    selectedIndices.assign(cSelectedItems, 0);
    cSelectedItems = m_wndListBox.GetSelItems(cSelectedItems, &selectedIndices[0]);
    for (int i = 0; i < cSelectedItems; i++)
    {
        int index = selectedIndices[i];
        if (index < (int)results.size())
        {
            output << results[index].GetMessageA();
            output << "\r\n";
        }
    }

    string finalString = output.str();

    size_t cchBuffer = finalString.length() + 1;
    GlobalAllocGuard globalAlloc(GMEM_MOVEABLE, cchBuffer * sizeof(char));
    if (globalAlloc.Global)
    {
        GlobalLockGuard<PSTR> globalLock(globalAlloc);
        if (globalLock.Object)
        {
            StringCchCopy(globalLock.Object, cchBuffer, finalString.c_str());
            globalLock.Unlock();

            OpenClipboardGuard openClip(this);
            if (openClip.IsOpen())
            {
                EmptyClipboard();
                if (SetClipboardData(CF_TEXT, globalAlloc.Global))
                {
                    globalAlloc.RelinquishOwnership();
                }
            }
        }
    }
}
#define VK_C        67
BOOL NewOutputPane::PreTranslateMessage(MSG* pMsg)
{
    BOOL fRet = FALSE;
    if (GetFocus() == static_cast<CWnd*>(&m_wndListBox))
    {
        if (pMsg->message == WM_KEYDOWN)
        {
            if (GetKeyState(VK_CONTROL) & 0x8000)
            {
                if (pMsg->wParam == VK_C)
                {
                    OnCopy();
                    fRet = TRUE;
                }
            }
        }
    }
    if (!fRet)
    {
        fRet = __super::PreTranslateMessage(pMsg);
    }
    return fRet;
}