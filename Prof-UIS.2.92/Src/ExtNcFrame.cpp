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

#if (!defined __EXT_NC_FRAME_H)
	#include <ExtNcFrame.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_MENUCONTROLBAR_H)
	#include <ExtMenuControlBar.h>
#endif

#if (!defined __AFXPRIV_H__)
	#include <AfxPriv.h>
#endif 

#if _MFC_VER < 0x700
	#include <../src/AfxImpl.h>
#else
	#ifndef __AFXSTATE_H__
		#include <../src/mfc/afxstat_.h>
	#endif
	#include <../src/mfc/AfxImpl.h>
#endif

// disable warning 4706
#pragma warning( push )
#pragma warning ( disable : 4706 )
	#include <multimon.h>
// rollback warning 4706
#pragma warning( pop )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if (!defined __EXT_MFC_NO_NC_FRAME )

IMPLEMENT_CExtPmBridge_MEMBERS_GENERIC( CExtPmBridgeNC )

CExtPmBridgeNC::CExtPmBridgeNC(
	CExtNcFrameImpl * pNcFrameImpl // = NULL
	)
	: m_pNcFrameImpl( pNcFrameImpl )
{
	PmBridge_Install();
}

CExtPmBridgeNC::~CExtPmBridgeNC()
{
	PmBridge_Uninstall();
}

const CExtNcFrameImpl * CExtPmBridgeNC::NcFrameImpl_Get() const
{
	return m_pNcFrameImpl;
}

CExtNcFrameImpl * CExtPmBridgeNC::NcFrameImpl_Get()
{
	return m_pNcFrameImpl;
}

void CExtPmBridgeNC::NcFrameImpl_Set(
	CExtNcFrameImpl * pNcFrameImpl
	)
{
	m_pNcFrameImpl = pNcFrameImpl;
}

HWND CExtPmBridgeNC::PmBridge_GetSafeHwnd() const
{
	if( m_pNcFrameImpl != NULL )
	{
		HWND hWnd =
			m_pNcFrameImpl->NcFrameImpl_OnQueryHWND();
		return hWnd;
	}
	return NULL;
}

void CExtPmBridgeNC::PmBridge_OnPaintManagerChanged(
	CExtPaintManager * pGlobalPM
	)
{
	if( m_pNcFrameImpl == NULL )
		return;
	m_pNcFrameImpl->NcFrameImpl_SyncThemeSettings( pGlobalPM, false );
	if( ! m_pNcFrameImpl->m_bNcFrameImpl_PivotPmSyncMode )
	{
//TRACE1( "---------------> CExtPmBridgeNC::PmBridge_OnPaintManagerChanged() --- %d\r\n", g_PaintManager.m_nInstallPaintManagerCounter );
		if( g_PaintManager.m_nInstallPaintManagerCounter != 0 )
		{
			HWND hWnd = PmBridge_GetSafeHwnd();
			if( hWnd != NULL )
				::PostMessage( hWnd, g_PaintManager.m_nMsgPaintManagerChanged, 0L, 0L );
		}
		else
			_AdjustThemeSettings();
		CExtPmBridge::PmBridge_OnPaintManagerChanged( pGlobalPM );
	}
	m_pNcFrameImpl->NcFrameImpl_SyncThemeSettings( pGlobalPM, true );
}

#if (! defined SPI_GETSCREENSAVERRUNNING)
	#define SPI_GETSCREENSAVERRUNNING 114
#endif

void CExtPmBridgeNC::PmBridge_OnDisplayChange(
	CExtPaintManager * pGlobalPM,
	CWnd * pWndNotifySrc,
	INT nDepthBPP,
	CPoint ptSizes
	)
{
	CExtPmBridge::PmBridge_OnDisplayChange(
		pGlobalPM,
		pWndNotifySrc,
		nDepthBPP,
		ptSizes
		);
//	if( g_PaintManager.m_bIsWinVistaOrLater )
//		Sleep( 3000 );
CExtNcFrameImpl * pNCFI = NcFrameImpl_Get();
	if( pNCFI == NULL )
		return;
CWnd * pWndFrameImpl = pNCFI->NcFrameImpl_GetFrameWindow();
	if( pWndFrameImpl->GetSafeHwnd() == NULL )
		return;
	if( ( pWndFrameImpl->GetStyle() & WS_CHILD ) != 0 )
		return;
	if( g_PaintManager.m_bIsWin2000orLater )
	{
		BOOL bScreenSavingIsRunning = FALSE;
		if( ! SystemParametersInfo( SPI_GETSCREENSAVERRUNNING, 0, &bScreenSavingIsRunning, 0 ) )
			bScreenSavingIsRunning = FALSE;
		if( bScreenSavingIsRunning )
			return;
	}
	else
	{
		CPoint ptCursor( 0, 0 );
		if( ! ::GetCursorPos( &ptCursor) )
			return;
	}
	_AdjustThemeSettings();
}

void CExtPmBridgeNC::PmBridge_OnThemeChanged(
	CExtPaintManager * pGlobalPM,
	CWnd * pWndNotifySrc,
	WPARAM wParam,
	LPARAM lParam
	)
{
	CExtPmBridge::PmBridge_OnThemeChanged(
		pGlobalPM,
		pWndNotifySrc,
		wParam,
		lParam
		);
//	if( g_PaintManager.m_bIsWinVistaOrLater )
//		Sleep( 1000 );
	_AdjustThemeSettings();
}

void CExtPmBridgeNC::_AdjustThemeSettings()
{
	if(		m_pNcFrameImpl == NULL
		||	m_pNcFrameImpl->m_bNcFrameImpl_InAdjustAdjustThemeSettings
		)
		return;
	if( ! m_pNcFrameImpl->m_bNcFrameImpl_IsEnabled )
		return;

	m_pNcFrameImpl->m_bNcFrameImpl_CreatePassed = true; // IMPORTANT: this may not be set earlier if frame is started as non-skinned
HWND hWndOwn = PmBridge_GetSafeHwnd();
	if( hWndOwn == NULL )
		return;
	ASSERT( ::IsWindow( hWndOwn ) );

	m_pNcFrameImpl->m_bNcFrameImpl_InAdjustAdjustThemeSettings = true;
	m_pNcFrameImpl->m_bNcFrameImpl_InAdjustAdjustThemeSettings_NoRgnSetup = true;
//	TRACE( "---------------> CExtPmBridgeNC::_AdjustThemeSettings()\r\n" );
WINDOWPLACEMENT _wp;
	::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
	_wp.length = sizeof(WINDOWPLACEMENT);
	CExtNcFrameImpl::stat_GetWindowPlacement( hWndOwn, _wp );
	if( ( ::__EXT_MFC_GetWindowLong( hWndOwn, GWL_STYLE ) & WS_VISIBLE ) == 0 )
		_wp.showCmd = SW_HIDE;
CWnd * pWndFrameImpl = (CWnd *)m_pNcFrameImpl->NcFrameImpl_GetFrameWindow();
	pWndFrameImpl->ModifyStyle( 0, WS_CAPTION|WS_BORDER, 0 );
	m_pNcFrameImpl->m_bNcFrameImpl_IsActive = false;
	if( pWndFrameImpl != NULL && pWndFrameImpl->IsWindowEnabled() )
	{
		HWND hWndFocus = ::GetFocus();
		if(		hWndFocus != NULL
			&&	(	pWndFrameImpl->m_hWnd == hWndFocus
			||	::IsChild( pWndFrameImpl->m_hWnd, hWndFocus )
				)
			)
			m_pNcFrameImpl->m_bNcFrameImpl_IsActive = true;
	}
DWORD dwInitialStyle = m_pNcFrameImpl->NcFrameImpl_GetInitialStyle();
DWORD dwCurrentStyle = dwInitialStyle; // ::__EXT_MFC_GetWindowLong( hWndOwn, GWL_STYLE );

HWND hWndSurface = NULL;
	if(		(	(dwCurrentStyle&(WS_CHILD|WS_VISIBLE)) == WS_VISIBLE
			&&	(! m_pNcFrameImpl->m_bNcFrameImpl_PivotPmSyncMode )
//			&&	(! m_pNcFrameImpl->NcFrameImpl_IsDwmCaptionReplacement() )
			)
		||	(	pWndFrameImpl != NULL
			&&	pWndFrameImpl->IsKindOf( RUNTIME_CLASS(CMDIChildWnd) )
			)
		)
	{
		CRect rcSurface;
		::GetWindowRect( hWndOwn, &rcSurface );
		hWndSurface =
			::CreateWindowEx(
				0, _T("Static"), _T(""), WS_POPUP,
				rcSurface.left,
				rcSurface.top,
				rcSurface.Width()
					+ (		(	_wp.showCmd == SW_SHOWNORMAL
							&&	g_PaintManager.m_bIsWinXPorLater
							) ? 1 : 0  // +1 for recomputing Window HRGN on WindowsXP or Later
						),
				rcSurface.Height(),
				hWndOwn, (HMENU)NULL, ::AfxGetInstanceHandle(), NULL
				);
		if( hWndSurface != NULL )
		{
			::EnableWindow( hWndSurface, FALSE );
			::ShowWindow( hWndSurface, SW_SHOWNOACTIVATE );
		} // if( hWndSurface != NULL )
	} // if( (dwCurrentStyle&(WS_CHILD|WS_VISIBLE)) == WS_VISIBLE )

CExtNcFrameImpl::NcLock _NcLock( * m_pNcFrameImpl );
	m_pNcFrameImpl->m_bNcFrameImpl_AssigningRGN = true;
	if(		( m_pNcFrameImpl->m_bNcFrameImpl_HelperLastThemeSupportedNC )
		&&	( ! m_pNcFrameImpl->NcFrameImpl_IsDwmCaptionReplacement() )
		)
		::SetWindowRgn( hWndOwn, NULL, TRUE );
	m_pNcFrameImpl->m_bNcFrameImpl_AssigningRGN = false;
	if( ! ::IsWindow( hWndOwn ) )
		return;
	_NcLock.UnlockNow();
	if( m_pNcFrameImpl->NcFrameImpl_IsDwmCaptionReplacement() )
		m_pNcFrameImpl->NcFrameImpl_AdjustVistaDwmCompatibilityIssues();
	else
		m_pNcFrameImpl->NcFrameImpl_SetupRgn();
	if(		pWndFrameImpl != NULL
		&&	pWndFrameImpl->IsKindOf( RUNTIME_CLASS(CMDIChildWnd) )
		&&	g_PaintManager.m_UxTheme.IsAppThemed()
		)
	{
		CRect rc;
		pWndFrameImpl->GetWindowRect( &rc );
		pWndFrameImpl->GetParent()->ScreenToClient( &rc );
		rc.left ++;
		pWndFrameImpl->MoveWindow( &rc, FALSE );
		rc.left --;
		pWndFrameImpl->MoveWindow( &rc, FALSE );
	}

	m_pNcFrameImpl->NcFrameImpl_RecalcNcFrame();
CWnd * pWndPermanent = CWnd::FromHandlePermanent( hWndOwn );
	if( pWndPermanent != NULL )
	{
		CFrameWnd * pFrameWnd = DYNAMIC_DOWNCAST( CFrameWnd, pWndPermanent );
		if( pFrameWnd != NULL )
			pFrameWnd->RecalcLayout();
	}

	if(		m_pNcFrameImpl->NcFrameImpl_IsSupported()
		&&	( ! m_pNcFrameImpl->NcFrameImpl_IsDwmCaptionReplacement() )
		)
	{
		m_pNcFrameImpl->NcFrameImpl_SetupRgn( NULL );
		::RedrawWindow( hWndOwn, NULL, NULL, RDW_INVALIDATE|RDW_ERASE|RDW_FRAME|RDW_ALLCHILDREN );
	} // if( m_pNcFrameImpl->NcFrameImpl_IsSupported() ...
	else
	{
		m_pNcFrameImpl->NcFrameImpl_RecalcNcFrame();
		if( (dwCurrentStyle&(WS_CHILD|WS_VISIBLE)) == WS_VISIBLE )
		{
			switch( _wp.showCmd )
			{
			case SW_SHOWNORMAL:
				if( g_PaintManager.m_bIsWinXPorLater )
				{
					CRect rc = _wp.rcNormalPosition;
					rc.right++;
					MoveWindow( hWndOwn, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE );
					rc.right--;
					MoveWindow( hWndOwn, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE );
				} // if( g_PaintManager.m_bIsWinXPorLater )
			break;
			} // switch( _wp.showCmd )
		} // if( (dwCurrentStyle&(WS_CHILD|WS_VISIBLE)) == WS_VISIBLE )
	} // else from if( m_pNcFrameImpl->NcFrameImpl_IsSupported() ...
	if( m_pNcFrameImpl->NcFrameImpl_IsDwmCaptionReplacement() )
		m_pNcFrameImpl->NcFrameImpl_RecalcNcFrame();
	else if( m_pNcFrameImpl->NcFrameImpl_IsSupported() )
		CExtNcFrameImpl::stat_SetWindowPlacement( hWndOwn, _wp );

bool bContinueRecalc = true;
	while( bContinueRecalc )
	{
		bContinueRecalc = false;
		MSG _msg;
		while( ::PeekMessage( &_msg, NULL, WM_WINDOWPOSCHANGING, WM_WINDOWPOSCHANGED, PM_NOREMOVE ) )
		{
			if( ! ::GetMessage( &_msg, NULL, WM_WINDOWPOSCHANGING, WM_WINDOWPOSCHANGED ) )
				break;
			::DispatchMessage( &_msg );
			bContinueRecalc = true;
		}
		while( ::PeekMessage( &_msg, NULL, WM_NCCALCSIZE, WM_NCCALCSIZE, PM_NOREMOVE ) )
		{
			if( ! ::GetMessage( &_msg, NULL, WM_NCCALCSIZE, WM_NCCALCSIZE ) )
				break;
			::DispatchMessage( &_msg );
			bContinueRecalc = true;
		}
		while( ::PeekMessage( &_msg, NULL, WM_WINDOWPOSCHANGING, WM_WINDOWPOSCHANGED, PM_NOREMOVE ) )
		{
			if( ! ::GetMessage( &_msg, NULL, WM_WINDOWPOSCHANGING, WM_WINDOWPOSCHANGED ) )
				break;
			::DispatchMessage( &_msg );
			bContinueRecalc = true;
		}
		while( ::PeekMessage( &_msg, NULL, WM_SIZE, WM_SIZE, PM_NOREMOVE ) )
		{
			if( ! ::GetMessage( &_msg, NULL, WM_SIZE, WM_SIZE ) )
				break;
			::DispatchMessage( &_msg );
			bContinueRecalc = true;
		}
	} // while( bContinueRecalc )
	if( hWndSurface != NULL )
	{
		::DestroyWindow( hWndSurface );
		CExtPaintManager::stat_PassPaintMessages();
	}

	if( ! m_pNcFrameImpl->NcFrameImpl_IsDwmBased() )
	{
		HWND hWndCheckZoomState = m_pNcFrameImpl->NcFrameImpl_OnQueryHWND();
		if(		hWndCheckZoomState != NULL
			&&	::IsWindow( hWndCheckZoomState )
			&&	::IsZoomed( hWndCheckZoomState )
//			&&	(! m_pNcFrameImpl->NcFrameImpl_IsSupported() )
			)
		{
			CRect rcCurrentWnd;
			::GetWindowRect( hWndCheckZoomState, &rcCurrentWnd );
			CPoint ptMaxPosCurrent = rcCurrentWnd.TopLeft();
			CExtPaintManager::monitor_parms_t _mp;
			CExtPaintManager::stat_GetMonitorParms( _mp, hWndCheckZoomState );
			::SendMessage( hWndCheckZoomState, WM_SETREDRAW, 0L, 0L );
			MINMAXINFO _mmi;
			::memset( &_mmi, 0, sizeof(MINMAXINFO) );
			_mmi.ptMaxPosition.x = _mp.m_rcWorkArea.left;
			_mmi.ptMaxPosition.y = _mp.m_rcWorkArea.top;
			_mmi.ptMaxTrackSize.x = _mp.m_rcWorkArea.Width();
			_mmi.ptMaxTrackSize.y = _mp.m_rcWorkArea.Height();
			_mmi.ptMinTrackSize.x = ::GetSystemMetrics( SM_CXMINTRACK );
			_mmi.ptMinTrackSize.y = ::GetSystemMetrics( SM_CYMINTRACK );
			_mmi.ptMaxSize.x = _mmi.ptMaxTrackSize.x;
			_mmi.ptMaxSize.y = _mmi.ptMaxTrackSize.y;
			::SendMessage( hWndCheckZoomState, WM_GETMINMAXINFO, 0, LPARAM(&_mmi) );
			CSize maxSize(
				max( _mmi.ptMaxTrackSize.x, _mp.m_rcWorkArea.Width() ),
				max( _mmi.ptMaxTrackSize.y, _mp.m_rcWorkArea.Height() )
				);
			CRect rcMax( /*_mmi.ptMaxPosition*/ ptMaxPosCurrent, maxSize );
			WINDOWPLACEMENT _wp, _wp2;
			::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
			_wp.length = sizeof(WINDOWPLACEMENT);
			::GetWindowPlacement( hWndCheckZoomState, &_wp );
			::memcpy( &_wp2, &_wp, sizeof(WINDOWPLACEMENT) );
			CRect rcMaxInflated( rcMax );
			if( m_pNcFrameImpl->NcFrameImpl_IsSupported() )
			{
				CRect rcInflate, rcTmp;
				PmBridge_GetPM()->NcFrame_GetMetrics( rcInflate, rcTmp, pWndFrameImpl );
				rcMaxInflated.InflateRect( rcInflate );
			}
			else
				rcMaxInflated.InflateRect( ::GetSystemMetrics(SM_CXFRAME), ::GetSystemMetrics(SM_CYFRAME) );
			_wp2.ptMaxPosition = rcMaxInflated.TopLeft();
			::CopyRect( &_wp2.rcNormalPosition, &rcMaxInflated );
			_wp2.showCmd = SW_SHOWMAXIMIZED;
			::SetWindowPlacement( hWndCheckZoomState, &_wp2 );
			_wp.showCmd = SW_SHOWMAXIMIZED;
			::SendMessage( hWndCheckZoomState, WM_SETREDRAW, 1L, 0L );
			::SetWindowPlacement( hWndCheckZoomState, &_wp );
		}
		m_pNcFrameImpl->m_bNcFrameImpl_InAdjustAdjustThemeSettings_NoRgnSetup = false;
		m_pNcFrameImpl->NcFrameImpl_SetupRgn();
	}

	m_pNcFrameImpl->m_bNcFrameImpl_InAdjustAdjustThemeSettings = false;
	m_pNcFrameImpl->m_bNcFrameImpl_InAdjustAdjustThemeSettings_NoRgnSetup = false;
	m_pNcFrameImpl->m_bNcFrameImpl_HelperLastThemeSupportedNC = m_pNcFrameImpl->NcFrameImpl_IsSupported();
}

CExtNcFrameImpl::NcLock::NcLock( const CExtNcFrameImpl & _NcFrameImpl )
	: m_NcFrameImpl( _NcFrameImpl )
	, m_bLocked( true )
{
	m_NcFrameImpl.NcFrameImpl_NcLock( true );
}

CExtNcFrameImpl::NcLock::~NcLock()
{
	UnlockNow();
}

void CExtNcFrameImpl::NcLock::UnlockNow()
{
	if( ! m_bLocked )
		return;
	m_bLocked = false;
	m_NcFrameImpl.NcFrameImpl_NcLock( false );
}

const UINT CExtNcFrameImpl::g_nMsgFindExtNcFrameImpl =
	::RegisterWindowMessage(
		_T("CExtNcFrameImpl::g_nMsgFindExtNcFrameImpl")
		);

CExtNcFrameImpl::CExtNcFrameImpl()
	: m_bNcFrameImpl_PivotPmSyncMode( false )
	, m_bNcFrameImpl_IsActive( false )
	, m_bNcFrameImpl_RgnSet( false )
	, m_bNcFrameImpl_RestoreBorder( false )
	, m_bNcFrameImpl_DelatayedFrameRecalc( false )
	, m_bNcFrameImpl_IsEnabled( true )
	, m_bNcFrameImpl_IsDwmBased( true )
	, m_bNcFrameImpl_Resizing( true )
	, m_bNcFrameImpl_Moving( true )
	, m_bNcFrameImpl_ResizingLoopTracking( false )
	, m_bNcFrameImpl_MovingLoopTracking( false )
	, m_bNcFrameImpl_QuickWindowPlacement( false )
	, m_rcNcFrameImpl_ScClose( 0, 0, 0, 0 )
	, m_rcNcFrameImpl_ScMaximize( 0, 0, 0, 0 )
	, m_rcNcFrameImpl_ScMinimize( 0, 0, 0, 0 )
	, m_rcNcFrameImpl_ScHelp( 0, 0, 0, 0 )
	, m_rcNcFrameImpl_Text( 0, 0, 0, 0 )
	, m_rcNcFrameImpl_Icon( 0, 0, 0, 0 )
	, m_nNcFrameImpl_ScTrackedButtonHover( 0 )
	, m_nNcFrameImpl_ScTrackedButtonPressed( 0 )
	, m_strNcFrameImpl_TipMinimize( _T("") )
	, m_strNcFrameImpl_TipMaximize( _T("") )
	, m_strNcFrameImpl_TipRestore( _T("") )
	, m_strNcFrameImpl_TipClose( _T("") )
	, m_strNcFrameImpl_TipHelp( _T("") )
	, m_dwNcFrameImpl_StyleInitial( 0 )
	, m_dwNcFrameImpl_StyleExInitial( 0 )
	, m_nNcFrameImpl_Lock( 0 )
	, m_nNcFrameImpl_LastCheckCursorHT( HTNOWHERE )
	, m_ptNcFrameImpl_LastCheckCursor( -32767, -32767 )
	, m_pNcFrameImplBridge( NULL )
	, m_bNcFrameImpl_HelperInsideNcHitTest( false )
	, m_bNcFrameImpl_RestoreEnabledState( false )
	, m_nNcFrameImpl_LastShowCmd( SW_HIDE )
	, m_bNcFrameImpl_NcInitPassed( false )
	, m_nNcFrameImpl_LastExternalNcHT( UINT(-1) )
	, m_rcNcFrameImpl_LastExternalNcHT( -1, -1, -1, -1 )
	, m_bNcFrameImpl_DwmSurfaceInitializationComplete( false )
	, m_bNcFrameImpl_CreatePassed( false )
	, m_bNcFrameImpl_AssigningRGN( false )
	, m_bNcFrameImpl_DestroyPassed( false )
	, m_bNcFrameImpl_InAdjustAdjustThemeSettings( false )
	, m_bNcFrameImpl_InAdjustAdjustThemeSettings_NoRgnSetup( false )
	, m_bNcFrameImpl_HelperLastThemeSupportedNC( false )
{
	::memset( &m_NcFrameImpl_MinMaxInfo_Cache, 0, sizeof(MINMAXINFO) );
	m_BridgeNC.NcFrameImpl_Set( this );
	m_wndNcFrameImpl_Tip.SetTipStyle( CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON );
	m_wndNcFrameImpl_Tip.SetShadowSize( 0 );
	if( g_PaintManager.m_bIsWin7OrLater )
		m_bNcFrameImpl_Moving = false;
}

CExtNcFrameImpl::~CExtNcFrameImpl()
{
	m_wndNcFrameImpl_Tip.Hide();
	if( m_wndNcFrameImpl_Tip.GetSafeHwnd() != NULL )
		m_wndNcFrameImpl_Tip.DestroyWindow();
	m_BridgeNC.NcFrameImpl_Set( NULL );
	NcFrameImpl_MapHtRects_Clean();
}

void CExtNcFrameImpl::NcFrameImpl_SyncThemeSettings( CExtPaintManager * pPM, bool bPostNotification )
{
	ASSERT( this != NULL );
	pPM;
	bPostNotification;
}

bool CExtNcFrameImpl::NcFrameImpl_GetWindowPlacement( WINDOWPLACEMENT & _wp ) const
{
	ASSERT( this != NULL );
CWnd * pWndFrameImpl = (CWnd *)NcFrameImpl_GetFrameWindow();
	if( pWndFrameImpl->GetSafeHwnd() == NULL )
		return false;
	ASSERT_VALID( pWndFrameImpl );
	::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
	_wp.length = sizeof(WINDOWPLACEMENT);
bool bRetVal = ::GetWindowPlacement( pWndFrameImpl->m_hWnd, &_wp ) ? true : false;
	return bRetVal;
}

bool CExtNcFrameImpl::NcFrameImpl_SetWindowPlacement( const WINDOWPLACEMENT & _wp )
{
	ASSERT( this != NULL );
CWnd * pWndFrameImpl = (CWnd *)NcFrameImpl_GetFrameWindow();
	if( pWndFrameImpl->GetSafeHwnd() == NULL )
		return false;
bool bRetVal = ::SetWindowPlacement( pWndFrameImpl->m_hWnd, &_wp ) ? true : false;
	return bRetVal;
}

BOOL CExtNcFrameImpl::GetWindowPlacement( WINDOWPLACEMENT * pWP ) const
{
	ASSERT( this != NULL );
	if( pWP == NULL )
		return FALSE;
	return NcFrameImpl_GetWindowPlacement( *pWP ) ? TRUE : FALSE;
}

BOOL CExtNcFrameImpl::GetWindowPlacement( WINDOWPLACEMENT & _wp ) const
{
	ASSERT( this != NULL );
	return NcFrameImpl_GetWindowPlacement( _wp ) ? TRUE : FALSE;
}

BOOL CExtNcFrameImpl::SetWindowPlacement( const WINDOWPLACEMENT * pWP )
{
	ASSERT( this != NULL );
	if( pWP == NULL )
		return FALSE;
	return NcFrameImpl_SetWindowPlacement( *pWP ) ? TRUE : FALSE;
}

