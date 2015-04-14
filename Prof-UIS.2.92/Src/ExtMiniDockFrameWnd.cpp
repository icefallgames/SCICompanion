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

#include "stdafx.h"

#if( !defined __EXTMINIDOCKFRAMEWND_H)
	#include "ExtMiniDockFrameWnd.h"
#endif

#if (!defined __EXTDOCKBAR_H)
	#include "ExtDockBar.h"
#endif

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	#if (!defined __EXT_CONTROLBAR_TABBED_FEATURES_H)
		#include "ExtControlBarTabbedFeatures.h"
	#endif // __EXT_CONTROLBAR_TABBED_FEATURES_H
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if _MFC_VER < 0x700
	#include <../src/AfxImpl.h>
#else
	#ifndef __AFXSTATE_H__
		#include <../src/mfc/afxstat_.h>
	#endif
	#include <../src/mfc/AfxImpl.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define __REMOVED_NONFIXMODE_STYLES \
	FWS_SNAPTOBARS

#define __REMOVED_STYLES_NORMAL \
	(MFS_4THICKFRAME|WS_THICKFRAME|WS_CAPTION|WS_BORDER|WS_DLGFRAME)
//|WS_SYSMENU

#define __REMOVED_STYLES_EXTENDED \
	(WS_EX_STATICEDGE|WS_EX_WINDOWEDGE|WS_EX_DLGMODALFRAME)
//	(WS_EX_TOOLWINDOW|WS_EX_STATICEDGE|WS_EX_WINDOWEDGE|WS_EX_DLGMODALFRAME)

#define __SWP_FOR_REMOVED_STYLES \
	(SWP_FRAMECHANGED|SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSIZE|SWP_NOMOVE|SWP_NOSENDCHANGING|SWP_NOREDRAW)

#define __cSM_CMONITORS            80
#define __cSM_CXVIRTUALSCREEN      78
#define __cSM_CYVIRTUALSCREEN      79
#define __cSM_XVIRTUALSCREEN       76
#define __cSM_YVIRTUALSCREEN       77

/////////////////////////////////////////////////////////////////////////////
// CExtMiniDockFrameWnd

bool CExtMiniDockFrameWnd::g_bOnWndPosChgProcessing = false;
bool CExtMiniDockFrameWnd::g_bAutoMakeTopmostOnMouseEvent = false;

IMPLEMENT_DYNCREATE( CExtMiniDockFrameWnd, CMiniDockFrameWnd );
IMPLEMENT_CExtPmBridge_MEMBERS( CExtMiniDockFrameWnd );

CExtMiniDockFrameWnd::CExtMiniDockFrameWnd()
	: m_bMovingLargeBar( false )
	, m_bHelperNcActive( false )
	, m_bLockRecalcLayout( false )
{
	m_rcBtnHideBar.SetRectEmpty();
	m_rcFrameCaption.SetRectEmpty();
	m_bBtnHideBar = false;
	m_nResizingMode = HTNOWHERE;
	m_ptLastResizing.x = m_ptLastResizing.y = -32767;
	m_ptResizingShift.x = m_ptResizingShift.y = 0;
	RegisterMiniDockFrameWndClass();
	PmBridge_Install();
}

CExtMiniDockFrameWnd::~CExtMiniDockFrameWnd()
{
	PmBridge_Uninstall();
}

void CExtMiniDockFrameWnd::PmBridge_OnPaintManagerChanged(
	CExtPaintManager * pGlobalPM
	)
{
	CExtPmBridge::PmBridge_OnPaintManagerChanged( pGlobalPM );
	if( GetSafeHwnd() == NULL )
		return;
	::SetWindowPos(
		m_hWnd, NULL, 0, 0, 0, 0,
		SWP_FRAMECHANGED
			|SWP_NOACTIVATE
			|SWP_NOZORDER
			|SWP_NOOWNERZORDER
			|SWP_NOMOVE
			|SWP_NOSIZE
		);
	::RedrawWindow(
		m_hWnd, NULL, NULL,
		RDW_INVALIDATE|RDW_ERASE|RDW_FRAME|RDW_ALLCHILDREN
		);
}

BEGIN_MESSAGE_MAP(CExtMiniDockFrameWnd, CMiniDockFrameWnd)
    //{{AFX_MSG_MAP(CExtMiniDockFrameWnd)
	ON_WM_NCMOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_NCPAINT()
	ON_WM_NCACTIVATE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCRBUTTONDOWN()
    ON_WM_GETMINMAXINFO()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_CANCELMODE()
	ON_WM_PAINT()
	ON_WM_NCCALCSIZE()
	//}}AFX_MSG_MAP
	__EXT_MFC_ON_WM_NCHITTEST()
	ON_WM_SYSCOLORCHANGE()
	__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_MESSAGE(__ExtMfc_WM_THEMECHANGED, OnThemeChanged)
END_MESSAGE_MAP()

void CExtMiniDockFrameWnd::OnSysColorChange() 
{
	ASSERT_VALID( this );
	CMiniDockFrameWnd::OnSysColorChange();
//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnSysColorChange( this );
//	g_CmdManager.OnSysColorChange( pPM, this );
	Invalidate();
}

LRESULT CExtMiniDockFrameWnd::OnDisplayChange( WPARAM wParam, LPARAM lParam )
{
//LRESULT lResult = CMiniDockFrameWnd::OnDisplayChange( wParam, lParam );

	CMiniDockFrameWnd::OnDisplayChange(wParam, LOWORD(lParam), HIWORD(lParam));

LRESULT lResult = 0;


//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnDisplayChange( this, (INT)wParam, CPoint(lParam) );
//	g_CmdManager.OnDisplayChange( pPM, this, (INT)wParam, CPoint(lParam) );
	Invalidate();
	return lResult;
}

LRESULT CExtMiniDockFrameWnd::OnThemeChanged( WPARAM wParam, LPARAM lParam )
{
LRESULT lResult = Default();
CExtPaintManager * pPM = PmBridge_GetPM();
	g_PaintManager.OnThemeChanged( this, wParam, lParam );
	g_CmdManager.OnThemeChanged( pPM, this, wParam, lParam );
	::SetWindowPos(
		m_hWnd, NULL, 0, 0, 0, 0,
		SWP_FRAMECHANGED
			|SWP_NOACTIVATE
			|SWP_NOZORDER
			|SWP_NOOWNERZORDER
			|SWP_NOMOVE
			|SWP_NOSIZE
		);
	RecalcLayout();
CExtControlBar * pExtBar = GetControlBarExt( false );
	if( pExtBar != NULL )
	{
		VERIFY(
			PmBridge_GetPM()->Bar_MiniDockFrameCaptionCalcRect(
				m_rcFrameCaption,
				this,
				pExtBar
				) 
			);
		SetTimer( 1, 1, NULL );
	} // if( pExtBar != NULL )
	return lResult;
}

void CExtMiniDockFrameWnd::OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection) 
{
	ASSERT_VALID( this );
	CMiniDockFrameWnd::OnSettingChange( uFlags, lpszSection );
//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnSettingChange( this, uFlags, lpszSection );
//	g_CmdManager.OnSettingChange( pPM, this, uFlags, lpszSection );
	::SetWindowPos(
		m_hWnd, NULL, 0, 0, 0, 0,
		SWP_FRAMECHANGED
			|SWP_NOACTIVATE
			|SWP_NOZORDER
			|SWP_NOOWNERZORDER
			|SWP_NOMOVE
			|SWP_NOSIZE
		);
	RecalcLayout();
CExtControlBar * pExtBar = GetControlBarExt( false );
	if( pExtBar != NULL )
	{
		VERIFY(
			PmBridge_GetPM()->Bar_MiniDockFrameCaptionCalcRect(
				m_rcFrameCaption,
				this,
				pExtBar
				) 
			);
		SetTimer( 1, 1, NULL );
	} // if( pExtBar != NULL )
}

BOOL CExtMiniDockFrameWnd::Create(CWnd* pParent, DWORD dwBarStyle)
{
//	if(!MDFW_Create(pParent, dwBarStyle))
//		return FALSE;
	if( ! CMiniDockFrameWnd::Create(
			pParent,
			dwBarStyle
			)
		)
		return FALSE;

	ModifyStyle( 0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS );
	m_wndDockBar.ModifyStyle( 0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS );

	if( ( (m_wndDockBar.GetExStyle()&(WS_EX_LAYOUTRTL|WS_EX_RTLREADING)) != 0 ) )
	{
		m_wndDockBar.ModifyStyleEx(
			WS_EX_LAYOUTRTL|WS_EX_RTLREADING,
			0,
			SWP_FRAMECHANGED
			);
		ASSERT( (m_wndDockBar.GetExStyle()&(WS_EX_LAYOUTRTL|WS_EX_RTLREADING)) == 0 );
	} // if( ( (m_wndDockBar.GetExStyle()&(WS_EX_LAYOUTRTL|WS_EX_RTLREADING)) != 0 ) )

	if( GetStyle() & __REMOVED_STYLES_NORMAL )
		ModifyStyle( __REMOVED_STYLES_NORMAL, 0, __SWP_FOR_REMOVED_STYLES );
	if( GetExStyle() & __REMOVED_STYLES_EXTENDED )
		ModifyStyleEx( __REMOVED_STYLES_EXTENDED, 0, __SWP_FOR_REMOVED_STYLES );
	
	if( ( (CWnd::GetExStyle()&(WS_EX_LAYOUTRTL|WS_EX_RTLREADING)) != 0 ) )
	{
		CWnd::ModifyStyleEx(
			WS_EX_LAYOUTRTL|WS_EX_RTLREADING,
			0,
			SWP_FRAMECHANGED
			);
		ASSERT( (CWnd::GetExStyle()&(WS_EX_LAYOUTRTL|WS_EX_RTLREADING)) == 0 );
	} // if( ( (CWnd::GetExStyle()&(WS_EX_LAYOUTRTL|WS_EX_RTLREADING)) != 0 ) )

	SetTimer( 1, 1, NULL );
	EnableToolTips();
	m_wndToolTip.Create( this );
	m_wndToolTip.Activate( TRUE );

	return TRUE;
}

void CExtMiniDockFrameWnd::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
	if( _ResizingIsInProgress() )
	{
		_ResizingEnd();
		return;
	}
	
	if( CExtControlBar::_DraggingGetBar() != NULL )
		return;

