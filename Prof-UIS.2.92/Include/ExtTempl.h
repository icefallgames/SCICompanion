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

#if (!defined __EXT_TEMPL_H)
#define __EXT_TEMPL_H

#if (!defined __EXT_MFC_DEF_H)
	#include <ExtMfcDef.h>
#endif // __EXT_MFC_DEF_H

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_TOOLCONTROLBAR_H)
	#include <ExtToolControlBar.h>
#endif

#if (!defined __EXT_CONTROLS_H)
	#include <ExtControls.h>
#endif

#if (!defined _AFX_NO_OCC_SUPPORT )

	#if (!defined __PROF_UIS_OCC_IMPL_INCLUDED )
		#define __PROF_UIS_OCC_IMPL_INCLUDED

		#if _MFC_VER < 0x700
			#if (!defined __PROF_UIS_AUTOMATION_PACK_IMPL__) && (!defined __PROF_SKIN_IMPL__)
				#include <../Src/occimpl.h>
				#define __EXT_TEMPL_OCC_SUPPORT_IMPLEMENTED
			#endif
		#else
			#include <ocdb.h>
			#include <afxocc.h>
			#include <../src/mfc/occimpl.h>
			#define __EXT_TEMPL_OCC_SUPPORT_IMPLEMENTED
		#endif

	#endif // (!defined __PROF_UIS_OCC_IMPL_INCLUDED )

#endif // (!defined _AFX_NO_OCC_SUPPORT )

/////////////////////////////////////////////////////////////////////////////
// CExtWFF template window extension (flicker free window)
//
// removes flicker effect caused by WM_ERASEBKGND or
// non-buffered painting, useful for new win32 (not old win16)
// common controls such as list-view or tree-view
//

template < class CExtWFFBase = CWnd, bool _bExludeChildAreas = true, bool _bEat_WM_ERASEBKGND = true, bool _bEmulate_WM_ERASEBKGND = true >
class CExtWFF : public CExtWFFBase
{
protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
	{
		switch( message )
		{
		case WM_ERASEBKGND:
			{
				bool bValEat_WM_ERASEBKGND = _bEat_WM_ERASEBKGND;
				if( bValEat_WM_ERASEBKGND )
					return FALSE;
			}
		break;
		case WM_PAINT:
			{
				CPaintDC dcPaint( this );
				bool bValExludeChildAreas = _bExludeChildAreas;
				if( bValExludeChildAreas )
					CExtPaintManager::stat_ExcludeChildAreas( dcPaint.GetSafeHdc(), GetSafeHwnd() );
				CRect rcClient;
				GetClientRect( &rcClient );
				CExtMemoryDC dc( &dcPaint, &rcClient );
				bool bValEmulate_WM_ERASEBKGND = _bEmulate_WM_ERASEBKGND;
				if( bValEmulate_WM_ERASEBKGND )
					DefWindowProc( WM_ERASEBKGND, WPARAM( dc.GetSafeHdc() ), LPARAM(0) );
				DefWindowProc( WM_PAINT, WPARAM( dc.GetSafeHdc() ), LPARAM(0) );
			}
			return TRUE;
		}
		return CExtWFFBase::WindowProc( message, wParam, lParam );
	}
}; // class CExtWFF

/////////////////////////////////////////////////////////////////////////////
// CExtWRB template window extension (window in resizable bar)
//
// adds thin non-client area border according to paint manager;
// useful for windows inserted into resizable control bar
//

template < class CExtWRBBase = CWnd >
class CExtWRB : public CExtWRBBase
{
public:
	virtual CExtPaintManager * PmBridge_GetPM() const
	{
		ASSERT_VALID( this );
		return g_PaintManager.GetPM();
	}
protected:
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
	{
		switch( message )
		{
		case WM_NCCALCSIZE:
		{
			NCCALCSIZE_PARAMS * pNCCSP = reinterpret_cast < NCCALCSIZE_PARAMS * > ( lParam );
			ASSERT( pNCCSP != NULL );
			CRect rcInBarWnd( pNCCSP->rgrc[0] );
			rcInBarWnd.DeflateRect( 1, 1 );
			::CopyRect( &(pNCCSP->rgrc[0]), rcInBarWnd );
		} // case WM_NCCALCSIZE
		return 0L;
		case WM_NCPAINT:
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
			PmBridge_GetPM()->PaintResizableBarChildNcAreaRect( dc, rcInBarWnd, this );
		} // case WM_NCPAINT
		return 0L;
		} // switch( message )
		return CExtWRBBase::WindowProc( message, wParam, lParam );
	}
}; // class CExtWRB

/////////////////////////////////////////////////////////////////////////////
// CExtWS template window extension (styled window)
//
// adds current PaintManager's background style,
// load/save window position feature,
// right-bottom resizing gripper (default is off)
// and cool system menu for WS_POPUP windows
//

#ifdef GET_WM_CTLCOLOR_HDC
	#define __EXT_GET_WM_CTLCOLOR_HDC(wp,lp,msg) GET_WM_CTLCOLOR_HDC(wp,lp,msg)
#else
	#define __EXT_GET_WM_CTLCOLOR_HDC(wp,lp,msg) (HDC)(wp)
#endif
#ifdef GET_WM_CTLCOLOR_HWND
	#define __EXT_GET_WM_CTLCOLOR_HWND(wp,lp,msg) GET_WM_CTLCOLOR_HWND(wp,lp,msg)
#else
	#define __EXT_GET_WM_CTLCOLOR_HWND(wp,lp,msg) (HWND)(lp)
#endif
#ifdef GET_WM_CTLCOLOR_TYPE
	#define __EXT_GET_WM_CTLCOLOR_TYPE(wp,lp,msg) GET_WM_CTLCOLOR_TYPE(wp,lp,msg)
#else
	#define __EXT_GET_WM_CTLCOLOR_TYPE(wp,lp,msg) (WORD)(msg - WM_CTLCOLORMSGBOX)
#endif

