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

#if (!defined __EXT_MFC_NO_SPIN)

#if (!defined __EXT_SPIN_H)
	#include <ExtSpinWnd.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtSpinWnd

IMPLEMENT_DYNCREATE( CExtSpinWnd, CSpinButtonCtrl );

CExtSpinWnd::CExtSpinWnd()
	: m_clrBackground( COLORREF(-1L) )
	, m_bCanceling( false )
	, m_bButtonUpHovered( false )
	, m_bButtonDownHovered( false )
	, m_bButtonUpPressed( false )
	, m_bButtonDownPressed( false )
	, m_bNcCalcSize( false )
{
}

CExtSpinWnd::~CExtSpinWnd()
{
CExtAnimationSite * pAcAS = AnimationClient_SiteGet();
	if( pAcAS != NULL )
		pAcAS->AnimationSite_ClientRemove( this );
}

BEGIN_MESSAGE_MAP(CExtSpinWnd, CSpinButtonCtrl)
	//{{AFX_MSG_MAP(CExtSpinWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CANCELMODE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATEAPP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtSpinWnd message handlers

void CExtSpinWnd::OnPaint() 
{
	ASSERT_VALID( this );
    if( ! m_bNcCalcSize )
        SetWindowPos(
            NULL, 0, 0, 0, 0,
            SWP_NOMOVE|SWP_NOSIZE 
            |SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_FRAMECHANGED
            );
CPaintDC dcPaint( this );
CRect rcClient;
	GetClientRect( &rcClient );
	if( ! dcPaint.RectVisible(&rcClient) )
		return;
CExtMemoryDC dc( &dcPaint, &rcClient );
	OnSpinDrawEntire( dc, rcClient );
}

bool CExtSpinWnd::_IsBuddyToEdit()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return false;
DWORD dwStyle = GetStyle();
bool bSpinAlignRight = ( ( dwStyle & UDS_ALIGNRIGHT ) != 0 ) ? true : false;
bool bSpinAlignLeft = ( ( dwStyle & UDS_ALIGNLEFT ) != 0 ) ? true : false;
	if( ! ( bSpinAlignRight || bSpinAlignLeft ) )
		return false;
HWND hWndBuddyTest = NULL;
static const TCHAR szEdit[] = _T("EDIT");
TCHAR szCompare[ 512 ];
	if( ( dwStyle & UDS_AUTOBUDDY ) != 0 )
	{
		hWndBuddyTest = ::GetWindow( m_hWnd, GW_HWNDPREV );
		if( hWndBuddyTest != NULL )
		{
			::memset( szCompare, 0, sizeof(szCompare) );
			::GetClassName( hWndBuddyTest, szCompare, sizeof(szCompare)/sizeof(szCompare[0]) );
			if( ::lstrcmpi( szCompare, szEdit ) != 0 )
				hWndBuddyTest = NULL;
		}
	}
	if( hWndBuddyTest == NULL )
	{
		hWndBuddyTest = (HWND) SendMessage( UDM_GETBUDDY, 0L, 0L );
		if( hWndBuddyTest != NULL && ::IsWindow( hWndBuddyTest ) )
		{
			::memset( szCompare, 0, sizeof(szCompare) );
			::GetClassName( hWndBuddyTest, szCompare, sizeof(szCompare)/sizeof(szCompare[0]) );
			if( ::lstrcmpi( szCompare, szEdit ) != 0 )
				hWndBuddyTest = NULL;
		}
		else
			hWndBuddyTest = NULL;
	}
	return ( hWndBuddyTest != NULL ) ? true : false;
}

bool CExtSpinWnd::_IsEnabled()
{
	ASSERT_VALID( this );
	bool bEnabled  = IsWindowEnabled() ? true : false;
	if( !bEnabled )
		return bEnabled;
	DWORD dwStyle = GetStyle();
	if( (dwStyle&UDS_AUTOBUDDY) != 0 )
	{
		CWnd * pWnd = this->GetWindow( GW_HWNDPREV );
		if( pWnd != NULL )
		{
			ASSERT_VALID( pWnd );
			bEnabled  = pWnd->IsWindowEnabled() ? true : false;
		}
	}
	return bEnabled;
}

