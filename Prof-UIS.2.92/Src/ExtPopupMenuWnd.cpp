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

#define _AFX_NO_OLE_SUPPORT

#ifndef __AFXPRIV_H__
	#include <AfxPriv.h>
#endif

#if _MFC_VER < 0x700
	#include <../src/AfxImpl.h>
#else
	#include <../src/mfc/AfxImpl.h>
#endif

#include <math.h>

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_MFC_NO_CUSTOMIZE) || (!defined __EXT_MFC_NO_DATE_PICKER_POPUP) || (!defined __EXT_MFC_NO_UNDO_REDO_POPUP) || (!defined __EXT_MFC_NO_GRIDBASEWND)
	#if (!defined __EXT_POPUP_CTRL_MENU_H)
		#include <ExtPopupCtrlMenu.h>
	#endif
#endif

#if (!defined __EXT_MENUCONTROLBAR_H)
	#include <ExtMenuControlBar.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_LOCALIZATION_H)
	#include <../Src/ExtLocalization.h>
#endif

#if (!defined __EXT_BUTTON_H)
	#include <ExtButton.h>
#endif

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	#if (!defined __EXT_RIBBON_BAR_H)
		#include <ExtRibbonBar.h>
	#endif // (!defined __EXT_RIBBON_BAR_H)
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

#if (!defined __EXT_SCROLLWND_H)
	#include <ExtScrollWnd.h>
#endif 

#include <Resources/Resource.h>

//#define _DEBUG_TEAROFF_TRANSPARENCY

#define __WM_START_RESIZING (WM_USER+98)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if (!defined __EXT_MFC_WINMMDLL_ALREADY_DYN_LIKNED)
	#define __EXT_MFC_WINMMDLL_ALREADY_DYN_LIKNED
	#pragma message("   Prof-UIS is automatically linking with winmm.lib")
	#pragma message("      (Windows Multimedia System)")
	#pragma comment( lib, "winmm.lib" ) 
#endif

#ifdef _DEBUG
	static int g_nPopupCount = 0;
	//#define __VPC_VERIFY_0 ASSERT( g_nPopupCount == 0 )
	#define __VPC_VERIFY_0 { }
	#define __VPC_INC g_nPopupCount++
	#define __VPC_DEC g_nPopupCount--
#else // _DEBUG
	#define __VPC_VERIFY_0 { }
	#define __VPC_INC { }
	#define __VPC_DEC { }
#endif // _DEBUG

#define ID_TIMER_ITEM_HIDE_DELAY 664

#define ID_TIMER_ANIMATION 666
#define ID_PERIOD_ANIMATION 30 // 20

#define ID_TIMER_ITEM_FOCUS_DELAY 667

#define ID_TIMER_SCROLLING 668
#define ID_PERIOD_SCROLLING 50 // 15

#define ID_TIMER_DELAY_SHOW 669
#define ID_PERIOD_DELAY_SHOW_QUICK 20
#define ID_PERIOD_DELAY_SHOW_NORMAL 500
#define ID_PERIOD_DELAY_SHOW_RIBBON 900

#define ID_TIMER_DELAY_EXPAND 670
#define ID_FREQ_DELAY_EXPAND 50
#define ID_TOTAL_DELAY_EXPAND 1500

#define ID_TIMER_DELAY_KILL 671
#define ID_PERIOD_DELAY_KILL 200

#define ID_TIMER_DELAY_PAINT 672

#define ID_TIMER_LIFE_STEP 673
#define ID_PERIOD_LIFE_STEP 200

#define ID_TIMER_FADE_OUT_ANIMATION 674

#define ID_TIMER_AUTO_EXPAND 675

#define __SCROLLING_PIXEL_STEP 5

static const int g_nAnimStepMetric = 20;
static clock_t g_nLastAnimTime = 0;

#define __ANIM_CY 15

#define __EXCLUDE_AREA_GAP_DX 3
#define __EXCLUDE_AREA_GAP_DY __EXCLUDE_AREA_GAP_DX

#define __BOX_ANIM_METRIC 32 // for __AT_BOXES,__AT_CIRCLES animation

CExtPopupMenuSite CExtPopupMenuSite::g_DefPopupMenuSite;

bool CExtPopupMenuWnd::g_bFullScreenMode = false; 

bool CExtPopupMenuWnd::g_bMenuWithShadows = true; // allow shadows
bool CExtPopupMenuWnd::g_bMenuExpanding = true; // allow hide rarely used
bool CExtPopupMenuWnd::g_bMenuDelayExpanding = true;  // show full menu after short delay
bool CExtPopupMenuWnd::g_bMenuHighlightRarely = true; // display rarely used in different style
bool CExtPopupMenuWnd::g_bMenuShowCoolTips = true; // allow display cool tooltips
bool CExtPopupMenuWnd::g_bMenuExpandAnimation = true; // allow animation when show rarely used
bool CExtPopupMenuWnd::g_bUseStretchOnExpandAnimation = false; // use StretchBlt() instead of BitBlt() for painting menu expand amimation
bool CExtPopupMenuWnd::g_bMenuLargeIcons = false; // use large icons
bool CExtPopupMenuWnd::g_bEatNcAreaClicks = true;
bool CExtPopupMenuWnd::g_bMRU_UseFullPathsInMenu = false;
bool CExtPopupMenuWnd::g_bMRU_UseFullPathsInTipTool = false;
bool CExtPopupMenuWnd::g_bShowStatusTipsOverDisabledItems = false;
bool CExtPopupMenuWnd::g_bShowCoolTipsOverDisabledItems = false;
CExtPopupMenuTipWnd::e_tip_style_t CExtPopupMenuWnd::g_eTtsClassicMenu = CExtPopupMenuTipWnd::__ETS_NONE; //__ETS_BALLOON;
CExtPopupMenuTipWnd::e_tip_style_t CExtPopupMenuWnd::g_eTtsPaletteMenu = CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON;
CExtPopupMenuTipWnd::e_tip_style_t CExtPopupMenuWnd::g_eTtsColorMenu = CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON;
CExtPopupMenuTipWnd::e_tip_style_t CExtPopupMenuWnd::g_eTtsTearOffCaption = CExtPopupMenuTipWnd::__ETS_INV_RECTANGLE_NO_ICON;

bool CExtPopupBaseWnd::g_bUseDesktopWorkArea = true; // align to desktop work area (false - to screen area)

bool CExtPopupBaseWnd::g_bEnableOnIdleCalls = false;

CObject * CExtPopupBaseWnd::g_pCombinedHelper = NULL;

CExtPopupBaseWnd::e_animation_type_t
	CExtPopupBaseWnd::g_DefAnimationType =
		CExtPopupBaseWnd::__AT_NONE; // __AT_FADE; // __AT_RANDOM

const UINT CExtPopupBaseWnd::g_nMsgPrepareMenu =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgPrepareMenu")
		);
const UINT CExtPopupBaseWnd::g_nMsgPrepareOneMenuLevel =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgPrepareOneMenuLevel")
		);
const UINT CExtPopupBaseWnd::g_nMsgPrepareDesktopBk =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgPrepareDesktopBk")
		);
const UINT CExtPopupBaseWnd::g_nMsgPopupNext =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgPopupNext")
		);
const UINT CExtPopupBaseWnd::g_nMsgPopupPrev =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgPopupPrev")
		);
const UINT CExtPopupBaseWnd::g_nMsgNotifyMenuExpanded =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgNotifyMenuExpanded")
		);
const UINT CExtPopupBaseWnd::g_nMsgNotifyMenuClosed =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgNotifyMenuClosed")
		);
const UINT CExtPopupBaseWnd::g_nMsgPopupDrawItem =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgPopupDrawItem")
		);
const UINT CExtPopupBaseWnd::g_nMsgPopupDrawLeftArea =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgPopupDrawLeftArea")
		);
const UINT CExtPopupBaseWnd::g_nMsgPopupDrawBackground =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgPopupDrawBackground")
		);
const UINT CExtPopupBaseWnd::g_nMsgCreateInplaceEdit =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgCreateInplaceEdit")
		);

const UINT CExtPopupBaseWnd::g_nMsgTranslateKeyboardEvent =
	::RegisterWindowMessage(
		_T("CExtPopupBaseWnd::g_nMsgTranslateKeyboardEvent")
		);
const UINT CExtPopupBaseWnd::g_nMsgTranslateMouseClickEvent =
	::RegisterWindowMessage(
		_T("CExtPopupBaseWnd::g_nMsgTranslateMouseClickEvent")
		);
const UINT CExtPopupBaseWnd::g_nMsgTranslateMouseMoveEvent =
	::RegisterWindowMessage(
		_T("CExtPopupBaseWnd::g_nMsgTranslateMouseMoveEvent")
		);
const UINT CExtPopupBaseWnd::g_nMsgTranslateMouseWheelEvent =
	::RegisterWindowMessage(
		_T("CExtPopupBaseWnd::g_nMsgTranslateMouseWheelEvent")
		);

#define __POPUP_WNDCLASS_STYLES__ \
	( /*CS_OWNDC|*/ CS_GLOBALCLASS|CS_HREDRAW|CS_VREDRAW|CS_SAVEBITS )

#define __TIP_WNDCLASS_STYLES__ \
	( CS_OWNDC|CS_GLOBALCLASS|CS_HREDRAW|CS_VREDRAW|CS_SAVEBITS )

#define __SHADOW_WNDCLASS_STYLES__ \
	( CS_OWNDC|CS_GLOBALCLASS|CS_HREDRAW|CS_VREDRAW|CS_SAVEBITS )

CExtPopupBaseWnd::TranslateKeyboardEventData_t::TranslateKeyboardEventData_t(
	CExtPopupBaseWnd * pEventSourcePopup,
	UINT nChar,
	UINT nRepCnt,
	UINT nFlags,
	bool bNoEat
	)
	: m_pEventSourcePopup( pEventSourcePopup )
	, m_bMessageHandled( false )
	, m_bNoEat( bNoEat )
	, m_nChar( nChar )
	, m_nRepCnt( nRepCnt )
	, m_nFlags( nFlags )
{
	ASSERT_VALID( m_pEventSourcePopup );
	ASSERT( m_pEventSourcePopup->GetSafeHwnd() != NULL );
}

CExtPopupBaseWnd::TranslateKeyboardEventData_t::operator WPARAM ()
{
	ASSERT( this != NULL );
WPARAM wParam =
		reinterpret_cast
		< WPARAM >
		( this );
	return wParam;
}

CExtPopupBaseWnd::TranslateKeyboardEventData_t & CExtPopupBaseWnd::TranslateKeyboardEventData_t::FromWPARAM( WPARAM wParam )
{
CExtPopupBaseWnd::TranslateKeyboardEventData_t * pData =
		reinterpret_cast
		< CExtPopupBaseWnd::TranslateKeyboardEventData_t * >
		( wParam );
	ASSERT( pData != NULL );
	return (*pData);
}

bool CExtPopupBaseWnd::TranslateKeyboardEventData_t::Notify( HWND hWnd )
{
	ASSERT( this != NULL );
	if( hWnd == NULL || (! ::IsWindow(hWnd) ) )
		return false;
	m_bMessageHandled = false;
WPARAM wParam = (*this);
	::SendMessage( hWnd, CExtPopupBaseWnd::g_nMsgTranslateKeyboardEvent, wParam, 0L );
	return m_bMessageHandled;
}

bool CExtPopupBaseWnd::TranslateKeyboardEventData_t::Notify()
{
	ASSERT( this != NULL );
	ASSERT_VALID( m_pEventSourcePopup );
	ASSERT( m_pEventSourcePopup->GetSafeHwnd() != NULL );
CExtPopupMenuWnd * pPopup =
		DYNAMIC_DOWNCAST(
			CExtPopupMenuWnd,
			m_pEventSourcePopup
			);
	if( pPopup == NULL )
		return false;
HWND hWnd = pPopup->GetCmdReceiverHWND();
bool bRetVal = Notify( hWnd );
	return bRetVal;
}

CExtPopupBaseWnd::TranslateMouseClickEventData_t::TranslateMouseClickEventData_t(
	CExtPopupBaseWnd * pEventSourcePopup,
	UINT nFlags,
	CPoint point,
	bool bNoEat
	)
	: m_pEventSourcePopup( pEventSourcePopup )
	, m_bMessageHandled( false )
	, m_bNoEat( bNoEat )
	, m_nFlags( nFlags )
	, m_point( point )
{
	ASSERT_VALID( m_pEventSourcePopup );
	ASSERT( m_pEventSourcePopup->GetSafeHwnd() != NULL );
}

CExtPopupBaseWnd::TranslateMouseClickEventData_t::operator WPARAM ()
{
	ASSERT( this != NULL );
WPARAM wParam =
		reinterpret_cast
		< WPARAM >
		( this );
	return wParam;
}

CExtPopupBaseWnd::TranslateMouseClickEventData_t & CExtPopupBaseWnd::TranslateMouseClickEventData_t::FromWPARAM( WPARAM wParam )
{
CExtPopupBaseWnd::TranslateMouseClickEventData_t * pData =
		reinterpret_cast
		< CExtPopupBaseWnd::TranslateMouseClickEventData_t * >
		( wParam );
	ASSERT( pData != NULL );
	return (*pData);
}

bool CExtPopupBaseWnd::TranslateMouseClickEventData_t::Notify( HWND hWnd )
{
	ASSERT( this != NULL );
	if( hWnd == NULL || (! ::IsWindow(hWnd) ) )
		return false;
	m_bMessageHandled = false;
WPARAM wParam = (*this);
	::SendMessage( hWnd, CExtPopupBaseWnd::g_nMsgTranslateMouseClickEvent, wParam, 0L );
	return m_bMessageHandled;
}

bool CExtPopupBaseWnd::TranslateMouseClickEventData_t::Notify()
{
	ASSERT( this != NULL );
	ASSERT_VALID( m_pEventSourcePopup );
	ASSERT( m_pEventSourcePopup->GetSafeHwnd() != NULL );
CExtPopupMenuWnd * pPopup =
		DYNAMIC_DOWNCAST(
			CExtPopupMenuWnd,
			m_pEventSourcePopup
			);
	if( pPopup == NULL )
		return false;
HWND hWnd = pPopup->GetCmdReceiverHWND();
bool bRetVal = Notify( hWnd );
	return bRetVal;
}

CExtPopupBaseWnd::TranslateMouseMoveEventData_t::TranslateMouseMoveEventData_t(
	CExtPopupBaseWnd * pEventSourcePopup,
	UINT nFlags,
	CPoint point,
	bool bNoEat
	)
	: m_pEventSourcePopup( pEventSourcePopup )
	, m_bMessageHandled( false )
	, m_bNoEat( bNoEat )
	, m_nFlags( nFlags )
	, m_point( point )
{
	ASSERT_VALID( m_pEventSourcePopup );
	ASSERT( m_pEventSourcePopup->GetSafeHwnd() != NULL );
}

CExtPopupBaseWnd::TranslateMouseMoveEventData_t::operator WPARAM ()
{
	ASSERT( this != NULL );
WPARAM wParam =
		reinterpret_cast
		< WPARAM >
		( this );
	return wParam;
}

CExtPopupBaseWnd::TranslateMouseMoveEventData_t & CExtPopupBaseWnd::TranslateMouseMoveEventData_t::FromWPARAM( WPARAM wParam )
{
CExtPopupBaseWnd::TranslateMouseMoveEventData_t * pData =
		reinterpret_cast
		< CExtPopupBaseWnd::TranslateMouseMoveEventData_t * >
		( wParam );
	ASSERT( pData != NULL );
	return (*pData);
}

bool CExtPopupBaseWnd::TranslateMouseMoveEventData_t::Notify( HWND hWnd )
{
	ASSERT( this != NULL );
	if( hWnd == NULL || (! ::IsWindow(hWnd) ) )
		return false;
	m_bMessageHandled = false;
WPARAM wParam = (*this);
	::SendMessage( hWnd, CExtPopupBaseWnd::g_nMsgTranslateMouseMoveEvent, wParam, 0L );
	return m_bMessageHandled;
}

bool CExtPopupBaseWnd::TranslateMouseMoveEventData_t::Notify()
{
	ASSERT( this != NULL );
	ASSERT_VALID( m_pEventSourcePopup );
	ASSERT( m_pEventSourcePopup->GetSafeHwnd() != NULL );
CExtPopupMenuWnd * pPopup =
		DYNAMIC_DOWNCAST(
			CExtPopupMenuWnd,
			m_pEventSourcePopup
			);
	if( pPopup == NULL )
		return false;
HWND hWnd = pPopup->GetCmdReceiverHWND();
bool bRetVal = Notify( hWnd );
	return bRetVal;
}

CExtPopupBaseWnd::TranslateMouseWheelEventData_t::TranslateMouseWheelEventData_t(
	CExtPopupBaseWnd * pEventSourcePopup,
	WPARAM wParam,
	LPARAM lParam,
	bool bNoEat
	)
	: m_pEventSourcePopup( pEventSourcePopup )
	, m_bMessageHandled( false )
	, m_bNoEat( bNoEat )
	, m_wParam( wParam )
	, m_lParam( lParam )
{
	ASSERT_VALID( m_pEventSourcePopup );
	ASSERT( m_pEventSourcePopup->GetSafeHwnd() != NULL );
}

CExtPopupBaseWnd::TranslateMouseWheelEventData_t::operator WPARAM ()
{
	ASSERT( this != NULL );
WPARAM wParam =
		reinterpret_cast
		< WPARAM >
		( this );
	return wParam;
}

CExtPopupBaseWnd::TranslateMouseWheelEventData_t & CExtPopupBaseWnd::TranslateMouseWheelEventData_t::FromWPARAM( WPARAM wParam )
{
CExtPopupBaseWnd::TranslateMouseWheelEventData_t * pData =
		reinterpret_cast
		< CExtPopupBaseWnd::TranslateMouseWheelEventData_t * >
		( wParam );
	ASSERT( pData != NULL );
	return (*pData);
}

bool CExtPopupBaseWnd::TranslateMouseWheelEventData_t::Notify( HWND hWnd )
{
	ASSERT( this != NULL );
	if( hWnd == NULL || (! ::IsWindow(hWnd) ) )
		return false;
	m_bMessageHandled = false;
WPARAM wParam = (*this);
	::SendMessage( hWnd, CExtPopupBaseWnd::g_nMsgTranslateMouseWheelEvent, wParam, 0L );
	return m_bMessageHandled;
}

bool CExtPopupBaseWnd::TranslateMouseWheelEventData_t::Notify()
{
	ASSERT( this != NULL );
	ASSERT_VALID( m_pEventSourcePopup );
	ASSERT( m_pEventSourcePopup->GetSafeHwnd() != NULL );
CExtPopupMenuWnd * pPopup =
		DYNAMIC_DOWNCAST(
			CExtPopupMenuWnd,
			m_pEventSourcePopup
			);
	if( pPopup == NULL )
		return false;
HWND hWnd = pPopup->GetCmdReceiverHWND();
bool bRetVal = Notify( hWnd );
	return bRetVal;
}

/////////////////////////////////////////////////////////////////////////////
// CExtSoundPlayer

CExtSoundPlayer::CExtSoundPlayerAutoPtr g_SoundPlayer;

IMPLEMENT_DYNCREATE( CExtSoundPlayer, CObject );

CExtSoundPlayer::InternalSoundEventInitDone_t::InternalSoundEventInitDone_t()
	: CEvent( FALSE, TRUE )
{
}

void CExtSoundPlayer::InternalSoundEventInitDone_t::SetInitDone()
{
	SetEvent();
}

void CExtSoundPlayer::InternalSoundEventInitDone_t::WaitInitDone()
{
HANDLE hEvent = *this;
	ASSERT( hEvent != NULL );
	::WaitForSingleObject( hEvent, INFINITE );
}

CExtSoundPlayer::InternalSoundThredadParms_t::InternalSoundThredadParms_t(
	__EXT_MFC_SAFE_LPCTSTR sSoundSpecBuffer,
	HANDLE hModule,
	DWORD dwPlayerFlags
	)
	: m_hModule( hModule )
	, m_dwPlayerFlags( dwPlayerFlags )
	, m_hThread( NULL )
	, m_dwThreadID( 0 )
	, m_pEventInitDone( NULL )
{
	::memset( m_sSoundSpecBuffer, 0, sizeof(m_sSoundSpecBuffer) );
	ASSERT( sSoundSpecBuffer != NULL );
	ASSERT( _tclen(sSoundSpecBuffer) > 0 );
	__EXT_MFC_STRNCPY(
		m_sSoundSpecBuffer,
		sizeof(m_sSoundSpecBuffer) / sizeof(m_sSoundSpecBuffer[0]),
		sSoundSpecBuffer,
		sizeof(m_sSoundSpecBuffer) / sizeof(m_sSoundSpecBuffer[0])
		);
}

CEvent CExtSoundPlayer::InternalSoundThredadParms_t::g_EventPlaySnd(
	TRUE,
	FALSE
	);

CCriticalSection CExtSoundPlayer::InternalSoundThredadParms_t::g_CsPlaySnd;

DWORD WINAPI CExtSoundPlayer::InternalSoundThredadParms_t::stat_ParallelPlayerProc(
	CExtSoundPlayer::InternalSoundThredadParms_t * pParms
	)
{
	ASSERT( pParms != NULL );
	pParms->PlaySound();
	g_EventPlaySnd.SetEvent();
	return 0;
}

//#define __TRACE_SOUND_PLAYER_THREAD

void CExtSoundPlayer::InternalSoundThredadParms_t::PlaySound()
{
TCHAR sSoundSpecBuffer[ sizeof(m_sSoundSpecBuffer) / sizeof(m_sSoundSpecBuffer[0]) ];
	__EXT_MFC_STRCPY(
		sSoundSpecBuffer,
		sizeof(m_sSoundSpecBuffer) / sizeof(m_sSoundSpecBuffer[0]),
		m_sSoundSpecBuffer
		);
HANDLE hModule = m_hModule;
DWORD dwPlayerFlags = m_dwPlayerFlags;
	if( m_pEventInitDone != NULL )
		m_pEventInitDone->SetInitDone();
#ifdef __TRACE_SOUND_PLAYER_THREAD
	TRACE1("CExtSoundPlayer::InternalSoundThredadParms_t::PlaySound(\"%s\") - START\n",sSoundSpecBuffer);
#endif // __TRACE_SOUND_PLAYER_THREAD
	if( (m_dwPlayerFlags & SND_ASYNC) != 0 )
		::PlaySound( NULL, NULL, SND_PURGE );
	::PlaySound(
		sSoundSpecBuffer,
		(HMODULE)hModule,
		dwPlayerFlags
		);
	if( (m_dwPlayerFlags & SND_SYNC) != 0 )
		::PlaySound( NULL, NULL, SND_PURGE );
#ifdef __TRACE_SOUND_PLAYER_THREAD
	TRACE1("CExtSoundPlayer::InternalSoundThredadParms_t::PlaySound(\"%s\") - STOP\n",sSoundSpecBuffer);
#endif // __TRACE_SOUND_PLAYER_THREAD
}

void CExtSoundPlayer::InternalSoundThredadParms_t::PlaySoundParallel()
{
	ASSERT( m_hThread == NULL );
CSingleLock slCs( &g_CsPlaySnd );
	slCs.Lock();
HANDLE hEvent = (HANDLE)g_EventPlaySnd;
	ASSERT( hEvent != NULL );
	if( ::WaitForSingleObject(hEvent,0) == WAIT_OBJECT_0 )
	{
		ASSERT( m_pEventInitDone == NULL );
		InternalSoundEventInitDone_t EventInitDone;
		m_pEventInitDone = &EventInitDone;
		m_hThread =
			::CreateThread(
				NULL,
				0,
				*this,
				this,
				CREATE_SUSPENDED,
				&m_dwThreadID
				);
		ASSERT( m_hThread != NULL );
		if( m_hThread != NULL )
		{
			VERIFY(
				::SetThreadPriority(
					m_hThread,
					THREAD_PRIORITY_BELOW_NORMAL
					)
				);
			::ResumeThread( m_hThread );
			EventInitDone.WaitInitDone();
		} // if( m_hThread != NULL )
		else
			g_EventPlaySnd.SetEvent();
	} // if( ::WaitForSingleObject(hEvent,0) == WAIT_OBJECT_0 )
	if( m_hThread != NULL )
		::CloseHandle( m_hThread );
	slCs.Unlock();
}

CExtSoundPlayer::CExtSoundPlayerAutoPtr::CExtSoundPlayerAutoPtr()
{
	m_pPlayer = new CExtSoundPlayer;
}

CExtSoundPlayer::CExtSoundPlayerAutoPtr::~CExtSoundPlayerAutoPtr()
{
	if( m_pPlayer != NULL )
		delete m_pPlayer;
}

void CExtSoundPlayer::CExtSoundPlayerAutoPtr::InstallSoundPlayer(
	CExtSoundPlayer * pPlayer
	)
{
	ASSERT( pPlayer != NULL );
	ASSERT_VALID( pPlayer );
	ASSERT_KINDOF( CExtSoundPlayer, m_pPlayer );
	if( m_pPlayer != NULL )
		delete m_pPlayer;
	m_pPlayer = pPlayer;
}

CExtSoundPlayer::CExtSoundPlayer()
{
}

CExtSoundPlayer::~CExtSoundPlayer()
{
}

void CExtSoundPlayer::PlaySound(
	CExtSoundPlayer::e_ui_sounds_t eSoundID
	)
{
	ASSERT_VALID( this );
	switch( eSoundID )
	{
	case __NO_SOUND:
		return;
	case __ON_MENU_EXPAND_CLICKED:
	case __ON_MENU_POPUP_DISPLAYED:
		{
			InternalSoundThredadParms_t _istp(
				_T("MenuPopup"),
				NULL,
				SND_ALIAS|SND_NODEFAULT|SND_NOWAIT|SND_ASYNC
				);
			_istp.PlaySoundParallel();
			return;
		}
	case __ON_MENU_CMD_CLICKED:
		{
			InternalSoundThredadParms_t _istp(
				_T("MenuCommand"),
				NULL,
				SND_ALIAS|SND_NODEFAULT|SND_NOWAIT|SND_SYNC
				);
			_istp.PlaySoundParallel();
			return;
		}
	} // switch( eSoundID )
}

/////////////////////////////////////////////////////////////////////////////
// CExtPopupMenuSite

CExtPopupMenuSite::CExtPopupMenuSite()
	: m_pTopMenu( NULL )
	, m_pWndCapture( NULL )
	, m_pWndAnimation( NULL )
	, m_bShutdownMode( false )
	, m_hMouseHook( NULL )
	, m_hKeyboardHook( NULL )
	, m_lpnResultCmdID( NULL )
	, m_pWndToolTip( NULL )
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	, m_pWndScreenTip( NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
{
}

CExtPopupMenuSite::~CExtPopupMenuSite()
{
	__PROF_UIS_MANAGE_STATE;
	_Done();
	for( ; true; )
	{
		POSITION pos = m_mapFadeOutInsances.GetStartPosition();
		if( pos == NULL )
			break;
		CExtPopupMenuWnd * pPopup = NULL;
		bool bFlag = false;
		m_mapFadeOutInsances.GetNextAssoc( pos, pPopup, bFlag );
		ASSERT_VALID( pPopup );
		pPopup->m_nFadeOutAnimationStepIndex = pPopup->m_nFadeOutAnimationStepCount;
		if( pPopup->GetSafeHwnd() == NULL )
		{
			//pPopup->m_bTopLevel = true;
			pPopup->_BuildItems( NULL, true );
			delete pPopup;
		}
		else
		{
			pPopup->TrackFlagsSet( pPopup->TrackFlagsGet() | TPMX_NO_FADE_OUT_ANIMATION );
			pPopup->DestroyWindow();
		}
	} // for( ; true; )
	if( m_pWndToolTip != NULL )
	{
		if( m_pWndToolTip->GetSafeHwnd() != NULL )
			m_pWndToolTip->DestroyWindow();
		delete m_pWndToolTip;
		m_pWndToolTip = NULL;
	} // if( m_pWndToolTip != NULL )
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( m_pWndScreenTip != NULL )
	{
		if( m_pWndScreenTip->GetSafeHwnd() != NULL )
			m_pWndScreenTip->DestroyWindow();
		delete m_pWndScreenTip;
		m_pWndScreenTip = NULL;
	} // if( m_pWndScreenTip != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
}

void CExtPopupMenuSite::FadeOutInstanceAdd( CExtPopupMenuWnd * pPopup )
{
	ASSERT( pPopup != NULL );
	m_mapFadeOutInsances.SetAt( pPopup, false );
}

void CExtPopupMenuSite::FadeOutInstanceRemove( CExtPopupMenuWnd * pPopup )
{
	ASSERT( pPopup != NULL );
	m_mapFadeOutInsances.RemoveKey( pPopup );
}

bool CExtPopupMenuSite::IsEmpty() const
{
	return (m_pTopMenu == NULL) ? true : false;
}

bool CExtPopupMenuSite::IsShutdownMode() const
{
	return m_bShutdownMode;
}

CExtPopupMenuTipWnd & CExtPopupMenuSite::GetTip()
{
	if( m_pWndToolTip == NULL )
		m_pWndToolTip = new CExtPopupMenuTipWnd;
	return (*m_pWndToolTip);
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtPopupScreenTipWnd & CExtPopupMenuSite::GetScreenTip()
{
	if( m_pWndScreenTip == NULL )
		m_pWndScreenTip = new CExtPopupScreenTipWnd;
	return (*m_pWndScreenTip);
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtPopupMenuWnd * CExtPopupMenuSite::GetCapture()
{
	if( IsEmpty() )
		return NULL;
	ASSERT( ! IsShutdownMode() );
	return m_pWndCapture;
}

void CExtPopupMenuSite::SetCapture(
	CExtPopupMenuWnd * pNewPopup // = NULL
	)
{
	if( IsEmpty() )
		return;
	ASSERT( ! IsShutdownMode() );
#ifdef _DEBUG
	if( pNewPopup != NULL )
	{
		ASSERT_VALID( pNewPopup );
	} // if( pNewPopup != NULL )
#endif // _DEBUG
	m_pWndCapture = pNewPopup;
}

void CExtPopupMenuSite::SetInstance(
	CExtPopupMenuWnd * pTopMenu // = NULL
	)
{
	ASSERT( ! IsShutdownMode() );
	if( ! IsEmpty() )
	{
		ASSERT( m_pTopMenu != NULL );
		ASSERT_VALID( m_pTopMenu );
		ASSERT( m_pTopMenu->m_bTopLevel );
		if( m_pTopMenu == pTopMenu )
			return;
		if( m_pTopMenu != pTopMenu )
			_Done();
	} // if( ! IsEmpty() )
	ASSERT( m_pTopMenu == NULL );
	if( pTopMenu != NULL )
	{
		ASSERT_VALID( pTopMenu );
		ASSERT( pTopMenu->m_bTopLevel );
		m_pTopMenu = pTopMenu;
		_Hook( true );
	} // if( pTopMenu != NULL )
}

CExtPopupMenuWnd * CExtPopupMenuSite::GetAnimated()
{
	if( IsEmpty() )
		return NULL;
	ASSERT( ! IsShutdownMode() );
	return m_pWndAnimation;
}

void CExtPopupMenuSite::SetAnimated(
	CExtPopupMenuWnd * pNewPopup // = NULL
	)
{
	if( IsEmpty() )
		return;
	ASSERT( ! IsShutdownMode() );
	m_pWndAnimation = pNewPopup;
}

CExtPopupMenuWnd * CExtPopupMenuSite::GetInstance()
{
	return m_pTopMenu;
}

void CExtPopupMenuSite::SetTargetCmdIdPtr(
	UINT * lpnResultCmdID // = NULL
	)
{
	m_lpnResultCmdID = lpnResultCmdID;
}

UINT * CExtPopupMenuSite::GetTargetCmdIdPtr()
{
	return m_lpnResultCmdID;
}

void CExtPopupMenuSite::DoneInstance()
{
	_Done();
}

void CExtPopupMenuSite :: operator = ( CExtPopupMenuWnd * pTopMenu )
{
	SetInstance(pTopMenu);
}

CExtPopupMenuWnd * CExtPopupMenuSite :: operator -> ()
{
	ASSERT( ! IsEmpty() );
	ASSERT( ! IsShutdownMode() );
	ASSERT( m_pTopMenu != NULL );
	return m_pTopMenu;
}

bool CExtPopupMenuSite::IsTopPopup(
	CExtPopupMenuWnd * pTopMenu
	) const
{
	if( pTopMenu->GetSafeHwnd() == NULL )
		return false;
	ASSERT_VALID( pTopMenu );
	if( IsEmpty() )
	{
		if( (pTopMenu->TrackFlagsGet()&TPMX_NO_SITE) != 0 )
			return true;
	}
	ASSERT( ! IsShutdownMode() );
	return (pTopMenu==m_pTopMenu) ? true : false;
}

void CExtPopupMenuSite::_Done()
{
	if( CExtBarButton::g_pTrackingMenuTBB != NULL )
	{
		bool bResetCurrentToolbarButton = true;
		CExtPopupMenuWnd * pPopup = GetInstance();
		if(		pPopup != NULL
			&&	pPopup->m_eCombineAlign == CExtPopupMenuWnd::__CMBA_NONE
			)
		{
			POINT pt;
			if( ::GetCursorPos( &pt ) )
			{
				HWND hWnd = ::WindowFromPoint( pt );
				if( hWnd != NULL )
				{
					CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
					if( pWnd != NULL )
					{
						CExtToolControlBar * pToolBar = DYNAMIC_DOWNCAST( CExtToolControlBar, pWnd );
						if( pToolBar != NULL )
						{
							pToolBar->ScreenToClient( &pt );
							int nIdx = pToolBar->HitTest( pt );
							if( nIdx >= 0 && pToolBar->GetButton( nIdx ) == CExtBarButton::g_pTrackingMenuTBB )
								bResetCurrentToolbarButton = false;
						}
					}
				}
			}
		}
		if( bResetCurrentToolbarButton )
			CExtBarButton::g_pTrackingMenuTBB = NULL;
	}
	if( m_pWndToolTip != NULL )
	{
		if( m_pWndToolTip->GetSafeHwnd() != NULL )
			m_pWndToolTip->DestroyWindow();
		delete m_pWndToolTip;
		m_pWndToolTip = NULL;
	} // if( m_pWndToolTip != NULL )
	if( m_bShutdownMode )
		return;
//	if( m_pWndToolTip != NULL )
//		m_pWndToolTip->Hide();
	m_bShutdownMode = true;
	m_pWndCapture = NULL;
	m_pWndAnimation = NULL;
	m_lpnResultCmdID = NULL;
	_Hook( false );
	if( m_pTopMenu != NULL )
	{
		ASSERT( m_pTopMenu->m_bTopLevel );
		bool bFadeOutMode = m_pTopMenu->_IsFadeOutAnimation();
		if( ! bFadeOutMode )
		{
			VERIFY( m_pTopMenu->_BuildItems( NULL, true ) );
			if( m_pTopMenu->GetSafeHwnd() != NULL )
			{
				CExtControlBar::stat_SetMessageString( m_pTopMenu->GetOwner() );
				m_pTopMenu->DestroyWindow();
			}
			ASSERT( m_pTopMenu->GetSafeHwnd() == NULL );
			delete m_pTopMenu;
		} // if( ! bFadeOutMode )
		m_pTopMenu = NULL;
	} // if( m_pTopMenu != NULL )
	__VPC_VERIFY_0;
	m_bShutdownMode = false;
}

LRESULT CALLBACK CExtPopupMenuSite::_HookMouseProc(
	int nCode,      // hook code
	WPARAM wParam,  // message identifier
	LPARAM lParam   // mouse coordinates
	)
{
	__PROF_UIS_MANAGE_STATE;

	if( nCode != HC_ACTION )
		return
			::CallNextHookEx(
				g_DefPopupMenuSite.m_hMouseHook,
				nCode,
				wParam,
				lParam
				);

MOUSEHOOKSTRUCT* lpMS = (MOUSEHOOKSTRUCT*)lParam;
	ASSERT( lpMS != NULL );

	if( CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON != NULL )
	{
		switch( wParam )
		{
		case WM_MOUSEMOVE:
		{
			CPoint pt( lpMS->pt );
			if(	CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->
					_OnMouseMove(
						(UINT)wParam,
						pt
						)
				)
				return 1; // eat!
		}
		break;

		case WM_MOUSEWHEEL:
		{
			if(	CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->
					_OnMouseWheel(
						wParam,
						lParam
						)
				)
				return 1; // eat!
		}
		break;

		case WM_NCLBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
		case WM_NCMBUTTONDOWN:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:

		case WM_NCLBUTTONUP:
		case WM_NCRBUTTONUP:
		case WM_NCMBUTTONUP:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		{
			CPoint ptSrc( lpMS->pt );
			if(	CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->_OnMouseClick(
					(UINT)wParam,
					ptSrc
					)
				)
				return 1; // eat!
		}
		break;
		} // switch( wParam )
	}

	if(		nCode == HC_ACTION // +2.55 - fix by Ulrich Berger-Kubik
		&&	(! g_DefPopupMenuSite.IsEmpty() )
		&&	(! g_DefPopupMenuSite.IsShutdownMode() )
		)
	{
		CExtPopupMenuWnd * pWndCapture =
			g_DefPopupMenuSite.GetCapture();
		if( pWndCapture->GetSafeHwnd() != NULL )
		{
			ASSERT_VALID( pWndCapture );
			CExtBarButton * pSavedTrackingMenuTBB = CExtBarButton::g_pTrackingMenuTBB;
			switch( wParam )
			{
			//case WM_MOUSEACTIVATE:
			//	if( lpMS->hwnd != pWndCapture->GetSafeHwnd() )
			//		return 1;
			//break;

			case WM_MOUSEMOVE:
			{
				CPoint pt( lpMS->pt );
				pWndCapture->ScreenToClient( &pt );
				bool bNoEat = false;
				if(	pWndCapture->
						_OnMouseMove(
							(UINT)wParam,
							pt,
							bNoEat
							)
					)
				{
					HWND hWnd = ::WindowFromPoint( lpMS->pt );
					if(		hWnd != NULL
						&&	::GetWindowThreadProcessId( hWnd, NULL ) == ::GetCurrentThreadId()
						)
					{
						INT nHT = (INT)::SendMessage( hWnd, WM_NCHITTEST, 0L, MAKELPARAM(lpMS->pt.x,lpMS->pt.y) );
						INT nMM = WM_MOUSEMOVE;
						if(		nHT != HTCLIENT
							&&	nHT != HTMINBUTTON
							&&	nHT != HTMAXBUTTON
							&&	nHT != HTHELP
							&&	nHT != HTCLOSE
							&&	nHT != HTNOWHERE
							)
							nMM = WM_NCMOUSEMOVE;
						else
							nHT = HTCLIENT;
						::SendMessage( hWnd, WM_SETCURSOR, (WPARAM)hWnd, MAKELPARAM(nHT,nMM) );
					}
					return 1; // eat!
				}
				if( bNoEat )
					return
						::CallNextHookEx(
							g_DefPopupMenuSite.m_hMouseHook,
							nCode,
							wParam,
							lParam
							);
			}
			break;

			case WM_MOUSEWHEEL:
			{
				bool bNoEat = false;
				if(	pWndCapture->
						_OnMouseWheel(
							wParam,
							lParam,
							bNoEat
							)
					)
					return 1; // eat!
				if( bNoEat )
					return
						::CallNextHookEx(
							g_DefPopupMenuSite.m_hMouseHook,
							nCode,
							wParam,
							lParam
							);
				return 1; // eat!
			}
			break;

			case WM_NCLBUTTONDBLCLK:
			case WM_NCRBUTTONDBLCLK:
			case WM_NCMBUTTONDBLCLK:
			case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDBLCLK:
				CExtBarButton::g_pTrackingMenuTBB = NULL;
				// continue ...

			case WM_NCLBUTTONDOWN:
			case WM_NCRBUTTONDOWN:
			case WM_NCMBUTTONDOWN:
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:

			case WM_NCLBUTTONUP:
			case WM_NCRBUTTONUP:
			case WM_NCMBUTTONUP:
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
			{
				HWND hWndFromPoint = WindowFromPoint( lpMS->pt );
				LRESULT nHT =
					::SendMessage(
						hWndFromPoint,
						WM_NCHITTEST,
						0L,
						MAKELPARAM( lpMS->pt.x, lpMS->pt.y )
						);
				if( hWndFromPoint != NULL )
				{
					if(		( nHT == HTSYSMENU || nHT == HTCAPTION )
						&&	wParam != WM_NCLBUTTONUP
						&&	( ! g_PaintManager.m_DWM.IsCompositionEnabled() )
						)
					{
						CWnd * pWndFromPointPermanent = CWnd::FromHandlePermanent( hWndFromPoint );
						if(		pWndFromPointPermanent == NULL
							||	(! pWndFromPointPermanent->IsKindOf( RUNTIME_CLASS(CMiniFrameWnd) ) )
							)
						{
							pWndCapture->TrackFlagsSet( pWndCapture->TrackFlagsGet() | TPMX_NO_FADE_OUT_ANIMATION );
							CExtPopupMenuWnd * pTLP = pWndCapture->GetTopLevelMenuWnd();
							if( pTLP != NULL && pTLP != pWndCapture )
								pTLP->TrackFlagsSet( pTLP->TrackFlagsGet() | TPMX_NO_FADE_OUT_ANIMATION );
							CExtToolControlBar::_CloseTrackingMenus();
							CExtPopupMenuWnd::CancelMenuTracking();
							return
								::CallNextHookEx(
									g_DefPopupMenuSite.m_hMouseHook,
									nCode,
									wParam,
									lParam
									);
						}
					}
				} // if( hWndFromPoint != NULL )

				//CRect rcWndCapture;
				//_pWndCapture->GetWindowRect( &rcWndCapture );
				//if( !rcWndCapture.PtInRect(lpMS->pt) )
				//	break;
				CPoint ptSrc( lpMS->pt );
				pWndCapture->ScreenToClient( &ptSrc );

				bool bSameToolBarButtonIsClicked = false;
				CExtPopupMenuWnd * pDefInstancePopup = CExtPopupMenuSite::g_DefPopupMenuSite.GetInstance();
				CPoint pt( lpMS->pt );
				HWND hWndPT = ::WindowFromPoint( pt );
				CWnd * pWndPermanentPT = NULL;
				if( hWndPT != NULL )
					pWndPermanentPT = CWnd::FromHandlePermanent( hWndPT );
				if(		pDefInstancePopup != NULL
					&&	pWndPermanentPT != NULL
					&&	pSavedTrackingMenuTBB != NULL
					&&	( wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONDBLCLK )
					)
				{
					ASSERT_VALID( pDefInstancePopup );
					ASSERT( pDefInstancePopup->GetSafeHwnd() != NULL );
					if(		( ! pDefInstancePopup->m_rcExcludeArea.IsRectEmpty() )
						&&	pDefInstancePopup->m_rcExcludeArea.PtInRect( pt )
						&&	pWndPermanentPT != NULL
						&&	pWndPermanentPT->IsKindOf( RUNTIME_CLASS( CExtToolControlBar ) )
						)
					{
						CPoint ptBarClient = pt;
						pWndPermanentPT->ScreenToClient( &ptBarClient );
						INT nHtTbb = ((CExtToolControlBar*)pWndPermanentPT)->HitTest( ptBarClient );
						if( nHtTbb >= 0 && LPVOID(((CExtToolControlBar*)pWndPermanentPT)->GetButton(nHtTbb)) == LPVOID(pSavedTrackingMenuTBB) )
							bSameToolBarButtonIsClicked = true;
					}
				}

				DWORD dwCTF = pWndCapture->TrackFlagsGet();
				bool bNoEat = false;
				HWND hWndCapture = pWndCapture->GetSafeHwnd();
				ASSERT( hWndCapture != NULL && ::IsWindow(hWndCapture) );
				if(	pWndCapture->_OnMouseClick(
						(UINT)wParam,
						ptSrc,
						bNoEat
						)
					)
				{
					if(		( ! bNoEat )
						&&	( dwCTF & TPMX_SYSMENU ) != 0
						&&	( wParam == WM_NCLBUTTONDOWN || wParam == WM_NCLBUTTONDBLCLK )
						&&	nHT == HTSYSMENU
						)
						return
							::CallNextHookEx(
								g_DefPopupMenuSite.m_hMouseHook,
								nCode,
								wParam,
								lParam
								);
					if( ! bSameToolBarButtonIsClicked )
						return 1; // eat!
				}
				if(		( ! bNoEat )
					&&	( dwCTF & TPMX_SYSMENU ) != 0
					&&	( wParam == WM_NCLBUTTONDOWN || wParam == WM_NCLBUTTONDBLCLK )
					&&	nHT == HTSYSMENU
					)
					bNoEat = true;
				if( bNoEat )
					return
						::CallNextHookEx(
							g_DefPopupMenuSite.m_hMouseHook,
							nCode,
							wParam,
							lParam
							);
				if( ! (::IsWindow(hWndCapture)) )
					return 1; // eat!
			
				// post processing			
				if( hWndPT != NULL )
				{
					CExtPopupMenuWnd * pPopup = pWndCapture;
					for( ; pPopup != NULL; pPopup = pPopup->m_pWndParentMenu )
					{
						CPoint ptClient( pt );
						pPopup->ScreenToClient( &ptClient );
						if( pPopup->_PtInWndArea(ptClient) )
							return 1; // eat!
						CExtPopupMenuTipWnd & _tipWnd = pPopup->GetTip();
						HWND hWndTest = _tipWnd.GetSafeHwnd();
						if( hWndTest != NULL )
						{
							if(		hWndTest == hWndPT
								||	::IsChild( hWndTest, hWndPT )
								)
								return 1; // eat!
						} // if( hWndTest != NULL )
						if(		pWndPermanentPT != NULL
							&&	::IsChild( pPopup->m_hWnd, hWndPT )
							&&	pWndPermanentPT->IsKindOf( RUNTIME_CLASS(CScrollBar) )
							)
							return 1; // eat!
					} // for( ; pPopup != NULL; pPopup = pPopup->m_pWndParentMenu )
				} // if( hWndPT != NULL )

				if( pDefInstancePopup != NULL )
				{
					bool bEatEvent = false;
					ASSERT_VALID( pDefInstancePopup );
					ASSERT( pDefInstancePopup->GetSafeHwnd() != NULL );
					if( bSameToolBarButtonIsClicked )
							bEatEvent = true;
					pDefInstancePopup->_CancelingSet();
					pDefInstancePopup->_OnCancelMode();
					if( ! bEatEvent )
					{
						if(		wParam != WM_LBUTTONDOWN
							&&	wParam != WM_RBUTTONDOWN
							&&	wParam != WM_MBUTTONDOWN
							)
							bEatEvent = true;
					}
					if( ! bEatEvent )
					{
						if( ! ::IsWindow( hWndPT ) )
							bEatEvent = true;
					}
					if( bEatEvent )
					{
						if( CExtPopupMenuWnd::g_bEatNcAreaClicks )
							return 1; // eat!
					}
				} // if( pDefInstancePopup != NULL )
//				else
//					CExtPopupMenuSite::g_DefPopupMenuSite._Done();
				
			}
			break;

			//default:
			//	return 1; // eat!
			} // switch( wParam )
		} // if( pWndCapture != NULL ....
		else
			return 1; // eat
	} // if( (! g_DefPopupMenuSite.IsEmpty() ) ...
	return
		::CallNextHookEx(
			g_DefPopupMenuSite.m_hMouseHook,
			nCode,
			wParam,
			lParam
			);
}

LRESULT CALLBACK CExtPopupMenuSite::_HookKeyboardProc(
	int nCode,      // hook code
	WPARAM wParam,  // virtual-key code
	LPARAM lParam   // keystroke-message information
	)
{
	__PROF_UIS_MANAGE_STATE;
	if( nCode != HC_ACTION )
		return
			::CallNextHookEx(
				g_DefPopupMenuSite.m_hKeyboardHook,
				nCode,
				wParam,
				lParam
				);

	if( CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON != NULL )
	{
		if(	CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->
				_OnKeyDown(
					(UINT)wParam,
					LOWORD(lParam),
					HIWORD(lParam)
					)
			)
			return 1; // eat!
	}

	if(		(! g_DefPopupMenuSite.IsEmpty() )
		&&	(! g_DefPopupMenuSite.IsShutdownMode() )
		)
	{
		CExtPopupMenuWnd * pWndCapture =
			g_DefPopupMenuSite.GetCapture();
		if( pWndCapture->GetSafeHwnd() != NULL )
		{
			ASSERT_VALID( pWndCapture );
			if( (lParam & (1<<31)) == 0 )
			{
				// clocks allows to fix problem with
				// duplicated fast key hook calls
				static clock_t _clock_step =
					CLOCKS_PER_SEC / 50; // 1/50 of second
				static clock_t _clock_last =
					0; //clock();
				clock_t _clock_curr =
					clock();
				//ASSERT( _clock_curr >= _clock_last );
				clock_t _clock_diff =
					abs(_clock_curr - _clock_last);
				bool bForceChildProcessing = false;
				HWND hWndFocus = ::GetFocus();
				if(		hWndFocus != NULL
					&&	( ( ::__EXT_MFC_GetWindowLong( hWndFocus, GWL_STYLE ) & WS_CHILD ) != 0 )
					)
				{
					HWND hWndTest = ::GetParent( hWndFocus );
					if( hWndTest != NULL )
					{
						CWnd * pWndPermantent =
							CWnd::FromHandlePermanent( hWndTest );
						if(		pWndPermantent != NULL
							&&	pWndPermantent->IsKindOf(
									RUNTIME_CLASS( CExtPopupMenuWnd )
									)
							)
							bForceChildProcessing = true;
					} // if( hWndTest != NULL )
				} // if( hWndFocus != NULL ...
				if(		_clock_diff >= _clock_step
					||	bForceChildProcessing
					)
				{
					_clock_last = _clock_curr;
					bool bNoEat = false;
					if(	pWndCapture->
							_OnKeyDown(
								(UINT)wParam,
								LOWORD(lParam),
								HIWORD(lParam),
								bNoEat
								)
						)
						return 1; // eat!
					if( bNoEat )
						return
							::CallNextHookEx(
								g_DefPopupMenuSite.m_hKeyboardHook,
								nCode,
								wParam,
								lParam
								);
					if( pWndCapture != NULL )
						return 1; // eat!
				} // if( _clock_diff >= _clock_step ...
				else
					return 1; // eat! 2.28 fix
			} // if( (lParam & (1<<31)) == 0 )
			else
			{
				// 2.21 fix for dialog system menu
				return 1; // eat!
			}
		} // if( pWndCapture->GetSafeHwnd() != NULL ...
//		if( ! CExtToolControlBar::g_bMenuTracking )
//			return 1; // eat!
	} // if( (! g_DefPopupMenuSite.IsEmpty() ) ...

	if(		(!CExtToolControlBar::g_bMenuTracking)
		&&	CExtPopupMenuWnd::IsCmdKeyActivation( lParam )
		&&	( lParam & (1<<31) ) != 0
		)
	{
		CExtPopupMenuWnd::CancelMenuTracking();
		return
			::CallNextHookEx(
				g_DefPopupMenuSite.m_hKeyboardHook,
				nCode,
				wParam,
				lParam
				);
	}

//	return
//		::CallNextHookEx(
//			g_DefPopupMenuSite.m_hKeyboardHook,
//			nCode,
//			wParam,
//			lParam
//			);
	return 1; // eat! 2.28 fix
}

void CExtPopupMenuSite::_Hook(
	bool bHook // = true
	)
{
	if( bHook )
	{
		if( m_hMouseHook == NULL )
		{
			m_hMouseHook =
				::SetWindowsHookEx(
					WH_MOUSE,
					_HookMouseProc, 
					0,
					::GetCurrentThreadId()
					);
			ASSERT( m_hMouseHook != NULL );
		}
		if( m_hKeyboardHook == NULL )
		{
			m_hKeyboardHook =
				::SetWindowsHookEx(
					WH_KEYBOARD,
					_HookKeyboardProc, 
					0,
					::GetCurrentThreadId()
					);
			ASSERT( m_hKeyboardHook != NULL );
		}
	} // if( bHook )
	else
	{
		if( m_hMouseHook != NULL )
		{
			::UnhookWindowsHookEx( m_hMouseHook );
			m_hMouseHook = NULL;
		}
		if( m_hKeyboardHook != NULL )
		{
			::UnhookWindowsHookEx( m_hKeyboardHook );
			m_hKeyboardHook = NULL;
		}
	} // else from if( bHook )
}

/////////////////////////////////////////////////////////////////////////////
// CExtWndShadow

CExtWndShadow::CExtWndShadow()
{
	Destroy();
}

CExtWndShadow::CExtWndShadow( const CExtWndShadow & other )
{
	Destroy();
	_Assign( other );
}

CExtWndShadow::~CExtWndShadow()
{
	Destroy();
}

CExtWndShadow & CExtWndShadow:: operator = ( const CExtWndShadow & other )
{
	Destroy();
	_Assign( other );
	return (*this);
}

void CExtWndShadow::_Assign( const CExtWndShadow & other )
{
	Destroy();
	if( other.m_bmp0org.GetSafeHandle() != NULL )
	{
		COLORREF * pClrSurface = NULL;
		HBITMAP hBitmap =
			CExtBitmap::stat_CloneBitmap(
				(HBITMAP)other.m_bmp0org.GetSafeHandle(),
				NULL,
				&pClrSurface
				);
		ASSERT( hBitmap != NULL );
		if( hBitmap != NULL )
		{
			ASSERT( pClrSurface != NULL );
			m_bmp0org.Attach( hBitmap );
		}
	}
	if( other.m_bmp1org.GetSafeHandle() != NULL )
	{
		COLORREF * pClrSurface = NULL;
		HBITMAP hBitmap =
			CExtBitmap::stat_CloneBitmap(
				(HBITMAP)other.m_bmp1org.GetSafeHandle(),
				NULL,
				&pClrSurface
				);
		ASSERT( hBitmap != NULL );
		if( hBitmap != NULL )
		{
			ASSERT( pClrSurface != NULL );
			m_bmp1org.Attach( hBitmap );
		}
	}
	if( other.m_bmp0mak.GetSafeHandle() != NULL )
	{
		COLORREF * pClrSurface = NULL;
		HBITMAP hBitmap =
			CExtBitmap::stat_CloneBitmap(
				(HBITMAP)other.m_bmp0mak.GetSafeHandle(),
				NULL,
				&pClrSurface
				);
		ASSERT( hBitmap != NULL );
		if( hBitmap != NULL )
		{
			ASSERT( pClrSurface != NULL );
			m_bmp0mak.Attach( hBitmap );
		}
	}
	if( other.m_bmp1mak.GetSafeHandle() != NULL )
	{
		COLORREF * pClrSurface = NULL;
		HBITMAP hBitmap =
			CExtBitmap::stat_CloneBitmap(
				(HBITMAP)other.m_bmp1mak.GetSafeHandle(),
				NULL,
				&pClrSurface
				);
		ASSERT( hBitmap != NULL );
		if( hBitmap != NULL )
		{
			ASSERT( pClrSurface != NULL );
			m_bmp1mak.Attach( hBitmap );
		}
	}
	m_rcWndArea = other.m_rcWndArea;
	m_rc1stArea = other.m_rc1stArea;
	m_rc2ndArea = other.m_rc2ndArea;
	m_nShadowSize = other.m_nShadowSize;
	m_nBr0 = other.m_nBr0;
	m_nBr1 = other.m_nBr1;
	m_bEnablePhotos = other.m_bEnablePhotos;
	m_bEnablePaintManagerColor = other.m_bEnablePaintManagerColor;
	if( other.m_rgnSafe.GetSafeHandle() != NULL )
	{
		if( m_rgnSafe.CreateRectRgn( 0, 0, 0, 0 ) )
		{
			int nCombineResult =
				m_rgnSafe.CombineRgn(
					&m_rgnSafe,
					(CRgn*)&other.m_rgnSafe,
					RGN_OR
					);
			ASSERT( nCombineResult != ERROR );
			nCombineResult;
		} // if( m_rgnSafe.CreateRectRgn( 0, 0, 0, 0 ) )
#ifdef _DEBUG
		else
		{
			ASSERT( FALSE );
		} // else from if( m_rgnSafe.CreateRectRgn( 0, 0, 0, 0 ) )
#endif // _DEBUG
	} // if( other.m_rgnSafe.GetSafeHandle() != NULL )
}

void CExtWndShadow::ApplyFloatingImage(
	HWND hWndStatic,
	HWND hWndFloat,
	LPCRECT pRectCustomFloatPos // = NULL
	)
{
	if(		m_bmp0mak.GetSafeHandle() == NULL
		&&	m_bmp1mak.GetSafeHandle() == NULL
		)
		return;
	if(		hWndStatic == NULL
		||	hWndFloat == NULL
		||	(! ::IsWindow(hWndStatic) )
		||	(! ::IsWindow(hWndFloat) )
		)
	{
		ASSERT( FALSE );
		return;
	}
CRect rcWndStatic = m_rcWndArea, rcWndFloat;
	::ClientToScreen( hWndStatic, ((LPPOINT)(&rcWndStatic)) );
	::ClientToScreen( hWndStatic, ((LPPOINT)(&rcWndStatic))+1 );
	if( pRectCustomFloatPos != NULL )
		rcWndFloat = (*pRectCustomFloatPos);
	else
		::GetWindowRect( hWndFloat, rcWndFloat );
CRect rcIntersection;
	if(	! rcIntersection.IntersectRect(
			&rcWndStatic,
			&rcWndFloat
			)
		)
		return;
HBITMAP hBmpFloat =
		CExtPaintManager::stat_PrintWnd( hWndFloat );
	if( hBmpFloat == NULL )
	{
		ASSERT( FALSE );
		return;
	}
CPoint ptOffset =
		rcWndFloat.TopLeft()
		- rcWndStatic.TopLeft()
		;
CBitmap _bmpFloatDestructor;
	_bmpFloatDestructor.Attach( hBmpFloat );
BITMAP _bmpInfo;
	::memset( &_bmpInfo, 0, sizeof(BITMAP) );
	::GetObject( hBmpFloat, sizeof(BITMAP), &_bmpInfo );
	if( ! ( _bmpInfo.bmWidth > 0 && _bmpInfo.bmHeight > 0 ) )
	{
		ASSERT( FALSE );
		return;
	}
CDC dcSrc, dcDst;
	if(		(! dcSrc.CreateCompatibleDC( NULL ) )
		||	(! dcDst.CreateCompatibleDC( NULL ) )
		)
	{
		ASSERT( FALSE );
		return;
	}
HGDIOBJ hBmpOldSrc = ::SelectObject( dcSrc.GetSafeHdc(), hBmpFloat );
	if( m_bmp0mak.GetSafeHandle() != NULL )
	{
		CBitmap * pOldBmpDst = dcDst.SelectObject( &m_bmp0mak );
		dcDst.BitBlt(
			ptOffset.x - m_rcWndArea.Width(),
			ptOffset.y,
			_bmpInfo.bmWidth,
			_bmpInfo.bmHeight,
			&dcSrc,
			0,
			0,
			SRCCOPY
			);
		dcDst.SelectObject( pOldBmpDst );
	}
	if( m_bmp1mak.GetSafeHandle() != NULL )
	{
		CBitmap * pOldBmpDst = dcDst.SelectObject( &m_bmp1mak );
		dcDst.BitBlt(
			ptOffset.x,
			ptOffset.y - m_rcWndArea.Height(),
			_bmpInfo.bmWidth,
			_bmpInfo.bmHeight,
			&dcSrc,
			0,
			0,
			SRCCOPY
			);
		dcDst.SelectObject( pOldBmpDst );
	}
	::SelectObject( dcSrc.GetSafeHdc(), hBmpOldSrc );
}

void CExtWndShadow::_DoPixelOvershadow(
	int nMakeSpec,
	int nPosX,
	int nPosY,
	COLORREF clrShadowAdjust
	)
{
	ASSERT( nMakeSpec >= 0 && nMakeSpec <= 100 );
	ASSERT( ! m_rcWndArea.IsRectEmpty() );
// 	ASSERT( nPosX < m_rcWndArea.Width() + INT(m_nShadowSize) );
// 	ASSERT( nPosY < m_rcWndArea.Height() + INT(m_nShadowSize) );
	if( nPosX >= m_rcWndArea.Width() + INT(m_nShadowSize) )
		return;
	if( nPosY >= m_rcWndArea.Height() + INT(m_nShadowSize) )
		return;
	ASSERT( m_pHelperDibSurface != NULL );
int nTotalWidth = m_rcWndArea.Width()+m_nShadowSize;
int nTotalHeight = m_rcWndArea.Height()+m_nShadowSize;
COLORREF * ptr =
		m_pHelperDibSurface
		+ nPosX
		+ (nTotalHeight-nPosY)*nTotalWidth;
COLORREF clrAdj = *ptr;
	if( clrShadowAdjust != ((COLORREF)(-1)) )
	{
		int nMakeSpecR = nMakeSpec + ::MulDiv( (100-nMakeSpec), GetBValue(clrShadowAdjust), 255 );
		int nMakeSpecG = nMakeSpec + ::MulDiv( (100-nMakeSpec), GetGValue(clrShadowAdjust), 255 );
		int nMakeSpecB = nMakeSpec + ::MulDiv( (100-nMakeSpec), GetRValue(clrShadowAdjust), 255 );
		if( nMakeSpecR > 100 )
			nMakeSpecR = 100;
		if( nMakeSpecG > 100 )
			nMakeSpecG = 100;
		if( nMakeSpecB > 100 )
			nMakeSpecB = 100;
		clrAdj = 
			RGB(
				(( nMakeSpecR * int(GetRValue(clrAdj)) ) / 100),
				(( nMakeSpecG * int(GetGValue(clrAdj)) ) / 100),
				(( nMakeSpecB * int(GetBValue(clrAdj)) ) / 100)
				);
	} // if( clrShadowAdjust != ((COLORREF)(-1)) )
	else
	{
		clrAdj = 
			RGB(
				(( nMakeSpec * int(GetRValue(clrAdj)) ) / 100),
				(( nMakeSpec * int(GetGValue(clrAdj)) ) / 100),
				(( nMakeSpec * int(GetBValue(clrAdj)) ) / 100)
				);
	} // else from if( clrShadowAdjust != ((COLORREF)(-1)) )
	*ptr = clrAdj;
}

bool CExtWndShadow::HavePhotos() const
{
	ASSERT( m_nShadowSize >= 0 );
	if(		m_nShadowSize == 0
		||	m_rcWndArea.IsRectEmpty()
		)
		return false;
	if(		m_bmp0mak.GetSafeHandle() == NULL
		||	m_bmp1mak.GetSafeHandle() == NULL
		)
		return false;
	return true;
}

bool CExtWndShadow::_HaveOriginals() const
{
	ASSERT( m_nShadowSize >= 0 );
	if(		m_nShadowSize == 0
		||	m_rcWndArea.IsRectEmpty()
		)
		return false;
	if(		m_bmp0org.GetSafeHandle() == NULL
		||	m_bmp1org.GetSafeHandle() == NULL
		)
		return false;
	return true;
}

bool CExtWndShadow::_RestoreOriginals( CDC & dc )
{
	ASSERT( m_nShadowSize >= 0 );
	if(		m_nShadowSize == 0
		||	m_rcWndArea.IsRectEmpty ()
		)
		return true;
	if(	! _HaveOriginals() )
		return false;
INT nWndAreaDX = m_rcWndArea.Width();
INT nWndAreaDY = m_rcWndArea.Height();
	ASSERT( nWndAreaDX > 0 && nWndAreaDY > 0 );
CDC dcmm;
	if( ! dcmm.CreateCompatibleDC(&dc) )
	{
		ASSERT( FALSE );
		return false;
	}

CBitmap * pbmpold = dcmm.SelectObject( &m_bmp0org );
	ASSERT( pbmpold != NULL );
	dc.BitBlt(
		m_rcWndArea.Width(),
		0,
		m_nShadowSize,
		nWndAreaDY + m_nShadowSize,
		&dcmm,
		0,
		0,
		SRCCOPY
		);
	dcmm.SelectObject( &m_bmp1org );
	dc.BitBlt(
		0,
		m_rcWndArea.Height(),
		nWndAreaDX + m_nShadowSize,
		m_nShadowSize,
		&dcmm,
		0,
		0,
		SRCCOPY
		);
	dcmm.SelectObject( pbmpold );
	return true;
}

bool CExtWndShadow::Restore( CDC & dc )
{
	ASSERT( m_nShadowSize >= 0 );
	if(		m_nShadowSize == 0
		||	m_rcWndArea.IsRectEmpty ()
		)
		return true;
	if(	! HavePhotos() )
		return false;

INT nWndAreaDX = m_rcWndArea.Width();
INT nWndAreaDY = m_rcWndArea.Height();
	ASSERT( nWndAreaDX > 0 && nWndAreaDY > 0 );
CDC dcmm;
	if( ! dcmm.CreateCompatibleDC(&dc) )
	{
		ASSERT( FALSE );
		return false;
	}

	if( m_rgnSafe.GetSafeHandle() != NULL )
		dc.SelectClipRgn( &m_rgnSafe, RGN_AND );

CBitmap * pbmpold = dcmm.SelectObject( &m_bmp0mak );
	ASSERT( pbmpold != NULL );
	dc.BitBlt(
		m_rcWndArea.right,
		m_rcWndArea.top,
		m_nShadowSize,
		nWndAreaDY + m_nShadowSize,
		&dcmm,
		0,
		0,
		SRCCOPY
		);
	dcmm.SelectObject( &m_bmp1mak );
	dc.BitBlt(
		m_rcWndArea.left,
		m_rcWndArea.bottom,
		nWndAreaDX + m_nShadowSize,
		m_nShadowSize,
		&dcmm,
		0,
		0,
		SRCCOPY
		);
	dcmm.SelectObject( pbmpold );

	if( m_rgnSafe.GetSafeHandle() != NULL )
		dc.SelectClipRgn( NULL );
	
	return true;
}

bool CExtWndShadow::Paint(
	CExtPaintManager * pPM,
	CDC & dc,
	const CRect & rcWndArea,
	const CRect & rc1stArea, // = CRect(0,0,0,0)
	const CRect & rc2ndArea, // = CRect(0,0,0,0)
	UINT nShadowSize, // = DEF_SHADOW_SIZE
	UINT nBr0, // = DEF_BRIGHTNESS_MIN
	UINT nBr1, // = DEF_BRIGHTNESS_MAX
	bool bEnablePhotos, // = true
	bool bEnablePaintManagerColor, // = true
	bool m_bMakePhotosOnly, // = false
	HWND hWndStatic, // = NULL
	HWND hWndFloat, // = NULL
	LPCRECT pRectCustomFloatPos // = NULL
	)
{
	m_rcWndArea = rcWndArea;
	m_rc1stArea = rc1stArea;
	m_rc2ndArea = rc2ndArea;
	m_nShadowSize = nShadowSize;
	m_nBr0 = nBr0;
	m_nBr1 = nBr1;
	m_bEnablePhotos = bEnablePhotos;
	m_bEnablePaintManagerColor = bEnablePaintManagerColor;
	return
		Paint(
			pPM,
			dc,
			m_bMakePhotosOnly,
			hWndStatic,
			hWndFloat,
			pRectCustomFloatPos
			);
}

bool CExtWndShadow::Paint(
	CExtPaintManager * pPM,
	CDC & dc,
	bool m_bMakePhotosOnly, // = false
	HWND hWndStatic, // = NULL
	HWND hWndFloat, // = NULL
	LPCRECT pRectCustomFloatPos // = NULL
	)
{
	ASSERT( m_nShadowSize >= 0 );
	ASSERT( ! m_rcWndArea.IsRectEmpty() );
	if(		m_nShadowSize == 0
		||	m_rcWndArea.IsRectEmpty()
		)
		return true;
	if( hWndFloat != NULL )
	{
		ASSERT( hWndStatic != NULL );
		ASSERT( ::IsWindow( hWndFloat ) );
		ASSERT( ::IsWindow( hWndStatic ) );
		__EXT_MFC_LONG_PTR dwStyle = ::__EXT_MFC_GetWindowLong(hWndFloat,GWL_STYLE);
		if( (dwStyle&WS_VISIBLE) == 0 )
			hWndFloat = hWndStatic = NULL;
	} // if( hWndFloat != NULL )
	else
	{
		hWndStatic = NULL;
	} // else from if( hWndFloat != NULL )

	if( ! _HaveOriginals() )
		_MakeOriginals( dc );

INT nWndAreaDX = m_rcWndArea.Width();
INT nWndAreaDY = m_rcWndArea.Height();
	if(		m_rgnSafe.GetSafeHandle() == NULL
		&&	(! m_rc1stArea.IsRectEmpty() )
		&&	(! m_rc2ndArea.IsRectEmpty() )
		)
	{
		CRect rc1(
			m_rcWndArea.right,
			m_rcWndArea.top,
			m_rcWndArea.right + m_nShadowSize,
			m_rcWndArea.top + nWndAreaDY + m_nShadowSize
			);
		CRect rc2(
			m_rcWndArea.left,
			m_rcWndArea.bottom,
			m_rcWndArea.left + nWndAreaDX + m_nShadowSize,
			m_rcWndArea.bottom + m_nShadowSize
			);
		m_rgnSafe.CreateRectRgnIndirect( &rc1 );
		if( m_rgnSafe.GetSafeHandle() != NULL )
		{
			CRgn rgnSecond;
			rgnSecond.CreateRectRgnIndirect( &rc2 );
			if(		rgnSecond.GetSafeHandle() != NULL
				&&	m_rgnSafe.CombineRgn( &m_rgnSafe, &rgnSecond, RGN_OR ) != ERROR
				)
			{
			}
			else
				m_rgnSafe.DeleteObject();
		} // if( m_rgnSafe.GetSafeHandle() != NULL )
	} // if( m_rgnSafe.GetSafeHandle() == NULL ...

	if( m_bMakePhotosOnly && m_bEnablePhotos )
	{
		CDC dcCache;
		CBitmap bmpCache;
		int nDX = nWndAreaDX + m_nShadowSize;
		int nDY = nWndAreaDY + m_nShadowSize;
		nDX += m_rc2ndArea.Width();
		nDY += m_rc2ndArea.Height();
		if(		dcCache.CreateCompatibleDC( &dc )
			&&	bmpCache.CreateCompatibleBitmap( &dc, nDX, nDY )
			)
		{
			CBitmap * pOldBmp = dcCache.SelectObject( &bmpCache );
			bool bRetVal = false;
			if(	dcCache.BitBlt(
					0, 0, nDX, nDY,
					&dc, 0, 0, SRCCOPY
					)
				)
			{
				bRetVal = Paint( pPM, dcCache, false );
				ASSERT( bRetVal );
			}
#ifdef _DEBUG
			else
			{
				ASSERT( FALSE );
			}
#endif // _DEBUG
			dcCache.SelectObject( pOldBmp );
			return bRetVal;
		}
#ifdef _DEBUG
		else
		{
			ASSERT( FALSE );
		}
#endif // _DEBUG
	} // if( m_bMakePhotosOnly && m_bEnablePhotos )

	if( m_rgnSafe.GetSafeHandle() != NULL )
		dc.SelectClipRgn( &m_rgnSafe, RGN_AND );
bool bRetVal = true, bDoPaint = false;
	if( _HaveOriginals() && hWndStatic != NULL && hWndFloat != NULL )
		bDoPaint = true;
	if( ! bDoPaint )
	{
		if( ! Restore( dc ) )
			bDoPaint = true;
	}
	if( bDoPaint )
	{
		if( CExtPaintManager::stat_GetBPP() > 8 )
			bRetVal =
				_PaintHi(
					pPM,
					dc,
					hWndStatic,
					hWndFloat,
					pRectCustomFloatPos
					);
		else
			bRetVal =
				_PaintLo(
					pPM,
					dc,
					hWndStatic,
					hWndFloat,
					pRectCustomFloatPos
					);
	}
	if( m_rgnSafe.GetSafeHandle() != NULL )
		dc.SelectClipRgn( NULL );
	return bRetVal;
}

bool CExtWndShadow::_PaintLo(
	CExtPaintManager * pPM,
	CDC & dc,
	HWND hWndStatic, // = NUL
	HWND hWndFloat, // = NULL
	LPCRECT pRectCustomFloatPos // = NULL
	)
{
	if( pPM == NULL )
		pPM = g_PaintManager.GetPM();
	ASSERT_VALID( pPM );
INT nWndAreaDX = m_rcWndArea.Width();
INT nWndAreaDY = m_rcWndArea.Height();
	if( m_bmp0mak.GetSafeHandle() != NULL )
		m_bmp0mak.DeleteObject();
	if( m_bmp1mak.GetSafeHandle() != NULL )
		m_bmp1mak.DeleteObject();
static int _Pattern[] =
{
	~0xAA,
	~0x55,
	~0xAA,
	~0x55,
	~0xAA,
	~0x55,
	~0xAA,
	~0x55
};
CBitmap _bitmapPattern;
CBrush _brushPattern;
	if(		(! _bitmapPattern.CreateBitmap( 8, 8, 1, 1, _Pattern ) )
		||	(! _brushPattern.CreatePatternBrush( &_bitmapPattern ) )
		)
		return false;
CRect rcMM(
		m_rcWndArea.left,
		m_rcWndArea.top,
		m_rcWndArea.right + nWndAreaDX,
		m_rcWndArea.bottom + nWndAreaDY
		);
CExtMemoryDC dcmm(
		&dc,
		&rcMM,
///		CExtMemoryDC::MDCOPT_RTL_COMPATIBILITY	|
			CExtMemoryDC::MDCOPT_TO_MEMORY
		);
	ASSERT( dcmm.GetSafeHdc() != NULL );
	if( dcmm.GetSafeHdc() == NULL )
		return false;
	dcmm.SetViewportOrg( 0, 0 );
	dcmm.SetWindowOrg( 0, 0 );
CPalette * pOldPalette = NULL;
	if( (::GetDeviceCaps(dcmm.m_hDC,RASTERCAPS) & RC_PALETTE) != 0 )
	{
        pOldPalette =
			dcmm.SelectPalette( & pPM->m_PaletteWide, FALSE );
        dcmm.RealizePalette();
    }
	dcmm.BitBlt(
		0,
		0,
		nWndAreaDX + m_nShadowSize,
		nWndAreaDY + m_nShadowSize,
		&dc,
		m_rcWndArea.left,
		m_rcWndArea.top,
		SRCCOPY
		);
	if( _HaveOriginals() )
		_RestoreOriginals( dcmm );

	if( hWndStatic != NULL && hWndFloat != NULL )
	{
		ASSERT( ::IsWindow(hWndStatic) );
		ASSERT( ::IsWindow(hWndFloat) );
		CRect rcWndStatic = m_rcWndArea, rcWndFloat;
		::ClientToScreen( hWndStatic, ((LPPOINT)(&rcWndStatic)) );
		::ClientToScreen( hWndStatic, ((LPPOINT)(&rcWndStatic))+1 );
		if( pRectCustomFloatPos != NULL )
			rcWndFloat = (*pRectCustomFloatPos);
		else
			::GetWindowRect( hWndFloat, rcWndFloat );
		HBITMAP hBmpFloat =
			CExtPaintManager::stat_PrintWnd( hWndFloat );
		if( hBmpFloat == NULL )
		{
			ASSERT( FALSE );
			return false;
		}
		CPoint ptOffset =
			rcWndFloat.TopLeft()
			- rcWndStatic.TopLeft()
			;
		CBitmap _bmpFloatDestructor;
		_bmpFloatDestructor.Attach( hBmpFloat );
		BITMAP _bmpInfo;
		::memset( &_bmpInfo, 0, sizeof(BITMAP) );
		::GetObject( hBmpFloat, sizeof(BITMAP), &_bmpInfo );
		if( ! ( _bmpInfo.bmWidth > 0 && _bmpInfo.bmHeight > 0 ) )
		{
			ASSERT( FALSE );
			return false;
		}
		CDC dcSrc;
		if(! dcSrc.CreateCompatibleDC( NULL ) )
		{
			ASSERT( FALSE );
			return false;
		}
		HGDIOBJ hBmpOldSrc = ::SelectObject( dcSrc.GetSafeHdc(), hBmpFloat );
		dcmm.BitBlt(
			ptOffset.x,
			ptOffset.y,
			_bmpInfo.bmWidth,
			_bmpInfo.bmHeight,
			&dcSrc,
			0,
			0,
			SRCCOPY
			);
		::SelectObject( dcSrc.GetSafeHdc(), hBmpOldSrc );
		_bmpFloatDestructor.DeleteObject();
	} // if( hWndStatic != NULL && hWndFloat != NULL )

UINT nPaintShadowSize =
		( m_nShadowSize > 4 )
			? 4
			: m_nShadowSize
			;
CRect rV(	nWndAreaDX,
			nPaintShadowSize,
			nWndAreaDX + nPaintShadowSize,
			nWndAreaDY
			);
CRect rH(	nPaintShadowSize,
			nWndAreaDY,
			nWndAreaDX + nPaintShadowSize,
			nWndAreaDY + nPaintShadowSize
			);
CBrush * pOldBrush = dcmm.SelectObject( &_brushPattern );
	dcmm.PatBlt( rH.left, rH.top, rH.Width(), rH.Height(), 0xA000C9 );
	dcmm.PatBlt( rV.left, rV.top, rV.Width(), rV.Height(), 0xA000C9 );
	dcmm.SelectObject( pOldBrush );
	dc.ExcludeClipRect( &m_rcWndArea );
	if( m_rgnSafe.GetSafeHandle() != NULL )
		dc.SelectClipRgn( &m_rgnSafe, RGN_AND );
	dc.BitBlt(
		m_rcWndArea.left,
		m_rcWndArea.top,
		nWndAreaDX + m_nShadowSize,
		nWndAreaDY + m_nShadowSize, 
		&dcmm,
		0,
		0,
		SRCCOPY
		);
	dc.SelectClipRgn( NULL );
	if( m_bEnablePhotos )
	{
		if( ! _MakePhotos(dc,dcmm) )
		{
			ASSERT( FALSE );
			return false;
		}
	}
	if( pOldPalette != NULL )
		dcmm.SelectPalette( pOldPalette, FALSE );
	dcmm.__Flush( FALSE );
	return true;
}

bool CExtWndShadow::_PaintHi(
	CExtPaintManager * pPM,
	CDC & dc,
	HWND hWndStatic, // = NULL
	HWND hWndFloat, // = NULL
	LPCRECT pRectCustomFloatPos // = NULL
	)
{
	if( pPM == NULL )
		pPM = g_PaintManager.GetPM();
	ASSERT_VALID( pPM );
INT nWndAreaDX = m_rcWndArea.Width();
INT nWndAreaDY = m_rcWndArea.Height();
CDC dcmm;
	if( ! dcmm.CreateCompatibleDC( &dc ) )
	{
		ASSERT( FALSE );
		return false;
	}
COLORREF clrShadowAdjust = ((COLORREF)(-1));
	if( m_bEnablePaintManagerColor )
		clrShadowAdjust = pPM->GetShadowAdjustColor();
BITMAPINFOHEADER bih;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = nWndAreaDX+m_nShadowSize;
	bih.biHeight = nWndAreaDY+m_nShadowSize;
	bih.biPlanes = 1;
	bih.biBitCount = 32;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = (nWndAreaDX+m_nShadowSize) * (nWndAreaDY+m_nShadowSize);
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;
	ASSERT( m_pHelperDibSurface == NULL );
HBITMAP hDIB =
		::CreateDIBSection(
			dcmm.GetSafeHdc(),
			(LPBITMAPINFO)&bih,
			DIB_RGB_COLORS,
			(void **)&m_pHelperDibSurface,
			NULL,
			NULL
			);
	if( hDIB == NULL || m_pHelperDibSurface == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
HGDIOBJ hOldSurface = dcmm.SelectObject( hDIB );
	dcmm.BitBlt(
		0,
		0,
		nWndAreaDX + m_nShadowSize,
		nWndAreaDY + m_nShadowSize,
		&dc,
		m_rcWndArea.left,
		m_rcWndArea.top,
		SRCCOPY
		);
	if( _HaveOriginals() )
		_RestoreOriginals( dcmm );

	if( hWndStatic != NULL && hWndFloat != NULL )
	{
		ASSERT( ::IsWindow(hWndStatic) );
		ASSERT( ::IsWindow(hWndFloat) );
		CRect rcWndStatic = m_rcWndArea, rcWndFloat;
		::ClientToScreen( hWndStatic, ((LPPOINT)(&rcWndStatic)) );
		::ClientToScreen( hWndStatic, ((LPPOINT)(&rcWndStatic))+1 );
		if( pRectCustomFloatPos != NULL )
			rcWndFloat = (*pRectCustomFloatPos);
		else
			::GetWindowRect( hWndFloat, rcWndFloat );
		HBITMAP hBmpFloat =
			CExtPaintManager::stat_PrintWnd( hWndFloat );
		if( hBmpFloat == NULL )
		{
			ASSERT( FALSE );
			return false;
		}
		CPoint ptOffset =
			rcWndFloat.TopLeft()
			- rcWndStatic.TopLeft()
			;
		CBitmap _bmpFloatDestructor;
		_bmpFloatDestructor.Attach( hBmpFloat );
		BITMAP _bmpInfo;
		::memset( &_bmpInfo, 0, sizeof(BITMAP) );
		::GetObject( hBmpFloat, sizeof(BITMAP), &_bmpInfo );
		if( ! ( _bmpInfo.bmWidth > 0 && _bmpInfo.bmHeight > 0 ) )
		{
			ASSERT( FALSE );
			return false;
		}
		CDC dcSrc;
		if( ! dcSrc.CreateCompatibleDC( NULL ) )
		{
			ASSERT( FALSE );
			return false;
		}
		HGDIOBJ hBmpOldSrc = ::SelectObject( dcSrc.GetSafeHdc(), hBmpFloat );
		dcmm.BitBlt(
			ptOffset.x,
			ptOffset.y,
			_bmpInfo.bmWidth,
			_bmpInfo.bmHeight,
			&dcSrc,
			0,
			0,
			SRCCOPY
			);
		::SelectObject( dcSrc.GetSafeHdc(), hBmpOldSrc );
		_bmpFloatDestructor.DeleteObject();
	} // if( hWndStatic != NULL && hWndFloat != NULL )

UINT nBrDiff = m_nBr1 - m_nBr0;
LONG n2nd, nStep, nDist, nDist1;
LONG nDist2 = m_nShadowSize * m_nShadowSize;
INT nMakeSpec;
bool	bCmbaV = false, bCmbaH = false,
		bCmbaVrt = false, bCmbaVrb = false,
		bCmbaHrb = false, bCmbaHlb = false,
		bCmbaLA = false;
INT		nX0 = 0, nX1 = nWndAreaDX,
		nY0 = 0, nY1 = nWndAreaDY,
		nLa0 = 0, nLa1 = 0;
	if( !( m_rc1stArea.IsRectEmpty() || m_rc2ndArea.IsRectEmpty() ) )
	{
		bool bCmbaDetected = false;
		if( m_rc1stArea.right == m_rc2ndArea.right )
		{ // vertical-right combining
			bCmbaDetected = true;
			if( m_rc1stArea.top < m_rc2ndArea.top )
				bCmbaVrb = true;
			else
				bCmbaVrt = true;
		} // vertical-right combining
		if(		(! bCmbaDetected )
			&&	m_rc1stArea.left == m_rc2ndArea.left
			)
		{ // vertical-left combining
			if(		m_rc2ndArea.bottom >= m_rc1stArea.top-1
				&&	m_rc2ndArea.top < m_rc1stArea.top
				)
			{
				if( m_rc2ndArea.right >= m_rc1stArea.right )
				{
					bCmbaDetected = true;
					bCmbaV = true;
				}
			}
			if(		(! bCmbaDetected )
				&&	m_rc1stArea.bottom >= m_rc2ndArea.top-1 
				&&	m_rc1stArea.top < m_rc2ndArea.top
				)
			{
				if( m_rc1stArea.right >= m_rc2ndArea.right )
				{
					bCmbaDetected = true;
					bCmbaH = true;
					nX0 =
						m_rcWndArea.right  -
						(m_rc1stArea.right - m_rc2ndArea.right);
				}
			}
		} // vertical-left combining
		if(		(! bCmbaDetected )
			&&	m_rc1stArea.bottom == m_rc2ndArea.bottom
			)
		{ // horizontal-bottom combining
			bCmbaDetected = true;
			if( m_rc1stArea.left < m_rc2ndArea.left )
				bCmbaHrb = true;
			else
				bCmbaHlb = true;
		} // horizontal-bottom combining
		if(		(!bCmbaDetected)
			&&	m_rc1stArea.right == m_rc2ndArea.left+1
			)
		{
			bCmbaDetected = true;
			bCmbaLA = true;
			nLa0 = m_rcWndArea.top
				+ m_rc2ndArea.bottom - m_rc1stArea.top;
			nLa1 = nLa0 + m_nShadowSize;
		}
		if(		(! bCmbaDetected )
			&&	m_rc1stArea.left == m_rc2ndArea.right - 1
			)
		{
			bCmbaDetected = true;
			bCmbaH = true;
		}
		bCmbaDetected;
	} // if( !( m_rc1stArea.IsRectEmpty() || m_rc2ndArea.IsRectEmpty() ) )
	for( nStep = 0; ULONG(nStep) < m_nShadowSize; nStep++ )
	{
		nMakeSpec = m_nBr0 + ( nStep * nBrDiff ) / m_nShadowSize;
		for(	n2nd = nX0
					+ m_nShadowSize * 2 + 1
					- ( bCmbaH ? ( m_nShadowSize * 2 - nStep ) : 0 )
					- ( bCmbaHlb ? m_nShadowSize : 0 )
					;
				n2nd < LONG(nX1) + ( bCmbaHrb ? LONG(m_nShadowSize) : 0 );
				n2nd++
				)
			_DoPixelOvershadow(
				nMakeSpec,
				n2nd,
				nY1 + nStep,
				clrShadowAdjust
				);
		for(	n2nd = nY0
					+ m_nShadowSize*2+1
					- ( bCmbaV ? ( m_nShadowSize * 2 - nStep ) : 0 )
					- ( bCmbaVrt ? m_nShadowSize : 0 )
					;
				n2nd < LONG(nY1) + ( bCmbaVrb ? LONG(m_nShadowSize) : 0 );
				n2nd++
				)
		{
			if(		bCmbaLA
				&&	n2nd >= nLa0
				&&	n2nd < LONG(nLa1) - ( LONG(m_nShadowSize) - nStep )
				)
			{
				continue;
			}
			_DoPixelOvershadow(
				nMakeSpec,
				nX1 + nStep,
				n2nd,
				clrShadowAdjust
				);
		}
		nDist1 = nStep*nStep;
		for( n2nd = 0; ULONG(n2nd) < m_nShadowSize; n2nd++ )
		{
			nDist = nDist1 + n2nd * n2nd;
			if( nDist > nDist2 )
				continue;
			nDist = (LONG)::sqrt( double(nDist) );
			nMakeSpec = m_nBr0 + ( nDist * nBrDiff ) / m_nShadowSize;
			if( ! ( bCmbaVrb || bCmbaHrb ) )
				_DoPixelOvershadow(
					nMakeSpec,
					nX1 + nStep,
					nY1 + n2nd,
					clrShadowAdjust
					);
			if( ! ( bCmbaH || bCmbaHlb ) )
				_DoPixelOvershadow(
					nMakeSpec,
					m_nShadowSize + ( m_nShadowSize - nStep ),
					nY1 + n2nd,
					clrShadowAdjust
					);
			if( ! ( bCmbaV || bCmbaVrt ) )
				_DoPixelOvershadow(
					nMakeSpec,
					nX1 + nStep,
					m_nShadowSize + ( m_nShadowSize - n2nd ),
					clrShadowAdjust
					);
		} // for( n2nd = 0; ULONG(n2nd) < m_nShadowSize; n2nd++ )
	} // for( nStep = 0; ULONG(nStep) < m_nShadowSize; nStep++ )
	dc.ExcludeClipRect( &m_rcWndArea );
	if( m_rgnSafe.GetSafeHandle() != NULL )
		dc.SelectClipRgn( &m_rgnSafe, RGN_AND );
	dc.BitBlt(
		m_rcWndArea.left,
		m_rcWndArea.top,
		nWndAreaDX + m_nShadowSize,
		nWndAreaDY + m_nShadowSize, 
		&dcmm,
		0,
		0,
		SRCCOPY
		);
	dc.SelectClipRgn( NULL );

bool bRetVal = true;
	if( m_bEnablePhotos )
	{
		if( ! _MakePhotos( dc, dcmm ) )
		{
			ASSERT( FALSE );
			bRetVal = false;
		} // if( ! _MakePhotos( dc, dcmm ) )
	} // if( m_bEnablePhotos )

	if( hOldSurface != NULL )
		dcmm.SelectObject( hOldSurface );
	::DeleteObject( hDIB );
	m_pHelperDibSurface = NULL;

	return bRetVal;
}

bool CExtWndShadow::_MakePhotos( CDC & dc, CDC & dcmm )
{
	ASSERT( m_nShadowSize >= 0 );
	ASSERT( ! m_rcWndArea.IsRectEmpty() );
INT nWndAreaDX = m_rcWndArea.Width();
INT nWndAreaDY = m_rcWndArea.Height();
	if( m_bmp0mak.GetSafeHandle() != NULL )
		m_bmp0mak.DeleteObject();
	if( m_bmp1mak.GetSafeHandle() != NULL )
		m_bmp1mak.DeleteObject();
	if(	! m_bmp0mak.CreateCompatibleBitmap(
			&dc,
			m_nShadowSize,
			nWndAreaDY + m_nShadowSize
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	dcmm.SelectObject( &m_bmp0mak );
	if(	! dcmm.BitBlt(
			0,
			0,
			m_nShadowSize,
			nWndAreaDY + m_nShadowSize,
			&dc,
			m_rcWndArea.right,
			m_rcWndArea.top,
			SRCCOPY
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	if(	! m_bmp1mak.CreateCompatibleBitmap(
			&dc,
			nWndAreaDX + m_nShadowSize,
			m_nShadowSize
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	dcmm.SelectObject( &m_bmp1mak );
	if(	! dcmm.BitBlt(
			0,
			0,
			nWndAreaDX + m_nShadowSize,
			m_nShadowSize,
			&dc,
			m_rcWndArea.left,
			m_rcWndArea.bottom,
			SRCCOPY
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	return true;
}

bool CExtWndShadow::_MakeOriginals( CDC & dc )
{
	ASSERT( m_nShadowSize >= 0 );
	ASSERT( ! m_rcWndArea.IsRectEmpty() );
INT nWndAreaDX = m_rcWndArea.Width();
INT nWndAreaDY = m_rcWndArea.Height();
	ASSERT(
			m_bmp0org.GetSafeHandle() == NULL
		&&	m_bmp1org.GetSafeHandle() == NULL
		);
CDC dcmm;
	if( ! dcmm.CreateCompatibleDC( NULL ) )
	{
		ASSERT( FALSE );
		return false;
	}
	if(	! m_bmp0org.CreateCompatibleBitmap(
			&dc,
			m_nShadowSize,
			nWndAreaDY + m_nShadowSize
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
CBitmap * pOldBmp = dcmm.SelectObject( &m_bmp0org );
	if(	! dcmm.BitBlt(
			0,
			0,
			m_nShadowSize,
			nWndAreaDY + m_nShadowSize,
			&dc,
			m_rcWndArea.right,
			m_rcWndArea.top,
			SRCCOPY
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	if(	! m_bmp1org.CreateCompatibleBitmap(
			&dc,
			nWndAreaDX + m_nShadowSize,
			m_nShadowSize
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	dcmm.SelectObject( &m_bmp1org );
	if(	! dcmm.BitBlt(
			0,
			0,
			nWndAreaDX + m_nShadowSize,
			m_nShadowSize,
			&dc,
			m_rcWndArea.left,
			m_rcWndArea.bottom,
			SRCCOPY
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	dcmm.SelectObject( pOldBmp );
	return true;
}

void CExtWndShadow::_FreeWinObjects()
{
	if( m_bmp0org.GetSafeHandle() != NULL )
		m_bmp0org.DeleteObject();
	if( m_bmp1org.GetSafeHandle() != NULL )
		m_bmp1org.DeleteObject();
	if( m_bmp0mak.GetSafeHandle() != NULL )
		m_bmp0mak.DeleteObject();
	if( m_bmp1mak.GetSafeHandle() != NULL )
		m_bmp1mak.DeleteObject();
	m_pHelperDibSurface = NULL;
}

void CExtWndShadow::Destroy()
{
	_FreeWinObjects();
	m_rcWndArea.SetRectEmpty();
	m_rgnSafe.DeleteObject();
	m_bEnablePhotos = false;
	m_bEnablePaintManagerColor = false;
}

void CExtWndShadow::PostReShadow()
{
	if(		m_bmp0org.GetSafeHandle() != NULL
		&&	m_bmp1org.GetSafeHandle() != NULL
		&&	m_bmp0mak.GetSafeHandle() != NULL
		&&	m_bmp1mak.GetSafeHandle() != NULL
		)
	{
		m_bmp0mak.DeleteObject();
		m_bmp1mak.DeleteObject();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CExtPopupMenuWnd::MENUITEMDATA

CExtPopupMenuWnd::MENUITEMDATA::MENUITEMDATA(
	CExtPopupMenuWnd * pOwner // = NULL
	)
	: m_cAccelChar( _T('\0') )
	, m_pOwner( pOwner )
{
	// self constant type assertion
	ASSERT( TYPE_SEPARATOR==ID_SEPARATOR );
	m_pWndChild = NULL;
	m_nItemIndex = -100;
	m_nCmdID = TYPE_SEPARATOR;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_pCmdNode = NULL;
	m_pCustomKeyTipInfoBasic = NULL;
	m_pCustomKeyTipInfoDD = NULL;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_sItemText = m_sAccelText = m_sExtendedText = _T("");
	m_sizeItem.cx = m_sizeItem.cy = 0;
	m_nIconAreaWidth = 0;
	m_bSelected
		= m_bDisplayed
		= m_bForceDisplayed
		= m_bChildCombine
		= m_bToolButton
		= m_bToolWrap
		= m_bBold
		= m_bDefault
		= m_bEnabledAppearance
		= m_bBigAccent
		= m_bForcePopupDisabled
		= m_bForceEnabled
		= m_bForceNoLeftGradient
		= m_bCheck
		= m_bRadio
		= m_bIndeterminate
		= m_bXtraChecked
		= m_bNoCmdUI
		= m_bIconMode
		= false;
	m_pCbCmdDeliver = NULL;
	m_pCbXtraMarkState = NULL;
	m_bEnabled = true;
	m_hWndSpecCmdReceiver = NULL;
	m_pHelperInplaceEditWnd = NULL;
	m_pInplaceEditStr = NULL;
	m_pCbVerifyTextInput = NULL;
	m_pCbPutTextInputResult = NULL;
	m_pInplaceEditCbWndProc = NULL;
	m_pInplaceEditCbCookie = NULL;
	m_nInplaceEditWidth = 0;
	m_bAllowInplaceEditActivation = true;
	m_nLParam = 0L;
	m_bTempSelTextValid = false;
	m_sTempSelText.Empty();
	m_nMarginLeft
		= m_nMarginTop
		= m_nMarginRight
		= m_nMarginBottom
		 = -1;
	m_rcCacheBasic.SetRect( 0, 0, 0, 0 );
	m_rcCacheIndents.SetRect( 0, 0, 0, 0 );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_pWndKeyTipBasic = m_pWndKeyTipDD = NULL;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
}

CExtPopupMenuWnd::MENUITEMDATA::MENUITEMDATA(
	const CExtPopupMenuWnd::MENUITEMDATA & other
	)
	: m_cAccelChar( _T('\0') )
{
	m_pOwner = other.m_pOwner;
	// self constant type assertion
	ASSERT( TYPE_SEPARATOR==ID_SEPARATOR );
	m_pWndChild = NULL;
	m_nItemIndex = -100;
	m_nCmdID = TYPE_SEPARATOR;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_pCmdNode = NULL;
	m_pCustomKeyTipInfoBasic = NULL;
	m_pCustomKeyTipInfoDD = NULL;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_sItemText = m_sAccelText = m_sExtendedText = _T("");
	m_sizeItem.cx = m_sizeItem.cy = 0;
	m_nIconAreaWidth = 0;
	m_bSelected
		= m_bDisplayed
		= m_bForceDisplayed
		= m_bChildCombine
		= m_bToolButton
		= m_bToolWrap
		= m_bBold
		= m_bDefault
		= m_bEnabledAppearance
		= m_bBigAccent
		= m_bForcePopupDisabled
		= m_bForceEnabled
		= m_bForceNoLeftGradient
		= m_bCheck
		= m_bRadio
		= m_bIndeterminate
		= m_bXtraChecked
		= m_bNoCmdUI
		= m_bIconMode
		= false;
	m_pCbCmdDeliver = NULL;
	m_pCbXtraMarkState = NULL;
	m_bEnabled = true;
	m_hWndSpecCmdReceiver = NULL;
	m_pHelperInplaceEditWnd = NULL;
	m_pInplaceEditStr = NULL;
	m_pCbVerifyTextInput = NULL;
	m_pCbPutTextInputResult = NULL;
	m_pInplaceEditCbWndProc = NULL;
	m_pInplaceEditCbCookie = NULL;
	m_nInplaceEditWidth = 0;
	m_bAllowInplaceEditActivation = true;
	m_nLParam = 0L;
	m_bTempSelTextValid = false;
	m_sTempSelText.Empty();
	m_rcCacheBasic.SetRect( 0, 0, 0, 0 );
	m_rcCacheIndents.SetRect( 0, 0, 0, 0 );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_pWndKeyTipBasic = m_pWndKeyTipDD = NULL;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	AssignFromOther( other );
}

CExtPopupMenuWnd::MENUITEMDATA::~MENUITEMDATA()
{
	// DestroyPopup();
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
HDWP hPassiveModeDWP = NULL;
	KeyTipTrackingQuery( false, NULL, hPassiveModeDWP );
	if( m_pCustomKeyTipInfoBasic != NULL )
	{
		delete m_pCustomKeyTipInfoBasic;
		m_pCustomKeyTipInfoBasic = NULL;
	}
	if( m_pCustomKeyTipInfoDD != NULL )
	{
		delete m_pCustomKeyTipInfoDD;
		m_pCustomKeyTipInfoDD = NULL;
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtCustomizeCmdKeyTip * CExtPopupMenuWnd::MENUITEMDATA::KeyTipGetInfo(
	bool bBasic
	)
{
CExtCustomizeCmdTreeNode * pNode = GetCmdNode();
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		CExtCustomizeCmdKeyTip * pCmdKeyTip = pNode->CmdKeyTipGet( bBasic );
		return pCmdKeyTip;
	}
	if( bBasic )
	{
		if( m_pCustomKeyTipInfoBasic == NULL )
			m_pCustomKeyTipInfoBasic = new CExtCustomizeCmdKeyTip;
		return m_pCustomKeyTipInfoBasic;
	}
	else
	{
		if( m_pCustomKeyTipInfoDD == NULL )
			m_pCustomKeyTipInfoDD = new CExtCustomizeCmdKeyTip;
		return m_pCustomKeyTipInfoDD;
	}
}

CExtSafeString CExtPopupMenuWnd::MENUITEMDATA::KeyTipGetText(
	INT nIndent,
	bool bBasic
	)
{
CExtSafeString strKeyTipText( _T("") );
CExtCustomizeCmdKeyTip * pCmdKeyTip = KeyTipGetInfo( bBasic );
	if( pCmdKeyTip != NULL )
		strKeyTipText = pCmdKeyTip->GetTipText( nIndent );
	return strKeyTipText;
}

CWnd * CExtPopupMenuWnd::MENUITEMDATA::KeyTipGetParentWnd()
{
CExtPopupMenuWnd * pOwnerPopupMenuWnd = GetOwner();
	if( pOwnerPopupMenuWnd->GetSafeHwnd() == NULL )
		return NULL;
	ASSERT_VALID( pOwnerPopupMenuWnd );
	return pOwnerPopupMenuWnd;
}

CPoint CExtPopupMenuWnd::MENUITEMDATA::KeyTipGetGuideLines(
	bool bBasic
	)
{
CExtPopupMenuWnd * pOwnerPopupMenuWnd = GetOwner();
	ASSERT( pOwnerPopupMenuWnd->GetSafeHwnd() != NULL );
	ASSERT_VALID( pOwnerPopupMenuWnd );
CRect rc = GetCachedRect( false );
	pOwnerPopupMenuWnd->ClientToScreen( &rc );
	if( bBasic )
	{
		int nIconAreaWidth = GetIconAreaWidth();
		CPoint ptGuideLines(
			rc.left + nIconAreaWidth,
			rc.bottom 
			);
		return ptGuideLines;
	} 
	else
	{
		CPoint ptGuideLines(
			rc.right,
			rc.bottom 
			);
		return ptGuideLines;
	}
}

CExtPopupKeyTipWnd * CExtPopupMenuWnd::MENUITEMDATA::KeyTipGetWnd(
	bool bBasic
	)
{
#ifdef _DEBUG
	if( m_pWndKeyTipBasic != NULL )
	{
		ASSERT_VALID( m_pWndKeyTipBasic );
	}
	if( m_pWndKeyTipDD != NULL )
	{
		ASSERT_VALID( m_pWndKeyTipDD );
	}
#endif // _DEBUG
	return bBasic ? m_pWndKeyTipBasic : m_pWndKeyTipDD;
}

void CExtPopupMenuWnd::MENUITEMDATA::KeyTipTrackingQuery(
	bool bShow,
	CExtCustomizeCmdKeyTip * pKeyTipChain,
	HDWP & hPassiveModeDWP
	)
{
	if( bShow )
	{
		CExtPopupMenuWnd * pOwnerPopupMenuWnd = GetOwner();
		ASSERT( pOwnerPopupMenuWnd->GetSafeHwnd() != NULL );
		ASSERT_VALID( pOwnerPopupMenuWnd );
		INT nKTI, nIndent = pKeyTipChain->KeyCodeGetCount();
		for( nKTI = 0; nKTI < 2; nKTI++ )
		{
			bool bBasic = ( nKTI == 0 ) ? true : false;
			CExtPopupKeyTipWnd *& pWndKeyTip = bBasic ? m_pWndKeyTipBasic : m_pWndKeyTipDD;
			ASSERT_VALID( pKeyTipChain );
			CExtSafeString strKeyTipText = KeyTipGetText( nIndent, bBasic );
			if( strKeyTipText.IsEmpty() )
				continue;
			if( pWndKeyTip == NULL )
			{
				try
				{
					pWndKeyTip = new CExtPopupKeyTipWnd;
				}
				catch( CException * pException )
				{
					pException->Delete();
					continue;
				}
			} // if( pWndKeyTip == NULL )
#ifdef _DEBUG
			else
			{
				ASSERT_VALID( pWndKeyTip );
			} // else from if( pWndKeyTip == NULL )
#endif // _DEBUG
			CWnd * pWndKeyTipParent = KeyTipGetParentWnd();
			ASSERT_VALID( pWndKeyTipParent );
			ASSERT( pWndKeyTipParent->GetSafeHwnd() != NULL );
			CPoint ptGuideLines = KeyTipGetGuideLines( bBasic );
			if( ! pWndKeyTip->Create(
					pWndKeyTipParent,
					ptGuideLines,
					strKeyTipText,
					IsEnabled(),
					hPassiveModeDWP
					)
				)
			{
				delete pWndKeyTip;
				pWndKeyTip = NULL;
				continue;
			}
			pWndKeyTip->ShowWindow( SW_SHOWNOACTIVATE );
		} // for( nKTI = 0; nKTI < 2; nKTI++ )
	} // if( bShow )
	else
	{
		if( m_pWndKeyTipBasic != NULL )
		{
			m_pWndKeyTipBasic->Hide();
			delete m_pWndKeyTipBasic;
			m_pWndKeyTipBasic = NULL;
		}
		if( m_pWndKeyTipDD != NULL )
		{
			m_pWndKeyTipDD->Hide();
			delete m_pWndKeyTipDD;
			m_pWndKeyTipDD = NULL;
		}
	} // else from if( bShow )
}

bool CExtPopupMenuWnd::MENUITEMDATA::KeyTipTranslate(
	DWORD dwKeyCode,
	CExtCustomizeCmdKeyTip * pKeyTipChain
	)
{
	ASSERT_VALID( pKeyTipChain );
CExtPopupMenuWnd * pOwnerPopupMenuWnd = GetOwner();
	if( pOwnerPopupMenuWnd->GetSafeHwnd() == NULL )
		return false;
	ASSERT_VALID( pOwnerPopupMenuWnd );
	if( ! IsEnabled() )
		return false;
INT nKTI;
	for( nKTI = 0; nKTI < 2; nKTI++ )
	{
		bool bBasic = ( nKTI == 0 ) ? true : false;
		CExtCustomizeCmdKeyTip * pCmdKeyTip = KeyTipGetInfo( bBasic );
		if( pCmdKeyTip == NULL )
			continue;
		CExtPopupKeyTipWnd * pWndKeyTip = KeyTipGetWnd( bBasic );
		if(		pWndKeyTip->GetSafeHwnd() == NULL
			||	( pWndKeyTip->GetStyle() & WS_VISIBLE ) == 0
			)
			continue;
		if( ! pCmdKeyTip->CompareWidthChain( *pKeyTipChain ) )
			continue;
		INT nKeyCodeCountOwn = pCmdKeyTip->KeyCodeGetCount();
		INT nKeyCodeCountChain = pKeyTipChain->KeyCodeGetCount();
		ASSERT( nKeyCodeCountOwn > nKeyCodeCountChain );
		DWORD dwKeyCodeOwn = pCmdKeyTip->KeyCodeGetAt( nKeyCodeCountChain );
		if( dwKeyCodeOwn != dwKeyCode )
			continue;
		pKeyTipChain->KeyCodeAdd( dwKeyCode );
		nKeyCodeCountChain++;
		if( nKeyCodeCountOwn > nKeyCodeCountChain )
			return true;
		KeyTipInvokeAction( bBasic );
		return true;
	} // for( nKTI = 0; nKTI < 2; nKTI++ )
	return false;
}

void CExtPopupMenuWnd::MENUITEMDATA::KeyTipDisplay(
	CExtCustomizeCmdKeyTip & keyTipChain
	)
{
	ASSERT_VALID( (&keyTipChain) );
CExtPopupMenuWnd * pOwnerPopupMenuWnd = GetOwner();
	if( pOwnerPopupMenuWnd->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( pOwnerPopupMenuWnd );
//	if( ! IsEnabled() )
//		return;
INT nKTI;
	for( nKTI = 0; nKTI < 2; nKTI++ )
	{
		bool bBasic = ( nKTI == 0 ) ? true : false;
		CExtCustomizeCmdKeyTip * pCmdKeyTip = KeyTipGetInfo( bBasic );
		if( pCmdKeyTip == NULL )
			continue;
		CExtPopupKeyTipWnd * pWndKeyTip = KeyTipGetWnd( bBasic );
		if(		pWndKeyTip->GetSafeHwnd() == NULL
			||	( pWndKeyTip->GetStyle() & WS_VISIBLE ) == 0
			)
			continue;
		if( pCmdKeyTip->CompareWidthChain( keyTipChain ) )
			continue;
		pWndKeyTip->ShowWindow( SW_HIDE );
	} // for( nKTI = 0; nKTI < 2; nKTI++ )
}

bool CExtPopupMenuWnd::MENUITEMDATA::KeyTipInvokeAction(
	bool bBasic
	)
{
CExtPopupMenuWnd * pOwnerPopupMenuWnd = GetOwner();
	if( pOwnerPopupMenuWnd->GetSafeHwnd() == NULL )
		return false;
	ASSERT_VALID( pOwnerPopupMenuWnd );
bool bSeparatedDropDown = false;
const CExtCustomizeCmdTreeNode * pNode = GetCmdNode();
	if( pNode != NULL )
	{
		ASSERT_VALID( pNode );
		bSeparatedDropDown =
			(pNode->GetFlags() & __ECTN_TBB_SEPARATED_DROPDOWN)
				? true : false;
	} // if( pNode != NULL )
	if(		( ( ! bBasic ) || (! bSeparatedDropDown ) )
		&&	IsPopup()
		&&	( ! IsInplaceEdit() )
		)
	{
		INT nCurIndex = pOwnerPopupMenuWnd->ItemGetIndexOf( this );
		pOwnerPopupMenuWnd->_ItemFocusSet( nCurIndex, TRUE, TRUE );
		return true;
	} // if( ( ! bBasic ) ...
	if( ! bBasic )
		return false;
INT nCurIndex = pOwnerPopupMenuWnd->ItemGetIndexOf( this );
	pOwnerPopupMenuWnd->_ItemFocusSet( nCurIndex, FALSE, TRUE );
	if( IsInplaceEdit() )
	{
		CWnd * pWndInplace = GetInplaceEditPtr();
		if( pWndInplace != NULL )
		{
			ASSERT_VALID( pWndInplace );
			ASSERT( pWndInplace->GetSafeHwnd() != NULL );
			if( IsAllowInplaceEditActivation() )
			{
				if( (pWndInplace->GetStyle() & WS_VISIBLE) == 0 )
				{
					CRect rcInplaceEdit;
					pOwnerPopupMenuWnd->_GetItemRect( nCurIndex, rcInplaceEdit );
					rcInplaceEdit =
						AdjustInplaceEditRect(
							rcInplaceEdit,
							pOwnerPopupMenuWnd->OnQueryLayoutRTL()
							);
					pWndInplace->SetWindowPos(
						NULL,
						rcInplaceEdit.left, rcInplaceEdit.top,
						rcInplaceEdit.Width(), rcInplaceEdit.Height(),
						SWP_NOZORDER|SWP_NOOWNERZORDER
							|SWP_NOACTIVATE|SWP_SHOWWINDOW
						);
				} // if( (pWndInplace->GetStyle() & WS_VISIBLE) == 0 )
				pWndInplace->SetFocus();
				return true;
			} // if( IsAllowInplaceEditActivation() )
		} // if( pWndInplace != NULL )
	}
	pOwnerPopupMenuWnd->_EndSequence( GetCmdID(), pOwnerPopupMenuWnd->GetCmdReceiverHWND() );
	return true;
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtPopupMenuWnd * CExtPopupMenuWnd::MENUITEMDATA::GetOwner()
{
	return m_pOwner;
}

const CExtPopupMenuWnd * CExtPopupMenuWnd::MENUITEMDATA::GetOwner() const
{
	return m_pOwner;
}

void CExtPopupMenuWnd::MENUITEMDATA::SetOwner( CExtPopupMenuWnd * pOwner )
{
	m_pOwner = pOwner;
}

CExtPopupMenuWnd::MENUITEMDATA &
	CExtPopupMenuWnd::MENUITEMDATA::operator = (
		const CExtPopupMenuWnd::MENUITEMDATA & other
		)
{
	AssignFromOther( other );
	return (*this);
}

void CExtPopupMenuWnd::MENUITEMDATA::AssignFromOther(
	const CExtPopupMenuWnd::MENUITEMDATA & other
	)
{
	// self constant type assertion
	ASSERT( TYPE_SEPARATOR==ID_SEPARATOR );

LPVOID lpvThis = (LPVOID)this;
LPVOID lpvOther = (LPVOID)(&other);
	if( lpvThis == lpvOther )
		return;
	
	m_pOwner = other.m_pOwner;
	m_pWndChild = other.m_pWndChild;
	m_nItemIndex = other.m_nItemIndex;
	m_nCmdID = other.m_nCmdID;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_pCmdNode = other.m_pCmdNode;
	if( m_pCustomKeyTipInfoBasic != NULL )
	{
		delete m_pCustomKeyTipInfoBasic;
		m_pCustomKeyTipInfoBasic = NULL;
	}
	if( other.m_pCustomKeyTipInfoBasic != NULL )
		m_pCustomKeyTipInfoBasic = new CExtCustomizeCmdKeyTip( *other.m_pCustomKeyTipInfoBasic );
	if( m_pCustomKeyTipInfoDD != NULL )
	{
		delete m_pCustomKeyTipInfoDD;
		m_pCustomKeyTipInfoDD = NULL;
	}
	if( other.m_pCustomKeyTipInfoDD != NULL )
		m_pCustomKeyTipInfoDD = new CExtCustomizeCmdKeyTip( *other.m_pCustomKeyTipInfoDD );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_sItemText = other.m_sItemText;
	m_sExtendedText = other.m_sExtendedText;
	m_sAccelText = other.m_sAccelText;
	m_iconPopup = other.m_iconPopup;
	m_sizeItem = other.m_sizeItem;
	m_nIconAreaWidth = other.m_nIconAreaWidth;
	m_bSelected = other.m_bSelected;
	m_bDisplayed = other.m_bDisplayed;
	m_bForceDisplayed = other.m_bForceDisplayed;
	m_bChildCombine = other.m_bChildCombine;
	m_bToolButton = other.m_bToolButton;
	m_bToolWrap = other.m_bToolWrap;
	m_bBold = other.m_bBold;
	m_bDefault = other.m_bDefault;
	m_bEnabledAppearance = other.m_bEnabledAppearance;
	m_bBigAccent = other.m_bBigAccent;
	m_bForcePopupDisabled = other.m_bForcePopupDisabled;
	m_bForceEnabled = other.m_bForceEnabled;
	m_bForceNoLeftGradient = other.m_bForceNoLeftGradient;
	m_cAccelChar = other.m_cAccelChar;
	m_hWndSpecCmdReceiver = other.m_hWndSpecCmdReceiver;
	m_bCheck = other.m_bCheck;
	m_bRadio = other.m_bRadio;
	m_pCbCmdDeliver = other.m_pCbCmdDeliver;
	m_pCbXtraMarkState = other.m_pCbXtraMarkState;
	m_bXtraChecked = other.m_bXtraChecked;
	m_bNoCmdUI = other.m_bNoCmdUI;
	m_bIconMode = other.m_bIconMode;
	m_bEnabled = other.m_bEnabled;
	m_bIndeterminate = other.m_bIndeterminate;
	m_pHelperInplaceEditWnd = other.m_pHelperInplaceEditWnd;
	m_pInplaceEditStr = other.m_pInplaceEditStr;
	m_pCbVerifyTextInput = other.m_pCbVerifyTextInput;
	m_pCbPutTextInputResult = other.m_pCbPutTextInputResult;
	m_pInplaceEditCbWndProc = other.m_pInplaceEditCbWndProc;
	m_pInplaceEditCbCookie = other.m_pInplaceEditCbCookie;
	m_nInplaceEditWidth = other.m_nInplaceEditWidth;
	m_bAllowInplaceEditActivation = other.m_bAllowInplaceEditActivation;
	m_nLParam = other.m_nLParam;
	m_bTempSelTextValid = other.m_bTempSelTextValid;
	m_sTempSelText = other.m_sTempSelText;
	m_nMarginLeft = m_nMarginLeft;
	m_nMarginTop = m_nMarginTop;
	m_nMarginRight = m_nMarginRight;
	m_nMarginBottom = m_nMarginBottom;
	m_rcCacheBasic = other.m_rcCacheBasic;
	m_rcCacheIndents = other.m_rcCacheIndents;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
HDWP hPassiveModeDWP = NULL;
	KeyTipTrackingQuery( false, NULL, hPassiveModeDWP );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
}

void CExtPopupMenuWnd::MENUITEMDATA::SetOuterMargins(
	INT nMarginLeft, // = -1
	INT nMarginTop, // = -1
	INT nMarginRight, // = -1
	INT nMarginBottom // = -1
	)
{
	if( nMarginLeft >= 0 )
		m_nMarginLeft = nMarginLeft;
	if( nMarginTop >= 0 )
		m_nMarginTop = nMarginTop;
	if( nMarginRight >= 0 )
		m_nMarginRight = nMarginRight;
	if( nMarginBottom >= 0 )
		m_nMarginBottom = nMarginBottom;
}

void CExtPopupMenuWnd::MENUITEMDATA::SetOuterMargins(
	const RECT & rcMargins
	)
{
	SetOuterMargins(
		rcMargins.left,
		rcMargins.top,
		rcMargins.right,
		rcMargins.bottom
		);
}

void CExtPopupMenuWnd::MENUITEMDATA::GetOuterMargins(
	INT * p_nMarginLeft, // = NULL,
	INT * p_nMarginTop, // = NULL,
	INT * p_nMarginRight, // = NULL,
	INT * p_nMarginBottom // = NULL
	) const
{
	// returns default indent values, if indents are is not initialized yet
	if( p_nMarginLeft != NULL )
		(*p_nMarginLeft) = 
			(m_nMarginLeft == -1) ? 1 : m_nMarginLeft;
	if(	p_nMarginTop != NULL )
		(*p_nMarginTop) = 
			(m_nMarginTop == -1) ? 0 : m_nMarginTop;
	if(	p_nMarginRight != NULL )
		(*p_nMarginRight) = 
			(m_nMarginRight == -1) ? 1 : m_nMarginRight;
	if( p_nMarginBottom != NULL )
		(*p_nMarginBottom) = 
			(m_nMarginBottom == -1) ? 1 : m_nMarginBottom;
	
}

void CExtPopupMenuWnd::MENUITEMDATA::GetOuterMargins(
	RECT & rcMargins
	) const
{
INT nMarginLeft, nMarginTop, nMarginRight, nMarginBottom;
	GetOuterMargins(
		&nMarginLeft,
		&nMarginTop,
		&nMarginRight,
		&nMarginBottom
		);
	::SetRect( &rcMargins, nMarginLeft, nMarginTop, nMarginRight, nMarginBottom );
}

CRect CExtPopupMenuWnd::MENUITEMDATA::GetOuterMargins() const
{
INT nMarginLeft, nMarginTop, nMarginRight, nMarginBottom;
	GetOuterMargins(
		&nMarginLeft,
		&nMarginTop,
		&nMarginRight,
		&nMarginBottom
		);
CRect rcMargins;
	::SetRect( &rcMargins, nMarginLeft, nMarginTop, nMarginRight, nMarginBottom );
	return rcMargins;
}

int CExtPopupMenuWnd::MENUITEMDATA::GetMeasuredHeight()
{
INT nHeight = m_sizeItem.cy;
INT nMarginTop = 0, nMarginBottom = 0;
	GetOuterMargins(
		NULL,
		&nMarginTop,
		NULL,
		&nMarginBottom
		);
	nHeight += nMarginTop;
	nHeight += nMarginBottom;
	return nHeight;
}
int CExtPopupMenuWnd::MENUITEMDATA::GetMeasuredWidth()
{
INT nWidth = m_sizeItem.cx;
INT nMarginLeft = 0, nMarginRight = 0;
	GetOuterMargins(
		&nMarginLeft,
		NULL,
		&nMarginRight,
		NULL
		);
	nWidth += nMarginLeft;
	nWidth += nMarginRight;
	return nWidth;
}

CRect CExtPopupMenuWnd::MENUITEMDATA::AdjustInplaceEditRect(
	const RECT & rcItem,
	bool bRTL
	) const
{
CRect rcInplaceEdit( rcItem );
int nInplaceEditWidth = GetInplaceEditWidth();
	ASSERT( nInplaceEditWidth > 0 );
INT nDD = m_pOwner->PmBridge_GetPM()->GetDropDownButtonWidth( (CObject*)GetOwner() );
	if( bRTL )
	{
		rcInplaceEdit.DeflateRect( 1, 2, 1, 2 );
		rcInplaceEdit.right = rcInplaceEdit.left + nInplaceEditWidth;
		if( IsPopup() )
			rcInplaceEdit.right -= nDD;
	} // if( bRTL )
	else
	{
		rcInplaceEdit.DeflateRect( 1, 2, 1, 2 );
		rcInplaceEdit.left = rcInplaceEdit.right - nInplaceEditWidth;
		if( IsPopup() )
			rcInplaceEdit.right -= nDD;
	} // else from if( bRTL )
	return rcInplaceEdit;
}

UINT CExtPopupMenuWnd::MENUITEMDATA::GetCmdID() const
{
//	if(		IsSeparator()
//		||	IsInplaceEdit()
//		)
//		return ( (UINT) (IDC_STATIC) );
	if( IsPopup() )
	{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		CExtCustomizeCmdTreeNode * pNode =
			( const_cast < MENUITEMDATA * > ( this ) )
				-> GetCmdNode();
		if( pNode != NULL )
		{
			ASSERT_VALID( pNode );
			if( ( pNode->GetFlags() & __ECTN_TBB_COLOR ) != 0 )
				return 0;
//			if( ( pNode->GetFlags() & __ECTN_TBB_UNDO_REDO ) != 0 )
//				return pNode->GetCmdID();
//			if( ( pNode->GetFlags() & __ECTN_TBB_SEPARATED_DROPDOWN ) == 0 )
//				return 0;
//			CExtCmdItem * pCmdItem =
//				g_CmdManager->CmdGetPtr(
//					g_CmdManager->ProfileNameFromWnd( GetCmdReceiver() ),
//					pNode->GetCmdID()
//					);
//			if( pCmdItem != NULL )
//				return pCmdItem->m_nCmdID;
			if( ( pNode->GetFlags() & (__ECTN_TBB_UNDO_REDO|__ECTN_TBB_SEPARATED_DROPDOWN) ) != 0 )
				return pNode->GetCmdID();
		} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		return ( (UINT) (IDC_STATIC) );
	} // if( IsPopup() )
	return m_nCmdID;
}

UINT CExtPopupMenuWnd::MENUITEMDATA::SetCmdID( UINT nCmdID )
{
	// should be used only without references to command manager
	ASSERT( ! IsPopup() ); // should not be applied for popups
UINT nOldCmdID = m_nCmdID;
	m_nCmdID = nCmdID;
	return nOldCmdID;
}

bool CExtPopupMenuWnd::MENUITEMDATA::IsExecutableCmdID() const
{
	if(		IsSeparator()
		||	IsInplaceEdit()
		)
		return false;
	if( IsPopup() )
	{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		CExtCustomizeCmdTreeNode * pNode =
			( const_cast < MENUITEMDATA * > ( this ) )
				-> GetCmdNode();
		if( pNode == NULL )
			return false;
		ASSERT_VALID( pNode );
		if( ( pNode->GetFlags() & __ECTN_TBB_COLOR ) != 0 )
			return false;
//		if( ( pNode->GetFlags() & __ECTN_TBB_UNDO_REDO ) != 0 )
//			return false;
//		if( ( pNode->GetFlags() & __ECTN_TBB_SEPARATED_DROPDOWN ) == 0 )
//			return false;
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd( GetCmdReceiver() ),
				pNode->GetCmdID()
				);
		if(		pCmdItem != NULL
			&&	pCmdItem->StateIsSeparatedDD()
			)
			return true;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		return false;
	} // if( IsPopup() )
	return true;
}

bool CExtPopupMenuWnd::MENUITEMDATA::SetPopupText( __EXT_MFC_SAFE_LPCTSTR strText )
{
	SetText( strText );
	int nPos = m_sItemText.Find(_T('\t'));
	if( nPos >= 0 )
	{
		CExtSafeString sLeft = m_sItemText.Left(nPos);
		CExtSafeString sRight = m_sItemText.Right(m_sItemText.GetLength()-nPos-1);
		m_sItemText = sLeft;
		m_sAccelText = sRight;
	}
	return true;
}

bool CExtPopupMenuWnd::MENUITEMDATA::SetPopupAccelText( __EXT_MFC_SAFE_LPCTSTR strText ) // NULL if empty
{
	m_sAccelText = (strText == NULL) ? _T("") : strText;
	return true;
}

__EXT_MFC_SAFE_LPCTSTR CExtPopupMenuWnd::MENUITEMDATA::GetText() const
{
	if( IsSeparator() )
		return _T("");
	return m_sItemText;
}

void CExtPopupMenuWnd::MENUITEMDATA::SetText( __EXT_MFC_SAFE_LPCTSTR strText )
{
	if( IsSeparator() )
		return;
	m_sItemText = (strText != NULL) ? strText : _T("");
}

__EXT_MFC_SAFE_LPCTSTR CExtPopupMenuWnd::MENUITEMDATA::GetAccelText() const
{
	if( IsSeparator() )
		return _T("");
	return m_sAccelText;
}

void CExtPopupMenuWnd::MENUITEMDATA::SetAccelText( __EXT_MFC_SAFE_LPCTSTR strText )
{
	if( IsSeparator() )
		return;
	m_sAccelText = (strText != NULL) ? strText : _T("");
}

void CExtPopupMenuWnd::MENUITEMDATA::AccelCharInit()
{
	m_cAccelChar = _T('\0');
CExtSafeString sDisplayText( GetText() );
	if( sDisplayText.IsEmpty() )
		return;
int nIndex = sDisplayText.Find( _T('&') );
	if( nIndex + 1 == sDisplayText.GetLength() )
	{
		TRACE(_T("warning : & is bad position, access key is invalid.\n"));
		return;
	}
	if( nIndex < 0 )
	{
		if( ! CExtPopupMenuWnd::g_bAllowNonAccelPositioning )
			return;
		m_cAccelChar = sDisplayText[0];
	}
	else
		m_cAccelChar = sDisplayText[nIndex + 1]; // -1 + 1 = 0; it's ok
	TCHAR szChar[2] = { TCHAR(m_cAccelChar), _T('\0') };
	::CharUpper( szChar );
	m_cAccelChar = szChar[0];
}

__EXT_MFC_SAFE_TCHAR CExtPopupMenuWnd::MENUITEMDATA::AccelCharGet() const
{
	return __EXT_MFC_SAFE_TCHAR( TCHAR(m_cAccelChar) );
}

bool CExtPopupMenuWnd::MENUITEMDATA::AccelCharIsSet() const
{
	return (m_cAccelChar == _T('\0') ) ? false : true;
}

CExtCmdIcon * CExtPopupMenuWnd::MENUITEMDATA::GetIconPtr()
{
	if( IsSeparator() )
		return NULL;
//	if( (! m_iconPopup.IsEmpty() ) || IsNoCmdUI() )
//		return (m_iconPopup.IsEmpty()) ? NULL : (&m_iconPopup);
	if( ! m_iconPopup.IsEmpty() )
		return (&m_iconPopup);
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = GetCmdNode();
	if( pNode != NULL )
		return
			pNode->GetIconPtrInMenu(
				g_CmdManager->CmdGetPtr(
					g_CmdManager->ProfileNameFromWnd( GetCmdReceiver() ),
					pNode->GetCmdID() //m_nCmdID
					)
				);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( IsPopup() )
		return NULL;
	return
		g_CmdManager->CmdGetIconPtr(
			g_CmdManager->ProfileNameFromWnd( GetCmdReceiver() ),
			m_nCmdID
			);
}

const CExtCmdIcon * CExtPopupMenuWnd::MENUITEMDATA::GetIconPtr() const
{
	return
		( const_cast < CExtPopupMenuWnd::MENUITEMDATA * > ( this ) )
		-> GetIconPtr();
}

CExtCmdIcon & CExtPopupMenuWnd::MENUITEMDATA::GetIcon()
{
CExtCmdIcon * pIcon = GetIconPtr();
	if( pIcon != NULL )
		return (*pIcon);
static CExtCmdIcon g_EmptyIcon;
	return g_EmptyIcon;
}

const CExtCmdIcon & CExtPopupMenuWnd::MENUITEMDATA::GetIcon() const
{
	return
		( const_cast < CExtPopupMenuWnd::MENUITEMDATA * > ( this ) )
		-> GetIcon();
}


bool CExtPopupMenuWnd::MENUITEMDATA::UpdateCmdManagerCommand(
	CExtCmdItem * pCmdItem,
	int nItemIndex
	)
{
	ASSERT( pCmdItem != NULL );
	ASSERT( !IsPopup() );
	ASSERT( m_pWndChild == NULL );
	m_sItemText.Empty();
	m_sAccelText.Empty();
	m_nCmdID = pCmdItem->m_nCmdID;
	if( ! CExtCmdManager::IsCommand(m_nCmdID) )
	{
		m_nCmdID = TYPE_SEPARATOR;
		m_nItemIndex = -100;
		ASSERT( FALSE );
		return false;
	}
	if( nItemIndex >= 0 )
		m_nItemIndex = nItemIndex;
	SetText( pCmdItem->m_sMenuText );
	SetAccelText( pCmdItem->m_sAccelText );
	AccelCharInit();
	MeasureItem( NULL );
	return true;
}

CExtCmdItem * CExtPopupMenuWnd::MENUITEMDATA::GetCmd() const
{
	if( IsSeparator() || IsNoCmdUI() )
		return NULL;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	CExtCustomizeCmdTreeNode * pNode =
		( const_cast < MENUITEMDATA * > ( this ) )
			-> GetCmdNode();
	if(		pNode != NULL
		&&	(	( pNode->GetFlags() & (__ECTN_TBB_COLOR|__ECTN_TBB_SEPARATED_DROPDOWN) ) != 0
			||	( pNode->GetFlags() & (__ECTN_TBB_UNDO_REDO|__ECTN_TBB_SEPARATED_DROPDOWN) ) != 0
			||	IsInplaceEdit()
			)
		)
	{
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd( GetCmdReceiver() ),
				pNode->GetCmdID(
					( ( pNode->GetFlags() & __ECTN_TBB_SEPARATED_DROPDOWN ) != 0 )
						? true : false
					)
				);
//		ASSERT( pCmdItem != NULL );
		return pCmdItem;
	} // if( pNode != NULL ...
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( IsPopup() )
		return NULL;
	if(		m_pOwner != NULL
		&&	m_pOwner->_IsRibbonMode()
		&&	m_pOwner->m_hWndNotifyMenuClosed != NULL
		&&	::IsWindow( m_pOwner->m_hWndNotifyMenuClosed )
		)
	{
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd( m_pOwner->m_hWndNotifyMenuClosed ),
				m_nCmdID
				);
		//ASSERT( pCmdItem != NULL );
		return pCmdItem;
	}
	else
	{
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd( GetCmdReceiver() ),
				m_nCmdID
				);
		//ASSERT( pCmdItem != NULL );
		return pCmdItem;
	}
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtCustomizeCmdTreeNode * CExtPopupMenuWnd::MENUITEMDATA::GetCmdNode()
{
#ifdef _DEBUG
	if( m_pCmdNode != NULL )
	{
		ASSERT_VALID( m_pCmdNode );
	}
#endif // _DEBUG
	return m_pCmdNode;
}

void CExtPopupMenuWnd::MENUITEMDATA::SetCmdNode( CExtCustomizeCmdTreeNode * pNode )
{
	m_pCmdNode = pNode;
#ifdef _DEBUG
	if( m_pCmdNode != NULL )
	{
		ASSERT_VALID( m_pCmdNode );
	}
#endif // _DEBUG
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

void CExtPopupMenuWnd::MENUITEMDATA::MeasureItem(
	CExtPaintManager * pPM
	)
{
	if( pPM == NULL )
		pPM = g_PaintManager.GetPM();
	ASSERT_VALID( pPM );
	if( IsToolButton() )
		m_nIconAreaWidth = 0;
	else
	{
		m_nIconAreaWidth = __EXT_MENU_ICON_CX;
		m_nIconAreaWidth = pPM->UiScalingDo( m_nIconAreaWidth, CExtPaintManager::__EUIST_Y );
	}
	if( pPM == NULL )
		pPM = g_PaintManager.GetPM();
	if( IsSeparator() )
	{
		m_sizeItem.cx = 0;
		m_sizeItem.cy = pPM->GetSeparatorHeight();
		return;
	}
bool bBold = IsBold();
bool bForceNoText = false;
CExtCmdIcon * pIcon = GetIconPtr();
	if( pIcon != NULL && pIcon->IsEmpty() )
		pIcon = NULL;
	if( pIcon == NULL || ( pIcon != NULL && (! IsIconMode() ) ) )
	{ // if non displaying text in icon mode
		AccelCharInit();
		CExtSafeString sMeasureText( GetText() );
		if( ! ( IsInplaceEdit() || IsToolButton() ) )
		{
			if(		m_pOwner == NULL
				||	( m_pOwner->TrackFlagsGet() & TPMX_HIDE_KEYBOARD_ACCELERATORS ) == 0
				)
				sMeasureText += (LPCTSTR)GetAccelText();
		}
		sMeasureText.Replace( _T("&"), _T("") );
		CWindowDC dc( NULL );
		CFont * pOldFont =
			dc.SelectObject(
				bBold
					? (&(pPM->m_FontBold))
					: (&(pPM->m_FontNormal))
			);
		//ASSERT( pOldFont != NULL );
		CRect rect( 0, 0, 0, 0 );
		CExtRichContentLayout::stat_DrawText(
			dc.GetSafeHdc(),
			LPCTSTR(sMeasureText), sMeasureText.GetLength(),
			&rect,
			DT_CALCRECT|DT_SINGLELINE|DT_LEFT, 0
			);
		m_sizeItem.cx = rect.Width();
		m_sizeItem.cy = rect.Height();

		if( m_sExtendedText.GetLength() > 0 )
		{
			CRect rcExtended( 0, 0, 0, 0 );
			CExtRichContentLayout::stat_DrawText(
				dc.GetSafeHdc(),
				LPCTSTR(m_sExtendedText), m_sExtendedText.GetLength(),
				&rcExtended,
				DT_CALCRECT|DT_TOP|DT_LEFT, 0
				);
			CSize _sizeExtended = rcExtended.Size();
			m_sizeItem.cx = max( m_sizeItem.cx, _sizeExtended.cx );
			m_sizeItem.cy += _sizeExtended.cy + __EXT_MENU_IMG_RECT_OUT_GAP;
		}
		else if( IsBigAccent() )
			m_sizeItem.cy += rect.Height();

		m_sizeItem.cx += __EXT_MENU_IMG_RECT_OUT_GAP*2;
		m_sizeItem.cy += __EXT_MENU_IMG_RECT_OUT_GAP*2;

		dc.SelectObject( pOldFont );
	} // if non displaying text in icon mode
	else
	{
		bForceNoText = true;
		m_sizeItem.cx = m_sizeItem.cy = 0;
	}
	if( bBold )
		m_sizeItem.cx += 5;

CSize sizeIcon( 0, 0 );
int nIconHeightMetric = 0;
	if( ! ( IsToolButton() && IsInplaceEdit() ) )
	{
		if( pIcon != NULL )
		{
			sizeIcon = pIcon->GetSize();
			sizeIcon.cx = pPM->UiScalingDo( sizeIcon.cx, CExtPaintManager::__EUIST_X );
			sizeIcon.cy = pPM->UiScalingDo( sizeIcon.cy, CExtPaintManager::__EUIST_Y );
			if( g_bMenuLargeIcons )
			{
				sizeIcon.cx *= 2;
				sizeIcon.cy *= 2;
			} // if( g_bMenuLargeIcons )
			CSize _sizeMinIconArea(
				pPM->UiScalingDo( __EXT_MENU_ICON_CX, CExtPaintManager::__EUIST_X ),
				pPM->UiScalingDo( __EXT_MENU_ICON_CY, CExtPaintManager::__EUIST_Y )
				);
			if( sizeIcon.cx < _sizeMinIconArea.cx )
				sizeIcon.cx = _sizeMinIconArea.cx;
			if( sizeIcon.cy < _sizeMinIconArea.cy )
				sizeIcon.cy = _sizeMinIconArea.cy;
			INT nAdjust = pPM->UiScalingDo( 18, CExtPaintManager::__EUIST_X );
			if(	sizeIcon.cx <= nAdjust && sizeIcon.cy < sizeIcon.cx )
				sizeIcon.cy = sizeIcon.cx;
			if( ! IsToolButton() )
			{
				INT _cx = pPM->UiScalingDo( sizeIcon.cx, CExtPaintManager::__EUIST_X );
				m_nIconAreaWidth = max( m_nIconAreaWidth, _cx );
			}
			else
				m_sizeItem.cx +=
					sizeIcon.cx +
					pPM->UiScalingDo( __EXT_MENU_IMG_RECT_OUT_GAP*2, CExtPaintManager::__EUIST_X )
					;
			nIconHeightMetric =
					sizeIcon.cy +
					pPM->UiScalingDo( __EXT_MENU_IMG_RECT_OUT_GAP*2, CExtPaintManager::__EUIST_Y )
					;
			m_sizeItem.cy = max( m_sizeItem.cy, nIconHeightMetric );
		} // if( pIcon != NULL )
	}

	m_sizeItem.cy = max( m_sizeItem.cy, nIconHeightMetric );
INT nMenuMinHeight = pPM->UiScalingDo( __EXT_MENU_MIN_HEIGHT, CExtPaintManager::__EUIST_Y );
	if( m_sizeItem.cy < nMenuMinHeight )
		m_sizeItem.cy = nMenuMinHeight;
	if( IsToolButton() )
	{
		if( sizeIcon.cy == 0 )
			m_sizeItem.cy -= 2;
	}
	else
	{
		INT _cx = pPM->UiScalingDo( sizeIcon.cx, CExtPaintManager::__EUIST_X );
		m_nIconAreaWidth = max( _cx, m_sizeItem.cy );
		if( IsExtraMark() )
		{
			m_nIconAreaWidth *= 2;
			m_nIconAreaWidth += pPM->UiScalingDo( 4, CExtPaintManager::__EUIST_Y );;
		} // if( IsExtraMark() )
		m_nIconAreaWidth += pPM->UiScalingDo( 2, CExtPaintManager::__EUIST_Y );; // + 2.28
		m_sizeItem.cx += __EXT_MENU_GAP * 7;
		m_sizeItem.cx += 5; // + 2.28
	}

	if( IsInplaceEdit() )
	{
		m_sizeItem.cx += m_nInplaceEditWidth;
		if( ! IsToolButton() )
		{
			m_sizeItem.cx += + 2 + 10;
			if( g_bMenuLargeIcons )
				m_sizeItem.cx += m_nIconAreaWidth;
		} // if( ! IsToolButton() )
		if( ! g_bMenuLargeIcons )
			m_sizeItem.cy -= 2; // + 2.28
	} // if( IsInplaceEdit() )
	else if( IsToolButton() )
	{
		if( IsPopup() )
		{
			//m_sizeItem.cx += __DROPDOWN_ARROW_GAP * 2 + __EXT_MENU_GAP * 2 + 5;
			m_sizeItem.cx += __DROPDOWN_ARROW_GAP * 2 + __EXT_MENU_GAP + 3;
			if( ! bForceNoText )
				m_sizeItem.cx += __EXT_MENU_GAP;
		}
		//m_sizeItem.cx += __EXT_MENU_TOOLBTN_GAP * 2;
		m_sizeItem.cx += __EXT_MENU_TOOLBTN_GAP;
//		if( bForceNoText )
//		{
//			m_sizeItem.cx += 3;
//			m_sizeItem.cy += 3;
//		}
	} // if( IsToolButton() )
	else
	{
		m_sizeItem.cx +=
			m_nIconAreaWidth // sizeIcon.cx
			+ __EXT_MENU_POPUP_ARROW_AREA_DX;
	} // else from if( IsToolButton() )

	if( IsToolButton() )
	{
		CExtPopupMenuWnd * pOwner = GetOwner();
		ASSERT_VALID( pOwner );
		CSize _size = pPM->Toolbar_GetMinButtonSize( pOwner );
		ASSERT( _size.cx >= 0 && _size.cy >= 0 );
//		if( m_sizeItem.cx < _size.cx )
//			m_sizeItem.cx = _size.cx;
		if( m_sizeItem.cy < _size.cy )
			m_sizeItem.cy = _size.cy;
		if( m_sizeItem.cx < _size.cx )
			m_sizeItem.cx = _size.cx;
	} // if( IsToolButton() )
}

bool CExtPopupMenuWnd::MENUITEMDATA::UpdateFromMenu(
	HWND hWndCmdProfileInfo,
	CMenu * pTrackMenu,
	int nItemIndex,
	bool bNoRefToCmdMngr // = false
	)
{
	ASSERT( pTrackMenu != NULL );
	ASSERT( pTrackMenu->GetSafeHmenu() != NULL );
	
#ifdef _DEBUG
	int	nMenuItemCount = pTrackMenu->GetMenuItemCount();
	ASSERT( nMenuItemCount > 0 );
	ASSERT( nItemIndex >= 0 && nItemIndex<nMenuItemCount );
#endif // _DEBUG

MENUITEMINFO _mii;
	::memset( &_mii, 0, sizeof(MENUITEMINFO) );
	_mii.cbSize = sizeof(MENUITEMINFO);
	_mii.fMask = MIIM_CHECKMARKS|MIIM_DATA|MIIM_ID|MIIM_STATE|MIIM_SUBMENU|MIIM_TYPE;
//__EXT_MFC_MENUITEMINFO_V5 _mii;
//	::memset( &_mii, 0, sizeof(__EXT_MFC_MENUITEMINFO_V5) );
//	_mii.cbSize = /*g_PaintManager.m_bIsWin2000orLater ? sizeof(__EXT_MFC_MENUITEMINFO_V5) :*/ sizeof(MENUITEMINFO);
//	_mii.fMask = MIIM_CHECKMARKS|MIIM_DATA|MIIM_ID|MIIM_STATE|MIIM_SUBMENU|MIIM_TYPE;
//	if( g_PaintManager.m_bIsWin2000orLater )
//		_mii.fMask |= __EXT_MFC_MIIM_BITMAP;

	_mii.cch = __MAX_UI_ITEM_TEXT;
CExtSafeString _sItemText;
	_mii.dwTypeData = _sItemText.GetBuffer(__MAX_UI_ITEM_TEXT);
	ASSERT( _mii.dwTypeData != NULL );
	if( _mii.dwTypeData == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	if( ! pTrackMenu->GetMenuItemInfo( nItemIndex, (LPMENUITEMINFO)(&_mii), TRUE ) )
	{
//		if( g_PaintManager.m_bIsWin2000orLater )
//		{
//			_mii.cbSize = sizeof(MENUITEMINFO);
//			_mii.fMask &= ~(__EXT_MFC_MIIM_BITMAP);
//			if( ! pTrackMenu->GetMenuItemInfo( nItemIndex, (LPMENUITEMINFO)(&_mii), TRUE ) )
//			{
//				_sItemText.ReleaseBuffer();
//				ASSERT( FALSE );
//				return false;
//			}
//		}
//		else
//		{
			_sItemText.ReleaseBuffer();
			ASSERT( FALSE );
			return false;
//		}
	}
	_sItemText.ReleaseBuffer();

//CBitmap bmpExtractedFromMenuItem;
//	if(		( _mii.fMask & (__EXT_MFC_MIIM_BITMAP) ) != 0
//		&&	_mii.hbmpItem != NULL
//		&&	_mii.hbmpItem != __EXT_MFC_HBMMENU_CALLBACK
//		&&	_mii.hbmpItem != __EXT_MFC_HBMMENU_SYSTEM
//		&&	_mii.hbmpItem != __EXT_MFC_HBMMENU_MBAR_RESTORE
//		&&	_mii.hbmpItem != __EXT_MFC_HBMMENU_MBAR_MINIMIZE
//		&&	_mii.hbmpItem != __EXT_MFC_HBMMENU_MBAR_CLOSE
//		&&	_mii.hbmpItem != __EXT_MFC_HBMMENU_MBAR_CLOSE_D
//		&&	_mii.hbmpItem != __EXT_MFC_HBMMENU_MBAR_MINIMIZE_D
//		&&	_mii.hbmpItem != __EXT_MFC_HBMMENU_POPUP_CLOSE
//		&&	_mii.hbmpItem != __EXT_MFC_HBMMENU_POPUP_RESTORE
//		&&	_mii.hbmpItem != __EXT_MFC_HBMMENU_POPUP_MAXIMIZE
//		&&	_mii.hbmpItem != __EXT_MFC_HBMMENU_POPUP_MINIMIZE
//		)
//		bmpExtractedFromMenuItem.Attach( _mii.hbmpItem );
//	_mii.hbmpItem = NULL;
//	_mii.fMask &= ~(__EXT_MFC_MIIM_BITMAP);

	m_nItemIndex = nItemIndex;
	if( (_mii.fType&MFT_SEPARATOR) != 0 )
	{
		m_sItemText.Empty();
		m_nCmdID = TYPE_SEPARATOR;
		MeasureItem( NULL );
		return true;
	}

	SetBold( ( (_mii.fState&MFS_DEFAULT) != 0 ) ? true : false );

	if( _mii.hSubMenu != NULL )
	{
		if( ! ConstructPopup() )
		{
			ASSERT( FALSE );
			return false;
		}
		m_nCmdID = (UINT)TYPE_POPUP;
		SetPopupText( _sItemText );
		MeasureItem( NULL );
		return true;
	}

//	m_sItemText.Empty();
	if( ! CExtCmdManager::IsCommand(_mii.wID) )
	{
		ASSERT( FALSE );
		return false;
	}
	m_nCmdID = _mii.wID;

	if( bNoRefToCmdMngr )
	{
		// use text as is
		SetNoCmdUI( true );
		Enable( ( (_mii.fState&MFS_DISABLED) != 0 ) ? false : true );
		Check( ( (_mii.fState&MFS_CHECKED) != 0 ) ? true : false );
	} // if( bNoRefToCmdMngr )
	else
	{
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd( hWndCmdProfileInfo ),
				m_nCmdID
				);
		ASSERT( pCmdItem != NULL );
		if( pCmdItem == NULL )
			return false;
		//	if( _sItemText.IsEmpty() )
		//		_sItemText = pCmdItem->m_sMenuText;
		//	if( _sItemText.IsEmpty() )
		//		_sItemText = pCmdItem->m_sToolbarText;
		if( ! pCmdItem->m_sMenuText.IsEmpty() )
			_sItemText = pCmdItem->m_sMenuText;
		else if( ! pCmdItem->m_sToolbarText.IsEmpty() )
			_sItemText = pCmdItem->m_sToolbarText;
		SetAccelText( pCmdItem->m_sAccelText );
	} // else from if( bNoRefToCmdMngr )
	SetPopupText( _sItemText );
	MeasureItem( NULL );
	return true;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

bool CExtPopupMenuWnd::MENUITEMDATA::stat_CbVerifyTextInput(
	CInPlaceEditWnd & wndEdit,
	CExtCustomizeCmdTreeNode * pNode,
	__EXT_MFC_SAFE_LPCTSTR sTextOld,
	__EXT_MFC_SAFE_LPCTSTR sTextNew
	)
{
	ASSERT_VALID( pNode );
	ASSERT( sTextOld != NULL );
	ASSERT( sTextNew != NULL );
	ASSERT( wndEdit.GetSafeHwnd() != NULL );
CExtCustomizeSite * pSite = CExtCustomizeSite::GetCustomizeSite( wndEdit.GetSafeHwnd() );
	if( pSite == NULL )
		return true;
	return
		pSite->OnTextFieldVerify(
			NULL,
			pNode,
			sTextOld,
			sTextNew
			);
}

void CExtPopupMenuWnd::MENUITEMDATA::stat_CbPutTextInputResult(
	CInPlaceEditWnd & wndEdit,
	CExtCustomizeCmdTreeNode * pNode,
	__EXT_MFC_SAFE_LPCTSTR sTextNew
	)
{
	ASSERT_VALID( pNode );
	ASSERT( sTextNew != NULL );
	ASSERT( wndEdit.GetSafeHwnd() != NULL );
CExtCustomizeSite * pSite = CExtCustomizeSite::GetCustomizeSite( wndEdit.GetSafeHwnd() );
	if( pSite == NULL )
		return;
	pSite->OnTextFieldInplaceTextSet(
		NULL,
		pNode,
		pNode->m_sDefInplaceEditBuffer,
		sTextNew
		);
}

bool CExtPopupMenuWnd::MENUITEMDATA::stat_CbInplaceEditWndProc(
	LRESULT & lResult,
	UINT message,
	WPARAM wParam,
	LPARAM lParam,
	CEdit & wndEdit,
	CExtCustomizeCmdTreeNode * pNode
	)
{
	if( message == CExtCustomizeSite::g_nMsgQueryCustomizeSite )
		return false;
	if( (wndEdit.GetStyle() & WS_VISIBLE) == 0 )
		return false;
	ASSERT_VALID( pNode );
	ASSERT( wndEdit.GetSafeHwnd() != NULL );
CExtCustomizeSite * pSite = CExtCustomizeSite::GetCustomizeSite( wndEdit.GetSafeHwnd() );
	if( pSite == NULL )
		return false;
	return
		pSite->OnTextFieldWndProcHook(
			lResult,
			message,
			wParam,
			lParam,
			wndEdit,
			NULL,
			pNode
			);
}

#ifndef __EXT_MFC_NO_BUILTIN_DATEFIELD

bool CExtPopupMenuWnd::MENUITEMDATA::stat_CbInitDatePickerContent(
	CExtDatePickerWnd & wndDatePicker,
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( pNode );
	ASSERT( wndDatePicker.GetSafeHwnd() != NULL );
CExtCustomizeSite * pSite = CExtCustomizeSite::GetCustomizeSite( wndDatePicker.GetSafeHwnd() );
	if( pSite == NULL )
		return false;
	return
		pSite->OnPopupDatePickerInitContent(
			NULL,
			pNode,
			wndDatePicker
			);
}

bool CExtPopupMenuWnd::MENUITEMDATA::stat_CbDatePickerSelection(
	LPVOID pSelectionNotification,
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( pNode );
const CExtDatePickerWnd::SELECTION_NOTIFICATION * pSN =
		CExtDatePickerWnd::SELECTION_NOTIFICATION::
			FromWPARAM( (WPARAM)pSelectionNotification );
	ASSERT( pSN != NULL );
	if( pSN == NULL )
		return false;
CExtCustomizeSite * pSite = CExtCustomizeSite::GetCustomizeSite( pSN->m_hWndDatePicker ); 
	if( pSite == NULL )
		return false;
	return
		pSite->OnPopupDatePickerSelChange(
			NULL,
			pNode,
			pSelectionNotification
			);
}

#endif // __EXT_MFC_NO_BUILTIN_DATEFIELD

#ifndef __EXT_MFC_NO_UNDO_REDO_POPUP

bool CExtPopupMenuWnd::MENUITEMDATA::stat_CbFormatCaption(
	CExtSafeString & strCaption,
	CExtPopupUndoRedoMenuWnd * pUndoRedoPopupMenuWnd,
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( pNode );
	if( pUndoRedoPopupMenuWnd == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	ASSERT_VALID( pUndoRedoPopupMenuWnd );
CExtCustomizeSite * pSite = CExtCustomizeSite::GetCustomizeSite( pUndoRedoPopupMenuWnd->m_hWnd ); 
	if( pSite == NULL )
		return false;
	return
		pSite->OnPopupUndoRedoFormatCaption(
			NULL,
			pNode,
			pUndoRedoPopupMenuWnd,
			strCaption
			);
}

#endif // __EXT_MFC_NO_UNDO_REDO_POPUP

bool CExtPopupMenuWnd::MENUITEMDATA::stat_CbInitListBoxContent(
	CExtPopupInplaceListBox & wndListBox,
	CExtCustomizeCmdTreeNode * pNode
	)
{
	ASSERT_VALID( pNode );
	ASSERT( wndListBox.GetSafeHwnd() != NULL );
CExtCustomizeSite * pSite = CExtCustomizeSite::GetCustomizeSite( wndListBox.GetSafeHwnd() );
	if( pSite == NULL )
		return false;
	return
		pSite->OnPopupListBoxInitContent(
			NULL,
			pNode,
			wndListBox
			);
}

bool CExtPopupMenuWnd::MENUITEMDATA::stat_CbListBoxSelection(
	CExtPopupInplaceListBox & wndListBox,
	CExtCustomizeCmdTreeNode * pNode,
	int eSAT // CExtPopupInplaceListBox::e_sel_action_t
	)
{
	ASSERT_VALID( pNode );
	ASSERT( wndListBox.GetSafeHwnd() != NULL );
CExtCustomizeSite * pSite = CExtCustomizeSite::GetCustomizeSite( wndListBox.GetSafeHwnd() );
	if( pSite == NULL )
		return false;
	switch( ((CExtPopupInplaceListBox::e_sel_action_t)eSAT) )
	{
	case CExtPopupInplaceListBox::__SAT_SELCHANGE:
		return
			pSite->OnPopupListBoxSelChange(
				NULL,
				pNode,
				wndListBox
				);
	case CExtPopupInplaceListBox::__SAT_SELENDOK:
		return
			pSite->OnPopupListBoxSelEndOK(
				NULL,
				pNode,
				wndListBox
				);
	case CExtPopupInplaceListBox::__SAT_SELENDCANCEL:
		return
			pSite->OnPopupListBoxSelEndCancel(
				NULL,
				pNode,
				wndListBox
				);
	case CExtPopupInplaceListBox::__SAT_CLOSE:
		return
			pSite->OnPopupListBoxClose(
				NULL,
				pNode,
				wndListBox
				);
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( ((CExtPopupInplaceListBox::e_sel_action_t)eSAT) )
	return false;
}

bool CExtPopupMenuWnd::MENUITEMDATA::stat_CbListBoxItemDraw(
	CExtPopupInplaceListBox & wndListBox,
	CExtCustomizeCmdTreeNode * pNode,
	LPDRAWITEMSTRUCT pDIS
	)
{
	ASSERT_VALID( pNode );
	ASSERT( pDIS != NULL && pDIS->hDC != NULL );
	ASSERT( wndListBox.GetSafeHwnd() != NULL );
CExtCustomizeSite * pSite = CExtCustomizeSite::GetCustomizeSite( wndListBox.GetSafeHwnd() );
	if( pSite == NULL )
		return false;
	if(	pSite->OnPopupListBoxItemDraw(
			NULL,
			pNode,
			wndListBox,
			pDIS
			)
		)
		return true;

INT nIndex = INT( pDIS->itemID );

	if( (wndListBox.GetStyle() & LBS_HASSTRINGS) == 0 )
		return false;
	if( nIndex < 0 || nIndex >= wndListBox.GetCount() )
		return true;

CExtPopupMenuWnd * pPopup =
		STATIC_DOWNCAST( CExtPopupMenuWnd, wndListBox.GetParent() );
CExtPaintManager * pPM = pPopup->PmBridge_GetPM();

bool bEnabled = wndListBox.LbItemIsEnabled( nIndex );
bool bSelected = ( pDIS->itemState & ODS_SELECTED ) != 0 ? true : false;

COLORREF clrText = wndListBox.LbItemTextColorGet( nIndex );
COLORREF clrBack = wndListBox.LbItemBackColorGet( nIndex );

CDC dc;
	dc.Attach( pDIS->hDC );

	if( pDIS->itemAction & (ODA_DRAWENTIRE | ODA_SELECT) )
	{
		CRect rcErase( pDIS->rcItem );
		if( nIndex == (wndListBox.GetCount()-1) )
		{
			CRect rcClient;
			wndListBox.GetClientRect( &rcClient );
			if( rcErase.bottom < rcClient.bottom )
				rcErase.bottom = rcClient.bottom;
		}
		dc.FillSolidRect( 
			rcErase, 
			pPM->GetColor( COLOR_WINDOW )
			);
		dc.FillSolidRect(
			&pDIS->rcItem,
			(clrBack != COLORREF(-1L) && (!bSelected))
				? clrBack
				: pPM->GetColor(
					( bSelected && bEnabled )
						? COLOR_HIGHLIGHT
						: COLOR_WINDOW,
				&wndListBox
				)
			);
	}

	if( nIndex >= 0 )	
	{
		CString sText;
		wndListBox.GetText( (INT)pDIS->itemID, sText );
		if( ! sText.IsEmpty() )
		{
			COLORREF clrTextOld =
				dc.SetTextColor(
					(clrText != COLORREF(-1L) && (!bSelected))
						? clrText
						: pPM->GetColor(
							( bSelected && bEnabled )
								? COLOR_HIGHLIGHTTEXT 
								: bEnabled
									? COLOR_BTNTEXT 
									: CExtPaintManager::CLR_TEXT_DISABLED,
							&wndListBox
							)
					);
			int nOldBkMode = dc.SetBkMode( TRANSPARENT );
			CRect rcText( pDIS->rcItem );
			rcText.DeflateRect(
				pSite->OnPopupListBoxCalcItemExtraSizes(
					NULL,
					pNode
					)
				);
			CExtRichContentLayout::stat_DrawText(
				dc.m_hDC,
				LPCTSTR(sText),
				&rcText,
				DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_NOPREFIX, 0
				);
			dc.SetBkMode( nOldBkMode );
			dc.SetTextColor( clrTextOld );
		} // if( ! sText.IsEmpty() )
	} // if( nIndex >= 0 )

	if( bSelected && bEnabled )
	{
		COLORREF clrTextOld = 
			dc.SetTextColor( RGB(255,255,255) );
		COLORREF clrBkOld =
			dc.SetBkColor( RGB(0,0,0) );
		dc.DrawFocusRect( &pDIS->rcItem );
		dc.SetBkColor( clrBkOld );
		dc.SetTextColor( clrTextOld );
	}

	dc.Detach();
	return true;
}

bool CExtPopupMenuWnd::MENUITEMDATA::stat_CbListBoxItemMeasure(
	CExtPopupInplaceListBox & wndListBox,
	CExtCustomizeCmdTreeNode * pNode,
	LPMEASUREITEMSTRUCT pMIS
	)
{
	ASSERT_VALID( pNode );
	ASSERT( pMIS != NULL );
	ASSERT( wndListBox.GetSafeHwnd() != NULL );
CExtCustomizeSite * pSite = CExtCustomizeSite::GetCustomizeSite( wndListBox.GetSafeHwnd() );
	if( pSite == NULL )
		return false;
	if(	pSite->OnPopupListBoxItemMeasure(
			NULL,
			pNode,
			wndListBox,
			pMIS
			)
		)
		return true;

	if( (wndListBox.GetStyle() & LBS_HASSTRINGS) == 0 )
		return false;
	pMIS->itemWidth = pMIS->itemHeight = 10;
	if( ((INT)pMIS->itemID) < 0 || ((INT)pMIS->itemID) >= wndListBox.GetCount() )
		return true;
CExtSafeString sMeasure;
	wndListBox.GetText( (INT)pMIS->itemID, *((CString*)&sMeasure) );
	if( sMeasure.IsEmpty() )
		return true;

CExtPopupMenuWnd * pPopup =
		STATIC_DOWNCAST( CExtPopupMenuWnd, wndListBox.GetParent() );
CExtPaintManager * pPM = pPopup->PmBridge_GetPM();

CWindowDC dc( NULL );
CRect rcMeasure =
		CExtPaintManager::stat_CalcTextDimension(
			dc,
			pPM->m_FontNormal,
			sMeasure
			);
	rcMeasure.InflateRect(
		pSite->OnPopupListBoxCalcItemExtraSizes(
			NULL,
			pNode
			)
		);
	pMIS->itemWidth = rcMeasure.Width();
	pMIS->itemHeight = rcMeasure.Height();
	return true;
}
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

bool CExtPopupMenuWnd::MENUITEMDATA::UpdateFromCmdTree(
	HWND hWndCmdRecv,
	CExtCustomizeCmdTreeNode * pNode,
	int nItemIndex,
	CExtPopupMenuWnd * pPopupParent // = NULL
	)
{
	ASSERT_VALID( pNode );
	m_nItemIndex = nItemIndex;

	if( pNode->PreConstructMenuItem(
			this,
			hWndCmdRecv,
			nItemIndex,
			pPopupParent
			)
		)
		return true;

	if( pNode->GetFlags()&__ECTN_BOLD_IN_MENU )
		m_bBold = true;
	if( pNode->GetFlags()&__ECTN_BIG_ACCENT_IN_MENU )
		m_bBigAccent = true;
	m_sExtendedText = pNode->GetTextMenuExtended();
LPCTSTR sCaptionText = LPCTSTR( pNode->GetTextMenuCaption() );

CExtCustomizeSite * pSite = NULL;
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	if( (pNode->GetFlags() & __ECTN_TBB_TEXT_FIELD) != 0 )
	{
		ASSERT( m_hWndSpecCmdReceiver != NULL );
		ASSERT( ::IsWindow(m_hWndSpecCmdReceiver) );
		pSite =
			CExtCustomizeSite::GetCustomizeSite(
				m_hWndSpecCmdReceiver
				);
//		if( pSite == NULL )
//		{
//			ASSERT( FALSE );
//			return false;
//		}
		bool bComboField =
			( (pNode->GetFlags() & __ECTN_TBB_COMBO) != 0 )
				? true : false;
		bool bDateField =
			( (pNode->GetFlags() & __ECTN_TBB_DATE) != 0 )
				? true : false;
		
		m_nCmdID =
			( bComboField || bDateField )
				? ((UINT)TYPE_POPUP)
				: pNode->GetCmdID( false )
				;
		m_pCmdNode = pNode;
		if( bComboField )
		{
			// construct a popup
			//ASSERT( m_iconPopup.IsEmpty() );
			//ASSERT( m_pWndChild == NULL );
			if( ! m_iconPopup.IsEmpty() )
				m_iconPopup.Empty();
			if( m_pWndChild != NULL )
			{
				m_pWndChild->m_pWndParentMenu = NULL;
				delete m_pWndChild;
				m_pWndChild = NULL;
			}
			
			CExtPopupControlMenuWnd * pExtCrtPopup = NULL;
			if( pSite != NULL )
				pExtCrtPopup = 
					pSite->OnTextFieldCreateDropPopup(
						NULL,
						pNode,
						m_hWndSpecCmdReceiver,
						false
						);
			if( pExtCrtPopup != NULL )
			{
				m_pWndChild = pExtCrtPopup;
				if( pPopupParent != NULL )
					m_pWndChild->m_pWndParentMenu = pPopupParent;
				if( sCaptionText != NULL )
					pExtCrtPopup->MenuCaptionTextSet( sCaptionText );
			} // if( pExtCrtPopup != NULL )
			else
			{
				CExtPopupListBoxMenuWnd * pPopup =
					STATIC_DOWNCAST(
						CExtPopupListBoxMenuWnd,
						CExtPopupMenuWnd::InstantiatePopupMenu(
							hWndCmdRecv,
							RUNTIME_CLASS(CExtPopupListBoxMenuWnd),
							pNode // this
							)
						);
				if( sCaptionText != NULL )
					pPopup->MenuCaptionTextSet( sCaptionText );
				if( pPopupParent != NULL )
					pPopup->m_pWndParentMenu = pPopupParent;
				pPopup->m_lParamListCookie = pNode->GetLParam();
				pPopup->m_pCbListBoxSelection = 
					(CExtPopupInplaceListBox::pCbListBoxSelection)
						stat_CbListBoxSelection;
				pPopup->m_pSelectionCookie = (LPVOID)pNode;
				pPopup->m_pCbInitListBoxContent =
					(CExtPopupInplaceListBox::pCbInitListBoxContent)
						stat_CbInitListBoxContent;
				pPopup->m_pInitListBoxCookie = (LPVOID)pNode;
				pPopup->m_pCbListBoxItemDraw =
					(CExtPopupInplaceListBox::pCbListBoxItemDraw)
						stat_CbListBoxItemDraw;
				pPopup->m_pCbListBoxItemMeasure =
					(CExtPopupInplaceListBox::pCbListBoxItemMeasure)
						stat_CbListBoxItemMeasure;
				pPopup->m_pListBoxItemCookie = (LPVOID)pNode;
				CSize sizeMeasure( 0, 0 );
				if( pSite != NULL )
				{
					pSite->OnPopupListBoxGetStyles(
						NULL,
						pNode,
						pPopup->m_dwListBoxStyles
						);
					sizeMeasure =
						pSite->OnPopupListBoxMeasureTrackSize(
							NULL,
							pNode
							);
				} // if( pSite != NULL )
				if( sizeMeasure.cx == 0 && sizeMeasure.cy == 0 )
				{
					pPopup->m_sizeChildControl.cx = pNode->TextFieldWidthGet();
					pPopup->m_sizeChildControl.cy = pNode->DropDownHeightMaxGet();
				} // if( sizeMeasure.cx == 0 && sizeMeasure.cy == 0 )
				else
					pPopup->m_sizeChildControl = sizeMeasure;
				m_pWndChild = pPopup;
			} // else from if( pExtCrtPopup != NULL )
			m_pWndChild->m_hWndCmdReceiver = m_hWndSpecCmdReceiver;
			//
			m_sItemText = _T("");
			m_sAccelText = _T("");
			SetPopupText( pNode->GetTextInMenu( NULL ) );
		} // if( bComboField )
#ifndef __EXT_MFC_NO_BUILTIN_DATEFIELD
		else if( bDateField )
		{
		
			// construct a popup
			ASSERT( m_iconPopup.IsEmpty() );
			ASSERT( m_pWndChild == NULL );
			
			CExtPopupControlMenuWnd * pExtCrtPopup =
				pSite->OnTextFieldCreateDropPopup(
					NULL,
					pNode,
					m_hWndSpecCmdReceiver,
					false
					);
			if( pExtCrtPopup != NULL )
			{
				m_pWndChild = pExtCrtPopup;
				if( sCaptionText != NULL )
					pExtCrtPopup->MenuCaptionTextSet( sCaptionText );
			} // if( pExtCrtPopup != NULL )
			else
			{
				CSize szCalendarDimensions = CSize(1,1);
				DWORD dwDatePickerWindowStyle = WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN;
				DWORD dwDatePickerStyle = __EDPWS_DEFAULT;
				pSite->OnPopupDatePickerGetStyles(
					NULL,
					pNode,
					szCalendarDimensions,
					dwDatePickerStyle
					);
				CExtPopupDatePickerMenuWnd * pPopup =
					STATIC_DOWNCAST(
						CExtPopupDatePickerMenuWnd,
						CExtPopupMenuWnd::InstantiatePopupMenu(
							m_hWndSpecCmdReceiver,
							RUNTIME_CLASS(CExtPopupDatePickerMenuWnd),
							pNode // this
							)
						);
				pPopup->Construct(
						pNode->GetLParam(),
						szCalendarDimensions,
						dwDatePickerWindowStyle,
						dwDatePickerStyle
						);
				if( sCaptionText != NULL )
					pPopup->MenuCaptionTextSet( sCaptionText );
			
				pPopup->m_pCbDatePickerSelection = 
					(CExtPopupInplaceDatePicker::pCbDatePickerSelection)
						stat_CbDatePickerSelection;
				pPopup->m_pDatePickerSelectionCookie = (LPVOID)pNode;
				
				pPopup->m_pCbInitDatePickerContent =
					(CExtPopupInplaceDatePicker::pCbInitDatePickerContent)
						stat_CbInitDatePickerContent;
				pPopup->m_pInitDatePickerCookie = (LPVOID)pNode;
		
				m_pWndChild = pPopup;
			} // else from if( pExtCrtPopup != NULL )
			m_pWndChild->m_hWndCmdReceiver = m_hWndSpecCmdReceiver;

			m_sItemText = _T("");
			m_sAccelText = _T("");
			SetPopupText( pNode->GetTextInMenu( NULL ) );
		} // if( bDateField )
#endif // __EXT_MFC_NO_BUILTIN_DATEFIELD
		else
			m_sItemText = pNode->GetTextInMenu( NULL );
		if( pSite != NULL )
			pSite->OnTextFieldInplaceTextGet(
				NULL,
				pNode,
				pNode->m_sDefInplaceEditBuffer
				);
		SetInplaceEdit(
			&pNode->m_sDefInplaceEditBuffer,
			(CExtPopupMenuWnd::pCbVerifyTextInput)stat_CbVerifyTextInput,
			(CExtPopupMenuWnd::pCbPutTextInputResult)stat_CbPutTextInputResult,
			(CExtPopupMenuWnd::pCbInplaceEditWndProc)stat_CbInplaceEditWndProc,
			(LPVOID)pNode,
			pNode->TextFieldWidthGet()
			);

		bool bTextFieldIsNotEditable =
			( (pNode->GetFlags() & __ECTN_TBB_TF_NE) != 0 )
				? true : false;
		AllowInplaceEditActivation( ! bTextFieldIsNotEditable );

		SetDisplayed( true );

		SetAccelText( _T("") );
		
		MeasureItem( NULL );
		return true;
	} // if( (pNode->GetFlags() & __ECTN_TBB_TEXT_FIELD) != 0 )
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)

	if( pNode->GetFlags() & __ECTN_TBB_COLOR )
	{
		ASSERT( m_iconPopup.IsEmpty() );
		ASSERT( m_pWndChild == NULL );
		CExtPopupColorMenuWnd * pColorPopup =
			STATIC_DOWNCAST(
				CExtPopupColorMenuWnd,
				CExtPopupMenuWnd::InstantiatePopupMenu(
					hWndCmdRecv,
					RUNTIME_CLASS(CExtPopupColorMenuWnd),
					pPopupParent // this
					)
				);
		if( sCaptionText != NULL )
			pColorPopup->MenuCaptionTextSet( sCaptionText );
		 m_pWndChild = pColorPopup;
		if( pPopupParent != NULL )
			m_pWndChild->m_pWndParentMenu = pPopupParent;
		
		ASSERT( m_hWndSpecCmdReceiver != NULL );
		ASSERT( ::IsWindow(m_hWndSpecCmdReceiver) );
		m_pWndChild->m_hWndCmdReceiver = m_hWndSpecCmdReceiver;

		m_nCmdID = (UINT)TYPE_POPUP;
		m_sItemText = pNode->GetTextInMenu( NULL );
		m_pCmdNode = pNode;
		ASSERT( m_pWndChild != NULL );
		m_pWndChild->m_pNode = pNode;

		pColorPopup->m_lParamCookie = pNode->GetLParam();
		pColorPopup->m_bEnableBtnColorDefault =
			( (pNode->GetFlags() & __ECTN_TBB_MCB_DEFAULT) != 0 ) ? true : false;
		pColorPopup->m_bEnableBtnColorCustom =
			( (pNode->GetFlags() & __ECTN_TBB_MCB_CUSTOM) != 0 ) ? true : false;
		if( pSite == NULL && m_hWndSpecCmdReceiver != NULL )
			pSite =
				CExtCustomizeSite::GetCustomizeSite(
					m_hWndSpecCmdReceiver
					);
		if( pSite == NULL )
		{
			ASSERT( FALSE );
			return false;
		}
		pSite->OnColorItemValueGet(
			NULL,
			pNode,
			pColorPopup->m_clrDefault,
			false
			);
		pSite->OnColorItemValueGet(
			NULL,
			pNode,
			pColorPopup->m_clrInitial,
			true
			);
		CExtSafeString sBtnText;
		if(	pSite->OnColorItemGetBtnTextDefault(
				NULL,
				pNode,
				sBtnText
				)
			)
			pColorPopup->m_sBtnTextColorDefault = sBtnText;
		sBtnText.Empty();
		if(	pSite->OnColorItemGetBtnTextCustom(
				NULL,
				pNode,
				sBtnText
				)
			)
			pColorPopup->m_sBtnTextColorCustom = sBtnText;
		pSite->OnColorItemGenerateIcon(
			NULL,
			pNode,
			pColorPopup->m_clrInitial,
			m_iconPopup
			);
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd( hWndCmdRecv ),
				pNode->GetCmdID()
				);
		if( pCmdItem != NULL )
			SetAccelText( pCmdItem->m_sAccelText );

		MeasureItem( NULL );
		return true;
	} // if( pNode->GetFlags() & __ECTN_TBB_COLOR )

#ifndef __EXT_MFC_NO_UNDO_REDO_POPUP

	else if( pNode->GetFlags() & __ECTN_TBB_UNDO_REDO )
	{
		ASSERT( m_iconPopup.IsEmpty() );
		ASSERT( m_pWndChild == NULL );
		
		CExtPopupUndoRedoMenuWnd * pPopup =
			STATIC_DOWNCAST(
				CExtPopupUndoRedoMenuWnd,
				CExtPopupMenuWnd::InstantiatePopupMenu(
					hWndCmdRecv,
					RUNTIME_CLASS(CExtPopupUndoRedoMenuWnd),
					m_pOwner
					)
				);
		if( sCaptionText != NULL )
			pPopup->MenuCaptionTextSet( sCaptionText );
		pPopup->m_lParamCookie = pNode->GetLParam();
		pPopup->m_pCbListBoxSelection = 
			(CExtPopupInplaceListBox::pCbListBoxSelection)
				stat_CbListBoxSelection;
		pPopup->m_pSelectionCookie = (LPVOID)pNode;
		pPopup->m_pCbInitListBoxContent =
			(CExtPopupInplaceListBox::pCbInitListBoxContent)
				stat_CbInitListBoxContent;
		pPopup->m_pInitListBoxCookie = (LPVOID)pNode;
		pPopup->m_pCbListBoxItemDraw =
			(CExtPopupInplaceListBox::pCbListBoxItemDraw)
				stat_CbListBoxItemDraw;
		pPopup->m_pCbListBoxItemMeasure =
			(CExtPopupInplaceListBox::pCbListBoxItemMeasure)
				stat_CbListBoxItemMeasure;
		pPopup->m_pListBoxItemCookie = (LPVOID)pNode;

		if( pSite == NULL )
			pSite = CExtCustomizeSite::GetCustomizeSite( m_hWndSpecCmdReceiver );
		if( pSite == NULL )
		{
			ASSERT( FALSE );
			return false;
		}
		pSite->OnPopupListBoxGetStyles(
			NULL,
			pNode,
			pPopup->m_dwListBoxStyles
			);

		pPopup->m_pCbFormatCaption =
			(CExtPopupUndoRedoMenuWnd::pCbFormatCaption)
				stat_CbFormatCaption;
		pPopup->m_pFormatCaptionCookie = (LPVOID)pNode;
			
		CSize sizeMeasure =
			pSite->OnPopupListBoxMeasureTrackSize(
				NULL,
				pNode
				);
		if( sizeMeasure.cx == 0 && sizeMeasure.cy == 0 )
		{
			pPopup->m_sizeChildControl.cx = pNode->TextFieldWidthGet();
			pPopup->m_sizeChildControl.cy = pNode->DropDownHeightMaxGet();
		} // if( sizeMeasure.cx == 0 && sizeMeasure.cy == 0 )
		else
			pPopup->m_sizeChildControl = sizeMeasure;
		m_pWndChild = pPopup;
		
		if( pPopupParent != NULL )
			m_pWndChild->m_pWndParentMenu = pPopupParent;
		
		ASSERT( m_hWndSpecCmdReceiver != NULL );
		ASSERT( ::IsWindow(m_hWndSpecCmdReceiver) );
		m_pWndChild->m_hWndCmdReceiver = m_hWndSpecCmdReceiver;

		SetPopupText( pNode->GetTextInMenu( NULL ) );
//		m_nCmdID = pNode->GetCmdID( true );
		m_nCmdID = (UINT)TYPE_POPUP;

		m_pCmdNode = pNode;
		ASSERT( m_pWndChild != NULL );
		m_pWndChild->m_pNode = pNode;

		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd( hWndCmdRecv ),
				pNode->GetCmdID()
				);
		if( pCmdItem != NULL )
			SetAccelText( pCmdItem->m_sAccelText );

		MeasureItem( NULL );
		return true;	
	} // else if( pNode->GetFlags() & __ECTN_TBB_UNDO_REDO )

#endif // __EXT_MFC_NO_UNDO_REDO_POPUP

	if(		(pNode->GetFlags() & __ECTN_AUTO_POPUP) != 0
		||	pNode->GetNodeCount() > 0
		)
	{
		if( ! ConstructPopup() )
		{
			ASSERT( FALSE );
			return false;
		}
		if( sCaptionText != NULL && m_pWndChild != NULL )
			m_pWndChild->MenuCaptionTextSet( sCaptionText );
		m_nCmdID = (UINT)TYPE_POPUP;
		m_sItemText = pNode->GetTextInMenu( NULL );
		m_pCmdNode = pNode;
		ASSERT( m_pWndChild != NULL );
		m_pWndChild->m_pNode = pNode;
		MeasureItem( NULL );
		return true;
	}

//	m_sItemText.Empty();
UINT nNodeCmdID = pNode->GetCmdID();
	if(  nNodeCmdID == ID_SEPARATOR )
	{
		SetSeparator( nItemIndex );
		return true;
	}
	else if( ! CExtCmdManager::IsCommand( nNodeCmdID ) )
	{
		ASSERT( FALSE );
		return false;
	}
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	if( (pNode->GetFlags() & (__ECTN_TBB_COMBO|__ECTN_TBB_DATE|__ECTN_TBB_TEXT_FIELD) ) == 0 )
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
		m_nCmdID = pNode->GetCmdID();
CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd( hWndCmdRecv ),
			pNode->GetCmdID()
			);
//	ASSERT( pCmdItem != NULL );
//	if( pCmdItem == NULL )
//		return false;
	m_sItemText = pNode->GetTextInMenu( pCmdItem );
	m_pCmdNode = pNode;

	if( pCmdItem != NULL )
		SetAccelText( pCmdItem->m_sAccelText );
	
	pNode->PostConstructMenuItem(
		this,
		hWndCmdRecv,
		nItemIndex,
		pPopupParent
		);

	MeasureItem( NULL );
	return true;
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

/////////////////////////////////////////////////////////////////////////////
// CExtPopupBaseWnd

IMPLEMENT_DYNCREATE( CExtPopupBaseWnd, __BaseClassOfCExtPopupBaseWnd )
IMPLEMENT_CExtPmBridge_MEMBERS( CExtPopupBaseWnd );

CExtPopupBaseWnd::CExtPopupBaseWnd()
	: m_bPrintState( false )
	, m_eCombineAlign( __CMBA_NONE )
	, m_hFont( NULL )
{
	m_clrFadeBitsSrc
		= m_clrFadeBitsDst
		= m_clrFadeBitsTmp
		= NULL;
	m_AnimationType = g_DefAnimationType;
	_InitAnimation();
	m_ePlaySoundOnAnimationFinished =
		CExtSoundPlayer::__NO_SOUND;

	m_sizeFullItems.cx = m_sizeFullItems.cy = 1;

	m_bExcludeAreaSpec
		= m_bCombineWithEA
		= false;
	m_rcExcludeArea.SetRectEmpty();
	m_ptTrack.x = m_ptTrack.y
		= m_ptTrackOriginal.x = m_ptTrackOriginal.y
		= m_ptTrackInvoked.x = m_ptTrackInvoked.y
		= m_ptTrackWatched.x = m_ptTrackWatched.y
		= 0;

	PmBridge_Install();
}

CExtPopupBaseWnd::~CExtPopupBaseWnd()
{
	PmBridge_Uninstall();
}

void CExtPopupBaseWnd::_AdjustAnimation(
	CExtPopupBaseWnd::e_animation_type_t & eAT
	)
{
	ASSERT_VALID( this );
	eAT;
}

void CExtPopupMenuWnd::_AdjustAnimation(
	CExtPopupBaseWnd::e_animation_type_t & eAT
	)
{
	ASSERT_VALID( this );
	if( _IsRibbonMode() )
		eAT = __AT_CONTENT_DISPLAY;
}

CExtSafeString CExtPopupBaseWnd::GetAnimaptionName( CExtPopupBaseWnd::e_animation_type_t eAT )
{
CExtSafeString strAT;
	switch( eAT )
	{
	case __AT_NONE:
		if( ! g_ResourceManager->LoadString( strAT, IDS_EXT_AT_NONE ) )
			strAT = _T("(none)");
		break;
	case __AT_RANDOM:
		if( ! g_ResourceManager->LoadString( strAT, IDS_EXT_AT_RANDOM ) )
			strAT = _T("Random select");
		break;
	case __AT_ROLL:
		if( ! g_ResourceManager->LoadString( strAT, IDS_EXT_AT_ROLL ) )
			strAT = _T("Roll");
		break;
	case __AT_SLIDE:
		if( ! g_ResourceManager->LoadString( strAT, IDS_EXT_AT_SLIDE ) )
			strAT = _T("Slide");
		break;
	case __AT_FADE:
		if( ! g_ResourceManager->LoadString( strAT, IDS_EXT_AT_FADE ) )
			strAT = _T("Fade");
		break;
	case __AT_ROLL_AND_STRETCH:
		if( ! g_ResourceManager->LoadString( strAT, IDS_EXT_AT_ROLL_AND_STRETCH ) )
			strAT = _T("Roll and stretch");
		break;
	case __AT_SLIDE_AND_STRETCH:
		if( ! g_ResourceManager->LoadString( strAT, IDS_EXT_AT_SLIDE_AND_STRETCH ) )
			strAT = _T("Slide and stretch");
		break;
	case __AT_NOISE:
		if( ! g_ResourceManager->LoadString( strAT, IDS_EXT_AT_NOISE ) )
			strAT = _T("Noise");
		break;
	case __AT_BOXES:
		if( ! g_ResourceManager->LoadString( strAT, IDS_EXT_AT_BOXES ) )
			strAT = _T("Boxes");
		break;
	case __AT_CIRCLES:
		if( ! g_ResourceManager->LoadString( strAT, IDS_EXT_AT_CIRCLES ) )
			strAT = _T("Circles");
		break;
	case __AT_HOLES:
		if( ! g_ResourceManager->LoadString( strAT, IDS_EXT_AT_HOLES ) )
			strAT = _T("Holes");
		break;
	} // switch( eAT )
	return strAT;
}

void CExtPopupBaseWnd::_InitAnimation()
{
	ASSERT_VALID( this );

	m_nAnimPercent = m_nAnimPercentOld = 0;

	_AdjustAnimation( m_AnimationType );

	if(		(	m_AnimationType < 0
			||	m_AnimationType > __AT_MAX_VALID_VALUE
			)
		&&	m_AnimationType != __AT_CONTENT_DISPLAY
		&&	m_AnimationType != __AT_CONTENT_EXPAND
		)
		m_AnimationType = __AT_NONE;

	if( m_AnimationType == __AT_RANDOM )
	{
		int nRand = rand() % (__AT_MAX_VALID_VALUE+1);
		ASSERT( nRand >= 0 && nRand <= __AT_MAX_VALID_VALUE );
		if( nRand == __AT_NONE || nRand == __AT_RANDOM )
			nRand = __AT_FADE; // most cool of all effects
		m_AnimationType = (e_animation_type_t)nRand;
	}

int nBitsPerPixel = CExtPaintManager::stat_GetBPP();
	if(		nBitsPerPixel <= 8
		&&	m_AnimationType != __AT_CONTENT_EXPAND
		&&	m_AnimationType != __AT_CONTENT_DISPLAY
		)
	{
		m_AnimationType = __AT_NONE;
		m_bAnimFinished = true;
	}

	else
	{
		m_bAnimFinished =
			(m_AnimationType == __AT_NONE) ? true : false;
	}

//	ASSERT(
//			m_bmpScreenDst.GetSafeHandle() == NULL
//		&&	m_bmpScreenSrc.GetSafeHandle() == NULL
//		&&	m_bmpScreenTmp.GetSafeHandle() == NULL
//		&&	m_clrFadeBitsSrc == NULL
//		&&	m_clrFadeBitsDst == NULL
//		&&	m_clrFadeBitsTmp == NULL
//		);
	if( m_bmpScreenDst.GetSafeHandle() != NULL )
		m_bmpScreenDst.DeleteObject();
	if( m_bmpScreenSrc.GetSafeHandle() != NULL )
		m_bmpScreenSrc.DeleteObject();
// 	if( m_bmpScreenSrcAlt.GetSafeHandle() != NULL )
// 		m_bmpScreenSrcAlt.DeleteObject();
	if( m_bmpScreenTmp.GetSafeHandle() != NULL )
		m_bmpScreenTmp.DeleteObject();
	m_clrFadeBitsSrc = NULL;
	m_clrFadeBitsDst = NULL;
	m_clrFadeBitsTmp = NULL;
	
}

void CExtPopupBaseWnd::_StartAnimation()
{
	ASSERT_VALID( this );
	_InitAnimation();
	if( m_AnimationType != __AT_NONE )
	{
		SetTimer( ID_TIMER_ANIMATION, ID_PERIOD_ANIMATION, NULL );
		g_nLastAnimTime = clock();
	}
}

void CExtPopupMenuWnd::_StartAnimation()
{
	ASSERT_VALID( this );
	if( _FindCustomizeMode() )
		return;
HWND hWndOwn = m_hWnd;
	_ItemFocusDelay();
	if( ! ::IsWindow( hWndOwn ) )
		return;
	_CoolTipHide( false );
	if( ! ::IsWindow( hWndOwn ) )
		return;
	CExtPopupBaseWnd::_StartAnimation();
	if( ! ::IsWindow( hWndOwn ) )
		return;
	GetSite().SetAnimated(
		m_bAnimFinished ? NULL : this
		);
}

BEGIN_MESSAGE_MAP(CExtPopupBaseWnd, __BaseClassOfCExtPopupBaseWnd)
	//{{AFX_MSG_MAP(CExtPopupBaseWnd)
	ON_WM_MOUSEACTIVATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_SIZE()
	ON_WM_ACTIVATEAPP()
	ON_WM_CANCELMODE()
	ON_WM_SETCURSOR()
	ON_WM_TIMER()
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(WM_GETFONT, OnGetFont)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
 
LRESULT CExtPopupBaseWnd::OnSetFont( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
HFONT hFont = (HFONT) wParam;
BOOL bRedraw = (BOOL) lParam;
	m_hFont = hFont; 
	if( bRedraw )
		Invalidate();
	return 0L;
}
  
LRESULT CExtPopupBaseWnd::OnGetFont( WPARAM, LPARAM )
{
	ASSERT_VALID( this );
    return (LRESULT) m_hFont;
}

LRESULT CExtPopupBaseWnd::WindowProc(
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
{
	if( message == WM_PRINT || message == WM_PRINTCLIENT )
	{
		HDC hDC = (HDC)wParam;
		if( hDC == NULL )
			return 0;
		m_bPrintState = true;
		CDC * pDC = CDC::FromHandle( hDC );
		_DoPaint( *pDC );
		m_bPrintState = false;
		return 0;
	}
LRESULT lResult = __BaseClassOfCExtPopupBaseWnd :: WindowProc( message, wParam, lParam );
	return lResult;
}

void CExtPopupBaseWnd::OnPaint() 
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
CPaintDC dc( this );
	if( !(::IsWindowVisible(GetSafeHwnd())) )
		return;
	CExtPaintManager::stat_ExcludeChildAreas(
		dc.GetSafeHdc(),
		m_hWnd
		);
	if( ! m_bAnimFinished )
		_DrawAnimatedState( dc );
	else
		_DoPaint( dc );
}

void CExtPopupBaseWnd::_DrawAnimatedState( CDC & dc )

{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
CRect rcClient;
	GetClientRect( &rcClient );

int cx = rcClient.Width();
int cy = rcClient.Height();

CDC * pPaintDC = &dc;

CPalette * pOldPalette = NULL;
	if( (::GetDeviceCaps(pPaintDC->m_hDC,RASTERCAPS) & RC_PALETTE) != 0 )
    {
        pOldPalette =
			pPaintDC->SelectPalette( & PmBridge_GetPM()->m_PaletteWide, FALSE );
        pPaintDC->RealizePalette();
    }

CDC dcmm;
	VERIFY( dcmm.CreateCompatibleDC(pPaintDC) );

bool bFail = false;
	if( m_bmpScreenDst.GetSafeHandle() == NULL )
	{
		ASSERT( m_bmpScreenSrc.GetSafeHandle() == NULL );
		ASSERT( m_bmpScreenTmp.GetSafeHandle() == NULL );
		
		int nBitsPerPixel = CExtPaintManager::stat_GetBPP();
		CBitmap * pBmpOld = NULL;
		if(		(	m_AnimationType == __AT_FADE
				&&	nBitsPerPixel > 8
				)
			||	m_AnimationType == __AT_CONTENT_EXPAND
			||	m_AnimationType == __AT_CONTENT_DISPLAY
			||	m_AnimationType == __AT_ROLL
			||	m_AnimationType == __AT_SLIDE
			||	m_AnimationType == __AT_ROLL_AND_STRETCH
			||	m_AnimationType == __AT_SLIDE_AND_STRETCH
			||	m_AnimationType == __AT_NOISE
			||	m_AnimationType == __AT_BOXES
			||	m_AnimationType == __AT_CIRCLES
			||	m_AnimationType == __AT_HOLES
			)
		{
			BITMAPINFOHEADER bih;
			bih.biSize = sizeof(BITMAPINFOHEADER);
			bih.biWidth = cx;
			bih.biHeight = cy;
			bih.biPlanes = 1;
			bih.biBitCount = 32;
			bih.biCompression = BI_RGB;
			bih.biSizeImage = cx * cy;
			bih.biXPelsPerMeter = 0;
			bih.biYPelsPerMeter = 0;
			bih.biClrUsed = 0;
			bih.biClrImportant = 0;

			HBITMAP hDIB =
				::CreateDIBSection(
					dcmm.GetSafeHdc(),
					(LPBITMAPINFO)&bih,
					DIB_RGB_COLORS,
					(void **)&m_clrFadeBitsSrc,
					NULL,
					NULL
					);
			if( hDIB != NULL && m_clrFadeBitsSrc != NULL )
			{
				m_bmpScreenSrc.Attach( hDIB );

				hDIB =
					::CreateDIBSection(
						dcmm.GetSafeHdc(),
						(LPBITMAPINFO)&bih,
						DIB_RGB_COLORS,
						(void **)&m_clrFadeBitsDst,
						NULL,
						NULL
						);
				if( hDIB != NULL && m_clrFadeBitsDst != NULL )
				{
					m_bmpScreenDst.Attach( hDIB );

					hDIB =
						::CreateDIBSection(
							dcmm.GetSafeHdc(),
							(LPBITMAPINFO)&bih,
							DIB_RGB_COLORS,
							(void **)&m_clrFadeBitsTmp,
							NULL,
							NULL
							);
					if( hDIB != NULL && m_clrFadeBitsTmp != NULL )
					{
						m_bmpScreenTmp.Attach( hDIB );

						pBmpOld =
							dcmm.SelectObject( &m_bmpScreenSrc );

						bool bDefaultCapture = true;
						if(		g_PaintManager.m_bIsWinVistaOrLater
							&&	g_PaintManager.m_DWM.IsCompositionEnabled()
							&&	m_bmpScreenSrcAlt.GetSafeHandle() != NULL
							)
						{
							CDC dcX;
							if( dcX.CreateCompatibleDC( NULL ) )
							{
								CBitmap * pBmpOldX = dcX.SelectObject( &m_bmpScreenSrcAlt );
								dcmm.BitBlt( 0, 0, cx, cy, &dcX, 0, 0, SRCCOPY );
								dcX.SelectObject( pBmpOldX );
								dcX.DeleteDC();
								bDefaultCapture = false;
							}
						}
						if( bDefaultCapture )
							dcmm.BitBlt(
								0, 0, cx, cy, pPaintDC,
								rcClient.left,
								rcClient.top,
								SRCCOPY
								);
						UINT nSizeInBytes = 
							sizeof(COLORREF) * cx * cy;
						__EXT_MFC_MEMCPY(
							m_clrFadeBitsDst,
							nSizeInBytes,
							m_clrFadeBitsSrc,
							nSizeInBytes
							);
						dcmm.SelectObject( &m_bmpScreenDst );
					} // if( hDIB != NULL && m_clrFadeBitsTmp != NULL )
					else
					{
						m_bmpScreenSrc.DeleteObject();
						m_bmpScreenDst.DeleteObject();
						//return;
						bFail = true;
					} // else from if( hDIB != NULL && m_clrFadeBitsTmp != NULL )
				} // if( hDIB != NULL && m_clrFadeBitsDst != NULL )
				else
				{
					m_bmpScreenSrc.DeleteObject();
					//return;
					bFail = true;
				} // else from if( hDIB != NULL && m_clrFadeBitsDst != NULL )
			} // if( hDIB != NULL && m_clrFadeBitsSrc != NULL )
		}
		else
		{
			m_bmpScreenDst.CreateCompatibleBitmap(
				pPaintDC,
				cx,
				cy
				);
			pBmpOld =
				dcmm.SelectObject( &m_bmpScreenDst );
		}

		if( ! bFail )
			_DoPaint( dcmm, false );
		dcmm.SetViewportOrg( CPoint(0,0) );
		dcmm.SelectObject( pBmpOld );
	} // if( m_bmpScreenDst.GetSafeHandle() == NULL )

CBitmap * pBmpOld = NULL;
	if( ! bFail )
	{
		COLORREF * src = m_clrFadeBitsSrc;
		COLORREF * dst = m_clrFadeBitsDst;
		COLORREF * tmp = m_clrFadeBitsTmp;

		switch( m_AnimationType )
		{
		case __AT_BOXES:
		case __AT_CIRCLES:
		case __AT_HOLES:
			if( tmp!= NULL && src!=NULL && dst != NULL )
			{
				ASSERT(
					m_nAnimPercent >= 0 && m_nAnimPercent <= 100
					&&
					m_nAnimPercentOld >= 0 && m_nAnimPercentOld <= 100
					&&
					m_nAnimPercentOld <= m_nAnimPercent
					);
				ASSERT( __BOX_ANIM_METRIC > 0 ); // self assert
				if( m_nAnimPercentOld != m_nAnimPercent )
				{
					pBmpOld = dcmm.SelectObject( &m_bmpScreenTmp );
					if( m_AnimationType == __AT_BOXES )
					{ // do boxes animation
						for( int y=0; y<cy; y++ )
						{
							int _y0 = y % __BOX_ANIM_METRIC;
							int _yPercent = 100 -
								(_y0 * 100) / __BOX_ANIM_METRIC;
							if( _yPercent < m_nAnimPercent )
							{
								UINT nSizeInBytes = 
									cx * sizeof(COLORREF);
								__EXT_MFC_MEMCPY(
									tmp,
									nSizeInBytes,
									src,
									nSizeInBytes
									);
								tmp += cx;
								src += cx;
								dst += cx;
								continue;
							}
							for( int x=0; x<cx; x++ )
							{
								int _x0 = x % __BOX_ANIM_METRIC;
								int _xPercent =
									(_x0 * 100) / __BOX_ANIM_METRIC;
								if( _xPercent < m_nAnimPercent )
									*tmp = *dst;
								else
									*tmp = *src;
								tmp++; src++; dst++;
							}
						} // for( int y=0; y<cy; y++ )
					} // do boxes animation
					else
					{ // do circles/holes animation
						double radius = 0;
						if( m_AnimationType == __AT_HOLES )
							radius = 
								((((double)(__BOX_ANIM_METRIC)) / 2) * (100-m_nAnimPercent))
								/ 100;
						else
							radius = 
								((((double)(__BOX_ANIM_METRIC)) / 2) * m_nAnimPercent)
								/ 100;
						if( radius > 0 )
						{
							for( int y=0; y<cy; y++ )
							{
								int _y0 = y % __BOX_ANIM_METRIC;
								int _y1 = y - _y0;
								//int _y2 = _y1 + __BOX_ANIM_METRIC;
								int _yCenter = _y1 + __BOX_ANIM_METRIC/2;
								int _yDist = abs( y - _yCenter );
								double _yDist2 = (double)_yDist*_yDist;
								for( int x=0; x<cx; x++ )
								{
									int _x0 = x % __BOX_ANIM_METRIC;
									int _x1 = x - _x0;
									//int _x2 = _x1 + __BOX_ANIM_METRIC;
									int _xCenter = _x1 + __BOX_ANIM_METRIC/2;
									int _xDist = abs( x - _xCenter );
									double _xDist2 = (double)_xDist*_xDist;

									double nDispance =
										::sqrt(_yDist2+_xDist2);
									if( m_AnimationType == __AT_HOLES )
									{ // holes
										if( nDispance >= radius )
											*tmp = *dst;
										else
											*tmp = *src;
									} // holes
									else
									{ // circles
										if( nDispance < radius )
											*tmp = *dst;
										else
											*tmp = *src;
									} // circles
									tmp++; src++; dst++;
								}
							} // for( int y=0; y<cy; y++ )
						} // if( radius > 0 )
					} // do circles/holes animation

					int nMenuShadowSize = OnQueryMenuShadowSize();
					if( m_bCombineWithEA )
					{ // paint combined area AS IS
						ASSERT( !m_rcExcludeArea.IsRectEmpty() );
						CRect rcExcludeArea( m_rcExcludeArea );
						ScreenToClient( &rcExcludeArea );
						pBmpOld = dcmm.SelectObject( &m_bmpScreenDst );
						pPaintDC->BitBlt(
							rcExcludeArea.left, rcExcludeArea.top,
							rcExcludeArea.Width(), rcExcludeArea.Height(), 
							&dcmm,
							rcExcludeArea.left, rcExcludeArea.top,
							SRCCOPY
							);
						dcmm.SelectObject( pBmpOld );
						pBmpOld = NULL;
						if( m_eCombineAlign != __CMBA_NONE )
						{
							rcExcludeArea.InflateRect(
								(m_eCombineAlign == __CMBA_RIGHT) ?
									0 : nMenuShadowSize,
								(m_eCombineAlign == __CMBA_BOTTOM) ?
									0 : nMenuShadowSize,
								(m_eCombineAlign == __CMBA_LEFT) ?
									0 : nMenuShadowSize,
								(m_eCombineAlign == __CMBA_TOP) ?
									0 : nMenuShadowSize
								);
						} // if( m_eCombineAlign != __CMBA_NONE )
						pPaintDC->ExcludeClipRect(
							&rcExcludeArea
							);
					} // paint combined area AS IS
					if( nMenuShadowSize > 0 )
					{
						CRect rcExcludeArea( rcClient );
						rcExcludeArea.left =
							rcExcludeArea.right
							- nMenuShadowSize;
						pPaintDC->ExcludeClipRect(
							&rcExcludeArea
							);
						rcExcludeArea = rcClient ;
						rcExcludeArea.top =
							rcExcludeArea.bottom
							- nMenuShadowSize;
						pPaintDC->ExcludeClipRect(
							&rcExcludeArea
							);
					}
					
					pPaintDC->BitBlt(
						rcClient.left, rcClient.top, cx, cy, 
						&dcmm,
						0,
						0,
						SRCCOPY
						);

					if( nMenuShadowSize > 0 || m_bCombineWithEA )
						pPaintDC->SelectClipRgn( NULL );
				} // if( m_nAnimPercentOld != m_nAnimPercent )
			}
		break; // cases __AT_BOXES,__AT_CIRCLES,__AT_HOLES
		case __AT_NOISE:
			if( tmp!= NULL && src!=NULL && dst != NULL )
			{
				ASSERT(
					m_nAnimPercent >= 0 && m_nAnimPercent <= 100
					&&
					m_nAnimPercentOld >= 0 && m_nAnimPercentOld <= 100
					&&
					m_nAnimPercentOld <= m_nAnimPercent
					);
				if( m_nAnimPercentOld != m_nAnimPercent )
				{
					int nDiff =
						m_nAnimPercent - m_nAnimPercentOld;
					ASSERT( nDiff > 0 );
					int nRandHalf = nDiff / 2 + 1;
					ASSERT( nRandHalf > 0 );
					pBmpOld = dcmm.SelectObject( &m_bmpScreenTmp );
					for( int pixel = 0; pixel < cx * cy; pixel++ )
					{
						int nRandValue = 
							rand() % nDiff;
						if( nRandValue > nRandHalf )
							*tmp = *dst;
//						else
//							*tmp = *src;
						tmp++; src++; dst++;
					} // for( int pixel = 0; pixel < cx * cy; pixel++ )

					int nMenuShadowSize = OnQueryMenuShadowSize();
					if( m_bCombineWithEA )
					{ // paint combined area AS IS
						ASSERT( !m_rcExcludeArea.IsRectEmpty() );
						CRect rcExcludeArea( m_rcExcludeArea );
						ScreenToClient( &rcExcludeArea );
						pBmpOld = dcmm.SelectObject( &m_bmpScreenDst );
						pPaintDC->BitBlt(
							rcExcludeArea.left, rcExcludeArea.top,
							rcExcludeArea.Width(), rcExcludeArea.Height(), 
							&dcmm,
							rcExcludeArea.left, rcExcludeArea.top,
							SRCCOPY
							);
						dcmm.SelectObject( pBmpOld );
						pBmpOld = NULL;
						if( m_eCombineAlign != __CMBA_NONE )
						{
							rcExcludeArea.InflateRect(
								(m_eCombineAlign == __CMBA_RIGHT) ?
									0 : nMenuShadowSize,
								(m_eCombineAlign == __CMBA_BOTTOM) ?
									0 : nMenuShadowSize,
								(m_eCombineAlign == __CMBA_LEFT) ?
									0 : nMenuShadowSize,
								(m_eCombineAlign == __CMBA_TOP) ?
									0 : nMenuShadowSize
								);
						} // if( m_eCombineAlign != __CMBA_NONE )
						pPaintDC->ExcludeClipRect(
							&rcExcludeArea
							);
					} // paint combined area AS IS
					if( nMenuShadowSize > 0 )
					{
						CRect rcExcludeArea( rcClient );
						rcExcludeArea.left =
							rcExcludeArea.right
							- nMenuShadowSize;
						pPaintDC->ExcludeClipRect(
							&rcExcludeArea
							);
						rcExcludeArea = rcClient ;
						rcExcludeArea.top =
							rcExcludeArea.bottom
							- nMenuShadowSize;
						pPaintDC->ExcludeClipRect(
							&rcExcludeArea
							);
					}
					
					pPaintDC->BitBlt(
						rcClient.left, rcClient.top, cx, cy, 
						&dcmm,
						0,
						0,
						SRCCOPY
						);

					if( nMenuShadowSize > 0 || m_bCombineWithEA )
						pPaintDC->SelectClipRgn( NULL );
				} // if( m_nAnimPercentOld != m_nAnimPercent )
			}
		break; // case __AT_NOISE
		case __AT_SLIDE:
		case __AT_ROLL:
		case __AT_ROLL_AND_STRETCH:
		case __AT_SLIDE_AND_STRETCH:
			if( tmp!= NULL && src!=NULL && dst != NULL )
			{
				ASSERT(
					m_nAnimPercent >= 0 && m_nAnimPercent <= 100
					&&
					m_nAnimPercentOld >= 0 && m_nAnimPercentOld <= 100
					&&
					m_nAnimPercentOld <= m_nAnimPercent
					);

				CRect _rcClient;
				_GetClientRect( &_rcClient );

				if( m_bCombineWithEA )
				{ // paint combined area AS IS
					ASSERT( !m_rcExcludeArea.IsRectEmpty() );
					CRect rcExcludeArea( m_rcExcludeArea );
					ScreenToClient( &rcExcludeArea );
					pBmpOld = dcmm.SelectObject( &m_bmpScreenDst );
					pPaintDC->BitBlt(
						rcExcludeArea.left, rcExcludeArea.top,
						rcExcludeArea.Width(), rcExcludeArea.Height(), 
						&dcmm,
						rcExcludeArea.left, rcExcludeArea.top,
						SRCCOPY
						);
					dcmm.SelectObject( pBmpOld );
					pBmpOld = NULL;
					if( m_eCombineAlign != __CMBA_NONE )
					{
						int nMenuShadowSize = OnQueryMenuShadowSize();
						rcExcludeArea.InflateRect(
							(m_eCombineAlign == __CMBA_RIGHT) ?
								0 : nMenuShadowSize,
							(m_eCombineAlign == __CMBA_BOTTOM) ?
								0 : nMenuShadowSize,
							(m_eCombineAlign == __CMBA_LEFT) ?
								0 : nMenuShadowSize,
							(m_eCombineAlign == __CMBA_TOP) ?
								0 : nMenuShadowSize
							);
					} // if( m_eCombineAlign != __CMBA_NONE )
					pPaintDC->ExcludeClipRect(
						&rcExcludeArea
						);
				} // paint combined area AS IS

				pBmpOld = dcmm.SelectObject( &m_bmpScreenDst );
				int cx =
					_rcClient.Width();
				int cy =
					_rcClient.Height();
				if( m_AnimationType == __AT_ROLL
					||
					m_AnimationType == __AT_SLIDE
					)
				{ // non-stratchable variants
					int _cx = 0;
					if( m_AnimationType == __AT_ROLL )
						_cx = (cx * (100-m_nAnimPercent)) / 100;
					int _cy =
						(cy * (100-m_nAnimPercent)) / 100;
					if( m_eCombineAlign == __CMBA_RIGHT )
					{
						CRgn rgnClip;
						if( rgnClip.CreateRectRgnIndirect( &_rcClient ) )
							pPaintDC->SelectClipRgn( &rgnClip );
						pPaintDC->BitBlt(
							_rcClient.left+_cx, _rcClient.top-_cy, cx, cy, 
							&dcmm,
							_rcClient.left, _rcClient.top,
							SRCCOPY
							);
						pPaintDC->SelectClipRgn( NULL );
					} // if m_eCombineAlign is __CMBA_RIGHT
					else if( m_eCombineAlign == __CMBA_BOTTOM )
					{
						CRgn rgnClip;
						if( rgnClip.CreateRectRgnIndirect( &_rcClient ) )
							pPaintDC->SelectClipRgn( &rgnClip );
						pPaintDC->BitBlt(
							_rcClient.left-_cx, _rcClient.top+_cy, cx, cy,
							&dcmm,
							_rcClient.left, _rcClient.top,
							SRCCOPY
							);
						pPaintDC->SelectClipRgn( NULL );
					} // if m_eCombineAlign is __CMBA_BOTTOM
					else
					{ // if m_eCombineAlign is __CMBA_TOP or __CMBA_LEFT
						pPaintDC->BitBlt(
							_rcClient.left-_cx, _rcClient.top-_cy, cx, cy,
							&dcmm,
							_rcClient.left, _rcClient.top,
							SRCCOPY
							);
					} // if m_eCombineAlign is __CMBA_TOP or __CMBA_LEFT
				} // non-stratchable variants
				else
				{ // stratchable variants
					int _cx = cx;
					if( m_AnimationType == __AT_ROLL_AND_STRETCH )
						_cx = (cx * m_nAnimPercent) / 100;
					int _cy =
						(cy * m_nAnimPercent) / 100;
					int nOldStretchBltMode =
						pPaintDC->SetStretchBltMode(
							g_PaintManager.m_bIsWinNT ? HALFTONE : COLORONCOLOR
							);
					if(		m_eCombineAlign == __CMBA_RIGHT
						&&	m_AnimationType == __AT_ROLL_AND_STRETCH
						)
					{
						CRgn rgnClip;
						if( rgnClip.CreateRectRgnIndirect( &_rcClient ) )
							pPaintDC->SelectClipRgn( &rgnClip );
						pPaintDC->StretchBlt(
							_rcClient.left+cx-_cx, _rcClient.top, _cx, _cy,
							&dcmm,
							_rcClient.left, _rcClient.top, cx, cy, 
							SRCCOPY
							);
						pPaintDC->SelectClipRgn( NULL );
					} // if m_eCombineAlign is __CMBA_RIGHT with __AT_ROLL_AND_STRETCH animation
					else if( m_eCombineAlign == __CMBA_BOTTOM )
					{
						CRgn rgnClip;
						if( rgnClip.CreateRectRgnIndirect( &_rcClient ) )
							pPaintDC->SelectClipRgn( &rgnClip );
						pPaintDC->StretchBlt(
							_rcClient.left-cx+_cx, _rcClient.top+cy-_cy, _cx, _cy,
							&dcmm,
							_rcClient.left, _rcClient.top, cx, cy, 
							SRCCOPY
							);
						pPaintDC->SelectClipRgn( NULL );
					} // if m_eCombineAlign is __CMBA_BOTTOM
					else
					{ // if m_eCombineAlign is __CMBA_TOP or __CMBA_LEFT
						pPaintDC->StretchBlt(
							_rcClient.left, _rcClient.top, _cx, _cy,
							&dcmm,
							_rcClient.left, _rcClient.top, cx, cy, 
							SRCCOPY
							);
					} // if m_eCombineAlign is __CMBA_TOP or __CMBA_LEFT
					pPaintDC->SetStretchBltMode( nOldStretchBltMode );
				} // stratchable variants

				if( m_bCombineWithEA )
					pPaintDC->SelectClipRgn( NULL );
			}
		break; // cases __AT_ROLL, __AT_SLIDE, __AT_ROLL_AND_STRETCH, __AT_ROLL_AND_STRETCH

		case __AT_CONTENT_EXPAND:
			if(		tmp!= NULL
				&&	src!=NULL
				&&	dst != NULL
				&&	IsKindOf( RUNTIME_CLASS(CExtPopupMenuWnd) )
				)
			{
				CExtPopupMenuWnd * pThisPopup = STATIC_DOWNCAST( CExtPopupMenuWnd, this );
				ASSERT(
					m_nAnimPercent >= 0 && m_nAnimPercent <= 100
					&&
					m_nAnimPercentOld >= 0 && m_nAnimPercentOld <= 100
					&&
					m_nAnimPercentOld <= m_nAnimPercent
					);

				CRect _rcClient;
				_GetClientRect( &_rcClient );

				if( m_bCombineWithEA )
				{ // paint combined area AS IS
					ASSERT( !m_rcExcludeArea.IsRectEmpty() );
					CRect rcExcludeArea( m_rcExcludeArea );
					ScreenToClient( &rcExcludeArea );
					pBmpOld = dcmm.SelectObject( &m_bmpScreenDst );
					pPaintDC->BitBlt(
						rcExcludeArea.left, rcExcludeArea.top,
						rcExcludeArea.Width(), rcExcludeArea.Height(), 
						&dcmm,
						rcExcludeArea.left, rcExcludeArea.top,
						SRCCOPY
						);
					dcmm.SelectObject( pBmpOld );
					pBmpOld = NULL;
				} // paint combined area AS IS

				pBmpOld = dcmm.SelectObject( &m_bmpScreenDst );
				int cx =
					_rcClient.Width();
				int cy =
					_rcClient.Height();
				int _cx = cx;
				int _cy =
					(cy * m_nAnimPercent) / 100;

				CExtPopupMenuWnd::visible_items_t v;
					pThisPopup->_GetVisibleItems(
						dcmm.GetSafeHdc(), // dc.GetSafeHdc()
						v
						);
				if( v.GetSize() == 0 )
				{ // if menu is empty
					int nOldStretchBltMode =
						pPaintDC->SetStretchBltMode(
							g_PaintManager.m_bIsWinNT ? HALFTONE : COLORONCOLOR
							);
					pPaintDC->StretchBlt(
						_rcClient.left, _rcClient.top, _cx, _cy, 
						&dcmm,
						_rcClient.left, _rcClient.top, cx, cy,
						SRCCOPY
						);
					pPaintDC->SetStretchBltMode( nOldStretchBltMode );
				} // if menu is empty
				else
				{ // if menu is NOT empty
					int y_pos_src = _rcClient.top;
					int y_pos_dst = _rcClient.top;
					INT vis_iter = 0;
					int nItemIndex = 0;
					int nItemCount = int(v.GetSize());
					ASSERT( nItemCount > 0 );
					CExtPopupMenuWnd::expand_effect_rects_container_t vRects;
					
					for( ; vis_iter < v.GetSize(); ++vis_iter, ++nItemIndex )
					{ // calc rects for visible items
						CExtPopupMenuWnd::VisibleItemDefinition_t & vi = v[vis_iter];
						CExtPopupMenuWnd::MENUITEMDATA & mi =
							pThisPopup->ItemGetInfo( vi.m_nIndex );
						ASSERT(
								vi.m_nIndex >= 0
							&&	vi.m_nIndex < pThisPopup->ItemGetCount() // m_items_all.GetSize()
							);
						ASSERT( mi.IsDisplayed() );
						ASSERT( mi.GetCmdID() == vi.m_nHelperCmdID );

						CRect rcItemSrc;
						pThisPopup->_GetItemRect( vi.m_nIndex, rcItemSrc, false );
						rcItemSrc.left = _rcClient.left;
						rcItemSrc.right = _rcClient.right;
						if( nItemIndex == 0 )
							rcItemSrc.top = _rcClient.top;
						else if( nItemIndex == (nItemCount-1) )
							rcItemSrc.bottom = _rcClient.bottom;

						int y_pos_src_next = 
							y_pos_src + rcItemSrc.Height();
						CRect rcItemDst( rcItemSrc );
						rcItemDst.OffsetRect(
							0,
							y_pos_dst - rcItemDst.top
							);
						if( vi.m_bRarelyUsed )
							rcItemDst.bottom =
								rcItemDst.top
								+
								(rcItemDst.Height() * m_nAnimPercent)
									/ 100;
						int y_pos_dst_next = 
							y_pos_dst + rcItemDst.Height();
						
						CExtPopupMenuWnd::ExpandEffectRects_t _eert(
							rcItemSrc,
							rcItemDst
							);
						vRects.Add( _eert );
						y_pos_dst = y_pos_dst_next;
						y_pos_src = y_pos_src_next;
					} // calc rects for visible items

					// y_pos_dst is now equal to size of
					// all compressed items
					ASSERT( y_pos_dst <= y_pos_src );
					if(		m_bCombineWithEA
						&&	m_eCombineAlign == __CMBA_BOTTOM
						&&	y_pos_dst != y_pos_src
						)
					{ // expand from bottom to top
						INT iter_rects = 0;
						int nDstOffset =
							y_pos_src - y_pos_dst;
						for( ; iter_rects < vRects.GetSize(); ++iter_rects )
						{ // adjust destination rects
							CExtPopupMenuWnd::ExpandEffectRects_t & eerc =
								vRects[ iter_rects ];
							eerc.m_rcDst.OffsetRect(
								0,
								nDstOffset
								);
						} // adjust destination rects
					} // expand from bottom to top

					INT iter_rects = 0;
					for( ; iter_rects < vRects.GetSize(); ++iter_rects )
					{ // paint visible items
						CExtPopupMenuWnd::ExpandEffectRects_t & eerc =
							vRects[ iter_rects ];
						if( eerc.m_rcDst.IsRectEmpty() )
							continue;
						if( CExtPopupMenuWnd::g_bUseStretchOnExpandAnimation )
						{
							int nOldStretchBltMode =
								pPaintDC->SetStretchBltMode(
									g_PaintManager.m_bIsWinNT ? HALFTONE : COLORONCOLOR
									);
							// something better then Office XP
							pPaintDC->StretchBlt(
								eerc.m_rcDst.left, eerc.m_rcDst.top,
								eerc.m_rcDst.Width(), eerc.m_rcDst.Height(), 
								&dcmm,
								eerc.m_rcSrc.left, eerc.m_rcSrc.top,
								eerc.m_rcSrc.Width(), eerc.m_rcSrc.Height(), 
								SRCCOPY
								);
							pPaintDC->SetStretchBltMode( nOldStretchBltMode );
						} // if( CExtPopupMenuWnd::g_bUseStretchOnExpandAnimation )
						else
							// somthing really like Office XP
							pPaintDC->BitBlt(
								eerc.m_rcDst.left, eerc.m_rcDst.top,
								eerc.m_rcDst.Width(), eerc.m_rcDst.Height(), 
								&dcmm,
								eerc.m_rcSrc.left, eerc.m_rcSrc.top,
								SRCCOPY
								);
					} // paint visible items
				} // if menu is NOT empty

			}
		break; // case __AT_CONTENT_EXPAND

		case __AT_CONTENT_DISPLAY:
			if( tmp!= NULL && src!=NULL && dst != NULL )
			{
				pBmpOld = dcmm.SelectObject( &m_bmpScreenDst );
				pPaintDC->BitBlt(
					rcClient.left, rcClient.top, cx, cy, 
					&dcmm,
					0,
					0,
					SRCCOPY
					);
			}
		break; // case __AT_CONTENT_DISPLAY

		case __AT_FADE:
			if( tmp!= NULL && src!=NULL && dst != NULL )
			{
				ASSERT(
					m_nAnimPercent >= 0 && m_nAnimPercent <= 100
					&&
					m_nAnimPercentOld >= 0 && m_nAnimPercentOld <= 100
					&&
					m_nAnimPercentOld <= m_nAnimPercent
					);
				pBmpOld = dcmm.SelectObject( &m_bmpScreenTmp );
				for( int pixel = 0; pixel < cx * cy; pixel++ )
				{
					COLORREF c0 = *dst++;
					COLORREF c1 = *src++;
					*tmp++ =
						RGB(
							(m_nAnimPercent*long(GetRValue(c0)) + (100L-m_nAnimPercent)*long(GetRValue(c1)) ) / 100L,
							(m_nAnimPercent*long(GetGValue(c0)) + (100L-m_nAnimPercent)*long(GetGValue(c1)) ) / 100L,
							(m_nAnimPercent*long(GetBValue(c0)) + (100L-m_nAnimPercent)*long(GetBValue(c1)) ) / 100L
							);
				} // for( int pixel = 0; pixel < cx * cy; pixel++ )
				pPaintDC->BitBlt(
					rcClient.left, rcClient.top, cx, cy, 
					&dcmm,
					0,
					0,
					SRCCOPY
					);
			}
		break; // case __AT_FADE

#ifdef _DEBUG
		default:
			ASSERT( FALSE );
		break; // default
#endif // _DEBUG
		} // switch( m_AnimationType )
	} // if( ! bFail )

	if( pBmpOld != NULL )
		dcmm.SelectObject( pBmpOld );

	if( pOldPalette != NULL )
		pPaintDC->SelectPalette( pOldPalette, FALSE );
}

void CExtPopupBaseWnd::_DoPaint( CDC & dcPaint, bool bUseBackBuffer /*= true*/ )
{
	ASSERT_VALID( this );
	dcPaint;
	bUseBackBuffer;
}

void CExtPopupBaseWnd::_DelayPaint()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
	PostMessage( WM_TIMER, ID_TIMER_DELAY_PAINT );
}

void CExtPopupBaseWnd::OnTimer(__EXT_MFC_UINT_PTR nIDEvent) 
{
	ASSERT_VALID( this );

	switch( nIDEvent )
	{
	case ID_TIMER_LIFE_STEP:
		return;
	case ID_TIMER_DELAY_PAINT:
		{
			KillTimer( ID_TIMER_DELAY_PAINT );
			CClientDC dc( this );
			_DoPaint( dc );
		}
		return;
	case ID_TIMER_DELAY_SHOW:
	{
		VERIFY( KillTimer( ID_TIMER_DELAY_SHOW ) );
		ShowWindow( SW_SHOWNA );
		Invalidate( FALSE );
		UpdateWindow();
		return;
	}
	// case ID_TIMER_DELAY_SHOW

	case ID_TIMER_ANIMATION:
		if( ! m_bAnimFinished )
		{
			clock_t nCurrAnimTime = clock();
			int nDuration =
				nCurrAnimTime - g_nLastAnimTime;
			int nSteps = (int)
				(0.5 + (float) nDuration / ID_PERIOD_ANIMATION);
			switch( m_AnimationType )
			{
			case __AT_CONTENT_DISPLAY:
				m_bAnimFinished = true;
			break;
			case __AT_FADE:
			case __AT_CONTENT_EXPAND:
			case __AT_ROLL:
			case __AT_SLIDE:
			case __AT_ROLL_AND_STRETCH:
			case __AT_SLIDE_AND_STRETCH:
			case __AT_NOISE:
			case __AT_BOXES:
			case __AT_CIRCLES:
			case __AT_HOLES:
				m_nAnimPercentOld = m_nAnimPercent;
				m_nAnimPercent += g_nAnimStepMetric;
				if( m_nAnimPercent >
						100 + nSteps * g_nAnimStepMetric
					)
					m_nAnimPercent = 101;
				if( m_nAnimPercent > 100 )
					m_bAnimFinished = true;
			break;
#ifdef _DEBUG
			default:
				ASSERT( FALSE );
			break;
#endif // _DEBUG
			} // switch( m_AnimationType )

			if( m_bAnimFinished )
			{
				if( !_FindCustomizeMode() )
				{
					g_SoundPlayer->PlaySound(
						m_ePlaySoundOnAnimationFinished
						);
					m_ePlaySoundOnAnimationFinished =
						CExtSoundPlayer::__NO_SOUND;
				} // if( !_FindCustomizeMode() )
				KillTimer(ID_TIMER_ANIMATION);
				m_AnimationType = g_DefAnimationType;
			} // if( m_bAnimFinished )

			if( GetSafeHwnd() != NULL )
			{
				if( m_bAnimFinished )
					_EndAnimation();
				else
				{
					Invalidate( FALSE );
					UpdateWindow();
				}
				g_nLastAnimTime = nCurrAnimTime;
			}
		} // if( ! m_bAnimFinished )
		
		if( m_bAnimFinished )
			if( IsKindOf(RUNTIME_CLASS(CExtPopupMenuWnd)) )
				((CExtPopupMenuWnd *)this)->
					GetSite().SetAnimated( NULL );

		return;
	// case ID_TIMER_ANIMATION
	default:
		__BaseClassOfCExtPopupBaseWnd::OnTimer(nIDEvent);
	break; // default
	} // switch( nIDEvent )
}

int CExtPopupBaseWnd::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	ASSERT_VALID( this );

	pDesktopWnd;
	nHitTest;
	message;
	if( _FindCustomizeMode() )
	{
		if( IsKindOf(RUNTIME_CLASS(CExtPopupMenuWnd)) )
			((CExtPopupMenuWnd*)this)->_DoResetChainPositions();
		else
			SetWindowPos(
				&CWnd::wndTopMost,
				-1, -1, -1, -1,
				SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE
				);
		return MA_NOACTIVATE;
	}

HWND hWndInPlace = ::GetWindow( m_hWnd, GW_CHILD );
	if( hWndInPlace != NULL )
	{
		CPoint ptScreenClick;
		if( ! ::GetCursorPos(&ptScreenClick) )
			return MA_NOACTIVATEANDEAT;
		HWND hWndFromPoint = ::WindowFromPoint( ptScreenClick );
		if(		hWndFromPoint != NULL
			&&	(::__EXT_MFC_GetWindowLong(hWndFromPoint,GWL_STYLE)&WS_CHILD) != 0
			&&	::GetParent(hWndFromPoint) == m_hWnd
			)
		return MA_ACTIVATE;
	} // if( hWndInPlace != NULL )

	return MA_NOACTIVATEANDEAT;
	//return CExtPopupBaseWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CExtPopupMenuWnd::_DoShowChain(
	int nCmdShow // = SW_SHOWNA
	)
{
	ASSERT_VALID( this );
	if( m_hWnd == NULL || (! ::IsWindow(m_hWnd) ) )
		return;
	ShowWindow( nCmdShow );
	if( m_nCurIndex < 0 )
		return;
MENUITEMDATA & mi = ItemGetInfo( m_nCurIndex );
	if( ! mi.IsPopup() )
		return;
	mi.GetPopup()->_DoShowChain( nCmdShow );
}

void CExtPopupMenuWnd::_DoResetChainPositions()
{
	ASSERT_VALID( this );
CExtPopupMenuWnd * pChild = NULL;
	if( m_nCurIndex >= 0 )
	{
		MENUITEMDATA & mi = ItemGetInfo( m_nCurIndex );
		if( mi.IsPopup() )
		{
			pChild = mi.GetPopup();
			if(		pChild->GetSafeHwnd() == NULL
				||	(! pChild->IsWindowVisible() )
				)
				pChild = NULL;
		} // if( mi.IsPopup() )
	} // if( m_nCurIndex >= 0 )
	if( pChild != NULL )
		pChild->_DoResetChainPositions();
	_DoResetChainPositionsImpl( NULL );
}

void CExtPopupMenuWnd::_DoResetChainPositionsImpl(
	CExtPopupMenuWnd * pPrev
	)
{
	SetWindowPos(
		( pPrev == NULL ) ? (&CWnd::wndTopMost) : pPrev,
		0, 0, 0, 0,
		SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE
			|SWP_NOREDRAW
		);
	{ // BLOCK : DC painting
		CClientDC dc( this );
		_DoPaint( dc );
	} // BLOCK : DC painting
	if( m_pWndParentMenu != NULL )
		m_pWndParentMenu->_DoResetChainPositionsImpl( this );
}

BOOL CExtPopupBaseWnd::OnEraseBkgnd(CDC* pDC) 
{
	ASSERT_VALID( this );

	pDC;
	return TRUE;
}

void CExtPopupBaseWnd::OnNcPaint() 
{
	ASSERT_VALID( this );
}

void CExtPopupBaseWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	ASSERT_VALID( this );

	bCalcValidRects;
	lpncsp;
///	__BaseClassOfCExtPopupBaseWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void CExtPopupBaseWnd::OnSize(UINT nType, int cx, int cy) 
{
	ASSERT_VALID( this );
	__BaseClassOfCExtPopupBaseWnd::OnSize(nType, cx, cy);
	Invalidate(FALSE);
}

bool CExtPopupBaseWnd::OnQueryLayoutRTL() const
{
	ASSERT_VALID( this );
bool bRTL =
		( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 )
			? true : false;
	return bRTL;
}

BOOL CExtPopupBaseWnd::DestroyWindow()
{
__PROF_UIS_MANAGE_STATE;
	if( m_ctrlShadow.GetSafeHwnd() )
		m_ctrlShadow.DestroyWindow();
HWND hWndOwn = GetSafeHwnd();
	if( hWndOwn == NULL || (! ::IsWindow( hWndOwn ) ) )
		return TRUE;
	return ::DestroyWindow( hWndOwn );
}

void CExtPopupBaseWnd::PostNcDestroy() 
{
	ASSERT_VALID( this );
}

void CExtPopupMenuWnd::PostNcDestroy() 
{
	ASSERT_VALID( this );
	if( _FindCustomizeMode() )
		return;
	if(		m_bTopLevel
		&&	( ! _IsFadeOutAnimation() )
		)
	{
		CExtPopupMenuSite & _site = GetSite();
		DWORD dwTrackFlags = TrackFlagsGet();
		ASSERT( m_hWndCmdReceiver != NULL );
		if( ::IsWindow(m_hWndCmdReceiver) )
			::SendMessage(
				m_hWndCmdReceiver,
				g_nMsgNotifyMenuClosed,
				0,
				LPARAM( this )
				);
		if(		m_hWndNotifyMenuClosed != NULL
			&&	::IsWindow( m_hWndNotifyMenuClosed )
			)
			::SendMessage(
				m_hWndNotifyMenuClosed,
				g_nMsgNotifyMenuClosed,
				0,
				LPARAM( this )
				);
		if(		( ! _site.IsEmpty() )
			&&	( ! _site.IsShutdownMode() )
			&&	(dwTrackFlags&TPMX_NO_SITE) == 0
			)
		{
			_site.DoneInstance();
			ASSERT( _site.IsEmpty() );
			ASSERT( ! _site.IsShutdownMode() );
		}
	} // if( m_bTopLevel )
}

void CExtPopupMenuWnd::_DeleteFadeOutMenu()
{
	ASSERT_VALID( this );
	delete this;
}

bool CExtPopupBaseWnd::_FindHelpMode() const
{
//	if( ::AfxGetApp()->m_bHelpMode )
//		return true;
	return false;
}

bool CExtPopupMenuWnd::_FindHelpMode() const
{
DWORD dwTrackFlags = TrackFlagsGet();
	if( (dwTrackFlags&TPMX_HELP_CTX_MODE) != 0 )
		return true;
	if( m_pWndParentMenu != NULL )
		return m_pWndParentMenu->_FindHelpMode();
	return CExtPopupBaseWnd::_FindHelpMode();
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * CExtPopupMenuWnd::_FindCustomizeSite() const
{
	if( m_pCustomizeSite != NULL )
		return (CExtCustomizeSite *)m_pCustomizeSite;
	if( m_pWndParentMenu != NULL )
		return m_pWndParentMenu->_FindCustomizeSite();
	return NULL;
}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

bool CExtPopupBaseWnd::_FindCustomizeMode() const
{
	return false;
}

INT CExtPopupMenuWnd::_FindChildPopup( const CExtPopupMenuWnd * pPopup ) const
{
	ASSERT_VALID( this );
	if( pPopup == NULL )
		return -1;
	ASSERT_VALID( pPopup );
INT nIndex, nCount = ItemGetCount();
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		const MENUITEMDATA & _mii = ItemGetInfo( nIndex );
		if(! _mii.IsPopup() )
			continue;
		const CExtPopupMenuWnd * pChildPopup = _mii.GetPopup();
		if( LPVOID(pChildPopup) == LPVOID(pPopup) )
			return nIndex;
	}
	return -1;
}

bool CExtPopupMenuWnd::_FindCustomizeMode() const
{
DWORD dwTrackFlags = TrackFlagsGet();
	if( (dwTrackFlags&TPMX_CUSTOMIZE_MODE) != 0 )
		return true;
	if( m_pWndParentMenu != NULL )
		return m_pWndParentMenu->_FindCustomizeMode();
	return false;
}

BOOL CExtPopupBaseWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	ASSERT_VALID( this );
	if( _FindHelpMode() )
	{
		SetCursor( afxData.hcurHelp );
		return TRUE;
	}
	return __BaseClassOfCExtPopupBaseWnd::OnSetCursor(pWnd, nHitTest, message);
}

/////////////////////////////////////////////////////////////////////////////
// CExtPopupMenuTipWnd

IMPLEMENT_DYNCREATE(CExtPopupMenuTipWnd, CExtPopupBaseWnd)

bool CExtPopupMenuTipWnd::g_bEnableAdvTips = true;

CExtPopupMenuTipWnd::CExtPopupMenuTipWnd()
	: m_bFlipHorz( false )
	, m_bFlipVert( false )
	, m_sizeClassicMargins( 3, 2 )
	, m_sizeRounders( 12, 10 )
	, m_sizeLeader( 25, 25 )
	, m_nIconMarginDX( 5 )
	, m_nBalloonDistance( 12 )
	, m_nClassicDistance( 13 )
	, m_eTS( CExtPopupMenuTipWnd::__ETS_BALLOON )
	, m_nSizeShadow( -1 )
	, m_ttLA( 0 )
	, m_bPassiveMode( false )
	, m_bDelayedLayeredBehavior( false )
	, m_ptGuideLines( -32767, -32767 )
	, m_nTransparencyKey( BYTE(0x0FF) )
	, m_rcAlignment( 0, 0, 0, 0 )
	, m_nPeriodDelayShowQuick( ID_PERIOD_DELAY_SHOW_QUICK )
	, m_nPeriodDelayShowNormal( ID_PERIOD_DELAY_SHOW_NORMAL )
	, m_hWndLastParent( NULL )
	, m_bNoHideDetection( false )
{
	m_AnimationType = __AT_NONE;
	m_bAnimFinished = true;
// HICON hIcon =
// 		::LoadIcon(
// 			NULL,
// 			IDI_INFORMATION
// 			);
// 	if( hIcon != NULL )
// 	{
// 		m_sizeRenderingIcon.cx = 16;
// 		m_sizeRenderingIcon.cy = 16;
// 		m_icon.m_bmpNormal.AssignFromHICON( hIcon );
// 		::DestroyIcon( hIcon );
// 		CExtBitmap::Filter _f( CExtBitmap::Filter::hermite );
// 		m_icon.m_bmpNormal.Scale( 14, 14, _f );
// 	}

BYTE arrIconInformation[]=
{
0x42,0x4D,0x36,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x10,0x00,
0x00,0x00,0x01,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,
0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x54,0x29,0x14,0x00,0x51,0x26,0x12,0x1B,0x56,0x2A,0x15,0x52,0x55,0x2A,
0x15,0x0F,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,
0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x54,0x29,
0x14,0x00,0x51,0x26,0x11,0x36,0x9A,0x6B,0x45,0xD1,0x75,0x47,0x29,0xCF,0x50,0x26,0x12,0x19,0x55,0x2A,0x15,0x00,0x55,0x2A,
0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,
0x15,0x00,0x55,0x2A,0x15,0x00,0x54,0x29,0x14,0x00,0x50,0x25,0x11,0x00,0x4C,0x21,0x0C,0x34,0x91,0x66,0x45,0xE4,0xE8,0xBC,
0x8E,0xFF,0x78,0x4A,0x2C,0xCD,0x4E,0x23,0x10,0x1F,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,
0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x53,0x27,0x13,0x00,0x4F,0x23,0x0F,0x17,0x58,0x2C,
0x16,0x66,0x69,0x3F,0x27,0x9C,0x7B,0x54,0x3A,0xE0,0xC8,0xA4,0x81,0xFF,0xEB,0xC3,0x99,0xFF,0x86,0x5B,0x3D,0xF1,0x50,0x24,
0x0F,0xB1,0x4F,0x24,0x10,0x65,0x54,0x29,0x14,0x18,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,
0x15,0x00,0x50,0x24,0x10,0x00,0x5D,0x32,0x1A,0x44,0x8B,0x65,0x4A,0xC8,0xAF,0x8F,0x71,0xFF,0xC7,0xA7,0x87,0xFF,0xD7,0xB8,
0x97,0xFF,0xE8,0xC8,0xA5,0xFF,0xF4,0xD0,0xAA,0xFF,0xCF,0xAC,0x8A,0xFF,0xAA,0x85,0x64,0xFF,0x7A,0x51,0x34,0xFB,0x53,0x27,
0x11,0xBA,0x53,0x28,0x13,0x3B,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x50,0x24,0x10,0x00,0x70,0x46,0x2D,0x51,0xB2,0x94,
0x79,0xF1,0xD8,0xBD,0xA2,0xFF,0xED,0xD1,0xB3,0xFF,0xE8,0xC0,0x9E,0xFF,0xDD,0xA3,0x7A,0xFF,0xEF,0xB4,0x8A,0xFF,0xED,0xB2,
0x87,0xFF,0xE9,0xBD,0x97,0xFF,0xEA,0xCA,0xA7,0xFF,0xD3,0xB2,0x91,0xFF,0x9D,0x76,0x56,0xFF,0x58,0x2C,0x15,0xD2,0x52,0x27,
0x13,0x36,0x55,0x2A,0x15,0x00,0x63,0x39,0x22,0x25,0xB9,0x9F,0x86,0xEB,0xDF,0xC7,0xB0,0xFF,0xFB,0xDF,0xC2,0xFF,0xFF,0xE6,
0xC8,0xFF,0xF3,0xD7,0xB9,0xFF,0xAD,0x64,0x3C,0xFF,0x9F,0x33,0x0D,0xFF,0xB3,0x65,0x3A,0xFF,0xF1,0xD0,0xAD,0xFF,0xFF,0xE1,
0xBE,0xFF,0xF6,0xD3,0xB2,0xFF,0xDB,0xBE,0x9E,0xFF,0x9B,0x75,0x56,0xFF,0x52,0x26,0x10,0xAB,0x54,0x2A,0x15,0x0E,0xA2,0x83,
0x6A,0x9C,0xDD,0xCA,0xB6,0xFF,0xFB,0xE3,0xCB,0xFF,0xFF,0xE7,0xCD,0xFF,0xFF,0xE4,0xCA,0xFF,0xFF,0xF1,0xD6,0xFF,0xBF,0x8A,
0x67,0xFF,0x84,0x19,0x00,0xFF,0xC3,0x88,0x63,0xFF,0xFF,0xEE,0xD1,0xFF,0xFF,0xE0,0xC0,0xFF,0xFF,0xE1,0xC1,0xFF,0xF6,0xD7,
0xB8,0xFF,0xD7,0xBA,0x9C,0xFF,0x73,0x49,0x30,0xEE,0x50,0x25,0x10,0x42,0xCC,0xB8,0xA5,0xE7,0xEF,0xDD,0xCB,0xFF,0xFF,0xEC,
0xD7,0xFF,0xFF,0xE9,0xD3,0xFF,0xFF,0xE8,0xD1,0xFF,0xFF,0xF2,0xDC,0xFF,0xBD,0x88,0x67,0xFF,0x88,0x1D,0x00,0xFF,0xC3,0x89,
0x66,0xFF,0xFF,0xEE,0xD5,0xFF,0xFF,0xE4,0xC8,0xFF,0xFF,0xE3,0xC7,0xFF,0xFF,0xE4,0xC7,0xFF,0xEF,0xD6,0xBA,0xFF,0xA1,0x7E,
0x64,0xFD,0x4E,0x22,0x0D,0x5D,0xD3,0xC6,0xB9,0xF0,0xF8,0xE7,0xD8,0xFF,0xFF,0xEF,0xDE,0xFF,0xFF,0xED,0xDB,0xFF,0xFF,0xEC,
0xD8,0xFF,0xFF,0xFA,0xE8,0xFF,0xC1,0x8F,0x71,0xFF,0x88,0x1C,0x00,0xFF,0xC3,0x8A,0x69,0xFF,0xFF,0xF2,0xDC,0xFF,0xFF,0xE7,
0xCF,0xFF,0xFF,0xE7,0xCE,0xFF,0xFF,0xE8,0xCF,0xFF,0xF8,0xE1,0xC9,0xFF,0xAD,0x8F,0x78,0xFE,0x4D,0x21,0x0C,0x5E,0xD5,0xCB,
0xC1,0xF1,0xF9,0xED,0xE1,0xFF,0xFF,0xF3,0xE6,0xFF,0xFF,0xF1,0xE3,0xFF,0xFF,0xF3,0xE4,0xFF,0xEE,0xDD,0xCB,0xFF,0xA3,0x59,
0x34,0xFF,0x81,0x13,0x00,0xFF,0xBC,0x86,0x66,0xFF,0xFF,0xF7,0xE6,0xFF,0xFF,0xEB,0xD7,0xFF,0xFF,0xEA,0xD5,0xFF,0xFF,0xED,
0xD7,0xFF,0xFA,0xE9,0xD3,0xFF,0xAF,0x93,0x7D,0xF9,0x4D,0x21,0x0C,0x50,0xDB,0xCF,0xC2,0xD5,0xF4,0xEE,0xE7,0xFF,0xFF,0xF9,
0xF0,0xFF,0xFF,0xF4,0xEA,0xFF,0xFF,0xF9,0xEF,0xFF,0xDC,0xC4,0xB3,0xFF,0xB5,0x89,0x74,0xFF,0xAE,0x84,0x70,0xFF,0xD1,0xB5,
0xA2,0xFF,0xFF,0xF6,0xE7,0xFF,0xFF,0xEF,0xDF,0xFF,0xFF,0xED,0xDC,0xFF,0xFF,0xF6,0xE5,0xFF,0xFB,0xEE,0xDC,0xFF,0x9F,0x7F,
0x69,0xD3,0x4D,0x21,0x0B,0x1C,0xE3,0xCF,0xBB,0x6A,0xE8,0xE5,0xE2,0xFF,0xFE,0xFC,0xF8,0xFF,0xFF,0xFA,0xF5,0xFF,0xFF,0xF7,
0xEF,0xFF,0xFF,0xFE,0xF8,0xFF,0xF7,0xE5,0xD8,0xFF,0xF3,0xCC,0xB3,0xFF,0xFF,0xF0,0xE3,0xFF,0xFF,0xF5,0xEA,0xFF,0xFF,0xF2,
0xE6,0xFF,0xFF,0xF8,0xEF,0xFF,0xFF,0xFA,0xF1,0xFF,0xE7,0xD7,0xC7,0xFF,0x6A,0x41,0x2B,0x67,0x50,0x25,0x10,0x00,0xEC,0xD2,
0xB8,0x04,0xE7,0xDB,0xCF,0xB0,0xEF,0xEE,0xEC,0xFF,0xFE,0xFE,0xFD,0xFF,0xFF,0xFE,0xFB,0xFF,0xFF,0xFF,0xFF,0xFF,0x9C,0x61,
0x43,0xFF,0x8F,0x21,0x00,0xFF,0xC7,0x99,0x80,0xFF,0xFF,0xFF,0xFD,0xFF,0xFF,0xFC,0xF8,0xFF,0xFF,0xFF,0xFC,0xFF,0xFA,0xF2,
0xE8,0xFF,0x97,0x77,0x62,0x91,0x4D,0x20,0x0B,0x04,0x55,0x2A,0x15,0x00,0xED,0xD3,0xBA,0x00,0xED,0xD4,0xBC,0x0C,0xE7,0xDD,
0xD2,0x9B,0xF2,0xF1,0xF0,0xFF,0xFB,0xFC,0xFD,0xFF,0xFE,0xFF,0xFF,0xFF,0xC6,0xB2,0xA8,0xFF,0x8A,0x5C,0x4F,0xFF,0xDF,0xD2,
0xCB,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xEE,0xE5,0xDD,0xF1,0x95,0x76,0x64,0x76,0x51,0x24,0x0F,0x03,0x54,0x29,
0x14,0x00,0x55,0x2A,0x15,0x00,0xED,0xD3,0xBA,0x00,0xED,0xD4,0xBD,0x00,0xEA,0xD8,0xC6,0x00,0xCB,0xB6,0xA8,0x3D,0xE1,0xD9,
0xD1,0xA1,0xF1,0xEE,0xEB,0xE1,0xFA,0xFC,0xFD,0xEE,0xFB,0xFF,0xFF,0xEE,0xFA,0xFA,0xFA,0xEE,0xED,0xE5,0xDD,0xD1,0xAE,0x97,
0x89,0x88,0x68,0x40,0x2C,0x26,0x4C,0x20,0x0A,0x00,0x54,0x29,0x14,0x00,0x55,0x2A,0x15,0x00,0x55,0x2A,0x15,0x00,0x00
};
	m_sizeRenderingIcon.cx = 16;
	m_sizeRenderingIcon.cy = 16;
	VERIFY(
		m_icon.m_bmpNormal.LoadBMP_Buffer(
			arrIconInformation,
			sizeof(arrIconInformation) / sizeof(arrIconInformation[0])
			)
		);

	g_listAllTips.AddTail( this );
}

CExtPopupMenuTipWnd::~CExtPopupMenuTipWnd()
{
POSITION pos = g_listAllTips.Find( this );
	if( pos != NULL )
		g_listAllTips.RemoveAt( pos );
}

BEGIN_MESSAGE_MAP(CExtPopupMenuTipWnd, CExtPopupBaseWnd)
	//{{AFX_MSG_MAP(CExtPopupMenuTipWnd)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATEAPP()
END_MESSAGE_MAP()

HRGN CExtPopupMenuTipWnd::CalcRegion(
	CDC & dc,
	CSize * pSize // = NULL
	)
{
	__PROF_UIS_MANAGE_STATE;
//	ASSERT_VALID( this );
HRGN hRegion = NULL;
	m_bFlipHorz = false;
	m_bFlipVert = false;
CWnd * pWndSearchMon = GetParent();
	ASSERT_VALID( pWndSearchMon );
	ASSERT( pWndSearchMon->GetSafeHwnd() != NULL );
CExtPaintManager::monitor_parms_t _mp;
	CExtPaintManager::stat_GetMonitorParms( _mp, pWndSearchMon );
CRect rcDesktop =
		g_bUseDesktopWorkArea
			? _mp.m_rcWorkArea
			: _mp.m_rcMonitor
			;
CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );
CRect rcTextMeasure( 0, 0, 0, 0 );
CFont * pAdvTipFont = &m_fontAdvTip;
	if( pAdvTipFont->GetSafeHandle() == NULL )
		pAdvTipFont = pPM->AdvTip_GetFont( this );
	if( pAdvTipFont->GetSafeHandle() == NULL )
		pAdvTipFont = &(pPM->m_FontNormal);
CFont * pOldFont = dc.SelectObject( pAdvTipFont );
	if( ! m_sText.IsEmpty() )
		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC,
			LPCTSTR(m_sText), m_sText.GetLength(),
			&rcTextMeasure,
			DT_LEFT|DT_CALCRECT, 0
			);
	dc.SelectObject( pOldFont );
CRect rcWnd = rcTextMeasure;
e_tip_style_t eTS = GetTipStyle();
	switch( eTS )
	{
	case __ETS_BALLOON:
	case __ETS_BALLOON_NO_ICON:
	{
		rcWnd.InflateRect( m_sizeRounders );
		if( eTS != __ETS_BALLOON_NO_ICON && (! m_icon.IsEmpty() ) )
		{
			rcWnd.right += m_sizeRenderingIcon.cx + m_nIconMarginDX;
			if( rcWnd.Height() < m_sizeRenderingIcon.cy )
				rcWnd.bottom = rcWnd.top + m_sizeRenderingIcon.cy;
		}
		POINT ptLeader[3];
		ptLeader[0].x = rcWnd.Width() - m_sizeRounders.cx;
		ptLeader[0].y = rcWnd.Height() - m_sizeRounders.cy;
		ptLeader[1].x = ptLeader[0].x;
		ptLeader[1].y = ptLeader[0].y + m_sizeLeader.cy;
		ptLeader[2].x = ptLeader[0].x - m_sizeLeader.cx;
		ptLeader[2].y = rcWnd.Height() - m_sizeRounders.cy;
		CRect rcRoundRectRgn( CPoint( 0, 0 ), rcWnd.Size() );
		CRect rcRectRgn( 0, 0, rcWnd.Width(), rcWnd.Height() + m_sizeLeader.cy );
		if( ( m_ptTrack.x - rcWnd.Width() ) < rcDesktop.left )
		{
			m_bFlipHorz = true;
			ptLeader[0].x = m_sizeRounders.cx;
			ptLeader[0].y = rcWnd.Height() - m_sizeRounders.cy;
			ptLeader[1].x = ptLeader[0].x;
			ptLeader[1].y = ptLeader[0].y + m_sizeLeader.cy;
			ptLeader[2].x = ptLeader[0].x + m_sizeLeader.cx;
			ptLeader[2].y = ptLeader[0].y;
		}
		if( ( m_ptTrack.y - rcWnd.Height() - m_sizeRounders.cy * 2 ) < rcDesktop.top )
		{
			m_bFlipVert = true;
			ptLeader[0].y -= rcWnd.Height() - m_sizeRounders.cy * 2;
			ptLeader[1].y = ptLeader[0].y - m_sizeLeader.cy;
			ptLeader[2].y -= ptLeader[0].y;
		}
		CRgn rgnLeader, rgnCaption;
		rgnCaption.CreateRoundRectRgn(
			rcRoundRectRgn.left,
			rcRoundRectRgn.top,
			rcRoundRectRgn.right,
			rcRoundRectRgn.bottom,
			m_sizeRounders.cx,
			m_sizeRounders.cy
			);
		rgnLeader.CreatePolygonRgn(
			ptLeader,
			3,
			ALTERNATE
			);
		hRegion = ::CreateRectRgnIndirect( &rcRectRgn );
		if( hRegion == NULL )
		{
			ASSERT( FALSE );
			return NULL;
		}
		::CombineRgn(
			hRegion,
			rgnCaption. operator HRGN (),
			rgnLeader. operator HRGN (),
			RGN_OR
			);
		if( m_bFlipVert )
			::OffsetRgn(
				hRegion,
				0,
				m_sizeLeader.cy
				);
		if( pSize != NULL )
		{
			pSize->cx = rcWnd.Width();
			pSize->cy = rcWnd.Height() + m_sizeLeader.cy;
		}
	} // cases __ETS_BALLOON, __ETS_BALLOON_NO_ICON
	break;
	case __ETS_RECTANGLE:
	case __ETS_RECTANGLE_NO_ICON:
	case __ETS_INV_RECTANGLE:
	case __ETS_INV_RECTANGLE_NO_ICON:
	{
		if(		eTS != __ETS_RECTANGLE_NO_ICON
			&&	eTS != __ETS_INV_RECTANGLE_NO_ICON
			&&	(! m_icon.IsEmpty() ) )
		{
			rcWnd.right += m_sizeRenderingIcon.cx + m_nIconMarginDX;
			if( rcWnd.Height() < m_sizeRenderingIcon.cy )
				rcWnd.bottom = rcWnd.top + m_sizeRenderingIcon.cy;
		}
		rcWnd.InflateRect( m_sizeClassicMargins );
		int nSizeShadow = CalcShadowSize();
		ASSERT( nSizeShadow >= 0 );
		rcWnd.InflateRect( 0, 0, nSizeShadow, nSizeShadow );
		CRect rcRectRgn( CPoint( 0, 0 ), rcWnd.Size() );
		hRegion =  ::CreateRectRgnIndirect( &rcRectRgn );
		if( hRegion != NULL )
			PmBridge_GetPM()->AdvTip_CalcRgn(
				hRegion,
				rcRectRgn,
				m_ctrlShadow.IsAvailable()
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
					&& ( ! IsKindOf( RUNTIME_CLASS( CExtPopupKeyTipWnd ) ) )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
					,
				nSizeShadow,
				this
				);
		if( hRegion == NULL )
		{
			ASSERT( FALSE );
			return NULL;
		}
		if( ( m_ptTrack.x + rcWnd.Width() ) > rcDesktop.right )
			m_bFlipHorz = true;
		if( ( m_ptTrack.y + rcWnd.Height() ) > rcDesktop.bottom )
			m_bFlipVert = true;
		if( pSize != NULL )
		{
			pSize->cx = rcWnd.Width();
			pSize->cy = rcWnd.Height();
		}
	} // rectangle cases
	break;
	default:
		ASSERT( FALSE );
		return NULL;
	} // switch( eTS )
	return hRegion;
}

void CExtPopupMenuTipWnd::_EndAnimation()
{
	//ASSERT_VALID( this );
	if(		(	m_eTS == __ETS_RECTANGLE
			||	m_eTS == __ETS_RECTANGLE_NO_ICON
			||	m_eTS == __ETS_INV_RECTANGLE
			||	m_eTS == __ETS_INV_RECTANGLE_NO_ICON
			)
		&&	m_ctrlShadow.IsAvailable()
		&&	m_ctrlShadow.GetSafeHwnd() == NULL
		)
	{
		INT nSizeShadow = m_nSizeShadow;
		if( nSizeShadow < 0 )
			nSizeShadow = PmBridge_GetPM()->GetTipShadowSize();
		if( nSizeShadow > 0 )
			m_ctrlShadow.Create( m_hWnd, nSizeShadow );
	}
}

void CExtPopupMenuTipWnd::_DoPaint(
	CDC & dcPaint,
	bool bUseBackBuffer //= true
	)
{
	//ASSERT_VALID( this );
	bUseBackBuffer;
CDC & dc = dcPaint;
CRect rcLayout;
	GetClientRect( &rcLayout );
HRGN hRegion = CreateRectRgnIndirect( &rcLayout );
	GetWindowRgn( hRegion );
CRgn * pRegion = CRgn::FromHandle( hRegion );

CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );
CFont * pAdvTipFont = &m_fontAdvTip;
	if( pAdvTipFont->GetSafeHandle() == NULL )
		pAdvTipFont = pPM->AdvTip_GetFont( this );
	if( pAdvTipFont->GetSafeHandle() == NULL )
		pAdvTipFont = &(pPM->m_FontNormal);
COLORREF clrAdvTipText = pPM->AdvTip_GetTextColor( this );
	if( clrAdvTipText == COLORREF(-1L) )
		clrAdvTipText = pPM->GetColor( COLOR_INFOTEXT, this );
e_tip_style_t eTS = GetTipStyle();
	switch( eTS )
	{
	case __ETS_BALLOON:
	case __ETS_BALLOON_NO_ICON:
	{
		CBrush brushWindow;
		brushWindow.CreateSolidBrush(
			//pPM->GetColor( COLOR_INFOBK, this )
			::GetSysColor( COLOR_INFOBK )
			);
		CBrush brushFrame;
		brushFrame.CreateSolidBrush(
			//pPM->GetColor( COLOR_3DDKSHADOW, this )
			::GetSysColor( COLOR_3DDKSHADOW )
			);
		CBrush brushInnerFrame;
		brushInnerFrame.CreateSolidBrush(
			//pPM->GetColor( COLOR_3DFACE, this)
			::GetSysColor( COLOR_3DFACE )
			);
		// frame
		dc.FillRgn( pRegion, &brushWindow );
		dc.FrameRgn( pRegion, &brushInnerFrame, 3, 3 );
		dc.FrameRgn( pRegion, &brushFrame, 1, 1 );
		// adjust icon's area
		rcLayout.DeflateRect( m_sizeRounders.cx, m_sizeRounders.cy, 0, 0 );
		if( eTS != __ETS_BALLOON_NO_ICON && (! m_icon.IsEmpty() ) )
			rcLayout.left += m_sizeRenderingIcon.cx + m_nIconMarginDX;
		CFont * pOldFont = dc.SelectObject( pAdvTipFont );
		int nOldBkMode = dc.SetBkMode( TRANSPARENT );
		int nIconVertPos = m_sizeRounders.cy;
		if( m_bFlipVert )
		{
			rcLayout.OffsetRect( 0, m_sizeLeader.cy );
			nIconVertPos += m_sizeLeader.cy;
		}
		COLORREF clrOldTextColor = dc.SetTextColor( clrAdvTipText );
		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC,
			LPCTSTR(m_sText), m_sText.GetLength(),
			&rcLayout,
			DT_TOP|DT_LEFT, 0
			);
		dc.SetBkMode( nOldBkMode );
		dc.SetTextColor( clrOldTextColor );
		dc.SelectObject( pOldFont );
		// icon
		if( eTS != __ETS_BALLOON_NO_ICON && (! m_icon.IsEmpty() ) )
		{
			m_icon.Paint(
				pPM,
				dc.GetSafeHdc(),
 				m_sizeRounders.cx,
 				nIconVertPos,
 				m_sizeRenderingIcon.cx,
 				m_sizeRenderingIcon.cy
				);
		}
	} // cases __ETS_BALLOON, __ETS_BALLOON_NO_ICON
	break;
	case __ETS_RECTANGLE:
	case __ETS_RECTANGLE_NO_ICON:
	case __ETS_INV_RECTANGLE:
	case __ETS_INV_RECTANGLE_NO_ICON:
	{
		int nSizeShadow = CalcShadowSize();
		ASSERT( nSizeShadow >= 0 );
		if( nSizeShadow > 0 )
		{
			rcLayout.DeflateRect( 0, 0, nSizeShadow, nSizeShadow );
			m_shadow.Paint(
				pPM,
				dc,
				rcLayout,
				CRect( 0, 0, 0, 0 ),
				CRect( 0, 0, 0, 0 ),
				nSizeShadow,
				70,
				95
				);
		}

		bool bAdvTipWithShadow =
			(	m_nSizeShadow > 0
			||	(	m_ctrlShadow.GetSafeHwnd() != NULL
				&&	( m_ctrlShadow.GetStyle() & WS_VISIBLE ) != 0
				)
			) ? true : false;
		if(	! pPM->AdvTip_PaintBackground(
				dc,
				rcLayout,
				bAdvTipWithShadow,
				this
				)
			)
		{
			// frame
			//dc.FillRgn( pRegion, &brushWindow );
			//dc.FrameRgn( pRegion, &brushFrame, 1, 1 );
			dc.FillSolidRect(
				&rcLayout,
				::GetSysColor( COLOR_INFOBK )
				);
			dc.Draw3dRect(
				&rcLayout,
				pPM->GetColor( COLOR_3DDKSHADOW, this ),
				pPM->GetColor( COLOR_3DDKSHADOW, this )
				);
		}

		// adjust icon's area
		rcLayout.DeflateRect( m_sizeClassicMargins );
		if(		eTS != __ETS_RECTANGLE_NO_ICON
			&&	eTS != __ETS_INV_RECTANGLE_NO_ICON
			&&	(! m_icon.IsEmpty() )
			)
			rcLayout.left += m_sizeRenderingIcon.cx + m_nIconMarginDX;
		
		CFont * pOldFont = dc.SelectObject( pAdvTipFont );
		int nOldBkMode = dc.SetBkMode( TRANSPARENT );
		COLORREF clrOldTextColor = dc.SetTextColor( clrAdvTipText );
		CExtRichContentLayout::stat_DrawText(
			dc.m_hDC,
			LPCTSTR(m_sText), m_sText.GetLength(),
			&rcLayout,
			DT_TOP|DT_LEFT|DT_VCENTER, 0
			);
		dc.SetBkMode( nOldBkMode );
		dc.SetTextColor( clrOldTextColor );
		dc.SelectObject( pOldFont );

		// icon
		if(		eTS != __ETS_RECTANGLE_NO_ICON
			&&	eTS != __ETS_INV_RECTANGLE_NO_ICON
			&&	(! m_icon.IsEmpty() )
			)
		{
			int nIconHorzPos =
				rcLayout.left
				- m_sizeRenderingIcon.cx
				- m_nIconMarginDX
				;
			int nIconVertPos =
				rcLayout.top
				+ ( rcLayout.Height() - m_sizeRenderingIcon.cy ) / 2
				;
			// ICON 2.53
// 			HICON hIcon = m_icon.GetIcon();
// 			if( hIcon != NULL )
// 				::DrawIconEx(
// 					dc.GetSafeHdc(),
// 					nIconHorzPos,
// 					nIconVertPos,
// 					hIcon,
// 					m_sizeRenderingIcon.cx,
// 					m_sizeRenderingIcon.cy,
// 					0,
// 					NULL,
// 					DI_NORMAL
// 					);
			m_icon.Paint(
				pPM,
				dc.GetSafeHdc(),
				nIconHorzPos,
				nIconVertPos,
				m_sizeRenderingIcon.cx,
				m_sizeRenderingIcon.cy
				);
		}
	} // rectangle cases
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( eTS )
	::DeleteObject( hRegion );
}

__EXT_MFC_SAFE_LPCTSTR CExtPopupMenuTipWnd::GetText() const
{
	return LPCTSTR(m_sText);
}

void CExtPopupMenuTipWnd::SetText(
	__EXT_MFC_SAFE_LPCTSTR lpszText
	)
{
	//ASSERT_VALID( this );
	m_sText = ( lpszText == NULL ) ? _T("") : lpszText;
	m_sText.TrimLeft();
	m_sText.TrimRight();
}

void CExtPopupMenuTipWnd::SetIcon(
	HICON hIcon,
	bool bCloneIcon // = false
	) 
{
	//ASSERT_VALID( this );
	if( ! m_icon.IsEmpty() )
		m_icon.Empty();
	if( hIcon == NULL )
		return;
	m_icon.AssignFromHICON( hIcon, bCloneIcon );
	m_sizeRenderingIcon = m_icon.GetSize();
	if( m_hWnd != NULL && ::IsWindow( m_hWnd ) )
		RedrawWindow();
}

void CExtPopupMenuTipWnd::SetIcon(
	CExtCmdIcon & _icon
	)
{
	//ASSERT_VALID( this );
	m_icon = _icon;
	m_sizeRenderingIcon = m_icon.GetSize();
	if( m_hWnd != NULL && ::IsWindow( m_hWnd ) )
		RedrawWindow();
}

CExtPopupMenuTipWnd::e_tip_style_t CExtPopupMenuTipWnd::GetTipStyle() const
{
	//ASSERT_VALID( this );
	return m_eTS;
}

void CExtPopupMenuTipWnd::SetTipStyle( CExtPopupMenuTipWnd::e_tip_style_t eTS )
{
	//ASSERT_VALID( this );
	m_eTS = eTS;
}

int CExtPopupMenuTipWnd::GetShadowSize() const
{
	//ASSERT_VALID( this );
	return m_nSizeShadow;
}

void CExtPopupMenuTipWnd::SetShadowSize( int nSizeShadow )
{
	//ASSERT_VALID( this );
	if( m_nSizeShadow == nSizeShadow )
		return;
	m_shadow.Destroy();
	m_nSizeShadow = nSizeShadow;
}

int CExtPopupMenuTipWnd::CalcShadowSize() const
{
	//ASSERT_VALID( this );
	if( m_ctrlShadow.IsAvailable() )
		return 0;
	if( m_nSizeShadow >= 0 )
		return m_nSizeShadow;
int nSizeShadow = PmBridge_GetPM()->GetTipShadowSize();
	if( nSizeShadow < 0 )
		return 0;
	return nSizeShadow;
}

CTypedPtrList < CPtrList, CExtPopupMenuTipWnd * > CExtPopupMenuTipWnd::g_listAllTips;

void CExtPopupMenuTipWnd::UpdateDelayedLayeredBehaviorAll(
	CRuntimeClass * pRTC // = NULL
	)
{
POSITION pos = g_listAllTips.GetHeadPosition();
	for( ; pos != NULL; )
	{
		CExtPopupMenuTipWnd * pWndPopupMenuTip = g_listAllTips.GetNext( pos );
		ASSERT_VALID( pWndPopupMenuTip );
		if( pRTC != NULL )
		{
			if( ! pWndPopupMenuTip->IsKindOf( pRTC ) )
				continue;
		}
		pWndPopupMenuTip->UpdateDelayedLayeredBehavior();
	}
}

void CExtPopupMenuTipWnd::UpdateDelayedLayeredBehavior()
{
	__PROF_UIS_MANAGE_STATE;
	//ASSERT_VALID( this );
	if(		( ! m_bDelayedLayeredBehavior )
		||	( ! g_PaintManager.m_bIsWin2000orLater )
		||	GetSafeHwnd() == NULL
		)
		return;
	ASSERT( g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL );
	g_PaintManager.m_pfnSetLayeredWindowAttributes( m_hWnd, 0, m_nTransparencyKey, __EXT_MFC_LWA_ALPHA );
}

bool CExtPopupMenuTipWnd::Show(
	CWnd * pWndParent,
	const RECT & rcExcludeArea,
	bool bNoDelay,
	HDWP & hPassiveModeDWP
	)
{
	__PROF_UIS_MANAGE_STATE;
	//ASSERT_VALID( this );
	ASSERT_VALID( pWndParent );
	ASSERT( pWndParent->GetSafeHwnd() != NULL );
	if( ! g_bEnableAdvTips )
	{
		Hide();
		return true;
	}
e_tip_style_t eTS = GetTipStyle();
	if( eTS == __ETS_NONE )
	{
		Hide();
		return true;
	}
bool bQuickDelay = false;
	if( GetSafeHwnd() != NULL )
	{
		if( m_rcExcludeArea == rcExcludeArea )
			return true;
		if(		( ! bNoDelay )
			&&	(GetStyle()&WS_VISIBLE) != 0
			)
			bQuickDelay = true;
		Hide();
		if( ( ! bNoDelay ) && ( ! bQuickDelay ) )
		{
			long ttLA = (long)::time( NULL );
			long ttDist = ttLA - m_ttLA;
			if( ttDist <= 1 )
			{
				bQuickDelay = true;
				m_ttLA = ttLA;
			} // if( ttDist <= 1 )
		} // if( ( ! bNoDelay ) && ( ! bQuickDelay ) )
	} // if( GetSafeHwnd() != NULL )
HCURSOR hCursor = ::LoadCursor( NULL, IDC_ARROW );
	ASSERT( hCursor != NULL );

	if(		GetSafeHwnd() != NULL
		&&	GetParent() != pWndParent
		)
	{
		Hide();
		DestroyWindow();
	}

	if( GetSafeHwnd() == NULL )
	{
		CExtSafeString strClassName = ::AfxRegisterWndClass ( __TIP_WNDCLASS_STYLES__, hCursor, NULL, NULL );
		bool bLayered = false, bForceLayered = false;
		if(		m_bPassiveMode
			&&	(	( g_PaintManager.m_bIsWin2000orLater && ( m_nTransparencyKey != BYTE(0x0FF) || m_bDelayedLayeredBehavior ) )
				||	( g_PaintManager.m_bIsWinVistaOrLater && g_PaintManager.m_DWM.IsCompositionEnabled() )
				)
			)
			bLayered = true;
		if(		(! bLayered)
			&&	g_PaintManager.m_bIsWin2000orLater
			&&	g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL
			&&	( m_AnimationType == __AT_NONE || m_AnimationType == __AT_CONTENT_DISPLAY )
			)
			bForceLayered = true;
		BOOL bCreateResult =
			CWnd::CreateEx(
				WS_EX_TOPMOST // | WS_EX_TRANSPARENT
					|	( g_PaintManager.m_bIsWin2000orLater ? (WS_EX_NOINHERITLAYOUT) : 0 )
					|	( ( bLayered || bForceLayered ) ? (__EXT_MFC_WS_EX_LAYERED) : 0 )
					,
				strClassName,
				NULL,
				WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
				0, 0, 0, 0,
				pWndParent->GetSafeHwnd(),
				NULL,
				NULL
				);
		ASSERT( bCreateResult );
		if( ! bCreateResult )
			return false;
		if( bForceLayered && (!bLayered) )
		{
			g_PaintManager.m_pfnSetLayeredWindowAttributes( m_hWnd, 0, 1, __EXT_MFC_LWA_ALPHA );
			ShowWindow( SW_SHOWNOACTIVATE );
			Invalidate( FALSE );
			UpdateWindow();
			g_PaintManager.m_pfnSetLayeredWindowAttributes( m_hWnd, 0, 255, __EXT_MFC_LWA_ALPHA );
		}
		if(		m_bPassiveMode
			&&	(	( g_PaintManager.m_bIsWin2000orLater && ( m_nTransparencyKey != BYTE(0x0FF) || m_bDelayedLayeredBehavior ) )
				||	( g_PaintManager.m_bIsWinVistaOrLater && g_PaintManager.m_DWM.IsCompositionEnabled() )
				)
			)
		{
			ASSERT( g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL );
			g_PaintManager.m_pfnSetLayeredWindowAttributes( m_hWnd, 0, 0, __EXT_MFC_LWA_ALPHA );
		}
	} // if( GetSafeHwnd() == NULL )
#ifdef _DEBUG
	else
	{
		ASSERT_VALID( pWndParent );
		ASSERT( pWndParent->GetSafeHwnd() != NULL );
	} // else from if( GetSafeHwnd() == NULL )
#endif // _DEBUG
	m_rcExcludeArea = rcExcludeArea;
	m_rcExcludeArea.NormalizeRect();
CExtPaintManager::monitor_parms_t _mp;
	CExtPaintManager::stat_GetMonitorParms( _mp, m_rcExcludeArea.CenterPoint() );
CRect rcDesktop =
		g_bUseDesktopWorkArea
			? _mp.m_rcWorkArea
			: _mp.m_rcMonitor
			;
CPoint ptTipTarget( m_rcExcludeArea.TopLeft() );
CPoint ptWndPos( 0, 0 );
CSize _sizeWindow( 0, 0 );
bool bIR = true;
	switch( eTS )
	{
	case __ETS_BALLOON:
	case __ETS_BALLOON_NO_ICON:
	{
		ptTipTarget +=
			CSize( m_nBalloonDistance/2, m_nBalloonDistance/2 );
		m_ptTrack = m_ptTrackOriginal = ptTipTarget;
		CDC * pDC = GetDC();
		ASSERT_VALID( pDC );
		HRGN hRegion = CalcRegion( *pDC, &_sizeWindow );
		ASSERT( hRegion != NULL );
		ReleaseDC(pDC);
		VERIFY(
			::SetWindowRgn(
				m_hWnd,
				hRegion,
				TRUE
				)
			);
		if( m_bFlipHorz )
			m_ptTrack.x +=
				m_rcExcludeArea.Width() - m_nBalloonDistance
				+ _sizeWindow.cx - m_sizeRounders.cx * 2
				;
		if( m_bFlipVert )
			m_ptTrack.y += 
				m_rcExcludeArea.Height() - m_nBalloonDistance
				+ _sizeWindow.cy - m_sizeRounders.cx * 2
				;
		ptWndPos.x = m_ptTrack.x - _sizeWindow.cx + m_sizeRounders.cx;
		ptWndPos.y = m_ptTrack.y - _sizeWindow.cy + m_sizeRounders.cy;
		SetWindowPos(
			&CWnd::wndTopMost,
			ptWndPos.x,
			ptWndPos.y,
			_sizeWindow.cx,
			_sizeWindow.cy,
			SWP_HIDEWINDOW|SWP_NOSENDCHANGING
				|SWP_NOZORDER|SWP_NOOWNERZORDER
				|SWP_NOACTIVATE //|SWP_NOCOPYBITS
			);
	} // cases __ETS_BALLOON, __ETS_BALLOON_NO_ICON
	break;
	case __ETS_RECTANGLE:
	case __ETS_RECTANGLE_NO_ICON:
		bIR = false; // continue cases
	case __ETS_INV_RECTANGLE:
	case __ETS_INV_RECTANGLE_NO_ICON:
	{
		CDC * pDC = GetDC();
		ASSERT_VALID( pDC );
		HRGN hRegion = CalcRegion( *pDC, &_sizeWindow );
		ASSERT( hRegion != NULL );
		ReleaseDC(pDC);
		ptTipTarget.x = m_rcExcludeArea.CenterPoint().x;
		if( m_bFlipHorz )
			ptTipTarget.x -=
				m_rcExcludeArea.Width()
				+ _sizeWindow.cx
				;
		if( bIR )
			ptTipTarget.y =
				m_rcExcludeArea.top
				- _sizeWindow.cy
				- 3
				;
		else
		{
			bool bResetY = true;
			POINT ptCursor = { 0, 0 };
			if(		::GetCursorPos( &ptCursor )
				&&	m_rcExcludeArea.PtInRect(ptCursor)
				)
			{
				CExtCmdIcon _icon;
				_icon.AssignFromHICON( (HICON)hCursor, false, true );
				if( ! _icon.IsEmpty() )
				{
					ptTipTarget.x = ptCursor.x;
					if( m_bFlipHorz )
						ptTipTarget.x -=
							_sizeWindow.cx
							;
					CSize _sizeIcon = _icon.GetSize();
					if( ! g_PaintManager.m_bIsWinXPorLater )
					{
						if( _sizeIcon.cx > 16 )
							_sizeIcon.cx = 16;
						if( _sizeIcon.cy > 16 )
							_sizeIcon.cy = 16;
					}
					ptTipTarget.y =
						ptCursor.y
						+ _sizeIcon.cy
						;
					if( ptTipTarget.y < m_rcExcludeArea.bottom )
						ptTipTarget.y = m_rcExcludeArea.bottom;
					bResetY = false;
				}
			}
			if( bResetY )
				ptTipTarget.y =
					m_rcExcludeArea.bottom
					+ m_nClassicDistance
					;
		}
		m_ptTrack = m_ptTrackOriginal = ptTipTarget;
		VERIFY(
			::SetWindowRgn(
				m_hWnd,
				hRegion,
				TRUE
				)
			);
		if( m_bFlipVert )
		{
			if( bIR )
				m_ptTrack.y =
					m_rcExcludeArea.bottom
					+ m_nClassicDistance
					;
			else
				m_ptTrack.y =
					m_rcExcludeArea.top
					- m_nClassicDistance
					;
		}
		if( (m_ptTrack.x + _sizeWindow.cx) > rcDesktop.right )
			m_ptTrack.x = rcDesktop.right - _sizeWindow.cx;
		if( m_ptTrack.x < rcDesktop.left )
			m_ptTrack.x = rcDesktop.left;
		if( (m_ptTrack.y + _sizeWindow.cy) > rcDesktop.bottom )
			m_ptTrack.y = rcDesktop.bottom - _sizeWindow.cy;
		if( m_ptTrack.y < rcDesktop.top )
			m_ptTrack.y = rcDesktop.top;
		ptWndPos = m_ptTrack;
		SetWindowPos(
			&CWnd::wndTopMost,
			ptWndPos.x,
			ptWndPos.y,
			_sizeWindow.cx,
			_sizeWindow.cy,
			SWP_HIDEWINDOW|SWP_NOSENDCHANGING
				|SWP_NOZORDER|SWP_NOOWNERZORDER
				|SWP_NOACTIVATE //|SWP_NOCOPYBITS
			);
	} // rectangle cases
	break;
	default:
		ASSERT( FALSE );
		return NULL;
	} // switch( eTS )

	if( hCursor != NULL )
		::DestroyCursor( hCursor );

	if( m_ptGuideLines.x != -32767 && m_ptGuideLines.y != -32767 )
	{
		CRect rcWnd(
			CPoint(
				m_ptGuideLines.x - _sizeWindow.cx / 2,
				m_ptGuideLines.y - _sizeWindow.cy / 2
				),
			_sizeWindow
			);
		MoveWindow( &rcWnd );
	}
	else
	{
		CRect rcWnd( ptWndPos, _sizeWindow );
		if( ! m_rcAlignment.IsRectEmpty() )
		{
			rcWnd.OffsetRect(
				m_rcAlignment.left - rcWnd.left,
				m_rcAlignment.bottom - rcWnd.top
				);
			m_rcAlignment.SetRect( 0, 0, 0, 0 );
		}
		CExtPaintManager::monitor_parms_t _mp;
		CExtPaintManager::stat_GetMonitorParms( _mp, m_rcExcludeArea );
		CRect rcDesktop = g_bUseDesktopWorkArea
			? _mp.m_rcWorkArea
			: _mp.m_rcMonitor
			;
		if( rcWnd.right > rcDesktop.right )
			rcWnd.OffsetRect(
				- ( rcWnd.right - rcDesktop.right ),
				0
				);
		if( rcWnd.left < rcDesktop.left )
			rcWnd.OffsetRect( rcDesktop.left - rcWnd.left, 0 );
		if( rcWnd.bottom > rcDesktop.bottom )
			rcWnd.OffsetRect(
				0,
				- ( rcWnd.bottom - rcDesktop.bottom )
				);
		if( rcWnd.top < rcDesktop.top )
			rcWnd.OffsetRect( 0, rcDesktop.top - rcWnd.top );
		MoveWindow( &rcWnd );
	}

	if( ! m_bPassiveMode )
	{
		if( bNoDelay )
			SendMessage( WM_TIMER, ID_TIMER_DELAY_SHOW );
		else
			SetTimer( ID_TIMER_DELAY_SHOW, bQuickDelay ? m_nPeriodDelayShowQuick : m_nPeriodDelayShowNormal, NULL );
		SetTimer( ID_TIMER_DELAY_KILL, ID_PERIOD_DELAY_KILL, NULL );
		SetTimer( ID_TIMER_LIFE_STEP, ID_PERIOD_LIFE_STEP, NULL );
	} // if( ! m_bPassiveMode )
	else
	{
		EnableWindow( FALSE );
		if(		( g_PaintManager.m_bIsWin2000orLater && ( m_nTransparencyKey != BYTE(0x0FF) || m_bDelayedLayeredBehavior ) )
			||	( g_PaintManager.m_bIsWinVistaOrLater && g_PaintManager.m_DWM.IsCompositionEnabled() )
			)
		{
			ASSERT( g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL );
			if( hPassiveModeDWP != NULL && g_PaintManager.m_bIsWinVistaOrLater )
			{
				g_PaintManager.m_pfnSetLayeredWindowAttributes( m_hWnd, 0, m_nTransparencyKey, __EXT_MFC_LWA_ALPHA );
				hPassiveModeDWP =
					::DeferWindowPos(
						hPassiveModeDWP, m_hWnd, NULL, 0, 0, 0, 0,
						SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE
							|SWP_NOZORDER|SWP_NOOWNERZORDER
							//|SWP_NOREDRAW
							|SWP_SHOWWINDOW
						);
			} // if( hPassiveModeDWP != NULL && g_PaintManager.m_bIsWinVistaOrLater )
			if(		hPassiveModeDWP == NULL
				||	m_bDelayedLayeredBehavior
				||	( ! g_PaintManager.m_bIsWinVistaOrLater )
				)
			{
				g_PaintManager.m_pfnSetLayeredWindowAttributes( m_hWnd, 0, 1, __EXT_MFC_LWA_ALPHA );
				ShowWindow( SW_SHOWNOACTIVATE );
				if( ! m_bDelayedLayeredBehavior )
				{
					Invalidate( FALSE );
					UpdateWindow();
					g_PaintManager.m_pfnSetLayeredWindowAttributes( m_hWnd, 0, m_nTransparencyKey, __EXT_MFC_LWA_ALPHA );
				}
			} // if( hPassiveModeDWP == NULL || ...
		}
		//ModifyStyleEx( 0, WS_EX_TRANSPARENT );
	} // else from if( ! m_bPassiveMode )
	m_hWndLastParent = pWndParent->GetSafeHwnd();
	return true;
}

void CExtPopupMenuTipWnd::Hide()
{
	//ASSERT_VALID( this );
	if( GetSafeHwnd() != NULL )
	{
		KillTimer( ID_TIMER_DELAY_SHOW );
		KillTimer( ID_TIMER_DELAY_KILL );
		KillTimer( ID_TIMER_LIFE_STEP );
		KillTimer( ID_TIMER_ANIMATION );
		SetWindowPos(
			NULL, 0, 0, 0, 0,
			SWP_HIDEWINDOW|SWP_NOSENDCHANGING
				|SWP_NOSIZE|SWP_NOMOVE
				|SWP_NOZORDER|SWP_NOOWNERZORDER
				|SWP_NOACTIVATE //|SWP_NOCOPYBITS
			);
//		CWnd * pWnd = GetParent();
//		CExtPopupMenuWnd * pPopup =
//			DYNAMIC_DOWNCAST(
//				CExtPopupMenuWnd,
//				pWnd
//				);
//		for( ; pPopup != NULL; pPopup = pPopup->GetParentMenuWnd() )
//			pPopup->_DelayPaint();
	}
	m_hWndLastParent = NULL;
	m_rcExcludeArea.SetRectEmpty();
	m_shadow.Destroy();
	if( m_ctrlShadow.GetSafeHwnd() != NULL )
		m_ctrlShadow.DestroyWindow();
}

void CExtPopupMenuTipWnd::OnTimer( __EXT_MFC_UINT_PTR nIDEvent ) 
{
	//ASSERT_VALID( this );

	switch( nIDEvent )
	{
	case ID_TIMER_LIFE_STEP:
		if(		CExtPopupMenuWnd::IsKeyPressed(VK_LBUTTON)
			||	CExtPopupMenuWnd::IsKeyPressed(VK_MBUTTON)
			||	CExtPopupMenuWnd::IsKeyPressed(VK_RBUTTON)
			)
		{
			HWND hWndCapture = ::GetCapture();
			if( hWndCapture != NULL && ::IsWindow( hWndCapture ) )
			{
				CWnd * pWndCapture = CWnd::FromHandlePermanent( hWndCapture );
				if(		pWndCapture != NULL
					&&	pWndCapture->IsKindOf( RUNTIME_CLASS(CExtScrollBar) )
					&&	((CExtScrollBar*)pWndCapture)->m_bCompleteRepaint
					)
					return;
			}
			Hide();
		}
		else if(
				CExtPopupMenuWnd::IsKeyPressed(VK_MENU)
			||	CExtControlBar::_DraggingGetBar() != NULL
			)
			Hide();
		else if( CExtPopupMenuWnd::IsMenuTracking() )
		{
			CWnd * pWndParent = GetParent();
			if(		pWndParent == NULL
				||	(! pWndParent->IsKindOf( RUNTIME_CLASS(CExtPopupMenuWnd) ) )
				)
				Hide();
		}
		return;
	case ID_TIMER_DELAY_SHOW:
		KillTimer( ID_TIMER_DELAY_SHOW );
		if( m_AnimationType != __AT_NONE )
		{
			SetWindowPos(
				NULL, 0, 0, 0, 0,
				SWP_SHOWWINDOW|SWP_NOSENDCHANGING|SWP_NOREDRAW
					|SWP_NOSIZE|SWP_NOMOVE
					|SWP_NOZORDER|SWP_NOOWNERZORDER
					|SWP_NOACTIVATE //|SWP_NOCOPYBITS
				);
//			if( g_DefAnimationType != CExtPopupMenuWnd::__AT_NONE )
				_StartAnimation();
			return;
		}
		if( g_PaintManager.m_bIsWin2000orLater && m_nTransparencyKey != BYTE(0x0FF) )
		{
			ASSERT( g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL );
			g_PaintManager.m_pfnSetLayeredWindowAttributes( m_hWnd,  0, 1, __EXT_MFC_LWA_ALPHA );
		}
		SetWindowPos(
			NULL, 0, 0, 0, 0,
			SWP_SHOWWINDOW|SWP_NOSENDCHANGING
				|SWP_NOSIZE|SWP_NOMOVE
				|SWP_NOZORDER|SWP_NOOWNERZORDER
				|SWP_NOACTIVATE //|SWP_NOCOPYBITS
			);
		Invalidate( FALSE );
		UpdateWindow();
		if( g_PaintManager.m_bIsWin2000orLater && m_nTransparencyKey != BYTE(0x0FF) )
		{
			ASSERT( g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL );
			g_PaintManager.m_pfnSetLayeredWindowAttributes( m_hWnd,  0, m_nTransparencyKey, __EXT_MFC_LWA_ALPHA );
			Invalidate( FALSE );
			UpdateWindow();
		}
		if(		(	m_eTS == __ETS_RECTANGLE
				||	m_eTS == __ETS_RECTANGLE_NO_ICON
				||	m_eTS == __ETS_INV_RECTANGLE
				||	m_eTS == __ETS_INV_RECTANGLE_NO_ICON
				)
			&& m_ctrlShadow.IsAvailable()
			)
		{
			INT nSizeShadow = m_nSizeShadow;
			if( nSizeShadow < 0 )
				nSizeShadow = PmBridge_GetPM()->GetTipShadowSize();
			if( nSizeShadow > 0 )
				m_ctrlShadow.Create( m_hWnd, nSizeShadow );
		}
		return;
	case ID_TIMER_DELAY_KILL:
		if( m_bNoHideDetection )
			return;
		POINT ptCursor = { 0, 0 };
		if(		::GetCursorPos( &ptCursor )
			&&	m_rcExcludeArea.PtInRect(ptCursor)
			)
			return;
		Hide();
		return;
	} // switch( nIDEvent )

	CExtPopupBaseWnd::OnTimer( nIDEvent );
}

#if _MFC_VER < 0x700
void CExtPopupMenuTipWnd::OnActivateApp(BOOL bActive, HTASK hTask) 
#else
void CExtPopupMenuTipWnd::OnActivateApp(BOOL bActive, DWORD hTask) 
#endif
{
	//ASSERT_VALID( this );
	bActive;
	hTask;
	Hide();
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

/////////////////////////////////////////////////////////////////////////////
// CExtPopupKeyTipWnd

IMPLEMENT_DYNCREATE( CExtPopupKeyTipWnd, CExtPopupMenuTipWnd );

CExtPopupKeyTipWnd::CExtPopupKeyTipWnd()
{
	m_bPassiveMode = true;
	SetShadowSize( 0 );
	SetTipStyle( __ETS_RECTANGLE_NO_ICON );
}

CExtPopupKeyTipWnd::~CExtPopupKeyTipWnd()
{
}

BEGIN_MESSAGE_MAP(CExtPopupKeyTipWnd,CExtPopupMenuTipWnd)
    //{{AFX_MSG_MAP(CExtPopupKeyTipWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

bool CExtPopupKeyTipWnd::Create(
	CWnd * pWndKeyTipParent,
	CPoint ptGuideLines,
	__EXT_MFC_SAFE_LPCTSTR strKeyTipText,
	bool bEnabled,
	HDWP & hPassiveModeDWP
	)
{
	ASSERT_VALID( this );
	ASSERT( strKeyTipText != NULL && _tcslen( strKeyTipText ) > 0 );
	ASSERT_VALID( pWndKeyTipParent );
	ASSERT( pWndKeyTipParent->GetSafeHwnd() != NULL );
	SetText( strKeyTipText );
	m_ptGuideLines = ptGuideLines;
CRect rcKeyTipExcludeArea( ptGuideLines, ptGuideLines );
	rcKeyTipExcludeArea.InflateRect( 1, 1 );
	m_nTransparencyKey = BYTE(0x0FF);
	m_bDelayedLayeredBehavior = true;
	if( ( ! bEnabled ) && g_PaintManager.m_bIsWin2000orLater )
		m_nTransparencyKey = BYTE(0x080);
	if( ! Show( pWndKeyTipParent, rcKeyTipExcludeArea, true, hPassiveModeDWP ) )
	{
		return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CExtPopupScreenTipWnd

IMPLEMENT_DYNCREATE( CExtPopupScreenTipWnd, CExtPopupMenuTipWnd );

CExtPopupScreenTipWnd::CExtPopupScreenTipWnd()
{
	m_AnimationType = g_PaintManager.m_bIsWin2000orLater ? __AT_NONE : __AT_FADE;
	m_nPeriodDelayShowQuick = ID_PERIOD_DELAY_SHOW_NORMAL;
	m_nPeriodDelayShowNormal = ID_PERIOD_DELAY_SHOW_RIBBON;
	m_sizeClassicMargins.cx = m_sizeClassicMargins.cy = 0;
	SetTipStyle( __ETS_RECTANGLE_NO_ICON );
}

CExtPopupScreenTipWnd::~CExtPopupScreenTipWnd()
{
}

BEGIN_MESSAGE_MAP(CExtPopupScreenTipWnd,CExtPopupMenuTipWnd)
    //{{AFX_MSG_MAP(CExtPopupScreenTipWnd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

HRGN CExtPopupScreenTipWnd::CalcRegion(
	CDC & dc,
	CSize * pSize // = NULL
	)
{
	__PROF_UIS_MANAGE_STATE;
//	ASSERT_VALID( this );
	if( m_cmdScreenTip.IsEmpty() )
		return CExtPopupMenuTipWnd::CalcRegion( dc, pSize );
HRGN hRegion = NULL;
	m_bFlipHorz = false;
	m_bFlipVert = false;
CWnd * pWndSearchMon = GetParent();
	ASSERT_VALID( pWndSearchMon );
	ASSERT( pWndSearchMon->GetSafeHwnd() != NULL );
CExtPaintManager::monitor_parms_t _mp;
	CExtPaintManager::stat_GetMonitorParms( _mp, pWndSearchMon );
CRect rcDesktop =
		g_bUseDesktopWorkArea
			? _mp.m_rcWorkArea
			: _mp.m_rcMonitor
			;
CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );
CRect rcWnd( CPoint(0,0), pPM->ScreenTip_Measure( dc, m_cmdScreenTip, this ) );
e_tip_style_t eTS = GetTipStyle();
	switch( eTS )
	{
	case __ETS_BALLOON:
	case __ETS_BALLOON_NO_ICON:
	{
		rcWnd.InflateRect( m_sizeRounders );
		if( eTS != __ETS_BALLOON_NO_ICON && (! m_icon.IsEmpty() ) )
		{
			rcWnd.right += m_sizeRenderingIcon.cx + m_nIconMarginDX;
			if( rcWnd.Height() < m_sizeRenderingIcon.cy )
				rcWnd.bottom = rcWnd.top + m_sizeRenderingIcon.cy;
		}
		POINT ptLeader[3];
		ptLeader[0].x = rcWnd.Width() - m_sizeRounders.cx;
		ptLeader[0].y = rcWnd.Height() - m_sizeRounders.cy;
		ptLeader[1].x = ptLeader[0].x;
		ptLeader[1].y = ptLeader[0].y + m_sizeLeader.cy;
		ptLeader[2].x = ptLeader[0].x - m_sizeLeader.cx;
		ptLeader[2].y = rcWnd.Height() - m_sizeRounders.cy;
		CRect rcRoundRectRgn( CPoint( 0, 0 ), rcWnd.Size() );
		CRect rcRectRgn( 0, 0, rcWnd.Width(), rcWnd.Height() + m_sizeLeader.cy );
		if( ( m_ptTrack.x - rcWnd.Width() ) < rcDesktop.left )
		{
			m_bFlipHorz = true;
			ptLeader[0].x = m_sizeRounders.cx;
			ptLeader[0].y = rcWnd.Height() - m_sizeRounders.cy;
			ptLeader[1].x = ptLeader[0].x;
			ptLeader[1].y = ptLeader[0].y + m_sizeLeader.cy;
			ptLeader[2].x = ptLeader[0].x + m_sizeLeader.cx;
			ptLeader[2].y = ptLeader[0].y;
		}
		if( ( m_ptTrack.y - rcWnd.Height() - m_sizeRounders.cy * 2 ) < rcDesktop.top )
		{
			m_bFlipVert = true;
			ptLeader[0].y -= rcWnd.Height() - m_sizeRounders.cy * 2;
			ptLeader[1].y = ptLeader[0].y - m_sizeLeader.cy;
			ptLeader[2].y -= ptLeader[0].y;
		}
		CRgn rgnLeader, rgnCaption;
		rgnCaption.CreateRoundRectRgn(
			rcRoundRectRgn.left,
			rcRoundRectRgn.top,
			rcRoundRectRgn.right,
			rcRoundRectRgn.bottom,
			m_sizeRounders.cx,
			m_sizeRounders.cy
			);
		rgnLeader.CreatePolygonRgn(
			ptLeader,
			3,
			ALTERNATE
			);
		hRegion = ::CreateRectRgnIndirect( &rcRectRgn );
		if( hRegion == NULL )
		{
			ASSERT( FALSE );
			return NULL;
		}
		::CombineRgn(
			hRegion,
			rgnCaption. operator HRGN (),
			rgnLeader. operator HRGN (),
			RGN_OR
			);
		if( m_bFlipVert )
			::OffsetRgn(
				hRegion,
				0,
				m_sizeLeader.cy
				);
		if( pSize != NULL )
		{
			pSize->cx = rcWnd.Width();
			pSize->cy = rcWnd.Height() + m_sizeLeader.cy;
		}
	} // cases __ETS_BALLOON, __ETS_BALLOON_NO_ICON
	break;
	case __ETS_RECTANGLE:
	case __ETS_RECTANGLE_NO_ICON:
	case __ETS_INV_RECTANGLE:
	case __ETS_INV_RECTANGLE_NO_ICON:
	{
		if(		eTS != __ETS_RECTANGLE_NO_ICON
			&&	eTS != __ETS_INV_RECTANGLE_NO_ICON
			&&	(! m_icon.IsEmpty() ) )
		{
			rcWnd.right += m_sizeRenderingIcon.cx + m_nIconMarginDX;
			if( rcWnd.Height() < m_sizeRenderingIcon.cy )
				rcWnd.bottom = rcWnd.top + m_sizeRenderingIcon.cy;
		}
		rcWnd.InflateRect( m_sizeClassicMargins );
		int nSizeShadow = CalcShadowSize();
		ASSERT( nSizeShadow >= 0 );
		rcWnd.InflateRect( 0, 0, nSizeShadow, nSizeShadow );
		CRect rcRectRgn( CPoint( 0, 0 ), rcWnd.Size() );
		hRegion =  ::CreateRectRgnIndirect( &rcRectRgn );
		if( hRegion != NULL )
			PmBridge_GetPM()->AdvTip_CalcRgn(
				hRegion,
				rcRectRgn,
				m_ctrlShadow.IsAvailable(),
				nSizeShadow,
				this
				);
		if( hRegion == NULL )
		{
			ASSERT( FALSE );
			return NULL;
		}
		if( ( m_ptTrack.x + rcWnd.Width() ) > rcDesktop.right )
			m_bFlipHorz = true;
		if( ( m_ptTrack.y + rcWnd.Height() ) > rcDesktop.bottom )
			m_bFlipVert = true;
		if( pSize != NULL )
		{
			pSize->cx = rcWnd.Width();
			pSize->cy = rcWnd.Height();
		}
	} // rectangle cases
	break;
	default:
		ASSERT( FALSE );
		return NULL;
	} // switch( eTS )
	return hRegion;
}

void CExtPopupScreenTipWnd::_DoPaint(
	CDC & dcPaint,
	bool bUseBackBuffer // = true
	)
{
	//ASSERT_VALID( this );
	if( m_cmdScreenTip.IsEmpty() )
	{
		CExtPopupMenuTipWnd::_DoPaint( dcPaint, bUseBackBuffer );
		return;
	}
CDC & dc = dcPaint;
CRect rcLayout;
	GetClientRect( &rcLayout );
HRGN hRegion = CreateRectRgnIndirect( &rcLayout );
	GetWindowRgn( hRegion );
CRgn * pRegion = CRgn::FromHandle( hRegion );

CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );
e_tip_style_t eTS = GetTipStyle();
	switch( eTS )
	{
	case __ETS_BALLOON:
	case __ETS_BALLOON_NO_ICON:
	{
		CBrush brushWindow;
		brushWindow.CreateSolidBrush(
			//pPM->GetColor( COLOR_INFOBK, this )
			::GetSysColor( COLOR_INFOBK )
			);
		CBrush brushFrame;
		brushFrame.CreateSolidBrush(
			//pPM->GetColor( COLOR_3DDKSHADOW, this )
			::GetSysColor( COLOR_3DDKSHADOW )
			);
		CBrush brushInnerFrame;
		brushInnerFrame.CreateSolidBrush(
			//pPM->GetColor( COLOR_3DFACE, this)
			::GetSysColor( COLOR_3DFACE )
			);
		// frame
		dc.FillRgn( pRegion, &brushWindow );
		dc.FrameRgn( pRegion, &brushInnerFrame, 3, 3 );
		dc.FrameRgn( pRegion, &brushFrame, 1, 1 );
		// adjust icon's area
		rcLayout.DeflateRect( m_sizeRounders.cx, m_sizeRounders.cy, 0, 0 );
		if( eTS != __ETS_BALLOON_NO_ICON && (! m_icon.IsEmpty() ) )
			rcLayout.left += m_sizeRenderingIcon.cx + m_nIconMarginDX;
		int nIconVertPos = m_sizeRounders.cy;
		if( m_bFlipVert )
		{
			rcLayout.OffsetRect( 0, m_sizeLeader.cy );
			nIconVertPos += m_sizeLeader.cy;
		}
		pPM->ScreenTip_Paint( dc, rcLayout, m_cmdScreenTip, this );
	} // cases __ETS_BALLOON, __ETS_BALLOON_NO_ICON
	break;
	case __ETS_RECTANGLE:
	case __ETS_RECTANGLE_NO_ICON:
	case __ETS_INV_RECTANGLE:
	case __ETS_INV_RECTANGLE_NO_ICON:
	{
		int nSizeShadow = CalcShadowSize();
		ASSERT( nSizeShadow >= 0 );
		if( nSizeShadow > 0 )
		{
			rcLayout.DeflateRect( 0, 0, nSizeShadow, nSizeShadow );
			m_shadow.Paint(
				pPM,
				dc,
				rcLayout,
				CRect( 0, 0, 0, 0 ),
				CRect( 0, 0, 0, 0 ),
				nSizeShadow,
				70,
				95
				);
		}
		pPM->ScreenTip_Paint( dc, rcLayout, m_cmdScreenTip, this );
	} // rectangle cases
	break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
	break;
#endif // _DEBUG
	} // switch( eTS )
	::DeleteObject( hRegion );
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

/////////////////////////////////////////////////////////////////////////////
// CExtPopupMenuWnd

CExtSafeString CExtPopupMenuWnd::g_strSpecSymbols( _T("~`!@#$%^&*()_-+={}[]:;\"'|\\<,>.?/ ") );

INT CExtPopupMenuWnd::g_nTimeDelayedDisplaying = 300;
INT CExtPopupMenuWnd::g_nTimeDelayedHiding = 300;

bool CExtPopupMenuWnd::g_bUseAcceleratedMenuScrolling = false;
INT CExtPopupMenuWnd::g_nAcceleratedIncreaseStep = 500;
INT CExtPopupMenuWnd::g_nAcceleratedIncreasePercent = 50;
INT CExtPopupMenuWnd::g_nAcceleratedPixelsPerStepMax = 400;

bool CExtPopupMenuWnd::g_bTranslateContextHelpCmd = false;
ACCEL CExtPopupMenuWnd::g_accelContextHelp = { FVIRTKEY, VK_F1, ID_HELP };

// is allowed menu item positioning without
// using only & - marked text
bool CExtPopupMenuWnd::g_bAllowNonAccelPositioning = false;

INT CExtPopupMenuWnd::g_nDefaultFadeOutAnimationStepCount = 8;
INT CExtPopupMenuWnd::g_nDefaultFadeOutAnimationEllapse = 20;

INT CExtPopupMenuWnd::g_nAutoExpandTime = 4000;

IMPLEMENT_DYNCREATE( CExtPopupMenuWnd, CExtPopupBaseWnd )

static CTypedPtrList < CPtrList, CExtPopupMenuWnd * > g_ListDetached;

CExtPopupMenuWnd::CExtPopupMenuWnd()
	: m_pWndParentMenu( NULL )
	, m_hWndCmdReceiver( NULL )
	, m_hWndNotifyMenuClosed( NULL )
	, m_nLeftAreaWidth( 0 )
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	, m_pNode( NULL )
	, m_pDragSrcNode( NULL )
	, m_nDragSrcIdx( -1 )
	, m_bHelperNoRemoveSrc( false )
	, m_bHelperDragOverPassed( false )
	, m_pCustomizeSite( NULL )
	, m_nHelperDropIndexBefore( -1 )
	, m_bHelperNoHideChild( false )
	, m_ptHelperDragStart( 0, 0 )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	, m_nAcceleratedPixelsPerStepCurrent( __SCROLLING_PIXEL_STEP )
	, m_nAcceleratedMilliSecondsPassed( 0 )
	, m_sMenuCaption( _T("") )
	, m_sizeCaptionMeasured( 0, 0 )
	, m_bHelperResizingMode( false )
	, m_sizeResizingMin( 10, 10 )
	, m_sizeResizingMax( 32767, 32767 )
	, m_bHelperAnimationControllerDetected( false )
	, m_nFadeOutAnimationStepIndex( 0 )
	, m_nFadeOutAnimationStepCount( g_nDefaultFadeOutAnimationStepCount )
	, m_nFadeOutAnimationEllapse( g_nDefaultFadeOutAnimationEllapse )
	, m_bCanceling( false )
	, m_bHelperMouseBtnDownOnStart( false )
	, m_ptStartMousePos( -32767, -32767 )
	, m_bHelperReCreateDynamicShadowAtTheEndOfNearestAnimation( false )
	, m_bEndSequenceInvoked( false )
{
	__VPC_INC;
	_Init();
	g_ListDetached.AddTail( this );
}

CExtPopupMenuWnd::~CExtPopupMenuWnd()
{
POSITION pos = g_ListDetached.Find( this );
	if( pos != NULL )
		g_ListDetached.RemoveAt( pos );

	_OnCancelMode( true );
	if( m_bTopLevel )
	{
		VERIFY( _BuildItems( NULL, true ) );
	}
	__VPC_DEC;
#ifdef _DEBUG
	if( m_bTopLevel )
	{
		__VPC_VERIFY_0;
	}
#endif // _DEBUG
	CExtPopupMenuSite::g_DefPopupMenuSite.FadeOutInstanceRemove( this );
}

bool CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND(
	HWND hWndTestFrom,
	HWND hWndActive, // = ::GetActiveWindow(),
	bool bCheckEnabled, // = true,
	bool bCheckVisible, // = true,
	bool bCheckExtPopupMenuTracking // = true
	)
{
	if( bCheckExtPopupMenuTracking )
	{
		if( CExtPopupMenuWnd::IsMenuTracking() )
			return false;
	} // if( bCheckExtPopupMenuTracking )
	
	if(		hWndTestFrom == NULL
		||	( ! ::IsWindow( hWndTestFrom ) )
		)
		return false;
	if(		hWndActive == NULL
		||	( ! ::IsWindow( hWndActive ) )
		)
		return false;

	if( bCheckEnabled )
	{
		if( ! ::IsWindowEnabled( hWndTestFrom ) )
			return false;
		if( ! ::IsWindowEnabled( hWndActive ) )
			return false;
	} // if( bCheckEnabled )

__EXT_MFC_LONG_PTR dwWndStyle;
	if( bCheckVisible )
	{
		dwWndStyle = ::__EXT_MFC_GetWindowLong( hWndTestFrom, GWL_STYLE );
		if( (dwWndStyle & WS_VISIBLE) == 0 )
			return false;

		dwWndStyle = ::__EXT_MFC_GetWindowLong( hWndActive, GWL_STYLE );
		if( (dwWndStyle & WS_VISIBLE) == 0 )
			return false;
	} // if( bCheckVisible )

const DWORD dwTestStyles = (WS_VISIBLE|WS_POPUP);

	for(	HWND hWndParentOfTest = ::GetParent( hWndTestFrom );
			hWndParentOfTest != NULL;
			hWndParentOfTest = ::GetParent( hWndParentOfTest )
			)
	{ // walk parent popup windows of hWndTestFrom
		ASSERT( ::IsWindow( hWndParentOfTest ) );
		dwWndStyle = ::__EXT_MFC_GetWindowLong( hWndParentOfTest, GWL_STYLE );
		if( (dwWndStyle&dwTestStyles) == 0L )
			continue;
		if( hWndParentOfTest == hWndActive )
			return true;

		for(	HWND hWndParentOfActive = hWndActive;
				hWndParentOfActive != NULL;
				hWndParentOfActive = ::GetParent( hWndParentOfActive )
				)
		{ // walk parent popup windows of hWndActive
			ASSERT( ::IsWindow( hWndParentOfActive ) );
			dwWndStyle = ::__EXT_MFC_GetWindowLong( hWndParentOfActive, GWL_STYLE );
			if( (dwWndStyle&dwTestStyles) == 0L )
				continue;
			if( hWndParentOfActive == hWndParentOfTest )
				return true;
		} // walk parent popup windows of hWndActive

	} // walk parent popup windows of hWndTestFrom

	return false;
}

void CExtPopupMenuWnd::_Init()
{
	ASSERT_VALID( this );

	m_nDyScrollOffset = 0;
	m_bTopLevel
		= m_bScrollingAvailable
		= m_bExpandAvailable
		= m_bExpandWasPressed
		= m_bCombineWithEA 
		= m_bSuspendTips
		= false;
	m_dwTrackFlags = 0;
	m_nWaitingExpandTickCount = 0;
	m_nCurIndex
		= m_nDelayedFocusItemIndex
		= m_nLastMousePick
		= IDX_NOTHING;
	m_bDelayedHiding = false;

	m_nScrollingDirection = 0;
	m_rcScrollTop.SetRectEmpty();
	m_rcScrollBottom.SetRectEmpty();
	m_rcExpandBtn.SetRectEmpty();
	m_rcTearOffBtn.SetRectEmpty();
	m_rcRibbonBtnOptions.SetRectEmpty();
	m_rcRibbonBtnExit.SetRectEmpty();

	m_pCbPaintCombinedCookie = NULL;
	m_pCbPaintCombinedContent = NULL;
	m_bCookieIsObject = false;

	_InitAnimation();
}

CObject * CExtPopupBaseWnd::GetCookieAsObject()
{
	if(		(!m_bCookieIsObject)
		||	m_pCbPaintCombinedCookie == NULL
		)
		return NULL;
CObject * pObj = (CObject*)m_pCbPaintCombinedCookie;
	ASSERT_VALID( pObj );
	return pObj;
}

bool CExtPopupBaseWnd::IsToolBtnCombinedArea() const
{
	if( !IsKindOf( RUNTIME_CLASS(CExtPopupMenuWnd) ) )
		return false;
CExtPopupMenuWnd * pPopup = (CExtPopupMenuWnd*)(this);
	if(		(LPVOID)(CExtPopupMenuWnd::stat_paint_combined_menu_toolbtn)
		!=	(LPVOID)(pPopup->GetCbPaintCombinedContent())
		)
		return false;
	return true;
}

int CExtPopupMenuWnd::_GetCurIndex() const
{
	ASSERT_VALID( this );

	return m_nCurIndex;
}

int CExtPopupMenuWnd::_GetSpecBtnHeight()
{
int nHeight = g_PaintManager->GetMenuExpandButtonHeight();
	ASSERT( nHeight > 0 );
	return nHeight;
}

CRect CExtPopupMenuWnd::OnQueryMenuBorderMetrics() const
{
	ASSERT_VALID( this );
CExtPaintManager * pPM = PmBridge_GetPM();
CRect rcMB = pPM->GetMenuBorderMetrics( (CWnd*)this );
	ASSERT( rcMB.left >= 0 );
	ASSERT( rcMB.top >= 0 );
	ASSERT( rcMB.right >= 0 );
	ASSERT( rcMB.bottom >= 0 );
CSize _sizeCaption = MenuCaptionMeasure();
	rcMB.top += _sizeCaption.cy;
INT nMarginHeight = 0, nResizingCornerPartWidth = 0;
	pPM->MenuBottomResizingMarginGetInfo(
			nMarginHeight,
			nResizingCornerPartWidth,
			const_cast < CExtPopupMenuWnd * > ( this )
			);
	rcMB.bottom += nMarginHeight;
	return rcMB;
}

__EXT_MFC_SAFE_LPCTSTR CExtPopupMenuWnd::MenuCaptionTextGet() const
{
	ASSERT_VALID( this );
	return LPCTSTR(m_sMenuCaption);
}

void CExtPopupMenuWnd::MenuCaptionTextSet(
	__EXT_MFC_SAFE_LPCTSTR sMenuCaption // = NULL
	)
{
	ASSERT_VALID( this );
	m_sizeCaptionMeasured.cx = m_sizeCaptionMeasured.cy = 0;
	m_sMenuCaption.Empty();
	if( sMenuCaption != NULL )
		m_sMenuCaption = sMenuCaption;
}

CSize CExtPopupMenuWnd::MenuCaptionMeasure() const
{
	ASSERT_VALID( this );
	if( m_sizeCaptionMeasured.cx > 0 && m_sizeCaptionMeasured.cy > 0 )
		return m_sizeCaptionMeasured;
CWindowDC dc( NULL );
	m_sizeCaptionMeasured =
		PmBridge_GetPM()->MenuCaption_Measure(
			dc,
			MenuCaptionTextGet(),
			const_cast < CExtPopupMenuWnd * > ( this )
			);
	return m_sizeCaptionMeasured;
}

void CExtPopupMenuWnd::MenuCaptionPaint(
	CDC & dc,
	CRect rcMenuCaption
	) const
{
	ASSERT_VALID( this );
	if(		m_sizeCaptionMeasured.cx <= 0
		||	m_sizeCaptionMeasured.cy <= 0
		||	rcMenuCaption.IsRectEmpty()
		||	( ! dc.RectVisible( &rcMenuCaption ) )
		)
		return;
	PmBridge_GetPM()->MenuCaption_Paint(
		dc,
		rcMenuCaption,
		MenuCaptionTextGet(),
		const_cast < CExtPopupMenuWnd * > ( this )
		);
}

void CExtPopupMenuWnd::_CachedRectsEmpty()
{
	ASSERT_VALID( this );
int nItemIndex = 0, nItemCount = (int)m_items_all.GetSize();
	for( ; nItemIndex < nItemCount; nItemIndex++ )
	{
		MENUITEMDATA & _mii = ItemGetInfo( nItemIndex );
		_mii.CacheRectsEmpty();
	} // for( ; nItemIndex < nItemCount; nItemIndex++ )
}

void CExtPopupMenuWnd::_RecalcTrackParms(
	bool bMoveWindow // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	m_ptTrack = m_ptTrackOriginal;
	m_eCombineAlign = __CMBA_NONE;
bool bPointAdjusted = true;
CSize _size = _CalcTrackSize();
	if( m_bExcludeAreaSpec )
	{
		bool bRTL = OnQueryLayoutRTL();
		if( bRTL && (!m_bTopLevel) )
		{
			CExtPopupMenuWnd * pTop = m_pWndParentMenu;
			for( ; pTop != NULL; pTop = pTop->GetParentMenuWnd() )
			{
				if( pTop->_IsTopLevelPopup() )
					break;
			} // for( ; pTop != NULL; pTop = pTop->GetParentMenuWnd() )
			if( pTop != NULL )
			{
				DWORD dwTrackFlags = pTop->TrackFlagsGet();
				switch( (dwTrackFlags & TPMX_ALIGN_MASK) )
				{
				case TPMX_TOPALIGN:
				case TPMX_BOTTOMALIGN:
					dwTrackFlags &= ~(TPMX_ALIGN_MASK);
					dwTrackFlags |= TPMX_RIGHTALIGN;
					pTop->TrackFlagsSet( dwTrackFlags );
				break;
				} // switch( (pTop->m_dwTrackFlags & TPMX_ALIGN_MASK) )
			} // if( pTop != NULL )
		} // if( bRTL && (!m_bTopLevel) )
		DWORD dwTrackFlags = TrackFlagsGet();
		switch( (dwTrackFlags & TPMX_ALIGN_MASK) )
		{
		case TPMX_LEFTALIGN:
			m_ptTrack.x = m_rcExcludeArea.right;
			m_ptTrack.y = m_rcExcludeArea.top;
		break;
		case TPMX_RIGHTALIGN:
			m_ptTrack.x = m_rcExcludeArea.left - _size.cx;
			m_ptTrack.y = m_rcExcludeArea.top;
		break;
		case TPMX_TOPALIGN:
			m_ptTrack.x = m_rcExcludeArea.left;
			m_ptTrack.y = m_rcExcludeArea.bottom;
		break;
		case TPMX_BOTTOMALIGN:
			m_ptTrack.x = m_rcExcludeArea.left;
			m_ptTrack.y = m_rcExcludeArea.top - _size.cy;
		break;
		default:
			bPointAdjusted = false;
		break;
		} // switch( (dwTrackFlags & TPMX_ALIGN_MASK) )
	} // if( m_bExcludeAreaSpec )
	if( ! bPointAdjusted )
	{
		DWORD dwTrackFlags = TrackFlagsGet();
		if( (dwTrackFlags & TPMX_ALIGN_MASK) == TPMX_RIGHTALIGN )
			m_ptTrack.x -= _size.cx;
		else
		{
			if( (dwTrackFlags & TPMX_ALIGN_MASK) == TPMX_CENTERALIGN )
				m_ptTrack.x -= _size.cx/2;
		}
		if( (dwTrackFlags & TPMX_ALIGN_MASK) == TPMX_BOTTOMALIGN )
			m_ptTrack.y -= _size.cy;
		else
		{
			if( (dwTrackFlags & TPMX_ALIGN_MASK) == TPMX_VCENTERALIGN )
				m_ptTrack.y -= _size.cy/2;
		}
	} // if( !bPointAdjusted )
	if( bMoveWindow )
	{
		CRect wr = _CalcTrackRect();
		MoveWindow( &wr );
	} // if( bMoveWindow )
}

void CExtPopupMenuWnd::_RecalcLayoutImpl()
{
	ASSERT_VALID( this );
CRect rcClient;
	_GetClientRect( &rcClient );
bool bTearOff = _IsTearOff();
int nTearOffCaptionHeight = bTearOff ? _GetTearOffCaptionHeight() : 0;
int nScrollBtnDy = _GetSpecBtnHeight();
CRect rcMB = OnQueryMenuBorderMetrics();
int nMenuShadowSize = 0;
//		OnQueryMenuShadowSize();

int nScrollBottomOffsetDy = 0;
	if( m_bExpandAvailable )
	{
		m_rcExpandBtn.left =
			rcClient.left + rcMB.left;
		m_rcExpandBtn.right =
			rcClient.right - (rcMB.right + nMenuShadowSize);
		m_rcExpandBtn.bottom =
			rcClient.bottom - (rcMB.bottom + nMenuShadowSize);
		m_rcExpandBtn.top =
			m_rcExpandBtn.bottom - nScrollBtnDy;
		nScrollBottomOffsetDy =
			- m_rcExpandBtn.Height();
		m_rcExpandBtn.left += m_nLeftAreaWidth;
		m_rcExpandBtn.OffsetRect(0,-1);
	} // if( m_bExpandAvailable )

	m_rcRibbonBtnOptions.SetRectEmpty();
	m_rcRibbonBtnExit.SetRectEmpty();
DWORD dwTrackFlags = TrackFlagsGet();
	if( (dwTrackFlags&TPMX_RIBBON_FILE_MENU) != 0 )
	{

		CRect rcAlign = rcClient;
		rcAlign.top = rcAlign.bottom - rcMB.bottom;
		rcAlign.right -= 7;
		rcAlign.DeflateRect( 0, 3 );
		if( (dwTrackFlags&TPMX_RIBBON_EXIT_BUTTON) != 0 )
		{
			INT nButtonWidth = 50;
			RIBBONFILEMENUBUTTONQUERY _rfmbq( this, TPMX_RIBBON_EXIT_BUTTON );
			if( _rfmbq.Notify() )
			{
				nButtonWidth = _rfmbq.m_nMeasuredWidth;
				ASSERT( nButtonWidth >= 0 );
			}
			m_rcRibbonBtnExit = rcAlign;
			m_rcRibbonBtnExit.left = m_rcRibbonBtnExit.right - nButtonWidth;
			rcAlign.right -= m_rcRibbonBtnExit.Width() + 5;
		} // if( (dwTrackFlags&TPMX_RIBBON_EXIT_BUTTON) != 0 )
		if( (dwTrackFlags&TPMX_RIBBON_OPTIONS_BUTTON) != 0 )
		{
			INT nButtonWidth = 50;
			RIBBONFILEMENUBUTTONQUERY _rfmbq( this, TPMX_RIBBON_OPTIONS_BUTTON );
			if( _rfmbq.Notify() )
			{
				nButtonWidth = _rfmbq.m_nMeasuredWidth;
				ASSERT( nButtonWidth >= 0 );
			}
			m_rcRibbonBtnOptions = rcAlign;
			m_rcRibbonBtnOptions.left = m_rcRibbonBtnOptions.right - nButtonWidth;
			rcAlign.right -= m_rcRibbonBtnOptions.Width() + 5;
		} // if( (dwTrackFlags&TPMX_RIBBON_OPTIONS_BUTTON) != 0 )
	} // if( (dwTrackFlags&TPMX_RIBBON_FILE_MENU) != 0 )

	if( m_bScrollingAvailable )
	{
		m_rcScrollTop.left = m_rcScrollBottom.left =
			rcClient.left + rcMB.left;
		m_rcScrollTop.right = m_rcScrollBottom.right =
			rcClient.right - (rcMB.right + nMenuShadowSize);

		m_rcScrollTop.top =
			rcClient.top + rcMB.top;
		m_rcScrollTop.bottom =
			m_rcScrollTop.top + nScrollBtnDy;
		
		m_rcScrollBottom.bottom =
			rcClient.bottom - (rcMB.bottom + nMenuShadowSize);
		m_rcScrollBottom.top =
			m_rcScrollBottom.bottom - nScrollBtnDy;

		m_rcScrollTop.DeflateRect(
			m_nLeftAreaWidth + 1,
			0,
			1,
			0
			);
		m_rcScrollBottom.DeflateRect(
			m_nLeftAreaWidth + 1,
			0,
			1,
			0
			);
		m_rcScrollBottom.OffsetRect( 0, nScrollBottomOffsetDy );
		m_rcScrollBottom.OffsetRect(0,-1);
		m_rcScrollTop.OffsetRect( 0, 1 );
		if( bTearOff )
		{
			m_rcScrollTop.OffsetRect( 0, nTearOffCaptionHeight );
			m_rcScrollBottom.OffsetRect( 0, nTearOffCaptionHeight );
		}
	} // if( m_bScrollingAvailable )

	if( bTearOff )
	{
		m_rcTearOffBtn.left =
			rcClient.left + rcMB.left;
		m_rcTearOffBtn.right =
			rcClient.right - (rcMB.right + nMenuShadowSize);
		m_rcTearOffBtn.top =
			rcClient.top + rcMB.top;
		m_rcTearOffBtn.bottom =
			m_rcTearOffBtn.top + nTearOffCaptionHeight;
		m_rcTearOffBtn.DeflateRect(
			m_nLeftAreaWidth + 1,
			0,
			1,
			0
			);
		m_rcTearOffBtn.OffsetRect( 0, 1 );
	}
}

class CExtPopupMenuWnd::CExtPopupMenuCmdUI : public CCmdUI
{
public: // re-implementations only
	HWND m_hWndCmdReceiver;
	CExtPopupMenuWnd * m_pPopup;
	CExtPopupMenuWnd::MENUITEMDATA * m_pItemDataActive;
	CExtPopupMenuCmdUI(
		HWND hWndCmdReceiver,
		CExtPopupMenuWnd * pPopup
		)
		: m_hWndCmdReceiver( hWndCmdReceiver )
		, m_pPopup( pPopup )
		, m_pItemDataActive( NULL )
	{
		ASSERT( m_hWndCmdReceiver != NULL && ::IsWindow(m_hWndCmdReceiver) );
		ASSERT_VALID( m_pPopup );
		CCmdUI::m_pOther = NULL; // menu
		CCmdUI::m_nIndexMax = (UINT)m_pPopup->ItemGetCount();
		CCmdUI::m_nIndex = 0;
	}
	void SetActiveIndex( int nIndex )
	{
		ASSERT( m_hWndCmdReceiver != NULL && ::IsWindow(m_hWndCmdReceiver) );
		ASSERT_VALID( m_pPopup );
		CCmdUI::m_nIndex = (UINT)nIndex;
		ASSERT( CCmdUI::m_nIndex >= 0 && CCmdUI::m_nIndex < CCmdUI::m_nIndexMax );
		m_pItemDataActive = &m_pPopup->ItemGetInfo(nIndex);
		ASSERT( m_pItemDataActive != NULL );
		CCmdUI::m_nID = m_pItemDataActive->GetCmdID();
		if(		m_pItemDataActive->IsPopup()
			||	m_pItemDataActive->IsSeparator()
			||	CExtCmdManager::IsSystemCommand( CCmdUI::m_nID )
			)
			return;
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd(m_hWndCmdReceiver),
				CCmdUI::m_nID
				);
		if( pCmdItem == NULL )
			return;
//		m_pItemDataActive->SetText( pCmdItem->m_sMenuText );
		m_pItemDataActive->SetAccelText( pCmdItem->m_sAccelText );
		m_pItemDataActive->AccelCharInit();
		int nIconAreaWidthSaved = m_pItemDataActive->GetIconAreaWidth();
		m_pItemDataActive->MeasureItem( NULL );
		m_pItemDataActive->UpdateIconAreaWidth( nIconAreaWidthSaved );
	}
	virtual void Enable(BOOL bOn)
	{
		CCmdUI::m_bEnableChanged = TRUE;
		ASSERT( m_hWndCmdReceiver != NULL && ::IsWindow(m_hWndCmdReceiver) );
		ASSERT_VALID( m_pPopup );
		ASSERT( m_pItemDataActive != NULL );
		if( m_pItemDataActive->IsPopup() && m_pItemDataActive->GetCmd() == NULL )
			return;
		if( m_pItemDataActive->IsForceEnabled() )
			bOn = TRUE;
		m_pItemDataActive->Enable( bOn ? true : false );
//		m_pItemDataActive->MeasureItem( NULL );
	}
	virtual void SetRadio(BOOL bOn)
	{
		ASSERT( m_hWndCmdReceiver != NULL && ::IsWindow(m_hWndCmdReceiver) );
		ASSERT_VALID( m_pPopup );
		ASSERT( m_pItemDataActive != NULL );
		if( m_pItemDataActive->IsPopup() )
			return;
		m_pItemDataActive->Radio( bOn ? true : false );
//		m_pItemDataActive->MeasureItem( NULL );
	}
	virtual void SetCheck(int nCheck)
	{
		ASSERT_VALID( m_pPopup );
		ASSERT( m_pItemDataActive != NULL );
		if( m_pItemDataActive->IsPopup() )
			return;
		m_pItemDataActive->Indeterminate( ( nCheck >= 2 ) ? true : false );
		m_pItemDataActive->Check( nCheck ? true : false );
//		m_pItemDataActive->MeasureItem( NULL );
	}
	virtual void SetText( LPCTSTR lpszText )
	{
		ASSERT( m_hWndCmdReceiver != NULL && ::IsWindow(m_hWndCmdReceiver) );
		ASSERT_VALID( m_pPopup );
		ASSERT( m_pItemDataActive != NULL );
		if( lpszText == NULL )
			return;
		if( m_pItemDataActive->IsPopup() )
			return;
		m_pItemDataActive->SetText( lpszText );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		CExtCustomizeCmdTreeNode * pNode = m_pItemDataActive->GetCmdNode();
		if( pNode != NULL )
		{
			ASSERT_VALID( pNode );
			pNode->SetTextInMenu( lpszText );
			pNode->SetTextInToolbar( lpszText );
		} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		m_pItemDataActive->MeasureItem( NULL );
	}
}; // class CExtPopupMenuWnd::CExtPopupMenuCmdUI

void CExtPopupMenuWnd::_UpdateCmdUI()
{
	ASSERT_VALID( this );

	if( _FindHelpMode() )
		return;

	if( m_bTopLevel )
	{
		DWORD dwTrackFlags = TrackFlagsGet();
		if( (dwTrackFlags&TPMX_NO_CMD_UI) != 0 )
			return;
	}
	ASSERT( m_hWndCmdReceiver != NULL );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
__EXT_MFC_SAFE_LPCTSTR strProfileName = g_CmdManager->ProfileNameFromWnd(m_hWndCmdReceiver);
CExtCustomizeSite * pSite = CExtCustomizeSite::GetCustomizeSite(m_hWndCmdReceiver);
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

BOOL bDisableIfNoHndler = TRUE;
CExtPopupMenuCmdUI _state( m_hWndCmdReceiver, this );

bool bResyncItems = false;
	for(	INT _iter = 0;
			_iter < m_items_all.GetSize();
			_iter++
			)
	{ // walk all items
		_state.SetActiveIndex( _iter );
		ASSERT( _state.m_pItemDataActive != NULL );
		if( _state.m_pItemDataActive->IsNoCmdUI() )
			continue;
		if( _state.m_pItemDataActive->IsSeparator() )
			continue;
		if( _state.m_pItemDataActive->IsPopup() )
		{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
			if(	/*	_state.m_pItemDataActive->IsInplaceEdit()
				&&*/	pSite != NULL
				&&	_state.m_pItemDataActive->GetCmdNode() != NULL
				)
			{
				UINT _nCmdIdSaved = _state.m_pItemDataActive->m_nCmdID;
				_state.m_nID =
					_state.m_pItemDataActive ->
						GetCmdNode() ->
							GetCmdID( false );
				ASSERT( CExtCmdManager::IsCommand( _state.m_nID ) );
				HWND hWndCmdReceiver =
					_state.m_pItemDataActive->GetCmdReceiver();
				ASSERT( hWndCmdReceiver != NULL );
				ASSERT( ::IsWindow(hWndCmdReceiver) );
				CWnd * pWndCmdReceiver =
					CWnd::FromHandle( hWndCmdReceiver );
				ASSERT_VALID( pWndCmdReceiver );
				_state.DoUpdate( pWndCmdReceiver, bDisableIfNoHndler );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
				if(		pSite != NULL
					&&	pSite->IsCustomizeMode()
					)
					_state.m_pItemDataActive->SetForceEnabled( true );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
				_state.m_pItemDataActive->m_nCmdID = _nCmdIdSaved;

				_state.m_pItemDataActive->GetPopup()->_UpdateCmdUI();
				
				continue;
			}
			else
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
			{
				_state.m_pItemDataActive->GetPopup()->_UpdateCmdUI();
				continue;
			}
		}
		if( _state.m_pItemDataActive->IsExtraMark() )
		{
			bResyncItems = true;
			continue;
		}
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		if(		pSite != NULL
			&&	pSite->IsUserBarCommand(
					_state.m_pItemDataActive->GetCmdID()
					)
			&&	strProfileName != NULL
			)
		{
			_state.m_pItemDataActive->SetForceEnabled( true );
			CExtCmdItem * pCmdItem =
				g_CmdManager->CmdGetPtr(
					strProfileName,
					_state.m_pItemDataActive->GetCmdID()
					);
			ASSERT( pCmdItem != NULL );
			CExtToolControlBar * pUserBar =
				pSite->GetUserBar( _state.m_pItemDataActive->GetCmdID() );
			ASSERT_VALID( pUserBar );
			bool bCheck = pUserBar->IsVisible() ? true : false;
			pCmdItem->StateSetCheck( bCheck );
			_state.SetCheck( bCheck );
			continue;
		}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		if(		CExtCmdManager::IsCommandNeedsSpecUpdate(
					_state.m_pItemDataActive->GetCmdID()
					)
			||	CExtCmdManager::IsSystemCommand(
					_state.m_pItemDataActive->GetCmdID()
					)
			)
			continue;

		HWND hWndCmdReceiver =
			_state.m_pItemDataActive->GetCmdReceiver();
		ASSERT( hWndCmdReceiver != NULL );
		ASSERT( ::IsWindow(hWndCmdReceiver) );
		CWnd * pWndCmdReceiver =
			CWnd::FromHandle( hWndCmdReceiver );
		ASSERT_VALID( pWndCmdReceiver );
		_state.DoUpdate( pWndCmdReceiver, bDisableIfNoHndler );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		if(		pSite != NULL
			&&	pSite->IsCustomizeMode()
			)
			_state.m_pItemDataActive->SetForceEnabled( true );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	}  // walk all items

	if( bResyncItems )
		_SyncItems();
}

bool CExtPopupMenuWnd::MENUITEMDATA::ConstructPopup()
{
	ASSERT( m_iconPopup.IsEmpty() );
	ASSERT( m_pWndChild == NULL );
	m_pWndChild =
		CExtPopupMenuWnd::InstantiatePopupMenu(
			m_hWndSpecCmdReceiver,
			RUNTIME_CLASS(CExtPopupMenuWnd),
			m_pOwner
			);
	ASSERT( m_hWndSpecCmdReceiver != NULL );
	ASSERT( ::IsWindow(m_hWndSpecCmdReceiver) );
	m_pWndChild->m_hWndCmdReceiver = m_hWndSpecCmdReceiver;
	
	return true;
}

void CExtPopupMenuWnd::MENUITEMDATA::DestroyPopup()
{
	VERIFY( SetPopupIcon(NULL) );
	if( m_pWndChild == NULL )
		return;
	if( m_pWndChild->GetSafeHwnd() != NULL )
	{
		CExtPopupMenuWnd * pPopup = m_pWndChild;
//		INT nSaved = pPopup->m_nFadeOutAnimationStepCount;
//		pPopup->m_nFadeOutAnimationStepCount = -1;
		pPopup->DestroyWindow();
//		pPopup->m_nFadeOutAnimationStepCount = nSaved;
	}
//	else
//		delete m_pWndChild;
	delete m_pWndChild;
	m_pWndChild = NULL;
}

void CExtPopupMenuWnd::MENUITEMDATA::SetCustomTip( __EXT_MFC_SAFE_LPCTSTR sTip )
{
	m_sCustomTipText.Empty();
	if( sTip != NULL )
		m_sCustomTipText = sTip;
}
__EXT_MFC_SAFE_LPCTSTR CExtPopupMenuWnd::MENUITEMDATA::GetCustomTip() const
{
	if( m_sCustomTipText.IsEmpty() )
		return NULL;
	return LPCTSTR( m_sCustomTipText );
}

void CExtPopupMenuWnd::MENUITEMDATA::SetExtendedText( __EXT_MFC_SAFE_LPCTSTR sExtendedText )
{
bool bMeasure = false;
	if( sExtendedText != NULL )
	{
		if( m_sExtendedText != LPCTSTR(sExtendedText) )
		{
			m_sExtendedText = sExtendedText;
			bMeasure = true;
		}
	}
	else if( ! m_sExtendedText.IsEmpty() )
	{
		m_sExtendedText.Empty();
		bMeasure = true;
	}
	if( bMeasure )
		MeasureItem( NULL );
}
__EXT_MFC_SAFE_LPCTSTR CExtPopupMenuWnd::MENUITEMDATA::GetExtendedText() const
{
	if( m_sExtendedText.IsEmpty() )
		return NULL;
	return LPCTSTR( m_sExtendedText );
}

void CExtPopupMenuWnd::MENUITEMDATA::GetTip( CExtSafeString & sTip ) const
{
	sTip = _T("");
	if( IsPopup() || IsSeparator() )
		return;
LPCTSTR sCustomTip = GetCustomTip();
	if( sCustomTip != NULL )
	{
		sTip = sCustomTip;
		return;
	}
	if( GetCmd() == NULL )
		return;
CExtCmdItem * pCmdItem = GetCmd();
	if( pCmdItem == NULL )
		return;
UINT nTranslatedResourceCmdID = UINT(-1);
	switch( pCmdItem->m_nCmdID )
	{
	case SC_CLOSE:
		nTranslatedResourceCmdID = AFX_IDS_SCCLOSE;
	break;
	case SC_SIZE:
		nTranslatedResourceCmdID = AFX_IDS_SCSIZE;
	break;
	case SC_MOVE:
		nTranslatedResourceCmdID = AFX_IDS_SCMOVE;
	break;
	case SC_MINIMIZE:
		nTranslatedResourceCmdID = AFX_IDS_SCMINIMIZE;
	break;
	case SC_MAXIMIZE:
		nTranslatedResourceCmdID = AFX_IDS_SCMAXIMIZE;
	break;
	case SC_RESTORE:
		nTranslatedResourceCmdID = AFX_IDS_SCRESTORE;
	break;
	case SC_NEXTWINDOW:
		nTranslatedResourceCmdID = AFX_IDS_SCNEXTWINDOW;
	break;
	case SC_PREVWINDOW:
		nTranslatedResourceCmdID = AFX_IDS_SCPREVWINDOW;
	break;
	case SC_TASKLIST:
		nTranslatedResourceCmdID = AFX_IDS_SCTASKLIST;
	break;
	} // switch( pCmdItem->m_nCmdID )
	if( nTranslatedResourceCmdID == UINT(-1) )
	{
		sTip = pCmdItem->m_sTipStatus;
		return;
	} // if( nTranslatedResourceCmdID == UINT(-1) )

	if( ! g_ResourceManager->LoadString( sTip, nTranslatedResourceCmdID ) )
	{
		sTip = pCmdItem->m_sTipStatus;
		return;
	}
	else
	{
		int nSep = sTip.ReverseFind('\n');
		if( nSep < 0 )
			return;
		CString sBuffer = sTip.Left( nSep );
		sTip = sBuffer;
	}
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

bool CExtPopupMenuWnd::MENUITEMDATA::InitSceenTip( CExtPopupScreenTipWnd & _wndScreenTip )
{
	if( m_pOwner->GetSafeHwnd() == NULL )
		return false;
	ASSERT_VALID( m_pOwner );
INT nOwnIndex = m_pOwner->ItemGetIndexOf( this );
	if( nOwnIndex < 0 )
		return false;
CExtCustomizeCmdTreeNode * pNode = GetCmdNode();
	if( pNode == NULL )
		return false;
	ASSERT_VALID( pNode );
CExtCustomizeCmdScreenTip * pCCST = pNode->CmdScreenTipFindForDisplaying();
	if( pCCST == NULL )
		return false;
	ASSERT_VALID( pCCST );
	if( pCCST->IsEmpty() )
		return false;
CRect rcOwn( 0, 0, 0, 0 );
	m_pOwner->_GetItemRect( nOwnIndex, rcOwn, true );
	if( rcOwn.IsRectEmpty() )
		return false;
	m_pOwner->ClientToScreen( &rcOwn );
	if( _wndScreenTip.GetSafeHwnd() != NULL )
	{
		CRect rcExisting = _wndScreenTip.GetExcludeArea();
		if( rcOwn == rcExisting )
			return true;
	}
	_wndScreenTip.Hide();
	_wndScreenTip.m_cmdScreenTip = (*pCCST);
	//if( CExtToolControlBar::g_bToolbarShortcutKeysOnScreenTips )
	{
		CExtCmdItem * pCmdItem = GetCmd();
		if(		pCmdItem != NULL
			&&	( ! pCmdItem->m_sAccelText.IsEmpty() )
			)
		{
			CExtSafeString str = _wndScreenTip.m_cmdScreenTip.CaptionMainGet();
			if( ! str.IsEmpty() )
			{
				str += _T(" (");
				str += pCmdItem->m_sAccelText;
				str += _T(")");
				_wndScreenTip.m_cmdScreenTip.CaptionMainSet( LPCTSTR( str ) );
			}
			else
			{
				CExtSafeString str = _wndScreenTip.m_cmdScreenTip.TextMainGet();
				if( ! str.IsEmpty() )
				{
					str += _T(" (");
					str += pCmdItem->m_sAccelText;
					str += _T(")");
					_wndScreenTip.m_cmdScreenTip.TextMainSet( LPCTSTR( str ) );
				}
			}
		}
	}
	_wndScreenTip.Show( m_pOwner, rcOwn );
	return true;
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

bool CExtPopupMenuWnd::_BuildItems(
	CMenu * pBuildMenu,
	bool bTopLevel,
	bool bNoRefToCmdMngr // = false
	)
{
	ASSERT_VALID( this );
#ifdef _DEBUG
	if( pBuildMenu != NULL )
	{
		ASSERT( m_hWndCmdReceiver != NULL );
		ASSERT( ::IsWindow(m_hWndCmdReceiver) );
	}
#endif // _DEBUG
INT iter = 0;
	for( ; iter < m_items_all.GetSize(); iter++)
	{
		MENUITEMDATA & mi = ItemGetInfo( iter );
		if( mi.IsPopup() )
		{
			CExtPopupMenuWnd * pPopup = mi.GetPopup();
			pPopup->m_hWndCmdReceiver = m_hWndCmdReceiver;
			INT nSaved = pPopup->m_nFadeOutAnimationStepCount;
			pPopup->m_nFadeOutAnimationStepCount = -1;
			pPopup->_OnCancelMode();
			pPopup->m_nFadeOutAnimationStepCount = nSaved;
			VERIFY( pPopup->_BuildItems( NULL, false, bNoRefToCmdMngr ) );
			mi.DestroyPopup();
		}
	} // for( ; iter < m_items_all.GetSize(); iter++)
	m_items_all.RemoveAll();

	if( pBuildMenu == NULL )
		return true;
	ASSERT( pBuildMenu->GetSafeHmenu() != NULL );

	m_bTopLevel = bTopLevel;
	if( m_bTopLevel )
		m_pWndParentMenu = NULL;

	// build items loop
bool bPrevWasSeparator = false;
int nMruUpdateIndex = -1, nInsertedIndex = 0;
int	nMenuItemCount = pBuildMenu->GetMenuItemCount();
	ASSERT( nMenuItemCount >= 0 );
	m_items_all.RemoveAll();
	for( int nItemIndex=0; nItemIndex<nMenuItemCount; nItemIndex++ )
	{
		UINT nMenuItemID =
			pBuildMenu->GetMenuItemID( nItemIndex );
		if(		ID_FILE_MRU_FIRST <= nMenuItemID
			&&	nMenuItemID <= ID_FILE_MRU_LAST
			)
		{
			if( nMruUpdateIndex < 0 )
				nMruUpdateIndex = nInsertedIndex;
			continue;
		}
		MENUITEMDATA mi( this );
		mi.SetCmdReceiver( m_hWndCmdReceiver );
		VERIFY(
			mi.UpdateFromMenu(
				m_hWndCmdReceiver,
				pBuildMenu,
				nItemIndex,
				bNoRefToCmdMngr
				)
			);
		bool bIsSeparator = mi.IsSeparator();
		if( bPrevWasSeparator && bIsSeparator )
				continue;
		bPrevWasSeparator = bIsSeparator;
		
		if( mi.IsPopup() )
		{
			CMenu * pSubMenu =
					pBuildMenu->GetSubMenu(nItemIndex);
			ASSERT( pSubMenu != NULL );
			ASSERT( pSubMenu->GetSafeHmenu() != NULL );
			mi.GetPopup()->m_hWndCmdReceiver = m_hWndCmdReceiver;
			mi.GetPopup()->_BuildItems( pSubMenu, false, bNoRefToCmdMngr );
			mi.GetPopup()->m_pWndParentMenu = this;
		}
		_InsertItem( -1, mi, bNoRefToCmdMngr );
		nInsertedIndex++;
	} // for( int nItemIndex=0; nItemIndex<nMenuItemCount; nItemIndex++ )
	ASSERT( m_items_all.GetSize() <= nMenuItemCount );

int nMruInsertCount = 0;
	if( nMruUpdateIndex >= 0 && (!bNoRefToCmdMngr) )
	{ // insert recently opened file items
		ASSERT( nMruUpdateIndex < nMenuItemCount );
		CRecentFileList * pRecentFileList =
			InternalFriendlyWinApp::_GetFriendlyApp()->
				_GetRecentFileList();
		if( pRecentFileList != NULL )
		{ // can be null !?
			int nRecentCount =
				pRecentFileList->GetSize();
			TCHAR sCurrDir[_MAX_PATH+1];
			::memset(sCurrDir,0,sizeof(sCurrDir));
			::GetCurrentDirectory(_MAX_PATH,sCurrDir);
			int nLenCurDir = (int)_tcslen(sCurrDir);
			for( int nItemIndex=0; nItemIndex<nRecentCount; nItemIndex++ )
			{
				CExtSafeString sDisplayName( _T("") );
				CExtSafeString sDisplayNameFullPath( _T("") );
				CExtSafeString sDisplayNameMenu( _T("") );
				CExtSafeString sDisplayNameTipTool( _T("") );

            sDisplayNameFullPath = ( ! pRecentFileList->m_arrNames[ nItemIndex ].IsEmpty() ) ? LPCTSTR( pRecentFileList->m_arrNames[ nItemIndex ] ) : _T("");
            pRecentFileList->GetDisplayName( *((CString *)&sDisplayName), nItemIndex, sCurrDir, nLenCurDir, TRUE );

            if( sDisplayName.IsEmpty() || sDisplayNameFullPath.IsEmpty() )
					continue;

            if( g_bMRU_UseFullPathsInMenu )
               sDisplayNameMenu = sDisplayNameFullPath;
            else 
               sDisplayNameMenu = sDisplayName;

            if( g_bMRU_UseFullPathsInTipTool )
               sDisplayNameTipTool = sDisplayNameFullPath;
            else 
               sDisplayNameTipTool = sDisplayName;


				UINT nCmdID = ID_FILE_MRU_FIRST + nItemIndex;
				ASSERT( nCmdID <= ID_FILE_MRU_LAST );
				CExtCmdItem * pCmdItem =
					g_CmdManager->CmdGetPtr(
						g_CmdManager->ProfileNameFromWnd( m_hWndCmdReceiver ),
						nCmdID
						);
				if( pCmdItem == NULL )
					pCmdItem =
						g_CmdManager->CmdAllocPtr(
							g_CmdManager->ProfileNameFromWnd( m_hWndCmdReceiver ),
							nCmdID
							);
				ASSERT( pCmdItem != NULL );
				if( pCmdItem == NULL )
					return false;
				int nDisplayIndex = nItemIndex+1;
				if( nDisplayIndex < 10 )
					pCmdItem->m_sMenuText.Format(
						_T("&%d %s"),
						nDisplayIndex,
						sDisplayNameMenu
						);
				else
					pCmdItem->m_sMenuText.Format(
						_T("%d&%d %s"),
						nDisplayIndex/10,
						nDisplayIndex%10,
						sDisplayNameMenu
						);
				
				CExtSafeString sRecentFileFmt1,sRecentFileFmt2;
				if( ! g_ResourceManager->LoadString( sRecentFileFmt1, IDS_RECENT_FILE_FMT_1 ) )
					sRecentFileFmt1 = _T("Recent file %d");

				if( ! g_ResourceManager->LoadString( sRecentFileFmt2, IDS_RECENT_FILE_FMT_2 ) )
					sRecentFileFmt2 = _T("Recent file %d (\"%s\")");

				pCmdItem->m_sToolbarText.Format(
					(LPCTSTR)sRecentFileFmt1,
					nItemIndex + 1
					);
				pCmdItem->m_sTipTool.Format(
					(LPCTSTR)sRecentFileFmt2,
					nItemIndex + 1,
					sDisplayNameTipTool
					);
				pCmdItem->m_sTipStatus = pCmdItem->m_sTipTool;

				BOOL bInsRetVal =
					ItemInsert(
						nCmdID,
						nMruUpdateIndex + nItemIndex
						);
				
				if( ! bInsRetVal )
				{
					ASSERT( FALSE );
					return false;
				}
				nMruInsertCount++;
			} // for( nItemIndex=0; nItemIndex<nRecentCount; nItemIndex++ )
		} // can be null !?
		if( nMruInsertCount > 0 )
		{
			if( ! ItemInsert(
					ID_SEPARATOR,
					nMruUpdateIndex + nMruInsertCount
					)
				)
			{
				ASSERT( FALSE );
				return false;
			}
			nMruInsertCount++;
		} // if( nMruInsertCount > 0 )
	} // insert recently opened file items

	_SyncItems();
	return true;
}

void CExtPopupMenuWnd::_GetClientRect(RECT * pRectClient)
{
	ASSERT_VALID( this );
	ASSERT( pRectClient != NULL );
	ASSERT( GetSafeHwnd() != NULL );
	::CopyRect( pRectClient, &m_rcClient );
	ASSERT( pRectClient->top <= pRectClient->bottom );
	ASSERT( pRectClient->left <= pRectClient->right );
}

BOOL CExtPopupMenuWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	ASSERT_VALID( this );
	if( _FindHelpMode() )
	{
		SetCursor( afxData.hcurHelp );
		return TRUE;
	}
CPoint ptCursor;
	if( ::GetCursorPos(&ptCursor) )
	{
		ScreenToClient( &ptCursor );
		int nIndex = _HitTest( ptCursor );
		if( _IsTearOff() && nIndex == IDX_TEAROFF )
		{
			HCURSOR hCursor = ::LoadCursor( NULL, IDC_SIZEALL );
			ASSERT( hCursor != NULL );
			::SetCursor( hCursor );
			return TRUE;
		} // if( _IsTearOff() && nIndex == IDX_TEAROFF )
		if( ItemGetCount() > 0 )
		{
			if( nIndex >= 0 )
			{
				MENUITEMDATA & mi = ItemGetInfo( nIndex );
				if(		(! mi.IsExtraMark() )
					&&	mi.IsInplaceEdit()
					&&	mi.IsAllowInplaceEditActivation()
					&&	(! _FindHelpMode() )
					)
				{
					CRect rcItem;
					_GetItemRect( nIndex, rcItem );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
					if( _FindCustomizeMode() )
					{
						ASSERT_VALID( mi.GetCmdNode() );
						if(		mi.IsSelected()
							&&	(mi.GetCmdNode()->GetFlags() & __ECTN_TBB_RESIZABLE) != 0
							)
						{ // if button can be resized
							int nDdWidth = PmBridge_GetPM()->GetDropDividerMerics().cx / 2;
							if( nDdWidth < 2 )
								nDdWidth = 2;
							CRect rcH( rcItem );
							rcH.right = rcH.left + nDdWidth;
							bool bSetCursor = false;
							if( rcH.PtInRect(ptCursor) )
								bSetCursor = true;
							else
							{
								rcH = rcItem;
								rcH.left = rcH.right - nDdWidth;
								if( rcH.PtInRect(ptCursor) )
									bSetCursor = true;
							} // else from if( rcH.PtInRect(ptCursor) )
							if( bSetCursor )
							{
								CExtLocalResourceHelper _LRH;
								CWinApp * pApp = ::AfxGetApp();
								ASSERT_VALID( pApp );
								HCURSOR hCursor = pApp->LoadCursor( MAKEINTRESOURCE( IDC_EXT_RESIZE_H1 ) );
								if( hCursor == NULL )
								{
									ASSERT( FALSE );
									hCursor = ::LoadCursor( NULL, IDC_SIZEWE );
								} // if( hCursor == NULL )
								ASSERT( hCursor != NULL );
								::SetCursor( hCursor );
								return TRUE;
							} // if( bSetCursor )
						} // if button can be resized
					} // if( _FindCustomizeMode() )
					else
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
					{
						CRect rcInplaceCtrl =
							mi.AdjustInplaceEditRect(
								rcItem,
								OnQueryLayoutRTL()
								);
						if( rcInplaceCtrl.PtInRect(ptCursor) )
						{
							SetCursor( ::LoadCursor( NULL, IDC_IBEAM ) );
							return TRUE;
						} // if( rcInplaceCtrl.PtInRect(ptCursor) )
						SetCursor( ::LoadCursor( NULL, IDC_ARROW ) );
					} // else from if( _FindCustomizeMode() )
				}
			} // if( nIndex >= 0 )
		} // if( ItemGetCount() > 0)
	} // if( ::GetCursorPos(&ptCursor) )
	return CExtPopupBaseWnd::OnSetCursor(pWnd, nHitTest, message);
}

int CExtPopupMenuWnd::_HitTest(
	const CPoint & point,
	bool * p_bInplaceControlArea, // = NULL
	bool * p_bInplaceDropDownArea // = NULL
	)
{
	ASSERT_VALID( this );

	if( p_bInplaceControlArea != NULL )
		*p_bInplaceControlArea = false;
	if( p_bInplaceDropDownArea != NULL )
		*p_bInplaceDropDownArea = false;

	if( GetSafeHwnd() == NULL )
		return IDX_NOTHING;

CRect rcClient;
	_GetClientRect( &rcClient );
	if( ! rcClient.PtInRect(point) )
		return IDX_NOTHING;

DWORD dwTrackFlags = TrackFlagsGet();
	if( (dwTrackFlags&TPMX_RIBBON_FILE_MENU) != 0 )
	{
		if( (dwTrackFlags&TPMX_RIBBON_EXIT_BUTTON) != 0 )
		{
			CRect rcItem;
			_GetItemRect(IDX_RIBBON_FILE_MENU_EXIT_BUTTON,rcItem);
			if( rcItem.PtInRect(point) )
				return IDX_RIBBON_FILE_MENU_EXIT_BUTTON;
		} // if( (dwTrackFlags&TPMX_RIBBON_EXIT_BUTTON) != 0 )
		if( (dwTrackFlags&TPMX_RIBBON_OPTIONS_BUTTON) != 0 )
		{
			CRect rcItem;
			_GetItemRect(IDX_RIBBON_FILE_MENU_OPTIONS_BUTTON,rcItem);
			if( rcItem.PtInRect(point) )
				return IDX_RIBBON_FILE_MENU_OPTIONS_BUTTON;
		} // if( (dwTrackFlags&TPMX_RIBBON_OPTIONS_BUTTON) != 0 )
	} // if( (dwTrackFlags&TPMX_RIBBON_FILE_MENU) != 0 )

	if( m_bScrollingAvailable )
	{
		CRect rcItem;
		if( m_nDyScrollOffset != 0 )
		{
			_GetItemRect(IDX_SCROLL_TOP,rcItem);
			if( rcItem.PtInRect(point) )
				return IDX_SCROLL_TOP;
		}
		int nMaxScrollPos = _GetMaxScrollPos();
		if( m_nDyScrollOffset != nMaxScrollPos )
		{
			_GetItemRect(IDX_SCROLL_BOTTOM,rcItem);
			if( rcItem.PtInRect(point) )
				return IDX_SCROLL_BOTTOM;
		}
	}
	if( m_bExpandAvailable )
	{
		CRect rcExpand;
		_GetItemRect(IDX_EXPAND,rcExpand);
		if( rcExpand.PtInRect(point) )
			return IDX_EXPAND;
	}
	if( _IsTearOff() )
	{
		CRect rcTearOff;
		_GetItemRect(IDX_TEAROFF,rcTearOff);
		if( rcTearOff.PtInRect(point) )
		{
			if(		_FindCustomizeMode()
				||	_FindHelpMode()
				)
				return IDX_NOTHING;
			return IDX_TEAROFF;
		}
	}

bool bRTL = OnQueryLayoutRTL();
INT iter = 0;
int nIndex = 0;
	for( ; iter < m_items_all.GetSize(); iter++,nIndex++ )
	{
		MENUITEMDATA & mi = ItemGetInfo( iter );
		if( !mi.IsDisplayed() )
			continue;

		CRect rcItem;
		_GetItemRect( nIndex, rcItem );
		if( rcItem.PtInRect(point) )
		{
			int nRetIndex = mi.GetIndex();
			ASSERT(
					nRetIndex >= 0
				&&	nRetIndex < m_items_all.GetSize()
				);
			if(		(	p_bInplaceControlArea != NULL
					||	p_bInplaceDropDownArea != NULL
					)
				&&	mi.IsInplaceEdit()
				)
			{
				CRect rcInplaceCtrl =
					mi.AdjustInplaceEditRect(
						rcItem,
						bRTL
						);
				if( p_bInplaceControlArea != NULL )
				{
					if( rcInplaceCtrl.PtInRect(point) )
						*p_bInplaceControlArea = true;
				}
				if(		p_bInplaceDropDownArea != NULL
					&&	mi.IsPopup()
					)
				{
					rcInplaceCtrl.left = rcInplaceCtrl.right;
					rcInplaceCtrl.right += PmBridge_GetPM()->GetDropDownButtonWidth( this );
					if( rcInplaceCtrl.PtInRect(point) )
						*p_bInplaceDropDownArea = true;
				}
			}
			return nRetIndex;
		}
	} // for( ; iter < m_items_all.GetSize(); iter++,nIndex++ )
	return IDX_NOTHING;
};


BEGIN_MESSAGE_MAP(CExtPopupMenuWnd, CExtPopupBaseWnd)
	//{{AFX_MSG_MAP(CExtPopupMenuWnd)
	ON_WM_CANCELMODE()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATEAPP()
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
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_SYSDEADCHAR()
	ON_WM_CHAR()
	ON_WM_GETMINMAXINFO()
	__EXT_MFC_ON_WM_NCHITTEST()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtPopupMenuWnd message handlers

UINT CExtPopupMenuWnd::OnNcHitTest(CPoint point)
{
DWORD dwTrackFlags = TrackFlagsGet();
	if( (dwTrackFlags&TPMX_RIBBON_RESIZING) != 0 )
	{
		CRect rcMB = PmBridge_GetPM()->GetMenuBorderMetrics( (CWnd*)this );
		ASSERT( rcMB.left >= 0 );
		ASSERT( rcMB.top >= 0 );
		ASSERT( rcMB.right >= 0 );
		ASSERT( rcMB.bottom >= 0 );
		CRect rcClient;
		_GetClientRect( &rcClient );
		CRect rcResizingArea = rcClient;
		rcResizingArea.left += rcMB.left;
		rcResizingArea.right -= rcMB.right;
		rcResizingArea.bottom -= rcMB.bottom;
		INT nMarginHeight = 0, nResizingCornerPartWidth = 0;
		PmBridge_GetPM()->MenuBottomResizingMarginGetInfo(
			nMarginHeight,
			nResizingCornerPartWidth,
			this
			);
		rcResizingArea.top = rcResizingArea.bottom - nMarginHeight;
		ClientToScreen( &rcResizingArea );
		if( (dwTrackFlags&TPMX_RIBBON_RESIZING_VERTICAL_ONLY) != 0 )
		{
			if( rcResizingArea.PtInRect( point ) )
				return HTBOTTOM;
		}
		else
		{
			rcResizingArea.left = rcResizingArea.right - nResizingCornerPartWidth;
			if( rcResizingArea.PtInRect( point ) )
				return HTBOTTOMRIGHT;
		}
	}
	return (UINT)CExtPopupBaseWnd::OnNcHitTest( point );
}

void CExtPopupMenuWnd::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	CExtPopupBaseWnd::OnGetMinMaxInfo( lpMMI );
}

BOOL CExtPopupMenuWnd::LoadMenu( 
	HWND hWndCmdRecv,
	UINT nIDResource,
	bool bPopupMenu, // = true
	bool bNoRefToCmdMngr // = false
	)
{
	ASSERT_VALID( this );

CMenu menu;
	if( ! g_ResourceManager->LoadMenu( menu, nIDResource ) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	return
		UpdateFromMenu(
			hWndCmdRecv,
			&menu,
			bPopupMenu,
			true,
			bNoRefToCmdMngr
			);
}

bool CExtPopupMenuWnd::g_bMdiWindowsMenuUsesCheckInsteadOfRadio = false;

BOOL CExtPopupMenuWnd::UpdateMdiWindowsMenu(
	CWnd * pWndStartSearchMdiFrameWnd // = NULL
	)
{
	ASSERT_VALID( this );

	ASSERT( m_hWndCmdReceiver != NULL );
	ASSERT( ::IsWindow(m_hWndCmdReceiver) );
CMDIFrameWnd * pFrame = NULL;
CWnd * pWnd =
		(pWndStartSearchMdiFrameWnd != NULL)
			? pWndStartSearchMdiFrameWnd
			: FromHandle(m_hWndCmdReceiver) // ::AfxGetMainWnd()
			;
	while( true )
	{
		ASSERT( pWnd != NULL );
		ASSERT_VALID( pWnd );
		pFrame =
			DYNAMIC_DOWNCAST(
				CMDIFrameWnd,
				pWnd
				);
		if( pFrame != NULL )
			break;
		pWnd = pWnd->GetParentFrame();
		if( pWnd == NULL )
			return FALSE;
	} // while( true )
	if( pFrame == NULL )
		return FALSE;
int nItemIndex = ItemGetCount();
	if( nItemIndex > 0 )
	{
		MENUITEMDATA & mi = ItemGetInfo(nItemIndex-1);
		if( ! mi.IsSeparator() )
		{
			ItemInsert(ID_SEPARATOR);
			nItemIndex++;
			ASSERT( nItemIndex == ItemGetCount() );
		}
	}

HWND m_hWndMDIClient = pFrame->m_hWndMDIClient;
	ASSERT( m_hWndMDIClient != NULL );
	ASSERT( ::IsWindow(m_hWndMDIClient) );

HWND hWndActiveChildFrame = (HWND)
		::SendMessage( m_hWndMDIClient, WM_MDIGETACTIVE, 0, NULL );
	if( hWndActiveChildFrame != NULL )
	{
		ASSERT( ::IsWindow( hWndActiveChildFrame ) );
		CWnd * pWndTempAnalyze =
			CWnd::FromHandle( hWndActiveChildFrame );
		if( pWndTempAnalyze != NULL )
		{
			ASSERT_VALID( pWndTempAnalyze );
			CMDIChildWnd * pWndTempMDIChild =
				DYNAMIC_DOWNCAST( CMDIChildWnd, pWndTempAnalyze );
			if(		pWndTempMDIChild != NULL
				&&	pWndTempMDIChild->m_bPseudoInactive
				&&	(pWndTempMDIChild->GetStyle()&WS_VISIBLE) == 0
				)
				hWndActiveChildFrame = NULL;
		} // if( pWndTempAnalyze != NULL )
	} // if( hWndActiveChildFrame != NULL )

INT nCmdID = __ID_MDIWNDLIST_FIRST;
INT nWinCount = 0;
	for( INT nWin = 1; true; nWin++, nCmdID++ )
	{
		HWND hWndChildFrame =
			::GetDlgItem(m_hWndMDIClient, nCmdID);
		if( hWndChildFrame == NULL )
			break;
		if( !::IsWindowVisible( hWndChildFrame ) )
			continue;

		nWinCount ++;

		if( nWinCount <= __ID_MDIWNDLIST_COUNT )
		{ // if head of MDI windows list
			ASSERT( ::IsWindow(hWndChildFrame) );
			CString sWinName( _T("") );
			CWnd::FromHandle( hWndChildFrame )-> GetWindowText( sWinName );
			CExtCmdItem * pCmdItem =
				g_CmdManager->CmdGetPtr(
					g_CmdManager->ProfileNameFromWnd( pWndStartSearchMdiFrameWnd->GetSafeHwnd() ),
					nCmdID
					);
			if( pCmdItem == NULL )
				pCmdItem =
				g_CmdManager->CmdAllocPtr(
					g_CmdManager->ProfileNameFromWnd( pWndStartSearchMdiFrameWnd->GetSafeHwnd() ),
					nCmdID
					);
			ASSERT( pCmdItem != NULL );
			if( pCmdItem == NULL )
				return FALSE;
			pCmdItem->m_sMenuText.Format(
				_T("&%d %s"),
				nWinCount,
				sWinName
				);
			pCmdItem->m_sToolbarText = pCmdItem->m_sMenuText;
			CExtSafeString sDocumentNameFmt;
			if( ! g_ResourceManager->LoadString( sDocumentNameFmt, IDS_DOCUMENT_NAME_FMT ) )
				sDocumentNameFmt = _T("Document \"%s\"");

			pCmdItem->m_sTipTool.Format(
				(LPCTSTR)sDocumentNameFmt,
				sWinName
				);
			pCmdItem->m_sTipStatus = pCmdItem->m_sTipTool;
			bool bRadio =
				(hWndChildFrame == hWndActiveChildFrame)
					? true : false;
			pCmdItem->StateSetRadio( bRadio );
			if( ! ItemInsert(nCmdID) )
			{
				ASSERT( FALSE );
				return FALSE;
			}
			MENUITEMDATA & mi = ItemGetInfo( ItemGetCount()-1 );
			if( g_bMdiWindowsMenuUsesCheckInsteadOfRadio )
				mi.Check( bRadio );
			else
				mi.Radio( bRadio );
		} // if head of MDI windows list
		else
		{ // MDI windows list enough long, trim it
			if( ! ItemInsert(TYPE_SEPARATOR) )
			{
				ASSERT( FALSE );
				return FALSE;
			}
			HWND hWndSearch = pWndStartSearchMdiFrameWnd->GetSafeHwnd();
			ASSERT( hWndSearch != NULL );
			ASSERT( ::IsWindow(hWndSearch) );
			CExtCmdItem * pCmdItem =
				g_CmdManager->CmdGetPtr(
					g_CmdManager->ProfileNameFromWnd( hWndSearch ),
					__ID_MDIWND_DLGWINDOWS
					);
			if( pCmdItem == NULL )
				pCmdItem =
				g_CmdManager->CmdAllocPtr(
					g_CmdManager->ProfileNameFromWnd( hWndSearch ),
					__ID_MDIWND_DLGWINDOWS
					);
			ASSERT( pCmdItem != NULL );
			if( pCmdItem == NULL )
				return FALSE;
			
			CExtSafeString sMoreWindows,sManageWindows;
			if( ! g_ResourceManager->LoadString( sMoreWindows, IDS_MDI_MORE_WINDOWS ) )
				sMoreWindows = _T("More Windows");

			if( ! g_ResourceManager->LoadString( sManageWindows, IDS_MDI_MANAGE_OPENED_WINDOWS ) )
				sManageWindows = _T("Manage opened document windows");

			pCmdItem->m_sMenuText = sMoreWindows;
			pCmdItem->m_sToolbarText = pCmdItem->m_sMenuText;
			pCmdItem->m_sTipTool = sManageWindows;
			pCmdItem->m_sTipStatus = pCmdItem->m_sTipTool;
			if( ! ItemInsert(nCmdID) )
			{
				ASSERT( FALSE );
				return FALSE;
			}
			break;
		} // MDI windows list enough long, trim it
	} // for( INT nWin = 1; true; nWin++, nCmdID++ )
	
	_SyncItems();
	return TRUE;
}

HMENU CExtPopupMenuWnd::ExportToMenu(
	BOOL bDeep // = TRUE
	) const
{
	ASSERT_VALID( this );

CMenu menu;
	if( !menu.CreatePopupMenu() )
	{
		ASSERT( FALSE );
		return NULL;
	}

OSVERSIONINFO ov;
    ov.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	VERIFY( ::GetVersionEx( &ov ) );
bool bStupidNT4XX =
		(
		( ov.dwPlatformId == VER_PLATFORM_WIN32_NT )
		&&
		( ov.dwMajorVersion < 5 )
		);

int nCount = ItemGetCount();
	for( int nItem = 0; nItem < nCount; nItem++ )
	{
		const MENUITEMDATA & mi =
			ItemGetInfo( nItem );

		if( mi.IsPopup() )
		{
			CExtSafeString sMiText( mi.GetText() );
			CExtSafeString sMiAccelText( _T("") );
			if( ( TrackFlagsGet() & TPMX_HIDE_KEYBOARD_ACCELERATORS ) == 0 )
				sMiAccelText = mi.GetAccelText();
			if( !sMiAccelText.IsEmpty() )
			{
				sMiText += _T("\t");
				sMiText += sMiAccelText;
			}
			const CExtPopupMenuWnd * pPopup = mi.GetPopup();
			ASSERT( pPopup != NULL );
			HMENU hPopupData;
			if( bDeep )
				hPopupData = pPopup->ExportToMenu();
			else
				hPopupData = ::CreatePopupMenu();
			if( ( ! (::IsMenu(hPopupData)) )
				||
				(!	menu.AppendMenu(
						MF_STRING|MF_POPUP,
						__EXT_MFC_UINT_PTR(hPopupData),
						sMiText
						)
					)
				)
			{
				ASSERT( FALSE );
				//continue;
			}
			if( bStupidNT4XX )
			{
				VERIFY( ::DestroyMenu( hPopupData ) );
			}
			continue;
		} // if( mi.IsPopup() )

		if( mi.IsSeparator() )
		{
			VERIFY( menu.AppendMenu( MF_SEPARATOR ) );
			continue;
		} // if( mi.IsSeparator() )

		CExtSafeString sMiText( mi.GetText() );
		CExtSafeString sMiAccelText( _T("") );
		if( ( TrackFlagsGet() & TPMX_HIDE_KEYBOARD_ACCELERATORS ) == 0 )
			sMiAccelText = mi.GetAccelText();
		if( ! sMiAccelText.IsEmpty() )
		{
			sMiText += _T("\t");
			sMiText += sMiAccelText;
		}
		VERIFY(
			menu.AppendMenu(
				MF_STRING,
				mi.GetCmdID(),
				sMiText
				)
			);

	} // for( int nItem = 0; nItem < nCount; nItem++ )

	return menu.Detach();
}

BOOL CExtPopupMenuWnd::UpdateFromMenu( 
	HWND hWndCmdRecv,
	CMenu * pBuildMenu,
	bool bPopupMenu, // = true
	bool bTopLevel, // = true
	bool bNoRefToCmdMngr // = false
	)
{
	ASSERT_VALID( this );

	if( hWndCmdRecv != NULL )
		m_hWndCmdReceiver = hWndCmdRecv;
	ASSERT( m_hWndCmdReceiver != NULL );
	ASSERT( ::IsWindow(m_hWndCmdReceiver) );

	if(		pBuildMenu == NULL
		||	( !( ::IsMenu( pBuildMenu->GetSafeHmenu() ) ) )
		)
	{
		ASSERT( FALSE );
		return false;
	}
	if( bPopupMenu )
	{
		pBuildMenu = pBuildMenu->GetSubMenu(0);
		if(		pBuildMenu == NULL
			||	( ! ( ::IsMenu( pBuildMenu->GetSafeHmenu() ) ) )
			)
		{
//			ASSERT( FALSE );
			return false;
		}
	}

	if( ! bNoRefToCmdMngr )
	{
		if(	! g_CmdManager->UpdateFromMenu(
				g_CmdManager->ProfileNameFromWnd( m_hWndCmdReceiver ),
				pBuildMenu->GetSafeHmenu()
				)
			)
		{
			ASSERT( FALSE );
			return false;
		}
	} // if( ! bNoRefToCmdMngr )

	if( ! _BuildItems(
			pBuildMenu,
			bTopLevel,
			bNoRefToCmdMngr
			)
		)
		return FALSE;
	return TRUE;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
bool CExtPopupMenuWnd::UpdateFromCmdTree(
	HWND hWndCmdRecv,
	CExtCustomizeCmdTreeNode * pNode,
	bool bTopLevel // = true
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNode );
	m_pNode = pNode;

	if( hWndCmdRecv != NULL )
		m_hWndCmdReceiver = hWndCmdRecv;
	ASSERT( m_hWndCmdReceiver != NULL );
	ASSERT( ::IsWindow(m_hWndCmdReceiver) );

__EXT_MFC_SAFE_LPCTSTR strProfileName =
		g_CmdManager->ProfileNameFromWnd( m_hWndCmdReceiver );
bool bCustomizeMode = false;
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWndCmdReceiver );
	if( pSite != NULL )
		bCustomizeMode = pSite->IsCustomizeMode();

INT iter = 0;
	for( ; iter < m_items_all.GetSize(); iter++)
	{
		MENUITEMDATA & mi = ItemGetInfo( iter );
		if( mi.IsPopup() )
		{
			CExtPopupMenuWnd * pPopup = mi.GetPopup();
			pPopup->m_hWndCmdReceiver = m_hWndCmdReceiver;
			INT nSaved = pPopup->m_nFadeOutAnimationStepCount;
			pPopup->m_nFadeOutAnimationStepCount = -1;
			pPopup->_OnCancelMode();
			pPopup->m_nFadeOutAnimationStepCount = nSaved;
			VERIFY( pPopup->_BuildItems( NULL, false ) );
			mi.DestroyPopup();
		}
	} // for( ; iter < m_items_all.GetSize(); iter++)
	m_items_all.RemoveAll();

	if( pNode == NULL )
		return true;
	ASSERT_VALID( pNode );

	m_bTopLevel = bTopLevel;
	if( m_bTopLevel )
		m_pWndParentMenu = NULL;

	// build items loop
int nMruUpdateIndex = -1, nInsertedIndex = 0;
int	nMenuItemCount = pNode->GetNodeCount();
	m_items_all.RemoveAll();
bool bSeparatorMode = false;
	for( int nItemIndex = 0; nItemIndex < nMenuItemCount; nItemIndex++ )
	{
		CExtCustomizeCmdTreeNode * pChildNode =
			pNode->ElementAt( nItemIndex );
		ASSERT_VALID( pChildNode );
		if( ( pChildNode->GetFlags() & __ECTN_GROUP_START ) )
		{
			if( ! bSeparatorMode )
			{
				bSeparatorMode = true;
				VERIFY( ItemInsert( CExtPopupMenuWnd::TYPE_SEPARATOR, nInsertedIndex++ ) );
			}
		} // if( pChildNode->GetFlags() & __ECTN_GROUP_START )
		if( pChildNode->GetFlags() & __ECTN_TBB_HIDDEN )
			continue;
		UINT nMenuItemID = pChildNode->GetCmdID( false );
		if(		ID_FILE_MRU_FIRST <= nMenuItemID
			&&	nMenuItemID <= ID_FILE_MRU_LAST
			&&	(! bCustomizeMode )
			)
		{
			if( nMruUpdateIndex < 0 )
				nMruUpdateIndex = nInsertedIndex;
			continue;
		}
		MENUITEMDATA mi( this );
		mi.SetCmdReceiver( m_hWndCmdReceiver );
		VERIFY( mi.UpdateFromCmdTree( m_hWndCmdReceiver, pChildNode, nInsertedIndex ) );
		if( ! mi.IsSeparator() )
		{
			bSeparatorMode = false;
			CExtCmdItem * pCmdItem = NULL;
			if( mi.IsPopup() )
			{
				CExtPopupMenuWnd * pDesc = mi.GetPopup();
				pDesc->m_hWndCmdReceiver = m_hWndCmdReceiver;
				pDesc->UpdateFromCmdTree( hWndCmdRecv, pChildNode, false );
				pDesc->m_pWndParentMenu = this;
			} // if( mi.IsPopup() )
			else
			{
				pCmdItem = g_CmdManager->CmdGetPtr( strProfileName, mi.GetCmdID() );
/*
				// HASH Added this because during our Alpha release
				// some commands were deleted but the menus were
				// still loaded from resource
				if( pCmdItem == NULL )
					continue;
*/
				if( pCmdItem != NULL )
					mi.SetAccelText( pCmdItem->m_sAccelText );
			} // else from if( mi.IsPopup() )

			mi.SetText( pChildNode->GetTextInMenu( pCmdItem ) );
			mi.AccelCharInit();
			mi.MeasureItem( NULL );
		} // if( ! mi.IsSeparator() )

		_InsertItem( -1, mi );
		nInsertedIndex++;
	} // for( int nItemIndex = 0; nItemIndex < nMenuItemCount; nItemIndex++ )

int nMruInsertCount = 0;
	if( nMruUpdateIndex >= 0 )
	{ // insert recently opened file items
		//ASSERT( nMruUpdateIndex <= nMenuItemCount );
		CRecentFileList * pRecentFileList =
			InternalFriendlyWinApp::_GetFriendlyApp()->
				_GetRecentFileList();
		if( pRecentFileList != NULL )
		{ // can be null !?
			int nRecentCount =
				pRecentFileList->GetSize();
			TCHAR sCurrDir[_MAX_PATH+1];
			::memset(sCurrDir,0,sizeof(sCurrDir));
			::GetCurrentDirectory(_MAX_PATH,sCurrDir);
			int nLenCurDir = (int)_tcslen(sCurrDir);
			for( int nItemIndex=0; nItemIndex<nRecentCount; nItemIndex++ )
			{
				CExtSafeString sDisplayName( _T("") );
				CExtSafeString sDisplayNameFullPath( _T("") );
				CExtSafeString sDisplayNameMenu( _T("") );
				CExtSafeString sDisplayNameTipTool( _T("") );

            sDisplayNameFullPath = ( ! pRecentFileList->m_arrNames[ nItemIndex ].IsEmpty() ) ? LPCTSTR( pRecentFileList->m_arrNames[ nItemIndex ] ) : _T("");
            pRecentFileList->GetDisplayName( *((CString *)&sDisplayName), nItemIndex, sCurrDir, nLenCurDir, TRUE );

            if( sDisplayName.IsEmpty() || sDisplayNameFullPath.IsEmpty() )
					continue;

            if( g_bMRU_UseFullPathsInMenu )
               sDisplayNameMenu = sDisplayNameFullPath;
            else 
               sDisplayNameMenu = sDisplayName;

            if( g_bMRU_UseFullPathsInTipTool )
               sDisplayNameTipTool = sDisplayNameFullPath;
            else 
               sDisplayNameTipTool = sDisplayName;

            UINT nCmdID = ID_FILE_MRU_FIRST + nItemIndex;
				ASSERT( nCmdID <= ID_FILE_MRU_LAST );
				CExtCmdItem * pCmdItem =
					g_CmdManager->CmdGetPtr(
						g_CmdManager->ProfileNameFromWnd( m_hWndCmdReceiver ),
						nCmdID
						);
				if( pCmdItem == NULL )
					pCmdItem =
						g_CmdManager->CmdAllocPtr(
							g_CmdManager->ProfileNameFromWnd( m_hWndCmdReceiver ),
							nCmdID
							);
				ASSERT( pCmdItem != NULL );
				if( pCmdItem == NULL )
					return false;
				int nDisplayIndex = nItemIndex+1;
				if( nDisplayIndex < 10 )
					pCmdItem->m_sMenuText.Format(
						_T("&%d %s"),
						nDisplayIndex,
						sDisplayNameMenu
						);
				else
					pCmdItem->m_sMenuText.Format(
						_T("%d&%d %s"),
						nDisplayIndex/10,
						nDisplayIndex%10,
						sDisplayNameMenu
						);
				
				CExtSafeString sRecentFileFmt1,sRecentFileFmt2;
				if( ! g_ResourceManager->LoadString( sRecentFileFmt1, IDS_RECENT_FILE_FMT_1 ) )
					sRecentFileFmt1 = _T("Recent file %d");

				if( ! g_ResourceManager->LoadString( sRecentFileFmt2, IDS_RECENT_FILE_FMT_2 ) )
					sRecentFileFmt2 = _T("Recent file %d (\"%s\")");

				pCmdItem->m_sToolbarText.Format(
					(LPCTSTR)sRecentFileFmt1,
					nItemIndex + 1
					);
				pCmdItem->m_sTipTool.Format(
					(LPCTSTR)sRecentFileFmt2,
					nItemIndex + 1,
					sDisplayNameTipTool
					);
				pCmdItem->m_sTipStatus = pCmdItem->m_sTipTool;

				BOOL bInsRetVal =
					ItemInsert(
						nCmdID,
						nMruUpdateIndex + nItemIndex
						);
				
				if( !bInsRetVal )
				{
					ASSERT( FALSE );
					return false;
				}
				nMruInsertCount++;
			} // for( nItemIndex=0; nItemIndex<nRecentCount; nItemIndex++ )
		} // can be null !?
/*
		if( nMruInsertCount > 0 )
		{
			if(		nMruUpdateIndex > 0
				&&	(! ItemGetInfo( nMruUpdateIndex - 1 ).IsSeparator() )
				)
			{
				if( ! ItemInsert(
						ID_SEPARATOR,
						nMruUpdateIndex
						)
					)
				{
					ASSERT( FALSE );
					return false;
				}
				nMruInsertCount++;
				nMruUpdateIndex++;
			}
			if(		( nMruUpdateIndex + nMruInsertCount ) < m_items_all.GetSize()
				&&	(! ItemGetInfo( nMruUpdateIndex + nMruInsertCount ).IsSeparator() )
				)
			{
				if( ! ItemInsert(
						ID_SEPARATOR,
						nMruUpdateIndex + nMruInsertCount
						)
					)
				{
					ASSERT( FALSE );
					return false;
				}
				nMruInsertCount++;
			}
		} // if( nMruInsertCount > 0 )
*/
		if(		nMruInsertCount == 0
			&&	nMruUpdateIndex > 0
			&&	nMruUpdateIndex < (ItemGetCount()-1)
			&&	ItemGetInfo( nMruUpdateIndex ).IsSeparator()
			&&	ItemGetInfo( nMruUpdateIndex-1 ).IsSeparator()
			)
			m_items_all.RemoveAt( nMruUpdateIndex );
	} // insert recently opened file items

	_SyncItems();
	return true;
}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)


CExtPopupMenuSite & CExtPopupMenuWnd::GetSite() const
{
	return CExtPopupMenuSite::g_DefPopupMenuSite;
}

CExtPopupMenuWnd * CExtPopupMenuWnd::GetTrackingMenu()
{
CExtPopupMenuWnd * pPopup = CExtPopupMenuSite::g_DefPopupMenuSite.GetInstance();
	if( pPopup->GetSafeHwnd() == NULL )
		return NULL;
	ASSERT_VALID( pPopup );
	return pPopup;
}

void CExtPopupMenuWnd::CancelMenuTracking()
{
	if(		(! CExtPopupMenuSite::g_DefPopupMenuSite.IsEmpty() )
		&&	(! CExtPopupMenuSite::g_DefPopupMenuSite.IsShutdownMode() )
		&&	CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON == NULL
		)
	{
		CExtPopupMenuSite::g_DefPopupMenuSite.DoneInstance();
		ASSERT( CExtPopupMenuSite::g_DefPopupMenuSite.IsEmpty() );
		ASSERT( !CExtPopupMenuSite::g_DefPopupMenuSite.IsShutdownMode() );
	}
}

BOOL CExtPopupMenuWnd::IsMenuTracking()
{
	if( CExtPopupMenuSite::g_DefPopupMenuSite.IsEmpty()
		//|| CExtPopupMenuSite::g_DefPopupMenuSite.GetCapture() == NULL
		)
		return FALSE;
	return TRUE;
}

BOOL CExtPopupMenuWnd::TrackPopupMenu(
	DWORD dwTrackFlags,
	int x,
	int y,
	LPCRECT lpRect, // = NULL
	LPVOID pCbPaintCombinedCookie, // = NULL
	pCbPaintCombinedContent pCbPCC, // = NULL
	UINT * lpnResultCmdID, // = NULL
	bool bCookieIsObject // = false
	)
{
	ASSERT_VALID( this );
CExtPopupMenuSite & _site = GetSite();
CExtPopupMenuWnd * pOldPopup = _site.GetInstance();
	if( pOldPopup != NULL )
	{
		ASSERT_VALID( pOldPopup );
		DWORD dwTrackFlagsOld = pOldPopup->TrackFlagsGet();
		dwTrackFlagsOld |= TPMX_NO_FADE_OUT_ANIMATION;
		dwTrackFlags |= TPMX_FORCE_NO_ANIMATION;
		pOldPopup->TrackFlagsSet( dwTrackFlagsOld );
	}
POSITION pos = g_ListDetached.GetHeadPosition();
	for( ; pos != NULL; )
	{
		CExtPopupMenuWnd * pPopup = g_ListDetached.GetNext( pos );
		if( pPopup == this )
			continue;
		if( pPopup->GetParentMenuWnd() != NULL )
			continue;
		ASSERT_VALID( pPopup );
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
		if( pPopup->IsKindOf( RUNTIME_CLASS(CExtRibbonPopupMenuWnd) ) )
			continue;
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
		if(		pPopup->GetSafeHwnd() != NULL
			&&	(pPopup->TrackFlagsGet()&(TPMX_NO_SITE|TPMX_RIBBON_MODE)) != 0
			)
		{
			pPopup->TrackFlagsSet( pPopup->TrackFlagsGet() | TPMX_NO_FADE_OUT_ANIMATION );
			pPopup->DestroyWindow();
			if( _site.GetInstance() == pPopup )
				_site.DoneInstance();
			CExtToolControlBar::_CloseCustomMenusAll();
			CExtToolControlBar::_CloseTrackingMenus();
			break;
		}
	}

	if(		(dwTrackFlags&TPMX_RIBBON_RESIZING) != 0
		&&	(! m_ctrlShadow.IsAvailable() )
		)
		dwTrackFlags |= TPMX_NO_SHADOWS;

bool bLetSurfacesUpdate = false;
CExtPopupMenuTipWnd & _tipWndSite = GetSite().GetTip();
	if(		_tipWndSite.GetSafeHwnd() != NULL
		&&	(_tipWndSite.GetStyle()&WS_VISIBLE) != 0
		)
	{
		_tipWndSite.Hide();
		_tipWndSite.DestroyWindow();
		bLetSurfacesUpdate = true;
	}
	CWnd::CancelToolTips();
CExtPopupMenuTipWnd & _tipWnd = GetTip();
	if(		_tipWnd.GetSafeHwnd() != NULL
		&&	(_tipWnd.GetStyle()&WS_VISIBLE) != 0
		)
	{
		_tipWnd.Hide();
		_tipWndSite.DestroyWindow();
		bLetSurfacesUpdate = true;
	}
	if( bLetSurfacesUpdate )
		CExtPaintManager::stat_PassPaintMessages();
	if( lpnResultCmdID != NULL )
		(*lpnResultCmdID) = 0;

	ASSERT( m_hWndCmdReceiver != NULL );
	ASSERT( ::IsWindow(m_hWndCmdReceiver) );
CWnd * pWndCmdReceiver =
		CWnd::FromHandlePermanent( m_hWndCmdReceiver );
	if(		pWndCmdReceiver != NULL
		&&	CExtControlBar::FindHelpMode( pWndCmdReceiver )
		)
		dwTrackFlags |= TPMX_HELP_CTX_MODE;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_pCustomizeSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWndCmdReceiver );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	
	ASSERT(
		(dwTrackFlags&(TPMX_CUSTOMIZE_MODE|TPMX_HELP_CTX_MODE))
		!= (TPMX_CUSTOMIZE_MODE|TPMX_HELP_CTX_MODE)
		);

	if( (dwTrackFlags&TPMX_CUSTOMIZE_MODE) != 0 )
	{
		dwTrackFlags &= ~
			(	TPMX_COMBINE_MASK
				|TPMX_SELECT_ANY
				|TPMX_DO_MESSAGE_LOOP
				|TPMX_OWNERDRAW_FIXED
			);
		dwTrackFlags |=
				TPMX_NO_SHADOWS
			|	TPMX_NO_DYNAMIC_SHADOWS
			|	TPMX_NO_HIDE_RARELY
			|	TPMX_NO_WM_COMMAND
			|	TPMX_NO_CMD_UI
			;
	} // if( (dwTrackFlags&TPMX_CUSTOMIZE_MODE) != 0 )

	if( (dwTrackFlags&TPMX_HELP_CTX_MODE) != 0 )
		dwTrackFlags |= TPMX_NO_HIDE_RARELY;

HWND hWndCurrentFocus = ::GetFocus();
	if( hWndCurrentFocus != NULL )
	{
		CWnd * pWndTest = CWnd::FromHandlePermanent( hWndCurrentFocus );
		if( pWndTest == NULL )
		{
			HWND hWnd = ::GetParent( hWndCurrentFocus );
			if( hWnd != NULL )
				pWndTest = CWnd::FromHandlePermanent( hWnd );
		} // if( pWndTest == NULL )
		if( pWndTest != NULL )
		{
			CComboBox * pComboTest =
				DYNAMIC_DOWNCAST( CComboBox, pWndTest );
			if( pComboTest == NULL )
			{
				pWndTest = pWndTest->GetParent();
				if( pWndTest != NULL )
				{
					pComboTest =
						DYNAMIC_DOWNCAST( CComboBox, pWndTest );
				}
			} // if( pComboTest == NULL )
			if(		pComboTest != NULL
				&&	(pComboTest->GetStyle() & (CBS_DROPDOWN|CBS_DROPDOWNLIST)) != 0
				&&	pComboTest->GetDroppedState()
				)
				pComboTest->ShowDropDown( FALSE );
		} // if( pWndTest != NULL )
	} // if( hWndCurrentFocus != NULL )

//CExtPopupMenuSite & _site = GetSite();
bool bSiteWasEmpty = _site.IsEmpty();
	if( ! bSiteWasEmpty )
	{
		if( _site.IsShutdownMode() )
			return FALSE;
		if( (TrackFlagsGet()&TPMX_NO_SITE) == 0 )
			_site.DoneInstance();
	}

//bool bCaptureChanged = false;
HWND hWndCapture = ::GetCapture();
CWnd * pWndCapture = FromHandlePermanent( hWndCapture );
	if( pWndCapture != NULL )
	{
		ReleaseCapture();
		if( ::IsWindow(hWndCapture) )
		{
			ASSERT_VALID( pWndCapture );
			pWndCapture->SendMessage(WM_CANCELMODE);
		}
//		bCaptureChanged = true;
	}

	if( ! _site.IsEmpty() )
		_site._Done();

	ASSERT( _site.IsEmpty() );
	ASSERT( !_site.IsShutdownMode() );

// 	ASSERT( m_hWndCmdReceiver != NULL );
// 	ASSERT( ::IsWindow(m_hWndCmdReceiver) );
	if(		m_hWndCmdReceiver == NULL 
		||	( !::IsWindow( m_hWndCmdReceiver ) )
		)
		return FALSE;

//	if( !bSiteWasEmpty
//		|| CExtToolControlBar::g_bMenuTracking
//		|| bCaptureChanged
//		)
//	{
//		PassMsgLoop( CExtPopupBaseWnd::g_bEnableOnIdleCalls );
//		if( ! ::IsWindow(m_hWndCmdReceiver) )
//			return FALSE;
//	}
//	if( !_site.IsEmpty() )
//		return FALSE;

	ASSERT( m_bTopLevel );
	ASSERT( _site.IsEmpty() );

	if( (dwTrackFlags&(TPMX_CUSTOMIZE_MODE|TPMX_NO_SITE)) == 0 )
	{
		_site.SetInstance( this );
		if( lpnResultCmdID != NULL )
			_site.SetTargetCmdIdPtr( lpnResultCmdID );
	}

	m_dwTrackFlags = dwTrackFlags;

	if( (dwTrackFlags&(TPMX_CUSTOMIZE_MODE|TPMX_HELP_CTX_MODE)) == 0 )
		_UpdateCmdUI();

	if(	! _TrackPopupMenu(
			dwTrackFlags,
			x,
			y,
			lpRect,
			pCbPaintCombinedCookie,
			pCbPCC,
			bCookieIsObject
			)
		)
	{
		//ASSERT( FALSE );
		if( (dwTrackFlags&(TPMX_CUSTOMIZE_MODE|TPMX_NO_SITE)) == 0 )
		{
			_site.DoneInstance();
			ASSERT( _site.IsEmpty() );
			ASSERT( !_site.IsShutdownMode() );
		} // if( (dwTrackFlags&TPMX_CUSTOMIZE_MODE) == 0 )
		return FALSE;
	}
	if( (dwTrackFlags&TPMX_DO_MESSAGE_LOOP) == 0 )
		return TRUE;
	// do popup menu message loop
HWND hWndThis = GetSafeHwnd();
	while( IsMenuTracking() )
	{
		WaitMessage();
		CExtPopupMenuWnd::PassMsgLoop( CExtPopupBaseWnd::g_bEnableOnIdleCalls );
		if( ! ::IsWindow( hWndThis ) )
			break;
	} // while( IsMenuTracking() )
	
	if( ! CExtToolControlBar::g_bMenuTracking )
	{
		if( (dwTrackFlags&TPMX_NO_SITE) == 0 )
		{
			if( _site.GetInstance() == this )
				_site.DoneInstance();
//			ASSERT( _site.IsEmpty() );
//			ASSERT( !_site.IsShutdownMode() );
		}
	} // if( ! CExtToolControlBar::g_bMenuTracking )

	return TRUE;
}

void CExtPopupMenuWnd::PassMsgLoop(
	bool bEnableOnIdleCalls
	)
{
	__PROF_UIS_MANAGE_STATE;

MSG msg;
	// Process all the messages in the message queue
	while( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
	{
		if( !AfxGetThread()->PumpMessage() )
		{
			PostQuitMessage(0);
			return;
		} // if( !AfxGetThread()->PumpMessage() )
	} // while( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
	if( bEnableOnIdleCalls )
	{
		for(	LONG nIdleCounter = 0L;
				::AfxGetThread()->OnIdle(nIdleCounter);
				nIdleCounter ++
				);
	}
}

CSize CExtPopupMenuWnd::_CalcTrackSize()
{
	ASSERT_VALID( this );

int nMenuShadowSize = OnQueryMenuShadowSize();
CRect rcMB = OnQueryMenuBorderMetrics();
int nXtraSzX = rcMB.left + rcMB.right + nMenuShadowSize;
int nXtraSzY = rcMB.top + rcMB.bottom + nMenuShadowSize;
CSize _size( m_sizeFullItems );
	if( m_bExpandAvailable )
		_size.cy += _GetSpecBtnHeight(); // expand button
	_size += CSize( nXtraSzX + m_nLeftAreaWidth, nXtraSzY );
	if( _size.cx < (nMenuShadowSize*2 + 10) )
		_size.cx = nMenuShadowSize*2 + 10;
	if( _size.cy < (nMenuShadowSize*2 + 10) )
		_size.cy = nMenuShadowSize*2 + 10;

	_size.cy += 1; // +/- 2.28

bool bFoundVisibleItem = false;
	for( int nIndex = 0; nIndex != m_items_all.GetSize(); nIndex++ )
	{
		MENUITEMDATA & mi = ItemGetInfo( nIndex );
		if( mi.IsDisplayed() )
		{
			bFoundVisibleItem = true;
			break;
		}
	}
	if( ! bFoundVisibleItem )
		_size.cy += 1; // + 2.31

#if (!defined __EXT_MFC_NO_RIBBON_BAR)
CExtPopupMenuWnd * pParent = GetParentMenuWnd();
	if(		pParent != NULL
		&&	pParent->_IsTopLevelPopup()
		&&	(pParent->TrackFlagsGet()&TPMX_RIBBON_FILE_MENU) != 0
		)
	{
		CExtRibbonGalleryPopupMenuWnd * pGalleryMenu =
			DYNAMIC_DOWNCAST( CExtRibbonGalleryPopupMenuWnd, pParent );
		if( pGalleryMenu != NULL )
		{
			CSize _sizeAdjust = pGalleryMenu->m_sizeChildControl;
			_sizeAdjust.cx -= nMenuShadowSize;
			_sizeAdjust.cy -= nMenuShadowSize;
			_size.cx = max( _size.cx, _sizeAdjust.cx );
			_size.cy = max( _size.cy, _sizeAdjust.cy );
		}
	}
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

CSize _sizeCaption = MenuCaptionMeasure();
INT nCaptionMetric = _sizeCaption.cx + rcMB.left + rcMB.right;
	_size.cx = max( _size.cx, nCaptionMetric ); // + 2.89

	return _size;
}

CRect CExtPopupMenuWnd::_CalcTrackRect()
{
	ASSERT_VALID( this );

	m_eCombineAlign = __CMBA_NONE;

bool bRTL = OnQueryLayoutRTL();

CExtPaintManager::monitor_parms_t _mp;
	if( m_bExcludeAreaSpec )
		CExtPaintManager::stat_GetMonitorParms( _mp, m_rcExcludeArea );
	else if( m_pWndParentMenu != NULL )
	{
		ASSERT_VALID( m_pWndParentMenu );
		ASSERT( m_pWndParentMenu->GetSafeHwnd() != NULL );
		CExtPaintManager::stat_GetMonitorParms( _mp, m_pWndParentMenu );
	} // else if( m_pWndParentMenu != NULL )
	else
		CExtPaintManager::stat_GetMonitorParms( _mp, m_ptTrack );
CRect rcDesktop = g_bUseDesktopWorkArea
		? _mp.m_rcWorkArea
		: _mp.m_rcMonitor
		;

CSize _size = _CalcTrackSize();
bool bTearOff = _IsTearOff();
int nTearOffCaptionHeight = bTearOff ? _GetTearOffCaptionHeight() : 0;
	if( bTearOff )
		_size.cy += nTearOffCaptionHeight;
int nMenuShadowSize =
		OnQueryMenuShadowSize();
CRect rcMB = OnQueryMenuBorderMetrics();
int nGapShift = 0;
	if( ! m_bCombineWithEA )
		nGapShift = rcMB.top + nMenuShadowSize;

	if( (m_dwTrackFlags&TPMX_ALIGN_MASK) == 0 && (!m_bCombineWithEA) )
	{
		if( m_pWndParentMenu->GetSafeHwnd() != NULL )
		{
			if( (m_pWndParentMenu->m_dwTrackFlags&TPMX_ALIGN_MASK) != 0 )
				m_dwTrackFlags |=
					m_pWndParentMenu->m_dwTrackFlags
						& (~(TPMX_ALIGN_MASK|TPMX_PALETTE|TPMX_PALETTE_TB_BKGND|TPMX_RIBBON_RESIZING|TPMX_RIBBON_RESIZING_VERTICAL_ONLY));
			else
			{
				CRect rc1;
				m_pWndParentMenu->GetWindowRect( &rc1 );
				if( m_pWndParentMenu->m_pWndParentMenu->GetSafeHwnd() != NULL )
				{
					CRect rc2;
					m_pWndParentMenu->m_pWndParentMenu->GetWindowRect(&rc2);
					int x1 = rc1.left + rc1.Width()/2;
					int x2 = rc2.left + rc2.Width()/2;
					if( x1 >= x2 )
						m_dwTrackFlags |= TPMX_LEFTALIGN;
					else
						m_dwTrackFlags |= TPMX_RIGHTALIGN;
				} // if( m_pWndParentMenu->m_pWndParentMenu->GetSafeHwnd() != NULL )
			} // else from if( (m_pWndParentMenu->m_dwTrackFlags&TPMX_ALIGN_MASK) != 0 )
		} // if( m_pWndParentMenu->GetSafeHwnd() != NULL )
	} // if( (m_dwTrackFlags&TPMX_ALIGN_MASK) == 0 && (!m_bCombineWithEA) )

CRect wr( m_ptTrack, _size );
int nCombineOffset = 0;
	if( m_bCombineWithEA )
	{
		if( (m_dwTrackFlags&TPMX_ALIGN_MASK) == TPMX_TOPALIGN )
		{
			nCombineOffset = 1;
			wr.OffsetRect( 0, -nCombineOffset );
		}
		if( (m_dwTrackFlags&TPMX_ALIGN_MASK) == TPMX_LEFTALIGN )
		{
			nCombineOffset = 1;
			wr.OffsetRect( -nCombineOffset, 0 );
		}
		if( (m_dwTrackFlags&TPMX_ALIGN_MASK) == TPMX_BOTTOMALIGN )
		{
			nCombineOffset = -nMenuShadowSize - 1;
			wr.OffsetRect( 0 , -nCombineOffset );
		}
		if( (m_dwTrackFlags&TPMX_ALIGN_MASK) == TPMX_RIGHTALIGN )
		{
			if( (! bRTL) || m_bTopLevel )
				nCombineOffset = -nMenuShadowSize - 1;
			wr.OffsetRect( -nCombineOffset, 0 );
		}
	} // if( m_bCombineWithEA )

	if(		bRTL
		&&	(	(m_dwTrackFlags&TPMX_ALIGN_MASK) == TPMX_TOPALIGN
			||	(m_dwTrackFlags&TPMX_ALIGN_MASK) == TPMX_BOTTOMALIGN
			)
		)
	{
		wr.OffsetRect(
			- wr.Width() + m_rcExcludeArea.Width()
				+ nMenuShadowSize,
			0
			);
	}
	
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
CExtPopupMenuWnd * pParent = GetParentMenuWnd();
	if(		pParent != NULL
		&&	pParent->_IsTopLevelPopup()
		&&	(pParent->TrackFlagsGet()&TPMX_RIBBON_FILE_MENU) != 0
		&&	pParent->GetSafeHwnd() != NULL
		)
	{
		CExtRibbonGalleryPopupMenuWnd * pGalleryMenu =
			DYNAMIC_DOWNCAST( CExtRibbonGalleryPopupMenuWnd, pParent );
		if( pGalleryMenu != NULL )
		{
			CRect wr;
			::GetWindowRect( pGalleryMenu->GetChildCtrlHWND(), &wr );
			CSize _sizeTrack = pGalleryMenu->m_sizeChildControl;
			_sizeTrack.cx = max( _sizeTrack.cx, _size.cx );
			_sizeTrack.cy = max( _sizeTrack.cy, _size.cy );
			_sizeTrack.cx -= nMenuShadowSize;
			_sizeTrack.cy -= nMenuShadowSize;
			wr.right = wr.left + _sizeTrack.cx;
			wr.bottom = wr.top + _sizeTrack.cy;

			m_rcClient = wr;
			m_rcClient.OffsetRect( -wr.TopLeft() );
			m_rcClient.DeflateRect( 0, 0, nMenuShadowSize, nMenuShadowSize );
			return wr;
		}
	}
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)

	if( wr.bottom > rcDesktop.bottom )
	{
		if( m_bExcludeAreaSpec )
		{
			wr.OffsetRect(
				0,
				-wr.Height()
					+ m_rcExcludeArea.Height()
					+ nGapShift
					+ (m_bCombineWithEA ? nMenuShadowSize : 0)
				);
		} // if( m_bExcludeAreaSpec )
		if( wr.bottom > rcDesktop.bottom )
			wr.OffsetRect(
				0,
				m_bExcludeAreaSpec
					? -(wr.bottom - rcDesktop.bottom)
					: - wr.Height()
				);
	} // if( wr.bottom > rcDesktop.bottom )
	if( wr.top < rcDesktop.top )
		wr.OffsetRect( 0, rcDesktop.top - wr.top );

	if( wr.right > rcDesktop.right )
	{
		if( (!m_bTopLevel) && m_bExcludeAreaSpec )
			wr.OffsetRect(
				-wr.Width() - m_rcExcludeArea.Width(),
				0
				);
		if( wr.right > rcDesktop.right )
			wr.OffsetRect(
				m_bExcludeAreaSpec
					? -(wr.right - rcDesktop.right)
					: -wr.Width(),
				0
				);
	} // if( wr.right > rcDesktop.right )
	if( wr.left < rcDesktop.left )
	{
		if( (!m_bTopLevel) && m_bExcludeAreaSpec )
			wr.OffsetRect(
				wr.Width() + m_rcExcludeArea.Width(),
				0
				);
		if( wr.left < rcDesktop.left )
			wr.OffsetRect( rcDesktop.left - wr.left, 0 );
	} // if( wr.left < rcDesktop.left )

	if( m_bExcludeAreaSpec || m_bCombineWithEA )
	{
		CRect rcIntersection;
		if(		rcIntersection.IntersectRect(
					&wr,
					&m_rcExcludeArea
					)
			||	m_bCombineWithEA
			)
		{ // if need adjust with exclude area
			CRect rcFree( 0, 0, 0, 0 );
			if( m_rcExcludeArea.left > rcDesktop.left )
				rcFree.left = m_rcExcludeArea.left - rcDesktop.left;
			if( m_rcExcludeArea.right < rcDesktop.right )
				rcFree.right = rcDesktop.right - m_rcExcludeArea.right;
			if( m_rcExcludeArea.top > rcDesktop.top )
				rcFree.top = m_rcExcludeArea.top - rcDesktop.top;
			if( m_rcExcludeArea.bottom < rcDesktop.bottom )
				rcFree.bottom = rcDesktop.bottom - m_rcExcludeArea.bottom;
			if(		(m_dwTrackFlags&TPMX_ALIGN_MASK) == TPMX_LEFTALIGN
				&&	rcFree.right > wr.Width()
				)
				wr.OffsetRect(
					wr.left - m_rcExcludeArea.right
						+ (m_bCombineWithEA ? nCombineOffset : 0)
						,
					0
					);
			if(		(m_dwTrackFlags&TPMX_ALIGN_MASK) == TPMX_RIGHTALIGN
				&&	rcFree.left > wr.Width()
				)
				wr.OffsetRect(
					m_rcExcludeArea.left - wr.right
						- (m_bCombineWithEA ? nCombineOffset : 0)
						,
					0
					);

			if( (m_dwTrackFlags&TPMX_ALIGN_MASK) == TPMX_TOPALIGN )
			{
				if( rcFree.bottom > wr.Height() )
					wr.OffsetRect(
						0,
						wr.top - m_rcExcludeArea.bottom
							+ (m_bCombineWithEA ? nCombineOffset : 0)
						);
				else
				{
					if( GetSafeHwnd() != NULL )
					{
						CRect rcWndOld;
						GetWindowRect( &rcWndOld );
						ShowWindow( SW_HIDE );
						_FreeWinObjects();
						HWND hWndThis = GetSafeHwnd();
						bool bWasCreated =
							(hWndThis != NULL && ::IsWindow(hWndThis))
								? true : false;
//						CExtPopupMenuWnd::PassMsgLoop(
//							CExtPopupBaseWnd::g_bEnableOnIdleCalls
//							);
						if( bWasCreated && (!::IsWindow(hWndThis)) )
							return CRect( 0, 0, 0, 0 );
						if( ! rcWndOld.IsRectEmpty() )
						{
							CWnd * pWndTLP = GetTopLevelParent();
							if( pWndTLP != NULL )
							{
								ASSERT_VALID( pWndTLP );
								if( pWndTLP->IsWindowVisible() )
								{
									CRect rcTLP;
									pWndTLP->GetWindowRect( &rcTLP );
									if( ! rcTLP.IsRectEmpty() )
									{
										CRect rcIntersection;
										if(	rcIntersection.IntersectRect(
												&rcTLP,
												&rcWndOld
												)
											)
											pWndTLP->UpdateWindow();
									} // if( !rcTLP.IsRectEmpty() )
								} // if( pWndTLP->IsWindowVisible() )
							} // if( pWndTLP != NULL )
						} // if( ! rcWndOld.IsRectEmpty() )
					} // if( GetSafeHwnd() != NULL )
					m_dwTrackFlags &= ~(TPMX_ALIGN_MASK);
					//m_dwTrackFlags |= TPMX_BOTTOMALIGN;
					m_dwTrackFlags |= TPMX_BOTTOMALIGN; // TPMX_RIGHTALIGN;
					return _CalcTrackRect();
				} // else from if( rcFree.bottom > wr.Height() )
			} // if( (m_dwTrackFlags&TPMX_ALIGN_MASK) == TPMX_TOPALIGN )

			if(		(m_dwTrackFlags&TPMX_ALIGN_MASK) == TPMX_BOTTOMALIGN
				&&	rcFree.top > wr.Height()
				)
				wr.OffsetRect(
					0,
					m_rcExcludeArea.top - wr.bottom
						- (m_bCombineWithEA ? nCombineOffset : 0)
					);
		} // if need adjust with exclude area
	} // if( m_bExcludeAreaSpec || m_bCombineWithEA )

	if( wr.right > rcDesktop.right )
	{
		wr.OffsetRect( -(wr.right - rcDesktop.right), 0 );
		if( m_bTopLevel )
		{
			m_bExcludeAreaSpec = false;
			m_bCombineWithEA = false;
		}
	} // if( wr.right > rcDesktop.right )
	if( wr.left < rcDesktop.left )
	{
		wr.OffsetRect( rcDesktop.left - wr.left, 0 );
		if( m_bTopLevel )
		{
			m_bExcludeAreaSpec = false;
			m_bCombineWithEA = false;
		}
	} // if( wr.left < rcDesktop.left )
	if( wr.bottom > rcDesktop.bottom )
	{
		wr.OffsetRect( 0, -(wr.bottom - rcDesktop.bottom) );
		if( m_bTopLevel )
		{
			m_bExcludeAreaSpec = false;
			m_bCombineWithEA = false;
		}
	} // if( wr.bottom > rcDesktop.bottom )
	if( wr.top < rcDesktop.top )
	{
		wr.OffsetRect( 0, rcDesktop.top - wr.top );
		if( m_bTopLevel )
		{
			m_bExcludeAreaSpec = false;
			m_bCombineWithEA = false;
		}
	} // if( wr.top < rcDesktop.top )
	if( wr.bottom > rcDesktop.bottom )
	{
		wr.bottom = rcDesktop.bottom;
		m_bScrollingAvailable = true;
	} // if( wr.bottom > rcDesktop.bottom )

	if( m_bCombineWithEA )
	{
		ASSERT( m_bExcludeAreaSpec );
		if( wr.bottom == m_rcExcludeArea.top - nCombineOffset )
			m_eCombineAlign = __CMBA_BOTTOM;
		if( wr.top == m_rcExcludeArea.bottom  - nCombineOffset )
			m_eCombineAlign = __CMBA_TOP;
		if( wr.left == m_rcExcludeArea.right - nCombineOffset )
			m_eCombineAlign = __CMBA_LEFT;
		if( wr.right == m_rcExcludeArea.left - nCombineOffset  )
			m_eCombineAlign = __CMBA_RIGHT;
		if( m_eCombineAlign == __CMBA_NONE )
		{
			m_bCombineWithEA = false;
			return _CalcTrackRect();
		} // if( m_eCombineAlign == __CMBA_NONE )
		// init window region
		if( m_rgnWnd.GetSafeHandle() != NULL )
		{
			VERIFY( m_rgnWnd.DeleteObject() );
		}
		CRect rcExcludeArea(m_rcExcludeArea);
		rcExcludeArea.InflateRect(
			0,
			0,
			nMenuShadowSize,
			nMenuShadowSize
			);
		CRect rcFullWnd(
			min( wr.left, rcExcludeArea.left ),
			min( wr.top, rcExcludeArea.top ),
			max( wr.right, rcExcludeArea.right ),
			max( wr.bottom, rcExcludeArea.bottom )
			);

		if( ! m_bTopLevel )
		{
			if(			m_eCombineAlign == __CMBA_LEFT
					||	m_eCombineAlign == __CMBA_RIGHT
					)
					rcExcludeArea.DeflateRect( 0, 0, 0, nMenuShadowSize );
			else if(	m_eCombineAlign == __CMBA_TOP
					||	m_eCombineAlign == __CMBA_BOTTOM
					)
					rcExcludeArea.DeflateRect( 0, 0, nMenuShadowSize, 0 );
		} // if( ! m_bTopLevel )
		
		CRgn rgnAreaWnd, rgnAreaExclude;
		if(		(! m_rgnWnd.CreateRectRgnIndirect( &rcFullWnd ) )
			||	(! rgnAreaExclude.CreateRectRgnIndirect( &rcExcludeArea ) )
			||	(! rgnAreaWnd.CreateRectRgnIndirect( &wr ) )
			||	m_rgnWnd.CombineRgn(
					&rgnAreaWnd,
					&rgnAreaExclude,
					RGN_OR
					) == ERROR
			||	m_rgnWnd.OffsetRgn(
					-rcFullWnd.TopLeft()
					) == ERROR
			)
		{
			if( m_rgnWnd.GetSafeHandle() != NULL )
			{
				VERIFY( m_rgnWnd.DeleteObject() );
			}
			m_bCombineWithEA = false;
			return _CalcTrackRect();
		}
		ASSERT( m_rgnWnd.GetSafeHandle() != NULL );
		
		m_rcClient = wr;
		m_rcClient.OffsetRect( -rcFullWnd.TopLeft() );
		m_rcClient.DeflateRect( 0, 0, nMenuShadowSize, nMenuShadowSize );
		ASSERT( m_rcClient.top <= m_rcClient.bottom );
		ASSERT( m_rcClient.left <= m_rcClient.right );
//		if( bTearOff )
//			m_rcClient.top += nTearOffCaptionHeight;

		if( ! m_bTopLevel )
		{
			if( m_eCombineAlign == __CMBA_RIGHT )
				rcFullWnd.OffsetRect( nMenuShadowSize + 1, 0 );
			else if( m_eCombineAlign == __CMBA_LEFT )
				rcFullWnd.OffsetRect( -2, 0 );
		}

		return rcFullWnd;
	} // if( m_bCombineWithEA )
	m_rcClient = wr;
	m_rcClient.OffsetRect( -wr.TopLeft() );
	m_rcClient.DeflateRect( 0, 0, nMenuShadowSize, nMenuShadowSize );
//	if( bTearOff )
//		m_rcClient.top += nTearOffCaptionHeight;
	ASSERT( m_rcClient.top <= m_rcClient.bottom );
	ASSERT( m_rcClient.left <= m_rcClient.right );
	return wr;
}

bool CExtPopupMenuWnd::OnQueryLayoutRTL() const
{
	ASSERT_VALID( this );
	if( m_pWndParentMenu != NULL )
	{
		ASSERT_VALID( this );
		return m_pWndParentMenu->OnQueryLayoutRTL();
	} // if( m_pWndParentMenu != NULL )
	return CExtPopupBaseWnd::OnQueryLayoutRTL();
}

void CExtPopupMenuWnd::_CancelingSet()
{
	ASSERT_VALID( this );
	if( m_bCanceling )
		return;
	m_bCanceling = true;
	//if( ( GetStyle() & WS_VISIBLE ) != 0 )
	{
		if( m_pWndParentMenu != NULL )
		{
			if( ! m_pWndParentMenu->_CancelingGet() )
				m_pWndParentMenu->_CancelingSet();
		}
		if(		m_nCurIndex >= 0
			&&	ItemGetInfo( m_nCurIndex ).IsPopup()
			)
		{
			CExtPopupMenuWnd * pPopup = ItemGetInfo( m_nCurIndex ).GetPopup();
			if( ! pPopup->_CancelingGet() )
				pPopup->_CancelingSet();
		}
	}
}

bool CExtPopupMenuWnd::_CancelingGet() const
{
	ASSERT_VALID( this );
	return m_bCanceling;
}

BOOL CExtPopupMenuWnd::_TrackPopupMenu(
	DWORD dwTrackFlags,
	int x,
	int y,
	LPCRECT lpRect,
	LPVOID pCbPaintCombinedCookie, // = NULL
	pCbPaintCombinedContent pCbPCC, // = NULL
	bool bCookieIsObject // = false
	)
{
	///////////////////////////// dwTrackFlags |= TPMX_COMBINE_NONE|TPMX_FORCE_NO_ANIMATION|TPMX_NO_DYNAMIC_SHADOWS|TPMX_NO_SHADOWS;
	ASSERT_VALID( this );
	m_bCanceling = false;

	CExtPaintManager::stat_PassPaintMessages();

	::GetCursorPos( &m_ptTrackInvoked );
	m_ptTrackWatched = m_ptTrackInvoked;

	if( _IsTopLevelPopup() )
		m_bHelperMouseBtnDownOnStart =
			(	IsKeyPressed( VK_LBUTTON )
			||	IsKeyPressed( VK_RBUTTON )
			) ? true : false;
	else
		m_bHelperMouseBtnDownOnStart = false;
	if( m_bHelperMouseBtnDownOnStart )
		::GetCursorPos( &m_ptStartMousePos );
bool bForceExpandRarelyUsed = (dwTrackFlags&TPMX_NO_HIDE_RARELY)
		? true : false;
	if( ! g_bMenuExpanding )
		bForceExpandRarelyUsed = true;

	ASSERT( m_hWndCmdReceiver != NULL );
	ASSERT( ::IsWindow(m_hWndCmdReceiver) );

	if( GetSafeHwnd() != NULL )
		::DestroyWindow( m_hWnd ); // fade out animation effect

CExtPopupMenuSite & _site = GetSite();

	if(		(!_FindCustomizeMode())
		&&	(	_site.IsTopPopup(this)
			||	GetParentMenuWnd() == NULL
			)
		)
	{
		ASSERT( m_bTopLevel );
		MsgPrepareMenuData_t _mpmEntireTree( this );
		_mpmEntireTree.SendMessage( m_hWndCmdReceiver, false );
		if( _mpmEntireTree.m_bMenuCanceled )
			return FALSE;
		if( _mpmEntireTree.m_bMenuChanged )
		{
			_SyncItems();
			_UpdateCmdUI();
		}
		ASSERT( m_bTopLevel );
//		ASSERT( _site.IsTopPopup(this) );
	}
MsgPrepareMenuData_t _mpmOneTreeLevel( this );
	_mpmOneTreeLevel.SendMessage( m_hWndCmdReceiver, true );
	if( _mpmOneTreeLevel.m_bMenuCanceled )
		return FALSE;
	if( _mpmOneTreeLevel.m_bMenuChanged )
	{
		_SyncItems();
		_UpdateCmdUI();
	}

	if( !_FindCustomizeMode() )
	{ // BLOCK: update system commands
		INT iter = 0;
		for(; iter < m_items_all.GetSize(); ++iter )
		{
			MENUITEMDATA & mi = ItemGetInfo( iter );
			if( mi.IsSeparator() )
				continue;
			UINT nCmdID = mi.GetCmdID();
			if( ! CExtCmdManager::IsSystemCommand( nCmdID ) )
				continue;
			WINDOWPLACEMENT wndpl;
			::memset(&wndpl,0,sizeof(WINDOWPLACEMENT));
			wndpl.length = sizeof(WINDOWPLACEMENT);
			VERIFY(
				::GetWindowPlacement(
					mi.GetCmdReceiver(),
					&wndpl
					)
				);
			__EXT_MFC_LONG_PTR dwWndStyle = ::__EXT_MFC_GetWindowLong( mi.GetCmdReceiver(), GWL_STYLE );
			__EXT_MFC_LONG_PTR dwWndExStyle = ::__EXT_MFC_GetWindowLong( mi.GetCmdReceiver(), GWL_EXSTYLE );
			bool bSysCmdEnabled = false;
			switch( nCmdID )
			{
			case SC_CLOSE:
				{
					bSysCmdEnabled = true;
					HMENU hSysMenu = ::GetSystemMenu( mi.GetCmdReceiver(), FALSE );
					MENUITEMINFO _mii;
					::memset( &_mii, 0, sizeof(MENUITEMINFO) );
					_mii.cbSize = sizeof(MENUITEMINFO);
					_mii.fMask = MIIM_STATE;
					if(		hSysMenu != NULL
						&&	::GetMenuItemInfo(
								hSysMenu,
								SC_CLOSE,
								FALSE,
								&_mii
								)
						)
					{
						if( (_mii.fState & MFS_DISABLED) != 0 )
							bSysCmdEnabled = false;
					} // if( hSysMenu != NULL ...
				}
			break;
			case SC_SIZE:
			case SC_MOVE:
				if(		wndpl.showCmd != SW_SHOWMINIMIZED
					&&	wndpl.showCmd != SW_SHOWMAXIMIZED
					&&	!g_bFullScreenMode
					)
					bSysCmdEnabled = true;
			break;
			case SC_MINIMIZE:
				if(		(dwWndStyle & WS_MINIMIZEBOX) != 0
					&&	wndpl.showCmd != SW_SHOWMINIMIZED
					)
					bSysCmdEnabled = true;
			break;
			case SC_MAXIMIZE:
				if(		(dwWndStyle & WS_MAXIMIZEBOX) != 0
					&&	wndpl.showCmd != SW_SHOWMAXIMIZED
					&&	!g_bFullScreenMode
					)
					bSysCmdEnabled = true;
			break;
			case SC_RESTORE:
				if(		(dwWndStyle & (WS_MINIMIZEBOX|WS_MAXIMIZEBOX)) != 0
					&&	(	wndpl.showCmd == SW_SHOWMAXIMIZED
						||	wndpl.showCmd == SW_SHOWMINIMIZED
						//||wndpl.showCmd == SW_SHOWNORMAL
						)
					)
					bSysCmdEnabled = true;
			break;
			case SC_CONTEXTHELP:
				if( (dwWndExStyle & WS_EX_CONTEXTHELP) != 0 )
					bSysCmdEnabled = true;
			break;
//			case SC_NEXTWINDOW:
//			case SC_PREVWINDOW:
//			case SC_VSCROLL:
//			case SC_HSCROLL:
//			case SC_MOUSEMENU:
//			case SC_KEYMENU:
//			case SC_ARRANGE:
//			case SC_TASKLIST:
//			case SC_SCREENSAVE:
//#if(WINVER >= 0x0400)
//			case SC_DEFAULT:
//			case SC_MONITORPOWER:
//			case SC_SEPARATOR:
//#endif /* WINVER >= 0x0400 */
			case SC_HOTKEY:
			default:
				continue;
			} // switch( nCmdID )
			CExtCmdItem * pCmdItem =
				g_CmdManager->CmdGetPtr(
					g_CmdManager->ProfileNameFromWnd( m_hWndCmdReceiver ),
					nCmdID
					);
			if( pCmdItem == NULL )
			{
				pCmdItem =
					g_CmdManager->CmdAllocPtr(
						g_CmdManager->ProfileNameFromWnd( m_hWndCmdReceiver ),
						nCmdID
						);
				ASSERT( pCmdItem != NULL );
			}
			pCmdItem->StateEnable( bSysCmdEnabled );
			mi.Enable( bSysCmdEnabled );
			ASSERT( CExtCmdManager::IsSystemCommand( nCmdID ) );
			HWND hWndItemCmdReceiver = mi.GetCmdReceiver();
			HMENU hSysMenu = ::GetSystemMenu( hWndItemCmdReceiver, FALSE );
			if( hSysMenu != NULL )
			{
				MENUITEMINFO _mii;
				::memset( &_mii, 0, sizeof(MENUITEMINFO) );
				_mii.cbSize = sizeof(MENUITEMINFO);
				_mii.fMask =
					MIIM_CHECKMARKS|MIIM_DATA|MIIM_ID|MIIM_STATE
					|MIIM_SUBMENU|MIIM_TYPE;
				_mii.cch = __MAX_UI_ITEM_TEXT;
				CExtSafeString sMenuItemText;
				_mii.dwTypeData = sMenuItemText.GetBuffer( __MAX_UI_ITEM_TEXT );
				ASSERT( _mii.dwTypeData != NULL );
				if( _mii.dwTypeData != NULL )
				{
					if(	::GetMenuItemInfo(
							hSysMenu,
							nCmdID,
							FALSE,
							&_mii
							)
						)
					{
						sMenuItemText.ReleaseBuffer();
						sMenuItemText.Replace( _T("\n"), _T("") );
						sMenuItemText.Replace( _T("\r"), _T("") );
						sMenuItemText.TrimLeft();
						sMenuItemText.TrimRight();
						if( ! sMenuItemText.IsEmpty() )
						{
							int nSep =
								sMenuItemText.ReverseFind( _T('\t') );
							if( nSep >= 0 )
							{
								int nLen = sMenuItemText.GetLength();
								pCmdItem->m_sAccelText = sMenuItemText.Right( nLen - nSep );
								pCmdItem->m_sAccelText.TrimLeft();
								pCmdItem->m_sAccelText.TrimRight();
								pCmdItem->m_sMenuText = sMenuItemText.Left( nSep );
								pCmdItem->m_sMenuText.TrimLeft();
								pCmdItem->m_sMenuText.TrimRight();
							}
							else
							{
								pCmdItem->m_sMenuText = sMenuItemText;
								pCmdItem->m_sAccelText = _T("");
							}
							if( pCmdItem->m_nCmdID == SC_CLOSE )
							{
								CWnd * pWnd = CWnd::FromHandlePermanent( hWndItemCmdReceiver );
								if(		pWnd != NULL
									&&	pWnd->IsKindOf( RUNTIME_CLASS(CMDIChildWnd) )
									)
									pCmdItem->m_sAccelText = _T("Ctrl+F4");
							}
							mi.SetPopupText( pCmdItem->m_sMenuText );
							mi.SetAccelText( pCmdItem->m_sAccelText );
						} // if( ! sMenuItemText.IsEmpty() )
					}
					else
						sMenuItemText.ReleaseBuffer();
				} // if( _mii.dwTypeData != NULL )
				if( (_mii.fState&MFS_DEFAULT) != 0 )
					mi.SetDefault();
			} // if( hSysMenu != NULL )
		} // for(; iter < m_items_all.end(); ++iter )
	} // BLOCK: update system commands

CWnd * pWndCmdReceiver =
		CWnd::FromHandle( m_hWndCmdReceiver );
	ASSERT_VALID( pWndCmdReceiver );
//	pWndCmdReceiver->ActivateTopParent();
//	pWndCmdReceiver->BringWindowToTop();
//	pWndCmdReceiver->SetFocus();
	
	//_site._Hook( true );

	// adjust own data
bool bOldTopLevel = m_bTopLevel;
bool bOldExpandAvailable = m_bExpandAvailable;
DWORD dwPortedTrackFlags = m_dwTrackFlags&(TPMX_PALETTE|TPMX_PALETTE_TB_BKGND);
	_Init();
	m_bTopLevel = bOldTopLevel;
	m_bExpandAvailable = bOldExpandAvailable;
	m_dwTrackFlags = dwTrackFlags | dwPortedTrackFlags;
	m_pCbPaintCombinedCookie = pCbPaintCombinedCookie;
	m_pCbPaintCombinedContent = pCbPCC;
	m_bCookieIsObject = bCookieIsObject;
	if( !m_bTopLevel )
	{
		ASSERT( m_pWndParentMenu != NULL );
		if( m_pWndParentMenu->m_bExpandWasPressed )
		{
			if( m_bExpandAvailable )
			{
				m_bExpandAvailable = false;
				m_bExpandWasPressed = true;
				_SyncItems();
			}
			else
				m_bExpandWasPressed = true;
		}
	} // if( !m_bTopLevel )
	else
	{
		if( bForceExpandRarelyUsed )
		{
			if( m_bExpandAvailable )
			{
				m_bExpandAvailable = false;
				m_bExpandWasPressed = true;
				_SyncItems();
			}
			else
				m_bExpandWasPressed = true;
		} // if( bForceExpandRarelyUsed )
		else
			_SyncItems();
	} // else from if( !m_bTopLevel )

	// adjust screen position
	m_ptTrack.x = m_ptTrackOriginal.x = x;
	m_ptTrack.y = m_ptTrackOriginal.y = y;
	if(		( m_ptTrack.x < -32000 || m_ptTrack.y < -32000 )
		&&	(dwTrackFlags&TPMX_RIBBON_MODE) == 0
		)
	{
		if( ! ::GetCursorPos(&m_ptTrack) )
			return FALSE;
	}

	if( lpRect != NULL )
	{
		m_rcExcludeArea = *lpRect;
		m_bExcludeAreaSpec = true;
	}
	else
	{
		m_bExcludeAreaSpec = false;
		m_rcExcludeArea.left = m_ptTrack.x - __EXCLUDE_AREA_GAP_DX;
		m_rcExcludeArea.right = m_ptTrack.x + __EXCLUDE_AREA_GAP_DX;
		m_rcExcludeArea.top = m_ptTrack.y - __EXCLUDE_AREA_GAP_DY;
		m_rcExcludeArea.bottom = m_ptTrack.y + __EXCLUDE_AREA_GAP_DY;
	}

	// adjust combine with exclude area mode
	m_bCombineWithEA = false;
	if( m_bExcludeAreaSpec )
	{
		switch( (dwTrackFlags&TPMX_COMBINE_MASK) )
		{
		case TPMX_COMBINE_ANY_SUITABLE:
			m_bCombineWithEA = true;
		break;
		case TPMX_COMBINE_DEFAULT:
			m_bCombineWithEA =
				PmBridge_GetPM()->
					IsMenuMustCombineExcludeArea(
						bCookieIsObject
							? ((CObject*)pCbPaintCombinedCookie)
							: NULL
						);
		break;
		} // switch( (dwTrackFlags&TPMX_COMBINE_MASK) )
	} // if( m_bExcludeAreaSpec )

CSize _size = _CalcTrackSize();
bool bPointAdjusted = true;
	if( m_bExcludeAreaSpec )
	{
		bool bRTL = OnQueryLayoutRTL();
		if( bRTL && (!m_bTopLevel) )
		{
			CExtPopupMenuWnd * pTop = m_pWndParentMenu;
			for( ; pTop != NULL; pTop = pTop->GetParentMenuWnd() )
			{
				if( pTop->_IsTopLevelPopup() )
					break;
			} // for( ; pTop != NULL; pTop = pTop->GetParentMenuWnd() )
			if( pTop != NULL )
			{
				switch( (pTop->m_dwTrackFlags & TPMX_ALIGN_MASK) )
				{
				case TPMX_TOPALIGN:
				case TPMX_BOTTOMALIGN:
					m_dwTrackFlags &= ~(TPMX_ALIGN_MASK);
					m_dwTrackFlags |= TPMX_RIGHTALIGN;
				break;
				} // switch( (pTop->m_dwTrackFlags & TPMX_ALIGN_MASK) )
			} // if( pTop != NULL )
		} // if( bRTL && (!m_bTopLevel) )
		switch( (m_dwTrackFlags & TPMX_ALIGN_MASK) )
		{
		case TPMX_LEFTALIGN:
			m_ptTrack.x = m_rcExcludeArea.right;
			m_ptTrack.y = m_rcExcludeArea.top;
		break;
		case TPMX_RIGHTALIGN:
			m_ptTrack.x = m_rcExcludeArea.left - _size.cx;
			m_ptTrack.y = m_rcExcludeArea.top;
		break;
		case TPMX_TOPALIGN:
			m_ptTrack.x = m_rcExcludeArea.left;
			m_ptTrack.y = m_rcExcludeArea.bottom;
		break;
		case TPMX_BOTTOMALIGN:
			m_ptTrack.x = m_rcExcludeArea.left;
			m_ptTrack.y = m_rcExcludeArea.top - _size.cy;
		break;
		default:
			bPointAdjusted = false;
		break;
		} // switch( (m_dwTrackFlags & TPMX_ALIGN_MASK) )
	} // if( m_bExcludeAreaSpec )
	if( ! bPointAdjusted )
	{
		if( (m_dwTrackFlags & TPMX_ALIGN_MASK) == TPMX_RIGHTALIGN )
			m_ptTrack.x -= _size.cx;
		else
		{
			if( (m_dwTrackFlags & TPMX_ALIGN_MASK) == TPMX_CENTERALIGN )
				m_ptTrack.x -= _size.cx/2;
		}
		if( (m_dwTrackFlags & TPMX_ALIGN_MASK) == TPMX_BOTTOMALIGN )
			m_ptTrack.y -= _size.cy;
		else
		{
			if( (m_dwTrackFlags & TPMX_ALIGN_MASK) == TPMX_VCENTERALIGN )
				m_ptTrack.y -= _size.cy/2;
		}
	} // if( !bPointAdjusted )

//#ifdef _DEBUG
bool bCreateResult =
//#endif // _DEBUG
		_CreateHelper( pWndCmdReceiver );
	//ASSERT( bCreateResult );
	if( !bCreateResult )
		return FALSE;

	if( dwTrackFlags & TPMX_SELECT_ANY )
	{
		int nItem = _GetNextItem(__NI_ANY);
		if( nItem >= 0 )
		{
			HWND hWndOwn = m_hWnd;
			_ItemFocusSet(
				nItem,
				FALSE,
				FALSE
				);
			if( ! ::IsWindow( hWndOwn ) )
				return FALSE;
		}
	}

HWND hWndOwn = m_hWnd;
	if(		(dwTrackFlags&TPMX_FORCE_NO_ANIMATION) != 0
		||	_FindCustomizeMode()
		)
		m_AnimationType = __AT_NONE;
	else
	{
		m_AnimationType = g_DefAnimationType;
		if( CExtToolControlBar::g_bMenuTracking )
			m_AnimationType = __AT_NONE;
		m_ePlaySoundOnAnimationFinished =
			CExtSoundPlayer::__ON_MENU_POPUP_DISPLAYED;
		_StartAnimation();
		if( ! ::IsWindow( hWndOwn ) )
			return FALSE;
	} // if( _FindCustomizeMode() )
	if( m_AnimationType == __AT_NONE )
	{
		m_AnimationType = __AT_CONTENT_DISPLAY;
		m_ePlaySoundOnAnimationFinished =
			CExtSoundPlayer::__ON_MENU_POPUP_DISPLAYED;
		_StartAnimation();
		if( ! ::IsWindow( hWndOwn ) )
			return FALSE;
		ASSERT( m_AnimationType == __AT_CONTENT_DISPLAY );
	}

	if( m_rgnWnd.GetSafeHandle() != NULL )
	{
		ASSERT( m_bExcludeAreaSpec );
		ASSERT( m_bCombineWithEA );
		ASSERT( m_eCombineAlign != __CMBA_NONE );
		CRgn rgnTmp;
		VERIFY( rgnTmp.CreateRectRgn(0,0,0,0) );
		rgnTmp.CopyRgn( &m_rgnWnd );
		ASSERT( rgnTmp.GetSafeHandle() != NULL );
		VERIFY(
			SetWindowRgn(
				(HRGN)rgnTmp.Detach(),
				FALSE
				)
			);
	} // if( m_rgnWnd.GetSafeHandle() != NULL )
	if( ! g_PaintManager.m_bIsWin2000orLater )
		CExtPaintManager::stat_PassPaintMessages();
bool bFadeOut = _IsFadeOutAnimation();
bool bForceLayered = false;
	if(		(! bFadeOut)
		&&	g_PaintManager.m_bIsWin2000orLater
		&&	g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL
		&&	( m_AnimationType == __AT_NONE || m_AnimationType == __AT_CONTENT_DISPLAY )
		//&&	( TrackFlagsGet() & TPMX_RIBBON_RESIZING ) == 0
		)
		bForceLayered = true;
	if( bFadeOut || bForceLayered )
	{
		ASSERT( g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL );
		g_PaintManager.m_pfnSetLayeredWindowAttributes( m_hWnd, 0, 1, __EXT_MFC_LWA_ALPHA );
	} // if( bFadeOut )

	if(		g_PaintManager.m_bIsWinVistaOrLater
		&&	g_PaintManager.m_DWM.IsCompositionEnabled()
		)
	{
		CRect rcCapture;
		GetWindowRect( &rcCapture );
		HBITMAP hBmpScreenSrcAlt = CExtPaintManager::stat_GetScreenSurfacePart( rcCapture );
		if( hBmpScreenSrcAlt != NULL )
		{
			if( m_bmpScreenSrcAlt.GetSafeHandle() != NULL )
				m_bmpScreenSrcAlt.DeleteObject();
			m_bmpScreenSrcAlt.Attach( hBmpScreenSrcAlt );
		}
	}
	SetWindowPos(
		&CWnd::wndTopMost, 0, 0, 0, 0,
		SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE
			|SWP_SHOWWINDOW|SWP_NOZORDER
		);
	if( bFadeOut || bForceLayered )
	{
		ASSERT( g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL );
		CExtPaintManager::stat_PassPaintMessages();
		g_PaintManager.m_pfnSetLayeredWindowAttributes( m_hWnd,  0, 255, __EXT_MFC_LWA_ALPHA );
	} // if( bFadeOut )
	else if( ! g_PaintManager.m_bIsWin2000orLater )
		CExtPaintManager::stat_PassPaintMessages();
	if( ! ::IsWindow( hWndOwn ) )
		return FALSE;

	ASSERT( IsWindowVisible() );

	if( ! _FindCustomizeMode() )
	{
		_SetCapture();
	}
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	else
	{
		CExtCustomizeSite * pSite = _FindCustomizeSite();
		ASSERT( pSite != NULL );
		pSite->RegisterCommandDropTarget( this, this );
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if( m_bTopLevel && (! m_bExpandWasPressed ) && m_bExpandAvailable && g_nAutoExpandTime > 0 )
		SetTimer( ID_TIMER_AUTO_EXPAND, g_nAutoExpandTime, NULL );

	return TRUE;
}

CExtPopupMenuTipWnd & CExtPopupMenuWnd::GetTip()
{
	return m_wndToolTip;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtPopupScreenTipWnd & CExtPopupMenuWnd::GetScreenTip()
{
	return m_wndScreenTip;
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

bool CExtPopupMenuWnd::_CreateHelper(
	CWnd * pWndCmdReceiver
	)
{
	ASSERT_VALID( this );

bool bHasInplaceItems = false;
int nCount = ItemGetCount();
	for( int i = 0; i < nCount; i++ )
	{
		MENUITEMDATA & mi = ItemGetInfo( i );
		if( !mi.IsInplaceEdit() )
			continue;
		//ASSERT( mi.GetInplaceEditPtr() == NULL );
		mi.ResetActiveInplaceEditPtr();
		bHasInplaceItems = true;
		break;
	} // for( int i = 0; i < nCount; i++ )
	if( bHasInplaceItems || _IsPalette() )
		_SyncItems();

CRect wr = _CalcTrackRect();
	if( wr.IsRectEmpty() )
		return false;
UINT nClassStyle = __POPUP_WNDCLASS_STYLES__;
//	if( (TrackFlagsGet()&TPMX_FORCE_NO_ANIMATION) != 0 )
//		nClassStyle &= ~(CS_SAVEBITS);
HCURSOR hCursor = ::LoadCursor( NULL, IDC_ARROW );
	ASSERT( hCursor != NULL );
CExtSafeString strMenuClassName =
		::AfxRegisterWndClass(
			nClassStyle,
			hCursor,
			(HBRUSH)(COLOR_BTNFACE + 1),
			NULL
			);
	ASSERT( !strMenuClassName.IsEmpty() );

	pWndCmdReceiver->SendMessage(
		g_nMsgPrepareDesktopBk,
		WPARAM( this )
		);

bool bFadeOut = _IsFadeOutAnimation(), bForceLayered = false;
	if(		(! bFadeOut)
		&&	g_PaintManager.m_bIsWin2000orLater
		&&	g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL
		&&	( m_AnimationType == __AT_NONE || m_AnimationType == __AT_CONTENT_DISPLAY )
		//&&	( TrackFlagsGet() & TPMX_RIBBON_RESIZING ) == 0
		)
		bForceLayered = true;
BOOL bCreateResult =
		__BaseClassOfCExtPopupBaseWnd::CreateEx(
			_FindCustomizeMode() 
				? 0 
				: WS_EX_TOPMOST
					| ( g_PaintManager.m_bIsWin2000orLater ? (WS_EX_NOINHERITLAYOUT) : 0 )
					| ( ( bFadeOut || bForceLayered ) ? (__EXT_MFC_WS_EX_LAYERED) : 0 )
				,
			strMenuClassName,
			NULL,
			WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
			wr,
			pWndCmdReceiver,
			0 // IDC_STATIC
			);
	if( ! bCreateResult )
	{
		ASSERT( FALSE );
		return false;
	}
	ModifyStyleEx( WS_EX_LAYOUT_RTL, 0, SWP_FRAMECHANGED );
	if( bFadeOut || bForceLayered )
		g_PaintManager.m_pfnSetLayeredWindowAttributes( m_hWnd, 0, 0, __EXT_MFC_LWA_ALPHA );
	if( IsSyncFullRowItems() )
		_SyncItems();
	_RecalcLayoutImpl();
	if( !_FindHelpMode() )
		::SetCursor( hCursor );
	if( bHasInplaceItems )
	{
		bool bRTL = OnQueryLayoutRTL();
		nCount = ItemGetCount();
		for( int i = 0; i < nCount; i++ )
		{
			MENUITEMDATA & mi = ItemGetInfo( i );
			if( !mi.IsInplaceEdit() )
				continue;
			//ASSERT( mi.GetInplaceEditPtr() == NULL );
			mi.ResetActiveInplaceEditPtr();
			CRect rc( 0, 0, 0, 0 );
			if( mi.IsDisplayed() )
			{
				CRect rcItem;
				_GetItemRect( i, rcItem );
				rc = mi.AdjustInplaceEditRect( rcItem, bRTL );
			} // if( mi.IsDisplayed() )
			mi.CreateInplaceEdit( this, rc );
		} // for( i = 0; i < nCount; i++ )
	} // if( bHasInplaceItems )

// 	if( _IsPopupWithShadowsDynamic() )
// 		m_ctrlShadow.Create( m_hWnd, PmBridge_GetPM()->GetMenuShadowSize() );
	return true;
}

bool CExtPopupMenuWnd::IsNoHighlightIconArea()
{
	ASSERT_VALID( this );
	if(		_IsTopLevelPopup()
		&&	(TrackFlagsGet()&TPMX_RIBBON_FILE_MENU) != 0
		)
		return true;
CExtPopupMenuWnd * pParent = GetParentMenuWnd();
	if(		pParent != NULL
		&&	pParent->_IsTopLevelPopup()
		&&	(pParent->TrackFlagsGet()&TPMX_RIBBON_FILE_MENU) != 0
		)
		return true;
	return false;
}

bool CExtPopupMenuWnd::IsSyncFullRowItems()
{
	ASSERT_VALID( this );
	if( (TrackFlagsGet()&(TPMX_RIBBON_RESIZING)) != 0 )
		return true;
CExtPopupMenuWnd * pParent = GetParentMenuWnd();
	if(		pParent != NULL
		&&	pParent->_IsTopLevelPopup()
		&&	(pParent->TrackFlagsGet()&TPMX_RIBBON_FILE_MENU) != 0
		)
		return true;
	return false;
}

BOOL CExtPopupMenuWnd::DestroyWindow()
{
__PROF_UIS_MANAGE_STATE;
	ASSERT_VALID( this );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	_KeyTipsShow( false );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

int nCount = ItemGetCount();
	for( int i = 0; i < nCount; i++ )
	{
		MENUITEMDATA & mi = ItemGetInfo( i );
		if( mi.IsInplaceEdit() )
		{
			if( mi.GetInplaceEditPtr() != NULL )
			{
				ASSERT_VALID( mi.GetInplaceEditPtr() );
				ASSERT( mi.GetInplaceEditPtr()->GetSafeHwnd() != NULL );
				mi.GetInplaceEditPtr()->DestroyWindow();
			} // if( mi.GetInplaceEditPtr() != NULL )
			mi.ResetActiveInplaceEditPtr();
		}
	} // for( int i = 0; i < nCount; i++ )

bool bAutoDestroy = false;
	if( _IsFadeOutAnimation() )
	{
		CExtPopupMenuSite & _site = GetSite();
		DWORD dwTrackFlags = TrackFlagsGet();
		if( m_bTopLevel )
		{
			bAutoDestroy = true;
			_site.SetCapture( NULL );
			ASSERT( m_hWndCmdReceiver != NULL );
			if( ::IsWindow(m_hWndCmdReceiver) )
				::SendMessage(
					m_hWndCmdReceiver,
					g_nMsgNotifyMenuClosed,
					0,
					LPARAM( this )
					);
			if(		m_hWndNotifyMenuClosed != NULL
				&&	::IsWindow( m_hWndNotifyMenuClosed )
				)
				::SendMessage(
					m_hWndNotifyMenuClosed,
					g_nMsgNotifyMenuClosed,
					0,
					LPARAM( this )
					);
			CExtControlBar::stat_SetMessageString(
				GetOwner()
				);
		} // if( m_bTopLevel )
		CExtToolControlBar::_CloseCustomMenusAll();
		if( CExtToolControlBar::g_bMenuTracking )
			CExtToolControlBar::_CloseTrackingMenus();
		bool bAnimateSequence = false;
		if( ! m_bHelperAnimationControllerDetected )
		{
			if( m_nCurIndex < 0 )
				bAnimateSequence = true;
			else if(
					( ! ItemGetInfo( m_nCurIndex ).IsPopup() )
				||	ItemGetInfo( m_nCurIndex ).GetPopup()->GetSafeHwnd() == NULL
				)
				bAnimateSequence = true;
		}
		if( bAnimateSequence )
		{
			if(		(! _site.IsEmpty() )
//				&&	(! _site.IsShutdownMode() )
				&&	(dwTrackFlags&TPMX_NO_SITE) == 0
				)
			{
				if( _site.GetInstance() == this )
					_site.DoneInstance();
				//ASSERT( _site.IsEmpty() );
				//ASSERT( !_site.IsShutdownMode() );
			}
			CExtPopupMenuWnd * pPopup = this;
			for( ; pPopup != NULL; )
			{
				CExtPopupMenuWnd * pParent = pPopup->m_pWndParentMenu;
				pPopup->m_bHelperAnimationControllerDetected = true;
				if( pParent == NULL )
					CExtPopupMenuSite::g_DefPopupMenuSite.FadeOutInstanceAdd( pPopup );
				pPopup = pParent;
			}
			m_nFadeOutAnimationStepIndex = 0;
			SetTimer(
				ID_TIMER_FADE_OUT_ANIMATION,
				UINT(m_nFadeOutAnimationEllapse),
				NULL
				);
		}
		return TRUE;
	} // if( _IsFadeOutAnimation() )

	if( m_ctrlShadow.GetSafeHwnd() != NULL )
		m_ctrlShadow.DestroyWindow();
	if( GetSafeHwnd() == NULL )
	{
		ASSERT( ! bAutoDestroy );
		return TRUE;
	}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( _FindCustomizeMode() )
	{
		CExtCustomizeSite * pSite = _FindCustomizeSite();
		ASSERT( pSite != NULL );
		pSite->UnRegisterCommandDropTarget( this );
	} // if( _FindCustomizeMode() )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	return CExtPopupBaseWnd::DestroyWindow();
}

void CExtPopupMenuWnd::_GetItemRect(
	int nIndex,
	RECT & rectItem,
	bool bIncludeIndents // = true
	)
{
	ASSERT_VALID( this );
int nCountOfItems = (int)m_items_all.GetSize();
	if( 0 <= nIndex && nIndex < nCountOfItems )
	{
		if( _IsPalette() )
		{
			_GetItemRectPalette(
				nIndex,
				rectItem,
				bIncludeIndents
				);
			return;
		}
	}
	if( nIndex == IDX_RIBBON_FILE_MENU_OPTIONS_BUTTON )
	{
		rectItem = m_rcRibbonBtnOptions;
		return;
	}
	if( nIndex == IDX_RIBBON_FILE_MENU_EXIT_BUTTON )
	{
		rectItem = m_rcRibbonBtnExit;
		return;
	}
CRect rcClient;
	_GetClientRect( &rcClient );
	rcClient.left += m_nLeftAreaWidth;
	if( m_bScrollingAvailable )
	{
		ASSERT( ! m_rcScrollTop.IsRectEmpty() );
		ASSERT( ! m_rcScrollBottom.IsRectEmpty() );
		if( nIndex == IDX_SCROLL_TOP )
		{
			rectItem = m_rcScrollTop;
			return;
		}
		if( nIndex == IDX_SCROLL_BOTTOM )
		{
			rectItem = m_rcScrollBottom;
			return;
		}
	}
	if(		m_bExpandAvailable
		&&	nIndex == IDX_EXPAND
		)
	{
		rectItem = m_rcExpandBtn;
		if( bIncludeIndents )
			((CRect *)(&rectItem))->DeflateRect(1,0);
		return;
	}
bool bTearOff = _IsTearOff();
int nTearOffCaptionHeight = bTearOff ? _GetTearOffCaptionHeight() : 0;
	if(		bTearOff
		&&	nIndex == IDX_TEAROFF
		)
	{
		rectItem = m_rcTearOffBtn;
		if( bIncludeIndents )
			((CRect *)(&rectItem))->DeflateRect(1,0);
		return;
	}

	if( nCountOfItems == 0 )
	{
		::SetRectEmpty( &rectItem );
		return;
	}
	if( nIndex < 0 )
	{
		::SetRectEmpty( &rectItem );
		return;
	}

	ASSERT( nIndex >= 0 && nIndex < nCountOfItems );
MENUITEMDATA & mi = ItemGetInfo( nIndex );
	if( ! mi.IsDisplayed() )
	{
		::SetRectEmpty( &rectItem );
		return;
	}

CRect rcCache = mi.GetCachedRect( bIncludeIndents );
	if( ! rcCache.IsRectEmpty() )
	{
		::CopyRect( &rectItem, rcCache );
		return;
	}

CRect rcMB = OnQueryMenuBorderMetrics();
int y = m_nDyScrollOffset + rcMB.top;

CRect rcItem;
	rcItem.SetRectEmpty();
	if( mi.IsToolButton() )
	{
		rcItem.left =
			rcMB.left; 
			// + __EXT_MENU_TOOLBTN_GAP; // - 2.28 
		rcItem.right =
			mi.GetMeasuredWidth();
	} // if( mi.IsToolButton() )
	else
	{
		rcItem.left =
			rcMB.left;
		rcItem.right =
			m_sizeFullItems.cx
			+ rcMB.left;
	} // else from if( mi.IsToolButton() )

	if( bIncludeIndents )
	{
		INT nMarginLeft = 0, nMarginRight = 0;
		mi.GetOuterMargins(
			&nMarginLeft,
			NULL,
			&nMarginRight,
			NULL
			);
		rcItem.DeflateRect(
			nMarginLeft,
			0,
			nMarginRight,
			0
			);
	} // if( bIncludeIndents )

INT iter = 0;
	for(	int i = 0;
			i < nIndex
				&& iter != m_items_all.GetSize();
			i++, iter++
			)
	{
		MENUITEMDATA & mi = ItemGetInfo( iter );
		if( ! mi.IsDisplayed() )
			continue;
		const RECT & rcBase = mi.GetBaseRect();
		y += (rcBase.bottom - rcBase.top);
		INT nMarginTop = 0, nMarginBottom = 0;
		mi.GetOuterMargins(
			NULL,
			&nMarginTop,
			NULL,
			&nMarginBottom
			);
		if( mi.GetIndex() == ItemGetCount() - 1 )
			nMarginBottom--;
		y += (nMarginTop + nMarginBottom);
	}
	ASSERT( iter != m_items_all.GetSize() );
	rcItem.OffsetRect( 0, y + 1 );
	mi = ItemGetInfo( iter );
	ASSERT( mi.IsDisplayed() );

const RECT & rcBase = mi.GetBaseRect();
	rcItem.bottom =
		rcItem.top + rcBase.bottom - rcBase.top;
	
INT nMarginTop = 0, nMarginBottom = 0;
	mi.GetOuterMargins(
		NULL,
		&nMarginTop,
		NULL,
		&nMarginBottom
		);
	if( mi.GetIndex() == ItemGetCount() - 1 )
		nMarginBottom--;

	rcItem.bottom += (nMarginTop + nMarginBottom);

	if( bIncludeIndents )
	{
		rcItem.DeflateRect(
			0,
			nMarginTop,
			0,
			nMarginBottom
			);
	} // if( bIncludeIndents )
	
	// FIX for combined exclude area
	rcItem.OffsetRect( rcClient.TopLeft() );
	if( bTearOff )
		rcItem.OffsetRect( 0, nTearOffCaptionHeight );
	rectItem = rcItem;

	mi.CacheRect( rectItem, bIncludeIndents );
}

void CExtPopupMenuWnd::_GetItemRectPalette(
	int nIndex,
	RECT & rectItem,
	bool bIncludeIndents // = true
	)
{
	ASSERT_VALID( this );
int nCountOfItems = (int)m_items_all.GetSize();
	if( nCountOfItems == 0 )
	{
		::SetRectEmpty( &rectItem );
		return;
	}
	if( nIndex < 0 || nIndex >= nCountOfItems )
	{
		CExtPopupMenuWnd::_GetItemRect(
			nIndex,
			rectItem,
			bIncludeIndents
			);
		return;
	}
MENUITEMDATA & mi = ItemGetInfo( nIndex );
	if( ! mi.IsDisplayed() )
	{
		::SetRectEmpty( &rectItem );
		return;
	}

CRect rcCache = mi.GetCachedRect( bIncludeIndents );
	if( ! rcCache.IsRectEmpty() )
	{
		::CopyRect( &rectItem, rcCache );
		return;
	}

CRect rcClient;
	_GetClientRect( &rcClient );
	rcClient.left += m_nLeftAreaWidth;
CRect rcMB = OnQueryMenuBorderMetrics();
int y = m_nDyScrollOffset + rcMB.top;

CRect rcItem;
	rcItem.SetRectEmpty();
	if( mi.IsSeparator() )
	{
		rcItem.left = rcItem.right = rcMB.left + 2; 
	}
	else if( mi.IsToolButton() )
	{
		rcItem.left = rcMB.left + 2; 
		rcItem.right = rcItem.left + mi.GetMeasuredWidth();
	}
	else
	{
		rcItem.left = rcMB.left;
		rcItem.right = m_sizeFullItems.cx + rcMB.left;
	}
int x = rcItem.left;
int xRow = x, dyRow = 0;
int iter = 0, i = 0, nRowStart = 0;
	for( ; i < nIndex && iter != nCountOfItems; i++, iter++ )
	{
		MENUITEMDATA & mi = ItemGetInfo( iter );
		if( ! mi.IsDisplayed() )
			continue;
		bool bWrap = _IsPaletteItemWrap( iter );
		const RECT & rcBase = mi.GetBaseRect();
		if( mi.IsSeparator() )
			bWrap = true;
		int nWidth = rcBase.right - rcBase.left;
		int nHeight = rcBase.bottom - rcBase.top;
		dyRow = max( dyRow, nHeight );
		if( bWrap )
		{
			xRow = x;
			y += dyRow;
			dyRow = 0;
			if( mi.IsSeparator() )
				y +=
					PmBridge_GetPM()->GetSeparatorHeight()
					+ __EXT_MENU_GAP
					;
			if( i < nIndex )
				nRowStart = i+1;
		}
		else
		{
			xRow += nWidth;
			if( mi.IsInplaceEdit() )
				xRow += (iter == 0) ? __EXT_MENU_GAP : (__EXT_MENU_GAP*2);
//			else
//				xRow += (nRowStart == i) ? 0 : __EXT_MENU_GAP;
			xRow += 2;
		}
	} // for( ; i < nIndex && iter != nCountOfItems; i++, iter++ )

	ASSERT( iter != m_items_all.GetSize() );
bool bTearOff = _IsTearOff();
int nTearOffCaptionHeight = bTearOff ? _GetTearOffCaptionHeight() : 0;
	mi = ItemGetInfo( iter );
	ASSERT( mi.IsDisplayed() );
	if( mi.IsSeparator() )
	{
		rcItem.OffsetRect( 0, y + 2 );
		rcItem.OffsetRect( rcClient.TopLeft() );
		rcItem.right =
			rcClient.right
			- rcItem.left + rcClient.left 
			;
		rcItem.top += __EXT_MENU_GAP / 2;
		rcItem.bottom =
			rcItem.top
			+ PmBridge_GetPM()->GetSeparatorHeight()
			+ __EXT_MENU_GAP;
			;
		if( bTearOff )
			rcItem.OffsetRect( 0, nTearOffCaptionHeight );
		rectItem = rcItem;
		return;
	}
	rcItem.OffsetRect( xRow - rcItem.left, 0 );
	if( iter != 0 && mi.IsInplaceEdit() )
		rcItem.OffsetRect( __EXT_MENU_GAP, 0 );
	rcItem.OffsetRect( 0, y + 2 );

const RECT & rcBase = mi.GetBaseRect();
	rcItem.bottom =
		rcItem.top + rcBase.bottom - rcBase.top;

	dyRow = 0;
	for( iter = nRowStart; iter != nCountOfItems; iter++ )
	{
		MENUITEMDATA & mi = ItemGetInfo( iter );
		if( ! mi.IsDisplayed() )
			continue;
		bool bWrap = _IsPaletteItemWrap( iter );
		const RECT & rcBase = mi.GetBaseRect();
		if( mi.IsSeparator() )
			bWrap = true;
		int nHeight = rcBase.bottom - rcBase.top;
		dyRow = max( dyRow, nHeight );
		if( bWrap )
			break;
	} // for( iter = nRowStart; iter != nCountOfItems; iter++ )
	if( dyRow > 0 )
	{
		ASSERT( iter >= nIndex );
		int nHeight = rcItem.Height();
		ASSERT( nHeight <= dyRow );
		rcItem.OffsetRect(
			0,
			( dyRow - nHeight ) / 2
			);
	}

	rcItem.OffsetRect( rcClient.TopLeft() );
	if( bTearOff )
		rcItem.OffsetRect( 0, nTearOffCaptionHeight );
	rectItem = rcItem;

	mi.CacheRect( rectItem, bIncludeIndents );
}

void CExtPopupMenuWnd::_GetVisibleItems(
	HDC hDcTest, // may be NULL
	visible_items_t & v
	)
{
	ASSERT_VALID( this );
	if( _IsPalette() )
	{
		_GetVisibleItemsPalette( hDcTest, v );
		return;
	}

	v.RemoveAll();
INT iter = 0;
	for(	int nIndex = 0;
			iter != m_items_all.GetSize();
			iter++, nIndex++
			)
	{ // find visible items
		MENUITEMDATA & mi = ItemGetInfo( iter );
		if( !mi.IsDisplayed() )
			continue;
		CRect rcItem;
		_GetItemRect( nIndex, rcItem, true );
		CRect rcItemWithIndents;
		_GetItemRect( nIndex, rcItemWithIndents, false );
		if(		rcItem.IsRectEmpty() 
			||	rcItemWithIndents.IsRectEmpty()
			)
			continue;
		if(		hDcTest != NULL
			&&	(	!::RectVisible( hDcTest, &rcItem ) 
				||	!::RectVisible( hDcTest, &rcItemWithIndents ) 
				)
			)
			continue;
		VisibleItemDefinition_t vi;
		vi.m_nIndex = nIndex;
		vi.m_nHelperCmdID = mi.GetCmdID();
		vi.m_rcItem = rcItem;
		vi.m_rcItemWithIndents = rcItemWithIndents;
		if( mi.IsSeparator() )
		{
			vi.m_bHelperIsSeparator = true;
			v.Add( vi );
			continue;
		}
		if( mi.IsPopup() )
		{
			vi.m_bHelperIsPopup = true;
			CExtCmdItem * pCmdItem = mi.GetCmd();
			if( pCmdItem != NULL )
			{
				vi.m_bRarelyUsed =
					(	(!_FindCustomizeMode())
					&&	pCmdItem->StateIsRarelyUsed()
					) ? true : false;
			} // if( pCmdItem != NULL )
			else
				vi.m_bRarelyUsed =
						(!_FindCustomizeMode())
					&&	mi.IsAllItemsRarelyUsed()
					&&	(!mi.IsForceDisplayed())
					;
			v.Add( vi );
			continue;
		}
		if( mi.IsExtraMark() )
			vi.m_bRarelyUsed = false;
		else
		{
			CExtCmdItem * pCmdItem = mi.GetCmd();
			if( pCmdItem != NULL )
				vi.m_bRarelyUsed =
					(	(!_FindCustomizeMode())
					&&	pCmdItem->StateIsRarelyUsed()
					) ? true : false;
			else
				vi.m_bRarelyUsed = false;
		} // else from if( mi.IsExtraMark() )
		v.Add( vi );
	} // find visible items
INT vis_iter = 0;
	for( ; vis_iter < v.GetSize(); ++vis_iter )
	{ // setup prev/next rarely used flags
		VisibleItemDefinition_t & vi = v[ vis_iter ];
		INT iter_prev = vis_iter;
		INT iter_next = vis_iter;
		if( vis_iter != 0 )
		{
			--iter_prev;
			if( v[iter_prev].m_bRarelyUsed )
				vi.m_bRarelyUsedPrev = true;
		} // if( vis_iter != v.begin() )
		++iter_next;
		if( iter_next != v.GetSize() )
		{
			if( v[iter_next].m_bRarelyUsed )
				vi.m_bRarelyUsedNext = true;
		} // if( iter_next != v.end() )

		if( vi.m_bHelperIsSeparator )
		{
			if(		vi.m_bRarelyUsedPrev
				&&	vi.m_bRarelyUsedNext
				&&	(!_FindCustomizeMode())
				)
			{
				vi.m_bRarelyUsed = true;
				ASSERT( vis_iter != 0 );
				v[iter_prev].m_bRarelyUsedNext = true;
				ASSERT( iter_next != 0 );
				v[iter_next].m_bRarelyUsedPrev = true;
			}
		}
	} // setup prev/next rarely used flags
}

void CExtPopupMenuWnd::_GetVisibleItemsPalette(
	HDC hDcTest, // may be NULL
	CExtPopupMenuWnd::visible_items_t & v
	)
{
	ASSERT_VALID( this );
	v.RemoveAll();
int nItemIndex = 0, nItemCount = (int)m_items_all.GetSize();
	for( ; nItemIndex < nItemCount; nItemIndex++ )
	{
		MENUITEMDATA & mi = ItemGetInfo( nItemIndex );
		if( ! mi.IsDisplayed() )
			mi.SetDisplayed( true );
		CRect rcItem;
		_GetItemRect( nItemIndex, rcItem, true );
		if(		hDcTest != NULL
			&&	(! ::RectVisible( hDcTest, &rcItem )  )
			)
			continue;
		VisibleItemDefinition_t vi;
		vi.m_nIndex = nItemIndex;
		vi.m_nHelperCmdID = mi.GetCmdID();
		vi.m_rcItem = rcItem;
		vi.m_rcItemWithIndents = rcItem;
		if( mi.IsSeparator() )
			vi.m_bHelperIsSeparator = true;
		else if( mi.IsPopup() )
			vi.m_bHelperIsPopup = true;
		v.Add( vi );
	} // for( ; nItemIndex < nItemCount; nItemIndex++ )
}

bool CExtPopupMenuWnd::_IsPaletteItemWrap(
	int nIndex
	) const
{
	ASSERT_VALID( this );
	if( ! _IsPalette() )
		return false;
	if( nIndex < 0 )
		return false;
int nCountOfItems = ItemGetCount();
	if( nIndex >= nCountOfItems  )
		return false;
	if( nIndex < (nCountOfItems-1) )
	{
		const MENUITEMDATA & miNext = ItemGetInfo( nIndex + 1 );
		if( miNext.IsSeparator() )
			return true;
	}
const MENUITEMDATA & mi = ItemGetInfo( nIndex );
	if( mi.IsToolWrap() )
		return true;
	return false;
}

void CExtPopupMenuWnd::stat_paint_combined_menu_toolbtn(
	LPVOID pCookie,
	CDC & dc,
	const CWnd & refWndMenu,
	const CRect & rcExcludeArea, // in screen coords
	int eCombineAlign // CExtPopupMenuWnd::e_combine_align_t values
	)
{
	eCombineAlign;
INT nIndex = (INT)(__EXT_MFC_INT_PTR)pCookie;
	ASSERT( refWndMenu.GetSafeHwnd() != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );

	if( rcExcludeArea.IsRectEmpty() )
		return;

	ASSERT_KINDOF( CExtPopupMenuWnd, (&refWndMenu) );
CExtPopupMenuWnd * pPopup =
		(CExtPopupMenuWnd *)(&refWndMenu);
	ASSERT_VALID( pPopup );
	ASSERT( ! pPopup->m_bTopLevel );
CExtPopupMenuWnd * pPopup2 = pPopup->m_pWndParentMenu;
	ASSERT_VALID( pPopup2 );
	ASSERT( 0 <= nIndex && nIndex < pPopup2->ItemGetCount() );
CExtPopupMenuWnd::MENUITEMDATA & mi = pPopup2->ItemGetInfo(nIndex);

CRect rcItem;
	pPopup2->_GetItemRect( nIndex, rcItem );
CRect rcClientExcludeArea( rcExcludeArea );
	pPopup->ScreenToClient( &rcClientExcludeArea );
	rcItem.OffsetRect( - rcItem.TopLeft() );
	rcItem.OffsetRect( rcClientExcludeArea.TopLeft() );
CRect rcTargetClient;
	pPopup->_GetClientRect( &rcTargetClient );
	dc.ExcludeClipRect( &rcTargetClient );

bool bOwnerDrawMode = ( pPopup->GetSite()->m_dwTrackFlags&TPMX_OWNERDRAW_FIXED ) ? true : false;
CExtPopupMenuWnd::DRAWITEMDATA _DrawItemData(
		&dc,
		&rcItem,
		&rcItem,
		&mi,
		pPopup,
		bOwnerDrawMode,
		mi.IsPopup(),
		false, // g_bMenuHighlightRarely && vi.m_bRarelyUsed,
		false, // g_bMenuHighlightRarely && vi.m_bRarelyUsedPrev,
		false  // g_bMenuHighlightRarely && vi.m_bRarelyUsedNext
		);

bool bPainted =
		(_DrawItemData.m_bMenuOwnerDrawFixed)
			? _DrawItemData.DoOwnerDrawPainting()
			: false
			;

bool bForceNoIcon = false, bForceNoText = false;
	if( (pPopup2->TrackFlagsGet()&TPMX_PALETTE) != 0 )
	{
		CExtCmdIcon * pIcon = mi.GetIconPtr();
		if( pIcon != NULL && pIcon->IsEmpty() )
			pIcon = NULL;
		if( mi.IsInplaceEdit() )
			bForceNoIcon = bForceNoText = true;
		else if( pIcon == NULL || ( pIcon != NULL && (! mi.IsIconMode() ) ) )
		{
			//bForceNoIcon = true;
		}
		else
			bForceNoText = true;
	}
	if( ! bPainted )
		_DrawItemData.PaintDefault(
			bForceNoIcon,
			bForceNoText
			);

	dc.SelectClipRgn( NULL );
}

void CExtPopupMenuWnd::DRAWITEMDATA::PaintDefault(
	bool bForceNoIcon, // = false
	bool bForceNoText, // = false
	bool bForceNoCheck, // = false
	bool bForceEnabled, // = false
	bool bForceUnselected // = false
	)
{
	ASSERT( m_pDC != NULL );
	ASSERT( m_pDC->GetSafeHdc() != NULL );
	ASSERT( m_pRectItem != NULL );
	ASSERT( m_pRectItemWithIndents != NULL );
	ASSERT( m_pItemData != NULL );
	ASSERT( m_pPopup != NULL );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite = NULL;
bool bCustomizeMode = false;
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
bool bResizingNode = false;
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	if( m_pPopup->_FindCustomizeMode() )
	{
		pSite = m_pPopup->_FindCustomizeSite();
		ASSERT( pSite != NULL );
		bCustomizeMode = true;
		bForceNoCheck = bForceEnabled = true;
		bForceUnselected = m_pItemData->IsPopup() ? false : true;
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
		CExtCustomizeCmdTreeNode * pNode = ((MENUITEMDATA *)m_pItemData)->GetCmdNode();
		if( pNode != NULL )
		{
			ASSERT_VALID( pNode );
			if( pNode == pSite->GetResizingNode() )
				bResizingNode = true;
		}
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if(		m_pItemData->IsToolButton()
		&&	(! m_pItemData->IsInplaceEdit() )
		)
	{
		bool bSelDraw =
			bForceUnselected 
				? false 
				: m_pItemData->IsSelected();
		bool bTransparent = ! m_pItemData->IsSelected();

		bool bIsPopupAndPopupVisible = false;
		if( m_pItemData->IsPopup() )
		{
			const CExtPopupMenuWnd * pChildPopup = m_pItemData->GetPopup();
			if(		pChildPopup->GetSafeHwnd() != NULL
				&&	(pChildPopup->GetStyle() & WS_VISIBLE) != 0
				)
				bIsPopupAndPopupVisible = true;
		}

		if(		bSelDraw 
			&&	bIsPopupAndPopupVisible
			)
		{
			bSelDraw = false;
			bTransparent = true;
		}

		bool bPushed =
				bSelDraw
			&&	m_pItemData->IsPopup();
		if(		(! bForceNoCheck)
			&&	(	m_pItemData->GetCheck()
				||	m_pItemData->GetRadio()
				)
			)
		{
			bPushed = true;
			bTransparent = false;
		}

		bool bSeparatedDropDown = false;
		bool bPushedDropDown = false;
		bool bHoverDropDown = false;

		if(		bSelDraw
			&&	m_pItemData->IsPopup() 
			&&	(!bIsPopupAndPopupVisible)
			)
		{
			bPushedDropDown = false;
			bHoverDropDown = true;
		}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		const CExtCustomizeCmdTreeNode * pNode =
			m_pItemData->GetCmdNode();
		if( pNode != NULL )
		{
			ASSERT_VALID( pNode );
			bSeparatedDropDown =
				(pNode->GetFlags() & __ECTN_TBB_SEPARATED_DROPDOWN)
					? true : false;
		} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

		bool bCombiningEnabled = 
			m_pPopup->PmBridge_GetPM()->IsMenuMustCombineExcludeArea( 
				NULL // should be NULL
				);
		if(		(!bCombiningEnabled)
			&&	bIsPopupAndPopupVisible
			)
		{
			bPushed = true;
			bTransparent = false;
			if( bSeparatedDropDown )
			{
				bHoverDropDown = true;
				bPushedDropDown = true;
			}
		}
		CExtCmdIcon _iconLarge;
		CExtCmdIcon * pIcon = ( bForceNoIcon ) ? NULL : ((MENUITEMDATA *)m_pItemData)->GetIconPtr();
		if( g_bMenuLargeIcons && pIcon != NULL )
		{
			CSize _sizeIcon = pIcon->GetSize();
			_sizeIcon.cx *= 2;
			_sizeIcon.cy *= 2;
			if(	_iconLarge.CreateScaledCopy(
					*pIcon,
					_sizeIcon
					)
				)
				pIcon = &_iconLarge;
#ifdef _DEBUG
			else
			{
				ASSERT( FALSE );
			}
#endif // _DEBUG
		} // if( g_bMenuLargeIcons && pIcon != NULL )
		bool bDrawBorder =
				m_pItemData->IsSelected()
			||	bPushed
			||	bSelDraw
			;
		CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
			m_pPopup,
			true,
			*m_pRectItem,
			bForceNoText ? _T("") : m_pItemData->GetText(),
			pIcon,
			true,
			bSelDraw, // hover
			bPushed, // pushed
			false,
			bForceEnabled
				|| m_pItemData->IsEnabled()
				|| m_pItemData->IsForceEnabled()
				|| m_pItemData->IsEnabledAppearance()
				,
			bDrawBorder, // border
			false,
			false,
			0,
			m_pItemData->IsBold()
				? ((HFONT)(m_pPopup->PmBridge_GetPM()->m_FontBold.GetSafeHandle()))
				: NULL,
			m_pItemData->IsPopup() ? true : false,
			0,
			bTransparent // transparent
			);

		_ppbd.m_rcBorderSizes = CRect( 1, 1, 1, 1 );
		_ppbd.m_nIconAlignment = ( CExtPaintManager::__ALIGN_HORIZ_LEFT | CExtPaintManager::__ALIGN_VERT_CENTER );
		_ppbd.m_nTextAlignment = ( CExtPaintManager::__ALIGN_HORIZ_CENTER | CExtPaintManager::__ALIGN_VERT_CENTER );
		_ppbd.m_rcIconMargins = CRect( 2, 1, 1, 1 );
		_ppbd.m_rcTextMargins = CRect( 1, 1, 1, 1 );

		_ppbd.m_bSeparatedDropDown = bSeparatedDropDown;
		_ppbd.m_bPushedDropDown = bPushedDropDown;
		_ppbd.m_bHoverDropDown = bHoverDropDown;
		
		m_pPopup->PmBridge_GetPM()->PaintPushButton( *m_pDC, _ppbd );
	} // if( m_pItemData->IsToolButton() ...
	else
	{
		CExtCmdIcon * pIcon =
			( bForceNoIcon )
				? NULL
				: ((MENUITEMDATA *)m_pItemData)->GetIconPtr();
		if( pIcon != NULL && pIcon->IsEmpty() )
			pIcon = NULL;
		CExtCmdIcon _iconLarge;
		if( g_bMenuLargeIcons && pIcon != NULL )
		{
			CSize _sizeIcon = pIcon->GetSize();
			_sizeIcon.cx *= 2;
			_sizeIcon.cy *= 2;
			if(	_iconLarge.CreateScaledCopy(
					*pIcon,
					_sizeIcon
					)
				)
				pIcon = &_iconLarge;
#ifdef _DEBUG
			else
			{
				ASSERT( FALSE );
			}
#endif // _DEBUG
		} // if( g_bMenuLargeIcons && pIcon != NULL )
		CExtPaintManager::PAINTMENUITEMDATA _pmid(
			m_pPopup,
			CRect( *m_pRectItem ),
			CRect( *m_pRectItemWithIndents ),
			m_pItemData->GetIconAreaWidth(),
			bForceNoText ? _T("") : m_pItemData->GetText(),
			( bForceNoText || ( m_pPopup->TrackFlagsGet() & TPMX_HIDE_KEYBOARD_ACCELERATORS ) != 0 )
				? _T("") : m_pItemData->GetAccelText(),
			pIcon,
			m_bPopupItem,
			bForceUnselected ? false : m_pItemData->IsSelected(),
			bForceNoCheck ? false : m_pItemData->GetCheck(),
			bForceNoCheck ? false : m_pItemData->GetRadio(),
			m_pItemData->IsIndeterminated(),
			bForceEnabled
				|| m_pItemData->IsEnabled()
				|| m_pItemData->IsForceEnabled()
				|| m_pItemData->IsEnabledAppearance()
				,
			m_bRarelyUsed,
			m_bRarelyUsedPrevious,
			m_bRarelyUsedNext,
			m_pItemData->GetCmdID(),
			m_pItemData->IsBold()
				? ((HFONT)(m_pPopup->PmBridge_GetPM()->m_FontBold.GetSafeHandle()))
				: NULL
			);
		_pmid.m_bBigAccent = m_pItemData->IsBigAccent();
		_pmid.m_bForceNoLeftGradient =
			(	m_pItemData->IsForceNoLeftGradient()
			||	m_pPopup->IsNoHighlightIconArea()
			) ? true : false;
		if( ((MENUITEMDATA *)m_pItemData)->IsInplaceEdit() )
		{
			_pmid.m_rcInPlaceWnd =
				m_pItemData->AdjustInplaceEditRect(
					*m_pRectItem,
					m_pPopup->OnQueryLayoutRTL()
					);
			_pmid.m_strInPlaceText =
				m_pItemData->GetInplaceEditText();
		} // if( ((MENUITEMDATA *)m_pItemData)->IsInplaceEdit() )
		if(		m_pItemData->IsExtraMark()
			&&	(! m_pPopup->_FindHelpMode() )
			)
		{
			_pmid.m_bXtraMark = true;
			_pmid.m_bXtraChecked = m_pItemData->IsExtraChecked();
		} // if( m_pItemData->IsExtraMark() ...
		ASSERT( !_pmid.m_bHelperPopupDropped );
		if(		m_pItemData->IsPopup()
			&&	m_pItemData->GetPopup()->GetSafeHwnd() != NULL
			&&	m_pItemData->GetPopup()->IsWindowVisible()
			)
			_pmid.m_bHelperPopupDropped = true;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
		if( bResizingNode )
			_pmid.m_bSelected = true;
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
		const CExtCustomizeCmdTreeNode * pNode =
			m_pItemData->GetCmdNode();
		if( pNode != NULL )
		{
			ASSERT_VALID( pNode );
			_pmid.m_bSeparatedDropDown =
				(pNode->GetFlags() & __ECTN_TBB_SEPARATED_DROPDOWN)
					? true : false;
		} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		_pmid.m_sExtendedText = m_pItemData->GetExtendedText();
		m_pPopup->PmBridge_GetPM()->PaintMenuItem( *m_pDC, _pmid );
	} // else from if( m_pItemData->IsToolButton() )
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( m_pPopup->m_nHelperDropIndexBefore >= 0 )
	{
		int nCount = m_pPopup->ItemGetCount();
		ASSERT( m_pPopup->m_nHelperDropIndexBefore <= nCount );
		int nAlignAt = 0;
		int nItemIndex = m_pItemData->GetIndex();
		if( nItemIndex == m_pPopup->m_nHelperDropIndexBefore )
			nAlignAt = -1;
		else if(	nItemIndex == ( nCount - 1 )
				&&	m_pPopup->m_nHelperDropIndexBefore == nCount
			)
			nAlignAt = 1;
		else if(	( nItemIndex + 1 ) == m_pPopup->m_nHelperDropIndexBefore
				&&	m_pPopup->ItemGetInfo( nItemIndex + 1 ).IsSeparator()
				)
		{
			nItemIndex;
			nAlignAt = 1;
		}
		if( nAlignAt != 0 )
		{ // if paint drop divider
			CSize _sizeDDM = m_pPopup->PmBridge_GetPM()->GetDropDividerMerics();
			CRect rcItem;
			ASSERT( !m_pPopup->ItemGetInfo(nItemIndex).IsSeparator() );
			m_pPopup->_GetItemRect( nItemIndex, rcItem );
			int nY =
				( nAlignAt > 0 )
					? rcItem.bottom - _sizeDDM.cy/2
					: rcItem.top + _sizeDDM.cy/2
					;
			rcItem.top = rcItem.bottom = nY;
			m_pPopup->PmBridge_GetPM()->PaintDropDivider(
				*m_pDC,
				rcItem,
				true,
				false
				);
		} // if( nAlignAt != 0 )
	} // if( m_pPopup->m_nHelperDropIndexBefore >= 0 )
	if( bCustomizeMode )
	{
		ASSERT( pSite != NULL );
		if(
				(
					pSite != NULL
				&&
							(	pSite->CustomizedNodeGet() != NULL
							&&	pSite->CustomizedNodeGet()  == ((MENUITEMDATA*)m_pItemData)->GetCmdNode()
							)
				)
			||
				(	m_pPopup->m_pDragSrcNode != NULL
				&&	((MENUITEMDATA*)m_pItemData)->GetCmdNode() == m_pPopup->m_pDragSrcNode
				)
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
			||	bResizingNode
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
			)
		{
			CRect rcItem = m_pRectItem;
			m_pPopup->PmBridge_GetPM()->PaintDragSourceRect(
				*m_pDC,
				rcItem
				);
		}
	} // if( bCustomizeMode )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
}

bool CExtPopupMenuWnd::DRAWITEMDATA::DoOwnerDrawPainting()
{
	ASSERT( m_pDC != NULL );
	ASSERT( m_pDC->GetSafeHdc() != NULL );
	ASSERT( m_pRectItem != NULL );
	ASSERT( m_pItemData != NULL );
	ASSERT( m_pPopup != NULL );
	HWND hWndNotify =
		m_pPopup->m_hWndCmdReceiver;
	ASSERT( hWndNotify != NULL );
	ASSERT( ::IsWindow(hWndNotify) );
	LRESULT lResult =
		::SendMessage(
			hWndNotify,
			CExtPopupMenuWnd::g_nMsgPopupDrawItem,
			0,
			LPARAM( *this )
			);
	return lResult ? true : false;
}

void CExtPopupMenuWnd::DRAWBACKGROUNDDATA::Notify()
{
	ASSERT( m_DC.GetSafeHdc() != NULL );
	ASSERT( m_pPopup != NULL );
	ASSERT( m_pPopup != NULL );
HWND hWndNotify =
		m_pPopup->GetCmdReceiverHWND();
	ASSERT( hWndNotify != NULL );
	ASSERT( ::IsWindow(hWndNotify) );
	::SendMessage(
		hWndNotify,
		CExtPopupMenuWnd::g_nMsgPopupDrawBackground,
		0,
		LPARAM( *this )
		);
}

bool CExtPopupMenuWnd::DRAWLEFTAREADATA::DoOwnerDrawPainting()
{
	ASSERT( m_pDC != NULL );
	ASSERT( m_pDC->GetSafeHdc() != NULL );
	ASSERT( m_pRectLeftArea != NULL );
	ASSERT( m_pPopup != NULL );
	HWND hWndNotify =
		m_pPopup->m_hWndCmdReceiver;
	ASSERT( hWndNotify != NULL );
	ASSERT( ::IsWindow(hWndNotify) );
	LRESULT lResult =
		::SendMessage(
			hWndNotify,
			CExtPopupMenuWnd::g_nMsgPopupDrawLeftArea,
			0,
			LPARAM( *this )
			);
	return lResult ? true : false;
}

void CExtPopupMenuWnd::_DoPaint(
	CDC & dcPaint,
	bool bUseBackBuffer // = true
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dcPaint) );
	ASSERT( dcPaint.GetSafeHdc() != NULL );
	if( m_hWnd != NULL )
	{
		ASSERT( ::IsWindow(m_hWnd) );
		CExtPaintManager::stat_ExcludeChildAreas( dcPaint.GetSafeHdc(), m_hWnd );
	} // if( m_hWnd != NULL )
	ASSERT( m_hWndCmdReceiver != NULL );
	ASSERT( ::IsWindow(m_hWndCmdReceiver) );
CRect rcMB = OnQueryMenuBorderMetrics();
int nMenuShadowSize = OnQueryMenuShadowSize();
CRect rcRealClient, rcClient;
	GetClientRect( &rcRealClient );
	_GetClientRect( &rcClient );
CPalette * pOldPalette = NULL, * pOldPalette2 = NULL;
	if( ::GetDeviceCaps(dcPaint.m_hDC,RASTERCAPS) & RC_PALETTE )
    {
        pOldPalette = dcPaint.SelectPalette( &PmBridge_GetPM()->m_PaletteWide, FALSE );
        dcPaint.RealizePalette();
    }
	PmBridge_GetPM()->AdjustMenuDC( this, dcPaint, false );

CSize _sizeCaption = MenuCaptionMeasure();
CExtMemoryDC mdc;
	if( bUseBackBuffer )
	{
		CRect rcMemDC = rcClient;
		if( _sizeCaption.cx > 0 && _sizeCaption.cy > 0 )
			rcMemDC.top -= _sizeCaption.cy;
		mdc.__InitMemoryDC( &dcPaint, &rcMemDC );
		if( pOldPalette != NULL )
		{
			pOldPalette2 = mdc.SelectPalette( &PmBridge_GetPM()->m_PaletteWide, FALSE );
			mdc.RealizePalette();
		}
		PmBridge_GetPM()->AdjustMenuDC( this, dcPaint, true );
	}
CDC & dcDummyRef = mdc;
CDC & dc = bUseBackBuffer ? dcDummyRef : dcPaint;
CFont * pOldFont = (CFont *) dc.SelectObject( &PmBridge_GetPM()->m_FontNormal );
COLORREF clrFill = COLORREF(-1L);

DRAWBACKGROUNDDATA _dbkgnd( dc, rcClient, this );
	_dbkgnd.Notify();
	if( ! _dbkgnd.m_bCustomBackgroundDrawn )
	{
		if(		_IsPalette()
			&&	( TrackFlagsGet() & TPMX_PALETTE_TB_BKGND ) != NULL
			)
		{
			CRgn rgnClient;
			if( rgnClient.CreateRectRgnIndirect( &rcClient ) )
				dc.SelectClipRgn( &rgnClient );
			if( ! PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
				clrFill = PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT );
			dc.SelectClipRgn( NULL );
		}
		else
			clrFill = PmBridge_GetPM()->GetMenuFrameFillColor( this );
		if( clrFill != COLORREF(-1L) )
			dc.FillSolidRect( &rcClient, clrFill );
	} // if( ! _dbkgnd.m_bCustomBackgroundDrawn )

	if( ! _dbkgnd.m_bCustomBorderDrawn )
		PmBridge_GetPM()->PaintMenuBorder( dc, rcClient, this );

	if( _sizeCaption.cx > 0 && _sizeCaption.cy > 0 )
	{
		CRect rcMenuCaption = rcClient;
		rcMenuCaption.DeflateRect( &rcMB );
		rcMenuCaption.bottom = rcMenuCaption.top;
		rcMenuCaption.top -= _sizeCaption.cy;
		MenuCaptionPaint( dc, rcMenuCaption );
	} // if( _sizeCaption.cx > 0 && _sizeCaption.cy > 0 )

	if( (TrackFlagsGet()&TPMX_RIBBON_RESIZING) != 0 )
	{
		CRect rcMB = PmBridge_GetPM()->GetMenuBorderMetrics( (CWnd*)this );
		ASSERT( rcMB.left >= 0 );
		ASSERT( rcMB.top >= 0 );
		ASSERT( rcMB.right >= 0 );
		ASSERT( rcMB.bottom >= 0 );
		CRect rcResizingArea = rcClient;
		rcResizingArea.left += rcMB.left;
		rcResizingArea.right -= rcMB.right;
		rcResizingArea.bottom -= rcMB.bottom;
		INT nMarginHeight = 0, nResizingCornerPartWidth = 0;
		PmBridge_GetPM()->MenuBottomResizingMarginGetInfo( nMarginHeight, nResizingCornerPartWidth, this );
		rcResizingArea.top = rcResizingArea.bottom - nMarginHeight;
		PmBridge_GetPM()->MenuBottomResizingMarginPaint( dc, rcResizingArea, this );
	}

CRect rcExpand;
	if( m_bExpandAvailable )
	{
		_GetItemRect( IDX_EXPAND, rcExpand, false );
		dc.ExcludeClipRect( rcExpand );
		CRect rcExcludePart( rcExpand );
		rcExcludePart.left = rcRealClient.left;
		rcExcludePart.right = rcRealClient.right;
		dc.ExcludeClipRect( rcExcludePart );
	}
CRect rcTearOff(0,0,0,0);
bool bTearOff = _IsTearOff();
	if( bTearOff )
	{
		_GetItemRect( IDX_TEAROFF, rcTearOff, false );
		dc.ExcludeClipRect( rcTearOff );
		CRect rcExcludePart( rcTearOff );
		rcExcludePart.left = rcRealClient.left;
		rcExcludePart.right = rcRealClient.right;
		dc.ExcludeClipRect( rcExcludePart );
	}

CRect rcScrollTop;
CRect rcScrollBottom;
int nMaxScrollPos = 0;
	if( m_bScrollingAvailable )
	{
		if( m_nDyScrollOffset != 0 )
		{
			_GetItemRect( IDX_SCROLL_TOP, rcScrollTop );
			dc.ExcludeClipRect( rcScrollTop );
			CRect rcExcludePart( rcScrollTop );
			rcExcludePart.InflateRect( 0, 1 );
			rcExcludePart.left = rcRealClient.left;
			rcExcludePart.right = rcRealClient.right;
			dc.ExcludeClipRect( rcExcludePart );
		}
		nMaxScrollPos = _GetMaxScrollPos();
		if( m_nDyScrollOffset != nMaxScrollPos )
		{
			_GetItemRect( IDX_SCROLL_BOTTOM, rcScrollBottom );
			dc.ExcludeClipRect( rcScrollBottom );
			CRect rcExcludePart( rcScrollBottom );
			rcExcludePart.InflateRect( 0, 1 );
			rcExcludePart.left = rcRealClient.left;
			rcExcludePart.right = rcRealClient.right;
			dc.ExcludeClipRect( rcExcludePart );
		}
	}

	if( rcClient.top >= rcRealClient.top )
	{
		CRect rcExcludePart( rcRealClient );
		rcExcludePart.bottom = rcClient.top + rcMB.top;
		dc.ExcludeClipRect( rcExcludePart );
	}
	if( rcClient.bottom <= rcRealClient.bottom )
	{
		CRect rcExcludePart( rcRealClient );
		rcExcludePart.top = rcClient.bottom - rcMB.bottom;
		dc.ExcludeClipRect( rcExcludePart );
	}

	_DoPaintMenuItems( dc );

CPoint point( 0, 0 );
	::GetCursorPos( &point );
	ScreenToClient( &point );
	dc.SelectClipRgn( NULL );

	if( m_bExpandAvailable )
	{
		bool bHover =
			(	CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON == NULL
			&&	rcExpand.PtInRect(point)
			) ? true : false;
		bool bPushed = bHover;
		PmBridge_GetPM()->PaintMenuExpandButton( dc, rcExpand, bHover, bPushed, this );
	} // if( m_bExpandAvailable )
	if( bTearOff )
	{
		bool bHover = (rcTearOff.PtInRect(point))? true : false;
		if( bHover && _FindCustomizeMode() )
			bHover = false;
		bool bPushed = bHover;
		PmBridge_GetPM()->PaintMenuTearOffButton( dc, rcTearOff, bHover, bPushed, this );
	}

	if( m_bScrollingAvailable )
	{
		if( m_nDyScrollOffset != 0 )
		{
			bool bHover =
				(	rcScrollTop.PtInRect(point)
				&&	CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON == NULL
				)	? true : false;
			bool bPushed = ( m_nScrollingDirection > 0 ) ? true : false;
			PmBridge_GetPM()->PaintMenuScrollButton( dc, bHover, bPushed, true, rcScrollTop, this );
		}
		if( m_nDyScrollOffset != nMaxScrollPos )
		{
			bool bHover =
				(	rcScrollBottom.PtInRect(point)
				&&	CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON == NULL
				)	? true : false;
			bool bPushed =
				( m_nScrollingDirection < 0 ) ? true : false;
			PmBridge_GetPM()->PaintMenuScrollButton( dc, bHover, bPushed, false, rcScrollBottom, this );
		}
	} // if( m_bScrollingAvailable )
	if( m_nLeftAreaWidth > 0 )
	{
		CRect rcLeftArea( rcClient );
		rcLeftArea.right = rcLeftArea.left + m_nLeftAreaWidth;
		rcLeftArea.OffsetRect( rcMB.left, 0 );
		rcLeftArea.DeflateRect( 0, rcMB.top, 0, rcMB.bottom );
		DRAWLEFTAREADATA _DrawLeftAreaData( &dc, &rcLeftArea, this );
		_DrawLeftAreaData.DoOwnerDrawPainting();
	}
	if( m_bCombineWithEA )
	{
		ASSERT( m_eCombineAlign != __CMBA_NONE );
		dc.SelectClipRgn( NULL );
		CRect rcExcludeClient( m_rcExcludeArea );
		ScreenToClient( &rcExcludeClient );
		PmBridge_GetPM()->PaintMenuCombinedArea( dc, rcExcludeClient, rcClient, m_eCombineAlign, this );
		dc.SelectClipRgn( NULL );
		if( m_pCbPaintCombinedContent != NULL )
		{
			CExtMemoryDC dcCA( &dc, &rcExcludeClient, CExtMemoryDC::MDCOPT_TO_MEMORY|CExtMemoryDC::MDCOPT_FILL_BITS|CExtMemoryDC::MDCOPT_RTL_COMPATIBILITY );
			m_pCbPaintCombinedContent( m_pCbPaintCombinedCookie, dcCA, *this, m_rcExcludeArea, m_eCombineAlign );
		} // if( m_pCbPaintCombinedContent != NULL )
	} // if( m_bCombineWithEA )

#ifdef _DEBUG_TEAROFF_TRANSPARENCY
	if(		CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON != NULL
		&&	(!CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->m_bHelperForceFloatInvisible)
		&&	CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->m_hWndHelperFloat != NULL
		&&	::IsWindow( CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->m_hWndHelperFloat )
		)
	{
		CRect rcFloatingArea = CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->m_rcCustomFloatPos;
		if( rcFloatingArea.IsRectEmpty() )
			::GetWindowRect(
				CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->m_hWndHelperFloat,
				&rcFloatingArea
				);
		if( _IsFloatingAreaInstersectsWithMenu( rcFloatingArea, false ) )
		{
			HBITMAP hBmpSufrace =
				CExtPaintManager::stat_PrintWnd(
				CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->m_hWndHelperFloat
				);
			if( hBmpSufrace != NULL )
			{
				CBitmap bmpSurface;
				bmpSurface.Attach( hBmpSufrace );
				CDC dcSurface;
				if( dcSurface.CreateCompatibleDC(NULL) )
				{
					CBitmap * pBmpOld = dcSurface.SelectObject( &bmpSurface );
					CRect rcWndThis;
					GetWindowRect( &rcWndThis );
					dc.BitBlt(
						rcFloatingArea.left - rcWndThis.left,
						rcFloatingArea.top - rcWndThis.top,
						rcFloatingArea.Width(),
						rcFloatingArea.Height(),
						&dcSurface,
						0,
						0,
						SRCCOPY
						);
					dcSurface.SelectObject( pBmpOld );
				}
			}
		}
	}
#endif // _DEBUG_TEAROFF_TRANSPARENCY

DWORD dwTrackFlags = TrackFlagsGet();
	if( (dwTrackFlags&TPMX_RIBBON_FILE_MENU) != 0 )
	{
		if( (dwTrackFlags&TPMX_RIBBON_EXIT_BUTTON) != 0 )
		{
			dc.SelectClipRgn( NULL );
//			dc.FillSolidRect( &m_rcRibbonBtnExit, 255 );
			RIBBONFILEMENUBUTTONQUERY _rfmbq( this, TPMX_RIBBON_EXIT_BUTTON, &dc );
			_rfmbq.m_rcButton = m_rcRibbonBtnExit;
			_rfmbq.m_nMeasuredWidth = _rfmbq.m_rcButton.Width();
			_rfmbq.m_bHover = ( m_nCurIndex == INT(IDX_RIBBON_FILE_MENU_EXIT_BUTTON) ) ? true : false;
			_rfmbq.m_bPressed = ( _rfmbq.m_bHover && IsKeyPressed(VK_LBUTTON) ) ? true : false;
			_rfmbq.Notify();
		} // if( (dwTrackFlags&TPMX_RIBBON_EXIT_BUTTON) != 0 )
		if( (dwTrackFlags&TPMX_RIBBON_OPTIONS_BUTTON) != 0 )
		{
			dc.SelectClipRgn( NULL );
//			dc.FillSolidRect( &m_rcRibbonBtnOptions, 255 );
			RIBBONFILEMENUBUTTONQUERY _rfmbq( this, TPMX_RIBBON_OPTIONS_BUTTON, &dc );
			_rfmbq.m_rcButton = m_rcRibbonBtnOptions;
			_rfmbq.m_nMeasuredWidth = _rfmbq.m_rcButton.Width();
			_rfmbq.m_bHover = ( m_nCurIndex == INT(IDX_RIBBON_FILE_MENU_OPTIONS_BUTTON) ) ? true : false;
			_rfmbq.m_bPressed = ( _rfmbq.m_bHover && IsKeyPressed(VK_LBUTTON) ) ? true : false;
			_rfmbq.Notify();
		} // if( (dwTrackFlags&TPMX_RIBBON_OPTIONS_BUTTON) != 0 )
	} // if( (dwTrackFlags&TPMX_RIBBON_FILE_MENU) != 0 )

	dc.SelectObject( pOldFont );
	if( bUseBackBuffer )
	{
		ASSERT( mdc.GetSafeHdc() != NULL );
		if( pOldPalette2 != NULL )
			mdc.SelectPalette( pOldPalette2, FALSE );
		mdc.__Flush();
	}

	if(		_IsPopupWithShadows()
		&&	m_bAnimFinished
		&&	nMenuShadowSize > 0
		)
	{ // if we need to paint shadow for client area (and combined exclude area)
		dcPaint.SelectClipRgn(NULL);
		//dcPaint.ExcludeClipRect( &rcClient );
		CRect
			rcExcludeClient( 0,0,0,0 ),
			rcExcludeScreen( 0,0,0,0 ),
			rcBaseScreen( 0,0,0,0 );
		HWND hWndFloat = NULL;
		if(		CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON != NULL
			&&	(!CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->m_bHelperForceFloatInvisible)
			&&	CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->m_hWndHelperFloat != NULL
			&&	::IsWindow( CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->m_hWndHelperFloat )
			)
		{
			hWndFloat = CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->m_hWndHelperFloat;
			if( hWndFloat != NULL )
			{
				__EXT_MFC_LONG_PTR dwStyle = ::__EXT_MFC_GetWindowLong( hWndFloat, GWL_STYLE );
				if( (dwStyle&WS_VISIBLE) == 0 )
					hWndFloat = NULL;
			}
		}
		if( m_bCombineWithEA )
		{
			if(		_IsPopupWithShadows()
				&&	m_bAnimFinished
				&&	nMenuShadowSize > 0
				&&	m_bTopLevel
				)
			{ // if we need to paint shadow for combined exclude area
				rcExcludeScreen = m_rcExcludeArea;
				rcExcludeClient = rcExcludeScreen;
				ScreenToClient( &rcExcludeClient );
				dcPaint.ExcludeClipRect( &rcClient );
				rcBaseScreen = rcClient;
				ClientToScreen( &rcBaseScreen );
				VERIFY(
					m_ShadowCMBA.Paint(
						PmBridge_GetPM(),
						dcPaint,
						rcExcludeClient,
						rcExcludeScreen,
						rcBaseScreen,
						nMenuShadowSize,
						CExtWndShadow::DEF_BRIGHTNESS_MIN,
						CExtWndShadow::DEF_BRIGHTNESS_MAX,
						true,
						true,
						false,
						m_hWnd,
						hWndFloat,
						(	CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON != NULL
						&&	(! CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->m_rcCustomFloatPos.IsRectEmpty() )
						)
							? (&CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->m_rcCustomFloatPos)
							: NULL
						)
					);
				dcPaint.SelectClipRgn(NULL);
			} // if we need to paint shadow for combined exclude area

			CRect rcExcludePart( m_rcExcludeArea );
			ScreenToClient( &rcExcludePart );
			dcPaint.ExcludeClipRect( &rcExcludePart );
		} // if( m_bCombineWithEA )
		VERIFY(
			m_ShadowMain.Paint(
				PmBridge_GetPM(),
				dcPaint,
				rcClient,
				rcBaseScreen,
				rcExcludeScreen,
				nMenuShadowSize,
				CExtWndShadow::DEF_BRIGHTNESS_MIN,
				CExtWndShadow::DEF_BRIGHTNESS_MAX,
				true,
				true,
				false,
				m_hWnd,
				hWndFloat,
				(	CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON != NULL
				&&	(! CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->m_rcCustomFloatPos.IsRectEmpty() )
				)
					? (&CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON->m_rcCustomFloatPos)
					: NULL
				)
			);
	} // if we need to paint shadow for client area (and combined exclude area)

	if( m_bCombineWithEA )
	{
		ASSERT( m_eCombineAlign != __CMBA_NONE );
		dcPaint.SelectClipRgn( NULL );
		CRect rcExcludeClient( m_rcExcludeArea );
		ScreenToClient( &rcExcludeClient );
		CRect rx( rcExcludeClient );
		CExtMemoryDC dc( &dcPaint, &rx );
		PmBridge_GetPM()->PaintMenuCombinedArea(
			dc,
			rcExcludeClient,
			rcClient,
			m_eCombineAlign,
			this
			);
		if( m_pCbPaintCombinedContent != NULL )
		{
			CExtMemoryDC dcCA( &dc, &rcExcludeClient, CExtMemoryDC::MDCOPT_TO_MEMORY|CExtMemoryDC::MDCOPT_FILL_BITS|CExtMemoryDC::MDCOPT_RTL_COMPATIBILITY );
			m_pCbPaintCombinedContent(
				m_pCbPaintCombinedCookie,
				dcCA,
				*this,
				m_rcExcludeArea,
				m_eCombineAlign
				);
		} // if( m_pCbPaintCombinedContent != NULL )
	} // if( m_bCombineWithEA )

	if( pOldPalette != NULL )
		dcPaint.SelectPalette( pOldPalette, FALSE );
}

HWND CExtPopupMenuWnd::GetCmdReceiverHWND()
{
	ASSERT_VALID( this );
	ASSERT( m_hWndCmdReceiver != NULL );
	ASSERT( ::IsWindow( m_hWndCmdReceiver ) );
	return m_hWndCmdReceiver;
}

HWND CExtPopupMenuWnd::GetCmdReceiverHWND() const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtPopupMenuWnd * > ( this ) )
		-> GetCmdReceiverHWND();
}

DWORD CExtPopupMenuWnd::TrackFlagsGet() const
{
	ASSERT_VALID( this );
	if( ! m_bTopLevel )
		return (m_dwTrackFlags&(~(TPMX_RIBBON_FILE_MENU)));
	return m_dwTrackFlags;
}

void CExtPopupMenuWnd::TrackFlagsSet(
	DWORD dwTrackFlags // = 0
	)
{
	ASSERT_VALID( this );
	m_dwTrackFlags = dwTrackFlags;
}


int CExtPopupMenuWnd::_GetTearOffCaptionHeight() const
{
	ASSERT_VALID( this );
int nTearOffCaptionHeight =
		PmBridge_GetPM()->TearOff_GetCaptionHeight();
	return nTearOffCaptionHeight;
}

bool CExtPopupMenuWnd::_IsTearOff() const
{
	ASSERT_VALID( this );
DWORD dwTrackFlags = TrackFlagsGet();
	if( (dwTrackFlags&TPMX_TEAROFF) != 0 )
		return true;
//	if( _IsFadeOutAnimation() )
//		return false;
const CExtPopupMenuWnd * pTLP = GetTopLevelMenuWnd();
	if( pTLP != NULL && pTLP->m_bEndSequenceInvoked )
		return false;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
HWND hWndCmdReceiver = GetCmdReceiverHWND();
	if( hWndCmdReceiver == NULL )
		return false;
const CExtCustomizeCmdTreeNode * pNode = _CmdNodeGet();
	if( pNode == NULL )
		return false;
	ASSERT_VALID( pNode );
UINT nCmdID = pNode->GetCmdID( false );
LPCTSTR strProfileName =
		g_CmdManager->ProfileNameFromWnd( hWndCmdReceiver );
	if( strProfileName == NULL )
		return false;
CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr( strProfileName, nCmdID );
	if( pCmdItem == NULL )
		return false;
	if( pCmdItem->StateIsTearOff() )
		return true;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	return false;
}

bool CExtPopupMenuWnd::_IsPalette() const
{
	ASSERT_VALID( this );
DWORD dwTrackFlags = TrackFlagsGet();
	if( (dwTrackFlags&TPMX_PALETTE) != 0 )
		return true;
	return false;
}

void CExtPopupMenuWnd::_DoPaintMenuItems( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( _IsPalette() )
	{
		_DoPaintMenuItemsPalette( dc );
		return;
	}
visible_items_t v;
	_GetVisibleItems( dc.GetSafeHdc(), v );
INT vis_iter = 0;
bool bPrevToolButton = false;
	for( ; vis_iter < v.GetSize(); ++vis_iter )
	{ // paint visible items
		VisibleItemDefinition_t & vi = v[ vis_iter ];
		ASSERT(
				vi.m_nIndex >= 0
			&&	vi.m_nIndex < m_items_all.GetSize()
			);
		ASSERT( !vi.m_rcItem.IsRectEmpty() );
		ASSERT( !vi.m_rcItemWithIndents.IsRectEmpty() );
		ASSERT( dc.RectVisible(&vi.m_rcItem) );
		ASSERT( dc.RectVisible(&vi.m_rcItemWithIndents) );
		MENUITEMDATA & mi =
			ItemGetInfo( vi.m_nIndex );
		ASSERT( mi.IsDisplayed() );
		ASSERT( mi.GetCmdID() == vi.m_nHelperCmdID );
		if( vi.m_bHelperIsSeparator )
		{
			ASSERT( mi.IsSeparator() );
			if( bPrevToolButton )
			{
				PmBridge_GetPM()->PaintSeparator( dc, vi.m_rcItemWithIndents, false, true, this );
			}
			else
				PmBridge_GetPM()->PaintMenuSeparator(
					dc,
					vi.m_rcItemWithIndents,
					mi.GetIconAreaWidth(),
					g_bMenuHighlightRarely && vi.m_bRarelyUsed,
					( mi.IsForceNoLeftGradient() || IsNoHighlightIconArea() ) ? true : false,
					this
					);
			bPrevToolButton = false;
			continue;
		}
		ASSERT( !mi.IsSeparator() );
		ASSERT( mi.IsPopup() == vi.m_bHelperIsPopup );

		bool bOwnerDrawMode =
			(		(! _FindCustomizeMode() )
				&&	( GetSite()->m_dwTrackFlags&TPMX_OWNERDRAW_FIXED ) ? true : false
			) ? true : false;
		DRAWITEMDATA _DrawItemData(
			&dc,
			&vi.m_rcItem,
			&vi.m_rcItemWithIndents,
			&mi,
			this,
			bOwnerDrawMode,
			vi.m_bHelperIsPopup,
			g_bMenuHighlightRarely && vi.m_bRarelyUsed,
			g_bMenuHighlightRarely && vi.m_bRarelyUsedPrev,
			g_bMenuHighlightRarely && vi.m_bRarelyUsedNext
			);
		
		bool bPainted =
			(_DrawItemData.m_bMenuOwnerDrawFixed)
				? _DrawItemData.DoOwnerDrawPainting()
				: false
				;
		if( ! bPainted )
			_DrawItemData.PaintDefault();

		bPrevToolButton = mi.IsToolButton();
	} // paint visible items
}

void CExtPopupMenuWnd::_DoPaintMenuItemsPalette( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
visible_items_t v;
	_GetVisibleItems( dc.GetSafeHdc(), v );
INT vis_iter = 0;
	for( ; vis_iter < v.GetSize(); ++vis_iter )
	{ // paint visible items
		VisibleItemDefinition_t & vi = v[ vis_iter ];
		ASSERT(
				vi.m_nIndex >= 0
			&&	vi.m_nIndex < m_items_all.GetSize()
			);
		ASSERT( !vi.m_rcItem.IsRectEmpty() );
		ASSERT( !vi.m_rcItemWithIndents.IsRectEmpty() );
		ASSERT( dc.RectVisible(&vi.m_rcItem) );
		ASSERT( dc.RectVisible(&vi.m_rcItemWithIndents) );
		MENUITEMDATA & mi =
			ItemGetInfo( vi.m_nIndex );
		ASSERT( mi.IsDisplayed() );
		ASSERT( mi.GetCmdID() == vi.m_nHelperCmdID );
		if( vi.m_bHelperIsSeparator )
		{
			ASSERT( mi.IsSeparator() );
			CRect rc = vi.m_rcItemWithIndents;
			PmBridge_GetPM()->PaintSeparator( dc, vi.m_rcItemWithIndents, false, true, this );
			continue;
		}
		ASSERT( !mi.IsSeparator() );
		ASSERT( mi.IsPopup() == vi.m_bHelperIsPopup );
		
		bool bOwnerDrawMode =
			(		(! _FindCustomizeMode() )
				&&	( GetSite()->TrackFlagsGet()&TPMX_OWNERDRAW_FIXED ) != 0 // fix 2.23 ( == replaced with != )
			) ? true : false;
		DRAWITEMDATA _DrawItemData(
			&dc,
			&vi.m_rcItem,
			&vi.m_rcItemWithIndents,
			&mi,
			this,
			bOwnerDrawMode,
			vi.m_bHelperIsPopup,
			g_bMenuHighlightRarely && vi.m_bRarelyUsed,
			g_bMenuHighlightRarely && vi.m_bRarelyUsedPrev,
			g_bMenuHighlightRarely && vi.m_bRarelyUsedNext
			);
		
		bool bPainted =
			(_DrawItemData.m_bMenuOwnerDrawFixed)
				? _DrawItemData.DoOwnerDrawPainting()
				: false
				;
		bool bForceNoIcon = false, bForceNoText = false;
		CExtCmdIcon * pIcon = mi.GetIconPtr();
		if( pIcon != NULL && pIcon->IsEmpty() )
			pIcon = NULL;
		if( mi.IsInplaceEdit() )
			bForceNoIcon = bForceNoText = true;
		else if( pIcon == NULL || ( pIcon != NULL && (! mi.IsIconMode() ) ) )
		{
			//bForceNoIcon = true;
		}
		else
			bForceNoText = true;
		if( ! bPainted )
			_DrawItemData.PaintDefault( bForceNoIcon, bForceNoText );
	} // paint visible items
}

#if _MFC_VER < 0x700
void CExtPopupMenuWnd::OnActivateApp(BOOL bActive, HTASK hTask) 
#else
void CExtPopupMenuWnd::OnActivateApp(BOOL bActive, DWORD hTask) 
#endif
{
	ASSERT_VALID( this );
	bActive;
	hTask;
	if( ( GetStyle() & WS_VISIBLE ) == 0 )
	{
		m_pWndParentMenu = NULL;
		m_dwTrackFlags &= ~(TPMX_CUSTOMIZE_MODE);
	}
	_CancelingSet();
	_OnCancelMode();
}

bool CExtPopupMenuWnd::_StartScrolling(int nButtonIndex)
{
	ASSERT_VALID( this );

	if( GetSafeHwnd() == NULL )
		return FALSE;

	if( (nButtonIndex == IDX_SCROLL_TOP
			|| nButtonIndex == IDX_SCROLL_BOTTOM)
		)
	{
		ASSERT( m_bScrollingAvailable );
		HWND hWndOwn = m_hWnd;
		_ItemFocusDelay();
		if( ! ::IsWindow( hWndOwn ) )
			return true;
		_ItemFocusCancel( FALSE );
		if( ! ::IsWindow( hWndOwn ) )
			return true;
		m_nCurIndex = nButtonIndex;
		Invalidate( FALSE );
		m_nAcceleratedPixelsPerStepCurrent = __SCROLLING_PIXEL_STEP;
		m_nAcceleratedMilliSecondsPassed = 0;
		SetTimer(
			ID_TIMER_SCROLLING,
			ID_PERIOD_SCROLLING,
			NULL
			);
		if( !_FindCustomizeMode() )
			_SetCapture();
		return true;
	}
	return false;
}

bool CExtPopupMenuWnd::_PtInWndArea( CPoint ptClient )
{
	if( GetSafeHwnd() == NULL )
		return false;

	ClientToScreen( &ptClient );
HWND hWndFromPoint = ::WindowFromPoint(ptClient);
	if( hWndFromPoint == NULL )
		return false;
	if( hWndFromPoint == m_hWnd )
		return true;
	return false;
}

LRESULT CExtPopupMenuWnd::WindowProc(
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
{
	switch( message )
	{
	case __WM_START_RESIZING:
		//if( _IsResizingMode() )
		{
			m_bHelperResizingMode = true;
			CPoint ptScreenClick( short(LOWORD(lParam)), short(HIWORD(lParam)) );
			_DoResizing(
				ptScreenClick,
				( (TrackFlagsGet()&TPMX_RIBBON_RESIZING_VERTICAL_ONLY) != 0 ) ? true : false
				);
		}
		return 0;
	//break;
	case WM_NCDESTROY:
	case WM_DESTROY:
		m_bDelayedHiding = false;
		if( m_ctrlShadow.GetSafeHwnd() != NULL )
			m_ctrlShadow.DestroyWindow();
	break;
	} // switch( message )
LRESULT lResult = CExtPopupBaseWnd::WindowProc( message, wParam, lParam );
	return lResult;
}


bool CExtPopupMenuWnd::_OnMouseClick(
	UINT nFlags,
	CPoint point,
	bool & bNoEat
	)
{
	ASSERT_VALID( this );

	if( GetSafeHwnd() == NULL )
		return false;

// - 2.27 (for command processing in separated dropdown item)
//	if( ! m_bAnimFinished )
//		return true;

CExtPopupMenuSite & _site = GetSite();
	if(	_site.IsShutdownMode()
		|| _site.IsEmpty()
		|| _site.GetAnimated() != NULL
		)
	{
		if( ( TrackFlagsGet() & TPMX_SYSMENU ) != 0 )
		{
			bNoEat = true;
			return false;
		}
		return true;
	}

TranslateMouseClickEventData_t _td( this, nFlags, point, bNoEat );
	if( _td.Notify() )
	{
		bNoEat = _td.m_bNoEat;
		return true;
	}

bool bLButtonUpCall =
		(nFlags==WM_LBUTTONUP || nFlags==WM_NCLBUTTONUP)
			? true : false;
CPoint ptScreenClick( point );
	ClientToScreen( &ptScreenClick );
	if( (TrackFlagsGet()&TPMX_RIBBON_RESIZING) != 0 )
	{
		UINT nHT = (UINT) ::SendMessage( m_hWnd, WM_NCHITTEST, 0L, MAKELPARAM(ptScreenClick.x,ptScreenClick.y) );
		switch( nHT )
		{
		case HTLEFT:
		case HTRIGHT:
		case HTTOP:
		case HTBOTTOM:
		case HTTOPLEFT:
		case HTTOPRIGHT:
		case HTBOTTOMLEFT:
		case HTBOTTOMRIGHT:
			if( ! bLButtonUpCall )
			{
				m_bHelperResizingMode = true;
				PostMessage( __WM_START_RESIZING, 0, MAKELPARAM(ptScreenClick.x,ptScreenClick.y) );
			} // if( ! bLButtonUpCall )
			m_bHelperResizingMode = false;
			bNoEat = false;
			return true;
		} // switch( nHT )
		m_bHelperResizingMode = false;
	} // if( (TrackFlagsGet()&TPMX_RIBBON_RESIZING) != 0 )

HWND hWndFromPoint = ::WindowFromPoint( ptScreenClick );
	if( hWndFromPoint != NULL )
	{
		if(		(::__EXT_MFC_GetWindowLong(hWndFromPoint,GWL_STYLE)&WS_CHILD) != 0
			&&	::GetParent(hWndFromPoint) == m_hWnd
			)
		{
			CWnd * pWnd = CWnd::FromHandlePermanent( hWndFromPoint );
			if( pWnd != NULL )
			{
				int nCount = ItemGetCount();
				for( int i = 0; i < nCount; i++ )
				{
					MENUITEMDATA & mi = ItemGetInfo( i );
					if( !mi.IsInplaceEdit() )
						continue;
					if( ((LPVOID)mi.GetInplaceEditPtr()) != ((LPVOID)pWnd) )
						continue;
					if( m_nCurIndex == i )
						break;
					HWND hWndOwn = m_hWnd;
					_ItemFocusSet( i, FALSE, TRUE, FALSE );
					if( ! ::IsWindow( hWndOwn ) )
						return true;
					break;
				} // for( int i = 0; i < nCount; i++ )
			} // if( pWnd != NULL )
			bNoEat = true;
			return false;
		}
		if( ! bLButtonUpCall )
		{
			CWnd * pWndPermanent = CWnd::FromHandlePermanent( hWndFromPoint );
			if( pWndPermanent != NULL )
			{
				if( pWndPermanent->IsKindOf( RUNTIME_CLASS( CExtMenuControlBar ) ) )
				{
					CPoint ptClientPermanent = ptScreenClick;
					pWndPermanent->ScreenToClient( &ptClientPermanent );
					int nBtnIdx = ((CExtMenuControlBar*)pWndPermanent)->HitTest( ptClientPermanent );
					if( nBtnIdx >= 0 )
					{
						CExtBarButton * pTBB = ((CExtMenuControlBar*)pWndPermanent)->GetButton( nBtnIdx );
						ASSERT_VALID( pTBB );
						if( pTBB->IsKindOf( RUNTIME_CLASS( CExtBarMdiDocButton ) ) )
						{
							_OnCancelMode();
							bNoEat = true;
							return false;
						} // if( pTBB->IsKindOf( RUNTIME_CLASS( CExtBarMdiDocButton ) ) )
					} // if( nBtnIdx >= 0 )
				} // if( pWndPermanent->IsKindOf( RUNTIME_CLASS( CExtMenuControlBar ) ) )
			} // if( pWndPermanent != NULL )
		} // if( ! bLButtonUpCall )
	} // if( hWndFromPoint != NULL )

/*
CRect rectWindow;
	GetWindowRect( &rectWindow );
	ScreenToClient( &rectWindow );
	if( !rectWindow.PtInRect(point) )
*/
	if( bLButtonUpCall && (!_PtInWndArea(point)) )
	{
		CExtPopupMenuTipWnd & _tipWnd = GetTip();
		if( _tipWnd.GetSafeHwnd() != NULL )
		{
			CPoint ptScreen( point );
			ClientToScreen( &ptScreen );
			if( _tipWnd.GetSafeHwnd() == ::WindowFromPoint(ptScreen) )
				return true; // eat!
		}

		HWND hWndOwn = GetSafeHwnd();
		ASSERT( hWndOwn != NULL );
		ASSERT( ::IsWindow(hWndOwn) );

		if(		m_pWndParentMenu != NULL
			&&	m_pWndParentMenu->GetSafeHwnd() != NULL
			)
		{
			ASSERT_VALID( m_pWndParentMenu );
			ClientToScreen( &point );
			m_pWndParentMenu->ScreenToClient( &point );

			CExtPopupMenuWnd * pWndParentMenu = m_pWndParentMenu;
			if(	pWndParentMenu->_OnMouseClick(
					nFlags,
					point,
					bNoEat
					)
				)
			{
				if( bNoEat )
					return false;
				if( ::IsWindow(hWndOwn) )
				{
					INT nSaved = m_nFadeOutAnimationStepCount;
					m_nFadeOutAnimationStepCount = -1;
					GetTopLevelMenuWnd()->PostMessage( WM_CANCELMODE );
					_OnCancelMode();
					m_nFadeOutAnimationStepCount = nSaved;
				}
				return true;
			}
			return false;
		}

		// fixed in v. 2.20
		if(		nFlags == WM_RBUTTONUP
			||	nFlags == WM_LBUTTONUP
			)
		{
			// + 2.83
			CExtPopupMenuWnd * pTopLevelPopup = GetTopLevelMenuWnd();
			ASSERT_VALID( pTopLevelPopup );
			bool bHelperMouseBtnDownOnStart = pTopLevelPopup->m_bHelperMouseBtnDownOnStart;
			pTopLevelPopup->m_bHelperMouseBtnDownOnStart = false;
			// + 2.25
			if(		::IsWindow(hWndOwn)
				&&	CExtToolControlBar::g_bMenuTracking
				&&	hWndFromPoint != NULL
				)
			{
				CWnd * pWndCmp =
					CWnd::FromHandlePermanent( hWndFromPoint );
				if( pWndCmp != NULL )
				{
					CExtToolControlBar * pToolBar =
						DYNAMIC_DOWNCAST( CExtToolControlBar, pWndCmp );
					if( pToolBar != NULL )
					{
						ASSERT_VALID( pToolBar );
						int nBtnIdx = pToolBar->GetMenuTrackingButton();
						if( nBtnIdx >= 0 )
						{
							CExtBarButton * pTBB = pToolBar->GetButton( nBtnIdx );
							if( pTBB != NULL )
							{
								ASSERT_VALID( pTBB );
								CRect rcBtn = pTBB->Rect();
								pToolBar->ClientToScreen( &rcBtn );
								if( rcBtn.PtInRect(ptScreenClick) )
									return true;
							} // if( pTBB != NULL )
						} // if( nBtnIdx >= 0 )
					} // if( pToolBar == pWndCmp )
				} // if( pWndCmp != NULL )
				if( ! bHelperMouseBtnDownOnStart ) // + 2.83
					_OnCancelMode();
			}
			return true;
		}

		if(		nFlags == WM_NCLBUTTONUP
			&&	g_PaintManager.m_DWM.IsCompositionEnabled()
			)
			return true;
		_OnCancelMode();
		return false;
	} // if( bLButtonUpCall && (!_PtInWndArea(point)) )

bool bInplaceControlArea = false, bInplaceDropDownArea = false;
int nHitTest =
		_HitTest(
			point,
			&bInplaceControlArea,
			&bInplaceDropDownArea
			);

	if( _IsTearOff() && nHitTest == IDX_TEAROFF )
	{
		bool bLButtonUpCall =
			( nFlags == WM_LBUTTONUP || nFlags == WM_NCLBUTTONUP )
				? true : false;
		if( ! bLButtonUpCall )
			_DoTearOff();
		return true;
	}

	if( bLButtonUpCall )
	{
		HWND hWndOwn = m_hWnd;
		if( _StartScrolling(nHitTest) )
			return true;
		if( ! ::IsWindow( hWndOwn ) )
			return true;
	}

	if( nHitTest < 0 )
	{
		if(		nHitTest == IDX_RIBBON_FILE_MENU_OPTIONS_BUTTON
			||	nHitTest == IDX_RIBBON_FILE_MENU_EXIT_BUTTON
			)
		{
			if( bLButtonUpCall )
			{
				DWORD dwButtonType = 0;
				switch( nHitTest )
				{
				case IDX_RIBBON_FILE_MENU_OPTIONS_BUTTON:
					dwButtonType = TPMX_RIBBON_OPTIONS_BUTTON;
				break;
				case IDX_RIBBON_FILE_MENU_EXIT_BUTTON:
					dwButtonType = TPMX_RIBBON_EXIT_BUTTON;
				break;
				} // switch( nHitTest )
				RIBBONFILEMENUBUTTONINVOCATION _rfmbi( dwButtonType, this );
				m_nCurIndex = IDX_NOTHING;
				_SetCapture();
				if( nHitTest == IDX_RIBBON_FILE_MENU_EXIT_BUTTON )
					m_dwTrackFlags |= TPMX_NO_FADE_OUT_ANIMATION;
				DestroyWindow();
				g_SoundPlayer->PlaySound( CExtSoundPlayer::__ON_MENU_CMD_CLICKED );
				_rfmbi.Notify();
			} // if( bLButtonUpCall )
			else
			{
				CRect rcItem;
				_GetItemRect( nHitTest, rcItem );
				if( ! rcItem.IsRectEmpty() )
					InvalidateRect( &rcItem );
			} // else from if( bLButtonUpCall )
			return true;
		}
		if( bLButtonUpCall )
		{

			if(		(	nHitTest == IDX_SCROLL_TOP
					||	nHitTest == IDX_SCROLL_BOTTOM
					)
				//&&	m_nCurIndex != nHitTest
				)
			{
				return true;
			}

			if( nHitTest == IDX_EXPAND )
			{
				if(		( ! m_bExpandWasPressed )
					&&	m_bExpandAvailable
					)
					_DoExpand();
				return true;
			}

			// fixed in v. 2.20
			if(		nFlags != WM_RBUTTONUP
				&&	nFlags != WM_LBUTTONUP
				)
			{
				_OnCancelMode();
				return true;
			}
		} // if( bLButtonUpCall )


		if(		m_eCombineAlign != __CMBA_NONE
			&&	m_bTopLevel
			&&	(!bLButtonUpCall)
			)
		{
			CRect rcExcludeAreaTest( m_rcExcludeArea );
			ScreenToClient( &rcExcludeAreaTest );
			if( rcExcludeAreaTest.PtInRect( point ) )
			{
				_OnCancelMode();
				return true;
			} // if( rcExcludeAreaTest.PtInRect( point ) )
		} // if( m_eCombineAlign != __CMBA_NONE ...

		if(		nFlags != WM_LBUTTONUP
			&&	nFlags != WM_LBUTTONDOWN
			&&	nFlags != WM_NCLBUTTONUP
			&&	nFlags != WM_NCLBUTTONDOWN
			&&	nFlags != WM_RBUTTONDOWN
			&&	nFlags != WM_NCRBUTTONDOWN
			&&	nFlags != WM_MBUTTONDOWN
			&&	nFlags != WM_NCMBUTTONDOWN
			)
		{
			bNoEat = false;
			return true;
		}

		return false;
	}
	ASSERT( nHitTest < m_items_all.GetSize() );

	if( !bLButtonUpCall )
		return true;

MENUITEMDATA & mi = ItemGetInfo(nHitTest);
	if( ( ! mi.IsEnabled() ) || mi.IsSeparator() )
	{
		_ItemFocusCancel( TRUE );
		_SetCapture();
		return true;
	}

	if(		mi.IsInplaceEdit()
		&&	mi.IsPopup()
		&&	(! mi.IsAllowInplaceEditActivation() )
		)
	{
		bInplaceControlArea = false;
		bInplaceDropDownArea = true;
	}

	if(		mi.IsInplaceEdit()
		&&	( ! mi.IsPopup() )
		&&	( ! bInplaceDropDownArea )
		&&	( ! _FindHelpMode() )
		)
	{
		_SetCapture();
		HWND hWndOwn = m_hWnd;
		_ItemFocusSet( nHitTest, FALSE, TRUE, FALSE );
		if( ! ::IsWindow( hWndOwn ) )
			return true;
		CWnd * pWndInplace = mi.GetInplaceEditPtr();
		if( pWndInplace != NULL )
		{
			ASSERT_VALID( pWndInplace );
			ASSERT( pWndInplace->GetSafeHwnd() != NULL );
			if( mi.IsAllowInplaceEditActivation() )
			{
				if( (pWndInplace->GetStyle() & WS_VISIBLE) == 0 )
				{
					CRect rcInplaceEdit;
					_GetItemRect( nHitTest, rcInplaceEdit );
					rcInplaceEdit =
						mi.AdjustInplaceEditRect(
							rcInplaceEdit,
							OnQueryLayoutRTL()
							);
					pWndInplace->SetWindowPos(
						NULL,
						rcInplaceEdit.left, rcInplaceEdit.top,
						rcInplaceEdit.Width(), rcInplaceEdit.Height(),
						SWP_NOZORDER|SWP_NOOWNERZORDER
							|SWP_NOACTIVATE|SWP_SHOWWINDOW
						);
				} // if( (pWndInplace->GetStyle() & WS_VISIBLE) == 0 )
				pWndInplace->SetFocus();
			} // if( mi.IsAllowInplaceEditActivation() )
		} // if( pWndInplace != NULL )
		return true;
	} // if( mi.IsInplaceEdit() ...
bool bContinueAsCommand = false;
	if( mi.IsPopup() )
	{
		bool bPopupOpened = false;
		if( mi.GetPopup()->GetSafeHwnd() != NULL )
			bPopupOpened = true;
//		if( bPopupOpened )
//			return false;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		if( (!bPopupOpened) && mi.IsInplaceEdit() && _FindHelpMode() )
		{
			CExtCustomizeCmdTreeNode * pNode = mi.GetCmdNode();
			if( pNode != NULL )
				_EndSequence(
					pNode->GetCmdID( false ),
					mi.GetCmdReceiver()
					);
		} // if( (!bPopupOpened) && mi.IsInplaceEdit() && _FindHelpMode() )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

		if( (!bPopupOpened) && mi.IsInplaceEdit() && bInplaceDropDownArea )
		{
			HWND hWndOwn = m_hWnd;
			_ItemFocusCancel( FALSE );
			if( ! ::IsWindow( hWndOwn ) )
				return true;
		}
// - 2.27 (for command processing in separated dropdown item)
//		if( m_nDelayedFocusItemIndex < 0 )
		{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
			bool bSeparatedDropDown = false;
			const CExtCustomizeCmdTreeNode * pNode = mi.GetCmdNode();
			if( pNode != NULL )
			{
				ASSERT_VALID( pNode );
				bSeparatedDropDown =
					(pNode->GetFlags() & __ECTN_TBB_SEPARATED_DROPDOWN)
						? true : false;
			} // if( pNode != NULL )
			if( bSeparatedDropDown )
			{
				bool bRTL = OnQueryLayoutRTL();
				CRect rcItem;
				_GetItemRect( nHitTest, rcItem );
				CRect rcPopupArrow( rcItem );
				//rcPopupArrow.DeflateRect(__EXT_MENU_GAP,__EXT_MENU_GAP);
				if( bRTL )
					rcPopupArrow.right = rcPopupArrow.left + __EXT_MENU_POPUP_ARROW_AREA_DX;
				else
					rcPopupArrow.left = rcPopupArrow.right - __EXT_MENU_POPUP_ARROW_AREA_DX;
				ClientToScreen( &rcPopupArrow );
				if( ! rcPopupArrow.PtInRect( ptScreenClick ) )
				{
					_SetCapture();
					_ItemFocusCancel( FALSE, TRUE );
					_ItemFocusSet( nHitTest, FALSE, FALSE, FALSE );
					bContinueAsCommand = true;
				}
			} // if( bSeparatedDropDown )
			else
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
				if( ! bPopupOpened )
				{
					HWND hWndOwn = m_hWnd;
					_ItemFocusSet(
						nHitTest,
						TRUE,
						TRUE,
						FALSE
						);
					if( ! ::IsWindow( hWndOwn ) )
						return true;
				}
				else
					return false;
		} // if( m_nDelayedFocusItemIndex < 0 )
		if( ! bContinueAsCommand )
		{
			_SetCapture();
			return true;
		}
	} // if( mi.IsPopup() )

	if( mi.IsExtraMark() && (! _FindHelpMode() ) )
	{
		ASSERT( mi.GetExtraMarkCallback() != NULL );
		mi.GetExtraMarkCallback()( this, &mi );
		CClientDC dc( this );
		_DoPaint( dc );
	} // if( mi.IsExtraMark() ...
	else if(
			mi.IsExecutableCmdID()
		||	bContinueAsCommand
		||	(	mi.IsPopup()
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
			&&	mi.GetCmdNode() != NULL
			&&	(mi.GetCmdNode()->GetFlags()&__ECTN_TBB_SEPARATED_DROPDOWN) != 0
			&&	(mi.GetCmdNode()->GetFlags()&(__ECTN_TBB_COLOR|__ECTN_TBB_UNDO_REDO|__ECTN_TBB_DATE)) != 0
#else // (!defined __EXT_MFC_NO_CUSTOMIZE)
			&&	(	mi.GetPopup()->IsKindOf( RUNTIME_CLASS(CExtPopupColorMenuWnd) )
#if (!defined __EXT_MFC_NO_UNDO_REDO_POPUP)
				||	mi.GetPopup()->IsKindOf( RUNTIME_CLASS(CExtPopupUndoRedoMenuWnd) )
#endif // (!defined __EXT_MFC_NO_UNDO_REDO_POPUP)
#if (!defined __EXT_MFC_NO_DATE_PICKER_POPUP)
				||	mi.GetPopup()->IsKindOf( RUNTIME_CLASS(CExtPopupDatePickerMenuWnd) )
#endif // (!defined __EXT_MFC_NO_DATE_PICKER_POPUP)
				)
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
			)
		)
	{
		CExtPopupMenuWnd * pTopLevelPopup = GetTopLevelMenuWnd();
		ASSERT_VALID( pTopLevelPopup );
		bool bHelperMouseBtnDownOnStart = pTopLevelPopup->m_bHelperMouseBtnDownOnStart;
		pTopLevelPopup->m_bHelperMouseBtnDownOnStart = false;
		bool bContinue = true;
		if( mi.IsExecutableCmdID() )
		{
			if( bHelperMouseBtnDownOnStart )
			{
				CPoint pt;
				::GetCursorPos( &pt );
				if( m_ptStartMousePos == pt )
					bContinue = false;
			}
		}
		if( bContinue )
		{
			if(	!(		mi.GetCmdDeliverCb() != NULL
					&&	mi.GetCmdDeliverCb()( this, &mi )
					)
				)
			{
				_CancelingSet();
				_EndSequenceEx( mi );
			}
		}
	} // else if( mi.IsExecutableCmdID() ...
	else
		_OnCancelMode();
	return true;
}

void CExtPopupMenuWnd::_EndSequenceEx(
	CExtPopupMenuWnd::MENUITEMDATA & mi
	)
{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	_KeyTipsShow( false );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	if(		(! _IsFadeOutAnimation() )
		&&	m_ctrlShadow.GetSafeHwnd() != NULL
		)
		m_ctrlShadow.DestroyWindow();

CExtPopupMenuWnd * pTLP = GetTopLevelMenuWnd();
	if( pTLP != NULL )
		pTLP->m_bEndSequenceInvoked = true;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeCmdTreeNode * pNode = mi.GetCmdNode();
CExtCustomizeSite * pSite =
			CExtCustomizeSite::GetCustomizeSite(
				mi.GetCmdReceiver()
				);
	if( pNode != NULL && pSite != NULL )
	{
		if( mi.IsExecutableCmdID() )
		{
			ASSERT_VALID( pNode );
			if( ( pNode->GetFlags() & (__ECTN_TBB_COLOR|__ECTN_TBB_SEPARATED_DROPDOWN) ) == 0 )
			{
				CExtCustomizeCmdTreeNode * pNodeParent = pNode->GetParentNode();
				if( pNodeParent != NULL )
				{
					ASSERT_VALID( pNodeParent );
					if(		(	pNodeParent->GetFlags() &
									(	__ECTN_TBB_COLOR
									|	__ECTN_TBB_SEPARATED_DROPDOWN
									|	__ECTN_TBB_AUTOCHANGE_ID
										)
							)
							==		(	__ECTN_TBB_SEPARATED_DROPDOWN
									|	__ECTN_TBB_AUTOCHANGE_ID
									)
						)
					{
						pNodeParent->SetCmdID(
							pNode->GetCmdID(),
							true
							);
						HWND hWnd = mi.GetCmdReceiver();
						if( hWnd != NULL && ::IsWindow( hWnd ) )
						{
							CExtCmdItem * pCmdItem = g_CmdManager->CmdGetPtr( g_CmdManager->ProfileNameFromWnd( hWnd ), pNode->GetCmdID() );
							if( pCmdItem != NULL )
							{
								pNodeParent->SetTextInMenu(
									pNode->GetTextInMenu( pCmdItem, true )
									);
								pNodeParent->SetTextInToolbar(
									pNode->GetTextInToolbar( pCmdItem, true )
									);
							} // if( pCmdItem != NULL )
						} // if( hWnd != NULL && ::IsWindow( hWnd ) )
					}
				} // if( pNodeParent != NULL )
			} // if( ( pNode->GetFlags() & (__ECTN_TBB_COLOR|__ECTN_TBB_SEPARATED_DROPDOWN) ) == 0 )
		} // if( mi.IsExecutableCmdID() )
		else if( ( pNode->GetFlags() & (__ECTN_TBB_COLOR|__ECTN_TBB_SEPARATED_DROPDOWN) ) == (__ECTN_TBB_COLOR|__ECTN_TBB_SEPARATED_DROPDOWN) )
		{
			_EndSequence();
			COLORREF clr;
			if(	pSite->OnColorItemValueGet(
					NULL,
					pNode,
					clr,
					true
					)
				)
				pSite->OnColorItemValueSet(
					NULL,
					pNode,
					clr,
					true
					);
			return;
		} // else if( ( pNode->GetFlags() & (__ECTN_TBB_COLOR|__ECTN_TBB_SEPARATED_DROPDOWN) ) == (__ECTN_TBB_COLOR|__ECTN_TBB_SEPARATED_DROPDOWN) )
	} // if( pNode != NULL && pSite != NULL )

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	_EndSequence( mi.GetCmdID(), mi.GetCmdReceiver() );
}

void CExtPopupMenuWnd::_EndSequence(
	UINT nCmdID, // = 0
	HWND hWndDeliver // = NULL
	)
{
	__PROF_UIS_MANAGE_STATE;
	ASSERT_VALID( this );

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	_KeyTipsShow( false );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

HWND hWndOwn = GetSafeHwnd();
	ASSERT( hWndOwn != NULL );
bool bCustomizeMode = _FindCustomizeMode();

	if(		(! _IsFadeOutAnimation() )
		&&	m_ctrlShadow.GetSafeHwnd() != NULL
		)
		m_ctrlShadow.DestroyWindow();

#ifdef _DEBUG
	if( hWndDeliver != NULL )
	{
		ASSERT( ::IsWindow(hWndDeliver) );
	}
#endif // _DEBUG
	ASSERT( m_hWndCmdReceiver != NULL );
	ASSERT( ::IsWindow(m_hWndCmdReceiver) );

CExtPopupMenuSite & _site = GetSite();
UINT * lpnResultCmdID = _site.GetTargetCmdIdPtr();
	if( lpnResultCmdID != NULL )
		(*lpnResultCmdID) = nCmdID;
DWORD dwTrackFlags = TrackFlagsGet();
CExtCmdItem * pCmdItem = NULL;
	if( nCmdID > 0 && ((dwTrackFlags&TPMX_NO_WM_COMMAND)==0) )
	{
		pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd( m_hWndCmdReceiver ),
				nCmdID
				);
		//ASSERT( pCmdItem != NULL );
	} // if( nCmdID > 0 && ((dwTrackFlags&TPMX_NO_WM_COMMAND)==0) )

CExtPopupMenuWnd * pTopPopup = _site.GetInstance();
	ASSERT_VALID( pTopPopup );
HWND hWndTopPopup = pTopPopup->GetSafeHwnd();
	ASSERT(
			hWndTopPopup != NULL
		&&	::IsWindow( hWndTopPopup )
		);

	if( hWndDeliver == NULL )
		hWndDeliver = m_hWndCmdReceiver;

CWnd * pTopLevelParent = GetTopLevelParent();
HWND hWndTopLevelParent = pTopLevelParent->GetSafeHwnd();

CRect rcBkUpdate( 0, 0, 0, 0 );
	// hide all sequence
bool bFadeOut = _IsFadeOutAnimation();
	if( ! bFadeOut )
	{
		CExtPopupMenuWnd * pPopup = this;
		for( ; pPopup != NULL; pPopup = pPopup->m_pWndParentMenu )
		{
			ASSERT_VALID( pPopup );
			ASSERT( pPopup->GetSafeHwnd() != NULL );
			CRect rcPopup;
			pPopup->GetWindowRect( &rcPopup );
			if( rcBkUpdate.IsRectEmpty() )
				rcBkUpdate = rcPopup;
			else
			{
				CRect rcPrev( rcBkUpdate );
				rcBkUpdate.UnionRect( &rcPrev, &rcPopup );
			}
			CExtPopupMenuTipWnd & _tipWnd = pPopup->GetTip();
			if(		_tipWnd.GetSafeHwnd() != NULL
				&&	_tipWnd.IsWindowVisible()
				)
			{
				CRect rcToolTip;
				_tipWnd.GetWindowRect( &rcToolTip );
				CRect rcPrev( rcBkUpdate );
				rcBkUpdate.UnionRect( &rcPrev, &rcToolTip );
			}
			pPopup->ShowWindow( SW_HIDE );
		} // for( ; pPopup != NULL; pPopup = pPopup->m_pWndParentMenu )
		if( ! g_PaintManager.m_bIsWin2000orLater )
			CExtPaintManager::stat_PassPaintMessages();
		// allow bottom windows repaint
		if(	!rcBkUpdate.IsRectEmpty()
			&&	hWndTopLevelParent != NULL
			&& ::IsWindow(hWndTopLevelParent)
			)
		{
			::ScreenToClient(
				hWndTopLevelParent, (LPPOINT)&rcBkUpdate);
			::ScreenToClient(
				hWndTopLevelParent, ((LPPOINT)&rcBkUpdate)+1
				);
		}
	} // if( ! bFadeOut )

	if(		m_hWndNotifyMenuClosed != NULL
		&&	::IsWindow( m_hWndNotifyMenuClosed )
		)
		::SendMessage(
			m_hWndNotifyMenuClosed,
			g_nMsgNotifyMenuClosed,
			WPARAM(nCmdID),
			LPARAM( this )
			);

	// top popup (and all children) finally must be closed

	if( ! bFadeOut )
	{
		if( ::IsWindow( hWndTopPopup ) )
			::PostMessage( hWndTopPopup, WM_CLOSE, 0, 0 );
		CExtToolControlBar::_CloseCustomMenusAll();
		if( CExtToolControlBar::g_bMenuTracking )
			CExtToolControlBar::_CloseTrackingMenus();
	} // if( ! bFadeOut )

bool bOwnWindowDestroyed = ::IsWindow( hWndOwn ) ? false : true;
	if(		::IsWindow( hWndDeliver )
		&&	( bOwnWindowDestroyed || (! bCustomizeMode ) )
		)
	{
		if( pCmdItem != NULL )
		{
			// if command target still exists -> deliver command to it
			if( (dwTrackFlags&TPMX_NO_WM_COMMAND)==0 )
			{
				if( ! bFadeOut )
					_site.DoneInstance();
				VERIFY( pCmdItem->Deliver( hWndDeliver ) );
			}
		} // if( pCmdItem != NULL )
		else
		{
			if(		(! bOwnWindowDestroyed )
				&&	bFadeOut
				&&	m_nCurIndex >= 0
				&&	ItemGetInfo( m_nCurIndex ).IsPopup()
				)
			{
				CExtPopupMenuWnd * pPopup = ItemGetInfo( m_nCurIndex ).GetPopup();
				if( pPopup->GetSafeHwnd() != NULL )
				{
					INT nSaved = pPopup->m_nFadeOutAnimationStepCount;
					if( nCmdID == 0 )
						pPopup->m_nFadeOutAnimationStepCount = -1;
					pPopup->_EndSequence( nCmdID, hWndDeliver );
					pPopup->m_nFadeOutAnimationStepCount = nSaved;
					return;
				}
			}
			if(		(dwTrackFlags&TPMX_NO_WM_COMMAND) == 0 
				&&	nCmdID != 0
				)
				::PostMessage(
					hWndDeliver,
					WM_COMMAND,
					WPARAM(nCmdID),
					0L
					);
		} // else from if( pCmdItem != NULL )
		g_SoundPlayer->PlaySound( CExtSoundPlayer::__ON_MENU_CMD_CLICKED );
	} // if( pCmdItem != NULL && ...

	if( bOwnWindowDestroyed )
		return;

	if( bFadeOut )
		DestroyWindow();
	else
		_site.DoneInstance();
}

void CExtPopupMenuWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	nFlags;
	point;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( (!_FindCustomizeMode()) )
		return;
int nIndex = _HitTest( point );
	if( nIndex >= 0 )
		return;
	if(		nIndex == IDX_SCROLL_TOP
		||	nIndex == IDX_SCROLL_BOTTOM
		)
	{
		HWND hWndOwn = m_hWnd;
		_StartScrolling( nIndex );
		if( ! ::IsWindow( hWndOwn ) )
			return;
	}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
}

void CExtPopupMenuWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	nFlags;
	point;
	if( _IsResizingMode() )
		return;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( (!_FindCustomizeMode()) )
		return;
int nIndex = _HitTest( point );
	if( nIndex < 0 )
		return;
	ASSERT( nIndex < ItemGetCount() );
MENUITEMDATA & mi = ItemGetInfo( nIndex );
	if( mi.IsSeparator() )
		return;
CExtCustomizeSite * pSite = _FindCustomizeSite();
	ASSERT( pSite != NULL );
CExtCustomizeCmdTreeNode * pClickedCmdNode = mi.GetCmdNode();
	if( pClickedCmdNode == NULL )
		return;
	ASSERT_VALID( pClickedCmdNode );
#if (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	if(		mi.IsSelected()
		&&	(pClickedCmdNode->GetFlags() & __ECTN_TBB_RESIZABLE) != 0
		)
	{ // if button can be resized
		int nDdWidth = PmBridge_GetPM()->GetDropDividerMerics().cx / 2;
		if( nDdWidth < 2 )
			nDdWidth = 2;
		CRect rcItem( 0, 0, 0, 0 );
		_GetItemRect( nIndex, rcItem );
		CRect rcH( rcItem );
		rcH.right = rcH.left + nDdWidth;
		int nInitialResizingStateH = 0;
		if( rcH.PtInRect(point) )
			nInitialResizingStateH = -1;
		else
		{
			rcH = rcItem;
			rcH.left = rcH.right - nDdWidth;
			if( rcH.PtInRect(point) )
				nInitialResizingStateH = 1;
		} // else from if( rcH.PtInRect(point) )
		if( nInitialResizingStateH != 0 )
		{
			int nWidthMin = 0, nWidthMax = 0;
			pSite->OnGetCmdItemMinMaxSizeH(
				NULL,
				pClickedCmdNode,
				&nWidthMin,
				&nWidthMax
				);
			CRect rcInplaceEdit =
				mi.AdjustInplaceEditRect(
					rcItem,
					OnQueryLayoutRTL()
					);
			rcInplaceEdit.top = rcItem.top;
			rcInplaceEdit.bottom = rcItem.bottom;
			INT nDD = PmBridge_GetPM()->GetDropDownButtonWidth( this );
			if( mi.IsPopup() )
				rcInplaceEdit.right += nDD;
			if( !pSite->OnCmdNodeResizeH(
					nInitialResizingStateH,
					m_hWnd,
					NULL,
					pClickedCmdNode,
					nWidthMin,
					nWidthMax,
					rcInplaceEdit // rcItem
					)
				)
				return; // resizing is canceled
//			if( nDD != 0 )
//				pClickedCmdNode->TextFieldWidthSet(
//					pClickedCmdNode->TextFieldWidthGet() - nDD
//					);
			int nSavedIndex = m_nCurIndex;
			m_nCurIndex = -1;
			_BuildItems( NULL, m_bTopLevel );
			VERIFY( UpdateFromCmdTree( m_hWndCmdReceiver, m_pNode, m_bTopLevel ) );
			_SyncItems();
			ShowWindow( SW_HIDE	);
			_RecalcLayoutImpl();
			VERIFY( SetWindowRgn( NULL, FALSE ) );
			ASSERT( !IsWindowVisible() );
			_FreeWinObjects();
			// adjust screen position
			CRect rcWnd = _CalcTrackRect();
			if( m_rgnWnd.GetSafeHandle() != NULL )
			{
				ASSERT( m_bExcludeAreaSpec );
				ASSERT( m_bCombineWithEA );
				ASSERT( m_eCombineAlign != __CMBA_NONE );
				CRgn rgnTmp;
				VERIFY( rgnTmp.CreateRectRgn(0,0,0,0) );
				rgnTmp.CopyRgn( &m_rgnWnd );
				ASSERT( rgnTmp.GetSafeHandle() != NULL );
				VERIFY(
					SetWindowRgn(
						(HRGN)rgnTmp.Detach(),
						FALSE
						)
					);
			} // if( m_rgnWnd.GetSafeHandle() != NULL )
			SetWindowPos(
				NULL,
				rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(),
				SWP_NOACTIVATE
					|SWP_NOZORDER //|SWP_NOOWNERZORDER
				);
			if( m_bScrollingAvailable )
				_RecalcLayoutImpl();
			ShowWindow( SW_SHOWNA );
			UpdateWindow();
			HWND hWndOwn = m_hWnd;
			_ItemFocusSet( nSavedIndex, FALSE, FALSE, FALSE );
			if( ! ::IsWindow( hWndOwn ) )
				return;
			ASSERT( m_pDragSrcNode == NULL );
			m_pDragSrcNode = pClickedCmdNode;
			CClientDC dc( this );
			_DoPaint( dc );
			m_pDragSrcNode = NULL;
			return;
		} // if( nInitialResizingStateH != 0 )
	} // if button can be resized
#endif // (!defined __EXT_MFC_NO_BUILTIN_TEXTFIELD)
	ASSERT( m_pDragSrcNode == NULL );
	ASSERT( m_nDragSrcIdx < 0 );
	m_pDragSrcNode = pClickedCmdNode;
	m_nDragSrcIdx = nIndex;
	m_bHelperNoRemoveSrc = false;
CRect rcItem( 0, 0, 1, 1 );
	_DoResetChainPositions();
	m_ptHelperDragStart = point;
	m_bHelperNoHideChild = true;
	m_bHelperDragOverPassed = false;
HWND hWndOwn = m_hWnd;
	_ItemFocusSet( nIndex, TRUE, TRUE, FALSE );
	if( ! ::IsWindow( hWndOwn ) )
		return;
	ASSERT( hWndOwn != NULL && ::IsWindow(hWndOwn) );
	if( ! CExtControlBar::stat_DoDragDetect( hWndOwn, point ) )
	{
		m_pDragSrcNode = NULL;
		m_bHelperNoHideChild = false;
		m_nDragSrcIdx = -1;
		m_bHelperNoRemoveSrc = false;
		m_bHelperDragOverPassed = false;
		return;
	}
	pSite->DoDragCmdNode(
		this,
		NULL,
		m_pDragSrcNode,
		rcItem
		);
	if( ! ::IsWindow(hWndOwn) )
		return;
	m_pDragSrcNode = NULL;
	m_bHelperNoHideChild = false;
	m_nDragSrcIdx = -1;
	m_bHelperNoRemoveSrc = false;
	m_bHelperDragOverPassed = false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
}

void CExtPopupMenuWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	nFlags;
	point;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( (!_FindCustomizeMode()) )
		return;
int nIndex = _HitTest( point );
	if( nIndex < 0 )
		return;
	ASSERT( nIndex < ItemGetCount() );
MENUITEMDATA & mi = ItemGetInfo( nIndex );
	if( mi.IsSeparator() )
		return;
HWND hWndOwn = m_hWnd;
	_ItemFocusSet( mi.GetIndex(), TRUE, TRUE, FALSE );
	if( ! ::IsWindow( hWndOwn ) )
		return;
CExtCustomizeCmdTreeNode * pNode = mi.GetCmdNode();
	ASSERT_VALID( pNode );
CExtCustomizeSite * pSite = _FindCustomizeSite();
	ASSERT( pSite != NULL );
CRect rcItem( 0, 0, 1, 1 );
	_DoResetChainPositions();

CExtCmdItem * pCmdItem =
	g_CmdManager->CmdGetPtr(
		g_CmdManager->ProfileNameFromWnd(m_hWnd),
		pNode->GetCmdID( false )
		);
	ASSERT( pCmdItem != NULL );
	if( !pSite->CustomizedNodeSet( pNode ) )
		return;
	{ // BLOCK
		CClientDC dc( this );
		_DoPaint( dc );
	} // BLOCK
bool bCanAddRemoveSeparator = (nIndex > 0) ? true : false;
bool bDelete = false;
	ASSERT( hWndOwn != NULL && ::IsWindow(hWndOwn) );
	if( !pSite->OnCustomizeTreeNode(
			NULL,
			pNode,
			pCmdItem,
			bDelete,
			bCanAddRemoveSeparator,
			this
			)
		)
	{
		if( pSite->CustomizedNodeGet() == pNode )
		{
			VERIFY( pSite->CustomizedNodeSet( NULL ) );
		}
		if(		hWndOwn == NULL
			||	(! ::IsWindow(hWndOwn) )
			)
			return;
		CClientDC dc( this );
		_DoPaint( dc );
		return;
	}
	VERIFY( pSite->CustomizedNodeSet( NULL ) );

	if( !::IsWindow(hWndOwn) )
		return;
	
	m_nCurIndex = -1;
	_BuildItems( NULL, m_bTopLevel );
	if( bDelete )
	{
		ASSERT_VALID( pNode->GetParentNode() );
		int nOwnNodeIndex = pNode->GetOwnIndex();
		if( nOwnNodeIndex < (pNode->GetParentNode()->GetNodeCount()-1) )
			pNode->GetParentNode()->ElementAt(nOwnNodeIndex+1)
				->ModifyFlags( 0, __ECTN_GROUP_START );
		pNode->RemoveSelf( pCmdItem );
	}
	ASSERT_VALID( m_pNode );
	VERIFY( UpdateFromCmdTree( m_hWndCmdReceiver, m_pNode, m_bTopLevel ) );
	_SyncItems();
	ShowWindow( SW_HIDE	);
	_RecalcLayoutImpl();
	VERIFY( SetWindowRgn( NULL, FALSE ) );
	ASSERT( !IsWindowVisible() );
	_FreeWinObjects();
// adjust screen position
CRect rcWnd = _CalcTrackRect();
	if( m_rgnWnd.GetSafeHandle() != NULL )
	{
		ASSERT( m_bExcludeAreaSpec );
		ASSERT( m_bCombineWithEA );
		ASSERT( m_eCombineAlign != __CMBA_NONE );
		CRgn rgnTmp;
		VERIFY( rgnTmp.CreateRectRgn(0,0,0,0) );
		rgnTmp.CopyRgn( &m_rgnWnd );
		ASSERT( rgnTmp.GetSafeHandle() != NULL );
		VERIFY(
			SetWindowRgn(
				(HRGN)rgnTmp.Detach(),
				FALSE
				)
			);
	} // if( m_rgnWnd.GetSafeHandle() != NULL )
	SetWindowPos(
		NULL,
		rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(),
		SWP_NOACTIVATE
			|SWP_NOZORDER //|SWP_NOOWNERZORDER
		);
	if( m_bScrollingAvailable )
		_RecalcLayoutImpl();
	ShowWindow( SW_SHOWNA );

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
}

void CExtPopupMenuWnd::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	ASSERT_VALID( this );
HWND hWndThis = m_hWnd;
	CExtPopupBaseWnd::OnShowWindow(bShow, nStatus);
	if( ! ::IsWindow(hWndThis) )
		return;
	if(		_FindCustomizeMode()
		&&	bShow
		)
	{
		CClientDC dc( this );
		_DoPaint( dc );
	}
}

void CExtPopupMenuWnd::OnCancelMode() 
{
	ASSERT_VALID( this );
	if( CExtPopupMenuWnd::TEAROFFNOTIFICATON::g_pCurrentTEAROFFNOTIFICATON != NULL )
		return;

	if( _FindCustomizeMode() )
		return;

	TrackFlagsSet( TrackFlagsGet() | TPMX_NO_FADE_OUT_ANIMATION );

HWND hWndOwn = GetSafeHwnd();
	ASSERT( hWndOwn != NULL );
	if(		(TrackFlagsGet()&TPMX_NO_SITE) == 0
		&&	(! m_bHelperAnimationControllerDetected )
		)
	{
		CExtPopupMenuSite & _site = GetSite();
		if( _site.GetInstance() == this )
			_site.DoneInstance();
//		ASSERT( _site.IsEmpty() );
//		ASSERT( !_site.IsShutdownMode() );
	}

	if(		( ! ::IsWindow( hWndOwn ) )
//		||	( ! _IsFadeOutAnimation() )
		)
	{
		CExtToolControlBar::_CloseCustomMenusAll();
		if( CExtToolControlBar::g_bMenuTracking )
			CExtToolControlBar::_CloseTrackingMenus();
	}
}

void CExtPopupMenuWnd::_FreeWinObjects()
{
	ASSERT_VALID( this );

	if( m_rgnWnd.GetSafeHandle() != NULL )
	{
		VERIFY(
			m_rgnWnd.DeleteObject()
			);
	}
	m_ShadowMain.Destroy();
	m_ShadowCMBA.Destroy();
	_SurfacesDelete();
	if( GetSafeHwnd() == NULL )
		return;
	KillTimer(ID_TIMER_ANIMATION);
	//KillTimer(ID_TIMER_DELAY_SHOW);
	//KillTimer(ID_TIMER_ITEM_FOCUS_DELAY);
	//KillTimer(ID_TIMER_ITEM_HIDE_DELAY);
	//KillTimer(ID_TIMER_SCROLLING);
	//m_nAcceleratedPixelsPerStepCurrent = __SCROLLING_PIXEL_STEP;
	//m_nAcceleratedMilliSecondsPassed = 0;
	//KillTimer(ID_TIMER_DELAY_KILL);
	//KillTimer(ID_TIMER_LIFE_STEP);
}

void CExtPopupMenuWnd::_OnCancelMode(
	bool bNcDestroy // = false
	) 
{
	ASSERT_VALID( this );

	// update status bar message
	CExtControlBar::stat_SetMessageString(
		GetSafeOwner()
		);

bool bCusomizeMode = _FindCustomizeMode();
	if( ! bCusomizeMode )
	{
		HWND hWndOwn = m_hWnd;
		_CoolTipHide( false );
		if( ! ::IsWindow( hWndOwn ) )
			return;
		_FreeWinObjects();
		_ReleaseCapture();
	} // if( !bCusomizeMode )

INT iter = 0;
	for( ; iter < m_items_all.GetSize(); iter++ )
	{
		MENUITEMDATA & mi = ItemGetInfo( iter );
		mi.SetSelected( false );

		if( mi.IsPopup() )
		{
			if( mi.GetPopup()->GetSafeHwnd() != NULL )
				mi.GetPopup()->_OnCancelMode();
		}
	}
	m_nCurIndex = m_nLastMousePick = IDX_NOTHING;

	if( bNcDestroy )
		return;

HWND hWndOwn = GetSafeHwnd();
	if( ! ::IsWindow( hWndOwn ) )
		return;
//	if( ( GetStyle() & WS_VISIBLE ) == 0 )
//		return;

bool bFadeOut = _IsFadeOutAnimation();
bool bTopLevel = m_bTopLevel;
	if( ! bFadeOut )
	{
		if(		m_ctrlShadow.GetSafeHwnd() != NULL
			&&	(m_ctrlShadow.GetStyle()&WS_VISIBLE)
			)
			m_ctrlShadow.ShowWindow( SW_HIDE );
		ShowWindow( SW_HIDE );
	}
	if( ! bCusomizeMode )
	{
		HWND hWndOwn = m_hWnd;
		_ItemFocusDelay();
		if( ! ::IsWindow( hWndOwn ) )
			return;
//		INT nSaved = m_nFadeOutAnimationStepCount;
//		m_nFadeOutAnimationStepCount = -1;
		DestroyWindow();
//		m_nFadeOutAnimationStepCount = nSaved;
	} // if( ! bCusomizeMode )

	if(		CExtToolControlBar::g_bMenuTracking
		&&	bTopLevel
		&&	( ! bCusomizeMode )
		&&	( ! bFadeOut )
		)
	{
		CExtToolControlBar::_CloseCustomMenusAll();
		CExtToolControlBar::_CloseTrackingMenus();
	}
}

const UINT CExtPopupMenuWnd::g_nMsgRibbonFileMenuButtonInvocation =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgRibbonFileMenuButtonInvocation")
		);

CExtPopupMenuWnd::RIBBONFILEMENUBUTTONINVOCATION::RIBBONFILEMENUBUTTONINVOCATION(
	DWORD dwButtonType,
	CExtPopupMenuWnd * pPopup
	)
	: m_dwButtonType( dwButtonType )
	, m_hWndTarget1( NULL )
	, m_hWndTarget2( NULL )
	, m_bQueryHandled( false )
{
	ASSERT_VALID( pPopup );
	m_hWndTarget1 = pPopup->GetCmdReceiverHWND();
	m_hWndTarget2 = pPopup->m_hWndNotifyMenuClosed;
}

bool CExtPopupMenuWnd::RIBBONFILEMENUBUTTONINVOCATION::Notify()
{
	m_bQueryHandled = false;
	if(		m_hWndTarget1 != NULL
		&&	::IsWindow( m_hWndTarget1 )
		)
	{
		::SendMessage(
			m_hWndTarget1,
			CExtPopupMenuWnd::g_nMsgRibbonFileMenuButtonInvocation,
			*this,
			0L
			);
		if( m_bQueryHandled )
			return true;
	}
	if(		m_hWndTarget2 != NULL
		&&	::IsWindow( m_hWndTarget2 )
		)
	{
		::SendMessage(
			m_hWndTarget2,
			CExtPopupMenuWnd::g_nMsgRibbonFileMenuButtonInvocation,
			*this,
			0L
			);
		if( m_bQueryHandled )
			return true;
	}
	return false;
}

CExtPopupMenuWnd::RIBBONFILEMENUBUTTONINVOCATION::operator WPARAM() const
{
	return WPARAM( this );
}

CExtPopupMenuWnd::RIBBONFILEMENUBUTTONINVOCATION * CExtPopupMenuWnd::RIBBONFILEMENUBUTTONINVOCATION::FromWPARAM( WPARAM wParam )
{
RIBBONFILEMENUBUTTONINVOCATION * pICN =
			reinterpret_cast < RIBBONFILEMENUBUTTONINVOCATION * > ( wParam );
	ASSERT( pICN != NULL );
	return pICN;
}

const UINT CExtPopupMenuWnd::g_nMsgRibbonFileMenuButtonQuery =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgRibbonFileMenuButtonQuery")
		);

CExtPopupMenuWnd::RIBBONFILEMENUBUTTONQUERY::RIBBONFILEMENUBUTTONQUERY(
	CExtPopupMenuWnd * pPopup,
	DWORD dwButtonType,
	CDC * pDC // = NULL
	)
	: m_pPopup( pPopup )
	, m_dwButtonType( dwButtonType )
	, m_pDC( pDC )
	, m_nMeasuredWidth( 0 )
	, m_rcButton( 0, 0, 0, 0 )
	, m_bHover( false )
	, m_bPressed( false )
	, m_bQueryHandled( false )
{
}

bool CExtPopupMenuWnd::RIBBONFILEMENUBUTTONQUERY::Notify()
{
	if( m_pPopup == NULL )
		return false;
	ASSERT_VALID( m_pPopup );
	m_bQueryHandled = false;
HWND hWndTarget = m_pPopup->GetCmdReceiverHWND();
	if(		hWndTarget == NULL
		||	(! ::IsWindow( hWndTarget ) )
		)
	{
		::SendMessage(
			hWndTarget,
			CExtPopupMenuWnd::g_nMsgRibbonFileMenuButtonQuery,
			*this,
			0L
			);
		if( m_bQueryHandled )
			return true;
	}
	hWndTarget = m_pPopup->m_hWndNotifyMenuClosed;
	if(		hWndTarget == NULL
		||	(! ::IsWindow( hWndTarget ) )
		)
		return false;
	::SendMessage(
		hWndTarget,
		CExtPopupMenuWnd::g_nMsgRibbonFileMenuButtonQuery,
		*this,
		0L
		);
	return m_bQueryHandled;
}

CExtPopupMenuWnd::RIBBONFILEMENUBUTTONQUERY::operator WPARAM() const
{
	return WPARAM( this );
}

CExtPopupMenuWnd::RIBBONFILEMENUBUTTONQUERY * CExtPopupMenuWnd::RIBBONFILEMENUBUTTONQUERY::FromWPARAM( WPARAM wParam )
{
RIBBONFILEMENUBUTTONQUERY * pICN =
			reinterpret_cast < RIBBONFILEMENUBUTTONQUERY * > ( wParam );
	ASSERT( pICN != NULL );
	return pICN;
}

const UINT CExtPopupMenuWnd::g_nMsgItemCoveringNotification =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgItemCoveringNotification")
		);
const UINT CExtPopupMenuWnd::g_nMsgTearOff =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgTearOff")
		);
const UINT CExtPopupMenuWnd::g_nMsgCustomFloatPos =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgCustomFloatPos")
		);

CExtPopupMenuWnd::ITEMCOVERINGNOTIFICATON::ITEMCOVERINGNOTIFICATON(
	CExtPopupMenuWnd * pPopup,
	CExtBarButton * pTBB,
	CExtPopupMenuWnd::ITEMCOVERINGNOTIFICATON::eICN_t eICN,
	INT nPopupItemIndex, // = -1
	bool bEnableDropChild, // = false
	bool bSelectAnyInChild // = false
	)
	: m_pPopup( pPopup )
	, m_pTBB( pTBB )
	, m_nPopupItemIndex( nPopupItemIndex )
	, m_eICN( eICN )
	, m_bEnableDropChild( bEnableDropChild )
	, m_bSelectAnyInChild( bSelectAnyInChild )
	, m_bSuspendTips( false )
{
#ifdef _DEBUG
	if( m_pPopup != NULL )
	{
		ASSERT_VALID( m_pPopup );
		ASSERT( m_pTBB == NULL );
		INT nPopupItemCount = m_pPopup->ItemGetCount();
		ASSERT( 0 <= m_nPopupItemIndex && m_nPopupItemIndex <= nPopupItemCount );
		ASSERT(
				m_eICN == __EICN_CANCEL
			||	m_eICN == __EICN_SET
			||	m_eICN == __EICN_DELAY
			);
	} // if( m_pPopup != NULL )
	else
	{
		ASSERT( m_pPopup == NULL );
		ASSERT_VALID( m_pTBB );
		ASSERT( m_nPopupItemIndex == -1 );
		ASSERT( m_bEnableDropChild == false );
		ASSERT( m_bSelectAnyInChild == false );
		ASSERT(
				m_eICN == __EICN_CANCEL
			||	m_eICN == __EICN_SET
			);
		CExtToolControlBar * pToolBar = m_pTBB->GetBar();
		ASSERT_VALID( pToolBar );
	} // else from if( m_pPopup != NULL )
#endif // _DEBUG
}

bool CExtPopupMenuWnd::ITEMCOVERINGNOTIFICATON::Notify()
{
	if( m_pPopup != NULL )
	{
		ASSERT_VALID( m_pPopup );
		ASSERT( m_pTBB == NULL );
#ifdef _DEBUG
		INT nPopupItemCount = m_pPopup->ItemGetCount();
		ASSERT( 0 <= m_nPopupItemIndex && m_nPopupItemIndex <= nPopupItemCount );
#endif // _DEBUG
		ASSERT(
				m_eICN == __EICN_CANCEL
			||	m_eICN == __EICN_SET
			||	m_eICN == __EICN_DELAY
			);
		HWND hWndTarget = m_pPopup->GetCmdReceiverHWND();
		if(		hWndTarget == NULL
			||	(! ::IsWindow( hWndTarget ) )
			)
			return false;
		if(		m_pPopup->_FindHelpMode()
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
			||	m_pPopup->_FindCustomizeMode()
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
			)
			return false;
//
//		CWnd * pWndTarget = CWnd::FromHandle( hWndTarget );
//		CString strTrace;
//		strTrace.Format(
//			"   popup menu item covering notification to \"%s\" class, %d - code, %d - item index, id=%d\n",
//			pWndTarget->GetRuntimeClass()->m_lpszClassName,
//			int(m_eICN),
//			m_nPopupItemIndex,
//			m_pPopup->ItemGetInfo(m_nPopupItemIndex).GetCmdID()
//			);
//		TRACE( LPCSTR(strTrace) );
//
		::SendMessage(
			hWndTarget,
			CExtPopupMenuWnd::g_nMsgItemCoveringNotification,
			*this,
			0L
			);
		return true;
	} // if( m_pPopup != NULL )
	else
	{
		ASSERT( m_pPopup == NULL );
		ASSERT_VALID( m_pTBB );
		ASSERT( m_nPopupItemIndex == -1 );
		ASSERT( m_bEnableDropChild == false );
		ASSERT( m_bSelectAnyInChild == false );
		ASSERT(
				m_eICN == __EICN_CANCEL
			||	m_eICN == __EICN_SET
			);
		CExtToolControlBar * pToolBar = m_pTBB->GetBar();
		ASSERT_VALID( pToolBar );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		if( pToolBar->_IsDockSiteCustomizeMode() )
			return false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		HWND hWndTarget = NULL;
		if( pToolBar->m_pDockSite != NULL )
		{
			ASSERT_VALID( pToolBar->m_pDockSite );
			hWndTarget = pToolBar->m_pDockSite->GetSafeHwnd();
			if(		hWndTarget != NULL
				&&	pToolBar->m_pDockSite->m_bHelpMode
				)
				return false;
		}
		if( hWndTarget == NULL )
			hWndTarget = pToolBar->GetParent()->GetSafeHwnd();
		if(		hWndTarget == NULL
			||	(! ::IsWindow( hWndTarget ) )
			)
			return false;
//
//		CWnd * pWndTarget = CWnd::FromHandle( hWndTarget );
//		CString strTrace;
//		strTrace.Format(
//			"   toolbar item covering notification: \"%s\" class, %d - code, id=%d\n",
//			pWndTarget->GetRuntimeClass()->m_lpszClassName,
//			int(m_eICN),
//			m_pTBB->GetCmdID( true )
//			);
//		TRACE( LPCSTR(strTrace) );
//
		::SendMessage(
			hWndTarget,
			CExtPopupMenuWnd::g_nMsgItemCoveringNotification,
			*this,
			0L
			);
		return true;
	} // else from if( m_pPopup != NULL )
}

CExtPopupMenuWnd::ITEMCOVERINGNOTIFICATON::operator WPARAM() const
{
	return WPARAM( this );
}

CExtPopupMenuWnd::ITEMCOVERINGNOTIFICATON *
	CExtPopupMenuWnd::ITEMCOVERINGNOTIFICATON::FromWPARAM( WPARAM wParam )
{
ITEMCOVERINGNOTIFICATON * pICN =
			reinterpret_cast < ITEMCOVERINGNOTIFICATON * > ( wParam );
	ASSERT( pICN != NULL );
	return pICN;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

bool CExtPopupMenuWnd::_KeyTipsEnabledGet() const
{
	ASSERT_VALID( this );
	if( ( TrackFlagsGet() & TPMX_RIBBON_KEYTIPS ) != 0 )
		return true;
const CExtPopupMenuWnd * pParent = GetParentMenuWnd();
	if( pParent != NULL )
		return pParent->_KeyTipsEnabledGet();
	return false;
}

void CExtPopupMenuWnd::_KeyTipsShow( bool bShow )
{
	ASSERT_VALID( this );
	if( ! _KeyTipsEnabledGet() )
		return;
	if( _FindCustomizeMode() )
		return;
	if( _FindHelpMode() )
		return;
HDWP hPassiveModeDWP = NULL;
	if(		bShow
//		&&	g_PaintManager.m_bIsWinVistaOrLater
//		&&	g_PaintManager.m_DWM.IsCompositionEnabled()
		)
		hPassiveModeDWP = ::BeginDeferWindowPos( 64 );
visible_items_t v;
	{ // block for DC
		CWindowDC dcDesktop( NULL );
		_GetVisibleItems( dcDesktop.GetSafeHdc(), v );
	} // block for DC
INT vis_iter, vis_count = INT( v.GetSize() );
	for( vis_iter = 0; vis_iter < vis_count; vis_iter++ )
	{
		VisibleItemDefinition_t & vi = v[ vis_iter ];
		ASSERT(
				vi.m_nIndex >= 0
			&&	vi.m_nIndex < m_items_all.GetSize()
			);
		ASSERT( !vi.m_rcItem.IsRectEmpty() );
		ASSERT( !vi.m_rcItemWithIndents.IsRectEmpty() );
		MENUITEMDATA & mi = ItemGetInfo( vi.m_nIndex );
		ASSERT( mi.IsDisplayed() );
		ASSERT( mi.GetCmdID() == vi.m_nHelperCmdID );
		mi.KeyTipTrackingQuery( bShow, &m_keyTipChain, hPassiveModeDWP );
	} // for( vis_iter = 0; vis_iter < vis_count; vis_iter++ )
	if( ! bShow )
		m_keyTipChain.KeyCodeRemoveAll();
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
DWORD dwTrackFlags = TrackFlagsGet();
	if(		(dwTrackFlags&TPMX_RIBBON_OPTIONS_BUTTON) != 0
		&&	m_keyTipOptionsButton.KeyCodeGetCount() > 0
		)
	{
		if( bShow )
		{
			if( m_keyTipOptionsButton.KeyCodeGetCount() > 0 )
			{
				if( m_wndKeyTipOptionsButton.GetSafeHwnd() == NULL )
				{
					CExtSafeString strKeyTipText = m_keyTipOptionsButton.GetTipText();
					ASSERT( ! strKeyTipText.IsEmpty() );
					CRect rc;
					_GetItemRect( IDX_RIBBON_FILE_MENU_OPTIONS_BUTTON, rc, false );
					ClientToScreen( &rc );
					CPoint ptGuideLines(
						rc.left + ( rc.Width() / 2 ),
						rc.bottom
						);
					if(	m_wndKeyTipOptionsButton.Create(
							this,
							ptGuideLines,
							strKeyTipText,
							true,
							hPassiveModeDWP
							)
						)
						m_wndKeyTipOptionsButton.ShowWindow( SW_SHOWNOACTIVATE );
				}
				else
					m_wndKeyTipOptionsButton.ShowWindow( SW_SHOWNOACTIVATE );
			}
		}
		else
			m_wndKeyTipOptionsButton.Hide();
	}
	if(		(dwTrackFlags&TPMX_RIBBON_EXIT_BUTTON) != 0
		&&	m_keyTipExitButton.KeyCodeGetCount() > 0
		)
	{
		if( bShow )
		{
			if( m_keyTipExitButton.KeyCodeGetCount() > 0 )
			{
				if( m_wndKeyTipExitButton.GetSafeHwnd() == NULL )
				{
					CExtSafeString strKeyTipText = m_keyTipExitButton.GetTipText();
					ASSERT( ! strKeyTipText.IsEmpty() );
					CRect rc;
					_GetItemRect( IDX_RIBBON_FILE_MENU_EXIT_BUTTON, rc, false );
					ClientToScreen( &rc );
					CPoint ptGuideLines(
						rc.left + ( rc.Width() / 2 ),
						rc.bottom
						);
					if(	m_wndKeyTipExitButton.Create(
							this,
							ptGuideLines,
							strKeyTipText,
							true,
							hPassiveModeDWP
							)
						)
						m_wndKeyTipExitButton.ShowWindow( SW_SHOWNOACTIVATE );
				}
				else
					m_wndKeyTipExitButton.ShowWindow( SW_SHOWNOACTIVATE );
			}
		}
		else
			m_wndKeyTipExitButton.Hide();
	}
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( hPassiveModeDWP != NULL )
	{
		::EndDeferWindowPos( hPassiveModeDWP );
		CExtPaintManager::stat_PassPaintMessages();
	}
	if( bShow )
		CExtPopupMenuTipWnd::UpdateDelayedLayeredBehaviorAll();
}

bool CExtPopupMenuWnd::_KeyTipsTranslate(
	DWORD dwKeyCode,
	INT * p_nIndex // = NULL
	)
{
	ASSERT_VALID( this );
	if( p_nIndex != NULL )
		(*p_nIndex) = -1;
HWND hWndOwn = GetSafeHwnd();
	if(		hWndOwn == NULL
		||	( ! ::IsWindow( hWndOwn ) )
		)
		return false;
	if( ! _KeyTipsEnabledGet() )
		return false;
	if( _GetCapture() != this )
		return false;
	if( _FindCustomizeMode() )
		return false;
	if( _FindHelpMode() )
		return false;
visible_items_t v;
	{ // block for DC
		CWindowDC dcDesktop( NULL );
		_GetVisibleItems( dcDesktop.GetSafeHdc(), v );
	} // block for DC
INT vis_iter, vis_count = INT( v.GetSize() );
	for( vis_iter = 0; vis_iter < vis_count; vis_iter++ )
	{
		VisibleItemDefinition_t & vi = v[ vis_iter ];
		ASSERT(
				vi.m_nIndex >= 0
			&&	vi.m_nIndex < m_items_all.GetSize()
			);
		ASSERT( !vi.m_rcItem.IsRectEmpty() );
		ASSERT( !vi.m_rcItemWithIndents.IsRectEmpty() );
		MENUITEMDATA & mi = ItemGetInfo( vi.m_nIndex );
		ASSERT( mi.IsDisplayed() );
		ASSERT( mi.GetCmdID() == vi.m_nHelperCmdID );
		INT nIndexOf = ItemGetIndexOf( &mi );
		if( mi.KeyTipTranslate( dwKeyCode, &m_keyTipChain ) )
		{
			if( p_nIndex != NULL )
				(*p_nIndex) = nIndexOf;
			return true;
		}
	} // for( vis_iter = 0; vis_iter < vis_count; vis_iter++ )
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
INT nKTI, nKeyCodeCountChain = m_keyTipChain.KeyCodeGetCount();
	for( nKTI = 0; nKTI < 2; nKTI++ )
	{
		CExtPopupKeyTipWnd & wndKeyTip = ( nKTI == 0 ) ? m_wndKeyTipOptionsButton : m_wndKeyTipExitButton;
		CExtCustomizeCmdKeyTip & keyTipInfo =  ( nKTI == 0 ) ? m_keyTipOptionsButton : m_keyTipExitButton;
		if(		 wndKeyTip.GetSafeHwnd() == NULL
			||	( wndKeyTip.GetStyle() & WS_VISIBLE ) == 0
			)
			continue;
		if( ! keyTipInfo.CompareWidthChain( m_keyTipChain ) )
			continue;
		INT nKeyCodeCountOwn = keyTipInfo.KeyCodeGetCount();
		ASSERT( nKeyCodeCountOwn > nKeyCodeCountChain );
		DWORD dwKeyCodeOwn = keyTipInfo.KeyCodeGetAt( nKeyCodeCountChain );
		if( dwKeyCodeOwn != dwKeyCode )
			continue;
		m_keyTipChain.KeyCodeAdd( dwKeyCode );
		nKeyCodeCountChain++;
		if( nKeyCodeCountOwn > nKeyCodeCountChain )
			continue;
		INT nHitTest = ( nKTI == 0 ) ? IDX_RIBBON_FILE_MENU_OPTIONS_BUTTON : IDX_RIBBON_FILE_MENU_EXIT_BUTTON;
		DWORD dwButtonType = 0;
		switch( nHitTest )
		{
		case IDX_RIBBON_FILE_MENU_OPTIONS_BUTTON:
			dwButtonType = TPMX_RIBBON_OPTIONS_BUTTON;
		break;
		case IDX_RIBBON_FILE_MENU_EXIT_BUTTON:
			dwButtonType = TPMX_RIBBON_EXIT_BUTTON;
		break;
		} // switch( nHitTest )
		RIBBONFILEMENUBUTTONINVOCATION _rfmbi( dwButtonType, this );
		m_nCurIndex = IDX_NOTHING;
		_SetCapture();
		if( nHitTest == IDX_RIBBON_FILE_MENU_EXIT_BUTTON )
			m_dwTrackFlags |= TPMX_NO_FADE_OUT_ANIMATION;
		DestroyWindow();
		_rfmbi.Notify();
		return true;
	} // for( nKTI = 0; nKTI < 2; nKTI++ )
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
	return false;
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

void CExtPopupMenuWnd::_ItemFocusSet(
	int nCurIndex,
	BOOL bEnableDropChild,
	BOOL bRepaint,
	BOOL bSelectAnyInChild, // = TRUE
	BOOL bDelayHiding // = FALSE // delay hiding previous
	)
{
	ASSERT_VALID( this );

HWND hWndOwn = m_hWnd;
ItemFocusTracker_t _ift( this );
	if( m_nCurIndex != nCurIndex )
	{
		_ItemFocusCancel( FALSE, TRUE, bDelayHiding );
		//ASSERT( m_nCurIndex < 0 );
		m_nCurIndex = nCurIndex;
		if( m_nCurIndex < 0 )
			return;
		ITEMCOVERINGNOTIFICATON _icn(
			this,
			NULL,
			ITEMCOVERINGNOTIFICATON::__EICN_SET,
			m_nCurIndex,
			bEnableDropChild ? true : false,
			bSelectAnyInChild ? true : false
			);
		_icn.Notify();
		m_bSuspendTips = _icn.m_bSuspendTips;
		if( ! ::IsWindow( hWndOwn ) )
			return;
	}

	if( (! bDelayHiding) && bEnableDropChild && nCurIndex >= 0 )
	{
		int nCount = ItemGetCount();
		for( int i = 0; i < nCount; i++ )
		{
			if( i == nCurIndex )
				continue;
			MENUITEMDATA & mii_child = ItemGetInfo( i );
			if( i == nCount )
				continue;
			if( ! mii_child.IsPopup() )
				continue;
			CExtPopupMenuWnd * pChildPopup = mii_child.GetPopup();
			ASSERT_VALID( pChildPopup );
			if( pChildPopup->GetSafeHwnd() == NULL )
				continue;
			INT nSaved = pChildPopup->m_nFadeOutAnimationStepCount;
			pChildPopup->m_nFadeOutAnimationStepCount = -1;
			pChildPopup->_OnCancelMode();
			pChildPopup->m_nFadeOutAnimationStepCount = nSaved;
			if( ! ::IsWindow( hWndOwn ) )
				return;
		}
		KillTimer( ID_TIMER_ITEM_FOCUS_DELAY );
	}

bool bResetMessageString = true;
bool bWasSet = false;
MENUITEMDATA & mi = ItemGetInfo(m_nCurIndex);
	if( mi.IsEnabled() && mi.IsDisplayed() )
	{
		mi.SetSelected( true );
		bWasSet = true;

		// adjust scrolling
		CRect rcMB = OnQueryMenuBorderMetrics();
		if( m_bScrollingAvailable )
		{
			bool bResetRectsCache = false;
			int nMetric;
			int nMaxScrollPos = _GetMaxScrollPos();
			CRect rcItem,rcClient,
				rcScrollBtnUp,rcScrollBtnDown;
			_GetItemRect( IDX_SCROLL_TOP, rcScrollBtnUp );
			_GetItemRect( IDX_SCROLL_BOTTOM, rcScrollBtnDown );
			_GetClientRect( rcClient );
			_GetItemRect( m_nCurIndex, rcItem );

			nMetric = rcClient.bottom;
			if( m_nDyScrollOffset != nMaxScrollPos )
			{
				nMetric -= rcScrollBtnDown.Height();
				if( m_bExpandAvailable )
				{
					CRect rcExpand;
					_GetItemRect( IDX_EXPAND, rcExpand );
					nMetric -= rcExpand.Height();
				}
			}
			if( rcItem.bottom >= nMetric )
			{
				m_nDyScrollOffset -=
					rcItem.bottom
					- nMetric
					+ rcMB.top + rcMB.bottom
					;
				bResetRectsCache = true;
			}

			nMetric = rcClient.top;
			if( m_nDyScrollOffset != 0 )
			{
				nMetric += rcScrollBtnUp.Height();
			}
			if( rcItem.top <= nMetric )
			{
				m_nDyScrollOffset +=
					nMetric
					- rcItem.top
					+ rcMB.top + rcMB.bottom
					;
				bResetRectsCache = true;
			}

			if( m_nDyScrollOffset > 0 )
			{
				m_nDyScrollOffset = 0;
				bResetRectsCache = true;
			}
			else
			{
				if( m_nDyScrollOffset < nMaxScrollPos )
				{
					m_nDyScrollOffset = nMaxScrollPos;
					bResetRectsCache = true;
				}
			}
			if( bResetRectsCache )
				_CachedRectsEmpty();
		} // if( m_bScrollingAvailable )
	} // if( mi.IsEnabled() && mi.IsDisplayed() )
bool bSetStatusMsg = bWasSet;
	if( (! bSetStatusMsg) && (! mi.IsEnabled() ) && mi.IsDisplayed() && g_bShowStatusTipsOverDisabledItems )
		bSetStatusMsg = true;

	// update status bar message
CWnd * pWndOwner = GetOwner();
	if( pWndOwner != NULL )
	{
		ASSERT_VALID( pWndOwner );
		if( bSetStatusMsg && (!mi.IsPopup()) && (!mi.IsSeparator()) && (!mi.IsNoCmdUI()) )
		{
			UINT nCmdID = mi.GetCmdID();
			if( CExtCmdManager::IsCommand( nCmdID ) )
			{
				UINT nCmdIDToSend = nCmdID;
				bool bAbandonSend = false;
				if( CExtCmdManager::IsSystemCommand( nCmdID ) )
				{
					switch( nCmdID )
					{
					case SC_CLOSE:
						nCmdIDToSend = AFX_IDS_SCCLOSE;
					break;
					case SC_SIZE:
						nCmdIDToSend = AFX_IDS_SCSIZE;
					break;
					case SC_MOVE:
						nCmdIDToSend = AFX_IDS_SCMOVE;
					break;
					case SC_MINIMIZE:
						nCmdIDToSend = AFX_IDS_SCMINIMIZE;
					break;
					case SC_MAXIMIZE:
						nCmdIDToSend = AFX_IDS_SCMAXIMIZE;
					break;
					case SC_RESTORE:
						nCmdIDToSend = AFX_IDS_SCRESTORE;
					break;
					case SC_NEXTWINDOW:
						nCmdIDToSend = AFX_IDS_SCNEXTWINDOW;
					break;
					case SC_PREVWINDOW:
						nCmdIDToSend = AFX_IDS_SCPREVWINDOW;
					break;
					case SC_TASKLIST:
						nCmdIDToSend = AFX_IDS_SCTASKLIST;
					break;
					default:
						bAbandonSend = true;
					break;
					} // switch( nCmdID )
				} // if( CExtCmdManager::IsSystemCommand( nCmdID ) )
				if( !bAbandonSend )
				{
					CExtControlBar::stat_SetMessageString(
						pWndOwner,
						nCmdIDToSend
						);
					bResetMessageString = false;
				} // if( !bAbandonSend )
			} // if( CExtCmdManager::IsCommand( nCmdID ) )
		} // if( bSetStatusMsg && (!mi.IsPopup()) && (!mi.IsSeparator()) && (!mi.IsNoCmdUI()) )
		if( bResetMessageString )
			CExtControlBar::stat_SetMessageString( pWndOwner );
	} // if( bWasSet )
	
	if( bRepaint && ::IsWindowVisible( GetSafeHwnd() ) )
	{
		CClientDC dc( this );
		_DoPaint( dc );
	} // if( bRepaint && ::IsWindowVisible( GetSafeHwnd() ) )

	if( mi.IsPopup() )
	{
		if( mi.IsInplaceEdit() )
		{
			HWND hWndInplaceEdit = mi.GetInplaceEditPtr()->GetSafeHwnd();
			if(		hWndInplaceEdit != NULL
				&&	::IsWindow( hWndInplaceEdit )
				&&	(::__EXT_MFC_GetWindowLong(hWndInplaceEdit,GWL_STYLE)&WS_VISIBLE) != 0
				)
				bEnableDropChild = false;
		} // if( mi.IsInplaceEdit() )
	} // if( mi.IsPopup() )
	else
		bEnableDropChild = false;
bool bDelayExpanding = true;
	if(		bWasSet
		&&	bEnableDropChild
		&&	mi.GetPopup()->_CanStartLevelTracking()
		)
	{ // if new popup tracking
		if( !_FindCustomizeMode() )
			_ReleaseCapture();
		CRect rectItem;
		_GetItemRect(
			m_nCurIndex,
			rectItem
			);
		rectItem.left -= m_nLeftAreaWidth;
		CPoint point;
		point.x = rectItem.right;
		point.y = rectItem.top,
		ClientToScreen(&point);
		ClientToScreen( &rectItem );
		ASSERT( mi.GetPopup()->m_pWndParentMenu == this );
		bool bDisplayedSuccessfully = false;
		if( mi.GetPopup()->GetSafeHwnd() == NULL )
		{
			DWORD dwChildTrackFlags =
				mi.IsChildCombine() ? TPMX_COMBINE_DEFAULT : 0;
			dwChildTrackFlags |=
				m_dwTrackFlags &
					(	TPMX_NO_SHADOWS
					|	TPMX_NO_DYNAMIC_SHADOWS
					|	TPMX_NO_CMD_UI
					|	TPMX_NO_WM_COMMAND
					|	TPMX_RIBBON_MODE
					);
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
			_KeyTipsShow( false );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
			bDisplayedSuccessfully = 
				mi.GetPopup()->
					_TrackPopupMenu(
						dwChildTrackFlags,
						point.x,point.y,
						&rectItem,
						(LPVOID)(__EXT_MFC_INT_PTR)m_nCurIndex,
						stat_paint_combined_menu_toolbtn
						) ? true : false;
		} // if( mi.GetPopup()->GetSafeHwnd() == NULL )
		else
		{
			bDisplayedSuccessfully = true;
			if( !mi.GetPopup()->IsWindowVisible() )
			{
				mi.GetPopup()->SetWindowPos(
					&CWnd::wndTopMost, 0, 0, 0, 0,
					SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE
						|SWP_NOREDRAW
						|SWP_SHOWWINDOW
					);
				CClientDC dc( mi.GetPopup() );
				mi.GetPopup()->_DoPaint( dc );
			} // if( !mi.GetPopup()->IsWindowVisible() )
		} // else from
		if( bDisplayedSuccessfully && bSelectAnyInChild )
		{ // if track-popup succeeded and we need to select any child item
			int nCountOfPopupItems = (int)
				mi.GetPopup()->m_items_all.GetSize();
			if( nCountOfPopupItems > 0 )
			{
				int nIndex = mi.GetPopup()->_GetNextItem(__NI_ANY);
				if( nIndex >= 0 )
				{
					mi.GetPopup()->_ItemFocusSet(
						nIndex,
						FALSE,
						( GetSite().GetAnimated() == NULL ) ?
							TRUE : FALSE
						);
					if( ! ::IsWindow( hWndOwn ) )
						return;
				}
			}
		} // if track-popup succeeded and we need to select any child item
		if( bDisplayedSuccessfully /*&& mi.IsInplaceEdit()*/ )
		{
			CClientDC dc( this );
			_DoPaint( dc );
			bDelayExpanding = false;
		}
	} // if new popup tracking

	_ift.FocusChangingUnRegister();

	KillTimer( ID_TIMER_AUTO_EXPAND );
	if( bDelayExpanding )
	{
		if( m_bTopLevel && (! m_bExpandWasPressed ) && m_bExpandAvailable && g_nAutoExpandTime > 0 )
			SetTimer( ID_TIMER_AUTO_EXPAND, g_nAutoExpandTime, NULL );
	}
}

bool CExtPopupMenuWnd::_CoolTipIsVisible()
{
	ASSERT_VALID( this );
	if( _FindCustomizeMode() )
		return false;
	if( GetSafeHwnd() == NULL )
		return false;
	if( _IsRibbonMode() )
		return false;
	if( CExtPopupMenuSite::g_DefPopupMenuSite.GetCapture() != this )
		return false;
CExtPopupMenuTipWnd & _tipWnd = GetTip();
HWND hWndToolTip = _tipWnd.GetSafeHwnd();
	if(		hWndToolTip != NULL
		&&	::IsWindow( hWndToolTip )
		&&	::IsWindowVisible( hWndToolTip )
		)
		return true;
	return false;
}

void CExtPopupMenuWnd::_CoolTipHide(
	bool bAdvOperation // = true
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtPopupScreenTipWnd & _wndScreenTip = GetScreenTip();
	_wndScreenTip.Hide();
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

CExtPopupMenuTipWnd & _tipWnd = GetTip();
HWND hWndToolTip = _tipWnd.GetSafeHwnd();
	if(		hWndToolTip == NULL
		||	(! ::IsWindow( hWndToolTip ) )
		)
		return;
	if( bAdvOperation && ( GetSite().GetAnimated() == NULL ) )
	{
		m_AnimationType = __AT_CONTENT_DISPLAY;
		HWND hWndOwn = m_hWnd;
		_StartAnimation();
		if( ! ::IsWindow( hWndOwn ) )
			return;
	}
	_tipWnd.Hide();
}

CExtPopupMenuTipWnd::e_tip_style_t CExtPopupMenuWnd::_CoolTipGetStyle() const
{
	ASSERT_VALID( this );
	if( _IsPalette() )
		return g_eTtsPaletteMenu;
	return g_eTtsClassicMenu;
}

CMap < CExtPopupMenuWnd *, CExtPopupMenuWnd *, INT, INT > CExtPopupMenuWnd::ItemFocusTracker_t::g_map;

CExtPopupMenuWnd::ItemFocusTracker_t::ItemFocusTracker_t( const CExtPopupMenuWnd & _Popup )
	: m_Popup( _Popup )
{
	ASSERT_VALID( ( &(m_Popup) ) );
	FocusChangingRegister();
}

CExtPopupMenuWnd::ItemFocusTracker_t::ItemFocusTracker_t( const CExtPopupMenuWnd * pPopup )
	: m_Popup( *pPopup )
{
	ASSERT_VALID( ( &(m_Popup) ) );
	FocusChangingRegister();
}

CExtPopupMenuWnd::ItemFocusTracker_t::~ItemFocusTracker_t()
{
	ASSERT_VALID( ( &(m_Popup) ) );
	FocusChangingUnRegister();
}

void CExtPopupMenuWnd::ItemFocusTracker_t::FocusChangingRegister()
{
	ASSERT_VALID( ( &(m_Popup) ) );
INT i = 0;
	if(	g_map.Lookup(
			const_cast < CExtPopupMenuWnd * > ( &(m_Popup) ),
			i
			)
		)
	{
		ASSERT( i >= 1 );
		i++;
	}
	else
		i = 1;
	g_map.SetAt(
		const_cast < CExtPopupMenuWnd * > ( &(m_Popup) ),
		i
		);
}

void CExtPopupMenuWnd::ItemFocusTracker_t::FocusChangingUnRegister()
{
	ASSERT_VALID( ( &(m_Popup) ) );
INT i = 0;
	if(	! g_map.Lookup(
			const_cast < CExtPopupMenuWnd * > ( &(m_Popup) ),
			i
			)
		)
		return;
	ASSERT( i >= 1 );
	i--;
	if( i == 0 )
		g_map.RemoveKey(
			const_cast < CExtPopupMenuWnd * > ( &(m_Popup) )
			);
	else
		g_map.SetAt(
			const_cast < CExtPopupMenuWnd * > ( &(m_Popup) ),
			i
			);
}

bool CExtPopupMenuWnd::ItemFocusTracker_t::FocusChangingIsRegistered()
{
	ASSERT_VALID( ( &(m_Popup) ) );
INT i = 0;
	if(	g_map.Lookup(
			const_cast < CExtPopupMenuWnd * > ( &(m_Popup) ),
			i
			)
		)
	{
		ASSERT( i >= 1 );
		return true;
	}
	else
		return false;
}

bool CExtPopupMenuWnd::ItemFocusTracker_t::FocusChangingIsInProgress( const CExtPopupMenuWnd * pPopup )
{
	if( pPopup == NULL )
		return false;
	ASSERT_VALID( pPopup );
INT i = 0;
	if( ! g_map.Lookup(
			const_cast < CExtPopupMenuWnd * > ( pPopup ),
			i
			)
		)
		return false;
	ASSERT( i >= 1 );
	return true;
}

bool CExtPopupMenuWnd::_ItemFocusChangingIsInProgress() const
{
	ASSERT_VALID( this );
	if( _CancelingGet() )
		return false;
bool bRetVal = CExtPopupMenuWnd::ItemFocusTracker_t::FocusChangingIsInProgress( this );
	return bRetVal;
}

void CExtPopupMenuWnd::_ItemFocusCancel(
	BOOL bRepaint,
	BOOL bCancelTip, // = TRUE
	BOOL bDelayHiding // = FALSE
	)
{
	ASSERT_VALID( this );

	if( !_FindCustomizeMode() )
	{
		HWND hWndOwn = m_hWnd;
		if( bCancelTip )
			_CoolTipHide();
		if( ! ::IsWindow( hWndOwn ) )
			return;
	}

	if( m_nCurIndex != IDX_NOTHING )
	{
		CRect rcCurrItem;
		_GetItemRect( m_nCurIndex, rcCurrItem );
		if( ! rcCurrItem.IsRectEmpty() )
			InvalidateRect( &rcCurrItem );
	}

ItemFocusTracker_t _ift( this );
	if( m_nCurIndex < 0 )
	{
		m_nCurIndex = IDX_NOTHING;
	} // if( m_nCurIndex < 0 )
	else
	{
		ASSERT( m_nCurIndex < m_items_all.GetSize() );
		MENUITEMDATA & mi = ItemGetInfo(m_nCurIndex);
		if( mi.IsInplaceEdit() )
		{
			mi.SetTempSelectedInplaceEditText( NULL );
			CWnd * pWndInplace = mi.GetInplaceEditPtr();
			if( pWndInplace != NULL )
			{
				ASSERT_VALID( pWndInplace );
				ASSERT( pWndInplace->GetSafeHwnd() != NULL );
				if( (pWndInplace->GetStyle() & WS_VISIBLE) != 0 )
					pWndInplace->ShowWindow( SW_HIDE );
			} // if( pWndInplace != NULL )
		}
		mi.SetSelected( false );
		ITEMCOVERINGNOTIFICATON _icn(
			this,
			NULL,
			ITEMCOVERINGNOTIFICATON::__EICN_CANCEL,
			m_nCurIndex
			); 
		m_nCurIndex = IDX_NOTHING;
		HWND hWndOwn = m_hWnd;
		_icn.Notify();
		m_bSuspendTips = _icn.m_bSuspendTips;
		if( ! ::IsWindow( hWndOwn ) )
			return;

		if( mi.IsPopup() )
		{
			CExtPopupMenuWnd * pPopup = mi.GetPopup();
			ASSERT_VALID( pPopup );
			if( bDelayHiding && (! pPopup->m_bDelayedHiding ) && pPopup->GetSafeHwnd() != NULL )
			{
				pPopup->m_bDelayedHiding = true;
				ASSERT( g_nTimeDelayedHiding > 0 );
				pPopup->SetTimer( ID_TIMER_ITEM_HIDE_DELAY, g_nTimeDelayedHiding, NULL );
			}
			else
			{
				INT nSaved = pPopup->m_nFadeOutAnimationStepCount;
				pPopup->m_nFadeOutAnimationStepCount = -1;
				pPopup->_OnCancelMode();
				pPopup->m_nFadeOutAnimationStepCount = nSaved;
				if( ! ::IsWindow( hWndOwn ) )
					return;
			}
		}
	} // else from if( m_nCurIndex < 0 )

	// update status bar message
CWnd * pWndOwner = GetOwner();
	if( pWndOwner != NULL )
		CExtControlBar::stat_SetMessageString( pWndOwner );

	if( bRepaint && ::IsWindowVisible(GetSafeHwnd()) )
	{
		CClientDC dc( this );
		_DoPaint( dc );
	}

	_ift.FocusChangingUnRegister();

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	_KeyTipsShow( true );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	KillTimer( ID_TIMER_AUTO_EXPAND );
}

bool CExtPopupMenuWnd::_CanStartLevelTracking()
{
	ASSERT_VALID( this );
	return true;
}

bool CExtPopupMenuWnd::_OnMouseWheel(
	WPARAM wParam,
	LPARAM lParam,
	bool & bNoEat
	)
{
	ASSERT_VALID( this );

	if( _IsResizingMode() )
	{
		bNoEat = false;
		return true;
	}

TranslateMouseWheelEventData_t _td( this, wParam, lParam, bNoEat );
	if( _td.Notify() )
	{
		bNoEat = _td.m_bNoEat;
		return true;
	}

	return true;
}

const UINT CExtPopupMenuWnd::g_nMsgInstantiatePopupMenu =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::g_nMsgInstantiatePopupMenu")
		);

CExtPopupMenuWnd::INSTANTIATEPOPUPMENUNOTIFICATION::INSTANTIATEPOPUPMENUNOTIFICATION(
	CRuntimeClass * pMenuRTC,
	CObject * pQuerySrc,
	LPARAM lParam
	)
	: m_pMenuRTC( pMenuRTC )
	, m_pQuerySrc( pQuerySrc )
	, m_lParam( lParam )
	, m_pCreatedPopupMenuInstance( NULL )
{
}

void CExtPopupMenuWnd::INSTANTIATEPOPUPMENUNOTIFICATION::Notify( HWND hWndNotifyTarget )
{
	ASSERT( hWndNotifyTarget != NULL && ::IsWindow( hWndNotifyTarget ) );
	::SendMessage( hWndNotifyTarget, CExtPopupMenuWnd::g_nMsgInstantiatePopupMenu, (*this), 0L );
}

CExtPopupMenuWnd::INSTANTIATEPOPUPMENUNOTIFICATION::
	operator WPARAM()
{
	return WPARAM( this );
}

CExtPopupMenuWnd::INSTANTIATEPOPUPMENUNOTIFICATION &
	CExtPopupMenuWnd::INSTANTIATEPOPUPMENUNOTIFICATION::
		FromWPARAM( WPARAM wParam )
{
INSTANTIATEPOPUPMENUNOTIFICATION * pIPMN =
			reinterpret_cast < INSTANTIATEPOPUPMENUNOTIFICATION * > ( wParam );
	ASSERT( pIPMN != NULL );
	return (*pIPMN);
}

CExtPopupMenuWnd * CExtPopupMenuWnd::InstantiatePopupMenu(
	HWND hWndNotifyTarget,
	CRuntimeClass * pMenuRTC,
	CObject * pQuerySrc,
	LPARAM lParam // = 0L
	)
{
	ASSERT( pMenuRTC != NULL );
	ASSERT( pMenuRTC->IsDerivedFrom(RUNTIME_CLASS(CExtPopupMenuWnd)) );
	if( hWndNotifyTarget != NULL && ::IsWindow( hWndNotifyTarget ) )
	{
		HWND hWnd = hWndNotifyTarget;
		for( ; hWnd != NULL; )
		{
			INSTANTIATEPOPUPMENUNOTIFICATION _ipmn( pMenuRTC, pQuerySrc, lParam );
			_ipmn.Notify( hWndNotifyTarget );
			if( _ipmn.m_pCreatedPopupMenuInstance != NULL )
			{
				ASSERT_VALID( _ipmn.m_pCreatedPopupMenuInstance );
				ASSERT( _ipmn.m_pCreatedPopupMenuInstance->IsKindOf( pMenuRTC ) );
				return _ipmn.m_pCreatedPopupMenuInstance;
			}
			__EXT_MFC_LONG_PTR dwStyle = ::__EXT_MFC_GetWindowLong( hWnd, GWL_STYLE );
			if( ( dwStyle & WS_CHILD ) == 0 )
			{
				CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
				if( pWnd != NULL && pWnd->IsKindOf( RUNTIME_CLASS(CFrameWnd) ) )
				{
					pWnd = pWnd->GetParentFrame();
					if( pWnd != NULL )
					{
						hWnd = pWnd->m_hWnd;
						continue;
					}
				}
				break;
			} // if( ( dwStyle & WS_CHILD ) == 0 )
			hWnd = ::GetParent( hWnd );
		} // for( ; hWnd != NULL; )
	} // if( hWndNotifyTarget != NULL && ::IsWindow( hWndNotifyTarget ) )
CExtPopupMenuWnd * pPopup = STATIC_DOWNCAST( CExtPopupMenuWnd, pMenuRTC->CreateObject() );
	ASSERT_VALID( pPopup );
	return pPopup;
}

CSize CExtPopupMenuWnd::ResizingMinSizeGet() const
{
	ASSERT_VALID( this );
	return m_sizeResizingMin;
}

void CExtPopupMenuWnd::ResizingMinSizeSet( CSize _size )
{
	ASSERT_VALID( this );
	m_sizeResizingMin = _size;
}

CSize CExtPopupMenuWnd::ResizingMaxSizeGet() const
{
	ASSERT_VALID( this );
	return m_sizeResizingMax;
}

void CExtPopupMenuWnd::ResizingMaxSizeSet( CSize _size )
{
	ASSERT_VALID( this );
	m_sizeResizingMax = _size;
}

bool CExtPopupMenuWnd::_IsResizingMode() const
{
	ASSERT_VALID( this );
	return m_bHelperResizingMode;
}

void CExtPopupMenuWnd::_DoResizing(
	CPoint ptScreenClick,
	bool bVerticalOnlyResizing
	)
{
	ASSERT_VALID( this );
	_ItemFocusCancel( TRUE, TRUE );
	_SetCapture();
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	_KeyTipsShow( false );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
HWND hWndOwn = GetSafeHwnd();
	ASSERT( hWndOwn != NULL && ::IsWindow( hWndOwn ) );
bool bReCreateShadow = ( m_ctrlShadow.GetSafeHwnd() != NULL ) ? true : false;
MSG msg;
CPoint ptMove;
CSize _sizeMin( ResizingMinSizeGet() ), _sizeMax( ResizingMaxSizeGet() ),
		_sizeShift, _sizeLast, _sizeCurrent, _sizeInitial, _sizeAdjust, _sizeControlAdjust,
		_sizeClientCurrent, _sizeClientInitial( m_rcClient.right - m_rcClient.left, m_rcClient.bottom - m_rcClient.top );
CRect rcWnd;
	GetWindowRect( &rcWnd );
	_sizeLast = _sizeCurrent = _sizeInitial = rcWnd.Size();
	::SetCapture( hWndOwn );
	while( ::IsWindow( hWndOwn ) && _IsResizingMode() )
	{
		::WaitMessage();
		if( ! ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
			continue;
		switch( msg.message )
		{
		case WM_LBUTTONUP:
		case WM_NCLBUTTONUP:
		case WM_MBUTTONUP:
		case WM_NCMBUTTONUP:
		case WM_RBUTTONUP:
		case WM_NCRBUTTONUP:
		case WM_KEYDOWN:
			::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE );
		case WM_CANCELMODE:
		case WM_ACTIVATEAPP:
			m_bHelperResizingMode = false;
		continue;

		case WM_MOUSEMOVE:
			{
				::PeekMessage( &msg, NULL, 0, 0, PM_REMOVE );
				if( ! ::GetCursorPos( &ptMove ) )
					continue;
				if( bVerticalOnlyResizing )
					_sizeShift.cx = 0;
				else
					_sizeShift.cx = ptMove.x - ptScreenClick.x;
				_sizeShift.cy = ptMove.y - ptScreenClick.y;
				_sizeCurrent = _sizeInitial + _sizeShift;
				_sizeAdjust = _sizeCurrent;
				_sizeCurrent.cx = max( _sizeCurrent.cx, _sizeMin.cx );
				_sizeCurrent.cy = max( _sizeCurrent.cy, _sizeMin.cy );
				_sizeCurrent.cx = min( _sizeCurrent.cx, _sizeMax.cx );
				_sizeCurrent.cy = min( _sizeCurrent.cy, _sizeMax.cy );
				if( _sizeLast == _sizeCurrent )
					continue;
				_sizeAdjust -= _sizeCurrent;
				_sizeControlAdjust = _sizeCurrent - _sizeLast;
				_sizeLast = _sizeCurrent;
				rcWnd.right = rcWnd.left + _sizeCurrent.cx;
				rcWnd.bottom = rcWnd.top + _sizeCurrent.cy;
				_sizeClientCurrent = _sizeClientInitial + _sizeShift - _sizeAdjust;
				m_rcClient.right = m_rcClient.left + _sizeClientCurrent.cx;
				m_rcClient.bottom = m_rcClient.top + _sizeClientCurrent.cy;
				_DoAdjustControlMetrics( _sizeControlAdjust );
  				if( bReCreateShadow )
					m_ctrlShadow.DestroyWindow();
				MoveWindow( &rcWnd, FALSE );
  				if( bReCreateShadow )
					m_ctrlShadow.Create( hWndOwn, PmBridge_GetPM()->GetMenuShadowSize() );
				_SyncItems();
				_RecalcLayoutImpl();
				RedrawWindow(
					NULL,
					NULL,
					RDW_INVALIDATE|RDW_ERASE
						//|RDW_UPDATENOW|RDW_ERASENOW
						|RDW_FRAME|RDW_ALLCHILDREN
					);
				CExtPaintManager::stat_PassPaintMessages();
			}
		continue;
		} // switch( msg.message )
		if( ! ::AfxGetThread() -> PumpMessage() )
		{
			m_bHelperResizingMode = false;
			break;
		}
	} // while( ::IsWindow( hWndOwn ) && _IsResizingMode() )
	::ReleaseCapture();
}

void CExtPopupMenuWnd::_DoAdjustControlMetrics(
	CSize _sizeAdjust
	)
{
	ASSERT_VALID( this );
	_sizeAdjust;
}

bool CExtPopupMenuWnd::_OnMouseMove(
	UINT nFlags,
	CPoint point,
	bool & bNoEat
	)
{
	ASSERT_VALID( this );

	if( GetSafeHwnd() == NULL )
		return false;

	if( ! m_bAnimFinished )
		return true;

	if( _IsResizingMode() )
	{
		bNoEat = true;
		return false;
	}

CExtPopupMenuSite & _site = GetSite();
	if(	_site.IsShutdownMode()
		|| _site.IsEmpty()
		|| _site.GetAnimated() != NULL
		)
		return true;

TranslateMouseMoveEventData_t _td( this, nFlags, point, bNoEat );
	if( _td.Notify() )
	{
		bNoEat = _td.m_bNoEat;
		return true;
	}

CPoint ptScreenClick( point );
	ClientToScreen( &ptScreenClick );
HWND hWndFromPoint = ::WindowFromPoint( ptScreenClick );
	if(		hWndFromPoint != NULL
		&&	(::__EXT_MFC_GetWindowLong(hWndFromPoint,GWL_STYLE)&WS_CHILD) != 0
		&&	::GetParent(hWndFromPoint) == m_hWnd
		)
	{
		CWnd * pWnd = CWnd::FromHandlePermanent( hWndFromPoint );
		if( pWnd != NULL )
		{
			int nCount = ItemGetCount();
			for( int i = 0; i < nCount; i++ )
			{
				MENUITEMDATA & mi = ItemGetInfo( i );
				if( !mi.IsInplaceEdit() )
					continue;
				if( ((LPVOID)mi.GetInplaceEditPtr()) != ((LPVOID)pWnd) )
					continue;
				if( m_nCurIndex == i )
					break;
				HWND hWndOwn = m_hWnd;
				_ItemFocusSet( i, FALSE, TRUE, TRUE );
				if( ! ::IsWindow( hWndOwn ) )
					return true;
				break;
			} // for( int i = 0; i < nCount; i++ )
		} // if( pWnd != NULL )
		bNoEat = true;
		return false;
	}
HWND hWndFocus = ::GetFocus();
	if(		hWndFocus != NULL
		&&	(::__EXT_MFC_GetWindowLong(hWndFocus,GWL_STYLE)&WS_CHILD) != 0
		&&	::GetParent(hWndFocus) == m_hWnd
		)
		return true;

	if( !_PtInWndArea(point) )
	{

		if(		m_pWndParentMenu != NULL
			&&	m_pWndParentMenu->GetSafeHwnd() != NULL
			)
		{
			ASSERT_VALID( m_pWndParentMenu );
			CPoint point = ptScreenClick;
			m_pWndParentMenu->ScreenToClient( &point );
			if( ptScreenClick == m_ptTrackWatched )
			{
				m_ptTrackWatched.x = m_ptTrackWatched.y = 0;
				return true;
			}
			HWND hWndOwn = m_hWnd;
			if( m_pWndParentMenu->_OnMouseMove(
					nFlags,
					point,
					bNoEat
					)
				)
			{
				if( ! ::IsWindow( hWndOwn ) )
					return true;
				m_nLastMousePick = IDX_NOTHING;
				if( bNoEat )
					return false;
				if(		::IsWindow(hWndOwn)
					&&	((::__EXT_MFC_GetClassLong(hWndOwn,GWL_STYLE))&WS_VISIBLE) != 0
					)
					_OnCancelMode();
				return true;
			}
			if( ! ::IsWindow( hWndOwn ) )
				return true;
		}

		if( GetSite().GetCapture() == this )
		{
			HWND hWndTest = hWndFromPoint;
			for( ; true; )
			{
				if( (::__EXT_MFC_GetWindowLong(hWndTest,GWL_STYLE)&WS_CHILD) != 0 )
					hWndTest = ::GetParent( hWndTest );
				else
					break;
			}
			CWnd * pWndTest = CWnd::FromHandlePermanent( hWndTest );
			CExtPopupMenuWnd * pPopup = DYNAMIC_DOWNCAST( CExtPopupMenuWnd, pWndTest );
			if( pPopup != NULL )
			{
				bool bAllowReAssignCapture = false;
				CExtPopupMenuWnd * pTestPopup = pPopup;
				for( ; pTestPopup != NULL; pTestPopup = pTestPopup->GetParentMenuWnd() )
				{
					if( pTestPopup == this )
					{
						bAllowReAssignCapture = true;
						break;
					}
				}
				if( bAllowReAssignCapture )
				{
					CExtPopupMenuWnd * pPrevPopup = NULL;
					pTestPopup = pPopup;
					for( ; pTestPopup != NULL;  )
					{
						pTestPopup->m_nDelayedFocusItemIndex = IDX_NOTHING;
						pTestPopup->KillTimer( ID_TIMER_ITEM_FOCUS_DELAY );
						pTestPopup->KillTimer( ID_TIMER_ITEM_HIDE_DELAY );
						pTestPopup->m_bDelayedHiding = false;
						pTestPopup->m_nLastMousePick = IDX_NOTHING;
						if( pPrevPopup != NULL )
						{
							INT nIndex, nCount = pTestPopup->ItemGetCount();
							for( nIndex = 0; nIndex < nCount; nIndex++ )
							{
								MENUITEMDATA & _mii = pTestPopup->ItemGetInfo( nIndex );
								if( ! _mii.IsPopup() )
									continue;
								CExtPopupMenuWnd * pChildPopup = _mii.GetPopup();
								if( pChildPopup == pPrevPopup )
								{
									if( pTestPopup->m_nCurIndex != nIndex )
									{
										pTestPopup->_ItemFocusSet( nIndex, TRUE, TRUE, FALSE, FALSE );
										pTestPopup->m_nLastMousePick = nIndex;
									}
									break;
								}
							}
						}
						pPrevPopup = pTestPopup;
						pTestPopup = pTestPopup->GetParentMenuWnd();
					}
					//::ReleaseCapture();
					pPopup->_SetCapture();
					CPoint point = ptScreenClick;
					pPopup->ScreenToClient( &point );
					return true; //pPopup->_OnMouseMove( nFlags, point, bNoEat );
				}
			}

			HWND hWndOwn = m_hWnd;
			if( m_nCurIndex >= 0 && m_nLastMousePick >= 0 )
			{
				MENUITEMDATA & _mii = ItemGetInfo( m_nCurIndex );
				if( ! _mii.IsPopup() )
				{
					_ItemFocusCancel( TRUE );
					if( ! ::IsWindow( hWndOwn ) )
						return true;
				}
				else
				{
					if( _mii.GetPopup()->m_nCurIndex < 0 )
					{
						_ItemFocusCancel( TRUE );
						if( ! ::IsWindow( hWndOwn ) )
							return true;
					}
				} // if( ! _mii.IsPopup() )
			} // if( m_nCurIndex >= 0 && m_nLastMousePick >= 0 )
			else if( m_nCurIndex != IDX_NOTHING )
			{
				bool bAlt =
					( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 )
						? true : false;
				if( (! bAlt) && (TrackFlagsGet()&TPMX_SELECT_ANY) == 0 && m_nLastMousePick != IDX_NOTHING )
					_ItemFocusCancel( TRUE );
				if( ! ::IsWindow( hWndOwn ) )
					return true;
			} // else if( m_nCurIndex != IDX_NOTHING )
		} // if( m_nCurIndex >= 0 && m_nLastMousePick >= 0 )
		m_nLastMousePick = IDX_NOTHING;

		return false;
	}

int nCurIndex = _HitTest(point);
	if( nCurIndex == IDX_EXPAND )
	{
		HWND hWndOwn = m_hWnd;
		_ItemFocusCancel( TRUE );
		if( ! ::IsWindow( hWndOwn ) )
			return true;
		_SetCapture();
		if( g_bMenuDelayExpanding )
		{
//			_SetCapture();
			if( m_nWaitingExpandTickCount == 0 )
			{
				SetTimer(
					ID_TIMER_DELAY_EXPAND,
					ID_TOTAL_DELAY_EXPAND/ID_FREQ_DELAY_EXPAND,
					NULL
					);
				m_nWaitingExpandTickCount = 1;
			}
		} // if( g_bMenuDelayExpanding )
		return true;
	}
	if( _IsTearOff() && nCurIndex == IDX_TEAROFF )
	{
		HWND hWndOwn = m_hWnd;
		_ItemFocusCancel( TRUE, FALSE );
		if( ! ::IsWindow( hWndOwn ) )
			return true;
		_SetCapture();
		HCURSOR hCursor = ::LoadCursor( NULL, IDC_SIZEALL );
		ASSERT( hCursor != NULL );
		::SetCursor( hCursor );
		if(		g_bMenuShowCoolTips
			&&	GetSite().GetCapture() == this
			)
		{
			CRect rcItem;
			_GetItemRect(nCurIndex,rcItem);
			ClientToScreen( &rcItem );
			bool bShowTip = (!m_bSuspendTips);
			CExtPopupMenuTipWnd & _tipWnd = GetTip();
			if( _tipWnd.GetSafeHwnd() != NULL )
			{
				CRect rcExcludeArea = _tipWnd.GetExcludeArea();
				if( rcExcludeArea == rcItem )
					bShowTip = false;
			} // if( _tipWnd.GetSafeHwnd() != NULL )
			if( bShowTip )
			{
				CExtSafeString sTipText;
				g_ResourceManager->LoadString( sTipText, ID_EXT_TEAR_OFF_MENU_TIP );
				if( sTipText.IsEmpty() )
					sTipText = _T("Drag to make this menu float");
				_tipWnd.SetText( sTipText );
				_tipWnd.SetTipStyle(
					g_eTtsTearOffCaption // _CoolTipGetStyle()
					);
				VERIFY(
					_tipWnd.Show(
						this,
						rcItem
						)
					);
			} // if( bShowTip )
		}
		return true;
	}

HWND hWndOwn = m_hWnd;
	if( _StartScrolling(nCurIndex) )
		return true;
	if( ! ::IsWindow( hWndOwn ) )
		return true;

// 	if(		m_nCurIndex == IDX_RIBBON_FILE_MENU_OPTIONS_BUTTON
// 		||	m_nCurIndex == IDX_RIBBON_FILE_MENU_EXIT_BUTTON
// 		)
	if( m_nCurIndex != nCurIndex )
	{
		CRect rcCurrItem;
		_GetItemRect( m_nCurIndex, rcCurrItem );
		if( ! rcCurrItem.IsRectEmpty() )
			InvalidateRect( &rcCurrItem );
	}
	if(		nCurIndex == IDX_RIBBON_FILE_MENU_OPTIONS_BUTTON
		||	nCurIndex == IDX_RIBBON_FILE_MENU_EXIT_BUTTON
		)
	{
		if(		m_nCurIndex >= 0
			&&	ItemGetInfo( m_nCurIndex ).IsPopup()
			&&	ItemGetInfo( m_nCurIndex ).GetPopup()->GetSafeHwnd() != NULL
			)
			return true;
		CRect rcCurrItem;
		_GetItemRect( nCurIndex, rcCurrItem );
		InvalidateRect( &rcCurrItem );
		_ItemFocusCancel( FALSE, TRUE );
		m_nCurIndex = nCurIndex;
		_SetCapture();
		return true;
	}
	if( nCurIndex >= 0 )
	{
#ifdef _DEBUG
		int nCountOfItems = (int)m_items_all.GetSize();
		ASSERT( nCurIndex < nCountOfItems );
#endif // _DEBUG
		MENUITEMDATA & mi = ItemGetInfo(nCurIndex);
		if(		( ( ! mi.IsEnabled() ) && ( ! g_bShowCoolTipsOverDisabledItems ) )
			||	mi.IsSeparator()
			)
		{
			_ItemFocusCancel( TRUE, TRUE, TRUE );
			_SetCapture();
			return false;
		}
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		const CExtCustomizeCmdTreeNode * pNode =
			mi.GetCmdNode();
		if( pNode != NULL )
		{
			ASSERT_VALID( pNode );
			if( (pNode->GetFlags() & __ECTN_TBB_SEPARATED_DROPDOWN) != 0 )
			{
				bool bRTL = OnQueryLayoutRTL();
				CRect rcDD;
				_GetItemRect( nCurIndex, rcDD, false );
				if( bRTL )
					rcDD.right = rcDD.left + __EXT_MENU_POPUP_ARROW_AREA_DX;
				else
					rcDD.left = rcDD.right - __EXT_MENU_POPUP_ARROW_AREA_DX;
				if( ! rcDD.PtInRect( point ) )
				{
					HWND hWndOwn = m_hWnd;
					_ItemFocusDelay();
//					bool bFocusAreadySet = false;
//					if( mi.IsPopup() )
//						_ItemFocusDelay( nCurIndex, &bFocusAreadySet );
					if( ! ::IsWindow( hWndOwn ) )
						return true;
//					if( ! bFocusAreadySet )
//					{
						_SetCapture();
						_ItemFocusSet(
							nCurIndex,
							FALSE,
							TRUE,
							FALSE
							);
						Invalidate( FALSE );
//					}
					if(		g_bMenuShowCoolTips
						&&	(! m_bSuspendTips )
//						&&	nCurIndex != nOldCurIndex
//						&&	GetSite().GetCapture() == this
						&&	mi.GetPopup()->GetSafeHwnd() == NULL
						)
					{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
						bool bInitCoolTip = true;
						if( _IsRibbonMode() )
						{
							if(		mi.KeyTipGetWnd( true )->GetSafeHwnd() != NULL
								||	mi.KeyTipGetWnd( false )->GetSafeHwnd() != NULL
								)
								bInitCoolTip = false;
							else
							{
								CExtPopupScreenTipWnd & _wndScreenTip = GetScreenTip();
								if( mi.InitSceenTip( _wndScreenTip ) )
									bInitCoolTip = false;
							}
						} // if( _IsRibbonMode() )
						if( bInitCoolTip )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
						{
							CExtSafeString sTipText;
							mi.GetTip( sTipText );
							if( ! sTipText.IsEmpty() )
							{
								CRect rcItem;
								_GetItemRect(nCurIndex,rcItem);
								ClientToScreen( &rcItem );
								CExtPopupMenuTipWnd & _tipWnd = GetTip();
								_tipWnd.SetText( sTipText );
								_tipWnd.SetTipStyle( _CoolTipGetStyle() );
								VERIFY(
									_tipWnd.Show(
										this,
										rcItem
										)
									);
							} // if( ! sTipText.IsEmpty() )
						} // if( bInitCoolTip )
					} // if( g_bMenuShowCoolTips ....
					return true;
				}
				else
					if( m_nCurIndex != nCurIndex )
						_ItemFocusCancel( TRUE, TRUE );
				if( mi.GetPopup()->GetSafeHwnd() != NULL )
					return false;
				m_nCurIndex = IDX_NOTHING;
			}
		} // if( pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		int nOldCurIndex = m_nCurIndex; 
		if( nOldCurIndex != nCurIndex )
		{
			HWND hWndOwn = m_hWnd;
			_ItemFocusDelay();
			bool bFocusAreadySet = false;
			if( mi.IsPopup() )
				_ItemFocusDelay( nCurIndex, &bFocusAreadySet );
			if( ! ::IsWindow( hWndOwn ) )
				return true;
			if( ! bFocusAreadySet )
			{
				_SetCapture();
				_ItemFocusSet( nCurIndex, FALSE, TRUE, FALSE, TRUE );
			}
			if( ! ::IsWindow( hWndOwn ) )
				return true;
			m_nLastMousePick = nCurIndex;
			MENUITEMDATA & mi = ItemGetInfo(nCurIndex);
			if( (! bFocusAreadySet ) && mi.IsPopup() )
			{
				CExtPopupMenuTipWnd & _tipWnd = GetTip();
				_tipWnd.Hide();
				if( mi.GetPopup()->GetSafeHwnd() == NULL )
				{
					_ItemFocusDelay( nCurIndex );
					if( ! ::IsWindow( hWndOwn ) )
						return true;
				}
				return true;
			}
			if(		g_bMenuShowCoolTips
				&&	(! m_bSuspendTips )
				&&	nCurIndex != nOldCurIndex
				&&	GetSite().GetCapture() == this
				)
			{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
				bool bInitCoolTip = true;
				if( _IsRibbonMode() )
				{
					if(		mi.KeyTipGetWnd( true )->GetSafeHwnd() != NULL
						||	mi.KeyTipGetWnd( false )->GetSafeHwnd() != NULL
						)
						bInitCoolTip = false;
					else
					{
						CExtPopupScreenTipWnd & _wndScreenTip = GetScreenTip();
						if( mi.InitSceenTip( _wndScreenTip ) )
							bInitCoolTip = false;
					}
				} // if( _IsRibbonMode() )
				if( bInitCoolTip )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
				{
					CExtSafeString sTipText;
					mi.GetTip( sTipText );
					if( ! sTipText.IsEmpty() )
					{
						CRect rcItem;
						_GetItemRect(nCurIndex,rcItem);
						ClientToScreen( &rcItem );
						CExtPopupMenuTipWnd & _tipWnd = GetTip();
						_tipWnd.SetText( sTipText );
						_tipWnd.SetTipStyle( _CoolTipGetStyle() );
						VERIFY(
							_tipWnd.Show(
								this,
								rcItem
								)
							);
					} // if( ! sTipText.IsEmpty() )
				} // if( bInitCoolTip )
			} // if( g_bMenuShowCoolTips ....
		} // if( nOldCurIndex != nCurIndex )

		return (nOldCurIndex == nCurIndex) ? false : true;
	} // if( nCurIndex >= 0 )

	return false;
}

void CExtPopupMenuWnd::_ItemFocusDelay(
	int nItemIndex, // = IDX_NOTHING
	bool * p_bFocusAreadySet // = NULL
	)
{
	ASSERT_VALID( this );

	if( p_bFocusAreadySet != NULL )
		(*p_bFocusAreadySet) = false;

HWND hWndOwn = m_hWnd;
	if( ! ::IsWindow( hWndOwn ) )
		return;

	if( nItemIndex >= 0 )
	{
		MENUITEMDATA & mii = ItemGetInfo( nItemIndex );
		if( mii.IsPopup() )
		{
			CExtPopupMenuWnd * pPopup = mii.GetPopup();
			ASSERT_VALID( pPopup );
			bool bPopupAlreadyOpen = false;
			if( pPopup->GetSafeHwnd() != NULL )
			{
				int nCount = ItemGetCount();
				for( int i = 0; i < nCount; i++ )
				{
					MENUITEMDATA & mii_child = ItemGetInfo( i );
					if( i == nItemIndex )
						continue;
					if( ! mii_child.IsPopup() )
						continue;
					CExtPopupMenuWnd * pChildPopup = mii_child.GetPopup();
					ASSERT_VALID( pChildPopup );
					if( pChildPopup->GetSafeHwnd() == NULL )
						continue;
					INT nSaved = pChildPopup->m_nFadeOutAnimationStepCount;
					pChildPopup->m_nFadeOutAnimationStepCount = -1;
					pChildPopup->_OnCancelMode();
					pChildPopup->m_nFadeOutAnimationStepCount = nSaved;
					if( ! ::IsWindow( hWndOwn ) )
						return;
				}
				bPopupAlreadyOpen = true;
				pPopup->m_bDelayedHiding = false;
				pPopup->KillTimer( ID_TIMER_ITEM_HIDE_DELAY );
				pPopup->_ItemFocusCancel( TRUE, FALSE, FALSE );
				{
					CClientDC dc( pPopup );
					pPopup->_DoPaint( dc );
				}
				pPopup->_SetCapture();
				m_nDelayedFocusItemIndex = IDX_NOTHING;
				KillTimer( ID_TIMER_DELAY_EXPAND );
				if( m_nCurIndex >= 0 )
				{
					MENUITEMDATA & mii_old = ItemGetInfo( m_nCurIndex );
					mii_old.SetSelected( false );
				}
				m_nCurIndex = nItemIndex;
				mii.SetSelected( true );
				{
					CClientDC dc( this );
					_DoPaint( dc );
				}
				if( p_bFocusAreadySet != NULL )
					(*p_bFocusAreadySet) = true;
				return;
			}
		}
	}

ItemFocusTracker_t _ift( this );
	if( m_nDelayedFocusItemIndex != nItemIndex )
	{
		if( m_nDelayedFocusItemIndex >= 0 )
			KillTimer( ID_TIMER_ITEM_FOCUS_DELAY );
		//m_nDelayedFocusItemIndex = IDX_NOTHING;
	}
	if( nItemIndex < 0 )
	{
		m_nDelayedFocusItemIndex = IDX_NOTHING;
		return;
	}
	if( m_nDelayedFocusItemIndex == nItemIndex )
		return;
	ASSERT( nItemIndex>=0 && nItemIndex<m_items_all.GetSize() );
	m_nDelayedFocusItemIndex = nItemIndex;
	ASSERT( ItemGetInfo(m_nDelayedFocusItemIndex).IsPopup() );
ITEMCOVERINGNOTIFICATON _icn(
		this,
		NULL,
		ITEMCOVERINGNOTIFICATON::__EICN_DELAY,
		m_nDelayedFocusItemIndex
		);
	_icn.Notify();
	m_bSuspendTips = _icn.m_bSuspendTips;
	if( ! ::IsWindow( hWndOwn ) )
		return;
	KillTimer( ID_TIMER_AUTO_EXPAND );
	ASSERT( g_nTimeDelayedDisplaying > 0 );
	SetTimer( ID_TIMER_ITEM_FOCUS_DELAY, g_nTimeDelayedDisplaying, NULL );
	_ift.FocusChangingUnRegister();
}

int CExtPopupMenuWnd::_GetMaxScrollPos()
{
	ASSERT_VALID( this );

	ASSERT( m_bScrollingAvailable );
CRect rcClient;
	_GetClientRect( &rcClient );
int nClientHeight =
		rcClient.Height();
CRect rcMB = OnQueryMenuBorderMetrics();
int nMenuShadowSize = OnQueryMenuShadowSize();
int nMaxScrollPos = m_sizeFullItems.cy;
	nMaxScrollPos += rcMB.top + rcMB.bottom;
	nMaxScrollPos += nMenuShadowSize;
	nMaxScrollPos -= nClientHeight;
	//ASSERT( nMaxScrollPos > 0 );
	nMaxScrollPos = max( 0, nMaxScrollPos );
	if( m_bExpandAvailable )
		nMaxScrollPos += _GetSpecBtnHeight(); // xpand btn
	if( _IsTearOff() )
		nMaxScrollPos += _GetTearOffCaptionHeight();
	return -nMaxScrollPos;
}

void CExtPopupMenuWnd::OnTimer(__EXT_MFC_UINT_PTR nIDEvent) 
{
	ASSERT_VALID( this );

	switch( nIDEvent )
	{
	case ID_TIMER_AUTO_EXPAND:
	{
		if( m_nCurIndex >= 0 )
		{
			MENUITEMDATA & mii = ItemGetInfo( m_nCurIndex );
			if( mii.IsInplaceEdit() )
			{
				CWnd * pWnd = mii.GetInplaceEditPtr();
				if(		pWnd->GetSafeHwnd() != NULL
					&&	::GetFocus() == pWnd->m_hWnd
					)
					return;
			}
			if(		mii.IsPopup()
				&&	mii.GetPopup()->GetSafeHwnd() != NULL
				)
			{
				KillTimer( nIDEvent );
				return;
			}
		}
		KillTimer( nIDEvent );
		_DoExpand();
	}
	return;

	case ID_TIMER_FADE_OUT_ANIMATION:
	{
		if( m_nFadeOutAnimationStepIndex < m_nFadeOutAnimationStepCount )
		{
			CExtPopupMenuWnd * pPopup;
			for( pPopup = this; pPopup != NULL; pPopup = pPopup->m_pWndParentMenu )
			{
				if( pPopup->GetSafeHwnd() == NULL )
					continue;
				ASSERT( g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL );
				if( (pPopup->GetStyle()&WS_VISIBLE) == 0 )
					continue;
				if( m_nFadeOutAnimationStepIndex == 0 )
				{
					pPopup->m_bHelperAnimationControllerDetected = true;
					if( (pPopup->GetExStyle()&(__EXT_MFC_WS_EX_LAYERED|WS_EX_TRANSPARENT)) != (__EXT_MFC_WS_EX_LAYERED|WS_EX_TRANSPARENT) )
					pPopup->ModifyStyleEx( 0, (__EXT_MFC_WS_EX_LAYERED|WS_EX_TRANSPARENT) );
					pPopup->EnableWindow( FALSE );
				}
				BYTE nAlpha = (BYTE)
					::MulDiv(
						m_nFadeOutAnimationStepCount - m_nFadeOutAnimationStepIndex - 1,
						255,
						m_nFadeOutAnimationStepCount
						);
				g_PaintManager.m_pfnSetLayeredWindowAttributes( pPopup->m_hWnd, 0, nAlpha, __EXT_MFC_LWA_ALPHA );
				if(		pPopup->m_ctrlShadow.GetSafeHwnd() != NULL
					&&	(pPopup->m_ctrlShadow.GetStyle()&WS_VISIBLE)
					)
					pPopup->m_ctrlShadow.UpdateWindowLayer( nAlpha );
				CExtPaintManager::stat_PassPaintMessages();
			} // for( pPopup = this; pPopup != NULL; pPopup = pPopup->m_pWndParentMenu )
			m_nFadeOutAnimationStepIndex ++;
		} // if( m_nFadeOutAnimationStepIndex < m_nFadeOutAnimationStepCount )
		else
		{
			KillTimer( nIDEvent );
			if( m_ctrlShadow.GetSafeHwnd() != NULL )
				m_ctrlShadow.DestroyWindow();
			CExtPopupMenuWnd * pPopup = this;
			for( ; pPopup != NULL; )
			{
				VERIFY( pPopup->_BuildItems( NULL, true ) );
				//pPopup->_DeleteFadeOutMenu();
				if(		pPopup->m_ctrlShadow.GetSafeHwnd() != NULL
					&&	(pPopup->m_ctrlShadow.GetStyle()&WS_VISIBLE)
					)
					pPopup->m_ctrlShadow.DestroyWindow();
				HWND hWnd = pPopup->m_hWnd;
				pPopup = pPopup->m_pWndParentMenu;
				::DestroyWindow( hWnd );
			}
		} // else from if( m_nFadeOutAnimationStepIndex < m_nFadeOutAnimationStepCount )
	}
	return;

	case ID_TIMER_DELAY_EXPAND:
	{
		if( m_nWaitingExpandTickCount <
				ID_TOTAL_DELAY_EXPAND
			)
		{
			if( m_nWaitingExpandTickCount <= 1 )
				m_nWaitingExpandTickCount = 0;
			m_nWaitingExpandTickCount +=
				ID_FREQ_DELAY_EXPAND;
			return;
		}
		VERIFY( KillTimer(ID_TIMER_DELAY_EXPAND) );
		m_nWaitingExpandTickCount = 0;
		if( m_bExpandAvailable
			&& (!m_bExpandWasPressed)
			&& ( GetSite().GetAnimated() == NULL )
			)
		{ // if expand can be performed
			CPoint point;
			if( ! ::GetCursorPos(&point) )
				return;
			ScreenToClient( &point );
			CRect rcExpand;
			_GetItemRect( IDX_EXPAND, rcExpand );
			if( rcExpand.PtInRect(point) )
				_DoExpand();
		} // if expand can be performed
	}
	return;

	case ID_TIMER_SCROLLING:
	{
		HWND hWndOwn = m_hWnd;
		_ItemFocusDelay();
		if( ! ::IsWindow( hWndOwn ) )
			return;
		if( m_nCurIndex >= 0 )
		{
			_ItemFocusCancel(TRUE);
			if( ! ::IsWindow( hWndOwn ) )
				return;
		}

		CPoint point;
		if( ! ::GetCursorPos(&point) )
			return;
		ScreenToClient( &point );

		m_nScrollingDirection = 0;

		CRect rcItem;
		_GetItemRect(IDX_SCROLL_TOP,rcItem);
		if( m_nAcceleratedPixelsPerStepCurrent > g_nAcceleratedPixelsPerStepMax )
			m_nAcceleratedPixelsPerStepCurrent = g_nAcceleratedPixelsPerStepMax;
		if( rcItem.PtInRect(point) )
		{
			m_nScrollingDirection = m_nAcceleratedPixelsPerStepCurrent; // __SCROLLING_PIXEL_STEP
		}
		else
		{
			_GetItemRect(IDX_SCROLL_BOTTOM,rcItem);
			if( rcItem.PtInRect(point) )
				m_nScrollingDirection = -m_nAcceleratedPixelsPerStepCurrent; // (-__SCROLLING_PIXEL_STEP);
		}

		m_nDyScrollOffset += m_nScrollingDirection;

		bool bKillThisTimer =
			(m_nScrollingDirection == 0) ? true : false;
		if( m_nDyScrollOffset > 0 )
		{
			m_nDyScrollOffset = 0;
			bKillThisTimer = true;
		}
		else
		{
			int nMaxScrollPos = _GetMaxScrollPos();
			if( m_nDyScrollOffset < nMaxScrollPos )
			{
				m_nDyScrollOffset = nMaxScrollPos;
				bKillThisTimer = true;
			}
		}

		if( bKillThisTimer )
		{
			VERIFY( KillTimer(ID_TIMER_SCROLLING) );
			m_nAcceleratedPixelsPerStepCurrent = __SCROLLING_PIXEL_STEP;
			m_nAcceleratedMilliSecondsPassed = 0;
			m_nScrollingDirection = 0;
		}
		else
		{
			if( g_bUseAcceleratedMenuScrolling )
			{
				INT nAcceleratedIncreasePercent = g_nAcceleratedIncreasePercent;
				INT nAcceleratedIncreaseStep = g_nAcceleratedIncreaseStep;
				if(		nAcceleratedIncreaseStep >= 0
					&&	nAcceleratedIncreasePercent > 0
					)
				{
					if( m_nAcceleratedMilliSecondsPassed >= nAcceleratedIncreaseStep )
					{
						m_nAcceleratedMilliSecondsPassed = 0;
						INT nStepPx =
							::MulDiv(
								m_nAcceleratedPixelsPerStepCurrent,
								100,
								__SCROLLING_PIXEL_STEP
								);
						nStepPx +=
							::MulDiv(
								nStepPx,
								nAcceleratedIncreasePercent,
								100
								);
						m_nAcceleratedPixelsPerStepCurrent =
							::MulDiv(
								nStepPx,
								__SCROLLING_PIXEL_STEP,
								100
								);
					} // if( m_nAcceleratedMilliSecondsPassed >= nAcceleratedIncreaseStep )
					else
					{
						m_nAcceleratedMilliSecondsPassed += ID_PERIOD_SCROLLING;
					} // else from if( m_nAcceleratedMilliSecondsPassed >= nAcceleratedIncreaseStep )
				}
			} // if( g_bUseAcceleratedMenuScrolling )
		}

		_CachedRectsEmpty();

		Invalidate( FALSE );
	}
	return;

	case ID_TIMER_ITEM_HIDE_DELAY:
	{
		KillTimer( nIDEvent );
		if( ! m_bDelayedHiding )
			return;
		INT nSaved = m_nFadeOutAnimationStepCount;
		m_nFadeOutAnimationStepCount = -1;
		_OnCancelMode();
		m_nFadeOutAnimationStepCount = nSaved;
		return;
	}

	case ID_TIMER_ITEM_FOCUS_DELAY:
	{
		if( GetSite().GetAnimated() != NULL )
			return;
		KillTimer( nIDEvent );
		if( GetSite().IsShutdownMode() )
			return;
		Invalidate( FALSE );
		if( m_nDelayedFocusItemIndex >= 0 )
		{
			ASSERT( m_nDelayedFocusItemIndex>=0 && m_nDelayedFocusItemIndex<m_items_all.GetSize() );
			ASSERT( ItemGetInfo(m_nDelayedFocusItemIndex).IsPopup() );
			CRect rcItem;
			_GetItemRect(m_nDelayedFocusItemIndex,rcItem);
			CPoint point;
			if( ! ::GetCursorPos( &point ) )
				return;
			ScreenToClient( &point );
			if( rcItem.PtInRect(point) )
			{
				HWND hWndOwn = m_hWnd;
				_ItemFocusSet(
					m_nDelayedFocusItemIndex,
					TRUE,
					TRUE,
					FALSE
					);
				if( ! ::IsWindow( hWndOwn ) )
					return;
			}
		} // if( m_nDelayedFocusItemIndex >= 0 )
		m_nDelayedFocusItemIndex = IDX_NOTHING;
	}
	return;
	
	default:
		CExtPopupBaseWnd::OnTimer(nIDEvent);
	break; // default
	} // switch( nIDEvent )
}

CExtPopupMenuWnd * CExtPopupMenuWnd::_GetCapture()
{
	if( (TrackFlagsGet()&TPMX_NO_SITE) != 0 )
		return this;
	return GetSite().GetCapture();
}

void CExtPopupMenuWnd::_SetCapture()
{
	ASSERT_VALID( this );
	if( (TrackFlagsGet()&TPMX_NO_SITE) != 0 )
		return;
	ASSERT( !_FindCustomizeMode() );

CExtPopupMenuSite & _site = GetSite();
	if( GetSafeHwnd() != NULL )
	{
		_site.SetCapture( this );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		_KeyTipsShow( true );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	}
	else
		_site.SetCapture( NULL );
}

void CExtPopupMenuWnd::_ReleaseCapture()
{
	ASSERT_VALID( this );
	ASSERT( !_FindCustomizeMode() );

CExtPopupMenuSite & _site = GetSite();
	if(		_site.IsEmpty()
		||	_site.IsShutdownMode()
		)
		return;
	if( _site.GetCapture() == this )
		_site.SetCapture( NULL );
}

int CExtPopupMenuWnd::_GetNextItem(
	next_item_t nit
	)
{
	ASSERT_VALID( this );

int nCountOfItems = (int)m_items_all.GetSize();
bool bCanExpand = false;
	if(		m_bExpandAvailable
		&&	(!m_bExpandWasPressed)
		)
	{
		if( IsAllItemsRarelyUsed() )
			return IDX_EXPAND;
		bCanExpand = true;
	}
	if(		nit == __NI_NOTHING
		||	nCountOfItems == 0
		)
	{
		if( bCanExpand )
			return IDX_EXPAND;
		return IDX_NOTHING;
	}
	if(		nit == __NI_ANY
		&&	m_nCurIndex >= 0
		)
		return m_nCurIndex;
	if( m_nCurIndex < 0 )
	{
		INT iter = 0;
		for( ; iter < m_items_all.GetSize(); ++iter )
		{
			MENUITEMDATA & mi = ItemGetInfo( iter );
			if(		mi.IsEnabled()
				&&	mi.IsDisplayed()
				)
				return mi.GetIndex();
		}
		if( bCanExpand )
			return IDX_EXPAND;
		return IDX_NOTHING;
	} // if( m_nCurIndex < 0 )

	if( nit == __NI_ROW_NEXT )
	{
		if( nCountOfItems < 2 )
		{
			if( bCanExpand )
				return IDX_EXPAND;
			return IDX_NOTHING;
		}
		INT iter = m_nCurIndex;
		if( ! ItemGetInfo( iter ).IsIconMode() )
			return _GetNextItem( __NI_NEXT );
		CRect rcCurrItem;
		_GetItemRect( iter, rcCurrItem, false );
		for( ; iter < m_items_all.GetSize(); ++iter )
		{
			MENUITEMDATA & mi = ItemGetInfo( iter );
			if(		mi.IsDisplayed()
				&&	ItemPaletteRowWrapGet( iter )
				)
			{
				++iter;
				break;
			}
		}
		int nPotentialIdx = IDX_NOTHING;
		for( ; iter < m_items_all.GetSize(); ++iter )
		{
			MENUITEMDATA & mi = ItemGetInfo( iter );
			if(		(! mi.IsEnabled() )
				||	(! mi.IsDisplayed() )
				)
				continue;
			CRect rcItem;
			_GetItemRect( iter, rcItem, false );
			if( rcItem.left >= rcCurrItem.left )
				return mi.GetIndex();
			else if(
					ItemPaletteRowWrapGet( iter )
				&&	nPotentialIdx != IDX_NOTHING
				)
				break;
			nPotentialIdx = iter;
		}
		if(		nPotentialIdx == IDX_NOTHING
			&&	bCanExpand
			)
			return IDX_EXPAND;
		return nPotentialIdx;
	} // if( nit == __NI_ROW_NEXT )
	if( nit == __NI_ROW_PREV )
	{
		if( nCountOfItems < 2 )
			return IDX_NOTHING;
		INT iter = m_nCurIndex;
		if( ! ItemGetInfo( iter ).IsIconMode() )
			return _GetNextItem( __NI_PREV );
		CRect rcCurrItem;
		_GetItemRect( iter, rcCurrItem, false );
		for( ; (iter+1) > 0; --iter )
		{
			MENUITEMDATA & mi = ItemGetInfo( iter );
			if(		mi.IsDisplayed()
				&&	ItemPaletteRowWrapGet( iter )
				&&	iter != m_nCurIndex
				)
				break;
		}
		int nPotentialIdx = IDX_NOTHING;
		for( ; (iter+1) > 0; --iter )
		{
			if(		ItemPaletteRowWrapGet( iter )
				&&	nPotentialIdx != IDX_NOTHING
				)
				break;
			MENUITEMDATA & mi = ItemGetInfo( iter );
			if(		(! mi.IsEnabled() )
				||	(! mi.IsDisplayed() )
				)
				continue;
			CRect rcItem;
			_GetItemRect( iter, rcItem, false );
			if( rcItem.right <= rcCurrItem.right )
				return mi.GetIndex();
			nPotentialIdx = iter;
		}
		if(		nPotentialIdx == IDX_NOTHING
			&&	bCanExpand
			)
			return IDX_EXPAND;
		return nPotentialIdx;
	} // if( nit == __NI_ROW_PREV )

	if( nit == __NI_NEXT )
	{
		if( nCountOfItems < 2 )
		{
			if( bCanExpand )
				return IDX_EXPAND;
			return IDX_NOTHING;
		}
		INT iter = m_nCurIndex + 1;
		for( ; iter < m_items_all.GetSize(); ++iter )
		{
			MENUITEMDATA & mi = ItemGetInfo( iter );
			if( mi.IsEnabled() && mi.IsDisplayed() )
				return mi.GetIndex();
		}
		if( bCanExpand )
			return IDX_EXPAND;
		if( m_nCurIndex == 0 )
			return IDX_NOTHING;
		iter = 0;
		INT iter2 = m_nCurIndex - 1;
		do
		{
			MENUITEMDATA & mi = ItemGetInfo( iter );
			if( mi.IsEnabled() && mi.IsDisplayed() )
				return mi.GetIndex();
			iter++;
		} while( iter != iter2 );
		return IDX_NOTHING;
	} // if( nit == __NI_NEXT )
	if( nit == __NI_PREV )
	{
		if( nCountOfItems < 2 )
			return IDX_NOTHING;
		if( m_nCurIndex != 0 )
		{
			int nIndex = m_nCurIndex - 1;
			INT iter = nIndex;
			for( ; true;  )
			{
				MENUITEMDATA & mi = ItemGetInfo( iter );
				if( mi.IsEnabled() && mi.IsDisplayed() )
					return mi.GetIndex();
				if( nIndex == 0 )
					break;
				--iter;
				--nIndex;
			}
		} // if( m_nCurIndex != 0 )
		int nIndex = m_nCurIndex + 1;
		INT iter2 = nIndex;
		INT iter = INT(m_items_all.GetSize()) - 1;
		do
		{
			MENUITEMDATA & mi = ItemGetInfo( iter );
			if( mi.IsEnabled() && mi.IsDisplayed() )
				return mi.GetIndex();
			iter--;
		} while( iter != iter2 );
		return IDX_NOTHING;
	} // if( nit == __NI_PREV )

	return IDX_NOTHING;
}

const ACCEL * CExtPopupMenuWnd::_GetTranslatedContextHelpAccel() const
{
	if( ! g_bTranslateContextHelpCmd )
		return NULL;
	return &g_accelContextHelp;
}

bool CExtPopupMenuWnd::_OnKeyDown(
	UINT nChar,
	UINT nRepCnt,
	UINT nFlags,
	bool & bNoEat
	)
{
	ASSERT_VALID( this );
	nFlags;

	ASSERT( m_hWndCmdReceiver != NULL );
	ASSERT( ::IsWindow(m_hWndCmdReceiver) );

	nRepCnt;
	if( GetSafeHwnd() == NULL )
		return true;

	if( ! m_bAnimFinished )
		return true;

	if( _IsResizingMode() )
		return true;

CExtPopupMenuSite & _site = GetSite();
	if(	_site.IsShutdownMode()
		|| _site.IsEmpty()
		|| _site.GetAnimated() != NULL
		)
		return true;

TranslateKeyboardEventData_t _td( this, nChar, nRepCnt, nFlags, bNoEat );
	if( _td.Notify() )
	{
		bNoEat = _td.m_bNoEat;
		return true;
	}

bool bAlt =
		( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 )
			? true : false;
bool bCtrl =
		( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 )
			? true : false;
bool bShift =
		( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 )
			? true : false;

HWND hWndFocus = ::GetFocus();
	if(		hWndFocus != NULL
		&&	(::__EXT_MFC_GetWindowLong(hWndFocus,GWL_STYLE)&WS_CHILD) != 0
		&&	::GetParent(hWndFocus) == m_hWnd
		&&	(	nChar != VK_RETURN
			||	m_nCurIndex < 0
			||	(! ItemGetInfo(m_nCurIndex).IsInplaceEdit() )
			)
		)
	{
		if( bAlt )
			PostMessage( WM_CANCELMODE );
		else if( nChar == VK_ESCAPE )
			SetFocus();
		else if( (nChar == VK_INSERT || nChar == _T('C')) && bCtrl )
				::SendMessage( hWndFocus, WM_COPY, 0, 0 );
		else if(	( nChar == _T('X') && bCtrl )
				||	( nChar == VK_DELETE && bShift )
				)
				::SendMessage( hWndFocus, WM_CUT, 0, 0 );
		else if(	( nChar == _T('V') && bCtrl )
				||	( nChar == VK_INSERT && bShift )
				)
				::SendMessage( hWndFocus, WM_PASTE, 0, 0 );
		else if( nChar == VK_DELETE )
				::SendMessage( hWndFocus, CExtPopupMenuWnd::CInPlaceEditWnd::g_nMsgDeleteSelection, 0, 0 );
		else if( nChar == _T('A') && bCtrl )
				::SendMessage( hWndFocus, CExtPopupMenuWnd::CInPlaceEditWnd::g_nMsgSelectAll, 0, 0 );
		else
			bNoEat = true;
		return (!bNoEat);
	}

next_item_t nit = __NI_NOTHING;

	if( nChar == VK_TAB && (! ( bCtrl || bAlt ) ) )
	{
		if(		_IsPalette()
			&&	ItemGetCount() > 0
			&&	m_nCurIndex >= 0
			&&	m_nCurIndex <= (ItemGetCount() - 1)
			&&	ItemGetInfo(m_nCurIndex).IsToolButton()
			)
			nChar = bShift ? VK_LEFT : VK_RIGHT;
		else
			nChar = bShift ? VK_UP : VK_DOWN;
	}

bool bRTL = OnQueryLayoutRTL();
	if(		bRTL
		&&	(	nChar == VK_LEFT
			||	nChar == VK_RIGHT
			)
		)
		nChar = (nChar == VK_LEFT) ? VK_RIGHT : VK_LEFT;

	switch( nChar )
	{
	case VK_MENU:
		_EndSequence();
		return true;
	case VK_SPACE:
		return true;
	case VK_F4:
	{
		if( m_nCurIndex < 0 )
			return true;
		MENUITEMDATA & mi = ItemGetInfo( m_nCurIndex );
		if( mi.IsExtraMark() )
			return true;
		if( ! ( mi.IsInplaceEdit() && mi.IsPopup() ) )
			return true;
		if( mi.GetPopup()->GetSafeHwnd() == NULL )
		{
			HWND hWndOwn = m_hWnd;
			_ItemFocusSet(
				m_nCurIndex,
				TRUE,
				TRUE
				);
			if( ! ::IsWindow( hWndOwn ) )
				return true;
		}
		return true;
	}
	case VK_RETURN:
	{
		if( m_nCurIndex < 0 )
		{
			INT nPos = ItemDefaultFind();
			if( nPos < 0 )
				return true; // break;
			_ItemFocusSet(
				nPos,
				FALSE,
				TRUE,
				TRUE
				);
		}
		MENUITEMDATA & mi = ItemGetInfo( m_nCurIndex );
		if( mi.IsInplaceEdit() )
		{
			CWnd * pWndInplace = mi.GetInplaceEditPtr();
			if( pWndInplace != NULL )
			{
				ASSERT_VALID( pWndInplace );
				ASSERT( pWndInplace->GetSafeHwnd() );
				if( mi.IsAllowInplaceEditActivation() )
				{
					if( (pWndInplace->GetStyle() & WS_VISIBLE) == 0 )
					{
						CRect rcInplaceEdit;
						_GetItemRect( m_nCurIndex, rcInplaceEdit );
						rcInplaceEdit =
							mi.AdjustInplaceEditRect(
								rcInplaceEdit,
								OnQueryLayoutRTL()
								);
						pWndInplace->SetWindowPos(
							NULL,
							rcInplaceEdit.left, rcInplaceEdit.top,
							rcInplaceEdit.Width(), rcInplaceEdit.Height(),
							SWP_NOZORDER|SWP_NOOWNERZORDER
								|SWP_NOACTIVATE|SWP_SHOWWINDOW
							);
						pWndInplace->SetFocus();
					} // if( (pWndInplace->GetStyle() & WS_VISIBLE) == 0 )
					else
					{
						bNoEat = true;
						return false;
					}
				} // if( mi.IsAllowInplaceEditActivation() )
			} // if( pWndInplace != NULL )
			return true;
		}
		if( mi.IsExtraMark() && (! _FindHelpMode() ) )
		{
			ASSERT( mi.GetExtraMarkCallback() != NULL );
			mi.GetExtraMarkCallback()( this, &mi );
			CClientDC dc( this );
			_DoPaint( dc );
			return true;
		} // if( mi.IsExtraMark() ....
		if(		mi.IsExecutableCmdID()
			||	(	mi.IsPopup()
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
				&&	mi.GetCmdNode() != NULL
				&&	(mi.GetCmdNode()->GetFlags()&__ECTN_TBB_SEPARATED_DROPDOWN) != 0
				&&	(mi.GetCmdNode()->GetFlags()&(__ECTN_TBB_COLOR|__ECTN_TBB_UNDO_REDO)) != 0
#else // (!defined __EXT_MFC_NO_CUSTOMIZE)
				&&	(	mi.GetPopup()->IsKindOf( RUNTIME_CLASS(CExtPopupColorMenuWnd) )
#if (!defined __EXT_MFC_NO_UNDO_REDO_POPUP)
					||	mi.GetPopup()->IsKindOf( RUNTIME_CLASS(CExtPopupUndoRedoMenuWnd) )
#endif // (!defined __EXT_MFC_NO_UNDO_REDO_POPUP)
					)
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
				)
			)
		{
			if(	!(		mi.GetCmdDeliverCb() != NULL
					&&	mi.GetCmdDeliverCb()( this, &mi )
					)
				)
			{
				_CancelingSet();
				_EndSequenceEx( mi );
			}
			return true;
		}
	} // case VK_RETURN

	case VK_RIGHT:
	{
		if(		_IsPalette()
			&&	ItemGetCount() > 0
			&&	m_nCurIndex >= 0
			&&	m_nCurIndex <= (ItemGetCount() - 1)
			&&	(! ItemGetInfo(m_nCurIndex).IsPopup() )
			)
		{
			nit = __NI_NEXT;
			break;
		}
		bool bSendChangeMenu = false;
		if( m_nCurIndex < 0 )
		{
			if( m_items_all.GetSize() == 0 )
				return true; // false;
//			nit = __NI_ANY;
			bSendChangeMenu = true;
		}
		else
		{
			MENUITEMDATA & mi = ItemGetInfo(m_nCurIndex);
			if( !mi.IsPopup() )
			{
				bSendChangeMenu = true;
			}
			else
			{
				if( mi.GetPopup()->GetSafeHwnd() == NULL )
				{
					HWND hWndOwn = m_hWnd;
					_ItemFocusDelay();
					if( ! ::IsWindow( hWndOwn ) )
						return true;
					_ItemFocusSet(
						m_nCurIndex,
						TRUE,
						TRUE
						);
					if( ! ::IsWindow( hWndOwn ) )
						return true;
				}
			}
		}
		if( bSendChangeMenu && nChar==VK_RIGHT )
		{
			ASSERT( m_hWndCmdReceiver != NULL );
			ASSERT( ::IsWindow(m_hWndCmdReceiver) );
			if( CExtToolControlBar::g_bMenuTracking )
				::PostMessage(
					m_hWndCmdReceiver,
					bRTL ? CExtPopupMenuWnd::g_nMsgPopupPrev : CExtPopupMenuWnd::g_nMsgPopupNext,
					0,
					LPARAM( this )
					);
			return true;
		}
	}
	break; // case VK_RIGHT

	case VK_ESCAPE:
		if( m_bTopLevel )
		{
			_EndSequence();
			return true;
		}
	// continue falling here...
	case VK_LEFT:
		if(		nChar == VK_LEFT
			&&	_IsPalette()
			&&	ItemGetCount() > 0
			&&	m_nCurIndex >= 0
			)
		{
			nit = __NI_PREV;
			break;
		}
		else if( m_pWndParentMenu == NULL )
		{
			if( CExtToolControlBar::g_bMenuTracking )
				::PostMessage(
					m_hWndCmdReceiver,
					bRTL ? CExtPopupMenuWnd::g_nMsgPopupNext : CExtPopupMenuWnd::g_nMsgPopupPrev,
					0,
					LPARAM( this )
					);
			return true;
		}
		else
		{
			ASSERT( !m_bTopLevel );
			int nParentCurIndex =
				m_pWndParentMenu->m_nCurIndex;
			ASSERT(
				nParentCurIndex >= 0
				&&
				nParentCurIndex <=
					m_pWndParentMenu->m_items_all.GetSize()
				);
			HWND hWndOwn = m_hWnd;
			//::ReleaseCapture();
			m_pWndParentMenu->_SetCapture();
			m_pWndParentMenu->_ItemFocusCancel(
				FALSE
				);
			m_pWndParentMenu->_ItemFocusSet(
				nParentCurIndex,
				FALSE,
				TRUE
				);
			if( ! ::IsWindow( hWndOwn ) )
				return true;
			m_pWndParentMenu->_SetCapture();
			return true;
		}

//	case VK_ESCAPE:
//		_EndSequence();
//		return true;
	
	case VK_UP:
		if(		nChar == VK_UP
			&&	ItemGetCount() > 0
			&&	0 <= m_nCurIndex
			&&	m_nCurIndex < ItemGetCount()
			&&	(! ItemGetInfo(m_nCurIndex).IsIconMode() )
			)
			nit = __NI_PREV;
		else
			nit = _IsPalette() ? __NI_ROW_PREV : __NI_PREV;
	break; // case VK_UP
	case VK_DOWN:
		if( IsKeyPressed( VK_CONTROL ) )
		{
			if( m_bExpandAvailable )
				_DoExpand();
			return true;
		}
		if(		ItemGetCount() > 0
			&&	0 <= m_nCurIndex
			&&	m_nCurIndex < ItemGetCount()
			&&	(! ItemGetInfo(m_nCurIndex).IsIconMode() )
			)
			nit = __NI_NEXT;
		else
			nit = _IsPalette() ? __NI_ROW_NEXT : __NI_NEXT;
	break; // case VK_DOWN
	case VK_NEXT:
		nit = __NI_PAGE_DOWN;
	break; // case VK_NEXT
	case VK_PRIOR:
		nit = __NI_PAGE_UP;
	break; // case VK_PRIOR

	default:

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( _KeyTipsEnabledGet() )
	{ // BLOCK: key tip processing
		if(		( _T('A') <= nChar && nChar <= _T('Z') )
			||	( _T('0') <= nChar && nChar <= _T('9') )
			)
		{
			if( _KeyTipsTranslate( nChar ) )
				return true;
		}
	} // BLOCK: key tip processing
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	{ // BLOCK: try analyze quick char access
		if(		IsKeyPressed(VK_SHIFT)
			||	IsKeyPressed(VK_CONTROL)
//			||	IsKeyPressed(VK_MENU)
//			||	( (nFlags & (1<<29)) != 0 ) // ALT pressed?
			)
			return false;
		BYTE lpKeyState[256];
		::GetKeyboardState( lpKeyState );
		UINT wScanCode = ::MapVirtualKey( nChar, 0 );
		HKL hKeyboardLayout =
			::GetKeyboardLayout(
				( ::AfxGetThread() ) -> m_nThreadID
				);
#if (defined _UNICODE)
		TCHAR szChar[2] = { _T('\0'), _T('\0') };
		::ToUnicodeEx( nChar, wScanCode, lpKeyState, szChar, 1, 1, hKeyboardLayout );
		WORD nMapped = WORD( szChar[0] );
#else
		WORD nMapped = 0;
		::ToAsciiEx( nChar, wScanCode, lpKeyState, &nMapped, 1, hKeyboardLayout );
		//TCHAR szChar[2] = { (TCHAR)nMapped, _T('\0') };
#endif
		BOOL bSearchDisplayedOnly = FALSE;
		INT nNextIdx =
			ItemFindByAccessChar(
				(TCHAR)nMapped,
				m_nCurIndex,
				TRUE,
				bSearchDisplayedOnly
				);
		if(		(! bSearchDisplayedOnly )
			&&	nNextIdx >= 0
			&&	m_bExpandAvailable
			&&	(! m_bExpandWasPressed )
			)
		{
			MENUITEMDATA & miNext = ItemGetInfo( nNextIdx );
			if(		(! miNext.IsDisplayed() )
				&&	(	ItemFindByAccessChar(
							(TCHAR)nMapped,
							nNextIdx,
							TRUE,
							bSearchDisplayedOnly
							) >= 0
					||	miNext.IsPopup()
					)
				)
			{
				KillTimer( ID_TIMER_DELAY_EXPAND );
				bool bSavedMenuExpandAnimation = g_bMenuExpandAnimation;
				g_bMenuExpandAnimation = false;
				_DoExpand();
				_EndAnimation();
				KillTimer( ID_TIMER_ANIMATION );
				m_bAnimFinished = true;
				GetSite().SetAnimated( NULL );
				_DoReCreateShadows();
				g_bMenuExpandAnimation = bSavedMenuExpandAnimation;
				CClientDC dc( this );
				_DoPaint( dc );
			}
			if( miNext.IsPopup() )
			{
				_ItemFocusSet( nNextIdx, TRUE, TRUE, TRUE );
				return true;
			}
		}
		if(		nNextIdx < 0
			&&	m_nCurIndex >= 0
			&&	ItemFindByAccessChar(
					(TCHAR)nMapped,
					-1,
					TRUE,
					bSearchDisplayedOnly
					)
					== m_nCurIndex
			)
		{ // if only 1 with this char
			MENUITEMDATA & mi = ItemGetInfo( m_nCurIndex );
			if( mi.IsInplaceEdit() && (!mi.IsPopup()) )
			{
				CWnd * pWndInplace = mi.GetInplaceEditPtr();
				if( pWndInplace != NULL )
				{
					ASSERT_VALID( pWndInplace );
					ASSERT( pWndInplace->GetSafeHwnd() );
					if( mi.IsAllowInplaceEditActivation() )
					{
						if( (pWndInplace->GetStyle() & WS_VISIBLE) == 0 )
						{
							CRect rcInplaceEdit;
							_GetItemRect( m_nCurIndex, rcInplaceEdit );
							rcInplaceEdit =
								mi.AdjustInplaceEditRect(
									rcInplaceEdit,
									OnQueryLayoutRTL()
									);
							pWndInplace->SetWindowPos(
								NULL,
								rcInplaceEdit.left, rcInplaceEdit.top,
								rcInplaceEdit.Width(), rcInplaceEdit.Height(),
								SWP_NOZORDER|SWP_NOOWNERZORDER
									|SWP_NOACTIVATE|SWP_SHOWWINDOW
								);
						} // if( (pWndInplace->GetStyle() & WS_VISIBLE) == 0 )
						pWndInplace->SetFocus();
					} // if( mi.IsAllowInplaceEditActivation() )
				} // if( pWndInplace != NULL )
				return true;
			} // if( mi.IsInplaceEdit() && (!mi.IsPopup()) )
			if( mi.IsExtraMark() && (! _FindHelpMode() ) )
			{
				ASSERT( mi.GetExtraMarkCallback() != NULL );
				mi.GetExtraMarkCallback()( this, &mi );
				CClientDC dc( this );
				_DoPaint( dc );
				return true;
			} // if( mi.IsExtraMark() ...
			if( mi.IsPopup() )
			{
				_ItemFocusSet( m_nCurIndex, TRUE, TRUE );
				return true;
			}
			if( mi.IsExecutableCmdID() )
			{
				if(	!(		mi.GetCmdDeliverCb() != NULL
						&&	mi.GetCmdDeliverCb()( this, &mi )
						)
					)
					_EndSequenceEx( mi );
				return true;
			} // if( mi.IsExecutableCmdID() )
		} // if only 1 with this char
		if(		m_nCurIndex != nNextIdx
			&&	nNextIdx >= 0
			)
		{
			HWND hWndOwn = m_hWnd;
			_ItemFocusSet( nNextIdx, TRUE, TRUE );
			if( ! ::IsWindow( hWndOwn ) )
				return true;
			if( ItemFindByAccessChar(
					(TCHAR)nMapped,
					nNextIdx
					) < 0
				)
			{ // if only 1 with this char
				MENUITEMDATA & mi =
					ItemGetInfo( nNextIdx );
				if( mi.IsInplaceEdit() && (!mi.IsPopup()) )
				{
					CWnd * pWndInplace = mi.GetInplaceEditPtr();
					if( pWndInplace != NULL )
					{
						ASSERT_VALID( pWndInplace );
						ASSERT( pWndInplace->GetSafeHwnd() );
						if( mi.IsAllowInplaceEditActivation() )
						{
							if( (pWndInplace->GetStyle() & WS_VISIBLE) == 0 )
							{
								CRect rcInplaceEdit;
								_GetItemRect( nNextIdx, rcInplaceEdit );
								rcInplaceEdit =
									mi.AdjustInplaceEditRect(
										rcInplaceEdit,
										OnQueryLayoutRTL()
										);
								pWndInplace->SetWindowPos(
									NULL,
									rcInplaceEdit.left, rcInplaceEdit.top,
									rcInplaceEdit.Width(), rcInplaceEdit.Height(),
									SWP_NOZORDER|SWP_NOOWNERZORDER
										|SWP_NOACTIVATE|SWP_SHOWWINDOW
									);
							} // if( (pWndInplace->GetStyle() & WS_VISIBLE) == 0 )
							pWndInplace->SetFocus();
						} // if( mi.IsAllowInplaceEditActivation() )
					} // if( pWndInplace != NULL )
					return true;
				} // if( mi.IsInplaceEdit() && (!mi.IsPopup()) )
				if( mi.IsExtraMark() && (! _FindHelpMode() ) )
				{
					ASSERT( mi.GetExtraMarkCallback() != NULL );
					mi.GetExtraMarkCallback()( this, &mi );
					CClientDC dc( this );
					_DoPaint( dc );
					return true;
				} // if( mi.IsExtraMark() ...
				if(		mi.IsExecutableCmdID()
					//&&	( ! mi.IsPopup() )
					&&	_GetCapture() == this
					)
				{
					if(	!(		mi.GetCmdDeliverCb() != NULL
							&&	mi.GetCmdDeliverCb()( this, &mi )
							)
						)
					{
						_CancelingSet();
						_EndSequenceEx( mi );
					}
					return true;
				} // if( mi.IsExecutableCmdID() )
			} // if only 1 with this char
			return true;
		}

	} // BLOCK: try analyze quick char access
	{ // BLOCK: help key processing
		const ACCEL * pAccel = _GetTranslatedContextHelpAccel();
		if(		m_nCurIndex >= 0
			&&	pAccel != NULL
			&&	nChar == WPARAM(pAccel->key)
			&&	(pAccel->fVirt&(FSHIFT|FCONTROL|FALT)) ==
					(	0
					|	( bShift ? (FSHIFT) : 0 )
					|	( bCtrl ? (FCONTROL) : 0 )
					|	( bAlt ? (FALT) : 0 )
					)
			)
		{
			MENUITEMDATA & mi = ItemGetInfo( m_nCurIndex );
			if( ( ! mi.IsPopup() ) && ( ! mi.IsExtraMark() ) )
			{
				UINT nHitTestCmdID = mi.GetCmdID();
				bNoEat = false;
				_EndSequence();
				::AfxGetApp()->WinHelp( nHitTestCmdID, HELP_CONTEXT );
				return true;
			} // if( ( ! mi.IsPopup() ) && ( ! mi.IsExtraMark() ) )
		} // if( m_nCurIndex >= 0 ...
	} // BLOCK: help key processing

	} // switch( nChar )

	if( nit != __NI_NOTHING )
	{
		int nItemIndex = _GetNextItem(nit);
		if( m_items_all.GetSize() > 0
			&& nItemIndex >= 0
			&& nItemIndex != m_nCurIndex
			)
		{
			HWND hWndOwn = m_hWnd;
			_ItemFocusSet(
				nItemIndex,
				FALSE,
				TRUE
				);
			if( ! ::IsWindow( hWndOwn ) )
				return true;
			return true; // false;
		}
		if( nItemIndex == IDX_EXPAND )
		{
			ASSERT( m_bExpandAvailable );
			ASSERT( !m_bExpandWasPressed );
			int nItemIndex = m_nCurIndex;
			HWND hWndOwn = m_hWnd;
			_DoExpand();
			if( ! ::IsWindow( hWndOwn ) )
				return true;
			if( nItemIndex >= 0 )
			{
				HWND hWndOwn = m_hWnd;
				_ItemFocusSet(
					nItemIndex,
					FALSE,
					//m_bAnimFinished
					( GetSite().GetAnimated() == NULL )
						? TRUE : FALSE
					);
				if( ! ::IsWindow( hWndOwn ) )
					return true;
				int i = _GetNextItem(__NI_NEXT);
				_ItemFocusSet(
					(i>0) ? i : nItemIndex,
					FALSE,
					//m_bAnimFinished
					( GetSite().GetAnimated() == NULL )
						? TRUE : FALSE
					);
				if( ! ::IsWindow( hWndOwn ) )
					return true;
			}
			return true;
		}
	} // if( nit != __NI_NOTHING )

	return false;
}

INT CExtPopupMenuWnd::ItemFindByText(
	__EXT_MFC_SAFE_LPCTSTR sText,
	INT nStartIdx, // = -1
	BOOL bRestartAt0 // = TRUE
	) const
{
	bRestartAt0;
	ASSERT_VALID( this );

	if( sText == NULL || sText[0] == _T('\0') )
	{
		ASSERT( FALSE );
		return -1;
	}

int nCount = (int)m_items_all.GetSize(); //ItemGetCount();
int nIdx = (nStartIdx >= 0) ? (nStartIdx + 1) : 0;
	for( ; nIdx < nCount; nIdx++ )
	{
		const MENUITEMDATA & mi = ItemGetInfo(nIdx);
//		if( !mi.AccelCharIsSet() )
//			continue;
//		if( !mi.IsDisplayed() )
//			continue;
//		if( !mi.IsEnabled() )
//			continue;
		if( _tcscmp( sText, mi.GetText() ) == 0 )
		{
			if( nStartIdx != nIdx )
				return nIdx;
		}
	} // for( nIdx = nStartIdx; nIdx < nCount; nIdx++ )
	if( nStartIdx == 0 )
		return -1;
	for( nIdx = 0; nIdx<nStartIdx; nIdx++ )
	{
		const MENUITEMDATA & mi = ItemGetInfo(nIdx);
//		if( !mi.AccelCharIsSet() )
//			continue;
//		if( !mi.IsDisplayed() )
//			continue;
//		if( !mi.IsEnabled() )
//			continue;
		if( _tcscmp( sText, mi.GetText() ) == 0 )
		{
			if( nStartIdx != nIdx )
				return nIdx;
		}
	} // for( nIdx = 0; nIdx<nStartIdx; nIdx++ )
	return -1;
}

INT CExtPopupMenuWnd::ItemFindByAccessChar(
	__EXT_MFC_SAFE_TCHAR chrAccess,
	INT nStartIdx, // = -1
	BOOL bRestartAt0, // = TRUE
	BOOL bSearchDisplayedOnly // = TRUE
	) const
{
	ASSERT_VALID( this );
	if( chrAccess == _T('\0') )
		return -1;
TCHAR szChar[2] = { chrAccess, _T('\0') };
	::CharUpper( szChar );
int cAccelSearch = szChar[0];
	if( cAccelSearch == _T('\0') )
		return -1;
int nCount = (int)m_items_all.GetSize(); //ItemGetCount();
int nIdx = ( nStartIdx >= 0 ) ? ( nStartIdx + 1 ) : 0;
	if( nIdx >= nCount && bRestartAt0 )
		nIdx = 0;
	for( ; nIdx < nCount; nIdx++ )
	{
		const MENUITEMDATA & mi = ItemGetInfo(nIdx);
		if( ! mi.AccelCharIsSet() )
			continue;
		if( ! mi.IsDisplayed() )
		{
			if( bSearchDisplayedOnly )
				continue;
		}
		if( ! mi.IsEnabled() )
			continue;
		TCHAR cAccel = mi.AccelCharGet();
		if( cAccelSearch == cAccel )
		{
			if( nStartIdx != nIdx )
				return nIdx;
		}
	} // for( nIdx = nStartIdx; nIdx < nCount; nIdx++ )
	if( nStartIdx == 0 )
		return -1;
	for( nIdx = 0; nIdx<nStartIdx; nIdx++ )
	{
		const MENUITEMDATA & mi = ItemGetInfo(nIdx);
		if( ! mi.AccelCharIsSet() )
			continue;
		if( ! mi.IsDisplayed() )
		{
			if( bSearchDisplayedOnly )
				continue;
		}
		if( ! mi.IsEnabled() )
			continue;
		TCHAR cAccel = mi.AccelCharGet();
		if( cAccelSearch == cAccel )
		{
			if( nStartIdx != nIdx )
				return nIdx;
		}
	} // for( nIdx = 0; nIdx<nStartIdx; nIdx++ )
	return -1;
}

BOOL CExtPopupMenuWnd::IsCmdKeyActivation(
	LPARAM lParam // = 0
	)
{
	if( (lParam & (1<<29)) != 0 ) // ALT pressed?
		return TRUE;
	
static int nTestKeys[] = {
	VK_MENU,VK_RMENU,VK_LMENU,
	VK_CONTROL,VK_RCONTROL,VK_LCONTROL,
	VK_SHIFT,VK_RSHIFT,VK_LSHIFT
};
	for( int i=0; i<sizeof(nTestKeys)/sizeof(int); i++ )
	{
		if( IsKeyPressed(nTestKeys[i]) )
		{
//			if(		nTestKeys[i] == VK_CONTROL
//				||	nTestKeys[i] == VK_RCONTROL
//				||	nTestKeys[i] == VK_LCONTROL
//				)
//			{
//				if( IsKeyPressed(VK_DOWN) )
//					return FALSE;
//			}
			return TRUE;
		}
	}
	return FALSE;
}

INT CExtPopupMenuWnd::ItemGetCount() const
{
	ASSERT_VALID( this );
INT nCountOfItems = INT(m_items_all.GetSize());
	return nCountOfItems;
}

INT CExtPopupMenuWnd::ItemFindPosForCmdID(
	UINT nCmdID,
	INT nPosStart // = -1
	) const
{
	ASSERT_VALID( this );

INT nPos = (nPosStart < 0) ? 0 : nPosStart + 1;
INT nCount = ItemGetCount();
	for( ; nPos < nCount; nPos++ )
	{
		UINT nCmdID2 = ItemGetCmdID( nPos );
		if( nCmdID == nCmdID2 )
			return nPos;
	}
	return -1; // not found
}

UINT CExtPopupMenuWnd::ItemGetCmdID( // menu_item_type_t values can be returned
	INT nPos
	) const
{
	ASSERT_VALID( this );

	ASSERT( GetTrackingMenu() != this );
INT nCountOfItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfItems )
	{
		ASSERT( FALSE );
		return TYPE_SEPARATOR;
	}
const MENUITEMDATA & mi = ItemGetInfo(nPos);
UINT nCmdID = mi.GetCmdID();
	return nCmdID;
}

bool CExtPopupMenuWnd::ItemSetPopupIcon(
	INT nPos,
	HICON hIcon // = NULL // no icon by default
	)
{
	ASSERT_VALID( this );
CExtCmdIcon _icon;
	if( hIcon != NULL )
		_icon.AssignFromHICON( hIcon, false );
	return ItemSetPopupIcon( nPos, _icon );
}

bool CExtPopupMenuWnd::ItemSetPopupIcon(
	INT nPos,
	const CExtCmdIcon & _icon
	)
{
	ASSERT_VALID( this );
	ASSERT( GetTrackingMenu() != this );
INT nCountOfItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfItems )
	{
		ASSERT( FALSE );
		return false;
	}
MENUITEMDATA & mi = ItemGetInfo( nPos );
	return mi.SetPopupIcon( _icon );
}

bool CExtPopupMenuWnd::ItemSetPopupText(
	INT nPos,
	__EXT_MFC_SAFE_LPCTSTR sText // NULL if empty
	)
{
	ASSERT_VALID( this );

	ASSERT( GetTrackingMenu() != this );
INT nCountOfItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfItems )
	{
		ASSERT( FALSE );
		return false;
	}
MENUITEMDATA & mi = ItemGetInfo(nPos);
	if( !mi.SetPopupText( sText ) )
		return false;
	mi.MeasureItem( NULL );
	return true;
}

bool CExtPopupMenuWnd::ItemSetPopupAccelText(
	INT nPos,
	__EXT_MFC_SAFE_LPCTSTR sText // NULL if empty
	)
{
	ASSERT_VALID( this );

	ASSERT( GetTrackingMenu() != this );
INT nCountOfItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfItems )
	{
		ASSERT( FALSE );
		return false;
	}
MENUITEMDATA & mi = ItemGetInfo(nPos);
bool bRetVal = mi.SetPopupAccelText( sText );
	if( bRetVal )
		_SyncItems();
	return bRetVal;
}

CExtCmdIcon & CExtPopupMenuWnd::ItemGetIcon( INT nPos )
{
	ASSERT_VALID( this );

	ASSERT( GetTrackingMenu() != this );
INT nCountOfItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfItems )
	{
		ASSERT( FALSE );
		static CExtCmdIcon g_EmptyIcon;
		return g_EmptyIcon;
	}
const MENUITEMDATA & mi = ItemGetInfo(nPos);
CExtCmdIcon & _icon = ((MENUITEMDATA &)mi).GetIcon();
	return _icon;
}

const CExtCmdIcon & CExtPopupMenuWnd::ItemGetIcon( INT nPos ) const
{
	ASSERT_VALID( this );
	return
		( const_cast < CExtPopupMenuWnd * > ( this ) )
		-> ItemGetIcon( nPos );
}

CExtPopupMenuWnd::MENUITEMDATA & CExtPopupMenuWnd::ItemGetInfo( INT nPos )
{
	ASSERT_VALID( this );
INT nCountOfItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfItems )
	{
		ASSERT( FALSE );
		AfxThrowNotSupportedException();
	}
	return _GetItemRef( nPos );
}

const CExtPopupMenuWnd::MENUITEMDATA & CExtPopupMenuWnd::ItemGetInfo( INT nPos ) const
{
	ASSERT_VALID( this );
INT nCountOfItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfItems )
	{
		ASSERT( FALSE );
		AfxThrowNotSupportedException();
	}
	return _GetItemRef( nPos );
}


CExtSafeString CExtPopupMenuWnd::ItemGetText( INT nPos ) const
{
	ASSERT_VALID( this );

	ASSERT( GetTrackingMenu() != this );
INT nCountOfItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfItems )
	{
		ASSERT( FALSE );
		return CExtSafeString( _T("") );
	}
const MENUITEMDATA & mi = ItemGetInfo( nPos );
	return CExtSafeString( mi.GetText() );
}

CExtSafeString CExtPopupMenuWnd::ItemGetAccelText( INT nPos ) const
{
	ASSERT_VALID( this );

	ASSERT( GetTrackingMenu() != this );
INT nCountOfItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfItems )
	{
		ASSERT( FALSE );
		return CExtSafeString( _T("") );
	}
const MENUITEMDATA & mi = ItemGetInfo(nPos);
	return CExtSafeString( mi.GetAccelText() );
}

bool CExtPopupMenuWnd::ItemIsDisplayed(
	INT nPos
	) const
{
const MENUITEMDATA & mi = ItemGetInfo( nPos );
	return mi.IsDisplayed();
}

void CExtPopupMenuWnd::ItemSetDisplayed(
	INT nPos,
	bool bForceDisplayed // = true
	)
{
MENUITEMDATA & mi = ItemGetInfo( nPos );
	mi.SetForceDisplayed( bForceDisplayed );
	_SyncItems();
}

CExtPopupMenuWnd * CExtPopupMenuWnd::ItemGetPopup(
	INT nPos
	)
{
	ASSERT_VALID( this );

	ASSERT( GetTrackingMenu() != this );
const CExtPopupMenuWnd * pChild =
		( (const CExtPopupMenuWnd *) this) ->
			ItemGetPopup( nPos );
	if( pChild == NULL )
		return NULL;
	return
		const_cast
			< CExtPopupMenuWnd * >
				(pChild);
}

CExtPopupMenuWnd::TEAROFFNOTIFICATON *
	CExtPopupMenuWnd::TEAROFFNOTIFICATON::
		g_pCurrentTEAROFFNOTIFICATON = NULL;

CExtPopupMenuWnd::TEAROFFNOTIFICATON::
	TEAROFFNOTIFICATON(
		CExtPopupMenuWnd * pPopup,
		const POINT & ptScreenStartPos
		)
		: m_hWndTearOff( NULL )
		, m_hWndHelperPopup( pPopup->GetSafeHwnd() )
		, m_hWndHelperFloat( NULL )
		, m_bHelperForceFloatInvisible( false )
		, m_pPopup( pPopup )
		, m_pHelperTearOffBar( NULL )
		, m_ptScreenStartPos( ptScreenStartPos )
		, m_sizeTearOffArea( 5, 5 )
		, m_rcHelperScreenStartArea( ptScreenStartPos, ptScreenStartPos )
		, m_rcCustomFloatPos( 0, 0, 0, 0 )
		, m_bHelperMenuAreasUpdated( false )
		, m_eTONT( CExtPopupMenuWnd::__ETONT_BEGIN )
		, m_bCanceled( false )
		, m_bFinished( false )
{
	m_rcHelperScreenStartArea.InflateRect( m_sizeTearOffArea );
	ASSERT_VALID( m_pPopup );
	ASSERT( m_hWndHelperPopup != NULL && ::IsWindow(m_hWndHelperPopup) );
}

CExtPopupMenuWnd::TEAROFFNOTIFICATON::
	~TEAROFFNOTIFICATON()
{
	CExtHookSink::RemoveAllWndHooks();
}

bool CExtPopupMenuWnd::TEAROFFNOTIFICATON::
	Notify()
{
HWND hWndNotifyTarget = m_pPopup->GetCmdReceiverHWND();
	if( hWndNotifyTarget == NULL || (! ::IsWindow(hWndNotifyTarget) ) )
		return false;
	m_eTONT = CExtPopupMenuWnd::__ETONT_BEGIN;
	::SendMessage(
		hWndNotifyTarget,
		CExtPopupMenuWnd::g_nMsgTearOff,
		*this,
		0L
		);
	if(		m_hWndTearOff == NULL
		|| (! ::IsWindow(m_hWndTearOff) )
		||	m_bCanceled
		)
		return false;
	m_pHelperTearOffBar = NULL;
CWnd * pWnd = CWnd::FromHandlePermanent( m_hWndTearOff );
	if( pWnd != NULL )
	{
		m_pHelperTearOffBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pWnd );
		if( m_pHelperTearOffBar != NULL )
		{
			if( m_pHelperTearOffBar->m_pDockSite == NULL )
			{
				ASSERT( FALSE );
				m_pHelperTearOffBar = NULL;
			}
		}
	}
	return DoDefaultTearOff();
}

bool CExtPopupMenuWnd::TEAROFFNOTIFICATON::
	DoDefaultTearOff()
{
	g_pCurrentTEAROFFNOTIFICATON = this;
	m_eTONT = CExtPopupMenuWnd::__ETONT_UPDATE;
	m_bHelperForceFloatInvisible = false;
	if( m_hWndHelperFloat == NULL )
	{
		if( m_pHelperTearOffBar != NULL )
		{
			if( m_pHelperTearOffBar->IsFloating() )
				m_hWndHelperFloat =
					m_pHelperTearOffBar->
						GetParentFrame()->
							GetSafeHwnd();
		} // if( m_pHelperTearOffBar != NULL )
		else
			m_hWndHelperFloat = m_hWndTearOff;
	} // if( m_hWndHelperFloat == NULL )
	if( m_hWndHelperFloat != NULL )
	{
		ASSERT( m_hWndHelperFloat != NULL && ::IsWindow(m_hWndHelperFloat) );
		CExtHookSink::SetupHookWndSink( m_hWndHelperFloat );
	} // if( m_hWndHelperFloat != NULL )
	for( ; !( m_bFinished || m_bCanceled); )
	{
		if( ! ::IsWindow(m_hWndHelperPopup) )
			break;
		if( ! ::IsWindow(m_hWndTearOff) )
		{
			m_bFinished = false;
			m_bCanceled = true;
			break;
		}

		CExtPopupMenuWnd::PassMsgLoop( false );

		DoStep();
		CExtPaintManager::stat_PassPaintMessages();
	} // for( ; !( m_bFinished || m_bCanceled); )
	::ReleaseCapture();
	CExtHookSink::RemoveAllWndHooks();
	if( ::IsWindow(m_hWndHelperPopup) )
	{
		ASSERT_VALID( m_pPopup );
		HWND hWndNotifyTarget = m_pPopup->GetCmdReceiverHWND();
		if( hWndNotifyTarget != NULL && ::IsWindow(hWndNotifyTarget) )
		{
			m_eTONT = CExtPopupMenuWnd::__ETONT_FINISH;
			::SendMessage(
				hWndNotifyTarget,
				CExtPopupMenuWnd::g_nMsgTearOff,
				*this,
				0L
				);
		} // if( hWndNotifyTarget != NULL && ::IsWindow(hWndNotifyTarget) )
		if( ::IsWindow(m_hWndHelperPopup) )
		{
			ASSERT_VALID( m_pPopup );
			if( (m_pPopup->GetStyle()&WS_VISIBLE) == 0 )
			{
				CExtToolControlBar::_CloseCustomMenusAll();
				CExtPopupMenuWnd::CancelMenuTracking();
				if( CExtToolControlBar::g_bMenuTracking )
					CExtToolControlBar::_CloseTrackingMenus();
			} // if( (m_pPopup->GetStyle()&WS_VISIBLE) == 0 )
		} // if( ::IsWindow(m_hWndHelperPopup) )
	} // if( ::IsWindow(m_hWndHelperPopup) )
	if( LPVOID(g_pCurrentTEAROFFNOTIFICATON) == LPVOID(this) )
		g_pCurrentTEAROFFNOTIFICATON = NULL;
	return m_bFinished;
}

void CExtPopupMenuWnd::TEAROFFNOTIFICATON::DoStep(
	bool bFinalStep // = false
	)
{
	bFinalStep;
	if( m_bFinished )
		return;
	if(		(! ::IsWindow(m_hWndTearOff) )
		||	(! ::IsWindow(m_hWndHelperPopup) )
		||	( m_pPopup == NULL )
		||	( CWnd::FromHandlePermanent(m_hWndHelperPopup) != m_pPopup )
		)
		return;
	ASSERT_VALID( m_pPopup );
HWND hWndNotifyTarget = m_pPopup->GetCmdReceiverHWND();
	ASSERT( hWndNotifyTarget != NULL && ::IsWindow(hWndNotifyTarget) );
	m_eTONT = CExtPopupMenuWnd::__ETONT_UPDATE;
	::SendMessage(
		hWndNotifyTarget,
		CExtPopupMenuWnd::g_nMsgTearOff,
		*this,
		0L
		);
	if(		m_pHelperTearOffBar != NULL
		&&	m_pHelperTearOffBar->GetSafeHwnd() != NULL
		&&	m_pHelperTearOffBar->m_pDockSite->GetSafeHwnd() != NULL
		)
	{
		ASSERT_VALID( m_pHelperTearOffBar );
		ASSERT( m_pHelperTearOffBar->GetSafeHwnd() == m_hWndTearOff );
		ASSERT_VALID( m_pHelperTearOffBar->m_pDockSite );
		CPoint ptCursorScreen;
		if( ! ::GetCursorPos( &ptCursorScreen ) )
		{
			m_bCanceled = true;
			return;
		}
		if( PtInTearOffArea(ptCursorScreen) )
		{
			if( (m_pPopup->GetStyle()&WS_VISIBLE) == 0 )
			{
				if( m_pHelperTearOffBar->IsVisible() )
					m_pHelperTearOffBar->m_pDockSite->
						ShowControlBar( m_pHelperTearOffBar, FALSE, FALSE );
				m_bHelperForceFloatInvisible = true;
CExtPaintManager::stat_PassPaintMessages();
				m_pPopup->SetWindowPos(
					NULL, 0, 0, 0, 0,
					SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOREDRAW
					|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOMOVE|SWP_NOSIZE
//					|SWP_NOSENDCHANGING
					);
				CExtPopupMenuWnd * pPopup = m_pPopup->GetParentMenuWnd();
				for( int iNL = 0; pPopup != NULL; pPopup = pPopup->GetParentMenuWnd(), iNL++ )
				{
					pPopup->m_ShadowCMBA.PostReShadow();
					pPopup->m_ShadowMain.PostReShadow();
					CClientDC dc( pPopup );
					pPopup->_DoPaint( dc );
				}
				CClientDC dc( m_pPopup );
				m_pPopup->_DoPaint( dc );
				m_bHelperForceFloatInvisible = false;
			} // if( (m_pPopup->GetStyle()&WS_VISIBLE) == 0 )
			if(		m_pPopup->m_ctrlShadow.GetSafeHwnd() != NULL
				&&	(m_pPopup->m_ctrlShadow.GetStyle()&WS_VISIBLE) == 0
				)
				m_pPopup->m_ctrlShadow.ShowWindow( SW_SHOWNA );
		} // if( PtInTearOffArea(ptCursorScreen) )
		else
		{
			bool bDoRecreateShadow = false;
			if( ! m_bHelperMenuAreasUpdated )
			{
				m_bHelperMenuAreasUpdated = true;
				if( m_pHelperTearOffBar->IsVisible() )
				{
					m_pHelperTearOffBar->m_pDockSite->
						ShowControlBar( m_pHelperTearOffBar, FALSE, FALSE );
CExtPaintManager::stat_PassPaintMessages();
					bDoRecreateShadow = true;
					if( m_pHelperTearOffBar->IsFloating() )
					{
						CRect rcFloatingArea;
						m_pHelperTearOffBar->GetParentFrame()->
							GetWindowRect( &rcFloatingArea );
						if( ! m_pPopup->_IsFloatingAreaInstersectsWithMenu(
								rcFloatingArea
								)
							)
							bDoRecreateShadow = false;
					} // if( m_pHelperTearOffBar->IsFloating() )
				} // if( m_pHelperTearOffBar->IsVisible() )
			} // if( ! m_bHelperMenuAreasUpdated )
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			if(		(! m_pHelperTearOffBar->IsFixedDockStyle())
				&&	( m_pHelperTearOffBar->AutoHideModeGet() )
				)
			{
				m_pHelperTearOffBar->AutoHideModeSet( false, false, true, true );
				bDoRecreateShadow = true;
			}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
			if( bDoRecreateShadow )
			{
CExtPaintManager::stat_PassPaintMessages();
				m_pPopup->_DoReCreateShadows();
			}
			{ // block
				CExtPopupMenuWnd * pPopup = m_pPopup;
				for( ; pPopup != NULL; pPopup = pPopup->GetParentMenuWnd() )
				{
					if(		( pPopup->GetStyle() & WS_VISIBLE ) != 0
						&&	pPopup->m_ctrlShadow.GetSafeHwnd() != NULL
						)
					{
CExtPaintManager::stat_PassPaintMessages();
						pPopup->m_ctrlShadow.SetWindowPos(
							pPopup, 0, 0, 0, 0,
							SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW
							);
					}
				}
			} // block
			if( (m_pPopup->GetStyle()&WS_VISIBLE) != 0 )
				m_pPopup->ShowWindow( SW_HIDE );
			if(		m_pPopup->m_ctrlShadow.GetSafeHwnd() != NULL
				&&	(m_pPopup->m_ctrlShadow.GetStyle()&WS_VISIBLE) != 0
				)
				m_pPopup->m_ctrlShadow.ShowWindow( SW_HIDE );
			CPoint ptFloat(
				ptCursorScreen.x - 30,
				ptCursorScreen.y - 10
				);
			CRect rcParentFrame;
			m_pHelperTearOffBar->GetParentFrame()->GetWindowRect( &rcParentFrame );
			if( rcParentFrame.TopLeft() != ptFloat )
			{
CExtPaintManager::stat_PassPaintMessages();
				m_pHelperTearOffBar->FloatControlBar(
					ptFloat
					);
			}
			if(		m_hWndHelperFloat == NULL
				&&	m_pHelperTearOffBar != NULL
				&&	m_pHelperTearOffBar->IsFloating()
				)
			{
				m_hWndHelperFloat =
					m_pHelperTearOffBar->
						GetParentFrame()->
							GetSafeHwnd();
				ASSERT(
						m_hWndHelperFloat != NULL
					&&	::IsWindow( m_hWndHelperFloat )
					);
				CExtHookSink::SetupHookWndSink( m_hWndHelperFloat );
			} // if( m_hWndHelperFloat == NULL )
			if( ! m_pHelperTearOffBar->IsVisible() )
				m_pHelperTearOffBar->m_pDockSite->
					ShowControlBar( m_pHelperTearOffBar, TRUE, FALSE );
			CRect rcTearOffBar;
			m_pHelperTearOffBar->GetWindowRect( &rcTearOffBar );
			CExtPopupMenuWnd * pPopup = m_pPopup;
			for( int iNL = 0; pPopup != NULL; pPopup = pPopup->GetParentMenuWnd(), iNL++ )
			{
				if( (pPopup->GetStyle()&WS_VISIBLE) == 0 )
					continue;
				CRect rcPopup;
				pPopup->GetWindowRect( &rcPopup );
				CRect rcIntersecton;
				if( ! rcIntersecton.IntersectRect(
						&rcTearOffBar,
						&rcPopup
						)
					)
					continue;
				CClientDC dc( pPopup );
				pPopup->_DoPaint( dc );
			}
		} // else from if( PtInTearOffArea(ptCursorScreen) )
	} // if( m_pHelperTearOffBar != NULL ...
	else
	{
		m_bCanceled = true; // this mode not supported yet
		return;
	} // else from if( m_pHelperTearOffBar != NULL ...
	if( ::GetCapture() != m_hWndTearOff )
		::SetCapture( m_hWndTearOff );

}

bool CExtPopupMenuWnd::_IsFloatingAreaInstersectsWithMenu(
	const RECT & rcFloatingArea,
	bool bAnalyzeParents // = true
	) const
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
CRect rcWnd;
	::GetWindowRect( GetSafeHwnd(), &rcWnd );
CRect rcIntersecton;
	if( rcIntersecton.IntersectRect(
			&rcWnd,
			&rcFloatingArea
			)
		)
		return true;
	if( ! bAnalyzeParents )
		return false;
const CExtPopupMenuWnd * pParentMenuWnd = GetParentMenuWnd();
	if( pParentMenuWnd == NULL )
		return false;
	return
		pParentMenuWnd->_IsFloatingAreaInstersectsWithMenu(
			rcFloatingArea,
			true
			);
}

bool CExtPopupMenuWnd::TEAROFFNOTIFICATON::
	PtInTearOffArea( const POINT & ptScreen ) const
{
	ASSERT( m_sizeTearOffArea.cx >= 0 && m_sizeTearOffArea.cy >= 0 );
bool bRetVal =
		m_rcHelperScreenStartArea.PtInRect( ptScreen )
			? true : false;
	if( bRetVal )
		return true;
	if( ! ::IsWindow( m_hWndHelperPopup ) )
		return false;
	ASSERT_VALID( m_pPopup );
CRect rcTearOff;
	m_pPopup->_GetItemRect( CExtPopupMenuWnd::IDX_TEAROFF, rcTearOff, false );
	m_pPopup->ClientToScreen( &rcTearOff );
	rcTearOff.InflateRect( 0, 15 );
	bRetVal =
		rcTearOff.PtInRect( ptScreen )
			? true : false;
	return bRetVal;
}

CExtPopupMenuWnd::TEAROFFNOTIFICATON::
	operator WPARAM() const
{
	return WPARAM( this );
}

CExtPopupMenuWnd::TEAROFFNOTIFICATON *
	CExtPopupMenuWnd::TEAROFFNOTIFICATON::
		FromWPARAM( WPARAM wParam )
{
TEAROFFNOTIFICATON * pTON =
			reinterpret_cast < TEAROFFNOTIFICATON * > ( wParam );
	ASSERT( pTON != NULL );
	return pTON;
}

bool CExtPopupMenuWnd::TEAROFFNOTIFICATON::_OnMouseWheel(
	WPARAM wParam,
	LPARAM lParam
	)
{
	__PROF_UIS_MANAGE_STATE;
	wParam;
	lParam;
	if( LPVOID(g_pCurrentTEAROFFNOTIFICATON) != LPVOID(this) )
		return false;
	return true;
}

bool CExtPopupMenuWnd::TEAROFFNOTIFICATON::_OnMouseMove(
	UINT nFlags,
	CPoint point
	)
{
	__PROF_UIS_MANAGE_STATE;
	nFlags;
	point;
	if( LPVOID(g_pCurrentTEAROFFNOTIFICATON) != LPVOID(this) )
		return false;
	DoStep( true );
	return true;
}

bool CExtPopupMenuWnd::TEAROFFNOTIFICATON::_OnMouseClick(
	UINT nFlags,
	CPoint point
	)
{
	__PROF_UIS_MANAGE_STATE;
	nFlags;
	point;
	if( LPVOID(g_pCurrentTEAROFFNOTIFICATON) != LPVOID(this) )
		return false;
	switch( nFlags)
	{
	case WM_NCLBUTTONUP:
	case WM_NCRBUTTONUP:
	case WM_NCMBUTTONUP:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		DoStep( true );
		m_bFinished = true;
		if( ::IsWindow( m_hWndHelperPopup ) )
			::PostMessage( m_hWndHelperPopup, WM_CANCELMODE, 0L, 0L );
		break;
	}
	return true;
}

bool CExtPopupMenuWnd::TEAROFFNOTIFICATON::_OnKeyDown(
	UINT nChar,
	UINT nRepCnt,
	UINT nFlags
	)
{
	__PROF_UIS_MANAGE_STATE;
	nChar;
	nRepCnt;
	nFlags;
	if( LPVOID(g_pCurrentTEAROFFNOTIFICATON) != LPVOID(this) )
		return false;
	DoStep( true );
	m_bFinished = true;
	if( ::IsWindow( m_hWndHelperPopup ) )
		::PostMessage( m_hWndHelperPopup, WM_CANCELMODE, 0L, 0L );
	return true;
}

bool CExtPopupMenuWnd::TEAROFFNOTIFICATON::OnHookWndMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	UINT nMessage,
	WPARAM & wParam,
	LPARAM & lParam
	)
{
	__PROF_UIS_MANAGE_STATE;
	if(		m_hWndHelperFloat != NULL
		&&	::IsWindow( m_hWndHelperFloat )
		&&	m_hWndHelperPopup != NULL
		&&	::IsWindow( m_hWndHelperPopup )
		)
	{
		ASSERT_VALID( m_pPopup );
		bool bUpdateShadows = false;
		m_bHelperForceFloatInvisible = false;
		switch( nMessage )
		{
		case WM_SHOWWINDOW:
			bUpdateShadows = true;
			if( wParam == 0 )
				m_bHelperForceFloatInvisible = true;
			break;
		case WM_SIZE:
		case WM_WINDOWPOSCHANGED:
			if( m_pHelperTearOffBar == NULL )
			{
				__EXT_MFC_LONG_PTR dwStyle = ::__EXT_MFC_GetWindowLong(m_hWndHelperFloat,GWL_STYLE);
				bUpdateShadows = ( (dwStyle&WS_VISIBLE) != 0 ) ? true : false;
			}
			break;
		default:
			if( nMessage == CExtPopupMenuWnd::g_nMsgCustomFloatPos )
			{
				if( ((HWND)wParam) == m_hWndHelperFloat )
				{
					bUpdateShadows = true;
					LPRECT pRectCustomFloatPos = (LPRECT)lParam;
					ASSERT( pRectCustomFloatPos != NULL );
					m_rcCustomFloatPos = (*pRectCustomFloatPos);
				} // if( ((HWND)wParam) == m_hWndHelperFloat )
			} // if( nMessage == CExtPopupMenuWnd::g_nMsgCustomFloatPos )
			break;
		} // switch( nMessage )
		if( bUpdateShadows )
		{
			lResult =
				CExtHookSink::OnHookWndMsg(
					lResult,
					hWndHooked,
					nMessage,
					wParam,
					lParam
					);
			CExtPopupMenuWnd * pPopup = m_pPopup;
			for( ;	pPopup != NULL;
					pPopup = pPopup->GetParentMenuWnd()
					)
			{
				if( ! pPopup->_IsPopupWithShadows() )
					continue;
				pPopup->Invalidate();
			}
			m_rcCustomFloatPos.SetRectEmpty();
			CExtPaintManager::stat_PassPaintMessages();
			return true;
		} // if( bUpdateShadows )
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

void CExtPopupMenuWnd::_DoReCreateShadows()
{
	ASSERT( GetSafeHwnd() != NULL );
	m_ShadowMain.Destroy();
	m_ShadowCMBA.Destroy();
	if( m_ctrlShadow.GetSafeHwnd() == NULL )
		ShowWindow( SW_HIDE );
CExtPopupMenuWnd * pWndParentMenu = GetParentMenuWnd();
	if( pWndParentMenu == NULL )
	{
		CWnd * pWndTLP = GetTopLevelParent();
		if( pWndTLP != NULL )
			pWndTLP->UpdateWindow();
		CExtPaintManager::stat_PassPaintMessages();
	}
	else
	{
		ASSERT( pWndParentMenu->GetSafeHwnd() != NULL );
		pWndParentMenu->_DoReCreateShadows();
	}
	SetWindowPos(
		NULL, 0, 0, 0, 0,
		SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOREDRAW
		|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOMOVE|SWP_NOSIZE
//		|SWP_NOSENDCHANGING
		);
CClientDC dc( this );
	_DoPaint( dc );
}

void CExtPopupMenuWnd::_DoTearOff()
{
	ASSERT_VALID( this );
	ASSERT( ! _FindCustomizeMode() );
POINT ptScreen = { 0, 0 };
	if( ! ::GetCursorPos( &ptScreen ) )
		return;
	_CoolTipHide();
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	_KeyTipsShow( false );
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
TEAROFFNOTIFICATON _ton( this, ptScreen );
	_ton.Notify();
}

void CExtPopupMenuWnd::_DoExpand()
{
	ASSERT_VALID( this );
	ASSERT( ! _FindCustomizeMode() );

HWND hWndOwn = m_hWnd;
	_ItemFocusDelay();
	if( ! ::IsWindow( hWndOwn ) )
		return;
	_CoolTipHide( false );
	if( ! ::IsWindow( hWndOwn ) )
		return;
	if(		( GetSite().GetAnimated() != NULL )
		||	_CoolTipIsVisible()
		)
		return;
	m_bHelperReCreateDynamicShadowAtTheEndOfNearestAnimation = false;
bool bDynamicShadowMode = false;
	if( m_ctrlShadow.GetSafeHwnd() != NULL )
	{
		bDynamicShadowMode = true;
		m_bHelperReCreateDynamicShadowAtTheEndOfNearestAnimation = true;
	}

	ASSERT( m_bExpandAvailable );
	ASSERT( !m_bExpandWasPressed );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( ( GetSite().GetAnimated() == NULL ) );

	m_bExpandWasPressed = true;
CSize m_sizeFullItems2 = m_sizeFullItems;
	_SyncItems();
	_RecalcLayoutImpl();

	if( m_sizeFullItems != m_sizeFullItems2)
	{ // if size of all items was changed
// 		ASSERT(
// 				m_sizeFullItems.cx >= m_sizeFullItems2.cx
// 			&&	m_sizeFullItems.cy >= m_sizeFullItems2.cy
// 			);

		CRect rcWndOld;
		GetWindowRect( &rcWndOld );
		
		VERIFY(
			SetWindowRgn(
				NULL,
				FALSE
				)
			);
		if( ! bDynamicShadowMode )
		{
			ShowWindow( SW_HIDE	);
			ASSERT( !IsWindowVisible() );
		} // if( ! bDynamicShadowMode )
		if( !rcWndOld.IsRectEmpty() )
		{
			CWnd * pWndTLP = GetTopLevelParent();
			if( pWndTLP != NULL )
			{
				ASSERT_VALID( pWndTLP );
				if( pWndTLP->IsWindowVisible() )
				{
					CRect rcTLP;
					pWndTLP->GetWindowRect( &rcTLP );
					if( !rcTLP.IsRectEmpty() )
					{
						CRect rcIntersection;
						if(	rcIntersection.IntersectRect(
								&rcTLP,
								&rcWndOld
								)
							)
							pWndTLP->UpdateWindow();
					} // if( !rcTLP.IsRectEmpty() )
				} // if( pWndTLP->IsWindowVisible() )
			} // if( pWndTLP != NULL )
		} // if( !rcWndOld.IsRectEmpty() )

		_FreeWinObjects();

		ASSERT( GetSafeHwnd() != NULL );
		ASSERT( ( GetSite().GetAnimated() == NULL ) );

		m_bAnimFinished = false;
		m_AnimationType = __AT_CONTENT_DISPLAY;

		ASSERT( GetSafeHwnd() != NULL );

		// adjust screen position
		CRect rcWnd = _CalcTrackRect();
		
		ASSERT( GetSafeHwnd() != NULL );

		if(		rcWnd.top != rcWndOld.top
			&&	rcWnd.bottom != rcWndOld.bottom
			&&	m_ctrlShadow.GetSafeHwnd() != NULL
			)
		{
			m_ctrlShadow.DestroyWindow();
			m_bHelperReCreateDynamicShadowAtTheEndOfNearestAnimation = false;
		}

		if( m_rgnWnd.GetSafeHandle() != NULL )
		{
			ASSERT( m_bExcludeAreaSpec );
			ASSERT( m_bCombineWithEA );
			ASSERT( m_eCombineAlign != __CMBA_NONE );
			CRgn rgnTmp;
			VERIFY( rgnTmp.CreateRectRgn(0,0,0,0) );
			rgnTmp.CopyRgn( &m_rgnWnd );
			ASSERT( rgnTmp.GetSafeHandle() != NULL );
			VERIFY(
				SetWindowRgn(
					(HRGN)rgnTmp.Detach(),
					FALSE
					)
				);
		} // if( m_rgnWnd.GetSafeHandle() != NULL )
		
		m_bAnimFinished = true;

		m_ePlaySoundOnAnimationFinished =
			CExtSoundPlayer::__ON_MENU_EXPAND_CLICKED;
		m_AnimationType =
			g_bMenuExpandAnimation
				? __AT_CONTENT_EXPAND
				: __AT_CONTENT_DISPLAY
				;
		HWND hWndOwn = m_hWnd;
		_StartAnimation();
		if( ! ::IsWindow( hWndOwn ) )
			return;
		if( m_AnimationType == __AT_NONE )
		{
			m_AnimationType = __AT_CONTENT_DISPLAY;
			_StartAnimation();
			if( ! ::IsWindow( hWndOwn ) )
				return;
			ASSERT( m_AnimationType == __AT_CONTENT_DISPLAY );
		}

		if(		m_bScrollingAvailable
			||	_IsTearOff()
			)
			_RecalcLayoutImpl();
		SetWindowPos(
			NULL,
			rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(),
			SWP_NOACTIVATE|SWP_SHOWWINDOW|SWP_NOZORDER
			);

		ASSERT( IsWindowVisible() );

	} // if size of all items was changed
	else
		UpdateWindow();

	ASSERT( m_hWndCmdReceiver != NULL );
	ASSERT( ::IsWindow(m_hWndCmdReceiver) );
	::SendMessage(
		m_hWndCmdReceiver,
		g_nMsgNotifyMenuExpanded,
		0,
		LPARAM( this )
		);
// 	if( bRestoreShadow )
// 	{
// 		CExtPopupMenuWnd * pPopup = this;
// 		HWND hWndOwn = m_hWnd;
// 		for( ; true; )
// 		{
// 			PassMsgLoop( false );
// 			if( ! ::IsWindow(hWndOwn) )
// 				return;
// 			CWnd * pWndPermanent = CWnd::FromHandlePermanent( hWndOwn );
// 			if( pWndPermanent == NULL )
// 				return;
// 			if( pWndPermanent != pPopup )
// 				return;
// 			if( m_bAnimFinished )
// 				break;
// 		}
// 		if( m_ctrlShadow.GetSafeHwnd() != NULL )
// 			m_ctrlShadow.DestroyWindow();
// 		m_ctrlShadow.Create( m_hWnd, PmBridge_GetPM()->GetMenuShadowSize() );
// 	}
}

bool CExtPopupMenuWnd::IsAllItemsRarelyUsed() const
{
	ASSERT_VALID( this );

INT _iter = 0;
	for( ; _iter < m_items_all.GetSize(); ++_iter )
	{
		const MENUITEMDATA & mi = ItemGetInfo( _iter );
		if( mi.IsDisplayed() || mi.IsExtraMark() )
			return false;
	} // for( ; _iter != m_items_all.end(); ++_iter )
	return true;
}

void CExtPopupMenuWnd::_SyncItems()
{
	ASSERT_VALID( this );
	ASSERT( m_hWndCmdReceiver != NULL );
	ASSERT( ::IsWindow(m_hWndCmdReceiver) );
	if( _IsPalette() )
	{
		_SyncItemsPalette();
		return;
	}

CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );
DWORD dwTrackFlags = TrackFlagsGet();
INT nIconAreaWidthLager = __EXT_MENU_ICON_CX + __EXT_MENU_GAP * 2;
	nIconAreaWidthLager = pPM->UiScalingDo( nIconAreaWidthLager, CExtPaintManager::__EUIST_X );
int nItemIndex = 0, nItemCount = (int)m_items_all.GetSize();
	for( ; nItemIndex < nItemCount; nItemIndex++ )
	{
		MENUITEMDATA & _mii = ItemGetInfo( nItemIndex );
		int nWidth = _mii.GetIconAreaWidth();
		nIconAreaWidthLager =
			max( nIconAreaWidthLager, nWidth );
		_mii.CacheRectsEmpty();
	} // for( ; nItemIndex < nItemCount; nItemIndex++ )

	nIconAreaWidthLager = 
		max( nIconAreaWidthLager, pPM->GetMenuIconAreaWidth() );

	for( nItemIndex = 0; nItemIndex < nItemCount; nItemIndex++ )
	{
		MENUITEMDATA & _mii = ItemGetInfo( nItemIndex );
		_mii.UpdateIconAreaWidth( nIconAreaWidthLager );
	} // for( nItemIndex = 0; nItemIndex < nItemCount; nItemIndex++ )

	m_bExpandAvailable = false;
//	m_sizeFullItems.cx = m_sizeFullItems.cy = 1; // - 2.28
	m_sizeFullItems.cx = m_sizeFullItems.cy = 0; // + 2.28

bool bFullViewMode =
		(	m_bExpandWasPressed
		||	( (dwTrackFlags&TPMX_NO_HIDE_RARELY) != 0 )
		) ? true : false;
int nSeparatorIndex = -1;
int nDisplayedCount = 0;
	for( nItemIndex = 0; nItemIndex < nItemCount; nItemIndex++ )
	{
		MENUITEMDATA & _mii = ItemGetInfo( nItemIndex );
		_mii.SetIndex( nItemIndex );
		if( bFullViewMode )
		{
			_mii.SetDisplayed( true );
			nDisplayedCount ++;
			if( _mii.IsPopup() )
			{
				ASSERT( _mii.GetPopup() != NULL );
				_mii.GetPopup()->_SyncItems();
			} // if( _mii.IsPopup() )
		} // if( bFullViewMode )
		else
		{
			if( _mii.IsPopup() )
			{
				ASSERT( _mii.GetPopup() != NULL );
				_mii.GetPopup()->_SyncItems();
				CExtCmdItem * pCmdItem = _mii.GetCmd();
				if( pCmdItem != NULL )
				{
					bool bRarelyUsed =
						(	(!_FindCustomizeMode())
						&&	pCmdItem->StateIsRarelyUsed()
						) ? true : false;
					if( bRarelyUsed )
					{
						_mii.SetDisplayed( false );
					} // if( bRarelyUsed )
					else
					{
						_mii.SetDisplayed( true );
						nDisplayedCount ++;
						nSeparatorIndex = -1;
					} // else from if( bRarelyUsed )
				} // if( pCmdItem != NULL )
				else if( _mii.IsAllItemsRarelyUsed() )
				{
					_mii.SetDisplayed( false );
				} // else if( _mii.IsAllItemsRarelyUsed() )
				else
				{ // if at least one displayed
					_mii.SetDisplayed( true );
					nDisplayedCount ++;
					nSeparatorIndex = -1;
				} // if at least one displayed
			} // if( _mii.IsPopup() )
			else if( _mii.IsSeparator() )
			{
				if( nSeparatorIndex >= 0 || nDisplayedCount == 0 )
				{
					_mii.SetDisplayed( false );
				} // if( nSeparatorIndex >= 0 || nDisplayedCount == 0 )
				else
				{
					_mii.SetDisplayed( true );
					nDisplayedCount ++;
					nSeparatorIndex = nItemIndex;
				} // else from if( nSeparatorIndex >= 0 || nDisplayedCount == 0 )
			} // else if( _mii.IsSeparator() )
			else
			{ // if command item
				CExtCmdItem * pCmdItem = _mii.GetCmd();
				if( pCmdItem == NULL || _mii.IsExtraMark() )
				{
					_mii.SetDisplayed( true );
					nDisplayedCount ++;
					nSeparatorIndex = -1;
				} // if( pCmdItem == NULL || _mii.IsExtraMark() )
				else if( pCmdItem->StateIsRarelyUsed() )
				{
					_mii.SetDisplayed( false );
				} // else if( pCmdItem->StateIsRarelyUsed() )
				else
				{ // if basic command or frequently used
					_mii.SetDisplayed( true );
					nDisplayedCount ++;
					nSeparatorIndex = -1;
				} // if basic command or frequently used
			} // if command item
		} // else from if( bFullViewMode )
		if( _mii.IsDisplayed() )
		{
			m_sizeFullItems.cy += _mii.GetMeasuredHeight();
			int nMeasuredWidth = _mii.GetMeasuredWidth();
			if( m_sizeFullItems.cx < nMeasuredWidth )
				m_sizeFullItems.cx = nMeasuredWidth;
		} // if( _mii.IsDisplayed() )
		else
		{
			ASSERT( ! m_bExpandWasPressed );
			m_bExpandAvailable = true;
		} // else from if( _mii.IsDisplayed() )
	} // for( nItemIndex = 0; nItemIndex < nItemCount; nItemIndex++ )
	if( nSeparatorIndex >= 0 )
	{ // remove last separator
		ASSERT( ! bFullViewMode );
		MENUITEMDATA & _mii = ItemGetInfo( nSeparatorIndex );
		ASSERT( _mii.IsSeparator() );
		ASSERT( _mii.IsDisplayed() );
		_mii.SetDisplayed( false );
		m_sizeFullItems.cy -= _mii.GetMeasuredHeight();
	} // if( nSeparatorIndex >= 0 )

	if(		GetSafeHwnd() != NULL
		&&	IsSyncFullRowItems()
		)
	{
		CRect rcMB = OnQueryMenuBorderMetrics();
		CRect rcClient;
		_GetClientRect( &rcClient );
		m_sizeFullItems.cx = rcClient.Width() - rcMB.left - rcMB.right;
	}

static const INT nMixDx = 80;
	if( m_sizeFullItems.cx < nMixDx )
		m_sizeFullItems.cx = nMixDx;
}

void CExtPopupMenuWnd::_SyncItemsPalette()
{
	ASSERT_VALID( this );
	ASSERT( m_hWndCmdReceiver != NULL );
	ASSERT( ::IsWindow(m_hWndCmdReceiver) );
	m_bExpandAvailable = false;
	m_sizeFullItems.cx = m_sizeFullItems.cy = 0;
int nItemIndex = 0, nCountOfItems = (int)m_items_all.GetSize();
	for( nItemIndex = 0; nItemIndex < nCountOfItems; nItemIndex++ )
	{
		MENUITEMDATA & _mii = ItemGetInfo( nItemIndex );
		_mii.CacheRectsEmpty();
		_mii.SetToolButton();
		bool bIconMode = true;
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		CExtCustomizeCmdTreeNode * pNode = _mii.GetCmdNode();
		if( pNode != NULL )
		{
			DWORD dwDipslayStyle = pNode->GetFlags() & __ECTN_DISPLAY_MASK;
			if(		dwDipslayStyle == __ECTN_DISPLAY_TEXT_AND_IMAGE
				||	dwDipslayStyle == __ECTN_DISPLAY_TEXTONLY_ALWAYS
				)
				bIconMode = false;
		}
		else
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		{
			LPCTSTR strText = _mii.GetText();
			if(		strText != NULL
				&&	_tcslen( strText ) > 0
				)
				bIconMode = false;
		}
		_mii.SetIconMode( bIconMode );
		_mii.SetIndex( nItemIndex );
		_mii.SetDisplayed( true );
		_mii.UpdateIconAreaWidth( 0 );
		_mii.MeasureItem( NULL );
		if( _mii.IsPopup() )
		{
			ASSERT( _mii.GetPopup() != NULL );
			_mii.GetPopup()->_SyncItems();
		} // if( _mii.IsPopup() )
	} // for( nItemIndex = 0; nItemIndex < nCountOfItems; nItemIndex++ )

CRect rcMB = OnQueryMenuBorderMetrics();
int x = 0, y = 0;
int xRow = x, dyRow = 0, dxMaxRow = 0, nRowStart = 0;
	for( nItemIndex = 0; nItemIndex < nCountOfItems; nItemIndex++ )
	{
		MENUITEMDATA & mi = ItemGetInfo( nItemIndex );
		if( ! mi.IsDisplayed() )
			continue;
		bool bWrap = _IsPaletteItemWrap( nItemIndex );
		const RECT & rcBase = mi.GetBaseRect();
		if( mi.IsSeparator() )
			bWrap = true;
		int nWidth = rcBase.right - rcBase.left;
		int nHeight = rcBase.bottom-rcBase.top;
		dyRow = max( dyRow, nHeight );
		xRow += nWidth;
		if( mi.IsInplaceEdit() )
			xRow += (nItemIndex == 0) ? __EXT_MENU_GAP : (__EXT_MENU_GAP*2);
//		else
//			xRow += (nRowStart == nItemIndex) ? 0 : __EXT_MENU_GAP;
		xRow += 2;
		dxMaxRow = max( dxMaxRow, xRow );
		if( bWrap )
		{
			dxMaxRow = max( dxMaxRow, xRow );
			xRow = x;
			y += dyRow;
			dyRow = 0;
			if( mi.IsSeparator() )
				y +=
					PmBridge_GetPM()->GetSeparatorHeight()
					+ __EXT_MENU_GAP
					;
			nRowStart = nItemIndex + 1;
		}
	} // for( nItemIndex = 0; nItemIndex < nCountOfItems; nItemIndex++ )
	dxMaxRow = max( dxMaxRow, xRow );

	m_sizeFullItems.cx = dxMaxRow + rcMB.left + rcMB.right + __EXT_MENU_GAP;
	m_sizeFullItems.cy = y + dyRow + __EXT_MENU_GAP;
	if( m_sizeFullItems.cx < 20 )
		m_sizeFullItems.cx = 20;
	if( m_sizeFullItems.cy < 10 )
		m_sizeFullItems.cy = 10;
}

void CExtPopupMenuWnd::ResetPaletteWrapping(
	bool bCreateToolBarWrapping, // = false
	int nIndexFirst, // = 0
	int nIndexLast // = -1
	)
{
	ASSERT_VALID( this );
	if( ! _IsPalette() )
		return;
int nCountOfItems = ItemGetCount();
	if( nCountOfItems == 0 )
		return;
	if( nIndexFirst < 0 || nIndexFirst >= nCountOfItems )
		return;
	if( nIndexLast < 0 || nIndexLast >= nCountOfItems )
		nIndexLast = nCountOfItems - 1;
	if( nIndexFirst >= nIndexLast )
		return;
int nIndex, nSummaryWidth = 0;
static const int nAdjustStep = __EXT_MENU_GAP*2;
	for( nIndex = nIndexFirst; nIndex <= nIndexLast; nIndex ++ )
	{
		MENUITEMDATA & mi = ItemGetInfo( nIndex );
		mi.SetToolWrap( false );
		if( ! bCreateToolBarWrapping )
			continue;
		if( mi.IsSeparator() )
			continue;
		int nWidth = mi.GetBaseRect().Width() + nAdjustStep;
		nSummaryWidth += nWidth;
	} // for( nIndex = nIndexFirst; nIndex <= nIndex; nIndex ++ )
	if( ! bCreateToolBarWrapping )
		return;
static const int nAdjustWidth = 150;
	if( nSummaryWidth < nAdjustWidth )
		return;
int dxRow = 0;
int nApproxRowWidth = ::MulDiv( nSummaryWidth, 25, 100 );
	if( nApproxRowWidth < nAdjustWidth )
		nApproxRowWidth = nAdjustWidth;
	for( nIndex = nIndexFirst; nIndex <= nIndexLast; nIndex ++ )
	{
		MENUITEMDATA & mi = ItemGetInfo( nIndex );
		if( mi.IsSeparator() )
		{
			dxRow = 0;
			continue;
		}
		int nWidth = mi.GetBaseRect().Width() + nAdjustStep;
		if(		dxRow >= nApproxRowWidth
			//||	nIndex == nIndexLast
			)
		{
			if(		nIndex > nIndexFirst
				//&&	nIndex != nIndexLast
				)
			{
				MENUITEMDATA & mi = ItemGetInfo( nIndex - 1 );
				mi.SetToolWrap( true );
				dxRow = nWidth;
				continue;
			}
			mi.SetToolWrap( true );
			dxRow = 0;
			continue;
		}
		dxRow += nWidth;
	} // for( nIndex = nIndexFirst; nIndex <= nIndex; nIndex ++ )
}

const CExtPopupMenuWnd * CExtPopupMenuWnd::ItemGetPopup(
	INT nPos
	) const
{
	ASSERT_VALID( this );

	ASSERT( GetTrackingMenu() != this );
INT nCountOfItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfItems )
	{
		ASSERT( FALSE );
		return NULL;
	}
const MENUITEMDATA & mi = ItemGetInfo(nPos);
/*
UINT nCmdID = mi.GetCmdID();
	if( nCmdID != TYPE_POPUP )
		return NULL;
	ASSERT( mi.IsPopup() );
*/

	if( !mi.IsPopup() )
		return NULL;

	return mi.GetPopup();
}

BOOL CExtPopupMenuWnd::ItemRemove(
	INT nPos // = -1 // remove all
	)
{
	ASSERT_VALID( this );
	ASSERT( GetTrackingMenu() != this );

	if( nPos < 0 )
	{
		if( ! _BuildItems( NULL, false ) )
		{
			ASSERT( FALSE );
			return FALSE;
		}
		_SyncItems();
		return TRUE;
	} // if( nPos < 0 )

INT nCountOfItems = ItemGetCount();
	if( nPos >= nCountOfItems )
	{
		ASSERT( FALSE );
		return FALSE;
	}

MENUITEMDATA & mi = ItemGetInfo(nPos);
	if( mi.IsPopup() )
	{
		mi.GetPopup()->_OnCancelMode();
		VERIFY( mi.GetPopup()->_BuildItems( NULL, false ) );
		mi.DestroyPopup();
	}
	m_items_all.RemoveAt( nPos );
	ASSERT( nCountOfItems == m_items_all.GetSize() + 1 );
	m_nCurIndex = -1;

	_SyncItems();
	return TRUE;
}

INT CExtPopupMenuWnd::ItemRemoveAllSeparators()
{
	ASSERT_VALID( this );
	ASSERT( GetTrackingMenu() != this );
INT nCountOfItems = ItemGetCount(), nIndex = 0, nCountRemoved = 0;
	for( ; nIndex < nCountOfItems; )
	{
		MENUITEMDATA & mi = ItemGetInfo(nIndex);
		if( mi.IsSeparator() )
		{
			m_items_all.RemoveAt( nIndex );
			nCountOfItems --;
			nCountRemoved ++;
		}
		else
			nIndex ++;
	}
	m_nCurIndex = -1;
	if( nCountRemoved > 0 )
		_SyncItems();
	return nCountRemoved;
}

BOOL CExtPopupMenuWnd::ItemInsertSpecPopup( // insert specific popup implementation
	CExtPopupMenuWnd * pSpecPopup,
	INT nPos, // = -1 // append
	__EXT_MFC_SAFE_LPCTSTR sText, // = NULL
	HICON hIcon // = NULL
	)
{
	ASSERT_VALID( this );
	ASSERT( pSpecPopup != NULL );
CExtCmdIcon icon;
	if( hIcon != NULL )
		icon.AssignFromHICON( hIcon, false );
	return
		ItemInsertSpecPopup( // insert specific popup implementation
			pSpecPopup,
			nPos,
			sText,
			icon
			);
}

BOOL CExtPopupMenuWnd::ItemInsertSpecPopup( // insert specific popup implementation
	CExtPopupMenuWnd * pSpecPopup,
	INT nPos, // -1 - append
	__EXT_MFC_SAFE_LPCTSTR sText,
	const CExtCmdIcon & icon
	)
{
	ASSERT_VALID( this );

	ASSERT( pSpecPopup != NULL );

INT nCountOfMenuItems = ItemGetCount();
	if( nPos < 0 )
		nPos = nCountOfMenuItems;
	if( nPos > nCountOfMenuItems )
	{
		ASSERT( FALSE );
		return FALSE;
	}

	ASSERT( m_hWndCmdReceiver != NULL );
	ASSERT( ::IsWindow(m_hWndCmdReceiver) );

MENUITEMDATA mi( this );
	mi.SetCmdReceiver( m_hWndCmdReceiver );

	pSpecPopup->m_bTopLevel = false;
	mi.SetPopup( nPos, sText, icon, pSpecPopup );
	ASSERT( mi.GetPopup()->m_pWndParentMenu == NULL ); // still not initialized
	ASSERT( !mi.GetPopup()->m_bTopLevel );
	mi.GetPopup()->m_pWndParentMenu = this;
	mi.SetDisplayed( true );

	_InsertItem( nPos, mi );

	if( m_bTopLevel )
		_SyncItems();
	else
	{
		ASSERT( m_pWndParentMenu != NULL );
		m_pWndParentMenu->_SyncItems();
	}

	return TRUE;
}

bool CExtPopupMenuWnd::ItemPaletteRowWrapGet(
	INT nPos
	) const
{
	ASSERT_VALID( this );
INT nCountOfMenuItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfMenuItems )
		return false;
const MENUITEMDATA & mi = ItemGetInfo( nPos );
bool bWrap = mi.IsToolWrap();
	return bWrap;
}

void CExtPopupMenuWnd::ItemPaletteRowWrapSet(
	INT nPos,
	bool bWrap // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( GetTrackingMenu() != this );
INT nCountOfMenuItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfMenuItems )
		return;
MENUITEMDATA & mi = ItemGetInfo( nPos );
	mi.SetToolWrap( bWrap );
}

bool CExtPopupMenuWnd::ItemToolLayoutGet(
	INT nPos
	) const
{
	ASSERT_VALID( this );
	ASSERT( GetTrackingMenu() != this );
INT nCountOfMenuItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfMenuItems )
		return false;
const MENUITEMDATA & mi = ItemGetInfo( nPos );
bool bToolButton = mi.IsToolButton();
	return bToolButton;
}

void CExtPopupMenuWnd::ItemToolLayoutSet(
	INT nPos,
	bool bToolButton // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( GetTrackingMenu() != this );
INT nCountOfMenuItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfMenuItems )
		return;
MENUITEMDATA & mi = ItemGetInfo( nPos );
	mi.SetToolButton( bToolButton );
}

bool CExtPopupMenuWnd::ItemEnabledGet(
	INT nPos
	) const
{
	ASSERT_VALID( this );
	ASSERT( GetTrackingMenu() != this );
INT nCountOfMenuItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfMenuItems )
		return false;
const MENUITEMDATA & mi = ItemGetInfo( nPos );
bool bEnabled = mi.IsEnabled();
	return bEnabled;
}

void CExtPopupMenuWnd::ItemEnabledSet(
	INT nPos,
	bool bEnabled // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( GetTrackingMenu() != this );
INT nCountOfMenuItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfMenuItems )
		return;
MENUITEMDATA & mi = ItemGetInfo( nPos );
	mi.Enable( bEnabled );
}

bool CExtPopupMenuWnd::ItemBoldGet(
	INT nPos
	) const
{
	ASSERT_VALID( this );
INT nCountOfMenuItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfMenuItems )
		return false;
const MENUITEMDATA & mi = ItemGetInfo( nPos );
bool bBold = mi.IsBold();
	return bBold;
}

void CExtPopupMenuWnd::ItemBoldSet(
	INT nPos,
	bool bBold // = true
	)
{
	ASSERT_VALID( this );
INT nCountOfMenuItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfMenuItems )
		return;
MENUITEMDATA & mi = ItemGetInfo( nPos );
	mi.SetBold( bBold );
}

bool CExtPopupMenuWnd::ItemDefaultGet(
	INT nPos
	) const
{
	ASSERT_VALID( this );
INT nCountOfMenuItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfMenuItems )
		return false;
const MENUITEMDATA & mi = ItemGetInfo( nPos );
bool bDefault = mi.IsDefault();
	return bDefault;
}

void CExtPopupMenuWnd::ItemDefaultSet(
	INT nPos,
	bool bDefault // = true
	)
{
	ASSERT_VALID( this );
INT nCountOfMenuItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfMenuItems )
		return;
MENUITEMDATA & mi = ItemGetInfo( nPos );
	mi.SetDefault( bDefault );
	mi.SetBold( bDefault );
}

INT CExtPopupMenuWnd::ItemDefaultFind() const
{
	ASSERT_VALID( this );
INT nCountOfMenuItems = ItemGetCount();
	if( nCountOfMenuItems == 0 )
		return -1;
	for( INT nIndex = 0; nIndex < nCountOfMenuItems; nIndex++ )
	{
		if( ItemDefaultGet( nIndex ) )
			return nIndex;
	}
	return -1;
}

void CExtPopupMenuWnd::ItemDefaultUnset()
{
	ASSERT_VALID( this );
INT nCountOfMenuItems = ItemGetCount();
	if( nCountOfMenuItems == 0 )
		return;
	for( INT nIndex = 0; nIndex < nCountOfMenuItems; nIndex++ )
		ItemDefaultSet( nIndex, false );
}

bool CExtPopupMenuWnd::ItemEnabledAppearanceGet(
	INT nPos
	) const
{
	ASSERT_VALID( this );
	ASSERT( GetTrackingMenu() != this );
INT nCountOfMenuItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfMenuItems )
		return false;
const MENUITEMDATA & mi = ItemGetInfo( nPos );
bool bEnabledAppearance = mi.IsEnabledAppearance();
	return bEnabledAppearance;
}

void CExtPopupMenuWnd::ItemEnabledAppearanceSet(
	INT nPos,
	bool bEnabledAppearance // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( GetTrackingMenu() != this );
INT nCountOfMenuItems = ItemGetCount();
	if( nPos < 0 || nPos >= nCountOfMenuItems )
		return;
MENUITEMDATA & mi = ItemGetInfo( nPos );
	mi.SetEnabledAppearance( bEnabledAppearance );
}

BOOL CExtPopupMenuWnd::ItemInsert(
	UINT nCmdID, // = TYPE_SEPARATOR,
	INT nPos, // = -1 // append
	__EXT_MFC_SAFE_LPCTSTR sText, // = NULL // for TYPE_POPUP only
	HICON hIcon, // = NULL // for TYPE_POPUP only
	HWND hWndSpecCmdReceiver // = NULL // specific command receiver
	)
{
	ASSERT_VALID( this );
CExtCmdIcon icon;
	if( hIcon != NULL )
		icon.AssignFromHICON( hIcon, false );
	return
		ItemInsert(
			nCmdID,
			nPos,
			sText,
			icon,
			hWndSpecCmdReceiver
			);
}

BOOL CExtPopupMenuWnd::ItemInsert(
	UINT nCmdID,
	INT nPos, // -1 - append
	__EXT_MFC_SAFE_LPCTSTR sText, // for TYPE_POPUP only
	const CExtCmdIcon & icon, // for TYPE_POPUP only
	HWND hWndSpecCmdReceiver // = NULL // specific command receiver
	)
{
	ASSERT_VALID( this );
	ASSERT( GetTrackingMenu() != this );
INT nCountOfMenuItems = ItemGetCount();
	if( nPos < 0 )
		nPos = nCountOfMenuItems;
	if( nPos > nCountOfMenuItems )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	ASSERT( m_hWndCmdReceiver != NULL );
	ASSERT( ::IsWindow(m_hWndCmdReceiver) );
MENUITEMDATA mi( this );
	mi.SetCmdReceiver( 
		(hWndSpecCmdReceiver != NULL)
			? hWndSpecCmdReceiver
			: m_hWndCmdReceiver
		);
	switch( nCmdID )
	{
	case TYPE_POPUP:
		mi.SetPopup( nPos, sText, icon );
		break; // case TYPE_POPUP
	case TYPE_SEPARATOR:
		mi.SetSeparator( nPos );
		break; // case TYPE_SEPARATOR
	default:
	{
		ASSERT( m_hWndCmdReceiver != NULL );
		ASSERT( ::IsWindow(m_hWndCmdReceiver) );
		CExtCmdItem * pCmdItem =
			g_CmdManager->CmdGetPtr(
				g_CmdManager->ProfileNameFromWnd(
					(hWndSpecCmdReceiver != NULL)
						? hWndSpecCmdReceiver
						: m_hWndCmdReceiver
					),
				nCmdID
				);
		ASSERT( pCmdItem != NULL );
		if( pCmdItem == NULL )
			return FALSE;
		VERIFY(
			mi.UpdateCmdManagerCommand(
				pCmdItem,
				nPos
				)
			);
	}
	break; // default
	} // switch( nCmdID )
	if( mi.IsPopup() )
	{
		ASSERT( mi.GetPopup()->m_pWndParentMenu == NULL ); // still not initialized
		mi.GetPopup()->m_pWndParentMenu = this;
		ASSERT( !mi.GetPopup()->m_bTopLevel );
	}
	_InsertItem( nPos, mi );
	if( m_bTopLevel )
		_SyncItems();
	else
	{
		//ASSERT( m_pWndParentMenu != NULL );
		//m_pWndParentMenu->_SyncItems();
		if( m_pWndParentMenu != NULL )
			m_pWndParentMenu->_SyncItems();
	}
	return TRUE;
}

BOOL CExtPopupMenuWnd::ItemInsertCommand( // no references to command manager
	UINT nCmdID, // = TYPE_SEPARATOR
	INT nPos, // = -1 // append
	__EXT_MFC_SAFE_LPCTSTR sItemText, // = NULL
	__EXT_MFC_SAFE_LPCTSTR sAccelText, // = NULL
	HICON hIcon, // = NULL
	bool bCopyIcon, // = true
	int nCheck, // = 0 // 0 - no check, 1 - checked, 2 - indeterminate, 3 - radio
	HWND hWndSpecCmdReceiver // = NULL
	)
{
CExtCmdIcon icon;
	if( hIcon != NULL )
		icon.AssignFromHICON( hIcon, bCopyIcon );
BOOL bRetVal =
		ItemInsertCommand(
			nCmdID, nPos, sItemText, sAccelText,
			icon, nCheck, hWndSpecCmdReceiver
			);
	return bRetVal;
}

INT CExtPopupMenuWnd::ItemGetIndexOf(
	const CExtPopupMenuWnd::MENUITEMDATA * pMII
	) const
{
	ASSERT_VALID( this );
	if(		pMII == NULL
		||	pMII->GetOwner() != this
		)
		return -1;
INT nIndex, nCount = ItemGetCount();
	for( nIndex = 0; nIndex < nCount; nIndex ++ )
	{
		const CExtPopupMenuWnd::MENUITEMDATA * pOwnMII = &( ItemGetInfo( nIndex ) );
		if( pOwnMII == pMII )
			return nIndex;
	}
	return -1;
}

BOOL CExtPopupMenuWnd::ItemInsertCommand( // no references to command manager
	UINT nCmdID,
	INT nPos, // -1 append
	__EXT_MFC_SAFE_LPCTSTR sItemText, // may be NULL
	__EXT_MFC_SAFE_LPCTSTR sAccelText, // may be NULL
	const CExtCmdIcon & icon,
	int nCheck, // = 0 // 0 - no check, 1 - checked, 2 - indeterminate, 3 - radio
	HWND hWndSpecCmdReceiver // = NULL
	)
{
	ASSERT_VALID( this );
	ASSERT( GetTrackingMenu() != this );
INT nCountOfMenuItems = ItemGetCount();
	if( nPos < 0 )
		nPos = nCountOfMenuItems;
	if( nPos > nCountOfMenuItems )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	ASSERT( m_hWndCmdReceiver != NULL );
	ASSERT( ::IsWindow(m_hWndCmdReceiver) );
MENUITEMDATA mi( this );
	mi.SetCmdReceiver( 
		(hWndSpecCmdReceiver != NULL)
			? hWndSpecCmdReceiver
			: m_hWndCmdReceiver
		);
	mi.SetNoCmdUI( true );

	switch( nCmdID )
	{
	case TYPE_POPUP:
		mi.SetPopup(
			nPos,
			(sItemText == NULL) ? _T("") : sItemText,
			NULL
			);
		mi.SetPopupIcon( icon );
		break; // case TYPE_POPUP
	case TYPE_SEPARATOR:
		mi.SetSeparator( nPos );
		break; // case TYPE_SEPARATOR
	default:
	{
		ASSERT( m_hWndCmdReceiver != NULL );
		ASSERT( ::IsWindow(m_hWndCmdReceiver) );
		mi.SetCmdID( nCmdID );
		mi.SetText( (sItemText == NULL) ? _T("") : sItemText );
		mi.SetAccelText( (sAccelText == NULL) ? _T("") : sAccelText );
		mi.AccelCharInit();
		mi.SetPopupIcon( icon );
		switch( nCheck )
		{
		case 1: mi.Check( true ); break;
		case 2: mi.Indeterminate( true ); break;
		case 3: mi.Radio( true ); break;
		} // switch( nCheck )
		mi.MeasureItem( NULL );
	}
	break; // default
	} // switch( nCmdID )
	mi.SetIndex( nPos );
	mi.SetDisplayed( true );
	if( mi.IsPopup() )
	{
		ASSERT( mi.GetPopup()->m_pWndParentMenu == NULL ); // still not initialized
		mi.GetPopup()->m_pWndParentMenu = this;
		ASSERT( !mi.GetPopup()->m_bTopLevel );
	}
	_InsertItem( nPos, mi );
	if( m_bTopLevel )
		_SyncItems();
	else
	{
		if( m_pWndParentMenu != NULL )
			m_pWndParentMenu->_SyncItems();
	}
	return TRUE;
}

void CExtPopupMenuWnd::SetLeftAreaWidth(
	UINT nWidth // = 0
	)
{
	m_nLeftAreaWidth = nWidth;
}

UINT CExtPopupMenuWnd::GetLeftAreaWidth() const
{
	return m_nLeftAreaWidth;
}

BOOL CExtPopupMenuWnd::CreatePopupMenu( HWND hWndCmdRecv )
{
	ASSERT_VALID( this );
	ASSERT( ::IsWindow(hWndCmdRecv) );

	ASSERT( GetTrackingMenu() != this );

	if( ItemGetCount() != 0 )
		ItemRemove( -1 );

	if(			m_pWndParentMenu != NULL
		//||	m_bTopLevel
		//||	ItemGetCount() != 0
		)
	{
		// already created
		ASSERT( FALSE );
		return FALSE;
	}
	m_sizeFullItems.cx = m_sizeFullItems.cy = 1;
	m_bTopLevel = true;
	m_pWndParentMenu = NULL;
	m_hWndCmdReceiver = hWndCmdRecv;
	
	return TRUE;
}

void CExtPopupMenuWnd::SetCmdTargetToAllItems(
	HWND hWndSpecCmdReceiver, // = NULL
	bool bOnlyThisLevel // = false
	)
{
	ASSERT_VALID( this );

	ASSERT( GetTrackingMenu() != this );

INT iter = 0;
	for( ; iter < m_items_all.GetSize(); iter++)
	{
		MENUITEMDATA & mi = ItemGetInfo( iter );
		if( mi.IsSeparator() )
			continue;
		if( mi.IsPopup() )
		{
			if( !bOnlyThisLevel )
				mi.GetPopup()->SetCmdTargetToAllItems(
					hWndSpecCmdReceiver,
					true
					);
		}
		else
		{
			mi.SetCmdReceiver( hWndSpecCmdReceiver );
		}
	}
}

//CExtPopupMenuWnd::const_items_container_t &
//	CExtPopupMenuWnd::_GetItemsContaner() const
//{
//	return m_items_all;
//}
//
//CExtPopupMenuWnd::items_container_t &
//	CExtPopupMenuWnd::_GetItemsContaner()
//{
//	return m_items_all;
//}

CExtPopupMenuWnd::MENUITEMDATA &
	CExtPopupMenuWnd::_GetItemRef(int nIndex)
{
	ASSERT_VALID( this );

	ASSERT( nIndex >= 0 && nIndex < m_items_all.GetSize() );
MENUITEMDATA & mi = m_items_all.ElementAt( nIndex );
	return mi;
}

const CExtPopupMenuWnd::MENUITEMDATA &
	CExtPopupMenuWnd::_GetItemRef(int nIndex) const
{
	ASSERT_VALID( this );

	return
		( const_cast < CExtPopupMenuWnd * > ( this ) ) ->
			_GetItemRef( nIndex );
}

void CExtPopupMenuWnd::_InsertItem(
	int nInsertBefore,
	MENUITEMDATA & mi,
	bool bNoRefToCmdMngr // = false
	)
{
	ASSERT_VALID( this );
	bNoRefToCmdMngr;
	m_items_all.InsertAt(
		(nInsertBefore < 0)
			? m_items_all.GetSize()
			: nInsertBefore
			,
		mi
		);
}

void CExtPopupBaseWnd::_EndAnimation()
{
	ASSERT_VALID( this );
	_SurfacesDelete();
}

void CExtPopupMenuWnd::_EndAnimation()
{
	ASSERT_VALID( this );
	CExtPopupBaseWnd::_EndAnimation();
	Invalidate( FALSE );
	_SetCapture();
	if( _IsPopupWithShadowsDynamic() )
	{
		if(		m_bHelperReCreateDynamicShadowAtTheEndOfNearestAnimation
			&&	m_ctrlShadow.GetSafeHwnd() != NULL
			)
			m_ctrlShadow.DestroyWindow();
		m_bHelperReCreateDynamicShadowAtTheEndOfNearestAnimation = false;
		if( m_ctrlShadow.GetSafeHwnd() == NULL )
			m_ctrlShadow.Create( m_hWnd, PmBridge_GetPM()->GetMenuShadowSize() );
	}
}

bool CExtPopupBaseWnd::_IsPopupWithShadows() const
{
	if( ! CExtPopupMenuWnd::g_bMenuWithShadows )
		return false;
	if( m_ctrlShadow.IsAvailable() )
		return false;
	return true;
}

bool CExtPopupBaseWnd::_IsPopupWithShadowsDynamic() const
{
	if( ! CExtPopupMenuWnd::g_bMenuWithShadows )
		return false;
	if( ! m_ctrlShadow.IsAvailable() )
		return false;
	return true;
}

bool CExtPopupMenuWnd::_IsPopupWithShadows() const
{
	if( ! CExtPopupBaseWnd::_IsPopupWithShadows() )
		return false;
DWORD dwTrackFlags = TrackFlagsGet();
	if( ( dwTrackFlags & TPMX_NO_SHADOWS ) != 0 )
		return false;
//	if( ( dwTrackFlags & TPMX_NO_DYNAMIC_SHADOWS ) == 0 )
//		return false; // replacement with dynamic shadow
	return true;
}

bool CExtPopupMenuWnd::_IsPopupWithShadowsDynamic() const
{
DWORD dwTrackFlags = TrackFlagsGet();
	if( ( dwTrackFlags & TPMX_NO_DYNAMIC_SHADOWS ) != 0 )
		return false;
	return CExtPopupBaseWnd::_IsPopupWithShadowsDynamic();
}

bool CExtPopupMenuWnd::_IsRibbonMode() const
{
DWORD dwTrackFlags = TrackFlagsGet();
	if( dwTrackFlags & TPMX_RIBBON_MODE )
		return true;
	if( m_pWndParentMenu != NULL )
		return m_pWndParentMenu->_IsRibbonMode();
	return false;
}

bool CExtPopupBaseWnd::_IsFadeOutAnimation() const
{
	return false;
}

bool CExtPopupMenuWnd::_IsFadeOutAnimation() const
{
	if( _ItemFocusChangingIsInProgress() )
		return false;
	if( ! g_PaintManager.m_bIsWin2000orLater )
		return false;
	if( g_PaintManager.m_pfnSetLayeredWindowAttributes == NULL )
		return false;
	if(		m_nFadeOutAnimationStepCount <= 0
		||	m_nFadeOutAnimationEllapse <= 0
		)
		return false;
const CExtPopupMenuWnd * pTLP = GetTopLevelMenuWnd();
	if( pTLP == NULL )
		return false;
DWORD dwTrackFlags = pTLP->TrackFlagsGet();
	if( dwTrackFlags & TPMX_NO_FADE_OUT_ANIMATION )
		return false;
#if (!defined __EXT_MFC_NO_RIBBON_BAR)
	if(		m_hWndNotifyMenuClosed != NULL
		&&	::IsWindow( m_hWndNotifyMenuClosed )
		)
	{
		CWnd * pWnd = CWnd::FromHandlePermanent( m_hWndNotifyMenuClosed );
		if( pWnd != NULL )
		{
			CExtRibbonPage * pRibbonPage = DYNAMIC_DOWNCAST( CExtRibbonPage, pWnd );
			if(		pRibbonPage != NULL
				&&	pRibbonPage->m_bHelperPopupMode
				)
				return false;
		}
	}
#endif // (!defined __EXT_MFC_NO_RIBBON_BAR)
	if( dwTrackFlags & TPMX_RIBBON_MODE )
		return true;
	if( m_pWndParentMenu != NULL )
		return m_pWndParentMenu->_IsFadeOutAnimation();
	return false;
}

int CExtPopupBaseWnd::OnQueryMenuShadowSize() const
{
	if( ! _IsPopupWithShadows() )
		return 0;
//int nBitsPerPixel = CExtPaintManager::stat_GetBPP();
//	if( nBitsPerPixel <= 8 )
//		return 0;
	return PmBridge_GetPM()->GetMenuShadowSize();
};

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
void CExtPopupMenuWnd::_SetDropIndexBefore(
	int nIndex // = -1 // reset
	)
{
	ASSERT_VALID( this );
	if( m_nHelperDropIndexBefore == nIndex )
		return;
	m_nHelperDropIndexBefore = nIndex;
	if( GetSafeHwnd() == NULL )
		return;
CClientDC dc( this );
	_DoPaint( dc );
}

// CExtCustomizeSite::ICustomizeDropTarget
DROPEFFECT CExtPopupMenuWnd::OnCustomizeTargetOver(
	CExtCustomizeSite::CCmdDragInfo & _dragInfo,
	CPoint point,
	DWORD dwKeyState
	)
{
	ASSERT_VALID( this );
	ASSERT( !_dragInfo.IsEmpty() );
	dwKeyState;
	point;

	if( _IsPalette() )
		return DROPEFFECT_NONE;

	if( m_pDragSrcNode != NULL )
		m_bHelperDragOverPassed = true;

	ASSERT_VALID( m_pNode );
	ASSERT_VALID( _dragInfo.m_pDropNodeC );
	if(		m_pNode == _dragInfo.m_pDropNodeC
		||	_dragInfo.m_pDropNodeC->IsChild( m_pNode )
		)
		return DROPEFFECT_NONE;

//	_DoResetChainPositions();

	if( ItemGetCount() == 0 )
	{
		if( (dwKeyState & MK_CONTROL) != NULL )
			return DROPEFFECT_COPY;
		return DROPEFFECT_MOVE;
	}

int nIndex = _HitTest( point );
	if( nIndex < 0 )
	{
		if(		nIndex == IDX_SCROLL_TOP
			||	nIndex == IDX_SCROLL_BOTTOM
			)
		{
			HWND hWndOwn = m_hWnd;
			if( ! ::IsWindow( hWndOwn ) )
				return DROPEFFECT_NONE;
			_StartScrolling( nIndex );
			if( ! ::IsWindow( hWndOwn ) )
				return DROPEFFECT_NONE;
		}
		_SetDropIndexBefore();
		return DROPEFFECT_NONE;
	}
MENUITEMDATA & miPreTest = CExtPopupMenuWnd::ItemGetInfo( nIndex );
	if( miPreTest.IsSeparator() )
	{
		if( nIndex == 0 )
		{
			ASSERT( FALSE );
			_SetDropIndexBefore();
			_ItemFocusCancel( TRUE );
			return DROPEFFECT_NONE;
		} // if( nIndex == 0 )
		nIndex--;
	} // if( miPreTest.IsSeparator() )
MENUITEMDATA & mi = CExtPopupMenuWnd::ItemGetInfo( nIndex );
	if( mi.IsSeparator() )
	{
		ASSERT( FALSE );
		_SetDropIndexBefore();
		_ItemFocusCancel( TRUE );
		return DROPEFFECT_NONE;
	} // if( mi.IsSeparator() )
CRect rcItem;
	_GetItemRect( nIndex, rcItem );
int nMidY = rcItem.CenterPoint().y;
	if( point.y > nMidY )
		nIndex++;
	_SetDropIndexBefore( nIndex );
	if( mi.IsPopup() )
	{
		HWND hWndOwn = m_hWnd;
		_ItemFocusSet( mi.GetIndex(), TRUE, TRUE, FALSE );
		if( ! ::IsWindow( hWndOwn ) )
			return DROPEFFECT_NONE;
	}
	else
	{
		HWND hWndOwn = m_hWnd;
		_ItemFocusCancel( TRUE );
		if( ! ::IsWindow( hWndOwn ) )
			return DROPEFFECT_NONE;
	}
	if( (dwKeyState & MK_CONTROL) != NULL )
		return DROPEFFECT_COPY;
	return DROPEFFECT_MOVE;
}
void CExtPopupMenuWnd::OnCustomizeTargetLeave()
{
	ASSERT_VALID( this );
	if( _IsPalette() )
		return;
	_SetDropIndexBefore();
}

bool CExtPopupMenuWnd::OnCustomizeTargetDrop(
	CExtCustomizeSite::CCmdDragInfo & _dragInfo,
	CPoint point,
	DROPEFFECT de
	)
{
	ASSERT_VALID( this );
	ASSERT( !_dragInfo.IsEmpty() );
	_dragInfo;
	point;
	ASSERT(
			de == DROPEFFECT_NONE
		||	de == DROPEFFECT_MOVE
		||	de == DROPEFFECT_COPY
		);
	if( _IsPalette() )
		return false;
int nItemCount = ItemGetCount();
	if(		de == DROPEFFECT_NONE
		||	( nItemCount > 0 && m_nHelperDropIndexBefore < 0 )
		)
	{
		_SetDropIndexBefore();
		m_ptHelperDragStart.x = m_ptHelperDragStart.y = 0;
		return false;
	}
CExtCustomizeSite * pSite = _FindCustomizeSite();
	ASSERT( pSite != NULL );
	ASSERT_VALID( m_pNode );
	ASSERT_VALID( _dragInfo.m_pDropNodeC );
bool bRetVal = true;
	m_bHelperNoHideChild = true;
int nNodeInsertPos = -1;
CExtCustomizeCmdTreeNode * pClonedNode = _dragInfo.m_pDropNodeC->CloneNode( pSite );
	ASSERT_VALID( pClonedNode );
	if( _dragInfo.m_pDropNodeI != NULL )
	{
		ASSERT_VALID( _dragInfo.m_pDropNodeI );
		// reset basic command ID
		pClonedNode->SetCmdID( _dragInfo.m_pDropNodeI->GetCmdID( false ), false );
		pClonedNode->SetCmdID( _dragInfo.m_pDropNodeC->GetCmdID( true ), true );
	} // if( _dragInfo.m_pDropNodeI != NULL )
	if( nItemCount == 0 )
	{
		nNodeInsertPos = 0;
		pClonedNode->ModifyFlags( 0, __ECTN_GROUP_START );
	} // if( nItemCount == 0 )
	else
	{
		ASSERT( m_nHelperDropIndexBefore >= 0 && m_nHelperDropIndexBefore <= nItemCount );
		if( m_nHelperDropIndexBefore == nItemCount )
		{
			nNodeInsertPos = m_pNode->GetNodeCount();
		} // if( m_nHelperDropIndexBefore == nItemCount )
		else
		{
			if( m_nHelperDropIndexBefore == 0 )
				pClonedNode->ModifyFlags( 0, __ECTN_GROUP_START );
			MENUITEMDATA & miDropPos = ItemGetInfo( m_nHelperDropIndexBefore );
			bool bDropPosIsExactlyBeforeSeparator = miDropPos.IsSeparator();
			CExtCustomizeCmdTreeNode * pDropBeforeNode =
				bDropPosIsExactlyBeforeSeparator
					? ( ItemGetInfo( m_nHelperDropIndexBefore + 1 ).GetCmdNode() )
					: ( miDropPos.GetCmdNode() )
					;
			ASSERT_VALID( pDropBeforeNode );
			nNodeInsertPos = pDropBeforeNode->GetOwnIndex();
			if( (! bDropPosIsExactlyBeforeSeparator ) && (pDropBeforeNode->GetFlags() & __ECTN_GROUP_START) != 0 )
			{
				ASSERT( m_nHelperDropIndexBefore > 0 );
				pClonedNode->ModifyFlags( __ECTN_GROUP_START, 0 );
				pDropBeforeNode->ModifyFlags( 0, __ECTN_GROUP_START );
			}
			if( m_pDragSrcNode != NULL )
			{
				ASSERT( 0 <= m_nDragSrcIdx && m_nDragSrcIdx < nItemCount );
				if( pDropBeforeNode == m_pDragSrcNode )
				{
					delete pClonedNode;
					_SetDropIndexBefore();
					ASSERT( m_nDragSrcIdx >= 0 && m_nDragSrcIdx < nItemCount );
					HWND hWndOwn = m_hWnd;
					_ItemFocusSet( m_nDragSrcIdx, TRUE, TRUE, FALSE );
					if( ! ::IsWindow( hWndOwn ) )
						return true;
					m_bHelperNoHideChild = true;
					CClientDC dc( this );
					_DoPaint( dc );
					m_bHelperNoRemoveSrc = true;
					return true;
				} // if( pDropBeforeNode == m_pDragSrcNode )
				int nDragNodeIdx = m_pDragSrcNode->GetOwnIndex();
				if( nDragNodeIdx > 0 )
				{
					CExtCustomizeCmdTreeNode * pDragNodePr = 
						m_pDragSrcNode->GetParentNode()->ElementAt( nDragNodeIdx-1 );
					ASSERT_VALID( pDragNodePr );
					if( pDragNodePr == m_pDragSrcNode )
					{
						delete pClonedNode;
						_SetDropIndexBefore();
						ASSERT( m_nDragSrcIdx >= 0 && m_nDragSrcIdx < nItemCount );
						HWND hWndOwn = m_hWnd;
						_ItemFocusSet( m_nDragSrcIdx, TRUE, TRUE, FALSE );
						if( ! ::IsWindow( hWndOwn ) )
							return true;
						m_bHelperNoHideChild = true;
						CClientDC dc( this );
						_DoPaint( dc );
						m_bHelperNoRemoveSrc = true;
						return true;
					} // if( pDragNodePr == m_pDragSrcNode )
				} // if( nDragNodeIdx > 0 )
			} // if( m_pDragSrcNode != NULL )
		} // else from if( m_nHelperDropIndexBefore == nItemCount )
	} // else from if( nItemCount == 0 )
HWND hWndOwn = m_hWnd;
	_ItemFocusCancel( TRUE );
	if( ! ::IsWindow( hWndOwn ) )
		return true;
	ASSERT( 0 <= nNodeInsertPos && nNodeInsertPos <= m_pNode->GetNodeCount() );
	m_pNode->InsertNode( pSite, pClonedNode, nNodeInsertPos );

MENUITEMDATA mi( this );
	mi.SetCmdReceiver( m_hWndCmdReceiver );
	VERIFY(
		mi.UpdateFromCmdTree(
			m_hWndCmdReceiver,
			pClonedNode,
			m_nHelperDropIndexBefore
			)
		);
	if( mi.IsPopup() )
	{
		mi.GetPopup()->m_hWndCmdReceiver = m_hWndCmdReceiver;
		mi.GetPopup()->UpdateFromCmdTree(
			m_hWndCmdReceiver,
			pClonedNode,
			false
			);
		mi.GetPopup()->m_pWndParentMenu = this;
	} // if( mi.IsPopup() )
	ASSERT( !mi.IsSeparator() );
	ASSERT( ((LPVOID)mi.GetCmdNode()) == ((LPVOID)pClonedNode) );
	_InsertItem( m_nHelperDropIndexBefore, mi );

	_SyncItems();
	ShowWindow( SW_HIDE	);
	_RecalcLayoutImpl();
	VERIFY( SetWindowRgn( NULL, FALSE ) );
	ASSERT( !IsWindowVisible() );
	_FreeWinObjects();
// adjust screen position
CRect rcWnd = _CalcTrackRect();
	if( m_rgnWnd.GetSafeHandle() != NULL )
	{
		ASSERT( m_bExcludeAreaSpec );
		ASSERT( m_bCombineWithEA );
		ASSERT( m_eCombineAlign != __CMBA_NONE );
		CRgn rgnTmp;
		VERIFY( rgnTmp.CreateRectRgn(0,0,0,0) );
		rgnTmp.CopyRgn( &m_rgnWnd );
		ASSERT( rgnTmp.GetSafeHandle() != NULL );
		VERIFY(
			SetWindowRgn(
				(HRGN)rgnTmp.Detach(),
				FALSE
				)
			);
	} // if( m_rgnWnd.GetSafeHandle() != NULL )
	SetWindowPos(
		NULL,
		rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(),
		SWP_NOACTIVATE
			|SWP_NOZORDER //|SWP_NOOWNERZORDER
		);
	if( m_bScrollingAvailable )
		_RecalcLayoutImpl();
	ShowWindow( SW_SHOWNA );

	_SetDropIndexBefore();
	m_ptHelperDragStart.x = m_ptHelperDragStart.y = 0;
	return bRetVal;
}

// CExtCustomizeSite::ICustomizeDropSource
void CExtPopupMenuWnd::OnCustomizeSourceDragComplete(
	DROPEFFECT de,
	bool bCanceled,
	bool * p_bNoResetActiveItem
	)
{
	ASSERT_VALID( this );
	ASSERT(
			de == DROPEFFECT_NONE
		||	de == DROPEFFECT_MOVE
		||	de == DROPEFFECT_COPY
		);
	if( _IsPalette() )
		return;
	ASSERT( p_bNoResetActiveItem != NULL );
//	if(		m_ptHelperDragStart.y != 0
//		&&	m_bHelperNoHideChild
//		&&	de != DROPEFFECT_NONE
//		)
//	{
//		m_ptHelperDragStart.x = m_ptHelperDragStart.y = 0;
//		return;
//	}
	m_ptHelperDragStart.x = m_ptHelperDragStart.y = 0;
	if( de == DROPEFFECT_NONE )
		_ItemFocusCancel( TRUE );
	if( GetSafeHwnd() != NULL && (!m_bHelperNoHideChild) )
	{
		HWND hWndOwn = m_hWnd;
		_ItemFocusCancel( TRUE );
		if( ! ::IsWindow( hWndOwn ) )
			return;
	}
	m_bHelperNoHideChild = false;
CExtCustomizeSite * pSite = _FindCustomizeSite();
	ASSERT( pSite != NULL );
	if(		(!m_bHelperDragOverPassed)
		&&	m_pDragSrcNode != NULL
		)
		m_bHelperNoRemoveSrc = true;
	if( bCanceled || m_bHelperNoRemoveSrc || de == DROPEFFECT_COPY )
	{
		m_bHelperNoRemoveSrc = false;
		if( pSite->IsCustomizeMode() )
			*p_bNoResetActiveItem = true;
		return;
	} // if( bCanceled || de == DROPEFFECT_COPY || m_bHelperAddRemSeparator )

CExtCustomizeCmdTreeNode * pNode = pSite->DraggedNodeGet();
	ASSERT_VALID( pNode );
	ASSERT_VALID( m_pNode );
	ASSERT( ((LPVOID)pNode->GetParentNode()) == ((LPVOID)m_pNode) );
int nItemCount = ItemGetCount();
bool bPrevSeparator = false;
int nItem = 0;
	for( nItem = 0; nItem < nItemCount; nItem++ )
	{
		MENUITEMDATA & mi = ItemGetInfo( nItem );
		if( mi.IsSeparator() )
		{
			ASSERT( !bPrevSeparator );
			bPrevSeparator = true;
			continue;
		}
		CExtCustomizeCmdTreeNode * pCurrNode = mi.GetCmdNode();
		ASSERT_VALID( pCurrNode );
		if( ((LPVOID)pCurrNode) == ((LPVOID)pNode) )
			break;
		bPrevSeparator = false;
	} // for( nItem = 0; nItem < nItemCount; nItem++ )
	ASSERT( nItem < nItemCount ); // should be found
	if( bPrevSeparator )
	{
//		ASSERT( (pNode->GetFlags() & __ECTN_GROUP_START) != 0 );

		MENUITEMDATA & miRemove2 = ItemGetInfo( nItem );
		if( miRemove2.IsPopup() )
			miRemove2.DestroyPopup();

		pNode->ModifyFlags( __ECTN_GROUP_START, 0 );
		ASSERT( nItem > 0 );
		MENUITEMDATA & miRemove = ItemGetInfo( nItem-1 );
		ASSERT( miRemove.IsSeparator() );
		pNode->ModifyFlags( 0, __ECTN_GROUP_START );
		m_items_all.RemoveAt( nItem-1, 2 );
		m_nCurIndex = -1;
		nItemCount -= 2;
	} // if( bPrevSeparator )
	else
	{
//		ASSERT( (pNode->GetFlags() & __ECTN_GROUP_START) == 0 );
		pNode->ModifyFlags( 0, __ECTN_GROUP_START );
		MENUITEMDATA & miRemove = ItemGetInfo( nItem );
		if( miRemove.IsPopup() )
			miRemove.DestroyPopup();
		m_items_all.RemoveAt( nItem, 1 );
		m_nCurIndex = -1;
		nItemCount--;
	} // else from if( bPrevSeparator )
CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd(m_hWnd),
			pNode->GetCmdID( false )
			);
	ASSERT( pCmdItem != NULL );
	pNode->RemoveSelf( pCmdItem );
	pSite->DraggedNodeSet( NULL );
	if(		nItemCount > 0
		&&	ItemGetInfo(0).IsSeparator()
		)
	{
		m_items_all.RemoveAt( 0, 1 );
		m_nCurIndex = -1;
		m_pNode->ElementAt(0)->ModifyFlags( 0, __ECTN_GROUP_START );
		nItemCount--;
	} // if( nItemCount > 0 ...
	if(		nItemCount > 0
		&&	ItemGetInfo(nItemCount-1).IsSeparator()
		)
	{
		m_items_all.RemoveAt( nItemCount-1, 1 );
		m_nCurIndex = -1;
		nItemCount--;
	} // if( nItemCount > 0 ...

	_SyncItems();
	_SetDropIndexBefore();

	if( m_hWnd == NULL || (! ::IsWindow(m_hWnd) ) )
		return;

	ShowWindow( SW_HIDE	);
	_RecalcLayoutImpl();
	VERIFY( SetWindowRgn( NULL, FALSE ) );
	ASSERT( !IsWindowVisible() );
	_FreeWinObjects();
// adjust screen position
CRect rcWnd = _CalcTrackRect();
	if( m_rgnWnd.GetSafeHandle() != NULL )
	{
		ASSERT( m_bExcludeAreaSpec );
		ASSERT( m_bCombineWithEA );
		ASSERT( m_eCombineAlign != __CMBA_NONE );
		CRgn rgnTmp;
		VERIFY( rgnTmp.CreateRectRgn(0,0,0,0) );
		rgnTmp.CopyRgn( &m_rgnWnd );
		ASSERT( rgnTmp.GetSafeHandle() != NULL );
		VERIFY(
			SetWindowRgn(
				(HRGN)rgnTmp.Detach(),
				FALSE
				)
			);
	} // if( m_rgnWnd.GetSafeHandle() != NULL )
	SetWindowPos(
		NULL,
		rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(),
		SWP_NOACTIVATE
			|SWP_NOZORDER //|SWP_NOOWNERZORDER
		);
	if( m_bScrollingAvailable )
		_RecalcLayoutImpl();
	ShowWindow( SW_SHOWNA );

}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

/////////////////////////////////////////////////////////////////////////////
// CExtPopupColorMenuWnd

IMPLEMENT_DYNCREATE(CExtPopupColorMenuWnd, CExtPopupMenuWnd)

BEGIN_MESSAGE_MAP(CExtPopupColorMenuWnd, CExtPopupMenuWnd)
	//{{AFX_MSG_MAP(CExtPopupColorMenuWnd)
	//}}AFX_MSG_MAP
    ON_WM_QUERYNEWPALETTE()
    ON_WM_PALETTECHANGED()
END_MESSAGE_MAP()

const UINT CExtPopupColorMenuWnd::g_nMsgNotifyColorChanged =
	::RegisterWindowMessage(
		_T("CExtPopupColorMenuWnd::g_nMsgNotifyColorChanged")
		);
const UINT CExtPopupColorMenuWnd::g_nMsgNotifyColorChangedFinally =
	::RegisterWindowMessage(
		_T("CExtPopupColorMenuWnd::g_nMsgNotifyColorChangedFinally")
		);

const UINT CExtPopupColorMenuWnd::g_nMsgNotifyCustColor =
	::RegisterWindowMessage(
		_T("CExtPopupColorMenuWnd::g_nMsgNotifyCustColor")
		);

#define __N_CLR_DX             8
#define __N_CLR_DY             5
#define __N_CLR_COUNT          (__N_CLR_DX*__N_CLR_DY)

CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY::COLORREF_TABLE_ENTRY()
	: m_clrValue( RGB(0,0,0) )
	, m_nStrTblResID( 0 )
{
}

CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY::COLORREF_TABLE_ENTRY(
	const CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY & other
	)
	: m_clrValue( RGB(0,0,0) )
	, m_nStrTblResID( 0 )
{
	_AssignFromOther( other );
}

CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY::COLORREF_TABLE_ENTRY(
	UINT     nStrTblResID,
	COLORREF clrValue,
	__EXT_MFC_SAFE_LPCTSTR strColorNameEnglish
	)
	: m_clrValue( clrValue )
	, m_strColorNameEnglish( strColorNameEnglish )
	, m_nStrTblResID( nStrTblResID )
{
	nStrTblResID;
}

CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY::~COLORREF_TABLE_ENTRY()
{
}

CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY &
	CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY:: operator = (
		const CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY & other
		)
{
	_AssignFromOther( other );
	return (*this);
}

void CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY::_AssignFromOther(
	const CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY & other
	)
{
	m_nStrTblResID = other.m_nStrTblResID;
	m_clrValue = other.m_clrValue;
	m_strColorNameEnglish = other.m_strColorNameEnglish;
}

CExtSafeString CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY::GetLocalColorName() const
{
	__PROF_UIS_MANAGE_STATE;

CExtSafeString _sColorNameLocal;
	if( m_nStrTblResID != 0 )
	{
		g_ResourceManager->LoadString( _sColorNameLocal, m_nStrTblResID );
		if( ! _sColorNameLocal.IsEmpty() )
			return _sColorNameLocal;
	}
	return CExtSafeString( m_strColorNameEnglish );
}

// table captured from color picker
// control source by Chris Maunder
const CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY
	CExtPopupColorMenuWnd::g_arrDefaultColors[ 40 ] =
{
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_BLACK,            RGB(0x00, 0x00, 0x00),    _T("Black")             ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_BROWN,            RGB(0xA5, 0x2A, 0x00),    _T("Brown")             ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_DARK_OLIVE_GREEN, RGB(0x00, 0x40, 0x40),    _T("Dark Olive Green")  ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_DARK_GREEN,       RGB(0x00, 0x55, 0x00),    _T("Dark Green")        ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_DARK_TEAL,        RGB(0x00, 0x00, 0x5E),    _T("Dark Teal")         ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_DARK_BLUE,        RGB(0x00, 0x00, 0x8B),    _T("Dark blue")         ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_INDIGO,           RGB(0x4B, 0x00, 0x82),    _T("Indigo")            ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_DARK_GREY,        RGB(0x28, 0x28, 0x28),    _T("Dark grey")         ),

    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_DARK_RED,         RGB(0x8B, 0x00, 0x00),    _T("Dark red")          ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_ORANGE,           RGB(0xFF, 0x68, 0x20),    _T("Orange")            ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_DARK_YELLOW,      RGB(0x8B, 0x8B, 0x00),    _T("Dark yellow")       ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_GREEN,            RGB(0x00, 0x93, 0x00),    _T("Green")             ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_TEAL,             RGB(0x38, 0x8E, 0x8E),    _T("Teal")              ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_BLUE,             RGB(0x00, 0x00, 0xFF),    _T("Blue")              ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_BLUE_GREY,        RGB(0x7B, 0x7B, 0xC0),    _T("Blue-grey")         ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_GREY_50,          RGB(0x66, 0x66, 0x66),    _T("Grey - 50")         ),

    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_RED,              RGB(0xFF, 0x00, 0x00),    _T("Red")               ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_LIGHT_ORANGE,     RGB(0xFF, 0xAD, 0x5B),    _T("Light orange")      ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_LIME,             RGB(0x32, 0xCD, 0x32),    _T("Lime")              ), 
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_SEA_GREEN,        RGB(0x3C, 0xB3, 0x71),    _T("Sea green")         ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_AQUA,             RGB(0x7F, 0xFF, 0xD4),    _T("Aqua")              ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_LIGHT_BLUE,       RGB(0x7D, 0x9E, 0xC0),    _T("Light blue")        ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_VIOLET,           RGB(0x80, 0x00, 0x80),    _T("Violet")            ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_GREY_40,          RGB(0x7F, 0x7F, 0x7F),    _T("Grey - 40")         ),

    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_PINK,             RGB(0xFF, 0xC0, 0xCB),    _T("Pink")              ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_GOLD,             RGB(0xFF, 0xD7, 0x00),    _T("Gold")              ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_YELLOW,           RGB(0xFF, 0xFF, 0x00),    _T("Yellow")            ),    
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_BRIGHT_GREEN,     RGB(0x00, 0xFF, 0x00),    _T("Bright green")      ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_TURQUOISE,        RGB(0x40, 0xE0, 0xD0),    _T("Turquoise")         ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_SKYBLUE,          RGB(0xC0, 0xFF, 0xFF),    _T("Skyblue")           ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_PLUM,             RGB(0x48, 0x00, 0x48),    _T("Plum")              ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_LIGHT_GREY,       RGB(0xC0, 0xC0, 0xC0),    _T("Light grey")        ),

    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_ROSE,             RGB(0xFF, 0xE4, 0xE1),    _T("Rose")              ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_TAN,              RGB(0xD2, 0xB4, 0x8C),    _T("Tan")               ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_LIGHT_YELLOW,     RGB(0xFF, 0xFF, 0xE0),    _T("Light yellow")      ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_PALE_GREEN,       RGB(0x98, 0xFB, 0x98),    _T("Pale green ")       ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_PALE_TURQUOISE,   RGB(0xAF, 0xEE, 0xEE),    _T("Pale turquoise")    ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_PALE_BLUE,        RGB(0x68, 0x83, 0x8B),    _T("Pale blue")         ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_LAVENDER,         RGB(0xE6, 0xE6, 0xFA),    _T("Lavender")          ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_WHITE,            RGB(0xFF, 0xFF, 0xFF),    _T("White")             ),
};

const CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY
	CExtPopupColorMenuWnd::g_arrDefaultColorsLess[ 16 ] =
{
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_BLACK,            RGB(0x00, 0x00, 0x00),    _T("Black")             ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_GREY_50,          RGB(0x66, 0x66, 0x66),    _T("Grey - 50")         ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_DARK_RED,         RGB(0x8B, 0x00, 0x00),    _T("Dark red")          ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_DARK_YELLOW,      RGB(0x8B, 0x8B, 0x00),    _T("Dark yellow")       ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_GREEN,            RGB(0x00, 0x93, 0x00),    _T("Green")             ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_INDIGO,           RGB(0x4B, 0x00, 0x82),    _T("Indigo")            ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_DARK_BLUE,        RGB(0x00, 0x00, 0x8B),    _T("Dark blue")         ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_DARK_TEAL,        RGB(0x00, 0x00, 0x5E),    _T("Dark Teal")         ),
		
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_WHITE,            RGB(0xFF, 0xFF, 0xFF),    _T("White")             ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_LIGHT_GREY,       RGB(0xC0, 0xC0, 0xC0),    _T("Light grey")        ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_RED,              RGB(0xFF, 0x00, 0x00),    _T("Red")               ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_YELLOW,           RGB(0xFF, 0xFF, 0x00),    _T("Yellow")            ),    
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_BRIGHT_GREEN,     RGB(0x00, 0xFF, 0x00),    _T("Bright green")      ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_SKYBLUE,          RGB(0xC0, 0xFF, 0xFF),    _T("Skyblue")           ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_BLUE,             RGB(0x00, 0x00, 0xFF),    _T("Blue")              ),
    CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY( IDS_EXT_COLOR_NAME_ROSE,             RGB(0xFF, 0xE4, 0xE1),    _T("Rose")              ),
};

CExtPopupColorMenuWnd::CExtPopupColorMenuWnd()
	: m_pClrValueFinal( NULL )
	, m_bHelperInNotifyColorChanged( false )
	, m_sizeColorItemCell( 16, 16 )
	, m_sizeColorItemSpaces( 2, 2 )
	, m_nColorIdxCurr( -1 )
	, m_nInitialColorIdx( -1 )
	, m_hWndNotifyColorChanged( NULL ) // - use command targed
	, m_clrInitial( COLORREF(-1) ) // unexisting
	, m_clrDefault( RGB( 0, 0, 0 ) )
	, m_bEnableBtnColorDefault( true )
	, m_bEnableBtnColorCustom( true )
	, m_rcDefColorText( 0, 0, 0, 0 )
	, m_rcCustColorText( 0, 0, 0, 0 )
	, m_lParamCookie( 0 )
	, m_nColorsInRow( 8 )
{
	ASSERT(
		__N_CLR_COUNT ==
			(	  sizeof( g_arrDefaultColors )
				/ sizeof( g_arrDefaultColors[ 0 ] )
			)
		);

	if( ! g_ResourceManager->LoadString( m_sBtnTextColorDefault, IDS_COLOR_DEFAULT ) )
		m_sBtnTextColorDefault = _T("Default Color");

	if( ! g_ResourceManager->LoadString( m_sBtnTextColorCustom, IDS_COLOR_CUSTOM ) )
		m_sBtnTextColorCustom = _T("Custom Color ...");

	SetColors8x5();
}

CExtPopupColorMenuWnd::~CExtPopupColorMenuWnd()
{
	_RemoveAllColorEntries();
}

void CExtPopupColorMenuWnd::_RemoveAllColorEntries()
{
int nIdx, nColorCellCount = (int)m_arrColors.GetSize();
	for( nIdx = 0; nIdx < nColorCellCount; nIdx++ )
	{
		COLORREF_TABLE_ENTRY * pCTE = m_arrColors[ nIdx ];
		ASSERT( pCTE != NULL );
		delete pCTE;
	}
	m_arrColors.RemoveAll();
}

bool CExtPopupColorMenuWnd::IsAllItemsRarelyUsed() const
{
	ASSERT_VALID( this );
	return false;
}

CExtPopupMenuTipWnd::e_tip_style_t CExtPopupColorMenuWnd::_CoolTipGetStyle() const
{
	ASSERT_VALID( this );
	return g_eTtsColorMenu;
}

BOOL CExtPopupColorMenuWnd::OnQueryNewPalette() 
{
	ASSERT_VALID( this );

	Invalidate();
	return CWnd::OnQueryNewPalette();
}

void CExtPopupColorMenuWnd::OnPaletteChanged(CWnd* pFocusWnd) 
{
	ASSERT_VALID( this );

	CWnd::OnPaletteChanged(pFocusWnd);
	if( pFocusWnd->GetSafeHwnd() != GetSafeHwnd() )
	{
		Invalidate();
	}
}

CSize CExtPopupColorMenuWnd::_GetColorItemCellSize() const
{
	ASSERT_VALID( this );
	ASSERT( m_sizeColorItemCell.cx > 0 );
	ASSERT( m_sizeColorItemCell.cy > 0 );
	return m_sizeColorItemCell;
}

CSize CExtPopupColorMenuWnd::_GetColorItemSpaces() const
{
	ASSERT_VALID( this );
	ASSERT( m_sizeColorItemSpaces.cx >= 0 );
	ASSERT( m_sizeColorItemSpaces.cy >= 0 );
	return m_sizeColorItemSpaces;
}

int CExtPopupColorMenuWnd::_GetColorCellCount() const
{
	ASSERT_VALID( this );
	return int(m_arrColors.GetSize());
}

int CExtPopupColorMenuWnd::_GetColorsInRow() const
{
	ASSERT_VALID( this );
	ASSERT( m_nColorsInRow > 0 );
	return m_nColorsInRow;
}

void CExtPopupColorMenuWnd::_SetColorsInRow( int nColorsInRow )
{
	ASSERT_VALID( this );
	ASSERT( nColorsInRow > 0 );
	m_nColorsInRow = nColorsInRow;
}

CSize CExtPopupColorMenuWnd::_GetColorDimension() const
{
	ASSERT_VALID( this );
int nColorCellCount = _GetColorCellCount();
int nColorsInRow = _GetColorsInRow();
int nRest = nColorCellCount % nColorsInRow;
CSize _sizeDimension(
		nColorsInRow,
		nColorCellCount / nColorsInRow +
			( ( nRest > 0 ) ? 1 : 0 )
		);
	if( _sizeDimension.cy == 1 )
		_sizeDimension.cx = nColorCellCount;
	return _sizeDimension;
}

const CExtPopupColorMenuWnd::COLORREF_TABLE_ENTRY *
	CExtPopupColorMenuWnd::_GetColorEntry( int nIndex ) const
{
	ASSERT_VALID( this );
	if( nIndex < 0 || nIndex >= _GetColorCellCount() )
	{
		ASSERT( FALSE );
		return NULL;
	}
const COLORREF_TABLE_ENTRY * pCTE = m_arrColors[ nIndex ];
	ASSERT( pCTE != NULL );
	return pCTE;
}

COLORREF CExtPopupColorMenuWnd::_GetColorValue( int nIndex ) const
{
	ASSERT_VALID( this );
const COLORREF_TABLE_ENTRY * pCTE = _GetColorEntry( nIndex );
	if( pCTE == NULL )
		return RGB( 0, 0, 0 );
	return pCTE->m_clrValue;
}

CExtSafeString CExtPopupColorMenuWnd::_GetLocalColorName( int nIndex ) const
{
	ASSERT_VALID( this );
const COLORREF_TABLE_ENTRY * pCTE = _GetColorEntry( nIndex );
	if( pCTE == NULL )
		return CExtSafeString( _T("") );
	return pCTE->GetLocalColorName();
}

int CExtPopupColorMenuWnd::_GetNextKeyColor(
	int nColorIndex,
	UINT nChar // VK_LEFT, VK_RIGHT, VK_UP and VK_DOWN
	)
{
	ASSERT_VALID( this );
int nColorCellCount = _GetColorCellCount();
	if( nColorCellCount == 0 )
		return -1;
	if( nColorIndex < 0 || nColorIndex >= nColorCellCount )
		return 0;
int nColorsInRow = _GetColorsInRow();
int nRest = nColorCellCount % nColorsInRow;
CSize _sizeDimension(
		nColorsInRow,
		nColorCellCount / nColorsInRow +
			( ( nRest > 0 ) ? 1 : 0 )
		);
	if( _sizeDimension.cy == 1 )
		_sizeDimension.cx = nColorCellCount;
int nColNo = nColorIndex % nColorsInRow;
	ASSERT( 0 <= nColNo && nColNo < _sizeDimension.cx );
int nRowNo = nColorIndex / nColorsInRow;
	ASSERT( 0 <= nRowNo && nRowNo < _sizeDimension.cy );
	switch( nChar )
	{
	case VK_LEFT:
	{
		if( _sizeDimension.cx == 1 )
			return nColorIndex;
		if( nColNo > 0 )
			nColNo --;
		else
		{
			int nCountInRow = _sizeDimension.cx;
			if( nRowNo == (_sizeDimension.cy-1) && nRest > 0 )
				nCountInRow = nRest;
			nColNo = nCountInRow - 1;
		}
	}
	break;
	case VK_RIGHT:
	{
		if( _sizeDimension.cx == 1 )
			return nColorIndex;
		int nCountInRow = _sizeDimension.cx;
		if( nRowNo == (_sizeDimension.cy-1) && nRest > 0 )
			nCountInRow = nRest;
		if( nColNo >= (nCountInRow-1) )
			nColNo = 0 ;
		else
			nColNo ++;
	}
	break;
	case VK_UP:
	{
		if( _sizeDimension.cy == 1 )
			return nColorIndex;
		if( nRowNo > 0 )
			nRowNo --;
		else
		{
			nRowNo = _sizeDimension.cy - 1;
			if( nRest > 0 && nColNo >= nRest )
			{
				nRowNo --;
				ASSERT( nRowNo >= 0 );
			}
		}
	}
	break;
	case VK_DOWN:
	{
		if( _sizeDimension.cy == 1 )
			return nColorIndex;
		nRowNo ++;
		if( nRowNo >= _sizeDimension.cy )
			nRowNo = 0;
		else if( nRowNo == (_sizeDimension.cy-1) && ( nRest > 0 ) )
		{
			if( nColNo >= nRest )
				nRowNo = 0;
		}
	}
	break;
#ifdef _DEBUG
	default:
	{
		ASSERT( FALSE );
	}
	break;
#endif // _DEBUG
	} // switch( nChar )
	nColorIndex = nRowNo * _sizeDimension.cx + nColNo;
	ASSERT( 0 <= nColorIndex && nColorIndex < nColorCellCount );
	return nColorIndex;
}


void CExtPopupColorMenuWnd::AddColor( COLORREF_TABLE_ENTRY * pCTE )
{
	ASSERT_VALID( this );
	ASSERT( pCTE != NULL );
	m_arrColors.Add( pCTE );
}

void CExtPopupColorMenuWnd::RemoveAllColors()
{
	ASSERT_VALID( this );
	_RemoveAllColorEntries();
}

void CExtPopupColorMenuWnd::SetColorsInRow( int nColorsInRow )
{
	ASSERT_VALID( this );
	_SetColorsInRow( nColorsInRow );
}

void CExtPopupColorMenuWnd::SetColors8x5()
{
	ASSERT_VALID( this );
	SetColorsInRow( 8 );
	RemoveAllColors();
int nIdx, nColorCellCount = sizeof( g_arrDefaultColors ) / sizeof( g_arrDefaultColors[ 0 ] );
	for( nIdx = 0; nIdx < nColorCellCount; nIdx++ )
	{
		const COLORREF_TABLE_ENTRY * pCTE = g_arrDefaultColors + nIdx;
		AddColor( new COLORREF_TABLE_ENTRY( *pCTE ) );
	} // for( nIdx = 0; nIdx < nColorCellCount; nIdx++ )
}

void CExtPopupColorMenuWnd::SetColors8x2()
{
	ASSERT_VALID( this );
	SetColorsInRow( 8 );
	RemoveAllColors();
int nIdx, nColorCellCount = sizeof( g_arrDefaultColorsLess ) / sizeof( g_arrDefaultColorsLess[ 0 ] );
	for( nIdx = 0; nIdx < nColorCellCount; nIdx++ )
	{
		const COLORREF_TABLE_ENTRY * pCTE = g_arrDefaultColorsLess + nIdx;
		AddColor( new COLORREF_TABLE_ENTRY( *pCTE ) );
	} // for( nIdx = 0; nIdx < nColorCellCount; nIdx++ )
}

CSize CExtPopupColorMenuWnd::_CalcTrackSize()
{
	ASSERT_VALID( this );
CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );

CRect rcMB = OnQueryMenuBorderMetrics();
int nMenuShadowSize = OnQueryMenuShadowSize();
CSize _sizeColorItemCell = _GetColorItemCellSize();
CSize _sizeColorItemSpaces = _GetColorItemSpaces();
CSize _sizeDimension = _GetColorDimension();

CSize _size(
		(_sizeColorItemCell.cx + _sizeColorItemSpaces.cx * 2 )
				* _sizeDimension.cx
			+ rcMB.left + rcMB.right + nMenuShadowSize
			,
		(_sizeColorItemCell.cy + _sizeColorItemSpaces.cy * 2 )
				* _sizeDimension.cy
			+ rcMB.left + rcMB.right + nMenuShadowSize
		);
bool bTearOff = _IsTearOff();
int nTearOffCaptionHeight = bTearOff ? _GetTearOffCaptionHeight() : 0;
	if( m_bEnableBtnColorDefault )
	{
		_size.cy +=
			PmBridge_GetPM()->GetSeparatorHeight()
			+ __EXT_MENU_GAP*2;
		CExtSafeString sMeasureText( m_sBtnTextColorDefault );
		sMeasureText.Replace( _T("&"), _T("") );

		CWindowDC dc(NULL);
		CFont * pOldFont =
			dc.SelectObject( &(PmBridge_GetPM()->m_FontNormal) );
		ASSERT( pOldFont != NULL );
		CRect rect(0,0,0,0);
		CSize _sizeDefColorText;
		_sizeDefColorText.cy =
			CExtRichContentLayout::stat_DrawText(
				dc.GetSafeHdc(),
				LPCTSTR(sMeasureText), sMeasureText.GetLength(),
				&rect,
				DT_CALCRECT|DT_SINGLELINE|DT_LEFT|DT_TOP, 0
				);
		_sizeDefColorText.cx = rect.Width();
		dc.SelectObject( pOldFont );
		_sizeDefColorText.cx += pPM->UiScalingDo( __EXT_MENU_GAP * 2, CExtPaintManager::__EUIST_X );
		INT nMenuMinHeight = pPM->UiScalingDo( __EXT_MENU_MIN_HEIGHT, CExtPaintManager::__EUIST_Y );
		if( _sizeDefColorText.cy < nMenuMinHeight )
			_sizeDefColorText.cy = nMenuMinHeight;
		_size.cy += _sizeDefColorText.cy;
		if( _size.cx < _sizeDefColorText.cx )
			_size.cx = _sizeDefColorText.cx;
		CPoint pt(
			rcMB.left + __EXT_MENU_GAP,
			rcMB.top + __EXT_MENU_GAP
			);
		m_rcDefColorText.SetRect(
			pt,
			pt
			+
			CSize(
				max( _size.cx , _sizeDefColorText.cx )
					- __EXT_MENU_GAP*2
					- rcMB.left - rcMB.right
					- nMenuShadowSize
					,
				_sizeDefColorText.cy
				)
			);
		if( bTearOff )
			m_rcDefColorText.OffsetRect( 0, nTearOffCaptionHeight );
	} // if( m_bEnableBtnColorDefault )
	if( m_bEnableBtnColorCustom )
	{
		_size.cy +=
			PmBridge_GetPM()->GetSeparatorHeight()
			+ __EXT_MENU_GAP*2
			+ 1 // + 2.28
			;
		CExtSafeString sMeasureText( m_sBtnTextColorCustom );
		sMeasureText.Replace( _T("&"), _T("") );

		CWindowDC dc(NULL);
		CFont * pOldFont =
			dc.SelectObject( &(PmBridge_GetPM()->m_FontNormal) );
		ASSERT( pOldFont != NULL );
		CRect rect(0,0,0,0);
		CSize _sizeCustColorText;
		_sizeCustColorText.cy =
			CExtRichContentLayout::stat_DrawText(
				dc.GetSafeHdc(),
				LPCTSTR(sMeasureText), sMeasureText.GetLength(),
				&rect,
				DT_CALCRECT|DT_SINGLELINE|DT_LEFT|DT_TOP, 0
				);
		_sizeCustColorText.cx = rect.Width();
		dc.SelectObject( pOldFont );
		_sizeCustColorText.cx += __EXT_MENU_GAP * 2;
		INT nMenuMinHeight = pPM->UiScalingDo( __EXT_MENU_MIN_HEIGHT, CExtPaintManager::__EUIST_Y );
		if( _sizeCustColorText.cy < nMenuMinHeight )
			_sizeCustColorText.cy = nMenuMinHeight;
		_size.cy += _sizeCustColorText.cy;
		if( _size.cx < _sizeCustColorText.cx )
			_size.cx = _sizeCustColorText.cx;
		CPoint pt(
			rcMB.left + __EXT_MENU_GAP,
			rcMB.top + __EXT_MENU_GAP
			);
		m_rcCustColorText.SetRect(
			pt,
			pt
			+
			CSize(
				max( _size.cx , _sizeCustColorText.cx )
					- __EXT_MENU_GAP*2
					- rcMB.left - rcMB.right
					- nMenuShadowSize
					,
				_sizeCustColorText.cy
				)
			);
		if( m_bEnableBtnColorDefault )
			m_rcCustColorText.OffsetRect(
				0,
				m_rcCustColorText.Height()
				+ __EXT_MENU_GAP*2
				+ PmBridge_GetPM()->GetSeparatorHeight()
				);
		m_rcCustColorText.OffsetRect(
			0,
			rcMB.top +
				( _sizeColorItemCell.cy + _sizeColorItemSpaces.cy * 2 )
					* _sizeDimension.cy
				+ _sizeColorItemSpaces.cy
			);
		if( bTearOff )
			m_rcCustColorText.OffsetRect( 0, nTearOffCaptionHeight );
	} // if( m_bEnableBtnColorCustom )

	_size.cx += m_nLeftAreaWidth;
	return _size;
}

CPoint CExtPopupColorMenuWnd::_GetColorItemCoord( int nIdx )
{
	ASSERT_VALID( this );
CSize _sizeDimension = _GetColorDimension();
	ASSERT( nIdx >= 0 && nIdx < _GetColorCellCount() );
int nX = nIdx % _sizeDimension.cx;
	ASSERT( nX < _sizeDimension.cx );
int nY = nIdx / _sizeDimension.cx;
	ASSERT( nY < _sizeDimension.cy );
	return CPoint( nX, nY );
}

CRect CExtPopupColorMenuWnd::_GetColorItemRect(int nIdx)
{
	ASSERT_VALID( this );

	ASSERT( nIdx >= 0 && nIdx < _GetColorCellCount() );
CPoint ptCoord =  _GetColorItemCoord( nIdx );
CRect rcMB = OnQueryMenuBorderMetrics();
CSize _sizeColorItemCell = _GetColorItemCellSize();
CSize _sizeColorItemSpaces = _GetColorItemSpaces();
CRect rcItem(
		CPoint(
			rcMB.left +
				( _sizeColorItemCell.cx + _sizeColorItemSpaces.cx*2) 
					* ptCoord.x
				+ _sizeColorItemSpaces.cx
				,
			rcMB.top +
				( _sizeColorItemCell.cy + _sizeColorItemSpaces.cy * 2 )
					* ptCoord.y
				+ _sizeColorItemSpaces.cy
			),
		_sizeColorItemCell
		);
	if( m_bEnableBtnColorDefault )
	{
		rcItem.OffsetRect(
			0,
			m_rcDefColorText.Height()
			+ PmBridge_GetPM()->GetSeparatorHeight()
			+ __EXT_MENU_GAP*2
			);
	}
CRect rcClient;
	_GetClientRect( &rcClient );
	rcItem.OffsetRect( rcClient.TopLeft() );
	rcItem.OffsetRect( m_nLeftAreaWidth, 0 );

	if( _IsTearOff() )
	{
		int nTearOffCaptionHeight =
			_GetTearOffCaptionHeight();
		rcItem.OffsetRect( 0, nTearOffCaptionHeight );
	}

	return rcItem;
}

CRect CExtPopupColorMenuWnd::_CalcTrackRect()
{
	ASSERT_VALID( this );
	return CExtPopupMenuWnd::_CalcTrackRect();
}

bool CExtPopupColorMenuWnd::_CreateHelper(
	CWnd * pWndCmdReceiver
	)
{
	ASSERT_VALID( this );

	if( ! CExtPopupMenuWnd::_CreateHelper(
			pWndCmdReceiver
			)
		)
		return false;
int nIdx =
		_FindCellByColorRef( m_clrInitial );
	if( nIdx >= 0 )
	{
		m_nInitialColorIdx = nIdx;
		m_nColorIdxCurr = nIdx;
		Invalidate();
	}
CRect rcClient;
	_GetClientRect( &rcClient );
	if( m_bEnableBtnColorDefault )
	{
		m_rcDefColorText.OffsetRect( rcClient.TopLeft() );
		m_rcDefColorText.OffsetRect( m_nLeftAreaWidth, 0 );
	}
	if( m_bEnableBtnColorCustom )
	{
		m_rcCustColorText.OffsetRect( rcClient.TopLeft() );
		m_rcCustColorText.OffsetRect( m_nLeftAreaWidth, 0 );
	}
	return true;
}

void CExtPopupColorMenuWnd::_DoPaint( CDC & dcPaint, bool bUseBackBuffer /*= true*/ )
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dcPaint) );
	ASSERT( dcPaint.GetSafeHdc() != NULL );
CRect rcRealClient;
	GetClientRect( &rcRealClient );
	rcRealClient.OffsetRect( OnQueryMenuShadowSize(), 0 );
CRect rcClient;
	_GetClientRect( &rcClient );
    // Select and realize the palette
CPalette * pOldPalette = NULL;
    if( ::GetDeviceCaps(dcPaint.m_hDC,RASTERCAPS) & RC_PALETTE )
    {
        pOldPalette = dcPaint.SelectPalette( &PmBridge_GetPM()->m_PaletteWide, FALSE );
        dcPaint.RealizePalette();
    }
CExtMemoryDC mdc;
	if( bUseBackBuffer )
	{
		mdc.__InitMemoryDC(
			&dcPaint,
			&rcClient,
			CExtMemoryDC::MDCOPT_TO_MEMORY|CExtMemoryDC::MDCOPT_FILL_SURFACE|CExtMemoryDC::MDCOPT_FORCE_DIB 
			);
	} // if( bUseBackBuffer )
CDC & dcDummyRef = mdc;
CDC & dc = bUseBackBuffer ? dcDummyRef : dcPaint;
CFont * pOldFont = (CFont *) dc.SelectObject( &PmBridge_GetPM()->m_FontNormal );
COLORREF clrFill = PmBridge_GetPM()->GetMenuFrameFillColor( this );
	if( clrFill != COLORREF(-1L) )
		dc.FillSolidRect( &rcClient, clrFill );
	PmBridge_GetPM()->PaintMenuBorder( dc, rcClient, this );
	ASSERT( !m_bExpandAvailable );
	ASSERT( !m_bScrollingAvailable );
CRect rcMB = OnQueryMenuBorderMetrics();
int nMenuShadowSize = OnQueryMenuShadowSize();
	if( rcClient.top >= rcRealClient.top )
	{
		CRect rcExcludePart(rcRealClient);
		rcExcludePart.bottom =
			rcClient.top + rcMB.top;
		dc.ExcludeClipRect( rcExcludePart );
	}
	if( rcClient.bottom <= rcRealClient.bottom )
	{
		CRect rcExcludePart(rcRealClient);
		rcExcludePart.top =
			rcClient.bottom - rcMB.bottom;
		dc.ExcludeClipRect( rcExcludePart );
	}
	// paint default color button
	if( m_bEnableBtnColorDefault )
	{
		bool bSelected =
			(m_nColorIdxCurr == IDX_DEFAULT_COLOR_BTN) ?
			true : false;
		CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
			this,
			true,
			m_rcDefColorText,
			m_sBtnTextColorDefault,
			NULL,
			true,
			bSelected,
			false, // bSelected,
			false,
			true,
			bSelected,
			false,
			false,
			CExtPaintManager::__ALIGN_HORIZ_CENTER|CExtPaintManager::__ALIGN_VERT_CENTER,
			NULL,
			false,
			0,
			!bSelected
			);
		PmBridge_GetPM()->PaintPushButton( dc, _ppbd );
		CRect rcItem( m_rcDefColorText );
		rcItem.OffsetRect( 0, rcItem.Height() + __EXT_MENU_GAP );
		rcItem.bottom = rcItem.top + PmBridge_GetPM()->GetSeparatorHeight();
		rcItem.top += rcItem.Height()/2;
		rcItem.bottom = rcItem.top+1;
		PmBridge_GetPM()->PaintSeparator( dc, rcItem, false, true, this );
	}
CSize _sizeColorItemSpaces = _GetColorItemSpaces();
	if( m_bEnableBtnColorCustom )
	{
		bool bSelected = ( m_nColorIdxCurr == IDX_CUSTOM_COLOR_BTN ) ? true : false;
		CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
			this,
			true,
			m_rcCustColorText,
			m_sBtnTextColorCustom,
			NULL,
			true,
			bSelected,
			false, // bSelected,
			false,
			true,
			bSelected,
			false,
			false,
			CExtPaintManager::__ALIGN_HORIZ_CENTER|CExtPaintManager::__ALIGN_VERT_CENTER,
			NULL,
			false,
			0,
			!bSelected
			);
		PmBridge_GetPM()->PaintPushButton( dc, _ppbd );
		CRect rcItem( m_rcCustColorText );
		rcItem.OffsetRect( 0, - _sizeColorItemSpaces.cy );
		rcItem.bottom = rcItem.top;
		rcItem.top -= PmBridge_GetPM()->GetSeparatorHeight();
		rcItem.top += rcItem.Height()/2;
		rcItem.bottom = rcItem.top+1;
		PmBridge_GetPM()->PaintSeparator( dc, rcItem, false, true, this );
	}

	// paint color buttons
COLORREF clrColorBorder =
		PmBridge_GetPM()->GetColor( COLOR_3DDKSHADOW, this );
int nIdx, nColorCellCount = _GetColorCellCount();
	for( nIdx = 0; nIdx < nColorCellCount; nIdx++ )
	{
		bool bSelected = ( m_nColorIdxCurr == nIdx || m_nInitialColorIdx == nIdx ) ? true : false;
		CRect rcItem = _GetColorItemRect( nIdx );
		CExtPaintManager::PAINTPUSHBUTTONDATA _ppbd(
			this,
			true,
			rcItem,
			_T(""),
			NULL,
			true,
			bSelected,
			bSelected,
			false,
			true,
			bSelected,
			false,
			false,
			CExtPaintManager::__ALIGN_HORIZ_CENTER|CExtPaintManager::__ALIGN_VERT_CENTER,
			NULL,
			false,
			0,
			!bSelected
			);
		_ppbd.m_bHelperColorCellButton = true;
		PmBridge_GetPM()->PaintPushButton( dc, _ppbd );
		rcItem.DeflateRect( _sizeColorItemSpaces.cx, _sizeColorItemSpaces.cy );
		dc.FillSolidRect( &rcItem, _GetColorValue( nIdx ) );
		dc.Draw3dRect( &rcItem, clrColorBorder,clrColorBorder );
	} // for( nIdx = 0; nIdx < nColorCellCount; nIdx++ )
	if( m_nLeftAreaWidth > 0 )
	{
		CRect rcLeftArea( rcClient );
		rcLeftArea.right = rcLeftArea.left + m_nLeftAreaWidth;
		rcLeftArea.OffsetRect( rcMB.left, 0 );
		rcLeftArea.DeflateRect( 0, rcMB.top, 0, rcMB.bottom );
		DRAWLEFTAREADATA _DrawLeftAreaData( &dc, &rcLeftArea, this );
		_DrawLeftAreaData.DoOwnerDrawPainting();
	}

	if( _IsTearOff() )
	{
		CPoint point( 0, 0 );
		if( ::GetCursorPos( &point ) )
			ScreenToClient( &point );
		CRect rcTearOff;
		_GetItemRect( IDX_TEAROFF, rcTearOff, false );
		bool bHover =
			(rcTearOff.PtInRect(point))? true : false;
		if( bHover && _FindCustomizeMode() )
			bHover = false;
		bool bPushed = bHover;
		PmBridge_GetPM()->PaintMenuTearOffButton( dc, rcTearOff, bHover, bPushed, this );
	} // if( m_bExpandAvailable )

CPoint point( 0, 0 );
	::GetCursorPos( &point );
	ScreenToClient( &point );
	dc.SelectClipRgn( NULL );
	dcPaint.SelectClipRgn( NULL );

CRgn _rgnCli;
	if( _rgnCli.CreateRectRgnIndirect( &rcClient ) )
		dcPaint.SelectClipRgn( &_rgnCli );

	if( m_bCombineWithEA )
	{
		ASSERT( m_eCombineAlign != __CMBA_NONE );
		dc.SelectClipRgn(NULL);
		CRect rcExcludeClient( m_rcExcludeArea );
		ScreenToClient( &rcExcludeClient );
		CRgn _rgn;
		if( _rgn.CreateRectRgnIndirect( &rcExcludeClient ) )
			dcPaint.SelectClipRgn( &_rgn, RGN_OR );
		PmBridge_GetPM()->PaintMenuCombinedArea( dc, rcExcludeClient, rcClient, m_eCombineAlign, this );
		if( m_pCbPaintCombinedContent != NULL )
		{
			CExtMemoryDC dcCA( &dc, &rcExcludeClient, CExtMemoryDC::MDCOPT_TO_MEMORY|CExtMemoryDC::MDCOPT_FILL_BITS|CExtMemoryDC::MDCOPT_RTL_COMPATIBILITY );
			m_pCbPaintCombinedContent( m_pCbPaintCombinedCookie, dcCA, *this, m_rcExcludeArea, m_eCombineAlign );
		}
	} // if( m_bCombineWithEA )
	dc.SelectObject( pOldFont );

	if( bUseBackBuffer )
	{
		CRgn _rgn;
		if( _rgn.CreateRectRgnIndirect( &rcClient ) )
			dcPaint.SelectClipRgn( &_rgn, RGN_OR );
		ASSERT( mdc.GetSafeHdc() != NULL );
		mdc.__Flush();
		dcPaint.SelectClipRgn( NULL );
	}

	if(		_IsPopupWithShadows()
		&&	m_bAnimFinished
		&&	nMenuShadowSize > 0
		)
	{ // if we need to paint shadow for client area (and combined exclude area)
		dcPaint.SelectClipRgn(NULL);
		CRect
			rcExcludeClient( 0,0,0,0 ),
			rcExcludeScreen( 0,0,0,0 ),
			rcBaseScreen( 0,0,0,0 );
		if( m_bCombineWithEA )
		{
			if(		_IsPopupWithShadows()
				&&	m_bAnimFinished
				&&	nMenuShadowSize > 0
				)
			{ // if we need to paint shadow for combined exclude area
				rcExcludeScreen = m_rcExcludeArea;
				rcExcludeClient = rcExcludeScreen;
				ScreenToClient( &rcExcludeClient );
				dcPaint.ExcludeClipRect( &rcClient );
				rcBaseScreen = rcClient;
				ClientToScreen( &rcBaseScreen );
				VERIFY(
					m_ShadowCMBA.Paint(
						PmBridge_GetPM(),
						dcPaint,
						rcExcludeClient,
						rcExcludeScreen,
						rcBaseScreen,
						nMenuShadowSize
						)
					);
				dcPaint.SelectClipRgn(NULL);
			} // if we need to paint shadow for combined exclude area

			CRect rcExcludePart( m_rcExcludeArea );
			ScreenToClient( &rcExcludePart );
			dcPaint.ExcludeClipRect( &rcExcludePart );
		} // if( m_bCombineWithEA )
		VERIFY(
			m_ShadowMain.Paint(
				PmBridge_GetPM(),
				dcPaint,
				rcClient,
				rcBaseScreen,
				rcExcludeScreen,
				nMenuShadowSize
				)
			);
	} // if we need to paint shadow for client area (and combined exclude area)

	if( pOldPalette != NULL )
		dcPaint.SelectPalette( pOldPalette, FALSE );
}

int CExtPopupColorMenuWnd::_ColorItemHitTest(
	const CPoint & point
	)
{
	ASSERT_VALID( this );
int nIdx, nColorCellCount = _GetColorCellCount();
	for( nIdx = 0; nIdx < nColorCellCount; nIdx++ )
	{
		CRect rcItem = _GetColorItemRect( nIdx );
		if( rcItem.PtInRect(point) )
			return nIdx;
	} // for( nIdx = 0; nIdx < nColorCellCount; nIdx++ )
	if( m_rcDefColorText.PtInRect(point) )
		return IDX_DEFAULT_COLOR_BTN;
	if( m_rcCustColorText.PtInRect(point) )
		return IDX_CUSTOM_COLOR_BTN;
	return -1;
}

bool CExtPopupColorMenuWnd::_CanStartLevelTracking()
{
	ASSERT_VALID( this );
	if( _FindHelpMode() )
		return false;
	if( _FindCustomizeMode() )
		return false;
	return true;
}

bool CExtPopupColorMenuWnd::_OnMouseWheel(
	WPARAM wParam,
	LPARAM lParam,
	bool & bNoEat
	)
{
	ASSERT_VALID( this );

TranslateMouseWheelEventData_t _td( this, wParam, lParam, bNoEat );
	if( _td.Notify() )
	{
		bNoEat = _td.m_bNoEat;
		return true;
	}

	return true;
}

bool CExtPopupColorMenuWnd::_OnMouseMove(
	UINT nFlags,
	CPoint point,
	bool & bNoEat
	)
{
	ASSERT_VALID( this );

	if( GetSafeHwnd() == NULL )
		return false;

	if( GetSite().GetAnimated() != NULL )
		return true;

TranslateMouseMoveEventData_t _td( this, nFlags, point, bNoEat );
	if( _td.Notify() )
	{
		bNoEat = _td.m_bNoEat;
		return true;
	}

CPoint ptScreenClick( point );
	ClientToScreen( &ptScreenClick );
HWND hWndFromPoint = ::WindowFromPoint( ptScreenClick );
	if(		hWndFromPoint != NULL
		&&	(::__EXT_MFC_GetWindowLong(hWndFromPoint,GWL_STYLE)&WS_CHILD) != NULL
		&&	::GetParent(hWndFromPoint) == m_hWnd
		)
	{
		return false; // inplace edit
	}

	if( ! _PtInWndArea( point ) )
	{
		if( m_pWndParentMenu != NULL
			&& m_pWndParentMenu->GetSafeHwnd() != NULL
			)
		{
			ASSERT_VALID( m_pWndParentMenu );
			ClientToScreen( &point );
			m_pWndParentMenu->ScreenToClient( &point );
			if( m_pWndParentMenu->_OnMouseMove(
					nFlags,
					point,
					bNoEat
					)
				)
			{
				if( bNoEat )
					return false;
				_OnCancelMode();
				return true;
			}
		}
		return false;
	}

	if( _IsTearOff() )
	{
		if( CExtPopupMenuWnd::_HitTest(point) == IDX_TEAROFF )
		{
			HWND hWndOwn = m_hWnd;
			_ItemFocusCancel( TRUE, FALSE );
			if( ! ::IsWindow( hWndOwn ) )
				return true;
			_SetCapture();
			HCURSOR hCursor = ::LoadCursor( NULL, IDC_SIZEALL );
			ASSERT( hCursor != NULL );
			::SetCursor( hCursor );
			if(		g_bMenuShowCoolTips
				&&	GetSite().GetCapture() == this
				)
			{
				CRect rcItem;
				_GetItemRect(IDX_TEAROFF,rcItem);
				ClientToScreen( &rcItem );
				bool bShowTip = (!m_bSuspendTips);
				CExtPopupMenuTipWnd & _tipWnd = GetTip();
				if( _tipWnd.GetSafeHwnd() != NULL )
				{
					CRect rcExcludeArea = _tipWnd.GetExcludeArea();
					if( rcExcludeArea == rcItem )
						bShowTip = false;
				}
				if( bShowTip )
				{
					CExtSafeString sTipText;
					g_ResourceManager->LoadString( sTipText, ID_EXT_TEAR_OFF_MENU_TIP );
					if( sTipText.IsEmpty() )
						sTipText = _T("Drag to make this menu float");

					_tipWnd.SetText( sTipText );
					_tipWnd.SetTipStyle( _CoolTipGetStyle() );
					VERIFY(
						_tipWnd.Show(
							this,
							rcItem
							)
						);
				}
			}
			return true;
		}
		HCURSOR hCursor = ::LoadCursor( NULL, IDC_ARROW );
		ASSERT( hCursor != NULL );
		::SetCursor( hCursor );
	}

bool bHoverChanged = false;
int nColorIdxCurr = _ColorItemHitTest(point);
	if( m_nColorIdxCurr != nColorIdxCurr )
	{
		m_nColorIdxCurr = nColorIdxCurr;
		bHoverChanged = true;
		if( m_nColorIdxCurr != IDX_CUSTOM_COLOR_BTN )
			_NotifyColorChanged();
		Invalidate();
	}

	if( m_nColorIdxCurr >= 0 )
	{
		if(		g_bMenuShowCoolTips
			&&	(! m_bSuspendTips )
			&&	bHoverChanged
			)
		{
			CExtSafeString sTipText =
				_GetLocalColorName( m_nColorIdxCurr );
			if( !sTipText.IsEmpty()
				&& GetSite().GetCapture() == this
				&& ( GetSite().GetAnimated() == NULL )
				)
			{
				CRect rcItem =
					_GetColorItemRect(m_nColorIdxCurr);
				ClientToScreen( &rcItem );
				CExtPopupMenuTipWnd & _tipWnd = GetTip();
				_tipWnd.SetText( sTipText );
				_tipWnd.SetTipStyle( _CoolTipGetStyle() );
				VERIFY(
					_tipWnd.Show(
						this,
						rcItem
						)
					);
			}
		} // if( nOldCurIndex != nCurIndex )
	}
	else
	{
		HWND hWndOwn = m_hWnd;
		_CoolTipHide();
		if( ! ::IsWindow( hWndOwn ) )
			return true;
	}

	return true;
}

bool CExtPopupColorMenuWnd::_OnMouseClick(
	UINT nFlags,
	CPoint point,
	bool & bNoEat
	)
{
	ASSERT_VALID( this );
	bNoEat;

	if( GetSafeHwnd() == NULL )
		return false;

	if( GetSite().GetAnimated() != NULL )
		return true;

TranslateMouseClickEventData_t _td( this, nFlags, point, bNoEat );
	if( _td.Notify() )
	{
		bNoEat = _td.m_bNoEat;
		return true;
	}

bool bLButtonUpCall =
		(nFlags==WM_LBUTTONUP || nFlags==WM_NCLBUTTONUP)
			? true : false;

	if(		m_eCombineAlign != __CMBA_NONE
		&&	m_bTopLevel
		)
	{
		CRect rcExcludeAreaTest( m_rcExcludeArea );
		ScreenToClient( &rcExcludeAreaTest );
		if( rcExcludeAreaTest.PtInRect( point ) )
		{
			if( !bLButtonUpCall )
			{
				_OnCancelMode();
				return true;
			}
			return false;
		} // if( rcExcludeAreaTest.PtInRect( point ) )
	} // if( m_eCombineAlign != __CMBA_NONE ...

	if( _IsTearOff() && CExtPopupMenuWnd::_HitTest(point) == IDX_TEAROFF )
	{
		if( ! bLButtonUpCall )
			_DoTearOff();
		return true;
	}

	if( ! bLButtonUpCall )
		return false;

bool bEndSequence = false;
HWND hWndOwn = GetSafeHwnd();
	ASSERT( hWndOwn != NULL && ::IsWindow(hWndOwn) );
int m_nColorIdxCurr = _ColorItemHitTest(point);
	if(	(		m_nColorIdxCurr >= 0
			||	m_nColorIdxCurr == IDX_DEFAULT_COLOR_BTN
			||	m_nColorIdxCurr == IDX_CUSTOM_COLOR_BTN
			)
		&&	( ! _FindCustomizeMode() )
		)
	{
		bEndSequence = true;
		_NotifyColorChanged( true );
		g_SoundPlayer->PlaySound( CExtSoundPlayer::__ON_MENU_CMD_CLICKED );
	}
	if( ! ::IsWindow(hWndOwn) )
		return true;

CPoint ptScreenClick( point );
	ClientToScreen( &ptScreenClick );
HWND hWndFromPoint = ::WindowFromPoint( ptScreenClick );
	if(		hWndFromPoint != NULL
		&&	hWndFromPoint == m_pWndParentMenu->GetSafeHwnd()
		)
	{
		m_pWndParentMenu->ScreenToClient( &ptScreenClick );
		return
			m_pWndParentMenu->_OnMouseClick(
				nFlags,
				ptScreenClick,
				bNoEat
				);
	}

	if( bEndSequence || ( ! bLButtonUpCall ) )
	{
		COLORREF clr = (COLORREF)__ECST_NONE;
		switch( m_nColorIdxCurr )
		{
		case IDX_DEFAULT_COLOR_BTN:
			clr = (COLORREF)__ECST_BUTTON_DEFAULT;
			break;
		case IDX_CUSTOM_COLOR_BTN:
			clr = (COLORREF)__ECST_BUTTON_CUSTOM;
		    break;
		default:
			clr = _GetColorValue( m_nColorIdxCurr );
		    break;
		}
//		_EndSequence( clr );
// 		_EndSequence( 0xFFFF );
		CExtPopupMenuSite & _site = GetSite();
		UINT * lpnResultCmdID = _site.GetTargetCmdIdPtr();
		if( lpnResultCmdID != NULL )
		{
			(*lpnResultCmdID) = (UINT)clr;
			_site.SetTargetCmdIdPtr( NULL );
		}
 		_EndSequence( 0 );
	}
	return true;
}

bool CExtPopupColorMenuWnd::_OnKeyDown(
	UINT nChar,
	UINT nRepCnt,
	UINT nFlags,
	bool & bNoEat
	)
{
	ASSERT_VALID( this );

	nRepCnt;
	nFlags;
	bNoEat;
	if( GetSafeHwnd() == NULL )
		return true; //false;

	if( GetSite().GetAnimated() != NULL )
		return true;

TranslateKeyboardEventData_t _td( this, nChar, nRepCnt, nFlags, bNoEat );
	if( _td.Notify() )
	{
		bNoEat = _td.m_bNoEat;
		return true;
	}

bool bEat = false;
	switch( nChar )
	{
	case VK_RETURN:
	{
		HWND hWndThis = GetSafeHwnd();
		if(	(	m_nColorIdxCurr >= 0
				|| m_nColorIdxCurr == IDX_DEFAULT_COLOR_BTN
				|| m_nColorIdxCurr == IDX_CUSTOM_COLOR_BTN
				)
			&&	( ! _FindCustomizeMode() )
			)
		{
			_NotifyColorChanged( true );
			g_SoundPlayer->PlaySound( CExtSoundPlayer::__ON_MENU_CMD_CLICKED );
		}
		if( !::IsWindow(hWndThis) )
			return true;

		COLORREF clr = (COLORREF)__ECST_NONE;
		switch( m_nColorIdxCurr )
		{
		case IDX_DEFAULT_COLOR_BTN:
			clr = (COLORREF)__ECST_BUTTON_DEFAULT;
			break;
		case IDX_CUSTOM_COLOR_BTN:
			clr = (COLORREF)__ECST_BUTTON_CUSTOM;
		    break;
		default:
			clr = _GetColorValue( m_nColorIdxCurr );
		    break;
		}
		_EndSequence( clr );
		return true;
	} // VK_RETURN
	
	case VK_MENU:
		_EndSequence();
		return true;
	
	case VK_ESCAPE:
		if( m_bTopLevel )
			_EndSequence();
		else
		{
			int nParentCurIndex =
				m_pWndParentMenu->_GetCurIndex();
			ASSERT(
				nParentCurIndex >= 0
				&&
				nParentCurIndex <=
					m_pWndParentMenu->ItemGetCount()
				);
			HWND hWndOwn = m_hWnd;
			m_pWndParentMenu->_SetCapture();
			m_pWndParentMenu->_ItemFocusCancel(
				FALSE
				);
			m_pWndParentMenu->_ItemFocusSet(
				nParentCurIndex,
				FALSE,
				TRUE
				);
			if( ! ::IsWindow( hWndOwn ) )
				return true;
			m_pWndParentMenu->_SetCapture();
		}
		return true;

//	case VK_RIGHT:
//		bEat = true;
//		if( m_nColorIdxCurr < 0 )
//			m_nColorIdxCurr = 0;
//		else
//		{
//			CPoint ptCoord =
//				_GetColorItemCoord( m_nColorIdxCurr );
//			ptCoord.x++;
//			CSize _sizeDimension = _GetColorDimension();
//			if( ptCoord.x >= _sizeDimension.cx )
//				ptCoord.x = 0;
//			m_nColorIdxCurr = 
//				ptCoord.x + ptCoord.y * _sizeDimension.cx;
//		}
//		break;
//	case VK_LEFT:
//		bEat = true;
//		if( m_nColorIdxCurr < 0 )
//			m_nColorIdxCurr = 0;
//		else
//		{
//			CPoint ptCoord =
//				_GetColorItemCoord( m_nColorIdxCurr );
//			ptCoord.x--;
//			CSize _sizeDimension = _GetColorDimension();
//			if( ptCoord.x < 0 )
//				ptCoord.x = _sizeDimension.cx - 1;
//			m_nColorIdxCurr = 
//				ptCoord.x + ptCoord.y * _sizeDimension.cx;
//		}
//		break;
//	case VK_DOWN:
//		bEat = true;
//		if( m_nColorIdxCurr < 0 )
//			m_nColorIdxCurr = 0;
//		else
//		{
//			CPoint ptCoord =
//				_GetColorItemCoord( m_nColorIdxCurr );
//			ptCoord.y++;
//			CSize _sizeDimension = _GetColorDimension();
//			if( ptCoord.y >= _sizeDimension.cy )
//				ptCoord.y = 0;
//			m_nColorIdxCurr = 
//				ptCoord.x + ptCoord.y * _sizeDimension.cx;
//		}
//		break;
//	case VK_UP:
//		bEat = true;
//		if( m_nColorIdxCurr < 0 )
//			m_nColorIdxCurr = 0;
//		else
//		{
//			CPoint ptCoord =
//				_GetColorItemCoord( m_nColorIdxCurr );
//			ptCoord.y--;
//			CSize _sizeDimension = _GetColorDimension();
//			if( ptCoord.y < 0 )
//				ptCoord.y = _sizeDimension.cy - 1;
//			m_nColorIdxCurr = 
//				ptCoord.x + ptCoord.y * _sizeDimension.cx;
//		}
//		break;

	case VK_LEFT:
	case VK_RIGHT:
	case VK_UP:
	case VK_DOWN:
	case VK_TAB:
		{
			if( nChar == VK_TAB )
			{
				bool bAlt =
					( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 )
						? true : false;
				bool bCtrl =
					( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 )
						? true : false;
				if( bCtrl || bAlt )
					break;
				bool bShift =
					( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 )
						? true : false;
				nChar = bShift ? VK_LEFT : VK_RIGHT;
			}
			int nOldIdx = m_nColorIdxCurr;
			m_nColorIdxCurr = _GetNextKeyColor( nOldIdx, nChar );
			bEat = ( m_nColorIdxCurr != nOldIdx ) ? true : false;
		}
		break;
	} // switch( nChar )

	if( bEat )
	{
		HWND hWndOwn = m_hWnd;
		_CoolTipHide();
		if( ! ::IsWindow( hWndOwn ) )
			return true;
		_NotifyColorChanged();
		Invalidate();
	}

	return bEat;
}

HWND CExtPopupColorMenuWnd::_GetWndNotifyColorChanged()
{
	ASSERT_VALID( this );
	if( m_hWndNotifyColorChanged != NULL )
	{
		ASSERT( ::IsWindow(m_hWndNotifyColorChanged) );
		return m_hWndNotifyColorChanged;
	}
	ASSERT( m_hWndCmdReceiver != NULL );
	ASSERT( ::IsWindow(m_hWndCmdReceiver) );
	return m_hWndCmdReceiver;
}

void CExtPopupColorMenuWnd::_NotifyColorChanged(
	bool bFinal // = false
	)
{
	ASSERT_VALID( this );
	if( m_bHelperInNotifyColorChanged )
		return;
	m_bHelperInNotifyColorChanged = true;

HWND hWndNotify = _GetWndNotifyColorChanged();
	ASSERT( hWndNotify != NULL );
	ASSERT( ::IsWindow(hWndNotify) );

COLORREF clr = RGB(0,0,0);
	if( m_nColorIdxCurr == IDX_CUSTOM_COLOR_BTN )
	{
		if( bFinal && m_pClrValueFinal != NULL )
			(*m_pClrValueFinal) = (COLORREF)__ECST_BUTTON_CUSTOM;

		// if the following pSite->OnColorItemCustom method invoke return false the method will continue
		// but the popup menu object may be invalid because of it can be closed
		HWND hWnd = m_hWnd;
		LPARAM lParamCookie = m_lParamCookie;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		if( m_pNode != NULL )
		{
			ASSERT_VALID( m_pNode );
			CExtCustomizeSite * pSite = _FindCustomizeSite();
			if(		pSite != NULL
				&&	pSite->OnColorItemCustom( m_pNode )
				)
			{
				if( ! ::IsWindow(hWnd) )
					return;
				m_bHelperInNotifyColorChanged = false;
				return;
			}
		} // if( m_pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		
		::PostMessage(
			hWndNotify,
			g_nMsgNotifyCustColor,
			0,
			lParamCookie
			);

		if(		hWnd != NULL 
			&&	::IsWindow( hWnd )
			)
			m_bHelperInNotifyColorChanged = false;

		return;
	} // if( m_nColorIdxCurr == IDX_CUSTOM_COLOR_BTN )
	else if( m_nColorIdxCurr == IDX_DEFAULT_COLOR_BTN )
		clr = m_clrDefault;
	else
	{
		if( m_nColorIdxCurr < 0 )
		{
			clr = m_clrInitial;
			//m_bHelperInNotifyColorChanged = false;
			//return;
//			if( bFinal )
//			{
//				m_bHelperInNotifyColorChanged = false;
//				return;
//			}
//			clr = m_clrInitial;
		}
		else
			clr = _GetColorValue( m_nColorIdxCurr );
	}
	if( bFinal && m_pClrValueFinal != NULL )
		(*m_pClrValueFinal) = clr;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	if( m_pNode != NULL )
	{
		ASSERT_VALID( m_pNode );
		CExtCustomizeSite * pSite = _FindCustomizeSite();
		if(		pSite != NULL
			&&	pSite->OnColorItemChanged(
					NULL,
					m_pNode,
					bFinal,
					clr,
					m_lParamCookie
					)
			)
		{
			m_bHelperInNotifyColorChanged = false;
			return;
		}
	} // if( m_pNode != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	::PostMessage(
		hWndNotify,
		bFinal
			? g_nMsgNotifyColorChangedFinally
			: g_nMsgNotifyColorChanged
			,
		(WPARAM)clr,
		m_lParamCookie
		);
	m_bHelperInNotifyColorChanged = false;
}

int CExtPopupColorMenuWnd::_FindCellByColorRef(COLORREF clr)
{
	ASSERT_VALID( this );
int nIdx, nColorCellCount = _GetColorCellCount();
	for( nIdx = 0; nIdx < nColorCellCount; nIdx++ )
	{
		if( _GetColorValue( nIdx ) == clr )
			return nIdx;
	} // for( nIdx = 0; nIdx < nColorCellCount; nIdx++ )
	return -1;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
// CExtCustomizeSite::ICustomizeDropTarget
DROPEFFECT CExtPopupColorMenuWnd::OnCustomizeTargetOver(
	CExtCustomizeSite::CCmdDragInfo & _dragInfo,
	CPoint point,
	DWORD dwKeyState
	)
{
	ASSERT_VALID( this );
	ASSERT( !_dragInfo.IsEmpty() );
	_dragInfo;
	point;
	dwKeyState;
	return DROPEFFECT_NONE;
}
void CExtPopupColorMenuWnd::OnCustomizeTargetLeave()
{
	ASSERT_VALID( this );
}
bool CExtPopupColorMenuWnd::OnCustomizeTargetDrop(
	CExtCustomizeSite::CCmdDragInfo & _dragInfo,
	CPoint point,
	DROPEFFECT de
	)
{
	ASSERT_VALID( this );
	ASSERT( !_dragInfo.IsEmpty() );;
	_dragInfo;
	point;
	de;
	return false;
}
// CExtCustomizeSite::ICustomizeDropSource
void CExtPopupColorMenuWnd::OnCustomizeSourceDragComplete(
	DROPEFFECT de,
	bool bCanceled,
	bool * p_bNoResetActiveItem
	)
{
	ASSERT_VALID( this );
	ASSERT( p_bNoResetActiveItem != NULL );
	de;
	bCanceled;
	p_bNoResetActiveItem;
}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

/////////////////////////////////////////////////////////////////////////////
// CExtPopupMenuWnd::CInPlaceEditWnd

const UINT CExtPopupMenuWnd::CInPlaceEditWnd::g_nMsgDeleteSelection =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::CInPlaceEditWnd::g_nMsgDeleteSelection")
		);
const UINT CExtPopupMenuWnd::CInPlaceEditWnd::g_nMsgSelectAll =
	::RegisterWindowMessage(
		_T("CExtPopupMenuWnd::CInPlaceEditWnd::g_nMsgSelectAll")
		);

CExtPopupMenuWnd::CInPlaceEditWnd::CInPlaceEditWnd(
	CExtSafeString * pStr,
	CExtPopupMenuWnd::pCbVerifyTextInput pCbVerify, // = NULL
	CExtPopupMenuWnd::pCbPutTextInputResult pCbResult, // = NULL
	CExtPopupMenuWnd::pCbInplaceEditWndProc pCbWndProc, // = NULL
	LPVOID pCbCookie // = NULL
	)
	: m_pStr( pStr )
	, m_pCbVerifyTextInput( pCbVerify )
	, m_pCbPutTextInputResult( pCbResult )
	, m_pCbWndProc( pCbWndProc )
	, m_pCbCookie( pCbCookie )
	, m_bNoCancelAtKillFocus( false )
{
	ASSERT( m_pStr != NULL );
}

CExtPopupMenuWnd::CInPlaceEditWnd::~CInPlaceEditWnd()
{
}

#ifdef _DEBUG
void CExtPopupMenuWnd::CInPlaceEditWnd::AssertValid() const
{
	CEdit::AssertValid();
	ASSERT( m_pStr != NULL );
}
#endif // _DEBUG

bool CExtPopupMenuWnd::CInPlaceEditWnd::Create(
	CExtPopupMenuWnd * pPopup,
	CRect rc,
	UINT nDlgCtrlID
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pPopup );
	ASSERT( pPopup->GetSafeHwnd() != NULL );
	m_bNoCancelAtKillFocus = false;
	m_sTextSaved = *m_pStr;
	
bool bRTL = pPopup->OnQueryLayoutRTL();
	if( ! CEdit::Create(
			WS_CHILD|ES_LEFT|ES_AUTOHSCROLL,
			rc,
			pPopup,
			nDlgCtrlID
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	SetFont( & pPopup->PmBridge_GetPM()->m_FontNormal );
	SetWindowText( *m_pStr );
	SetSel( 0, -1 ); // SetSel( 0, 0 );
	if( bRTL )
		ModifyStyleEx( 0, WS_EX_LAYOUTRTL, SWP_FRAMECHANGED );
	return true;
}

LRESULT CExtPopupMenuWnd::CInPlaceEditWnd::WindowProc(
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
{
	if( m_pCbWndProc != NULL )
	{
		LRESULT lResult = 0L;
		if( m_pCbWndProc(
				lResult,
				message,
				wParam,
				lParam,
				*this,
				m_pCbCookie
				)
			)
			return lResult;
	} // if( m_pCbWndProc != NULL )
	if( message == WM_NCCALCSIZE )
	{
		NCCALCSIZE_PARAMS * pNCCSP =
			reinterpret_cast < NCCALCSIZE_PARAMS * > ( lParam );
		ASSERT( pNCCSP != NULL );
		CRect rcInBarWnd( pNCCSP->rgrc[0] );
		//rcInBarWnd.DeflateRect( 2, 2, 0, 2 );
		rcInBarWnd.DeflateRect( 1, 1, 0, 2 );
		::CopyRect( &(pNCCSP->rgrc[0]), rcInBarWnd );
		return 0;
	} // if( message == WM_NCCALCSIZE )
	if( message == WM_NCPAINT )
	{
		CRect rcInBarWnd, rcInBarClient;
		GetWindowRect( &rcInBarWnd );
		GetClientRect( &rcInBarClient );
		ClientToScreen( &rcInBarClient );
		if( rcInBarWnd == rcInBarClient )
			return 0;
		CPoint ptDevOffset = -rcInBarWnd.TopLeft();
		rcInBarWnd.OffsetRect( ptDevOffset );
		rcInBarClient.OffsetRect( ptDevOffset );
		CWindowDC dc( this );
		ASSERT( dc.GetSafeHdc() != NULL );
		dc.ExcludeClipRect( &rcInBarClient );
		dc.FillSolidRect(
			rcInBarWnd,
			GetPopupMenu()->PmBridge_GetPM()->GetColor( COLOR_WINDOW, this )
			);
		return 0;
	} // if( message == WM_NCPAINT )
	if( message == WM_GETDLGCODE )
		return DLGC_WANTALLKEYS|DLGC_WANTCHARS|DLGC_WANTTAB;
	if(		message == WM_RBUTTONDOWN
		||	message == WM_RBUTTONUP
		||	message == WM_RBUTTONDBLCLK
		||	message == WM_CONTEXTMENU
		)
		return 0;
	
	if( message == WM_KEYDOWN )
	{
		ASSERT( m_pStr != NULL );

		if(		int(wParam) == VK_SHIFT
			||	int(wParam) == VK_CONTROL
			||	int(wParam) == VK_MENU
			)
			return CEdit::WindowProc( message, wParam, lParam );

		if( int(wParam) == VK_ESCAPE || int(wParam) == VK_MENU )
		{
//			if( m_pCbVerifyTextInput != NULL )
//			{
//				if(	m_pCbVerifyTextInput(
//						*this,
//						m_pCbCookie,
//						m_sTextSaved.IsEmpty() ? _T("") : ((LPCTSTR)m_sTextSaved),
//						m_sTextSaved.IsEmpty() ? _T("") : ((LPCTSTR)m_sTextSaved)
//						)
//					)
//				{
//					if( m_pCbPutTextInputResult != NULL )
//						m_pCbPutTextInputResult(
//							*this,
//							m_pCbCookie,
//							m_sTextSaved.IsEmpty() ? _T("") : ((LPCTSTR)m_sTextSaved)
//							);
//					else
//						*m_pStr = m_sTextSaved;
//				}
//			} // if( m_pCbVerifyTextInput != NULL )
//			else
//			{
//					if( m_pCbPutTextInputResult != NULL )
//						m_pCbPutTextInputResult(
//							*this,
//							m_pCbCookie,
//							m_sTextSaved.IsEmpty() ? _T("") : ((LPCTSTR)m_sTextSaved)
//							);
//					else
//						*m_pStr = m_sTextSaved;
//			} // else from if( m_pCbVerifyTextInput != NULL )
			if( int(wParam) == VK_MENU )
			{
				GetPopupMenu()->PostMessage( WM_CANCELMODE );
				return 0;
			}
			CExtPopupMenuWnd * pPopup = GetPopupMenu();
			int nCurIndex = pPopup->_GetCurIndex();
			ASSERT(
					0 <= nCurIndex
				&&	nCurIndex < pPopup->ItemGetCount()
				);
			pPopup->SetFocus();
			HWND hWndOwn = m_hWnd;
			pPopup->_ItemFocusCancel( FALSE );
			if( ! ::IsWindow( hWndOwn ) )
				return 0;
			pPopup->_ItemFocusSet( nCurIndex, FALSE, TRUE );
			if( ! ::IsWindow( hWndOwn ) )
				return 0;
			return 0;
		} // if( int(wParam) == VK_ESCAPE || int(wParam) == VK_MENU )

		if( int(wParam) == VK_RETURN )
		{
			ASSERT( m_pStr != NULL );
			m_bNoCancelAtKillFocus = true;
			CString sText;
			GetWindowText( sText );
			HWND hWndThis = GetSafeHwnd();
			ASSERT( hWndThis != NULL && ::IsWindow(hWndThis) );
			if( m_pCbVerifyTextInput != NULL )
			{
				if(	m_pCbVerifyTextInput(
						*this,
						m_pCbCookie,
						sText.IsEmpty() ? _T("") : ((LPCTSTR)sText),
						sText.IsEmpty() ? _T("") : ((LPCTSTR)sText)
						)
					)
				{
					if( ! ::IsWindow(hWndThis) )
						return 0;
					if( m_pCbPutTextInputResult != NULL )
						m_pCbPutTextInputResult(
							*this,
							m_pCbCookie,
							sText.IsEmpty() ? _T("") : ((LPCTSTR)sText)
							);
					else
						*m_pStr = sText;
				}
			} // if( m_pCbVerifyTextInput != NULL )
			else
			{
					if( ! ::IsWindow(hWndThis) )
						return 0;
					if( m_pCbPutTextInputResult != NULL )
						m_pCbPutTextInputResult(
							*this,
							m_pCbCookie,
							sText.IsEmpty() ? _T("") : ((LPCTSTR)sText)
							);
					else
						*m_pStr = sText;
			} // else from if( m_pCbVerifyTextInput != NULL )
			if( ! ::IsWindow(hWndThis) )
				return 0;
			CExtPopupMenuWnd * pPopup = GetPopupMenu();
			pPopup->PostMessage( WM_CANCELMODE );
			return 0;
		} // if( int(wParam) == VK_RETURN )
		bool bAlt =
			( (::GetAsyncKeyState(VK_MENU)&0x8000) != 0 )
				? true : false;
		if( bAlt )
		{
			GetPopupMenu()->PostMessage( WM_CANCELMODE );
			return 0;
		}
		
		ASSERT( m_pStr != NULL );
		CString sTextOld;
		GetWindowText( sTextOld );
		DWORD dwSelSaved = CEdit::GetSel();
		CEdit::SetRedraw( FALSE );
		LRESULT lResult = CEdit::WindowProc( message, wParam, lParam );
		CString sTextNew;
		GetWindowText( sTextNew );
		if( m_pCbVerifyTextInput != NULL )
		{
			if(	m_pCbVerifyTextInput(
					*this,
					m_pCbCookie,
					sTextOld.IsEmpty() ? _T("") : ((LPCTSTR)sTextOld),
					sTextNew.IsEmpty() ? _T("") : ((LPCTSTR)sTextNew)
					)
				)
			{
				//*m_pStr = sTextNew;
			}
			else
			{
				CEdit::SetSel( 0, -1 );
				CEdit::ReplaceSel( sTextOld );
				CEdit::SetSel( dwSelSaved );
			}
		} // if( m_pCbVerifyTextInput != NULL )
		//else
			//*m_pStr = sTextNew;
		CEdit::SetRedraw( TRUE );
		Invalidate();
		UpdateWindow();
		
		return lResult;
	} // if( message == WM_KEYDOWN )
	else if( message == WM_CHAR )
	{
		if( wParam == VK_RETURN )
			return 0;
		ASSERT( m_pStr != NULL );
		CString sTextOld;
		GetWindowText( sTextOld );
		DWORD dwSelSaved = CEdit::GetSel();
		CEdit::SetRedraw( FALSE );
		LRESULT lResult = CEdit::WindowProc( message, wParam, lParam );
		CString sTextNew;
		GetWindowText( sTextNew );
		if( m_pCbVerifyTextInput != NULL )
		{
			if(	m_pCbVerifyTextInput(
					*this,
					m_pCbCookie,
					sTextOld.IsEmpty() ? _T("") : ((LPCTSTR)sTextOld),
					sTextNew.IsEmpty() ? _T("") : ((LPCTSTR)sTextNew)
					)
				)
			{
				//*m_pStr = sTextNew;
			}
			else
			{
				CEdit::SetSel( 0, -1 );
				CEdit::ReplaceSel( sTextOld );
				CEdit::SetSel( dwSelSaved );
			}
		} // if( m_pCbVerifyTextInput != NULL )
		//else
			//*m_pStr = sTextNew;
		CEdit::SetRedraw( TRUE );
		Invalidate();
		UpdateWindow();

		return lResult;
	} // else if( message == WM_CHAR )
	else if( message == CExtPopupMenuWnd::CInPlaceEditWnd::g_nMsgDeleteSelection )
	{
		//int nChrStart = 0, nChrEnd = 0;
		//CEdit::GetSel( nChrStart, nChrEnd );
		DWORD dwSelSaved = CEdit::GetSel();
		int nChrStart = int(LOWORD(dwSelSaved));
		int nChrEnd = int(HIWORD(dwSelSaved));
		if( m_pCbVerifyTextInput != NULL )
		{
			CString sTextOld, sTextNew;
			GetWindowText( sTextOld );
			SetRedraw( FALSE );
			if( nChrStart != nChrEnd )
				CEdit::ReplaceSel( _T("") );
			else
			{
				nChrEnd = nChrStart + 1;
				SetSel( nChrStart, nChrEnd );
				CEdit::ReplaceSel( _T("") );
				SetSel( nChrStart, nChrStart );
			} // else from if( nChrStart != nChrEnd )
			GetWindowText( sTextNew );
			if(	m_pCbVerifyTextInput(
					*this,
					m_pCbCookie,
					sTextOld.IsEmpty() ? _T("") : ((LPCTSTR)sTextOld),
					sTextNew.IsEmpty() ? _T("") : ((LPCTSTR)sTextNew)
					)
				)
			{
				//*m_pStr = sTextNew;
			}
			else
			{
				CEdit::SetSel( 0, -1 );
				CEdit::ReplaceSel( sTextOld );
				CEdit::SetSel( dwSelSaved );
			}
			SetRedraw( TRUE );
			Invalidate();
		} // if( m_pCbVerifyTextInput != NULL )
//		if( nChrStart != nChrEnd )
//			CEdit::ReplaceSel( _T("") );
//		else
//		{
//			nChrEnd = nChrStart + 1;
//			SetRedraw( FALSE );
//			SetSel( nChrStart, nChrEnd );
//			CEdit::ReplaceSel( _T("") );
//			SetSel( nChrStart, nChrStart );
//			SetRedraw( TRUE );
//			Invalidate();
//		} // else from if( nChrStart != nChrEnd )
		return 0;
	} // else if( message == CExtPopupMenuWnd::CInPlaceEditWnd::g_nMsgDeleteSelection )
	else if( message == CExtPopupMenuWnd::CInPlaceEditWnd::g_nMsgSelectAll )
	{
		CEdit::SetSel( 0, -1 );
		return 0;
	} // else if( message == CExtPopupMenuWnd::CInPlaceEditWnd::g_nMsgSelectAll )
	else if( message == WM_KILLFOCUS )
	{
		ASSERT( m_pStr != NULL );
		ShowWindow( SW_HIDE );
		if( !m_bNoCancelAtKillFocus )
			SetWindowText( *m_pStr );
//		ASSERT( m_pStr != NULL );
//		CString sText;
//		if( m_bNoCancelAtKillFocus )
//			GetWindowText( sText );
//		else
//			sText = (LPCTSTR)m_sTextSaved;
//		if( m_pCbVerifyTextInput != NULL )
//		{
//			if(	m_pCbVerifyTextInput(
//					*this,
//					m_pCbCookie,
//					sText.IsEmpty() ? _T("") : ((LPCTSTR)sText),
//					sText.IsEmpty() ? _T("") : ((LPCTSTR)sText)
//					)
//				)
//			{
//					if( m_pCbPutTextInputResult != NULL )
//						m_pCbPutTextInputResult(
//							*this,
//							m_pCbCookie,
//							sText.IsEmpty() ? _T("") : ((LPCTSTR)sText)
//							);
//					else
//						*m_pStr = sText;
//			}
//		} // if( m_pCbVerifyTextInput != NULL )
//		else
//		{
//					if( m_pCbPutTextInputResult != NULL )
//						m_pCbPutTextInputResult(
//							*this,
//							m_pCbCookie,
//							sText.IsEmpty() ? _T("") : ((LPCTSTR)sText)
//							);
//					else
//						*m_pStr = sText;
//		} // else from if( m_pCbVerifyTextInput != NULL )
	} // else if( message == WM_KILLFOCUS )

	return CEdit::WindowProc( message, wParam, lParam );
}

void CExtPopupMenuWnd::CInPlaceEditWnd::PostNcDestroy()
{
	ASSERT_VALID( this );
	delete this;
}

CExtPopupMenuWnd * CExtPopupMenuWnd::CInPlaceEditWnd::GetPopupMenu()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
CWnd * pWndParent = GetParent();
	ASSERT_VALID( pWndParent );
	ASSERT_KINDOF( CExtPopupMenuWnd, pWndParent );
	ASSERT( pWndParent->GetSafeHwnd() != NULL );
	return STATIC_DOWNCAST( CExtPopupMenuWnd, pWndParent );
}

/////////////////////////////////////////////////////////////////////////////
// CExtShadowCtrl

IMPLEMENT_DYNCREATE( CExtShadowCtrl, CWnd );

CExtShadowCtrl::CExtShadowCtrl()
	: m_rcOuterPadding( 0, 0, 0, 0 )
	, m_rcRenderingPaddingData( 0, 0, 0, 0 )
	, m_hWndParent( NULL )
{
}

CExtShadowCtrl::~CExtShadowCtrl()
{
}

bool CExtShadowCtrl::IsAvailable() const
{
	ASSERT_VALID( this );
	if( ! g_PaintManager.m_bIsWin2000orLater )
		return false;
//	if( ! CExtPopupMenuWnd::g_bMenuWithShadows )
//		return false;
	if(		g_PaintManager.m_pfnSetLayeredWindowAttributes == NULL
		||	g_PaintManager.m_pfnUpdateLayeredWindow == NULL
		||	CExtPaintManager::stat_GetBPP() < 15
		)
		return false;
	return true;
}

bool CExtShadowCtrl::Create(
	HWND hWndParent,
	INT nShadowSize
	)
{ ////// return false;
	ASSERT_VALID( this );
	ASSERT( m_hWnd == NULL );
	ASSERT( nShadowSize >= 0 );
	if( ! IsAvailable() )
		return false;
	ASSERT( g_PaintManager.m_bIsWin2000orLater );
	ASSERT( g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL );
	ASSERT( g_PaintManager.m_pfnUpdateLayeredWindow != NULL );
	if(		hWndParent == NULL
		||	(! ::IsWindow( hWndParent ) )
		)
	{
		ASSERT( FALSE );
		return false;
	}
__EXT_MFC_LONG_PTR dwParentStyle = ::__EXT_MFC_GetWindowLong( hWndParent, GWL_STYLE );
	if( ( dwParentStyle & WS_CHILD ) != 0 )
	{
		ASSERT( FALSE );
		return false;
	}
static TCHAR strShadowClassName[] = _T("Professional Shadow");
static bool g_bWndClassRegistered = false;
	if( ! g_bWndClassRegistered )
	{
		WNDCLASS _wndClassInfo;
		HINSTANCE hInst = ::AfxGetInstanceHandle();
		if( ! ::GetClassInfo( hInst, strShadowClassName, &_wndClassInfo ) )
		{
			_wndClassInfo.style = __SHADOW_WNDCLASS_STYLES__;
			_wndClassInfo.lpfnWndProc = ::DefWindowProc;
			_wndClassInfo.cbClsExtra = _wndClassInfo.cbWndExtra = 0;
			_wndClassInfo.hInstance = hInst;
			_wndClassInfo.hIcon = NULL;
			_wndClassInfo.hCursor = ::LoadCursor( NULL, IDC_ARROW );
			ASSERT( _wndClassInfo.hCursor != NULL );
			_wndClassInfo.hbrBackground = NULL; 
			_wndClassInfo.lpszMenuName = NULL;
			_wndClassInfo.lpszClassName = strShadowClassName;
			if( !::AfxRegisterClass( & _wndClassInfo ) )
			{
				ASSERT( FALSE );
				return false;
			}
		}
		g_bWndClassRegistered = true;
	} // if( ! g_bWndClassRegistered )
	if( m_bmpLayerSource.IsEmpty() )
	{
		static BYTE g_arrBmpBytes1x1[] =
		{
			0x42,0x4D,0x36,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x01,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x00,0x00,0x00,0x2B,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,
			0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x2B,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x2B,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x0E,0x0E
		};
		static BYTE g_arrBmpBytes2x2[] =
		{
			0x42,0x4D,0x36,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x01,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x00,0x00,0x00,0x2B,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,
			0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x2B,0x00,0x00,0x00,0x0E,0x00,0x00,0x00,0x13,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,
			0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3A,0x00,0x00,0x00,0x13,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x2B,0x00,0x00,0x00,0x0E,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x0E,0x00,0x00,0x00,0x05,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
		};
		static BYTE g_arrBmpBytes3x3[] =
		{
			0x42,0x4D,0x36,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x01,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x11,0x00,0x00,0x00,0x22,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,
			0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x22,0x00,0x00,0x00,0x11,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x3D,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,
			0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x3D,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x1A,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,
			0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x4D,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x1A,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3D,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x17,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x22,0x00,0x00,0x00,0x11,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x11,0x00,0x00,0x00,0x08,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x03,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
		};
		static BYTE g_arrBmpBytes4x4[] =
		{
			0x42,0x4D,0x36,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x01,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x0B,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,
			0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x0E,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x34,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,
			0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x34,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x34,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x41,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,
			0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x41,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x34,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x41,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,
			0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x41,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x1E,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x41,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x1E,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x34,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x1B,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x34,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x18,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x0C,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x0E,0x00,0x00,0x00,0x0B,0x00,0x00,0x00,0x07,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x04,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x01,0x01
		};
		static BYTE g_arrBmpBytes5x5[] =
		{
			0x42,0x4D,0x36,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x01,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,
			0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x0B,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,
			0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x0B,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x0E,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x34,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,
			0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x34,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x0E,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x34,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x41,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,
			0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x41,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x34,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x41,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,
			0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x4A,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x41,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x44,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x41,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x12,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x34,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x10,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x34,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x0E,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x23,0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x0B,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x18,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x07,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x0E,0x00,0x00,0x00,0x0B,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x05,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x02,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x01
		};
		static BYTE g_arrBmpBytes6x6[] =
		{
			0x42,0x4D,0x36,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x01,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x0D,0x00,0x00,0x00,0x13,0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x1D,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,
			0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1D,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x13,0x00,0x00,0x00,0x0D,0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x13,0x00,0x00,0x00,0x1A,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x25,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x2D,0x00,0x00,
			0x00,0x2D,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x25,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x1A,0x00,0x00,0x00,0x13,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x38,0x00,0x00,
			0x00,0x38,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x0B,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x25,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x35,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x40,0x00,0x00,
			0x00,0x40,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x35,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x25,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x0B,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x14,0x00,0x00,0x00,0x1D,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x41,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x47,0x00,0x00,
			0x00,0x47,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x47,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x41,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x1D,0x00,0x00,0x00,0x14,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x49,0x00,0x00,0x00,0x4B,0x00,0x00,0x00,0x4B,0x00,0x00,0x00,0x4B,0x00,0x00,0x00,0x4B,0x00,0x00,0x00,0x4B,0x00,0x00,0x00,0x4B,0x00,0x00,0x00,0x4B,0x00,0x00,0x00,0x4B,0x00,0x00,0x00,0x4B,0x00,0x00,0x00,0x4B,0x00,0x00,
			0x00,0x4B,0x00,0x00,0x00,0x4B,0x00,0x00,0x00,0x4B,0x00,0x00,0x00,0x4B,0x00,0x00,0x00,0x4B,0x00,0x00,0x00,0x4B,0x00,0x00,0x00,0x49,0x00,0x00,0x00,0x45,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0C,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x2D,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0D,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x36,0x00,0x00,0x00,0x2C,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x15,0x00,0x00,0x00,0x0C,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x3B,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x29,0x00,0x00,0x00,0x1D,0x00,0x00,0x00,0x14,0x00,0x00,0x00,0x0C,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x35,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x25,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x0B,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x2E,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x09,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x25,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x1A,0x00,0x00,0x00,0x13,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x08,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x1B,0x00,0x00,0x00,0x17,0x00,0x00,0x00,0x13,0x00,0x00,0x00,0x0D,0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x05,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x12,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x04,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x0B,0x00,0x00,0x00,0x09,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x02,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x05,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x01,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,
			0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00
		};
		LPBYTE pArrBmpBytes = NULL;
		INT nBmpBytesCount = 0;
		CRect _rcOuterPadding( 0, 0, 0, 0 ), _rcRenderingPaddingData( 0, 0, 0, 0 );
		switch( nShadowSize )
		{
		case 0:
			return false;
		case 1:
				pArrBmpBytes = g_arrBmpBytes1x1;
				nBmpBytesCount = INT( sizeof( g_arrBmpBytes1x1 ) / sizeof( g_arrBmpBytes1x1[0] ) );
				_rcOuterPadding.SetRect( 0, 0, 1, 1 );
				_rcRenderingPaddingData.SetRect( 3, 3, 3, 3 );
			break;
		case 2:
				pArrBmpBytes = g_arrBmpBytes2x2;
				nBmpBytesCount = INT( sizeof( g_arrBmpBytes2x2 ) / sizeof( g_arrBmpBytes2x2[0] ) );
				_rcOuterPadding.SetRect( 0, 0, 2, 2 );
				_rcRenderingPaddingData.SetRect( 4, 4, 4, 4 );
			break;
		case 3:
				pArrBmpBytes = g_arrBmpBytes3x3;
				nBmpBytesCount = INT( sizeof( g_arrBmpBytes3x3 ) / sizeof( g_arrBmpBytes3x3[0] ) );
				_rcOuterPadding.SetRect( 0, 0, 3, 3 );
				_rcRenderingPaddingData.SetRect( 6, 6, 6, 6 );
			break;
		case 4:
				pArrBmpBytes = g_arrBmpBytes4x4;
				nBmpBytesCount = INT( sizeof( g_arrBmpBytes4x4 ) / sizeof( g_arrBmpBytes4x4[0] ) );
				_rcOuterPadding.SetRect( 0, 0, 4, 4 );
				_rcRenderingPaddingData.SetRect( 8, 8, 8, 8 );
			break;
		case 5:
				pArrBmpBytes = g_arrBmpBytes5x5;
				nBmpBytesCount = INT( sizeof( g_arrBmpBytes5x5 ) / sizeof( g_arrBmpBytes5x5[0] ) );
				_rcOuterPadding.SetRect( 0, 0, 5, 5 );
				_rcRenderingPaddingData.SetRect( 8, 8, 8, 8 );
			break;
		default:
				nShadowSize = 6;
				pArrBmpBytes = g_arrBmpBytes6x6;
				nBmpBytesCount = INT( sizeof( g_arrBmpBytes6x6 ) / sizeof( g_arrBmpBytes6x6[0] ) );
				_rcOuterPadding.SetRect( 0, 0, 6, 6 );
				_rcRenderingPaddingData.SetRect( 10, 10, 10, 10 );
			break;
		} // switch( nShadowSize )
		ASSERT( pArrBmpBytes != NULL );
		ASSERT( nBmpBytesCount > 0 );
		if( ! m_bmpLayerSource.LoadBMP_Buffer( pArrBmpBytes, nBmpBytesCount, true ) )
		{
			ASSERT( FALSE );
			return false;
		}
		m_rcOuterPadding = _rcOuterPadding;
		m_rcRenderingPaddingData = _rcRenderingPaddingData;
	} // if( m_bmpLayerSource.IsEmpty() )
CRect rcWnd;
	::GetWindowRect( hWndParent, &rcWnd );
 	rcWnd.InflateRect(
 		m_rcOuterPadding.left,
 		m_rcOuterPadding.top,
 		m_rcOuterPadding.right,
 		m_rcOuterPadding.bottom
 		);
CSize _sizeWnd = rcWnd.Size();
	if( m_bmpLayerData.GetSafeHandle() == NULL )
	{
		CExtBitmap _bmpColored;
		CExtBitmap * pBmpSrc = &m_bmpLayerSource;
		COLORREF clrShadow = g_PaintManager->GetShadowAdjustColor();
		if( clrShadow != COLORREF(-1L) )
		{
			_bmpColored = (*pBmpSrc);
			if( ! _bmpColored.IsEmpty() )
			{
				_bmpColored.AdjustHLS( COLORREF(-1L), RGB(0,0,0), 0.0, 0.30, 0.5 );
				_bmpColored.ColorizeMono( COLORREF(-1L), clrShadow );
				pBmpSrc = &_bmpColored;
			}
		} // if( clrShadow != COLORREF(-1L) )
		pBmpSrc->PreMultiplyRGBChannels();
		CWindowDC dcDesktop(NULL);
		BITMAPINFOHEADER bih;
		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biWidth = _sizeWnd.cx;
		bih.biHeight = _sizeWnd.cy;
		bih.biPlanes = 1;
		bih.biBitCount = 32;
		bih.biCompression = BI_RGB;
		bih.biSizeImage = bih.biWidth*bih.biHeight;
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;
		COLORREF * pClrSurface = NULL;
		HBITMAP hDIB =
			::CreateDIBSection(
				dcDesktop.GetSafeHdc(),
				(LPBITMAPINFO)&bih,
				DIB_RGB_COLORS,
				(void **)&pClrSurface,
				NULL,
				NULL
				);
		if( hDIB == NULL )
		{
			ASSERT( FALSE );
			return false;
		}
		ASSERT( pClrSurface != NULL );
		CDC dc;
		if( ! dc.CreateCompatibleDC( &dcDesktop ) )
		{
			ASSERT( FALSE );
			return false;
		}
		HGDIOBJ hOld = dc.SelectObject( hDIB );
		CRect rcPadding = m_rcRenderingPaddingData;
		if( ( rcPadding.left + rcPadding.right ) >= _sizeWnd.cx )
			rcPadding.left = rcPadding.right = 0;
		if( ( rcPadding.top + rcPadding.bottom ) >= _sizeWnd.cy )
			rcPadding.top = rcPadding.bottom = 0;
		CWnd * pWndPermanent = CWnd::FromHandlePermanent( hWndParent );
		bool bDefaultSimpleShadow = true;
		if( pWndPermanent != NULL )
		{
			ASSERT_VALID( pWndPermanent );
			CExtPopupMenuWnd * pPopup = DYNAMIC_DOWNCAST( CExtPopupMenuWnd, pWndPermanent );
			if( pPopup != NULL )
			{
				if( pPopup->_IsCombineWithEA() )
				{
					bDefaultSimpleShadow = false;
					CRect rcClient, rcExcludeArea = pPopup->GetExcludeArea();
					pPopup->ScreenToClient( &rcExcludeArea );
					pPopup->_GetClientRect( &rcClient );
					dc.ExcludeClipRect( &rcExcludeArea );
					dc.ExcludeClipRect( &rcClient );
					rcExcludeArea.InflateRect( 0, 0, nShadowSize, nShadowSize );
					rcClient.InflateRect( 0, 0, nShadowSize, nShadowSize );
					pBmpSrc->AlphaBlendSkinParts(
						dc.GetSafeHdc(),
						rcExcludeArea,
						rcPadding,
						CExtBitmap::__EDM_STRETCH,
						true
						);
					pBmpSrc->AlphaBlendSkinParts(
						dc.GetSafeHdc(),
						rcClient,
						rcPadding,
						CExtBitmap::__EDM_STRETCH,
						true
						);
				} // if( pPopup->_IsCombineWithEA() )
			} // if( pPopup != NULL )
		} // if( pWndPermanent != NULL )
		if( bDefaultSimpleShadow )
		{
			CRect rcDst( 0, 0, _sizeWnd.cx, _sizeWnd.cy );
			pBmpSrc->AlphaBlendSkinParts(
				dc.GetSafeHdc(),
				rcDst,
				rcPadding,
				CExtBitmap::__EDM_STRETCH,
				true
				);
		} // if( bDefaultSimpleShadow )
		dc.SelectObject( hOld );
		m_bmpLayerData.Attach( hDIB );
	} // if( m_bmpLayerData.GetSafeHandle() == NULL )
	if( ! CWnd::CreateEx(
			WS_EX_TOPMOST
				|__EXT_MFC_WS_EX_LAYERED
				| WS_EX_TRANSPARENT
				| ( g_PaintManager.m_bIsWin2000orLater
						? (WS_EX_NOINHERITLAYOUT)
						: 0
					)
				,
			strShadowClassName,
			NULL,
			WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
			rcWnd.left,
			rcWnd.top,
			_sizeWnd.cx,
			_sizeWnd.cy,
			hWndParent,
			(HMENU)NULL
			)
		)
	{
		ASSERT( FALSE );
		return false;
	}
	if( UpdateWindowLayer( (BYTE)255 ) )
	{
		::SetWindowPos(
			m_hWnd, hWndParent, 0, 0, 0, 0,
			SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE
 				|SWP_SHOWWINDOW
				|SWP_NOSENDCHANGING
				|SWP_NOREDRAW
				 //|SWP_NOCOPYBITS
			);
	}
	m_hWndParent = hWndParent;
	//SetupHookWndSink( m_hWndParent );
	return true;
}

bool CExtShadowCtrl::UpdateWindowLayer(
	BYTE nConstantAlpha
	)
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( g_PaintManager.m_bIsWin2000orLater );
	ASSERT( g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL );
	ASSERT( g_PaintManager.m_pfnUpdateLayeredWindow != NULL );
	return UpdateWindowLayer( m_bmpLayerData, nConstantAlpha );
}

bool CExtShadowCtrl::UpdateWindowLayer(
	CBitmap & _bmpLayer,
	BYTE nConstantAlpha
	)
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( g_PaintManager.m_bIsWin2000orLater );
	ASSERT( g_PaintManager.m_pfnSetLayeredWindowAttributes != NULL );
	ASSERT( g_PaintManager.m_pfnUpdateLayeredWindow != NULL );
	if( _bmpLayer.GetSafeHandle() == NULL )
		return false;
bool bRetVal = false;
CWindowDC dcDesktop( NULL );
CDC dcMem;
	if( dcMem.CreateCompatibleDC( &dcDesktop ) )
	{
		CBitmap * pOldBmp = dcMem.SelectObject( &_bmpLayer );
		BITMAP _bi;
		::memset( &_bi, 0, sizeof(BITMAP) );
		_bmpLayer.GetBitmap( &_bi );
		CExtPaintManager::CExtPaintManagerAutoPtr::__EXT_MFC_BLENDFUNCTION _bf =
		{
			__EXT_MFC_AC_SRC_OVER,
			0,
			nConstantAlpha,
			__EXT_MFC_AC_SRC_ALPHA
		};
		CRect rcWnd;
		GetWindowRect( &rcWnd );
		CPoint ptSrc( 0, 0 ), ptDst( rcWnd.TopLeft() );
		CSize sizeDst = rcWnd.Size();
		bRetVal =
			g_PaintManager.m_pfnUpdateLayeredWindow(
				GetSafeHwnd(),
				dcDesktop,
				&ptDst,
				&sizeDst,
				dcMem,
				&ptSrc,
				0,
				&_bf,
				__EXT_MFC_ULW_ALPHA
				) ? true : false;
		dcMem.SelectObject( pOldBmp );
	} // if( dcMem.CreateCompatibleDC( &dcDesktop ) )
	return bRetVal;
}

LRESULT CExtShadowCtrl::WindowProc(
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
{
	switch( message )
	{
	case WM_NCCALCSIZE:
		return 0;
	case WM_NCDESTROY:
	case WM_DESTROY:
		m_hWndParent = NULL;
//		RemoveAllWndHooks();
		if( m_bmpLayerData.GetSafeHandle() != NULL )
			m_bmpLayerData.DeleteObject();
		break;
	} // switch( message )
LRESULT lResult = CWnd::WindowProc( message, wParam, lParam );
	return lResult;
}

//bool CExtShadowCtrl::OnHookWndMsg(
//	LRESULT & lResult,
//	HWND hWndHooked,
//	UINT nMessage,
//	WPARAM & wParam,
//	LPARAM & lParam
//	)
//{
//__PROF_UIS_MANAGE_STATE;
//	if( hWndHooked == m_hWndParent && GetSafeHwnd() != NULL )
//	{
//		switch( nMessage )
//		{
//		case WM_DESTROY:
//		case WM_NCDESTROY:
//			DestroyWindow();
//			break;
//		} // switch( nMessage )
//	} // if( hWndHooked == m_hWndParent ...
//bool bRetVal =
//	CExtHookSink::OnHookWndMsg(
//		lResult,
//		hWndHooked,
//		nMessage,
//		wParam,
//		lParam
//		);
//	return bRetVal;
//}

void CExtPopupMenuWnd::_ForceCancelCallbacs()
{
	ASSERT_VALID( this );
}