template < class CExtWSBase, UINT nIdTimerSysMenuTracking = UINT(301) >
class CExtWS : public CExtWSBase
{
protected:
	CExtSafeString m_sSection; // section name and
	CExtSafeString m_sEntry;   // entry for save/restore
	bool m_bStyledWndInitDone:1, m_bEnableSaveRestore:1, m_bSysMenuTracking:1, m_bShowResizingGripper:1, m_bAutoSubclassChildControls:1, m_bGripperRTL:1;
	COLORREF m_clrBackground;
public:
	CExtWSGripper m_wndGripper;
	CExtWS()
		: m_bStyledWndInitDone( false )
		, m_bSysMenuTracking( false )
		, m_bEnableSaveRestore( false )
		, m_bShowResizingGripper( false )
		, m_bAutoSubclassChildControls( false )
		, m_bGripperRTL( false )
		, m_clrBackground( COLORREF(-1L) )	
		{ }
	CExtWS( UINT nIDTemplate, CWnd * pParentWnd )
		: CExtWSBase( nIDTemplate, pParentWnd )
		, m_bStyledWndInitDone( false )
		, m_bSysMenuTracking( false )
		, m_bEnableSaveRestore( false )
		, m_bShowResizingGripper( false )
		, m_bAutoSubclassChildControls( false )
		, m_bGripperRTL( false )
		, m_clrBackground( COLORREF(-1L) )	
		{ }
	CExtWS( __EXT_MFC_SAFE_LPCTSTR lpszTemplateName, CWnd * pParentWnd )
		: CExtWSBase( lpszTemplateName, pParentWnd )
		, m_bStyledWndInitDone( false )
		, m_bSysMenuTracking( false )
		, m_bEnableSaveRestore( false )
		, m_bShowResizingGripper( false )
		, m_bAutoSubclassChildControls( false )
		, m_bGripperRTL( false )
		, m_clrBackground( COLORREF(-1L) )	
		{ }
	CExtWS( UINT nIDTemplate, UINT nIDCaption = 0 )
		: CExtWSBase( nIDTemplate, nIDCaption )
		, m_bStyledWndInitDone( false )
		, m_bSysMenuTracking( false )
		, m_bEnableSaveRestore( false )
		, m_bShowResizingGripper( false )
		, m_bAutoSubclassChildControls( false )
		, m_bGripperRTL( false )
		, m_clrBackground( COLORREF(-1L) )	
		{ }
	CExtWS( __EXT_MFC_SAFE_LPCTSTR lpszTemplateName, UINT nIDCaption = 0 )
		: CExtWSBase( lpszTemplateName, nIDCaption )
		, m_bStyledWndInitDone( false )
		, m_bSysMenuTracking( false )
		, m_bEnableSaveRestore( false )
		, m_bShowResizingGripper( false )
		, m_bAutoSubclassChildControls( false )
		, m_bGripperRTL( false )
		, m_clrBackground( COLORREF(-1L) )	
		{ }
	CExtWS( UINT nIDCaption, CWnd *pParentWnd, UINT iSelectPage )
		: CExtWSBase( nIDCaption, pParentWnd, iSelectPage )
		, m_bStyledWndInitDone( false )
		, m_bSysMenuTracking( false )
		, m_bEnableSaveRestore( false )
		, m_bShowResizingGripper( false )
		, m_bAutoSubclassChildControls( false )
		, m_bGripperRTL( false )
		, m_clrBackground( COLORREF(-1L) )	
		{ }
	CExtWS( __EXT_MFC_SAFE_LPCTSTR pszCaption, CWnd *pParentWnd, UINT iSelectPage )
		: CExtWSBase( pszCaption, pParentWnd, iSelectPage )
		, m_bStyledWndInitDone( false )
		, m_bSysMenuTracking( false )
		, m_bEnableSaveRestore( false )
		, m_bShowResizingGripper( false )
		, m_bAutoSubclassChildControls( false )
		, m_bGripperRTL( false )
		, m_clrBackground( COLORREF(-1L) )	
	{ }
	virtual CExtPaintManager * PmBridge_GetPM() const
		{ ASSERT_VALID( this ); return g_PaintManager.GetPM(); }
	COLORREF SetBkColor( COLORREF clrBk )
	{
		COLORREF clrBkOld = m_clrBackground;
		m_clrBackground = clrBk; 
		if( GetSafeHwnd() != NULL )
		{
			Invalidate();
			UpdateWindow();
		}	
		return clrBkOld;
	}
	COLORREF GetBkColor() const
		{ return m_clrBackground; }
	void SetAutoSubclassChildControls( bool bSet = true )
		{ m_bAutoSubclassChildControls = bSet; }
	bool GetAutoSubclassChildControls() const
		{ return m_bAutoSubclassChildControls; }
	void SubclassChildControls()
		{ if( GetSafeHwnd() != NULL ) ::SubclassChildControls( m_hWnd ); }
	virtual void ShowSizeGrip( BOOL bShow = TRUE )
	{
		bool _bShow = bShow ? true : false;
		if( m_bShowResizingGripper == _bShow )
			return;
		m_bShowResizingGripper = _bShow;
		if( m_wndGripper.GetSafeHwnd() == NULL )
			return;
		ASSERT( GetSafeHwnd() != NULL );
		m_wndGripper.ShowWindow( m_bShowResizingGripper ? SW_SHOW : SW_HIDE  );
		if( m_bShowResizingGripper )
			return;
		CRect rcGripper;			
		m_wndGripper.GetWindowRect( &rcGripper );
		ScreenToClient( &rcGripper );
		InvalidateRect( &rcGripper );
	}
	virtual void SaveWindowRect() const
		{ VERIFY( CExtControlBar::stat_SaveWindowRect( GetSafeHwnd(), __EXT_MFC_SAFE_LPCTSTR(m_sSection), __EXT_MFC_SAFE_LPCTSTR(m_sEntry) ) ); }
	virtual void LoadWindowRect( bool bForceHideWindow = false )
		{ CExtControlBar::stat_LoadWindowRect( GetSafeHwnd(), __EXT_MFC_SAFE_LPCTSTR(m_sSection), __EXT_MFC_SAFE_LPCTSTR(m_sEntry), bForceHideWindow ); }
	virtual void EnableSaveRestore( __EXT_MFC_SAFE_LPCTSTR pszSection, __EXT_MFC_SAFE_LPCTSTR pszEntry, bool bForceHideWindow = false )
		{ m_sSection = pszSection; m_sEntry = pszEntry; m_bEnableSaveRestore = true; LoadWindowRect( bForceHideWindow ); }
	virtual void DisableSaveRestore()
		{ m_bEnableSaveRestore = false; }
	virtual BOOL _TrackWndSystemPopupMenu( CPoint * pPoint, BOOL bSelectAny )
	{
		if( (GetStyle() & WS_POPUP) == 0 )
			return FALSE;
		if(	CExtPopupMenuWnd::IsMenuTracking() )
			CExtPopupMenuWnd::CancelMenuTracking();
		m_bSysMenuTracking = false;
		CMenu * pSysMenu = GetSystemMenu( FALSE );
		if( pSysMenu == NULL )
			return FALSE;
		ASSERT( ::IsMenu(pSysMenu->GetSafeHmenu()) );
		CExtPopupMenuWnd * pPopup = CExtPopupMenuWnd::InstantiatePopupMenu( GetSafeHwnd(), RUNTIME_CLASS(CExtPopupMenuWnd), this );
		if( ! pPopup->CreatePopupMenu( GetSafeHwnd() ) )
		{
			ASSERT( FALSE );
			delete pPopup;
			return FALSE;
		}
		if( ! pPopup->UpdateFromMenu( GetSafeHwnd(), pSysMenu, false ) )
		{
			ASSERT( FALSE );
			delete pPopup;
			return FALSE;
		}
		__EXT_MFC_LONG_PTR dwExStyle = ::__EXT_MFC_GetWindowLong( GetSafeHwnd(), GWL_EXSTYLE );
		bool bRTL = ( (dwExStyle & WS_EX_LAYOUTRTL) != 0 ) ? true : false;
		CPoint point;
		if( pPoint != NULL)
			point = *pPoint;
		else
		{
			CRect rcWnd, rcClient, rcHelper;
			GetWindowRect( &rcWnd );
			GetClientRect( &rcClient );
			rcHelper = rcWnd;
			ScreenToClient( &rcHelper );
			ASSERT( rcHelper.top <= rcClient.top );
			int yDiff = rcClient.top - rcHelper.top;
			int n = 0;
			WINDOWPLACEMENT _wp;
			::memset( &_wp, 0, sizeof(WINDOWPLACEMENT) );
			_wp.length = sizeof(WINDOWPLACEMENT);
			VERIFY( CExtControlBar::stat_GetWindowPlacement( GetSafeHwnd(), _wp ) );
			if( _wp.showCmd != SW_SHOWMAXIMIZED ) 
				n = ::GetSystemMetrics( SM_CXFRAME );
			point = rcWnd.TopLeft();
			if( bRTL )
			{
				point = CPoint( rcWnd.right, rcWnd.top );
				n = -n;
			}
			point.x += n;
			point.y += yDiff;
		}
		DWORD dwTrackFlags = (bRTL ? TPMX_RIGHTALIGN : TPMX_TOPALIGN);
		if( bSelectAny )
			dwTrackFlags |= TPMX_SELECT_ANY;
		HWND hWndFocus = ::GetFocus();
		if(		hWndFocus != NULL
			&&	(!	(	hWndFocus == m_hWnd
					||	::IsChild( m_hWnd, hWndFocus )
				)	)
			)
			SetFocus();
		if( bRTL )
			point.x += pPopup->OnQueryMenuShadowSize();
		CRect rcExclude( point, point );
		m_bSysMenuTracking = pPopup->TrackPopupMenu( dwTrackFlags | TPMX_SYSMENU, point.x,point.y, rcExclude ) ? true : false;
		if( m_bSysMenuTracking )
			SetTimer( nIdTimerSysMenuTracking, 20, NULL );
		return m_bSysMenuTracking; //TRUE;
	}
	virtual BOOL PreTranslateMessage( MSG * pMsg )
	{
		if(		GetSafeHwnd() != NULL
			&&	(	pMsg->message == WM_SYSKEYDOWN
				||	pMsg->message == WM_SYSKEYUP
				||	pMsg->message == WM_KEYDOWN
				//||	pMsg->message == WM_KEYUP
				)
			)
		{
			BOOL bSuccessRetVal = TRUE;
			if( ( GetStyle() & WS_POPUP ) == 0 )
				bSuccessRetVal = FALSE;
			bool bKeyUp =
				(	//pMsg->message == WM_KEYUP ||
					pMsg->message == WM_SYSKEYUP )
						? true : false ;
			BOOL bAlt = HIWORD(pMsg->lParam) & KF_ALTDOWN;
			__EXT_MFC_SAFE_TCHAR vkTCHAR = (TCHAR)pMsg->wParam;
			if( bAlt && vkTCHAR == ((TCHAR)VK_SPACE) )
			{
				if( bKeyUp )
					return bSuccessRetVal;
				// allow child popup track it's system menu
				CWnd * pWnd = CWnd::GetActiveWindow();
				if( pWnd->GetSafeHwnd() == NULL )
					return bSuccessRetVal;
				if(		pWnd->m_hWnd != GetSafeHwnd()
					&&	(pWnd->GetStyle() & WS_POPUP) != 0
					)
					return bSuccessRetVal;
				// track dialog system menu
				if( ( GetStyle() & WS_POPUP ) != 0 && CExtWSBase::GetSystemMenu( FALSE ) != NULL )
					{ VERIFY( _TrackWndSystemPopupMenu( NULL, TRUE ) ); }
				return bSuccessRetVal;
			} // if( bAlt && vkTCHAR == ((TCHAR)VK_SPACE) )
		}
		return CExtWSBase::PreTranslateMessage( pMsg );
	}
protected:
	virtual void _InitSizeGripper()
	{
		if( GetSafeHwnd() == NULL )
			return;
		bool bHaveSizeGripper = false, bShowResizingGripper = false;
		if( m_wndGripper.GetSafeHwnd() != NULL && ::IsWindow( m_wndGripper.m_hWnd ) )
		{
			bHaveSizeGripper = true;
			bShowResizingGripper = ( ( m_wndGripper.GetStyle() & WS_VISIBLE ) != 0 ) ? true : false;
		}		
		CRect rcGripper;
		GetClientRect( &rcGripper );
		CSize szGripperSize = PmBridge_GetPM()->GetResizingGriperSize( this );
		rcGripper.left = rcGripper.right - szGripperSize.cx;
		rcGripper.top = rcGripper.bottom - szGripperSize.cy;
		WNDCLASS _wndClassInfo;
		::memset( &_wndClassInfo, 0, sizeof(WNDCLASS) );
		HINSTANCE hInst = ::AfxGetInstanceHandle();
		if( ! ::GetClassInfo( hInst, __EXT_SIZE_GRIPPER_CLASS_NAME, &_wndClassInfo ) )
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
			_wndClassInfo.lpszClassName = __EXT_SIZE_GRIPPER_CLASS_NAME;
			if( ! ::AfxRegisterClass( & _wndClassInfo ) )
				{ ASSERT( FALSE ); return; }
		}
		if( bHaveSizeGripper )
			m_wndGripper.SetWindowPos( NULL, rcGripper.left,  rcGripper.top, rcGripper.Width(), rcGripper.Height(), SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOOWNERZORDER );
		else if( ! m_wndGripper.Create( __EXT_SIZE_GRIPPER_CLASS_NAME, NULL, WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, rcGripper, this, UINT(__EXT_MFC_IDC_STATIC) ) )
			return;
		__EXT_MFC_LONG_PTR dwExStyle = ::__EXT_MFC_GetWindowLong( GetSafeHwnd(), GWL_EXSTYLE );
		m_bGripperRTL = ( (dwExStyle & WS_EX_LAYOUTRTL) != 0 ) ? true : false;
		// show the sizing gripper only if it is really needed
		m_bShowResizingGripper = bHaveSizeGripper ? bShowResizingGripper : ( ( ( GetStyle() & WS_THICKFRAME ) == WS_THICKFRAME ) ? true : false );
		m_wndGripper.SetWindowPos( & CWnd::wndTop, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_FRAMECHANGED | ( m_bShowResizingGripper ? SWP_SHOWWINDOW : SWP_HIDEWINDOW ) );
	}
	const AFX_MSGMAP_ENTRY * _FindMessageMapHandler( UINT nMsg, UINT nCode, UINT nID )
	{
		ASSERT_VALID( this );
		const AFX_MSGMAP * pMessageMap = GetMessageMap(); 
		if( pMessageMap == NULL )
			return NULL;
		UINT nEntry = 0;
		const AFX_MSGMAP_ENTRY * lpEntry = NULL;
		while( ( lpEntry = ( pMessageMap->lpEntries + nEntry ) ) -> nSig != AfxSig_end )
		{
			if( lpEntry->nMessage == nMsg && lpEntry->nCode == nCode &&	nID >= lpEntry->nID && nID <= lpEntry->nLastID )
				return lpEntry;
			nEntry++;
		}
		return NULL;
	}
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam ) 
	{
		if(		(	message == WM_CREATE 
				&&	(!(	IsKindOf( RUNTIME_CLASS(CDialog) )
					||	IsKindOf( RUNTIME_CLASS(CPropertyPage) )
					||	IsKindOf( RUNTIME_CLASS(CPropertySheet) )
					))
				)
			||	message == WM_INITDIALOG
			)
		{
			_InitSizeGripper();
			LRESULT lResult = CExtWSBase::WindowProc( message, wParam, lParam );
			if( m_bAutoSubclassChildControls )
				SubclassChildControls();
			return lResult;
		}
		if( message == WM_WINDOWPOSCHANGED )
		{
			LRESULT lResult = CExtWSBase::WindowProc( message, wParam, lParam );
			LPWINDOWPOS lpWindowPos = reinterpret_cast < LPWINDOWPOS > (lParam);
			ASSERT( lpWindowPos != NULL );
			if(		m_wndGripper.m_hWnd != NULL 
				&&	::IsWindow( m_wndGripper.m_hWnd )
				)
			{
				__EXT_MFC_LONG_PTR dwExStyle = ::__EXT_MFC_GetWindowLong( GetSafeHwnd(), GWL_EXSTYLE );
				bool bRTL = ( (dwExStyle & WS_EX_LAYOUTRTL) != 0 ) ? true : false;
				if( m_bGripperRTL != bRTL )
					_InitSizeGripper();
				CRect rcClient;			
				GetClientRect( &rcClient );
				CSize szGripperSize = PmBridge_GetPM()->GetResizingGriperSize( this );
				m_wndGripper.SetWindowPos(
					NULL, rcClient.right - szGripperSize.cx, rcClient.bottom - szGripperSize.cy, 0, 0,
					SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSIZE|SWP_NOACTIVATE
					);
				m_wndGripper.Invalidate();
				m_wndGripper.UpdateWindow();
			}
			if( ( lpWindowPos->flags & SWP_NOSIZE ) != 0 )
				return lResult;
			if(		::IsWindowVisible( m_hWnd ) 
				// - 2.64
				// &&	PmBridge_GetPM()->GetCb2DbTransparentMode( this )
				)
			{
				// if m_hWnd is dialog, then redraw all child controls
				bool bDlg = false;
				TCHAR strClassName[ _MAX_PATH + 1 ] = _T("");
				::GetClassName( m_hWnd, strClassName, _MAX_PATH );
				if( _tcscmp( strClassName, _T("#32770") ) == 0 )
					bDlg = true;
				if( bDlg )
				{
					RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW| RDW_NOCHILDREN|RDW_NOFRAME );
					for( CWnd * pWnd = this->GetWindow(GW_CHILD); pWnd != NULL; pWnd = pWnd->GetWindow(GW_HWNDNEXT) )
					{
						if( (pWnd->GetStyle() & WS_VISIBLE) == 0 )
							continue;
						pWnd->RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_NOFRAME );
					}
				} // if( bDlg )
			}
			return lResult;
		} // if( message == WM_WINDOWPOSCHANGED )
		if( message == WM_PRINT || message == WM_PRINTCLIENT )
		{
			CExtWSBase::WindowProc( message, wParam, lParam );
			CDC dc;
			dc.Attach( (HDC)wParam );
			CRect rcRgnWnd, rcRgnClient;
			GetWindowRect( &rcRgnWnd );
			GetClientRect( &rcRgnClient );
			if( ( lParam & (PRF_CLIENT|PRF_ERASEBKGND) ) != 0 )
			{
				CPoint ptVpOffset( 0, 0 );
				if( ( lParam & PRF_NONCLIENT ) != 0 )
				{
					CRect rcWnd = rcRgnWnd, rcClient = rcRgnClient;
					ClientToScreen( &rcClient );
					ptVpOffset.x = rcWnd.left - rcClient.left;
					ptVpOffset.y = rcWnd.top - rcClient.top;
				}
				if( ptVpOffset.x != 0 || ptVpOffset.y != 0 )
					dc.OffsetViewportOrg( -ptVpOffset.x, -ptVpOffset.y );
				for( CWnd * pWnd = GetWindow( GW_CHILD ); pWnd != NULL && pWnd != this; pWnd = pWnd->GetWindow( GW_HWNDNEXT ) )
				{
					if( (pWnd->GetStyle() & WS_VISIBLE) == 0 )
						continue;
					CRect rcChild;
					pWnd->GetWindowRect( &rcChild );
					ScreenToClient( &rcChild );
					dc.ExcludeClipRect( &rcChild );
				}
				COLORREF clrBackground = GetBkColor();
				bool bTransparent = false;
				if( PmBridge_GetPM()->GetCb2DbTransparentMode(this) && ( clrBackground == COLORREF(-1L) ) )
				{
					CExtPaintManager::stat_ExcludeChildAreas( dc, GetSafeHwnd(), CExtPaintManager::stat_DefExcludeChildAreaCallback );
					if( PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
						bTransparent = true;
				}
				if( ( ! bTransparent ) && clrBackground != COLORREF(-1L) )
					dc.FillSolidRect( &rcRgnClient, clrBackground );	
				dc.SelectClipRgn( NULL );
				PmBridge_GetPM()->OnPaintSessionComplete( this );
				if( ptVpOffset.x != 0 || ptVpOffset.y != 0 )
					dc.OffsetViewportOrg( ptVpOffset.x, ptVpOffset.y );
			} // if( ( lParam & (PRF_CLIENT|PRF_ERASEBKGND) ) != 0 )
			if( (lParam&PRF_CHILDREN) != 0 )
				CExtPaintManager::stat_PrintChildren( m_hWnd, message, dc.GetSafeHdc(), lParam, false );
			dc.Detach();
			return (!0);
		}
		if( message == WM_PAINT )
		{
			ASSERT_VALID( this );
			bool bDlg = IsKindOf( RUNTIME_CLASS(CDialog) ) ? true : false;
			if( ! bDlg )
			{
				TCHAR strClassName[ _MAX_PATH + 1 ] = _T("");
				::GetClassName( m_hWnd, strClassName, _MAX_PATH );
				__EXT_MFC_STRLWR( strClassName, _MAX_PATH + 1 );
				if( _tcsstr( strClassName, _T("#") ) != NULL )
					bDlg = true;
			}
			// look through the message map to see if this message is handled by user
			const AFX_MSGMAP_ENTRY * lpEntry = _FindMessageMapHandler( message, 0, 0 );
			bool bPainted = false;
			if( ( ! bDlg ) && lpEntry != NULL )
			{
				// invoke the user defined message handler
				union { AFX_PMSG pfn; void (AFX_MSG_CALL CCmdTarget::*pfn_COMMAND)(); } mmf;
				mmf.pfn = lpEntry->pfn;
				(this->*mmf.pfn_COMMAND)();
				bPainted = true;
			}
			if( ! bPainted )
			{
				CPaintDC dc( this );
				COLORREF clrBackground = GetBkColor();
				bool bTransparent = false;
				if( PmBridge_GetPM()->GetCb2DbTransparentMode(this) && ( clrBackground == COLORREF(-1L) ) )
				{
					CExtPaintManager::stat_ExcludeChildAreas( dc, GetSafeHwnd(), CExtPaintManager::stat_DefExcludeChildAreaCallback );
					if( PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
						bTransparent = true;
				}
				if( ( ! bTransparent) && clrBackground != COLORREF(-1L) )
				{
					CRect rcClient;
					GetClientRect( &rcClient );
					dc.FillSolidRect( &rcClient, clrBackground );	
				}
				PmBridge_GetPM()->OnPaintSessionComplete( this );
			} // if( ! bPainted )

			if( bDlg && lpEntry != NULL ) 
			{
				// invoke the user defined message handler
				union { AFX_PMSG pfn; void (AFX_MSG_CALL CCmdTarget::*pfn_COMMAND)(); } mmf;
				mmf.pfn = lpEntry->pfn;
				(this->*mmf.pfn_COMMAND)();
				return 0L;
			}
			return 0L;
		} // if( message == WM_PAINT )
		if( message == WM_ERASEBKGND )
		{
			ASSERT_VALID( this );
			// look through the message map to see if this message is handled by user
			const AFX_MSGMAP_ENTRY * lpEntry = _FindMessageMapHandler( message, 0, 0 );
			if( lpEntry != NULL )
			{
				// invoke the user defined message handler
				union { AFX_PMSG pfn; BOOL (AFX_MSG_CALL CWnd::*pfn_bD)( CDC* ); } mmf;
				mmf.pfn = lpEntry->pfn;
				LRESULT lResult = (this->*mmf.pfn_bD)( CDC::FromHandle( ( HDC ) wParam ) );
				return lResult;
			} // if( lpEntry != NULL )
			HDC hDC = reinterpret_cast < HDC > (wParam);
			ASSERT( hDC != NULL );
			HWND hWnd = ::WindowFromDC( hDC );
			if( hWnd != NULL && hWnd != m_hWnd )
				return CExtWSBase::WindowProc( message, wParam, lParam );
			if( ( GetStyle() & WS_CLIPCHILDREN ) != 0 && PmBridge_GetPM()->GetCb2DbTransparentMode(this) )
				return (!0L);
			CExtPaintManager::stat_ExcludeChildAreas( hDC, GetSafeHwnd(), CExtPaintManager::stat_DefExcludeChildAreaCallback );
			return CExtWSBase::WindowProc( message, wParam, lParam );
		} // if( message == WM_ERASEBKGND )
		if( message >= WM_CTLCOLORMSGBOX && message <= WM_CTLCOLORSTATIC )
		{
			HBRUSH hBrush = (HBRUSH) CExtWSBase::WindowProc( message, wParam, lParam );
			HDC hDC = __EXT_GET_WM_CTLCOLOR_HDC( wParam, lParam, message );
			HWND hWnd = __EXT_GET_WM_CTLCOLOR_HWND( wParam, lParam, message );
			ASSERT( hWnd != NULL );
			INT nCtlColor = __EXT_GET_WM_CTLCOLOR_TYPE( wParam, lParam, message );
			if(		nCtlColor == CTLCOLOR_DLG
				||	nCtlColor == CTLCOLOR_STATIC
				||	nCtlColor == CTLCOLOR_MSGBOX
				)
			{
				if( nCtlColor == CTLCOLOR_STATIC )
				{
					bool bDefaultProcessing = false;
					if( hWnd == NULL || (! ::IsWindow( hWnd ) ) )
						bDefaultProcessing = true;
					else
					{
						CExtSafeString sClassName;
						::GetClassName( hWnd, LPTSTR( sClassName.GetBuffer( _MAX_PATH + 1 ) ), _MAX_PATH );
						sClassName.ReleaseBuffer();
						sClassName.MakeLower();
						if( sClassName != LPCTSTR( _T("static") ) )
							bDefaultProcessing = true;
						if( bDefaultProcessing )
							return (LRESULT)hBrush;
					} // else from if( hWnd == NULL || (! ::IsWindow(hWnd) ) )
					::SetBkMode( hDC, TRANSPARENT );
					// (+ v.2.22) corrected by the Dmitry Yakovlev's advice
					if(		(GetStyle()&WS_CHILD) != 0
						&&	PmBridge_GetPM()->GetCb2DbTransparentMode(this)
						)
						return (LRESULT)::GetStockObject( NULL_BRUSH );
				}
				static CBrush brBackground;
				static COLORREF clrLast = (COLORREF)(-1L);
				COLORREF clrNew = PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT, this );
				if( clrLast != clrNew )
				{
					if( brBackground.GetSafeHandle() != NULL )
						brBackground.DeleteObject();
				}
				if( brBackground.GetSafeHandle() == NULL )
				{
					clrLast = clrNew;
					VERIFY( brBackground.CreateSolidBrush( clrLast ) );
				} // if( brBackground.GetSafeHandle() == NULL )
				return (LRESULT)brBackground.GetSafeHandle();
			}
			return (LRESULT)hBrush;
		} // if( message >= WM_CTLCOLORMSGBOX && message <= WM_CTLCOLORSTATIC )
		if( message == WM_TIMER )
		{
			if( wParam == nIdTimerSysMenuTracking )
			{
				if( ! CExtPopupMenuWnd::IsMenuTracking() )
				{
					m_bSysMenuTracking = false;
					KillTimer( nIdTimerSysMenuTracking );
				}
				return 0;
			}
			return CExtWSBase::WindowProc( message, wParam, lParam );
		} // if( message == WM_TIMER )
		if( message == WM_DESTROY )
		{
			if( m_bEnableSaveRestore )
				SaveWindowRect();
			return CExtWSBase::WindowProc( message, wParam, lParam );
		}
		if(		( message == WM_NCLBUTTONDOWN && wParam == HTSYSMENU )
			||	( message == WM_NCRBUTTONUP && ( wParam == HTCAPTION || wParam == HTSYSMENU ) )
			)
		{
			if( wParam == HTCAPTION )
			{
				CPoint point;
				if( ::GetCursorPos( &point ) )
					_TrackWndSystemPopupMenu( &point, FALSE );
			} // if( wParam == HTCAPTION )
			else
				_TrackWndSystemPopupMenu( NULL, FALSE );
			return 0;
		}
		if(		( message == WM_NCLBUTTONDOWN || message == WM_NCRBUTTONDOWN )
			&&	wParam == HTSYSMENU
			)
			return 0;
		if(		message == WM_NCLBUTTONDBLCLK
			&&	wParam == HTSYSMENU
			)
			CExtPopupMenuWnd::CancelMenuTracking();
		if( message == WM_NCRBUTTONDOWN && wParam == HTCAPTION )
		{
			CPoint point;
			if( ::GetCursorPos( &point ) )
				_TrackWndSystemPopupMenu( &point, FALSE );
			return 0;
		}
		return CExtWSBase::WindowProc( message, wParam, lParam );
	}
	virtual void PostNcDestroy()
	{
		m_bStyledWndInitDone = false;
		CExtWSBase::PostNcDestroy();
	}
	
}; // class CExtWS

