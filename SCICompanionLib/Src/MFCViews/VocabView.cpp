// VocabView.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "VocabView.h"
#include "VocabDoc.h"
#include "Vocab000.h"
#include "resource.h"
#include "FindWordDialog.h"

const static int TIMER_MATCHTEXT = 1234;

// CVocabView

IMPLEMENT_DYNCREATE(CVocabView, CListView)

static const UINT _uFindDialogMessage = ::RegisterWindowMessage(FINDMSGSTRING);

CVocabView::CVocabView()
{
    _iView = LVS_REPORT;
    _bInLabelEdit = false;
    _bEditingNewItem = false;
    _pFindDialog = NULL;
}

CVocabView::~CVocabView()
{
}

const key_value_pair<WordClass, UINT> _cClassToCommand[] =
{
    { WordClass::Number,                ID_VOCAB_NUMBER },
    { WordClass::Proposition,           ID_VOCAB_PREPOSITION },
    { WordClass::Article,               ID_VOCAB_ARTICLE },
    { WordClass::QualifyingAdjective,   ID_VOCAB_QUALIFYINGADJECTIVE },
    { WordClass::RelativePronoun,       ID_VOCAB_RELATIVEPRONOUN },
    { WordClass::Noun,                  ID_VOCAB_NOUN },
    { WordClass::IndicativeVerb,        ID_VOCAB_INDICATIVEVERB },
    { WordClass::Adverb,                ID_VOCAB_ADVERB },
    { WordClass::ImperativeVerb,        ID_VOCAB_IMPERATIVEVERB },
};

BOOL _CommandIDToVocabClass(UINT nID, WordClass &dwClass)
{
    return FindKey(_cClassToCommand, ARRAYSIZE(_cClassToCommand), nID, dwClass);
}

BEGIN_MESSAGE_MAP(CVocabView, CListView)
    ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
    ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
    ON_WM_KEYDOWN()
    ON_WM_CHAR()
    ON_WM_TIMER()
    ON_NOTIFY_REFLECT(NM_DBLCLK, OnItemDoubleClick)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_WORD, OnUpdateWordIndicator)
    ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateIfSelection)
    ON_UPDATE_COMMAND_UI(ID_VOCAB_NEWWORD, OnUpdateAlwaysOn)
    ON_UPDATE_COMMAND_UI(ID_EDIT_RENAME, OnUpdateIfSelection)
    ON_WM_CONTEXTMENU()
    ON_COMMAND(ID_EDIT_DELETE, OnDelete)
    ON_COMMAND_RANGE(ID_VOCAB_NUMBER, ID_VOCAB_IMPERATIVEVERB, OnVocabCommand)
    ON_COMMAND(ID_VOCAB_NEWWORD, OnNewWord)
    ON_COMMAND(ID_EDIT_RENAME, OnRename)
    ON_COMMAND(ID_EDIT_FIND, OnFind)
    ON_UPDATE_COMMAND_UI_RANGE(ID_VOCAB_NUMBER, ID_VOCAB_IMPERATIVEVERB, OnUpdateVocabCommand)
    ON_REGISTERED_MESSAGE(_uFindDialogMessage, OnFindDialogMessage)
END_MESSAGE_MAP()

#define COL_WORDS 0
#define COL_GROUP 1
#define COL_CLASS 2


const struct
{
    PTSTR pszName; // Should be const, but the LVCOLUMN struct accepts only non-const
    int cx;
}
c_VocabColumnInfo [] =
{
    { TEXT("Words"), 350 },
    { TEXT("Group"), 50 },
    { TEXT("Class"), 300 },
};

void CVocabView::_InitColumns()
{
    CListCtrl& listCtl = GetListCtrl();
    for (int i = 0; i < ARRAYSIZE(c_VocabColumnInfo); i++)
    {
        // Name
        LVCOLUMN col = { 0 };
        col.mask = LVCF_FMT | LVCF_ORDER | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH; 
        col.iOrder = i;
        col.iSubItem = i;
        col.pszText = c_VocabColumnInfo[i].pszName;
        col.cx = c_VocabColumnInfo[i].cx;
        col.fmt = LVCFMT_RIGHT;
        listCtl.InsertColumn(i, &col);
    }
}