BOOL CExtNcFrameImpl::SetWindowPlacement( const WINDOWPLACEMENT & _wp )
{
	ASSERT( this != NULL );
	return NcFrameImpl_SetWindowPlacement( _wp ) ? TRUE : FALSE;
}

bool CExtNcFrameImpl::stat_GetWindowPlacement( HWND hWnd, WINDOWPLACEMENT & _wp )
{
	if( hWnd == NULL || ( ! ::IsWindow( hWnd ) ) )
		return false;
bool bRetVal = false;
	::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
	_wp.length = sizeof(WINDOWPLACEMENT);
	bRetVal = ::GetWindowPlacement( hWnd, &_wp ) ? true : false;
	return bRetVal;
}

bool CExtNcFrameImpl::stat_SetWindowPlacement( HWND hWnd, const WINDOWPLACEMENT & _wp )
{
	if( hWnd == NULL || ( ! ::IsWindow( hWnd ) ) )
		return false;
bool bRetVal = false;
	bRetVal = ::SetWindowPlacement( hWnd, &_wp ) ? true : false;
	return bRetVal;
}

void CExtNcFrameImpl::NcFrameImpl_CalcWindowRect(
	LPRECT lpClientRect,
	UINT nAdjustType // = UINT(CWnd::adjustBorder)
	)
{
	ASSERT( this != NULL );
	ASSERT( lpClientRect != NULL );
CWnd * pWndFrameImpl = (CWnd *)NcFrameImpl_GetFrameWindow();
	ASSERT_VALID( pWndFrameImpl );
	if( NcFrameImpl_IsSupported() && ( ! NcFrameImpl_IsDwmBased() ) )
	{
		CExtPaintManager * pPM = NcFrameImpl_GetPM();
		ASSERT_VALID( pPM );
		CRect rcInflate, rcTmp;
		pPM->NcFrame_GetMetrics( rcInflate, rcTmp, pWndFrameImpl );
		bool bFrameActive = NcFrameImpl_IsActive();
		INT nAdjustCaptionHeight = 0;
//		if(		m_pNcFrameImplBridge != NULL
//			&&	m_pNcFrameImplBridge->NcFrameImplBridge_OnQueryCaptionMergeMode()
//			)
		{
			nAdjustCaptionHeight =
				pPM->NcFrame_GetCaptionHeight(
					bFrameActive,
					pWndFrameImpl
					);
			rcInflate.top += nAdjustCaptionHeight;
		}
		lpClientRect->top     -=  rcInflate.top;
		lpClientRect->bottom  +=  rcInflate.bottom;
		lpClientRect->left    -=  rcInflate.left;
		lpClientRect->right   +=  rcInflate.right;
	}
	else
	{
		DWORD dwExStyle = pWndFrameImpl->GetExStyle();
		if( nAdjustType == 0 )
			dwExStyle &= ~WS_EX_CLIENTEDGE;
		::AdjustWindowRectEx( lpClientRect, pWndFrameImpl->GetStyle(), FALSE, dwExStyle );
	}
}

bool CExtNcFrameImpl::stat_SaveWindowRect(
	HWND hWnd,
	__EXT_MFC_SAFE_LPCTSTR strSection,
	__EXT_MFC_SAFE_LPCTSTR strEntry
	)
{
	if(		LPCTSTR( strSection ) == NULL
		||	LPCTSTR( strEntry ) == NULL
		||	_tcslen( LPCTSTR( strSection ) ) == 0
		||	_tcslen( LPCTSTR( strEntry ) ) == 0
		||	hWnd == NULL
		|| ( ! ::IsWindow( hWnd ) )
		)
		return false;
CExtSafeString strWP;
WINDOWPLACEMENT _wp;
	::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
	_wp.length = sizeof(WINDOWPLACEMENT);
	CExtControlBar::stat_GetWindowPlacement( hWnd, _wp );
RECT & rc = _wp.rcNormalPosition;
	strWP.Format( _T("%d,%d,%d,%d,%d,%d"), rc.left, rc.top, rc.right, rc.bottom, _wp.showCmd, _wp.flags );
	::AfxGetApp()->WriteProfileString( strSection, strEntry, __EXT_MFC_SAFE_LPCTSTR( strWP ) );
	return true;
}

bool CExtNcFrameImpl::stat_LoadWindowRect(
	HWND hWnd,
	__EXT_MFC_SAFE_LPCTSTR strSection,
	__EXT_MFC_SAFE_LPCTSTR strEntry,
	bool bForceHideWindow // = false
	)
{
	if(		LPCTSTR( strSection ) == NULL
		||	LPCTSTR( strEntry ) == NULL
		||	_tcslen( LPCTSTR( strSection ) ) == 0
		||	_tcslen( LPCTSTR( strEntry ) ) == 0
		||	hWnd == NULL
		|| ( ! ::IsWindow( hWnd ) )
		)
		return false;
CExtSafeString strWP;
WINDOWPLACEMENT _wp;
	strWP = ::AfxGetApp()->GetProfileString( strSection, strEntry );
	if( strWP.IsEmpty() )
		return false;
	::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
	_wp.length = sizeof(WINDOWPLACEMENT);
	CExtControlBar::stat_GetWindowPlacement( hWnd, _wp );
	RECT & rc = _wp.rcNormalPosition;
	if( __EXT_MFC_STSCANF(
			__EXT_MFC_SAFE_LPCTSTR( strWP ),
			__EXT_MFC_SAFE_LPCTSTR( _T("%d,%d,%d,%d,%d,%d") ),
			&rc.left, 
			&rc.top, 
			&rc.right, 
			&rc.bottom,
			&_wp.showCmd,
			&_wp.flags
			) == 6
		)
	{
		if( bForceHideWindow )
			_wp.showCmd = SW_HIDE;
		CExtControlBar::stat_SetWindowPlacement( hWnd, _wp );
	}
	::RedrawWindow( hWnd, NULL, NULL, RDW_INVALIDATE|RDW_ERASE|RDW_UPDATENOW|RDW_ALLCHILDREN|RDW_FRAME );
	return true;
}

void CExtNcFrameImpl::stat_MDI_GetWindowList(
	HWND hWndMdiClient,
	CList < HWND, HWND > & _list,
	CList < HWND, HWND > & _listWindowsToSkip,
	bool bAddActiveToTail,
	bool bIncludeEnabled,
	bool bIncludeDisabled,
	bool bIncludeIconic,
	bool bIncludeNonIconic,
	bool bIncludeWithOwner,
	bool bIncludeWithoutOwner,
	bool bIncludeVisible, // = true
	bool bIncludeHidden // = false
	)
{
	ASSERT( hWndMdiClient != NULL && ::IsWindow( hWndMdiClient ) );
	_list.RemoveAll();
BOOL bMax = FALSE;
HWND hWndActiveMdiChildFrame = (HWND) ::SendMessage( hWndMdiClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMax );
HWND hWnd = ::GetWindow( hWndMdiClient, GW_CHILD );
	for( ; hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
	{
		bool bAdd = false;
		bool bEnabled = ::IsWindowEnabled( hWnd ) ? true : false;
		if(			  bEnabled  && bIncludeEnabled )
			bAdd = true;
		else if(	(!bEnabled) && bIncludeDisabled )
			bAdd = true;
		if( ! bAdd )
		{
			if( hWnd == hWndActiveMdiChildFrame )
				hWndActiveMdiChildFrame = NULL;
			continue;
		}

		bAdd = false;
		bool bIconic = ::IsIconic( hWnd ) ? true : false;
		if(			  bIconic   && bIncludeIconic )
			bAdd = true;
		else if(	(!bIconic)  && bIncludeNonIconic )
			bAdd = true;
		if( ! bAdd )
		{
			if( hWnd == hWndActiveMdiChildFrame )
				hWndActiveMdiChildFrame = NULL;
			continue;
		}

		bAdd = false;
		__EXT_MFC_LONG_PTR dwStyle = ::__EXT_MFC_GetWindowLong( hWnd, GWL_STYLE );
		bool bVisible = ( ( dwStyle & WS_VISIBLE ) != 0 ) ? true : false;
		if(			  bVisible  && bIncludeVisible )
			bAdd = true;
		else if(	(!bVisible) && bIncludeHidden )
			bAdd = true;
		if( ! bAdd )
		{
			if( hWnd == hWndActiveMdiChildFrame )
				hWndActiveMdiChildFrame = NULL;
			continue;
		}

		bAdd = false;
		HWND hWndOwner = ::GetWindow( hWnd, GW_OWNER );
		if(		 bIncludeWithOwner    && ( hWndOwner != NULL ) )
			bAdd = true;
		else if( bIncludeWithoutOwner && ( hWndOwner == NULL ) )
			bAdd = true;
		if( ! bAdd )
		{
			if( hWnd == hWndActiveMdiChildFrame )
				hWndActiveMdiChildFrame = NULL;
			continue;
		}

		bAdd = true;
		if( bAddActiveToTail && hWndActiveMdiChildFrame == hWnd )
		{
			if( bAdd )
				bAdd = false;
			else
				hWndActiveMdiChildFrame = NULL;
		}
		if( ! bAdd )
			continue;

		if(		bAdd
			&&	_listWindowsToSkip.Find( hWnd )
			)
			bAdd = false;
		if( ! bAdd )
		{
			if( hWnd == hWndActiveMdiChildFrame )
				hWndActiveMdiChildFrame = NULL;
			continue;
		}

		if( bAdd )
		{
			TCHAR szCompare[512] = _T("");
			::GetClassName(
				hWnd,
				szCompare,
				sizeof( szCompare )/sizeof( szCompare[0] )
				);
			if( _tcsicmp( szCompare, _T("Static") ) == 0 )
				bAdd = false;
		}
		if( ! bAdd )
		{
			if( hWnd == hWndActiveMdiChildFrame )
				hWndActiveMdiChildFrame = NULL;
			continue;
		}

		_list.AddTail( hWnd );
	}
	if( bAddActiveToTail && hWndActiveMdiChildFrame != NULL )
		_list.AddTail( hWndActiveMdiChildFrame );
}

INT CExtNcFrameImpl::stat_MDI_ArrangeIconic(
	HWND hWndMdiClient,
	CList < HWND, HWND > & _listWindowsToSkip
	)
{
	ASSERT( hWndMdiClient != NULL && ::IsWindow( hWndMdiClient ) );
CList < HWND, HWND > _list;
	stat_MDI_GetWindowList(
		hWndMdiClient,
		_list,
		_listWindowsToSkip,
		false, // bAddActiveToTail
		true,  // bIncludeEnabled
		true,  // bIncludeDisabled
		true,  // bIncludeIconic
		false, // bIncludeNonIconic
		false, // bIncludeWithOwner,
		true   // bIncludeWithoutOwner,
		);
INT nRetVal = stat_MDI_ArrangeIconicEWL( hWndMdiClient, _list );
	return nRetVal;
}

INT CExtNcFrameImpl::stat_MDI_ArrangeIconicEWL( // explicit window list based version
	HWND hWndMdiClient,
	CList < HWND, HWND > & _list
	)
{
	ASSERT( hWndMdiClient != NULL && ::IsWindow( hWndMdiClient ) );
INT nIndex, nCount = INT( _list.GetCount() );
	if( nCount == 0 )
		return 0;
INT nCaptionHeight = -1;
INT nBorderHeightX2 = -1;
CExtPaintManager * pPM = g_PaintManager.GetPM();
	ASSERT_VALID( pPM );
POSITION pos = _list.GetHeadPosition();
	if( pos != NULL )
	{
		HWND hWnd = _list.GetNext( pos );
		ASSERT( hWnd != NULL && ::IsWindow( hWnd ) );
		CWnd * pWnd = CWnd::FromHandle( hWnd );
		nCaptionHeight = pPM->NcFrame_GetCaptionHeight( true, pWnd );
		CRect rcNcBorders, rcThemePadding;
		pPM->NcFrame_GetMetrics( rcNcBorders, rcThemePadding, pWnd );
		nBorderHeightX2 = rcNcBorders.top + rcNcBorders.bottom;
	}
	if( nCaptionHeight <= 0 )
		nCaptionHeight = ::GetSystemMetrics( SM_CYCAPTION );
	nBorderHeightX2 = ::GetSystemMetrics( SM_CYBORDER ) * 2;
CSize sizeItem(
		::GetSystemMetrics( SM_CXSMICON )
			+ ::GetSystemMetrics( SM_CXBORDER ) * 2
			+ 142
			,
		nCaptionHeight + nBorderHeightX2
		);
CSize sizeSpacing( 0, 0 );
CRect rcClient;
    GetClientRect( hWndMdiClient, &rcClient );
CSize sizeClient = rcClient.Size();
INT nCountInRow = sizeClient.cx / ( sizeItem.cx + sizeSpacing.cx );
HDWP hDWP = ::BeginDeferWindowPos( nCount );
	pos = _list.GetHeadPosition();
    for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		ASSERT( pos != NULL );
		HWND hWnd = _list.GetNext( pos );
		ASSERT( hWnd != NULL && ::IsWindow( hWnd ) );
		INT nRow = nIndex / nCountInRow;
		INT nColumn = nIndex % nCountInRow;
		hDWP =
			::DeferWindowPos(
				hDWP, hWnd, NULL,
				nColumn * ( sizeSpacing.cx + sizeItem.cx ),
				sizeClient.cy - ( nRow + 1) * ( sizeSpacing.cy + sizeItem.cy ),
				sizeItem.cx, sizeItem.cy,
				SWP_NOZORDER|SWP_NOACTIVATE
				);
	}
	::EndDeferWindowPos( hDWP );
INT nRetVal = sizeItem.cy + sizeSpacing.cy;
	return nRetVal;
}

void CExtNcFrameImpl::stat_MDI_Cascade(
	HWND hWndMdiClient,
	CList < HWND, HWND > & _listWindowsToSkip,
	WPARAM nActionFlags,
	INT nStairWindowCount // = 0 // zero or negative - compute automatically
	)
{
	ASSERT( hWndMdiClient != NULL && ::IsWindow( hWndMdiClient ) );
CList < HWND, HWND > _list;
	stat_MDI_GetWindowList(
		hWndMdiClient,
		_list,
		_listWindowsToSkip,
		true,  // bAddActiveToTail
		true,  // bIncludeEnabled
		( ( nActionFlags & MDITILE_SKIPDISABLED ) == 0 ) ? true : false, // bIncludeDisabled
		false, // bIncludeIconic
		true,  // bIncludeNonIconic
		false, // bIncludeWithOwner,
		true   // bIncludeWithoutOwner,
		);
	stat_MDI_CascadeEWL( hWndMdiClient, _list, nStairWindowCount );
}

void CExtNcFrameImpl::stat_MDI_CascadeEWL( // explicit window list based version
	HWND hWndMdiClient,
	CList < HWND, HWND > & _list,
	INT nStairWindowCount // = 0 // zero or negative - compute automatically
	)
{
	ASSERT( hWndMdiClient != NULL && ::IsWindow( hWndMdiClient ) );
INT nIndex, nCount = INT( _list.GetCount() );
	if( nCount == 0 )
		return;
	if( nStairWindowCount <= 0 )
		nStairWindowCount = 7;
    ShowScrollBar( hWndMdiClient, SB_BOTH, FALSE );
INT nStepX = ::GetSystemMetrics( SM_CYCAPTION ) - ::GetSystemMetrics( SM_CYFRAME );
INT nStepY = ::GetSystemMetrics( SM_CYCAPTION ) + ::GetSystemMetrics( SM_CYFRAME ) - 1;
CRect rcClient;
    GetClientRect( hWndMdiClient, &rcClient );
//	rcClient.bottom -= stat_MDI_ArrangeIconicEWL( hWndMdiClient, _list );
CList < HWND, HWND > _listWindowsToSkip;
	rcClient.bottom -= stat_MDI_ArrangeIconic( hWndMdiClient, _listWindowsToSkip );
CSize sizeClient = rcClient.Size();
CSize sizeChild( sizeClient.cx - nStepX * nStairWindowCount, sizeClient.cy - nStepY * nStairWindowCount );
HDWP hDWP = ::BeginDeferWindowPos( nCount );
POSITION pos = _list.GetHeadPosition();
    for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		ASSERT( pos != NULL );
		HWND hWnd = _list.GetNext( pos );
		ASSERT( hWnd != NULL && ::IsWindow( hWnd ) );
		INT nDI = nIndex % nStairWindowCount;
		hDWP =
			::DeferWindowPos(
				hDWP, hWnd, HWND_TOP,
				nDI * nStepX, nDI * nStepY, sizeChild.cx, sizeChild.cy,
				SWP_NOACTIVATE|SWP_FRAMECHANGED
				);
    }
	::EndDeferWindowPos( hDWP );
}

void CExtNcFrameImpl::stat_MDI_Tile(
	HWND hWndMdiClient,
	CList < HWND, HWND > & _listWindowsToSkip,
	WPARAM nActionFlags
	)
{
	ASSERT( hWndMdiClient != NULL && ::IsWindow( hWndMdiClient ) );
CList < HWND, HWND > _list;
	stat_MDI_GetWindowList(
		hWndMdiClient,
		_list,
		_listWindowsToSkip,
		true,  // bAddActiveToTail
		true,  // bIncludeEnabled
		( ( nActionFlags & MDITILE_SKIPDISABLED ) == 0 ) ? true : false, // bIncludeDisabled
		false, // bIncludeIconic
		true,  // bIncludeNonIconic
		false, // bIncludeWithOwner,
		true   // bIncludeWithoutOwner,
		);
bool bHorizontal = ( ( nActionFlags & MDITILE_HORIZONTAL ) != 0 ) ? true : false;
	stat_MDI_TileEWL( hWndMdiClient, _list, bHorizontal );
}

void CExtNcFrameImpl::stat_MDI_TileEWL( // explicit window list based version
	HWND hWndMdiClient,
	CList < HWND, HWND > & _list,
	bool bHorizontal
	)
{
	ASSERT( hWndMdiClient != NULL && ::IsWindow( hWndMdiClient ) );
INT nCount = INT( _list.GetCount() );
	if( nCount == 0 )
		return;
    ShowScrollBar( hWndMdiClient, SB_BOTH, FALSE );
CRect rcClient;
    GetClientRect( hWndMdiClient, &rcClient );
//	rcClient.bottom -= stat_MDI_ArrangeIconicEWL( hWndMdiClient, _list );
CList < HWND, HWND > _listWindowsToSkip;
	rcClient.bottom -= stat_MDI_ArrangeIconic( hWndMdiClient, _listWindowsToSkip );
CSize sizeClient = rcClient.Size();
INT nRest = 1;
INT nIndexX = (INT)::sqrt( (double)nCount );
INT nIndexY = nIndexX;
    if( nIndexY == 1 )
	{
		if( bHorizontal )
			nIndexY = nCount;
		else
			nIndexX = nCount;
	}
	else
	{
		for( ; true; )
		{
			nRest = nCount - nIndexY * ( nIndexX - 1 );
			if( bHorizontal )
			{
				if( ( nRest - nIndexX ) >= nIndexY )
					nIndexY ++;
				else
					break;
			}
			else
			{
				if( ( nRest - nIndexY ) >= nIndexX )
					nIndexX ++;
				else
					break;
			}
		}
		nRest = nCount - nIndexY * nIndexX;
		if( nRest < 0 )
		{
			if( bHorizontal )
				nRest = nCount - nIndexY * ( nIndexX - 1 );
			else
				nRest = nCount - nIndexX * ( nIndexY - 1 );
			ASSERT( nRest >= 0 );
		}
		else if( nRest == 0 )
			nRest = bHorizontal ? nIndexX : nIndexY;
		else if( nRest > 0 )
		{
			if( bHorizontal )
				nIndexY ++;
			else
				nIndexX ++;
		}
		if( bHorizontal )
		{
			if( nRest < nIndexX )
			{
				nRest = nIndexX + nRest;
				nIndexY --;
			}
		}
		else
		{
			if( nRest < nIndexY )
			{
				nRest = nIndexY + nRest;
				nIndexX --;
			}
		}
    }
CSize sizeChild( sizeClient.cx / nIndexX, sizeClient.cy / nIndexY );
HDWP hDWP = ::BeginDeferWindowPos( nCount );
POSITION pos = _list.GetHeadPosition();
INT nWalkIndex = 0, nX, nY;
	for( nX = 0, nY = 0; nX < nCount; nX ++ )
	{
		ASSERT( pos != NULL );
		HWND hWnd = _list.GetNext( pos );
		ASSERT( hWnd != NULL && ::IsWindow( hWnd ) );
		CPoint ptTopLeft( 0, 0 );
		if( bHorizontal )
		{
			ptTopLeft.x = ( nWalkIndex - nY * nIndexX ) * sizeChild.cx;
			ptTopLeft.y = nY * sizeChild.cy;
			if(		( ( nWalkIndex % nIndexX ) == ( nIndexX - 1 ) )
				&&	( nY < ( nIndexY - 1 ) )
				)
				nY ++;
		}
		else
		{
			ptTopLeft.x = nY * sizeChild.cx;
			ptTopLeft.y = ( nWalkIndex - nY * nIndexY ) * sizeChild.cy;
			if(		( ( nWalkIndex % nIndexY ) == ( nIndexY - 1 ) )
				&&	( nY < ( nIndexX - 1 ) )
				)
				nY ++;
		}
		hDWP =
			::DeferWindowPos(
				hDWP, hWnd, NULL,
				ptTopLeft.x, ptTopLeft.y, sizeChild.cx, sizeChild.cy,
				SWP_NOACTIVATE|SWP_NOZORDER
				);
		if( bHorizontal )
		{
			if( nY == ( nIndexY - 1 ) )
				sizeChild.cx = sizeClient.cx / nRest;
		}
		else
		{
			if( nY == ( nIndexX - 1 ) )
				sizeChild.cy = sizeClient.cy / nRest;
		}
		nWalkIndex ++;
	}
	::EndDeferWindowPos( hDWP );
}

void CExtNcFrameImpl::NcFrameImpl_MapHtRects_Clean()
{
POSITION pos = m_mapNcFrameImpl_HtRects.GetStartPosition();
	for( ; pos != NULL; )
	{
		LPVOID p1, p2;
		m_mapNcFrameImpl_HtRects.GetNextAssoc( pos, p1, p2 );
		ASSERT( p2 != NULL );
		RECT * pRect = ( RECT * ) p2;
		delete pRect;
	} // for( ; pos != NULL; )
	m_mapNcFrameImpl_HtRects.RemoveAll();
}

void CExtNcFrameImpl::NcFrameImpl_MapHtRects_SetAt( LONG nHT, const RECT & rc ) const
{
LPVOID p;
	if( m_mapNcFrameImpl_HtRects.Lookup( LPVOID(__EXT_MFC_LONG_PTR(nHT)), p ) )
	{
		ASSERT( p != NULL );
		RECT * pRect = ( RECT * ) p;
		pRect->left = rc.left;
		pRect->top = rc.top;
		pRect->right = rc.right;
		pRect->bottom = rc.bottom;
	}
	else
	{
		RECT * pRect = new RECT;
		pRect->left = rc.left;
		pRect->top = rc.top;
		pRect->right = rc.right;
		pRect->bottom = rc.bottom;
		m_mapNcFrameImpl_HtRects.SetAt( LPVOID(__EXT_MFC_LONG_PTR(nHT)), pRect );
	}
}

bool CExtNcFrameImpl::NcFrameImpl_MapHtRects_GetAt( LONG nHT, RECT & rc ) const
{
LPVOID p;
	if( m_mapNcFrameImpl_HtRects.Lookup( LPVOID(__EXT_MFC_LONG_PTR(nHT)), p ) )
	{
		ASSERT( p != NULL );
		RECT * pRect = ( RECT * ) p;
		rc.left = pRect->left;
		rc.top = pRect->top;
		rc.right = pRect->right;
		rc.bottom = pRect->bottom;
		return true;
	}
	return false;
}

bool CExtNcFrameImpl::NcFrameImpl_IsNcLocked() const
{
	ASSERT( m_nNcFrameImpl_Lock >= 0 );
	if( m_nNcFrameImpl_Lock == 0 )
		return false;
	else
		return true;
}

void CExtNcFrameImpl::NcFrameImpl_NcLock( bool bLock ) const
{
	ASSERT( m_nNcFrameImpl_Lock >= 0 );
	if( bLock )
		m_nNcFrameImpl_Lock ++;
	else
		m_nNcFrameImpl_Lock --;
	ASSERT( m_nNcFrameImpl_Lock >= 0 );
}

DWORD CExtNcFrameImpl::NcFrameImpl_GetInitialStyle() const
{
	return (DWORD)m_dwNcFrameImpl_StyleInitial;
}

DWORD CExtNcFrameImpl::NcFrameImpl_GetInitialStyleEx() const
{
	return (DWORD)m_dwNcFrameImpl_StyleExInitial;
}

HWND CExtNcFrameImpl::NcFrameImpl_OnQueryHWND() const
{
	return ( const_cast < CExtNcFrameImpl * > (this) ) -> NcFrameImpl_OnQueryHWND();
}

CWnd * CExtNcFrameImpl::NcFrameImpl_GetFrameWindow()
{
HWND hWnd = NcFrameImpl_OnQueryHWND();
	if( hWnd == NULL )
		return NULL;
	ASSERT( ::IsWindow( hWnd ) );
	return CWnd::FromHandle( hWnd );
}

const CWnd * CExtNcFrameImpl::NcFrameImpl_GetFrameWindow() const
{
	return ( const_cast < CExtNcFrameImpl * > (this) ) -> NcFrameImpl_GetFrameWindow();
}

CRect CExtNcFrameImpl::NcFrameImpl_GetNcHtRect(
	UINT nHT,
	LPMINMAXINFO pMinMaxInfo,
	LPCRECT pRectWnd // = NULL
	) const
{
	return
		NcFrameImpl_GetNcHtRect(
			nHT,
			true,
			false,
			false,
			pMinMaxInfo,
			pRectWnd
			);
}

