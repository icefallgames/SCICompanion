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

// CEditViewCelChooser view
class CNewRasterResourceDocument;

class CEditViewCelChooser : public CListView
{
	DECLARE_DYNCREATE(CEditViewCelChooser)

protected:
	CEditViewCelChooser();           // protected constructor used by dynamic creation
	virtual ~CEditViewCelChooser();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
    void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
    void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
    void OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);

protected:
	DECLARE_MESSAGE_MAP()
    void OnMouseMove(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point);

    CSize _CalcMaxSize();
    BOOL _RegenImageList(int cCountHint);
    void _RefreshView(bool forceRegen);

private:
    CNewRasterResourceDocument *_GetDoc();
    HIMAGELIST _himlCels;
    CSize _sizeHiml;
    BOOL _fSetViewMode;

    int _iDragIndex;
    std::unique_ptr<CImageList> _pDragImage;

    BOOL _fDragEnabled;
};


