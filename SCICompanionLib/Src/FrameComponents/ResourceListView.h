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


// CResourceListCtrl view


// FWD decl
class CResourceListDoc;
class ResourceBlob;

typedef CDocument*(* PFNRESOURCEOPEN )(const ResourceBlob *pData);

#define OVERLAYINDEX_UNUSED 1

class CResourceListCtrl : public CListCtrl
{
	DECLARE_DYNCREATE(CResourceListCtrl)

public:
	CResourceListCtrl();
	virtual ~CResourceListCtrl();
    CResourceListDoc* GetDocument() const; // non-debug version is inline
    void SetDocument(CResourceListDoc *pDocument) { m_pDocument = pDocument; }
    void SetType(ResourceType dwType) { _dwType = dwType; }
    // subclass overrides to specify the type of resource they show
    virtual ResourceType GetType() { return _dwType; }
    virtual DWORD GetDefaultViewMode() { return LVS_REPORT; }
    void UpdateView();
    void ChangeViewMode(int iView);
    int GetViewMode() { return _iView; }
    void OnUpdate(LPARAM lHint, CObject *pHint);
    void SetOpenFunction(PFNRESOURCEOPEN pfnOpen) { _pfnOpen = pfnOpen; }
    void OnDelete();
    int GetSelectedItem();
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    BOOL InLabelEdit() { return _bInLabelEdit; }
    void OnRename();
    void OnExtractResources();
    void OnViewRawData();
    BOOL PreTranslateMessage(MSG *pMsg);

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    void OnItemClick(NMHDR* pNMHDR, LRESULT* pResult);
    void OnItemDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
    void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
    void OnColumnClicked(NMHDR* pNMHDR, LRESULT* pResult);
    void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
    void OnNotifyDelete(NMHDR* pNMHDR, LRESULT* pResult);
    void OnGetInfoTip(NMHDR* pNMHDR, LRESULT* pResult);
    void OnBeginScroll(NMHDR* pNMHDR, LRESULT* pResult);
    void OnEndScroll(NMHDR* pNMHDR, LRESULT* pResult);
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult);
    void OnContextMenu(CWnd *pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()

    void _SortItemsHelper(int sortColumn, bool toggle);
    void _UpdateStatusIfFlagsChanged(const ResourceBlob &data, ResourceLoadStatusFlags originalFlags, int item);
    void _InitColumns();
    void _InsertItem(ResourceBlob *pData);
    void _DeleteItem(const ResourceBlob *pData);
    virtual void _PrepareLVITEM(LVITEM *pItem);
    virtual void _OnItemDoubleClick(const ResourceBlob *pData);
    virtual void _OnInitListView(int cItems);
    void _ReevaluateRecency(const ResourceBlob *pData);
    HRESULT _UpdateEntries();
    ResourceBlob *_GetResourceForItem(int nItem) { return (ResourceBlob*)GetItemData(nItem); }
    int _GetItemForResource(const ResourceBlob *pData);
    void _DeleteMatchingItems(int resourceNumber, int packageNumber, ResourceSourceFlags sourceFlags);

    int _iView;
    BOOL _bFirstTime;
    BOOL _bDidInitialUpdate;
    BOOL _bInLabelEdit;
    BOOL _bScrolling;

    CResourceListDoc *m_pDocument;
    ResourceType _dwType;
    PFNRESOURCEOPEN _pfnOpen;

    std::unordered_map<int, bool> _sortOrder;
    int _iSortColumn;
};


#ifndef _DEBUG  // debug version in VocabView.cpp
inline CResourceListDoc* CResourceListCtrl::GetDocument() const
   { return reinterpret_cast<CResourceListDoc*>(m_pDocument); }
#endif

