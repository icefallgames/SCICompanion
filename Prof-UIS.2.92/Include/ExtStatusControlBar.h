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

#if (!defined __EXT_STATUSCONTROLBAR_H)
#define __EXT_STATUSCONTROLBAR_H

#ifndef __EXT_MFC_NO_STATUSBAR

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CExtPopupMenuTipWnd;

/////////////////////////////////////////////////////////////////////////////
// CExtStatusControlBar window

class __PROF_UIS_API CExtStatusControlBar
	: public CStatusBar
	, public CExtPmBridge
{
// Construction
public:
	DECLARE_DYNCREATE( CExtStatusControlBar );
	DECLARE_CExtPmBridge_MEMBERS( CExtStatusControlBar );

	CExtStatusControlBar();

// Attributes
public:

// Operations
public:
	
	int GetPaneCount() const;	
	void SetPaneWidth(int nIndex, int nWidth);

	enum e_StatusPaneBackgroundAccent_t
	{
		__ESPBA_AUTOMATIC	= 0,
		__ESPBA_LIGHT		= 1,
		__ESPBA_DARK		= 2,
		__ESPBA_MIN_VALUE   = 0,
		__ESPBA_MAX_VALUE   = 2,
	};

protected:
	struct __PROF_UIS_API _ED_
	{
		UINT m_nDTF;
		CExtSafeString m_strTipText;
		e_StatusPaneBackgroundAccent_t m_eSPBA;
		COLORREF m_clrTextNormal;
		COLORREF m_clrTextDisabled;
		COLORREF m_clrBack;
		_ED_(	UINT _nDTF = 0,
				__EXT_MFC_SAFE_LPCTSTR _strTipText = NULL,
				COLORREF clrTextNormal = COLORREF(-1L),
				COLORREF clrTextDisabled = COLORREF(-1L),
				COLORREF clrBack = COLORREF(-1L)
				)
				: m_nDTF( _nDTF )
				, m_strTipText( _strTipText == NULL ? _T("") : _strTipText )
				, m_eSPBA( __ESPBA_AUTOMATIC )
				, m_clrTextNormal( clrTextNormal )
				, m_clrTextDisabled( clrTextDisabled )
				, m_clrBack( clrBack )
		{
		}
		_ED_ & operator = ( const _ED_ & other )
		{
			m_nDTF = other.m_nDTF;
			m_strTipText = other.m_strTipText;
			m_eSPBA = other.m_eSPBA;
			m_clrTextNormal = other.m_clrTextNormal;
			m_clrTextDisabled = other.m_clrTextDisabled;
			m_clrBack = other.m_clrBack;
			return (*this);
		}
		_ED_( const _ED_ & other )
		{
			(*this) = other;
		}
	}; // struct _ED_

	CMap < UINT, UINT, _ED_, _ED_ > m_mapIdToExtData;

	bool m_bLockPainting:1;

public:
	UINT GetPaneDrawTextFlags(
		int nIndex
		) const;
	void SetPaneDrawTextFlags(
		int nIndex,
		UINT _nDTF
		);

	virtual e_StatusPaneBackgroundAccent_t OnQueryPaneBackgroundAccent(
		int nIndex
		) const;
	e_StatusPaneBackgroundAccent_t GetPaneBackgroundAccent(
		int nIndex
		) const;
	void SetPaneBackgroundAccent(
		int nIndex,
		e_StatusPaneBackgroundAccent_t eSPBA
		);
	
	bool AddPane(
		UINT nID,	// ID of the  pane
		int nIndex	// index of the pane
		);
	
protected:
	bool _RemovePaneImpl(
		UINT nID,	// ID of the pane
		bool bRefresh
		);
public:
	bool RemovePane(
		UINT nID	// ID of the pane
		);
protected:
	int _RemovePanesImpl(
		int nIndexToRemove,
		int nCountToRemove,
		bool bRefresh
		);
public:
	int RemovePanes(
		int nIndexToRemove,
		int nCountToRemove
		);

	bool SetPaneControl(CWnd* pWnd, UINT nID, bool bAutoDestroy);
	bool SetPaneControl(HWND hWnd, UINT nID, bool bAutoDestroy);

	bool RemovePaneControl( UINT nID );
	
	void DrawPaneSeparatorsSet( bool bSet = true );
	bool DrawPaneSeparatorsGet() const;
	
	void OuterRectInFirstBandSet( bool bSet = true );
	bool OuterRectInFirstBandGet() const;
	
	void HideTextOnDisabledPanesSet( bool bSet = true );
	bool HideTextOnDisabledPanesGet() const;

	void EnableControl( int nIndex, bool bEnable = true );

	void SetPaneInfo(int nIndex, UINT nID, UINT nStyle, int cxWidth);
	void SetPaneStyle(int nIndex, UINT nStyle);

	void WidthFromContent( int nIndex );
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtStatusControlBar)
	protected:
	virtual void PostNcDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtStatusControlBar();

	UINT GetItemID(
		int nIndex
		) const;
	BOOL SetIndicators(
		const UINT * lpIDArray,
		int nIDCount
		);
	void DrawBorders( 
		CDC * pDC, 
		CRect & rc 
		);

	virtual int HitTestStatusPane(
		CPoint ptClient
		) const;
	
	CExtSafeString GetTipText( 
		int nPane 
		) 
		const;
	void SetTipText( 
		int nPane, 
		__EXT_MFC_SAFE_LPCTSTR pszTipText 
		);
	void SetPaneBkColor( 
		int nPane, 
		COLORREF clrBk = COLORREF(-1L)
		);
	void SetPaneTextColor(
		int nPane, 
		bool bEnabled,
		COLORREF clrText = COLORREF(-1L)
		);
	COLORREF GetPaneBkColor( 
		int nPane
		) const;
	COLORREF GetPaneTextColor(
		int nPane,
		bool bEnabled
		) const;

	INT m_nAdvancedTipStyle;

	bool m_bOuterRectInFirstBand:1;
	bool m_bCompleteRepaint:1;
	bool m_bDrawPaneSeparatorsInsteadOfBorders:1;
	bool m_bHideTextOnDisabledPanes:1;
	bool m_bDoNotPaintIcons:1;
	UINT m_nDrawPaneTextFlags; // DT_SINGLELINE|DT_LEFT|DT_VCENTER

	virtual bool OnQueryDrawPaneSeparatorsInsteadOfBorders() const;