CRect CExtNcFrameImpl::NcFrameImpl_GetNcHtRect(
	UINT nHT,
	bool bScreenMapping, // = true
	bool bLayoutBordersH, // = false
	bool bLayoutBordersV, // = false
	LPMINMAXINFO pMinMaxInfo, // = NULL
	LPCRECT pRectWnd // = NULL
	) const
{
	if( m_pNcFrameImplBridge != NULL )
	{
		CRect rc;
		if( m_pNcFrameImplBridge->
				NcFrameImpl_GetNcHtRect(
					rc,
					nHT,
					bScreenMapping,
					bLayoutBordersH,
					bLayoutBordersV,
					pMinMaxInfo,
					pRectWnd
					)
			)
			return rc;
	}
RECT rc = { 0, 0, 0, 0 };
	if( NcFrameImpl_MapHtRects_GetAt( LONG(nHT), rc ) )
	{
		if( bScreenMapping )
		{
			CRect rcWnd( 0, 0, 0, 0 );
			if( pRectWnd != NULL )
				rcWnd = (*pRectWnd);
			else
			{
 				CWnd * pWndFrameImpl = (CWnd *)NcFrameImpl_GetFrameWindow();
 				ASSERT_VALID( pWndFrameImpl );
				pWndFrameImpl->GetWindowRect( &rcWnd );
			} // else from if( pRectWnd != NULL )
			::OffsetRect( &rc, rcWnd.left, rcWnd.top );
		} // if( bScreenMapping )
	} // if( NcFrameImpl_MapHtRects_GetAt( LONG(nHT), rc ) )
	else
	{
		CExtCmdIcon _icon;
		NcFrameImpl_GetIcon( _icon );
		CWnd * pWndFrameImpl = (CWnd *)NcFrameImpl_GetFrameWindow();
		ASSERT_VALID( pWndFrameImpl );
		rc =
			NcFrameImpl_GetPM()->NcFrame_GetHtRect(
				nHT,
				bScreenMapping,
				bLayoutBordersH,
				bLayoutBordersV,
				&_icon,
				pWndFrameImpl,
				pMinMaxInfo
				);
		CRect rc2 = rc;
		if( bScreenMapping )
		{
			CRect rcWnd( 0, 0, 0, 0 );
			if( pRectWnd != NULL )
				rcWnd = (*pRectWnd);
			else
			{
 				CWnd * pWndFrameImpl = (CWnd *)NcFrameImpl_GetFrameWindow();
 				ASSERT_VALID( pWndFrameImpl );
				pWndFrameImpl->GetWindowRect( &rcWnd );
			} // else from if( pRectWnd != NULL )
			::OffsetRect( &rc, -rcWnd.left, -rcWnd.top );
		} // if( bScreenMapping )
		NcFrameImpl_MapHtRects_SetAt( LONG(nHT), rc2 );
	} // else from if( NcFrameImpl_MapHtRects_GetAt( LONG(nHT), rc1 ) )
	return rc;
}

CRect CExtNcFrameImpl::NcFrameImpl_GetNcScRect( UINT nSC ) const
{
	if( m_rcNcFrameImpl_ScClose.IsRectEmpty() )
		NcFrameImpl_ReCacheScButtonRects();
	if( m_pNcFrameImplBridge != NULL )
	{
		CRect rc;
		if( m_pNcFrameImplBridge->NcFrameImplBridge_GetNcScRect( nSC, rc ) )
		{
			switch( nSC )
			{
			case SC_CLOSE:
				m_rcNcFrameImpl_ScClose = rc;
			case SC_MAXIMIZE:
			case SC_RESTORE:
				m_rcNcFrameImpl_ScMaximize = rc;
			case SC_MINIMIZE:
				m_rcNcFrameImpl_ScMinimize = rc;
			case SC_CONTEXTHELP:
				m_rcNcFrameImpl_ScHelp = rc;
			} // switch( nSC )
			return rc;
		}
	}
	switch( nSC )
	{
	case SC_CLOSE:
		return m_rcNcFrameImpl_ScClose;
	case SC_RESTORE:
		if(		NcFrameImpl_GetFrameWindow() != NULL
			&&	NcFrameImpl_GetFrameWindow()->IsIconic()
			)
			return m_rcNcFrameImpl_ScMinimize;
	case SC_MAXIMIZE:
		return m_rcNcFrameImpl_ScMaximize;
	case SC_MINIMIZE:
		return m_rcNcFrameImpl_ScMinimize;
	case SC_CONTEXTHELP:
		return m_rcNcFrameImpl_ScHelp;
	} // switch( nSC )
	return CRect( 0, 0, 0, 0 );
}

void CExtNcFrameImpl::NcFrameImpl_GetIcon( CExtCmdIcon & _icon ) const
{
	_icon.Empty();
	if( ! m_iconNcFrameImpl_QueryCache.IsEmpty() )
	{
		_icon = m_iconNcFrameImpl_QueryCache;
		return;
	}
const CWnd * pWndFrameImpl = NcFrameImpl_GetFrameWindow();
HWND hWnd = (HWND)pWndFrameImpl->GetSafeHwnd();
	if( hWnd == NULL )
		return;
HICON hIcon = (HICON)
		::SendMessage(
			hWnd,
			WM_GETICON,
			WPARAM(ICON_SMALL),
			0
			);
	if( hIcon == NULL )
	{
		hIcon = (HICON)
			::SendMessage(
				hWnd,
				WM_GETICON,
				WPARAM(ICON_BIG),
				0
				);
		if( hIcon == NULL )
		{
			hIcon = (HICON)(__EXT_MFC_DWORD_PTR) ::__EXT_MFC_GetClassLong( hWnd, __EXT_MFC_GCL_HICONSM );
			if( hIcon == NULL )
			{
				hIcon = (HICON)(__EXT_MFC_DWORD_PTR) ::__EXT_MFC_GetClassLong( hWnd, __EXT_MFC_GCL_HICON );
			} // if( hIcon == NULL )
		} // if( hIcon == NULL )
	} // if( hIcon == NULL )
	if( hIcon == NULL )
		return;
	_icon.AssignFromHICON( hIcon, true );
CSize _sizeIcon = _icon.GetSize();
	if( _sizeIcon.cx != 16 || _sizeIcon.cy != 16 )
		_icon.Scale( CSize(16,16) );
	m_iconNcFrameImpl_QueryCache = _icon;
}

void CExtNcFrameImpl::NcFrameImpl_ReCacheScButtonRects() const
{
CWnd * pWndFrameImpl = (CWnd *)NcFrameImpl_GetFrameWindow();
	ASSERT_VALID( pWndFrameImpl );
	CExtCmdIcon _icon;
	NcFrameImpl_GetIcon( _icon );
CString strCaption;
	pWndFrameImpl->GetWindowText( strCaption );
MINMAXINFO * pMinMaxInfo = (MINMAXINFO*)(&m_NcFrameImpl_MinMaxInfo_Cache);
	NcFrameImpl_GetPM()->NcFrame_GetRects(
		&_icon,
		LPCTSTR(strCaption),
		DT_LEFT|DT_VCENTER,
		m_rcNcFrameImpl_Icon,
		m_rcNcFrameImpl_Text,
		m_rcNcFrameImpl_ScHelp,
		m_rcNcFrameImpl_ScMinimize,
		m_rcNcFrameImpl_ScMaximize,
		m_rcNcFrameImpl_ScClose,
		pWndFrameImpl,
		pMinMaxInfo
		);
	if( m_pNcFrameImplBridge != NULL )
	{
		m_pNcFrameImplBridge->NcFrameImplBridge_GetNcScRect( SC_CLOSE, m_rcNcFrameImpl_ScClose );
		m_pNcFrameImplBridge->NcFrameImplBridge_GetNcScRect( SC_MAXIMIZE, m_rcNcFrameImpl_ScMaximize );
		m_pNcFrameImplBridge->NcFrameImplBridge_GetNcScRect( SC_MINIMIZE, m_rcNcFrameImpl_ScMinimize );
		m_pNcFrameImplBridge->NcFrameImplBridge_GetNcScRect( SC_CONTEXTHELP, m_rcNcFrameImpl_ScHelp );
	} // if( m_pNcFrameImplBridge != NULL )
}

void CExtNcFrameImpl::NcFrameImpl_DelayRgnAdjustment()
{
	if( NcFrameImpl_IsDwmBased() )
		return;
	m_bNcFrameImpl_RgnSet = false;
}

void CExtNcFrameImpl::NcFrameImpl_SetupRgn(
	WINDOWPOS * pWndPos // = NULL
	)
{
	if( ! NcFrameImpl_IsSupported() )
		return;
CWnd * pWndFrameImpl = NcFrameImpl_GetFrameWindow();
	ASSERT_VALID( pWndFrameImpl );
	if( NcFrameImpl_IsDwmBased() )
	{
		m_bNcFrameImpl_AssigningRGN = true;
		pWndFrameImpl->SetWindowRgn( NULL, TRUE );
		m_bNcFrameImpl_AssigningRGN = false;
		return;
	}
	if( m_bNcFrameImpl_InAdjustAdjustThemeSettings_NoRgnSetup )
		return;
	if( NcFrameImpl_IsNcLocked() )
		return;
	m_nNcFrameImpl_LastCheckCursorHT = HTNOWHERE;
	m_ptNcFrameImpl_LastCheckCursor.x = m_ptNcFrameImpl_LastCheckCursor.y = -32767;
	m_bNcFrameImpl_RgnSet = true;
__EXT_MFC_LONG_PTR dwStyle = m_dwNcFrameImpl_StyleInitial; // pWndFrameImpl->GetStyle();
	if( (dwStyle&WS_BORDER) == 0 )
		return;
	CRect rcWnd( 0, 0, 0, 0 );
	if( pWndPos != NULL )
	{
		if( ( pWndPos->flags & SWP_NOSIZE) !=  0 )
			return;
		rcWnd.right = rcWnd.left + pWndPos->cx;
		rcWnd.bottom = rcWnd.top + pWndPos->cy;
	}
	else
	{
		pWndFrameImpl->GetWindowRect( &rcWnd );
		rcWnd.OffsetRect( -rcWnd.TopLeft() );
	}
	m_rcNcFrameImpl_ScClose.SetRectEmpty();
	m_rcNcFrameImpl_ScMaximize.SetRectEmpty();
	m_rcNcFrameImpl_ScMinimize.SetRectEmpty();
	m_rcNcFrameImpl_ScHelp.SetRectEmpty();
HRGN hRgn =
		NcFrameImpl_GetPM()->NcFrame_GenerateSkinFrameRGN(
			rcWnd,
			pWndFrameImpl
			);
	if( m_pNcFrameImplBridge != NULL )
	{

		HRGN hRgnExclude = m_pNcFrameImplBridge->NcFrameImplBridge_GetNcExcludeHRGN();
		if( hRgnExclude != NULL )
		{
			CRect rcBox( 0, 0, 0, 0 ); 
			GetRgnBox( hRgnExclude, &rcBox );
			HRGN hRgnBox = ::CreateRectRgnIndirect( &rcBox );
			::CombineRgn( hRgn, hRgn, hRgnBox, RGN_OR );
			::DeleteObject( hRgnBox );
			::CombineRgn( hRgn, hRgn, hRgnExclude, RGN_XOR );
		}
	}
	m_bNcFrameImpl_AssigningRGN = true;
	pWndFrameImpl->SetWindowRgn( hRgn, TRUE );
	m_bNcFrameImpl_AssigningRGN = false;
}


BOOL CALLBACK stat_CalcMaxResizingInfo( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData )
{
	hMonitor; hdcMonitor; dwData;
	ASSERT( lprcMonitor != NULL );
LPRECT pInfo = (LPRECT)dwData;
	ASSERT( pInfo != NULL );
	pInfo->left = min( pInfo->left, lprcMonitor->left );
	pInfo->top = min( pInfo->top, lprcMonitor->top );
	pInfo->right = max( pInfo->right, lprcMonitor->right );
	pInfo->bottom = max( pInfo->bottom, lprcMonitor->bottom );
	return TRUE;
}

bool CExtNcFrameImpl::NcFrameImpl_GetMinMaxInfo(
	LPMINMAXINFO pMMI
	) const
{
	ASSERT( pMMI != NULL );
	if( ! NcFrameImpl_IsSupported() )
		return false;
CWnd * pWndFrameImpl = const_cast < CWnd * > ( NcFrameImpl_GetFrameWindow() );
WINDOWPLACEMENT _wp;
	::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
	_wp.length = sizeof(WINDOWPLACEMENT);
	CExtNcFrameImpl::stat_GetWindowPlacement( pWndFrameImpl->m_hWnd, _wp );
CExtPaintManager::monitor_parms_t _mp;
	CExtPaintManager::stat_GetMonitorParms( _mp, _wp.rcNormalPosition );
	if( NcFrameImpl_IsDwmBased() )
	{
		APPBARDATA _data;
		::memset( &_data, 0, sizeof(APPBARDATA) );
		_data.cbSize = sizeof(APPBARDATA);
		UINT nSHR = (UINT)::SHAppBarMessage( ABM_GETSTATE, &_data );
	//	if( nSHR == (ABS_AUTOHIDE|ABS_ALWAYSONTOP) )
		if(		( nSHR & (ABS_AUTOHIDE) ) != 0
			&&	_mp.m_rcWorkArea.PtInRect( CPoint( ( _data.rc.right - _data.rc.left ) / 2, ( _data.rc.bottom - _data.rc.top ) / 2 ) )
			)
		{
			nSHR = (UINT)::SHAppBarMessage( ABM_GETTASKBARPOS, &_data );
			if( nSHR == 1 )
			{
				CSize sizeMonitor( _mp.m_rcMonitor.Size() );
				pMMI->ptMaxSize.x = min( pMMI->ptMaxSize.x, sizeMonitor.cx );
				pMMI->ptMaxSize.y = min( pMMI->ptMaxSize.y, sizeMonitor.cy );
				switch(_data.uEdge)
				{
				case ABE_TOP:
					pMMI->ptMaxPosition.y ++;
					pMMI->ptMaxSize.y --;
				break;
				case ABE_BOTTOM:
					pMMI->ptMaxSize.y --;
				break;
				case ABE_LEFT:
					pMMI->ptMaxPosition.x ++;
					pMMI->ptMaxSize.x --;
				break;
				case ABE_RIGHT:
					pMMI->ptMaxSize.x --;
				break;
				}
			} // if( nSHR == 1 )
		}
		return true;
	}
	if( ( pWndFrameImpl->GetStyle() & WS_CHILD ) == 0 )
	{
		CRect rcBorders( 0, 0, 0, 1 );
		if( _wp.showCmd == SW_SHOWMINIMIZED )
		{
			pMMI->ptMaxPosition.x = _mp.m_rcWorkArea.left + rcBorders.left;
			pMMI->ptMaxPosition.y = _mp.m_rcWorkArea.top + rcBorders.top;
		}
		else
		{
			pMMI->ptMaxPosition.x = 0 + rcBorders.left;
			pMMI->ptMaxPosition.y = 0 + rcBorders.top;
		}
		if( g_PaintManager.m_bIsWin98orLater || g_PaintManager.m_bIsWin2000orLater )
		{
			CRect rcClip( -32767, -32767, +32767, +32767 ), rcInfo( 0, 0, 0, 0 );
			if( EnumDisplayMonitors( NULL, &rcClip, stat_CalcMaxResizingInfo, LPARAM(LPVOID(&rcInfo)) ) )
			{
				pMMI->ptMaxTrackSize.x = pMMI->ptMaxSize.x = rcInfo.Width();
				pMMI->ptMaxTrackSize.y = pMMI->ptMaxSize.y = rcInfo.Height();
			}
		}

		/*pMMI->ptMaxTrackSize.x =*/ pMMI->ptMaxSize.x = _mp.m_rcWorkArea.Width() - rcBorders.left - rcBorders.right;
		/*pMMI->ptMaxTrackSize.y =*/ pMMI->ptMaxSize.y = _mp.m_rcWorkArea.Height() - rcBorders.top - rcBorders.bottom;
//TRACE2( "max-pos %d %d\r\n", pMMI->ptMaxPosition.x, pMMI->ptMaxPosition.y );
		APPBARDATA _data;
		::memset( &_data, 0, sizeof(APPBARDATA) );
		_data.cbSize = sizeof(APPBARDATA);
		UINT nSHR = (UINT)::SHAppBarMessage( ABM_GETSTATE, &_data );
		if( nSHR == (ABS_AUTOHIDE|ABS_ALWAYSONTOP) )
		{
			nSHR = (UINT)::SHAppBarMessage( ABM_GETTASKBARPOS, &_data );
			if( nSHR == 1 )
			{
				switch(_data.uEdge)
				{
				case ABE_TOP:
					pMMI->ptMaxPosition.y ++;
					pMMI->ptMaxSize.y --;
				break;
				case ABE_BOTTOM:
					pMMI->ptMaxSize.y --;
				break;
				case ABE_LEFT:
					pMMI->ptMaxPosition.x ++;
					pMMI->ptMaxSize.x --;
				break;
				case ABE_RIGHT:
					pMMI->ptMaxSize.x --;
				break;
				}
			} // if( nSHR == 1 )
		} // if( nSHR == (ABS_AUTOHIDE|ABS_ALWAYSONTOP) )
	} // if( ( pWndFrameImpl->GetStyle() & WS_CHILD ) == 0 )
	return true;
}

void CExtNcFrameImpl::NcFrameImpl_ExtractScTipText( const CWnd * pWnd, UINT nSC, CExtSafeString & strTipText )
{
	if( ! strTipText.IsEmpty() )
		return;
	if( pWnd->GetSafeHwnd() != NULL )
	{
		if( pWnd->IsIconic() && nSC == SC_MINIMIZE )
			nSC = SC_RESTORE;
		CMenu * pMenu = pWnd->GetSystemMenu( FALSE );
		if( pMenu->GetSafeHmenu() != NULL )
		{
			CString sTmp;
			pMenu->GetMenuString( nSC, sTmp, MF_BYCOMMAND );
			if( ! sTmp.IsEmpty() )
			{
				sTmp.Replace( _T("&"), _T("") );
				INT nPos = sTmp.Find( _T("\t") );
				if( nPos > 0 )
					sTmp = sTmp.Left( nPos );
				strTipText = LPCTSTR( sTmp );
				return;
			}
		}
	}
	switch( nSC )
	{
	case SC_MINIMIZE:
		strTipText = _T("Minimize");
	break;
	case SC_MAXIMIZE:
		strTipText = _T("Maximize");
	break;
	case SC_RESTORE:
		strTipText = _T("Restore");
	break;
	case SC_CLOSE:
		strTipText = _T("Close");
	break;
	case SC_CONTEXTHELP:
		strTipText = _T("Help");
	break;
	}
}

__EXT_MFC_SAFE_LPCTSTR CExtNcFrameImpl::NcFrameImpl_GetScTipText( UINT nSC ) const
{
const CWnd * pWndFrameImpl = NcFrameImpl_GetFrameWindow();
	ASSERT_VALID( pWndFrameImpl );
	if( m_rcNcFrameImpl_ScClose.IsRectEmpty() )
		NcFrameImpl_ReCacheScButtonRects();
	switch( nSC )
	{
	case SC_CLOSE:
	{
		NcFrameImpl_ExtractScTipText( pWndFrameImpl, nSC, m_strNcFrameImpl_TipClose );
		return LPCTSTR( m_strNcFrameImpl_TipClose );
	}
	case SC_MAXIMIZE:
	case SC_RESTORE:
	{
		if( pWndFrameImpl->IsZoomed() )
		{
			nSC = SC_RESTORE;
			NcFrameImpl_ExtractScTipText( pWndFrameImpl, nSC, m_strNcFrameImpl_TipRestore );
			return LPCTSTR( m_strNcFrameImpl_TipRestore );
		}
		else
		{
			nSC = SC_MAXIMIZE;
			NcFrameImpl_ExtractScTipText( pWndFrameImpl, nSC, m_strNcFrameImpl_TipMaximize );
			return LPCTSTR( m_strNcFrameImpl_TipMaximize );
		}
	}
	case SC_MINIMIZE:
	{
		if( pWndFrameImpl->IsIconic() )
		{
			NcFrameImpl_ExtractScTipText( pWndFrameImpl, nSC, m_strNcFrameImpl_TipRestore );
			return LPCTSTR( m_strNcFrameImpl_TipRestore );
		}
		else
		{
			NcFrameImpl_ExtractScTipText( pWndFrameImpl, nSC, m_strNcFrameImpl_TipMinimize );
			return LPCTSTR( m_strNcFrameImpl_TipMinimize );
		}
	}
	case SC_CONTEXTHELP:
	{
		NcFrameImpl_ExtractScTipText( pWndFrameImpl, nSC, m_strNcFrameImpl_TipHelp );
		return LPCTSTR( m_strNcFrameImpl_TipHelp );
	}
	} // switch( nSC )
	return NULL;
}

bool CExtNcFrameImpl::NcFrameImpl_IsSupported() const
{
	if( ! m_bNcFrameImpl_IsEnabled )
		return false;
const CWnd * pWndFrameImpl = NcFrameImpl_GetFrameWindow();
	if( pWndFrameImpl->GetSafeHwnd() == NULL )
		return false;
	if( ! NcFrameImpl_GetPM()->NcFrame_IsSupported( pWndFrameImpl ) )
		return false;
CFrameWnd * pFrameWnd = DYNAMIC_DOWNCAST( CFrameWnd, pWndFrameImpl );
	if(		pFrameWnd != NULL
		&&	CExtControlBar::IsOleIpObjActive( pFrameWnd )
		)
	{
		if( m_bNcFrameImpl_RgnSet )
		{
			(((CExtNcFrameImpl*)this)->m_bNcFrameImpl_RgnSet) = false;
			::SetWindowRgn( pWndFrameImpl->m_hWnd, NULL, FALSE );
		} // if( m_bNcFrameImpl_RgnSet )
		return false;
	}
	return true;
}

bool CExtNcFrameImpl::NcFrameImpl_IsDwmBased() const
{
	if( ! NcFrameImpl_IsSupported() )
		return false;
bool bDwmMode = g_PaintManager.m_DWM.IsCompositionEnabled() ? true : false;
	if( ! bDwmMode )
		return false;
	return m_bNcFrameImpl_IsDwmBased;
}

bool CExtNcFrameImpl::NcFrameImpl_IsDwmCaptionReplacement() const
{
	if( ! NcFrameImpl_IsDwmBased() )
		return false;
	if( m_pNcFrameImplBridge != NULL )
	{
		HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
		if(		hWnd != NULL
			&&	( ::__EXT_MFC_GetWindowLong( hWnd, GWL_STYLE ) & WS_VISIBLE ) != 0
			&&	m_pNcFrameImplBridge->NcFrameImplBridge_OnOverPaint_IsEnabled()
			)
			return true;
	}
	return false;
}

bool CExtNcFrameImpl::NcFrameImpl_IsForceEmpty() const
{
	if( NcFrameImpl_IsDwmBased() )
		return true;
	return false;
}

CRect CExtNcFrameImpl::NcFrameImpl_GetForceEmptyNcBorder() const
{
	return CRect( 0, 0, 0, 0 );
}

bool CExtNcFrameImpl::NcFrameImpl_IsForceEmptyNcBorderEmpty() const
{
	if( NcFrameImpl_IsDwmBased() )
		return true;
CRect rc = NcFrameImpl_GetForceEmptyNcBorder();
	ASSERT( rc.left >= 0 );
	ASSERT( rc.top >= 0 );
	ASSERT( rc.right >= 0 );
	ASSERT( rc.bottom >= 0 );
	if(		rc.left > 0
		||	rc.top > 0
		||	rc.right > 0
		||	rc.bottom > 0
		)
		return false;
	return true;
}

bool CExtNcFrameImpl::NcFrameImpl_IsActive()
{
HWND hWndOwn = NcFrameImpl_OnQueryHWND();
	if( hWndOwn == NULL )
		return false;
	return m_bNcFrameImpl_IsActive;
}

