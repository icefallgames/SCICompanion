#include "stdafx.h"
#include "AppState.h"
#include "MessageView.h"
#include "MessageDoc.h"
#include "MessageSource.h"
#include "Text.h"
#include "Message.h"
#include "format.h"
#include "NounsAndCases.h"

using namespace std;

IMPLEMENT_DYNCREATE(CMessageView, CListView)

CMessageView::CMessageView() : _iSortColumn(-1), _iView(LVS_REPORT), _fInited(false)
{
}

CMessageView::~CMessageView()
{
}

BEGIN_MESSAGE_MAP(CMessageView, CListView)
    ON_COMMAND(ID_EDIT_DELETE, OnDelete)
    ON_COMMAND(IDC_BUTTONADDNEW, OnAddNew)
    ON_COMMAND(IDC_BUTTONADDSEQ, OnAddSeq)
    ON_COMMAND(IDC_BUTTONCLONE, OnCloneMessage)
    ON_COMMAND(ID_MESSAGE_EXPORT, OnExportMessage)
    ON_COMMAND(ID_MESSAGE_IMPORT, OnImportMessage)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
    ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClicked)
    ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, OnUpdateIfSelection)
    ON_UPDATE_COMMAND_UI(IDC_BUTTONADDSEQ, OnUpdateIfSelection)
    ON_UPDATE_COMMAND_UI(IDC_BUTTONCLONE, OnUpdateIfSelection)
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
    { TEXT("Noun"), 110, MessagePropertyFlags::Noun, COL_NOUN },
    { TEXT("Verb"), 100, MessagePropertyFlags::Verb, COL_VERB },
    { TEXT("Cond."), 100, MessagePropertyFlags::Condition, COL_CONDITION },
    { TEXT("Seq."), 50, MessagePropertyFlags::Sequence, COL_SEQUENCE },
    { TEXT("Talker"), 110, MessagePropertyFlags::Talker, COL_TALKER },
};

CImageList g_messageImageList;
bool g_createdMessageImageList = false;

