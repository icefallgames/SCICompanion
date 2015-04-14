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

#if (!defined __EXT_MFC_NO_SLIDER)

#if (!defined __EXT_SLIDER_H)
	#include <ExtSliderWnd.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_TOOLCONTROLBAR_H)
	#include <ExtToolControlBar.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtSliderWnd

IMPLEMENT_DYNCREATE( CExtSliderWnd, CSliderCtrl );		
IMPLEMENT_CExtPmBridge_MEMBERS( CExtSliderWnd );

CExtSliderWnd::CExtSliderWnd()
	: m_eStyle( ES_SYSTEM )
	, m_clrBackground( COLORREF(-1L) )
	, m_clrBackgroundPrev( COLORREF(-1L) )
	, m_nAdvancedTipStyle( INT(CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON) )
	, m_bToolTipActive( false )
{
	PmBridge_Install();
	EnableToolTips();
}

CExtSliderWnd::~CExtSliderWnd()
{
	PmBridge_Uninstall();

	if( m_brBack.GetSafeHandle() != NULL )
		m_brBack.DeleteObject();
}

BEGIN_MESSAGE_MAP(CExtSliderWnd, CSliderCtrl)
	//{{AFX_MSG_MAP(CExtSliderWnd)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomdraw)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
	ON_WM_SYSCOLORCHANGE()
	__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_MESSAGE(__ExtMfc_WM_THEMECHANGED, OnThemeChanged)
END_MESSAGE_MAP()

COLORREF CExtSliderWnd::SetBkColor( COLORREF clrBk )
{ 
	ASSERT_VALID( this );
	COLORREF clrBkOld = m_clrBackground;
	m_clrBackground = clrBk; 
	if( GetSafeHwnd() != NULL )
		UpdateSliderWnd();
	return clrBkOld;
}

COLORREF CExtSliderWnd::GetBkColor() const
{ 
	ASSERT_VALID( this );
	return m_clrBackground; 
}

void CExtSliderWnd::SetStyle( e_style_t eStyle )
{
	ASSERT_VALID( this );
	m_eStyle = eStyle;
	if( GetSafeHwnd() != NULL )
		UpdateSliderWnd();
}

CExtSliderWnd::e_style_t CExtSliderWnd::GetStyle() const
{
	ASSERT_VALID( this );
	return m_eStyle;
}

/////////////////////////////////////////////////////////////////////////////
// CExtSliderWnd message handlers

BOOL CExtSliderWnd::OnEraseBkgnd(CDC* pDC) 
{
	ASSERT_VALID( this );
	pDC;
	return FALSE;
}

HBRUSH CExtSliderWnd::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	ASSERT_VALID( this );
	if( nCtlColor == CTLCOLOR_STATIC )
	{
		COLORREF clrBk = GetBkColor();
	
		CRect rcClient;
		GetClientRect( &rcClient );
		
		CBitmap bmpGradient;
		if( bmpGradient.CreateCompatibleBitmap( 
				pDC, 
				rcClient.Width(), 
				rcClient.Height() 
				) 
			)
		{
			CBitmap * pBmpOld = pDC->SelectObject( &bmpGradient );
			bool bTransparent = false;
			if(		PmBridge_GetPM()->GetCb2DbTransparentMode( this )
				&&	( clrBk == COLORREF(-1L) )
				)
			{
				CExtPaintManager::stat_ExcludeChildAreas(
					*pDC,
					GetSafeHwnd(),
					CExtPaintManager::stat_DefExcludeChildAreaCallback
					);
				if( PmBridge_GetPM()->PaintDockerBkgnd( true, *pDC, this ) )
					bTransparent = true;
			}
			pDC->SelectObject( pBmpOld );
			if( bTransparent )
			{
				if( m_brBack.GetSafeHandle() != NULL )
					m_brBack.DeleteObject();
				if( m_brBack.CreatePatternBrush( &bmpGradient ) )
				{
					m_clrBackgroundPrev = NULL;
					return m_brBack;
				}
			}
		}

		if( clrBk == COLORREF(-1L) )
			clrBk = PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT, this );

		if( m_clrBackgroundPrev != clrBk )
		{
			if( m_brBack.GetSafeHandle() != NULL )
				m_brBack.DeleteObject();
			m_brBack.CreateSolidBrush( clrBk );
			m_clrBackgroundPrev = clrBk;
		}
		return m_brBack;
	}
	else
		return (HBRUSH)Default();
}