void CExtNcFrameImpl::NcFrameImpl_OnNcPaint(
	CDC & dcPaint,
	bool bOuterEmulationMode // = false
	)
{
	bOuterEmulationMode;
CExtPaintManager * pPM = NcFrameImpl_GetPM();
	ASSERT_VALID( pPM );
CWnd * pWndFrameImpl = NcFrameImpl_GetFrameWindow();
CRect rcNcBorders, rcThemePadding;
	pPM->NcFrame_GetMetrics( rcNcBorders, rcThemePadding, pWndFrameImpl );
bool bFrameActive = NcFrameImpl_IsActive();
INT nAdjustCaptionHeight = 0;
	if(		m_pNcFrameImplBridge != NULL
		&&	m_pNcFrameImplBridge->NcFrameImplBridge_OnQueryCaptionMergeMode()
		)
	{
		nAdjustCaptionHeight =
			pPM->NcFrame_GetCaptionHeight(
				bFrameActive,
				pWndFrameImpl
				);
		rcNcBorders.top += nAdjustCaptionHeight;
	}
CRect rcWnd, rcClient;
	pWndFrameImpl->GetWindowRect( &rcWnd );
	pWndFrameImpl->GetClientRect( &rcClient );
CRect rcSavedClient = rcClient;
	pWndFrameImpl->ClientToScreen( &rcClient );
	rcClient.OffsetRect( -rcWnd.TopLeft() );
CRect rcRealClient = rcClient;
	rcClient.top += nAdjustCaptionHeight;
	rcWnd.OffsetRect( -rcWnd.TopLeft() );
DWORD dwOldLayout = dcPaint.SetLayout( LAYOUT_LTR );

	if( ! bOuterEmulationMode  )
	{
		dcPaint.ExcludeClipRect( &rcRealClient );
		if( m_pNcFrameImplBridge != NULL && nAdjustCaptionHeight > 0 )
		{
			HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
			if( hWnd != NULL )
			{
				CRect rc = rcSavedClient;
				rc.bottom = rc.top + nAdjustCaptionHeight;
				pWndFrameImpl->ClientToScreen( &rc );
				::ScreenToClient( hWnd, (LPPOINT)&rc );
				::ScreenToClient( hWnd, ((LPPOINT)&rc)+1 );
				::InvalidateRect( hWnd, &rc, TRUE );
				::UpdateWindow( hWnd );
			}
		}
	} // if( ! bOuterEmulationMode  )

	{ // BLOCK: memory DC
		CExtMemoryDC dc( &dcPaint, &rcWnd );
		NcFrameImpl_ReCacheScButtonRects();
		bool bFrameEnabled = pWndFrameImpl->IsWindowEnabled() ? true : false;
		bool bFrameMaximized = pWndFrameImpl->IsZoomed() ? true : false;
		CExtPaintManager::e_nc_button_state_t
			eStateButtonHelp = CExtPaintManager::__ENCBS_DISABLED,
			eStateButtonMinimize = CExtPaintManager::__ENCBS_DISABLED,
			eStateButtonMaximizeRestore = CExtPaintManager::__ENCBS_DISABLED,
			eStateButtonClose = CExtPaintManager::__ENCBS_DISABLED;
		if( ! m_rcNcFrameImpl_ScClose.IsRectEmpty() )
		{
			if( NcFrameImpl_OnQuerySystemCommandEnabled( SC_CLOSE ) )
				eStateButtonClose =
					( m_nNcFrameImpl_ScTrackedButtonPressed == SC_CLOSE )
						? CExtPaintManager::__ENCBS_PRESSED
						: ( ( m_nNcFrameImpl_ScTrackedButtonHover == SC_CLOSE
							&& m_nNcFrameImpl_ScTrackedButtonPressed == 0 )
							? CExtPaintManager::__ENCBS_HOVER
							: CExtPaintManager::__ENCBS_NORMAL )
						;
		}
		if( ! m_rcNcFrameImpl_ScMaximize.IsRectEmpty() )
		{
			if( bFrameMaximized )
			{
				if(		NcFrameImpl_OnQuerySystemCommandEnabled( SC_RESTORE )
					||	NcFrameImpl_OnQuerySystemCommandEnabled( SC_MAXIMIZE )
					)
					eStateButtonMaximizeRestore =
						( m_nNcFrameImpl_ScTrackedButtonPressed == SC_MAXIMIZE )
							? CExtPaintManager::__ENCBS_PRESSED
							: ( ( m_nNcFrameImpl_ScTrackedButtonHover == SC_MAXIMIZE
								&& m_nNcFrameImpl_ScTrackedButtonPressed == 0 )
								? CExtPaintManager::__ENCBS_HOVER
								: CExtPaintManager::__ENCBS_NORMAL )
							;
			}
			else
			{
				if(		NcFrameImpl_OnQuerySystemCommandEnabled( SC_RESTORE )
					||	NcFrameImpl_OnQuerySystemCommandEnabled( SC_MAXIMIZE )
					)
					eStateButtonMaximizeRestore =
						( m_nNcFrameImpl_ScTrackedButtonPressed == SC_MAXIMIZE )
							? CExtPaintManager::__ENCBS_PRESSED
							: ( ( m_nNcFrameImpl_ScTrackedButtonHover == SC_MAXIMIZE
								&& m_nNcFrameImpl_ScTrackedButtonPressed == 0 )
								? CExtPaintManager::__ENCBS_HOVER
								: CExtPaintManager::__ENCBS_NORMAL )
							;
			}
		}
		if( ! m_rcNcFrameImpl_ScMinimize.IsRectEmpty() )
		{
			if( NcFrameImpl_OnQuerySystemCommandEnabled( SC_MINIMIZE ) )
				eStateButtonMinimize =
					( m_nNcFrameImpl_ScTrackedButtonPressed == SC_MINIMIZE )
						? CExtPaintManager::__ENCBS_PRESSED
						: ( ( m_nNcFrameImpl_ScTrackedButtonHover == SC_MINIMIZE
							&& m_nNcFrameImpl_ScTrackedButtonPressed == 0 )
							? CExtPaintManager::__ENCBS_HOVER
							: CExtPaintManager::__ENCBS_NORMAL )
						;
		}
		if( ! m_rcNcFrameImpl_ScHelp.IsRectEmpty() )
		{
			if( NcFrameImpl_OnQuerySystemCommandEnabled( SC_CONTEXTHELP ) )
				eStateButtonHelp =
					( m_nNcFrameImpl_ScTrackedButtonPressed == SC_CONTEXTHELP )
						? CExtPaintManager::__ENCBS_PRESSED
						: ( ( m_nNcFrameImpl_ScTrackedButtonHover == SC_CONTEXTHELP
							&& m_nNcFrameImpl_ScTrackedButtonPressed == 0 )
							? CExtPaintManager::__ENCBS_HOVER
							: CExtPaintManager::__ENCBS_NORMAL )
						;
		}
		bool bDrawIcon = true, bDrawCaptionText = true;
		CExtCmdIcon _icon;
		CString strCaption;
		CRect rcDrawIcon = m_rcNcFrameImpl_Icon;
		CRect rcDrawText = m_rcNcFrameImpl_Text;
		UINT nDtFlags = DT_SINGLELINE|DT_LEFT|DT_VCENTER;
		if( m_pNcFrameImplBridge != NULL )
		{
			bDrawIcon =
				m_pNcFrameImplBridge->NcFrameImplBridge_OnQueryDrawIcon(
					rcDrawIcon
					);
			bDrawCaptionText =
				m_pNcFrameImplBridge->NcFrameImplBridge_OnQueryDrawCaptionText(
					rcDrawText,
					nDtFlags
					);
		} // if( m_pNcFrameImplBridge != NULL )
		if( bDrawIcon )
			NcFrameImpl_GetIcon( _icon );
		pWndFrameImpl->GetWindowText( strCaption );
		pPM->NcFrame_Paint(
			dc,
			bDrawIcon ? (&_icon) : NULL,
			bDrawCaptionText ? LPCTSTR(strCaption) : _T(""),
			nDtFlags,
			rcWnd,
			rcClient,
			rcDrawIcon,
			rcDrawText,
			m_rcNcFrameImpl_ScHelp,
			m_rcNcFrameImpl_ScMinimize,
			m_rcNcFrameImpl_ScMaximize,
			m_rcNcFrameImpl_ScClose,
			bFrameActive,
			bFrameEnabled,
			bFrameMaximized,
			eStateButtonHelp,
			eStateButtonMinimize,
			eStateButtonMaximizeRestore,
			eStateButtonClose,
			pWndFrameImpl
			);
		if( m_pNcFrameImplBridge != NULL )
		{
			if( ! bOuterEmulationMode )
				m_pNcFrameImplBridge->NcFrameImplBridge_OnOverPaint(
					dc,
					rcWnd,
					rcClient
					);
			if( ! bDrawCaptionText )
				m_pNcFrameImplBridge->NcFrameImplBridge_OnDrawCaptionText(
					dc,
					LPCTSTR(strCaption),
					rcDrawText
					);
		}
	} // BLOCK: memory DC
	dcPaint.SetLayout( dwOldLayout );
}

void CExtNcFrameImpl::NcFrameImpl_AdjustVistaDwmCompatibilityIssues()
{
	if( ! NcFrameImpl_IsSupported() )
		return;
bool bDwmMode = g_PaintManager.m_DWM.IsCompositionEnabled() ? true : false;
	if( ! bDwmMode )
		return;
HWND hWndOwn = NcFrameImpl_OnQueryHWND();
	if( hWndOwn == NULL || ( ! ::IsWindow( hWndOwn ) ) )
		return;
bool bDwmArea = NcFrameImpl_IsDwmCaptionReplacement();
	if( ! bDwmArea )
		return;
bool bCustomNcArea = false;
CExtDWM::__EXT_DWMNCRENDERINGPOLICY ncrp =
		bCustomNcArea
			? CExtDWM::__EXT_DWMNCRP_DISABLED
			: CExtDWM::__EXT_DWMNCRP_ENABLED // CExtDWM::__EXT_DWMNCRP_USEWINDOWSTYLE
			;
HRESULT hr = 
		g_PaintManager.m_DWM.DwmSetWindowAttribute(
			hWndOwn,
			CExtDWM::__EXT_DWMWA_NCRENDERING_POLICY,
			&ncrp,
			sizeof(ncrp)
			);
//	ASSERT( hr == S_OK );
BOOL bAllow = bDwmArea ? TRUE : FALSE;
	hr = 
		g_PaintManager.m_DWM.DwmSetWindowAttribute(
			hWndOwn,
			CExtDWM::__EXT_DWMWA_ALLOW_NCPAINT,
			&bAllow,
			sizeof(bAllow)
			);
	hr;
}

bool CExtNcFrameImpl::NcFrameImpl_FilterWindowProc( LRESULT & lResult, UINT message, WPARAM wParam, LPARAM lParam )
{
	wParam;
	lParam;
	if( message == g_PaintManager.m_nMsgPaintManagerChanged )
	{
		m_BridgeNC.PmBridge_OnPaintManagerChanged( m_BridgeNC.PmBridge_GetPM() );
		lResult = 0L;
		return true;
	} // if( message == g_PaintManager.m_nMsgPaintManagerChanged )
	if( message == g_nMsgFindExtNcFrameImpl )
	{
		CExtNcFrameImpl ** ppExtNcFrameImpl = (CExtNcFrameImpl **)wParam;
		(*ppExtNcFrameImpl) = this;
		m_pNcFrameImplBridge = (IExtNcFrameImplBridge*)lParam;
		return true;
	} // if( message == g_nMsgFindExtNcFrameImpl )
	return false;
}