void CVocabView::_ChangeView()
{
    CListCtrl& listCtl = GetListCtrl();
    LONG_PTR dwStyle = GetWindowLongPtr(listCtl.m_hWnd, GWL_STYLE); 
    dwStyle &= ~LVS_TYPEMASK;
    dwStyle |= _iView;
    SetWindowLongPtr(listCtl.m_hWnd, GWL_STYLE, (LONG)dwStyle);
}

void _GetClassString(PTSTR pszBuf, size_t cch, WordClass dwClass)
{
    std::string str;
    GetWordClassString(dwClass, str);
    StringCchCopy(pszBuf, cch, str.c_str());
}

void _GetGroupString(PTSTR pszBuf, size_t cch, Vocab000::WordGroup dwGroup)
{
    StringCchPrintf(pszBuf, cch, TEXT("%4x"), dwGroup);
}


void CVocabView::_UpdateItemInfo(int nItem, DWORD dwInfo)
{
    CListCtrl& listCtl = GetListCtrl();
    listCtl.SetItemData(nItem, dwInfo);
    TCHAR szBuf[100];
    _GetClassString(szBuf, ARRAYSIZE(szBuf), GetWordClass(dwInfo));
    listCtl.SetItemText(nItem, COL_CLASS, szBuf);
    _GetGroupString(szBuf, ARRAYSIZE(szBuf), GetWordGroup(dwInfo));
    listCtl.SetItemText(nItem, COL_GROUP, szBuf);
}

void CVocabView::_InsertItem(int iItem, Vocab000::WordGroup dwGroup, WordClass dwClass, const std::string &strWords)
{
    CListCtrl& listCtl = GetListCtrl();

    TCHAR szBuf[100];
    StringCchPrintf(szBuf, ARRAYSIZE(szBuf), TEXT("%s"), strWords.c_str());
    LVITEM item = { 0 };
    item.mask = LVIF_TEXT | LVIF_PARAM;
    item.iItem = iItem;
    item.iSubItem = 0;
    item.pszText = szBuf;
    item.lParam = InfoFromClassAndGroup(dwClass, dwGroup);
    listCtl.InsertItem(&item);

    // Now the columns
    for (int iSub = 1; iSub < ARRAYSIZE(c_VocabColumnInfo); iSub++)
    {
        LVITEM item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = iItem;
        item.iSubItem = iSub;
        item.pszText = szBuf;
        switch (iSub)
        {
        case COL_CLASS:
            _GetClassString(szBuf, ARRAYSIZE(szBuf), dwClass);
            break;
        case COL_GROUP:
            _GetGroupString(szBuf, ARRAYSIZE(szBuf), dwGroup);
            break;
        }
        listCtl.SetItem(&item);
    }
}

BOOL CVocabView::PreCreateWindow(CREATESTRUCT& cs)
{
    // We wish to enable label edits.
    cs.style |= LVS_EDITLABELS | LVS_SINGLESEL | LVS_SHOWSELALWAYS;
	return __super::PreCreateWindow(cs);
}

void CVocabView::OnInitialUpdate()
{
    __super::OnInitialUpdate();
    DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
    GetListCtrl().SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LPARAM(dwExStyle));
}

BOOL CVocabView::PreTranslateMessage(MSG *pMsg)
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
    return __super::PreTranslateMessage(pMsg);
}


