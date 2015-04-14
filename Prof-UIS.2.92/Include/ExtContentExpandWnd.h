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

#if (!defined __EXT_EXTCONTENTEXPANDWND_H)
#define __EXT_EXTCONTENTEXPANDWND_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_MOUSECAPTURESINK_H)
	#include <../Src/ExtMouseCaptureSink.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtContentExpandWnd window

#define __EXT_CONTENTEXPANDWND_CLASS_NAME _T("ProfUIS-ContentExpandWnd")

#define __ECWAF_PASS_WM_LBUTTONDOWN		0x00000001L
#define __ECWAF_PASS_WM_MBUTTONDOWN		0x00000002L
#define __ECWAF_PASS_WM_RBUTTONDOWN		0x00000004L
#define __ECWAF_PASS_WM_MOUSEWHEEL		0x00000008L
#define __ECWAF_PASS_WM_MOUSEMOVE		0x00000010L
#define __ECWAF_TRANSPARENT_WND			0x00000020L
#define __ECWAF_DRAW_ITEM				0x00000040L
#define __ECWAF_DRAW_RED_DOTS			0x00000080L
#define __ECWAF_DRAW_RED_ARROWS			0x00000100L
#define __ECWAF_DRAW_RED_ARROWS_V		0x00000200L // arrows are vertical
#define __ECWAF_POST_INVALIDATE			0x00000400L
#define __ECWAF_DRAW_SOURCE				0x00000800L
#define __ECWAF_TRANSPARENT_ITEM		0x00001000L
#define __ECWAF_NO_CAPTURE				0x00002000L
#define __ECWAF_PASS_WM_LBUTTONUP		0x00004000L
#define __ECWAF_PASS_WM_MBUTTONUP		0x00008000L
#define __ECWAF_PASS_WM_RBUTTONUP		0x00010000L
#define __ECWAF_PASS_WM_LBUTTONDBLCLK	0x00020000L
#define __ECWAF_PASS_WM_MBUTTONDBLCLK	0x00040000L
#define __ECWAF_PASS_WM_RBUTTONDBLCLK	0x00080000L
#define __ECWAF_REDIRECT_RETVAL			0x00100000L
#define __ECWAF_REDIRECT_NO_DEACTIVATE	0x00200000L
#define __ECWAF_REDIRECT_AND_HANDLE		0x00400000L
#define __ECWAF_HANDLE_MOUSE_ACTIVATE	0x00800000L
#define __ECWAF_MA_ACTIVATE				0x01000000L
#define __ECWAF_MA_ACTIVATEANDEAT		0x02000000L
#define __ECWAF_MA_NOACTIVATE			0x03000000L
#define __ECWAF_MA_NOACTIVATEANDEAT		0x04000000L
#define __ECWAF_MA_MASK					0x07000000L

#define __ECWAF_REDIRECT_MOUSE \
	(	__ECWAF_PASS_WM_MOUSEWHEEL \
	|	__ECWAF_PASS_WM_MOUSEMOVE \
	|	__ECWAF_PASS_WM_LBUTTONDOWN \
	|	__ECWAF_PASS_WM_MBUTTONDOWN \
	|	__ECWAF_PASS_WM_RBUTTONDOWN \
	|	__ECWAF_PASS_WM_LBUTTONUP \
	|	__ECWAF_PASS_WM_MBUTTONUP \
	|	__ECWAF_PASS_WM_RBUTTONUP \
	|	__ECWAF_PASS_WM_LBUTTONDBLCLK \
	|	__ECWAF_PASS_WM_MBUTTONDBLCLK \
	|	__ECWAF_PASS_WM_RBUTTONDBLCLK \
	)

#define __ECWAF_DRAW_ANY \
	(	__ECWAF_DRAW_ITEM \
	|	__ECWAF_DRAW_RED_DOTS \
	|	__ECWAF_DRAW_RED_ARROWS \
	|	__ECWAF_POST_INVALIDATE \
	|	__ECWAF_DRAW_SOURCE \
	)

