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

