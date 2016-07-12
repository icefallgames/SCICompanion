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
// VocabView.cpp : implementation file
//

#include "stdafx.h"
#include "AppState.h"
#include "VocabView.h"
#include "VocabDoc.h"
#include "Vocab000.h"
#include "resource.h"
#include "FindWordDialog.h"
#include "format.h"

const static int TIMER_MATCHTEXT = 1234;

// CVocabView

IMPLEMENT_DYNCREATE(CVocabView, CListView)

static const UINT _uFindDialogMessage = ::RegisterWindowMessage(FINDMSGSTRING);

CVocabView::CVocabView() : _addedColumns(false), _iView(LVS_REPORT), _bInLabelEdit(false), _bEditingNewItem(false), _pFindDialog(nullptr) {}

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
    { WordClass::Conjunction,           ID_VOCAB_CONJUNCTION },
    { WordClass::Association,           ID_VOCAB_ASSOCIATION },
    { WordClass::Punctuation,           ID_VOCAB_PUNCTUATION },
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
    ON_COMMAND_RANGE(ID_VOCAB_NUMBER, ID_VOCAB_PUNCTUATION, OnVocabCommand)
    ON_COMMAND(ID_VOCAB_NEWWORD, OnNewWord)
    ON_COMMAND(ID_EDIT_RENAME, OnRename)
    ON_COMMAND(ID_EDIT_FIND, OnFind)
    ON_UPDATE_COMMAND_UI_RANGE(ID_VOCAB_NUMBER, ID_VOCAB_PUNCTUATION, OnUpdateVocabCommand)
    ON_REGISTERED_MESSAGE(_uFindDialogMessage, OnFindDialogMessage)
END_MESSAGE_MAP()

#define COL_WORDS 0
#define COL_GROUP 2
#define COL_CLASS 1


const struct
{
    PTSTR pszName; // Should be const, but the LVCOLUMN struct accepts only non-const
    int cx;
}
c_VocabColumnInfo [] =
{
    { TEXT("Words"), 350 },
    { TEXT("Class"), 200 },
    { TEXT("Group"), 50 },
};