/////////////////////////////////////////////////////////////////////////////
// CExtWA template window extension
//
// adds child anchoring support to any window;
// anchoring idea by Paolo Messina, copyright provided:
//
//
// Copyright (C) 2000 by Paolo Messina
// (ppescher@yahoo.com)
//

const SIZE __RDA_NONE	= {	 -1,	 -1 };
const SIZE __RDA_LT		= {	  0,	  0 };
const SIZE __RDA_RT		= {	100,	  0 };
const SIZE __RDA_LB		= {	  0,	100 };
const SIZE __RDA_RB		= {	100,	100 };

const SIZE __RDA_KEEP	= __RDA_LT;
const SIZE __RDA_X		= __RDA_RT;
const SIZE __RDA_Y		= __RDA_LB;
const SIZE __RDA_XY		= __RDA_RB;
const SIZE __RDA_BOTH	= __RDA_RB;


template < class CExtWABase >
class CExtWA : public CExtWABase
{
protected:
	bool m_bAnchoringInitDone:1, m_bUseMaxTrack:1, m_bUseMinTrack:1, m_bUseMaxRect:1;
	CPoint m_ptMinTrackSize;	// min tracking size
	CPoint m_ptMaxTrackSize;	// max tracking size
	CPoint m_ptMaxPos;			// maximized position
	CPoint m_ptMaxSize;			// maximized size
	struct RDI_t
	{
		// upper-left corner
		SIZE m_AnchLT, m_MargLT;
		// bottom-right corner
		SIZE m_AnchRB, m_MargRB;
		RDI_t(
			SIZE AnchLT = __RDA_LT,
			SIZE MargLT = CSize( 0, 0 ), 
			SIZE AnchRB = __RDA_LT,
			SIZE MargRB = CSize( 0, 0 )
			)
			: m_AnchLT( AnchLT )
			, m_MargLT( MargLT )
			, m_AnchRB( AnchRB )
			, m_MargRB( MargRB )
		{
		}
		RDI_t( const RDI_t & other )
			: m_AnchLT( other.m_AnchLT )
			, m_MargLT( other.m_MargLT )
			, m_AnchRB( other.m_AnchRB )
			, m_MargRB( other.m_MargRB )
		{
		}
		RDI_t & operator = ( const RDI_t & other )
		{
			m_AnchLT = other.m_AnchLT;
			m_MargLT = other.m_MargLT;
			m_AnchRB = other.m_AnchRB;
			m_MargRB = other.m_MargRB;
			return *this;
		}
	}; // struct RDI_t
	// map of reposition-able controls
	CMap < HWND, HWND, RDI_t, RDI_t > m_mapRDI;
public:
	CExtWA()
		: m_bAnchoringInitDone( false )
		, m_bUseMinTrack( false )
		, m_bUseMaxTrack( false )
		, m_bUseMaxRect( false )
		, m_ptMinTrackSize( 0, 0 )
		, m_ptMaxTrackSize( 0, 0 )
		, m_ptMaxPos( 0, 0 )
		, m_ptMaxSize( 0, 0 )
		{ }
	CExtWA( UINT nIDTemplate, CWnd * pParentWnd )
		: CExtWABase( nIDTemplate, pParentWnd )
		, m_bAnchoringInitDone( false )
		, m_bUseMinTrack( false )
		, m_bUseMaxTrack( false )
		, m_bUseMaxRect( false )
		, m_ptMinTrackSize( 0, 0 )
		, m_ptMaxTrackSize( 0, 0 )
		, m_ptMaxPos( 0, 0 )
		, m_ptMaxSize( 0, 0 )
		{ }
	CExtWA( __EXT_MFC_SAFE_LPCTSTR lpszTemplateName, CWnd * pParentWnd )
		: CExtWABase( lpszTemplateName, pParentWnd )
		, m_bAnchoringInitDone( false )
		, m_bUseMinTrack( false )
		, m_bUseMaxTrack( false )
		, m_bUseMaxRect( false )
		, m_ptMinTrackSize( 0, 0 )
		, m_ptMaxTrackSize( 0, 0 )
		, m_ptMaxPos( 0, 0 )
		, m_ptMaxSize( 0, 0 )
		{ }
	CExtWA( UINT nIDTemplate, UINT nIDCaption = 0 )
		: CExtWABase( nIDTemplate, nIDCaption )
		, m_bAnchoringInitDone( false )
		, m_bUseMinTrack( false )
		, m_bUseMaxTrack( false )
		, m_bUseMaxRect( false )
		, m_ptMinTrackSize( 0, 0 )
		, m_ptMaxTrackSize( 0, 0 )
		, m_ptMaxPos( 0, 0 )
		, m_ptMaxSize( 0, 0 )
		{ }
	CExtWA( __EXT_MFC_SAFE_LPCTSTR lpszTemplateName, UINT nIDCaption = 0 )
		: CExtWABase( lpszTemplateName, nIDCaption )
		, m_bAnchoringInitDone( false )
		, m_bUseMinTrack( false )
		, m_bUseMaxTrack( false )
		, m_bUseMaxRect( false )
		, m_ptMinTrackSize( 0, 0 )
		, m_ptMaxTrackSize( 0, 0 )
		, m_ptMaxPos( 0, 0 )
		, m_ptMaxSize( 0, 0 )
		{ }
	CExtWA( UINT nIDCaption, CWnd *pParentWnd, UINT iSelectPage )
		: CExtWABase( nIDCaption, pParentWnd, iSelectPage )
		, m_bAnchoringInitDone( false )
		, m_bUseMinTrack( false )
		, m_bUseMaxTrack( false )
		, m_bUseMaxRect( false )
		, m_ptMinTrackSize( 0, 0 )
		, m_ptMaxTrackSize( 0, 0 )
		, m_ptMaxPos( 0, 0 )
		, m_ptMaxSize( 0, 0 )
		{ }
	CExtWA( __EXT_MFC_SAFE_LPCTSTR pszCaption, CWnd * pParentWnd, UINT iSelectPage )
		: CExtWABase( pszCaption, pParentWnd, iSelectPage )
		, m_bAnchoringInitDone( false )
		, m_bUseMinTrack( false )
		, m_bUseMaxTrack( false )
		, m_bUseMaxRect( false )
		, m_ptMinTrackSize( 0, 0 )
		, m_ptMaxTrackSize( 0, 0 )
		, m_ptMaxPos( 0, 0 )
		, m_ptMaxSize( 0, 0 )
		{ }
	virtual ~CExtWA()
		{ RemoveAllAnchors(); }
	virtual CExtPaintManager * PmBridge_GetPM() const
	{ ASSERT_VALID( this ); return g_PaintManager.GetPM(); }	
	bool RemoveAnchor( UINT nDlgCtrlID )
	{
		if( GetSafeHwnd() == NULL )
			return false;
		HWND hWnd = NULL;
#if (defined __EXT_TEMPL_OCC_SUPPORT_IMPLEMENTED)
		if( m_pCtrlCont != NULL )
			m_pCtrlCont->GetDlgItem( nDlgCtrlID, &hWnd );
		else
#endif // (defined __EXT_TEMPL_OCC_SUPPORT_IMPLEMENTED)
			hWnd = ::GetDlgItem( GetSafeHwnd(), nDlgCtrlID );
		if( hWnd == NULL || ( ! ::IsWindow(hWnd) ) )
			return false;
		return RemoveAnchor( hWnd );
	}
	bool RemoveAnchor( HWND hWnd )
		{ return m_mapRDI.RemoveKey( hWnd ) ? true : false; }
	void RemoveAllAnchors()
		{ m_mapRDI.RemoveAll(); }
	bool AddAnchor( UINT nDlgCtrlID, const CRect & rcAnch )
	{
		if( GetSafeHwnd() == NULL )
			return false;
		HWND hWnd = NULL;
#if (defined __EXT_TEMPL_OCC_SUPPORT_IMPLEMENTED)
		if( m_pCtrlCont != NULL )
			m_pCtrlCont->GetDlgItem( nDlgCtrlID, &hWnd );
		else
#endif // (defined __EXT_TEMPL_OCC_SUPPORT_IMPLEMENTED)
			hWnd = ::GetDlgItem( GetSafeHwnd(), nDlgCtrlID );
		if( hWnd == NULL || ( ! ::IsWindow(hWnd) ) )
			return false;
		return AddAnchor( hWnd, rcAnch );
	}
	bool AddAnchor( HWND hWnd, const CRect & rcAnch )
	{
		if( GetSafeHwnd() == NULL )
			return false;
		return AddAnchor( hWnd, CSize( rcAnch.left, rcAnch.top ), CSize( rcAnch.right, rcAnch.bottom ) );
	}
	bool AddAnchor( UINT nDlgCtrlID, CSize AnchLT, CSize AnchRB = __RDA_NONE )
	{
		if( AnchLT == __RDA_NONE || GetSafeHwnd() == NULL )
			return false;
		HWND hWnd = NULL;
#if (defined __EXT_TEMPL_OCC_SUPPORT_IMPLEMENTED)
		if( m_pCtrlCont != NULL )
			m_pCtrlCont->GetDlgItem( nDlgCtrlID, &hWnd );
		else
#endif // (defined __EXT_TEMPL_OCC_SUPPORT_IMPLEMENTED)
			hWnd = ::GetDlgItem( GetSafeHwnd(), nDlgCtrlID );
		if( hWnd == NULL || ( ! ::IsWindow(hWnd) ) )
			return false;
		return AddAnchor( hWnd, AnchLT, AnchRB );
	}
	virtual bool AddAnchor( HWND hWnd, CSize AnchLT, CSize AnchRB = __RDA_NONE )
	{
		if( AnchLT == __RDA_NONE || GetSafeHwnd() == NULL || ( ! ::IsChild( m_hWnd, hWnd ) ) )
			return false;
		CRect rcClient, rcItem;
		GetClientRect( &rcClient );
		int cx = rcClient.Width();
		int cy = rcClient.Height();
		::GetWindowRect( hWnd, &rcItem );
		ScreenToClient( &rcItem );
		CSize MargLT, MargRB;
		if( AnchRB == __RDA_NONE )
			AnchRB = AnchLT;
		MargLT.cx = rcItem.left - cx*AnchLT.cx/100;
		MargLT.cy = rcItem.top - cy*AnchLT.cy/100;
		MargRB.cx = rcItem.right - cx*AnchRB.cx/100;
		MargRB.cy = rcItem.bottom - cy*AnchRB.cy/100;
		m_mapRDI.SetAt( hWnd, RDI_t( AnchLT, MargLT, AnchRB, MargRB ) );
		return true;
	}
	virtual void ArrangeLayout( int cx = -1, int cy = -1 )
	{
		if( GetSafeHwnd() == NULL )
			return;
		INT nCount = INT( m_mapRDI.GetCount() );
		if( nCount <= 0 )
			return;
		if( cx < 0 || cy < 0 )
		{
			CRect rcClient;
			GetClientRect( &rcClient );
			cx = rcClient.Width();
			cy = rcClient.Height();
		} // if( cx < 0 || cy < 0 )
		HDWP _hDWP = ::BeginDeferWindowPos( nCount );
		ASSERT( _hDWP != NULL );
		CList < HWND, HWND > listInvalidHWNDs;
		POSITION pos = m_mapRDI.GetStartPosition();
		for( ; pos != NULL; )
		{
			HWND _hWnd;
			RDI_t _rdi;
			m_mapRDI.GetNextAssoc( pos, _hWnd, _rdi );
			if( ! ::IsWindow( _hWnd ) )
			{
				listInvalidHWNDs.AddTail(_hWnd);
				continue;
			}
			int x  = _rdi.m_MargLT.cx + ::MulDiv(cx,_rdi.m_AnchLT.cx,100);
			int y  = _rdi.m_MargLT.cy + ::MulDiv(cy,_rdi.m_AnchLT.cy,100);
			int dx = _rdi.m_MargRB.cx + ::MulDiv(cx,_rdi.m_AnchRB.cx,100) - x;
			int dy = _rdi.m_MargRB.cy + ::MulDiv(cy,_rdi.m_AnchRB.cy,100) - y;
			RECT rcWnd;
			::GetWindowRect( _hWnd, &rcWnd );
			::ScreenToClient( m_hWnd, ((LPPOINT)(&rcWnd))+0 );
			::ScreenToClient( m_hWnd, ((LPPOINT)(&rcWnd))+1 );
			UINT uFlags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER;
			if( rcWnd.left == x && rcWnd.top == y )
				uFlags |= SWP_NOMOVE;
			if(		( rcWnd.right - rcWnd.left ) == dx
				&&	( rcWnd.bottom - rcWnd.top ) == dy
				)
				uFlags |= SWP_NOSIZE;
			if(		( uFlags & SWP_NOMOVE ) != 0
				&&	( uFlags & SWP_NOSIZE ) != 0
				)
				continue;
			_hDWP = ::DeferWindowPos( _hDWP, _hWnd, NULL, x, y, dx, dy, uFlags );
			ASSERT( _hDWP != NULL );
		} // for( ; pos != NULL; )

		VERIFY( ::EndDeferWindowPos( _hDWP ) );
		if( listInvalidHWNDs.GetCount() > 0 )
		{
			pos = listInvalidHWNDs.GetHeadPosition();
			for( ; pos != NULL; )
			{
				HWND hWnd = listInvalidHWNDs.GetNext(pos);
				m_mapRDI.RemoveKey( hWnd );
			} // for( ; pos != NULL; )
		} // if( listInvalidHWNDs.GetCount() > 0 )
		if( ( m_nFlags & WF_OLECTLCONTAINER ) != 0 )
		{
			for( pos = m_mapRDI.GetStartPosition(); pos != NULL; )
			{
				HWND _hWnd;
				RDI_t _rdi;
				m_mapRDI.GetNextAssoc( pos, _hWnd, _rdi );
				ASSERT( ::IsWindow( _hWnd ) );
				if( ! ::IsWindow( _hWnd ) )
					continue;
				CWnd * pWnd = CWnd::FromHandlePermanent( _hWnd );
				if( pWnd == NULL )
					continue;
				LPUNKNOWN pUnknown = pWnd->GetControlUnknown();
				if( pUnknown != NULL )
				{
					IOleObject * pOleObject = NULL;
					HRESULT hr = pUnknown->QueryInterface( __uuidof(IOleObject), (LPVOID*)(&pOleObject) );
					if( hr == S_OK )
					{
						ASSERT( pOleObject != NULL );
						IOleInPlaceObject * pOleInPlaceObject = NULL;
						hr = pUnknown->QueryInterface( __uuidof(IOleInPlaceObject), (LPVOID*)(&pOleInPlaceObject) );
						if( hr == S_OK )
						{
							ASSERT( pOleInPlaceObject != NULL );
							CRect rcClient;
							GetClientRect( &rcClient );
							CRect rc;
							pWnd->GetWindowRect( &rc );
							ScreenToClient( &rc );
							SIZEL _sizeL;
							_sizeL.cx = rc.Width();
							_sizeL.cy = rc.Height();
							CClientDC dc(pWnd);
							dc.DPtoHIMETRIC(&_sizeL);
							hr = pOleObject->SetExtent( DVASPECT_CONTENT, &_sizeL );
					 		ASSERT( SUCCEEDED( hr ) );
							hr = pOleInPlaceObject->SetObjectRects( &rc, &rcClient );
					 		ASSERT( SUCCEEDED( hr ) );
							hr;
							IOleClientSite * pOleClientSite = NULL;
							hr = pOleObject->GetClientSite( &pOleClientSite );
							if( hr == S_OK )
							{ // ActiveX resizing fix by Jean-Yves Tremblay
								ASSERT( pOleClientSite != NULL );
								IOleInPlaceSite * pOleInPlaceSite = NULL;
								hr = pOleClientSite->QueryInterface( __uuidof(IOleInPlaceSite), (LPVOID*)(&pOleInPlaceSite) );
								if( hr == S_OK )
								{
									ASSERT( pOleInPlaceSite != NULL );
									hr = pOleInPlaceSite->OnPosRectChange( &rc );
									ASSERT( SUCCEEDED( hr ) );
									pOleInPlaceSite->Release();
								}
								pOleClientSite->Release();
							} // ActiveX resizing fix by Jean-Yves Tremblay

							pOleInPlaceObject->Release();
						} // if( hr == S_OK )
						pOleObject->Release();
					} // if( hr == S_OK )
				} // if( pUnknown != NULL )
			} // for( pos = m_mapRDI.GetStartPosition(); pos != NULL; )
		} // if( ( m_nFlags & WF_OLECTLCONTAINER ) != 0 )
	}
	void SetMaximizedRect( const CRect & rc )
	{
		m_bUseMaxRect = true;
		m_ptMaxPos = rc.TopLeft();
		m_ptMaxSize.x = rc.Width();
		m_ptMaxSize.y = rc.Height();
	}
	void ResetMaximizedRect()
		{ m_bUseMaxRect = false; }
	void SetMinTrackSize( const CSize & size )
	{
		m_bUseMinTrack = true;
		m_ptMinTrackSize.x = size.cx;
		m_ptMinTrackSize.y = size.cy;
	}
	void ResetMinTrackSize()
		{ m_bUseMinTrack = false; }
	void SetMaxTrackSize( const CSize & size )
	{
		m_bUseMaxTrack = true;
		m_ptMaxTrackSize.x = size.cx;
		m_ptMaxTrackSize.y = size.cy;
	}
	void ResetMaxTrackSize()
		{ m_bUseMaxTrack = false; }
protected:
	virtual bool NcFrameImpl_GetMinMaxInfo( LPMINMAXINFO pMMI ) const
	{
		if( m_bUseMinTrack )
			pMMI->ptMinTrackSize = m_ptMinTrackSize;
		if( m_bUseMaxTrack )
			pMMI->ptMaxTrackSize = m_ptMaxTrackSize;
		if( m_bUseMaxRect )
		{
			pMMI->ptMaxPosition = m_ptMaxPos;
			pMMI->ptMaxSize = m_ptMaxSize;
		}
		return true;
	}
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam ) 
	{
		if( message == WM_GETMINMAXINFO )
		{
			if( ! m_bAnchoringInitDone )
				return 0;
			CExtWABase::WindowProc( message, wParam, lParam );
			LPMINMAXINFO pMMI = reinterpret_cast < LPMINMAXINFO > ( lParam );
			NcFrameImpl_GetMinMaxInfo( pMMI );
			return 0;
		} // if( message == WM_GETMINMAXINFO )
		if( message == WM_SIZE )
		{
			LRESULT lResult = CExtWABase::WindowProc( message, wParam, lParam );
			if( wParam == SIZE_MINIMIZED )
				return lResult;
			if( ! m_bAnchoringInitDone )
			{
				// gets the template size as the min track size
				CRect rc;
				GetWindowRect( &rc );
				m_ptMinTrackSize.x = rc.Width();
				m_ptMinTrackSize.y = rc.Height();
				m_bUseMinTrack = true;
				m_bAnchoringInitDone = true;
				ArrangeLayout();
				return lResult;
			} // if( !m_bAnchoringInitDone )
			if( wParam == SIZE_MAXHIDE || wParam == SIZE_MAXSHOW )
				return lResult; // arrangement not needed
			if( m_bAnchoringInitDone )
				ArrangeLayout( LOWORD(lParam), HIWORD(lParam) );
			return lResult;
		} // if( message == WM_SIZE )
		CWnd * pWndThis = this;
		HWND hWndOwn = m_hWnd;
		LRESULT lResult = CExtWABase::WindowProc( message, wParam, lParam );
		if(		message == WM_DESTROY
			||	message == WM_NCDESTROY
			)
		{
			if(		hWndOwn != NULL
				&&	::IsWindow( hWndOwn )
				&&	CWnd::FromHandlePermanent( hWndOwn ) == pWndThis
				)
				RemoveAllAnchors();
		}
		return lResult;
	}
}; // class CExtWA

