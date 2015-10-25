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
class CTextDoc;

// CTextView view

class CTextView : public CListView
{
	DECLARE_DYNCREATE(CTextView)

protected:
	CTextView();           // protected constructor used by dynamic creation
	virtual ~CTextView();

public:
    CTextDoc* GetDocument() const;
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
    void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
    void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
    void OnItemDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
    void OnDelete();
    void OnUpdateIfSelection(CCmdUI *pCmdUI);
    void OnNewString();
    void OnRename();
    void OnMoveDown();
    void OnMoveUp();
    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

    DECLARE_MESSAGE_MAP()

    void _InitColumns();
    void _InsertItem(int iItem, PCTSTR pszString);
    void _OnEndEditingNewItem(PCTSTR pszString, NMLVDISPINFO *plvdi);
    void _ChangeView();
    int _GetSelectedItem();
    void _SwapStrings(int iIndex1, int iIndex2);

private:
    int _iView;
    bool _bInLabelEdit;
    bool _bEditingNewItem;
    int _iNewString;
    bool _fInited;
    std::vector<LVCOLUMN> _columns;
};

#ifndef _DEBUG  // debug version in TextView.cpp
inline CTextDoc* CTextView::GetDocument() const
   { return reinterpret_cast<CTextDoc*>(m_pDocument); }
#endif


