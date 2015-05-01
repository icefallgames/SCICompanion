// TextView.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "TextView.h"
#include "TextDoc.h"
#include "Text.h"

// CTextView

using namespace std;

IMPLEMENT_DYNCREATE(CTextView, CListView)

CTextView::CTextView()
{
    _iView = LVS_REPORT;
    _bInLabelEdit = false;
    _bEditingNewItem = false;
    _iNewString = 0;
    _fInited = false;
}

CTextView::~CTextView()
{
}

BEGIN_MESSAGE_MAP(CTextView, CListView)
    ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
    ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
    ON_WM_LBUTTONDOWN()
    ON_COMMAND(ID_EDIT_DELETE, OnDelete)
    ON_COMMAND(ID_EDIT_RENAME, OnRename)
    ON_COMMAND(ID_MOVEDOWN, OnMoveDown)
    ON_COMMAND(ID_MOVEUP, OnMoveUp)
    ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, OnUpdateIfSelection)
    ON_UPDATE_COMMAND_UI(ID_MOVEDOWN, OnUpdateIfSelection)
    ON_UPDATE_COMMAND_UI(ID_MOVEUP, OnUpdateIfSelection)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateIfSelection)
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

#define TEXT_INVITATION TEXT("<Click to add new text>")

void _EscapeString(CString &str)
{
    str.Replace(TEXT("\n"), TEXT("\\n"));
    str.Replace(TEXT("\t"), TEXT("\\t"));
}

void _UnescapeString(CString &str)
{
    str.Replace(TEXT("\\n"), TEXT("\n"));
    str.Replace(TEXT("\\t"), TEXT("\t"));
}

#define COL_STRING 0
#define COL_NUMBER 1
#define COL_NOUN 2
#define COL_VERB 3
#define COL_CONDITION 4
#define COL_SEQUENCE 5
#define COL_TALKER 6

const struct
{
    PTSTR pszName; // Should be const, but the LVCOLUMN struct accepts only non-const
    int cx;
}
c_TextColumnInfo [] =
{
    { TEXT("Text"), 600 },
    { TEXT("Number"), 100 },
};

const struct
{
    PTSTR pszName; // Should be const, but the LVCOLUMN struct accepts only non-const
    int cx;
    MessagePropertyFlags Flags;
    int ColumnNumber;
}
c_MessageColumnInfo[] =
{
    { TEXT("Noun"), 60, MessagePropertyFlags::Noun, COL_NOUN },
    { TEXT("Verb"), 60, MessagePropertyFlags::Verb, COL_VERB },
    { TEXT("Cond."), 60, MessagePropertyFlags::Condition, COL_CONDITION },
    { TEXT("Seq."), 60, MessagePropertyFlags::Sequence, COL_SEQUENCE },
    { TEXT("Talker"), 60, MessagePropertyFlags::Talker, COL_TALKER },
};

void CTextView::_InitColumns()
{
    TextComponent &text = GetDocument()->GetResource()->GetComponent<TextComponent>();
    CListCtrl& listCtl = GetListCtrl();

    std::vector<int> orderArray;
    orderArray.push_back(COL_NUMBER);
    for (int i = 0; i < ARRAYSIZE(c_TextColumnInfo); i++)
    {
        // Name
        LVCOLUMN col = { 0 };
        col.mask = LVCF_FMT | LVCF_ORDER | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH; 
        col.iOrder = i;
        col.iSubItem = i;
        col.pszText = c_TextColumnInfo[i].pszName;
        col.cx = c_TextColumnInfo[i].cx;
        col.fmt = LVCFMT_RIGHT;
        _columns.push_back(col);
        listCtl.InsertColumn(i, &col);
    }
    int index = 2;
    for (int i = 0; i < ARRAYSIZE(c_MessageColumnInfo) ; i++)
    {
        if (IsFlagSet(text.Flags, c_MessageColumnInfo[i].Flags))
        {
            LVCOLUMN col = { 0 };
            col.mask = LVCF_FMT | LVCF_ORDER | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
            col.iOrder = index;
            col.iSubItem = c_MessageColumnInfo[i].ColumnNumber;
            col.pszText = c_MessageColumnInfo[i].pszName;
            col.cx = c_MessageColumnInfo[i].cx;
            col.fmt = LVCFMT_RIGHT;
            _columns.push_back(col);
            listCtl.InsertColumn(index, &col);
            orderArray.push_back(index);
            index++;
        }
    }

    orderArray.push_back(COL_STRING);
    listCtl.SetColumnOrderArray(orderArray.size(), &orderArray[0]);
}

void CTextView::_ChangeView()
{
    CListCtrl& listCtl = GetListCtrl();
    LONG_PTR dwStyle = GetWindowLongPtr(listCtl.m_hWnd, GWL_STYLE); 
    dwStyle &= ~LVS_TYPEMASK;
    dwStyle |= _iView;
    SetWindowLongPtr(listCtl.m_hWnd, GWL_STYLE, (LONG)dwStyle);
}