/////////////////////////////////////////////////////////////////////////////
// CExtADLG template adaptor for CDialog

template < class CExtADLGBase >
class CExtADLG : public CExtADLGBase
{
public:
	CExtADLG()
	{
	}
	CExtADLG(
		UINT nIDTemplate,
		CWnd * pParentWnd
		)
		: CExtADLGBase( nIDTemplate, pParentWnd )
	{
	}
	
	CExtADLG(
		__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
		CWnd * pParentWnd
		)
		: CExtADLGBase( lpszTemplateName, pParentWnd )
	{
	}
	
	CExtADLG(
			UINT nIDTemplate,
			UINT nIDCaption = 0
			)
	{
		// no such constructor in CDialog
		nIDTemplate;
		nIDCaption;
		ASSERT( FALSE );
	}
	
	CExtADLG(
			__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
			UINT nIDCaption = 0
			)
	{
		// no such constructor in CDialog
		lpszTemplateName;
		nIDCaption;
		ASSERT( FALSE );
	}
	
	CExtADLG(
		UINT nIDCaption,
		CWnd *pParentWnd,
		UINT iSelectPage
		)
	{
		// no such constructor in CDialog
		nIDCaption;
		pParentWnd;
		iSelectPage;
		ASSERT( FALSE );
	}
	