CRect rcBarWnd;
	GetWindowRect( &rcBarWnd );
CPoint ptDevOffset = -rcBarWnd.TopLeft();
CPoint ptWnd( point + ptDevOffset );

CExtControlBar * pExtBar = GetControlBarExt( false );
	if( pExtBar != NULL && pExtBar->m_bNcAreaBtnTracking )
	{
		INT nCountOfNcButtons = pExtBar->NcButtons_GetCount();
		for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
		{
			CExtBarNcAreaButton * pBtn =
				pExtBar->NcButtons_GetAt( nBtn );
			if( !pBtn->OnQueryVisibility() )
				continue;
			if( pBtn->OnNcAreaClicked( ptWnd ) )
				return;
		} // for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
		return;
	} // if( pExtBar != NULL && pExtBar->m_bNcAreaBtnTracking )

	CMiniDockFrameWnd::OnNcLButtonUp(nHitTest, point);
}

void CExtMiniDockFrameWnd::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		pSite->DropTargetPopupCancel();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtControlBar * pExtBar = GetControlBarExt( false );
CPoint p(point);
CRect rcWnd;
	GetWindowRect( &rcWnd );
	p.Offset( -rcWnd.left, -rcWnd.top );
	if( m_rcBtnHideBar.PtInRect( p ) )
		return;
	if( nHitTest == HTCAPTION )
	{
		pExtBar = GetControlBarExt( true );
		ActivateTopParent();
		if(		pExtBar != NULL
			&&	(! pExtBar->_IsDockSiteCustomizeMode() )
			)
			BringWindowToTop();
		if( (m_wndDockBar.m_dwStyle & CBRS_FLOAT_MULTI) == 0 )
		{
			int nPos = 1;
			CControlBar * pBar = NULL;
			while( pBar == NULL && nPos < m_wndDockBar.m_arrBars.GetSize() )
				pBar = reinterpret_cast < CExtDockBar & > (m_wndDockBar).GetDockedControlBar(nPos++);
			ASSERT( pBar != NULL );
			ASSERT_KINDOF( CControlBar, pBar );
			ASSERT( pBar->m_pDockContext != NULL );
			if(		pExtBar == NULL
				||	(	pExtBar != NULL
					&&	pExtBar->IsFixedMode()
					&&	(! pExtBar->_IsShowContentWhenDragging() )
					)
				)
			{
				pBar->m_pDockContext->StartDrag(point);
				return;
			}
			CRect rcGrip = pExtBar->_RectGripGet();
			int nCyGripper = min( rcGrip.Width(), rcGrip.Height() );
			pExtBar->_DraggingStart(
				point,
				point
					- rcWnd.TopLeft()
					- CPoint( nCyGripper, nCyGripper/2 )
				);
			return;
		}
	}
	else if( nHitTest >= HTSIZEFIRST && nHitTest <= HTSIZELAST )
	{
		ActivateTopParent();
		int nPos = 1;
		CControlBar * pBar = NULL;
		while(
			pBar == NULL
			&& nPos < m_wndDockBar.m_arrBars.GetSize()
			)
			pBar =
				reinterpret_cast < CExtDockBar & >
					(m_wndDockBar).GetDockedControlBar(nPos++);
		ASSERT( pBar != NULL );
		ASSERT_KINDOF( CControlBar, pBar );
		ASSERT( pBar->m_pDockContext != NULL );
		ASSERT( (m_wndDockBar.m_dwStyle & CBRS_FLOAT_MULTI) == 0 );
		if(	! pBar->IsKindOf( RUNTIME_CLASS(CExtControlBar) ) )
			CMiniDockFrameWnd::OnNcLButtonDown( nHitTest, point);
		else
		{
			CExtControlBar * pExtBar =
				(CExtControlBar *)pBar;
			if( !pExtBar->_IsShowContentWhenDragging()
				&&	pExtBar->IsFixedMode()
				)
			{
				pBar->m_pDockContext->StartResize( nHitTest, point );
				return;
			}
			ClientToScreen( &point );
			_ResizingStart( nHitTest, point );
		}
		return;
	}
	else if( nHitTest == HTMINBUTTON
		|| nHitTest == HTMAXBUTTON
		|| nHitTest == HTGROWBOX
		)
		return;
	
	CMiniFrameWnd::OnNcLButtonDown( nHitTest, point );
}

void CExtMiniDockFrameWnd::OnNcLButtonDblClk(UINT nHitTest, CPoint point) 
{
	if( nHitTest == HTCAPTION )
	{
		CRect rcWnd;
		GetWindowRect( &rcWnd );
		CPoint ptTestBtnHideBar( point - rcWnd.TopLeft() );
		if( m_rcBtnHideBar.PtInRect(ptTestBtnHideBar) )
			return;
	} // if( nHitTest == HTCAPTION )

CExtControlBar * pExtBar = GetControlBarExt( false );
	if( pExtBar != NULL )
	{
		ASSERT_VALID( pExtBar );
		if( ! pExtBar->_FloatStateChangingIsEnabled( pExtBar ) )
			return;
		pExtBar->ToggleDocking();
		return;
	}

	CMiniDockFrameWnd::OnNcLButtonDblClk(nHitTest, point);
}

void CExtMiniDockFrameWnd::_ResizingStart(
	UINT nHitTest,
	const CPoint & point
	)
{
	ASSERT( m_nResizingMode == HTNOWHERE );
	ASSERT( IsWindowVisible() );

CPoint pointCurrentCursorPos = point;
	ScreenToClient( &pointCurrentCursorPos );

	CExtPopupMenuWnd::CancelMenuTracking();
	CWnd::CancelToolTips();

CExtControlBar * pExtBar = GetControlBarExt( true );
	ASSERT_VALID( pExtBar );
	if( ! pExtBar->_FloatStateChangingIsEnabled( pExtBar ) )
		return;

	if(		( ! pExtBar->_IsShowContentWhenDragging() )
		&&	pExtBar->IsFixedMode()
		)
	{
		m_nResizingMode = HTNOWHERE;
		pExtBar->m_pDockContext->StartResize(nHitTest, point);
		return;
	}

	switch( nHitTest )
	{
	case HTLEFT:
	case HTRIGHT:
	case HTTOP:
	case HTTOPLEFT:
	case HTTOPRIGHT:
	case HTBOTTOM:
	case HTBOTTOMLEFT:
	case HTBOTTOMRIGHT:
		m_nResizingMode = nHitTest;
	break;
	default:
		return;
	} // switch( nHitTest )

#ifdef _DEBUG
int nPos = 1;
CControlBar * pDebugDummyBar = NULL;
	while(
		pDebugDummyBar == NULL
		&& nPos < m_wndDockBar.m_arrBars.GetSize()
		)
		pDebugDummyBar =
			reinterpret_cast < CExtDockBar & >
				( m_wndDockBar ) . GetDockedControlBar( nPos++ );
	ASSERT( pDebugDummyBar != NULL );
	ASSERT_KINDOF( CExtControlBar, pDebugDummyBar );
	ASSERT( pDebugDummyBar->m_pDockContext != NULL );
	// CBRS_SIZE_DYNAMIC toolbars cannot have the CBRS_FLOAT_MULTI style
	ASSERT( (m_wndDockBar.m_dwStyle & CBRS_FLOAT_MULTI) == 0 );
#endif // _DEBUG
	
	ASSERT( pExtBar == pDebugDummyBar );
	ASSERT( pExtBar->m_pDockContext != NULL );

	if( ! pExtBar->IsFixedMode() )
		ModifyStyle( __REMOVED_NONFIXMODE_STYLES, 0 );
	BringWindowToTop();
	CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
	GetWindowRect( &m_rcWndResizingStart );

HWND hWndExtBar = pExtBar->m_hWnd;
	pExtBar->OnControlBarPositionChange(
		pExtBar,
		CExtControlBar::__ECBPC_FLOATING_FRAME_RESIZING,
		true,
		false
		);

	_ResizingUpdateState( pointCurrentCursorPos );
	pExtBar->OnControlBarPositionChange(
		pExtBar,
		CExtControlBar::__ECBPC_FLOATING_FRAME_RESIZING,
		false,
		false
		);

	while( m_nResizingMode != HTNOWHERE )
	{
		::WaitMessage();
		MSG msg;
		// Process all the messages in the message queue
		while( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			bool bStop = false;
			switch( msg.message )
			{
			case WM_ACTIVATEAPP:
			case WM_COMMAND:
				bStop = true;
			break;
			default:
				if(	WM_KEYFIRST <= msg.message
					&& msg.message <= WM_KEYLAST
					)
				bStop = true;
			break;
			case WM_MOUSEMOVE:
				if( _ResizingIsInProgress() )
				{
					PeekMessage(&msg, NULL, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE);
					for( ; PeekMessage(&msg, NULL, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_NOREMOVE); )
						PeekMessage(&msg, NULL, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE);
					if( ::GetCursorPos( &pointCurrentCursorPos ) )
					{
						_ResizingUpdateState( pointCurrentCursorPos );
						pExtBar->OnControlBarPositionChange(
							pExtBar,
							CExtControlBar::__ECBPC_FLOATING_FRAME_RESIZING,
							false,
							false
							);
					}
					continue;
				} // if( _ResizingIsInProgress() )
				break;
			} // switch( msg.message )
			if( (!bStop) &&
				CExtMouseCaptureSink::GetCapture() != GetSafeHwnd()
				)
				bStop = true;
			if( bStop )
			{
				_ResizingEnd();
				if( ::IsWindow( hWndExtBar ) )
					pExtBar->OnControlBarPositionChange(
						pExtBar,
						CExtControlBar::__ECBPC_FLOATING_FRAME_RESIZING,
						false,
						true
						);
				return;
			}
			if( !AfxGetThread()->PumpMessage() )
			{
				PostQuitMessage(0);
				return; // Signal WM_QUIT received
			} // if( !AfxGetThread()->PumpMessage() )
		} // while( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		
		if( CExtControlBar::g_bEnableOnIdleCalls )
		{
			for(	LONG nIdleCounter = 0L;
					::AfxGetThread()->OnIdle(nIdleCounter);
					nIdleCounter ++
					);
		}
	} // while( m_nResizingMode != HTNOWHERE )

	if( ::IsWindow( hWndExtBar ) )
		pExtBar->OnControlBarPositionChange(
			pExtBar,
			CExtControlBar::__ECBPC_FLOATING_FRAME_RESIZING,
			false,
			true
			);
}