bool CExtNcFrameImpl::NcFrameImpl_PreWindowProc( LRESULT & lResult, UINT message, WPARAM wParam, LPARAM lParam )
{
	if( ! m_bNcFrameImpl_IsEnabled )
		return false;
	m_bNcFrameImpl_RestoreBorder = false;
CWnd * pWndFrameImpl = NcFrameImpl_GetFrameWindow();
	switch( message )
	{
	case __EXT_MFC_WM_NCUAHDRAWCAPTION:
	case __EXT_MFC_WM_NCUAHDRAWFRAME:
		if( ! NcFrameImpl_IsSupported() )
			break;
		if( NcFrameImpl_IsDwmBased() )
			break;
		lResult = 0;
		return true;
	case WM_SETTEXT:
	{
		if( ! NcFrameImpl_IsSupported() )
			break;
		if( NcFrameImpl_IsDwmBased() )
		{
			if( m_pNcFrameImplBridge != NULL )
			{
				HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
				if( hWnd != NULL )
				{
					::InvalidateRect( hWnd, NULL, TRUE );
					//::UpdateWindow( hWnd );
				}
			}
			break;
		}
		struct friendly_wnd_t : public CWnd { friend class CExtNcFrameImpl; };
		DWORD dw = pWndFrameImpl->GetStyle()&WS_CAPTION;
		if( dw != 0 )
			pWndFrameImpl->ModifyStyle( WS_CAPTION, 0 );
		lResult = ((friendly_wnd_t*)pWndFrameImpl)->DefWindowProc( message, wParam, lParam );
		if( dw != 0 )
			pWndFrameImpl->ModifyStyle( 0, dw );
		pWndFrameImpl->SendMessage( WM_NCPAINT );
		if( m_pNcFrameImplBridge != NULL )
		{
			HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
			if( hWnd != NULL )
			{
				::InvalidateRect( hWnd, NULL, TRUE );
				//::UpdateWindow( hWnd );
			}
		}
	}
	return true;
	case WM_MDIRESTORE:
	case WM_MDIMAXIMIZE:
	case WM_MDIACTIVATE:
	{
		if( ! NcFrameImpl_IsSupported() )
			break;
		if( NcFrameImpl_IsDwmBased() )
			break;
		HWND hWndMdiClient = ::GetParent( pWndFrameImpl->m_hWnd );
		CRect rcClient;
		::GetClientRect( hWndMdiClient, &rcClient );
		::SendMessage( hWndMdiClient, WM_SIZE, SIZE_RESTORED, MAKELPARAM( rcClient.Width(), rcClient.Height() ) );
		NcFrameImpl_SetupRgn();
		if( message == WM_MDIACTIVATE )
		{
			CMDIChildWnd * pWndMdiChild = DYNAMIC_DOWNCAST( CMDIChildWnd, pWndFrameImpl );
			if( pWndMdiChild != NULL )
			{
				HWND hWnd = (HWND)wParam;
				if( hWnd != NULL && ::IsWindow(hWnd) )
				{
					MSG _msg;
					for( ; ::PeekMessage( &_msg, hWnd, WM_NCPAINT, WM_NCPAINT, PM_REMOVE ); );
					::PostMessage( hWnd, WM_NCACTIVATE, FALSE, 0 );
				}
				hWnd = (HWND)lParam;
				if( hWnd != NULL && ::IsWindow(hWnd) )
				{
					MSG _msg;
					for( ; ::PeekMessage( &_msg, hWnd, WM_NCPAINT, WM_NCPAINT, PM_REMOVE ); );
					::PostMessage( hWnd, WM_NCACTIVATE, TRUE, 0 );
				}
			}
		}
	}
	break;
	case WM_WINDOWPOSCHANGED:
	case WM_WINDOWPOSCHANGING:
	case WM_SIZE:
		if( ! NcFrameImpl_IsSupported() )
			break;
		if( NcFrameImpl_IsDwmBased() )
			break;
		if( NcFrameImpl_IsNcLocked() )
			return true;
		break;
	case WM_SETICON:
		if( wParam == ICON_SMALL && ((HICON)lParam) != NULL )
		{
			m_iconNcFrameImpl_QueryCache.AssignFromHICON( ((HICON)lParam), true );
			m_iconNcFrameImpl_QueryCache.Scale( CSize(16,16) );
		} // if( wParam == ICON_SMALL && ((HICON)lParam) != NULL )
		break;
	case WM_NCACTIVATE:
		{
			if( ! NcFrameImpl_IsSupported() )
				break;
			if( NcFrameImpl_IsDwmBased() )
				break;
			HWND hWndOwn = NcFrameImpl_OnQueryHWND();
			ASSERT( hWndOwn != NULL && ::IsWindow(hWndOwn) );
			if( m_pNcFrameImplBridge != NULL )
			{
				HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
				if( hWnd != NULL )
				{
					::InvalidateRect( hWnd, NULL, TRUE );
					//::UpdateWindow( hWnd );
				}
				if( NcFrameImpl_IsDwmCaptionReplacement() )
				{
					CExtControlBar * pBar = DYNAMIC_DOWNCAST( CExtControlBar, CWnd::FromHandlePermanent( hWnd ) );
					if( pBar != NULL )
					{
						INT nIndex, nCount = pBar->AnimationSite_ClientGetCount();
						for( nIndex = 0; nIndex < nCount; nIndex ++ )
						{
							CExtAnimationClient * pAC = pBar->AnimationSite_ClientGetAt( nIndex );
							ASSERT( pAC != NULL );
							pBar->AnimationSite_ClientProgressStop( pAC );
							pAC->AnimationClient_StateGet( false ).Empty();
							pAC->AnimationClient_StateGet( true ).Empty();
						} // for( nIndex = 0; nIndex < nCount; nIndex ++ )
					} // if( pBar != NULL )
					if( m_pNcFrameImplBridge != NULL )
					{
						if( hWnd != NULL && ::IsWindow( hWnd ) )
							::RedrawWindow(
								hWnd,
								NULL,
								NULL,
								RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_ALLCHILDREN
								);
					}
					::SendMessage( hWndOwn, WM_NCPAINT, 0L, 0L );
					break;
				} // if( NcFrameImpl_IsDwmCaptionReplacement() )
			}
			if( NcFrameImpl_IsDwmBased() )
				break;
			if( pWndFrameImpl->IsKindOf( RUNTIME_CLASS(CMDIChildWnd) ) )
			{
				pWndFrameImpl->SetRedraw( FALSE );
				struct friendly_wnd_t : public CWnd { friend class CExtNcFrameImpl; };
				lResult = ((friendly_wnd_t*)pWndFrameImpl)->DefWindowProc( message, wParam, lParam );
				pWndFrameImpl->SetRedraw( TRUE );
				pWndFrameImpl->RedrawWindow(
					NULL,
					NULL,
					RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_ALLCHILDREN
					);
			}
			::SendMessage( hWndOwn, WM_NCPAINT, 0L, 0L );
 			lResult = 1;
		}
		if( ! m_bNcFrameImpl_NcInitPassed )
		{
			m_bNcFrameImpl_NcInitPassed = true;
			break;
		}
		return true;
	case WM_CREATE:
		NcFrameImpl_MapHtRects_Clean();
		NcFrameImpl_AdjustVistaDwmCompatibilityIssues();
		if( NcFrameImpl_IsSupported() )
			NcFrameImpl_RecalcNcFrame();
	break;
	case WM_INITDIALOG:
		NcFrameImpl_AdjustVistaDwmCompatibilityIssues();
		if(		NcFrameImpl_IsSupported()
			&&	( ! NcFrameImpl_IsDwmBased() )
			)
		{
			CExtPaintManager * pPM = NcFrameImpl_GetPM();
			ASSERT_VALID( pPM );
			INT nCaptionHeightSkinned = pPM->NcFrame_GetCaptionHeight( true, pWndFrameImpl );
			if( nCaptionHeightSkinned > 0 )
			{
				INT nCaptionHeightDefault = ::GetSystemMetrics( SM_CYCAPTION );
				INT nCaptionHeightDifference = nCaptionHeightSkinned - nCaptionHeightDefault;
				if( nCaptionHeightDifference != 0 )
				{
					WINDOWPLACEMENT _wp;
					::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
					_wp.length = sizeof(WINDOWPLACEMENT);
					if( ::GetWindowPlacement( pWndFrameImpl->m_hWnd, &_wp ) )
					{
						_wp.rcNormalPosition.bottom += nCaptionHeightDifference;
//						::SetWindowPlacement( pWndFrameImpl->m_hWnd, &_wp );
						if( _wp.showCmd == SW_SHOWNORMAL || _wp.showCmd == SW_RESTORE )
							pWndFrameImpl->SetWindowPos(
								NULL,
								0,
								0,
								_wp.rcNormalPosition.right - _wp.rcNormalPosition.left,
								_wp.rcNormalPosition.bottom - _wp.rcNormalPosition.top,
								SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOREDRAW
								);
					}
				}
			}
		}
		NcFrameImpl_SetupRgn();
	break;
	case WM_ACTIVATEAPP:
		if( wParam == 0 )
			m_bNcFrameImpl_IsActive = false;
		else
			m_bNcFrameImpl_IsActive = NcFrameImpl_IsActive();

		if(		NcFrameImpl_IsSupported()
			&&	( ! NcFrameImpl_IsDwmBased() )
			&&	m_pNcFrameImplBridge != NULL
			)
		{
			HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
			if( hWnd != NULL )
			{
				::InvalidateRect( hWnd, NULL, TRUE );
				//::UpdateWindow( hWnd );
			}
		}

		if(		(! NcFrameImpl_IsForceEmpty() )
			||	(! NcFrameImpl_IsForceEmptyNcBorderEmpty() )
			)
			pWndFrameImpl->SendMessage( WM_NCPAINT );
		else
		{
			if( NcFrameImpl_IsDwmCaptionReplacement() )
			{
				if( m_pNcFrameImplBridge != NULL )
				{
					HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
					if( hWnd != NULL && ::IsWindow( hWnd ) )
						::RedrawWindow(
							hWnd,
							NULL,
							NULL,
							RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_ALLCHILDREN
							);
				}
				HWND hWndOwn = NcFrameImpl_OnQueryHWND();
				ASSERT( hWndOwn != NULL && ::IsWindow(hWndOwn) );
				::SendMessage( hWndOwn, WM_NCPAINT, 0L, 0L );
				break;
			} // if( NcFrameImpl_IsDwmCaptionReplacement() )
		}
	break;
	case WM_ACTIVATE:
		{
			m_bNcFrameImpl_IsActive = ( LOWORD(wParam) == WA_INACTIVE ) ? false : true;
			if( ( ! m_bNcFrameImpl_IsActive ) && pWndFrameImpl->IsWindowEnabled() )
			{
				HWND hWnd = ::GetActiveWindow();
				if( hWnd != NULL )
				{
					CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
					if(		pWnd != NULL
						&&	(	pWnd == pWndFrameImpl
							||	pWnd->IsKindOf( RUNTIME_CLASS(CMiniFrameWnd) )
							)
						)
						m_bNcFrameImpl_IsActive = true;
				}
			}
			if(		pWndFrameImpl->IsWindowEnabled()
				&&	(	(! NcFrameImpl_IsForceEmpty() )
					||	(! NcFrameImpl_IsForceEmptyNcBorderEmpty() )
					)
				)
			{
				NcFrameImpl_DelayRgnAdjustment();
				NcFrameImpl_SetupRgn();
			}
			else
			{
				if( NcFrameImpl_IsDwmCaptionReplacement() )
				{
					if( m_pNcFrameImplBridge != NULL )
					{
						HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
						if( hWnd != NULL && ::IsWindow( hWnd ) )
							::RedrawWindow(
								hWnd,
								NULL,
								NULL,
								RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_ALLCHILDREN
								);
					}
				} // if( NcFrameImpl_IsDwmCaptionReplacement() )
			}
			HWND hWndOwn = NcFrameImpl_OnQueryHWND();
			ASSERT( hWndOwn != NULL && ::IsWindow(hWndOwn) );
			::SendMessage( hWndOwn, WM_NCPAINT, 0L, 0L );
			if( m_pNcFrameImplBridge != NULL )
			{
				HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
				if( hWnd != NULL )
				{
					::InvalidateRect( hWnd, NULL, TRUE );
					::UpdateWindow( hWnd );
				}
			}
		}
	break;
	case WM_NCLBUTTONDBLCLK:
		if( NcFrameImpl_IsDwmCaptionReplacement() && m_pNcFrameImplBridge != NULL )
		{
			if( wParam == HTCAPTION || wParam == HTSYSMENU || wParam == HTTOP )
			{
				HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
				if( hWnd != NULL && ::IsWindow( hWnd ) )
				{
					CPoint ptCursor;
					::GetCursorPos( &ptCursor );
					::ScreenToClient( hWnd, &ptCursor );
					CExtToolControlBar * pBar = DYNAMIC_DOWNCAST( CExtToolControlBar, CWnd::FromHandlePermanent( hWnd ) );
					if( pBar != NULL )
					{
						INT nToolBarButtonHT = pBar->HitTest( ptCursor );
						if( nToolBarButtonHT >= 0 )
						{
							lResult = ::SendMessage( hWnd, WM_LBUTTONDBLCLK, 0, MAKELPARAM( ptCursor.x, ptCursor.y ) );
							return true;
						}
					}
				} // if( hWnd != NULL && ::IsWindow( hWnd ) )
			} // if( wParam == ...
			break;
		} // if( NcFrameImpl_IsDwmCaptionReplacement() && m_pNcFrameImplBridge != NULL )
		if( NcFrameImpl_IsForceEmpty() )
			break;
		if( wParam == HTCAPTION )
		{
			HWND hWndOwn = NcFrameImpl_OnQueryHWND();
			ASSERT( hWndOwn != NULL && ::IsWindow(hWndOwn) );
			bool bZoomed = ( ::IsZoomed( hWndOwn ) ) ? true : false;
			UINT nSC = bZoomed ? SC_RESTORE : SC_MAXIMIZE;
			if(		NcFrameImpl_OnQuerySystemCommandEnabled( SC_RESTORE )
				||	NcFrameImpl_OnQuerySystemCommandEnabled( SC_MAXIMIZE )
				)
				::SendMessage( hWndOwn, WM_SYSCOMMAND, WPARAM(nSC), lParam );
			lResult = 0L;
			return true;
		} // if( wParam == HTCAPTION )
		else if( wParam == HTSYSMENU )
		{
			if( NcFrameImpl_OnQuerySystemCommandEnabled( SC_CLOSE ) )
			{
				HWND hWndOwn = pWndFrameImpl->GetSafeHwnd();
				::SendMessage( hWndOwn, WM_SYSCOMMAND, SC_CLOSE, lParam );
				if( ::IsWindow( hWndOwn ) )
					pWndFrameImpl->SendMessage( WM_NCPAINT );
				lResult = 0L;
				return true;
			} // if( NcFrameImpl_OnQuerySystemCommandEnabled( SC_CLOSE ) )
		} // else if( wParam == HTSYSMENU )
	break;
	case WM_NCHITTEST:
		if( ! NcFrameImpl_IsSupported() )
			break;
		if( NcFrameImpl_IsDwmCaptionReplacement() )
		{
			if(	g_PaintManager.m_DWM.DwmDefWindowProc(
					pWndFrameImpl->m_hWnd,
					message,
					wParam,
					lParam,
					&lResult
					)
				)
				return true;
			INT nV = ::GetSystemMetrics( SM_CYFRAME );
			INT nCV = nV + ::GetSystemMetrics( SM_CYCAPTION );
			if( pWndFrameImpl->IsZoomed() )
				nCV += nV;
			CPoint pointScreen;
			if( ::GetCursorPos( &pointScreen ) )
			{
				CRect rc;
				pWndFrameImpl->GetWindowRect( &rc );
				rc.bottom = rc.top + nV;
				if( rc.PtInRect( pointScreen ) )
				{
					lResult = HTTOP;
					return true;
				}
				rc.bottom = rc.top + nCV;
				rc.DeflateRect( ::GetSystemMetrics(SM_CXFRAME), 0 );
				if( rc.PtInRect( pointScreen ) )
				{
					lResult = HTCAPTION;
					return true;
				}
			}
		}
		if( NcFrameImpl_IsDwmBased() )
			break;
		if( ! NcFrameImpl_IsForceEmpty() )
		{
			if( m_bNcFrameImpl_HelperInsideNcHitTest )
			{
				lResult = HTNOWHERE;
				return true;
			}
			m_bNcFrameImpl_HelperInsideNcHitTest = true;
			CRect rcWnd;
			pWndFrameImpl->GetWindowRect( &rcWnd );
			CPoint pointScreen, pointWnd( -32767, -32767 );
			if( ::GetCursorPos( &pointScreen ) )
			{
				pointWnd = pointScreen;
				pointWnd -= rcWnd.TopLeft();
			}
			if( m_pNcFrameImplBridge != NULL )
			{
				HRGN hRgn = m_pNcFrameImplBridge->NcFrameImplBridge_GetNcResizingHRGN();
				if(		hRgn != NULL
					&&	::PtInRegion( hRgn, pointWnd.x, pointWnd.y )
					)
				{
					m_nNcFrameImpl_LastExternalNcHT = HTTOPLEFT;
					lResult = HTTOPLEFT;
					//NcFrameImpl_CheckCursor( pointScreen, LPARAM(lResult) );
					::SetCursor( ::LoadCursor( NULL, IDC_SIZENWSE ) );
					m_bNcFrameImpl_HelperInsideNcHitTest = false;
					return true;
				}
			}
			CRect rcTmp = NcFrameImpl_GetNcScRect( SC_CLOSE );
			if( rcTmp.PtInRect(pointWnd) )
			{
				m_nNcFrameImpl_LastExternalNcHT = HTCLOSE;
				lResult = HTCAPTION;
				NcFrameImpl_CheckCursor( pointScreen, LPARAM(HTCLOSE) );
				m_bNcFrameImpl_HelperInsideNcHitTest = false;
				if( m_pNcFrameImplBridge != NULL )
				{
					HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
					if( hWnd != NULL && ::IsWindow( hWnd ) )
					{
						rcTmp.OffsetRect( rcWnd.TopLeft() );
						::ScreenToClient( hWnd, ((LPPOINT)(&rcTmp))+0 );
						::ScreenToClient( hWnd, ((LPPOINT)(&rcTmp))+1 );
						::InvalidateRect( hWnd, &rcTmp, TRUE );
						if( m_rcNcFrameImpl_LastExternalNcHT != rcTmp )
						{
							::InvalidateRect( hWnd, &m_rcNcFrameImpl_LastExternalNcHT, TRUE );
							m_rcNcFrameImpl_LastExternalNcHT = rcTmp;
						}
						//::UpdateWindow( hWnd );
					}
				}
				return true;
			}
			rcTmp = NcFrameImpl_GetNcScRect( SC_MINIMIZE );
			if( rcTmp.PtInRect(pointWnd) )
			{
				m_nNcFrameImpl_LastExternalNcHT = HTMINBUTTON;
				lResult = HTCAPTION;
				NcFrameImpl_CheckCursor( pointScreen, LPARAM(HTMINBUTTON) );
				m_bNcFrameImpl_HelperInsideNcHitTest = false;
				if( m_pNcFrameImplBridge != NULL )
				{
					HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
					if( hWnd != NULL && ::IsWindow( hWnd ) )
					{
						rcTmp.OffsetRect( rcWnd.TopLeft() );
						::ScreenToClient( hWnd, ((LPPOINT)(&rcTmp))+0 );
						::ScreenToClient( hWnd, ((LPPOINT)(&rcTmp))+1 );
						::InvalidateRect( hWnd, &rcTmp, TRUE );
						if( m_rcNcFrameImpl_LastExternalNcHT != rcTmp )
						{
							::InvalidateRect( hWnd, &m_rcNcFrameImpl_LastExternalNcHT, TRUE );
							m_rcNcFrameImpl_LastExternalNcHT = rcTmp;
						}
						//::UpdateWindow( hWnd );
					}
				}
				return true;
			}
			rcTmp = NcFrameImpl_GetNcScRect( SC_MAXIMIZE );
			if( rcTmp.PtInRect(pointWnd) )
			{
				m_nNcFrameImpl_LastExternalNcHT = HTMAXBUTTON;
				lResult = HTCAPTION;
				NcFrameImpl_CheckCursor( pointScreen, LPARAM(HTMAXBUTTON) );
				m_bNcFrameImpl_HelperInsideNcHitTest = false;
				if( m_pNcFrameImplBridge != NULL )
				{
					HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
					if( hWnd != NULL && ::IsWindow( hWnd ) )
					{
						rcTmp.OffsetRect( rcWnd.TopLeft() );
						::ScreenToClient( hWnd, ((LPPOINT)(&rcTmp))+0 );
						::ScreenToClient( hWnd, ((LPPOINT)(&rcTmp))+1 );
						::InvalidateRect( hWnd, &rcTmp, TRUE );
						if( m_rcNcFrameImpl_LastExternalNcHT != rcTmp )
						{
							::InvalidateRect( hWnd, &m_rcNcFrameImpl_LastExternalNcHT, TRUE );
							m_rcNcFrameImpl_LastExternalNcHT = rcTmp;
						}
					}
				}
				return true;
			}
			rcTmp = NcFrameImpl_GetNcScRect( SC_CONTEXTHELP );
			if( rcTmp.PtInRect(pointWnd) )
			{
				m_nNcFrameImpl_LastExternalNcHT = HTHELP;
				lResult = HTCAPTION;
				NcFrameImpl_CheckCursor( pointScreen, LPARAM(HTHELP) );
				m_bNcFrameImpl_HelperInsideNcHitTest = false;
				if( m_pNcFrameImplBridge != NULL )
				{
					HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
					if( hWnd != NULL && ::IsWindow( hWnd ) )
					{
						rcTmp.OffsetRect( rcWnd.TopLeft() );
						::ScreenToClient( hWnd, ((LPPOINT)(&rcTmp))+0 );
						::ScreenToClient( hWnd, ((LPPOINT)(&rcTmp))+1 );
						::InvalidateRect( hWnd, &rcTmp, TRUE );
						if( m_rcNcFrameImpl_LastExternalNcHT != rcTmp )
						{
							::InvalidateRect( hWnd, &m_rcNcFrameImpl_LastExternalNcHT, TRUE );
							m_rcNcFrameImpl_LastExternalNcHT = rcTmp;
						}
					}
				}
				return true;
			}

			MINMAXINFO * pMinMaxInfo = &m_NcFrameImpl_MinMaxInfo_Cache;

			pWndFrameImpl->ModifyStyle( WS_CAPTION|WS_BORDER, 0, 0 );
			Sleep( 1 ); // this line of code magically fixes 50-100% CPU eating problem when moving mouse over skinable NC areas
			if( NcFrameImpl_GetNcHtRect(HTTOPLEFT,false,false,false,pMinMaxInfo,&rcWnd).PtInRect(pointWnd) )
			{
				m_nNcFrameImpl_LastExternalNcHT = HTTOPLEFT;
				lResult = HTTOPLEFT;
				NcFrameImpl_CheckCursor( pointScreen, LPARAM(lResult) );
				m_bNcFrameImpl_HelperInsideNcHitTest = false;
				return true;
			}
			if( NcFrameImpl_GetNcHtRect(HTTOPRIGHT,false,false,false,pMinMaxInfo,&rcWnd).PtInRect(pointWnd) )
			{
				m_nNcFrameImpl_LastExternalNcHT = HTTOPRIGHT;
				lResult = HTTOPRIGHT;
				NcFrameImpl_CheckCursor( pointScreen, LPARAM(lResult) );
				m_bNcFrameImpl_HelperInsideNcHitTest = false;
				return true;
			}
			if( NcFrameImpl_GetNcHtRect(HTBOTTOMLEFT,false,false,false,pMinMaxInfo,&rcWnd).PtInRect(pointWnd) )
			{
				m_nNcFrameImpl_LastExternalNcHT = HTBOTTOMLEFT;
				lResult = HTBOTTOMLEFT;
				NcFrameImpl_CheckCursor( pointScreen, LPARAM(lResult) );
				m_bNcFrameImpl_HelperInsideNcHitTest = false;
				return true;
			}
			if( NcFrameImpl_GetNcHtRect(HTBOTTOMRIGHT,false,false,false,pMinMaxInfo,&rcWnd).PtInRect(pointWnd) )
			{
				m_nNcFrameImpl_LastExternalNcHT = HTBOTTOMRIGHT;
				lResult = HTBOTTOMRIGHT;
				NcFrameImpl_CheckCursor( pointScreen, LPARAM(lResult) );
				m_bNcFrameImpl_HelperInsideNcHitTest = false;
				return true;
			}
			if( NcFrameImpl_GetNcHtRect(HTTOP,false,false,false,pMinMaxInfo,&rcWnd).PtInRect(pointWnd) )
			{
				m_nNcFrameImpl_LastExternalNcHT = HTTOP;
				lResult = HTTOP;
				NcFrameImpl_CheckCursor( pointScreen, LPARAM(lResult) );
				m_bNcFrameImpl_HelperInsideNcHitTest = false;
				return true;
			}
			if( NcFrameImpl_GetNcHtRect(HTBOTTOM,false,false,false,pMinMaxInfo,&rcWnd).PtInRect(pointWnd) )
			{
				m_nNcFrameImpl_LastExternalNcHT = HTBOTTOM;
				lResult = HTBOTTOM;
				NcFrameImpl_CheckCursor( pointScreen, LPARAM(lResult) );
				m_bNcFrameImpl_HelperInsideNcHitTest = false;
				return true;
			}
			if( NcFrameImpl_GetNcHtRect(HTLEFT,false,false,false,pMinMaxInfo,&rcWnd).PtInRect(pointWnd) )
			{
				m_nNcFrameImpl_LastExternalNcHT = HTLEFT;
				lResult = HTLEFT;
				NcFrameImpl_CheckCursor( pointScreen, LPARAM(lResult) );
				m_bNcFrameImpl_HelperInsideNcHitTest = false;
				return true;
			}
			if( NcFrameImpl_GetNcHtRect(HTRIGHT,false,false,false,pMinMaxInfo,&rcWnd).PtInRect(pointWnd) )
			{
				m_nNcFrameImpl_LastExternalNcHT = HTRIGHT;
				lResult = HTRIGHT;
				NcFrameImpl_CheckCursor( pointScreen, LPARAM(lResult) );
				m_bNcFrameImpl_HelperInsideNcHitTest = false;
				return true;
			}
			if( NcFrameImpl_GetNcHtRect(HTSYSMENU,false,false,false,pMinMaxInfo,&rcWnd).PtInRect(pointWnd) )
			{
				m_nNcFrameImpl_LastExternalNcHT = HTSYSMENU;
				lResult = HTSYSMENU;
				NcFrameImpl_CheckCursor( pointScreen, LPARAM(lResult) );
//				if( pWndFrameImpl->IsIconic() && (! CExtPopupMenuWnd::IsKeyPressed( VK_LBUTTON) ) )
//					lResult = HTNOWHERE;
				m_bNcFrameImpl_HelperInsideNcHitTest = false;
				return true;
			}
			if( NcFrameImpl_GetNcHtRect(HTCAPTION,false,false,false,pMinMaxInfo,&rcWnd).PtInRect(pointWnd) )
			{
				m_nNcFrameImpl_LastExternalNcHT = HTCAPTION;
				lResult = HTCAPTION;
				NcFrameImpl_CheckCursor( pointScreen, LPARAM(lResult) );
//				if( pWndFrameImpl->IsIconic() && (! CExtPopupMenuWnd::IsKeyPressed( VK_LBUTTON) ) )
//					lResult = HTNOWHERE;
				m_bNcFrameImpl_HelperInsideNcHitTest = false;
				return true;
			}
			m_bNcFrameImpl_HelperInsideNcHitTest = false;
		} // if( ! NcFrameImpl_IsForceEmpty() )
		lResult = HTCLIENT;
		return true;
		//break;
	case WM_NCCALCSIZE:
		if( ! NcFrameImpl_IsSupported() )
			break;
		if( pWndFrameImpl->IsIconic() )
			break;
		if(		m_bNcFrameImpl_InAdjustAdjustThemeSettings
			&&	NcFrameImpl_IsDwmBased()
			&&	(! NcFrameImpl_IsDwmCaptionReplacement() )
			)
			break;
		if(		m_bNcFrameImpl_InAdjustAdjustThemeSettings_NoRgnSetup
			&&	m_bNcFrameImpl_InAdjustAdjustThemeSettings
			&&	(! NcFrameImpl_IsDwmCaptionReplacement() )
			)
		{
			CRect rcWnd, rcClient;
			pWndFrameImpl->GetWindowRect( &rcWnd );
			pWndFrameImpl->ScreenToClient( &rcWnd );
			pWndFrameImpl->GetClientRect( &rcClient );
			CRect rcDiff( rcClient.left - rcWnd.left, rcClient.top - rcWnd.top, rcWnd.right - rcClient.right, rcWnd.bottom - rcClient.bottom );
			NCCALCSIZE_PARAMS * pNCCSP = reinterpret_cast < NCCALCSIZE_PARAMS * > ( lParam );
			ASSERT( pNCCSP != NULL );
			pNCCSP->rgrc[0].left   += rcDiff.left;
			pNCCSP->rgrc[0].top    += rcDiff.top;
			pNCCSP->rgrc[0].right  += rcDiff.right;
			pNCCSP->rgrc[0].bottom += rcDiff.bottom;
			lResult = 0L;
			return true;
		}
		{ // BLOCK: adjust MDI client area styles
			HWND hWndMdiClient = CExtControlBar::stat_FindMdiClientHWND( pWndFrameImpl->m_hWnd );
			if( hWndMdiClient != NULL )
			{
				__EXT_MFC_LONG_PTR dwStyle = ::__EXT_MFC_GetWindowLong( hWndMdiClient, GWL_STYLE );
				if( (dwStyle&WS_BORDER) != 0 )
					::__EXT_MFC_SetWindowLong( hWndMdiClient, GWL_STYLE, dwStyle&(~(WS_BORDER)) );
				__EXT_MFC_LONG_PTR dwStyleEx = ::__EXT_MFC_GetWindowLong( hWndMdiClient, GWL_EXSTYLE );
				if( (dwStyleEx&(WS_EX_CLIENTEDGE|WS_EX_STATICEDGE|WS_EX_DLGMODALFRAME)) != 0 )
					::__EXT_MFC_SetWindowLong( hWndMdiClient, GWL_EXSTYLE, (dwStyleEx&(~(WS_EX_CLIENTEDGE|WS_EX_STATICEDGE|WS_EX_DLGMODALFRAME)))|WS_EX_CLIENTEDGE );
			}
		} // BLOCK: adjust MDI client area styles
		if(		pWndFrameImpl->IsKindOf( RUNTIME_CLASS( CMDIChildWnd ) )
			&&	m_bNcFrameImpl_IsActive
			&&	(pWndFrameImpl->GetStyle()&WS_MAXIMIZE) != 0
			)
			break;
		else
		{
			bool bNcCaptionMerging = false;
			if( m_pNcFrameImplBridge != NULL )
			{
				HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
				if(		hWnd != NULL
					&&	m_pNcFrameImplBridge->NcFrameImplBridge_OnQueryCaptionMergeMode()
					)
				{
					bNcCaptionMerging = true;
					if( ! m_bNcFrameImpl_CreatePassed )
						return false;
					if( NcFrameImpl_IsNcLocked() )
						return false;
					CFrameWnd * pFrame = DYNAMIC_DOWNCAST( CFrameWnd, pWndFrameImpl );
					if( pFrame != NULL )
					{
						bool bIsLoading = false;
						if( CExtControlBar::ProfileBarStateIsSerializing( pFrame, &bIsLoading ) )
						{
							if( bIsLoading )
								return false;
						}
					}
				}
			}
			CExtUxTheme::__EXT_UX_MARGINS _margins = { 0, 0, 0, 0 };
			bool bDwmMode = NcFrameImpl_IsDwmBased();
			if( bDwmMode && ( ! bNcCaptionMerging ) )
			{
				m_bNcFrameImpl_DwmSurfaceInitializationComplete = false;
				g_PaintManager.m_DWM.DwmExtendFrameIntoClientArea( pWndFrameImpl->m_hWnd, &_margins );
				return false;
			}
			if( ! NcFrameImpl_IsNcLocked() )
			{
				bool bZoomed = pWndFrameImpl->IsZoomed() ? true : false;
				NCCALCSIZE_PARAMS * pNCCSP = reinterpret_cast < NCCALCSIZE_PARAMS * > ( lParam );
				ASSERT( pNCCSP != NULL );
				if( bNcCaptionMerging && bDwmMode )
				{
					INT nV = ::GetSystemMetrics( SM_CYFRAME );
					INT nCV = nV + ::GetSystemMetrics( SM_CYCAPTION );
					INT nLeftRight = ::GetSystemMetrics( SM_CXFRAME ), nTopBottom = nV;
					if( bZoomed )
						nCV += nV;
					_margins.cyTopHeight = nCV;
					pNCCSP->rgrc[0].left += nLeftRight;
					pNCCSP->rgrc[0].right -= nLeftRight;
					pNCCSP->rgrc[0].bottom -= nTopBottom;
				} // if( bNcCaptionMerging && bDwmMode )
				else if( NcFrameImpl_IsForceEmpty() )
				{
					if( ! NcFrameImpl_IsForceEmptyNcBorderEmpty() )
					{
						CRect rc = NcFrameImpl_GetForceEmptyNcBorder();
						if( ! bNcCaptionMerging )
							pNCCSP->rgrc[0].top += rc.top;
						pNCCSP->rgrc[0].left += rc.left;
						pNCCSP->rgrc[0].right -= rc.right;
						pNCCSP->rgrc[0].bottom -= rc.bottom;
					} // if( ! NcFrameImpl_IsForceEmptyNcBorderEmpty() )
				} // else if( NcFrameImpl_IsForceEmpty() )
				else
				{
					CRect rcNcBorders, rcThemePadding;
					NcFrameImpl_GetPM()->NcFrame_GetMetrics(
						rcNcBorders,
						rcThemePadding,
						pWndFrameImpl
						);

					if( ! bNcCaptionMerging )
						pNCCSP->rgrc[0].top += rcThemePadding.top;
//					if( ! bZoomed )
					{
						pNCCSP->rgrc[0].left    += rcThemePadding.left;
						pNCCSP->rgrc[0].right   -= rcThemePadding.right;
						pNCCSP->rgrc[0].bottom  -= rcThemePadding.bottom;
					}
//					bool bMaximized = ( ( pWndFrameImpl->GetStyle() & WS_MAXIMIZE ) != 0 ) ? true : false;
//					if( bMaximized )
//					{
//						pNCCSP->rgrc[0].left   -= rcNcBorders.left;
//						pNCCSP->rgrc[0].right  += rcNcBorders.right;
//						pNCCSP->rgrc[0].top    -= rcNcBorders.top;
//						pNCCSP->rgrc[0].bottom += rcNcBorders.bottom;
//					}
					if(		m_pNcFrameImplBridge != NULL
						&&	m_pNcFrameImplBridge->NcFrameImplBridge_OnQueryCaptionMergeMode()
						)
						pNCCSP->rgrc[0].top -=
							NcFrameImpl_GetPM() -> 
								NcFrame_GetCaptionHeight(
									NcFrameImpl_IsActive(),
									NcFrameImpl_GetFrameWindow()
									);
				}
				WINDOWPLACEMENT _wp;
				::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
				if( bDwmMode )
					GetWindowPlacement( &_wp );
				if( m_pNcFrameImplBridge != NULL && NcFrameImpl_IsDwmCaptionReplacement()  )
				{
					m_pNcFrameImplBridge->NcFrameImpl_PreSetWindowPlacement( _wp );
					m_pNcFrameImplBridge->NcFrameImpl_AdjustDwmArea( _margins );
				}
				m_bNcFrameImpl_DwmSurfaceInitializationComplete = true;
				g_PaintManager.m_DWM.DwmExtendFrameIntoClientArea( pWndFrameImpl->m_hWnd, &_margins );
				if( m_pNcFrameImplBridge != NULL && NcFrameImpl_IsDwmCaptionReplacement()  )
					m_pNcFrameImplBridge->NcFrameImpl_PostSetWindowPlacement( _wp );
			} // if( ! NcFrameImpl_IsNcLocked() )
			lResult = 0;
			return true;
		}

	case WM_NCPAINT:
		if( m_bNcFrameImpl_DestroyPassed )
		{
			lResult = 0;
			return true;
		}
		if( ! NcFrameImpl_IsDwmBased() )
		{
			if(		(! NcFrameImpl_IsForceEmpty() )
//				||	m_bNcFrameImpl_RestoreEnabledState
				)
			{
				CMDIChildWnd * pWndMdiChild = DYNAMIC_DOWNCAST( CMDIChildWnd, pWndFrameImpl );
				if( pWndMdiChild != NULL && pWndMdiChild->IsWindowEnabled() )
				{
					HWND hWndMDIClient = ::GetParent( pWndMdiChild->m_hWnd );
					HWND hWndActiveChildFrame = (HWND) ::SendMessage( hWndMDIClient, WM_MDIGETACTIVE, 0, NULL );
					if( hWndActiveChildFrame != NULL )
						m_bNcFrameImpl_IsActive = ( hWndActiveChildFrame == pWndMdiChild->m_hWnd ) ? true : false;
				}
				CWindowDC dcPaint( pWndFrameImpl );
				NcFrameImpl_OnNcPaint( dcPaint );
				lResult = 0;
				return true;
			}
		} //if( ! NcFrameImpl_IsDwmBased() )
		break;
	case WM_NCLBUTTONUP:
			if( NcFrameImpl_IsDwmCaptionReplacement() && m_pNcFrameImplBridge != NULL )
			{
				if( wParam == HTCAPTION || wParam == HTSYSMENU || wParam == HTTOP )
				{
					HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
					if( hWnd != NULL && ::IsWindow( hWnd ) )
					{
						CPoint ptCursor;
						::GetCursorPos( &ptCursor );
						::ScreenToClient( hWnd, &ptCursor );
						CExtToolControlBar * pBar = DYNAMIC_DOWNCAST( CExtToolControlBar, CWnd::FromHandlePermanent( hWnd ) );
						if( pBar != NULL )
						{
							INT nToolBarButtonHT = pBar->HitTest( ptCursor );
							if( nToolBarButtonHT >= 0 )
							{
								lResult = ::SendMessage( hWnd, WM_LBUTTONUP, 0, MAKELPARAM( ptCursor.x, ptCursor.y ) );
								return true;
							}
						}
					} // if( hWnd != NULL && ::IsWindow( hWnd ) )
				} // if( wParam == ...
				break;
			} // if( NcFrameImpl_IsDwmCaptionReplacement() && m_pNcFrameImplBridge != NULL )
			if( ! NcFrameImpl_IsSupported() )
				break;
			return true;
//		break;
	case WM_NCLBUTTONDOWN:
		{
			if( ! NcFrameImpl_IsSupported() )
				break;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
			if( NcFrameImpl_IsDwmCaptionReplacement() && m_pNcFrameImplBridge != NULL )
			{
				if( wParam == HTCAPTION || wParam == HTSYSMENU || wParam == HTTOP )
				{
					HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
					if( hWnd != NULL && ::IsWindow( hWnd ) )
					{
						CPoint ptCursor;
						::GetCursorPos( &ptCursor );
						::ScreenToClient( hWnd, &ptCursor );
						CExtToolControlBar * pBar = DYNAMIC_DOWNCAST( CExtToolControlBar, CWnd::FromHandlePermanent( hWnd ) );
						if( pBar != NULL )
						{
							CExtPaintManager * pPM = NcFrameImpl_GetPM();
							ASSERT_VALID( pPM );
							if(		(	(! pPM->Ribbon_FileButtonIsItegrationSupported() )
									||	pPM->Ribbon_IsSystemMenuIconPresent()
									)
								&&	pBar->IsKindOf( RUNTIME_CLASS(CExtMenuControlBar) )
								)
							{
								CRect rc;
								pBar->GetClientRect( &rc );
								rc.right = rc.left + ::GetSystemMetrics(SM_CXICON) / 2;
								rc.bottom = rc.top + ::GetSystemMetrics(SM_CYICON) / 2;
								rc.OffsetRect( 0, ::GetSystemMetrics(SM_CXFRAME) );
								if( rc.PtInRect( ptCursor ) )
								{
									((CExtMenuControlBar*)pBar)->TrackMainFrameSystemMenu();
									return true;
								}
							}
							INT nToolBarButtonHT = pBar->HitTest( ptCursor );
							if( nToolBarButtonHT >= 0 )
							{
								lResult = ::SendMessage( hWnd, WM_LBUTTONDOWN, 0, MAKELPARAM( ptCursor.x, ptCursor.y ) );
								return true;
							}
						}
					} // if( hWnd != NULL && ::IsWindow( hWnd ) )
				} // if( wParam == ...
				break;
			} // if( NcFrameImpl_IsDwmCaptionReplacement() && m_pNcFrameImplBridge != NULL )
#endif // #if (!defined __EXT_MFC_NO_RIBBON_BAR)
			if( NcFrameImpl_IsForceEmpty() )
				break;

			if(		( ! ( m_bNcFrameImpl_Resizing || m_bNcFrameImpl_Moving ) )
				&&	( ! NcFrameImpl_IsDwmBased() )
				&&	( ( ! g_PaintManager.m_bIsWin7OrLater ) || wParam != HTCAPTION  )
				)
				m_bNcFrameImpl_Resizing = m_bNcFrameImpl_Moving = true;
			if( m_bNcFrameImpl_Resizing || m_bNcFrameImpl_Moving )
			{
				if( NcFrameImpl_IsDwmBased() )
					m_bNcFrameImpl_Resizing = m_bNcFrameImpl_Moving = false;
				if( ::GetActiveWindow() != pWndFrameImpl->m_hWnd )
					pWndFrameImpl->SetActiveWindow();
			}

			m_nNcFrameImpl_ScTrackedButtonPressed = 0;
			CRect rcWnd;
			pWndFrameImpl->GetWindowRect( &rcWnd );
			CPoint pointWnd;
			::GetCursorPos( &pointWnd );
			pointWnd -= rcWnd.TopLeft();
			if( m_rcNcFrameImpl_ScClose.PtInRect( pointWnd ) )
				m_nNcFrameImpl_ScTrackedButtonPressed = SC_CLOSE;
			else if( m_rcNcFrameImpl_ScMaximize.PtInRect( pointWnd ) )
				m_nNcFrameImpl_ScTrackedButtonPressed = SC_MAXIMIZE;
			else if( m_rcNcFrameImpl_ScMinimize.PtInRect( pointWnd ) )
				m_nNcFrameImpl_ScTrackedButtonPressed = SC_MINIMIZE;
			else if( m_rcNcFrameImpl_ScHelp.PtInRect( pointWnd ) )
				m_nNcFrameImpl_ScTrackedButtonPressed = SC_CONTEXTHELP;
			if( m_nNcFrameImpl_ScTrackedButtonPressed != 0 )
			{
				HWND hWnd = NULL;
				if(		m_pNcFrameImplBridge != NULL
					&&	( hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd() ) != NULL
					&&	(! m_rcNcFrameImpl_LastExternalNcHT.IsRectEmpty() )
					)
					::InvalidateRect( hWnd, m_rcNcFrameImpl_LastExternalNcHT, TRUE );
				pWndFrameImpl->SendMessage( WM_NCPAINT );
				if(		m_nNcFrameImpl_ScTrackedButtonPressed != 0
					&&	CWnd::GetCapture() != pWndFrameImpl
					)
					pWndFrameImpl->SetCapture();
			} // if( m_nNcFrameImpl_ScTrackedButtonPressed != 0 )
			if(		/*
			m_bNcFrameImpl_Moving
							&&*/
				wParam == HTSYSMENU
				)
			{ // system menu tracking code
				lResult = 0;
				return false;
			} // system menu tracking code
			else if(
					m_bNcFrameImpl_Moving
				&&	wParam == HTCAPTION
				)
			{ // track moving loop
				DWORD dwStyle = pWndFrameImpl->GetStyle();
				static const DWORD dwCheckMask = WS_MAXIMIZE|WS_VISIBLE|WS_DISABLED;
				DWORD dwCheckResult = dwStyle&dwCheckMask;
				if( dwCheckResult != WS_VISIBLE )
				{
					lResult = 0;
					return true;
				}
				m_bNcFrameImpl_MovingLoopTracking = true;
				HWND hWnd = pWndFrameImpl->GetSafeHwnd();
				::SendMessage( hWnd, WM_ENTERSIZEMOVE, 0L, 0L );
				::SetCapture( hWnd );
				CPoint ptStart( short(LOWORD(lParam)), short(HIWORD(lParam)) );
				CRect rcWndStart;
				::GetWindowRect( hWnd, rcWndStart );
				HANDLE hCurrentThread = ::GetCurrentThread();
				int nOldCurrentThreadPriority = ::GetThreadPriority( hCurrentThread );
				::SetThreadPriority( hCurrentThread, THREAD_PRIORITY_ABOVE_NORMAL );
				HANDLE hProcess = ::GetCurrentProcess();
				DWORD dwOldProcessPriorityClass = ::GetPriorityClass( hProcess );
				::SetPriorityClass( hProcess, HIGH_PRIORITY_CLASS );
				bool bStop = false, bStartDetected = false;
				for( ; ! bStop ; )
				{
					::WaitMessage();
					MSG msg;
					// Process all the messages in the message queue
					while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
					{
						switch( msg.message )
						{
						case WM_ACTIVATEAPP:
						case WM_CANCELMODE:
						case WM_LBUTTONDOWN:
						case WM_LBUTTONUP:
						case WM_LBUTTONDBLCLK:
						case WM_MBUTTONDOWN:
						case WM_MBUTTONUP:
						case WM_MBUTTONDBLCLK:
						case WM_RBUTTONDOWN:
						case WM_RBUTTONUP:
						case WM_RBUTTONDBLCLK:
						case WM_NCLBUTTONDOWN:
						case WM_NCLBUTTONUP:
						case WM_NCLBUTTONDBLCLK:
						case WM_NCRBUTTONDOWN:
						case WM_NCRBUTTONUP:
						case WM_NCRBUTTONDBLCLK:
						case WM_NCMBUTTONDOWN:
						case WM_NCMBUTTONUP:
						case WM_NCMBUTTONDBLCLK:
						case WM_CONTEXTMENU:
							bStop = true;
						break;
						case WM_CAPTURECHANGED:
							if( hWnd != ((HWND)lParam) )
								bStop = true;
						break;
						case WM_SIZE:
						case WM_WINDOWPOSCHANGING:
						case WM_WINDOWPOSCHANGED:
						case WM_ERASEBKGND:
						case WM_NCHITTEST:
						case WM_NCMOUSEMOVE:
							if( ::IsWindow( hWnd ) )
							{
								::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE );
								continue;
							}
							bStop = true;
						break;
						case WM_MOUSEMOVE:
							::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE );
							if( msg.hwnd == hWnd )
							{
//								while( ::PeekMessage( &msg, hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE ) )
//								{
//									if( ::PeekMessage( &msg, hWnd, WM_NCLBUTTONUP, WM_NCLBUTTONUP, PM_NOREMOVE ) )
//										break;
//								}
								CPoint pt;
								if( ! ::GetCursorPos( &pt ) )
								{
									bStop = true;
									break;
								}
								if( ( ::__EXT_MFC_GetWindowLong(hWnd,GWL_STYLE) & WS_CHILD ) != 0 )
								{
									HWND hWndParent = ::GetParent( hWnd );
									ASSERT( hWndParent != NULL );
									CRect rcWndParent;
									::GetWindowRect( hWndParent, &rcWndParent );
									pt.x = max( pt.x, rcWndParent.left );
									pt.x = min( pt.x, rcWndParent.right );
									pt.y = max( pt.y, rcWndParent.top );
									pt.y = min( pt.y, rcWndParent.bottom );
								}
								CPoint ptOffset(
									pt.x - ptStart.x,
									pt.y - ptStart.y
									);
								if(		(! bStartDetected )
									&&	abs( ptOffset.x ) < 5
									&&	abs( ptOffset.y ) < 5
									)
									continue;
								bStartDetected = true;
								CRect rcWnd = rcWndStart;
								rcWnd.OffsetRect( ptOffset );
								CRect rcWndCurrent;
								::GetWindowRect( hWnd, &rcWndCurrent );
								if( rcWndCurrent != rcWnd )
								{
									if( ( ::__EXT_MFC_GetWindowLong(hWnd,GWL_STYLE) & WS_CHILD ) != 0 )
									{
										HWND hWndParent = ::GetParent( hWnd );
										ASSERT( hWndParent != NULL );
										::ScreenToClient( hWndParent, ((LPPOINT)&rcWnd) );
										::ScreenToClient( hWndParent, ((LPPOINT)&rcWnd)+1 );
									}
									CRect rcWndAdjusted = rcWnd;
									if( ::SendMessage( hWnd, WM_MOVING, 0L, LPARAM(&rcWndAdjusted) ) != 0L )
										rcWnd = rcWndAdjusted;
									::MoveWindow( hWnd, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), TRUE );
									SendMessage( hWnd, WM_NCPAINT, 0, 0 );
									CExtPaintManager::stat_PassPaintMessages();
									::Sleep(1);
								} // if( rcWndCurrent != rcWnd )
							} // if( msg.hwnd == hWnd )
 							if( ::IsWindow( hWnd ) )
 								continue;
							bStop = true;
						break;
						default:
							if(	WM_KEYFIRST <= msg.message
								&& msg.message <= WM_KEYLAST
								)
								bStop = true;
						break;
						} // switch( msg.message )
						if( ! ::IsWindow( hWnd ) )
							bStop = true;
						if( bStop )
							break;
						CWinThread * pThread = AfxGetThread();
						if( pThread == NULL )
							::DispatchMessage( &msg );
						else
						if( ! pThread->PumpMessage() )
						{
							PostQuitMessage(0);
							break; // Signal WM_QUIT received
						} // if( !AfxGetThread()->PumpMessage() )
					} // while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
					if( bStop )
						break;
					for(	LONG nIdleCounter = 0L;
							true; // ::AfxGetThread()->OnIdle(nIdleCounter);
							nIdleCounter ++
							)
					{
						CWinThread * pThread = AfxGetThread();
						if( pThread == NULL )
							break;
						if( ! pThread->OnIdle( nIdleCounter ) )
							break;
					}
				} // for( ; ! bStop ; )
				::SetThreadPriority( hCurrentThread, nOldCurrentThreadPriority );
				::CloseHandle( hCurrentThread );
				::SetPriorityClass( hProcess, dwOldProcessPriorityClass );
				::CloseHandle( hProcess );
				if( ::IsWindow( hWnd ) )
				{
					m_bNcFrameImpl_MovingLoopTracking = false;
					if( ::GetCapture() == hWnd )
						::ReleaseCapture();
				}
				::SendMessage( hWnd, WM_EXITSIZEMOVE, 0L, 0L );
				lResult = 0;
				return true;
			} // track moving loop
			else if(
					m_bNcFrameImpl_Resizing
				&&		(	wParam == HTLEFT
						||	wParam == HTRIGHT
						||	wParam == HTTOP
						||	wParam == HTBOTTOM
						||	wParam == HTTOPLEFT
						||	wParam == HTBOTTOMLEFT
						||	wParam == HTTOPRIGHT
						||	wParam == HTBOTTOMRIGHT
						)
				)
			{ // track resizing loop
				DWORD dwStyle = pWndFrameImpl->GetStyle();
				static const DWORD dwCheckMask = WS_MAXIMIZE|WS_MINIMIZE|WS_VISIBLE|WS_DISABLED;
				DWORD dwCheckResult = dwStyle&dwCheckMask;
				if( dwCheckResult != WS_VISIBLE )
				{
					lResult = 0;
					return true;
				}
				WPARAM wParamSizingCode = 0L;
				switch( wParam )
				{
				case HTTOP:           wParamSizingCode = WMSZ_TOP;           break;
				case HTBOTTOM:        wParamSizingCode = WMSZ_BOTTOM;        break;
				case HTLEFT:          wParamSizingCode = WMSZ_LEFT;          break;
				case HTRIGHT:         wParamSizingCode = WMSZ_RIGHT;         break;
				case HTTOPLEFT:       wParamSizingCode = WMSZ_TOPLEFT;       break;
				case HTTOPRIGHT:      wParamSizingCode = WMSZ_TOPRIGHT;      break;
				case HTBOTTOMLEFT:    wParamSizingCode = WMSZ_BOTTOMLEFT;    break;
				case HTBOTTOMRIGHT:   wParamSizingCode = WMSZ_BOTTOMRIGHT;   break;
				} // switch( wParam )
				ASSERT( wParamSizingCode != 0L );
				HWND hWnd = pWndFrameImpl->GetSafeHwnd();
				::SendMessage( hWnd, WM_ENTERSIZEMOVE, 0L, 0L );
				::SetCapture( hWnd );
				CPoint ptStart( short(LOWORD(lParam)), short(HIWORD(lParam)) );
				CRect rcWndStart;
				::GetWindowRect( hWnd, rcWndStart );
				HANDLE hCurrentThread = ::GetCurrentThread();
				int nOldCurrentThreadPriority = ::GetThreadPriority( hCurrentThread );
				::SetThreadPriority( hCurrentThread, THREAD_PRIORITY_ABOVE_NORMAL );
				HANDLE hProcess = ::GetCurrentProcess();
				DWORD dwOldProcessPriorityClass = ::GetPriorityClass( hProcess );
				::SetPriorityClass( hProcess, HIGH_PRIORITY_CLASS );

				m_bNcFrameImpl_ResizingLoopTracking = true;
				MINMAXINFO _mmi;
				::memcpy( &_mmi, &m_NcFrameImpl_MinMaxInfo_Cache, sizeof(MINMAXINFO) );
				NcFrameImpl_GetMinMaxInfo( &_mmi );

				bool bStop = false;
				for( ; ! bStop ; )
				{
					::WaitMessage();
					MSG msg;
					// Process all the messages in the message queue
					while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
					{
						switch( msg.message )
						{
						case WM_ACTIVATEAPP:
						case WM_CANCELMODE:
						case WM_LBUTTONDOWN:
						case WM_LBUTTONUP:
						case WM_LBUTTONDBLCLK:
						case WM_MBUTTONDOWN:
						case WM_MBUTTONUP:
						case WM_MBUTTONDBLCLK:
						case WM_RBUTTONDOWN:
						case WM_RBUTTONUP:
						case WM_RBUTTONDBLCLK:
						case WM_NCLBUTTONDOWN:
						case WM_NCLBUTTONUP:
						case WM_NCLBUTTONDBLCLK:
						case WM_NCRBUTTONDOWN:
						case WM_NCRBUTTONUP:
						case WM_NCRBUTTONDBLCLK:
						case WM_NCMBUTTONDOWN:
						case WM_NCMBUTTONUP:
						case WM_NCMBUTTONDBLCLK:
						case WM_CONTEXTMENU:
							bStop = true;
						break;
						case WM_CAPTURECHANGED:
							if( hWnd != ((HWND)lParam) )
								bStop = true;
						break;
						case WM_SIZE:
						case WM_WINDOWPOSCHANGING:
						case WM_WINDOWPOSCHANGED:
						case WM_ERASEBKGND:
						case WM_NCHITTEST:
						case WM_NCMOUSEMOVE:
							if( ::IsWindow( hWnd ) )
							{
								::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE );
								continue;
							}
							bStop = true;
						break;
						case WM_MOUSEMOVE:
							::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE );
							if( msg.hwnd == hWnd )
							{
//								while( ::PeekMessage( &msg, hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE ) )
//								{
//									if( ::PeekMessage( &msg, hWnd, WM_NCLBUTTONUP, WM_NCLBUTTONUP, PM_NOREMOVE ) )
//										break;
//								}
								CPoint pt;
								if( ! ::GetCursorPos( &pt ) )
								{
									bStop = true;
									break;
								}
								CRect rcWnd = rcWndStart;
								if(		wParam == HTLEFT
									||	wParam == HTTOPLEFT
									||	wParam == HTBOTTOMLEFT
									)
								{
									rcWnd.left = rcWndStart.left + pt.x - ptStart.x;
									if( rcWnd.Width() < _mmi.ptMinTrackSize.x )
										rcWnd.left = rcWnd.right - _mmi.ptMinTrackSize.x;
									if( rcWnd.Width() > _mmi.ptMaxTrackSize.x )
										rcWnd.left = rcWnd.right - _mmi.ptMaxTrackSize.x;
									//if( rcWnd.left > rcWndStart.left )
									//	rcWnd.left = rcWndStart.left;
								}
								if(		wParam == HTRIGHT
									||	wParam == HTTOPRIGHT
									||	wParam == HTBOTTOMRIGHT
									)
								{
									rcWnd.right = rcWndStart.right + pt.x - ptStart.x;
									if( rcWnd.Width() < _mmi.ptMinTrackSize.x )
										rcWnd.right = rcWnd.left + _mmi.ptMinTrackSize.x;
									if( rcWnd.Width() > _mmi.ptMaxTrackSize.x )
										rcWnd.right = rcWnd.left + _mmi.ptMaxTrackSize.x;
									//if( rcWnd.right < rcWndStart.right )
									//	rcWnd.right = rcWndStart.right;
								}
								if(		wParam == HTTOP
									||	wParam == HTTOPLEFT
									||	wParam == HTTOPRIGHT
									)
								{
									rcWnd.top = rcWndStart.top + pt.y - ptStart.y;
									if( rcWnd.Height() < _mmi.ptMinTrackSize.y )
										rcWnd.top = rcWnd.bottom - _mmi.ptMinTrackSize.y;
									if( rcWnd.Height() > _mmi.ptMaxTrackSize.y )
										rcWnd.top = rcWnd.bottom - _mmi.ptMaxTrackSize.y;
									//if( rcWnd.top > rcWndStart.top )
									//	rcWnd.top = rcWndStart.top;
								}
								if(		wParam == HTBOTTOM
									||	wParam == HTBOTTOMLEFT
									||	wParam == HTBOTTOMRIGHT
									)
								{
									rcWnd.bottom = rcWndStart.bottom + pt.y - ptStart.y;
									if( rcWnd.Height() < _mmi.ptMinTrackSize.y )
										rcWnd.bottom = rcWnd.top + _mmi.ptMinTrackSize.y;
									if( rcWnd.Height() > _mmi.ptMaxTrackSize.y )
										rcWnd.bottom = rcWnd.top + _mmi.ptMaxTrackSize.y;
									//if( rcWnd.bottom < rcWndStart.bottom )
									//	rcWnd.bottom = rcWndStart.bottom;
								}
								CRect rcWndCurrent;
								::GetWindowRect( hWnd, &rcWndCurrent );
								if( rcWndCurrent != rcWnd )
								{
									UINT nIDFirst = 0, nIDLast = 0xFFFF, nIDLeftOver = AFX_IDW_PANE_FIRST;
									AFX_SIZEPARENTPARAMS layout = { NULL, { 0, 0, 0, 0 }, { 0, 0 }, FALSE };
									HWND hWndLeftOver = NULL;
									layout.bStretch = TRUE;
									CRect rcClientCurrent;
									::GetClientRect( hWnd, &rcClientCurrent );
									layout.rect.right = rcClientCurrent.right + rcWnd.Width() - rcWndCurrent.Width();
									layout.rect.bottom = rcClientCurrent.bottom + rcWnd.Height() - rcWndCurrent.Height();
									layout.hDWP = ::BeginDeferWindowPos( 8 );
									HWND hWndChild = ::GetTopWindow( hWnd );
									for( ; hWndChild != NULL; hWndChild = ::GetNextWindow( hWndChild, GW_HWNDNEXT ) )
									{
										UINT nIDC = _AfxGetDlgCtrlID( hWndChild );
										if( nIDC == nIDLeftOver )
											hWndLeftOver = hWndChild;
										else
											if(		nIDC >= nIDFirst
												&&	nIDC <= nIDLast
												)
												::SendMessage( hWndChild, WM_SIZEPARENT, 0, (LPARAM)&layout );
									}
									if( nIDLeftOver != 0 && hWndLeftOver != NULL )
									{
										CWnd * pLeftOver = CWnd::FromHandle( hWndLeftOver );
										pLeftOver->CalcWindowRect( &layout.rect );
										::AfxRepositionWindow( &layout, hWndLeftOver, &layout.rect );
									}
									if(		layout.hDWP == NULL
										||	( ! ::EndDeferWindowPos( layout.hDWP ) )
										)
									{
										TRACE0( "Warning: DeferWindowPos failed - low system resources.\r\n" );
									}
									if( ( ::__EXT_MFC_GetWindowLong(hWnd,GWL_STYLE) & WS_CHILD ) != 0 )
									{
										HWND hWndParent = ::GetParent( hWnd );
										ASSERT( hWndParent != NULL );
										::ScreenToClient( hWndParent, ((LPPOINT)&rcWnd) );
										::ScreenToClient( hWndParent, ((LPPOINT)&rcWnd)+1 );
									}
									CRect rcWndAdjusted = rcWnd;
									if( ::SendMessage( hWnd, WM_SIZING, wParamSizingCode, LPARAM(&rcWndAdjusted) ) != 0L )
										rcWnd = rcWndAdjusted;
									::MoveWindow( hWnd, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), TRUE );
									SendMessage( hWnd, WM_NCPAINT, 0, 0 );
									CExtPaintManager::stat_PassPaintMessages();
									::Sleep(1);
								} // if( rcWndCurrent != rcWnd )
							} // if( msg.hwnd == hWnd )
 							if( ::IsWindow( hWnd ) )
 								continue;
							bStop = true;
						break;
						default:
							if(	WM_KEYFIRST <= msg.message
								&& msg.message <= WM_KEYLAST
								)
								bStop = true;
						break;
						} // switch( msg.message )
						if( ! ::IsWindow( hWnd ) )
							bStop = true;
						if( bStop )
							break;
						if( ! AfxGetThread()->PumpMessage() )
						{
							PostQuitMessage(0);
							break; // Signal WM_QUIT received
						} // if( !AfxGetThread()->PumpMessage() )
					} // while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
					if( bStop )
						break;
					for(	LONG nIdleCounter = 0L;
							::AfxGetThread()->OnIdle(nIdleCounter);
							nIdleCounter ++
							);
				} // for( ; ! bStop ; )
				::SetThreadPriority( hCurrentThread, nOldCurrentThreadPriority );
				::CloseHandle( hCurrentThread );
				::SetPriorityClass( hProcess, dwOldProcessPriorityClass );
				::CloseHandle( hProcess );
				if( ::IsWindow( hWnd ) )
				{
					m_bNcFrameImpl_ResizingLoopTracking = false;
					if( ::GetCapture() == hWnd )
						::ReleaseCapture();
				}
				::SendMessage( hWnd, WM_EXITSIZEMOVE, 0L, 0L );
				lResult = 0;
				return true;
			} // track resizing loop