	CExtADLG(
		__EXT_MFC_SAFE_LPCTSTR pszCaption,
		CWnd *pParentWnd,
		UINT iSelectPage
		)
	{
		// no such constructor in CDialog
		pszCaption;
		pParentWnd;
		iSelectPage;
		ASSERT( FALSE );
	}
}; // class CExtADLG

/////////////////////////////////////////////////////////////////////////////
// CExtAPSH template adaptor for CPropertySheet

template < class CExtAPSHBase >
class CExtAPSH : public CExtAPSHBase
{
public:
	CExtAPSH()
	{
	}
	
	CExtAPSH(
		UINT nIDTemplate,
		CWnd * pParentWnd
		)
	{
		// no such constructor in CPropertySheet
		nIDTemplate;
		pParentWnd;
		ASSERT( FALSE );
	}
	
	CExtAPSH(
		__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
		CWnd * pParentWnd
		)
	{
		// no such constructor in CPropertySheet
		lpszTemplateName;
		pParentWnd;
		ASSERT( FALSE );
	}
	
	CExtAPSH(
			UINT nIDTemplate,
			UINT nIDCaption = 0
			)
	{
		// no such constructor in CPropertySheet
		nIDTemplate;
		nIDCaption;
		ASSERT( FALSE );
	}
	