void CVocabView::_InitColumns()
{
    if (_addedColumns)
    {
        return;
    }
    _addedColumns = true;
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

std::string _GetGroupString(Vocab000::WordGroup dwGroup)
{
    return fmt::format("{0:4x}", (DWORD)dwGroup);
}

void CVocabView::_InsertItem(int iItem, Vocab000::WordGroup dwGroup, WordClass dwClass, const std::string &strWords)
{
    CListCtrl& listCtl = GetListCtrl();
    LVITEM item = { 0 };
    item.mask = LVIF_TEXT | LVIF_PARAM;
    item.iItem = iItem;
    item.iSubItem = 0;
    item.pszText = const_cast<PSTR>(strWords.c_str());
    item.lParam = dwGroup;
    listCtl.InsertItem(&item);

    _SetItem(iItem, dwGroup, dwClass);
}

void CVocabView::_SetItem(int iItem, Vocab000::WordGroup dwGroup, WordClass dwClass)
{
    // Now the columns
    for (int iSub = 1; iSub < ARRAYSIZE(c_VocabColumnInfo); iSub++)
    {
        std::string theString;
        LVITEM item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = iItem;
        item.iSubItem = iSub;
        switch (iSub)
        {
            case COL_CLASS:
                theString = GetWordClassString(dwClass);
                break;
            case COL_GROUP:
                theString = _GetGroupString(dwGroup);
                break;
        }
        item.pszText = const_cast<PSTR>(theString.c_str());
        GetListCtrl().SetItem(&item);
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

Vocab000::WordGroup CVocabView::_GetItemGroup(int item)
{
    return static_cast<Vocab000::WordGroup>(GetListCtrl().GetItemData(item));
}

void CVocabView::OnVocabCommand(UINT nID)
{
    // Get the selected item's state and toggle it.
    int nItem = _GetSelectedItem();
    if (nItem != -1)
    {
        Vocab000::WordGroup group = _GetItemGroup(nItem);

        WordClass dwClass;
        GetVocab()->GetGroupClass(group, &dwClass);
        WordClass dwClassFlag;
        if (_CommandIDToVocabClass(nID, dwClassFlag))
        {
            if (IsFlagSet(dwClass, dwClassFlag))
            {
                // The bit is on - remove it.
                ClearFlag(dwClass, dwClassFlag);
            }
            else
            {
                // The bit is off - add it.
                dwClass = (dwClass | dwClassFlag);
            }

            // Change the class of the group!
            CVocabDoc *pDoc = GetDocument();
            if (pDoc)
            {
                pDoc->ApplyChanges<Vocab000>(
                    [&](Vocab000 &vocab)
                {
                    VocabChangeHint hint = vocab.SetGroupClass(group, dwClass);
                    return WrapHint(hint);
                }
                );
            }
        }
    }
}

void CVocabView::OnUpdateVocabCommand(CCmdUI *pCmdID)
{
    // Get the selected item's state and toggle it.
    int nItem = _GetSelectedItem();
    if (nItem != -1)
    {
        const Vocab000 *pVocab = GetVocab();
        if (pVocab)
        {
            Vocab000::WordGroup group = _GetItemGroup(nItem);
            WordClass wordClass = WordClass::Unknown;
            pVocab->GetGroupClass(group, &wordClass);
            WordClass dwClassFlag;
            if (_CommandIDToVocabClass(pCmdID->m_nID, dwClassFlag))
            {
                pCmdID->SetCheck(IsFlagSet(wordClass, dwClassFlag) ? 1 : 0);
            }
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
    CArray<CString, PCTSTR> newWords;    

    // Always delete, because we'll go through the standard update mechanism.
    listCtl.DeleteItem(plvdi->item.iItem);

    if (SUCCEEDED(_ParseViewString(pszNewWords, newWords)))
    {
        CVocabDoc *pDoc = GetDocument();
        if (pDoc)
        {
            pDoc->ApplyChanges<Vocab000>(
                [&](Vocab000 &vocab)
                {
                    // Add any new words.
                    CString strFirstNewWord;
                    VocabChangeHint hint = VocabChangeHint::None;
                    bool error = false;
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
                    return WrapHint(hint);
                }
                );
        }
    }
}

void CVocabView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
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
                CVocabDoc *pDoc = GetDocument();
                if (pDoc)
                {
                    pDoc->ApplyChanges<Vocab000>(
                        [&](Vocab000 &vocab)
                    {
                        VocabChangeHint hint = VocabChangeHint::None;

                        // Now compare the two, and remove words or add words, based on that.
                        Vocab000::WordGroup group = _GetItemGroup(plvdi->item.iItem);
                        bool bail = false;

                        // Add any new words.
                        for (INT_PTR i = 0; i <= postEditWords.GetUpperBound(); i++)
                        {
                            CString strNewWord = postEditWords.GetAt(i);
                            if (!_FindString(_preEditWords, strNewWord))
                            {
                                if (IsValidVocabString(strNewWord, true))
                                {
                                    // This is a new word.
                                    hint |= vocab.AddWordToGroup(strNewWord, group, true);
                                }
                                else
                                {
                                    // If this error occured, then just don't do any changes.
                                    bail = true;
                                }
                            }
                        }

                        if (!bail)
                        {
                            // Do the reverse, remove any old words:
                            for (INT_PTR i = 0; i <= _preEditWords.GetUpperBound(); i++)
                            {
                                if (!_FindString(postEditWords, _preEditWords.GetAt(i)))
                                {
                                    // This word needs to be removed.
                                    hint |= vocab.RemoveWord(_preEditWords.GetAt(i));
                                }
                            }
                        }

                        return WrapHint(hint);
                    }
                    );
                }
            }
        }
    }
    else if ((plvdi->item.pszText == NULL) && (_bEditingNewItem))
    {
        // The user cancelled out of a new item.  Let's delete this new item.
        GetListCtrl().DeleteItem(plvdi->item.iItem);
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
                SelectGroup(dwGroup);
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
            // Select this group.
            _strMatchText.AppendChar(nChar);
            SelectGroup(pVocab->GroupFromString(_strMatchText));

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

int CVocabView::_FindItemByGroup(Vocab000::WordGroup group)
{
    LVFINDINFO findInfo = { 0 };
    findInfo.flags = LVFI_PARAM;
    findInfo.lParam = (LPARAM)group;
    return GetListCtrl().FindItem(&findInfo);
}

//
// Select a group and scroll in to view.
//
void CVocabView::SelectGroup(Vocab000::WordGroup group)
{
    int iItem = _FindItemByGroup(group);
    if (iItem != -1)
    {
        CListCtrl& listCtl = GetListCtrl();
        // Select and scroll into view.
        listCtl.SetItemState(iItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        listCtl.EnsureVisible(iItem, FALSE);
    }
}

#ifdef SORT_BY_GROUP
//
// Sorting based on group, which contains the resource number.
//
int CALLBACK CVocabView::s_SortListByGroup(LPARAM lpOne, LPARAM lpTwo, LPARAM lpArg)
{
    return ((int)lpOne - (int)lpTwo);
}
#endif

void CVocabView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)
{
    VocabChangeHint hint = GetHint<VocabChangeHint>(pHint);
    Vocab000::WordGroup group = (Vocab000::WordGroup)(((DWORD)hint & 0xffff0000) >> 16);

    CVocabDoc *pDoc = GetDocument();
    if (pDoc)
    {
        const Vocab000 *pVocab = pDoc->GetVocab();
        if (pVocab)
        {
            int previouslySelectedItem = _GetSelectedItem();
            Vocab000::WordGroup previouslySelectedGroup = 0;
            if (previouslySelectedItem != -1)
            {
                previouslySelectedGroup = _GetItemGroup(previouslySelectedItem);
            }

            if (IsFlagSet(hint, VocabChangeHint::Changed))
            {
                // Prepare the listview.
                GetListCtrl().SetRedraw(FALSE);
                GetListCtrl().DeleteAllItems();
                _InitColumns();
                _ChangeView();

                int iItem = 0;

                // Copy to a vector so we can sort. Sorting in listview is slow.
                std::vector<Vocab000::WordGroup> allGroups;
                allGroups.reserve(pVocab->GetNumberOfGroups());
                std::transform(pVocab->GroupsBegin(), pVocab->GroupsEnd(), std::back_inserter(allGroups),
                    [](const Vocab000::groups_iterator::value_type &pair) { return pair.first; }
                    );

                std::sort(allGroups.begin(), allGroups.end());
                for (Vocab000::WordGroup group : allGroups)
                {
                    WordClass wordClass;
                    pVocab->GetGroupClass(group, &wordClass);
                    _InsertItem(iItem, group, wordClass, pVocab->GetGroupWordString(group));
                    iItem++;
                }
 
                GetListCtrl().SetRedraw(TRUE);

                if (previouslySelectedItem != -1)
                {
                    SelectGroup(previouslySelectedGroup);
                }
            }
            else if (IsFlagSet(hint, VocabChangeHint::AddWordGroup))
            {
                // Insert at end, since that's where we were editing it.
                int count = GetListCtrl().GetItemCount();
                WordClass wordClass;
                if (pVocab->GetGroupClass(group, &wordClass))
                {
                    _InsertItem(count, group, wordClass, pVocab->GetGroupWordString(group));
                }
                SelectGroup(group);
            }
            else if (IsFlagSet(hint, VocabChangeHint::DeleteWordGroup))
            {
                int index = _FindItemByGroup(group);
                if (index != -1)
                {
                    GetListCtrl().DeleteItem(index);
                    if ((previouslySelectedItem != -1) && (previouslySelectedItem < GetListCtrl().GetItemCount()))
                    {
                        // Select the same index again (e.g. the next item)
                        GetListCtrl().SetItemState(previouslySelectedItem, LVIS_SELECTED, LVIS_SELECTED);
                    }
                }
            }
            else if (IsFlagSet(hint, VocabChangeHint::EditWordGroup))
            {
                int index = _FindItemByGroup(group);
                if (index != -1)
                {
                    // Update it
                    WordClass wordClass;
                    if (pVocab->GetGroupClass(group, &wordClass))
                    {
                        _SetItem(index, group, wordClass);
                        GetListCtrl().SetItemText(index, COL_WORDS, pVocab->GetGroupWordString(group).c_str());
                    }
                }
            }
        }
    }
}

const Vocab000* CVocabView::GetVocab() const
{
    const Vocab000 *pVocab = NULL;
    CVocabDoc *pDoc = GetDocument();
    if (pDoc)
    {
        pVocab = pDoc->GetVocab();
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