void CExtMiniDockFrameWnd::_ResizingEnd()
{
	m_ptLastResizing.x = m_ptLastResizing.y = -32767;
	m_ptResizingShift.x = m_ptResizingShift.y = 0;
	//ASSERT( m_nResizingMode != HTNOWHERE );
	if( m_nResizingMode == HTNOWHERE )
		return;
	if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
		CExtMouseCaptureSink::ReleaseCapture();
CExtControlBar * pExtBar = GetControlBarExt( true );
	ASSERT_VALID( pExtBar );
	if( !pExtBar->IsFixedMode() )
		ModifyStyle( 0, __REMOVED_NONFIXMODE_STYLES );
	m_nResizingMode = HTNOWHERE;
}

bool CExtMiniDockFrameWnd::g_bInResizingUpdateState = false;

void CExtMiniDockFrameWnd::_ResizingUpdateState(
	const CPoint & point
	)
{
	if(		g_bInResizingUpdateState
		||	m_ptLastResizing == point
		)
		return;
CPoint _pt( point );
	ASSERT( m_nResizingMode != HTNOWHERE );
CRect rcWnd, rcClient;
	GetWindowRect( &rcWnd );
	GetClientRect( &rcClient );
CExtControlBar * pExtBar = GetControlBarExt( true );
	ASSERT_VALID( pExtBar );
bool bFixedMode = pExtBar->IsFixedMode();

	if( m_ptLastResizing.x == -32767 && m_ptLastResizing.y == -32767 )
	{ // adjust "point" parameter by border location
		CRect rcClientScreen = rcClient;
		ClientToScreen( &rcClientScreen );
		m_ptResizingShift.x = m_ptResizingShift.y = 0;
		bool bAdjustTop = false, bAdjustBottom = false, bAdjustLeft = false, bAdjustRight = false;
		switch( m_nResizingMode )
		{
		case HTTOP:
			bAdjustTop = true;
		break;
		case HTBOTTOM:
			bAdjustBottom = true;
		break;
		case HTLEFT:
			bAdjustLeft = true;
		break;
		case HTRIGHT:
			bAdjustRight = true;
		break;
		case HTTOPLEFT:
			bAdjustTop = true;
			bAdjustLeft = true;
		break;
		case HTTOPRIGHT:
			bAdjustTop = true;
			bAdjustRight = true;
		break;
		case HTBOTTOMLEFT:
			bAdjustBottom = true;
			bAdjustLeft = true;
		break;
		case HTBOTTOMRIGHT:
			bAdjustBottom = true;
			bAdjustRight = true;
		break;
		} // switch( m_nResizingMode )
		if( bAdjustTop )
		{
			m_ptResizingShift.y += _pt.y - rcWnd.top;
			if( bFixedMode )
				m_ptResizingShift.y += rcClientScreen.top - rcWnd.top;
		}
		if( bAdjustBottom )
		{
			m_ptResizingShift.y += rcWnd.bottom - _pt.y;
			if( bFixedMode )
				m_ptResizingShift.y -= rcWnd.bottom - rcClientScreen.bottom;
		}
		if( bAdjustLeft )
		{
			m_ptResizingShift.x += _pt.x - rcWnd.left;
			if( bFixedMode )
				m_ptResizingShift.y += rcClientScreen.left - rcWnd.left;
		}
		if( bAdjustRight )
		{
			m_ptResizingShift.x += rcWnd.right - _pt.x;
			if( bFixedMode )
				m_ptResizingShift.y += rcWnd.right - rcClientScreen.right;
		}
	} // adjust "point" parameter by border location
	_pt += m_ptResizingShift;

//TRACE2( "m_ptResizingShift = %d, %d\r\n", m_ptResizingShift.x, m_ptResizingShift.y );

#ifdef __FREIWALD_ORIGINAL__
	m_ptLastResizing = _pt;
#else

	// store the coordinates of "point" here,
	// because "point is the base value for the above comparation

	m_ptLastResizing = point;
#endif 

	g_bInResizingUpdateState = true;

CRect rcClientOrg, rcWndOrg;
	rcClientOrg = rcClient;
	rcWndOrg = rcWnd;
CSize sizeDiff =
		rcWnd.Size() - rcClient.Size();
	ASSERT( sizeDiff.cx >= 0 && sizeDiff.cy >= 0 );
CSize sizeNcDiff( sizeDiff );

int nCalcMinHW = pExtBar->_CalcDesiredMinHW();
int nCalcMinVH = pExtBar->_CalcDesiredMinVH();
	if( sizeDiff.cx < nCalcMinHW )
		sizeDiff.cx = nCalcMinHW;
	if( sizeDiff.cy < nCalcMinVH )
		sizeDiff.cy = nCalcMinVH;

UINT nHit = m_nResizingMode;
bool	bLeft = false, bTop = false, bHorz = false,
		bInvTop = false, bInvLeft = false;
CPoint ptOffsetWnd( 0, 0 );
	if( bFixedMode )
	{
		switch( nHit )
		{
		case HTTOPLEFT:
		case HTTOPRIGHT:
			nHit = HTTOP;
		break;
		case HTBOTTOMLEFT:
		case HTBOTTOMRIGHT:
			nHit = HTBOTTOM;
		break;
		} // switch( nHit )
		switch( nHit )
		{
		case HTTOP:
			_pt.y =
				rcWndOrg.bottom
				+ rcWndOrg.top - _pt.y;
			nHit = HTBOTTOM;
			bInvLeft = true;
			bInvTop = true;
		break;
		case HTLEFT:
			_pt.x =
				rcWndOrg.right
				+ rcWndOrg.left - _pt.x;
			nHit = HTRIGHT;
			bInvLeft = true;
			bInvTop = true;
		break;
		} // switch( nHit )
	} // if( bFixedMode )
	if( bInvLeft || bInvTop || (! bFixedMode ) )
	{
		CRect rcWnd( rcWndOrg );
		ScreenToClient( & rcWnd );
		if( bInvLeft || (! bFixedMode ) )
			ptOffsetWnd.x =
				abs( rcWnd.left - rcClientOrg.left );
		if( bInvTop || (! bFixedMode ) )
			ptOffsetWnd.y =
				abs( rcWnd.top - rcClientOrg.top );
	} // if( bInvLeft || bInvTop )

	switch( nHit )
	{
	case HTLEFT:
		bHorz = true;
		rcWnd.left = min( _pt.x, rcWnd.right );
		bLeft = true;
	break;
	case HTRIGHT:
		bHorz = true;
		rcWnd.right = max( _pt.x, rcWnd.left );
	break;
	case HTTOP:
		rcWnd.top = min( _pt.y, rcWnd.bottom );
		bTop = true;
	break;
	case HTTOPLEFT:
		rcWnd.left = min( _pt.x, rcWnd.right );
		rcWnd.top = min( _pt.y, rcWnd.bottom );
		bLeft = true;
		bTop = true;
	break;
	case HTTOPRIGHT:
		rcWnd.right = max( _pt.x, rcWnd.left );
		rcWnd.top = min( _pt.y, rcWnd.bottom );
		bTop = true;
	break;
	case HTBOTTOM:
		rcWnd.bottom = max( _pt.y, rcWnd.top );
	break;
	case HTBOTTOMLEFT:
		rcWnd.left = min( _pt.x, rcWnd.right );
		rcWnd.bottom = max( _pt.y, rcWnd.top );
		bLeft = true;
	break;
	case HTBOTTOMRIGHT:
		rcWnd.right = max( _pt.x, rcWnd.left );
		rcWnd.bottom = max( _pt.y, rcWnd.top );
	break;
	default:
		ASSERT( FALSE );
		g_bInResizingUpdateState = false;
		return;
	} // switch( nHit )

	if( rcWnd.Width() < sizeDiff.cx )
		rcWnd.right = rcWnd.left + sizeDiff.cx;
	if( rcWnd.Height() < sizeDiff.cy )
		rcWnd.bottom = rcWnd.top + sizeDiff.cy;
	if( rcWnd.right > rcWndOrg.right
		&& bLeft
		)
		rcWnd.OffsetRect(
			rcWndOrg.right - rcWnd.right,
			0
			);
	if( rcWnd.bottom > rcWndOrg.bottom
		&& bTop
		)
		rcWnd.OffsetRect(
			0,
			rcWndOrg.bottom - rcWnd.bottom
			);
	rcClient.SetRect(
		rcWnd.left - (rcClient.left-rcWndOrg.left),
		rcWnd.top - (rcClient.top-rcWndOrg.top),
		rcWnd.right - (rcClient.right-rcWndOrg.right),
		rcWnd.bottom - (rcClient.bottom-rcWndOrg.bottom)
		);

	if( ! bFixedMode )
	{
		ASSERT( (GetStyle() & __REMOVED_NONFIXMODE_STYLES) == 0 );
		CSize _sizeMinFloated =
			pExtBar->_CalcDesiredMinFloatedSize();
		CSize _size = rcWnd.Size();
		if( _size.cx < _sizeMinFloated.cx )
			_size.cx = _sizeMinFloated.cx;
		if( _size.cy < _sizeMinFloated.cy )
			_size.cy = _sizeMinFloated.cy;
		CExtPaintManager::monitor_parms_t _mp;
		CExtPaintManager::stat_GetMonitorParms( _mp, this );
		CSize _sizeMaxFloated = _mp.m_rcWorkArea.Size();

#ifdef __FREIWALD_ORIGINAL__
#else

	// It should be possible to enlarge the size of a floating frame to cover two or more monitors
	// in a multiple monitor configuration.

	// The original implementation, however, adjusted the dimensions of the floating frame according to the
	// dimensions of the primary monitor.

	// This does not only prevent the floating frame from being enlarged to more than one monitor,
	// but may also cause wrong calculations, when the frame is resized on the non-primary monitor and
	// the dimensions / screen resolution of this non-primary monitor are different to those of the primary monitor.

	// This has been adjusted with the following change:

		if (GetSystemMetrics(__cSM_CMONITORS) > 1)
		{
			_sizeMaxFloated = CSize(GetSystemMetrics(__cSM_CXVIRTUALSCREEN), GetSystemMetrics(__cSM_CYVIRTUALSCREEN));
		}

#endif

		if( _size.cx > _sizeMaxFloated.cx )
			_size.cx = _sizeMaxFloated.cx;
		if( _size.cy > _sizeMaxFloated.cy )
			_size.cy = _sizeMaxFloated.cy;
//CRect rc1, rc2;
//GetClientRect( &rc1 );
//pExtBar->GetClientRect( &rc2 );
//CSize sizeDiff2 = rc1.Size() - rc2.Size();
//_size += sizeDiff2;

#ifdef __FREIWALD_ORIGINAL__
		pExtBar->m_sizeFloated = _size - ptOffsetWnd;
#else

		// The original code caused the control bar to change its size, 
		// even if the right border was only clicked without moving the mouse

		// The following change fixes this:

		pExtBar->m_sizeFloated += (_size - rcWndOrg.Size());

#endif

//pExtBar->m_pDockContext->m_sizeLast = pExtBar->m_sizeFloated;
		CRect rcWndValidated( rcWnd.TopLeft(), _size );
//pExtBar->m_pDockContext->m_rectDragHorz
//	= pExtBar->m_pDockContext->m_rectDragVert
//	= pExtBar->m_pDockContext->m_rectFrameDragHorz
//	= pExtBar->m_pDockContext->m_rectFrameDragVert
//	= rcWndValidated;
		MoveWindow( &rcWndValidated );
		RecalcLayout();
		SetTimer( 1, 1, NULL );
		g_bInResizingUpdateState = false;
		return;
	} // if( ! bFixedMode )

INT nResizingFrameDX = 0;
INT nResizingFrameDY = 0;
INT nCaptionDY = 0;
	OnQueryNcMetrics(
		nResizingFrameDX,
		nResizingFrameDY,
		nCaptionDY
		);

DWORD dwMode = LM_HORZ|LM_COMMIT;
LONG nLendth = 0;
	if( bHorz )	
		nLendth = rcClient.Width();
	else
	{
		dwMode |= LM_LENGTHY;
		nLendth =
			rcClient.Height()
			- nResizingFrameDY;
	} // else from if( bHorz )	
CSize _sizeFloatedPrev = pExtBar->m_sizeFloated;
CSize _size =
		pExtBar->CalcDynamicLayout( nLendth, dwMode );
	if( _sizeFloatedPrev == _size )
	{
		g_bInResizingUpdateState = false;
		return;
	}

//	if( _size.cx < _sizeMinFloated.cx )
//		_size.cx = _sizeMinFloated.cx;
//	if( _size.cy < _sizeMinFloated.cy )
//		_size.cy = _sizeMinFloated.cy;

	pExtBar->m_sizeFloated = _size;

	if( bInvTop )
	{
		ASSERT( rcWnd.top == rcWndOrg.top );
		rcWnd.bottom = m_rcWndResizingStart.bottom;
		rcWnd.top =
			rcWnd.bottom - _size.cy - ptOffsetWnd.y;
	}
	if( bInvLeft )
	{
		ASSERT( rcWnd.left == rcWndOrg.left );
		rcWnd.right = m_rcWndResizingStart.right;
		rcWnd.left =
			rcWnd.right - _size.cx - ptOffsetWnd.x;
	}

	ASSERT( pExtBar->m_pDockSite != NULL );
	ASSERT( pExtBar->m_pDockBar != NULL );
	pExtBar->m_ptFloatHelper = rcWnd.TopLeft();

	_size += sizeNcDiff;
	_size.cx -= nResizingFrameDX; // (+ v.2.22)
	_size.cy -= nResizingFrameDY;
CRect rcToMoveMinFrame(
		pExtBar->m_ptFloatHelper,
		_size
		);

	MoveWindow( &rcToMoveMinFrame);
//	SendMessage( WM_NCPAINT );
	PostMessage( WM_NCPAINT );
	RecalcLayout();
	pExtBar->RedrawWindow(
		NULL,
		NULL,
		RDW_INVALIDATE|RDW_UPDATENOW
			|RDW_ERASE|RDW_ERASENOW
			|RDW_ALLCHILDREN
		);
	g_bInResizingUpdateState = false;
}