	CExtAPSH(
			__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
			UINT nIDCaption = 0
			)
	{
		// no such constructor in CPropertySheet
		lpszTemplateName;
		nIDCaption;
		ASSERT( FALSE );
	}
	
	CExtAPSH(
		UINT nIDCaption,
		CWnd *pParentWnd,
		UINT iSelectPage
		)
		: CExtAPSHBase( nIDCaption, pParentWnd, iSelectPage )
	{
	}
	
	CExtAPSH(
		__EXT_MFC_SAFE_LPCTSTR pszCaption,
		CWnd *pParentWnd,
		UINT iSelectPage
		)
		: CExtAPSHBase( pszCaption, pParentWnd, iSelectPage )
	{
	}
}; // class CExtAPSH

/////////////////////////////////////////////////////////////////////////////
// CExtAPPG template adaptor for CPropertyPage

template < class CExtAPPGBase >
class CExtAPPG : public CExtAPPGBase
{
public:
	CExtAPPG()
	{
	}
	
	CExtAPPG(
		UINT nIDTemplate,
		CWnd * pParentWnd
		)
	{
		// no such constructor in CPropertyPage
		nIDTemplate;
		pParentWnd;
		ASSERT( FALSE );
	}
	
	CExtAPPG(
		__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
		CWnd * pParentWnd
		)
	{
		// no such constructor in CPropertyPage
		lpszTemplateName;
		pParentWnd;
		ASSERT( FALSE );
	}
	
