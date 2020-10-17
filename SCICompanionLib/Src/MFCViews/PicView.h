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
// PicView.h : interface of the CPicView class
//


#pragma once

#include "PicDoc.h"
#include "ScrollingThing.h"
#include "PaletteDefinitionCallback.h"
#include "Pic.h"
#include "PaletteOperations.h"
#include "NamedPosition.h"

class PicDrawManager;
class PicClipsDialog;

const int PriorityBarWidth = 30;

struct CommandModifier;
struct PicData;
class ViewPort;

class CPicView : public CScrollingThing<CView>, public IBitmapEditor, public IPicDrawPlugin, public IVGAPaletteDefinitionCallback, public IEGAPaletteDefinitionCallback
{
private: // create from serialization only
    CPicView();
    DECLARE_DYNCREATE(CPicView)

    enum ToolType
    {
        None         = 0x00000000,
        Command      = 0x00000001, // We're using a CommandType
        History      = 0x00000002,
        Zoom         = 0x00000003,
        Pasting      = 0x00000004, // We're currently pasting commands
        Polygons     = 0x00000005,
        NamedPositions = 0x00000006,
    };
    const static key_value_pair<ToolType, UINT> c_toolToID [];
    static ToolType _IDToTool(UINT nID);


// Attributes
public:
    CPicDoc* GetDocument() const;

// Operations
public:
    void SetOpacity(int iOpacity);
    int GetOpacity() { return _iTraceAlpha; }


// Overrides
    public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();

    // IPicDrawPlugin
    void DrawOnPic(ViewPort &viewPort, PicData &picData, PicScreenFlags flags) override;
    bool WillDrawOnPic() override;
    PicScreenFlags GetRequiredScreens() override;
    PicPositionFlags GetRequiredPicPosition() {
        // Pre and post. Pre because we want to ensure we keep around the main buffer
        // in order to quickly draw drawing tools on top. And Post because that's what
        // we actually render to the screen.
        return PicPositionFlags::PrePlugin | PicPositionFlags::PostPlugin;
    }

    // True if we need to redraw our layers on top
    bool EnsureBitmapUpToDate(CDC *pDC, PicScreen screen, bool force);

    // IBitmapEditor
    CPoint GetCursorPos() override;
    CSize GetBitmapSize() override;
    PicScreen GetPicScreen() override { return _mainViewScreen; }

    // IVGAPaletteDefinitionCallback
    void OnVGAPaletteChanged() override;
    // IEGAPaletteDefinitionCallback
    void SetPosition(ptrdiff_t position) override;

protected:
    void InvalidateOurselves();
    void InvalidateOurselvesImmediately();

    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual BOOL OnEraseBkgnd(CDC *pDC);
    virtual void OnRButtonDown(UINT nFlags, CPoint point);
    virtual void OnLButtonDown(UINT nFlags, CPoint point);
    virtual void OnLButtonDblClk(UINT nFlags, CPoint point);
    virtual void OnRButtonDblClk(UINT nFlags, CPoint point);
    virtual void OnLButtonUp(UINT nFlags, CPoint point);
    virtual void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

// Implementation
public:
    virtual ~CPicView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    virtual void OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint);