bool CExtSpinWnd::_IsReadOnly()
{
	ASSERT_VALID( this );
	bool bReadOnly = false;
	if( _IsBuddyToEdit() )
	{
		CWnd * pWnd = this->GetWindow( GW_HWNDPREV );
		if( pWnd != NULL )
		{
			ASSERT_VALID( pWnd );
			DWORD dwStyle = pWnd->GetStyle();
			if( (dwStyle&ES_READONLY) != 0 )
				bReadOnly = true;
		}
	}
	return bReadOnly;
}

bool CExtSpinWnd::GetButtonRect( 
	CRect & rcButtonUp,
	CRect & rcButtonDown,
	const CRect * prcClient // = NULL
	)
{
	ASSERT_VALID( this );

	CRect rcClient;
	if( prcClient == NULL )
		GetClientRect( &rcClient );
	else
		rcClient = *prcClient;

	DWORD dwStyle = GetStyle();
	bool bHorz = ((dwStyle&UDS_HORZ) != 0) ? true : false;

	rcButtonUp = rcButtonDown = rcClient;
	if( !bHorz )
	{
		rcButtonUp.bottom = rcButtonUp.top + (rcClient.Height() / 2);
		rcButtonDown.top = rcButtonUp.bottom;
	}
	else
	{
		rcButtonUp.right = (rcClient.Width() / 2);
		rcButtonDown.left = rcButtonUp.right;
	}

	return true;
}

void CExtSpinWnd::OnSpinEraseClientArea(
	CDC & dc,
	const CRect & rcClient
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	
	if( !dc.RectVisible( rcClient ) )
		return;

	COLORREF clrBackground = OnQueryBkColor();
	if(		_IsBuddyToEdit() 
		&&	clrBackground == COLORREF(-1L)
		)
	{
		if(		(!_IsEnabled())
			||	_IsReadOnly()
			)
			clrBackground = PmBridge_GetPM()->GetColor( COLOR_3DFACE, this );
		else
			clrBackground = PmBridge_GetPM()->GetColor( COLOR_WINDOW, this );
	}
bool bTransparent = false;
	if(		(! bTransparent )
		&&	PmBridge_GetPM()->GetCb2DbTransparentMode(this)
		&&	( clrBackground == COLORREF(-1L) )
		)
	{
		CExtPaintManager::stat_ExcludeChildAreas(
			dc,
			GetSafeHwnd(),
			CExtPaintManager::stat_DefExcludeChildAreaCallback
			);
		if( PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
			bTransparent = true;
	}
	if( ! bTransparent )
		dc.FillSolidRect(
			&rcClient,
			(clrBackground != COLORREF(-1L)) 
				? clrBackground 
				: PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT, this ) 
			);
	
	PmBridge_GetPM()->OnPaintSessionComplete( this );
}

void CExtSpinWnd::OnSpinDrawEntire(
	CDC & dc,
	const CRect & rcClient
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	
	if( ! dc.RectVisible( rcClient ) )
		return;

	if( AnimationClient_StatePaint( dc ) )
		return;

	OnSpinEraseClientArea( 
		dc, 
		rcClient 
		);

	DWORD dwStyle = GetStyle();

	bool bHorz = ((dwStyle&UDS_HORZ) != 0) ? true : false;
	bool bEnabled  = _IsEnabled();
	bool bFlat = false;
	bool bHotTrack = ((dwStyle&UDS_HOTTRACK) != 0) ? true : false;
	bool bDrawBorder = true;
	
	CRect rcButtonUp;
	CRect rcButtonDown;
	GetButtonRect( 
		rcButtonUp, 
		rcButtonDown,
		&rcClient 
		);

	PmBridge_GetPM()->PaintSpinButton(
		dc,
		rcButtonUp,
		true,		
		bHorz,
		bEnabled,
		bFlat,
		bDrawBorder,
		m_bButtonUpPressed,
		m_bButtonUpHovered,
		bHotTrack,
		this,
		0L
		);

	PmBridge_GetPM()->PaintSpinButton(
		dc,
		rcButtonDown,
		false,		
		bHorz,
		bEnabled,
		bFlat,
		bDrawBorder,
		m_bButtonDownPressed,
		m_bButtonDownHovered,
		bHotTrack,
		this,
		0L
		);
}

