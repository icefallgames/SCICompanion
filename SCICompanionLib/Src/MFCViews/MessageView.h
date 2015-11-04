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
#pragma once

struct TextComponent;
struct TextEntry;
class CMessageDoc;

class CMessageView : public CListView
{
	DECLARE_DYNCREATE(CMessageView)

protected:
	CMessageView();           // protected constructor used by dynamic creation
	virtual ~CMessageView();

public:
    CMessageDoc* GetDocument() const;
    const TextComponent* GetTextComponent() const;

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
    virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    BOOL PreTranslateMessage(MSG *pMsg);

protected:
    void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
    void OnChooseMessageDefine(UINT nID);
    void OnDelete();
    void OnAddNew();
    void OnCloneMessage();
    void OnAddSeq();
    void OnUpdateIfSelection(CCmdUI *pCmdUI);
    void OnNewString();
    void OnExportMessage();
    void OnImportMessage();
    void OnCommandAudioSize(CCmdUI *pCmdUI);
    void OnContextMenu(CWnd *pWnd, CPoint point);

    DECLARE_MESSAGE_MAP()

    void _InitColumns();
    void _SetItem(int itemIndex, int visualIndex, PCTSTR pszString, const TextComponent *text, bool insert);
    void _ChangeView();
    int _GetSelectedItem();
    void _UpdateSelection(int *topIndex = nullptr);
    void _AddEntryAtCurrentPosition(const TextEntry &entry);
    const TextEntry *_GetEntry();

private:
    void OnColumnClicked(NMHDR* pNMHDR, LRESULT* pResult);
    void _SortItemsHelper(int sortColumn, bool toggle);
    int _GetViewIndex(int index);
    const MessageSource *_GetMessageSource(int column);

    int _iView;
    bool _fInited;
    std::vector<LVCOLUMN> _columns;

    std::unordered_map<int, bool> _sortOrder;
    int _iSortColumn;
    int _contextMenuColumn;
};

#ifndef _DEBUG  // debug version in TextView.cpp
inline CMessageDoc* CMessageView::GetDocument() const
{ return reinterpret_cast<CMessageDoc*>(m_pDocument); }
#endif