void CVocabView::OnContextMenu(CWnd *pWnd, CPoint point)
{
    BOOL fShow = FALSE;
    CListCtrl &listCtl = GetListCtrl();
    int iItem = _GetSelectedItem();
    if (iItem != -1)
    {
        CRect rc;
        if (listCtl.GetItemRect(iItem, &rc, LVIR_BOUNDS))
        {
            if ((point.x == -1) && (point.y == -1))
            {
                point = rc.TopLeft();
                point.Offset(5, 5);
                fShow = TRUE;
            }
            else
            {
                // Point is in screen coords
                ScreenToClient(&point);
                if (PtInRect(&rc, point))
                {
                    fShow = TRUE;
                }
            }
        }
    }

    if (fShow)
    {
        CMenu contextMenu; 
        contextMenu.LoadMenu(IDR_MENUVOCAB); 
        CMenu* pTracker; 
        pTracker = contextMenu.GetSubMenu(0); 

        if (pTracker)
        {
            ClientToScreen(&point);
            pTracker->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x , point.y , AfxGetMainWnd()); 
        }
    }

    //has always to be called (function of base class)
    __super::OnContextMenu(pWnd, point);
}

void CVocabView::OnVocabCommand(UINT nID)
{
    // Get the selected item's state and toggle it.
    int nItem = _GetSelectedItem();
    if (nItem)
    {
        CListCtrl &listCtl = GetListCtrl();
        DWORD dwInfo = static_cast<DWORD>(listCtl.GetItemData(nItem));
        WordClass dwClass = GetWordClass(dwInfo);
        WordClass dwClassFlag;
        if (_CommandIDToVocabClass(nID, dwClassFlag))
        {
            if (IsFlagSet(dwClass, dwClassFlag))
            {
                // The bit is on - remove it.
                //dwClass &= ~static_cast<int>(dwClassFlag);
                ClearFlag(dwClass, dwClassFlag);
            }
            else
            {
                // The bit is off - add it.
                dwClass = (dwClass | dwClassFlag);
            }

            // Change the class of the group!
            Vocab000 *pVocab = GetVocab();
            if (pVocab)
            {
                if (SUCCEEDED(pVocab->SetGroupClass(GetWordGroup(dwInfo), dwClass)))
                {
                    CDocument *pDoc = GetDocument();
                    if (pDoc)
                    {
                        pDoc->SetModifiedFlag();
                    }
                    _UpdateItemInfo(nItem, InfoFromClassAndGroup(dwClass, GetWordGroup(dwInfo)));
                }
            }
        }
    }
}

void CVocabView::OnUpdateVocabCommand(CCmdUI *pCmdID)
{
    // Get the selected item's state and toggle it.
    int nItem = _GetSelectedItem();
    if (nItem)
    {
        CListCtrl &listCtl = GetListCtrl();
        DWORD dwInfo = static_cast<DWORD>(listCtl.GetItemData(nItem));
        WordClass dwClass = GetWordClass(dwInfo);
        WordClass dwClassFlag;
        if (_CommandIDToVocabClass(pCmdID->m_nID, dwClassFlag))
        {
            pCmdID->SetCheck(IsFlagSet(dwClass, dwClassFlag) ? 1 : 0);
        }
    }
}


BOOL _FindString(CArray<CString, PCTSTR> &words, PCTSTR pszWord)
{
    BOOL fRet = FALSE;
    for (INT_PTR i = 0; !fRet && (i <= words.GetUpperBound()); i++)
    {
        if (0 == lstrcmp(pszWord, words.GetAt(i)))
        {
            fRet = TRUE;
        }
    }
    return fRet;
}