LRESULT CExtSliderWnd::WindowProc( UINT message, WPARAM wParam, LPARAM lParam ) 
{
#if (defined WM_UPDATEUISTATE)	
	ASSERT( WM_UPDATEUISTATE == 0x0128 );
#endif
	// WM_UPDATEUISTATE causes repaint without WM_PAINT, so we eat it
	if( message == 0x0128 )
		return 0;
	if(		message == WM_SETTEXT 
		||	message == WM_ENABLE
		)
	{
		LRESULT lResult = CSliderCtrl::WindowProc( message, wParam, lParam );
		Invalidate();
		UpdateWindow();
		return lResult;
	}
	else if( message == WM_MOUSEMOVE )
	{
		if(		m_nAdvancedTipStyle != INT(CExtPopupMenuTipWnd::__ETS_NONE)
			&&	m_bToolTipActive
			&&	( ! m_strToolTipText.IsEmpty() )
			&&	(! CExtPopupMenuWnd::IsMenuTracking() )
			)
		{
			CExtPopupMenuTipWnd * pATTW =
				OnAdvancedPopupMenuTipWndGet();
			if( pATTW != NULL )
			{
				CRect rcArea;
				GetWindowRect( &rcArea );
				OnAdvancedPopupMenuTipWndDisplay(
					*pATTW,
					rcArea
					);
			} // if( pATTW != NULL )
		}
	}

	if(		OnAdvancedPopupMenuTipWndGet() == NULL
		&&	message == WM_NOTIFY
		&&	m_wndToolTip.GetSafeHwnd() != NULL
		&&	((LPNMHDR)lParam) != NULL
		&&	((LPNMHDR)lParam)->hwndFrom == m_wndToolTip.GetSafeHwnd()
		&&	((LPNMHDR)lParam)->code == TTN_SHOW
		)
		::SetWindowPos(
			m_wndToolTip.GetSafeHwnd(),
			HWND_TOP,
			0,0,0,0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE
			);

	return CSliderCtrl::WindowProc (message, wParam, lParam );
}

BOOL CExtSliderWnd::PreTranslateMessage( MSG * pMsg )
{
	if(		(! CExtPopupMenuWnd::IsMenuTracking() )
		&&	OnAdvancedPopupMenuTipWndGet() == NULL
		)
	{
		InitToolTip();
		m_wndToolTip.RelayEvent( pMsg );
	}
	return CSliderCtrl::PreTranslateMessage( pMsg );
}

void CExtSliderWnd::SetTooltipText(
	int nId,
	BOOL bActivate // = TRUE
	)
{
CExtSafeString sText;
	g_ResourceManager->LoadString( sText, nId );
	SetTooltipText( LPCTSTR(sText), bActivate );
}

void CExtSliderWnd::SetTooltipText(
	__EXT_MFC_SAFE_LPCTSTR sText,
	BOOL bActivate // = TRUE
	)
{
	if( sText == NULL )
		sText = _T("");
	CWnd::CancelToolTips();
CExtPopupMenuTipWnd * pATTW = OnAdvancedPopupMenuTipWndGet();
	m_strToolTipText = sText;
	if( pATTW != NULL )
	{
		//pATTW->SetText( sText );
		m_bToolTipActive = bActivate ? true : false;
	} // if( pATTW != NULL )
	else
	{
		InitToolTip();
		if( m_wndToolTip.GetToolCount() == 0 )
		{
			CRect rectBtn; 
			GetClientRect(rectBtn);
			m_wndToolTip.AddTool(this,sText,rectBtn,1);
		}
		m_wndToolTip.UpdateTipText(sText,this,1);
		m_wndToolTip.SetMaxTipWidth( -1 );
		CString strText( sText );
		if( strText.Find( _T("\r") ) >= 0 )
			m_wndToolTip.SetMaxTipWidth( OnQueryMaxTipWidth( sText ) );
		m_wndToolTip.Activate( bActivate );
	} // else from if( pATTW != NULL )
}

int CExtSliderWnd::OnQueryMaxTipWidth( 
	__EXT_MFC_SAFE_LPCTSTR lpszText 
	)
{
	lpszText;
	return 250;
}

void CExtSliderWnd::ActivateTooltip( BOOL bActivate )
{
	m_bToolTipActive = bActivate ? true : false;
	if( OnAdvancedPopupMenuTipWndGet() != NULL )
		return;
	if( m_wndToolTip.GetToolCount() == 0 )
		return;
	m_wndToolTip.Activate(bActivate);
}