void CMessageView::_InitColumns()
{
    if (!g_createdMessageImageList)
    {
        g_messageImageList.Create(IDB_MESSAGEAUDIOICONS, 16, 32, RGB(255, 0, 255));
        g_createdMessageImageList = true;
    }
    GetListCtrl().SetImageList(&g_messageImageList, LVSIL_SMALL);

    const TextComponent &text = GetDocument()->GetResource()->GetComponent<TextComponent>();
    CListCtrl& listCtl = GetListCtrl();

    std::vector<int> orderArray;
    for (int i = 0; i < ARRAYSIZE(c_TextColumnInfo); i++)
    {
        // Name
        LVCOLUMN col = { 0 };
        col.mask = LVCF_FMT | LVCF_ORDER | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        col.iImage = 0;
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

struct SortInfo
{
    SortInfo(const TextComponent::container_type &texts, int sortColumn) : texts(texts), sortColumn(sortColumn)
    { }
    const TextComponent::container_type &texts;
    int sortColumn;
};

template<int Multiplier>
int CALLBACK MessageColumnSort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    SortInfo *sortInfo = reinterpret_cast<SortInfo*>(lParamSort);
    const TextEntry *p1 = &sortInfo->texts[lParam1];
    const TextEntry *p2 = &sortInfo->texts[lParam2];
    int iRet = 0;
    switch (sortInfo->sortColumn)
    {
        case COL_STRING:
            iRet = (p1->Text.compare(p2->Text));
            break;
        case COL_NOUN:
            iRet = p1->Noun - p2->Noun;
            break;
        case COL_VERB:
            iRet = p1->Verb - p2->Verb;
            break;
        case COL_CONDITION:
            iRet = p1->Condition - p2->Condition;
            break;
        case COL_SEQUENCE:
            iRet = p1->Sequence - p2->Sequence;
            break;
        case COL_TALKER:
            iRet = p1->Talker - p2->Talker;
            break;
    }
    return iRet * Multiplier;
}

void CMessageView::_SortItemsHelper(int sortColumn, bool toggle)
{
    if (sortColumn == -1)
    {
        return;
    }

    const TextComponent *pText = GetDocument()->GetResource()->TryGetComponent<TextComponent>();
    if (pText)
    {
        auto it = _sortOrder.find(sortColumn);
        if (it == _sortOrder.end())
        {
            _sortOrder[sortColumn] = true;
        }
        else if (toggle)
        {
            _sortOrder[sortColumn] = !it->second;
        }

        SortInfo sortInfo(pText->Texts, _iSortColumn);
        GetListCtrl().SortItems(_sortOrder[sortColumn] ? MessageColumnSort<1> : MessageColumnSort<-1>, reinterpret_cast<DWORD_PTR>(&sortInfo));
    }
}

void CMessageView::OnColumnClicked(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLISTVIEW *plv = reinterpret_cast<NMLISTVIEW*>(pNMHDR);
    ASSERT(plv->iItem == -1);
    _iSortColumn = plv->iSubItem;
    _SortItemsHelper(_iSortColumn, true);
}


void CMessageView::_ChangeView()
{
    CListCtrl& listCtl = GetListCtrl();
    LONG_PTR dwStyle = GetWindowLongPtr(listCtl.m_hWnd, GWL_STYLE); 
    dwStyle &= ~LVS_TYPEMASK;
    dwStyle |= _iView;
    SetWindowLongPtr(listCtl.m_hWnd, GWL_STYLE, (LONG)dwStyle);
}

void _GetDefineFromValue(char *psz, size_t cchBuf, const MessageSource *source, uint16_t value, bool skipPrefix)
{
    bool found = false;
    if (source)
    {
        for (const auto &define : source->GetDefines())
        {
            if (define.second == value)
            {
                const char *pszName = define.first.c_str();
                if (skipPrefix && (define.first.size() > 2))
                {
                    pszName += 2;
                }
                found = true;
                StringCchCopy(psz, cchBuf, pszName);
            }
        }
        if (!found && value)
        {
            found = true;
            StringCchPrintf(psz, cchBuf, "%d", value);
        }
    }

    if (!found)
    {
        StringCchCopy(psz, cchBuf, "none");
    }
}

void CMessageView::_SetItem(int itemIndex, int visualIndex, PCTSTR pszString, const TextComponent *text, bool insert)
{
    assert(insert || (_GetViewIndex(itemIndex) == visualIndex));
    CListCtrl& listCtl = GetListCtrl();

    CMessageDoc *pDoc = GetDocument();
    bool hasAudio = pDoc && pDoc->FindAudioResource(GetMessageTuple(text->Texts[itemIndex]));
    bool hasLipSync = pDoc && pDoc->FindSyncResource(GetMessageTuple(text->Texts[itemIndex]));

    LVITEM item = { 0 };
    item.mask = LVIF_TEXT | LVIF_PARAM;
    item.mask |= LVIF_IMAGE;
    item.iImage = (hasLipSync ? 2 : (hasAudio ? 1 : 0));    // Lipsync implies also audio
    item.iItem = visualIndex;
    item.lParam = itemIndex;
    item.iSubItem = 0;
    item.pszText = const_cast<PTSTR>(pszString);
    if (insert)
    {
        listCtl.InsertItem(&item);
    }
    else
    {
        listCtl.SetItem(&item);
    }

    // Now the columns
    for (int iSub = 1; iSub < (int)_columns.size(); iSub++)
    {
        TCHAR szBuf[100];
        LVITEM item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = visualIndex;
        item.iSubItem = _columns[iSub].iSubItem;
        item.pszText = szBuf;

        int iSubItem = _columns[iSub].iSubItem;
        if (text)
        {
            const NounsAndCasesComponent &nounsAndCases = GetDocument()->GetResource()->GetComponent<NounsAndCasesComponent>();
            switch (iSubItem)
            {
            case COL_NOUN:
                _GetDefineFromValue(szBuf, ARRAYSIZE(szBuf), &nounsAndCases.GetNouns(), text->Texts[itemIndex].Noun, true);
                break;
            case COL_VERB:
                _GetDefineFromValue(szBuf, ARRAYSIZE(szBuf), appState->GetResourceMap().GetVerbsMessageSource(), text->Texts[itemIndex].Verb, true);
                break;
            case COL_CONDITION:
                _GetDefineFromValue(szBuf, ARRAYSIZE(szBuf), &nounsAndCases.GetCases(), text->Texts[itemIndex].Condition, true);
                break;
            case COL_SEQUENCE:
                StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%d"), text->Texts[itemIndex].Sequence);
                break;
            case COL_TALKER:
                _GetDefineFromValue(szBuf, ARRAYSIZE(szBuf), appState->GetResourceMap().GetTalkersMessageSource(), text->Texts[itemIndex].Talker, false);
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
    cs.style |= LVS_SINGLESEL | LVS_SHOWSELALWAYS;
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
        // But the param is the actual index
        nItem = listCtl.GetItemData(nItem);
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
                pDoc->SetSelectedIndex(pnmlv->lParam);
            }
        }
    }
}

void CMessageView::_AddEntryAtCurrentPosition(const TextEntry &entry)
{
    CMessageDoc *pDoc = GetDocument();
    if (pDoc)
    {
        int index = pDoc->GetSelectedIndex();
        index++;
        pDoc->ApplyChanges<TextComponent>(
            [index, &entry](TextComponent &text)
        {
            text.Texts.insert(text.Texts.begin() + index, entry);
            return WrapHint(MessageChangeHint::Changed);
        }
        );

        // Now select it
        pDoc->SetSelectedIndex(index);
    }
}

void CMessageView::OnAddSeq()
{
    const TextEntry *text = _GetEntry();
    if (text)
    {
        TextEntry newEntry = *text;
        newEntry.Sequence++;
        _AddEntryAtCurrentPosition(newEntry);
    }
}

void CMessageView::OnCloneMessage()
{
    const TextEntry *text = _GetEntry();
    if (text)
    {
        _AddEntryAtCurrentPosition(*text);
    }
}

const char c_szMessageTxtFilter[] = "txt files (*.txt)|*.txt|All Files|*.*|";

void CMessageView::OnExportMessage()
{
    CMessageDoc *pDoc = GetDocument();
    if (pDoc)
    {
        const ResourceEntity *resource = pDoc->GetResource();
        if (resource)
        {
            CFileDialog fileDialog(FALSE, nullptr, fmt::format("{0}.txt", resource->ResourceNumber).c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR, c_szMessageTxtFilter);
            if (IDOK == fileDialog.DoModal())
            {
                CString strFileName = fileDialog.GetPathName();
                ExportMessageToFile(resource->GetComponent<TextComponent>(), (PCSTR)strFileName);
            }
        }
    }
}

void CMessageView::OnImportMessage()
{
    CMessageDoc *pDoc = GetDocument();
    if (pDoc)
    {
        pDoc->ApplyChanges<TextComponent>(
            [](TextComponent &text)
        {
            MessageChangeHint hint = MessageChangeHint::None;
            CFileDialog fileDialog(TRUE, nullptr, nullptr, OFN_HIDEREADONLY | OFN_NOCHANGEDIR, c_szMessageTxtFilter);
            if (IDOK == fileDialog.DoModal())
            {
                CString strFileName = fileDialog.GetPathName();
                ImportMessageFromFile(text, (PCSTR)strFileName);
                hint |= MessageChangeHint::Changed;
            }
            return WrapHint(hint);
        }
            );
    }
}


const TextEntry *CMessageView::_GetEntry()
{
    const TextEntry *entry = nullptr;
    const TextComponent* text = GetTextComponent();
    if (text)
    {
        int index = _GetSelectedItem();
        if ((index != -1) && (index < (int)text->Texts.size()))
        {
            entry = &text->Texts[index];
        }
    }
    return entry;
}

void CMessageView::OnAddNew()
{
    // Default to the NARRATOR talker (usually 99), and sequence 1.
    TextEntry newEntry({ 0, 0, 0, 1, 99, 0, "" });
    _AddEntryAtCurrentPosition(newEntry);
}

void CMessageView::OnDelete()
{
    int nItem = _GetSelectedItem();
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
            int newSelectedIndex = max(0, min(nItem, (int)(GetTextComponent()->Texts.size() - 1)));
            pDoc->SetSelectedIndex(newSelectedIndex, true);
        }
    }
}