CControlBar * CExtMiniDockFrameWnd::GetControlBar()
{
	ASSERT_VALID( this );
    if( GetSafeHwnd() == NULL )
        return NULL;
CWnd* pTempWnd = GetWindow( GW_CHILD );
    if( pTempWnd == NULL )
        return NULL;
	pTempWnd = pTempWnd->GetWindow( GW_CHILD );
    if( pTempWnd == NULL )
        return NULL;
    if( !pTempWnd->IsKindOf( RUNTIME_CLASS(CControlBar) ) )
        return NULL;
    return reinterpret_cast < CControlBar * > ( pTempWnd );
}

const CControlBar * CExtMiniDockFrameWnd::GetControlBar() const
{
	ASSERT_VALID( this );
	return ( const_cast < CExtMiniDockFrameWnd * > ( this ) ) -> GetControlBar();
}

CExtControlBar * CExtMiniDockFrameWnd::GetControlBarExt( bool bRootLayoutBar )
{
	ASSERT_VALID( this );
CControlBar * pTempWnd = GetControlBar();
    if( pTempWnd == NULL )
        return NULL;
CExtControlBar * pRootLayoutBar = DYNAMIC_DOWNCAST( CExtControlBar, pTempWnd );
    if( pRootLayoutBar == NULL )
        return NULL;
	if( bRootLayoutBar )
		return pRootLayoutBar;
CExtControlBar * pSingleBar = _GetSingleVisibleBar();
	if( pSingleBar != NULL )
		return pSingleBar;
    return pRootLayoutBar;
}

const CExtControlBar * CExtMiniDockFrameWnd::GetControlBarExt( bool bRootLayoutBar ) const
{
	ASSERT_VALID( this );
	return ( const_cast < CExtMiniDockFrameWnd * > ( this ) ) -> GetControlBarExt( bRootLayoutBar );
}

void CExtMiniDockFrameWnd::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	if( _ResizingIsInProgress() )
	{
		_ResizingEnd();
		return;
	}
	if( nHitTest == HTCAPTION )
	{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		CExtCustomizeSite * pSite =
			CExtCustomizeSite::GetCustomizeSite( m_hWnd );
		if(		pSite != NULL
			&&	pSite->IsCustomizeMode()
			)
			return;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		if( CExtControlBar::FindHelpMode(this) )
			return;
		CExtControlBar * pExtBar = GetControlBarExt( false );
		if( pExtBar != NULL )
		{
			ASSERT_VALID( pExtBar );
			
			CFrameWnd * pFrame = pExtBar->_GetDockingFrameImpl();
			ASSERT_VALID( pFrame );

			CPoint point;
			if( ! ::GetCursorPos(&point) )
				return;
			HWND hWndTrack = pFrame->GetSafeHwnd();
			ASSERT( hWndTrack != NULL && ::IsWindow(hWndTrack) );
			CExtPopupMenuWnd * pPopup =
				CExtPopupMenuWnd::InstantiatePopupMenu(
					hWndTrack,
					RUNTIME_CLASS(CExtPopupMenuWnd),
					this
					);
			VERIFY( pPopup->CreatePopupMenu(hWndTrack) );
			if(	! CExtDockBar::_ConstructDockSiteControlBarPopupMenu(
					pFrame,
					pPopup,
					CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_MINIFRAME_NC_CTX,
					pExtBar,
					WM_NCRBUTTONDOWN,
					NULL
					)
				)
			{
				delete pPopup;
				return;
			}
			if( pPopup->ItemGetCount() == 0 )
			{
				delete pPopup;
				return;
			}
			//::SetFocus( hWndTrack );
			if( ! pPopup->TrackPopupMenu(
					TPMX_OWNERDRAW_FIXED,
					point.x,
					point.y,
					NULL,
					this,
					NULL,
					NULL,
					true
					)
				)
			{
				//delete pPopup;
			}
			return;
		} // if( pExtBar != NULL )
	} // if( nHitTest == HTCAPTION )
	CMiniDockFrameWnd::OnNcRButtonDown(nHitTest,point);
}

void CExtMiniDockFrameWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
//	CMiniDockFrameWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
	bCalcValidRects;
	if(	PmBridge_GetPM()->Bar_MiniDockFrameNcCalcSize(
			lpncsp->rgrc[0],
			this,
			GetControlBarExt( false )
			)
		)
		return;

INT nResizingFrameDX = 0;
INT nResizingFrameDY = 0;
INT nCaptionDY = 0;
	OnQueryNcMetrics(
		nResizingFrameDX,
		nResizingFrameDY,
		nCaptionDY
		);

	::InflateRect(
		&lpncsp->rgrc[0],
		- nResizingFrameDX,
		- nResizingFrameDY
		);
	lpncsp->rgrc[0].top += nCaptionDY;
}

void CExtMiniDockFrameWnd::CalcWindowRect(
	LPRECT lpClientRect,
	UINT nAdjustType // = CWnd::AdjustType::adjustBorder
	)
{
	ASSERT_VALID( this );
	ASSERT( lpClientRect != NULL );
	nAdjustType;

INT nResizingFrameDX = 0;
INT nResizingFrameDY = 0;
INT nCaptionDY = 0;
	OnQueryNcMetrics(
		nResizingFrameDX,
		nResizingFrameDY,
		nCaptionDY
		);

	::InflateRect(
		lpClientRect,
		nResizingFrameDX,
		nResizingFrameDY
		);
	lpClientRect->top -= nCaptionDY;
}

void CExtMiniDockFrameWnd::OnNcPaint() 
{
CRect rcClient, rcBar;
	GetClientRect(rcClient);
	ClientToScreen(rcClient);
	GetWindowRect(rcBar);
	rcClient.OffsetRect(-rcBar.TopLeft());
	rcBar.OffsetRect(-rcBar.TopLeft());
CWindowDC dcSrc( this );
	dcSrc.ExcludeClipRect( &rcClient );
CExtMemoryDC dcWin( &dcSrc, &rcBar );
	RenderNC( dcWin );
	PmBridge_GetPM()->OnPaintSessionComplete( this );
}

CExtControlBar * CExtMiniDockFrameWnd::FindSingleVisbleFlatingBar()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return false;
CWnd * pWnd = GetWindow( GW_CHILD );
	if( pWnd == NULL )
		return NULL;
