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

//#define VISUAL_SCRIPT_EDITOR

#ifdef VISUAL_SCRIPT_EDITOR

// CVisualScriptDialog dialog

#include "ScrollingThing.h"
#include "SCIPropertyCtrl.h"
#include "PicDrawManager.h"
#include "ScriptOM.h"

class sci::Script;
class SCIClassBrowser;
class PicResource;
class CVisualScriptDialog;

class CVisualScript : public CScrollingThing<CWnd>
{
public:
    CVisualScript()
    {
        _iZoom = 2;
        _pbitmapDoubleBuf = NULL;
        _fDoubleBuf = FALSE;
        _pdataDisplay = NULL;
        _pdataAux = NULL;
        _pPic = NULL;
        LOGBRUSH logBrush = { BS_HOLLOW, 0, 0 };
        _brushHollow.CreateBrushIndirect(&logBrush);
        _fInCapture = FALSE;
        _wPic = 0;
    }

    virtual ~CVisualScript();

    void OnPaint();
    void OnMouseMove(UINT nFlags, CPoint point);
    void OnLButtonDown(UINT nFlags, CPoint point);
    void OnRButtonDown(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point);
    void OnLButtonDblClk(UINT nFlags, CPoint point);
    BOOL OnEraseBkgnd(CDC *pDC);
    void OnKillFocus(CWnd *pNewWnd);
    void OnSetFocus(CWnd *pNewWnd);
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    UINT OnGetDlgCode() { return DLGC_WANTARROWS | DLGC_WANTTAB; } // Don't call super, it includes DLGC_STATIC

    void SetScript(const sci::Script *pScript, CVisualScriptDialog *pDialog) { _pScript = pScript; _pDialog = pDialog; }
    void UpdateViews();
    void SetPic(WORD wRes) { _wPic = wRes; _GeneratePicResource(); }

private:
    DECLARE_MESSAGE_MAP()
    virtual int _GetViewWidth() { return DEFAULT_PIC_WIDTH * _iZoom; }
    virtual int _GetViewHeight() { return DEFAULT_PIC_HEIGHT * _iZoom; }
    CPoint _MapClientPointToPic(CPoint ptClient);
    void _SelectItemFromPicPoint(CPoint point);
    void _CleanUp();
    void _OnDraw(CDC *pDC);
    void _CleanDoubleBuffer();
    void _GenerateDoubleBuffer(CDC *pDC);
    void _GeneratePicResource();
    BOOL _EnsureViews();

    int _iZoom;

    int _fDoubleBuf;
    CBitmap *_pbitmapDoubleBuf;
    WORD _wPic;
    const sci::Script *_pScript; // Not owned by us.

    PicResource *_pPic;
    PicDrawManager _pdm;

    BYTE *_pdataDisplay;
    BYTE *_pdataAux;

    CVisualScriptDialog *_pDialog;
    CArray<BOOL, BOOL> _viewHotState;
    CBrush _brushHollow;

    BOOL _fInCapture;
    CPoint _ptCapture;
};

class CVisualScriptView : public CSCIPropertyBagNotify
{
public:
    CVisualScriptView(SCIClassBrowser *pBrowser, sci::ClassDefinition *pClass, WORD wView) :
        CSCIPropertyBagNotify(pBrowser, pClass)
    {
        _fActor = FALSE; // Must come before _CacheProperties.
        _pView = NULL;
        _wView = wView;
        _CacheProperties(TRUE);
        _RefreshView();
        _iCycle = -1;
        _fMoving = FALSE;
        _cycleSpeedCount = 0;
    }

    ~CVisualScriptView();

    void OnDraw(BYTE *pDataDisplay, const BYTE *pDataPriority, int iPriorityLevel, BOOL fOutline = FALSE);
    BOOL HitTestView(CPoint ptPic);
    void GetViewRect(CRect *prc);
    void Offset(CPoint ptDelta);
    void SetPriority(int iPri);
    std::string GetSuperClass() { return _pClass->GetSuperClass(); }
    void Cycle(BOOL fCycle = TRUE);
    void Move();

    // ISCIPropertyBagNotify override
    bool SetProperty(PCTSTR pszName, sci::PropertyValue value)
    {
        bool fRet = __super::SetProperty(pszName, value);
        _CacheProperties();
        return fRet;
    }

private:
    void _CacheProperties(BOOL fFirstTime = FALSE);
    int _GetCel() { return (_iCycle == -1) ? _nCel : _iCycle; }
    int _GetY() { return _fMoving ? _yTemp : _y; }
    int _GetX() { return _fMoving ? _xTemp : _x; }
    void _RefreshView();

    ViewResource *_pView;
    WORD _wView;

    __int16 _nCel;
    __int16 _nLoop;
    __int16 _x;
    __int16 _y;
    __int16 _z;
    __int16 _iPri;
    __int16 _cycleSpeed;
    __int16 _moveSpeed;
    __int16 _xStep;
    __int16 _yStep;
    CString _strSpecies;

    // For animation.
    int _iCycle;
    __int16 _cycleSpeedCount;
    BOOL _fMoving;
    __int16 _moveSpeedCount;
    __int16 _xTemp;
    __int16 _yTemp;
    int _iDir; 

    BOOL _fActor;
};

typedef CArray<CVisualScriptView*, CVisualScriptView*> CVisualScriptViewArray;

class CVisualScriptDialog : public CDialog, public ISCIPropertyChange
{
	DECLARE_DYNAMIC(CVisualScriptDialog)

public:
    CVisualScriptDialog(sci::Script *pScript, SCIClassBrowser *pBrowser, CWnd* pParent = NULL);   // standard constructor
	virtual ~CVisualScriptDialog();
    CVisualScriptViewArray &GetViews() { return _views; }
    int GetSelectedViewIndex() { return _iSelected; }
    void SetSelectedViewIndex(int iSelected);

// Dialog Data
	enum { IDD = IDD_VISUALSCRIPTDIALOG };

    afx_msg void OnCommandUIAlwaysValid(CCmdUI *pCmdUI) { pCmdUI->Enable(TRUE); }

    // ISCIPropertyChange
    virtual void OnPropertyChange(ISCIPropertyBag *pSource, PCTSTR pszName, sci::PropertyValue value);
    virtual void OnNameChange(ISCIPropertyBag *pSource, PCTSTR pszName);
    virtual void OnSpeciesChange(ISCIPropertyBag *pSource, PCTSTR pszSpecies) { /*TODO*/ }
    virtual void OnAddProperty(ISCIPropertyBag *pSource, PCTSTR pszName) { /* TODO*/ };
    virtual void OnRemoveProperty(ISCIPropertyBag *pSource, PCTSTR pszName) { /* TODO*/ };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnAnimateClicked();
    afx_msg void OnMoveClicked();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    void _UpdateNameDisplay();

    CVisualScript m_wndVisualScript;
    sci::Script *_pScript;
    SCIClassBrowser *_pBrowser;

    CVisualScriptViewArray _views;
    int _iSelected; // Index into _views

    CSCIPropertyCtrl m_wndProperties;
    CTreeCtrl m_wndObjectTree;
    CTabCtrl m_wndTabs;

    CSize _size;

    CImageList m_imagelist;
    CFont m_fontObjectName;

    CButton m_wndAnimate;
    CButton m_wndMove;
    BOOL _fAnimating;
    BOOL _fMoving;

    bool _fInitialized;
};


#endif