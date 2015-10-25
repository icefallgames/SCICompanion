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

// Comment this out to go back to the classbrowser way of building RoomExplorer
#define USE_COMPILED_INFO

#include "ScrollingThing.h"
#include "ClassBrowser.h"
#include "QueueItems.h"

struct PaletteComponent;
class CRoomExplorerDoc;
class CompiledObjectBase;
class GlobalCompiledScriptLookups;

class CRoomView
{
public:
    CRoomView();
    ~CRoomView();

    uint16_t wView;
    uint16_t wx;
    uint16_t wy;
    uint16_t wLoop;
    uint16_t wCel;
    CSize size;
    HBITMAP hbmp;
    ResourceBlob blob;
};

class CRoomExplorerWorkItem
{
public:
    CRoomExplorerWorkItem(uint16_t wScriptIn) : wScript(wScriptIn) {}

    uint16_t wScript;
    ResourceBlob blob;
    std::vector<std::unique_ptr<CRoomView>> _views;
};


class CRoomExplorerWorkResult
{
public:
    CRoomExplorerWorkResult() { wScript= 0; }
    std::unique_ptr<BYTE[]> pBitmapData;
    uint16_t wScript;
    std::unique_ptr<PaletteComponent> optionalPalette;

    static CRoomExplorerWorkResult *CreateFromWorkItem(CRoomExplorerWorkItem *pWorkItem);
};

// fwd decl
class CRoomExplorerNode;

struct ImportantSelectors
{
    uint16_t North;
    uint16_t East;
    uint16_t South;
    uint16_t West;
    uint16_t Picture;
    uint16_t View;
    uint16_t X;
    uint16_t Y;
    uint16_t Loop;
    uint16_t Cel;
};

class CRoomExplorerView : public CScrollingThing<CView>
{
protected: // create from serialization only
    CRoomExplorerView();
    DECLARE_DYNCREATE(CRoomExplorerView)

// Attributes
public:
    CRoomExplorerDoc* GetDocument() const;

// Overrides
    public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
protected:
    virtual BOOL OnEraseBkgnd(CDC *pDC);

// Implementation
public:
    virtual ~CRoomExplorerView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);
    virtual void OnInitialUpdate();
    virtual void OnSize(UINT nType, int cx, int cy);
    virtual void OnMouseMove(UINT nFlags, CPoint point);
    virtual void OnTimer(UINT_PTR nIDEvent);
    virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
    virtual void OnKillFocus(CWnd *pNewWnd);
    virtual void OnLButtonDown(UINT nFlags, CPoint point);
    virtual void OnLButtonUp(UINT nFlags, CPoint point);

    QueueItems<CRoomExplorerWorkItem, CRoomExplorerWorkResult> *GetQueue() { return _pQueue; }

    void InvalidateGridRect(LPRECT prc);
    int GetZoom() { return _iZoom; }
    CPoint GetOrigin() { return CPoint(_xOrigin, _yOrigin); }

protected:
    LRESULT _OnMouseLeave(WPARAM wParam, LPARAM lParam);
    void _GenerateDoubleBuffer(CDC *pDC);
    void _CleanDoubleBuffer();
    void _OnDraw(CDC *pDC, BOOL fHitTestOnly = FALSE, CPoint point = CPoint(0, 0), uint16_t *pwRoom = NULL);
    void _RecalcHeight();
    LRESULT _OnRoomBitmapReady(WPARAM wParam, LPARAM lParam);
    void _OnMouseWheel(UINT nFlags, BOOL fForward, CPoint pt, short nNotches);
    CPoint _GetMoveDelta(int *piMoveDirection = NULL);
    void _UpdateCursor();

    // Scrolling
    virtual int _GetViewWidth() { return _cx / _iZoom; }
    virtual int _GetViewHeight() { return _cy / _iZoom; }

    CPoint _MapClientPointToView(CPoint ptScreen);
    CPoint _MapViewPointToClient(CPoint ptPic);

// Generated message map functions
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()