void CTextView::_SwapStrings(int iIndex1, int iIndex2)
{
    CListCtrl& listCtl = GetListCtrl();
    CString strTemp = listCtl.GetItemText(iIndex1, 0);
    listCtl.SetItemText(iIndex1, 0, listCtl.GetItemText(iIndex2, 0));
    listCtl.SetItemText(iIndex2, 0, strTemp);
}

void CTextView::_InsertItem(int iItem, PCTSTR pszString, const TextComponent *text)
{
    CListCtrl& listCtl = GetListCtrl();

    LVITEM item = { 0 };
    item.mask = LVIF_TEXT;
    item.iItem = iItem;
    item.iSubItem = 0;
    item.pszText = const_cast<PTSTR>(pszString);
    listCtl.InsertItem(&item);

    // Now the columns
    for (int iSub = 1; iSub < (int)_columns.size(); iSub++)
    {
        TCHAR szBuf[100];
        LVITEM item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = iItem;
        item.iSubItem = _columns[iSub].iSubItem;
        item.pszText = szBuf;

        int iSubItem = _columns[iSub].iSubItem;
        if (iSubItem == COL_NUMBER)
        {
            StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%d"), iItem);
        }
        else if (text)
        {
            switch (iSubItem)
            {
            case COL_NOUN:
                StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%d"), text->Texts[iItem].Noun);
                break;
            case COL_VERB:
                StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%d"), text->Texts[iItem].Verb);
                break;
            case COL_CONDITION:
                StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%d"), text->Texts[iItem].Condition);
                break;
            case COL_SEQUENCE:
                StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%d"), text->Texts[iItem].Sequence);
                break;
            case COL_TALKER:
                StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%d"), text->Texts[iItem].Talker);
                break;
            }
        }
        listCtl.SetItem(&item);
    }
}


BOOL CTextView::PreCreateWindow(CREATESTRUCT& cs)
{
    // We wish to enable label edits.
    cs.style |= LVS_EDITLABELS | LVS_SINGLESEL | LVS_SHOWSELALWAYS;
	// If I get rid of singlesel, then multiselect happens, but only when I click on labels. WTF.
	// The number column is still single select.
	return __super::PreCreateWindow(cs);
}

void CTextView::OnInitialUpdate()
{
    __super::OnInitialUpdate();
    DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
    GetListCtrl().SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LPARAM(dwExStyle));
}

#define VK_C        67
#define VK_V        86
#define VK_X        88
#define VK_Z        90

BOOL CTextView::PreTranslateMessage(MSG *pMsg)
{
	if (_bInLabelEdit && (pMsg->message >= WM_KEYFIRST) && (pMsg->message <= WM_KEYLAST))
	{
        // Fwd the delete key to the edit control
        if ((pMsg->message != WM_CHAR) && (pMsg->wParam == VK_DELETE))
        {
            HWND hwndEdit = reinterpret_cast<HWND>(SendMessage(LVM_GETEDITCONTROL, 0, 0));
            if (hwndEdit)
            {
                ::SendMessage(hwndEdit, pMsg->message, pMsg->wParam, pMsg->lParam);
                return 1; // Don't dispatch message, we handled it.
            }
        }
    }

    if (_bInLabelEdit && (pMsg->message == WM_KEYDOWN))
    {
        CListCtrl& oListctrl = GetListCtrl();
        CEdit* edit = oListctrl.GetEditControl();
        if (edit)
        {
            if (GetKeyState(VK_CONTROL))
            {
                if (pMsg->wParam == VK_C)
                {
                    edit->Copy();
                    return TRUE;
                }
                if (pMsg->wParam == VK_V)
                {
                    edit->Paste();
                    return TRUE;
                }
                if (pMsg->wParam == VK_X)
                {
                    edit->Cut();
                    return TRUE;
                }
                if (pMsg->wParam == VK_Z)
                {
                    edit->Undo();
                    return TRUE;
                }
            }
        }
    }

    return __super::PreTranslateMessage(pMsg);
}


void CTextView::_OnEndEditingNewItem(PCTSTR pszString, NMLVDISPINFO *plvdi)
{
    CListCtrl& listCtl = GetListCtrl();
    HRESULT hr = E_FAIL;
    CTextDoc *pDoc = GetDocument();
    if (pDoc)
    {
        CString str = pszString;
        _UnescapeString(str);

        pDoc->ApplyChanges<TextComponent>(
            [&](TextComponent &text)
        {
            int iIndex = text.AddString((PCTSTR)str);
            assert(iIndex == plvdi->item.iItem);
            listCtl.SetItemText(plvdi->item.iItem, COL_STRING, pszString);
            return WrapHint(TextChangeHint::Changed);
        }
        );

        hr = S_OK;
    }

    if (SUCCEEDED(hr))
    {
        // Add another "new string"
        _iNewString = listCtl.GetItemCount();
        _InsertItem(_iNewString, TEXT_INVITATION, nullptr);
    }
}

