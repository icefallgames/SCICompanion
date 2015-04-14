// This is part of the Professional User Interface Suite library.
// Copyright (C) 2001-2011 FOSS Software, Inc.
// All rights reserved.
//
// http://www.prof-uis.com
// mailto:support@prof-uis.com
//
// This source code can be used, modified and redistributed
// under the terms of the license agreement that is included
// in the Professional User Interface Suite package.
//
// Warranties and Disclaimers:
// THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND
// INCLUDING, BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
// IN NO EVENT WILL FOSS SOFTWARE INC. BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY OR CONSEQUENTIAL DAMAGES,
// INCLUDING DAMAGES FOR LOSS OF PROFITS, LOSS OR INACCURACY OF DATA,
// INCURRED BY ANY PERSON FROM SUCH PERSON'S USAGE OF THIS SOFTWARE
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

#if( !defined __EXTMINIDOCKFRAMEWND_H)
#define __EXTMINIDOCKFRAMEWND_H

#if (!defined __AFXPRIV_H__)
	#include <AfxPriv.h>
#endif

#if (!defined __EXT_MOUSECAPTURESINK_H)
	#include <../Src/ExtMouseCaptureSink.h>
#endif

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CExtBarNcAreaButton;
class CExtControlBar;

/////////////////////////////////////////////////////////////////////////
// CExtMiniDockFrameWnd window

class __PROF_UIS_API CExtMiniDockFrameWnd
	: public CMiniDockFrameWnd
	, public CExtMouseCaptureSink
	, public CExtPmBridge
{
	bool m_bMovingLargeBar:1;

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CExtMiniDockFrameWnd)
    public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL Create(CWnd* pParent, DWORD dwBarStyle);
    //}}AFX_VIRTUAL
protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
public:
	virtual CExtControlBar * FindSingleVisbleFlatingBar();
	virtual void RenderNC( CDC & dc );
	BOOL PreTranslateMessage( MSG* pMsg );

	bool m_bHelperNcActive:1, m_bLockRecalcLayout:1;
	CToolTipCtrl m_wndToolTip;

// Implementation
public:
    DECLARE_DYNCREATE( CExtMiniDockFrameWnd );
	DECLARE_CExtPmBridge_MEMBERS( CExtMiniDockFrameWnd );

	CExtMiniDockFrameWnd();
	virtual ~CExtMiniDockFrameWnd();

	virtual void PmBridge_OnPaintManagerChanged(
		CExtPaintManager * pGlobalPM
		);
    
	CControlBar * GetControlBar();
	const CControlBar * GetControlBar() const;
	CExtControlBar * GetControlBarExt( bool bRootLayoutBar );
	const CExtControlBar * GetControlBarExt( bool bRootLayoutBar ) const;

protected:
	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		) const;
public:
	virtual void CalcWindowRect(
		LPRECT lpClientRect,
		UINT nAdjustType = CWnd::adjustBorder
		);
	virtual void OnQueryNcMetrics(
		INT & nResizingFrameDX,
		INT & nResizingFrameDY,
		INT & nCaptionDY
		);
	
    //{{AFX_MSG(CExtMiniDockFrameWnd)
	afx_msg void OnNcMouseMove( UINT nHitTest, CPoint point );
	afx_msg void OnTimer( __EXT_MFC_UINT_PTR nIDEvent );
	afx_msg void OnClose();
	afx_msg void OnNcPaint();
	afx_msg BOOL OnNcActivate(BOOL bActive);
    afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
    afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
    afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
    afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	//}}AFX_MSG
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection);
	afx_msg LRESULT OnDisplayChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnThemeChanged( WPARAM wParam, LPARAM lParam );
    DECLARE_MESSAGE_MAP()

private:
	static bool g_bOnWndPosChgProcessing;
public:
	static bool g_bAutoMakeTopmostOnMouseEvent;
protected:
	CSize	m_LastSize;
	CRect	m_rcBtnHideBar;
	bool	m_bBtnHideBar;
	CRect	m_rcFrameCaption;
	CPoint	m_ptLastResizing, m_ptResizingShift;
	
	void _AnalyzeButtonsHover(
		bool bAllowShowAdvacedTip = false
		);
	void _AnalyzeButtonsHover(
		CPoint point,
		bool bAllowShowAdvacedTip = false
		);

private:
	static bool g_bInResizingUpdateState;
	UINT m_nResizingMode;
	CRect m_rcWndResizingStart;
protected:
	bool _ResizingIsInProgress()
	{
		return (m_nResizingMode == HTNOWHERE) ? false : true;
	}
	void _ResizingStart( UINT nHitTest, const CPoint & point );
	void _ResizingEnd();
	void _ResizingUpdateState( const CPoint & point );
	friend class CExtBarNcAreaButton;
public:
	virtual CExtControlBar * _GetSingleVisibleBar();
	virtual bool _GetSingleVisibleCaptionText( CExtSafeString & sCaption );
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	static bool g_bMiniDockFrameWndClassRegistered;
	static bool RegisterMiniDockFrameWndClass();
}; // class CExtMiniDockFrameWnd

#define __EXT_MINIDOCKFRAMEWND_CLASS_NAME _T("ProfUIS-MiniDockFrameWnd")

#endif // __EXTMINIDOCKFRAMEWND_H
