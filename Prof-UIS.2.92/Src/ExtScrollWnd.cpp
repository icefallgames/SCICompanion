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

#if (!defined __EXT_SCROLLWND_H)
	#include <ExtScrollWnd.h>
#endif 

#if (!defined __AFXPRIV_H__)
	#include <AfxPriv.h>
#endif 

#if (!defined __EXT_TOOLCONTROLBAR_H)
	#include <ExtToolControlBar.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../src/ExtMemoryDC.h>
#endif

#if _MFC_VER < 0x700
	#include <../src/AfxImpl.h>
#else
	#ifndef __AFXSTATE_H__
		#include <../src/mfc/afxstat_.h>
	#endif
	#include <../src/mfc/AfxImpl.h>
#endif

#if (!defined __EXT_LOCALIZATION_H)
	#include <../Src/ExtLocalization.h>
#endif

#if (!defined __EXT_STATUSCONTROLBAR_H)
	#include <ExtStatusControlBar.h>
#endif

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	#if (!defined __EXTCUSTOMIZE_H)
		#include <ExtCustomize.h>
	#endif
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#include <Resources/Resource.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtScrollBar

IMPLEMENT_DYNCREATE( CExtScrollBar, CScrollBar );

CExtScrollBar::CExtScrollBar()
	: m_bAutoDeleteOnPostNcDestroy( false )
	, m_bReflectParentSizing( true )
	, m_eSO( CExtScrollBar::__ESO_AUTO )
	, m_bDisableAutoReserveSpace( false )
	, m_bAutoReserveSpaceBefore( false )
	, m_bAutoReserveSpaceAfter( false )
	, m_bSmoothPainting( true )
	, m_hWndBefore( NULL )
	, m_hWndAfter( NULL )
	, m_bHideHwndBefore( false )
	, m_bHideHwndAfter( false )
	, m_bCompleteRepaint( true )
	, m_nHorzBarHeight( 0 )
	, m_nVertBarWidth( 0 )
	, m_nHorzButtonWidth( 0 )
	, m_nVertButtonHeight( 0 )
	, m_nHorzThumbMinWidth( 0 )
	, m_nVertThumbMinHeight( 0 )
	, m_bProcessingClick( false )
	, m_bProcessingOutClick( false )
	, m_bProcessingHover( false )
	, m_bProcessingCancelMode( false )
	, m_nSBMHT( INT(CExtPaintManager::__ESBMHT_NOWHERE) )
	, m_nHelperTrackPos( -1 )
	, m_bHelperHaveTrackPos( false )
	, m_bHelperLightAccent( true )
	, m_bEnabledToolTips( false )
	, m_bEnabledTrackingToolTips( false )
	, m_bEnableHookSpy( false )
	, m_bHookCapturedState( false )
	, m_nMouseActivateCode( MA_ACTIVATE )
	, m_bSendActionNotifications( true )
	, m_bPopupInactiveLightMode( false )
	, m_nStepSize( 1 )
	, m_bAlwaysSendThumbButtonNotifications( false )
	, m_bSendEqualNotifications( false )
	, m_ptCtxMenuCache( 0, 0 )
	, m_bEnableCtxMenu( false )
	, m_bZeroExtentMode( false )
{
	m_bHookConvertLocationsToClient = true;
	if( ! g_PaintManager.m_bIsWinNT4orLater )
		m_bCompleteRepaint = false;
/*
	::memset( &m_scrollInfo, 0, sizeof(SCROLLINFO) );
	m_scrollInfo.cbSize = sizeof(SCROLLINFO);
	m_scrollInfo.fMask = SIF_ALL;
*/
}

CExtScrollBar::~CExtScrollBar()
{
CExtAnimationSite * pAcAS = AnimationClient_SiteGet();
	if( pAcAS != NULL )
		pAcAS->AnimationSite_ClientRemove( this );
}

INT CExtScrollBar::GetStepSize() const
{
	ASSERT_VALID( this );
	return m_nStepSize;
}

BEGIN_MESSAGE_MAP( CExtScrollBar, CScrollBar )
	//{{AFX_MSG_MAP(CExtScrollBar)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SIZE()
	ON_WM_NCCALCSIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SHOWWINDOW()
	ON_WM_ENABLE()
	ON_WM_CANCELMODE()
	ON_WM_CAPTURECHANGED()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	__EXT_MFC_ON_WM_NCHITTEST()
	ON_WM_TIMER()
	__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
	ON_MESSAGE( WM_SIZEPARENT, OnSizeParent )
	ON_COMMAND( __EXT_ID_SCROLL_HERE,			OnScrollHere )
	ON_COMMAND( __EXT_ID_SCROLL_EDGE_LOWER,		OnScrollEdgeLower )
	ON_COMMAND( __EXT_ID_SCROLL_EDGE_UPPER,		OnScrollEdgeUpper )
	ON_COMMAND( __EXT_ID_SCROLL_PAGE_LOWER,		OnScrollPageLower )
	ON_COMMAND( __EXT_ID_SCROLL_PAGE_UPPER,		OnScrollPageUpper )
	ON_COMMAND( __EXT_ID_SCROLL_LINE_LOWER,		OnScrollLineLower )
	ON_COMMAND( __EXT_ID_SCROLL_LINE_UPPER,		OnScrollLineUpper )
END_MESSAGE_MAP()

bool CExtScrollBar::AnimationClient_EnabledGet() const
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
		return false;
	return CExtAnimationSingleton::AnimationClient_EnabledGet();
}

bool CExtScrollBar::AnimationSite_EnabledGet() const
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
		return false;
	return CExtAnimationSingleton::AnimationSite_EnabledGet();
}

void CExtScrollBar::AnimationSite_OnProgressShutdownTimer( UINT nTimerID )
{
	ASSERT_VALID( this );
	CExtAnimationSingleton::AnimationSite_OnProgressShutdownTimer( nTimerID );
// 	AnimationClient_StateGet( false ).Empty();
// 	AnimationClient_StateGet( true ).Empty();
}

HWND CExtScrollBar::AnimationSite_GetSafeHWND() const
{
__PROF_UIS_MANAGE_STATE;
HWND hWnd = GetSafeHwnd();
	return hWnd;
}

const CExtAnimationParameters *
	CExtScrollBar::AnimationClient_OnQueryAnimationParameters(
		INT eAPT // __EAPT_*** animation type
		) const
{
	ASSERT_VALID( this );
//return (&PmBridge_GetPM()->g_DefAnimationParametersVerySlow);
const CExtAnimationParameters * pAnimationParameters =
		PmBridge_GetPM()->Animation_GetParameters(
			eAPT,
			(CObject*)this,
			this
			);
	return pAnimationParameters;
}

bool CExtScrollBar::AnimationClient_CacheNextState(
	CDC & dc,
	const RECT & rcAcAnimationTarget,
	bool bAnimate,
	INT eAPT // __EAPT_*** animation type
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( AnimationClient_CacheGeneratorIsLocked() );
	OnSbPaint( dc );
	return
		CExtAnimationSingleton::AnimationClient_CacheNextState(
			dc,
			rcAcAnimationTarget,
			bAnimate,
			eAPT
			);
}

void CExtScrollBar::PmBridge_OnPaintManagerChanged( CExtPaintManager * pGlobalPM )
{
	ASSERT_VALID( this );
	CExtAnimationSingleton::PmBridge_OnPaintManagerChanged( pGlobalPM );
	if( m_bCompleteRepaint )
	{
		_ScanSysMertics();
		if( GetSafeHwnd() != NULL )
		{
			SetWindowPos(
				NULL, 0, 0, 0, 0,
				SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOOWNERZORDER
					|SWP_NOACTIVATE|SWP_FRAMECHANGED
				);
			Invalidate();
		}
	} // if( m_bCompleteRepaint )
}

void CExtScrollBar::_ScanSysMertics()
{
	ASSERT_VALID( this );
bool bGetDefaultMetrics = true;
	if( m_bCompleteRepaint )
	{
		if( PmBridge_GetPM()->ScrollBar_GetMetrics(
				&m_nHorzBarHeight,
				&m_nVertBarWidth,
				&m_nHorzButtonWidth,
				&m_nVertButtonHeight,
				&m_nHorzThumbMinWidth,
				&m_nVertThumbMinHeight,
				this
				)
			)
			bGetDefaultMetrics = false;
	} // if( m_bCompleteRepaint )
	if( bGetDefaultMetrics )
	{
		m_nHorzBarHeight	  = ::GetSystemMetrics( SM_CYHSCROLL );
		m_nVertBarWidth		  = ::GetSystemMetrics( SM_CXVSCROLL );
		m_nHorzButtonWidth    = ::GetSystemMetrics( SM_CXHSCROLL );
		m_nVertButtonHeight   = ::GetSystemMetrics( SM_CYVSCROLL );
		m_nHorzThumbMinWidth  = 9; // ::GetSystemMetrics( SM_CXHTHUMB );
		m_nVertThumbMinHeight = 9; // ::GetSystemMetrics( SM_CYVTHUMB );
	} // if( bGetDefaultMetrics )
	if( m_bZeroExtentMode )
	{
		m_nHorzBarHeight	  = 0;
		m_nVertBarWidth		  = 0;
	} // if( m_bZeroExtentMode )
}

void CExtScrollBar::SyncReservedSpace( CExtScrollBar * pOtherBar )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pOtherBar );
	ASSERT( this != pOtherBar );
CWnd * pWndNext = GetWindow( GW_HWNDNEXT );
	if( pWndNext != pOtherBar )
		return;
	m_bAutoReserveSpaceBefore = m_bAutoReserveSpaceAfter = false;
	pOtherBar->m_bAutoReserveSpaceBefore = pOtherBar->m_bAutoReserveSpaceAfter = false;
	if( m_bDisableAutoReserveSpace )
		return;
	if( (GetStyle()&WS_VISIBLE) == 0 || (pOtherBar->GetStyle()&WS_VISIBLE) == 0 )
		return;
eScrollerOrientation_t _eSOown = GetScrollerOrientation();
eScrollerOrientation_t _eSOother = pOtherBar->GetScrollerOrientation();
	switch( _eSOown )
	{
	case __ESO_TOP:
	case __ESO_BOTTOM:
		switch( _eSOother )
		{
		case __ESO_LEFT:
			m_bAutoReserveSpaceBefore = true;
			break;
		case __ESO_RIGHT:
			m_bAutoReserveSpaceAfter = true;
			break;
		case __ESO_TOP:
		case __ESO_BOTTOM:
		case __ESO_NONE:
			return;
#ifdef _DEBUG
		default:
			ASSERT( FALSE );
			return;
#endif // _DEBUG
		} // switch( _eSOother )
		break;
	case __ESO_LEFT:
	case __ESO_RIGHT:
		switch( _eSOother )
		{
		case __ESO_TOP:
			m_bAutoReserveSpaceBefore = true;
			break;
		case __ESO_BOTTOM:
			m_bAutoReserveSpaceAfter = true;
			break;
		case __ESO_LEFT:
		case __ESO_RIGHT:
		case __ESO_NONE:
			return;
#ifdef _DEBUG
		default:
			ASSERT( FALSE );
			return;
#endif // _DEBUG
		} // switch( _eSOother )
		break;
	case __ESO_NONE:
		return;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
		return;
#endif // _DEBUG
	} // switch( _eSOown )
}

CExtScrollBar::eScrollerOrientation_t
	CExtScrollBar::GetScrollerOrientation() const
{
	ASSERT_VALID( this );
	ASSERT( __ESO_MIN_VALUE <= m_eSO && m_eSO <= __ESO_MAX_VALUE );
eScrollerOrientation_t _eSO = m_eSO;
	if( _eSO == __ESO_AUTO )
	{
		DWORD dwWndStyle = CWnd::GetStyle();
		if( ( dwWndStyle & (SBS_TOPALIGN|SBS_VERT) ) == (SBS_TOPALIGN) )
			_eSO = __ESO_TOP;
		else if( ( dwWndStyle & (SBS_BOTTOMALIGN|SBS_VERT) ) == (SBS_BOTTOMALIGN) )
			_eSO = __ESO_BOTTOM;
		else if( ( dwWndStyle & (SBS_LEFTALIGN|SBS_VERT) ) == (SBS_LEFTALIGN|SBS_VERT) )
			_eSO = __ESO_LEFT;
		else if( ( dwWndStyle & (SBS_RIGHTALIGN|SBS_VERT) ) == (SBS_RIGHTALIGN|SBS_VERT) )
			_eSO = __ESO_RIGHT;
		else if( ( dwWndStyle & SBS_VERT ) != 0 )
			_eSO = __ESO_RIGHT;
		else
			_eSO = __ESO_BOTTOM;
//			_eSO = __ESO_NONE;
	} // if( _eSO == __ESO_AUTO )
	return _eSO;
}

void CExtScrollBar::PreSubclassWindow()
{
	m_bProcessingClick
		= m_bProcessingOutClick
		= m_bProcessingHover
		= m_bProcessingCancelMode
		= m_bHelperHaveTrackPos
		= false;
	m_nHelperTrackPos = -1;
	m_nSBMHT = INT(CExtPaintManager::__ESBMHT_NOWHERE);
	CScrollBar::PreSubclassWindow();
	if(		( ! IsKindOf( RUNTIME_CLASS(CExtZoomScrollBar) ) )
		&&	( (GetExStyle()&WS_EX_LAYOUTRTL) != 0 )
		)
		ModifyStyleEx( WS_EX_LAYOUTRTL, 0, SWP_FRAMECHANGED );
	_ScanSysMertics();

	AnimationSite_ClientProgressStop( this );
	AnimationClient_StateGet( false ).Empty();
	AnimationClient_StateGet( true ).Empty();
CRect rcClient;
	GetClientRect( &rcClient );
	AnimationClient_TargetRectSet( rcClient );
	if( m_bEnableHookSpy )
		HookSpyRegister( __EHSEF_MOUSE_ALL_WITHOUT_WHEEL|__EHSEF_WND_PROC_IN|__EHSEF_PRE_TRANSLATION );
}

LRESULT CExtScrollBar::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
LRESULT lResult = CScrollBar::DefWindowProc( message, wParam, lParam );
	return lResult;
}

LRESULT CExtScrollBar::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == WM_PRINT || message == WM_PRINTCLIENT )
	{
		CDC * pDC = CDC::FromHandle( (HDC)wParam );
		if( m_bCompleteRepaint )
			OnSbPaint( *pDC );
		else
			DefWindowProc( WM_PAINT, (WPARAM)pDC->GetSafeHdc(), 0L );
		return (!0);
	}
bool bUnLockRedraw = false;
HWND hWndOwn = m_hWnd;
	if(		hWndOwn != NULL
		&&	::IsWindow( hWndOwn )
		&&	m_bCompleteRepaint
		&&	_IsLockAbleMessage( message )
		&&	(GetStyle()&WS_VISIBLE) != 0
		)
	{
		bUnLockRedraw = true;
		::SendMessage( hWndOwn, WM_SETREDRAW, FALSE, 0 );
	}
LRESULT lResult = CScrollBar::WindowProc( message, wParam, lParam );
	if(		message == SBM_GETSCROLLINFO
		&&	m_bCompleteRepaint
		&&	lResult != 0L
		)
	{
		SCROLLINFO * pSI = (SCROLLINFO *)lParam;
		if(		pSI != NULL
			&&	pSI->cbSize == sizeof(SCROLLINFO)
			&&	( ( pSI->fMask & SIF_TRACKPOS ) != 0 )
			&&	m_bHelperHaveTrackPos	// &&	m_nHelperTrackPos != -1
			)
			pSI->nTrackPos = m_nHelperTrackPos;
	}

/*
	//if( m_bCompleteRepaint )
	{
		switch( message )
		{
		case SBM_GETPOS:
			lResult = LRESULT(m_scrollInfo.nPos);
		break;
		case SBM_SETPOS:
			m_scrollInfo.nPos = INT(wParam);
			m_scrollInfo.nPos = max( m_scrollInfo.nPos, m_scrollInfo.nMin );
			m_scrollInfo.nPos = min( m_scrollInfo.nPos, m_scrollInfo.nMax );
			lResult = LRESULT(m_scrollInfo.nPos);
			if( lParam != 0 )
				Invalidate();
		break;
		case SBM_GETRANGE:
		{
			LPINT pMin = (LPINT)wParam;
			LPINT pMax = (LPINT)lParam;
			if( pMin != NULL )
				(*pMin) = m_scrollInfo.nMin;
			if( pMax != NULL )
				(*pMax) = m_scrollInfo.nMax;
		}
		break;
		case SBM_SETRANGEREDRAW:
			Invalidate();
			// continue falling
		case SBM_SETRANGE:
			m_scrollInfo.nMin = INT(wParam);
			m_scrollInfo.nMax = INT(lParam);
			m_scrollInfo.nMax = max( m_scrollInfo.nMax, m_scrollInfo.nMin );
			m_scrollInfo.nTrackPos = max( m_scrollInfo.nTrackPos, m_scrollInfo.nMin );
			m_scrollInfo.nTrackPos = min( m_scrollInfo.nTrackPos, m_scrollInfo.nMax );
			m_scrollInfo.nPos = max( m_scrollInfo.nPos, m_scrollInfo.nMin );
			m_scrollInfo.nPos = min( m_scrollInfo.nPos, m_scrollInfo.nMax );
			lResult = LRESULT(m_scrollInfo.nPos);
		break;
		//case SBM_ENABLE_ARROWS:
		//break;
		case SBM_GETSCROLLINFO:
		{
			lResult = 0;
			SCROLLINFO * pSI = (SCROLLINFO*)lParam;
			if( pSI != NULL && pSI->cbSize >= sizeof(SCROLLINFO) )
			{
				if( ( pSI->fMask & SIF_POS ) != 0 )
				{
					lResult = 1;
					pSI->nPos = m_scrollInfo.nPos;
				}
				if( ( pSI->fMask & SIF_TRACKPOS ) != 0 )
				{
					lResult = 1;
					pSI->nTrackPos = m_scrollInfo.nTrackPos;
				}
				if( ( pSI->fMask & SIF_PAGE ) != 0 )
				{
					lResult = 1;
					pSI->nPage = m_scrollInfo.nPage;
				}
				if( ( pSI->fMask & SIF_RANGE ) != 0 )
				{
					lResult = 1;
					pSI->nMin = m_scrollInfo.nMin;
					pSI->nMax = m_scrollInfo.nMax;
				}
			}
		}
		break;
		case SBM_SETSCROLLINFO:
		{
			SCROLLINFO * pSI = (SCROLLINFO*)lParam;
			if( pSI != NULL && pSI->cbSize >= sizeof(SCROLLINFO) )
			{
				if( ( pSI->fMask & SIF_POS ) != 0 )
					m_scrollInfo.nPos = pSI->nPos;
				if( ( pSI->fMask & SIF_TRACKPOS ) != 0 )
					m_scrollInfo.nTrackPos = pSI->nTrackPos;
				if( ( pSI->fMask & SIF_PAGE ) != 0 )
					m_scrollInfo.nPage = pSI->nPage;
				if( ( pSI->fMask & SIF_RANGE ) != 0 )
				{
					m_scrollInfo.nMin = pSI->nMin;
					m_scrollInfo.nMax = pSI->nMax;
				}
			}
			m_scrollInfo.nMax = max( m_scrollInfo.nMax, m_scrollInfo.nMin );
			m_scrollInfo.nTrackPos = max( m_scrollInfo.nTrackPos, m_scrollInfo.nMin );
			m_scrollInfo.nTrackPos = min( m_scrollInfo.nTrackPos, m_scrollInfo.nMax );
			m_scrollInfo.nPos = max( m_scrollInfo.nPos, m_scrollInfo.nMin );
			m_scrollInfo.nPos = min( m_scrollInfo.nPos, m_scrollInfo.nMax );
			lResult = LRESULT(m_scrollInfo.nPos);
			if( wParam != 0 )
				Invalidate();
		}
		break;
		} // switch( message )
	}
*/

	if(		hWndOwn != NULL
		&&	::IsWindow( hWndOwn )
		&&	bUnLockRedraw
		)
	{
		CWnd * pWnd = CWnd::FromHandlePermanent( hWndOwn );
		if( pWnd == this )
		{
			::SendMessage( hWndOwn, WM_SETREDRAW, TRUE, 0 );
			if( (GetStyle()&WS_VISIBLE) != 0 )
			{
				Invalidate();
				if( ScrollBar_CaptureGet() == m_hWnd )
					UpdateWindow();
			}
		}
	}
	return lResult;
}

bool CExtScrollBar::_IsLockAbleMessage( UINT message ) const
{
	if( ! m_bCompleteRepaint )
		return false;
	switch( message )
	{
	case SBM_SETPOS:
	case SBM_SETRANGE:
	case SBM_SETRANGEREDRAW:
	case SBM_SETSCROLLINFO:
	case SBM_ENABLE_ARROWS:
		return true;
	} // switch( message )
	return false;
}

INT CExtScrollBar::_GetScrollPos(
	bool bTrackPos // = false
	) const
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
SCROLLINFO _scroll_info;
	::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
	_scroll_info.cbSize = sizeof(SCROLLINFO);
CScrollBar * pSB = ( CScrollBar * )this;
	if(	pSB->GetScrollInfo(
			&_scroll_info,
			bTrackPos ? SIF_TRACKPOS : SIF_POS
			)
		)
	{
		LONG nRetVal = (LONG)
			(	bTrackPos
					? _scroll_info.nTrackPos
					: _scroll_info.nPos
			);
		return nRetVal;
	}
	return CScrollBar::GetScrollPos();
}

void CExtScrollBar::_SetScrollPos(
	INT nPos,
	bool bTrackPos, // = false
	bool bRedraw, // = true
	bool bSendScrollingNotification // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	AnimationSite_ClientProgressStop( this );
	AnimationClient_StateGet( false ).Empty();
	AnimationClient_StateGet( true ).Empty();
CExtPaintManager::PAINTSCROLLBARDATA _psbd( this );
SCROLLINFO _scroll_info;
	::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
	_scroll_info.cbSize = sizeof(SCROLLINFO);
	if( bTrackPos )
	{
		_scroll_info.fMask = SIF_TRACKPOS;
		_scroll_info.nTrackPos = (int)nPos;
	}
	else
	{
		_scroll_info.fMask = SIF_POS;
		_scroll_info.nPos = (int)nPos;
	}
	m_nHelperTrackPos = (int)nPos;
	if( ! CScrollBar::SetScrollInfo(
			&_scroll_info,
			bRedraw ? TRUE : FALSE
			)
		)
		CScrollBar::SetScrollPos( nPos, bRedraw ? TRUE : FALSE );
	else
		bRedraw = false;
	if(		bSendScrollingNotification
		&&	_psbd.m_bEnabled
		&&	(	m_nSBMHT == CExtPaintManager::__ESBMHT_THUMB
			||	m_bAlwaysSendThumbButtonNotifications
			)
		)
	{
		HWND hWndParent = ::GetParent( m_hWnd );
		if( hWndParent != NULL )
		{
			::SendMessage(
				hWndParent,
				_psbd.m_bHorzBar ? WM_HSCROLL : WM_VSCROLL,
				MAKEWPARAM(
					( bTrackPos ? SB_THUMBTRACK : SB_THUMBPOSITION ),
					nPos
					),
				LPARAM(m_hWnd)
				);
		}
	}
	if( bRedraw )
	{
		Invalidate();
		if( ScrollBar_CaptureGet() == m_hWnd )
			UpdateWindow();
	}
}

CExtNCSB_ScrollContainer * CExtScrollBar::_GetParentScrollContainer()
{
	if( GetSafeHwnd() == NULL )
		return NULL;
	ASSERT_VALID( this );
HWND hWndParent = ::GetParent( m_hWnd );
	if( hWndParent == NULL )
		return NULL;
CWnd * pWndParent = CWnd::FromHandlePermanent( hWndParent );
	if( pWndParent == NULL )
		return NULL;
CExtNCSB_ScrollContainer * pScrollContainer = DYNAMIC_DOWNCAST( CExtNCSB_ScrollContainer, pWndParent );
	return pScrollContainer;
}

CExtPopupMenuTipWnd * CExtScrollBar::OnAdvancedPopupMenuTipWndGet() const
{
	if( ! m_bEnabledToolTips )
		return NULL;
	return (&( CExtPopupMenuSite::g_DefPopupMenuSite.GetTip() ));
}

bool CExtScrollBar::OnAdvancedPopupMenuTipWndGetInfo(
	CExtPopupMenuTipWnd & _ATTW,
	INT nSBMHT,
	bool bTrackingTip,
	CRect & rcExcludeArea,
	CExtSafeString & strTipText
	) const
{
	ASSERT_VALID( this );
	_ATTW;
	strTipText.Empty();
	if(		( ! m_bEnabledToolTips )
		||	( bTrackingTip && ( ! m_bEnabledTrackingToolTips ) )
		)
		return false;
CExtPaintManager::PAINTSCROLLBARDATA _psbd(
		const_cast < CExtScrollBar * > ( this ),
		bTrackingTip,
		CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT)
		);
	switch( nSBMHT )
	{
	case INT(CExtPaintManager::__ESBMHT_BUTTON_UP):
		rcExcludeArea = _psbd.m_rcButtonUp;
		strTipText = m_strTipTextForUpButton;
	break;
	case INT(CExtPaintManager::__ESBMHT_BUTTON_DOWN):
		rcExcludeArea = _psbd.m_rcButtonDown;
		strTipText = m_strTipTextForDownButton;
	break;
	case INT(CExtPaintManager::__ESBMHT_THUMB):
		rcExcludeArea = _psbd.m_rcThumb;
		strTipText = m_strTipTextForThumbButton;
	break;
	case INT(CExtPaintManager::__ESBMHT_PAGE_UP):
		rcExcludeArea = _psbd.m_rcPageUp;
		strTipText = m_strTipTextForPageUpArea;
	break;
	case INT(CExtPaintManager::__ESBMHT_PAGE_DOWN):
		rcExcludeArea = _psbd.m_rcPageDown;
		strTipText = m_strTipTextForPageDownArea;
	break;
	default:
		return false;
	} // switch( nSBMHT )
	ClientToScreen( &rcExcludeArea );
	return true;
}

void CExtScrollBar::OnAdvancedPopupMenuTipWndDisplay(
	CExtPopupMenuTipWnd & _ATTW,
	INT nSBMHT,
	bool bTrackingTip
	) const
{
	ASSERT_VALID( this );
	if(		( ! m_bEnabledToolTips )
		||	( bTrackingTip && ( ! m_bEnabledTrackingToolTips ) )
		)
	{
		_ATTW.Hide();
		return;
	}
CRect rcExcludeArea( 0, 0, 0, 0 );
CExtSafeString strTipText;
	if( ! OnAdvancedPopupMenuTipWndGetInfo(
			_ATTW,
			nSBMHT,
			bTrackingTip,
			rcExcludeArea,
			strTipText
			)
		)
	{
		_ATTW.Hide();
		return;
	}
	if( strTipText.IsEmpty() )
	{
		_ATTW.Hide();
		return;
	}
	OnAdvancedPopupMenuTipWndDisplay(
		_ATTW,
		rcExcludeArea,
		LPCTSTR( strTipText ),
		bTrackingTip
		);
}

void CExtScrollBar::OnAdvancedPopupMenuTipWndDisplay(
	CExtPopupMenuTipWnd & _ATTW,
	const RECT & rcExcludeArea,
	__EXT_MFC_SAFE_LPCTSTR strTipText,
	bool bTrackingTip
	) const
{
	ASSERT_VALID( this );
	ASSERT( strTipText != NULL && _tcslen( strTipText ) > 0 );
	if(		( ! m_bEnabledToolTips )
		||	( bTrackingTip && ( ! m_bEnabledTrackingToolTips ) )
		)
	{
		_ATTW.Hide();
		return;
	}
	if(		_ATTW.GetSafeHwnd() != NULL
		&&	(_ATTW.GetStyle()&WS_VISIBLE) != 0
		&&	_ATTW.GetExcludeArea() == rcExcludeArea
		)
	{
		LPCTSTR strTipTextOld = _ATTW.GetText();
		if(		strTipTextOld != NULL
			&&	_tcscmp( strTipTextOld, strTipText ) == 0
			)
			return;
	}
	if( m_eSO == __ESO_BOTTOM )
		_ATTW.SetTipStyle( CExtPopupMenuTipWnd::__ETS_INV_RECTANGLE_NO_ICON );
	else
		_ATTW.SetTipStyle( CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON );
	_ATTW.SetText( strTipText );
	_ATTW.Show( (CWnd*)this, rcExcludeArea, bTrackingTip );
	if( bTrackingTip )
		CExtPaintManager::stat_PassPaintMessages();
}

void CExtScrollBar::PostNcDestroy()
{
	ASSERT_VALID( this );
	m_bProcessingClick
		= m_bProcessingOutClick
		= m_bProcessingHover
		= m_bProcessingCancelMode
		= m_bHelperHaveTrackPos
		= false;
	m_nHelperTrackPos = -1;
	m_nSBMHT = INT(CExtPaintManager::__ESBMHT_NOWHERE);

	if( m_bEnableHookSpy )
		HookSpyUnregister();

	if( m_bAutoDeleteOnPostNcDestroy )
		delete this;
}

BOOL CExtScrollBar::OnEraseBkgnd( CDC * pDC ) 
{
	if( (! m_bSmoothPainting ) && (! m_bCompleteRepaint ) )
		return CScrollBar::OnEraseBkgnd( pDC );
	return TRUE;
}

void CExtScrollBar::OnSbPaint( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );

CExtPaintManager::PAINTSCROLLBARDATA _psbd( this );
	_psbd.m_bHelperLightAccent = m_bHelperLightAccent;
	if( _psbd.m_bEnabled )
	{
		_psbd.m_eSBMHT =
			(CExtPaintManager::e_scroll_bar_mouse_hover_type_t)
			m_nSBMHT;
		_psbd.m_bHoveredAreaIsPressed
			= m_bProcessingClick && (! m_bProcessingOutClick );
	} // if( _psbd.m_bEnabled )
	
	PmBridge_GetPM()->ScrollBar_Paint( dc, _psbd );
}

void CExtScrollBar::OnPaint() 
{
	if( (! m_bSmoothPainting ) && (! m_bCompleteRepaint ) )
	{
		CScrollBar::OnPaint();
		return;
	} // if( !m_bSmoothPainting )
CRect rcClient;
	GetClientRect( &rcClient );
CPaintDC dcPaint( this );
CExtMemoryDC dc(
		&dcPaint,
		&rcClient
		);
	if( m_bCompleteRepaint )
	{
		if( AnimationClient_StatePaint( dc ) )
			return;
		OnSbPaint( dc );
	} // if( m_bCompleteRepaint )
	else
		DefWindowProc( WM_PAINT, (WPARAM)dc.GetSafeHdc(), 0L );
}

void CExtScrollBar::OnSettingChange( UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection ) 
{
	ASSERT_VALID( this );
	CScrollBar::OnSettingChange( uFlags, lpszSection );
	_ScanSysMertics();
}

LRESULT CExtScrollBar::OnSizeParent( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	if( ! m_bReflectParentSizing )
		return (LRESULT)0;
	wParam;
DWORD dwWndStyle = CWnd::GetStyle();
	if( (dwWndStyle & WS_VISIBLE) == 0 )
		return 0;
AFX_SIZEPARENTPARAMS * lpLayout = (AFX_SIZEPARENTPARAMS*)lParam;
	ASSERT( lpLayout != NULL );

CRect rcOwnLayout( lpLayout->rect );
eScrollerOrientation_t _eSO = GetScrollerOrientation();
CSize _sizeNeeded( 0, 0 );
CRect	rcWndBeforeSrc( 0, 0, 0, 0 ),
		rcWndBeforeDst( 0, 0, 0, 0 ),
		rcWndAfterSrc( 0, 0, 0, 0 ),
		rcWndAfterDst( 0, 0, 0, 0 );
bool	bHwndBeforeVisible = false, bHwndAfterVisible = false,
		bForceHideBefore = false, bForceHideAfter = false;
HWND	hWndBefore = NULL, hWndAfter = NULL, hWndParent = NULL;
	if( lpLayout->hDWP != NULL )
	{
		if(		m_hWndBefore != NULL
			&&	::IsWindow( m_hWndBefore )
			)
		{
			bHwndBeforeVisible =
				( ::__EXT_MFC_GetWindowLong( m_hWndBefore, GWL_STYLE ) & WS_VISIBLE )
					? true : false;
			if( ! m_bHideHwndBefore )
			{
				::GetWindowRect( m_hWndBefore, &rcWndBeforeSrc );
				if( hWndParent == NULL )
				{
					hWndParent = ::GetParent( m_hWnd );
					ASSERT( hWndParent != NULL && ::IsWindow(hWndParent) );
					ASSERT( hWndParent ==  ::GetParent( m_hWndBefore ) );
				} // if( hWndParent == NULL )
				::ScreenToClient( m_hWndBefore, LPPOINT(&rcWndBeforeSrc) );
				::ScreenToClient( m_hWndBefore, LPPOINT(&rcWndBeforeSrc) + 1 );
				__EXT_MFC_LONG_PTR dwExStyle = ::__EXT_MFC_GetWindowLong( m_hWndBefore, GWL_EXSTYLE );
				if( dwExStyle & WS_EX_LAYOUTRTL )
					__EXT_MFC_SwapLeftRight( rcWndBeforeSrc );
				rcWndBeforeDst = rcWndBeforeSrc;
			} // if( !m_bHideHwndBefore )
			hWndBefore = m_hWndBefore;
		}
		if(		m_hWndAfter != NULL
			&&	::IsWindow( m_hWndAfter )
			)
		{
			bHwndAfterVisible =
				( ::__EXT_MFC_GetWindowLong( m_hWndAfter, GWL_STYLE ) & WS_VISIBLE )
					? true : false;
			if( ! m_bHideHwndAfter )
			{
				::GetWindowRect( m_hWndAfter, &rcWndAfterSrc );
				if( hWndParent == NULL )
				{
					hWndParent = ::GetParent( m_hWnd );
					ASSERT( hWndParent != NULL && ::IsWindow(hWndParent) );
					ASSERT( hWndParent ==  ::GetParent( m_hWndAfter ) );
				} // if( hWndParent == NULL )
				::ScreenToClient( m_hWndAfter, LPPOINT(&rcWndAfterSrc) );
				::ScreenToClient( m_hWndAfter, LPPOINT(&rcWndAfterSrc) + 1 );
				__EXT_MFC_LONG_PTR dwExStyle = ::__EXT_MFC_GetWindowLong( m_hWndAfter, GWL_EXSTYLE );
				if( dwExStyle & WS_EX_LAYOUTRTL )
					__EXT_MFC_SwapLeftRight( rcWndAfterSrc );
				rcWndAfterDst = rcWndAfterSrc;
			} // if( !m_bHideHwndAfter )
			hWndAfter = m_hWndAfter;
		}
	} // if( lpLayout->hDWP != NULL )
int nMinExtent;
	switch( _eSO )
	{
	case __ESO_TOP:
		_sizeNeeded.cy = m_nHorzBarHeight;
		lpLayout->rect.top += _sizeNeeded.cy;
		rcOwnLayout.bottom = rcOwnLayout.top + _sizeNeeded.cy;
		lpLayout->sizeTotal.cy += _sizeNeeded.cy;
		if( m_bAutoReserveSpaceBefore )
			rcOwnLayout.left += m_nVertBarWidth;
		if( m_bAutoReserveSpaceAfter )
			rcOwnLayout.right -= m_nVertBarWidth;
		nMinExtent = ::GetSystemMetrics(SM_CXHSCROLL)*2;
		if( hWndAfter != NULL )
		{
			rcWndAfterDst.OffsetRect(
				rcOwnLayout.right - rcWndAfterDst.left - rcWndAfterDst.Width(),
				rcOwnLayout.top - rcWndAfterDst.top
				);
			if( rcWndAfterDst.left <= (rcOwnLayout.left + nMinExtent) )
				bForceHideAfter = true;
			else
			{
				rcWndAfterDst.bottom =
					rcWndAfterDst.top + rcOwnLayout.Height();
				rcOwnLayout.right -= rcWndAfterDst.Width();
			} // if( rcWndAfterDst.left <= (rcOwnLayout.left + nMinExtent) )
		} // if( hWndAfter != NULL )
		if( hWndBefore != NULL )
		{
			rcWndBeforeDst.OffsetRect(
				rcOwnLayout.left - rcWndBeforeDst.left,
				rcOwnLayout.top - rcWndBeforeDst.top
				);
			if( rcWndBeforeDst.right >= (rcOwnLayout.right - nMinExtent) )
				bForceHideBefore = true;
			else
			{
				rcWndBeforeDst.bottom =
					rcWndBeforeDst.top + rcOwnLayout.Height();
				rcOwnLayout.left += rcWndBeforeDst.Width();
			} // else from if( rcWndBeforeDst.right >= (rcOwnLayout.right - nMinExtent) )
		} // if( hWndBefore != NULL )
		break;
	case __ESO_BOTTOM:
		_sizeNeeded.cy = m_nHorzBarHeight;
		lpLayout->rect.bottom -= _sizeNeeded.cy;
		rcOwnLayout.top = rcOwnLayout.bottom - _sizeNeeded.cy;
		lpLayout->sizeTotal.cy += _sizeNeeded.cy;
		if( m_bAutoReserveSpaceBefore )
			rcOwnLayout.left += m_nVertBarWidth;
		if( m_bAutoReserveSpaceAfter )
			rcOwnLayout.right -= m_nVertBarWidth;
		nMinExtent = ::GetSystemMetrics(SM_CXHSCROLL)*2;
		if( hWndAfter != NULL )
		{
			rcWndAfterDst.OffsetRect(
				rcOwnLayout.right - rcWndAfterDst.left - rcWndAfterDst.Width(),
				rcOwnLayout.top - rcWndAfterDst.top
				);
			if( rcWndAfterDst.left <= (rcOwnLayout.left + nMinExtent) )
				bForceHideAfter = true;
			else
			{
				rcWndAfterDst.bottom =
					rcWndAfterDst.top + rcOwnLayout.Height();
				rcOwnLayout.right -= rcWndAfterDst.Width();
			} // if( rcWndAfterDst.left <= (rcOwnLayout.left + nMinExtent) )
		} // if( hWndAfter != NULL )
		if( hWndBefore != NULL )
		{
			rcWndBeforeDst.OffsetRect(
				rcOwnLayout.left - rcWndBeforeDst.left,
				rcOwnLayout.top - rcWndBeforeDst.top
				);
			if( rcWndBeforeDst.right >= (rcOwnLayout.right - nMinExtent) )
				bForceHideBefore = true;
			else
			{
				rcWndBeforeDst.bottom =
					rcWndBeforeDst.top + rcOwnLayout.Height();
				rcOwnLayout.left += rcWndBeforeDst.Width();
			} // else from if( rcWndBeforeDst.right >= (rcOwnLayout.right - nMinExtent) )
		} // if( hWndBefore != NULL )
		break;
	case __ESO_LEFT:
		_sizeNeeded.cx = m_nVertBarWidth;
		lpLayout->rect.left += _sizeNeeded.cx;
		rcOwnLayout.right = rcOwnLayout.left + _sizeNeeded.cx;
		lpLayout->sizeTotal.cx += _sizeNeeded.cx;
		if( m_bAutoReserveSpaceBefore )
			rcOwnLayout.top += m_nHorzBarHeight;
		if( m_bAutoReserveSpaceAfter )
			rcOwnLayout.bottom -= m_nHorzBarHeight;
		break;
	case __ESO_RIGHT:
		_sizeNeeded.cx = m_nVertBarWidth;
		lpLayout->rect.right -= _sizeNeeded.cx;
		rcOwnLayout.left = rcOwnLayout.right - _sizeNeeded.cx;
		lpLayout->sizeTotal.cx += _sizeNeeded.cx;
		if( m_bAutoReserveSpaceBefore )
			rcOwnLayout.top += m_nHorzBarHeight;
		if( m_bAutoReserveSpaceAfter )
			rcOwnLayout.bottom -= m_nHorzBarHeight;
		nMinExtent = ::GetSystemMetrics(SM_CYVSCROLL)*2;
		if( hWndAfter != NULL )
		{
			rcWndAfterDst.OffsetRect(
				rcOwnLayout.left - rcWndAfterDst.left,
				rcOwnLayout.bottom - rcWndAfterDst.top - rcWndAfterDst.Height()
				);
			if( rcWndAfterDst.top <= (rcOwnLayout.top + nMinExtent) )
				bForceHideAfter = true;
			else
			{
				rcWndAfterDst.right =
					rcWndAfterDst.left + rcOwnLayout.Width();
				rcOwnLayout.bottom -= rcWndAfterDst.Height();
			} // if( rcWndAfterDst.top <= (rcOwnLayout.top + nMinExtent) )
		} // if( hWndAfter != NULL )
		if( hWndBefore != NULL )
		{
			rcWndBeforeDst.OffsetRect(
				rcOwnLayout.left - rcWndBeforeDst.left,
				rcOwnLayout.top - rcWndBeforeDst.top
				);
			if( rcWndBeforeDst.bottom >= (rcOwnLayout.bottom - nMinExtent) )
				bForceHideBefore = true;
			else
			{
				rcWndBeforeDst.right =
					rcWndBeforeDst.left + rcOwnLayout.Width();
				rcOwnLayout.top += rcWndBeforeDst.Height();
			} // else from if( rcWndBeforeDst.bottom >= (rcOwnLayout.bottom - nMinExtent) )
		} // if( hWndBefore != NULL )
		break;
	case __ESO_NONE:
		break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
		break;
#endif // _DEBUG
	} // switch( _eSO )

//	ASSERT( ! rcOwnLayout.IsRectEmpty() );
	if( lpLayout->hDWP != NULL )
	{
		::AfxRepositionWindow(
			lpLayout,
			m_hWnd,
			&rcOwnLayout
			);
		if( hWndBefore != NULL )
		{
			if(		(m_bHideHwndBefore && bHwndBeforeVisible)
				||	bForceHideBefore
				)
				lpLayout->hDWP =
					::DeferWindowPos(
						lpLayout->hDWP,
						hWndBefore, NULL, 0, 0, 0, 0,
						SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE
							|SWP_NOZORDER|SWP_NOOWNERZORDER
							|SWP_HIDEWINDOW
						);
			else if( rcWndBeforeDst != rcWndBeforeSrc )
				lpLayout->hDWP =
					::DeferWindowPos(
						lpLayout->hDWP,
						hWndBefore, NULL,
						rcWndBeforeDst.left, rcWndBeforeDst.top,
						rcWndBeforeDst.Width(), rcWndBeforeDst.Height(),
						SWP_NOACTIVATE|SWP_SHOWWINDOW
							|SWP_NOZORDER|SWP_NOOWNERZORDER
						);
		} // if( hWndBefore != NULL )
		if( hWndAfter != NULL )
		{
			if(		(m_bHideHwndAfter && bHwndAfterVisible)
				||	bForceHideAfter
				)
				lpLayout->hDWP =
					::DeferWindowPos(
						lpLayout->hDWP,
						hWndAfter, NULL, 0, 0, 0, 0,
						SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE
							|SWP_NOZORDER|SWP_NOOWNERZORDER
							|SWP_HIDEWINDOW
						);
			else if( rcWndAfterDst != rcWndAfterSrc )
				lpLayout->hDWP =
					::DeferWindowPos(
						lpLayout->hDWP,
						hWndAfter, NULL,
						rcWndAfterDst.left, rcWndAfterDst.top,
						rcWndAfterDst.Width(), rcWndAfterDst.Height(),
						SWP_NOACTIVATE|SWP_SHOWWINDOW
							|SWP_NOZORDER|SWP_NOOWNERZORDER
						);
		} // if( hWndAfter != NULL )
	} // if( lpLayout->hDWP != NULL )
	return 0L;
}

void CExtScrollBar::OnWindowPosChanging( WINDOWPOS FAR * lpwndpos )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
		CScrollBar::OnWindowPosChanging( lpwndpos );
}

void CExtScrollBar::OnWindowPosChanged( WINDOWPOS FAR * lpwndpos )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
	{
		CScrollBar::OnWindowPosChanged( lpwndpos );
		return;
	}

	AnimationSite_ClientProgressStop( this );
	AnimationClient_StateGet( false ).Empty();
	AnimationClient_StateGet( true ).Empty();
CRect rcClient;
	GetClientRect( &rcClient );
	AnimationClient_TargetRectSet( rcClient );

	if( (GetStyle()&WS_VISIBLE) == 0 )
		return;
	Invalidate();
	if( ScrollBar_CaptureGet() == m_hWnd )
		UpdateWindow();
}

void CExtScrollBar::OnSize(UINT nType, int cx, int cy)
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
	{
		CScrollBar::OnSize( nType, cx, cy );
		return;
	}

	AnimationSite_ClientProgressStop( this );
	AnimationClient_StateGet( false ).Empty();
	AnimationClient_StateGet( true ).Empty();
CRect rcClient;
	GetClientRect( &rcClient );
	AnimationClient_TargetRectSet( rcClient );

CExtPopupMenuTipWnd * pATTW =
		OnAdvancedPopupMenuTipWndGet();
	if( pATTW != NULL )
		pATTW->Hide();

	if( (GetStyle()&WS_VISIBLE) == 0 )
		return;
	Invalidate();
	if( ScrollBar_CaptureGet() == m_hWnd )
		UpdateWindow();
}

void CExtScrollBar::OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR * lpncsp )
{
	ASSERT_VALID( this );
	if( m_bCompleteRepaint )
		return;
	CScrollBar::OnNcCalcSize( bCalcValidRects, lpncsp );
}

UINT CExtScrollBar::OnNcHitTest( CPoint point )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
		return UINT( CScrollBar::OnNcHitTest( point ) );
	return HTCLIENT;
}

int CExtScrollBar::OnMouseActivate( CWnd * pDesktopWnd, UINT nHitTest, UINT message )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
		return CScrollBar::OnMouseActivate( pDesktopWnd, nHitTest, message );
	return m_nMouseActivateCode;
}

void CExtScrollBar::OnShowWindow( BOOL bShow, UINT nStatus ) 
{
	CScrollBar::OnShowWindow( bShow, nStatus );
	if( ! bShow )
	{
		CExtPopupMenuTipWnd * pATTW =
			OnAdvancedPopupMenuTipWndGet();
		if( pATTW != NULL )
			pATTW->Hide();
	} // if( ! bShow )
}

void CExtScrollBar::OnEnable(BOOL bEnable)
{
	ASSERT_VALID( this );
	CScrollBar::OnEnable( bEnable );
	if(		(! m_bCompleteRepaint )
		||	( GetStyle() & WS_VISIBLE ) == 0
		)
		return;
	if( ! bEnable )
	{
		CExtPopupMenuTipWnd * pATTW =
			OnAdvancedPopupMenuTipWndGet();
		if( pATTW != NULL )
			pATTW->Hide();
	} // if( ! bEnable )
	Invalidate();
	if( ScrollBar_CaptureGet() == m_hWnd )
		UpdateWindow();
}

void CExtScrollBar::OnCancelMode()
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
	{
		CScrollBar::OnCancelMode();
		return;
	}
	if( m_bProcessingCancelMode )
		return;
	m_bProcessingCancelMode = true;
	m_bProcessingClick
		= m_bProcessingOutClick
		= m_bProcessingHover
		= false;
	m_nSBMHT = INT(CExtPaintManager::__ESBMHT_NOWHERE);
	ScrollBar_CaptureRelease();
CExtPopupMenuTipWnd * pATTW =
		OnAdvancedPopupMenuTipWndGet();
	if( pATTW != NULL )
		pATTW->Hide();
	Invalidate();
	///UpdateWindow();
	m_bProcessingCancelMode = false;
}

void CExtScrollBar::OnCaptureChanged( CWnd * pWnd )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
	{
		CScrollBar::OnCaptureChanged( pWnd );
		return;
	}
 	if( pWnd != this )
	{
		m_bHookCapturedState = false;
		if( pWnd != NULL )
			SendMessage( WM_CANCELMODE );
	}
	else
		m_bHookCapturedState = true;
}

BOOL CExtScrollBar::OnSetCursor( CWnd * pWnd, UINT nHitTest, UINT message )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
		return CScrollBar::OnSetCursor( pWnd, nHitTest, message );
	SetCursor( ::LoadCursor( NULL, IDC_ARROW ) );
	return TRUE;
}

bool CExtScrollBar::OnHookSpyPreTranslateMessage( MSG * pMSG )
{
__PROF_UIS_MANAGE_STATE;
	ASSERT_VALID( this );
	ASSERT( pMSG != NULL );
	if(		pMSG->hwnd == m_hWnd
		&&	GetSafeHwnd() != NULL
		&&	( GetStyle() & WS_VISIBLE ) != 0
		&&	pMSG->message == WM_TIMER
		&&	AnimationSite_OnHookTimer( UINT(pMSG->wParam) )
		)
		return true;
	return false;
}

bool CExtScrollBar::OnHookSpyMouseMoveMsg( MSG * pMSG )
{
__PROF_UIS_MANAGE_STATE;
	ASSERT_VALID( this );
	ASSERT( pMSG != NULL );
	if(		(! m_bEnableHookSpy )
		||	( ! m_bCompleteRepaint )
		||	pMSG->hwnd == NULL
		||	GetSafeHwnd() == NULL
		)
		return false;
// 	if(		pMSG->hwnd != m_hWnd
// 		&&	( ! m_bHookCapturedState )
// 		)
// 		return false;

DWORD dwOwnWndStyle = GetStyle();
	if( (dwOwnWndStyle&(WS_VISIBLE|WS_DISABLED)) != WS_VISIBLE )
		return false;

CPoint ptScreen( -32767, -32767 );

MSG _msg;
	::memcpy( &_msg, pMSG, sizeof(MSG) );
	if( pMSG->message == WM_NCMOUSEMOVE )
	{
		_msg.message = WM_MOUSEMOVE;
		_msg.wParam = 0;
		CPoint point( short(LOWORD(DWORD(_msg.lParam))), short(HIWORD(DWORD(_msg.lParam))) );
		ptScreen = point;
		ScreenToClient( &point );
		_msg.lParam = MAKELPARAM( point.x ,point.y );
	}
	else if( pMSG->message == WM_MOUSEMOVE )
	{
		if( _msg.hwnd != m_hWnd )
		{
			CPoint point( short(LOWORD(DWORD(_msg.lParam))), short(HIWORD(DWORD(_msg.lParam))) );
			::ClientToScreen( _msg.hwnd, &point );
			ptScreen = point;
			ScreenToClient( &point );
			_msg.lParam = MAKELPARAM( point.x ,point.y );
		}
	}
	else
		return false;

//	return ScrollBar_OnMouseMoveMsg( pMSG );

HWND hWndFromPoint = ::WindowFromPoint( ptScreen );
	if( hWndFromPoint == m_hWnd )
		ScrollBar_CaptureSet();

	if(		pMSG->hwnd != m_hWnd
		&&	( ! m_bHookCapturedState )
		)
		return false;

WNDPROC pWNDPROC = (WNDPROC) ::__EXT_MFC_GetWindowLong( m_hWnd, __EXT_MFC_GWL_WNDPROC );
	::CallWindowProc( pWNDPROC, m_hWnd, _msg.message, _msg.wParam, _msg.lParam );
	return true;
}

bool CExtScrollBar::OnHookSpyMouseClickMsg( MSG * pMSG )
{
__PROF_UIS_MANAGE_STATE;
	ASSERT_VALID( this );
	ASSERT( pMSG != NULL );
	if(		(! m_bEnableHookSpy )
		||	( ! m_bCompleteRepaint )
		||	pMSG->hwnd == NULL
		||	GetSafeHwnd() == NULL
		)
		return false;
//	if(		pMSG->hwnd != m_hWnd
//		&&	( ! m_bHookCapturedState )
//		)
//		return false;

DWORD dwOwnWndStyle = GetStyle();
	if( (dwOwnWndStyle&(WS_VISIBLE|WS_DISABLED)) != WS_VISIBLE )
		return false;

CPoint ptScreen( -32767, -32767 );

MSG _msg;
	::memcpy( &_msg, pMSG, sizeof(MSG) );
bool bNcTranslate = false;
	switch( _msg.message )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		if( _msg.hwnd != m_hWnd )
		{
			CPoint point( short(LOWORD(DWORD(_msg.lParam))), short(HIWORD(DWORD(_msg.lParam))) );
			::ClientToScreen( _msg.hwnd, &point );
			ptScreen = point;
			ScreenToClient( &point );
			_msg.lParam = MAKELPARAM( point.x ,point.y );
		}
	break;
	case WM_NCLBUTTONDOWN:
		_msg.message = WM_LBUTTONDOWN;
		bNcTranslate = true;
	break;
	case WM_NCLBUTTONUP:
		_msg.message = WM_LBUTTONUP;
		bNcTranslate = true;
	break;
	case WM_NCLBUTTONDBLCLK:
		_msg.message = WM_LBUTTONDBLCLK;
		bNcTranslate = true;
	break;
	case WM_NCRBUTTONDOWN:
		_msg.message = WM_RBUTTONDOWN;
		bNcTranslate = true;
	break;
	case WM_NCRBUTTONUP:
		_msg.message = WM_RBUTTONUP;
		bNcTranslate = true;
	break;
	case WM_NCRBUTTONDBLCLK:
		_msg.message = WM_RBUTTONDBLCLK;
		bNcTranslate = true;
	break;
	case WM_NCMBUTTONDOWN:
		_msg.message = WM_MBUTTONDOWN;
		bNcTranslate = true;
	break;
	case WM_NCMBUTTONUP:
		_msg.message = WM_MBUTTONUP;
		bNcTranslate = true;
	break;
	case WM_NCMBUTTONDBLCLK:
		_msg.message = WM_MBUTTONDBLCLK;
		bNcTranslate = true;
	break;
	default:
		return false;
	} //switch( _msg.message )
	if( bNcTranslate )
	{
		_msg.wParam = 0;
		CPoint point( short(LOWORD(DWORD(_msg.lParam))), short(HIWORD(DWORD(_msg.lParam))) );
		ptScreen = point;
		ScreenToClient( &point );
		_msg.lParam = MAKELPARAM( point.x ,point.y );
	} //if( bNcTranslate )

//	return ScrollBar_OnMouseClickMsg( pMSG );

HWND hWndFromPoint = ::WindowFromPoint( ptScreen );
	if( hWndFromPoint == m_hWnd )
	{
		//ScrollBar_CaptureSet();
m_bHookCapturedState = true;
	}
	else
//	if(		pMSG->hwnd != m_hWnd
//		&&	( ! m_bHookCapturedState )
//		)
		return false;

WNDPROC pWNDPROC = (WNDPROC) ::__EXT_MFC_GetWindowLong( m_hWnd, __EXT_MFC_GWL_WNDPROC );
	::CallWindowProc( pWNDPROC, m_hWnd, _msg.message, _msg.wParam, _msg.lParam );
	return true;
}

HWND CExtScrollBar::ScrollBar_CaptureGet() const
{
	ASSERT_VALID( this );
	if( m_bEnableHookSpy && m_bHookCapturedState )
		return m_hWnd;
HWND hWndCapture = ::GetCapture();
	return hWndCapture;
}

void CExtScrollBar::ScrollBar_CaptureSet()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() != NULL && ::GetCapture() != m_hWnd )
	{
		m_bHookCapturedState = true;
		if( ! m_bEnableHookSpy )
			::SetCapture( m_hWnd );
	}
	else
		m_bHookCapturedState = false;
}

void CExtScrollBar::ScrollBar_CaptureRelease()
{
	ASSERT_VALID( this );
	m_bHookCapturedState = false;
	if( ! m_bEnableHookSpy )
		if( GetSafeHwnd() != NULL && ::GetCapture() == m_hWnd )
			::ReleaseCapture();
}

bool CExtScrollBar::ScrollBar_OnMouseMoveMsg( MSG * pMSG )
{
	ASSERT_VALID( this );
	ASSERT( pMSG != NULL );
	if( m_bProcessingClick )
		return true;
CPoint point( short(LOWORD(DWORD(pMSG->lParam))), short(HIWORD(DWORD(pMSG->lParam))) );
bool bAnimationLocked = AnimationClient_CacheGeneratorIsLocked();
HWND hWndCapture = ScrollBar_CaptureGet(); // ::GetCapture();
CExtPopupMenuTipWnd * pATTW = OnAdvancedPopupMenuTipWndGet();
	if( m_bProcessingHover )
	{
		CPoint ptScreen = point;
		ClientToScreen( &ptScreen );
		HWND hWndFromPoint = ::WindowFromPoint( ptScreen );
		if( hWndFromPoint != m_hWnd )
		{
			if( ! bAnimationLocked )
			{
				AnimationClient_CacheGeneratorLock();
//				if( AnimationClient_StateGet(true).IsEmpty() )
 					AnimationClient_CacheNextStateMinInfo(
						false,
						__EAPT_BY_HOVERED_STATE_TURNED_OFF
						);
			}

			m_bProcessingHover = false;
			m_nSBMHT = INT(CExtPaintManager::__ESBMHT_NOWHERE);

			if( hWndCapture == m_hWnd )
				SendMessage( WM_CANCELMODE );

			if( ! bAnimationLocked )
			{
 				AnimationClient_CacheNextStateMinInfo(
					true,
					__EAPT_BY_HOVERED_STATE_TURNED_OFF
					);
				AnimationClient_CacheGeneratorUnlock();
			}

			if( pATTW != NULL )
				pATTW->Hide();

			Invalidate();
			UpdateWindow();
			return true;
		} // if( hWndFromPoint != m_hWnd )
		CExtPaintManager::PAINTSCROLLBARDATA _psbd( this );
		_psbd.AdjustHT( point );
		if( m_nSBMHT != INT(_psbd.m_eSBMHT) )
		{

			if( ! bAnimationLocked )
			{
				AnimationClient_CacheGeneratorLock();
//				if( AnimationClient_StateGet(true).IsEmpty() )
 					AnimationClient_CacheNextStateMinInfo(
						false,
						__EAPT_BY_HOVERED_STATE_TURNED_ON
						);
			}

			m_nSBMHT = INT(_psbd.m_eSBMHT);
			
			if( ! bAnimationLocked )
			{
 				AnimationClient_CacheNextStateMinInfo(
					true,
					__EAPT_BY_HOVERED_STATE_TURNED_ON
					);
				AnimationClient_CacheGeneratorUnlock();
			}

			Invalidate();
			UpdateWindow();
		} // if( m_nSBMHT != INT(_psbd.m_eSBMHT) )
		if( hWndCapture != m_hWnd )
			ScrollBar_CaptureSet();
		if( pATTW != NULL )
			OnAdvancedPopupMenuTipWndDisplay( *pATTW, m_nSBMHT, false );
		return true;
	} // if( m_bProcessingHover )

	if( ! bAnimationLocked )
	{
		AnimationClient_CacheGeneratorLock();
//		if( AnimationClient_StateGet(true).IsEmpty() )
 			AnimationClient_CacheNextStateMinInfo(
				false,
				__EAPT_BY_HOVERED_STATE_TURNED_ON
				);
	}

	m_bProcessingHover = true;
CExtPaintManager::PAINTSCROLLBARDATA _psbd( this );
	_psbd.AdjustHT( point );

bool bForceRepaint = false;
	if( m_nSBMHT != INT(_psbd.m_eSBMHT) )
	{
		m_nSBMHT = INT(_psbd.m_eSBMHT);
		bForceRepaint = true;
	} // if( m_nSBMHT != INT(_psbd.m_eSBMHT) )

	if( ! bAnimationLocked )
	{
 		AnimationClient_CacheNextStateMinInfo(
			true,
			__EAPT_BY_HOVERED_STATE_TURNED_ON
			);
		AnimationClient_CacheGeneratorUnlock();
	}

	if( bForceRepaint )
	{
		Invalidate();
		UpdateWindow();
	} // if( bForceRepaint )
	if( hWndCapture != m_hWnd )
		ScrollBar_CaptureSet();
	if( pATTW != NULL )
		OnAdvancedPopupMenuTipWndDisplay( *pATTW, m_nSBMHT, false );
	return true;
}

bool CExtScrollBar::ScrollBar_OnMouseClickMsg( MSG * pMSG )
{
	ASSERT_VALID( this );
	ASSERT( pMSG != NULL );
	switch( pMSG->message )
	{
	case WM_LBUTTONDOWN:
		ScrollBar_TrackMouseLButtonDown( pMSG );
	break;
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	{
		CExtPopupMenuTipWnd * pATTW = OnAdvancedPopupMenuTipWndGet();
		if( pATTW != NULL )
			pATTW->Hide();
	}
	break;
	default:
		return false;
	} // switch( pMSG->message )
	if( pMSG->message == WM_RBUTTONDOWN && m_bEnableCtxMenu )
		return false;
	return true;
}

void CExtScrollBar::OnMouseMove( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
	{
		CScrollBar::OnMouseMove( nFlags, point );
		return;
	}
MSG _msg;
	::memset( &_msg, 0, sizeof(MSG) );
	_msg.hwnd = m_hWnd;
    _msg.message = WM_MOUSEMOVE;
    _msg.wParam = WPARAM(nFlags);
    _msg.lParam = MAKELPARAM( point.x ,point.y );
	ScrollBar_OnMouseMoveMsg( &_msg );
}

void CExtScrollBar::OnLButtonDown( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
	{
		CScrollBar::OnLButtonDown( nFlags, point );
		return;
	}
MSG _msg;
	::memset( &_msg, 0, sizeof(MSG) );
	_msg.hwnd = m_hWnd;
    _msg.message = WM_LBUTTONDOWN;
    _msg.wParam = WPARAM(nFlags);
    _msg.lParam = MAKELPARAM( point.x ,point.y );
	ScrollBar_OnMouseClickMsg( &_msg );
}

void CExtScrollBar::ScrollBar_TrackMouseLButtonDown( MSG * pMSG )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
		return;
CPoint point( short(LOWORD(DWORD(pMSG->lParam))), short(HIWORD(DWORD(pMSG->lParam))) );
CExtPopupMenuTipWnd * pATTW =
		OnAdvancedPopupMenuTipWndGet();
CExtPaintManager::PAINTSCROLLBARDATA _psbd( this );
	_psbd.AdjustHT( point );
	if(		(! m_bProcessingHover)
		||	m_bProcessingClick
		||	(! _psbd.m_bEnabled )
		||	_psbd.m_eSBMHT == CExtPaintManager::__ESBMHT_NOWHERE
		)
	{
		if(		_psbd.m_eSBMHT == CExtPaintManager::__ESBMHT_NOWHERE
			||	(! _psbd.m_bEnabled )
			)
		{
			if( pATTW != NULL )
				pATTW->Hide();
			SendMessage( WM_CANCELMODE );
			Invalidate();
			UpdateWindow();
			return;
		}
	}

	if( ! m_bPopupInactiveLightMode )
		ActivateTopParent();

bool bAnimationLocked = AnimationClient_CacheGeneratorIsLocked();
	if( ! bAnimationLocked )
	{
		AnimationClient_CacheGeneratorLock();
//		if( AnimationClient_StateGet(true).IsEmpty() )
 			AnimationClient_CacheNextStateMinInfo(
				false,
				__EAPT_BY_PRESSED_STATE_TURNED_ON
				);
	}

	if( m_bEnableHookSpy )
		HookSpyUnregister();

	m_nSBMHT = INT(_psbd.m_eSBMHT);
	m_bProcessingClick = m_bProcessingHover = true;
	m_bProcessingOutClick = false;
	if( ! bAnimationLocked )
	{
 		AnimationClient_CacheNextStateMinInfo(
			true,
			__EAPT_BY_PRESSED_STATE_TURNED_ON
			);
		AnimationClient_CacheGeneratorUnlock();
	}
	Invalidate();
	UpdateWindow();
	if( pATTW != NULL )
		OnAdvancedPopupMenuTipWndDisplay( *pATTW, m_nSBMHT, true );

INT nScrollPosStart = _psbd.m_DSI.nPos, nScrollPos = _psbd.m_DSI.nPos;
	m_nHelperTrackPos = _psbd.m_DSI.nPos;
	m_bHelperHaveTrackPos = true;
CRect rcArea = _psbd.GetAreaRectHT();
const UINT nTimerID_zero_start = 401;
const UINT nTimerID_1st_slow = 402;
const UINT nTimerEllapse_1st_slow = 400;
const UINT nTimerID_2nd_fast = 403;
const UINT nTimerEllapse_2nd_fast = 100;
HWND hWndOwn = GetSafeHwnd();
	ASSERT( hWndOwn != NULL && ::IsWindow( hWndOwn ) );
HWND hWndParent = ::GetParent( hWndOwn );
bool bVirtualMode = false, bFinalNotify = true;
#if (!defined __EXT_MFC_NO_SCROLLWND)
CExtScrollWnd * pExtScrollWnd = NULL;
	if( hWndParent != NULL )
	{
		CWnd * pWndParentPermanent = CWnd::FromHandlePermanent( hWndParent );
		if( pWndParentPermanent != NULL )
		{
			pExtScrollWnd =
				DYNAMIC_DOWNCAST(
					CExtScrollWnd,
					pWndParentPermanent
					);
#if (!defined __EXT_MFC_NO_SCROLLITEMWND)
			if( pExtScrollWnd != NULL )
			{
				CExtScrollItemWnd * pExtScrollItemWnd =
					DYNAMIC_DOWNCAST(
						CExtScrollItemWnd,
						pWndParentPermanent
						);
				if( pExtScrollItemWnd != NULL )
				{
					DWORD dwScrollType = __ESIW_ST_NONE;
					if( _psbd.m_bHorzBar )
						dwScrollType = pExtScrollItemWnd->SiwScrollTypeHGet();
					else
						dwScrollType = pExtScrollItemWnd->SiwScrollTypeVGet();
					if( dwScrollType == __ESIW_ST_VIRTUAL )
						bVirtualMode = true;
				} // if( pExtScrollItemWnd != NULL )
			} // if( pExtScrollWnd != NULL )
#endif // (!defined __EXT_MFC_NO_SCROLLITEMWND)
		} // if( pWndParentPermanent != NULL )
	} // if( hWndParent != NULL )
#endif // (!defined __EXT_MFC_NO_SCROLLWND)

bool bStopFlag = false;
CPoint ptCursor( point );
INT nStepSize = 0L;
bool bUpStep = false;
bool bMouseButtonsNotSwapped = ( ::GetSystemMetrics( SM_SWAPBUTTON ) != 0 ) ? false : true;
	switch( _psbd.m_eSBMHT )
	{
	case CExtPaintManager::__ESBMHT_BUTTON_UP:
		bUpStep = true;
		// continue falling here ...
	case CExtPaintManager::__ESBMHT_BUTTON_DOWN:
		nStepSize = GetStepSize(); // nStepSize = 1L;
#if (!defined __EXT_MFC_NO_SCROLLWND)
		if( pExtScrollWnd != NULL )
		{
			int nDir = ( _psbd.m_eSBMHT == CExtPaintManager::__ESBMHT_BUTTON_DOWN ) ? (+1) : (-1);
			CSize _size = pExtScrollWnd->OnSwGetLineSize( nDir );
			nStepSize = _psbd.m_bHorzBar ? _size.cx : _size.cy;
			if( nStepSize <= 0L )
				nStepSize = GetStepSize(); // nStepSize = 1L;
		}
#endif // (!defined __EXT_MFC_NO_SCROLLWND)
		break;
	case CExtPaintManager::__ESBMHT_PAGE_UP:
		bUpStep = true;
		// continue falling here ...
	case CExtPaintManager::__ESBMHT_PAGE_DOWN:
		nStepSize = (INT)_psbd.m_DSI.nPage;
#if (!defined __EXT_MFC_NO_SCROLLWND)
		if( pExtScrollWnd != NULL )
		{
			int nDir = ( _psbd.m_eSBMHT == CExtPaintManager::__ESBMHT_PAGE_DOWN ) ? (+1) : (-1);
			CSize _size = pExtScrollWnd->OnSwGetPageSize( nDir );
			nStepSize = _psbd.m_bHorzBar ? _size.cx : _size.cy;
		}
#endif // (!defined __EXT_MFC_NO_SCROLLWND)
		if( nStepSize <= 0L )
			nStepSize = GetStepSize(); // nStepSize = 1L;
		break;
	case CExtPaintManager::__ESBMHT_THUMB:
		break;
	} // switch( _psbd.m_eSBMHT )
bool bMenuMode = false;
	if( CExtPopupMenuWnd::IsMenuTracking() )
	{
//		CExtPopupMenuWnd * pPopup = CExtPopupMenuSite::g_DefPopupMenuSite.GetInstance();
//		if( pPopup != NULL )
//		{
//			CWnd * pWnd = GetParent();
//			for( ; pWnd != NULL; pWnd = pWnd->GetParent() )
//			{
//				if( pWnd == pPopup )
//				{
//					bMenuMode = true;
//					break;
//				} // if( pWnd == pPopup )
//			} // for( ; pWnd != NULL; pWnd = pWnd->GetParent() )
//		} // if( pPopup != NULL )
		CWnd * pWnd = GetParent();
		for( ; pWnd != NULL; pWnd = pWnd->GetParent() )
		{
			if( pWnd->IsKindOf( RUNTIME_CLASS(CExtPopupMenuWnd) ) )
			{
				bMenuMode = true;
				break;
			} // if( pWnd == pPopup )
			if( (pWnd->GetStyle()&WS_CHILD) == 0 )
				break;
		} // for( ; pWnd != NULL; pWnd = pWnd->GetParent() )
	} // if( CExtPopupMenuWnd::IsMenuTracking() )
INT nMx = INT( _psbd.m_DSI.nMax - _psbd.m_DSI.nPage + 1 );
INT nScrollLimit =
		  _psbd.m_DSI.nMax
		- _psbd.m_DSI.nMin
		- _psbd.m_DSI.nPage
		+ 1
		;
	ASSERT( nScrollLimit >= 0 );
	if( nStepSize > nScrollLimit )
		nStepSize = nScrollLimit;
CRect rcScrollable = _psbd.m_rcBar;
	if( _psbd.m_bHorzBar )
	{
		rcScrollable.left = _psbd.m_rcButtonUp.right;
		rcScrollable.right = _psbd.m_rcButtonDown.left;
	}
	else
	{
		rcScrollable.top = _psbd.m_rcButtonUp.bottom;
		rcScrollable.bottom = _psbd.m_rcButtonDown.top;
	}
	ScrollBar_CaptureSet(); //::SetCapture( hWndOwn );
	if( nStepSize != 0L )
	{
		::PostMessage( hWndOwn, WM_TIMER, WPARAM(nTimerID_zero_start), LPARAM(0L) );
		//::SetTimer( hWndOwn, nTimerID_1st_slow, nTimerEllapse_1st_slow, NULL );
	}
	for( MSG msg; ::IsWindow( hWndOwn ) && (!bStopFlag); )
	{
		if( ! PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			if( ! ::IsWindow( hWndOwn ) )
				break;
			::WaitMessage();
			continue;
		} // if( !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )

		bool bAnalyzeThumb = false;
		switch( msg.message )
		{
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_CANCELMODE:
		case WM_ACTIVATEAPP:
		case WM_KEYDOWN:
		case WM_KEYUP:
			bStopFlag = true;
		break;
		case WM_CAPTURECHANGED:
			if( (HWND)msg.wParam != hWndOwn )
				bStopFlag = true;
		break;
		case WM_MOUSEMOVE:
			if( m_nSBMHT == INT(CExtPaintManager::__ESBMHT_THUMB) )
			{
				if(		( ! CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ? VK_LBUTTON : VK_RBUTTON,true) )
					||	CExtPopupMenuWnd::IsKeyPressed( VK_MBUTTON )
					||	CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ?  VK_RBUTTON : VK_LBUTTON,true )
					||	( (!bMenuMode) && CExtPopupMenuWnd::IsMenuTracking() )
					)
				{
					bStopFlag = true;
					break;
				}
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
				bAnalyzeThumb = true;
				::GetCursorPos( &ptCursor );
				::ScreenToClient( hWndOwn, &ptCursor );
				break;
			} // if( m_nSBMHT == INT(CExtPaintManager::__ESBMHT_THUMB) )
			if( nStepSize == 0 )
				break;
		case WM_TIMER:
			{
				if(		( ! CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ? VK_LBUTTON : VK_RBUTTON,true) )
					||	CExtPopupMenuWnd::IsKeyPressed( VK_MBUTTON )
					||	CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ? VK_RBUTTON : VK_LBUTTON,true )
					||	( (!bMenuMode) && CExtPopupMenuWnd::IsMenuTracking() )
					)
				{
					bStopFlag = true;
					break;
				}
				if( msg.hwnd != hWndOwn )
					break;
				if(		msg.wParam != nTimerID_zero_start
					&&	msg.wParam != nTimerID_1st_slow
					&&	msg.wParam != nTimerID_2nd_fast
					)
					break;
				if( msg.wParam == nTimerID_zero_start )
					::SetTimer( hWndOwn, nTimerID_1st_slow, nTimerEllapse_1st_slow, NULL );
				else if( msg.wParam == nTimerID_1st_slow )
				{
					::KillTimer( hWndOwn, nTimerID_1st_slow );
					CExtPaintManager::stat_PassPaintMessages();
					::SetTimer( hWndOwn, nTimerID_2nd_fast, nTimerEllapse_2nd_fast, NULL );
				}
				ASSERT( nStepSize != 0L );
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
				::GetCursorPos( &ptCursor );
				::ScreenToClient( hWndOwn, &ptCursor );
				bool bPause = false;
				if( ! rcArea.PtInRect( ptCursor ) )
					bPause = true;
				else
				{
					if(		m_nSBMHT == INT(CExtPaintManager::__ESBMHT_PAGE_UP)
						||	m_nSBMHT == INT(CExtPaintManager::__ESBMHT_PAGE_DOWN)
						)
					{ // update changed area rect (page up/down cases)
						CExtPaintManager::PAINTSCROLLBARDATA _psbd2( this );
						_psbd2.AdjustHT( ptCursor );
						INT nSBMHT2 = INT( _psbd.m_eSBMHT );
						if( nSBMHT2 != m_nSBMHT )
							bPause = true;
						else
						{
							CRect rcArea2 = _psbd2.GetAreaRectHT();
							if( ! rcArea2.PtInRect( ptCursor ) )
								bPause = true;
							else
							{
								if( _psbd2.m_bHorzBar )
								{
									if( m_nSBMHT == INT(CExtPaintManager::__ESBMHT_PAGE_UP) )
									{
										if( ptCursor.x >= _psbd2.m_rcThumb.left )
											bPause = true;
									}
									else if( m_nSBMHT == INT(CExtPaintManager::__ESBMHT_PAGE_DOWN) )
									{
										if( ptCursor.x <= _psbd2.m_rcThumb.right )
											bPause = true;
									}
								}
								else
								{
									if( m_nSBMHT == INT(CExtPaintManager::__ESBMHT_PAGE_UP) )
									{
										if( ptCursor.y >= _psbd2.m_rcThumb.top )
											bPause = true;
									}
									else if( m_nSBMHT == INT(CExtPaintManager::__ESBMHT_PAGE_DOWN) )
									{
										if( ptCursor.y <= _psbd2.m_rcThumb.bottom )
											bPause = true;
									}
								}
							}
						}
					} // update changed area rect (page up/down cases)
				}
				if( bPause )
				{
					if( ! m_bProcessingOutClick )
					{
						m_bProcessingOutClick = true;
						Invalidate();
					}
					if( pATTW != NULL )
						pATTW->Hide();
					continue;
				}

				if( bUpStep )
				{
					nScrollPos -= nStepSize;
					if( nScrollPos < _psbd.m_DSI.nMin )
						nScrollPos = _psbd.m_DSI.nMin;
				} // if( bUpStep )
				else
				{
					nScrollPos += nStepSize;
					if( nScrollPos > nMx )
						nScrollPos = nMx;
				} // else from if( bUpStep )
				if( m_bSendEqualNotifications || _GetScrollPos( true ) != nScrollPos )
				{
					bool bSendScrollingNotification = true, bTrackPos = true;
					if( hWndParent != NULL )
					{
						switch( m_nSBMHT )
						{
						case (CExtPaintManager::__ESBMHT_BUTTON_UP):
//							bSendScrollingNotification = false;
							if( m_bSendActionNotifications )
								::SendMessage(
									hWndParent,
									_psbd.m_bHorzBar ? WM_HSCROLL : WM_VSCROLL,
									MAKEWPARAM(
										( _psbd.m_bHorzBar ? SB_LINELEFT : SB_LINEUP ),
										0
										),
									LPARAM(m_hWnd)
									);
if( ! bVirtualMode )
	_SetScrollPos( nScrollPos, bTrackPos, true, bSendScrollingNotification );
else
	bFinalNotify = (!m_bSendActionNotifications); // false;
						break;
						case (CExtPaintManager::__ESBMHT_BUTTON_DOWN):
//							bSendScrollingNotification = false;
							if( m_bSendActionNotifications )
								::SendMessage(
									hWndParent,
									_psbd.m_bHorzBar ? WM_HSCROLL : WM_VSCROLL,
									MAKEWPARAM(
										( _psbd.m_bHorzBar ? SB_LINERIGHT : SB_LINEDOWN ),
										0
										),
									LPARAM(m_hWnd)
									);
if( ! bVirtualMode )
	_SetScrollPos( nScrollPos, bTrackPos, true, bSendScrollingNotification );
else
	bFinalNotify = (!m_bSendActionNotifications); // false;
						break;
						case (CExtPaintManager::__ESBMHT_PAGE_UP):
//							bSendScrollingNotification = false;
							if( m_bSendActionNotifications )
								::SendMessage(
									hWndParent,
									_psbd.m_bHorzBar ? WM_HSCROLL : WM_VSCROLL,
									MAKEWPARAM(
										( _psbd.m_bHorzBar ? SB_PAGELEFT : SB_PAGEUP ),
										0
										),
									LPARAM(m_hWnd)
									);
if( ! bVirtualMode )
	_SetScrollPos( nScrollPos, bTrackPos, true, bSendScrollingNotification );
else
	bFinalNotify = (!m_bSendActionNotifications); // false;
						break;
						case (CExtPaintManager::__ESBMHT_PAGE_DOWN):
//							bSendScrollingNotification = false;
							if( m_bSendActionNotifications )
								::SendMessage(
									hWndParent,
									_psbd.m_bHorzBar ? WM_HSCROLL : WM_VSCROLL,
									MAKEWPARAM(
										( _psbd.m_bHorzBar ? SB_PAGERIGHT : SB_PAGEDOWN ),
										0
										),
									LPARAM(m_hWnd)
									);
if( ! bVirtualMode )
	_SetScrollPos( nScrollPos, bTrackPos, true, bSendScrollingNotification );
else
	bFinalNotify = (!m_bSendActionNotifications); // false;
						break;
						case (CExtPaintManager::__ESBMHT_THUMB):
							bTrackPos = true;
if( ! bVirtualMode )
	_SetScrollPos( nScrollPos, bTrackPos, true, bSendScrollingNotification );
else
	bFinalNotify = false; // (!m_bSendActionNotifications); // false;
						break;
						} // switch( m_nSBMHT )
					} // if( hWndParent != NULL )
// 					if( ! bVirtualMode )
// 						_SetScrollPos( nScrollPos, bTrackPos, true, bSendScrollingNotification );
// 					else
// 						bFinalNotify = false;
					if( pATTW != NULL && ( ! bAnalyzeThumb ) )
						OnAdvancedPopupMenuTipWndDisplay( *pATTW, m_nSBMHT, true );
				} // if( m_bSendEqualNotifications || _GetScrollPos( true ) != nScrollPos )

CExtPaintManager::PAINTSCROLLBARDATA _psbd2( this );
::memcpy( &_psbd.m_DSI, &_psbd2.m_DSI, sizeof(SCROLLINFO) );
nMx = INT( _psbd.m_DSI.nMax - _psbd.m_DSI.nPage + 1 );
nScrollLimit =
	  _psbd.m_DSI.nMax
	- _psbd.m_DSI.nMin
	- _psbd.m_DSI.nPage
	+ 1
	;
ASSERT( nScrollLimit >= 0 );
if( nStepSize > nScrollLimit )
	nStepSize = nScrollLimit;

				_psbd.AdjustHT( ptCursor );
				bool bProcessingOutClick =
					( m_nSBMHT == INT(_psbd.m_eSBMHT) )
						? false : true;
				rcArea = _psbd.GetAreaRect( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(m_nSBMHT) );
				if( m_bProcessingOutClick != bProcessingOutClick )
				{
					bool bAnimationLocked = AnimationClient_CacheGeneratorIsLocked();
					if( ! bAnimationLocked )
					{
						AnimationClient_CacheGeneratorLock();
//						if( AnimationClient_StateGet(true).IsEmpty() )
 							AnimationClient_CacheNextStateMinInfo(
								false,
								__EAPT_BY_PRESSED_STATE_TURNED_OFF
								);
					}
					m_bProcessingOutClick = bProcessingOutClick;
					if( ! bAnimationLocked )
					{
 						AnimationClient_CacheNextStateMinInfo(
							true,
							__EAPT_BY_PRESSED_STATE_TURNED_OFF
							);
						AnimationClient_CacheGeneratorUnlock();
					}
					Invalidate();
					UpdateWindow();
				}
			}
		break;
		default:
		{
			if(		( ! CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ? VK_LBUTTON : VK_RBUTTON,true) )
				||	CExtPopupMenuWnd::IsKeyPressed( VK_MBUTTON )
				||	CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ? VK_RBUTTON : VK_LBUTTON,true )
				||	( (!bMenuMode) && CExtPopupMenuWnd::IsMenuTracking() )
				)
				bStopFlag = true;
		}
		break;
		} // switch( msg.message )
		if( ! ::IsWindow( hWndOwn ) )
			bStopFlag = true;
		if( bStopFlag || nScrollLimit == 0L )
			break;
		if( bAnalyzeThumb )
		{
			LONG nPixelOffset = _psbd.m_bHorzBar
				? (ptCursor.x - point.x)
				: (ptCursor.y - point.y);
			LONG nPixelExtent = _psbd.m_bHorzBar
				? (rcScrollable.Width() - _psbd.m_rcThumb.Width())
				: (rcScrollable.Height() - _psbd.m_rcThumb.Height());
			if( nPixelExtent <= 0 )
			{
				bStopFlag = true;
				break;
			} // if( nPixelExtent <= 0 )
			if( abs(nPixelOffset) > nPixelExtent )
				nPixelOffset =
					(nPixelOffset < 0)
						? (-nPixelExtent)
						: nPixelExtent;
			INT nShift =
				( nPixelExtent == 0 || nPixelOffset == 0 )
					? 0
					: ::MulDiv( nScrollLimit, abs(nPixelOffset), nPixelExtent );
			nScrollPos = nScrollPosStart;
			if( nPixelOffset < 0 )
			{
				nScrollPos -= nShift;
				if( nScrollPos < _psbd.m_DSI.nMin )
					nScrollPos = _psbd.m_DSI.nMin;
			} // if( nPixelOffset < 0 )
			else
			{
				nScrollPos += nShift;
				if( nScrollPos > nMx )
					nScrollPos = nMx;
			} // else from if( nPixelOffset < 0 )

			if(		(! bVirtualMode )
			//	||	nScrollPos == _psbd.m_DSI.nMin
			//	||	nScrollPos >= _psbd.m_DSI.nMax-200000
				)
			{
				if( m_bSendEqualNotifications || _GetScrollPos( true ) != nScrollPos )
				{
					_SetScrollPos( nScrollPos, true );
					if( pATTW != NULL )
						OnAdvancedPopupMenuTipWndDisplay( *pATTW, m_nSBMHT, true );
				} // if( m_bSendEqualNotifications || _GetScrollPos( true ) != nScrollPos )
				bFinalNotify = true;
			}

CExtPaintManager::PAINTSCROLLBARDATA _psbd2( this );
::memcpy( &_psbd.m_DSI, &_psbd2.m_DSI, sizeof(SCROLLINFO) );
nMx = INT( _psbd.m_DSI.nMax - _psbd.m_DSI.nPage + 1 );
nScrollLimit =
	  _psbd.m_DSI.nMax
	- _psbd.m_DSI.nMin
	- _psbd.m_DSI.nPage
	+ 1
	;
ASSERT( nScrollLimit >= 0 );
if( nStepSize > nScrollLimit )
	nStepSize = nScrollLimit;

			_psbd.AdjustHT( ptCursor );
//			bool bProcessingOutClick =
//				( m_nSBMHT == INT(_psbd.m_eSBMHT) )
//					? false : true;
			rcArea = _psbd.GetAreaRect( CExtPaintManager::__ESBMHT_THUMB );
//			if( m_bProcessingOutClick != bProcessingOutClick )
//			{
//				m_bProcessingOutClick = bProcessingOutClick;
//				Invalidate();
//				UpdateWindow();
//			}
			continue;
		} // if( bAnalyzeThumb )
		if(		m_bPopupInactiveLightMode
			&&	(	msg.message == WM_TIMER
				||	(__EXT_MFC_WM_MOUSEFIRST <= msg.message && msg.message <= __EXT_MFC_WM_MOUSELAST )
				)
			)
			::PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
		else
		if( ! AfxGetThread()->PumpMessage() )
			break;
	} // for( MSG msg; ::IsWindow( hWndOwn ) && (!bStopFlag); )
	if( ! ::IsWindow( hWndOwn ) )
		return;
	if( nStepSize != 0L )
	{
		::KillTimer( hWndOwn, nTimerID_1st_slow );
		::KillTimer( hWndOwn, nTimerID_2nd_fast );
	}

	bAnimationLocked = AnimationClient_CacheGeneratorIsLocked();
	if( ! bAnimationLocked )
	{
		AnimationClient_CacheGeneratorLock();
		if( AnimationClient_StateGet(true).IsEmpty() )
 			AnimationClient_CacheNextStateMinInfo(
				false,
				__EAPT_BY_PRESSED_STATE_TURNED_OFF
				);
	}
// 	if( nScrollPos > INT(_psbd.m_DSI.nMax-_psbd.m_DSI.nMin-_psbd.m_DSI.nPage) )
// 		nScrollPos = INT(_psbd.m_DSI.nMax-_psbd.m_DSI.nMin-_psbd.m_DSI.nPage);
	if(		bFinalNotify
//		&&	_GetScrollPos( bHorz, false ) != nScrollPos
		)
	{
		bool bSendScrollingNotification = true;
//		switch( m_nSBMHT )
//		{
//		case (CExtPaintManager::__ESBMHT_BUTTON_UP):
//		case (CExtPaintManager::__ESBMHT_BUTTON_DOWN):
//		case (CExtPaintManager::__ESBMHT_PAGE_UP):
//		case (CExtPaintManager::__ESBMHT_PAGE_DOWN):
//			bSendScrollingNotification = false;
//		break;
//		}
//		_SetScrollPos( nScrollPos, true, false, bSendScrollingNotification );
		_SetScrollPos( nScrollPos, false, true, bSendScrollingNotification );
	}
	m_nSBMHT = INT(CExtPaintManager::__ESBMHT_NOWHERE);
	m_bProcessingClick
		= m_bProcessingOutClick
		= m_bProcessingHover
		= false;
	if( ! bAnimationLocked )
	{
		::GetCursorPos( &ptCursor );
		ScreenToClient( &ptCursor );
		_psbd.AdjustHT( ptCursor );
		m_nSBMHT = INT(_psbd.m_eSBMHT);
 		AnimationClient_CacheNextStateMinInfo(
			true,
			__EAPT_BY_PRESSED_STATE_TURNED_OFF
			);
		AnimationClient_CacheGeneratorUnlock();
	}

	Invalidate();
	UpdateWindow();
	m_nHelperTrackPos = -1;
	m_bHelperHaveTrackPos = false;
//	if( ::GetCapture() == hWndOwn )
//		::ReleaseCapture();
	ScrollBar_CaptureRelease();
	if( pATTW != NULL )
		OnAdvancedPopupMenuTipWndDisplay( *pATTW, INT(_psbd.m_eSBMHT), false );

	::SendMessage(
		hWndParent,
		_psbd.m_bHorzBar ? WM_HSCROLL : WM_VSCROLL,
		MAKEWPARAM( SB_ENDSCROLL, 0 ), // MAKEWPARAM( SB_ENDSCROLL, nPos ),
		LPARAM(m_hWnd)
		);

	if( m_bEnableHookSpy )
		HookSpyRegister( __EHSEF_MOUSE_ALL_WITHOUT_WHEEL|__EHSEF_WND_PROC_IN|__EHSEF_PRE_TRANSLATION );
}

void CExtScrollBar::OnLButtonUp( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
	{
		CScrollBar::OnLButtonUp( nFlags, point );
		return;
	}
MSG _msg;
	::memset( &_msg, 0, sizeof(MSG) );
	_msg.hwnd = m_hWnd;
    _msg.message = WM_LBUTTONUP;
    _msg.wParam = WPARAM(nFlags);
    _msg.lParam = MAKELPARAM( point.x ,point.y );
	ScrollBar_OnMouseClickMsg( &_msg );
}

void CExtScrollBar::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
	{
		CScrollBar::OnLButtonDblClk( nFlags, point );
		return;
	}
MSG _msg;
	::memset( &_msg, 0, sizeof(MSG) );
	_msg.hwnd = m_hWnd;
    _msg.message = WM_LBUTTONDOWN; // WM_LBUTTONDBLCLK;
    _msg.wParam = WPARAM(nFlags);
    _msg.lParam = MAKELPARAM( point.x ,point.y );
	ScrollBar_OnMouseClickMsg( &_msg );
}

void CExtScrollBar::OnMButtonDown( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
	{
		CScrollBar::OnMButtonDown( nFlags, point );
		return;
	}
MSG _msg;
	::memset( &_msg, 0, sizeof(MSG) );
	_msg.hwnd = m_hWnd;
    _msg.message = WM_MBUTTONDOWN;
    _msg.wParam = WPARAM(nFlags);
    _msg.lParam = MAKELPARAM( point.x ,point.y );
	ScrollBar_OnMouseClickMsg( &_msg );
}

void CExtScrollBar::OnMButtonUp( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
	{
		CScrollBar::OnMButtonUp( nFlags, point );
		return;
	}
MSG _msg;
	::memset( &_msg, 0, sizeof(MSG) );
	_msg.hwnd = m_hWnd;
    _msg.message = WM_MBUTTONUP;
    _msg.wParam = WPARAM(nFlags);
    _msg.lParam = MAKELPARAM( point.x ,point.y );
	ScrollBar_OnMouseClickMsg( &_msg );
}

void CExtScrollBar::OnMButtonDblClk( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
	{
		CScrollBar::OnMButtonDblClk( nFlags, point );
		return;
	}
MSG _msg;
	::memset( &_msg, 0, sizeof(MSG) );
	_msg.hwnd = m_hWnd;
    _msg.message = WM_MBUTTONDBLCLK;
    _msg.wParam = WPARAM(nFlags);
    _msg.lParam = MAKELPARAM( point.x ,point.y );
	ScrollBar_OnMouseClickMsg( &_msg );
}

void CExtScrollBar::OnRButtonDown( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
	{
		CScrollBar::OnRButtonDown( nFlags, point );
		return;
	}
MSG _msg;
	::memset( &_msg, 0, sizeof(MSG) );
	_msg.hwnd = m_hWnd;
    _msg.message = WM_RBUTTONDOWN;
    _msg.wParam = WPARAM(nFlags);
    _msg.lParam = MAKELPARAM( point.x ,point.y );
	if( ScrollBar_OnMouseClickMsg( &_msg ) )
		return;
	if( ! m_bEnableCtxMenu )
		return;
CExtPopupMenuTipWnd * pATTW = OnAdvancedPopupMenuTipWndGet();
	if( pATTW != NULL )
		pATTW->Hide();
	m_ptCtxMenuCache = point;
CPoint ptScreen = point;
	ClientToScreen( &ptScreen );
	SendMessage( WM_CONTEXTMENU, WPARAM(m_hWnd), MAKELPARAM(ptScreen.x,ptScreen.y) );
}

void CExtScrollBar::OnRButtonUp( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
	{
		CScrollBar::OnRButtonUp( nFlags, point );
		return;
	}
MSG _msg;
	::memset( &_msg, 0, sizeof(MSG) );
	_msg.hwnd = m_hWnd;
    _msg.message = WM_RBUTTONUP;
    _msg.wParam = WPARAM(nFlags);
    _msg.lParam = MAKELPARAM( point.x ,point.y );
	ScrollBar_OnMouseClickMsg( &_msg );
}

void CExtScrollBar::OnRButtonDblClk( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
	{
		CScrollBar::OnRButtonDblClk( nFlags, point );
		return;
	}
MSG _msg;
	::memset( &_msg, 0, sizeof(MSG) );
	_msg.hwnd = m_hWnd;
    _msg.message = WM_RBUTTONDBLCLK;
    _msg.wParam = WPARAM(nFlags);
    _msg.lParam = MAKELPARAM( point.x ,point.y );
	ScrollBar_OnMouseClickMsg( &_msg );
}

void CExtScrollBar::OnTimer( __EXT_MFC_UINT_PTR nIDEvent )
{
	ASSERT_VALID( this );
	if( AnimationSite_OnHookTimer( UINT(nIDEvent) ) )
		return;
	CScrollBar::OnTimer( nIDEvent );
}

void CExtScrollBar::OnContextMenu( CWnd * pWnd, CPoint point )
{
    ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
	{
		CScrollBar::OnContextMenu( pWnd, point );
		return;
	}
	if( ! m_bEnableCtxMenu )
		return;
DWORD dwStyle = GetStyle();
	if( ( dwStyle & SBS_SIZEBOX ) != 0 )
		return;
SCROLLINFO scrollInfo;
	::memset( &scrollInfo, 0, sizeof(SCROLLINFO) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;
	if( ! GetScrollInfo( &scrollInfo ) )
		return;
	if( CExtPopupMenuWnd::IsMenuTracking() )
		return;
CExtNCSB_ScrollContainer * pScrollContainer = _GetParentScrollContainer();
	if( pScrollContainer != NULL )
	{
		HWND hWndScrollingTarget = pScrollContainer->GetSafeScrollingTargetHWND();
		if( hWndScrollingTarget != NULL && ::IsWindow( hWndScrollingTarget ) )
		{
			TCHAR strClassName[ 512 ];
			::memset( strClassName, 0, sizeof(strClassName) );
			::GetClassName( hWndScrollingTarget, strClassName, 512 );
			if( _tcslen( strClassName ) > 0  )
			{
				__EXT_MFC_STRLWR( strClassName, 512 );
				if( _tcscmp( strClassName, _T("combolbox") ) == 0 )
				{
					//__EXT_MFC_LONG_PTR dwWndStyle = ::__EXT_MFC_GetWindowLong( hWndScrollingTarget, GWL_STYLE );
					//if( ( dwWndStyle & WS_CHILD ) == 0 )
						return;
				}
			}
		}
	}

bool bVert = ( ( dwStyle & SBS_VERT ) != 0 ) ? true : false;
HWND hWndOwn = m_hWnd;
HWND hWndFocus = ::GetFocus();
	if( hWndFocus == NULL )
	{
		hWndFocus = ::GetParent( m_hWnd );
		if( hWndFocus == NULL )
			hWndFocus = hWndOwn;
		::SetFocus( hWndFocus );
		if( ! ::IsWindow( hWndOwn ) )
			return;
	}
CExtPopupMenuWnd * pPopup =
		CExtPopupMenuWnd::InstantiatePopupMenu(
			GetSafeHwnd(),
			RUNTIME_CLASS(CExtPopupMenuWnd),
			this
			);
	if( pPopup == NULL )
		return;
	if( ! pPopup->CreatePopupMenu( hWndOwn ) )
	{
		ASSERT( FALSE );
		return;
	}

CExtSafeString strMenuItemText;

	strMenuItemText = _T("Scroll Here");
	VERIFY( pPopup->ItemInsertCommand( __EXT_ID_SCROLL_HERE, -1, LPCTSTR(strMenuItemText), NULL, NULL, false, 0, hWndOwn ) );

	VERIFY( pPopup->ItemInsert() );

	if( bVert )
	{
		strMenuItemText = _T("Top");
		VERIFY( pPopup->ItemInsertCommand( __EXT_ID_SCROLL_EDGE_LOWER, -1, LPCTSTR(strMenuItemText), NULL, NULL, false, 0, hWndOwn ) );

		strMenuItemText = _T("Bottom");
		VERIFY( pPopup->ItemInsertCommand( __EXT_ID_SCROLL_EDGE_UPPER, -1, LPCTSTR(strMenuItemText), NULL, NULL, false, 0, hWndOwn ) );

		VERIFY( pPopup->ItemInsert() );

		strMenuItemText = _T("Page Up");
		VERIFY( pPopup->ItemInsertCommand( __EXT_ID_SCROLL_PAGE_LOWER, -1, LPCTSTR(strMenuItemText), NULL, NULL, false, 0, hWndOwn ) );

		strMenuItemText = _T("Page Down");
		VERIFY( pPopup->ItemInsertCommand( __EXT_ID_SCROLL_PAGE_UPPER, -1, LPCTSTR(strMenuItemText), NULL, NULL, false, 0, hWndOwn ) );

		VERIFY( pPopup->ItemInsert() );

		strMenuItemText = _T("Scroll Up");
		VERIFY( pPopup->ItemInsertCommand( __EXT_ID_SCROLL_LINE_LOWER, -1, LPCTSTR(strMenuItemText), NULL, NULL, false, 0, hWndOwn ) );

		strMenuItemText = _T("Scroll Down");
		VERIFY( pPopup->ItemInsertCommand( __EXT_ID_SCROLL_LINE_UPPER, -1, LPCTSTR(strMenuItemText), NULL, NULL, false, 0, hWndOwn ) );
	} // if( bVert )
	else
	{
		strMenuItemText = _T("Left Edge");
		VERIFY( pPopup->ItemInsertCommand( __EXT_ID_SCROLL_EDGE_LOWER, -1, LPCTSTR(strMenuItemText), NULL, NULL, false, 0, hWndOwn ) );

		strMenuItemText = _T("Right Edge");
		VERIFY( pPopup->ItemInsertCommand( __EXT_ID_SCROLL_EDGE_UPPER, -1, LPCTSTR(strMenuItemText), NULL, NULL, false, 0, hWndOwn ) );

		VERIFY( pPopup->ItemInsert() );

		strMenuItemText = _T("Page Left");
		VERIFY( pPopup->ItemInsertCommand( __EXT_ID_SCROLL_PAGE_LOWER, -1, LPCTSTR(strMenuItemText), NULL, NULL, false, 0, hWndOwn ) );

		strMenuItemText = _T("Page Right");
		VERIFY( pPopup->ItemInsertCommand( __EXT_ID_SCROLL_PAGE_UPPER, -1, LPCTSTR(strMenuItemText), NULL, NULL, false, 0, hWndOwn ) );

		VERIFY( pPopup->ItemInsert() );

		strMenuItemText = _T("Scroll Left");
		VERIFY( pPopup->ItemInsertCommand( __EXT_ID_SCROLL_LINE_LOWER, -1, LPCTSTR(strMenuItemText), NULL, NULL, false, 0, hWndOwn ) );

		strMenuItemText = _T("Scroll Right");
		VERIFY( pPopup->ItemInsertCommand( __EXT_ID_SCROLL_LINE_UPPER, -1, LPCTSTR(strMenuItemText), NULL, NULL, false, 0, hWndOwn ) );
	} // else from if( bVert )
	if( ! pPopup->TrackPopupMenu( TPMX_NO_CMD_UI|TPMX_NO_HIDE_RARELY|TPMX_HIDE_KEYBOARD_ACCELERATORS, point.x, point.y ) )
	{
		delete pPopup;
		ASSERT( FALSE );
		return;
	}
}

void CExtScrollBar::OnScrollHere()
{
	ASSERT_VALID( this );
	if( ! m_bEnableCtxMenu )
		return;
DWORD dwStyle = GetStyle();
	if( ( dwStyle & SBS_SIZEBOX ) != 0 )
		return;
SCROLLINFO scrollInfo;
	::memset( &scrollInfo, 0, sizeof(SCROLLINFO) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;
	if( ! GetScrollInfo( &scrollInfo ) )
		return;
INT nRange = scrollInfo.nMax - scrollInfo.nMin;
	if( nRange <= 0 || nRange <= INT(scrollInfo.nPage) )
		return;
bool bVert = ( ( dwStyle & SBS_VERT ) != 0 ) ? true : false;
CRect rcClient;
	GetClientRect( &rcClient );
INT nPosOld = INT( GetScrollPos() );
INT nPosNew = nPosOld;
	if( bVert )
	{
		INT nHeight = INT( rcClient.Height() - m_nHorzBarHeight * 2 );
		if( nHeight <= 1 )
			return;
		nPosNew = (INT) ::MulDiv( nRange, m_ptCtxMenuCache.y - m_nHorzBarHeight, nHeight ) + scrollInfo.nMin;
	} // if( bVert )
	else
	{
		INT nWidth = INT( rcClient.Width() - m_nVertBarWidth * 2 );
		if( nWidth <= 1 )
			return;
		nPosNew = (INT) ::MulDiv( nRange, m_ptCtxMenuCache.x - m_nVertBarWidth, nWidth ) + scrollInfo.nMin;
	} // else from if( bVert )
	nPosNew = max( nPosNew, scrollInfo.nMin );
	nPosNew = min( nPosNew, scrollInfo.nMax );
	if( nPosOld != nPosNew )
	{
		HWND hWndInvalidate = ::GetParent( m_hWnd ), hWndOwn = m_hWnd;
		_SetScrollPos( nPosNew );
		if( ! ::IsWindow( hWndOwn ) )
			return;
		CExtNCSB_ScrollContainer * pScrollContainer = _GetParentScrollContainer();
		if( pScrollContainer != NULL )
		{
			HWND hWndScrollingTarget = pScrollContainer->GetSafeScrollingTargetHWND();
			if( hWndScrollingTarget != NULL && ::IsWindow( hWndScrollingTarget ) )
				hWndInvalidate = hWndScrollingTarget;
		}
		if( hWndInvalidate != NULL )
		{
			::InvalidateRect( hWndInvalidate, NULL, TRUE );
			::UpdateWindow( hWndInvalidate );
		}
	}
}

void CExtScrollBar::OnScrollEdgeLower()
{
	ASSERT_VALID( this );
	if( ! m_bEnableCtxMenu )
		return;
DWORD dwStyle = GetStyle();
	if( ( dwStyle & SBS_SIZEBOX ) != 0 )
		return;
SCROLLINFO scrollInfo;
	::memset( &scrollInfo, 0, sizeof(SCROLLINFO) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;
	if( ! GetScrollInfo( &scrollInfo ) )
		return;
INT nRange = scrollInfo.nMax - scrollInfo.nMin;
	if( nRange <= 0 || nRange <= INT(scrollInfo.nPage) )
		return;
INT nPosOld = INT( GetScrollPos() ), nPosNew = scrollInfo.nMin;
	nPosNew = max( nPosNew, scrollInfo.nMin );
	nPosNew = min( nPosNew, scrollInfo.nMax );
	if( nPosOld != nPosNew )
	{
		HWND hWndInvalidate = ::GetParent( m_hWnd ), hWndOwn = m_hWnd;
		_SetScrollPos( nPosNew );
		if( ! ::IsWindow( hWndOwn ) )
			return;
		CExtNCSB_ScrollContainer * pScrollContainer = _GetParentScrollContainer();
		if( pScrollContainer != NULL )
		{
			HWND hWndScrollingTarget = pScrollContainer->GetSafeScrollingTargetHWND();
			if( hWndScrollingTarget != NULL && ::IsWindow( hWndScrollingTarget ) )
				hWndInvalidate = hWndScrollingTarget;
		}
		if( hWndInvalidate != NULL )
		{
			::InvalidateRect( hWndInvalidate, NULL, TRUE );
			::UpdateWindow( hWndInvalidate );
		}
	}
}

void CExtScrollBar::OnScrollEdgeUpper()
{
	ASSERT_VALID( this );
	if( ! m_bEnableCtxMenu )
		return;
DWORD dwStyle = GetStyle();
	if( ( dwStyle & SBS_SIZEBOX ) != 0 )
		return;
SCROLLINFO scrollInfo;
	::memset( &scrollInfo, 0, sizeof(SCROLLINFO) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;
	if( ! GetScrollInfo( &scrollInfo ) )
		return;
INT nRange = scrollInfo.nMax - scrollInfo.nMin;
	if( nRange <= 0 || nRange <= INT(scrollInfo.nPage) )
		return;
INT nPosOld = INT( GetScrollPos() ), nPosNew = scrollInfo.nMax;
	nPosNew = max( nPosNew, scrollInfo.nMin );
	nPosNew = min( nPosNew, scrollInfo.nMax );
	if( nPosOld != nPosNew )
	{
		HWND hWndInvalidate = ::GetParent( m_hWnd ), hWndOwn = m_hWnd;
		_SetScrollPos( nPosNew );
		if( ! ::IsWindow( hWndOwn ) )
			return;
		CExtNCSB_ScrollContainer * pScrollContainer = _GetParentScrollContainer();
		if( pScrollContainer != NULL )
		{
			HWND hWndScrollingTarget = pScrollContainer->GetSafeScrollingTargetHWND();
			if( hWndScrollingTarget != NULL && ::IsWindow( hWndScrollingTarget ) )
				hWndInvalidate = hWndScrollingTarget;
		}
		if( hWndInvalidate != NULL )
		{
			::InvalidateRect( hWndInvalidate, NULL, TRUE );
			::UpdateWindow( hWndInvalidate );
		}
	}
}

void CExtScrollBar::OnScrollLineLower()
{
	ASSERT_VALID( this );
	if( ! m_bEnableCtxMenu )
		return;
DWORD dwStyle = GetStyle();
	if( ( dwStyle & SBS_SIZEBOX ) != 0 )
		return;
SCROLLINFO scrollInfo;
	::memset( &scrollInfo, 0, sizeof(SCROLLINFO) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;
	if( ! GetScrollInfo( &scrollInfo ) )
		return;
INT nRange = scrollInfo.nMax - scrollInfo.nMin;
	if( nRange <= 0 || nRange <= INT(scrollInfo.nPage) )
		return;
INT nStepSize = max( m_nStepSize, 1 );
INT nPosOld = INT( GetScrollPos() );
INT nPosNew = nPosOld - nStepSize;
	nPosNew = max( nPosNew, scrollInfo.nMin );
	nPosNew = min( nPosNew, scrollInfo.nMax );
	if( nPosOld != nPosNew )
	{
		HWND hWndInvalidate = ::GetParent( m_hWnd ), hWndOwn = m_hWnd;
		_SetScrollPos( nPosNew );
		if( ! ::IsWindow( hWndOwn ) )
			return;
		CExtNCSB_ScrollContainer * pScrollContainer = _GetParentScrollContainer();
		if( pScrollContainer != NULL )
		{
			HWND hWndScrollingTarget = pScrollContainer->GetSafeScrollingTargetHWND();
			if( hWndScrollingTarget != NULL && ::IsWindow( hWndScrollingTarget ) )
				hWndInvalidate = hWndScrollingTarget;
		}
		if( hWndInvalidate != NULL )
		{
			::InvalidateRect( hWndInvalidate, NULL, TRUE );
			::UpdateWindow( hWndInvalidate );
		}
	}
}

void CExtScrollBar::OnScrollLineUpper()
{
	ASSERT_VALID( this );
	if( ! m_bEnableCtxMenu )
		return;
DWORD dwStyle = GetStyle();
	if( ( dwStyle & SBS_SIZEBOX ) != 0 )
		return;
SCROLLINFO scrollInfo;
	::memset( &scrollInfo, 0, sizeof(SCROLLINFO) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;
	if( ! GetScrollInfo( &scrollInfo ) )
		return;
INT nRange = scrollInfo.nMax - scrollInfo.nMin;
	if( nRange <= 0 || nRange <= INT(scrollInfo.nPage) )
		return;
INT nStepSize = max( m_nStepSize, 1 );
INT nPosOld = INT( GetScrollPos() );
INT nPosNew = nPosOld + nStepSize;
	nPosNew = max( nPosNew, scrollInfo.nMin );
	nPosNew = min( nPosNew, scrollInfo.nMax );
	if( nPosOld != nPosNew )
	{
		HWND hWndInvalidate = ::GetParent( m_hWnd ), hWndOwn = m_hWnd;
		_SetScrollPos( nPosNew );
		if( ! ::IsWindow( hWndOwn ) )
			return;
		CExtNCSB_ScrollContainer * pScrollContainer = _GetParentScrollContainer();
		if( pScrollContainer != NULL )
		{
			HWND hWndScrollingTarget = pScrollContainer->GetSafeScrollingTargetHWND();
			if( hWndScrollingTarget != NULL && ::IsWindow( hWndScrollingTarget ) )
				hWndInvalidate = hWndScrollingTarget;
		}
		if( hWndInvalidate != NULL )
		{
			::InvalidateRect( hWndInvalidate, NULL, TRUE );
			::UpdateWindow( hWndInvalidate );
		}
	}
}

void CExtScrollBar::OnScrollPageLower()
{
	ASSERT_VALID( this );
	if( ! m_bEnableCtxMenu )
		return;
DWORD dwStyle = GetStyle();
	if( ( dwStyle & SBS_SIZEBOX ) != 0 )
		return;
SCROLLINFO scrollInfo;
	::memset( &scrollInfo, 0, sizeof(SCROLLINFO) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;
	if( ! GetScrollInfo( &scrollInfo ) )
		return;
INT nRange = scrollInfo.nMax - scrollInfo.nMin;
	if( nRange <= 0 || nRange <= INT(scrollInfo.nPage) )
		return;
INT nPosOld = INT( GetScrollPos() );
INT nPosNew = nPosOld - scrollInfo.nPage;
	nPosNew = max( nPosNew, scrollInfo.nMin );
	nPosNew = min( nPosNew, scrollInfo.nMax );
	if( nPosOld != nPosNew )
	{
		HWND hWndInvalidate = ::GetParent( m_hWnd ), hWndOwn = m_hWnd;
		_SetScrollPos( nPosNew );
		if( ! ::IsWindow( hWndOwn ) )
			return;
		CExtNCSB_ScrollContainer * pScrollContainer = _GetParentScrollContainer();
		if( pScrollContainer != NULL )
		{
			HWND hWndScrollingTarget = pScrollContainer->GetSafeScrollingTargetHWND();
			if( hWndScrollingTarget != NULL && ::IsWindow( hWndScrollingTarget ) )
				hWndInvalidate = hWndScrollingTarget;
		}
		if( hWndInvalidate != NULL )
		{
			::InvalidateRect( hWndInvalidate, NULL, TRUE );
			::UpdateWindow( hWndInvalidate );
		}
	}
}

void CExtScrollBar::OnScrollPageUpper()
{
	ASSERT_VALID( this );
	if( ! m_bEnableCtxMenu )
		return;
DWORD dwStyle = GetStyle();
	if( ( dwStyle & SBS_SIZEBOX ) != 0 )
		return;
SCROLLINFO scrollInfo;
	::memset( &scrollInfo, 0, sizeof(SCROLLINFO) );
	scrollInfo.cbSize = sizeof( SCROLLINFO );
	scrollInfo.fMask = SIF_ALL;
	if( ! GetScrollInfo( &scrollInfo ) )
		return;
INT nRange = scrollInfo.nMax - scrollInfo.nMin;
	if( nRange <= 0 || nRange <= INT(scrollInfo.nPage) )
		return;
INT nPosOld = INT( GetScrollPos() );
INT nPosNew = nPosOld + scrollInfo.nPage;
	nPosNew = max( nPosNew, scrollInfo.nMin );
	nPosNew = min( nPosNew, scrollInfo.nMax );
	if( nPosOld != nPosNew )
	{
		HWND hWndInvalidate = ::GetParent( m_hWnd ), hWndOwn = m_hWnd;
		_SetScrollPos( nPosNew );
		if( ! ::IsWindow( hWndOwn ) )
			return;
		CExtNCSB_ScrollContainer * pScrollContainer = _GetParentScrollContainer();
		if( pScrollContainer != NULL )
		{
			HWND hWndScrollingTarget = pScrollContainer->GetSafeScrollingTargetHWND();
			if( hWndScrollingTarget != NULL && ::IsWindow( hWndScrollingTarget ) )
				hWndInvalidate = hWndScrollingTarget;
		}
		if( hWndInvalidate != NULL )
		{
			::InvalidateRect( hWndInvalidate, NULL, TRUE );
			::UpdateWindow( hWndInvalidate );
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CExtZoomScrollBar

IMPLEMENT_DYNCREATE( CExtZoomScrollBar, CExtScrollBar );

CExtZoomScrollBar::CExtZoomScrollBar()
{
}

CExtZoomScrollBar::~CExtZoomScrollBar()
{
}

BEGIN_MESSAGE_MAP( CExtZoomScrollBar, CExtScrollBar )
	//{{AFX_MSG_MAP(CExtZoomScrollBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// class CExtNCSB_ScrollContainer

IMPLEMENT_DYNAMIC( CExtNCSB_ScrollContainer, CWnd );

CExtNCSB_ScrollContainer::CExtNCSB_ScrollContainer(
	HWND hWndScrollingTaget,
	CExtNCSB_ScrollContainer::e_mode_t eMode
	)
	: m_hWndScrollingTaget( hWndScrollingTaget )
	, m_eMode( eMode )
	, m_pWndScrollBar( NULL )
{
	ASSERT( m_hWndScrollingTaget != NULL && ::IsWindow( m_hWndScrollingTaget ) );
	ASSERT( INT(__EM_MIN_VALUE) <= INT(m_eMode) && INT(m_eMode) <= INT(__EM_MAX_VALUE) );
}

CExtNCSB_ScrollContainer::~CExtNCSB_ScrollContainer()
{
}

CExtScrollBar * CExtNCSB_ScrollContainer::GetScrollBarInContainer() const
{
	ASSERT_VALID( this );
	return m_pWndScrollBar;
}

CExtNCSB_ScrollContainer::e_mode_t CExtNCSB_ScrollContainer::GetMode() const
{
	ASSERT_VALID( this );
	ASSERT( INT(__EM_MIN_VALUE) <= INT(m_eMode) && INT(m_eMode) <= INT(__EM_MAX_VALUE) );
	return m_eMode;
}

HWND CExtNCSB_ScrollContainer::GetSafeScrollingTargetHWND() const
{
	ASSERT_VALID( this );
//	if( GetSafeHwnd() == NULL )
//		return NULL;
	if( m_hWndScrollingTaget == NULL || (! ::IsWindow( m_hWndScrollingTaget ) ) )
		return NULL;
	return m_hWndScrollingTaget;
}

CExtScrollBar * CExtNCSB_ScrollContainer::OnInstantiateAndCreateScrollBar( bool bChildMode )
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return NULL;
e_mode_t eMode = GetMode();
	if( ! ( eMode == __EM_HORIZONTAL_SCROLL_BAR || eMode == __EM_VERTICAL_SCROLL_BAR ) )
		return NULL;
CExtScrollBar * pWndScrollBar = NULL;
	try
	{
		TCHAR szCompare[512] = _T("");
		::GetClassName( m_hWndScrollingTaget, szCompare, sizeof( szCompare ) / sizeof( szCompare[0] ) );
		pWndScrollBar = new CExtScrollBar;
		if(		_tcsicmp(  szCompare, _T("edit") ) == 0
			||	_tcsnicmp( szCompare, _T("riched"), 6 ) == 0
			||	_tcsicmp(  szCompare, _T("listbox") ) == 0
			||	_tcsicmp(  szCompare, _T("combolbox") ) == 0
			||	_tcsicmp(  szCompare, _T("systreeview32") ) == 0
			||	_tcsicmp(  szCompare, _T("syslistview32") ) == 0
			)
		{
			pWndScrollBar->m_bAlwaysSendThumbButtonNotifications = true;
			pWndScrollBar->m_bSendActionNotifications = false;
		}
		if( ! bChildMode )
		{
			pWndScrollBar->m_bPopupInactiveLightMode = true;
			pWndScrollBar->m_bEnableHookSpy = true;
			pWndScrollBar->m_nMouseActivateCode = MA_NOACTIVATE;
		}
		CRect rc;
		GetClientRect( &rc );
		if( ! pWndScrollBar->Create(
				WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS
					| ( ( eMode == __EM_HORIZONTAL_SCROLL_BAR ) ? SBS_HORZ : SBS_VERT )
					,
				rc,
				this,
				UINT(__EXT_MFC_IDC_STATIC)
				)
			)
			return NULL;
		return pWndScrollBar;
	}
	catch( CException * pException )
	{
		pException->Delete();
	}
	catch( ... )
	{
	}
	if( pWndScrollBar != NULL )
		delete pWndScrollBar;
	return NULL;
}

bool CExtNCSB_ScrollContainer::Create( CWnd * pWndParent )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pWndParent );
	ASSERT( pWndParent->GetSafeHwnd() != NULL );

HWND hWndScrollingTaget = GetSafeScrollingTargetHWND();
	if( hWndScrollingTaget == NULL || (! ::IsWindow( hWndScrollingTaget ) ) )
		return false;

WNDCLASS _wndClassInfo;
HINSTANCE hInst = ::AfxGetInstanceHandle();
	if( ! ::GetClassInfo( hInst, __EXT_NCSB_SCROLL_CONTAINER_CLASS_NAME, &_wndClassInfo ) )
	{
		_wndClassInfo.style = CS_GLOBALCLASS|CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW;
		_wndClassInfo.lpfnWndProc = ::DefWindowProc;
		_wndClassInfo.cbClsExtra = _wndClassInfo.cbWndExtra = 0;
		_wndClassInfo.hInstance = hInst;
		_wndClassInfo.hIcon = NULL;
		_wndClassInfo.hCursor = ::LoadCursor( NULL, IDC_ARROW );
		ASSERT( _wndClassInfo.hCursor != NULL );
		_wndClassInfo.hbrBackground = NULL; 
		_wndClassInfo.lpszMenuName = NULL;
		_wndClassInfo.lpszClassName = __EXT_NCSB_SCROLL_CONTAINER_CLASS_NAME;
		if( ! ::AfxRegisterClass( &_wndClassInfo ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}

e_mode_t eMode = GetMode();
static RECT rcInitWndPos = { 0, 0, 0, 0 };
UINT nID = UINT(__EXT_MFC_IDC_STATIC);
CRect rc = rcInitWndPos;
__EXT_MFC_LONG_PTR dwScrollingTargetStyle = ::__EXT_MFC_GetWindowLong( hWndScrollingTaget, GWL_STYLE );
bool bChildMode = ( (dwScrollingTargetStyle&WS_CHILD) != 0 ) ? true : false;
	if( bChildMode && ::GetParent(m_hWndScrollingTaget) == ::GetDesktopWindow() )
		bChildMode = false;
DWORD dwStyle = WS_CLIPCHILDREN|WS_CLIPSIBLINGS, dwExStyle = WS_EX_NOINHERITLAYOUT;
	if( bChildMode )
	{
		if( eMode == __EM_MIDDLE_CONTAINER )
		{
			::GetWindowRect( hWndScrollingTaget, &rc );
			pWndParent->ScreenToClient( &rc );
			nID = (UINT)::__EXT_MFC_GetWindowLong( hWndScrollingTaget, GWL_ID );
			if( (dwScrollingTargetStyle&WS_VISIBLE) != 0 )
				dwStyle |= WS_VISIBLE;
			dwExStyle |= WS_EX_CONTROLPARENT;
		}
		dwStyle |= WS_CHILD;
	}
	else
	{
		ASSERT( eMode != __EM_MIDDLE_CONTAINER );
		dwStyle |= WS_POPUP;
		nID = (UINT)NULL;
	}

	if( ! CWnd::CreateEx(
			dwExStyle,
			__EXT_NCSB_SCROLL_CONTAINER_CLASS_NAME,
			__EXT_NCSB_SCROLL_CONTAINER_CLASS_NAME,
			dwStyle,
			rc,
			pWndParent,
			nID
			)
		)
		return false;

	if( eMode == __EM_HORIZONTAL_SCROLL_BAR || eMode == __EM_VERTICAL_SCROLL_BAR )
	{
		m_pWndScrollBar = OnInstantiateAndCreateScrollBar( bChildMode );
		if( m_pWndScrollBar == NULL )
		{
			DestroyWindow();
			return false;
		}
	}
	else if( eMode == __EM_MIDDLE_CONTAINER )
	{
		::SetWindowPos(
			m_hWnd, hWndScrollingTaget, 0, 0, 0, 0,
			SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOOWNERZORDER|SWP_NOREDRAW|SWP_NOSENDCHANGING
			);
		::SetParent( hWndScrollingTaget, m_hWnd );
		CRect rcClient;
		GetClientRect( &rcClient );
		::MoveWindow( hWndScrollingTaget, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), TRUE );
		bool bEnable = ( (dwScrollingTargetStyle&WS_DISABLED) != 0 ) ? FALSE : TRUE;
		EnableWindow( bEnable );
	}
	return true;
}

void CExtNCSB_ScrollContainer::PreSubclassWindow()
{
	ASSERT_VALID( this );
	CWnd::PreSubclassWindow();
}

void CExtNCSB_ScrollContainer::PostNcDestroy()
{
	ASSERT_VALID( this );
CExtScrollBar * pWndScrollBar = GetScrollBarInContainer();
	if( pWndScrollBar != NULL )
	{
		if( pWndScrollBar->GetSafeHwnd() != NULL )
			pWndScrollBar->DestroyWindow();
		delete pWndScrollBar;
	}
	CWnd::PostNcDestroy();
	delete this;
}

LRESULT CExtNCSB_ScrollContainer::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_NCCALCSIZE:
		return 0L;
	case WM_PAINT:
	{
		CPaintDC dcPaint( this );
		CRect rcClient;
		GetClientRect( &rcClient );
		CExtMemoryDC dc( &dcPaint, &rcClient );
		g_PaintManager->PaintDockerBkgnd( true, dc, this );
		return 0L;
	}
	case WM_SIZE:
	{
		e_mode_t eMode = GetMode();
		if( eMode == __EM_HORIZONTAL_SCROLL_BAR || eMode == __EM_VERTICAL_SCROLL_BAR )
		{
			CExtScrollBar * pWndScrollBar = GetScrollBarInContainer();
			if( pWndScrollBar->GetSafeHwnd() != NULL )
				pWndScrollBar->MoveWindow( 0, 0, LOWORD(DWORD(lParam)), HIWORD(DWORD(lParam)) );
		}
		else if( eMode == __EM_MIDDLE_CONTAINER )
		{
			HWND hWndScrollingTaget = GetSafeScrollingTargetHWND();
			if( hWndScrollingTaget != NULL )
			{
				CRect rcClient;
				GetClientRect( &rcClient );
				::MoveWindow( hWndScrollingTaget, rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), TRUE );
			}
		}
	}
	break;
	case WM_HSCROLL:
	case WM_VSCROLL:
	{
		e_mode_t eMode = GetMode();
		if( eMode == __EM_HORIZONTAL_SCROLL_BAR || eMode == __EM_VERTICAL_SCROLL_BAR )
		{
			HWND hWndScrollingTaget = GetSafeScrollingTargetHWND();
			if( hWndScrollingTaget != NULL )
				return ::SendMessage( hWndScrollingTaget, message, wParam, NULL );
		}
	}
	return 0L;
	} // switch( message )
LRESULT lResult = CWnd::WindowProc( message, wParam, lParam );
	switch( message )
	{
	case WM_SHOWWINDOW:
	case WM_ENABLE:
	{
		e_mode_t eMode = GetMode();
		if( eMode == __EM_MIDDLE_CONTAINER )
		{
			HWND hWndScrollingTaget = GetSafeScrollingTargetHWND();
			if( hWndScrollingTaget != NULL )
			{
				DWORD dwStyle = GetStyle();
				bool bEnable = ( (dwStyle&WS_DISABLED) != 0 ) ? FALSE : TRUE;
				::EnableWindow( hWndScrollingTaget, bEnable );
				//int nCmdShow = ( (dwStyle&WS_VISIBLE) != 0 ) ? SW_SHOWNOACTIVATE : SW_HIDE;
				//::ShowWindow( hWndScrollingTaget, nCmdShow );
			}
		}
	}
	break;
	case WM_SETFOCUS:
	{
		e_mode_t eMode = GetMode();
		if( eMode == __EM_MIDDLE_CONTAINER )
		{
			HWND hWndScrollingTaget = GetSafeScrollingTargetHWND();
			if( hWndScrollingTaget != NULL )
				::SetFocus( hWndScrollingTaget );
		}
	}
	break;
	} // switch( message )
	return lResult;
}

CExtNCSB < CListCtrl > :: CExtNCSB(
	bool bNcsbDelayedInitialization, // = false
	bool bNcsbForceMiddleContainerMode // = false
	)
	: CExtNCSB_Impl < CExtANCSB < CListCtrl > > (
		bNcsbDelayedInitialization,
		bNcsbForceMiddleContainerMode
		)
{
}

CExtNCSB < CListCtrl > :: ~CExtNCSB()
{
}

void CExtNCSB < CListCtrl > :: AdjustScrollMetrics()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
DWORD dwType = DWORD( GetStyle() & LVS_TYPEMASK );
CExtNCSB_ScrollContainer * pWndH = NCSB_GetContainer( CExtNCSB_ScrollContainer::__EM_HORIZONTAL_SCROLL_BAR );
CExtNCSB_ScrollContainer * pWndV = NCSB_GetContainer( CExtNCSB_ScrollContainer::__EM_VERTICAL_SCROLL_BAR );
CExtScrollBar * pSBH = ( pWndH != NULL ) ? pWndH->GetScrollBarInContainer() : NULL;
CExtScrollBar * pSBV = ( pWndV != NULL ) ? pWndV->GetScrollBarInContainer() : NULL;
INT m_nStepSizeH = -1, m_nStepSizeV = -1;
	switch( dwType )
	{
	case LVS_ICON:
		m_nStepSizeV = 64;
	break;
	case LVS_SMALLICON:
	case LVS_LIST:
		m_nStepSizeV = 16;
	break;
	case LVS_REPORT:
		m_nStepSizeV = 1;
	break;
	} // switch( dwType )
	if( m_nStepSizeH > 0 && pSBH != NULL )
		pSBH->m_nStepSize = m_nStepSizeH;
	if( m_nStepSizeV > 0 && pSBV != NULL )
		pSBV->m_nStepSize = m_nStepSizeV;
}

LRESULT CExtNCSB < CListCtrl > :: WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_HSCROLL:
	{
		UINT nSBCode = UINT(LOWORD(DWORD(wParam))), nPos = UINT(HIWORD(DWORD(wParam)));
		//TRACE2( "WM_HSCROLL, nSBCode = %d, nPos = %d\r\n", nSBCode, nPos );
		INT nItemExtent = 1;
		DWORD dwStyle = GetStyle();
		DWORD dwListCtrlType = dwStyle&LVS_TYPEMASK;
		switch( dwListCtrlType )
		{
		case LVS_ICON:
		case LVS_SMALLICON:
		case LVS_REPORT:
		break;
		case LVS_LIST:
		{
			CRect rcItem( 0, 0, 0, 0 );
			INT nTopIndex = GetTopIndex();
			if(		nTopIndex >= 0
				&&	CListCtrl::GetItemRect( nTopIndex, &rcItem, LVIR_BOUNDS )
				)
				nItemExtent = rcItem.Width();
		}
		break;
		} // switch( dwListCtrlType )
		if( nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION )
		{
			INT nPosOld = GetScrollPos( SB_HORZ );
			INT nPosShift = nPos*nItemExtent - nPosOld*nItemExtent;
			Scroll( CSize( nPosShift, 0 ) );
			return 0L;
		}
		if(		nSBCode == SB_LINELEFT
			||	nSBCode == SB_LINERIGHT
			||	nSBCode == SB_PAGELEFT
			||	nSBCode == SB_PAGERIGHT
			||	nSBCode == SB_LEFT
			||	nSBCode == SB_RIGHT
			||	nSBCode == SB_ENDSCROLL
			)
			return 0L;
	} // case WM_HSCROLL:
	break; // case WM_VSCROLL
	case WM_VSCROLL:
	{
		UINT nSBCode = UINT(LOWORD(DWORD(wParam))), nPos = UINT(HIWORD(DWORD(wParam)));
		//TRACE2( "WM_VSCROLL, nSBCode = %d, nPos = %d\r\n", nSBCode, nPos );
		INT nItemExtent = 1;
		DWORD dwStyle = GetStyle();
		DWORD dwListCtrlType = dwStyle&LVS_TYPEMASK;
		switch( dwListCtrlType )
		{
		case LVS_ICON:
		case LVS_SMALLICON:
		case LVS_LIST:
		break;
		case LVS_REPORT:
		{
			CRect rcItem( 0, 0, 0, 0 );
			INT nTopIndex = GetTopIndex();
			if(		nTopIndex >= 0
				&&	CListCtrl::GetItemRect( nTopIndex, &rcItem, LVIR_BOUNDS )
				)
				nItemExtent = rcItem.Height();
			else
			{
				TEXTMETRIC _tm;
				::memset( &_tm, 0, sizeof(TEXTMETRIC) );
				CClientDC dc( this );
				CFont * pFont = GetFont();
				int nSave = dc.SaveDC();
				dc.SelectObject( pFont );
				dc.GetTextMetrics( &_tm );
				nItemExtent = _tm.tmHeight + _tm.tmExternalLeading + 1;
				dc.RestoreDC( nSave );
			}
		}
		break;
		} // switch( dwListCtrlType )
		if( nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION )
		{
			INT nPosOld = GetScrollPos( SB_VERT );
			INT nPosShift = nPos*nItemExtent - nPosOld*nItemExtent;
			Scroll( CSize( 0, nPosShift ) );
			return 0L;
		}
		if(		nSBCode == SB_LINEUP
			||	nSBCode == SB_LINEDOWN
			||	nSBCode == SB_PAGEUP
			||	nSBCode == SB_PAGEDOWN
			||	nSBCode == SB_TOP
			||	nSBCode == SB_BOTTOM
			||  nSBCode ==  SB_ENDSCROLL
			)
			return 0L;
	}
	break; // case WM_VSCROLL
	} // switch( message )
LRESULT lResult = CExtNCSB_Impl < CExtANCSB < CListCtrl > > :: WindowProc( message, wParam, lParam );
	switch( message )
	{
	case WM_STYLECHANGED:
		if( wParam == GWL_STYLE )
		{
			STYLESTRUCT * ptr = (STYLESTRUCT *)lParam;
			ASSERT( ptr != NULL );
			DWORD dwOldType = DWORD( ptr->styleOld & LVS_TYPEMASK );
			DWORD dwNewType = DWORD( ptr->styleNew & LVS_TYPEMASK );
			if( dwOldType != dwNewType )
				AdjustScrollMetrics();
		} // if( wParam == GWL_STYLE )
	break; // case WM_STYLECHANGED
	} // switch( message )
	return lResult;
}

#if (!defined __EXT_MFC_NO_SCROLLWND)

/////////////////////////////////////////////////////////////////////////////
// CExtScrollWnd

IMPLEMENT_DYNCREATE( CExtScrollWnd, CWnd );
IMPLEMENT_CExtPmBridge_MEMBERS( CExtScrollWnd );

CExtScrollWnd::CExtScrollWnd()
	: m_nUpdateScrollBars( 0 )
	, m_bScrollPhysical( false )
	, m_bEatErasing( true )
	, m_bClientCB( false )
	, m_bBufferedPainting( true )
	, m_bScrollUpdateWindow( false )
	, m_bScrollInvalidate( true )
	, m_bScrollErase( false )
	, m_bRedrawUpdateWindow( false )
	, m_bRedrawInvalidate( true )
	, m_bRedrawErase( false )
	, m_bUse32BitScrollInfo( true )
	, m_hFont( NULL )
	, m_bDisplayPopOverScrollBars( false )
	, m_bLightPopOverScrollBars( false )
	, m_bPopOverScrollBarsVisible( false )
	, m_bPopOverScrollBarsTracking( false )
	, m_bHideNormalScrollBars( false )
	, m_nPopOverScrollBarTimerID( 399 )
	, m_nPopOverScrollBarTimerEllapse( 2000 )
{
	PmBridge_Install();
}

CExtScrollWnd::~CExtScrollWnd()
{
	PmBridge_Uninstall();
}


BEGIN_MESSAGE_MAP( CExtScrollWnd, CWnd )
	//{{AFX_MSG_MAP(CExtScrollWnd)
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_ENABLE()
	ON_MESSAGE( WM_SETFONT, OnSetFont )
	ON_MESSAGE( WM_GETFONT, OnGetFont )	
	ON_WM_NCCALCSIZE()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_SYSCOLORCHANGE()
	__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_MESSAGE(__ExtMfc_WM_THEMECHANGED, OnThemeChanged)
END_MESSAGE_MAP()

void CExtScrollWnd::OnEnable( BOOL bEnable )
{
	ASSERT_VALID( this );
	if( bEnable )
		OnSwUpdateScrollBars();
	else
		OnSwEnableScrollBarCtrl( SB_BOTH, false );
	OnSwRecalcLayout( true );
 	RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_ALLCHILDREN|RDW_FRAME );
}

LRESULT CExtScrollWnd::OnSetFont( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
HFONT hFont = (HFONT) wParam;
BOOL bRedraw = (BOOL) lParam;
	m_hFont = hFont; 
	OnSwRecalcLayout( true );
    if( bRedraw )
        Invalidate();
	return 0L;
}
  
LRESULT CExtScrollWnd::OnGetFont( WPARAM, LPARAM )
{
	ASSERT_VALID( this );
    return (LRESULT) m_hFont;
}

void CExtScrollWnd::OnSysColorChange() 
{
	ASSERT_VALID( this );
	CWnd::OnSysColorChange();
//CExtPaintManager * pPM = PmBridge_GetPM();
//	ASSERT_VALID( pPM );
//	g_PaintManager.OnSysColorChange( this );
//	g_CmdManager.OnSysColorChange( pPM, this );
	Invalidate();
}

LRESULT CExtScrollWnd::OnDisplayChange( WPARAM wParam, LPARAM lParam )
{
//LRESULT lResult = CWnd::OnDisplayChange( wParam, lParam );

CWnd::OnDisplayChange(wParam, LOWORD(lParam), HIWORD(lParam));

LRESULT lResult = 0;


//CExtPaintManager * pPM = PmBridge_GetPM();
//	ASSERT_VALID( pPM );
//	g_PaintManager.OnDisplayChange( this, (INT)wParam, CPoint(lParam) );
//	g_CmdManager.OnDisplayChange( pPM, this, (INT)wParam, CPoint(lParam) );
	Invalidate();
	return lResult;
}

LRESULT CExtScrollWnd::OnThemeChanged( WPARAM wParam, LPARAM lParam )
{
	wParam;
	lParam;
LRESULT lResult = Default();
//CExtPaintManager * pPM = PmBridge_GetPM();
//	ASSERT_VALID( pPM );
//	g_PaintManager.OnThemeChanged( this, wParam, lParam );
//	g_CmdManager.OnThemeChanged( pPM, this, wParam, lParam );
	OnSwRecalcLayout( true );
	Invalidate();
	return lResult;
}

void CExtScrollWnd::OnSettingChange( UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection ) 
{
	ASSERT_VALID( this );
	CWnd::OnSettingChange(uFlags, lpszSection);
//CExtPaintManager * pPM = PmBridge_GetPM();
//	ASSERT_VALID( pPM );
//	g_PaintManager.OnSettingChange( this, uFlags, lpszSection );
//	g_CmdManager.OnSettingChange( pPM, this, uFlags, lpszSection );
	OnSwRecalcLayout( true );
	Invalidate();
}

void CExtScrollWnd::PmBridge_OnPaintManagerChanged(
	CExtPaintManager * pGlobalPM
	)
{
	ASSERT_VALID( this );
	CExtPmBridge::PmBridge_OnPaintManagerChanged( pGlobalPM );
	if( GetSafeHwnd() != NULL )
		OnSwRecalcLayout( true );
}

CSize CExtScrollWnd::OnSwGetTotalSize() const
{
	ASSERT_VALID( this );
	return CSize( 0, 0 );
}

CSize CExtScrollWnd::OnSwGetPageSize( int nDirection ) const
{
	ASSERT_VALID( this );
	nDirection;
	return CSize( 0, 0 );
}

CSize CExtScrollWnd::OnSwGetLineSize( int nDirection ) const
{
	ASSERT_VALID( this );
	nDirection;
	return CSize( 0, 0 );
}

bool CExtScrollWnd::OnSwScrollInfoAdjust(
	int nBar,
	SCROLLINFO & si,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nBar == SB_HORZ || nBar == SB_VERT );
CScrollBar * pScrollBarWnd = GetScrollBarCtrl( nBar );
	if( pScrollBarWnd != NULL )
	{
		if( pScrollBarWnd->SetScrollInfo(
				&si,
				bRedraw ? TRUE : FALSE
				)
			)
			return true;
		return false;
	} // if( pScrollBarWnd != NULL )
	if( CWnd::SetScrollInfo(
			nBar,
			&si,
			bRedraw ? TRUE : FALSE
			)
		)
		return true;
	return false;
}

void CExtScrollWnd::OnSwSetScrollRange(
	int nBar,
	LONG nMinPos,
	LONG nMaxPos,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nBar == SB_HORZ || nBar == SB_VERT );
CScrollBar * pScrollBarWnd = GetScrollBarCtrl( nBar );
	if( pScrollBarWnd != NULL )
	{
		pScrollBarWnd->SetScrollRange(
			(int)nMinPos,
			(int)nMaxPos,
			bRedraw ? TRUE : FALSE
			);
		return;
	} // if( pScrollBarWnd != NULL )
	CWnd::SetScrollRange(
		nBar,
		(int)nMinPos,
		(int)nMaxPos,
		bRedraw ? TRUE : FALSE
		);
}

LONG CExtScrollWnd::ScrollLimit32Get( int nBar ) const
{
	ASSERT_VALID( this );
	ASSERT( nBar == SB_HORZ || nBar == SB_VERT );
CScrollBar * pScrollBarWnd = GetScrollBarCtrl( nBar );
	if( pScrollBarWnd != NULL )
	{
		if( m_bUse32BitScrollInfo )
		{
			SCROLLINFO _scroll_info;
			::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
			_scroll_info.cbSize = sizeof(SCROLLINFO);
			if(	pScrollBarWnd->GetScrollInfo(
					&_scroll_info,
					SIF_RANGE|SIF_PAGE
					)
				)
			{
				LONG nRetVal = (LONG)
					(_scroll_info.nMax
					- _scroll_info.nMin
					- _scroll_info.nPage
					+ 1);
				ASSERT( nRetVal >= 0 );
				return nRetVal;
			}
			//ASSERT( FALSE );
		} // if( m_bUse32BitScrollInfo )
		LONG nRetVal = (LONG)
			pScrollBarWnd->GetScrollLimit();
		return nRetVal;
	} // if( pScrollBarWnd != NULL )
	if( m_bUse32BitScrollInfo )
	{
		SCROLLINFO _scroll_info;
		::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
		_scroll_info.cbSize = sizeof(SCROLLINFO);
		if(	( const_cast < CExtScrollWnd * > ( this ) ) ->
				CWnd::GetScrollInfo(
					nBar,
					&_scroll_info,
					SIF_RANGE|SIF_PAGE
					)
			)
		{
			LONG nRetVal = (LONG)
				(_scroll_info.nMax
				- _scroll_info.nMin
				- _scroll_info.nPage
				+ 1);
			ASSERT( nRetVal >= 0 );
			return nRetVal;
		}
		//ASSERT( FALSE );
	} // if( m_bUse32BitScrollInfo )
LONG nRetVal = (LONG)
		(	( const_cast < CExtScrollWnd * > ( this ) ) ->
				CWnd::GetScrollLimit( nBar )
		);
	return nRetVal;
}

void CExtScrollWnd::ScrollInfo32Get(
	int nBar,
	LONG * p_nMinPos,
	LONG * p_nMaxPos,
	LONG * p_nPageSize,
	LONG * p_nCurrPos,
	LONG * p_nTrackPos
	) const
{
	ASSERT_VALID( this );
	ASSERT( nBar == SB_HORZ || nBar == SB_VERT );
	if( p_nMinPos != NULL )
		*p_nMinPos = 0L;
	if( p_nMaxPos != NULL )
		*p_nMaxPos = 0L;
	if( p_nPageSize != NULL )
		*p_nPageSize = 0L;
	if( p_nCurrPos != NULL )
		*p_nCurrPos = 0L;
	if( p_nTrackPos != NULL )
		*p_nTrackPos = 0L;
CScrollBar * pScrollBarWnd = GetScrollBarCtrl( nBar );
	if( pScrollBarWnd != NULL )
	{
		if( m_bUse32BitScrollInfo )
		{
			SCROLLINFO _scroll_info;
			::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
			_scroll_info.cbSize = sizeof(SCROLLINFO);
			if(	pScrollBarWnd->GetScrollInfo(
					&_scroll_info,
					SIF_RANGE|SIF_PAGE|SIF_POS|SIF_TRACKPOS
					)
				)
			{
				if( p_nMinPos != NULL )
					*p_nMinPos = _scroll_info.nMin;
				if( p_nMaxPos != NULL )
					*p_nMaxPos = _scroll_info.nMax;
				if( p_nPageSize != NULL )
					*p_nPageSize = _scroll_info.nPage;
				if( p_nCurrPos != NULL )
					*p_nCurrPos = _scroll_info.nPos;
				if( p_nTrackPos != NULL )
					*p_nTrackPos = _scroll_info.nTrackPos;
				return;
			}
			//ASSERT( FALSE );
		} // if( m_bUse32BitScrollInfo )
		if( p_nMinPos != NULL || p_nMaxPos != NULL )
		{
			INT nMin = 0, nMax = 0;
			pScrollBarWnd->GetScrollRange( &nMin, &nMax );
			if( p_nMinPos != NULL )
				*p_nMinPos = nMin;
			if( p_nMaxPos != NULL )
				*p_nMaxPos = nMax;
			
		} // if( p_nMinPos != NULL || p_nMaxPos != NULL )
		if( p_nCurrPos != NULL || p_nTrackPos != NULL )
		{
			INT nPos = pScrollBarWnd->GetScrollPos();
			if( p_nCurrPos != NULL )
				*p_nCurrPos = nPos;
			if( p_nTrackPos != NULL )
				*p_nTrackPos = nPos;
		} // if( p_nCurrPos != NULL || p_nTrackPos != NULL )
		return;
	} // if( pScrollBarWnd != NULL )
	if( m_bUse32BitScrollInfo )
	{
		SCROLLINFO _scroll_info;
		::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
		_scroll_info.cbSize = sizeof(SCROLLINFO);
		if(	( const_cast < CExtScrollWnd * > ( this ) ) ->
				CWnd::GetScrollInfo(
					nBar,
					&_scroll_info,
					SIF_RANGE|SIF_PAGE|SIF_POS|SIF_TRACKPOS
					)
			)
		{
			if( p_nMinPos != NULL )
				*p_nMinPos = _scroll_info.nMin;
			if( p_nMaxPos != NULL )
				*p_nMaxPos = _scroll_info.nMax;
			if( p_nPageSize != NULL )
				*p_nPageSize = _scroll_info.nPage;
			if( p_nCurrPos != NULL )
				*p_nCurrPos = _scroll_info.nPos;
			if( p_nTrackPos != NULL )
				*p_nTrackPos = _scroll_info.nTrackPos;
			return;
		}
		//ASSERT( FALSE );
	} // if( m_bUse32BitScrollInfo )
	if( p_nMinPos != NULL || p_nMaxPos != NULL )
	{
		INT nMin = 0, nMax = 0;
		CWnd::GetScrollRange( nBar, &nMin, &nMax );
		if( p_nMinPos != NULL )
			*p_nMinPos = nMin;
		if( p_nMaxPos != NULL )
			*p_nMaxPos = nMax;
		
	} // if( p_nMinPos != NULL || p_nMaxPos != NULL )
	if( p_nCurrPos != NULL || p_nTrackPos != NULL )
	{
		INT nPos = CWnd::GetScrollPos( nBar );
		if( p_nCurrPos != NULL )
			*p_nCurrPos = nPos;
		if( p_nTrackPos != NULL )
			*p_nTrackPos = nPos;
	} // if( p_nCurrPos != NULL || p_nTrackPos != NULL )
	return;
}

LONG CExtScrollWnd::ScrollPos32Get(
	int nBar,
	bool bTrackPos // = false
	) const
{
	ASSERT_VALID( this );
	ASSERT( nBar == SB_HORZ || nBar == SB_VERT );
CScrollBar * pScrollBarWnd = GetScrollBarCtrl( nBar );
	if( pScrollBarWnd != NULL )
	{
//		if(		bTrackPos
//			&&	pScrollBarWnd->IsKindOf( RUNTIME_CLASS(CExtScrollBar) )
//			&&	((CExtScrollBar*)pScrollBarWnd)->m_nHelperTrackPos != -1
//			)
//			return ((CExtScrollBar*)pScrollBarWnd)->m_nHelperTrackPos;
		if( m_bUse32BitScrollInfo )
		{
			SCROLLINFO _scroll_info;
			::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
			_scroll_info.cbSize = sizeof(SCROLLINFO);
			if(	pScrollBarWnd->GetScrollInfo(
					&_scroll_info,
					bTrackPos ? SIF_TRACKPOS : SIF_POS
					)
				)
			{
				LONG nRetVal = (LONG)
					(	bTrackPos
							? _scroll_info.nTrackPos
							: _scroll_info.nPos
					);
				return nRetVal;
			}
			//ASSERT( FALSE );
		} // if( m_bUse32BitScrollInfo )
		LONG nRetVal = (LONG)
			pScrollBarWnd->GetScrollPos();
		return nRetVal;
	} // if( pScrollBarWnd != NULL )
DWORD dwWndStyle = CWnd::GetStyle();
	if(		( nBar == SB_HORZ && (dwWndStyle & WS_HSCROLL) == 0 )
		||	( nBar == SB_VERT && (dwWndStyle & WS_VSCROLL) == 0 )
		)
		return 0;
	if( m_bUse32BitScrollInfo )
	{
		SCROLLINFO _scroll_info;
		::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
		_scroll_info.cbSize = sizeof(SCROLLINFO);
		if( ( const_cast < CExtScrollWnd * > ( this ) ) ->
				CWnd::GetScrollInfo(
					nBar,
					&_scroll_info,
					bTrackPos ? SIF_TRACKPOS : SIF_POS
					)
				)
		{
			LONG nRetVal = (LONG)
				(	bTrackPos
						? _scroll_info.nTrackPos
						: _scroll_info.nPos
				);
			return nRetVal;
		}
		//ASSERT( FALSE );
	} // if( m_bUse32BitScrollInfo )
LONG nRetVal = (LONG)
		(	( const_cast < CExtScrollWnd * > ( this ) ) ->
				CWnd::GetScrollPos( nBar )
		);
	return nRetVal;
}

void CExtScrollWnd::ScrollPos32Set(
	int nBar,
	LONG nPos,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( nBar == SB_HORZ || nBar == SB_VERT );
	_HandlePopOverScrollBarVisibility( true );
CScrollBar * pScrollBarWnd = GetScrollBarCtrl( nBar );
	if( pScrollBarWnd != NULL )
	{
		if( m_bUse32BitScrollInfo )
		{
			SCROLLINFO _scroll_info;
			::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
			_scroll_info.cbSize = sizeof(SCROLLINFO);
			_scroll_info.fMask = SIF_POS;
			_scroll_info.nPos = (int)nPos;
			if( pScrollBarWnd->SetScrollInfo(
					&_scroll_info,
					bRedraw ? TRUE : FALSE
					)
				)
				return;
			//ASSERT( FALSE );
		} // if( m_bUse32BitScrollInfo )
		pScrollBarWnd->SetScrollPos(
			(int)nPos,
			bRedraw ? TRUE : FALSE
			);
		return;
	} // if( pScrollBarWnd != NULL )
	if( m_bUse32BitScrollInfo )
	{
		SCROLLINFO _scroll_info;
		::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
		_scroll_info.cbSize = sizeof(SCROLLINFO);
		_scroll_info.fMask = SIF_POS;
		_scroll_info.nPos = (int)nPos;
		if( CWnd::SetScrollInfo(
				nBar,
				&_scroll_info,
				bRedraw ? TRUE : FALSE
				)
			)
			return;
		//ASSERT( FALSE );
	} // if( m_bUse32BitScrollInfo )
	CWnd::SetScrollPos(
		nBar,
		(int)nPos,
		bRedraw ? TRUE : FALSE
		);
}

CPoint CExtScrollWnd::OnSwGetScrollPos() const
{
	ASSERT_VALID( this );
CPoint pt(
		ScrollPos32Get( SB_HORZ ),
		ScrollPos32Get( SB_VERT )
		);
	ASSERT( pt.x >= 0 && pt.y >= 0 );
//LONG xMaxValue = ScrollLimit32Get( SB_HORZ );
//LONG yMaxValue = ScrollLimit32Get( SB_VERT );
//	pt.x = min( pt.x, xMaxValue );
//	pt.y = min( pt.y, yMaxValue );
	return pt;
}

CPoint CExtScrollWnd::OnSwGetScrollPaintPos() const
{
	ASSERT_VALID( this );
	return OnSwGetScrollPos();
}

CRect CExtScrollWnd::OnSwRecalcLayout(
	bool bDoLayout,
	LPCRECT pRectClientSrc // = NULL
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return CRect( *pRectClientSrc );
	if( bDoLayout )
	{
		CScrollBar * pScrollBarWndH = GetScrollBarCtrl( SB_HORZ );
		CScrollBar * pScrollBarWndV = GetScrollBarCtrl( SB_VERT );
		if( pScrollBarWndH != NULL && pScrollBarWndV != NULL )
		{
			CExtScrollBar * pExtScrollBarWndH = DYNAMIC_DOWNCAST( CExtScrollBar, pScrollBarWndH );
			CExtScrollBar * pExtScrollBarWndV = DYNAMIC_DOWNCAST( CExtScrollBar, pScrollBarWndV );
			if( pExtScrollBarWndH != NULL && pExtScrollBarWndV != NULL )
			{
				pExtScrollBarWndH->SyncReservedSpace( pExtScrollBarWndV );
				pExtScrollBarWndV->SyncReservedSpace( pExtScrollBarWndH );
			} // if( pExtScrollBarWndH != NULL && pExtScrollBarWndV != NULL )
		} // if( pScrollBarWndH != NULL && pScrollBarWndV != NULL )
	} // if( bDoLayout )
CRect rcClient;
	if( pRectClientSrc != NULL )
		rcClient = *pRectClientSrc;
	else
		CWnd::GetClientRect( &rcClient );
	CWnd::RepositionBars(
		0,
		0x0FFFF,
		AFX_IDW_PANE_FIRST,
		bDoLayout ? CWnd::reposDefault : CWnd::reposQuery,
		&rcClient,
		&rcClient,
		TRUE
		);
	return rcClient;
}

CRect CExtScrollWnd::OnSwGetClientRect() const
{
	ASSERT_VALID( this );
CRect rcClient =
		( const_cast < CExtScrollWnd * > ( this ) )
			-> OnSwRecalcLayout( false );
	return rcClient;
}

bool CExtScrollWnd::OnSwHasScrollBar( bool bHorz ) const
{
	ASSERT_VALID( this );
CScrollBar * pScrollBarWnd = GetScrollBarCtrl( bHorz ? SB_HORZ : SB_VERT );
	if( pScrollBarWnd != NULL )
	{
		if( pScrollBarWnd->IsWindowEnabled() )
			return true;
		return false;
	} // if( pScrollBarWnd != NULL )
DWORD dwWndStyle = CWnd::GetStyle();
DWORD dwTestStyle = bHorz ? WS_HSCROLL : WS_VSCROLL;
	if( ( dwWndStyle & dwTestStyle ) != 0 )
		return true;
	return false;
}

bool CExtScrollWnd::OnSwCanAutoHideScrollBar( bool bHorz ) const
{
	ASSERT_VALID( this );
	bHorz;
	return true;
}

void CExtScrollWnd::OnSwEnableScrollBarCtrl( int nBar, bool bEnable )
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
	ASSERT( nBar == SB_HORZ || nBar == SB_VERT || nBar == SB_BOTH );
	if( ! IsWindowEnabled() )
		bEnable = false;
DWORD dwWndStyle = CWnd::GetStyle();
CPoint ptMove = OnSwGetScrollPos();
CScrollBar * pScrollBarWndH = GetScrollBarCtrl( SB_HORZ );
CScrollBar * pScrollBarWndV = GetScrollBarCtrl( SB_VERT );
	if( pScrollBarWndH != NULL )
	{
		CExtScrollBar * pExtSBH = DYNAMIC_DOWNCAST( CExtScrollBar, pScrollBarWndH );
		if( pExtSBH != NULL && pExtSBH->m_bZeroExtentMode != m_bHideNormalScrollBars )
		{
			pExtSBH->m_bZeroExtentMode = m_bHideNormalScrollBars;
			pExtSBH->_ScanSysMertics();
		}
	}
	if( pScrollBarWndV != NULL )
	{
		CExtScrollBar * pExtSBV = DYNAMIC_DOWNCAST( CExtScrollBar, pScrollBarWndV );
		if( pExtSBV != NULL && pExtSBV->m_bZeroExtentMode != m_bHideNormalScrollBars )
		{
			pExtSBV->m_bZeroExtentMode = m_bHideNormalScrollBars;
			pExtSBV->_ScanSysMertics();
		}
	}
	if( nBar == SB_HORZ || nBar == SB_BOTH )
	{
		if( (!bEnable) && ptMove.x != 0 )
		{
			ptMove.x = 0;
			OnSwSetScrollPos( ptMove );
		} // if( (!bEnable) && ptMove.x != 0 )
		if( pScrollBarWndH != NULL )
		{
			if( (dwWndStyle & WS_HSCROLL) != 0 )
				CWnd::ShowScrollBar( SB_HORZ, FALSE );
			bool bAreadyEnabled = pScrollBarWndH->IsWindowEnabled() ? true : false;
			if(	!(		( bAreadyEnabled && bEnable )
					||	( (!bAreadyEnabled) && (!bEnable) )
					)
				)
			{
				pScrollBarWndH->EnableWindow( bEnable ? TRUE : FALSE );
				if( OnSwCanAutoHideScrollBar(true) )
					pScrollBarWndH->ShowWindow( ( bEnable && (!m_bHideNormalScrollBars) ) ? SW_SHOW : SW_HIDE );
				else if( bEnable && (pScrollBarWndH->GetStyle()&WS_VISIBLE) == 0 )
					pScrollBarWndH->ShowWindow( m_bHideNormalScrollBars ? SW_HIDE : SW_SHOW );
			}
		} // if( pScrollBarWndH != NULL )
		else
		{
			if( OnSwCanAutoHideScrollBar(true) )
				CWnd::ShowScrollBar( SB_HORZ, bEnable ? TRUE : FALSE );
			else
				CWnd::EnableScrollBar( SB_HORZ, bEnable ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH );
		} // else from if( pScrollBarWndH != NULL )
	} // if( nBar == SB_HORZ || nBar == SB_BOTH )
	if( nBar == SB_VERT || nBar == SB_BOTH )
	{
		if( (!bEnable) && ptMove.y != 0 )
		{
			ptMove.y = 0;
			OnSwSetScrollPos( ptMove );
		} // if( (!bEnable) && ptMove.y != 0 )
		if( pScrollBarWndV != NULL )
		{
			if( (dwWndStyle & WS_VSCROLL) != 0 )
				CWnd::ShowScrollBar( SB_VERT, FALSE );
			bool bAreadyEnabled = pScrollBarWndV->IsWindowEnabled() ? true : false;
			if(	!(		( bAreadyEnabled && bEnable )
					||	( (!bAreadyEnabled) && (!bEnable) )
					)
				)
			{
				pScrollBarWndV->EnableWindow( ( bEnable && (!m_bHideNormalScrollBars) ) ? TRUE : FALSE );
				if( OnSwCanAutoHideScrollBar(false) )
					pScrollBarWndV->ShowWindow( bEnable ? SW_SHOW : SW_HIDE );
				else if( bEnable && (pScrollBarWndV->GetStyle()&WS_VISIBLE) == 0 )
					pScrollBarWndV->ShowWindow( m_bHideNormalScrollBars ? SW_HIDE : SW_SHOW );
			}
		} // if( pScrollBarWndV != NULL )
		else
		{
			if( OnSwCanAutoHideScrollBar(false) )
				CWnd::ShowScrollBar( SB_VERT, bEnable ? TRUE : FALSE );
			else
				CWnd::EnableScrollBar( SB_VERT, bEnable ? ESB_ENABLE_BOTH : ESB_DISABLE_BOTH );
		} // else from if( pScrollBarWndV != NULL )
	} // if( nBar == SB_VERT || nBar == SB_BOTH )
CExtScrollBar * pExtScrollBarWndH = DYNAMIC_DOWNCAST( CExtScrollBar, pScrollBarWndH );
CExtScrollBar * pExtScrollBarWndV = DYNAMIC_DOWNCAST( CExtScrollBar, pScrollBarWndV );
	if( pExtScrollBarWndH != NULL && pExtScrollBarWndV != NULL )
	{
		pExtScrollBarWndH->SyncReservedSpace( pExtScrollBarWndV );
		pExtScrollBarWndV->SyncReservedSpace( pExtScrollBarWndH );
	} // if( pExtScrollBarWndH != NULL && pExtScrollBarWndV != NULL )
}

void CExtScrollWnd::OnSwSetScrollPos( POINT pt )
{
	ASSERT_VALID( this );
	ASSERT( pt.x >= 0 && pt.y >= 0 );
int xOrgValue = ScrollPos32Get( SB_HORZ );
	ScrollPos32Set( SB_HORZ, pt.x );
int yOrgValue = ScrollPos32Get( SB_VERT );
	ScrollPos32Set( SB_VERT, pt.y );
	OnSwDoScrollWindow( xOrgValue - pt.x, yOrgValue - pt.y );
}

UINT CExtScrollWnd::OnSwCalcMouseScrollLines(
	UINT fFlags,
	short zDelta,
	CPoint point
	)
{
	ASSERT_VALID( this );
	fFlags;
	zDelta;
	point;
	return g_PaintManager.GetMouseWheelScrollLines();
}

bool CExtScrollWnd::OnSwDoMouseWheel(
	UINT fFlags,
	short zDelta,
	CPoint point
	)
{
	ASSERT_VALID( this );
	point;
	if( CExtPopupMenuWnd::IsMenuTracking() )
	{
		CExtPopupMenuWnd * pPopup = CExtPopupMenuSite::g_DefPopupMenuSite.GetInstance();
		if( pPopup == NULL )
			return false;
		bool bFound = false;
		CWnd * pWnd = GetParent();
		for( ; pWnd != NULL; pWnd = pWnd->GetParent() )
		{
			if( pWnd == pPopup )
			{
				bFound = true;
				break;
			}
		}
		if( ! bFound )
			return false;
	} // if( CExtPopupMenuWnd::IsMenuTracking() )
	else if(		(fFlags&(MK_CONTROL)) != 0
		||	CExtToolControlBar::g_bMenuTracking
		)
		return false;

	if( ! ( OnSwHasScrollBar(true) || OnSwHasScrollBar(false) ) )
		return false;
bool bResult = false;
UINT uWheelScrollLines =
		OnSwCalcMouseScrollLines(
			fFlags,
			zDelta,
			point
			);
	if(		OnSwHasScrollBar(false)
		&&	( (fFlags&(MK_SHIFT)) == 0 )
		)
	{
		int nDisplacement;
		int nToScroll =
			::MulDiv( -zDelta, uWheelScrollLines, WHEEL_DELTA );
		if(		nToScroll == -1
			||	uWheelScrollLines == WHEEL_PAGESCROLL
			)
		{
			nDisplacement = OnSwGetPageSize( -1 ).cy;
			if( zDelta > 0 )
				nDisplacement = -nDisplacement;
		}
		else
		{
			nDisplacement =
				nToScroll * OnSwGetLineSize( 1 ).cy;
			nDisplacement =
				min(
					nDisplacement,
					OnSwGetPageSize( 1 ).cy
					);
		}
		bResult =
			OnSwDoScrollBy(
				CSize( 0, nDisplacement ),
				true
				);
	}
	else if(
			OnSwHasScrollBar( true )
		&&	( (fFlags&(MK_SHIFT)) != 0 )
		)
	{
		int nDisplacement;
		int nToScroll =
			::MulDiv( -zDelta, uWheelScrollLines, WHEEL_DELTA );
		if(		nToScroll == -1
			||	uWheelScrollLines == WHEEL_PAGESCROLL
			)
		{
			nDisplacement = OnSwGetPageSize( -1 ).cx;
			if( zDelta > 0 )
				nDisplacement = -nDisplacement;
		}
		else
		{
			nDisplacement =
				nToScroll * OnSwGetLineSize( 1 ).cx;
			nDisplacement =
				min(
					nDisplacement,
					OnSwGetPageSize( 1 ).cx
					);
		}
		bResult =
			OnSwDoScrollBy(
				CSize( nDisplacement, 0 ),
				true
				);
	}

	if( bResult )
	{
		OnSwInvalidate( m_bScrollErase );
		if( m_bScrollUpdateWindow )
			OnSwUpdateWindow();
	}

	return bResult;
}

bool CExtScrollWnd::OnSwDoScroll(
	UINT nScrollCode,
	UINT nPos,
	bool bDoScroll // = true
	)
{
	ASSERT_VALID( this );
LONG xPos, xOrgValue, yPos, yOrgValue;
	xOrgValue = xPos = ScrollPos32Get( SB_HORZ );
	switch( LOBYTE(nScrollCode) )
	{
	case SB_ENDSCROLL: break;
	case SB_TOP:		xPos  = 0;							break;
	case SB_BOTTOM:		xPos  = __EXT_SCROLL_NUMERIC_MAX;	break;
	case SB_LINEUP:		xPos -= OnSwGetLineSize( -1 ).cx;	break;
	case SB_LINEDOWN:	xPos += OnSwGetLineSize(  1 ).cx;	break;
	case SB_PAGEUP:		xPos -= OnSwGetPageSize( -1 ).cx;	break;
	case SB_PAGEDOWN:	xPos += OnSwGetPageSize(  1 ).cx;	break;
	case SB_THUMBTRACK:
		if( ! OnSwQueryThumbTrackEnabled(true) )
			break;
	case SB_THUMBPOSITION:
			if( m_bUse32BitScrollInfo )
				nPos = ScrollPos32Get( SB_HORZ, true );
			xPos  = nPos;
		break;
	}
	yOrgValue = yPos = ScrollPos32Get( SB_VERT );
	switch( HIBYTE(nScrollCode) )
	{
	case SB_ENDSCROLL: break;
	case SB_TOP:		yPos  = 0;							break;
	case SB_BOTTOM:		yPos  = __EXT_SCROLL_NUMERIC_MAX;	break;
	case SB_LINEUP:		yPos -= OnSwGetLineSize( -1 ).cy;	break;
	case SB_LINEDOWN:	yPos += OnSwGetLineSize(  1 ).cy;	break;
	case SB_PAGEUP:		yPos -= OnSwGetPageSize( -1 ).cy;	break;
	case SB_PAGEDOWN:	yPos += OnSwGetPageSize(  1 ).cy;	break;
	case SB_THUMBTRACK:
		if( ! OnSwQueryThumbTrackEnabled(false) )
			break;
	case SB_THUMBPOSITION:
			if( m_bUse32BitScrollInfo )
				nPos = ScrollPos32Get( SB_VERT, true );
			yPos  = nPos;
		break;
	}
bool bResult =
		OnSwDoScrollBy(
			CSize( xPos - xOrgValue, yPos - yOrgValue ),
			bDoScroll
			);
	if( bResult && bDoScroll && m_bScrollUpdateWindow )
		OnSwUpdateWindow();
	return bResult;
}

bool CExtScrollWnd::OnSwDoScrollBy(
	CSize sizeScroll,
	bool bDoScroll // = true
	)
{
	ASSERT_VALID( this );
int xMaxValue, xOrgValue, xPos, yMaxValue, yOrgValue, yPos;
	if( sizeScroll.cy != 0 && ( ! OnSwHasScrollBar(false) ) )
		sizeScroll.cy = 0;
	if( sizeScroll.cx != 0 && ( ! OnSwHasScrollBar(true) ) )
		sizeScroll.cx = 0;
	// adjust current positions
	xOrgValue = xPos = ScrollPos32Get( SB_HORZ );
	xMaxValue = ScrollLimit32Get( SB_HORZ );
	xPos += sizeScroll.cx;
	if( xPos < 0 )
		xPos = 0;
	else if( xPos > xMaxValue )
		xPos = xMaxValue;
	yOrgValue = yPos = ScrollPos32Get( SB_VERT );
	yMaxValue = ScrollLimit32Get( SB_VERT );
	yPos += sizeScroll.cy;
	if( yPos < 0 )
		yPos = 0;
	else if( yPos > yMaxValue )
		yPos = yMaxValue;
	if( xPos == xOrgValue && yPos == yOrgValue )
		return false;
	if( bDoScroll )
	{
		OnSwDoScrollWindow( xOrgValue - xPos, yOrgValue - yPos );
		if( xPos != xOrgValue )
			ScrollPos32Set( SB_HORZ, xPos );
		if( yPos != yOrgValue )
			ScrollPos32Set( SB_VERT, yPos );
	}
	return true;
}

CSize CExtScrollWnd::OnSwGetScrollBarSizes() const
{
	ASSERT_VALID( this );
CSize sizeSb( 0, 0 );
	if( m_bHideNormalScrollBars )
		return sizeSb;
DWORD dwWndStyle = CWnd::GetStyle();
	if( GetScrollBarCtrl(SB_HORZ) == NULL )
	{
		sizeSb.cx = afxData.cxVScroll;
		if( dwWndStyle & WS_BORDER )
			sizeSb.cx -= __EXT_MFC_CX_BORDER;
	} // if( GetScrollBarCtrl(SB_HORZ) == NULL )
	if( GetScrollBarCtrl(SB_VERT) == NULL )
	{
		sizeSb.cy = afxData.cyHScroll;
		if( dwWndStyle & WS_BORDER )
			sizeSb.cy -= __EXT_MFC_CY_BORDER;
	} // if( GetScrollBarCtrl(SB_VERT) == NULL )
	return sizeSb;
}

bool CExtScrollWnd::OnSwCalcClientSizes(
	CSize & size,
	CSize & sizeSb
	)
{
	// return true if enough room to add scrollbars if needed
	ASSERT_VALID( this );
CRect rcClient = OnSwGetClientRect();
	size = rcClient.Size();
	sizeSb = OnSwGetScrollBarSizes();
	if(		sizeSb.cx != 0
		&&	OnSwHasScrollBar( false )
		)
		size.cx += sizeSb.cx;
	if(		sizeSb.cy != 0
		&&	OnSwHasScrollBar( true )
		)
		size.cy += sizeSb.cy;
//	return
//		( size.cx > sizeSb.cx && size.cy > sizeSb.cy )
//			? true : false;
	return true;
}

bool CExtScrollWnd::OnSwQueryThumbTrackEnabled( bool bHorz ) const
{
	ASSERT_VALID( this );
	bHorz;
	return true;
}

void CExtScrollWnd::OnSwGetScrollBarState(
	CSize sizeClient,
	CSize & sizeNeedSB,
	CSize & sizeRange,
	CPoint & ptMove,
	bool bInsideClient
	) const
{
	ASSERT_VALID( this );
CSize sizeSb = OnSwGetScrollBarSizes();
	sizeRange = OnSwGetTotalSize() - sizeClient;
	ptMove = OnSwGetScrollPos();
bool bNeedH = sizeRange.cx > 0;
	if( !bNeedH )
		ptMove.x = 0;
	else if( bInsideClient )
		sizeRange.cy += sizeSb.cy;
bool bNeedV = sizeRange.cy > 0;
	if( !bNeedV )
		ptMove.y = 0;
	else if ( bInsideClient )
		sizeRange.cx += sizeSb.cx;
	if( bNeedV && (!bNeedH) && sizeRange.cx > 0 )
	{
		ASSERT( bInsideClient );
		bNeedH = true;
		sizeRange.cy += sizeSb.cy;
	} // if( bNeedV && (!bNeedH) && sizeRange.cx > 0 )
	if( sizeRange.cx > 0 && ptMove.x >= sizeRange.cx )
		ptMove.x = sizeRange.cx;
	if( sizeRange.cy > 0 && ptMove.y >= sizeRange.cy )
		ptMove.y = sizeRange.cy;
	sizeNeedSB.cx = bNeedH;
	sizeNeedSB.cy = bNeedV;
}

void CExtScrollWnd::OnSwUpdateScrollBars()
{
	ASSERT_VALID( this );
	if( m_nUpdateScrollBars > 1 )
		return;
	m_nUpdateScrollBars ++;

	ASSERT( OnSwGetTotalSize().cx >= 0 && OnSwGetTotalSize().cy >= 0 );
CRect rcClient;
bool bCalcClient = true;
CWnd * pParentWnd = GetParent();
	if(		pParentWnd != NULL
		&&	(BOOL)pParentWnd->SendMessage(
				WM_RECALCPARENT,
				0,
				(LPARAM)(LPCRECT)&rcClient
				) != 0
		)
		bCalcClient = false;
CSize sizeClient;
CSize sizeSb;
	if( bCalcClient )
	{
		if( ! OnSwCalcClientSizes( sizeClient, sizeSb ) )
		{ // no room for scroll bars
			CRect rcClient2 = OnSwGetClientRect();
			if( rcClient2.Width() > 0 && rcClient2.Height() > 0 )
			{
				OnSwEnableScrollBarCtrl( SB_BOTH, false );
				OnSwRecalcLayout( true );
			}
			m_nUpdateScrollBars --;
			return;
		}
	}
	else
	{ // let parent window determine the "client" rect
		sizeSb = OnSwGetScrollBarSizes();
		sizeClient.cx = rcClient.right - rcClient.left;
		sizeClient.cy = rcClient.bottom - rcClient.top;
	}

	// if enough room to add scrollbars
CSize sizeRange;
CPoint ptMove;
CSize sizeNeedSB;
	OnSwGetScrollBarState(
		sizeClient,
		sizeNeedSB,
		sizeRange,
		ptMove,
		bCalcClient
		);
	if( sizeNeedSB.cx )
		sizeClient.cy -= sizeSb.cy;
	if( sizeNeedSB.cy )
		sizeClient.cx -= sizeSb.cx;

	// scroll window + update
	OnSwSetScrollPos( ptMove );

	// the scrollbar page range
SCROLLINFO _scroll_info;
	::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
	_scroll_info.cbSize = sizeof(SCROLLINFO);
	_scroll_info.fMask = SIF_PAGE|SIF_RANGE;
	_scroll_info.nMin = 0;
	// update bars
	OnSwEnableScrollBarCtrl( SB_HORZ, sizeNeedSB.cx ? true : false );
	if( sizeNeedSB.cx )
	{
		_scroll_info.nPage = sizeClient.cx;
		_scroll_info.nMax = OnSwGetTotalSize().cx - 1;
		if( ! OnSwScrollInfoAdjust(
				SB_HORZ,
				_scroll_info,
				true
				)
			)
			OnSwSetScrollRange(
				SB_HORZ,
				0,
				sizeRange.cx,
				true
				);
	} // if( sizeNeedSB.cx )
	OnSwEnableScrollBarCtrl( SB_VERT, sizeNeedSB.cy ? true : false );
	if( sizeNeedSB.cy )
	{
		_scroll_info.nPage = sizeClient.cy;
		_scroll_info.nMax = OnSwGetTotalSize().cy - 1;
		if( ! OnSwScrollInfoAdjust(
				SB_VERT,
				_scroll_info,
				true
				)
			)
			OnSwSetScrollRange(
				SB_VERT,
				0,
				sizeRange.cy,
				true
				);
	} // if( sizeNeedSB.cy )
	OnSwRecalcLayout( true );
	m_nUpdateScrollBars --;
}

void CExtScrollWnd::OnSwDoScrollWindow(
	int xAmount,
	int yAmount,
	LPCRECT lpRect, // = NULL
	LPCRECT lpClipRect // = NULL
	)
{
	ASSERT_VALID( this );
	if( m_bScrollPhysical )
	{
		CRect rcClient = OnSwGetClientRect();
		CRect rcScroll = ( lpRect == NULL )     ? rcClient : (*lpRect);
		CRect rcClip   = ( lpClipRect == NULL ) ? rcClient : (*lpClipRect);
		CWnd::ScrollWindow(
			xAmount,
			yAmount,
			rcScroll, // lpRect,
			rcClip // lpClipRect
			);
	}
	if( m_bScrollInvalidate )
		OnSwInvalidate( m_bScrollErase );
	if( m_bScrollUpdateWindow )
		OnSwUpdateWindow();
}

void CExtScrollWnd::OnSwInvalidate( bool bErase )
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
	if( ! IsWindowVisible() )
		return;
CScrollBar * pScrollBarWndH = GetScrollBarCtrl( SB_HORZ );
CScrollBar * pScrollBarWndV = GetScrollBarCtrl( SB_VERT );
	if(		pScrollBarWndH != NULL
		&&	( pScrollBarWndH->GetStyle() & WS_VISIBLE ) == 0
		)
		pScrollBarWndH = NULL;
	if(		pScrollBarWndV != NULL
		&&	( pScrollBarWndV->GetStyle() & WS_VISIBLE ) == 0
		)
		pScrollBarWndV = NULL;
	if( pScrollBarWndH == NULL && pScrollBarWndV == NULL )
	{
		CWnd::Invalidate( bErase ? TRUE : FALSE );
		return;
	} // if( pScrollBarWndH == NULL && pScrollBarWndV == NULL )
CRect rcClientReal;
	CWnd::GetClientRect( &rcClientReal );
CRgn rgnInvalidate;
	if( !rgnInvalidate.CreateRectRgnIndirect(&rcClientReal) )
	{
		CWnd::Invalidate( bErase ? TRUE : FALSE );
		return;
	}
	if( pScrollBarWndH != NULL )
	{
		CRect rcBar;
		pScrollBarWndH->GetWindowRect( &rcBar );
		ScreenToClient( &rcBar );
		CRgn rgnBar;
		if(		(! rgnBar.CreateRectRgnIndirect(&rcBar) )
			||	rgnInvalidate.CombineRgn(
					&rgnInvalidate,
					&rgnBar,
					RGN_DIFF
					) == ERROR
			)
		{
			CWnd::Invalidate( bErase ? TRUE : FALSE );
			return;
		}
	} // if( pScrollBarWndH != NULL )
	if( pScrollBarWndV != NULL )
	{
		CRect rcBar;
		pScrollBarWndV->GetWindowRect( &rcBar );
		ScreenToClient( &rcBar );
		CRgn rgnBar;
		if(		(! rgnBar.CreateRectRgnIndirect(&rcBar) )
			||	rgnInvalidate.CombineRgn(
					&rgnInvalidate,
					&rgnBar,
					RGN_DIFF
					) == ERROR
			)
		{
			CWnd::Invalidate( bErase ? TRUE : FALSE );
			return;
		}
	} // if( pScrollBarWndV != NULL )
	CWnd::InvalidateRgn( &rgnInvalidate, bErase ? TRUE : FALSE );
}

void CExtScrollWnd::OnSwUpdateWindow()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
	UpdateWindow();
}

void CExtScrollWnd::OnSwDoRedraw()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
	if( m_bRedrawInvalidate )
		OnSwInvalidate( m_bRedrawErase );
	if( m_bRedrawUpdateWindow )
		OnSwUpdateWindow();
}

void CExtScrollWnd::OnSwPaint( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
}

void CExtScrollWnd::OnSwPaintPopOverItems( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( m_bDisplayPopOverScrollBars && ( m_bPopOverScrollBarsVisible || m_nPopOverScrollBarTimerEllapse <= 0 ) )
		OnSwPaintPopOverScrollBars( dc );
}

void CExtScrollWnd::OnSwCalcPopOverScrollBarRects(
	RECT & rcSbH_entire, RECT & rcSbV_entire,
	RECT & rcSbH_paging, RECT & rcSbV_paging,
	RECT & rcSbH_thumb, RECT & rcSbV_thumb,
	RECT & rcSbH_line_up, RECT & rcSbV_line_up,
	RECT & rcSbH_line_down, RECT & rcSbV_line_down,
	RECT & rcSbH_page_up, RECT & rcSbV_page_up,
	RECT & rcSbH_page_down, RECT & rcSbV_page_down
	) const
{
	ASSERT_VALID( this );
	::SetRect( &rcSbH_entire, 0, 0, 0, 0 );
	::SetRect( &rcSbV_entire, 0, 0, 0, 0 );
	::SetRect( &rcSbH_paging, 0, 0, 0, 0 );
	::SetRect( &rcSbV_paging, 0, 0, 0, 0 );
	::SetRect( &rcSbH_thumb, 0, 0, 0, 0 );
	::SetRect( &rcSbV_thumb, 0, 0, 0, 0 );
	::SetRect( &rcSbH_line_up, 0, 0, 0, 0 );
	::SetRect( &rcSbV_line_up, 0, 0, 0, 0 );
	::SetRect( &rcSbH_line_down, 0, 0, 0, 0 );
	::SetRect( &rcSbV_line_down, 0, 0, 0, 0 );
	::SetRect( &rcSbH_page_up, 0, 0, 0, 0 );
	::SetRect( &rcSbV_page_up, 0, 0, 0, 0 );
	::SetRect( &rcSbH_page_down, 0, 0, 0, 0 );
	::SetRect( &rcSbV_page_down, 0, 0, 0, 0 );
bool bH = OnSwHasScrollBar( true ), bV = OnSwHasScrollBar( false );
	if( ! ( bH || bV ) )
		return;
LONG	nMinPosH = 0L, nMaxPosH = 0L, nPageSizeH = 0L, nCurrPosH = 0L, nTrackPosH = 0L, nRangeH = 0L,
		nMinPosV = 0L, nMaxPosV = 0L, nPageSizeV = 0L, nCurrPosV = 0L, nTrackPosV = 0L, nRangeV = 0L;
	if( bH )
	{
		ScrollInfo32Get( SB_HORZ, &nMinPosH, &nMaxPosH, &nPageSizeH, &nCurrPosH, &nTrackPosH );
		nRangeH = nMaxPosH - nMinPosH + 1L - nPageSizeH;
		if( nRangeH <= 0L || nPageSizeH <= 0L )
			bH = false;
		else
		{
			nCurrPosH -= nMinPosH;
			nCurrPosH = max( nCurrPosH, 0L );
			nCurrPosH = min( nCurrPosH, nRangeH );
		}
	}
	if( bV )
	{
		ScrollInfo32Get( SB_VERT, &nMinPosV, &nMaxPosV, &nPageSizeV, &nCurrPosV, &nTrackPosV );
		nRangeV = nMaxPosV - nMinPosV + 1L - nPageSizeV;
		if( nRangeV <= 0L || nPageSizeV <= 0L )
			bV = false;
		else
		{
			nCurrPosV -= nMinPosV;
			nCurrPosV = max( nCurrPosV, 0L );
			nCurrPosV = min( nCurrPosV, nRangeV );
		}
	}
	if( ! ( bH || bV ) )
		return;
CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );
static const LONG g_nCrossExtX       = pPM->UiScalingDo( 12, CExtPaintManager::__EUIST_X );
static const LONG g_nCrossExtY       = pPM->UiScalingDo( 12, CExtPaintManager::__EUIST_Y );
static const LONG g_nOuterSpaceX     = pPM->UiScalingDo(  4, CExtPaintManager::__EUIST_X );
static const LONG g_nOuterSpaceY     = pPM->UiScalingDo(  4, CExtPaintManager::__EUIST_Y );
static const LONG g_nMinThumbExtentX = pPM->UiScalingDo(  6, CExtPaintManager::__EUIST_X );
static const LONG g_nMinThumbExtentY = pPM->UiScalingDo(  6, CExtPaintManager::__EUIST_Y );
static const LONG g_nButtonExtentX   = pPM->UiScalingDo( 20, CExtPaintManager::__EUIST_X );
static const LONG g_nButtonExtentY   = pPM->UiScalingDo( 20, CExtPaintManager::__EUIST_Y );
static const LONG g_nButtonSpaceX    = pPM->UiScalingDo(  2, CExtPaintManager::__EUIST_X );
static const LONG g_nButtonSpaceY    = pPM->UiScalingDo(  2, CExtPaintManager::__EUIST_Y );
static const LONG g_nCornerGapX      = pPM->UiScalingDo(  2, CExtPaintManager::__EUIST_X );
static const LONG g_nCornerGapY      = pPM->UiScalingDo(  2, CExtPaintManager::__EUIST_Y );
CRect rcPO = OnSwCalcPopOverRect();
	rcPO.DeflateRect( g_nOuterSpaceX, g_nOuterSpaceY );
CRect rcH( rcPO.left, rcPO.bottom - g_nCrossExtY, rcPO.right, rcPO.bottom ), rcV( rcPO.right - g_nCrossExtX, rcPO.top, rcPO.right, rcPO.bottom );
	if( bH && bV )
	{
		rcH.right  -= g_nCrossExtX + g_nCornerGapX;
		rcV.bottom -= g_nCrossExtY + g_nCornerGapY;
	}
	if( bH )
	{
		::CopyRect( &rcSbH_entire, &rcH );
		if( ! m_bLightPopOverScrollBars )
		{
			::SetRect( &rcSbH_line_up, rcSbH_entire.left, rcSbH_entire.top, rcSbH_entire.left + g_nButtonExtentX, rcSbH_entire.bottom );
			::SetRect( &rcSbH_line_down, rcSbH_entire.right - g_nButtonExtentX, rcSbH_entire.top, rcSbH_entire.right, rcSbH_entire.bottom );
			rcH.left  += g_nButtonExtentX + g_nButtonSpaceX;
			rcH.right -= g_nButtonExtentX + g_nButtonSpaceX;
		}
		::CopyRect( &rcSbH_paging, &rcH );
		LONG nTotalExtPX = rcH.Width();
		if( nTotalExtPX < g_nMinThumbExtentX )
		{
			nTotalExtPX = g_nMinThumbExtentX;
			if( ! m_bLightPopOverScrollBars )
				rcSbH_paging.right = rcSbH_paging.left + nTotalExtPX;
		}
		LONG nThumbExtPX = ::MulDiv( nTotalExtPX, nPageSizeH, nRangeH + nPageSizeH );
		nThumbExtPX = max( nThumbExtPX, g_nMinThumbExtentX );
		LONG nThumbOffsetPX = ::MulDiv( nTotalExtPX, nCurrPosH, nRangeH + nPageSizeH );
		rcH.left += nThumbOffsetPX;
		rcH.right = rcH.left + nThumbExtPX;
		::CopyRect( &rcSbH_thumb, &rcH );
		if( ! m_bLightPopOverScrollBars )
		{
			::SetRect( &rcSbH_page_up, rcSbH_paging.left, rcSbH_paging.top, rcSbH_thumb.left, rcSbH_paging.bottom );
			::SetRect( &rcSbH_page_down, rcSbH_thumb.right, rcSbH_paging.top, rcSbH_paging.right, rcSbH_paging.bottom );
		}
	}
	if( bV )
	{
		::CopyRect( &rcSbV_entire, &rcV );
		if( ! m_bLightPopOverScrollBars )
		{
			::SetRect( &rcSbV_line_up, rcSbV_entire.left, rcSbV_entire.top, rcSbV_entire.right, rcSbV_entire.top + g_nButtonExtentY );
			::SetRect( &rcSbV_line_down, rcSbV_entire.left, rcSbV_entire.bottom - g_nButtonExtentY, rcSbV_entire.right, rcSbV_entire.bottom );
			rcV.top    += g_nButtonExtentY + g_nButtonSpaceY;
			rcV.bottom -= g_nButtonExtentY + g_nButtonSpaceY;
		}
		::CopyRect( &rcSbV_paging, &rcV );
		LONG nTotalExtPX = rcV.Height();
		if( nTotalExtPX < g_nMinThumbExtentY )
		{
			nTotalExtPX = g_nMinThumbExtentY;
			if( ! m_bLightPopOverScrollBars )
				rcSbH_paging.bottom = rcSbH_paging.top + nTotalExtPX;
		}
		LONG nThumbExtPX = ::MulDiv( nTotalExtPX, nPageSizeV, nRangeV + nPageSizeV );
		nThumbExtPX = max( nThumbExtPX, g_nMinThumbExtentY );
		LONG nThumbOffsetPX = ::MulDiv( nTotalExtPX, nCurrPosV, nRangeV + nPageSizeV );
		rcV.top += nThumbOffsetPX;
		rcV.bottom = rcV.top + nThumbExtPX;
		::CopyRect( &rcSbV_thumb, &rcV );
		if( ! m_bLightPopOverScrollBars )
		{
			::SetRect( &rcSbV_page_up, rcSbV_paging.left, rcSbV_paging.top, rcSbV_paging.right, rcSbV_thumb.top );
			::SetRect( &rcSbV_page_down, rcSbV_paging.left, rcSbV_thumb.bottom, rcSbV_paging.right, rcSbV_paging.bottom );
		}
	}
}

void CExtScrollWnd::OnSwPaintPopOverScrollBars( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CExtBitmap & _bmpScrollThumb = CExtRichContentItem::stat_GetScrollThumbBitmap();
	if( _bmpScrollThumb.IsEmpty() )
		return;
CRect		rcSbH_entire( 0, 0, 0, 0 ), rcSbV_entire( 0, 0, 0, 0 ),
			rcSbH_paging( 0, 0, 0, 0 ), rcSbV_paging( 0, 0, 0, 0 ),
			rcSbH_thumb( 0, 0, 0, 0 ), rcSbV_thumb( 0, 0, 0, 0 ),
			rcSbH_line_up( 0, 0, 0, 0 ), rcSbV_line_up( 0, 0, 0, 0 ),
			rcSbH_line_down( 0, 0, 0, 0 ), rcSbV_line_down( 0, 0, 0, 0 ),
			rcSbH_page_up( 0, 0, 0, 0 ), rcSbV_page_up( 0, 0, 0, 0 ),
			rcSbH_page_down( 0, 0, 0, 0 ), rcSbV_page_down( 0, 0, 0, 0 );
	OnSwCalcPopOverScrollBarRects(
		rcSbH_entire, rcSbV_entire,
		rcSbH_paging, rcSbV_paging,
		rcSbH_thumb, rcSbV_thumb,
		rcSbH_line_up, rcSbV_line_up,
		rcSbH_line_down, rcSbV_line_down,
		rcSbH_page_up, rcSbV_page_up,
		rcSbH_page_down, rcSbV_page_down
		);
	if( ! rcSbH_entire.IsRectEmpty() )
	{
		if( ! m_bLightPopOverScrollBars )
			_bmpScrollThumb.AlphaBlendSkinParts( dc.m_hDC, rcSbH_paging, CRect(3,3,3,3), CExtBitmap::__EDM_STRETCH, true, true, BYTE(0x60) );
		if( ! rcSbH_thumb.IsRectEmpty() )
		{
			_bmpScrollThumb.AlphaBlendSkinParts( dc.m_hDC, rcSbH_thumb, CRect(3,3,3,3), CExtBitmap::__EDM_STRETCH, true, true, BYTE(0xFF) );
		}
		if( ! rcSbH_line_up.IsRectEmpty() )
		{
			_bmpScrollThumb.AlphaBlendSkinParts( dc.m_hDC, rcSbH_line_up, CRect(3,3,3,3), CExtBitmap::__EDM_STRETCH, true, true, BYTE(0xFF) );
		}
		if( ! rcSbH_line_down.IsRectEmpty() )
		{
			_bmpScrollThumb.AlphaBlendSkinParts( dc.m_hDC, rcSbH_line_down, CRect(3,3,3,3), CExtBitmap::__EDM_STRETCH, true, true, BYTE(0xFF) );
		}
	}
	if( ! rcSbV_entire.IsRectEmpty() )
	{
		if( ! m_bLightPopOverScrollBars )
			_bmpScrollThumb.AlphaBlendSkinParts( dc.m_hDC, rcSbV_paging, CRect(3,3,3,3), CExtBitmap::__EDM_STRETCH, true, true, BYTE(0x60) );
		if( ! rcSbV_thumb.IsRectEmpty() )
		{
			_bmpScrollThumb.AlphaBlendSkinParts( dc.m_hDC, rcSbV_thumb, CRect(3,3,3,3), CExtBitmap::__EDM_STRETCH, true, true, BYTE(0xFF) );
		}
		if( ! rcSbV_line_up.IsRectEmpty() )
		{
			_bmpScrollThumb.AlphaBlendSkinParts( dc.m_hDC, rcSbV_line_up, CRect(3,3,3,3), CExtBitmap::__EDM_STRETCH, true, true, BYTE(0xFF) );
		}
		if( ! rcSbV_line_down.IsRectEmpty() )
		{
			_bmpScrollThumb.AlphaBlendSkinParts( dc.m_hDC, rcSbV_line_down, CRect(3,3,3,3), CExtBitmap::__EDM_STRETCH, true, true, BYTE(0xFF) );
		}
	}
}

CRect CExtScrollWnd::OnSwCalcPopOverRect() const
{
	ASSERT_VALID( this );
	return OnSwGetClientRect();
}

INT CExtScrollWnd::OnSwGetPopOverScrollBarStepSize( bool bHorz ) const
{
	ASSERT_VALID( this );
	bHorz;
	return 1;
}

void CExtScrollWnd::OnSwTrackPopOverScrollBar(
	const POINT & point,
	bool bHorz,
	INT nSBMHT // CExtPaintManager::e_scroll_bar_mouse_hover_type_t
	)
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	OnSwSetPopOverScrollBarCursor( point, bHorz, nSBMHT );
	_HandlePopOverScrollBarVisibility( true );

CRect		rcSbH_entire( 0, 0, 0, 0 ), rcSbV_entire( 0, 0, 0, 0 ),
			rcSbH_paging( 0, 0, 0, 0 ), rcSbV_paging( 0, 0, 0, 0 ),
			rcSbH_thumb( 0, 0, 0, 0 ), rcSbV_thumb( 0, 0, 0, 0 ),
			rcSbH_line_up( 0, 0, 0, 0 ), rcSbV_line_up( 0, 0, 0, 0 ),
			rcSbH_line_down( 0, 0, 0, 0 ), rcSbV_line_down( 0, 0, 0, 0 ),
			rcSbH_page_up( 0, 0, 0, 0 ), rcSbV_page_up( 0, 0, 0, 0 ),
			rcSbH_page_down( 0, 0, 0, 0 ), rcSbV_page_down( 0, 0, 0, 0 );
	OnSwCalcPopOverScrollBarRects(
		rcSbH_entire, rcSbV_entire,
		rcSbH_paging, rcSbV_paging,
		rcSbH_thumb, rcSbV_thumb,
		rcSbH_line_up, rcSbV_line_up,
		rcSbH_line_down, rcSbV_line_down,
		rcSbH_page_up, rcSbV_page_up,
		rcSbH_page_down, rcSbV_page_down
		);
LONG nMinPos = 0L, nMaxPos = 0L, nPageSize = 0L, nCurrPos = 0L, nTrackPos = 0L;
	ScrollInfo32Get( bHorz ? SB_HORZ : SB_VERT, &nMinPos, &nMaxPos, &nPageSize, &nCurrPos, &nTrackPos );
CRect rcThumb = bHorz ? rcSbH_thumb : rcSbV_thumb;

INT nScrollPosStart = nCurrPos, nScrollPos = nCurrPos;
CRect rcArea( 0, 0, 0, 0 );
	switch( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) )
	{
	case CExtPaintManager::__ESBMHT_BUTTON_UP:
		rcArea = bHorz ? rcSbH_line_up : rcSbV_line_up;
	break;
	case CExtPaintManager::__ESBMHT_BUTTON_DOWN:
		rcArea = bHorz ? rcSbH_line_down : rcSbV_line_down;
	break;
	case CExtPaintManager::__ESBMHT_THUMB:
		rcArea = bHorz ? rcSbH_thumb : rcSbV_thumb;
	break;
	case CExtPaintManager::__ESBMHT_PAGE_UP:
		rcArea = bHorz ? rcSbH_page_up : rcSbV_page_up;
	break;
	case CExtPaintManager::__ESBMHT_PAGE_DOWN:
		rcArea = bHorz ? rcSbH_page_down : rcSbV_page_down;
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) )
const UINT nTimerID_zero_start = 401;
const UINT nTimerID_1st_slow = 402;
const UINT nTimerEllapse_1st_slow = 400;
const UINT nTimerID_2nd_fast = 403;
const UINT nTimerEllapse_2nd_fast = 100;
HWND hWndOwn = GetSafeHwnd();
	ASSERT( hWndOwn != NULL && ::IsWindow( hWndOwn ) );
bool bVirtualMode = false;
#if (!defined __EXT_MFC_NO_SCROLLITEMWND)
CExtScrollItemWnd * pExtScrollItemWnd = DYNAMIC_DOWNCAST( CExtScrollItemWnd, this );
	if( pExtScrollItemWnd != NULL )
	{
		DWORD dwScrollType = __ESIW_ST_NONE;
		if( bHorz )
			dwScrollType = pExtScrollItemWnd->SiwScrollTypeHGet();
		else
			dwScrollType = pExtScrollItemWnd->SiwScrollTypeVGet();
		if( dwScrollType == __ESIW_ST_VIRTUAL )
			bVirtualMode = true;
	} // if( pExtScrollItemWnd != NULL )
#endif // (!defined __EXT_MFC_NO_SCROLLITEMWND)

bool bStopFlag = false;
CPoint ptCursor( point );
INT nStepSize = 0L;
bool bUpStep = false;
bool bMouseButtonsNotSwapped = ( ::GetSystemMetrics( SM_SWAPBUTTON ) != 0 ) ? false : true;
	switch( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) )
	{
	case CExtPaintManager::__ESBMHT_BUTTON_UP:
		bUpStep = true;
		// continue falling here ...
	case CExtPaintManager::__ESBMHT_BUTTON_DOWN:
	{
		///nStepSize = OnSwGetPopOverScrollBarStepSize( bHorz ); // nStepSize = 1L;
		int nDir = ( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) == CExtPaintManager::__ESBMHT_BUTTON_DOWN ) ? (+1) : (-1);
		CSize _size = OnSwGetLineSize( nDir );
		nStepSize = bHorz ? _size.cx : _size.cy;
		if( nStepSize <= 0L )
			nStepSize = OnSwGetPopOverScrollBarStepSize( bHorz ); // nStepSize = 1L;
	}
	break;
	case CExtPaintManager::__ESBMHT_PAGE_UP:
		bUpStep = true;
		// continue falling here ...
	case CExtPaintManager::__ESBMHT_PAGE_DOWN:
	{
		///nStepSize = (INT)_psbd.m_DSI.nPage;
		int nDir = ( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) == CExtPaintManager::__ESBMHT_PAGE_DOWN ) ? (+1) : (-1);
		CSize _size = OnSwGetPageSize( nDir );
		nStepSize = bHorz ? _size.cx : _size.cy;
		if( nStepSize <= 0L )
			nStepSize = OnSwGetPopOverScrollBarStepSize( bHorz ); // nStepSize = 1L;
	}
	break;
	} // switch( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) )
bool bMenuMode = false;
	if( CExtPopupMenuWnd::IsMenuTracking() )
	{
		CWnd * pWnd = GetParent();
		for( ; pWnd != NULL; pWnd = pWnd->GetParent() )
		{
			if( pWnd->IsKindOf( RUNTIME_CLASS(CExtPopupMenuWnd) ) )
			{
				bMenuMode = true;
				break;
			} // if( pWnd == pPopup )
			if( (pWnd->GetStyle()&WS_CHILD) == 0 )
				break;
		} // for( ; pWnd != NULL; pWnd = pWnd->GetParent() )
	} // if( CExtPopupMenuWnd::IsMenuTracking() )
INT nMx = INT( nMaxPos - nPageSize + 1 );
INT nScrollLimit =
		  nMaxPos
		- nMinPos
		- nPageSize
		+ 1
		;
	ASSERT( nScrollLimit >= 0 );
	if( nStepSize > nScrollLimit )
		nStepSize = nScrollLimit;
	m_bPopOverScrollBarsTracking = true;
CRect rcScrollable = bHorz ? rcSbH_paging : rcSbV_paging;
	::SetCapture( hWndOwn );
	if( nStepSize != 0L )
	{
		::PostMessage( hWndOwn, WM_TIMER, WPARAM(nTimerID_zero_start), LPARAM(0L) );
		//::SetTimer( hWndOwn, nTimerID_1st_slow, nTimerEllapse_1st_slow, NULL );
	}
	for( MSG msg; ::IsWindow( hWndOwn ) && (!bStopFlag); )
	{
		if( ! PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			if( ! ::IsWindow( hWndOwn ) )
				break;
			::WaitMessage();
			continue;
		} // if( !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )

		bool bAnalyzeThumb = false;
		switch( msg.message )
		{
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_CANCELMODE:
		case WM_ACTIVATEAPP:
		case WM_KEYDOWN:
		case WM_KEYUP:
			bStopFlag = true;
		break;
		case WM_CAPTURECHANGED:
			if( (HWND)msg.wParam != hWndOwn )
				bStopFlag = true;
		break;
		case WM_MOUSEMOVE:
			if( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) == INT(CExtPaintManager::__ESBMHT_THUMB) )
			{
				if(		( ! CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ? VK_LBUTTON : VK_RBUTTON,true) )
					||	CExtPopupMenuWnd::IsKeyPressed( VK_MBUTTON )
					||	CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ?  VK_RBUTTON : VK_LBUTTON,true )
					||	( (!bMenuMode) && CExtPopupMenuWnd::IsMenuTracking() )
					)
				{
					bStopFlag = true;
					break;
				}
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
				bAnalyzeThumb = true;
				::GetCursorPos( &ptCursor );
				::ScreenToClient( hWndOwn, &ptCursor );
				break;
			} // if( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) == INT(CExtPaintManager::__ESBMHT_THUMB) )
			if( nStepSize == 0 )
				break;
		case WM_TIMER:
			{
				if(		( ! CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ? VK_LBUTTON : VK_RBUTTON,true) )
					||	CExtPopupMenuWnd::IsKeyPressed( VK_MBUTTON )
					||	CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ? VK_RBUTTON : VK_LBUTTON,true )
					||	( (!bMenuMode) && CExtPopupMenuWnd::IsMenuTracking() )
					)
				{
					bStopFlag = true;
					break;
				}
				if( msg.hwnd != hWndOwn )
					break;
				if(		msg.wParam != nTimerID_zero_start
					&&	msg.wParam != nTimerID_1st_slow
					&&	msg.wParam != nTimerID_2nd_fast
					)
					break;
				if( msg.wParam == nTimerID_zero_start )
					::SetTimer( hWndOwn, nTimerID_1st_slow, nTimerEllapse_1st_slow, NULL );
				else if( msg.wParam == nTimerID_1st_slow )
				{
					::KillTimer( hWndOwn, nTimerID_1st_slow );
					CExtPaintManager::stat_PassPaintMessages();
					::SetTimer( hWndOwn, nTimerID_2nd_fast, nTimerEllapse_2nd_fast, NULL );
				}
				ASSERT( nStepSize != 0L );
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
				::GetCursorPos( &ptCursor );
				::ScreenToClient( hWndOwn, &ptCursor );

				CRect rcArea2( 0, 0, 0, 0 );
				INT nSBMHT2 = INT(CExtPaintManager::__ESBMHT_NOWHERE);
				bool bHorz2 = false;
				if( ! OnSwPopOverScrollBarHitTest( ptCursor, nSBMHT2, bHorz2 ) )
					nSBMHT2 = INT(CExtPaintManager::__ESBMHT_NOWHERE);
				else
				{
					OnSwCalcPopOverScrollBarRects(
						rcSbH_entire, rcSbV_entire,
						rcSbH_paging, rcSbV_paging,
						rcSbH_thumb, rcSbV_thumb,
						rcSbH_line_up, rcSbV_line_up,
						rcSbH_line_down, rcSbV_line_down,
						rcSbH_page_up, rcSbV_page_up,
						rcSbH_page_down, rcSbV_page_down
						);
					switch( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) )
					{
					case CExtPaintManager::__ESBMHT_BUTTON_UP:
						rcArea2 = bHorz ? rcSbH_line_up : rcSbV_line_up;
					break;
					case CExtPaintManager::__ESBMHT_BUTTON_DOWN:
						rcArea2 = bHorz ? rcSbH_line_down : rcSbV_line_down;
					break;
					case CExtPaintManager::__ESBMHT_THUMB:
						rcArea2 = bHorz ? rcSbH_thumb : rcSbV_thumb;
					break;
					case CExtPaintManager::__ESBMHT_PAGE_UP:
						rcArea2 = bHorz ? rcSbH_page_up : rcSbV_page_up;
					break;
					case CExtPaintManager::__ESBMHT_PAGE_DOWN:
						rcArea2 = bHorz ? rcSbH_page_down : rcSbV_page_down;
					break;
					} // switch( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) )
				} // else from if( ! OnSwPopOverScrollBarHitTest( ptCursor, nSBMHT2, bHorz2 ) )
				
				bool bPause = false;
				if( ! rcArea.PtInRect( ptCursor ) )
					bPause = true;
				else
				{
					if(		CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) == INT(CExtPaintManager::__ESBMHT_PAGE_UP)
						||	CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) == INT(CExtPaintManager::__ESBMHT_PAGE_DOWN)
						)
					{ // update changed area rect (page up/down cases)
						if( nSBMHT2 != nSBMHT || bHorz2 != bHorz )
							bPause = true;
						else
						{
							if( ! rcArea2.PtInRect( ptCursor ) )
								bPause = true;
							else
							{
								if( bHorz )
								{
									if( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) == INT(CExtPaintManager::__ESBMHT_PAGE_UP) )
									{
										if( ptCursor.x >= rcThumb.left )
											bPause = true;
									}
									else if( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) == INT(CExtPaintManager::__ESBMHT_PAGE_DOWN) )
									{
										if( ptCursor.x <= rcThumb.right )
											bPause = true;
									}
								}
								else
								{
									if( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) == INT(CExtPaintManager::__ESBMHT_PAGE_UP) )
									{
										if( ptCursor.y >= rcThumb.top )
											bPause = true;
									}
									else if( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) == INT(CExtPaintManager::__ESBMHT_PAGE_DOWN) )
									{
										if( ptCursor.y <= rcThumb.bottom )
											bPause = true;
									}
								}
							}
						}
					} // update changed area rect (page up/down cases)
				}
				if( bPause )
					continue;

				if( bUpStep )
				{
					nScrollPos -= nStepSize;
					if( nScrollPos < nMinPos )
						nScrollPos = nMinPos;
				} // if( bUpStep )
				else
				{
					nScrollPos += nStepSize;
					if( nScrollPos > nMx )
						nScrollPos = nMx;
				} // else from if( bUpStep )
				if( _GetScrollPos( bHorz, true ) != nScrollPos )
				{
					bool bTrackPos = true;
					switch( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) )
					{
					case (CExtPaintManager::__ESBMHT_BUTTON_UP):
if( ! bVirtualMode )
//	_SetScrollPos( bHorz, nScrollPos, bTrackPos, true, true );
	_SetScrollPos( bHorz, nScrollPos, false, true, true );
					break;
					case (CExtPaintManager::__ESBMHT_BUTTON_DOWN):
if( ! bVirtualMode )
//	_SetScrollPos( bHorz, nScrollPos, bTrackPos, true, true );
	_SetScrollPos( bHorz, nScrollPos, false, true, true );
					break;
					case (CExtPaintManager::__ESBMHT_PAGE_UP):
if( ! bVirtualMode )
//	_SetScrollPos( bHorz, nScrollPos, bTrackPos, true, true );
	_SetScrollPos( bHorz, nScrollPos, false, true, true );
					break;
					case (CExtPaintManager::__ESBMHT_PAGE_DOWN):
if( ! bVirtualMode )
//	_SetScrollPos( bHorz, nScrollPos, bTrackPos, true, true );
	_SetScrollPos( bHorz, nScrollPos, false, true, true );
					break;
					case (CExtPaintManager::__ESBMHT_THUMB):
						bTrackPos = true;
if( ! bVirtualMode )
//	_SetScrollPos( bHorz, nScrollPos, bTrackPos, true, true );
	_SetScrollPos( bHorz, nScrollPos, false, true, true );
					break;
					} // switch( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(nSBMHT) )
				} // if( m_bSendEqualNotifications || _GetScrollPos( bHorz, true ) != nScrollPos )

				INT nScrollLimit =
						  nMaxPos
						- nMinPos
						- nPageSize
						+ 1
						;
				ASSERT( nScrollLimit >= 0 );
				if( nStepSize > nScrollLimit )
					nStepSize = nScrollLimit;

				rcArea = rcArea2;
			}
		break;
		default:
		{
			if(		( ! CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ? VK_LBUTTON : VK_RBUTTON,true) )
				||	CExtPopupMenuWnd::IsKeyPressed( VK_MBUTTON )
				||	CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ? VK_RBUTTON : VK_LBUTTON,true )
				||	( (!bMenuMode) && CExtPopupMenuWnd::IsMenuTracking() )
				)
				bStopFlag = true;
		}
		break;
		} // switch( msg.message )
		if( ! ::IsWindow( hWndOwn ) )
			bStopFlag = true;
		if( bStopFlag || nScrollLimit == 0L )
			break;
		if( bAnalyzeThumb )
		{
			LONG nPixelOffset = bHorz
				? (ptCursor.x - point.x)
				: (ptCursor.y - point.y);
			LONG nPixelExtent = bHorz
				? (rcScrollable.Width() - rcThumb.Width())
				: (rcScrollable.Height() - rcThumb.Height());
			if( nPixelExtent <= 0 )
			{
				bStopFlag = true;
				break;
			} // if( nPixelExtent <= 0 )
			if( abs(nPixelOffset) > nPixelExtent )
				nPixelOffset =
					(nPixelOffset < 0)
						? (-nPixelExtent)
						: nPixelExtent;
			INT nShift =
				( nPixelExtent == 0 || nPixelOffset == 0 )
					? 0
					: ::MulDiv( nScrollLimit, abs(nPixelOffset), nPixelExtent );
			nScrollPos = nScrollPosStart;
			if( nPixelOffset < 0 )
			{
				nScrollPos -= nShift;
				if( nScrollPos < nMinPos )
					nScrollPos = nMinPos;
			} // if( nPixelOffset < 0 )
			else
			{
				nScrollPos += nShift;
				if( nScrollPos > nMx )
					nScrollPos = nMx;
			} // else from if( nPixelOffset < 0 )

			if( ! bVirtualMode )
			{
				if( _GetScrollPos( bHorz, true ) != nScrollPos )
				{
//					_SetScrollPos( bHorz, nScrollPos, true );
				//	_SetScrollPos( bHorz, nScrollPos, true, true, true );
					_SetScrollPos( bHorz, nScrollPos, false, true, true );
				}
			}

			INT nScrollLimit =
					  nMaxPos
					- nMinPos
					- nPageSize
					+ 1
					;
			ASSERT( nScrollLimit >= 0 );
			ASSERT( nScrollLimit >= 0 );
			if( nStepSize > nScrollLimit )
				nStepSize = nScrollLimit;

			rcArea = rcSbH_thumb;
			continue;
		} // if( bAnalyzeThumb )
		if( ! AfxGetThread()->PumpMessage() )
			break;
	} // for( MSG msg; ::IsWindow( hWndOwn ) && (!bStopFlag); )
	if( ! ::IsWindow( hWndOwn ) )
		return;
	if( nStepSize != 0L )
	{
		::KillTimer( hWndOwn, nTimerID_1st_slow );
		::KillTimer( hWndOwn, nTimerID_2nd_fast );
	}

	m_bPopOverScrollBarsTracking = false;
	_SetScrollPos( bHorz, nScrollPos, false, true, true );

	if( ::GetCapture() == hWndOwn )
		::ReleaseCapture();

	::SendMessage(
		hWndOwn,
		bHorz ? WM_HSCROLL : WM_VSCROLL,
		MAKEWPARAM( SB_ENDSCROLL, 0 ),
		LPARAM(hWndOwn)
		);
}

void CExtScrollWnd::OnSwSetPopOverScrollBarCursor(
	const POINT & point,
	bool bHorz,
	INT nSBMHT // CExtPaintManager::e_scroll_bar_mouse_hover_type_t
	)
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	point; nSBMHT;
HCURSOR hCursor = ::LoadCursor( NULL, bHorz ? IDC_SIZEWE : IDC_SIZENS );
	if( hCursor != NULL )
		::SetCursor( hCursor );
}

bool CExtScrollWnd::OnSwPopOverScrollBarHitTest(
	const POINT & ptClient,
	INT & nSBMHT, // CExtPaintManager::e_scroll_bar_mouse_hover_type_t
	bool & bHorz
	) const
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	nSBMHT = INT(CExtPaintManager::__ESBMHT_NOWHERE);
CRect	rcSbH_entire( 0, 0, 0, 0 ), rcSbV_entire( 0, 0, 0, 0 ),
		rcSbH_paging( 0, 0, 0, 0 ), rcSbV_paging( 0, 0, 0, 0 ),
		rcSbH_thumb( 0, 0, 0, 0 ), rcSbV_thumb( 0, 0, 0, 0 ),
		rcSbH_line_up( 0, 0, 0, 0 ), rcSbV_line_up( 0, 0, 0, 0 ),
		rcSbH_line_down( 0, 0, 0, 0 ), rcSbV_line_down( 0, 0, 0, 0 ),
		rcSbH_page_up( 0, 0, 0, 0 ), rcSbV_page_up( 0, 0, 0, 0 ),
		rcSbH_page_down( 0, 0, 0, 0 ), rcSbV_page_down( 0, 0, 0, 0 );
	OnSwCalcPopOverScrollBarRects(
		rcSbH_entire, rcSbV_entire,
		rcSbH_paging, rcSbV_paging,
		rcSbH_thumb, rcSbV_thumb,
		rcSbH_line_up, rcSbV_line_up,
		rcSbH_line_down, rcSbV_line_down,
		rcSbH_page_up, rcSbV_page_up,
		rcSbH_page_down, rcSbV_page_down
		);
	if( ! rcSbH_entire.IsRectEmpty() && rcSbH_entire.PtInRect( ptClient ) )
	{
		bHorz = true;
		if( ! rcSbH_thumb.IsRectEmpty() && rcSbH_thumb.PtInRect( ptClient ) )
		{
			nSBMHT = INT(CExtPaintManager::__ESBMHT_THUMB);
			return true;
		}
		if( ! m_bLightPopOverScrollBars )
		{
			if( ! rcSbH_line_up.IsRectEmpty() && rcSbH_line_up.PtInRect( ptClient ) )
			{
				nSBMHT = INT(CExtPaintManager::__ESBMHT_BUTTON_UP);
				return true;
			}
			if( ! rcSbH_line_down.IsRectEmpty() && rcSbH_line_down.PtInRect( ptClient ) )
			{
				nSBMHT = INT(CExtPaintManager::__ESBMHT_BUTTON_DOWN);
				return true;
			}
			if( ! rcSbH_page_up.IsRectEmpty() && rcSbH_page_up.PtInRect( ptClient ) )
			{
				nSBMHT = INT(CExtPaintManager::__ESBMHT_PAGE_UP);
				return true;
			}
			if( ! rcSbH_page_down.IsRectEmpty() && rcSbH_page_down.PtInRect( ptClient ) )
			{
				nSBMHT = INT(CExtPaintManager::__ESBMHT_PAGE_DOWN);
				return true;
			}
		} // if( ! m_bLightPopOverScrollBars )
		return false;
	} // if( ! rcSbH_entire.IsRectEmpty() && rcSbH_entire.PtInRect( ptClient ) )
	if( ! rcSbV_entire.IsRectEmpty() && rcSbV_entire.PtInRect( ptClient ) )
	{
		bHorz = false;
		if( ! rcSbV_thumb.IsRectEmpty() && rcSbV_thumb.PtInRect( ptClient ) )
		{
			nSBMHT = INT(CExtPaintManager::__ESBMHT_THUMB);
			return true;
		}
		if( ! m_bLightPopOverScrollBars )
		{
			if( ! rcSbV_line_up.IsRectEmpty() && rcSbV_line_up.PtInRect( ptClient ) )
			{
				nSBMHT = INT(CExtPaintManager::__ESBMHT_BUTTON_UP);
				return true;
			}
			if( ! rcSbV_line_down.IsRectEmpty() && rcSbV_line_down.PtInRect( ptClient ) )
			{
				nSBMHT = INT(CExtPaintManager::__ESBMHT_BUTTON_DOWN);
				return true;
			}
			if( ! rcSbV_page_up.IsRectEmpty() && rcSbV_page_up.PtInRect( ptClient ) )
			{
				nSBMHT = INT(CExtPaintManager::__ESBMHT_PAGE_UP);
				return true;
			}
			if( ! rcSbV_page_down.IsRectEmpty() && rcSbV_page_down.PtInRect( ptClient ) )
			{
				nSBMHT = INT(CExtPaintManager::__ESBMHT_PAGE_DOWN);
				return true;
			}
		} // if( ! m_bLightPopOverScrollBars )
		return false;
	} // if( ! rcSbV_entire.IsRectEmpty() && rcSbV_entire.PtInRect( ptClient ) )
	return false;
}

INT CExtScrollWnd::_GetScrollPos(
	bool bHorz,
	bool bTrackPos // = false
	) const
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
LONG nMinPos = 0L, nMaxPos = 0L, nPageSize = 0L, nCurrPos = 0L, nTrackPos = 0L;
	ScrollInfo32Get( bHorz ? SB_HORZ : SB_VERT, &nMinPos, &nMaxPos, &nPageSize, &nCurrPos, &nTrackPos );
	return bTrackPos ? nTrackPos : nCurrPos;
}

void CExtScrollWnd::_SetScrollPos(
	bool bHorz,
	INT nPos,
	bool bTrackPos, // = false
	bool bRedraw, // = true
	bool bSendScrollingNotification // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	_HandlePopOverScrollBarVisibility( true );
HWND hWndLParam = m_hWnd;
CScrollBar * pScrollBar = GetScrollBarCtrl( bHorz ? SB_HORZ : SB_VERT );
	if( pScrollBar != NULL )
	{
		hWndLParam = pScrollBar->GetSafeHwnd();
		ASSERT( hWndLParam != NULL );
		CExtScrollBar * pExtScrollBar = DYNAMIC_DOWNCAST( CExtScrollBar, pScrollBar );
		if( pExtScrollBar != NULL )
		{
			pExtScrollBar->_SetScrollPos( nPos, bTrackPos, bRedraw, /*bSendScrollingNotification*/ false );
//			if( bRedraw )
//				OnSwInvalidate( true );
//			return;
		}
	}
	ScrollPos32Set( bHorz ? SB_HORZ : SB_VERT, nPos, bRedraw );
	if( bRedraw )
		OnSwInvalidate( true );

	if(		bSendScrollingNotification
		&&	OnSwHasScrollBar( bHorz )
		)
	{
		SendMessage(
			bHorz ? WM_HSCROLL : WM_VSCROLL,
			MAKEWPARAM(
				( bTrackPos ? SB_THUMBTRACK : SB_THUMBPOSITION ),
				nPos
				),
			LPARAM(hWndLParam)
			);
	}
}

void CExtScrollWnd::OnTimer( __EXT_MFC_UINT_PTR nIDEvent )
{
	ASSERT_VALID( this );
	if( nIDEvent == ( (__EXT_MFC_UINT_PTR) m_nPopOverScrollBarTimerID ) )
	{
		if( m_bPopOverScrollBarsTracking )
			return;
		if( m_bPopOverScrollBarsVisible )
		{
			CPoint ptCursor;
			if( ::GetCursorPos( &ptCursor ) && ::WindowFromPoint( ptCursor ) == m_hWnd )
			{
				ScreenToClient( &ptCursor );
				INT nSBMHT = INT(CExtPaintManager::__ESBMHT_NOWHERE);
				bool bHorz = false;
				if( OnSwPopOverScrollBarHitTest( ptCursor, nSBMHT, bHorz ) )
					return;
			} // if( ::GetCursorPos( &ptCursor ) && ::WindowFromPoint( ptCursor ) == m_hWnd )
			_HandlePopOverScrollBarVisibility( false );
		}
		return;
	} // if( nIDEvent == ( (__EXT_MFC_UINT_PTR) m_nPopOverScrollBarTimerID ) )
 	CWnd::OnTimer( nIDEvent );
}

void CExtScrollWnd::_HandlePopOverScrollBarVisibility( bool bShow )
{
	ASSERT_VALID( this );
	if( ( ! m_bDisplayPopOverScrollBars ) || m_nPopOverScrollBarTimerEllapse <= 0L )
	{
		KillTimer( m_nPopOverScrollBarTimerID );
		return;
	}
	if( bShow == m_bPopOverScrollBarsVisible )
		return;
	m_bPopOverScrollBarsVisible = bShow;
	if( GetSafeHwnd() == NULL )
		return;
	if( ! m_bPopOverScrollBarsVisible )
	{
		for( MSG msg; ::PeekMessage(&msg,NULL,WM_MOUSEFIRST,WM_MOUSELAST,PM_REMOVE); )
		{
			if( ! AfxGetThread()->PumpMessage() )
				break;
		}
	}
	KillTimer( m_nPopOverScrollBarTimerID );
	OnSwInvalidate( true );
	if( m_bPopOverScrollBarsVisible && m_nPopOverScrollBarTimerEllapse > 0 )
		SetTimer( m_nPopOverScrollBarTimerID, m_nPopOverScrollBarTimerEllapse, NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CExtScrollWnd message handlers

void CExtScrollWnd::OnSize( UINT nType, int cx, int cy )
{
	ASSERT_VALID( this );
	CWnd::OnSize( nType, cx, cy );
	if( nType != SIZE_MINIMIZED )
	{
		bool bH1 = OnSwHasScrollBar( true ), bV1 = OnSwHasScrollBar( false );
		OnSwRecalcLayout( true );
		OnSwUpdateScrollBars();
		bool bH2 = OnSwHasScrollBar( true ), bV2 = OnSwHasScrollBar( false );
		if( bH1 != bH2 || bV1 != bV2 )
			OnSwUpdateScrollBars();
		OnSwDoRedraw();
	}
}

void CExtScrollWnd::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar * pScrollBar )
{
	ASSERT_VALID( this );
	_HandlePopOverScrollBarVisibility( true );
	if(		pScrollBar != NULL
		&&	pScrollBar->SendChildNotifyLastMsg()
		)
		return;
	if( pScrollBar != GetScrollBarCtrl(SB_HORZ) )
		return;
	if( nSBCode == SB_ENDSCROLL )
		return;
	OnSwDoScroll( MAKEWORD( nSBCode, -1 ), nPos );
	if(		nSBCode == SB_ENDSCROLL
		&&	( ! OnSwQueryThumbTrackEnabled( true ) )
		)
		OnSwInvalidate( false );
}

void CExtScrollWnd::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar * pScrollBar )
{
	ASSERT_VALID( this );
	_HandlePopOverScrollBarVisibility( true );
	if(		pScrollBar != NULL
		&&	pScrollBar->SendChildNotifyLastMsg()
		)
		return;
	if( pScrollBar != GetScrollBarCtrl(SB_VERT) )
		return;
	if( nSBCode == SB_ENDSCROLL )
		return;
	OnSwDoScroll( MAKEWORD( -1, nSBCode ), nPos );
	if(		nSBCode == SB_ENDSCROLL
		&&	( ! OnSwQueryThumbTrackEnabled( false ) )
		)
		OnSwInvalidate( false );
}

BOOL CExtScrollWnd::OnMouseWheel( UINT fFlags, short zDelta, CPoint point )
{
	ASSERT_VALID( this );
	return OnSwDoMouseWheel( fFlags, zDelta, point ) ? true : false;
}

void CExtScrollWnd::OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR * lpncsp )
{
	ASSERT_VALID( this );
	CWnd::OnNcCalcSize( bCalcValidRects, lpncsp );
	if( m_bHideNormalScrollBars )
	{
 		DWORD dwWndStyle = CWnd::GetStyle();
		if( ( dwWndStyle & ( WS_HSCROLL | WS_VSCROLL ) ) != 0 )
		{
			CScrollBar * pScrollBarWndH = GetScrollBarCtrl( SB_HORZ );
			CScrollBar * pScrollBarWndV = GetScrollBarCtrl( SB_VERT );
			CRect rcNc = lpncsp->rgrc[0];
			if( pScrollBarWndH == NULL && ( dwWndStyle & WS_HSCROLL ) != 0 )
			{
				rcNc.bottom += afxData.cyHScroll;
//				if( dwWndStyle & WS_BORDER )
//					rcNc.bottom += __EXT_MFC_CY_BORDER;
			}
			if( pScrollBarWndV == NULL && ( dwWndStyle & WS_VSCROLL ) != 0 )
			{
				rcNc.right += afxData.cxVScroll;
//				if( dwWndStyle & WS_BORDER )
//					rcNc.right += __EXT_MFC_CX_BORDER;
			}
			::CopyRect( &lpncsp->rgrc[0], &rcNc );
		} // if( ( dwWndStyle & ( WS_HSCROLL | WS_VSCROLL ) ) != 0 )
	} // if( m_bHideNormalScrollBars )
}

BOOL CExtScrollWnd::OnEraseBkgnd( CDC * pDC ) 
{
	ASSERT_VALID( this );
	if( m_bEatErasing )
		return TRUE;
	return CWnd::OnEraseBkgnd(pDC);
}

void CExtScrollWnd::OnPaint() 
{
	ASSERT_VALID( this );
bool bSingleBufferPaint = false;
CPaintDC dcPaint( this );
CExtPaintManager::stat_ExcludeChildAreas(
		dcPaint.m_hDC,
		m_hWnd,
		CExtPaintManager::stat_DefExcludeChildAreaCallback
		);
CRect rcAreaBetweenScrollBarWindows( 0, 0, 0, 0 );
CScrollBar * pScrollBarH = GetScrollBarCtrl( SB_HORZ );
CScrollBar * pScrollBarV = GetScrollBarCtrl( SB_VERT );
	if(		pScrollBarH->GetSafeHwnd() != NULL
		&&	pScrollBarV->GetSafeHwnd() != NULL
		&&	( pScrollBarH->GetStyle() & WS_VISIBLE ) != 0
		&&	( pScrollBarV->GetStyle() & WS_VISIBLE ) != 0
		)
	{
		CRect rcH, rcV;
		pScrollBarH->GetWindowRect( &rcH );
		pScrollBarV->GetWindowRect( &rcV );
		ScreenToClient( &rcH );
		ScreenToClient( &rcV );
		rcAreaBetweenScrollBarWindows.SetRect( rcV.left, rcH.top, rcV.right, rcH.bottom );
	} // if( pScrollBarH->GetSafeHwnd() != NULL ...
CPoint ptSp = OnSwGetScrollPaintPos();
	if( m_bBufferedPainting )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		CExtMemoryDC dc( &dcPaint, m_bClientCB ? (&rcClient) : NULL );
		ASSERT( dc.GetSafeHdc() != NULL );
		if( dc.GetSafeHdc() != NULL )
		{
			CPoint ptVpOld( dc.SetViewportOrg( -ptSp ) );
			OnSwPaint( dc );
			dc.SetViewportOrg( ptVpOld );
			if(		(! rcAreaBetweenScrollBarWindows.IsRectEmpty() )
				&&	dc.RectVisible( &rcAreaBetweenScrollBarWindows )
				)
				OnSwPaintAreaBetweenScrollBarWindows( dc, rcAreaBetweenScrollBarWindows );
			OnSwPaintPopOverItems( dc );
		} // if( dc.GetSafeHdc() != NULL )
		else
			bSingleBufferPaint = true;
	} // if( m_bBufferedPainting )
	else
		bSingleBufferPaint = true;
	if( bSingleBufferPaint )
	{
		CPoint ptVpOld( dcPaint.SetViewportOrg( -ptSp ) );
		OnSwPaint( dcPaint );
		dcPaint.SetViewportOrg( ptVpOld );
		if(		(! rcAreaBetweenScrollBarWindows.IsRectEmpty() )
			&&	dcPaint.RectVisible( &rcAreaBetweenScrollBarWindows )
			)
			OnSwPaintAreaBetweenScrollBarWindows( dcPaint, rcAreaBetweenScrollBarWindows );
		OnSwPaintPopOverItems( dcPaint );
	} // if( bSingleBufferPaint )
}

void CExtScrollWnd::PreSubclassWindow()
{
	ASSERT_VALID( this );
	m_bPopOverScrollBarsTracking = m_bPopOverScrollBarsVisible = false;
	CWnd::PreSubclassWindow();
}

void CExtScrollWnd::PostNcDestroy()
{
	ASSERT_VALID( this );
	m_bPopOverScrollBarsTracking = m_bPopOverScrollBarsVisible = false;
	CWnd::PostNcDestroy();
}

LRESULT CExtScrollWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_PRINT || message == WM_PRINTCLIENT )
	{
		CDC * pDC = CDC::FromHandle( (HDC)wParam );
		if( (lParam&(PRF_CLIENT|PRF_ERASEBKGND)) != 0 )
		{
			OnSwPaint( *pDC );
			CScrollBar * pScrollBarH = GetScrollBarCtrl( SB_HORZ );
			CScrollBar * pScrollBarV = GetScrollBarCtrl( SB_VERT );
			if(		pScrollBarH->GetSafeHwnd() != NULL
				&&	pScrollBarV->GetSafeHwnd() != NULL
				&&	( pScrollBarH->GetStyle() & WS_VISIBLE ) != 0
				&&	( pScrollBarV->GetStyle() & WS_VISIBLE ) != 0
				)
			{
				CRect rcH, rcV;
				pScrollBarH->GetWindowRect( &rcH );
				pScrollBarV->GetWindowRect( &rcV );
				ScreenToClient( &rcH );
				ScreenToClient( &rcV );
				CRect rcAreaBetweenScrollBarWindows( rcV.left, rcH.top, rcV.right, rcH.bottom );
				if(		(! rcAreaBetweenScrollBarWindows.IsRectEmpty() )
					&&	pDC->RectVisible( &rcAreaBetweenScrollBarWindows )
					)
					OnSwPaintAreaBetweenScrollBarWindows( *pDC, rcAreaBetweenScrollBarWindows );
			} // if( pScrollBarH->GetSafeHwnd() != NULL ...
		}
		if( (lParam&PRF_CHILDREN) != 0 )
			CExtPaintManager::stat_PrintChildren(
				m_hWnd,
				message,
				pDC->GetSafeHdc(),
				lParam,
				false
				);
		return (!0);
	}

	if( ( message == WM_LBUTTONDOWN || message == WM_MOUSEMOVE ) && m_bDisplayPopOverScrollBars )
	{
		if( message == WM_MOUSEMOVE )
			_HandlePopOverScrollBarVisibility( true );
		CPoint ptCursor( LOWORD(lParam), HIWORD(lParam) );
		INT nSBMHT = INT(CExtPaintManager::__ESBMHT_NOWHERE);
		bool bHorz = false;
		if( OnSwPopOverScrollBarHitTest( ptCursor, nSBMHT, bHorz ) )
		{
			if( message == WM_LBUTTONDOWN )
				OnSwTrackPopOverScrollBar( ptCursor, bHorz, nSBMHT );
			else if( message == WM_MOUSEMOVE )
				OnSwSetPopOverScrollBarCursor( ptCursor, bHorz, nSBMHT );
			return 0L;
		} // if( OnSwPopOverScrollBarHitTest( ptCursor, nSBMHT, bHorz ) )
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

void CExtScrollWnd::OnSwPaintAreaBetweenScrollBarWindows(
	CDC & dc,
	const RECT & rcAreaBetweenScrollBarWindows
	)
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );
CRect rc = rcAreaBetweenScrollBarWindows;
CExtMemoryDC dcMem( &dc, &rc );
CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );
	if(		(! pPM->GetCb2DbTransparentMode(this ) )
		||	(! pPM->PaintDockerBkgnd( true, dcMem, this ) )
		//||	(! pPM->PaintDocumentClientAreaBkgnd( dcMem, this ) )
		)
		dc.FillSolidRect(
			&rc,
			pPM->GetColor(
				CExtPaintManager::CLR_3DFACE_OUT,
				this
				)
			);
}

BOOL CExtScrollWnd::PreTranslateMessage(MSG* pMsg) 
{
	if(		pMsg->message == WM_MOUSEWHEEL
		&&	GetSafeHwnd() != NULL
		&&	CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND( GetSafeHwnd() )
		)
	{
		CPoint point = pMsg->lParam;
		HWND hWnd = ::WindowFromPoint( point );
		if( hWnd != NULL && hWnd == m_hWnd )
		{
			UINT fFlags = LOWORD(pMsg->wParam);
			short zDelta = HIWORD(pMsg->wParam);
			if( OnSwDoMouseWheel( fFlags, zDelta, point ) )
				return TRUE;
		}
	}
	return CWnd::PreTranslateMessage(pMsg);
}

void CExtScrollWnd::OnSwEnsurePointAvail( CPoint point )
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL)
		return;
	if( ! ( OnSwHasScrollBar(false) || OnSwHasScrollBar(true) ) )
		return;
CRect rcClient;
	CWnd::GetClientRect( &rcClient );
//	if( rcClient.PtInRect(point) )
//		return;

CPoint ptScrollOrg( OnSwGetScrollPos() );
CPoint ptScroll( ptScrollOrg );
CSize sizeTotal( OnSwGetTotalSize() );

	if( OnSwHasScrollBar(true) )
	{
		if( point.x < 0 )
			ptScroll.x += point.x;
		else if( point.x > rcClient.Width() )
			ptScroll.x += point.x - rcClient.Width();
		if( ptScroll.x < 0 )
			ptScroll.x = 0;
		else if( ptScroll.x > sizeTotal.cx )
			ptScroll.x = sizeTotal.cx;
	} // if( OnSwHasScrollBar(true) )
	if( OnSwHasScrollBar(false) )
	{
		if( point.y < 0 )
			ptScroll.y += point.y;
		else if( point.y > rcClient.Height() )
			ptScroll.y += point.y - rcClient.Height();
		if( ptScroll.y < 0 )
			ptScroll.y = 0;
		else if( ptScroll.y > sizeTotal.cy )
			ptScroll.y = sizeTotal.cy;
	} // if( OnSwHasScrollBar(false) )

	if( ptScrollOrg != ptScroll )
		OnSwSetScrollPos( ptScroll );
}

#if (!defined __EXT_MFC_NO_SCROLLITEMWND)

/////////////////////////////////////////////////////////////////////////////
// CExtScrollItemWnd window & CExtScrollItemCacheInfo helper

CExtScrollItemCacheInfo::CExtScrollItemCacheInfo(
	LONG nPosH,   // = 0L
	LONG nMaxH,   // = 0L
	LONG nPageSizeH, // = 0L
	LONG nPosV,   // = 0L
	LONG nMaxV,   // = 0L
	LONG nPageSizeV, // = 0L
	bool bBeginH,    // = false
	bool bEndH,      // = false
	bool bBeginV,    // = false
	bool bEndV       // = false
	)
	: m_nPosH( nPosH )
	, m_nMaxH( nMaxH )
	, m_nPageSizeH( nPageSizeH )
	, m_nPosV( nPosV )
	, m_nMaxV( nMaxV )
	, m_nPageSizeV( nPageSizeV )
	, m_bBeginH( bBeginH )
	, m_bEndH( bEndH )
	, m_bBeginV( bBeginV )
	, m_bEndV( bEndV )
{
	ASSERT( IsValid() );
}

CExtScrollItemCacheInfo::CExtScrollItemCacheInfo(
	const CExtScrollItemCacheInfo & other
	)
{
	(*this) = other;
	ASSERT( IsValid() );
}

void CExtScrollItemCacheInfo::Assign(
	const CExtScrollItemCacheInfo & other,
	bool bAssignH, // = true
	bool bAssignV // = true
	)
{
	if( bAssignH )
	{
		m_nPosH = other.m_nPosH;
		m_nMaxH = other.m_nMaxH;
		m_nPageSizeH = other.m_nPageSizeH;
		m_bBeginH = other.m_bBeginH;
		m_bEndH = other.m_bEndH;
	} // if( bAssignH )
	if( bAssignV )
	{
		m_nPosV = other.m_nPosV;
		m_nMaxV = other.m_nMaxV;
		m_nPageSizeV = other.m_nPageSizeV;
		m_bBeginV = other.m_bBeginV;
		m_bEndV = other.m_bEndV;
	} // if( bAssignV )
	ASSERT( IsValid() );
}

void CExtScrollItemCacheInfo::AssignH(
	const CExtScrollItemCacheInfo & other
	)
{
	Assign( other, true, false );
}

void CExtScrollItemCacheInfo::AssignV(
	const CExtScrollItemCacheInfo & other
	)
{
	Assign( other, false, true );
}

CExtScrollItemCacheInfo & CExtScrollItemCacheInfo::operator = (
	const CExtScrollItemCacheInfo & other
	)
{
	Assign( other );
	return (*this);
}

bool CExtScrollItemCacheInfo::operator == (
	const CExtScrollItemCacheInfo & other
	) const
{
	return IsEqual( other );
}

bool CExtScrollItemCacheInfo::operator != (
	const CExtScrollItemCacheInfo & other
	) const
{
	return ( ! IsEqual( other ) );
}

void CExtScrollItemCacheInfo::Empty(
	bool bEmptyH, // = true
	bool bEmptyV // = true
	)
{
	ASSERT( IsValid() );
	if( bEmptyH )
	{
		m_nPosH = 0L;
		m_nMaxH = 0L;
		m_nPageSizeH = 0L;
		m_bBeginH = false;
		m_bEndH = false;
	} // if( bEmptyH )
	if( bEmptyV )
	{
		m_nPosV = 0L;
		m_nMaxV = 0L;
		m_nPageSizeV = 0L;
		m_bBeginV = false;
		m_bEndV = false;
	} // if( bEmptyV )
}

void CExtScrollItemCacheInfo::EmptyH()
{
	ASSERT( IsValid() );
	Empty( true, false );
}

void CExtScrollItemCacheInfo::EmptyV()
{
	ASSERT( IsValid() );
	Empty( false, true );
}

bool CExtScrollItemCacheInfo::IsEmpty(
	bool bCheckEmptyH, // = true
	bool bCheckEmptyV // = true
	) const
{
	ASSERT( IsValid() );
	if(		(	bCheckEmptyH
			&&	m_nMaxH != 0L
			)
		||	(	bCheckEmptyV
			&&	m_nMaxV != 0L
			)
		)
		return false;
	return true;
}

bool CExtScrollItemCacheInfo::IsEmptyH() const
{
	ASSERT( IsValid() );
	return IsEmpty( true, false );
}

bool CExtScrollItemCacheInfo::IsEmptyV() const
{
	ASSERT( IsValid() );
	return IsEmpty( false, true );
}

bool CExtScrollItemCacheInfo::IsValid() const
{
	if(		m_nPosH >= 0L
		&&	m_nPosV >= 0L
		&&	m_nMaxH >= 0L
		&&	m_nMaxV >= 0L
		&&	m_nPageSizeH >= 0L
		&&	m_nPageSizeV >= 0L
		&&	0L <= m_nPosH && m_nPosH <= m_nMaxH
		&&	0L <= m_nPosV && m_nPosV <= m_nMaxV
		)
		return true;
	return false;
}

bool CExtScrollItemCacheInfo::IsEqual( // static
	const CExtScrollItemCacheInfo & left,
	const CExtScrollItemCacheInfo & right,
	bool bComparePosH,   // = true
	bool bComparePosV,   // = true
	bool bCompareMaxH,   // = true
	bool bCompareMaxV,   // = true
	bool bComparePageSizeH, // = true
	bool bComparePageSizeV, // = true
	bool bCompareBofH,      // = false
	bool bCompareBofV,      // = false
	bool bCompareEofH,      // = false
	bool bCompareEofV       // = false
	)
{
	ASSERT( left.IsValid() );
	ASSERT( left.IsValid() );
	if(		(	bComparePosH
			&&	left.m_nPosH != right.m_nPosH
			)
		||	(	bComparePosV
			&&	left.m_nPosV != right.m_nPosV
			)
		||	(	bCompareMaxH
			&&	left.m_nMaxH != right.m_nMaxH
			)
		||	(	bCompareMaxV
			&&	left.m_nMaxV != right.m_nMaxV
			)
		||	(	bComparePageSizeH
			&&	left.m_nPageSizeH != right.m_nPageSizeH
			)
		||	(	bComparePageSizeV
			&&	left.m_nPageSizeV != right.m_nPageSizeV
			)
		||	(	bCompareBofH
			&&	left.m_bBeginH != right.m_bBeginH
			)
		||	(	bCompareBofV
			&&	left.m_bBeginV != right.m_bBeginV
			)
		||	(	bCompareEofH
			&&	left.m_bEndH != right.m_bEndH
			)
		||	(	bCompareEofV
			&&	left.m_bEndV != right.m_bEndV
			)
		)
		return false;
	return true;
}

bool CExtScrollItemCacheInfo::IsEqual(
	const CExtScrollItemCacheInfo & other,
	bool bComparePosH,   // = true
	bool bComparePosV,   // = true
	bool bCompareMaxH,   // = true
	bool bCompareMaxV,   // = true
	bool bComparePageSizeH, // = true
	bool bComparePageSizeV, // = true
	bool bCompareBofH,      // = false
	bool bCompareBofV,      // = false
	bool bCompareEofH,      // = false
	bool bCompareEofV       // = false
	) const
{
	return
		IsEqual(
			*this, other,
			bComparePosH, bComparePosV,
			bCompareMaxH, bCompareMaxV,
			bComparePageSizeH, bComparePageSizeV,
			bCompareBofH, bCompareBofV,
			bCompareEofH, bCompareEofV
			);
}

LONG CExtScrollItemCacheInfo::GetExtentH() const
{
	ASSERT( IsValid() );
	return (m_nMaxH + m_nPageSizeH);
}

LONG CExtScrollItemCacheInfo::GetExtentV() const
{
	ASSERT( IsValid() );
	return (m_nMaxV + m_nPageSizeV);
}

LONG CExtScrollItemCacheInfo::GetPosChangingH(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_nPosH - _sciOld.m_nPosH);
}

LONG CExtScrollItemCacheInfo::GetPosChangingV(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_nPosV - _sciOld.m_nPosV);
}

LONG CExtScrollItemCacheInfo::GetMaxChangingH(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_nMaxH - _sciOld.m_nMaxH);
}

LONG CExtScrollItemCacheInfo::GetMaxChangingV(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_nMaxV - _sciOld.m_nMaxV);
}

LONG CExtScrollItemCacheInfo::GetExtentChangingH(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
LONG nExtentThis = GetExtentH();
LONG nExtentOld = _sciOld.GetExtentH();
	return (nExtentThis - nExtentOld);
}

LONG CExtScrollItemCacheInfo::GetExtentChangingV(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
LONG nExtentThis = GetExtentV();
LONG nExtentOld = _sciOld.GetExtentV();
	return (nExtentThis - nExtentOld);
}

LONG CExtScrollItemCacheInfo::GetPageSizeChangingH(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_nPageSizeH - _sciOld.m_nPageSizeH);
}

LONG CExtScrollItemCacheInfo::GetPageSizeChangingV(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_nPageSizeV - _sciOld.m_nPageSizeV);
}

bool CExtScrollItemCacheInfo::IsShiftToBofH(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_bBeginH&&(!_sciOld.m_bBeginH)) ? true : false;
}

bool CExtScrollItemCacheInfo::IsShiftToBofV(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_bBeginV&&(!_sciOld.m_bBeginV)) ? true : false;
}

bool CExtScrollItemCacheInfo::IsShiftToEofH(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_bEndH&&(!_sciOld.m_bEndH)) ? true : false;
}

bool CExtScrollItemCacheInfo::IsShiftToEofV(
	const CExtScrollItemCacheInfo & _sciOld
	) const
{
	ASSERT( IsValid() );
	return (m_bEndV&&(!_sciOld.m_bEndV)) ? true : false;
}

void CExtScrollItemCacheInfo::FlipHV()
{
	ASSERT( IsValid() );
LONG nTemp;
bool bTemp;
	
	nTemp = m_nPosH;
	m_nPosH = m_nPosV;
	m_nPosV = nTemp;

	nTemp = m_nMaxH;
	m_nMaxH = m_nMaxV;
	m_nMaxV = nTemp;

	nTemp = m_nPageSizeH;
	m_nPageSizeH = m_nPageSizeV;
	m_nPageSizeV = nTemp;

	bTemp = m_bBeginH;
	m_bBeginH = m_bBeginV;
	m_bBeginV = bTemp;

	bTemp = m_bEndH;
	m_bEndH = m_bEndV;
	m_bEndV = bTemp;
	
	ASSERT( IsValid() );
}

IMPLEMENT_DYNCREATE( CExtScrollItemWnd, CExtScrollWnd );

CExtScrollItemWnd::CExtScrollItemWnd()
	: m_bDirectCreateCall( false )
	, m_dwScrollItemWndStyle( 0L )
	, m_dwScrollItemWndStyleEx( 0L )
	, m_bExtractResourceStyles( true )
{
	VERIFY( RegisterScrollItemWndClass() );
}

CExtScrollItemWnd::~CExtScrollItemWnd()
{
}

BEGIN_MESSAGE_MAP( CExtScrollItemWnd, CExtScrollWnd )
	//{{AFX_MSG_MAP(CExtScrollItemWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool CExtScrollItemWnd::g_bScrollItemWndClassRegistered = false;

bool CExtScrollItemWnd::RegisterScrollItemWndClass()
{
	if( g_bScrollItemWndClassRegistered )
		return true;
WNDCLASS _wndClassInfo;
HINSTANCE hInst = ::AfxGetInstanceHandle();
	if( ! ::GetClassInfo( hInst, __EXT_SCROLLITEMWND_CLASS_NAME, &_wndClassInfo ) )
	{
		_wndClassInfo.style = CS_GLOBALCLASS|CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW;
		_wndClassInfo.lpfnWndProc = ::DefWindowProc;
		_wndClassInfo.cbClsExtra = _wndClassInfo.cbWndExtra = 0;
		_wndClassInfo.hInstance = hInst;
		_wndClassInfo.hIcon = NULL;
		_wndClassInfo.hCursor = ::LoadCursor( NULL, IDC_ARROW );
		ASSERT( _wndClassInfo.hCursor != NULL );
		_wndClassInfo.hbrBackground = NULL; 
		_wndClassInfo.lpszMenuName = NULL;
		_wndClassInfo.lpszClassName = __EXT_SCROLLITEMWND_CLASS_NAME;
		if( ! ::AfxRegisterClass( &_wndClassInfo ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}
	g_bScrollItemWndClassRegistered = true;
	return true;
}

BOOL CExtScrollItemWnd::Create(
	CWnd * pParentWnd,
	const RECT & rcWnd, // = CRect( 0, 0, 0, 0 )
	UINT nDlgCtrlID, // = UINT( __EXT_MFC_IDC_STATIC )
	DWORD dwScrollItemWndStyle, // = 0L
	DWORD dwWindowStyle, // = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
	CCreateContext * pContext // = NULL
	)
{
	ASSERT_VALID( this );
	pContext;
	if( ! RegisterScrollItemWndClass() )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	m_bDirectCreateCall = true;
	m_dwScrollItemWndStyle = dwScrollItemWndStyle;
	if( ! CWnd::CreateEx( 0L, __EXT_SCROLLITEMWND_CLASS_NAME, _T(""), dwWindowStyle, rcWnd, pParentWnd, nDlgCtrlID ) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	if( !_CreateHelper() )
	{
		ASSERT( FALSE );
		AfxThrowMemoryException();
	}
	return TRUE;
}

BOOL CExtScrollItemWnd::PreCreateWindow( CREATESTRUCT & cs ) 
{
	ASSERT_VALID( this );
	if(		( ! RegisterScrollItemWndClass() )
		||	( ! CWnd::PreCreateWindow( cs ) )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	cs.lpszClass = __EXT_SCROLLITEMWND_CLASS_NAME;
	cs.style |= WS_CLIPSIBLINGS|WS_CLIPCHILDREN;
	return TRUE;
}

bool CExtScrollItemWnd::_CreateHelper()
{
	ASSERT_VALID( this );
	OnSwUpdateScrollBars();
	OnSwDoRedraw();
	return true;
}

void CExtScrollItemWnd::PreSubclassWindow()
{
	ASSERT_VALID( this );
	CExtScrollWnd::PreSubclassWindow();
	if( m_bDirectCreateCall )
		return;
	if( m_bExtractResourceStyles )
	{
		__EXT_MFC_LONG_PTR dwStyle = ::__EXT_MFC_GetWindowLong( m_hWnd, GWL_STYLE );
		m_dwScrollItemWndStyle = DWORD( dwStyle & __EXTMFC_ALL_FORM_MOVABLE_WND_STYLES );
		::__EXT_MFC_SetWindowLong( m_hWnd, GWL_STYLE, dwStyle & (~__EXTMFC_ALL_FORM_MOVABLE_WND_STYLES) );
	} // if( m_bExtractResourceStyles )
	
	if( !_CreateHelper() )
	{
		ASSERT( FALSE );
		AfxThrowMemoryException();
	} // if( !Create() )
}

void CExtScrollItemWnd::PostNcDestroy()
{
	ASSERT_VALID( this );
	CExtScrollWnd::PostNcDestroy();
}

DWORD CExtScrollItemWnd::SiwGetStyle() const
{
	ASSERT( this != NULL );
	return m_dwScrollItemWndStyle;
}

DWORD CExtScrollItemWnd::SiwModifyStyle(
	DWORD dwStyleAdd,
	DWORD dwStyleRemove, // = 0L
	bool bUpdateWnd // = true
	)
{
	ASSERT( this != NULL );
DWORD dwScrollItemWndStyleOld = SiwGetStyle();
	if( dwStyleAdd == 0 && dwStyleRemove == 0 )
		return dwScrollItemWndStyleOld;
	m_dwScrollItemWndStyle &= ~dwStyleRemove;
	m_dwScrollItemWndStyle |= dwStyleAdd;
	if(		bUpdateWnd
		&&	GetSafeHwnd() != NULL
		)
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	}
	return dwScrollItemWndStyleOld;
}

DWORD CExtScrollItemWnd::SiwGetStyleEx() const
{
	ASSERT( this != NULL );
	return m_dwScrollItemWndStyleEx;
}

DWORD CExtScrollItemWnd::SiwModifyStyleEx(
	DWORD dwStyleExAdd,
	DWORD dwStyleExRemove, // = 0L
	bool bUpdateWnd // = true
	)
{
	ASSERT( this != NULL );
DWORD dwScrollItemWndStyleExOld = SiwGetStyle();
	if( dwStyleExAdd == 0 && dwStyleExRemove == 0 )
		return dwScrollItemWndStyleExOld;
	m_dwScrollItemWndStyleEx &= ~dwStyleExRemove;
	m_dwScrollItemWndStyleEx |= dwStyleExAdd;
	if(		bUpdateWnd
		&&	GetSafeHwnd() != NULL
		)
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	}
	return dwScrollItemWndStyleExOld;
}

DWORD CExtScrollItemWnd::SiwScrollTypeHGet() const
{
	ASSERT( this != NULL );
DWORD dwScrollItemWndStyle = SiwGetStyle();
	return (dwScrollItemWndStyle&__ESIS_STH_MASK);
}

DWORD CExtScrollItemWnd::SiwScrollTypeVGet() const
{
	ASSERT( this != NULL );
DWORD dwScrollItemWndStyle = SiwGetStyle();
	return ((dwScrollItemWndStyle&__ESIS_STV_MASK) >> __ESIW_ST_BIT_COUNT);
}

DWORD CExtScrollItemWnd::SiwScrollTypeHSet(
	DWORD dwScrollTypeNew,
	bool bUpdateWnd // = true
	)
{
	ASSERT( this != NULL );
	dwScrollTypeNew &= __ESIW_ST_MASK;
DWORD dwScrollTypeOld = SiwScrollTypeHGet();
	if( dwScrollTypeNew == dwScrollTypeOld )
		return dwScrollTypeOld;
	SiwModifyStyle(
		dwScrollTypeNew,
		__ESIS_STH_MASK,
		bUpdateWnd
		);
	return dwScrollTypeOld;
}

DWORD CExtScrollItemWnd::SiwScrollTypeVSet(
	DWORD dwScrollTypeNew,
	bool bUpdateWnd // = true
	)
{
	ASSERT( this != NULL );
	dwScrollTypeNew &= __ESIW_ST_MASK;
DWORD dwScrollTypeOld = SiwScrollTypeVGet();
	if( dwScrollTypeNew == dwScrollTypeOld )
		return dwScrollTypeOld;
	SiwModifyStyle(
		dwScrollTypeNew << __ESIW_ST_BIT_COUNT,
		__ESIS_STV_MASK,
		bUpdateWnd
		);
	return dwScrollTypeOld;
}

bool CExtScrollItemWnd::SiwPreferHorizontalWalkGet() const
{
	ASSERT_VALID( this );
bool bPreferHorizontalWalk =
	( (SiwGetStyle() & __ESIS_PREFER_HORZ_WALK) != 0 )
		? true : false;
	return bPreferHorizontalWalk;
}

bool CExtScrollItemWnd::SiwPreferHorizontalWalkSet(
	bool bPreferHorizontalWalk
	)
{
bool bPreferHorizontalWalkOld = SiwPreferHorizontalWalkGet();
	if(		(bPreferHorizontalWalk && bPreferHorizontalWalkOld)
		||	((!bPreferHorizontalWalk) && (!bPreferHorizontalWalkOld))
		)
		return bPreferHorizontalWalkOld;
	SiwModifyStyle(
		( (bPreferHorizontalWalk) ? __ESIS_PREFER_HORZ_WALK : 0 ),
		__ESIS_PREFER_HORZ_WALK,
		false
		);
	return bPreferHorizontalWalkOld;
}

CSize CExtScrollItemWnd::OnSwGetLineSize( int nDirection ) const
{
	ASSERT_VALID( this );
	nDirection;
static const CSize g_sizePixelScrollStep( 3, 3 );
CSize _sizeLine( 0, 0 );
DWORD dwScrollType = SiwScrollTypeHGet();
	if( dwScrollType == __ESIW_ST_PIXEL )
		_sizeLine.cx = g_sizePixelScrollStep.cx;
	else if(	dwScrollType == __ESIW_ST_ITEM
			||	dwScrollType == __ESIW_ST_VIRTUAL
			)
		_sizeLine.cx = 1;
	dwScrollType = SiwScrollTypeVGet();
	if( dwScrollType == __ESIW_ST_PIXEL )
		_sizeLine.cy = g_sizePixelScrollStep.cy;
	else if(	dwScrollType == __ESIW_ST_ITEM
			||	dwScrollType == __ESIW_ST_VIRTUAL
			)
		_sizeLine.cy = 1;
	return _sizeLine;
}

void CExtScrollItemWnd::OnSwUpdateScrollBars()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
	if( m_nUpdateScrollBars > 1 )
		return;
	m_nUpdateScrollBars ++;

	ASSERT( OnSwGetTotalSize().cx >= 0 && OnSwGetTotalSize().cy >= 0 );
CRect rcClient;
bool bCalcClient = true;
CWnd * pParentWnd = GetParent();
	if(		pParentWnd != NULL
		&&	(BOOL)pParentWnd->SendMessage(
				WM_RECALCPARENT,
				0,
				(LPARAM)(LPCRECT)&rcClient
				) != 0
		)
		bCalcClient = false;
CSize sizeClient;
CSize sizeSb;
	if( bCalcClient )
	{
		if( !OnSwCalcClientSizes( sizeClient, sizeSb ) )
		{ // no room for scroll bars
			CRect rcClient2 = OnSwGetClientRect();
			if( rcClient2.Width() > 0 && rcClient2.Height()> 0 )
			{
				OnSwEnableScrollBarCtrl( SB_BOTH, false );
				OnSwRecalcLayout( true );
			}
			m_nUpdateScrollBars --;
			return;
		}
	}
	else
	{ // let parent window determine the "client" rect
		sizeSb = OnSwGetScrollBarSizes();
		sizeClient.cx = rcClient.right - rcClient.left;
		sizeClient.cy = rcClient.bottom - rcClient.top;
	}

DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if( dwScrollTypeH == __ESIW_ST_ITEM || dwScrollTypeV == __ESIW_ST_ITEM )
	{
		CSize _sizeVR = OnSiwGetVisibleRange().Size();
		if( dwScrollTypeH == __ESIW_ST_ITEM )
			sizeClient.cx = _sizeVR.cx;
		if( dwScrollTypeV == __ESIW_ST_ITEM )
			sizeClient.cy = _sizeVR.cy;
	} // if( dwScrollTypeH == __ESIW_ST_ITEM || dwScrollTypeV == __ESIW_ST_ITEM )

bool bHasSbH0 = OnSwHasScrollBar( true );
bool bHasSbV0 = OnSwHasScrollBar( false );

	// if enough room to add scrollbars
CSize sizeRange;
CPoint ptMove;
CSize sizeNeedSB;
	OnSwGetScrollBarState(
		sizeClient,
		sizeNeedSB,
		sizeRange,
		ptMove,
		bCalcClient
		);

//	if( bCalcClient )
//	{
//		if(		sizeNeedSB.cx != 0
//			&&	dwScrollTypeV == __ESIW_ST_PIXEL
//			//&&	bHasSbH0
//			)
//			sizeClient.cy -= sizeSb.cy;
//		if(		sizeNeedSB.cy != 0
//			&&	dwScrollTypeH == __ESIW_ST_PIXEL
//			//&&	bHasSbV0
//			)
//			sizeClient.cx -= sizeSb.cx;
//	} // if( bCalcClient )

	// scroll window + update
CPoint ptCurrScrollPos = OnSwGetScrollPos();
	if( ptMove != ptCurrScrollPos )
		OnSwSetScrollPos( ptMove );

	// the scrollbar page range
SCROLLINFO _scroll_info;
	::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
	_scroll_info.cbSize = sizeof(SCROLLINFO);
	_scroll_info.fMask = SIF_PAGE|SIF_RANGE;
	_scroll_info.nMin = 0;

//DWORD dwScrollTypeH = SiwScrollTypeHGet();
//DWORD dwScrollTypeV = SiwScrollTypeVGet();

CSize _sizeTotalISR = OnSwGetTotalSize();
	if( _sizeTotalISR.cx < 0 )
		_sizeTotalISR.cx = __EXT_SCROLL_NUMERIC_MAX;
	if( _sizeTotalISR.cy < 0 )
		_sizeTotalISR.cy = __EXT_SCROLL_NUMERIC_MAX;
CSize _sizePageISR = OnSwGetPageSize( 0 );

	// update bars
	if( dwScrollTypeH == __ESIW_ST_NONE )
		OnSwEnableScrollBarCtrl( SB_HORZ, false );
	else if( dwScrollTypeH == __ESIW_ST_PIXEL )
	{
		OnSwEnableScrollBarCtrl( SB_HORZ, sizeNeedSB.cx ? true : false );
		if( sizeNeedSB.cx )
		{
			_scroll_info.nPage = sizeClient.cx;
			_scroll_info.nMax = OnSwGetTotalSize().cx - 1;
			if( bHasSbH0 && bHasSbV0 )
				_scroll_info.nMax += OnSwGetScrollBarSizes().cx;
			if( ! OnSwScrollInfoAdjust(
					SB_HORZ,
					_scroll_info,
					true
					)
				)
				OnSwSetScrollRange(
					SB_HORZ,
					0,
					sizeRange.cx,
					true
					);
		} // if( sizeNeedSB.cx )
	} // else if( dwScrollTypeH == __ESIW_ST_PIXEL )
	else if( dwScrollTypeH == __ESIW_ST_ITEM )
	{
		if( _sizeTotalISR.cx > _sizePageISR.cx )
		{
			ASSERT( _sizePageISR.cx >= 0 );
			OnSwEnableScrollBarCtrl( SB_HORZ, true );
			_scroll_info.nPage = _sizePageISR.cx;
			_scroll_info.nMax = _sizeTotalISR.cx - 1;
			if( ! OnSwScrollInfoAdjust(
					SB_HORZ,
					_scroll_info,
					true
					)
				)
				OnSwSetScrollRange(
					SB_HORZ,
					0,
					_sizeTotalISR.cx,
					true
					);
		}
		else
			OnSwEnableScrollBarCtrl( SB_HORZ, false );
	} // else if( dwScrollTypeH == __ESIW_ST_ITEM )
	else
	{ // if __ESIW_ST_VIRTUAL
		ASSERT( dwScrollTypeH == __ESIW_ST_VIRTUAL );
		bool bBOF = OnSiwQueryVirtualBOF( true );
		bool bEOF = OnSiwQueryVirtualEOF( true );
		bool bEnableSB = true;
		if( bBOF )
		{
			if( bEOF )
				bEnableSB = false;
			else
				_scroll_info.nPos = 0;
		} // if( bBOF )
		else
		{
			if( bEOF )
				_scroll_info.nPos = __EXT_SCROLL_NUMERIC_MAX - 1;
			else
				_scroll_info.nPos = __EXT_SCROLL_NUMERIC_MAX / 2;
		} // else from if( bBOF )
		if( bEnableSB )
		{
			OnSwEnableScrollBarCtrl( SB_HORZ, true );
			_scroll_info.fMask = SIF_PAGE|SIF_RANGE|SIF_POS;
			_scroll_info.nPage = _sizePageISR.cx;
			_scroll_info.nMax = __EXT_SCROLL_NUMERIC_MAX;
			if( ! OnSwScrollInfoAdjust(
					SB_HORZ,
					_scroll_info,
					true
					)
				)
				OnSwSetScrollRange(
					SB_HORZ,
					0,
					_sizeTotalISR.cx,
					true
					);
		} // if( bEnableSB )
		else
			OnSwEnableScrollBarCtrl( SB_HORZ, false );
	} // if __ESIW_ST_VIRTUAL

	// recalc for vertical parms
	_sizeTotalISR = OnSwGetTotalSize();
	if( _sizeTotalISR.cx < 0 )
		_sizeTotalISR.cx = __EXT_SCROLL_NUMERIC_MAX;
	if( _sizeTotalISR.cy < 0 )
		_sizeTotalISR.cy = __EXT_SCROLL_NUMERIC_MAX;
	_sizePageISR = OnSwGetPageSize( 0 );

	if( dwScrollTypeV == __ESIW_ST_NONE )
		OnSwEnableScrollBarCtrl( SB_VERT, false );
	else if( dwScrollTypeV == __ESIW_ST_PIXEL )
	{
		OnSwEnableScrollBarCtrl( SB_VERT, sizeNeedSB.cy ? true : false );
		if( sizeNeedSB.cy )
		{
			_scroll_info.nPage = sizeClient.cy;
			_scroll_info.nMax = OnSwGetTotalSize().cy - 1;
			if( bHasSbH0 && bHasSbV0 )
				_scroll_info.nMax += OnSwGetScrollBarSizes().cy;
			if( ! OnSwScrollInfoAdjust(
					SB_VERT,
					_scroll_info,
					true
					)
				)
				OnSwSetScrollRange(
					SB_VERT,
					0,
					sizeRange.cy,
					true
					);
		} // if( sizeNeedSB.cy )
	} // else if( dwScrollTypeV == __ESIW_ST_PIXEL )
	else if( dwScrollTypeV == __ESIW_ST_ITEM )
	{
		if( _sizeTotalISR.cy > _sizePageISR.cy )
		{
			ASSERT( _sizePageISR.cy >= 0 );
			OnSwEnableScrollBarCtrl( SB_VERT, true );
			_scroll_info.nPage = _sizePageISR.cy;
			_scroll_info.nMax = _sizeTotalISR.cy - 1;
			if( ! OnSwScrollInfoAdjust(
					SB_VERT,
					_scroll_info,
					true
					)
				)
				OnSwSetScrollRange(
					SB_VERT,
					0,
					_sizeTotalISR.cy,
					true
					);
		}
		else
			OnSwEnableScrollBarCtrl( SB_VERT, false );
	} // else if( dwScrollTypeV == __ESIW_ST_ITEM )
	else
	{ // if __ESIW_ST_VIRTUAL
		ASSERT( dwScrollTypeV == __ESIW_ST_VIRTUAL );
		bool bBOF = OnSiwQueryVirtualBOF( false );
		bool bEOF = OnSiwQueryVirtualEOF( false );
		bool bEnableSB = true;
		if( bBOF )
		{
			if( bEOF )
				bEnableSB = false;
			else
				_scroll_info.nPos = 0;
		} // if( bBOF )
		else
		{
			if( bEOF )
				_scroll_info.nPos = __EXT_SCROLL_NUMERIC_MAX - 1;
			else
				_scroll_info.nPos = __EXT_SCROLL_NUMERIC_MAX / 2;
		} // else from if( bBOF )
		if( bEnableSB )
		{
			OnSwEnableScrollBarCtrl( SB_VERT, true );
			_scroll_info.fMask = SIF_PAGE|SIF_RANGE|SIF_POS;
			_scroll_info.nPage = _sizePageISR.cy;
			_scroll_info.nMax = __EXT_SCROLL_NUMERIC_MAX;
			if( ! OnSwScrollInfoAdjust(
					SB_VERT,
					_scroll_info,
					true
					)
				)
				OnSwSetScrollRange(
					SB_VERT,
					0,
					_sizeTotalISR.cy,
					true
					);
		} // if( bEnableSB )
		else
			OnSwEnableScrollBarCtrl( SB_VERT, false );
	} // if __ESIW_ST_VIRTUAL

// adjust virtual position and cache state
//	if(		dwScrollTypeH == __ESIW_ST_VIRTUAL
//		||	dwScrollTypeV == __ESIW_ST_VIRTUAL
//		)
		OnSwDoScrollBy( CSize(0,0), false );
	OnSwRecalcLayout( true );

bool bHasSbH1 = OnSwHasScrollBar( true );
bool bHasSbV1 = OnSwHasScrollBar( false );
	if(		bHasSbH0 != bHasSbH1
		||	bHasSbV0 != bHasSbV1
//		||	dwScrollTypeH == __ESIW_ST_VIRTUAL
//		||	dwScrollTypeV == __ESIW_ST_VIRTUAL
		)
		OnSwUpdateScrollBars();
	else
	{
		if(		( bHasSbH1 && GetScrollBarCtrl( SB_HORZ )->GetSafeHwnd() == NULL )
			||	( bHasSbV1 && GetScrollBarCtrl( SB_VERT )->GetSafeHwnd() == NULL )
			)
			SendMessage( WM_NCPAINT );
	}

	m_nUpdateScrollBars --;
}

bool CExtScrollItemWnd::SiwAutohideScrollBarGet( bool bHorz ) const
{
	ASSERT_VALID( this );
	if( bHorz )
	{
		if( ( SiwGetStyle() & __ESIS_DISABLE_AUTOHIDE_SB_H ) != 0 )
			return false;
	} // if( bHorz )
	else
	{
		if( ( SiwGetStyle() & __ESIS_DISABLE_AUTOHIDE_SB_V ) != 0 )
			return false;
	} // else from if( bHorz )
	return true;
}

bool CExtScrollItemWnd::SiwAutohideScrollBarSet(
	bool bHorz,
	bool bAutoHide // = true
	)
{
	ASSERT_VALID( this );
bool bAutohideScrollBarOld = SiwThumbTrackEnabledGet( bHorz );
	if(		(bAutohideScrollBarOld && bAutoHide)
		||	((!bAutohideScrollBarOld) && (!bAutoHide))
		)
		return bAutohideScrollBarOld;
DWORD dwRemove = bHorz ? __ESIS_DISABLE_AUTOHIDE_SB_H : __ESIS_DISABLE_AUTOHIDE_SB_V;
DWORD dwAdd = bAutoHide ? 0L : dwRemove;
	SiwModifyStyle( dwAdd, dwRemove, false );
	return bAutohideScrollBarOld;
}

bool CExtScrollItemWnd::SiwThumbTrackEnabledGet( bool bHorz ) const
{
	ASSERT_VALID( this );
	if( bHorz )
	{
		if( ( SiwGetStyle() & __ESIS_DISABLE_THUMBTRACK_H ) != 0 )
			return false;
	} // if( bHorz )
	else
	{
		if( ( SiwGetStyle() & __ESIS_DISABLE_THUMBTRACK_V ) != 0 )
			return false;
	} // else from if( bHorz )
	return true;
}

bool CExtScrollItemWnd::SiwThumbTrackEnabledSet(
	bool bHorz,
	bool bEnabled // = true
	)
{
	ASSERT_VALID( this );
bool bThumbTrackEnabledOld = SiwThumbTrackEnabledGet( bHorz );
	if(		(bThumbTrackEnabledOld && bEnabled)
		||	((!bThumbTrackEnabledOld) && (!bEnabled))
		)
		return bThumbTrackEnabledOld;
DWORD dwRemove = bHorz ? __ESIS_DISABLE_THUMBTRACK_H : __ESIS_DISABLE_THUMBTRACK_V;
DWORD dwAdd = bEnabled ? 0L : dwRemove;
	SiwModifyStyle( dwAdd, dwRemove, false );
	return bThumbTrackEnabledOld;
}

bool CExtScrollItemWnd::OnSwQueryThumbTrackEnabled( bool bHorz ) const
{
	ASSERT_VALID( this );
	if( ! SiwThumbTrackEnabledGet( bHorz ) )
		return false;
DWORD dwScrollType = bHorz
		? SiwScrollTypeHGet()
		: SiwScrollTypeVGet()
		;
	if( dwScrollType == __ESIW_ST_VIRTUAL )
	{
		CScrollBar * pScrollBar =
			GetScrollBarCtrl(
				bHorz
					? SB_HORZ
					: SB_VERT
				);
		if( pScrollBar->GetSafeHwnd() != NULL )
		{
			CExtScrollBar * pExtScrollBar =
				DYNAMIC_DOWNCAST(
					CExtScrollBar,
					pScrollBar
					);
			if(		pExtScrollBar != NULL
				&&	pExtScrollBar->m_bCompleteRepaint
				)
				return true;
		} // if( pScrollBar->GetSafeHwnd() != NULL )
		return false;
	} // if( dwScrollType == __ESIW_ST_VIRTUAL )
	return true;
}

CSize CExtScrollItemWnd::OnSwGetTotalSize() const
{
	ASSERT_VALID( this );
CSize _sizeTotalISR( 0, 0 );
DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if( dwScrollTypeH == __ESIW_ST_VIRTUAL )
		_sizeTotalISR.cx = __EXT_SCROLL_NUMERIC_MAX;
	if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
		_sizeTotalISR.cy = __EXT_SCROLL_NUMERIC_MAX;
	return _sizeTotalISR;
}

CSize CExtScrollItemWnd::OnSwGetPageSize( int nDirection ) const
{
	ASSERT_VALID( this );
CRect rcClient = OnSwGetClientRect();
	if( rcClient.right < rcClient.left )
		rcClient.right = rcClient.left;
	if( rcClient.bottom < rcClient.top )
		rcClient.bottom = rcClient.top;
CSize _sizeClient = rcClient.Size();
CSize _sizePage = OnSiwCalcPageMetrics( nDirection );

DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();

	if( dwScrollTypeH == __ESIW_ST_NONE )
		_sizePage.cx = 0;
	else if( dwScrollTypeH == __ESIW_ST_PIXEL )
		_sizePage.cx = _sizeClient.cx;
	else if( dwScrollTypeH == __ESIW_ST_ITEM )
	{
	} // else if( dwScrollTypeH == __ESIW_ST_ITEM )
	else
	{ // if __ESIW_ST_VIRTUAL
		ASSERT( dwScrollTypeH == __ESIW_ST_VIRTUAL );
	} // if __ESIW_ST_VIRTUAL
	
	if( dwScrollTypeV == __ESIW_ST_NONE )
		_sizePage.cy = 0;
	else if( dwScrollTypeV == __ESIW_ST_PIXEL )
		_sizePage.cy = _sizeClient.cy;
	else if( dwScrollTypeV == __ESIW_ST_ITEM )
	{
	} // else if( dwScrollTypeV == __ESIW_ST_ITEM )
	else
	{ // if __ESIW_ST_VIRTUAL
		ASSERT( dwScrollTypeV == __ESIW_ST_VIRTUAL );
	} // if __ESIW_ST_VIRTUAL

	return _sizePage;
}

CSize CExtScrollItemWnd::OnSiwCalcItemSize() const
{
	ASSERT_VALID( this );
	return CSize( 0, 0 );
}

CSize CExtScrollItemWnd::OnSiwCalcPageMetrics( int nDirection ) const
{
	ASSERT_VALID( this );
	nDirection;
CSize _sizeItem = OnSiwCalcItemSize();
	ASSERT( _sizeItem.cx >= 0 && _sizeItem.cy >= 0 );
CRect rcClient = OnSwGetClientRect();
CSize _sizeClient = rcClient.Size();
	if( _sizeClient.cx < 0 )
		_sizeClient.cx = 0;
	if( _sizeClient.cy < 0 )
		_sizeClient.cy = 0;
CSize _sizePageMetrics(
		( _sizeItem.cx == 0 ) ? 0 : ( _sizeClient.cx / _sizeItem.cx ),
		( _sizeItem.cy == 0 ) ? 0 : ( _sizeClient.cy / _sizeItem.cy )
		);
	if( _sizeItem.cx != 0 )
	{
		if( _sizePageMetrics.cx == 0 )
			_sizePageMetrics.cx = 1;
	}
	else
		_sizePageMetrics.cx = 0;
	if( _sizeItem.cy != 0 )
	{
		if( _sizePageMetrics.cy == 0 )
			_sizePageMetrics.cy = 1;
	}
	else
		_sizePageMetrics.cy = 0;
	return _sizePageMetrics;
}

bool CExtScrollItemWnd::OnSiwQueryVirtualBOF( bool bHorz ) const
{
	ASSERT_VALID( this );
	if( bHorz )
	{
		DWORD dwScrollTypeH = SiwScrollTypeHGet();
		if( dwScrollTypeH != __ESIW_ST_VIRTUAL )
			return true;
	} // if( bHorz )
	else
	{
		DWORD dwScrollTypeV = SiwScrollTypeVGet();
		if( dwScrollTypeV != __ESIW_ST_VIRTUAL )
			return true;
	} // else from if( bHorz )
	return false;
}

bool CExtScrollItemWnd::OnSiwQueryVirtualEOF( bool bHorz ) const
{
	ASSERT_VALID( this );
	if( bHorz )
	{
		DWORD dwScrollTypeH = SiwScrollTypeHGet();
		if( dwScrollTypeH != __ESIW_ST_VIRTUAL )
			return true;
	} // if( bHorz )
	else
	{
		DWORD dwScrollTypeV = SiwScrollTypeVGet();
		if( dwScrollTypeV != __ESIW_ST_VIRTUAL )
			return true;
	} // else from if( bHorz )
	return false;
}

INT CExtScrollItemWnd::OnSiwQueryItemExtentH(
	LONG nColNo,
	INT * p_nExtraSpaceBefore, // = NULL
	INT * p_nExtraSpaceAfter // = NULL
	) const
{
	ASSERT_VALID( this );
	ASSERT( nColNo >= 0 );
	nColNo;
	if( p_nExtraSpaceBefore != NULL )
		(*p_nExtraSpaceBefore) = 0;
	if( p_nExtraSpaceAfter != NULL )
		(*p_nExtraSpaceAfter) = 0;
	return 0;
}

INT CExtScrollItemWnd::OnSiwQueryItemExtentV(
	LONG nRowNo,
	INT * p_nExtraSpaceBefore, // = NULL
	INT * p_nExtraSpaceAfter // = NULL
	) const
{
	ASSERT_VALID( this );
	ASSERT( nRowNo >= 0 );
	nRowNo;
	if( p_nExtraSpaceBefore != NULL )
		(*p_nExtraSpaceBefore) = 0;
	if( p_nExtraSpaceAfter != NULL )
		(*p_nExtraSpaceAfter) = 0;
	return 0;
}

bool CExtScrollItemWnd::OnSiwVirtualEndTestH( LONG nColNo, LPARAM lParam ) const
{
	ASSERT_VALID( this );
	ASSERT( nColNo >= 0 );
	nColNo;
	lParam;
	return false;
}

bool CExtScrollItemWnd::OnSiwVirtualEndTestV( LONG nRowNo, LPARAM lParam ) const
{
	ASSERT_VALID( this );
	ASSERT( nRowNo >= 0 );
	nRowNo;
	lParam;
	return false;
}

bool CExtScrollItemWnd::OnSiwWalkCell(
	CDC & dc,
	LPVOID pQueryData,
	LONG nVisibleColNo,
	LONG nVisibleRowNo,
	LONG nColNo,
	LONG nRowNo,
	const RECT & rcCellExtra,
	const RECT & rcCell,
	const RECT & rcVisibleRange,
	bool & bVirtualRightReached,
	bool & bVirtualBottomReached,
	DWORD dwAreaFlags,
	bool bFocusedControl
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL || pQueryData != NULL );
	//ASSERT( nVisibleColNo >= 0 );
	//ASSERT( nVisibleRowNo >= 0 );
	ASSERT( nColNo >= 0 );
	ASSERT( nRowNo >= 0 );
//	ASSERT( rcCell.right >= rcCell.left );
//	ASSERT( rcCell.bottom >= rcCell.top );
	ASSERT( rcCellExtra.right >= rcCellExtra.left );
	ASSERT( rcCellExtra.bottom >= rcCellExtra.top );
	ASSERT( rcCellExtra.left <= rcCell.left );
	ASSERT( rcCellExtra.top <= rcCell.top );
	ASSERT( rcCellExtra.right >= rcCell.right );
	ASSERT( rcCellExtra.bottom >= rcCell.bottom );
	dc;
	pQueryData;
	nVisibleColNo;
	nVisibleRowNo;
	nColNo;
	nRowNo;
	rcCellExtra;
	rcCell;
	rcVisibleRange;
	bVirtualRightReached;
	bVirtualBottomReached;
	dwAreaFlags;
	bFocusedControl;
	return false;
}

bool CExtScrollItemWnd::OnSiwWalkItemsH(
	CDC & dc,
	LPVOID pQueryData,
	const RECT & rcRowExtra,
	const RECT & rcRow,
	LONG nVisibleRowNo,
	LONG nRowNo,
	const RECT & rcVisibleRange,
	bool & bVirtualBottomReached,
	DWORD dwAreaFlags,
	bool bFocusedControl
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL || pQueryData != NULL );
	//ASSERT( nVisibleRowNo >= 0 );
	ASSERT( nRowNo >= 0 );
	ASSERT( rcRow.right >= rcRow.left );
	ASSERT( rcRow.bottom >= rcRow.top );
	ASSERT( rcRowExtra.right >= rcRowExtra.left );
	ASSERT( rcRowExtra.bottom >= rcRowExtra.top );
	ASSERT( rcRowExtra.left <= rcRow.left );
	ASSERT( rcRowExtra.top <= rcRow.top );
	ASSERT( rcRowExtra.right >= rcRow.right );
	ASSERT( rcRowExtra.bottom >= rcRow.bottom );
	bVirtualBottomReached = OnSiwVirtualEndTestV( nRowNo, LPARAM(dwAreaFlags) );
	if( bVirtualBottomReached )
		return false;
	if(		rcRowExtra.right == rcRowExtra.left
		||	rcRowExtra.bottom == rcRowExtra.top
		)
		return false;
	if( dc.GetSafeHdc() == NULL )
		return false; // no hit-test like query support
	if( ! dc.RectVisible( &rcRow ) )
		return false;
CRect rcClient = OnSwGetClientRect();
LONG nVisibleColNo = 0;
LONG nColNo = rcVisibleRange.left;
bool bVirtualRightReached = false;
CRect rcCellExtra(
		rcClient.left,
		rcRowExtra.top,
		rcClient.left,
		rcRowExtra.bottom
		);
	for( ; nColNo <= rcVisibleRange.right; nColNo++, nVisibleColNo++ )
	{
		INT nExtraSpaceBefore = 0, nExtraSpaceAfter = 0;
		INT nColWidth =
			OnSiwQueryItemExtentH(
				nColNo,
				&nExtraSpaceBefore,
				&nExtraSpaceAfter
				);
		ASSERT( nColWidth >= 0 );
		ASSERT( nExtraSpaceBefore >= 0 );
		ASSERT( nExtraSpaceAfter >= 0 );
		rcCellExtra.right += nColWidth;
		if(		rcCellExtra.right < rcClient.left
			||	rcCellExtra.left > rcClient.right
			)
			continue;
		CRect rcCell(
			rcCellExtra.left + nExtraSpaceBefore,
			rcRow.top,
			rcCellExtra.right - nExtraSpaceAfter,
			rcRow.bottom
			);
		OnSiwWalkCell(
			dc,
			pQueryData,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			rcCellExtra,
			rcCell,
			rcVisibleRange,
			bVirtualRightReached,
			bVirtualBottomReached,
			dwAreaFlags,
			bFocusedControl
			);
		if( bVirtualRightReached || bVirtualBottomReached )
			break;
		rcCellExtra.left = rcCellExtra.right;
	} // for( ; nColNo <= rcVisibleRange.right; nColNo++, nVisibleColNo++ )
	return false;
}

bool CExtScrollItemWnd::OnSiwWalkItemsV(
	CDC & dc,
	LPVOID pQueryData,
	const RECT & rcColExtra,
	const RECT & rcCol,
	LONG nVisibleColNo,
	LONG nColNo,
	const RECT & rcVisibleRange,
	bool & bVirtualRightReached,
	DWORD dwAreaFlags,
	bool bFocusedControl
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL || pQueryData != NULL );
	//ASSERT( nVisibleColNo >= 0 );
	ASSERT( nColNo >= 0 );
	ASSERT( rcCol.right >= rcCol.left );
	ASSERT( rcCol.bottom >= rcCol.top );
	ASSERT( rcColExtra.right >= rcColExtra.left );
	ASSERT( rcColExtra.bottom >= rcColExtra.top );
	ASSERT( rcColExtra.left <= rcCol.left );
	ASSERT( rcColExtra.top <= rcCol.top );
	ASSERT( rcColExtra.right >= rcCol.right );
	ASSERT( rcColExtra.bottom >= rcCol.bottom );
	bVirtualRightReached = OnSiwVirtualEndTestH( nColNo, LPARAM(dwAreaFlags) );
	if( bVirtualRightReached )
		return false;
	if(		rcColExtra.right == rcColExtra.left
		||	rcColExtra.bottom == rcColExtra.top
		)
		return false;
	if( dc.GetSafeHdc() == NULL )
		return false; // no hit-test like query support
	if( ! dc.RectVisible( &rcCol ) )
		return false;
CRect rcClient = OnSwGetClientRect();
LONG nVisibleRowNo = 0;
LONG nRowNo = rcVisibleRange.top;
bool bVirtualBottomReached = false;
CRect rcCellExtra(
		rcColExtra.left,
		rcClient.top,
		rcColExtra.right,
		rcClient.top
		);
	for( ; nRowNo <= rcVisibleRange.bottom; nRowNo++, nVisibleRowNo++ )
	{
		INT nExtraSpaceBefore = 0, nExtraSpaceAfter = 0;
		INT nRowHeight =
			OnSiwQueryItemExtentV(
				nRowNo,
				&nExtraSpaceBefore,
				&nExtraSpaceAfter
				);
		ASSERT( nRowHeight >= 0 );
		ASSERT( nExtraSpaceBefore >= 0 );
		ASSERT( nExtraSpaceAfter >= 0 );
		rcCellExtra.bottom += nRowHeight;
		if(		rcCellExtra.right < rcClient.left
			||	rcCellExtra.left > rcClient.right
			)
			continue;
		CRect rcCell(
			rcCol.left,
			rcCellExtra.top + nExtraSpaceBefore,
			rcCol.right,
			rcCellExtra.bottom - nExtraSpaceAfter
			);
		OnSiwWalkCell(
			dc,
			pQueryData,
			nVisibleColNo,
			nVisibleRowNo,
			nColNo,
			nRowNo,
			rcCellExtra,
			rcCell,
			rcVisibleRange,
			bVirtualRightReached,
			bVirtualBottomReached,
			dwAreaFlags,
			bFocusedControl
			);
		if( bVirtualRightReached || bVirtualBottomReached )
			break;
		rcCellExtra.top = rcCellExtra.bottom;
	} // for( ; nRowNo <= rcVisibleRange.bottom; nRowNo++, nVisibleRowNo++ )
	return false;
}

bool CExtScrollItemWnd::OnSiwQueryFocusedControlState() const
{
	ASSERT_VALID( this );
	if( m_hWnd == NULL || (! ::IsWindow(m_hWnd) ) )
		return false;
HWND hWndFocus = ::GetFocus();
	if(		m_hWnd == hWndFocus
		||	::IsChild( m_hWnd, hWndFocus )
		)
		return true;
	return false;
}

void CExtScrollItemWnd::OnSiwDrawFocusRect(
	CDC &dc,
	LPCRECT pRect,
	CObject * pObjSrc, // = NULL
	LPARAM lParam // = 0L
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
#ifdef _DEBUG
	if( pObjSrc != NULL )
	{
		ASSERT_VALID( pObjSrc );
	}
#endif // _DEBUG
	pObjSrc;
	lParam;
COLORREF clrTextOld = 
		dc.SetTextColor( RGB(255,255,255) );
COLORREF clrBkOld =
		dc.SetBkColor( RGB(0,0,0) );
	dc.DrawFocusRect( pRect );
	dc.SetBkColor( clrBkOld );
	dc.SetTextColor( clrTextOld );
}

COLORREF CExtScrollItemWnd::OnSiwGetReadOnlyTextColor() const
{
	ASSERT_VALID( this );
//	return OnSiwGetSysColor( COLOR_BTNTEXT );
//	return OnSiwGetSysColor( COLOR_3DSHADOW );
	return OnSiwGetSysColor( COLOR_3DSHADOW );
}

COLORREF CExtScrollItemWnd::OnSiwGetReadOnlyBackgroundColor() const
{
	ASSERT_VALID( this );
//	return OnSiwGetSysColor( COLOR_WINDOW );
//	return OnSiwGetSysColor( COLOR_3DFACE );
//	return OnSiwGetSysColor( COLOR_WINDOW );
	return COLORREF(-1L);
}

COLORREF CExtScrollItemWnd::OnSiwGetSysColor( int nIndex ) const
{
	ASSERT_VALID( this );
DWORD dwSiwGetStyleEx = SiwGetStyleEx();
bool bUsePmColors = ( ( dwSiwGetStyleEx & __EGWS_EX_PM_COLORS ) != 0 ) ? true : false;
COLORREF clr = bUsePmColors ? PmBridge_GetPM()->GetColor( nIndex, (CObject*)this ) : ( ::GetSysColor( nIndex ) );
	return clr;
}

// nLighterOrDarker is related to COLOR_3DFACE brush:
// -3 - darkest, -2 - darker, -1 - dark,
//  1 - light,    2 - lighter, 3 - lightest
CBrush & CExtScrollItemWnd::OnSiwGetLighterOrDarkerBrush( int nLighterOrDarker ) const
{
	ASSERT_VALID( this );
DWORD dwSiwGetStyleEx = SiwGetStyleEx();
bool bUsePmColors = ( ( dwSiwGetStyleEx & __EGWS_EX_PM_COLORS ) != 0 ) ? true : false;
CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );
	switch( nLighterOrDarker )
	{
	case -3:	return bUsePmColors ? pPM->m_brushDarkestDefault  : pPM->m_brushDarkestSystem;
	case -2:	return bUsePmColors ? pPM->m_brushDarkerDefault   : pPM->m_brushDarkerSystem;
	case -1:	return bUsePmColors ? pPM->m_brushDarkDefault     : pPM->m_brushDarkSystem;
	case 1:		return bUsePmColors ? pPM->m_brushLightDefault    : pPM->m_brushLightSystem;
	case 2:		return bUsePmColors ? pPM->m_brushLighterDefault  : pPM->m_brushLighterSystem;
	default:	ASSERT( nLighterOrDarker == 3 );
				return bUsePmColors ? pPM->m_brushLightestDefault : pPM->m_brushLightestSystem;
	} // switch( nLighterOrDarker )
}

CFont & CExtScrollItemWnd::OnSiwGetDefaultFont() const
{
	ASSERT_VALID( this );
CFont * pFont = NULL;
	if( GetSafeHwnd() != NULL )
		pFont = GetFont();
	return ( pFont->GetSafeHandle() != NULL ) ? (*pFont) : PmBridge_GetPM()->m_FontNormal;
}

void CExtScrollItemWnd::OnSwPaint( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
bool bFocusedControl = OnSiwQueryFocusedControlState();
	OnSiwPaintBackground( dc, bFocusedControl );
	OnSiwPaintForeground( dc, bFocusedControl );
}

void CExtScrollItemWnd::OnSiwPaintBackground(
	CDC & dc,
	bool bFocusedControl
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	bFocusedControl;
CRect rcRealClient;
	CWnd::GetClientRect( &rcRealClient );
	if( ! dc.RectVisible(&rcRealClient) )
		return;
CRect rcClient = OnSwGetClientRect();
	if( rcClient != rcRealClient )
	{
		COLORREF clrEntire = OnSiwGetSysColor( COLOR_3DFACE );
		dc.FillSolidRect( &rcRealClient, clrEntire );
	} // if( rcClient != rcRealClient )
	if( dc.RectVisible(&rcClient) )
	{
		COLORREF clrWindow = 
			OnSiwGetSysColor( IsWindowEnabled() ? COLOR_WINDOW : COLOR_3DFACE );
		dc.FillSolidRect( &rcClient, clrWindow );
	} // if( dc.RectVisible(&rcClient) )
}

void CExtScrollItemWnd::OnSiwPaintForeground(
	CDC & dc,
	bool bFocusedControl
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CRect rcClient = OnSwGetClientRect();
	if( ! dc.RectVisible(&rcClient) )
		return;
CRect rcVisibleRange = OnSiwGetVisibleRange();
	ASSERT( rcVisibleRange.left <= rcVisibleRange.right );
	ASSERT( rcVisibleRange.top <= rcVisibleRange.bottom );
CSize _sizeItem = OnSiwCalcItemSize();
	ASSERT( _sizeItem.cx >= 0 && _sizeItem.cy >= 0 );
	if(	!(		rcVisibleRange.left <= rcVisibleRange.right
			&&	rcVisibleRange.top <= rcVisibleRange.bottom
			&&	_sizeItem.cx > 0
			&&	_sizeItem.cy > 0
			)
		)
		return;
DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if(		(	dwScrollTypeH == __ESIW_ST_NONE
			||	dwScrollTypeH == __ESIW_ST_PIXEL
			)
		&&	(	dwScrollTypeV == __ESIW_ST_NONE
			||	dwScrollTypeV == __ESIW_ST_PIXEL
			)
		)
		return;
bool bPreferHorizontalWalk = SiwPreferHorizontalWalkGet();
	if( bPreferHorizontalWalk )
	{
		if(		dwScrollTypeH == __ESIW_ST_NONE
			||	dwScrollTypeH == __ESIW_ST_PIXEL
			)
			bPreferHorizontalWalk = false;
	} // if( bPreferHorizontalWalk )
	else
	{
		if(		dwScrollTypeV == __ESIW_ST_NONE
			||	dwScrollTypeV == __ESIW_ST_PIXEL
			)
			bPreferHorizontalWalk = true;
	} // else from if( bPreferHorizontalWalk )
CRgn rgnClientClip;
	if( rgnClientClip.CreateRectRgnIndirect(&rcClient) )
		dc.SelectClipRgn( &rgnClientClip );
COLORREF clrText = OnSiwGetSysColor( COLOR_BTNTEXT );
int nOldBkMode = dc.SetBkMode( TRANSPARENT );
COLORREF clrTextOld = dc.SetTextColor( clrText );
CFont * pOldFont = dc.SelectObject( &( OnSiwGetDefaultFont() ) );
	if( bPreferHorizontalWalk )
	{
		LONG nColNo = rcVisibleRange.left;
		LONG nVisibleColNo = 0;
		bool bVirtualRightReached = false;
		CRect rcColExtra(
			rcClient.left,
			rcClient.top,
			rcClient.left,
			rcClient.bottom
			);
		for( ; nColNo <= rcVisibleRange.right; nColNo ++, nVisibleColNo++ )
		{
			INT nExtraSpaceBefore = 0, nExtraSpaceAfter = 0;
			INT nColWidth =
				OnSiwQueryItemExtentH(
					nColNo,
					&nExtraSpaceBefore,
					&nExtraSpaceAfter
					);
			ASSERT( nColWidth >= 0 );
			ASSERT( nExtraSpaceBefore >= 0 );
			ASSERT( nExtraSpaceAfter >= 0 );
			if( nColWidth == 0 )
				continue;
			rcColExtra.right += nColWidth;
			CRect rcCol( rcColExtra );
			rcCol.DeflateRect(
				nExtraSpaceBefore,
				0,
				nExtraSpaceAfter,
				0
				);
			ASSERT( rcCol.left <= rcCol.right );
			OnSiwWalkItemsV(
				dc,
				NULL,
				rcColExtra,
				rcCol,
				nVisibleColNo,
				nColNo,
				rcVisibleRange,
				bVirtualRightReached,
				0L,
				bFocusedControl
				);
			if( bVirtualRightReached )
				break;
			rcColExtra.left = rcColExtra.right;
		}
	} // if( bPreferHorizontalWalk )
	else
	{
		LONG nRowNo = rcVisibleRange.top;
		LONG nVisibleRowNo = 0;
		bool bVirtualBottomReached = false;
		CRect rcRowExtra(
			rcClient.left,
			rcClient.top,
			rcClient.right,
			rcClient.top
			);
		for( ; nRowNo <= rcVisibleRange.bottom; nRowNo ++, nVisibleRowNo++ )
		{
			INT nExtraSpaceBefore = 0, nExtraSpaceAfter = 0;
			INT nRowHeight =
				OnSiwQueryItemExtentV(
					nRowNo,
					&nExtraSpaceBefore,
					&nExtraSpaceAfter
					);
			ASSERT( nRowHeight >= 0 );
			ASSERT( nExtraSpaceBefore >= 0 );
			ASSERT( nExtraSpaceAfter >= 0 );
			if( nRowHeight == 0 )
				continue;
			rcRowExtra.bottom += nRowHeight;
			CRect rcRow( rcRowExtra );
			rcRow.DeflateRect(
				0,
				nExtraSpaceBefore,
				0,
				nExtraSpaceAfter
				);
			ASSERT( rcRow.top <= rcRow.bottom );
			OnSiwWalkItemsH(
				dc,
				NULL,
				rcRowExtra,
				rcRow,
				nVisibleRowNo,
				nRowNo,
				rcVisibleRange,
				bVirtualBottomReached,
				0L,
				bFocusedControl
				);
			if( bVirtualBottomReached )
				break;
			rcRowExtra.top = rcRowExtra.bottom;
		}
	} // else from if( bPreferHorizontalWalk )
	dc.SelectObject( pOldFont );
	dc.SetTextColor( clrTextOld );
	dc.SetBkMode( nOldBkMode );
	dc.SelectClipRgn( NULL );
}

bool CExtScrollItemWnd::OnSwDoScroll(
	UINT nScrollCode,
	UINT nPos,
	bool bDoScroll // = true
	)
{
	ASSERT_VALID( this );
DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
LONG xPos, xOrgValue, yPos, yOrgValue;
	xOrgValue = xPos = ScrollPos32Get( SB_HORZ );
	switch( LOBYTE(nScrollCode) )
	{
	case SB_ENDSCROLL: break;
	case SB_TOP:		xPos  = 0;							break;
	case SB_BOTTOM:		xPos  = __EXT_SCROLL_NUMERIC_MAX;	break;
	case SB_LINEUP:		xPos -= OnSwGetLineSize( -1 ).cx;	break;
	case SB_LINEDOWN:	xPos += OnSwGetLineSize(  1 ).cx;	break;
	case SB_PAGEUP:		xPos -= OnSwGetPageSize( -1 ).cx;	break;
	case SB_PAGEDOWN:
		{
			LONG nPageSize = OnSwGetPageSize( 1 ).cx;
			if( dwScrollTypeH != __ESIW_ST_VIRTUAL )
			{
				CExtScrollBar * pExtSB = DYNAMIC_DOWNCAST( CExtScrollBar, GetScrollBarCtrl(SB_HORZ) );
				if(		pExtSB->GetSafeHwnd() != NULL
					&&	::GetCapture() == pExtSB->m_hWnd
					)
					xPos = ScrollPos32Get( SB_HORZ, true );
				xPos += nPageSize;
			}
			else
			{
				if( (__EXT_SCROLL_NUMERIC_MAX - nPageSize) < xPos )
					xPos = __EXT_SCROLL_NUMERIC_MAX;
				else
					xPos += nPageSize;
			} // else from if( dwScrollTypeH != __ESIW_ST_VIRTUAL )
		}
		break;
	case SB_THUMBTRACK:
		if( ! OnSwQueryThumbTrackEnabled(true) )
			return false;
	case SB_THUMBPOSITION:
		if( dwScrollTypeH != __ESIW_ST_VIRTUAL )
		{
			if( m_bUse32BitScrollInfo )
				nPos = ScrollPos32Get( SB_HORZ, true );
			xPos = nPos;
		} // if( dwScrollTypeH != __ESIW_ST_VIRTUAL )
		else
		{
			if( nPos <= 0 )
				xPos = 0;
			else
				xPos = __EXT_SCROLL_NUMERIC_MAX;
		} // else from if( dwScrollTypeH != __ESIW_ST_VIRTUAL )
		break;
	}
	yOrgValue = yPos = ScrollPos32Get( SB_VERT );
	switch( HIBYTE(nScrollCode) )
	{
	case SB_ENDSCROLL: break;
	case SB_TOP:		yPos  = 0;							break;
	case SB_BOTTOM:		yPos  = __EXT_SCROLL_NUMERIC_MAX;	break;
	case SB_LINEUP:		yPos -= OnSwGetLineSize( -1 ).cy;	break;
	case SB_LINEDOWN:	yPos += OnSwGetLineSize(  1 ).cy;	break;
	case SB_PAGEUP:		yPos -= OnSwGetPageSize( -1 ).cy;	break;
	case SB_PAGEDOWN:
		{
			LONG nPageSize = OnSwGetPageSize( 1 ).cy;
			if( dwScrollTypeV != __ESIW_ST_VIRTUAL )
			{
				CExtScrollBar * pExtSB = DYNAMIC_DOWNCAST( CExtScrollBar, GetScrollBarCtrl(SB_VERT) );
				if(		pExtSB->GetSafeHwnd() != NULL
					&&	::GetCapture() == pExtSB->m_hWnd
					)
					yPos = ScrollPos32Get( SB_VERT, true );
				yPos += nPageSize;
			}
			else
			{
				if( (__EXT_SCROLL_NUMERIC_MAX - nPageSize) < yPos )
					yPos = __EXT_SCROLL_NUMERIC_MAX;
				else
					yPos += nPageSize;
			} // else from if( dwScrollTypeV != __ESIW_ST_VIRTUAL )
		}
		break;
	case SB_THUMBTRACK:
		if( ! OnSwQueryThumbTrackEnabled(false) )
			return false;
	case SB_THUMBPOSITION:
		if( dwScrollTypeV != __ESIW_ST_VIRTUAL )
		{
			if( m_bUse32BitScrollInfo )
				nPos = ScrollPos32Get( SB_VERT, true );
			yPos = (int)nPos;
		} // if( dwScrollTypeV != __ESIW_ST_VIRTUAL )
		else
		{
			if( nPos <= 0 )
				yPos = 0;
			else
				yPos = __EXT_SCROLL_NUMERIC_MAX;
		} // else from if( dwScrollTypeV != __ESIW_ST_VIRTUAL )
		break;
	}
bool bResult = OnSwDoScrollBy( CSize( xPos - xOrgValue, yPos - yOrgValue ), bDoScroll );
	if( bResult && bDoScroll && m_bScrollUpdateWindow )
		OnSwUpdateWindow();
	return bResult;
}

bool CExtScrollItemWnd::SiwFireCacheChanging(
	LONG nScrollPosNewX, // = -1L // if <0L - use current
	LONG nScrollPosNewY, // = -1L // if <0L - use current
	bool bUpdate // = true
	)
{
	ASSERT_VALID( this );
LONG	xMaxValue = 0L, xPos = 0L, xPosOld = 0L,
		yMaxValue = 0L, yPos = 0L, yPosOld = 0L;
	if( bUpdate || OnSwHasScrollBar(true) )
	{
		xPos = ScrollPos32Get( SB_HORZ );
		xMaxValue = ScrollLimit32Get( SB_HORZ );
		if( nScrollPosNewX >= 0L )
		{
			xPosOld = xPos;
			xPos = nScrollPosNewX;
		} // if( nScrollPosNewX >= 0L )
		if( xPos < 0 )
			xPos = 0;
		else if( xPos > xMaxValue )
			xPos = xMaxValue;
		if( bUpdate && xPos != xPosOld  )
			ScrollPos32Set( SB_HORZ, xPos, false );
	} // if( bUpdate || OnSwHasScrollBar(true) )
	if( bUpdate || OnSwHasScrollBar(false) )
	{
		yPos = ScrollPos32Get( SB_VERT );
		yMaxValue = ScrollLimit32Get( SB_VERT );
		if( nScrollPosNewY >= 0L )
		{
			yPosOld = yPos;
			yPos = nScrollPosNewY;
		} // if( nScrollPosNewY >= 0L )
		if( yPos < 0 )
			yPos = 0;
		else if( yPos > yMaxValue )
			yPos = yMaxValue;
		if( bUpdate && yPos != yPosOld  )
			ScrollPos32Set( SB_VERT, yPos, false );
	} // if( bUpdate || OnSwHasScrollBar(false) )
bool	bBeginX = ( (xPos == 0) ? true : false ),
		bBeginY = ( (yPos == 0) ? true : false ),
		bEndX = ( (xPos == xMaxValue && xMaxValue > 0) ? true : false ),
		bEndY = ( (yPos == yMaxValue && yMaxValue > 0) ? true : false );
CSize _sizePage = OnSwGetPageSize( 0 );
	if( _sizePage.cx < 0 )
		_sizePage.cx = 0;
	if( _sizePage.cy < 0 )
		_sizePage.cy = 0;
	m_sciLast.Empty();
bool bRetVal =
		_NotifyCacheChanged(
			xPos, xPosOld, xMaxValue, _sizePage.cx,
			yPos, yPosOld, yMaxValue, _sizePage.cy,
			bBeginX, bEndX,
			bBeginY, bEndY
			);
	if( bUpdate )
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	} // if( bUpdate )
	return bRetVal;
}

bool CExtScrollItemWnd::OnSwDoScrollBy(
	CSize sizeScroll,
	bool bDoScroll // = true
	)
{
	ASSERT_VALID( this );
LONG xMaxValue, xOrgValue, xPos, yMaxValue, yOrgValue, yPos;
	if( ! OnSwHasScrollBar(false) )
		sizeScroll.cy = 0;
	if( ! OnSwHasScrollBar(true) )
		sizeScroll.cx = 0;

	xOrgValue = xPos = ScrollPos32Get( SB_HORZ );
	xMaxValue = ScrollLimit32Get( SB_HORZ );
	xPos += sizeScroll.cx;
	if( xPos < 0 )
		xPos = 0;
	else if( xPos > xMaxValue )
		xPos = xMaxValue;

	yOrgValue = yPos = ScrollPos32Get( SB_VERT );
	yMaxValue = ScrollLimit32Get( SB_VERT );
	yPos += sizeScroll.cy;
	if( yPos < 0 )
		yPos = 0;
	else if( yPos > yMaxValue )
		yPos = yMaxValue;
	
bool	bBeginX = ( (xPos == 0) ? true : false ),
		bBeginY = ( (yPos == 0) ? true : false ),
		bEndX = ( (xPos == xMaxValue && xMaxValue > 0) ? true : false ),
		bEndY = ( (yPos == yMaxValue && yMaxValue > 0) ? true : false );
CSize _sizePage = OnSwGetPageSize( 0 );
bool bRetVal =
		_NotifyCacheChanged(
			xPos, xOrgValue, xMaxValue, _sizePage.cx,
			yPos, yOrgValue, yMaxValue, _sizePage.cy,
			bBeginX, bEndX,
			bBeginY, bEndY
			);
	if( !bRetVal )
		return false;

	if(		xPos == xOrgValue
		&&	yPos == yOrgValue
		)
		return true; //false;

	if( bDoScroll )
	{
		CSize sizeItem = OnSiwCalcItemSize();
		int xAmount = xOrgValue - xPos;
		int yAmount = yOrgValue - yPos;
		xAmount *= sizeItem.cx;
		yAmount *= sizeItem.cy;
		OnSwDoScrollWindow( xAmount, yAmount );
		if( xPos != xOrgValue )
			ScrollPos32Set( SB_HORZ, xPos );
		if( yPos != yOrgValue )
			ScrollPos32Set( SB_VERT, yPos );
	} // if( bDoScroll )

	return true;
}

CPoint CExtScrollItemWnd::OnSwGetScrollPaintPos() const
{
	ASSERT_VALID( this );
	return CPoint( 0, 0 );
}

CRect CExtScrollItemWnd::OnSiwGetFrozenRange() const
{
	ASSERT_VALID( this );
	return CRect( 0, 0, 0, 0 );
}

CRect CExtScrollItemWnd::OnSiwGetVisibleRange() const
{
	ASSERT_VALID( this );
CRect rcFrozenRange = OnSiwGetFrozenRange();
	ASSERT(
			rcFrozenRange.top    >= 0
		&&	rcFrozenRange.bottom >= 0
		&&	rcFrozenRange.left   >= 0
		&&	rcFrozenRange.right  >= 0
		);
DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
CSize _sizeTotal = OnSwGetTotalSize();
CSize _sizePage = OnSiwCalcPageMetrics( 0 );
CSize _sizeRange(
		(dwScrollTypeH == __ESIW_ST_NONE) ? _sizeTotal.cx : ( min( _sizeTotal.cx, _sizePage.cx ) ),
		(dwScrollTypeV == __ESIW_ST_NONE) ? _sizeTotal.cy : ( min( _sizeTotal.cy, _sizePage.cy ) )
		);
CPoint ptMove = OnSwGetScrollPos();
CRect rcVisibleRange( ptMove, _sizeRange );
	if( ((ULONG)rcVisibleRange.right) >= ((ULONG)_sizeTotal.cx)
		&&	dwScrollTypeH != __ESIW_ST_NONE
		&&	dwScrollTypeH != __ESIW_ST_VIRTUAL
		)
		rcVisibleRange.right = _sizeTotal.cx-1;
	if( ((ULONG)rcVisibleRange.right) < ((ULONG)rcVisibleRange.left) )
		rcVisibleRange.right = rcVisibleRange.left;
	if( ((ULONG)rcVisibleRange.bottom) >= ((ULONG)_sizeTotal.cy)
		&&	dwScrollTypeV != __ESIW_ST_NONE
		&&	dwScrollTypeV != __ESIW_ST_VIRTUAL
		)
		rcVisibleRange.bottom = _sizeTotal.cy-1;
	if( ((ULONG)rcVisibleRange.bottom) < ((ULONG)rcVisibleRange.top) )
		rcVisibleRange.bottom = rcVisibleRange.top;
	if( dwScrollTypeH == __ESIW_ST_VIRTUAL )
		rcVisibleRange.OffsetRect( -rcVisibleRange.left, 0 );
	if( dwScrollTypeV == __ESIW_ST_VIRTUAL )
		rcVisibleRange.OffsetRect( 0, -rcVisibleRange.top );
	rcVisibleRange.OffsetRect( rcFrozenRange.left, rcFrozenRange.top );
	return rcVisibleRange;
}

void CExtScrollItemWnd::OnSwGetScrollBarState(
	CSize sizeClient,
	CSize & sizeNeedSB,
	CSize & sizeRange,
	CPoint & ptMove,
	bool bInsideClient
	) const
{
	ASSERT_VALID( this );

DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();

bool bHasSbH0 = OnSwHasScrollBar( true );
bool bHasSbV0 = OnSwHasScrollBar( false );

CSize sizeSb = OnSwGetScrollBarSizes();
CSize sizeTotal = OnSwGetTotalSize();
	sizeRange = sizeTotal - sizeClient;
	ptMove = OnSwGetScrollPos();
bool bNeedH = sizeRange.cx > 0;
	if( !bNeedH )
		ptMove.x = 0;
	else if(
				bInsideClient
			&&	dwScrollTypeV == __ESIW_ST_PIXEL
			&&	( bHasSbH0 )
			)
		sizeRange.cy += sizeSb.cy;
bool bNeedV = sizeRange.cy > 0;
	if( ! bNeedV )
		ptMove.y = 0;
	else if (
				bInsideClient
			&&	dwScrollTypeH == __ESIW_ST_PIXEL
			&&	( bHasSbV0 )
			)
			sizeRange.cx += sizeSb.cx;
	if(		bNeedV
		&&	(!bNeedH)
		&&	sizeRange.cx > 0
		)
	{
		ASSERT( bInsideClient );
		bNeedH = true;
		if( dwScrollTypeV == __ESIW_ST_PIXEL )
		{
			sizeRange.cy += sizeSb.cy;
		}
	} // if( bNeedV ...
	if( sizeRange.cx > 0 && ptMove.x >= sizeRange.cx )
		ptMove.x = sizeRange.cx;
	if( sizeRange.cy > 0 && ptMove.y >= sizeRange.cy )
		ptMove.y = sizeRange.cy;
	if( bNeedH || bNeedV )
	{
		CSize sizePage = OnSwGetPageSize( 0 );
		CSize sizePxClient( 0, 0 );
		if( dwScrollTypeH == __ESIW_ST_PIXEL || dwScrollTypeV == __ESIW_ST_PIXEL )
		{
			sizePxClient = OnSwGetClientRect().Size();
		}
		if( bNeedH )
		{
			if( dwScrollTypeH == __ESIW_ST_PIXEL )
			{
				if( OnSwHasScrollBar( true ) )
					sizePxClient.cx += sizeSb.cx;
				sizePage.cx = max( sizePage.cx, sizePxClient.cx );
			}
			if( sizeTotal.cx < sizePage.cx )
			{
				bNeedH = false;
				ptMove.x = 0;
			}
		}
		if( bNeedV )
		{
			if( dwScrollTypeV == __ESIW_ST_PIXEL )
			{
				if( OnSwHasScrollBar( false ) )
					sizePxClient.cy += sizeSb.cy;
				sizePage.cy = max( sizePage.cy, sizePxClient.cy );
			}
			if( sizeTotal.cy < sizePage.cy )
			{
				bNeedV = false;
				ptMove.y = 0;
			}
		}
	}
	sizeNeedSB.cx = bNeedH;
	sizeNeedSB.cy = bNeedV;

CPoint ptMove_ISR = OnSwGetScrollPos();
	if(		dwScrollTypeH == __ESIW_ST_ITEM
		||	dwScrollTypeH == __ESIW_ST_VIRTUAL
		)
		ptMove.x = ptMove_ISR.x;
	else if( dwScrollTypeH == __ESIW_ST_NONE )
		sizeNeedSB.cx = 0;
	
	if(		dwScrollTypeV == __ESIW_ST_ITEM
		||	dwScrollTypeV == __ESIW_ST_VIRTUAL
		)
		ptMove.y = ptMove_ISR.y;
	else if( dwScrollTypeV == __ESIW_ST_NONE )
		sizeNeedSB.cy = 0;
}

bool CExtScrollItemWnd::OnSwCanAutoHideScrollBar( bool bHorz ) const
{
	ASSERT_VALID( this );
	return SiwAutohideScrollBarGet( bHorz );
}

bool CExtScrollItemWnd::_NotifyCacheChanged(
	LONG nPosH,
	LONG nPosOldH,
	LONG nMaxH,
	LONG nPageSizeH,
	LONG nPosV,
	LONG nPosOldV,
	LONG nMaxV,
	LONG nPageSizeV,
	bool bBeginH,
	bool bEndH,
	bool bBeginV,
	bool bEndV
	)
{
	ASSERT_VALID( this );
	nPosOldH;
	nPosOldV;
	ASSERT(
			nPosH >= 0L
		&&	nPosOldH >= 0L
		&&	nPosV >= 0L
		&&	nPosOldV >= 0L
		&&	nMaxH >= 0L
		&&	nMaxV >= 0L
		&&	nPageSizeH >= 0L
		&&	nPageSizeV >= 0L
		);
	ASSERT( 0L <= nPosH && nPosH <= nMaxH );
	ASSERT( 0L <= nPosOldH && nPosOldH <= nMaxH );
	ASSERT( 0L <= nPosV && nPosV <= nMaxV );
	ASSERT( 0L <= nPosOldV && nPosOldV <= nMaxV );

//DWORD dwScrollTypeH = SiwScrollTypeHGet();
DWORD dwScrollTypeV = SiwScrollTypeVGet();
	if(		dwScrollTypeV != __ESIW_ST_NONE
		&&	dwScrollTypeV != __ESIW_ST_PIXEL
		&&	OnSwHasScrollBar( false )
		)
	{
		if( nPosV < nMaxV )
		{
			nPageSizeV++;
		}
	}

CExtScrollItemCacheInfo _sciNew(
		nPosH,
		nMaxH,
		nPageSizeH,
		nPosV,
		nMaxV,
		nPageSizeV,
		bBeginH,
		bEndH,
		bBeginV,
		bEndV
		);
	ASSERT( _sciNew.IsValid() );
	if( m_sciLast == _sciNew )
		return true;
	if( ! OnSiwCacheChanged( _sciNew, m_sciLast ) )
		return false;
	m_sciLast = _sciNew;
	return true;
}

bool CExtScrollItemWnd::OnSiwCacheChanged(
	const CExtScrollItemCacheInfo & _sciNew,
	const CExtScrollItemCacheInfo & _sciOld
	)
{
	ASSERT_VALID( this );
	ASSERT( _sciNew.IsValid() );
	ASSERT( _sciOld.IsValid() );
	_sciNew;
	_sciOld;
///////////////////////////////////////////////////////////////////////
/// DEBUG BLOCK (BEGIN): verify cache notification is valid
///////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
LONG nPosChangingH = _sciNew.m_nPosH - _sciOld.m_nPosH;
LONG nPosChangingV = _sciNew.m_nPosV - _sciOld.m_nPosV;
	ASSERT( nPosChangingH == _sciNew.GetPosChangingH(_sciOld) );
	ASSERT( nPosChangingV == _sciNew.GetPosChangingV(_sciOld) );
LONG nPageSizeChangingH = _sciNew.m_nPageSizeH - _sciOld.m_nPageSizeH;
LONG nPageSizeChangingV = _sciNew.m_nPageSizeV - _sciOld.m_nPageSizeV;
	ASSERT( nPageSizeChangingH == _sciNew.GetPageSizeChangingH(_sciOld) );
	ASSERT( nPageSizeChangingV == _sciNew.GetPageSizeChangingV(_sciOld) );
LONG nMaxChangingH = _sciNew.m_nMaxH - _sciOld.m_nMaxH;
LONG nMaxChangingV = _sciNew.m_nMaxV - _sciOld.m_nMaxV;
	ASSERT( nMaxChangingH == _sciNew.GetMaxChangingH(_sciOld) );
	ASSERT( nMaxChangingV == _sciNew.GetMaxChangingV(_sciOld) );
LONG nExtentH_New = _sciNew.m_nMaxH + _sciNew.m_nPageSizeH;
LONG nExtentH_Old = _sciOld.m_nMaxH + _sciOld.m_nPageSizeH;
LONG nExtentV_New = _sciNew.m_nMaxV + _sciNew.m_nPageSizeV;
LONG nExtentV_Old = _sciOld.m_nMaxV + _sciOld.m_nPageSizeV;
LONG nExtentChangingH = nExtentH_New - nExtentH_Old;
LONG nExtentChangingV = nExtentV_New - nExtentV_Old;
	ASSERT( nExtentChangingH == _sciNew.GetExtentChangingH(_sciOld) );
	ASSERT( nExtentChangingV == _sciNew.GetExtentChangingV(_sciOld) );
	if(			nPosChangingH == 0L
			&&	nPosChangingV == 0L
			&&	nPageSizeChangingH == 0L
			&&	nPageSizeChangingV == 0L
			&&	nMaxChangingH == 0L
			&&	nMaxChangingV == 0L
			&&	nExtentChangingH == 0L
			&&	nExtentChangingV == 0L
		)
	{
		ASSERT( FALSE );
	}
//CString strTrace;
//	strTrace.Format(
//		_T("   --> CExtScrollItemWnd::OnSiwCacheChanged()\n")
//		_T("   -->    nPosNewH=%08ld nMaxH=%08ld PG=%08ld BEGIN=%s END=%s\n")
//		_T("   -->    nPosNewV=%08ld nMaxV=%08ld PG=%08ld BEGIN=%s END=%s\n")
//		,
//		_sciNew.m_nPosH, _sciNew.m_nMaxH, _sciNew.m_nPageSizeH,
//			_sciNew.m_bBeginH ? _T("yes") : _T("not"),
//			_sciNew.m_bEndH ? _T("yes") : _T("not"),
//		_sciNew.m_nPosV, _sciNew.m_nMaxV, _sciNew.m_nPageSizeV,
//			_sciNew.m_bBeginV ? _T("yes") : _T("not"),
//			_sciNew.m_bEndV ? _T("yes") : _T("not")
//		);
//	TRACE0( LPCTSTR(strTrace) );
#endif // _DEBUG
///////////////////////////////////////////////////////////////////////
/// DEBUG BLOCK (END): verify cache notification is valid
///////////////////////////////////////////////////////////////////////
	return true;
}

#endif // (!defined __EXT_MFC_NO_SCROLLITEMWND)

#if (!defined __EXT_MFC_NO_SCROLLCONAINERWND)

/////////////////////////////////////////////////////////////////////////////
// CExtScrollContainerWnd

IMPLEMENT_DYNCREATE( CExtScrollContainerWnd, CExtScrollWnd );

CExtScrollContainerWnd::CExtScrollContainerWnd()
	: m_sizeToScroll( 0, 0 )
	, m_clrBkColor( COLORREF(-1L) )
	, m_bAutoDelete( false )
	, m_bRedirectPreTranslateMessage( false )
	, m_bRedirectOnCmdMsg( false )
	, m_bRedirectFocus( true )
	, m_bSynchronizeEnabledState( false )
	, m_nScrollRedrawFlags( RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_ALLCHILDREN )
{
}

CExtScrollContainerWnd::~CExtScrollContainerWnd()
{
}

BEGIN_MESSAGE_MAP( CExtScrollContainerWnd, CExtScrollWnd )
	//{{AFX_MSG_MAP(CExtScrollContainerWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CExtScrollContainerWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( ! CExtScrollWnd::PreCreateWindow( cs ) )
		return FALSE;
	cs.dwExStyle &= ~(WS_EX_CLIENTEDGE|WS_EX_STATICEDGE);
	cs.dwExStyle |= WS_EX_CONTROLPARENT;
	cs.style &= ~(WS_BORDER);
	cs.style |= WS_CLIPSIBLINGS|WS_CLIPCHILDREN;
	cs.lpszClass = ::AfxRegisterWndClass( CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, ::LoadCursor( NULL, IDC_ARROW ), NULL, NULL );
	return TRUE;
}

CPoint CExtScrollContainerWnd::OnSwGetScrollPaintPos() const
{
	ASSERT_VALID( this );
	return CPoint( 0, 0 );
}

void CExtScrollContainerWnd::OnSwPaint( CDC & dc )
{
	ASSERT_VALID( this );
	if( m_clrBkColor != COLORREF(-1L) )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		dc.FillSolidRect(
			&rcClient,
			m_clrBkColor
			);
		return;
	}
	if( ! g_PaintManager->PaintDockerBkgnd(
			true,
			dc,
			this
			)
		)
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		dc.FillSolidRect(
			&rcClient,
			g_PaintManager->GetColor( COLOR_BTNFACE, this )
			);
	}
}

void CExtScrollContainerWnd::OnSwPaintAreaBetweenScrollBarWindows(
	CDC & dc,
	const RECT & rcAreaBetweenScrollBarWindows
	)
{
	ASSERT_VALID( this );
	dc.FillSolidRect(
		&rcAreaBetweenScrollBarWindows,
		g_PaintManager->GetColor( COLOR_BTNFACE, this )
		);
}

CSize CExtScrollContainerWnd::OnSwGetTotalSize() const
{
	ASSERT_VALID( this );
	return m_sizeToScroll;
}

CSize CExtScrollContainerWnd::OnSwGetPageSize( int nDirection ) const
{
	ASSERT_VALID( this );
	nDirection;
	return CSize( 100, 100 );
}

CSize CExtScrollContainerWnd::OnSwGetLineSize( int nDirection ) const
{
	ASSERT_VALID( this );
	nDirection;
	return CSize( 1, 1 );
}

CScrollBar * CExtScrollContainerWnd::GetScrollBarCtrl(int nBar) const
{
	ASSERT_VALID( this );
	if( m_hWnd == NULL || (! ::IsWindow(m_hWnd) ) )
		return NULL;
	ASSERT( nBar == SB_HORZ || nBar == SB_VERT );
	if( nBar == SB_HORZ )
	{
		if( m_wndScrollBarH.GetSafeHwnd() != NULL )
			return ( const_cast < CExtScrollBar * > ( &m_wndScrollBarH ) );
	} // if( nBar == SB_HORZ )
	else
	{
		if( m_wndScrollBarV.GetSafeHwnd() != NULL )
			return ( const_cast < CExtScrollBar * > ( &m_wndScrollBarV ) );
	} // else from if( nBar == SB_HORZ )
	return NULL;
}

void CExtScrollContainerWnd::PreSubclassWindow() 
{
	CExtScrollWnd::PreSubclassWindow();
}

void CExtScrollContainerWnd::PostNcDestroy()
{
	CExtScrollWnd::PostNcDestroy();
	if( m_bAutoDelete )
		delete this;
}


void CExtScrollContainerWnd::OnSetFocus( CWnd * pOldWnd )
{
	CExtScrollWnd::OnSetFocus( pOldWnd );
	if( ! m_bRedirectFocus )
		return;
CWnd * pWndToScroll = OnGetWndToScroll();
	if( pWndToScroll == NULL )
		return;
	pWndToScroll->SetFocus();
}

void CExtScrollContainerWnd::OnEnable( BOOL bEnable )
{
	CExtScrollWnd::OnEnable( bEnable );
	if( ! m_bSynchronizeEnabledState )
		return;
CWnd * pWndToScroll = OnGetWndToScroll();
	if( pWndToScroll == NULL )
		return;
	pWndToScroll->EnableWindow( bEnable );
}

BOOL CExtScrollContainerWnd::PreTranslateMessage( MSG * pMsg )
{
	if( m_bRedirectPreTranslateMessage )
	{
		CWnd * pWndToScroll = OnGetWndToScroll();
		if(		pWndToScroll != NULL
			&&	pWndToScroll->PreTranslateMessage( pMsg )
			)
			return TRUE;
	}
	return CExtScrollWnd::PreTranslateMessage( pMsg );
}

BOOL CExtScrollContainerWnd::OnCmdMsg( UINT nID, int nCode, void * pExtra, AFX_CMDHANDLERINFO * pHandlerInfo )
{
	if( m_bRedirectOnCmdMsg )
	{
		CWnd * pWndToScroll = OnGetWndToScroll();
		if(		pWndToScroll != NULL
			&&	pWndToScroll->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo )
			)
			return TRUE;
	}
	return CExtScrollWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

int CExtScrollContainerWnd::OnCreate( LPCREATESTRUCT lpCreateStruct ) 
{
	if( CExtScrollWnd::OnCreate( lpCreateStruct ) == -1 )
		return -1;
	
	m_wndScrollBarH.m_eSO = CExtScrollBar::__ESO_BOTTOM;
	m_wndScrollBarV.m_eSO = CExtScrollBar::__ESO_RIGHT;
	if( ! m_wndScrollBarV.Create(
			WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|SBS_VERT|SBS_RIGHTALIGN,
			CRect(0,0,0,0),
			this,
			1
			)
		)
	{
		ASSERT( FALSE );
		return -1;
	}
	if( ! m_wndScrollBarH.Create(
			WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|SBS_HORZ|SBS_BOTTOMALIGN,
			CRect(0,0,0,0),
			this,
			2
			)
		)
	{
		ASSERT( FALSE );
		return -1;
	}
	m_wndScrollBarH.SyncReservedSpace( &m_wndScrollBarV );
	m_wndScrollBarV.SyncReservedSpace( &m_wndScrollBarH );
	if( ! m_wndCorner.Create(
				_T(""),
				WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
				CRect(0,0,0,0),
				this,
				3
				)
			)
	{
		ASSERT( FALSE );
		return -1;
	}
	OnSwRecalcLayout( true );
	
	return 0;
}

CRect CExtScrollContainerWnd::OnSwRecalcLayout(
	bool bDoLayout,
	LPCRECT pRectClientSrc // = NULL
	)
{
	ASSERT_VALID( this );
CWnd * pWndToScroll = OnGetWndToScroll();
	if( m_sizeToScroll.cx == 0 && m_sizeToScroll.cy == 0 )
	{
		if( pWndToScroll != NULL )
		{
			ASSERT_VALID( pWndToScroll );
			CRect rcWnd;
			pWndToScroll->GetWindowRect( &rcWnd );
			m_sizeToScroll = rcWnd.Size();
		} // if( pWndToScroll != NULL )
	} // if( m_sizeToScroll.cx == 0 && m_sizeToScroll.cy == 0 )
	else
	{
		if( pWndToScroll == NULL )
			m_sizeToScroll.cx = m_sizeToScroll.cy = 0;
	} // else from if( m_sizeToScroll.cx == 0 && m_sizeToScroll.cy == 0 )
CRect rc = CExtScrollWnd::OnSwRecalcLayout( bDoLayout, pRectClientSrc ); 
	if(		( ! bDoLayout)
		||	m_sizeToScroll.cx == 0
		||	m_sizeToScroll.cy == 0
		)
		return rc;
	ASSERT_VALID( pWndToScroll );
CSize _sizeFree = rc.Size();
CSize _sizeEffective(
		max( _sizeFree.cx, m_sizeToScroll.cx ),
		max( _sizeFree.cy, m_sizeToScroll.cy )
		);
LONG lPosHorz = ScrollPos32Get( SB_HORZ );
LONG lPosVert = ScrollPos32Get( SB_VERT );
CSize sizeSB( 0, 0 );
	if(		m_wndCorner.GetSafeHwnd() != NULL
		&&	m_wndScrollBarH.GetSafeHwnd() != NULL
		&&	m_wndScrollBarV.GetSafeHwnd() != NULL
		)
	{
		CRect rcCorner( 0, 0, 0, 0 ), rcSBH( 0, 0, 0, 0 ), rcSBV( 0, 0, 0, 0 );
		bool bVisibleH = ( ( m_wndScrollBarH.GetStyle() & WS_VISIBLE ) != 0 ) ? true : false;
		bool bVisibleV = ( ( m_wndScrollBarV.GetStyle() & WS_VISIBLE ) != 0 ) ? true : false;
		if( bVisibleH )
		{
			m_wndScrollBarH.GetWindowRect( &rcSBH );
			sizeSB.cy = rcSBH.Height();
		}
		if( bVisibleV )
		{
			m_wndScrollBarV.GetWindowRect( &rcSBV );
			sizeSB.cx = rcSBV.Width();
		}
		if( bVisibleH && bVisibleV )
		{
			rcCorner.SetRect( rcSBV.left, rcSBH.top, rcSBV.right, rcSBH.bottom );
			ScreenToClient( &rcCorner );
		}
		if( ! rcCorner.IsRectEmpty() )
			m_wndCorner.SetWindowPos(
				&wndTop,
				rcCorner.left, rcCorner.top, rcCorner.Width(), rcCorner.Height(),
				SWP_NOACTIVATE|SWP_SHOWWINDOW|SWP_NOOWNERZORDER|SWP_NOCOPYBITS
				);
		else if( ( m_wndCorner.GetStyle() & WS_VISIBLE ) != 0 )
			m_wndCorner.ShowWindow( SW_HIDE );
	}
	if( m_bSynchronizeEnabledState )
	{
		BOOL bEnabledContainer = IsWindowEnabled();
		BOOL bEnabledWndToScroll = pWndToScroll->IsWindowEnabled();
		if(		( bEnabledContainer && (!bEnabledWndToScroll) )
			||	( (!bEnabledContainer) && bEnabledWndToScroll )
			)
			pWndToScroll->EnableWindow( bEnabledContainer );
	}
COLORREF clrCornerBk = m_wndCorner.GetBkColor();
	if( m_clrBkColor != clrCornerBk )
		m_wndCorner.SetBkColor( m_clrBkColor );
	if( bDoLayout )
	{
		if( _sizeFree.cx >= m_sizeToScroll.cx )
		{
			_sizeFree.cx -= sizeSB.cx;
			_sizeEffective.cx = max( _sizeFree.cx, m_sizeToScroll.cx );
		}
		if( _sizeFree.cy >= m_sizeToScroll.cy )
		{
			_sizeFree.cy -= sizeSB.cy;
			_sizeEffective.cy = max( _sizeFree.cy, m_sizeToScroll.cy );
		}
		//pWndToScroll->MoveWindow( -lPosHorz, -lPosVert, _sizeEffective.cx, _sizeEffective.cy );
		pWndToScroll->SetWindowPos(
			&CWnd::wndBottom, -lPosHorz, -lPosVert, _sizeEffective.cx, _sizeEffective.cy,
			SWP_NOACTIVATE|SWP_SHOWWINDOW|SWP_NOOWNERZORDER|SWP_NOCOPYBITS
			);
		OnSwUpdateScrollBars();
	}
	return rc;
}

bool CExtScrollContainerWnd::OnSwDoScrollBy(
	CSize sizeScroll,
	bool bDoScroll // = true
	)
{
	ASSERT_VALID( this );
	CExtScrollWnd::OnSwDoScrollBy(sizeScroll,bDoScroll);
	if(		m_sizeToScroll.cx == 0
		||	m_sizeToScroll.cy == 0
		)
		return true;
CWnd * pWndToScroll = OnGetWndToScroll();
	ASSERT_VALID( pWndToScroll );
CRect rc = OnSwGetClientRect();
CSize _sizeFree = rc.Size();
CSize _sizeEffective(
		max( _sizeFree.cx, m_sizeToScroll.cx ),
		max( _sizeFree.cy, m_sizeToScroll.cy )
		);
LONG lPosHorz = ScrollPos32Get( SB_HORZ );
LONG lPosVert = ScrollPos32Get( SB_VERT );
	//pWndToScroll->MoveWindow( -lPosHorz, -lPosVert, _sizeEffective.cx, _sizeEffective.cy );	
	pWndToScroll->SetWindowPos( &CWnd::wndBottom, -lPosHorz, -lPosVert, _sizeEffective.cx, _sizeEffective.cy, SWP_SHOWWINDOW );
	if( m_nScrollRedrawFlags != 0 )
		RedrawWindow( NULL, NULL, m_nScrollRedrawFlags );
	return true;
}

bool CExtScrollContainerWnd::Create(
	CWnd * pWndParent,
	const RECT & rc,
	UINT nDlgCtrlID // = AFX_IDW_PANE_FIRST
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() != NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	if( ! CWnd::Create(
			NULL, NULL, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
			rc, pWndParent, nDlgCtrlID )
		)
	{
		ASSERT( FALSE );
		return false;
	}
	return true;
}

CWnd * CExtScrollContainerWnd::OnGetWndToScroll()
{
	ASSERT_VALID( this );
CWnd * pWndToScroll = GetWindow( GW_CHILD );
	for( ; pWndToScroll != NULL; pWndToScroll = pWndToScroll->GetWindow( GW_HWNDNEXT ) )
	{
		if(		pWndToScroll->IsKindOf( RUNTIME_CLASS( CExtScrollBar ) )
			||	LPVOID(pWndToScroll) == LPVOID(&m_wndCorner)
			)
			continue;
		return pWndToScroll;
	}
	return NULL;
}

#endif // (!defined __EXT_MFC_NO_SCROLLCONAINERWND)

#if (!defined __EXT_MFC_NO_SCROLLRICHCONTENTWND)

/////////////////////////////////////////////////////////////////////////////
// CExtRichContentScrollWnd window

#define __EXT_RCS_WND_APPLY_HOVER_RESET_WORKAROUND__

IMPLEMENT_DYNCREATE( CExtRichContentScrollWnd, CExtScrollWnd );

CExtRichContentScrollWnd::CExtRichContentScrollWnd()
	: m_pLayout( NULL )
	, m_bDirectCreateCall( false )
	, m_bRcsDelayRecalcLayout( false )
	, m_dwRichContentScrollWndStyle( __EXT_RCS_DEFAULT )
	, m_dwRichContentScrollWndStyleEx( __EXT_RCS_EX_DEFAULT )
	, m_bExtractResourceStyles( true )
	, m_rcLayoutLast( -32767, -32767, -32767, -32767 )
	, m_bProcessingCancelMode( false )
	, m_nMouseActivateCode( MA_ACTIVATE )
	, m_hCursorDefault( NULL )
	, m_nVkPressed( VK_LBUTTON )
	, m_bPressedTracking( false )
	, m_pRCI_focus( NULL )
	, m_pHtItemScrollBarTracking( NULL )
	, m_pRCI_ScrollBarTracking( NULL )
{
//	m_bScrollPhysical = true;
	VERIFY( RegisterRichContentScrollWndClass() );
}

CExtRichContentScrollWnd::~CExtRichContentScrollWnd()
{
	if( m_pLayout != NULL )
	{
		delete m_pLayout;
		m_pLayout = NULL;
	}
}

BEGIN_MESSAGE_MAP( CExtRichContentScrollWnd, CExtScrollWnd )
	//{{AFX_MSG_MAP(CExtRichContentScrollWnd)
	ON_WM_SETCURSOR()
	ON_WM_CANCELMODE()
	ON_WM_CAPTURECHANGED()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_MOUSEACTIVATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_GETDLGCODE()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SHOWWINDOW()
	ON_WM_ACTIVATEAPP()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_WM_ACTIVATEAPP()
END_MESSAGE_MAP()

CExtRichContentLayout * CExtRichContentScrollWnd::OnRcsQueryLayout()
{
	ASSERT_VALID( this );
	if( m_pLayout == NULL )
	{
		m_pLayout = new CExtRichContentLayout;
		m_pLayout->ListenerAdd( this );
	}
	return m_pLayout;
}

void CExtRichContentScrollWnd::RcsDelayRecalcLayout()
{
	ASSERT_VALID( this );
	m_bRcsDelayRecalcLayout = true;
}

CExtRichContentLayout * CExtRichContentScrollWnd::RcsLayoutGet()
{
	ASSERT_VALID( this );
	return OnRcsQueryLayout();
}

const CExtRichContentLayout * CExtRichContentScrollWnd::RcsLayoutGet() const
{
	ASSERT_VALID( this );
	return ( const_cast < CExtRichContentScrollWnd * > ( this ) ) -> OnRcsQueryLayout();
}

bool CExtRichContentScrollWnd::g_bRichContentScrollWndClassRegistered = false;

bool CExtRichContentScrollWnd::RegisterRichContentScrollWndClass()
{
	if( g_bRichContentScrollWndClassRegistered )
		return true;
WNDCLASS _wndClassInfo;
HINSTANCE hInst = ::AfxGetInstanceHandle();
	if( ! ::GetClassInfo( hInst, __EXT_RICH_CONTENT_SCROLL_WND_CLASS_NAME, &_wndClassInfo ) )
	{
		_wndClassInfo.style = CS_GLOBALCLASS|CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW;
		_wndClassInfo.lpfnWndProc = ::DefWindowProc;
		_wndClassInfo.cbClsExtra = _wndClassInfo.cbWndExtra = 0;
		_wndClassInfo.hInstance = hInst;
		_wndClassInfo.hIcon = NULL;
		_wndClassInfo.hCursor = ::LoadCursor( NULL, IDC_ARROW );
		ASSERT( _wndClassInfo.hCursor != NULL );
		_wndClassInfo.hbrBackground = NULL; 
		_wndClassInfo.lpszMenuName = NULL;
		_wndClassInfo.lpszClassName = __EXT_RICH_CONTENT_SCROLL_WND_CLASS_NAME;
		if( ! ::AfxRegisterClass( &_wndClassInfo ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}
	g_bRichContentScrollWndClassRegistered = true;
	return true;
}

bool CExtRichContentScrollWnd::Create(
	CWnd * pParentWnd,
	const RECT & rcWnd, // = CRect( 0, 0, 0, 0 )
	UINT nDlgCtrlID, // = UINT( __EXT_MFC_IDC_STATIC )
	DWORD dwRichContentScrollWndStyle, // = 0L
	DWORD dwRichContentScrollWndStyleEx, // = 0L
	DWORD dwWindowStyle, // = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
	CCreateContext * pContext // = NULL
	)
{
	ASSERT_VALID( this );
	pContext;
	if( ! RegisterRichContentScrollWndClass() )
	{
		ASSERT( FALSE );
		return false;
	}
	m_bDirectCreateCall = true;
	m_dwRichContentScrollWndStyle = dwRichContentScrollWndStyle;
	m_dwRichContentScrollWndStyleEx = dwRichContentScrollWndStyleEx;
	if( ! CWnd::CreateEx( 0L, __EXT_RICH_CONTENT_SCROLL_WND_CLASS_NAME, _T(""), dwWindowStyle, rcWnd, pParentWnd, nDlgCtrlID ) )
	{
		ASSERT( FALSE );
		return false;
	}
	if( !_CreateHelper() )
	{
		ASSERT( FALSE );
		AfxThrowMemoryException();
	}
	return true;
}

void CExtRichContentScrollWnd::PreSubclassWindow()
{
	ASSERT_VALID( this );
	CExtScrollWnd::PreSubclassWindow();
	m_rcLayoutLast.SetRect( -32767, -32767, -32767, -32767 );
	m_bRcsDelayRecalcLayout = m_bProcessingCancelMode = false;
	m_pRCI_focus = NULL;
	m_bPressedTracking = false;
	m_htInfoHover.Empty();
	m_htInfoPressed.Empty();
	if( m_bDirectCreateCall )
		return;
	if( m_bExtractResourceStyles )
	{
		__EXT_MFC_LONG_PTR dwStyle = ::__EXT_MFC_GetWindowLong( m_hWnd, GWL_STYLE );
		m_dwRichContentScrollWndStyle = DWORD( dwStyle & __EXTMFC_ALL_FORM_MOVABLE_WND_STYLES );
		::__EXT_MFC_SetWindowLong( m_hWnd, GWL_STYLE, dwStyle & (~__EXTMFC_ALL_FORM_MOVABLE_WND_STYLES) );
	}
	if( !_CreateHelper() )
	{
		ASSERT( FALSE );
		AfxThrowMemoryException();
	}
}

BOOL CExtRichContentScrollWnd::PreCreateWindow( CREATESTRUCT & cs )
{
	ASSERT_VALID( this );
	if( ( ! RegisterRichContentScrollWndClass() )
		|| ( !CExtScrollWnd::PreCreateWindow( cs ) )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}	
	cs.lpszClass = __EXT_RICH_CONTENT_SCROLL_WND_CLASS_NAME;
	cs.style |= WS_CLIPSIBLINGS|WS_CLIPCHILDREN;
	return TRUE;
}

LRESULT CExtRichContentScrollWnd::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_SETTEXT:
		RcsTextSet( LPCTSTR(lParam) );
		return TRUE;
	case WM_GETTEXTLENGTH:
		return LRESULT( RcsTextGet().GetLength() );
	case WM_GETTEXT:
		{
			__EXT_MFC_INT_PTR nRequested = (__EXT_MFC_INT_PTR)wParam;
			if( nRequested < 1 )
				return 0L;
			CString str = RcsTextGet();
			__EXT_MFC_INT_PTR nPresent = str.GetLength() + 1;
			ASSERT( nPresent >= 1 );
			__EXT_MFC_INT_PTR nCopyZ = min( nPresent, nRequested );
			ASSERT( nCopyZ >= 1 );
			__EXT_MFC_STRNCPY( LPTSTR(lParam), nRequested, LPCTSTR(str), nCopyZ - 1 );
			( * ( LPTSTR(lParam) + nCopyZ ) ) = _T('\0');
			return nCopyZ;
		}
	//break;
	} // switch( message )
LRESULT lResult = CExtScrollWnd::WindowProc( message, wParam, lParam );
	return lResult;
}

BOOL CExtRichContentScrollWnd::PreTranslateMessage( MSG * pMsg )
{
	if( pMsg->hwnd == m_hWnd && GetSafeHwnd() != NULL )
	{
		if(		//WM_MOUSEFIRST <= pMsg->message && pMsg->message <= WM_MOUSELAST
				(	pMsg->message == WM_MOUSEMOVE
				||	pMsg->message == WM_LBUTTONDOWN
				||	pMsg->message == WM_LBUTTONUP
				||	pMsg->message == WM_LBUTTONDBLCLK
				||	pMsg->message == WM_MBUTTONDOWN
				||	pMsg->message == WM_MBUTTONUP
				||	pMsg->message == WM_MBUTTONDBLCLK
				||	pMsg->message == WM_RBUTTONDOWN
				||	pMsg->message == WM_RBUTTONUP
				||	pMsg->message == WM_RBUTTONDBLCLK
				)
			&&	( RcsGetStyle() & __EXT_RCS_EVENTS_MOUSE_PRE_TRANSLATE ) != 0
			)
		{
			if( pMsg->message == WM_MOUSEMOVE )
			{
				static CPoint g_ptLast( -32767, -32767 );
				CPoint pt( LOWORD(pMsg->lParam), HIWORD(pMsg->lParam) );
				ClientToScreen( &pt );
				if( g_ptLast != pt )
				{
					g_ptLast = pt;
					_HandlePopOverScrollBarVisibility( true );
				}
			}

			if( ( pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_MOUSEMOVE ) && m_bDisplayPopOverScrollBars )
			{
				CPoint ptCursor( LOWORD(pMsg->lParam), HIWORD(pMsg->lParam) );
				INT nSBMHT = INT(CExtPaintManager::__ESBMHT_NOWHERE);
				bool bHorz = false;
				if( OnSwPopOverScrollBarHitTest( ptCursor, nSBMHT, bHorz ) )
				{
					if( pMsg->message == WM_LBUTTONDOWN )
						OnSwTrackPopOverScrollBar( ptCursor, bHorz, nSBMHT );
					else if( pMsg->message == WM_MOUSEMOVE )
						OnSwSetPopOverScrollBarCursor( ptCursor, bHorz, nSBMHT );
					return TRUE;
				} // if( OnSwPopOverScrollBarHitTest( ptCursor, nSBMHT, bHorz ) )
			}

			switch( pMsg->message )
			{
			case WM_MOUSEMOVE:
				if( OnRcsAnalyzeMouseMoveEvent( UINT(pMsg->wParam), CPoint( LOWORD(DWORD(pMsg->lParam)), HIWORD(DWORD(pMsg->lParam)) ) ) )
					return TRUE;
			break;
			case WM_LBUTTONDOWN:
				if( OnRcsAnalyzeMouseClickEvent( VK_LBUTTON, 1, UINT(pMsg->wParam), CPoint( LOWORD(DWORD(pMsg->lParam)), HIWORD(DWORD(pMsg->lParam)) ) ) )
					return TRUE;
			break;
			case WM_LBUTTONUP:
				if( OnRcsAnalyzeMouseClickEvent( VK_LBUTTON, 0, UINT(pMsg->wParam), CPoint( LOWORD(DWORD(pMsg->lParam)), HIWORD(DWORD(pMsg->lParam)) ) ) )
					return TRUE;
			break;
			case WM_LBUTTONDBLCLK:
				if( OnRcsAnalyzeMouseClickEvent( VK_LBUTTON, 2, UINT(pMsg->wParam), CPoint( LOWORD(DWORD(pMsg->lParam)), HIWORD(DWORD(pMsg->lParam)) ) ) )
					return TRUE;
			break;
			case WM_MBUTTONDOWN:
				if( OnRcsAnalyzeMouseClickEvent( VK_MBUTTON, 1, UINT(pMsg->wParam), CPoint( LOWORD(DWORD(pMsg->lParam)), HIWORD(DWORD(pMsg->lParam)) ) ) )
					return TRUE;
			break;
			case WM_MBUTTONUP:
				if( OnRcsAnalyzeMouseClickEvent( VK_MBUTTON, 0, UINT(pMsg->wParam), CPoint( LOWORD(DWORD(pMsg->lParam)), HIWORD(DWORD(pMsg->lParam)) ) ) )
					return TRUE;
			break;
			case WM_MBUTTONDBLCLK:
				if( OnRcsAnalyzeMouseClickEvent( VK_MBUTTON, 2, UINT(pMsg->wParam), CPoint( LOWORD(DWORD(pMsg->lParam)), HIWORD(DWORD(pMsg->lParam)) ) ) )
					return TRUE;
			break;
			case WM_RBUTTONDOWN:
				if( OnRcsAnalyzeMouseClickEvent( VK_RBUTTON, 1, UINT(pMsg->wParam), CPoint( LOWORD(DWORD(pMsg->lParam)), HIWORD(DWORD(pMsg->lParam)) ) ) )
					return TRUE;
			break;
			case WM_RBUTTONUP:
				if( OnRcsAnalyzeMouseClickEvent( VK_RBUTTON, 0, UINT(pMsg->wParam), CPoint( LOWORD(DWORD(pMsg->lParam)), HIWORD(DWORD(pMsg->lParam)) ) ) )
					return TRUE;
			break;
			case WM_RBUTTONDBLCLK:
				if( OnRcsAnalyzeMouseClickEvent( VK_RBUTTON, 2, UINT(pMsg->wParam), CPoint( LOWORD(DWORD(pMsg->lParam)), HIWORD(DWORD(pMsg->lParam)) ) ) )
					return TRUE;
			break;
			} // switch( pMsg->message )
		}
		else
		if(		//WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST
				(	pMsg->message == WM_KEYDOWN
				||	pMsg->message == WM_KEYUP
				||	pMsg->message == WM_SYSKEYDOWN
				||	pMsg->message == WM_SYSKEYUP
				)
			&&	( RcsGetStyle() & __EXT_RCS_EVENTS_KEYBOARD_PRE_TRANSLATE ) != 0
			&&	::GetFocus() == m_hWnd
			)
		{
			if( pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP )
			{
				if( OnRcsAnalyzeKeyEvent( false, ( pMsg->message == WM_KEYDOWN ) ? true : false, UINT(pMsg->wParam), UINT(LOWORD(DWORD(pMsg->lParam))), UINT(HIWORD(DWORD(pMsg->lParam))) ) )
					return TRUE;
			}
			else if( pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_SYSKEYUP )
			{
				if( OnRcsAnalyzeKeyEvent( true, ( pMsg->message == WM_SYSKEYDOWN ) ? true : false, UINT(pMsg->wParam), UINT(LOWORD(DWORD(pMsg->lParam))), UINT(HIWORD(DWORD(pMsg->lParam))) ) )
					return TRUE;
			}
		}
	} // if( pMsg->hwnd == m_hWnd && GetSafeHwnd() != NULL )
	return CExtScrollWnd::PreTranslateMessage( pMsg );
}

bool CExtRichContentScrollWnd::_CreateHelper()
{
	ASSERT_VALID( this );
	OnSwUpdateScrollBars();
	//OnSwDoRedraw();
	return true;
}

DWORD CExtRichContentScrollWnd::RcsGetStyle() const
{
	ASSERT( this != NULL );
	return m_dwRichContentScrollWndStyle;
}

DWORD CExtRichContentScrollWnd::RcsModifyStyle(
	DWORD dwStyleAdd,
	DWORD dwStyleRemove, // = 0L
	bool bUpdateWnd // = true
	)
{
	ASSERT( this != NULL );
DWORD dwRichContentScrollWndStyleOld = RcsGetStyle();
	if( dwStyleAdd == 0 && dwStyleRemove == 0 )
		return dwRichContentScrollWndStyleOld;
	m_dwRichContentScrollWndStyle &= ~dwStyleRemove;
	m_dwRichContentScrollWndStyle |= dwStyleAdd;
	if(		bUpdateWnd
		&&	GetSafeHwnd() != NULL
		)
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	}
	return dwRichContentScrollWndStyleOld;
}

DWORD CExtRichContentScrollWnd::RcsGetStyleEx() const
{
	ASSERT( this != NULL );
	return m_dwRichContentScrollWndStyleEx;
}

DWORD CExtRichContentScrollWnd::RcsModifyStyleEx(
	DWORD dwStyleExAdd,
	DWORD dwStyleExRemove, // = 0L
	bool bUpdateWnd // = true
	)
{
	ASSERT( this != NULL );
DWORD dwRichContentScrollWndStyleExOld = RcsGetStyle();
	if( dwStyleExAdd == 0 && dwStyleExRemove == 0 )
		return dwRichContentScrollWndStyleExOld;
	m_dwRichContentScrollWndStyleEx &= ~dwStyleExRemove;
	m_dwRichContentScrollWndStyleEx |= dwStyleExAdd;
	if(		bUpdateWnd
		&&	GetSafeHwnd() != NULL
		)
	{
		OnSwUpdateScrollBars();
		OnSwDoRedraw();
	}
	return dwRichContentScrollWndStyleExOld;
}

CString CExtRichContentScrollWnd::RcsTextGet() const
{
	ASSERT_VALID( this );
	return m_strRcsText;
}

void CExtRichContentScrollWnd::RcsTextSet(
	LPCTSTR strText, // = NULL
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( strText == NULL )
		strText = _T("");
	if( m_strRcsText == strText )
		return;
	m_pHtItemScrollBarTracking = NULL;
	m_pRCI_ScrollBarTracking = NULL;
	m_pRCI_focus = NULL;
	m_bPressedTracking = false;
	m_htInfoHover.Empty();
	m_htInfoPressed.Empty();
	m_rcLayoutLast.SetRect( -32767, -32767, -32767, -32767 );
	m_strRcsText = LPCTSTR(strText);
	if( strText == NULL )
		strText = _T("");
CExtRichContentLayout * pRCL = RcsLayoutGet();
	if( pRCL != NULL )
	{
		bool bEmptyLocalObjectContainerOnParse = ( ( RcsGetStyle() & __EXT_RCS_EMPTY_LOCAL_OBJECT_CONTAINER_ON_PARSE ) != 0 ) ? true : false;
		pRCL->Parse( strText, CExtRichContentLayout::__ELFMT_HTML, 0, 0, bEmptyLocalObjectContainerOnParse );
	}
	if( ! bRedraw )
		return;
	if( GetSafeHwnd() == NULL )
		return;
	OnSwInvalidate( m_bRedrawErase );
	RcsDelayRecalcLayout();
	OnSwRecalcLayout( true );
	OnSwUpdateScrollBars();
}

CSize CExtRichContentScrollWnd::OnSwGetLineSize( int nDirection ) const
{
	ASSERT_VALID( this );
const CExtRichContentLayout * pRCL = RcsLayoutGet();
	if( pRCL == NULL )
		return CSize( 0, 0 );
const CExtRichContentItem * pRootRCI = ( pRCL->m_listHeap.GetCount() > 0 ) ? pRCL->m_listHeap.GetHead() : NULL;
	if( pRootRCI == NULL )
		return CSize( 0, 0 );
CSize sizePage = OnSwGetPageSize( nDirection );
CSize sizeLine( sizePage.cx / 20, sizePage.cy / 20 );
	sizeLine.cx = max( sizeLine.cx, 1 );
	sizeLine.cy = max( sizeLine.cy, 1 );
	return sizeLine;
}

CSize CExtRichContentScrollWnd::OnSwGetPageSize( int nDirection ) const
{
	ASSERT_VALID( this );
	nDirection;
const CExtRichContentLayout * pRCL = RcsLayoutGet();
	if( pRCL == NULL )
		return CSize( 0, 0 );
const CExtRichContentItem * pRootRCI = ( pRCL->m_listHeap.GetCount() > 0 ) ? pRCL->m_listHeap.GetHead() : NULL;
	if( pRootRCI == NULL )
		return CSize( 0, 0 );
CSize sizePage = OnSwGetClientRect().Size();
	return sizePage;
}

CSize CExtRichContentScrollWnd::OnSwGetTotalSize() const
{
	ASSERT_VALID( this );
const CExtRichContentLayout * pRCL = RcsLayoutGet();
	if( pRCL == NULL )
		return CSize( 0, 0 );
const CExtRichContentItem * pRootRCI = ( pRCL->m_listHeap.GetCount() > 0 ) ? pRCL->m_listHeap.GetHead() : NULL;
	if( pRootRCI == NULL )
		return CSize( 0, 0 );
CSize sizeTotal = pRootRCI->m_rcLayout.Size();
	return sizeTotal;
}

HFONT CExtRichContentScrollWnd::OnRcsGetFont() const
{
	ASSERT_VALID( this );
	return (HFONT) PmBridge_GetPM()->m_FontNormal.GetSafeHandle();
}

void CExtRichContentScrollWnd::OnSwPaint( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CExtRichContentLayout * pRCL = RcsLayoutGet();
	if( pRCL == NULL )
		return;
	if( m_bRcsDelayRecalcLayout )
		OnSwRecalcLayout( true );
CPoint ptScrollPos = OnSwGetScrollPaintPos();
CRect rcClient = OnSwGetClientRect();
CRect rcDocRenderTarget( rcClient );
	rcDocRenderTarget.OffsetRect( ptScrollPos );
CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );
COLORREF clrText = pPM->GetColor( COLOR_WINDOWTEXT, this );
COLORREF clrBack = pPM->GetColor( COLOR_WINDOW, this );
COLORREF clrOldText = dc.SetTextColor( clrText );
COLORREF clrOldBack = dc.SetBkColor( clrBack );
INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
HFONT hFontRCS = OnRcsGetFont();
CFont * pFont = ( hFontRCS != NULL ) ? CFont::FromHandle( hFontRCS ) : GetFont();
CFont * pOldFont = dc.SelectObject( pFont );
CPoint ptOVPO( 0, 0 );
CExtRichContentItem * pRootRCI = ( pRCL->m_listHeap.GetCount() > 0 ) ? pRCL->m_listHeap.GetHead() : NULL;
	if( pRootRCI != NULL )
	{
		CSize sizeLayout = pRootRCI->m_rcLayout.Size(), sizeClient = rcClient.Size();
		switch( pRCL->m_eLO )
		{
		case CExtRichContentLayout::__ELOT_NORMAL:
		break;
		case CExtRichContentLayout::__ELOT_90_CW:
			if( sizeLayout.cx > sizeClient.cx )
				ptOVPO.x = sizeLayout.cx - sizeClient.cx; // - ::GetSystemMetrics(SM_CXVSCROLL);
		break;
		case CExtRichContentLayout::__ELOT_180_CW:
			if( sizeLayout.cx > sizeClient.cx )
				ptOVPO.x = sizeLayout.cx - sizeClient.cx; // - ::GetSystemMetrics(SM_CXVSCROLL);
			if( sizeLayout.cy > sizeClient.cy )
				ptOVPO.y = sizeLayout.cy - sizeClient.cy; // - ::GetSystemMetrics(SM_CYHSCROLL);
		break;
		case CExtRichContentLayout::__ELOT_270_CW:
			if( sizeLayout.cy > sizeClient.cy )
				ptOVPO.y = sizeLayout.cy - sizeClient.cy; // - ::GetSystemMetrics(SM_CYHSCROLL);
		break;
		}
	}
	dc.OffsetViewportOrg( ptOVPO.x, ptOVPO.y );
	pRCL->DoPaint( dc, rcDocRenderTarget, ptScrollPos );
	dc.OffsetViewportOrg( -ptOVPO.x, -ptOVPO.y );
	dc.SelectObject( pOldFont );
	dc.SetBkMode( nOldBkMode );
	dc.SetBkColor( clrOldBack );
	dc.SetTextColor( clrOldText );
}

CRect CExtRichContentScrollWnd::OnSwRecalcLayout(
	bool bDoLayout,
	LPCRECT pRectClientSrc // = NULL
	)
{
	ASSERT_VALID( this );
CRect rcLayout = CExtScrollWnd::OnSwRecalcLayout( bDoLayout, pRectClientSrc );
	if( GetSafeHwnd() == NULL )
		return rcLayout;
bool bRectChanged = ( rcLayout != m_rcLayoutLast ) ? true : false;
	if( m_bRcsDelayRecalcLayout || bRectChanged )
	{
		m_rcLayoutLast = rcLayout;
		if( bDoLayout )
		{
			CExtRichContentLayout * pRCL = RcsLayoutGet();
			if( pRCL != NULL && pRCL->m_listHeap.GetCount() > 0 )
			{
				CExtRichContentItem * pRootRCI = ( pRCL->m_listHeap.GetCount() > 0 ) ? pRCL->m_listHeap.GetHead() : NULL;
				ASSERT( pRootRCI != NULL );
				CRect _rcLayout( rcLayout );
				CSize sizeScrollBars( ::GetSystemMetrics(SM_CXVSCROLL), ::GetSystemMetrics(SM_CYHSCROLL) );
				bool bHaveScrollBarH = OnSwHasScrollBar( true ), bHaveScrollBarV = OnSwHasScrollBar( false );
				CClientDC dc( this );
				HFONT hFontRCS = OnRcsGetFont();
				CFont * pFont = ( hFontRCS != NULL ) ? CFont::FromHandle( hFontRCS ) : GetFont();
				CFont * pOldFont = dc.SelectObject( pFont );
				OnRcsPreLayoutContent( dc, _rcLayout, pFont );
				pRCL->DoLayout( dc, _rcLayout );
				OnRcsPostLayoutContent( dc, _rcLayout, pFont );
				bool bAddSBH = false, bRemoveSBH = false;
				bool bAddSBV = false, bRemoveSBV = false;
				_rcLayout = rcLayout;
				CSize sizeContent = pRootRCI->m_rcLayout.Size(), sizeLayout = _rcLayout.Size();
				if( sizeContent.cx > sizeLayout.cx )
					bAddSBH = (! bHaveScrollBarH);
				else
					bRemoveSBH = bHaveScrollBarH;
				if( sizeContent.cy > sizeLayout.cy )
					bAddSBV = (! bHaveScrollBarV);
				else
					bRemoveSBV = bHaveScrollBarV;
				if( bRectChanged || bAddSBH || bRemoveSBH || bAddSBV || bRemoveSBV )
				{
					if( bAddSBH )
						OnSwEnableScrollBarCtrl( SB_HORZ, true );
					if( bRemoveSBH )
						OnSwEnableScrollBarCtrl( SB_HORZ, false );
					if( bAddSBV )
						OnSwEnableScrollBarCtrl( SB_VERT, true );
					if( bRemoveSBV )
						OnSwEnableScrollBarCtrl( SB_VERT, false );
					m_bRcsDelayRecalcLayout = false;
					OnSwUpdateScrollBars();
					rcLayout = CExtScrollWnd::OnSwRecalcLayout( bDoLayout, pRectClientSrc );
					_rcLayout = OnSwGetClientRect();
					OnRcsPreLayoutContent( dc, _rcLayout, pFont );
					pRCL->DoLayout( dc, _rcLayout );
					OnRcsPostLayoutContent( dc, _rcLayout, pFont );
					m_rcLayoutLast = _rcLayout;
				} // if( bAddSBH || bRemoveSBH || bAddSBV || bRemoveSBV )
				dc.SelectObject( pOldFont );
				pRCL->LayoutAttachedControls( OnSwGetScrollPos() );
				m_bRcsDelayRecalcLayout = false;
			} // if( pRCL != NULL && pRCL->m_listHeap.GetCount() > 0 )
		} // if( bDoLayout )
	} // if( m_bRcsDelayRecalcLayout || bRectChanged )
	return rcLayout;
}

void CExtRichContentScrollWnd::OnRcsPreLayoutContent( CDC & dc, const CRect & rcLayout, CFont * pFont )
	{ ASSERT_VALID( this ); ASSERT( dc.GetSafeHdc() != NULL ); ASSERT( pFont->GetSafeHandle() != NULL ); dc; rcLayout; pFont; }
void CExtRichContentScrollWnd::OnRcsPostLayoutContent( CDC & dc, const CRect & rcLayout, CFont * pFont )
	{ ASSERT_VALID( this ); ASSERT( dc.GetSafeHdc() != NULL ); ASSERT( pFont->GetSafeHandle() != NULL ); dc; rcLayout; pFont; }

bool CExtRichContentScrollWnd::OnSwDoScrollBy(
	CSize sizeScroll,
	bool bDoScroll // = true
	)
{
	ASSERT_VALID( this );
bool bRetVal = CExtScrollWnd::OnSwDoScrollBy( sizeScroll, bDoScroll );
CExtRichContentLayout * pRCL = RcsLayoutGet();
	if( pRCL != NULL )
		pRCL->LayoutAttachedControls( OnSwGetScrollPos() );
	return bRetVal;
}
void CExtRichContentScrollWnd::OnSwUpdateScrollBars()
{
	ASSERT_VALID( this );
	CExtScrollWnd::OnSwUpdateScrollBars();
	if( m_bRcsDelayRecalcLayout )
		OnSwRecalcLayout( true );
}

bool CExtRichContentScrollWnd::OnSwCanAutoHideScrollBar( bool bHorz ) const
{
	ASSERT_VALID( this );
	return CExtScrollWnd::OnSwCanAutoHideScrollBar( bHorz );	
}

bool CExtRichContentScrollWnd::_DoSetCursor( const POINT & ptCursorClient )
{
	ASSERT_VALID( this );
	return OnRcsSetCursor( ptCursorClient );
}

bool CExtRichContentScrollWnd::_DoSetCursor()
{
	ASSERT_VALID( this );
CPoint ptCursorClient;
	if( ! ::GetCursorPos( &ptCursorClient ) )
		return false;
	ScreenToClient( &ptCursorClient );
	return _DoSetCursor( ptCursorClient );
}

BOOL CExtRichContentScrollWnd::OnSetCursor( CWnd * pWnd, UINT nHitTest, UINT message )
{
	ASSERT_VALID( this );
	if( _DoSetCursor() )
		return TRUE;
	return CExtScrollWnd::OnSetCursor( pWnd, nHitTest, message );
}

void CExtRichContentScrollWnd::OnCancelMode()
{
	ASSERT_VALID( this );
	CExtScrollWnd::OnCancelMode();
	if( m_bProcessingCancelMode )
		return;
	m_bProcessingCancelMode = true;
CExtRichContentLayout * pRCL = RcsLayoutGet();
CExtRichContentItem * pRootRCI = ( pRCL->m_listHeap.GetCount() > 0 ) ? pRCL->m_listHeap.GetHead() : NULL;
	if( pRootRCI != NULL )
	{
		bool bHaveElementsWithResetStyles = false;
		bool bQuickReStyleMode = ( ( RcsGetStyle() & __EXT_RCS_QUICK_RESTYLE_MODE ) ) ? true : false;
		if( ! m_htInfoHover.IsEmpty() )
		{
			m_htInfoHover.Empty();
			CExtRichStyleDescription::pseudo_class_def_t statesHoverOnly;
			statesHoverOnly.m_eynHover = CExtRichStyleDescription::e_ui_yn_yes;
			OnRcsResetElementStates( pRootRCI, statesHoverOnly, CExtRichStyleDescription::e_ui_yn_unspecified );
			bHaveElementsWithResetStyles = true;
		} // if( ! m_htInfoHover.IsEmpty() )
		//if( m_bPressedTracking )
		if( m_bPressedTracking || ( ! m_htInfoPressed.IsEmpty() ) )
		{
			m_bPressedTracking = false;
			CExtRichStyleDescription::pseudo_class_def_t statesPressedOnly;
			statesPressedOnly.m_eynPressed = CExtRichStyleDescription::e_ui_yn_yes;
			POSITION pos = m_htInfoPressed.m_listHtBranch.GetHeadPosition();
			for( ; pos != NULL; )
			{
				CExtRichContentHitTestInfo::ht_item_t & _ht_item = m_htInfoPressed.m_listHtBranch.GetNext( pos );
				__EXT_DEBUG_RICH_CONTENT_ASSERT( _ht_item.m_pRCI != NULL );
				_ht_item.m_pRCI->RciEvent_OnPressedLeave( m_nVkPressed, 0, 0, m_htInfoPressed, m_htInfoPressed, m_hWnd );
				if( OnRcsResetElementStates( _ht_item.m_pRCI, statesPressedOnly, CExtRichStyleDescription::e_ui_yn_unspecified ) )
					bHaveElementsWithResetStyles = true;
			} // for( ; pos != NULL; )
			m_htInfoPressed.Empty();
			bHaveElementsWithResetStyles = true;
		} // if( m_bPressedTracking )
		if( bHaveElementsWithResetStyles )
		{
			if( ! bQuickReStyleMode )
			{
				RcsDelayRecalcLayout();
				OnSwRecalcLayout( true );
				OnSwUpdateScrollBars();
			}
			OnSwInvalidate( m_bRedrawErase );
		} // if( bHaveElementsWithResetStyles )
	} // if( pRootRCI != NULL )
	if( ::GetCapture() == m_hWnd )
		::ReleaseCapture();
	m_bProcessingCancelMode = false;
}

#if _MFC_VER < 0x700
void CExtRichContentScrollWnd::OnActivateApp(BOOL bActive, HTASK hTask) 
#else
void CExtRichContentScrollWnd::OnActivateApp(BOOL bActive, DWORD hTask) 
#endif
{
	CExtNSB < CExtScrollWnd > :: OnActivateApp(bActive, hTask);
	if( ! bActive )
		SendMessage( WM_CANCELMODE );
}


void CExtRichContentScrollWnd::OnCaptureChanged( CWnd  * pWnd )
{
	ASSERT_VALID( this );
	CExtScrollWnd::OnCaptureChanged( pWnd );
	if( ::GetCapture() != m_hWnd )
		SendMessage( WM_CANCELMODE );
}

void CExtRichContentScrollWnd::OnSetFocus( CWnd * pOldWnd )
{
	ASSERT_VALID( this );
	CExtScrollWnd::OnSetFocus( pOldWnd );
	OnSwInvalidate( m_bRedrawErase );
}

void CExtRichContentScrollWnd::OnKillFocus( CWnd * pNewWnd )
{
	ASSERT_VALID( this );
	CExtScrollWnd::OnKillFocus( pNewWnd );
	if( m_bPressedTracking )
		SendMessage( WM_CANCELMODE );
	else
		OnSwInvalidate( m_bRedrawErase );
}

int CExtRichContentScrollWnd::OnMouseActivate( CWnd * pDesktopWnd, UINT nHitTest, UINT message )
{
	ASSERT_VALID( this );
	pDesktopWnd; nHitTest; message;
	return m_nMouseActivateCode;
}

void CExtRichContentScrollWnd::OnMouseMove( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ( RcsGetStyle() & __EXT_RCS_EVENTS_MOUSE_WND_PROC ) != 0 && OnRcsAnalyzeMouseMoveEvent( nFlags, point ) )
		return;
	CExtScrollWnd::OnMouseMove( nFlags, point );
}

void CExtRichContentScrollWnd::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ( RcsGetStyle() & __EXT_RCS_EVENTS_MOUSE_WND_PROC ) != 0 && OnRcsAnalyzeMouseClickEvent( VK_LBUTTON, 2, nFlags, point ) )
		return;
	CExtScrollWnd::OnLButtonDblClk( nFlags, point );
}

void CExtRichContentScrollWnd::OnLButtonDown( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
UINT nRepCnt = 1;
static clock_t g_nLastClock = 0;
static HWND g_hWndLastClick = NULL;
static CPoint g_ptLastClick(-32767,-32767);
CPoint ptLastClick = point;
	ClientToScreen( &ptLastClick );
clock_t nCurrClock = ::clock();
	if(		g_hWndLastClick == m_hWnd
		&&	g_ptLastClick == ptLastClick
		)
	{
		clock_t nDiff = (clock_t)::abs( nCurrClock - g_nLastClock );
		clock_t nDoubleClickTime = (clock_t)::GetDoubleClickTime();
		if( nDiff <= nDoubleClickTime )
			nRepCnt = 2;
	}
	g_ptLastClick = ptLastClick;
	g_hWndLastClick = m_hWnd;
	g_nLastClock = nCurrClock;

HWND hWndOwn = m_hWnd, hWndFocus = ::GetFocus();
	if( ( hWndFocus != m_hWnd ) && ( m_nMouseActivateCode == MA_ACTIVATE || m_nMouseActivateCode == MA_ACTIVATEANDEAT ) )
	{ // check MDI child frame activation explicitly
		bool bIsMDIChildWindow = false;
		// find the parent frame of the window that was just clicked
		HWND hWndActivate = ::GetParent( hWndOwn );
		for( ; hWndActivate != NULL; hWndActivate = ::GetParent( hWndActivate ) )
		{
			__EXT_MFC_LONG_PTR dwWndStyle = ::__EXT_MFC_GetWindowLong( hWndActivate, GWL_STYLE );
			if( ( dwWndStyle & WS_CHILD ) == 0 )
				break;
			__EXT_MFC_LONG_PTR dwWndStyleEx = ::__EXT_MFC_GetWindowLong( hWndActivate, GWL_EXSTYLE );
			if( ( dwWndStyleEx & WS_EX_MDICHILD ) == 0 )
				continue;
			bIsMDIChildWindow = true;
			break;
		} // for( ; hWndActivate != NULL; hWndActivate = ::GetParent( hWndActivate ) )
		if( bIsMDIChildWindow )
		{
			// find the parent frame of the window that has focus
			bIsMDIChildWindow = false;
			HWND hWndActivate2 = hWndFocus;
			for( ; hWndActivate2 != NULL; hWndActivate2 = ::GetParent( hWndActivate2 ) )
			{
				__EXT_MFC_LONG_PTR dwWndStyle = ::__EXT_MFC_GetWindowLong( hWndActivate2, GWL_STYLE );
				if( ( dwWndStyle & WS_CHILD ) == 0 )
					break;
				__EXT_MFC_LONG_PTR dwWndStyleEx = ::__EXT_MFC_GetWindowLong( hWndActivate2, GWL_EXSTYLE );
				if( ( dwWndStyleEx & WS_EX_MDICHILD ) == 0 )
					continue;
				bIsMDIChildWindow = true;
				break;
			} // for( ; hWndActivate2 != NULL; hWndActivate2 = ::GetParent( hWndActivate2 ) )
			if( hWndActivate != hWndActivate2 )
			{ // if windows are in different frames, set the focus to the new frame
				ActivateTopParent();
				CFrameWnd * pFrame = GetParentFrame();
				if( pFrame != NULL )
				{
					CMDIChildWnd * pMDIChildWnd = DYNAMIC_DOWNCAST( CMDIChildWnd, pFrame );
					if( pMDIChildWnd != NULL )
					{
						pMDIChildWnd->MDIActivate();
						pMDIChildWnd->SetFocus();
					} // if( pMDIChildWnd != NULL )
				} // if( pFrame != NULL )
			} // if windows are in different frames, set the focus to the new frame
		}
		if( ::GetFocus() != m_hWnd )
			SetFocus();
	} // check MDI child frame activation explicitly

	if( ( RcsGetStyle() & __EXT_RCS_EVENTS_MOUSE_WND_PROC ) != 0 && OnRcsAnalyzeMouseClickEvent( VK_LBUTTON, nRepCnt, nFlags, point ) )
		return;
	CExtScrollWnd::OnLButtonDown( nFlags, point );
}

void CExtRichContentScrollWnd::OnLButtonUp( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ( RcsGetStyle() & __EXT_RCS_EVENTS_MOUSE_WND_PROC ) != 0 && OnRcsAnalyzeMouseClickEvent( VK_LBUTTON, 0, nFlags, point ) )
		return;
	CExtScrollWnd::OnLButtonUp( nFlags, point );
}

void CExtRichContentScrollWnd::OnRButtonDblClk( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ( RcsGetStyle() & __EXT_RCS_EVENTS_MOUSE_WND_PROC ) != 0 && OnRcsAnalyzeMouseClickEvent( VK_RBUTTON, 2, nFlags, point ) )
		return;
	CExtScrollWnd::OnRButtonDblClk( nFlags, point );
}

void CExtRichContentScrollWnd::OnRButtonDown( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ( RcsGetStyle() & __EXT_RCS_EVENTS_MOUSE_WND_PROC ) != 0 && OnRcsAnalyzeMouseClickEvent( VK_RBUTTON, 1, nFlags, point ) )
		return;
	CExtScrollWnd::OnRButtonDown( nFlags, point );
}

void CExtRichContentScrollWnd::OnRButtonUp( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ( RcsGetStyle() & __EXT_RCS_EVENTS_MOUSE_WND_PROC ) != 0 && OnRcsAnalyzeMouseClickEvent( VK_RBUTTON, 0, nFlags, point ) )
		return;
	CExtScrollWnd::OnRButtonUp( nFlags, point );
}

bool CExtRichContentScrollWnd::OnRcsSetCursor( const POINT & ptCursorClient )
{
	ASSERT_VALID( this );
	ptCursorClient;
HCURSOR hCursor = m_hCursorDefault;
	if( hCursor == NULL )
	{
		hCursor = ::LoadCursor( NULL, IDC_ARROW );
		ASSERT( hCursor != NULL );
	}
	SetCursor( hCursor );
	return true;
}

bool CExtRichContentScrollWnd::OnRcsAnalyzeMouseMoveEvent(
	UINT nFlags, // mouse event flags
	CPoint point // mouse pointer in client coordinates
	)
{
	ASSERT_VALID( this );
	nFlags;
	if( m_bPressedTracking )
		return true;
CExtRichContentLayout * pRCL = RcsLayoutGet();
CExtRichContentItem * pRootRCI = ( pRCL->m_listHeap.GetCount() > 0 ) ? pRCL->m_listHeap.GetHead() : NULL;
	if( pRootRCI == NULL )
	{
		if( ! m_htInfoHover.IsEmpty() )
		{
			m_htInfoHover.Empty();
			RcsDelayRecalcLayout();
			OnSwRecalcLayout( true );
			OnSwUpdateScrollBars();
			OnSwInvalidate( m_bRedrawErase );
		} // if( ! m_htInfoHover.IsEmpty() )
		return true;
	} // if( pRootRCI == NULL )

CPoint ptScreen( point );
	ClientToScreen( &ptScreen );
HWND hWndFromPoint = ::WindowFromPoint( ptScreen );
CExtRichContentHitTestInfo htInfoHover( point + OnSwGetScrollPos(), *pRCL );
	if( hWndFromPoint == m_hWnd ) // do hit testing only in this case
		htInfoHover.HitTest();
//	if( m_htInfoHover == htInfoHover )
//		return true;
bool bQuickReStyleMode = ( ( RcsGetStyle() & __EXT_RCS_QUICK_RESTYLE_MODE ) ) ? true : false;
CExtRichStyleDescription::pseudo_class_def_t statesHoverOnly;
	statesHoverOnly.m_eynHover = CExtRichStyleDescription::e_ui_yn_yes;
POSITION pos1 = m_htInfoHover.m_listHtBranch.GetHeadPosition();
POSITION pos2 = htInfoHover.m_listHtBranch.GetHeadPosition();
#ifdef _DEBUG
POSITION pos1r = NULL;
#endif // _DEBUG
POSITION pos2r = NULL;
bool bHaveEqualTop = true, bHaveElementsWithResetStyles = false;
CExtRichContentItem * pResetStyleRCI;
	for( pResetStyleRCI = NULL; pos1 != NULL; )
	{
		if( bHaveEqualTop )
		{
#ifdef _DEBUG
			pos1r = pos1;
#endif // _DEBUG
			pos2r = pos2;
		}
		CExtRichContentHitTestInfo::ht_item_t & _ht_item1 = m_htInfoHover.m_listHtBranch.GetNext( pos1 );
		__EXT_DEBUG_RICH_CONTENT_ASSERT( _ht_item1.m_pRCI != NULL );
		if( pos2 != NULL && bHaveEqualTop )
		{
			CExtRichContentHitTestInfo::ht_item_t & _ht_item2 = htInfoHover.m_listHtBranch.GetNext( pos2 );
			__EXT_DEBUG_RICH_CONTENT_ASSERT( _ht_item2.m_pRCI != NULL );
			if( _ht_item1.m_pRCI == _ht_item2.m_pRCI )
				continue;
			else
				bHaveEqualTop = false;
		}
		else
			bHaveEqualTop = false;
		_ht_item1.m_pRCI->RciEvent_OnMouseLeave( m_htInfoHover, htInfoHover, m_hWnd );
		_ht_item1.m_pRCI->ResetElementStateInfo( statesHoverOnly, CExtRichStyleDescription::e_ui_yn_unspecified );
		if( _ht_item1.m_pRCI->HaveAffectedCssPseudoClasses( statesHoverOnly ) )
		{
			bHaveElementsWithResetStyles = true;
			pResetStyleRCI = _ht_item1.m_pRCI;
		}
	} // for( pResetStyleRCI = NULL; pos1 != NULL; )
	if( pResetStyleRCI != NULL )
		OnRcsResetElementStyle( pResetStyleRCI );
	if( ! bHaveEqualTop )
#ifdef _DEBUG
		pos1r =
#endif // _DEBUG
			pos2r = NULL;
	__EXT_DEBUG_RICH_CONTENT_ASSERT( ( pos1r != NULL && pos2r != NULL ) || ( pos1r == NULL && pos2r == NULL ) );
//	pos1 = ( pos1r != NULL ) ? pos1r : m_htInfoHover.m_listHtBranch.GetHeadPosition();
	pos2 = ( pos2r != NULL ) ? pos2r : htInfoHover.m_listHtBranch.GetHeadPosition();
#ifdef _DEBUG
	pos1r;
#endif // _DEBUG
	for( pResetStyleRCI = NULL; pos2 != NULL; )
	{
		CExtRichContentHitTestInfo::ht_item_t & _ht_item2 = htInfoHover.m_listHtBranch.GetNext( pos2 );
		__EXT_DEBUG_RICH_CONTENT_ASSERT( _ht_item2.m_pRCI != NULL )
		_ht_item2.m_pRCI->ResetElementStateInfo( statesHoverOnly, CExtRichStyleDescription::e_ui_yn_yes );
		if( _ht_item2.m_pRCI->HaveAffectedCssPseudoClasses( statesHoverOnly ) )
		{
			bHaveElementsWithResetStyles = true;
			pResetStyleRCI = _ht_item2.m_pRCI;
		}
		_ht_item2.m_pRCI->RciEvent_OnMouseEnter( m_htInfoHover, htInfoHover, m_hWnd );
	} // for( pResetStyleRCI = NULL; pos2 != NULL; )
	if( pResetStyleRCI != NULL )
		OnRcsResetElementStyle( pResetStyleRCI );

	m_htInfoHover = htInfoHover;

	if( htInfoHover.IsEmpty() )
	{
		if( ::GetCapture() == m_hWnd )
			::ReleaseCapture();
	}
	else
	{
		if( ::GetCapture() != m_hWnd )
			::SetCapture( m_hWnd );
	}

bool bSbInvalidate = false;
	if( m_pRCI_ScrollBarTracking != NULL )
	{
		m_pRCI_ScrollBarTracking->m_nSBMHT = INT(CExtPaintManager::__ESBMHT_NOWHERE);
		m_pRCI_ScrollBarTracking->m_bSbProcessingClick = false;
		m_pRCI_ScrollBarTracking->m_bSbProcessingOutClick = false;
		m_pRCI_ScrollBarTracking->m_bSbProcessingHover = false;
		bSbInvalidate = true;
		m_pRCI_ScrollBarTracking = NULL;
	}
	m_pHtItemScrollBarTracking = NULL;
	if(		htInfoHover.m_listHtBranch.GetCount() > 0
		&&	( ( htInfoHover.m_listHtBranch.GetTail().m_dwHtFlags & __ERCHT_ON_ANY_SCROLL_BAR ) != 0 )
		&&	( ( htInfoHover.m_listHtBranch.GetTail().m_dwHtFlags & __ERCHT_ON_SB_DISABLED ) == 0 )
		)
	{ // if scroll bar part is hovered
		const CExtRichContentHitTestInfo::ht_item_t & _ht_item = htInfoHover.m_listHtBranch.GetTail();
		ASSERT( ( _ht_item.m_dwHtFlags & (__ERCHT_ON_ANY_SCROLL_BAR|__ERCHT_ON_SB_DISABLED) ) != 0 );
		m_pHtItemScrollBarTracking = const_cast < CExtRichContentHitTestInfo::ht_item_t * > ( &_ht_item );
		m_pRCI_ScrollBarTracking = m_pHtItemScrollBarTracking->m_pRCI;
		CExtPaintManager::PAINTSCROLLBARDATA _psbd;
		m_pRCI_ScrollBarTracking->Scrolling_FillScrollBarInfo( _ht_item.m_pRCI->m_bHorzScrollBarTracking, _ht_item.m_rcPart, &_psbd );
		_psbd.AdjustHT( htInfoHover.m_ptHT );
		m_pRCI_ScrollBarTracking->m_nSBMHT = INT(_psbd.m_eSBMHT);
		m_pRCI_ScrollBarTracking->m_bSbProcessingClick = false;
		m_pRCI_ScrollBarTracking->m_bSbProcessingOutClick = false;
		m_pRCI_ScrollBarTracking->m_bSbProcessingHover = true;
		m_pRCI_ScrollBarTracking->m_bSbProcessingHorzBar = //_psbd.m_bHorzBar;
			( ( _ht_item.m_dwHtFlags & __ERCHT_SBH_SB_MASK ) != 0 ) ? true : false;
		bSbInvalidate = true;
	} // if scroll bar part is hovered
	if( bHaveElementsWithResetStyles )
		bSbInvalidate = true;
	if( bSbInvalidate )
		OnSwInvalidate( m_bRedrawErase );
	if( ! bHaveElementsWithResetStyles )
		return true;
	if( ! bQuickReStyleMode )
	{
		RcsDelayRecalcLayout();
		if( bSbInvalidate )
		{
			OnSwRecalcLayout( true );
			OnSwUpdateScrollBars();
		}
	}
	return true;
}

bool CExtRichContentScrollWnd::OnSwDoMouseWheel(
	UINT fFlags,
	short zDelta,
	CPoint point
	)
{
	ASSERT_VALID( this );
	if( m_bPressedTracking )
		return CExtNSB < CExtScrollWnd > :: OnSwDoMouseWheel( fFlags, zDelta, point );
CExtRichContentLayout * pRCL = RcsLayoutGet();
CExtRichContentItem * pRootRCI = ( pRCL->m_listHeap.GetCount() > 0 ) ? pRCL->m_listHeap.GetHead() : NULL;
	if( pRootRCI == NULL )
			return CExtNSB < CExtScrollWnd > :: OnSwDoMouseWheel( fFlags, zDelta, point );
	if( m_htInfoHover.IsEmpty() )
		return CExtNSB < CExtScrollWnd > :: OnSwDoMouseWheel( fFlags, zDelta, point );
POSITION pos = m_htInfoHover.m_listHtBranch.GetHeadPosition();
	for( ; pos != NULL; )
	{
		CExtRichContentHitTestInfo::ht_item_t & _ht_item = m_htInfoHover.m_listHtBranch.GetNext( pos );
		ASSERT( _ht_item.m_pRCI != NULL );
		if( _ht_item.m_pRCI->RciEvent_OnMouseWheel( fFlags, zDelta, point, m_htInfoHover, m_hWnd ) )
			return true;
	} // for( ; pos != NULL; )
	if( m_pRCI_focus == NULL )
		return CExtNSB < CExtScrollWnd > :: OnSwDoMouseWheel( fFlags, zDelta, point );
CExtRichContentItem * pRCI = m_pRCI_focus;
	for( ; pRCI != NULL; pRCI = pRCI->m_pParent )
	{
		if( pRCI->RciEvent_OnMouseWheel( fFlags, zDelta, point, m_htInfoHover, m_hWnd ) )
			return true;
	} // for( ; pRCI != NULL; pRCI = pRCI->m_pParent )
	return CExtNSB < CExtScrollWnd > :: OnSwDoMouseWheel( fFlags, zDelta, point );
}

void CExtRichContentScrollWnd::OnRcsAnalyzeScrollBarMouseClickEvent(
	const CExtRichContentHitTestInfo & htInfoPressed
	)
{
	ASSERT_VALID( this );
	ASSERT( htInfoPressed.m_listHtBranch.GetCount() > 0 );
const CExtRichContentHitTestInfo::ht_item_t & _ht_item = htInfoPressed.m_listHtBranch.GetTail();
	ASSERT( ( _ht_item.m_dwHtFlags & (__ERCHT_ON_ANY_SCROLL_BAR|__ERCHT_ON_SB_DISABLED) ) != 0 );
	if( ( _ht_item.m_dwHtFlags & __ERCHT_ON_SB_DISABLED ) != 0 )
		return;
	m_pHtItemScrollBarTracking = const_cast < CExtRichContentHitTestInfo::ht_item_t * > ( &_ht_item );
	m_pRCI_ScrollBarTracking = m_pHtItemScrollBarTracking->m_pRCI;
	ASSERT(
			(	( ( _ht_item.m_dwHtFlags & __ERCHT_SBH_SB_MASK ) != 0 )
			&&	( ( _ht_item.m_dwHtFlags & __ERCHT_SBV_SB_MASK ) == 0 )
			)
		||	(	( ( _ht_item.m_dwHtFlags & __ERCHT_SBH_SB_MASK ) == 0 )
			&&	( ( _ht_item.m_dwHtFlags & __ERCHT_SBV_SB_MASK ) != 0 )
			)
		);
	_ht_item.m_pRCI->m_bHorzScrollBarTracking = _ht_item.m_pRCI->m_bSbProcessingHorzBar = ( ( _ht_item.m_dwHtFlags & __ERCHT_SBH_SB_MASK ) != 0 ) ? true : false;
CExtPaintManager::PAINTSCROLLBARDATA _psbd;
	_ht_item.m_pRCI->Scrolling_FillScrollBarInfo( _ht_item.m_pRCI->m_bHorzScrollBarTracking, _ht_item.m_rcPart, &_psbd );
	_psbd.AdjustHT( htInfoPressed.m_ptHT );
//	if( ! m_bPopupInactiveLightMode )
		ActivateTopParent();
	_ht_item.m_pRCI->m_nSBMHT = INT(_psbd.m_eSBMHT);
	_ht_item.m_pRCI->m_bSbProcessingClick = _ht_item.m_pRCI->m_bSbProcessingHover = true;
	_ht_item.m_pRCI->m_bSbProcessingOutClick = false;
	_ht_item.m_pRCI->m_bSbProcessingHorzBar = _psbd.m_bHorzBar;
	OnSwInvalidate( m_bRedrawErase );
//	if( pATTW != NULL )
//		OnAdvancedPopupMenuTipWndDisplay( *pATTW, m_nSBMHT, true );
INT nScrollPosStart = _psbd.m_DSI.nPos, nScrollPos = _psbd.m_DSI.nPos;
INT m_nHelperTrackPos = _psbd.m_DSI.nPos;
bool m_bHelperHaveTrackPos = true;
CRect rcArea = _psbd.GetAreaRectHT();
const UINT nTimerID_zero_start = 401;
const UINT nTimerID_1st_slow = 402;
const UINT nTimerEllapse_1st_slow = 400;
const UINT nTimerID_2nd_fast = 403;
const UINT nTimerEllapse_2nd_fast = 100;
HWND hWndOwn = GetSafeHwnd();
	ASSERT( hWndOwn != NULL && ::IsWindow( hWndOwn ) );

bool bStopFlag = false;
CPoint ptCursor( htInfoPressed.m_ptHT );
	ptCursor += OnSwGetScrollPos();
INT nStepSize = 0L;
bool bUpStep = false;
bool bMouseButtonsNotSwapped = ( ::GetSystemMetrics( SM_SWAPBUTTON ) != 0 ) ? false : true;
	switch( _psbd.m_eSBMHT )
	{
	case CExtPaintManager::__ESBMHT_BUTTON_UP:
		bUpStep = true;
		// continue falling here ...
	case CExtPaintManager::__ESBMHT_BUTTON_DOWN:
		nStepSize = _ht_item.m_pRCI->GetScrollBarStepSize( _ht_item.m_pRCI->m_bHorzScrollBarTracking ); // nStepSize = 1L;
		break;
	case CExtPaintManager::__ESBMHT_PAGE_UP:
		bUpStep = true;
		// continue falling here ...
	case CExtPaintManager::__ESBMHT_PAGE_DOWN:
		nStepSize = (INT)_psbd.m_DSI.nPage;
		if( nStepSize <= 0L )
			nStepSize = _ht_item.m_pRCI->GetScrollBarStepSize( _ht_item.m_pRCI->m_bHorzScrollBarTracking ); // nStepSize = 1L;
		break;
	case CExtPaintManager::__ESBMHT_THUMB:
		break;
	} // switch( _psbd.m_eSBMHT )
/*
bool bMenuMode = false;
	if( CExtPopupMenuWnd::IsMenuTracking() )
	{
		CWnd * pWnd = GetParent();
		for( ; pWnd != NULL; pWnd = pWnd->GetParent() )
		{
			if( pWnd->IsKindOf( RUNTIME_CLASS(CExtPopupMenuWnd) ) )
			{
				bMenuMode = true;
				break;
			} // if( pWnd == pPopup )
			if( (pWnd->GetStyle()&WS_CHILD) == 0 )
				break;
		} // for( ; pWnd != NULL; pWnd = pWnd->GetParent() )
	} // if( CExtPopupMenuWnd::IsMenuTracking() )
*/
INT nMx = INT( _psbd.m_DSI.nMax - _psbd.m_DSI.nPage + 1 );
INT nScrollLimit =
		  _psbd.m_DSI.nMax
		- _psbd.m_DSI.nMin
		- _psbd.m_DSI.nPage
		+ 1
		;
	ASSERT( nScrollLimit >= 0 );
	if( nStepSize > nScrollLimit )
		nStepSize = nScrollLimit;
CRect rcScrollable = _psbd.m_rcBar;
	if( _psbd.m_bHorzBar )
	{
		rcScrollable.left = _psbd.m_rcButtonUp.right;
		rcScrollable.right = _psbd.m_rcButtonDown.left;
	}
	else
	{
		rcScrollable.top = _psbd.m_rcButtonUp.bottom;
		rcScrollable.bottom = _psbd.m_rcButtonDown.top;
	}
//	ScrollBar_CaptureSet(); //::SetCapture( hWndOwn );
CExtMouseCaptureSink::SetCapture( m_hWnd );
	if( nStepSize != 0L )
	{
		::PostMessage( hWndOwn, WM_TIMER, WPARAM(nTimerID_zero_start), LPARAM(0L) );
		//::SetTimer( hWndOwn, nTimerID_1st_slow, nTimerEllapse_1st_slow, NULL );
	}
	for( MSG msg; ::IsWindow( hWndOwn ) && (!bStopFlag); )
	{
		if( ! PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			if( ! ::IsWindow( hWndOwn ) )
				break;
			::WaitMessage();
			continue;
		} // if( !PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )

		bool bAnalyzeThumb = false;
		switch( msg.message )
		{
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_CANCELMODE:
		case WM_ACTIVATEAPP:
		case WM_KEYDOWN:
		case WM_KEYUP:
			bStopFlag = true;
		break;
		case WM_CAPTURECHANGED:
			if( (HWND)msg.wParam != hWndOwn )
				bStopFlag = true;
		break;
		case WM_MOUSEMOVE:
			//OnSwInvalidate( m_bRedrawErase );
			if( _ht_item.m_pRCI->m_nSBMHT == INT(CExtPaintManager::__ESBMHT_THUMB) )
			{
				if(		( ! CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ? VK_LBUTTON : VK_RBUTTON,true) )
					||	CExtPopupMenuWnd::IsKeyPressed( VK_MBUTTON )
					||	CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ?  VK_RBUTTON : VK_LBUTTON,true )
/*
					||	( (!bMenuMode) && CExtPopupMenuWnd::IsMenuTracking() )
*/
					)
				{
					bStopFlag = true;
					break;
				}
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
				bAnalyzeThumb = true;
				::GetCursorPos( &ptCursor );
				::ScreenToClient( hWndOwn, &ptCursor );
				ptCursor += OnSwGetScrollPos();
				break;
			} // if( _ht_item.m_pRCI->m_nSBMHT == INT(CExtPaintManager::__ESBMHT_THUMB) )
			if( nStepSize == 0 )
				break;
		case WM_TIMER:
			{
				if(		( ! CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ? VK_LBUTTON : VK_RBUTTON,true) )
					||	CExtPopupMenuWnd::IsKeyPressed( VK_MBUTTON )
					||	CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ? VK_RBUTTON : VK_LBUTTON,true )
///					||	( (!bMenuMode) && CExtPopupMenuWnd::IsMenuTracking() )

					)
				{
					bStopFlag = true;
					break;
				}
				if( msg.hwnd != hWndOwn )
					break;
				if(		msg.wParam != nTimerID_zero_start
					&&	msg.wParam != nTimerID_1st_slow
					&&	msg.wParam != nTimerID_2nd_fast
					)
					break;
				if( msg.wParam == nTimerID_zero_start )
					::SetTimer( hWndOwn, nTimerID_1st_slow, nTimerEllapse_1st_slow, NULL );
				else if( msg.wParam == nTimerID_1st_slow )
				{
					::KillTimer( hWndOwn, nTimerID_1st_slow );
					CExtPaintManager::stat_PassPaintMessages();
					::SetTimer( hWndOwn, nTimerID_2nd_fast, nTimerEllapse_2nd_fast, NULL );
				}
				ASSERT( nStepSize != 0L );
				PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
				::GetCursorPos( &ptCursor );
				::ScreenToClient( hWndOwn, &ptCursor );
				ptCursor += OnSwGetScrollPos();
				bool bPause = false;
				if( ! rcArea.PtInRect( ptCursor ) )
					bPause = true;
				else
				{
					if(		_ht_item.m_pRCI->m_nSBMHT == INT(CExtPaintManager::__ESBMHT_PAGE_UP)
						||	_ht_item.m_pRCI->m_nSBMHT == INT(CExtPaintManager::__ESBMHT_PAGE_DOWN)
						)
					{ // update changed area rect (page up/down cases)
						CExtPaintManager::PAINTSCROLLBARDATA _psbd2;
						_ht_item.m_pRCI->Scrolling_FillScrollBarInfo( _ht_item.m_pRCI->m_bHorzScrollBarTracking, _ht_item.m_rcPart, &_psbd2 );
						_psbd2.AdjustHT( ptCursor );
						_ht_item.m_pRCI->m_bSbProcessingHorzBar = _psbd2.m_bHorzBar;
						INT nSBMHT2 = INT( _psbd2.m_eSBMHT );
						if( nSBMHT2 != _ht_item.m_pRCI->m_nSBMHT )
							bPause = true;
						else
						{
							CRect rcArea2 = _psbd2.GetAreaRectHT();
							if( ! rcArea2.PtInRect( ptCursor ) )
								bPause = true;
							else
							{
								if( _psbd2.m_bHorzBar )
								{
									if( _ht_item.m_pRCI->m_nSBMHT == INT(CExtPaintManager::__ESBMHT_PAGE_UP) )
									{
										if( ptCursor.x >= _psbd2.m_rcThumb.left )
											bPause = true;
									}
									else if( _ht_item.m_pRCI->m_nSBMHT == INT(CExtPaintManager::__ESBMHT_PAGE_DOWN) )
									{
										if( ptCursor.x <= _psbd2.m_rcThumb.right )
											bPause = true;
									}
								}
								else
								{
									if( _ht_item.m_pRCI->m_nSBMHT == INT(CExtPaintManager::__ESBMHT_PAGE_UP) )
									{
										if( ptCursor.y >= _psbd2.m_rcThumb.top )
											bPause = true;
									}
									else if( _ht_item.m_pRCI->m_nSBMHT == INT(CExtPaintManager::__ESBMHT_PAGE_DOWN) )
									{
										if( ptCursor.y <= _psbd2.m_rcThumb.bottom )
											bPause = true;
									}
								}
							}
						}
					} // update changed area rect (page up/down cases)
				}
				if( bPause )
				{
					if( ! _ht_item.m_pRCI->m_bSbProcessingOutClick )
					{
						_ht_item.m_pRCI->m_bSbProcessingOutClick = true;
						Invalidate();
					}
/*
					if( pATTW != NULL )
						pATTW->Hide();
*/
					continue;
				}

				if( bUpStep )
				{
					nScrollPos -= nStepSize;
					if( nScrollPos < _psbd.m_DSI.nMin )
						nScrollPos = _psbd.m_DSI.nMin;
				} // if( bUpStep )
				else
				{
					nScrollPos += nStepSize;
					if( nScrollPos > nMx )
						nScrollPos = nMx;
				} // else from if( bUpStep )
				CPoint ptLSP = _ht_item.m_pRCI->m_ptScrollPos;
				LONG & nLSP = _ht_item.m_pRCI->m_bHorzScrollBarTracking ? ptLSP.x : ptLSP.y;
				if( nLSP != nScrollPos )
				{
					nLSP = nScrollPos;
					switch( _ht_item.m_pRCI->m_nSBMHT )
					{
					case (CExtPaintManager::__ESBMHT_BUTTON_UP):
					case (CExtPaintManager::__ESBMHT_BUTTON_DOWN):
					case (CExtPaintManager::__ESBMHT_PAGE_UP):
					case (CExtPaintManager::__ESBMHT_PAGE_DOWN):
					case (CExtPaintManager::__ESBMHT_THUMB):
						_ht_item.m_pRCI->_DoScroll( ptLSP, m_hWnd );
					break;
					} // switch( m_nSBMHT )
/*
					if( pATTW != NULL && ( ! bAnalyzeThumb ) )
						OnAdvancedPopupMenuTipWndDisplay( *pATTW, m_nSBMHT, true );
*/
				} // if( nLSP != nScrollPos )

CExtPaintManager::PAINTSCROLLBARDATA _psbd2;
	_ht_item.m_pRCI->Scrolling_FillScrollBarInfo( _ht_item.m_pRCI->m_bHorzScrollBarTracking, _ht_item.m_rcPart, &_psbd2 );
	////////////////////////_psbd2.m_DSI.nPos = _psbd2.m_DSI.nTrackPos = nScrollPos;
::memcpy( &_psbd.m_DSI, &_psbd2.m_DSI, sizeof(SCROLLINFO) );
nMx = INT( _psbd.m_DSI.nMax - _psbd.m_DSI.nPage + 1 );
nScrollLimit =
	  _psbd.m_DSI.nMax
	- _psbd.m_DSI.nMin
	- _psbd.m_DSI.nPage
	+ 1
	;
ASSERT( nScrollLimit >= 0 );
if( nStepSize > nScrollLimit )
	nStepSize = nScrollLimit;

				_psbd2.AdjustHT( ptCursor );
				_ht_item.m_pRCI->m_bSbProcessingHorzBar = _psbd2.m_bHorzBar;
				bool bProcessingOutClick = ( _ht_item.m_pRCI->m_nSBMHT == INT(_psbd2.m_eSBMHT) ) ? false : true;
				rcArea = _psbd2.GetAreaRect( CExtPaintManager::e_scroll_bar_mouse_hover_type_t(_ht_item.m_pRCI->m_nSBMHT) );
				if( _ht_item.m_pRCI->m_bSbProcessingOutClick != bProcessingOutClick )
				{
					_ht_item.m_pRCI->m_bSbProcessingOutClick = bProcessingOutClick;
					Invalidate();
					UpdateWindow();
				}
			}
		break;
		default:
		{
			if(		( ! CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ? VK_LBUTTON : VK_RBUTTON,true) )
				||	CExtPopupMenuWnd::IsKeyPressed( VK_MBUTTON )
				||	CExtPopupMenuWnd::IsKeyPressed( bMouseButtonsNotSwapped ? VK_RBUTTON : VK_LBUTTON,true )
/*
				||	( (!bMenuMode) && CExtPopupMenuWnd::IsMenuTracking() )
*/
				)
				bStopFlag = true;
		}
		break;
		} // switch( msg.message )
		if( ! ::IsWindow( hWndOwn ) )
			bStopFlag = true;
		if( bStopFlag || nScrollLimit == 0L )
			break;
		if( bAnalyzeThumb )
		{
			LONG nPixelOffset = _ht_item.m_pRCI->m_bHorzScrollBarTracking
				? (ptCursor.x - htInfoPressed.m_ptHT.x)
				: (ptCursor.y - htInfoPressed.m_ptHT.y);
			LONG nPixelExtent = _ht_item.m_pRCI->m_bHorzScrollBarTracking
				? (rcScrollable.Width() - _psbd.m_rcThumb.Width())
				: (rcScrollable.Height() - _psbd.m_rcThumb.Height());
			if( nPixelExtent <= 0 )
			{
				bStopFlag = true;
				break;
			} // if( nPixelExtent <= 0 )
			if( abs(nPixelOffset) > nPixelExtent )
				nPixelOffset =
					(nPixelOffset < 0)
						? (-nPixelExtent)
						: nPixelExtent;
			INT nShift =
				( nPixelExtent == 0 || nPixelOffset == 0 )
					? 0
					: ::MulDiv( nScrollLimit, abs(nPixelOffset), nPixelExtent );
			nScrollPos = nScrollPosStart;
			if( nPixelOffset < 0 )
			{
				nScrollPos -= nShift;
				if( nScrollPos < _psbd.m_DSI.nMin )
					nScrollPos = _psbd.m_DSI.nMin;
			} // if( nPixelOffset < 0 )
			else
			{
				nScrollPos += nShift;
				if( nScrollPos > nMx )
					nScrollPos = nMx;
			} // else from if( nPixelOffset < 0 )

			CPoint ptLSP = _ht_item.m_pRCI->m_ptScrollPos;
			LONG & nLSP = _ht_item.m_pRCI->m_bHorzScrollBarTracking ? ptLSP.x : ptLSP.y;
			if( nLSP != nScrollPos )
			{
				nLSP = nScrollPos;
				_ht_item.m_pRCI->_DoScroll( ptLSP, m_hWnd );
/*
				if( pATTW != NULL )
					OnAdvancedPopupMenuTipWndDisplay( *pATTW, m_nSBMHT, true );
*/
			} // if( nLSP != nScrollPos )

CExtPaintManager::PAINTSCROLLBARDATA _psbd2;
	_ht_item.m_pRCI->Scrolling_FillScrollBarInfo( _ht_item.m_pRCI->m_bHorzScrollBarTracking, _ht_item.m_rcPart, &_psbd2 );
::memcpy( &_psbd.m_DSI, &_psbd2.m_DSI, sizeof(SCROLLINFO) );
nMx = INT( _psbd.m_DSI.nMax - _psbd.m_DSI.nPage + 1 );
nScrollLimit =
	  _psbd.m_DSI.nMax
	- _psbd.m_DSI.nMin
	- _psbd.m_DSI.nPage
	+ 1
	;
ASSERT( nScrollLimit >= 0 );
if( nStepSize > nScrollLimit )
	nStepSize = nScrollLimit;

			_psbd2.AdjustHT( ptCursor );
			_ht_item.m_pRCI->m_bSbProcessingHorzBar = _psbd2.m_bHorzBar;
			rcArea = _psbd2.GetAreaRect( CExtPaintManager::__ESBMHT_THUMB );
			continue;
		} // if( bAnalyzeThumb )
/*
		if(		m_bPopupInactiveLightMode
			&&	(	msg.message == WM_TIMER
				||	(__EXT_MFC_WM_MOUSEFIRST <= msg.message && msg.message <= __EXT_MFC_WM_MOUSELAST )
				)
			)
			::PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
*/
		else
		if( ! AfxGetThread()->PumpMessage() )
			break;
	} // for( MSG msg; ::IsWindow( hWndOwn ) && (!bStopFlag); )
	if( ! ::IsWindow( hWndOwn ) )
		return;
	if( nStepSize != 0L )
	{
		::KillTimer( hWndOwn, nTimerID_1st_slow );
		::KillTimer( hWndOwn, nTimerID_2nd_fast );
	}

	if( m_pRCI_ScrollBarTracking != NULL )
	{
		CPoint ptLSP = _ht_item.m_pRCI->m_ptScrollPos;
		LONG & nLSP = m_pRCI_ScrollBarTracking->m_bHorzScrollBarTracking ? ptLSP.x : ptLSP.y;
		if( nLSP != nScrollPos )
		{
			nLSP = nScrollPos;
			_ht_item.m_pRCI->_DoScroll( ptLSP, m_hWnd );
		}
	}
	_ht_item.m_pRCI->m_nSBMHT = INT(CExtPaintManager::__ESBMHT_NOWHERE);
	_ht_item.m_pRCI->m_bSbProcessingClick
		= _ht_item.m_pRCI->m_bSbProcessingOutClick
		= _ht_item.m_pRCI->m_bSbProcessingHover
		= false;
	m_pHtItemScrollBarTracking = NULL;
	m_pRCI_ScrollBarTracking = NULL;

	Invalidate();
	UpdateWindow();
	m_nHelperTrackPos = -1;
	m_bHelperHaveTrackPos = false;
//	if( ::GetCapture() == hWndOwn )
//		::ReleaseCapture();
//	ScrollBar_CaptureRelease();
	CExtMouseCaptureSink::ReleaseCapture();
/*
	if( pATTW != NULL )
		OnAdvancedPopupMenuTipWndDisplay( *pATTW, INT(_psbd.m_eSBMHT), false );
*/

}

bool CExtRichContentScrollWnd::OnRcsAnalyzeMouseClickEvent(
	UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
	UINT nRepCnt, // 0 - button up, 1 - single click, 2 - double click, 3 - post single click & begin editing
	UINT nFlags, // mouse event flags
	CPoint point // mouse pointer in client coordinates
	)
{
	ASSERT_VALID( this );
	if( m_nMouseActivateCode == MA_ACTIVATE && ::GetFocus() != m_hWnd )
		SetFocus();
CExtRichContentLayout * pRCL = RcsLayoutGet();
CExtRichContentItem * pRootRCI = ( pRCL->m_listHeap.GetCount() > 0 ) ? pRCL->m_listHeap.GetHead() : NULL;
	if( pRootRCI == NULL )
		return true;
	if( nChar != VK_LBUTTON )
		return true;
CExtRichContentHitTestInfo htInfoPressed( point + OnSwGetScrollPos(), *pRCL );
	htInfoPressed.HitTest();
	if(		nRepCnt == 1
		&&	htInfoPressed.m_listHtBranch.GetCount() > 0
		&&	( ( htInfoPressed.m_listHtBranch.GetTail().m_dwHtFlags & (__ERCHT_ON_ANY_SCROLL_BAR|__ERCHT_ON_SB_DISABLED) ) != 0 )
		)
	{
		OnRcsAnalyzeScrollBarMouseClickEvent( htInfoPressed );
		return true;
	}
bool bQuickReStyleMode = ( ( RcsGetStyle() & __EXT_RCS_QUICK_RESTYLE_MODE ) ) ? true : false;
bool bRecalcPassed = false;
//	if( m_htInfoPressed == htInfoPressed )
//		return true;
CList < CExtRichContentHitTestInfo::ht_item_t, CExtRichContentHitTestInfo::ht_item_t & > & _listHtBranch =
		( nRepCnt == 1 ) ? htInfoPressed.m_listHtBranch : m_htInfoPressed.m_listHtBranch;
POSITION pos = _listHtBranch.GetHeadPosition();
bool bHaveElementsWithResetStyles = false;
CExtRichContentItem * pRCI_LastSupportingPressed = NULL;
CExtRichStyleDescription::pseudo_class_def_t statesFocusOnly;
	statesFocusOnly.m_eynFocus = CExtRichStyleDescription::e_ui_yn_yes;
CExtRichStyleDescription::pseudo_class_def_t statesPressedOnly;
	statesPressedOnly.m_eynPressed = CExtRichStyleDescription::e_ui_yn_yes;
CExtRichContentItem * pRCI_next_focus = NULL, * pResetStyleRCI;
	for( pResetStyleRCI = NULL; pos != NULL; )
	{
		CExtRichContentHitTestInfo::ht_item_t & _ht_item = _listHtBranch.GetNext( pos );
		__EXT_DEBUG_RICH_CONTENT_ASSERT( _ht_item.m_pRCI != NULL );
		if( nRepCnt == 0 || nRepCnt == 2 )
		{
			_ht_item.m_pRCI->RciEvent_OnPressedLeave( nChar, nRepCnt, nFlags, m_htInfoPressed, htInfoPressed, m_hWnd );
			_ht_item.m_pRCI->ResetElementStateInfo( statesPressedOnly, CExtRichStyleDescription::e_ui_yn_unspecified );
			if( _ht_item.m_pRCI->HaveAffectedCssPseudoClasses( statesPressedOnly ) )
			{
				bHaveElementsWithResetStyles = true;
				pResetStyleRCI = _ht_item.m_pRCI;
			}
		}
		else if( nRepCnt == 1 )
		{
			_ht_item.m_pRCI->RciEvent_OnPressedEnter( nChar, nRepCnt, nFlags, m_htInfoPressed, htInfoPressed, m_hWnd );
			_ht_item.m_pRCI->ResetElementStateInfo( statesPressedOnly, CExtRichStyleDescription::e_ui_yn_yes );
			if( _ht_item.m_pRCI->HaveAffectedCssPseudoClasses( statesPressedOnly ) )
			{
				bHaveElementsWithResetStyles = true;
				pResetStyleRCI = _ht_item.m_pRCI;
			}
		}
		if( _ht_item.m_pRCI->HaveAffectedCssPseudoClasses( statesPressedOnly ) )
		{
			bHaveElementsWithResetStyles = true;
			pRCI_LastSupportingPressed = _ht_item.m_pRCI;
			pResetStyleRCI = _ht_item.m_pRCI;
		}
		if( _ht_item.m_pRCI->HaveAffectedCssPseudoClasses( statesFocusOnly ) )
			pRCI_next_focus = _ht_item.m_pRCI;
	} // for( pResetStyleRCI = NULL; pos != NULL; )
	if( pResetStyleRCI != NULL )
		OnRcsResetElementStyle( pResetStyleRCI );

	if( bHaveElementsWithResetStyles )
		OnSwInvalidate( m_bRedrawErase );

bool bRecalc = false;
	if( nRepCnt == 0 || nRepCnt == 2 )
	{
		bRecalc = true;
		m_bPressedTracking = false;
		m_bProcessingCancelMode = true;
		if( ::GetCapture() == m_hWnd )
			::ReleaseCapture();
		m_bProcessingCancelMode = false;
		if( m_nVkPressed == nChar && pRCI_LastSupportingPressed != NULL && pRCI_LastSupportingPressed->CalcEffectiveRect().PtInRect( point ) )
		{
			POSITION pos = _listHtBranch.GetHeadPosition();
			for( pResetStyleRCI = NULL; pos != NULL; )
			{
				CExtRichContentHitTestInfo::ht_item_t & _ht_item = _listHtBranch.GetNext( pos );
				__EXT_DEBUG_RICH_CONTENT_ASSERT( _ht_item.m_pRCI != NULL );
				HWND hWndOwn = m_hWnd;
				if( nRepCnt == 0 )
					_ht_item.m_pRCI->RciEvent_OnClick( nChar, nFlags, m_htInfoPressed, htInfoPressed, m_hWnd );
				else if( nRepCnt == 2 )
					_ht_item.m_pRCI->RciEvent_OnDoubleClick( nChar, nFlags, m_htInfoPressed, htInfoPressed, m_hWnd );
				if( ! ::IsWindow(hWndOwn) )
					return true;
				if( _ht_item.m_pRCI->HaveAffectedCssPseudoClasses( statesFocusOnly ) )
				{
					bHaveElementsWithResetStyles = true;
					pResetStyleRCI = _ht_item.m_pRCI;
				}
			} // for( pResetStyleRCI = NULL; pos != NULL; )
			if( pResetStyleRCI != NULL )
				OnRcsResetElementStyle( pResetStyleRCI );

			if(  pRCI_next_focus != NULL && LPVOID(pRCI_next_focus) != LPVOID(m_pRCI_focus) )
			{
				bHaveElementsWithResetStyles = true;

#if (defined __EXT_RCS_WND_APPLY_HOVER_RESET_WORKAROUND__)
				// BEGIN // temporarily solution, reset hovered state to avoid incorrect inheritance from parent element inside OnRcsResetElementStates()
				if( ! m_htInfoHover.IsEmpty() )
				{
					m_htInfoHover.Empty();
					CExtRichStyleDescription::pseudo_class_def_t statesHoverOnly;
					statesHoverOnly.m_eynHover = CExtRichStyleDescription::e_ui_yn_yes;
					OnRcsResetElementStates( pRootRCI, statesHoverOnly, CExtRichStyleDescription::e_ui_yn_unspecified );
				}
				//  END  // temporarily solution, reset hovered state to avoid incorrect inheritance from parent element inside OnRcsResetElementStates()
#endif // (defined __EXT_RCS_WND_APPLY_HOVER_RESET_WORKAROUND__)

				CExtRichContentItem * pOldFocusRCI = m_pRCI_focus;
				if( m_pRCI_focus != NULL )
				{
					m_pRCI_focus->RciEvent_OnFocusLeave( m_hWnd, pRCI_next_focus );
					if( OnRcsResetElementStates( m_pRCI_focus, statesFocusOnly, CExtRichStyleDescription::e_ui_yn_unspecified ) )
						bHaveElementsWithResetStyles = true;
				}
				m_pRCI_focus = pRCI_next_focus;
				m_pRCI_focus->RciEvent_OnFocusEnter( m_hWnd, pOldFocusRCI );
				OnSwInvalidate( m_bRedrawErase );
				if( OnRcsResetElementStates( m_pRCI_focus, statesFocusOnly, CExtRichStyleDescription::e_ui_yn_yes ) )
					bHaveElementsWithResetStyles = true;
				bRecalcPassed = true;
				if( ! bQuickReStyleMode )
				{
					RcsDelayRecalcLayout();
					OnSwRecalcLayout( true );
				}
			} // if( pRCI_next_focus != NULL && LPVOID(pRCI_next_focus) != LPVOID(m_pRCI_focus) )
		} // if( m_nVkPressed == nChar && pRCI_LastSupportingPressed != NULL && pRCI_LastSupportingPressed->CalcEffectiveRect().PtInRect( point ) )
		OnRcsAnalyzeMouseMoveEvent( nFlags, point );
	} // if( nRepCnt == 0 || nRepCnt == 2 )
	else if( nRepCnt == 1 )
	{
		if( ! bHaveElementsWithResetStyles )
			return true;
		bRecalc = true;
		m_bPressedTracking = true;
		m_nVkPressed = nChar;
		m_htInfoPressed = htInfoPressed;
		if( htInfoPressed.IsEmpty() )
		{
			if( ::GetCapture() == m_hWnd )
				::ReleaseCapture();
		}
		else
		{
			if( ::GetCapture() != m_hWnd )
				::SetCapture( m_hWnd );
		}
	} // else if( nRepCnt == 1 )

	if( bRecalc )
	{
		if( ! bQuickReStyleMode )
		{
			if( ! bRecalcPassed )
			{
				RcsDelayRecalcLayout();
				OnSwRecalcLayout( true );
			}
			OnSwUpdateScrollBars();
		}
		OnSwInvalidate( m_bRedrawErase );
	}

	return true;
}

void CExtRichContentScrollWnd::OnMButtonDblClk( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( OnRcsAnalyzeMouseClickEvent( VK_MBUTTON, 2, nFlags, point ) )
		return;
	CExtScrollWnd::OnMButtonDblClk( nFlags, point );
}

void CExtRichContentScrollWnd::OnMButtonDown( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( OnRcsAnalyzeMouseClickEvent( VK_MBUTTON, 1, nFlags, point ) )
		return;
	CExtScrollWnd::OnMButtonDown( nFlags, point );
}

void CExtRichContentScrollWnd::OnMButtonUp( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( OnRcsAnalyzeMouseClickEvent( VK_MBUTTON, 0, nFlags, point ) )
		return;
	CExtScrollWnd::OnMButtonUp( nFlags, point );
}

UINT CExtRichContentScrollWnd::OnGetDlgCode()
{
	ASSERT_VALID( this );
	return DLGC_WANTARROWS|DLGC_WANTALLKEYS|DLGC_WANTCHARS|DLGC_WANTTAB;
}

void CExtRichContentScrollWnd::OnSysKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	ASSERT_VALID( this );
	if(	( RcsGetStyle() & __EXT_RCS_EVENTS_KEYBOARD_WND_PROC ) != 0 && OnRcsAnalyzeKeyEvent( true, true, nChar, nRepCnt, nFlags ) )
		return;
	CExtScrollWnd::OnSysKeyDown( nChar,  nRepCnt,  nFlags );
}

void CExtRichContentScrollWnd::OnSysKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	ASSERT_VALID( this );
	if(	( RcsGetStyle() & __EXT_RCS_EVENTS_KEYBOARD_WND_PROC ) != 0 && OnRcsAnalyzeKeyEvent( true, false, nChar, nRepCnt, nFlags ) )
		return;
	CExtScrollWnd::OnSysKeyUp( nChar,  nRepCnt,  nFlags );
}

void CExtRichContentScrollWnd::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	ASSERT_VALID( this );
	if(	( RcsGetStyle() & __EXT_RCS_EVENTS_KEYBOARD_WND_PROC ) != 0 && OnRcsAnalyzeKeyEvent( false, true, nChar, nRepCnt, nFlags ) )
		return;
	CExtScrollWnd::OnKeyDown( nChar,  nRepCnt,  nFlags );
}

void CExtRichContentScrollWnd::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	ASSERT_VALID( this );
	if(	( RcsGetStyle() & __EXT_RCS_EVENTS_KEYBOARD_WND_PROC ) != 0 && OnRcsAnalyzeKeyEvent( false, false, nChar, nRepCnt, nFlags ) )
		return;
	CExtScrollWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CExtRichContentScrollWnd::OnTimer( __EXT_MFC_UINT_PTR nIDEvent )
{
	ASSERT_VALID( this );
	CExtScrollWnd::OnTimer( nIDEvent );
}

void CExtRichContentScrollWnd::OnWindowPosChanging( WINDOWPOS FAR * lpwndpos ) 
{
	CExtScrollWnd::OnWindowPosChanging( lpwndpos );
}

void CExtRichContentScrollWnd::OnWindowPosChanged( WINDOWPOS FAR * lpwndpos )
{
	ASSERT_VALID( this );
	CExtNSB < CExtScrollWnd > :: OnWindowPosChanged( lpwndpos );
}

void CExtRichContentScrollWnd::OnShowWindow( BOOL bShow, UINT nStatus )
{
	ASSERT_VALID( this );
	CExtNSB < CExtScrollWnd > :: OnShowWindow( bShow, nStatus );
}

void CExtRichContentScrollWnd::OnSize( UINT nType, int cx, int cy )
{
	ASSERT_VALID( this );
	RcsDelayRecalcLayout();
	CExtNSB < CExtScrollWnd > :: OnSize( nType, cx, cy );
}

bool CExtRichContentScrollWnd::OnRcsAnalyzeKeyEvent(
	bool bSysKey,
	bool bKeyDownEvent, // true - key-down event, false - key-up event
	UINT nChar, // virtual key code
	UINT nRepCnt, // key-down/key-up press count
	UINT nFlags // key-down/key-up event flags
	)
{
	ASSERT_VALID( this );
	bSysKey;
	if( m_bPressedTracking )
		SendMessage( WM_CANCELMODE );
CExtRichContentLayout * pRCL = RcsLayoutGet();
CExtRichContentItem * pRootRCI = ( pRCL->m_listHeap.GetCount() > 0 ) ? pRCL->m_listHeap.GetHead() : NULL;
	if( pRootRCI == NULL )
		return true;
	if( m_pRCI_focus != NULL && m_pRCI_focus->RciEvent_OnKey( bSysKey, bKeyDownEvent, nChar, nRepCnt, nFlags, m_hWnd ) )
		return true;
bool bQuickReStyleMode = ( ( RcsGetStyle() & __EXT_RCS_QUICK_RESTYLE_MODE ) ) ? true : false;
bool bRecalcPassed = false, bHaveElementsWithResetStyles = false;
bool bAlt = ( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 ) ? true : false;
bool bCtrl = ( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 ) ? true : false;
bool bShift = ( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 ) ? true : false;
	switch( nChar )
	{
	case VK_TAB:
		if( bKeyDownEvent && (!bAlt) && (!bCtrl) && nRepCnt >= 1 )
		{
			CExtRichContentItem * pNewFocusRCI =  OnRcsFindNextTabStopElement( m_pRCI_focus, !bShift, nRepCnt );
			if( pNewFocusRCI == NULL )
				break;
			ASSERT( LPVOID(pNewFocusRCI) != LPVOID(m_pRCI_focus) );
			ASSERT( pNewFocusRCI->IsParticipantOfFocus() );
			bool bHaveElementsWithResetStyles = false;

#if (defined __EXT_RCS_WND_APPLY_HOVER_RESET_WORKAROUND__)
			// BEGIN // temporarily solution, reset hovered state to avoid incorrect inheritance from parent element inside OnRcsResetElementStates()
			if( ! m_htInfoHover.IsEmpty() )
			{
				m_htInfoHover.Empty();
				CExtRichStyleDescription::pseudo_class_def_t statesHoverOnly;
				statesHoverOnly.m_eynHover = CExtRichStyleDescription::e_ui_yn_yes;
				OnRcsResetElementStates( pRootRCI, statesHoverOnly, CExtRichStyleDescription::e_ui_yn_unspecified );
			}
			//  END  // temporarily solution, reset hovered state to avoid incorrect inheritance from parent element inside OnRcsResetElementStates()
#endif // (defined __EXT_RCS_WND_APPLY_HOVER_RESET_WORKAROUND__)

			CExtRichStyleDescription::pseudo_class_def_t statesFocusOnly;
			statesFocusOnly.m_eynFocus = CExtRichStyleDescription::e_ui_yn_yes;
			CExtRichContentItem * pOldFocusRCI = m_pRCI_focus;
			if( m_pRCI_focus != NULL )
			{
				m_pRCI_focus->RciEvent_OnFocusLeave( m_hWnd, pNewFocusRCI );
				if( OnRcsResetElementStates( m_pRCI_focus, statesFocusOnly, CExtRichStyleDescription::e_ui_yn_unspecified ) )
					bHaveElementsWithResetStyles = true;
			}
			m_pRCI_focus = pNewFocusRCI;
			bRecalcPassed = true;
			if( ! bQuickReStyleMode )
			{
				RcsDelayRecalcLayout();
				OnSwRecalcLayout( true );
			}
			m_pRCI_focus->RciEvent_OnFocusEnter( m_hWnd, pOldFocusRCI );
			OnSwInvalidate( m_bRedrawErase );
			if( OnRcsResetElementStates( m_pRCI_focus, statesFocusOnly, CExtRichStyleDescription::e_ui_yn_yes ) )
				bHaveElementsWithResetStyles = true;
			if( ! bHaveElementsWithResetStyles )
				return true;
			if( ! bQuickReStyleMode )
			{
				RcsDelayRecalcLayout();
				OnSwRecalcLayout( true );
				OnSwUpdateScrollBars();
			}
		} // if( bKeyDownEvent && (!bAlt) && (!bCtrl) && nRepCnt >= 1 )
	break; // case VK_TAB
	case VK_UP:
	case VK_DOWN:
	case VK_LEFT:
	case VK_RIGHT:
		if(		bKeyDownEvent && (!bAlt) && (!bCtrl) && (!bShift) && nRepCnt >= 1
			&&	m_pRCI_focus != NULL
			&&	m_pRCI_focus->m_pParent != NULL
			)
		{ // if can handle list style navigation keys
			const CExtRichStyleDescription & styleFocus = m_pRCI_focus->GetEffectiveStyle();
			if( styleFocus.m_eUiListNavigation == CExtRichStyleDescription::e_ui_ln_list_xy )
			{ // if the focused item uses adaptive x/y coordinate searching for the next focused element
				CRect rcFocus = m_pRCI_focus->CalcEffectiveRect();
				CPoint ptFocusCenter = rcFocus.CenterPoint();
				CExtRichContentItem * pRCI_found = NULL;
				for( UINT nStep = 1; nStep <= nRepCnt; nStep ++ )
				{ // search next focused element loop: step 1 - find closest at forward side; step 2 - find most far at backward side;
					LONG nDistanceFound2 = ( nStep == 1 ) ? LONG_MAX : 0L;
					POSITION pos = m_pRCI_focus->m_pParent->m_listItems.GetHeadPosition();
					for( ; pos != NULL; )
					{
						CExtRichContentItem * pRCI_walk = m_pRCI_focus->m_pParent->m_listItems.GetNext( pos );
						ASSERT( pRCI_walk != NULL );
						if(		LPVOID(pRCI_walk) == LPVOID(m_pRCI_focus)
							||	( ! pRCI_walk->IsParticipantOfFocus() )
							)
							continue;
						const CExtRichStyleDescription & styleWalk = pRCI_walk->GetEffectiveStyle();
						if(	!	(	INT(CExtRichStyleDescription::e_ui_ln_list_value_first) <= INT(styleWalk.m_eUiListNavigation)
								&&	INT(styleWalk.m_eUiListNavigation) <= INT(CExtRichStyleDescription::e_ui_ln_list_value_last)
								)
							)
							continue;
						CRect rcWalk = pRCI_walk->CalcEffectiveRect();
						CPoint ptWalkCenter = rcWalk.CenterPoint();
						UINT nChar2 = nChar;
						if( nStep == 2 )
						{
							switch( nChar )
							{
							case VK_UP:    nChar2 = VK_DOWN;  break;
							case VK_DOWN:  nChar2 = VK_UP;    break;
							case VK_LEFT:  nChar2 = VK_RIGHT; break;
							case VK_RIGHT: nChar2 = VK_LEFT;  break;
							} // switch( nChar )
						}
						switch( nChar2 )
						{
						case VK_UP:
							if( ! ( rcWalk.left <= ptFocusCenter.x && ptFocusCenter.x <= rcWalk.right && ptWalkCenter.y < ptFocusCenter.y ) )
								continue;
						break;
						case VK_DOWN:
							if( ! ( rcWalk.left <= ptFocusCenter.x && ptFocusCenter.x <= rcWalk.right && ptWalkCenter.y > ptFocusCenter.y ) )
								continue;
						break;
						case VK_LEFT:
							if( ! ( rcWalk.top <= ptFocusCenter.y && ptFocusCenter.y <= rcWalk.bottom && ptWalkCenter.x < ptFocusCenter.x ) )
								continue;
						break;
						case VK_RIGHT:
							if( ! ( rcWalk.top <= ptFocusCenter.y && ptFocusCenter.y <= rcWalk.bottom && ptWalkCenter.x > ptFocusCenter.x ) )
								continue;
						break;
						} // switch( nChar2 )
						LONG nDistanceWalkX = ptFocusCenter.x - ptWalkCenter.x, nDistanceWalkY = ptFocusCenter.y - ptWalkCenter.y;
						LONG nDistanceWalkX2 = nDistanceWalkX * nDistanceWalkX, nDistanceWalkY2 = nDistanceWalkY * nDistanceWalkY;
						LONG nDistanceWalk2 = nDistanceWalkX2 + nDistanceWalkY2;
						if( pRCI_found != NULL )
						{
							switch( nStep )
							{
							case 1:
								if( nDistanceFound2 < nDistanceWalk2 )
									continue;
							break;
							case 2:
								if( nDistanceFound2 > nDistanceWalk2 )
									continue;
							break;
							} // switch( nStep )
						} // if( pRCI_found != NULL )
						pRCI_found = pRCI_walk;
						nDistanceFound2 = nDistanceWalk2;
					} // for( ; pos != NULL; )
					if( pRCI_found != NULL )
						break;
				} // search next focused element loop: step 1 - find closest at forward side; step 2 - find most far at backward side;
				if( pRCI_found != NULL )
				{

#if (defined __EXT_RCS_WND_APPLY_HOVER_RESET_WORKAROUND__)
					// BEGIN // temporarily solution, reset hovered state to avoid incorrect inheritance from parent element inside OnRcsResetElementStates()
					if( ! m_htInfoHover.IsEmpty() )
					{
						m_htInfoHover.Empty();
						CExtRichStyleDescription::pseudo_class_def_t statesHoverOnly;
						statesHoverOnly.m_eynHover = CExtRichStyleDescription::e_ui_yn_yes;
						OnRcsResetElementStates( pRootRCI, statesHoverOnly, CExtRichStyleDescription::e_ui_yn_unspecified );
					}
					//  END  // temporarily solution, reset hovered state to avoid incorrect inheritance from parent element inside OnRcsResetElementStates()
#endif // (defined __EXT_RCS_WND_APPLY_HOVER_RESET_WORKAROUND__)

					CExtRichStyleDescription::pseudo_class_def_t statesFocusOnly;
					statesFocusOnly.m_eynFocus = CExtRichStyleDescription::e_ui_yn_yes;
					CExtRichContentItem * pOldFocusRCI = m_pRCI_focus;
					m_pRCI_focus->RciEvent_OnFocusLeave( m_hWnd, pRCI_found );
					if( OnRcsResetElementStates( m_pRCI_focus, statesFocusOnly, CExtRichStyleDescription::e_ui_yn_unspecified ) )
						bHaveElementsWithResetStyles = true;
					m_pRCI_focus = pRCI_found;
					m_pRCI_focus->RciEvent_OnFocusEnter( m_hWnd, pOldFocusRCI );
					OnSwInvalidate( m_bRedrawErase );
					if( OnRcsResetElementStates( m_pRCI_focus, statesFocusOnly, CExtRichStyleDescription::e_ui_yn_yes ) )
						bHaveElementsWithResetStyles = true;
					bRecalcPassed = true;
					if( ! bQuickReStyleMode )
					{
						RcsDelayRecalcLayout();
					//	OnSwRecalcLayout( true );
						OnSwUpdateScrollBars();
					}
					return true;
				} // if( pRCI_found != NULL )
			} // if the focused item uses adaptive x/y coordinate searching for the next focused element
			else
			if(		( styleFocus.m_eUiListNavigation == CExtRichStyleDescription::e_ui_ln_list_x && ( nChar == VK_LEFT || nChar == VK_RIGHT ) )
				||	( styleFocus.m_eUiListNavigation == CExtRichStyleDescription::e_ui_ln_list_y && ( nChar == VK_UP   || nChar == VK_DOWN  ) )
				)
			{ // if the focused item is the x or y list style navigation item supporting focus changing on left/right/up/down arrow keys
				CExtRichContentItem * pRCI_walk = NULL;
				if( nChar == VK_DOWN || nChar == VK_RIGHT )
					pRCI_walk =
						( m_pRCI_focus->m_pNext != NULL )
							?	m_pRCI_focus->m_pNext
							:	(		( m_pRCI_focus->m_pParent != NULL && m_pRCI_focus->m_pParent->m_listItems.GetCount() > 0 )
												? m_pRCI_focus->m_pParent->m_listItems.GetHead()
												: NULL
								)
							;
				else
					pRCI_walk =
						( m_pRCI_focus->m_pPrev != NULL )
							?	m_pRCI_focus->m_pPrev
							:	(		( m_pRCI_focus->m_pParent != NULL && m_pRCI_focus->m_pParent->m_listItems.GetCount() > 0 )
												? m_pRCI_focus->m_pParent->m_listItems.GetTail()
												: NULL
								)
							;
				bool bSameFound = false;
				for( ; pRCI_walk != NULL; pRCI_walk = ( nChar == VK_DOWN || nChar == VK_RIGHT ) ? pRCI_walk->m_pNext : pRCI_walk->m_pPrev )
				{
					if( LPVOID(pRCI_walk) == LPVOID(m_pRCI_focus) )
					{
						bSameFound = true;
						pRCI_walk = NULL;
						break;
					}
					if( ! pRCI_walk->IsParticipantOfFocus() )
						continue;
					const CExtRichStyleDescription & styleWalk = pRCI_walk->GetEffectiveStyle();
					if(		INT(CExtRichStyleDescription::e_ui_ln_list_value_first) <= INT(styleWalk.m_eUiListNavigation)
						&&	INT(styleWalk.m_eUiListNavigation) <= INT(CExtRichStyleDescription::e_ui_ln_list_value_last)
						)
						break;
				}
				if( (! bSameFound ) && pRCI_walk == NULL )
				{
					if( nChar == VK_DOWN || nChar == VK_RIGHT )
						pRCI_walk =
							( m_pRCI_focus->m_pParent != NULL && m_pRCI_focus->m_pParent->m_listItems.GetCount() > 0 )
								? m_pRCI_focus->m_pParent->m_listItems.GetHead()
								: NULL
								;
					else
						pRCI_walk =
							( m_pRCI_focus->m_pParent != NULL && m_pRCI_focus->m_pParent->m_listItems.GetCount() > 0 )
								? m_pRCI_focus->m_pParent->m_listItems.GetTail()
								: NULL
								;
					for( ; pRCI_walk != NULL; pRCI_walk = ( nChar == VK_DOWN || nChar == VK_RIGHT ) ? pRCI_walk->m_pNext : pRCI_walk->m_pPrev )
					{
						if( pRCI_walk == m_pRCI_focus )
						{
							bSameFound = true;
							pRCI_walk = NULL;
							break;
						}
						if( ! pRCI_walk->IsParticipantOfFocus() )
							continue;
						const CExtRichStyleDescription & styleWalk = pRCI_walk->GetEffectiveStyle();
						if(		INT(CExtRichStyleDescription::e_ui_ln_list_value_first) <= INT(styleWalk.m_eUiListNavigation)
							&&	INT(styleWalk.m_eUiListNavigation) <= INT(CExtRichStyleDescription::e_ui_ln_list_value_last)
							)
							break;
					}
				}
				if( pRCI_walk != NULL )
				{

#if (defined __EXT_RCS_WND_APPLY_HOVER_RESET_WORKAROUND__)
					// BEGIN // temporarily solution, reset hovered state to avoid incorrect inheritance from parent element inside OnRcsResetElementStates()
					if( ! m_htInfoHover.IsEmpty() )
					{
						m_htInfoHover.Empty();
						CExtRichStyleDescription::pseudo_class_def_t statesHoverOnly;
						statesHoverOnly.m_eynHover = CExtRichStyleDescription::e_ui_yn_yes;
						OnRcsResetElementStates( pRootRCI, statesHoverOnly, CExtRichStyleDescription::e_ui_yn_unspecified );
					}
					//  END  // temporarily solution, reset hovered state to avoid incorrect inheritance from parent element inside OnRcsResetElementStates()
#endif // (defined __EXT_RCS_WND_APPLY_HOVER_RESET_WORKAROUND__)

					CExtRichStyleDescription::pseudo_class_def_t statesFocusOnly;
					statesFocusOnly.m_eynFocus = CExtRichStyleDescription::e_ui_yn_yes;
					CExtRichContentItem * pOldFocusRCI = m_pRCI_focus;
					m_pRCI_focus->RciEvent_OnFocusLeave( m_hWnd, pRCI_walk );
					if( OnRcsResetElementStates( m_pRCI_focus, statesFocusOnly, CExtRichStyleDescription::e_ui_yn_unspecified ) )
						bHaveElementsWithResetStyles = true;
					m_pRCI_focus = pRCI_walk;
					m_pRCI_focus->RciEvent_OnFocusEnter( m_hWnd, pOldFocusRCI );
					OnSwInvalidate( m_bRedrawErase );
					if( OnRcsResetElementStates( m_pRCI_focus, statesFocusOnly, CExtRichStyleDescription::e_ui_yn_yes ) )
						bHaveElementsWithResetStyles = true;
					bRecalcPassed = true;
					if( ! bQuickReStyleMode )
					{
						RcsDelayRecalcLayout();
					//	OnSwRecalcLayout( true );
						OnSwUpdateScrollBars();
					}
					return true;
				} // if( pRCI_walk != NULL )
			} // if the focused item is the x or y list style navigation item supporting focus changing on left/right/up/down arrow keys
		} // if can handle list style navigation keys
		break; // cases VK_UP, VK_DOWN
	} // switch( nChar )
	bHaveElementsWithResetStyles;
	return true;
}

CExtRichContentItem * CExtRichContentScrollWnd::OnRcsFindNextTabStopElement(
	CExtRichContentItem * pStartSearchRCI,
	bool bNext,
	UINT nRepCnt // VK_TAB pressing count
	)
{
	ASSERT_VALID( this );
	if( pStartSearchRCI == NULL )
		return NULL;
CExtRichContentLayout * pRCL = RcsLayoutGet();
CExtRichContentItem * pRootRCI = ( pRCL->m_listHeap.GetCount() > 0 ) ? pRCL->m_listHeap.GetHead() : NULL;
	if( pRootRCI == NULL )
		return NULL;
	ASSERT( LPVOID(pRCL) == PVOID(&(pStartSearchRCI->m_layout)) );
CExtRichContentItem * pRCI_start = ( pStartSearchRCI != NULL ) ? pStartSearchRCI : ( bNext ? pRootRCI->BrowseNext() : pRootRCI->BrowsePrev() );
	if( pRCI_start == NULL )
		return NULL;
CExtRichContentItem * pRCI_walk = pRCI_start;
	for( INT nRepIndex = 0; nRepIndex < INT(nRepCnt); nRepIndex ++ )
	{
		bool bFoundParticipantOfFocus = false;
		for( ; true; )
		{
			pRCI_walk = bNext ? pRCI_walk->BrowseNext() : pRCI_walk->BrowsePrev();
			if( pRCI_walk == NULL )
			{
				pRCI_walk = bNext ? pRootRCI->BrowseNext() : pRootRCI->BrowsePrev();
				if( pRCI_walk == NULL )
					break;
			}
			if( pRCI_walk->IsParticipantOfFocus() )
			{
				bFoundParticipantOfFocus = true;
				break;
			}
			if( pRCI_walk == pRCI_start )
				break;
		} // for( ; true; )
		if( ! bFoundParticipantOfFocus )
			return NULL;
	} // for( INT nRepIndex = 0; nRepIndex < INT(nRepCnt); nRepIndex ++ )
	if( pRCI_walk == NULL || pRCI_walk == pRCI_start || pRCI_walk == pStartSearchRCI )
		return NULL;
	return pRCI_walk;
}

bool CExtRichContentScrollWnd::OnRcsResetElementStyle(
	CExtRichContentItem * pRCI,
	bool bDeep // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( pRCI != NULL );
	if( pRCI == NULL )
		return false;
CExtRichContentLayout * pRCL = RcsLayoutGet();
	ASSERT( LPVOID(pRCL) == PVOID(&(pRCI->m_layout)) );
	pRCI->ResetStyleInfo( bDeep );
bool bQuickReStyleMode = ( ( RcsGetStyle() & __EXT_RCS_QUICK_RESTYLE_MODE ) ) ? true : false;
	if( ! bQuickReStyleMode )
		return true;
static CExtRichStyleDescription g_styleEmpty;
CExtRichContentItem * pParentRCI = pRCI->m_pParent;
CExtRichStyleDescription & styleWalk = ( pParentRCI != NULL ) ? pParentRCI->GetEffectiveStyle() : g_styleEmpty;
CClientDC dc( this );
CExtRichContentItem::_StyleRestore_t _restore( dc, styleWalk, INT(pRCL->m_eLO), pRCI->m_thUnderline );
CFont * pOldFont = NULL;
	if( pParentRCI == NULL )
	{
		HFONT hFontRCS = OnRcsGetFont();
		CFont * pFont = ( hFontRCS != NULL ) ? CFont::FromHandle( hFontRCS ) : GetFont();
		pOldFont = dc.SelectObject( pFont );
	}
	pRCI->ReStyle( dc, styleWalk, true, bDeep );
	if( pParentRCI == NULL )
		dc.SelectObject( pOldFont );
	return true;
}

bool CExtRichContentScrollWnd::OnRcsResetElementStates(
	CExtRichContentItem * pRCI,
	const CExtRichStyleDescription::pseudo_class_def_t & statesToReset,
	CExtRichStyleDescription::e_ui_yes_not_option_t eynSetOn,
	bool bDeep // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( pRCI != NULL );
	if( pRCI == NULL )
		return false;
#if (defined _DEBUG)
CExtRichContentLayout * pRCL = RcsLayoutGet();
	ASSERT( LPVOID(pRCL) == PVOID(&(pRCI->m_layout)) );
	pRCL;
#endif // (defined _DEBUG)
	pRCI->ResetElementStateInfo( statesToReset, eynSetOn, bDeep );
	if( ! pRCI->HaveAffectedCssPseudoClasses( statesToReset ) )
	{
		bool bSelectionAffectedByFocus = false;
		if( statesToReset.m_eynFocus != CExtRichStyleDescription::e_ui_yn_unspecified )
		{
			const CExtRichStyleDescription & styleEffective = pRCI->GetEffectiveStyle();
			if(		styleEffective.m_eynFocusOnMakeSelected    == CExtRichStyleDescription::e_ui_yn_yes
				||	styleEffective.m_eynFocusOffMakeUnSelected == CExtRichStyleDescription::e_ui_yn_yes
				)
				bSelectionAffectedByFocus = true;
		}
		if( ! bSelectionAffectedByFocus )
			return false;
	}
	return OnRcsResetElementStyle( pRCI, bDeep );
}

CExtRichStyleDescription::e_ui_sb_type_t CExtRichContentScrollWnd::RciEvent_Scrolling_GetDefaultSBT( const CExtRichContentItem * pRCI ) const
{
	ASSERT_VALID( this );
	ASSERT( pRCI != NULL );
	pRCI;
	if( m_bDisplayPopOverScrollBars )
	{
		if( m_bLightPopOverScrollBars )
			return CExtRichStyleDescription::e_ui_sbt_pop_over_light;
		else
			return CExtRichStyleDescription::e_ui_sbt_pop_over_full;
	}
	return CExtRichStyleDescription::e_ui_sbt_classic;
}

bool CExtRichContentScrollWnd::RciEvent_ResetElementStateInfo(
	CExtRichContentItem * pRCI,
	HWND hWndContentViewer,
	const CExtRichStyleDescription::pseudo_class_def_t & statesToReset,
	CExtRichStyleDescription::e_ui_yes_not_option_t eynSetOn,
	bool bDeep // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( pRCI != NULL );
	ASSERT( hWndContentViewer == m_hWnd && ::IsWindow(hWndContentViewer) );
	hWndContentViewer;
	return OnRcsResetElementStates( pRCI, statesToReset, eynSetOn, bDeep );
}

#if (!defined __EXT_MFC_NO_RICHCONTENTGENERATORWND )

/////////////////////////////////////////////////////////////////////////////
// CExtRichGenTreeData

CExtRichGenTreeData::CExtRichGenTreeData()
	: m_pRoot( NULL )
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	, m_pKeyTipChain( NULL )
	, m_bKeyTipsDisplayed( false )
	, m_bHelperKeyTipsSupported( false )
	, m_bHelperUserStartKeyTipMode( true )
#endif // from (!defined __EXT_MFC_NO_CUSTOMIZE)
{
}

CExtRichGenTreeData::~CExtRichGenTreeData() { OnRcgtdHandleShutdown(); }

void CExtRichGenTreeData::OnRcgtdHandleShutdown()
{
	if( m_pRoot != NULL )
	{
		VERIFY( RcgtdItemRemove( *m_pRoot ) );
		ASSERT( m_mapItems.GetCount() == 0L );
		ASSERT( LPVOID(m_pRoot) == NULL );
	}
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( m_pKeyTipChain != NULL )
	{
		delete m_pKeyTipChain;
		m_pKeyTipChain = NULL;
	}
#endif // from (!defined __EXT_MFC_NO_CUSTOMIZE)
}

HTREEITEM CExtRichGenTreeData::RcgtdItemGetNext( HTREEITEM hti, INT nNextItemType ) const
{
	if( hti == NULL  )
	{
		if( nNextItemType != TVGN_ROOT )
			return NULL;
	}
	else
		{ ASSERT( RcgtdItemIsPresent( hti ) ); }
hti_t * pHTI = CExtRichGenTreeData::hti_t::FromHTREEITEM( hti );
	switch( nNextItemType )
	{
	case TVGN_PARENT:		return (*pHTI->m_pParent);
	case TVGN_FIRSTCHILD:	return (*pHTI->m_pFirstChild);
	case TVGN_LASTCHILD:	return (*pHTI->m_pLastChild);
	case TVGN_NEXT:			return (*pHTI->m_pNext);
	case TVGN_PREVIOUS:		return (*pHTI->m_pPrev);
	case TVGN_ROOT:
		if( m_pRoot == NULL )
		{
			try
			{
				m_pRoot = new hti_t;
			}
			catch( CException * pException )
			{
				ASSERT( FALSE );
				pException->Delete();
				return NULL;
			}
			catch( ... )
			{
				ASSERT( FALSE );
				return NULL;
			}
			m_mapItems.SetAt( m_pRoot, NULL );
			( const_cast < CExtRichGenTreeData * > ( this ) ) -> OnRcgtdItemAllocated( *m_pRoot );
			if( ! ( const_cast < CExtRichGenTreeData * > ( this ) ) -> OnRcgtdItemInserted( *m_pRoot ) )
			{
				VERIFY( ( const_cast < CExtRichGenTreeData * > ( this ) ) -> RcgtdItemRemove( *m_pRoot ) );
				ASSERT( m_mapItems.GetCount() == 0L );
				ASSERT( LPVOID(m_pRoot) == NULL );
				return NULL;
			}
		}
		return (*m_pRoot);
	} /// switch( nNextItemType )
	ASSERT( FALSE );
	return NULL;
}

bool CExtRichGenTreeData::RcgtdItemIsPresent( HTREEITEM hti ) const
{
	if( hti == NULL )
		return false;
hti_t * pHTI = CExtRichGenTreeData::hti_t::FromHTREEITEM( hti );
LPVOID ptr = NULL;
	if( ! m_mapItems.Lookup( pHTI, ptr ) )
		return false;
	return true;
}

HTREEITEM CExtRichGenTreeData::RcgtdItemInsert( HTREEITEM htiParent, HTREEITEM htiInsertBefore, LPVOID pItemData )
{
	if( htiParent == NULL )
		htiParent = RcgtdItemGetRoot();
	else
		{ ASSERT( RcgtdItemIsPresent( htiParent ) ); }
hti_t * pHtiParent = CExtRichGenTreeData::hti_t::FromHTREEITEM( htiParent );
hti_t * pHtiPrevSibling = NULL, * pHtiNextSibling = NULL;
	if( htiInsertBefore != NULL )
	{
		ASSERT( RcgtdItemIsPresent( htiInsertBefore ) );
		pHtiNextSibling = CExtRichGenTreeData::hti_t::FromHTREEITEM( htiInsertBefore );
		ASSERT( LPVOID(pHtiNextSibling->m_pParent) == LPVOID(pHtiParent) );
		pHtiPrevSibling = pHtiNextSibling->m_pPrev;
	}
	else
		pHtiPrevSibling = CExtRichGenTreeData::hti_t::FromHTREEITEM( RcgtdItemGetLastChild( htiParent ) );
hti_t * pHTI = NULL;
	try
	{
		pHTI = new hti_t;
	}
	catch( CException * pException )
	{
		ASSERT( FALSE );
		pException->Delete();
		return false;
	}
	catch( ... )
	{
		ASSERT( FALSE );
		return false;
	}
	pHTI->m_pItemData = pItemData;
	m_mapItems.SetAt( pHTI, NULL );
	OnRcgtdItemAllocated( *pHTI );
	pHTI->m_pParent = pHtiParent;	
	pHtiParent->m_nChildCount ++;
	pHTI->m_pNext = pHtiNextSibling;
	pHTI->m_pPrev = pHtiPrevSibling;
	if( pHTI->m_pNext != NULL )
		pHTI->m_pNext->m_pPrev = pHTI;
	if( pHTI->m_pPrev != NULL )
		pHTI->m_pPrev->m_pNext = pHTI;
	if( LPVOID(pHtiNextSibling) == LPVOID(pHTI->m_pParent->m_pFirstChild) || LPVOID(pHTI->m_pParent->m_pFirstChild) == NULL )
	{
		ASSERT( LPVOID(pHtiPrevSibling) == NULL );
		pHTI->m_pParent->m_pFirstChild = pHTI;
	}
	if( LPVOID(pHtiPrevSibling) == LPVOID(pHTI->m_pParent->m_pLastChild) || LPVOID(pHTI->m_pParent->m_pLastChild) == NULL )
	{
		ASSERT( LPVOID(pHtiNextSibling) == NULL );
		pHTI->m_pParent->m_pLastChild = pHTI;
	}
	if( ! OnRcgtdItemInserted( *pHTI ) )
	{
		VERIFY( RcgtdItemRemove( *pHTI ) );
		return NULL;
	}
	return (*pHTI);
}

bool CExtRichGenTreeData::RcgtdItemRemove( HTREEITEM hti )
{
	if( hti == NULL )
		return false;
	ASSERT( RcgtdItemIsPresent( hti ) );
	RcgtdItemRemoveAllChildren( hti );
hti_t * pHTI = CExtRichGenTreeData::hti_t::FromHTREEITEM( hti );
	ASSERT( pHTI->m_pFirstChild == NULL );
	ASSERT( pHTI->m_pLastChild == NULL );
	ASSERT( pHTI->m_nChildCount == 0L );
	if( ! OnRcgtdItemRemoving( hti ) )
		return false;
	if( pHTI->m_pItemData != NULL )
	{
		LPVOID pItemDataToRemove = pHTI->m_pItemData;
		pHTI->m_pItemData = NULL;
		OnRcgtdRemoveItemData( hti, pItemDataToRemove );
	}
	if( pHTI->m_pPrev != NULL )
		pHTI->m_pPrev->m_pNext = pHTI->m_pNext;
	if( pHTI->m_pNext != NULL )
		pHTI->m_pNext->m_pPrev = pHTI->m_pPrev;
	if( pHTI->m_pParent != NULL )
	{
		if( LPVOID(pHTI->m_pParent->m_pFirstChild) == LPVOID(pHTI) )
			pHTI->m_pParent->m_pFirstChild = pHTI->m_pNext;
		if( LPVOID(pHTI->m_pParent->m_pLastChild) == LPVOID(pHTI) )
			pHTI->m_pParent->m_pLastChild = pHTI->m_pPrev;
		ASSERT( pHTI->m_pParent->m_nChildCount > 0 );
		pHTI->m_pParent->m_nChildCount --;
	}
	VERIFY( m_mapItems.RemoveKey( pHTI ) );
	if( LPVOID(pHTI) == LPVOID(m_pRoot) )
		m_pRoot = NULL;
	delete pHTI;
	OnRcgtdItemRemoved( hti );
	return true;
}

ULONG CExtRichGenTreeData::RcgtdItemRemoveAllChildren( HTREEITEM hti )
{
	if( hti == NULL )
		hti = RcgtdItemGetRoot();
	else
		{ ASSERT( RcgtdItemIsPresent( hti ) ); }
ULONG nRemoved = 0;
	for( HTREEITEM hti2 = RcgtdItemGetFirstChild( hti ); hti2 != NULL; nRemoved ++, hti2 = RcgtdItemGetFirstChild( hti ) )
		{ VERIFY( RcgtdItemRemove( hti2 ) ); }
	return nRemoved;
}

ULONG CExtRichGenTreeData::RcgtdItemGetChildCount( HTREEITEM hti ) const
{
	if( hti == NULL )
		return false;
	ASSERT( RcgtdItemIsPresent( hti ) );
hti_t * pHTI = CExtRichGenTreeData::hti_t::FromHTREEITEM( hti );
	return pHTI->m_nChildCount;
}

HTREEITEM CExtRichGenTreeData::RcgtdItemGetChildAt( HTREEITEM hti, ULONG nIndex ) const
{
	if( hti == NULL )
		return false;
	ASSERT( RcgtdItemIsPresent( hti ) );
	for( hti = RcgtdItemGetFirstChild( hti ); nIndex != 0 && hti != NULL; nIndex --, hti = RcgtdItemGetNextSibling( hti ) );
	return hti;
}

LPVOID CExtRichGenTreeData::RcgtdItemDataGet( HTREEITEM hti ) const
{
	if( hti == NULL )
		return NULL;
	ASSERT( RcgtdItemIsPresent( hti ) );
hti_t * pHTI = CExtRichGenTreeData::hti_t::FromHTREEITEM( hti );
	return pHTI->m_pItemData;
}

bool CExtRichGenTreeData::RcgtdItemDataSet( HTREEITEM hti, LPVOID pItemData )
{
	if( hti == NULL )
		return false;
	ASSERT( RcgtdItemIsPresent( hti ) );
hti_t * pHTI = CExtRichGenTreeData::hti_t::FromHTREEITEM( hti );
	if( pHTI->m_pItemData != NULL )
	{
		LPVOID pItemDataToRemove = pHTI->m_pItemData;
		pHTI->m_pItemData = NULL;
		OnRcgtdRemoveItemData( hti, pItemDataToRemove );
	}
	pHTI->m_pItemData = pItemData;
	return true;
}

ULONG CExtRichGenTreeData::RcgtdItemIndentGet( HTREEITEM hti ) const
{
	if( hti == NULL )
		return false;
	ASSERT( RcgtdItemIsPresent( hti ) );
hti_t * pHTI = CExtRichGenTreeData::hti_t::FromHTREEITEM( hti );
ULONG nItemIndent = 0;
	for( ; pHTI->m_pParent != NULL; pHTI = pHTI->m_pParent )
		nItemIndent ++;
	return nItemIndent;
}

void CExtRichGenTreeData::OnRcgtdItemAllocated( HTREEITEM hti )
	{ ASSERT( hti != NULL ); ASSERT( RcgtdItemIsPresent( hti ) ); hti; }
bool CExtRichGenTreeData::OnRcgtdItemInserted( HTREEITEM hti )
	{ ASSERT( hti != NULL ); ASSERT( RcgtdItemIsPresent( hti ) ); hti; return true; }
bool CExtRichGenTreeData::OnRcgtdItemRemoving( HTREEITEM hti )
	{ ASSERT( hti != NULL ); ASSERT( RcgtdItemIsPresent( hti ) ); hti; return true; }
void CExtRichGenTreeData::OnRcgtdItemRemoved( HTREEITEM hti )
	{ ASSERT( hti != NULL ); ASSERT( ! RcgtdItemIsPresent( hti ) ); hti; }
void CExtRichGenTreeData::OnRcgtdRemoveItemData( HTREEITEM hti, LPVOID pItemDataToRemove )
	{ ASSERT( hti != NULL ); ASSERT( RcgtdItemIsPresent( hti ) ); ASSERT( pItemDataToRemove != NULL ); hti; pItemDataToRemove; }
bool CExtRichGenTreeData::OnRcgtdItemInvoke( HTREEITEM hti, CExtRichGenTreeData::e_item_invoke_type eIIT )
	{ ASSERT( hti != NULL ); ASSERT( RcgtdItemIsPresent( hti ) ); hti; eIIT; return false; }

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

bool CExtRichGenTreeData::RcgtdKeyTipsDisplayedGet() const
	{ return m_bKeyTipsDisplayed; }

void CExtRichGenTreeData::RcgtdKeyTipsDisplayedSet(
	bool bKeyTipsDisplayed // = true
	)
{
	if( ! m_bHelperKeyTipsSupported )
		m_bKeyTipsDisplayed = false;
	else
		m_bKeyTipsDisplayed = bKeyTipsDisplayed;
}

CExtCustomizeCmdKeyTip & CExtRichGenTreeData::RcgtdKeyTipChainGet()
{
	if( m_pKeyTipChain == NULL )
		m_pKeyTipChain = new CExtCustomizeCmdKeyTip;
	return (*m_pKeyTipChain);
}

const CExtCustomizeCmdKeyTip & CExtRichGenTreeData::RcgtdKeyTipChainGet() const
	{ return ( const_cast < CExtRichGenTreeData * > ( this ) ) -> RcgtdKeyTipChainGet(); }

void CExtRichGenTreeData::RcgtdKeyTipChainSet(
	const CExtCustomizeCmdKeyTip & keyTipChain
	)
{
CExtCustomizeCmdKeyTip & own_keyTipChain = RcgtdKeyTipChainGet();
	own_keyTipChain = keyTipChain;
}

void CExtRichGenTreeData::RcgtdKeyTipChainEmpty()
	{ RcgtdKeyTipChainGet().KeyCodeRemoveAll(); }

HTREEITEM CExtRichGenTreeData::OnRcgtdKeyTipTranslate(
	DWORD dwKeyCode,
	CExtCustomizeCmdKeyTip * pKeyTipChain,
	bool bAutoInvokeAction,
	bool & bActionInvoked,
	bool & bContinueKeyTipMode
	)
{
	bActionInvoked = false;
	for( HTREEITEM hti = RcgtdItemBrowseNext( RcgtdItemGetRoot() ); hti != NULL; hti = RcgtdItemBrowseNext( hti ) )
	{
		if(	OnRcgtdKeyTipTranslateItem(
				hti,
				dwKeyCode,
				pKeyTipChain,
				bAutoInvokeAction,
				bActionInvoked,
				bContinueKeyTipMode
				)
			)
			return hti;
	}
	return NULL;
}

CExtPopupKeyTipWnd * CExtRichGenTreeData::hti_t::_KeyTipWndGet(
	bool bEnsureInstantiated // = false
	)
{
	if( m_pWndKeyTip == NULL && bEnsureInstantiated )
	{
		try
		{
			m_pWndKeyTip = new CExtPopupKeyTipWnd;
			ASSERT_VALID( m_pWndKeyTip );
		}
		catch( CException * pException )
		{
			pException->Delete();
			return NULL;
		}
	} // if( m_pWndKeyTip == NULL && bEnsureInstantiated )
#ifdef _DEBUG
	if( m_pWndKeyTip != NULL )
	{
		ASSERT_VALID( m_pWndKeyTip );
	}
#endif // _DEBUG
	return m_pWndKeyTip;
}

void CExtRichGenTreeData::hti_t::_KeyTipWndDestroy()
{
	if( m_pWndKeyTip != NULL )
	{
		ASSERT_VALID( m_pWndKeyTip );
		m_pWndKeyTip->Hide();
		delete m_pWndKeyTip;
		m_pWndKeyTip = NULL;
	} // if( m_pWndKeyTip != NULL )
}

CExtPopupKeyTipWnd * CExtRichGenTreeData::OnRcgtdKeyTipGetWnd(
	HTREEITEM hti,
	bool bEnsureInstantiated // = false
	)
{
	if( hti == NULL )
		return NULL;
	ASSERT( RcgtdItemIsPresent( hti ) );
hti_t * pHTI = CExtRichGenTreeData::hti_t::FromHTREEITEM( hti );
	return pHTI->_KeyTipWndGet( bEnsureInstantiated );
}

CExtSafeString CExtRichGenTreeData::OnRcgtdKeyTipGetText(
	HTREEITEM hti,
	INT nIndent
	)
{
CExtSafeString strKeyTipText( _T("") );
CExtCustomizeCmdKeyTip * pCmdKeyTip = RcgtdItemKeyTipGet( hti );
	if( pCmdKeyTip != NULL )
		strKeyTipText = pCmdKeyTip->GetTipText( nIndent );
	return strKeyTipText;
}

void CExtRichGenTreeData::OnRcgtdKeyTipWndDestroy( HTREEITEM hti )
{
	if( hti == NULL )
		return;
	ASSERT( RcgtdItemIsPresent( hti ) );
hti_t * pHTI = CExtRichGenTreeData::hti_t::FromHTREEITEM( hti );
	pHTI->_KeyTipWndDestroy();
}

void CExtRichGenTreeData::OnRcgtdKeyTipTrackingQuery(
	HTREEITEM hti,
	bool bShow,
	CExtCustomizeCmdKeyTip * pKeyTipChain,
	HDWP & hPassiveModeDWP
	)
{
	if( bShow )
	{
		ASSERT_VALID( pKeyTipChain );
/*
		if(		( ! IsVisible() )
			||	( GetStyle() & TBBS_HIDDEN ) != 0
			||	pToolBar->GetSafeHwnd() == NULL
			||	( ! pToolBar->IsVisible() )
			)
		{
			OnRcgtdKeyTipTrackingQuery( false, NULL, hPassiveModeDWP );
			return;
		}
*/
		INT nIndent = pKeyTipChain->KeyCodeGetCount();
		CExtSafeString strKeyTipText = OnRcgtdKeyTipGetText( hti, nIndent );
		if( strKeyTipText.IsEmpty() )
		{
			OnRcgtdKeyTipTrackingQuery( hti, false, NULL, hPassiveModeDWP );
			return;
		}
		CExtPopupKeyTipWnd * pWndKeyTip = OnRcgtdKeyTipGetWnd( hti, true );
		if( pWndKeyTip == NULL )
			return;
		CWnd * pWndKeyTipParent = OnRcgtdKeyTipGetParentWnd();
		ASSERT_VALID( pWndKeyTipParent );
		ASSERT( pWndKeyTipParent->GetSafeHwnd() != NULL );
		CPoint ptKeyTipGetGuideLines = OnRcgtdKeyTipGetGuideLines( hti );
		if( ptKeyTipGetGuideLines.x == __EXT_RICH_CONTENT_NEGATIVE_UNUSED || ptKeyTipGetGuideLines.y == __EXT_RICH_CONTENT_NEGATIVE_UNUSED )
			return;
		if( ! pWndKeyTip->Create(
				pWndKeyTipParent,
				ptKeyTipGetGuideLines,
				strKeyTipText,
				true, /* IsEnabled(), */
				hPassiveModeDWP
				)
			)
		{
			OnRcgtdKeyTipTrackingQuery( hti, false, NULL, hPassiveModeDWP );
			return;
		}
		if( hPassiveModeDWP == NULL )
			pWndKeyTip->ShowWindow( SW_SHOWNOACTIVATE );
	} // if( bShow )
/*
	else
		OnRcgtdKeyTipWndDestroy( hti );
*/
}

void CExtRichGenTreeData::OnRcgtdKeyTipTrackingQueryNested(
	HTREEITEM htiStart,
	bool bShow,
	CExtCustomizeCmdKeyTip * pKeyTipChain,
	bool bApplyToThisItem,
	HDWP & hPassiveModeDWP
	)
{
	if( bApplyToThisItem )
		OnRcgtdKeyTipTrackingQuery( htiStart, bShow, pKeyTipChain, hPassiveModeDWP );
	for( HTREEITEM hti = RcgtdItemGetFirstChild( htiStart ); hti != NULL; hti = RcgtdItemGetNextSibling( hti ) )
		OnRcgtdKeyTipTrackingQueryNested( hti, bShow, pKeyTipChain, true, hPassiveModeDWP );
}

bool CExtRichGenTreeData::OnRcgtdKeyTipTranslateItem(
	HTREEITEM hti,
	DWORD dwKeyCode,
	CExtCustomizeCmdKeyTip * pKeyTipChain,
	bool bAutoInvokeAction,
	bool & bActionInvoked,
	bool & bContinueKeyTipMode
	)
{
	ASSERT( hti != NULL );
	ASSERT( RcgtdItemIsPresent( hti ) );
	bActionInvoked = false;
/*
	if( ! IsVisible() )
		return false;
	if( ( GetStyle() & TBBS_HIDDEN ) != 0 )
		return false;
	if( IsDisabled() )
		return false;
*/
CExtCustomizeCmdKeyTip * pCmdKeyTip = RcgtdItemKeyTipGet( hti );
	if( pCmdKeyTip == NULL )
		return false;
CExtPopupKeyTipWnd * pWndKeyTip = OnRcgtdKeyTipGetWnd( hti );
	if(		pWndKeyTip->GetSafeHwnd() == NULL
		||	( pWndKeyTip->GetStyle() & WS_VISIBLE ) == 0
		)
		return false;
	if( ! pCmdKeyTip->CompareWidthChain( *pKeyTipChain ) )
		return false;
INT nKeyCodeCountOwn = pCmdKeyTip->KeyCodeGetCount();
INT nKeyCodeCountChain = pKeyTipChain->KeyCodeGetCount();
	ASSERT( nKeyCodeCountOwn > nKeyCodeCountChain );
DWORD dwKeyCodeOwn = pCmdKeyTip->KeyCodeGetAt( nKeyCodeCountChain );
	if( dwKeyCodeOwn != dwKeyCode )
		return false;
	pKeyTipChain->KeyCodeAdd( dwKeyCode );
	nKeyCodeCountChain++;
	if( nKeyCodeCountOwn > nKeyCodeCountChain )
		return true;
	if( bAutoInvokeAction )
		bActionInvoked = OnRcgtdKeyTipInvokeAction( hti, bContinueKeyTipMode );
	return true;
}

void CExtRichGenTreeData::OnRcgtdKeyTipDisplay(
	HTREEITEM hti,
	CExtCustomizeCmdKeyTip & keyTipChain
	)
{
	ASSERT( hti != NULL );
	ASSERT( RcgtdItemIsPresent( hti ) );
/*
	if( ! IsVisible() )
		return;
	if( ( GetStyle() & TBBS_HIDDEN ) != 0 )
		return;
//	if( IsDisabled() )
//		return;
*/
CExtCustomizeCmdKeyTip * pCmdKeyTip = RcgtdItemKeyTipGet( hti );
	if( pCmdKeyTip == NULL )
		return;
CExtPopupKeyTipWnd * pWndKeyTip = OnRcgtdKeyTipGetWnd( hti );
	if(		pWndKeyTip->GetSafeHwnd() == NULL
		||	( pWndKeyTip->GetStyle() & WS_VISIBLE ) == 0
		)
		return;
	if( pCmdKeyTip->CompareWidthChain( keyTipChain ) )
		return;
	pWndKeyTip->ShowWindow( SW_HIDE );
}

bool CExtRichGenTreeData::OnRcgtdKeyTipInvokeAction(
	HTREEITEM hti,
	bool & bContinueKeyTipMode
	)
{
	ASSERT( hti != NULL );
	ASSERT( RcgtdItemIsPresent( hti ) );
	hti;
	bContinueKeyTipMode = false;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	OnRcgtdKeyTipTrackingStop();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return true;
}

void CExtRichGenTreeData::RcgtdKeyTipDisplay(
	CExtCustomizeCmdKeyTip & keyTipChain
	)
{
	for( HTREEITEM hti = RcgtdItemGetRoot(); hti != NULL; hti = RcgtdItemBrowseNext( hti ) )
		OnRcgtdKeyTipDisplay( hti, keyTipChain );
}

void CExtRichGenTreeData::OnRcgtdKeyTipTrackingStart()
{
MSG _msg;
	if(		::PeekMessage( &_msg, NULL, WM_ACTIVATEAPP, WM_ACTIVATEAPP, PM_NOREMOVE )
		&&	_msg.wParam == 0
		)
		return;
CWnd * pWndKeyTipParent = OnRcgtdKeyTipGetParentWnd();
	if( pWndKeyTipParent->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( pWndKeyTipParent );
	if( ! CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND( pWndKeyTipParent->m_hWnd ) )
		return;
	RcgtdKeyTipsDisplayedSet( true );
	RcgtdKeyTipChainEmpty();
	CExtPaintManager::stat_PassPaintMessages();
HDWP hPassiveModeDWP = ::BeginDeferWindowPos( 64 );
	OnRcgtdKeyTipTrackingQueryNested( RcgtdItemGetRoot(), true, &(RcgtdKeyTipChainGet()), false, hPassiveModeDWP );
	if( hPassiveModeDWP != NULL )
	{
		::EndDeferWindowPos( hPassiveModeDWP );
		CExtPaintManager::stat_PassPaintMessages();
	}
	CExtPopupMenuTipWnd::UpdateDelayedLayeredBehaviorAll();
	m_bHelperUserStartKeyTipMode = true;
}

void CExtRichGenTreeData::hti_t::_KeyTipTrackingStop()
{
	_KeyTipWndDestroy();
}

void CExtRichGenTreeData::OnRcgtdKeyTipTrackingStop( HTREEITEM hti )
{
	ASSERT( hti != NULL );
	ASSERT( RcgtdItemIsPresent( hti ) );
hti_t * pHTI = CExtRichGenTreeData::hti_t::FromHTREEITEM( hti );
	pHTI->_KeyTipTrackingStop();
}

void CExtRichGenTreeData::OnRcgtdKeyTipTrackingStop()
{
	if( ! RcgtdKeyTipsDisplayedGet() )
		return;
	for( HTREEITEM hti = RcgtdItemGetRoot(); hti != NULL; hti = RcgtdItemBrowseNext( hti ) )
		OnRcgtdKeyTipTrackingStop( hti );
	RcgtdKeyTipsDisplayedSet( false );
	RcgtdKeyTipChainEmpty();
	m_bHelperUserStartKeyTipMode = true;
}

void CExtRichGenTreeData::hti_t::_KeyTipDestroy()
{
CExtCustomizeCmdKeyTip * pCmdKeyTip = _KeyTipDetach();
	if( pCmdKeyTip != NULL )
		delete pCmdKeyTip;
}

void CExtRichGenTreeData::hti_t::_KeyTipSet( CExtCustomizeCmdKeyTip * pCmdKeyTip, bool bClone )
{
CExtCustomizeCmdKeyTip * pCurrentCCS = _KeyTipGet();
	if( pCurrentCCS == pCmdKeyTip )
		return;
	ASSERT( pCmdKeyTip != NULL );
	_KeyTipDestroy();
	m_pCmdKeyTip = bClone ? pCmdKeyTip->Clone() : pCmdKeyTip;
}

CExtCustomizeCmdKeyTip * CExtRichGenTreeData::RcgtdItemKeyTipGet( HTREEITEM hti ) const
{
	if( hti == NULL )
		return false;
	ASSERT( RcgtdItemIsPresent( hti ) );
hti_t * pHTI = CExtRichGenTreeData::hti_t::FromHTREEITEM( hti );
	return pHTI->_KeyTipGet();
}

CExtCustomizeCmdKeyTip * CExtRichGenTreeData::RcgtdItemKeyTipDetach( HTREEITEM hti )
{
	if( hti == NULL )
		return false;
	ASSERT( RcgtdItemIsPresent( hti ) );
hti_t * pHTI = CExtRichGenTreeData::hti_t::FromHTREEITEM( hti );
	return pHTI->_KeyTipDetach();
}

void CExtRichGenTreeData::RcgtdItemKeyTipDestroy( HTREEITEM hti )
{
	if( hti == NULL )
		return;
	ASSERT( RcgtdItemIsPresent( hti ) );
hti_t * pHTI = CExtRichGenTreeData::hti_t::FromHTREEITEM( hti );
	pHTI->_KeyTipDestroy();
}

void CExtRichGenTreeData::RcgtdItemKeyTipSet( HTREEITEM hti, CExtCustomizeCmdKeyTip * pCmdKeyTip, bool bClone )
{
	if( hti == NULL )
		return;
	ASSERT( RcgtdItemIsPresent( hti ) );
hti_t * pHTI = CExtRichGenTreeData::hti_t::FromHTREEITEM( hti );
	pHTI->_KeyTipSet( pCmdKeyTip, bClone );
}

void CExtRichGenTreeData::RcgtdItemKeyTipAttach( HTREEITEM hti, CExtCustomizeCmdKeyTip * pCmdKeyTip )
{
	if( hti == NULL )
		return;
	ASSERT( RcgtdItemIsPresent( hti ) );
hti_t * pHTI = CExtRichGenTreeData::hti_t::FromHTREEITEM( hti );
	pHTI->_KeyTipAttach( pCmdKeyTip );
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

BOOL CExtRichGenTreeData::PreTranslateMessage( MSG * pMsg )
{
	pMsg;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
UINT nMsg = pMsg->message;
	if(	   nMsg == WM_SYSKEYDOWN
		|| nMsg == WM_SYSKEYUP
		|| nMsg == WM_KEYDOWN
		|| nMsg == WM_KEYUP
		)
	{ // if key down/up messages
		BOOL bKeyUp = ( nMsg == WM_KEYUP || nMsg == WM_SYSKEYUP) ? TRUE : FALSE;
		BOOL bCtrl = ::GetKeyState(VK_CONTROL) & 0x80000000;
		BOOL bShift = ::GetKeyState(VK_SHIFT) & 0x80000000;
		BOOL bAlt = HIWORD(pMsg->lParam) & KF_ALTDOWN;
		//BOOL bExtended = HIWORD(pMsg->lParam) & KF_EXTENDED;
		//bool bRightAltGr = ( (::GetAsyncKeyState(VK_RMENU)&0x8000) != 0 ) ? true : false;
		TCHAR vkTCHAR = (TCHAR)pMsg->wParam;
		if( (!bCtrl) && (!bShift) && ( ( vkTCHAR != VK_MENU && (!bAlt) ) || vkTCHAR == VK_MENU ) )
		{
			if( RcgtdKeyTipsDisplayedGet() )
			{
				switch( vkTCHAR )
				{
				case VK_MENU:
					if( ! bKeyUp )
						return FALSE;
					if( RcgtdKeyTipsDisplayedGet() )
					{
						OnRcgtdKeyTipTrackingStop();
						return TRUE;
					}
				break; // case VK_MENU
				case VK_ESCAPE:
				case VK_RETURN:
					if( RcgtdKeyTipsDisplayedGet() )
					{
						OnRcgtdKeyTipTrackingStop();
						return TRUE;
					}
				break; // cases VK_ESCAPE, VK_RETURN
				default:
					if( m_bHelperKeyTipsSupported )
					{
						if(		( _T('A') <= vkTCHAR && vkTCHAR <= _T('Z') )
							||	( _T('0') <= vkTCHAR && vkTCHAR <= _T('9') )
							)
						{ // if alpha/numbers
							if( bKeyUp )
								return TRUE;
							bool bActionInvoked = false;
							bool bContinueKeyTipMode = false;
							CExtCustomizeCmdKeyTip & keyTipChain = RcgtdKeyTipChainGet();
							HWND hWndSafe = pMsg->hwnd;
							HTREEITEM htiTranslateResult =
								OnRcgtdKeyTipTranslate(
									(DWORD)pMsg->wParam,
									&keyTipChain,
									true,
									bActionInvoked,
									bContinueKeyTipMode
									);
							if( hWndSafe != NULL && (! ::IsWindow( hWndSafe ) ) )
								return TRUE;
							if( htiTranslateResult != NULL )
							{
								if( bActionInvoked && ( ! bContinueKeyTipMode ) )
									OnRcgtdKeyTipTrackingStop();
								else
									RcgtdKeyTipDisplay( keyTipChain );
							}
							return TRUE;
						} // if alpha/numbers
					} // if( m_bHelperKeyTipsSupported )
				break; // default case
				} // switch( vkTCHAR )
			} // if( RcgtdKeyTipsDisplayedGet() )
			else
			{
				switch( vkTCHAR )
				{
				case VK_MENU:
					if( m_bHelperKeyTipsSupported )
					{
						if( ! bKeyUp )
							return FALSE;
						OnRcgtdKeyTipTrackingStart();
						return TRUE;
					} // if( m_bHelperKeyTipsSupported )
				break; // case VK_MENU
				} // switch( vkTCHAR )
			} // else from if( RcgtdKeyTipsDisplayedGet() )
		} // if( (!bCtrl) && (!bShift) && ( ( vkTCHAR != VK_MENU && (!bAlt) ) || vkTCHAR == VK_MENU ) )
	} // if key down/up messages
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CExtRichGenItemData

CExtRichGenItemData::ItemWriter_t::ItemWriter_t(
	HTREEITEM hti,
	CExtRichGenItemData * pItemData,
	CExtSafeString & strContent,
	__EXT_MFC_SAFE_LPCTSTR strTagName,
	bool bAutoWriteHeaderMode, // = true
	bool bAutoWriteFooterMode // = true
	)
	: m_hti( hti )
	, m_pItemData( pItemData )
	, m_strContent( strContent )
	, m_strTagName( strTagName )
	, m_bAutoWriteHeaderMode( bAutoWriteHeaderMode )
	, m_bAutoWriteFooterMode( bAutoWriteFooterMode )
	, m_bWrittenHeader( false )
	, m_bWrittenFooter( false )
{
	ASSERT( m_hti != NULL );
	if( m_bAutoWriteHeaderMode )
		WriteHeader();
}

CExtRichGenItemData::ItemWriter_t::~ItemWriter_t()
{
	if( m_bAutoWriteFooterMode )
		WriteFooter();
}

void CExtRichGenItemData::ItemWriter_t::WriteHeader()
{
	if( m_bWrittenHeader )
		return;
	m_bWrittenHeader = true;
	if( m_pItemData != NULL )
	{
		CExtSafeString str = m_pItemData->ParmGet( __RCGI_PARM_OUTER_CONTENT_BEFORE );
		if( ! str.IsEmpty() )
		{
			Write( _T(" ") );
			Write( LPCTSTR(str) );
			Write( _T(" ") );
		}
	}
	Write( _T("<") );
	Write( LPCTSTR(m_strTagName) );
	if( m_pItemData != NULL )
	{
		CExtSafeString str = m_pItemData->ParmGet( __RCGI_PARM_TAG_PARAMETERS );
		if( ! str.IsEmpty() )
		{
			Write( _T(" ") );
			Write( LPCTSTR(str) );
			Write( _T(" ") );
		}
	}

static const TCHAR g_strNameAndIdTrimChars[] = _T(" \"\'\\\r\n\t");
	m_strTagAttr_Name.TrimLeft( g_strNameAndIdTrimChars );
	m_strTagAttr_Name.TrimRight( g_strNameAndIdTrimChars );
	m_strTagAttr_ID.TrimLeft( g_strNameAndIdTrimChars );
	m_strTagAttr_ID.TrimRight( g_strNameAndIdTrimChars );
	m_strTagAttr_BindingSrcID.TrimLeft( g_strNameAndIdTrimChars );
	m_strTagAttr_BindingSrcID.TrimRight( g_strNameAndIdTrimChars );
	m_strTagAttr_BindingDstID.TrimLeft( g_strNameAndIdTrimChars );
	m_strTagAttr_BindingDstID.TrimRight( g_strNameAndIdTrimChars );
	if( ( ! m_strTagAttr_Name.IsEmpty() ) && m_strTagAttr_Name.FindOneOf( g_strNameAndIdTrimChars ) < 0 )
		Format( _T(" name=\"%s\" "), LPCTSTR(m_strTagAttr_Name) );
	if( ( ! m_strTagAttr_ID.IsEmpty() ) && m_strTagAttr_ID.FindOneOf( g_strNameAndIdTrimChars ) < 0 )
		Format( _T(" id=\"%s\" "), LPCTSTR(m_strTagAttr_ID) );
	else if( m_pItemData != NULL )
		Format( _T(" id=\"%p\" "), LPVOID(m_hti) );
	if( ! m_strTagAttr_BindingSrcID.IsEmpty() )
		Format( _T(" uiidsrc=\"%s\" "), LPVOID(LPCTSTR(m_strTagAttr_BindingSrcID)) );
	else
		Format( _T(" uiidsrc=\"%p\" "), LPVOID(m_hti) );
	if( ! m_strTagAttr_BindingDstID.IsEmpty() )
		Format( _T(" uiiddst=\"%s\" "), LPVOID(LPCTSTR(m_strTagAttr_BindingDstID)) );

	Write( _T(">") );
	if( m_pItemData != NULL )
	{
		CExtSafeString str = m_pItemData->ParmGet( __RCGI_PARM_INNER_CONTENT_BEFORE );
		if( ! str.IsEmpty() )
		{
			Write( _T(" ") );
			Write( LPCTSTR(str) );
			Write( _T(" ") );
		}
		if( ! m_pItemData->m_strHtmlInner.IsEmpty() )
			Write( LPCTSTR(m_pItemData->m_strHtmlInner) );
	}
}

void CExtRichGenItemData::ItemWriter_t::WriteFooter()
{
	if( m_bWrittenFooter )
		return;
	m_bWrittenFooter = true;
	if( m_pItemData != NULL )
	{
		CExtSafeString str = m_pItemData->ParmGet( __RCGI_PARM_INNER_CONTENT_AFTER );
		if( ! str.IsEmpty() )
		{
			Write( _T(" ") );
			Write( LPCTSTR(str) );
			Write( _T(" ") );
		}
	}
	Write( _T("</") );
	Write( LPCTSTR(m_strTagName) );
	Write( _T(">\r\n") );
	if( m_pItemData != NULL )
	{
		CExtSafeString str = m_pItemData->ParmGet( __RCGI_PARM_OUTER_CONTENT_AFTER );
		if( ! str.IsEmpty() )
		{
			Write( _T(" ") );
			Write( LPCTSTR(str) );
			Write( _T(" ") );
		}
	}
}

void CExtRichGenItemData::ItemWriter_t::Write( __EXT_MFC_SAFE_LPCTSTR strContent )
	{ if( LPCTSTR(strContent) != LPCTSTR(NULL) ) m_strContent += LPCTSTR(strContent); }

void CExtRichGenItemData::ItemWriter_t::Format( __EXT_MFC_SAFE_LPCTSTR strFmt, ... )
{
	va_list a_va_list;
	va_start( a_va_list, LPCTSTR(strFmt) );
	_FormatImpl( LPCTSTR(strFmt), a_va_list );
	va_end( a_va_list );
}

void CExtRichGenItemData::ItemWriter_t::_FormatImpl( __EXT_MFC_SAFE_LPCTSTR strFmt, va_list a_va_list )
{
static const INT nBufferSize = 1024*10;
CString strBuffer;
LPTSTR pBuffer = strBuffer.GetBuffer( nBufferSize + 1 );
	if( pBuffer == NULL )
	{
		ASSERT( FALSE );
		return;
	}
	::memset( LPVOID(pBuffer), 0, sizeof(TCHAR) * ( nBufferSize + 1 ) );
#if _MFC_VER >= 0x800
	_vsntprintf_s( pBuffer, nBufferSize, nBufferSize, LPCTSTR(strFmt), a_va_list );
#else
	_vsntprintf( pBuffer, nBufferSize, LPCTSTR(strFmt), a_va_list );
#endif
	strBuffer.ReleaseBuffer();
	Write( LPCTSTR(strBuffer) );
}

CExtRichGenItemData::CExtRichGenItemData()
	: m_nItemType( __RCGIT_UNDEFINED )
	, m_nCmdID( 0 )
	{ }
CExtRichGenItemData::~CExtRichGenItemData()
	{ ParmRemoveAll(); }

void CExtRichGenItemData::ParmSet( __EXT_MFC_SAFE_LPCTSTR strName, __EXT_MFC_SAFE_LPCTSTR strValue )
{
	if( LPCTSTR(strName) == NULL || (*LPCTSTR(strName)) == _T('\0') )
		return;
	m_mapParms.SetAt( LPCTSTR(strName), ( LPCTSTR(strValue) != NULL ) ? LPCTSTR(strValue) : _T("") );
}

void CExtRichGenItemData::ParmAppend( __EXT_MFC_SAFE_LPCTSTR strName, __EXT_MFC_SAFE_LPCTSTR strValueToAppend )
{
	if( LPCTSTR(strValueToAppend) == NULL || _tcslen( LPCTSTR(strValueToAppend) ) == 0 )
		return;
CExtSafeString strValue = ParmGet( strName );
	strValue += LPCTSTR(strValueToAppend);
	ParmSet( LPCTSTR(strName), LPCTSTR(strValue) );
}

CExtSafeString CExtRichGenItemData::ParmGet( __EXT_MFC_SAFE_LPCTSTR strName ) const
{
CExtSafeString strValue;
	if( ! m_mapParms.Lookup( LPCTSTR(strName), strValue ) )
		strValue.Empty();
	return strValue;
}

bool CExtRichGenItemData::ParmIsPresent( __EXT_MFC_SAFE_LPCTSTR strName ) const
{
CExtSafeString strValue;
	if( ! m_mapParms.Lookup( LPCTSTR(strName), strValue ) )
		return false;
	return true;
}

bool CExtRichGenItemData::ParmRemove( __EXT_MFC_SAFE_LPCTSTR strName )
	{ return m_mapParms.RemoveKey( LPCTSTR(strName) ) ? true : false; }

void CExtRichGenItemData::ParmRemoveAll()
	{ m_mapParms.RemoveAll(); }

/////////////////////////////////////////////////////////////////////////////
// CExtRichGenItemCmdUI
// command UI update class

CExtRichGenItemCmdUI::CExtRichGenItemCmdUI(
	HTREEITEM htiUpdate,
	CExtRichGenWnd * pRCGW
	)
	: m_htiUpdate( htiUpdate )
	, m_pItemData( NULL )
	, m_pRCGW( pRCGW )
{
	ASSERT_VALID( m_pRCGW );
	ASSERT( m_htiUpdate != NULL );
	m_pItemData = (CExtRichGenItemData *) m_pRCGW->RcgtdItemDataGet( htiUpdate );
	ASSERT( m_pItemData != NULL );
	ASSERT( CExtCmdManager::IsCommand( m_pItemData->m_nCmdID ) );
	m_nID = m_pItemData->m_nCmdID;
}

void CExtRichGenItemCmdUI::Enable(
	BOOL bOn // = TRUE
	)
{
	ASSERT_VALID( m_pRCGW );
	ASSERT( m_htiUpdate != NULL );
	ASSERT( m_pItemData != NULL );
	ASSERT( CExtCmdManager::IsCommand( m_pItemData->m_nCmdID ) );
	m_bEnableChanged = m_pRCGW->OnRcgwUpdateItemCmdUI_Enable( *this, bOn ? true : false ) ? TRUE : FALSE;
}

void CExtRichGenItemCmdUI::SetCheck(
	int nCheck // = 1 // 0, 1 or 2 (indeterminate)
	)
{
	ASSERT_VALID( m_pRCGW );
	ASSERT( m_htiUpdate != NULL );
	ASSERT( m_pItemData != NULL );
	ASSERT( CExtCmdManager::IsCommand( m_pItemData->m_nCmdID ) );
	m_pRCGW->OnRcgwUpdateItemCmdUI_SetCheck( *this, nCheck );
}

void CExtRichGenItemCmdUI::SetRadio(
	BOOL bOn // = TRUE
	)
{
	ASSERT_VALID( m_pRCGW );
	ASSERT( m_htiUpdate != NULL );
	ASSERT( m_pItemData != NULL );
	ASSERT( CExtCmdManager::IsCommand( m_pItemData->m_nCmdID ) );
	m_pRCGW->OnRcgwUpdateItemCmdUI_SetRadio( *this, bOn ? true : false );
}

void CExtRichGenItemCmdUI::SetText(
	__EXT_MFC_SAFE_LPCTSTR lpszText
	)
{
	ASSERT_VALID( m_pRCGW );
	ASSERT( m_htiUpdate != NULL );
	ASSERT( m_pItemData != NULL );
	ASSERT( CExtCmdManager::IsCommand( m_pItemData->m_nCmdID ) );
	m_pRCGW->OnRcgwUpdateItemCmdUI_SetText( *this, lpszText );
}

/////////////////////////////////////////////////////////////////////////////
// CExtRichGenWnd window

IMPLEMENT_DYNCREATE( CExtRichGenWnd, CExtRichContentScrollWnd );

CExtRichGenWnd::CExtRichGenWnd()
	: m_helperHookedHWND( NULL )
{
}

CExtRichGenWnd::~CExtRichGenWnd()
{
	RcgwTreeItemFocusDelay( NULL, false );
	RcgwTreeItemSelectionDelay( NULL, false );
	RcgwTreeItemEnableDelay( NULL, false, false );
	OnRcgtdHandleShutdown();
}

BEGIN_MESSAGE_MAP( CExtRichGenWnd, CExtRichContentScrollWnd )
	//{{AFX_MSG_MAP(CExtRichGenWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CExtRichGenWnd::PreTranslateMessage( MSG * pMsg )
{
	if( CExtRichGenTreeData::PreTranslateMessage( pMsg ) )
		return TRUE;
	return CExtRichContentScrollWnd::PreTranslateMessage( pMsg );
}

void CExtRichGenWnd::PreSubclassWindow()
{
	CExtRichContentScrollWnd::PreSubclassWindow();
	if( m_helperHookedHWND != NULL && ::IsWindow( m_helperHookedHWND ) )
		SetupHookWndSink( m_helperHookedHWND );
	else
	{
		m_helperHookedHWND = NULL;
		for( HWND hWndLookupPopup = ::GetParent( m_hWnd ); hWndLookupPopup != NULL; hWndLookupPopup = ::GetParent( hWndLookupPopup ) )
		{
			__EXT_MFC_LONG_PTR dwWndStyle = ::__EXT_MFC_GetWindowLong( hWndLookupPopup, GWL_STYLE );
			if( ( dwWndStyle & WS_CHILD ) != 0 )
				continue;
			m_helperHookedHWND = hWndLookupPopup;
			SetupHookWndSink( m_helperHookedHWND );
			break;
		} // for( HWND hWndLookupPopup = ::GetParent( m_hWnd ); hWndLookupPopup != NULL; hWndLookupPopup = ::GetParent( hWndLookupPopup ) )
	}
}

void CExtRichGenWnd::PostNcDestroy()
{
	m_helperHookedHWND = NULL;
	RemoveAllWndHooks();
	CExtRichContentScrollWnd::PostNcDestroy();
}

bool CExtRichGenWnd::RciEvent_OnSelect(
	CExtRichContentItem * pRCI,
	HWND hWndContentViewer,
	bool bSelected
	)
{
	ASSERT_VALID( this );
	ASSERT( pRCI != NULL );
	ASSERT( hWndContentViewer == m_hWnd && ::IsWindow(hWndContentViewer) );
	hWndContentViewer;
//#if (!defined __EXT_MFC_NO_CUSTOMIZE)
//	OnRcgtdKeyTipTrackingStop();
//#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return CExtRichContentScrollWnd::RciEvent_OnSelect( pRCI, hWndContentViewer, bSelected );
}

bool CExtRichGenWnd::RciEvent_OnPressedEnter(
	CExtRichContentItem * pRCI,
	UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
	UINT nRepCnt, // 0 - button up, 1 - single click, 2 - double click, 3 - post single click & begin editing
	UINT nFlags, // mouse event flags
	const CExtRichContentHitTestInfo & htInfoOld,
	const CExtRichContentHitTestInfo & htInfoNew,
	HWND hWndContentViewer
	)
{
	__EXT_DEBUG_RICH_CONTENT_ASSERT( pRCI != NULL );
	__EXT_DEBUG_RICH_CONTENT_ASSERT( hWndContentViewer != NULL && ::IsWindow(hWndContentViewer) );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	OnRcgtdKeyTipTrackingStop();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtSafeString strID = pRCI->ElementUiBindingSrcIdGet();
	if( ! strID.IsEmpty() )
	{
		HTREEITEM hti = NULL;
		int nFieldsScanned =
#if _MFC_VER >= 0x800
			_stscanf_s( LPCTSTR(strID), _T("%p"), &hti );
#else
			_stscanf( LPCTSTR(strID), _T("%p"), &hti );
#endif
		ASSERT( nFieldsScanned == 1 );
		nFieldsScanned;
		CExtRichGenTreeData::e_item_invoke_type eIIT = __E_RCTD_IIT_MOUSE_LBUTTON_DOWN;
		if( nChar == VK_MBUTTON )
			eIIT = __E_RCTD_IIT_MOUSE_MBUTTON_DOWN;
		else if( nChar == VK_RBUTTON )
			eIIT = __E_RCTD_IIT_MOUSE_RBUTTON_DOWN;
		if( hti != NULL && OnRcgtdItemInvoke( hti, eIIT ) )
			return true;
	} // if( ! strID.IsEmpty() )

	return CExtRichContentScrollWnd::RciEvent_OnPressedEnter( pRCI, nChar, nRepCnt, nFlags, htInfoOld, htInfoNew, hWndContentViewer );
}

bool CExtRichGenWnd::RciEvent_OnClick(
	CExtRichContentItem * pRCI,
	UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
	UINT nFlags, // mouse event flags
	const CExtRichContentHitTestInfo & htInfoOld,
	const CExtRichContentHitTestInfo & htInfoNew,
	HWND hWndContentViewer
	)
{
	ASSERT_VALID( this );
	ASSERT( pRCI != NULL );
	ASSERT( hWndContentViewer != NULL && ::IsWindow(hWndContentViewer) );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	OnRcgtdKeyTipTrackingStop();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtSafeString strID = pRCI->ElementUiBindingSrcIdGet();
	if( ! strID.IsEmpty() )
	{
		HTREEITEM hti = NULL;
		int nFieldsScanned =
#if _MFC_VER >= 0x800
			_stscanf_s( LPCTSTR(strID), _T("%p"), &hti );
#else
			_stscanf( LPCTSTR(strID), _T("%p"), &hti );
#endif
		ASSERT( nFieldsScanned == 1 );
		nFieldsScanned;
		CExtRichGenTreeData::e_item_invoke_type eIIT = __E_RCTD_IIT_MOUSE_LBUTTON_UP;
		if( nChar == VK_MBUTTON )
			eIIT = __E_RCTD_IIT_MOUSE_MBUTTON_UP;
		else if( nChar == VK_RBUTTON )
			eIIT = __E_RCTD_IIT_MOUSE_RBUTTON_UP;
		if( hti != NULL && OnRcgtdItemInvoke( hti, eIIT ) )
			return true;
	} // if( ! strID.IsEmpty() )
	
	return CExtRichContentScrollWnd::RciEvent_OnClick( pRCI, nChar, nFlags, htInfoOld, htInfoNew, hWndContentViewer );
}

bool CExtRichGenWnd::RciEvent_OnDoubleClick(
	CExtRichContentItem * pRCI,
	UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
	UINT nFlags, // mouse event flags
	const CExtRichContentHitTestInfo & htInfoOld,
	const CExtRichContentHitTestInfo & htInfoNew,
	HWND hWndContentViewer
	)
{
	ASSERT_VALID( this );
	ASSERT( pRCI != NULL );
	ASSERT( hWndContentViewer != NULL && ::IsWindow(hWndContentViewer) );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	OnRcgtdKeyTipTrackingStop();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return CExtRichContentScrollWnd::RciEvent_OnDoubleClick( pRCI, nChar, nFlags, htInfoOld, htInfoNew, hWndContentViewer );
}

bool CExtRichGenWnd::OnRcsAnalyzeKeyEvent(
	bool bSysKey,
	bool bKeyDownEvent, // true - key-down event, false - key-up event
	UINT nChar, // virtual key code
	UINT nRepCnt, // key-down/key-up press count
	UINT nFlags // key-down/key-up event flags
	)
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	switch( nChar )
	{
	case VK_MENU:
		if( bKeyDownEvent )
			break;
	case VK_TAB:
	case VK_UP:
	case VK_DOWN:
	case VK_LEFT:
	case VK_RIGHT:
	case VK_PRIOR:
	case VK_NEXT:
	case VK_CONTROL:
	case VK_SHIFT:
		if( RcgtdKeyTipsDisplayedGet() )
		{
			OnRcgtdKeyTipTrackingStop();
			return true;
		}
	break;
	case VK_SPACE:
	case VK_RETURN:
		if( m_pRCI_focus != NULL )
		{
			CExtSafeString strID = m_pRCI_focus->ElementUiBindingSrcIdGet();
			ASSERT( ! strID.IsEmpty() );
			HTREEITEM hti = NULL;
			int nFieldsScanned =
#if _MFC_VER >= 0x800
				_stscanf_s( LPCTSTR(strID), _T("%p"), &hti );
#else
				_stscanf( LPCTSTR(strID), _T("%p"), &hti );
#endif
			ASSERT( nFieldsScanned == 1 );
			nFieldsScanned;
			if( hti != NULL )
				return
					OnRcgtdItemInvoke(
						hti,
						( nChar == VK_SPACE )
							? ( bKeyDownEvent ? __E_RCTD_IIT_KEY_VK_SPACE_DOWN  : __E_RCTD_IIT_KEY_VK_SPACE_UP  )
							: ( bKeyDownEvent ? __E_RCTD_IIT_KEY_VK_RETURN_DOWN : __E_RCTD_IIT_KEY_VK_RETURN_UP )
						);
		}
	break;
	} // switch( nChar )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return CExtRichContentScrollWnd::OnRcsAnalyzeKeyEvent( bSysKey, bKeyDownEvent, nChar, nRepCnt, nFlags );
}

bool CExtRichGenWnd::OnRcsAnalyzeMouseClickEvent(
	UINT nChar, // VK_LBUTTON, VK_RBUTTON or VK_MBUTTON only
	UINT nRepCnt, // 0 - button up, 1 - single click, 2 - double click, 3 - post single click & begin editing
	UINT nFlags, // mouse event flags
	CPoint point // mouse pointer in client coordinates
	)
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	OnRcgtdKeyTipTrackingStop();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return CExtRichContentScrollWnd::OnRcsAnalyzeMouseClickEvent( nChar, nRepCnt, nFlags, point );
}

void CExtRichGenWnd::OnRcsAnalyzeScrollBarMouseClickEvent(
	const CExtRichContentHitTestInfo & htInfoPressed
	)
{
	ASSERT_VALID( this );
	ASSERT( htInfoPressed.m_listHtBranch.GetCount() > 0 );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	OnRcgtdKeyTipTrackingStop();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	CExtRichContentScrollWnd::OnRcsAnalyzeScrollBarMouseClickEvent( htInfoPressed );
}

bool CExtRichGenWnd::OnHookWndMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	UINT nMessage,
	WPARAM & wParam,
	LPARAM & lParam
	)
{
__PROF_UIS_MANAGE_STATE;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( m_bHelperKeyTipsSupported && GetSafeHwnd() != NULL && m_helperHookedHWND != NULL && hWndHooked == m_helperHookedHWND )
	{
		if(		nMessage == WM_SIZE
			||	nMessage == WM_MOVE
			||	nMessage == WM_ACTIVATEAPP
			||	(	nMessage == WM_COMMAND
				&&	( ( (HIWORD(wParam)) == 0 ) || ( (HIWORD(wParam)) == 1 ) )
				)
			||	nMessage == WM_SYSCOMMAND
			)
		{ // if cancel flat tracking messages
			HWND hWndCapture = CExtMouseCaptureSink::GetCapture();
			if( hWndCapture == m_hWnd )
				CExtMouseCaptureSink::ReleaseCapture();
			OnRcgtdKeyTipTrackingStop();
		}// if cancel flat tracking messages
	} // if( m_bHelperKeyTipsSupported && GetSafeHwnd() != NULL && m_helperHookedHWND != NULL && hWndHooked == m_helperHookedHWND )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return 
		CExtHookSink::OnHookWndMsg(
			lResult,
			hWndHooked,
			nMessage,
			wParam,
			lParam
			);
}

bool CExtRichGenWnd::RcgwTreeItemIsFocused(
	HTREEITEM hti,
	bool bRetValIfElementNotFound // = false
	) const
{
	ASSERT_VALID( this );
	if( hti == NULL )
		return false;
	ASSERT( RcgtdItemIsPresent( hti ) );
	hti = ( const_cast < CExtRichGenWnd * > ( this ) ) -> OnRcgwTranslateTreeItemFocus( hti );
	if( hti == NULL )
		return bRetValIfElementNotFound;
	ASSERT( RcgtdItemIsPresent( hti ) );
CExtSafeString strID;
	strID.Format( _T("%p"), LPVOID(hti) );
CExtRichContentLayout * pRCL = ( const_cast < CExtRichGenWnd * > ( this ) ) -> RcsLayoutGet();
	ASSERT( pRCL != NULL );
CExtRichContentItem * pRCI = pRCL->ElementByUiBindingSrcID( LPCTSTR(strID) );
	if( pRCI == NULL )
		return bRetValIfElementNotFound;
	ASSERT( LPVOID(&(pRCI->m_layout)) == LPVOID(pRCL) );
	pRCI = ( const_cast < CExtRichGenWnd * > ( this ) ) -> OnRcgwTranslateElementFocus( pRCI );
	if( pRCI == NULL )
		return bRetValIfElementNotFound;
	ASSERT( LPVOID(&(pRCI->m_layout)) == LPVOID(pRCL) );
	if( pRCI->m_PCD.m_eynFocus == CExtRichStyleDescription::e_ui_yn_yes )
		return true;
	return false;
}

bool CExtRichGenWnd::RcgwTreeItemIsSelected(
	HTREEITEM hti,
	bool bRetValIfElementNotFound // = false
	) const
{
	ASSERT_VALID( this );
	if( hti == NULL )
		return false;
	ASSERT( RcgtdItemIsPresent( hti ) );
	hti = ( const_cast < CExtRichGenWnd * > ( this ) ) -> OnRcgwTranslateTreeItemSelection( hti );
	if( hti == NULL )
		return bRetValIfElementNotFound;
	ASSERT( RcgtdItemIsPresent( hti ) );
CExtSafeString strID;
	strID.Format( _T("%p"), LPVOID(hti) );
CExtRichContentLayout * pRCL = ( const_cast < CExtRichGenWnd * > ( this ) ) -> RcsLayoutGet();
	ASSERT( pRCL != NULL );
CExtRichContentItem * pRCI = pRCL->ElementByUiBindingSrcID( LPCTSTR(strID) );
	if( pRCI == NULL )
		return bRetValIfElementNotFound;
	ASSERT( LPVOID(&(pRCI->m_layout)) == LPVOID(pRCL) );
	pRCI = ( const_cast < CExtRichGenWnd * > ( this ) ) -> OnRcgwTranslateElementSelection( pRCI );
	if( pRCI == NULL )
		return bRetValIfElementNotFound;
	ASSERT( LPVOID(&(pRCI->m_layout)) == LPVOID(pRCL) );
	if( pRCI->m_PCD.m_eynSelection == CExtRichStyleDescription::e_ui_yn_yes )
		return true;
	return false;
}

bool CExtRichGenWnd::RcgwTreeItemIsEnabled(
	HTREEITEM hti,
	bool bRetValIfElementNotFound // = false
	) const
{
	ASSERT_VALID( this );
	if( hti == NULL )
		return false;
	ASSERT( RcgtdItemIsPresent( hti ) );
	hti = ( const_cast < CExtRichGenWnd * > ( this ) ) -> OnRcgwTranslateTreeItemEnable( hti );
	if( hti == NULL )
		return bRetValIfElementNotFound;
	ASSERT( RcgtdItemIsPresent( hti ) );
CExtSafeString strID;
	strID.Format( _T("%p"), LPVOID(hti) );
CExtRichContentLayout * pRCL = ( const_cast < CExtRichGenWnd * > ( this ) ) -> RcsLayoutGet();
	ASSERT( pRCL != NULL );
CExtRichContentItem * pRCI = pRCL->ElementByUiBindingSrcID( LPCTSTR(strID) );
	if( pRCI == NULL )
		return bRetValIfElementNotFound;
	ASSERT( LPVOID(&(pRCI->m_layout)) == LPVOID(pRCL) );
	pRCI = ( const_cast < CExtRichGenWnd * > ( this ) ) -> OnRcgwTranslateElementEnable( pRCI );
	if( pRCI == NULL )
		return bRetValIfElementNotFound;
	ASSERT( LPVOID(&(pRCI->m_layout)) == LPVOID(pRCL) );
	if( pRCI->m_PCD.m_eynDisabled != CExtRichStyleDescription::e_ui_yn_yes )
		return true;
	return false;
}

bool CExtRichGenWnd::RcgwTreeItemFocus(
	HTREEITEM htiNewFocus,
	bool bDeleayIfElementNotFound, // = true
	bool bRecalcLayout // = true
	)
{
	ASSERT_VALID( this );
	if( htiNewFocus == NULL )
		return false;
// #if (!defined __EXT_MFC_NO_CUSTOMIZE)
// 	OnRcgtdKeyTipTrackingStop();
// #endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	ASSERT( RcgtdItemIsPresent( htiNewFocus ) );
	htiNewFocus = OnRcgwTranslateTreeItemFocus( htiNewFocus );
	if( htiNewFocus == NULL )
		return false;
	ASSERT( RcgtdItemIsPresent( htiNewFocus ) );
CExtSafeString strID;
	strID.Format( _T("%p"), LPVOID(htiNewFocus) );
CExtRichContentLayout * pRCL = RcsLayoutGet();
	ASSERT( pRCL != NULL );
CExtRichContentItem * pNewFocusRCI = pRCL->ElementByUiBindingSrcID( LPCTSTR(strID) );
	if( pNewFocusRCI == NULL )
	{
		if( bDeleayIfElementNotFound )
			RcgwTreeItemFocusDelay( htiNewFocus );
		return false;
	}
	ASSERT( LPVOID(&(pNewFocusRCI->m_layout)) == LPVOID(pRCL) );
	pNewFocusRCI = OnRcgwTranslateElementFocus( pNewFocusRCI );
	if( pNewFocusRCI == NULL )
		return false;
	ASSERT( LPVOID(&(pNewFocusRCI->m_layout)) == LPVOID(pRCL) );
bool bQuickReStyleMode = ( ( RcsGetStyle() & __EXT_RCS_QUICK_RESTYLE_MODE ) ) ? true : false;
bool bHaveElementsWithResetStyles = false;

#if (defined __EXT_RCS_WND_APPLY_HOVER_RESET_WORKAROUND__)
	// BEGIN // temporarily solution, reset hovered state to avoid incorrect inheritance from parent element inside OnRcsResetElementStates()
	if( ! m_htInfoHover.IsEmpty() )
	{
		CExtRichContentItem * pRootRCI = ( pRCL->m_listHeap.GetCount() > 0 ) ? pRCL->m_listHeap.GetHead() : NULL;
		ASSERT( pRootRCI != NULL );
		m_htInfoHover.Empty();
		CExtRichStyleDescription::pseudo_class_def_t statesHoverOnly;
		statesHoverOnly.m_eynHover = CExtRichStyleDescription::e_ui_yn_yes;
		OnRcsResetElementStates( pRootRCI, statesHoverOnly, CExtRichStyleDescription::e_ui_yn_unspecified );
	}
	//  END  // temporarily solution, reset hovered state to avoid incorrect inheritance from parent element inside OnRcsResetElementStates()
#endif // (defined __EXT_RCS_WND_APPLY_HOVER_RESET_WORKAROUND__)

CExtRichStyleDescription::pseudo_class_def_t statesFocusOnly;
	statesFocusOnly.m_eynFocus = CExtRichStyleDescription::e_ui_yn_yes;
CExtRichContentItem * pOldFocusRCI = m_pRCI_focus;
	if( m_pRCI_focus != NULL )
	{
		m_pRCI_focus->RciEvent_OnFocusLeave( m_hWnd, pNewFocusRCI );
		if( OnRcsResetElementStates( m_pRCI_focus, statesFocusOnly, CExtRichStyleDescription::e_ui_yn_unspecified ) )
			bHaveElementsWithResetStyles = true;
	}
	m_pRCI_focus = pNewFocusRCI;
	m_pRCI_focus->RciEvent_OnFocusEnter( m_hWnd, pOldFocusRCI );
	if( bRecalcLayout )
		OnSwInvalidate( m_bRedrawErase );
	if( OnRcsResetElementStates( m_pRCI_focus, statesFocusOnly, CExtRichStyleDescription::e_ui_yn_yes ) )
		bHaveElementsWithResetStyles = true;
	if( bRecalcLayout && (! bQuickReStyleMode ) )
	{
		RcsDelayRecalcLayout();
	//	OnSwRecalcLayout( true );
		OnSwUpdateScrollBars();
	}
	bHaveElementsWithResetStyles;
	return true;
}

bool CExtRichGenWnd::RcgwTreeItemSelect(
	HTREEITEM htiSelect,
	bool bDeleayIfElementNotFound, // = true
	bool bRecalcLayout // = true
	)
{
	ASSERT_VALID( this );
	if( htiSelect == NULL )
		return false;
// #if (!defined __EXT_MFC_NO_CUSTOMIZE)
// 	OnRcgtdKeyTipTrackingStop();
// #endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	ASSERT( RcgtdItemIsPresent( htiSelect ) );
	htiSelect = OnRcgwTranslateTreeItemSelection( htiSelect );
	if( htiSelect == NULL )
		return false;
	ASSERT( RcgtdItemIsPresent( htiSelect ) );
CExtSafeString strID;
	strID.Format( _T("%p"), LPVOID(htiSelect) );
CExtRichContentLayout * pRCL = RcsLayoutGet();
	ASSERT( pRCL != NULL );
CExtRichContentItem * pSelectRCI = pRCL->ElementByUiBindingSrcID( LPCTSTR(strID) );
	if( pSelectRCI == NULL )
	{
		if( bDeleayIfElementNotFound )
			RcgwTreeItemSelectionDelay( htiSelect );
		return false;
	}
	ASSERT( LPVOID(&(pSelectRCI->m_layout)) == LPVOID(pRCL) );
	pSelectRCI = OnRcgwTranslateElementSelection( pSelectRCI );
	if( pSelectRCI == NULL )
		return false;
	ASSERT( LPVOID(&(pSelectRCI->m_layout)) == LPVOID(pRCL) );
bool bQuickReStyleMode = ( ( RcsGetStyle() & __EXT_RCS_QUICK_RESTYLE_MODE ) ) ? true : false;
bool bHaveElementsWithResetStyles = false;
CExtRichStyleDescription::pseudo_class_def_t statesSelectionOnly;
	statesSelectionOnly.m_eynSelection = CExtRichStyleDescription::e_ui_yn_yes;
	pSelectRCI->RciEvent_OnSelect( m_hWnd, true );
	if( bRecalcLayout )
		OnSwInvalidate( m_bRedrawErase );
	if( OnRcsResetElementStates( pSelectRCI, statesSelectionOnly, CExtRichStyleDescription::e_ui_yn_yes ) )
		bHaveElementsWithResetStyles = true;
	if( bRecalcLayout && (! bQuickReStyleMode ) )
	{
		RcsDelayRecalcLayout();
	//	OnSwRecalcLayout( true );
		OnSwUpdateScrollBars();
	}
	bHaveElementsWithResetStyles;
	return true;
}

bool CExtRichGenWnd::RcgwTreeItemEnable(
	HTREEITEM htiEnable,
	bool bEnable, // = true
	bool bDeleayIfElementNotFound, // = true
	bool bRecalcLayout // = true
	)
{
	ASSERT_VALID( this );
	if( htiEnable == NULL )
		return false;
// #if (!defined __EXT_MFC_NO_CUSTOMIZE)
// 	OnRcgtdKeyTipTrackingStop();
// #endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	ASSERT( RcgtdItemIsPresent( htiEnable ) );
	htiEnable = OnRcgwTranslateTreeItemEnable( htiEnable );
	if( htiEnable == NULL )
		return false;
	ASSERT( RcgtdItemIsPresent( htiEnable ) );
CExtSafeString strID;
	strID.Format( _T("%p"), LPVOID(htiEnable) );
CExtRichContentLayout * pRCL = RcsLayoutGet();
	ASSERT( pRCL != NULL );
CExtRichContentItem * pEnableRCI = pRCL->ElementByUiBindingSrcID( LPCTSTR(strID) );
	if( pEnableRCI == NULL )
	{
		if( bDeleayIfElementNotFound )
			RcgwTreeItemEnableDelay( htiEnable, bEnable );
		return false;
	}
	ASSERT( LPVOID(&(pEnableRCI->m_layout)) == LPVOID(pRCL) );
	pEnableRCI = OnRcgwTranslateElementEnable( pEnableRCI );
	if( pEnableRCI == NULL )
		return false;
	ASSERT( LPVOID(&(pEnableRCI->m_layout)) == LPVOID(pRCL) );
bool bQuickReStyleMode = ( ( RcsGetStyle() & __EXT_RCS_QUICK_RESTYLE_MODE ) ) ? true : false;
bool bHaveElementsWithResetStyles = false;
CExtRichStyleDescription::pseudo_class_def_t statesDisableOnly;
	statesDisableOnly.m_eynDisabled = CExtRichStyleDescription::e_ui_yn_yes;
	pEnableRCI->RciEvent_OnDisable( m_hWnd, ! bEnable );
	if( bRecalcLayout )
		OnSwInvalidate( m_bRedrawErase );
	if( OnRcsResetElementStates( pEnableRCI, statesDisableOnly, bEnable ? CExtRichStyleDescription::e_ui_yn_not : CExtRichStyleDescription::e_ui_yn_yes ) )
		bHaveElementsWithResetStyles = true;
	if( bRecalcLayout && (! bQuickReStyleMode ) )
	{
		RcsDelayRecalcLayout();
	//	OnSwRecalcLayout( true );
		OnSwUpdateScrollBars();
	}
	bHaveElementsWithResetStyles;
	return true;
}

bool CExtRichGenWnd::RcgwTreeItemFocusDelay( // if ( NULL, false ) - empty delayed list of HTREEITEMs
	HTREEITEM htiDelay,
	bool bDelay // = true
	)
{
	ASSERT_VALID( this );
	if( htiDelay == NULL )
	{
		if( bDelay )
			return false;
		m_listDelayedFocus.RemoveAll();
		return true;
	} // if( htiDelay == NULL )
	ASSERT( RcgtdItemIsPresent( htiDelay ) );
POSITION pos = m_listDelayedFocus.Find( htiDelay );
	if( pos != NULL )
		m_listDelayedFocus.RemoveAt( pos );
	if( bDelay )
		m_listDelayedFocus.AddTail( htiDelay );
	return true;
}

bool CExtRichGenWnd::RcgwTreeItemSelectionDelay( // if ( NULL, false ) - empty delayed list of HTREEITEMs
	HTREEITEM htiDelay,
	bool bDelay // = true
	)
{
	ASSERT_VALID( this );
	if( htiDelay == NULL )
	{
		if( bDelay )
			return false;
		m_listDelayedSelection.RemoveAll();
		return true;
	} // if( htiDelay == NULL )
	ASSERT( RcgtdItemIsPresent( htiDelay ) );
POSITION pos = m_listDelayedSelection.Find( htiDelay );
	if( pos != NULL )
		m_listDelayedSelection.RemoveAt( pos );
	if( bDelay )
		m_listDelayedSelection.AddTail( htiDelay );
	return true;
}

bool CExtRichGenWnd::RcgwTreeItemEnableDelay( // if ( NULL, false ) - empty delayed list of HTREEITEMs
	HTREEITEM htiDelay,
	bool bEnable, // = true
	bool bDelay // = true
	)
{
	ASSERT_VALID( this );
	if( htiDelay == NULL )
	{
		if( bDelay )
			return false;
		m_listDelayedEnable.RemoveAll();
		m_mapDelayedEnableStates.RemoveAll();
		return true;
	} // if( htiDelay == NULL )
	ASSERT( RcgtdItemIsPresent( htiDelay ) );
POSITION pos = m_listDelayedSelection.Find( htiDelay );
	if( pos != NULL )
	{
		m_listDelayedSelection.RemoveAt( pos );
		m_mapDelayedEnableStates.RemoveKey( htiDelay );
	}
	if( bDelay )
	{
		m_listDelayedSelection.AddTail( htiDelay );
		m_mapDelayedEnableStates.SetAt( htiDelay, bEnable );
	}
	return true;
}

HTREEITEM CExtRichGenWnd::OnRcgwTranslateTreeItem( HTREEITEM hti )
	{ ASSERT_VALID( this ); ASSERT( hti != NULL ); ASSERT( RcgtdItemIsPresent( hti ) ); return hti; }
HTREEITEM CExtRichGenWnd::OnRcgwTranslateTreeItemFocus( HTREEITEM hti )
	{ ASSERT_VALID( this ); ASSERT( hti != NULL ); ASSERT( RcgtdItemIsPresent( hti ) ); return OnRcgwTranslateTreeItem( hti ); }
HTREEITEM CExtRichGenWnd::OnRcgwTranslateTreeItemSelection( HTREEITEM hti )
	{ ASSERT_VALID( this ); ASSERT( hti != NULL ); ASSERT( RcgtdItemIsPresent( hti ) ); return OnRcgwTranslateTreeItem( hti ); }
HTREEITEM CExtRichGenWnd::OnRcgwTranslateTreeItemEnable( HTREEITEM hti )
	{ ASSERT_VALID( this ); ASSERT( hti != NULL ); ASSERT( RcgtdItemIsPresent( hti ) ); return OnRcgwTranslateTreeItem( hti ); }
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
HTREEITEM CExtRichGenWnd::OnRcgwTranslateTreeItemKeyTip( HTREEITEM hti )
	{ ASSERT_VALID( this ); ASSERT( hti != NULL ); ASSERT( RcgtdItemIsPresent( hti ) ); return OnRcgwTranslateTreeItem( hti ); }
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtRichContentItem * CExtRichGenWnd::OnRcgwTranslateElement( CExtRichContentItem * pRCI )
	{ ASSERT_VALID( this ); ASSERT( pRCI != NULL ); ASSERT( LPVOID(&(pRCI->m_layout)) == LPVOID(RcsLayoutGet()) ); return pRCI; }
CExtRichContentItem * CExtRichGenWnd::OnRcgwTranslateElementFocus( CExtRichContentItem * pRCI )
	{ ASSERT_VALID( this ); ASSERT( pRCI != NULL ); ASSERT( LPVOID(&(pRCI->m_layout)) == LPVOID(RcsLayoutGet()) ); return OnRcgwTranslateElement( pRCI ); }
CExtRichContentItem * CExtRichGenWnd::OnRcgwTranslateElementSelection( CExtRichContentItem * pRCI )
	{ ASSERT_VALID( this ); ASSERT( pRCI != NULL ); ASSERT( LPVOID(&(pRCI->m_layout)) == LPVOID(RcsLayoutGet()) ); return OnRcgwTranslateElement( pRCI ); }
CExtRichContentItem * CExtRichGenWnd::OnRcgwTranslateElementEnable( CExtRichContentItem * pRCI )
	{ ASSERT_VALID( this ); ASSERT( pRCI != NULL ); ASSERT( LPVOID(&(pRCI->m_layout)) == LPVOID(RcsLayoutGet()) ); return OnRcgwTranslateElement( pRCI ); }
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtRichContentItem * CExtRichGenWnd::OnRcgwTranslateElementKeyTip( CExtRichContentItem * pRCI )
	{ ASSERT_VALID( this ); ASSERT( pRCI != NULL ); ASSERT( LPVOID(&(pRCI->m_layout)) == LPVOID(RcsLayoutGet()) ); return OnRcgwTranslateElement( pRCI ); }
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

void CExtRichGenWnd::OnRcsPostLayoutContent( CDC & dc, const CRect & rcLayout, CFont * pFont )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pFont->GetSafeHandle() != NULL );
bool bAnySelected = false, bAnyFocused = false, bAnyEnabled = false;
POSITION pos = m_listDelayedSelection.GetHeadPosition();
	for( ; pos != NULL; )
	{
		HTREEITEM hti = m_listDelayedSelection.GetNext( pos );
		ASSERT( hti != NULL && RcgtdItemIsPresent( hti ) );
		RcgwTreeItemSelect( hti, false, false );
		bAnySelected = true;
	}
	m_listDelayedSelection.RemoveAll();
	pos = m_listDelayedFocus.GetHeadPosition();
	for( ; pos != NULL; )
	{
		HTREEITEM hti = m_listDelayedFocus.GetNext( pos );
		ASSERT( hti != NULL && RcgtdItemIsPresent( hti ) );
		RcgwTreeItemFocus( hti, false, false );
		bAnyFocused = true;
	}
	m_listDelayedFocus.RemoveAll();
	pos = m_listDelayedEnable.GetHeadPosition();
	for( ; pos != NULL; )
	{
		HTREEITEM hti = m_listDelayedEnable.GetNext( pos );
		ASSERT( hti != NULL && RcgtdItemIsPresent( hti ) );
		bool bEnable = false;
		m_mapDelayedEnableStates.Lookup( hti, bEnable );
		RcgwTreeItemEnable( hti, bEnable, false, false );
		bAnyEnabled = true;
	}
	m_listDelayedEnable.RemoveAll();
	m_mapDelayedEnableStates.RemoveAll();
	CExtRichContentScrollWnd::OnRcsPostLayoutContent( dc, rcLayout, pFont );
bool bAnyChanged = ( bAnySelected || bAnyFocused || bAnyEnabled ) ? true : false;
	if( bAnyChanged )
		RcsDelayRecalcLayout();
}

bool CExtRichGenWnd::OnRcgtdItemInserted( HTREEITEM hti )
{
	ASSERT_VALID( this );
	ASSERT( hti != NULL );
	ASSERT( RcgtdItemIsPresent( hti ) );
CExtRichGenItemData * pItemData = NULL;
	try
	{
		pItemData = new CExtRichGenItemData;
	}
	catch( CException * pException )
	{
		ASSERT( FALSE );
		pException->Delete();
		return false;
	}
	catch( ... )
	{
		ASSERT( FALSE );
		return false;
	}
	RcgtdItemDataSet( hti, pItemData );
	return true;
}

void CExtRichGenWnd::OnRcgtdRemoveItemData( HTREEITEM hti, LPVOID pItemDataToRemove )
{
	ASSERT_VALID( this );
	ASSERT( hti != NULL );
	ASSERT( RcgtdItemIsPresent( hti ) );
	ASSERT( pItemDataToRemove != NULL ); 
	hti;
CExtRichGenItemData * pItemData = (CExtRichGenItemData*) pItemDataToRemove;
	delete pItemData;
}

bool CExtRichGenWnd::OnRcgwGenerateContent_WalkChildren( CExtSafeString & strContent, HTREEITEM htiGen, INT nGenContentType )
{
	ASSERT_VALID( this );
	ASSERT( htiGen != NULL );
	ASSERT( RcgtdItemIsPresent( htiGen ) );
	for( htiGen = RcgtdItemGetFirstChild( htiGen ); htiGen != NULL; htiGen = RcgtdItemGetNextSibling( htiGen ) )
	{
		CExtSafeString strInnerContent;
		if( ! OnRcgwGenerateContent( strInnerContent, htiGen, nGenContentType ) )
			return false;
		strContent += strInnerContent;
	}
	return true;
}

bool CExtRichGenWnd::OnRcgwGenerateContent( CExtSafeString & strContent, HTREEITEM htiGen, INT nGenContentType )
{
	ASSERT_VALID( this );
	ASSERT( htiGen != NULL );
	ASSERT( RcgtdItemIsPresent( htiGen ) );
	return OnRcgwGenerateContent_WalkChildren( strContent, htiGen, nGenContentType );
}

INT CExtRichGenWnd::OnRcgwUpdateCmdUI(
	CCmdTarget * pCmdTarget,
	bool bDisableCmdIfNoHandler // = true
	)
{
	ASSERT_VALID( this );
	if( pCmdTarget != NULL )
		return OnRcgwUpdateItemCmdUI( pCmdTarget, NULL, true, bDisableCmdIfNoHandler );
	return false;
}

INT CExtRichGenWnd::OnRcgwUpdateItemCmdUI(
	CCmdTarget * pCmdTarget,
	HTREEITEM htiUpdate, // = NULL // NULL means root
	bool bDeep, // = true
	bool bDisableCmdIfNoHandler // = true
	)
{
	ASSERT_VALID( this );
	if( pCmdTarget == NULL )
		return false;
	ASSERT_VALID( pCmdTarget );
	if( htiUpdate == NULL )
	{
		htiUpdate = RcgtdItemGetRoot();
		if( htiUpdate == NULL )
			return false;
	}
bool bRetVal = false;
CExtRichGenItemData * pItemData = (CExtRichGenItemData *) RcgtdItemDataGet( htiUpdate );
	if( pItemData != NULL && CExtCmdManager::IsCommand( pItemData->m_nCmdID ) )
	{
		CExtRichGenItemCmdUI cmd_ui( htiUpdate, this );
		if( cmd_ui.DoUpdate( pCmdTarget, bDisableCmdIfNoHandler ? TRUE : FALSE ) )
			bRetVal = true;
	} // if( pItemData != NULL ...
	if( ! bDeep )
		return bRetVal;
	for( htiUpdate = RcgtdItemGetFirstChild( htiUpdate ); htiUpdate != NULL; htiUpdate = RcgtdItemGetNextSibling( htiUpdate ) )
	{
		if( OnRcgwUpdateItemCmdUI( pCmdTarget, htiUpdate, bDeep ) )
			bRetVal = true;
	}
	return bRetVal;
}

bool CExtRichGenWnd::OnRcgwUpdateItemCmdUI_Enable( CExtRichGenItemCmdUI & cmdUI, bool bOn )
{
	ASSERT_VALID( this );
bool bEnabled = RcgwTreeItemIsEnabled( cmdUI.m_htiUpdate );
	if( ( bEnabled && bOn ) || ( (!bEnabled) && (!bOn) ) )
		return false;
	RcgwTreeItemEnable( cmdUI.m_htiUpdate, bOn, false, false );
	RcsDelayRecalcLayout();
	return true;
}

void CExtRichGenWnd::OnRcgwUpdateItemCmdUI_SetCheck( CExtRichGenItemCmdUI & cmdUI, int nCheck )
	{ ASSERT_VALID( this ); cmdUI; nCheck; }
void CExtRichGenWnd::OnRcgwUpdateItemCmdUI_SetRadio( CExtRichGenItemCmdUI & cmdUI, bool bOn )
	{ ASSERT_VALID( this ); cmdUI; bOn; }
void CExtRichGenWnd::OnRcgwUpdateItemCmdUI_SetText( CExtRichGenItemCmdUI & cmdUI, __EXT_MFC_SAFE_LPCTSTR strText )
	{ ASSERT_VALID( this ); cmdUI; strText; }

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

CWnd * CExtRichGenWnd::OnRcgtdKeyTipGetParentWnd()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() != NULL )
		return this;
	return CWnd::GetDesktopWindow();
}

CPoint CExtRichGenWnd::OnRcgtdKeyTipGetGuideLines( HTREEITEM hti )
{
CPoint ptKeyTipGetGuideLines( __EXT_RICH_CONTENT_NEGATIVE_UNUSED, __EXT_RICH_CONTENT_NEGATIVE_UNUSED );
	if( hti == NULL )
		return ptKeyTipGetGuideLines;
	ASSERT( RcgtdItemIsPresent( hti ) );
	if( ! RcgwTreeItemIsEnabled( hti ) )
		return ptKeyTipGetGuideLines;
	hti = OnRcgwTranslateTreeItemKeyTip( hti );
	if( hti == NULL )
		return ptKeyTipGetGuideLines;
	ASSERT( RcgtdItemIsPresent( hti ) );
CExtRichContentLayout * pRCL = RcsLayoutGet();
	ASSERT( pRCL != NULL );
CExtSafeString strID;
	strID.Format( _T("%p"), LPVOID(hti) );
CExtRichContentItem * pRCI = pRCL->ElementByUiBindingSrcID( LPCTSTR(strID) );
	if( pRCI == NULL )
		return ptKeyTipGetGuideLines;
	pRCI = OnRcgwTranslateElementKeyTip( pRCI );
	if( pRCI == NULL )
		return ptKeyTipGetGuideLines;
	if( ! pRCI->IsParticipantOfHitTest( true, false ) )
		return ptKeyTipGetGuideLines;
	ASSERT( LPVOID(&(pRCI->m_layout)) == LPVOID(pRCL) );
CRect rc = pRCI->CalcEffectiveRect();
	ClientToScreen( rc );
	ptKeyTipGetGuideLines = rc.TopLeft();
	return ptKeyTipGetGuideLines;
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)






#endif // (!defined __EXT_MFC_NO_RICHCONTENTGENERATORWND)

#endif // (!defined __EXT_MFC_NO_SCROLLRICHCONTENTWND)

#endif // (!defined __EXT_MFC_NO_SCROLLWND)