public:
    class CRoomExplorerGrid
    {
    public:
        CRoomExplorerGrid();
        void SetRoomExplorer(CRoomExplorerView *pExplorer) { _pExplorer = pExplorer; }

#ifdef USE_COMPILED_INFO
        void AddRoom(GlobalCompiledScriptLookups &globalLookups, CompiledScript &compiledScript, CompiledObjectBase &classDefinition, const ImportantSelectors &importantSelectors, uint16_t viewSpeciesIndex);
#else
		void AddRoom(SCIClassBrowser &browser, uint16_t wScript, const sci::ClassDefinition *pClass, const std::vector<std::unique_ptr<sci::ClassDefinition>> &classes);
#endif
        
        void OrganizeRooms();
        void LoadResources();
        BOOL IsComplete() { return _fIsComplete; }
        ~CRoomExplorerGrid();
        const CRect *GetBounds();
        void DrawRooms(CDC *pDC, BOOL fHitTestOnly, CPoint pt, uint16_t *pwRoom);
        const std::unordered_map<uint16_t, std::unique_ptr<ResourceBlob>> &GetPics() const { return _pics; }
        const std::unordered_map<uint16_t, std::unique_ptr<ResourceBlob>> &GetViews() const { return _views; }
        CRoomExplorerNode *GetNode(uint16_t wScript);
        void SetHoveredRoom(uint16_t wRoom);

    private:
        void _DrawLineTo(CDC *pDC, CPoint pt, uint16_t wRoom);
        void _GetRoomRect(CRoomExplorerNode *pNode, CRect &rect);
        const CRoomExplorerNode *_GetRoomAt(int x, int y);
 
		typedef std::unordered_map<uint16_t, std::unique_ptr<CRoomExplorerNode>> numToNode_t;
        numToNode_t _nodes;

        std::unordered_map<uint16_t, std::unique_ptr<ResourceBlob>> _pics;
        std::unordered_map<uint16_t, std::unique_ptr<ResourceBlob>> _views;

        BOOL _fIsComplete;
        CRoomExplorerView *_pExplorer;

        uint16_t _wHoverScript;
        uint16_t _wSelectedScript;

        CRect _rcBounds;
    };

private:
    int _cy;
    int _cx;
    int _iZoom;

    BOOL _fDoubleBuf;
    CBitmap *_pbitmapDoubleBuf;
    CPoint _ptHoverClient;
    BOOL _fMoveTimer;
    BOOL _fInCapture;
    BOOL _fMouseButtonDown;

    CRoomExplorerGrid _grid;
    QueueItems<CRoomExplorerWorkItem, CRoomExplorerWorkResult> *_pQueue;

    int _iMoveDirection;
};

class CRoomExplorerNode
{
public:
    CRoomExplorerNode();
    void OnDraw(CDC *pDC, CRect *prc, QueueItems<CRoomExplorerWorkItem, CRoomExplorerWorkResult> *pQueue);

#ifdef USE_COMPILED_INFO
    void Init(const CRoomExplorerView::CRoomExplorerGrid *pGrid, GlobalCompiledScriptLookups &globalLookups, CompiledScript &compiledScript, CompiledObjectBase &classDefinition, const ImportantSelectors &importantSelectors, uint16_t viewSpeciesIndex);
    void _AddView(CompiledObjectBase &classDefinition, GlobalCompiledScriptLookups &globalLookups, const ImportantSelectors &importantSelectors);
#else
	void Init(SCIClassBrowser &browser, const CRoomExplorerView::CRoomExplorerGrid *pGrid, uint16_t wScript, const sci::ClassDefinition *pClass, const std::vector<std::unique_ptr<sci::ClassDefinition>> &classes);
    void _AddView(const CRoomExplorerView::CRoomExplorerGrid *pGrid, SCIClassBrowser &browser, uint16_t wScript, const sci::ClassDefinition *pClass);
#endif

    ~CRoomExplorerNode();

    uint16_t North;
    uint16_t East;
    uint16_t South;
    uint16_t West;
    uint16_t Picture;
    uint16_t ScriptNum;

    BOOL Considered;
    CPoint Position;

    std::unique_ptr<BYTE[]> pBitmapData;
    std::unique_ptr<PaletteComponent> optionalPalette;

    HBITMAP BitmapScaled;

private:
    void _ResizeBitmap(CSize size);

    BOOL _fAskedForBitmap;
    CSize _sizeScaled; // of BitmapScaled
    const CRoomExplorerView::CRoomExplorerGrid *_pGrid;
    std::unique_ptr<CRoomExplorerWorkItem> _pWorkItem;
};

#ifndef _DEBUG  // debug version in FontPreviewView.cpp
inline CRoomExplorerDoc* CRoomExplorerView::GetDocument() const
   { return reinterpret_cast<CRoomExplorerDoc*>(m_pDocument); }
#endif