CDockBar * pDockBar = DYNAMIC_DOWNCAST( CDockBar, pWnd );
	if( pDockBar == NULL )
		return NULL;
	if( pDockBar->GetDlgCtrlID() != AFX_IDW_DOCKBAR_FLOAT )
		return NULL;
CExtControlBar * pFoundBar = NULL;
	if( ! CExtControlBar::stat_FindSingleVisbleBarNested( pDockBar, &pFoundBar ) )
		return NULL;
	ASSERT_VALID( pFoundBar );
	ASSERT( pFoundBar->GetSafeHwnd() != NULL );
	ASSERT( ((CControlBar*)pFoundBar) != ((CControlBar*)pDockBar) );
	return pFoundBar;
}

void CExtMiniDockFrameWnd::RenderNC( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

CExtControlBar * pExtBar = GetControlBarExt( false );
bool bExtBar = false;
	if(		pExtBar != NULL
		&&	(! pExtBar->IsFixedMode() )
		)
		bExtBar = true;
CRect rcClient, rcBar;
	GetClientRect(rcClient);
	ClientToScreen(rcClient);
	GetWindowRect(rcBar);
	rcClient.OffsetRect(-rcBar.TopLeft());
	rcBar.OffsetRect(-rcBar.TopLeft());

#ifdef __FREIWALD_ORIGINAL__
bool bActive =
		(	m_bHelperNcActive
		//||	( pExtBar != NULL && pExtBar->IsBarWindowActive() )
		) ? true : false;
#else

bool bActive = false;

HWND hTmpWndFocus = ::GetFocus();
	if(		hTmpWndFocus != NULL
		&&	(	GetSafeHwnd() == hTmpWndFocus
			||	::IsChild( GetSafeHwnd(), hTmpWndFocus )
			)
		)
		bActive = true;

#endif

CExtPaintManager::PAINTDOCKINGFRAMEDATA _pdfd(
		this,
		rcBar,
		rcClient,
		bActive,
		true,
		bExtBar
		);
	PmBridge_GetPM()->PaintDockingFrame( dc, _pdfd );
bool bEnableCaptionText = true;
CExtSafeString sCaption;
	if( pExtBar != NULL )
	{
		if( pExtBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar))
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			&& ( !pExtBar->IsKindOf(RUNTIME_CLASS(CExtDynTabControlBar)) )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			)
			bEnableCaptionText = false;
		else
		{
			pExtBar->OnGetBarCaptionText(
				CExtControlBar::__EBCT_SINGLE_CAPTION_FLOATING,
				sCaption
				);
		}
	}
	if( ! bEnableCaptionText )
	{
		if(		pExtBar != NULL
			&&	pExtBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar))
			)
			bEnableCaptionText = _GetSingleVisibleCaptionText( sCaption );
	}
	else
		if( bEnableCaptionText && sCaption.IsEmpty() )
		{
			if( pExtBar != NULL )
			{
				pExtBar->OnGetBarCaptionText(
					CExtControlBar::__EBCT_SINGLE_CAPTION_FLOATING,
					sCaption
					);
			} // if( pExtBar != NULL )
			else
			{
				int nTextLen = GetWindowTextLength();
				if( nTextLen > 0 )
				{
					GetWindowText( sCaption.GetBuffer( nTextLen+2 ), nTextLen+1 );
					sCaption.ReleaseBuffer();
				} // if( nTextLen > 0 )
			} // else from if( pExtBar != NULL )
		}

CRect rcCapt( m_rcFrameCaption );
CRect rcText( m_rcFrameCaption );
	if( pExtBar == NULL )
		rcText.right = m_rcBtnHideBar.left;

	if( pExtBar != NULL )
		pExtBar->NcButtons_CalcGripperTextLocation( rcText );

	if( ! bActive )
	{
		HWND hTmpWndFocus = ::GetFocus();
		if(		hTmpWndFocus != NULL
			&&	(	GetSafeHwnd() == hTmpWndFocus
				||	::IsChild( GetSafeHwnd(), hTmpWndFocus )
				)
			)
			bActive = true;
	}

	rcText.DeflateRect(2,0);

CExtControlBar * pSingleVisbleFlatingBar = FindSingleVisbleFlatingBar();
	if( pSingleVisbleFlatingBar != NULL )
	{
		CExtControlBar * pFlashingBar = pSingleVisbleFlatingBar;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		CExtDynTabControlBar * pTabbedBar = DYNAMIC_DOWNCAST( CExtDynTabControlBar, pSingleVisbleFlatingBar );
		if( pTabbedBar != NULL )
		{
			LONG nSel = pTabbedBar->GetSwitcherSelection();
			if( nSel >= 0 )
			{
				CExtControlBar * pBar = pTabbedBar->GetBarAt( nSel, true );
				if( pBar != NULL )
				{
					ASSERT_VALID( pBar );
					ASSERT( pBar->GetSafeHwnd() != NULL );
					pFlashingBar = pBar;
				} // if( pBar != NULL )
			} // if( nSel >= 0 )
		} // if( pTabbedBar != NULL )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		CExtPaintManager::PAINTGRIPPERDATA _pgd(
			pFlashingBar, // pSingleVisbleFlatingBar
			rcCapt,
			rcText,
			bActive,
			true,
			false,
			bExtBar,
			sCaption.IsEmpty() ? LPCTSTR( NULL ) : sCaption,
			( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 ) ? true : false
			);
		bool bDrawDefaultCaption = true, bFlashCaptionHighlightedState = false;
		if( pFlashingBar->FlashCaptionIsInProgress( &bFlashCaptionHighlightedState ) )
		{
			_pgd.m_bFlashCaptionHighlightedState = bFlashCaptionHighlightedState;
			_pgd.m_clrFlashCaptionText = pFlashingBar->m_clrFlashCaptionText;
			_pgd.m_clrFlashCaptionBackground = pFlashingBar->m_clrFlashCaptionBackground;
			if(	pFlashingBar->FlashCaption_DoPaintNC(
					dc,
					(LPVOID)(&_pgd)
					)
				)
				bDrawDefaultCaption = false;
		} // if( pFlashingBar->FlashCaptionIsInProgress( &bFlashCaptionHighlightedState ) )
		if( bDrawDefaultCaption )
		{
			PmBridge_GetPM()->PaintGripper( dc, _pgd );
			if( pExtBar != NULL )
				pExtBar->NcButtons_Paint( dc );
		} // if( bDrawDefaultCaption )
	} // if( pSingleVisbleFlatingBar != NULL )
	else
	{
		CExtPaintManager::PAINTGRIPPERDATA _pgd(
				this,
				rcCapt,
				rcText,
				bActive,
				true,
				false,
				bExtBar,
				sCaption.IsEmpty() ? LPCTSTR( NULL ) : sCaption,
				( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 ) ? true : false
				);
		PmBridge_GetPM()->PaintGripper( dc, _pgd );
	}

	if( pExtBar != NULL )
		pExtBar->NcButtons_Paint( dc );
}

CExtControlBar * CExtMiniDockFrameWnd::_GetSingleVisibleBar()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return NULL;
CFrameWnd * pDockSite = GetParentFrame();
	if( pDockSite == NULL )
		return NULL;
	ASSERT( ! pDockSite->IsKindOf( RUNTIME_CLASS(CMiniFrameWnd) ) );
POSITION pos = pDockSite->m_listControlBars.GetHeadPosition();
	for( ; pos != NULL; )
	{
		CExtControlBar * pBar =
			DYNAMIC_DOWNCAST(
				CExtControlBar,
				((CControlBar *)(pDockSite->m_listControlBars.GetNext(pos)))
				);
		if(		pBar == NULL
			||	pBar->m_pDockSite == NULL
			||	pBar->IsFixedMode()
			||	(	pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar))
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
					&& !pBar->IsKindOf(RUNTIME_CLASS(CExtDynTabControlBar))
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
				)
			||	pBar->GetParentFrame() != this
			||	pBar->IsFloating()
			||	(! pBar->IsVisible() )
			)
			continue;
		ASSERT( pBar->m_pDockBar != NULL );
		if(
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynTabBar))
			||
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			!pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar))
			)
			continue;
		if( ! pBar->_IsSingleVisibleInFloatingPalette() )
			return NULL;
		return pBar;
	}
	return NULL;
}

bool CExtMiniDockFrameWnd::_GetSingleVisibleCaptionText( CExtSafeString & sCaption )
{
	ASSERT_VALID( this );
	sCaption = _T("");
CExtControlBar * pBar = _GetSingleVisibleBar();
	if( pBar == NULL )
		return false;
	pBar->OnGetBarCaptionText(
		CExtControlBar::__EBCT_SINGLE_CAPTION_FLOATING,
		sCaption
		);
	return sCaption.IsEmpty() ? false : true;
}

BOOL CExtMiniDockFrameWnd::OnNcActivate(BOOL bActive) 
{
	ASSERT_VALID( this );
	m_bHelperNcActive = bActive ? true : false;

static HWND g_hWndLastActivated = NULL;
	if( m_bHelperNcActive )
	{
		if( g_hWndLastActivated != NULL && g_hWndLastActivated != m_hWnd && ::IsWindow( g_hWndLastActivated ) )
			::SendMessage( g_hWndLastActivated, WM_NCPAINT, 0L, 0L );
		g_hWndLastActivated = m_hWnd;
	}

BOOL rc = CMiniDockFrameWnd::OnNcActivate( bActive );
CExtControlBar * pExtBar = GetControlBarExt( false );
	if( pExtBar != NULL )
	{
		ASSERT_VALID( pExtBar );
		pExtBar->_OnNcMiniFrameActivate( bActive ? true : false );
	}
	if( ( GetStyle() & WS_VISIBLE ) != 0 )
	{
//		SendMessage( WM_NCPAINT );
		PostMessage( WM_NCPAINT );
		if( bActive )
		{
			if( pExtBar != NULL )
			{
				ASSERT_VALID( pExtBar );
				pExtBar->RedrawWindow(
					NULL,
					NULL,
					RDW_INVALIDATE|RDW_UPDATENOW
						|RDW_ERASE|RDW_ALLCHILDREN
					);
			}
		}
	} // if( ( GetStyle() & WS_VISIBLE ) != 0 )
	return rc;
}