	CExtAPPG(
			UINT nIDTemplate,
			UINT nIDCaption = 0
			)
		: CExtAPPGBase( nIDTemplate, nIDCaption )
	{
	}
	
	CExtAPPG(
			__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
			UINT nIDCaption = 0
			)
		: CExtAPPGBase( lpszTemplateName, nIDCaption )
	{
	}
	
	CExtAPPG(
		UINT nIDCaption,
		CWnd *pParentWnd,
		UINT iSelectPage
		)
	{
		// no such constructor in CPropertyPage
		nIDCaption;
		pParentWnd;
		iSelectPage;
		ASSERT( FALSE );
	}
	
	CExtAPPG(
		__EXT_MFC_SAFE_LPCTSTR pszCaption,
		CWnd *pParentWnd,
		UINT iSelectPage
		)
	{
		// no such constructor in CPropertyPage
		pszCaption;
		pParentWnd;
		iSelectPage;
		ASSERT( FALSE );
	}
}; // class CExtAPPG

/////////////////////////////////////////////////////////////////////////////
// CExtAGW template adaptor for generic window classes

template < class CExtAGWBase >
class CExtAGW : public CExtAGWBase
{
public:
	CExtAGW()
	{
	}
	
	CExtAGW(
		UINT nIDTemplate,
		CWnd * pParentWnd
		)
	{
		// should not have such constructor
		nIDTemplate;
		pParentWnd;
		ASSERT( FALSE );
	}
	