void CVocabView::_OnEndEditingNewItem(PCTSTR pszNewWords, NMLVDISPINFO *plvdi)
{
    CListCtrl& listCtl = GetListCtrl();
    bool error = true;
    CArray<CString, PCTSTR> newWords;    
    if (SUCCEEDED(_ParseViewString(pszNewWords, newWords)))
    {
        CVocabDoc *pDoc = GetDocument();
        if (pDoc)
        {
            error = false;
            pDoc->ApplyChanges<Vocab000>(
                [&](Vocab000 &vocab)
                {
                    // Add any new words.
                    CString strFirstNewWord;
                    VocabChangeHint hint;
                    for (INT_PTR i = 0; !error && (i <= newWords.GetUpperBound()); i++)
                    {
                        CString strNewWord = newWords.GetAt(i);
                        error = !IsValidVocabString(strNewWord, TRUE);
                        if (!error)
                        {
                            if (strFirstNewWord.IsEmpty())
                            {
                                // Use class UnknownWordClass for now - the user can change it later.
                                VocabChangeHint hintTemp = vocab.AddNewWord(strNewWord, WordClass::Unknown, TRUE);
                                if (hintTemp != VocabChangeHint::None)
                                {
                                    strFirstNewWord = strNewWord;
                                }
                                hint |= hintTemp;
                            }
                            else
                            {
                                // As this one as a synonym of the last.
                                hint |= vocab.AddSynonym(strNewWord, strFirstNewWord);
                            }
                        }
                    }

                    if (!error && !strFirstNewWord.IsEmpty())
                    {
                        GetDocument()->SetModifiedFlag();

                        // Figure out which group it belongs to.
                        Vocab000::WordGroup dwGroup = vocab.GroupFromString(strFirstNewWord);
                        std::string strWordsInView = vocab.GetGroupWordString(dwGroup);

                        // Set the 1) official word string, and the 2) group
                        listCtl.SetItemText(plvdi->item.iItem, COL_WORDS, strWordsInView.c_str());
                        _UpdateItemInfo(plvdi->item.iItem, InfoFromClassAndGroup(WordClass::Unknown, dwGroup));
                    }

                    return WrapHint(hint);
                }
                );
        }
    }

    if (error)
    {
        listCtl.DeleteItem(plvdi->item.iItem);
    }
}

void CVocabView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    CListCtrl& listCtl = GetListCtrl();
    *pResult = FALSE;
    NMLVDISPINFO *plvdi = (NMLVDISPINFO *)pNMHDR;
    if (plvdi->item.pszText && _bInLabelEdit)
    {
        CString strLower = plvdi->item.pszText;
        strLower.MakeLower();

        if (_bEditingNewItem)
        {
            _OnEndEditingNewItem(strLower, plvdi);
            _bEditingNewItem = false;
        }
        else
        {
            CArray<CString, PCTSTR> postEditWords;
            if (SUCCEEDED(_ParseViewString(strLower, postEditWords)))
            {
                Vocab000 *pVocab = GetVocab();
                if (pVocab)
                {
                    // Now compare the two, and remove words or add words, based on that.
                    DWORD dwInfo = static_cast<DWORD>(listCtl.GetItemData(plvdi->item.iItem));
                    BOOL fBail = FALSE;

                    // Add any new words.
                    for (INT_PTR i = 0; i <= postEditWords.GetUpperBound(); i++)
                    {
                        CString strNewWord = postEditWords.GetAt(i);
                        if (!_FindString(_preEditWords, strNewWord))
                        {
                            if (IsValidVocabString(strNewWord, TRUE))
                            {
                                // This is a new word.
                                if (SUCCEEDED(pVocab->AddWord(strNewWord, dwInfo, TRUE)))
                                {
                                    // Note: If this failed, it will bring up UI indicating the reason.
                                    GetDocument()->SetModifiedFlag();
                                }
                            }
                            else
                            {
                                // If this error occured, then just don't do any changes.
                                fBail = TRUE;
                            }
                        }
                    }

                    if (!fBail)
                    {
                        // Do the reverse, remove any old words:
                        for (INT_PTR i = 0; i <= _preEditWords.GetUpperBound(); i++)
                        {
                            if (!_FindString(postEditWords, _preEditWords.GetAt(i)))
                            {
                                // This word needs to be removed.
                                pVocab->RemoveWord(_preEditWords.GetAt(i));
                                GetDocument()->SetModifiedFlag();
                            }
                        }

                        // Ok we're done.  We should be able to get a new string.
                        std::string strWords = pVocab->GetGroupWordString(GetWordGroup(dwInfo));
                        
                        if (strWords.empty())
                        {
                            // If the string is empty, then no words are left for this group.
                            // Remove it.
                            listCtl.DeleteItem(plvdi->item.iItem);
                        }
                        else
                        {
                            // Update the name in the view:
                            listCtl.SetItemText(plvdi->item.iItem, 0, strWords.c_str());
                        }
                    }
                }
            }
        }
    }
    else if ((plvdi->item.pszText == NULL) && (_bEditingNewItem))
    {
        // The user cancelled out of a new item.  Let's delete this new item.
        listCtl.DeleteItem(plvdi->item.iItem);
        _bEditingNewItem = false;
    }
    _bInLabelEdit = false;
}