void CExtMiniDockFrameWnd::OnSize(UINT nType, int cx, int cy) 
{
//	if( _ResizingIsInProgress() )
//		return;

	ASSERT( cx >= 0 && cy >= 0 );
CExtPaintManager::monitor_parms_t _mp;
	CExtPaintManager::stat_GetMonitorParms( _mp, this );

CRect rcWnd;
	GetWindowRect( &rcWnd );

#ifdef __FREIWALD_ORIGINAL__

	if(		( cx > _mp.m_rcWorkArea.Width() && _mp.m_rcWorkArea.Width() > 0 )
		||	( cy > _mp.m_rcWorkArea.Height() && _mp.m_rcWorkArea.Height() > 0 )
		||	rcWnd.right < _mp.m_rcWorkArea.left
		||	rcWnd.bottom < _mp.m_rcWorkArea.top
		||	rcWnd.left > _mp.m_rcWorkArea.right
		||	rcWnd.top > _mp.m_rcWorkArea.bottom
		)

#else

	// It should be possible to enlarge the size of a floating frame to cover two or more monitors
	// in a multiple monitor configuration.

	// The original implementation, however, adjusted the dimensions of the floating frame according to the
	// dimensions of the primary monitor.

	// This does not only prevent the floating frame from being enlarged to more than one monitor,
	// but may also cause wrong calculations, when the frame is resized on the non-primary monitor and
	// the dimensions / screen resolution of this non-primary monitor are different to those of the primary monitor.

	// This has been adjusted with the following change:

	CRect rcWorkArea = _mp.m_rcWorkArea;

	if (GetSystemMetrics(__cSM_CMONITORS) > 1)
	{
		rcWorkArea = CRect(CPoint(GetSystemMetrics(__cSM_XVIRTUALSCREEN), GetSystemMetrics(__cSM_YVIRTUALSCREEN)), 
											 CSize(GetSystemMetrics(__cSM_CXVIRTUALSCREEN), GetSystemMetrics(__cSM_CYVIRTUALSCREEN)));

	}

	if(		( cx > rcWorkArea.Width() && rcWorkArea.Width() > 0 )
		||	( cy > rcWorkArea.Height() && rcWorkArea.Height() > 0 )
		||	rcWnd.right < rcWorkArea.left
		||	rcWnd.bottom < rcWorkArea.top
		||	rcWnd.left > rcWorkArea.right
		||	rcWnd.top > rcWorkArea.bottom
		)

#endif
	{
		if( ! m_bMovingLargeBar )
			SetTimer( 5, 1, NULL );
		return;
	}

	CMiniDockFrameWnd::OnSize(nType, cx, cy);

CExtControlBar * pExtBar = GetControlBarExt( false );
	VERIFY(
		PmBridge_GetPM()->Bar_MiniDockFrameCaptionCalcRect(
			m_rcFrameCaption,
			this,
			pExtBar
			) 
		);
int nDummyMetric = ::GetSystemMetrics( SM_CXSMSIZE ) - 1;
int nRight =
		m_rcFrameCaption.right - nDummyMetric - 1;
	m_rcBtnHideBar.SetRect(
		nRight,
		m_rcFrameCaption.top + 2,
		nRight + nDummyMetric - 2,
		m_rcFrameCaption.bottom - 2
		);

	if( pExtBar != NULL )
	{
		pExtBar->OnNcAreaButtonsReinitialize();
		pExtBar->OnNcAreaButtonsReposition();
	} // if( pExtBar != NULL )
	if( ::GetCapture() == m_hWnd )
		SendMessage( WM_NCPAINT );
	CExtPaintManager::stat_PassPaintMessages();
}

void CExtMiniDockFrameWnd::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CMiniDockFrameWnd::OnWindowPosChanged(lpwndpos);
CExtControlBar * pExtBar = GetControlBarExt( false );
	if(		_ResizingIsInProgress()
		||	g_bOnWndPosChgProcessing
		||	(lpwndpos->flags & SWP_NOMOVE) != 0
		)
	{
		return;
	}
	if( pExtBar == NULL || (! pExtBar->IsFloating()) )
		return;
CRect rcWnd;
	GetWindowRect( &rcWnd );

INT nResizingFrameDX = 0;
INT nResizingFrameDY = 0;
INT nCaptionDY = 0;
	OnQueryNcMetrics(
		nResizingFrameDX,
		nResizingFrameDY,
		nCaptionDY
		);
CExtPaintManager::monitor_parms_t _mp;
	CExtPaintManager::stat_GetMonitorParms( _mp, this );
	ASSERT( ! g_bOnWndPosChgProcessing );
	//if( rcWnd.top < _mp.m_rcWorkArea.top )
	if( rcWnd.bottom < _mp.m_rcWorkArea.top )
	{
		rcWnd.top = _mp.m_rcWorkArea.top;
		g_bOnWndPosChgProcessing = true;
	}
	else if( rcWnd.top + nCaptionDY > _mp.m_rcWorkArea.bottom )
	{
		rcWnd.top =
			max(
			_mp.m_rcWorkArea.top, 
			_mp.m_rcWorkArea.bottom
				- nCaptionDY
			);
		g_bOnWndPosChgProcessing = true;
	}
	if( ! g_bOnWndPosChgProcessing )
	{
		return;
	}
	SetWindowPos(
		NULL,
		rcWnd.left, rcWnd.top, -1, -1,
		SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE
		);

	pExtBar->OnNcAreaButtonsReposition();
	PostMessage( WM_NCPAINT );
	CExtPaintManager::stat_PassPaintMessages();
	g_bOnWndPosChgProcessing = false;
}

void CExtMiniDockFrameWnd::OnWindowPosChanging( WINDOWPOS* lpwndpos )
{
	CMiniDockFrameWnd::OnWindowPosChanging( lpwndpos );
	if( GetStyle() & __REMOVED_STYLES_NORMAL )
		ModifyStyle( __REMOVED_STYLES_NORMAL, 0, __SWP_FOR_REMOVED_STYLES );
	if( GetExStyle() & __REMOVED_STYLES_EXTENDED )
		ModifyStyleEx( __REMOVED_STYLES_EXTENDED, 0, __SWP_FOR_REMOVED_STYLES );
}

void CExtMiniDockFrameWnd::_AnalyzeButtonsHover(
	bool bAllowShowAdvacedTip // = false
	)
{
	ASSERT_VALID( this );
CPoint point;
	if( ::GetCursorPos( &point ) )
		_AnalyzeButtonsHover( point, bAllowShowAdvacedTip );
}

void CExtMiniDockFrameWnd::_AnalyzeButtonsHover(
	CPoint point,
	bool bAllowShowAdvacedTip // = false
	)
{
	ASSERT_VALID( this );
CExtControlBar * pExtBar = GetControlBarExt( false );
	if( pExtBar != NULL )
	{
		CExtPopupMenuTipWnd * pATTW = pExtBar->OnAdvancedPopupMenuTipWndGet();
		if( pATTW == NULL )
			bAllowShowAdvacedTip = false;
		CExtBarNcAreaButton * pBtn = NULL;
		pExtBar->NcButtons_HitTest( point, &pBtn );
		if( bAllowShowAdvacedTip && pBtn != NULL )
		{
			TOOLINFO _ti;
			::memset( &_ti, 0, sizeof(AFX_OLDTOOLINFO) );
			_ti.cbSize = sizeof( TOOLINFO );
			_ti.hinst = ::AfxGetInstanceHandle();
			if( pBtn->OnToolHitTest( point, &_ti ) > 0 )
			{
				if(		_ti.lpszText != NULL
					&&	_ti.lpszText != LPSTR_TEXTCALLBACK
					&&	_tcslen( _ti.lpszText ) > 0
					)
				{
					CRect rcArea = *pBtn;
					CRect rcDefOffsetWnd;
					GetWindowRect( &rcDefOffsetWnd );
					rcArea.OffsetRect( rcDefOffsetWnd.TopLeft() );
					pExtBar->OnAdvancedPopupMenuTipWndDisplay(
						*pATTW,
						rcArea,
						_ti.lpszText
						);
				}
			}
			if(		_ti.lpszText != NULL
				&&	_ti.lpszText != LPSTR_TEXTCALLBACK
				)
				::free( _ti.lpszText );
		} // if( bAllowShowAdvacedTip && pBtn != NULL )
		return;
	}
CRect rcClient, rcBar;
	GetClientRect( &rcClient );
	ClientToScreen( &rcClient );
	GetWindowRect( &rcBar );
	rcClient.OffsetRect( -rcBar.TopLeft() );
	rcBar.OffsetRect (-rcBar.TopLeft() );
	ScreenToClient( &point );
	point.y -= rcBar.top - rcClient.top;
bool bBtnHideBarHover = false;
	if( m_rcBtnHideBar.PtInRect(point) )
		bBtnHideBarHover = true;
	if(	m_bBtnHideBar != bBtnHideBarHover )
	{
		m_bBtnHideBar = bBtnHideBarHover;
//		SendMessage( WM_NCPAINT );
		PostMessage( WM_NCPAINT );
	}
	if( m_bBtnHideBar || bBtnHideBarHover )
		SetTimer( 2, 20, NULL );
	else
		KillTimer( 2 );
}

void CExtMiniDockFrameWnd::OnNcMouseMove( UINT nHitTest, CPoint point )
{
	if( g_bAutoMakeTopmostOnMouseEvent
		&& (!CExtPopupMenuWnd::IsMenuTracking())
		&& (CExtControlBar::_DraggingGetBar() == NULL)
		)
		BringWindowToTop();
	if( _ResizingIsInProgress() )
		return;
	CMiniDockFrameWnd::OnNcMouseMove( nHitTest, point );
	_AnalyzeButtonsHover( point, true );
}