protected:
	COLORREF m_clrStatusBk;
	COLORREF m_clrGripper;
	COLORREF m_clrPaneRect;
	COLORREF m_clrPaneTextNormal;
	COLORREF m_clrPaneTextDisabled;
	
	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		) const;

	virtual CExtPopupMenuTipWnd * OnAdvancedPopupMenuTipWndGet() const;
	virtual void OnAdvancedPopupMenuTipWndDisplay(
		CExtPopupMenuTipWnd & _ATTW,
		const RECT & rcExcludeArea,
		__EXT_MFC_SAFE_LPCTSTR strTipText
		) const;

	virtual void _RemovePanesImpl();
	void _SyncStatusBarColors();

	CFrameWnd * _GetDockingFrameImpl();

	virtual void DoPaint( CDC * pDC );

	virtual void OnPaintEntireBackground(
		CDC & dc,
		const CRect & rcBk
		);
	
	virtual void OnErasePaneBackground( // by default does nothing
		CDC & dc,
		int nPaneIdx,
		const CRect & rcPane
		);

	virtual void OnPaintPane(
		CDC & dc,
		int nPaneIdx,
		const CRect & rcPane,
		DWORD dwPaneStyle,
		HICON hIcon,
		__EXT_MFC_SAFE_LPCTSTR sPaneText, // by default it's text from the status bar control
		UINT nDrawTextFlags // by default m_nDrawPaneTextFlags is used
		);

	virtual COLORREF OnQueryPaneTextColor( // COLORREF(-1L) <- detect color automatically
		int nPaneIdx,
		bool bEnabled
		) const;
	virtual COLORREF OnQueryPaneBkColor( // COLORREF(-1L) <- detect color automatically
		int nPaneIdx
		) const;

	virtual bool OnQueryGripperEnabledState() const;
	virtual void OnPaintGripper(
		CDC & dc,
		const CRect & rcGrip
		);
	virtual void OnPaintSeparator(
		CDC & dc,
		int nPaneIdxAfterSep,
		const CRect & rcPaneBefore,
		const CRect & rcPaneAfter
		);

	struct __PROF_UIS_API _STATUSBAR_PANE_
	{
		UINT    nID;        // IDC of indicator: 0 => normal text area
		int     cxText;     // width of string area in pixels
		//   on both sides there is a 3 pixel gap and
		//   a one pixel border, making a pane 6 pixels wider
		UINT    nStyle;     // style flags (SBPS_*)
		UINT    nFlags;     // state flags (SBPF_*)
		CExtSafeString strText;    // text in the pane
		_STATUSBAR_PANE_()
		{
			nID = cxText = nStyle = nFlags = 0;
		}
	}; // struct _STATUSBAR_PANE_
	
	struct __PROF_UIS_API _STATUSBAR_PANE_CTRL_
	{
		HWND hWnd;
		UINT nID;
		bool bAutoDestroy;		
	}; // struct _STATUSBAR_PANE_CTRL_
	
	CArray < _STATUSBAR_PANE_CTRL_*, _STATUSBAR_PANE_CTRL_* > m_arrPaneControls; 
	
	_STATUSBAR_PANE_* GetPanePtr(int nIndex) const;
	_STATUSBAR_PANE_ * _GetPanePtr( int nIndex ) const;
	
	bool PaneInfoGet(
		int nIndex, 
		_STATUSBAR_PANE_* pPane
		) const;
	bool PaneInfoSet(
		int nIndex, 
		_STATUSBAR_PANE_* pPane
		);
	void RepositionControls( 
		bool bUpdate = true 
		);
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CExtStatusControlBar)
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	//}}AFX_MSG
	afx_msg LRESULT OnSizeParent( WPARAM wParam, LPARAM lParam );
	afx_msg void OnSysColorChange();
	afx_msg void OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection);
	afx_msg LRESULT OnDisplayChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnThemeChanged( WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
		
}; // class CExtStatusControlBar

#endif // __EXT_MFC_NO_STATUSBAR

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __EXT_STATUSCONTROLBAR_H
