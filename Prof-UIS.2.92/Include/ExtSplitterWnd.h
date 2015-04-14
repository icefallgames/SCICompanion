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

#if (!defined __EXT_SPLITTER_WND_H)
#define __EXT_SPLITTER_WND_H

#if (!defined __EXT_MFC_NO_SPLITTER_WND)

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

/////////////////////////////////////////////////////////////////////////////
// CExtSplitterBaseWnd

class __PROF_UIS_API CExtSplitterBaseWnd
	: public CSplitterWnd
	, public CExtPmBridge
{
public:
	DECLARE_DYNCREATE( CExtSplitterBaseWnd );
	DECLARE_CExtPmBridge_MEMBERS( CExtSplitterBaseWnd );
	CExtSplitterBaseWnd();
	virtual ~CExtSplitterBaseWnd();

	COLORREF m_clrBackground;
	virtual COLORREF OnQueryBackgroundColor() const;

	enum HitTestValue
	{
		noHit                   = 0,
		vSplitterBox            = 1,
		hSplitterBox            = 2,
		bothSplitterBox         = 3,
		vSplitterBar1           = 101,
		vSplitterBar15          = 115,
		hSplitterBar1           = 201,
		hSplitterBar15          = 215,
		splitterIntersection1   = 301,
		splitterIntersection225 = 525
	};
	static HCURSOR g_split_hcurLast;
	static HCURSOR g_split_hcurDestroy;
	static UINT g_split_idcPrimaryLast;
	static int stat_CanSplitRowCol( CRowColInfo * pInfoBefore, int nBeforeSize, int nSizeSplitter );
	static void stat_LayoutRowCol( CRowColInfo * pInfoArray, int nMax, int nSize, int nSizeSplitter );
	static void stat_DeferClientPos( AFX_SIZEPARENTPARAMS* lpLayout, CWnd * pWnd, int x, int y, int cx, int cy, BOOL bScrollBar );
	class __PROF_UIS_API friently_view_t : public CView
	{
	public:
		friend class CExtSplitterBaseWnd;
	};
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump( CDumpContext & dc ) const;
#endif
	virtual BOOL Create(
		CWnd * pParentWnd,
		int nMaxRows,
		int nMaxCols,
		SIZE sizeMin,
		CCreateContext * pContext,
		DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_HSCROLL|WS_VSCROLL|SPLS_DYNAMIC_SPLIT,
		UINT nID = AFX_IDW_PANE_FIRST
		);
	virtual BOOL CreateStatic(
		CWnd * pParentWnd,
		int nRows,
		int nCols,
		DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		UINT nID = AFX_IDW_PANE_FIRST
		);
	virtual BOOL CreateView(
		int row,
		int col,
		CRuntimeClass * pViewClass,
		SIZE sizeInit,
		CCreateContext * pContext
		);
	virtual int GetRowCount() const { return m_nRows; }
	virtual int GetColumnCount() const { return m_nCols; }
	virtual void GetRowInfo( int row, int & cyCur, int & cyMin ) const;
	virtual void SetRowInfo( int row, int cyIdeal, int cyMin );
	virtual void GetColumnInfo( int col, int & cxCur, int & cxMin ) const;
	virtual void SetColumnInfo( int col, int cxIdeal, int cxMin );
	virtual DWORD GetScrollStyle() const;
	virtual void SetScrollStyle( DWORD dwStyle );
	virtual CWnd * GetPane(int row, int col) const;
	virtual BOOL IsChildPane( CWnd * pWnd, int * pRow, int * pCol );
	virtual BOOL IsChildPane( CWnd * pWnd, int & row, int & col ) { return IsChildPane(pWnd, &row, &col); }
	virtual int IdFromRowCol( int row, int col ) const;
	virtual BOOL IsTracking() { return m_bTracking; }
	virtual void RecalcLayout();
	virtual BOOL CreateScrollBarCtrl( DWORD dwStyle, UINT nID );
	virtual void DeleteView( int row, int col );
	virtual BOOL SplitRow( int cyBefore );
	virtual BOOL SplitColumn( int cxBefore );
	virtual void DeleteRow( int rowDelete );
	virtual void DeleteColumn( int colDelete );
	virtual CWnd * GetActivePane( int * pRow = NULL, int * pCol = NULL );
	virtual void SetActivePane( int row, int col, CWnd * pWnd = NULL );
	virtual BOOL CanActivateNext( BOOL bPrev = FALSE );
	virtual void ActivateNext( BOOL bPrev = FALSE );
	virtual BOOL DoKeyboardSplit();
	virtual BOOL DoScroll( CView * pViewFrom, UINT nScrollCode, BOOL bDoScroll = TRUE );
	virtual BOOL DoScrollBy( CView * pViewFrom, CSize sizeScroll, BOOL bDoScroll = TRUE );
protected:
	virtual void OnDrawSplitter( CDC * pDC, ESplitType nType, const CRect & rect );
	virtual void OnInvertTracker( const CRect & rect );
	CWnd * GetActivePane( int & row, int & col ) { return GetActivePane( &row, &col ); }
//	CRuntimeClass * m_pDynamicViewClass;
//	int m_nMaxRows, m_nMaxCols, m_cxSplitter, m_cySplitter, m_cxBorderShare, m_cyBorderShare,
//		m_cxSplitterGap, m_cySplitterGap, m_cxBorder, m_cyBorder, m_nRows, m_nCols, m_htTrack;
//	BOOL m_bHasHScroll, m_bHasVScroll, m_bTracking, m_bTracking2;
//	CRowColInfo * m_pColInfo, m_pRowInfo;
//	CPoint m_ptTrackOffset;
//	CRect m_rectLimit, m_rectTracker, m_rectTracker2;
	virtual BOOL CreateCommon( CWnd * pParentWnd, SIZE sizeMin, DWORD dwStyle, UINT nID );
	virtual int HitTest( CPoint pt ) const;
	virtual void GetInsideRect( CRect & rect ) const;
	virtual void GetHitRect( int ht, CRect & rect );
	virtual void TrackRowSize( int y, int row );
	virtual void TrackColumnSize( int x, int col );
	virtual void DrawAllSplitBars( CDC * pDC, int cxInside, int cyInside );
	virtual void SetSplitCursor( int ht );
	virtual CWnd * GetSizingParent();
	virtual void StartTracking( int ht );
	virtual void StopTracking( BOOL bAccept );
	virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
	virtual BOOL OnNotify( WPARAM wParam, LPARAM lParam, LRESULT * pResult );
	//{{AFX_MSG(CExtSplitterBaseWnd)
	afx_msg BOOL OnSetCursor( CWnd * pWnd, UINT nHitTest, UINT message );
	afx_msg void OnMouseMove( UINT nFlags, CPoint pt);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown( UINT nFlags, CPoint pt );
	afx_msg void OnLButtonDblClk( UINT nFlags, CPoint pt );
	afx_msg void OnLButtonUp( UINT nFlags, CPoint pt );
	afx_msg void OnCancelMode();
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnSize( UINT nType, int cx, int cy);
	afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar * pScrollBar );
	afx_msg void OnVScroll( UINT nSBCode, UINT nPos, CScrollBar * pScrollBar );
	afx_msg BOOL OnMouseWheel( UINT nFlags, short zDelta, CPoint pt );
	afx_msg BOOL OnNcCreate( LPCREATESTRUCT lpcs );
	afx_msg void OnSysCommand( UINT nID, LPARAM lParam );
	//}}AFX_MSG
	afx_msg void OnDisplayChange();
	afx_msg LRESULT OnNextDlgCtrl( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CExtSplitterWnd

class __PROF_UIS_API CExtSplitterWnd : public CExtSplitterBaseWnd
{
public:
	DECLARE_DYNCREATE( CExtSplitterWnd );

	CExtSplitterWnd();
	virtual ~CExtSplitterWnd();

protected:
	virtual void DrawAllSplitBars(
		CDC * pDC,
		int cxInside,
		int cyInside
		);
	virtual void OnDrawSplitter(
		CDC* pDC,
		ESplitType nType,
		const CRect & rectArg
		);
	virtual void OnInvertTracker(
		const CRect & rect
		);
public:
	virtual BOOL CreateScrollBarCtrl( DWORD dwStyle, UINT nID );

	//{{AFX_VIRTUAL(CExtSplitterWnd)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CExtSplitterWnd)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; // class CExtSplitterWnd

#endif // (!defined __EXT_MFC_NO_SPLITTER_WND)

#endif // !defined(__EXT_SPLITTER_WND_H)