//			DWORD dw = 0 ;
//			if( dw != 0 )
//				pWndFrameImpl->ModifyStyle( WS_CAPTION, 0 );
			struct friendly_wnd_t : public CWnd { friend class CExtNcFrameImpl; };
			lResult = ((friendly_wnd_t*)pWndFrameImpl)->DefWindowProc( message, wParam, lParam );
//			if( dw != 0 )
//				pWndFrameImpl->ModifyStyle( 0, dw );
			return true;

		}
	break;
	case WM_LBUTTONDOWN:
		{
			if( NcFrameImpl_IsForceEmpty() )
				break;
			if( pWndFrameImpl->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)) )
				pWndFrameImpl->GetParent()->SendMessage( WM_MDIACTIVATE, (WPARAM)pWndFrameImpl->m_hWnd );
			m_nNcFrameImpl_ScTrackedButtonPressed = 0;
			CRect rcWnd;
			pWndFrameImpl->GetWindowRect( &rcWnd );
			CPoint pointWnd;
			::GetCursorPos( &pointWnd );
			pointWnd -= rcWnd.TopLeft();
			m_rcNcFrameImpl_ScClose    = NcFrameImpl_GetNcScRect( SC_CLOSE );
			m_rcNcFrameImpl_ScMaximize = NcFrameImpl_GetNcScRect( SC_MAXIMIZE );
			m_rcNcFrameImpl_ScMinimize = NcFrameImpl_GetNcScRect( SC_MINIMIZE );
			m_rcNcFrameImpl_ScHelp     = NcFrameImpl_GetNcScRect( SC_CONTEXTHELP );
			if( m_rcNcFrameImpl_ScClose.PtInRect( pointWnd ) )
				m_nNcFrameImpl_ScTrackedButtonPressed = SC_CLOSE;
			else if( m_rcNcFrameImpl_ScMaximize.PtInRect( pointWnd ) )
				m_nNcFrameImpl_ScTrackedButtonPressed = SC_MAXIMIZE;
			else if( m_rcNcFrameImpl_ScMinimize.PtInRect( pointWnd ) )
				m_nNcFrameImpl_ScTrackedButtonPressed = SC_MINIMIZE;
			else if( m_rcNcFrameImpl_ScHelp.PtInRect( pointWnd ) )
				m_nNcFrameImpl_ScTrackedButtonPressed = SC_CONTEXTHELP;
			if( m_nNcFrameImpl_ScTrackedButtonPressed != 0 )
			{
				if( ::GetActiveWindow() != pWndFrameImpl->m_hWnd )
					pWndFrameImpl->SetActiveWindow();
				pWndFrameImpl->SendMessage( WM_NCPAINT );
				HWND hWnd = NULL;
				if(		m_pNcFrameImplBridge != NULL
					&&	( hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd() ) != NULL
					&&	(! m_rcNcFrameImpl_LastExternalNcHT.IsRectEmpty() )
					)
					::InvalidateRect( hWnd, m_rcNcFrameImpl_LastExternalNcHT, TRUE );
				if(		m_nNcFrameImpl_ScTrackedButtonPressed != 0
					&&	CWnd::GetCapture() != pWndFrameImpl
					)
					pWndFrameImpl->SetCapture();
			} // if( m_nNcFrameImpl_ScTrackedButtonPressed != 0 )
		}
	break;
	case WM_LBUTTONUP:
		{
			HWND hWnd = NULL;
			if(		m_pNcFrameImplBridge != NULL
				&&	( hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd() ) != NULL
				&&	(! m_rcNcFrameImpl_LastExternalNcHT.IsRectEmpty() )
				)
			{
				m_nNcFrameImpl_LastExternalNcHT = UINT(-1);
				::InvalidateRect( hWnd, m_rcNcFrameImpl_LastExternalNcHT, TRUE );
				//::UpdateWindow( hWnd );
				m_rcNcFrameImpl_LastExternalNcHT.SetRect( -1, -1, -1, -1 );
			}

			if( NcFrameImpl_IsForceEmpty() )
				break;
			if( m_nNcFrameImpl_ScTrackedButtonPressed == 0 )
				break;
			UINT nSC = m_nNcFrameImpl_ScTrackedButtonPressed;
			if( nSC == SC_MAXIMIZE && pWndFrameImpl->IsZoomed() )
				nSC = SC_RESTORE;
			else if( nSC == SC_MINIMIZE && pWndFrameImpl->IsIconic() )
				nSC = SC_RESTORE;
			m_nNcFrameImpl_ScTrackedButtonHover = m_nNcFrameImpl_ScTrackedButtonPressed = 0;
			m_wndNcFrameImpl_Tip.Hide();
			if( CWnd::GetCapture() == pWndFrameImpl )
				::ReleaseCapture();
			pWndFrameImpl->SendMessage( WM_NCPAINT );
			CRect rcButton = NcFrameImpl_GetNcScRect( nSC );
			CRect rcWnd;
			pWndFrameImpl->GetWindowRect( &rcWnd );
			CPoint pointWnd;
			::GetCursorPos( &pointWnd );
			pointWnd -= rcWnd.TopLeft();
			if( rcButton.PtInRect(pointWnd) )
			{
				HWND hWndOwn = pWndFrameImpl->GetSafeHwnd();
				ASSERT( hWndOwn != NULL && ::IsWindow(hWndOwn) );
				bool bEnabled = NcFrameImpl_OnQuerySystemCommandEnabled( nSC );
				if( ! bEnabled )
				{
					if( nSC == SC_MAXIMIZE )
						bEnabled = NcFrameImpl_OnQuerySystemCommandEnabled( SC_RESTORE );
					else if( nSC == SC_RESTORE )
						bEnabled = NcFrameImpl_OnQuerySystemCommandEnabled( SC_MAXIMIZE );
				}
				if( bEnabled )
				{
					if(		nSC == SC_MAXIMIZE
						&&	pWndFrameImpl->IsIconic()
						&&	pWndFrameImpl->IsKindOf( RUNTIME_CLASS(CMDIChildWnd) )
						&&	g_PaintManager.m_bIsWinVistaOrLater
						)
						pWndFrameImpl->SendMessage( WM_SYSCOMMAND, SC_RESTORE );
					if(		nSC != SC_MAXIMIZE
						||	( nSC == SC_MAXIMIZE && (! pWndFrameImpl->IsZoomed() ) )
						)
						pWndFrameImpl->SendMessage( WM_SYSCOMMAND, nSC );
					if( ::IsWindow( hWndOwn ) )
					{
						if( nSC == SC_CLOSE )
							pWndFrameImpl->SendMessage( WM_NCPAINT );
					} // if( ::IsWindow( hWndOwn ) )
				}
			} // if( rcButton.PtInRect(pointWnd) )
		}
		lResult = 0;
		return true;
	case WM_MOUSEMOVE:
		{
			HWND hWnd = NULL;
			if(		m_pNcFrameImplBridge != NULL
				&&	( hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd() ) != NULL
				&&	(! m_rcNcFrameImpl_LastExternalNcHT.IsRectEmpty() )
				)
				::InvalidateRect( hWnd, m_rcNcFrameImpl_LastExternalNcHT, TRUE );

			if( NcFrameImpl_IsForceEmpty() )
				break;
			if( m_nNcFrameImpl_ScTrackedButtonHover == 0 )
				break;
			CRect rcWnd;
			pWndFrameImpl->GetWindowRect( &rcWnd );
			CPoint pointWnd;
			if( ::GetCursorPos( &pointWnd ) )
			{
				pWndFrameImpl->PostMessage( WM_NCHITTEST, 0, MAKELPARAM( pointWnd.x, pointWnd.y ) );
				pointWnd -= rcWnd.TopLeft();
			}
			else
				pointWnd.x = pointWnd.y = -32767;
			UINT nScTrackedButtonHover = 0;
			if( m_rcNcFrameImpl_ScClose.PtInRect( pointWnd ) )
				nScTrackedButtonHover = SC_CLOSE;
			else if( m_rcNcFrameImpl_ScMaximize.PtInRect( pointWnd ) )
				nScTrackedButtonHover = SC_MAXIMIZE;
			else if( m_rcNcFrameImpl_ScMinimize.PtInRect( pointWnd ) )
				nScTrackedButtonHover = SC_MINIMIZE;
			else if( m_rcNcFrameImpl_ScHelp.PtInRect( pointWnd ) )
				nScTrackedButtonHover = SC_CONTEXTHELP;
			if( m_nNcFrameImpl_ScTrackedButtonHover != nScTrackedButtonHover )
			{
				m_nNcFrameImpl_ScTrackedButtonHover = nScTrackedButtonHover;
				pWndFrameImpl->SendMessage( WM_NCPAINT );
				if(		m_nNcFrameImpl_ScTrackedButtonHover == 0
					&&	m_nNcFrameImpl_ScTrackedButtonPressed == 0
					)
				{
					if( CWnd::GetCapture() == pWndFrameImpl )
						::ReleaseCapture();
					m_wndNcFrameImpl_Tip.Hide();
				}
				else
				{
					LPCTSTR strTipText = NcFrameImpl_GetScTipText( m_nNcFrameImpl_ScTrackedButtonHover );
					if( strTipText == NULL || _tcslen(strTipText) == 0 )
						m_wndNcFrameImpl_Tip.Hide();
					else
					{
						CRect rcItem = NcFrameImpl_GetNcScRect( m_nNcFrameImpl_ScTrackedButtonHover );
						rcItem.OffsetRect( rcWnd.TopLeft() );
						m_wndNcFrameImpl_Tip.SetText( strTipText );
						m_wndNcFrameImpl_Tip.Show( pWndFrameImpl, rcItem );
					}
				}
				lResult = 0;
				return true;
			} // if( m_nNcFrameImpl_ScTrackedButtonHover != nScTrackedButtonHover )
		}
		lResult = 0;
		return true;
	case WM_NCMOUSEMOVE:
		if( ! NcFrameImpl_IsSupported() )
			break;
		else
		{

			HWND hWnd = NULL;
			if(		m_pNcFrameImplBridge != NULL
				&&	( hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd() ) != NULL
				&&	(! m_rcNcFrameImpl_LastExternalNcHT.IsRectEmpty() )
				)
				::InvalidateRect( hWnd, m_rcNcFrameImpl_LastExternalNcHT, TRUE );

			if( NcFrameImpl_IsDwmCaptionReplacement() && m_pNcFrameImplBridge != NULL )
			{
				if( wParam == HTCAPTION || wParam == HTSYSMENU )
				{
					hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
					if( hWnd != NULL && ::IsWindow( hWnd ) )
					{
						CPoint ptCursor;
						::GetCursorPos( &ptCursor );
						::ScreenToClient( hWnd, &ptCursor );
						CExtControlBar * pBar = DYNAMIC_DOWNCAST( CExtControlBar, CWnd::FromHandlePermanent( hWnd ) );
						if( pBar != NULL )
						{
							if( pBar->_OnMouseMoveMsg( 0, ptCursor ) )
							{
								lResult = 0L;
								return true;
							}
						}
						else
						{
							lResult = ::SendMessage( hWnd, WM_MOUSEMOVE, 0, MAKELPARAM( ptCursor.x, ptCursor.y ) );
							return true;
						}
					} // if( hWnd != NULL && ::IsWindow( hWnd ) )
				} // if( wParam == HTCAPTION || wParam == HTSYSMENU )
				break;
			} // if( NcFrameImpl_IsDwmCaptionReplacement() && m_pNcFrameImplBridge != NULL )
			if( NcFrameImpl_IsForceEmpty() )
				break;
			if( m_rcNcFrameImpl_ScClose.IsRectEmpty() )
				NcFrameImpl_ReCacheScButtonRects();
			CRect rcWnd;
			pWndFrameImpl->GetWindowRect( &rcWnd );
			CPoint pointWnd;
			if( ::GetCursorPos( &pointWnd ) )
			{
				pWndFrameImpl->PostMessage( WM_NCHITTEST, 0, MAKELPARAM( pointWnd.x, pointWnd.y ) );
				pointWnd -= rcWnd.TopLeft();
			}
			else
				pointWnd.x = pointWnd.y = -32767;
			UINT nScTrackedButtonHover = 0;
			if( m_rcNcFrameImpl_ScClose.PtInRect( pointWnd ) )
				nScTrackedButtonHover = SC_CLOSE;
			else if( m_rcNcFrameImpl_ScMaximize.PtInRect( pointWnd ) )
				nScTrackedButtonHover = SC_MAXIMIZE;
			else if( m_rcNcFrameImpl_ScMinimize.PtInRect( pointWnd ) )
				nScTrackedButtonHover = SC_MINIMIZE;
			else if( m_rcNcFrameImpl_ScHelp.PtInRect( pointWnd ) )
				nScTrackedButtonHover = SC_CONTEXTHELP;
			if( m_nNcFrameImpl_ScTrackedButtonHover != nScTrackedButtonHover )
			{
				m_nNcFrameImpl_ScTrackedButtonHover = nScTrackedButtonHover;
				pWndFrameImpl->SendMessage( WM_NCPAINT );
				if( m_nNcFrameImpl_ScTrackedButtonHover != 0 )
				{
					if( CWnd::GetCapture() != pWndFrameImpl )
						pWndFrameImpl->SetCapture();
					LPCTSTR strTipText = NcFrameImpl_GetScTipText( m_nNcFrameImpl_ScTrackedButtonHover );
					if( strTipText == NULL || _tcslen(strTipText) == 0 )
						m_wndNcFrameImpl_Tip.Hide();
					else
					{
						CRect rcItem = NcFrameImpl_GetNcScRect( m_nNcFrameImpl_ScTrackedButtonHover );
						rcItem.OffsetRect( rcWnd.TopLeft() );
						m_wndNcFrameImpl_Tip.SetText( strTipText );
						m_wndNcFrameImpl_Tip.Show( pWndFrameImpl, rcItem );
					}
				}
				else
					m_wndNcFrameImpl_Tip.Hide();
			} // if( m_nNcFrameImpl_ScTrackedButtonHover != nScTrackedButtonHover )
		}
		lResult = 0;
	return true;
	case WM_SYSCOMMAND:
		{
			if( ! NcFrameImpl_IsSupported() )
				break;
			if( NcFrameImpl_IsDwmBased() )
				break;
			CExtPopupMenuWnd::CancelMenuTracking();
			CWnd::CancelToolTips();
			CExtPopupMenuSite::g_DefPopupMenuSite.GetTip().Hide();
			m_wndNcFrameImpl_Tip.Hide();
			if( m_wndNcFrameImpl_Tip.GetSafeHwnd() != NULL )
				m_wndNcFrameImpl_Tip.DestroyWindow();
			UINT nSC = UINT( wParam );
			if( nSC == SC_SIZE || nSC == SC_NEXTWINDOW || nSC == SC_PREVWINDOW )
				break;
			if( ! NcFrameImpl_OnQuerySystemCommandEnabled( nSC ) )
			{
				lResult = 0;
				return true;
			}
			pWndFrameImpl->ModifyStyle( 0, WS_CAPTION|WS_BORDER, 0 );
			if( NcFrameImpl_OnQueryQuickWindowPlacement() )
			{
				if(		nSC == SC_MAXIMIZE
					||	nSC == SC_MINIMIZE
					||	nSC == SC_RESTORE
					)
				{
					WINDOWPLACEMENT _wp;
					::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
					if( GetWindowPlacement( &_wp ) )
					{
						switch( nSC )
						{
						case SC_MAXIMIZE:
							_wp.showCmd = SW_SHOWMAXIMIZED;
						break;
						case SC_MINIMIZE:
							m_nNcFrameImpl_LastShowCmd = _wp.showCmd;
							_wp.showCmd = SW_SHOWMINIMIZED;
						break;
						case SC_RESTORE:
							_wp.showCmd =
								( m_nNcFrameImpl_LastShowCmd != SW_HIDE )
									? m_nNcFrameImpl_LastShowCmd
									: SW_RESTORE // SW_SHOWNORMAL
									;
							m_nNcFrameImpl_LastShowCmd = SW_HIDE;
							if( _wp.showCmd == SW_SHOWMAXIMIZED && pWndFrameImpl->IsZoomed() )
								_wp.showCmd = SW_RESTORE;
						break;
#ifdef _DEBUG
						default:
							ASSERT( FALSE );
						break;
#endif // _DEBUG
						} // switch( nSC )
						HWND hWndSurface = NULL;
						if(		_wp.showCmd == SW_SHOWMAXIMIZED
							&&	pWndFrameImpl->IsKindOf(RUNTIME_CLASS(CMDIChildWnd))
							)
						{
							CRect rcSurface;
							pWndFrameImpl->GetWindowRect( &rcSurface );
							hWndSurface =
								::CreateWindowEx(
									0, _T("Static"), _T(""), WS_POPUP,
									rcSurface.left,
									rcSurface.top,
									rcSurface.Width()
										+ (		(	_wp.showCmd == SW_SHOWNORMAL
												&&	g_PaintManager.m_bIsWinXPorLater
												) ? 1 : 0  // +1 for recomputing Window HRGN on WindowsXP or Later
											),
									rcSurface.Height(),
									pWndFrameImpl->m_hWnd, (HMENU)NULL, ::AfxGetInstanceHandle(), NULL
									);
							if( hWndSurface != NULL )
							{
								::EnableWindow( hWndSurface, FALSE );
								::ShowWindow( hWndSurface, SW_SHOWNOACTIVATE );
							} // if( hWndSurface != NULL )
						}
						SetWindowPlacement( &_wp );
						if( hWndSurface != NULL )
							::DestroyWindow( hWndSurface );
					} // if( GetWindowPlacement( hWndOwn, &_wp ) )
					lResult = 0;
					return true;
				}
			} // if( NcFrameImpl_OnQueryQuickWindowPlacement() )
			if( nSC == SC_MINIMIZE && pWndFrameImpl->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)) )
			{ // fix for minimizing of maximized MDI child frame
				HWND hWndMdiClient = ::GetParent( pWndFrameImpl->m_hWnd );
				BOOL bMax = FALSE;
				HWND hWndActiveMdiChildFrame = (HWND) ::SendMessage( hWndMdiClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMax );
				if( hWndActiveMdiChildFrame != NULL && bMax )
				{
					bool bNextAvailable = false;
					HWND hWnd = ::GetWindow( hWndActiveMdiChildFrame, GW_HWNDNEXT );
					for( ; hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
					{
						__EXT_MFC_LONG_PTR dwWndStyle = ::__EXT_MFC_GetWindowLong( hWnd, GWL_STYLE );
						if( ( dwWndStyle & WS_VISIBLE ) == 0 )
							continue;
						if( IsIconic( hWnd ) )
							continue;
						bNextAvailable = true;
						break;
					}
					if( ! bNextAvailable )
					{
						hWnd = ::GetWindow( hWndActiveMdiChildFrame, GW_HWNDPREV );
						for( ; hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDPREV ) )
						{
							__EXT_MFC_LONG_PTR dwWndStyle = ::__EXT_MFC_GetWindowLong( hWnd, GWL_STYLE );
							if( ( dwWndStyle & WS_VISIBLE ) == 0 )
								continue;
							if( IsIconic( hWnd ) )
								continue;
							bNextAvailable = true;
							break;
						}
					}
					if( bNextAvailable )
					{
						CRect rcSurface;
						::GetClientRect( hWndMdiClient, &rcSurface );
						HWND hWndSurface =
							::CreateWindowEx(
								0, _T("Static"), _T(""), WS_CHILD, rcSurface.left, rcSurface.top, rcSurface.Width(), rcSurface.Height(),
								hWndMdiClient, (HMENU)NULL, ::AfxGetInstanceHandle(), NULL
								);
						if( hWndSurface != NULL )
						{
							::EnableWindow( hWndSurface, FALSE );
							::ShowWindow( hWndSurface, SW_SHOWNOACTIVATE );
						}
						::SendMessage( hWndMdiClient, WM_MDINEXT, WPARAM(pWndFrameImpl->m_hWnd), 0L );
						struct friendly_wnd_t : public CWnd { friend class CExtNcFrameImpl; };
						lResult = ((friendly_wnd_t*)pWndFrameImpl)->DefWindowProc( message, wParam, lParam );
						hWndActiveMdiChildFrame = (HWND) ::SendMessage( hWndMdiClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMax );
						if( hWndActiveMdiChildFrame != NULL )
						{
							::SendMessage( hWndMdiClient, WM_MDIMAXIMIZE, WPARAM(hWndActiveMdiChildFrame), 0L );
							::SendMessage( hWndMdiClient, WM_MDIACTIVATE, WPARAM(hWndActiveMdiChildFrame), 0L );
						}
						::DestroyWindow( hWndSurface );
						return true;
					} // if( bNextAvailable )
				} // if( hWndActiveMdiChildFrame != NULL && bMax )
			} // fix for minimizing of maximized MDI child frame
			if(		m_bNcFrameImpl_IsEnabled
				&&	( ! m_bNcFrameImpl_RestoreEnabledState )
				&&	nSC != SC_MOVE
				&&	nSC != SC_SIZE
				&&	pWndFrameImpl->IsKindOf( RUNTIME_CLASS( CMDIChildWnd ) )
				)
				m_bNcFrameImpl_RestoreEnabledState = true;
			if( nSC != SC_CLOSE )
				NcFrameImpl_NcLock( true );
		}
	break;
	case WM_DESTROY:
	case WM_NCDESTROY:
		m_bNcFrameImpl_DwmSurfaceInitializationComplete = false;
		m_bNcFrameImpl_CreatePassed = false;
		m_bNcFrameImpl_DestroyPassed = true;
	case WM_SETTINGCHANGE:
		NcFrameImpl_MapHtRects_Clean();
		if( ! NcFrameImpl_IsSupported() )
			break;
		if( NcFrameImpl_IsDwmBased() )
			break;
		if( wParam == SPI_SETWORKAREA )
		{
			WINDOWPLACEMENT _wp;
			::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
			_wp.length = sizeof(WINDOWPLACEMENT);
			if( GetWindowPlacement( _wp ) && _wp.showCmd == SW_SHOWMAXIMIZED )
			{
				CExtPaintManager::monitor_parms_t _mp;
				CExtPaintManager::stat_GetMonitorParms( _mp, pWndFrameImpl );
				WINDOWPLACEMENT _wpAdjust;
				::memcpy( &_wpAdjust, &_wp, sizeof(WINDOWPLACEMENT) );
				_wpAdjust.showCmd = SW_SHOWNORMAL; // SW_RESTORE;
//				pWndFrameImpl->GetWindowRect( &_wpAdjust.rcNormalPosition );
				::CopyRect( &_wpAdjust.rcNormalPosition, &_mp.m_rcWorkArea );
				_wpAdjust.ptMaxPosition.x = _wpAdjust.ptMaxPosition.x = _wpAdjust.rcNormalPosition.left;
				_wpAdjust.ptMaxPosition.y = _wpAdjust.ptMaxPosition.y = _wpAdjust.rcNormalPosition.top;
				SetWindowPlacement( _wpAdjust );
				SetWindowPlacement( _wp );
				//NcFrameImpl_RecalcNcFrame();
			}
		}
	break;
	case WM_DISPLAYCHANGE:
		NcFrameImpl_MapHtRects_Clean();
	break;
	case WM_GETMINMAXINFO:
		if( m_bNcFrameImpl_ResizingLoopTracking || m_bNcFrameImpl_MovingLoopTracking )
			 break;
		if( NcFrameImpl_IsSupported() )
		{
			LPMINMAXINFO pMMI = (LPMINMAXINFO)lParam;
			ASSERT( pMMI != NULL );
			struct friendly_wnd_t : public CWnd { friend class CExtNcFrameImpl; };
			((friendly_wnd_t*)pWndFrameImpl)->DefWindowProc( message, wParam, lParam );
			bool bOK = NcFrameImpl_GetMinMaxInfo( pMMI );
			::memcpy( &m_NcFrameImpl_MinMaxInfo_Cache, pMMI, sizeof(MINMAXINFO) );
			if( bOK )
				return true;
		} // if( NcFrameImpl_IsSupported() )
	break;
	} // switch( message )
	return false;
}