void CExtMiniDockFrameWnd::OnTimer( __EXT_MFC_UINT_PTR nIDEvent )
{
CExtControlBar * pExtBar = GetControlBarExt( false );
	if( nIDEvent == 1 )
	{
		if( pExtBar == NULL )
			return;
		KillTimer( 1 );
		if( pExtBar->IsFixedMode() )
		{
			CRect rcWnd;
			GetWindowRect( &rcWnd );
			pExtBar->SetInitDesiredSizeFloating( rcWnd.Size() );
		}
		pExtBar->OnNcAreaButtonsReposition();
 		if( IsWindowVisible() )
// 			SendMessage( WM_NCPAINT );
			PostMessage( WM_NCPAINT );
		return;
	}
	if( nIDEvent == 2 )
	{
		_AnalyzeButtonsHover();
		return;
	}
	if( nIDEvent == 4 )
	{
		if( CExtControlBar::_DraggingGetBar() != NULL )
		{
			KillTimer( 4 );
			return;
		} // if( CExtControlBar::_DraggingGetBar() != NULL )
		if( pExtBar != NULL )
		{
			ASSERT_VALID( pExtBar );
			CPoint point;
			if( ! ::GetCursorPos(&point) )
				return;
			UINT nHT = pExtBar->NcButtons_HitTest( point );
			if( nHT == HTCLIENT )
			{
				PostMessage( WM_NCPAINT );
				pExtBar->m_bNcAreaBtnTracking = false;
			}
		}
		if( !pExtBar->m_bNcAreaBtnTracking )
			KillTimer( 4 );
		return;
	}
	if( nIDEvent == 5 )
	{
		KillTimer( 5 );
		CExtPaintManager::monitor_parms_t _mp;
		CExtPaintManager::stat_GetMonitorParms( _mp, this );
		CRect rcWnd;
		GetWindowRect( &rcWnd );

#ifdef __FREIWALD_ORIGINAL__

		if(		( rcWnd.Width() > _mp.m_rcWorkArea.Width() && _mp.m_rcWorkArea.Width() > 0 )
			||	( rcWnd.Height() > _mp.m_rcWorkArea.Height() && _mp.m_rcWorkArea.Height() > 0 )
			)

		{
			rcWnd.right =
				rcWnd.left
				+ ( min( rcWnd.Width(), _mp.m_rcWorkArea.Width() ) )
				;
			rcWnd.bottom =
				rcWnd.top
				+ ( min( rcWnd.Height(), _mp.m_rcWorkArea.Height() ) )
				;

#else

	// It should be possible to enlarge the size of a floating frame to cover two or more monitors
	// in a multiple monitor configuration.

	// The original implementation, however, adjusted the dimensions of the floating frame according to the
	// dimensions of the primary monitor.

	// This does not only prevent the floating frame from being enlarged to more than one monitor,
	// but may also cause wrong calculations, when the frame is resized on the non-primary monitor and
	// the dimensions / screen resolution of this non-primary monitor are different to those of the primary monitor.

	// This has been adjusted with the following change:

		CRect rcWorkArea = _mp.m_rcWorkArea;

		if (GetSystemMetrics(__cSM_CMONITORS) > 1)
		{
			rcWorkArea = CRect(CPoint(GetSystemMetrics(__cSM_XVIRTUALSCREEN), GetSystemMetrics(__cSM_YVIRTUALSCREEN)), 
												 CSize(GetSystemMetrics(__cSM_CXVIRTUALSCREEN), GetSystemMetrics(__cSM_CYVIRTUALSCREEN)));

		}

		if(		( rcWnd.Width() > rcWorkArea.Width() && rcWorkArea.Width() > 0 )
			||	( rcWnd.Height() > rcWorkArea.Height() && rcWorkArea.Height() > 0 )
			)
		{
			rcWnd.right =
				rcWnd.left
				+ ( min( rcWnd.Width(), rcWorkArea.Width() ) )
				;
			rcWnd.bottom =
				rcWnd.top
				+ ( min( rcWnd.Height(), rcWorkArea.Height() ) )
				;
#endif

			m_bMovingLargeBar = true;
			MoveWindow( &rcWnd );
//			SendMessage( WM_NCPAINT );
			PostMessage( WM_NCPAINT );
			m_bMovingLargeBar = false;
		}

		CRect rcWndAdjusted( 0, 0, 0, 0 );
		if( rcWnd.right < _mp.m_rcWorkArea.left )
		{
			rcWndAdjusted = rcWnd;
			rcWndAdjusted.OffsetRect(
				_mp.m_rcWorkArea.left - rcWnd.left,
				0
				);
		}
		if( rcWnd.bottom < _mp.m_rcWorkArea.top )
		{
			rcWndAdjusted = rcWnd;
			rcWndAdjusted.OffsetRect(
				0,
				_mp.m_rcWorkArea.top - rcWnd.top
				);
		}
		if( rcWnd.left > _mp.m_rcWorkArea.right )
		{
			rcWndAdjusted = rcWnd;
			rcWndAdjusted.OffsetRect(
				_mp.m_rcWorkArea.right - rcWnd.right,
				0
				);
		}
		if( rcWnd.top > _mp.m_rcWorkArea.bottom )
		{
			rcWndAdjusted = rcWnd;
			rcWndAdjusted.OffsetRect(
				0,
				_mp.m_rcWorkArea.bottom - rcWnd.bottom
				);
		}
		if( ! rcWndAdjusted.IsRectEmpty() )
		{
			m_bMovingLargeBar = true;
			MoveWindow( &rcWndAdjusted );
//			SendMessage( WM_NCPAINT );
			PostMessage( WM_NCPAINT );
			m_bMovingLargeBar = false;
		}
		return;
	}

	if( pExtBar == NULL )
		return;
	ASSERT_VALID( pExtBar );
	if( nIDEvent == (UINT)pExtBar->GetDlgCtrlID() )
	{
		if( _ResizingIsInProgress() )
			return;
		CPoint pt;
		if( ! ::GetCursorPos( &pt ) )
			return;
		CWnd * pTempWnd = WindowFromPoint( pt );
		int IsMouseInsideTheWindow =
			IsChild( pTempWnd ) || pTempWnd->m_hWnd == m_hWnd;

		if(	IsMouseInsideTheWindow
			|| IsChild(GetFocus())  
			|| (CExtMouseCaptureSink::GetCapture() && pExtBar->m_hWnd == CExtMouseCaptureSink::GetCapture()) 
			|| CExtPopupMenuWnd::IsMenuTracking()
			) 
		{

			if(	IsMouseInsideTheWindow 
				&& !CExtPopupMenuWnd::IsMenuTracking()
				&& IsWindowEnabled()
				&& GetFocus()
				) 
				BringWindowToTop();
		}
	} // if( nIDEvent == (UINT)pExtBar->GetDlgCtrlID() )
}

void CExtMiniDockFrameWnd::OnClose() 
{
	CMiniDockFrameWnd::OnClose();
}

void CExtMiniDockFrameWnd::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
    CMiniDockFrameWnd::OnGetMinMaxInfo( lpMMI );
CExtPaintManager::monitor_parms_t _mp;
	CExtPaintManager::stat_GetMonitorParms( _mp, this );
	if( lpMMI->ptMaxTrackSize.x > _mp.m_rcWorkArea.Width() )
		lpMMI->ptMaxTrackSize.x = _mp.m_rcWorkArea.Width();
	if( lpMMI->ptMaxTrackSize.y > _mp.m_rcWorkArea.Height() ) 
		lpMMI->ptMaxTrackSize.y = _mp.m_rcWorkArea.Height();
}

void CExtMiniDockFrameWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( g_bAutoMakeTopmostOnMouseEvent
		&& (!CExtPopupMenuWnd::IsMenuTracking())
		)
		BringWindowToTop();
	if( _ResizingIsInProgress() )
	{
		ClientToScreen( &point );
		_ResizingUpdateState( point );
		return;
	}
	CMiniDockFrameWnd::OnMouseMove(nFlags, point);
}

void CExtMiniDockFrameWnd::OnCaptureChanged(CWnd *pWnd) 
{
	if( _ResizingIsInProgress()
		&& pWnd != this
		)
	{
		_ResizingEnd();
		return;
	}
	CMiniDockFrameWnd::OnCaptureChanged(pWnd);
}

void CExtMiniDockFrameWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( _ResizingIsInProgress() )
	{
		_ResizingEnd();
		return;
	}
	CMiniDockFrameWnd::OnLButtonUp(nFlags, point);
}

void CExtMiniDockFrameWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if( _ResizingIsInProgress() )
	{
		_ResizingEnd();
		return;
	}
	CMiniDockFrameWnd::OnRButtonDown(nFlags, point);
}

BOOL CExtMiniDockFrameWnd::OnEraseBkgnd(CDC* pDC) 
{
	pDC;
	return TRUE;
}