void CTextView::OnUpdateIfSelection(CCmdUI *pCmdUI)
{
    //pCmdUI->Enable((!_bInLabelEdit && (_GetSelectedItem() != -1)) ? 1 : 0);
	pCmdUI->Enable((!_bInLabelEdit && (_GetSelectedItem() != -1)) ? 1 : 0);
}

void CTextView::OnLButtonDown(UINT nFlags, CPoint point)
{
    // Hit test, to see if we were on column 0.
    CListCtrl& listCtl = GetListCtrl();
    BOOL fWentToEditMode = FALSE;
    
    LVHITTESTINFO lvhti = { 0 };
    lvhti.flags = LVHT_ONITEM;
    lvhti.pt = point;
    int nItem = listCtl.SubItemHitTest(&lvhti);
    if (nItem != -1)
    {
        if (lvhti.iSubItem == COL_STRING)
        {
            // The user clicked on the string part.  Go into edit mode.
            listCtl.EditLabel(nItem);
            fWentToEditMode = TRUE;
        }
    }

    if (!fWentToEditMode)
    {
        __super::OnLButtonDown(nFlags, point);
    }
}

void CTextView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    BOOL fProcessed = FALSE;
    if (nChar == VK_TAB)
    {
        // When a tab key is pressed, move selection to the next item, and start a label edit.
        int nItem = _GetSelectedItem();
        if ((nItem != -1) && (nItem < _iNewString))
        {
            nItem++;
            CListCtrl& listCtl = GetListCtrl();
            listCtl.EditLabel(nItem);
            fProcessed = TRUE;

        }
    }
    if (!fProcessed)
    {
        __super::OnKeyDown(nChar, nRepCnt, nFlags);
    }
}

void CTextView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    CListCtrl& listCtl = GetListCtrl();
    *pResult = FALSE;
    NMLVDISPINFO *plvdi = (NMLVDISPINFO *)pNMHDR;
    if (plvdi->item.pszText && _bInLabelEdit)
    {
        if (_bEditingNewItem)
        {
            _OnEndEditingNewItem(plvdi->item.pszText, plvdi);
            _bEditingNewItem = false;
        }
        else
        {
            // Change the string in the resource directly (don't want any updates)
            CTextDoc *pDoc = GetDocument();
            if (pDoc)
            {
                // This is limited to 260 chars.
                // CString str = plvdi->item.pszText;
                CString strResult;
                GetListCtrl().GetEditControl()->GetWindowText(strResult);
                _UnescapeString(strResult);

                pDoc->ApplyChanges<TextComponent>(
                    [&](TextComponent &text)
                {
                    return WrapHint(text.SetStringAt(plvdi->item.iItem, (PCTSTR)strResult));
                }
                );

                // Update the name in the view:
                listCtl.SetItemText(plvdi->item.iItem, 0, strResult);
            }
        }
    }
    _bEditingNewItem = false;
    _bInLabelEdit = false;
}

void CTextView::OnItemDoubleClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    // Enter label edit.
    NM_LISTVIEW *pNMLV = (NMLISTVIEW*)pNMHDR;
    int iViewIndex = pNMLV->iItem;
    CListCtrl& listCtl = GetListCtrl();
    listCtl.EditLabel(iViewIndex);
}

void CTextView::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLVDISPINFO *plvdi = (NMLVDISPINFO *)pNMHDR;
    CEdit* pEdit = GetListCtrl().GetEditControl();

    // Overcome listview's 260 character limit
    pEdit->LimitText(2048); // better than 260!
    CString str = GetListCtrl().GetItemText(plvdi->item.iItem, 0);
    pEdit->SetWindowText(str);
    
    // Did the user click on the "new word" entry?
    _bEditingNewItem = (plvdi->item.iItem == _iNewString);
    _bInLabelEdit = true;
}

//
// Returns -1 if there is no item selected.
//
int CTextView::_GetSelectedItem()
{
    int nItem = -1;
    CListCtrl& listCtl = GetListCtrl();
    POSITION pos = listCtl.GetFirstSelectedItemPosition();
    if (pos != NULL)
    {
        nItem = listCtl.GetNextSelectedItem(pos);
    }
    return nItem;
}