bool CExtNcFrameImpl::NcFrameImpl_OnQuerySystemCommandEnabled(
	UINT nSystemCommandID
	)
{
CWnd * pWndFrameImpl = NcFrameImpl_GetFrameWindow();
	if( pWndFrameImpl == NULL )
		return false;
	if( nSystemCommandID == SC_CLOSE )
	{
		CMenu * pSysMenu = pWndFrameImpl->GetSystemMenu( FALSE );
		if( pSysMenu == NULL )
			return false;
		UINT _nSystemCommandID = nSystemCommandID;
		UINT nMenuItemState =
			pSysMenu->GetMenuState( _nSystemCommandID, MF_BYCOMMAND );
		if( nMenuItemState == 0xFFFFFFFF )
			return false;
		if( (nMenuItemState&(MF_DISABLED|MF_GRAYED)) != 0 )
			return false;
	}
DWORD dwStyle = pWndFrameImpl->GetStyle();
	if( nSystemCommandID == SC_MINIMIZE )
	{
		if( (dwStyle&WS_MINIMIZEBOX) == 0 )
			return false;
	}
	if( nSystemCommandID == SC_MAXIMIZE )
	{
		if( (dwStyle&WS_MAXIMIZEBOX) == 0 )
			return false;
	}
	if( nSystemCommandID == SC_RESTORE || nSystemCommandID == SC_MAXIMIZE )
	{
		MINMAXINFO _mmi;
		::memcpy( &_mmi, &m_NcFrameImpl_MinMaxInfo_Cache, sizeof(MINMAXINFO) );
		NcFrameImpl_GetMinMaxInfo( &_mmi );
		if( nSystemCommandID == SC_RESTORE )
		{
			WINDOWPLACEMENT _wp;
			::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
			_wp.length = sizeof(WINDOWPLACEMENT);
			GetWindowPlacement( &_wp );
			if(		_wp.showCmd == SW_SHOWMINIMIZED
				||	_wp.showCmd == SW_SHOWMINNOACTIVE
				||	_wp.showCmd == SW_RESTORE
				||	_wp.showCmd == SW_SHOWMAXIMIZED
				)
				return true;
		}
		if(		_mmi.ptMinTrackSize.x >= _mmi.ptMaxTrackSize.x
			||	_mmi.ptMinTrackSize.y >= _mmi.ptMaxTrackSize.y
			)
			return false;
	}
	return true;
}

CExtPaintManager * CExtNcFrameImpl::NcFrameImpl_GetPM()
{
	return m_BridgeNC.PmBridge_GetPM();
}

CExtPaintManager * CExtNcFrameImpl::NcFrameImpl_GetPM() const
{
	return
		( const_cast < CExtNcFrameImpl * > ( this ) )
			-> NcFrameImpl_GetPM();
}

CExtNcFrameImpl * CExtNcFrameImpl::NcFrameImpl_FindInstance(
	HWND hWnd,
	IExtNcFrameImplBridge * pNcFrameImplBridge // = NULL
	)
{
	for( ; hWnd != NULL && ::IsWindow( hWnd ); hWnd = ::GetParent( hWnd ) )
	{
		if( ( ::__EXT_MFC_GetWindowLong( hWnd, GWL_STYLE ) & WS_CHILD ) != 0 )
			continue;
		CExtNcFrameImpl * pExtNcFrameImpl = NULL;
		::SendMessage(
			hWnd,
			g_nMsgFindExtNcFrameImpl,
			WPARAM(&pExtNcFrameImpl),
			LPARAM(pNcFrameImplBridge)
			);
		if( pExtNcFrameImpl != NULL )
			return pExtNcFrameImpl;
	} // for( ; hWnd != NULL && ::IsWindow( hWnd ); hWnd = ::GetParent( hWnd ) )
	return NULL;
}

void CExtNcFrameImpl::NcFrameImpl_CheckCursor(
	CPoint pointScreen,
	LPARAM nHT,
	bool bCheckWindowFromPoint // = true
	)
{
	if(		(! NcFrameImpl_IsSupported() )
		||	NcFrameImpl_IsDwmBased()
		||	(! ::GetCursorPos(&pointScreen) ) // reset
		)
	{
		m_nNcFrameImpl_LastCheckCursorHT = HTNOWHERE;
		m_ptNcFrameImpl_LastCheckCursor.x = m_ptNcFrameImpl_LastCheckCursor.y = -32767;
		return;
	}
HWND hWndOwn = NcFrameImpl_OnQueryHWND();
	if(		hWndOwn == NULL
		||	(	bCheckWindowFromPoint
			&&	::WindowFromPoint( pointScreen ) != hWndOwn
			)
		)
	{
		m_nNcFrameImpl_LastCheckCursorHT = HTNOWHERE;
		m_ptNcFrameImpl_LastCheckCursor.x = m_ptNcFrameImpl_LastCheckCursor.y = -32767;
		return;
	}
	if(		m_ptNcFrameImpl_LastCheckCursor == pointScreen
		&&	m_nNcFrameImpl_LastCheckCursorHT == nHT
		)
		return;
	m_ptNcFrameImpl_LastCheckCursor = pointScreen;
	m_nNcFrameImpl_LastCheckCursorHT = nHT;
	::SendMessage( hWndOwn, WM_SETCURSOR, WPARAM(hWndOwn), nHT );
}

