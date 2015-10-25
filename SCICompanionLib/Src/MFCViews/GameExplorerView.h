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

// ResourceListView.h : interface of the ResourceManagerView class
//


#pragma once

#include "ResourceListView.h"

// FWD decl
class CResourceListDoc;

class CGameExplorerView : public CView
{
protected: // create from serialization only
    CGameExplorerView();
    DECLARE_DYNCREATE(CGameExplorerView)

// Attributes
public:
    CResourceListDoc* GetDocument() const;

// Operations
public:

// Overrides
    public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
    virtual void OnInitialUpdate();

    DROPEFFECT OnDragEnter(COleDataObject *pDataObject, DWORD dwKeyState, CPoint point);
    DROPEFFECT OnDragOver(COleDataObject *pDataObject, DWORD dwKeyState, CPoint point);
    void OnDragLeave();
    BOOL OnDrop(COleDataObject *pDataObject, DROPEFFECT dropEffect, CPoint point);

protected:
// Generated message map functions
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSelChange(NMHDR *pnmhdr, LRESULT *plres);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    void OnViewDetails();
    void OnViewThumbnails();
    void OnUpdateViewCommands(CCmdUI *pCmdUI);
    afx_msg void OnDelete();
    afx_msg void OnRename();
    afx_msg void OnExtractResources();
    afx_msg void OnViewRawData();
    afx_msg void OnUpdateDelete(CCmdUI *pCmdUI);
    afx_msg void OnUpdateItemCount(CCmdUI *pCmdUI);
    void OnDraw(CDC *pDC);
    DECLARE_MESSAGE_MAP()

protected:
    void _SyncSize();
    void _OnSelChange();
    void _OnSelChange(ResourceType iSelectedTab);
    CResourceListCtrl *_GetSelectedView();

    // Array of list controls (not list views)
    std::unique_ptr<CResourceListCtrl> _lists[NumResourceTypes];
    BOOL _bFirstSizing;

    COleDropTarget _dropTarget;
    CArray<CString, CString&> _dropFiles;

    ResourceType _iSelectedTab;

    CFont m_font;
};

#ifndef _DEBUG  // debug version in ResourceManagerView.cpp
inline CResourceListDoc* CGameExplorerView::GetDocument() const
   { return reinterpret_cast<CResourceListDoc*>(m_pDocument); }
#endif