protected:
    CPoint _GetGutterOffset();
    const PicComponent *_GetEditPic() const;
    void _ClampPoint(point16 &point);
    void _ClampPoint(CPoint &point);
    size16 _GetPicSize() const;
    const PaletteComponent *_GetPalette();
    void _OnHistoryLClick(CPoint point);
    void _OnNamedPositionsLClick(CPoint point, bool allowClickOff, bool possiblyStartDrag);
    void _OnHistoryRClick(CPoint point);
    void _OnPatternLClick(CPoint point);
    void _OnPatternRClick();
    void _OnPicClipsLClick(CPoint point);
    void _OnPicClipsRClick();
    void _OnLineLClick(CPoint point);
    void _OnLineRClick(CPoint point);
    void _OnFillLClick(CPoint point);
    void _OnCircleLClick(CPoint point);
    void _OnCircleRClick(CPoint point);
    void _OnZoomLClick(CPoint *ppt = NULL);
    void _OnZoomRClick(CPoint *ppt = NULL);
    void _OnZoomClick(CPoint *ppt, int iMultiplier);
    void _OnPolygonLClick(CPoint point);
    void _OnPolygonRClick(CPoint point);
    void _EndNewPoly();
    void _EndPolyDrag();
    void _EndNamedPositionDrag(CPoint point);
    void _PushMousePositionToNamedPosition(CPoint point);
    void _OnPolyMouseMove(CPoint point);
    void _OnNamedPositionMouseMove(CPoint point);
    UINT _GetCurrentToolOrCommand();
    void _UpdateCursor();
    void _OnCommandChanged();
    void _OnShowPalette(BYTE b);
    void _DrawCoord(CDC *pDC, int x, int y, int additionalSize = 0);
    bool _IsCoordCloseEnough(int x, int y);
    int _GetTransformHitTestDistance(int x, int y);

    template<typename _Function>
    void _EnsurateCoordinates(PicCommand::CommandType commandType, _Function func);

    CPoint _MapClientPointToPic(CPoint ptScreen);
    CPoint _MapPicPointToClient(CPoint ptPic);
    void _EnsureDoubleBuffer();
    PicCommand _EnsurePriorityBarCommand();
    void _GenerateTraceImage(CDC *pDC);
    void _OnDraw(CDC* pDC, PicScreen screen);
    void _DrawPriorityBar(CDC *pDC, CPoint offset);
    bool _HitTestPriorityBar(CPoint pt, int *barIndex);
    void _MovePriorityBar(bool commit, int dy);
    void _DrawPriorityLines(CDC *pDC);
    int _HitTestCurrentPolyPoint(CPoint point);
    int _HitTestCurrentPolyEdge(CPoint point);
    void _InitPicClipsCommandAdjust(PICCOMMAND_ADJUST &adjust, CPoint point);
    void _DrawPolygons(CDC *pDC);
    void _DrawPolygon(CDC *pDC, const SCIPolygon *polygon, bool isActive);
    void _DrawShowingEgoEGA(ViewPort &viewPort, PicData &picData, PicScreenFlags flags);
    void _DrawNamedPositionsEGA(ViewPort &viewPort, PicData &picData, PicScreenFlags flags);
    void _DrawShowingEgoVGA(CDC &dc, PicDrawManager &pdm);
    CRect _DrawShowingEgoWorker(const ViewPort &viewPort, uint8_t *pdataVisual, const uint8_t *pdataPriority, PicScreenFlags flags, const NamedPosition &thing);
    void _DrawLineDraw(const ViewPort &viewPort, PicData data, PicScreenFlags screenFlags);
    void _DrawCircleDraw(const ViewPort &viewPort, PicData data, PicScreenFlags screenFlags);
    void _DrawPenPreview(const ViewPort &viewPort, PicData data, PicScreenFlags screenFlags);
    void _DrawClipPreview(const ViewPort &viewPort, PicData data, PicScreenFlags screenFlags);
    void _TransformPoint(bool commit, CPoint pt);
    void _DrawGuideLines(CDC &dcMem);
    void _InitCommandAdjust(PICCOMMAND_ADJUST *pAdjust);
    void _DrawPasteCommands(const ViewPort &viewPort, PicData data, PicScreenFlags screenFlags);
    void _DrawEgoCoordinates(CDC *pDC);
    void _DrawNamedPositionMarkers(CDC *pDC);
    void _DrawNamedPositionCoordinates(CDC *pDC, CPoint ptHitTest);
    void _DrawThingCoordinates(CDC *pDC, bool useBox, const NamedPosition &thing, COLORREF color, bool sciCoordinates, bool showName);
    void _MakeRandomNR();
    void _MakeNewMasterTraceImage(PCTSTR pszFileName, BITMAPINFO *pbmi, void *pBits);
    void _InsertPastedCommands();
    void _OnPasteCommands();
    void _GetPasteRect(CRect &rect);
    void _CleanUpPaste();
    void _OnMouseWheel(UINT nFlags, BOOL fForward, CPoint pt, short nNotches);
    bool _HitTestFakeEgo(CPoint pt);
    bool _HitTestViewThing(CPoint pt, const NamedPosition &thing);
    CPoint _FindCenterOfFakeEgo(const NamedPosition &thing);
    bool _NearPasteCorners();
    bool _EvaluateCanBeHere(CPoint pt);
    bool _CanBeInPolygons(CPoint pt);
    void _OnFakeEgoViewChosen(int resourceNumber);
    PicDrawManager &_GetDrawManager();
    const SCIPolygon *_GetCurrentPolygon();
    ResourceEntity *_GetFakeEgo();
    ResourceEntity *_GetViewResourceForThing(const NamedPosition &thing);
    void _OnCopyFakeEgoAttributesHelper();

    // Scrolling
    int _GetViewWidth() override;
    int _GetViewHeight() override;

    int GetPicScreenHeight() const;
    CPoint GetZoomOffset() const;

    void _SnapCoordinate(CPoint &pt);