void CVocabView::OnItemDoubleClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    // Enter label edit.
    NM_LISTVIEW *pNMLV = (NMLISTVIEW*)pNMHDR;
    int iViewIndex = pNMLV->iItem;
    CListCtrl& listCtl = GetListCtrl();
    listCtl.EditLabel(iViewIndex);
}

PTSTR _SkipWhitespaceOrBar(PTSTR psz)
{
    while (*psz && ((*psz == TEXT('\t')) || (*psz == TEXT('\n')) || (*psz == TEXT(' '))  || (*psz == TEXT('|'))))
    {
        psz++;
    }
    return psz;
}

PTSTR _FindWhitespaceOrBar(PTSTR psz)
{
    while (*psz && (*psz != TEXT('\t')) && (*psz != TEXT('\n')) && (*psz != TEXT(' ')) && (*psz != TEXT('|')))
    {
        psz++;
    }
    return psz;
}

HRESULT CVocabView::_ParseViewString(PCTSTR pszString, CArray<CString, PCTSTR> &words)
{
    HRESULT hr = E_OUTOFMEMORY;
    size_t cch = lstrlen(pszString) + 2; // Make it double-null terminated.
    PTSTR pszCopy = new TCHAR[cch];
    if (pszCopy)
    {
        PTSTR pszDelete = pszCopy;
        pszCopy[cch - 1] = 0;
        hr = S_OK;
        StringCchCopy(pszCopy, cch, pszString);
        ASSERT(pszCopy[cch - 2] == 0);

        PTSTR pszEnd;
        do
        {
            pszCopy = _SkipWhitespaceOrBar(pszCopy);
            pszEnd = _FindWhitespaceOrBar(pszCopy);
            *pszEnd = 0;
            if (lstrlen(pszCopy) > 0)
            {
                words.Add(pszCopy);
            }
            pszCopy = pszEnd + 1;
        } while (*pszCopy);

        delete pszDelete;
    }
    return hr;
}

void CVocabView::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
    _preEditWords.RemoveAll();
    NMLVDISPINFO *plvdi = (NMLVDISPINFO *)pNMHDR;
    CListCtrl &listCtl = GetListCtrl();
    CString strText = listCtl.GetItemText(plvdi->item.iItem, COL_WORDS);
    if (SUCCEEDED(_ParseViewString(strText, _preEditWords)))
    {
        _bInLabelEdit = true;
    }
}

void CVocabView::OnNewWord()
{
    const  Vocab000 *pVocab = GetVocab();
    if (pVocab)
    {
        CListCtrl &listCtl = GetListCtrl();
        std::string strNewWord = "new word";
        int nNewItem = listCtl.GetItemCount();

        // Insert it with a temporary INVALID group and class
        _InsertItem(nNewItem, 0, WordClass::Unknown, strNewWord);

        // Put it in editmode.
        listCtl.EditLabel(nNewItem);
        _bEditingNewItem = true;

        // Note: if label edit is cancelled, we need to do know this and delete it.
        // We detect this in OnEndLabelEdit, when pszText is NULL.
    }
}