#define __ECWAF_USER_DEFINED_04			0x08000000L
#define __ECWAF_USER_DEFINED_03			0x10000000L
#define __ECWAF_USER_DEFINED_02			0x20000000L
#define __ECWAF_USER_DEFINED_01			0x40000000L
#define __ECWAF_USER_DEFINED_00			0x80000000L

#define __ECWAF_DEF_EXPANDED_ITEM_PAINTER \
	(	__ECWAF_PASS_WM_LBUTTONDOWN \
	|	__ECWAF_PASS_WM_MBUTTONDOWN \
	|	__ECWAF_PASS_WM_RBUTTONDOWN \
	|	__ECWAF_PASS_WM_MOUSEWHEEL \
	|	__ECWAF_DRAW_ITEM \
	/*|	__ECWAF_DRAW_SOURCE*/ \
	)

#define __ECWAF_DEF_ARROWS_H \
	(	__ECWAF_PASS_WM_LBUTTONDOWN \
	|	__ECWAF_PASS_WM_MBUTTONDOWN \
	|	__ECWAF_PASS_WM_RBUTTONDOWN \
	|	__ECWAF_PASS_WM_MOUSEWHEEL \
	|	__ECWAF_DRAW_RED_ARROWS \
	)

#define __ECWAF_DEF_ARROWS_V \
	(	__ECWAF_PASS_WM_LBUTTONDOWN \
	|	__ECWAF_PASS_WM_MBUTTONDOWN \
	|	__ECWAF_PASS_WM_RBUTTONDOWN \
	|	__ECWAF_PASS_WM_MOUSEWHEEL \
	|	__ECWAF_DRAW_RED_ARROWS \
	|	__ECWAF_DRAW_RED_ARROWS_V \
	)

#define __ECWAF_DEFAULT __ECWAF_DEF_EXPANDED_ITEM_PAINTER

class __PROF_UIS_API CExtRedirectOleDropTarget : public COleDropTarget
{
	COleDropTarget * m_pOleDropTargetOther;
	CWnd * m_pOtherWnd;
	CWnd * m_pWndSrc;
public:
	CExtRedirectOleDropTarget(
		COleDropTarget * pOleDropTargetOther,
		CWnd * pOtherWnd,
		CWnd * pWndSrc
		)
		: m_pOleDropTargetOther( pOleDropTargetOther )
		, m_pOtherWnd( pOtherWnd )
		, m_pWndSrc( pWndSrc )
	{
		ASSERT( m_pOleDropTargetOther != NULL );
#ifdef _DEBUG
		if( m_pOtherWnd != NULL )
		{
			ASSERT( m_pOtherWnd->GetSafeHwnd() != NULL );
		}
#endif // _DEBUG
		ASSERT( m_pWndSrc != NULL );
		ASSERT( m_pWndSrc->GetSafeHwnd() != NULL );
		VERIFY( Register( m_pWndSrc ) );
	}
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point)
	{
		ASSERT( m_pOleDropTargetOther != NULL );
		if( m_pOtherWnd != NULL )
			pWnd = m_pOtherWnd;
		return
			m_pOleDropTargetOther->
				OnDragEnter(pWnd,pDataObject,dwKeyState,point);
	}
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point)
	{
		ASSERT( m_pOleDropTargetOther != NULL );
		if( m_pOtherWnd != NULL )
			pWnd = m_pOtherWnd;
		return
			m_pOleDropTargetOther->
				OnDragOver(pWnd,pDataObject,dwKeyState,point);
	}
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point)
	{
		ASSERT( m_pOleDropTargetOther != NULL );
		if( m_pOtherWnd != NULL )
			pWnd = m_pOtherWnd;
		return
			m_pOleDropTargetOther->
				OnDrop(pWnd,pDataObject,dropEffect,point);
	}
	virtual DROPEFFECT OnDropEx(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropDefault, DROPEFFECT dropList, CPoint point)
	{
		ASSERT( m_pOleDropTargetOther != NULL );
		if( m_pOtherWnd != NULL )
			pWnd = m_pOtherWnd;
		return
			m_pOleDropTargetOther->
				OnDropEx(pWnd,pDataObject,dropDefault,dropList,point);
	}
	virtual void OnDragLeave(CWnd* pWnd)
	{
		ASSERT( m_pOleDropTargetOther != NULL );
		if( m_pOtherWnd != NULL )
			pWnd = m_pOtherWnd;
		m_pOleDropTargetOther->OnDragLeave(pWnd);
	}
	virtual DROPEFFECT OnDragScroll(CWnd* pWnd, DWORD dwKeyState,
		CPoint point)
	{
		ASSERT( m_pOleDropTargetOther != NULL );
		if( m_pOtherWnd != NULL )
			pWnd = m_pOtherWnd;
		return
			m_pOleDropTargetOther->
				OnDragScroll(pWnd,dwKeyState,point);
	}
}; // class CExtRedirectOleDropTarget

