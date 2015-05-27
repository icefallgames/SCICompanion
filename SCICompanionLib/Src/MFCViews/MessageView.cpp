#include "stdafx.h"
#include "AppState.h"
#include "MessageView.h"
#include "MessageDoc.h"
#include "Text.h"
#include "Message.h"

using namespace std;

IMPLEMENT_DYNCREATE(CMessageView, CListView)

CMessageView::CMessageView()
{
    _iView = LVS_REPORT;
    _fInited = false;
}

CMessageView::~CMessageView()
{
}

BEGIN_MESSAGE_MAP(CMessageView, CListView)
    ON_COMMAND(ID_EDIT_DELETE, OnDelete)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
    ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, OnUpdateIfSelection)
    ON_UPDATE_COMMAND_UI(ID_MOVEDOWN, OnUpdateIfSelection)
    ON_UPDATE_COMMAND_UI(ID_MOVEUP, OnUpdateIfSelection)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateIfSelection)
END_MESSAGE_MAP()

void _EscapeString(CString &str);
void _UnescapeString(CString &str);

#define COL_STRING 0
#define COL_NOUN 1
#define COL_VERB 2
#define COL_CONDITION 3
#define COL_SEQUENCE 4
#define COL_TALKER 5

const struct
{
    PTSTR pszName; // Should be const, but the LVCOLUMN struct accepts only non-const
    int cx;
}
c_TextColumnInfo [] =
{
    { TEXT("Text"), 600 },
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

void CMessageView::_InitColumns()
{
    TextComponent &text = GetDocument()->GetResource()->GetComponent<TextComponent>();
    CListCtrl& listCtl = GetListCtrl();

    std::vector<int> orderArray;
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
    int index = 1;
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

void CMessageView::_ChangeView()
{
    CListCtrl& listCtl = GetListCtrl();
    LONG_PTR dwStyle = GetWindowLongPtr(listCtl.m_hWnd, GWL_STYLE); 
    dwStyle &= ~LVS_TYPEMASK;
    dwStyle |= _iView;
    SetWindowLongPtr(listCtl.m_hWnd, GWL_STYLE, (LONG)dwStyle);
}

void CMessageView::_SwapStrings(int iIndex1, int iIndex2)
{
    CListCtrl& listCtl = GetListCtrl();
    CString strTemp = listCtl.GetItemText(iIndex1, 0);
    listCtl.SetItemText(iIndex1, 0, listCtl.GetItemText(iIndex2, 0));
    listCtl.SetItemText(iIndex2, 0, strTemp);
}

void CMessageView::_InsertItem(int iItem, PCTSTR pszString, const TextComponent *text)
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
        if (text)
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
                // Temporary code for message investigation
                // StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%x"), text->Texts[iItem].Style);
                break;
            }
        }
        listCtl.SetItem(&item);
    }
}


BOOL CMessageView::PreCreateWindow(CREATESTRUCT& cs)
{
    // We wish to enable label edits.
    cs.style |= LVS_EDITLABELS | LVS_SINGLESEL | LVS_SHOWSELALWAYS;
	// If I get rid of singlesel, then multiselect happens, but only when I click on labels. WTF.
	// The number column is still single select.
	return __super::PreCreateWindow(cs);
}

void CMessageView::OnInitialUpdate()
{
    __super::OnInitialUpdate();
    DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
    GetListCtrl().SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LPARAM(dwExStyle));
}

BOOL CMessageView::PreTranslateMessage(MSG *pMsg)
{
    return __super::PreTranslateMessage(pMsg);
}



void CMessageView::OnUpdateIfSelection(CCmdUI *pCmdUI)
{
    //pCmdUI->Enable((!_bInLabelEdit && (_GetSelectedItem() != -1)) ? 1 : 0);
	pCmdUI->Enable(((_GetSelectedItem() != -1)) ? 1 : 0);
}

//
// Returns -1 if there is no item selected.
//
int CMessageView::_GetSelectedItem()
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

void CMessageView::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    // If an item was selected, select its corresponding cel.
    NMLISTVIEW *pnmlv = (NMLISTVIEW*)pNMHDR;
    if (pnmlv->uChanged & LVIF_STATE)
    {
        if (pnmlv->uNewState & LVIS_SELECTED)
        {
            CMessageDoc *pDoc = GetDocument();
            if (pDoc)
            {
                pDoc->SetSelectedIndex(pnmlv->iItem);
            }
        }
    }
}

void CMessageView::OnDelete()
{
    int nItem = _GetSelectedItem();
    // delete, as long as we're not in a label edit, and we have a selected item, and that selected
    // item is not the "new string" item.
    if (nItem != -1)
    {
        CListCtrl &listCtl = GetListCtrl();
        
        // Delete the item from the resource.  So we can keep tracking of moving selection to the next guy,
        // we'll modify the resource directly (not the document), so this won't cause a full update.
        // That means, though, that we'll need to delete the item from the listview ourselves,
        // and adjust all the resource numbers.
        CMessageDoc *pDoc = GetDocument();
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

            // Select the next item in the view.
            if (!listCtl.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED) && (nItem > 0))
            {
                listCtl.SetItemState(nItem - 1, LVIS_SELECTED, LVIS_SELECTED);
            }
        }
    }
}

void CMessageView::_UpdateSelection()
{
    CMessageDoc *pDoc = GetDocument();
    if (pDoc)
    {
        int index = pDoc->GetSelectedIndex();
        this->GetListCtrl().SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
        this->GetListCtrl().SetSelectionMark(index);
    }
}

void CMessageView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{

    MessageChangeHint hint = GetHint<MessageChangeHint>(pHint);
    if (IsFlagSet(hint, MessageChangeHint::Selection))
    {
        _UpdateSelection();
    }

    if (IsFlagSet(hint, MessageChangeHint::Changed))
    {
        CMessageDoc *pDoc = GetDocument();

        // Prepare the listview.
        if (!_fInited && GetDocument())
        {
            _InitColumns();
            _ChangeView();
            _fInited = true;
        }

        if (pDoc)
        {
            const TextComponent *pText = pDoc->GetResource()->TryGetComponent<TextComponent>();
            if (pText)
            {
                GetListCtrl().SetRedraw(FALSE);
                GetListCtrl().DeleteAllItems();
                
                int i = 0;
                for (auto &individualString : pText->Texts)
                {
                    CString strText = individualString.Text.c_str();
                    _EscapeString(strText);
                    _InsertItem(i, (PCSTR)strText, pText);
                    ++i;
                }

                _UpdateSelection();
                GetListCtrl().SetRedraw(TRUE);
            }
        }
    }
}

TextComponent* CMessageView::GetTextComponent() const
{
    TextComponent *pText = nullptr;
    CMessageDoc *pDoc = GetDocument();
    if (pDoc)
    {
        pText = pDoc->GetResource()->TryGetComponent<TextComponent>();
    }
    return pText;
}

// CMessageView diagnostics

#ifdef _DEBUG
void CMessageView::AssertValid() const
{
	CListView::AssertValid();
}

void CMessageView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
CMessageDoc* CMessageView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMessageDoc)));
    return (CMessageDoc*)m_pDocument;
}

#endif //_DEBUG


// CMessageView message handlers