void CVocabView::OnRename()
{
    // Enter label edit.
    int nItem = _GetSelectedItem();
    if (nItem != -1)
    {
        CListCtrl& listCtl = GetListCtrl();
        listCtl.EditLabel(nItem);
    }
}

void CVocabView::OnFind()
{
    if (_pFindDialog == NULL)
    {
        _pFindDialog = new CFindWordDialog();
        if (_pFindDialog)
        {
            if (_pFindDialog->Create(TRUE, NULL, NULL, FR_DOWN, this))
            {
            }
        }
    }
}

LRESULT CVocabView::OnFindDialogMessage(WPARAM wParam, LPARAM lParam)
{
    ASSERT(_pFindDialog != NULL);

    // If the FR_DIALOGTERM flag is set,
    // invalidate the handle identifying the dialog box.
    if (_pFindDialog->IsTerminating())
    {
        _pFindDialog = NULL;
        return 0;
    }

    // If the FR_FINDNEXT flag is set,
    // call the application-defined search routine
    // to search for the requested string.
    if(_pFindDialog->FindNext())
    {
        CString strWord = _pFindDialog->GetFindString();
        const Vocab000 *pVocab = GetVocab();
        if (pVocab)
        {
            Vocab000::WordGroup dwGroup;
            if (pVocab->LookupWord((PCSTR)strWord, dwGroup))
            {
                WordClass dwClass;
                pVocab->GetGroupClass(dwGroup, &dwClass);
                SelectGroup(InfoFromClassAndGroup(dwClass, dwGroup));
                _pFindDialog->SetStatus(TEXT(""));
            }
            else
            {
                TCHAR szMsg[MAX_PATH];
                StringCchPrintf(szMsg, ARRAYSIZE(szMsg), TEXT("\"%s\" not found!"), (PCSTR)strWord);
                _pFindDialog->SetStatus(szMsg);
            }
        }
    }

    return 0;
}

void CVocabView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == VK_F2)
    {
        OnRename();
    }
    else if (nChar == VK_F4) // temporary
    {
        OnNewWord();
    }
    else
    {
        __super::OnKeyDown(nChar, nRepCnt, nFlags);
    }
}


void CVocabView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (IsCharAlphaNumericA(nChar))
    {
        // Try to find the best match.
        const Vocab000 *pVocab = GetVocab();
        if (pVocab)
        {
            _strMatchText.AppendChar(nChar);
            Vocab000::WordGroup dwGroup = pVocab->GroupFromString(_strMatchText);
            // Select this group.
            WordClass dwClass;
            pVocab->GetGroupClass(dwGroup, &dwClass);
            SelectGroup(InfoFromClassAndGroup(dwClass, dwGroup));

            SetTimer(TIMER_MATCHTEXT, 1000, NULL);
        }
        // Son't let these chars get to listview, since it will compete with us.
    }
    else
    {
        __super::OnChar(nChar, nRepCnt, nFlags);
    }
}

void CVocabView::OnTimer(UINT_PTR nIDEvent)
{
    // Timer ran out.
    if (nIDEvent == TIMER_MATCHTEXT)
    {
        _strMatchText = TEXT("");
    }
    else
    {
        // Must call __super::OnTimer, or else the label-editing won't work.
        __super::OnTimer(nIDEvent);
    }
}

void CVocabView::OnUpdateWordIndicator(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(); 
    pCmdUI->SetText(_strMatchText);
}

void CVocabView::OnUpdateIfSelection(CCmdUI *pCmdUI)
{
    pCmdUI->Enable((!_bInLabelEdit && (_GetSelectedItem() != -1)) ? 1 : 0);
}