CExtPopupMenuTipWnd * CExtSliderWnd::OnAdvancedPopupMenuTipWndGet() const
{
	if( m_nAdvancedTipStyle == INT(CExtPopupMenuTipWnd::__ETS_NONE) )
		return NULL;
	return (&( CExtPopupMenuSite::g_DefPopupMenuSite.GetTip() ));
}

void CExtSliderWnd::OnAdvancedPopupMenuTipWndDisplay(
	CExtPopupMenuTipWnd & _ATTW,
	const RECT & rcExcludeArea
	) const
{
	ASSERT_VALID( this );
	if( ! m_bToolTipActive )
		return;
	if( m_strToolTipText.IsEmpty() )
		return;
	_ATTW.SetText( LPCTSTR(m_strToolTipText) );
	_ATTW.SetTipStyle( (CExtPopupMenuTipWnd::e_tip_style_t)m_nAdvancedTipStyle );
	_ATTW.Show( (CWnd*)this, rcExcludeArea );
}

__EXT_MFC_INT_PTR CExtSliderWnd::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	) const
{
	__PROF_UIS_MANAGE_STATE;
	__EXT_MFC_IMPLEMENT_TT_REDIR_OnToolHitTest( CExtSliderWnd, CExtToolControlBar );
	return CSliderCtrl::OnToolHitTest( point, pTI );
}

void CExtSliderWnd::InitToolTip()
{
	//if( OnAdvancedPopupMenuTipWndGet() != NULL )
	//	return;
	if( m_wndToolTip.m_hWnd == NULL )
	{
		m_wndToolTip.Create(this);
		m_wndToolTip.Activate(FALSE);
	}
}

bool CExtSliderWnd::OnSliderDrawChannel(
	CDC & dc,
	const CRect & rcChannel,
	UINT uItemState
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	uItemState;
	if( ! dc.RectVisible( &rcChannel ) )
		return false;

	if( m_eStyle != ES_PROFUIS )
		return false;

	DWORD dwStyle = CWnd::GetStyle();
	bool bEnabled = 
		IsWindowEnabled() 
			? true
			: false;
	bool bHorz = 
		(dwStyle&TBS_VERT) != 0 
			? false
			: true;
	return 
		PmBridge_GetPM()->PaintSliderChannel( 
			dc, 
			rcChannel,
			bHorz,
			bEnabled,
			this
			);
}

bool CExtSliderWnd::OnSliderDrawThumb(
	CDC & dc,
	const CRect & rcThumb,
	UINT uItemState
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible( &rcThumb ) )
		return false;

	if( m_eStyle != ES_PROFUIS )
		return false;

	DWORD dwStyle = CWnd::GetStyle();
	if( (dwStyle&TBS_NOTHUMB) != NULL )
		return false;

	bool bHorz = 
		(dwStyle&TBS_VERT) != 0 
			? false
			: true;
	bool bLeft = 
		(dwStyle&TBS_LEFT) != 0 
			? true
			: false;
	bool bRight = 
		(dwStyle&TBS_LEFT) == 0 
			? true
			: false;	
	if( (dwStyle&TBS_BOTH) != 0 )
	{
		bLeft = true;
		bRight = true;
	}
	bool bPushed = 
		(uItemState&CDIS_SELECTED) != 0 
			? true
			: false;
// 	bool bFocused = 
// 		(uItemState&CDIS_FOCUS) != 0 
// 			? true
// 			: false;
	bool bFocused = 
		( GetFocus() == this )
			? true
			: false;
	bool bEnabled = 
		IsWindowEnabled() 
			? true
			: false;

	CPoint ptCursor( 0, 0 );
	::GetCursorPos( &ptCursor );
	ScreenToClient( &ptCursor );
	bool bHover = 
		rcThumb.PtInRect( ptCursor )
			? true
			: false;
	return 
		PmBridge_GetPM()->PaintSliderThumb( 
			dc, 
			rcThumb,
			bHorz,
			bLeft,
			bRight,
			bEnabled,
			bPushed,
			bFocused,
			bHover,
			this
			);
}

bool CExtSliderWnd::OnSliderDrawTics(
	CDC & dc,
	const CRect & rcClient,
	UINT uItemState
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	uItemState;
	if( ! dc.RectVisible( &rcClient ) )
		return false;

	DWORD dwStyle = CWnd::GetStyle();
	if( (dwStyle&TBS_NOTICKS) != 0 )
		return false;

	bool bEnabled = 
		IsWindowEnabled() 
			? true
			: false;
	bool bHorz = 
		(dwStyle&TBS_VERT) != 0 
			? false
			: true;
	return 
		PmBridge_GetPM()->PaintSliderTics( 
			dc, 
			rcClient,
			bHorz,
			bEnabled,
			this
			);
}