BOOL CExtSpinWnd::OnEraseBkgnd(CDC* pDC) 
{
	pDC;
	return TRUE;
}

#if _MFC_VER < 0x700
void CExtSpinWnd::OnActivateApp(BOOL bActive, HTASK hTask) 
#else
void CExtSpinWnd::OnActivateApp(BOOL bActive, DWORD hTask) 
#endif
{
	CSpinButtonCtrl::OnActivateApp(bActive, hTask);
	if( ! bActive )
		SendMessage( WM_CANCELMODE );
}

void CExtSpinWnd::OnCancelMode() 
{
	CSpinButtonCtrl::OnCancelMode();

	if( m_bCanceling )
		return;
	m_bCanceling = true;

	if( ::GetCapture() == GetSafeHwnd() )
		::ReleaseCapture();

	if(		m_bButtonUpHovered
		||	m_bButtonDownHovered
		||	m_bButtonUpPressed
		||	m_bButtonDownPressed
		)
	{
		bool bExitingPushedState = ( m_bButtonUpPressed || m_bButtonDownPressed ) ? true : false;
		bool bAnimationLocked = AnimationClient_CacheGeneratorIsLocked();
		if( ! bAnimationLocked )
		{
			AnimationClient_CacheGeneratorLock();
	//		if( AnimationClient_StateGet(true).IsEmpty() )
 				AnimationClient_CacheNextStateMinInfo(
					false,
					bExitingPushedState ? __EAPT_BY_PRESSED_STATE_TURNED_OFF : __EAPT_BY_HOVERED_STATE_TURNED_OFF
					);
		}

		m_bButtonUpHovered = false;
		m_bButtonDownHovered = false;
		m_bButtonUpPressed = false;
		m_bButtonDownPressed = false;

		if( ! bAnimationLocked )
		{
 			AnimationClient_CacheNextStateMinInfo(
				true,
				bExitingPushedState ? __EAPT_BY_PRESSED_STATE_TURNED_OFF : __EAPT_BY_HOVERED_STATE_TURNED_OFF
				);
			AnimationClient_CacheGeneratorUnlock();
		}

		Invalidate();
	}

	m_bCanceling = false;
}

void CExtSpinWnd::OnCaptureChanged(CWnd *pWnd) 
{
	CSpinButtonCtrl::OnCaptureChanged(pWnd);

	if( m_bCanceling )
		return;

HWND hWndCapture = ::GetCapture();
	if(		hWndCapture != m_hWnd
		&&	hWndCapture != NULL
		)
		SendMessage( WM_CANCELMODE );
}

void CExtSpinWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CSpinButtonCtrl::OnLButtonDown(nFlags, point);
	_ProcessMouseClick( point, true, MK_LBUTTON );
}

void CExtSpinWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CSpinButtonCtrl::OnLButtonUp(nFlags, point);
	_ProcessMouseClick( point, false, MK_LBUTTON );
}

void CExtSpinWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	CSpinButtonCtrl::OnMouseMove(nFlags, point);
	_ProcessMouseMove( point );
}

void CExtSpinWnd::PreSubclassWindow() 
{
	CSpinButtonCtrl::PreSubclassWindow();

	m_bCanceling = false;
	m_bButtonUpHovered = false;
	m_bButtonDownHovered = false;
	m_bButtonUpPressed = false;
	m_bButtonDownPressed = false;
	m_bNcCalcSize = false;

	AnimationSite_ClientProgressStop( this );
	AnimationClient_StateGet( false ).Empty();
	AnimationClient_StateGet( true ).Empty();
CRect rcClient;
	GetClientRect( &rcClient );
	AnimationClient_TargetRectSet( rcClient );
}

LRESULT CExtSpinWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
#if (defined WM_UPDATEUISTATE)	
	ASSERT( WM_UPDATEUISTATE == 0x0128 );