void CVocabView::OnDelete()
{
    int nItem = _GetSelectedItem();
    if (!_bInLabelEdit && (nItem != -1))
    {
        CListCtrl &listCtl = GetListCtrl();
        CString strText = listCtl.GetItemText(nItem, COL_WORDS);
        CArray<CString, PCTSTR> words;
        // REVIEW: Doesn't handle words with spaces properly (which you can enter via SCI Studio)
        // which means if you delete 'foo bar', then it will delete 'foo' and 'bar'
        if (SUCCEEDED(_ParseViewString(strText, words)))
        {
            CVocabDoc *pDoc = GetDocument();
            if (pDoc)
            {
                pDoc->ApplyChanges<Vocab000>(
                    [&](Vocab000 &vocab)
                    {
                        VocabChangeHint hint = VocabChangeHint::None;
                        // Delete all these words.
                        for (INT_PTR i = 0; i <= words.GetUpperBound(); i++)
                        {
                            hint |= vocab.RemoveWord(words.GetAt(i));
                            assert(IsFlagSet(hint, VocabChangeHint::Changed)); // Else we're not in sync
                        }

                        // Delete the item from view. Do this in here before we return anything, as that will cause an update.
                        listCtl.DeleteItem(nItem);

                        // Select the next item in the view.
                        if (!listCtl.SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED) && (nItem > 0))
                        {
                            listCtl.SetItemState(nItem - 1, LVIS_SELECTED, LVIS_SELECTED);
                        }

                        return WrapHint(hint);
                    }
                    );
            }
        }
    }
}

//
// Returns -1 if there is no item selected.
//
int CVocabView::_GetSelectedItem()
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

//
// Select a group and scroll in to view.
//
void CVocabView::SelectGroup(DWORD dwInfo)
{
    CListCtrl& listCtl = GetListCtrl();
    LVFINDINFO findInfo = { 0 };
    findInfo.flags = LVFI_PARAM;
    findInfo.lParam = (LPARAM)dwInfo;
    int iItem = listCtl.FindItem(&findInfo);
    if (iItem != -1)
    {
        // Select and scroll into view.
        listCtl.SetItemState(iItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        listCtl.EnsureVisible(iItem, FALSE);
    }
}

//
// Sorting based on group, which contains the resource number.
//
int CALLBACK CVocabView::s_SortListByGroup(LPARAM lpOne, LPARAM lpTwo, LPARAM lpArg)
{
    return ((int)lpOne - (int)lpTwo);
}


void CVocabView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
    VocabChangeHint hint = GetHint<VocabChangeHint>(pHint);
    if (IsFlagSet(hint, VocabChangeHint::Changed))
    {
        // Prepare the listview.
        _InitColumns();
        _ChangeView();

        CVocabDoc *pDoc = GetDocument();
        if (pDoc)
        {
            const Vocab000 *pVocab = pDoc->GetVocab();
            if (pVocab)
            {
                int iItem = 0;
                Vocab000::groups_iterator position = pVocab->GroupsBegin();
                while (position != pVocab->GroupsEnd())
                {
                    Vocab000::WordGroup dwGroup;
                    WordClass dwClass;
                    std::string strWords;
                    pVocab->EnumGroups(position, dwGroup, dwClass, strWords);
                    _InsertItem(iItem, dwGroup, dwClass, strWords);
                    iItem++;
                }

                // Sort
                GetListCtrl().SortItems(CVocabView::s_SortListByGroup, NULL);
            }
        }
    }
}


Vocab000* CVocabView::GetVocab() const
{
    Vocab000 *pVocab = NULL;
    CVocabDoc *pDoc = GetDocument();
    if (pDoc)
    {
        // REVIEW: We need to clean this up and go through the proper channels for GetVocab()
        pVocab = const_cast<Vocab000*>(pDoc->GetVocab());
    }
    return pVocab;
}



// CVocabView diagnostics

#ifdef _DEBUG
void CVocabView::AssertValid() const
{
	CListView::AssertValid();
}

void CVocabView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CVocabDoc* CVocabView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVocabDoc)));
    return (CVocabDoc*)m_pDocument;
}
#endif //_DEBUG


// CVocabView message handlers