void CTextView::OnDelete()
{
    int nItem = _GetSelectedItem();
    // delete, as long as we're not in a label edit, and we have a selected item, and that selected
    // item is not the "new string" item.
    if (!_bInLabelEdit && (nItem != -1) && (nItem != _iNewString))
    {
        CListCtrl &listCtl = GetListCtrl();
        
        // Delete the item from the resource.  So we can keep tracking of moving selection to the next guy,
        // we'll modify the resource directly (not the document), so this won't cause a full update.
        // That means, though, that we'll need to delete the item from the listview ourselves,
        // and adjust all the resource numbers.
        CTextDoc *pDoc = GetDocument();
        if (pDoc)
        {
            pDoc->ApplyChanges<TextComponent>(
                [&](TextComponent &text)
            {
                return WrapHint(text.DeleteString(nItem));
            }
            );


            // Delete the item from view.
            listCtl.DeleteItem(nItem);
            // And adjust the "new string" item.
            _iNewString--;
#ifdef DEBUG
            // Verify the integrity of the "new string" item.
            CString strNewString = listCtl.GetItemText(_iNewString, 0);
            ASSERT(0 == strNewString.Compare(TEXT_INVITATION));
#endif

            // Now adjust the resource number for all item after nItem;
            int cItems = listCtl.GetItemCount();
            for (int i = nItem; i < cItems; i++)
            {
                TCHAR szBuf[100];
                StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%d"), i);
                listCtl.SetItemText(i, COL_NUMBER, szBuf);
            }

            // Select the next item in the view.
            if (!listCtl.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED) && (nItem > 0))
            {
                listCtl.SetItemState(nItem - 1, LVIS_SELECTED, LVIS_SELECTED);
            }
        }
    }
}

void CTextView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
    TextChangeHint hint = GetHint<TextChangeHint>(pHint);
    if (IsFlagSet(hint, TextChangeHint::Changed))
    {
        // Prepare the listview.
        if (!_fInited && GetDocument())
        {
            _InitColumns();
            _ChangeView();
            _fInited = true;
        }

        CTextDoc *pDoc = GetDocument();
        if (pDoc)
        {
            const TextComponent *pText = pDoc->GetResource()->TryGetComponent<TextComponent>();
            if (pText)
            {
                GetListCtrl().SetRedraw(FALSE);
                int iSelected = _GetSelectedItem(); // Attempt to preserve selection
                GetListCtrl().DeleteAllItems();
                
                int i = 0;
                for (auto &individualString : pText->Texts)
                {
                    CString strText = individualString.Text.c_str();
                    _EscapeString(strText);
                    _InsertItem(i, (PCSTR)strText, pText);
                    ++i;
                }

                // Reserve a spot for a new word
                _iNewString = i;
                _InsertItem(i, TEXT_INVITATION, nullptr);

                // Restore selection
                if (iSelected != -1)
                {
                    GetListCtrl().SetItemState(iSelected, LVIS_SELECTED, LVIS_SELECTED);
                }
                GetListCtrl().SetRedraw(TRUE);
            }
        }
    }
}

TextComponent* CTextView::GetTextComponent() const
{
    TextComponent *pText = nullptr;
    CTextDoc *pDoc = GetDocument();
    if (pDoc)
    {
        pText = pDoc->GetResource()->TryGetComponent<TextComponent>();
    }
    return pText;
}

void CTextView::OnRename()
{
    // Enter label edit.
    int nItem = _GetSelectedItem();
    if (nItem != -1)
    {
        CListCtrl& listCtl = GetListCtrl();
        listCtl.EditLabel(nItem);
    }
}

void CTextView::OnMoveDown()
{
    CTextDoc *pDoc = GetDocument();
    if (pDoc)
    {
        int iSel = _GetSelectedItem();
        pDoc->ApplyChanges<TextComponent>(
            [&](TextComponent &text)
            {
                TextChangeHint hint = text.MoveStringDown(iSel);
                if (IsFlagSet(hint, TextChangeHint::Changed))
                {
                    _SwapStrings(iSel, iSel + 1);
                    GetListCtrl().SetItemState(iSel + 1, LVIS_SELECTED, LVIS_SELECTED);
                }
                return WrapHint(hint);
            }
            );
    }
}


void CTextView::OnMoveUp()
{
    CTextDoc *pDoc = GetDocument();
    if (pDoc)
    {
        int iSel = _GetSelectedItem();

        pDoc->ApplyChanges<TextComponent>(
            [&](TextComponent &text)
        {
            TextChangeHint hint = text.MoveStringUp(iSel);
            if (IsFlagSet(hint, TextChangeHint::Changed))
            {
                _SwapStrings(iSel, iSel - 1);
                GetListCtrl().SetItemState(iSel - 1, LVIS_SELECTED, LVIS_SELECTED);
            }
            return WrapHint(hint);
        }
        );
    }
}


// CTextView diagnostics

#ifdef _DEBUG
void CTextView::AssertValid() const
{
	CListView::AssertValid();
}

void CTextView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
CTextDoc* CTextView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTextDoc)));
    return (CTextDoc*)m_pDocument;
}

#endif //_DEBUG


// CTextView message handlers