void CExtSliderWnd::OnCustomdraw( NMHDR * pNMHDR, LRESULT * pResult ) 
{
	LPNMCUSTOMDRAW lpNMCustomDraw = (LPNMCUSTOMDRAW)pNMHDR;
	CDC * pDC = CDC::FromHandle( lpNMCustomDraw->hdc );
	CRect rcPaint( lpNMCustomDraw->rc );

	*pResult = CDRF_DODEFAULT;

	switch( lpNMCustomDraw->dwDrawStage ) 
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:  // Before an item is drawn. This is where we perform our item-specific custom drawing
        if( lpNMCustomDraw->dwItemSpec == TBCD_THUMB )
        {
			if( OnSliderDrawThumb(
					*pDC,
					rcPaint,
					lpNMCustomDraw->uItemState
					) 
				)
				*pResult = CDRF_SKIPDEFAULT;
        } 
		else if( lpNMCustomDraw->dwItemSpec == TBCD_CHANNEL )
        {
			if( OnSliderDrawChannel(
					*pDC,
					rcPaint,
					lpNMCustomDraw->uItemState
					) 
				)
				*pResult = CDRF_SKIPDEFAULT;
        }
 		else if( lpNMCustomDraw->dwItemSpec == TBCD_TICS )
        {
			CRect rcClient;
			GetClientRect( &rcClient );
			if( OnSliderDrawTics(
					*pDC,
					rcClient,
					lpNMCustomDraw->uItemState
					) 
				)
				*pResult = CDRF_SKIPDEFAULT;
        }
		else
		{
			ASSERT( FALSE );
		}
		break;
	} // switch
}

void CExtSliderWnd::OnSysColorChange() 
{
	ASSERT_VALID( this );
	CSliderCtrl::OnSysColorChange();
//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnSysColorChange( this );
//	g_CmdManager.OnSysColorChange( pPM, this );
	UpdateSliderWnd();
}

LRESULT CExtSliderWnd::OnDisplayChange( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
//LRESULT lResult = CSliderCtrl::OnDisplayChange( wParam, lParam );

CSliderCtrl::OnDisplayChange(wParam, LOWORD(lParam), HIWORD(lParam));

LRESULT lResult = 0;


//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnDisplayChange( this, (INT)wParam, CPoint(lParam) );
//	g_CmdManager.OnDisplayChange( pPM, this, (INT)wParam, CPoint(lParam) );
	UpdateSliderWnd();
	return lResult;
}

LRESULT CExtSliderWnd::OnThemeChanged( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	wParam;
	lParam;
LRESULT lResult = Default();
//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnThemeChanged( this, wParam, lParam );
//	g_CmdManager.OnThemeChanged( pPM, this, wParam, lParam );
	return lResult;
}

void CExtSliderWnd::OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection) 
{
	ASSERT_VALID( this );
	CSliderCtrl::OnSettingChange(uFlags, lpszSection);
//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnSettingChange( this, uFlags, lpszSection );
//	g_CmdManager.OnSettingChange( pPM, this, uFlags, lpszSection );
	UpdateSliderWnd();
}

void CExtSliderWnd::PmBridge_OnPaintManagerChanged(
	CExtPaintManager * pGlobalPM
	)
{
	CExtPmBridge::PmBridge_OnPaintManagerChanged( pGlobalPM );
	UpdateSliderWnd();
}

void CExtSliderWnd::UpdateSliderWnd()
{
	ASSERT_VALID( this );
HWND hWndOwn = GetSafeHwnd();
	if( hWndOwn == NULL )
		return;
HWND hWndParent = ::GetParent( hWndOwn );
	if( hWndParent != NULL )
	{
		bool bDisabled = false;
		if( ! ::IsWindowEnabled( hWndOwn ) )
		{
			bDisabled = true;
			EnableWindow( TRUE );
		}
		HWND hWndFocus = ::GetFocus();
		if( hWndFocus == hWndOwn )
		{
			::SetFocus( hWndParent );
			::SetFocus( hWndFocus );
		}
		else
		{
			::SetFocus( hWndOwn );
			::SetFocus( hWndFocus );
		}
		if( bDisabled )
			EnableWindow( FALSE );
	}
	Invalidate();
	UpdateWindow();
}

#endif // (!defined __EXT_MFC_NO_SLIDER)