#endif
	switch( message )
	{
	case 0x0128: // WM_UPDATEUISTATE causes repaint without WM_PAINT, so we eat it
		return 0;
	case WM_SETTEXT: 
	case WM_ENABLE:
	{
		LRESULT lResult = CSpinButtonCtrl::WindowProc(message, wParam, lParam);
		Invalidate();
		UpdateWindow();
		return lResult;
	}
	case WM_TIMER:
		if( AnimationSite_OnHookTimer( UINT(wParam) ) )
			return 0L;
	break;
	case WM_PRINT:
	case WM_PRINTCLIENT:
	{
		CDC * pDC = CDC::FromHandle( (HDC)wParam );
		CRect rcClient;
		GetClientRect( &rcClient );
		OnSpinDrawEntire( *pDC, rcClient );
		return (!0);
	}
	case UDM_SETBUDDY:
	{
		HWND hWndBuddy = (HWND)wParam;
		if( hWndBuddy != NULL && ::IsWindow(hWndBuddy) )
			::SetWindowPos( m_hWnd, hWndBuddy, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOOWNERZORDER|SWP_NOACTIVATE );
	}
	break;
	} // switch( message )
	return CSpinButtonCtrl::WindowProc(message, wParam, lParam);
}

void CExtSpinWnd::SetBkColor( COLORREF clrBk )
{ 
	ASSERT_VALID( this );
	m_clrBackground = clrBk; 
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

COLORREF CExtSpinWnd::GetBkColor() const
{ 
	ASSERT_VALID( this );
	return m_clrBackground; 
}

COLORREF CExtSpinWnd::OnQueryBkColor() const
{ 
	ASSERT_VALID( this );
	return GetBkColor(); 
}

bool CExtSpinWnd::_ProcessMouseClick(
	CPoint point,
	bool bButtonPressed,
	INT nMouseButton // MK_... values
	)
{
	ASSERT_VALID( this );

	// process only left mouse button clicks
	if( nMouseButton != MK_LBUTTON )
		return false;

CRect rcButtonUp, rcButtonDown;
	GetButtonRect( rcButtonUp, rcButtonDown );

bool bButtonUpHoveredPrev = m_bButtonUpHovered;
bool bButtonDownHoveredPrev = m_bButtonDownHovered;
bool bButtonUpHoveredNext = false;
bool bButtonDownHoveredNext = false;

bool bButtonUpPressedPrev = m_bButtonUpPressed;
bool bButtonDownPressedPrev = m_bButtonDownPressed;
bool bButtonUpPressedNext = false;
bool bButtonDownPressedNext = false;

	if(		(! rcButtonUp.IsRectEmpty() )
		&&	rcButtonUp.PtInRect( point )
		)
	{
		bButtonUpHoveredNext = true;
		if( bButtonPressed )
			bButtonUpPressedNext = true;
	}
	if(		(! rcButtonDown.IsRectEmpty() )
		&&	rcButtonDown.PtInRect( point )
		)
	{
		bButtonDownHoveredNext = true;
		if( bButtonPressed )
			bButtonDownPressedNext = true;
	}

	if(		bButtonUpHoveredNext
		||	bButtonDownHoveredNext
		||	bButtonUpPressedNext
		||	bButtonDownPressedNext
		)
	{
		if( ::GetCapture() != GetSafeHwnd() )
			::SetCapture( GetSafeHwnd() );
	}
	else
	{
		if( ::GetCapture() == GetSafeHwnd() )
			::ReleaseCapture();
	}

	if(		bButtonUpHoveredNext != bButtonUpHoveredPrev
		||	bButtonDownHoveredNext != bButtonDownHoveredPrev
		||	bButtonUpPressedNext != bButtonUpPressedPrev
		||	bButtonDownPressedNext != bButtonDownPressedPrev
		)
	{
		bool bAnimationLocked = AnimationClient_CacheGeneratorIsLocked();
		if( ! bAnimationLocked )
		{
			AnimationClient_CacheGeneratorLock();
//			if( AnimationClient_StateGet(true).IsEmpty() )
 				AnimationClient_CacheNextStateMinInfo(
					false,
					(	(	bButtonUpPressedNext != bButtonUpPressedPrev
						||	bButtonDownPressedNext != bButtonDownPressedPrev
						)
						&&	( bButtonUpPressedNext || bButtonDownPressedNext )
					)
					? __EAPT_BY_PRESSED_STATE_TURNED_ON : __EAPT_BY_PRESSED_STATE_TURNED_OFF
					);
		}

		m_bButtonUpHovered = bButtonUpHoveredNext;
		m_bButtonDownHovered = bButtonDownHoveredNext;
		m_bButtonUpPressed = bButtonUpPressedNext;
		m_bButtonDownPressed = bButtonDownPressedNext;

		if( ! bAnimationLocked )
		{
 			AnimationClient_CacheNextStateMinInfo(
				true,
					(	(	bButtonUpPressedNext != bButtonUpPressedPrev
						||	bButtonDownPressedNext != bButtonDownPressedPrev
						)
						&&	( bButtonUpPressedNext || bButtonDownPressedNext )
					)
					? __EAPT_BY_PRESSED_STATE_TURNED_ON : __EAPT_BY_PRESSED_STATE_TURNED_OFF
				);
			AnimationClient_CacheGeneratorUnlock();
		}

		Invalidate();
	}

	return true;
}

bool CExtSpinWnd::_ProcessMouseMove( CPoint point )
{
	ASSERT_VALID( this );
	if(		CExtPopupMenuWnd::IsMenuTracking() 
		||	(! CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND( GetSafeHwnd() ) )
		)
		return false;
	if( m_bButtonUpPressed || m_bButtonDownPressed )
	{
		bool bMouseButtonsNotSwapped = ( ::GetSystemMetrics( SM_SWAPBUTTON ) != 0 ) ? false : true;
		int nMouseKey = bMouseButtonsNotSwapped ? VK_LBUTTON : VK_RBUTTON;
		bool bReallyPressed = CExtPopupMenuWnd::IsKeyPressed( nMouseKey ) ? true : false;
		if( ! bReallyPressed )
		{
			SendMessage( WM_CANCELMODE );
			return false;
		}
	}
CRect rcButtonUp, rcButtonDown;
	GetButtonRect( rcButtonUp, rcButtonDown );
bool bButtonUpHoveredPrev   = m_bButtonUpHovered;
bool bButtonDownHoveredPrev = m_bButtonDownHovered;
bool bButtonUpHoveredNext   = false;
bool bButtonDownHoveredNext = false;
	if( ( ! rcButtonUp.IsRectEmpty() ) && rcButtonUp.PtInRect( point ) )
		bButtonUpHoveredNext = true;
	if( ( ! rcButtonDown.IsRectEmpty() ) && rcButtonDown.PtInRect( point ) )
		bButtonDownHoveredNext = true;
	if( bButtonUpHoveredNext || bButtonDownHoveredNext )
	{
		if( ::GetCapture() != GetSafeHwnd() )
			::SetCapture( GetSafeHwnd() );
	}
	else if( ( ! m_bButtonUpPressed ) && ( ! m_bButtonDownPressed) )
	{
		if( ::GetCapture() == GetSafeHwnd() )
			::ReleaseCapture();
	}
	if( bButtonUpHoveredNext != bButtonUpHoveredPrev || bButtonDownHoveredNext != bButtonDownHoveredPrev )
	{
		bool bAnimationLocked = AnimationClient_CacheGeneratorIsLocked();
		if( ! bAnimationLocked )
		{
			AnimationClient_CacheGeneratorLock();
 			AnimationClient_CacheNextStateMinInfo(
				false,
				( bButtonUpHoveredNext || bButtonDownHoveredNext ) ? __EAPT_BY_HOVERED_STATE_TURNED_ON : __EAPT_BY_HOVERED_STATE_TURNED_OFF
				);
		}
		m_bButtonUpHovered = bButtonUpHoveredNext;
		m_bButtonDownHovered = bButtonDownHoveredNext;
		if( ! bAnimationLocked )
		{
 			AnimationClient_CacheNextStateMinInfo(
				true,
				( bButtonUpHoveredNext || bButtonDownHoveredNext ) ? __EAPT_BY_HOVERED_STATE_TURNED_ON : __EAPT_BY_HOVERED_STATE_TURNED_OFF
				);
			AnimationClient_CacheGeneratorUnlock();
		}
		Invalidate();
	}
	return true;
}

void CExtSpinWnd::OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp ) 
{
	bCalcValidRects;
CRect & rcNcRect = reinterpret_cast < CRect & > ( lpncsp->rgrc[0] );
	if( _IsBuddyToEdit() )
	{
		DWORD dwStyle = GetStyle();
		bool bSpinAlignRight = ( (dwStyle&UDS_ALIGNRIGHT) != 0 ) ? true : false;
		bool bSpinAlignLeft = ( (dwStyle&UDS_ALIGNLEFT) != 0 ) ? true : false;
		bool bHasScrolBars = false;
		CWnd * pWnd = this->GetWindow( GW_HWNDPREV );
		if( pWnd != NULL )
		{
			ASSERT_VALID( pWnd );
			DWORD dwStyle = pWnd->GetStyle();
			if(		(dwStyle&WS_VSCROLL) != 0
				||	(dwStyle&WS_HSCROLL) != 0
				)
				bHasScrolBars = true;
		}
	
CRect rcNcAreaMargins = 
		PmBridge_GetPM()->Spin_QueryNcAreaMargins(
			bSpinAlignRight,
			bSpinAlignLeft,
			bHasScrolBars
			);

 		rcNcRect.DeflateRect( rcNcAreaMargins );

		__EXT_MFC_LONG_PTR dwExStyle = ::__EXT_MFC_GetWindowLong( GetSafeHwnd(), GWL_EXSTYLE );
		bool bRTL = ( (dwExStyle & WS_EX_LAYOUTRTL) != 0 ) ? true : false;

		if( bRTL )
			rcNcRect.OffsetRect( 3, 0 );
	}
	m_bNcCalcSize = true;
}

