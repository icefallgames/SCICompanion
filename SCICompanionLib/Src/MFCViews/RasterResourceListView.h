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
//
// RasterResourceListView.h
//

#pragma once

#include "ResourceListView.h"
#include "QueueItems.h"
#include "ResourceBlob.h"

// This is created by the UI thread, and deleted by the worker thread.
class VIEWWORKITEM
{
public:
    ResourceBlob blob;
    LPARAM lParam;
};


// This is created by the worker thread, posted to the UI thread.
// Well, actually just the message "check for results" is posted to the 
// UI thread.
class VIEWWORKRESULT
{
public:
    VIEWWORKRESULT() { hbmp = NULL; nID = 0; }
    ~VIEWWORKRESULT() { DeleteObject(hbmp); }
    HBITMAP hbmp;
    UINT nID;
    int iResourceNumber;
    int iPackageNumber;
    LPARAM lParam;

    static VIEWWORKRESULT *CreateFromWorkItem(VIEWWORKITEM *pWorkItem);
};

class CRasterResourceListCtrl : public CResourceListCtrl
{
protected: // create from serialization only
    CRasterResourceListCtrl();
    DECLARE_DYNCREATE(CRasterResourceListCtrl)

// Attributes
public:
    virtual DWORD GetDefaultViewMode() { return LVS_ICON; }

// Operations
public:

// Implementation
public:
    virtual ~CRasterResourceListCtrl();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
    void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);

private:
    virtual void _PrepareLVITEM(LVITEM *pItem);
    virtual void _OnInitListView(int cItems);

// Generated message map functions
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    LRESULT OnImageReady(WPARAM wParam, LPARAM lParam);
    BOOL SetItemImage(int nItem, int nImageIndex);
    DECLARE_MESSAGE_MAP()

    HIMAGELIST _himlPics;
    int _iCorruptBitmapIndex;
    int _iTokenImageIndex;
    std::unique_ptr<QueueItems<VIEWWORKITEM, VIEWWORKRESULT>> _pQueue;
    int _iLastImageReadyHint;
};

