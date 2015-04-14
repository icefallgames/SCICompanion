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

#if (!defined __EXT_SPIN_H)
#define __EXT_SPIN_H

#if (!defined __EXT_MFC_NO_SPIN)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_CONTROLBAR_H)
	#include <ExtControlBar.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtSpinWnd window

class __PROF_UIS_API CExtSpinWnd 
	: public CSpinButtonCtrl
	, public CExtAnimationSingleton
{
// Construction
public:
	DECLARE_DYNCREATE( CExtSpinWnd );
	CExtSpinWnd();

// Attributes
public:

// Operations
public:
	void SetBkColor( COLORREF clrBk );
	COLORREF GetBkColor() const;

	bool GetButtonRect( 
		CRect & rcButtonUp,
		CRect & rcButtonDown,
		const CRect * prcClient = NULL
		);
	
public:
	virtual void PmBridge_OnPaintManagerChanged(
		CExtPaintManager * pGlobalPM
		);
	virtual HWND AnimationSite_GetSafeHWND() const;
	virtual const CExtAnimationParameters *
		AnimationClient_OnQueryAnimationParameters(
			INT eAPT // __EAPT_*** animation type
			) const;
	virtual bool AnimationClient_CacheNextState(
		CDC & dc,
		const RECT & rcAcAnimationTarget,
		bool bAnimate,
		INT eAPT // __EAPT_*** animation type
		);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtSpinWnd)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtSpinWnd();

	virtual void OnSpinDrawEntire(
		CDC & dc,
		const CRect & rcClient
		);
	virtual void OnSpinEraseClientArea(
		CDC & dc,
		const CRect & rcClient
		);

protected:
	bool m_bCanceling:1;
	bool m_bButtonUpHovered:1;
	bool m_bButtonDownHovered:1;
	bool m_bButtonUpPressed:1;
	bool m_bButtonDownPressed:1;
	COLORREF m_clrBackground;
    bool m_bNcCalcSize:1;

	bool _IsBuddyToEdit();
	bool _IsEnabled();
	bool _IsReadOnly();

	virtual COLORREF OnQueryBkColor() const;
	
	virtual bool _ProcessMouseMove( 
		CPoint point 
		);
	virtual bool _ProcessMouseClick(
		CPoint point,
		bool bButtonPressed,
		INT nMouseButton // MK_... values
		);

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtSpinWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCancelMode();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
#if _MFC_VER < 0x700
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#endif
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // (!defined __EXT_MFC_NO_SPIN)

#endif // (!defined __EXT_SPIN_H)