// Generated message map functions
protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPenCommand();
    afx_msg void OnLineCommand();
    afx_msg void OnFillCommand();
    afx_msg void OnCircleCommand();
    afx_msg void OnPicClipsCommand();
    afx_msg void OnDrawOff();
    afx_msg void OnZoomTool();
    afx_msg void OnZoomIn() { _OnZoomLClick(&_ptCurrentHover); }
    afx_msg void OnZoomOut() { _OnZoomRClick(&_ptCurrentHover); }
    afx_msg void OnSetEgoPriority(UINT nID);
    afx_msg void OnEnableFakeEgo(UINT nID);
    afx_msg void OnHistoryTool();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnTraceBitmap();
    afx_msg void OnShowTraceImage();
	afx_msg void OnUnditherPic();
    afx_msg void OnToggleVisual();
    afx_msg void OnTogglePriority();
    afx_msg void OnToggleGridLines();
    afx_msg void OnToggleControl();
    afx_msg void OnShowVisual();
    afx_msg void OnShowPriority();
    afx_msg void OnShowControl();
    afx_msg void OnShowPalette0();
    afx_msg void OnShowPalette1();
    afx_msg void OnShowPalette2();
    afx_msg void OnShowPalette3();
    afx_msg void OnTogglePriorityLines();
    afx_msg void OnPolyPath();
    afx_msg void OnNamedPositions();
    afx_msg void OnToggleEgo();
    afx_msg void OnLightUpCoords();
    afx_msg void OnSetPalette();
    afx_msg void OnCopyPic();
    afx_msg void OnPaste();
    afx_msg void OnPasteIntoPic();
    afx_msg void OnObserveControlLines();
    afx_msg void OnObservePolygons();
    afx_msg void OnCopyFakeEgoAttributes();
    afx_msg void OnAppendCopyFakeEgoAttributes();
    afx_msg void OnExportPalettizedBitmap();
    afx_msg void OnExportAsAnimatedGIF();
    afx_msg void EditVGAPalette();
    afx_msg void ChangeDimensions();
    afx_msg void OnEnableFakeEgoCustom();
    afx_msg void OnDeletePoint();
    afx_msg void OnCutLine();
    afx_msg void RemoveSetVisual();
    afx_msg void EditCelData();
    afx_msg void OnPicClips();
    afx_msg void OnOpenView();
    afx_msg void OnUpdateOpenView(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAllPicCommands(CCmdUI *pCmdUI);
    afx_msg void OnUpdateShowTraceImage(CCmdUI *pCmdUI);
	afx_msg void OnUpdateUnditherPic(CCmdUI *pCmdUI);
    afx_msg void OnUpdateAlphaSlider(CCmdUI *pCmdUI);
    afx_msg void OnUpdateCutLine(CCmdUI *pCmdUI);
    afx_msg void OnCommandUIAlwaysValid(CCmdUI *pCmdUI) { pCmdUI->Enable(TRUE); }
    afx_msg void OnUpdateEditCelData(CCmdUI *pCmdUI);
    afx_msg void OnCommandUIStatus(CCmdUI *pCmdUI);
    afx_msg void OnUpdateShowScreenControl(CCmdUI *pCmdUI);
    afx_msg void OnUpdateShowPaletteControl(CCmdUI *pCmdUI);
    afx_msg void OnUpdateTogglePriorityLines(CCmdUI *pCmdUI);
    afx_msg void OnUpdateToggleGridLines(CCmdUI *pCmdUI);
    afx_msg void OnUpdateToggleEgo(CCmdUI *pCmdUI);
    afx_msg void OnUpdateLightUpCoords(CCmdUI *pCmdUI);
    afx_msg void OnUpdateToggleScreen(CCmdUI *pCmdUI);
    afx_msg void OnUpdateSetVisual(CCmdUI *pCmdUI);
    afx_msg void OnCommandUpdatePastePic(CCmdUI *pCmdUI);
    afx_msg void OnUpdateIsGDIPAvailable(CCmdUI *pCmdUI);
    afx_msg void OnUpdateObserveControlLines(CCmdUI *pCmdUI);
    afx_msg void OnUpdateObservePolygons(CCmdUI *pCmdUI);
    afx_msg void OnUpdateIsVGA(CCmdUI *pCmdUI);
    LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

private:

    void _AttachPicPlugin();
    bool _attachedPlugin;

    // For scrolling (screen coordinates)
    int _cyPic;
    int _cxPic;

    ToolType _currentTool;
    PicCommand::CommandType _currentCommand;    // Only valid if _currentTool == Command

    BOOL _fShowTraceImage;
    BOOL _fHaveTraceImage;

    BOOL _fShowPriorityLines;

    BOOL _fShowingEgo;

    std::vector<NamedPosition> _fakeEgoAttributes;
    BOOL _fCapturing;
    CPoint _pointEgoOrig;
    CPoint _pointCapture;
    bool _fCanBeHere;   // Can the fake ego be here?

    // When we're drawing temporary stuff on top (e.g. lines):
    // Everything here is screen coordinates (not pic coordinates)
    BOOL _fDrawingLine;
    BOOL _fDrawingCircle;
    BOOL _fPreviewPen;
    BOOL _fMouseWithin;
    BOOL _fPreviewClips;
    bool _fGridLines;

    bool _fPriLineHover;
    int _priBarMoveIndex;
    int _originalPriValue;
    PicCommand _originalPriValueCommand;

    // These are all in pic coordinates
    int _xOld;  // -1
    int _yOld;  // -1
    CPoint _ptCurrentHover;

    // This is our screen bitmap for double-buffering
    CBitmap _bitmapDoubleBuf;
    bool _fDoubleBuf;
    PicScreenFlags _isDrawUpToDate;

    // Trace image.
    CBitmap _bitmapTrace;
    std::unique_ptr<Gdiplus::Bitmap> _pgdiplusTrace;
    BOOL _fRegenerateTrace;

    // alpha-blended Priority lines
    CBitmap _bitmapPriLines;

    // Which screen are we drawing?
    PicScreen _mainViewScreen;

    BYTE _bRandomNR;

    bool _transformingCoords;
    std::unique_ptr<CommandModifier> _transformCommandMod;

    // For command pasting.
    std::vector<PicCommand> _pastedCommands;
    // The x, y, width and height of the commands that were pasted (doesn't change)
    int16_t _xPasted;
    int16_t _yPasted;
    int16_t _cxPasted;
    int16_t _cyPasted;
    // Offset from the *original* position of the commands (not from the current drag)
    int16_t _xPasteOffset;
    int16_t _yPasteOffset;
    // Offset from where we began a drag (e.g. zero at first)
    int16_t _xPasteOffsetFromCapture;
    int16_t _yPasteOffsetFromCapture;
    // Have the commands been flipped?
    bool _fHFlip;
    bool _fVFlip;
    // Pasted command rotation
    int _iAngle;
    int _iAngleCapture;
    bool _fRotating;
    // Pasted command scaling:
    int16_t _cxPastedScale;
    int16_t _cyPastedScale;
    bool _fInPasteRect;

    int _iTraceAlpha;

    bool _fDrawPasteBox; // yucj?

    // SCI1 Polygons
    std::unique_ptr<SCIPolygon> _currentPolyInEdit;
    // New ones:
    int _currentPolyPointIndexInEdit;
    CPoint _nextPolyPoint;
    // Old ones:
    int _currentHoverPolyPointIndex;
    int _currentHoverPolyEdgeIndex;
    int _polyDragPointIndex;
    point16 _startDragPolyPoint;
    point16 _currentDragPolyPoint;

    NamedPosition _capturedNamedPosition;
    int _hoverIndexNamedPosition;
    unordered_map<int16_t, unique_ptr<ResourceEntity>> _namedPositionViews;
    CFont _coordFont;
    uint16_t _gotoView;
};