class __PROF_UIS_API CExtContentExpandWnd
	: public CWnd
	, public CExtMouseCaptureSink
	, public CExtPmBridge
{

// Construction
public:
	DECLARE_DYNCREATE( CExtContentExpandWnd );
	DECLARE_CExtPmBridge_MEMBERS( CExtContentExpandWnd );

	CExtContentExpandWnd();

// Attributes
public:
	CExtShadowCtrl m_ctrlShadow;

	static UINT g_nMsgPaintItemContent;

	struct __PROF_UIS_API PAINT_ITEM_CONTENT_DATA
	{
		CDC & m_dc;
		CRect m_rcItem;
		PAINT_ITEM_CONTENT_DATA(
			CDC & dc,
			const RECT & rcItem
			)
			: m_dc( dc )
			, m_rcItem( rcItem )
		{
			ASSERT( m_dc.GetSafeHdc() != NULL );
		}

	private:
		bool Notify( HWND hWndNotify )
		{
			ASSERT( hWndNotify != NULL );
			ASSERT( ::IsWindow(hWndNotify) );
			ASSERT( m_dc.GetSafeHdc() != NULL );
			if(		m_rcItem.right <= m_rcItem.left
				||	m_rcItem.bottom <= m_rcItem.top
				)
				return true;
			if( !m_dc.RectVisible( &m_rcItem ) )
				return true;
			LRESULT lResult =
				::SendMessage(
					hWndNotify,
					CExtContentExpandWnd::g_nMsgPaintItemContent,
					(WPARAM)(this),
					0
					);
			return lResult ? true : false;
		}
	
		friend class CExtContentExpandWnd;
	}; // struct PAINT_ITEM_CONTENT_DATA

protected:
	HFONT m_hFont;
	static bool g_bContentExpandWndClassRegistered;
	bool m_bDirectCreateCall:1;
	bool m_bVerticalMode:1;
	CRect m_rcWnd;
	CRect m_rcItem;
	CWnd * m_pWndNotify;
	DWORD m_dwActivateFlags;
	CRgn m_rgnHelperAreas, m_rgnWindow, m_rgnItem;
	CBrush m_brHelperAreas;
	CExtRedirectOleDropTarget * m_pRODT;

// Operations
public:
	bool Activate(
		const RECT & rcItem,
		CWnd * pWndNotify,
		DWORD dwActivateFlags = __ECWAF_DEFAULT,
		COleDropTarget * pODT = NULL,
		CWnd * pWndOtherForODT = NULL
		);
	void Deactivate();
	void RecalcLayout();

protected:
	bool RegisterContentExpandWndClass();
	bool _CreateHelper();
	void _InitMembers();
	void _FlushOleRedir();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExtContentExpandWnd)
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CExtContentExpandWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CExtContentExpandWnd)
	afx_msg UINT OnNcHitTest(CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg LRESULT OnSetFont( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetFont( WPARAM, LPARAM );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
}; // class CExtContentExpandWnd

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // __EXT_EXTCONTENTEXPANDWND_H
