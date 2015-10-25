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
// FontPreviewView.h : interface of the CFontPreviewView class
//

#pragma once

#include "ScrollingThing.h"
#include "NewRasterResourceDocument.h"

class CFontPreviewView : public CScrollingThing<CView>
{
protected: // create from serialization only
    CFontPreviewView();
    DECLARE_DYNCREATE(CFontPreviewView)

// Attributes
public:
    CNewRasterResourceDocument* GetDocument() const;

    void SetFontResource(ResourceEntity *pfr) { _fontResource = pfr; _RecalcHeight(); Invalidate(FALSE); }

// Overrides
    public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
protected:
    virtual BOOL OnEraseBkgnd(CDC *pDC);

// Implementation
public:
    virtual ~CFontPreviewView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
    virtual void OnInitialUpdate();
    virtual void OnSize(UINT nType, int cx, int cy);
    virtual BOOL OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message);
    void OverridePreviewLetters(const std::string &letters);

protected:
    void _GenerateDoubleBuffer(CDC *pDC);
    void _CleanDoubleBuffer();
    void _OnDraw(CDC *pDC, BOOL fHitTestOnly = FALSE, CPoint point = CPoint(0, 0), int *pnChar = NULL);
    void _RecalcHeight();
    const ResourceEntity *_GetFont();
    void _SyncLetters();
    void OnLButtonDown(UINT nFlags, CPoint point);

    // Scrolling
    int _GetViewWidth() override { return _cxPreview; }
    int _GetViewHeight() override;

// Generated message map functions
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()

private:

    // For scrolling (font coordinates)
    int _cyFont;
    int _cxFont;
    int _cxPreview;

    int _iZoom;

    bool _fDoubleBuf;
    CBitmap *_pbitmapDoubleBuf;

    ResourceEntity *_fontResource;

    std::string _previewLetters;
};

#ifndef _DEBUG  // debug version in FontPreviewView.cpp
inline CNewRasterResourceDocument* CFontPreviewView::GetDocument() const
{ return static_cast<CNewRasterResourceDocument*>(m_pDocument); }
#endif