void CExtSpinWnd::OnNcPaint() 
{
	if( _IsBuddyToEdit() )
	{
		CWnd * pWnd = this->GetWindow( GW_HWNDPREV );
		if( pWnd != NULL )
		{
			ASSERT_VALID( pWnd );
			pWnd->SendMessage( WM_NCPAINT );
		}
	}
}

void CExtSpinWnd::OnSize(UINT nType, int cx, int cy) 
{
	AnimationSite_ClientProgressStop( this );
	AnimationClient_StateGet( false ).Empty();
	AnimationClient_StateGet( true ).Empty();
CRect rectButton;
	GetClientRect( &rectButton );
	AnimationClient_TargetRectSet( rectButton );

	CSpinButtonCtrl::OnSize( nType, cx, cy );
}

void CExtSpinWnd::PmBridge_OnPaintManagerChanged(
	CExtPaintManager * pGlobalPM
	)
{
	m_bNcCalcSize = false;
	if( GetSafeHwnd() != NULL )
		SetWindowPos(
			NULL, 0, 0, 0, 0,
			SWP_NOMOVE|SWP_NOSIZE 
			|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_FRAMECHANGED
			);
	CExtAnimationSingleton::PmBridge_OnPaintManagerChanged( pGlobalPM );
}

HWND CExtSpinWnd::AnimationSite_GetSafeHWND() const
{
__PROF_UIS_MANAGE_STATE;
HWND hWnd = GetSafeHwnd();
	return hWnd;
}

const CExtAnimationParameters *
	CExtSpinWnd::AnimationClient_OnQueryAnimationParameters(
		INT eAPT // __EAPT_*** animation type
		) const
{
	ASSERT_VALID( this );
const CExtAnimationParameters * pAnimationParameters =
		PmBridge_GetPM()->Animation_GetParameters(
			eAPT,
			(CObject*)this,
			this
			);
	return pAnimationParameters;
}

bool CExtSpinWnd::AnimationClient_CacheNextState(
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

	OnSpinDrawEntire( dc, &rcAcAnimationTarget );

	return
		CExtAnimationSingleton::AnimationClient_CacheNextState(
			dc,
			rcAcAnimationTarget,
			bAnimate,
			eAPT
			);
}

#endif // (!defined __EXT_MFC_NO_SPIN)


