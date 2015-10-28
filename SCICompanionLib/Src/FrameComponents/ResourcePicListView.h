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
// ResourcePicListView.h
//

#pragma once

#include "ResourceListView.h"
#include "QueueItems.h"
#include "ResourceBlob.h"

// This is created by the UI thread, and deleted by the worker thread.
class PICWORKITEM
{
public:
    ResourceBlob blob;
};


// This is created by the worker thread, posted to the UI thread.
// Well, actually just the message "check for results" is posted to the 
// UI thread.
class PICWORKRESULT
{
public:
    PICWORKRESULT() { hbmp = NULL; nID= 0; }
    ~PICWORKRESULT() { DeleteObject(hbmp); }
    HBITMAP hbmp;
    UINT nID;
    int iResourceNumber;
    int iPackageNumber;

    static PICWORKRESULT *CreateFromWorkItem(PICWORKITEM *pWorkItem);
};

class CResourcePicListCtrl : public CResourceListCtrl
{
protected: // create from serialization only
    CResourcePicListCtrl();
    DECLARE_DYNCREATE(CResourcePicListCtrl)

// Attributes
public:
    virtual ResourceType GetType() { return ResourceType::Pic; }
    virtual DWORD GetDefaultViewMode() { return LVS_ICON; }

// Operations
public:

// Implementation
public:
    virtual ~CResourcePicListCtrl();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
    void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);

protected:
    virtual void _PrepareLVITEM(LVITEM *pItem);
    virtual void _OnInitListView(int cItems);

// Generated message map functions
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    LRESULT OnPicReady(WPARAM wParam, LPARAM lParam);
    BOOL SetItemImage(int nItem , int nImageIndex);
    DECLARE_MESSAGE_MAP()

private:
    HIMAGELIST _himlPics;

    int _iTokenImageIndex;
    QueueItems<PICWORKITEM, PICWORKRESULT> *_pQueue;
};

