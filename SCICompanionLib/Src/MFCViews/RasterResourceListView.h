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
    DECLARE_MESSAGE_MAP()

    HIMAGELIST _himlPics;
    int _iCorruptBitmapIndex;
};