UINT CExtMiniDockFrameWnd::OnNcHitTest(CPoint point) 
{
	if( CExtControlBar::_DraggingGetBar() != NULL )
		return HTNOWHERE;

CExtControlBar * pExtBar = GetControlBarExt( false );
	if( pExtBar != NULL )
	{
		ASSERT_VALID( pExtBar );
		if( ! pExtBar->_FloatStateChangingIsEnabled( pExtBar ) )
			return HTNOWHERE;
	}

	if( GetStyle() & MFS_MOVEFRAME )
		ModifyStyle( MFS_MOVEFRAME, 0 );
//UINT nHit = CMiniDockFrameWnd::OnNcHitTest(point);
CRect rcBar;
	GetWindowRect( &rcBar );
	if( !rcBar.PtInRect( point ) )
		return HTNOWHERE;
UINT nHit = HTCLIENT;


INT nResizingFrameDX = 0;
INT nResizingFrameDY = 0;
INT nCaptionDY = 0;
	OnQueryNcMetrics(
		nResizingFrameDX,
		nResizingFrameDY,
		nCaptionDY
		);

CRect rcTest(
		rcBar.left,
		rcBar.top,
		rcBar.right,
		rcBar.top + nResizingFrameDY
		);
	if( rcTest.PtInRect( point ) )
		nHit = HTTOP;
	else
	{
		rcTest.SetRect(
			rcBar.left,
			rcBar.bottom - nResizingFrameDY,
			rcBar.right,
			rcBar.bottom
			);
		if( rcTest.PtInRect( point ) )
			nHit = HTBOTTOM;
		else
		{
			rcTest.SetRect(
				rcBar.left,
				rcBar.top,
				rcBar.left + nResizingFrameDX,
				rcBar.bottom
				);
			if( rcTest.PtInRect( point ) )
				nHit = HTLEFT;
			else
			{
				rcTest.SetRect(
					rcBar.right - nResizingFrameDX,
					rcBar.top,
					rcBar.right,
					rcBar.bottom
					);
				if( rcTest.PtInRect( point ) )
					nHit = HTRIGHT;
				else
				{
					rcTest.SetRect(
						rcBar.left,
						rcBar.top + nResizingFrameDY,
						rcBar.right,
						rcBar.top + nResizingFrameDY + nCaptionDY
						);
					if( rcTest.PtInRect( point ) )
						nHit = HTCAPTION;
				}
			}
		}
	}
	switch( nHit )
	{
	case HTTOP:
	{
		rcTest.SetRect(
			rcBar.left,
			rcBar.top,
			rcBar.left + nCaptionDY,
			rcBar.top + nResizingFrameDY
			);
		if( rcTest.PtInRect(point) )
			nHit = HTTOPLEFT;
		else
		{
			rcTest.SetRect(
				rcBar.right - nResizingFrameDY,
				rcBar.top,
				rcBar.right,
				rcBar.top + nResizingFrameDY
				);
			if( rcTest.PtInRect(point) )
				nHit = HTTOPRIGHT;
		}
	}
	break;
	case HTBOTTOM:
	{
		rcTest.SetRect(
			rcBar.left,
			rcBar.bottom - nResizingFrameDY,
			rcBar.left + nCaptionDY,
			rcBar.bottom
			);
		if( rcTest.PtInRect(point) )
			nHit = HTBOTTOMLEFT;
		else
		{
			rcTest.SetRect(
				rcBar.right - nResizingFrameDY,
				rcBar.bottom - nResizingFrameDY,
				rcBar.right,
				rcBar.bottom
				);
			if( rcTest.PtInRect(point) )
				nHit = HTBOTTOMRIGHT;
		}
	}
	break;
	case HTLEFT:
	{
		rcTest.SetRect(
			rcBar.left,
			rcBar.top,
			rcBar.left + nResizingFrameDX,
			rcBar.top + nCaptionDY
			);
		if( rcTest.PtInRect(point) )
			nHit = HTTOPLEFT;
		else
		{
			rcTest.SetRect(
				rcBar.left,
				rcBar.bottom - nCaptionDY,
				rcBar.left + nResizingFrameDX,
				rcBar.bottom
				);
			if( rcTest.PtInRect(point) )
				nHit = HTBOTTOMLEFT;
		}
	}
	break;
	case HTRIGHT:
	{
		rcTest.SetRect(
			rcBar.right - nResizingFrameDX,
			rcBar.top,
			rcBar.right,
			rcBar.top + nCaptionDY
			);
		if( rcTest.PtInRect(point) )
			nHit = HTTOPRIGHT;
		else
		{
			rcTest.SetRect(
				rcBar.right - nResizingFrameDX,
				rcBar.bottom - nCaptionDY,
				rcBar.right,
				rcBar.bottom
				);
			if( rcTest.PtInRect(point) )
				nHit = HTBOTTOMRIGHT;
		}
	}
	break;
	} // switch( nHit )

bool bNcRepaint = false;
	if( pExtBar != NULL )
	{
		ASSERT_VALID( pExtBar );
		UINT nHT = pExtBar->NcButtons_HitTest( point );
		if( nHT != HTCLIENT )
		{
			pExtBar->m_bNcAreaBtnTracking = true;
			if( pExtBar->_IsDockSiteCustomizeMode() )
				SetTimer( 4, 20, NULL );
			return nHT;
		}
		else
		{
			bNcRepaint = true;
			pExtBar->m_bNcAreaBtnTracking = false;
		}
		if( pExtBar->IsFixedMode() )
		{
			bool bPaletteMode = false;
			CExtToolControlBar * pToolBar =
				DYNAMIC_DOWNCAST(
					CExtToolControlBar,
					pExtBar
					);
			if( pToolBar != NULL )
			{
				ASSERT_VALID( pToolBar );
				bPaletteMode = pToolBar->m_bPaletteMode;
			} // if( pToolBar != NULL )
			switch( nHit )
			{
			case HTTOP:
			case HTBOTTOM:
			case HTLEFT:
			case HTRIGHT:
				if( bPaletteMode )
					nHit = HTCAPTION;
			break;
			case HTTOPLEFT:
			case HTTOPRIGHT:
				nHit = bPaletteMode ? HTCAPTION : HTTOP;
			break;
			case HTBOTTOMLEFT:
			case HTBOTTOMRIGHT:
				nHit = bPaletteMode ? HTCAPTION : HTBOTTOM;
			break;
			} // switch( nHit )
		} // if( pExtBar->IsFixedMode() )
	} // if( pExtBar != NULL )
	if( bNcRepaint )
//		SendMessage( WM_NCPAINT );
		PostMessage( WM_NCPAINT );
	return nHit;
}

void CExtMiniDockFrameWnd::OnCancelMode() 
{
	CMiniDockFrameWnd::OnCancelMode();
	CExtPopupMenuWnd::CancelMenuTracking();
	CWnd::CancelToolTips();
//	ASSERT( ! CExtPopupMenuWnd::IsMenuTracking() );
	if( _ResizingIsInProgress() )
		_ResizingEnd();
}

void CExtMiniDockFrameWnd::OnPaint() 
{
CPaintDC dcPaint( this );
//	return;
CRect rcClient;
	GetClientRect( &rcClient );
	if(		rcClient.IsRectEmpty()
		||	(! dcPaint.RectVisible( &rcClient ) )
		)
		return;
CExtMemoryDC dc( &dcPaint, &rcClient );
	CExtPaintManager::stat_ExcludeChildAreas(
		dcPaint.GetSafeHdc(),
		GetSafeHwnd()
		);
	if(		(! PmBridge_GetPM()->GetCb2DbTransparentMode(this) )
		||	(! PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
		)
		dc.FillSolidRect(
			&rcClient,
			PmBridge_GetPM()->GetColor(
				CExtPaintManager::CLR_3DFACE_OUT,
				this
				)
			);
}

__EXT_MFC_INT_PTR CExtMiniDockFrameWnd::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	) const
{
	ASSERT_VALID( this );
	
	if( pTI == NULL || m_wndToolTip.GetSafeHwnd() == NULL )
		return -1;

const CControlBar * pBar = GetControlBar();
	if( pBar == NULL )
		return -1;
	ASSERT_VALID( pBar );
	if( (pBar->m_dwStyle & CBRS_TOOLTIPS) == NULL )
		return -1;

__EXT_MFC_INT_PTR nRet = pBar->OnToolHitTest( point, pTI );
	if( nRet < 0 )
		return -1;

	if( pTI->lpszText != NULL )
	{
		CExtMiniDockFrameWnd * pNCT = (CExtMiniDockFrameWnd *)this;
		VERIFY(
			pNCT->m_wndToolTip.AddTool(
				pNCT,
				pTI->lpszText,
				&pTI->rect,
				1
				)
			);
	}
	
	return -1;
}

LRESULT CExtMiniDockFrameWnd::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message)
	{
	case WM_DESTROY:
		m_bHelperNcActive = false;
	break;
	case WM_PRINT:
	case WM_PRINTCLIENT:
		{
			CDC * pDC = CDC::FromHandle( (HDC) wParam );
			if( (lParam&PRF_NONCLIENT) != 0 )
				RenderNC( *pDC );
			if( (lParam&PRF_CHILDREN) != 0 )
				CExtPaintManager::stat_PrintChildren(
					m_hWnd,
					message,
					pDC->GetSafeHdc(),
					lParam
					);
		}
		return (!0);
	} // switch( message)
LRESULT lResult = CMiniDockFrameWnd::WindowProc( message, wParam, lParam );
	return lResult;
}

BOOL CExtMiniDockFrameWnd::PreTranslateMessage( MSG* pMsg ) 
{
	if( m_wndToolTip.GetSafeHwnd() != NULL )
		m_wndToolTip.RelayEvent( pMsg );
	return CMiniDockFrameWnd::PreTranslateMessage( pMsg );
}

BOOL CExtMiniDockFrameWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( ! RegisterMiniDockFrameWndClass() )
	{
		ASSERT( FALSE );
		return FALSE;
	}

	cs.lpszClass = __EXT_MINIDOCKFRAMEWND_CLASS_NAME;

	if( ! CMiniDockFrameWnd::PreCreateWindow( cs ) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	cs.dwExStyle |= WS_EX_TOOLWINDOW;
	cs.dwExStyle &= ~(WS_EX_LAYOUTRTL|WS_EX_RTLREADING);
	return TRUE;
}

void CExtMiniDockFrameWnd::RecalcLayout(BOOL bNotify /* = TRUE */ )
{
	if( GetSafeHwnd() == NULL || m_bInRecalcLayout || m_bLockRecalcLayout )
		return;
	CMiniDockFrameWnd::RecalcLayout( bNotify );
	if( GetSafeHwnd() != NULL )
		SendMessage( WM_NCPAINT );
}

void CExtMiniDockFrameWnd::OnQueryNcMetrics(
	INT & nResizingFrameDX,
	INT & nResizingFrameDY,
	INT & nCaptionDY
	)
{
	PmBridge_GetPM()->Bar_MiniDockFrameGetNcMetrics(
		nResizingFrameDX,
		nResizingFrameDY,
		nCaptionDY,
		this,
		GetControlBarExt( false )
		);
}

bool CExtMiniDockFrameWnd::g_bMiniDockFrameWndClassRegistered = false;

bool CExtMiniDockFrameWnd::RegisterMiniDockFrameWndClass()
{
	if( g_bMiniDockFrameWndClassRegistered )
		return true;
WNDCLASS _wndClassInfo;
HINSTANCE hInst = ::AfxGetInstanceHandle();
	if( ! ::GetClassInfo( hInst, __EXT_MINIDOCKFRAMEWND_CLASS_NAME, &_wndClassInfo ) )
	{
		_wndClassInfo.style = CS_GLOBALCLASS|CS_DBLCLKS;
		_wndClassInfo.lpfnWndProc = ::DefWindowProc;
		_wndClassInfo.cbClsExtra = _wndClassInfo.cbWndExtra = 0;
		_wndClassInfo.hInstance = hInst;
		_wndClassInfo.hIcon = NULL;
		_wndClassInfo.hCursor = ::LoadCursor( NULL, IDC_ARROW );
		ASSERT( _wndClassInfo.hCursor != NULL );
		_wndClassInfo.hbrBackground = NULL; 
		_wndClassInfo.lpszMenuName = NULL;
		_wndClassInfo.lpszClassName = __EXT_MINIDOCKFRAMEWND_CLASS_NAME;
		if( ! ::AfxRegisterClass( &_wndClassInfo ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}
	g_bMiniDockFrameWndClassRegistered = true;
	return true;
}
