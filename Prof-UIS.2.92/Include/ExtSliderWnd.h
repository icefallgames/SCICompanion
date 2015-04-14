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

#if (!defined __EXT_SLIDER_H)
#define __EXT_SLIDER_H

#if (!defined __EXT_MFC_NO_SLIDER)

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CExtPopupMenuWnd;
class CExtPopupMenuTipWnd;

/////////////////////////////////////////////////////////////////////////////
// CExtSliderWnd window

class __PROF_UIS_API CExtSliderWnd
	: public CSliderCtrl
	, public CExtPmBridge
{
// Construction
public:
	DECLARE_DYNCREATE( CExtSliderWnd );		
	DECLARE_CExtPmBridge_MEMBERS( CExtSliderWnd );

	CExtSliderWnd();

// Attributes
public:
	enum e_style_t
	{
		ES_SYSTEM	= 0,
		ES_PROFUIS	= 1,
	};

protected:
	COLORREF m_clrBackground;
	COLORREF m_clrBackgroundPrev;
	CBrush m_brBack;
	e_style_t m_eStyle;

// Operations
public:
	INT m_nAdvancedTipStyle; // CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON
	void SetTooltipText(
		int nId,
		BOOL bActivate = TRUE
		);
	void SetTooltipText(
		__EXT_MFC_SAFE_LPCTSTR sText,
		BOOL bActivate = TRUE
		);
	void ActivateTooltip(
		BOOL bEnable = TRUE
		);
	virtual int OnQueryMaxTipWidth( 
		__EXT_MFC_SAFE_LPCTSTR lpszText 
		);
	virtual CExtPopupMenuTipWnd * OnAdvancedPopupMenuTipWndGet() const;
	virtual void OnAdvancedPopupMenuTipWndDisplay(
		CExtPopupMenuTipWnd & _ATTW,
		const RECT & rcExcludeArea
		) const;
protected:
	CExtSafeString m_strToolTipText;
	bool m_bToolTipActive:1;
	CToolTipCtrl m_wndToolTip;
	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		) const;
public:
	void InitToolTip();

	void SetStyle( e_style_t eStyle );
	e_style_t GetStyle() const;

	COLORREF SetBkColor( COLORREF clrBk );
	COLORREF GetBkColor() const;

	void UpdateSliderWnd();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtSliderWnd)
	public:
	virtual BOOL PreTranslateMessage( MSG * pMsg );
	protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtSliderWnd();

protected:
	virtual bool OnSliderDrawChannel(
		CDC & dc,
		const CRect & rcChannel,
		UINT uItemState
		);
	virtual bool OnSliderDrawThumb(
		CDC & dc,
		const CRect & rcThumb,
		UINT uItemState
		);
	virtual bool OnSliderDrawTics(
		CDC & dc,
		const CRect & rcClient,
		UINT uItemState
		);
	virtual void PmBridge_OnPaintManagerChanged(
		CExtPaintManager * pGlobalPM
		);

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtSliderWnd)
	afx_msg void OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	//}}AFX_MSG
	afx_msg void OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection);
	afx_msg void OnSysColorChange();
	afx_msg LRESULT OnDisplayChange( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnThemeChanged( WPARAM wParam, LPARAM lParam );

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // (!defined __EXT_MFC_NO_SLIDER)

#endif // !defined(__EXT_SLIDER_H)