void CExtNcFrameImpl::NcFrameImpl_PostWindowProc( LRESULT & lResult, UINT message, WPARAM wParam, LPARAM lParam )
{
	lResult;
	if( ! m_bNcFrameImpl_IsEnabled )
		return;
	switch( message )
	{
	case WM_ENABLE:
		if( m_bNcFrameImpl_IsActive && wParam == 0 )
		{
			m_bNcFrameImpl_IsActive = false;
			if(		NcFrameImpl_IsSupported()
				&&	( ! NcFrameImpl_IsDwmBased() )
				)
			{
				CWnd * pWndFrameImpl = (CWnd *)NcFrameImpl_GetFrameWindow();
				pWndFrameImpl->SendMessage( WM_NCPAINT );
			}
		}
	break;
	case WM_CAPTURECHANGED:
		{
			if( ! NcFrameImpl_IsSupported() )
				break;
			if( NcFrameImpl_IsDwmBased() )
				break;
 			CWnd * pWndFrameImpl = (CWnd *)NcFrameImpl_GetFrameWindow();
			if( ((HWND)lParam) != pWndFrameImpl->m_hWnd )
				pWndFrameImpl->ModifyStyle( 0, WS_CAPTION|WS_BORDER, 0 );
		}
	break;
 	case WM_NCPAINT:
		if( NcFrameImpl_IsDwmCaptionReplacement() && m_pNcFrameImplBridge != NULL )
		{
			HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
			if( hWnd != NULL && ::IsWindow( hWnd ) )
			{
 				CWnd * pWndFrameImpl = (CWnd *)NcFrameImpl_GetFrameWindow();
 				ASSERT_VALID( pWndFrameImpl );
 				CWindowDC dcWnd( pWndFrameImpl );
				CRect rcWnd, rcClient;
				pWndFrameImpl->GetWindowRect( &rcWnd );
				pWndFrameImpl->GetClientRect( &rcClient );
				pWndFrameImpl->ClientToScreen( &rcClient );
				rcClient.OffsetRect( -rcWnd.TopLeft() );
				rcWnd.OffsetRect( -rcWnd.TopLeft() );
				dcWnd.ExcludeClipRect( &rcClient );
				CExtMemoryDC dc( &dcWnd, &rcWnd );
				dc.FillSolidRect( &rcWnd, RGB(0,0,0) );
				CPoint ptViewPortOffset(
					::GetSystemMetrics( SM_CXSIZEFRAME ),
					::GetSystemMetrics( SM_CYCAPTION )
						- NcFrameImpl_GetPM()->NcFrame_GetCaptionHeight( true, pWndFrameImpl )
						+ ::GetSystemMetrics( SM_CYSIZEFRAME )
					);
				dc.OffsetViewportOrg( ptViewPortOffset.x, ptViewPortOffset.y );
				::SendMessage( hWnd, WM_PRINTCLIENT, WPARAM(dc.m_hDC), PRF_CLIENT );
				dc.OffsetViewportOrg( -ptViewPortOffset.x, -ptViewPortOffset.y );
			} // if( hWnd != NULL && ::IsWindow( hWnd ) )
		} // if( NcFrameImpl_IsDwmCaptionReplacement() && m_pNcFrameImplBridge != NULL )
 	break;
	case 0x031E: // WM_DWMCOMPOSITIONCHANGED
		{
			CExtPaintManager * pPM = NcFrameImpl_GetPM();
			ASSERT_VALID( pPM );
//TRACE( "---------------> WM_DWMCOMPOSITIONCHANGED\r\n" );
			m_BridgeNC.PmBridge_OnPaintManagerChanged( pPM );
//			if( m_bNcFrameImpl_PivotPmSyncMode )
//				m_BridgeNC._AdjustThemeSettings();
			CWnd * pWndFrameImpl = NcFrameImpl_GetFrameWindow();
			if( pWndFrameImpl->GetSafeHwnd() != NULL )
			{
				if( pWndFrameImpl->IsKindOf( RUNTIME_CLASS(CFrameWnd) ) )
					((CFrameWnd*)pWndFrameImpl)->RecalcLayout();
				BOOL bDwmIsCompositionEnabled = FALSE;
				if(		m_pNcFrameImplBridge != NULL
					&&	m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd() != NULL
					&&	( ! pWndFrameImpl->IsZoomed() )
					&&	g_PaintManager.m_DWM.DwmIsCompositionEnabled( & bDwmIsCompositionEnabled ) == S_OK
					&&	( ! bDwmIsCompositionEnabled )
					)
					CWnd::GetDesktopWindow()->RedrawWindow(
						NULL, NULL,
						RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN
						);
			}
		}
	break;
	case WM_CREATE:
		m_bNcFrameImpl_CreatePassed = true;
	break;
	case WM_PRINT:
	case WM_PRINTCLIENT:
		{
			CWnd * pWndFrameImpl = (CWnd *)NcFrameImpl_GetFrameWindow();
			ASSERT_VALID( pWndFrameImpl );

			CDC dc;
			dc.Attach( (HDC)wParam );

			CRect rcRgnWnd, rcRgnClient;
			pWndFrameImpl->GetWindowRect( &rcRgnWnd );
			pWndFrameImpl->GetClientRect( &rcRgnClient );

			if(		(lParam&PRF_NONCLIENT) != 0
				&&	NcFrameImpl_IsSupported()
				&&	( ! NcFrameImpl_IsDwmBased() )
				)
			{
				CRect rcWnd = rcRgnWnd, rcClient = rcRgnClient;
				pWndFrameImpl->ClientToScreen( &rcClient );
				rcClient.OffsetRect( -rcWnd.TopLeft() );
				rcWnd.OffsetRect( -rcWnd.TopLeft() );
				CRgn rgnWnd;
				if( rgnWnd.CreateRectRgnIndirect(&rcWnd) )
					dc.SelectClipRgn( &rgnWnd );
				dc.ExcludeClipRect( &rcClient );
				NcFrameImpl_OnNcPaint( dc );
				dc.SelectClipRgn( NULL );
			}
			if( (lParam&PRF_CHILDREN) != 0 )
				CExtPaintManager::stat_PrintChildren(
					pWndFrameImpl->m_hWnd,
					message,
					dc.GetSafeHdc(),
					lParam,
					false
					);
			dc.Detach();
		} // if( NcFrameImpl_IsSupported() && ( ! NcFrameImpl_IsDwmBased() ) )
	break;
	case WM_NCLBUTTONDBLCLK:
		if( NcFrameImpl_IsDwmBased() )
			break;
		m_bNcFrameImpl_RestoreBorder = false;
	break;
	case WM_SIZE:
		{
			if( ! NcFrameImpl_IsSupported() )
				break;
			m_nNcFrameImpl_ScTrackedButtonPressed = 0;
			m_nNcFrameImpl_ScTrackedButtonHover = 0;
			NcFrameImpl_MapHtRects_Clean();
			CWnd * pWndFrameImpl = NcFrameImpl_GetFrameWindow();
			if( ! NcFrameImpl_IsDwmBased() )
			{
				if( (pWndFrameImpl->GetExStyle()&WS_EX_LAYOUTRTL) != 0 )
				{
					m_bNcFrameImpl_DelatayedFrameRecalc = false;
					NcFrameImpl_SetupRgn( NULL );
				}
			}
			CMDIFrameWnd * pMdiFrame = DYNAMIC_DOWNCAST( CMDIFrameWnd, pWndFrameImpl );
			if( pMdiFrame != NULL )
			{
				HWND hWndMdiClient = CExtControlBar::stat_FindMdiClientHWND( pMdiFrame->m_hWnd );
				if( hWndMdiClient != NULL )
				{
					BOOL bMax = FALSE;
					HWND hWndActiveMdiChildFrame = (HWND) ::SendMessage( hWndMdiClient, WM_MDIGETACTIVE, 0, (LPARAM)&bMax );
					::SendMessage( hWndActiveMdiChildFrame, WM_NCMOUSEMOVE, 0L, 0L );
				}
			}
		}
	break;
	case WM_WINDOWPOSCHANGING:
		if( ! NcFrameImpl_IsSupported() )
			break;
		if( NcFrameImpl_IsDwmBased() )
			break;
		if( ! NcFrameImpl_IsForceEmpty() )
		{
			if( m_bNcFrameImpl_AssigningRGN )
				break;
			LPWINDOWPOS lpWindowPos = 
				reinterpret_cast < LPWINDOWPOS > (lParam);
			ASSERT( lpWindowPos != NULL );
			//lpWindowPos->flags |= SWP_FRAMECHANGED;
			m_bNcFrameImpl_DelatayedFrameRecalc =
				( ( lpWindowPos->flags & SWP_FRAMECHANGED ) == 0 )
					? true : false;
			CWnd * pWndFrameImpl = NcFrameImpl_GetFrameWindow();
			if(		pWndFrameImpl->IsKindOf( RUNTIME_CLASS(CMDIChildWnd) )
				&&	(pWndFrameImpl->GetStyle()&WS_MAXIMIZE) != 0
				)
			{
				m_bNcFrameImpl_AssigningRGN = true;
				pWndFrameImpl->SetWindowRgn( NULL, FALSE );
				m_bNcFrameImpl_AssigningRGN = false;
			}
			else
				NcFrameImpl_SetupRgn( (WINDOWPOS *)lParam );
		}
	break;
	case WM_WINDOWPOSCHANGED:
		{
			if( ! NcFrameImpl_IsSupported() )
				break;
			if( NcFrameImpl_IsDwmBased() )
				break;
			m_bNcFrameImpl_DelatayedFrameRecalc = false;
			CWnd * pWndFrameImpl = NcFrameImpl_GetFrameWindow();
			if(		pWndFrameImpl->IsKindOf(RUNTIME_CLASS(CMDIChildWnd))
				&&	pWndFrameImpl->IsZoomed()
				)
			{
				HWND hWndMdiClient = ::GetParent( pWndFrameImpl->m_hWnd );
				CRect rcClient;
				::GetClientRect( hWndMdiClient, &rcClient );
				::SendMessage( hWndMdiClient, WM_SIZE, SIZE_RESTORED, MAKELPARAM( rcClient.Width(), rcClient.Height() ) );
				//NcFrameImpl_SetupRgn();
			}
			if( ! m_bNcFrameImpl_RestoreEnabledState )
				break;
			pWndFrameImpl->SendMessage( WM_NCPAINT );
			if( m_pNcFrameImplBridge != NULL )
			{
				HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
				if( hWnd != NULL )
				{
					::InvalidateRect( hWnd, NULL, TRUE );
					::UpdateWindow( hWnd );
				}
			}
		}
		// continuing switch ...
	case WM_SYSCOMMAND:
		{
			if( ! NcFrameImpl_IsSupported() )
				break;
			if( NcFrameImpl_IsDwmBased() )
				break;
			UINT nSC = UINT( wParam );
			if( nSC == SC_SIZE || nSC == SC_NEXTWINDOW || nSC == SC_PREVWINDOW )
				break;
			CWnd * pWndFrameImpl = NcFrameImpl_GetFrameWindow();
			if(		pWndFrameImpl->GetSafeHwnd() != NULL
				&&	m_bNcFrameImpl_RestoreEnabledState
				)
				m_bNcFrameImpl_IsEnabled = true;
			m_bNcFrameImpl_RestoreEnabledState = false;
			if( message == WM_SYSCOMMAND && nSC != SC_CLOSE )
				NcFrameImpl_NcLock( false );
			if(		pWndFrameImpl->IsKindOf( RUNTIME_CLASS(CMDIChildWnd) )
				&&	(pWndFrameImpl->GetStyle()&WS_MAXIMIZE) != 0
				)
			{
				m_bNcFrameImpl_AssigningRGN = true;
				pWndFrameImpl->SetWindowRgn( NULL, TRUE );
				m_bNcFrameImpl_AssigningRGN = false;
				((CMDIChildWnd*)pWndFrameImpl)->RecalcLayout();
				pWndFrameImpl->GetParent()->PostMessage( WM_MDIREFRESHMENU );
			}
			else
				NcFrameImpl_SetupRgn();
			if( pWndFrameImpl->IsWindowVisible() )
			{
				if( message == WM_SYSCOMMAND && ( nSC == SC_MAXIMIZE || nSC == SC_RESTORE ) )
				{
					if( nSC == SC_MAXIMIZE )
					{
						CMDIChildWnd * pWndMdiChild = DYNAMIC_DOWNCAST( CMDIChildWnd, pWndFrameImpl );
						if( pWndMdiChild != NULL )
						{
							CRect rcWnd;
							pWndMdiChild->GetWindowRect( &rcWnd );
							pWndMdiChild->SendMessage( WM_NCHITTEST, MAKELPARAM( rcWnd.left, rcWnd.top ) );
							CWnd * pWndMC = pWndFrameImpl->GetParent();
							CRect rcMC;
							pWndMC->GetWindowRect( &rcMC );
							pWndMC->SendMessage( WM_SIZE, SIZE_RESTORED, MAKELPARAM(rcMC.Width(),rcMC.Height()) );
							pWndMC->SendMessage( WM_MDIACTIVATE, (WPARAM)pWndFrameImpl->m_hWnd );
						}
					}
					pWndFrameImpl->RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_ERASE|RDW_FRAME|RDW_ALLCHILDREN );
				}
				else
					pWndFrameImpl->SendMessage( WM_NCPAINT );
			}
			if(		message == WM_SYSCOMMAND
				&&	nSC == SC_RESTORE
				&&	m_pNcFrameImplBridge != NULL
				)
			{
				HWND hWnd = m_pNcFrameImplBridge->NcFrameImplBridge_GetSafeHwnd();
				if( hWnd != NULL )
				{
					CWnd * pWndPermanent = CWnd::FromHandlePermanent( hWnd );
					if( pWndPermanent != NULL )
					{
						CExtToolControlBar * pBar = DYNAMIC_DOWNCAST( CExtToolControlBar, pWndPermanent );
						if( pBar != NULL )
						{
							pBar->_RibbonPageRslaResetStateData();
							pBar->_RecalcLayoutImpl();
						}
					}
				}
			}
		} // case WM_SYSCOMMAND
	break;
	} // switch( message )
}

bool CExtNcFrameImpl::NcFrameImpl_OnQueryQuickWindowPlacement() const
{
	if( ! NcFrameImpl_IsSupported() )
		return false;
	if( g_PaintManager.m_DWM.IsCompositionEnabled() )
		return false;
	if( m_bNcFrameImpl_QuickWindowPlacement )
		return true;
const CWnd * pWndFrameImpl = NcFrameImpl_GetFrameWindow();
	if( pWndFrameImpl != NULL && pWndFrameImpl->IsKindOf( RUNTIME_CLASS(CMDIChildWnd) ) )
		return true;
	return false;
}

bool CExtNcFrameImpl::NcFrameImpl_RecalcNcFrame()
{
	if( NcFrameImpl_IsNcLocked() )
		return false;
HWND hWndOwn = NcFrameImpl_OnQueryHWND();
	if( hWndOwn == NULL )
		return false;
	ASSERT( ::IsWindow( hWndOwn ) );
	::SetWindowPos(
		hWndOwn, NULL, 0, 0, 0, 0,
		SWP_FRAMECHANGED
			|SWP_NOACTIVATE
			|SWP_NOZORDER
			|SWP_NOOWNERZORDER
			|SWP_NOMOVE
			|SWP_NOSIZE
		);
	return true;
}

void CExtNcFrameImpl::PreSubclassWindow()
{
	m_bNcFrameImpl_DestroyPassed = false;
	::memset( &m_NcFrameImpl_MinMaxInfo_Cache, 0, sizeof(MINMAXINFO) );
HWND hWndOwn = NcFrameImpl_OnQueryHWND();
	ASSERT( hWndOwn != NULL && ::IsWindow( hWndOwn ) );
	m_dwNcFrameImpl_StyleInitial = ::__EXT_MFC_GetWindowLong( hWndOwn, GWL_STYLE );
	m_dwNcFrameImpl_StyleExInitial = ::__EXT_MFC_GetWindowLong( hWndOwn, GWL_EXSTYLE );
	if( m_bNcFrameImpl_IsDwmBased && (m_dwNcFrameImpl_StyleInitial&WS_CHILD) != 0 )
		m_bNcFrameImpl_IsDwmBased = false;
	if( m_bNcFrameImpl_IsDwmBased )
		m_bNcFrameImpl_Moving = m_bNcFrameImpl_Resizing = false;
	m_bNcFrameImpl_HelperLastThemeSupportedNC = NcFrameImpl_IsSupported();
	if(		(m_dwNcFrameImpl_StyleInitial&(WS_BORDER|WS_CAPTION)) != 0
		&&	m_bNcFrameImpl_HelperLastThemeSupportedNC
		)
		NcFrameImpl_DelayRgnAdjustment();
	NcFrameImpl_MapHtRects_Clean();
	NcFrameImpl_RecalcNcFrame();
	{ // BLOCK: adjust MDI client area styles
		HWND hWndMdiClient = CExtControlBar::stat_FindMdiClientHWND( hWndOwn );
		if( hWndMdiClient != NULL )
		{
			__EXT_MFC_LONG_PTR dwStyle = ::__EXT_MFC_GetWindowLong( hWndMdiClient, GWL_STYLE );
			if( (dwStyle&WS_BORDER) != 0 )
				::__EXT_MFC_SetWindowLong( hWndMdiClient, GWL_STYLE, dwStyle&(~(WS_BORDER)) );
			__EXT_MFC_LONG_PTR dwStyleEx = ::__EXT_MFC_GetWindowLong( hWndMdiClient, GWL_EXSTYLE );
			if( (dwStyleEx&(WS_EX_CLIENTEDGE|WS_EX_STATICEDGE|WS_EX_DLGMODALFRAME)) != 0 )
				::__EXT_MFC_SetWindowLong( hWndMdiClient, GWL_EXSTYLE, (dwStyleEx&(~(WS_EX_CLIENTEDGE|WS_EX_STATICEDGE|WS_EX_DLGMODALFRAME)))|WS_EX_CLIENTEDGE );
		}
	} // BLOCK: adjust MDI client area styles
	m_bNcFrameImpl_ResizingLoopTracking = m_bNcFrameImpl_MovingLoopTracking = false;
}

void CExtNcFrameImpl::PostNcDestroy()
{
	::memset( &m_NcFrameImpl_MinMaxInfo_Cache, 0, sizeof(MINMAXINFO) );
	m_pNcFrameImplBridge = NULL;
	NcFrameImpl_DelayRgnAdjustment();
	m_nNcFrameImpl_LastCheckCursorHT = HTNOWHERE;
	m_ptNcFrameImpl_LastCheckCursor.x = m_ptNcFrameImpl_LastCheckCursor.y = -32767;
	m_iconNcFrameImpl_QueryCache.Empty();
	m_bNcFrameImpl_NcInitPassed = false;
	m_bNcFrameImpl_ResizingLoopTracking = m_bNcFrameImpl_MovingLoopTracking = false;
}

IMPLEMENT_CExtPmBridge_MEMBERS( CExtNcFrameWatchMDIMF );

UINT CExtNcFrameWatchMDIMF::g_nMsgSurface =
	::RegisterWindowMessage( _T("CExtNcFrameWatchMDIMF::g_nMsgSurface") );

CExtNcFrameWatchMDIMF::CExtNcFrameWatchMDIMF()
	: m_pNcFrameImpl( NULL )
	, m_hWndSurface( NULL )
{
	PmBridge_Install();
}

CExtNcFrameWatchMDIMF::~CExtNcFrameWatchMDIMF()
{
	PmBridge_Uninstall();
	if( m_hWndSurface != NULL )
		::DestroyWindow( m_hWndSurface );
}

void CExtNcFrameWatchMDIMF::_Surface( bool bForward )
{
CWnd * pWndFrameImpl = m_pNcFrameImpl->NcFrameImpl_GetFrameWindow();
	if( bForward )
	{
		if( pWndFrameImpl->GetSafeHwnd() == NULL )
			return;
		if( m_hWndSurface == NULL )
		{
			HWND hWndParent = pWndFrameImpl->GetSafeHwnd();
			ASSERT( hWndParent != NULL );
			hWndParent = CExtControlBar::stat_FindMdiClientHWND( hWndParent );
			ASSERT( hWndParent != NULL );
			CRect rcSurface;
			::GetClientRect( hWndParent, &rcSurface );
			m_hWndSurface =
				::CreateWindowEx(
					0, _T("Static"), _T(""), WS_CHILD, rcSurface.left, rcSurface.top, rcSurface.Width(), rcSurface.Height(),
					hWndParent, (HMENU)NULL, ::AfxGetInstanceHandle(), NULL
					);
			if( m_hWndSurface != NULL )
			{
				::EnableWindow( m_hWndSurface, FALSE );
				::ShowWindow( m_hWndSurface, SW_SHOWNOACTIVATE );
			} // if( m_hWndSurface != NULL )
		} // if( m_hWndSurface == NULL )
	} // if( bForward )
	else
	{
		if( m_hWndSurface != NULL )
		{
			::DestroyWindow( m_hWndSurface );
			m_hWndSurface = NULL;
		} // if( m_hWndSurface != NULL )
	} // else from if( bForward )
}

static CWnd * stat_pWndInit = NULL;
bool CExtNcFrameWatchMDIMF::OnHookWndMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	UINT nMessage,
	WPARAM & wParam,
	LPARAM & lParam
	)
{
	if( m_pNcFrameImpl != NULL )
	{
		switch( nMessage )
		{
		case WM_MDIDESTROY:
		case WM_MDINEXT:
		case WM_MDIICONARRANGE:
		case WM_MDITILE:
		case WM_MDICASCADE:
		case WM_MDIACTIVATE:
		//case WM_MDICREATE:
			{
				_Surface( true );
				__EXT_MFC_LONG_PTR dwStyle = ::__EXT_MFC_GetWindowLong( hWndHooked, GWL_STYLE );
				if( (dwStyle&WS_CLIPCHILDREN|WS_CLIPCHILDREN) == 0 )
					::__EXT_MFC_SetWindowLong( hWndHooked, GWL_STYLE, dwStyle|WS_CLIPCHILDREN||WS_CLIPCHILDREN );
				{ // BLOCK: adjust MDI client area styles
					HWND hWndMdiClient = CExtControlBar::stat_FindMdiClientHWND( hWndHooked );
					if( hWndMdiClient != NULL )
					{
						__EXT_MFC_LONG_PTR dwStyle = ::__EXT_MFC_GetWindowLong( hWndMdiClient, GWL_STYLE );
						if( (dwStyle&WS_BORDER) != 0 )
							::__EXT_MFC_SetWindowLong( hWndMdiClient, GWL_STYLE, dwStyle&(~(WS_BORDER)) );
						__EXT_MFC_LONG_PTR dwStyleEx = ::__EXT_MFC_GetWindowLong( hWndMdiClient, GWL_EXSTYLE );
						if( (dwStyleEx&(WS_EX_CLIENTEDGE|WS_EX_STATICEDGE|WS_EX_DLGMODALFRAME)) != 0 )
							::__EXT_MFC_SetWindowLong( hWndMdiClient, GWL_EXSTYLE, (dwStyleEx&(~(WS_EX_CLIENTEDGE|WS_EX_STATICEDGE|WS_EX_DLGMODALFRAME)))|WS_EX_CLIENTEDGE );
					}
				} // BLOCK: adjust MDI client area styles
				if( nMessage == WM_MDICASCADE || nMessage == WM_MDITILE )
				{
					BOOL bMax = FALSE;
					HWND hWndActiveMdiChildFrame = (HWND)
						::SendMessage(
							hWndHooked,
							WM_MDIGETACTIVE,
							0,
							(LPARAM)&bMax
							);
					if( hWndActiveMdiChildFrame != NULL && bMax )
						::SendMessage(
							hWndHooked,
							WM_MDIRESTORE,
							(WPARAM)hWndActiveMdiChildFrame,
							0
							);
				} // if( nMessage == WM_MDICASCADE || nMessage == WM_MDITILE )

				switch( nMessage )
				{
				case WM_MDIICONARRANGE:
				{
					CList < HWND, HWND > _listWindowsToSkip;
					if( m_hWndSurface != NULL )
						_listWindowsToSkip.AddTail( m_hWndSurface );
					CExtNcFrameImpl::stat_MDI_ArrangeIconic( hWndHooked, _listWindowsToSkip );
					_Surface( false );
					lResult = TRUE;
				}
				return true;
				case WM_MDITILE:
				{
					CList < HWND, HWND > _listWindowsToSkip;
					if( m_hWndSurface != NULL )
						_listWindowsToSkip.AddTail( m_hWndSurface );
					CExtNcFrameImpl::stat_MDI_Tile( hWndHooked, _listWindowsToSkip, wParam );
					_Surface( false );
					lResult = TRUE;
				}
				return true;
				case WM_MDICASCADE:
				{
					CList < HWND, HWND > _listWindowsToSkip;
					if( m_hWndSurface != NULL )
						_listWindowsToSkip.AddTail( m_hWndSurface );
					CExtNcFrameImpl::stat_MDI_Cascade( hWndHooked, _listWindowsToSkip, wParam );
					_Surface( false );
					lResult = TRUE;
				}
				return true;
				} // switch( nMessage )
			}
		break;
		default:
			if( nMessage == CExtNcFrameWatchMDIMF::g_nMsgSurface )
			{
				_Surface( wParam ? true : false );
				lResult = 1L;
				return true;
			}
		break;
		} // switch( nMessage )
	} // if( m_pNcFrameImpl != NULL )
	return
		CExtHookSink::OnHookWndMsg(
			lResult,
			hWndHooked,
			nMessage,
			wParam,
			lParam
			);
}

void CExtNcFrameWatchMDIMF::OnPostHookWndMsg(
	LRESULT lResult,
	HWND hWndHooked,
	UINT nMessage,
	WPARAM wParam,
	LPARAM lParam
	)
{
	if( m_pNcFrameImpl != NULL )
	{
		switch( nMessage )
		{
		case WM_MDIDESTROY:
		case WM_MDINEXT:
		case WM_MDIICONARRANGE:
		case WM_MDITILE:
		case WM_MDICASCADE:
		case WM_MDIACTIVATE:
		//case WM_MDICREATE:
			{
				CRect rcMC;
				::GetWindowRect( hWndHooked, &rcMC );
				::SendMessage( hWndHooked, WM_SIZE, SIZE_RESTORED, MAKELPARAM(rcMC.Width(),rcMC.Height()) );
				_Surface( false );
				::RedrawWindow( hWndHooked, NULL, NULL,
					RDW_INVALIDATE|RDW_ERASE|RDW_FRAME|RDW_ALLCHILDREN
					);
			}
		break;
		} // switch( nMessage )
	} // if( m_pNcFrameImpl != NULL )
	CExtHookSink::OnPostHookWndMsg(
		lResult,
		hWndHooked,
		nMessage,
		wParam,
		lParam
		);
}

#endif // (!defined __EXT_MFC_NO_NC_FRAME )


