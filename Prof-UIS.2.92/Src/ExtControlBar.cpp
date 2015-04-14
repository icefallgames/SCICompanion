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

// extcontrolbar.cpp : implementation file
//

#include "stdafx.h"

//
// Many thanks to Paul DiLascia & Cristi Posea, their works
// were a powerful incentive to learn more about pretty
// complicated MFC docking windows mechanism
//

#if (!defined __EXT_CONTROLBAR_H)
	#include <ExtControlBar.h>
#endif

#if (!defined __EXT_RICH_CONTENT_H)
	#include <ExtRichContent.h>
#endif // (!defined __EXT_RICH_CONTENT_H)

#if (!defined __EXTDOCKBAR_H)
	#include "ExtDockBar.h"
#endif

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	#if (!defined __EXT_CONTROLBAR_TABBED_FEATURES_H)
		#include "ExtControlBarTabbedFeatures.h"
	#endif // __EXT_CONTROLBAR_TABBED_FEATURES_H
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

#if (!defined __EXTMINIDOCKFRAMEWND_H)
	#include "ExtMiniDockFrameWnd.h"
#endif

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_NC_FRAME_H)
	#include <ExtNcFrame.h>
#endif

#if (!defined __EXT_LOCALIZATION_H)
	#include <../Src/ExtLocalization.h>
#endif

#if (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)

	#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
		#if (! defined __EXT_TAB_PAGE_CONTAINER_WND_H)
			#include <ExtTabPageContainerWnd.h>
		#endif // (!defined __EXT_TAB_PAGE_CONTAINER_WND_H)
	#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)

	#if (!defined __EXT_NC_FRAME_H)
		#include <ExtNcFrame.h>
	#endif

#endif // (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)

#include <Resources/Resource.h>

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

#ifndef __AFXOLE_H__
	#include <AfxOle.h>
#endif 

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	#if (!defined __EXTCUSTOMIZE_H)
		#include <ExtCustomize.h>
	#endif
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtAnimationParameters

CExtAnimationParameters::CExtAnimationParameters(
	INT nAcStepCount, // = __EXT_MFC_DEF_ANIMATION_STEP_COUNT
	UINT nAcTimerElapse // = __EXT_MFC_DEF_ANIMATION_TIMER_ELAPSE,
	)
	: m_nAcStepCount( nAcStepCount )
	, m_nAcTimerElapse( nAcTimerElapse )
{
}

CExtAnimationParameters::~CExtAnimationParameters()
{
}

INT CExtAnimationParameters::AnimationClient_StepCountGet() const
{
	ASSERT( this != NULL );
	ASSERT( 0 <= m_nAcStepCount && m_nAcStepCount <= __EXT_MFC_DEF_ANIMATION_STEP_COUNT_MAX );
	return m_nAcStepCount;
}

void CExtAnimationParameters::AnimationClient_StepCountSet( INT nAcStepCount )
{
	ASSERT( this != NULL );
	ASSERT( 0 <= m_nAcStepCount && m_nAcStepCount <= __EXT_MFC_DEF_ANIMATION_STEP_COUNT_MAX );
	ASSERT( 0 <= nAcStepCount && nAcStepCount <= __EXT_MFC_DEF_ANIMATION_STEP_COUNT_MAX );
	m_nAcStepCount = nAcStepCount;
}

UINT CExtAnimationParameters::AnimationClient_TimerElapseGet() const
{
	ASSERT( this != NULL );
	return m_nAcTimerElapse;
}

void CExtAnimationParameters::AnimationClient_TimerElapseSet( UINT nAcTimerElapse )
{
	ASSERT( this != NULL );
	m_nAcTimerElapse = nAcTimerElapse;
}

/////////////////////////////////////////////////////////////////////////////
// CExtAnimationClient

CExtAnimationClient::CExtAnimationClient(
	CExtAnimationSite * pAcAS, // = NULL
	bool bAcEnabled, // = true
	bool bAcNonClientMode, // = false
	INT nAcStepNumber, // = 0
	INT nAcStepCount, // = __EXT_MFC_DEF_ANIMATION_STEP_COUNT
	UINT nAcTimerElapse, // = __EXT_MFC_DEF_ANIMATION_TIMER_ELAPSE,
	const RECT * pAcRcAnimationTarget // = NULL
	)
	: m_pAcAS( pAcAS )
	, m_bAcEnabled( bAcEnabled )
	, m_bAcNonClientMode( bAcNonClientMode )
	, m_nAcStepNumber( nAcStepNumber )
	, m_nAcStepCount( nAcStepCount )
	, m_nAcTimerElapse( nAcTimerElapse )
	, m_rcAcTarget( 0, 0, 0, 0 )
	, m_nAcCacheGeneratorLock( 0 )
	, m_eAPT_Last( 0 )
{
	ASSERT( 0 <= m_nAcStepNumber );
	ASSERT( 0 <= m_nAcStepCount && m_nAcStepCount <= __EXT_MFC_DEF_ANIMATION_STEP_COUNT_MAX );
	if( m_pAcAS != NULL && m_pAcAS->AnimationSite_ClientGetIndexOf( this ) < 0 )
		m_pAcAS->AnimationSite_ClientAdd( this );
	if( pAcRcAnimationTarget != NULL )
		m_rcAcTarget = (*pAcRcAnimationTarget);
}

CExtAnimationClient::~CExtAnimationClient()
{
	if( m_pAcAS != NULL )
		m_pAcAS->AnimationSite_ClientRemove( this );
}

void CExtAnimationClient::AnimationClient_CacheGeneratorLock()
{
	ASSERT( this != NULL );
	ASSERT( m_nAcCacheGeneratorLock >= 0 );
	m_nAcCacheGeneratorLock ++;
}

void CExtAnimationClient::AnimationClient_CacheGeneratorUnlock()
{
	ASSERT( this != NULL );
	ASSERT( m_nAcCacheGeneratorLock >= 0 );
	m_nAcCacheGeneratorLock --;
}

bool CExtAnimationClient::AnimationClient_CacheGeneratorIsLocked() const
{
	ASSERT( this != NULL );
	ASSERT( m_nAcCacheGeneratorLock >= 0 );
	if( ! AnimationClient_EnabledGet() )
		return true;
	if( m_nAcCacheGeneratorLock == 0 )
		return false;
	else
		return true;
}

CExtAnimationSite * CExtAnimationClient::AnimationClient_SiteGet()
{
	ASSERT( this != NULL );
	return m_pAcAS;
}

const CExtAnimationSite * CExtAnimationClient::AnimationClient_SiteGet() const
{
	ASSERT( this != NULL );
	return
		( const_cast < CExtAnimationClient * > ( this ) )
		-> AnimationClient_SiteGet();
}

void CExtAnimationClient::AnimationClient_SiteSet(
	CExtAnimationSite * pAcAS // = NULL
	)
{
	ASSERT( this != NULL );
	if( m_pAcAS == pAcAS )
		return;
	if( m_pAcAS != NULL )
		m_pAcAS->AnimationSite_ClientRemove( this );
	m_pAcAS = pAcAS;
	if( m_pAcAS != NULL && m_pAcAS->AnimationSite_ClientGetIndexOf( this ) < 0 )
		m_pAcAS->AnimationSite_ClientAdd( this );
}

CExtBitmap & CExtAnimationClient::AnimationClient_StateGet( bool bAcNext )
{
	ASSERT( this != NULL );
	if( bAcNext )
		return m_bmpAcStateNext;
	else
		return m_bmpAcStatePrev;
}

const CExtBitmap & CExtAnimationClient::AnimationClient_StateGet( bool bAcNext ) const
{
	ASSERT( this != NULL );
	return
		( const_cast < CExtAnimationClient * > ( this ) )
		-> AnimationClient_StateGet( bAcNext );
}

bool CExtAnimationClient::AnimationClient_IsEmpty() const
{
	ASSERT( this != NULL );
	if(		AnimationClient_StateGet( false ).IsEmpty()
		&&	AnimationClient_StateGet( true ).IsEmpty()
		)
		return true;
	else
		return false;
}

void CExtAnimationClient::AnimationClient_Empty()
{
	ASSERT( this != NULL );
	AnimationClient_StateGet( false ).Empty();
	AnimationClient_StateGet( true ).Empty();
	AnimationClient_StepNumberSet( 0 );
//	AnimationClient_NonClientModeSet();
//CRect rcEmpty( 0, 0, 0, 0 );
//	AnimationClient_TargetRectSet( rcEmpty );
}

INT CExtAnimationClient::AnimationClient_StepNumberGet() const
{
	ASSERT( this != NULL );
	ASSERT( 0 <= m_nAcStepNumber );
	return m_nAcStepNumber;
}

void CExtAnimationClient::AnimationClient_StepNumberSet( INT nAcStepNumber )
{
	ASSERT( this != NULL );
	ASSERT( 0 <= m_nAcStepNumber );
	ASSERT( 0 <= nAcStepNumber );
	m_nAcStepNumber = nAcStepNumber;
}

INT CExtAnimationClient::AnimationClient_StepCountGet() const
{
	ASSERT( this != NULL );
	ASSERT( 0 <= m_nAcStepCount && m_nAcStepCount <= __EXT_MFC_DEF_ANIMATION_STEP_COUNT_MAX );
	return m_nAcStepCount;
}

void CExtAnimationClient::AnimationClient_StepCountSet( INT nAcStepCount )
{
	ASSERT( this != NULL );
	ASSERT( 0 <= m_nAcStepCount && m_nAcStepCount <= __EXT_MFC_DEF_ANIMATION_STEP_COUNT_MAX );
	ASSERT( 0 <= nAcStepCount && nAcStepCount <= __EXT_MFC_DEF_ANIMATION_STEP_COUNT_MAX );
	m_nAcStepCount = nAcStepCount;
}

UINT CExtAnimationClient::AnimationClient_TimerElapseGet() const
{
	ASSERT( this != NULL );
	return m_nAcTimerElapse;
}

void CExtAnimationClient::AnimationClient_TimerElapseSet( UINT nAcTimerElapse )
{
	ASSERT( this != NULL );
	m_nAcTimerElapse = nAcTimerElapse;
}

bool CExtAnimationClient::AnimationClient_NonClientModeGet() const
{
	ASSERT( this != NULL );
	return m_bAcNonClientMode;
}

void CExtAnimationClient::AnimationClient_NonClientModeSet(
	bool bAcNonClientMode // = false
	)
{
	ASSERT( this != NULL );
	m_bAcNonClientMode = bAcNonClientMode;
}

bool CExtAnimationClient::AnimationClient_EnabledGet() const
{
	ASSERT( this != NULL );
	return m_bAcEnabled;
}

void CExtAnimationClient::AnimationClient_EnabledSet(
	bool bAcEnabled // = true
	)
{
	ASSERT( this != NULL );
	m_bAcEnabled = bAcEnabled;
}

bool CExtAnimationClient::AnimationClient_OnQueryEnabledState(
	INT eAPT // __EAPT_*** animation type
	) const
{
	ASSERT( this != NULL );
const CExtAnimationSite * pAcAS = AnimationClient_SiteGet();
	if( pAcAS == NULL )
		return false;
	if( pAcAS->AnimationSite_ClientGetIndexOf( this ) < 0 )
		return false;
	if( ! AnimationClient_EnabledGet() )
		return false;
	if( AnimationClient_IsEmpty() )
		return false;
	if( AnimationClient_TargetRectGet().IsRectEmpty() )
		return false;
INT nAcStepCount = AnimationClient_OnStepCountGet( eAPT );
	ASSERT( 0 <= nAcStepCount && nAcStepCount <= __EXT_MFC_DEF_ANIMATION_STEP_COUNT_MAX );
	if( nAcStepCount == 0 )
		return false;
INT nAcStepNumber = AnimationClient_StepNumberGet();
	ASSERT( 0 <= nAcStepNumber );
	if( nAcStepNumber >= nAcStepCount )
		return false;
	return true;
}

CRect CExtAnimationClient::AnimationClient_TargetRectGet() const
{
	ASSERT( this != NULL );
	return m_rcAcTarget;
}

void CExtAnimationClient::AnimationClient_TargetRectSet(
	const RECT & rcAcAnimationTarget
	)
{
	ASSERT( this != NULL );
	m_rcAcTarget = rcAcAnimationTarget;
}

bool CExtAnimationClient::AnimationClient_CacheNextState(
	CDC & dc,
	const RECT & rcAcAnimationTarget,
	bool bAnimate,
	INT eAPT // __EAPT_*** animation type
	)
{
	ASSERT( this != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );
CExtAnimationSite * pAcAS = AnimationClient_SiteGet();
	if( pAcAS == NULL )
		return false;
	if( pAcAS->AnimationSite_ClientGetIndexOf( this ) < 0 )
		return false;
	if( ! AnimationClient_EnabledGet() )
		return false;
	if( AnimationClient_OnStepCountGet( eAPT ) == 0 )
		return false;
	m_bmpAcStatePrev = m_bmpAcStateNext;
	m_bmpAcStateNext.FromSurface( dc, rcAcAnimationTarget );
	m_bmpAcStateNext.Make32();
	AnimationClient_NextStateAdjust( m_bmpAcStateNext );
	m_rcAcTarget = rcAcAnimationTarget;
//	if( AnimationClient_IsEmpty() )
//		return false;
	AnimationClient_StepNumberSet( 0 );
	if( bAnimate )
		pAcAS->AnimationSite_OnClientStateChanged( this, eAPT );
	else
		pAcAS->AnimationSite_ClientProgressStop( this );
	return true;
}

bool CExtAnimationClient::AnimationClient_CacheNextState(
	HWND hWnd,
	const RECT & rcAcAnimationTarget,
	bool bAnimate,
	INT eAPT // __EAPT_*** animation type
	)
{
	ASSERT( this != NULL );
	if( hWnd == NULL || ( ! ::IsWindow( hWnd ) ) )
		return false;
CExtAnimationSite * pAcAS = AnimationClient_SiteGet();
	if( pAcAS == NULL )
		return false;
	if( pAcAS->AnimationSite_ClientGetIndexOf( this ) < 0 )
		return false;
	if( ! AnimationClient_EnabledGet() )
		return false;
	if( AnimationClient_OnStepCountGet( eAPT ) == 0 )
		return false;
bool bAcNonClientMode = AnimationClient_NonClientModeGet();
HDC hDC =
		(! bAcNonClientMode )
			? ( ::GetDC( hWnd ) )
			: ( ::GetWindowDC( hWnd ) )
			;
	if( hDC == NULL )
		return false;
	if( bAcNonClientMode )
	{
		RECT rcClient, rcWnd;
		if(		::GetClientRect( hWnd, &rcClient )
			&&	::GetWindowRect( hWnd, &rcWnd )
			&&	::ClientToScreen( hWnd, LPPOINT(&rcClient) )
			&&	::ClientToScreen( hWnd, LPPOINT(&rcClient)+1 )
			&&	::OffsetRect( &rcClient, -rcWnd.left, -rcWnd.top )
			)
		{
			::ExcludeClipRect(
				hDC,
				rcClient.left,
				rcClient.top,
				rcClient.right,
				rcClient.bottom
				);
		} // if( ::GetClientRect( hWnd, &rcClient ) ...
		else
		{
			::ReleaseDC( hWnd, hDC );
			return false;
		} // else from if( ::GetClientRect( hWnd, &rcClient ) ...
	} // if( bAcNonClientMode )
bool bRetVal = false;
CDC * pDC = CDC::FromHandle( hDC );
	if( pDC != NULL )
	{
		CRect _rcAcAnimationTarget = rcAcAnimationTarget;
		CExtMemoryDC dc( pDC, &_rcAcAnimationTarget );
		AnimationClient_NextStatePrepare(
			dc,
			rcAcAnimationTarget,
			bAnimate,
			eAPT
			);
		bRetVal = 
			AnimationClient_CacheNextState(
				dc, //*pDC,
				rcAcAnimationTarget,
				bAnimate,
				eAPT
				);
		dc.__Flush( FALSE );
	}
	::ReleaseDC( hWnd, hDC );
	return bRetVal;
}

bool CExtAnimationClient::AnimationClient_CacheNextStateMinInfo(
	bool bAnimate,
	INT eAPT // __EAPT_*** animation type
	)
{
	ASSERT( this != NULL );
CExtAnimationSite * pAcAS = AnimationClient_SiteGet();
	if( pAcAS == NULL )
		return false;
HWND hWnd = pAcAS->AnimationSite_GetSafeHWND();
	if( hWnd == NULL )
		return false;
	ASSERT( ::IsWindow( hWnd ) );
CRect rcAcAnimationTarget = AnimationClient_TargetRectGet();
	return
		AnimationClient_CacheNextState(
			hWnd,
			rcAcAnimationTarget,
			bAnimate,
			eAPT
			);
}

void CExtAnimationClient::AnimationClient_NextStatePrepare(
	CDC & dc,
	const RECT & rcAcAnimationTarget,
	bool bAnimate,
	INT eAPT // __EAPT_*** animation type
	)
{
	ASSERT( this != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );
	dc;
	rcAcAnimationTarget;
	bAnimate;
	eAPT;
}

void CExtAnimationClient::AnimationClient_NextStateAdjust(
	CExtBitmap & _bmp
	)
{
	ASSERT( this != NULL );
	_bmp;
}

void CExtAnimationClient::AnimationClient_StateDoStep()
{
	ASSERT( this != NULL );
CExtAnimationSite * pAcAS = AnimationClient_SiteGet();
	if( pAcAS == NULL )
		return;
HWND hWnd = pAcAS->AnimationSite_GetSafeHWND();
	if( hWnd == NULL )
		return;
	ASSERT( ::IsWindow( hWnd ) );
CRect _rcAcAnimationTarget = AnimationClient_TargetRectGet();
 	::InvalidateRect( hWnd, &_rcAcAnimationTarget, TRUE );
//static int g_nDebugCounter = 0;
//	TRACE1( "Animation step %d\r\n", g_nDebugCounter++ );
}

bool CExtAnimationClient::AnimationClient_StatePaint(
	CDC & dc
	)
{
	ASSERT( this != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( AnimationClient_CacheGeneratorIsLocked() )
		return false;
	if( ! AnimationClient_OnQueryEnabledState( m_eAPT_Last ) )
		return false;
CRect rcAcAnimationTarget = AnimationClient_TargetRectGet();
	if( dc.RectVisible( &rcAcAnimationTarget ) )
	{
		INT nAcStepNumber = AnimationClient_StepNumberGet();
		INT nAcStepCount = AnimationClient_OnStepCountGet( m_eAPT_Last );
		ASSERT( nAcStepNumber <= nAcStepCount && nAcStepCount > 0 );
		CExtBitmap & bmpPrev = AnimationClient_StateGet( false );
		CExtBitmap & bmpNext = AnimationClient_StateGet( true );
		if( bmpPrev.IsEmpty() )
			return false;
		if( bmpNext.IsEmpty() )
			return false;
		double lfNextSCA = double(nAcStepNumber + 1) / double(nAcStepCount);
// 		lfNextSCA = double( sin( lfNextSCA * 0.8 ) );
// 		lfNextSCA = double( sin( lfNextSCA * 0.8 ) );
		if( lfNextSCA > 1.0 )
			lfNextSCA = 1.0;
		INT nNextSCA = INT( lfNextSCA * 255.0 );
		if( nNextSCA > 255 )
			nNextSCA = 255;
		ASSERT( 0 <= nNextSCA && nNextSCA <= 255 );
		int nPrevSCA = 255; // 255 - nNextSCA;
		int nOldStretchBltMode = ::GetStretchBltMode( dc.m_hDC );
		::SetStretchBltMode( dc.m_hDC, g_PaintManager.m_bIsWinNT ? HALFTONE : COLORONCOLOR );

		DWORD dwPrevOldRTF = 0, dwNextOldRTF = 0;
		bool bChangeRTF = false;
		if(		g_PaintManager.m_bIsWinVistaOrLater
			&&	g_PaintManager.m_DWM.IsCompositionEnabled()
			)
			bChangeRTF = true;
		if( bChangeRTF )
		{
			dwPrevOldRTF = bmpPrev.RunTimeFlagsGet();
			dwNextOldRTF = bmpNext.RunTimeFlagsGet();
			bmpPrev.RunTimeFlagsSet( dwPrevOldRTF | __EXT_BMP_FLAG_NO_RTL_DETECTION );
			bmpNext.RunTimeFlagsSet( dwNextOldRTF | __EXT_BMP_FLAG_NO_RTL_DETECTION );
		}

		if( nPrevSCA > 0 )
			bmpPrev.AlphaBlend(
				dc.m_hDC,
				rcAcAnimationTarget,
				BYTE(nPrevSCA)
				);
		if( nNextSCA > 0 )
			bmpNext.AlphaBlend(
				dc.m_hDC,
				rcAcAnimationTarget,
				BYTE(nNextSCA)
				);

		if( bChangeRTF )
		{
			bmpPrev.RunTimeFlagsSet( dwPrevOldRTF );
			bmpNext.RunTimeFlagsSet( dwNextOldRTF );
		}

		::SetStretchBltMode( dc.m_hDC, nOldStretchBltMode );
	} // if( dc.RectVisible( &rcAcAnimationTarget ) )
	return true;
}

bool CExtAnimationClient::AnimationClient_StatePaint(
	HWND hWnd
	)
{
	ASSERT( this != NULL );
	if( hWnd == NULL || ( ! ::IsWindow( hWnd ) ) )
		return false;
CExtAnimationSite * pAcAS = AnimationClient_SiteGet();
	if( pAcAS == NULL )
		return false;
	if( pAcAS->AnimationSite_ClientGetIndexOf( this ) < 0 )
		return false;
	if( ! AnimationClient_EnabledGet() )
		return false;
	if( AnimationClient_OnStepCountGet( m_eAPT_Last ) == 0 )
		return false;
bool bAcNonClientMode = AnimationClient_NonClientModeGet();
HDC hDC =
		bAcNonClientMode
			? ( ::GetWindowDC( hWnd ) )
			: ( ::GetDC( hWnd ) )
			;
	if( hDC == NULL )
		return false;
	if( bAcNonClientMode )
	{
		RECT rcClient, rcWnd;
		if(		::GetClientRect( hWnd, &rcClient )
			&&	::GetWindowRect( hWnd, &rcWnd )
			&&	::ClientToScreen( hWnd, LPPOINT(&rcClient) )
			&&	::ClientToScreen( hWnd, LPPOINT(&rcClient)+1 )
			&&	::OffsetRect( &rcClient, -rcWnd.left, -rcWnd.top )
			)
		{
			::ExcludeClipRect(
				hDC,
				rcClient.left,
				rcClient.top,
				rcClient.right,
				rcClient.bottom
				);
		} // if( ::GetClientRect( hWnd, &rcClient ) ...
		else
		{
			::ReleaseDC( hWnd, hDC );
			return false;
		} // else from if( ::GetClientRect( hWnd, &rcClient ) ...
	} // if( bAcNonClientMode )
bool bRetVal = false;
CDC * pDC = CDC::FromHandle( hDC );
	if( pDC != NULL )
	{
		CRect _rcAcAnimationTarget = AnimationClient_TargetRectGet();
		CExtMemoryDC dc( pDC, &_rcAcAnimationTarget );
		dc.FillSolidRect( &_rcAcAnimationTarget, 0 );
		bRetVal = AnimationClient_StatePaint( dc );
	}
	::ReleaseDC( hWnd, hDC );
	return bRetVal;
}

bool CExtAnimationClient::AnimationClient_StatePaint()
{
	ASSERT( this != NULL );
CExtAnimationSite * pAcAS = AnimationClient_SiteGet();
	if( pAcAS == NULL )
		return false;
HWND hWnd = pAcAS->AnimationSite_GetSafeHWND();
	if( hWnd == NULL )
		return false;
	ASSERT( ::IsWindow( hWnd ) );
	return AnimationClient_StatePaint( hWnd );
}

void CExtAnimationClient::AnimationClient_OnProgressStart(
	INT eAPT // __EAPT_*** animation type
	)
{
	ASSERT( this != NULL );
	eAPT;
	AnimationClient_StepNumberSet( 0 );
}

void CExtAnimationClient::AnimationClient_OnProgressStop(
	INT eAPT // __EAPT_*** animation type
	)
{
	ASSERT( this != NULL );
	AnimationClient_StepNumberSet( AnimationClient_OnStepCountGet( eAPT ) );
// 	AnimationClient_StateGet( true ).Empty();
// 	AnimationClient_StateGet( false ).Empty();
}

INT CExtAnimationClient::AnimationClient_OnStepCountGet(
	INT eAPT // __EAPT_*** animation type
	) const
{
	ASSERT( this != NULL );
const CExtAnimationParameters * pAnimationParameters =
		AnimationClient_OnQueryAnimationParameters( eAPT );
	if( pAnimationParameters != NULL )
	{
		INT nAcStepCount =
			pAnimationParameters ->
				AnimationClient_StepCountGet();
		return nAcStepCount;
	} // if( pAnimationParameters != NULL )
INT nAcStepCount =
		AnimationClient_StepCountGet();
	return nAcStepCount;
}

UINT CExtAnimationClient::AnimationClient_OnTimerElapseGet(
	INT eAPT // __EAPT_*** animation type
	) const
{
	ASSERT( this != NULL );
const CExtAnimationParameters * pAnimationParameters =
		AnimationClient_OnQueryAnimationParameters( eAPT );
	if( pAnimationParameters != NULL )
	{
		UINT nAcTimerElapse =
			pAnimationParameters ->
				AnimationClient_TimerElapseGet();
		return nAcTimerElapse;
	} // if( pAnimationParameters != NULL )
UINT nAcTimerElapse =
		AnimationClient_TimerElapseGet();
	return nAcTimerElapse;
}

const CExtAnimationParameters *
	CExtAnimationClient::AnimationClient_OnQueryAnimationParameters(
		INT eAPT // __EAPT_*** animation type
		) const
{
	ASSERT( this != NULL );
const CExtAnimationParameters * pAnimationParameters =
		g_PaintManager->Animation_GetParameters(
			eAPT,
			NULL,
			this
			);
	return pAnimationParameters;
}

INT CExtAnimationClient::AnimationClient_LastAnimationTypeGet() const
{
	ASSERT( this != NULL );
	return m_eAPT_Last;
}

void CExtAnimationClient::AnimationClient_LastAnimationTypeSet(
	INT eAPT // __EAPT_*** animation type
	)
{
	ASSERT( this != NULL );
	m_eAPT_Last = eAPT;
}

/////////////////////////////////////////////////////////////////////////////
// CExtAnimationSite

IMPLEMENT_CExtPmBridge_MEMBERS_GENERIC( CExtAnimationSite );

CExtAnimationSite::CExtAnimationSite(
	bool bAsEnabled // = true
	)
	: m_bAsEnabled( bAsEnabled )
{
//	AnimationSite_Init( false );
//	m_AcHS.AnimationSiteSet( this, false );

	PmBridge_Install();
}

CExtAnimationSite::~CExtAnimationSite()
{
	PmBridge_Uninstall();

	AnimationSite_Done();
	AnimationSite_ClientRemove();
}

HWND CExtAnimationSite::PmBridge_GetSafeHwnd() const
{
	ASSERT( this != NULL );
	return AnimationSite_GetSafeHWND();
}

void CExtAnimationSite::PmBridge_OnPaintManagerChanged(
	CExtPaintManager * pGlobalPM
	)
{
	ASSERT( this != NULL );
INT nIndex, nCount = AnimationSite_ClientGetCount();
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		CExtAnimationClient * pAC = AnimationSite_ClientGetAt( nIndex );
		ASSERT( pAC != NULL );
		AnimationSite_ClientProgressStop( pAC );
		pAC->AnimationClient_StateGet( false ).Empty();
		pAC->AnimationClient_StateGet( true ).Empty();
	} // for( nIndex = 0; nIndex < nCount; nIndex ++ )
	CExtPmBridge::PmBridge_OnPaintManagerChanged(
		pGlobalPM
		);
}

bool CExtAnimationSite::AnimationSite_EnabledGet() const
{
	ASSERT( this != NULL );
	return m_bAsEnabled;
}

void CExtAnimationSite::AnimationSite_EnabledSet(
	bool bAsEnabled // = true
	)
{
	ASSERT( this != NULL );
	m_bAsEnabled = bAsEnabled;
}

bool CExtAnimationSite::AnimationSite_OnQueryEnabledState() const
{
	ASSERT( this != NULL );
	if( ! AnimationSite_EnabledGet() )
		return false;
HWND hWnd = AnimationSite_GetSafeHWND();
	if( hWnd == NULL )
		return false;
	ASSERT( ::IsWindow( hWnd ) );
	return true;
}

INT CExtAnimationSite::AnimationSite_ClientGetCount(
	bool bAsIncludeClientArea, // = true
	bool bAsIncludeNonClientArea // = true
	) const
{
	ASSERT( this != NULL );
INT nCount = INT( m_arrAsAnimationClients.GetSize() );
	if( nCount == 0 )
		return 0;
	if( bAsIncludeClientArea )
	{
		if( bAsIncludeNonClientArea )
			return nCount;
		nCount = 0;
		INT nIndex;
		for( nIndex = 0; nIndex < nCount; nIndex ++ )
		{
			const CExtAnimationClient * pAC = m_arrAsAnimationClients.GetAt( nIndex );
			ASSERT( pAC != NULL );
			if( pAC->AnimationClient_NonClientModeGet() )
				continue;
			nCount ++;
		} // for( nIndex = 0; nIndex < nCount; nIndex ++ )
		return nCount;
	} // if( bAsIncludeClientArea )
	else
	{
		if( bAsIncludeNonClientArea )
		{
			nCount = 0;
			INT nIndex;
			for( nIndex = 0; nIndex < nCount; nIndex ++ )
			{
				const CExtAnimationClient * pAC = m_arrAsAnimationClients.GetAt( nIndex );
				ASSERT( pAC != NULL );
				if( ! pAC->AnimationClient_NonClientModeGet() )
					continue;
				nCount ++;
			} // for( nIndex = 0; nIndex < nCount; nIndex ++ )
			return nCount;
		} // if( bAsIncludeNonClientArea )
		return 0;
	} // else from if( bAsIncludeClientArea )
}

CExtAnimationClient * CExtAnimationSite::AnimationSite_ClientGetAt( INT nAcIndex )
{
	ASSERT( this != NULL );
	if( nAcIndex < 0 )
		return NULL;
INT nCount = INT( m_arrAsAnimationClients.GetSize() );
	if( nAcIndex >= nCount )
		return NULL;
CExtAnimationClient * pAC = m_arrAsAnimationClients.GetAt( nAcIndex );
	ASSERT( pAC != NULL );
	return pAC;
}

const CExtAnimationClient * CExtAnimationSite::AnimationSite_ClientGetAt( INT nAcIndex ) const
{
	ASSERT( this != NULL );
	return
		( const_cast < CExtAnimationSite * > ( this ) )
		-> AnimationSite_ClientGetAt( nAcIndex );
}

bool CExtAnimationSite::AnimationSite_ClientInsertAt( INT nAcIndex, CExtAnimationClient * pAC )
{
	ASSERT( this != NULL );
	if( pAC == NULL )
		return false;
	if( nAcIndex < 0 )
		return false;
INT nCount = INT( m_arrAsAnimationClients.GetSize() );
	if( nAcIndex > nCount )
		return false;
	if( AnimationSite_ClientGetIndexOf( pAC ) >= 0 )
		return false;
	m_arrAsAnimationClients.InsertAt( nAcIndex, pAC );
	return true;
}

bool CExtAnimationSite::AnimationSite_ClientAdd( CExtAnimationClient * pAC )
{
	ASSERT( this != NULL );
	return AnimationSite_ClientInsertAt( AnimationSite_ClientGetCount(), pAC );
}

INT CExtAnimationSite::AnimationSite_ClientGetIndexOf( const CExtAnimationClient * pAC ) const
{
	ASSERT( this != NULL );
	if( pAC == NULL )
		return -1;
INT nIndex, nCount = AnimationSite_ClientGetCount();
	if( nCount == 0 )
		return -1;
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		const CExtAnimationClient * pOwnAC = AnimationSite_ClientGetAt( nIndex );
		ASSERT( pOwnAC != NULL );
		if( pOwnAC == pAC )
			return nIndex;
	} // for( nIndex = 0; nIndex < nCount; nIndex ++ )
	return -1;
}

INT CExtAnimationSite::AnimationSite_ClientRemove(
	INT nAcIndex, // = 0
	INT nAcCount, // = -1
	bool bDelete // = true
	)
{
	ASSERT( this != NULL );
	if( nAcIndex < 0 || nAcCount == 0 )
		return 0;
INT nCount = INT( m_arrAsAnimationClients.GetSize() );
	if( nAcIndex >= nCount )
		return 0;
	if( nAcCount < 0 )
		nAcCount = nCount;
INT nIndex = nAcIndex, nCountRemoved = 0;
	for( ; nIndex < nCount && nCountRemoved < nAcCount; )
	{
		CExtAnimationClient * pAC = m_arrAsAnimationClients.GetAt( nIndex );
		ASSERT( pAC != NULL );
		m_arrAsAnimationClients.RemoveAt( nIndex, 1 );
		nCount --;
		nCountRemoved ++;
		AnimationSite_ClientProgressStop( pAC );
		if( bDelete )
			delete pAC;
	} // for( ; nIndex < nCount && nCountRemoved < nAcCount; )
	return nCountRemoved;
}

bool CExtAnimationSite::AnimationSite_ClientRemove(
	CExtAnimationClient * pAC,
	bool bDelete // = true
	)
{
	ASSERT( this != NULL );
INT nAcIndex = AnimationSite_ClientGetIndexOf( pAC );
	if( nAcIndex < 0 )
		return false;
	if( AnimationSite_ClientRemove( nAcIndex, 1, bDelete ) != 1 )
		return false;
	return true;
}

WORD CExtAnimationSite::AnimationSite_TimerGetBaseID()
{
	ASSERT( this != NULL );
	return __EXT_MFC_DEF_ANIMATION_TIMER_BASE_TIMER_ID;
}

WORD CExtAnimationSite::AnimationSite_TimerGetMaxID()
{
	ASSERT( this != NULL );
	return __EXT_MFC_DEF_ANIMATION_TIMER_MAX_TIMER_ID;
}

bool CExtAnimationSite::AnimationSite_ClientProgressStart(
	CExtAnimationClient * pAC,
	INT eAPT // __EAPT_*** animation type
	)
{
	ASSERT( this != NULL );
	if( pAC == NULL )
		return false;
	AnimationSite_ClientProgressStop( pAC );
	pAC->AnimationClient_LastAnimationTypeSet( eAPT );
HWND hWnd = AnimationSite_GetSafeHWND();
	if( hWnd == NULL )
		return false;
	ASSERT( ::IsWindow( hWnd ) );
WORD wElapse = (WORD)pAC->AnimationClient_OnTimerElapseGet( pAC->AnimationClient_LastAnimationTypeGet() );
WORD wTimerID = __EXT_MFC_DEF_ANIMATION_TIMER_INVALID_ID;
	if( ! m_mapAsAnimationTimers.Lookup( wElapse, wTimerID ) )
	{
		wTimerID = AnimationSite_TimerGetBaseID();
		WORD wLastTimerID = AnimationSite_TimerGetMaxID();
		ASSERT( wTimerID <= wLastTimerID );
		for( ; wTimerID <= wLastTimerID; wTimerID ++ )
		{
			bool bFreeTimerID = true;
			POSITION pos = m_mapAsAnimationTimers.GetStartPosition();
			for( ; pos != NULL; )
			{
				WORD wCheckTimerID, wCheckElapse;
				m_mapAsAnimationTimers.GetNextAssoc( pos, wCheckElapse, wCheckTimerID );
				if( wTimerID == wCheckTimerID )
				{
					bFreeTimerID = false;
					break;
				}
			} // for( ; pos != NULL; )
			if( bFreeTimerID )
				break;
			else
				continue;
		} // for( ; wTimerID <= wLastTimerID; wTimerID ++ )
		ASSERT( wTimerID != __EXT_MFC_DEF_ANIMATION_TIMER_INVALID_ID );
		if( wTimerID > wLastTimerID )
			return false;
		m_mapAsAnimationTimers.SetAt( wElapse, wTimerID );
		::SetTimer( hWnd, UINT(wTimerID), UINT(wElapse), NULL );
	} // if( ! m_mapAsAnimationTimers.Lookup( wElapse, wTimerID ) )
	m_mapAsAnimationClients.SetAt( pAC, wTimerID );
	pAC->AnimationClient_OnProgressStart( pAC->AnimationClient_LastAnimationTypeGet() );
	return true;
}

void CExtAnimationSite::AnimationSite_ClientProgressStop(
	CExtAnimationClient * pAC
	)
{
	ASSERT( this != NULL );
	if( pAC == NULL )
		return;
WORD wTimerID = __EXT_MFC_DEF_ANIMATION_TIMER_INVALID_ID;
	if( ! m_mapAsAnimationClients.Lookup( pAC, wTimerID ) )
		return;
	ASSERT( wTimerID != __EXT_MFC_DEF_ANIMATION_TIMER_INVALID_ID );
	m_mapAsAnimationClients.RemoveKey( pAC );
//	pAC->AnimationClient_Empty();
CRect _rcAcAnimationTarget = pAC->AnimationClient_TargetRectGet();
bool bKillTimer = true;
POSITION pos = m_mapAsAnimationClients.GetStartPosition();
	for( ; pos != NULL; )
	{
		CExtAnimationClient * pOtherAC = NULL;
		WORD wOtherTimerID = __EXT_MFC_DEF_ANIMATION_TIMER_INVALID_ID;
		m_mapAsAnimationClients.GetNextAssoc( pos, pOtherAC, wOtherTimerID );
		ASSERT( pOtherAC != NULL );
		ASSERT( wOtherTimerID != __EXT_MFC_DEF_ANIMATION_TIMER_INVALID_ID );
		if( wOtherTimerID == wTimerID )
			bKillTimer = false;
	} // for( ; pos != NULL; )
HWND hWnd = AnimationSite_GetSafeHWND();
	ASSERT( hWnd == NULL || ::IsWindow( hWnd ) );
	if( bKillTimer )
	{
		WORD wElapse = (WORD)pAC->AnimationClient_OnTimerElapseGet( pAC->AnimationClient_LastAnimationTypeGet() );
		m_mapAsAnimationTimers.RemoveKey( wElapse );
		if( hWnd != NULL )
		{
			ASSERT( ::IsWindow( hWnd ) );
			::KillTimer( hWnd, UINT(wTimerID) );
		}
	} // if( bKillTimer )
	if( hWnd != NULL )
	{
		ASSERT( ::IsWindow( hWnd ) );
		::InvalidateRect( hWnd, &_rcAcAnimationTarget, TRUE );
	}
	pAC->AnimationClient_OnProgressStop( pAC->AnimationClient_LastAnimationTypeGet() );
	if( bKillTimer )
		AnimationSite_OnProgressShutdownTimer( UINT(wTimerID) );
}

CExtAnimationSite::AnimationHookSink::AnimationHookSink(
	CExtAnimationSite * pAS // = NULL
	)
	: m_pAS( pAS )
	, m_bHookEnabled( false )
{
	AnimationInit( false );
}

CExtAnimationSite::AnimationHookSink::~AnimationHookSink()
{
	AnimationDone();
}

CExtAnimationSite * CExtAnimationSite::AnimationHookSink::AnimationSiteGet()
{
	ASSERT( this != NULL );
	return m_pAS;
}

const CExtAnimationSite * CExtAnimationSite::AnimationHookSink::AnimationSiteGet() const
{
	ASSERT( this != NULL );
	return 
		( const_cast < AnimationHookSink * > ( this ) )
		-> AnimationSiteGet();
}

void CExtAnimationSite::AnimationHookSink::AnimationSiteSet( CExtAnimationSite * pAS, bool bHookEnabled )
{
	ASSERT( this != NULL );
	if( m_pAS == pAS )
		return;
	AnimationDone();
	m_pAS = pAS;
	AnimationInit( bHookEnabled );
}

bool CExtAnimationSite::AnimationHookSink::AnimationInit( bool bHookEnabled )
{
	ASSERT( this != NULL );
CExtAnimationSite * pAS = AnimationSiteGet();
	if( pAS == NULL )
		return false;
HWND hWnd = pAS->AnimationSite_GetSafeHWND();
	if( hWnd == NULL )
		return false;
	ASSERT( ::IsWindow( hWnd ) );
	m_bHookEnabled = bHookEnabled;
	if( bHookEnabled )
		SetupHookWndSink( hWnd );
	pAS->AnimationSite_OnAnimationHookInit();
	return true;
}

void CExtAnimationSite::AnimationHookSink::AnimationDone()
{
	ASSERT( this != NULL );
//	if( bHookEnabled )
//		RemoveAllWndHooks();
CExtAnimationSite * pAS = AnimationSiteGet();
	if( pAS != NULL )
	{
//		if( m_bHookEnabled )
//		{
//			HWND hWnd = pAS->AnimationSite_GetSafeHWND();
//			if( hWnd != NULL )
//			{
//				ASSERT( ::IsWindow( hWnd ) );
//				if( IsHookedWindow( hWnd ) )
//					SetupHookWndSink( hWnd, true );
//			}
//		} // if( m_bHookEnabled )
		pAS->AnimationSite_OnAnimationHookDone();
	}
}

bool CExtAnimationSite::AnimationHookSink::OnHookWndMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	UINT nMessage,
	WPARAM & wParam,
	LPARAM & lParam
	)
{
__PROF_UIS_MANAGE_STATE;
	ASSERT( this != NULL );
CExtAnimationSite * pAS = AnimationSiteGet();
	if(		pAS != NULL
		&&	hWndHooked == pAS->AnimationSite_GetSafeHWND()
		)
	{
		if(		nMessage == WM_DESTROY
			||	nMessage == WM_NCDESTROY
			)
			AnimationDone();
		else if(
			pAS->AnimationSite_OnHookWndMsg(
				lResult,
				hWndHooked,
				nMessage,
				wParam,
				lParam
				)
			)
			return true;
	}
	return 
		CExtHookSink::OnHookWndMsg(
			lResult,
			hWndHooked,
			nMessage,
			wParam,
			lParam
			);
}

void CExtAnimationSite::AnimationSite_OnAnimationHookInit()
{
	ASSERT( this != NULL );
}

void CExtAnimationSite::AnimationSite_OnAnimationHookDone()
{
	ASSERT( this != NULL );
}

bool CExtAnimationSite::AnimationSite_OnHookWndMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	UINT nMessage,
	WPARAM & wParam,
	LPARAM & lParam
	)
{
	ASSERT( this != NULL );
	lParam;
	if(		nMessage == WM_TIMER
		&&	hWndHooked == AnimationSite_GetSafeHWND()
		)
	{
		if( CExtAnimationSite::AnimationSite_OnHookTimer( UINT(wParam) ) )
		{
			lResult = 0;
			return true;
		}
	} // if( nMessage == WM_TIMER ...
	return false;
}

bool CExtAnimationSite::AnimationSite_OnHookTimer(
	UINT nTimerID
	)
{
	ASSERT( this != NULL );
HWND hWnd = AnimationSite_GetSafeHWND();
	if( hWnd == NULL )
		return false;
	ASSERT( ::IsWindow( hWnd ) );
bool bTimerHandled = false;
POSITION pos = m_mapAsAnimationTimers.GetStartPosition();
	for( ; pos != NULL; )
	{
		WORD wTimerID, wElapse;
		m_mapAsAnimationTimers.GetNextAssoc( pos, wElapse, wTimerID );
		if( wTimerID == WORD(nTimerID) )
		{
			bTimerHandled = true;
			break;
		}
	} // for( ; pos != NULL; )
	if( ! bTimerHandled )
		return false;
INT nIndex, nCount = AnimationSite_ClientGetCount();
INT nCountAnimatedCurrent = 0, nCountAnimatedMax = 10;
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		CExtAnimationClient * pAC =
			AnimationSite_ClientGetAt(
				nCount - nIndex - 1
				);
		ASSERT( pAC != NULL );
		WORD wTimerID = __EXT_MFC_DEF_ANIMATION_TIMER_INVALID_ID;
		if( ! m_mapAsAnimationClients.Lookup( pAC, wTimerID ) )
			continue;
		ASSERT( wTimerID != __EXT_MFC_DEF_ANIMATION_TIMER_INVALID_ID );
		if( ! pAC->AnimationClient_OnQueryEnabledState( pAC->AnimationClient_LastAnimationTypeGet() ) )
		{
			AnimationSite_ClientProgressStop( pAC );
			continue;
		}
		//CRect _rcAcAnimationTarget = pAC->AnimationClient_TargetRectGet();
		if( wTimerID == WORD(nTimerID) )
		{
			INT nAcStepNumber = pAC->AnimationClient_StepNumberGet();
			nAcStepNumber ++;
			pAC->AnimationClient_StepNumberSet( nAcStepNumber );
			if(		(! pAC->AnimationClient_OnQueryEnabledState( pAC->AnimationClient_LastAnimationTypeGet() ) )
				||	nCountAnimatedCurrent > nCountAnimatedMax
				)
				AnimationSite_ClientProgressStop( pAC );
			else
			{
				pAC->AnimationClient_StateDoStep();
				nCountAnimatedCurrent ++;
			}
		} // if( wTimerID == WORD(nTimerID) )
		//::InvalidateRect( hWnd, &_rcAcAnimationTarget, TRUE );
//		pAC->AnimationClient_StateDoStep();
	} // for( nIndex = 0; nIndex < nCount; nIndex ++ )
//  	if( nCountAnimatedCurrent == 0 )
//  		::InvalidateRect( hWnd, NULL, TRUE );
//	::UpdateWindow( hWnd );
	return true;
}

bool CExtAnimationSite::AnimationSite_Init( bool bHookEnabled )
{
	ASSERT( this != NULL );
	m_AcHS.AnimationSiteSet( NULL, false );
HWND hWnd = AnimationSite_GetSafeHWND();
	if( hWnd == NULL )
		return false;
	ASSERT( ::IsWindow( hWnd ) );
	m_AcHS.AnimationSiteSet( this, bHookEnabled );
	if( ! m_AcHS.IsHookedWindow( hWnd ) )
		return false;
	return true;
}

void CExtAnimationSite::AnimationSite_Done()
{
	ASSERT( this != NULL );
	m_AcHS.AnimationSiteSet( NULL, false );
}

void CExtAnimationSite::AnimationSite_OnClientStateChanged(
	CExtAnimationClient * pAC,
	INT eAPT // __EAPT_*** animation type
	)
{
	ASSERT( this != NULL );
	ASSERT( pAC != NULL );
	AnimationSite_ClientProgressStart( pAC, eAPT );
}

void CExtAnimationSite::AnimationSite_PassAnimationEvents()
{
	ASSERT( this != NULL );
	if( ! AnimationSite_EnabledGet() )
		return;
HWND hWnd = AnimationSite_GetSafeHWND();
	// Pass timers first for animation issues
bool bTimersFound = false;
MSG msg;
	for( ; ::PeekMessage( &msg, hWnd, WM_TIMER, WM_TIMER, PM_NOREMOVE );  )
	{
		if( ! ::GetMessage( &msg, hWnd, WM_TIMER, WM_TIMER ) )
			break;
		::DispatchMessage( &msg );
		bTimersFound = true;
	}
	for( ; ::PeekMessage( &msg, hWnd, __EXT_MFC_WM_MOUSEFIRST, __EXT_MFC_WM_MOUSELAST, PM_NOREMOVE );  )
	{
		if( ! ::GetMessage( &msg, hWnd, __EXT_MFC_WM_MOUSEFIRST, __EXT_MFC_WM_MOUSELAST ) )
			break;
		::DispatchMessage( &msg );
		bTimersFound = true;
	}
	if( bTimersFound )
	{
		CExtPaintManager::stat_PassPaintMessages();
//		if( hWnd != NULL )
//			UpdateWindow( hWnd );
	}
}

void CExtAnimationSite::AnimationSite_OnProgressShutdownTimer( UINT nTimerID )
{
	ASSERT( this != NULL );
	ASSERT( nTimerID != __EXT_MFC_DEF_ANIMATION_TIMER_INVALID_ID );
	nTimerID;
}

/////////////////////////////////////////////////////////////////////////////
// CExtAnimationSingleton

CExtAnimationSingleton::CExtAnimationSingleton(
	bool bAsEnabled, // = true
	INT nAcStepNumber, //  = 0
	INT nAcStepCount, //  = __EXT_MFC_DEF_ANIMATION_STEP_COUNT
	UINT nAcTimerElapse, //  = __EXT_MFC_DEF_ANIMATION_TIMER_ELAPSE
	const RECT * pAcRcAnimationTarget //  = NULL
	)
	: CExtAnimationSite(
		bAsEnabled
		)
	, CExtAnimationClient(
		NULL,
		bAsEnabled,
		false,
		nAcStepNumber,
		nAcStepCount,
		nAcTimerElapse,
		pAcRcAnimationTarget
		)
{
	AnimationClient_SiteSet( this );
	AnimationSite_ClientAdd( this );
}

CExtAnimationSingleton::~CExtAnimationSingleton()
{
CExtAnimationSite * pAcAS = AnimationClient_SiteGet();
	if( pAcAS != NULL )
		pAcAS->AnimationSite_ClientRemove( this );
}

/////////////////////////////////////////////////////////////////////////////

#define __TIMER_ID_DRAGGING_START			4100
#define __TIMER_ID_DRELAYED_REPAINT_FAKE	4101
//#define __TIMER_ID_DELAYED_UPDATE			4102

#define __DOCKSITE_DCX_FLAGS \
	( DCX_WINDOW \
		|DCX_CACHE \
		|DCX_LOCKWINDOWUPDATE \
		|DCX_CLIPSIBLINGS \
	)

#define __BAR_NC_GAP_X 1 //afxData.cxBorder2
#define __BAR_NC_GAP_Y 1 //afxData.cyBorder2

#define __DOCKING_SIDE_GAP 10

static HCURSOR g_hCursorArrow	= ::LoadCursor( NULL, IDC_ARROW );
static HCURSOR g_hCursorDrag	= ::LoadCursor( NULL, IDC_SIZEALL );
static HCURSOR g_hCursorResizeV	= ::LoadCursor( NULL, IDC_SIZENS );
static HCURSOR g_hCursorResizeH	= ::LoadCursor( NULL, IDC_SIZEWE );

/////////////////////////////////////////////////////////////////////////////
// CExtControlBar::FriendlyDockBarHack

class CExtControlBar::FriendlyDockBarHack
	: CDockBar
{
public:
	friend class CExtControlBar;
	friend class CExtControlBar::InternalDraggingState_t;

	INT GetCalcExtentMax( BOOL bHorz )
	{
		ASSERT( this != NULL );
		ASSERT_VALID( this );
		ASSERT( m_pDockSite != NULL );
		ASSERT_VALID( m_pDockSite );
		ASSERT_KINDOF( CDockBar, this );
		INT nCalcExtentMax = bHorz
			? m_rectLayout.Width()
			: m_rectLayout.Height();
		ASSERT( nCalcExtentMax >= 0 );
		if(		nCalcExtentMax == 0
			||	m_rectLayout.IsRectEmpty()
			)
		{
			CRect rcDock;
			GetClientRect( &rcDock );
			nCalcExtentMax = bHorz
				? rcDock.Width()  // + 2
				: rcDock.Height() // - 2
				;
			if( nCalcExtentMax <= 0 )
				return 3; // dummy choice
		} // if( nCalcExtentMax == 0 )
		return nCalcExtentMax;
	}

	void _InsertBar( INT nPos, CControlBar * pBar )
	{
		ASSERT_VALID( this );
		ASSERT( nPos >= 0 && nPos < m_arrBars.GetSize() );
		m_arrBars.InsertAt( nPos, pBar );
		ASSERT_VALID( this );
#ifdef _DEBUG
		if( pBar != NULL )
		{
			ASSERT_BAR_LOCATION_IN_ROW( this, pBar );
		}
#endif // _DEBUG
	}
	
	void _InsertRowSplitter( INT nPos )
	{
		ASSERT_VALID( this );
		_InsertBar( nPos, NULL );
		ASSERT_VALID( this );
	}

}; // class CExtControlBar::FriendlyDockBarHack

/////////////////////////////////////////////////////////////////////////////
// CExtControlBar::POPUP_MENU_EVENT_DATA

CExtControlBar::POPUP_MENU_EVENT_DATA::POPUP_MENU_EVENT_DATA(
	SHORT nHelperNotificationType,
	CExtPopupMenuWnd * pPopupMenuWnd,
	CWnd * pWndEventSrc,
	const POINT & ptScreen,
	UINT nMsgID, // = WM_NULL
	CObject * pObjectSrc // = NULL
	)
	: m_nHelperNotificationType( nHelperNotificationType )
	, m_pPopupMenuWnd( pPopupMenuWnd )
	, m_pWndEventSrc( pWndEventSrc )
	, m_ptScreen( ptScreen )
	, m_nMsgID( nMsgID )
	, m_pObjectSrc( pObjectSrc )
	, m_pWndInitialTarget( NULL )
	, m_bPostNotification( false )
{
	ASSERT_VALID( m_pPopupMenuWnd );
	ASSERT_VALID( m_pWndEventSrc );
	ASSERT( m_pWndEventSrc->GetSafeHwnd() != NULL );
	if( m_ptScreen.x < 0 || m_ptScreen.y < 0 )
	{
		if( ! ::GetCursorPos( &m_ptScreen ) )
			m_ptScreen.x = m_ptScreen.y = 0;
	}
}

CExtControlBar::POPUP_MENU_EVENT_DATA::~POPUP_MENU_EVENT_DATA()
{
}

bool CExtControlBar::POPUP_MENU_EVENT_DATA::IsControlBarMenuListNotification() const
{
	switch( m_nHelperNotificationType )
	{
	case __PMED_DOCKBAR_CTX:
	case __PMED_CONTROLBAR_CTX:
	case __PMED_CONTROLBAR_NC_CTX:
	case __PMED_STATUSBAR_CTX:
	case __PMED_AUTOHIDESLIDER_CTX:
	case __PMED_MINIFRAME_NC_CTX:
	case __PMED_MDICLIAREA_CTX:
	case __PMED_CONTROLBAR_NCBTNMENU_BARS:
	case __PMED_CTXEXPBTN_BARS:
		return true;
	} // switch( m_nHelperNotificationType )
	return false;
}

CExtControlBar::POPUP_MENU_EVENT_DATA::operator WPARAM() const
{
	return ( (WPARAM) this );
}

CExtControlBar::POPUP_MENU_EVENT_DATA *
	CExtControlBar::POPUP_MENU_EVENT_DATA::FromWParam( WPARAM wParam )
{
	return ( (POPUP_MENU_EVENT_DATA *) wParam );
}

CExtControlBar::POPUP_MENU_EVENT_DATA *
	CExtControlBar::POPUP_MENU_EVENT_DATA::FromMSG( MSG * pMSG )
{
	ASSERT( pMSG != NULL );
	ASSERT( pMSG->message == CExtControlBar::g_nMsgConstructPopupMenu );
	return FromWParam( pMSG->wParam );
}

bool CExtControlBar::POPUP_MENU_EVENT_DATA::NotifyTarget(
	bool bPostNotification
	)
{
	ASSERT_VALID( m_pPopupMenuWnd );
	ASSERT_VALID( m_pWndEventSrc );
	ASSERT( m_pWndEventSrc->GetSafeHwnd() != NULL );
	m_bPostNotification = bPostNotification;
CControlBar * pBar = DYNAMIC_DOWNCAST( CControlBar, m_pWndEventSrc );
	if( pBar != NULL )
		m_pWndInitialTarget = pBar->m_pDockSite;
	if( m_pWndInitialTarget == NULL )
	{
		m_pWndInitialTarget =  m_pWndEventSrc->GetParentFrame();
		if( m_pWndInitialTarget == NULL )
		{
			m_pWndInitialTarget = m_pWndEventSrc->GetParent();
			ASSERT_VALID( m_pWndInitialTarget );
		}
		else if( m_pWndInitialTarget->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)) )
		{
			m_pWndInitialTarget = m_pWndInitialTarget->GetParentFrame();
			ASSERT_VALID( m_pWndInitialTarget );
			ASSERT( ! m_pWndInitialTarget->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)) );
		}
	}
	ASSERT_VALID( m_pWndInitialTarget );
	ASSERT( m_pWndInitialTarget->GetSafeHwnd() != NULL );
LRESULT lResult =
		m_pWndInitialTarget->SendMessage(
			CExtControlBar::g_nMsgConstructPopupMenu,
			*this
			);
	return (lResult != 0) ? true : false;
}


/////////////////////////////////////////////////////////////////////////////
// CExtControlBar

IMPLEMENT_DYNCREATE( CExtControlBar, CControlBar );

void CExtControlBar::PmBridge_OnPaintManagerChanged(
	CExtPaintManager * pGlobalPM
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() != NULL )
	{
		CUSTOM_NC_AREA_QUERY_DATA _cnaqd( this );
		_cnaqd.Notify();
	} // if( GetSafeHwnd() != NULL )
	CExtAnimationSite::PmBridge_OnPaintManagerChanged(
		pGlobalPM
		);
}

//CCriticalSection CExtControlBar::g_csCB;
ExtControlBarVector_t CExtControlBar::g_AllBars;
CExtControlBar::InternalDockBarInfo_t CExtControlBar::g_DockBarInfo;
bool CExtControlBar::g_bEnableOnIdleCalls = false;
bool CExtControlBar::g_bUseAdvancedToolTips = true;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
bool CExtControlBar::g_bUseMouseClickOnlyInAutoHideTabs = false; // (+ v.2.82) 
bool CExtControlBar::g_bUseMouseClickOnOffStyleAutoHideTabs = false; // (+ v.2.83) 
bool CExtControlBar::g_bAllowAutohideTabsOleDropTarget = true; // (+ v.2.23)
bool CExtControlBar::g_bAllowDynamicTabsOleDropTarget = true; // (+ v.2.23)
bool CExtControlBar::g_bAllowKeyboardTabSwitch = true; // (+ v.2.24)
bool CExtControlBar::g_bAllowKeyboardAutohideSwitch = true; // (+ v.2.24)
bool CExtControlBar::g_bCloseOnlyOneBarInTabGroup = false; // (+ v.2.40)
bool CExtControlBar::g_bTabsAtTop = false; // (+ v.2.43)
DWORD CExtControlBar::g_dwTabContainerTabsStyle = __ETWS_EQUAL_WIDTHS; // | __ETWS_MULTI_ROW_COLUMN | __ETWS_FULL_WIDTH
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

CExtControlBar::eResizablePanelDockingType_t CExtControlBar::g_eResizablePanelDockingType
	= CExtControlBar::__RESIZABLE_DOCKING_TYPE_BY_THEME;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
CExtControlBar::eAutoHideTabsType_t CExtControlBar::g_eAutoHideTabsType
	= CExtControlBar::__AUTOHIDE_TABS_TYPE_BY_THEME;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

const UINT CExtControlBar::g_nMsgQueryFocusChangingEnabled =
	::RegisterWindowMessage(
		_T("CExtControlBar::g_nMsgQueryFocusChangingEnabled")
		);
const UINT CExtControlBar::g_nMsgQueryRepositionCalcEffect =
	::RegisterWindowMessage(
		_T("CExtControlBar::g_nMsgQueryRepositionCalcEffect")
		);
const UINT CExtControlBar::g_nMsgQueryIcon =
	::RegisterWindowMessage(
		_T("CExtControlBar::g_nMsgQueryIcon")
		);
const UINT CExtControlBar::g_nMsgPutToPopupMenu =
	::RegisterWindowMessage(
		_T("CExtControlBar::g_nMsgPutToPopupMenu")
		);
const UINT CExtControlBar::g_nMsgConstructPopupMenu =
	::RegisterWindowMessage(
		_T("CExtControlBar::g_nMsgConstructPopupMenu")
		);
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
int CExtControlBar::g_nTabDrarDropSelectionDelay = 500;
const UINT CExtControlBar::g_nMsgCreateTabbedBar =
	::RegisterWindowMessage(
		_T("CExtControlBar::g_nMsgCreateTabbedBar")
		);
	bool CExtControlBar::g_bDisableAutoHideAnimationExpanding = false;
	bool CExtControlBar::g_bDisableAutoHideAnimationCollapsing = false;
	INT CExtControlBar::g_nAutoHideAnimationStepCount = 5;
	INT CExtControlBar::g_nAutoHideAnimationStepTime = 10;
	const int CExtControlBar::g_nTabShapeMaxHeight = 18;
	const int CExtControlBar::g_nTabShapeBottomLeftAreaWidth = 8;
	const int CExtControlBar::g_nTabShapeBottomMiddleAreaWidth = 36;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
const UINT CExtControlBar::g_nMsgDrawTrackedArea =
	::RegisterWindowMessage(
		_T("CExtControlBar::g_nMsgDrawTrackedArea")
		);

CExtControlBar * CExtControlBar::g_pBarToggleFloatingState = NULL;

CExtControlBar * CExtControlBar::_GetBarToggleFloatingState()
{
	return g_pBarToggleFloatingState;
}

CExtControlBar::QueryFocusChangingEnabled_t::QueryFocusChangingEnabled_t(
	CWnd * pWndQueryOwner,
	HWND hWndNewFocusOwner
	)
	: m_bFocusChangingEnabled( true )
	, m_pWndQueryOwner( pWndQueryOwner )
	, m_hWndNewFocusOwner( hWndNewFocusOwner )
{
	ASSERT_VALID( m_pWndQueryOwner );
	ASSERT( m_pWndQueryOwner->GetSafeHwnd() != NULL );
	ASSERT( m_hWndNewFocusOwner != NULL && ::IsWindow( m_hWndNewFocusOwner ) );
}

bool CExtControlBar::stat_QueryFocusChangingEnabled(
	HWND hWndNotifyTarget,
	CWnd * pWndQueryOwner,
	HWND hWndNewFocusOwner
	)
{
	ASSERT( hWndNotifyTarget != NULL && ::IsWindow( hWndNotifyTarget ) );
	ASSERT_VALID( pWndQueryOwner );
	ASSERT( pWndQueryOwner->GetSafeHwnd() != NULL );
	ASSERT( hWndNewFocusOwner != NULL && ::IsWindow( hWndNewFocusOwner ) );
CExtControlBar * pBar = DYNAMIC_DOWNCAST( CExtControlBar, pWndQueryOwner );
	if(		pBar != NULL
		&&	pBar->IsFixedMode()
		)
		return false;
QueryFocusChangingEnabled_t _QFCE( pWndQueryOwner, hWndNewFocusOwner );
	::SendMessage( hWndNotifyTarget, g_nMsgQueryFocusChangingEnabled, _QFCE, 0 );
	return _QFCE.m_bFocusChangingEnabled;
}

bool CExtControlBar::stat_QueryFocusChangingEnabled(
	CExtControlBar * pWndQueryOwner,
	HWND hWndNewFocusOwner
	)
{
	ASSERT_VALID( pWndQueryOwner );
	ASSERT( pWndQueryOwner->GetSafeHwnd() != NULL );
	ASSERT( hWndNewFocusOwner != NULL && ::IsWindow( hWndNewFocusOwner ) );
HWND hWndNotifyTarget = pWndQueryOwner->m_pDockSite->GetSafeHwnd();
	if( hWndNotifyTarget == NULL )
		hWndNotifyTarget = ::GetParent( pWndQueryOwner->m_hWnd );
	return
		stat_QueryFocusChangingEnabled(
			hWndNotifyTarget,
			pWndQueryOwner,
			hWndNewFocusOwner
			);
}

bool CExtControlBar::stat_DragDetect_ImplStep(
	HWND hWnd,
	POINT pt,
	UINT nUpMessage // = WM_LBUTTONUP
	)
{
	if( hWnd == NULL || (! ::IsWindow( hWnd ) ) )
		return false;
CSize sizeDrag( ::GetSystemMetrics( SM_CXDRAG ), ::GetSystemMetrics( SM_CYDRAG ) );
CRect rc( pt.x - sizeDrag.cx, pt.y - sizeDrag.cy, pt.x + sizeDrag.cx, pt.y + sizeDrag.cy );
	::SetCapture( hWnd );
	for( MSG _msg; ::IsWindow( hWnd ) ; )
    {
        for(	;
				::IsWindow( hWnd )
			&&	(	::PeekMessage( &_msg, hWnd, WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE )
				||	::PeekMessage( &_msg, 0, WM_CANCELMODE, WM_CANCELMODE, PM_NOREMOVE )
				||	::PeekMessage( &_msg, 0, WM_ACTIVATEAPP, WM_ACTIVATEAPP, PM_NOREMOVE )
				)
				;
				)
        {
            if(		_msg.message == nUpMessage
				||	_msg.message == WM_CANCELMODE
				||	_msg.message == WM_ACTIVATEAPP
				)
            {
                ::ReleaseCapture();
                return false;
            }
			::PeekMessage( &_msg, hWnd, _msg.message, _msg.message, PM_REMOVE );
            if( _msg.message == WM_MOUSEMOVE )
            {
                CPoint ptMessage( LOWORD( _msg.lParam ), HIWORD( _msg.lParam ) );
                if( ! rc.PtInRect( ptMessage ) )
                {
                    ReleaseCapture();
                    return true;
                }
            }
       }
        ::WaitMessage();
    }
    return false;
}

bool CExtControlBar::stat_DoDragDetect(
	HWND hWnd,
	const POINT & ptWndClient,
	UINT nUpMessage // = WM_LBUTTONUP
	)
{
	if( hWnd == NULL || (! ::IsWindow( hWnd ) ) )
		return false;
CPoint ptScreen = ptWndClient;
	::ClientToScreen( hWnd, &ptScreen );
	for( ; stat_DragDetect_ImplStep( hWnd, ptScreen, nUpMessage ) ; )
	{
		POINT ptCursorPos = { -32767, -32767 };
		if( ! ::GetCursorPos( &ptCursorPos ) )
			return false;
		if(  ptScreen == ptCursorPos )
			continue; //only drag time elapsed but mouse is not moved
		return true;
	}
	return false;
}

static CExtSafeString productsection2regkeypath(
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	)
{
	return CExtCmdManager::GetSubSystemRegKeyPath(
		__PROF_UIS_REG_CONTROL_BAR,
		sProfileName,
		sSectionNameCompany,
		sSectionNameProduct
		);
}

CExtControlBar::InternalDockBarInfo_t::InternalDockBarInfo_t()
{
	m_mapDockBarInfo.SetAt(
		AFX_IDW_DOCKBAR_TOP,
		CBRS_TOP
		);
	m_mapDockBarInfo.SetAt(
		AFX_IDW_DOCKBAR_BOTTOM,
		CBRS_BOTTOM
		);
	m_mapDockBarInfo.SetAt(
		AFX_IDW_DOCKBAR_LEFT,
		CBRS_LEFT
		);
	m_mapDockBarInfo.SetAt(
		AFX_IDW_DOCKBAR_RIGHT,
		CBRS_RIGHT
		);
}

DWORD CExtControlBar::InternalDockBarInfo_t::operator [] ( UINT nDockBarID )
{
	DWORD dwStyle = 0;
	VERIFY( m_mapDockBarInfo.Lookup(nDockBarID,dwStyle) );
	return dwStyle;
}

bool CExtControlBar::InternalDockBarInfo_t::ReplaceFrameDockBar(
	UINT nDockBarID,
	DWORD dwDockStyle,
	CFrameWnd * pFrame
	)
{
	ASSERT_VALID(pFrame);

	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);

DWORD dwDockBarStyle = (*this)[nDockBarID];
	if( (dwDockBarStyle&dwDockStyle&CBRS_ALIGN_ANY) == 0 )
		return false;
	
CDockBar * pDockBar = (CDockBar*)
		pFrame->GetControlBar(nDockBarID);
	ASSERT_VALID( pDockBar );
	ASSERT_KINDOF( CDockBar, pDockBar );
	if( pDockBar->IsKindOf(
			RUNTIME_CLASS(CExtDockOuterBar)
			)
		)
		return true;

BOOL bExplicitDelete = !pDockBar->m_bAutoDelete;
	pDockBar->m_pDockSite->RemoveControlBar( pDockBar );
	pDockBar->m_pDockSite = NULL;
	pDockBar->DestroyWindow();
	if( bExplicitDelete )
		delete pDockBar;
CExtDockBar * pDockBarEx = new CExtDockOuterBar();
DWORD dwStyle =
		WS_CHILD|WS_VISIBLE
			|WS_CLIPSIBLINGS|WS_CLIPCHILDREN
			|dwDockBarStyle
		;
	if(	! pDockBarEx->Create(
			pFrame,
			dwStyle,
			nDockBarID
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}

	return true;
}

CExtControlBar::CExtControlBar()
	: m_bGripperStaticallyAtTop( true )
	//, m_bAllowContractExpand( false ) // (- v.2.24)
	, m_bDoNotEraseClientBackground( false )
	, m_bPresubclassDialogMode( false )
	, m_bReflectParentSizing( true )
	, m_bInCustomModeUpdateCmdUI( false )
	, m_bFixedMode( false )
	, m_bNoForcedRecalcMetrics( false )
	, m_bAppearInDockSiteControlBarPopupMenu( true )
	, m_nGripHeightAtTop( ::GetSystemMetrics(SM_CYSMCAPTION)+1 )
	, m_nGripWidthAtLeft( ::GetSystemMetrics(SM_CYSMCAPTION)+1 )
	, m_nSeparatorHeight( min( 4, ::GetSystemMetrics(SM_CYSIZEFRAME) ) )
	, m_nSeparatorWidth(  min( 4, ::GetSystemMetrics(SM_CXSIZEFRAME) ) )
	, m_nTrackerOffset( 0 )
	, m_nMetricOffset( 0 )
	, m_sizeDockedH( 200, 150 )
	, m_sizeDockedV( 150, 200 )
	, m_sizeFloated( 200, 200 )
	, m_bRowResizing( false )
	, m_bRowRecalcing( false )
	, m_bDragging( false )
	, m_bReposSingleChildMode( true )
	, m_ptHelperDragOffsetFloated( 0, 0 )
	, m_ptHelperDragOffsetDockedH( 0, 0 )
	, m_ptHelperDragOffsetDockedV( 0, 0 )
	, m_ptFloatHelper( 0, 0 )
	, m_bWindowActive( false )
	, m_bTopRecalcing( false )
	, m_bNcAreaBtnTracking( false )
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	, m_bAutoHideDeactivateOnEsc( false )
	, m_bAutoHideMode( false )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	, m_ptDraggingLast( -1, -1 )
	, m_bUpdatingChain( false )
	, m_nDelayedRowUpdateMetric( 0 )
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	, m_bVisibleInCustomizeListBox( false )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	, m_nTearOffCmdID( 0 )
	, m_bSuspendTips( false )
	, m_bDelelayRepaintNcButtons( false )
	, m_bHelperSuppressDestruction( false )
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	, m_nAutoHideWidth( -1 )
	, m_nAutoHideHeight( -1 )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	, m_bDisplaying( false )
	, m_bVisCheckFlag( false )
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	, m_pNotifiedCustomizeSite( NULL )
#endif
	, m_bUseInvisibleCmdUI( false )
	, m_eCBPC( CExtControlBar::__ECBPC_EMPTY )
	, m_nFlashCaptionCounter( 0 )
	, m_nFlashCaptionStepMilliseconds( 0 )
	, m_nFlashCaptionTimerID( 99 )
	, m_clrFlashCaptionText( COLORREF(-1L) )
	, m_clrFlashCaptionBackground( COLORREF(-1L) )
	, m_bFlashCaptionHighlightedState( false )
	, m_bDelayShowInvoked( false )
	, m_bDelayShowFlag( false )
{
	VERIFY( RegisterControlBarClass() );
	m_nMinHW = ::GetSystemMetrics( SM_CYSMCAPTION ) + 1 + m_nSeparatorWidth;
	m_nMinVH = ::GetSystemMetrics( SM_CYSMCAPTION ) + 1 + m_nSeparatorHeight;

	m_rcGrip.SetRectEmpty();
	m_rcRowResize.SetRectEmpty();
	m_rcRowRecalc.SetRectEmpty();
	m_rcRowRecalcUp.SetRectEmpty();

//CSingleLock _slCsCB( &g_csCB );
//	_slCsCB.Lock();
	g_AllBars.Add( this );
//	_slCsCB.Unlock();

}

CExtControlBar::~CExtControlBar()
{
	AnimationSite_ClientRemove( 0, -1, false );

//CSingleLock _slCsCB( &g_csCB );
//	_slCsCB.Lock();
INT nCountOfExtBars = (INT)g_AllBars.GetSize();
	for( INT nExtBar = 0; nExtBar < nCountOfExtBars; nExtBar++ )
	{
		CExtControlBar * pBar = g_AllBars[ nExtBar ];
		ASSERT( pBar != NULL );
		if( pBar == this )
		{
			g_AllBars.RemoveAt( nExtBar );
			break;
		}
	} // for( INT nExtBar = 0; nExtBar < nCountOfExtBars; nExtBar++ )
//	_slCsCB.Unlock();

	NcButtons_RemoveAll();
}

BEGIN_MESSAGE_MAP(CExtControlBar, CControlBar)
	//{{AFX_MSG_MAP(CExtControlBar)
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_CANCELMODE()
	ON_WM_CAPTURECHANGED()
	ON_WM_MOUSEMOVE()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCLBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_WM_MBUTTONDOWN()
    ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_NCRBUTTONUP()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCMBUTTONDOWN()
	ON_WM_NCRBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
    ON_MESSAGE( WM_SETTEXT, OnSetText )
	ON_MESSAGE( WM_SIZEPARENT, OnSizeParent )
	ON_MESSAGE( WM_HELPHITTEST, OnHelpHitTest )
	ON_WM_SYSCOLORCHANGE()
	__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
	ON_MESSAGE( WM_DISPLAYCHANGE, OnDisplayChange )
	ON_MESSAGE( __ExtMfc_WM_THEMECHANGED, OnThemeChanged )
	__EXT_MFC_ON_WM_NCHITTEST()
	ON_MESSAGE_VOID( WM_INITIALUPDATE, OnInitialUpdate )
	ON_MESSAGE( WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI )
END_MESSAGE_MAP()


BOOL CExtControlBar::Create(
	__EXT_MFC_SAFE_LPCTSTR lpszWindowName,
	CWnd * pParentWnd,
    UINT nID, // = AFX_IDW_DIALOGBAR
	DWORD dwStyle // =
		//WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS
		//|CBRS_TOP|CBRS_GRIPPER|CBRS_TOOLTIPS
		//|CBRS_FLYBY|CBRS_SIZE_DYNAMIC
		//|CBRS_HIDE_INPLACE
	)
{
    ASSERT_VALID( pParentWnd );
 
	if( !RegisterControlBarClass() )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	
	ASSERT(
		!((dwStyle & CBRS_SIZE_FIXED)
		&&
        (dwStyle & CBRS_SIZE_DYNAMIC))
		);
    m_dwStyle = dwStyle & CBRS_ALL;

CExtSafeString sWindowName( (LPCTSTR(lpszWindowName) != NULL) ? LPCTSTR(lpszWindowName) : _T("") );
	if( lpszWindowName == NULL && nID != 0 && nID != UINT(-1) )
	{
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd( pParentWnd->GetSafeHwnd() ),
				nID
				);
		if( pCmdItem != NULL )
		{
			sWindowName = pCmdItem->m_sMenuText;
			sWindowName.Replace( _T("&"), _T("") );
		}
		else
		{
			if( ! g_ResourceManager->LoadString( sWindowName, nID ) )
				sWindowName = _T("");
		}
	}
	if( ! sWindowName.IsEmpty() )
	{
		int nNlPos = sWindowName.Find( _T('\n') );
		if( nNlPos >= 0 )
			sWindowName = sWindowName.Left( nNlPos );
	}
    
	dwStyle &= ~CBRS_ALL;
    dwStyle |= WS_CLIPCHILDREN;
CExtSafeString sWndClassName(
		::AfxRegisterWndClass(
			CS_DBLCLKS,
			g_hCursorArrow,
			PmBridge_GetPM()->GetBrush( COLOR_BTNFACE ),
			0
			)
		);

	if(	! CWnd::Create(
			sWndClassName,
			sWindowName.IsEmpty() ? _T("") : sWindowName,
			dwStyle,
			CRect( 0, 0, 0, 0 ),
			pParentWnd,
			nID
			)
		)
	{
		ASSERT( FALSE );
        return FALSE;
	}
	if( ( (CWnd::GetExStyle()&(WS_EX_LAYOUTRTL|WS_EX_RTLREADING)) != 0 ) )
	{
		CWnd::ModifyStyleEx(
			WS_EX_LAYOUTRTL|WS_EX_RTLREADING,
			0,
			SWP_FRAMECHANGED
			);
		ASSERT( (CWnd::GetExStyle()&(WS_EX_LAYOUTRTL|WS_EX_RTLREADING)) == 0 );
	} // if( ( (CWnd::GetExStyle()&(WS_EX_LAYOUTRTL|WS_EX_RTLREADING)) != 0 ) )
    return TRUE;
}

bool CExtControlBar::_SetupFloatingFrameWndImpl(
	CFrameWnd * pFrame
	)	
{
	ASSERT( pFrame != NULL );
	return
		((CExtControlBar::InternalFriendlyFrameWnd *)pFrame)->
			SetupSmilyFrameWnd();
}

bool CExtControlBar::_FrameEnableDockingImpl(
	CFrameWnd * pFrame,
	DWORD dwDockStyle, // = CBRS_ALIGN_ANY
	bool bReplaceFloatingWnd // = true
	) 
{
	// NOTE: this rule is specific for current implementation
	ASSERT( dwDockStyle == CBRS_ALIGN_ANY );

	ASSERT_VALID(pFrame);
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);

	pFrame->EnableDocking( dwDockStyle );
	
	if(	!g_DockBarInfo.ReplaceFrameDockBar(
			AFX_IDW_DOCKBAR_TOP,
			dwDockStyle,
			pFrame
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	if(	!g_DockBarInfo.ReplaceFrameDockBar(
			AFX_IDW_DOCKBAR_BOTTOM,
			dwDockStyle,
			pFrame
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	if(	!g_DockBarInfo.ReplaceFrameDockBar(
			AFX_IDW_DOCKBAR_LEFT,
			dwDockStyle,
			pFrame
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	if(	!g_DockBarInfo.ReplaceFrameDockBar(
			AFX_IDW_DOCKBAR_RIGHT,
			dwDockStyle,
			pFrame
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}

	if( bReplaceFloatingWnd )
	{
		if( !_SetupFloatingFrameWndImpl( pFrame ) )
		{
			ASSERT( FALSE );
			return false;
		}
	}

	return true;
}

const CRect & CExtControlBar::_RectGripGet() const
{
	ASSERT_VALID( this );
	return m_rcGrip;
}

void CExtControlBar::_RectGripSet( const RECT & rcGrip )
{
	ASSERT_VALID( this );
	m_rcGrip = rcGrip;
}

const CRect & CExtControlBar::_RectRowResizeGet() const
{
	ASSERT_VALID( this );
	return m_rcRowResize;
}

void CExtControlBar::_RectRowResizeSet( const RECT & rcRowResize )
{
	ASSERT_VALID( this );
	m_rcRowResize = rcRowResize;
}

const CRect & CExtControlBar::_RectRowRecalcGet() const
{
	ASSERT_VALID( this );
	return m_rcRowRecalc;
}

void CExtControlBar::_RectRowRecalcSet( const RECT & rcRowRecalc )
{
	ASSERT_VALID( this );
	m_rcRowRecalc = rcRowRecalc;
}

const CRect & CExtControlBar::_RectRowRecalcUpGet() const
{
	ASSERT_VALID( this );
	return m_rcRowRecalcUp;
}

void CExtControlBar::_RectRowRecalcUpSet( const RECT & rcRowRecalcUp )
{
	ASSERT_VALID( this );
	m_rcRowRecalcUp = rcRowRecalcUp;
}

INT CExtControlBar::_GripperWidthAtLeftGet() const
{
	ASSERT_VALID( this );
	ASSERT( m_nGripWidthAtLeft >= 0 );
	return m_nGripWidthAtLeft;
}

void CExtControlBar::_GripperWidthAtLeftSet( INT nGripWidthAtLeft )
{
	ASSERT_VALID( this );
	ASSERT( nGripWidthAtLeft >= 0 );
	m_nGripWidthAtLeft = nGripWidthAtLeft;
}

INT CExtControlBar::_GripperHeightAtTopGet() const
{
	ASSERT_VALID( this );
	ASSERT( m_nGripHeightAtTop >= 0 );
	return m_nGripHeightAtTop;
}

void CExtControlBar::_GripperHeightAtTopSet( INT nGripHeightAtTop )
{
	ASSERT_VALID( this );
	ASSERT( nGripHeightAtTop >= 0 );
	m_nGripHeightAtTop = nGripHeightAtTop;
}

INT CExtControlBar::_SeparatorWidthGet() const
{
	ASSERT_VALID( this );
	ASSERT( m_nSeparatorWidth >= 0 );
	return m_nSeparatorWidth;
}

void CExtControlBar::_SeparatorWidthSet( INT nSeparatorWidth )
{
	ASSERT_VALID( this );
	ASSERT( nSeparatorWidth >= 0 );
	m_nSeparatorWidth = nSeparatorWidth;
}

INT CExtControlBar::_SeparatorHeightGet() const
{
	ASSERT_VALID( this );
	return m_nSeparatorHeight;
}

void CExtControlBar::_SeparatorHeightSet( INT nSeparatorHeight )
{
	ASSERT_VALID( this );
	ASSERT( nSeparatorHeight >= 0 );
	m_nSeparatorHeight = nSeparatorHeight;
}

bool CExtControlBar::_IsCustomNcArea() const
{
	ASSERT_VALID( this );
	return g_bUseCustomNcArea;
}

bool CExtControlBar::g_bUseCustomNcArea = false;

const UINT CExtControlBar::g_nMsgCustomNcAreaQuery =
	::RegisterWindowMessage(
		_T("CExtControlBar::g_nMsgCustomNcAreaQuery")
		);

CExtControlBar::CUSTOM_NC_AREA_QUERY_DATA::CUSTOM_NC_AREA_QUERY_DATA(
	CExtControlBar * pBar,
	HDC hDcDraw // = NULL
	)
	: m_pBar( pBar )
	, m_hDcDraw( hDcDraw )
	, m_bQueryHandled( false )
{
	ASSERT_VALID( m_pBar );
}

bool CExtControlBar::CUSTOM_NC_AREA_QUERY_DATA::Notify()
{
	ASSERT_VALID( m_pBar );
	m_bQueryHandled = false;
	if( m_pBar->_IsCustomNcArea() )
	{
		CWnd * pWndNotifyTarget = m_pBar->m_pDockSite;
		if( pWndNotifyTarget->GetSafeHwnd() == NULL )
		{
			pWndNotifyTarget = m_pBar->GetOwner();
			if( pWndNotifyTarget->GetSafeHwnd() == NULL )
			{
				pWndNotifyTarget = m_pBar->GetParent();
			}
		}
		if( pWndNotifyTarget->GetSafeHwnd() != NULL )
			pWndNotifyTarget->SendMessage(
				CExtControlBar::g_nMsgCustomNcAreaQuery,
				*this
				);
	} // if( m_pBar->_IsCustomNcArea() )
	if( ! m_bQueryHandled )
	{
		INT nSeparatorWidth =
			m_pBar->PmBridge_GetPM()->Bar_SeparatorWidthGet( m_pBar );
		INT nSeparatorHeight =
			m_pBar->PmBridge_GetPM()->Bar_SeparatorHeightGet( m_pBar );
		if( nSeparatorWidth < 0 )
			nSeparatorWidth =  min( 4, ::GetSystemMetrics(SM_CXSIZEFRAME) );
		if( nSeparatorHeight < 0 )
			nSeparatorHeight = min( 4, ::GetSystemMetrics(SM_CYSIZEFRAME) );
		m_pBar->_SeparatorWidthSet( nSeparatorWidth );
		m_pBar->_SeparatorHeightSet( nSeparatorHeight );
		if(		(m_pBar->GetBarStyle()&CBRS_GRIPPER) != 0
			&&	(! m_pBar->IsFixedMode() )
			)
		{
			INT nGripWidthAtLeft =
				m_pBar->PmBridge_GetPM()->Bar_GripperWidthAtLeftGet( m_pBar );
			INT nGripHeightAtTop =
				m_pBar->PmBridge_GetPM()->Bar_GripperHeightAtTopGet( m_pBar );
			if( nGripWidthAtLeft < 0 )
				nGripWidthAtLeft = ::GetSystemMetrics(SM_CYSMCAPTION)+1;
			if( nGripHeightAtTop < 0 )
				nGripHeightAtTop = ::GetSystemMetrics(SM_CYSMCAPTION)+1;
			m_pBar->_GripperWidthAtLeftSet( nGripWidthAtLeft );
			m_pBar->_GripperHeightAtTopSet( nGripHeightAtTop );
		}
	} // if( ! m_bQueryHandled )
	return m_bQueryHandled;
}

CExtControlBar::CUSTOM_NC_AREA_QUERY_DATA::operator WPARAM() const
{
	return WPARAM( this );
}

CExtControlBar::CUSTOM_NC_AREA_QUERY_DATA *
	CExtControlBar::CUSTOM_NC_AREA_QUERY_DATA::FromWPARAM( WPARAM wParam )
{
CUSTOM_NC_AREA_QUERY_DATA * pCNAQD =
			reinterpret_cast < CUSTOM_NC_AREA_QUERY_DATA * > ( wParam );
	ASSERT( pCNAQD != NULL );
	return pCNAQD;
}

bool CExtControlBar::FrameEnableDocking(
	CFrameWnd * pFrame,
	DWORD dwDockStyle, // = CBRS_ALIGN_ANY
	bool bReplaceFloatingWnd // = true
	) 
{
	return
		_FrameEnableDockingImpl(
			pFrame,
			dwDockStyle,
			bReplaceFloatingWnd
			);
}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
bool CExtControlBar::FrameInjectAutoHideAreas(
	CFrameWnd * pFrame
	) 
{
	return _FrameInjectAutoHideAreasImpl( pFrame );
}

bool CExtControlBar::_FrameInjectAutoHideAreasImpl(
	CFrameWnd * pFrame
	) 
{
	ASSERT_VALID( pFrame );
	ASSERT( pFrame->GetSafeHwnd() != NULL );
CExtDynAutoHideSlider * pWndSlider = new CExtDynAutoHideSlider;
	if(	! pWndSlider->Create( pFrame ) )
	{
		ASSERT( FALSE );
		return false;
	}

CExtDynAutoHideArea * pWndAutoHideArea =
		new CExtDynAutoHideArea();
	if(	! pWndAutoHideArea->Create(
			pFrame,
			pWndSlider,
			AFX_IDW_DOCKBAR_TOP
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}

	pWndAutoHideArea =
		new CExtDynAutoHideArea();
	if(	! pWndAutoHideArea->Create(
			pFrame,
			pWndSlider,
			AFX_IDW_DOCKBAR_BOTTOM
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	
	pWndAutoHideArea =
		new CExtDynAutoHideArea();
	if(	! pWndAutoHideArea->Create(
			pFrame,
			pWndSlider,
			AFX_IDW_DOCKBAR_LEFT
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	
	pWndAutoHideArea =
		new CExtDynAutoHideArea();
	if(	! pWndAutoHideArea->Create(
			pFrame,
			pWndSlider,
			AFX_IDW_DOCKBAR_RIGHT
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}

	pFrame->DelayRecalcLayout();
	return true;
}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

bool CExtControlBar::IsFixedMode() const
{
	return m_bFixedMode;
}

bool CExtControlBar::IsFixedDockStyle() const
{
	return IsFixedMode();
}

bool CExtControlBar::ProfileBarStateLoad(
	CFrameWnd * pFrame,
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%
	LPWINDOWPLACEMENT pFrameWp, // = NULL // need serialize frame's WP
	bool bSerializeFixedBarsState, // = true,
	bool bSerializeResizableBarsState, // = true
	HKEY hKeyRoot, // HKEY_CURRENT_USER
	bool bEnableThrowExceptions // = false
	)
{
	ASSERT( sSectionNameCompany != NULL );
	ASSERT( sSectionNameProduct != NULL );
	ASSERT( sSectionNameProfile != NULL );
	ASSERT_VALID( pFrame );

__EXT_MFC_SAFE_LPCTSTR strProfileName =
		g_CmdManager->ProfileNameFromWnd(
			pFrame->GetSafeHwnd()
			);
	if(		pFrame == NULL
		||	strProfileName == NULL
		||	strProfileName[0] == _T('\0')
		)
	{
		//ASSERT( FALSE );
		TRACE0( "Prof-UIS: CExtControlBar::ProfileBarStateLoad() failed to load bar state\n" );
		if( bEnableThrowExceptions )
			::AfxThrowUserException();
		return false;
	}
	ASSERT_VALID( pFrame );
CExtSafeString sRegKeyPath=
		productsection2regkeypath(
			sSectionNameProfile,
			sSectionNameCompany,
			sSectionNameProduct
			);
	try
	{
		// prepare memory file and archive,
		// get information from registry
		CMemFile _file;
		if( ! CExtCmdManager::FileObjFromRegistry(
				_file,
				sRegKeyPath,
				hKeyRoot,
				bEnableThrowExceptions
				)
			)
		{
			ASSERT( ! bEnableThrowExceptions );
			TRACE0( "Prof-UIS: CExtControlBar::ProfileBarStateLoad() failed to load bar state\n" );
			// Windows XP fix - begin
			WINDOWPLACEMENT _wpf;
			::memset( &_wpf, 0, sizeof(WINDOWPLACEMENT) );
			_wpf.length = sizeof(WINDOWPLACEMENT);
			CWnd * pWndPlacement = stat_GetWndForPlacement( pFrame );
			ASSERT_VALID( pWndPlacement );
			//if( pWndPlacement->GetWindowPlacement(&_wpf) )
			if( CExtControlBar::stat_GetWindowPlacement( pWndPlacement->m_hWnd, _wpf ) )
			{
				_wpf.ptMinPosition.x = _wpf.ptMinPosition.y = 0;
				_wpf.ptMaxPosition.x = _wpf.ptMaxPosition.y = 0;
				_wpf.showCmd = 
					(pWndPlacement->GetStyle() & WS_VISIBLE)
						? SW_SHOWNA
						: SW_HIDE;
				//pWndPlacement->SetWindowPlacement(&_wpf);
				CExtControlBar::stat_SetWindowPlacement( pWndPlacement->m_hWnd, _wpf );
			}
			// Windows XP fix - end
			return false;
		}
		CArchive ar(
			&_file,
			CArchive::load
			);
		// do serialization
		if( ! ProfileBarStateSerialize(
				ar,
				pFrame,
				pFrameWp,
				bSerializeFixedBarsState,
				bSerializeResizableBarsState,
				bEnableThrowExceptions
				)
			)
		{
			//ASSERT( FALSE );
			TRACE0( "Prof-UIS: CExtControlBar::ProfileBarStateLoad() failed to load bar state\n" );
			return false;
		}
		return true;
	} // try
	catch( CException * pXept )
	{
		if( bEnableThrowExceptions )
			throw;
		pXept->Delete();
		//ASSERT( FALSE );
	} // catch( CException * pXept )
	catch( ... )
	{
		if( bEnableThrowExceptions )
			throw;
		//ASSERT( FALSE );
	} // catch( ... )
	TRACE0( "Prof-UIS: CExtControlBar::ProfileBarStateLoad() failed to load bar state\n" );
	return false;
}

bool CExtControlBar::ProfileBarStateSave(
	CFrameWnd * pFrame,
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%\%sSectionNameProfile%
	LPWINDOWPLACEMENT pFrameWp, // = NULL // need serialize frame's WP
	bool bSerializeFixedBarsState, // = true,
	bool bSerializeResizableBarsState, // = true
	HKEY hKeyRoot, // HKEY_CURRENT_USER
	bool bEnableThrowExceptions // = false
	)
{
	ASSERT( sSectionNameCompany != NULL );
	ASSERT( sSectionNameProduct != NULL );
	ASSERT( sSectionNameProfile != NULL );
	ASSERT_VALID( pFrame );

__EXT_MFC_SAFE_LPCTSTR strProfileName =
		g_CmdManager->ProfileNameFromWnd(
			pFrame->GetSafeHwnd()
			);
	if(		pFrame == NULL
		||	strProfileName == NULL
		||	strProfileName[0] == _T('\0')
		)
	{
		//ASSERT( FALSE );
		TRACE0( "Prof-UIS: CExtControlBar::ProfileBarStateSave() failed to save bar state\n" );
		if( bEnableThrowExceptions )
			::AfxThrowUserException();
		return false;
	}
	ASSERT_VALID( pFrame );
CExtSafeString sRegKeyPath =
		productsection2regkeypath(
			sSectionNameProfile,
			sSectionNameCompany,
			sSectionNameProduct
			);
	try
	{
		// prepare memory file and archive
		CMemFile _file;
		CArchive ar(
			&_file,
			CArchive::store
			);
		// do serialization
		ProfileBarStateSerialize(
			ar,
			pFrame,
			pFrameWp,
			bSerializeFixedBarsState,
			bSerializeResizableBarsState,
			bEnableThrowExceptions
			);
		// OK, serialization passed
		ar.Flush();
		ar.Close();
		// put information to registry
		if(	! CExtCmdManager::FileObjToRegistry(
				_file,
				sRegKeyPath,
				hKeyRoot,
				bEnableThrowExceptions
				)
			)
		{
			ASSERT( ! bEnableThrowExceptions );
			//ASSERT( FALSE );
			TRACE0( "Prof-UIS: CExtControlBar::ProfileBarStateSave() failed to save bar state\n" );
			return false;
		}
		return true;
	} // try
	catch( CException * pXept )
	{
		if( bEnableThrowExceptions )
			throw;
		pXept->Delete();
		//ASSERT( FALSE );
	} // catch( CException * pXept )
	catch( ... )
	{
		if( bEnableThrowExceptions )
			throw;
		//ASSERT( FALSE );
	} // catch( ... )
	TRACE0( "Prof-UIS: CExtControlBar::ProfileBarStateSave() failed to save bar state\n" );
	return false;
}

bool CExtControlBar::InternalFriendlyFrameWnd::SetupSmilyFrameWnd(void)
{
	m_pFloatingFrameClass =
		RUNTIME_CLASS(CExtMiniDockFrameWnd);

	return true;
}

//
//static CString __DEBUG_get_step_string( int nCharStep )
//{
//CString s = _T("");
//	for( int i = 0; i < nCharStep; i++ )
//		s += "    ";
//	return s;
//}
//static CString __DEBUG_get_bar_info_string(
//	LPCTSTR strPrefix,
//	CControlBarInfo * pInfo,
//	int nCharStep = 1
//	)
//{
//	ASSERT( pInfo != NULL );
//	ASSERT_VALID( pInfo->m_pBar );
//CString s, sWndText;
//	pInfo->m_pBar->GetWindowText( sWndText );
//	s.Format(
//		_T("%s%sID = %d =\"%s\" (%s)\n"),
//		strPrefix,
//		LPCTSTR( __DEBUG_get_step_string( nCharStep ) ),
//		int(pInfo->m_nBarID),
//		LPCTSTR(sWndText),
//		pInfo->m_pBar->GetRuntimeClass()->m_lpszClassName
//		);
//int nChildrenCount = int( pInfo->m_arrBarID.GetSize() );
//	if( nChildrenCount > 0 )
//	{
//		CString sTmp;
//		sTmp.Format(
//			_T("%s%s%d children:\n"),
//			strPrefix,
//			LPCTSTR( __DEBUG_get_step_string( nCharStep + 1 ) ),
//			nChildrenCount
//			);
//		s += sTmp;
//		for( int i = 0; i < nChildrenCount; i++ )
//		{
//			sTmp.Format(
//				_T("%s%sID = %d\n"),
//				strPrefix,
//				LPCTSTR( __DEBUG_get_step_string( nCharStep + 2 ) ),
//				int( pInfo->m_arrBarID[i] )
//				);
//			s += sTmp;
//		}
//	}
//	return s;
//}
//static void __DEBUG_trace_bar_info(
//	LPCTSTR strPrefix,
//	CControlBarInfo * pInfo,
//	int nCharStep = 1
//	)
//{
//	afxDump << __DEBUG_get_bar_info_string( strPrefix, pInfo, nCharStep );
//}
//

void CExtControlBar::InternalFriendlyFrameWnd::SetOuterDockState(
	const CDockState & state,
	const CExtControlBar::OuterPropsArr_t & arrOuterProps,
	bool bSerializeFixedBarsState,
	bool bSerializeResizableBarsState
	)
{
	// first pass through barinfo's sets the m_pBar member correctly
	// creating floating frames if necessary
//CMapPtrToWord _mapCFV;
int i = 0;
	for( i = 0; i < state.m_arrBarInfo.GetSize(); i++ )
	{
		CControlBarInfo * pInfo = (CControlBarInfo *)
			state.m_arrBarInfo[i];
		ASSERT( pInfo != NULL );
		CControlBar * pExistingBar = GetControlBar( pInfo->m_nBarID );
		if( pExistingBar != NULL )
		{

//
//CControlBar * pOld = pInfo->m_pBar;
//pInfo->m_pBar = pExistingBar;
//__DEBUG_trace_bar_info( _T("-LOAD-"), pInfo, 4 );
//pInfo->m_pBar = pOld;
//

			if(		(! pExistingBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
				||	( ((CExtControlBar*)pExistingBar)->IsFixedMode() )
				||	( ((CExtControlBar*)pExistingBar)->IsFixedDockStyle() )
				)
			{
				if( ! bSerializeFixedBarsState )
					continue;
			}
			else
			{
				if( ! bSerializeResizableBarsState )
					continue;
			}
		}
		if( pInfo->m_bFloating )
		{
			// need to create floating frame to match
			CMiniDockFrameWnd * pDockFrame =
				CreateFloatingFrame(
					pInfo->m_bHorz ?
						CBRS_ALIGN_TOP : CBRS_ALIGN_LEFT
					);
			ASSERT( pDockFrame != NULL) ;
			CRect rcWnd( pInfo->m_pointPos, CSize(10, 10) );
			pDockFrame->CalcWindowRect( &rcWnd );
			pDockFrame->SetWindowPos(
				NULL,
				rcWnd.left, rcWnd.top, 0, 0,
				SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE
				);
			CDockBar * pDockBar = (CDockBar *)
				pDockFrame->GetDlgItem( AFX_IDW_DOCKBAR_FLOAT );
			ASSERT( pDockBar != NULL );
			ASSERT_KINDOF( CDockBar, pDockBar );
			pInfo->m_pBar = pDockBar;
// 			if( pInfo->m_bVisible )
// 				_mapCFV.SetAt( pDockFrame, 0 );
		} // if( pInfo->m_bFloating )
		else // regular dock bar or toolbar
		{
			pInfo->m_pBar =
				pExistingBar; // GetControlBar( pInfo->m_nBarID );
			if( pInfo->m_pBar == NULL )
				continue;
		} // else from if( pInfo->m_bFloating )
		pInfo->m_pBar->m_nMRUWidth = pInfo->m_nMRUWidth;
	} // for( i = 0; i < state.m_arrBarInfo.GetSize(); i++ )

	// update new outer props
	for( i = 0; i < arrOuterProps.GetSize(); i++ )
	{
		OuterItemData_t _oid = arrOuterProps[i];
		CControlBar * pBarTmp = GetControlBar( _oid.m_nBarID );
		if( pBarTmp == NULL )
			continue;
		CExtControlBar * pExtBar = DYNAMIC_DOWNCAST( CExtControlBar, pBarTmp );
		if( pExtBar == NULL )
			continue;
		if(		( pExtBar->IsFixedMode() )
			||	( pExtBar->IsFixedDockStyle() )
			)
		{
			if( ! bSerializeFixedBarsState )
				continue;
		}
		else
		{
			if( ! bSerializeResizableBarsState )
				continue;
		}
		pExtBar->m_ptFloatHelper = _oid.m_ptFloatHelper;
		pExtBar->m_sizeDockedH = _oid.m_sizeDockedH;
		pExtBar->m_sizeDockedV = _oid.m_sizeDockedV;
		pExtBar->m_sizeFloated = _oid.m_sizeFloated;
		pExtBar->_AffixmentSetOuter( &_oid.m_AffixmentData );
	} // for( i = 0; i < arrOuterProps.GetSize(); i++ )

	// the second pass will actually dock all of the control bars and
	// set everything correctly
	for( i = 0; i < state.m_arrBarInfo.GetSize(); i++ )
	{
		CControlBarInfo * pInfo = (CControlBarInfo *)
			state.m_arrBarInfo[i];
		ASSERT( pInfo != NULL );
		if( pInfo->m_pBar == NULL )
			continue;
		if(		(! pInfo->m_pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
			||	( ((CExtControlBar*)pInfo->m_pBar)->IsFixedMode() )
			||	( ((CExtControlBar*)pInfo->m_pBar)->IsFixedDockStyle() )
			)
		{
			if( ! bSerializeFixedBarsState )
				continue;
		}
		else
		{
			if( ! bSerializeResizableBarsState )
				continue;
		}
		if(		pInfo->m_pBar->m_pDockContext != NULL
			&&	pInfo->m_pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
			)
			((CExtControlBar*)(pInfo->m_pBar))->m_ptFloatHelper
				= pInfo->m_ptMRUFloatPos;
		pInfo->m_pBar->SetBarInfo( pInfo, this );
	} // for( i = 0; i < state.m_arrBarInfo.GetSize(); i++ )

	// the third pass will move independent floating bars
	// into valid positions
	for( i = 0; i < arrOuterProps.GetSize(); i++ )
	{
		OuterItemData_t _oid = arrOuterProps[i];
		CControlBar * pBarTmp = GetControlBar( _oid.m_nBarID );
		if( pBarTmp == NULL )
			continue;
		CExtControlBar * pExtBar = DYNAMIC_DOWNCAST( CExtControlBar, pBarTmp );
		if( pExtBar == NULL )
			continue;
		if(		( pExtBar->IsFixedMode() )
			||	( pExtBar->IsFixedDockStyle() )
			)
		{
			if( ! bSerializeFixedBarsState )
				continue;
		}
		else
		{
			if( ! bSerializeResizableBarsState )
				continue;
		}
		if( ! _oid.m_bFloating )
			continue;
		CSize _sizeFloating = _oid.m_sizeFloated;
		CRect rcFrameInitial( _oid.m_ptFloatHelper, _sizeFloating /*_oid.m_sizeFloated*/ );
		CExtMiniDockFrameWnd * pMiniFrame =
			DYNAMIC_DOWNCAST(
				CExtMiniDockFrameWnd,
				pExtBar->GetParentFrame()
				);
// 		if( pMiniFrame != NULL )
// 		{
// 			CRect rcClientFrame, rcWndFrame;
// 			pMiniFrame->GetClientRect( &rcClientFrame );
// 			pMiniFrame->GetWindowRect( &rcWndFrame );
// 			_sizeFloating += rcWndFrame.Size() - rcClientFrame.Size();
// 			rcFrameInitial.right += rcWndFrame.Width() - rcClientFrame.Width();
// 			rcFrameInitial.bottom += rcWndFrame.Height() - rcClientFrame.Height();
// 		}
		CRect rcFrameNew =
			CExtPaintManager::stat_AlignWndRectToMonitor(
				rcFrameInitial
				);
// 		CExtMiniDockFrameWnd * pMiniFrame =
// 			DYNAMIC_DOWNCAST(
// 				CExtMiniDockFrameWnd,
// 				pExtBar->GetParentFrame()
// 				);
		if( pMiniFrame != NULL )
		{
			ASSERT_VALID( pMiniFrame );
			CRect rcFrameReal;
			pMiniFrame->GetWindowRect( rcFrameReal );
			
			//if( rcFrameReal == rcFrameNew )
			if( rcFrameReal.TopLeft() == rcFrameNew.TopLeft() )
			{
				pExtBar->DelayShow( _oid.m_bVisible ? TRUE : FALSE );
				pMiniFrame->RecalcLayout();
				continue;
			}
			pExtBar->OnNcAreaButtonsReinitialize();
			if( ! pExtBar->IsFixedMode() )
				pMiniFrame->ModifyStyle( FWS_SNAPTOBARS, 0 );
			//((InternalFriendlyFrameWnd *)pMiniFrame)->m_bInRecalcLayout = TRUE;
			pMiniFrame->MoveWindow( &rcFrameNew, FALSE );
			//((InternalFriendlyFrameWnd *)pMiniFrame)->m_bInRecalcLayout = FALSE;
			pMiniFrame->DelayRecalcLayout();
			//WORD tmp;
			//if(		_mapCFV.Lookup( pMiniFrame, tmp )
			//	&&	( pMiniFrame->GetStyle()&WS_VISIBLE ) == 0
			//	&&	( ! pExtBar->IsFixedMode() )
			//	)
			if( _oid.m_bVisible )
			{
				pExtBar->DelayShow( TRUE );
				CSize _size = rcFrameNew.Size();
				pExtBar->SetInitDesiredSizeFloating( _size );
				pExtBar->m_pDockContext->m_sizeLast = _size;

				pMiniFrame->ShowWindow( SW_SHOWNOACTIVATE );
// 				((InternalFriendlyFrameWnd *)pMiniFrame)->m_bInRecalcLayout = TRUE;
 				pMiniFrame->MoveWindow( &rcFrameNew, FALSE );
// 				((InternalFriendlyFrameWnd *)pMiniFrame)->m_bInRecalcLayout = FALSE;
				pMiniFrame->RecalcLayout();
				CRect rcClientFrame, rcWndFrame;
 				pMiniFrame->GetClientRect( &rcClientFrame );
 				pMiniFrame->GetWindowRect( &rcWndFrame );
				CRect rcClientBar, rcWndBar;
				pExtBar->GetClientRect( &rcClientBar );
				pExtBar->GetWindowRect( &rcWndBar );
				CSize _sizeBar =
					_sizeFloating // _oid.m_sizeFloated
						- rcClientFrame.Size()
						+ rcWndFrame.Size()
						- rcClientBar.Size()
						+ rcWndBar.Size()
						;
				pExtBar->OnRepositionSingleChild(
					_sizeBar.cx,
					_sizeBar.cy,
					true
					);
			}
		} // if( pMiniFrame != NULL )
		else
			pExtBar->FloatControlBar( rcFrameNew.TopLeft() );
	} // for( i = 0; i < arrOuterProps.GetSize(); i++ )

	// last pass shows all the floating windows that were previously shown
	for( i = 0; i < state.m_arrBarInfo.GetSize(); i++ )
	{
		CControlBarInfo * pInfo = (CControlBarInfo *)
			state.m_arrBarInfo[i];
		ASSERT( pInfo != NULL );
		if( pInfo->m_pBar == NULL )
			continue;
		if(		(! pInfo->m_pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
			||	( ((CExtControlBar*)pInfo->m_pBar)->IsFixedMode() )
			||	( ((CExtControlBar*)pInfo->m_pBar)->IsFixedDockStyle() )
			)
		{
			if( ! bSerializeFixedBarsState )
				continue;
		}
		else
		{
			if( ! bSerializeResizableBarsState )
				continue;
		}
		if( ! pInfo->m_bFloating )
			continue;
		CFrameWnd * pFrameWnd =
			pInfo->m_pBar->GetParentFrame();
		CDockBar * pDockBar = (CDockBar *) pInfo->m_pBar;
		ASSERT_KINDOF( CDockBar, pDockBar );
		if( pDockBar->GetDockedVisibleCount() > 0 )
		{
			pFrameWnd->RecalcLayout();
			pFrameWnd->ShowWindow( SW_SHOWNA );
			for( INT nBar = 1; nBar < pDockBar->m_arrBars.GetSize(); nBar ++ )
			{
				ASSERT( pDockBar->m_arrBars[0] == NULL );
				CControlBar * pBar = (CControlBar *)
					pDockBar->m_arrBars[nBar];
				if( pBar == NULL )
					continue;
				if( __PLACEHODLER_BAR_PTR(pBar) )
					continue;
				CExtControlBar * pExtBar =
					DYNAMIC_DOWNCAST(
						CExtControlBar,
						pBar
						);
				if( pExtBar == NULL
					|| pExtBar->IsFixedMode()
					)
					continue;
				pExtBar->_RecalcNcArea();
			}
		} // if( pDockBar->GetDockedVisibleCount() > 0 )
	} // for( i = 0; i < state.m_arrBarInfo.GetSize(); i++ )
}

void CExtControlBar::InternalFriendlyFrameWnd::GetOuterDockState(
	CDockState & state,
	CExtControlBar::OuterPropsArr_t & arrOuterProps
	) const
{
	for(	POSITION pos = m_listControlBars.GetHeadPosition();
			pos != NULL;
		)
	{ // get state info for each bar
		CControlBar * pBar = (CControlBar *)
			m_listControlBars.GetNext( pos );
		ASSERT( pBar != NULL );

		if(		pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar))
			||	pBar->IsKindOf(RUNTIME_CLASS(CExtDockDynBar))
			)
			continue;

		if(		pBar->m_pDockBar != NULL
			&&	pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynBar))
			)
			continue;

		if( pBar->IsDockBar() )
		{
			UINT nID = pBar->GetDlgCtrlID();
			ASSERT_DOCKBAR_DLGCTRLID( nID );
			if( nID == AFX_IDW_DOCKBAR_FLOAT )
			{
				CWnd * pWnd = pBar->GetWindow(GW_CHILD);
				if( pWnd->GetSafeHwnd() == NULL )
					continue;
				CControlBar * pBarChild =
					STATIC_DOWNCAST(
						CControlBar,
						pWnd
						);
				if( pBarChild->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
					continue;
			}
			if(		pBar->IsKindOf(RUNTIME_CLASS(CExtDockBar))
				&&	((CExtDockBar *)pBar)->_GetCircleNo() != 0
				)
				continue;
		} // if( pBar->IsDockBar() )
#ifdef _DEBUG
		else
		{
			ASSERT( !pBar->IsKindOf(RUNTIME_CLASS(CDockBar)) );
		} // else from if( pBar->IsDockBar() )
#endif // _DEBUG

		CSize sizeReset( -1, -1 );
		if( pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
		{
			if( pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
				continue;

			if(		(! ((CExtControlBar*)pBar)->IsFixedMode() )
				&&	(! pBar->IsFloating() )
				)
			{
				if( pBar->m_pDockBar == NULL )
					continue;
				CExtDockBar * pDockBar =
					STATIC_DOWNCAST(
						CExtDockBar,
						pBar->m_pDockBar
						);
				ASSERT( !pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynBar)) );
				if( pDockBar->_GetCircleNo() > 0 )
					continue;
			}
			OuterItemData_t _oid;
			_oid.m_nBarID = pBar->GetDlgCtrlID();
			_oid.m_ptFloatHelper = ((CExtControlBar*)pBar)->m_ptFloatHelper;
			_oid.m_sizeDockedH = ((CExtControlBar*)pBar)->m_sizeDockedH;
			_oid.m_sizeDockedV = ((CExtControlBar*)pBar)->m_sizeDockedV;
			_oid.m_sizeFloated = ((CExtControlBar*)pBar)->m_sizeFloated;
			ASSERT( _oid.m_AffixmentData.IsEmpty() );
			InternalAffixmentData * pAffixmentData =
				((CExtControlBar*)pBar)->_AffixmentGetOuter();
			if( pAffixmentData != NULL )
				_oid.m_AffixmentData = *pAffixmentData;

			_oid.m_bVisible = pBar->IsVisible() ? true : false;
	
			ASSERT( !_oid.m_bFloating );
			if( pBar->IsFloating() )
			{
				CRect rcBarWnd;
				CRect rcFraweWnd;
				pBar->GetWindowRect( &rcBarWnd );
				CExtMiniDockFrameWnd * pMiniFrame =
					STATIC_DOWNCAST(
						CExtMiniDockFrameWnd,
						pBar->GetParentFrame()
						);
				pMiniFrame->GetWindowRect( &rcFraweWnd );
				CSize sizeBarWnd = rcBarWnd.Size();
				CSize sizeFraweWnd = rcFraweWnd.Size();
				CSize sizeDiff = sizeFraweWnd - sizeBarWnd;
				_oid.m_ptFloatHelper = rcFraweWnd.TopLeft();
				if( ((CExtControlBar*)pBar)->IsFixedMode() )
				{
					_oid.m_sizeFloated += sizeDiff;
					sizeReset = _oid.m_sizeFloated;
				} // if( ((CExtControlBar*)pBar)->IsFixedMode() )
				_oid.m_bFloating = true;
			} // if( pBar->IsFloating() )

			arrOuterProps.Add( _oid );
		} // if( pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )

		CControlBarInfo* pInfo = new CControlBarInfo;
		pBar->GetBarInfo( pInfo );
		if( pInfo->m_pBar == NULL )
			continue;

//
//__DEBUG_trace_bar_info( _T("-SAVE-"), pInfo, 4 );
//

		if( sizeReset.cx > 0 && sizeReset.cy > 0 )
			pInfo->m_nMRUWidth = sizeReset.cx;

		state.m_arrBarInfo.Add(pInfo);
	} // get state info for each bar
}

void CExtControlBar::InternalFriendlyFrameWnd::SetDockState(
	const CExtControlBar::InternalDockStateSite & state,
	bool bPresetWP
	)
{
	state.StateSet( bPresetWP );
}

void CExtControlBar::InternalFriendlyFrameWnd::GetDockState(
	CExtControlBar::InternalDockStateSite & state
	) const
{
	state.StateGet();
}

CExtControlBar::InternalDockStateSite::InternalDockStateSite(
	CFrameWnd * pDockSite,
	bool bSerializeFixedBarsState, // = true,
	bool bSerializeResizableBarsState // = true
	)
	: m_pDockSite( pDockSite )
	, m_pOuterState( NULL )
	, m_bSerializeFixedBarsState( bSerializeFixedBarsState )
	, m_bSerializeResizableBarsState( bSerializeResizableBarsState )
{
	ASSERT_VALID( m_pDockSite );
	ASSERT_KINDOF( CFrameWnd, m_pDockSite );
	ASSERT( !m_pDockSite->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) );
	::memset( &m_wp, 0, sizeof(WINDOWPLACEMENT) );
	m_pOuterState = new CDockState;
}

CExtControlBar::InternalDockStateSite::~InternalDockStateSite()
{
	Clear();
	if( m_pOuterState != NULL )
		delete m_pOuterState;
}

void CExtControlBar::InternalDockStateSite::Clear()
{
	ASSERT_VALID( this );
	ASSERT( m_pOuterState != NULL );
	::memset( &m_wp, 0, sizeof(WINDOWPLACEMENT) );
	m_wp.length = sizeof(WINDOWPLACEMENT);

	m_pOuterState->Clear();

INT nCount = (INT)m_arrBarStates.GetSize();
	for( INT i = 0; i < nCount; i++ )
	{
		InternalDockStateBar * pState =
			m_arrBarStates[i];
		ASSERT_VALID( pState );
		pState->Clear();
		delete pState;
	}
	m_arrBarStates.RemoveAll();

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	m_arrAutoHideOrder.RemoveAll();
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	m_arrOuterProps.RemoveAll();
}

void CExtControlBar::InternalDockStateSite::Serialize( CArchive & ar )
{
	ASSERT_VALID( this );

//bool bSerializeFixedBarsState = m_bSerializeFixedBarsState;
bool bSerializeResizableBarsState = m_bSerializeResizableBarsState;
INT i, nCount;
DWORD dwApiVer0 = g_CmdManager.GetVersionDWORD( false );
DWORD dwApiVer1 = g_CmdManager.GetVersionDWORD( true );
DWORD dwReserved = 0;

	if( ar.IsStoring() )
	{

		// serialize version info
		CExtSafeString sTmpBuffer;
		CExtSafeString sFriendlyVer;
		sFriendlyVer.Format(
			_T("Prof-UIS (v. %s) control bar state"),
			g_CmdManager.GetVersionString( sTmpBuffer )
			);
		ar << sFriendlyVer;
		ar << dwApiVer0;
		ar << dwApiVer1;
		ar << dwReserved;
		ar << dwReserved;
		ar << dwReserved;
		ar << dwReserved;

		// serialize frame position
		CExtSafeString sFrameWp;
		sFrameWp.Format(
			_T("%d,%d,%d,%d,%d,%d"),
			m_wp.rcNormalPosition.left,
			m_wp.rcNormalPosition.top,
			m_wp.rcNormalPosition.right,
			m_wp.rcNormalPosition.bottom,
			m_wp.showCmd,
			m_wp.flags
			);
		ar << sFrameWp;

		//if( bSerializeFixedBarsState )
		{ // BLOCK BEGIN (+2.52): replacement of CDockState::Serialize() method
			//
			// this block fixes bug in MFC 8.0 which causes 32<-->64 bit
			// version incompatibility of the CControlBarInfo::Serialize() method
			// invoked from the CDockState::Serialize() method
			//
			// the CDockState::Serialize() methods body also expanded into
			// this code block
			//
			// the InternalFriendlyDockState class defined locally is needed
			// for accessing the CDockState::m_dwVersion protected property
			//

			////// m_pOuterState->Serialize( ar );

			class InternalFriendlyDockState : public CDockState
			{
			public:
				friend class CExtControlBar::InternalDockStateSite;
			};
			InternalFriendlyDockState * pDockState =
				(InternalFriendlyDockState *)m_pOuterState;
			ar << pDockState->m_dwVersion;
			if( pDockState->m_dwVersion > 1 )
				ar << pDockState->GetScreenSize();
			WORD wBarInfoIndex, wBarInfoCount = WORD(pDockState->m_arrBarInfo.GetSize());
			ar << wBarInfoCount;
			for( wBarInfoIndex = 0; wBarInfoIndex < wBarInfoCount; wBarInfoIndex++ )
			{

				////// ((CControlBarInfo*)m_arrBarInfo[i])->Serialize(ar, this);

				CControlBarInfo * pCBI =
					(CControlBarInfo*)pDockState->m_arrBarInfo[wBarInfoIndex];
				ar << (DWORD)pCBI->m_nBarID;
				ar << (DWORD)pCBI->m_bVisible;
				ar << (DWORD)pCBI->m_bFloating;
				ar << (DWORD)pCBI->m_bHorz;
				ar << pCBI->m_pointPos;
				if( pDockState->GetVersion() > 1 )
				{
					ar << (DWORD)pCBI->m_nMRUWidth;
					ar << (DWORD)pCBI->m_bDocking;
					if( pCBI->m_bDocking )
					{
						ar << (DWORD)pCBI->m_uMRUDockID;
						ar << pCBI->m_rectMRUDockPos;
						ar << pCBI->m_dwMRUFloatStyle;
						ar << pCBI->m_ptMRUFloatPos;
					} // if( pCBI->m_bDocking )
				} // if( pDockState->GetVersion() > 1 )
				WORD wIdIndex, wIdCount = WORD(pCBI->m_arrBarID.GetSize());
				ar << wIdCount;
				for( wIdIndex = 0; wIdIndex < wIdCount; wIdIndex++ )
				{
					DWORD dw = DWORD(__EXT_MFC_DWORD_PTR(pCBI->m_arrBarID[wIdIndex]));
					ar << dw;
				} // for( wIdIndex = 0; wIdIndex < wIdCount; wIdIndex++ )
			} // for( wBarInfoIndex = 0; wBarInfoIndex < wBarInfoCount; wBarInfoIndex++ )

			//
			// the final block is added to serialize the Prof-UIS specific
			// properties of the fixed sized control bars located in the
			// outer dock bar windows (CExtDockOuterBar)
			//

			nCount = (INT)m_arrOuterProps.GetSize();
			ar << DWORD(nCount);
			for( i = 0; i < nCount; i++ )
				m_arrOuterProps.ElementAt(i).Serialize( ar );
		} // BLOCK END (+2.52): replacement of CDockState::Serialize() method

		if( bSerializeResizableBarsState )
		{
			nCount = (INT)m_arrBarStates.GetSize();
			ar << DWORD(nCount);
			for( i = 0; i < nCount; i++ )
			{
				InternalDockStateBar * pState =
					m_arrBarStates[i];
				ASSERT_VALID( pState );
				pState->Serialize( ar );
			} // for( i = 0; i < nCount; i++ )
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			nCount = (INT)m_arrAutoHideOrder.GetSize();
			ar << DWORD(nCount);
			for( i = 0; i < nCount; i++ )
			{
				UINT nBarID = m_arrAutoHideOrder[ i ];
				ar << DWORD(nBarID);
			} // for( i = 0; i < nCount; i++ )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		} // if( bSerializeResizableBarsState )
		
	} // if( ar.IsStoring() )
	else
	{
		Clear();

		// serialize version info
		DWORD dwApiVer0a = 0, dwApiVer1a = 0, dwTmp;
		CExtSafeString sFriendlyVer;
		ar >> sFriendlyVer;
		ar >> dwApiVer0a;
		ar >> dwApiVer1a;
		ar >> dwReserved;
		ar >> dwReserved;
		ar >> dwReserved;
		ar >> dwReserved;
//		if( dwApiVer1 != dwApiVer1a )
		if( dwApiVer1 < dwApiVer1a )
		{
			ASSERT( FALSE );
#if _MFC_VER >= 0x0800
			::AfxThrowArchiveException( CArchiveException::genericException, NULL );
#else
			::AfxThrowArchiveException( CArchiveException::generic, NULL );
#endif
		}

		CExtSafeString sFrameWp;
		ar >> sFrameWp;
		ASSERT( !sFrameWp.IsEmpty() );
		::memset( &m_wp, 0, sizeof(WINDOWPLACEMENT) );
		m_wp.length = sizeof(WINDOWPLACEMENT);
		CWnd * pWndPlacement = stat_GetWndForPlacement( m_pDockSite );
		ASSERT_VALID( pWndPlacement );
		// VERIFY( pWndPlacement->GetWindowPlacement(&m_wp)  );
		VERIFY( CExtControlBar::stat_GetWindowPlacement( pWndPlacement->m_hWnd, m_wp ) );
		m_wp.ptMinPosition.x = m_wp.ptMinPosition.y = 0;
		m_wp.ptMaxPosition.x = m_wp.ptMaxPosition.y = 0;

		if(	__EXT_MFC_STSCANF(
				sFrameWp,
				_T("%d,%d,%d,%d,%d,%d"),
				&m_wp.rcNormalPosition.left,
				&m_wp.rcNormalPosition.top,
				&m_wp.rcNormalPosition.right,
				&m_wp.rcNormalPosition.bottom,
				&m_wp.showCmd,
				&m_wp.flags
				) != 6
			)
		{
			ASSERT( FALSE );
			::memset( &m_wp, 0, sizeof(WINDOWPLACEMENT) );
			m_wp.length = sizeof(WINDOWPLACEMENT);
			m_wp.showCmd = SW_HIDE;
			//return false;
			#if _MFC_VER >= 0x0800
				::AfxThrowArchiveException( CArchiveException::genericException, NULL );
			#else
				::AfxThrowArchiveException( CArchiveException::generic, NULL );
			#endif
		}

		//if( bSerializeFixedBarsState )
		{ // BLOCK BEGIN (+2.52): replacement of CDockState::Serialize() method
			//
			// this block fixes bug in MFC 8.0 which causes 32<-->64 bit
			// version incompatibility of the CControlBarInfo::Serialize() method
			// invoked from the CDockState::Serialize() method
			//
			// the CDockState::Serialize() methods body also expanded into
			// this code block
			//
			// the InternalFriendlyDockState class defined locally is needed
			// for accessing the CDockState::m_dwVersion protected property
			//

			////// m_pOuterState->Serialize( ar );

			class InternalFriendlyDockState : public CDockState
			{
			public:
				friend class CExtControlBar::InternalDockStateSite;
			};
			InternalFriendlyDockState * pDockState =
				(InternalFriendlyDockState *)m_pOuterState;
			pDockState->Clear();
			ar >> pDockState->m_dwVersion;
			ASSERT( pDockState->m_dwVersion == 1 || pDockState->m_dwVersion == 2 );
			if( pDockState->m_dwVersion > 1 )
			{
				CSize size;
				ar >> size;
				pDockState->SetScreenSize(size);
			} // if( pDockState->m_dwVersion > 1 )
			WORD nOldSize;
			ar >> nOldSize;
			pDockState->m_arrBarInfo.SetSize( nOldSize );
			WORD wBarInfoIndex, wBarInfoCount = WORD(pDockState->m_arrBarInfo.GetSize());
			for( wBarInfoIndex = 0; wBarInfoIndex < wBarInfoCount; wBarInfoIndex++ )
			{

				////// m_arrBarInfo[i] = new CControlBarInfo;
				////// ((CControlBarInfo*)m_arrBarInfo[i])->Serialize(ar, this);

				CControlBarInfo * pCBI = new CControlBarInfo;
				pDockState->m_arrBarInfo.SetAt( wBarInfoIndex, pCBI );
				DWORD dw;
				ar >> dw;
				pCBI->m_nBarID = (int)dw;
				ar >> dw;
				pCBI->m_bVisible = (BOOL)dw;
				ar >> dw;
				pCBI->m_bFloating = (BOOL)dw;
				ar >> dw;
				pCBI->m_bHorz = (BOOL)dw;
				ar >> pCBI->m_pointPos;
				if( pDockState->GetVersion() > 1 )
				{
					pDockState->ScalePoint( pCBI->m_pointPos );
					ar >> dw;
					pCBI->m_nMRUWidth = (int)dw;
					ar >> dw;
					pCBI->m_bDocking = (BOOL)dw;
					if( pCBI->m_bDocking )
					{
						ar >> dw;
						pCBI->m_uMRUDockID = (DWORD)dw;
						ar >> pCBI->m_rectMRUDockPos;
						pDockState->ScaleRectPos( pCBI->m_rectMRUDockPos );
						ar >> pCBI->m_dwMRUFloatStyle;
						ar >> pCBI->m_ptMRUFloatPos;
						pDockState->ScalePoint( pCBI->m_ptMRUFloatPos );
					} // if( pCBI->m_bDocking )
				} // if( pDockState->GetVersion() > 1 )
				WORD wIdIndex, wIdCount;
				ar >> wIdCount;
				pCBI->m_arrBarID.SetSize( wIdCount );
				for( wIdIndex = 0; wIdIndex < wIdCount; wIdIndex++ )
				{
					DWORD dw;
					ar >> dw;
#if (_MFC_VER >= 0x900)
					pCBI->m_arrBarID.SetAt( wIdIndex, (UINT)dw );
#else
					pCBI->m_arrBarID.SetAt( wIdIndex, (LPVOID)(__EXT_MFC_DWORD_PTR)dw );
#endif
				} // for( wIdIndex = 0; wIdIndex < wIdCount; wIdIndex++ )
			} // for( wBarInfoIndex = 0; wBarInfoIndex < wBarInfoCount; wBarInfoIndex++ )
			pDockState->m_dwVersion = 2;

			//
			// the final block is added to serialize the Prof-UIS specific
			// properties of the fixed sized control bars located in the
			// outer dock bar windows (CExtDockOuterBar)
			//

			ar >> dwTmp;
			nCount = INT(dwTmp);
			for( i = 0; i < nCount; i++ )
			{
				OuterItemData_t _oid;
				_oid.Serialize( ar );
				m_arrOuterProps.Add( _oid );
			} // for( i = 0; i < nCount; i++ )
		} // BLOCK END (+2.52): replacement of CDockState::Serialize() method

		if( bSerializeResizableBarsState )
		{
			ar >> dwTmp;
			nCount = INT(dwTmp);
			for( i = 0; i < nCount; i++ )
			{
				InternalDockStateBar * pState =
					new InternalDockStateBar;
				pState->Serialize( ar );
				ASSERT_VALID( pState );
				m_arrBarStates.Add( pState );
			} // for( i = 0; i < nCount; i++ )
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			ar >> dwTmp;
			nCount = INT(dwTmp);
			for( i = 0; i < nCount; i++ )
			{
				UINT nBarID;
				ar >> dwTmp;
				nBarID = UINT(dwTmp);
				m_arrAutoHideOrder.Add( nBarID );
			} // for( i = 0; i < nCount; i++ )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		} // if( bSerializeResizableBarsState )

	} // else from if( ar.IsStoring() )
}

void CExtControlBar::InternalDockStateSite::StateGet()
{
	ASSERT_VALID( this );
	ASSERT( m_pOuterState != NULL );
	ASSERT_VALID( m_pDockSite );

	Clear();

	CExtDockBar::_OptimizeCircles( m_pDockSite );

CWnd * pWndPlacement = stat_GetWndForPlacement( m_pDockSite );
	ASSERT_VALID( pWndPlacement );
	::memset( &m_wp, 0, sizeof(WINDOWPLACEMENT) );
	m_wp.length = sizeof(WINDOWPLACEMENT);
//	VERIFY( pWndPlacement->GetWindowPlacement( &m_wp ) );
	VERIFY( CExtControlBar::stat_GetWindowPlacement( pWndPlacement->m_hWnd, m_wp ) );

POSITION pos = m_pDockSite->m_listControlBars.GetHeadPosition();
	for( ; pos != NULL; )
	{
		CControlBar * pBar = (CControlBar *)
			m_pDockSite->m_listControlBars.GetNext( pos );
		ASSERT_VALID( pBar );
		
		if( pBar->IsFloating() )
		{
			if( pBar->IsDockBar() )
				continue;
			if( pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
			{
				CExtMiniDockFrameWnd * pFrame =
					STATIC_DOWNCAST(
						CExtMiniDockFrameWnd,
						pBar->GetParentFrame()
						);
				ASSERT_VALID( pBar->m_pDockBar );
				ASSERT_KINDOF( CDockBar, pBar->m_pDockBar );
				ASSERT( !pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar)) );
				ASSERT( pBar->m_pDockBar->GetDlgCtrlID() == AFX_IDW_DOCKBAR_FLOAT );
				ASSERT( pBar->m_pDockBar->GetParent() == pFrame );
				AppendFloatingLayout(
					STATIC_DOWNCAST(
						CExtMiniDockFrameWnd,
						pFrame
						)
					);
			} // if( pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
			continue;
		} // if( pBar->IsFloating() )
		
		if( !pBar->IsDockBar() )
			continue;

		ASSERT_KINDOF( CDockBar, pBar );
		if( !pBar->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
			continue;

		CFrameWnd * pFrame = pBar->GetParentFrame();
		ASSERT_VALID( pFrame );
		if( pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
		{
			ASSERT_KINDOF( CExtMiniDockFrameWnd, pFrame );
			continue;
		}

		ASSERT(
			!pBar->GetParentFrame()->IsKindOf(
				RUNTIME_CLASS( CMiniFrameWnd )
				)
			);
		
		CExtDockBar * pDockBar =
			STATIC_DOWNCAST(
				CExtDockBar,
				pBar
				);

		UINT nCircleNo = pDockBar->_GetCircleNo();
		if( nCircleNo == 0 )
			continue;

		AppendDockBarAtSideLayout( pDockBar );
	}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
CExtDynAutoHideArea * arrAutoHiders[4] =
{
	NULL, NULL, NULL, NULL
};
static const UINT arrSideMap[4] =
{
	AFX_IDW_DOCKBAR_TOP,
	AFX_IDW_DOCKBAR_BOTTOM,
	AFX_IDW_DOCKBAR_LEFT,
	AFX_IDW_DOCKBAR_RIGHT,
};
	for( INT nSide = 0; nSide < 4; nSide++ )
	{
		UINT nDockBarID = arrSideMap[ nSide ];
		CExtDockBar * pDockBar = (CExtDockBar *)
			m_pDockSite->GetControlBar( nDockBarID );
		if( pDockBar == NULL )
			continue;
		ASSERT_VALID( pDockBar );
		ASSERT_KINDOF( CExtDockBar, pDockBar );
		ASSERT( pDockBar->_GetCircleNo() == 0 );
		arrAutoHiders[ nSide ] = pDockBar->_GetAutoHideArea();
		if( arrAutoHiders[ nSide ] == NULL )
		{
			ASSERT( nSide == 0 );
			break;
		}
		ASSERT_VALID( arrAutoHiders[ nSide ] );
		ASSERT(
			UINT( arrAutoHiders[ nSide ]->GetDlgCtrlID() )
				== arrSideMap[nSide]
				);
	} // for( INT nSide = 0; nSide < 4; nSide++ )

	if( arrAutoHiders[0] != NULL )
	{
		for( INT nSide = 0; nSide < 4; nSide++ )
		{
			ASSERT_VALID( arrAutoHiders[nSide] );
			INT nCount = arrAutoHiders[nSide]->ItemGetCount();
			for( INT nItem = 0; nItem < nCount; nItem++ )
			{
				CExtTabWnd::TAB_ITEM_INFO * pTII =
					arrAutoHiders[nSide]->ItemGet( nItem );
				ASSERT_VALID( pTII );
				if( (pTII->GetItemStyle() & __ETWI_IN_GROUP_ACTIVE) == 0 )
					continue;
				CExtControlBar * pBar =
					reinterpret_cast < CExtControlBar * >
						( pTII->LParamGet() );
				ASSERT_VALID( pBar );
				ASSERT_KINDOF( CExtControlBar, pBar );
				ASSERT( !pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) );
				ASSERT( !pBar->IsFixedMode() );
				ASSERT( m_pDockSite->m_listControlBars.Find(pBar) != NULL );
				ASSERT( pBar->AutoHideModeGet() );
				UINT nBarID = (UINT) pBar->GetDlgCtrlID();
				m_arrAutoHideOrder.Add( nBarID );
			} // for( INT nItem = 0; nItem < nCount; nItem++ )
		} // for( nSide = 0; nSide < 4; nSide++ )
	} // if( arrAutoHiders[0] != NULL )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	((InternalFriendlyFrameWnd *)m_pDockSite)->GetOuterDockState(
		*m_pOuterState,
		m_arrOuterProps
		);
}

CExtControlBar::InternalAffixmentData::InternalAffixmentData()
	: m_nAffixmentWeight( 0 )
	, m_rcAffixment( 0, 0, 0, 0 )
	, m_sizeCompressed( 0, 0 )
	, m_dwAffixmentFlags( __EAFF_FIXED_MODE_INITIAL_RECALC )
{
}

CExtControlBar::InternalAffixmentData::InternalAffixmentData(
	const CExtControlBar::InternalAffixmentData & other
	)
{
	_AssignFromOther( other );
}

CExtControlBar::InternalAffixmentData::~InternalAffixmentData()
{
}

void CExtControlBar::InternalAffixmentData::_AssignFromOther(
	const CExtControlBar::InternalAffixmentData & other
	)
{
	m_nAffixmentWeight = other.m_nAffixmentWeight;
	m_rcAffixment = other.m_rcAffixment;
	m_sizeCompressed = other.m_sizeCompressed;
	m_dwAffixmentFlags = other.m_dwAffixmentFlags;
}

bool CExtControlBar::InternalAffixmentData::IsEmpty() const
{
	return (m_nAffixmentWeight==0) ? true : false;
}

void CExtControlBar::InternalAffixmentData::Empty()
{
	m_nAffixmentWeight = 0;
	m_rcAffixment.SetRectEmpty();
	ASSERT(
			m_rcAffixment.left == 0
		&&	m_rcAffixment.right == 0
		&&	m_rcAffixment.top == 0
		&&	m_rcAffixment.bottom == 0
		);
	m_sizeCompressed.cx = m_sizeCompressed.cy = 0;
	m_dwAffixmentFlags = __EAFF_FIXED_MODE_INITIAL_RECALC;
}

int CExtControlBar::InternalAffixmentData::CompareAffixment(
	const CExtControlBar::InternalAffixmentData & other
	) const
{
	if( !IsEmpty() )
	{
		if( other.IsEmpty() )
			return 1;
		if( m_nAffixmentWeight > other.m_nAffixmentWeight )
			return 1;
		if( m_nAffixmentWeight < other.m_nAffixmentWeight )
			return -1;
	} // if( !IsEmpty() )
	else
	{
		if( !other.IsEmpty() )
			return -1;
	} // else from if( !IsEmpty() )
	return 0;
}

void CExtControlBar::InternalAffixmentData::Serialize( CArchive & ar )
{
DWORD dwHelperFlags = 0;
	if( ar.IsStoring() )
	{
		if( IsEmpty() )
			dwHelperFlags |= 0x01;
		ar << dwHelperFlags;
		if( dwHelperFlags & 0x01 )
			return;
		ar << DWORD(m_nAffixmentWeight);
		ar << m_rcAffixment;
		ar << m_sizeCompressed;
		ar << m_dwAffixmentFlags;
	} // if( ar.IsStoring() )
	else
	{
		Empty();
		ar >> dwHelperFlags;
		if( dwHelperFlags & 0x01 )
			return;
		DWORD dwTmp;
		ar >> dwTmp;
		m_nAffixmentWeight = UINT(dwTmp);
		ar >> m_rcAffixment;
		ar >> m_sizeCompressed;
		ar >> m_dwAffixmentFlags;
	} // else from if( ar.IsStoring() )
}

CExtControlBar::InternalDockStateBar *
	CExtControlBar::InternalDockStateSite::AppendFloatingLayout(
		CExtMiniDockFrameWnd * pMiniFrame
		)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pMiniFrame );
	ASSERT_KINDOF( CExtMiniDockFrameWnd, pMiniFrame );

CDockBar * pFloatDocker =
		STATIC_DOWNCAST(
			CDockBar,
			pMiniFrame->GetWindow( GW_CHILD )
			);
	ASSERT_VALID( pFloatDocker );
	ASSERT( pFloatDocker->IsDockBar() );
	ASSERT_KINDOF( CDockBar, pFloatDocker );
	ASSERT( !pFloatDocker->IsKindOf(RUNTIME_CLASS(CExtDockBar)) );
	ASSERT( pFloatDocker->GetDlgCtrlID() == AFX_IDW_DOCKBAR_FLOAT );
CExtDynControlBar * pDynDocker =
		STATIC_DOWNCAST(
			CExtDynControlBar,
			pFloatDocker->GetWindow( GW_CHILD )
			);
	ASSERT_VALID( pDynDocker );

InternalDockStateBar * _idsb =
		new InternalDockStateBar;
	_idsb->StateGet( pDynDocker, NULL );
	ASSERT( _idsb->m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING );
	m_arrBarStates.Add( _idsb );

	return _idsb;
}

CExtControlBar::InternalDockStateBar *
	CExtControlBar::InternalDockStateSite::AppendDockBarAtSideLayout(
		CExtDockBar * pDockBar
		)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pDockBar );
	ASSERT_KINDOF( CExtDockBar, pDockBar );
	ASSERT( !pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynBar)) );
	ASSERT( pDockBar->_GetCircleNo() > 0 );

	ASSERT( pDockBar->m_arrBars[0] == NULL );
bool bNewRow = false;
INT nCount = (INT)pDockBar->m_arrBars.GetSize();
InternalDockStateBar * pLastState = NULL;
	for( INT nBar = 1; nBar < nCount; nBar ++ )
	{
		CExtControlBar * pBar = (CExtControlBar *)
			pDockBar->m_arrBars[nBar];
		if( pBar == NULL )
		{
			bNewRow = true;
			continue;
		}
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		ASSERT_KINDOF( CExtControlBar, pBar );
		ASSERT( !pBar->IsFixedMode() );

		InternalDockStateBar * _idsb =
			new InternalDockStateBar;
		_idsb->StateGet( pBar, NULL );
		if( bNewRow )
			_idsb->m_dwDockStateFlags |= InternalDockStateBar::__STATE_NEW_ROW;
		m_arrBarStates.Add( _idsb );
		bNewRow = false;
		pLastState = _idsb;
	}
	return pLastState;
}

void CExtControlBar::InternalDockStateSite::StateSet( bool bPresetWP ) const
{
	ASSERT_VALID( this );
	ASSERT( m_pOuterState != NULL );
	ASSERT_VALID( m_pDockSite );

	if( bPresetWP )
	{
		CRect rcDockSiteWnd;
		if(		m_wp.showCmd == SW_SHOWMAXIMIZED
			||	m_wp.showCmd == SW_SHOWMINIMIZED
			||	m_wp.showCmd == SW_SHOWMINNOACTIVE
			||	m_wp.showCmd == SW_HIDE
			||	m_wp.showCmd == SW_FORCEMINIMIZE
			)
		{
			CExtPaintManager::monitor_parms_t _mp;
			CExtPaintManager::stat_GetMonitorParms( _mp, m_wp.rcNormalPosition );
			rcDockSiteWnd = _mp.m_rcWorkArea;
		}
		else
		{
			rcDockSiteWnd =  m_wp.rcNormalPosition;
		}
		CWnd * pWndPlacement = CExtControlBar::stat_GetWndForPlacement( m_pDockSite );
		ASSERT_VALID( pWndPlacement );
		if( pWndPlacement != m_pDockSite )
		{
			m_pDockSite->SetWindowPos(
				NULL,
				0, 0,
				rcDockSiteWnd.Width(), rcDockSiteWnd.Height(),
				SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE
					|SWP_NOREDRAW ////|SWP_NOSENDCHANGING 
					|SWP_NOMOVE
				);
			pWndPlacement->SetWindowPos(
				NULL,
				rcDockSiteWnd.left, rcDockSiteWnd.top,
				rcDockSiteWnd.Width(), rcDockSiteWnd.Height(),
				SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE
					//|SWP_NOREDRAW
					////|SWP_NOSENDCHANGING
				);
		}
		else
			pWndPlacement->SetWindowPos(
				NULL,
				rcDockSiteWnd.left, rcDockSiteWnd.top,
				rcDockSiteWnd.Width(), rcDockSiteWnd.Height(),
				SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE
					|SWP_NOREDRAW ////|SWP_NOSENDCHANGING 
				);
	} // if( bPresetWP )

POSITION pos;
INT i, nCount;

ExtControlBarVector_t vRedockBars;
	for(	pos = m_pDockSite->m_listControlBars.GetHeadPosition();
			pos != NULL;
			)
	{
		CControlBar * pBar = (CControlBar *)
			m_pDockSite->m_listControlBars.GetNext( pos );
		ASSERT_VALID( pBar );

		if(		(! pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
			||	( ((CExtControlBar*)pBar)->IsFixedMode() )
			||	( ((CExtControlBar*)pBar)->IsFixedDockStyle() )
			)
		{
			if( ! m_bSerializeFixedBarsState )
				continue;
		}
		else
		{
			if( ! m_bSerializeResizableBarsState )
				continue;
		}
		
		if( pBar->GetParentFrame()->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
		{
			if(		pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
				&&	(! pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
				)
				vRedockBars.Add( (CExtControlBar *)pBar );
			continue;
		}

		if( pBar->IsDockBar() )
			continue;
		if(		pBar->m_pDockSite == NULL
			||	pBar->m_pDockContext == NULL
			)
			continue;
		if( pBar->m_pDockBar == NULL )
		{
			m_pDockSite->DockControlBar( pBar );
			m_pDockSite->ShowControlBar( pBar, FALSE, TRUE );
			continue;
		}
		if(		(! pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
			||	pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar))
			||	((CExtControlBar *)pBar)->IsFixedMode()
			)
			continue;

		ASSERT_KINDOF( CExtDockBar, pBar->m_pDockBar );
		if(		((CExtDockBar*)pBar->m_pDockBar)->_GetCircleNo() == 0
			&&	(! pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynBar)) )
			)
			continue;
		
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if(		m_bSerializeResizableBarsState
			&&	((CExtControlBar *)pBar)->AutoHideModeGet()
			)
			((CExtControlBar *)pBar)->AutoHideModeSet(
				false, false, false, true );
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

		m_pDockSite->ShowControlBar( pBar, FALSE, TRUE );
	}

	nCount = (INT)vRedockBars.GetSize();
	for( i = 0; i < nCount; i++ )
	{
		CExtControlBar * pBar = vRedockBars[i];
		DWORD dwBarStyle = pBar->GetBarStyle();
		UINT nBarDockerID = AFX_IDW_DOCKBAR_TOP;
		if( dwBarStyle & CBRS_ALIGN_BOTTOM )
			nBarDockerID = AFX_IDW_DOCKBAR_BOTTOM;
		else if( dwBarStyle & CBRS_ALIGN_LEFT )
			nBarDockerID = AFX_IDW_DOCKBAR_LEFT;
		else if( dwBarStyle & CBRS_ALIGN_RIGHT )
			nBarDockerID = AFX_IDW_DOCKBAR_RIGHT;
		if( pBar->IsFixedMode() )
		{
			bool bFloating = pBar->IsFloating();
			if( bFloating )
				m_pDockSite->ShowControlBar( pBar, FALSE, FALSE );
			m_pDockSite->DockControlBar(
				pBar,
				nBarDockerID
				);
			m_pDockSite->ShowControlBar( pBar, FALSE, TRUE );
		} // if( pBar->IsFixedMode() )
		else
		{
			((CExtControlBar *)pBar)->DockControlBar(
				nBarDockerID,
				1,
				m_pDockSite,
				false
				);
			m_pDockSite->ShowControlBar( pBar, FALSE, TRUE );
		} // else from if( pBar->IsFixedMode() )
	} // for( i = 0; i < nCount; i++ )

	CExtDockBar::_OptimizeCircles( m_pDockSite );
	m_pDockSite->RecalcLayout();

	if( m_bSerializeResizableBarsState )
	{
		ASSERT( ! ((InternalFriendlyFrameWnd*)m_pDockSite)->m_bInRecalcLayout );
		((InternalFriendlyFrameWnd*)m_pDockSite)->m_bInRecalcLayout = TRUE;
		nCount = (INT)m_arrBarStates.GetSize();
		for( i = 0; i < nCount; i++ )
		{
			InternalDockStateBar * pState =
				m_arrBarStates[i];
			ASSERT_VALID( pState );
			pState->StateSet( m_pDockSite, NULL );
		} // for( i = 0; i < nCount; i++ )
		((InternalFriendlyFrameWnd*)m_pDockSite)->m_bInRecalcLayout = FALSE;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		nCount = (INT)m_arrAutoHideOrder.GetSize();
		for( i = 0; i < nCount; i++ )
		{
			UINT nBarID = m_arrAutoHideOrder[ i ];
			CExtControlBar * pBar =
				DYNAMIC_DOWNCAST(
					CExtControlBar,
					m_pDockSite->GetControlBar( nBarID )
					);
//KJ TELESTE:
			if( pBar == NULL )
				return;

			ASSERT_VALID( pBar );
			ASSERT_KINDOF( CExtControlBar, pBar );
			ASSERT( ! pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) );
			ASSERT( ! pBar->IsFixedMode() );
			ASSERT( ! pBar->AutoHideModeGet() );
			ASSERT( ! pBar->IsFloating() );
			ASSERT( ! pBar->GetParentFrame()->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) );
			pBar->AutoHideModeSet( true, false, false, true );
			ASSERT_KINDOF( CExtDockBar, pBar->m_pDockBar );

			CWnd * pWndParent = pBar->m_pDockBar;
			for( ; true ; )
			{
				ASSERT_VALID( pWndParent );
				if( pWndParent->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
				{
					if( ! pWndParent->IsKindOf(RUNTIME_CLASS(CExtDockDynBar)) )
					{
						ASSERT( pWndParent->GetParent() == m_pDockSite );
						break;
					} // if( ! pWndParent->IsKindOf(RUNTIME_CLASS(CExtDockDynBar)) )
				} // if( pWndParent->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
				pWndParent = pWndParent->GetParent();
			} // for( ; true ; )
			ASSERT_KINDOF( CExtDockBar, pWndParent );
			ASSERT( ((CExtDockBar*)pWndParent)->_GetCircleNo() > 0 );
			UINT nTopDockBarID = pWndParent->GetDlgCtrlID();
			ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nTopDockBarID );
			CExtDockBar * pOuterDocker =
				STATIC_DOWNCAST(
					CExtDockBar,
					m_pDockSite->GetControlBar( nTopDockBarID )
					);
			ASSERT_VALID( pOuterDocker );
			ASSERT( UINT(pOuterDocker->GetDlgCtrlID()) == nTopDockBarID );
			
			CExtDynAutoHideArea * pAutoHideArea =
				pOuterDocker->_GetAutoHideArea();
			ASSERT_VALID( pAutoHideArea );
			ASSERT( UINT(pAutoHideArea->GetDlgCtrlID()) == nTopDockBarID );
			LONG nPos = pAutoHideArea->FindControlBar( pBar );
			ASSERT( nPos >= 0 );
			
			CExtTabWnd::TAB_ITEM_INFO * pTII =
				pAutoHideArea->ItemGet( nPos );
			pTII->ModifyItemStyle( 0, __ETWI_IN_GROUP_ACTIVE );

		} // for( i = 0; i < nCount; i++ )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	} // if( m_bSerializeResizableBarsState )

	//if( m_bSerializeFixedBarsState )
		((InternalFriendlyFrameWnd *)m_pDockSite)->SetOuterDockState(
			*m_pOuterState,
			m_arrOuterProps,
			m_bSerializeFixedBarsState,
			m_bSerializeResizableBarsState
			);

	if( m_bSerializeResizableBarsState )
	{
		nCount = (INT)m_arrBarStates.GetSize();
		for( i = 0; i< nCount; i++ )
		{
			InternalDockStateBar * pState =
				m_arrBarStates[i];
			ASSERT_VALID( pState );
			pState->PreSyncAttributes( m_pDockSite, NULL );
		}

#ifdef _DEBUG
		for(	pos = m_pDockSite->m_listControlBars.GetHeadPosition();
				pos != NULL;
				)
		{
			CControlBar * pBar = (CControlBar *)
				m_pDockSite->m_listControlBars.GetNext( pos );
			ASSERT_VALID( pBar );
			CExtDockBar * pExtDocker = DYNAMIC_DOWNCAST( CExtDockBar, pBar );
			if( pExtDocker != NULL )
			{
				ASSERT( ! pExtDocker->m_bLockedOptimize );
			}
		}
#endif // _DEBUG
	} // if( m_bSerializeResizableBarsState )

	m_pDockSite->RecalcLayout();

	if( m_bSerializeResizableBarsState )
	{
		nCount = (INT)m_arrBarStates.GetSize();
		for( i = 0; i < nCount; i++ )
		{
			InternalDockStateBar * pState =
				m_arrBarStates[i];
			ASSERT_VALID( pState );
			pState->PostSyncAttributes( m_pDockSite, NULL );
			if( pState->m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING )
			{
				ASSERT_VALID( pState->m_pHelperBar );
				CExtMiniDockFrameWnd * pMiniFrame = STATIC_DOWNCAST( CExtMiniDockFrameWnd, pState->m_pHelperBar->GetParentFrame() );
				pState->m_pHelperBar->_RecalcNcArea();
				pState->m_pHelperBar->OnRepositionSingleChild();
				pMiniFrame->RecalcLayout();
			}
		}
	} // if( m_bSerializeResizableBarsState )

	for(	pos = m_pDockSite->m_listControlBars.GetHeadPosition();
			pos != NULL;
			)
	{
		CControlBar * pBar = (CControlBar *)
			m_pDockSite->m_listControlBars.GetNext( pos );
		ASSERT_VALID( pBar );
		if( pBar->IsDockBar() )
			continue;
		if(		(!pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)))
			||	pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar))
			)
			continue;
		if( !( (CExtControlBar *)pBar)->IsFixedMode() )
			continue;
		((CExtControlBar *)pBar)->_RecalcNcArea();
		if( ! pBar->IsFloating() )
			continue;
		ASSERT_KINDOF( CMiniDockFrameWnd, pBar->GetParentFrame() );
		((CExtControlBar *)pBar)->_RecalcLayoutImpl();
	}


// 	m_pDockSite->SendMessageToDescendants(
// 		WM_IDLEUPDATECMDUI,
// 		(WPARAM)TRUE,
// 		0
// 		);
// 	m_pDockSite->SendMessageToDescendants(
// 		WM_IDLEUPDATECMDUI,
// 		(WPARAM)TRUE,
// 		0,
// 		TRUE,
// 		TRUE
// 		);
	m_pDockSite->SendMessage(
		WM_SETMESSAGESTRING,
		AFX_IDS_IDLEMESSAGE
		);
}

CExtControlBar::InternalDockStateBar::InternalDockStateBar()
{
	InitMembers();
}

CExtControlBar::InternalDockStateBar::~InternalDockStateBar()
{
	Clear();
}

void CExtControlBar::InternalDockStateBar::Serialize( CArchive & ar )
{
	ASSERT_VALID( this );

INT i, nCount;

	if( ar.IsStoring() )
	{
		ar << DWORD(m_nBarID);
		ar << DWORD(m_nDockBarID);
		ar << DWORD(m_nCircleNo);
		ar << DWORD(m_nMRUWidth);
		ar << m_dwBarFlags;
		ar << m_dwDockStateFlags;
		ar << m_dwMRUFloatStyle;
		ar << m_ptFloatPos;
		ar << m_ptMRUFloatPos;
		ar << m_rcMRUDockPos;
		ar << m_rcFloatingFrame;
		ar << m_dwFloatingFrameCreateStyle;
		ar << DWORD(m_nDelayedRowUpdateMetric);
		ar << m_rcBar;
		ar << m_sizeDockedH;
		ar << m_sizeDockedV;
		ar << m_sizeFloated;

		nCount = (INT)m_arrChildStates.GetSize();
		ar << DWORD(nCount);
		for( i = 0; i < nCount; i++ )
		{
			InternalDockStateBar * pState = m_arrChildStates[i];
			ASSERT_VALID( pState );
			pState->Serialize( ar );
		}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		ar << DWORD(m_nAutoHideWidth);
		ar << DWORD(m_nAutoHideHeight);
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	} // if( ar.IsStoring() )
	else
	{
		Clear();
		
		DWORD dwTmp;
		ar >> dwTmp;
		m_nBarID = UINT(dwTmp);
		ar >> dwTmp;
		m_nDockBarID = UINT(dwTmp);
		ar >> dwTmp;
		m_nCircleNo = INT(dwTmp);
		ar >> dwTmp;
		m_nMRUWidth = INT(dwTmp);
		ar >> m_dwBarFlags;
		ar >> m_dwDockStateFlags;
		ar >> m_dwMRUFloatStyle;
		ar >> m_ptFloatPos;
		ar >> m_ptMRUFloatPos;
		ar >> m_rcMRUDockPos;
		ar >> m_rcFloatingFrame;
		ar >> m_dwFloatingFrameCreateStyle;
		ar >> dwTmp;
		m_nDelayedRowUpdateMetric = INT(dwTmp);
		ar >> m_rcBar;
		ar >> m_sizeDockedH;
		ar >> m_sizeDockedV;
		ar >> m_sizeFloated;

		ar >> dwTmp;
		nCount = INT(dwTmp);
		for( i = 0; i < nCount; i++ )
		{
			InternalDockStateBar * pState = new InternalDockStateBar;
			pState->Serialize( ar );
			ASSERT_VALID( pState );
			m_arrChildStates.Add( pState );
		}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		ar >> dwTmp;
		m_nAutoHideWidth = INT(dwTmp);
		ar >> dwTmp;
		m_nAutoHideHeight = INT(dwTmp);
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	} // else from if( ar.IsStoring() )
}

void CExtControlBar::InternalDockStateBar::Clear()
{
	ASSERT_VALID( this );
	InitMembers();

INT i, nCount;

	nCount = (INT)m_arrChildStates.GetSize();
	for( i = 0; i < nCount; i++ )
	{
		InternalDockStateBar * pState = m_arrChildStates[i];
		ASSERT_VALID( pState );
		pState->Clear();
		delete pState;
	}
	m_arrChildStates.RemoveAll();

}

void CExtControlBar::InternalDockStateBar::InitMembers()
{
	ASSERT( this != NULL );
	m_nBarID = m_nDockBarID = AFX_IDW_DOCKBAR_FLOAT;
	m_nCircleNo = 0;
	m_nMRUWidth = 50;
	m_ptFloatPos.x = m_ptFloatPos.y = 0;
	m_rcMRUDockPos.SetRectEmpty();
	m_rcFloatingFrame.SetRect( 0, 0, 200, 200 );
	m_dwFloatingFrameCreateStyle = WS_CHILD;
	m_nDelayedRowUpdateMetric = 0;
	m_rcBar.SetRect( 0, 0, 200, 200 );
	m_dwMRUFloatStyle = 0;
	m_ptMRUFloatPos.x = m_ptMRUFloatPos.y;
	m_sizeDockedH.cx = m_sizeDockedH.cy = 200;
	m_sizeDockedV.cx = m_sizeDockedV.cy = 200;
	m_sizeFloated.cx = m_sizeFloated.cy = 200;

	m_dwDockStateFlags = 0;

	m_nHelperAciveTabChildIndex = -1;
	m_pHelperBar = NULL;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	m_nAutoHideWidth = -1;
	m_nAutoHideHeight = -1;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
}

void CExtControlBar::InternalDockStateBar::StateGet(
	CExtControlBar * pBar,
	CExtControlBar::InternalDockStateBar * pParentState
	)
{
	pParentState;
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	ASSERT( !pBar->IsFixedMode() );
	ASSERT_VALID( pBar->m_pDockBar );
	ASSERT( pBar->m_pDockContext != NULL );
#ifdef _DEBUG
	if( pBar->IsFloating() )
	{
		ASSERT_KINDOF( CDockBar, pBar->m_pDockBar );
		ASSERT( !pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar)) );
	}
	else
	{
		ASSERT_KINDOF( CExtDockBar, pBar->m_pDockBar );
	}
#endif // _DEBUG

	Clear();

	m_nBarID = pBar->GetDlgCtrlID();
	m_nDockBarID = pBar->m_pDockBar->GetDlgCtrlID();
	m_nCircleNo = 0;
	if( ! pBar->IsFloating() )
	{

		ASSERT_KINDOF( CExtDockBar, pBar->m_pDockBar );
		m_nCircleNo = ((CExtDockBar *)pBar->m_pDockBar)->_GetCircleNo();

#ifdef _DEBUG
		CFrameWnd * pParentFrame = pBar->GetParentFrame();
		if( pParentFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
		{
			ASSERT_KINDOF( CExtMiniDockFrameWnd, pParentFrame );
			ASSERT( m_nCircleNo == 0 );
		}
		else
		{
			//ASSERT( m_nCircleNo > 0 );
			ASSERT( pParentFrame == pBar->m_pDockSite );
		}
#endif // _DEBUG

	}

	if( pBar->IsVisible() )
		m_dwDockStateFlags |= __STATE_VISIBLE;
	
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( pBar->AutoHideModeGet() )
	{
		CExtDockDynTabBar * pTabbedDocker =
			DYNAMIC_DOWNCAST(
				CExtDockDynTabBar,
				pBar->m_pDockBar
				);
		if( pTabbedDocker == NULL )
		{
			if( !pBar->IsKindOf(RUNTIME_CLASS(CExtDynTabControlBar)) )
				m_dwDockStateFlags |= __STATE_AUTOHIDDEN;
		}
		else
		{
			ASSERT_VALID( pParentState );
			ASSERT_KINDOF( CExtDynTabControlBar, pTabbedDocker->GetParent() );
			pParentState->m_dwDockStateFlags |= __STATE_AUTOHIDDEN;
		} // else from if( pTabbedDocker == NULL )
	} // if( pBar->AutoHideModeGet() )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	
	m_nMRUWidth = pBar->m_nMRUWidth;

CRect m_rcBar;
	pBar->GetWindowRect( &m_rcBar );
	pBar->m_pDockBar->ScreenToClient( &m_rcBar );

	m_ptFloatPos = pBar->m_ptFloatHelper;
	m_sizeDockedH = pBar->m_sizeDockedH; // + sizeNcArea;
	m_sizeDockedV = pBar->m_sizeDockedV; // + sizeNcArea;
	m_sizeFloated = pBar->m_sizeFloated;

	m_nDelayedRowUpdateMetric = pBar->m_nDelayedRowUpdateMetric;

	m_rcMRUDockPos = pBar->m_pDockContext->m_rectMRUDockPos;
	m_dwMRUFloatStyle = pBar->m_pDockContext->m_dwMRUFloatStyle;
	m_ptMRUFloatPos = pBar->m_pDockContext->m_ptMRUFloatPos;

	m_dwBarFlags = pBar->m_dwStyle;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	m_nAutoHideWidth = pBar->AutoHideWidthGet();
	m_nAutoHideHeight = pBar->AutoHideHeightGet();

	if( pBar->IsKindOf(RUNTIME_CLASS(CExtDynTabControlBar)) )
	{ // if tabbed container
		m_dwDockStateFlags |= InternalDockStateBar::__DOCK_DYNAMIC_TABBED;

		CExtDynTabControlBar * pTabbedBar =
			STATIC_DOWNCAST(
				CExtDynTabControlBar,
				pBar
				);
		
		CExtDockDynTabBar * pTabbedDocker =
			STATIC_DOWNCAST(
				CExtDockDynTabBar,
				pTabbedBar->m_pWndDynDocker //  GetWindow( GW_CHILD )
				);

		ASSERT( pTabbedDocker->m_arrBars.GetSize() > 1 );
		ASSERT( pTabbedDocker->m_arrBars[0] == NULL );

		LONG nSel = -1;
		if( pBar->GetStyle() & WS_VISIBLE )
			nSel = pTabbedBar->GetSwitcherSelection();

		ExtControlBarVector_t vBars;
		pTabbedDocker->CalcOrderedVector( vBars );
		LONG nCount = (LONG)vBars.GetSize();
		//ASSERT( nCount > 1 );
#ifdef _DEBUG
		INT nRealDockedCount = pTabbedDocker->GetDockedCount();
		ASSERT( nCount == nRealDockedCount );
#endif // _DEBUG
		for( LONG nBar = 0; nBar < nCount; nBar++ )
		{
			CExtControlBar * pChildBar = vBars[ nBar ];
			ASSERT_VALID( pChildBar );
			ASSERT_KINDOF( CExtControlBar, pChildBar );
			ASSERT( !pChildBar->IsFixedMode() );

			InternalDockStateBar * _idsb =
				new InternalDockStateBar;
			_idsb->StateGet( pChildBar, this );
			_idsb->m_dwDockStateFlags |= InternalDockStateBar::__STATE_DYNAMIC_CHILD;
			_idsb->m_dwDockStateFlags |= InternalDockStateBar::__STATE_TAB_CHILD;
			if( nSel == nBar )
				_idsb->m_dwDockStateFlags |= InternalDockStateBar::__STATE_TAB_ACTIVE;
			m_arrChildStates.Add( _idsb );
		}
		ASSERT( m_arrChildStates.GetSize() == nRealDockedCount );

		if( pBar->IsFloating() )
		{
			m_dwDockStateFlags |= InternalDockStateBar::__STATE_FLOATING;
			CExtMiniDockFrameWnd * pMiniFrame =
				STATIC_DOWNCAST(
					CExtMiniDockFrameWnd,
					pBar->GetParentFrame()
					);
			pMiniFrame->GetWindowRect( &m_rcFloatingFrame );
			m_dwFloatingFrameCreateStyle = pMiniFrame->GetStyle();
		}

	} // if tabbed container
	else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
	{ // if dynamic half-split container
		m_dwDockStateFlags |= InternalDockStateBar::__DOCK_DYNAMIC_HALF_SPLIT;
		
		CExtDockDynBar * pDynDocker =
			STATIC_DOWNCAST(
				CExtDockDynBar,
				pBar->GetWindow( GW_CHILD )
				);

		INT nCount = (INT)pDynDocker->m_arrBars.GetSize();
		ASSERT( nCount > 1 );
		ASSERT( pDynDocker->m_arrBars[0] == NULL );

		bool bNewRow = false;
		for( INT nBar = 1; nBar < nCount; nBar++ )
		{
			CExtControlBar * pChildBar = (CExtControlBar *)
				pDynDocker->m_arrBars[nBar];
			if( pChildBar == NULL )
			{
				bNewRow = true;
				continue;
			}
			if( __PLACEHODLER_BAR_PTR(pChildBar) )
				continue;
			ASSERT_KINDOF( CExtControlBar, pChildBar );
			ASSERT( !pChildBar->IsFixedMode() );

//			if( pChildBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
//			{
//				if( ((CExtDynControlBar*)pChildBar)->m_pWndDynDocker->GetDockedCount() == 0 )
//					continue;
//			}

			InternalDockStateBar * _idsb =
				new InternalDockStateBar;
			_idsb->StateGet( pChildBar, this );
			if( bNewRow )
				_idsb->m_dwDockStateFlags |= InternalDockStateBar::__STATE_NEW_ROW;
			_idsb->m_dwDockStateFlags |= InternalDockStateBar::__STATE_DYNAMIC_CHILD;
			m_arrChildStates.Add( _idsb );

			bNewRow = false;
		}
#ifdef _DEBUG
		INT nRealDockedCount = pDynDocker->GetDockedCount();
		ASSERT( nRealDockedCount > 0 );
		ASSERT( m_arrChildStates.GetSize() == nRealDockedCount );
#endif // _DEBUG

		if( pBar->IsFloating() )
		{
			m_dwDockStateFlags |= InternalDockStateBar::__STATE_FLOATING;
			CExtMiniDockFrameWnd * pMiniFrame =
				STATIC_DOWNCAST(
					CExtMiniDockFrameWnd,
					pBar->GetParentFrame()
					);
			pMiniFrame->GetWindowRect( &m_rcFloatingFrame );
			m_dwFloatingFrameCreateStyle = pMiniFrame->GetStyle();
		}

	} // if dynamic half-split container
	else
	{ // if bar at side
		m_dwDockStateFlags |= InternalDockStateBar::__DOCK_AT_SIDE;
		ASSERT( !pBar->IsFloating() );
	} // if bar at side

}

void CExtControlBar::InternalDockStateBar::StateSet(
	CFrameWnd * pDockSite,
	CExtControlBar::InternalDockStateBar * pParentState
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pDockSite );

	m_pHelperBar = NULL;

	switch( (m_dwDockStateFlags & __DOCK_AT_MASK) )
	{
	case __DOCK_AT_NOWHERE:
	break; // case __DOCK_AT_NOWHERE
	case __DOCK_AT_SIDE:
	{
		ASSERT_DOCKBAR_DLGCTRLID_DOCKED( m_nDockBarID );
		ASSERT( (m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING) == 0 );
		
		m_pHelperBar = (CExtControlBar *)
			pDockSite->GetControlBar( m_nBarID );
		if( m_pHelperBar == NULL )
		{
//KJ TELESTE	::AfxThrowUserException();
			return;	//KJ TELESTE: Should return.
		}
		ASSERT_VALID( m_pHelperBar );
		ASSERT_KINDOF( CExtControlBar, m_pHelperBar );
		ASSERT( !m_pHelperBar->IsFixedMode() );

		if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_DYNAMIC_CHILD) != 0 )
		{
			ASSERT_VALID( pParentState );
			ASSERT_VALID( pParentState->m_pHelperBar );
			ASSERT_KINDOF( CExtDynControlBar, pParentState->m_pHelperBar );

			CExtDockDynBar * pDockBar =
				STATIC_DOWNCAST(
					CExtDockDynBar,
					(	STATIC_DOWNCAST(
							CExtDynControlBar,
							pParentState->m_pHelperBar
							)
						)->m_pWndDynDocker
					);

			pDockBar->_LockSequenceOptimization( true );
			
			m_pHelperBar->m_bUpdatingChain = true;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_TAB_CHILD) != 0 )
			{
				ASSERT_KINDOF( CExtDynTabControlBar, pParentState->m_pHelperBar );
				((CExtDynTabControlBar*)pParentState->m_pHelperBar)
					-> InsertBar(
						m_pHelperBar,
						-1,
						false
						);
			} // if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_TAB_CHILD) != 0 )
			else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			{
				pDockBar->DockControlBar( m_pHelperBar );
			} // else from if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_TAB_CHILD) != 0 )
			
			m_pHelperBar->MoveWindow( &m_rcBar, FALSE );
			m_pHelperBar->m_bUpdatingChain = false;
			
			pDockBar->_LockSequenceOptimization( false );

			INT nBarPos = 
				pDockBar->FindBar( m_pHelperBar );
			ASSERT( nBarPos >= 1 );
			if( nBarPos > 1 )
			{
				if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
				{
					if( pDockBar->m_arrBars[nBarPos-1] != NULL  )
						pDockBar->m_arrBars.InsertAt(
							nBarPos,
							(CControlBar *)NULL
							);
				} // if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
				else
				{
					if( pDockBar->m_arrBars[nBarPos-1] == NULL  )
						pDockBar->m_arrBars.RemoveAt( nBarPos-1 );
				} // else from if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
			} // if( nBarPos > 1 )
		} // if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_DYNAMIC_CHILD) != 0 )
		else
		{
			ASSERT( m_nCircleNo > 0 );

			ASSERT_VALID( m_pHelperBar->m_pDockBar );
			ASSERT( m_pHelperBar->m_pDockContext != NULL );
			ASSERT_KINDOF( CExtDockBar, m_pHelperBar->m_pDockBar );

			CExtDockBar * pDockBar =
				STATIC_DOWNCAST(
					CExtDockBar,
					pDockSite->GetControlBar( m_nDockBarID )
					);
			pDockBar =
				pDockBar->_GetBarByCircleNo( m_nCircleNo );

			pDockBar->_LockSequenceOptimization( true );
			pDockBar->DockControlBar( m_pHelperBar );
			pDockBar->_LockSequenceOptimization( false );

			INT nBarPos = 
				pDockBar->FindBar( m_pHelperBar );
			ASSERT( nBarPos >= 1 );
			if( nBarPos > 1 )
			{
				if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
				{
					if( pDockBar->m_arrBars[nBarPos-1] != NULL  )
						pDockBar->m_arrBars.InsertAt(
							nBarPos,
							(CControlBar *)NULL
							);
				} // if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
				else
				{
					if( pDockBar->m_arrBars[nBarPos-1] == NULL  )
						pDockBar->m_arrBars.RemoveAt( nBarPos-1 );
				} // else from if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
			} // if( nBarPos > 1 )
		} // else from if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_DYNAMIC_CHILD) != 0 )
	}
	break; // case __DOCK_AT_SIDE
	case __DOCK_DYNAMIC_HALF_SPLIT:
	{
		m_pHelperBar = new CExtDynControlBar;

		DWORD dwDynStyle =
				WS_CHILD
				| (m_dwDockStateFlags & CBRS_ALIGN_ANY)
				;
		dwDynStyle |= CBRS_BORDER_ANY;
		dwDynStyle &= ~( CBRS_FLOATING | CBRS_GRIPPER );
		dwDynStyle |= CBRS_TOOLTIPS;

		m_pHelperBar->SetInitDesiredSizeHorizontal( m_sizeDockedH );
		m_pHelperBar->SetInitDesiredSizeVertical( m_sizeDockedV );
		m_pHelperBar->SetInitDesiredSizeFloating( m_sizeFloated );
		if(	! m_pHelperBar->Create(
				NULL,
				pDockSite, // pParentFrame
				m_nBarID,
				dwDynStyle
				)
			)
		{
			ASSERT( FALSE );
			return;
		}
		ASSERT( ((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker != NULL );
		ASSERT( ((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker->GetSafeHwnd() != NULL );
		m_pHelperBar->m_pDockSite = pDockSite;
		((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker->m_pDockSite = pDockSite;
		m_pHelperBar->m_hWndOwner = pDockSite->GetSafeHwnd();
		m_pHelperBar->EnableDocking( CBRS_ALIGN_ANY );
		ASSERT( m_pHelperBar->m_pDockContext != NULL );
		ASSERT( m_pHelperBar->m_pDockContext->m_pDockSite == pDockSite );
		ASSERT_VALID( m_pHelperBar );

		if( pDockSite->m_listControlBars.Find(m_pHelperBar) == NULL )
			pDockSite->m_listControlBars.AddTail( m_pHelperBar );

		m_pHelperBar->OnNcAreaButtonsReinitialize();
		CFrameWnd * pParentFrame = pDockSite;
		if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING) != 0 )
		{
			CExtMiniDockFrameWnd * pMiniFrame =
				m_pHelperBar->OnCreateFloatingFrame(
					m_dwFloatingFrameCreateStyle
					);
			pParentFrame = pMiniFrame;
			CRect rcFrameAdjusted =
				CExtPaintManager::stat_AlignWndRectToMonitor(
					m_rcFloatingFrame
					);
			pMiniFrame->ModifyStyle( FWS_SNAPTOBARS, 0 );
			pMiniFrame->MoveWindow( &rcFrameAdjusted, FALSE );
			pMiniFrame->DelayRecalcLayout();
		} // if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING) != 0 )

		if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING) != 0 )
		{
			CExtMiniDockFrameWnd * pMiniFrame =
				STATIC_DOWNCAST(
					CExtMiniDockFrameWnd,
					pParentFrame
					);
			CDockBar * pFloatingDocker =
				STATIC_DOWNCAST(
					CDockBar,
					pMiniFrame->GetWindow(GW_CHILD)
					);
			ASSERT( !pFloatingDocker->IsKindOf(RUNTIME_CLASS(CExtDockBar)) );
			ASSERT( pFloatingDocker->GetDlgCtrlID() == AFX_IDW_DOCKBAR_FLOAT );
			m_pHelperBar->SetParent( pFloatingDocker );
		} // if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING) != 0 )

		if( !((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker->m_bHelperDockSiteModified )
		{
			POSITION posToRemove =
				pDockSite->m_listControlBars.Find(
					((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker
					);
			if( posToRemove != NULL )
			{
				pDockSite->m_listControlBars.RemoveAt( posToRemove );
				((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker->m_bHelperDockSiteModified = true;
			} // if( posToRemove != NULL )
		} // if( !((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker->m_bHelperDockSiteModified )

		if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING) != 0 )
		{
			CDockBar * pDockBar =
				STATIC_DOWNCAST(
					CDockBar,
					pParentFrame->GetWindow( GW_CHILD )
					);
			ASSERT_KINDOF( CDockBar, pDockBar );
			ASSERT( !pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar)) );
			ASSERT( pDockBar->GetDlgCtrlID() ==	AFX_IDW_DOCKBAR_FLOAT );
			((CExtDockBar*)pDockBar)->DockControlBar( m_pHelperBar );
			ASSERT( pDockBar->IsFloating() );
			ASSERT( m_pHelperBar->IsFloating() );
			
			m_pHelperBar->m_bUpdatingChain = true;
			m_pHelperBar->MoveWindow( &m_rcBar, FALSE );
			m_pHelperBar->m_bUpdatingChain = false;
		} // if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING) != 0 )
		else if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_DYNAMIC_CHILD) != 0 )
		{
			ASSERT_VALID( pParentState );
			ASSERT_VALID( pParentState->m_pHelperBar );
			ASSERT_KINDOF( CExtDynControlBar, m_pHelperBar );

			CExtDockBar * pDockBar =
				STATIC_DOWNCAST(
					CExtDockBar,
					(	STATIC_DOWNCAST(
							CExtDynControlBar,
							pParentState->m_pHelperBar
							)
						)->m_pWndDynDocker
					);

			pDockBar->_LockSequenceOptimization( true );

			pDockBar->DockControlBar( m_pHelperBar );
			
			m_pHelperBar->m_bUpdatingChain = true;
			m_pHelperBar->MoveWindow( &m_rcBar, FALSE );
			m_pHelperBar->m_bUpdatingChain = false;
			
			pDockBar->_LockSequenceOptimization( false );

			INT nBarPos = 
				pDockBar->FindBar( m_pHelperBar );
			ASSERT( nBarPos >= 1 );
			if( nBarPos > 1 )
			{
				if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
				{
					if( pDockBar->m_arrBars[nBarPos-1] != NULL  )
						pDockBar->m_arrBars.InsertAt(
							nBarPos,
							(CControlBar *)NULL
							);
				} // if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
				else
				{
					if( pDockBar->m_arrBars[nBarPos-1] == NULL  )
						pDockBar->m_arrBars.RemoveAt( nBarPos-1 );
				} // else from if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
			} // if( nBarPos > 1 )
		} // else if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_DYNAMIC_CHILD) != 0 )
		else
		{
			ASSERT( m_nCircleNo > 0 );
			ASSERT( m_pHelperBar->m_pDockContext != NULL );

			CExtDockBar * pDockBar =
				STATIC_DOWNCAST(
					CExtDockBar,
					pDockSite->GetControlBar( m_nDockBarID )
					);
			pDockBar =
				pDockBar->_GetBarByCircleNo( m_nCircleNo );

			pDockBar->_LockSequenceOptimization( true );
			
			pDockBar->DockControlBar( m_pHelperBar );
			
			m_pHelperBar->m_bUpdatingChain = true;
			m_pHelperBar->MoveWindow( &m_rcBar, FALSE );
			m_pHelperBar->m_bUpdatingChain = false;
			
			pDockBar->_LockSequenceOptimization( false );

			INT nBarPos = 
				pDockBar->FindBar( m_pHelperBar );
			ASSERT( nBarPos >= 1 );
			if( nBarPos > 1 )
			{
				if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
				{
					if( pDockBar->m_arrBars[nBarPos-1] != NULL  )
						pDockBar->m_arrBars.InsertAt(
							nBarPos,
							(CControlBar *)NULL
							);
				} // if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
				else
				{
					if( pDockBar->m_arrBars[nBarPos-1] == NULL  )
						pDockBar->m_arrBars.RemoveAt( nBarPos-1 );
				} // else from if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
			} // if( nBarPos > 1 )
		} // else from if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_DYNAMIC_CHILD) != 0 )

		INT nCount = (INT)m_arrChildStates.GetSize();
		for( INT nBar = 0; nBar < nCount; nBar ++ )
		{
			InternalDockStateBar * pState = m_arrChildStates[nBar];
			ASSERT_VALID( pState );
			pState->StateSet( pDockSite, this );
		}
	
#ifdef _DEBUG
		INT nRealDockedCount =
			((CExtDynControlBar *)m_pHelperBar)
				-> m_pWndDynDocker
				-> GetDockedCount();
		ASSERT( nRealDockedCount == nCount );
#endif // _DEBUG

	}
	break; // case __DOCK_DYNAMIC_HALF_SPLIT
	
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	case __DOCK_DYNAMIC_TABBED:
	{
		m_pHelperBar = NULL;
		pDockSite->SendMessage(
			g_nMsgCreateTabbedBar,
			WPARAM( &m_pHelperBar )
			);
#ifdef _DEBUG
		if( m_pHelperBar != NULL )
		{
			ASSERT_VALID( m_pHelperBar );
			ASSERT_KINDOF( CExtDynTabControlBar, m_pHelperBar );
		} // if( m_pHelperBar != NULL )
#endif // _DEBUG
		if( m_pHelperBar == NULL )
			m_pHelperBar = new CExtDynTabControlBar;

		DWORD dwDynStyle =
				WS_CHILD
				| (m_dwDockStateFlags & CBRS_ALIGN_ANY)
				;
		dwDynStyle |= CBRS_BORDER_ANY;
		dwDynStyle &= ~( CBRS_FLOATING | CBRS_GRIPPER );
		dwDynStyle |= CBRS_TOOLTIPS;

		m_pHelperBar->SetInitDesiredSizeHorizontal( m_sizeDockedH );
		m_pHelperBar->SetInitDesiredSizeVertical( m_sizeDockedV );
		m_pHelperBar->SetInitDesiredSizeFloating( m_sizeFloated );
		if(	! m_pHelperBar->Create(
				NULL,
				pDockSite, // pParentFrame
				m_nBarID,
				dwDynStyle
				)
			)
		{
			ASSERT( FALSE );
			return;
		}
		ASSERT( ((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker != NULL );
		ASSERT( ((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker->GetSafeHwnd() != NULL );
		m_pHelperBar->m_pDockSite = pDockSite;
		((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker->m_pDockSite = pDockSite;
		m_pHelperBar->m_hWndOwner = pDockSite->GetSafeHwnd();
		m_pHelperBar->EnableDocking( CBRS_ALIGN_ANY );
		ASSERT( m_pHelperBar->m_pDockContext != NULL );
		ASSERT( m_pHelperBar->m_pDockContext->m_pDockSite == pDockSite );
		ASSERT_VALID( m_pHelperBar );

		if( pDockSite->m_listControlBars.Find(m_pHelperBar) == NULL )
			pDockSite->m_listControlBars.AddTail( m_pHelperBar );

		m_pHelperBar->OnNcAreaButtonsReinitialize();
		CFrameWnd * pParentFrame = pDockSite;
		if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING) != 0 )
		{
			CExtMiniDockFrameWnd * pMiniFrame =
				m_pHelperBar->OnCreateFloatingFrame(
					m_dwFloatingFrameCreateStyle
					);
			CRect rcFrameAdjusted =
				CExtPaintManager::stat_AlignWndRectToMonitor(
					m_rcFloatingFrame
					);
			pMiniFrame->ModifyStyle( FWS_SNAPTOBARS, 0 );
			pMiniFrame->MoveWindow( &rcFrameAdjusted, FALSE );
			pMiniFrame->DelayRecalcLayout();
			pParentFrame = pMiniFrame;
		} // if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING) != 0 )

		if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING) != 0 )
		{
			CExtMiniDockFrameWnd * pMiniFrame =
				STATIC_DOWNCAST(
					CExtMiniDockFrameWnd,
					pParentFrame
					);
			CDockBar * pFloatingDocker =
				STATIC_DOWNCAST(
					CDockBar,
					pMiniFrame->GetWindow(GW_CHILD)
					);
			ASSERT( !pFloatingDocker->IsKindOf(RUNTIME_CLASS(CExtDockBar)) );
			ASSERT( pFloatingDocker->GetDlgCtrlID() == AFX_IDW_DOCKBAR_FLOAT );
			m_pHelperBar->SetParent( pFloatingDocker );
		} // if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING) != 0 )

		if( !((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker->m_bHelperDockSiteModified )
		{
			POSITION posToRemove =
				pDockSite->m_listControlBars.Find(
					((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker
					);
			if( posToRemove != NULL )
			{
				pDockSite->m_listControlBars.RemoveAt( posToRemove );
				((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker->m_bHelperDockSiteModified = true;
			} // if( posToRemove != NULL )
		} // if( !((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker->m_bHelperDockSiteModified )

		if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING) != 0 )
		{
			CDockBar * pDockBar =
				STATIC_DOWNCAST(
					CDockBar,
					pParentFrame->GetWindow( GW_CHILD )
					);
			ASSERT_KINDOF( CDockBar, pDockBar );
			ASSERT( !pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar)) );
			ASSERT( pDockBar->GetDlgCtrlID() ==	AFX_IDW_DOCKBAR_FLOAT );
			((CExtDockBar*)pDockBar)->DockControlBar( m_pHelperBar );
			ASSERT( pDockBar->IsFloating() );
			ASSERT( m_pHelperBar->IsFloating() );
			
			m_pHelperBar->m_bUpdatingChain = true;
			m_pHelperBar->MoveWindow( &m_rcBar, FALSE );
			m_pHelperBar->m_bUpdatingChain = false;
		} // if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING) != 0 )
		else if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_DYNAMIC_CHILD) != 0 )
		{
			ASSERT_VALID( pParentState );
			ASSERT_VALID( pParentState->m_pHelperBar );
			ASSERT_KINDOF( CExtDynControlBar, m_pHelperBar );

			CExtDockBar * pDockBar =
				STATIC_DOWNCAST(
					CExtDockBar,
					(	STATIC_DOWNCAST(
							CExtDynControlBar,
							pParentState->m_pHelperBar
							)
						)->m_pWndDynDocker
					);

			pDockBar->_LockSequenceOptimization( true );
			
			pDockBar->DockControlBar( m_pHelperBar );
			
			m_pHelperBar->m_bUpdatingChain = true;
			m_pHelperBar->MoveWindow( &m_rcBar, FALSE );
			m_pHelperBar->m_bUpdatingChain = false;
			
			pDockBar->_LockSequenceOptimization( false );

			INT nBarPos = 
				pDockBar->FindBar( m_pHelperBar );
			ASSERT( nBarPos >= 1 );
			if( nBarPos > 1 )
			{
				if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
				{
					if( pDockBar->m_arrBars[nBarPos-1] != NULL  )
						pDockBar->m_arrBars.InsertAt(
							nBarPos,
							(CControlBar *)NULL
							);
				} // if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
				else
				{
					if( pDockBar->m_arrBars[nBarPos-1] == NULL  )
						pDockBar->m_arrBars.RemoveAt( nBarPos-1 );
				} // else from if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
			} // if( nBarPos > 1 )
		} // else if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_DYNAMIC_CHILD) != 0 )
		else
		{
			ASSERT( m_nCircleNo > 0 );
			ASSERT( m_pHelperBar->m_pDockContext != NULL );

			CExtDockBar * pDockBar =
				STATIC_DOWNCAST(
					CExtDockBar,
					pDockSite->GetControlBar( m_nDockBarID )
					);
			pDockBar =
				pDockBar->_GetBarByCircleNo( m_nCircleNo );

			pDockBar->_LockSequenceOptimization( true );
			
			pDockBar->DockControlBar( m_pHelperBar );
			
			m_pHelperBar->m_bUpdatingChain = true;
			m_pHelperBar->MoveWindow( &m_rcBar, FALSE );
			m_pHelperBar->m_bUpdatingChain = false;
			
			pDockBar->_LockSequenceOptimization( false );

			INT nBarPos = 
				pDockBar->FindBar( m_pHelperBar );
			ASSERT( nBarPos >= 1 );
			if( nBarPos > 1 )
			{
				if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
				{
					if( pDockBar->m_arrBars[nBarPos-1] != NULL  )
						pDockBar->m_arrBars.InsertAt(
							nBarPos,
							(CControlBar *)NULL
							);
				} // if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
				else
				{
					if( pDockBar->m_arrBars[nBarPos-1] == NULL  )
						pDockBar->m_arrBars.RemoveAt( nBarPos-1 );
				} // else from if( (m_dwDockStateFlags & __STATE_NEW_ROW) != 0 )
			} // if( nBarPos > 1 )
		} // else from if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_DYNAMIC_CHILD) != 0 )

		INT nCount = (INT)m_arrChildStates.GetSize();
		//ASSERT( nCount > 1 );
		m_nHelperAciveTabChildIndex = -1;
		for( INT nBar = 0; nBar < nCount; nBar ++ )
		{
			InternalDockStateBar * pState = m_arrChildStates[nBar];
			ASSERT_VALID( pState );
			if( (pState->m_dwDockStateFlags & InternalDockStateBar::__STATE_TAB_ACTIVE) != 0 )
			{
				if( (pState->m_dwDockStateFlags & InternalDockStateBar::__STATE_VISIBLE) != 0 )
				{
					ASSERT( m_nHelperAciveTabChildIndex == -1 );
					m_nHelperAciveTabChildIndex = nBar;
				}
			}
			pState->StateSet( pDockSite, this );
		}

#ifdef _DEBUG
		INT nRealDockedCount =
			((CExtDynTabControlBar *)m_pHelperBar)
				-> m_pWndDynDocker
				-> GetDockedCount();
		ASSERT( nRealDockedCount == nCount );
#endif // _DEBUG

	}
	break; // case __DOCK_DYNAMIC_TABBED
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

#ifdef _DEBUG
	default:
		{
			ASSERT( FALSE );
		}
	break;
#endif // _DEBUG
	} // switch( (m_dwDockStateFlags & __DOCK_AT_MASK) )

	ASSERT_VALID( m_pHelperBar );

//bool bBarWndVisible = ( ( m_pHelperBar->GetStyle() & WS_VISIBLE ) != 0 ) ? true : false;
//	if( (m_dwDockStateFlags & __STATE_VISIBLE) != 0 )
//	{
//		if( !bBarWndVisible )
//			pDockSite->ShowControlBar( m_pHelperBar, TRUE, TRUE );
//	} // if( (m_dwDockStateFlags & __STATE_VISIBLE) != 0 )
//	else
//	{
//		if( bBarWndVisible )
//			pDockSite->ShowControlBar( m_pHelperBar, FALSE, TRUE );
//	} // else from if( (m_dwDockStateFlags & __STATE_VISIBLE) != 0 )

	pDockSite->ShowControlBar(
		m_pHelperBar,
		(m_dwDockStateFlags & __STATE_VISIBLE) ? TRUE : FALSE,
		TRUE
		);

CExtMiniDockFrameWnd * pMiniFrame =
		DYNAMIC_DOWNCAST(
			CExtMiniDockFrameWnd,
			m_pHelperBar->GetParentFrame()
			);
	if( pMiniFrame != NULL )
	{
		ASSERT_VALID( pMiniFrame );
		INT nCount = (INT)pMiniFrame->m_listControlBars.GetCount();
		if( nCount != 0 )
		{
			ASSERT( nCount == 1 );
			pMiniFrame->m_listControlBars.RemoveAll();
		}

		CRect rcFrameInitial;
		pMiniFrame->GetWindowRect( &rcFrameInitial );
		CRect rcFrameAdjusted =
			CExtPaintManager::stat_AlignWndRectToMonitor(
				rcFrameInitial
				);
		if( rcFrameAdjusted != rcFrameInitial )
		{
			pMiniFrame->MoveWindow( &rcFrameAdjusted, FALSE );
			pMiniFrame->DelayRecalcLayout();
		} // if( rcFrameAdjusted != rcFrameInitial )
	} // if( pMiniFrame != NULL )


	m_pHelperBar->m_dwStyle = m_dwBarFlags;

	m_pHelperBar->m_ptFloatHelper = m_ptFloatPos;

	m_pHelperBar->m_sizeDockedH = m_sizeDockedH;
	m_pHelperBar->m_sizeDockedV = m_sizeDockedV;
	m_pHelperBar->m_sizeFloated = m_sizeFloated;

	m_pHelperBar->m_nDelayedRowUpdateMetric = m_nDelayedRowUpdateMetric;

	m_pHelperBar->m_nMRUWidth = m_nMRUWidth;

	m_pHelperBar->m_pDockContext->m_rectMRUDockPos = m_rcMRUDockPos;
	m_pHelperBar->m_pDockContext->m_dwMRUFloatStyle = m_dwMRUFloatStyle;
	m_pHelperBar->m_pDockContext->m_ptMRUFloatPos = m_ptMRUFloatPos;

//	if( m_dwDockStateFlags & __STATE_AUTOHIDDEN
//		&& !m_pHelperBar->AutoHideModeGet()
//		)
//	{
//		VERIFY(
//			m_pHelperBar->AutoHideModeSet(
//				true,
//				false,
//				false,
//				false
//				)
//			);
//	}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	m_pHelperBar->AutoHideWidthSet( m_nAutoHideWidth );
	m_pHelperBar->AutoHideHeightSet( m_nAutoHideHeight );
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

}

void CExtControlBar::InternalDockStateBar::PreSyncAttributes(
	CFrameWnd * pDockSite,
	CExtControlBar::InternalDockStateBar * pParentState
	)
{
//KJ TELESTE	ASSERT_VALID( this );
//KJ TELESTE	ASSERT_VALID( m_pHelperBar );
	pParentState;
//KJ TELESTE: Should be checked.
	if( m_pHelperBar == NULL )
		return;

	m_pHelperBar->m_ptFloatHelper = m_ptFloatPos;

	m_pHelperBar->m_sizeDockedH = m_sizeDockedH;
	m_pHelperBar->m_sizeDockedV = m_sizeDockedV;
	m_pHelperBar->m_sizeFloated = m_sizeFloated;

	m_pHelperBar->m_nDelayedRowUpdateMetric = m_nDelayedRowUpdateMetric;

	m_pHelperBar->m_nMRUWidth = m_nMRUWidth;

	m_pHelperBar->m_pDockContext->m_rectMRUDockPos = m_rcMRUDockPos;
	m_pHelperBar->m_pDockContext->m_dwMRUFloatStyle = m_dwMRUFloatStyle;
	m_pHelperBar->m_pDockContext->m_ptMRUFloatPos = m_ptMRUFloatPos;

	INT nCount = (INT)m_arrChildStates.GetSize();
	for( INT nBar = 0; nBar < nCount; nBar++ )
	{
		InternalDockStateBar * pState = m_arrChildStates[nBar];
		ASSERT_VALID( pState );
		pState->PreSyncAttributes( pDockSite, this );
	}
}

void CExtControlBar::InternalDockStateBar::PostSyncAttributes(
	CFrameWnd * pDockSite,
	CExtControlBar::InternalDockStateBar * pParentState
	)
{
//KJ TELESTE	ASSERT_VALID( this );
//KJ TELESTE	ASSERT_VALID( m_pHelperBar );
	pParentState;
//KJ TELESTE:
	if( m_pHelperBar == NULL )
		return;

bool bWalkChild = false;
	switch( (m_dwDockStateFlags & __DOCK_AT_MASK) )
	{
	case __DOCK_AT_NOWHERE:
	{
	}
	break; // case __DOCK_AT_NOWHERE
	case __DOCK_AT_SIDE:
	{
	}
	break; // case __DOCK_AT_SIDE
	case __DOCK_DYNAMIC_HALF_SPLIT:
	{
		bWalkChild = true;
		ASSERT_KINDOF( CExtDynControlBar, m_pHelperBar );
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		ASSERT( !m_pHelperBar->IsKindOf(RUNTIME_CLASS(CExtDynTabControlBar)) );
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

		if( !((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker->m_bHelperDockSiteModified )
		{
			POSITION posToRemove =
				pDockSite->m_listControlBars.Find(
					((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker
					);
			if( posToRemove != NULL )
			{
				pDockSite->m_listControlBars.RemoveAt( posToRemove );
				((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker->m_bHelperDockSiteModified = true;
			} // if( posToRemove != NULL )
		} // if( !((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker->m_bHelperDockSiteModified )

		if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING) != 0 )
		{
			CExtMiniDockFrameWnd * pParentFrame =
				STATIC_DOWNCAST(
					CExtMiniDockFrameWnd,
					m_pHelperBar->GetParentFrame()
					);
			pParentFrame->RecalcLayout();
		}

#ifdef _DEBUG
		INT nChildsCount = (INT)m_arrChildStates.GetSize();
		INT nRealDockedCount =
			((CExtDynControlBar *)m_pHelperBar)
				-> m_pWndDynDocker
				-> GetDockedCount();
		ASSERT( nRealDockedCount == nChildsCount );
#endif // _DEBUG
	}
	break; // case __DOCK_DYNAMIC_HALF_SPLIT
	
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	case __DOCK_DYNAMIC_TABBED:
	{
		bWalkChild = true;
		ASSERT_KINDOF( CExtDynTabControlBar, m_pHelperBar );

		if( !((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker->m_bHelperDockSiteModified )
		{
			POSITION posToRemove =
				pDockSite->m_listControlBars.Find(
					((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker
					);
			if( posToRemove != NULL )
			{
				pDockSite->m_listControlBars.RemoveAt( posToRemove );
				((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker->m_bHelperDockSiteModified = true;
			} // if( posToRemove != NULL )
		} // if( !((CExtDynControlBar *)m_pHelperBar)->m_pWndDynDocker->m_bHelperDockSiteModified )

		if( (m_dwDockStateFlags & InternalDockStateBar::__STATE_FLOATING) != 0 )
		{
			CExtMiniDockFrameWnd * pParentFrame =
				STATIC_DOWNCAST(
					CExtMiniDockFrameWnd,
					m_pHelperBar->GetParentFrame()
					);
			pParentFrame->RecalcLayout();
		}

		if( m_nHelperAciveTabChildIndex >= 0 )
		{
			//ASSERT( m_pHelperBar->GetStyle() & WS_VISIBLE );
			if( (m_pHelperBar->GetStyle() & WS_VISIBLE) == 0 )
				m_pHelperBar->ShowWindow( SW_SHOWNA );

			CExtDynTabControlBar * pTabbedBar =
				STATIC_DOWNCAST(
					CExtDynTabControlBar,
					m_pHelperBar
					);
			LONG nSel = 
				pTabbedBar->GetSwitcherSelection();
			if(		nSel != m_nHelperAciveTabChildIndex
				&&	m_nHelperAciveTabChildIndex >= 0
				&&	pTabbedBar->GetBarAt(m_nHelperAciveTabChildIndex,true)->IsVisible()
				)
				pTabbedBar->SetSwitcherSelection(
					m_nHelperAciveTabChildIndex,
					true,
					true
					);
		}
	
#ifdef _DEBUG
		INT nChildsCount = (INT)m_arrChildStates.GetSize();
		INT nRealDockedCount =
			((CExtDynTabControlBar *)m_pHelperBar)
				-> m_pWndDynDocker
				-> GetDockedCount();
		ASSERT( nRealDockedCount == nChildsCount );
#endif // _DEBUG
	}
	break; // case __DOCK_DYNAMIC_TABBED
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

#ifdef _DEBUG
	default:
		{
			ASSERT( FALSE );
		}
	break;
#endif // _DEBUG
	} // switch( (m_dwDockStateFlags & __DOCK_AT_MASK) )

	if( bWalkChild )
	{
		INT nCount = (INT)m_arrChildStates.GetSize();
		ASSERT( nCount > 0 );
		for( INT nBar = 0; nBar < nCount; nBar++ )
		{
			InternalDockStateBar * pState = m_arrChildStates[nBar];
			ASSERT_VALID( pState );
			pState->PostSyncAttributes( pDockSite, this );
		}
	}

//KJ TELESTE: Should be checked.
	if( m_pHelperBar == NULL )
		return;


	m_pHelperBar->m_ptFloatHelper = m_ptFloatPos;

	m_pHelperBar->m_sizeDockedH = m_sizeDockedH;
	m_pHelperBar->m_sizeDockedV = m_sizeDockedV;
	m_pHelperBar->m_sizeFloated = m_sizeFloated;

	m_pHelperBar->m_nDelayedRowUpdateMetric = m_nDelayedRowUpdateMetric;

	m_pHelperBar->m_nMRUWidth = m_nMRUWidth;

	m_pHelperBar->m_pDockContext->m_rectMRUDockPos = m_rcMRUDockPos;
	m_pHelperBar->m_pDockContext->m_dwMRUFloatStyle = m_dwMRUFloatStyle;
	m_pHelperBar->m_pDockContext->m_ptMRUFloatPos = m_ptMRUFloatPos;
}

CTypedPtrMap < CMapPtrToWord, CFrameWnd *, WORD > CExtControlBar::g_mapSF;

bool CExtControlBar::ProfileBarStateFramePush(
	CFrameWnd * pFrame,
	bool bIsLoading
	)
{
	ASSERT_VALID( pFrame );
WORD wLoadingFlagValue = 0;
	if( g_mapSF.Lookup( pFrame, wLoadingFlagValue ) )
		return false;
	wLoadingFlagValue = bIsLoading ? WORD(1) : WORD(0);
	g_mapSF.SetAt( pFrame, wLoadingFlagValue );
	return true;
}

bool CExtControlBar::ProfileBarStateFramePop(
	CFrameWnd * pFrame
	)
{
	ASSERT_VALID( pFrame );
WORD wLoadingFlagValue = 0;
	if( ! g_mapSF.Lookup( pFrame, wLoadingFlagValue ) )
		return false;
	g_mapSF.RemoveKey( pFrame );
	return true;
}

bool CExtControlBar::ProfileBarStateIsSerializing(
	CFrameWnd * pFrame,
	bool * p_bIsLoading // = NULL
	)
{
	if( pFrame == NULL )
		return false;
	if( pFrame->GetSafeHwnd() == NULL )
		return false;
	ASSERT_VALID( pFrame );
WORD wLoadingFlagValue = 0;
	if( ! g_mapSF.Lookup( pFrame, wLoadingFlagValue ) )
		return false;
	if( p_bIsLoading != NULL )
		(*p_bIsLoading) =
			( wLoadingFlagValue != 0 )
				? true : false;
	return true;
}

void CExtControlBar::OnExtBarStateSerializationComplete(
	bool bStore
	)
{
	ASSERT_VALID( this );
	bStore;
}

bool CExtControlBar::ProfileBarStateSerialize(
	CArchive & ar,
	CFrameWnd * pFrame,
	LPWINDOWPLACEMENT pFrameWp, // = NULL // need serialize frame's WP
	bool bSerializeFixedBarsState, // = true,
	bool bSerializeResizableBarsState, // = true
	bool bEnableThrowExceptions // = false
	)
{
	ASSERT_VALID( pFrame );
	ASSERT( pFrame->m_hWnd != NULL );
	ASSERT( ::IsWindow( pFrame->m_hWnd ) );
	if( ! ProfileBarStateFramePush(
			pFrame,
			ar.IsLoading() ? true : false
			)
		)
	{
		if( bEnableThrowExceptions )
			::AfxThrowUserException();
		return false;
	}
bool bRetVal = false;
DWORD dwGeneralFlags = 0;
DWORD dwReserved = 0L;
	try
	{
		try
		{
			InternalDockStateSite state(
				pFrame,
				bSerializeFixedBarsState,
				bSerializeResizableBarsState
				);

			if( ar.IsStoring() )
			{
				if( bSerializeFixedBarsState )
					dwGeneralFlags |= 0x00000001;
				if( bSerializeResizableBarsState )
					dwGeneralFlags |= 0x00000002;
				ar << dwGeneralFlags;
				ar << dwReserved;
				ar << dwReserved;
				ar << dwReserved;
				ar << dwReserved;
				((InternalFriendlyFrameWnd *)pFrame)->
					GetDockState(
						state
						);
				if( state.m_wp.showCmd == SW_SHOWMINIMIZED )
				{
					if( ( state.m_wp.flags & WPF_RESTORETOMAXIMIZED ) != 0 )
						state.m_wp.showCmd = SW_MAXIMIZE;
					else
						state.m_wp.showCmd = SW_NORMAL;
				} // if( state.m_wp.showCmd == SW_SHOWMINIMIZED )
				state.Serialize( ar );
				CExtControlBar::stat_SetMessageString( pFrame );
			} // if( ar.IsStoring() )
			else
			{
				DWORD dwTmp = 0;
				if( bSerializeFixedBarsState )
					dwTmp |= 0x00000001;
				if( bSerializeResizableBarsState )
					dwTmp |= 0x00000002;
				ar >> dwGeneralFlags;
				if( dwGeneralFlags != dwTmp )
				{
					if( bEnableThrowExceptions )
						::AfxThrowUserException();
					VERIFY( ProfileBarStateFramePop( pFrame ) );
					return false;
				}
				ar >> dwReserved;
				ar >> dwReserved;
				ar >> dwReserved;
				ar >> dwReserved;
				// win xp fix - begin
				WINDOWPLACEMENT _wpf;
				::memset( &_wpf, 0, sizeof(WINDOWPLACEMENT) );
				_wpf.length = sizeof(WINDOWPLACEMENT);
				CWnd * pWndPlacement = stat_GetWndForPlacement( pFrame );
				ASSERT_VALID( pWndPlacement );
				//if( pWndPlacement->GetWindowPlacement(&_wpf) )
				if( CExtControlBar::stat_GetWindowPlacement( pWndPlacement->m_hWnd, _wpf ) )
				{
					_wpf.ptMinPosition.x = _wpf.ptMinPosition.y = 0;
					_wpf.ptMaxPosition.x = _wpf.ptMaxPosition.y = 0;
					_wpf.showCmd = 
						(pWndPlacement->GetStyle() & WS_VISIBLE)
							? SW_SHOWNA
							: SW_HIDE;
					//pWndPlacement->SetWindowPlacement(&_wpf);
					CExtControlBar::stat_SetWindowPlacement( pWndPlacement->m_hWnd, _wpf );
				}
				// win xp fix - end
				bool bPresetWP =
					( pFrameWp != NULL ) ? true : false;
				state.Serialize( ar );
				((InternalFriendlyFrameWnd *)pFrame)->SetDockState(
					state,
					bPresetWP
					);
			} // else from if( ar.IsStoring() )

			if( pFrameWp != NULL )
				__EXT_MFC_MEMCPY(
					pFrameWp,
					sizeof(WINDOWPLACEMENT),
					&state.m_wp,
					sizeof(WINDOWPLACEMENT)
					);

			bool bStore = ar.IsStoring() ? true : false;
			POSITION pos = pFrame->m_listControlBars.GetHeadPosition();
			for( ; pos != NULL; )
			{
				CControlBar * pBar = (CControlBar *)
					pFrame->m_listControlBars.GetNext( pos );
				ASSERT_VALID( pBar );
				ASSERT_KINDOF( CControlBar, pBar );
				CExtControlBar * pExtBar =
					DYNAMIC_DOWNCAST(
						CExtControlBar,
						pBar
						);
				if( pExtBar != NULL )
				{
					if(		( pExtBar->IsFixedMode() )
						||	( pExtBar->IsFixedDockStyle() )
						)
					{
						if( ! bSerializeFixedBarsState )
							continue;
					} // if( pExtBar->IsFixedMode() )
					else
					{
						if( ! bSerializeResizableBarsState )
							continue;
					} // else from if( pExtBar->IsFixedMode() )
					pExtBar->OnExtBarStateSerializationComplete(
						bStore
						);
				} // if( pExtBar != NULL )
			} // for( ; pos != NULL; )

			// almost OK
			bRetVal = true;
		} // try
		catch( CException * pXept )
		{
			if( bEnableThrowExceptions )
				throw;
			pXept->Delete();
			//ASSERT( FALSE );
		} // catch( CException * pXept )
		catch( ... )
		{
			//ASSERT( FALSE );
			if( bEnableThrowExceptions )
				throw;
		} // catch( ... )
		//ASSERT( bRetVal );
	} // try
	catch( ... )
	{
		ASSERT( ! bRetVal );
		if( ! bEnableThrowExceptions )
		{
			VERIFY( ProfileBarStateFramePop( pFrame ) );
		}
	} // catch( ... )
	//ASSERT( bRetVal );

dwGeneralFlags;
dwReserved;

	if( ar.IsLoading() )
		CExtControlBar::stat_AdjustOccCompatibilityTree( pFrame );
	VERIFY( ProfileBarStateFramePop( pFrame ) );

	return bRetVal;
}

LRESULT CExtControlBar::OnSetText(WPARAM wParam, LPARAM lParam)
{
    wParam;
LRESULT lResult = CWnd::Default();
	if(	IsFloating() )
	{
		CFrameWnd * pFrame = GetParentFrame();
		if(		pFrame != NULL
			&&	pFrame->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd))
			)
		{
			m_pDockBar->SetWindowText( (LPTSTR) lParam );
			pFrame->SetWindowText( (LPTSTR) lParam );
			pFrame->DelayRecalcLayout();
			pFrame->SendMessage( WM_NCPAINT );
		}
	}

bool bTextOnGripper = false;
	if( IsBarWithGripper( NULL, &bTextOnGripper ) )
	{
		if( bTextOnGripper )
			SendMessage( WM_NCPAINT );
	}
	return lResult;
}

static void stat_AddUnique(
	ExtControlBarVector_t & vBars,
	CExtControlBar * pBar
	)
{
	ASSERT( pBar != NULL );
INT nCount = (INT)vBars.GetSize();
	for( INT nBar = 0; nBar < nCount; nBar++ )
	{
		CExtControlBar * pAddedBar = vBars[ nBar ];
		ASSERT( pAddedBar != NULL );
		if( pAddedBar == pBar )
			return;
	}
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( pBar->m_pDockBar != NULL )
	{
		if( pBar->m_pDockBar->IsKindOf( RUNTIME_CLASS(CExtDockDynTabBar) ) )
		{
			CExtDynTabControlBar * pTabbedBar =
				STATIC_DOWNCAST(
					CExtDynTabControlBar,
					pBar->m_pDockBar->GetParent()
					);
			stat_AddUnique( vBars, pTabbedBar );
			return;
		}
	}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	vBars.Add( pBar );
}

void CExtControlBar::_GetFrameControlBars(
	CControlBar * pBarExcludeSubTree,
	CFrameWnd * pFrame,
	ExtControlBarVector_t & vBars,
	DWORD dwCallOpt, // = ((DWORD)__GFECB_ALL_NONRESTRICT)
	CPoint ptRestrict // = CPoint( -1000, -1000 )
	)
{
	ASSERT( pFrame != NULL );
	ASSERT( pFrame->GetSafeHwnd() != NULL );
	ASSERT_VALID( pFrame );
	vBars.RemoveAll();
	if( ( dwCallOpt & 
			( DWORD(__GFECB_FIXED) | DWORD(__GFECB_NONFIXED) )
			) == 0
		)
		return; // empty query
	if( ( dwCallOpt & 
			( DWORD(__GFECB_BAR_VISIBLE) | DWORD(__GFECB_BAR_INVISIBLE) )
			) == 0
		)
		return; // empty query
	if( ( dwCallOpt & 
			( DWORD(__GFECB_WND_VISIBLE) | DWORD(__GFECB_WND_INVISIBLE) )
			) == 0
		)
		return; // empty query
//CSingleLock _slCsCB( &g_csCB );
//	_slCsCB.Lock();
int nAllBarsCount = (int)g_AllBars.GetSize();
	for( INT nBar = 0; nBar < nAllBarsCount; nBar++ )
	{
		CExtControlBar * pBar = g_AllBars[nBar];
		ASSERT_KINDOF( CExtControlBar, pBar );
		if( pBar->GetSafeHwnd() == NULL )
			continue;
		if(		pBarExcludeSubTree != NULL
			&&	(	pBar == pBarExcludeSubTree
				||	::IsChild( pBarExcludeSubTree->m_hWnd, pBar->m_hWnd )
				)
			)
		{
			continue;
		}

		if( pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar))
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			&& ( !pBar->IsKindOf(RUNTIME_CLASS(CExtDynTabControlBar)) )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			//&&	pBarExcludeSubTree == NULL
			)
			continue;
		if( pBar->m_bPresubclassDialogMode )
			continue;
		CFrameWnd * pBarFrame =
			_GetDockingFrameImpl( pBar );
		ASSERT( pBarFrame != NULL );
		ASSERT( pBarFrame->GetSafeHwnd() != NULL );
		ASSERT_VALID( pBarFrame );
		if( pBarFrame != pFrame )
			continue;
		if( dwCallOpt == ((DWORD)__GFECB_ALL_NONRESTRICT) )
		{
			//stat_AddUnique( vBars, pBar );
			vBars.Add( pBar );
			continue;
		}
		if( (dwCallOpt&((DWORD)__GFECB_FIXED)) == 0 )
		{
			if( pBar->IsFixedMode() )
				continue;
		}
		if( !pBar->IsFixedMode() )
		{
			if( (dwCallOpt&((DWORD)__GFECB_NONFIXED)) == 0 )
				continue;
			bool bMinOnRow = pBar->IsMinimizedOnRow();
			if( (dwCallOpt&((DWORD)__GFECB_NONFIXED_MIN)) == 0 )
			{
				if( bMinOnRow )
					continue;
			}
			if( (dwCallOpt&((DWORD)__GFECB_NONFIXED_NONMIN)) == 0 )
			{
				if( !bMinOnRow )
					continue;
			}
		}
		if( pBar->IsVisible() )
		{
			if( (dwCallOpt&((DWORD)__GFECB_BAR_VISIBLE)) == 0 )
				continue;
		}
		else
		{
			if( (dwCallOpt&((DWORD)__GFECB_BAR_INVISIBLE)) == 0 )
				continue;
		}
		if( pBar->IsWindowVisible() )
		{
			if( (dwCallOpt&((DWORD)__GFECB_WND_VISIBLE)) == 0 )
				continue;
		}
		else
		{
			if( (dwCallOpt&((DWORD)__GFECB_WND_INVISIBLE)) == 0 )
				continue;
		}

		if( pBar->IsFloating() )
		{
			if( (dwCallOpt&((DWORD)__GFECB_FLOATED)) == 0 )
				continue;
		} // if( pBar->IsFloating() )
		else
		{
			if( (dwCallOpt&((DWORD)__GFECB_DOCKED)) == 0 )
				continue;
			if( (dwCallOpt&((DWORD)__GFECB_DOCKED)) !=
					((DWORD)__GFECB_DOCKED)
				)
			{ // if not all docked bars
				UINT nDockBarID = pBar->GetSafeDockBarDlgCtrlID();
				switch( nDockBarID )
				{
				case AFX_IDW_DOCKBAR_TOP:
					if( (dwCallOpt&((DWORD)__GFECB_DOCKED_TOP)) == 0 )
						continue;
				break;
				case AFX_IDW_DOCKBAR_BOTTOM:
					if( (dwCallOpt&((DWORD)__GFECB_DOCKED_BOTTOM)) == 0 )
						continue;
				break;
				case AFX_IDW_DOCKBAR_LEFT:
					if( (dwCallOpt&((DWORD)__GFECB_DOCKED_LEFT)) == 0 )
						continue;
				break;
				case AFX_IDW_DOCKBAR_RIGHT:
					if( (dwCallOpt&((DWORD)__GFECB_DOCKED_RIGHT)) == 0 )
						continue;
				break;
#ifdef _DEBUG
				default:
					{
						ASSERT( FALSE );
					}
				break;
#endif // _DEBUG
				} // switch( nDockBarID )
			} // if not all docked bars
		} // else from if( pBar->IsFloating() )

		// restriction by point
		if( (dwCallOpt&((DWORD)__GFECB_BYPT_ALL)) != 0 )
		{ // if restriction by point
			CRect rcWnd;
			CWnd * pWndNcTest = pBar;
			if( pBar->IsFloating() )
			{
				pWndNcTest = pBar->GetDockingFrame();
				ASSERT( pWndNcTest != NULL );
				ASSERT( pWndNcTest->GetSafeHwnd() != NULL );
				ASSERT_VALID( pWndNcTest );
				ASSERT_KINDOF( CMiniDockFrameWnd, pWndNcTest );
			}
			pWndNcTest->GetWindowRect( &rcWnd );
			BOOL bInsideWndArea =
				rcWnd.PtInRect( ptRestrict );
			if( !bInsideWndArea )
				continue;
			
			if( (dwCallOpt&((DWORD)__GFECB_BYPT_ONLY_WND)) != 0 )
			{
				stat_AddUnique( vBars, pBar );
				//continue;
//				_slCsCB.Unlock();
				return;
			}
			
			if( (dwCallOpt&((DWORD)__GFECB_BYPT_NCGRIP)) != 0 )
			{
				CRect rcGrip = pBar->_RectGripGet();
				if( rcGrip.IsRectEmpty() )
					continue;
				CPoint ptWnd( ptRestrict );
				ptWnd -= rcWnd.TopLeft();
				if( ! rcGrip.PtInRect(ptWnd) )
					continue;
			}

			if( (dwCallOpt&((DWORD)__GFECB_BYPT_CLIENT)) != 0 )
			{
				CRect rcClient;
				pBar->GetClientRect( &rcClient );
				pBar->ClientToScreen( &rcClient );
				if( ! rcClient.PtInRect( ptRestrict ) )
					continue;
			}

			if( (dwCallOpt&((DWORD)__GFECB_BYPT_NC)) != 0 )
			{ // if any nc hit test restriction
				UINT nHitTest = (UINT)
					pWndNcTest->SendMessage(
						WM_NCHITTEST,
						WPARAM(0),
						MAKELPARAM(ptRestrict.x,ptRestrict.y)
						);
				if( (dwCallOpt&((DWORD)__GFECB_BYPT_HTLEFT)) != 0 )
				{
					if( nHitTest != HTLEFT )
						continue;
				}
				if( (dwCallOpt&((DWORD)__GFECB_BYPT_HTRIGHT)) != 0 )
				{
					if( nHitTest != HTRIGHT )
						continue;
				}
				if( (dwCallOpt&((DWORD)__GFECB_BYPT_HTTOP)) != 0 )
				{
					if( nHitTest != HTTOP )
						continue;
				}
				if( (dwCallOpt&((DWORD)__GFECB_BYPT_HTBOTTOM)) != 0 )
				{
					if( nHitTest != HTBOTTOM )
						continue;
				}

				if( (dwCallOpt&((DWORD)__GFECB_BYPT_HTTOPLEFT)) != 0 )
				{
					if( nHitTest != HTTOPLEFT )
						continue;
				}
				if( (dwCallOpt&((DWORD)__GFECB_BYPT_HTTOPRIGHT)) != 0 )
				{
					if( nHitTest != HTTOPRIGHT )
						continue;
				}
				if( (dwCallOpt&((DWORD)__GFECB_BYPT_HTBOTTOMLEFT)) != 0 )
				{
					if( nHitTest != HTBOTTOMLEFT )
						continue;
				}
				if( (dwCallOpt&((DWORD)__GFECB_BYPT_HTBOTTOMRIGHT)) != 0 )
				{
					if( nHitTest != HTBOTTOMRIGHT )
						continue;
				}

				if( (dwCallOpt&((DWORD)__GFECB_BYPT_HTNOWHERE)) != 0 )
				{
					if( nHitTest != HTNOWHERE )
						continue;
				}
				
				if( (dwCallOpt&((DWORD)__GFECB_BYPT_HTSYSMENU)) != 0 )
				{
					if( nHitTest != HTSYSMENU )
						continue;
				}
				
				if( (dwCallOpt&((DWORD)__GFECB_BYPT_HTGROWBOX)) != 0 )
				{
					if( nHitTest != HTGROWBOX )
						continue;
				}
				
				if( (dwCallOpt&((DWORD)__GFECB_BYPT_HTMINBUTTON)) != 0 )
				{
					if( nHitTest != HTMINBUTTON )
						continue;
				}
				
				if( (dwCallOpt&((DWORD)__GFECB_BYPT_HTMAXBUTTON)) != 0 )
				{
					if( nHitTest != HTMAXBUTTON )
						continue;
				}

				if( (dwCallOpt&((DWORD)__GFECB_BYPT_HTCLIENT)) != 0 )
				{
					if( nHitTest != HTCLIENT )
						continue;
				}
				
			} // if any nc hit test restriction
		} // if restriction by point

		stat_AddUnique( vBars, pBar );
	} // for( INT nBar = 0; nBar < nAllBarsCount; nBar++ )

//	_slCsCB.Unlock();
}

void CExtControlBar::_GetFrameControlBars(
	bool bExcludeOwnSubtree,
	ExtControlBarVector_t & vBars,
	DWORD dwCallOpt, // = ((DWORD)__GFECB_ALL_NONRESTRICT)
	CPoint ptRestrict // = CPoint( -1000, -1000 )
	) const
{
CFrameWnd * pFrame = _GetDockingFrameImpl();
	_GetFrameControlBars(
		bExcludeOwnSubtree ? ((CExtControlBar*)this) : NULL,
		pFrame,
		vBars,
		dwCallOpt,
		ptRestrict
		);
//	if( bExcludeOwnSubtree )
//	{
//		int nCount = vBars.GetSize();
//		for( int i = 0; i < nCount; )
//		{
//			CExtControlBar * pBar = vBars[ i ];
//			if(		pBar == this
//				||	::IsChild( m_hWnd, pBar->m_hWnd )
//				)
//			{
//				vBars.RemoveAt( i );
//				nCount --;
//				continue;
//			}
//			i++;
//		} // for( int i = 0; i < nCount; )
//	} // if( bExcludeOwnSubtree )
}

CControlBar * CExtControlBar::_GetNextControlBarInRow(
	CControlBar * pBar,
	bool bNext, // = true // next or previous
	bool bExtBar, // = false
	bool bNonFixed // = false
	)
{
	ASSERT( pBar != NULL );
	ASSERT( pBar->GetSafeHwnd() != NULL );
	ASSERT_VALID( pBar );
	ASSERT( !pBar->IsFloating() );
	ASSERT_VALID( pBar->m_pDockBar );
INT nPos = pBar->m_pDockBar->FindBar( pBar );
	ASSERT( nPos >= 0 );
	if( nPos == 0 && !bNext )
		return NULL; // first in first row
INT nCountOfSideBars = (INT)pBar->m_pDockBar->m_arrBars.GetSize();
	ASSERT( nPos < nCountOfSideBars );
	if( nPos == (nCountOfSideBars-1) && bNext )
		return NULL; // last in last row
	if( bNext )
	{
		for( nPos++; nPos < nCountOfSideBars; nPos++ )
		{
			CControlBar * pExamineBar = (CControlBar *)
				pBar->m_pDockBar->m_arrBars[ nPos ];
			ASSERT( pExamineBar != pBar );
			if( pExamineBar == NULL )
				return NULL; // row separator
			if( __PLACEHODLER_BAR_PTR(pExamineBar) )
				continue;
			if( bExtBar )
			{
				if( !pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
					continue;
				if( bNonFixed )
				{
					if( ((CExtControlBar *)pBar)->IsFixedMode() )
						continue;
				}
			}
			return pExamineBar;
		} // for( nPos++; nPos < nCountOfSideBars; nPos++ )
	} // if( bNext )
	else
	{
		for( nPos--; nPos >= 0; nPos-- )
		{
			CControlBar * pExamineBar = (CControlBar *)
				pBar->m_pDockBar->m_arrBars[ nPos ];
			ASSERT( pExamineBar != pBar );
			if( pExamineBar == NULL )
				return NULL; // row separator
			if( __PLACEHODLER_BAR_PTR(pExamineBar) )
				continue;
			if( bExtBar )
			{
				if( !pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
					continue;
				if( bNonFixed )
				{
					if( ((CExtControlBar *)pBar)->IsFixedMode() )
						continue;
				}
			}
			return pExamineBar;
		} // for( nPos--; nPos >= 0; nPos-- )
	} // else from if( bNext )
	return NULL;
}

CControlBar * CExtControlBar::_GetFirstControlBarInRow(
	CControlBar * pBar,
	bool bNext, // = false // last or first
	bool bExtBar, // = false
	bool bNonFixed // = false
	)
{
	ASSERT( pBar != NULL );
	ASSERT( pBar->GetSafeHwnd() != NULL );
	ASSERT_VALID( pBar );
	ASSERT( !pBar->IsFloating() );
	ASSERT_VALID( pBar->m_pDockBar );
INT nPos = pBar->m_pDockBar->FindBar( pBar );
	ASSERT( nPos > 0 );
	if( nPos == 1 && !bNext )
	{
		// first in first row
		if( bExtBar )
		{
			if( !pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
			{
				CControlBar * pBar2 =
					_GetNextControlBarInRow(
						pBar,true,bExtBar,bNonFixed );
				return ( pBar2 == NULL ) ? pBar : pBar2;
			}
			if( bNonFixed
				&& ((CExtControlBar *)pBar)->IsFixedMode()
				)
			{
				CControlBar * pBar2 =
					_GetNextControlBarInRow(
						pBar,true,bExtBar,bNonFixed );
				return ( pBar2 == NULL ) ? pBar : pBar2;
			}
		}
		return pBar;
	} // if( nPos == 1 && !bNext )
INT nCountOfSideBars = (INT)pBar->m_pDockBar->m_arrBars.GetSize();
	ASSERT( nPos <= nCountOfSideBars );
	if( nPos == (nCountOfSideBars-1) && bNext )
	{
		// last in last row
		if( bExtBar )
		{
			if( !pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
			{
				CControlBar * pBar2 =
					_GetNextControlBarInRow(
						pBar,false,bExtBar,bNonFixed );
				return ( pBar2 == NULL ) ? pBar : pBar2;
			}
			if( bNonFixed
				&& ((CExtControlBar *)pBar)->IsFixedMode()
				)
			{
				CControlBar * pBar2 =
					_GetNextControlBarInRow(
						pBar,false,bExtBar,bNonFixed );
				return ( pBar2 == NULL ) ? pBar : pBar2;
			}
		}
		return pBar;
	}
CControlBar * pPrevBar = pBar;
	if( bNext )
	{
		for( nPos++; nPos < nCountOfSideBars; nPos++ )
		{
			CControlBar * pExamineBar = (CControlBar *)
				pBar->m_pDockBar->m_arrBars[ nPos ];
			ASSERT( pExamineBar != pBar );
			if( pExamineBar == NULL )
				return pPrevBar; // row separator
			if( __PLACEHODLER_BAR_PTR(pExamineBar) )
				continue;
			if( bExtBar )
			{
				if( !pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
					continue;
				if( bNonFixed )
				{
					if( ((CExtControlBar *)pBar)->IsFixedMode() )
						continue;
				}
			}
			pPrevBar = pExamineBar;
		} // for( nPos++; nPos < nCountOfSideBars; nPos++ )
	} // if( bNext )
	else
	{
		for( nPos--; nPos >= 0; nPos-- )
		{
			CControlBar * pExamineBar = (CControlBar *)
				pBar->m_pDockBar->m_arrBars[ nPos ];
			ASSERT( pExamineBar != pBar );
			if( pExamineBar == NULL )
				return pPrevBar; // row separator
			if( __PLACEHODLER_BAR_PTR(pExamineBar) )
				continue;
			if( bExtBar )
			{
				if( !pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
					continue;
				if( bNonFixed )
				{
					if( ((CExtControlBar *)pBar)->IsFixedMode() )
						continue;
				}
			}
			pPrevBar = pExamineBar;
		} // for( nPos--; nPos >= 0; nPos-- )
	} // else from if( bNext )
	return pPrevBar;
}

void CExtControlBar::_GetNextRowBars(
	bool bNextRow,
	MfcControlBarVector_t & vBars,
	CRect * pRcAllBars, // = NULL
	INT * p_nOwnPos, // = NULL
	bool * p_bRowMaxSized, // = NULL // row with resizable bars?
	bool * p_bRowFullSized // = NULL // row with menu-like bar?
	) const
{
	ASSERT_VALID( this );
	if( pRcAllBars != NULL )
		pRcAllBars->SetRect( 0, 0, 0, 0 );
	if( p_nOwnPos != NULL )
		*p_nOwnPos = -1;
	if( p_bRowMaxSized != NULL )
		*p_bRowMaxSized = false;
	if( p_bRowFullSized != NULL )
		*p_bRowFullSized = false;
    vBars.RemoveAll();
	if( m_pDockBar == NULL )
		return;
	ASSERT_VALID( m_pDockBar );
INT nOwnIdx = m_pDockBar->FindBar( (CControlBar *)this );
	ASSERT( nOwnIdx >= 0 );
	if( nOwnIdx < 0 )
		return;
	if( p_nOwnPos != NULL )
		*p_nOwnPos = nOwnIdx;
INT nTotalDockCount = (INT)m_pDockBar->m_arrBars.GetSize();
INT nFirstToReview = -1;
INT nIncrement = bNextRow ? 1 : -1;
INT nBar = 0;
	for( nBar = nOwnIdx+nIncrement; true; nBar += nIncrement )
	{
		if( nBar == 0 || nBar == nTotalDockCount )
			return;
		CControlBar * pBar = (CControlBar *)
			m_pDockBar->m_arrBars[ nBar ];
		if( pBar == NULL )
		{
			nFirstToReview = nBar + nIncrement;
			if( nFirstToReview >= nTotalDockCount )
				return;
			break;
		}
#ifdef _DEBUG
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CControlBar, pBar );
#endif // _DEBUG
	} // for( nBar = nOwnIdx+nIncrement; true; nBar += nIncrement )
	
	if( nFirstToReview <= 0 )
		return;
//	ASSERT( 0 < nFirstToReview && nFirstToReview < nTotalDockCount );
	ASSERT( 0 <= nFirstToReview && nFirstToReview < nTotalDockCount );

INT nBarWidth = 0;
bool bHorz = IsDockedHorizontally();

	for( nBar = nFirstToReview; true; nBar += nIncrement )
	{
		if( nBar < 0 || nBar >= nTotalDockCount )
			break;
		CControlBar * pBar = (CControlBar *)
			m_pDockBar->m_arrBars[ nBar ];
		if( pBar == NULL )
		{
			if( vBars.GetSize() == 0 )
			{
				nBarWidth = 0;
				if( pRcAllBars != NULL )
					pRcAllBars->SetRect( 0, 0, 0, 0 );
				if( p_bRowMaxSized != NULL )
					*p_bRowMaxSized = false;
				if( p_bRowFullSized != NULL )
					*p_bRowFullSized = false;
				continue; // skip invisible or placeholder's row
			}
			break;
		}
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CControlBar, pBar );
		ASSERT( pBar != (CControlBar *)this );

		if( !pBar->IsVisible() )
			continue;
		
		if( pRcAllBars != NULL )
		{
			if( vBars.GetSize() == 0 )
				pBar->GetWindowRect( pRcAllBars );
			else
			{
				CRect rcWndBar;
				pBar->GetWindowRect( &rcWndBar );
				pRcAllBars->left =
					min( pRcAllBars->left, rcWndBar.left );
				pRcAllBars->top =
					min( pRcAllBars->top, rcWndBar.top );
				pRcAllBars->right =
					max( pRcAllBars->right, rcWndBar.right );
				pRcAllBars->bottom =
					max( pRcAllBars->bottom, rcWndBar.bottom );
			}
		} // if( pRcAllBars != NULL )

		if( (p_bRowMaxSized != NULL || p_bRowFullSized != NULL)
			&& pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
			)
		{
			if( p_bRowMaxSized != NULL
				&& ( ! (*p_bRowMaxSized) )
				&& !((CExtControlBar*)pBar)->IsFixedMode()
				)
				*p_bRowMaxSized = true;
			if(	p_bRowFullSized != NULL
				&& ( ! (*p_bRowFullSized) )
				&& pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
				&& ((CExtControlBar *)pBar)->_GetFullRowMode()
				)
				*p_bRowFullSized = true;
			if( p_bRowFullSized != NULL
				&& ( ! (*p_bRowFullSized) )
				)
			{
				CRect rcBarWnd;
				pBar->GetWindowRect( &rcBarWnd );
				nBarWidth +=
					bHorz ? rcBarWnd.Width() : rcBarWnd.Height();
			}
		}
		vBars.Add( pBar );
	} // for( nBar = nFirstToReview; true; nBar += nIncrement )

	if( p_bRowFullSized != NULL
		&& ( ! (*p_bRowFullSized) )
		)
	{
		ASSERT_VALID( m_pDockBar );
		CRect rcDockBarWnd;
		m_pDockBar->GetWindowRect( &rcDockBarWnd );
		INT nDockBarWidth = bHorz ? rcDockBarWnd.Width() : rcDockBarWnd.Height();
		nDockBarWidth --;
		if( nBarWidth >= nDockBarWidth )
			*p_bRowFullSized = true;
	}
}

void CExtControlBar::_GetRowBars(
	MfcControlBarVector_t & vBars,
	CRect * pRcAllBars, // = NULL
	INT * p_nOwnPos, // = NULL
	bool * p_bRowMaxSized, // = NULL // row with resizable bars?
	bool * p_bRowFullSized // = NULL // row with menu-like bar?
	) const
{
	ASSERT_VALID( this );
	if( pRcAllBars != NULL )
		pRcAllBars->SetRect( 0, 0, 0, 0 );
	if( p_nOwnPos != NULL )
		*p_nOwnPos = -1;
	if( p_bRowMaxSized != NULL )
		*p_bRowMaxSized = false;
	if( p_bRowFullSized != NULL )
		*p_bRowFullSized = false;
    vBars.RemoveAll();
bool bEmptyRowCalc = true;
	if( m_pDockBar == NULL )
		return;
	ASSERT_VALID( m_pDockBar );
INT nBarWidth = 0;
bool bHorz = IsDockedHorizontally();
INT nTotalDockCount = (INT)m_pDockBar->m_arrBars.GetSize();
bool bThisFound = false;
	for( INT nBar = 0; nBar < nTotalDockCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pDockBar->m_arrBars[ nBar ];
		if( pBar == NULL )
		{
			if( bThisFound )
				break;
			vBars.RemoveAll();
			bEmptyRowCalc = true;
			nBarWidth = 0;
			if( pRcAllBars != NULL )
				pRcAllBars->SetRect( 0, 0, 0, 0 );
			if( p_bRowMaxSized != NULL )
				*p_bRowMaxSized = false;
			if( p_bRowFullSized != NULL )
				*p_bRowFullSized = false;
			continue;
		}
		if( __PLACEHODLER_BAR_PTR( pBar ) )
			continue;
		ASSERT_KINDOF( CControlBar, pBar );
		
		if( pBar == (CControlBar *)this )
		{
			bThisFound = true;
			if( p_nOwnPos != NULL )
				*p_nOwnPos = (INT)vBars.GetSize();
		}

		if( !pBar->IsVisible() )
			continue;
		
		if( pRcAllBars != NULL )
		{
			if( bEmptyRowCalc )
			{
				bEmptyRowCalc = false;
				pBar->GetWindowRect( pRcAllBars );
			} // if( bEmptyRowCalc )
			else
			{
				CRect rcWndBar;
				pBar->GetWindowRect( &rcWndBar );
				pRcAllBars->left =
					min( pRcAllBars->left, rcWndBar.left );
				pRcAllBars->top =
					min( pRcAllBars->top, rcWndBar.top );
				pRcAllBars->right =
					max( pRcAllBars->right, rcWndBar.right );
				pRcAllBars->bottom =
					max( pRcAllBars->bottom, rcWndBar.bottom );
			} // else from if( bEmptyRowCalc )
		} // if( pRcAllBars != NULL )

		if( (p_bRowMaxSized != NULL || p_bRowFullSized != NULL)
			&& pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
			)
		{
			if( p_bRowMaxSized != NULL
				&& ( ! (*p_bRowMaxSized) )
				&& !((CExtControlBar*)pBar)->IsFixedMode()
				)
				*p_bRowMaxSized = true;
			if( p_bRowFullSized != NULL
				&& ( ! (*p_bRowFullSized) )
				&& pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
				&& ((CExtControlBar *)pBar)->_GetFullRowMode()
				)
				*p_bRowFullSized = true;
			if( p_bRowFullSized != NULL
				&& ( ! (*p_bRowFullSized) )
				)
			{
				CRect rcBarWnd;
				pBar->GetWindowRect( &rcBarWnd );
				nBarWidth +=
					bHorz ? rcBarWnd.Width() : rcBarWnd.Height();
			}
		}

		vBars.Add( pBar );
	} // for( INT nBar = 0; nBar < nTotalDockCount; nBar++ )

	if( p_bRowFullSized != NULL
		&& ( ! (*p_bRowFullSized) )
		)
	{
		ASSERT_VALID( m_pDockBar );
		CRect rcDockBarWnd;
		m_pDockBar->GetWindowRect( &rcDockBarWnd );
		INT nDockBarWidth = bHorz ? rcDockBarWnd.Width() : rcDockBarWnd.Height();
		nDockBarWidth --;
		if( nBarWidth >= nDockBarWidth )
			*p_bRowFullSized = true;
	}

#ifdef _DEBUG
	if( p_nOwnPos != NULL )
	{
		INT nCount = (INT)vBars.GetSize();
		ASSERT( nCount > 0 );
		ASSERT( 0 <= (*p_nOwnPos) && (*p_nOwnPos) < nCount );
		for( INT nBar = 0; nBar < nTotalDockCount; nBar++ )
		{
			CControlBar * pBar = (CControlBar *)
				m_pDockBar->m_arrBars[ nBar ];
			pBar;
		}
	}
#endif // _DEBUG
}

void CExtControlBar::_GetRowExtBars(
	ExtControlBarVector_t & vBars,
	bool bNonFixedOnly, // = true
	CRect * pRcAllBars, // = NULL
	INT * p_nOwnPos, // = NULL
	bool * p_bRowMaxSized, // = NULL // row with resizable bars?
	bool * p_bRowFullSized // = NULL // row with menu-like bar?
	) const
{
	ASSERT_VALID( this );
	if( pRcAllBars != NULL )
		pRcAllBars->SetRect( 0, 0, 0, 0 );
	if( p_nOwnPos != NULL )
		*p_nOwnPos = -1;
	if( p_bRowMaxSized != NULL )
		*p_bRowMaxSized = false;
	if( p_bRowFullSized != NULL )
		*p_bRowFullSized = false;
    vBars.RemoveAll();
bool bEmptyRowCalc = true;
	if( m_pDockBar == NULL )
		return;
	ASSERT_VALID( m_pDockBar );
INT nBarWidth = 0;
bool bHorz = IsDockedHorizontally();
INT nTotalDockCount = (INT)m_pDockBar->m_arrBars.GetSize();
bool bThisFound = false;
	for( INT nBar = 0; nBar < nTotalDockCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pDockBar->m_arrBars[ nBar ];
		if( pBar == NULL )
		{
			if( bThisFound )
				break;
			vBars.RemoveAll();
			bEmptyRowCalc = true;
			nBarWidth = 0;
			if( pRcAllBars != NULL )
				pRcAllBars->SetRect( 0, 0, 0, 0 );
			if( p_bRowMaxSized != NULL )
				*p_bRowMaxSized = false;
			if( p_bRowFullSized != NULL )
				*p_bRowFullSized = false;
			continue;
		}
		if( __PLACEHODLER_BAR_PTR( pBar ) )
			continue;
		ASSERT_KINDOF( CControlBar, pBar );

		if( pBar == (CControlBar *)this )
		{
			bThisFound = true;
			if( p_nOwnPos != NULL )
				*p_nOwnPos = (INT)vBars.GetSize();
		}

		if( pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
		{
			if( ( !((CExtControlBar *)pBar)->IsFixedMode() )
				|| (! bNonFixedOnly )
				)
				vBars.Add( (CExtControlBar *)pBar );
		
			if( p_bRowMaxSized != NULL || p_bRowFullSized != NULL )
			{
				if( p_bRowMaxSized != NULL
					&& ( ! (*p_bRowMaxSized) )
					&& !((CExtControlBar*)pBar)->IsFixedMode()
					)
					*p_bRowMaxSized = true;
				if( p_bRowFullSized != NULL
					&& ( ! (*p_bRowFullSized) )
					&& pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
					&& ((CExtControlBar *)pBar)->_GetFullRowMode()
					)
					*p_bRowFullSized = true;
			}
		}
		if( p_bRowFullSized != NULL
			&& ( ! (*p_bRowFullSized) )
			)
		{
			CRect rcBarWnd;
			pBar->GetWindowRect( &rcBarWnd );
			nBarWidth +=
				bHorz ? rcBarWnd.Width() : rcBarWnd.Height();
		}

		if( pRcAllBars != NULL )
		{
			if( bEmptyRowCalc )
			{
				bEmptyRowCalc = false;
				pBar->GetWindowRect( pRcAllBars );
			} // if( bEmptyRowCalc )
			else
			{
				CRect rcWndBar;
				pBar->GetWindowRect( &rcWndBar );
				pRcAllBars->left =
					min( pRcAllBars->left, rcWndBar.left );
				pRcAllBars->top =
					min( pRcAllBars->top, rcWndBar.top );
				pRcAllBars->right =
					max( pRcAllBars->right, rcWndBar.right );
				pRcAllBars->bottom =
					max( pRcAllBars->bottom, rcWndBar.bottom );
			} // if( bEmptyRowCalc )
		} // if( pRcAllBars != NULL )
	} // for( INT nBar = 0; nBar < nTotalDockCount; nBar++ )

	if( p_bRowFullSized != NULL
		&& ( ! (*p_bRowFullSized) )
		)
	{
		ASSERT_VALID( m_pDockBar );
		CRect rcDockBarWnd;
		m_pDockBar->GetWindowRect( &rcDockBarWnd );
		INT nDockBarWidth = bHorz ? rcDockBarWnd.Width() : rcDockBarWnd.Height();
		nDockBarWidth --;
		if( nBarWidth >= nDockBarWidth )
			*p_bRowFullSized = true;
	}

#ifdef _DEBUG
	if( p_nOwnPos != NULL )
	{
		INT nCount = (INT)vBars.GetSize();
		ASSERT( nCount > 0 );
		ASSERT( 0 <= (*p_nOwnPos) && (*p_nOwnPos) < nCount );
	}
#endif // _DEBUG
}

bool CExtControlBar::_IsFirstBarInRow( bool bExtBar ) const
{
	if( m_pDockBar == NULL )
		return false;
	ASSERT_VALID( m_pDockBar );
INT nRowCount = 0;
INT nTotalDockCount = (INT)m_pDockBar->m_arrBars.GetSize();
	for( INT nBar = 0; nBar < nTotalDockCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pDockBar->m_arrBars[ nBar ];
		if( pBar == NULL )
		{
			nRowCount = 0;
			continue;
		}
		if( __PLACEHODLER_BAR_PTR( pBar ) )
			continue;
		ASSERT_KINDOF( CControlBar, pBar );
		if( bExtBar )
		{
			CExtControlBar * pExtBar = 
				DYNAMIC_DOWNCAST( CExtControlBar, pBar );
			if( pExtBar == NULL )
				return false;
			if( pExtBar->IsFixedMode() )
				return false;
		}
		if( pBar == (CControlBar *)this )
		{
			return (nRowCount == 0) ? true : false;
		}
		nRowCount++;
	} // for( INT nBar = 0; nBar < nTotalDockCount; nBar++ )
	ASSERT( FALSE );
	return false;
}

bool CExtControlBar::_IsLastBarInRow( bool bExtBar ) const
{
	ASSERT( !IsFloating() );
	if( bExtBar )
	{
		ExtControlBarVector_t vBars;
		_GetRowExtBars( vBars );
		INT nCountOfBars = (INT)vBars.GetSize();
		ASSERT( nCountOfBars > 0 );
		if( vBars[nCountOfBars-1] == (CExtControlBar *)this )
			return true;
		return false;
	}
MfcControlBarVector_t vBars;
    _GetRowBars( vBars );
INT nCountOfBars = (INT)vBars.GetSize();
	ASSERT( nCountOfBars > 0 );
	if( vBars[nCountOfBars-1] == (CControlBar *)this )
		return true;
	return false;
}

CFrameWnd * CExtControlBar::_GetDockingFrameImpl(
	CControlBar * pBar
	)
{
	ASSERT_VALID( pBar );
	ASSERT( pBar->GetSafeHwnd() != NULL );

	if( pBar->m_pDockSite != NULL )
	{
		ASSERT_VALID( pBar->m_pDockSite );
		ASSERT( pBar->m_pDockSite->GetSafeHwnd() != NULL );
		ASSERT_KINDOF( CFrameWnd, pBar->m_pDockSite );
		return pBar->m_pDockSite;
	}

CFrameWnd * pFrame = pBar->GetParentFrame();
	if( pFrame == NULL )
		return NULL;

	ASSERT_VALID( pFrame );
	ASSERT_KINDOF( CFrameWnd, pFrame );
	if( pFrame->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)) )
	{
		pFrame = pFrame->GetParentFrame();
		ASSERT_VALID( pFrame );
		ASSERT( !pFrame->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)) );
	}
	return pFrame;
}

CFrameWnd * CExtControlBar::_GetDockingFrameImpl() const
{
	return _GetDockingFrameImpl( (CControlBar *)this );
}

UINT CExtControlBar::GetSafeDockBarDlgCtrlID() const
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
CWnd * pParentWnd = ( m_pDockBar != NULL ) ? m_pDockBar : GetParent();
	if(		pParentWnd->GetSafeHwnd() == NULL
		|| (! pParentWnd->IsKindOf( RUNTIME_CLASS(CDockBar) ) )
		)
		return AFX_IDW_DOCKBAR_FLOAT; // IDC_STATIC

UINT nDlgCtrlID = pParentWnd->GetDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID( nDlgCtrlID );
	return nDlgCtrlID;
}

bool CExtControlBar::IsDockedAtTop() const
{
	if( m_pDockSite == NULL )
		return (m_dwStyle & CBRS_TOP) ? true : false;
	return GetSafeDockBarDlgCtrlID() == AFX_IDW_DOCKBAR_TOP;
}

bool CExtControlBar::IsDockedAtBottom() const
{
	if( m_pDockSite == NULL )
		return (m_dwStyle & CBRS_BOTTOM) ? true : false;
	return GetSafeDockBarDlgCtrlID() == AFX_IDW_DOCKBAR_BOTTOM;
}

bool CExtControlBar::IsDockedAtLeft() const
{
	if( m_pDockSite == NULL )
		return (m_dwStyle & CBRS_LEFT) ? true : false;
	return GetSafeDockBarDlgCtrlID() == AFX_IDW_DOCKBAR_LEFT;
}

bool CExtControlBar::IsDockedAtRight() const
{
	if( m_pDockSite == NULL )
		return (m_dwStyle & CBRS_RIGHT) ? true : false;
	return GetSafeDockBarDlgCtrlID() == AFX_IDW_DOCKBAR_RIGHT;
}

bool CExtControlBar::IsDockedHorizontally() const
{
	if( m_pDockSite == NULL )
		return (m_dwStyle & (CBRS_TOP|CBRS_BOTTOM)) ? true : false;
UINT nDockBarDlgCtrlID = GetSafeDockBarDlgCtrlID();
	return	nDockBarDlgCtrlID == AFX_IDW_DOCKBAR_TOP
		||	nDockBarDlgCtrlID == AFX_IDW_DOCKBAR_BOTTOM;
}

bool CExtControlBar::IsDockedVertically() const
{
	if( m_pDockSite == NULL )
		return (m_dwStyle & (CBRS_LEFT|CBRS_RIGHT)) ? true : false;
UINT nDockBarDlgCtrlID = GetSafeDockBarDlgCtrlID();
	return	nDockBarDlgCtrlID == AFX_IDW_DOCKBAR_LEFT
		||	nDockBarDlgCtrlID == AFX_IDW_DOCKBAR_RIGHT;
}

bool CExtControlBar::IsDocked() const
{
	if( m_pDockSite == NULL )
		return true;
UINT nDockBarDlgCtrlID = GetSafeDockBarDlgCtrlID();
	return	nDockBarDlgCtrlID == AFX_IDW_DOCKBAR_TOP
		||	nDockBarDlgCtrlID == AFX_IDW_DOCKBAR_BOTTOM
		||	nDockBarDlgCtrlID == AFX_IDW_DOCKBAR_LEFT
		||	nDockBarDlgCtrlID == AFX_IDW_DOCKBAR_RIGHT;
}

bool CExtControlBar::IsFloating() const
{
	if( !IsDocked() )
	{
		if( GetParentFrame() != NULL )
			return true;
	}
	return false;
}

bool CExtControlBar::_IsSingleVisibleInFloatingPalette()
{
	ASSERT_VALID( this );
	if(		m_pDockSite == NULL
		||	(! IsVisible() )
		||	IsFixedMode()
//		||	m_bUpdatingChain
		)
		return false;
CFrameWnd * pFrame = GetParentFrame();
	if( pFrame == NULL )
		return false;
	if( ! pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
		return false;
	ASSERT_KINDOF( CExtMiniDockFrameWnd, pFrame );
	ASSERT( m_pDockSite != pFrame );
POSITION pos = m_pDockSite->m_listControlBars.GetHeadPosition();
	for( ; pos != NULL; )
	{
		CExtControlBar * pBar =
			DYNAMIC_DOWNCAST(
				CExtControlBar,
				((CControlBar *)(m_pDockSite->m_listControlBars.GetNext(pos)))
				);
		if(		pBar == NULL
			||	pBar == this
			||	pBar->m_pDockSite == NULL
			||	pBar->m_pDockBar == NULL
			||	pBar->IsFixedMode()
			||	(	pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar))
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
					&& !pBar->IsKindOf(RUNTIME_CLASS(CExtDynTabControlBar))
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
				)
			||	pBar->GetParentFrame() != pFrame
			)
			continue;
		HWND hWndDockBar = pBar->m_pDockBar->GetSafeHwnd();
		if(		hWndDockBar == NULL // by Guillaume Provost advice
			||	(! ::IsWindow(hWndDockBar) )
			)
			continue;
		ASSERT( pBar->m_pDockBar != NULL );
		if( 
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynTabBar))
			||
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			(! pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar) ) )
			)
			continue;
		if( pBar->IsVisible() )
			return false;
	}
	return true;
}

bool CExtControlBar::IsBarWithGripper(
	bool * pbGripperAtTop, // = NULL
	bool * pbTextOnGripper // = NULL
	) const
{
	ASSERT_VALID( this );
	if( pbGripperAtTop != NULL )
		*pbGripperAtTop = false;
	if( pbTextOnGripper != NULL )
		*pbTextOnGripper = false;
	if( GetSafeHwnd() == NULL )
		return false;
	if(		(m_dwStyle & CBRS_GRIPPER) == 0
		||	IsFloating()
		)
		return false;


CFrameWnd * pParentFrame = GetParentFrame();
	if(		pParentFrame != NULL
		&&	pParentFrame->IsKindOf(RUNTIME_CLASS(CExtMiniDockFrameWnd))
		)
	{
		if( ((CExtControlBar *)this)->_IsSingleVisibleInFloatingPalette() )
			return false;
// 		if( ((CExtMiniDockFrameWnd*)pParentFrame)->_GetSingleVisibleBar() != NULL )
// 			return false;
	}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( m_pDockBar != NULL
		&& !AutoHideModeGet()
		)
	{
		if( m_pDockBar->IsKindOf( RUNTIME_CLASS(CExtDockDynTabBar) ) )
		{
			if( pbGripperAtTop != NULL )
				*pbGripperAtTop = false;
			if( pbTextOnGripper != NULL )
				*pbTextOnGripper = false;
			return false;
		}
	}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	if( pbGripperAtTop != NULL
		&&	( m_bGripperStaticallyAtTop || IsDockedVertically() )
		)
		*pbGripperAtTop = true;
	if( pbTextOnGripper != NULL )
		*pbTextOnGripper = true;
    return true;
}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

bool CExtControlBar::IsCloseOnlyOneBarInTabGroup() const
{
	ASSERT_VALID( this );
	return g_bCloseOnlyOneBarInTabGroup;
}

int CExtControlBar::OnReflectTabOleDragDrop(
	COleDropTarget * pOleDropTarget
	)
{
	ASSERT_VALID( this );
	pOleDropTarget;
	return g_nTabDrarDropSelectionDelay;
}

#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

void CExtControlBar::OnShowControlBarChain(
	bool bShow,
	bool bDelay,
	bool bOnlyOneBar // = false
	)
{
	ASSERT_VALID( this );
	bOnlyOneBar;
CFrameWnd * pFrame = _GetDockingFrameImpl();
	ASSERT_VALID( pFrame );
	pFrame->ShowControlBar(
		this,
		bShow ? TRUE : FALSE,
		bDelay ? TRUE : FALSE
		);
}

CSize CExtControlBar::CalcFixedLayout(
	BOOL bStretch,
	BOOL bHorz
	)
{
	bStretch;
	return _CalcDesiredLayout( bHorz );
}

CSize CExtControlBar::CalcDynamicLayout(
	int nLength,
	DWORD nMode
	)
{
	// MSDN for LM_HORZDOCK and LM_VERTDOCK: ... return
	// horizontal/vertical docked dimensions ...

	if( ((nMode & (LM_HORZDOCK | LM_VERTDOCK)) == 0)
		|| IsFloating()
		)
	{ // if going to floated state or already floated
		
		if( nLength > 0 )
		{
			// MSDN for nLength: ... The requested dimension of the
			// control bar, either horizontal or vertical,
			// depending on dwMode ...

			// MSDN for LM_LENGTHY: ... Set if nLength indicates
			// height (Y-direction) instead of width.
			if( nMode & LM_LENGTHY ) 
				m_sizeFloated.cy = nLength;
			else
				m_sizeFloated.cx = nLength;
		} // if( nLength > 0 )

		// MSDN for LM_MRUWIDTH: ... Most Recently Used Dynamic Width.
		// Ignores nLength parameter and uses the remembered most
		// recently used width.

		// MSDN for LM_COMMIT: ... Resets LM_MRUWIDTH to current
		// width of floating control bar.
		if( nMode & ( LM_MRUWIDTH | LM_COMMIT ) )
			return m_sizeFloated;

		return _CalcDesiredLayout( nMode & LM_HORZ );

	} // if going to floated state or already floated

	if( nLength >= 0 )
		return _CalcDesiredLayout( nMode & LM_HORZ );

	ASSERT_VALID( m_pDockBar );
	if( ((CExtDockBar *)m_pDockBar)->m_bLayoutQuery )
		return _CalcDesiredLayout( nMode & LM_HORZ );

    ASSERT_VALID( m_pDockBar );
INT nCalcExtentMax = 
		((FriendlyDockBarHack*)m_pDockBar)->
			GetCalcExtentMax( nMode & LM_HORZ );
	ASSERT( nCalcExtentMax >= 0 );
	if( nCalcExtentMax == 0 )
		return _CalcDesiredLayout( nMode & LM_HORZ );
INT nCalcExtentDiff = 0;
INT nCalcExtentMin =
		(nMode & LM_HORZ) ? afxData.cyBorder2 : afxData.cxBorder2;
INT nCalcExtentReal = 0, nMaxWidth = 0;
INT nCountOfBarsAtSide = (INT)m_pDockBar->m_arrBars.GetSize();
	ASSERT( nCountOfBarsAtSide > 0 );
INT nThisIdx = -1, nReviewStart = -1, nReviewStop = -1;
INT nCountInRowReal = 0, nCountExtBarsInRowReal = 0;
INT nCountOfMinSizedBars = 0;
INT nBarAtSide = 0; 
	for( nBarAtSide = 0; nBarAtSide < nCountOfBarsAtSide ; nBarAtSide++ )
	{
		CControlBar * pBar =  (CControlBar *)
			m_pDockBar->m_arrBars[ nBarAtSide ];
		if( pBar == NULL )
		{
			if( nThisIdx >= 0 )
			{
				nReviewStop = nBarAtSide - 1;
				break;
			}
			nCountInRowReal = 0;
			nCountExtBarsInRowReal = 0;
			nMaxWidth = 0;
			nReviewStop = -1;
			nReviewStart = nBarAtSide + 1;
			continue;
		} // if( pBar == NULL )
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT( pBar->GetSafeHwnd() != NULL );
        if( ! pBar->IsVisible() )
			continue;
		if( pBar == this )
		{
			if( nCountExtBarsInRowReal > 0 )
				//goto __Align_Mutually_Row__;
				return _CalcDesiredLayout( nMode & LM_HORZ );
			nThisIdx = nBarAtSide;
			//nReviewStart = nThisIdx;
		}
		CRect rcBarWnd;
		pBar->GetWindowRect( &rcBarWnd );
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar!=NULL && pExtBar->IsFixedMode() )
			pExtBar = NULL;
//		INT nCalcExtentStep =
//			(nMode & LM_HORZ) ? rcBarWnd.Width() : rcBarWnd.Height();
		if( pExtBar != NULL )
		{
			nCountExtBarsInRowReal++;
			nMaxWidth =
				max(
					nMaxWidth,
					(nMode & LM_HORZ)
						? pExtBar->m_sizeDockedH.cy
						: pExtBar->m_sizeDockedV.cx
					);
		} // if( pExtBar != NULL )
		nCountInRowReal++;
	} // for( nBarAtSide = 0; nBarAtSide < nCountOfBarsAtSide ; nBarAtSide++ )

	ASSERT( nReviewStart <= nReviewStop );
	ASSERT( nReviewStart <= nThisIdx && nThisIdx <= nReviewStop );

	if( nCountInRowReal == 1 )
	{
        if( nMode & LM_HORZ )
            m_sizeDockedH.cx =
				max( _CalcDesiredMinHW(), nCalcExtentMax );
        else
            m_sizeDockedV.cy =
				max( _CalcDesiredMinVH(), nCalcExtentMax );
		goto __Align_Mutually_Row__;
	}

    for( nBarAtSide = nReviewStart; nBarAtSide <= nReviewStop; nBarAtSide++ )
    {
        CControlBar * pBar = (CControlBar*)
			m_pDockBar->m_arrBars[ nBarAtSide ];
        if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT( pBar->GetSafeHwnd() != NULL );
        if( ! pBar->IsVisible() )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar!=NULL && pExtBar->IsFixedMode() )
			pExtBar = NULL;

        INT nCalcExtentBar = 0;
        if( pExtBar == NULL )
        {
            CRect rcBar;
            pBar->GetWindowRect( &rcBar );
            nCalcExtentBar =
				(nMode & LM_HORZ) ?
					rcBar.Width() : rcBar.Height();
        } // if( pExtBar == NULL )
        else
		{
			nCalcExtentBar =
				(nMode & LM_HORZ)
					? pExtBar->_CalcDesiredMinHW()
					: pExtBar->_CalcDesiredMinVH()
					;
			LONG & ref_nCalcExtentBar =
				(nMode & LM_HORZ)
					? pExtBar->m_sizeDockedH.cx
					: pExtBar->m_sizeDockedV.cy
					;
			if( ref_nCalcExtentBar < nCalcExtentBar )
				ref_nCalcExtentBar = nCalcExtentBar;
		}
        nCalcExtentMin += nCalcExtentBar;
		if( nCalcExtentMin <= nCalcExtentMax )
		{
			if( pExtBar == NULL )
			{
				nCalcExtentMax -= nCalcExtentBar;
				continue;
			}
			nCalcExtentReal +=
				(nMode & LM_HORZ)
					? pExtBar->m_sizeDockedH.cx
					: pExtBar->m_sizeDockedV.cy ;
			continue;
		} // if( nCalcExtentMin <= nCalcExtentMax )
        
        if( nBarAtSide < nThisIdx )
        {
			ASSERT( pBar != this );
			((FriendlyDockBarHack*)m_pDockBar)->
				_InsertRowSplitter( nBarAtSide + 1 );
			return _CalcDesiredLayout( nMode & LM_HORZ );
        }
        if( nBarAtSide == nThisIdx )
        {
			ASSERT( pBar == this );
			if( nMode & LM_HORZ )
				m_sizeDockedH.cx =
					max( _CalcDesiredMinHW(), nCalcExtentMax );
			else
				m_sizeDockedV.cy =
					max( _CalcDesiredMinVH(), nCalcExtentMax );
			break;
        }
		ASSERT( pBar != this );
		((FriendlyDockBarHack*)m_pDockBar)->
			_InsertRowSplitter( nBarAtSide );
        nReviewStop = nBarAtSide - 1;
		ASSERT( nReviewStart <= nReviewStop );
		ASSERT( nReviewStart <= nThisIdx && nThisIdx <= nReviewStop );
        break;
    } // for( nBarAtSide = nReviewStart; nBarAtSide <= nReviewStop; nBarAtSide++ )

	nCountOfMinSizedBars = 0;
	for( nBarAtSide = nReviewStart; nBarAtSide <= nReviewStop; nBarAtSide++ )
	{
        CControlBar * pBar = (CControlBar*)
			m_pDockBar->m_arrBars[ nBarAtSide ];
        if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT( pBar->GetSafeHwnd() != NULL );
        if( !pBar->IsVisible() )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar!=NULL && pExtBar->IsFixedMode() )
			pExtBar = NULL;
		if( pExtBar == NULL )
			continue;
        LONG nMinMetric =
			(nMode & LM_HORZ)
				? pExtBar->_CalcDesiredMinHW()
				: pExtBar->_CalcDesiredMinVH()
				;
        LONG nCalcExtentBar =
			(nMode & LM_HORZ)
				? pExtBar->m_sizeDockedH.cx
				: pExtBar->m_sizeDockedV.cy
				;
		ASSERT( nCalcExtentBar >= nMinMetric );
		//
		if( nMinMetric == nCalcExtentBar )
			nCountOfMinSizedBars++;
	} // for( nBarAtSide = nReviewStart; nBarAtSide <= nReviewStop; nBarAtSide++ )
	ASSERT( nCountOfMinSizedBars <= nCountExtBarsInRowReal );

	nCalcExtentDiff = nCalcExtentMax - nCalcExtentReal;
	if( nCountOfMinSizedBars != nCountExtBarsInRowReal )
	{
//		INT nCalcStep =
//			nCalcExtentDiff /
//				(nCountExtBarsInRowReal - nCountOfMinSizedBars);
		INT nCalcStep = (nCalcExtentDiff > 0) ? 1 : -1;
		INT nPass = 0;
		INT nPassMax = abs(nCalcExtentDiff)*2;
		for(	nBarAtSide = nReviewStart;
				// nBarAtSide < nReviewStop &&
				nCalcExtentDiff != 0
				&& nCountOfMinSizedBars != nCountExtBarsInRowReal		
				&& nPass < nPassMax
				;
				)
		{
			ASSERT( nBarAtSide <= nReviewStop+1 );
			if( nBarAtSide == nReviewStop+1 )
			{
				nPass++;
				nBarAtSide = nReviewStart;
			}
			
			CControlBar * pBar = (CControlBar*)
				m_pDockBar->m_arrBars[ nBarAtSide ];
			if( pBar == NULL )
			{
				nBarAtSide++;
				continue;
			}
			if( __PLACEHODLER_BAR_PTR(pBar) )
			{
				nBarAtSide++;
				continue;
			}
			ASSERT_VALID( pBar );
			ASSERT( pBar->GetSafeHwnd() != NULL );
			if( !pBar->IsVisible() )
			{
				nBarAtSide++;
				continue;
			}
			CExtControlBar * pExtBar =
				DYNAMIC_DOWNCAST( CExtControlBar, pBar );
			if( pExtBar == NULL
				|| pExtBar->IsFixedMode()
				)
			{
				nBarAtSide++;
				continue;
			}

			if( nMode & LM_HORZ )
				pExtBar->m_sizeDockedH.cy = nMaxWidth;
			else
				pExtBar->m_sizeDockedV.cx = nMaxWidth;
			
			ASSERT( nCalcExtentDiff != 0 );

			LONG nMinMetric =
				(nMode & LM_HORZ)
					? pExtBar->_CalcDesiredMinHW()
					: pExtBar->_CalcDesiredMinVH()
					;
			LONG & ref_nCalcExtentBar =
				(nMode & LM_HORZ)
					? pExtBar->m_sizeDockedH.cx
					: pExtBar->m_sizeDockedV.cy
					;
			ASSERT( ref_nCalcExtentBar >= nMinMetric );
			if( ref_nCalcExtentBar == nMinMetric )
			{
				nBarAtSide++;
				continue;
			}

			ASSERT( ref_nCalcExtentBar > nMinMetric );
			ref_nCalcExtentBar += nCalcStep;
			nCalcExtentDiff -= nCalcStep;
			if( ref_nCalcExtentBar == nMinMetric )
				nCountOfMinSizedBars++;
			
			ASSERT( ref_nCalcExtentBar >= nMinMetric );
			nBarAtSide++;
		} // for( nBarAtSide = nReviewStart; ...

		ASSERT( nCalcExtentDiff == 0 || nPass == nPassMax );

	} // if( nCountOfMinSizedBars != nCountExtBarsInRowReal )
	else
	{
		CExtControlBar * pLastBar = (CExtControlBar *)
			_GetFirstControlBarInRow(
				this, true, true, true
				);
		ASSERT( pLastBar != NULL );
		while( !pLastBar->IsVisible() )
		{
			CExtControlBar * pLastBar2 = (CExtControlBar *)
				_GetNextControlBarInRow(
					pLastBar, false, true, true
					);
			if( pLastBar2 == NULL )
				break;
			pLastBar = pLastBar2;
		}
		ASSERT( pLastBar != NULL );
		LONG & ref_nCalcExtentBar =
			(nMode & LM_HORZ) ?
				pLastBar->m_sizeDockedH.cx : pLastBar->m_sizeDockedV.cy;
		ref_nCalcExtentBar += nCalcExtentDiff;
		nCalcExtentReal = nCalcExtentMax;
	} // else from if( nCountOfMinSizedBars != nCountExtBarsInRowReal )

__Align_Mutually_Row__:

	ASSERT_VALID( m_pDockBar );
//	if( ((CExtDockBar *)m_pDockBar)->m_bLayoutQuery )
//		return _CalcDesiredLayout( nMode & LM_HORZ );

//bool bAtTop = IsDockedAtTop();
CRect rcWndDockBar;
	m_pDockBar->GetWindowRect( &rcWndDockBar );
CPoint ptDevOffset = -rcWndDockBar.TopLeft();
INT nCalcBarOffset = 0;	

INT nHdwpCount = nReviewStop - nReviewStart + 1;
	ASSERT( nHdwpCount > 0 );
HANDLE hDWP = ::BeginDeferWindowPos( nHdwpCount );
	ASSERT( hDWP != NULL );
static const UINT nDwpFlags =
		SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOACTIVATE
		//|SWP_NOSENDCHANGING|SWP_NOREDRAW
		;

	for( nBarAtSide = nReviewStart; nBarAtSide <= nReviewStop; nBarAtSide++ )
	{
		CControlBar * pBar = (CControlBar*)
			m_pDockBar->m_arrBars[ nBarAtSide ];
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT( pBar->GetSafeHwnd() != NULL );
        if( ! pBar->IsVisible() )
			continue;

		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar != NULL && pExtBar->IsFixedMode() )
			pExtBar = NULL;
		CRect rcWndBarOrg;
		pBar->GetWindowRect( &rcWndBarOrg );
		CRect rcWndBar( rcWndBarOrg );
		rcWndBar.OffsetRect( ptDevOffset );
		
		if( pExtBar != NULL )
            rcWndBar =
				CRect(
					rcWndBar.TopLeft(),
					(nMode & LM_HORZ)
						? pExtBar->m_sizeDockedH
						: pExtBar->m_sizeDockedV
					);
		INT nBarRealOffset =
			(nMode & LM_HORZ) ? rcWndBar.left : rcWndBar.top;
        if( nBarRealOffset != nCalcBarOffset )
        {
            if( !(nMode & LM_HORZ) )
                rcWndBar.OffsetRect(
					0,
					nCalcBarOffset - rcWndBar.top
					);
//            else if( bAtTop )
//                rcWndBar.OffsetRect(
//					nCalcBarOffset - rcWndBar.left,
//					-2
//					);
            else
            	rcWndBar.OffsetRect(
					nCalcBarOffset - rcWndBar.left,
					0
					);
			
		} // if( nBarRealOffset != nCalcBarOffset )
		
		if( rcWndBar != rcWndBarOrg )
		{
			hDWP = ::DeferWindowPos(
				hDWP,
				pBar->GetSafeHwnd(),
				NULL,
				rcWndBar.left, rcWndBar.top,
				rcWndBar.Width(), rcWndBar.Height(),
				nDwpFlags
				);
			ASSERT( hDWP != NULL );
		} // if( rcWndBar != rcWndBarOrg )
			

        nCalcBarOffset += (nMode & LM_HORZ)
			? rcWndBar.Width() : rcWndBar.Height();
	} // for( nBarAtSide = nReviewStart; nBarAtSide <= nReviewStop; nBarAtSide++ )

	VERIFY( ::EndDeferWindowPos( hDWP ) );

	return _CalcDesiredLayout( nMode & LM_HORZ );
}

bool CExtControlBar::_GetFullRowMode() const
{
	ASSERT_VALID( this );
	return false;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtCustomizeSite * CExtControlBar::NotifiedCustomizeSiteGet() const
{
	ASSERT_VALID( this );
	if( m_pNotifiedCustomizeSite == LPVOID(1) )
		return NULL;
	if( m_pNotifiedCustomizeSite != NULL )
		return m_pNotifiedCustomizeSite;
HWND hWndOwn = GetSafeHwnd();
	if( hWndOwn == NULL )
		return NULL;
	m_pNotifiedCustomizeSite =
		CExtCustomizeSite::GetCustomizeSite( hWndOwn );
	if( m_pNotifiedCustomizeSite == NULL )
	{
		m_pNotifiedCustomizeSite = (CExtCustomizeSite*) LPVOID(1);
		return NULL;
	}
	return m_pNotifiedCustomizeSite;
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

bool CExtControlBar::_DisplayingGet() const
{
	ASSERT_VALID( this );
	return m_bDisplaying;
}

void CExtControlBar::_DisplayingSet( bool bDisplaying )
{
	ASSERT_VALID( this );
	m_bDisplaying = bDisplaying;
}

void CExtControlBar::AnimationSite_OnProgressShutdownTimer( UINT nTimerID )
{
	ASSERT_VALID( this);
	CExtAnimationSite::AnimationSite_OnProgressShutdownTimer( nTimerID );
}

HWND CExtControlBar::AnimationSite_GetSafeHWND() const
{
__PROF_UIS_MANAGE_STATE;
HWND hWnd = GetSafeHwnd();
	return hWnd;
}

INT CExtControlBar::_CalcDesiredMinHW() const
{
	ASSERT_VALID( this );
CRect rcRowRecalcUp = _RectRowRecalcUpGet();
INT nCalcVal = m_nMinHW + rcRowRecalcUp.Width();
	if( m_bGripperStaticallyAtTop )
		nCalcVal = max( nCalcVal, 10 );
	return nCalcVal;
}

INT CExtControlBar::_CalcDesiredMinVH() const
{
	ASSERT_VALID( this );
CRect rcRowRecalcUp = _RectRowRecalcUpGet();
INT nCalcVal = m_nMinVH + rcRowRecalcUp.Height();
	return nCalcVal;
}

CSize CExtControlBar::_CalcDesiredMinOuterSize( BOOL bHorz ) const
{
	ASSERT_VALID( this );
	bHorz;
	return CSize( _CalcDesiredMinHW(), _CalcDesiredMinVH() );
}

CSize CExtControlBar::_CalcDesiredMinFloatedSize() const
{
	ASSERT_VALID( this );
	// for non-fixed size bars only
	return CSize( 80, 60 );
}

CSize CExtControlBar::_CalcDesiredLayout(
	BOOL bHorz,
	BOOL bForceFullRow // = FALSE
	)
{
	ASSERT_VALID( this );
	if( IsFloating() )
		return m_sizeFloated;
	if( bHorz )
	{
		if( bForceFullRow )
			return CSize( MAXSHORT, m_sizeDockedH.cy );
		CSize sizeMin( _CalcDesiredMinHW(), _CalcDesiredMinVH() );
		if( m_sizeDockedH.cx < sizeMin.cx )
			m_sizeDockedH.cx = sizeMin.cx;
		if( m_sizeDockedH.cy < sizeMin.cy )
			m_sizeDockedH.cy = sizeMin.cy;
		return m_sizeDockedH;
	} // if( bHorz )
	else
	{
		if( bForceFullRow )
			return CSize( m_sizeDockedH.cx, MAXSHORT );
		CSize sizeMin( _CalcDesiredMinHW(), _CalcDesiredMinVH() );
		if( m_sizeDockedV.cx < sizeMin.cx )
			m_sizeDockedV.cx = sizeMin.cx;
		if( m_sizeDockedV.cy < sizeMin.cy )
			m_sizeDockedV.cy = sizeMin.cy;
		return m_sizeDockedV;
	} // else from if( bHorz )
}

CExtControlBar::InternalAffixmentData *
	CExtControlBar::_AffixmentGetOuter()
{
	ASSERT_VALID( this );
	return NULL;
}

void CExtControlBar::_AffixmentSetOuter(
	CExtControlBar::InternalAffixmentData * pAffixmentData
	)
{
	ASSERT_VALID( this );
	ASSERT( pAffixmentData != NULL );
	pAffixmentData;
}

CExtControlBar::InternalAffixmentData *
	CExtControlBar::_AffixmentGetOuter(
		CControlBar * pBar
		)
{
	ASSERT_VALID( pBar );
CExtControlBar * pExtBar =
		DYNAMIC_DOWNCAST( CExtControlBar, pBar );
	if( pExtBar == NULL )
		return NULL;
	return pExtBar->_AffixmentGetOuter();
}

int CExtControlBar::_AffixmentCompareOuter(
	CControlBar * pBar1,
	CControlBar * pBar2
	)
{
	ASSERT_VALID( pBar1 );
	ASSERT_VALID( pBar2 );
InternalAffixmentData * pAffixmentData1 =
		_AffixmentGetOuter( pBar1 );
InternalAffixmentData * pAffixmentData2 =
		_AffixmentGetOuter( pBar2 );
	if( pAffixmentData1 != NULL )
	{
		if( pAffixmentData2 != NULL )
			return pAffixmentData1->CompareAffixment( *pAffixmentData2 );
	} // if( pAffixmentData1 != NULL )
	else
	{
		if( pAffixmentData2 != NULL )
			return -1;
	} // else from if( pAffixmentData1 != NULL )
	return 0;
}

void CExtControlBar::_AffixmentRecalcOuter()
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pDockBar );
	ASSERT_KINDOF( CExtDockBar, m_pDockBar );
	ASSERT_VALID( m_pDockSite );
	ASSERT( !IsFloating() );
	ASSERT( GetParentFrame() == m_pDockSite );
	ASSERT( (! GetParentFrame()->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) ) );
	ASSERT( GetSafeDockBarDlgCtrlID() != AFX_IDW_DOCKBAR_FLOAT );
InternalAffixmentData * pAffixmentData =
		_AffixmentGetOuter();
	if( pAffixmentData == NULL )
		return;
CExtDockOuterBar * pDockBar =
		DYNAMIC_DOWNCAST(CExtDockOuterBar, m_pDockBar );
	if( pDockBar == NULL )
		return;
	pDockBar->_AffixmentBringToTop( this );
}

void CExtControlBar::_AffixmentSafeClearOuter()
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pDockSite );
InternalAffixmentData * pAffixmentData =
		_AffixmentGetOuter();
	if( pAffixmentData == NULL )
		return;
	pAffixmentData->Empty();
}

bool CExtControlBar::_AffixmentIsAlignedHW() const
{
	ASSERT_VALID( this );
	return false;
}

void CExtControlBar::DoPaint( CDC * pDC )
{
	if(		m_bReposSingleChildMode
		&&	(! IsFixedMode() )
		)
	{
		CWnd * pWnd = GetWindow( GW_CHILD );
		if(		pWnd != NULL
			&&	(pWnd->GetStyle()&WS_VISIBLE) != 0
			)
		{
			CRect rcClient, rcChild;
			GetClientRect( &rcClient );
			pWnd->GetWindowRect( &rcChild );
			ScreenToClient( &rcChild );
			if( rcClient == rcChild )
				return;
		}
	}

#if (defined __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__)

//	CControlBar::DoPaint( pDC );
	ASSERT( pDC->GetSafeHdc() != NULL );
CRect rcBarClient;
	GetClientRect( &rcBarClient );
	pDC->FillSolidRect( &rcBarClient, PmBridge_GetPM()->GetColor( COLOR_INFOBK, this ) );

CRect rcDummy( rcBarClient );
	rcDummy.DeflateRect( 4, 4 );
	pDC->DrawFocusRect( &rcDummy );

#else // __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__

	ASSERT( pDC->GetSafeHdc() != NULL );
CRect rcClient;
	GetClientRect( &rcClient );
	if(		rcClient.IsRectEmpty()
		||	(! pDC->RectVisible( &rcClient ) )
		)
		return;
	CExtPaintManager::stat_ExcludeChildAreas(
		*pDC,
		*this
		);
CExtMemoryDC dc(
		pDC,
		&rcClient
		);
	ASSERT( dc.GetSafeHdc() != NULL );
	if( dc.GetSafeHdc() != NULL )
		pDC = &dc;
	DoEraseBk( pDC );

	PmBridge_GetPM()->OnPaintSessionComplete( this );
#endif // ! __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__
}

void CExtControlBar::DoEraseBk( CDC * pDC )
{
	ASSERT_VALID( this );
	ASSERT( pDC->GetSafeHdc() != NULL );
CRect rcClient;
	GetClientRect( &rcClient );
	PmBridge_GetPM()->PaintControlBarClientArea( *pDC, rcClient, this );
}

BOOL CExtControlBar::DoCustomModePreTranslateMessage(
	CWnd * pWndPretranslateSrc,
	MSG * pMsg
	)
{
	ASSERT( pWndPretranslateSrc != NULL );
	ASSERT( pMsg != NULL );

HWND hWndPretranslateSrc = pWndPretranslateSrc->GetSafeHwnd();
	if(		hWndPretranslateSrc == NULL
		||	( ! ::IsWindow(hWndPretranslateSrc) )
		)
		return FALSE;

	for(	HWND hWndChild = ::GetWindow( hWndPretranslateSrc, GW_CHILD );
			hWndChild != NULL && ::IsWindow( hWndPretranslateSrc );
			hWndChild = ::GetWindow( hWndChild, GW_HWNDNEXT )
		)
	{
		CWnd * pWndChild = CWnd::FromHandlePermanent( hWndChild );
		if( pWndChild == NULL )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pWndChild );
		if( pExtBar == NULL )
			continue;
		if( pExtBar->PreTranslateMessage(pMsg) )
			return TRUE;
	}
	return FALSE;
}

void CExtControlBar::DoCustomModeUpdateControlBars(
	CWnd * pWndParentTarget,
	BOOL bDisableIfNoHndler // = TRUE
	)
{
	ASSERT_VALID( pWndParentTarget );

HWND hWndUpdateTarget = pWndParentTarget->GetSafeHwnd();
	if( hWndUpdateTarget == NULL )
		return;

	ASSERT( ::IsWindow( hWndUpdateTarget ) );
	for(	HWND hWndChild = ::GetWindow( hWndUpdateTarget, GW_CHILD );
			hWndChild != NULL && ::IsWindow( hWndUpdateTarget );
			hWndChild = ::GetWindow( hWndChild, GW_HWNDNEXT )
		)
	{
		CWnd * pWndChild = CWnd::FromHandlePermanent( hWndChild );
		if( pWndChild == NULL )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pWndChild );
		if( pExtBar == NULL )
			continue;
		pExtBar->DoCustomModeUpdateCmdUI(
			bDisableIfNoHndler
			);
	}
}

bool CExtControlBar::DoCustomModeUpdateCmdUI(
	BOOL bDisableIfNoHndler // = TRUE
	)
{
	ASSERT_VALID( this );

	if( m_bInCustomModeUpdateCmdUI )
		return true;

HWND hWndOwn = GetSafeHwnd();
	if( hWndOwn == NULL
		|| ( ! ::IsWindow( hWndOwn ) )
		)
		return false;

	m_bInCustomModeUpdateCmdUI = true;

CFrameWnd * pWndUpdateTarget = NULL;
	
	if( ! m_bPresubclassDialogMode )
	{
		pWndUpdateTarget = _GetDockingFrameImpl();
		ASSERT_VALID( pWndUpdateTarget );
		ASSERT_KINDOF( CFrameWnd, pWndUpdateTarget );
	}
	
	OnUpdateCmdUI(
		pWndUpdateTarget,
		bDisableIfNoHndler
		);
	
	m_bInCustomModeUpdateCmdUI = false;

	return true;
}

LRESULT CExtControlBar::OnIdleUpdateCmdUI( WPARAM wParam, LPARAM )
{
	if( CExtControlBar::_DraggingGetBar() != NULL )
		return 0L;
	if( CExtControlBar::ProfileBarStateIsSerializing( m_pDockSite ) )
		return 0L;

BOOL bVis = GetStyle() & WS_VISIBLE;
UINT swpFlags = 0;
	if( ( m_nStateFlags & delayHide ) && bVis )
		swpFlags = SWP_HIDEWINDOW;
	else if( ( m_nStateFlags & delayShow ) && ( ! bVis ) )
		swpFlags = SWP_SHOWWINDOW;
	m_nStateFlags &= ~(delayShow|delayHide);
	if( swpFlags != 0 )
	{
		SetWindowPos(
			NULL, 0, 0, 0, 0,
			swpFlags|
			SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE
			);
	}
	if(		m_bUseInvisibleCmdUI
		||	(	( GetStyle() & WS_VISIBLE ) != 0
			&&	(	m_pDockBar == NULL
				|| ( m_pDockBar->GetStyle() & WS_VISIBLE ) != 0
				)
			)
		)
	{
		CFrameWnd * pTarget = (CFrameWnd*)GetOwner();
		if( pTarget == NULL || ( ! pTarget->IsFrameWnd() ) )
			pTarget = GetParentFrame();
		if( pTarget != NULL )
			OnUpdateCmdUI( pTarget, (BOOL)wParam );
	}
	return 0L;
}

void CExtControlBar::OnInitialUpdate()
{
	OnIdleUpdateCmdUI( TRUE, 0L );
}

void CExtControlBar::OnUpdateCmdUI(
	CFrameWnd * pTarget,
	BOOL bDisableIfNoHndler
	)
{
	if(	m_bPresubclassDialogMode
		|| m_bDragging || m_bRowRecalcing || m_bRowResizing
		|| (CExtControlBar::_DraggingGetBar() != NULL)
		)
		return;

	pTarget;
	bDisableIfNoHndler;

	if(		( GetStyle() & WS_VISIBLE ) == 0
		&&	( ! m_bUseInvisibleCmdUI )
		)
		return;

CPoint ptScreen;
	if( ! ::GetCursorPos( &ptScreen ) )
		return;
bool bNeedRepaint = false;

UINT nHT = NcButtons_HitTest( ptScreen );
	if( nHT == HTCLIENT )
	{
		if( m_bNcAreaBtnTracking )
			bNeedRepaint = true;
		m_bNcAreaBtnTracking = false;
		if(		( ! CExtPopupMenuWnd::IsKeyPressed( VK_LBUTTON ) )
			&&	GetSafeHwnd() != NULL
			&&	::GetCapture() == m_hWnd
			)
			::ReleaseCapture();
	} // if( nHT == HTCLIENT )
	else
	{
		if( ! m_bNcAreaBtnTracking )
			bNeedRepaint = true;
		m_bNcAreaBtnTracking = true;
	} // else from if( nHT == HTCLIENT )

HWND hTmpWndFocus = ::GetFocus();
bool bWindowActiveOldVal = m_bWindowActive;
	m_bWindowActive =
		( hTmpWndFocus != NULL
		&& (  GetSafeHwnd() == hTmpWndFocus
			  || ::IsChild( GetSafeHwnd(), hTmpWndFocus )
			)
		);
	if( bWindowActiveOldVal != m_bWindowActive )
		bNeedRepaint = true;

	if( bNeedRepaint )
	{
		SendMessage( WM_NCPAINT );
		GetParent()->SendMessage( WM_NCPAINT );
		if( IsVisible() )
		{
			CFrameWnd * pFrame = GetParentFrame();
			if( pFrame->IsKindOf( RUNTIME_CLASS(CExtMiniDockFrameWnd) )  )
				pFrame->SendMessage( WM_NCPAINT );
		}
	}
}

bool CExtControlBar::IsBarWindowActive() const
{
	ASSERT_VALID( this );
	return m_bWindowActive;
}

void CExtControlBar::_RecalcNcArea()
{
	if( !IsVisible() )
		return;
	SetWindowPos(
		NULL, 0, 0, 0, 0,
		SWP_NOSIZE|SWP_NOMOVE
			|SWP_NOZORDER|SWP_NOOWNERZORDER
			|SWP_NOREPOSITION
			|SWP_FRAMECHANGED
		);
}

void CExtControlBar::_RecalcLayoutImpl()
{
	if( ! m_bReflectParentSizing )
		return;
	if( m_bPresubclassDialogMode )
		return;
CWnd * pWndParent = GetParentFrame();
	if(		pWndParent == NULL
		||	pWndParent->GetSafeHwnd() == NULL
		)
		return;
WINDOWPLACEMENT _wp;
	ZeroMemory( &_wp, sizeof(WINDOWPLACEMENT) );
	_wp.length = sizeof(WINDOWPLACEMENT);
InternalFriendlyFrameWnd * pFrame = (InternalFriendlyFrameWnd *)
		( DYNAMIC_DOWNCAST( CFrameWnd, pWndParent ) );
	if(		pFrame == NULL
		||	pFrame->IsInRecalcLayout()
		)
		return;
CWnd * pWndPlacement = stat_GetWndForPlacement( pWndParent );
	ASSERT_VALID( pWndPlacement );
	// pWndPlacement->GetWindowPlacement( &_wp );
	CExtControlBar::stat_GetWindowPlacement( pWndPlacement->m_hWnd, _wp );
	if( _wp.showCmd == SW_SHOWMINIMIZED )
	{
		//pFrame->DelayRecalcLayout();
		return;
	}
	if(		IsFixedMode()
		&&	IsFloating()
		&&	(! CExtControlBar::ProfileBarStateIsSerializing( pFrame ) )
		&&	(pFrame->GetStyle()&WS_VISIBLE) != 0
		)
		pFrame->RecalcLayout();
	else
		pFrame->DelayRecalcLayout();
}

void CExtControlBar::OnGetBarCaptionText(
	CExtControlBar::e_bar_caption_text_t eBCT,
	CExtSafeString & strCaptionText
	) const
{
	ASSERT_VALID( this );
	eBCT;
	strCaptionText.Empty();
	if( GetSafeHwnd() == NULL )
		return;
int nTextLen = GetWindowTextLength();
	if( nTextLen > 0 )
	{
		GetWindowText( strCaptionText.GetBuffer( nTextLen+2 ), nTextLen+1 );
		strCaptionText.ReleaseBuffer();
	} // if( nTextLen > 0 )

//	switch( eBCT )
//	{
//	case __EBCT_SINGLE_CAPTION_DOCKED:
//		strCaptionText += _T(" (1d)");
//		break;
//	case __EBCT_SINGLE_CAPTION_FLOATING:
//		strCaptionText += _T(" (1f)");
//		break;
//	case __EBCT_TAB_GROUP_CAPTION_DOCKED:
//		strCaptionText += _T(" (td)");
//		break;
//	case __EBCT_TAB_GROUP_CAPTION_FLOATING:
//		strCaptionText += _T(" (tf)");
//		break;
//	case __EBCT_TAB_ITEM_CAPTION:
//		strCaptionText += _T(" (ti)");
//		break;
//	case __EBCT_AUTOHIDE_ITEM_CAPTION:
//		strCaptionText += _T(" (ai)");
//		break;
//	case __EBCT_AUTOHIDE_SLIDER_CAPTION:
//		strCaptionText += _T(" (ac)");
//		break;
//	default:
//		{
//			ASSERT( FALSE );
//		}
//		break;
//	} // switch( eBCT )
}

void CExtControlBar::PostNcDestroy()
{
	if( m_bHelperSuppressDestruction )
		return;
	CControlBar::PostNcDestroy();
}

BOOL CExtControlBar::DestroyWindow()
{
	AnimationSite_Done();
	if( m_hWnd == NULL )
	{
//		if( m_bHelperSuppressDestruction )
//			delete this;
		return TRUE;
	}
	return CControlBar::DestroyWindow();
}

LRESULT CExtControlBar::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_NCMOUSEMOVE:
		{
			if( ! OnQueryBarHoverProcessingEnabled() )
				return 0;
			CExtPopupMenuTipWnd * pATTW =
				OnAdvancedPopupMenuTipWndGet();
			if( pATTW == NULL )
				return 0;
			CPoint ptScreen;
			if( ! ::GetCursorPos(&ptScreen) )
				return 0;
			CExtBarNcAreaButton * pBtn = NULL;
			NcButtons_HitTest( ptScreen, &pBtn );
			if( pBtn != NULL )
			{
				ASSERT_VALID( pBtn );
				TOOLINFO _ti;
				::memset( &_ti, 0, sizeof(AFX_OLDTOOLINFO) );
				_ti.cbSize = sizeof( TOOLINFO );
				_ti.hinst = ::AfxGetInstanceHandle();
				if( pBtn->OnToolHitTest( ptScreen, &_ti ) > 0 )
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
						OnAdvancedPopupMenuTipWndDisplay(
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
			}
		}
		break;
	case WM_PRINT:
	case WM_PRINTCLIENT:
		{
			CDC * pDC = CDC::FromHandle( (HDC) wParam );
			CRect rcRgnWnd, rcRgnClient;
			GetWindowRect( &rcRgnWnd );
			GetClientRect( &rcRgnClient );
			if( IsFloating() )
			{
				CRect rcFloatClient;
				GetParentFrame()->GetClientRect( &rcFloatClient );
				if( rcRgnClient.Width() > rcFloatClient.Width() )
					rcRgnClient.right = rcRgnClient.left + rcFloatClient.Width();
				if( rcRgnClient.Height() > rcFloatClient.Height() )
					rcRgnClient.bottom = rcRgnClient.top + rcFloatClient.Height();
			}
			if( (lParam&PRF_NONCLIENT) != 0 )
			{
				CRect rcWnd = rcRgnWnd, rcClient = rcRgnClient;
				ClientToScreen( &rcClient );
				rcClient.OffsetRect( -rcWnd.TopLeft() );
				rcWnd.OffsetRect( -rcWnd.TopLeft() );
				CRgn rgnWnd;
				if( rgnWnd.CreateRectRgnIndirect(&rcWnd) )
					pDC->SelectClipRgn( &rgnWnd );
				pDC->ExcludeClipRect( &rcClient );
				DoPaintNC( pDC );
				pDC->SelectClipRgn( NULL );
			}
			if( (lParam&(PRF_CLIENT|PRF_ERASEBKGND)) != 0 )
			{
				CPoint ptVpOffset( 0, 0 );
				if( (lParam&PRF_NONCLIENT) != 0 )
				{
					CRect rcWnd = rcRgnWnd, rcClient = rcRgnClient;
					ClientToScreen( &rcClient );
					ptVpOffset.x = rcWnd.left - rcClient.left;
					ptVpOffset.y = rcWnd.top - rcClient.top;
				}
				if(		ptVpOffset.x != 0
					||	ptVpOffset.y != 0
					)
					pDC->OffsetViewportOrg(
						-ptVpOffset.x,
						-ptVpOffset.y
						);
//				DoEraseBk( pDC );
//				DoPaint( pDC );
				CDC dcSurface;
				CBitmap bmpSurface;
				CWindowDC dcDesktop( NULL );
				if(		dcSurface.CreateCompatibleDC( NULL )
					&&	bmpSurface.CreateCompatibleBitmap(
							&dcDesktop,
							rcRgnClient.Width(),
							rcRgnClient.Height()
							)
					)
				{
					CBitmap * pOldBmp = dcSurface.SelectObject( &bmpSurface );
					DoEraseBk( &dcSurface );
					DoPaint( &dcSurface );
					pDC->BitBlt(
						0,
						0,
						rcRgnClient.Width(),
						rcRgnClient.Height(),
						&dcSurface,
						0,
						0,
						SRCCOPY
						);
					dcSurface.SelectObject( pOldBmp );
				}
				if(		ptVpOffset.x != 0
					||	ptVpOffset.y != 0
					)
					pDC->OffsetViewportOrg(
						ptVpOffset.x,
						ptVpOffset.y
						);
			} // if( (lParam&(PRF_CLIENT|PRF_ERASEBKGND)) != 0 )
			if( (lParam&PRF_CHILDREN) != 0 )
				CExtPaintManager::stat_PrintChildren(
					m_hWnd,
					message,
					pDC->GetSafeHdc(),
					lParam,
					false
					);
		}
		return (!0);
	case WM_TIMER:
//			if( wParam == __TIMER_ID_DELAYED_UPDATE )
//			{
//				CFrameWnd * pFrame = _GetDockingFrameImpl();
//				if( pFrame != NULL )
//					OnUpdateCmdUI( pFrame, TRUE );
//				KillTimer( __TIMER_ID_DELAYED_UPDATE );
//				return	0L;
//			} // if( wParam == __TIMER_ID_DELAYED_UPDATE )
//			else
			if( wParam == __TIMER_ID_DRELAYED_REPAINT_FAKE )
			{
				RedrawWindow(
					NULL,
					NULL,
					RDW_INVALIDATE|RDW_UPDATENOW
						|RDW_ERASE|RDW_ERASENOW
						|RDW_ALLCHILDREN
						|RDW_FRAME
					);
				KillTimer( __TIMER_ID_DRELAYED_REPAINT_FAKE );
				return	0L;
			} // if( wParam == __TIMER_ID_DRELAYED_REPAINT_FAKE )
			if( AnimationSite_OnHookTimer( UINT(wParam) ) )
				return 0L;
		break;

	case WM_CLOSE:

		// emulate for disabling update in user non-frame mode
		m_bInCustomModeUpdateCmdUI = true;

		return 0;

	case WM_DESTROY:
		AnimationSite_ClientRemove();

#if (_MFC_VER >= 0x700) && (_MFC_VER <= 0x710)
		if(		m_pDockSite != NULL
			&&	m_pDockSite->IsKindOf( RUNTIME_CLASS(CMDIChildWnd) )
			)
		{
			CFrameWnd * pFrame = GetParentFrame();
			if( pFrame != m_pDockSite )
				m_bHelperSuppressDestruction = true;
		}
#endif // (_MFC_VER >= 0x700) && (_MFC_VER <= 0x710)

		// emulate for disabling update in user non-frame mode
		m_bInCustomModeUpdateCmdUI = true;
		
		break;
	case WM_NCDESTROY:
		AnimationSite_ClientRemove();

		if( m_pDockContext != NULL )
		{
			m_pDockBar = NULL;
			delete m_pDockContext;
			m_pDockContext = NULL;
		}

		// emulate for disabling update in user non-frame mode
		m_bInCustomModeUpdateCmdUI = true;

		NcButtons_RemoveAll();
		break;
	case WM_SETFOCUS:
		{
			LRESULT lResult =
				CControlBar::WindowProc(message, wParam, lParam);
			if( IsMinimizedOnRow() )
				MaximizeOnRow();
			if( !IsFixedMode() )
			{
				CWnd * pWnd = GetWindow(GW_CHILD);
				if(		pWnd != NULL
					&&	stat_QueryFocusChangingEnabled( this, pWnd->m_hWnd )
					)
				{
					pWnd->SetFocus();
//#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
//					ASSERT(
//						pWnd->GetWindow(GW_HWNDNEXT) == NULL
//						|| IsKindOf(RUNTIME_CLASS(CExtDynTabControlBar))
//						);
//#else
//					ASSERT(
//						pWnd->GetWindow(GW_HWNDNEXT) == NULL
//						);
//#endif // else from (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
				}
			} // if( !IsFixedMode() )
			return lResult;
		}
	case WM_ERASEBKGND:
		if( !m_bDoNotEraseClientBackground )
			return !0;
		CExtPaintManager::stat_ExcludeChildAreas(
			(HDC)wParam,
			*this
			);
		return CControlBar::WindowProc(message, wParam, lParam);
	case WM_CREATE:
		{
			if( (!m_bPresubclassDialogMode) && (!IsFixedMode()) )
			{
				ASSERT( GetSafeHwnd() != NULL );
				HWND hWndParent = ::GetParent( GetSafeHwnd() );
				CFrameWnd * pFrame = NULL;
				do
				{
					ASSERT( hWndParent != NULL );
					ASSERT( ::IsWindow(hWndParent) );
					CWnd * pWnd = CWnd::FromHandle( hWndParent );
					if( pWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
					{
						pFrame = (CFrameWnd *)pWnd;
						break;
					}
				} while( pFrame == NULL );
				ASSERT( pFrame != NULL );
				ASSERT_VALID( pFrame );
				if( pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
				{
					pFrame = pFrame->GetParentFrame();
					ASSERT( pFrame != NULL );
					ASSERT_VALID( pFrame );
					ASSERT( !pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) );
				}
				VERIFY( _FrameEnableDockingImpl( pFrame ) );
			} // if( (!m_bPresubclassDialogMode) && (!IsFixedMode()) )

			////AnimationSite_Init( false );
		}
		break;
	case WM_SHOWWINDOW:
	case WM_SIZE:
			m_bDelelayRepaintNcButtons = true;
			OnControlBarPositionChange(
				this,
				__ECBPC_MUTUAL_DEPENDENCY,
				true,
				true
				);
		break;
	case WM_WINDOWPOSCHANGED:
		{
			CExtPopupMenuTipWnd * pATTW =
				OnAdvancedPopupMenuTipWndGet();
			if( pATTW != NULL )
				pATTW->Hide();
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
			CExtCustomizeSite * pSite = NotifiedCustomizeSiteGet();
			if( pSite != NULL )
				pSite->OnBarStateChanged( this );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
			
			if( m_pDockBar == NULL && (!m_bPresubclassDialogMode) )
				break; // on destruction

			LPWINDOWPOS lpWindowPos = 
				reinterpret_cast < LPWINDOWPOS > (lParam);
			ASSERT( lpWindowPos != NULL );

			if( ! IsFixedMode() )
				_UpdateVisibilityInChain();

			CExtControlBar * pBar = this;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			CExtDynTabControlBar * pTabBar = _GetNearestTabbedContainer();
			if( pTabBar != NULL && pTabBar != this )
				pBar = pTabBar;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			pBar->m_bDelelayRepaintNcButtons = true;
			pBar->PostMessage( WM_NCPAINT );

			if( (lpWindowPos->flags & SWP_FRAMECHANGED) == 0 )
				_RecalcNcArea();
			OnControlBarPositionChange(
				this,
				__ECBPC_MUTUAL_DEPENDENCY,
				true,
				true
				);
			break;
		}
	case WM_WINDOWPOSCHANGING:
		{
			m_bDelelayRepaintNcButtons = true;
			if( m_pDockBar == NULL && (!m_bPresubclassDialogMode) )
				break; // on destruction

			LPWINDOWPOS lpWindowPos = 
				reinterpret_cast < LPWINDOWPOS > (lParam);
			ASSERT( lpWindowPos != NULL );
			lpWindowPos->flags |= SWP_FRAMECHANGED;

			break;
		}
	case WM_SETTEXT:
		{
			m_bDelelayRepaintNcButtons = true;
			LRESULT lResult = CControlBar::WindowProc(message, wParam, lParam);
			if(		(! IsFixedMode() )
				&&	(! IsKindOf( RUNTIME_CLASS(CExtDynControlBar) ) )
				)
			{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
				if( AutoHideModeGet() )
				{
					CExtDynAutoHideArea * pTabs =
						CExtDynAutoHideArea::stat_FindAreaOfBar( this );
					if( pTabs != NULL )
					{
						ASSERT_VALID( pTabs );
						pTabs->UpdateTabWnd();
						CExtDynAutoHideSlider * pAutoHideSlider =
							pTabs->GetAutoHideSlider();
						if(		pAutoHideSlider->GetSafeHwnd() != NULL
							&&	(pAutoHideSlider->GetStyle()&WS_VISIBLE) != 0
							)
							pAutoHideSlider->SendMessage( WM_NCPAINT );
					} // if( pTabs != NULL )
				} // if( AutoHideModeGet() )
				else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
					if( IsVisible() )
				{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
					CWnd * pWnd = GetParent();
					bool bRecalcThisNcArea = true;
					if(		pWnd != NULL
						&&	pWnd->IsKindOf( RUNTIME_CLASS(CExtDockDynTabBar) )
						)
					{
						ASSERT_VALID( pWnd );
						pWnd = pWnd->GetParent();
						ASSERT_VALID( pWnd );
						CExtDynTabControlBar * pTabBar =
							DYNAMIC_DOWNCAST(
								CExtDynTabControlBar,
								pWnd
								);
						if( pTabBar != NULL )
						{
							pTabBar->InvalidateSwitcher();
							CFrameWnd * pFrame = pTabBar->GetParentFrame();
							ASSERT_VALID( pFrame );
							pFrame->DelayRecalcLayout();
							pFrame->PostMessage( WM_NULL );
							LONG nSelIdx = pTabBar->GetSwitcherSelection();
							if( nSelIdx >= 0 )
							{
								CExtControlBar * pBarTest = pTabBar->GetBarAt( nSelIdx, true );
								if( pBarTest == this )
								{
									CString strText;
									GetWindowText( strText );
									pTabBar->SetWindowText( strText );
								} // if( pBarTest == this )
							} // if( nSelIdx >= 0 )
							((CExtControlBar*)pTabBar)->_RecalcNcArea();
							bRecalcThisNcArea = false;
						} // if( pTabBar != NULL )
					} // if( pWnd != NULL ...
					if( bRecalcThisNcArea )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
						_RecalcNcArea();
				} // else if( IsVisible() )
			} // if( ! IsFixedMode() ...
			return lResult;
		} // case WM_SETTEXT:
	} // switch( message )

	return CControlBar::WindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// CExtControlBar message handlers

void CExtControlBar::NcButtons_Paint( CDC & dc )
{
	ASSERT( dc.GetSafeHdc() != NULL );
INT nCountOfNcButtons = NcButtons_GetCount();
	for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
	{
		CExtBarNcAreaButton * pBtn =
			NcButtons_GetAt( nBtn );
		if( ! pBtn->OnQueryVisibility() )
			continue;
		pBtn->OnNcAreaDraw( dc );
	} // for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
}

CRect & CExtControlBar::NcButtons_CalcGripperTextLocation(
	CRect & rcPreCalcText
	)
{
INT nCountOfNcButtons = NcButtons_GetCount();
	if( nCountOfNcButtons == 0 )
		return rcPreCalcText; 

bool bHorz = false;
bool bGripperAtTop = true;
	if( ! IsFloating() )
	{
		if( ! IsBarWithGripper( &bGripperAtTop ) )
			return rcPreCalcText;
		bHorz = IsDockedHorizontally();
	}

	// find last visible at right or at bottom
CExtBarNcAreaButton * pBtn = NULL;
	for( INT nBtnIdx = nCountOfNcButtons; nBtnIdx > 0 ; nBtnIdx-- )
	{
		CExtBarNcAreaButton * pBtnExamine = NcButtons_GetAt( nBtnIdx - 1 );
		ASSERT_VALID( pBtnExamine );
		if( pBtnExamine->OnQueryVisibility() )
		{
			pBtn = pBtnExamine;
			break;
		}
	} // for( INT nBtnIdx = nCountOfNcButtons; nBtnIdx > 0 ; nBtnIdx-- )
CRect rcDefalte = PmBridge_GetPM()->Bar_GetCaptionBordersForText( this );
	rcPreCalcText.DeflateRect(
		rcDefalte.left,
		rcDefalte.top,
		rcDefalte.right,
		rcDefalte.bottom
		);
	if( pBtn == NULL )
		return rcPreCalcText;

	// adjust caption text rect
const CRect & rcBtn = *pBtn;
	if( bHorz && (! bGripperAtTop ) )
	{
		rcPreCalcText.top = rcBtn.bottom + 1;
		if( rcPreCalcText.top > rcPreCalcText.bottom )
			rcPreCalcText.top = rcPreCalcText.bottom;
	} // if( bHorz && (! bGripperAtTop ) )
	else
	{
		if( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 )
		{
			rcPreCalcText.left = rcBtn.right + 1;
			if( rcPreCalcText.right < rcPreCalcText.left )
				rcPreCalcText.right = rcPreCalcText.left;
		}
		else
		{
			rcPreCalcText.right = rcBtn.left - 1;
			if( rcPreCalcText.left > rcPreCalcText.right )
				rcPreCalcText.left = rcPreCalcText.right;
		}
	} // if( bHorz && (! bGripperAtTop ) )

	return rcPreCalcText;
}

UINT CExtControlBar::NcButtons_HitTest(
	CPoint point,
	CExtBarNcAreaButton ** ppBtn // = NULL
	)
{
	ASSERT_VALID( this );
	if( ppBtn != NULL )
		(*ppBtn) = NULL;
bool bNeedRepaint = false;
CWnd * pWndNcRepaint = this;
CRect rcBarWnd;
	if( IsFloating() || _IsSingleVisibleInFloatingPalette() )
	{
		CExtMiniDockFrameWnd * pFrame = DYNAMIC_DOWNCAST( CExtMiniDockFrameWnd, GetParentFrame() );
		if( pFrame == NULL )
			return HTCLIENT;
		pFrame->GetWindowRect( &rcBarWnd );
		pWndNcRepaint = pFrame;
	} // if( IsFloating() || _IsSingleVisibleInFloatingPalette() )
	else
	{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( AutoHideModeGet() )
		{
			ASSERT_VALID( m_pDockBar );
			ASSERT_KINDOF( CExtDockBar, m_pDockBar );
			CExtDynAutoHideArea * pWndAutoHideArea =
				((CExtDockBar*)m_pDockBar)->_GetAutoHideArea();
			ASSERT_VALID( pWndAutoHideArea );
			ASSERT_VALID( pWndAutoHideArea->m_pWndSlider );
			if(		pWndAutoHideArea->m_pWndSlider->_GetBar() != this
				||	((pWndAutoHideArea->m_pWndSlider->GetStyle() & WS_VISIBLE) == 0)
				)
				return HTCLIENT;
			pWndAutoHideArea->m_pWndSlider->
				GetWindowRect( &rcBarWnd );
		}
		else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			GetWindowRect( &rcBarWnd );
	} // else from if( IsFloating() )
CPoint ptDevOffset = -rcBarWnd.TopLeft();
CPoint ptWnd( point + ptDevOffset );
bool bDragging = m_bDragging;
	if( ! bDragging )
		bDragging = (_DraggingGetBar() != NULL) ? true : false;
UINT nFirstHT = HTCLIENT;
	if(		_DraggingGetBar() == NULL
		&& (!CExtPopupMenuWnd::IsMenuTracking())
		)
	{
		INT nCountOfNcButtons = NcButtons_GetCount();
		for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
		{
			CExtBarNcAreaButton * pBtn = NcButtons_GetAt( nBtn );
			if( ! pBtn->OnQueryVisibility() )
				continue;
			if(		( pBtn->m_bHover || pBtn->m_bPushed )
				&&	ppBtn == NULL
				)
			{
				bNeedRepaint = true;
				if( bDragging || m_bRowRecalcing || m_bRowResizing )
				{
					pBtn->m_bHover = false;
					pBtn->m_bPushed = false;
					continue;
				}
			}
			UINT nHT = pBtn->OnNcAreaHitTest( ptWnd );
			if( nHT != HTCLIENT )
			{
				if( ppBtn != NULL )
				{
					*ppBtn = pBtn;
					return nHT;
				}
				// TO FIX: ASSERT( nFirstHT == HTCLIENT );
				nFirstHT = nHT;
				bNeedRepaint = true;
			}
		} // for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
	}
	if( bNeedRepaint )
		pWndNcRepaint->SendMessage( WM_NCPAINT );
	return nFirstHT;
}

UINT CExtControlBar::OnNcHitTest(CPoint point) 
{
	if( _DraggingGetBar() != NULL )
		return HTNOWHERE;

UINT nHT = NcButtons_HitTest( point );
	if( nHT != HTCLIENT )
	{
		m_bNcAreaBtnTracking = true;
		return nHT;
	}
	else
	{
		if( m_bNcAreaBtnTracking )
			PostMessage( WM_NCPAINT );
		m_bNcAreaBtnTracking = false;
//		if(		( ! CExtPopupMenuWnd::IsKeyPressed( VK_LBUTTON ) )
//			&&	GetSafeHwnd() != NULL
//			&&	::GetCapture() == m_hWnd
//			)
//			::ReleaseCapture();
	}

	if( IsFixedMode() || IsFloating() )
		return HTCLIENT;

CRect rcBarWnd;
	GetWindowRect( &rcBarWnd );
CPoint ptDevOffset = -rcBarWnd.TopLeft();
CRect rcBarClient;
	GetClientRect( &rcBarClient );
	ClientToScreen( &rcBarClient );
	if( rcBarWnd == rcBarClient )
		return HTCLIENT;
//	rcBarWnd.OffsetRect( ptDevOffset );
//	rcBarClient.OffsetRect( ptDevOffset );

	point += ptDevOffset;

CRect rcRowResize = _RectRowResizeGet();
CRect rcRowRecalc = _RectRowRecalcGet();
CRect rcRowRecalcUp = _RectRowRecalcUpGet();
bool bHorz = IsDockedHorizontally();
	if(		(! rcRowResize.IsRectEmpty() )
		&&	rcRowResize.PtInRect( point )
		)
	{
		if( ! _RowResizingIsEnabled( this ) )
			return HTNOWHERE;
		return bHorz ? HTBOTTOM : HTRIGHT;
	}	
	if(		(! rcRowRecalc.IsRectEmpty() )
		&&	rcRowRecalc.PtInRect( point )
		)
	{
		if( _IsLastBarInRow( true ) )
			return HTNOWHERE; //HTCLIENT;
		if( ! _RowRecalcingIsEnabled( this ) )
			return HTNOWHERE;
		if( !_RowRecalcing_IsBottomEnabled() )
			return HTNOWHERE; //HTCLIENT;
		return bHorz ? HTRIGHT : HTBOTTOM;
	}	
	if(		(! rcRowRecalcUp.IsRectEmpty() )
		&&	rcRowRecalcUp.PtInRect( point )
		)
	{
		if( _IsFirstBarInRow( true ) )
			return HTNOWHERE; //HTCLIENT;
		if( ! _RowRecalcingIsEnabled( this ) )
			return HTNOWHERE;
		if( ! _RowRecalcing_IsTopEnabled() )
			return HTNOWHERE; //HTCLIENT;
		return bHorz ? HTLEFT : HTTOP;
	}	
	return HTCLIENT;
}

void CExtControlBar::_GetDeepNcModeMetrics(
	bool & bEnableResizerLeft,
	bool & bEnableResizerTop,
	bool & bEnableResizerRight,
	bool & bEnableResizerBottom
	)
{
	bEnableResizerLeft = true;
	bEnableResizerTop = true;
	bEnableResizerRight = true;
	bEnableResizerBottom = true;

	if( IsFixedMode() )
		return;

//	if( !IsVisible() )
//		return;

	if( m_pDockSite == NULL || m_pDockBar == NULL )
	{
		bEnableResizerLeft = false;
		bEnableResizerTop = false;
		bEnableResizerRight = false;
		bEnableResizerBottom = false;
		return;
	}
	ASSERT_VALID( m_pDockSite );
	ASSERT_VALID( m_pDockBar );

	if( ! m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
	{
		bEnableResizerLeft = false;
		bEnableResizerTop = false;
		bEnableResizerRight = false;
		bEnableResizerBottom = false;
		return;
	}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynTabBar)) )
	{
		bEnableResizerLeft = false;
		bEnableResizerTop = false;
		bEnableResizerRight = false;
		bEnableResizerBottom = false;
		return;
	}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

CExtDockBar * pDockBar = (CExtDockBar *) m_pDockBar;
UINT nDockBarID = pDockBar->GetDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );

BOOL bHorz = ( nDockBarID == AFX_IDW_DOCKBAR_TOP || nDockBarID == AFX_IDW_DOCKBAR_BOTTOM );

INT nCount = (INT)pDockBar->m_arrBars.GetSize();
	ASSERT( nCount >= 1 );
	if( nCount == 1 )
		return;

INT nOwnIdx = pDockBar->FindBar( this );
	if( nOwnIdx == -1 )
		return; // destruction
	ASSERT( nOwnIdx > 0 && nOwnIdx < nCount );

bool bPrevRowExist = false;
bool bFirstVisibleOnRow = true;
INT nBar = nOwnIdx-1;
	for( ; nBar > 0; nBar-- )
	{
		CControlBar * pBar = (CControlBar *) pDockBar->m_arrBars[nBar];
		if( pBar == NULL )
		{
			if( nBar > 0 )
				bPrevRowExist = true;
			break;
		}
		if( __PLACEHODLER_BAR_PTR( pBar ) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CControlBar, pBar );
		if( pBar->IsVisible() )
			bFirstVisibleOnRow = false;
	}
bool bPrevVisibleRowExist = false;
	if( bPrevRowExist )
	{
		for( nBar--; nBar > 0; nBar-- )
		{
			CControlBar * pBar = (CControlBar *) pDockBar->m_arrBars[nBar];
			if( pBar == NULL )
				continue;
			if( __PLACEHODLER_BAR_PTR( pBar ) )
				continue;
			ASSERT_VALID( pBar );
			ASSERT_KINDOF( CControlBar, pBar );
			if( pBar->IsVisible() )
			{
				bPrevVisibleRowExist = true;
				break;
			}
		}
	}

bool bNextRowExist = false;
bool bLastVisibleOnRow = true;
	for( nBar = nOwnIdx+1; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *) pDockBar->m_arrBars[nBar];
		if( pBar == NULL )
		{
			if( nBar < (nCount-1) )
				bNextRowExist = true;
			break;
		}
		if( __PLACEHODLER_BAR_PTR( pBar ) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CControlBar, pBar );
		if( pBar->IsVisible() )
			bLastVisibleOnRow = false;
	}
bool bNextVisibleRowExist = false;
	if( bNextRowExist )
	{
		for( nBar++; nBar < nCount; nBar++ )
		{
			CControlBar * pBar = (CControlBar *) pDockBar->m_arrBars[nBar];
			if( pBar == NULL )
				continue;
			if( __PLACEHODLER_BAR_PTR( pBar ) )
				continue;
			ASSERT_VALID( pBar );
			ASSERT_KINDOF( CControlBar, pBar );
			if( pBar->IsVisible() )
			{
				bNextVisibleRowExist = true;
				break;
			}
		}
	}

	if( m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynBar)) )
	{ // if dockbar is dynamic docker
		if( bHorz )
		{
			if( !bPrevVisibleRowExist )
				bEnableResizerTop = false;
/**/		if( !bNextVisibleRowExist )
/**/			bEnableResizerBottom = false;
			if( bFirstVisibleOnRow )
				bEnableResizerLeft = false;
			if( bLastVisibleOnRow )
				bEnableResizerRight = false;
		} // if( bHorz )
		else
		{
			if( !bPrevVisibleRowExist )
				bEnableResizerLeft = false;
/**/		if( !bNextVisibleRowExist )
/**/			bEnableResizerRight = false;
			if( bFirstVisibleOnRow )
				bEnableResizerTop = false;
			if( bLastVisibleOnRow )
				bEnableResizerBottom = false;
		} // else from if( bHorz )

	} // if dockbar is dynamic docker
	else
	{ // if dockbar is static docker
		
		INT nOwnCircleNo = GetDockBarCircleNo();
		if( nOwnCircleNo > 0 )
		{
			switch( nDockBarID )
			{
			case AFX_IDW_DOCKBAR_TOP:
				if( !bPrevVisibleRowExist )
					bEnableResizerTop = false;
//				if( !bNextVisibleRowExist )
//					bEnableResizerBottom = false;
				if( bFirstVisibleOnRow )
					bEnableResizerLeft = false;
				if( bLastVisibleOnRow )
					bEnableResizerRight = false;
				break;
			case AFX_IDW_DOCKBAR_BOTTOM:
//				if( !bPrevVisibleRowExist )
//					bEnableResizerTop = false;
				if( !bNextVisibleRowExist )
					bEnableResizerBottom = false;
				if( bFirstVisibleOnRow )
					bEnableResizerLeft = false;
				if( bLastVisibleOnRow )
					bEnableResizerRight = false;
				break;
			case AFX_IDW_DOCKBAR_LEFT:
				if( !bPrevVisibleRowExist )
					bEnableResizerLeft = false;
//				if( !bNextVisibleRowExist )
//					bEnableResizerRight = false;
				if( bFirstVisibleOnRow )
					bEnableResizerTop = false;
				if( bLastVisibleOnRow )
					bEnableResizerBottom = false;
				break;
			case AFX_IDW_DOCKBAR_RIGHT:
//				if( !bPrevVisibleRowExist )
//					bEnableResizerLeft = false;
				if( !bNextVisibleRowExist )
					bEnableResizerRight = false;
				if( bFirstVisibleOnRow )
					bEnableResizerTop = false;
				if( bLastVisibleOnRow )
					bEnableResizerBottom = false;
				break;
#ifdef _DEBUG
			default:
					{
						ASSERT( FALSE );
					}
				break;
#endif // _DEBUG
			} // switch( nDockBarID )
		} // if( nOwnCircleNo > 0 )

	} // if dockbar is static docker

}

void CExtControlBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	bCalcValidRects;

	if( m_pDockBar == NULL && (!m_bPresubclassDialogMode) )
		return; // on destruction

CRect & rcClientLoc =
		reinterpret_cast < CRect & > ( lpncsp->rgrc[0] );
CRect rcClientLocSaved = rcClientLoc;
	if( IsFloating() ) // (+ v.2.22)
	{
		if( !IsFixedMode() ) // (+ v.2.23)
		{
			rcClientLoc.DeflateRect( 1, 1 );
			return;
		}
		rcClientLoc.OffsetRect( 0, -rcClientLoc.top );
		rcClientLoc.OffsetRect( -rcClientLoc.left, 0 );
		return;
	}


bool bEnableResizerLeft = true;
bool bEnableResizerTop = true;
bool bEnableResizerRight = true;
bool bEnableResizerBottom = true;
	_GetDeepNcModeMetrics(
		bEnableResizerLeft,
		bEnableResizerTop,
		bEnableResizerRight,
		bEnableResizerBottom
		);

CRect rcDevOrg( rcClientLoc );

CRect rcGrip, rcRowResize, rcRowRecalc, rcRowRecalcUp;
	rcGrip.SetRectEmpty();
	rcRowResize.SetRectEmpty();
	rcRowRecalc.SetRectEmpty();
	rcRowRecalcUp.SetRectEmpty();
INT nSeparatorWidth = 0;
INT nSeparatorHeight = 0;
INT nGripWidthAtLeft = 0;
INT nGripHeightAtTop = 0;
CUSTOM_NC_AREA_QUERY_DATA _cnaqd( this );
	_cnaqd.Notify();
	nSeparatorWidth = _SeparatorWidthGet();
	nSeparatorHeight = _SeparatorHeightGet();
	nGripWidthAtLeft = _GripperWidthAtLeftGet();
	nGripHeightAtTop = _GripperHeightAtTopGet();
	m_nMinHW = nSeparatorWidth;
	m_nMinVH = nSeparatorHeight;

bool bGripperAtTop = false;
bool bGripperExists = IsBarWithGripper( &bGripperAtTop );

#ifdef _DEBUG
	if( IsKindOf( RUNTIME_CLASS( CExtDynControlBar ) ) )
	{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( !IsKindOf( RUNTIME_CLASS( CExtDynTabControlBar ) ) )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		{
			ASSERT( !bGripperExists );
		}
	}
#endif // _DEBUG
	
CPoint ptGripOffset( 0, 0 );
UINT nDockBarDlgCtrlID = GetSafeDockBarDlgCtrlID();

	if( ! IsFixedMode() )
	{
		MfcControlBarVector_t vBars;
		_GetRowBars( vBars );
		int nCountOfnBars = (int)vBars.GetSize();
		INT nLastFixedIdx = -1;
		CExtControlBar * pLastExtBar = NULL;
		bool bAddTopRecalc = false;
		for( INT nBar = 0; nBar < nCountOfnBars ; nBar++ )
		{
			CControlBar * pBar = vBars[ nBar ];
			ASSERT_VALID( pBar );
			if( !pBar->IsVisible() )
				continue;
			CExtControlBar * pExtBar =
				DYNAMIC_DOWNCAST( CExtControlBar, pBar );
			if( pExtBar != NULL && pExtBar->IsFixedMode() )
				pExtBar = NULL;
			if( pExtBar != NULL )
			{
				if( pExtBar == this
					&& pLastExtBar != NULL
					&& nLastFixedIdx == (nBar-1)
					)
				{
					bAddTopRecalc = true;
					break;
				}
				pLastExtBar = pExtBar;
			}
			else
				nLastFixedIdx = nBar;
		} // for( INT nBar = 0; nBar < nCountOfnBars ; nBar++ )
		switch( nDockBarDlgCtrlID )
		{
		case AFX_IDW_DOCKBAR_TOP:
			if( bEnableResizerRight )
			{
				rcRowRecalc = rcClientLoc;
				rcRowRecalc.OffsetRect( -rcClientLoc.left, -rcClientLoc.top );
				rcRowRecalc.left = rcRowRecalc.right - nSeparatorWidth;
				rcClientLoc.right -= nSeparatorWidth;
			}
			
			if( bEnableResizerBottom )
			{
				rcClientLoc.DeflateRect( 0, 0, 0, nSeparatorHeight );
				rcRowResize = rcClientLoc;
				rcRowResize.OffsetRect( -rcClientLoc.left, -rcClientLoc.top );
				rcRowResize.top = rcRowResize.bottom;
				rcRowResize.bottom += nSeparatorHeight;
				rcRowResize.right = rcRowResize.left + rcDevOrg.Width();
			}

			if( bAddTopRecalc )
			{
				rcRowRecalcUp = rcClientLoc;
				rcRowRecalcUp.OffsetRect( -rcClientLoc.left, -rcClientLoc.top );
				rcRowRecalcUp.right = rcRowRecalcUp.left + nSeparatorWidth;
				if( bGripperExists )
					ptGripOffset.x += nSeparatorWidth;
				rcClientLoc.DeflateRect( nSeparatorWidth, 0, 0, 0 );
			} // if( bAddTopRecalc )
		break;
		case AFX_IDW_DOCKBAR_BOTTOM:
			if( bEnableResizerRight )
			{
				rcRowRecalc = rcClientLoc;
				rcRowRecalc.OffsetRect( -rcClientLoc.left, -rcClientLoc.top );
				rcRowRecalc.left = rcRowRecalc.right - nSeparatorWidth;
				rcClientLoc.right -= nSeparatorWidth;
			}
			
			if( bEnableResizerTop )
			{
				rcClientLoc.DeflateRect( 0, nSeparatorHeight, 0, 0 );
				rcRowResize = rcClientLoc;
				rcRowResize.OffsetRect( -rcClientLoc.left, -rcClientLoc.top );
				rcRowResize.bottom = rcRowResize.top + nSeparatorHeight;
				rcRowResize.right = rcRowResize.left + rcDevOrg.Width();
				if( bGripperExists )
					ptGripOffset.y += nSeparatorHeight;
			}

			if( bAddTopRecalc )
			{
				rcRowRecalcUp = rcClientLoc;
				rcRowRecalcUp.OffsetRect( -rcClientLoc.left, -rcClientLoc.top );
				rcRowRecalcUp.right = rcRowRecalcUp.left + nSeparatorWidth;
				if( bGripperExists )
					ptGripOffset.x += nSeparatorWidth;
				rcClientLoc.DeflateRect( nSeparatorWidth, 0, 0, 0 );
			} // if( bAddTopRecalc )
		break;
		case AFX_IDW_DOCKBAR_LEFT:
			if( bEnableResizerBottom )
			{
				rcRowRecalc = rcClientLoc;
				rcRowRecalc.OffsetRect( -rcClientLoc.left, -rcClientLoc.top );
				rcRowRecalc.top = rcRowRecalc.bottom - nSeparatorHeight;
				rcClientLoc.DeflateRect( 0, 0, 0, nSeparatorHeight );
			}
			
			if( bEnableResizerRight )
			{
				rcClientLoc.DeflateRect( 0, 0, nSeparatorHeight, 0 );
				rcRowResize = rcClientLoc;
				rcRowResize.OffsetRect( -rcClientLoc.left, -rcClientLoc.top );
				rcRowResize.left = rcRowResize.right;
				rcRowResize.right += nSeparatorWidth;
				rcRowResize.bottom = rcRowResize.top + rcDevOrg.Height();
			}

			if( bAddTopRecalc )
			{
				rcRowRecalcUp = rcClientLoc;
				rcRowRecalcUp.OffsetRect( -rcClientLoc.left, -rcClientLoc.top );
				rcRowRecalcUp.bottom = rcRowRecalcUp.top + nSeparatorWidth;
				if( bGripperExists )
					ptGripOffset.y += nSeparatorHeight;
				rcClientLoc.DeflateRect( 0, nSeparatorHeight, 0, 0 );
			} // if( bAddTopRecalc )
		break;
		case AFX_IDW_DOCKBAR_RIGHT:
			if( bEnableResizerBottom )
			{
				rcRowRecalc = rcClientLoc;
				rcRowRecalc.OffsetRect( -rcClientLoc.left, -rcClientLoc.top );
				rcRowRecalc.top = rcRowRecalc.bottom - nSeparatorHeight;
				rcClientLoc.DeflateRect( 0, 0, 0, nSeparatorHeight );
			}
			
			if( bEnableResizerLeft )
			{
				rcClientLoc.DeflateRect( nSeparatorHeight, 0, 0, 0 );
				rcRowResize = rcClientLoc;
				rcRowResize.OffsetRect( -rcClientLoc.left, -rcClientLoc.top );
				rcRowResize.right = rcRowResize.left + nSeparatorWidth;
				rcRowResize.bottom = rcRowResize.top + rcDevOrg.Height();
				if( bGripperExists )
					ptGripOffset.x += nSeparatorWidth;
			}

			if( bAddTopRecalc )
			{
				rcRowRecalcUp = rcClientLoc;
				rcRowRecalcUp.OffsetRect( -rcClientLoc.left, -rcClientLoc.top );
				rcRowRecalcUp.bottom = rcRowRecalcUp.top + nSeparatorWidth;
				if( bGripperExists )
					ptGripOffset.y += nSeparatorHeight;
				rcClientLoc.DeflateRect( 0, nSeparatorHeight, 0, 0 );
			} // if( bAddTopRecalc )
		break;
		} // switch( nDockBarDlgCtrlID )
	} // if( ! IsFixedMode() )
	else
	{
		PmBridge_GetPM()->FixedBar_AdjustClientRect(
			this,
			rcClientLoc
			);
	} // else from if( !IsFixedMode() )

	if( bGripperExists )
	{
		rcGrip = rcClientLoc;
		rcGrip.OffsetRect( -rcClientLoc.left, -rcClientLoc.top );
		if( bGripperAtTop )
		{
			rcGrip.bottom = rcGrip.top + nGripHeightAtTop;
			m_nMinVH += rcGrip.Height();
			if( IsFixedMode() )
				rcGrip.DeflateRect( __BAR_NC_GAP_X, 0 );
		} // if( bGripperAtTop )
		else
		{
			rcGrip.right = rcGrip.left + nGripHeightAtTop;
			m_nMinHW += rcGrip.Width();
			if( IsFixedMode() )
				rcGrip.DeflateRect( 0, __BAR_NC_GAP_Y );
		} // else from if( bGripperAtTop )
		rcGrip.OffsetRect( ptGripOffset );

		if( IsFixedMode() )
			rcGrip.DeflateRect(
				(nDockBarDlgCtrlID == AFX_IDW_DOCKBAR_LEFT)   ? __BAR_NC_GAP_X : 0,
				(nDockBarDlgCtrlID == AFX_IDW_DOCKBAR_TOP)    ? __BAR_NC_GAP_X : 0,
				(nDockBarDlgCtrlID == AFX_IDW_DOCKBAR_RIGHT)  ? __BAR_NC_GAP_X : 0,
				(nDockBarDlgCtrlID == AFX_IDW_DOCKBAR_BOTTOM) ? __BAR_NC_GAP_X : 0
				);
	} // if( bGripperExists )

	OnNcAreaButtonsReinitialize();
	OnNcAreaButtonsReposition();

	if( bGripperExists )
	{
		rcClientLoc.DeflateRect(
			bGripperAtTop
				? 0
				: nGripWidthAtLeft  + __BAR_NC_GAP_X
				,
			bGripperAtTop
				? nGripHeightAtTop  + __BAR_NC_GAP_Y
				: 0
				,
			0,
			0
			);
	} // if( bGripperExists )

//	if( !IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
//		rcClientLoc.DeflateRect( __BAR_NC_GAP_X, __BAR_NC_GAP_Y );

	if( rcClientLoc.bottom < rcClientLoc.top )
		rcClientLoc.bottom = rcClientLoc.top;
	if( rcClientLoc.right < rcClientLoc.left )
		rcClientLoc.right = rcClientLoc.left;

	_RectGripSet( rcGrip );
	_RectRowResizeSet( rcRowResize );
	_RectRowRecalcSet( rcRowRecalc );
	_RectRowRecalcUpSet( rcRowRecalcUp );
}

void CExtControlBar::OnNcPaint() 
{
//	if(		m_pDockBar == NULL
//		&& (!m_bPresubclassDialogMode)
//		)
//		return; // on destruction
	if( m_bDelelayRepaintNcButtons )
	{
		m_bDelelayRepaintNcButtons = false;
		OnNcAreaButtonsReposition();
	}

#if (defined __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__)

CRect rcBarWnd, rcBarClient;
	GetWindowRect( &rcBarWnd );
	GetClientRect( &rcBarClient );
	ClientToScreen( &rcBarClient );
	if( rcBarWnd == rcBarClient )
		return;
CPoint ptDevOffset = -rcBarWnd.TopLeft();
	rcBarWnd.OffsetRect( ptDevOffset );
	rcBarClient.OffsetRect( ptDevOffset );

CWindowDC dcBarWnd( this );
	ASSERT( dcBarWnd.GetSafeHdc() != NULL );
	dcBarWnd.ExcludeClipRect( &rcBarClient );

CDC & dc = dcBarWnd;
	dc.FillSolidRect( &rcBarWnd, PmBridge_GetPM()->GetColor( COLOR_3DFACE, this ) );

bool bGripperAtTop = false;
bool bGripperExists = IsBarWithGripper( &bGripperAtTop );

	if( bGripperExists )
	{
		CRect rcGrip = _RectGripGet();
		// ASSERT( ! rcGrip.IsRectEmpty() );
		CExtSafeString sWndText;
		OnGetBarCaptionText( __EBCT_SINGLE_CAPTION_DOCKED, sWndText );
		dc.FillSolidRect( &rcGrip, PmBridge_GetPM()->GetColor( COLOR_HIGHLIGHT, this ) );
		if( !sWndText.IsEmpty() )
		{
			CRect rcGripText( rcGrip );
			rcGripText.DeflateRect( 2, 2 );
			HFONT hOldFont = (HFONT)
				::SelectObject(
					dc.GetSafeHdc(),
					::GetStockObject(DEFAULT_GUI_FONT)
					);
			COLORREF clrTextOld =
				dc.SetTextColor( PmBridge_GetPM()->GetColor( COLOR_HIGHLIGHTTEXT, this ) );
			int nOldBkMode = dc.SetBkMode( TRANSPARENT );
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				sWndText,
				&rcGripText,
				DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS
				);
			dc.SetBkMode( nOldBkMode );
			dc.SetTextColor( clrTextOld );
			::SelectObject(
				dc.GetSafeHdc(),
				hOldFont
				);
		}
	} // if( bGripperExists )

CRect rcRowResize = _RectRowResizeGet();
CRect rcRowRecalc = _RectRowRecalcGet();
CRect rcRowRecalcUp = _RectRowRecalcUpGet();

	if( ! rcRowResize.IsRectEmpty() )
		dc.FillSolidRect( &rcRowResize, RGB(255,0,0) );
	if( ! rcRowRecalc.IsRectEmpty() )
		dc.FillSolidRect( &rcRowRecalc, RGB(0,255,0) );
	if( ! rcRowRecalcUp.IsRectEmpty() )
		dc.FillSolidRect( &rcRowRecalcUp, RGB(0,0,255) );

	if( IsBarWithGripper() && !IsFloating() )
		NcButtons_Paint( dc );

#else // __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__

CWindowDC dc( this );
CRect rcClient, rcBar;
	GetClientRect( rcClient );
	ClientToScreen( rcClient );
	GetWindowRect( rcBar );
	rcClient.OffsetRect( -rcBar.TopLeft() );
	rcBar.OffsetRect( -rcBar.TopLeft() );
	dc.ExcludeClipRect( rcClient );
CExtMemoryDC mdc( &dc, &rcBar );
CUSTOM_NC_AREA_QUERY_DATA _cnaqd( this, mdc.GetSafeHdc() );
	if( ! _cnaqd.Notify() )
		DoPaintNC( &mdc );
	mdc.__Flush();
	ReleaseDC( &dc );
	PmBridge_GetPM()->OnPaintSessionComplete( this );

#endif // ! __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__
}

void CExtControlBar::DoPaintNC( CDC * pDC )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pDC );
	ASSERT( pDC->GetSafeHdc() != NULL );

CRect rcClient, rcBar;
	GetClientRect( rcClient );
	ClientToScreen( rcClient );
	GetWindowRect( rcBar );
	rcClient.OffsetRect( -rcBar.TopLeft() );
	rcBar.OffsetRect( -rcBar.TopLeft() );
	pDC->ExcludeClipRect( rcClient );

	if(		(! PmBridge_GetPM()->GetCb2DbTransparentMode(this) )
		||	(! PmBridge_GetPM()->PaintDockerBkgnd( false, *pDC, this ) )
		)
		pDC->FillSolidRect(
			&rcBar,
			PmBridge_GetPM()->GetColor(
				CExtPaintManager::CLR_3DFACE_OUT, this
				)
			);
bool bFloating = IsFloating() ? true : false;
bool bFixedMode = IsFixedMode();
bool bPaintFrame = true;
	if(		bFloating
		&&	bFixedMode
		&&	CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON != NULL
		&&	CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->m_pHelperTearOffBar == this
		)
		bPaintFrame = false;
	if( bPaintFrame )
	{
		CExtPaintManager::PAINTDOCKINGFRAMEDATA _pdfd(
			this,
			rcBar,
			rcClient,
			IsBarWindowActive(),
			bFloating,
			! bFixedMode,
			! bFixedMode
			);
		PmBridge_GetPM()->PaintDockingFrame( *pDC, _pdfd );
	} // if( bPaintFrame )

	if( ! bFloating )
	{
		bool bHorz = IsDockedHorizontally();
		CRect rc;
		rc = _RectRowResizeGet();
		if( ! rc.IsRectEmpty() )
			PmBridge_GetPM()->PaintResizableBarSeparator(
				*pDC,
				rc,
				bHorz,
				this
				);
		rc = _RectRowRecalcGet();
		if( ! rc.IsRectEmpty() )
			PmBridge_GetPM()->PaintResizableBarSeparator(
				*pDC,
				rc,
				!bHorz,
				this
				);
		rc = _RectRowRecalcUpGet();
		if( ! rc.IsRectEmpty() )
			PmBridge_GetPM()->PaintResizableBarSeparator(
				*pDC,
				rc,
				!bHorz,
				this
				);
	} // if( ! bFloating )

bool bGripperAtTop = false;
	if( IsBarWithGripper(&bGripperAtTop) && (! bFloating ) )
	{
		CRect rcGrip = _RectGripGet();
		bool bHorz = IsDockedHorizontally();
		CRect rcText( rcGrip );
		if( bHorz && (! bGripperAtTop ) )
		{
			rcText.DeflateRect( 0, 2 );
			rcText.OffsetRect( -2, 0 );
		} // if( bHorz && (! bGripperAtTop ) )
		else
		{
			rcText.DeflateRect( 2, 0 );
		} // else from if( bHorz && (! bGripperAtTop ) )
		NcButtons_CalcGripperTextLocation( rcText );
		CExtSafeString sCaption;
		OnGetBarCaptionText( __EBCT_SINGLE_CAPTION_DOCKED, sCaption );
		CRgn rgnExclude;
		if( rgnExclude.CreateRectRgnIndirect( &rcGrip ) )
			pDC->SelectClipRgn( &rgnExclude );
		CExtControlBar * pFlashingBar = this;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		CExtDynTabControlBar * pTabbedBar = DYNAMIC_DOWNCAST( CExtDynTabControlBar, this );
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
			pFlashingBar, // this
			rcGrip,
			rcText,
			IsBarWindowActive(),
			false,
			bHorz && !bGripperAtTop,
			!bFixedMode,
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
					*pDC,
					(LPVOID)(&_pgd)
					)
				)
				bDrawDefaultCaption = false;
		} // if( pFlashingBar->FlashCaptionIsInProgress( &bFlashCaptionHighlightedState ) )
		if( bDrawDefaultCaption )
		{
			PmBridge_GetPM()->PaintGripper( *pDC, _pgd );
			NcButtons_Paint( *pDC );
		} // if( bDrawDefaultCaption )
		pDC->SelectClipRgn( NULL );
	} // if( IsBarWithGripper(&bGripperAtTop) && (! bFloating ) )
}

void CExtControlBar::OnSysColorChange() 
{
	CControlBar::OnSysColorChange();
//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnSysColorChange( this );
//	g_CmdManager.OnSysColorChange( pPM, this );
	_RecalcLayoutImpl();
}

void CExtControlBar::OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection) 
{
    CControlBar::OnSettingChange(uFlags, lpszSection);
//	m_nGripHeightAtTop = ::GetSystemMetrics(SM_CYSMCAPTION)+1;
//	m_nGripWidthAtLeft = ::GetSystemMetrics(SM_CYSMCAPTION)+1;
//	m_nSeparatorHeight = min( 4, ::GetSystemMetrics(SM_CYSIZEFRAME) );
//	m_nSeparatorWidth = min( 4, ::GetSystemMetrics(SM_CXSIZEFRAME) );
//	m_nMinHW = ::GetSystemMetrics(SM_CYSMCAPTION)+1 + m_nSeparatorHeight;
//	m_nMinVH = ::GetSystemMetrics(SM_CYSMCAPTION)+1 + m_nSeparatorHeight;

CExtPaintManager * pPM = PmBridge_GetPM();
	g_PaintManager.OnSettingChange( this, uFlags, lpszSection );
	g_CmdManager.OnSettingChange( pPM, this, uFlags, lpszSection );
	_RecalcLayoutImpl();
}

LRESULT CExtControlBar::OnDisplayChange( WPARAM wParam, LPARAM lParam )
{
//LRESULT lResult = CControlBar::OnDisplayChange( wParam, lParam );

	CControlBar::OnDisplayChange(wParam, LOWORD(lParam), HIWORD(lParam));

LRESULT lResult = 0;


//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnDisplayChange( this, (INT)wParam, CPoint(lParam) );
//	g_CmdManager.OnDisplayChange( pPM, this, (INT)wParam, CPoint(lParam) );
	return lResult;
}

LRESULT CExtControlBar::OnThemeChanged( WPARAM wParam, LPARAM lParam )
{
	wParam;
	lParam;
LRESULT lResult = Default();
//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnThemeChanged( this, wParam, lParam );
//	g_CmdManager.OnThemeChanged( pPM, this, wParam, lParam );
	return lResult;
}

void CExtControlBar::OnCancelMode() 
{
	if( m_bRowResizing )
		_RowResizingStop( true );
	if( m_bRowRecalcing )
		_RowRecalcingStop( true );
	if( m_bDragging )
		_DraggingStop( true );
//	CExtPopupMenuWnd::CancelMenuTracking();
	CWnd::CancelToolTips();
//	ASSERT( !CExtPopupMenuWnd::IsMenuTracking() );
	if( m_bNcAreaBtnTracking )
	{
		m_bNcAreaBtnTracking = false;
		PostMessage( WM_NCPAINT );
		if(		GetSafeHwnd() != NULL
			&&	::GetCapture() == m_hWnd
			)
			::ReleaseCapture();
	}
	CControlBar::OnCancelMode();
}

void CExtControlBar::OnCaptureChanged(CWnd *pWnd) 
{
	if( m_bRowResizing )
		_RowResizingStop( true );
	if( m_bRowRecalcing )
		_RowRecalcingStop( true );
//	if( m_bDragging )
//		_DraggingStop( true );
	CControlBar::OnCaptureChanged(pWnd);
}

bool CExtControlBar::_OnMouseMoveMsg(UINT nFlags, CPoint point)
{
	nFlags;
	_SetCursor( point );
	if( m_bNcAreaBtnTracking )
		return true;
	if( m_bRowResizing )
	{
		_RowResizingUpdateState();
		return true;
	}
	if( m_bRowRecalcing )
	{
		_RowRecalcingUpdateState();
		return true;
	}
	if( m_bDragging )
	{
		CPoint ptScreen( point );
		ClientToScreen( &ptScreen );
		_DraggingUpdateState(
			ptScreen,
			CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) ? true : false
			);
		return true;
	}
	if(		IsFloating()
		&&	CExtMiniDockFrameWnd::g_bAutoMakeTopmostOnMouseEvent
		&&	(!CExtPopupMenuWnd::IsMenuTracking())
		)
	{
		CFrameWnd * pParentFrame = GetParentFrame();
		if( pParentFrame != NULL )
			pParentFrame->BringWindowToTop();
	}

	return false;
}

void CExtControlBar::OnMouseMove(UINT nFlags, CPoint point) 
{
__PROF_UIS_MANAGE_STATE;
	if( ! CExtPopupMenuWnd::IsMenuTracking() )
	{
		if( ! OnQueryBarHoverProcessingEnabled() )
			return;
	} // if( ! CExtPopupMenuWnd::IsMenuTracking() )
	if( _OnMouseMoveMsg(nFlags,point) )
		return;
	CControlBar::OnMouseMove(nFlags,point);
}

bool CExtControlBar::OnQueryBarHoverProcessingEnabled() const
{
	ASSERT_VALID( this );
HWND hWndOwn = GetSafeHwnd();
	if( hWndOwn == NULL )
		return false;
	if( ! CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND( hWndOwn ) )
		return false;
	return true;
}

bool CExtControlBar::_RowRecalcing_IsBottomEnabled()
{
	// previous or next CExtControlBar should not be minimized
ExtControlBarVector_t vBars;
	_GetRowExtBars( vBars );
INT nCountOfBars = (INT)vBars.GetSize();
bool bThisPassed = false;
CExtControlBar * pBarPrev = NULL, * pBarNext = NULL;
	for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
	{
		CExtControlBar * pBar = vBars[ nBar ];
		ASSERT_VALID( pBar );
		if( !pBar->IsVisible() )
			continue;
		if( !bThisPassed )
			pBarPrev = pBar;
		else
		{
			pBarNext = pBar;
			break;
		}
		if( pBar == this )
		{
			bThisPassed = true;
			continue;
		}
	} // for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
bool bMinimizedPrev = true, bMinimizedNext = true;
	if( pBarPrev != NULL )
		bMinimizedPrev = pBarPrev->IsMinimizedOnRow();
	if( pBarNext != NULL )
		bMinimizedNext = pBarNext->IsMinimizedOnRow();
	if( bMinimizedPrev && bMinimizedNext )
		return false;
	return true;
}

bool CExtControlBar::_RowRecalcing_IsTopEnabled()
{
	// this or next extbar should not be minimized
ExtControlBarVector_t vBars;
	_GetRowExtBars( vBars );
INT nCountOfBars = (INT)vBars.GetSize();
CExtControlBar * pBarNext = NULL;
	for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
	{
		CExtControlBar * pBar = vBars[ nCountOfBars - nBar - 1 ];
		ASSERT_VALID( pBar );
		if( !pBar->IsVisible() )
			continue;
		if( pBar == this )
			break;
		pBarNext = pBar;
	} // for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
bool bMinimizedNext = true;
	if( pBarNext != NULL )
		bMinimizedNext = pBarNext->IsMinimizedOnRow();
	if( bMinimizedNext && IsMinimizedOnRow() )
		return false;
	return true;
}

void CExtControlBar::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	ASSERT_VALID( this );

	_ActivateOnClick();

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = NotifiedCustomizeSiteGet();
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		pSite->DropTargetPopupCancel();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( IsFixedMode() || IsFloating() )
		return;

	switch( nHitTest )
	{
	case HTLEFT:
	case HTBOTTOM:
		break; // case HTLEFT & HTBOTTOM
	default:
		if(		( !IsFixedMode() )
			&&	( !IsMinimizedOnRow() )
			)
		{
			CPoint ptScreen( point );
			//ClientToScreen( &ptScreen );
			CExtBarNcAreaButton * pBtn = NULL;
			( const_cast < CExtControlBar * > ( this ) ) ->
				NcButtons_HitTest( ptScreen, &pBtn );
			if( pBtn == NULL ) // (+ v.2.24)
			{
				if( stat_QueryFocusChangingEnabled( this, m_hWnd ) )
					SetFocus();
			}
		}
		//return;
		break;
	} // switch( nHitTest )

	ASSERT( !m_bRowRecalcing );
	ASSERT( !m_bRowResizing );

CRect rcBarWnd, rcBarClient;
	GetWindowRect( &rcBarWnd );
	GetClientRect( &rcBarClient );
	ClientToScreen( &rcBarClient );
	if( rcBarWnd == rcBarClient )
		return;
CPoint ptDevOffset = -rcBarWnd.TopLeft();
//	rcBarWnd.OffsetRect( ptDevOffset );
//	rcBarClient.OffsetRect( ptDevOffset );

	point += ptDevOffset;

CRect rcRowResize = _RectRowResizeGet();
CRect rcRowRecalc = _RectRowRecalcGet();
CRect rcRowRecalcUp = _RectRowRecalcUpGet();
bool bHorz = IsDockedHorizontally();
	if(		(! rcRowResize.IsRectEmpty() )
		&&	rcRowResize.PtInRect( point )
		)
	{
		ASSERT( (INT)nHitTest == (bHorz ? HTBOTTOM : HTRIGHT) );
		_RowResizingStart();
		return;
	}	
	if(		(! rcRowRecalc.IsRectEmpty() )
		&&	rcRowRecalc.PtInRect( point )
		)
	{
		ASSERT( !_IsLastBarInRow( true ) );
		ASSERT( (INT)nHitTest == (bHorz ? HTRIGHT : HTBOTTOM) );
		m_bTopRecalcing = false;
		
		if( !_RowRecalcing_IsBottomEnabled() )
			return;
		
		_RowRecalcingStart();
		return;
	}	
	if(		(! rcRowRecalcUp.IsRectEmpty() )
		&&	rcRowRecalcUp.PtInRect( point )
		)
	{
		ASSERT( !_IsFirstBarInRow( true ) );
		ASSERT( (INT)nHitTest == (bHorz ? HTLEFT : HTTOP) );

		if( !_RowRecalcing_IsTopEnabled() )
			return;
		
		m_bTopRecalcing = true;
		_RowRecalcingStart();
		return;
	}

CPoint ptScreen = point + rcBarWnd.TopLeft();
bool bNeedRepaint = false;
UINT nHT = NcButtons_HitTest( ptScreen );
	if( nHT == HTCLIENT )
	{
		if( m_bNcAreaBtnTracking )
			bNeedRepaint = true;
		m_bNcAreaBtnTracking = false;
		if(		GetSafeHwnd() != NULL
			&&	::GetCapture() == m_hWnd
			)
			::ReleaseCapture();
	} // if( nHT == HTCLIENT )
	else
	{
		if( !m_bNcAreaBtnTracking )
			bNeedRepaint = true;
		m_bNcAreaBtnTracking = true;
		if(		GetSafeHwnd() != NULL
			&&	::GetCapture() != m_hWnd
			)
			::SetCapture( m_hWnd );
	} // else from if( nHT == HTCLIENT )
	if( bNeedRepaint )
	{
		SendMessage( WM_NCPAINT );
		GetParent()->SendMessage( WM_NCPAINT );
	}
}

void CExtControlBar::OnNcLButtonUp(UINT nHitTest, CPoint point) 
{
//	if( m_bNcAreaBtnTracking )
//	{
//		if(		GetSafeHwnd() == NULL
//			||	::GetCapture() != m_hWnd
//			)
//			return;
//		::ReleaseCapture();
//		CRect rcBarWnd;
//		GetWindowRect( &rcBarWnd );
//		CPoint ptDevOffset = -rcBarWnd.TopLeft();
//		CPoint ptWnd( point + ptDevOffset );
//		INT nCountOfNcButtons = NcButtons_GetCount();
//		for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
//		{
//			CExtBarNcAreaButton * pBtn =
//				NcButtons_GetAt( nBtn );
//			if( ! pBtn->OnQueryVisibility() )
//				continue;
//			if( pBtn->OnNcAreaClicked( ptWnd ) )
//				return;
//		} // for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
//		return;
//	}

	if( m_bRowResizing )
	{
		_RowResizingStop( false );
		return;
	}
	
	if( m_bRowRecalcing )
	{
		_RowRecalcingStop( false );
		return;
	}
	
	if( m_bDragging )
	{
		//_DraggingStop( false );
		return;
	}

	CControlBar::OnNcLButtonUp(nHitTest, point);
}

void CExtControlBar::OnRButtonDown(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	_ActivateOnClick();
    CControlBar::OnRButtonDown( nFlags, point );
}

void CExtControlBar::OnMButtonDown(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	_ActivateOnClick();
    CControlBar::OnMButtonDown( nFlags, point );
}

void CExtControlBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );

    if( m_pDockBar == NULL || CExtPopupMenuWnd::IsMenuTracking() )
	{
        CWnd::OnLButtonDown(nFlags, point);
		return;
	}

	_ActivateOnClick();

	// start dragging
	ASSERT_VALID( m_pDockBar );
    ASSERT( m_pDockContext != NULL );

    ClientToScreen( &point );
	
bool bExtendedDragging = _IsShowContentWhenDragging();
	if( !bExtendedDragging && IsFixedMode() )
	{
		m_pDockContext->StartDrag( point );
		return;
	}

	if( m_pDockBar != NULL && (!IsFloating()) )
	{
		if( !m_pDockBar->IsKindOf(
				RUNTIME_CLASS(CExtDockBar)
				)
			)
			bExtendedDragging = false;
	}

	if( IsFloating() )
	{
		CMiniDockFrameWnd * pMiniFrame =
			DYNAMIC_DOWNCAST(
				CMiniDockFrameWnd,
				GetDockingFrame()
				);
		ASSERT_VALID( pMiniFrame );

		CExtMiniDockFrameWnd * pExtMiniFrame =
			DYNAMIC_DOWNCAST(
				CExtMiniDockFrameWnd,
				GetDockingFrame()
				);
		if( pExtMiniFrame != NULL )
		{
			pExtMiniFrame->OnNcLButtonDown(
				HTCAPTION,
				point
				);
			return;
		}
	} // if( IsFloating() )

	if( ! IsFixedMode() )
	{
		if( stat_QueryFocusChangingEnabled( this, m_hWnd ) )
			SetFocus();
	}

CPoint ptOffset( point );
CRect rcWnd;
	GetWindowRect( &rcWnd );
	ptOffset -= rcWnd.TopLeft();

	_DraggingStart( point, ptOffset );
}

void CExtControlBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_bRowResizing )
	{
		_RowResizingStop( false );
		return;
	}
	if( m_bRowRecalcing )
	{
		_RowRecalcingStop( false );
		return;
	}
	if( m_bDragging )
	{
		_DraggingStop( false );
		return;
	}
	if( m_bNcAreaBtnTracking )
	{
		if(		GetSafeHwnd() == NULL
			||	::GetCapture() != m_hWnd
			)
			return;
		::ReleaseCapture();
		CRect rcBarWnd;
		GetWindowRect( &rcBarWnd );
		CPoint ptDevOffset = -rcBarWnd.TopLeft();
		CPoint ptWnd = point;
		ClientToScreen( &ptWnd );
		ptWnd += ptDevOffset;
		INT nCountOfNcButtons = NcButtons_GetCount();
		for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
		{
			CExtBarNcAreaButton * pBtn =
				NcButtons_GetAt( nBtn );
			if( ! pBtn->OnQueryVisibility() )
				continue;
			if( pBtn->OnNcAreaClicked( ptWnd ) )
				return;
		} // for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
		return;
	}
	CControlBar::OnLButtonUp(nFlags, point);
}

void CExtControlBar::OnRButtonUp(UINT nFlags, CPoint point) 
{
	nFlags;

//	if( m_bRowResizing )
//		_RowResizingStop();
//	if( m_bRowRecalcing )
//		_RowRecalcingStop();
//	if( m_bDragging )
//		_DraggingStop( false );
//	CControlBar::OnRButtonUp(nFlags, point);

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = NotifiedCustomizeSiteGet();
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		return;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if(		m_bPresubclassDialogMode
		||	_GetDockingFrameImpl() == NULL
		||	CExtControlBar::FindHelpMode( this )
		)
	{
		CControlBar::OnRButtonUp( nFlags, point );
		return;
	}

CFrameWnd * pFrame = _GetDockingFrameImpl();
	ASSERT_VALID( pFrame );

	if( ! ::GetCursorPos(&point) )
		return;
	if(		(! IsFixedMode() )
		&&	(! IsKindOf(RUNTIME_CLASS(CExtPanelControlBar)) )
		)
	{
		CPoint ptClient = point;
		ScreenToClient( &ptClient );
		CRect rcClient;
		GetClientRect( &rcClient );
		if( rcClient.PtInRect(ptClient) )
			return;
	}

HWND hWndTrack = GetOwner()->GetSafeHwnd();
	ASSERT( hWndTrack != NULL && ::IsWindow(hWndTrack) );
CExtPopupMenuWnd * pPopup =
		CExtPopupMenuWnd::InstantiatePopupMenu(
			GetSafeHwnd(),
			RUNTIME_CLASS(CExtPopupMenuWnd),
			this
			);
	VERIFY( pPopup->CreatePopupMenu(hWndTrack) );
	if(	! CExtDockBar::_ConstructDockSiteControlBarPopupMenu(
			pFrame,
			pPopup,
			CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_CONTROLBAR_CTX,
			this,
			WM_RBUTTONUP,
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
CWnd * pWndChild = GetWindow( GW_CHILD );
	if( pWndChild != NULL )
	{
		HWND hWndOwn = m_hWnd;
		if( stat_QueryFocusChangingEnabled( this, pWndChild->m_hWnd ) )
		{
			pWndChild->SetFocus();
			CExtPopupMenuWnd::PassMsgLoop( true );
			if( ! ::IsWindow( hWndOwn ) )
			{
				delete pPopup;
				return;
			}
			m_bWindowActive = true;
			SendMessage( WM_NCPAINT );
		}
	}
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
}

void CExtControlBar::OnNcRButtonUp(UINT nHitTest, CPoint point)
{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = NotifiedCustomizeSiteGet();
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		return;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if(		m_bPresubclassDialogMode
		||	_GetDockingFrameImpl() == NULL
		||	CExtControlBar::FindHelpMode( this )
		)
	{
		CControlBar::OnNcRButtonUp( nHitTest, point );
		return;
	}

CFrameWnd * pFrame = _GetDockingFrameImpl();
	ASSERT_VALID( pFrame );
	if( ! ::GetCursorPos(&point) )
		return;
HWND hWndTrack = GetOwner()->GetSafeHwnd();
	ASSERT( hWndTrack != NULL && ::IsWindow(hWndTrack) );
CExtPopupMenuWnd * pPopup =
		CExtPopupMenuWnd::InstantiatePopupMenu(
			GetSafeHwnd(),
			RUNTIME_CLASS(CExtPopupMenuWnd),
			this
			);
	VERIFY( pPopup->CreatePopupMenu(hWndTrack) );
	if(	! CExtDockBar::_ConstructDockSiteControlBarPopupMenu(
			pFrame,
			pPopup,
			CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_CONTROLBAR_NC_CTX,
			this,
			WM_RBUTTONUP,
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
	if( stat_QueryFocusChangingEnabled( this, hWndTrack ) )
		::SetFocus( hWndTrack );
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
}

void CExtControlBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    if( m_pDockBar == NULL )
	{
        CWnd::OnLButtonDblClk(nFlags, point);
		return;
	}
	if( ! _FloatStateChangingIsEnabled( this ) )
		return;
	ToggleDocking();
}

void CExtControlBar::OnNcLButtonDblClk(UINT nHitTest, CPoint point) 
{
    if( m_pDockBar == NULL || nHitTest != HTCAPTION )
	{
        CWnd::OnNcLButtonDblClk(nHitTest, point);
		return;
	}
	if( ! _FloatStateChangingIsEnabled( this ) )
		return;
	ToggleDocking();
}

void CExtControlBar::OnNcMButtonDown(UINT nHitTest, CPoint point)
{
	ASSERT_VALID( this );
	_ActivateOnClick();
	CControlBar::OnNcMButtonDown( nHitTest, point );
}

void CExtControlBar::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	ASSERT_VALID( this );
	_ActivateOnClick();
	CControlBar::OnNcRButtonDown( nHitTest, point );
}

bool CExtControlBar::_IsShowContentWhenDragging() const
{
//BOOL bDragShowContent = FALSE;
//    ::SystemParametersInfo(
//		SPI_GETDRAGFULLWINDOWS,
//		0,
//        &bDragShowContent,
//		0
//		);
//	return bDragShowContent ? true : false;
#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
	eResizablePanelDockingType_t eResizablePanelDockingType =
		OnQueryDockingType();
	if( INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005) )
		return true;
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)
	return false;
}

bool CExtControlBar::_IsShowContentWhenRowResizing() const
{
#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
	eResizablePanelDockingType_t eResizablePanelDockingType =
		OnQueryDockingType();
	if( INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005) )
		return false;
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)
	return _IsShowContentWhenDragging();
}

bool CExtControlBar::_IsShowContentWhenRowRecalcing() const
{
#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
	eResizablePanelDockingType_t eResizablePanelDockingType =
		OnQueryDockingType();
	if( INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005) )
		return false;
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)
	return _IsShowContentWhenDragging();
}

CExtControlBar::e_control_bar_position_changing_t CExtControlBar::GetCurrentControlBarPositionChangingType() const
{
	ASSERT_VALID( this );
	return m_eCBPC;
}

void CExtControlBar::OnControlBarPositionChange(
	CExtControlBar * pBarEventSrc,
	e_control_bar_position_changing_t eCBPC,
	bool bInitial,
	bool bFinal
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBarEventSrc );
	pBarEventSrc;
	if( ! ( bInitial && bFinal ) )
	{
		if( bInitial && m_eCBPC == __ECBPC_EMPTY )
			m_eCBPC = eCBPC;
		else if( bFinal && m_eCBPC == eCBPC )
			m_eCBPC = __ECBPC_EMPTY;
	}
}

void CExtControlBar::_OnControlBarPositionChange_RedockingImpl(
	CExtControlBar * pBarEventSrc,
	e_control_bar_position_changing_t eCBPC,
	bool bInitial,
	bool bFinal
	)
{
	ASSERT_VALID( this );

	m_bDelayShowInvoked = true;
	m_bDelayShowFlag = IsVisible() ? true : false;
CExtDynControlBar * pDynContainer = _GetNearestDynamicContainer();
	if( pDynContainer != NULL )
	{
		pDynContainer->m_bDelayShowInvoked = true;
		pDynContainer->m_bDelayShowFlag = m_bDelayShowFlag;
	}

ExtControlBarVector_t vBars;
    _GetFrameControlBars(
		NULL,
		pBarEventSrc->m_pDockSite,
		vBars
		);
INT nCountOfBars = (INT)vBars.GetSize();
	ASSERT( nCountOfBars > 0 );
	for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
	{
		CExtControlBar * pBar = vBars[ nBar ];
		ASSERT_VALID( pBar );
		pBar->OnControlBarPositionChange(
			pBarEventSrc,
			eCBPC,
			bInitial,
			bFinal
			);
	}
}

void CExtControlBar::_OnControlBarPositionChange_InRowImpl(
	CExtControlBar * pBarEventSrc,
	e_control_bar_position_changing_t eCBPC,
	bool bInitial,
	bool bFinal
	)
{
	ASSERT_VALID( this );

	_OnControlBarPositionChange_RedockingImpl(
		pBarEventSrc,
		eCBPC,
		bInitial,
		bFinal
		);

//ExtControlBarVector_t vBars;
//	_GetRowExtBars( vBars );
//INT nCountOfBars = (INT)vBars.GetSize();
//	ASSERT( nCountOfBars > 0 );
//	for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
//	{
//		CExtControlBar * pBar = vBars[ nBar ];
//		ASSERT_VALID( pBar );
//		pBar->OnControlBarPositionChange(
//			pBarEventSrc,
//			eCBPC,
//			bInitial,
//			bFinal
//			);
//	}
}

CRect CExtControlBar::g_rcHelperRowResizingLimits( -32767, -32767, 32767, 32767 );

void CExtControlBar::_RowResizingStart()
{
	ASSERT( !m_bRowRecalcing );
	ASSERT( !m_bRowResizing );
	g_rcHelperRowResizingLimits.SetRect( -32767, -32767, 32767, 32767 );
	CWnd::CancelToolTips();
	CExtToolControlBar::_CloseTrackingMenus();
	CExtPopupMenuWnd::CancelMenuTracking();
	if( m_pDockSite->GetSafeHwnd() != NULL )
	{
		if( GetParentFrame() == m_pDockSite )
		{
			ASSERT_VALID( m_pDockBar );
			ASSERT( m_pDockBar->GetSafeHwnd() != NULL );
			CWnd * pWnd = m_pDockBar;
			for( ; true; )
			{
				CWnd * pWnd2 = pWnd->GetParent();
				if( pWnd2 == NULL )
				{
					pWnd = NULL;
					break;
				}
				if( pWnd2 == m_pDockSite )
					break;
				pWnd = pWnd2;
			} // for( ; true; )
			if( pWnd != NULL )
			{
				CExtDockBar * pDockBar = DYNAMIC_DOWNCAST( CExtDockBar, pWnd );
				if( pDockBar != NULL )
				{
					UINT nDockBarID = UINT( pDockBar->GetDlgCtrlID() );
					if(		nDockBarID == AFX_IDW_DOCKBAR_TOP
						||	nDockBarID == AFX_IDW_DOCKBAR_BOTTOM
						||	nDockBarID == AFX_IDW_DOCKBAR_LEFT
						||	nDockBarID == AFX_IDW_DOCKBAR_RIGHT
						)
					{
						CRect rcFrameInner, rcOwnClient;
						GetClientRect( &rcOwnClient );
						m_pDockSite->GetClientRect( &rcFrameInner );
						m_pDockSite->RepositionBars(
							0,
							0x0FFFF,
							AFX_IDW_PANE_FIRST,
							CWnd::reposQuery,
							rcFrameInner,
							rcFrameInner
							);
						ClientToScreen( &rcOwnClient );
						m_pDockSite->ClientToScreen( &rcFrameInner );
						g_rcHelperRowResizingLimits = rcOwnClient;
						switch( nDockBarID )
						{
						case AFX_IDW_DOCKBAR_TOP:
							g_rcHelperRowResizingLimits.bottom = rcFrameInner.bottom;
							break;
						case AFX_IDW_DOCKBAR_BOTTOM:
							g_rcHelperRowResizingLimits.top = rcFrameInner.top;
							break;
						case AFX_IDW_DOCKBAR_LEFT:
							g_rcHelperRowResizingLimits.right = rcFrameInner.right;
							break;
						case AFX_IDW_DOCKBAR_RIGHT:
							g_rcHelperRowResizingLimits.left = rcFrameInner.left;
							break;
						} // switch( nDockBarID )
					}
				} // if( pDockBar != NULL )
			} // if( pWnd != NULL )
		} // if( GetParentFrame() == m_pDockSite )

		POSITION pos = m_pDockSite->m_listControlBars.GetHeadPosition();
		for( ; pos != NULL; )
		{
			CControlBar * pBar = (CControlBar *)
				m_pDockSite->m_listControlBars.GetNext( pos );
			if(		pBar->GetSafeHwnd() == NULL
				||	pBar->IsDockBar()
				)
				continue;
			CExtMenuControlBar * pMenuBar =
				DYNAMIC_DOWNCAST( CExtMenuControlBar, pBar );
			if( pMenuBar == NULL )
				continue;
			pMenuBar->_CancelFlatTracking();
		} // for( ; pos != NULL; )
	} // if( m_pDockSite->GetSafeHwnd() != NULL )
	if( ! _RowResizingIsEnabled(this) )
		return;

	_OnControlBarPositionChange_InRowImpl(
		this,
		__ECBPC_ROW_RESIZING,
		true,
		false
		);

	m_bRowResizing = true;
	CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
	ASSERT_VALID( m_pDockSite );
	ASSERT( !IsFloating() );
	m_nMetricOffset = IsDockedHorizontally() ?
		m_sizeDockedH.cy : m_sizeDockedV.cx;
	if( !_IsShowContentWhenRowResizing() )
	{
		CPoint ptCursorScreen( 0, 0 );
		::GetCursorPos( &ptCursorScreen );
		m_nTrackerOffset =
			IsDockedHorizontally()
				? ptCursorScreen.y : ptCursorScreen.x;
		_DrawResizingTracker( false );
	} // if( !_IsShowContentWhenRowResizing() )

	while( m_bRowResizing )
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
			case WM_CANCELMODE:
//			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_NCLBUTTONUP:
//			case WM_NCLBUTTONDOWN:
			case WM_NCRBUTTONUP:
			case WM_NCRBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_NCMBUTTONDOWN:
			case WM_NCMBUTTONUP:
			case WM_CONTEXTMENU:
				bStop = true;
			break;
			default:
				if(	WM_KEYFIRST <= msg.message
					&& msg.message <= WM_KEYLAST
					)
					bStop = true;
//				else
//				{
//					if( CExtMouseCaptureSink::GetCapture() != GetSafeHwnd() )
//						bStop = true;
//					else
//					{
//						if(	(!CExtPopupMenuWnd::
//								IsKeyPressed(VK_LBUTTON))
//							|| CExtPopupMenuWnd::
//								IsKeyPressed(VK_RBUTTON)
//							|| CExtPopupMenuWnd::
//								IsKeyPressed(VK_MBUTTON)
//							)
//							bStop = true;
//					}
//				}
			break;
			} // switch( msg.message )
			if( bStop )
			{
				_RowResizingStop( true );
				break;
			}
			if( !AfxGetThread()->PumpMessage() )
			{
				PostQuitMessage(0);
				break; // Signal WM_QUIT received
			} // if( !AfxGetThread()->PumpMessage() )
		} // while( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		if( m_bRowResizing && CExtControlBar::g_bEnableOnIdleCalls )
		{
			for(	LONG nIdleCounter = 0L;
					::AfxGetThread()->OnIdle(nIdleCounter);
					nIdleCounter ++
					);
		}
	} // while( m_bRowResizing )

	_OnControlBarPositionChange_InRowImpl(
		this,
		__ECBPC_ROW_RESIZING,
		false,
		true
		);
}

void CExtControlBar::_RowResizingStop( bool bCancel )
{
	m_bRowResizing = false;
	if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
		CExtMouseCaptureSink::ReleaseCapture();
	ASSERT_VALID( m_pDockSite );
	ASSERT_VALID( m_pDockBar );
	ASSERT( !IsFloating() );

	if( !_IsShowContentWhenRowResizing() )
	{
		_DrawResizingTracker( true );

		if( !bCancel )
		{
			bool bHorz = IsDockedHorizontally();
			if(	m_nMetricOffset !=
					(bHorz ? m_sizeDockedH.cy : m_sizeDockedV.cx)
				)
			{
				ExtControlBarVector_t vBars;
				_GetRowExtBars( vBars );
				INT nCountOfBars = (INT)vBars.GetSize();
				ASSERT( nCountOfBars > 0 );
				LONG nDiff = 0;
				for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
				{
					CExtControlBar * pBar = vBars[ nBar ];
					ASSERT_VALID( pBar );
//					if( !pBar->IsVisible() )
//						continue;
					if( pBar->IsFixedMode() )
						continue;
					if( bHorz )
					{
						if( nDiff == 0 )
							nDiff = pBar->m_sizeDockedH.cy - m_nMetricOffset;
						pBar->m_sizeDockedH.cy = m_nMetricOffset;
					}
					else
					{
						if( nDiff == 0 )
							nDiff = pBar->m_sizeDockedV.cx - m_nMetricOffset;
						pBar->m_sizeDockedV.cx = m_nMetricOffset;
					}
				} // for( INT nBar = 0; nBar < nCountOfBars; nBar++ )

				if( nDiff != 0 )
				{
					UINT nDockBarID = GetSafeDockBarDlgCtrlID();
					ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );
					MfcControlBarVector_t vNextRow;
					if( nDockBarID == AFX_IDW_DOCKBAR_TOP
						|| nDockBarID == AFX_IDW_DOCKBAR_LEFT
						)
						_GetNextRowBars( true, vNextRow );
					else
						_GetNextRowBars( false, vNextRow );
					INT nCountOfBars = (INT)vNextRow.GetSize();
					for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
					{
						CExtControlBar * pBar = (CExtControlBar *)vNextRow[ nBar ];
						ASSERT_VALID( pBar );
						if( !pBar->IsKindOf( RUNTIME_CLASS(CExtControlBar) ) )
							continue;
						if( pBar->IsFixedMode() )
							continue;
						if( bHorz )
							pBar->m_sizeDockedH.cy += nDiff;
						else
							pBar->m_sizeDockedV.cx += nDiff;
					} // for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
				} // if( nDiff != 0 )
			}

//xx//
			//m_pDockSite->RecalcLayout();
			GetParentFrame()->RecalcLayout();
		} // if( !bCancel )

		ASSERT_VALID( m_pDockBar );
		m_pDockBar->RedrawWindow(
			NULL, NULL,
			RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASENOW
				|RDW_ALLCHILDREN
				|RDW_NOFRAME
			);

	} // if( !_IsShowContentWhenRowResizing() )
}

void CExtControlBar::_RowResizingUpdateState()
{
	ASSERT( m_bRowResizing );
	ASSERT_VALID( m_pDockSite );

CRect rcDockSite;
	m_pDockSite->GetWindowRect( &rcDockSite );
CSize sizeDockMax =
		rcDockSite.Size()
		- CSize(
			_CalcDesiredMinHW(),
			_CalcDesiredMinVH()
			);
	if( sizeDockMax.cx <= 0 || sizeDockMax.cy <= 0 )
		return;

CPoint ptCursorScreen( 0, 0 );
	::GetCursorPos( &ptCursorScreen );
	if( ptCursorScreen.x < g_rcHelperRowResizingLimits.left )
		ptCursorScreen.x = g_rcHelperRowResizingLimits.left;
	if( ptCursorScreen.x > g_rcHelperRowResizingLimits.right )
		ptCursorScreen.x = g_rcHelperRowResizingLimits.right;
	if( ptCursorScreen.y < g_rcHelperRowResizingLimits.top )
		ptCursorScreen.y = g_rcHelperRowResizingLimits.top;
	if( ptCursorScreen.y > g_rcHelperRowResizingLimits.bottom )
		ptCursorScreen.y = g_rcHelperRowResizingLimits.bottom;
CPoint ptCursor( ptCursorScreen );

CRect rcBarWnd;
	GetWindowRect( &rcBarWnd );
CPoint ptDevOffset = -rcBarWnd.TopLeft();
	ptCursor += ptDevOffset;


CRect rcRowResize = _RectRowResizeGet();
CRect rcRowRecalc = _RectRowRecalcGet();
CRect rcRowRecalcUp = _RectRowRecalcUpGet();
bool bHorz = IsDockedHorizontally();
	if( bHorz )
	{
		INT nMin1 = _CalcDesiredMinVH();
		bool bTop = IsDockedAtTop();
		INT nMin2 = bTop
			? (ptCursor.y + rcRowResize.Height()/2)
			: (m_sizeDockedH.cy - ptCursor.y + rcRowResize.Height()/2)
			;
		m_nMetricOffset = max( nMin1, nMin2 );
		if( m_nMetricOffset > sizeDockMax.cy )
			m_nMetricOffset = sizeDockMax.cy;
		if( m_nMetricOffset == m_sizeDockedH.cy )
			return;
	} // if( bHorz )
	else
	{
		INT nMin1 = _CalcDesiredMinHW();
		bool bLeft = IsDockedAtLeft();
		INT nMin2 = bLeft
			? (ptCursor.x + rcRowResize.Width()/2)
			: (m_sizeDockedV.cx - ptCursor.x + rcRowResize.Width()/2 )
			;
		m_nMetricOffset = max( nMin1, nMin2 );
		if( m_nMetricOffset > sizeDockMax.cx )
			m_nMetricOffset = sizeDockMax.cx;
		if( m_nMetricOffset == m_sizeDockedV.cx )
			return;
	} // else from if( bHorz )

bool bShowContent = _IsShowContentWhenRowResizing();
	if( bShowContent )
	{
		ExtControlBarVector_t vBars;
		_GetRowExtBars( vBars );
		INT nCountOfBars = (INT)vBars.GetSize();
		ASSERT( nCountOfBars > 0 );
		LONG nDiff = 0;
		for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
		{
			CExtControlBar * pBar = vBars[ nBar ];
			ASSERT_VALID( pBar );
//					if( !pBar->IsVisible() )
//						continue;
			if( pBar->IsFixedMode() )
				continue;
			if( bHorz )
			{
				if( nDiff == 0 )
					nDiff = pBar->m_sizeDockedH.cy - m_nMetricOffset;
				pBar->m_sizeDockedH.cy = m_nMetricOffset;
			}
			else
			{
				if( nDiff == 0 )
					nDiff = pBar->m_sizeDockedV.cx - m_nMetricOffset;
				pBar->m_sizeDockedV.cx = m_nMetricOffset;
			}
		} // for( INT nBar = 0; nBar < nCountOfBars; nBar++ )

/*
		if( nDiff != 0 )
		{
			UINT nDockBarID = GetSafeDockBarDlgCtrlID();
			ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );
			MfcControlBarVector_t vNextRow;
			if( nDockBarID == AFX_IDW_DOCKBAR_TOP
				|| nDockBarID == AFX_IDW_DOCKBAR_LEFT
				)
				_GetNextRowBars( true, vNextRow );
			else
				_GetNextRowBars( false, vNextRow );
			INT nCountOfBars = vNextRow.GetSize();
			for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
			{
				CExtControlBar * pBar = (CExtControlBar *)vNextRow[ nBar ];
				ASSERT_VALID( pBar );
				if( !pBar->IsKindOf( RUNTIME_CLASS(CExtControlBar) ) )
					continue;
				if( pBar->IsFixedMode() )
					continue;
				if( bHorz )
					pBar->m_sizeDockedH.cy += nDiff;
				else
					pBar->m_sizeDockedV.cx += nDiff;
			} // for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
		} // if( nDiff != 0 )
*/

		m_pDockSite->RecalcLayout();

		ASSERT_VALID( m_pDockBar );
		if( m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
			((CExtDockBar *)m_pDockBar)->OnDynamicLayoutUpdate();

		CExtPaintManager::stat_PassPaintMessages();

		_OnControlBarPositionChange_InRowImpl(
			this,
			__ECBPC_ROW_RESIZING,
			false,
			false
			);

		return;
	} // if( bShowContent )

	_DrawResizingTracker( true );
	m_nTrackerOffset =
		bHorz ? ptCursorScreen.y : ptCursorScreen.x;

	_OnControlBarPositionChange_InRowImpl(
		this,
		__ECBPC_ROW_RESIZING,
		false,
		false
		);

	_DrawResizingTracker( false );
}

bool CExtControlBar::g_bLockAllBars = false;

bool CExtControlBar::_DraggingIsEnabled(
	const CExtControlBar * pBarQuerySrc
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBarQuerySrc );
	pBarQuerySrc;
	return (! g_bLockAllBars );
}

bool CExtControlBar::_RowResizingIsEnabled(
	const CExtControlBar * pBarQuerySrc
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBarQuerySrc );
	pBarQuerySrc;
	return (! g_bLockAllBars );
}

bool CExtControlBar::_RowResizingGet() const
{
	ASSERT_VALID( this );
	return m_bRowResizing;
}

void CExtControlBar::_RowRecalcingStart()
{
	ASSERT( !m_bRowRecalcing );
	ASSERT( !m_bRowResizing );
	CWnd::CancelToolTips();
	CExtToolControlBar::_CloseTrackingMenus();
	CExtPopupMenuWnd::CancelMenuTracking();
	if( m_pDockSite->GetSafeHwnd() != NULL )
	{
		POSITION pos = m_pDockSite->m_listControlBars.GetHeadPosition();
		for( ; pos != NULL; )
		{
			CControlBar * pBar = (CControlBar *)
				m_pDockSite->m_listControlBars.GetNext( pos );
			if(		pBar->GetSafeHwnd() == NULL
				||	pBar->IsDockBar()
				)
				continue;
			CExtMenuControlBar * pMenuBar =
				DYNAMIC_DOWNCAST( CExtMenuControlBar, pBar );
			if( pMenuBar == NULL )
				continue;
			pMenuBar->_CancelFlatTracking();
		} // for( ; pos != NULL; )
	} // if( m_pDockSite->GetSafeHwnd() != NULL )
	if( ! _RowRecalcingIsEnabled(this) )
		return;

	_OnControlBarPositionChange_InRowImpl(
		this,
		__ECBPC_IN_ROW_RESIZING,
		true,
		false
		);

	m_bRowRecalcing = true;
    CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
	ASSERT_VALID( m_pDockSite );
	if( !_IsShowContentWhenRowRecalcing() )
	{
		CPoint ptCursorScreen( 0, 0 );
		::GetCursorPos( &ptCursorScreen );
		m_nTrackerOffset =
			IsDockedHorizontally()
				? ptCursorScreen.x
				: ptCursorScreen.y;
		m_nMetricOffset = 0;
		_DrawRecalcingTracker( false );
	} // if( !_IsShowContentWhenRowRecalcing() )

	while( m_bRowRecalcing )
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
			case WM_CANCELMODE:
//			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
//			case WM_NCLBUTTONUP:
			case WM_NCLBUTTONDOWN:
			case WM_NCRBUTTONUP:
			case WM_NCRBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_NCMBUTTONDOWN:
			case WM_NCMBUTTONUP:
			case WM_CONTEXTMENU:
				bStop = true;
			break;
			default:
				if(	WM_KEYFIRST <= msg.message
					&& msg.message <= WM_KEYLAST
					)
					bStop = true;
//				else
//				{
//					if( CExtMouseCaptureSink::GetCapture() != GetSafeHwnd() )
//						bStop = true;
//					else
//					{
//						if(	(!CExtPopupMenuWnd::
//								IsKeyPressed(VK_LBUTTON))
//							|| CExtPopupMenuWnd::
//								IsKeyPressed(VK_RBUTTON)
//							|| CExtPopupMenuWnd::
//								IsKeyPressed(VK_MBUTTON)
//							)
//							bStop = true;
//					}
//				}
			break;
			} // switch( msg.message )
			if( bStop )
			{
				_RowRecalcingStop( true );
				break;
			}
			if( !AfxGetThread()->PumpMessage() )
			{
				PostQuitMessage(0);
				break; // Signal WM_QUIT received
			} // if( !AfxGetThread()->PumpMessage() )
		} // while( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		if( m_bRowRecalcing && CExtControlBar::g_bEnableOnIdleCalls )
		{
			for(	LONG nIdleCounter = 0L;
					::AfxGetThread()->OnIdle(nIdleCounter);
					nIdleCounter ++
					);
		}
	} // while( m_bRowRecalcing )

	_OnControlBarPositionChange_InRowImpl(
		this,
		__ECBPC_IN_ROW_RESIZING,
		false,
		true
		);
}

void CExtControlBar::_RowRecalcingStop( bool bCancel )
{
	m_bRowRecalcing = false;
	if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
		CExtMouseCaptureSink::ReleaseCapture();
	ASSERT_VALID( m_pDockSite );
	ASSERT_VALID( m_pDockBar );
	ASSERT( !IsFloating() );

	if( !_IsShowContentWhenRowRecalcing() )
	{
		_DrawRecalcingTracker( true );

		if( !bCancel )
		{
			if( m_nMetricOffset != 0 )
			{
				bool bHorz = IsDockedHorizontally();
				ExtControlBarVector_t vBars;
				_GetRowExtBars( vBars );
				INT nCountOfBars = (INT)vBars.GetSize();
				ASSERT( nCountOfBars > 1 );
				bool bThisPassed = false;
				for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
				{
					CExtControlBar * pBar =
						vBars[
							m_bTopRecalcing
								? (nCountOfBars - nBar - 1)
								: nBar
							];
					ASSERT_VALID( pBar );
					if( pBar == this )
					{
						bThisPassed = true;
						continue;
					}
					if( !bThisPassed )
						continue;
					if( bHorz )
					{
						pBar->m_sizeDockedH.cx += m_nMetricOffset;
						if( pBar->m_sizeDockedH.cx < pBar->_CalcDesiredMinHW() )
						{
							m_sizeDockedH.cx -=
								pBar->_CalcDesiredMinHW() - pBar->m_sizeDockedH.cx;
							pBar->m_sizeDockedH.cx = pBar->_CalcDesiredMinHW();
						}
					} // if( bHorz )
					else
					{
						pBar->m_sizeDockedV.cy += m_nMetricOffset;
						if( pBar->m_sizeDockedV.cy < pBar->_CalcDesiredMinVH() )
						{
							m_sizeDockedV.cy -=
								pBar->_CalcDesiredMinVH() - pBar->m_sizeDockedV.cy;
							pBar->m_sizeDockedV.cy = pBar->_CalcDesiredMinVH();
						}
					} // else from if( bHorz )
					break;
				} // for( INT nBar = 0; nBar < nCountOfBars; nBar++ )

				if( bHorz )
				{
					m_sizeDockedH.cx -= m_nMetricOffset;
					if( m_sizeDockedH.cx < _CalcDesiredMinHW() )
					{
						m_sizeDockedH.cx +=
							_CalcDesiredMinHW() - m_sizeDockedH.cx;
						m_sizeDockedH.cx = _CalcDesiredMinHW();
					}
				} // if( bHorz )
				else
				{
					m_sizeDockedV.cy -= m_nMetricOffset;
					if( m_sizeDockedV.cy < _CalcDesiredMinVH() )
					{
						m_sizeDockedV.cy +=
							_CalcDesiredMinVH() - m_sizeDockedV.cy;
						m_sizeDockedV.cy = _CalcDesiredMinVH();
					}
				} // else from if( bHorz )

//xx//
				//m_pDockSite->RecalcLayout();
				GetParentFrame()->RecalcLayout();
			} // if( m_nMetricOffset != 0 )
		} // if( !bCancel )

		/*ASSERT_VALID( m_pDockBar );
		m_pDockBar->RedrawWindow(
			NULL, NULL,
			RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASENOW
				|RDW_ALLCHILDREN
				|RDW_NOFRAME
			);*/
	
	} // if( !_IsShowContentWhenRowRecalcing() )
}

void CExtControlBar::_RowRecalcingUpdateState()
{
	ASSERT( m_bRowRecalcing );
	ASSERT_VALID( m_pDockBar );
	ASSERT_VALID( m_pDockSite );

CPoint ptCursorScreen( 0, 0 );
	::GetCursorPos( &ptCursorScreen );
CPoint ptCursor( ptCursorScreen );

CRect rcBarWnd;
	GetWindowRect( &rcBarWnd );
CPoint ptDevOffset = -rcBarWnd.TopLeft();
	ptCursor += ptDevOffset;
CRect rcRowRecalc = _RectRowRecalcGet();
CRect rcRowRecalcUp = _RectRowRecalcUpGet();
bool bSysDragShowContent = _IsShowContentWhenRowRecalcing();
bool bHorz = IsDockedHorizontally();
	if( bHorz )
	{
		if( m_bTopRecalcing )
		{
			INT nMetric =
				max(
					_CalcDesiredMinHW(),
					(m_sizeDockedH.cx - ptCursor.x + rcRowRecalcUp.Width()/2)
					);
			m_nMetricOffset = m_sizeDockedH.cx - nMetric;
			if( m_nMetricOffset == 0 )
				return;
			if( bSysDragShowContent )
				m_sizeDockedH.cx = nMetric;
		} // if( m_bTopRecalcing )
		else
		{
			INT nMetric =
				max(
					_CalcDesiredMinHW(),
					(ptCursor.x + rcRowRecalc.Width()/2)
					);
			m_nMetricOffset = m_sizeDockedH.cx - nMetric;
			if( m_nMetricOffset == 0 )
				return;
			if( bSysDragShowContent )
				m_sizeDockedH.cx = nMetric;
		} // else from if( bTopRecalcing )
	} // if( bHorz )
	else
	{
		if( m_bTopRecalcing )
		{
			INT nMetric =
				max(
					_CalcDesiredMinVH(),
					(m_sizeDockedV.cy - ptCursor.y + rcRowRecalcUp.Height()/2)
					);
			m_nMetricOffset = m_sizeDockedV.cy - nMetric;
			if( m_nMetricOffset == 0 )
				return;
			if( bSysDragShowContent )
				m_sizeDockedV.cy = nMetric;
		} // if( m_bTopRecalcing )
		else
		{
			INT nMetric =
				max(
					_CalcDesiredMinVH(),
					(ptCursor.y + rcRowRecalc.Height()/2)
					);
			m_nMetricOffset = m_sizeDockedV.cy - nMetric;
			if( m_nMetricOffset == 0 )
				return;
			if( bSysDragShowContent )
				m_sizeDockedV.cy = nMetric;
		} // else from if( m_bTopRecalcing )
	} // else from if( bHorz )
	ASSERT( m_nMetricOffset != 0 );

	if( bSysDragShowContent )
	{
		ExtControlBarVector_t vBars;
		_GetRowExtBars( vBars );
		INT nCountOfBars = (INT)vBars.GetSize();
		ASSERT( nCountOfBars > 1 );
		bool bThisPassed = false;
		for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
		{
			CExtControlBar * pBar =
				vBars[
					m_bTopRecalcing
						? (nCountOfBars - nBar - 1)
						: nBar
					];
			ASSERT_VALID( pBar );
			if( pBar == this )
			{
				bThisPassed = true;
				continue;
			}
			if( !bThisPassed )
				continue;
			if( bHorz )
			{
				pBar->m_sizeDockedH.cx += m_nMetricOffset;
				if( pBar->m_sizeDockedH.cx < pBar->_CalcDesiredMinHW() )
				{
					m_sizeDockedH.cx -=
						pBar->_CalcDesiredMinHW() - pBar->m_sizeDockedH.cx;
					pBar->m_sizeDockedH.cx = pBar->_CalcDesiredMinHW();
				}
			} // if( bHorz )
			else
			{
				pBar->m_sizeDockedV.cy += m_nMetricOffset;
				if( pBar->m_sizeDockedV.cy < pBar->_CalcDesiredMinVH() )
				{
					m_sizeDockedV.cy -=
						pBar->_CalcDesiredMinVH() - pBar->m_sizeDockedV.cy;
					pBar->m_sizeDockedV.cy = pBar->_CalcDesiredMinVH();
				}
			} // else from if( bHorz )
			break;
		} // for( INT nBar = 0; nBar < nCountOfBars; nBar++ )

		m_pDockSite->RecalcLayout();

		_OnControlBarPositionChange_InRowImpl(
			this,
			__ECBPC_IN_ROW_RESIZING,
			false,
			false
			);

		ASSERT_VALID( m_pDockBar );
		if( m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
			((CExtDockBar *)m_pDockBar)->OnDynamicLayoutUpdate();
		
		return;
	} // if( bSysDragShowContent )

	ASSERT_VALID( m_pDockBar );
CRect rcDockBarWnd;
	m_pDockBar->GetWindowRect( &rcDockBarWnd );

INT nMinTrackerOffset = bHorz ?
		rcDockBarWnd.left : rcDockBarWnd.top;

	if( m_bTopRecalcing )
	{
		MfcControlBarVector_t vBars;
		_GetRowBars( vBars );
		INT nCountOfBars = (INT)vBars.GetSize();
		for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
		{
			CControlBar * pBar = vBars[ nBar ];
			ASSERT_VALID( pBar );
			if( !pBar->IsVisible() )
				continue;
			CExtControlBar * pExtBar =
				DYNAMIC_DOWNCAST( CExtControlBar, pBar );
			if( pExtBar != NULL && pExtBar->IsFixedMode() )
				pExtBar = NULL;
			if( pExtBar == NULL )
			{
				CRect rcBarWnd;
				pBar->GetWindowRect( &rcBarWnd );
				nMinTrackerOffset += bHorz ?
					rcBarWnd.Width() : rcBarWnd.Height();
			} // if( pExtBar == NULL )
			else
			{
				nMinTrackerOffset += bHorz
					? pExtBar->_CalcDesiredMinHW()
					: pExtBar->_CalcDesiredMinVH();
			} // else from if( pExtBar == NULL )
			if( pBar == this )
				break;
		} // for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
	} // if( m_bTopRecalcing )
	else
	{
		nMinTrackerOffset = bHorz
			? rcBarWnd.left + _CalcDesiredMinHW()
			: rcBarWnd.top  + _CalcDesiredMinVH()
			;
	} // else from if( m_bTopRecalcing )

INT nMaxTrackerOffset = 0;

	if( m_bTopRecalcing )
	{
		nMaxTrackerOffset = bHorz
			? rcBarWnd.right  - _CalcDesiredMinHW()
			: rcBarWnd.bottom - _CalcDesiredMinVH()
			;
	} // if( m_bTopRecalcing )
	else
	{
		nMaxTrackerOffset = bHorz ?
			rcDockBarWnd.right : rcDockBarWnd.bottom;
		MfcControlBarVector_t vBars;
			_GetRowBars( vBars );
		INT nCountOfBars = (INT)vBars.GetSize();
		for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
		{
			CControlBar * pBar = vBars[ nCountOfBars - nBar - 1 ];
			ASSERT_VALID( pBar );
			if( !pBar->IsVisible() )
				continue;
			if( pBar == this )
				break;
			CExtControlBar * pExtBar =
				DYNAMIC_DOWNCAST( CExtControlBar, pBar );
			if( pExtBar != NULL && pExtBar->IsFixedMode() )
				pExtBar = NULL;
			if( pExtBar == NULL )
			{
				CRect rcBarWnd;
				pBar->GetWindowRect( &rcBarWnd );
				nMaxTrackerOffset -= bHorz ?
					rcBarWnd.Width() : rcBarWnd.Height();
			} // if( pExtBar == NULL )
			else
			{
				nMaxTrackerOffset -= bHorz
					? pExtBar->_CalcDesiredMinHW()
					: pExtBar->_CalcDesiredMinVH();
			} // else from if( pExtBar == NULL )
		} // for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
	} // else from if( m_bTopRecalcing )

	_DrawRecalcingTracker( true );
	m_nTrackerOffset = bHorz
		? ptCursorScreen.x
		: ptCursorScreen.y;
	ASSERT( nMinTrackerOffset <= nMaxTrackerOffset );
	if( m_nTrackerOffset < nMinTrackerOffset )
		m_nTrackerOffset = nMinTrackerOffset;
	if( m_nTrackerOffset > nMaxTrackerOffset )
		m_nTrackerOffset = nMaxTrackerOffset;

	_OnControlBarPositionChange_InRowImpl(
		this,
		__ECBPC_IN_ROW_RESIZING,
		false,
		false
		);

	_DrawRecalcingTracker( false );
}

bool CExtControlBar::_RowRecalcingIsEnabled(
	const CExtControlBar * pBarQuerySrc
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBarQuerySrc );
	pBarQuerySrc;
	return (! g_bLockAllBars );
}

bool CExtControlBar::_RowRecalcingGet() const
{
	ASSERT_VALID( this );
	return m_bRowRecalcing;
}

void CExtControlBar::_DrawResizingTracker( bool bErasingPrevious )
{
	ASSERT_VALID( this );
	ASSERT( !IsFloating() );
	ASSERT_VALID( m_pDockSite );
	ASSERT_VALID( m_pDockBar );
CRect rcDockBar;
	m_pDockBar->GetWindowRect( &rcDockBar );
	if( rcDockBar.IsRectEmpty() )
		return;
CRect rcRowResize = _RectRowResizeGet();
CRect rcTracker( rcDockBar );
	if( IsDockedHorizontally() )
	{
		rcTracker.top =
			m_nTrackerOffset - rcRowResize.Height()/2;
		rcTracker.bottom =
			m_nTrackerOffset + rcRowResize.Height()/2;
	} // if( IsDockedHorizontally() )
	else
	{
		rcTracker.left =
			m_nTrackerOffset - rcRowResize.Width()/2;
		rcTracker.right =
			m_nTrackerOffset + rcRowResize.Width()/2;
	} // else from if( IsDockedHorizontally() )
CFrameWnd * pFrame = GetDockingFrame(); // m_pDockSite;
	ASSERT_VALID( pFrame );
CRect rcDockingFrame;
	pFrame->GetWindowRect( &rcDockingFrame );
	rcTracker.OffsetRect( -rcDockingFrame.TopLeft() );
CDC * pDC = pFrame->GetDCEx( NULL, __DOCKSITE_DCX_FLAGS );
DWORD dwOldLayout = pDC->SetLayout( LAYOUT_LTR );
MsgDrawTrackedAreaData_t _MsgDrawTrackedAreaData(
		*this,
		*pDC,
		__EDTAT_RESIZING_ENTIRE_ROW,
		rcTracker,
		bErasingPrevious
		);
	_MsgDrawTrackedAreaData.NotifyOrDrawDefault();
	pDC->SetLayout( dwOldLayout );
    pFrame->ReleaseDC(pDC);
}

void CExtControlBar::_DrawRecalcingTracker( bool bErasingPrevious )
{
	ASSERT_VALID( this );
	ASSERT( !IsFloating() );
	ASSERT_VALID( m_pDockSite );
	ASSERT_VALID( m_pDockBar );
CRect rcRowRecalc = _RectRowRecalcGet();
CRect rcRowRecalcUp = _RectRowRecalcUpGet();
CRect rcTracker;
	GetWindowRect( &rcTracker );
	if( IsDockedHorizontally() )
	{
		INT nMetric =
			m_bTopRecalcing
				? rcRowRecalcUp.Width()/2
				: rcRowRecalc.Width()/2;
		rcTracker.left =
			m_nTrackerOffset - nMetric;
		rcTracker.right =
			m_nTrackerOffset + nMetric;
	} // if( IsDockedHorizontally() )
	else
	{
		INT nMetric =
			m_bTopRecalcing
				? rcRowRecalcUp.Height()/2
				: rcRowRecalc.Height()/2;
		rcTracker.top =
			m_nTrackerOffset - nMetric;
		rcTracker.bottom =
			m_nTrackerOffset + nMetric;
	} // else from if( IsDockedHorizontally() )
CFrameWnd * pFrame = GetDockingFrame(); // m_pDockSite;
	ASSERT_VALID( pFrame );
CRect rcDockingFrame;
	pFrame->GetWindowRect( &rcDockingFrame );
	rcTracker.OffsetRect( -rcDockingFrame.TopLeft() );
CDC * pDC = pFrame->GetDCEx( NULL, __DOCKSITE_DCX_FLAGS );
DWORD dwOldLayout = pDC->SetLayout( LAYOUT_LTR );
MsgDrawTrackedAreaData_t _MsgDrawTrackedAreaData(
		*this,
		*pDC,
		__EDTAT_RESIZING_ENTIRE_ROW,
		rcTracker,
		bErasingPrevious
		);
	_MsgDrawTrackedAreaData.NotifyOrDrawDefault();
	pDC->SetLayout( dwOldLayout );
    pFrame->ReleaseDC(pDC);
}

bool CExtControlBar::IsOleIpObjActive(
	CFrameWnd * pFrameSearch
	)
{
	if( FindPrintPreviewMode( pFrameSearch ) )
		return false;
	if( pFrameSearch == NULL )
	{
		pFrameSearch =
			DYNAMIC_DOWNCAST( CFrameWnd, AfxGetMainWnd() );
		if( pFrameSearch == NULL )
			return false;
	}
	//ASSERT_VALID( pFrameSearch );
	if( pFrameSearch->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)) )
	{
		pFrameSearch = pFrameSearch->GetParentFrame();
		ASSERT_VALID( pFrameSearch );
		ASSERT( !pFrameSearch->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)) );
	}
CMDIFrameWnd * pFrameSearchMDI =
		DYNAMIC_DOWNCAST(
			CMDIFrameWnd,
			pFrameSearch
			);
	if(		pFrameSearchMDI != NULL
		&&	pFrameSearchMDI->m_hWndMDIClient != NULL
		&&	::IsWindow( pFrameSearchMDI->m_hWndMDIClient )
		)
	{
//		pFrameSearch = NULL;
//		BOOL bMaximized = FALSE;
//		HWND hWndActiveFrame = (HWND)
//			::SendMessage(
//				pFrameSearchMDI->m_hWndMDIClient,
//				WM_MDIGETACTIVE,
//				0,
//				(LPARAM)&bMaximized
//				);
//		if( hWndActiveFrame != NULL )
//		{
//			CWnd * pWndPermanent =
//				CWnd::FromHandlePermanent( hWndActiveFrame );
//			if( pWndPermanent != NULL )
//			{
//				CMDIChildWnd * pWndMDIChild =
//					DYNAMIC_DOWNCAST(
//						CMDIChildWnd,
//						pWndPermanent
//						);
//				if(		pWndMDIChild != NULL
//					&&	(! pWndMDIChild->m_bPseudoInactive )
//					&&	(pWndMDIChild->GetStyle()&WS_VISIBLE) != 0
//					)
//					pFrameSearch = pWndMDIChild;
//			}
//		}
		pFrameSearch = ((CMDIFrameWnd*)pFrameSearch)->GetActiveFrame();
		if( pFrameSearch == NULL )
			return false;
	}
bool bOleInplaceItemActivated = false;
CView * pActiveView = pFrameSearch->GetActiveView();
	if(		pActiveView != NULL
		&&	(! pActiveView->IsKindOf(RUNTIME_CLASS(CPreviewView)) )
		)
	{
		ASSERT_VALID( pActiveView );
		CDocument * pActiveDoc = pActiveView->GetDocument();
		if( pActiveDoc != NULL )
		{
			ASSERT_VALID( pActiveDoc );
			COleDocument * pActiveOleDoc = 
				DYNAMIC_DOWNCAST(COleDocument,pActiveDoc);
			if( pActiveOleDoc != NULL )
			{
				ASSERT_VALID( pActiveOleDoc );
				COleClientItem * pItem =
					pActiveOleDoc->GetInPlaceActiveItem(pActiveView);
				if( pItem != NULL )
					bOleInplaceItemActivated = true;
			} // if( pActiveOleDoc != NULL )
		} // if( pActiveDoc != NULL )
	} // if( pActiveView != NULL ...
	return bOleInplaceItemActivated;
}

bool CExtControlBar::IsOleIpObjActive() const
{
	ASSERT_VALID( this );
	return IsOleIpObjActive( _GetDockingFrameImpl() );
}

bool CExtControlBar::SafeDisplayBar()
{
	if( m_bPresubclassDialogMode )
	{
		if( GetStyle() & WS_VISIBLE )
			return true;
		return false;
	}
	if( IsVisible() )
	{
		if( (m_nStateFlags & delayShow) == 0 )
			return true;
	} // if( IsVisible() )
	if( m_nStateFlags &
			//(CControlBar::delayHide | CControlBar::tempHide)
			CControlBar::tempHide
		)
		return false;
CFrameWnd * pFrame =
		_GetDockingFrameImpl();
	ASSERT_VALID( pFrame );
bool bOleInplaceItemActivated =
		//CExtControlBar::IsOleIpObjActive( pFrame );
		IsOleIpObjActive();
	if( bOleInplaceItemActivated &&
			(m_dwStyle & CBRS_HIDE_INPLACE)
		)
		return false;
//	pFrame->ShowControlBar( this, TRUE, TRUE );
//	pFrame = GetParentFrame();
//	ASSERT_VALID( pFrame );
//	pFrame->RecalcLayout();

	CExtControlBar::DoFrameBarCheckCmd(
		pFrame,
		UINT(GetDlgCtrlID())
		);

HWND hWndThis = GetSafeHwnd();
	CExtPopupMenuWnd::PassMsgLoop(false);
	if( ! ::IsWindow(hWndThis) )
		return false;
	if( !IsVisible() )
	{
		ASSERT( FALSE );
		return false;
	}
	return true;
}

bool CExtControlBar::IsSingleOnRow(
	bool bOnlyFixed, // = false
	bool bOnlyNonFixed // = false
	) const
{
	ASSERT_VALID( this );
	ASSERT( !IsFloating() );
	ASSERT_VALID( m_pDockBar );
INT nTotalDockCount = (INT)m_pDockBar->m_arrBars.GetSize();
bool bThisFound = false;
INT nCountInRow = 0;
	for( INT nBar = 0; nBar < nTotalDockCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pDockBar->m_arrBars[ nBar ];
		if( pBar == NULL )
		{
			if( bThisFound )
				break;
			nCountInRow = 0;
			continue;
		}
		if( __PLACEHODLER_BAR_PTR( pBar ) )
			continue;
		ASSERT_KINDOF( CControlBar, pBar );

		if( pBar == (CControlBar *)this )
			bThisFound = true;

		if( !pBar->IsVisible() )
			continue;

		if( bOnlyFixed || bOnlyNonFixed )
		{
			bool bFixed = true;
			if( pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
				&& !((CExtControlBar *)pBar)->IsFixedMode()
				)
				bFixed = false;
			if( bOnlyFixed && !bFixed )
				continue;
			if( !bOnlyFixed && bFixed )
				continue;
		} // if( bOnlyFixed || bOnlyNonFixed )

		nCountInRow++;

		if( bThisFound && nCountInRow > 1 )
			return false;
	} // for( INT nBar = 0; nBar < nTotalDockCount; nBar++ )

//	ASSERT( nCountInRow >= 1 );
	return (nCountInRow == 1) ? true : false;
}

void CExtControlBar::MakeSingleOnRow()
{
CFrameWnd * pFrame = GetDockingFrame();
	ASSERT_VALID( pFrame );
	pFrame->RecalcLayout();
	ASSERT_VALID( m_pDockBar );
int nPos = m_pDockBar->FindBar( this );
	ASSERT( 1 <= nPos && nPos < m_pDockBar->m_arrBars.GetSize() );
	if( nPos < m_pDockBar->m_arrBars.GetSize()
		&&	m_pDockBar->m_arrBars[nPos+1] != NULL
		)
		m_pDockBar->m_arrBars.InsertAt(
			nPos+1, ((CControlBar*)NULL) );
	if( m_pDockBar->m_arrBars[nPos-1] != NULL )
		m_pDockBar->m_arrBars.InsertAt(
			nPos, ((CControlBar*)NULL) );
	if( IsDockedHorizontally() )
		m_sizeDockedH = CSize( MAXSHORT, m_sizeDockedH.cy );
	else
		m_sizeDockedV = CSize( m_sizeDockedV.cx, MAXSHORT );
	pFrame->RecalcLayout();
}

bool CExtControlBar::IsMinimizedOnRow() const
{
	if(	IsDockedVertically()
		&& m_sizeDockedV.cy == _CalcDesiredMinVH()
		)
		return true;
	if(	IsDockedHorizontally()
		&& m_sizeDockedH.cx == _CalcDesiredMinHW()
		)
		return true;
	return false;
}

bool CExtControlBar::IsMaximizedOnRow() const
{
	if( IsFloating() )
		return false;
	if(IsMinimizedOnRow())
		return false;
ExtControlBarVector_t vBars;
	( const_cast < CExtControlBar * > ( this ) ) ->
		_GetRowExtBars(
			vBars
			);
INT nCountOfBars = (INT)vBars.GetSize();
	for( int nBar = 0; nBar < nCountOfBars; nBar++ )
	{
		CExtControlBar * pBar = vBars[ nBar ];
		ASSERT( pBar != NULL );
		if( pBar == this )
			continue;
		if( !(pBar->IsMinimizedOnRow()) )
			return false;
	}
	return true;
}

void CExtControlBar::MinimizeOnRow()
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pDockSite );
	ASSERT_VALID( m_pDockBar );
	ASSERT( GetSafeHwnd() != NULL );
	if( IsFloating () || IsSingleOnRow() )
		return;
ExtControlBarVector_t vBars;
    _GetRowExtBars(vBars);
INT nCountOfBars = (INT)vBars.GetSize();
	if( nCountOfBars <= 1 )
		return;
CExtDockBar * pDockBar =
		(CExtDockBar*) m_pDockBar;
	ASSERT( pDockBar != NULL );
BOOL bHorz = IsDockedHorizontally();

INT nCalcExtentMax = 0;
	if( bHorz )
	{
		INT nMin = _CalcDesiredMinHW();
		nCalcExtentMax = m_sizeDockedH.cx - nMin;
		m_sizeDockedH.cx = nMin;
		ASSERT( nCalcExtentMax >= 0 );
	}
	else
	{
		INT nMin = _CalcDesiredMinVH();
		nCalcExtentMax = m_sizeDockedV.cy - nMin;
		m_sizeDockedV.cy = nMin;
		ASSERT( nCalcExtentMax >= 0 );
	}

	for( INT nBar = 0; nCalcExtentMax > 0; nBar++ )
	{
		if( nBar == nCountOfBars )
			nBar = 0;
		CExtControlBar * pBar = vBars[ nBar ];
		ASSERT_VALID( pBar );
		if( pBar != this )
		{
			if( bHorz )
				pBar->m_sizeDockedH.cx++;
			else
				pBar->m_sizeDockedV.cy++;
			nCalcExtentMax--;
		} // if( pBar != this )
	} // for( nBar = 0; nCalcExtentMax > 0; nBar++ )
	GetParentFrame()->RecalcLayout();
	//_GetDockingFrameImpl()->SetFocus();
}

void CExtControlBar::MaximizeOnRow()
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pDockSite );
	ASSERT_VALID( m_pDockBar );
	ASSERT( GetSafeHwnd() != NULL );
	if( IsFloating () || IsSingleOnRow() )
		return;
ExtControlBarVector_t vBars;
    _GetRowExtBars(vBars);
INT nCountOfBars = (INT)vBars.GetSize();
	if( nCountOfBars <= 1 )
		return;
CExtDockBar * pDockBar =
		(CExtDockBar*) m_pDockBar;
	ASSERT( pDockBar != NULL );
BOOL bHorz = IsDockedHorizontally();
INT nMetric = 0;
	for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
	{
		CExtControlBar * pBar = vBars[ nBar ];
		ASSERT_VALID( pBar );
		if( pBar == this )
			continue;
		if( bHorz )
		{
			INT nMin = pBar->_CalcDesiredMinHW();
			nMetric += pBar->m_sizeDockedH.cx - nMin;
			pBar->m_sizeDockedH.cx = nMin;
		}
		else
		{
			INT nMin = pBar->_CalcDesiredMinVH();
			nMetric += pBar->m_sizeDockedV.cy - nMin;
			pBar->m_sizeDockedV.cy = nMin;
		}
	} // for( INT nBar = 0; nBar < nCountOfBars; nBar++ )
	if( bHorz )
		m_sizeDockedH.cx += nMetric;
	else
		m_sizeDockedV.cy += nMetric;
	GetParentFrame()->RecalcLayout();
}

void CExtControlBar::OnRepositionSingleChild(
	int cx, // = -1, // if negative - get from client area
	int cy, // = -1
	bool bRedraw // = TRUE
	)
{
	if( ! m_bReposSingleChildMode )
		return;
    // automatic child resizing - only one child is allowed
HWND hWndChild = ::GetWindow( m_hWnd, GW_CHILD );
    if( hWndChild == NULL )
		return;
	ASSERT( ::IsWindow( hWndChild ) );
	ASSERT( ::GetWindow( hWndChild, GW_HWNDNEXT ) == NULL );
CRect rcClient;
	if( cx < 0 || cy < 0 )
		GetClientRect( &rcClient );
	else
		rcClient.SetRect( 0, 0, cx, cy );
	if( IsFloating() )
	{
//		CSize sizeFrame(
//			::GetSystemMetrics(SM_CXSIZEFRAME),
//			::GetSystemMetrics(SM_CYSIZEFRAME)
//			);
//		rcClient.DeflateRect( sizeFrame );
		rcClient.DeflateRect( 1, 1 );
	} // if( IsFloating() )
CRect rcChild;
	::GetWindowRect( hWndChild, &rcChild );
	ScreenToClient( &rcChild );
	if( rcChild == rcClient )
		return;
	::MoveWindow(
		hWndChild,
		rcClient.left,
		rcClient.top,
		rcClient.Width(),
		rcClient.Height(),
		bRedraw ? TRUE : FALSE
		);
}

void CExtControlBar::OnSize(UINT nType, int cx, int cy) 
{
	nType;
	if( m_bUpdatingChain )
		return;
	OnRepositionSingleChild( cx, cy, true );
CFrameWnd * pFrame = GetParentFrame();
	if(		pFrame != NULL
		&&	pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd))
		)
	{
		OnNcAreaButtonsReposition();
		pFrame->SendMessage( WM_NCPAINT );
	}
}

void CExtControlBar::_Dragging_OnStart()
{
	CWnd::CancelToolTips();
	CExtPopupMenuWnd::CancelMenuTracking();

	ASSERT_VALID( m_pDockBar );
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
CExtDockDynTabBar * pTabbedDockBar =
		DYNAMIC_DOWNCAST(
			CExtDockDynTabBar,
			m_pDockBar
			);
	if( pTabbedDockBar != NULL )
	{
		CExtDynTabControlBar * pTabbedBar =
			STATIC_DOWNCAST(
				CExtDynTabControlBar,
				pTabbedDockBar->GetParent()
				);
		ASSERT_VALID( pTabbedBar );
		pTabbedBar->SaveIndex( this );
	}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	GetOwner()->
		SendMessage(
			WM_SETMESSAGESTRING,
			IDS_EXTSTATUS_PREVENT_DOCKING
			);
}

void CExtControlBar::_Dragging_OnStop(
	InternalDraggingState_t & _dsOld,
	InternalDraggingState_t & _dsNew,
	bool bCancel
	)
{
	_dsOld, _dsNew, bCancel;

	GetOwner()->
		SendMessage(
			WM_SETMESSAGESTRING,
			AFX_IDS_IDLEMESSAGE
			);

	ASSERT_VALID( this );
	if( IsFixedMode() )
		return;

	ASSERT_VALID( (&_dsOld) );
	ASSERT_VALID( (&_dsNew) );
	ASSERT( _dsNew.ExtBarSrcGet() == this );

	ASSERT_VALID( m_pDockBar );

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

CExtDockDynTabBar * pTabbedDockBar =
		DYNAMIC_DOWNCAST(
			CExtDockDynTabBar,
			m_pDockBar
			);
	if( pTabbedDockBar != NULL )
	{
		CExtDynTabControlBar * pTabbedBar =
			STATIC_DOWNCAST(
				CExtDynTabControlBar,
				pTabbedDockBar->GetParent()
				);
		ASSERT_VALID( pTabbedBar );
		pTabbedBar->RemoveTemporaryItems( false );
		if( bCancel )
		{
			pTabbedBar->RestoreIndex( this );
			pTabbedBar->SelectControlBar( this );
		} // if( bCancel )
	} // if( pTabbedDockBar != NULL )

	if( bCancel && _dsNew.ExtBarDstGet() != NULL )
	{
		CExtDynTabControlBar * pTabbedBar =
			DYNAMIC_DOWNCAST(
				CExtDynTabControlBar,
				_dsNew.ExtBarDstGet()
				);
		if( pTabbedBar != NULL )
		{
			pTabbedBar->RemoveTemporaryItems( false );
		}
	} // if( bCancel && _dsNew.ExtBarDstGet() != NULL )

#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

}

void CExtControlBar::_SetCursor(
	const CPoint & point
	)
{
bool bCustomizeMode = false;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = NotifiedCustomizeSiteGet();
	if( pSite != NULL )
	{
		if( pSite->DraggedNodeGet() != NULL )
			return;
		bCustomizeMode = pSite->IsCustomizeMode();
	} // if( pSite != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( ! bCustomizeMode )
	{
		if( CExtControlBar::FindHelpMode(this) )
			return;
	} // if( ! bCustomizeMode )
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	else
	{
		ASSERT( ! CExtControlBar::FindHelpMode(this) );
	} // else from if( !bCustomizeMode )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

HCURSOR hCursor = NULL;
	if( ! m_bPresubclassDialogMode )
	{
		if( m_bDragging || m_bDragDetecting || m_bRowResizing || m_bRowRecalcing )
		{ // if can analyze resize cursors
			if( m_bDragging || m_bDragDetecting && ( ! ( m_bRowResizing || m_bRowRecalcing ) ) )
			{
				//ASSERT( ! ( m_bRowResizing || m_bRowRecalcing ) );
				hCursor = g_hCursorArrow;
				if( ( m_bDragDetecting || m_bDragging ) && IsFixedDockStyle() )
				{
					if( m_bDragging )
						hCursor = g_hCursorDrag;
					else
					{
						CRect rcBarWnd;
						GetWindowRect( &rcBarWnd );
						CPoint ptDevOffset = -rcBarWnd.TopLeft();
						CPoint ptWnd; //( point );
						//ClientToScreen( &ptWnd );
						::GetCursorPos( &ptWnd );
						ptWnd += ptDevOffset;
						CRect rcGrip = _RectGripGet();
						if( rcGrip.PtInRect( ptWnd ) )
							hCursor = g_hCursorDrag;
					}
				}
				else if( IsFixedDockStyle() )
				{
					e_calc_fixed_resizing_t eCFR =
						g_DragStateOld.GetCFR();
					if( eCFR == __ECFR_HORZ )
						hCursor = g_hCursorResizeH;
					else if( eCFR == __ECFR_VERT )
						hCursor = g_hCursorResizeV;
					else
						hCursor = g_hCursorDrag;
					ASSERT( hCursor != NULL );
				}
			} // if( m_bDragging || m_bDragDetecting && ( ! ( m_bRowResizing || m_bRowRecalcing ) ) )
			else
			{
				ASSERT( ! m_bDragging );
				ASSERT( ! IsFloating() );
				if( IsDockedVertically() )
				{
					if( m_bRowRecalcing )
					{
						ASSERT( ! m_bRowResizing );
						hCursor = g_hCursorResizeV;
						ASSERT( hCursor != NULL );
					} // if( m_bRowRecalcing )
					else
					{
						ASSERT( ! m_bRowRecalcing );
						hCursor = g_hCursorResizeH;
						ASSERT( hCursor != NULL );
					} // else from if( m_bRowRecalcing )
				} // if( IsDockedVertically() )
				else
				{
					if( m_bRowRecalcing )
					{
						ASSERT( ! m_bRowResizing );
						hCursor = g_hCursorResizeH;
						ASSERT( hCursor != NULL );
					} // if( m_bRowRecalcing )
					else
					{
						ASSERT( ! m_bRowRecalcing );
						hCursor = g_hCursorResizeV;
						ASSERT( hCursor != NULL );
					} // else if( m_bRowRecalcing )
				} // else from if( IsDockedVertically() )
			} // else from if( m_bDragging || m_bDragDetecting && ( ! ( m_bRowResizing || m_bRowRecalcing ) ) )
		} // if can analyze resize cursors
		else
		{ // if can not analyze resize cursors

			if( ! IsFloating() )
			{
				CRect rcBarWnd;
				GetWindowRect( &rcBarWnd );
				CPoint ptDevOffset = -rcBarWnd.TopLeft();
				CPoint ptWnd( point );
				ClientToScreen( &ptWnd );
				ptWnd += ptDevOffset;
				CRect rcGrip = _RectGripGet();
				if( rcGrip.PtInRect(ptWnd) )
				{
					INT nCountOfNcButtons = NcButtons_GetCount();
					for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
					{
						CExtBarNcAreaButton * pBtn =
							NcButtons_GetAt( nBtn );
						hCursor =
							pBtn->OnNcAreaQueryCursor( ptWnd );
						if( hCursor != NULL )
							break;
					} // for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
					if( hCursor == NULL )
					{
						if( _DraggingIsEnabled( this ) )
						{
							if( IsFixedDockStyle() )
								hCursor = g_hCursorDrag;
							else
								hCursor = g_hCursorArrow;
						}
						else
							hCursor = g_hCursorArrow;
						ASSERT( hCursor != NULL );
					} // if( hCursor == NULL )
				} // if( rcGrip.PtInRect(ptWnd) )
			} // if( !IsFloating() )
		
		} // if can not analyze resize cursors
	} // if( ! m_bPresubclassDialogMode )

	if( hCursor == NULL )
	{
		hCursor = (HCURSOR)(__EXT_MFC_DWORD_PTR)
			::__EXT_MFC_GetClassLong(
				m_hWnd,
				__EXT_MFC_GCL_HCURSOR
				);
		ASSERT( hCursor != NULL );
	} // if( hCursor == NULL )
	
	if( ::GetCursor() != hCursor )
		::SetCursor( hCursor );
}

CExtControlBar * CExtControlBar::_DraggingGetBar() // return currently dragging control bar
{
	if( g_DragStateOld.IsEmpty() )
		return NULL;
	return g_DragStateOld.ExtBarSrcGet();
}

bool CExtControlBar::_DraggingCancel() // returns true if dragging was really canceled
{
CExtControlBar * pBar = _DraggingGetBar();
	if( pBar == NULL )
		return false;
	pBar->_DraggingStop( true );
	return true;
}

void CExtControlBar::_DraggingStart(
	const CPoint & point,
	const CPoint & pointOffset, // = CPoint( 0, 0 );
	CSize sizeWaitMouseMove // = CSize( 1, 1 )
	)
{
	ASSERT( !m_bRowRecalcing );
	ASSERT( !m_bRowResizing );
	ASSERT( !m_bDragging );

	ASSERT_VALID( m_pDockSite );
	if( CExtControlBar::ProfileBarStateIsSerializing( m_pDockSite ) )
		return;

	if( ! _DraggingIsEnabled( this ) )
		return;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = NotifiedCustomizeSiteGet();
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		pSite->DropTargetPopupCancelEx( -2 );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	// (+ v.2.24)
	// detect valid dragging start event
MSG msg;
	for( ; PeekMessage( &msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE ); );
HWND hWndOwn = GetSafeHwnd();
	ASSERT( hWndOwn != NULL && ::IsWindow(hWndOwn) );
CExtMouseCaptureSink::SetCapture( hWndOwn );
	m_bDragDetecting = true;
	_SetCursor( point );
//sizeWaitMouseMove.cx = sizeWaitMouseMove.cy = 5; // debug
	if( sizeWaitMouseMove.cx > 0 && sizeWaitMouseMove.cy > 0 )
	{
		::WaitMessage();
		CPoint ptMouseStart( 0, 0 ), ptMouseCurrent( 0, 0 );
		::GetCursorPos( &ptMouseStart );
		bool bEnableStartDragging = true, bMouseShiftDetected = false;
		for( ; bEnableStartDragging && (!bMouseShiftDetected); )
		{
			for( 	;	::IsWindow(hWndOwn)
					&&	PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE );
					)
			{ // wait any mouse movement look
				MSG _msgOwnLBtnUp;
				::PeekMessage(
					&_msgOwnLBtnUp,
					hWndOwn,
					WM_LBUTTONUP,
					WM_LBUTTONUP,
					PM_NOREMOVE
					);
				if( _msgOwnLBtnUp.message == WM_LBUTTONUP )
				{
					bEnableStartDragging = false;
					CWnd * pWndChild = GetWindow( GW_CHILD );
					if( pWndChild != NULL )
					{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
						CExtDynTabControlBar * pTabbedBar =
						DYNAMIC_DOWNCAST( CExtDynTabControlBar, this );
						if( pTabbedBar != NULL )
						{
							LONG nSel = pTabbedBar->GetSwitcherSelection();
							if( nSel >= 0 )
							{
								CExtControlBar * pBar =
									pTabbedBar->GetBarAt( nSel, true );
								if( pBar != NULL )
								{
									CWnd * pWndChild = pBar->GetWindow( GW_CHILD );
									if( pWndChild != NULL )
									{
										if( stat_QueryFocusChangingEnabled( this, pWndChild->m_hWnd ) )
											pWndChild->SetFocus();
									}
									else
									{
										if( stat_QueryFocusChangingEnabled( this, m_hWnd ) )
											pBar->SetFocus();
									}
									break;
								} // if( pBar != NULL )
							} // if( nSel >= 0 )
						} // if( pTabbedBar != NULL )
						else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
						{
							if( stat_QueryFocusChangingEnabled( this, pWndChild->m_hWnd ) )
								pWndChild->SetFocus();
							break;
						}		
					} // if( pWndChild != NULL )
					if( stat_QueryFocusChangingEnabled( this, m_hWnd ) )
						SetFocus();
					break;
				} // if( _msgOwnLBtnUp.message == WM_LBUTTONUP )
				if( ::IsWindow(hWndOwn) )
				{
					if( msg.hwnd == hWndOwn )
					{
						if( msg.message == WM_MOUSEMOVE )
						{
							::GetCursorPos( &ptMouseCurrent );
							CSize sizeMouseShift(
								abs( (ptMouseCurrent.x - ptMouseStart.x) ),
								abs( (ptMouseCurrent.y - ptMouseStart.y) )
								);
							if(		sizeMouseShift.cx >= sizeWaitMouseMove.cx
								||	sizeMouseShift.cy >= sizeWaitMouseMove.cy
								)
							{
								bMouseShiftDetected = true;
								break; // start dragging
							}
							PeekMessage( &msg, hWndOwn, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE );
							continue;
						} // if( msg.message == WM_MOUSEMOVE )
						switch( msg.message )
						{
						case WM_CAPTURECHANGED:
							if( ((HWND)msg.lParam) != hWndOwn )
							{
								bEnableStartDragging = false;
								break;
							} // if( ((HWND)msg.lParam) != hWndOwn )
							continue;
						case WM_CANCELMODE:
						case WM_ACTIVATEAPP:
						case WM_CONTEXTMENU:
							bEnableStartDragging = false;
							break;
						default:
							if(		(	WM_KEYFIRST <= msg.message
									&&	msg.message <= WM_KEYLAST
									)
								||	(	__EXT_MFC_WM_MOUSEFIRST <= msg.message
									&&	msg.message <= __EXT_MFC_WM_MOUSELAST
									)
								)
							{
								if( msg.message == WM_KEYDOWN && msg.wParam == VK_CONTROL )
								{
									PeekMessage( &msg, msg.hwnd, msg.message, msg.message, PM_REMOVE );
									continue;
								}
								bEnableStartDragging = false;
							}
							break;
						} // switch( msg.message )
					} // if( msg.hwnd == hWndOwn )
					else
					{
						switch( msg.message )
						{
						case WM_CANCELMODE:
						case WM_ACTIVATEAPP:
						case WM_CONTEXTMENU:
							bEnableStartDragging = false;
							break;
						default:
							if(		(	WM_KEYFIRST <= msg.message
									&&	msg.message <= WM_KEYLAST
									)
								||	(	__EXT_MFC_WM_MOUSEFIRST <= msg.message
									&&	msg.message <= __EXT_MFC_WM_MOUSELAST
									)
								)
							{
								if( msg.message == WM_KEYDOWN && msg.wParam == VK_CONTROL )
								{
									PeekMessage( &msg, msg.hwnd, msg.message, msg.message, PM_REMOVE );
									continue;
								}
								bEnableStartDragging = false;
							}
							break;
						} // switch( msg.message )
					} // else from if( msg.hwnd == hWndOwn )
				} // if( ::IsWindow(hWndOwn) )
				else
					bEnableStartDragging = false;
				if( ! bEnableStartDragging )
					break;
				if( !AfxGetThread()->PumpMessage() )
				{
					PostQuitMessage(0);
					break; // Signal WM_QUIT received
				} // if( !AfxGetThread()->PumpMessage() )
				if( ! ( ::IsWindow(hWndOwn) ) )
				{
					bEnableStartDragging = false;
					break;
				}
				if( CExtControlBar::g_bEnableOnIdleCalls )
				{
					for(	LONG nIdleCounter = 0L;
							::AfxGetThread()->OnIdle(nIdleCounter);
							nIdleCounter ++
							);
				} // if( CExtControlBar::g_bEnableOnIdleCalls )
				if( ! ( ::IsWindow(hWndOwn) ) )
				{
					bEnableStartDragging = false;
					break;
				}
				::WaitMessage();
			} // wait any mouse movement look
		} // for( ; bEnableStartDragging && (!bMouseShiftDetected); )
		if( ! bMouseShiftDetected )
			bEnableStartDragging = false;
		if( ! bEnableStartDragging )
		{
			if(		::IsWindow(hWndOwn)
				&&	CExtMouseCaptureSink::GetCapture() == hWndOwn
				)
				CExtMouseCaptureSink::ReleaseCapture();
			m_bDragDetecting = false;
			return;
		} // if( ! bEnableStartDragging )
	} // if( sizeWaitMouseMove.cx > 0 && sizeWaitMouseMove.cy > 0 )
	//ASSERT( CExtMouseCaptureSink::GetCapture() == hWndOwn );
	CExtMouseCaptureSink::SetCapture( hWndOwn );
	_SetCursor( point );
	m_bDragDetecting = false;

	ASSERT( g_DragStateOld.IsEmpty() );
	ASSERT( g_DragStateNew.IsEmpty() );
	g_DragStateOld.ExtBarSrcSet( this );
	g_DragStateNew.ExtBarSrcSet( this );
	g_DragStateNew.SetEmpty( false );

    ASSERT( m_pDockBar != NULL );
    ASSERT( m_pDockContext != NULL );

	_OnControlBarPositionChange_RedockingImpl(
		this,
		__ECBPC_REDOCKING,
		true,
		false
		);

	_Dragging_OnStart();
///	CExtMouseCaptureSink::SetCapture( GetSafeHwnd() ); // (- v.2.24)
	m_bDragging = true;

CRect rcGrip = _RectGripGet();
INT nCyGripper = min( rcGrip.Width(), rcGrip.Height() );
CPoint ptHelperDragOffsetDefault(
		( nCyGripper > 2 )
			? nCyGripper
			: ( ::GetSystemMetrics(SM_CYSMCAPTION) / 2 )
			,
		( nCyGripper > 2 )
			? ( nCyGripper / 2 )
			: ( ::GetSystemMetrics(SM_CYSMCAPTION) / 2 )
		);
	m_ptHelperDragOffsetFloated = ptHelperDragOffsetDefault;
	m_ptHelperDragOffsetDockedH = ptHelperDragOffsetDefault;
	m_ptHelperDragOffsetDockedV = ptHelperDragOffsetDefault;
	if( pointOffset.x != 0 || pointOffset.y != 0 )
	{
		if( IsFloating() )
			m_ptHelperDragOffsetFloated = pointOffset;
		else if( IsDockedHorizontally() )
			m_ptHelperDragOffsetDockedH = pointOffset;
		else
			m_ptHelperDragOffsetDockedV = pointOffset;
	} // if( pointOffset.x != 0 || pointOffset.y != 0 )

bool bInitialUpdateDraggingState = true;

#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
eResizablePanelDockingType_t eResizablePanelDockingType =
	OnQueryDockingType();

	if(		(! IsFixedMode() )
		&&	(! IsFixedDockStyle() )
		)
	{
		if( INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005) )
		{
			_DraggingUpdateState(
				point,
				CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) ? true : false
				);
			if( g_DragStateNew.GetCDT() == __ECDT_FLOATED )
				_DraggingApplyState( g_DragStateNew, false );
			bInitialUpdateDraggingState = false;
			ASSERT_VALID( m_pDockSite );
			CExtDynDockMarkerWnd::CreateOuterMarkers(
				m_pDockSite,
				false,
				PmBridge_GetPM()
				);
//			CExtDynDockMarkerWnd::CreateInnerMarkers(
//				m_pDockSite
//				);
			CExtDynDockMarkerWnd::CreateDynamicMarkers(
				m_pDockSite,
				false,
				PmBridge_GetPM()
				);
		} // if( INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005) )
	}
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)

	if( bInitialUpdateDraggingState )
	{
		_DraggingUpdateState(
			point,
			CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) ? true : false
			);
		_OnControlBarPositionChange_RedockingImpl(
			this,
			__ECBPC_REDOCKING,
			false,
			false
			);
	}
	ASSERT( hWndOwn != NULL && ::IsWindow(hWndOwn) );
	::SetTimer( hWndOwn, __TIMER_ID_DRAGGING_START, 50, NULL );
	while( m_bDragging && ::IsWindow(hWndOwn) )
	{
		::WaitMessage();
		// Process all the messages in the message queue
		while( ::IsWindow(hWndOwn) && PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			bool bStop = false;
			switch( msg.message )
			{
			case WM_TIMER:
				if( UINT(msg.wParam) == __TIMER_ID_DRAGGING_START )
				{
					PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
					CPoint ptCursor( 0, 0 );
					::GetCursorPos( &ptCursor );
					ScreenToClient( &ptCursor );
					PostMessage(
						WM_MOUSEMOVE,
						(msg.message == WM_KEYDOWN) ? (VK_CONTROL) : 0L,
						MAKELPARAM( ptCursor.x, ptCursor.y )
						);
					continue;
				} // if( UINT(msg.wParam) == __TIMER_ID_DRAGGING_START )
				break;
			case WM_ACTIVATEAPP:
			case WM_CANCELMODE:
//			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_NCLBUTTONUP:
//			case WM_NCLBUTTONDOWN:
			case WM_NCRBUTTONUP:
			case WM_NCRBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_NCMBUTTONDOWN:
			case WM_NCMBUTTONUP:
			case WM_CONTEXTMENU:
				bStop = true;
			break;
			case WM_MOUSEMOVE:
				{
					PeekMessage(&msg, NULL, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE);
					for( ; PeekMessage(&msg, NULL, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_NOREMOVE); )
						PeekMessage(&msg, NULL, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE);
					CPoint pt( (DWORD)msg.lParam );
					if( msg.hwnd != m_hWnd )
					{
						::ClientToScreen( msg.hwnd, &pt );
						::ScreenToClient( m_hWnd, &pt );
					}
					if( _OnMouseMoveMsg((UINT)msg.wParam,pt) )
					{
						_OnControlBarPositionChange_RedockingImpl(
							this,
							__ECBPC_REDOCKING,
							false,
							false
							);
						//PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
						continue;
					} // if( pTestBar->_OnMouseMoveMsg((UINT)msg.wParam,pt) )
				}
			default:
				if(	WM_KEYFIRST <= msg.message
					&& msg.message <= WM_KEYLAST
					)
				{
					if( g_bUpdatingDragState )
						break;
					if(		(	msg.message == WM_KEYDOWN
							||	msg.message == WM_KEYUP )
						&&	msg.wParam == VK_CONTROL
						)
					{
						CPoint ptCursor( 0, 0 );
						::GetCursorPos( &ptCursor );
						if( m_ptDraggingLast == ptCursor )
							m_ptDraggingLast.x --;
						ScreenToClient( &ptCursor );
						PostMessage(
							WM_MOUSEMOVE,
							(msg.message == WM_KEYDOWN) ? (VK_CONTROL) : 0L,
							MAKELPARAM( ptCursor.x, ptCursor.y )
							);
						break;
					}
					bStop = true;
				}
				else
				{
					if( CExtMouseCaptureSink::GetCapture() != GetSafeHwnd() )
						bStop = true;
					else
					{
						if(	(!CExtPopupMenuWnd::
								IsKeyPressed(VK_LBUTTON))
							|| CExtPopupMenuWnd::
								IsKeyPressed(VK_RBUTTON)
							|| CExtPopupMenuWnd::
								IsKeyPressed(VK_MBUTTON)
							)
							bStop = true;
					}
				}
			break;
			} // switch( msg.message )
			if( ! ( ::IsWindow(hWndOwn) ) )
				break;
			if( bStop )
			{
				_DraggingStop( true );
				break;
			}
			if( !AfxGetThread()->PumpMessage() )
			{
				PostQuitMessage(0);
				break; // Signal WM_QUIT received
			} // if( !AfxGetThread()->PumpMessage() )
		} // while( ::IsWindow(hWndOwn) && PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		if( ! ( ::IsWindow(hWndOwn) ) )
			break;
		if( m_bDragging && CExtControlBar::g_bEnableOnIdleCalls )
		{
			for(	LONG nIdleCounter = 0L;
					::AfxGetThread()->OnIdle(nIdleCounter);
					nIdleCounter ++
					);
		}
	} // while( m_bDragging && ::IsWindow(hWndOwn) )
	if(  ::IsWindow(hWndOwn) )
	{
		::KillTimer( hWndOwn, __TIMER_ID_DRAGGING_START );
		_OnControlBarPositionChange_RedockingImpl(
			this,
			__ECBPC_REDOCKING,
			false,
			true
			);
	}
}

void CExtControlBar::_DraggingStop( bool bCancel )
{
#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
	CExtDynDockMarkerWnd::DestroyAllMarkers();
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)

	ASSERT( !m_bRowRecalcing );
	ASSERT( !m_bRowResizing );
	//ASSERT( m_bDragging );
	if( !m_bDragging )
		return;

HWND hWndOwn = GetSafeHwnd();
	ASSERT( hWndOwn != NULL );
	ASSERT( ::IsWindow(hWndOwn) );

	m_bDragging = false;
	m_ptDraggingLast.x = m_ptDraggingLast.y = -1;

bool bEmptyOld = g_DragStateOld.IsEmpty();
bool bOldEqualNew =
		(g_DragStateOld == g_DragStateNew)
			? true : false;
	
//	g_DragStateOld.ExtBarDstSet( NULL );
//HWND hWndDstBar = g_DragStateNew.ExtBarDstGet()->GetSafeHwnd();

#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
eResizablePanelDockingType_t eResizablePanelDockingType =
		OnQueryDockingType();
	if(		bCancel
		&&	INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005)
		)
		bCancel = false;
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)

	if( !g_DragStateNew.IsEmpty() )
	{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		e_calc_dock_type_t eCDT = g_DragStateNew.GetCDT();
		if( 	eCDT != __ECDT_TABBED_UPDATE
			&&	eCDT != __ECDT_TABBED_REINDEX
			&&	eCDT != __ECDT_TABBED_REINDEX_NHM
			)
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			CExtPaintManager::stat_PassPaintMessages();
		g_DragStateNew.DrawState( true );

		_Dragging_OnStop(
			g_DragStateOld,
			g_DragStateNew,
			bCancel
			);
		
		if( !bCancel )
		{
			if( _IsShowContentWhenDragging() )
			{
				if( (!bEmptyOld) && (!bOldEqualNew) )
					_DraggingApplyState( g_DragStateNew, true );
				else
				{
#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
					if( INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005) )
						_DraggingApplyState( g_DragStateNew, true );
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)
				}
			} // if( _IsShowContentWhenDragging() )
			else
			{
///					g_DragStateOld.SetEmpty();
					_DraggingApplyState( g_DragStateNew, true );
			} // else from if( _IsShowContentWhenDragging() )
		} // if( !bCancel )
	}

//	g_DragStateNew.ExtBarDstSet(
//		NULL,
//		( ::IsWindow(hWndDstBar) ) ? false : true
//		);

	g_DragStateOld.ExtBarDstSet( NULL, true );
	g_DragStateNew.ExtBarDstSet( NULL, true );

	g_DragStateOld.SetEmpty();
	g_DragStateNew.SetEmpty();

	if( ::IsWindow(hWndOwn) )
	{
		ASSERT_VALID( this );

		if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
			CExtMouseCaptureSink::ReleaseCapture();

		if(		IsFloating()
			&&	( ! _IsShowContentWhenDragging() )
			)
			RedrawWindow(
				NULL, NULL,
				RDW_INVALIDATE //|RDW_UPDATENOW
					|RDW_ERASE //|RDW_ERASENOW
					|RDW_ALLCHILDREN
				);

		if( bCancel )
			m_pDockSite->DelayRecalcLayout();
	} // if( ::IsWindow(hWndOwn) )

}

CExtControlBar::InternalDraggingState_t
	CExtControlBar::g_DragStateOld;
CExtControlBar::InternalDraggingState_t
	CExtControlBar::g_DragStateNew;

bool CExtControlBar::g_bUpdatingDragState = false;

void CExtControlBar::_DraggingUpdateState(
	const CPoint & point,
	bool bForceFloatMode
	)
{
	if( CExtMouseCaptureSink::GetCapture() != GetSafeHwnd() )
		return;
	if( g_bUpdatingDragState || m_ptDraggingLast == point )
		return;
//	if( !IsFloating() )
//	{
//		bool bHorz = IsDockedHorizontally();
//		if( bHorz
//			&& abs(m_ptDraggingLast.y-point.y) < 3
//			&& m_ptDraggingLast.x == point.x
//			)
//			return;
//		if( abs(m_ptDraggingLast.x-point.x) < 3
//			&& m_ptDraggingLast.y == point.y
//			)
//			return;
//	}
	m_ptDraggingLast = point;
	g_bUpdatingDragState = true;

	ASSERT( !m_bRowRecalcing );
	ASSERT( !m_bRowResizing );
	ASSERT( m_bDragging );

eResizablePanelDockingType_t eResizablePanelDockingType =
		OnQueryDockingType();

	g_DragStateNew.CalcStateDynamic( point, bForceFloatMode );
	if( g_DragStateNew != g_DragStateOld )
	{
		bool bShowContent = _IsShowContentWhenDragging();
		bool bEmptyOld = g_DragStateOld.IsEmpty();
		bool bEmptyNew = g_DragStateNew.IsEmpty();

		if( ! bEmptyOld )
		{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			e_calc_dock_type_t eCDT = g_DragStateOld.GetCDT();
			if( ! (		(!bEmptyNew)
					&&	(	eCDT == __ECDT_TABBED_REINDEX
						||	eCDT == __ECDT_TABBED_REINDEX_NHM
						)
					&&	g_DragStateOld.ExtBarDstGet() == g_DragStateNew.ExtBarDstGet()
					)
				)
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
				if( 	eCDT != __ECDT_TABBED_UPDATE
					&&	eCDT != __ECDT_TABBED_REINDEX
					&&	eCDT != __ECDT_TABBED_REINDEX_NHM
					)
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
					CExtPaintManager::stat_PassPaintMessages();
				g_DragStateOld.DrawState( true );
				CExtPaintManager::stat_PassPaintMessages();
			}
		} // if( ! bEmptyOld )

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		CFrameWnd * pFrame = g_DragStateOld.ExtBarSrcGet()->m_pDockSite;
		ASSERT_VALID( pFrame );
		POSITION pos = pFrame->m_listControlBars.GetHeadPosition();
		for( ; pos != NULL; )
		{
			CControlBar * pBar = (CControlBar *)
				pFrame->m_listControlBars.GetNext( pos );
			ASSERT_VALID( pBar );
			if( pBar->GetSafeHwnd() == NULL
				|| ( ! pBar->IsVisible() )
				)
				continue;
			CExtDynTabControlBar * pTabbedBar =
				DYNAMIC_DOWNCAST(
					CExtDynTabControlBar,
					pBar
					);
			if( pTabbedBar == NULL )
				continue;
			pTabbedBar->InvalidateSwitcher();
		}

		if( (!bEmptyOld) && (!bEmptyNew) )
		{
			e_calc_dock_type_t eCDT = g_DragStateOld.GetCDT();
			if( (		eCDT == __ECDT_TABBED_UPDATE
					||	eCDT == __ECDT_TABBED_REINDEX
					||	eCDT == __ECDT_TABBED_REINDEX_NHM
				)
				//&& g_DragStateOld.ExtBarDstGet() != g_DragStateNew.ExtBarDstGet()
				)
			{
				g_DragStateOld.ExtBarDstSet( NULL );
			}
		} // if( (!bEmptyOld) && (!bEmptyNew) )

#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		
		if( ! bEmptyNew )
		{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			e_calc_dock_type_t eCDT = g_DragStateNew.GetCDT();
			if( 	eCDT != __ECDT_TABBED_UPDATE
				&&	eCDT != __ECDT_TABBED_REINDEX
				&&	eCDT != __ECDT_TABBED_REINDEX_NHM
				)
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
				CExtPaintManager::stat_PassPaintMessages();
			g_DragStateNew.DrawState( false );
		}

		if( bShowContent )
		{
			bool bDoApply = true;
#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
			e_calc_dock_type_t eCDT = g_DragStateNew.GetCDT();
			if( 	eCDT == __ECDT_TABBED_UPDATE
				||	eCDT == __ECDT_TABBED_REINDEX
				||	eCDT == __ECDT_TABBED_REINDEX_NHM
				)
			{
				if( INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005) )
				{
					//if( eCDT != __ECDT_TABBED_REINDEX_NHM )
						bDoApply = false;
					CExtDynDockMarkerWnd::UnHighlightAllMarkers();
				}
				if( eCDT != __ECDT_TABBED_REINDEX_NHM )
					CExtDynDockMarkerWnd::ShowAllMarkers( false );
			}
			else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
				if(	INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005)
					&&	( ! IsFixedDockStyle() )
					&&	( ! IsFixedMode() )
					)
			{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
				if( eCDT != __ECDT_2005_BAR_NEW_TAB )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
				{
					CExtDynDockMarkerWnd::CreateDynamicMarkers(
						m_pDockSite,
						false,
						PmBridge_GetPM()
						);
					if( ! bForceFloatMode )
						CExtDynDockMarkerWnd::ShowAllMarkers( true );
				} // if( eCDT != __ECDT_2005_BAR_NEW_TAB )
			}
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)
			if(		bDoApply
				&&	(! g_DragStateOld.IsEmpty() )
				&&	g_DragStateOld != g_DragStateNew
				)
				_DraggingApplyState( g_DragStateNew, false );
		} // if( bShowContent )

		g_DragStateOld = g_DragStateNew;
	
	} // if( g_DragStateNew != g_DragStateOld )
	else 
	{
#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
		if( bForceFloatMode )
			CExtDynDockMarkerWnd::ShowAllMarkers( false );
		else
		{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
			e_calc_dock_type_t eCDT = g_DragStateNew.GetCDT();
			if( 	eCDT == __ECDT_TABBED_UPDATE
				||	eCDT == __ECDT_TABBED_REINDEX
				||	eCDT == __ECDT_TABBED_REINDEX_NHM
				)
			{
				if( INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005) )
					CExtDynDockMarkerWnd::UnHighlightAllMarkers();
				if( eCDT != __ECDT_TABBED_REINDEX_NHM )
					CExtDynDockMarkerWnd::ShowAllMarkers( false );
			}
			else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
				if(		INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005)
					&&	( ! IsFixedDockStyle() )
					&&	( ! IsFixedMode() )
					)
			{
				CExtDynDockMarkerWnd::CreateDynamicMarkers(
					m_pDockSite,
					false,
					PmBridge_GetPM()
					);
				CExtDynDockMarkerWnd::ShowAllMarkers( true );
			}
		} // else from if( bForceFloatMode )
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)
		CExtPaintManager::stat_PassPaintMessages();
	} // else from if( g_DragStateNew != g_DragStateOld )

MSG msg;
	while( ::PeekMessage( &msg, NULL, WM_TIMER, WM_TIMER, PM_NOREMOVE ) )
	{
		if( ! ::GetMessage( &msg, NULL, WM_TIMER, WM_TIMER ) )
			break;
		::DispatchMessage( &msg );
	}

	eResizablePanelDockingType;
	if( CExtMouseCaptureSink::GetCapture() != GetSafeHwnd() )
		CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );
	g_bUpdatingDragState = false;
}

bool CExtControlBar::NcButtons_HandleClick(
	CExtBarNcAreaButton * pNcAreaButton,
	CPoint point,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
#ifdef _DEBUG
	if( pBarEventSource != NULL )
	{
		ASSERT_VALID( pNcAreaButton );
	}
	if( pBarEventSource != NULL )
	{
		ASSERT_VALID( pBarEventSource );
	}
#endif // _DEBUG
	pNcAreaButton;
	point;
	pBarEventSource;
	pBarActiveInContainer;
	return false;
}

void CExtControlBar::NcButtons_PostClick(
	CExtBarNcAreaButton * pNcAreaButton,
	CPoint point,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
#ifdef _DEBUG
	if( pBarEventSource != NULL )
	{
		ASSERT_VALID( pNcAreaButton );
	}
	if( pBarEventSource != NULL )
	{
		ASSERT_VALID( pBarEventSource );
	}
#endif // _DEBUG
	pNcAreaButton;
	point;
	pBarEventSource;
	pBarActiveInContainer;
}

bool CExtControlBar::NcButtons_HandleQueryVisibility(
	bool & bButtonIsVisible,
	CExtBarNcAreaButton * pNcAreaButton,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNcAreaButton );
	ASSERT_VALID( pBarEventSource );
#if (defined _DEBUG)
	if( pBarActiveInContainer != NULL )
	{
		ASSERT_VALID( pBarActiveInContainer );
	}
#endif // (defined _DEBUG)
	bButtonIsVisible;
	pNcAreaButton;
	pBarEventSource;
	pBarActiveInContainer;
	return false;
}

bool CExtControlBar::NcButtons_HandleDraw(
	CDC & dc,
	CExtBarNcAreaButton * pNcAreaButton,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNcAreaButton );
	ASSERT_VALID( pBarEventSource );
#if (defined _DEBUG)
	if( pBarActiveInContainer != NULL )
	{
		ASSERT_VALID( pBarActiveInContainer );
	}
#endif // (defined _DEBUG)
	dc;
	pNcAreaButton;
	pBarEventSource;
	pBarActiveInContainer;
	return false;
}

bool CExtControlBar::NcButtons_HandleHitTest(
	UINT & nExternalHT,
	CPoint point,
	CExtBarNcAreaButton * pNcAreaButton,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNcAreaButton );
	ASSERT_VALID( pBarEventSource );
#if (defined _DEBUG)
	if( pBarActiveInContainer != NULL )
	{
		ASSERT_VALID( pBarActiveInContainer );
	}
#endif // (defined _DEBUG)
	nExternalHT;
	point;
	pNcAreaButton;
	pBarEventSource;
	pBarActiveInContainer;
	return false;
}

bool CExtControlBar::NcButtons_HandleQueryCursor(
	HCURSOR & hExternalCursor,
	CPoint point,
	CExtBarNcAreaButton * pNcAreaButton,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNcAreaButton );
	ASSERT_VALID( pBarEventSource );
#if (defined _DEBUG)
	if( pBarActiveInContainer != NULL )
	{
		ASSERT_VALID( pBarActiveInContainer );
	}
#endif // (defined _DEBUG)
	hExternalCursor;
	point;
	pNcAreaButton;
	pBarEventSource;
	pBarActiveInContainer;
	return false;
}

bool CExtControlBar::NcButtons_HandleReposition(
	CExtBarNcAreaButton * pPrevBtn,
	CExtBarNcAreaButton * pNcAreaButton,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNcAreaButton );
	ASSERT_VALID( pBarEventSource );
#if (defined _DEBUG)
	if( pPrevBtn != NULL )
	{
		ASSERT_VALID( pPrevBtn );
	}
#endif // (defined _DEBUG)
#if (defined _DEBUG)
	if( pBarActiveInContainer != NULL )
	{
		ASSERT_VALID( pBarActiveInContainer );
	}
#endif // (defined _DEBUG)
	pPrevBtn;
	pNcAreaButton;
	pBarEventSource;
	pBarActiveInContainer;
	return false;
}

bool CExtControlBar::NcButtons_HandleToolHitTest(
	__EXT_MFC_INT_PTR & hExternalToolHT,
	CPoint point,
	TOOLINFO * pTI,
	CExtBarNcAreaButton * pNcAreaButton,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNcAreaButton );
	ASSERT_VALID( pBarEventSource );
#if (defined _DEBUG)
	if( pBarActiveInContainer != NULL )
	{
		ASSERT_VALID( pBarActiveInContainer );
	}
#endif // (defined _DEBUG)
	hExternalToolHT;
	point;
	pTI;
	pNcAreaButton;
	pBarEventSource;
	pBarActiveInContainer;
	return false;
}

void CExtControlBar::OnNcAreaButtonsReinitialize()
{
	ASSERT_VALID( this );
INT nCountOfNcButtons = NcButtons_GetCount();
	if( nCountOfNcButtons > 0 )
		return;
	NcButtons_Add( new CExtBarNcAreaButtonClose(this) );
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	NcButtons_Add( new CExtBarNcAreaButtonAutoHide(this) );
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	// NcButtons_Add( new CExtBarNcAreaButtonExpand(this) );
	NcButtons_Add( new CExtBarNcAreaButtonMenu(this) );
	// NcButtons_Add( new CExtBarNcAreaButtonFloatingMaximizeRestore(this) );
}

void CExtControlBar::OnNcAreaButtonsReposition()
{
	ASSERT_VALID( this );
INT nBtn, nCountOfNcButtons = NcButtons_GetCount();
CExtBarNcAreaButton * pPrevBtn = NULL;
	for( nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
	{
		CExtBarNcAreaButton * pBtn =
			NcButtons_GetAt( nBtn );
		ASSERT_VALID( pBtn );
		pBtn->OnNcAreaReposition( pPrevBtn );
		if( ! pBtn->OnQueryVisibility() )
			continue;
		pPrevBtn = pBtn;
	} // for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
	if( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 )
	{
		bool bHaveNcInfo = false;
		CRect rcGrip( 0, 0, 0, 0 );
		for( nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
		{
			CExtBarNcAreaButton * pBtn =
				NcButtons_GetAt( nBtn );
			ASSERT_VALID( pBtn );
			if( ! pBtn->OnQueryVisibility() )
				continue;
			if( ! bHaveNcInfo )
			{
				bool bGripperAtTop = false;
				if( ! pBtn->GetBarNcAreaInfo( &rcGrip, NULL, &bGripperAtTop ) )
					break;
				if( ! bGripperAtTop )
					break;
				bHaveNcInfo = true;
			} // if( ! bHaveNcInfo )
			pBtn->m_rc.OffsetRect(
				rcGrip.left - pBtn->m_rc.left
					+ rcGrip.right - pBtn->m_rc.right
					,
				0
				);
		} // for( INT nBtn = 0; nBtn < nCountOfNcButtons; nBtn++ )
	} // if( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 )
}

/////////////////////////////////////////////////////////////////////////////
// CExtBarNcAreaButton

IMPLEMENT_DYNCREATE( CExtBarNcAreaButton, CObject )

CExtBarNcAreaButton::CExtBarNcAreaButton(
	CExtControlBar * pBar
	)
	: m_pBar( pBar )
	, m_rc( 0, 0, 0, 0 )
	, m_bHover( false )
	, m_bPushed( false )
	, m_bDisabled( false )
	, m_bPaintingCombinedArea( false )
{
	ASSERT_VALID( this );
}

CExtBarNcAreaButton::~CExtBarNcAreaButton()
{
	ASSERT_VALID( this );
}

void CExtBarNcAreaButton::OnNcDestroy()
{
	ASSERT_VALID( this );
	delete this;
}

bool CExtBarNcAreaButton::IsBarFixedMode() const
{
	ASSERT_VALID( this );
	return m_pBar->IsFixedMode();
}

bool CExtBarNcAreaButton::IsBarVisible() const
{
	ASSERT_VALID( this );
	return m_pBar->IsVisible() ? true : false;
}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
bool CExtBarNcAreaButton::OnQueryDeactivateAutohideSlider() const
{
	ASSERT_VALID( this );
	return true;
}

bool CExtBarNcAreaButton::IsBarAutoHideMode(
	bool * p_bDisplayed, // = NULL
	bool * p_bActive // = NULL
	) const
{
	ASSERT_VALID( this );
	if( p_bDisplayed != NULL )
		*p_bDisplayed = false;
	if( p_bActive != NULL )
		*p_bActive = false;
bool bAutohide = m_pBar->AutoHideModeGet();
	if( ! bAutohide )
		return false;
	if( p_bDisplayed != NULL || p_bActive != NULL )
	{
		if( m_pBar->m_pDockBar == NULL )
			return false;
		ASSERT_VALID( m_pBar->m_pDockBar );
		if( ! m_pBar->m_pDockBar->IsKindOf( RUNTIME_CLASS(CExtDockBar) ) )
			return false;
		CExtDynAutoHideArea * pWndAutoHideArea = ((CExtDockBar*)m_pBar->m_pDockBar)->_GetAutoHideArea();
		ASSERT_VALID( pWndAutoHideArea );
		if( pWndAutoHideArea->m_pWndSlider != NULL )
		{
			ASSERT_VALID( pWndAutoHideArea->m_pWndSlider );
			if( pWndAutoHideArea->m_pWndSlider->_GetBar() == m_pBar )
			{
				if( p_bDisplayed != NULL )
					*p_bDisplayed = true;
				if( p_bActive != NULL )
					*p_bActive = pWndAutoHideArea->m_pWndSlider->m_bWndActive;
			}
		}
	}
	return true;
}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

bool CExtBarNcAreaButton::IsBarFloated() const
{
	ASSERT_VALID( this );
	return m_pBar->IsFloating();
}

bool CExtBarNcAreaButton::IsBarDocked() const
{
	ASSERT_VALID( this );
	return m_pBar->IsDocked();
}

bool CExtBarNcAreaButton::IsBarDockedHorizontally() const
{
	ASSERT_VALID( this );
	return m_pBar->IsDockedHorizontally();
}

bool CExtBarNcAreaButton::IsBarDockedVertically() const
{
	ASSERT_VALID( this );
	return m_pBar->IsDockedVertically();
}

bool CExtBarNcAreaButton::IsBarDockedAtLeft() const
{
	ASSERT_VALID( this );
	return m_pBar->IsDockedAtLeft();
}

bool CExtBarNcAreaButton::IsBarDockedAtRight() const
{
	ASSERT_VALID( this );
	return m_pBar->IsDockedAtRight();
}

bool CExtBarNcAreaButton::IsBarDockedAtTop() const
{
	ASSERT_VALID( this );
	return m_pBar->IsDockedAtTop();
}

bool CExtBarNcAreaButton::IsBarDockedAtBottom() const
{
	ASSERT_VALID( this );
	return m_pBar->IsDockedAtBottom();
}

bool CExtBarNcAreaButton::GetBarNcAreaInfo( // returns false if no nc area on bar
	LPRECT pRectGripper, //  = NULL // in window coords
	LPRECT pRectWnd, // = NULL // in screen coords
	bool * pbGripperAtTop, //  = NULL
	bool * pbTextOnGripper //  = NULL
	) const
{
	ASSERT_VALID( this );
	if( pRectGripper != NULL )
		::SetRectEmpty( pRectGripper );
	if( pRectWnd != NULL )
		::SetRectEmpty( pRectWnd );
	
	if( IsBarFloated() )
	{
		if( pbGripperAtTop != NULL )
			*pbGripperAtTop = false;
		if( pbTextOnGripper != NULL )
			*pbTextOnGripper = false;
		
		CFrameWnd * pMiniFrame =
			m_pBar->GetDockingFrame();
		if( pRectWnd != NULL )
			pMiniFrame->GetWindowRect( pRectWnd );
		CExtMiniDockFrameWnd * pExtMiniFrame =
			DYNAMIC_DOWNCAST( CExtMiniDockFrameWnd, pMiniFrame );
		if( pExtMiniFrame == NULL )
			return false;
		
		if( pRectGripper != NULL )
		{
			CRect rcGrip( pExtMiniFrame->m_rcFrameCaption );
			rcGrip.InflateRect( 2, 2, 1, 2 );
			*pRectGripper = rcGrip;
		}
		if( pbGripperAtTop != NULL )
			*pbGripperAtTop = true;
		if( pbTextOnGripper != NULL )
			*pbTextOnGripper = true;
		return true;
	} // if( IsBarFloated() )

	if( pRectWnd != NULL )
		m_pBar->GetWindowRect( pRectWnd );
	if( ! m_pBar->IsFixedMode() )
	{
		CFrameWnd * pParentFrame = m_pBar->GetParentFrame();
		if(		pParentFrame != NULL
			&&	pParentFrame->IsKindOf(RUNTIME_CLASS(CExtMiniDockFrameWnd))
			)
		{
			if( m_pBar->_IsSingleVisibleInFloatingPalette() )
			{
				if( pbGripperAtTop != NULL )
					*pbGripperAtTop = false;
				if( pbTextOnGripper != NULL )
					*pbTextOnGripper = false;
				
				if( pRectWnd != NULL )
					pParentFrame->GetWindowRect( pRectWnd );
				CExtMiniDockFrameWnd * pExtMiniFrame =
					STATIC_DOWNCAST( CExtMiniDockFrameWnd, pParentFrame );
				
				if( pRectGripper != NULL )
				{
					CRect rcGrip( pExtMiniFrame->m_rcFrameCaption );
					rcGrip.InflateRect( 2, 2, 1, 2 );
					*pRectGripper = rcGrip;
				}
				if( pbGripperAtTop != NULL )
					*pbGripperAtTop = true;
				if( pbTextOnGripper != NULL )
					*pbTextOnGripper = true;
				return true;
			}
		}
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		else if( pRectWnd != NULL && IsBarAutoHideMode() )
		{
			CExtDynAutoHideArea * pTabs =
				CExtDynAutoHideArea::stat_FindAreaOfBar( m_pBar );
			if( pTabs != NULL )
			{
				CExtDynAutoHideSlider * pSlider =
					pTabs->GetAutoHideSlider();
				ASSERT_VALID( pSlider );
				pSlider->GetWindowRect( pRectWnd );
			} // if( pTabs != NULL )
		}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	}

bool bGrip =
		m_pBar->IsBarWithGripper(
			pbGripperAtTop,
			pbTextOnGripper
			);
	if( !bGrip )
		return false;
	if( pRectGripper != NULL )
	{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( m_pBar->AutoHideModeGet() )
		{
			ASSERT_VALID( m_pBar->m_pDockBar );
			ASSERT_KINDOF( CExtDockBar, m_pBar->m_pDockBar );
			CExtDynAutoHideArea * pAutoHideArea =
				((CExtDockBar*)m_pBar->m_pDockBar)->_GetAutoHideArea();
			ASSERT_VALID( pAutoHideArea );
			ASSERT_VALID( pAutoHideArea->m_pWndSlider );
			pAutoHideArea->m_pWndSlider->
				CalcInnerNcRects(
					NULL,
					NULL,
					pRectGripper
					);
		}
		else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			*pRectGripper = m_pBar->_RectGripGet();
	}
	return true;
}

bool CExtBarNcAreaButton::IsBarWndActive() const
{
	ASSERT_VALID( this );
	
	if( IsBarFloated() )
	{
		CFrameWnd * pMiniFrame =
			m_pBar->GetDockingFrame();
		CExtMiniDockFrameWnd * pExtMiniFrame =
			DYNAMIC_DOWNCAST( CExtMiniDockFrameWnd, pMiniFrame );
		if( pExtMiniFrame == NULL )
			return false;

		HWND hTmpWndFocus = ::GetFocus();
		if( hTmpWndFocus != NULL
			&& (  pExtMiniFrame->GetSafeHwnd() == hTmpWndFocus
				  || ::IsChild( pExtMiniFrame->GetSafeHwnd(), hTmpWndFocus )
				)
			)
			return true;
		return  false;
	} // if( IsBarFloated() )

	return m_pBar->IsBarWindowActive();
}

bool CExtBarNcAreaButton::IsBarSingleOnRow() const
{
	ASSERT_VALID( this );
	return m_pBar->IsSingleOnRow();
}

bool CExtBarNcAreaButton::IsBarMinimizedOnRow() const
{
	ASSERT_VALID( this );
	return m_pBar->IsMinimizedOnRow();
}

bool CExtBarNcAreaButton::IsBarMaximizedOnRow() const
{
	ASSERT_VALID( this );
	return m_pBar->IsMaximizedOnRow();
}

bool CExtBarNcAreaButton::IsBarFirstOnRow( bool bExtBar ) const
{
	ASSERT_VALID( this );
	return m_pBar->_IsFirstBarInRow( bExtBar );
}

bool CExtBarNcAreaButton::IsBarLastOnRow( bool bExtBar ) const
{
	ASSERT_VALID( this );
	return m_pBar->_IsLastBarInRow( bExtBar );
}

CFrameWnd * CExtBarNcAreaButton::GetDockingFrame()
{
	ASSERT_VALID( this );
	return m_pBar->_GetDockingFrameImpl();
}

void CExtBarNcAreaButton::OnNcAreaReposition(
	CExtBarNcAreaButton * pPrevBtn
	)
{
	ASSERT_VALID( this );

CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleReposition(
			pPrevBtn,
			this,
			pBar,
			NULL
			)
		)
		return;

	//m_bHover = m_bPushed = false;

bool bGripperAtTop = false;
CRect rcCaption;
	if(	! GetBarNcAreaInfo( &rcCaption, NULL, &bGripperAtTop ) )
		return;
	m_rc = rcCaption;
const int _nGapToBordersH = 2;
const int _nGapToBordersV = 3;
const int _nGapBetweenButtons = 3;
	if( bGripperAtTop )
	{
		if( ! GetBar()->PmBridge_GetPM()->Bar_LayoutNcAreaButton(
					m_rc,
					rcCaption,
					this,
					pPrevBtn,
					this
					)
				)
		{
			m_rc.DeflateRect(
				_nGapToBordersH,
				_nGapToBordersV + 1,
				_nGapToBordersH,
				_nGapToBordersV
				);
			m_rc.left = m_rc.right - m_rc.Height();
			m_rc.OffsetRect( -1, 0 );
			if( pPrevBtn != NULL )
			{
				const CRect & rcPrev = *pPrevBtn;
				m_rc.OffsetRect(
					rcPrev.left - m_rc.right -
						_nGapBetweenButtons,
					0
					);
			} // if( pPrevBtn != NULL )
		}
	} // if( bGripperAtTop )
	else
	{
		m_rc.DeflateRect( _nGapToBordersV, _nGapToBordersH );
		m_rc.bottom = m_rc.top + m_rc.Width();
		m_rc.OffsetRect( 0, 1 );
		if( pPrevBtn != NULL )
		{
			const CRect & rcPrev = *pPrevBtn;
			m_rc.OffsetRect( 0, - m_rc.top + rcPrev.bottom + _nGapBetweenButtons );
		} // if( pPrevBtn != NULL )
	} // else from if( bGripperAtTop )
}

bool CExtBarNcAreaButton::OnQueryVisibility() const
{
	ASSERT_VALID( this );
bool bButtonIsVisible = true;
CExtControlBar * pBar = const_cast < CExtControlBar * > ( GetBar() );
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleQueryVisibility(
			bButtonIsVisible,
			const_cast < CExtBarNcAreaButton * > ( this ),
			pBar,
			NULL
			)
		)
		return bButtonIsVisible;

	if( ! IsBarVisible() )
	{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		bool bDisplayed = false;
		if( IsBarAutoHideMode( &bDisplayed ) )
		{
			if( bDisplayed )
				return true;
		}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		return false;
	}
CRect rcGrip;
	if(	! GetBarNcAreaInfo( &rcGrip ) )
		return false;
	if( rcGrip.IsRectEmpty() )
		return false;
	return true;
}

void CExtBarNcAreaButton::NcDrawDefault(
	CDC & dc,
	int nDockBtnGlyptT
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( OnQueryVisibility() );
	if( m_rc.IsRectEmpty() )
		return;
bool bMenuTracking =
		CExtPopupMenuWnd::IsMenuTracking() ? true : false;

//bool bDrawActive = IsBarWndActive();
//	if( !bDrawActive )
//		IsBarAutoHideMode( NULL, &bDrawActive );
bool bDrawActive = false;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( !IsBarAutoHideMode( NULL, &bDrawActive ) )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		bDrawActive = IsBarWndActive();
	if( GetBar()->IsKindOf(RUNTIME_CLASS(CExtToolControlBar)) )
		bDrawActive = false;

CExtPaintManager::PAINTDOCKINGCAPTIONBUTTONDATA _pdcbd(
		this,
		m_rc,
		(CExtPaintManager::e_docking_caption_button_t)nDockBtnGlyptT,
		(!m_bDisabled) && m_bHover && (!bMenuTracking) && (!m_bPaintingCombinedArea),
		(!m_bDisabled) && m_bPushed && (!bMenuTracking) && (!m_bPaintingCombinedArea),
		(!m_bDisabled),
		(!IsBarFixedMode()) || m_bPaintingCombinedArea,
		bDrawActive,
		IsBarFloated()
		);
	GetBar()->PmBridge_GetPM()->PaintDockingCaptionButton( dc, _pdcbd );
}

void CExtBarNcAreaButton::OnNcAreaDraw( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( OnQueryVisibility() );
	if( m_rc.IsRectEmpty() )
		return;

CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleDraw(
			dc,
			this,
			pBar,
			NULL
			)
		)
		return;

#if (defined __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__)
	dc.FillSolidRect( &m_rc, RGB(255,128,128) );
#else // __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__
	dc;
#endif // !__DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__
}

UINT CExtBarNcAreaButton::OnNcAreaHitTest( CPoint point )
{
	ASSERT_VALID( this );

UINT nExternalHT = HTNOWHERE;
CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleHitTest(
			nExternalHT,
			point,
			this,
			pBar,
			NULL
			)
		)
		return nExternalHT;

	if( m_rc.PtInRect( point ) )
	{
		m_bPushed =
			(
				CExtPopupMenuWnd::IsKeyPressed( VK_LBUTTON )
				&& !CExtPopupMenuWnd::IsKeyPressed( VK_MBUTTON )
				&& !CExtPopupMenuWnd::IsKeyPressed( VK_RBUTTON )
			)
			? true : false;
		m_bHover = true;
		return HTNOWHERE;
	}
	m_bHover = m_bPushed = false;

	return HTCLIENT; // not inside nc-button
}

HCURSOR CExtBarNcAreaButton::OnNcAreaQueryCursor( CPoint point )
{
	ASSERT_VALID( this );

HCURSOR hExternalCursor = NULL;
CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleQueryCursor(
			hExternalCursor,
			point,
			this,
			pBar,
			NULL
			)
		)
		return hExternalCursor;

	if( m_rc.PtInRect( point ) )
		return g_hCursorArrow;
	return NULL; // not inside nc-button
}

bool CExtBarNcAreaButton::OnNcAreaClicked( CPoint point )
{
	ASSERT_VALID( this );
CExtControlBar * pExtBar = GetBar();
	ASSERT_VALID( pExtBar );
HWND hWndBar = pExtBar->GetSafeHwnd();
	if( pExtBar->NcButtons_HandleClick( this, point, pExtBar, NULL ) )
		return true;
	ASSERT_VALID( pExtBar );

	if( m_rc.PtInRect( point ) )
		return true;

	if(		hWndBar != NULL
		&&	::IsWindow( hWndBar )
		)
		pExtBar->NcButtons_PostClick( this, point, pExtBar, NULL );
	return false; // continue asking nc-buttons
}

__EXT_MFC_INT_PTR CExtBarNcAreaButton::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	)
{
	ASSERT_VALID( this );

__EXT_MFC_INT_PTR hExternalToolHT = -1;
CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleToolHitTest(
			hExternalToolHT,
			point,
			pTI,
			this,
			pBar,
			NULL
			)
		)
		return hExternalToolHT;

	point; // should be in button area
	pTI;

	return -1;
}

__EXT_MFC_INT_PTR CExtBarNcAreaButton::DoDefaultReflectForToolHitTest(
	TOOLINFO * pTI,
	UINT nIdResourceTooltipText,
	__EXT_MFC_SAFE_LPCTSTR sTooltipTextDefault
	)
{
	ASSERT_VALID( this );
	ASSERT( sTooltipTextDefault != NULL );
CExtLocalResourceHelper _LRH;
CExtSafeString sTooltipText;
	if( ! g_ResourceManager->LoadString( sTooltipText, nIdResourceTooltipText ) )
		sTooltipText = sTooltipTextDefault;
	
	return
		DoDefaultReflectForToolHitTest(
			pTI,
			sTooltipText
			);
}

__EXT_MFC_INT_PTR CExtBarNcAreaButton::DoDefaultReflectForToolHitTest(
	TOOLINFO * pTI,
	__EXT_MFC_SAFE_LPCTSTR sTooltipText
	)
{
	ASSERT_VALID( this );
	ASSERT( sTooltipText != NULL );

CPoint point;
	if( ::GetCursorPos( &point ) )
	{
		__EXT_MFC_INT_PTR hExternalToolHT = -1;
		CExtControlBar * pBar = GetBar();
		ASSERT_VALID( pBar );
		if(	pBar->NcButtons_HandleToolHitTest(
				hExternalToolHT,
				point,
				pTI,
				this,
				pBar,
				NULL
				)
			)
			return hExternalToolHT;
	}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if(		IsBarAutoHideMode()
		&&	GetBar()->OnAdvancedPopupMenuTipWndGet() == NULL
		)
		return 0;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	if( pTI != NULL )
	{
		CExtControlBar * pBar = GetBar();
		ASSERT_VALID( pBar );

		CPoint ptDevOffset( 0, 0 );
		if( pBar->IsFloating() )
		{
			CRect rcBarWnd, rcBarClient;
			CFrameWnd * pFrame = pBar->GetParentFrame();
			ASSERT_VALID( pFrame );
			pFrame->GetWindowRect( &rcBarWnd );
			pFrame->GetClientRect( &rcBarClient );
			pFrame->ClientToScreen( &rcBarClient );
			ptDevOffset =
				rcBarClient.TopLeft()
				- rcBarWnd.TopLeft();
		} // if( pBar->IsFloating() )
		else
		{
			CRect rcBarWnd, rcBarClient;
			pBar->GetWindowRect( &rcBarWnd );
			pBar->GetClientRect( &rcBarClient );
			pBar->ClientToScreen( &rcBarClient );
			ptDevOffset =
				rcBarClient.TopLeft()
				- rcBarWnd.TopLeft();
		} // else from if( pBar->IsFloating() )

		CRect rcArea = *this;
		rcArea.OffsetRect( -ptDevOffset );
		
		::CopyRect(
			&(pTI->rect),
			&rcArea
			);

		__EXT_MFC_INT_PTR nCmdID = (__EXT_MFC_INT_PTR)pBar->GetDlgCtrlID();
		
		pTI->uId = (UINT)nCmdID;
		pTI->hwnd = pBar->GetSafeHwnd();

		pTI->lpszText = (LPTSTR)
			::calloc(
				_tcslen(sTooltipText) + 1,
				sizeof(TCHAR)
				);
		if( pTI->lpszText != NULL )
			__EXT_MFC_STRCPY(
				pTI->lpszText,
				_tcslen(sTooltipText) + 1,
				sTooltipText
				);
		else
			pTI->lpszText = LPSTR_TEXTCALLBACK;

		return nCmdID;
	} // if( pTI != NULL )
	
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// CExtBarNcAreaButtonFloatingMaximizeRestore
//
//IMPLEMENT_DYNCREATE( CExtBarNcAreaButtonFloatingMaximizeRestore, CExtBarNcAreaButton )
//
//CExtBarNcAreaButtonFloatingMaximizeRestore::CExtBarNcAreaButtonFloatingMaximizeRestore(
//	CExtControlBar * pBar
//	)
//	: CExtBarNcAreaButton( pBar )
//{
//}
//
//bool CExtBarNcAreaButtonFloatingMaximizeRestore::OnQueryVisibility() const
//{
//	ASSERT_VALID( this );
//bool bButtonIsVisible = true;
//CExtControlBar * pBar = const_cast < CExtControlBar * > ( GetBar() );
//	ASSERT_VALID( pBar );
//	if(	pBar->NcButtons_HandleQueryVisibility(
//			bButtonIsVisible,
//			const_cast < CExtBarNcAreaButton * > ( this ),
//			pBar,
//			NULL
//			)
//		)
//		return bButtonIsVisible;
//
//	if( ! CExtBarNcAreaButton::OnQueryVisibility() )
//		return false;
//	if( IsBarFixedMode() || (! IsBarFloated() ) )
//		return false;
//#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
//	if( IsBarAutoHideMode() )
//		return false;
//#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
//	return true;
//}
//
//void CExtBarNcAreaButtonFloatingMaximizeRestore::OnNcAreaDraw( CDC & dc )
//{
//	ASSERT_VALID( this );
//	ASSERT( dc.GetSafeHdc() != NULL );
//	ASSERT( OnQueryVisibility() );
//	if( m_rc.IsRectEmpty() )
//		return;
//
//CExtControlBar * pBar = GetBar();
//	ASSERT_VALID( pBar );
//	if(	pBar->NcButtons_HandleDraw(
//			dc,
//			this,
//			pBar,
//			NULL
//			)
//		)
//		return;
//
//#if (defined __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__)
//
//	dc.FillSolidRect( &m_rc, RGB(128,0,128) );
//
//#else // __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__
//
//	m_bDisabled = false;
//	if( IsBarFixedMode() || (! IsBarFloated() ) )
//		m_bDisabled = true;
//#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
//	else if( IsBarAutoHideMode() )
//		m_bDisabled = true;
//#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
//
//bool bBarIsInFloatingMaximizedState = IsBarInFloatingMaximizedState();
//	NcDrawDefault(
//		dc,
//		bBarIsInFloatingMaximizedState
//			? CExtPaintManager::__DCBT_TREE_MINUS
//			: CExtPaintManager::__DCBT_TREE_PLUS
//		);
//
//#endif // !__DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__
//}
//
//UINT CExtBarNcAreaButtonFloatingMaximizeRestore::OnNcAreaHitTest( CPoint point )
//{
//	ASSERT_VALID( this );
//
//UINT nExternalHT = HTNOWHERE;
//CExtControlBar * pBar = GetBar();
//	ASSERT_VALID( pBar );
//	if(	pBar->NcButtons_HandleHitTest(
//			nExternalHT,
//			point,
//			this,
//			pBar,
//			NULL
//			)
//		)
//		return nExternalHT;
//
//UINT nHT = CExtBarNcAreaButton::OnNcAreaHitTest( point );
//	if( nHT == HTCLIENT )
//		return HTCLIENT; // not inside nc-button
//
//	if( IsBarFixedMode() || (! IsBarFloated() ) )
//		return HTCLIENT;
//#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
//	if( IsBarAutoHideMode() )
//		return HTCLIENT;
//#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
//
//	return HTMAXBUTTON;
//}
//
//bool CExtBarNcAreaButtonFloatingMaximizeRestore::OnNcAreaClicked( CPoint point )
//{
//	ASSERT_VALID( this );
//	if( ! m_rc.PtInRect( point ) )
//		return false; // continue asking nc-buttons
//
//	m_bHover = m_bPushed = false;
//CExtControlBar * pBar = GetBar();
//	ASSERT_VALID( pBar );
//HWND hWndBar = pBar->GetSafeHwnd();
//	if( pBar->NcButtons_HandleClick( this, point, pBar, NULL ) )
//		return true;
//	ASSERT_VALID( pBar );
//
//	if( IsBarFixedMode() || (! IsBarFloated() ) )
//		return false;
//#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
//	if( IsBarAutoHideMode() )
//	{
//		if(		hWndBar != NULL
//			&&	::IsWindow( hWndBar )
//			)
//			pBar->NcButtons_PostClick( this, point, pBar, NULL );
//		return false;
//	}
//#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
//
//CFrameWnd * pFrameWnd = pBar->GetParentFrame();
//	ASSERT_VALID( pFrameWnd );
//	ASSERT_KINDOF( CMiniFrameWnd, pFrameWnd );
//bool bBarIsInFloatingMaximizedState = IsBarInFloatingMaximizedState();
//	pFrameWnd->ShowWindow(
//		bBarIsInFloatingMaximizedState ? SW_RESTORE : SW_MAXIMIZE
//		);
//	pFrameWnd->SetWindowPos(
//		NULL, 0, 0, 0, 0,
//		SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE
//			|SWP_NOZORDER|SWP_NOOWNERZORDER
//			|SWP_FRAMECHANGED
//		);
//CRect rcFrameClient, rcFrameWnd;
//	pFrameWnd->GetClientRect( &rcFrameClient );
//	pFrameWnd->GetWindowRect( &rcFrameWnd );
//CSize sizeFrameClient = rcFrameClient.Size();
//	pBar->SetInitDesiredPosFloating( rcFrameWnd.TopLeft() );
//	pBar->SetInitDesiredSizeFloating( sizeFrameClient );
//	pBar->m_pDockContext->m_sizeLast = sizeFrameClient;
//	pBar->m_pDockContext->m_ptMRUFloatPos = rcFrameWnd.TopLeft();
//	pFrameWnd->RecalcLayout();
//	pBar->OnRepositionSingleChild( sizeFrameClient.cx, sizeFrameClient.cy, true );
//
//	if(		hWndBar != NULL
//		&&	::IsWindow( hWndBar )
//		)
//		pBar->NcButtons_PostClick( this, point, pBar, NULL );
//	return true;
//}
//
//__EXT_MFC_INT_PTR CExtBarNcAreaButtonFloatingMaximizeRestore::OnToolHitTest(
//	CPoint point,
//	TOOLINFO * pTI
//	)
//{
//	ASSERT_VALID( this );
//	
//	point; // should be in button area
//
//bool bBarIsInFloatingMaximizedState = IsBarInFloatingMaximizedState();
//	return
//		DoDefaultReflectForToolHitTest(
//			pTI,
//			bBarIsInFloatingMaximizedState ? _T("Maximize") : _T("Restore")
//			);
//}
//
//bool CExtBarNcAreaButtonFloatingMaximizeRestore::stat_IsBarInFloatingMaximizedState(
//	const CExtControlBar * pBar
//	)
//{
//	if( pBar->GetSafeHwnd() == NULL )
//		return false;
//	if( ! pBar->IsFloating() )
//		return false;
//CFrameWnd * pFrameWnd = pBar->GetParentFrame();
//	ASSERT_VALID( pFrameWnd );
//	ASSERT_KINDOF( CMiniFrameWnd, pFrameWnd );
//WINDOWPLACEMENT _wp;
//	::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
//	_wp.length = sizeof(WINDOWPLACEMENT);
//	pFrameWnd->GetWindowPlacement( &_wp );
//bool bBarIsInFloatingMaximizedState = ( _wp.showCmd == SW_SHOWMAXIMIZED ) ? true : false;
//	return bBarIsInFloatingMaximizedState;
//}
//
//bool CExtBarNcAreaButtonFloatingMaximizeRestore::IsBarInFloatingMaximizedState() const
//{
//	ASSERT_VALID( this );
//bool bBarIsInFloatingMaximizedState = stat_IsBarInFloatingMaximizedState( GetBar() );
//	return bBarIsInFloatingMaximizedState;
//}

/////////////////////////////////////////////////////////////////////////////
// CExtBarNcAreaButtonExpand

IMPLEMENT_DYNCREATE( CExtBarNcAreaButtonExpand, CExtBarNcAreaButton )

CExtBarNcAreaButtonExpand::CExtBarNcAreaButtonExpand(
	CExtControlBar * pBar
	)
	: CExtBarNcAreaButton( pBar )
{
}

bool CExtBarNcAreaButtonExpand::OnQueryVisibility() const
{
	ASSERT_VALID( this );
bool bButtonIsVisible = true;
CExtControlBar * pBar = const_cast < CExtControlBar * > ( GetBar() );
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleQueryVisibility(
			bButtonIsVisible,
			const_cast < CExtBarNcAreaButtonExpand * > ( this ),
			pBar,
			NULL
			)
		)
		return bButtonIsVisible;

	if( ! CExtBarNcAreaButton::OnQueryVisibility() )
		return false;
	if( ! pBar->_RowRecalcingIsEnabled( GetBar() ) )
		return false;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( IsBarAutoHideMode() )
		return false;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( IsBarFixedMode() || IsBarFloated() )
		return false;
	return true;
}

void CExtBarNcAreaButtonExpand::OnNcAreaDraw( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( OnQueryVisibility() );
	if( m_rc.IsRectEmpty() )
		return;

CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleDraw(
			dc,
			this,
			pBar,
			NULL
			)
		)
		return;

#if (defined __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__)

	dc.FillSolidRect( &m_rc, RGB(128,255,128) );

#else // __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	m_bDisabled = IsBarAutoHideMode();
	if( !m_bDisabled )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		m_bDisabled =
			IsBarSingleOnRow()
			;
bool bMax = IsBarMaximizedOnRow();
bool bHorz = IsBarDockedHorizontally();
	NcDrawDefault(
		dc,
		bHorz
			? ( bMax
					? CExtPaintManager::__DCBT_ARROW_RIGHT
					: CExtPaintManager::__DCBT_ARROW_LEFT
				)
			: ( bMax
					? CExtPaintManager::__DCBT_ARROW_DOWN
					: CExtPaintManager::__DCBT_ARROW_UP
					)
		);

#endif // !__DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__
}

UINT CExtBarNcAreaButtonExpand::OnNcAreaHitTest( CPoint point )
{
	ASSERT_VALID( this );

UINT nExternalHT = HTNOWHERE;
CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleHitTest(
			nExternalHT,
			point,
			this,
			pBar,
			NULL
			)
		)
		return nExternalHT;

UINT nHT = CExtBarNcAreaButton::OnNcAreaHitTest( point );
	if( nHT == HTCLIENT )
		return HTCLIENT; // not inside nc-button

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( IsBarAutoHideMode() )
		return HTCLIENT;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( IsBarSingleOnRow() )
		return HTCLIENT;
//	if( IsBarMaximizedOnRow() )
		return HTMINBUTTON;
//	return HTMAXBUTTON;
}

bool CExtBarNcAreaButtonExpand::OnNcAreaClicked( CPoint point )
{
	ASSERT_VALID( this );

CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
HWND hWndBar = pBar->GetSafeHwnd();
	if( pBar->NcButtons_HandleClick( this, point, pBar, NULL ) )
		return true;
	ASSERT_VALID( pBar );

	if( ! m_rc.PtInRect( point ) )
		return false; // continue asking nc-buttons

// 2.25 fix
		m_bHover = m_bPushed = false;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( IsBarAutoHideMode() )
	{
		if(		hWndBar != NULL
			&&	::IsWindow( hWndBar )
			)
			pBar->NcButtons_PostClick( this, point, pBar, NULL );
		return false;
	}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	if( IsBarSingleOnRow() )
	{
		if( ! IsBarWndActive() )
		{
			if( CExtControlBar::stat_QueryFocusChangingEnabled( pBar, pBar->m_hWnd ) )
				pBar->SetFocus();
		}
		if(		hWndBar != NULL
			&&	::IsWindow( hWndBar )
			)
			pBar->NcButtons_PostClick( this, point, pBar, NULL );
		return true;
	}

	if( IsBarMaximizedOnRow() )
		pBar->MinimizeOnRow();
	else
	{
		pBar->MaximizeOnRow();
		if( CExtControlBar::stat_QueryFocusChangingEnabled( pBar, pBar->m_hWnd ) )
			pBar->SetFocus();
	}
	
	if(		hWndBar != NULL
		&&	::IsWindow( hWndBar )
		)
		pBar->NcButtons_PostClick( this, point, pBar, NULL );
	return true;
}

__EXT_MFC_INT_PTR CExtBarNcAreaButtonExpand::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	)
{
	ASSERT_VALID( this );
	
	point; // should be in button area

bool bBarMaximizedOnRow = IsBarMaximizedOnRow();
	return
		DoDefaultReflectForToolHitTest(
			pTI,
			bBarMaximizedOnRow ? IDS_CBNCAREA_CONTRACT : IDS_CBNCAREA_EXPAND,
			bBarMaximizedOnRow ? _T("Contract") : _T("Expand")
			);
}

/////////////////////////////////////////////////////////////////////////////
// CExtBarNcAreaButtonClose

IMPLEMENT_DYNCREATE( CExtBarNcAreaButtonClose, CExtBarNcAreaButton )

CExtBarNcAreaButtonClose::CExtBarNcAreaButtonClose(
	CExtControlBar * pBar
	)
	: CExtBarNcAreaButton( pBar )
{
}

bool CExtBarNcAreaButtonClose::OnQueryVisibility() const
{
	ASSERT_VALID( this );
bool bButtonIsVisible = true;
CExtControlBar * pBar = const_cast < CExtControlBar * > ( GetBar() );
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleQueryVisibility(
			bButtonIsVisible,
			const_cast < CExtBarNcAreaButtonClose * > ( this ),
			pBar,
			NULL
			)
		)
		return bButtonIsVisible;

	if( ! CExtBarNcAreaButton::OnQueryVisibility() )
		return false;
	if( ! pBar->_ClosingIsEnabled( GetBar() ) )
		return false;
	if( ! IsBarFixedMode()  )
		return true;
	return IsBarFloated();
}

void CExtBarNcAreaButtonClose::OnNcAreaDraw( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( OnQueryVisibility() );
	if( m_rc.IsRectEmpty() )
		return;

CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleDraw(
			dc,
			this,
			pBar,
			NULL
			)
		)
		return;

#if (defined __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__)

	dc.FillSolidRect( &m_rc, RGB(128,128,255) );

#else // __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__

	NcDrawDefault(
		dc,
		CExtPaintManager::__DCBT_CLOSE
		);

#endif // !__DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__
}

UINT CExtBarNcAreaButtonClose::OnNcAreaHitTest( CPoint point )
{
	ASSERT_VALID( this );

UINT nExternalHT = HTNOWHERE;
CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleHitTest(
			nExternalHT,
			point,
			this,
			pBar,
			NULL
			)
		)
		return nExternalHT;

UINT nHT = CExtBarNcAreaButton::OnNcAreaHitTest( point );
	if( nHT == HTCLIENT )
		return HTCLIENT; // not inside nc-button
	//return HTCLOSE;
	return HTMINBUTTON;
}

bool CExtBarNcAreaButtonClose::OnNcAreaClicked( CPoint point )
{
	ASSERT_VALID( this );
CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
HWND hWndBar = pBar->GetSafeHwnd();
	if( pBar->NcButtons_HandleClick( this, point, pBar, NULL ) )
		return true;
	ASSERT_VALID( pBar );

	if( m_rc.PtInRect(point) )
	{
		m_bHover = m_bPushed = false;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( ! pBar->IsFixedMode() )
		{
			if( pBar->AutoHideModeGet() )
			{
				pBar->AutoHideModeSet( false, false, true, true );
				pBar->OnShowControlBarChain( false, false );
			}
			else
			{
				CFrameWnd * pFrame = pBar->GetParentFrame();
				ASSERT_VALID( pFrame );
				if(		pFrame->IsKindOf( RUNTIME_CLASS(CMiniDockFrameWnd) )
					&&	(pFrame->GetStyle() & FWS_SNAPTOBARS) != 0
					)
					pFrame->ModifyStyle( FWS_SNAPTOBARS, 0 );
				bool bOnlyOneBar = false;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
				bOnlyOneBar =
					pBar->IsCloseOnlyOneBarInTabGroup();
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
				pBar->OnShowControlBarChain( false, false, bOnlyOneBar );
			}
		}
		else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		{
			bool bOnlyOneBar = false;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			bOnlyOneBar =
				pBar->IsCloseOnlyOneBarInTabGroup();
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			pBar->OnShowControlBarChain( false, false, bOnlyOneBar );
		}
		if(		hWndBar != NULL
			&&	::IsWindow( hWndBar )
			)
			pBar->NcButtons_PostClick( this, point, pBar, NULL );
		return true;
	
	}
	if(		hWndBar != NULL
		&&	::IsWindow( hWndBar )
		)
		pBar->NcButtons_PostClick( this, point, pBar, NULL );
	return false; // continue asking nc-buttons
}

__EXT_MFC_INT_PTR CExtBarNcAreaButtonClose::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	)
{
	ASSERT_VALID( this );
	
	point; // should be in button area
	return
		DoDefaultReflectForToolHitTest(
			pTI,
			IDS_CBNCAREA_CLOSE,
			_T("Hide")
			);
}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

/////////////////////////////////////////////////////////////////////////////
// CExtBarNcAreaButtonAutoHide

IMPLEMENT_DYNCREATE( CExtBarNcAreaButtonAutoHide, CExtBarNcAreaButton )

CExtBarNcAreaButtonAutoHide::CExtBarNcAreaButtonAutoHide(
	CExtControlBar * pBar
	)
	: CExtBarNcAreaButton( pBar )
{
}

bool CExtBarNcAreaButtonAutoHide::OnQueryVisibility() const
{
	ASSERT_VALID( this );
bool bButtonIsVisible = true;
CExtControlBar * pBar = const_cast < CExtControlBar * > ( GetBar() );
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleQueryVisibility(
			bButtonIsVisible,
			const_cast < CExtBarNcAreaButtonAutoHide * > ( this ),
			pBar,
			NULL
			)
		)
		return bButtonIsVisible;

	if( ! CExtBarNcAreaButton::OnQueryVisibility() )
		return false;
	if( ! pBar->_AutoHidingIsEnabled( GetBar() ) )
		return false;
	if( IsBarFixedMode()  )
		return false;
	if( IsBarFloated() )
		return false;
	if( pBar->m_pDockBar == NULL )
		return false;
CExtDockBar * pDockBar = DYNAMIC_DOWNCAST( CExtDockBar, pBar->m_pDockBar );
	if( pDockBar == NULL )
		return false;
	if( pDockBar->_GetAutoHideArea() == NULL )
		return false;
CFrameWnd * pFrame = pBar->GetParentFrame();
	ASSERT_VALID( pFrame );
	if( pFrame->IsKindOf( RUNTIME_CLASS( CMiniFrameWnd ) ) )
		return false;
	return true;
}

void CExtBarNcAreaButtonAutoHide::OnNcAreaDraw( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( OnQueryVisibility() );
	if( m_rc.IsRectEmpty() )
		return;

CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleDraw(
			dc,
			this,
			pBar,
			NULL
			)
		)
		return;

#if (defined __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__)

	dc.FillSolidRect( &m_rc, RGB(255,128,255) );

#else // __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__

	NcDrawDefault(
		dc,
		IsBarAutoHideMode()
			? CExtPaintManager::__DCBT_WND_UNKEEP
			: CExtPaintManager::__DCBT_WND_KEEP
		);

#endif // !__DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__
}

UINT CExtBarNcAreaButtonAutoHide::OnNcAreaHitTest( CPoint point )
{
	ASSERT_VALID( this );

UINT nExternalHT = HTNOWHERE;
CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleHitTest(
			nExternalHT,
			point,
			this,
			pBar,
			NULL
			)
		)
		return nExternalHT;

UINT nHT = CExtBarNcAreaButton::OnNcAreaHitTest( point );
	if( nHT == HTCLIENT )
		return HTCLIENT; // not inside nc-button
	//return HTCLOSE;
	return HTMINBUTTON;
}

bool CExtBarNcAreaButtonAutoHide::OnNcAreaClicked( CPoint point )
{
	ASSERT_VALID( this );
CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
HWND hWndBar = pBar->GetSafeHwnd();
	if( pBar->NcButtons_HandleClick( this, point, pBar, NULL ) )
		return true;
	ASSERT_VALID( pBar );

	if( m_rc.PtInRect( point ) )
	{
		m_bPushed = m_bHover = false;
		ASSERT_VALID( pBar->m_pDockBar );
		bool bAutoHideMode = pBar->AutoHideModeGet();
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynTabBar)) )
		{
			pBar =
				STATIC_DOWNCAST(
					CExtDynTabControlBar,
					pBar->m_pDockBar->GetParent()
					);
		} // if( pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynTabBar)) )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		pBar->AutoHideModeSet( ! bAutoHideMode, false, true, false );
		if(		hWndBar != NULL
			&&	::IsWindow( hWndBar )
			)
			pBar->NcButtons_PostClick( this, point, pBar, NULL );
		return true;
	} // if( m_rc.PtInRect( point ) )
	if(		hWndBar != NULL
		&&	::IsWindow( hWndBar )
		)
		pBar->NcButtons_PostClick( this, point, pBar, NULL );
	return false; // continue asking nc-buttons
}

void CExtBarNcAreaButtonAutoHide::OnNcAreaReposition(
	CExtBarNcAreaButton * pPrevBtn
	)
{
	ASSERT_VALID( this );
CExtControlBar * pExtBar = GetBar();
	ASSERT_VALID( pExtBar );
bool bAutoHideMode = pExtBar->AutoHideModeGet();
	if( ! bAutoHideMode )
		m_bHover = m_bPushed = false;
	CExtBarNcAreaButton::OnNcAreaReposition( pPrevBtn );
}

__EXT_MFC_INT_PTR CExtBarNcAreaButtonAutoHide::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	)
{
	ASSERT_VALID( this );
	
	point; // should be in button area
	return
		DoDefaultReflectForToolHitTest(
			pTI,
			IDS_EXTTABWND_BTN_AUTOHIDE,
			_T("Auto Hide")
			);
}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

/////////////////////////////////////////////////////////////////////////////
// CExtBarNcAreaButtonMenu

IMPLEMENT_DYNCREATE( CExtBarNcAreaButtonMenu, CExtBarNcAreaButton )

CExtBarNcAreaButtonMenu::CExtBarNcAreaButtonMenu(
	CExtControlBar * pBar
	)
	: CExtBarNcAreaButton( pBar )
{
}

bool CExtBarNcAreaButtonMenu::OnQueryVisibility() const
{
	ASSERT_VALID( this );
bool bButtonIsVisible = true;
CExtControlBar * pBar = const_cast < CExtControlBar * > ( GetBar() );
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleQueryVisibility(
			bButtonIsVisible,
			const_cast < CExtBarNcAreaButtonMenu * > ( this ),
			pBar,
			NULL
			)
		)
		return bButtonIsVisible;

	if( ! CExtBarNcAreaButton::OnQueryVisibility() )
		return false;
	if( ! pBar->_NcMenuButtonIsEnabled( GetBar() ) )
		return false;
	if( ( ! IsBarFixedMode() ) || ( ! IsBarFloated() ) )
		return false;
	return true;
}

void CExtBarNcAreaButtonMenu::OnNcAreaDraw( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( OnQueryVisibility() );
	if( m_rc.IsRectEmpty() )
		return;

CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleDraw(
			dc,
			this,
			pBar,
			NULL
			)
		)
		return;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = pBar->NotifiedCustomizeSiteGet();
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		m_bHover = m_bPushed = false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#if (defined __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__)

	dc.FillSolidRect( &m_rc, RGB(255,255,128) );

#else // __DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__

	NcDrawDefault(
		dc,
		CExtPaintManager::__DCBT_ARROW_DOWN
		);

#endif // !__DEBUG_PAINTING_AREAS_EXTRESIZABLEBAR__
}

UINT CExtBarNcAreaButtonMenu::OnNcAreaHitTest( CPoint point )
{
	ASSERT_VALID( this );

UINT nExternalHT = HTNOWHERE;
CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleHitTest(
			nExternalHT,
			point,
			this,
			pBar,
			NULL
			)
		)
		return nExternalHT;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = GetBar()->NotifiedCustomizeSiteGet();
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		return HTCLIENT;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

UINT nHT = CExtBarNcAreaButton::OnNcAreaHitTest( point );
	if( nHT == HTCLIENT )
		return HTCLIENT; // not inside nc-button

	return HTMINBUTTON;
}

bool CExtBarNcAreaButtonMenu::OnNcAreaClicked( CPoint point )
{
	ASSERT_VALID( this );

CExtControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
HWND hWndBar = pBar->GetSafeHwnd();
	if( pBar->NcButtons_HandleClick( this, point, pBar, NULL ) )
		return true;
	ASSERT_VALID( pBar );

// 2.25 fix
		m_bHover = m_bPushed = false;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = pBar->NotifiedCustomizeSiteGet();
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
	{
		if(		hWndBar != NULL
			&&	::IsWindow( hWndBar )
			)
			pBar->NcButtons_PostClick( this, point, pBar, NULL );
		return true;
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( CExtControlBar::FindHelpMode( pBar ) )
	{
		if(		hWndBar != NULL
			&&	::IsWindow( hWndBar )
			)
			pBar->NcButtons_PostClick( this, point, pBar, NULL );
		return true;
	}

	if( ! m_rc.PtInRect( point ) )
	{
		if(		hWndBar != NULL
			&&	::IsWindow( hWndBar )
			)
			pBar->NcButtons_PostClick( this, point, pBar, NULL );
		return false; // continue asking nc-buttons
	}

CFrameWnd * pFrame = GetDockingFrame();
	ASSERT_VALID( pFrame );

HWND hWndTrack = pFrame->GetSafeHwnd();
	ASSERT(
			hWndTrack != NULL
		&&	::IsWindow( hWndTrack )
		);

CExtPopupMenuWnd * pPopup =
		CExtPopupMenuWnd::InstantiatePopupMenu(
			pBar->GetSafeHwnd(),
			RUNTIME_CLASS(CExtPopupMenuWnd),
			this
			);
	if( ! pPopup->CreatePopupMenu( hWndTrack ) )
	{
		ASSERT( FALSE );
		if(		hWndBar != NULL
			&&	::IsWindow( hWndBar )
			)
			pBar->NcButtons_PostClick( this, point, pBar, NULL );
		return false;
	}

CExtControlBar::POPUP_MENU_EVENT_DATA _pmed(
		CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_CONTROLBAR_NCBTNMENU_TOP,
		pPopup,
		pBar,
		CPoint( -1, -1 ),
		WM_NCLBUTTONDOWN,
		this
		);
	if( ! _pmed.NotifyTarget( false ) )
	{
		CExtLocalResourceHelper _LRH;
		CExtSafeString sShowHidePanels;
		if( ! g_ResourceManager->LoadString( sShowHidePanels, IDS_SHOW_HIDE_PANELS ) )
			sShowHidePanels = _T("&Show/hide panels");

		VERIFY(
			pPopup->ItemInsert(
				(UINT)CExtPopupMenuWnd::TYPE_POPUP,
				-1,
				sShowHidePanels
				)
			);
		CExtPopupMenuWnd::MENUITEMDATA & mi = pPopup->ItemGetInfo( pPopup->ItemGetCount() - 1 );
		mi.SetChildCombine();
		mi.SetToolButton();
		CExtPopupMenuWnd * pPopupBars = mi.GetPopup();
		ASSERT( pPopupBars != NULL );

		if(	! CExtDockBar::_ConstructDockSiteControlBarPopupMenu(
				pFrame,
				pPopupBars,
				CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_CONTROLBAR_NCBTNMENU_BARS,
				pBar,
				WM_NCLBUTTONDOWN,
				this
				)
			)
		{
			VERIFY(
				pPopup->ItemRemove(
					pPopup->ItemGetCount() - 1
					)
				);
		}
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		if(		pBar->IsKindOf( RUNTIME_CLASS(CExtToolControlBar) )
			&&	((CExtToolControlBar*)pBar)->m_bCustomizationAllowed
			&&	((CExtToolControlBar*)pBar)->GetCustomizeSite() != NULL
			)
		{ // if customization allowed
			VERIFY(
				CExtDockBar::_AppendAddRemoveButtonsCustomizeMenu(
					pPopup,
					m_pBar
					)
				);
		} // if customization allowed
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	} // if( ! _pmed.NotifyTarget( false ) )
	_pmed.NotifyTarget( true );

	if( pPopup->ItemGetCount() == 0 )
	{
		delete pPopup;
		if(		hWndBar != NULL
			&&	::IsWindow( hWndBar )
			)
			pBar->NcButtons_PostClick( this, point, pBar, NULL );
		return false;
	}

CRect rcBarWnd;
	VERIFY( GetBarNcAreaInfo( NULL, &rcBarWnd ) );
CRect rcBtnScreen( m_rc );
	rcBtnScreen.OffsetRect( rcBarWnd.TopLeft() );
	rcBtnScreen.InflateRect( 1, 1 );

CPoint ptCursor = rcBtnScreen.CenterPoint();
	if( CExtControlBar::stat_QueryFocusChangingEnabled( pBar, hWndTrack ) )
		::SetFocus( hWndTrack );
	if(	! pPopup->TrackPopupMenu(
			TPMX_TOPALIGN|TPMX_COMBINE_DEFAULT|TPMX_OWNERDRAW_FIXED,
			ptCursor.x,  ptCursor.y,
			&rcBtnScreen,
			this,
			_CbPaintCombinedContent,
			NULL,
			true
			)
		)
	{
		//delete pPopup;
		if(		hWndBar != NULL
			&&	::IsWindow( hWndBar )
			)
			pBar->NcButtons_PostClick( this, point, pBar, NULL );
		return false;
	}
	
	if(		hWndBar != NULL
		&&	::IsWindow( hWndBar )
		)
		pBar->NcButtons_PostClick( this, point, pBar, NULL );
	return true;
}

__EXT_MFC_INT_PTR CExtBarNcAreaButtonMenu::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	)
{
	ASSERT_VALID( this );
	
	point; // should be in button area
	return
		DoDefaultReflectForToolHitTest(
			pTI,
			IDS_CBNCAREA_MENU,
			_T("Options")
			);
}

void CExtBarNcAreaButtonMenu::_CbPaintCombinedContent(
	LPVOID pCookie,
	CDC & dc,
	const CWnd & refWndMenu,
	const CRect & rcExcludeArea, // in screen coords
	int eCombineAlign // CExtPopupMenuWnd::e_combine_align_t values
	)
{
	pCookie;
	dc;
	refWndMenu;
	rcExcludeArea;
	eCombineAlign;
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( refWndMenu.GetSafeHwnd() != NULL );

CExtBarNcAreaButtonMenu * pBtn = (CExtBarNcAreaButtonMenu *)pCookie;
	ASSERT( pBtn != NULL );
	ASSERT_VALID( pBtn );
	ASSERT( pBtn->IsKindOf(RUNTIME_CLASS(CExtBarNcAreaButtonMenu)) );

	ASSERT( eCombineAlign != CExtPopupMenuWnd::__CMBA_NONE );

	if( rcExcludeArea.IsRectEmpty() )
		return;

CRect rcBarWnd;
	VERIFY( pBtn->GetBarNcAreaInfo( NULL, &rcBarWnd ) );
CRect rcExcludeAreaX;
	refWndMenu.GetClientRect( &rcExcludeAreaX );
	refWndMenu.ClientToScreen( &rcExcludeAreaX );
CPoint ptOffset =
		rcBarWnd.TopLeft()
		- rcExcludeAreaX.TopLeft()
		;
CPoint ptViewportOrg = dc.GetViewportOrg();
	dc.SetViewportOrg( ptOffset );
	pBtn->m_bPaintingCombinedArea = true;
	pBtn->OnNcAreaDraw( dc );
	pBtn->m_bPaintingCombinedArea = false;
	dc.SetViewportOrg( ptViewportOrg );
}

LRESULT CExtControlBar::OnSizeParent(WPARAM wParam, LPARAM lParam)
{
	wParam;
	if( !m_bReflectParentSizing )
		return (LRESULT)0;
//	if( CExtControlBar::g_bUpdatingDragState )
//		return 0;

AFX_SIZEPARENTPARAMS * lpLayout =
		(AFX_SIZEPARENTPARAMS *) lParam;
	ASSERT( lpLayout != NULL );
DWORD dwStyle = RecalcDelayShow( lpLayout );
	if(		( (dwStyle&WS_VISIBLE) != 0 )
		&&	( (dwStyle&CBRS_ALIGN_ANY) != 0 )
		)
	{
		CRect rect = lpLayout->rect;
		CSize sizeAvail = rect.Size();
		DWORD dwMode = lpLayout->bStretch ? LM_STRETCH : 0;
		if(		( (m_dwStyle&CBRS_SIZE_DYNAMIC) != 0 )
			&&	( (m_dwStyle&CBRS_FLOATING ) != 0 )
			)
			dwMode |= LM_HORZ|LM_MRUWIDTH;
		else if( (dwStyle&CBRS_ORIENT_HORZ) != 0 )
			dwMode |= LM_HORZ|LM_HORZDOCK;
		else
			dwMode |=  LM_VERTDOCK;
		CSize size = CalcDynamicLayout( -1, dwMode );
		size.cx = min( size.cx, sizeAvail.cx );
		size.cy = min( size.cy, sizeAvail.cy );
		if( (dwStyle&CBRS_ORIENT_HORZ) != 0 )
		{
			lpLayout->sizeTotal.cy += size.cy;
			lpLayout->sizeTotal.cx = max( lpLayout->sizeTotal.cx, size.cx );
			if(	(dwStyle&CBRS_ALIGN_TOP) != 0 )
				lpLayout->rect.top += size.cy;
			else if( (dwStyle&CBRS_ALIGN_BOTTOM) != 0 )
			{
				rect.top = rect.bottom - size.cy;
				lpLayout->rect.bottom -= size.cy;
			}
		}
		else if( (dwStyle&CBRS_ORIENT_VERT) != 0 )
		{
			lpLayout->sizeTotal.cx += size.cx;
			lpLayout->sizeTotal.cy = max( lpLayout->sizeTotal.cy, size.cy );
			HWND hWndParent = ::GetParent( m_hWnd );
			ASSERT( hWndParent != NULL && ::IsWindow(hWndParent) );
			__EXT_MFC_ULONG_PTR dwStyleEx = ::__EXT_MFC_GetWindowLong( hWndParent, GWL_EXSTYLE );
			bool bParentRTL =
				(	(m_pDockBar != NULL)
				&&	( (dwStyleEx&WS_EX_LAYOUTRTL) != 0 )
				)
				? true : false;
			if( bParentRTL )
			{
				if( (dwStyle&CBRS_ALIGN_RIGHT) != 0 )
					lpLayout->rect.left += size.cx;
				else if( (dwStyle&CBRS_ALIGN_LEFT) != 0 )
				{
					rect.left = rect.right - size.cx;
					lpLayout->rect.right -= size.cx;
				}
			} // if( bParentRTL )
			else
			{
				if( (dwStyle&CBRS_ALIGN_LEFT) != 0 )
					lpLayout->rect.left += size.cx;
				else if( (dwStyle&CBRS_ALIGN_RIGHT) != 0 )
				{
					rect.left = rect.right - size.cx;
					lpLayout->rect.right -= size.cx;
				}
			} // else from if( bParentRTL )
		}
#ifdef _DEBUG
		else
		{
			ASSERT(FALSE);
		}
#endif // _DEBUG
		rect.right = rect.left + size.cx;
		rect.bottom = rect.top + size.cy;
		if( lpLayout->hDWP != NULL )
			AfxRepositionWindow( lpLayout, m_hWnd, &rect );
	}
	return 0L;
}

CExtControlBar * CExtControlBar::InternalDraggingState_t::ExtBarDstSet(
	CExtControlBar * pExtBarDst, // = NULL
	bool bNoCheckOnShutdown // = false
	)
{
	bNoCheckOnShutdown;
	ASSERT( this != NULL );
//	ASSERT_VALID( this );
CExtControlBar * pExtBarDstOld = m_pExtBarDst;
	m_pExtBarDst = pExtBarDst;
//	ASSERT_VALID( this );

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if(		(!bNoCheckOnShutdown)
		&&	pExtBarDstOld != NULL
		&&	pExtBarDstOld != m_pExtBarDst )
	{
		CExtDynTabControlBar * pTabbedBar =
			DYNAMIC_DOWNCAST(
				CExtDynTabControlBar,
				pExtBarDstOld
				);
		if( pTabbedBar != NULL )
			pTabbedBar->RemoveTemporaryItems( false );
	} // if( pExtBarDstOld != NULL && pExtBarDstOld != m_pExtBarDst )
	m_nTabReindex = 0;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	return pExtBarDstOld;
}

void CExtControlBar::InternalDraggingState_t::_AssignFromOther(
	const CExtControlBar::InternalDraggingState_t & other
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( (&other) );

	if( m_pExtBarDst != NULL
		&& m_pExtBarDst != other.m_pExtBarDst
		)
	{
		ExtBarDstSet( other.m_pExtBarDst );
	} // if( m_pExtBarDst ....

	m_bEmpty = other.m_bEmpty;
	m_bDeepHalfSplit = other.m_bDeepHalfSplit;
	m_eCDT = other.m_eCDT;
	m_eCFR = other.m_eCFR;
	m_nCircleNo = other.m_nCircleNo;
	m_rcTrack = other.m_rcTrack;
	m_pExtBarSrc = other.m_pExtBarSrc;
	m_pExtBarDst = other.m_pExtBarDst;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	m_nTabReindex = other.m_nTabReindex;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	ASSERT_VALID( this );
}

void CExtControlBar::InternalDraggingState_t::SetEmpty(
	bool bEmpty // = true
	)
{
//	ASSERT_VALID( this );
	ASSERT( this != NULL );

	m_eCFR = __ECFR_NO_RESIZING;
	if( bEmpty )
		ExtBarDstSet( NULL );
	
	m_bEmpty = bEmpty;
	if( m_bEmpty )
	{
		m_rcTrack.SetRectEmpty();
//		m_pExtBarDst = NULL;
	}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	m_nTabReindex = 0;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
void CExtControlBar::InternalDraggingState_t::ReindexPosCalc( CPoint ptCursor )
{
	ASSERT_VALID( this );
	ASSERT( !IsEmpty() );
	ASSERT(
			m_eCDT == __ECDT_TABBED_REINDEX
		||	m_eCDT == __ECDT_TABBED_REINDEX_NHM
		);
	ASSERT_VALID( m_pExtBarDst );
CExtDynTabControlBar * pTabbedBar =
		STATIC_DOWNCAST(
			CExtDynTabControlBar,
			m_pExtBarDst
			);
	m_nTabReindex = pTabbedBar->ReindexPosCalc( ptCursor );
}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

void CExtControlBar::InternalDraggingState_t::DrawState( bool bErasingPrevious )
{
	ASSERT_VALID( this );
	//ASSERT( ! IsEmpty() );
	ASSERT_VALID( m_pExtBarSrc );
	ASSERT_VALID( m_pExtBarSrc->m_pDockSite );
	ASSERT_VALID( m_pExtBarSrc->m_pDockBar );
	if( m_eCDT == __ECDT_OWN_AREA )
		return;
	if( m_pExtBarSrc->IsFixedMode() )
		return;
	
CExtControlBar::eResizablePanelDockingType_t eResizablePanelDockingType =
		m_pExtBarSrc->OnQueryDockingType();
	if( m_pExtBarSrc->_IsShowContentWhenDragging() )
	{
#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
		if( INT(eResizablePanelDockingType) < INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005) )
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)
			return;
	} // if( m_pExtBarSrc->_IsShowContentWhenDragging() )
	else
	{
		if( m_rcTrack.IsRectEmpty() )
			return;
	} // else from if( m_pExtBarSrc->_IsShowContentWhenDragging() )

	eResizablePanelDockingType;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if(		(	m_eCDT == __ECDT_TABBED_UPDATE
			||	m_eCDT == __ECDT_TABBED_REINDEX
			||	m_eCDT == __ECDT_TABBED_REINDEX_NHM
			)
		&&	m_pExtBarDst != NULL
		)
	{
		ASSERT_VALID( m_pExtBarDst );
		ASSERT_VALID( m_pExtBarDst->m_pDockSite );
		ASSERT_VALID( m_pExtBarDst->m_pDockBar );
		ASSERT( ! m_pExtBarDst->IsFixedMode() );

		ASSERT_KINDOF( CExtControlBar, m_pExtBarSrc );
		ASSERT_KINDOF( CExtDynTabControlBar, m_pExtBarDst );

#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
		if(		INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005)
			&&	m_pExtBarSrc->IsFloating()
			)
			m_pExtBarSrc->GetParentFrame()->ShowWindow( SW_HIDE );
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)

		if(		m_eCDT == __ECDT_TABBED_REINDEX
			||	m_eCDT == __ECDT_TABBED_REINDEX_NHM
			)
			((CExtDynTabControlBar*)m_pExtBarDst) ->
				RemoveTemporaryItems( false );
		((CExtDynTabControlBar*)m_pExtBarDst) ->
			InsertTemporaryItemsFrom(
				m_pExtBarSrc,
				ReindexPosGet(),
				false
				);
		return;
	}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
	if( INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005) )
	{
		if( m_pExtBarSrc->IsFloating() )
			m_pExtBarSrc->GetParentFrame()->ShowWindow( SW_SHOWNOACTIVATE );
		return;
	} // if( INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005) )
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)

CRect rcTrackerOuter( m_rcTrack ), rcDockingFrame, rcDockBarWnd;
	m_pExtBarSrc->m_pDockSite->GetWindowRect( &rcDockingFrame );

	if( m_pExtBarDst != NULL )
	{
		ASSERT_VALID( m_pExtBarDst );
		ASSERT_VALID( m_pExtBarDst->m_pDockSite );
		ASSERT_VALID( m_pExtBarDst->m_pDockBar );
		ASSERT( !m_pExtBarDst->IsFixedMode() );

		m_pExtBarDst->m_pDockBar->GetWindowRect( &rcDockBarWnd );
		if( rcDockBarWnd.IsRectEmpty() )
			return;
		if( rcTrackerOuter.left < rcDockBarWnd.left )
			rcTrackerOuter.left = rcDockBarWnd.left;
		if( rcTrackerOuter.top < rcDockBarWnd.top )
			rcTrackerOuter.top = rcDockBarWnd.top;
		if( rcTrackerOuter.right > rcDockBarWnd.right )
			rcTrackerOuter.right = rcDockBarWnd.right;
		if( rcTrackerOuter.bottom > rcDockBarWnd.bottom )
			rcTrackerOuter.bottom = rcDockBarWnd.bottom;

		if( rcTrackerOuter.IsRectEmpty() )
			return;
	} // if( m_pExtBarDst != NULL )

	rcTrackerOuter.OffsetRect( -rcDockingFrame.TopLeft() );

INT nTrackerBorderH = ::GetSystemMetrics( SM_CXSIZEFRAME );
INT nTrackerMinWidth = nTrackerBorderH*2+1;
	if( rcTrackerOuter.Width() < nTrackerMinWidth )
		return;
INT nTrackerBorderV = ::GetSystemMetrics( SM_CYSIZEFRAME );
INT nTrackerMinHeight = nTrackerBorderV*2+1;
	if( rcTrackerOuter.Height() < nTrackerMinHeight )
		return;

CWnd * pWndDrawTracker = NULL;
	if(		m_pExtBarDst != NULL
		||	m_eCDT == __ECDT_DOCKBAR_INNER_TOP
		||	m_eCDT == __ECDT_DOCKBAR_INNER_BOTTOM
		||	m_eCDT == __ECDT_DOCKBAR_INNER_LEFT
		||	m_eCDT == __ECDT_DOCKBAR_INNER_RIGHT
		||	m_eCDT == __ECDT_CIRCLE1_OUTER_TOP
		||	m_eCDT == __ECDT_CIRCLE1_OUTER_BOTTOM
		||	m_eCDT == __ECDT_CIRCLE1_OUTER_LEFT
		||	m_eCDT == __ECDT_CIRCLE1_OUTER_RIGHT
		)
	{
		if( m_pExtBarDst != NULL )
		{
			pWndDrawTracker = m_pExtBarDst->GetDockingFrame();
			if( m_pExtBarSrc->m_pDockSite != pWndDrawTracker )
			{
				rcTrackerOuter.OffsetRect( rcDockingFrame.TopLeft() );
				CRect rc;
				pWndDrawTracker->GetClientRect( &rc );
				pWndDrawTracker->ClientToScreen( &rc );
				rcTrackerOuter.OffsetRect( -rc.TopLeft() );
				rcTrackerOuter.OffsetRect(-1,-1);
				::ClientToScreen( pWndDrawTracker->m_hWnd, ((LPPOINT)(&rcTrackerOuter)+0) );
				::ClientToScreen( pWndDrawTracker->m_hWnd, ((LPPOINT)(&rcTrackerOuter)+1) );
				pWndDrawTracker = CWnd::GetDesktopWindow();
			} // if( m_pExtBarSrc->m_pDockSite != pWndDrawTracker )
		}
		else
			pWndDrawTracker = m_pExtBarSrc->m_pDockSite;
	} // if( m_pExtBarDst != NULL )
	else
		pWndDrawTracker = CWnd::GetDesktopWindow();
	ASSERT( pWndDrawTracker != NULL );
	ASSERT( pWndDrawTracker->GetSafeHwnd() != NULL );
CDC * pDcDrawTracker =
		pWndDrawTracker->GetDCEx( NULL, __DOCKSITE_DCX_FLAGS );
	ASSERT_VALID( pDcDrawTracker );
	ASSERT( pDcDrawTracker->GetSafeHdc() != NULL );
DWORD dwOldLayout = pDcDrawTracker->SetLayout( LAYOUT_LTR );
	if( pDcDrawTracker->RectVisible(&rcTrackerOuter) )
	{
		MsgDrawTrackedAreaData_t _MsgDrawTrackedAreaData(
			*m_pExtBarSrc,
			*pDcDrawTracker,
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			(m_eCDT == __ECDT_TABBED_NEW)
				?	__EDTAT_TAB_FRAME
				:
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
					__EDTAT_RECTANGULAR_FRAME
				,
			rcTrackerOuter,
			bErasingPrevious
			);
		_MsgDrawTrackedAreaData.NotifyOrDrawDefault();
	} // if( pDcDrawTracker->RectVisible(&rcTrackerOuter) )
	pDcDrawTracker->SetLayout( dwOldLayout );
    pWndDrawTracker->ReleaseDC( pDcDrawTracker );
}

CExtControlBar::OuterItemData_t::OuterItemData_t()
	: m_nBarID( AFX_IDW_DOCKBAR_FLOAT )
	, m_ptFloatHelper( 0, 0 )
	, m_sizeDockedH( 200, 200 )
	, m_sizeDockedV( 200, 200 )
	, m_sizeFloated( 200, 200 )
	, m_bFloating( false )
	, m_bVisible( false )
{
	ASSERT( m_AffixmentData.IsEmpty() );
}

CExtControlBar::OuterItemData_t::OuterItemData_t(
	const CExtControlBar::OuterItemData_t & other
	)
{
	m_nBarID = other.m_nBarID;
	m_ptFloatHelper = other.m_ptFloatHelper;
	m_sizeDockedH = other.m_sizeDockedH;
	m_sizeDockedV = other.m_sizeDockedV;
	m_sizeFloated = other.m_sizeFloated;
	m_AffixmentData = other.m_AffixmentData;
	m_bFloating = other.m_bFloating;
	m_bVisible = other.m_bVisible;
}

CExtControlBar::OuterItemData_t &
	CExtControlBar::OuterItemData_t::operator = (
		const CExtControlBar::OuterItemData_t & other
	)
{
	m_nBarID = other.m_nBarID;
	m_ptFloatHelper = other.m_ptFloatHelper;
	m_sizeDockedH = other.m_sizeDockedH;
	m_sizeDockedV = other.m_sizeDockedV;
	m_sizeFloated = other.m_sizeFloated;
	m_AffixmentData = other.m_AffixmentData;
	m_bFloating = other.m_bFloating;
	m_bVisible = other.m_bVisible;
	return (*this);
}

void CExtControlBar::OuterItemData_t::Serialize( CArchive & ar )
{
DWORD dwHelperFlags = 0;
	if( ar.IsStoring() )
	{
		if( m_bFloating )
			dwHelperFlags |= 0x01;
		if( m_bVisible )
			dwHelperFlags |= 0x02;
		ar << dwHelperFlags;
		ar << DWORD(m_nBarID);
		ar << m_ptFloatHelper;
		ar << m_sizeDockedH;
		ar << m_sizeDockedV;
		ar << m_sizeFloated;
	} // if( ar.IsStoring() )
	else
	{
		m_nBarID = 0;
		m_ptFloatHelper.x = m_ptFloatHelper.y = 0;
		m_sizeDockedH.cx = m_sizeDockedH.cy
			= m_sizeDockedV.cx = m_sizeDockedV.cy
			= m_sizeFloated.cx = m_sizeFloated.cy
			= 0;
		m_AffixmentData.Empty();
		ar >> dwHelperFlags;
		m_bFloating = m_bVisible = false;
		if( (dwHelperFlags & 0x01) != 0 )
			m_bFloating = true;
		if( (dwHelperFlags & 0x02) != 0 )
			m_bVisible = true;
		DWORD dwTmp;
		ar >> dwTmp;
		m_nBarID = UINT(dwTmp);
		ar >> m_ptFloatHelper;
		ar >> m_sizeDockedH;
		ar >> m_sizeDockedV;
		ar >> m_sizeFloated;
	} // else from if( ar.IsStoring() )
	m_AffixmentData.Serialize( ar );
}

CExtControlBar::InternalBarRepositionData_t::InternalBarRepositionData_t(
	CControlBar * pBar, // = NULL
	bool bSetAdvData // = false
	)
	: m_pBar( NULL )
	, m_rcSrc( 0, 0, 0, 0 )
	, m_rcDst( 0, 0, 0, 0 )
	, m_nSpaceAvailByBar( 0 )
	, m_nSpaceAvailBySideBefore( 0 )
	, m_nSpaceAvailBySideAfter( 0 )
{
	if( pBar != NULL )
		SetBar( pBar, bSetAdvData );
}

CExtControlBar::InternalBarRepositionData_t::InternalBarRepositionData_t(
	const InternalBarRepositionData_t & other
	)
{
	_AssignFromOther( other );
}

void CExtControlBar::InternalBarRepositionData_t::_AssignFromOther(
	const CExtControlBar::InternalBarRepositionData_t & other
	)
{
	m_pBar = other.m_pBar;
	m_rcSrc = other.m_rcSrc;
	m_rcDst = other.m_rcDst;
	m_nSpaceAvailByBar = other.m_nSpaceAvailByBar;
	m_nSpaceAvailBySideBefore = other.m_nSpaceAvailBySideBefore;
	m_nSpaceAvailBySideAfter = other.m_nSpaceAvailBySideAfter;
}

void CExtControlBar::InternalBarRepositionData_t::SetBar(
	CControlBar * pBar,
	bool bSetAdvData // = false
	)
{
	ASSERT_VALID( pBar );
	m_pBar = pBar;
	m_pBar->GetWindowRect( m_rcSrc );
	m_rcDst = m_rcSrc;
	m_nSpaceAvailByBar = 0;
	m_nSpaceAvailBySideBefore = 0;
	m_nSpaceAvailBySideAfter = 0;

	if( !bSetAdvData )
		return;

CExtControlBar * pExtBar = GetExtBarFixed();
	if( pExtBar == NULL )
		return;
	if( pExtBar->IsFloating() )
		return;
bool bHorz = pExtBar->IsDockedHorizontally();

CRect rcClient;
	pExtBar->GetClientRect( &rcClient );
	m_nSpaceAvailByBar = bHorz
		? m_rcSrc.Width() - rcClient.Width()
		: m_rcSrc.Height() - rcClient.Height();
	ASSERT( m_nSpaceAvailByBar > 0 );

//	pExtBar->ClientToScreen( &rcClient );
//CRect rcResetFixedSize(
//		m_rcSrc.TopLeft(),
//		bHorz ? pExtBar->m_sizeDockedH : pExtBar->m_sizeDockedV
//		);
//	rcResetFixedSize.left += rcClient.left - m_rcSrc.left;
//	rcResetFixedSize.top += rcClient.top - m_rcSrc.top;
//	rcResetFixedSize.right += m_rcSrc.right - rcClient.right;
//	rcResetFixedSize.bottom += m_rcSrc.bottom - rcClient.bottom;
//	m_rcSrc = rcResetFixedSize;

	ASSERT_VALID( pExtBar->m_pDockBar );
	ASSERT( pExtBar->m_pDockBar->IsWindowVisible() );
CRect wrDockBar;
	pExtBar->m_pDockBar->GetWindowRect( &wrDockBar );

CControlBar * pBarPrev =
		pExtBar->_GetNextControlBarInRow( pExtBar, false );
	while( !pBarPrev->IsVisible() )
	{
		CControlBar * pLastBar2 =
			_GetNextControlBarInRow( pBarPrev, false );
		if( pLastBar2 == NULL )
			break;
		pBarPrev = pLastBar2;
	}
	if( pBarPrev != NULL )
	{
		CRect wrPrevBar;
		pBarPrev->GetWindowRect( &wrPrevBar );
		INT m_nSpaceAvailBySideBefore = bHorz
			? m_rcSrc.left - wrPrevBar.right
			: m_rcSrc.top - wrPrevBar.bottom;
		ASSERT( m_nSpaceAvailBySideBefore >= 0 );
	} // if( pBarPrev != NULL )
	else
	{
		INT m_nSpaceAvailBySideBefore = bHorz
			? m_rcSrc.left - wrDockBar.left
			: m_rcSrc.top - wrDockBar.top;
		ASSERT( m_nSpaceAvailBySideBefore >= 0 );
	} // else from if( pBarPrev != NULL )

CControlBar * pBarNext =
		pExtBar->_GetNextControlBarInRow( pExtBar, true );
	while( !pBarNext->IsVisible() )
	{
		CControlBar * pLastBar2 =
			_GetNextControlBarInRow( pBarNext, true );
		if( pLastBar2 == NULL )
			break;
		pBarNext = pLastBar2;
	}
	if( pBarNext != NULL )
	{
		CRect wrNextBar;
		pBarNext->GetWindowRect( &wrNextBar );
		INT m_nSpaceAvailBySideAfter = bHorz
			? m_rcSrc.left - wrNextBar.right
			: m_rcSrc.top - wrNextBar.bottom;
		ASSERT( m_nSpaceAvailBySideAfter >= 0 );
	} // if( pBarNext != NULL )
	else
	{
		INT m_nSpaceAvailBySideBefore = bHorz
			? wrDockBar.right - m_rcSrc.right
			: wrDockBar.bottom - m_rcSrc.bottom;
		ASSERT( m_nSpaceAvailBySideBefore >= 0 );
	} // else from if( pBarNext != NULL )
}

CExtControlBar * CExtControlBar::InternalBarRepositionData_t::GetExtBar()
{
	if( m_pBar == NULL )
		return NULL;
CExtControlBar * pExtBar =
	DYNAMIC_DOWNCAST( CExtControlBar, m_pBar );
	return pExtBar;
}

CExtControlBar * CExtControlBar::InternalBarRepositionData_t::GetExtBarFixed()
{
CExtControlBar * pExtBar = GetExtBar();
	if( pExtBar == NULL )
		return NULL;
	if( pExtBar->IsFixedMode() )
		return NULL;
	return pExtBar;
}

void CExtControlBar::InternalDraggingState_t::_DoFixed_FloatingAdjustment(
	CPoint ptTrack // in screen coords
	)
{
	ASSERT_VALID( m_pExtBarSrc );

	g_DragStateOld.SetCFR( __ECFR_NO_RESIZING );

CRect rcGrip = m_pExtBarSrc->_RectGripGet();
INT nCyGripper = min( rcGrip.Width(), rcGrip.Height() );
CSize sizeOffset( nCyGripper, nCyGripper/2 );

bool bAlreadyFloat = m_pExtBarSrc->IsFloating();
	if( !bAlreadyFloat )
	{
		m_pExtBarSrc->m_ptFloatHelper =
			ptTrack - sizeOffset;

		if( ((CExtMouseCaptureSink *)m_pExtBarSrc)->GetCapture() != m_pExtBarSrc->GetSafeHwnd() )
			((CExtMouseCaptureSink *)m_pExtBarSrc)->SetCapture( m_pExtBarSrc->GetSafeHwnd() );
		m_pExtBarSrc->FloatControlBar();

		ASSERT( m_pExtBarSrc->m_pDockContext != NULL );
		m_pExtBarSrc->m_pDockContext->m_ptMRUFloatPos =
			m_pExtBarSrc->m_ptFloatHelper;

		if( ((CExtMouseCaptureSink *)m_pExtBarSrc)->GetCapture() != m_pExtBarSrc->GetSafeHwnd() )
			((CExtMouseCaptureSink *)m_pExtBarSrc)->SetCapture( m_pExtBarSrc->GetSafeHwnd() );
		return;
	} // if( !bAlreadyFloat )

CFrameWnd * pFloatingFrame = m_pExtBarSrc->GetParentFrame();
	ASSERT_VALID( pFloatingFrame );
	ASSERT_KINDOF( CMiniDockFrameWnd, pFloatingFrame );
CRect wrFloatingFrame;
	pFloatingFrame->GetWindowRect( &wrFloatingFrame );
	wrFloatingFrame.OffsetRect(
		-wrFloatingFrame.TopLeft() + (ptTrack-sizeOffset)
		);
	wrFloatingFrame.OffsetRect(
		- m_pExtBarSrc->m_ptHelperDragOffsetFloated
		);
//	
//CExtPaintManager::monitor_parms_t _mp;
//	CExtPaintManager::stat_GetMonitorParms( _mp, m_pExtBarSrc );
//	if( wrFloatingFrame.top < _mp.m_rcWorkArea.top )
//		wrFloatingFrame.OffsetRect(
//			0,
//			_mp.m_rcWorkArea.top - wrFloatingFrame.top
//			);
//	if( wrFloatingFrame.left < _mp.m_rcWorkArea.left )
//		wrFloatingFrame.OffsetRect(
//			_mp.m_rcWorkArea.left - wrFloatingFrame.left,
//			0
//			);
//	if( wrFloatingFrame.top + afxData.cyBorder2*2 + nCyGripper > _mp.m_rcWorkArea.bottom )
//		wrFloatingFrame.OffsetRect(
//			0,
//			_mp.m_rcWorkArea.bottom - (wrFloatingFrame.top + afxData.cyBorder2*2 + nCyGripper)
//			);
//	if( ptTrack.x > _mp.m_rcWorkArea.right )
//		wrFloatingFrame.OffsetRect(
//			_mp.m_rcWorkArea.right - ptTrack.x,
//			0
//			);
	pFloatingFrame->MoveWindow( &wrFloatingFrame );
	if( !m_pExtBarSrc->_IsDockSiteCustomizeMode() )
		pFloatingFrame->BringWindowToTop();
	pFloatingFrame->DelayRecalcLayout();

	ASSERT( m_pExtBarSrc->m_pDockContext != NULL );
	m_pExtBarSrc->m_pDockContext->m_ptMRUFloatPos =
		wrFloatingFrame.TopLeft();
	
	if( ((CExtMouseCaptureSink *)m_pExtBarSrc)->GetCapture() != m_pExtBarSrc->GetSafeHwnd() )
		((CExtMouseCaptureSink *)m_pExtBarSrc)->SetCapture( m_pExtBarSrc->GetSafeHwnd() );
}

bool CExtControlBar::InternalDraggingState_t::_DoFixed_OuterDockerAdjustment(
	CPoint ptTrack, // in screen coords
	CExtDockOuterBar * pOuterDocker
	)
{
	ASSERT_VALID( m_pExtBarSrc );
	ASSERT_VALID( m_pExtBarSrc->m_pDockSite );
	ASSERT_VALID( pOuterDocker );

	g_DragStateOld.SetCFR( __ECFR_NO_RESIZING );

	if(	(		pOuterDocker->m_dwStyle 
			&	m_pExtBarSrc->m_dwDockStyle
			&	CBRS_ALIGN_ANY
			) == 0
		)
		return false;

CRect rcGrip = m_pExtBarSrc->_RectGripGet();
INT nCyGripper = min( rcGrip.Width(), rcGrip.Height() );
CSize sizeOffset( nCyGripper, nCyGripper/2 );

UINT nDockBarID = pOuterDocker->GetDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );
BOOL bHorzDockBar = 
		(	nDockBarID == AFX_IDW_DOCKBAR_TOP
		||	nDockBarID == AFX_IDW_DOCKBAR_BOTTOM )
		? TRUE : FALSE;

	if( bHorzDockBar )
	{
		sizeOffset.cx = m_pExtBarSrc->m_ptHelperDragOffsetDockedH.x;
		sizeOffset.cy = m_pExtBarSrc->m_ptHelperDragOffsetDockedH.y;
	}
	else
	{
		sizeOffset.cx = m_pExtBarSrc->m_ptHelperDragOffsetDockedV.x;
		sizeOffset.cy = m_pExtBarSrc->m_ptHelperDragOffsetDockedV.y;
	}

	if( pOuterDocker != m_pExtBarSrc->m_pDockBar )
	{
		CSize sizeDocked =
			m_pExtBarSrc->CalcFixedLayout(
				FALSE,
				bHorzDockBar
				);
		ASSERT_VALID( m_pExtBarSrc->m_pDockBar );
		CRect rcInsert( ptTrack - sizeOffset, sizeDocked );
		m_pExtBarSrc->_AffixmentSafeClearOuter();
		pOuterDocker->DockControlBar(
			m_pExtBarSrc,
			rcInsert,
			true
			);
		if(		m_pExtBarSrc->_IsDockSiteCustomizeMode()
			&&	(!m_pExtBarSrc->IsSingleOnRow())
			)
			m_pExtBarSrc->MakeSingleOnRow();
		m_pExtBarSrc->_AffixmentRecalcOuter();
		m_pExtBarSrc->m_pDockSite->RecalcLayout();
		return true;
	} // if( pOuterDocker != m_pExtBarSrc->m_pDockBar )

bool bSwap = false;
CRect rcAllRow;
MfcControlBarVector_t vRow;
	m_pExtBarSrc->_GetRowBars(
		vRow,
		&rcAllRow,
		NULL,
		NULL,
		&bSwap
		);
	ASSERT( vRow.GetSize() > 0 );

int nMoveUpDownDirection = 0;
int nAdjustPixelsLT = 0, nAdjustPixelsRB = 0;
	if(		nDockBarID == AFX_IDW_DOCKBAR_RIGHT
		||	nDockBarID == AFX_IDW_DOCKBAR_BOTTOM
		)
		nAdjustPixelsRB = -2;
	else
		nAdjustPixelsLT = 2;
CRect rcUpDownAnalyze( rcAllRow );
	if( bHorzDockBar )
	{
		rcUpDownAnalyze.InflateRect( 0, 1 );
		if(		ptTrack.y <= (rcUpDownAnalyze.top + nAdjustPixelsLT)
			||	ptTrack.y >= (rcUpDownAnalyze.bottom + nAdjustPixelsRB)
			)
		{
			if( ptTrack.y <= (rcUpDownAnalyze.top + nAdjustPixelsLT) )
				nMoveUpDownDirection = -1;
			else
				nMoveUpDownDirection = 1;
		}
	} // if( bHorzDockBar )
	else
	{
		rcUpDownAnalyze.InflateRect( 1, 0 );
		if(		ptTrack.x <= (rcUpDownAnalyze.left + nAdjustPixelsLT)
			||	ptTrack.x >= (rcUpDownAnalyze.right + nAdjustPixelsRB)
			)
		{
			if( ptTrack.x <= (rcUpDownAnalyze.left + nAdjustPixelsLT) )
				nMoveUpDownDirection = -1;
			else
				nMoveUpDownDirection = 1;
		}
	} // else from if( bHorzDockBar )
	if( nMoveUpDownDirection != 0 )
	{
		if( vRow.GetSize() != 1 )
		{
			m_pExtBarSrc->_AffixmentSafeClearOuter();
			pOuterDocker->_NewRowDockControlBar(
				m_pExtBarSrc,
				m_pExtBarSrc,
				(nMoveUpDownDirection < 0) ? true : false
				);
			m_pExtBarSrc->_AffixmentRecalcOuter();

			m_pExtBarSrc->_AffixmentRecalcOuter();
			_DoFixed_StaticMinimalDockSiteRecalc(
				m_pExtBarSrc->m_pDockSite,
				pOuterDocker
				);
			return true;
		} // if( vRow.GetSize() != 1 )

		bool bSwapXtra = false;
		CRect rcAllRow2;
		MfcControlBarVector_t vRow2;
		m_pExtBarSrc->_GetNextRowBars(
			(nMoveUpDownDirection < 0) ? false : true,
			vRow2,
			&rcAllRow2,
			NULL,
			NULL,
			&bSwapXtra
			);
		if( bSwapXtra && (!bSwap) && vRow2.GetSize() > 0 )
		{
			CRect rcOuterDockerClient;
			pOuterDocker->GetClientRect( &rcOuterDockerClient );
			int nOuterDockerExtent = bHorzDockBar
				? rcOuterDockerClient.Width()
				: rcOuterDockerClient.Height()
				;
			CSize sizeDragMin = m_pExtBarSrc->_CalcDesiredMinOuterSize( bHorzDockBar );
			int nExtentDragMin = bHorzDockBar
				? sizeDragMin.cx
				: sizeDragMin.cy
				;
			int nExtentOtherOuterMin = 0;
			for( int nBar = 0; nBar < vRow2.GetSize(); nBar++ )
			{
				CControlBar * pOtherBar = vRow2[nBar];
				ASSERT_VALID( pOtherBar );
				ASSERT_KINDOF( CControlBar, pOtherBar );
				CExtControlBar * pExtBar =
					DYNAMIC_DOWNCAST( CExtControlBar, pOtherBar );
				CRect rcBarWnd;
				pOtherBar->GetWindowRect( &rcBarWnd );
				nExtentOtherOuterMin += bHorzDockBar
					? rcBarWnd.Width()
					: rcBarWnd.Height()
					;
				if(		pExtBar != NULL
					&&	pExtBar->IsFixedMode()
					&&	(! pExtBar->_GetFullRowMode() )
					)
				{
					CRect rcBarClient;
					pOtherBar->GetClientRect( &rcBarClient );
					CSize sizeOtherOuterMin = pExtBar->_CalcDesiredMinOuterSize( bHorzDockBar );
					nExtentOtherOuterMin += bHorzDockBar
						? (sizeOtherOuterMin.cx - rcBarClient.Width())
						: (sizeOtherOuterMin.cy - rcBarClient.Height())
						;
				}
			} // for( int nBar = 0; nBar < vRow2.GetSize(); nBar++ )
			if( (nExtentDragMin + nExtentOtherOuterMin) < nOuterDockerExtent )
				bSwapXtra = false;
		} // if( bSwapXtra && (!bSwap) && vRow2.GetSize() > 0 )
		
		if(		m_pExtBarSrc->_IsDockSiteCustomizeMode()
			&&	m_pExtBarSrc->IsSingleOnRow()
			)
			bSwap = true;

		if( bSwap || bSwapXtra )
		{
			if( vRow2.GetSize() > 0 )
			{
				CRect wrDockBar;
				pOuterDocker->GetWindowRect( &wrDockBar );
				CRect rcAnalyze( rcAllRow2 );
				if( bHorzDockBar )
				{
					rcAnalyze.left = wrDockBar.left;
					rcAnalyze.right = wrDockBar.right;
				} // if( bHorzDockBar )
				else
				{
					rcAnalyze.top = wrDockBar.top;
					rcAnalyze.bottom = wrDockBar.bottom;
				} // else from if( bHorzDockBar )
				if( nMoveUpDownDirection < 0 )
				{
					rcAnalyze.OffsetRect(
						bHorzDockBar ? 0 : (rcAllRow.right - rcAllRow2.right),
						bHorzDockBar ? (rcAllRow.bottom - rcAllRow2.bottom) : 0
						);
				} // if( nMoveUpDownDirection < 0 )
				else
				{
					rcAnalyze.OffsetRect(
						bHorzDockBar ? 0 : (rcAllRow2.right - rcAllRow.right),
						bHorzDockBar ? (rcAllRow2.bottom - rcAllRow.bottom) : 0
						);
				} // else from if( nMoveUpDownDirection < 0 )
				if( !rcAnalyze.PtInRect(ptTrack) )
				{
					_DoFixed_StaticOuterRowSwap(
						vRow2[0],
						vRow[0]
						);
					_DoFixed_StaticMinimalDockSiteRecalc(
						m_pExtBarSrc->m_pDockSite,
						pOuterDocker
						);
					return true;
				} // if( !rcAnalyze.PtInRect(ptTrack) )
			} // if( vRow2.GetSize() > 0 )
		} // if( bSwap || bSwapXtra )
		else
		{
			ASSERT( !bSwap );
			ASSERT( !bSwapXtra );
			if( vRow2.GetSize() != 0 )
			{
				// do redock to existing row
				CRect rcBar;
				m_pExtBarSrc->GetWindowRect( &rcBar );
				pOuterDocker->ScreenToClient( &rcBar );
				int nOffsetInRowActual = bHorzDockBar
						? rcBar.left
						: rcBar.top
						;
				CPoint ptShifted( ptTrack - sizeOffset );
				pOuterDocker->ScreenToClient( &ptShifted );
				int nOffsetInRowDesired = bHorzDockBar
						? ptShifted.x
						: ptShifted.y
						;
				int nShiftDesired = nOffsetInRowDesired - nOffsetInRowActual;
				
				//m_pExtBarSrc->_AffixmentSafeClearOuter();
				if( pOuterDocker->_ReDockToNewRow(
						m_pExtBarSrc,
						vRow2,
						nShiftDesired
						)
					)
				{ // if redock to new row is successful
					m_pExtBarSrc->_AffixmentRecalcOuter();
					_DoFixed_StaticMinimalDockSiteRecalc(
						m_pExtBarSrc->m_pDockSite,
						pOuterDocker
						);
					return true;
				} // if redock to new row is successful
				
				// if redock to new row is failed
				if( vRow2.GetSize() > 0 )
				{ // try swapping - 2nd attempt
					CRect wrDockBar;
					pOuterDocker->GetWindowRect( &wrDockBar );
					CRect rcAnalyze( rcAllRow2 );
					if( bHorzDockBar )
					{
						rcAnalyze.left = wrDockBar.left;
						rcAnalyze.right = wrDockBar.right;
					} // if( bHorzDockBar )
					else
					{
						rcAnalyze.top = wrDockBar.top;
						rcAnalyze.bottom = wrDockBar.bottom;
					} // else from if( bHorzDockBar )
					if( nMoveUpDownDirection < 0 )
					{
						rcAnalyze.OffsetRect(
							bHorzDockBar ? 0 : (rcAllRow.right - rcAllRow2.right),
							bHorzDockBar ? (rcAllRow.bottom - rcAllRow2.bottom) : 0
							);
					} // if( nMoveUpDownDirection < 0 )
					else
					{
						rcAnalyze.OffsetRect(
							bHorzDockBar ? 0 : (rcAllRow2.right - rcAllRow.right),
							bHorzDockBar ? (rcAllRow2.bottom - rcAllRow.bottom) : 0
							);
					} // else from if( nMoveUpDownDirection < 0 )
					if( !rcAnalyze.PtInRect(ptTrack) )
					{
m_pExtBarSrc->_AffixmentRecalcOuter();
						_DoFixed_StaticOuterRowSwap(
							vRow2[0],
							vRow[0]
							);
						_DoFixed_StaticMinimalDockSiteRecalc(
							m_pExtBarSrc->m_pDockSite,
							pOuterDocker
							);
						return true;
					} // if( !rcAnalyze.PtInRect(ptTrack) )
				} // try swapping - 2nd attempt
			} // if( vRow2.GetSize() != 0 )
		} // else from if( bSwap || bSwapXtra )
	} // if( nMoveUpDownDirection != 0 )
//	if( bSwap )
//		return true;

	// do slide
CRect rcBar;
	m_pExtBarSrc->GetWindowRect( &rcBar );
	pOuterDocker->ScreenToClient( &rcBar );
int nOffsetInRowActual = bHorzDockBar
		? rcBar.left
		: rcBar.top
		;
CPoint ptShifted( ptTrack - sizeOffset );
	pOuterDocker->ScreenToClient( &ptShifted );
int nOffsetInRowDesired = bHorzDockBar
		? ptShifted.x
		: ptShifted.y
		;
int nShiftDesired = nOffsetInRowDesired - nOffsetInRowActual;

BOOL bLayoutQueryOld = pOuterDocker->m_bLayoutQuery;
CRect rectLayoutOld( pOuterDocker->m_rectLayout );
	pOuterDocker->m_bLayoutQuery = FALSE;
	pOuterDocker->GetWindowRect( &pOuterDocker->m_rectLayout );
	pOuterDocker->m_rectLayout.OffsetRect(
		- pOuterDocker->m_rectLayout.TopLeft()
		);
bool bInplaceResizing = false;
//	if( m_pExtBarSrc->IsFixedMode() ) // (+ v.2.22)
	if( m_pExtBarSrc->IsFixedDockStyle() ) // (+ v.2.22)
	{
		InternalAffixmentData * pAffixmentData =
			m_pExtBarSrc->_AffixmentGetOuter();
		if( pAffixmentData != NULL )
		{
			if(		(! pAffixmentData->IsEmpty() )
				&&	(! m_pExtBarSrc->_GetFullRowMode() )
				)
			{
				bool bInitialRecalcAffixment = false;
				if( (pAffixmentData->m_dwAffixmentFlags & InternalAffixmentData::__EAFF_FIXED_MODE_INITIAL_RECALC) != 0 )
					bInitialRecalcAffixment = true;
				else
				{
					CRect rcOuterDocker;
					pOuterDocker->GetClientRect( &rcOuterDocker );
					ASSERT( rcOuterDocker.left == 0 && rcOuterDocker.top == 0 );
					if(		(bHorzDockBar && pAffixmentData->m_rcAffixment.Width() >= rcOuterDocker.right)
						||	(bHorzDockBar && pAffixmentData->m_rcAffixment.Height() >= rcOuterDocker.bottom)
						)
						bInitialRecalcAffixment = true;
				} // else from if( (pAffixmentData->m_dwAffixmentFlags & InternalAffixmentData::__EAFF_FIXED_MODE_INITIAL_RECALC) != 0 )
				if( bInitialRecalcAffixment )
				{
					pAffixmentData->Empty(); // initial recalc
					pAffixmentData->m_dwAffixmentFlags &=
						~(InternalAffixmentData::__EAFF_FIXED_MODE_INITIAL_RECALC);
				} // bInitialRecalcAffixment
			}
			if( pAffixmentData->IsEmpty() )
				pOuterDocker->_AffixmentBringToTop( m_pExtBarSrc );
		} // if( pAffixmentData != NULL )
	} // if( m_pExtBarSrc->IsFixedMode() ) // (+ v.2.22)
	pOuterDocker->_RedockInRow(
		m_pExtBarSrc,
		nShiftDesired,
		&bInplaceResizing
		);
	pOuterDocker->m_bLayoutQuery = bLayoutQueryOld;
	pOuterDocker->m_rectLayout = rectLayoutOld;

	if( bInplaceResizing )
	{
		g_DragStateOld.SetCFR(
			bHorzDockBar ? __ECFR_HORZ : __ECFR_VERT
			);
	} // if( bInplaceResizing )

	CExtPaintManager::stat_PassPaintMessages();
	return true;
}

void CExtControlBar::InternalDraggingState_t::_DoFixed_StaticMinimalDockSiteRecalc(
	CFrameWnd * pDockSite,
	CExtDockOuterBar * pOuterDocker
	)
{
	ASSERT_VALID( pDockSite ); 
	ASSERT_VALID( pOuterDocker ); 
	ASSERT( pOuterDocker->GetParent() == pDockSite );

	pOuterDocker->Invalidate();

	if( (pOuterDocker->GetStyle() & WS_VISIBLE) == 0 )
	{
		pDockSite->RecalcLayout();
		return;
	}

UINT nDockBarID = pOuterDocker->GetDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );
BOOL bHorzDockBar = 
		(	nDockBarID == AFX_IDW_DOCKBAR_TOP
		||	nDockBarID == AFX_IDW_DOCKBAR_BOTTOM )
		? TRUE : FALSE;

BOOL bLayoutQueryOld = pOuterDocker->m_bLayoutQuery;
CRect rectLayoutOld( pOuterDocker->m_rectLayout );

CRect rcDockerWnd;
	pOuterDocker->GetWindowRect( &rcDockerWnd );
CSize sizeDockBarBefore = rcDockerWnd.Size();

	pOuterDocker->m_rectLayout = rcDockerWnd;
	pOuterDocker->m_rectLayout.OffsetRect(
		- pOuterDocker->m_rectLayout.TopLeft()
		);
	pOuterDocker->m_bLayoutQuery = TRUE;
CSize sizeDockBarAfter =
		pOuterDocker->CalcFixedLayout( FALSE, bHorzDockBar );

bool bSizesAreEqual = false;
	if( bHorzDockBar )
	{
		if( sizeDockBarAfter.cy == sizeDockBarBefore.cy )
			bSizesAreEqual = true;
	} // if( bHorzDockBar )
	else
	{
		if( sizeDockBarAfter.cx == sizeDockBarBefore.cx )
			bSizesAreEqual = true;
	} // else from if( bHorzDockBar )

	if( bSizesAreEqual )
	{
		pOuterDocker->m_bLayoutQuery = FALSE;
		pOuterDocker->CalcFixedLayout( FALSE, bHorzDockBar );
	}

	pOuterDocker->m_bLayoutQuery = bLayoutQueryOld;
	pOuterDocker->m_rectLayout = rectLayoutOld;

	if( !bSizesAreEqual )
		pDockSite->RecalcLayout();

	CExtPaintManager::stat_PassPaintMessages();
}

void CExtControlBar::InternalDraggingState_t::_DoFixed_StaticOuterRowSwap(
	CControlBar * pBar1,
	CControlBar * pBar2
	)
{
	ASSERT_VALID( pBar1 );
	ASSERT_VALID( pBar2 );
	ASSERT_VALID( pBar1->m_pDockBar );
	ASSERT_VALID( pBar2->m_pDockBar );
	ASSERT_VALID( pBar1->m_pDockSite );
	ASSERT_VALID( pBar2->m_pDockSite );
	ASSERT( pBar1->m_pDockBar == pBar2->m_pDockBar );
	ASSERT( pBar1->m_pDockSite == pBar2->m_pDockSite );
	ASSERT( pBar1 != pBar2 );
CDockBar * pDockBar = pBar1->m_pDockBar;
int nPos1 = pDockBar->FindBar( pBar1 );
int	nPos2 = pDockBar->FindBar( pBar2 );
	ASSERT( nPos1 != nPos2 );
bool bWalkUp = (nPos1 < nPos2) ? true : false;

	pDockBar->Invalidate();
	pBar1->Invalidate();
	pBar2->Invalidate();

	ASSERT( pDockBar->m_arrBars[0] == NULL );

MfcControlBarVector_t vPush;
int nPosRemovedStart = nPos1, nPosRemovedEnd = nPos1;
int nPos = 0;
	for( nPos = nPos1; nPos > 0; nPos-- )
	{
		CControlBar * pBar = (CControlBar *)pDockBar->m_arrBars[nPos];
		if( pBar == NULL )
			break;
		vPush.InsertAt( 0, pBar );
		nPosRemovedStart = nPos;
	}
int nCount = (int)pDockBar->m_arrBars.GetSize();
	for( nPos = nPos1+1; nPos < nCount; nPos++ )
	{
		CControlBar * pBar = (CControlBar *)pDockBar->m_arrBars[nPos];
		if( pBar == NULL )
			break;
		vPush.Add( pBar );
		nPosRemovedEnd = nPos;
	}
	if( nPosRemovedEnd < nCount-1 )
	{
		CControlBar * pBar = (CControlBar *)pDockBar->m_arrBars[nPosRemovedEnd+1];
		if( pBar == NULL )
			nPosRemovedEnd++;
	}
int nCountToRemove = nPosRemovedEnd - nPosRemovedStart + 1;
	pDockBar->m_arrBars.RemoveAt( nPosRemovedStart, nCountToRemove );
	nPos2 = pDockBar->FindBar( pBar2 );
	nCount = (int)pDockBar->m_arrBars.GetSize();
	if( bWalkUp )
	{
		for( ; nPos2 < nCount; nPos2++ )
		{
			CControlBar * pBar = (CControlBar *)pDockBar->m_arrBars[nPos2];
			if( pBar == NULL )
			{
				nPos2++;
				break;
			}
			if( nPos2 == nCount-1 )
			{
				pDockBar->m_arrBars.Add( (CControlBar *)NULL );
				nPos2++;
				break;
			}
		}
	} // if( bWalkUp )
	else
	{
		ASSERT( nPos2 > 0 );
		for( ; nPos2 > 0; nPos2-- )
		{
			CControlBar * pBar = (CControlBar *)pDockBar->m_arrBars[nPos2];
			if( pBar == NULL )
			{
				nPos2++;
				break;
			}
		}
		if( nPos2 == 0 )
			nPos2++;
	} // else from if( bWalkUp )
	nCount = (int)vPush.GetSize();
	for( nPos = 0; nPos < nCount; nPos++, nPos2++ )
		pDockBar->m_arrBars.InsertAt( nPos2, vPush[nPos] );
	pDockBar->m_arrBars.InsertAt( nPos2, (CControlBar *)NULL );
	ASSERT_VALID( pDockBar );
}

void CExtControlBar::InternalDraggingState_t::_DoFixed_BasicAdjustment(
	CPoint ptTrack, // in screen coords
	bool bForceFloatMode
	)
{
	ASSERT_VALID( this );
	ASSERT( !IsEmpty() );
	ASSERT_VALID( m_pExtBarSrc );
	ASSERT( m_pExtBarSrc->IsFixedDockStyle() );
	
	ASSERT( m_pExtBarSrc->_IsShowContentWhenDragging() );
	
	TrackRectSetEmpty();

BOOL bForceFloat =
		(	bForceFloatMode ||
			CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL)
		) ? TRUE : FALSE;
	if( bForceFloat )
	{
		_DoFixed_FloatingAdjustment( ptTrack );
		return;
	}

CFrameWnd * pFrame = m_pExtBarSrc->_GetDockingFrameImpl();
	ASSERT_VALID( pFrame );

CRect wrFrame;
	pFrame->GetWindowRect( &wrFrame );
	wrFrame.InflateRect( afxData.cxBorder2, afxData.cyBorder2 );
	if( ! wrFrame.PtInRect( ptTrack ) )
	{
		_DoFixed_FloatingAdjustment( ptTrack );
		return;
	}

struct
{
	UINT m_nDockBarID;
	bool m_bForceFirstReview:1, m_bVisible:1, m_bForceExclude:1;
	RECT m_rcDockBarAnalyze;
	CExtDockOuterBar * m_pOuterDocker;
} arrDockBarAnalyze[4] =
{
	{ AFX_IDW_DOCKBAR_TOP,		false, false, false, {0,0,0,0}, NULL },
	{ AFX_IDW_DOCKBAR_BOTTOM,	false, false, false, {0,0,0,0}, NULL },
	{ AFX_IDW_DOCKBAR_LEFT,		false, false, false, {0,0,0,0}, NULL },
	{ AFX_IDW_DOCKBAR_RIGHT,	false, false, false, {0,0,0,0}, NULL },
};
UINT nOwnDockBarID = m_pExtBarSrc->GetSafeDockBarDlgCtrlID();
int nIdxForceFirstReview = -1;
int nDockBarIdx = 0;
	for( nDockBarIdx = 0; nDockBarIdx < 4; nDockBarIdx++ )
	{
		if( nOwnDockBarID == arrDockBarAnalyze[nDockBarIdx].m_nDockBarID )
		{
			ASSERT( nIdxForceFirstReview < 0 );
			nIdxForceFirstReview = nDockBarIdx;
			arrDockBarAnalyze[nDockBarIdx].m_bForceFirstReview = true;
		}
		arrDockBarAnalyze[nDockBarIdx].m_pOuterDocker =
			STATIC_DOWNCAST(
				CExtDockOuterBar,
				pFrame->GetControlBar(
					arrDockBarAnalyze[nDockBarIdx].m_nDockBarID
					)
				);
		ASSERT_VALID( arrDockBarAnalyze[nDockBarIdx].m_pOuterDocker );
//		const int nRdpExtraInnerDx = 6; // (- v.2.24)
//		const int nRdpExtraInnerDy = 6; // (- v.2.24)
//		const int nRdpExtraOuterDx = nRdpExtraInnerDx*2; // (- v.2.24)
//		const int nRdpExtraOuterDy = nRdpExtraInnerDy*2; // (- v.2.24)
		const int nRdpExtraInnerDx = 12; // (+ v.2.24)
		const int nRdpExtraInnerDy = 12; // (+ v.2.24)
		const int nRdpExtraOuterDx = ::MulDiv( nRdpExtraInnerDx, 3, 2 ); // (+ v.2.24)
		const int nRdpExtraOuterDy = ::MulDiv( nRdpExtraInnerDy, 3, 2 ); // (+ v.2.24)

		arrDockBarAnalyze[nDockBarIdx].m_bVisible =
			(arrDockBarAnalyze[nDockBarIdx].m_pOuterDocker->GetStyle() & WS_VISIBLE)
				? true : false;
		if( arrDockBarAnalyze[nDockBarIdx].m_bVisible )
		{
			arrDockBarAnalyze[nDockBarIdx].m_pOuterDocker->GetWindowRect(
				&( arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze )
				);
		} // if( arrDockBarAnalyze[nDockBarIdx].m_bVisible )
		else
		{
			arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze = wrFrame;
			switch( arrDockBarAnalyze[nDockBarIdx].m_nDockBarID )
			{
			case AFX_IDW_DOCKBAR_TOP:
				arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.bottom =
					arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.top;
			break;
			case AFX_IDW_DOCKBAR_BOTTOM:
				arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.top =
					arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.bottom;
			break;
			case AFX_IDW_DOCKBAR_LEFT:
				arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.right =
					arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.left;
			break;
			case AFX_IDW_DOCKBAR_RIGHT:
				arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.left =
					arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.right;
			break;
#ifdef _DEBUG
			default:
				{
					ASSERT( FALSE );
				}
			break;
#endif // _DEBUG
			} // switch( arrDockBarAnalyze[nDockBarIdx].m_nDockBarID )
		} // else from if( arrDockBarAnalyze[nDockBarIdx].m_bVisible )

		switch( arrDockBarAnalyze[nDockBarIdx].m_nDockBarID )
		{
		case AFX_IDW_DOCKBAR_TOP:
			arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.bottom +=
				nRdpExtraInnerDy;
			arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.top =
				wrFrame.top;
		break;
		case AFX_IDW_DOCKBAR_BOTTOM:
			arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.top -=
				nRdpExtraInnerDy;
			arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.bottom =
				wrFrame.bottom;
		break;
		case AFX_IDW_DOCKBAR_LEFT:
			arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.right +=
				nRdpExtraInnerDx;
			arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.left =
				wrFrame.left;
		break;
		case AFX_IDW_DOCKBAR_RIGHT:
			arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.left -=
				nRdpExtraInnerDx;
			arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.right =
				wrFrame.right;
		break;
#ifdef _DEBUG
		default:
			{
				ASSERT( FALSE );
			}
		break;
#endif // _DEBUG
		} // switch( arrDockBarAnalyze[nDockBarIdx].m_nDockBarID )

		if( arrDockBarAnalyze[nDockBarIdx].m_nDockBarID ==
				AFX_IDW_DOCKBAR_LEFT
			||
			arrDockBarAnalyze[nDockBarIdx].m_nDockBarID ==
				AFX_IDW_DOCKBAR_RIGHT
			)
		{
			arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.top =
				arrDockBarAnalyze[0].m_rcDockBarAnalyze.bottom
				- nRdpExtraInnerDy;
			arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze.bottom =
				arrDockBarAnalyze[1].m_rcDockBarAnalyze.top
				+ nRdpExtraInnerDy;
		}

		CRect rcReDockAnalyze(
			arrDockBarAnalyze[nDockBarIdx].m_rcDockBarAnalyze
			);
		rcReDockAnalyze.InflateRect(
			nRdpExtraOuterDx,
			nRdpExtraOuterDy
			);
		if( !rcReDockAnalyze.PtInRect( ptTrack ) )
			arrDockBarAnalyze[nDockBarIdx].m_bForceExclude = true;
	} // for( nDockBarIdx = 0; nDockBarIdx < 4; nDockBarIdx++ )

	if( nIdxForceFirstReview >= 0 )
	{
		if( arrDockBarAnalyze[nIdxForceFirstReview].m_bForceExclude )
			arrDockBarAnalyze[nIdxForceFirstReview].m_bForceFirstReview = false;
		else
		{
			if( _DoFixed_OuterDockerAdjustment(
					ptTrack,
					arrDockBarAnalyze[nIdxForceFirstReview].m_pOuterDocker
					)
				)
				return;
		} // else from if( arrDockBarAnalyze[nDockBarIdx].m_bForceExclude )
	} // if( nIdxForceFirstReview >= 0 )

	for( nDockBarIdx = 0; nDockBarIdx < 4; nDockBarIdx++ )
	{
		ASSERT_VALID( arrDockBarAnalyze[nDockBarIdx].m_pOuterDocker );
		if(		arrDockBarAnalyze[nDockBarIdx].m_bForceFirstReview
			||	(! arrDockBarAnalyze[nDockBarIdx].m_bVisible )
			||	arrDockBarAnalyze[nDockBarIdx].m_bForceExclude
			)
			continue;
		
		if( _DoFixed_OuterDockerAdjustment(
				ptTrack,
				arrDockBarAnalyze[nDockBarIdx].m_pOuterDocker
				)
			)
			return;
	} // for( nDockBarIdx = 0; nDockBarIdx < 4; nDockBarIdx++ )

	for( nDockBarIdx = 0; nDockBarIdx < 4; nDockBarIdx++ )
	{
		if(		arrDockBarAnalyze[nDockBarIdx].m_bForceFirstReview
			||	arrDockBarAnalyze[nDockBarIdx].m_bVisible
			||	arrDockBarAnalyze[nDockBarIdx].m_bForceExclude
			)
			continue;
		if( _DoFixed_OuterDockerAdjustment(
				ptTrack,
				arrDockBarAnalyze[nDockBarIdx].m_pOuterDocker
				)
			)
			return;
	} // for( nDockBarIdx = 0; nDockBarIdx < 4; nDockBarIdx++ )

	_DoFixed_FloatingAdjustment( ptTrack );
}

void CExtControlBar::InternalDraggingState_t::CalcStateDynamic(
	CPoint ptTrack, // in screen coords
	bool bForceFloatMode
	)
{
	ASSERT_VALID( this );
	ASSERT( !IsEmpty() );
	ASSERT_VALID( m_pExtBarSrc );

	if( m_pExtBarSrc->IsFixedDockStyle() )
	{
		ASSERT( m_pExtBarSrc->_IsShowContentWhenDragging() );
		_DoFixed_BasicAdjustment( ptTrack, bForceFloatMode );
		return;
	}

CExtControlBar::eResizablePanelDockingType_t eResizablePanelDockingType =
		m_pExtBarSrc->OnQueryDockingType();

bool bShowContent = m_pExtBarSrc->_IsShowContentWhenDragging();
	if(		bShowContent
		&&	m_pExtBarSrc->IsDocked()
#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
		&&	INT(eResizablePanelDockingType) < INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005)
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)
		)
	{
		ASSERT_VALID( m_pExtBarSrc->m_pDockBar );
		CRect rcDockBar;
		m_pExtBarSrc->m_pDockBar->GetWindowRect( &rcDockBar );
		if( rcDockBar.PtInRect(ptTrack) )
			return;
	} // if( bShowContent ...
	
	TrackRectSetEmpty();

	eResizablePanelDockingType;
static const DWORD _dwDockBarMap[4][2] =
{
	{ AFX_IDW_DOCKBAR_TOP,      CBRS_TOP    },
	{ AFX_IDW_DOCKBAR_BOTTOM,   CBRS_BOTTOM },
	{ AFX_IDW_DOCKBAR_LEFT,     CBRS_LEFT   },
	{ AFX_IDW_DOCKBAR_RIGHT,    CBRS_RIGHT  },
};

// reasonable dockbar parts
static const INT nRdpOuter = 16;
static const INT nRdpInner = nRdpOuter*2;
static const INT nRdpMostInner = 12;
// reasonable controlbar part
static const INT nRcpAnyHalfMin = 8;

	m_eCDT = __ECDT_FLOATED;
	m_pExtBarDst = NULL;
	//ExtBarDstSet( NULL );

CRect rcBarWnd;

INT nSrcMinHW = m_pExtBarSrc->_CalcDesiredMinHW();
INT nSrcMinVH = m_pExtBarSrc->_CalcDesiredMinVH();
	if( nSrcMinHW < nRcpAnyHalfMin )
		nSrcMinHW = nRcpAnyHalfMin;
	if( nSrcMinVH < nRcpAnyHalfMin )
		nSrcMinVH = nRcpAnyHalfMin;
INT nDstMinHW = nRcpAnyHalfMin; // some reasonable min width when horz docked
INT nDstMinVH = nRcpAnyHalfMin; // some reasonable min height when vert docked

CFrameWnd * pFrame = m_pExtBarSrc->_GetDockingFrameImpl();
	ASSERT_VALID( pFrame );
CRect rcFrameWnd;
	pFrame->GetWindowRect( &rcFrameWnd );

bool bForceFloat = bForceFloatMode;

#if (!defined __EXT_MFC_NO_DOCK_MARKERS)

	if(		INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005)
		&&	(! bForceFloat )
		&&	m_pExtBarSrc->IsFloating()
		)
	{
		ASSERT( m_eCDT == __ECDT_FLOATED );
		HWND hWnd = ::WindowFromPoint( ptTrack );
		if( hWnd != NULL )
		{
			ASSERT( ::IsWindow(hWnd) );
			CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
			if( pWnd != NULL )
			{
				CExtDynDockMarkerWnd * pDynDockMarkerWnd =
					DYNAMIC_DOWNCAST( CExtDynDockMarkerWnd, pWnd );
				if(		pDynDockMarkerWnd != NULL
					&&	pDynDockMarkerWnd->IsStateApplyable()
					)
				{ // if dockmarker is state applyable
					m_eCDT = pDynDockMarkerWnd->GetCDT();
					HWND hWnd = pDynDockMarkerWnd->GetMarkerTargetHWND();
					if( hWnd != NULL )
					{
						if( ::IsWindow(hWnd) )
						{
							CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
							if( pWnd != NULL )
							{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
								if( m_eCDT == __ECDT_2005_BAR_NEW_TAB )
								{
									CExtDynTabControlBar * pTabbedBar =
										DYNAMIC_DOWNCAST( CExtDynTabControlBar, pWnd );
									if( pTabbedBar != NULL )
									{
										if( m_pExtBarSrc->_CanDockToTabbedContainers( pTabbedBar ) )
										{
											CRect rcTabSwitcher;
											pTabbedBar->GetSwitcherWindowRect( rcTabSwitcher );
											CPoint ptTrackFake(
												rcTabSwitcher.left + 2, // rcTabSwitcher.right - 2,
												rcTabSwitcher.CenterPoint().y
												);
											ExtBarDstSet( pTabbedBar );
											if( pTabbedBar->IsSwitcherVisible() )
											{
												m_eCDT = __ECDT_TABBED_REINDEX_NHM;
												ReindexPosCalc( ptTrackFake );
											} // if( pTabbedBar->IsSwitcherVisible() )
											else
											{
												m_eCDT = __ECDT_2005_BAR_NEW_TAB;
												TrackRectSet( &rcBarWnd );
											} // else from if( pTabbedBar->IsSwitcherVisible() )
										} // if( m_pExtBarSrc->_CanDockToTabbedContainers( pTabbedBar ) )
										else
										{
											m_eCDT = __ECDT_FLOATED;
											bForceFloatMode = true;
										}
									} // if( pTabbedBar != NULL )
									else
									{
										CExtControlBar * pDestBar =
											DYNAMIC_DOWNCAST( CExtControlBar, pWnd );
										if(		pDestBar == NULL
											||	(! m_pExtBarSrc->_CanDockToTabbedContainers( pDestBar ) )
											||	(! pDestBar->_CanDockToTabbedContainers( m_pExtBarSrc ) )
											)
										{
											m_eCDT = __ECDT_FLOATED;
											bForceFloatMode = true;
										}
										else
											ExtBarDstSet( pDestBar );
									} // else from if( pTabbedBar != NULL )
								} // if( m_eCDT == __ECDT_2005_BAR_NEW_TAB )
								else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
									if(		m_eCDT == __ECDT_2005_BAR_TOP
										||	m_eCDT == __ECDT_2005_BAR_BOTTOM
										||	m_eCDT == __ECDT_2005_BAR_LEFT
										||	m_eCDT == __ECDT_2005_BAR_RIGHT
										)
								{
									CExtControlBar * pDestBar =
										DYNAMIC_DOWNCAST( CExtControlBar, pWnd );
									if(		pDestBar == NULL
										//|| (! m_pExtBarSrc->_CanDockToTabbedContainers( pDestBar ) )
										||	(! m_pExtBarSrc->_CanDockLTRB( pDestBar ) )
										||	(! pDestBar->_CanDockLTRB( m_pExtBarSrc ) )
										)
									{
										m_eCDT = __ECDT_FLOATED;
										bForceFloatMode = true;
									}
									else
										ExtBarDstSet( pDestBar );
								}
							} // if( pWnd != NULL )
						} // if( ::IsWindow(hWnd) )
						else
						{
							m_eCDT = __ECDT_FLOATED;
							bForceFloatMode = true;
						} // else from if( ::IsWindow(hWnd) )
					} // if( hWnd != NULL )
					if( m_eCDT != __ECDT_FLOATED )
						return;
				} // if dockmarker is state applyable
			} // if( pWnd != NULL )
		} // if( hWnd != NULL )
	}

#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)

	if(		
#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
			INT(eResizablePanelDockingType) < INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005)
		&&	
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)
			( ! bForceFloat )
		&&	m_pExtBarSrc->IsFloating()
		&&	(	m_pExtBarSrc->IsKindOf(RUNTIME_CLASS(CExtDynControlBar))
				|| (! bShowContent )
			)
		)
	{
		CFrameWnd * pFrameExcludeTest = m_pExtBarSrc->GetParentFrame();
		if( pFrameExcludeTest->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
		{
			CRect rcFrameExcludeText;
			pFrameExcludeTest->GetWindowRect( &rcFrameExcludeText );
			if( rcFrameExcludeText.PtInRect(ptTrack) )
				bForceFloat = true;
		}
	}


	if(		(! CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) )
		&&	(! bForceFloat )
		)
	{
		ASSERT( m_eCDT == __ECDT_FLOATED );

		bool bCanDockToInnerCircles =
			m_pExtBarSrc->_CanDockToInnerCircles();

		if(		bCanDockToInnerCircles
			//&&  (! bShowContent )
			)
		{ // try to analyze docking into floating bars
			ExtControlBarVector_t vBars;
			m_pExtBarSrc->_GetFrameControlBars(
#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
				( INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005) )
					? true : 
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)
					false,
				vBars,
				(DWORD)(
					__GFECB_NONFIXED_NONMIN //|__GFECB_FIXED
						|__GFECB_FLOATED
						|__GFECB_VIS_ALL
						|__GFECB_BYPT_ONLY_WND
					),
				ptTrack
				);

			CExtControlBar * pDesiredBar = NULL;
			INT nCountOfBars = (INT)vBars.GetSize();
			for( INT nBarIdx = 0; nBarIdx < nCountOfBars; nBarIdx++ )
			{
				CExtControlBar * pTestBar = vBars[nBarIdx];
				ASSERT_VALID( pTestBar );
				ASSERT( !pTestBar->IsFixedMode() );
				if( pTestBar == m_pExtBarSrc )
					continue;
				pDesiredBar = pTestBar;
				break;
			} // for( INT nBarIdx = 0; nBarIdx < nCountOfBars; nBarIdx++ )

			if( pDesiredBar != NULL )
			{
				ASSERT_VALID( pDesiredBar );
				//m_pExtBarDst = pDesiredBar;
				ExtBarDstSet( pDesiredBar );

				m_pExtBarDst->GetParentFrame()->GetWindowRect( &rcBarWnd );

				nDstMinHW = m_pExtBarDst->_CalcDesiredMinHW();
				nDstMinVH = m_pExtBarDst->_CalcDesiredMinVH();

				INT nWidth3   = nRdpInner; // rcBarWnd.Width();
				INT nHeight3  = nRdpInner; // rcBarWnd.Height();
//				INT nWidth2  = nWidth  / 2;
//				INT nHeight2 = nHeight / 2;
//				INT nWidth3  = nWidth  / 4;
//				INT nHeight3 = nHeight / 4;

//				UINT nDockBarID =
//					m_pExtBarDst->GetSafeDockBarDlgCtrlID();
//				ASSERT_DOCKBAR_DLGCTRLID( nDockBarID );
//				bool bHorz = 
//					(	nDockBarID == AFX_IDW_DOCKBAR_TOP
//					||	nDockBarID == AFX_IDW_DOCKBAR_BOTTOM
//					||	nDockBarID == AFX_IDW_DOCKBAR_FLOAT
//					) ? true : false;
				ASSERT( m_pExtBarDst->m_pDockBar != NULL );
				CDockBar * pDockBar =
					STATIC_DOWNCAST( CDockBar, m_pExtBarDst->m_pDockBar );
				ASSERT_VALID( pDockBar );

				bool bEnableInnerDock = true;
//				if( nDockBarID == AFX_IDW_DOCKBAR_LEFT
//					&& (m_pExtBarSrc->m_dwDockStyle&CBRS_ALIGN_LEFT) == 0
//					)
//					bEnableInnerDock = false;
//				if( nDockBarID == AFX_IDW_DOCKBAR_RIGHT
//					&& (m_pExtBarSrc->m_dwDockStyle&CBRS_ALIGN_RIGHT) == 0
//					)
//					bEnableInnerDock = false;
//				if( nDockBarID == AFX_IDW_DOCKBAR_TOP
//					&& (m_pExtBarSrc->m_dwDockStyle&CBRS_ALIGN_TOP) == 0
//					)
//					bEnableInnerDock = false;
//				if( nDockBarID == AFX_IDW_DOCKBAR_BOTTOM
//					&& (m_pExtBarSrc->m_dwDockStyle&CBRS_ALIGN_BOTTOM) == 0
//					)
//					bEnableInnerDock = false;
				
				if( bEnableInnerDock && (m_pExtBarSrc->m_dwDockStyle&CBRS_ALIGN_ANY) != 0 )
				{
					m_bDeepHalfSplit = false;

					if( !m_pExtBarDst->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar))
						|| m_pExtBarDst->IsFloating()
						)
						m_bDeepHalfSplit = true;
					
					CRect rcWndDockBar;
					pDockBar->GetWindowRect( &rcWndDockBar );
					
					CRect rcPart( rcBarWnd );
					do
					{ // SINGLE LOOP

						INT nHalfHeight = rcBarWnd.Height()/2;
						if(		rcBarWnd.Width() >= nSrcMinHW
							&&	nHalfHeight >= nSrcMinVH
							&&	nHalfHeight >= nDstMinVH
							)
						{ // if can do half splitting

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
							BOOL bTabbedDest =
								m_pExtBarDst->IsKindOf(RUNTIME_CLASS(CExtDynTabControlBar))
								&& ( ((CExtDynTabControlBar*)m_pExtBarDst)->IsSwitcherVisible() )
								;
							INT nTabbedShift = 0;
							bool bCanDockToTabbedContainers =
								(	m_pExtBarSrc->_CanDockToTabbedContainers( m_pExtBarDst )
									&& m_pExtBarDst->_CanDockToTabbedContainers( m_pExtBarSrc )
									) ? true : false;
							if( bCanDockToTabbedContainers )
							{
								rcPart = rcBarWnd;
								rcPart.bottom = rcPart.top + nHeight3;
								if(	rcPart.PtInRect(ptTrack) )
								{
									if( bTabbedDest )
										m_eCDT = __ECDT_TABBED_UPDATE;
									else
										m_eCDT = __ECDT_TABBED_NEW;
									rcPart = rcBarWnd;
									break;
								}
								nTabbedShift = nHeight3;
								rcPart.OffsetRect( 0, nTabbedShift );
							}
							else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
							{
								rcPart = rcBarWnd;
								rcPart.bottom = rcPart.top + nHeight3;
							}
							
							bool bCanDockLTRB =
								(	m_pExtBarSrc->_CanDockLTRB( m_pExtBarDst )
									&& m_pExtBarDst->_CanDockLTRB( m_pExtBarSrc )
									) ? true : false;
							if( ! bCanDockLTRB )
							{
								m_eCDT = __ECDT_FLOATED;
								bForceFloatMode = true;
								break;
							} // if( bCanDockLTRB )
							
							if( rcPart.PtInRect(ptTrack) )
							{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
								rcPart.OffsetRect( 0, -nTabbedShift );
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
								m_bDeepHalfSplit = true;
								m_eCDT = __ECDT_INNER_TOP;
								rcPart.bottom = rcPart.top + rcBarWnd.Height()/2; // + nHeight2;
								rcPart.left = min( rcPart.left, rcWndDockBar.left );
								rcPart.right = max( rcPart.right, rcWndDockBar.right );
								break;
							} // if( rcPart.PtInRect(ptTrack) )

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
							rcPart = rcBarWnd;
							INT nBottomShift = 0;
							if(		bCanDockToTabbedContainers
								//&& (!bShowContent)
								&&	bTabbedDest
								)
							{
								CExtDynTabControlBar * pTabbedBar =
									(CExtDynTabControlBar *)m_pExtBarDst;
								CRect rcTabSwitcher;
								pTabbedBar->GetSwitcherWindowRect( rcTabSwitcher );
								if( rcTabSwitcher.PtInRect(ptTrack) )
								{
									m_eCDT = __ECDT_TABBED_REINDEX;
									ReindexPosCalc( ptTrack );
									break;
								}
								nBottomShift = rcTabSwitcher.Size().cy;
							}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

							rcPart = rcBarWnd;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
							rcPart.bottom -= nBottomShift;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
							rcPart.top = rcPart.bottom - nHeight3;
							if( rcPart.PtInRect(ptTrack) )
							{
								m_bDeepHalfSplit = true;
								m_eCDT = __ECDT_INNER_BOTTOM;
								rcPart.top = rcPart.bottom - rcBarWnd.Height()/2; // - nHeight2;
								rcPart.left = min( rcPart.left, rcWndDockBar.left );
								rcPart.right = max( rcPart.right, rcWndDockBar.right );
								break;
							} // if( rcPart.PtInRect(ptTrack) )
						} // if can do half splitting

						
						INT nHalfWidth = rcBarWnd.Width()/2;
						if( rcBarWnd.Height() >= nSrcMinVH
							&& nHalfWidth >= nSrcMinHW
							&& nHalfWidth >= nDstMinHW
							)
						{ // if can do half splitting
							rcPart = rcBarWnd;
							rcPart.right = rcPart.left + nWidth3;
							if( rcPart.PtInRect(ptTrack) )
							{
								m_bDeepHalfSplit = true;
								m_eCDT = __ECDT_INNER_LEFT;
								rcPart.right = rcPart.left + rcBarWnd.Width()/2; // + nWidth2;
								rcPart.top = min( rcPart.top, rcWndDockBar.top );
								rcPart.bottom = max( rcPart.bottom, rcWndDockBar.bottom );
								break;
							} // if( rcPart.PtInRect(ptTrack) )

							rcPart = rcBarWnd;
							rcPart.left = rcPart.right - nWidth3;
							if( rcPart.PtInRect(ptTrack) )
							{
								m_bDeepHalfSplit = true;
								m_eCDT = __ECDT_INNER_RIGHT;
								rcPart.left = rcPart.right - rcBarWnd.Width()/2; // - nWidth2;
								rcPart.top = min( rcPart.top, rcWndDockBar.top );
								rcPart.bottom = max( rcPart.bottom, rcWndDockBar.bottom );
								break;
							} // if( rcPart.PtInRect(ptTrack) )
						} // if can do half splitting

					} // SINGLE LOOP
					while( false );

//					if( m_bDeepHalfSplit )
//					{
//						if( m_pExtBarDst->IsSingleOnRow() )
//							m_bDeepHalfSplit = false;
//					} // if( m_bDeepHalfSplit )

					if( m_eCDT != __ECDT_FLOATED )
					{

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
						if(	m_bDeepHalfSplit
							&& (		m_eCDT == __ECDT_TABBED_NEW
									||	m_eCDT == __ECDT_TABBED_UPDATE
									||	m_eCDT == __ECDT_TABBED_REINDEX
									||	m_eCDT == __ECDT_TABBED_REINDEX_NHM
									)
							)
							m_bDeepHalfSplit = false;

						ASSERT(
								(		m_bDeepHalfSplit
									&&	m_eCDT != __ECDT_TABBED_NEW
									&&	m_eCDT != __ECDT_TABBED_UPDATE
									&&	m_eCDT != __ECDT_TABBED_REINDEX
									&&	m_eCDT != __ECDT_TABBED_REINDEX_NHM
									)
								||
								(	(!m_bDeepHalfSplit)
									&& (		m_eCDT == __ECDT_TABBED_NEW
											||	m_eCDT == __ECDT_TABBED_UPDATE
											||	m_eCDT == __ECDT_TABBED_REINDEX
											||	m_eCDT == __ECDT_TABBED_REINDEX_NHM
											)
									)
							);
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

//						if( bHorz )
//						{
							if( rcPart.Width() < nSrcMinHW )
								m_eCDT = __ECDT_FLOATED;
//						} // if( bHorz )
//						else
//						{
							if( rcPart.Height() < nSrcMinVH )
								m_eCDT = __ECDT_FLOATED;
//						} // else from if( bHorz )
						if( m_eCDT != __ECDT_FLOATED )
							rcBarWnd = rcPart;
					} // if( m_eCDT != __ECDT_FLOATED )
				
				} // if( bEnableInnerDock && (m_pExtBarSrc->m_dwDockStyle&CBRS_ALIGN_ANY) != 0 )

			} // if( pDesiredBar != NULL )
		} // try to analyze docking into floating bars

		if( bCanDockToInnerCircles && m_eCDT == __ECDT_FLOATED && (!bForceFloatMode) )
		{
			// first try calc by circle 0 inner part
			CRect rcCircle0( 0, 0, 0, 0 );

			CExtDockBar * vDockBars0[4];
			INT nDockSide = 0;
			for( nDockSide = 0; nDockSide < 4;  nDockSide++ )
			{
				UINT nDockBarID =
					_dwDockBarMap[ nDockSide ] [ 0 ];
				ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );
				vDockBars0[ nDockSide ] = (CExtDockBar *)
					pFrame->GetControlBar( nDockBarID );
				ASSERT_VALID( vDockBars0[ nDockSide ] );
				ASSERT_KINDOF( CExtDockBar, vDockBars0[ nDockSide ] );
				ASSERT( vDockBars0[ nDockSide ] -> _GetCircleNo() == 0 );

				CRect rcDockBar;
				vDockBars0[ nDockSide ]->_GetPreCalcWindowRect( rcDockBar );
				switch( nDockBarID )
				{
				case AFX_IDW_DOCKBAR_TOP:
					rcCircle0.top = rcDockBar.bottom;
					break;
				case AFX_IDW_DOCKBAR_BOTTOM:
					rcCircle0.bottom = rcDockBar.top;
					break;
				case AFX_IDW_DOCKBAR_LEFT:
					rcCircle0.left = rcDockBar.right;
					break;
				case AFX_IDW_DOCKBAR_RIGHT:
					rcCircle0.right = rcDockBar.left;
					break;
#ifdef _DEBUG
				default:
					{
						ASSERT( FALSE );
					}
					break;
#endif // _DEBUG
				} // switch( nDockBarID )
			} // for( nDockSide = 0; nDockSide < 4;  nDockSide++ )

//			ASSERT( rcFrameWnd.left <= rcCircle0.left );
//			ASSERT( rcFrameWnd.top <= rcCircle0.top );
//			ASSERT( rcFrameWnd.right >= rcCircle0.right );
//			ASSERT( rcFrameWnd.bottom >= rcCircle0.bottom );
			CRect rcTestCircle0( rcCircle0 );
			//rcTestCircle0.InflateRect( nRdpOuter, nRdpOuter );
			rcTestCircle0.InflateRect( nRdpMostInner, nRdpMostInner );
			if( rcCircle0.left < rcCircle0.right
				&& rcCircle0.top < rcCircle0.bottom
				&& rcTestCircle0.PtInRect( ptTrack )
				)
			{ // if we have sense analyze docking into circle 0 inner part/circle 1 outer part
				for( nDockSide = 0; nDockSide < 4;  nDockSide++ )
				{
					DWORD dwDockStyle =
						_dwDockBarMap[ nDockSide ] [ 1 ];
					if( (m_pExtBarSrc->m_dwDockStyle & dwDockStyle) == 0 )
						continue; // can not dock at this side

					UINT nDockBarID =
						_dwDockBarMap[ nDockSide ] [ 0 ];
					ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );

					CRect rcFrameAnalyze( rcTestCircle0 );

					switch( nDockBarID )
					{
					case AFX_IDW_DOCKBAR_TOP:
					{
						//rcFrameAnalyze.top -= nRdpMostInner;
						rcFrameAnalyze.bottom =
							rcFrameAnalyze.top + nRdpOuter;
						if( rcFrameAnalyze.PtInRect(ptTrack) )
						{
							m_eCDT = __ECDT_CIRCLE1_OUTER_TOP;
							m_nCircleNo = 1;

							rcBarWnd.top = rcCircle0.top;
							rcBarWnd.bottom =
								rcBarWnd.top +
								m_pExtBarSrc->m_sizeDockedH.cy;
							rcBarWnd.left = rcCircle0.left;
							rcBarWnd.right = rcCircle0.right;
						} // if( rcFrameAnalyze.PtInRect(ptTrack) )
					}
					break; // case AFX_IDW_DOCKBAR_TOP
					case AFX_IDW_DOCKBAR_BOTTOM:
					{
						//rcFrameAnalyze.bottom += nRdpMostInner;
						rcFrameAnalyze.top =
							rcFrameAnalyze.bottom - nRdpOuter;
						if( rcFrameAnalyze.PtInRect(ptTrack) )
						{
							m_eCDT = __ECDT_CIRCLE1_OUTER_BOTTOM;
							m_nCircleNo = 1;

							rcBarWnd.bottom = rcCircle0.bottom;
							rcBarWnd.top =
								rcBarWnd.bottom -
								m_pExtBarSrc->m_sizeDockedH.cy;
							rcBarWnd.left = rcCircle0.left;
							rcBarWnd.right = rcCircle0.right;
						} // if( rcFrameAnalyze.PtInRect(ptTrack) )
					}
					break; // case AFX_IDW_DOCKBAR_BOTTOM
					case AFX_IDW_DOCKBAR_LEFT:
					{
						//rcFrameAnalyze.left -= nRdpMostInner;
						rcFrameAnalyze.right =
							rcFrameAnalyze.left + nRdpOuter;
						if( rcFrameAnalyze.PtInRect(ptTrack) )
						{
							m_eCDT = __ECDT_CIRCLE1_OUTER_LEFT;
							m_nCircleNo = 1;
							
							rcBarWnd.left = rcCircle0.left;
							rcBarWnd.right =
								rcBarWnd.left +
								m_pExtBarSrc->m_sizeDockedV.cx;
							rcBarWnd.top = rcCircle0.top;
							rcBarWnd.bottom = rcCircle0.bottom;
						} // if( rcFrameAnalyze.PtInRect(ptTrack) )
					}
					break; // case AFX_IDW_DOCKBAR_LEFT
					case AFX_IDW_DOCKBAR_RIGHT:
					{
						//rcFrameAnalyze.right += nRdpMostInner;
						rcFrameAnalyze.left =
							rcFrameAnalyze.right - nRdpOuter;
						if( rcFrameAnalyze.PtInRect(ptTrack) )
						{
							m_eCDT = __ECDT_CIRCLE1_OUTER_RIGHT;
							m_nCircleNo = 1;
							
							rcBarWnd.right = rcCircle0.right;
							rcBarWnd.left =
								rcBarWnd.right -
								m_pExtBarSrc->m_sizeDockedV.cx;
							rcBarWnd.top = rcCircle0.top;
							rcBarWnd.bottom = rcCircle0.bottom;
						} // if( rcFrameAnalyze.PtInRect(ptTrack) )
					} // case AFX_IDW_DOCKBAR_RIGHT
					break;
#ifdef _DEBUG
					default:
						{
							ASSERT( FALSE );
						}
					break;
#endif // _DEBUG
					} // switch( nDockBarID )

					if( m_eCDT != __ECDT_FLOATED )
						break;
				} // for( nDockSide = 0; nDockSide < 4;  nDockSide++ )
			} // if we have sense analyze docking into circle 0 inner part/circle 1 outer part

			if( m_eCDT == __ECDT_FLOATED )
			{ // try calc by most inner dockbars

				CRect rcFrameTest( rcFrameWnd );
				rcFrameTest.InflateRect( nRdpInner, nRdpInner );
				if( rcFrameTest.PtInRect( ptTrack ) )
				{
					
					CRect rcFrameInner;
					pFrame->GetClientRect( &rcFrameInner );
					RepositionBarsEx(
						pFrame,
						0,
						0x0FFFF,
						AFX_IDW_PANE_FIRST,
						CWnd::reposQuery,
						rcFrameInner,
						rcFrameInner
						);
//					pFrame->RepositionBars(
//						0,
//						0x0FFFF,
//						AFX_IDW_PANE_FIRST,
//						CWnd::reposQuery,
//						rcFrameInner,
//						rcFrameInner
//						);
					pFrame->ClientToScreen( &rcFrameInner );

					if( rcFrameInner.left < rcFrameInner.right
						&& rcFrameInner.top < rcFrameInner.bottom
						&& rcFrameInner.PtInRect( ptTrack )
						)
					{ // if we have any space to dock into inner circle
//						ASSERT( rcFrameWnd.left <= rcFrameInner.left );
//						ASSERT( rcFrameWnd.top <= rcFrameInner.top );
//						ASSERT( rcFrameWnd.right >= rcFrameInner.right );
//						ASSERT( rcFrameWnd.bottom >= rcFrameInner.bottom );

						CSize _sizeMaxInner = rcFrameInner.Size();
						_sizeMaxInner.cx = ::MulDiv( _sizeMaxInner.cx, 3, 4 );
						_sizeMaxInner.cy = ::MulDiv( _sizeMaxInner.cy, 3, 4 );

						UINT nSrcBarCircleNo =
							((CExtDockBar *)m_pExtBarSrc->m_pDockBar)->_GetCircleNo();
						for( nDockSide = 0; nDockSide < 4;  nDockSide++ )
						{
							DWORD dwDockStyle =
								_dwDockBarMap[ nDockSide ] [ 1 ];
							if( (m_pExtBarSrc->m_dwDockStyle & dwDockStyle) == 0 )
								continue; // can not dock at this side
							UINT nDockBarID =
								_dwDockBarMap[ nDockSide ] [ 0 ];
							ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );

							CExtDockBar * pDockBar = vDockBars0[ nDockSide ];
							ASSERT_VALID( pDockBar );
							ASSERT_KINDOF( CExtDockBar, pDockBar );
							ASSERT( pDockBar->_GetCircleNo() == 0 );

							if( bShowContent && m_pExtBarSrc->IsDocked() )
							{
								ASSERT( m_pExtBarSrc->m_pDockBar != NULL );
								ASSERT_VALID( m_pExtBarSrc->m_pDockBar );
								ASSERT_KINDOF( CExtDockBar, m_pExtBarSrc->m_pDockBar );
								if( pDockBar == m_pExtBarSrc->m_pDockBar )
								{
									CRect rcDockBar;
									pDockBar->GetWindowRect( &rcDockBar );
									if( rcDockBar.PtInRect(ptTrack) )
									{
										m_eCDT = __ECDT_OWN_AREA;
										nDstMinHW = nSrcMinHW;
										nDstMinVH = nSrcMinVH;
										return;
									}
									continue;
								} // if( pDockBar == m_pExtBarSrc->m_pDockBar )
							} // if( bShowContent && m_pExtBarSrc->IsDocked() )

							pDockBar = pDockBar->_GetDockBarInnerTop();
							ASSERT_VALID( pDockBar );
							ASSERT_KINDOF( CExtDockBar, pDockBar );
							
							CRect rcFrameAnalyze( rcFrameInner );

							switch( nDockBarID )
							{
							case AFX_IDW_DOCKBAR_TOP:
							{
								rcFrameAnalyze.bottom =
									rcFrameAnalyze.top + nRdpMostInner;
								if( rcFrameAnalyze.PtInRect(ptTrack) )
								{
									m_eCDT = __ECDT_DOCKBAR_INNER_TOP;
									m_nCircleNo = pDockBar->_GetCircleNo();

									bool bDeepCircleTarget = false;
									if(	m_nCircleNo <= (nSrcBarCircleNo + 2) )
										bDeepCircleTarget = true;
									if( !bDeepCircleTarget )
									{
										CExtDockBar * pBarLeft =
											pDockBar->_GetInCircle( AFX_IDW_DOCKBAR_LEFT );
										ASSERT_VALID( pBarLeft );
										CExtDockBar * pBarRight =
											pDockBar->_GetInCircle( AFX_IDW_DOCKBAR_RIGHT );
										ASSERT_VALID( pBarRight );
										if( !pBarLeft->_CanBeSafeOptimized()
											|| !pBarLeft->_CanBeSafeOptimized()
											)
											bDeepCircleTarget = true;
									}
									if( bDeepCircleTarget )
										m_nCircleNo++;

									rcBarWnd.top = rcFrameInner.top;
									rcBarWnd.bottom =
										rcBarWnd.top
										+
										min(
											m_pExtBarSrc->m_sizeDockedH.cy,
											_sizeMaxInner.cy
											);
									rcBarWnd.left = rcFrameInner.left;
									rcBarWnd.right = rcFrameInner.right;
								} // if( rcFrameAnalyze.PtInRect(ptTrack) )
							}
							break; // case AFX_IDW_DOCKBAR_TOP
							case AFX_IDW_DOCKBAR_BOTTOM:
							{
								rcFrameAnalyze.top =
									rcFrameAnalyze.bottom - nRdpMostInner;
								if( rcFrameAnalyze.PtInRect(ptTrack) )
								{
									m_eCDT = __ECDT_DOCKBAR_INNER_BOTTOM;
									m_nCircleNo = pDockBar->_GetCircleNo();

									bool bDeepCircleTarget = false;
									if(	m_nCircleNo <= (nSrcBarCircleNo + 2) )
										bDeepCircleTarget = true;
									if( !bDeepCircleTarget )
									{
										CExtDockBar * pBarLeft =
											pDockBar->_GetInCircle( AFX_IDW_DOCKBAR_LEFT );
										ASSERT_VALID( pBarLeft );
										CExtDockBar * pBarRight =
											pDockBar->_GetInCircle( AFX_IDW_DOCKBAR_RIGHT );
										ASSERT_VALID( pBarRight );
										if( !pBarLeft->_CanBeSafeOptimized()
											|| !pBarLeft->_CanBeSafeOptimized()
											)
											bDeepCircleTarget = true;
									}
									if( bDeepCircleTarget )
										m_nCircleNo++;

									rcBarWnd.bottom = rcFrameInner.bottom;
									rcBarWnd.top =
										rcBarWnd.bottom
										-
										min(
											m_pExtBarSrc->m_sizeDockedH.cy,
											_sizeMaxInner.cy
											);
									rcBarWnd.left = rcFrameInner.left;
									rcBarWnd.right = rcFrameInner.right;
								} // if( rcFrameAnalyze.PtInRect(ptTrack) )
							}
							break; // case AFX_IDW_DOCKBAR_BOTTOM
							case AFX_IDW_DOCKBAR_LEFT:
							{
								rcFrameAnalyze.right =
									rcFrameAnalyze.left + nRdpMostInner;
								if( rcFrameAnalyze.PtInRect(ptTrack) )
								{
									m_eCDT = __ECDT_DOCKBAR_INNER_LEFT;
									m_nCircleNo = pDockBar->_GetCircleNo();

									bool bDeepCircleTarget = false;
									if(	m_nCircleNo <= (nSrcBarCircleNo + 2) )
										bDeepCircleTarget = true;
									if( !bDeepCircleTarget )
									{
										CExtDockBar * pBarTop =
											pDockBar->_GetInCircle( AFX_IDW_DOCKBAR_TOP );
										ASSERT_VALID( pBarTop );
										CExtDockBar * pBarBottom =
											pDockBar->_GetInCircle( AFX_IDW_DOCKBAR_BOTTOM );
										ASSERT_VALID( pBarBottom );
										if( !pBarTop->_CanBeSafeOptimized()
											|| !pBarBottom->_CanBeSafeOptimized()
											)
											bDeepCircleTarget = true;
									}
									if( bDeepCircleTarget )
										m_nCircleNo++;
									
									rcBarWnd.left = rcFrameInner.left;
									rcBarWnd.right =
										rcBarWnd.left
										+
										min(
											m_pExtBarSrc->m_sizeDockedV.cx,
											_sizeMaxInner.cx
											);
									rcBarWnd.top = rcFrameInner.top;
									rcBarWnd.bottom = rcFrameInner.bottom;
								} // if( rcFrameAnalyze.PtInRect(ptTrack) )
							}
							break; // case AFX_IDW_DOCKBAR_LEFT
							case AFX_IDW_DOCKBAR_RIGHT:
							{
								rcFrameAnalyze.left =
									rcFrameAnalyze.right - nRdpMostInner;
								if( rcFrameAnalyze.PtInRect(ptTrack) )
								{
									m_eCDT = __ECDT_DOCKBAR_INNER_RIGHT;
									m_nCircleNo = pDockBar->_GetCircleNo();

									bool bDeepCircleTarget = false;
									if(	m_nCircleNo <= (nSrcBarCircleNo + 2) )
										bDeepCircleTarget = true;
									if( !bDeepCircleTarget )
									{
										CExtDockBar * pBarTop =
											pDockBar->_GetInCircle( AFX_IDW_DOCKBAR_TOP );
										ASSERT_VALID( pBarTop );
										CExtDockBar * pBarBottom =
											pDockBar->_GetInCircle( AFX_IDW_DOCKBAR_BOTTOM );
										ASSERT_VALID( pBarBottom );
										if( !pBarTop->_CanBeSafeOptimized()
											|| !pBarBottom->_CanBeSafeOptimized()
											)
											bDeepCircleTarget = true;
									}
									if( bDeepCircleTarget )
										m_nCircleNo++;
									
									rcBarWnd.right = rcFrameInner.right;
									rcBarWnd.left =
										rcBarWnd.right
										-
										min(
											m_pExtBarSrc->m_sizeDockedV.cx,
											_sizeMaxInner.cx
											);
									rcBarWnd.top = rcFrameInner.top;
									rcBarWnd.bottom = rcFrameInner.bottom;
								} // if( rcFrameAnalyze.PtInRect(ptTrack) )
							} // case AFX_IDW_DOCKBAR_RIGHT
							break;
#ifdef _DEBUG
							default:
								{
									ASSERT( FALSE );
								}
							break;
#endif // _DEBUG
							} // switch( nDockBarID )

							if( m_eCDT != __ECDT_FLOATED )
								break;
						} // for( nDockSide = 0; nDockSide < 4;  nDockSide++ )
					} // if we have any space to dock into inner circle
				} // if( rcFrameTest.PtInRect( ptTrack ) )
			} // try calc by most inner dockbars
		} // if( bCanDockToInnerCircles && m_eCDT == __ECDT_FLOATED )

		if( m_eCDT == __ECDT_FLOATED && (!bForceFloatMode) )
		{ // try calc by other bar
		
			ExtControlBarVector_t vBars;
			m_pExtBarSrc->_GetFrameControlBars(
#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
				( INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005) )
					? true : 
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)
					false,
				vBars,
				(DWORD)(
					__GFECB_NONFIXED_NONMIN //|__GFECB_FIXED
						|__GFECB_DOCKED
						|__GFECB_VIS_ALL
						|__GFECB_BYPT_ONLY_WND
					),
				ptTrack
				);
			INT nCountOfBars = (INT)vBars.GetSize();
			ASSERT( nCountOfBars >= 0 && nCountOfBars <= 1 );

			if( nCountOfBars == 1 )
			{
				m_pExtBarDst = vBars[0];
				//ExtBarDstSet( vBars[0] );
				ASSERT_VALID( m_pExtBarDst );
				m_pExtBarDst->GetWindowRect( &rcBarWnd );
				if( m_pExtBarDst == m_pExtBarSrc )
				{
					m_eCDT = __ECDT_OWN_AREA;
					
					nDstMinHW = nSrcMinHW;
					nDstMinVH = nSrcMinVH;
				} // if( m_pExtBarDst == m_pExtBarSrc )
				else //if( ! bShowContent )
				{
					m_eCDT = __ECDT_FLOATED;

					bool bCanAnalyzeInnerParts = true;
					if( !bCanDockToInnerCircles )
					{
						ASSERT_VALID( m_pExtBarDst->m_pDockBar );
						CExtDockBar * pDockBarDst =
							STATIC_DOWNCAST( CExtDockBar, m_pExtBarDst->m_pDockBar );
						ASSERT_KINDOF( CExtDockBar, pDockBarDst );
						if( pDockBarDst->_GetCircleNo() > 0 )
							bCanAnalyzeInnerParts = false;
					}

					if( bCanAnalyzeInnerParts && m_pExtBarDst != NULL )
					{
						nDstMinHW = m_pExtBarDst->_CalcDesiredMinHW();
						nDstMinVH = m_pExtBarDst->_CalcDesiredMinVH();
						INT nWidth3   = nRdpInner; // rcBarWnd.Width();
						INT nHeight3  = nRdpInner; // rcBarWnd.Height();
//						INT nWidth2  = nWidth  / 2;
//						INT nHeight2 = nHeight / 2;
//						INT nWidth3  = nWidth  / 4;
//						INT nHeight3 = nHeight / 4;

						ASSERT_VALID( m_pExtBarDst );
						UINT nDockBarID =
							m_pExtBarDst->GetSafeDockBarDlgCtrlID();
						if( nDockBarID == AFX_IDW_DOCKBAR_FLOAT )
							nDockBarID = AFX_IDW_DOCKBAR_TOP;
						ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );
						bool bHorz = 
							(	nDockBarID == AFX_IDW_DOCKBAR_TOP
							||	nDockBarID == AFX_IDW_DOCKBAR_BOTTOM
							) ? true : false;
						ASSERT( m_pExtBarDst->m_pDockBar != NULL );

//						CExtDockBar * pDockBar =
//							STATIC_DOWNCAST( CExtDockBar, m_pExtBarDst->m_pDockBar );
						CDockBar * pDockBar = m_pExtBarDst->m_pDockBar;

						ASSERT_VALID( pDockBar );

//						if( bShowContent && m_pExtBarSrc->IsDocked() )
//						{
//							ASSERT( m_pExtBarDst->IsDocked() );
//							ASSERT_VALID( m_pExtBarSrc->m_pDockBar );
//							if( m_pExtBarSrc->m_pDockBar == pDockBar )
//							{
//								CRect rcWndTargetBar, rcWndSrcBar;
//								m_pExtBarDst->GetWindowRect( &rcWndTargetBar );
//								m_pExtBarSrc->GetWindowRect( &rcWndSrcBar );
//								rcWndTargetBar.InflateRect( 2, 2 );
//								if( bHorz )
//								{
//									if(		abs(rcWndSrcBar.top-rcWndTargetBar.bottom) <= 2
//										||	abs(rcWndSrcBar.bottom-rcWndTargetBar.top) <= 2
//										)
//									{
//										rcWndSrcBar.InflateRect( 2, 0 );
//										if(		rcWndSrcBar.left < rcWndTargetBar.right
//												&& rcWndTargetBar.right < rcWndSrcBar.right
//											||
//												rcWndSrcBar.left < rcWndTargetBar.left
//												&& rcWndTargetBar.left < rcWndSrcBar.right
//											)
//										{
//											m_eCDT = __ECDT_OWN_AREA;
//											nDstMinHW = nSrcMinHW;
//											nDstMinVH = nSrcMinVH;
//											return;
//										}
//									}
//								} // if( bHorz )
//								else
//								{
//									if(		abs(rcWndSrcBar.left-rcWndTargetBar.right) <= 2
//										||	abs(rcWndSrcBar.right-rcWndTargetBar.left) <= 2
//										)
//									{
//										rcWndSrcBar.InflateRect( 0, 2 );
//										if(		rcWndSrcBar.top < rcWndTargetBar.bottom
//												&& rcWndTargetBar.bottom < rcWndSrcBar.bottom
//											||
//												rcWndSrcBar.top < rcWndTargetBar.top
//												&& rcWndTargetBar.top < rcWndSrcBar.bottom
//											)
//										{
//											m_eCDT = __ECDT_OWN_AREA;
//											nDstMinHW = nSrcMinHW;
//											nDstMinVH = nSrcMinVH;
//											return;
//										}
//									}
//								} // else from if( bHorz )
//							} // if( m_pExtBarSrc->m_pDockBar == pDockBar )
//						} // if( bShowContent && m_pExtBarSrc->IsDocked() )

						bool bEnableInnerDock = true;
						if( nDockBarID == AFX_IDW_DOCKBAR_LEFT
							&& (m_pExtBarSrc->m_dwDockStyle&CBRS_ALIGN_LEFT) == 0
							)
							bEnableInnerDock = false;
						if( nDockBarID == AFX_IDW_DOCKBAR_RIGHT
							&& (m_pExtBarSrc->m_dwDockStyle&CBRS_ALIGN_RIGHT) == 0
							)
							bEnableInnerDock = false;
						if( nDockBarID == AFX_IDW_DOCKBAR_TOP
							&& (m_pExtBarSrc->m_dwDockStyle&CBRS_ALIGN_TOP) == 0
							)
							bEnableInnerDock = false;
						if( nDockBarID == AFX_IDW_DOCKBAR_BOTTOM
							&& (m_pExtBarSrc->m_dwDockStyle&CBRS_ALIGN_BOTTOM) == 0
							)
							bEnableInnerDock = false;
						
						if( bEnableInnerDock && (m_pExtBarSrc->m_dwDockStyle&CBRS_ALIGN_ANY) != 0 )
						{
							m_bDeepHalfSplit = false;
							if( !m_pExtBarDst->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar))
								|| m_pExtBarDst->IsFloating()
								)
								m_bDeepHalfSplit = true;
							
							CRect rcWndDockBar;
							pDockBar->GetWindowRect( &rcWndDockBar );
							
							CRect rcPart( rcBarWnd );

							do
							{ // SINGLE LOOP
								
								INT nHalfHeight = nRdpInner*2; // rcBarWnd.Height()/2;
								if(	rcBarWnd.Width() >= nSrcMinHW
									&& nHalfHeight >= nSrcMinVH
									&& nHalfHeight >= nDstMinVH
									)
								{ // if can do half splitting
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
									BOOL bTabbedDest =
										m_pExtBarDst->IsKindOf(RUNTIME_CLASS(CExtDynTabControlBar))
										&& ( ((CExtDynTabControlBar*)m_pExtBarDst)->IsSwitcherVisible() )
										;

									INT nTabbedShift = 0;
									bool bCanDockToTabbedContainers =
										(	m_pExtBarSrc->_CanDockToTabbedContainers( m_pExtBarDst )
											&& m_pExtBarDst->_CanDockToTabbedContainers( m_pExtBarSrc )
											) ? true : false;
									if( bCanDockToTabbedContainers )
									{
											rcPart = rcBarWnd;
										if( g_bTabsAtTop )
											rcPart.top = rcPart.bottom - nHeight3;
										else
											rcPart.bottom = rcPart.top + nHeight3;
										if(	rcPart.PtInRect(ptTrack) )
										{
											if( bTabbedDest )
												m_eCDT = __ECDT_TABBED_UPDATE;
											else
												m_eCDT = __ECDT_TABBED_NEW;
											rcPart = rcBarWnd;
											break;
										}
										if( g_bTabsAtTop )
										{
											//rcPart = rcBarWnd;
											m_pExtBarDst->GetClientRect( &rcPart );
											rcPart.bottom = rcPart.top + nHeight3;
											nTabbedShift = nHeight3;
											//rcPart.OffsetRect( 0, nTabbedShift );
										}
										else
										{
											nTabbedShift = nHeight3;
											rcPart.OffsetRect( 0, nTabbedShift );
										}
									}
									else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
									{
										rcPart = rcBarWnd;
										rcPart.bottom = rcPart.top + nHeight3;
									}
									
									if( rcPart.PtInRect(ptTrack) )
									{
										if( !bShowContent || !bHorz )
										{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
											if( g_bTabsAtTop )
												rcPart.OffsetRect( 0, nTabbedShift );
											else
												rcPart.OffsetRect( 0, -nTabbedShift );
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
								m_bDeepHalfSplit = true;
											m_eCDT = __ECDT_INNER_TOP;
											rcPart.bottom = rcPart.top + rcBarWnd.Height()/2; // + nHeight2;
											
											if( bHorz && !m_bDeepHalfSplit )
											{
												rcPart.left = min( rcPart.left, rcWndDockBar.left );
												rcPart.right = max( rcPart.right, rcWndDockBar.right );
											}
											break;
										} // if( !bShowContent || !bHorz )
									} // if( rcPart.PtInRect(ptTrack) )

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
									rcPart = rcBarWnd;
									INT nBottomShift = 0;
									if(		bCanDockToTabbedContainers
										//&&	(!bShowContent)
										&&	bTabbedDest
										)
									{
										CExtDynTabControlBar * pTabbedBar =
											(CExtDynTabControlBar *)m_pExtBarDst;
										CRect rcTabSwitcher;
										pTabbedBar->GetSwitcherWindowRect( rcTabSwitcher );
										if( rcTabSwitcher.PtInRect(ptTrack) )
										{
											m_eCDT = __ECDT_TABBED_REINDEX;
											ReindexPosCalc( ptTrack );
											break;
										}
										nBottomShift = rcTabSwitcher.Size().cy;
									}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

									rcPart = rcBarWnd;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
									rcPart.bottom -= nBottomShift;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
									rcPart.top = rcPart.bottom - nHeight3;
									if( rcPart.PtInRect(ptTrack) )
									{
										if( !bShowContent || !bHorz )
										{
/*
											CRect rcDeep(
												rcPart.left,
												rcPart.top,
												rcPart.right,
												rcPart.bottom-rcPart.Height()/2
												);
											m_bDeepHalfSplit =
												rcDeep.PtInRect(ptTrack) ? true : false;
*/
m_bDeepHalfSplit = true;

											m_eCDT = __ECDT_INNER_BOTTOM;
											rcPart.top = rcPart.bottom - rcBarWnd.Height()/2;// - nHeight2;
											
											if( bHorz && !m_bDeepHalfSplit )
											{
												rcPart.left = min( rcPart.left, rcWndDockBar.left );
												rcPart.right = max( rcPart.right, rcWndDockBar.right );
											}
											break;
										} // if( !bShowContent || !bHorz )
									} // if( rcPart.PtInRect(ptTrack) )
								} // if can do half splitting
								
								INT nHalfWidth = nRdpInner*2; // rcBarWnd.Width()/2;
								if( rcBarWnd.Height() >= nSrcMinVH
									&& nHalfWidth >= nSrcMinHW
									&& nHalfWidth >= nDstMinHW
									)
								{ // if can do half splitting
									rcPart = rcBarWnd;
									rcPart.right = rcPart.left + nWidth3;
									if( rcPart.PtInRect(ptTrack) )
									{
										if( !bShowContent || bHorz )
										{
/*
											CRect rcDeep(
												rcPart.left+rcPart.Width()/2,
												rcPart.top,
												rcPart.right,
												rcPart.bottom
												);
											m_bDeepHalfSplit =
												rcDeep.PtInRect(ptTrack) ? true : false;
*/
m_bDeepHalfSplit = true;

											m_eCDT = __ECDT_INNER_LEFT;
											rcPart.right = rcPart.left + rcBarWnd.Width()/2; // + nWidth2;
											
											if( !bHorz && !m_bDeepHalfSplit )
											{
												rcPart.top = min( rcPart.top, rcWndDockBar.top );
												rcPart.bottom = max( rcPart.bottom, rcWndDockBar.bottom );
											}
											break;
										} // if( !bShowContent || bHorz )
									} // if( rcPart.PtInRect(ptTrack) )

									rcPart = rcBarWnd;
									rcPart.left = rcPart.right - nWidth3;
									if( rcPart.PtInRect(ptTrack) )
									{
										if( !bShowContent || bHorz )
										{
/*
											CRect rcDeep(
												rcPart.left,
												rcPart.top,
												rcPart.right-rcPart.Width()/2,
												rcPart.bottom
												);
											m_bDeepHalfSplit =
												rcDeep.PtInRect(ptTrack) ? true : false;
*/
m_bDeepHalfSplit = true;

											m_eCDT = __ECDT_INNER_RIGHT;
											rcPart.left = rcPart.right - rcBarWnd.Width()/2; // - nWidth2;
											
											if( !bHorz && !m_bDeepHalfSplit )
											{
												rcPart.top = min( rcPart.top, rcWndDockBar.top );
												rcPart.bottom = max( rcPart.bottom, rcWndDockBar.bottom );
											}
											break;
										} // if( !bShowContent || bHorz )
									} // if( rcPart.PtInRect(ptTrack) )
								} // if can do half splitting

							} // SINGLE LOOP
							while( false );

							if( m_bDeepHalfSplit )
							{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
								if(	m_bDeepHalfSplit
									&& (		m_eCDT == __ECDT_TABBED_NEW
											||	m_eCDT == __ECDT_TABBED_UPDATE
											||	m_eCDT == __ECDT_TABBED_REINDEX
											||	m_eCDT == __ECDT_TABBED_REINDEX_NHM
											)
									)
									m_bDeepHalfSplit = false;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

//								if(	m_pExtBarDst->IsFloating()
//									|| m_pExtBarDst->IsSingleOnRow()
//									)
//									m_bDeepHalfSplit = false;
								if(	!m_pExtBarDst->IsFloating()
									&& m_pExtBarDst->IsSingleOnRow()
									)
									m_bDeepHalfSplit = false;
							} // if( m_bDeepHalfSplit )

							if( m_eCDT != __ECDT_FLOATED )
							{
								if( bHorz )
								{
									if( rcPart.Width() < nSrcMinHW )
										m_eCDT = __ECDT_FLOATED;
								} // if( bHorz )
								else
								{
									if( rcPart.Height() < nSrcMinVH )
										m_eCDT = __ECDT_FLOATED;
								} // else from if( bHorz )
								if( m_eCDT != __ECDT_FLOATED )
									rcBarWnd = rcPart;
							} // if( m_eCDT != __ECDT_FLOATED )
						
						} // if( bEnableInnerDock && (m_pExtBarSrc->m_dwDockStyle&CBRS_ALIGN_ANY) != 0 )
					} // if( bCanAnalyzeInnerParts && m_pExtBarDst != NULL )

				} // else if( !bShowContent ) -> else from if( m_pExtBarDst == m_pExtBarSrc )
			} // if( nCountOfBars == 1 )
		} // try calc by other bar

	} // if( !CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) ....

#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
	if(		INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005)
		&&	m_eCDT != __ECDT_FLOATED
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		&&	m_eCDT != __ECDT_TABBED_REINDEX
		&&	m_eCDT != __ECDT_TABBED_REINDEX_NHM
		&&	m_eCDT != __ECDT_TABBED_UPDATE
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		)
		m_eCDT = __ECDT_FLOATED;
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)

	if( m_eCDT == __ECDT_FLOATED )
	{
		//m_pExtBarDst = NULL;
		ExtBarDstSet( NULL );
		if( m_pExtBarSrc->IsFloating() )
		{
			CMiniDockFrameWnd * pMiniFrame = 
				DYNAMIC_DOWNCAST(
					CMiniDockFrameWnd,
					m_pExtBarSrc->GetDockingFrame()
					);
			ASSERT_VALID( pMiniFrame );
			pMiniFrame->GetWindowRect( &rcBarWnd );
			rcBarWnd.OffsetRect( -rcBarWnd.TopLeft() );

			rcBarWnd.OffsetRect(
				ptTrack - m_pExtBarSrc->m_ptHelperDragOffsetFloated
				);

			CRect rcGrip = m_pExtBarSrc->_RectGripGet();
			INT nCyGripper = min( rcGrip.Width(), rcGrip.Height() );
			rcBarWnd.OffsetRect( -nCyGripper, -nCyGripper/2 );

		} // if( m_pExtBarSrc->IsFloating() )
		else
		{
			rcBarWnd.SetRect(
				ptTrack,
				ptTrack + m_pExtBarSrc->m_sizeFloated
				);
			rcBarWnd.OffsetRect(
				- m_pExtBarSrc->m_ptHelperDragOffsetFloated
				);
		} // else from if( m_pExtBarSrc->IsFloating() )

		rcBarWnd.OffsetRect( rcFrameWnd.TopLeft() );
	} // if( m_eCDT == __ECDT_FLOATED )

	TrackRectSet( &rcBarWnd );
}

#define __RECALC_FRAME_STEPS( __FRAME_PTR__ ) \
	( (__FRAME_PTR__)->IsKindOf(RUNTIME_CLASS(CExtMiniDockFrameWnd)) \
		? (__FRAME_PTR__)->RecalcLayout() \
		: (__FRAME_PTR__)->DelayRecalcLayout() )

void CExtControlBar::_DraggingDoDeepHalfSplit(
	CExtControlBar * pExtBar,
	UINT nHalfSplitID, // one of AFX_IDW_DOCKBAR_... docked value
	int nPercentToOccupy // = 50 // greater than zero and less than one hundred
	)
{
	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtControlBar, this );
	ASSERT( !IsFixedMode() );
	ASSERT_VALID( m_pDockSite );
	ASSERT_VALID( m_pDockBar );
	ASSERT_KINDOF( CDockBar, m_pDockBar );
	
	ASSERT_VALID( pExtBar );
	ASSERT_KINDOF( CExtControlBar, pExtBar );
	ASSERT( !pExtBar->IsFixedMode() );

	ASSERT( 0 < nPercentToOccupy && nPercentToOccupy < 100 );

#ifdef _DEBUG
	if( pExtBar->m_pDockBar != NULL )	
	{
		ASSERT_VALID( pExtBar->m_pDockSite );
		ASSERT_VALID( pExtBar->m_pDockBar );
		ASSERT_KINDOF( CDockBar, pExtBar->m_pDockBar );
		ASSERT( pExtBar->m_pDockSite == m_pDockSite );
	}
#endif // _DEBUG
	
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nHalfSplitID );

CDockBar * pDockBarIn = m_pDockBar;
//		STATIC_DOWNCAST( CExtDockBar, m_pDockBar );
CDockBar * pDockBarOut = pExtBar->m_pDockBar;
//		STATIC_DOWNCAST( CExtDockBar, pExtBar->m_pDockBar );

CRect rcDyn;
	GetWindowRect( &rcDyn );
CRect rcIn( rcDyn );
CRect rcOut( rcDyn );
	pDockBarIn->ScreenToClient( &rcDyn );
UINT nNewID = AFX_IDW_DOCKBAR_TOP;
DWORD nBarStyle = CBRS_ALIGN_TOP;
	switch( nHalfSplitID )
	{
	case AFX_IDW_DOCKBAR_TOP:
		nNewID = AFX_IDW_DOCKBAR_LEFT;
		nBarStyle = CBRS_ALIGN_LEFT;
		//rcIn.top += rcIn.Height()/2;
		rcIn.top += ::MulDiv( rcIn.Height(), nPercentToOccupy, 100 );
		rcOut.bottom = rcIn.top;
		break;
	case AFX_IDW_DOCKBAR_BOTTOM:
		nNewID = AFX_IDW_DOCKBAR_LEFT;
		nBarStyle = CBRS_ALIGN_LEFT;
		//rcIn.bottom -= rcIn.Height()/2;
		rcIn.bottom -= ::MulDiv( rcIn.Height(), nPercentToOccupy, 100 );
		rcOut.top = rcIn.bottom;
		break;
	case AFX_IDW_DOCKBAR_LEFT:
		//rcIn.left += rcIn.Width()/2;
		rcIn.left += ::MulDiv( rcIn.Width(), nPercentToOccupy, 100 );
		rcOut.right = rcIn.left;
		break;
	case AFX_IDW_DOCKBAR_RIGHT:
		//rcIn.right -= rcIn.Width()/2;
		rcIn.right -= ::MulDiv( rcIn.Width(), nPercentToOccupy, 100 );
		rcOut.left = rcIn.right;
		break;
#ifdef _DEBUG
	default:
		{
			ASSERT( FALSE );
		}
		break;
#endif // _DEBUG
	} // switch( nHalfSplitID )

DWORD dwDynStyle =
		GetStyle()
		| (GetBarStyle() & CBRS_ALIGN_ANY)
		;
	dwDynStyle |= CBRS_BORDER_ANY;
	dwDynStyle &= ~( CBRS_FLOATING | CBRS_GRIPPER );
	dwDynStyle |= CBRS_TOOLTIPS;

CExtDynControlBar * pDynBar = new CExtDynControlBar;
	pDynBar->SetInitDesiredSizeHorizontal( m_sizeDockedH );
	pDynBar->SetInitDesiredSizeVertical( m_sizeDockedV );
	if(	! pDynBar->Create(
			NULL,
			m_pDockSite,
			nNewID,
			dwDynStyle
			)
		)
	{
		ASSERT( FALSE );
		return;
	}
	ASSERT( pDynBar->m_pWndDynDocker != NULL );
	ASSERT( pDynBar->m_pWndDynDocker->GetSafeHwnd() != NULL );
CFrameWnd * pDockSite = m_pDockSite;
	pDynBar->m_pDockSite = m_pDockSite;
	pDynBar->m_pDockBar = m_pDockBar;
	pDynBar->EnableDocking( CBRS_ALIGN_ANY );
	ASSERT( pDynBar->m_pDockContext != NULL );
	ASSERT( pDynBar->m_pDockContext->m_pDockSite == m_pDockSite );

#ifdef _DEBUG
	if( pDockBarOut != NULL )
	{
		INT nPosOut = pDockBarOut->FindBar( pExtBar );
		ASSERT( nPosOut > 0 && nPosOut < pDockBarOut->m_arrBars.GetSize() );
	}
#endif // _DEBUG

INT nPosIn = pDockBarIn->FindBar( this );
	ASSERT( nPosIn > 0 && nPosIn < pDockBarIn->m_arrBars.GetSize() );
	pDockBarIn->m_arrBars[ nPosIn ] = pDynBar;

	pDynBar->m_pDockBar->m_bFloating = m_pDockBar->m_bFloating;
	m_pDockBar = pDynBar->m_pWndDynDocker;
	pExtBar->m_pDockBar = pDynBar->m_pWndDynDocker;

	m_pDockBar->m_bFloating = FALSE;
	
	if( pDynBar->m_pDockBar->m_bFloating )
		pDynBar->m_pDockBar->SetBarStyle(
			pDynBar->m_pDockBar->GetBarStyle()|CBRS_FLOATING
			);
	m_pDockBar->SetBarStyle(
		m_pDockBar->GetBarStyle() & (~CBRS_FLOATING)
		);

	ASSERT( !m_bUpdatingChain );
	ASSERT( !((CExtControlBar *)pDynBar)->m_bUpdatingChain );
	ASSERT( !pExtBar->m_bUpdatingChain );
	m_bUpdatingChain = true;
	((CExtControlBar *)pDynBar)->m_bUpdatingChain = true;
	pExtBar->m_bUpdatingChain = true;

	SetBarStyle( GetBarStyle() & (~CBRS_FLOATING) );
	pExtBar->SetBarStyle( pExtBar->GetBarStyle() & (~CBRS_FLOATING) );
	
	pDynBar->SetParent( pDockBarIn );
	SetParent( pDynBar->m_pWndDynDocker );
	pExtBar->SetParent( pDynBar->m_pWndDynDocker );

	if( pDockBarOut != NULL )
	{
		ASSERT( pDockBarOut->FindBar( pExtBar ) >= 0 );

		if( !pDockBarOut->m_bFloating
			&& pDockBarOut->IsKindOf(RUNTIME_CLASS(CExtDockBar))
			)
		{
			VERIFY(
				((CExtDockBar *)pDockBarOut)->
					RemoveControlBar( pExtBar, -1, 0, false )
				);
		}
		else
		{
			VERIFY( pDockBarOut->RemoveControlBar(pExtBar) );
		}

	}

	if(		nHalfSplitID == AFX_IDW_DOCKBAR_TOP
		||	nHalfSplitID == AFX_IDW_DOCKBAR_LEFT
		)
	{
		pDynBar->m_pWndDynDocker->m_arrBars.Add( pExtBar );
		pDynBar->m_pWndDynDocker->m_arrBars.Add( this );
	}
	else
	{
		pDynBar->m_pWndDynDocker->m_arrBars.Add( this );
		pDynBar->m_pWndDynDocker->m_arrBars.Add( pExtBar );
	}
	pDynBar->m_pWndDynDocker->m_arrBars.Add( NULL );

	SetInitDesiredSizeHorizontal( rcIn.Size() );
	SetInitDesiredSizeVertical( rcIn.Size() );
	pExtBar->SetInitDesiredSizeHorizontal( rcOut.Size() );
	pExtBar->SetInitDesiredSizeVertical( rcOut.Size() );

//	if( bOptimizeDockBarLayout
//		&& ::IsWindow( hWndDockBarOut )
//		)
//	{
//		ASSERT_VALID( pDockBarOut );
//		ASSERT(
//			CWnd::FromHandlePermanent(hWndDockBarOut)
//				== pDockBarOut
//			);
//		((CExtDockBar *)pDockBarOut)->OnDynamicLayoutOptimize();
//	}

	pDynBar->ScreenToClient( &rcIn );
	pDynBar->ScreenToClient( &rcOut );
	MoveWindow( &rcIn, FALSE );
	pExtBar->MoveWindow( &rcOut, FALSE );

	pDynBar->SetInitDesiredSizeFloating( rcDyn.Size() );
	pDynBar->MoveWindow( &rcDyn, FALSE );

	ASSERT_VALID( pDynBar );
	ASSERT_VALID( pDynBar->m_pWndDynDocker );

	m_bUpdatingChain = false;
	((CExtControlBar *)pDynBar)->m_bUpdatingChain = false;
	pExtBar->m_bUpdatingChain = false;

//pDynBar->m_pWndDynDocker->OnDynamicLayoutUpdate();
	CExtDockBar::_OptimizeCircles( m_pDockSite );

CFrameWnd * pFrame = pDynBar->GetParentFrame();
	if( pFrame != NULL )
	{
		ASSERT_VALID( pFrame );
		pFrame->RecalcLayout();
		pDynBar->OnRepositionSingleChild();
		pDynBar->OnNcAreaButtonsReinitialize();
		pDynBar->OnNcAreaButtonsReposition();
		ASSERT_VALID( pDynBar->m_pWndDynDocker );
		pDynBar->m_pWndDynDocker->OnDynamicLayoutUpdate();
	}
	stat_AdjustOccCompatibilityTree( pDockSite );
}

bool CExtControlBar::_DraggingApplyState(
	CExtControlBar::InternalDraggingState_t & _ds,
	bool bFinalApply
	)
{
#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
	CExtDynDockMarkerWnd::UnHighlightAllMarkers();
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)

	if( _ds.IsEmpty() )
		return false;

CExtControlBar * pExtBarSrc = _ds.ExtBarSrcGet();
	ASSERT_VALID( pExtBarSrc );

	if( pExtBarSrc->IsFixedDockStyle() )
		return false;

CFrameWnd * pFrame =
		pExtBarSrc->_GetDockingFrameImpl();
	ASSERT_VALID( pFrame );
CFrameWnd * pOldFrame =
		pExtBarSrc->GetParentFrame();
	ASSERT_VALID( pOldFrame );
HWND hWndOldFrame = pOldFrame->GetSafeHwnd();

CExtControlBar * pExtBarDst = _ds.ExtBarDstGet();
ExtControlBarVector_t vBarsDstRow;
INT nCountOfDstRowBars = 0;
UINT nDstDockBarID = AFX_IDW_DOCKBAR_FLOAT;
CDockBar * pDstDockBar = NULL;
bool bDstHorzDockBar = true;
bool bDstFloatBar = false;
INT nDstBarIdx = -1;
	if( pExtBarDst != NULL )
	{
		ASSERT_VALID( pExtBarDst );
		pExtBarDst->_GetRowExtBars( vBarsDstRow );
		nCountOfDstRowBars = (INT)vBarsDstRow.GetSize();
		ASSERT( nCountOfDstRowBars >= 1 );
		nDstDockBarID = pExtBarDst->GetSafeDockBarDlgCtrlID();
		ASSERT_DOCKBAR_DLGCTRLID( nDstDockBarID );
		ASSERT( pExtBarDst->m_pDockBar != NULL );
		pDstDockBar = pExtBarDst->m_pDockBar;
//			STATIC_DOWNCAST(
//				CExtDockBar,
//				pExtBarDst->m_pDockBar
//				);
		ASSERT( pDstDockBar != NULL );
		ASSERT_VALID( pDstDockBar );
		if( nDstDockBarID == AFX_IDW_DOCKBAR_LEFT
			|| nDstDockBarID == AFX_IDW_DOCKBAR_RIGHT
			)
			bDstHorzDockBar = false;
		if( nDstDockBarID == AFX_IDW_DOCKBAR_FLOAT )
			bDstFloatBar = true;
		nDstBarIdx = pDstDockBar->FindBar( pExtBarDst );
		ASSERT( nDstBarIdx >= 0 );
	} // if( pExtBarDst != NULL )

bool bInitialMode = false;
	if( pExtBarSrc->m_pDockBar == NULL )
		bInitialMode = true;

CRect rcTrack( _ds.TrackRectGet() );
e_calc_dock_type_t eCDT = _ds.GetCDT();

	if( bFinalApply )
	{
		switch( eCDT )
		{
		case __ECDT_2005_OUTER_TOP:
			VERIFY(
				pExtBarSrc->DockControlBarInnerOuter(
					AFX_IDW_DOCKBAR_TOP,
					false,
					NULL,
					true
					)
				);
			return true;
		case __ECDT_2005_OUTER_BOTTOM:
			VERIFY(
				pExtBarSrc->DockControlBarInnerOuter(
					AFX_IDW_DOCKBAR_BOTTOM,
					false,
					NULL,
					true
					)
				);
			return true;
		case __ECDT_2005_OUTER_LEFT:
			VERIFY(
				pExtBarSrc->DockControlBarInnerOuter(
					AFX_IDW_DOCKBAR_LEFT,
					false,
					NULL,
					true
					)
				);
			return true;
		case __ECDT_2005_OUTER_RIGHT:
			VERIFY(
				pExtBarSrc->DockControlBarInnerOuter(
					AFX_IDW_DOCKBAR_RIGHT,
					false,
					NULL,
					true
					)
				);
			return true;
		case __ECDT_2005_INNER_TOP:
			VERIFY(
				pExtBarSrc->DockControlBarInnerOuter(
					AFX_IDW_DOCKBAR_TOP,
					true,
					NULL,
					true
					)
				);
			return true;
		case __ECDT_2005_INNER_BOTTOM:
			VERIFY(
				pExtBarSrc->DockControlBarInnerOuter(
					AFX_IDW_DOCKBAR_BOTTOM,
					true,
					NULL,
					true
					)
				);
			return true;
		case __ECDT_2005_INNER_LEFT:
			VERIFY(
				pExtBarSrc->DockControlBarInnerOuter(
					AFX_IDW_DOCKBAR_LEFT,
					true,
					NULL,
					true
					)
				);
			return true;
		case __ECDT_2005_INNER_RIGHT:
			VERIFY(
				pExtBarSrc->DockControlBarInnerOuter(
					AFX_IDW_DOCKBAR_RIGHT,
					true,
					NULL,
					true
					)
				);
			return true;
		case __ECDT_2005_BAR_TOP:
			if( pExtBarDst == NULL )
				return true;
			VERIFY(
				pExtBarSrc->DockControlBarLTRB(
					pExtBarDst,
					AFX_IDW_DOCKBAR_TOP
					)
				);
			return true;
		case __ECDT_2005_BAR_BOTTOM:
			if( pExtBarDst == NULL )
				return true;
			VERIFY(
				pExtBarSrc->DockControlBarLTRB(
					pExtBarDst,
					AFX_IDW_DOCKBAR_BOTTOM
					)
				);
			return true;
		case __ECDT_2005_BAR_LEFT:
			if( pExtBarDst == NULL )
				return true;
			VERIFY(
				pExtBarSrc->DockControlBarLTRB(
					pExtBarDst,
					AFX_IDW_DOCKBAR_LEFT
					)
				);
			return true;
		case __ECDT_2005_BAR_RIGHT:
			if( pExtBarDst == NULL )
				return true;
			VERIFY(
				pExtBarSrc->DockControlBarLTRB(
					pExtBarDst,
					AFX_IDW_DOCKBAR_RIGHT
					)
				);
			return true;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		case __ECDT_2005_BAR_NEW_TAB:
			if( pExtBarDst == NULL )
				return true;
			ASSERT_VALID( pExtBarDst );
			ASSERT_VALID( pDstDockBar );
			VERIFY(
				pExtBarDst->DockControlBarIntoTabbedContainer(
					pExtBarSrc,
					0, //_ds.ReindexPosGet(), //-1,
					NULL,
					true
					)
				);
			return true;
#endif // else from  (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		} // switch( eCDT )
	} // if( bFinalApply )

	switch( eCDT )
	{
	case __ECDT_OWN_AREA:
		m_pDockSite->DelayRecalcLayout();
		return true;
	break;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	case __ECDT_TABBED_NEW:
	case __ECDT_TABBED_UPDATE:
	case __ECDT_TABBED_REINDEX:
	case __ECDT_TABBED_REINDEX_NHM:
	{
		if( pExtBarDst == NULL )
			return TRUE;
		ASSERT_VALID( pExtBarDst );
		ASSERT_VALID( pDstDockBar );
		VERIFY(
			pExtBarDst->DockControlBarIntoTabbedContainer(
				pExtBarSrc,
				_ds.ReindexPosGet(), //-1,
				NULL,
				true
				)
			);
		return true;
	}
	break;
	case __ECDT_2005_BAR_NEW_TAB:
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	case __ECDT_2005_OUTER_TOP:
	case __ECDT_2005_OUTER_BOTTOM:
	case __ECDT_2005_OUTER_LEFT:
	case __ECDT_2005_OUTER_RIGHT:
	case __ECDT_2005_INNER_TOP:
	case __ECDT_2005_INNER_BOTTOM:
	case __ECDT_2005_INNER_LEFT:
	case __ECDT_2005_INNER_RIGHT:
	case __ECDT_2005_BAR_TOP:
	case __ECDT_2005_BAR_BOTTOM:
	case __ECDT_2005_BAR_LEFT:
	case __ECDT_2005_BAR_RIGHT:
		{
#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
			CPoint ptCursor;
			if( ::GetCursorPos( &ptCursor ) )
			{
				HWND hWnd = ::WindowFromPoint( ptCursor );
				if( hWnd != NULL )
				{
					CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
					if( pWnd != NULL )
					{
						CExtDynDockMarkerWnd * pDynDockMarkerWnd =
							DYNAMIC_DOWNCAST( CExtDynDockMarkerWnd, pWnd );
						if(		pDynDockMarkerWnd != NULL
							&&	pDynDockMarkerWnd->IsStateApplyable()
							)
						{ // if dock marker is state apply-able
							eCDT = pDynDockMarkerWnd->GetCDT();
							if( _ds.GetCDT() == eCDT )
							{
								pDynDockMarkerWnd->HighlightMarkerTarget();
								return true;
							} // if( _ds.GetCDT() == eCDT )
						} // if dock marker is state apply-able
					} // if( pWnd != NULL )
				} // if( hWnd != NULL )
			} // if( ::GetCursorPos( &ptCursor ) )
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)
		}
		return true;

	case __ECDT_FLOATED:
	{
		bool bWasFloating = true;
		if( ! pExtBarSrc->IsFloating() )
		{
			bWasFloating = false;
			CExtControlBar * pUpSizeBar = (CExtControlBar *)
				pExtBarSrc->_GetNextControlBarInRow(
					pExtBarSrc, true, true, true
					);
			if( pUpSizeBar != NULL )
			{
				while( !pUpSizeBar->IsVisible() )
				{
					CExtControlBar * pUpSizeBar2 = (CExtControlBar *)
						_GetNextControlBarInRow(
							pUpSizeBar, true, true, true
							);
					if( pUpSizeBar2 == NULL )
						break;
					pUpSizeBar = pUpSizeBar2;
				}
			} // if( pUpSizeBar != NULL )
			if( pUpSizeBar == NULL )
			{
				pUpSizeBar = (CExtControlBar *)
					pExtBarSrc->_GetNextControlBarInRow(
						pExtBarSrc, false, true, true
						);
				if( pUpSizeBar != NULL )
				{
					while( !pUpSizeBar->IsVisible() )
					{
						CExtControlBar * pUpSizeBar2 = (CExtControlBar *)
							_GetNextControlBarInRow(
								pUpSizeBar, false, true, true
								);
						if( pUpSizeBar2 == NULL )
							break;
						pUpSizeBar = pUpSizeBar2;
					}
				} // if( pUpSizeBar != NULL )
			} // if( pUpSizeBar == NULL )
			if( pUpSizeBar != NULL )
			{
				CRect rcBarWnd;
				pExtBarSrc->GetWindowRect( &rcBarWnd );
				ASSERT( !rcBarWnd.IsRectEmpty() );
				bool bHorzUpsizing = pExtBarSrc->IsDockedHorizontally();
				LONG nUpSize = bHorzUpsizing
					? rcBarWnd.Width()
					: rcBarWnd.Height()
					;
				LONG & ref_nMetric = bHorzUpsizing
					? pUpSizeBar->m_sizeDockedH.cx
					: pUpSizeBar->m_sizeDockedV.cy;
				ref_nMetric += nUpSize;
			} // if( pUpSizeBar != NULL )
		} // if( ! pExtBarSrc->IsFloating() )
		CRect rcFrame;
		pFrame->GetWindowRect( &rcFrame );
		CPoint ptFloat( -rcFrame.TopLeft() );
		ptFloat += rcTrack.TopLeft();
		pExtBarSrc->FloatControlBar( ptFloat );
//		if( !bWasFloating && !bInitialMode )
//			pFrame->RecalcLayout();

		ASSERT( m_pDockContext != NULL );
		m_pDockContext->m_ptMRUFloatPos = ptFloat;

		if( pExtBarSrc->_IsShowContentWhenDragging() )
		{
			/*pExtBarSrc->RedrawWindow(
				NULL, NULL,
				RDW_INVALIDATE | RDW_UPDATENOW
					| RDW_ERASE | RDW_ERASENOW
					| RDW_ALLCHILDREN
				);*/
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			ASSERT_VALID( pExtBarSrc->m_pDockSite );
			CExtDynAutoHideArea
				* pAreaTop = NULL,
				* pAreaBottom = NULL,
				* pAreaLeft = NULL,
				* pAreaRight = NULL;
			CExtDynAutoHideArea::stat_FindAreas(
				pExtBarSrc->m_pDockSite,
				&pAreaTop,
				&pAreaBottom,
				&pAreaLeft,
				&pAreaRight
				);
			if( pAreaTop != NULL )
				pAreaTop->UpdateTabWnd();
			if( pAreaBottom != NULL )
				pAreaBottom->UpdateTabWnd();
			if( pAreaLeft != NULL )
				pAreaLeft->UpdateTabWnd();
			if( pAreaRight != NULL )
				pAreaRight->UpdateTabWnd();
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		}
		else
		{
			ASSERT_VALID( pExtBarSrc->m_pDockSite );
			pExtBarSrc->m_pDockSite->RecalcLayout();
		}
		return true;
	} // case __ECDT_FLOATED
	case __ECDT_INNER_TOP:
	{
		if( pExtBarDst == NULL )
			return TRUE;
		ASSERT_VALID( pExtBarDst );
/*
		ASSERT_VALID( pDstDockBar );
		ASSERT( nDstBarIdx >= 0 );
		if( bDstHorzDockBar || bDstFloatBar
			|| _ds.IsDeepHalfSplit()
			)
		{ // if full row or deep half split
			if( _ds.IsDeepHalfSplit() )
			{
				pExtBarDst->_DraggingDoDeepHalfSplit(
					pExtBarSrc,
					AFX_IDW_DOCKBAR_TOP
					);
				if( !bInitialMode )
				{
					pFrame->RecalcLayout();
					if( pOldFrame != pFrame
						&& ::IsWindow( hWndOldFrame )
						)
					{
						ASSERT_VALID( pOldFrame );
						__RECALC_FRAME_STEPS( pOldFrame );
					}
				}
			} // if( _ds.IsDeepHalfSplit() )
			else
			{
				for( INT nBarIdx = 0; nBarIdx < nCountOfDstRowBars; nBarIdx++ )
				{
					CExtControlBar * pExtBar = vBarsDstRow[nBarIdx];
					ASSERT_VALID( pExtBar );
					if( pExtBar == pExtBarSrc )
						continue;
					if( pExtBar->IsFixedMode() )
						continue;
					pExtBar->m_sizeDockedH.cy = rcTrack.Height();
				} // for( INT nBarIdx = 0; nBarIdx < nCountOfDstRowBars; nBarIdx++ )
				pExtBarSrc->m_sizeDockedH.cx = rcTrack.Width();
				pExtBarSrc->m_sizeDockedH.cy = rcTrack.Height();
				ASSERT_KINDOF( CExtDockBar, pDstDockBar );
				((CExtDockBar *)pDstDockBar)->
					_NewRowDockControlBar(
						pExtBarDst,
						pExtBarSrc,
						true
						);
				if( !bInitialMode )
				{
					pFrame->RecalcLayout();
					if( pOldFrame != pFrame
						&& ::IsWindow( hWndOldFrame )
						)
					{
						ASSERT_VALID( pOldFrame );
						__RECALC_FRAME_STEPS( pOldFrame );
					}
				}
			} // else from if( _ds.IsDeepHalfSplit() )
		} // if full row or deep half split
		else
		{ // if sizing bar half split
			pExtBarDst->m_sizeDockedV.cy = rcTrack.Height();
			pExtBarSrc->m_sizeDockedV.cy = rcTrack.Height();
			pExtBarSrc->m_sizeDockedV.cx = pExtBarDst->m_sizeDockedV.cx;
			ASSERT_KINDOF( CExtDockBar, pDstDockBar );
			((CExtDockBar *)pDstDockBar)->DockControlBar( pExtBarSrc, &rcTrack );
			ASSERT_VALID( pDstDockBar );
			if( !bInitialMode )
			{
				pFrame->RecalcLayout();
				if( pOldFrame != pFrame
					&& ::IsWindow( hWndOldFrame )
					)
				{
					ASSERT_VALID( pOldFrame );
					__RECALC_FRAME_STEPS( pOldFrame );
				}
				pExtBarSrc->SetWindowPos(NULL, 0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE
						|SWP_FRAMECHANGED
					);
			}
		} // if sizing bar half split
		return true;
*/
		pExtBarSrc->DockControlBarLTRB(
			pExtBarDst,
			AFX_IDW_DOCKBAR_TOP
			);
		return true;
	} // case __ECDT_INNER_TOP
	case __ECDT_INNER_BOTTOM:
	{
		if( pExtBarDst == NULL )
			return TRUE;
		ASSERT_VALID( pExtBarDst );
/*
		ASSERT_VALID( pDstDockBar );
		ASSERT( nDstBarIdx >= 0 );
		if( bDstHorzDockBar || bDstFloatBar
			|| _ds.IsDeepHalfSplit()
			)
		{ // if full row or deep half split
			if( _ds.IsDeepHalfSplit() )
			{
				pExtBarDst->_DraggingDoDeepHalfSplit(
					pExtBarSrc,
					AFX_IDW_DOCKBAR_BOTTOM
					);
				if( !bInitialMode )
				{
					pFrame->RecalcLayout();
					if( pOldFrame != pFrame
						&& ::IsWindow( hWndOldFrame )
						)
					{
						ASSERT_VALID( pOldFrame );
						__RECALC_FRAME_STEPS( pOldFrame );
					}
				}
			} // if( _ds.IsDeepHalfSplit() )
			else
			{
				for( INT nBarIdx = 0; nBarIdx < nCountOfDstRowBars; nBarIdx++ )
				{
					CExtControlBar * pExtBar = vBarsDstRow[nBarIdx];
					ASSERT_VALID( pExtBar );
					if( pExtBar == pExtBarSrc )
						continue;
					if( pExtBar->IsFixedMode() )
						continue;
					pExtBar->m_sizeDockedH.cy = rcTrack.Height();
				} // for( INT nBarIdx = 0; nBarIdx < nCountOfDstRowBars; nBarIdx++ )
				pExtBarSrc->m_sizeDockedH.cx = rcTrack.Width();
				pExtBarSrc->m_sizeDockedH.cy = rcTrack.Height();
				ASSERT_KINDOF( CExtDockBar, pDstDockBar );
				((CExtDockBar *)pDstDockBar)->
					_NewRowDockControlBar(
						pExtBarDst,
						pExtBarSrc,
						false
						);
				if( !bInitialMode )
				{
					pFrame->RecalcLayout();
					if( pOldFrame != pFrame
						&& ::IsWindow( hWndOldFrame )
						)
					{
						ASSERT_VALID( pOldFrame );
						__RECALC_FRAME_STEPS( pOldFrame );
					}
				}
			} // else from if( _ds.IsDeepHalfSplit() )
		} // if full row or deep half split
		else
		{ // if sizing bar half split
			pExtBarDst->m_sizeDockedV.cy = rcTrack.Height();
			pExtBarSrc->m_sizeDockedV.cy = rcTrack.Height();
			pExtBarSrc->m_sizeDockedV.cx = pExtBarDst->m_sizeDockedV.cx;
			ASSERT_KINDOF( CExtDockBar, pDstDockBar );
			((CExtDockBar *)pDstDockBar)->DockControlBar( pExtBarSrc, &rcTrack );
			ASSERT_VALID( pDstDockBar );
			if( !bInitialMode )
			{
				pFrame->RecalcLayout();
				if( pOldFrame != pFrame
					&& ::IsWindow( hWndOldFrame )
					)
				{
					ASSERT_VALID( pOldFrame );
					__RECALC_FRAME_STEPS( pOldFrame );
				}
				pExtBarSrc->SetWindowPos(NULL, 0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE
						|SWP_FRAMECHANGED
					);
			}
		} // if sizing bar half split
		return true;
*/
		pExtBarSrc->DockControlBarLTRB(
			pExtBarDst,
			AFX_IDW_DOCKBAR_BOTTOM
			);
		return true;
	} // case __ECDT_INNER_BOTTOM
	case __ECDT_INNER_LEFT:
	{
		if( pExtBarDst == NULL )
			return TRUE;
		ASSERT_VALID( pExtBarDst );
/*
		ASSERT_VALID( pDstDockBar );
		ASSERT( nDstBarIdx >= 0 );
		if( (!bDstHorzDockBar) || bDstFloatBar
			|| _ds.IsDeepHalfSplit()
			)
		{ // if full row or deep half split
			if( _ds.IsDeepHalfSplit() )
			{
				pExtBarDst->_DraggingDoDeepHalfSplit(
					pExtBarSrc,
					AFX_IDW_DOCKBAR_LEFT
					);
				if( !bInitialMode )
				{
					pFrame->RecalcLayout();
					if( pOldFrame != pFrame
						&& ::IsWindow( hWndOldFrame )
						)
					{
						ASSERT_VALID( pOldFrame );
						__RECALC_FRAME_STEPS( pOldFrame );
					}
				}
			} // if( _ds.IsDeepHalfSplit() )
			else
			{
				for( INT nBarIdx = 0; nBarIdx < nCountOfDstRowBars; nBarIdx++ )
				{
					CExtControlBar * pExtBar = vBarsDstRow[nBarIdx];
					ASSERT_VALID( pExtBar );
					if( pExtBar == pExtBarSrc )
						continue;
					if( pExtBar->IsFixedMode() )
						continue;
					pExtBar->m_sizeDockedV.cx = rcTrack.Width();
//if( pExtBar->IsKindOf(RUNTIME_CLASS(CExtDynTabControlBar)) )
//	((CExtDockDynTabBar*)((CExtDynTabControlBar*)pExtBar)->m_pWndDynDocker)->_SyncTabbedChilds( true, false );
				} // for( INT nBarIdx = 0; nBarIdx < nCountOfDstRowBars; nBarIdx++ )
				pExtBarSrc->m_sizeDockedV.cx = rcTrack.Width();
				pExtBarSrc->m_sizeDockedV.cy = rcTrack.Height();
				ASSERT_KINDOF( CExtDockBar, pDstDockBar );
				((CExtDockBar *)pDstDockBar)->
					_NewRowDockControlBar(
						pExtBarDst,
						pExtBarSrc,
						true
						);
				if( !bInitialMode )
				{
					pFrame->RecalcLayout();
					if( pOldFrame != pFrame
						&& ::IsWindow( hWndOldFrame )
						)
					{
						ASSERT_VALID( pOldFrame );
						__RECALC_FRAME_STEPS( pOldFrame );
					}
				}
			} // else from if( _ds.IsDeepHalfSplit() )
		} // if full row or deep half split
		else
		{ // if sizing bar half split
			pExtBarDst->m_sizeDockedH.cx = rcTrack.Width();
			pExtBarSrc->m_sizeDockedH.cx = rcTrack.Width();
			pExtBarSrc->m_sizeDockedH.cy = pExtBarDst->m_sizeDockedH.cy;
			ASSERT_KINDOF( CExtDockBar, pDstDockBar );
			((CExtDockBar *)pDstDockBar)->DockControlBar( pExtBarSrc, &rcTrack );
			ASSERT_VALID( pDstDockBar );
			if( !bInitialMode )
			{
				pFrame->RecalcLayout();
				if( pOldFrame != pFrame
					&& ::IsWindow( hWndOldFrame )
					)
				{
					ASSERT_VALID( pOldFrame );
					__RECALC_FRAME_STEPS( pOldFrame );
				}
				pExtBarSrc->SetWindowPos(NULL, 0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE
						|SWP_FRAMECHANGED
					);
			}
		} // if sizing bar half split
		return true;
*/
		pExtBarSrc->DockControlBarLTRB(
			pExtBarDst,
			AFX_IDW_DOCKBAR_LEFT
			);
		return true;
	} // case __ECDT_INNER_LEFT
	case __ECDT_INNER_RIGHT:
	{
		if( pExtBarDst == NULL )
			return TRUE;
		ASSERT_VALID( pExtBarDst );
/*
		ASSERT_VALID( pDstDockBar );
		ASSERT( nDstBarIdx >= 0 );
		if( (!bDstHorzDockBar) || bDstFloatBar
			|| _ds.IsDeepHalfSplit()
			)
		{ // if full row or deep half split
			if( _ds.IsDeepHalfSplit() )
			{
				pExtBarDst->_DraggingDoDeepHalfSplit(
					pExtBarSrc,
					AFX_IDW_DOCKBAR_RIGHT
					);
				if( !bInitialMode )
				{
					pFrame->RecalcLayout();
					if( pOldFrame != pFrame
						&& ::IsWindow( hWndOldFrame )
						)
					{
						ASSERT_VALID( pOldFrame );
						__RECALC_FRAME_STEPS( pOldFrame );
					}
				}
			} // if( _ds.IsDeepHalfSplit() )
			else
			{
				for( INT nBarIdx = 0; nBarIdx < nCountOfDstRowBars; nBarIdx++ )
				{
					CExtControlBar * pExtBar = vBarsDstRow[nBarIdx];
					ASSERT_VALID( pExtBar );
					if( pExtBar == pExtBarSrc )
						continue;
					if( pExtBar->IsFixedMode() )
						continue;
					pExtBar->m_sizeDockedV.cx = rcTrack.Width();
				} // for( INT nBarIdx = 0; nBarIdx < nCountOfDstRowBars; nBarIdx++ )
				pExtBarSrc->m_sizeDockedV.cx = rcTrack.Width();
				pExtBarSrc->m_sizeDockedV.cy = rcTrack.Height();
				ASSERT_KINDOF( CExtDockBar, pDstDockBar );
				((CExtDockBar *)pDstDockBar)->
					_NewRowDockControlBar(
						pExtBarDst,
						pExtBarSrc,
						false
						);
				if( !bInitialMode )
				{
					pFrame->RecalcLayout();
					if( pOldFrame != pFrame
						&& ::IsWindow( hWndOldFrame )
						)
					{
						ASSERT_VALID( pOldFrame );
						__RECALC_FRAME_STEPS( pOldFrame );
					}
				}
			} // else from if( _ds.IsDeepHalfSplit() )
		} // if full row or deep half split
		else
		{ // if sizing bar half split
			pExtBarDst->m_sizeDockedH.cx = rcTrack.Width();
			pExtBarSrc->m_sizeDockedH.cx = rcTrack.Width();
			pExtBarSrc->m_sizeDockedH.cy = pExtBarDst->m_sizeDockedH.cy;
			ASSERT_KINDOF( CExtDockBar, pDstDockBar );
			((CExtDockBar *)pDstDockBar)->DockControlBar( pExtBarSrc, &rcTrack );
			ASSERT_VALID( pDstDockBar );
			if( !bInitialMode )
			{
				pFrame->RecalcLayout();
				if( pOldFrame != pFrame
					&& ::IsWindow( hWndOldFrame )
					)
				{
					ASSERT_VALID( pOldFrame );
					__RECALC_FRAME_STEPS( pOldFrame );
				}
				pExtBarSrc->SetWindowPos(NULL, 0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE
						|SWP_FRAMECHANGED
					);
			}
		} // if sizing bar half split
		return true;
*/
		pExtBarSrc->DockControlBarLTRB(
			pExtBarDst,
			AFX_IDW_DOCKBAR_RIGHT
			);
		return true;
	} // case __ECDT_INNER_RIGHT
	case __ECDT_DOCKBAR_INNER_TOP:
	case __ECDT_DOCKBAR_INNER_BOTTOM:
	case __ECDT_DOCKBAR_INNER_LEFT:
	case __ECDT_DOCKBAR_INNER_RIGHT:
		{
			ASSERT_VALID( pExtBarSrc );

//			pExtBarSrc->m_sizeDockedV.cy = MAXSHORT;

			if(		eCDT == __ECDT_DOCKBAR_INNER_TOP
				||	eCDT == __ECDT_DOCKBAR_INNER_BOTTOM
				)
				pExtBarSrc->m_sizeDockedH.cy = rcTrack.Height();
			else
				pExtBarSrc->m_sizeDockedV.cx = rcTrack.Width();

			UINT nDockBarID = AFX_IDW_DOCKBAR_TOP;
			if( eCDT == __ECDT_DOCKBAR_INNER_BOTTOM )
				nDockBarID = AFX_IDW_DOCKBAR_BOTTOM;
			else if( eCDT == __ECDT_DOCKBAR_INNER_LEFT )
				nDockBarID = AFX_IDW_DOCKBAR_LEFT;
			else if( eCDT == __ECDT_DOCKBAR_INNER_RIGHT )
				nDockBarID = AFX_IDW_DOCKBAR_RIGHT;
			CExtDockBar * pDockBar = (CExtDockBar *)
				pFrame->GetControlBar( nDockBarID );
			ASSERT_VALID( pDockBar );
			ASSERT_KINDOF( CExtDockBar, pDockBar );
			ASSERT( pDockBar->_GetCircleNo() == 0 );
			UINT nCircleNoRequired = _ds.GetCircleNo();
			if( nCircleNoRequired > 0 )
			{
				pDockBar = pDockBar->_GetBarByCircleNo( nCircleNoRequired );
				ASSERT_VALID( pDockBar );
				ASSERT_KINDOF( CExtDockBar, pDockBar );
				ASSERT( pDockBar->_GetCircleNo() == nCircleNoRequired );
				ASSERT( ((UINT)pDockBar->GetDlgCtrlID()) == nDockBarID );
			}
		
			HWND hWndSrc = pExtBarSrc->GetSafeHwnd();

			pDockBar->_InnerOuterDockControlBar( pExtBarSrc, true );
			ASSERT_VALID( pDockBar );
			if( !bInitialMode )
			{
				pFrame->RecalcLayout();
				if( pOldFrame != pFrame
					&& ::IsWindow( hWndOldFrame )
					)
				{
					ASSERT_VALID( pOldFrame );
					__RECALC_FRAME_STEPS( pOldFrame );
				}
			}

			if( ::IsWindow( hWndSrc ) )
			{
				ASSERT_VALID( pExtBarSrc );
				ASSERT(
					CWnd::FromHandlePermanent(hWndSrc)
						== pExtBarSrc
					);
				pExtBarSrc->SetWindowPos(
					NULL, 0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE
						|SWP_FRAMECHANGED
					);
			}

			return true;
		}
	case __ECDT_CIRCLE1_OUTER_TOP:
	case __ECDT_CIRCLE1_OUTER_BOTTOM:
	case __ECDT_CIRCLE1_OUTER_LEFT:
	case __ECDT_CIRCLE1_OUTER_RIGHT:
		{
			UINT nDockBarID = 0;
			bool bInjectCircle = false;
			switch( eCDT )
			{
			case __ECDT_CIRCLE1_OUTER_TOP:
				nDockBarID = AFX_IDW_DOCKBAR_TOP;
				break;
			case __ECDT_CIRCLE1_OUTER_BOTTOM:
				nDockBarID = AFX_IDW_DOCKBAR_BOTTOM;
				break;
			case __ECDT_CIRCLE1_OUTER_LEFT:
				nDockBarID = AFX_IDW_DOCKBAR_LEFT;
				bInjectCircle = true;
				break;
			case __ECDT_CIRCLE1_OUTER_RIGHT:
				nDockBarID = AFX_IDW_DOCKBAR_RIGHT;
				bInjectCircle = true;
				break;
#ifdef _DEBUG
			default:
				{
					ASSERT( FALSE );
				}
				break;
#endif // _DEBUG
			} // switch( eCDT )

			CExtDockBar * pDockBar = (CExtDockBar *)
				pFrame->GetControlBar( nDockBarID );
			ASSERT_VALID( pDockBar );
			ASSERT_KINDOF( CExtDockBar, pDockBar );
			ASSERT( pDockBar->_GetCircleNo() == 0 );
			if( bInjectCircle )
				pDockBar->_InjectInnerCircle();
			pDockBar = pDockBar->_GetBarByCircleNo( 1 );
			ASSERT_VALID( pDockBar );
			ASSERT_KINDOF( CExtDockBar, pDockBar );
			ASSERT( pDockBar->_GetCircleNo() == 1 );

			HWND hWndSrc = pExtBarSrc->GetSafeHwnd();
			
			pDockBar->_InnerOuterDockControlBar( pExtBarSrc, false );
			ASSERT_VALID( pDockBar );
			if( !bInitialMode )
			{
				pFrame->RecalcLayout();
				if( pOldFrame != pFrame
					&& ::IsWindow( hWndOldFrame )
					)
				{
					ASSERT_VALID( pOldFrame );
					__RECALC_FRAME_STEPS( pOldFrame );
				}
			}

			if( ::IsWindow( hWndSrc ) )
			{
				ASSERT_VALID( pExtBarSrc );
				ASSERT(
					CWnd::FromHandlePermanent(hWndSrc)
						== pExtBarSrc
					);
				pExtBarSrc->SetWindowPos(
					NULL, 0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE
						|SWP_FRAMECHANGED
					);
			}
			
		}
		return true; // case __ECDT_CIRCLE1_OUTER_....
	} // switch( eCDT )
	ASSERT( FALSE );
	return false;
}

UINT CExtControlBar::GetDockBarCircleNo() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pDockSite );

	ASSERT_VALID( m_pDockBar );
	ASSERT_KINDOF( CExtDockBar, m_pDockBar );
	
	return ((CExtDockBar *)m_pDockBar)->_GetCircleNo();
}

bool CExtControlBar::_CanDockToInnerCircles() const
{
	return true;
}

bool CExtControlBar::_CanDockToTabbedContainers(
	CExtControlBar * pDestBar
	) const
{
	ASSERT_VALID( this );
	pDestBar;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( _IsShowContentWhenDragging() )
	{
#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
		eResizablePanelDockingType_t eResizablePanelDockingType =
			OnQueryDockingType();
		if( INT(eResizablePanelDockingType) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005) )
			return true;
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)
		return false;
	} // if( _IsShowContentWhenDragging() )
	return true;
#else
	return false;
#endif // else from (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
}

bool CExtControlBar::_CanDockLTRB(
	CExtControlBar * pDestBar
	) const
{
	ASSERT_VALID( this );
	pDestBar;
	return true;
}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
CExtDynTabControlBar * CExtControlBar::_GetNearestTabbedContainer()
{
	ASSERT_VALID( this );
	if( m_hWnd == NULL || (! ::IsWindow(m_hWnd) ) )
		return NULL;
CWnd * pWndParent = GetParent();
	if( ! pWndParent->IsKindOf(RUNTIME_CLASS(CExtDockDynTabBar)) )
		return NULL;
CExtDynTabControlBar * pTabbedContainer = STATIC_DOWNCAST( CExtDynTabControlBar, pWndParent->GetParent() );
	return pTabbedContainer;
}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

CExtDynControlBar * CExtControlBar::_GetNearestDynamicContainer()
{
	ASSERT_VALID( this );
	if( m_hWnd == NULL || (! ::IsWindow(m_hWnd) ) )
		return NULL;
CWnd * pWndParent = GetParent();
	if( ! pWndParent->IsKindOf(RUNTIME_CLASS(CExtDockDynBar)) )
		return NULL;
CExtDynControlBar * pDynContainer = STATIC_DOWNCAST( CExtDynControlBar, pWndParent->GetParent() );
	return pDynContainer;
}

// toggle docking state
void CExtControlBar::ToggleDocking()
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pDockSite );

	ASSERT_VALID( m_pDockBar );
#ifdef _DEBUG
	if( !IsFloating() )
	{
		ASSERT_KINDOF( CExtDockBar, m_pDockBar );
	}
#endif // _DEBUG
	ASSERT( m_pDockContext != NULL );

	ASSERT( m_pDockContext != NULL );
	
	if( IsFixedDockStyle() )
	{
		m_pDockContext->ToggleDocking();
		CFrameWnd * pFrame = _GetDockingFrameImpl();
		ASSERT_VALID( pFrame );
		pFrame->DelayRecalcLayout();
		_RecalcNcArea();
		return;
	}

	ASSERT( !IsFixedMode() );
	
//	m_pDockContext->ToggleDocking();
//	
//CFrameWnd * pFrame = _GetDockingFrameImpl();
//	ASSERT_VALID( pFrame );
//	pFrame->RecalcLayout();

UINT nDockBarID = GetSafeDockBarDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID( nDockBarID );
	if( nDockBarID == AFX_IDW_DOCKBAR_FLOAT )
	{
	} // if( nDockBarID == AFX_IDW_DOCKBAR_FLOAT )
	else
	{
		ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );
		if( !GetParentFrame()->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
			FloatControlBar();
	} // else from if( nDockBarID == AFX_IDW_DOCKBAR_FLOAT )
}

CExtControlBar * CExtControlBar::FindTearOffBar(
	CFrameWnd * pDockSite,
	UINT nTearOffCmdID
	)
{
	if( pDockSite->GetSafeHwnd() == NULL || nTearOffCmdID == 0 )
		return NULL;
	ASSERT_VALID( pDockSite );
	ASSERT( CExtCmdManager::IsCommand( nTearOffCmdID ) );
POSITION pos = pDockSite->m_listControlBars.GetHeadPosition();
	for( ; pos != NULL; )
	{
		CControlBar * pControlBar = (CControlBar *)
			pDockSite->m_listControlBars.GetNext( pos );
		ASSERT_VALID( pControlBar );
		ASSERT_KINDOF( CControlBar, pControlBar );
		if( pControlBar->IsDockBar() )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pControlBar );
		if( pExtBar == NULL )
			continue;
		UINT nCmdID = pExtBar->TearOffCommandGet();
		if( nCmdID == nTearOffCmdID )
			return pExtBar;
	} // for( ; pos != NULL; )
	return NULL;
}

UINT CExtControlBar::TearOffCommandGet() const
{
	ASSERT_VALID( this );
#ifdef _DEBUG
	if( m_nTearOffCmdID != 0 )
	{
		ASSERT( CExtCmdManager::IsCommand( m_nTearOffCmdID ) );
	}
#endif // _DEBUG
	return m_nTearOffCmdID;
}

void CExtControlBar::TearOffCommandSet( UINT nCmdID )
{
	ASSERT_VALID( this );
	m_nTearOffCmdID = nCmdID;
#ifdef _DEBUG
	if( m_nTearOffCmdID != 0 )
	{
		ASSERT( CExtCmdManager::IsCommand( m_nTearOffCmdID ) );
	}
#endif // _DEBUG
}

CExtMiniDockFrameWnd * CExtControlBar::OnCreateFloatingFrame(
	DWORD dwStyle
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pDockSite );
CExtMiniDockFrameWnd * pMiniFrame =
		new CExtMiniDockFrameWnd;
	if( ! pMiniFrame->Create(
			m_pDockSite,
			dwStyle
			)
		)
	{
		ASSERT( FALSE );
		AfxThrowResourceException();
	}
	return pMiniFrame;
}

// float control bar into recent floated position
void CExtControlBar::FloatControlBar(
	DWORD dwStyle // = CBRS_ALIGN_TOP
	)
{
	FloatControlBar( m_ptFloatHelper, dwStyle );
}

// float control bar
void CExtControlBar::FloatControlBar(
	CPoint ptFloat,
	DWORD dwStyle // = CBRS_ALIGN_TOP
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pDockSite );

	g_pBarToggleFloatingState = this;

#ifdef _DEBUG
bool bWasFloating = IsFloating();
#endif // _DEBUG
HWND hWndTopDynBar = NULL;
CExtDynControlBar * pWndTopDynBar = NULL;
	// if the bar is already floating and the dock bar only contains this
	// bar and same orientation then move the window rather than recreating
	// the frame
	if( m_pDockBar != NULL )
	{
		CDockBar * pDockBar = m_pDockBar;
		ASSERT_KINDOF( CDockBar, pDockBar );

		CExtDynControlBar * pDynBar = DYNAMIC_DOWNCAST( CExtDynControlBar, pDockBar->GetParent() );
		for( ; pDynBar != NULL; )
		{
			hWndTopDynBar = pDynBar->m_hWnd;
			pWndTopDynBar = pDynBar;
			CWnd * pWndWalk = pDynBar->GetParent();
			if( pWndWalk == NULL )
				break;
			CDockBar * pParentDockBar = DYNAMIC_DOWNCAST( CDockBar, pWndWalk );
			if( pParentDockBar == NULL )
				break;
			pWndWalk = pWndWalk->GetParent();
			if( pWndWalk == NULL )
				break;
			pDynBar = DYNAMIC_DOWNCAST( CExtDynControlBar, pWndWalk );
		}

		if(		pDockBar->m_bFloating
			&&	pDockBar->GetDockedCount() == 1
//			&&	(dwStyle & pDockBar->m_dwStyle & CBRS_ALIGN_ANY) != 0
			)
		{
			ASSERT( bWasFloating );
			CExtMiniDockFrameWnd * pDockFrame =
				(CExtMiniDockFrameWnd*) pDockBar->GetParent();
			ASSERT( pDockFrame != NULL );
			ASSERT_KINDOF( CExtMiniDockFrameWnd, pDockFrame );
			CRect rcWnd;
			GetWindowRect( &rcWnd );
			CRect rcCustomFloatPos( ptFloat, rcWnd.Size() );
			pDockFrame->SendMessage(
				CExtPopupMenuWnd::g_nMsgCustomFloatPos,
				WPARAM(pDockFrame->m_hWnd),
				LPARAM(&rcCustomFloatPos)
				);
			pDockFrame->SetWindowPos(
				NULL,
				ptFloat.x,
				ptFloat.y,
				0,
				0,
				SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE
				);
			// - 2.26
			//pDockFrame->RecalcLayout( TRUE );
			//pDockFrame->UpdateWindow();

			m_ptFloatHelper = ptFloat;
			
			g_pBarToggleFloatingState = NULL;
			return;
		}
	} // if( m_pDockBar != NULL )

	ASSERT(
		(!bWasFloating)
		|| m_pDockBar == NULL // initial mode
		);

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
CExtDockDynTabBar * pTabbedDocker =
		DYNAMIC_DOWNCAST(
			CExtDockDynTabBar,
			GetParent()
			);
	if( pTabbedDocker != NULL )
	{
		CExtDynTabControlBar * pTabbedBar =
			STATIC_DOWNCAST(
				CExtDynTabControlBar,
				pTabbedDocker->GetParent()
				);
//		LONG nSel = pTabbedBar->GetSwitcherSelection();
//		if(		nSel >= 0
//			&&	pTabbedBar->GetBarAt( nSel, true ) == this
//			)
//			pTabbedBar->RemoveSelFromSwitcher();
//		else
			pTabbedBar->RemoveControlBar( this );
	} // if( pTabbedDocker != NULL )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

bool bShowMiniFrame =
		( (GetStyle() & WS_VISIBLE) != 0 ) ? true : false;

	if( m_dwStyle & CBRS_SIZE_DYNAMIC )
	{
		dwStyle |= CBRS_SIZE_DYNAMIC;
		if (dwStyle & CBRS_ORIENT_VERT)
		{
			dwStyle &= ~CBRS_ALIGN_ANY;
			dwStyle |= CBRS_ALIGN_TOP;
		}
	}

CExtDockBar * pDockBarOut = (CExtDockBar *)m_pDockBar;
bool bOptimizeDockBarLayout = false;
	if(		pDockBarOut != NULL
		&&	(! pDockBarOut->m_bFloating )
		&&	pDockBarOut->IsKindOf(RUNTIME_CLASS(CExtDockBar))
		)
		bOptimizeDockBarLayout = true;
HWND hWndDockBarOuter =
		(pDockBarOut != NULL)
			? pDockBarOut->GetSafeHwnd()
			: NULL
			;

CFrameWnd * pOldFrame = GetParentFrame();
HWND hWndOldFrame = NULL;
	if( pOldFrame != NULL )
	{
		ASSERT_VALID( pOldFrame );
		hWndOldFrame = pOldFrame->GetSafeHwnd();
	}

CExtMiniDockFrameWnd * pDockFrame = OnCreateFloatingFrame( dwStyle );
	ASSERT( pDockFrame != NULL );
	ASSERT_KINDOF( CExtMiniDockFrameWnd, pDockFrame );
	pDockFrame->SetWindowPos(
		NULL, ptFloat.x, ptFloat.y, 0, 0,
		SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE
		);
	if( pDockFrame->m_hWndOwner == NULL )
		pDockFrame->m_hWndOwner = m_pDockSite->m_hWnd;

CDockBar * pDockBar = STATIC_DOWNCAST( CDockBar, pDockFrame->GetDlgItem( AFX_IDW_DOCKBAR_FLOAT ) );
	ASSERT_VALID( pDockBar );

	ASSERT( ! m_bUpdatingChain );
	m_bUpdatingChain = true;
	//pDockBar->m_bLockedOptimize = true;
	((CExtDockBar*)pDockBar)->DockControlBar( this, NULL, false, true );
	//pDockBar->m_bLockedOptimize = false;
	m_bUpdatingChain = false;
	pDockFrame->RecalcLayout( TRUE );
	if( bShowMiniFrame )
	{
		pDockFrame->ShowWindow( SW_SHOWNA );
		pDockFrame->UpdateWindow();
	}
	m_ptFloatHelper = ptFloat;
	if(		bOptimizeDockBarLayout
		&&	::IsWindow( hWndDockBarOuter )
		)
	{
		ASSERT_VALID( pDockBarOut );
		CExtMiniDockFrameWnd * pOldMiniFrame = DYNAMIC_DOWNCAST( CExtMiniDockFrameWnd, pDockBarOut->GetParentFrame() );
		HWND hWndOldMiniFrame = pOldMiniFrame->GetSafeHwnd();
		if( hWndOldMiniFrame != NULL )
			pOldMiniFrame->SetRedraw( FALSE );
		((CExtDockBar*)pDockBarOut)->OnDynamicLayoutOptimize();
		if( hWndOldMiniFrame != NULL && ::IsWindow(hWndOldMiniFrame) && LPVOID(pOldMiniFrame) == LPVOID(CWnd::FromHandlePermanent(hWndOldMiniFrame)) )
			pOldMiniFrame->SetRedraw( TRUE );
	}
	if( pOldFrame != m_pDockSite && ::IsWindow( hWndOldFrame ) )
	{
		ASSERT_VALID( pOldFrame );
		__RECALC_FRAME_STEPS( pOldFrame );
	}

	if( ! IsFixedDockStyle() )
		CExtDockBar::_RemovePlaceHolder( m_pDockSite, this );

	CExtDockBar::_OptimizeCircles( m_pDockSite );
	m_pDockSite->RecalcLayout();
	OnRepositionSingleChild();
	if( hWndTopDynBar != NULL && ::IsWindow( hWndTopDynBar ) && LPVOID(pWndTopDynBar) == LPVOID(CWnd::FromHandlePermanent(hWndTopDynBar)) )
		pWndTopDynBar->OnRepositionSingleChild();
	CExtControlBar::stat_AdjustOccCompatibilityTree( m_pDockSite );
	g_pBarToggleFloatingState = NULL;
}

// dock control bar to new circle (most inner or outer)
bool CExtControlBar::DockControlBarInnerOuter(
	UINT nDockBarID,
	bool bInner,
	CFrameWnd * pDockSite, // = NULL // can be NULL only if bar already was at once docked
	bool bRecalcLayout // = true
	)
{
	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtControlBar, this );
	if( pDockSite == NULL )
		pDockSite = m_pDockSite;
	ASSERT_VALID( pDockSite );
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );
CFrameWnd * pParentFrame = GetParentFrame();
	if(		pParentFrame != NULL
		&&	pParentFrame->IsKindOf( RUNTIME_CLASS(CExtMiniDockFrameWnd) )
		)
		pParentFrame->ShowWindow( SW_HIDE );
UINT nCircleNo = 1;
	CExtDockBar::_InjectCircle( pDockSite, nCircleNo );
	if( bInner )
		nCircleNo = CExtDockBar::_CreateInnerCircle( pDockSite );
	return
		DockControlBar(
			nDockBarID,
			nCircleNo,
			pDockSite,
			bRecalcLayout
			);
}

// dock control bar relatively to other control bar (extended version)
bool CExtControlBar::DockControlBarLTRB(
	CExtControlBar * pBarTarget,
	UINT nDockBarID, // = AFX_IDW_DOCKBAR_BOTTOM
	bool bRecalcLayout // = true
	)
{
	return DockControlBarLTRB( 50, pBarTarget, nDockBarID, bRecalcLayout );
}

// dock control bar relatively to other control bar using
// space percent specification (extended version)
bool CExtControlBar::DockControlBarLTRB(
	int nPercentToOccupy, // greater than zero and less than one hundred
	CExtControlBar * pBarTarget,
	UINT nDockBarID, // = AFX_IDW_DOCKBAR_BOTTOM
	bool bRecalcLayout // = true
	)
{
	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtControlBar, this );
	ASSERT_VALID( pBarTarget );
	ASSERT_KINDOF( CExtControlBar, pBarTarget );
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );

	ASSERT_VALID( pBarTarget->m_pDockBar );
	ASSERT_KINDOF( CDockBar, pBarTarget->m_pDockBar );
	ASSERT( ! IsFixedMode() );
	ASSERT( ! pBarTarget->IsFixedMode() );

	ASSERT( 0 < nPercentToOccupy && nPercentToOccupy < 100 );
#ifdef _DEBUG
	if( m_pDockBar != NULL )
	{
		ASSERT_VALID( m_pDockBar );
		ASSERT_KINDOF( CDockBar, m_pDockBar );
	}
#endif // _DEBUG

CFrameWnd * pParentFrame = GetParentFrame();
CFrameWnd * pDockSite = m_pDockSite;

#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
	if(		pParentFrame != NULL
		&&	pParentFrame->IsKindOf( RUNTIME_CLASS(CExtMiniDockFrameWnd) )
		&&	_DraggingGetBar() == this
		&&	INT(OnQueryDockingType()) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005)
		)
		pParentFrame->ShowWindow( SW_HIDE );
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
CExtDynTabControlBar * pTargetTabBar =
		pBarTarget->_GetNearestTabbedContainer();
	if( pTargetTabBar != NULL )
	{
		ASSERT_VALID( pTargetTabBar );
		pBarTarget = pTargetTabBar;
		ASSERT_VALID( pBarTarget->m_pDockBar );
		ASSERT_KINDOF( CDockBar, pBarTarget->m_pDockBar );
		ASSERT( ! pBarTarget->IsFixedMode() );
	} // if( pTargetTabBar != NULL )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

bool bRecalcDockSite = true;
	pParentFrame = pBarTarget->GetParentFrame();
	ASSERT_VALID( pParentFrame );
	if(		pParentFrame != pBarTarget->m_pDockSite
		&&	(m_pDockSite != NULL && pParentFrame != m_pDockSite )
		)
		bRecalcDockSite = false;
HWND hWndSrcFrame = NULL;
	if( m_pDockSite != NULL )
	{
		hWndSrcFrame = GetParentFrame()->GetSafeHwnd();
		if( hWndSrcFrame == pBarTarget->m_pDockSite->GetSafeHwnd() )
			hWndSrcFrame = NULL;
	} // if( m_pDockSite != NULL )

	pBarTarget->DelayShow( TRUE );
	DelayShow( TRUE );
HWND hWndBarTarget = pBarTarget->GetSafeHwnd();
	_DisplayingSet( false );
	pBarTarget->_DraggingDoDeepHalfSplit(
		this,
		nDockBarID,
		nPercentToOccupy
		);
	_DisplayingSet( false );
bRecalcLayout = bRecalcDockSite = true;
	if( bRecalcLayout )
	{
		ASSERT_VALID( pParentFrame );
		pParentFrame->RecalcLayout();
		if( hWndSrcFrame != NULL && ::IsWindow(hWndSrcFrame) )
		{
			CWnd * pWnd = CWnd::FromHandlePermanent( hWndSrcFrame );
			if( pWnd != NULL )
			{
				ASSERT_VALID( pWnd );
				CFrameWnd * pFrame =
					DYNAMIC_DOWNCAST( CFrameWnd, pWnd );
				if( pFrame != NULL )
					pFrame->RecalcLayout();
			} // if( pWnd != NULL )
		} // if( hWndSrcFrame != NULL && ::IsWindow(hWndSrcFrame) )
		if(		bRecalcDockSite
			&&	::IsWindow( hWndBarTarget )
			)
			pBarTarget->m_pDockSite->RecalcLayout();
	} // if( bRecalcLayout )

bool bMini = false;
CWnd * pWndTargetFrame = pBarTarget->GetParentFrame();
	if( pWndTargetFrame != NULL )
	{
		CExtMiniDockFrameWnd * pWndMiniFrame = DYNAMIC_DOWNCAST( CExtMiniDockFrameWnd, pWndTargetFrame );
		if( pWndMiniFrame != NULL )
		{
//			pWndMiniFrame->ModifyStyle( FWS_SNAPTOBARS, 0 );
			CWnd * pWnd = pWndMiniFrame->GetDlgItem( AFX_IDW_DOCKBAR_FLOAT );
			if( pWnd != NULL )
			{
				pWnd = pWnd->GetWindow( GW_CHILD );
				if( pWnd != NULL )
				{
					CExtDynControlBar * pDynControlBar = DYNAMIC_DOWNCAST( CExtDynControlBar, pWnd );
					if( pDynControlBar != NULL )
					{
						pDynControlBar->OnRepositionSingleChild();
						bMini = true;
					}
				}
			}
		}
	}
	if( ! bMini )
	{
		CWnd * pWnd = pBarTarget->GetParent();
		CExtDockBar * pExtDockBar = DYNAMIC_DOWNCAST( CExtDockBar, pWnd );
		if( pExtDockBar != NULL )
		{
			pWnd = pExtDockBar->GetParent();
			CExtDynControlBar * pDynControlBar = DYNAMIC_DOWNCAST( CExtDynControlBar, pWnd );
			if( pDynControlBar != NULL )
				pDynControlBar->OnRepositionSingleChild();
		}
	}

	CExtControlBar::stat_AdjustOccCompatibilityTree( pDockSite );

	return true;
}

// dock control bar to new row
bool CExtControlBar::DockControlBar(
	UINT nDockBarID,
	UINT nCircleNo,
	CFrameWnd * pDockSite, // = NULL // can be NULL only if bar already was at once docked
	bool bRecalcLayout // = true
	)
{
	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtControlBar, this );
	if( pDockSite == NULL )
		pDockSite = m_pDockSite;
	ASSERT_VALID( pDockSite );
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );
	if( bRecalcLayout && pDockSite != NULL )
		CExtDockBar::_OptimizeCircles( pDockSite );
CExtDockBar * pDockBar =  STATIC_DOWNCAST( CExtDockBar, pDockSite->GetControlBar( nDockBarID ) );
	ASSERT_VALID( pDockBar );
	ASSERT( pDockBar->_GetCircleNo() == 0 );
	if( nCircleNo == 0 )
		pDockBar->_InnerOuterDockControlBar( this, true );
	else
	{
		ASSERT( ! IsFixedMode() );
		pDockBar = pDockBar->_GetBarByCircleNo( nCircleNo );
		ASSERT_VALID( pDockBar );
		ASSERT( pDockBar->_GetCircleNo() == nCircleNo );
		pDockBar->_InnerOuterDockControlBar( this, true );
	}
	if( bRecalcLayout )
		pDockSite->RecalcLayout();
	CExtControlBar::stat_AdjustOccCompatibilityTree( pDockSite );
	return true;
}

// dock control bar relatively to other control bar
bool CExtControlBar::DockControlBar(
	CExtControlBar * pBar,
	bool bHorzBarHalfSplit,
	bool bInnerHalfSplit, // = false
	CFrameWnd * pDockSite, // = NULL // can be NULL only if bar already was at once docked
	bool bRecalcLayout // = true
	)
{
	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtControlBar, this );
	if( pDockSite == NULL )
		pDockSite = m_pDockSite;
	ASSERT_VALID( pDockSite );
	ASSERT_VALID( pBar );
	ASSERT(
		pBar->m_pDockSite == m_pDockSite
		|| pBar->m_pDockSite == NULL
		);

	ASSERT_VALID( m_pDockBar );
	ASSERT_KINDOF( CDockBar, m_pDockBar );
	ASSERT( !IsFixedMode() );
	ASSERT( !pBar->IsFixedMode() );

#ifdef _DEBUG
	if( pBar->m_pDockBar != NULL )
	{
		ASSERT_VALID( pBar->m_pDockBar );
		ASSERT_KINDOF( CExtDockBar, pBar->m_pDockBar );
	}
#endif // _DEBUG

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynTabBar)) )
	{
		CExtControlBar * pParentBar =
			STATIC_DOWNCAST(
				CExtControlBar,
				m_pDockBar->GetParent()
				);
		ASSERT_VALID( pParentBar );
		return
			pParentBar->
				DockControlBar(
					pBar,
					bHorzBarHalfSplit,
					bInnerHalfSplit,
					pDockSite,
					bRecalcLayout
					);
	}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

CRect rc;
	GetWindowRect( &rc );
//	rc.bottom = rc.top + rc.Height()/2;
//	rc.right = rc.left + rc.Width()/2;

InternalDraggingState_t _ds;
	_ds.ExtBarSrcSet( pBar );
	_ds.ExtBarDstSet( this );
	_ds.SetEmpty( false );
	_ds.TrackRectSet( rc );
//	pBar->m_sizeDockedH = m_sizeDockedH;
//	pBar->m_sizeDockedV = m_sizeDockedV;

UINT nOwnDockBarID = GetSafeDockBarDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID( nOwnDockBarID );
	switch( nOwnDockBarID )
	{
	case AFX_IDW_DOCKBAR_FLOAT:
		_ds.SetCDT(
			bHorzBarHalfSplit
				? (bInnerHalfSplit ? __ECDT_INNER_TOP : __ECDT_INNER_BOTTOM)
				: (bInnerHalfSplit ? __ECDT_INNER_LEFT : __ECDT_INNER_RIGHT)
				);
		_ds.SetDeepHalfSplit( true );
		break;
	case AFX_IDW_DOCKBAR_LEFT:
		_ds.SetCDT(
			bHorzBarHalfSplit
				? (bInnerHalfSplit ? __ECDT_INNER_TOP : __ECDT_INNER_BOTTOM)
				: (bInnerHalfSplit ? __ECDT_INNER_RIGHT : __ECDT_INNER_LEFT)
				);
		_ds.SetDeepHalfSplit( !bHorzBarHalfSplit );
		break;
	case AFX_IDW_DOCKBAR_RIGHT:
		_ds.SetCDT(
			bHorzBarHalfSplit
				? (bInnerHalfSplit ? __ECDT_INNER_TOP : __ECDT_INNER_BOTTOM)
				: (bInnerHalfSplit ? __ECDT_INNER_LEFT : __ECDT_INNER_RIGHT)
				);
		_ds.SetDeepHalfSplit( !bHorzBarHalfSplit );
		break;
	case AFX_IDW_DOCKBAR_TOP:
		_ds.SetCDT(
			bHorzBarHalfSplit
				? (bInnerHalfSplit ? __ECDT_INNER_LEFT : __ECDT_INNER_RIGHT)
				: (bInnerHalfSplit ? __ECDT_INNER_BOTTOM : __ECDT_INNER_TOP)
				);
		_ds.SetDeepHalfSplit( bHorzBarHalfSplit );
		break;
	case AFX_IDW_DOCKBAR_BOTTOM:
		_ds.SetCDT(
			bHorzBarHalfSplit
				? (bInnerHalfSplit ? __ECDT_INNER_LEFT : __ECDT_INNER_RIGHT)
				: (bInnerHalfSplit ? __ECDT_INNER_TOP: __ECDT_INNER_BOTTOM)
				);
		_ds.SetDeepHalfSplit( bHorzBarHalfSplit );
		break;
#ifdef _DEBUG
	default:
		{
			ASSERT( FALSE );
		}
		return false;
#endif // _DEBUG
	} // switch( nOwnDockBarID )

// tmp
//_ds.SetDeepHalfSplit( true );

bool bRetVal = _DraggingApplyState( _ds, true );
	
	if( bRecalcLayout )
		pDockSite->RecalcLayout();
	else
		pDockSite->DelayRecalcLayout();

	CExtControlBar::stat_AdjustOccCompatibilityTree( pDockSite );
	return bRetVal;
}

// dock control bar into tabbed container
bool CExtControlBar::DockControlBarIntoTabbedContainer(
	CExtControlBar * pBar,
	INT nIndex, // = -1, // append
	CFrameWnd * pDockSite, // = NULL // can be NULL only if bar already was at once docked
	bool bRecalcLayout // = true
	)
{

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtControlBar, this );
	if( pDockSite == NULL )
		pDockSite = m_pDockSite;
	ASSERT_VALID( pDockSite );
	ASSERT_VALID( pBar );
	ASSERT(
		pBar->m_pDockSite == m_pDockSite
		|| pBar->m_pDockSite == NULL
		);
	ASSERT( pBar != this );

	ASSERT_VALID( m_pDockBar );
	ASSERT_KINDOF( CDockBar, m_pDockBar );

#ifdef _DEBUG
	if( pBar->m_pDockBar != NULL )
	{
		ASSERT( pBar->m_pDockSite == pDockSite );
		ASSERT_VALID( pBar->m_pDockBar );
		ASSERT_KINDOF( CDockBar, pBar->m_pDockBar );
	}
#endif // _DEBUG

	if(		( ! _CanDockToTabbedContainers( pBar ) )
		||	(! pBar->_CanDockToTabbedContainers( this ) )
		||	IsFixedMode()
		||	IsFixedDockStyle()
		||	pBar->IsFixedMode()
		||	pBar->IsFixedDockStyle()
		)
		return false;

	if( IsKindOf(RUNTIME_CLASS(CExtDynTabControlBar)) )
	{
		ASSERT( !m_bUpdatingChain );
		ASSERT( !pBar->m_bUpdatingChain );
		CFrameWnd * pOldFrame = pBar->GetParentFrame();
		HWND hWndOldFrame = pOldFrame->GetSafeHwnd();

		m_bUpdatingChain = true;
		pBar->m_bUpdatingChain = true;
		if( pBar->m_pDockBar != NULL )
			pBar->GetParentFrame()->DelayRecalcLayout();
		BOOL bInsertingOtherTabs =
			pBar->IsKindOf(
				RUNTIME_CLASS(CExtDynTabControlBar)
				);

		//((CExtDynTabControlBar *)(this)) -> SetSwitcherRedraw( false );
		((CExtDynTabControlBar *)(this)) ->
			InsertBar( pBar, nIndex, false );

		if( nIndex < 0 )
			nIndex = 
				((CExtDynTabControlBar *)(this)) ->
					GetSwitcherItemCount() - 1;
		((CExtDynTabControlBar *)(this)) ->
			SetSwitcherSelection( nIndex, true, false );
		((CExtDynTabControlBar *)(this)) ->
			OnRepositionSingleChild( -1, -1, false );

		if( bRecalcLayout )
		{
			GetParentFrame()->RecalcLayout();
//			ASSERT_VALID( m_pDockBar );
//			m_pDockBar->CalcFixedLayout( TRUE, TRUE );
		}
		else
			GetParentFrame()->DelayRecalcLayout();
		
		HWND hWndBar = pBar->GetSafeHwnd();
		if( ! bInsertingOtherTabs )
		{
			ASSERT_VALID( pBar );
			pBar->OnRepositionSingleChild( -1, -1, false );
		} // if( ! bInsertingOtherTabs )
		else
		{
			POSITION pos = m_pDockSite->m_listControlBars.Find( pBar );
			if( pos != NULL )
				m_pDockSite->m_listControlBars.RemoveAt( pos );
			pBar->m_pDockBar->RemoveControlBar( pBar );
			if( ::IsWindow( hWndBar ) )
				pBar->DestroyWindow();
		} // else from if( ! bInsertingOtherTabs )

		m_bUpdatingChain = false;
		if( ::IsWindow( hWndBar ) )
			pBar->m_bUpdatingChain = false;
		if( bRecalcLayout )
			CExtDockBar::_OptimizeCircles( m_pDockSite );
		if( bRecalcLayout && ::IsWindow( hWndOldFrame ) )
		{
			ASSERT_VALID( pOldFrame );
			ASSERT(
				CWnd::FromHandlePermanent( hWndOldFrame )
					== pOldFrame
				);
			//if( pOldFrame != pBar->GetParentFrame() )
				pOldFrame->RecalcLayout();
		}
		if(		::IsWindow( hWndBar )
			&&	CWnd::FromHandlePermanent( hWndBar ) == pBar
			)
			CExtControlBar::stat_AdjustOccCompatibilityTree( pBar->m_pDockSite );
		if( bRecalcLayout )
			OnRepositionSingleChild( -1, -1, bRecalcLayout );
		return true;
	} // if( IsKindOf(RUNTIME_CLASS(CExtDynTabControlBar)) )

	if( m_pDockBar != NULL )
	{
		ASSERT_VALID( m_pDockBar );
		if( m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynTabBar)) )
		{
			CExtDynTabControlBar * pDynTabbed =
				STATIC_DOWNCAST(
					CExtDynTabControlBar,
					m_pDockBar->GetParent()
					);
			ASSERT_VALID( pDynTabbed );
			return
				pDynTabbed->DockControlBarIntoTabbedContainer(
					pBar,
					nIndex,
					pDockSite,
					bRecalcLayout
					);
		}
	}


	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtControlBar, this );
	ASSERT( !IsFixedMode() );
	ASSERT_VALID( m_pDockSite );
	ASSERT_VALID( m_pDockBar );
	ASSERT_KINDOF( CDockBar, m_pDockBar );
	
CDockBar * pDockBarIn = m_pDockBar;
			//		STATIC_DOWNCAST( CExtDockBar, m_pDockBar );
CDockBar * pDockBarOut = pBar->m_pDockBar;
			//		STATIC_DOWNCAST( CExtDockBar, pBar->m_pDockBar );
bool bOptimizeOuterDockBarLayout = false;
HWND hWndDockBarOuter = NULL;
	if( pDockBarOut != NULL )
	{
		if( pDockBarOut->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
			bOptimizeOuterDockBarLayout = true;
		hWndDockBarOuter = pDockBarOut->GetSafeHwnd(); 
	}

CFrameWnd * pOldFrame = pBar->GetParentFrame();
HWND hWndOldFrame = pOldFrame->GetSafeHwnd();

CRect rcDyn;
	GetWindowRect( &rcDyn );
	m_pDockBar->ScreenToClient( &rcDyn );

DWORD dwDynStyle =
		GetStyle()
		| (GetBarStyle() & CBRS_ALIGN_ANY)
		;
	dwDynStyle |= CBRS_BORDER_ANY | CBRS_GRIPPER;
	dwDynStyle &= ~( CBRS_FLOATING );
	dwDynStyle |= CBRS_TOOLTIPS;

UINT nNewID = GetSafeDockBarDlgCtrlID();

CExtDynTabControlBar * pDynBar = NULL;
	pDockSite->SendMessage(
		g_nMsgCreateTabbedBar,
		WPARAM( &pDynBar )
		);
#ifdef _DEBUG
	if( pDynBar != NULL )
	{
		ASSERT_VALID( pDynBar );
		ASSERT_KINDOF( CExtDynTabControlBar, pDynBar );
	} // if( pDynBar != NULL )
#endif // _DEBUG
	if( pDynBar == NULL )
		pDynBar = new CExtDynTabControlBar;

//	pDynBar->SetInitDesiredSizeHorizontal( m_sizeDockedH );
//	pDynBar->SetInitDesiredSizeVertical( m_sizeDockedV );
	if(	! pDynBar->Create(
			NULL,
			m_pDockSite,
			nNewID,
			dwDynStyle
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	ASSERT( pDynBar->m_pWndDynDocker != NULL );
	ASSERT( pDynBar->m_pWndDynDocker->GetSafeHwnd() != NULL );
	pDynBar->m_pDockSite = m_pDockSite;
	pDynBar->m_pDockBar = m_pDockBar;
	pDynBar->EnableDocking( CBRS_ALIGN_ANY );
	ASSERT( pDynBar->m_pDockContext != NULL );
	ASSERT( pDynBar->m_pDockContext->m_pDockSite == m_pDockSite );

//#ifdef _DEBUG
//INT nPosOut = pDockBarOut->FindBar( pBar );
//	ASSERT( nPosOut > 0 && nPosOut < pDockBarOut->m_arrBars.GetSize() );
//#endif // _DEBUG

INT nPosIn = pDockBarIn->FindBar( this );
	ASSERT( nPosIn > 0 && nPosIn < pDockBarIn->m_arrBars.GetSize() );
	pDockBarIn->m_arrBars[ nPosIn ] = pDynBar;

	pDynBar->m_pDockBar->m_bFloating = m_pDockBar->m_bFloating;
	m_pDockBar = pDynBar->m_pWndDynDocker;
//	pBar->m_pDockBar = pDynBar->m_pWndDynDocker;

	m_pDockBar->m_bFloating = FALSE;
	
	if( pDynBar->m_pDockBar->m_bFloating )
		pDynBar->m_pDockBar->SetBarStyle(
			pDynBar->m_pDockBar->GetBarStyle()|CBRS_FLOATING
			);
	m_pDockBar->SetBarStyle(
		m_pDockBar->GetBarStyle() & (~CBRS_FLOATING)
		);

	SetBarStyle( GetBarStyle() & (~CBRS_FLOATING) );

	ASSERT( !m_bUpdatingChain );
	ASSERT( !((CExtControlBar *)pDynBar)->m_bUpdatingChain );
	ASSERT( !pBar->m_bUpdatingChain );
	m_bUpdatingChain = true;
	((CExtControlBar *)pDynBar)->m_bUpdatingChain = true;
	pBar->m_bUpdatingChain = true;
	
	pDynBar->SetParent( pDockBarIn );
	SetParent( pDynBar->m_pWndDynDocker );

	pDynBar->m_pWndDynDocker->m_arrBars.Add( this );
	pDynBar->m_pWndDynDocker->m_arrBars.Add( NULL );

//	SetInitDesiredSizeHorizontal( rcDyn.Size() );
//	SetInitDesiredSizeVertical( rcDyn.Size() );
////	pBar->SetInitDesiredSizeHorizontal( rcDyn.Size() );
////	pBar->SetInitDesiredSizeVertical( rcDyn.Size() );

	((CExtControlBar *)pDynBar)->m_sizeDockedH = m_sizeDockedH;
	((CExtControlBar *)pDynBar)->m_sizeDockedV = m_sizeDockedV;
	((CExtControlBar *)pDynBar)->m_sizeFloated = m_sizeFloated;

CRect rcNew( rcDyn );
rcNew.OffsetRect( -rcNew.TopLeft() );
MoveWindow( &rcNew, FALSE );
//	pBar->MoveWindow( &rcNew, FALSE );
//	pDynBar->SetInitDesiredSizeFloating( rcDyn.Size() );
///////	pDynBar->MoveWindow( &rcDyn, FALSE );

	ASSERT_VALID( pDynBar );
	ASSERT_VALID( pDynBar->m_pWndDynDocker );

	pDynBar->InsertControlBar( this );
	pDynBar->InsertBar( pBar, -1, false );

	m_bUpdatingChain = false;
	((CExtControlBar *)pDynBar)->m_bUpdatingChain = false;
	pBar->m_bUpdatingChain = false;

	if(	bOptimizeOuterDockBarLayout
		&& ::IsWindow( hWndDockBarOuter )
		)
	{
		ASSERT_VALID( pDockBarOut );
		((CExtDockBar*)pDockBarOut)->OnDynamicLayoutOptimize();
	}

// temp
CExtDockBar::_RemovePlaceHolder( m_pDockSite, this );
CExtDockBar::_RemovePlaceHolder( m_pDockSite, pBar );

	CExtDockBar::_OptimizeCircles( m_pDockSite );
	((CExtDockDynTabBar *)pDynBar->m_pWndDynDocker)->_RepositionTabbedChilds( true );
	pDynBar->SetSwitcherSelection( 0, true, false );
CWnd * pWnd = pDynBar->GetParent();
	if( pWnd != NULL && pWnd->IsKindOf(RUNTIME_CLASS(CExtDockDynBar)) )
	{
		CExtDynControlBar * pParentLayoutBar = STATIC_DOWNCAST( CExtDynControlBar, pWnd->GetParent() );
		pParentLayoutBar->OnRepositionSingleChild();
	}

CFrameWnd * pFrame = pDynBar->GetParentFrame();
	if( pFrame != NULL )
	{
		BOOL bMiniFrame =
			pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd));
		if(	bMiniFrame || bRecalcLayout )
		{
			pDynBar->OnNcAreaButtonsReinitialize();
			pDynBar->OnNcAreaButtonsReposition();
			__RECALC_FRAME_STEPS( pFrame );
			if( bMiniFrame )
			{
				__RECALC_FRAME_STEPS( m_pDockSite );
			}
		} // if(	bMiniFrame || bRecalcLayout )
	} // if( pFrame != NULL )
	if( bRecalcLayout && ::IsWindow( hWndOldFrame ) )
	{
		ASSERT_VALID( pOldFrame );
		ASSERT(
			CWnd::FromHandlePermanent( hWndOldFrame )
				== pOldFrame
			);
		if( pOldFrame != pFrame )
			pOldFrame->RecalcLayout();
	}
	CExtControlBar::stat_AdjustOccCompatibilityTree( m_pDockSite );
	return true;
#else

	pBar;
	nIndex;
	pDockSite;
	bRecalcLayout;
	ASSERT( FALSE ); // no implementation
	return false;

#endif // else from (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
}

bool CExtControlBar::_FloatStateChangingIsEnabled(
	const CExtControlBar * pBarQuerySrc
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBarQuerySrc );
	pBarQuerySrc;
	return (! g_bLockAllBars );
}

bool CExtControlBar::_NcMenuButtonIsEnabled(
	const CExtControlBar * pBarQuerySrc
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBarQuerySrc );
	pBarQuerySrc;
	return (! g_bLockAllBars );
}

bool CExtControlBar::_ClosingIsEnabled(
	const CExtControlBar * pBarQuerySrc
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBarQuerySrc );
	pBarQuerySrc;
	return (! g_bLockAllBars );
}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

bool CExtControlBar::_AutoHidingIsEnabled(
	const CExtControlBar * pBarQuerySrc
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBarQuerySrc );
	pBarQuerySrc;
	return (! g_bLockAllBars );
}

INT CExtControlBar::AutoHideWidthGet() const
{
	ASSERT_VALID( this );
	return m_nAutoHideWidth;
}

void CExtControlBar::AutoHideWidthSet( INT nAutoHideWidth )
{
	ASSERT_VALID( this );
	m_nAutoHideWidth = nAutoHideWidth;
}

INT CExtControlBar::AutoHideHeightGet() const
{
	ASSERT_VALID( this );
	return m_nAutoHideHeight;
}

void CExtControlBar::AutoHideHeightSet( INT nAutoHideHeight )
{
	ASSERT_VALID( this );
	m_nAutoHideHeight = nAutoHideHeight;
}

// is bar in autohide mode
bool CExtControlBar::AutoHideModeGet() const
{
	ASSERT_VALID( this );

#ifdef _DEBUG
	if( m_bAutoHideMode )
	{
		ASSERT( !IsFixedMode() );
		ASSERT( !IsFixedDockStyle() );
	}
#endif // _DEBUG

	return m_bAutoHideMode;
}

// switch to/restore from autohide mode
bool CExtControlBar::AutoHideModeSet(
	bool bAutoHideMode,
	bool bAnimate,
	bool bRecalcLayout,
	bool bForceNoShow
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pDockSite );
	ASSERT_VALID( m_pDockBar );
	ASSERT_KINDOF( CExtDockBar, m_pDockBar );

	if( bAutoHideMode || m_bAutoHideMode )
	{
		if( IsFixedMode() || IsFixedDockStyle() )
		{
			ASSERT( FALSE );
			return false;
		}
	}

	if( IsFloating() )
	{
		ASSERT( FALSE );
		return false;
	}

	if( bAutoHideMode == m_bAutoHideMode )
		return true;

CExtDockBar * pDockBar =
		STATIC_DOWNCAST( CExtDockBar, m_pDockBar );
	if( pDockBar->IsKindOf( RUNTIME_CLASS(CExtDockDynTabBar) ) )
	{
		CExtDynTabControlBar * pTabbedBar =
			STATIC_DOWNCAST(
				CExtDynTabControlBar,
				m_pDockBar->GetParent()
				);
		((CExtControlBar *)pTabbedBar)->m_nDelayedRowUpdateMetric = 0;
		bool bRetVal =
			pTabbedBar->AutoHideModeSet(
				bAutoHideMode,
				bAnimate,
				bRecalcLayout,
				bAutoHideMode ? true : bForceNoShow
				);
		ASSERT( bAutoHideMode == m_bAutoHideMode );
		return bRetVal;
	}

CExtDynAutoHideArea * pWndAutoHideArea =
		pDockBar->_GetAutoHideArea();
	if( pWndAutoHideArea == NULL )
	{
		// FrameInjectAutoHideAreas was not called
		ASSERT( FALSE );
		return false;
	}
	ASSERT_VALID( pWndAutoHideArea );

	if( bAutoHideMode )
	{
		if( IsVisible() )
		{
			m_pDockSite->ShowControlBar( this, FALSE, TRUE );
			ASSERT( !IsVisible() );
			m_pDockSite->DelayRecalcLayout();
		} // if( IsVisible() )
		pWndAutoHideArea->AppendControlBar(
			this,
			bForceNoShow
			);

		CExtControlBar * pBar = NULL;
		CExtDynTabControlBar * pTabbedBar =
			DYNAMIC_DOWNCAST(
				CExtDynTabControlBar,
				m_pDockBar->GetParent()
				);
		if( pTabbedBar != NULL )
		{
			LONG nSel = pTabbedBar->GetSwitcherSelection();
			if( nSel >= 0 )
				pBar = pTabbedBar->GetBarAt( nSel, true );
		}
		else
			pBar = this;

		if( pBar != NULL )
		{
			CWnd * pWndParent = pBar->m_pDockBar;
			for( ; true ; )
			{
				ASSERT_VALID( pWndParent );
				if( pWndParent->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
				{
					if( ! pWndParent->IsKindOf(RUNTIME_CLASS(CExtDockDynBar)) )
					{
						ASSERT( pWndParent->GetParent() == m_pDockSite );
						break;
					} // if( ! pWndParent->IsKindOf(RUNTIME_CLASS(CExtDockDynBar)) )
				} // if( pWndParent->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
				pWndParent = pWndParent->GetParent();
			} // for( ; true ; )
			ASSERT_KINDOF( CExtDockBar, pWndParent );
			ASSERT( ((CExtDockBar*)pWndParent)->_GetCircleNo() > 0 );
			UINT nTopDockBarID = pWndParent->GetDlgCtrlID();
			ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nTopDockBarID );
			CExtDockBar * pOuterDocker =
				STATIC_DOWNCAST(
					CExtDockBar,
					m_pDockSite->GetControlBar( nTopDockBarID )
					);
			ASSERT_VALID( pOuterDocker );
			ASSERT( UINT(pOuterDocker->GetDlgCtrlID()) == nTopDockBarID );
			
			CExtDynAutoHideArea * pAutoHideArea =
				pOuterDocker->_GetAutoHideArea();
			ASSERT_VALID( pAutoHideArea );
			ASSERT( UINT(pAutoHideArea->GetDlgCtrlID()) == nTopDockBarID );

			LONG nPos = pAutoHideArea->FindControlBar( pBar );
			if( nPos >= 0 )
			{
				CExtTabWnd::TAB_ITEM_INFO * pTII =
					pAutoHideArea->ItemGet( nPos );
				pTII->ModifyItemStyle( 0, __ETWI_IN_GROUP_ACTIVE );
			}
		}
	
	} // if( bAutoHideMode )
	else
	{
		ASSERT( !IsVisible() );
		//m_pDockSite->ShowControlBar( this, TRUE, TRUE );
		m_pDockSite->DelayRecalcLayout();
		pWndAutoHideArea->RemoveControlBar(
			this,
			bForceNoShow
			);
		if( bForceNoShow )
		{
			HWND hWnd =
				::GetWindow(
					::GetParent(pWndAutoHideArea->m_hWnd),
					GW_CHILD
					);
			TCHAR sClassName[ _MAX_PATH + 1 ];
			for( ; hWnd != NULL; hWnd = ::GetWindow(hWnd,GW_HWNDNEXT) )
			{
				::memset( sClassName, 0, sizeof(sClassName) );
				::GetClassName(
					hWnd,
					sClassName,
					_MAX_PATH
					);
				if( _tcscmp( sClassName,__AUTOHIDE_SLIDER_WNDCLASS ) == 0 )
				{
					::SendMessage( hWnd, WM_CANCELMODE, 0L, 0L );
					break;
				}
			} // for( ; hWnd != NULL; hWnd = ::GetWindow(hWnd,GW_HWNDNEXT) )
			CExtDynTabControlBar * pTabbedBar =
				DYNAMIC_DOWNCAST( CExtDynTabControlBar, this );
			if( pTabbedBar != NULL )
			{
				LONG nPos, nCount = pTabbedBar->GetSwitcherItemCount();
				for( nPos = 0; nPos < nCount; nPos++ )
				{
					CExtControlBar * pBar =
						pTabbedBar->GetBarAt( nPos, true );
					if( pBar->IsVisible() )
						m_pDockSite->ShowControlBar( pBar, FALSE, TRUE );
#if (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)
					CExtDynamicControlBar * pDynamicBar = DYNAMIC_DOWNCAST( CExtDynamicControlBar, pBar );
					if( pDynamicBar != NULL )
						pDynamicBar->BarStateSet( CExtDynamicControlBar::__EDBS_DOCKED, false );
#endif // (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)
				} // for( nPos = 0; nPos < nCount; nPos++ )
			} // if( pTabbedBar != NULL )
		} // if( bForceNoShow )
		else
		{
#if (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)
			CExtDynTabControlBar * pTabbedBar =
				DYNAMIC_DOWNCAST( CExtDynTabControlBar, this );
			if( pTabbedBar != NULL )
			{
				LONG nPos, nCount = pTabbedBar->GetSwitcherItemCount();
				for( nPos = 0; nPos < nCount; nPos++ )
				{
					CExtControlBar * pBar =
						pTabbedBar->GetBarAt( nPos, true );
					CExtDynamicControlBar * pDynamicBar = DYNAMIC_DOWNCAST( CExtDynamicControlBar, pBar );
					if( pDynamicBar != NULL )
						pDynamicBar->BarStateSet( CExtDynamicControlBar::__EDBS_DOCKED, true );
				} // for( nPos = 0; nPos < nCount; nPos++ )
			} // if( pTabbedBar != NULL )
#endif // (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)
		} // else from if( bForceNoShow )
	} // else from if( bAutoHideMode )

	m_bAutoHideMode = bAutoHideMode;

	CExtControlBar::stat_AdjustOccCompatibilityTree( m_pDockSite );

	if( bRecalcLayout )
	{
		// tricky chain visibility updating
		bool bSaved = m_bAutoHideMode;
		m_bAutoHideMode = false;
		((InternalFriendlyFrameWnd*)m_pDockSite)->m_bInRecalcLayout = TRUE;
		_UpdateVisibilityInChain();
		((InternalFriendlyFrameWnd*)m_pDockSite)->m_bInRecalcLayout = FALSE;
		m_bAutoHideMode = bSaved;

		m_pDockSite->RecalcLayout();
		if( ! m_bAutoHideMode )
		{
			OnNcAreaButtonsReposition();
			PostMessage( WM_NCPAINT );
			PostMessage( WM_TIMER, __TIMER_ID_DRELAYED_REPAINT_FAKE );
		} // if( ! m_bAutoHideMode )
		CExtPaintManager::stat_PassPaintMessages();
	} // if( bRecalcLayout )

	return true;
}

bool CExtControlBar::OnQueryVisibilityInAutoHideTabs() const
{
	ASSERT_VALID( this );
	return true;
}

bool CExtControlBar::OnQueryVisibilityInContainerTabs() const
{
	ASSERT_VALID( this );
	return true;
}

#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

void CExtControlBar::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CControlBar::OnShowWindow(bShow, nStatus);

CExtPopupMenuTipWnd * pATTW =
		OnAdvancedPopupMenuTipWndGet();
	if( pATTW != NULL )
		pATTW->Hide();
	
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = NotifiedCustomizeSiteGet();
	if( pSite != NULL )
		pSite->OnBarStateChanged( this );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( nStatus != 0 )
		return;

	_UpdateVisibilityInChain();
}

bool CExtControlBar::stat_FindSingleVisbleBarNested(
	CDockBar * pDockBar,
	CExtControlBar ** ppFoundBar
	)
{
	ASSERT_VALID( pDockBar );
	ASSERT( pDockBar->GetSafeHwnd() != NULL );
	ASSERT_KINDOF( CDockBar, pDockBar );
	ASSERT( pDockBar->IsDockBar() );
	ASSERT( ppFoundBar != NULL );
INT nIndex, nCount = INT( pDockBar->m_arrBars.GetSize() );
	for( nIndex = 0; nIndex < nCount; nIndex++ )
	{
		CControlBar * pBar = (CControlBar *)pDockBar->m_arrBars.GetAt( nIndex );
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR( pBar ) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CControlBar, pBar );
		ASSERT( ! pBar->IsDockBar() );
		if( ! pBar->IsVisible() )
			continue;
		CExtControlBar * pExtBar = DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
//		{
//			if( (*ppFoundBar) != NULL )
//				return false;
//			(*ppFoundBar) = pBar;
//			continue;
//		} // if( pExtBar == NULL )
		CExtDynControlBar * pDynBar = DYNAMIC_DOWNCAST( CExtDynControlBar, pBar );
		if( pDynBar == NULL )
		{
			if( (*ppFoundBar) != NULL )
				return false;
			(*ppFoundBar) = pExtBar;
			continue;
		} // if( pDynBar == NULL )
		if(		pDynBar->m_pWndDynDocker == NULL
			||	pDynBar->m_pWndDynDocker->GetSafeHwnd() == NULL
			)
			continue;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		CExtDynTabControlBar * pTabbedBar = DYNAMIC_DOWNCAST( CExtDynTabControlBar, pDynBar );
		if( pTabbedBar != NULL )
		{
			LONG nSel = pTabbedBar->GetSwitcherSelection();
			if( nSel < 0 )
				continue;
			CExtControlBar * pSetInTabsBar = pTabbedBar->GetBarAt( nSel, true );
			if( pSetInTabsBar == NULL )
				continue;
			ASSERT_VALID( pSetInTabsBar );
			ASSERT( pSetInTabsBar->GetSafeHwnd() != NULL );
			if( (*ppFoundBar) != NULL )
				return false;
			(*ppFoundBar) = pSetInTabsBar;
			continue;
		} // if( pTabbedBar != NULL )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

		CExtControlBar * pFoundBar2 = NULL;
		if( stat_FindSingleVisbleBarNested( pDynBar->m_pWndDynDocker, &pFoundBar2 ) )
		{
			if( (*ppFoundBar) != NULL )
				return false;
			(*ppFoundBar) = pFoundBar2;
			continue;
		}
	} // for( nIndex = 0; nIndex < nCount; nIndex++ )
	if( (*ppFoundBar) != NULL )
		return true;
	return false;
}

bool CExtControlBar::FlashCaption_DoPaintNC(
	CDC & dc,
	LPVOID pPGD
	)
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( pPGD != NULL );
	dc;
	pPGD;
//	if( ! bFlashCaptionHighlightedState )
//		return false;
//CExtPaintManager::PAINTGRIPPERDATA & _pgd = *( (CExtPaintManager::PAINTGRIPPERDATA *) pPGD );
//	NcButtons_Paint( *pDC );
//	return true;
	return false;
}

void CExtControlBar::FlashCaption_OnRepaint()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( AutoHideModeGet())
	{
		CFrameWnd * pFrame = GetParentFrame();
		ASSERT_VALID( pFrame );
		ASSERT( pFrame->GetSafeHwnd() != NULL );
		ASSERT( ! pFrame->IsKindOf( RUNTIME_CLASS( CExtMiniDockFrameWnd ) ) );
		CExtDynAutoHideArea * pDAHA = NULL;
		CWnd * pWnd = pFrame->GetWindow( GW_CHILD );
		for( ; pWnd != NULL; pWnd = pWnd->GetWindow( GW_HWNDNEXT ) )
		{
			CExtDynAutoHideArea * pDAHA2 = DYNAMIC_DOWNCAST( CExtDynAutoHideArea, pWnd );
			if( pDAHA2 == NULL )
				continue;
			LONG nIndex = pDAHA2->FindControlBar( this );
			if( nIndex < 0 )
				continue;
			pDAHA = pDAHA2;
			break;
		}
		if( pDAHA->IsWindowVisible() )
			pDAHA->Invalidate();
		CExtDynAutoHideSlider * pDAHS = pDAHA->GetAutoHideSlider();
		if(		pDAHS != NULL
			&&	pDAHS->_GetBar() == this
			&&	pDAHS->IsWindowVisible()
			)
			pDAHS->SendMessage( WM_NCPAINT );
		return;
	} // if( AutoHideModeGet())
CExtDynTabControlBar * pTabbedBar = _GetNearestTabbedContainer();
	if( pTabbedBar != NULL )
	{
		ASSERT_VALID( pTabbedBar );
		ASSERT( pTabbedBar->GetSafeHwnd() != NULL );
		if( pTabbedBar->IsWindowVisible() )
			pTabbedBar->SendMessage( WM_NCPAINT );
		pTabbedBar->InvalidateSwitcher();
	} // if( pTabbedBar != NULL )
	else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	{
		if( IsWindowVisible() )
			SendMessage( WM_NCPAINT );
	} // else from if( pTabbedBar != NULL )
CFrameWnd * pFrame = GetParentFrame();
	ASSERT_VALID( pFrame );
	ASSERT( pFrame->GetSafeHwnd() != NULL );
	if(		pFrame->IsKindOf( RUNTIME_CLASS( CExtMiniDockFrameWnd ) )
		&&	pFrame->IsWindowVisible()
		)
		pFrame->SendMessage( WM_NCPAINT );
#if (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)
#if (!defined __EXT_MFC_NO_TABMDI_CTRL)
	else
	{
		bool bVisibleState = false;
		CExtDynamicControlBar * pBar = DYNAMIC_DOWNCAST( CExtDynamicControlBar, this );
		if(		pBar != NULL
			&&	pBar->BarStateGet( &bVisibleState ) == CExtDynamicControlBar::__EDBS_DOCUMENT
			)
		{
			if( bVisibleState )
			{
				CExtDynamicBarSite * pDBS = pBar->GetBarSite();
				if( pDBS != NULL )
				{
					switch( pDBS->GetDetectedUiType() )
					{
					case CExtDynamicBarSite::__EDUIT_MDI_WITH_TABS:
						{
							CExtTabWnd * pWndMdiTabs = pDBS->GetMdiTabs();
							if( pWndMdiTabs != NULL )
							{
								pWndMdiTabs->Invalidate();
								pWndMdiTabs->UpdateWindow();
							}
						}
					break;
					case CExtDynamicBarSite::__EDUIT_SDI_TAB_PAGE_CONTAINER:
						{
							CExtTabPageContainerWnd * pWndTPC = pDBS->GetTabPageContainer();
							if( pWndTPC != NULL )
							{
								CExtTabWnd * pWndTabs = pWndTPC->GetSafeTabWindow();
								if( pWndTabs != NULL )
								{
									pWndTabs->Invalidate();
									pWndTabs->UpdateWindow();
								}
							}
						}
					break;
					}
				}
			}
		}
	}
#endif // __EXT_MFC_NO_TABMDI_CTRL
#endif // __EXT_MFC_NO_DYNAMIC_BAR_SITE
}

void CExtControlBar::FlashCaption_OnTimer()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( m_nFlashCaptionCounter >= 0 );
	if( m_bFlashCaptionHighlightedState )
	{
		m_bFlashCaptionHighlightedState = false;
		FlashCaption_OnRepaint();
		m_nFlashCaptionCounter --;
		if( m_nFlashCaptionCounter <= 0 )
			KillTimer( m_nFlashCaptionTimerID );
	} // if( m_bFlashCaptionHighlightedState )
	else
	{
		m_bFlashCaptionHighlightedState = true;
		FlashCaption_OnRepaint();
	}
}

bool CExtControlBar::FlashCaptionIsInProgress(
	bool * p_bFlashCaptionHighlightedState // = NULL
	) const
{
	ASSERT_VALID( this );
	if( p_bFlashCaptionHighlightedState != NULL )
		(*p_bFlashCaptionHighlightedState) = false;
	if( GetSafeHwnd() == NULL )
		return false;
	if( m_nFlashCaptionCounter == 0 )
		return false;
	if( p_bFlashCaptionHighlightedState != NULL )
		(*p_bFlashCaptionHighlightedState) = m_bFlashCaptionHighlightedState;
	return true;
}

bool CExtControlBar::FlashCaption(
	INT nFlashCount, // = -1 // negative value cancels current flashing
	INT nStepMilliseconds, // = 250
	COLORREF clrFlashText, // = RGB(255,255,255),
	COLORREF clrFlashBackground // = RGB(255,0,0)
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return false;
	m_bFlashCaptionHighlightedState = false;
	if( nFlashCount <= 0 || nStepMilliseconds <= 0 )
	{
		m_nFlashCaptionCounter = 0;
		FlashCaption_OnTimer();
		return true;
	}
	m_clrFlashCaptionText = clrFlashText;
	m_clrFlashCaptionBackground = clrFlashBackground;
	m_nFlashCaptionCounter = nFlashCount;
	m_nFlashCaptionStepMilliseconds = nStepMilliseconds;
	SetTimer( UINT(m_nFlashCaptionTimerID), UINT(m_nFlashCaptionStepMilliseconds), NULL );
	return true;
}

bool CExtControlBar::FindHelpMode(
	CWnd * pWndStartSearch
	)
{
	ASSERT_VALID( pWndStartSearch );
	ASSERT( pWndStartSearch->GetSafeHwnd() != NULL );
CFrameWnd * pFrame = DYNAMIC_DOWNCAST( CFrameWnd, pWndStartSearch );
	if( pFrame == NULL )
		pFrame = pWndStartSearch->GetParentFrame();
	for( ;	pFrame != NULL;
			pFrame = pFrame->GetParentFrame()
		)
	{
		if( pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
			continue;
		if( pFrame->m_bHelpMode )
			return true;
	}
	return false;
}

bool CExtControlBar::FindPrintPreviewMode(
	CFrameWnd * pFrame
	)
{
	//ASSERT_VALID( pFrame );
CView * pView = pFrame->GetActiveView();
	if(		pView != NULL
		&&	pView->IsKindOf( RUNTIME_CLASS(CPreviewView) )
		)
		return true;
	return false;
}

bool CExtControlBar::IsDockSiteInPrintPreviewMode()
{
	ASSERT_VALID( this );
	if(		m_pDockSite == NULL
		||	m_bPresubclassDialogMode
		)
		return false;
	return FindPrintPreviewMode( m_pDockSite );
}

void CExtControlBar::DelayShow( BOOL bShow )
{
	ASSERT_VALID( this );

	m_bDelayShowInvoked = true;
	m_bDelayShowFlag = bShow ? true : false;
CExtDynControlBar * pDynContainer = _GetNearestDynamicContainer();
	if( pDynContainer != NULL )
	{
		pDynContainer->m_bDelayShowInvoked = true;
		pDynContainer->m_bDelayShowFlag = m_bDelayShowFlag;
	}

CMiniDockFrameWnd * pMiniFrame =
		DYNAMIC_DOWNCAST( CMiniDockFrameWnd, GetParentFrame() );
	if(		IsFloating()
		&&	(! IsVisible() )
		&&	(! IsFixedMode() )
		)
	{
		if( pMiniFrame != NULL )
		{
			ASSERT_VALID( pMiniFrame );
			pMiniFrame->ModifyStyle( 0, FWS_SNAPTOBARS );
		}
	}
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	else
	if( m_bAutoHideMode )
		bShow = FALSE;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( pMiniFrame != NULL && bShow && m_pDockBar->GetSafeHwnd() != NULL )
	{
		CControlBar::DelayShow( bShow );
		CControlBar * pParentBar = DYNAMIC_DOWNCAST( CControlBar, m_pDockBar->GetParent() );
		if( pParentBar != NULL )
			pParentBar->DelayShow( bShow );
		else
		{
			pMiniFrame->RecalcLayout();
			pMiniFrame->ShowWindow( SW_SHOWNA );
		}
		return;
	}
	CControlBar::DelayShow( bShow );
}

void CExtControlBar::_UpdateVisibilityInChain()
{
LONG nDelayedRowUpdateMetric = m_nDelayedRowUpdateMetric;
	m_nDelayedRowUpdateMetric = 0;
	if( m_pDockBar == NULL && (!m_bPresubclassDialogMode) )
		return; // on destruction

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( m_bAutoHideMode )
		return;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	if( _DraggingGetBar() != NULL )
		return;
	if( _GetBarToggleFloatingState() != NULL )
		return;

	if( IsFixedMode() || IsFixedDockStyle()
		|| m_pDockSite == NULL
		|| m_pDockBar == NULL
		|| IsDockSiteInPrintPreviewMode()
//		|| (	m_pDockBar->GetSafeHwnd() != NULL
//			&&	(!m_pDockBar->IsVisible())
//			)
		)
		return;

	ASSERT_VALID( m_pDockSite );

	if( m_bUpdatingChain )
		return;
	m_bUpdatingChain = true;

	m_nDelayedRowUpdateMetric = nDelayedRowUpdateMetric;
	_UpdateVisibilityInRow();

BOOL bVisible = IsVisible();
CExtDockDynBar * pDynDocker = DYNAMIC_DOWNCAST( CExtDockDynBar, m_pDockBar );
	if( pDynDocker != NULL )
	{ // update parent dynamic docker with new visiblity options
		ASSERT_KINDOF( CExtDockDynBar, pDynDocker );

		CExtDynControlBar * pDynBar =
			STATIC_DOWNCAST(
				CExtDynControlBar,
				pDynDocker->GetParent()
				);
		ASSERT_VALID( pDynBar );
//		if( pDynBar->m_bUpdatingChain )
//			return;
		
		if( bVisible )
		{
			if( ! pDynBar->IsVisible() )
				m_pDockSite->ShowControlBar( pDynBar, TRUE, TRUE );
//			pDynDocker->OnDynamicLayoutUpdate();
		} // if( bVisible )
		else
		{
			INT nDockedVisibleCount =  pDynDocker->GetDockedVisibleCount();
			if( nDockedVisibleCount == 0 )
			{
				if( pDynBar->IsVisible() )
					m_pDockSite->ShowControlBar( pDynBar, FALSE, TRUE );
			} // if( nDockedVisibleCount == 0 )
		} // else from if( bVisible )
	} // update parent dynamic docker with new visiblity options

	if( ! bVisible )
	{ // if( ! bVisible ) - update child invisibility options
		if( IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
		{
			CExtDockDynBar * pInnerDynDocker =
				((CExtDynControlBar *) this)->m_pWndDynDocker;
			if( pInnerDynDocker != NULL )
			{
				//pInnerDynDocker->ShowAll( FALSE );
				for( INT nPos = 0; nPos < pInnerDynDocker->m_arrBars.GetSize(); nPos++ )
				{
					CControlBar * pBar = pInnerDynDocker->GetDockedControlBar( nPos );
					if( pBar == NULL )
						continue;
					CExtDynControlBar * pDynBar =
						DYNAMIC_DOWNCAST( CExtDynControlBar, pBar );
					if( pDynBar == NULL )
						continue;
					//pDynBar->m_bUpdatingChain = true;
					pDynBar->DelayShow( FALSE );
					//pDynBar->SetWindowPos(
					//	NULL, 0, 0, 0, 0,
					//	SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE
					//		|SWP_NOACTIVATE
					//		|SWP_HIDEWINDOW
					//		|SWP_NOSENDCHANGING
					//	);
					//pDynBar->m_bUpdatingChain = false;
				} // for( INT nPos = 0; nPos < pInnerDynDocker->m_arrBars.GetSize(); nPos++ )
			} // if( pInnerDynDocker != NULL )
		}
	} // if( ! bVisible ) - update child invisibility options

CFrameWnd * pParentFrame = GetParentFrame();
	ASSERT_VALID( pParentFrame );
	if( pParentFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
		pParentFrame->RecalcLayout();

	m_bUpdatingChain = false;
}

void CExtControlBar::_UpdateVisibilityInRow()
{
	if( m_pDockBar == NULL && (!m_bPresubclassDialogMode) )
		return; // on destruction

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( m_bAutoHideMode )
		return;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	if( IsFixedMode() || IsFixedDockStyle()
		|| m_pDockSite == NULL
		|| m_pDockBar == NULL
		)
		return;
	ASSERT_VALID( m_pDockSite );

//	if( IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
//		return;
CExtDynControlBar * pDynBar =
		DYNAMIC_DOWNCAST( CExtDynControlBar, this );
	if(		pDynBar != NULL
		&&	(! pDynBar->_IsDelayedVisibilityUpdateEnabled() )
		)
		return;

	ASSERT_VALID( m_pDockBar );

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
CExtDynTabControlBar * pTabbedBar = NULL;
	if( m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynTabBar)) )
	{
		pTabbedBar =
			STATIC_DOWNCAST(
				CExtDynTabControlBar,
				m_pDockBar->GetParent()
				);
		ASSERT_VALID( pTabbedBar );
		pTabbedBar->UpdateBarVisibility( this );
	} // if( m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynTabBar)) )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

BOOL bVisible = IsVisible();

	// recalc desired size in row after ShowWindow(SW_SHOW)
UINT nDockBarID = GetSafeDockBarDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID( nDockBarID );
	if( nDockBarID == AFX_IDW_DOCKBAR_FLOAT )
		return;
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );
bool bHorz =
		(	nDockBarID == AFX_IDW_DOCKBAR_TOP
		||	nDockBarID == AFX_IDW_DOCKBAR_BOTTOM
		) ? true : false;

	if( !bVisible )
	{
		m_nDelayedRowUpdateMetric = bHorz
			? m_sizeDockedH.cx
			: m_sizeDockedV.cy
			;
		return;
	}
	if( m_nDelayedRowUpdateMetric == 0 )
		return;

long nDelayedRowUpdateMetric = m_nDelayedRowUpdateMetric;
	m_nDelayedRowUpdateMetric = 0;

ExtControlBarVector_t vRow;
	_GetRowExtBars( vRow );
INT nCountInRow = (INT)vRow.GetSize();
	ASSERT( nCountInRow > 0 );
	if( nCountInRow == 1 )
		return;

	for( INT nBar = 0; nBar < nCountInRow; )
	{
		CExtControlBar * pBar = vRow[ nBar ];
		ASSERT_VALID( pBar );
		BOOL bBarIsVisible = pBar->IsVisible();
		bool bFixedBar = pBar->IsFixedMode();
		if( pBar == this
			|| (!bBarIsVisible)
			|| bFixedBar
			)
		{
			nCountInRow--;
			vRow.RemoveAt( nBar );
			continue;
		}

		LONG nRealBarExtent = bHorz
			? pBar->m_sizeDockedH.cx
			: pBar->m_sizeDockedV.cy
			;
		LONG nMinBarExtent = bHorz
			? pBar->_CalcDesiredMinHW()
			: pBar->_CalcDesiredMinVH()
			;
		if( nRealBarExtent <= nMinBarExtent )
		{
			nCountInRow--;
			vRow.RemoveAt( nBar );
			continue;
		}

		nBar ++;
	} // for( INT nBar = 0; nBar < nCountInRow; )
	if( nCountInRow == 0 )
		return;

LONG nExtentDesizred = nDelayedRowUpdateMetric;
	for( ; nExtentDesizred > 0 && nCountInRow > 0; )
	{
		for( INT nBar = 0; nBar < nCountInRow; )
		{
			CExtControlBar * pBar = vRow[ nBar ];
			ASSERT_VALID( pBar );

			LONG & nRealBarExtentRef = bHorz
				? pBar->m_sizeDockedH.cx
				: pBar->m_sizeDockedV.cy
				;
			LONG nMinBarExtent = bHorz
				? pBar->_CalcDesiredMinHW()
				: pBar->_CalcDesiredMinVH()
				;

			ASSERT( nRealBarExtentRef > nMinBarExtent );

			nRealBarExtentRef --;
			nExtentDesizred --;
			if( nExtentDesizred == 0 )
				break;
			if( nRealBarExtentRef == nMinBarExtent )
			{
				nCountInRow--;
				vRow.RemoveAt( nBar );
				continue;
			}
			nBar ++;
		} // for( nBar = 0; nBar < nCountInRow; )
	} // for( ; nExtentDesizred > 0 && nCountInRow > 0; )
}

bool CExtControlBar::g_bControlBarClassRegistered = false;

bool CExtControlBar::RegisterControlBarClass()
{
	if( g_bControlBarClassRegistered )
		return true;
WNDCLASS _wndClassInfo;
HINSTANCE hInst = ::AfxGetInstanceHandle();
	if( ! ::GetClassInfo( hInst, __EXT_CONTROLBAR_CLASS_NAME, &_wndClassInfo ) )
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
		_wndClassInfo.lpszClassName = __EXT_CONTROLBAR_CLASS_NAME;
		if( !::AfxRegisterClass( &_wndClassInfo ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}
	g_bControlBarClassRegistered = true;
	return true;
}

BOOL CExtControlBar::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( ( !RegisterControlBarClass() )
		|| ( !CControlBar::PreCreateWindow(cs) )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	
	cs.lpszClass = __EXT_CONTROLBAR_CLASS_NAME;

	return TRUE;
}

void CExtControlBar::PreSubclassWindow() 
{
	CControlBar::PreSubclassWindow();

	// emulate for enabling update in user non-frame mode
	m_bInCustomModeUpdateCmdUI = false;

	if( ! m_bPresubclassDialogMode )
	{ // if m_bPresubclassDialogMode was not set manually
		HWND hWndParent = ::GetParent( m_hWnd );
		if( hWndParent != NULL )
		{
			CWnd * pWndParent = CWnd::FromHandlePermanent( hWndParent );
			if( pWndParent != NULL )
			{
				if(		( ! pWndParent->IsKindOf(RUNTIME_CLASS(CControlBar)) )
					&&	( ! pWndParent->IsKindOf(RUNTIME_CLASS(CFrameWnd)) )
					)
				{
					// non-frame mode
					// may be we are inside dialog
					__EXT_MFC_ULONG_PTR dwWndStyle = ::__EXT_MFC_GetWindowLong( m_hWnd, GWL_STYLE );
					m_dwStyle = DWORD(dwWndStyle & CBRS_ALL);
					m_bPresubclassDialogMode = true;
				}
			} // if( pWndParent != NULL )
		} // if( hWndParent != NULL )
	} // if m_bPresubclassDialogMode was not set manually

	if( m_dwStyle & CBRS_TOOLTIPS )
		EnableToolTips();

	if( ( (CWnd::GetExStyle()&(WS_EX_LAYOUTRTL|WS_EX_RTLREADING)) != 0 ) )
	{
		CWnd::ModifyStyleEx(
			WS_EX_LAYOUTRTL|WS_EX_RTLREADING,
			0,
			SWP_FRAMECHANGED
			);
		ASSERT( (CWnd::GetExStyle()&(WS_EX_LAYOUTRTL|WS_EX_RTLREADING)) == 0 );
	} // if( ( (CWnd::GetExStyle()&(WS_EX_LAYOUTRTL|WS_EX_RTLREADING)) != 0 ) )

	AnimationSite_Init( false );
}

#define ID_TIMER_WAIT   0xE000  // timer while waiting to show status
#define ID_TIMER_CHECK  0xE001  // timer to check for removal of status

BOOL CExtControlBar::PreTranslateMessage(MSG* pMsg) 
{
//	CControlBar::PreTranslateMessage(MSG* pMsg) 

	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);

	// allow tooltip messages to be filtered
	if( CWnd::PreTranslateMessage(pMsg) )
		return TRUE;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
CWnd * pWndParent = GetParent();
	if(		pWndParent->GetSafeHwnd() != NULL
		&&	pWndParent->IsKindOf( RUNTIME_CLASS(CExtDockDynBar) )
		&&	pWndParent->PreTranslateMessage(pMsg)
		)
		return TRUE;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	

	if(		(m_nFlags&WF_OLECTLCONTAINER) != NULL
		&&	WM_KEYFIRST <= pMsg->message
		&&	pMsg->message <= WM_KEYLAST
		)
	{ // analyze OLE keyboard message translation
		HWND hWndFocus = ::GetFocus();
		if(		hWndFocus != NULL
			&&	::IsChild( m_hWnd, hWndFocus )
			)
		{ // if focus is inside
			HWND hWndOwn = m_hWnd;
			HWND hWnd = hWndFocus;
			for( ; hWnd != NULL && hWnd != hWndOwn; hWnd = ::GetParent(hWnd) )
			{
				if( ! ::IsWindow(hWndOwn) )
					return FALSE;
				CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
				if( pWnd == NULL )
					continue;
				LPUNKNOWN pUnknown = pWnd->GetControlUnknown();
				if( pUnknown != NULL )
				{
					if( pWnd->PreTranslateMessage( pMsg ) )
						return TRUE;
					IOleInPlaceActiveObject * pOleInPlaceActiveObject = NULL;
					HRESULT hr =
						pUnknown->QueryInterface(
							__uuidof(IOleInPlaceActiveObject),
							(LPVOID*)(&pOleInPlaceActiveObject)
							);
					if( hr == S_OK )
					{
						ASSERT( pOleInPlaceActiveObject != NULL );
						hr =
							pOleInPlaceActiveObject->TranslateAccelerator(
								pMsg
								);
						pOleInPlaceActiveObject->Release();
						if( hr == S_OK )
							return TRUE;
					} // if( hr == S_OK )
				} // if( pUnknown != NULL )
			} // for( ; hWnd != NULL && hWnd != hWndOwn; hWnd = ::GetParent(hWnd) )
		} // if focus is inside
	} // analyze OLE keyboard message translation

CWnd * pOwner = GetOwner();

#if _MFC_VER < 0x700
UINT message = pMsg->message;
	// handle CBRS_FLYBY style (status bar flyby help)
	if(	(	(		m_dwStyle & CBRS_FLYBY)
				||	message == WM_LBUTTONDOWN
				||	message == WM_LBUTTONUP
				)
			&&
			(	(		message >= __EXT_MFC_WM_MOUSEFIRST
					&&	message <= __EXT_MFC_WM_MOUSELAST
					) ||
					(	message >= WM_NCMOUSEFIRST
					&& message <= WM_NCMOUSELAST
				)
			)
			&&
			(	(!		CExtToolControlBar::g_bMenuTracking
					||	CExtPopupMenuWnd::IsMenuTracking()
				)
			)
		 )
	{
//#if _MFC_VER < 0x700
		_AFX_THREAD_STATE * pThreadState =
			::AfxGetThreadState();
//#else
//		AFX_MODULE_THREAD_STATE * pThreadState =
//			//::AfxGetModuleThreadState();
//			//AfxGetModuleState()->m_thread.GetData();
//			::AfxGetThreadState()->m_pModuleState->m_thread.GetData();
//#endif

		// gather information about current mouse position
		CPoint point = pMsg->pt;
		ScreenToClient( &point );
		TOOLINFO _ti;
		::memset( &_ti, 0, sizeof(TOOLINFO) );
		_ti.cbSize = sizeof(AFX_OLDTOOLINFO);
		__EXT_MFC_INT_PTR nHit = OnToolHitTest( point, &_ti );
		if(		_ti.lpszText != NULL
			&&	_ti.lpszText != LPSTR_TEXTCALLBACK
			)
			free( _ti.lpszText );
		BOOL bNotButton =
			message == WM_LBUTTONDOWN && (_ti.uFlags & TTF_NOTBUTTON);
		if (message != WM_LBUTTONDOWN && GetKeyState(VK_LBUTTON) < 0)
			nHit = pThreadState->m_nLastStatus;

		// update state of status bar
		if (nHit < 0 || bNotButton)
		{
			if (GetKeyState(VK_LBUTTON) >= 0 || bNotButton)
			{
				SetStatusText(-1);
				KillTimer(ID_TIMER_CHECK);
			}
		}
		else
		{
			if (message == WM_LBUTTONUP)
			{
				SetStatusText(-1);
				ResetTimer(ID_TIMER_CHECK, 200);
			}
			else
			{
				if ((m_nStateFlags & statusSet) || GetKeyState(VK_LBUTTON) < 0)
					SetStatusText(nHit);

				else if (nHit != pThreadState->m_nLastStatus)
					ResetTimer(ID_TIMER_WAIT, 300);
			}
		}
		pThreadState->m_nLastStatus = nHit;
	}
#endif

	// don't translate dialog messages when in Shift+F1 help mode
CFrameWnd* pFrameWnd = GetTopLevelFrame();
	if( pFrameWnd != NULL && pFrameWnd->m_bHelpMode )
		return FALSE;

	if( ! m_bPresubclassDialogMode )
	{

		// since 'IsDialogMessage' will eat frame window accelerators,
		//   we call all frame windows' PreTranslateMessage first
		while( pOwner != NULL )
		{
			HWND hWndOwner = pOwner->GetSafeHwnd();
			ASSERT( hWndOwner != NULL );
			if( !::IsWindow(hWndOwner) )
				break;
			if( CWnd::FromHandlePermanent(hWndOwner) == NULL )
				break;

			// allow owner & frames to translate before IsDialogMessage does
			if (pOwner->PreTranslateMessage(pMsg))
				return TRUE;

			// try parent frames until there are no parent frames
			pOwner = pOwner->GetParentFrame();
		}
	
//		HWND hWndChild = ::GetWindow( m_hWnd, GW_CHILD );
//		if(		hWndChild != NULL
//			&&	pMsg->hwnd != NULL
//			&&	WM_KEYFIRST <= pMsg->message
//			&&	pMsg->message <= WM_KEYLAST
//			&&	::IsChild( hWndChild, pMsg->hwnd )
//			)
//		{
//			::TranslateMessage( pMsg );
//			::DispatchMessage( pMsg );
//			return TRUE;
//		}
	} // if( !m_bPresubclassDialogMode )

	return FALSE;

//	// filter both messages to dialog and from children
//	return PreTranslateInput(pMsg);
}

__EXT_MFC_INT_PTR CExtControlBar::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	) const
{
	ASSERT_VALID( this );

	if(		CExtToolControlBar::g_bMenuTracking
		||	CExtPopupMenuWnd::IsMenuTracking()
		||	OnAdvancedPopupMenuTipWndGet() != NULL
		||	m_bSuspendTips
		)
		return -1;

CPoint ptScreen( point );
	ClientToScreen( &ptScreen );
CExtBarNcAreaButton * pBtn = NULL;
	( const_cast < CExtControlBar * > ( this ) ) ->
		NcButtons_HitTest( ptScreen, &pBtn );
	if( pBtn != NULL )
	{
		ASSERT_VALID( pBtn );
		return pBtn->OnToolHitTest( point, pTI );
	}

	return -1; //CControlBar::OnToolHitTest( point, pTI );
}

CExtPopupMenuTipWnd * CExtControlBar::OnAdvancedPopupMenuTipWndGet(
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	pHelperSrc;
	lParam;
	if( ! g_bUseAdvancedToolTips )
		return NULL;
	if( m_bSuspendTips )
		return NULL;
	if( ( ( ( const_cast < CExtControlBar * > ( this ) ) -> GetBarStyle() ) & CBRS_TOOLTIPS ) == 0 )
		return NULL;
	return (&( CExtPopupMenuSite::g_DefPopupMenuSite.GetTip() ));
}

void CExtControlBar::OnAdvancedPopupMenuTipWndDisplay(
	CExtPopupMenuTipWnd & _ATTW,
	const RECT & rcExcludeArea,
	__EXT_MFC_SAFE_LPCTSTR strTipText,
	CObject * pHelperSrc, // = NULL
	LPARAM lParam // = 0
	) const
{
	ASSERT_VALID( this );
	ASSERT( strTipText != NULL && _tcslen( strTipText ) > 0 );
	pHelperSrc;
	lParam;
	if(		IsFixedMode()
		&&	IsFixedDockStyle()
		&&	(	(	m_pDockSite != NULL
				&&	GetSafeDockBarDlgCtrlID() == AFX_IDW_DOCKBAR_BOTTOM
				)
			||	( ( ( const_cast < CExtControlBar * > ( this ) ) -> GetBarStyle() ) & CBRS_ALIGN_BOTTOM ) != 0
			)
		)
		_ATTW.SetTipStyle( CExtPopupMenuTipWnd::__ETS_INV_RECTANGLE_NO_ICON );
	else
		_ATTW.SetTipStyle( CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON );
	_ATTW.SetText( strTipText );
	_ATTW.Show( (CWnd*)this, rcExcludeArea );
}

HWND CExtControlBar::FindHwndNotSafeForMutualPaint(
	CWnd * pWndStartSearch
	)
{
	ASSERT_VALID( pWndStartSearch );

	if( ( pWndStartSearch->GetStyle() & WS_VISIBLE ) == 0 )
		return NULL;

CFrameWnd * pFrame = DockSiteFromWnd( pWndStartSearch );
	if( pFrame != NULL )
	{
		ASSERT_VALID( pFrame );
		if( ( pFrame->GetStyle() & WS_VISIBLE ) == 0 )
			return NULL;
//#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
//		for(	HWND hWnd =
//					::GetWindow(
//						pFrame->GetSafeHwnd(),
//						GW_CHILD
//						);
//				hWnd != NULL;
//				hWnd =
//					::GetWindow(
//						hWnd,
//						GW_HWNDNEXT
//						)
//				)
//		{
//			CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
//			if( pWnd == NULL )
//				continue;
//			CExtDynAutoHideSlider * pSlider =
//				DYNAMIC_DOWNCAST( CExtDynAutoHideSlider, pWnd );
//			if( pSlider != NULL )
//			{
//				if( (pSlider->GetStyle() & WS_VISIBLE) == 0 )
//					break;
//				if( pSlider->m_nResizingMode != HTCLIENT )
//					return hWnd;
//			}
//		}
//#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	} // if( pFrame != NULL )


HWND hWndRetVal = NULL;
//CSingleLock _slCsCB( &g_csCB );
//	_slCsCB.Lock();
CExtControlBar * pBar = _DraggingGetBar();
	if( pBar != NULL )
	{
		if( pBar->_IsShowContentWhenDragging() )
		{
#if (!defined __EXT_MFC_NO_DOCK_MARKERS)
			if(		(! pBar->IsFixedMode() )
				&&	(! pBar->IsFixedDockStyle() )
				&&	INT(pBar->OnQueryDockingType()) >= INT(__RESIZABLE_DOCKING_TYPE_STUDIO_2005)
				&&	( ! g_PaintManager.m_bLayeredHighlighting2005 )
				)
			{
				POINT ptCursor;
				if( ::GetCursorPos( &ptCursor ) )
				{
					HWND hWndFromPoint = ::WindowFromPoint( ptCursor );
					if( hWndFromPoint != NULL )
					{
						ASSERT( ::IsWindow(hWndFromPoint) );
						CWnd * pWnd = CWnd::FromHandlePermanent( hWndFromPoint );
						if( pWnd != NULL )
						{
							CExtDynDockMarkerWnd * pDockMarkerWnd =
								DYNAMIC_DOWNCAST( CExtDynDockMarkerWnd, pWnd );
							if( pDockMarkerWnd != NULL )
								hWndRetVal = pBar->GetSafeHwnd();
						} // if( pWnd != NULL )
					} // if( hWndFromPoint != NULL )
				} // if( ::GetCursorPos( &ptCursor ) )
			}
#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)
		} // if( pBar->_IsShowContentWhenDragging() )
		else
			hWndRetVal = pBar->GetSafeHwnd();
	} // if( pBar != NULL )
	else
	{
		int nCountOfBars = (int)g_AllBars.GetSize();
		for( int nBar = 0; nBar < nCountOfBars; nBar++ )
		{
			pBar = g_AllBars[nBar];
			ASSERT_KINDOF( CExtControlBar, pBar );
			if( pBar->GetSafeHwnd() == NULL )
				continue;
			if( pBar->m_bRowRecalcing
				&& ( !pBar->_IsShowContentWhenRowRecalcing() )
				)
			{
				hWndRetVal = pBar->GetSafeHwnd();
				break;
			}
			if( pBar->m_bRowResizing
				&& ( !pBar->_IsShowContentWhenRowResizing() )
				)
			{
				hWndRetVal = pBar->GetSafeHwnd();
				break;
			}
		} // for( int nBar = 0; nBar < nCountOfBars; nBar++ )
	} // else from if( pBar != NULL )
//	_slCsCB.Unlock();

	return hWndRetVal;
}

CFrameWnd * CExtControlBar::DockSiteFromWnd(
	CWnd * pWndStartSearch
	)
{
	ASSERT_VALID( pWndStartSearch );
CFrameWnd * pFrame =
		DYNAMIC_DOWNCAST(
			CFrameWnd,
			pWndStartSearch
			);
	if( pFrame != NULL )
		return pFrame;
	pFrame = pWndStartSearch->GetParentFrame();
	if( pFrame == NULL )
		return NULL;
	ASSERT_VALID( pFrame );
	if( ! pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
		return pFrame;
	pFrame = pFrame->GetParentFrame();
	if( pFrame == NULL )
		return NULL;
	ASSERT_VALID( pFrame );
	ASSERT( !pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) );
	return pFrame;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

void CExtControlBar::OnCustomizeModeEnter()
{
	ASSERT_VALID( this );
	if( IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
		return;
CWnd * pWndChild = GetWindow( GW_CHILD );
	if( pWndChild->GetSafeHwnd() != NULL )
		pWndChild->EnableWindow( FALSE );
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	else if( AutoHideModeGet() )
	{
		ASSERT_VALID( m_pDockBar );
		ASSERT_KINDOF( CExtDockBar, m_pDockBar );
		CExtDynAutoHideArea * pWndAutoHideArea =
			((CExtDockBar*)m_pDockBar)->_GetAutoHideArea();
		if(		pWndAutoHideArea != NULL
			&&	pWndAutoHideArea->m_pWndSlider != NULL
			)
		{
			ASSERT_VALID( pWndAutoHideArea );
			ASSERT_VALID( pWndAutoHideArea->m_pWndSlider );
			pWndChild = pWndAutoHideArea->m_pWndSlider->GetWindow( GW_CHILD );
			if( pWndChild->GetSafeHwnd() != NULL )
				pWndChild->EnableWindow( FALSE );
		} // if( pWndAutoHideArea != NULL ...
	} // else if( AutoHideModeGet() )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
}

void CExtControlBar::OnCustomizeModeLeave()
{
	ASSERT_VALID( this );
	if( IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
		return;
CWnd * pWndChild = GetWindow( GW_CHILD );
	if( pWndChild->GetSafeHwnd() != NULL )
		pWndChild->EnableWindow( TRUE );
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	else if( AutoHideModeGet() )
	{
		ASSERT_VALID( m_pDockBar );
		ASSERT_KINDOF( CExtDockBar, m_pDockBar );
		CExtDynAutoHideArea * pWndAutoHideArea =
			((CExtDockBar*)m_pDockBar)->_GetAutoHideArea();
		if(		pWndAutoHideArea != NULL
			&&	pWndAutoHideArea->m_pWndSlider != NULL
			)
		{
			ASSERT_VALID( pWndAutoHideArea );
			ASSERT_VALID( pWndAutoHideArea->m_pWndSlider );
			pWndChild = pWndAutoHideArea->m_pWndSlider->GetWindow( GW_CHILD );
			if( pWndChild->GetSafeHwnd() != NULL )
				pWndChild->EnableWindow( TRUE );
		} // if( pWndAutoHideArea != NULL ...
	} // else if( AutoHideModeGet() )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
}

void CExtControlBar::OnCustomizeListBoxUpdate( CExtCustomizeToolbarsListBox * pWndList )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pWndList );
	if( !m_bVisibleInCustomizeListBox )
		return;
	pWndList->AddBar( this );
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

bool CExtControlBar::_IsDockSiteCustomizeMode() const
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = NotifiedCustomizeSiteGet();
	if( pSite != NULL )
		return pSite->IsCustomizeMode();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return false;
}

void CExtControlBar::_ActivateOnClick()
{
	ASSERT_VALID( this );
HWND hWndOwn = GetSafeHwnd();
	if(		hWndOwn == NULL
		||	(! ::IsWindow( hWndOwn ) )
		)
		return;
HWND hWndActivate = NULL;
	if( m_bPresubclassDialogMode )
		hWndActivate = ::GetParent( hWndOwn );
	else
		hWndActivate = _GetDockingFrameImpl()->GetSafeHwnd();
	if( hWndActivate == NULL )
	{
		if( stat_QueryFocusChangingEnabled( this, hWndOwn ) )
			::SetFocus( hWndOwn );
		return;
	}
CWnd * pWndPermanent =
		CWnd::FromHandlePermanent( hWndActivate );
	if( pWndPermanent != NULL )
	{
		CMDIChildWnd * pWndMDIChild =
			DYNAMIC_DOWNCAST( CMDIChildWnd, pWndPermanent );
		if( pWndMDIChild != NULL )
		{
			CFrameWnd * pWndFrame =
				pWndMDIChild->GetParentFrame();
			if( pWndFrame != NULL )
			{
				CMDIFrameWnd * pWndMDIFrame =
					DYNAMIC_DOWNCAST( CMDIFrameWnd, pWndFrame );
				if( pWndMDIFrame != NULL )
				{
					CMDIChildWnd * pActive =
						pWndMDIFrame->MDIGetActive();
					if( pWndMDIChild != pActive )
						pWndMDIChild->MDIActivate();
					return;
				}
			}
		}
	}
HWND hWndFocus = ::GetFocus();
	if( hWndFocus == NULL )
	{
		if( stat_QueryFocusChangingEnabled( this, hWndActivate ) )
			::SetFocus( hWndActivate );
		return;
	}
	if(		hWndActivate != hWndFocus
		||	(! ::IsChild(hWndActivate, hWndFocus) )
		)
	{
		if( ! OnQueryBarHoverProcessingEnabled() )
		{
			if( stat_QueryFocusChangingEnabled( this, hWndActivate ) )
				::SetFocus( hWndActivate );
		}
	}
}

void CExtControlBar::_OnNcMiniFrameActivate( bool bActive )
{
	ASSERT_VALID( this );
	bActive;
}

CExtControlBar::QUERY_REPOSITION_CALC_EFFECT_DATA::
	QUERY_REPOSITION_CALC_EFFECT_DATA(
		SHORT nFlags,
		CWnd * pWndToReposChilds,
		UINT nIDFirst,
		UINT nIDLast,
		UINT nIDLeftOver,
		LPRECT lpRectParam,
		LPCRECT lpcRectClient,
		bool bStretch
		)
	: m_nFlags( nFlags )
	, m_pWndToReposChilds( pWndToReposChilds )
	, m_nIDFirst( nIDFirst )
	, m_nIDLast( nIDLast )
	, m_nIDLeftOver( nIDLeftOver )
	, m_lpRectParam( lpRectParam )
	, m_lpcRectClient( lpcRectClient )
	, m_bStretch( bStretch )
{
	ASSERT_VALID( m_pWndToReposChilds );
	ASSERT( m_pWndToReposChilds->GetSafeHwnd() != NULL );
	ASSERT( ((int)__PRCF_WND_REPOS_DEFAULT) == ((int)CWnd::reposDefault) );
	ASSERT( ((int)__PRCF_WND_REPOS_QUERY) == ((int)CWnd::reposQuery) );
	ASSERT( ((int)__PRCF_WND_REPOS_EXTRA) == ((int)CWnd::reposExtra) );
}

CExtControlBar::QUERY_REPOSITION_CALC_EFFECT_DATA::
	~QUERY_REPOSITION_CALC_EFFECT_DATA()
{
}

void CExtControlBar::QUERY_REPOSITION_CALC_EFFECT_DATA::
	QueryTypeSet( SHORT nQueryType )
{
const SHORT nTest = SHORT(__PRCF_WND_REPOS_MASK);
	nQueryType &= nTest;
	m_nFlags &= ~nTest;
	m_nFlags |= nQueryType;
}

SHORT CExtControlBar::QUERY_REPOSITION_CALC_EFFECT_DATA::
	QueryTypeGet() const
{
const SHORT nTest = SHORT(__PRCF_WND_REPOS_MASK);
SHORT nRetVal = SHORT( m_nFlags & nTest );
	return nRetVal;
}

bool CExtControlBar::QUERY_REPOSITION_CALC_EFFECT_DATA::
	IsQueryReposDefault() const
{
bool bRetVal =
		( ((int)QueryTypeGet()) == ((int)CWnd::reposDefault) )
			? true : false;
	return bRetVal;
}

bool CExtControlBar::QUERY_REPOSITION_CALC_EFFECT_DATA::
	IsQueryReposQuery() const
{
bool bRetVal =
		( ((int)QueryTypeGet()) == ((int)CWnd::reposQuery) )
			? true : false;
	return bRetVal;
}

bool CExtControlBar::QUERY_REPOSITION_CALC_EFFECT_DATA::
	IsQueryReposExtra() const
{
bool bRetVal =
		( ((int)QueryTypeGet()) == ((int)CWnd::reposExtra) )
			? true : false;
	return bRetVal;
}

CExtControlBar::QUERY_REPOSITION_CALC_EFFECT_DATA::
	operator WPARAM() const
{
	return ( (WPARAM) this );
}

void CExtControlBar::QUERY_REPOSITION_CALC_EFFECT_DATA::
	ExcludeFromCenterSet(
		bool bExclude // = true
		)
{
const SHORT nTest = SHORT(__PRCF_EXCLUDE_FROM_CENTER);
	if( bExclude )
		m_nFlags |= nTest;
	else
		m_nFlags &= ~nTest;
}

bool CExtControlBar::QUERY_REPOSITION_CALC_EFFECT_DATA::
	ExcludeFromCenterGet() const
{
const SHORT nTest = SHORT(__PRCF_EXCLUDE_FROM_CENTER);
bool bRetVal = ( (m_nFlags&nTest) != 0 ) ? true : false;
	return bRetVal;
}

void CExtControlBar::RepositionBarsEx(
	CWnd * pWndToReposChilds,
	UINT nIDFirst,
	UINT nIDLast,
	UINT nIDLeftOver,
	UINT nFlag, // = reposDefault
	LPRECT lpRectParam, // = NULL
	LPCRECT lpRectClient, // = NULL
	BOOL bStretch // = TRUE
	)
{
	ASSERT_VALID( pWndToReposChilds );

	ASSERT( nFlag == 0 || nFlag == reposQuery || nFlag == reposExtra );

AFX_SIZEPARENTPARAMS layout;
HWND hWndLeftOver = NULL;

	layout.bStretch = bStretch;
	layout.sizeTotal.cx = layout.sizeTotal.cy = 0;
	if( lpRectClient != NULL )
		layout.rect = *lpRectClient;    // starting rect comes from parameter
	else
		pWndToReposChilds->GetClientRect(
			&layout.rect // starting rect comes from client rect
			);
	ASSERT( layout.rect.left <= layout.rect.right );

	if( nFlag != 1 )
		layout.hDWP = ::BeginDeferWindowPos(8); // reasonable guess
	else
		layout.hDWP = NULL; // not actually doing layout

	for(	HWND hWndChild = ::GetTopWindow( pWndToReposChilds->m_hWnd );
			hWndChild != NULL;
			hWndChild = ::GetWindow( hWndChild, GW_HWNDNEXT )
			)
	{
		UINT nIDC = _AfxGetDlgCtrlID(hWndChild);
		CWnd * pWnd = CWnd::FromHandlePermanent( hWndChild );
		if( nIDC == nIDLeftOver )
			hWndLeftOver = hWndChild;
		else if( pWnd != NULL )
		{
			QUERY_REPOSITION_CALC_EFFECT_DATA _qrced(
				(SHORT)nFlag,
				pWndToReposChilds,
				nIDFirst,
				nIDLast,
				nIDLeftOver,
				lpRectParam,
				lpRectClient,
				bStretch ? true : false
				);
			pWnd->SendMessage(
				g_nMsgQueryRepositionCalcEffect,
				_qrced,
				0L
				);
			if( nFlag == 1 && _qrced.ExcludeFromCenterGet() )
			{
				ASSERT( _qrced.IsQueryReposQuery() );
				continue;
			}
			if(
				nIDC >= nIDFirst
				&& nIDC <= nIDLast
				)
				::SendMessage(
					hWndChild,
					WM_SIZEPARENT,
					0,
					(LPARAM)&layout
					);
		}
	}

	// if just getting the available rectangle, return it now...
	if( nFlag == 1 )
	{
		ASSERT(lpRectParam != NULL);
		if( bStretch )
			::CopyRect( lpRectParam, &layout.rect );
		else
		{
			lpRectParam->left = lpRectParam->top = 0;
			lpRectParam->right = layout.sizeTotal.cx;
			lpRectParam->bottom = layout.sizeTotal.cy;
		}
		return;
	}

	// the rest is the client size of the left-over pane
	if( nIDLeftOver != 0 && hWndLeftOver != NULL )
	{
		CWnd* pLeftOver = CWnd::FromHandle(hWndLeftOver);
		// allow extra space as specified by lpRectBorder
		if( nFlag == 2 )
		{
			ASSERT(lpRectParam != NULL);
			layout.rect.left += lpRectParam->left;
			layout.rect.top += lpRectParam->top;
			layout.rect.right -= lpRectParam->right;
			layout.rect.bottom -= lpRectParam->bottom;
		}
		// reposition the window
		pLeftOver->CalcWindowRect( &layout.rect );
		AfxRepositionWindow( &layout, hWndLeftOver, &layout.rect );
	}

	// move and resize all the windows at once!
	if( layout.hDWP == NULL
		|| !::EndDeferWindowPos( layout.hDWP )
		)
		TRACE0("Warning: DeferWindowPos failed - low system resources.\n");
}

void CExtControlBar::_PreSyncRowMetrics()
{
	ASSERT_VALID( this );
	if( IsVisible()
		|| IsFloating()
		)
		return;

	//m_nDelayedRowUpdateMetric = 0;

ExtControlBarVector_t vRow;
	_GetRowExtBars( vRow );
INT nCount = (INT)vRow.GetSize();
INT i = 0;
	for( i = 0; i < nCount; )
	{
		CExtControlBar * pBar = vRow[ i ];
		ASSERT_VALID( pBar );
		if( !pBar->IsVisible()
			|| pBar->IsFixedMode()
			|| pBar->IsFixedDockStyle()
			|| pBar == this
			)
		{
			vRow.RemoveAt( i );
			nCount--;
		}
		else
			i++;
	}
UINT nDockbarID = GetSafeDockBarDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID( nDockbarID );
bool bHorz =
		(nDockbarID == AFX_IDW_DOCKBAR_TOP || nDockbarID == AFX_IDW_DOCKBAR_BOTTOM)
			? true : false;
LONG nOwnMetric = bHorz ? m_sizeDockedH.cx : m_sizeDockedV.cy;
	if( nOwnMetric == 0 )
	{
		nOwnMetric = bHorz ? _CalcDesiredMinHW() : _CalcDesiredMinVH();
		(bHorz ? m_sizeDockedH.cx : m_sizeDockedV.cy) = nOwnMetric;
	}

LONG & nCrossMetricOwnRef = bHorz ? m_sizeDockedH.cy : m_sizeDockedV.cx;
LONG nCrossMetricOther = -1;

	for( i = 0; i < nCount && nOwnMetric > 0; )
	{
		CExtControlBar * pBar = vRow[ i ];
		//pBar->m_nDelayedRowUpdateMetric = 0;
		LONG & nRefBarMetric = bHorz ? pBar->m_sizeDockedH.cx : pBar->m_sizeDockedV.cy;
		LONG nBarMinMetric = bHorz ? pBar->_CalcDesiredMinHW() : pBar->_CalcDesiredMinVH();
		if( nCrossMetricOther <= 0 )
			nCrossMetricOther = bHorz ? pBar->m_sizeDockedH.cy : pBar->m_sizeDockedV.cx;
		if( nRefBarMetric <= nBarMinMetric )
		{
			vRow.RemoveAt( i );
			nCount--;
			if( nCount == 0 )
				break;
			continue;
		}
		nRefBarMetric --;
		nOwnMetric --;
		ASSERT( nOwnMetric >= 0 );
		if( nOwnMetric == 0 )
			break;
		i++;
		ASSERT( i <= nCount );
		if( i == nCount )
			i = 0;
	}

	if( nCrossMetricOther > 0 )
		nCrossMetricOwnRef = nCrossMetricOther;
}

void CExtControlBar::OnFrameBarCheckCmd(
	bool bResizableCheckmarks // = false
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pDockSite );

	ASSERT(ID_VIEW_STATUS_BAR == AFX_IDW_STATUS_BAR);
	ASSERT(ID_VIEW_TOOLBAR == AFX_IDW_TOOLBAR);
	ASSERT(ID_VIEW_REBAR == AFX_IDW_REBAR);

CFrameWnd * pParentFrame = GetParentFrame();

//	if(		bResizableCheckmarks // (- v.2.23)
//		||	IsFixedDockStyle()
//		)
bool bVisible = ((GetStyle() & WS_VISIBLE) != 0) ? true : false;
	if(		IsFixedDockStyle()
		||	( bResizableCheckmarks && bVisible ) // (+ v.2.23)
		)
	{
		m_pDockSite->ShowControlBar(
			this,
			bVisible ? FALSE : TRUE,
			FALSE
			);
//		SetTimer( __TIMER_ID_DELAYED_UPDATE, 150, NULL );
		return;
	}

	ASSERT( ! IsFixedDockStyle() );

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( AutoHideModeGet() )
	{
		ASSERT_VALID( m_pDockBar );
		ASSERT_KINDOF( CExtDockBar, m_pDockBar );
		CExtDynAutoHideArea * pWndAutoHideArea =
			((CExtDockBar*)m_pDockBar)->_GetAutoHideArea();
		ASSERT_VALID( pWndAutoHideArea );
		ASSERT_VALID( pWndAutoHideArea->m_pWndSlider );
		if(		pWndAutoHideArea->m_pWndSlider->m_bActiveState
			&&	pWndAutoHideArea->m_pWndSlider->_GetBar() == this
			)
			return;
		LONG nIndex = pWndAutoHideArea->FindControlBar( this );
		if( nIndex < 0 )
		{
			ASSERT( FALSE );
			return;
		}
		pWndAutoHideArea->SelectionSet( nIndex, true, true );
//		ASSERT( pWndAutoHideArea->m_pWndSlider->m_bActiveState );
//		ASSERT( pWndAutoHideArea->m_pWndSlider->m_pBar == this );
//		ASSERT( pWndAutoHideArea->m_pWndSlider->GetStyle() & WS_VISIBLE );
		if( stat_QueryFocusChangingEnabled( this, pWndAutoHideArea->m_pWndSlider->m_hWnd ) )
			pWndAutoHideArea->m_pWndSlider->SetFocus();
//		SetTimer( __TIMER_ID_DELAYED_UPDATE, 150, NULL );
		return;
	}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	if( ! bVisible )
	{
		_DisplayingSet( true );
		m_pDockSite->ShowControlBar( this, TRUE, FALSE );
		CExtMiniDockFrameWnd * pMiniFrame = DYNAMIC_DOWNCAST( CExtMiniDockFrameWnd, pParentFrame );
		if( pMiniFrame != NULL )
		{
			_UpdateVisibilityInChain();
			pMiniFrame->RecalcLayout();
		}
	} // if( ! bVisible )

	ASSERT_VALID( m_pDockBar );
//	ASSERT_KINDOF( CExtDockBar, m_pDockBar );

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( ((CExtDockBar*)m_pDockBar)->IsKindOf(RUNTIME_CLASS(CExtDockDynTabBar)) )
	{
		CExtDynTabControlBar * pTabbed = 
			STATIC_DOWNCAST(
				CExtDynTabControlBar,
				m_pDockBar->GetParent()
				);
CRect rc;
pTabbed->GetWindowRect( &rc );
CSize _size = rc.Size();
pTabbed->SetInitDesiredSizeFloating( _size );
pTabbed->SetInitDesiredSizeHorizontal( _size );
pTabbed->SetInitDesiredSizeVertical( _size );
pTabbed->m_pDockContext->m_sizeLast = _size;
		pTabbed->SelectControlBar( this );
pTabbed->_DisplayingSet( true );
		pParentFrame->DelayRecalcLayout();
		if( stat_QueryFocusChangingEnabled( this, m_hWnd ) )
			SetFocus();
//		SetTimer( __TIMER_ID_DELAYED_UPDATE, 150, NULL );
		return;
	}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	pParentFrame->DelayRecalcLayout();
	if( pParentFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
	{
		pParentFrame->BringWindowToTop();
		if( IsFloating() && ( ! IsFixedMode() ) )
		{
			//pParentFrame->RecalcLayout();
			CRect rcCF;
			pParentFrame->GetClientRect( &rcCF );
			pParentFrame->GetDlgItem( AFX_IDW_DOCKBAR_FLOAT )->MoveWindow( &rcCF );
			MoveWindow( &rcCF );
		}
	}
	if( stat_QueryFocusChangingEnabled( this, m_hWnd ) )
		SetFocus();
//	SetTimer( __TIMER_ID_DELAYED_UPDATE, 150, NULL );
}

void CExtControlBar::OnFrameBarCheckUpdate(
	CCmdUI * pCmdUI,
	bool bResizableCheckmarks // = false
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pDockSite );

	ASSERT(ID_VIEW_STATUS_BAR == AFX_IDW_STATUS_BAR);
	ASSERT(ID_VIEW_TOOLBAR == AFX_IDW_TOOLBAR);
	ASSERT(ID_VIEW_REBAR == AFX_IDW_REBAR);

BOOL bEnabled = _ClosingIsEnabled( this ) ? TRUE : FALSE;
	pCmdUI->Enable( bEnabled );

	if(		bResizableCheckmarks
		||	IsFixedDockStyle()
		)
	{
		pCmdUI->SetCheck(
			(GetStyle() & WS_VISIBLE) != 0
			);
		return;
	}
}

BOOL CExtControlBar::DoFrameBarCheckCmd(
	CFrameWnd * pFrame,
	UINT nBarID,
	bool bResizableCheckmarks // = false
	)
{
	ASSERT_VALID( pFrame );

CControlBar * pBar = pFrame->GetControlBar( nBarID );
	if( pBar != NULL )
	{
		if( pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
			((CExtControlBar*)pBar)->
				OnFrameBarCheckCmd( bResizableCheckmarks );
		else
			pFrame->ShowControlBar(
				pBar,
				(pBar->GetStyle() & WS_VISIBLE) == 0,
				FALSE
				);
		return TRUE;
	}
	return FALSE;
}

void CExtControlBar::DoFrameBarCheckUpdate(
	CFrameWnd * pFrame,
	CCmdUI * pCmdUI,
	bool bResizableCheckmarks // = false
	)
{
	ASSERT_VALID( pFrame );

CControlBar * pBar = pFrame->GetControlBar( pCmdUI->m_nID );
	if( pBar != NULL )
	{
		if( pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
			((CExtControlBar*)pBar)->
				OnFrameBarCheckUpdate( pCmdUI, bResizableCheckmarks );
		else
			pCmdUI->SetCheck(
				(pBar->GetStyle() & WS_VISIBLE) != 0
				);
		return;
	}

	pCmdUI->ContinueRouting();
}

void CExtControlBar::OnSetFocus(CWnd* pOldWnd) 
{
	CControlBar::OnSetFocus(pOldWnd);

	if( IsFixedMode() )
		return;
	
HWND hWndChild = ::GetWindow( GetSafeHwnd(), GW_CHILD );
	if( hWndChild != NULL )
	{
		if( stat_QueryFocusChangingEnabled( this, hWndChild ) )
			::SetFocus( hWndChild );
	}
}

bool CExtControlBar::OnConstructDockSiteControlBarPopupMenu(
	CExtPopupMenuWnd * pPopup
	) const
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT_VALID( pPopup );
	if( ! m_bAppearInDockSiteControlBarPopupMenu )
		return false;
	if( g_bLockAllBars )
		return false;
UINT nBarID = (UINT)GetDlgCtrlID();
	VERIFY( pPopup->ItemInsert( nBarID ) );	
	return true;
}

LRESULT CExtControlBar::OnHelpHitTest(WPARAM wParam, LPARAM lParam)
{
HWND hWndCapture = ::GetCapture();	
	if( hWndCapture != NULL && hWndCapture == GetSafeHwnd() )
		SendMessage( WM_CANCELMODE );
LRESULT lResult = DoHelpHitTest( CPoint(DWORD(lParam)) );
	if( lResult == -3L )
		return 0L;
	if( lResult == -2L )
		return -1L;
	if( lResult != -1L )
		return HID_BASE_COMMAND+lResult;
	return CControlBar::OnHelpHitTest(wParam,lParam);
}

LRESULT CExtControlBar::DoHelpHitTest( CPoint ptHelpHitTest )
{
	ASSERT_VALID( this );
	ptHelpHitTest;
	return -1L;
}

bool CExtControlBar::g_bUseCmdManagerForSetMessageString = true;
void CExtControlBar::stat_SetMessageString(
	CWnd * pWnd,
	UINT nCmdID // = AFX_IDS_IDLEMESSAGE
	)
{
	if( pWnd->GetSafeHwnd() == NULL )
		return;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( pWnd->m_hWnd );
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		nCmdID = AFX_IDS_IDLEMESSAGE;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

CFrameWnd * pFrame = DYNAMIC_DOWNCAST( CFrameWnd, pWnd );
	if( pFrame == NULL && (pWnd->GetStyle() & WS_CHILD) != 0 )
		pFrame = pWnd->GetParentFrame();
	if(		pFrame != NULL
		&&	pFrame->IsKindOf(RUNTIME_CLASS(CExtMiniDockFrameWnd))
		)
		pFrame = pFrame->GetParentFrame();

HWND hWnd = 
	pFrame != NULL 
		? pFrame->m_hWnd
		: pWnd->m_hWnd;

		if(		g_bUseCmdManagerForSetMessageString
			&&	nCmdID != AFX_IDS_IDLEMESSAGE
			)
		{
			__EXT_MFC_SAFE_LPCTSTR strProfile = g_CmdManager->ProfileNameFromWnd( pWnd->m_hWnd );
			if( strProfile != NULL )
			{
				CExtCmdItem * pCmdItem = g_CmdManager->CmdGetPtr( strProfile, nCmdID );
				if( pCmdItem != NULL )
				{
					CWnd * pTempBar =
						CWnd::FromHandlePermanent(
							::GetDlgItem( hWnd, AFX_IDW_STATUS_BAR )
							);
					if( pTempBar != NULL )
					{
						CStatusBar * pStatusBar =
							DYNAMIC_DOWNCAST( CStatusBar, pTempBar );
						if( pStatusBar != NULL )
						{
							int nIdlePaneIndex =
								pStatusBar->CommandToIndex( 0 );
							if( nIdlePaneIndex != -1)
								pStatusBar->SetPaneText(
									nIdlePaneIndex,
									pCmdItem->m_sTipStatus.IsEmpty()
										? _T("")
										: pCmdItem->m_sTipStatus
									);
							return;
						} // if( pStatusBar != NULL )
					} // if( pTempBar != NULL )
				} // if( pCmdItem != NULL )
			} // if( strProfile != NULL )
		} // if( g_bUseCmdManagerForSetMessageString ...
	::SendMessage( 
		hWnd,
		WM_SETMESSAGESTRING, 
		nCmdID, 0
		);
}

HWND CExtControlBar::stat_FindMdiClientHWND(
	HWND hWndMdiFrame
	)
{
	for(	HWND hWnd = ::GetWindow( hWndMdiFrame, GW_CHILD );
			hWnd != NULL;
			hWnd = ::GetWindow( hWnd, GW_HWNDNEXT )
		)
	{
		ASSERT( ::IsWindow( hWnd ) );
		TCHAR strClassName[ 512 ];
		::memset( strClassName, 0, sizeof(strClassName) );
		::GetClassName( hWnd, strClassName, 512 );
		if( _tcslen( strClassName ) == 0 )
			continue;
		__EXT_MFC_STRLWR( strClassName, 512 );
		//if( _tcscmp( strClassName, _T("mdiclient") ) == 0 )
		if( _tcsstr( strClassName, _T("mdiclient") ) != NULL )
			return hWnd;
	}
	return NULL;
}

bool CExtControlBar::stat_AdjustOccCompatibilityTree(
	CWnd * pWndRoot
	)
{
	if( pWndRoot->GetSafeHwnd() == NULL )
		return false;
	ASSERT_VALID( pWndRoot );
	if( pWndRoot->GetControlUnknown() != NULL )
		return true;
bool bOccMode = false;
CWnd * pWnd = pWndRoot->GetWindow( GW_CHILD );
	for( ; pWnd != NULL; pWnd = pWnd->GetWindow(GW_HWNDNEXT) )
	{
		ASSERT_VALID( pWnd );
		if( CWnd::FromHandlePermanent(pWnd->GetSafeHwnd()) == NULL )
			continue;
		bOccMode |= stat_AdjustOccCompatibilityTree( pWnd );
	} // for( ; pWnd != NULL; pWnd = pWnd->GetWindow(GW_HWNDNEXT) )
	if( bOccMode )
		pWndRoot->m_nFlags |= WF_OLECTLCONTAINER;
	else
		pWndRoot->m_nFlags &= ~(WF_OLECTLCONTAINER);
CFrameWnd * pDockSite = DYNAMIC_DOWNCAST( CFrameWnd, pWndRoot );
	if(		pDockSite != NULL
		&&	pDockSite->IsKindOf( RUNTIME_CLASS(CExtMiniDockFrameWnd) )
		)
		pDockSite = NULL;
	if( pDockSite != NULL )
	{
		CMap
			<	CExtMiniDockFrameWnd *,
				CExtMiniDockFrameWnd *,
				char,
				char
				>
			setMiniFrames;
		POSITION pos = pDockSite->m_listControlBars.GetHeadPosition();
		for( ; pos != NULL; )
		{
			CControlBar * pControlBar = (CControlBar *)
				pDockSite->m_listControlBars.GetNext( pos );
			if( pControlBar->m_pDockSite == NULL )
				continue;
			ASSERT_VALID( pControlBar );
			ASSERT_KINDOF( CControlBar, pControlBar );
			if( pControlBar->IsDockBar() )
				continue;
			CFrameWnd * pBarParentFrame =
				pControlBar->GetParentFrame();
			ASSERT_VALID( pBarParentFrame );
			if( pBarParentFrame == pDockSite )
				continue;
			CExtMiniDockFrameWnd * pFloatingFrame = DYNAMIC_DOWNCAST( CExtMiniDockFrameWnd, pBarParentFrame );
			if( pFloatingFrame == NULL )
				continue;
			setMiniFrames.SetAt( pFloatingFrame, 0 );
		} // for( ; pos != NULL; )
		pos = setMiniFrames.GetStartPosition();
		for( ; pos != NULL; )
		{
			CExtMiniDockFrameWnd * pFloatingFrame = NULL;
			char nDummy;
			setMiniFrames.GetNextAssoc(pos,pFloatingFrame,nDummy);
			stat_AdjustOccCompatibilityTree( pFloatingFrame );
		} // for( ; pos != NULL; )
	}
	return bOccMode;
}

void CExtControlBar::stat_CompressFrameLayout(
	CFrameWnd * pDockSite,
	CSize sizeFreeSpaceAtCenterRequired, // = CSize(100,100)
	bool bCompressInnerFirst, // = true
	bool bCompressProportionally, // = false
	bool bRecalcLayout, // = true
	bool bDelayRecalcLayout, // = true
	UINT nCenterDlgCtrlID // = AFX_IDW_PANE_FIRST
	)
{
	if(		sizeFreeSpaceAtCenterRequired.cx < 0
		||	sizeFreeSpaceAtCenterRequired.cy < 0
		||	pDockSite->GetSafeHwnd() == NULL
		||	pDockSite->IsKindOf( RUNTIME_CLASS(CMiniDockFrameWnd) )
		)
	{ // incorrect parameter(s)
		ASSERT( FALSE );
		return;
	} // incorrect parameter(s)
	ASSERT_VALID( pDockSite );
CRect rcFrameInner;
	pDockSite->GetClientRect( &rcFrameInner );
	RepositionBarsEx(
		pDockSite,
		0,
		0x0FFFF,
		nCenterDlgCtrlID,
		CWnd::reposQuery,
		rcFrameInner,
		rcFrameInner
		);
bool bLayoutChanged = false;
CSize sizeFreeSpaceAtCenterCurrent = rcFrameInner.Size();
	for( int nSpaceType = 0; nSpaceType <= 1; nSpaceType ++ )
	{ // nSpaceType==0 - horizontal space search, nSpaceType==1 - vertical space search
		bool bHorzSpaceSearch = ( nSpaceType == 0 ) ? true : false;
		LONG nSizeRequired = bHorzSpaceSearch ? sizeFreeSpaceAtCenterRequired.cx : sizeFreeSpaceAtCenterRequired.cy;
		LONG nSizeCurrent  = bHorzSpaceSearch ? sizeFreeSpaceAtCenterCurrent.cx  : sizeFreeSpaceAtCenterCurrent.cy;
		if( nSizeRequired <= nSizeCurrent )
			continue;
		LONG nSizeAvailable = 0;
		UINT nDockBarID1 = bHorzSpaceSearch ? AFX_IDW_DOCKBAR_LEFT : AFX_IDW_DOCKBAR_TOP;
		UINT nDockBarID2 = bHorzSpaceSearch ? AFX_IDW_DOCKBAR_RIGHT : AFX_IDW_DOCKBAR_BOTTOM;
		CList < CExtDockBar *, CExtDockBar * > listDockBars;
		CList < LONG, LONG > listAvailableSizes;
		CWnd * pWnd = pDockSite->GetWindow( GW_CHILD );
		for( ; pWnd != NULL; pWnd = pWnd->GetWindow( GW_HWNDNEXT ) )
		{
			CExtDockBar * pDockBar = DYNAMIC_DOWNCAST( CExtDockBar, pWnd );
			if( pDockBar == NULL || pDockBar->IsKindOf( RUNTIME_CLASS(CExtDockOuterBar) ) )
				continue;
			UINT nDockBarID = pDockBar->GetDlgCtrlID();
			if( ! ( nDockBarID == nDockBarID1 || nDockBarID == nDockBarID2 ) )
				continue;
			if( ( pDockBar->GetStyle() & WS_VISIBLE ) == 0 )
				continue;
			CRect rcDockBarWnd;
			pDockBar->GetWindowRect( &rcDockBarWnd );
			LONG nDockBarMinSize = 0, nDockBarCurrentSize = bHorzSpaceSearch ? rcDockBarWnd.Width() : rcDockBarWnd.Height();
			CWnd * pTestBar = pDockBar->GetWindow( GW_CHILD );
			for( ; pTestBar != NULL; pTestBar = pTestBar->GetWindow( GW_HWNDNEXT ) )
			{
				CExtControlBar * pBar = DYNAMIC_DOWNCAST( CExtControlBar, pTestBar );
				if( pBar == NULL )
					continue;
				if( ! pBar->IsVisible() )
					continue;
				LONG nBarMinSize = bHorzSpaceSearch ? pBar->_CalcDesiredMinHW() : pBar->_CalcDesiredMinVH();
				nDockBarMinSize = max( nDockBarMinSize, nBarMinSize );
			} // for( ; pTestBar != NULL; pTestBar = pTestBar->GetWindow( GW_HWNDNEXT ) )
			LONG nDockBarAvailableSize = nDockBarCurrentSize - nDockBarMinSize;
			if( nDockBarAvailableSize <= 0 )
				continue;
			nSizeAvailable += nDockBarAvailableSize;
			if( bCompressInnerFirst )
			{
				listDockBars.AddHead( pDockBar );
				listAvailableSizes.AddHead( nDockBarAvailableSize );
			}
			else
			{
				listDockBars.AddTail( pDockBar );
				listAvailableSizes.AddTail( nDockBarAvailableSize );
			}
		} // for( ; pWnd != NULL; pWnd = pWnd->GetWindow( GW_HWNDNEXT ) )
		LONG nSizeToCompress = min( nSizeAvailable, nSizeRequired );
		if( nSizeToCompress <= 0 )
			continue;
		bLayoutChanged = true;
		ASSERT( listDockBars.GetCount() == listAvailableSizes.GetCount() );
		CMap < CExtDockBar *, CExtDockBar *, LONG, LONG > mapSizesToCompress;
		for( ; nSizeToCompress > 0; )
		{
			POSITION posDockBar = listDockBars.GetHeadPosition();
			POSITION posAvailableSize = listAvailableSizes.GetHeadPosition();
			LONG nStepResult = 0;
			for( ; posDockBar != NULL && nSizeToCompress > 0; )
			{
				CExtDockBar * pDockBar = listDockBars.GetNext( posDockBar );
				LONG nDockBarSizeCompressed = 0, nDockBarAvailableSize = listAvailableSizes.GetNext( posAvailableSize );
				if( ! mapSizesToCompress.Lookup( pDockBar, nDockBarSizeCompressed ) )
					nDockBarSizeCompressed = 0;
				nDockBarAvailableSize -= nDockBarSizeCompressed;
				ASSERT( nDockBarAvailableSize >= 0 );
				if( nDockBarAvailableSize == 0 )
					continue;
				if( bCompressProportionally )
				{
					nDockBarSizeCompressed ++;
					nSizeToCompress --;
					nStepResult ++;
				}
				else
				{
					LONG nCompress = min( nDockBarAvailableSize, nSizeToCompress );
					nDockBarSizeCompressed += nCompress;
					nSizeToCompress -= nCompress;
					nStepResult += nCompress;
				}
				mapSizesToCompress.SetAt( pDockBar, nDockBarSizeCompressed );
				ASSERT( nSizeToCompress >= 0 );
			} // for( ; posDockBar != NULL && nSizeToCompress > 0; )
			ASSERT( nStepResult >= 0 );
			if( nStepResult == 0 )
				break;
		} // for( ; nSizeToCompress > 0; )
		POSITION pos = mapSizesToCompress.GetStartPosition();
		for( ; pos != NULL ; )
		{
			CExtDockBar * pDockBar = NULL;
			LONG nDockBarSizeCompressed = 0;
			mapSizesToCompress.GetNextAssoc( pos, pDockBar, nDockBarSizeCompressed );
			CWnd * pTestBar = pDockBar->GetWindow( GW_CHILD );
			for( ; pTestBar != NULL; pTestBar = pTestBar->GetWindow( GW_HWNDNEXT ) )
			{
				CExtControlBar * pBar = DYNAMIC_DOWNCAST( CExtControlBar, pTestBar );
				if( pBar == NULL )
					continue;
				//if( ! pBar->IsVisible() )
				//	continue;
				CRect rcBarWnd;
				pBar->GetWindowRect( &rcBarWnd );
				CSize sizeBar = rcBarWnd.Size();
				if( bHorzSpaceSearch )
				{
					sizeBar.cx -= nDockBarSizeCompressed;
					pBar->SetInitDesiredSizeVertical( sizeBar );
				}
				else
				{
					sizeBar.cy -= nDockBarSizeCompressed;
					pBar->SetInitDesiredSizeHorizontal( sizeBar );
				}
			} // for( ; pTestBar != NULL; pTestBar = pTestBar->GetWindow( GW_HWNDNEXT ) )
		} // for( ; pos != NULL ; )
	} // nSpaceType==0 - horizontal space search, nSpaceType==1 - vertical space search
	if( ! bLayoutChanged )
		return;
	if( bRecalcLayout )
		pDockSite->RecalcLayout();
	else if( bDelayRecalcLayout )
		pDockSite->DelayRecalcLayout();
}

void CExtControlBar::stat_RedrawFloatingFrames( CFrameWnd * pDockSite )
{
	ASSERT_VALID( pDockSite );
CMap
	<	CMiniFrameWnd *,
		CMiniFrameWnd *,
		char,
		char
		>
	setMiniFrames;
POSITION pos = pDockSite->m_listControlBars.GetHeadPosition();
	for( ; pos != NULL; )
	{
		CControlBar * pControlBar = (CControlBar *)
			pDockSite->m_listControlBars.GetNext( pos );
		if( pControlBar->m_pDockSite == NULL )
			continue;
		ASSERT_VALID( pControlBar );
		ASSERT_KINDOF( CControlBar, pControlBar );
		if( pControlBar->IsDockBar() )
			continue;
		CFrameWnd * pBarParentFrame =
			pControlBar->GetParentFrame();
		ASSERT_VALID( pBarParentFrame );
		if( pBarParentFrame == pDockSite )
			continue;
		CMiniFrameWnd * pFloatingFrame =
			DYNAMIC_DOWNCAST(
				CMiniFrameWnd,
				pBarParentFrame
				);
		if( pFloatingFrame == NULL )
			continue;
		setMiniFrames.SetAt( pFloatingFrame, 0 );
	} // for( ; pos != NULL; )
	pos = setMiniFrames.GetStartPosition();
	for( ; pos != NULL; )
	{
		CMiniFrameWnd * pFloatingFrame = NULL;
		char nDummy;
		setMiniFrames.GetNextAssoc(pos,pFloatingFrame,nDummy);
		pFloatingFrame->RecalcLayout();
		pFloatingFrame->RedrawWindow(
			NULL,
			NULL,
			RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE
				| RDW_FRAME | RDW_ALLCHILDREN
			);
	} // for( ; pos != NULL; )
}


void CExtControlBar::stat_RecalcBarMetrics( CFrameWnd * pDockSite )
{
	ASSERT_VALID( pDockSite );
	for(	POSITION pos = pDockSite->m_listControlBars.GetHeadPosition();
			pos != NULL;
			)
	{
		CControlBar * pControlBar = (CControlBar *)
			pDockSite->m_listControlBars.GetNext( pos );
		if( pControlBar->m_pDockSite == NULL )
			continue;
		ASSERT_VALID( pControlBar );
		ASSERT_KINDOF( CControlBar, pControlBar );
		if( pControlBar->IsDockBar() )
			continue;
		CExtControlBar * pExtControlBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pControlBar );
		if(		pExtControlBar != NULL
			&&	pExtControlBar->m_bNoForcedRecalcMetrics
			)
			continue;
		pControlBar->SetWindowPos(
			NULL, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER
				//|SWP_NOREDRAW
				|SWP_FRAMECHANGED
			);
		//pControlBar->PostMessage( WM_NCPAINT );
		CExtToolControlBar * pToolBar =
			DYNAMIC_DOWNCAST( CExtToolControlBar, pControlBar );
		if( pToolBar != NULL )
			pToolBar->_RecalcPositionsImpl();
		pControlBar->Invalidate();
	}
}

CWnd * CExtControlBar::stat_GetWndForPlacement( CWnd * pWnd )
{
	ASSERT_VALID( pWnd );
	ASSERT( pWnd->GetSafeHwnd() != NULL );
	for( ; true; pWnd = pWnd->GetParent() )
	{
		DWORD dwStyle = pWnd->GetStyle();
		if( (dwStyle&WS_CHILD) == 0 )
			break;
	} // for( ; true; pWnd = pWnd->GetParent() )
	return pWnd;
}

bool CExtControlBar::stat_GetWindowPlacement( HWND hWnd, WINDOWPLACEMENT & _wp )
{
#if (!defined __EXT_MFC_NO_NC_FRAME )
	return CExtNcFrameImpl::stat_GetWindowPlacement( hWnd, _wp );
#else
	return ::GetWindowPlacement( hWnd, &_wp ) ? true : false;
#endif
}

bool CExtControlBar::stat_SetWindowPlacement( HWND hWnd, const WINDOWPLACEMENT & _wp )
{
#if (!defined __EXT_MFC_NO_NC_FRAME )
	return CExtNcFrameImpl::stat_SetWindowPlacement( hWnd, _wp );
#else
	return ::SetWindowPlacement( hWnd, &_wp ) ? true : false;
#endif
}

bool CExtControlBar::stat_SaveWindowRect(
	HWND hWnd,
	__EXT_MFC_SAFE_LPCTSTR strSection,
	__EXT_MFC_SAFE_LPCTSTR strEntry
	)
{
#if (!defined __EXT_MFC_NO_NC_FRAME )
	return CExtNcFrameImpl::stat_SaveWindowRect( hWnd, strSection, strEntry );
#else
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
#endif
}

bool CExtControlBar::stat_LoadWindowRect(
	HWND hWnd,
	__EXT_MFC_SAFE_LPCTSTR strSection,
	__EXT_MFC_SAFE_LPCTSTR strEntry,
	bool bForceHideWindow // = false
	)
{
#if (!defined __EXT_MFC_NO_NC_FRAME )
	return CExtNcFrameImpl::stat_LoadWindowRect( hWnd, strSection, strEntry, bForceHideWindow );
#else
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
#endif
}

void CExtControlBar::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	ASSERT_VALID( this );
	pWnd;
	ScreenToClient( &point );
CRect rcClient;
	GetClientRect( &rcClient );
	if( rcClient.PtInRect(point) )
		return;
	OnRButtonUp( 0, point );
}

void CExtControlBar::OnTimer(__EXT_MFC_UINT_PTR nIDEvent)
{
	ASSERT_VALID( this );
	if( nIDEvent == ((__EXT_MFC_UINT_PTR)m_nFlashCaptionTimerID) )
	{
		FlashCaption_OnTimer();
		return;
	}
	CControlBar::OnTimer( nIDEvent );
}

CExtControlBar::eResizablePanelDockingType_t
	CExtControlBar::OnQueryDockingType() const
{
	ASSERT_VALID( this );
eResizablePanelDockingType_t eResizablePanelDockingType =
		g_eResizablePanelDockingType;
	if( eResizablePanelDockingType == __RESIZABLE_DOCKING_TYPE_BY_THEME )
		eResizablePanelDockingType = (eResizablePanelDockingType_t)
			PmBridge_GetPM()->GetThemeDockingType();
	ASSERT( __RESIZABLE_DOCKING_TYPE_MIN_VALUE <= eResizablePanelDockingType );
	ASSERT( eResizablePanelDockingType <= __RESIZABLE_DOCKING_TYPE_MAX_VALUE );
	return eResizablePanelDockingType;
}

#if (!defined __EXT_MFC_NO_DOCK_MARKERS)

/////////////////////////////////////////////////////////////////////////////
// CExtDynDockHighlightWnd window

IMPLEMENT_DYNAMIC( CExtDynDockHighlightWnd, CWnd )
IMPLEMENT_CExtPmBridge_MEMBERS( CExtDynDockHighlightWnd );

CExtDynDockHighlightWnd::CExtDynDockHighlightWnd(
	CExtDynDockMarkerWnd * pWndDynDockMarker
	)
	: m_pWndDynDockMarker( pWndDynDockMarker )
{
	ASSERT_VALID( m_pWndDynDockMarker );
	ASSERT( m_pWndDynDockMarker->GetSafeHwnd() != NULL );
	ASSERT( pWndDynDockMarker->IsStateApplyable() );
CExtControlBar * pDraggedBar =
		CExtControlBar::_DraggingGetBar();
	if( pDraggedBar == NULL )
		return;
	ASSERT_VALID( pDraggedBar );
HWND hWndMarkerTarget = pWndDynDockMarker->GetMarkerTargetHWND();
CRect rcScreen( 0, 0, 0, 0 );
	if( hWndMarkerTarget == NULL )
	{
		if( CExtDynDockMarkerWnd::g_arrDockMarkerWndsOuter.GetSize() != 4 )
		{
			ASSERT( FALSE );
			return;
		} // if( CExtDynDockMarkerWnd::g_arrDockMarkerWndsOuter.GetSize() != 4 )
		CRect rcTemp;
		CExtDynDockMarkerWnd::g_arrDockMarkerWndsOuter[0]->GetWindowRect( &rcTemp );
		rcScreen.top = rcTemp.top;
		CExtDynDockMarkerWnd::g_arrDockMarkerWndsOuter[1]->GetWindowRect( &rcTemp );
		rcScreen.bottom = rcTemp.bottom;
		CExtDynDockMarkerWnd::g_arrDockMarkerWndsOuter[2]->GetWindowRect( &rcTemp );
		rcScreen.left = rcTemp.left;
		CExtDynDockMarkerWnd::g_arrDockMarkerWndsOuter[3]->GetWindowRect( &rcTemp );
		rcScreen.right = rcTemp.right;
		CRect rcOuterMargins = PmBridge_GetPM()->DockMarker_GetOuterDistance();
		rcScreen.InflateRect( &rcOuterMargins );
	} // if( hWndMarkerTarget == NULL )
	else
	{
		::GetWindowRect( hWndMarkerTarget, &rcScreen );
		CWnd * pWnd = CWnd::FromHandlePermanent( hWndMarkerTarget );
		if(		pWnd == NULL
			||	(! pWnd->IsKindOf( RUNTIME_CLASS(CExtControlBar) ) )
			)
			hWndMarkerTarget = NULL;
	} // else from if( hWndMarkerTarget == NULL )

CExtPaintManager::eDockMarkerType_t eDockMarkerType =
		(CExtPaintManager::eDockMarkerType_t)
			pWndDynDockMarker->GetMarkerType();
static const CSize _sizeMinMetric( 30, 30 );
	switch( eDockMarkerType )
	{
	case CExtPaintManager::__EDMT_2005_TOP:
		if( hWndMarkerTarget != NULL )
		{
			rcScreen.bottom = rcScreen.top + rcScreen.Height() / 2;
		} // if( hWndMarkerTarget != NULL )
		else
		{
			CSize _size = pDraggedBar->GetSizeDockedHorizontal();
			if( _size.cy <= _sizeMinMetric.cy )
				_size.cy = _sizeMinMetric.cy;
			rcScreen.bottom = rcScreen.top + _size.cy;
		} // else from if( hWndMarkerTarget != NULL )
		break;
	case CExtPaintManager::__EDMT_2005_BOTTOM:
		if( hWndMarkerTarget != NULL )
		{
			rcScreen.top = rcScreen.bottom - rcScreen.Height() / 2;
		} // if( hWndMarkerTarget != NULL )
		else
		{
			CSize _size = pDraggedBar->GetSizeDockedHorizontal();
			if( _size.cy <= _sizeMinMetric.cy )
				_size.cy = _sizeMinMetric.cy;
			rcScreen.top = rcScreen.bottom - _size.cy;
		} // else from if( hWndMarkerTarget != NULL )
		break;
	case CExtPaintManager::__EDMT_2005_LEFT:
		if( hWndMarkerTarget != NULL )
		{
			rcScreen.right = rcScreen.left + rcScreen.Width() / 2;
		} // if( hWndMarkerTarget != NULL )
		else
		{
			CSize _size = pDraggedBar->GetSizeDockedVertical();
			if( _size.cx <= _sizeMinMetric.cx )
				_size.cx = _sizeMinMetric.cx;
			rcScreen.right = rcScreen.left + _size.cx;
		} // else from if( hWndMarkerTarget != NULL )
		break;
	case CExtPaintManager::__EDMT_2005_RIGHT:
		if( hWndMarkerTarget != NULL )
		{
			rcScreen.left = rcScreen.right - rcScreen.Width() / 2;
		} // if( hWndMarkerTarget != NULL )
		else
		{
			CSize _size = pDraggedBar->GetSizeDockedVertical();
			if( _size.cx <= _sizeMinMetric.cx )
				_size.cx = _sizeMinMetric.cx;
			rcScreen.left = rcScreen.right - _size.cx;
		} // else from if( hWndMarkerTarget != NULL )
		break;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	case CExtPaintManager::__EDMT_2005_TAB:
		break;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	default:
		{
			ASSERT( FALSE );
		}
		return;
	} // switch( eDockMarkerType )

	RegisterDockHighlightClass();
	if( CExtDynDockMarkerWnd::g_pWndMarkersParent == NULL )
	{
		CExtDynDockMarkerWnd::g_pWndMarkersParent = new CWnd;
		if( ! CExtDynDockMarkerWnd::g_pWndMarkersParent->CreateEx(
				WS_EX_TOPMOST,
				__EXT_DYNDOCKMARKER_CLASS_NAME,
				_T(""),
				WS_POPUP,
				0,
				0,
				0,
				0,
				::GetDesktopWindow(),
				(HMENU)NULL,
				0L
				)
			)
		{
			ASSERT( FALSE );
			delete CExtDynDockMarkerWnd::g_pWndMarkersParent;
			CExtDynDockMarkerWnd::g_pWndMarkersParent = NULL;
			return;
		}
	} // if( CExtDynDockMarkerWnd::g_pWndMarkersParent == NULL )
BOOL bCreateResult =
		CWnd::CreateEx(
			WS_EX_TOPMOST
				| (	g_PaintManager.m_bLayeredHighlighting2005
						? __EXT_MFC_WS_EX_LAYERED
						: 0
					)
				,
			__EXT_DYNDOCKHIGHLIGHTWND_CLASS_NAME,
			_T(""),
			WS_POPUP,
			rcScreen.left,
			rcScreen.top,
			rcScreen.Width(),
			rcScreen.Height(),
			CExtDynDockMarkerWnd::g_pWndMarkersParent->GetSafeHwnd(),
			(HMENU)NULL,
			0L
			);
	if( ! bCreateResult )
	{
		ASSERT( FALSE );
		return;
	} // if( ! bCreateResult )
	if( g_PaintManager.m_bLayeredHighlighting2005 )
	{
		bool bTabShape = false;
		CRect rcTabMainArea(0,0,0,0), rcTabBottomMiddleArea(0,0,0,0);

		bool bTabsAtTop = false;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

		bTabsAtTop = CExtControlBar::g_bTabsAtTop;
		
		if( m_pWndDynDockMarker->GetMarkerType() == CExtPaintManager::__EDMT_2005_TAB )
		{
			CRect rcClient = rcScreen;
			rcClient.OffsetRect( - rcClient.TopLeft() );
			bTabShape = true;
			int nTabHeight = CExtControlBar::g_nTabShapeMaxHeight;
			int nTrackerHeight = rcClient.Height();
			if( nTrackerHeight < (CExtControlBar::g_nTabShapeMaxHeight * 2) )
				nTabHeight = nTrackerHeight / 2;
			rcTabMainArea = rcClient;
			if( bTabsAtTop )
				rcTabMainArea.top += nTabHeight;
			else
				rcTabMainArea.bottom -= nTabHeight;
			ASSERT( rcTabMainArea.bottom > rcTabMainArea.top );
			
			CRect rcTabBottomLeftArea( rcClient );
			rcTabBottomLeftArea.right =
				rcTabBottomLeftArea.left
				+ CExtControlBar::g_nTabShapeBottomLeftAreaWidth
				;
			if( bTabsAtTop )
				rcTabBottomLeftArea.bottom =
					rcTabBottomLeftArea.top + nTabHeight;
			else
				rcTabBottomLeftArea.top =
					rcTabBottomLeftArea.bottom - nTabHeight;

			rcTabBottomMiddleArea = rcTabBottomLeftArea;
			rcTabBottomMiddleArea.OffsetRect(
				rcTabBottomLeftArea.Width(),
				0
				);
			rcTabBottomMiddleArea.right =
				rcTabBottomMiddleArea.left
				+ CExtControlBar::g_nTabShapeBottomMiddleAreaWidth
				;
			if( rcTabBottomMiddleArea.right > rcClient.right )
				rcTabBottomMiddleArea.right = 
					rcTabBottomMiddleArea.left
					+ (rcClient.right - rcTabBottomMiddleArea.left) / 2
					;
			
			CRect rcTabBottomRightArea( rcTabBottomLeftArea );
			rcTabBottomRightArea.right = rcClient.right;
			rcTabBottomRightArea.left = rcTabBottomMiddleArea.right;

			CExtControlBar * pDraggedBar = CExtControlBar::_DraggingGetBar();
			if( pDraggedBar->GetSafeHwnd() != NULL )
			{
				ASSERT_VALID( pDraggedBar );
				ASSERT_VALID( pDraggedBar->m_pDockSite );
				ASSERT( pDraggedBar->m_pDockSite->GetSafeHwnd() != NULL );
				if( (pDraggedBar->m_pDockSite->GetExStyle()&WS_EX_LAYOUTRTL) != 0 )
				{
					rcTabBottomRightArea.left =
						rcTabBottomRightArea.right
						- rcTabBottomLeftArea.Width()
						;
					rcTabBottomMiddleArea.OffsetRect(
						rcTabBottomRightArea.left
						- rcTabBottomMiddleArea.right
						,
						0
						);
					rcTabBottomLeftArea.right =
						rcTabBottomMiddleArea.left
						;
				} // if( (pDraggedBar->m_pDockSite->GetExStyle()&WS_EX_LAYOUTRTL) != 0 )
			} // if( pDraggedBar->GetSafeHwnd() != NULL )

		} // if( m_pWndDynDockMarker->GetMarkerType() == CExtPaintManager::__EDMT_2005_TAB )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		PmBridge_GetPM()->DockMarker_AdjustHighlightedLayer(
			m_hWnd,
			rcScreen.Width(),
			rcScreen.Height(),
			bTabShape,
			bTabsAtTop,
			rcTabMainArea,
			rcTabBottomMiddleArea
			);
	}

	SetWindowPos(
		CExtDynDockMarkerWnd::g_pWndMarkersParent,
		0, 0, 0, 0,
		SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW
		);
}

CExtDynDockHighlightWnd::~CExtDynDockHighlightWnd()
{
}

BEGIN_MESSAGE_MAP(CExtDynDockHighlightWnd, CWnd)
	//{{AFX_MSG_MAP(CExtDynDockHighlightWnd)
	ON_WM_NCCALCSIZE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool CExtDynDockHighlightWnd::g_bDockHighlightClassRegistered = false;

bool CExtDynDockHighlightWnd::RegisterDockHighlightClass()
{
	if( g_bDockHighlightClassRegistered )
		return true;
WNDCLASS _wndClassInfo;
HINSTANCE hInst = ::AfxGetInstanceHandle();
	if( ! ::GetClassInfo( hInst, __EXT_DYNDOCKHIGHLIGHTWND_CLASS_NAME, &_wndClassInfo ) )
	{
		_wndClassInfo.style = CS_GLOBALCLASS; //|CS_DBLCLKS;
		_wndClassInfo.lpfnWndProc = ::DefWindowProc;
		_wndClassInfo.cbClsExtra = _wndClassInfo.cbWndExtra = 0;
		_wndClassInfo.hInstance = hInst;
		_wndClassInfo.hIcon = NULL;
		_wndClassInfo.hCursor = ::LoadCursor( NULL, IDC_ARROW );
		ASSERT( _wndClassInfo.hCursor != NULL );
		_wndClassInfo.hbrBackground = NULL; 
		_wndClassInfo.lpszMenuName = NULL;
		_wndClassInfo.lpszClassName = __EXT_DYNDOCKHIGHLIGHTWND_CLASS_NAME;
		if( !::AfxRegisterClass( &_wndClassInfo ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}
	g_bDockHighlightClassRegistered = true;
	return true;
}

BOOL CExtDynDockHighlightWnd::PreCreateWindow( CREATESTRUCT & cs ) 
{
	if(		( !RegisterDockHighlightClass() )
		||	( ! CWnd::PreCreateWindow(cs) )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	
	cs.lpszClass = __EXT_DYNDOCKHIGHLIGHTWND_CLASS_NAME;

	return TRUE;
}

void CExtDynDockHighlightWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
//	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
	bCalcValidRects;
	lpncsp;
}

int CExtDynDockHighlightWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
//	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
	pDesktopWnd;
	nHitTest;
	message;
	return MA_NOACTIVATEANDEAT;
}

void CExtDynDockHighlightWnd::OnPaint() 
{
	if( g_PaintManager.m_bLayeredHighlighting2005 )
	{
		Default();
		return;
	} // if( g_PaintManager.m_bLayeredHighlighting2005 )
CRect rcClient;
	GetClientRect( &rcClient );
CPaintDC dcPaint( this );
	if( rcClient.IsRectEmpty() )
		return;
	if( m_bmpHighlightSurface.IsEmpty() )
	{
		BITMAPINFOHEADER bih;
		::memset( &bih, 0, sizeof(BITMAPINFOHEADER) );
		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biWidth = rcClient.Width();
		bih.biHeight = rcClient.Height();
		bih.biPlanes = 1;
		bih.biBitCount = 32;
		bih.biCompression = BI_RGB;
		bih.biSizeImage = bih.biWidth * bih.biHeight;
		COLORREF * pSurfaceCB = NULL;
		HBITMAP hDib =
			::CreateDIBSection(
				dcPaint,
				(LPBITMAPINFO)&bih,
				DIB_RGB_COLORS,
				(void **)&pSurfaceCB,
				NULL,
				NULL
				);
		if( hDib == NULL || pSurfaceCB == NULL )
		{
			ASSERT( FALSE );
			return;
		}
		CDC dcMemory;
		if( ! dcMemory.CreateCompatibleDC( &dcPaint ) )
		{
			ASSERT( FALSE );
			::DeleteObject( hDib );
			return;
		}
		HGDIOBJ hOldBmp = dcMemory.SelectObject( hDib );
		dcMemory.BitBlt(
			0,
			0,
			bih.biWidth,
			bih.biHeight,
			&dcPaint,
			0,
			0,
			SRCCOPY
			);
		dcMemory.SelectObject( hOldBmp );
		dcMemory.DeleteDC();

		bool bTabShape = false;
		CRect rcTabMainArea(0,0,0,0), rcTabBottomMiddleArea(0,0,0,0);

		bool bTabsAtTop = false;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

		bTabsAtTop = CExtControlBar::g_bTabsAtTop;
		
		if( m_pWndDynDockMarker->GetMarkerType() == CExtPaintManager::__EDMT_2005_TAB )
		{
			bTabShape = true;
			int nTabHeight = CExtControlBar::g_nTabShapeMaxHeight;
			int nTrackerHeight = rcClient.Height();
			if( nTrackerHeight < (CExtControlBar::g_nTabShapeMaxHeight * 2) )
				nTabHeight = nTrackerHeight / 2;
			rcTabMainArea = rcClient;
			if( bTabsAtTop )
				rcTabMainArea.top += nTabHeight;
			else
				rcTabMainArea.bottom -= nTabHeight;
			ASSERT( rcTabMainArea.bottom > rcTabMainArea.top );
			
			CRect rcTabBottomLeftArea( rcClient );
			rcTabBottomLeftArea.right =
				rcTabBottomLeftArea.left
				+ CExtControlBar::g_nTabShapeBottomLeftAreaWidth
				;
			if( bTabsAtTop )
				rcTabBottomLeftArea.bottom =
					rcTabBottomLeftArea.top + nTabHeight;
			else
				rcTabBottomLeftArea.top =
					rcTabBottomLeftArea.bottom - nTabHeight;

			rcTabBottomMiddleArea = rcTabBottomLeftArea;
			rcTabBottomMiddleArea.OffsetRect(
				rcTabBottomLeftArea.Width(),
				0
				);
			rcTabBottomMiddleArea.right =
				rcTabBottomMiddleArea.left
				+ CExtControlBar::g_nTabShapeBottomMiddleAreaWidth
				;
			if( rcTabBottomMiddleArea.right > rcClient.right )
				rcTabBottomMiddleArea.right = 
					rcTabBottomMiddleArea.left
					+ (rcClient.right - rcTabBottomMiddleArea.left) / 2
					;
			
			CRect rcTabBottomRightArea( rcTabBottomLeftArea );
			rcTabBottomRightArea.right = rcClient.right;
			rcTabBottomRightArea.left = rcTabBottomMiddleArea.right;

			CExtControlBar * pDraggedBar = CExtControlBar::_DraggingGetBar();
			if( pDraggedBar->GetSafeHwnd() != NULL )
			{
				ASSERT_VALID( pDraggedBar );
				ASSERT_VALID( pDraggedBar->m_pDockSite );
				ASSERT( pDraggedBar->m_pDockSite->GetSafeHwnd() != NULL );
				if( (pDraggedBar->m_pDockSite->GetExStyle()&WS_EX_LAYOUTRTL) != 0 )
				{
					rcTabBottomRightArea.left =
						rcTabBottomRightArea.right
						- rcTabBottomLeftArea.Width()
						;
					rcTabBottomMiddleArea.OffsetRect(
						rcTabBottomRightArea.left
						- rcTabBottomMiddleArea.right
						,
						0
						);
					rcTabBottomLeftArea.right =
						rcTabBottomMiddleArea.left
						;
				} // if( (pDraggedBar->m_pDockSite->GetExStyle()&WS_EX_LAYOUTRTL) != 0 )
			} // if( pDraggedBar->GetSafeHwnd() != NULL )

		} // if( m_pWndDynDockMarker->GetMarkerType() == CExtPaintManager::__EDMT_2005_TAB )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		
		PmBridge_GetPM()->DockMarker_AdjustHighlightedArea(
			pSurfaceCB,
			bih.biWidth,
			bih.biHeight,
			bTabShape,
			bTabsAtTop,
			rcTabMainArea,
			rcTabBottomMiddleArea
			);

		m_bmpHighlightSurface.FromBitmap( hDib );
		::DeleteObject( hDib );
	} // if( m_bmpHighlightSurface.IsEmpty() )
	ASSERT( ! m_bmpHighlightSurface.IsEmpty() );
// CDC dcMemory;
// 	if( dcMemory.CreateCompatibleDC( &dcPaint ) )
// 	{
// 		CBitmap * pOldBmp =
// 			dcMemory.SelectObject(
// 				&m_bmpHighlightSurface
// 				);
// 		dcPaint.BitBlt(
// 			0,
// 			0,
// 			rcClient.Width(),
// 			rcClient.Height(),
// 			&dcMemory,
// 			0,
// 			0,
// 			SRCCOPY
// 			);
// 		dcMemory.SelectObject( pOldBmp );
// 	} // if( dcMemory.CreateCompatibleDC( &dcPaint ) )
// #ifdef _DEBUG
// 	else
// 	{
// 		ASSERT( FALSE );
// 	} // else from if( dcMemory.CreateCompatibleDC( &dcPaint ) )
// #endif // _DEBUG
	m_bmpHighlightSurface.AlphaBlend(
		dcPaint.m_hDC,
		0,
		0,
		rcClient.Width(),
		rcClient.Height()
		);
}

/////////////////////////////////////////////////////////////////////////////
// CExtDynDockMarkerWnd window

#define __DYNDOCKMARKER_TIMER_ID_TEMP			77
#define __DYNDOCKMARKER_TIMER_ID_ANIMATE		88
#define __DYNDOCKMARKER_ANIMATION_PERCENT_STEP	25

IMPLEMENT_DYNAMIC( CExtDynDockMarkerWnd, CWnd )
IMPLEMENT_CExtPmBridge_MEMBERS( CExtDynDockMarkerWnd );

CWnd * CExtDynDockMarkerWnd::g_pWndMarkersParent = NULL;

CExtDynDockMarkerWnd::CExtDynDockMarkerWnd(
	CExtDynDockMarkerWnd * pWndParentMarker,
	HWND hWndMarkerTarget,
	int eMarkerType,
	const CExtBitmap & _bmpIn,
	const CExtBitmap & _bmpOut,
	HRGN hRgn,
	CRect rcScreen,
	const CExtBitmap * _pBmpOuterBk, // = NULL
	int nOffserInnerX, // = 0
	int nOffserInnerY, // = 0
	bool bSparsedMode // = false
	)
	: m_pWndParentMarker( pWndParentMarker )
	, m_hWndMarkerTarget( hWndMarkerTarget )
	, m_hWndLastHoverDepenentMarker( NULL )
	, m_ptOffsetInner( nOffserInnerX, nOffserInnerY )
	, m_bSparsedMode( bSparsedMode )
	, m_bAnimationInProcess( false )
	, m_bAnimationShow( false )
	, m_bDestroyOnAnimationStop( false )
	, m_nOpaquePercent( 100 )
{
	m_pWndParentMarker;
	m_pWndDockHighlight = NULL;
//	ASSERT( m_hWndMarkerTarget != NULL && ::IsWindow(m_hWndMarkerTarget) );
	m_eMarkerType = eMarkerType;
	ASSERT(
			int(CExtPaintManager::__EDMT_MIN_VALUE) <= m_eMarkerType
		&&	m_eMarkerType <= int(CExtPaintManager::__EDMT_MAX_VALUE)
		);
	m_eMTS = __EMTS_UNKNOWN;
	ASSERT( ! _bmpIn.IsEmpty() );
	ASSERT( ! _bmpOut.IsEmpty() );
	ASSERT( hRgn != NULL );
	m_bmpIn = _bmpIn;
	m_bmpOut = _bmpOut;
	if( _pBmpOuterBk != NULL  )
//		m_bmpOuterBk.Attach( hBmpOuterBk );
		m_bmpOuterBk = (*_pBmpOuterBk);
	m_rgn.Attach( hRgn );
	RegisterDockMarkerClass();
	if( g_pWndMarkersParent == NULL )
	{
		g_pWndMarkersParent = new CWnd;
		if( ! g_pWndMarkersParent->CreateEx(
				WS_EX_TOPMOST,
				__EXT_DYNDOCKMARKER_CLASS_NAME,
				_T(""),
				WS_POPUP,
				0,
				0,
				0,
				0,
				::GetDesktopWindow(),
				(HMENU)NULL,
				0L
				)
			)
		{
			ASSERT( FALSE );
			delete g_pWndMarkersParent;
			g_pWndMarkersParent = NULL;
			return;
		}
	} // if( g_pWndMarkersParent == NULL )
BOOL bCreateResult =
		CWnd::CreateEx(
			WS_EX_TOPMOST
				| (	g_PaintManager.m_bLayeredHighlighting2005
						? __EXT_MFC_WS_EX_LAYERED
						: 0
					)
				,
			__EXT_DYNDOCKMARKER_CLASS_NAME,
			_T(""),
			WS_POPUP,
			rcScreen.left,
			rcScreen.top,
			rcScreen.Width(),
			rcScreen.Height(),
			g_pWndMarkersParent->GetSafeHwnd(),
			(HMENU)NULL,
			0L
			);
	if( ! bCreateResult )
	{
		ASSERT( FALSE );
		return;
	} // if( ! bCreateResult )
	if( g_PaintManager.m_bLayeredHighlighting2005 )
		g_PaintManager.m_pfnSetLayeredWindowAttributes(
			GetSafeHwnd(), 
			0, 
			255,
			__EXT_MFC_LWA_ALPHA
			);
	::SetWindowRgn(
		m_hWnd,
		(HRGN)m_rgn.GetSafeHandle(),
		FALSE
		);

//	if( m_pWndParentMarker->GetSafeHwnd() != NULL )
//		SetWindowPos( m_pWndParentMarker, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE );
//	if( eMarkerType == INT(CExtPaintManager::__EDMT_2005_GROUP_CENTRAL) )
//		ShowWindow( SW_HIDE );

	SetTimer( __DYNDOCKMARKER_TIMER_ID_TEMP, 50, NULL );
}

CExtDynDockMarkerWnd::~CExtDynDockMarkerWnd()
{
	HighlightMarkerTarget( false );
}

BEGIN_MESSAGE_MAP(CExtDynDockMarkerWnd, CWnd)
	//{{AFX_MSG_MAP(CExtDynDockMarkerWnd)
	ON_WM_NCCALCSIZE()
	ON_WM_MOUSEACTIVATE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
    ON_MESSAGE( WM_PRINT, OnPrintClient )
    ON_MESSAGE( WM_PRINTCLIENT, OnPrintClient )
END_MESSAGE_MAP()

CTypedPtrArray < CPtrArray, CExtDynDockMarkerWnd * >
	CExtDynDockMarkerWnd::g_arrDockMarkerWndsOuter;
CTypedPtrArray < CPtrArray, CExtDynDockMarkerWnd * >
	CExtDynDockMarkerWnd::g_arrDockMarkerWndsInner;
HWND CExtDynDockMarkerWnd::g_hWndLastDynamicTarget = NULL;

bool CExtDynDockMarkerWnd::HighlightMarkerTarget(
	bool bHighlight // = true
	)
{
	ASSERT_VALID( this );
	if(		GetSafeHwnd() != NULL
		&&	( GetExStyle() & __EXT_MFC_WS_EX_LAYERED ) != 0
		&&	g_PaintManager.m_bLayeredHighlighting2005
		)
		g_PaintManager.m_pfnSetLayeredWindowAttributes(
			GetSafeHwnd(), 
			0, 
			g_PaintManager->DockMarker_GetAlpha( bHighlight, this ),
			__EXT_MFC_LWA_ALPHA
			);
	if( bHighlight )
	{
		if( m_pWndDockHighlight != NULL )
			HighlightMarkerTarget( false );
		UnHighlightMarkersExcluding( this );
		m_pWndDockHighlight =
			new CExtDynDockHighlightWnd( this );
		if( m_pWndDockHighlight->GetSafeHwnd() == NULL )
		{
			HighlightMarkerTarget( false );
			ASSERT( FALSE );
			return false;
		} // if( m_pWndDockHighlight->GetSafeHwnd() == NULL )
		return true;
	} // if( bHighlight )
	else
	{
		if( m_pWndDockHighlight != NULL )
		{
			ASSERT_VALID( m_pWndDockHighlight );
			if( m_pWndDockHighlight->GetSafeHwnd() != NULL )
				m_pWndDockHighlight->DestroyWindow();
			delete m_pWndDockHighlight;
			m_pWndDockHighlight = NULL;
		} // if( m_pWndDockHighlight != NULL )
		return true;
	} // else from if( bHighlight )
}

CExtControlBar::e_calc_dock_type_t CExtDynDockMarkerWnd::GetCDT()
{
	ASSERT_VALID( this );
	ASSERT(
			int(CExtPaintManager::__EDMT_MIN_VALUE) <= m_eMarkerType
		&&	m_eMarkerType <= int(CExtPaintManager::__EDMT_MAX_VALUE)
		);
CExtControlBar * pBar = NULL;
	if( m_hWndMarkerTarget != NULL )
	{
		CWnd * pWnd = CWnd::FromHandlePermanent( m_hWndMarkerTarget );
		if( pWnd != NULL )
		{
			pBar =
				DYNAMIC_DOWNCAST( CExtControlBar, pWnd );
		} // if( pWnd != NULL )
	} // if( m_hWndMarkerTarget != NULL )
	switch( m_eMarkerType )
	{
	case int(CExtPaintManager::__EDMT_2005_TOP):
		return
			( pBar != NULL )
				? CExtControlBar::__ECDT_2005_BAR_TOP
				: (
					( m_hWndMarkerTarget != NULL )
						? CExtControlBar::__ECDT_2005_INNER_TOP
						: CExtControlBar::__ECDT_2005_OUTER_TOP
					)
				;
	case int(CExtPaintManager::__EDMT_2005_BOTTOM):
		return
			( pBar != NULL )
				? CExtControlBar::__ECDT_2005_BAR_BOTTOM
				: (
					( m_hWndMarkerTarget != NULL )
						? CExtControlBar::__ECDT_2005_INNER_BOTTOM
						: CExtControlBar::__ECDT_2005_OUTER_BOTTOM
					)
				;
	case int(CExtPaintManager::__EDMT_2005_LEFT):
		return
			( pBar != NULL )
				? CExtControlBar::__ECDT_2005_BAR_LEFT
				: (
					( m_hWndMarkerTarget != NULL )
						? CExtControlBar::__ECDT_2005_INNER_LEFT
						: CExtControlBar::__ECDT_2005_OUTER_LEFT
					)
				;
	case int(CExtPaintManager::__EDMT_2005_RIGHT):
		return
			( pBar != NULL )
				? CExtControlBar::__ECDT_2005_BAR_RIGHT
				: (
					( m_hWndMarkerTarget != NULL )
						? CExtControlBar::__ECDT_2005_INNER_RIGHT
						: CExtControlBar::__ECDT_2005_OUTER_RIGHT
					)
				;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	case int(CExtPaintManager::__EDMT_2005_TAB):
		{
			if( pBar != NULL )
				return CExtControlBar::__ECDT_2005_BAR_NEW_TAB;
			break;
		}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
	} // switch( m_eMarkerType )
	return CExtControlBar::__ECDT_FLOATED;
}

int CExtDynDockMarkerWnd::GetMarkerType() const
{
	ASSERT_VALID( this );
	ASSERT(
			int(CExtPaintManager::__EDMT_MIN_VALUE) <= m_eMarkerType
		&&	m_eMarkerType <= int(CExtPaintManager::__EDMT_MAX_VALUE)
		);
	return m_eMarkerType;
}

HWND CExtDynDockMarkerWnd::GetMarkerTargetHWND() const
{
	ASSERT_VALID( this );
	//ASSERT( m_hWndMarkerTarget != NULL && ::IsWindow(m_hWndMarkerTarget) );
	return m_hWndMarkerTarget;
}

bool CExtDynDockMarkerWnd::IsStateApplyable() const
{
	ASSERT_VALID( this );
	ASSERT(
			int(CExtPaintManager::__EDMT_MIN_VALUE) <= m_eMarkerType
		&&	m_eMarkerType <= int(CExtPaintManager::__EDMT_MAX_VALUE)
		);
	if(		m_eMarkerType == int(CExtPaintManager::__EDMT_2005_TOP)
		||	m_eMarkerType == int(CExtPaintManager::__EDMT_2005_BOTTOM)
		||	m_eMarkerType == int(CExtPaintManager::__EDMT_2005_LEFT)
		||	m_eMarkerType == int(CExtPaintManager::__EDMT_2005_RIGHT)
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		||	m_eMarkerType == int(CExtPaintManager::__EDMT_2005_TAB)
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		)
		return true;
	return false;
}

void CExtDynDockMarkerWnd::UnHighlightAllMarkers(
	bool bPassPaintMessages // = true
	)
{
	UnHighlightMarkersExcluding(
		NULL,
		bPassPaintMessages
		);
}

void CExtDynDockMarkerWnd::UnHighlightMarkersExcluding(
	CExtDynDockMarkerWnd * pMarkerExclude,
	bool bPassPaintMessages // = true
	)
{
	UnHighlightMarkersExcluding( g_arrDockMarkerWndsOuter, pMarkerExclude, false );
	UnHighlightMarkersExcluding( g_arrDockMarkerWndsInner, pMarkerExclude, false );
	if( bPassPaintMessages )
		CExtPaintManager::stat_PassPaintMessages();
}

void CExtDynDockMarkerWnd::UnHighlightMarkersExcluding(
	CTypedPtrArray < CPtrArray, CExtDynDockMarkerWnd * >
		& arrDockMarkerWnds,
	CExtDynDockMarkerWnd * pMarkerExclude,
	bool bPassPaintMessages // = true
	)
{
int nCount = (int)arrDockMarkerWnds.GetSize();
	for( int nIndex = 0; nIndex < nCount; nIndex++ )
	{
		CExtDynDockMarkerWnd * pDynDockMarkerWnd =
			arrDockMarkerWnds[ nIndex ];
		ASSERT_VALID( pDynDockMarkerWnd );
		ASSERT( pDynDockMarkerWnd->GetSafeHwnd() != NULL );
		if( pDynDockMarkerWnd == pMarkerExclude )
			continue;
		VERIFY( pDynDockMarkerWnd->HighlightMarkerTarget( false ) );
	} // for( int nIndex = 0; nIndex < nCount; nIndex++ )
	if( bPassPaintMessages )
		CExtPaintManager::stat_PassPaintMessages();
}


void CExtDynDockMarkerWnd::ShowAllMarkers(
	bool bShow // = true
	)
{
	ShowMarkers( g_arrDockMarkerWndsOuter, bShow, false );
	ShowMarkers( g_arrDockMarkerWndsInner, bShow, true );
}

void CExtDynDockMarkerWnd::DestroyAllMarkers()
{
	DestroyMarkers( g_arrDockMarkerWndsOuter, false );
	DestroyMarkers( g_arrDockMarkerWndsInner, false );
	if( g_pWndMarkersParent != NULL )
	{
		ASSERT_VALID( g_pWndMarkersParent );
		g_pWndMarkersParent->DestroyWindow();
		delete g_pWndMarkersParent;
		g_pWndMarkersParent = NULL;
	} // if( g_pWndMarkersParent != NULL )
	g_hWndLastDynamicTarget = NULL;
}

void CExtDynDockMarkerWnd::DestroyMarkers(
	CTypedPtrArray < CPtrArray, CExtDynDockMarkerWnd * >
		& arrDockMarkerWnds,
	bool bAnimate // = true
	)
{
	ShowMarkers( arrDockMarkerWnds, false, bAnimate );
int nCount = (int)arrDockMarkerWnds.GetSize();
	for( int nIndex = 0; nIndex < nCount; nIndex++ )
	{
		CExtDynDockMarkerWnd * pDynDockMarkerWnd =
			arrDockMarkerWnds[ nIndex ];
		ASSERT_VALID( pDynDockMarkerWnd );
		if( pDynDockMarkerWnd->GetSafeHwnd() != NULL )
		{
			if( pDynDockMarkerWnd->IsAnimationInProcess() )
				pDynDockMarkerWnd->m_bDestroyOnAnimationStop = true;
			else
			pDynDockMarkerWnd->DestroyWindow();
		}
	} // for( int nIndex = 0; nIndex < nCount; nIndex++ )
	arrDockMarkerWnds.RemoveAll();
}

void CExtDynDockMarkerWnd::PostNcDestroy() 
{
	CWnd::PostNcDestroy();
	delete this;
}

bool CExtDynDockMarkerWnd::IsAnimationInProcess() const
{
	ASSERT_VALID( this );
	return m_bAnimationInProcess;
}

void CExtDynDockMarkerWnd::AnimationStart(
	bool bShow
	)
{
	ASSERT_VALID( this );
	AnimationStop();

	bool bNeedToAnimate = false;
	if( (GetStyle()&WS_VISIBLE) == 0 )
	{
		if( bShow )
		{
			bNeedToAnimate = true;

			if(		g_PaintManager.m_bIsWin2000orLater 
				&&	g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL
				&&	CExtPaintManager::stat_GetBPP() > 8
				)
			{
				__EXT_MFC_ULONG_PTR nStyle = ::__EXT_MFC_GetWindowLong( GetSafeHwnd(), GWL_EXSTYLE );
				::__EXT_MFC_SetWindowLong( GetSafeHwnd(), GWL_EXSTYLE, nStyle | __EXT_MFC_WS_EX_LAYERED );
				m_nOpaquePercent = 0;
				g_PaintManager.m_pfnSetLayeredWindowAttributes(
					GetSafeHwnd(), 
					0, 
					BYTE((255 * m_nOpaquePercent) / 100), 
					__EXT_MFC_LWA_ALPHA
					);
				ShowWindow( SW_SHOWNOACTIVATE );
				CExtPaintManager::stat_PassPaintMessages();
			}
		} // if( bShow )
	}
	else
	{
		if( !bShow )
		{
			bNeedToAnimate = true;

			if(		g_PaintManager.m_bIsWin2000orLater 
				&&	g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL
				&&	CExtPaintManager::stat_GetBPP() > 8
				)
			{
				__EXT_MFC_ULONG_PTR nStyle = ::__EXT_MFC_GetWindowLong( GetSafeHwnd(), GWL_EXSTYLE );
				::__EXT_MFC_SetWindowLong( GetSafeHwnd(), GWL_EXSTYLE, nStyle | __EXT_MFC_WS_EX_LAYERED );
				m_nOpaquePercent = 100;
				g_PaintManager.m_pfnSetLayeredWindowAttributes(
					GetSafeHwnd(), 
					0, 
					BYTE((255 * m_nOpaquePercent) / 100), 
					__EXT_MFC_LWA_ALPHA
					);
				CExtPaintManager::stat_PassPaintMessages();
			}
		} // if( !bShow )
	}

	if( bNeedToAnimate )
	{
		m_bAnimationShow = bShow;
		m_bAnimationInProcess = true;
		m_bDestroyOnAnimationStop = false;
		SetTimer( __DYNDOCKMARKER_TIMER_ID_ANIMATE, 1, NULL );
		OnTimer( __DYNDOCKMARKER_TIMER_ID_ANIMATE );
	}
}

void CExtDynDockMarkerWnd::AnimationStop()
{
	ASSERT_VALID( this );
	KillTimer( __DYNDOCKMARKER_TIMER_ID_ANIMATE );

	if( (GetStyle()&WS_VISIBLE) != 0 )
	{
		if( !m_bAnimationShow )
			ShowWindow( SW_HIDE );
	}
	else
	{
		if( m_bAnimationShow )
			ShowWindow( SW_SHOWNOACTIVATE );
	}
	
	::RedrawWindow(
		GetSafeHwnd(), 
		NULL, 
		NULL, 
		RDW_INVALIDATE | RDW_UPDATENOW
		);
	m_bAnimationInProcess = false;
	if( m_bDestroyOnAnimationStop )
		DestroyWindow();
}

bool CExtDynDockMarkerWnd::ShowMarker( 
	bool bShow, 
	bool bAnimate // = true 
	)
{
	ASSERT_VALID( this );
	if(		g_PaintManager.m_bIsWin2000orLater 
		&&	g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL
		&&	CExtPaintManager::stat_GetBPP() > 8
		&&	PmBridge_GetPM()->m_eStyle2005 != CExtPaintManager::__ES2005_BETA1
		&&	bAnimate
		)
	{
		AnimationStart( bShow );
	}
	else
	{
		if( (GetStyle()&WS_VISIBLE) != 0 )
		{
			if( !bShow )
				ShowWindow( SW_HIDE );
		}
		else
		{
			if( bShow )
				ShowWindow( SW_SHOWNOACTIVATE );
		}
	}
	return true;
}

void CExtDynDockMarkerWnd::ShowMarkers(
	CTypedPtrArray < CPtrArray, CExtDynDockMarkerWnd * >
		& arrDockMarkerWnds,
	bool bShow, // = true
	bool bAnimate // = true
	)
{
static bool bAnimatingInProcess = false;
int nCount = (int)arrDockMarkerWnds.GetSize();
	if( nCount <= 0 )
		return;
//	if(		g_PaintManager.m_bIsWin2000orLater 
//		&&	g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL
//		&&	CExtPaintManager::stat_GetBPP() > 8
//		&&	bAnimate
//		&&  !bAnimatingInProcess
//		)
//	{
//		bool bNeedToAnimate = false;
//
//		// apply the __EXT_MFC_WS_EX_LAYERED style for all the marker windows
//		int nIndex = 0;
//		for( nIndex = 0; nIndex < nCount; nIndex++ )
//		{
//			CExtDynDockMarkerWnd * pDynDockMarkerWnd =
//				arrDockMarkerWnds[ nIndex ];
//			ASSERT_VALID( pDynDockMarkerWnd );
//			ASSERT( pDynDockMarkerWnd->GetSafeHwnd() != NULL );
//
//			if( (pDynDockMarkerWnd->GetStyle()&WS_VISIBLE) == 0 )
//			{
//				if( bShow )
//				{
//					bNeedToAnimate = true;
//
//					LONG nStyle = 
//						::__EXT_MFC_GetWindowLong(
//							pDynDockMarkerWnd->GetSafeHwnd(), 
//							GWL_EXSTYLE
//							);
//					::__EXT_MFC_SetWindowLong(
//						pDynDockMarkerWnd->GetSafeHwnd(), 
//						GWL_EXSTYLE, 
//						nStyle | __EXT_MFC_WS_EX_LAYERED
//						);
//					g_PaintManager.m_pfnSetLayeredWindowAttributes(
//						pDynDockMarkerWnd->GetSafeHwnd(), 
//						0, 
//						0, 
//						__EXT_MFC_LWA_ALPHA
//						);
//					pDynDockMarkerWnd->ShowWindow( SW_SHOWNOACTIVATE );
//
//				} // if( bShow )
//			}
//			else
//			{
//				if( !bShow )
//				{
//					bNeedToAnimate = true;
//
//					LONG nStyle = 
//						::__EXT_MFC_GetWindowLong(
//							pDynDockMarkerWnd->GetSafeHwnd(), 
//							GWL_EXSTYLE
//							);
//					::__EXT_MFC_SetWindowLong(
//						pDynDockMarkerWnd->GetSafeHwnd(), 
//						GWL_EXSTYLE, 
//						nStyle | __EXT_MFC_WS_EX_LAYERED
//						);
//					g_PaintManager.m_pfnSetLayeredWindowAttributes(
//						pDynDockMarkerWnd->GetSafeHwnd(), 
//						0, 
//						255, 
//						__EXT_MFC_LWA_ALPHA
//						);
//				} // if( !bShow )
//			}
//		}
//
//		if( bNeedToAnimate )
//		{
//			bAnimatingInProcess = true;
//			for( int nPercent = 25; nPercent <= 100; nPercent += 25 ) 
//			{
//				// Make the marker window nPercent% alpha
//				for( nIndex = 0; nIndex < nCount; nIndex++ )
//				{
//					CExtDynDockMarkerWnd * pDynDockMarkerWnd =
//						arrDockMarkerWnds[ nIndex ];
//					ASSERT_VALID( pDynDockMarkerWnd );
//					ASSERT( pDynDockMarkerWnd->GetSafeHwnd() != NULL );
//
//					g_PaintManager.m_pfnSetLayeredWindowAttributes(
//						pDynDockMarkerWnd->GetSafeHwnd(), 
//						0, 
//						BYTE((255 * (bShow ? nPercent : 100 - nPercent)) / 100), 
//						__EXT_MFC_LWA_ALPHA
//						);
//					::RedrawWindow(
//						pDynDockMarkerWnd->GetSafeHwnd(), 
//						NULL, 
//						NULL, 
//						RDW_INVALIDATE | RDW_UPDATENOW
//						);
//				}
//				Sleep( 15 );
//			}
//		}
//		
//		// remove the __EXT_MFC_WS_EX_LAYERED style for all the marker windows
//		for( nIndex = 0; nIndex < nCount; nIndex++ )
//		{
//			CExtDynDockMarkerWnd * pDynDockMarkerWnd =
//				arrDockMarkerWnds[ nIndex ];
//			ASSERT_VALID( pDynDockMarkerWnd );
//			ASSERT( pDynDockMarkerWnd->GetSafeHwnd() != NULL );
//			
//			if( (pDynDockMarkerWnd->GetStyle()&WS_VISIBLE) != 0 )
//			{
//				if( !bShow )
//					pDynDockMarkerWnd->ShowWindow( SW_HIDE );
//			}
//			else
//			{
//				if( bShow )
//					pDynDockMarkerWnd->ShowWindow( SW_SHOWNOACTIVATE );
//			}
//			
//			LONG nStyle = 
//				::__EXT_MFC_GetWindowLong(
//					pDynDockMarkerWnd->GetSafeHwnd(), 
//					GWL_EXSTYLE
//					);
//			if( (nStyle & __EXT_MFC_WS_EX_LAYERED) != 0 )
//			{
//				::__EXT_MFC_SetWindowLong(
//					pDynDockMarkerWnd->GetSafeHwnd(), 
//					GWL_EXSTYLE, 
//					nStyle & ~__EXT_MFC_WS_EX_LAYERED
//					);
//				::RedrawWindow(
//					pDynDockMarkerWnd->GetSafeHwnd(), 
//					NULL, 
//					NULL, 
//					RDW_INVALIDATE | RDW_UPDATENOW
//					);
//			}
//		}
//		bAnimatingInProcess = false;
//	}
//	else
	{
	for( int nIndex = 0; nIndex < nCount; nIndex++ )
	{
		CExtDynDockMarkerWnd * pDynDockMarkerWnd =
			arrDockMarkerWnds[ nIndex ];
		ASSERT_VALID( pDynDockMarkerWnd );
		ASSERT( pDynDockMarkerWnd->GetSafeHwnd() != NULL );
		if( (pDynDockMarkerWnd->GetStyle()&WS_VISIBLE) != 0 )
		{
				if( !bShow )
					//pDynDockMarkerWnd->ShowWindow( SW_HIDE );
					pDynDockMarkerWnd->ShowMarker( false, bAnimate );
			}
		else
		{
			if( bShow )
					//pDynDockMarkerWnd->ShowWindow( SW_SHOWNOACTIVATE );
					pDynDockMarkerWnd->ShowMarker( true, bAnimate );
			}
	} // for( int nIndex = 0; nIndex < nCount; nIndex++ )
	}
	CExtPaintManager::stat_PassPaintMessages();
}

void CExtDynDockMarkerWnd::CreateOuterMarkers(
	CFrameWnd * pFrame,
	bool bShow, // = true
	CExtPaintManager * pPM // = NULL
	)
{
	ASSERT_VALID( pFrame );
	ASSERT_KINDOF( CFrameWnd, pFrame );
AFX_SIZEPARENTPARAMS layout;
	layout.bStretch = FALSE;
	layout.sizeTotal.cx = layout.sizeTotal.cy = 0;
		pFrame->GetClientRect(
			&layout.rect
			);
	layout.hDWP = NULL;
//HWND hWndLeftOver = NULL;
	for(	HWND hWndChild = ::GetTopWindow( pFrame->m_hWnd );
			hWndChild != NULL;
			hWndChild = ::GetWindow( hWndChild, GW_HWNDNEXT )
			)
	{
		UINT nIDC = _AfxGetDlgCtrlID(hWndChild);
		if( nIDC == AFX_IDW_PANE_FIRST )
		{
			//hWndLeftOver = hWndChild;
			continue;
		}
		CWnd * pWnd =
			CWnd::FromHandlePermanent( hWndChild );
		if( pWnd != NULL )
		{
			CExtDockBar * pExtDockBar =
				DYNAMIC_DOWNCAST(
					CExtDockBar,
					pWnd
					);
			if( pExtDockBar != NULL )
			{
				UINT nCircleNo = pExtDockBar->_GetCircleNo();
				if( nCircleNo >= 1 )
					break;
			} // if( pExtDockBar != NULL )
			CExtControlBar::QUERY_REPOSITION_CALC_EFFECT_DATA _qrced(
				(SHORT)reposQuery,
				pFrame,
				0,
				0x0FFFF,
				AFX_IDW_PANE_FIRST,
				&layout.rect,
				&layout.rect,
				false
				);
			pWnd->SendMessage(
				CExtControlBar::g_nMsgQueryRepositionCalcEffect,
				_qrced,
				0L
				);
			if( _qrced.ExcludeFromCenterGet() )
			{
				ASSERT( _qrced.IsQueryReposQuery() );
				continue;
			}
		} // if( pWnd != NULL )
		::SendMessage(
			hWndChild,
			WM_SIZEPARENT,
			0,
			(LPARAM)&layout
			);
	}
	if( pPM == NULL )
		pPM = g_PaintManager.GetPM();
CRect rcScreen( layout.rect );
	pFrame->ClientToScreen( &rcScreen );
	VERIFY(
		pPM->DockMarker_CreateWnds(
			NULL, //hWndLeftOver
			CExtPaintManager::__EDMT_2005_GROUP_SPARSED,
			rcScreen,
			g_arrDockMarkerWndsOuter,
			( INT(CExtControlBar::g_eResizablePanelDockingType) < 0 )
				? ( ( pPM == NULL ) ? g_PaintManager->GetThemeDockingType() : pPM->GetThemeDockingType() )
				: INT(CExtControlBar::g_eResizablePanelDockingType)
				,
			NULL
			)
		);
	ShowMarkers(
		g_arrDockMarkerWndsOuter,
		bShow,
		false
		);
}

//void CExtDynDockMarkerWnd::CreateInnerMarkers(
//	CFrameWnd * pFrame,
//	bool bShow // = true
//	)
//{
//	ASSERT_VALID( pFrame );
//	ASSERT_KINDOF( CFrameWnd, pFrame );
//CRect rcScreen;
//	pFrame->GetClientRect( &rcScreen );
//	CExtControlBar::RepositionBarsEx(
//		pFrame,
//		0,
//		0x0FFFF,
//		AFX_IDW_PANE_FIRST,
//		CWnd::reposQuery,
//		rcScreen,
//		rcScreen
//		);
//	pFrame->ClientToScreen( &rcScreen );
//	VERIFY(
//		PmBridge_GetPM()->DockMarker_CreateWnds(
//			CExtPaintManager::__EDMT_2005_GROUP_SPARSED,
//			rcScreen,
//			g_arrDockMarkerWndsInner,
//			NULL
//			)
//		);
//	ShowMarkers(
//		g_arrDockMarkerWndsInner,
//		bShow
//		);
//}

CExtDynDockMarkerWnd * CExtDynDockMarkerWnd::stat_FindMarker(
	HWND hWndMarkerTarget,
	int eMarkerType,
	bool bSparsedMode
	)
{
CTypedPtrArray < CPtrArray, CExtDynDockMarkerWnd * > & _arr =
		bSparsedMode
			? CExtDynDockMarkerWnd::g_arrDockMarkerWndsOuter
			: CExtDynDockMarkerWnd::g_arrDockMarkerWndsInner
			;
int nIndex, nCount = (int)_arr.GetSize();
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		CExtDynDockMarkerWnd * pDynDockMarkerWnd =
			_arr[ nIndex ];
		ASSERT_VALID( pDynDockMarkerWnd );
		if(		pDynDockMarkerWnd->m_eMarkerType == eMarkerType
			&&	(	( bSparsedMode && pDynDockMarkerWnd->m_bSparsedMode )
				||	( (!bSparsedMode) && (!pDynDockMarkerWnd->m_bSparsedMode) )
				)
			)
		{
			if( hWndMarkerTarget != NULL )
			{
				if( hWndMarkerTarget != pDynDockMarkerWnd->m_hWndMarkerTarget )
					continue;
			}
			return pDynDockMarkerWnd;
		}
	} // for( nIndex = 0; nIndex < nCount; nIndex ++ )
	return NULL;
}

HWND CExtDynDockMarkerWnd::stat_AnalyzeDynBar(
	CExtDynControlBar * pDynBar,
	CPoint ptCursor
	)
{
	ASSERT_VALID( pDynBar );
	ASSERT_KINDOF( CExtDynControlBar, pDynBar );
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	ASSERT( ! pDynBar->IsKindOf( RUNTIME_CLASS(CExtDynTabControlBar) ) );
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	ASSERT( pDynBar->GetSafeHwnd() != NULL );
CExtDockDynBar * pWndDynDocker = pDynBar->m_pWndDynDocker;
	ASSERT_VALID( pWndDynDocker );
	ASSERT_KINDOF( CExtDockDynBar, pWndDynDocker );
	ASSERT( pWndDynDocker->GetSafeHwnd() != NULL );
CExtControlBar * pDraggedBar =
		CExtControlBar::_DraggingGetBar();
	ASSERT_VALID( pDraggedBar );
ExtControlBarVector_t vBars;
	pWndDynDocker->CalcOrderedVector( vBars );
int nBarCount = (int)vBars.GetSize();
	for( int nBarIdx = 0; nBarIdx < nBarCount; nBarIdx++ )
	{
		CExtControlBar * pExtBar = vBars[ nBarIdx ];
		ASSERT_VALID( pExtBar );
		ASSERT_KINDOF( CExtControlBar, pExtBar );
		if(		pExtBar->IsFixedMode()
			||	pExtBar->IsFixedDockStyle()
			)
			continue;
		if( ! pExtBar->IsWindowVisible() )
			continue;
		CRect rcWnd;
		pExtBar->GetWindowRect( &rcWnd );
		if( ! rcWnd.PtInRect(ptCursor) )
			continue;
		HWND hWndDynamicTarget = pExtBar->GetSafeHwnd();
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		CExtDynTabControlBar * pDynTabControlBar =
			pExtBar->_GetNearestTabbedContainer();
		if( pDynTabControlBar != NULL )
			hWndDynamicTarget = pDynTabControlBar->GetSafeHwnd();
		else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		{
			CExtDynControlBar * pDynBar =
				DYNAMIC_DOWNCAST( CExtDynControlBar, pExtBar );
			if( pDynBar != NULL )
			{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
				pDynTabControlBar =
					DYNAMIC_DOWNCAST( CExtDynTabControlBar, pDynBar );
				if( pDynTabControlBar != NULL )
					hWndDynamicTarget = pDynTabControlBar->GetSafeHwnd();
				else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
				{
					hWndDynamicTarget =
						stat_AnalyzeDynBar(
							pDynBar,
							ptCursor
							);
					if( hWndDynamicTarget == NULL )
						continue;
					return hWndDynamicTarget;
				} // else from if( pDynTabControlBar != NULL )
			} // if( pDynBar != NULL )
		}
		return hWndDynamicTarget;
	} // for( int nBarIdx = 0; nBarIdx < nBarCount; nBarIdx++ )
	return NULL;
}

void CExtDynDockMarkerWnd::CreateDynamicMarkers(
	CFrameWnd * pFrame,
	bool bShow, // = true
	CExtPaintManager * pPM // = NULL
	)
{
	ASSERT_VALID( pFrame );
	ASSERT_KINDOF( CFrameWnd, pFrame );
	if( pPM == NULL )
		pPM = g_PaintManager.GetPM();
bool bInnerAreaFound = false;
HWND hWndDynamicTarget = NULL;
CPoint ptCursor;
	if( ::GetCursorPos( &ptCursor ) )
	{
		CExtControlBar * pDraggedBar =
			CExtControlBar::_DraggingGetBar();
		if( pDraggedBar == NULL )
			return;
		bool bFound = false;
		hWndDynamicTarget = ::WindowFromPoint( ptCursor );
		if( hWndDynamicTarget != NULL )
		{
			CWnd * pWnd =
				CWnd::FromHandlePermanent( hWndDynamicTarget );
			if( pWnd != NULL )
			{
				CExtDynDockMarkerWnd * pDynDockMarkerWnd =
					DYNAMIC_DOWNCAST( CExtDynDockMarkerWnd, pWnd );
				if( pDynDockMarkerWnd != NULL )
					return;
			} // if( pWnd != NULL )
		} // if( hWndDynamicTarget != NULL )
		if( ! bFound )
		{
			for(	POSITION pos = pFrame->m_listControlBars.GetHeadPosition();
					pos != NULL;
				)
			{
				CControlBar * pBar = (CControlBar *)
					pFrame->m_listControlBars.GetNext( pos );
				ASSERT_VALID( pBar );
				ASSERT_KINDOF( CControlBar, pBar );
				if(		pDraggedBar != NULL
					&&	LPVOID(pDraggedBar) == LPVOID(pBar)
					)
					continue;
				if( pBar->IsDockBar() )
					continue;
				CExtControlBar * pExtBar =
					DYNAMIC_DOWNCAST( CExtControlBar, pBar );
				if( pExtBar == NULL )
					continue;
				if(		pExtBar->IsFixedMode()
					||	pExtBar->IsFixedDockStyle()
					)
					continue;
				//DWORD dwStyle = pBar->GetStyle();
				//if( (dwStyle&WS_VISIBLE) == 0 )
				//	continue;
				if( ! pBar->IsWindowVisible() )
					continue;
				CFrameWnd * pParentFrame =
					pBar->GetParentFrame();
				if( pParentFrame == pFrame )
					continue;
				if( ! pParentFrame->IsKindOf(RUNTIME_CLASS(CExtMiniDockFrameWnd)) )
					continue;
				DWORD dwParentFrameStyle = pParentFrame->GetStyle();
				if( (dwParentFrameStyle&WS_VISIBLE) == 0 )
					continue;
				if(		pDraggedBar != NULL
					&&	pDraggedBar->GetParentFrame() == pParentFrame
					)
					continue;
				CRect rcWnd;
				pParentFrame->GetWindowRect( &rcWnd );
				if( ! rcWnd.PtInRect(ptCursor) )
					continue;
				pExtBar->GetWindowRect( &rcWnd );
				if( ! rcWnd.PtInRect(ptCursor) )
					continue;
				hWndDynamicTarget = pExtBar->GetSafeHwnd();
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
				CExtDynTabControlBar * pDynTabControlBar =
					pExtBar->_GetNearestTabbedContainer();
				if( pDynTabControlBar != NULL )
					hWndDynamicTarget = pDynTabControlBar->GetSafeHwnd();
				else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
				{
					CExtDynControlBar * pDynBar =
						DYNAMIC_DOWNCAST( CExtDynControlBar, pExtBar );
					if( pDynBar != NULL )
					{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
						pDynTabControlBar =
							DYNAMIC_DOWNCAST( CExtDynTabControlBar, pDynBar );
						if( pDynTabControlBar != NULL )
							hWndDynamicTarget = pDynTabControlBar->GetSafeHwnd();
						else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
						{
							hWndDynamicTarget =
								stat_AnalyzeDynBar(
									pDynBar,
									ptCursor
									);
							if( hWndDynamicTarget == NULL )
								continue;
							bFound = true;
							break;
						} // else from if( pDynTabControlBar != NULL )
					} // if( pDynBar != NULL )
				}
				bFound = true;
				break;
			}
			if( ! bFound )
			{
				for(	hWndDynamicTarget = ::GetNextWindow( pFrame->GetSafeHwnd(), GW_CHILD );
						hWndDynamicTarget != NULL;
						hWndDynamicTarget = ::GetNextWindow( hWndDynamicTarget, GW_HWNDNEXT )
						)
				{
					//DWORD dwStyle = (DWORD)
					//	::__EXT_MFC_GetWindowLong( hWndDynamicTarget, GWL_STYLE );
					//if( (dwStyle&WS_VISIBLE) == 0 )
					//	continue;
					if( ! ::IsWindowVisible( hWndDynamicTarget ) )
						continue;
					CRect rcWnd;
					::GetWindowRect( hWndDynamicTarget, &rcWnd );
					if( ! rcWnd.PtInRect(ptCursor) )
						continue;
					UINT nDlgCtrlID = (UINT)
						::GetDlgCtrlID( hWndDynamicTarget );
					if( nDlgCtrlID == AFX_IDW_PANE_FIRST )
					{
						bFound = bInnerAreaFound = true;
						break;
					} // if( nDlgCtrlID == AFX_IDW_PANE_FIRST )
					CWnd * pWnd =
						CWnd::FromHandlePermanent( hWndDynamicTarget );
					if( pWnd == NULL )
						break;
					if( g_hWndLastDynamicTarget == hWndDynamicTarget )
					{
						bFound = true;
						break;
					}
					CExtDockBar * pDockBar =
						DYNAMIC_DOWNCAST( CExtDockBar, pWnd );
					if( pDockBar == NULL )
						break;
					if( pDockBar->_GetCircleNo() < 1 )
						break;
					for(	hWndDynamicTarget = ::GetNextWindow( hWndDynamicTarget, GW_CHILD );
							hWndDynamicTarget != NULL;
							hWndDynamicTarget = ::GetNextWindow( hWndDynamicTarget, GW_HWNDNEXT )
							)
					{
						__EXT_MFC_ULONG_PTR dwStyle = ::__EXT_MFC_GetWindowLong( hWndDynamicTarget, GWL_STYLE );
						if( (dwStyle&WS_VISIBLE) == 0 )
							continue;
						CRect rcWnd;
						::GetWindowRect( hWndDynamicTarget, &rcWnd );
						if( ! rcWnd.PtInRect(ptCursor) )
							continue;
						CWnd * pWnd =
							CWnd::FromHandlePermanent( hWndDynamicTarget );
						if( pWnd == NULL )
							break;
						if( g_hWndLastDynamicTarget == hWndDynamicTarget )
						{
							bFound = true;
							break;
						}
						CExtControlBar * pBar =
							DYNAMIC_DOWNCAST( CExtControlBar, pWnd );
						if( pBar == NULL )
							continue;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
						CExtDynTabControlBar * pDynTabControlBar =
							pBar->_GetNearestTabbedContainer();
						if( pDynTabControlBar != NULL )
							hWndDynamicTarget = pDynTabControlBar->GetSafeHwnd();
						else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
						{
							CExtDynControlBar * pDynBar =
								DYNAMIC_DOWNCAST( CExtDynControlBar, pBar );
							if( pDynBar != NULL )
							{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
								pDynTabControlBar =
									DYNAMIC_DOWNCAST( CExtDynTabControlBar, pDynBar );
								if( pDynTabControlBar != NULL )
									hWndDynamicTarget = pDynTabControlBar->GetSafeHwnd();
								else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
								{
									hWndDynamicTarget =
										stat_AnalyzeDynBar(
											pDynBar,
											ptCursor
											);
									if( hWndDynamicTarget == NULL )
										continue;
									bFound = true;
									break;
								} // else from if( pDynTabControlBar != NULL )
							} // if( pDynBar != NULL )
						}
						bFound = true;
						break;
					}
					break;
				}
			} // if( ! bFound )
		} // if( ! bFound )
		if( ! bFound )
			hWndDynamicTarget = NULL;
	} // if( ::GetCursorPos( &ptCursor ) )

	if(		g_hWndLastDynamicTarget != NULL
		&&	(! ::IsWindow(g_hWndLastDynamicTarget) )
		)
		g_hWndLastDynamicTarget = NULL;
	if( g_hWndLastDynamicTarget == hWndDynamicTarget )
	{
		if( g_hWndLastDynamicTarget == NULL )
			DestroyMarkers( g_arrDockMarkerWndsInner, true );
		return;
	}
	DestroyMarkers( g_arrDockMarkerWndsInner, true );
	if( hWndDynamicTarget != NULL )
	{
		CWnd * pWndPermanent = CWnd::FromHandlePermanent( hWndDynamicTarget );
		if(		pWndPermanent != NULL
			&&	pWndPermanent->IsKindOf( RUNTIME_CLASS(CExtControlBar) )
			&&	(!(	((CExtControlBar*)pWndPermanent)->IsFixedDockStyle()
				||	((CExtControlBar*)pWndPermanent)->IsFixedMode()
				))
			)
		{
			CExtControlBar * pBarDst = (CExtControlBar*)
				pWndPermanent;
			CExtControlBar * pBarSrc =
				CExtControlBar::_DraggingGetBar();
			ASSERT_VALID( pBarSrc );
			if(		(! pBarSrc->_CanDockToTabbedContainers(pBarDst) )
				||	(! pBarDst->_CanDockToTabbedContainers(pBarSrc) )
				)
			{
				if(		(! pBarSrc->_CanDockLTRB(pBarDst) )
					||	(! pBarDst->_CanDockLTRB(pBarSrc) )
					)
				{
					g_hWndLastDynamicTarget = NULL;
					return;
				}
				bInnerAreaFound = false;
			}
		}
	} // if( hWndDynamicTarget != NULL )
	g_hWndLastDynamicTarget = hWndDynamicTarget;
	if( g_hWndLastDynamicTarget != NULL )
	{
		CRect rcScreen;
		::GetWindowRect( g_hWndLastDynamicTarget, &rcScreen );
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		VERIFY(
			pPM->DockMarker_CreateWnds(
				g_hWndLastDynamicTarget,
				bInnerAreaFound
					? CExtPaintManager::__EDMT_2005_GROUP_CENTRAL
					: CExtPaintManager::__EDMT_2005_GROUP_TAB
					,
				rcScreen,
				g_arrDockMarkerWndsInner,
				( INT(CExtControlBar::g_eResizablePanelDockingType) < 0 )
					? ( ( pPM == NULL ) ? g_PaintManager->GetThemeDockingType() : pPM->GetThemeDockingType() )
					: INT(CExtControlBar::g_eResizablePanelDockingType)
					,
				NULL
				)
			);
#else // (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		VERIFY(
			pPM->DockMarker_CreateWnds(
				g_hWndLastDynamicTarget,
				CExtPaintManager::__EDMT_2005_GROUP_CENTRAL,
				rcScreen,
				g_arrDockMarkerWndsInner,
				( INT(CExtControlBar::g_eResizablePanelDockingType) < 0 )
					? ( ( pPM == NULL ) ? g_PaintManager->GetThemeDockingType() : pPM->GetThemeDockingType() )
					: INT(CExtControlBar::g_eResizablePanelDockingType)
					,
				NULL
				)
			);
#endif // else from (!defined __EXT_MFC_NO_TAB_CONTROLBARS )
		ShowMarkers(
			g_arrDockMarkerWndsInner,
			bShow,
			true
			);
	} // if( g_hWndLastDynamicTarget != NULL )
}

bool CExtDynDockMarkerWnd::g_bDockMarkerClassRegistered = false;

bool CExtDynDockMarkerWnd::RegisterDockMarkerClass()
{
	if( g_bDockMarkerClassRegistered )
		return true;
WNDCLASS _wndClassInfo;
HINSTANCE hInst = ::AfxGetInstanceHandle();
	if( ! ::GetClassInfo( hInst, __EXT_DYNDOCKMARKER_CLASS_NAME, &_wndClassInfo ) )
	{
		_wndClassInfo.style = CS_GLOBALCLASS; //|CS_DBLCLKS;
		_wndClassInfo.lpfnWndProc = ::DefWindowProc;
		_wndClassInfo.cbClsExtra = _wndClassInfo.cbWndExtra = 0;
		_wndClassInfo.hInstance = hInst;
		_wndClassInfo.hIcon = NULL;
		_wndClassInfo.hCursor = ::LoadCursor( NULL, IDC_ARROW );
		ASSERT( _wndClassInfo.hCursor != NULL );
		_wndClassInfo.hbrBackground = NULL; 
		_wndClassInfo.lpszMenuName = NULL;
		_wndClassInfo.lpszClassName = __EXT_DYNDOCKMARKER_CLASS_NAME;
		if( !::AfxRegisterClass( &_wndClassInfo ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}
	g_bDockMarkerClassRegistered = true;
	return true;
}

BOOL CExtDynDockMarkerWnd::PreCreateWindow( CREATESTRUCT & cs ) 
{
	if(		( !RegisterDockMarkerClass() )
		||	( ! CWnd::PreCreateWindow(cs) )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	
	cs.lpszClass = __EXT_DYNDOCKMARKER_CLASS_NAME;

	return TRUE;
}

void CExtDynDockMarkerWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
//	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
	bCalcValidRects;
	lpncsp;
}

int CExtDynDockMarkerWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
//	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
	pDesktopWnd;
	nHitTest;
	message;
	return MA_NOACTIVATEANDEAT;
}

void CExtDynDockMarkerWnd::_RenderImpl( CDC * pDC )
{
	ASSERT_VALID( this );
	ASSERT( pDC != NULL );
	ASSERT( pDC->GetSafeHdc() != NULL );

CRect rcClient;
	GetClientRect( &rcClient );
// CDC dcMemory;
// 	if( dcMemory.CreateCompatibleDC( pDC ) )
// 	{
		if( ! m_bmpOuterBk.IsEmpty() )
		{
// 			CBitmap * pOldBmp =
// 				dcMemory.SelectObject( &m_bmpOuterBk );
// 			pDC->BitBlt(
// 				rcClient.left,
// 				rcClient.top,
// 				rcClient.Width(),
// 				rcClient.Height(),
// 				&dcMemory,
// 				0,
// 				0,
// 				SRCCOPY
// 				);
// 			dcMemory.SelectObject( pOldBmp );
			m_bmpOuterBk.AlphaBlend(
				pDC->GetSafeHdc(),
				rcClient
				);
		}
		CExtBitmap * pBmpInner =
			(m_eMTS == __EMTS_HOVER)
				? (&m_bmpIn)
				: (&m_bmpOut)
				;
		CSize _size = pBmpInner->GetSize();
		ASSERT( _size.cx > 0 && _size.cy > 0 );
// 		CBitmap * pOldBmp =
// 			dcMemory.SelectObject( pBmpInner );
// 		pDC->BitBlt(
// 			rcClient.left + m_ptOffsetInner.x,
// 			rcClient.top + m_ptOffsetInner.y,
// 			bmpDescription.bmWidth,
// 			bmpDescription.bmHeight,
// 			&dcMemory,
// 			0,
// 			0,
// 			SRCCOPY
// 			);
// 		dcMemory.SelectObject( pOldBmp );
		pBmpInner->AlphaBlend(
			pDC->GetSafeHdc(),
			rcClient.left + m_ptOffsetInner.x,
			rcClient.top + m_ptOffsetInner.y,
			_size.cx,
			_size.cy
			);
		PmBridge_GetPM()->DockMarker_PostRender(
			*pDC,
			rcClient,
			(CExtPaintManager::eDockMarkerType_t)m_eMarkerType,
			( m_eMTS == __EMTS_HOVER ) ? true : false,
			(CObject *)this
			);
// 	} // if( dcMemory.CreateCompatibleDC( &dcPaint ) )
// #ifdef _DEBUG
// 	else
// 	{
// 		ASSERT( FALSE );
// 	} // else from if( dcMemory.CreateCompatibleDC( &dcPaint ) )
// #endif // _DEBUG
}

LRESULT CExtDynDockMarkerWnd::OnPrintClient( WPARAM wParam, LPARAM lParam )
{
	lParam;
LRESULT lResult = Default();
CDC * pDC = CDC::FromHandle( (HDC)wParam );
	_RenderImpl( pDC );
    return lResult;
}

void CExtDynDockMarkerWnd::OnPaint() 
{
CPaintDC dcPaint( this );
	_RenderImpl( &dcPaint );
}

void CExtDynDockMarkerWnd::OnTimer(__EXT_MFC_UINT_PTR nIDEvent) 
{
	if( nIDEvent == __DYNDOCKMARKER_TIMER_ID_ANIMATE  )
	{
		if(		(m_nOpaquePercent <= 0 && !m_bAnimationShow)
			||	(m_nOpaquePercent >= 100 && m_bAnimationShow)
			)
		{
			AnimationStop();
			return;
		}
		m_nOpaquePercent += 
			m_bAnimationShow 
				? + __DYNDOCKMARKER_ANIMATION_PERCENT_STEP
				: - __DYNDOCKMARKER_ANIMATION_PERCENT_STEP;

		if(		g_PaintManager.m_bIsWin2000orLater 
			&&	g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL
			&&	CExtPaintManager::stat_GetBPP() > 8
			)
		{
			g_PaintManager.m_pfnSetLayeredWindowAttributes(
				GetSafeHwnd(), 
				0, 
				BYTE((g_PaintManager->DockMarker_GetAlpha( false, this ) * m_nOpaquePercent) / 100), 
				__EXT_MFC_LWA_ALPHA
				);
			::RedrawWindow(
				GetSafeHwnd(), 
				NULL, 
				NULL, 
				RDW_INVALIDATE | RDW_UPDATENOW
				);
		}
		return;
	} // if( nIDEvent == __DYNDOCKMARKER_TIMER_ID_ANIMATE  )
	else if( nIDEvent == __DYNDOCKMARKER_TIMER_ID_TEMP  )
	{
		if( (GetStyle()&WS_VISIBLE) == 0 )
			return;
		if( m_hWndMarkerTarget != NULL )
		{
			if( ! ( ::IsWindow( m_hWndMarkerTarget ) ) )
			{
				ShowMarkers( g_arrDockMarkerWndsInner, false, true );
				CExtControlBar * pDraggedBar =
					CExtControlBar::_DraggingGetBar();
				if( pDraggedBar == NULL )
					return;
				ASSERT_VALID( pDraggedBar );
				CPoint ptCursor( 0, 0 );
				::GetCursorPos( &ptCursor );
				pDraggedBar->ScreenToClient( &ptCursor );
				pDraggedBar->PostMessage(
					WM_MOUSEMOVE,
					CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL) ? (VK_CONTROL) : 0L,
					MAKELPARAM( ptCursor.x, ptCursor.y )
					);
				return;
			} // if( ! ( ::IsWindow( m_hWndMarkerTarget ) ) )
		} // if( m_hWndMarkerTarget != NULL )
		eMouseTrackState_t eMTS = __EMTS_OUT;
		CPoint ptCursor;
		if( ::GetCursorPos( &ptCursor ) )
		{
			HWND hWnd = ::WindowFromPoint( ptCursor );
			if( m_hWnd == hWnd )
				eMTS = __EMTS_HOVER;
		} // if( ::GetCursorPos( &ptCursor ) )
		if( m_eMTS == eMTS )
			return;
		m_eMTS = eMTS;
		if( eMTS != __EMTS_HOVER )
			HighlightMarkerTarget( false );
		//m_hWndLastHoverDepenentMarker = NULL;
		if(		(! m_bSparsedMode )
			&&	m_eMarkerType != int(CExtPaintManager::__EDMT_2005_CENTER)
			)
		{
			CExtDynDockMarkerWnd * pDynDockMarkerWnd =
				stat_FindMarker(
					m_hWndMarkerTarget,
					int(CExtPaintManager::__EDMT_2005_CENTER),
					m_bSparsedMode
					);
			if( pDynDockMarkerWnd->GetSafeHwnd() != NULL )
			{
				if( eMTS == __EMTS_HOVER )
				{
					pDynDockMarkerWnd->m_hWndLastHoverDepenentMarker = m_hWnd;
					pDynDockMarkerWnd->Invalidate();
					pDynDockMarkerWnd->UpdateWindow();
				}
				else if( pDynDockMarkerWnd->m_hWndLastHoverDepenentMarker == m_hWnd )
				{
					pDynDockMarkerWnd->m_hWndLastHoverDepenentMarker = NULL;
					pDynDockMarkerWnd->Invalidate();
					pDynDockMarkerWnd->UpdateWindow();
				}
			}
		} // if( ! m_bSparsedMode ...
		//if( ! IsStateApplyable() )
		//	UnHighlightAllMarkers();
		Invalidate();
		UpdateWindow();
		return;
	} // if( nIDEvent == __DYNDOCKMARKER_TIMER_ID_TEMP  )
	CWnd::OnTimer(nIDEvent);
}

BOOL CExtDynDockMarkerWnd::DestroyWindow() 
{
	HighlightMarkerTarget( false );
	return CWnd::DestroyWindow();
}

#endif // (!defined __EXT_MFC_NO_DOCK_MARKERS)

void CExtControlBar::MsgDrawTrackedAreaData_t::NotifyOrDrawDefault()
{
	if( ! Notify() )
		DrawDefault();
}

bool CExtControlBar::MsgDrawTrackedAreaData_t::Notify()
{
	ASSERT_VALID( (&m_wndBar) );
	ASSERT( m_wndBar.GetSafeHwnd() != NULL );
	ASSERT( m_dcDraw.GetSafeHdc() != NULL );
CFrameWnd * pFrame = m_wndBar.GetParentFrame();
	if( pFrame == NULL )
		return false;
	if( pFrame->IsKindOf( RUNTIME_CLASS(CExtMiniDockFrameWnd) ) )
	{
		pFrame = pFrame->GetParentFrame();
		if( pFrame == NULL )
			return false;
	} // if( pFrame->IsKindOf( RUNTIME_CLASS(CExtMiniDockFrameWnd) ) )
	pFrame->SendMessage(
		CExtControlBar::g_nMsgDrawTrackedArea,
		WPARAM( this )
		);
	return m_bMessageHandled;
}

void CExtControlBar::MsgDrawTrackedAreaData_t::DrawDefault()
{
	ASSERT_VALID( (&m_wndBar) );
	ASSERT( m_wndBar.GetSafeHwnd() != NULL );
	ASSERT( m_dcDraw.GetSafeHdc() != NULL );
HBRUSH hBrush = (HBRUSH) CDC::GetHalftoneBrush()->GetSafeHandle();
	ASSERT( hBrush != NULL );
	Draw( hBrush, PATINVERT );
}

void CExtControlBar::MsgDrawTrackedAreaData_t::Draw(
	HBRUSH hBrush,
	DWORD dwPatBltROP // raster operation code for invoking PatBlt()
	)
{
	ASSERT_VALID( (&m_wndBar) );
	ASSERT( m_wndBar.GetSafeHwnd() != NULL );
	ASSERT( m_dcDraw.GetSafeHdc() != NULL );
	ASSERT( hBrush != NULL );
HGDIOBJ hBrushOld = m_dcDraw.SelectObject( hBrush );
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
bool bTabShape = false;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	switch( m_eDTAT )
	{
	case __EDTAT_RESIZING_ENTIRE_ROW:
	case __EDTAT_RESIZING_PART_OF_ROW:
		m_dcDraw.PatBlt(
			m_rcDrawArea.left,
			m_rcDrawArea.top,
			m_rcDrawArea.Width(),
			m_rcDrawArea.Height(),
			dwPatBltROP
			);
	break;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	case __EDTAT_TAB_FRAME:
		bTabShape = true;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	case __EDTAT_RECTANGULAR_FRAME:
	{
INT nTrackerBorderH = ::GetSystemMetrics( SM_CXSIZEFRAME );
INT nTrackerBorderV = ::GetSystemMetrics( SM_CYSIZEFRAME );
		CRect rcTrackerInner( m_rcDrawArea );
		rcTrackerInner.DeflateRect(
			nTrackerBorderH,
			nTrackerBorderV
			);

		bool bRgnCreated = false;
		CRgn
			rgnPaint,
			rgnInner, rgnOuter,
			rgnTabMainArea,
			rgnTabBottomLeftArea,
			rgnTabBottomMiddleArea,
			rgnTabBottomRightArea;

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( bTabShape )
		{
			int nTabHeight = CExtControlBar::g_nTabShapeMaxHeight;
			int nTrackerHeight = m_rcDrawArea.Height();
			if( nTrackerHeight < (CExtControlBar::g_nTabShapeMaxHeight * 2) )
				nTabHeight = nTrackerHeight / 2;
			//if( nTabHeight >= (nTrackerBorderV * 3 + 3) )
			//{
				CRect rcTabMainArea( rcTrackerInner );
				if( CExtControlBar::g_bTabsAtTop )
					rcTabMainArea.top += nTabHeight;
				else
					rcTabMainArea.bottom -= nTabHeight;
				ASSERT( rcTabMainArea.bottom > rcTabMainArea.top );
				
				CRect rcTabBottomLeftArea( m_rcDrawArea );
				rcTabBottomLeftArea.right =
					rcTabBottomLeftArea.left
					+ CExtControlBar::g_nTabShapeBottomLeftAreaWidth
					;
				if( CExtControlBar::g_bTabsAtTop )
					rcTabBottomLeftArea.bottom =
						rcTabBottomLeftArea.top + nTabHeight;
				else
					rcTabBottomLeftArea.top =
						rcTabBottomLeftArea.bottom - nTabHeight;

				CRect rcTabBottomMiddleArea( rcTabBottomLeftArea );
				rcTabBottomMiddleArea.OffsetRect(
					nTrackerBorderH + rcTabBottomLeftArea.Width(),
					CExtControlBar::g_bTabsAtTop ? nTrackerBorderV : (-nTrackerBorderV)
					);
				rcTabBottomMiddleArea.right =
					rcTabBottomMiddleArea.left
					+ CExtControlBar::g_nTabShapeBottomMiddleAreaWidth
					;
				if( rcTabBottomMiddleArea.right > rcTrackerInner.right )
					rcTabBottomMiddleArea.right = 
						rcTabBottomMiddleArea.left
						+ (rcTrackerInner.right - rcTabBottomMiddleArea.left) / 2
						;
				
				CRect rcTabBottomRightArea( rcTabBottomLeftArea );
				rcTabBottomRightArea.right = m_rcDrawArea.right;
				rcTabBottomRightArea.left =
					rcTabBottomMiddleArea.right
					+ nTrackerBorderH;

				if( m_wndBar.m_pDockSite != NULL )
				{
					ASSERT_VALID( m_wndBar.m_pDockSite );
					ASSERT( m_wndBar.m_pDockSite->GetSafeHwnd() != NULL );
					if( (m_wndBar.m_pDockSite->GetExStyle()&WS_EX_LAYOUTRTL) != 0 )
					{
						rcTabBottomRightArea.left =
							rcTabBottomRightArea.right
							- rcTabBottomLeftArea.Width()
							- nTrackerBorderH
							;
						rcTabBottomMiddleArea.OffsetRect(
							rcTabBottomRightArea.left
							- rcTabBottomMiddleArea.right
							- nTrackerBorderH
							,
							0
							);
						rcTabBottomLeftArea.right =
							rcTabBottomMiddleArea.left
							- nTrackerBorderH
							;
					} // if( (m_wndBar.m_pDockSite->GetExStyle()&WS_EX_LAYOUTRTL) != 0 )
				} // if( m_wndBar.m_pDockSite != NULL )

				if(		rgnOuter.CreateRectRgnIndirect(
							&m_rcDrawArea
							)
					&&	rgnTabMainArea.CreateRectRgnIndirect(
							&rcTabMainArea
							)
					&&	rgnTabBottomLeftArea.CreateRectRgnIndirect(
							&rcTabBottomLeftArea
							)
					&&	rgnTabBottomMiddleArea.CreateRectRgnIndirect(
							&rcTabBottomMiddleArea
							)
					&&	rgnTabBottomRightArea.CreateRectRgnIndirect(
							&rcTabBottomRightArea
							)
					&&	rgnPaint.CreateRectRgnIndirect(
							&m_rcDrawArea
							)
					)
				{ // if regions created successfully
					int nRgnPaintCombineResult = 
						rgnTabMainArea.CombineRgn(
							&rgnTabMainArea,
							&rgnTabBottomLeftArea,
							RGN_OR
							);
					if( nRgnPaintCombineResult != ERROR
						&& nRgnPaintCombineResult != NULLREGION
						)
					{
						nRgnPaintCombineResult = 
							rgnTabMainArea.CombineRgn(
								&rgnTabMainArea,
								&rgnTabBottomMiddleArea,
								RGN_OR
								);
						if( nRgnPaintCombineResult != ERROR
							&& nRgnPaintCombineResult != NULLREGION
							)
						{
							nRgnPaintCombineResult = 
								rgnTabMainArea.CombineRgn(
									&rgnTabMainArea,
									&rgnTabBottomRightArea,
									RGN_OR
									);
							if( nRgnPaintCombineResult != ERROR
								&& nRgnPaintCombineResult != NULLREGION
								)
							{
								nRgnPaintCombineResult = 
									rgnPaint.CombineRgn(
										&rgnPaint,
										&rgnTabMainArea,
										RGN_DIFF
										);
								if( nRgnPaintCombineResult != ERROR
									&& nRgnPaintCombineResult != NULLREGION
									)
								{ // if paint region created successfully & non-empty
									bRgnCreated = true;
								} // if paint region created successfully & non-empty
							}
						}
					}
				} // if regions created successfully
			//} // if( nTabHeight >= (nTrackerBorderV * 3  + 3) )
		} // if( bTabShape )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		
		if( ! bRgnCreated )
		{ // create non-tabbed shape
			if(		rgnOuter.CreateRectRgnIndirect(
						&m_rcDrawArea
						)
				&&	rgnInner.CreateRectRgnIndirect(
						&rcTrackerInner
						)
				&&	rgnPaint.CreateRectRgn(0,0,0,0)
				)
			{ // if regions created successfully
				int nRgnPaintCombineResult =
					rgnPaint.CombineRgn(
						&rgnOuter,
						&rgnInner,
						RGN_DIFF
						);
				if( nRgnPaintCombineResult != ERROR
					&& nRgnPaintCombineResult != NULLREGION
					)
				{ // if paint region created successfully & non-empty
					bRgnCreated = true;
				} // if paint region created successfully & non-empty
			} // if regions created successfully
		} // create non-tabbed shape

		if( bRgnCreated )
		{ // if we have sense to paint pattern
			m_dcDraw.SelectClipRgn( &rgnPaint );
			m_dcDraw.PatBlt(
				m_rcDrawArea.left,
				m_rcDrawArea.top,
				m_rcDrawArea.Width(),
				m_rcDrawArea.Height(),
				dwPatBltROP
				);
			m_dcDraw.SelectClipRgn( NULL );
		} // if we have sense to paint pattern
	}
	break;
#ifdef _DEBUG
	default:
		{
			ASSERT( FALSE );
		}
	break;
#endif // _DEBUG
	} // switch( m_eDTAT )
	m_dcDraw.SelectObject( hBrushOld );
}

#if (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)

/////////////////////////////////////////////////////////////////////////////
// CExtDynamicBarNcAreaButtonMenu

IMPLEMENT_DYNCREATE( CExtDynamicBarNcAreaButtonMenu, CExtBarNcAreaButtonMenu );

CExtDynamicBarNcAreaButtonMenu::CExtDynamicBarNcAreaButtonMenu(
	CExtControlBar * pBar // = NULL
	)
	: CExtBarNcAreaButtonMenu( pBar )
{
}

bool CExtDynamicBarNcAreaButtonMenu::OnQueryVisibility() const
{
	ASSERT_VALID( this );
bool bButtonIsVisible = true;
CExtControlBar * pBar = const_cast < CExtControlBar * > ( GetBar() );
	ASSERT_VALID( pBar );
	if(	pBar->NcButtons_HandleQueryVisibility(
			bButtonIsVisible,
			const_cast < CExtDynamicBarNcAreaButtonMenu * > ( this ),
			pBar,
			NULL
			)
		)
		return bButtonIsVisible;

	if( ! pBar->IsKindOf( RUNTIME_CLASS(CExtDynamicControlBar) ) )
	{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		CExtDynamicTabbedControlBar * pDTB =
			DYNAMIC_DOWNCAST( CExtDynamicTabbedControlBar, pBar );
		if( pDTB == NULL )
			return false;
		LONG nSel = pDTB->GetSwitcherSelection();
		if( nSel < 0 )
			return false;
		CExtControlBar * pSelBar = pDTB->GetBarAt( nSel, true );
		if( pSelBar == NULL )
			return false;
		if( ! pSelBar->IsKindOf( RUNTIME_CLASS(CExtDynamicControlBar) ) )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			return false;
	}
	if( ! pBar->_NcMenuButtonIsEnabled( pBar ) )
		return false;
	if( IsBarFixedMode() )
		return false;
	return true;
}

bool CExtDynamicBarNcAreaButtonMenu::OnQueryDeactivateAutohideSlider() const
{
	ASSERT_VALID( this );
	return false;
}

bool CExtDynamicBarNcAreaButtonMenu::OnNcAreaClicked( CPoint point )
{
	ASSERT_VALID( this );
	m_bHover = m_bPushed = false;
CExtControlBar * pExtBar = GetBar();
	ASSERT_VALID( pExtBar );
HWND hWndBar = pExtBar->GetSafeHwnd();
	if( pExtBar->NcButtons_HandleClick( this, point, pExtBar, NULL ) )
		return true;
	ASSERT_VALID( pExtBar );

CExtDynamicControlBar * pBar =
		DYNAMIC_DOWNCAST(
			CExtDynamicControlBar,
			pExtBar
			);
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( pBar == NULL )
	{
		CExtDynamicTabbedControlBar * pTabbedBar =
			STATIC_DOWNCAST(
				CExtDynamicTabbedControlBar,
				pExtBar
				);
		LONG nSel = pTabbedBar->GetSwitcherSelection();
		if( nSel < 0 )
		{
			if( pTabbedBar->GetSwitcherItemCount() == 0 )
				return true;
			nSel = 0;
		}
		pBar =
			DYNAMIC_DOWNCAST(
				CExtDynamicControlBar,
				pTabbedBar->GetBarAt(
					nSel,
					true
					)
				);
		if( pBar == NULL )
			return true;
	} // if( pBar == NULL )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = pBar->NotifiedCustomizeSiteGet();
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
	{
		if(		hWndBar != NULL
			&&	::IsWindow( hWndBar )
			)
			pExtBar->NcButtons_PostClick( this, point, pExtBar, NULL );
		return true;
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( CExtControlBar::FindHelpMode( pBar ) )
	{
		return true;
	}
	if( ! m_rc.PtInRect( point ) )
	{
		if(		hWndBar != NULL
			&&	::IsWindow( hWndBar )
			)
			pExtBar->NcButtons_PostClick( this, point, pExtBar, NULL );
		return false; // continue asking nc-buttons
	}
//CFrameWnd * pFrame = GetDockingFrame();
//	ASSERT_VALID( pFrame );
//HWND hWndTrack = pFrame->GetSafeHwnd();
HWND hWndTrack = pBar->GetSafeHwnd();
	ASSERT(
			hWndTrack != NULL
		&&	::IsWindow( hWndTrack )
		);
CExtPopupMenuWnd * pPopup =
		CExtPopupMenuWnd::InstantiatePopupMenu(
			pExtBar->GetSafeHwnd(),
			RUNTIME_CLASS(CExtPopupMenuWnd),
			this
			);
	if( ! pPopup->CreatePopupMenu( hWndTrack ) )
	{
		ASSERT( FALSE );
		delete pPopup;
		if(		hWndBar != NULL
			&&	::IsWindow( hWndBar )
			)
			pExtBar->NcButtons_PostClick( this, point, pExtBar, NULL );
		return false;
	}
CExtControlBar::POPUP_MENU_EVENT_DATA _pmed(
		CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_CONTROLBAR_NCBTNMENU_DYNSTATE,
		pPopup,
		m_pBar,
		CPoint( -1, -1 ),
		WM_NCLBUTTONDOWN,
		this
		);
	if( ! _pmed.NotifyTarget( false ) )
	{
		if( ! pBar->OnInitDbsMenu(
				pPopup,
				hWndTrack,
				this
				)
			)
		{
			ASSERT( FALSE );
			delete pPopup;
			if(		hWndBar != NULL
				&&	::IsWindow( hWndBar )
				)
				pExtBar->NcButtons_PostClick( this, point, pExtBar, NULL );
			return false;
		}
	} // if( ! _pmed.NotifyTarget( false ) )
	_pmed.NotifyTarget( true );
	if( pPopup->ItemGetCount() == 0 )
	{
		delete pPopup;
		if(		hWndBar != NULL
			&&	::IsWindow( hWndBar )
			)
			pExtBar->NcButtons_PostClick( this, point, pExtBar, NULL );
		return false;
	}
CRect rcBarWnd;
	VERIFY( GetBarNcAreaInfo( NULL, &rcBarWnd ) );
CRect rcBtnScreen( m_rc );
	m_bHover = m_bPushed = true;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( pBar->AutoHideModeGet() )
	{
		CExtDynAutoHideArea * pTabs =
			CExtDynAutoHideArea::stat_FindAreaOfBar( pBar );
		if( pTabs == NULL )
			return true;
		CExtDynAutoHideSlider * pSlider =
			pTabs->GetAutoHideSlider();
		ASSERT_VALID( pSlider );
		if( CExtControlBar::stat_QueryFocusChangingEnabled( pBar, pSlider->m_hWnd ) )
			pSlider->SetFocus();
		pSlider->PostMessage( WM_NCPAINT );
	} // if( pBar->AutoHideModeGet() )
	else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	{
		if( CExtControlBar::stat_QueryFocusChangingEnabled( pBar, pBar->m_hWnd ) )
			pBar->SetFocus();
		((CExtDynamicControlBar *)pBar)->m_bWindowActive = true;
		pBar->PostMessage( WM_NCPAINT );
		HWND hWndBar = pBar->m_hWnd;
		CExtPopupMenuWnd::PassMsgLoop( true );
		if( ! ::IsWindow(hWndBar) )
			return true;
	}
	CExtPaintManager::stat_PassPaintMessages();
	rcBtnScreen.InflateRect( 1, 1 );
	rcBtnScreen.OffsetRect( rcBarWnd.TopLeft() );
CPoint ptCursor = rcBtnScreen.CenterPoint();
	if(	! pPopup->TrackPopupMenu(
			TPMX_TOPALIGN|TPMX_COMBINE_NONE|TPMX_OWNERDRAW_FIXED,
			ptCursor.x,  ptCursor.y,
			&rcBtnScreen,
			this,
			_CbPaintCombinedContent,
			NULL,
			true
			)
		)
	{
		//delete pPopup;
		if(		hWndBar != NULL
			&&	::IsWindow( hWndBar )
			)
			pExtBar->NcButtons_PostClick( this, point, pExtBar, NULL );
		return false;
	}
	if(		hWndBar != NULL
		&&	::IsWindow( hWndBar )
		)
		pExtBar->NcButtons_PostClick( this, point, pExtBar, NULL );
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CExtDynamicBarNcAreaButtonAutoHide

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

IMPLEMENT_DYNCREATE( CExtDynamicBarNcAreaButtonAutoHide, CExtBarNcAreaButtonAutoHide );

CExtDynamicBarNcAreaButtonAutoHide::CExtDynamicBarNcAreaButtonAutoHide(
	CExtControlBar * pBar // = NULL
	)
	: CExtBarNcAreaButtonAutoHide( pBar )
{
}

bool CExtDynamicBarNcAreaButtonAutoHide::OnNcAreaClicked( CPoint point )
{
	ASSERT_VALID( this );
CExtControlBar * pExtBar = GetBar();
	ASSERT_VALID( pExtBar );
HWND hWndBar = pExtBar->GetSafeHwnd();
	if( pExtBar->NcButtons_HandleClick( this, point, pExtBar, NULL ) )
		return true;
	ASSERT_VALID( pExtBar );

bool bRetVal = CExtBarNcAreaButtonAutoHide::OnNcAreaClicked( point );
	if( bRetVal )
	{
		m_bHover = m_bPushed = false;
		CExtDynTabControlBar * pTabbedBar =
			DYNAMIC_DOWNCAST(
				CExtDynTabControlBar,
				pExtBar
				);
		if( pTabbedBar == NULL )
		{
			CExtDockDynTabBar * pDynDockBar =
				DYNAMIC_DOWNCAST(
					CExtDockDynTabBar,
					pExtBar->GetParent()
					);
			if( pDynDockBar != NULL )
			{
				pTabbedBar =
					STATIC_DOWNCAST(
						CExtDynTabControlBar,
						pDynDockBar->GetParent()
						);
			} // if( pDynDockBar != NULL )
		} // if( pTabbedBar == NULL )
		if( pTabbedBar != NULL )
		{
			LONG nPos, nCount = pTabbedBar->GetSwitcherItemCount();
			for( nPos = 0; nPos < nCount; nPos++ )
			{
				CExtControlBar * pContainedBar =
					pTabbedBar->GetBarAt( nPos, true );
				CExtDynamicControlBar * pDynamicBar = DYNAMIC_DOWNCAST( CExtDynamicControlBar, pContainedBar );
				if( pDynamicBar != NULL )
				{
					if( pDynamicBar->m_eDBS != CExtDynamicControlBar::__EDBS_DOCUMENT )
						pDynamicBar->m_eDBS =
							pDynamicBar->AutoHideModeGet()
								? CExtDynamicControlBar::__EDBS_AUTO_HIDDEN
								: CExtDynamicControlBar::__EDBS_DOCKED
								;
				} // if( pDynamicBar != NULL )
			} // for( nPos = 0; nPos < nCount; nPos++ )
		} // if( pTabbedBar != NULL )
		else
		{
			CExtDynamicControlBar * pDynamicBar = DYNAMIC_DOWNCAST( CExtDynamicControlBar, pExtBar );
			if( pDynamicBar != NULL )
					pDynamicBar->m_eDBS =
						pDynamicBar->AutoHideModeGet()
							? CExtDynamicControlBar::__EDBS_AUTO_HIDDEN
							: CExtDynamicControlBar::__EDBS_DOCKED
							;
		} // else if( pTabbedBar != NULL )
//		ASSERT_VALID( pExtBar->m_pDockBar );
//		CExtDockBar * pExtDockBar =
//			STATIC_DOWNCAST(
//				CExtDockBar,
//				pExtBar->m_pDockBar
//				);
//		CExtDynAutoHideArea * pAutoHideArea =
//			pExtDockBar->_GetAutoHideArea();
//		ASSERT_VALID( pAutoHideArea );
//		CExtDynAutoHideSlider * pAutoHideSlider =
//			pAutoHideArea->GetAutoHideSlider();
//		ASSERT_VALID( pAutoHideSlider );
//		CExtPopupMenuWnd::PassMsgLoop( false );
//		pAutoHideSlider->PostMessage( WM_NCPAINT );
	}
	if(		hWndBar != NULL
		&&	::IsWindow( hWndBar )
		)
		pExtBar->NcButtons_PostClick( this, point, pExtBar, NULL );
	return bRetVal;
}

#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

/////////////////////////////////////////////////////////////////////////////
// CExtDynamicBarNcAreaButtonClose

IMPLEMENT_DYNCREATE( CExtDynamicBarNcAreaButtonClose, CExtBarNcAreaButtonClose );

CExtDynamicBarNcAreaButtonClose::CExtDynamicBarNcAreaButtonClose(
	CExtControlBar * pBar // = NULL
	)
	: CExtBarNcAreaButtonClose( pBar )
{
}

bool CExtDynamicBarNcAreaButtonClose::OnNcAreaClicked( CPoint point )
{
	ASSERT_VALID( this );
	if( ! m_rc.PtInRect( point ) )
		return false; // continue asking nc-buttons
	m_bHover = m_bPushed = false;
CExtControlBar * pExtBar = GetBar();
	ASSERT_VALID( pExtBar );
HWND hWndBar = pExtBar->GetSafeHwnd();
	CExtBarNcAreaButtonClose::OnNcAreaClicked( point );
	if(		hWndBar == NULL
		||	(! ::IsWindow( hWndBar ) )
		)
		return true;
 	if( pExtBar->NcButtons_HandleClick( this, point, pExtBar, NULL ) )
 		return true;
	if(		hWndBar == NULL
		||	(! ::IsWindow( hWndBar ) )
		)
		return true;
	ASSERT_VALID( pExtBar );
CExtDynamicControlBar * pBar =
		DYNAMIC_DOWNCAST(
			CExtDynamicControlBar,
			pExtBar
			);
	if( pBar != NULL )
		pBar->BarStateSet( pBar->BarStateGet(), false );
	if(		hWndBar != NULL
		&&	::IsWindow( hWndBar )
		)
		pExtBar->NcButtons_PostClick( this, point, pExtBar, NULL );
 	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CExtDynamicControlBar

IMPLEMENT_SERIAL( CExtDynamicControlBar, CExtControlBar, VERSIONABLE_SCHEMA|1 );

bool CExtDynamicControlBar::g_bInsertToHeadOfTPC = true;

CExtDynamicControlBar::CExtDynamicControlBar()
	: m_bPersistentBar( false )
	, m_pDBS( NULL )
	, m_eDBS( CExtDynamicControlBar::__EDBS_DOCKED )
	, m_hWndChildCached( NULL )
	, m_nMdiMenuResourceID( 0 )
	, m_nMdiAccelTableResourceID( 0 )
	, m_dwAdditionalStateFlags( CExtDynamicControlBar::__EASF_INIT_VALUE )
	, m_eHelperSwitchingDBS( CExtDynamicControlBar::__EDBS_DOCKED )
	, m_bHelperSwitchingIntoVisibleState( false )
	, m_bInsertToHeadOfTPC( g_bInsertToHeadOfTPC )
#if (! defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL) || (! defined __EXT_MFC_NO_TABMDI_CTRL)
	, m_bEnableDndDocumentTabs( true )
#endif // (! defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL) || (! defined __EXT_MFC_NO_TABMDI_CTRL)
{
}

CExtDynamicControlBar::~CExtDynamicControlBar()
{
	if( m_pDockContext != NULL )
	{
		m_pDockContext->m_pDockSite = NULL;
		delete m_pDockContext;
		m_pDockContext = NULL;
	} // if( m_pDockContext != NULL )
}

#if (! defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL) || (! defined __EXT_MFC_NO_TABMDI_CTRL)

bool CExtDynamicControlBar::OnStartDraggingDocumentTabItem(
	CExtTabWnd * pWndTab,
	LONG nTabItemIndex
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pWndTab );
	pWndTab;
	nTabItemIndex;
	if( ! m_bEnableDndDocumentTabs )
		return false;
	if( ! _DraggingIsEnabled( this ) )
		return true;
CPoint point;
	if( ! ::GetCursorPos( &point ) )
		return false;
	if( ! BarStateSet( __EDBS_DOCKED, false ) )
	{
		ASSERT( FALSE );
		return true;
	}
bool bShowContentWhenDragging = _IsShowContentWhenDragging();
CPoint ptFloat( point );
	ptFloat.x -= 15;
	ptFloat.y -= 45;
	ASSERT_VALID( m_pDockSite );
	m_pDockSite->ShowControlBar( this, TRUE, TRUE );
CExtControlBar * pFloatBar = this;
CFrameWnd * pParentFrame = GetParentFrame();
	ASSERT( pParentFrame != NULL );
	if( ! pParentFrame->IsKindOf( RUNTIME_CLASS(CExtMiniDockFrameWnd) ) )
	{
		FloatControlBar( ptFloat );
		pParentFrame = GetParentFrame();
	}
	else
	{
		CExtDynControlBar * pDynBar = _GetNearestDynamicContainer();
		for( ; pDynBar != NULL; pDynBar = pDynBar->_GetNearestDynamicContainer() )
			pFloatBar = pDynBar;
		pFloatBar->FloatControlBar( ptFloat );
	}
	pParentFrame->ShowWindow( SW_SHOWNA );
	pParentFrame->DelayRecalcLayout();
	pParentFrame->SendMessage( WM_IDLEUPDATECMDUI );
HWND hWndOwn = m_hWnd;
	if( pFloatBar != this )
	{
		m_pDockSite->ShowControlBar( pFloatBar, TRUE, TRUE );
		pFloatBar->OnRepositionSingleChild();
		::AfxGetApp()->OnIdle( 0 );
		if( ! ( ::IsWindow(hWndOwn) && CWnd::FromHandlePermanent( hWndOwn ) == this ) )
			return false;
	}
	OnRepositionSingleChild();
CPoint ptOffset( point );
CRect rcWnd;
	pFloatBar->GetWindowRect( &rcWnd );

	ptOffset -= rcWnd.TopLeft();
	if( m_pDockSite->GetSafeHwnd() != NULL && ( ! bShowContentWhenDragging ) )
	{
		ASSERT_VALID( m_pDockSite );
		m_pDockSite->ShowControlBar( this, TRUE, FALSE );
	}
	pFloatBar->_DraggingStart( point, ptOffset );
	return true;
}

#endif // (! defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL) || (! defined __EXT_MFC_NO_TABMDI_CTRL)

bool CExtDynamicControlBar::IsPersistentBar() const
{
	ASSERT_VALID( this );
	return m_bPersistentBar;
}

void CExtDynamicControlBar::OnGetBarCaptionText(
	CExtControlBar::e_bar_caption_text_t eBCT,
	CExtSafeString & strCaptionText
	) const
{
	ASSERT_VALID( this );
	if( ! m_strCaptionText.IsEmpty() )
	{
		strCaptionText = m_strCaptionText;
		return;
	} // if( ! m_strCaptionText.IsEmpty() )
	CExtControlBar::OnGetBarCaptionText(
		eBCT,
		strCaptionText
		);
}

bool CExtDynamicControlBar::NcButtons_HandleClick(
	CExtBarNcAreaButton * pNcAreaButton,
	CPoint point,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
#ifdef _DEBUG
	if( pBarEventSource != NULL )
	{
		ASSERT_VALID( pNcAreaButton );
	}
	if( pBarEventSource != NULL )
	{
		ASSERT_VALID( pBarEventSource );
	}
#endif // _DEBUG
	return
		CExtControlBar::NcButtons_HandleClick(
			pNcAreaButton,
			point,
			pBarEventSource,
			pBarActiveInContainer
			);
}

void CExtDynamicControlBar::NcButtons_PostClick(
	CExtBarNcAreaButton * pNcAreaButton,
	CPoint point,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
#ifdef _DEBUG
	if( pBarEventSource != NULL )
	{
		ASSERT_VALID( pNcAreaButton );
	}
	if( pBarEventSource != NULL )
	{
		ASSERT_VALID( pBarEventSource );
	}
#endif // _DEBUG
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if(		(! IsCloseOnlyOneBarInTabGroup() )
		&&	pNcAreaButton->IsKindOf( RUNTIME_CLASS(CExtBarNcAreaButtonClose) )
		)
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	{
		bool bIsVisible = false;
		CExtDynamicControlBar::eDynamicBarState_t eDBS =
			BarStateGet( &bIsVisible );
		if(		bIsVisible
			&&	(	eDBS == CExtDynamicControlBar::__EDBS_DOCKED
				||	eDBS == CExtDynamicControlBar::__EDBS_FLOATING
				)
			)
			BarStateSet( eDBS, false );
	}// if( pNcAreaButton->IsKindOf( RUNTIME_CLASS(CExtBarNcAreaButtonClose) ) )

	CExtControlBar::NcButtons_PostClick(
		pNcAreaButton,
		point,
		pBarEventSource,
		pBarActiveInContainer
		);
}

CExtDynamicBarSite * CExtDynamicControlBar::GetBarSite()
{
	ASSERT_VALID( this );
	if( m_pDBS == NULL )
		m_pDBS = CExtDynamicBarSite::FindBarSite( this );
	return m_pDBS;
}

void CExtDynamicControlBar::PostNcDestroy()
{
	ASSERT_VALID( this );
	delete this;
}

void CExtDynamicControlBar::OnNcAreaButtonsReinitialize()
{
	ASSERT_VALID( this );
INT nCountOfNcButtons = NcButtons_GetCount();
	if( nCountOfNcButtons > 0 )
		return;
	NcButtons_Add( new CExtDynamicBarNcAreaButtonClose(this) );
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	NcButtons_Add( new CExtDynamicBarNcAreaButtonAutoHide(this) );
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	NcButtons_Add( new CExtDynamicBarNcAreaButtonMenu(this) );
}

LRESULT CExtDynamicControlBar::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_DESTROY:
		{
			CExtDynamicBarSite * pDBS = GetBarSite();
			if( pDBS != NULL ) // shutdown mode issue
				pDBS->_BarFreeImpl( this, true, false );
			return 0;
		}
	break; // case WM_DESTROY
	case WM_COMMAND:
		{
			UINT nCmdID = (UINT)wParam;
			switch( nCmdID )
			{
			case ID_EXT_DYN_BAR_FLOATING:
				BarStateSet( __EDBS_FLOATING, true );
			break;
			case ID_EXT_DYN_BAR_DOCKABLE:
				BarStateSet( __EDBS_DOCKED, true );
			break;
			case ID_EXT_DYN_BAR_TABBED_DOCUMENT:
			case ID_EXT_DYN_BAR_MDI_DOCUMENT:
				BarStateSet( __EDBS_DOCUMENT, true );
			break;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			case ID_EXT_DYN_BAR_AUTO_HIDE:
				BarStateSet( __EDBS_AUTO_HIDDEN, true );
			break;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			case ID_EXT_DYN_BAR_HIDE:
				BarStateSet( BarStateGet(), false );
			break;
			default:
				return CExtControlBar::WindowProc( message, wParam, lParam );                                
			} // switch( nCmdID )
			return 0;
		}
	break; // case WM_COMMAND
	case WM_SHOWWINDOW:
	case WM_SIZE:
	case WM_WINDOWPOSCHANGED:
		{
			if(		m_pDockSite == NULL
				||	(! ProfileBarStateIsSerializing( m_pDockSite ) )
				)
			{
				if( IsVisible() )
				{
					m_dwAdditionalStateFlags &= ~__EASF_INVISIBLE;
				}
				else
				{
					eDynamicBarState_t eDBS = BarStateGet();
					if( eDBS == __EDBS_FLOATING || eDBS == __EDBS_DOCKED )
						m_dwAdditionalStateFlags |= __EASF_INVISIBLE;
				}
			}
		}
	break;
	} // switch( message )
	return CExtControlBar::WindowProc( message, wParam, lParam );
}

bool CExtDynamicControlBar::_CanDockLTRB(
	CExtControlBar * pDestBar
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pDestBar );
eDynamicBarState_t eDBS = BarStateGet();
	if( eDBS != __EDBS_DOCKED )
		return false;
CExtDynamicControlBar * pSameKindOfBar =
		DYNAMIC_DOWNCAST(
			CExtDynamicControlBar,
			pDestBar
			);
	if( pSameKindOfBar != NULL )
	{
		CExtDynamicControlBar::eDynamicBarState_t eDBS =
			pSameKindOfBar->BarStateGet();
		if( eDBS != CExtDynamicControlBar::__EDBS_DOCKED )
			return false;
	} // if( pSameKindOfBar != NULL )
	return CExtControlBar::_CanDockLTRB( pDestBar );
}

bool CExtDynamicControlBar::_CanDockToTabbedContainers(
	CExtControlBar * pDestBar
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( pDestBar );
	return _CanDockLTRB( pDestBar );
}

void CExtDynamicControlBar::_DraggingUpdateState(
	const CPoint & point,
	bool bForceFloatMode
	)
{
	ASSERT_VALID( this );
	CExtControlBar::_DraggingUpdateState(
		point,
		( BarStateGet() == __EDBS_FLOATING )
			? true
			: bForceFloatMode
		);
}

bool CExtDynamicControlBar::OnQueryUseCheckMarkCommand()
{
	ASSERT_VALID( this );
	return false;
}

bool CExtDynamicControlBar::OnInitDbsMenu(
	CExtPopupMenuWnd * pPopup,
	HWND hWndTrack,
	CObject * pHelperSrc,
	LPARAM lParamHelper // = 0
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pPopup );
	ASSERT( hWndTrack != NULL && ::IsWindow( hWndTrack ) );
	ASSERT_VALID( pHelperSrc );
	pHelperSrc;
	lParamHelper;
CExtDynamicBarSite * pDBS = GetBarSite();
	if( pDBS == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
CExtDynamicBarSite::eDetectedUiType_t eDUIT	= pDBS->GetDetectedUiType();
INT nCountInitial = pPopup->ItemGetCount();
	if( nCountInitial == 0 )
	{
		if( ! pPopup->LoadMenu(
				hWndTrack,
				IDR_NC_MENU_DYN_BAR,
				true,
				true
				)
			)
		{
			ASSERT( FALSE );
			return false;
		}
	} // if( nCountInitial == 0 )
	else
	{
		if( ! pPopup->ItemGetInfo( nCountInitial - 1 ).IsSeparator() )
		{
			VERIFY( pPopup->ItemInsert() );
			nCountInitial++;
		} // if( ! pPopup->ItemGetInfo( nCountInitial - 1 ).IsSeparator() )
		CMenu _menu;
		if( ! g_ResourceManager->LoadMenu(
				_menu,
				IDR_NC_MENU_DYN_BAR
				)
			)
		{
			ASSERT( FALSE );
			return false;
		}
		ASSERT( _menu.GetSafeHmenu() != NULL );
		CMenu * pSubMenu = _menu.GetSubMenu( 0 );
		if( pSubMenu == NULL )
		{
			ASSERT( FALSE );
			return false;
		}
		INT nPos;
		INT nCount = (INT)pSubMenu->GetMenuItemCount();
		for( nPos = 0; nPos < nCount; nPos++ )
		{
			CExtSafeString sMenuText;
			MENUITEMINFO _mii;
			::memset( &_mii, 0, sizeof(MENUITEMINFO) );
			_mii.cbSize = sizeof(MENUITEMINFO);
			_mii.fMask = MIIM_CHECKMARKS|MIIM_DATA|MIIM_ID|MIIM_STATE|MIIM_SUBMENU|MIIM_TYPE;
			_mii.cch = __MAX_UI_ITEM_TEXT;
			_mii.dwTypeData = sMenuText.GetBuffer( 1024 );
			ASSERT( _mii.dwTypeData != NULL );
			if( _mii.dwTypeData == NULL )
			{
				ASSERT( FALSE );
				continue;
			}
			if( ! ::GetMenuItemInfo( pSubMenu->GetSafeHmenu(), (UINT)nPos, TRUE, &_mii ) )
			{
				sMenuText.ReleaseBuffer();
				ASSERT( FALSE );
				continue;
			}
			sMenuText.ReleaseBuffer();
			if(		(_mii.fType & MFT_SEPARATOR) != 0
				||	_mii.hSubMenu != NULL
				)
				continue;
			if( ! CExtCmdManager::IsCommand( _mii.wID ) )
				continue;
			sMenuText.Replace( _T("\n"), _T("") );
			sMenuText.Replace( _T("\r"), _T("") );
			sMenuText.TrimLeft();
			sMenuText.TrimRight();
			if( sMenuText.IsEmpty() )
				continue;
			int nSep =
				sMenuText.ReverseFind( _T('\t') );
			if( nSep >= 0 )
			{
				CExtSafeString sTmp;
				sTmp = sMenuText.Left( nSep );
				sTmp.TrimLeft();
				sTmp.TrimRight();
				sMenuText = sTmp;
			} // if( nSep >= 0 )
			if(		eDUIT == CExtDynamicBarSite::__EDUIT_UNSUPPORTED_ENVIRONMENT
				&&	(	_mii.wID == ID_EXT_DYN_BAR_TABBED_DOCUMENT
					||	_mii.wID == ID_EXT_DYN_BAR_MDI_DOCUMENT
					)
				)
				continue;
			VERIFY(
				pPopup->ItemInsertCommand(
					_mii.wID,
					-1,
					LPCTSTR(sMenuText),
					NULL,
					NULL,
					false,
					0,
					hWndTrack
					)
				);
		} // for( nPos = 0; nPos < nCount; nPos++ )
	} // else from if( nCountInitial == 0 )
CExtDynamicControlBar::eDynamicBarState_t eDBS =
		BarStateGet();
INT nPos;
INT nCount = pPopup->ItemGetCount();
	for( nPos = 0; nPos < nCount; nPos ++ )
	{
		CExtPopupMenuWnd::MENUITEMDATA & _mii = pPopup->ItemGetInfo( nPos );
		if( _mii.IsPopup() || _mii.IsSeparator() )
			continue;
		UINT nCmdID = _mii.GetCmdID();
		if( CExtCmdManager::IsSystemCommand( nCmdID ) )
		{
			_mii.SetNoCmdUI( true );
		}
	}
	for( nPos = nCountInitial; nPos < nCount; )
	{
		CExtPopupMenuWnd::MENUITEMDATA & _mii =
			pPopup->ItemGetInfo( nPos );
		if( _mii.IsPopup() || _mii.IsSeparator() )
		{
			nPos++; // fix by Dave Kymlicka
			continue;
		}
		UINT nCmdID = _mii.GetCmdID();
		switch( nCmdID )
		{
		case ID_EXT_DYN_BAR_FLOATING:
			_mii.Check(
					( eDBS == CExtDynamicControlBar::__EDBS_FLOATING )
						? true : false
				);
			_mii.Enable(
					(	(	eDBS == CExtDynamicControlBar::__EDBS_FLOATING
						||	eDBS == CExtDynamicControlBar::__EDBS_DOCKED
						||	eDBS == CExtDynamicControlBar::__EDBS_DOCUMENT
						)
					&&	_FloatStateChangingIsEnabled( this )
					)	? true : false
				);
		break;
		case ID_EXT_DYN_BAR_DOCKABLE:
			_mii.Check(
					( eDBS == CExtDynamicControlBar::__EDBS_DOCKED )
						? true : false
				);
			_mii.Enable(
					(	(	eDBS == CExtDynamicControlBar::__EDBS_FLOATING
						||	eDBS == CExtDynamicControlBar::__EDBS_DOCKED
						||	eDBS == CExtDynamicControlBar::__EDBS_DOCUMENT
						)
					&&	_FloatStateChangingIsEnabled( this )
					)	? true : false
				);
		break;
		case ID_EXT_DYN_BAR_TABBED_DOCUMENT:
		case ID_EXT_DYN_BAR_MDI_DOCUMENT:
			if( eDUIT == CExtDynamicBarSite::__EDUIT_UNSUPPORTED_ENVIRONMENT )
			{
				if(		nCmdID == ID_EXT_DYN_BAR_TABBED_DOCUMENT
					||	nCmdID == ID_EXT_DYN_BAR_MDI_DOCUMENT
					)
				{
					VERIFY( pPopup->ItemRemove( nPos ) );
					nCount--;
					continue;
				}
			}
			if( eDUIT == CExtDynamicBarSite::__EDUIT_MDI_ORDINARY )
			{
				if( nCmdID == ID_EXT_DYN_BAR_TABBED_DOCUMENT )
				{
					VERIFY( pPopup->ItemRemove( nPos ) );
					nCount--;
					continue;
				}
			}
			else if(
					eDUIT == CExtDynamicBarSite::__EDUIT_MDI_WITH_TABS
				||	eDUIT == CExtDynamicBarSite::__EDUIT_SDI_TAB_PAGE_CONTAINER
				)
			{
				if( nCmdID == ID_EXT_DYN_BAR_MDI_DOCUMENT )
				{
					VERIFY( pPopup->ItemRemove( nPos ) );
					nCount--;
					continue;
				}
			}
			_mii.Check(
					( eDBS == CExtDynamicControlBar::__EDBS_DOCUMENT )
						? true : false
				);
			_mii.Enable(
					(	(	eDBS == CExtDynamicControlBar::__EDBS_FLOATING
						||	eDBS == CExtDynamicControlBar::__EDBS_DOCKED
						||	eDBS == CExtDynamicControlBar::__EDBS_DOCUMENT
						)
					&&	_FloatStateChangingIsEnabled( this )
					)	? true : false
				);
		break;
		case ID_EXT_DYN_BAR_AUTO_HIDE:
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			_mii.Check(
					( eDBS == CExtDynamicControlBar::__EDBS_AUTO_HIDDEN )
						? true : false
				);
			_mii.Enable(
					(	(	eDBS == CExtDynamicControlBar::__EDBS_AUTO_HIDDEN
						||	(	eDBS == CExtDynamicControlBar::__EDBS_DOCKED
							&&	GetParentFrame() == m_pDockSite
							)
						)
					&&	_AutoHidingIsEnabled( this )
					)	? true : false
				);
#else
			VERIFY( pPopup->ItemRemove( nPos ) );
			nCount--;
			continue;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		break;
		case ID_EXT_DYN_BAR_HIDE:
			_mii.Check( false );
			_mii.Enable( _ClosingIsEnabled( this ) );
		break;
		default:
			_mii.Check( false );
			_mii.Enable( false );
		break;
		} // switch( nCmdID )
		nPos++;
	} // for( nPos = nCountInitial; nPos < nCount; )

	if( ! _ClosingIsEnabled( this ) )
	{
		INT nPosToRemove = pPopup->ItemFindPosForCmdID( SC_CLOSE );
		for( ; nPosToRemove >= 0; nPosToRemove = pPopup->ItemFindPosForCmdID( SC_CLOSE ) )
		{
			pPopup->ItemRemove( nPosToRemove );
			if(		nPosToRemove > 0
				&&	pPopup->ItemGetInfo( nPosToRemove -1 ).IsSeparator()
				)
				pPopup->ItemRemove( nPosToRemove - 1 );
		} // for( ; nPosToRemove >= 0; nPosToRemove = pPopup->ItemFindPosForCmdID( SC_CLOSE ) )
	} // if( ! _ClosingIsEnabled( this ) )

	return true;
}

void CExtDynamicControlBar::PmBridge_OnPaintManagerChanged(
	CExtPaintManager * pGlobalPM
	)
{
	m_icon.OnEmptyGeneratedBitmaps();
	CExtControlBar::PmBridge_OnPaintManagerChanged( pGlobalPM );
}

HWND CExtDynamicControlBar::OnQueryChildHWND()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
HWND hWnd = ::GetWindow( m_hWnd, GW_CHILD );
	if( m_hWndChildCached == NULL && hWnd != NULL )
		m_hWndChildCached = hWnd;
	return m_hWndChildCached;
}

void CExtDynamicControlBar::OnMoveChildToDocument()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
HWND hWndChild = OnQueryChildHWND();
	if( hWndChild == NULL )
		return;

CExtDynamicBarSite * pDBS = GetBarSite();
	if( pDBS == NULL )
		return;
CExtDynamicBarSite::eDetectedUiType_t eDUIT	= pDBS->GetDetectedUiType();
HWND hWndContainer = ::GetParent( hWndChild );
	ASSERT( hWndContainer != NULL );
	if( hWndContainer != m_hWnd )
	{
		CWnd * pWndPermanent = CWnd::FromHandlePermanent( hWndContainer );
		if( pWndPermanent != NULL )
		{
			CExtDynamicMDIChildWnd * pWndMdiChild =
				DYNAMIC_DOWNCAST(
					CExtDynamicMDIChildWnd,
					pWndPermanent
					);
			if( pWndMdiChild != NULL )
			{
				CMDIFrameWnd * pMDIFrame =
					STATIC_DOWNCAST(
						CMDIFrameWnd,
						pWndMdiChild->GetParentFrame()
						);
				ASSERT_VALID( pMDIFrame );
				CMDIChildWnd * pActive = pMDIFrame->MDIGetActive();
				if( pWndMdiChild != pActive )
					pWndMdiChild->MDIActivate();
			} // if( pWndMdiChild != NULL )
			else
			{
#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
				if( eDUIT == CExtDynamicBarSite::__EDUIT_SDI_TAB_PAGE_CONTAINER )
				{
					CExtTabPageContainerWnd * pWndTabPageContainer =
						DYNAMIC_DOWNCAST(
							CExtTabPageContainerWnd,
							pWndPermanent
							);
					if( pWndTabPageContainer != NULL )
					{
						CExtTabWnd * pWndTabs = pWndTabPageContainer->GetSafeTabWindow();
						ASSERT_VALID( pWndTabs );
						int nPageIdx = pWndTabs->ItemFindByLParam( LPARAM(OnQueryChildHWND()) );
						if( nPageIdx >= 0 )
						{
							CExtTabWnd::TAB_ITEM_INFO * pTII = pWndTabs->ItemGet( nPageIdx );
							ASSERT_VALID( pTII );
							pTII->EventProviderSet( this );
							if( pDBS->m_listBarsInDocMode.Find( this ) == NULL )
								pDBS->m_listBarsInDocMode.AddHead( this );
							return;
						} // if( nPageIdx >= 0 )
					} // if( pWndTabPageContainer != NULL )
				} // if( eDUIT == CExtDynamicBarSite::__EDUIT_SDI_TAB_PAGE_CONTAINER )
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
			} // else from if( pWndMdiChild != NULL )
		} // if( pWndPermanent != NULL )
		return;
	} // if( hWndContainer != m_hWnd )
	if( eDUIT == CExtDynamicBarSite::__EDUIT_UNSUPPORTED_ENVIRONMENT )
		return;

CExtSafeString sDocumentCaptionText;
	OnGetBarCaptionText(
		__EBCT_DOCUMENT_CONTAINER_CAPTION,
		sDocumentCaptionText
		);

#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
	if( eDUIT == CExtDynamicBarSite::__EDUIT_SDI_TAB_PAGE_CONTAINER )
	{
		CExtTabPageContainerWnd * pWndTabPageContainer =
			pDBS->GetTabPageContainer();
		ASSERT_VALID( pWndTabPageContainer );
		ASSERT( pWndTabPageContainer->GetDlgCtrlID() == AFX_IDW_PANE_FIRST );
		::ShowWindow( hWndChild, SW_HIDE );
		::SetParent( hWndChild, pWndTabPageContainer->m_hWnd );
		CRect rcClient;
		pWndTabPageContainer->GetClientRect( &rcClient );
		CWnd::RepositionBars(
			0,
			0x0FFFF,
			0x101,
			CWnd::reposQuery,
			&rcClient,
			&rcClient,
			TRUE
			);
		::SetWindowPos(
			hWndChild,
			HWND_BOTTOM,
			rcClient.left,
			rcClient.top,
			rcClient.Width(),
			rcClient.Height(),
			SWP_NOACTIVATE|SWP_NOCOPYBITS
			);
		int nPageIdx = m_bInsertToHeadOfTPC ? 0 : pWndTabPageContainer->PageGetCount();
#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_ONENOTE_CTRL)
		CExtTabPageContainerOneNoteWnd * pOneNoteTPC =
			DYNAMIC_DOWNCAST(
				CExtTabPageContainerOneNoteWnd,
				pWndTabPageContainer
				);
		if( pOneNoteTPC != NULL )
		{
			VERIFY(
				pOneNoteTPC->PageInsert(
					CWnd::FromHandle( hWndChild ),
					m_icon,
					sDocumentCaptionText.IsEmpty() ? _T("") : LPCTSTR(sDocumentCaptionText),
					nPageIdx,
					true
					)
				);
		} // if( pOneNoteTPC != NULL )
		else
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_ONENOTE_CTRL)
		{
			VERIFY(
				pWndTabPageContainer->PageInsert(
					CWnd::FromHandle( hWndChild ),
					m_icon,
					sDocumentCaptionText.IsEmpty() ? _T("") : LPCTSTR(sDocumentCaptionText),
					nPageIdx,
					true
					)
				);
		} // else from if( pOneNoteTPC != NULL )
		if( pWndTabPageContainer->ShowBtnCloseGet() )
			pWndTabPageContainer->EnabledBtnCloseSet( true );
		CExtTabWnd * pWndTabs = pWndTabPageContainer->GetSafeTabWindow();
		ASSERT_VALID( pWndTabs );
		CExtTabWnd::TAB_ITEM_INFO * pTII = pWndTabs->ItemGet( nPageIdx );
		ASSERT_VALID( pTII );
		ASSERT( ((HWND)pTII->LParamGet()) != NULL );
		ASSERT( ((HWND)pTII->LParamGet()) == OnQueryChildHWND() );
		pTII->EventProviderSet( this );
		pDBS->m_listBarsInDocMode.AddHead( this );
		return;
	} // if( eDUIT == CExtDynamicBarSite::__EDUIT_SDI_TAB_PAGE_CONTAINER )
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)

CMDIFrameWnd * pMdiFrame =
		STATIC_DOWNCAST(
			CMDIFrameWnd,
			m_pDockSite
			);
	ASSERT_VALID( pMdiFrame );
bool bMaximized = false;
HWND hWndActiveMdiChildFrame =
		pDBS->GetActiveMdiChildFrameHWND( &bMaximized );
	if( hWndActiveMdiChildFrame == NULL )
		bMaximized = true;

DWORD dwMdiChildFrameWindowStyles =
		( (WS_OVERLAPPEDWINDOW) & (~WS_POPUP) )
			|WS_CHILD
			|WS_VISIBLE
			|FWS_ADDTOTITLE
			;
	if( bMaximized )
		dwMdiChildFrameWindowStyles |= WS_MAXIMIZE;
CExtDynamicMDIChildWnd * pWndMdiChild =
		OnCreateDynamicMDIChildWndInstance();
	if( pWndMdiChild == NULL )
	{
		ASSERT( FALSE );
		return;
	}
	ASSERT_VALID( pWndMdiChild );
	if( ! m_icon.IsEmpty() )
		pWndMdiChild->m_icon = m_icon;
	if( ! pWndMdiChild->Create(
			::AfxRegisterWndClass(
				CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS
				),
			sDocumentCaptionText.IsEmpty()
				? _T("")
				: LPCTSTR(sDocumentCaptionText),
			dwMdiChildFrameWindowStyles,
			CFrameWnd::rectDefault,
			STATIC_DOWNCAST( CMDIFrameWnd, m_pDockSite )
			)
		)
	{
		ASSERT( FALSE );
		return;
	}
	pWndMdiChild->SetTitle(
		sDocumentCaptionText.IsEmpty() ? _T("") : LPCTSTR(sDocumentCaptionText)
		);
	pWndMdiChild->DelayUpdateFrameTitle();
HMENU hMenuDefault = NULL;
	if( m_nMdiMenuResourceID != 0 )
	{
		CMenu _menu;
		if( g_ResourceManager->LoadMenu( _menu, m_nMdiMenuResourceID ) )
		{
			ASSERT( _menu.GetSafeHmenu() != NULL );
			hMenuDefault = _menu.Detach();
		} // if( g_ResourceManager->LoadMenu( _menu, m_nMdiMenuResourceID ) )
	} // if( m_nMdiMenuResourceID != 0 )
	if( hMenuDefault == NULL )
		hMenuDefault = pMdiFrame->m_hMenuDefault;
HACCEL hAccelTable = pMdiFrame->m_hAccelTable;
	if( m_nMdiAccelTableResourceID != 0 )
		hAccelTable =
			g_ResourceManager->LoadAccelerators(
				m_nMdiMenuResourceID
				);
	if( hAccelTable == NULL )
		hAccelTable = pMdiFrame->m_hAccelTable;
	pWndMdiChild->SetHandles(
		hMenuDefault,
		hAccelTable
		);
	pWndMdiChild->MDIActivate();
	if( hMenuDefault != NULL )
		pMdiFrame->OnUpdateFrameMenu( hMenuDefault );
	pDBS->m_listBarsInDocMode.AddHead( this );
}

CExtDynamicMDIChildWnd * CExtDynamicControlBar::OnCreateDynamicMDIChildWndInstance()
{
	ASSERT_VALID( this );
CExtDynamicBarSite * pDBS = GetBarSite();
	if( pDBS == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
CExtDynamicMDIChildWnd * pWndMdiChild =
		pDBS->OnDbsCreateDynamicMDIChildWndInstance( this );
	return pWndMdiChild;
}

void CExtDynamicControlBar::OnMoveChildToBar()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
HWND hWndChild = OnQueryChildHWND();
	if( hWndChild == NULL )
		return;
HWND hWndContainer = ::GetParent( hWndChild );
	ASSERT( hWndContainer != NULL );
	if( hWndContainer == m_hWnd )
		return;
CExtDynamicBarSite * pDBS = GetBarSite();
	if( pDBS == NULL )
		return;
CExtDynamicBarSite::eDetectedUiType_t eDUIT	= pDBS->GetDetectedUiType();
	if( eDUIT == CExtDynamicBarSite::__EDUIT_UNSUPPORTED_ENVIRONMENT )
		return;
POSITION posInList = pDBS->m_listBarsInDocMode.Find( this );
	if( posInList != NULL )
		pDBS->m_listBarsInDocMode.RemoveAt( posInList );

#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
	if( eDUIT == CExtDynamicBarSite::__EDUIT_SDI_TAB_PAGE_CONTAINER )
	{
		CExtTabPageContainerWnd * pWndTabPageContainer =
			pDBS->GetTabPageContainer();
		ASSERT_VALID( pWndTabPageContainer );
		ASSERT( pWndTabPageContainer->GetDlgCtrlID() == AFX_IDW_PANE_FIRST );
		int nSelPageIdx = pWndTabPageContainer->PageSelectionGet();
		if( nSelPageIdx < 0 )
			return;
		HWND hWndChild = OnQueryChildHWND();
		if( hWndChild != NULL && ::IsWindow( hWndChild ) )
		{
			HWND hWndChildInTPC = pWndTabPageContainer->PageHwndGetSafe( nSelPageIdx );
			ASSERT( hWndChildInTPC != NULL && ::IsWindow( hWndChildInTPC ) );
			if(  hWndChild == hWndChildInTPC )
			{
				::ShowWindow( hWndChild, SW_HIDE );
				::SetParent( hWndChild, m_hWnd );
				pWndTabPageContainer->PageRemove( nSelPageIdx, 1, false );
				CRect rcClient;
				GetClientRect( &rcClient );
				::MoveWindow(
					hWndChild,
					rcClient.left,
					rcClient.top,
					rcClient.Width(),
					rcClient.Height(),
					TRUE
					);
				GetParentFrame()->DelayRecalcLayout();
				::ShowWindow( hWndChild, SW_SHOW );
			} // if(  hWndChild == hWndChildInTPC )
			if( pWndTabPageContainer->ShowBtnCloseGet() )
				pWndTabPageContainer->EnabledBtnCloseSet(
					( pWndTabPageContainer->PageGetCount() > 0 )
						? true : false
					);
			pWndTabPageContainer->Invalidate();
		} // if( hWndChild != NULL && ::IsWindow( hWndChild ) )
		CExtMiniDockFrameWnd * pMiniFrame = DYNAMIC_DOWNCAST( CExtMiniDockFrameWnd, GetParentFrame() );
		if( pMiniFrame != NULL )
		{
			bool bFloatingVisible = false;
			if(		(	m_eHelperSwitchingDBS == CExtDynamicControlBar::__EDBS_FLOATING
					||	m_eHelperSwitchingDBS == CExtDynamicControlBar::__EDBS_DOCKED
					)
				&&	m_bHelperSwitchingIntoVisibleState
				&&	BarStateGet() != CExtDynamicControlBar::__EDBS_DOCUMENT
				)
				bFloatingVisible = true;
			pMiniFrame->ModifyStyle( FWS_SNAPTOBARS, 0 );
			CSize _sizeF = GetSizeFloating();
			CRect rcWndBar( 0, 0, _sizeF.cx, _sizeF.cy );
			CRect rcClientFrame, rcWndFrame, rcFrameNC(0,0,0,0);
			pMiniFrame->GetWindowRect( &rcWndFrame );
			if(	! pMiniFrame->PmBridge_GetPM()->Bar_MiniDockFrameNcCalcSize(
					rcFrameNC,
					pMiniFrame,
					this
					)
				)
			{
				rcFrameNC.left = rcFrameNC.right = ::GetSystemMetrics( SM_CXSIZEFRAME );
				rcFrameNC.top = rcFrameNC.bottom = ::GetSystemMetrics( SM_CYSIZEFRAME );
				rcFrameNC.top += ::GetSystemMetrics( SM_CYSMCAPTION );
			}
			rcWndFrame.right = rcWndFrame.left + rcWndBar.Width(); // + rcFrameNC.left + rcFrameNC.right;
			rcWndFrame.bottom = rcWndFrame.top + rcWndBar.Height(); // + rcFrameNC.top + rcFrameNC.bottom;
			rcWndBar.right -= rcFrameNC.left + rcFrameNC.right;
			rcWndBar.bottom -= rcFrameNC.top + rcFrameNC.bottom;
			rcClientFrame = rcWndFrame;
			rcClientFrame.DeflateRect( 0, 0, 4, 4 );
			rcClientFrame.OffsetRect( -rcClientFrame.TopLeft() );
			bool bSingleBarFliatingMode = false;
			CWnd * pWnd = pMiniFrame->GetDlgItem( AFX_IDW_DOCKBAR_FLOAT );
			if( pWnd != NULL )
			{
				pWnd = pWnd->GetWindow( GW_CHILD );
				if( LPVOID(pWnd) == LPVOID(this) )
					bSingleBarFliatingMode = true;
			}
			if( bSingleBarFliatingMode )
			{
				CRect rcMove = rcWndFrame;
				rcMove.right += rcFrameNC.left + rcFrameNC.right - 4;
				rcMove.bottom += rcFrameNC.top + rcFrameNC.bottom - 4;
				pMiniFrame->MoveWindow( &rcMove );
			}
			pMiniFrame->DelayRecalcLayout();
			if( bFloatingVisible )
				DelayShow( TRUE );
			if( bFloatingVisible )
			{
				pMiniFrame->ShowWindow( SW_SHOWNOACTIVATE );
				pMiniFrame->RecalcLayout();
			}
			OnRepositionSingleChild();
			OnNcAreaButtonsReposition();
		} // if( pMiniFrame != NULL )
// 		else
// 		{
// 			CRect rcClient;
// 			m_pBar->GetClientRect( &rcClient );
// 			::MoveWindow(
// 				hWndChild,
// 				rcClient.left,
// 				rcClient.top,
// 				rcClient.Width(),
// 				rcClient.Height(),
// 				TRUE
// 				);
// 		} // else from if( pMiniFrame != NULL )
		return;
	} // if( eDUIT == CExtDynamicBarSite::__EDUIT_SDI_TAB_PAGE_CONTAINER )
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)

CWnd * pWndPermanent =
		CWnd::FromHandlePermanent(hWndContainer);
	if( pWndPermanent != NULL )
	{
		CExtDynamicMDIChildWnd * pWndMdiChild =
			DYNAMIC_DOWNCAST(
				CExtDynamicMDIChildWnd,
				pWndPermanent
				);
		if( pWndMdiChild != NULL )
		{
			ASSERT_VALID( pWndMdiChild );
			pWndMdiChild->DestroyWindow();
		} // if( pWndMdiChild != NULL )
	} // if( pWndPermanent != NULL )
}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

bool CExtDynamicControlBar::AutoHideModeSet(
	bool bAutoHideMode,
	bool bAnimate,
	bool bRecalcLayout,
	bool bForceNoShow
	)
{
	ASSERT_VALID( this );
bool bRetVal =
		CExtControlBar::AutoHideModeSet(
			bAutoHideMode,
			bAnimate,
			bRecalcLayout,
			bForceNoShow
			);
//	if(		m_eDBS == __EDBS_AUTO_HIDDEN
//		&&	( ! bAutoHideMode )
//		)
//		m_eDBS = __EDBS_DOCKED;
	return bRetVal;
}

bool CExtDynamicControlBar::OnQueryVisibilityInAutoHideTabs() const
{
	return OnQueryVisibilityInContainerTabs();
}

bool CExtDynamicControlBar::OnQueryVisibilityInContainerTabs() const
{
	ASSERT_VALID( this );
	if( BarStateGet() == __EDBS_DOCUMENT )
		return false;
	return CExtControlBar::OnQueryVisibilityInAutoHideTabs();
}

#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

void CExtDynamicControlBar::OnSerializeDynamicProps( CArchive & ar )
{
	ASSERT_VALID( this );
DWORD dwTmp = 0;
	if( ar.IsStoring() )
	{
		dwTmp = (DWORD)m_eDBS;
		ar << dwTmp;
		ar << m_dwAdditionalStateFlags;
		ar << DWORD(m_nMdiMenuResourceID);
		ar << DWORD(m_nMdiAccelTableResourceID);
	} // if( ar.IsStoring() )
	else
	{
		ar >> dwTmp;
		m_eDBS = (eDynamicBarState_t)dwTmp;
		ar >> m_dwAdditionalStateFlags;
		ar >> dwTmp;
		m_nMdiMenuResourceID = UINT(dwTmp);
		ar >> dwTmp;
		m_nMdiAccelTableResourceID = UINT(dwTmp);
		m_bAppearInDockSiteControlBarPopupMenu =
			( m_eDBS == __EDBS_DOCUMENT ) ? false : true;
		if( ( m_dwAdditionalStateFlags & __EASF_INVISIBLE ) != 0 )
			DelayShow( FALSE );
		if( m_eDBS == __EDBS_DOCUMENT )
			OnMoveChildToDocument();
		else
			OnMoveChildToBar();
	} // else from if( ar.IsStoring() )
}

void CExtDynamicControlBar::OnExtBarStateSerializationComplete(
	bool bStore
	)
{
	ASSERT_VALID( this );
	CExtControlBar::OnExtBarStateSerializationComplete(
		bStore
		);
	if( bStore )
		return;
	OnQueryChildHWND();
bool bIsVisibleState;
	if( BarStateGet( &bIsVisibleState ) == __EDBS_DOCUMENT )
	{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		ASSERT( ! AutoHideModeGet() );
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( bIsVisibleState )
			OnMoveChildToDocument();
	} // if( BarStateGet( &bIsVisibleState ) == __EDBS_DOCUMENT )
}

bool CExtDynamicControlBar::OnInsertDynamicBarCommandIntoPopupMenu(
	CExtPopupMenuWnd * pPopup,
	INT nInsertPos
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pPopup );
	ASSERT( 0 <= nInsertPos && nInsertPos <= pPopup->ItemGetCount() );
UINT nCmdID = UINT( GetDlgCtrlID() );
	ASSERT(
		g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd( m_hWnd ),
			nCmdID
			) != 0
		);
	if(	! pPopup->ItemInsert(
			nCmdID,
			nInsertPos
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	return true;
}

CExtDynamicControlBar::eDynamicBarState_t
	CExtDynamicControlBar::BarStateGet(
		bool * p_bIsVisibleState // = NULL
		) const
{
	ASSERT_VALID( this );
	if( p_bIsVisibleState != NULL )
	{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( m_eDBS == __EDBS_AUTO_HIDDEN )
		{
			(*p_bIsVisibleState) = false;
			if(		m_hWnd != NULL
				&&	::IsWindow( m_hWnd )
				&&	::GetWindow( m_hWnd, GW_CHILD ) == NULL
				&&	AutoHideModeGet()
				)
			{
				ASSERT_KINDOF( CExtDockBar, m_pDockBar );
				CExtDynAutoHideArea * pWndAutoHideArea =
					((CExtDockBar*)m_pDockBar)->_GetAutoHideArea();
				ASSERT_VALID( pWndAutoHideArea );
				CExtDynAutoHideSlider * pWndSlider =
					pWndAutoHideArea->GetAutoHideSlider();
				if(		LPCVOID(pWndSlider->GetBar()) == LPCVOID(this)
					&&	(pWndSlider->GetStyle()&WS_VISIBLE) != 0
					)
					(*p_bIsVisibleState) = true;
			}
		} // if( m_eDBS == __EDBS_AUTO_HIDDEN )
		else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			(*p_bIsVisibleState) =
				( ( m_dwAdditionalStateFlags & __EASF_INVISIBLE ) != 0 )
					? false : true;
	}
	return m_eDBS;
}

bool CExtDynamicControlBar::BarStateSet(
	CExtDynamicControlBar::eDynamicBarState_t eDBS,
	bool bShow
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pDockSite );
//	if( m_eDBS == eDBS )
//		return true;
	m_eHelperSwitchingDBS = eDBS;
	m_bHelperSwitchingIntoVisibleState = bShow;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( eDBS == __EDBS_AUTO_HIDDEN && (!bShow) )
		eDBS = __EDBS_DOCKED;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
CExtDynamicBarSite * pDBS = GetBarSite();
	if( pDBS == NULL )
		return false;
CExtDynamicBarSite::eDetectedUiType_t eDUIT	= pDBS->GetDetectedUiType();
	switch( eDBS )
	{
	case __EDBS_FLOATING:
	{
		m_bAppearInDockSiteControlBarPopupMenu = true;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( AutoHideModeGet() )
			AutoHideModeSet( false, false, false, true );
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		bool bFloating = IsFloating();
		CExtMiniDockFrameWnd * pOldMiniDockFrameWndToHide = NULL;
		HWND hWndOldMiniDockFrameWndToHide = NULL;
		bool bHideOldFrame = false;
		CFrameWnd * pParentFrame = GetParentFrame();
		ASSERT( pParentFrame != NULL );
		if( ! bFloating )
		{
			pOldMiniDockFrameWndToHide = DYNAMIC_DOWNCAST( CExtMiniDockFrameWnd, pParentFrame );
			if( pOldMiniDockFrameWndToHide != NULL )
			{
				hWndOldMiniDockFrameWndToHide = pOldMiniDockFrameWndToHide->GetSafeHwnd();
				if( pOldMiniDockFrameWndToHide->_GetSingleVisibleBar() != this )
					pOldMiniDockFrameWndToHide = NULL;
				else
					bHideOldFrame = true;
			}
		}
		else
		{
			OnMoveChildToBar();
			DelayShow( bShow ? TRUE : FALSE );
//  		if( ! bShow )
//  			m_pDockSite->ShowControlBar( this, FALSE, FALSE );
//  		else
//  			m_pDockSite->ShowControlBar( this, TRUE, FALSE );
			if(		(! bShow ) && IsVisible()
				||	bShow && (! IsVisible() )
				)
			{
				OnFrameBarCheckCmd( true ); // show/hide me
			}
			GetParentFrame()->RecalcLayout();
			break;
		}
		OnMoveChildToBar();
		if( ! bFloating )
			FloatControlBar();
		CControlBar::DelayShow( bShow ? TRUE : FALSE );
		if(		hWndOldMiniDockFrameWndToHide != NULL
			&&	pOldMiniDockFrameWndToHide == NULL
			&&	::IsWindow( hWndOldMiniDockFrameWndToHide )
			)
		{
			bHideOldFrame = false;
			pOldMiniDockFrameWndToHide = DYNAMIC_DOWNCAST( CExtMiniDockFrameWnd, CWnd::FromHandlePermanent( ( hWndOldMiniDockFrameWndToHide ) ) );
		}
		if( pOldMiniDockFrameWndToHide != NULL )
		{
			if( bHideOldFrame )
				pOldMiniDockFrameWndToHide->ShowWindow( SW_HIDE );
			CWnd * pWnd = pOldMiniDockFrameWndToHide->GetDlgItem( AFX_IDW_DOCKBAR_FLOAT );
			if( pWnd != NULL )
			{
				pOldMiniDockFrameWndToHide->RepositionBars( 0, 0xFFFF, AFX_IDW_DOCKBAR_FLOAT );
				pWnd = pWnd->GetWindow( GW_CHILD );
				if( pWnd != NULL )
				{
					CExtDynControlBar * pDynControlBar = DYNAMIC_DOWNCAST( CExtDynControlBar, pWnd );
					if( pDynControlBar != NULL )
					{
						if( pDynControlBar->m_pWndDynDocker != NULL )
							pDynControlBar->m_pWndDynDocker->OnDynamicLayoutOptimize();
					}
				}
			}
			pOldMiniDockFrameWndToHide->RepositionBars( 0, 0xFFFF, AFX_IDW_DOCKBAR_FLOAT );
//			pOldMiniDockFrameWndToHide->RecalcLayout();
		}
//  		if( ! bShow )
//  			m_pDockSite->ShowControlBar( this, FALSE, FALSE );
//  		else
//  			m_pDockSite->ShowControlBar( this, TRUE, FALSE );
			if(		(! bShow ) && IsVisible()
				||	bShow && (! IsVisible() )
				)
			{
				OnFrameBarCheckCmd( true ); // show/hide me
			}
		CFrameWnd * pWndParentFrame = GetParentFrame();
		ASSERT_VALID( pWndParentFrame );
		ASSERT_KINDOF( CExtMiniDockFrameWnd, pWndParentFrame );
		if( bShow && ( pWndParentFrame->GetStyle() & WS_VISIBLE ) == 0 )
			pWndParentFrame->ShowWindow( SW_SHOWNA );
		else if( (!bShow) && ( pWndParentFrame->GetStyle() & WS_VISIBLE ) != 0 )
			pWndParentFrame->ShowWindow( SW_HIDE );
		if( bShow )
			pWndParentFrame->RecalcLayout();
	}
	break;
	case __EDBS_DOCKED:
	{
		if( IsFloating() )
		{
			m_bAppearInDockSiteControlBarPopupMenu = true;
			OnMoveChildToBar();
			DelayShow( bShow ? TRUE : FALSE );
//  		if( ! bShow )
//  			m_pDockSite->ShowControlBar( this, FALSE, FALSE );
//  		else
//  			m_pDockSite->ShowControlBar( this, TRUE, FALSE );
			if(		(! bShow ) && IsVisible()
				||	bShow && (! IsVisible() )
				)
			{
				OnFrameBarCheckCmd( true ); // show/hide me
			}
			GetParentFrame()->RecalcLayout();
			break;
		}
		CRect rcFrameWnd( 0, 0, 0, 0 );
		CFrameWnd * pWndFrame = GetParentFrame();
		CExtMiniDockFrameWnd * pWndMiniFrame = NULL;
		CExtDynControlBar * pDynControlBar = NULL;
		if( pWndFrame != NULL )
		{
			pWndFrame->GetWindowRect( &rcFrameWnd );
			pWndMiniFrame = DYNAMIC_DOWNCAST( CExtMiniDockFrameWnd, pWndFrame );
			if( pWndMiniFrame != NULL )
			{
				pWndMiniFrame->ModifyStyle( FWS_SNAPTOBARS, 0 );
				CWnd * pWnd = pWndMiniFrame->GetDlgItem( AFX_IDW_DOCKBAR_FLOAT );
				if( pWnd != NULL )
				{
					pWnd = pWnd->GetWindow( GW_CHILD );
					if( pWnd != NULL )
						pDynControlBar = DYNAMIC_DOWNCAST( CExtDynControlBar, pWnd );
				}
			}
		}
		m_bAppearInDockSiteControlBarPopupMenu = true;
		OnMoveChildToBar();
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( AutoHideModeGet() )
		{
			AutoHideModeSet( false, false, true, true );
			if( bShow )
				OnFrameBarCheckCmd( false ); // show me
		} // if( AutoHideModeGet() )
		else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		{
			if( bShow )
				m_eDBS = eDBS; // for making OnQueryVisibilityInContainerTabs() returning true
			if(		(! bShow ) && IsVisible()
				||	bShow && (! IsVisible() )
				)
				OnFrameBarCheckCmd( true ); // show/hide me
			if( pWndMiniFrame != NULL )
			{
				CWnd * pWnd = pWndMiniFrame->GetDlgItem( AFX_IDW_DOCKBAR_FLOAT );
				if( pWnd != NULL )
				{
					pWndFrame->MoveWindow( &rcFrameWnd );
					pWndMiniFrame->RepositionBars( 0, 0xFFFF, AFX_IDW_DOCKBAR_FLOAT );
					if( pDynControlBar != NULL )
						pDynControlBar->OnRepositionSingleChild();
				}
				pWndFrame->RecalcLayout();
			}
		} // else from if( AutoHideModeGet() )
	}
	break;
	case __EDBS_DOCUMENT:
		if( OnQueryChildHWND() == NULL )
		{
			ASSERT( FALSE );
			return false;
		} // if( OnQueryChildHWND() == NULL )
		if( eDUIT == CExtDynamicBarSite::__EDUIT_UNSUPPORTED_ENVIRONMENT )
		{
			ASSERT( FALSE );
			return false;
		} // if( OnQueryChildHWND() == NULL )
		m_bAppearInDockSiteControlBarPopupMenu = false;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( AutoHideModeGet() )
			AutoHideModeSet( false, false, false, true );
		else
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			if( IsVisible() )
			{
				OnFrameBarCheckCmd( true ); // hide me
				GetParentFrame()->RecalcLayout();
			}
		if( bShow )
			OnMoveChildToDocument();
		else
			OnMoveChildToBar();
	break;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	case __EDBS_AUTO_HIDDEN:
		m_bAppearInDockSiteControlBarPopupMenu = true;
		ASSERT( bShow );
		if( ! AutoHideModeGet() )
		{
			OnMoveChildToBar();
			if(		IsFloating()
				||	GetParentFrame() != m_pDockSite
				)
			{
				VERIFY(
					DockControlBarInnerOuter(
						AFX_IDW_DOCKBAR_LEFT,
						false,
						m_pDockSite,
						false
						)
					);
			}
			VERIFY(
				AutoHideModeSet(
					true,
					false,
					true,
					false
					)
				);
		} // if( ! AutoHideModeGet() )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	break;
#ifdef _DEBUG
	default:
		{
			ASSERT( FALSE );
		}
	break;
#endif // _DEBUG
	} //switch( eDBS )
	m_eDBS = eDBS;
	if( bShow )
		m_dwAdditionalStateFlags &= ~__EASF_INVISIBLE;
	else
		m_dwAdditionalStateFlags |= __EASF_INVISIBLE;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
//	if( eDBS != __EDBS_DOCUMENT && eDBS != __EDBS_FLOATING )
	if( eDBS == __EDBS_AUTO_HIDDEN )
	{
		CExtDockDynTabBar * pDynDockBar =
			DYNAMIC_DOWNCAST(
				CExtDockDynTabBar,
				GetParent()
				);
		if( pDynDockBar != NULL )
		{
			CExtDynTabControlBar * pTabbedBar =
				DYNAMIC_DOWNCAST(
					CExtDynTabControlBar,
					pDynDockBar->GetParent()
					);
			LONG nPos, nCount = pTabbedBar->GetSwitcherItemCount();
			for( nPos = 0; nPos < nCount; nPos++ )
			{
				CExtControlBar * pContainedBar = pTabbedBar->GetBarAt( nPos, true );
				CExtDynamicControlBar * pDynamicBar = DYNAMIC_DOWNCAST( CExtDynamicControlBar, pContainedBar );
				if( pDynamicBar != NULL )
					pDynamicBar->m_eDBS = eDBS;
			} // for( nPos = 0; nPos < nCount; nPos++ )
		} // if( pDynDockBar != NULL )
	}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CExtDynamicMDIChildWnd

IMPLEMENT_DYNAMIC( CExtDynamicMDIChildWnd, CMDIChildWnd );

CExtDynamicMDIChildWnd::CExtDynamicMDIChildWnd(
	CExtDynamicControlBar * pBar
	)
	: m_pDBS( NULL )
	, m_pBar( NULL )
	, m_hWndHelperBar( NULL )
	, m_hIconExtracted( NULL )
{
	if( pBar != NULL )
		AttachBar( pBar );
}

CExtDynamicMDIChildWnd::~CExtDynamicMDIChildWnd()
{
	if( m_hMenuShared != NULL )
	{
		::DestroyMenu( m_hMenuShared );
		m_hMenuShared = NULL;
	}
	if( m_hAccelTable != NULL )
	{
		::DestroyAcceleratorTable( m_hAccelTable );
		m_hAccelTable = NULL;
	}
	if( m_hIconExtracted != NULL )
	{
		::DestroyIcon( m_hIconExtracted );
		m_hIconExtracted = NULL;
	}
}

void CExtDynamicMDIChildWnd::AttachBar( CExtDynamicControlBar * pBar )
{
	ASSERT_VALID( this );
	m_pBar = pBar;
	ASSERT_VALID( m_pBar );
	m_hWndHelperBar = m_pBar->GetSafeHwnd();
	ASSERT( m_hWndHelperBar != NULL );
	ASSERT( ::GetWindow( m_hWndHelperBar, GW_CHILD ) != NULL );
}

void CExtDynamicMDIChildWnd::PostNcDestroy()
{
	delete this;
}

CExtDynamicControlBar * CExtDynamicMDIChildWnd::GetBar()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT_VALID( m_pBar );
	ASSERT( m_pBar->GetSafeHwnd() == m_hWndHelperBar );
	return m_pBar;
}

CExtDynamicBarSite * CExtDynamicMDIChildWnd::GetBarSite()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT_VALID( m_pBar );
	ASSERT( m_pBar->GetSafeHwnd() == m_hWndHelperBar );
	if( m_pDBS == NULL )
		m_pDBS = m_pBar->GetBarSite();
	return m_pDBS;
}

//BOOL CExtDynamicMDIChildWnd::PreTranslateMessage(MSG* pMsg)
//{
//	ASSERT_VALID( this );
//	if( m_pBar != NULL )
//	{
//		ASSERT_VALID( m_pBar );
//		if( m_pBar->PreTranslateMessage( pMSG ) )
//			return TRUE;
//	} // if( m_pBar != NULL )
//	return CMDIChildWnd::PreTranslateMessage( pMSG );
//}

BOOL CExtDynamicMDIChildWnd::OnCmdMsg(
	UINT nID,
	int nCode,
	void * pExtra,
	AFX_CMDHANDLERINFO * pHandlerInfo
	)
{
	ASSERT_VALID( this );
	if( m_pBar != NULL )
	{
		ASSERT_VALID( m_pBar );
		if(	m_pBar->OnCmdMsg(
				nID,
				nCode,
				pExtra,
				pHandlerInfo
				)
			)
			return TRUE;
	} // if( m_pBar != NULL )
	return
		CMDIChildWnd::OnCmdMsg(
			nID,
			nCode,
			pExtra,
			pHandlerInfo
			);
}

BOOL CExtDynamicMDIChildWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	ASSERT_VALID( this );
	if( ! CMDIChildWnd::PreCreateWindow(cs) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	cs.dwExStyle &= ~(WS_EX_CLIENTEDGE);
	return TRUE;
}

LRESULT CExtDynamicMDIChildWnd::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == WM_SETFOCUS )
	{
		ASSERT( m_hWndHelperBar != NULL );
		if( ::IsWindow(m_hWndHelperBar) )
		{
			ASSERT_VALID( m_pBar );
			ASSERT( m_pBar->GetSafeHwnd() == m_hWndHelperBar );
			HWND hWndChild = m_pBar->OnQueryChildHWND();
			if(		hWndChild != NULL
				&&	::GetParent(hWndChild) == m_hWnd
				)
			{
				if( CExtControlBar::stat_QueryFocusChangingEnabled( m_pBar, hWndChild ) )
					::SetFocus( hWndChild );
			}
		} // if( ::IsWindow(m_hWndHelperBar) )
		return 0;
	} // else if( message == WM_SETFOCUS )
	if(		(message == WM_SYSCOMMAND && wParam == SC_CLOSE)
		||	message == WM_CLOSE
		)
	{ // closing messages
		ASSERT( m_hWndHelperBar != NULL );
		if( ::IsWindow(m_hWndHelperBar) )
		{
			ASSERT_VALID( m_pBar );
			ASSERT( m_pBar->GetSafeHwnd() == m_hWndHelperBar );
			m_pBar->BarStateSet(
				CExtDynamicControlBar::__EDBS_DOCUMENT,
				false
				);
		} // if( ::IsWindow(m_hWndHelperBar) )
		return 0;
	} // closing messages
// 	if( message == WM_NCPAINT )
// 	{
// 		// WM_NCPAINT and WM_WINDOWPOSCHANGED messages are
// 		// handled to fix WinXP MDI interface flicker bug
// 		CMDIFrameWnd * pMdiFrame =
// 			STATIC_DOWNCAST( CMDIFrameWnd, GetParentFrame() );
// 		ASSERT_VALID( pMdiFrame );
// 		BOOL bMax = FALSE;
// 		CMDIChildWnd * pActive = pMdiFrame->MDIGetActive( &bMax );
// 		pActive;
// 		if( bMax )
// 			return 0 ;
// 	} // if( message == WM_NCPAINT )
// 	else if( message == WM_WINDOWPOSCHANGED )
// 	{
// 		// WM_NCPAINT and WM_WINDOWPOSCHANGED messages are
// 		// handled to fix WinXP MDI interface flicker bug
// 		CMDIFrameWnd * pMdiFrame =
// 			STATIC_DOWNCAST( CMDIFrameWnd, GetParentFrame() );
// 		ASSERT_VALID( pMdiFrame );
// 		BOOL bMax = FALSE;
// 		CMDIChildWnd * pActive = pMdiFrame->MDIGetActive( &bMax );
// 		pActive;
// 		if( ! bMax )
// 		{
// 			LRESULT lResult = CMDIChildWnd::WindowProc( message, wParam, lParam );
// 			return lResult;
// 		} // if( ! bMax )
// 		SetRedraw( FALSE );
// 		LRESULT lResult = CMDIChildWnd::WindowProc( message, wParam, lParam );
// 		SetRedraw( TRUE );
// 		RedrawWindow(
// 			NULL, NULL,
// 			RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN|RDW_FRAME
// 			);
// 		return lResult;
// 	} // else if( message == WM_WINDOWPOSCHANGED )
LRESULT lResult = CMDIChildWnd::WindowProc( message, wParam, lParam );
	if( message == WM_CREATE )
	{
		ASSERT_VALID( m_pBar );
		ASSERT( m_pBar->GetSafeHwnd() == m_hWndHelperBar );
		HWND hWndChild = ::GetWindow( m_pBar->GetSafeHwnd(), GW_CHILD );
		ASSERT( hWndChild != NULL );
		::__EXT_MFC_SetWindowLong( hWndChild, GWL_ID, AFX_IDW_PANE_FIRST );
		::SetParent( hWndChild, m_hWnd );
		if( ! m_icon.IsEmpty() )
		{
			ASSERT( m_hIconExtracted == NULL );
			m_hIconExtracted = m_icon.ExtractHICON();
			if( m_hIconExtracted != NULL )
			{
				SetIcon( m_hIconExtracted, FALSE );
				SetIcon( m_hIconExtracted, TRUE );
			}
		} // if( ! m_icon.IsEmpty() )
	
		CMenu * pSysMenu = GetSystemMenu( FALSE );
		if( pSysMenu != NULL )
		{
			ASSERT_KINDOF(CMenu, pSysMenu);
			pSysMenu->EnableMenuItem(
				SC_CLOSE,
				MF_BYCOMMAND
				| ( m_pBar->_ClosingIsEnabled( m_pBar ) ? MF_ENABLED : MF_DISABLED )
				);
		} // if( pSysMenu != NULL )

	} // if( message == WM_CREATE )
	return lResult;
}

BOOL CExtDynamicMDIChildWnd::DestroyWindow()
{
	ASSERT_VALID( this );
HWND hWndChild = ::GetWindow( m_hWnd, GW_CHILD );
	ASSERT( hWndChild != NULL );
	ASSERT_VALID( m_pBar );
	ASSERT( m_pBar->GetSafeHwnd() != NULL );
	if( ! m_pBar->_ClosingIsEnabled( m_pBar ) )
		return FALSE;
	::SetParent( hWndChild, m_pBar->m_hWnd );
CExtMiniDockFrameWnd * pMiniFrame = DYNAMIC_DOWNCAST( CExtMiniDockFrameWnd, m_pBar->GetParentFrame() );
	if( pMiniFrame != NULL )
	{
		bool bFloatingVisible = false;
		if(		(	m_pBar->m_eHelperSwitchingDBS == CExtDynamicControlBar::__EDBS_FLOATING
				||	m_pBar->m_eHelperSwitchingDBS == CExtDynamicControlBar::__EDBS_DOCKED
				)
			&&	m_pBar->m_bHelperSwitchingIntoVisibleState
			&&	m_pBar->BarStateGet() != CExtDynamicControlBar::__EDBS_DOCUMENT
			)
			bFloatingVisible = true;
		pMiniFrame->ModifyStyle( FWS_SNAPTOBARS, 0 );
		CSize _sizeF = m_pBar->GetSizeFloating();
		CRect rcWndBar( 0, 0, _sizeF.cx, _sizeF.cy );
		CRect rcClientFrame, rcWndFrame, rcFrameNC(0,0,0,0);
		pMiniFrame->GetWindowRect( &rcWndFrame );
		if(	! pMiniFrame->PmBridge_GetPM()->Bar_MiniDockFrameNcCalcSize(
				rcFrameNC,
				pMiniFrame,
				m_pBar
				)
			)
		{
			rcFrameNC.left = rcFrameNC.right = ::GetSystemMetrics( SM_CXSIZEFRAME );
			rcFrameNC.top = rcFrameNC.bottom = ::GetSystemMetrics( SM_CYSIZEFRAME );
			rcFrameNC.top += ::GetSystemMetrics( SM_CYSMCAPTION );
		}
		rcWndFrame.right = rcWndFrame.left + rcWndBar.Width(); // + rcFrameNC.left + rcFrameNC.right;
		rcWndFrame.bottom = rcWndFrame.top + rcWndBar.Height(); // + rcFrameNC.top + rcFrameNC.bottom;
		rcWndBar.right -= rcFrameNC.left + rcFrameNC.right;
		rcWndBar.bottom -= rcFrameNC.top + rcFrameNC.bottom;
		rcClientFrame = rcWndFrame;
		rcClientFrame.DeflateRect( 0, 0, 4, 4 );
		rcClientFrame.OffsetRect( -rcClientFrame.TopLeft() );
		bool bSingleBarFliatingMode = false;
		CWnd * pWnd = pMiniFrame->GetDlgItem( AFX_IDW_DOCKBAR_FLOAT );
		if( pWnd != NULL )
		{
			pWnd = pWnd->GetWindow( GW_CHILD );
			if( LPVOID(pWnd) == LPVOID(m_pBar) )
				bSingleBarFliatingMode = true;
		}
		if( bSingleBarFliatingMode )
		{
			CRect rcMove = rcWndFrame;
			rcMove.right += rcFrameNC.left + rcFrameNC.right - 4;
			rcMove.bottom += rcFrameNC.top + rcFrameNC.bottom - 4;
			pMiniFrame->MoveWindow( &rcMove );
		}
		pMiniFrame->DelayRecalcLayout();
		if( bFloatingVisible )
			m_pBar->DelayShow( TRUE );
		if( bFloatingVisible )
		{
			pMiniFrame->ShowWindow( SW_SHOWNOACTIVATE );
			pMiniFrame->RecalcLayout();
		}
		m_pBar->OnRepositionSingleChild();
		m_pBar->OnNcAreaButtonsReposition();
	} // if( pMiniFrame != NULL )
	else
	{
		CRect rcClient;
		m_pBar->GetClientRect( &rcClient );
		::MoveWindow(
			hWndChild,
			rcClient.left,
			rcClient.top,
			rcClient.Width(),
			rcClient.Height(),
			TRUE
			);
	} // else from if( pMiniFrame != NULL )
	return CMDIChildWnd::DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CExtDynamicBarHookSink

CExtDynamicBarHookSink::CExtDynamicBarHookSink()
	: m_pDBS( NULL )
{
}

CExtDynamicBarHookSink::~CExtDynamicBarHookSink()
{
}

CFrameWnd * CExtDynamicBarHookSink::DockSiteGet()
{
	ASSERT( this != NULL );
CExtDynamicBarSite * pDBS = GetBarSite();
	if( pDBS == NULL )
		return NULL;
	return pDBS->DockSiteGet();
}

CExtDynamicBarSite * CExtDynamicBarHookSink::GetBarSite()
{
	ASSERT( this != NULL );
	return m_pDBS;
}

bool CExtDynamicBarHookSink::OnHookWndMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	UINT nMessage,
	WPARAM & wParam,
	LPARAM & lParam
	)
{
	ASSERT( this != NULL );
CExtDynamicBarSite * pDBS = GetBarSite();
CFrameWnd * pDockSite = DockSiteGet();
	if(		pDockSite->GetSafeHwnd() != NULL
		&&	hWndHooked == pDockSite->GetSafeHwnd()
		)
	{
		switch( nMessage )
		{
		case WM_DESTROY:
			{
				//pDBS->_BarFreeAllImpl( true );
				if( pDockSite->IsKindOf( RUNTIME_CLASS(CMDIChildWnd) ) )
					pDBS->BarFreeAll();
			}
		break; // case WM_DESTROY
		default:
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			if( nMessage == CExtControlBar::g_nMsgCreateTabbedBar )
			{
				lResult = 0L;
				CExtDynTabControlBar ** ppTabbedBar =
					(CExtDynTabControlBar **)wParam;
				ASSERT( ppTabbedBar != NULL );
				(*ppTabbedBar) = pDBS->OnDbsCreateTabbedBarInstance();
				ASSERT( (*ppTabbedBar) != NULL );
				return true;
			} // if( nMessage == CExtControlBar::g_nMsgCreateTabbedBar )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			if( nMessage == CExtDynamicBarSite::g_nMsgFindDynamicBarSite )
			{
				CExtDynamicBarSite ** ppDBS =
					(CExtDynamicBarSite **)wParam;
				ASSERT( ppDBS != NULL );
				(*ppDBS) = pDBS;
				lResult = 0L;
				return true;
			} // if( nMessage == CExtDynamicBarSite::g_nMsgFindDynamicBarSite )
			if( nMessage == CExtPopupMenuWnd::g_nMsgPrepareMenu )
			{
				CExtPopupMenuWnd::MsgPrepareMenuData_t * pData =
					reinterpret_cast
					< CExtPopupMenuWnd::MsgPrepareMenuData_t * >
					( wParam );
				ASSERT( pData != NULL );
				CExtPopupMenuWnd * pPopup = pData->m_pPopup;
				ASSERT( pPopup != NULL );
				pData->m_bMenuChanged = OnPreparePopupMenu( pPopup );
				return false;
			} // if( nMessage == CExtPopupMenuWnd::g_nMsgPrepareMenu )
			if( nMessage == CExtMenuControlBar::g_nMsgPrepareSystemMenu )
			{
				CExtPopupMenuWnd * pPopup = (CExtPopupMenuWnd *)wParam;
				ASSERT_VALID( pPopup );
				ASSERT_KINDOF( CExtPopupMenuWnd, pPopup );
				CFrameWnd * pFrame = (CFrameWnd*)lParam;
				ASSERT_VALID( pFrame );
				ASSERT_KINDOF( CFrameWnd, pFrame );
				CExtDynamicMDIChildWnd * pDynamicMDIChildWnd = DYNAMIC_DOWNCAST( CExtDynamicMDIChildWnd, pFrame );
				if( pDynamicMDIChildWnd == NULL )
					return false;
				CExtDynamicControlBar * pBar = pDynamicMDIChildWnd->GetBar();
				ASSERT_VALID( pBar );
				pBar->OnInitDbsMenu(
					pPopup,
					pBar->m_hWnd,
					pDynamicMDIChildWnd
					);
				return false;
			} // if( nMessage == CExtMenuControlBar::g_nMsgPrepareSystemMenu )
			if( nMessage == CExtControlBar::g_nMsgConstructPopupMenu )
			{
				CExtControlBar::POPUP_MENU_EVENT_DATA * p_pmed =
					CExtControlBar::POPUP_MENU_EVENT_DATA::FromWParam( wParam );
				ASSERT( p_pmed != NULL );
				ASSERT_VALID( p_pmed->m_pPopupMenuWnd );
				ASSERT_VALID( p_pmed->m_pWndEventSrc );
				ASSERT( p_pmed->m_pWndEventSrc->GetSafeHwnd() != NULL );
				if( p_pmed->m_bPostNotification )
					return false;
#if ( (!defined __EXT_MFC_NO_TABMDI_CTRL) || (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL) )
				if(
#if (!defined __EXT_MFC_NO_TABMDI_CTRL)
					p_pmed->m_nHelperNotificationType == CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_MDITABS_CTX
#endif // (!defined __EXT_MFC_NO_TABMDI_CTRL)
#if ( (!defined __EXT_MFC_NO_TABMDI_CTRL) && (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL) )
					||
#endif // ( (!defined __EXT_MFC_NO_TABMDI_CTRL) && (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL) )
#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
					p_pmed->m_nHelperNotificationType == CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_TAB_PAGE_CONTAINER_TABS_CTX
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
					)
				{
					CExtTabWnd * pWndTab =
						STATIC_DOWNCAST(
							CExtTabWnd,
							p_pmed->m_pWndEventSrc
							);
					CPoint point = p_pmed->m_ptScreen;
					pWndTab->ScreenToClient( &point );
					LONG nItemIdx =
						pWndTab->ItemHitTest( point );
					if( nItemIdx < 0 )
						return false;
					HWND hWnd = (HWND)
						pWndTab->ItemGet( nItemIdx )->LParamGet();
					if( hWnd == NULL || (! ::IsWindow(hWnd) ) )
						return false;
					CExtDynamicControlBar * pBar = NULL;
					CObject * pObjHelperSrc = NULL;
#if (!defined __EXT_MFC_NO_TABMDI_CTRL)
					if( p_pmed->m_nHelperNotificationType == CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_MDITABS_CTX )
					{
						CWnd * pWnd =
							CWnd::FromHandlePermanent( hWnd );
						if( pWnd == NULL )
							return false;
						CExtDynamicMDIChildWnd * pDynamicMDIChildWnd = DYNAMIC_DOWNCAST( CExtDynamicMDIChildWnd, pWnd );
						if( pDynamicMDIChildWnd == NULL )
							return false;
						pObjHelperSrc = pDynamicMDIChildWnd;
						pBar =
							pDynamicMDIChildWnd->GetBar();
						ASSERT_VALID( pBar );
					} // if( p_pmed->m_nHelperNotificationType == CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_MDITABS_CTX )
#endif // (!defined __EXT_MFC_NO_TABMDI_CTRL)
#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
					if( p_pmed->m_nHelperNotificationType == CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_TAB_PAGE_CONTAINER_TABS_CTX )
					{
						CWnd * pWnd = CWnd::FromHandle( hWnd );
						pObjHelperSrc = pWnd;
						pBar =
							pDBS->BarFindByChildHWND( hWnd );
						if( pBar == NULL )
							return false;
						ASSERT_VALID( pBar );
					} // if( p_pmed->m_nHelperNotificationType == CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_TAB_PAGE_CONTAINER_TABS_CTX );
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
					pBar->OnInitDbsMenu(
						p_pmed->m_pPopupMenuWnd,
						pBar->m_hWnd,
						pObjHelperSrc
						);
					CExtPaintManager::stat_PassPaintMessages();
					lResult = 1L;
					return true;
				} // if( ...
#endif // ( (!defined __EXT_MFC_NO_TABMDI_CTRL) || (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL) )
				if(		p_pmed->m_nHelperNotificationType ==
							CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_CONTROLBAR_NC_CTX
					||	p_pmed->m_nHelperNotificationType ==
							CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_CONTROLBAR_CTX
					||	p_pmed->m_nHelperNotificationType ==
							CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_MINIFRAME_NC_CTX
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
					||	p_pmed->m_nHelperNotificationType ==
							CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_AUTOHIDESLIDER_CTX
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
					)
				{
					CExtDynamicControlBar * pBar =
						DYNAMIC_DOWNCAST(
							CExtDynamicControlBar,
							p_pmed->m_pWndEventSrc
							);
					if( pBar == NULL )
					{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
						CExtDynamicTabbedControlBar * pTabbedBar =
							DYNAMIC_DOWNCAST(
								CExtDynamicTabbedControlBar,
								p_pmed->m_pWndEventSrc
								);
						if( pTabbedBar == NULL )
							return false;
						LONG nSel = pTabbedBar->GetSwitcherSelection();
						if( nSel < 0 )
							return false;
						CExtControlBar * pExtBar =
							pTabbedBar->GetBarAt( nSel, true );
						if( pExtBar == NULL )
							return false;
						pBar =
							DYNAMIC_DOWNCAST(
								CExtDynamicControlBar,
								pExtBar
								);
						if( pBar == NULL )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
							return false;
					} // if( pBar == NULL )
					ASSERT_VALID( pBar );
					p_pmed->m_pPopupMenuWnd->ItemRemove();
					pBar->OnInitDbsMenu(
						p_pmed->m_pPopupMenuWnd,
						pBar->m_hWnd,
						pBar
						);
					lResult = 1L;
					return true;
				} // if( ...
				return false;
			} // if( nMessage == CExtControlBar::g_nMsgConstructPopupMenu )
 		break; // default
 		} // switch( nMessage )
	}
	return 
		CExtHookSink::OnHookWndMsg(
			lResult,
			hWndHooked,
			nMessage,
			wParam,
			lParam
			);
}

bool CExtDynamicBarHookSink::OnPreparePopupMenu( CExtPopupMenuWnd * pPopup )
{
	ASSERT( this != NULL );
	ASSERT_VALID( pPopup );
CExtDynamicBarSite * pDBS = GetBarSite();
	if( pDBS == NULL )
		return false;
CFrameWnd * pDockSite = pDBS->DockSiteGet();
	if( pDockSite->GetSafeHwnd() == NULL )
		return false;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( pDockSite->GetSafeHwnd() );
	if(		pSite != NULL
		&&	pSite->IsCustomizeMode()
		)
		return false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( CExtControlBar::FindHelpMode( pDockSite ) )
		return false;
	return pDBS->OnPreparePopupMenu( pPopup );
}

/////////////////////////////////////////////////////////////////////////////
// CExtDynamicBarSite

const UINT CExtDynamicBarSite::g_nMsgFindDynamicBarSite =
	::RegisterWindowMessage(
		_T("CExtDynamicBarSite::g_nMsgFindDynamicBarSite")
		);

CExtDynamicBarSite::CExtDynamicBarSite()
	: m_pDockSite( NULL )
	, m_eDUIT( CExtDynamicBarSite::__EDUIT_NOT_DETECTED_YET )
#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
	, m_pWndTabPageContainer( NULL )
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
#if (!defined __EXT_MFC_NO_TABMDI_CTRL)
	, m_pWndMdiTabs( NULL )
#endif // (!defined __EXT_MFC_NO_TABMDI_CTRL)
	, m_hWndMdiClient( NULL )
	, m_nBarListInMenusCmdID( 0 )
#if (! defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL) || (! defined __EXT_MFC_NO_TABMDI_CTRL)
	, m_bEnableDndDocumentTabs( true )
#endif // (! defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL) || (! defined __EXT_MFC_NO_TABMDI_CTRL)
{
	m_DbsHookSink.m_pDBS = this;
}

CExtDynamicBarSite::~CExtDynamicBarSite()
{
}

#if (! defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL) || (! defined __EXT_MFC_NO_TABMDI_CTRL)

bool CExtDynamicBarSite::OnStartDraggingDocumentTabItem(
	CExtDynamicControlBar * pBar,
	CExtTabWnd * pWndTab,
	LONG nTabItemIndex
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pBar );
	ASSERT_VALID( pWndTab );
	if( ! m_bEnableDndDocumentTabs )
		return false;
	return pBar->OnStartDraggingDocumentTabItem( pWndTab, nTabItemIndex );
}

#endif // (! defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL) || (! defined __EXT_MFC_NO_TABMDI_CTRL)

CExtDynamicBarSite::eDetectedUiType_t CExtDynamicBarSite::GetDetectedUiType() const
{
	ASSERT( this != NULL );
	if( m_eDUIT != __EDUIT_NOT_DETECTED_YET )
	{
		ASSERT(
				__EDUIT_VALID_VALUE_MIN <= m_eDUIT
			&&	m_eDUIT <= __EDUIT_VALID_VALUE_MAX
			);
		return m_eDUIT;
	}
const CFrameWnd * pDockSite = DockSiteGet();
	if( pDockSite->GetSafeHwnd() == NULL )
		return __EDUIT_UNSUPPORTED_ENVIRONMENT;
	ASSERT_VALID( pDockSite );
	m_hWndMdiClient =
		CExtControlBar::stat_FindMdiClientHWND(
			pDockSite->m_hWnd
			);
	if(		pDockSite->IsKindOf( RUNTIME_CLASS(CMDIFrameWnd) )
		||	m_hWndMdiClient != NULL
		)
	{
		m_eDUIT = __EDUIT_MDI_ORDINARY;
#if (!defined __EXT_MFC_NO_TABMDI_CTRL)
		HWND hWnd = ::GetWindow( pDockSite->m_hWnd, GW_CHILD );
		for( ; hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
		{
			if( hWnd == m_hWndMdiClient )
				continue;
			CWnd * pWnd =
				CWnd::FromHandlePermanent( hWnd );
			if( pWnd == NULL )
				continue;
			CExtTabWnd * pWndTab =
				DYNAMIC_DOWNCAST(
					CExtTabWnd,
					pWnd
					);
			if(		pWndTab == NULL
				||	(! pWndTab->_IsMdiTabCtrl() )
				)
				continue;
			m_pWndMdiTabs = pWndTab;
			m_eDUIT = __EDUIT_MDI_WITH_TABS;
			break;
		} // for( ; hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
#endif // (!defined __EXT_MFC_NO_TABMDI_CTRL)
	} // if( pDockSite->IsKindOf( RUNTIME_CLASS(CMDIFrameWnd) ) ...
	else
	{
#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
		HWND hWndCenter =
			::GetDlgItem(
				pDockSite->m_hWnd,
				AFX_IDW_PANE_FIRST
				);
		if( hWndCenter == NULL )
			return __EDUIT_UNSUPPORTED_ENVIRONMENT;
		CWnd * pWndCenter =
			CWnd::FromHandlePermanent( hWndCenter );
		if( pWndCenter == NULL )
			return __EDUIT_UNSUPPORTED_ENVIRONMENT;
		m_pWndTabPageContainer =
			DYNAMIC_DOWNCAST(
				CExtTabPageContainerWnd,
				pWndCenter
				);
		if( m_pWndTabPageContainer == NULL )
			return __EDUIT_UNSUPPORTED_ENVIRONMENT;
		m_eDUIT = __EDUIT_SDI_TAB_PAGE_CONTAINER;
#else
		m_eDUIT = __EDUIT_UNSUPPORTED_ENVIRONMENT;
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
	} // else from if( pDockSite->IsKindOf( RUNTIME_CLASS(CMDIFrameWnd) ) ...
	return m_eDUIT;
}

#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)

CExtTabPageContainerWnd * CExtDynamicBarSite::GetTabPageContainer()
{
	ASSERT( this != NULL );
	if( m_eDUIT == __EDUIT_NOT_DETECTED_YET )
		GetDetectedUiType();
	return m_pWndTabPageContainer;
}

const CExtTabPageContainerWnd * CExtDynamicBarSite::GetTabPageContainer() const
{
	ASSERT( this != NULL );
	if( m_eDUIT == __EDUIT_NOT_DETECTED_YET )
		GetDetectedUiType();
	return m_pWndTabPageContainer;
}

#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)

#if (!defined __EXT_MFC_NO_TABMDI_CTRL)

CExtTabWnd * CExtDynamicBarSite::GetMdiTabs()
{
	ASSERT( this != NULL );
	if( m_eDUIT == __EDUIT_NOT_DETECTED_YET )
		GetDetectedUiType();
	return m_pWndMdiTabs;
}

const CExtTabWnd * CExtDynamicBarSite::GetMdiTabs() const
{
	ASSERT( this != NULL );
	if( m_eDUIT == __EDUIT_NOT_DETECTED_YET )
		GetDetectedUiType();
	return m_pWndMdiTabs;
}

#endif // (!defined __EXT_MFC_NO_TABMDI_CTRL)

HWND CExtDynamicBarSite::GetMdiClient() const
{
	ASSERT( this != NULL );
	if( m_eDUIT == __EDUIT_NOT_DETECTED_YET )
		GetDetectedUiType();
	return m_hWndMdiClient;
}

CExtDynamicBarSite * CExtDynamicBarSite::FindBarSite(
	HWND hWnd
	)
{
	if( hWnd == NULL || (! ::IsWindow(hWnd) ) )
		return NULL;
CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
	if( pWnd != NULL )
	{
		CFrameWnd * pFrame = NULL;
		pFrame = DYNAMIC_DOWNCAST( CFrameWnd, pWnd );
		if( pFrame == NULL )
		{
			CExtDynamicControlBar * pBar =
				DYNAMIC_DOWNCAST( CExtDynamicControlBar, pWnd );
			if( pBar != NULL )
				pFrame = pBar->m_pDockSite;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			else
			{
				CExtDynamicTabbedControlBar * pTabbedBar =
					DYNAMIC_DOWNCAST( CExtDynamicTabbedControlBar, pWnd );
				if( pTabbedBar != NULL )
					pFrame = pTabbedBar->m_pDockSite;
			}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		} // if( pFrame == NULL )
		if( pFrame == NULL )
			pFrame = pWnd->GetParentFrame();
		if( pFrame != NULL )
		{
			if( pFrame->IsKindOf( RUNTIME_CLASS(CExtMiniDockFrameWnd) ) )
				pFrame = pFrame->GetParentFrame();
			if( pFrame != NULL )
			{
				CExtDynamicBarSite * pDBS = NULL;
				pFrame->SendMessage( g_nMsgFindDynamicBarSite, WPARAM(&pDBS) );
				if( pDBS != NULL )
					return pDBS;
			} // if( pFrame != NULL )
		} // if( pFrame != NULL )
	}
	for( ; hWnd != NULL; hWnd = ::GetParent( hWnd ) )
	{
		CExtDynamicBarSite * pDBS = NULL;
		::SendMessage( hWnd, g_nMsgFindDynamicBarSite, WPARAM(&pDBS), 0L );
		if( pDBS != NULL )
			return pDBS;
	} // for( ; hWnd != NULL; hWnd = ::GetParent( hWnd ) )
	return NULL;
}

CExtDynamicBarSite * CExtDynamicBarSite::FindBarSite(
	CWnd * pWnd
	)
{
HWND hWnd = pWnd->GetSafeHwnd();
	if( hWnd == NULL )
		return NULL;
	return FindBarSite( hWnd );
}

CFrameWnd * CExtDynamicBarSite::DockSiteGet()
{
	ASSERT( this != NULL );
	return m_pDockSite;
}

HWND CExtDynamicBarSite::GetActiveMdiChildFrameHWND(
	bool * p_bMaximized, // = NULL
	CMDIChildWnd ** ppMDIChildWnd // = NULL
	)
{
	ASSERT( this != NULL );
	if( p_bMaximized != NULL)
		(*p_bMaximized) = false;
	if( ppMDIChildWnd != NULL)
		(*ppMDIChildWnd) = NULL;
HWND hWndMDIClient = GetMdiClient();
	if( hWndMDIClient == NULL )
		return NULL;
BOOL bMaximized = FALSE;
HWND hWndActiveMdiChildFrame = (HWND)
		::SendMessage(
			hWndMDIClient,
			WM_MDIGETACTIVE,
			0,
			(LPARAM)&bMaximized
			);
	if( p_bMaximized != NULL)
		(*p_bMaximized) = bMaximized ? true : false;
	if( hWndActiveMdiChildFrame == NULL )
		return NULL;
CWnd * pWndPermanent =
		CWnd::FromHandlePermanent( hWndActiveMdiChildFrame );
	if( pWndPermanent == NULL )
		return hWndActiveMdiChildFrame;
CMDIChildWnd * pMDIChildWnd =
		DYNAMIC_DOWNCAST(
			CMDIChildWnd,
			pWndPermanent
			);
	if( pMDIChildWnd == NULL )
		return hWndActiveMdiChildFrame;
	if(		pMDIChildWnd->m_bPseudoInactive
		&&	(pMDIChildWnd->GetStyle() & WS_VISIBLE) == 0
		)
	{
		if( p_bMaximized != NULL)
			(*p_bMaximized) = false;
	}
	else if( ppMDIChildWnd != NULL )
		(*ppMDIChildWnd) = pMDIChildWnd;
	return hWndActiveMdiChildFrame;
}

const CFrameWnd * CExtDynamicBarSite::DockSiteGet() const
{
	ASSERT( this != NULL );
	return m_pDockSite;
}

void CExtDynamicBarSite::Install( CFrameWnd * pDockSite )
{
	ASSERT( this != NULL );
	ASSERT( m_pDockSite == NULL );
	m_pDockSite = pDockSite;
	ASSERT_VALID( m_pDockSite );
	ASSERT( m_pDockSite->GetSafeHwnd() != NULL );
	m_DbsHookSink.SetupHookWndSink( m_pDockSite->GetSafeHwnd() );
}

BOOL CExtDynamicBarSite::OnCmdMsg(
	UINT nID,
	int nCode,
	void * pExtra,
	AFX_CMDHANDLERINFO * pHandlerInfo
	)
{
	ASSERT( this != NULL );
	pHandlerInfo;
	if( nCode == CN_COMMAND || nCode == CN_UPDATE_COMMAND_UI )
	{
		POSITION pos = BarEnumGetStartPosition();
		for( ; pos != NULL; )
		{
			CExtDynamicControlBar * pBar =
				BarEnumGetNext( pos );
			ASSERT_VALID( pBar );
			if( UINT(pBar->GetDlgCtrlID()) == nID )
				return
					OnCmdMsgOneBar(
						pBar,
						nCode,
						pExtra,
						pHandlerInfo
						);
		} // for( ; pos != NULL; )
	} // if( nCode == CN_COMMAND || nCode == CN_UPDATE_COMMAND_UI )
	return FALSE;
}

BOOL CExtDynamicBarSite::OnCmdMsgOneBar(
	CExtDynamicControlBar * pBar,
	int nCode,
	void * pExtra,
	AFX_CMDHANDLERINFO * pHandlerInfo
	)
{
	ASSERT_VALID( pBar );
	pHandlerInfo;
CFrameWnd * pDockSite = DockSiteGet();
	if( pDockSite->GetSafeHwnd() == NULL )
		return FALSE;
	ASSERT_VALID( pDockSite );
	ASSERT( pDockSite->GetSafeHwnd() != NULL );
	if( nCode == CN_COMMAND )
	{
		CExtDynamicControlBar::eDynamicBarState_t eDBS =
			pBar->BarStateGet();
		if( eDBS == CExtDynamicControlBar::__EDBS_DOCUMENT )
		{
			pBar->BarStateSet( eDBS, true );
#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
			if( GetDetectedUiType() == __EDUIT_SDI_TAB_PAGE_CONTAINER )
			{
				HWND hWndChild = pBar->OnQueryChildHWND();
				CExtTabPageContainerWnd * pWndTabPageContainer =
					GetTabPageContainer();
				if(		pWndTabPageContainer != NULL
					&&	hWndChild != NULL
					&&	::IsWindow( hWndChild )
					)
				{
					int nPageCount =
						pWndTabPageContainer->PageGetCount();
					for( int nPageIdx = 0; nPageIdx < nPageCount; nPageIdx++ )
					{
						HWND hWnd =
							pWndTabPageContainer->PageHwndGetSafe(
								nPageIdx
								);
						if( hWnd == hWndChild )
						{
							int nSelIdx =
								pWndTabPageContainer->PageSelectionGet();
							if( nSelIdx != nPageIdx )
								pWndTabPageContainer->PageSelectionSet( nPageIdx );
							break;
						} // if( hWnd == hWndChild )
					} // for( int nPageIdx = 0; nPageIdx < nPageCount; nPageIdx++ )
				}
			} // if( GetDetectedUiType() == __EDUIT_SDI_TAB_PAGE_CONTAINER )
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
		} // if( eDBS == CExtDynamicControlBar::__EDBS_DOCUMENT )
		else
			CExtControlBar::DoFrameBarCheckCmd(
				pDockSite,
				UINT( pBar->GetDlgCtrlID() ),
				OnDbsQueryUseCheckMarkCommand( pBar )
				);
	} // if( nCode == CN_COMMAND )
	else
		CExtControlBar::DoFrameBarCheckUpdate(
			pDockSite,
			(CCmdUI*)pExtra,
			OnDbsQueryUseCheckMarkCommand( pBar )
			);
	return TRUE;
}

CExtDynamicMDIChildWnd * CExtDynamicBarSite::OnDbsCreateDynamicMDIChildWndInstance(
	CExtDynamicControlBar * pBar
	)
{
	ASSERT_VALID( pBar );
CExtDynamicMDIChildWnd * pWndMdiChild = new CExtNCW < CExtDynamicMDIChildWnd >;
	pWndMdiChild->AttachBar( pBar );
	return pWndMdiChild;
}

bool CExtDynamicBarSite::OnDbsQueryUseCheckMarkCommand(
	CExtDynamicControlBar * pBar
	)
{
	ASSERT_VALID( pBar );
	return pBar->OnQueryUseCheckMarkCommand();
}

bool CExtDynamicBarSite::StateSerialize(
	CArchive & ar,
	bool bEnableThrowExceptions // = false
	)
{
	ASSERT( this != NULL );
DWORD dwGeneralFlags = 0;
DWORD dwReserved = 0L;
	try
	{
		if( ar.IsStoring() )
		{
			ar << dwGeneralFlags;
			ar << dwReserved;
			ar << dwReserved;
			ar << dwReserved;
			ar << dwReserved;
			DWORD dwCount = (DWORD)BarEnumGetCount();
			ar << dwCount;
			POSITION pos = BarEnumGetStartPosition();
			for( ; pos != NULL; )
			{
				CExtDynamicControlBar * pBar =
					BarEnumGetNext( pos );
				ASSERT_VALID( pBar );
				bool bVisibleState = false;
				if( pBar->BarStateGet( &bVisibleState ) == CExtDynamicControlBar::__EDBS_DOCUMENT )
				{
					if( bVisibleState )
					{
						ASSERT( m_listBarsInDocMode.Find( pBar ) != NULL );
						continue;
					}
				}
				ASSERT( m_listBarsInDocMode.Find( pBar ) == NULL );
				UINT nCmdID = (UINT)pBar->GetDlgCtrlID();
				ar << DWORD(nCmdID);
				bool bPersistentBar = pBar->IsPersistentBar(); 
				DWORD dwSerializeFlags = 0;
				if( bPersistentBar )
					dwSerializeFlags |= 0x01;
				ar << dwSerializeFlags;
				if( ! bPersistentBar )
				{
					CRuntimeClass * pRTC = pBar->GetRuntimeClass();
					ASSERT( pRTC != NULL );
					ar.WriteClass( pRTC );
				} // if( ! bPersistentBar )
				ar << pBar->m_strCaptionText;
				pBar->m_icon.Serialize( ar );
				pBar->OnSerializeDynamicProps( ar );
			} // for( ; pos != NULL; )
			CTypedPtrList < CPtrList, CExtDynamicControlBar * > _listSerialize;
			bool bUseDefaultOrder = true;
			CExtDynamicBarSite::eDetectedUiType_t eDUIT	= GetDetectedUiType();
			eDUIT;
#if (!defined __EXT_MFC_NO_TABMDI_CTRL)
			if( eDUIT == __EDUIT_MDI_WITH_TABS )
			{ // detect MDI order using tabs
				CExtTabWnd * pTabs = GetMdiTabs();
				if( pTabs != NULL )
				{
					LONG nIndex, nCount = pTabs->ItemGetCount();
					for( nIndex = 0L; nIndex < nCount; nIndex ++ )
					{
						HWND hWnd = (HWND)pTabs->ItemGet( nIndex )->LParamGet();
						CWnd * pWndPremanent = CWnd::FromHandlePermanent( hWnd );
						if( pWndPremanent == NULL )
							continue;
						CExtDynamicMDIChildWnd * pWndMdiChild = DYNAMIC_DOWNCAST( CExtDynamicMDIChildWnd, pWndPremanent );
						if( pWndMdiChild == NULL )
							continue;
						if( pWndMdiChild->GetBarSite() != this )
							continue;
						CExtDynamicControlBar * pBar = pWndMdiChild->GetBar();
						if( pBar == NULL )
							continue;
						if( m_listBarsInDocMode.Find( pBar ) == NULL )
							continue;
						_listSerialize.AddTail( pBar );
					} // for( nIndex = 0L; nIndex < nCount; nIndex ++ )
					if( _listSerialize.GetCount() == m_listBarsInDocMode.GetCount() )
						bUseDefaultOrder = false;
					else
						_listSerialize.RemoveAll();
				} // if( pTabs != NULL )
			} // detect MDI order using tabs
#endif // (!defined __EXT_MFC_NO_TABMDI_CTRL)
#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
			if( eDUIT == __EDUIT_SDI_TAB_PAGE_CONTAINER )
			{ // detect document mode order using SDI tab page container
				CExtTabPageContainerWnd * pWndTabPageContainer = GetTabPageContainer();
				if( pWndTabPageContainer->GetSafeHwnd() != NULL )
				{
					ASSERT_VALID( pWndTabPageContainer );
					CTypedPtrList < CPtrList, CExtDynamicControlBar * >  _listFound;
					INT nIndex = 0, nCount = pWndTabPageContainer->PageGetCount();
					for( nIndex = 0; nIndex < nCount; nIndex ++ )
					{
						HWND hWndPage = pWndTabPageContainer->PageHwndGetSafe( nIndex );
						if( hWndPage == NULL )
							continue;
						CExtDynamicControlBar * pBar = BarFindByChildHWND( hWndPage );
						if( pBar == NULL )
							continue;
						ASSERT_VALID( pBar );
#ifdef _DEBUG
						bool bVisibleState = false;
						ASSERT( pBar->BarStateGet( &bVisibleState ) == CExtDynamicControlBar::__EDBS_DOCUMENT );
						ASSERT( bVisibleState );
#endif // _DEBUG
						if( m_listBarsInDocMode.Find( pBar ) == NULL )
							continue;
						_listSerialize.AddTail( pBar );
					}
					if( _listSerialize.GetCount() == m_listBarsInDocMode.GetCount() )
						bUseDefaultOrder = false;
					else
						_listSerialize.RemoveAll();
				}
			} // detect document mode order using SDI tab page container
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
			if( bUseDefaultOrder )
				_listSerialize.AddTail( &m_listBarsInDocMode );
			//pos = m_listBarsInDocMode.GetTailPosition();
			pos = _listSerialize.GetTailPosition();
			for( ; pos != NULL; )
			{
				//CExtDynamicControlBar * pBar = m_listBarsInDocMode.GetPrev( pos );
				CExtDynamicControlBar * pBar = _listSerialize.GetPrev( pos );
				ASSERT_VALID( pBar );
				ASSERT( pBar->BarStateGet() == CExtDynamicControlBar::__EDBS_DOCUMENT );
				UINT nCmdID = (UINT)pBar->GetDlgCtrlID();
				ar << DWORD(nCmdID);
				bool bPersistentBar = pBar->IsPersistentBar(); 
				DWORD dwSerializeFlags = 0;
				if( bPersistentBar )
					dwSerializeFlags |= 0x01;
				ar << dwSerializeFlags;
				if( ! bPersistentBar )
				{
					CRuntimeClass * pRTC = pBar->GetRuntimeClass();
					ASSERT( pRTC != NULL );
					ar.WriteClass( pRTC );
				} // if( ! bPersistentBar )
				ar << pBar->m_strCaptionText;
				pBar->m_icon.Serialize( ar );
				pBar->OnSerializeDynamicProps( ar );
			} // for( ; pos != NULL; )
		} // if( ar.IsStoring() )
		else
		{
			ar >> dwGeneralFlags;
			ar >> dwReserved;
			ar >> dwReserved;
			ar >> dwReserved;
			ar >> dwReserved;
			BarFreeAll( false, true );
			m_listBarsInDocMode.RemoveAll();
			ASSERT( BarEnumGetCount( false, true ) == 0 );
			DWORD dwCount;
			ar >> dwCount;
			for( DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++ )
			{
				UINT nCmdID;
				DWORD dwTmp;
				ar >> dwTmp;
				nCmdID = UINT(dwTmp);
				DWORD dwSerializeFlags;
				ar >> dwSerializeFlags;
				CExtDynamicControlBar * pBar = NULL;
				if( (dwSerializeFlags & 0x01) != 0 )
				{ // if serializing persistent bar
					pBar = BarGetByCmdID( nCmdID );
					if( pBar == NULL )
					{
						ASSERT( FALSE );
						BarFreeAll( false, true );
					#if _MFC_VER >= 0x0800
						::AfxThrowArchiveException( CArchiveException::genericException );
					#else
						::AfxThrowArchiveException( CArchiveException::generic );
					#endif
					} // if( pBar == NULL )
					ASSERT_VALID( pBar );
					ar >> pBar->m_strCaptionText;
					pBar->m_icon.Serialize( ar );
				} // if serializing persistent bar
				else
				{ // if serializing dynamic bar
					CRuntimeClass * pRTC = ar.ReadClass();
					ASSERT( pRTC != NULL );
					CExtSafeString strCaptionText;
					ar >> strCaptionText;
					CExtCmdIcon icon;
					icon.Serialize( ar );
					pBar =
						BarAlloc(
							strCaptionText,
							icon,
							nCmdID,
							pRTC
							);
					if( pBar == NULL )
					{
						ASSERT( FALSE );
						BarFreeAll( false, true );
					#if _MFC_VER >= 0x0800
						::AfxThrowArchiveException( CArchiveException::genericException );
					#else
						::AfxThrowArchiveException( CArchiveException::generic );
					#endif
					} // if( pBar == NULL )
				} // if serializing dynamic bar
				ASSERT_VALID( pBar );
				pBar->OnSerializeDynamicProps( ar );
			} // for( DWORD dwIndex = 0; dwIndex < dwCount; dwIndex++ )
			ASSERT( ((DWORD)BarEnumGetCount()) == dwCount );
		} // else from if( ar.IsStoring() )
		return true;
	} // try
	catch( CException * pException )
	{
		if( bEnableThrowExceptions )
			throw;
		pException->Delete();
	} // catch( CException * pException )
	catch( ... )
	{
		if( bEnableThrowExceptions )
			throw;
	} // catch( ... )
dwGeneralFlags;
dwReserved;
	return false;
}

bool CExtDynamicBarSite::StateLoad(
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%
	HKEY hKeyRoot, // = HKEY_CURRENT_USER
	bool bEnableThrowExceptions // = false
	)
{
	ASSERT( this != NULL );
	ASSERT( sSectionNameCompany != NULL );
	ASSERT( sSectionNameProduct != NULL );
	ASSERT( sSectionNameProfile != NULL );
const CFrameWnd * pDockSite = DockSiteGet();
	if( pDockSite->GetSafeHwnd() == NULL )
	{
		if( bEnableThrowExceptions )
			::AfxThrowUserException();
		return false;
	}
	ASSERT_VALID( pDockSite );
	ASSERT( pDockSite->GetSafeHwnd() != NULL );
CExtSafeString sRegKeyPath =
		CExtCmdManager::GetSubSystemRegKeyPath(
			__PROF_UIS_REG_DBS,
			sSectionNameProfile,
			sSectionNameCompany,
			sSectionNameProduct
			);
	try
	{
		CMemFile _file;
		if( ! CExtCmdManager::FileObjFromRegistry(
				_file,
				sRegKeyPath,
				hKeyRoot,
				bEnableThrowExceptions
				)
			)
			return false;
		CArchive ar(
			&_file,
			CArchive::load
			);
		if( ! StateSerialize( ar ) )
			return false;
		return true;
	} // try
	catch( CException * pXept )
	{
		if( bEnableThrowExceptions )
			throw;
		pXept->Delete();
		//ASSERT( FALSE );
	} // catch( CException * pXept )
	catch( ... )
	{
		if( bEnableThrowExceptions )
			throw;
		//ASSERT( FALSE );
	} // catch( ... )
	return false;
}

bool CExtDynamicBarSite::StateSave(
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%
	HKEY hKeyRoot, // = HKEY_CURRENT_USER
	bool bEnableThrowExceptions // = false
	)
{
	ASSERT( this != NULL );
	ASSERT( sSectionNameCompany != NULL );
	ASSERT( sSectionNameProduct != NULL );
	ASSERT( sSectionNameProfile != NULL );
const CFrameWnd * pDockSite = DockSiteGet();
	if( pDockSite->GetSafeHwnd() == NULL )
	{
		if( bEnableThrowExceptions )
			::AfxThrowUserException();
		return false;
	}
	ASSERT_VALID( pDockSite );
	ASSERT( pDockSite->GetSafeHwnd() != NULL );
CExtSafeString sRegKeyPath =
		CExtCmdManager::GetSubSystemRegKeyPath(
			__PROF_UIS_REG_DBS,
			sSectionNameProfile,
			sSectionNameCompany,
			sSectionNameProduct
			);
	try
	{
		CMemFile _file;
		CArchive ar(
			&_file,
			CArchive::store
			);
		if( ! StateSerialize( ar ) )
			return false;
		ar.Flush();
		ar.Close();
		if( ! CExtCmdManager::FileObjToRegistry(
				_file,
				sRegKeyPath,
				hKeyRoot,
				bEnableThrowExceptions
				)
			)
			return false;
		return true;
	} // try
	catch( CException * pXept )
	{
		if( bEnableThrowExceptions )
			throw;
		pXept->Delete();
		//ASSERT( FALSE );
	} // catch( CException * pXept )
	catch( ... )
	{
		if( bEnableThrowExceptions )
			throw;
		//ASSERT( FALSE );
	} // catch( ... )
	return false;
}

#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
bool CExtDynamicBarSite::OnTabPageContainerClickedButton(
	LONG nHitTest,
	bool bButtonPressed,
	INT nMouseButton, // MK_... values
	UINT nMouseEventFlags
	)
{
	ASSERT( this != NULL );
	ASSERT( GetDetectedUiType() == __EDUIT_SDI_TAB_PAGE_CONTAINER );
CExtTabPageContainerWnd * pWndTabPageContainer = GetTabPageContainer();
	ASSERT_VALID( pWndTabPageContainer );
	nMouseButton;
	nMouseEventFlags;
	nHitTest;
	if(		bButtonPressed
		||	nMouseButton != MK_LBUTTON
		||	nHitTest != __ETWH_BUTTON_CLOSE
		||	pWndTabPageContainer->PageGetCount() == 0
		)
		return true;
int nSel = pWndTabPageContainer->PageSelectionGet();
	if( nSel < 0 )
		return true;
HWND hWndChild =
		pWndTabPageContainer->PageHwndGetSafe( nSel );
	if( hWndChild == NULL )
		return true;
	ASSERT( ::IsWindow( hWndChild ) );
CExtDynamicControlBar * pBar =
		BarFindByChildHWND( hWndChild );
	if( pBar == NULL )
		return true;
	if( ! pBar->_ClosingIsEnabled( pBar ) )
		return true;
	pBar->BarStateSet(
		CExtDynamicControlBar::__EDBS_DOCUMENT,
		false
		);
	return true;
}
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)

bool CExtDynamicBarSite::OnPreparePopupMenu( CExtPopupMenuWnd * pPopup )
{
	ASSERT( this != NULL );
	ASSERT_VALID( pPopup );
	if( m_nBarListInMenusCmdID == 0 )
		return false;
bool bRetVal = false;
INT nItemCount = pPopup->ItemGetCount();
	for( INT nItemIndex = 0; nItemIndex < nItemCount; )
	{
		CExtPopupMenuWnd::MENUITEMDATA & _mii =
			pPopup->ItemGetInfo( nItemIndex );
		if( _mii.IsSeparator() )
		{
			nItemIndex ++;
			continue;
		}
		if( _mii.IsPopup() )
		{
			CExtPopupMenuWnd * pSubPopup =
				_mii.GetPopup();
			ASSERT_VALID( pSubPopup );
			if( OnPreparePopupMenu( pSubPopup ) )
				bRetVal = true;
			nItemIndex ++;
			continue;
		}
		UINT nCmdID = _mii.GetCmdID();
		if( nCmdID != m_nBarListInMenusCmdID )
		{
			nItemIndex ++;
			continue;
		}
		pPopup->ItemRemove( nItemIndex );
		nItemCount--;
		if(		nItemIndex > 0
			&&	pPopup->ItemGetInfo(nItemIndex-1).IsSeparator()
			)
		{
			nItemIndex--;
			nItemCount--;
			pPopup->ItemRemove( nItemIndex );
		}
		INT nInsertedCount =
			OnPrepareBarsListInMenu(
				pPopup,
				nItemIndex
				);
		if( nInsertedCount == 0 )
			continue;
		bRetVal = true;
		if(		nItemIndex > 0
			&&	(! pPopup->ItemGetInfo(nItemIndex-1).IsSeparator() )
			)
		{
			pPopup->ItemInsert( ID_SEPARATOR, nItemIndex );
			nItemIndex++;
			nItemCount++;
		}
		nItemIndex += nInsertedCount;
		nItemCount += nInsertedCount;
	} // for( INT nItemIndex = 0; nItemIndex < nItemCount; )
	return bRetVal;
}

INT CExtDynamicBarSite::OnPrepareBarsListInMenu(
	CExtPopupMenuWnd * pPopup,
	INT nInsertPos
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pPopup );
	ASSERT( 0 <= nInsertPos && nInsertPos <= pPopup->ItemGetCount() );
POSITION pos = BarEnumGetStartPosition();
INT nInsertedCount = 0;
	for( ; pos != NULL; )
	{
		CExtDynamicControlBar * pBar =
			BarEnumGetNext( pos );
		ASSERT_VALID( pBar );
		ASSERT( pBar->GetSafeHwnd() != NULL );
		ASSERT_VALID( pBar->m_pDockBar );
		ASSERT( pBar->m_pDockSite == DockSiteGet() );
		if(	pBar->OnInsertDynamicBarCommandIntoPopupMenu(
				pPopup,
				nInsertPos + nInsertedCount
				)
			)
			nInsertedCount++;
	} // for( ; pos != NULL; )
	return nInsertedCount;
}

UINT CExtDynamicBarSite::OnDbsAllocNewBarCommandID(
	__EXT_MFC_SAFE_LPCTSTR strCaptionText,
	const CExtCmdIcon & icon,
	UINT nDesiredCmdID // = 0
	) const
{
	ASSERT( this != NULL );
const CFrameWnd * pDockSite = DockSiteGet();
	if( pDockSite->GetSafeHwnd() == NULL )
		return 0;
	ASSERT_VALID( pDockSite );
	ASSERT( pDockSite->GetSafeHwnd() != NULL );
LPCTSTR strProfileName =
		g_CmdManager->ProfileNameFromWnd(
			pDockSite->GetSafeHwnd()
			);
	if( strProfileName == NULL )
		return 0;
CExtCmdItem * pCmdItem =
		g_CmdManager->CmdAllocPtr(
			strProfileName,
			nDesiredCmdID
			);
	if( pCmdItem == NULL )
		return 0;
	if( strCaptionText != NULL )
	{
		pCmdItem->m_sMenuText = strCaptionText;
		pCmdItem->m_sToolbarText = strCaptionText;
	} // if( strCaptionText != NULL )
	if( ! icon.IsEmpty() )
		g_CmdManager->CmdSetIcon(
			strProfileName,
			pCmdItem->m_nCmdID,
			icon
			);
	return pCmdItem->m_nCmdID;
}

void CExtDynamicBarSite::OnDbsFreeBarCommandID( UINT nCmdID ) const
{
	ASSERT( this != NULL );
const CFrameWnd * pDockSite = DockSiteGet();
	if( pDockSite->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( pDockSite );
	ASSERT( pDockSite->GetSafeHwnd() != NULL );
LPCTSTR strProfileName =
		g_CmdManager->ProfileNameFromWnd(
			pDockSite->GetSafeHwnd()
			);
	if( strProfileName == NULL )
		return;
	g_CmdManager->CmdSetIcon( strProfileName, nCmdID, (HICON)NULL );
	g_CmdManager->CmdRemove( strProfileName, nCmdID );
}

void CExtDynamicBarSite::OnDbsResetPageSequence()
{
	ASSERT( this != NULL );
#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
	if( GetDetectedUiType() == CExtDynamicBarSite::__EDUIT_SDI_TAB_PAGE_CONTAINER )
	{
		CExtTabPageContainerWnd * pWndTabPageContainer = GetTabPageContainer();
		if( pWndTabPageContainer->GetSafeHwnd() == NULL )
			return;
		ASSERT_VALID( pWndTabPageContainer );
		CTypedPtrList < CPtrList, CExtDynamicControlBar * >  _listFound;
		INT nIndex = 0, nCount = pWndTabPageContainer->PageGetCount();
		for( nIndex = 0; nIndex < nCount; nIndex ++ )
		{
			HWND hWndPage = pWndTabPageContainer->PageHwndGetSafe( nIndex );
			if( hWndPage == NULL )
				continue;
			CExtDynamicControlBar * pBar = BarFindByChildHWND( hWndPage );
			if( pBar == NULL )
				continue;
			ASSERT_VALID( pBar );
#ifdef _DEBUG
			bool bVisibleState = false;
			ASSERT( pBar->BarStateGet( &bVisibleState ) == CExtDynamicControlBar::__EDBS_DOCUMENT );
			ASSERT( bVisibleState );
#endif // _DEBUG
			ASSERT( m_listBarsInDocMode.Find( pBar ) != NULL );
			_listFound.AddTail( pBar );
		}
		if( _listFound.GetCount() == m_listBarsInDocMode.GetCount() )
		{
			m_listBarsInDocMode.RemoveAll();
			m_listBarsInDocMode.AddTail( &_listFound );
			ASSERT( _listFound.GetCount() == m_listBarsInDocMode.GetCount() );
		}
		return;
	} // if( GetDetectedUiType() == CExtDynamicBarSite::__EDUIT_SDI_TAB_PAGE_CONTAINER )
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
}

CExtDynamicControlBar * CExtDynamicBarSite::OnDbsCreateNewBarInstance() const
{
	ASSERT( this != NULL );
	return new CExtDynamicControlBar;
}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

CExtDynamicTabbedControlBar * CExtDynamicBarSite::OnDbsCreateTabbedBarInstance() const
{
	ASSERT( this != NULL );
CExtDynamicTabbedControlBar * pBar = new CExtDynamicTabbedControlBar;
	return pBar;
}

#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

//CExtDynamicControlBar * CExtDynamicBarSite::BarFindAnyVisible()
//{
//	ASSERT( this != NULL );
//CFrameWnd * pDockSite = DockSiteGet();
//	if( pDockSite->GetSafeHwnd() == NULL )
//		return NULL;
//	ASSERT_VALID( pDockSite );
//	ASSERT( pDockSite->GetSafeHwnd() != NULL );
//POSITION pos = BarEnumGetStartPosition();
//	for( ; pos != NULL; )
//	{
//		ASSERT( pos != NULL );
//		CExtDynamicControlBar * pBar =
//			BarEnumGetNext( pos );
//		ASSERT_VALID( pBar );
//		ASSERT( pBar->GetSafeHwnd() != NULL );
//		ASSERT_VALID( pBar->m_pDockBar );
//		ASSERT( pBar->m_pDockSite == pDockSite );
//		if( pBar->AutoHideModeGet() )
//			continue;
//		if( ! pBar->IsVisible() )
//			continue;
//		return pBar;
//	} // for( ; pos != NULL; )
//	return NULL;
//}

CExtDynamicControlBar * CExtDynamicBarSite::BarFindByChildHWND( HWND hWndChildToFind )
{
	ASSERT( this != NULL );
	ASSERT( hWndChildToFind != NULL && ::IsWindow( hWndChildToFind ) );
POSITION pos = BarEnumGetStartPosition();
	for( ; pos != NULL; )
	{
		ASSERT( pos != NULL );
		CExtDynamicControlBar * pBar =
			BarEnumGetNext( pos );
		ASSERT_VALID( pBar );
		ASSERT( pBar->GetSafeHwnd() != NULL );
		ASSERT_VALID( pBar->m_pDockBar );
		HWND hWnd = pBar->OnQueryChildHWND();
		if( hWnd == NULL )
			continue;
		if( hWndChildToFind == hWnd )
			return pBar;
	} // for( ; pos != NULL; )
	return NULL;
}

CExtDynamicControlBar * CExtDynamicBarSite::BarAlloc(
	__EXT_MFC_SAFE_LPCTSTR strCaptionText,
	const CExtCmdIcon & icon,
	UINT nDesiredCmdID, // = 0
	CRuntimeClass * pDesiredRTC, // = NULL
	bool bPersistentBar // = false
	)
{
	ASSERT( this != NULL );
CFrameWnd * pDockSite = DockSiteGet();
	if( pDockSite->GetSafeHwnd() == NULL )
		return NULL;
	ASSERT_VALID( pDockSite );
	ASSERT( pDockSite->GetSafeHwnd() != NULL );
UINT nCmdID =
		OnDbsAllocNewBarCommandID(
			strCaptionText,
			icon,
			nDesiredCmdID
			);
	if( nCmdID == 0 )
		return NULL;
CExtDynamicControlBar * pBar =
		( pDesiredRTC != NULL )
			? ( (CExtDynamicControlBar *) pDesiredRTC->CreateObject() )
			: OnDbsCreateNewBarInstance()
			;
	if( pBar == NULL )
	{
		OnDbsFreeBarCommandID( nCmdID );
		return NULL;
	}
	ASSERT_VALID( pBar );
	ASSERT_KINDOF( CExtDynamicControlBar, pBar );
	pBar->m_bPersistentBar = bPersistentBar;
	if( ! pBar->Create(
			( strCaptionText == NULL )
				? _T("") : strCaptionText,
			pDockSite,
			nCmdID,
			WS_CHILD //|WS_VISIBLE
				|WS_CLIPCHILDREN|WS_CLIPSIBLINGS
				|CBRS_TOP|CBRS_GRIPPER|CBRS_TOOLTIPS
				|CBRS_FLYBY|CBRS_SIZE_DYNAMIC
				|CBRS_HIDE_INPLACE
			)
		)
	{
		OnDbsFreeBarCommandID( nCmdID );
		delete pBar;
		return NULL;
	}
	pBar->m_strCaptionText =
		( strCaptionText == NULL )
			? _T("") : strCaptionText;
	pBar->m_icon = icon;
//CExtDynamicControlBar * pOtherBar = BarFindAnyVisible();
	m_mapBars.SetAt( pBar, NULL );
	m_listBars.AddTail( pBar );
	pBar->EnableDocking( CBRS_ALIGN_ANY );
//	if( pOtherBar != NULL )
//	{
//		ASSERT_VALID( pOtherBar );
//		VERIFY(
//			pOtherBar->DockControlBarIntoTabbedContainer(
//				pBar,
//				-1,
//				pDockSite,
//				false
//				)
//			);
//	} // if( pOtherBar != NULL )
//	else
	{
		VERIFY(
			pBar->DockControlBarInnerOuter(
				AFX_IDW_DOCKBAR_LEFT,
				true,
				pDockSite,
				false
				)
			);
		pDockSite->ShowControlBar( pBar, FALSE, TRUE );
	} // else from if( pOtherBar != NULL )
	return pBar;
}

void CExtDynamicBarSite::BarFree( CExtDynamicControlBar * pBar )
{
	ASSERT( this != NULL );
	_BarFreeImpl(
		pBar,
		false,
		false
		);
}

void CExtDynamicBarSite::_BarFreeImpl(
	CExtDynamicControlBar * pBar,
	bool bForceFloatMode,
	bool bForceNoOptimizeMode
	)
{
	ASSERT( this != NULL );
	ASSERT_VALID( pBar );
UINT nCmdID = (UINT)pBar->GetDlgCtrlID();
bool bPersistentBar = pBar->IsPersistentBar();
	if( pBar->m_pDockSite != NULL && pBar->m_pDockBar != NULL )
	{
		ASSERT_VALID( pBar->m_pDockSite );
		ASSERT_VALID( pBar->m_pDockBar );
		HWND hWndChild = pBar->OnQueryChildHWND();
		if( hWndChild != NULL && ::IsWindow( hWndChild ) )
		{
			//pBar->BarStateSet( CExtDynamicControlBar::__EDBS_DOCKED, false );
			HWND hWndContainer = ::GetParent( hWndChild );
			if( hWndContainer != pBar->m_hWnd )
			{
				CWnd * pWndPermanent =
					CWnd::FromHandlePermanent( hWndContainer );
				if( pWndPermanent != NULL )
				{
					CExtDynamicBarSite::eDetectedUiType_t eDUIT	= GetDetectedUiType();
					if(		eDUIT == __EDUIT_MDI_ORDINARY
						||	eDUIT == __EDUIT_MDI_WITH_TABS
						)
					{ // if MDI environment
						CExtDynamicMDIChildWnd * pWndMdiChild =
							DYNAMIC_DOWNCAST(
								CExtDynamicMDIChildWnd,
								pWndPermanent
								);
						if( pWndMdiChild != NULL )
							pWndMdiChild->DestroyWindow();
					} // if MDI environment
#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
					else if( eDUIT == __EDUIT_SDI_TAB_PAGE_CONTAINER )
					{ // if SDI tab page container environment
						CExtTabPageContainerWnd * pWndTabPageContainer =
							DYNAMIC_DOWNCAST(
								CExtTabPageContainerWnd,
								pWndPermanent
								);
						if( pWndTabPageContainer != NULL )
						{
							int nPageCount =
								pWndTabPageContainer->PageGetCount();
							for( int nPageIdx = 0; nPageIdx < nPageCount; nPageIdx++ )
							{
								HWND hWnd =
									pWndTabPageContainer->PageHwndGetSafe(
										nPageIdx
										);
								if( hWnd == hWndChild )
								{
									pWndTabPageContainer->
										PageRemove( nPageIdx, 1, true );
									break;
								} // if( hWnd == hWndChild )
							} // for( int nPageIdx = 0; nPageIdx < nPageCount; nPageIdx++ )
						} // if( pWndTabPageContainer != NULL )
					} // if SDI tab page container environment
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
				} // if( pWndPermanent != NULL )
			} // if( hWndContainer != pBar->m_hWnd )
		} // if( hWndChild != NULL && ::IsWindow( hWndChild ) )
	} // if( pBar->m_pDockSite != NULL && pBar->m_pDockBar != NULL )
	if(		pBar->m_pDockBar == NULL
		||	bForceFloatMode
		)
	{
		CWnd * pWnd = NULL;
		if( ! m_mapBars.Lookup( pBar, pWnd ) )
			return;
		m_mapBars.RemoveKey( pBar );
		POSITION posInList = m_listBars.Find( pBar );
		if( posInList != NULL )
			m_listBars.RemoveAt( posInList );
		posInList = m_listBarsInDocMode.Find( pBar );
		if( posInList != NULL )
			m_listBarsInDocMode.RemoveAt( posInList );
		if( pBar->m_pDockBar != NULL )
		{
			CMiniDockFrameWnd * pMiniFrame = NULL;
			CWnd * pWnd = pBar->GetParentFrame();
			if( pWnd != NULL )
			{
				pMiniFrame = 
					DYNAMIC_DOWNCAST(
						CMiniDockFrameWnd,
						pBar->GetDockingFrame()
						);
			}
			int nPos = pBar->m_pDockBar->FindBar( pBar );
			if( nPos > 0 )
			{
			//	VERIFY( pBar->m_pDockBar->RemoveControlBar(pBar) );
				pBar->m_pDockBar->m_arrBars.RemoveAt( nPos );
			}
			CFrameWnd * pDockSite = DockSiteGet();
			if( pDockSite->GetSafeHwnd() != NULL )
				pDockSite->RemoveControlBar( pBar );
			pBar->m_pDockSite = NULL;
			pBar->m_pDockBar = NULL;
			if( pMiniFrame != NULL )
				pMiniFrame->DestroyWindow();
			else
				pBar->DestroyWindow();
		} // if( pBar->m_pDockBar != NULL )
		return;
	}
	ASSERT_VALID( pBar->m_pDockBar );
CFrameWnd * pDockSite = DockSiteGet();
	if( pDockSite->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( pDockSite );
	ASSERT( pBar->m_pDockSite == pDockSite );
	ASSERT( pDockSite->GetSafeHwnd() != NULL );
CWnd * pWnd = NULL;
	if( ! m_mapBars.Lookup( pBar, pWnd ) )
		return;
CMiniDockFrameWnd * pMiniFrame = NULL;
	if( pBar->IsFloating() )
	{
		pMiniFrame = 
			DYNAMIC_DOWNCAST(
				CMiniDockFrameWnd,
				pBar->GetDockingFrame()
				);
		ASSERT_VALID( pMiniFrame );
	} // if( pBar->IsFloating() )
	else
	{
		ASSERT( ! pBar->m_pDockBar->m_bFloating );
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( pBar->AutoHideModeGet() )
		{
			ASSERT_KINDOF( CExtDockBar, pBar->m_pDockBar );
			CExtDynAutoHideArea * pWndAutoHideArea =
				((CExtDockBar*)pBar->m_pDockBar)->_GetAutoHideArea();
			ASSERT_VALID( pWndAutoHideArea );
			CExtDynAutoHideSlider * pWndSlider =
				pWndAutoHideArea->GetAutoHideSlider();
			ASSERT_VALID( pWndSlider );
			if( (pWndSlider->GetStyle()&WS_VISIBLE) != 0 )
				pWndSlider->SendMessage( WM_CANCELMODE );
			pWndAutoHideArea->RemoveControlBar( pBar, true );
		} // if( pBar->AutoHideModeGet() )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
		{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			if( pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynTabBar)) )
			{
				CExtDynTabControlBar * pTabbedBar =
					STATIC_DOWNCAST(
						CExtDynTabControlBar,
						pBar->m_pDockBar->GetParent()
						);
				LONG nIdx = pTabbedBar->FindControlBar( pBar );
				if( nIdx >= 0 )
				{
					LONG nSel = pTabbedBar->GetSwitcherSelection();
					if( nIdx != nSel )
						pTabbedBar->SetSwitcherSelection( nIdx );
					pTabbedBar->RemoveSelFromSwitcher();
				} // if( nIdx >= 0 )
			} // if( pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockDynTabBar)) )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			VERIFY(
				((CExtDockBar *)pBar->m_pDockBar)->
					RemoveControlBar( pBar, -1, 0, false )
				);
		}
		else
		{
			VERIFY( pBar->m_pDockBar->RemoveControlBar(pBar) );
		}
	} // else from if( pBar->IsFloating() )
	pDockSite->RemoveControlBar( pBar );
	pBar->m_pDockSite = NULL;
INT nPosInDockBar = pBar->m_pDockBar->FindBar( pBar );
	if( nPosInDockBar >= 0 )
	{
		pBar->m_pDockBar->m_arrBars.RemoveAt( nPosInDockBar );
		if(		nPosInDockBar > 1
			&&	pBar->m_pDockBar->m_arrBars[ nPosInDockBar - 1 ] == m_pDockSite
			)
			pBar->m_pDockBar->m_arrBars.RemoveAt( nPosInDockBar - 1 );
	}
	pBar->m_pDockBar = NULL;
	m_mapBars.RemoveKey( pBar );
POSITION posInList = m_listBars.Find( pBar );
	if( posInList != NULL )
		m_listBars.RemoveAt( posInList );
	posInList = m_listBarsInDocMode.Find( pBar );
	if( posInList != NULL )
		m_listBarsInDocMode.RemoveAt( posInList );
	if( ! bPersistentBar )
		OnDbsFreeBarCommandID( nCmdID );
	if( pMiniFrame != NULL )
		pMiniFrame->DestroyWindow();
	else
		pBar->DestroyWindow();
	if( ! bForceNoOptimizeMode )
		CExtDockBar::_OptimizeCircles( pDockSite );
}

void CExtDynamicBarSite::BarFreeAll(
	bool bFreePersistentBars, // = true
	bool bFreeDynamicBars // = true
	)
{
	ASSERT( this != NULL );
	_BarFreeAllImpl(
		false,
		bFreePersistentBars,
		bFreeDynamicBars
		);
}

void CExtDynamicBarSite::_BarFreeAllImpl(
	bool bForceNoOptimizeMode,
	bool bFreePersistentBars, // = true
	bool bFreeDynamicBars // = true
	)
{
	ASSERT( this != NULL );
CFrameWnd * pDockSite = DockSiteGet();
	if( pDockSite->GetSafeHwnd() == NULL )
	{
		ASSERT( BarEnumGetCount() == 0 );
		return;
	}
	ASSERT( pDockSite->GetSafeHwnd() != NULL );
	for( ; BarEnumGetCount(bFreePersistentBars,bFreeDynamicBars) > 0; )
	{
		POSITION pos = BarEnumGetStartPosition();
		ASSERT( pos != NULL );
		CExtDynamicControlBar * pBar =
			BarEnumGetNext( pos, bFreePersistentBars, bFreeDynamicBars );
		ASSERT_VALID( pBar );
		_BarFreeImpl( pBar, false, bForceNoOptimizeMode );
	} // for( ; BarEnumGetCount(bFreePersistentBars,bFreeDynamicBars) > 0; )
}

CExtDynamicControlBar * CExtDynamicBarSite::BarGetByCmdID( UINT nLookupCmdID )
{
	ASSERT( this != NULL );
POSITION pos = BarEnumGetStartPosition();
	for( ; pos != NULL; )
	{
		CExtDynamicControlBar * pBar =
			BarEnumGetNext( pos );
		ASSERT_VALID( pBar );
		UINT nCmdID = (UINT)pBar->GetDlgCtrlID();
		if( nCmdID == nLookupCmdID )
			return pBar;
	} // for( ; pos != NULL; )
	return NULL;
}

void CExtDynamicBarSite::BarEnumGetArray(
	CTypedPtrArray < CPtrArray, CExtDynamicControlBar * > & arrAllDynamicBars,
	bool bIncludePersistentBars, // = true
	bool bIncludeDynamicBars // = true
	)
{
	ASSERT( this != NULL );
	arrAllDynamicBars.RemoveAll();
	if( (!bIncludePersistentBars) && (!bIncludeDynamicBars) )
		return;
POSITION pos = BarEnumGetStartPosition();
	for( ; pos != NULL; )
	{
		CExtDynamicControlBar * pBar =
			BarEnumGetNext( pos );
		ASSERT_VALID( pBar );
		bool bPersistentBar = pBar->IsPersistentBar();
		if(		( bIncludePersistentBars && bPersistentBar )
			||	( bIncludeDynamicBars && (!bPersistentBar) )
			)
			arrAllDynamicBars.Add( pBar );
	} // for( ; pos != NULL; )
}

INT CExtDynamicBarSite::BarEnumGetCount(
	bool bIncludePersistentBars, // = true
	bool bIncludeDynamicBars // = true
	) const
{
	ASSERT( this != NULL );
	ASSERT( INT( m_mapBars.GetCount() ) == INT( m_listBars.GetCount() ) );
	if( bIncludePersistentBars && bIncludeDynamicBars )
		return INT( m_listBars.GetCount() );
POSITION pos = BarEnumGetStartPosition();
INT nCount = 0;
	for( ; pos != NULL; )
	{
		const CExtDynamicControlBar * pBar =
			BarEnumGetNext( pos );
		ASSERT_VALID( pBar );
		bool bPersistentBar = pBar->IsPersistentBar();
		if( bIncludePersistentBars && bPersistentBar )
			nCount ++;
		if( bIncludeDynamicBars && (!bPersistentBar) )
			nCount ++;
	} // for( ; pos != NULL; )
	return nCount;
}

POSITION CExtDynamicBarSite::BarEnumGetStartPosition() const
{
	ASSERT( this != NULL );
	return m_listBars.GetHeadPosition();
}

CExtDynamicControlBar * CExtDynamicBarSite::BarEnumGetNext(
	POSITION & pos,
	bool bIncludePersistentBars, // = true
	bool bIncludeDynamicBars // = true
	)
{
	ASSERT( this != NULL );
	for( ; pos != NULL; )
	{
		CExtDynamicControlBar * pBar = m_listBars.GetNext( pos );
		ASSERT_VALID( pBar );
		bool bPersistentBar = pBar->IsPersistentBar();
		if( bIncludePersistentBars && bPersistentBar )
			return pBar;
		if( bIncludeDynamicBars && (!bPersistentBar) )
			return pBar;
	} // for( ; pos != NULL; )
	return NULL;
}

const CExtDynamicControlBar * CExtDynamicBarSite::BarEnumGetNext(
	POSITION & pos,
	bool bIncludePersistentBars, // = true
	bool bIncludeDynamicBars // = true
	) const
{
	ASSERT( this != NULL );
	return
		( const_cast < CExtDynamicBarSite * > ( this ) ) ->
			BarEnumGetNext(
				pos,
				bIncludePersistentBars,
				bIncludeDynamicBars
				);
}


#endif // (!defined __EXT_MFC_NO_DYNAMIC_BAR_SITE)