int CMessageView::_GetViewIndex(int index)
{
    CListCtrl &list = GetListCtrl();
    int cItems = list.GetItemCount();
    for (int i = 0; i < cItems; i++)
    {
        if (index == list.GetItemData(i))
        {
            return i;
        }
    }
    return -1;
}

void CMessageView::_UpdateSelection(int *topIndex)
{
    CMessageDoc *pDoc = GetDocument();
    if (pDoc)
    {
        int viewIndex = _GetViewIndex(pDoc->GetSelectedIndex());
        this->GetListCtrl().SetItemState(viewIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        //this->GetListCtrl().SetSelectionMark(index);
        if (topIndex)
        {
            this->GetListCtrl().EnsureVisible(*topIndex, FALSE);
            this->GetListCtrl().EnsureVisible(*topIndex + this->GetListCtrl().GetCountPerPage() - 1, TRUE);
            /*
            CRect rc;
            this->GetListCtrl().GetItemRect(0, &rc, LVIR_LABEL);
            CSize size = { 0, rc.Height() };
            int newTopIndex = this->GetListCtrl().GetTopIndex();
            size.cy *= (*topIndex - newTopIndex);
            if (*topIndex != this->GetListCtrl().GetTopIndex())
            {
                this->GetListCtrl().Scroll(size);
            }*/
        }
        else
        {
            this->GetListCtrl().EnsureVisible(viewIndex, FALSE);
        }
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
                // Try to maintain the position
                int topIndex = GetListCtrl().GetTopIndex();

                GetListCtrl().SetRedraw(FALSE);
                GetListCtrl().DeleteAllItems();
                
                int i = 0;
                for (auto &individualString : pText->Texts)
                {
                    CString strText = individualString.Text.c_str();
                    _EscapeString(strText);
                    _SetItem(i, i, (PCSTR)strText, pText, true);
                    ++i;
                }

                _UpdateSelection(&topIndex);

                // Sort
                _SortItemsHelper(_iSortColumn, false);

                GetListCtrl().SetRedraw(TRUE);
            }
        }

        ClearFlag(hint, MessageChangeHint::AllMessageFiles); // Since we already re-popualted
    }
    else if (IsFlagSet(hint, MessageChangeHint::ItemChanged))
    {
        CMessageDoc *pDoc = GetDocument();
        if (pDoc)
        {
            int index = pDoc->GetSelectedIndex();
            const TextComponent *pText = pDoc->GetResource()->TryGetComponent<TextComponent>();
            if (pText)
            {
                if ((index >= 0) && (index < (int)pText->Texts.size()))
                {
                    auto &entry = pText->Texts[index];
                    // Just update this row
                    CString strText = entry.Text.c_str();
                    _EscapeString(strText);
                    _SetItem(index, _GetViewIndex(index), (PCSTR)strText, pText, false);

                    _SortItemsHelper(_iSortColumn, false);
                }
            }
        }
    }

    if (IsFlagSet(hint, MessageChangeHint::AllMessageFiles))
    {
        const TextComponent *pText = GetTextComponent();
        if (pText)
        {
            for (size_t i = 0; i < pText->Texts.size(); i++)
            {
                auto &entry = pText->Texts[i];
                // Just update this row
                CString strText = entry.Text.c_str();
                _EscapeString(strText);
                _SetItem(i, i, (PCSTR)strText, pText, false);
            }
            _SortItemsHelper(_iSortColumn, false);
        }
    }
}

const TextComponent* CMessageView::GetTextComponent() const
{
    const TextComponent *pText = nullptr;
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