	CExtAGW(
		__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
		CWnd * pParentWnd
		)
	{
		// should not have such constructor
		lpszTemplateName;
		pParentWnd;
		ASSERT( FALSE );
	}
	
	CExtAGW(
			UINT nIDTemplate,
			UINT nIDCaption = 0
			)
		: CExtAGWBase( nIDTemplate, nIDCaption )
	{
		// should not have such constructor
		nIDTemplate;
		nIDCaption;
		ASSERT( FALSE );
	}
	
	CExtAGW(
			__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
			UINT nIDCaption = 0
			)
		: CExtAGWBase( lpszTemplateName, nIDCaption )
	{
		// should not have such constructor
		lpszTemplateName;
		nIDCaption;
		ASSERT( FALSE );
	}
	
	CExtAGW(
		UINT nIDCaption,
		CWnd *pParentWnd,
		UINT iSelectPage
		)
	{
		// should not have such constructor
		nIDCaption;
		pParentWnd;
		iSelectPage;
		ASSERT( FALSE );
	}
	
	CExtAGW(
		__EXT_MFC_SAFE_LPCTSTR pszCaption,
		CWnd *pParentWnd,
		UINT iSelectPage
		)
	{
		// should not have such constructor
		pszCaption;
		pParentWnd;
		iSelectPage;
		ASSERT( FALSE );
	}
}; // class CExtAGW

/////////////////////////////////////////////////////////////////////////////
// CExtAFV template adaptor for CFormView

template < class CExtAFVBase >
class CExtAFV : public CExtAFVBase
{
public:    
	CExtAFV()
		: CExtAFVBase( (UINT) 0 )
	{
	}    
	CExtAFV(
		__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
		CWnd * pParentWnd
		)
		: CExtAFVBase( lpszTemplateName )
	{        
		pParentWnd;    
	}    
	CExtAFV(
		UINT nIDTemplate,
		CWnd * pParentWnd
		)
		: CExtAFVBase( nIDTemplate )
	{        
		pParentWnd;    
	}
}; // class CExtAFV

/////////////////////////////////////////////////////////////////////////////
// CExtCTA template (cool tip able)

#define __EXT_MFC_TIMER_ID_COOL_TIP_TEMPLATE 7654

template < class _CTA_BASE >
class CExtCTA : public _CTA_BASE
{
protected:
	mutable CExtPopupMenuTipWnd m_wndCoolTip;
	mutable __EXT_MFC_INT_PTR m_nLastToolHitID;
	mutable CRect m_rcLastToolHit;
	void _CancelTip() const
	{
		if( GetSafeHwnd() != NULL )
			((CWnd*)this)->KillTimer( __EXT_MFC_TIMER_ID_COOL_TIP_TEMPLATE );
		m_nLastToolHitID = -1;
		m_rcLastToolHit.SetRectEmpty();
		if( m_wndCoolTip.GetSafeHwnd() != NULL )
			m_wndCoolTip.Hide();
		CWnd::CancelToolTips();
	}
protected:
	virtual __EXT_MFC_INT_PTR OnToolHitTest(
		CPoint point,
		TOOLINFO * pTI
		) const
	{
		ASSERT_VALID( this );
		__EXT_MFC_INT_PTR nToolTipHit =
			_CTA_BASE::OnToolHitTest( point, pTI );
		bool bHide = true;
		if( nToolTipHit >= 0 )
		{
			if( m_nLastToolHitID != nToolTipHit )
			{
				if( pTI->lpszText != NULL )
				{
					if( pTI->lpszText != LPSTR_TEXTCALLBACK )
					{
						bHide = false;
						m_nLastToolHitID = nToolTipHit;
						m_rcLastToolHit = pTI->rect;
						ClientToScreen( &m_rcLastToolHit );
						m_wndCoolTip.SetText(
							LPCTSTR(LPVOID(pTI->lpszText))
							);
						((CWnd*)this)->SetTimer(
							__EXT_MFC_TIMER_ID_COOL_TIP_TEMPLATE,
							50,
							NULL
							);
						m_wndCoolTip.Show(
							(CWnd*)this,
							m_rcLastToolHit
							);
						::free( pTI->lpszText );
					} // if( pTI->lpszText != LPSTR_TEXTCALLBACK )
					pTI->lpszText = NULL;
				} // if( pTI->lpszText != NULL )
			} // if( nToolTipHit >= 0 )
			else
				bHide = false;
			nToolTipHit = -1;
		}
		if( bHide )
			_CancelTip();
		return nToolTipHit;
	}
	LRESULT WindowProc(
		UINT message,
		WPARAM wParam,
		LPARAM lParam
		)
	{
		if( message == WM_DESTROY || message == WM_NCDESTROY )
			_CancelTip();
		else if(
				message == WM_TIMER
			&&	wParam == __EXT_MFC_TIMER_ID_COOL_TIP_TEMPLATE
			)
		{
			if(		m_nLastToolHitID < 0
				||	m_rcLastToolHit.IsRectEmpty()
				)
			{
				_CancelTip();
				return 0;
			}
			POINT ptCursor;
			if( ! ::GetCursorPos( &ptCursor ) )
			{
				_CancelTip();
				return 0;
			}
			if( ! m_rcLastToolHit.PtInRect(ptCursor) )
			{
				_CancelTip();
				return 0;
			}
			return 0; 
		}
		LRESULT lResult = 
			_CTA_BASE::WindowProc( message, wParam, lParam );
		return lResult;
	}
	virtual void PreSubclassWindow()
	{
		_CancelTip();
		_CTA_BASE::PreSubclassWindow();
	}
}; // class CExtCTA

#endif // __EXT_TEMPL_H