#ifndef _DEBUG  // debug version in PicEditorView.cpp
inline CPicDoc* CPicView::GetDocument() const
   { return reinterpret_cast<CPicDoc*>(m_pDocument); }
#endif

//
// Slider for trace image alpha
//
class CExtAlphaSlider : public CExtBarSliderButton
{
public:
    CExtAlphaSlider::CExtAlphaSlider(CExtToolControlBar *pBar, UINT nCmdId) :
        CExtBarSliderButton(pBar, nCmdId, 0,
    		// scroll total/pos/page
            100, 50, 0,
		    // button extent horz(left/right)/vert(up/down) in pixels
		    // (if zeros - use slider-like layout instead of scrollbar-like)
		    0, 0,
		    // total slider control extent horz/vert in pixels
		    100, 100)
    {
        _pView = NULL;
    }
    void SetView(CPicView *pView)
    {
        if (pView != _pView)
        {
            _pView = pView;
            // Update from view
            ScrollPosSet((ULONG)_pView->GetOpacity());
        }
    }
    ULONG ScrollPosSet(ULONG nScrollPos)
    {
        // Push to view.
        ULONG nOldPos = __super::ScrollPosSet(nScrollPos);
        if (_pView)
        {
            // Note: It's possible we hold a ref to a _pView that gets
            // deleted, but this should be ok, since we'll never get here again unless
            // we are assigned a new view (since the scroll pos should only change if
            // 1) we are assigned a new view, or
            // 2) the user moves it, which means our view is still valid.
            _pView->SetOpacity((int)nScrollPos);
        }
        return nOldPos;
    }
private:
    CPicView *_pView;
};

extern CExtAlphaSlider *g_pPicAlphaSlider;

extern std::unique_ptr<PicClipsDialog> g_PicClipsDialog;
