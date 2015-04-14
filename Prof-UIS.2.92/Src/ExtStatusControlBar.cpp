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

#ifndef __EXT_MFC_NO_STATUSBAR

#if (!defined __EXT_STATUSCONTROLBAR_H)
	#include <ExtStatusControlBar.h>
#endif

#if (!defined __EXT_RICH_CONTENT_H)
	#include <ExtRichContent.h>
#endif // (!defined __EXT_RICH_CONTENT_H)

#if (!defined __EXTDOCKBAR_H)
	#include "ExtDockBar.h"
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if _MFC_VER < 0x700
	#include <../src/AfxImpl.h>
#else
	#include <../src/mfc/AfxImpl.h>
#endif

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	#if (!defined __EXTCUSTOMIZE_H)
		#include <ExtCustomize.h>
	#endif
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#if (!defined __EXT_MEMORY_DC_H)
	#include <../src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if (!defined SBPF_UPDATE)
	#define SBPF_UPDATE 0x0001  // pending update of text
#endif // _DEBUG

/////////////////////////////////////////////////////////////////////////////
// CExtStatusControlBar

IMPLEMENT_DYNCREATE( CExtStatusControlBar, CStatusBar )
IMPLEMENT_CExtPmBridge_MEMBERS( CExtStatusControlBar );

CExtStatusControlBar::CExtStatusControlBar()
	: m_bOuterRectInFirstBand( false )
	, m_bCompleteRepaint( true )
	, m_bDrawPaneSeparatorsInsteadOfBorders( false ) // default status bar control behavior
	, m_bHideTextOnDisabledPanes( true ) // default status bar control behavior
	, m_bDoNotPaintIcons( false )
	, m_nDrawPaneTextFlags( DT_SINGLELINE|DT_LEFT|DT_VCENTER ) // default status bar control behavior
	, m_bLockPainting( false )
	, m_nAdvancedTipStyle( INT(CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON) )
{
	m_clrStatusBk = (COLORREF)(-1);
	PmBridge_Install();
}

CExtStatusControlBar::~CExtStatusControlBar()
{
	PmBridge_Uninstall();
	_RemovePanesImpl();
}

void CExtStatusControlBar::PostNcDestroy()
{
	_RemovePanesImpl();
	CStatusBar::PostNcDestroy();
}

void CExtStatusControlBar::_RemovePanesImpl()
{
INT nCount = INT( m_arrPaneControls.GetSize() );
	if( nCount == 0 )
		return;
CList < UINT, UINT > _listCtrlIDs( nCount );
 	for( INT i = 0; i < nCount; i++ )
 		_listCtrlIDs.AddTail( UINT( m_arrPaneControls[i]->nID ) );
	for( POSITION pos = _listCtrlIDs.GetHeadPosition(); pos != NULL; )
	{
		UINT nID = _listCtrlIDs.GetNext( pos );
		RemovePaneControl( nID );
	}
}

bool CExtStatusControlBar::OnQueryDrawPaneSeparatorsInsteadOfBorders() const
{
	ASSERT_VALID( this );
	if(		m_bCompleteRepaint
		&&	PmBridge_GetPM()->StatusBar_QuerySkinSupport(this)
		)
		return true;
	return m_bDrawPaneSeparatorsInsteadOfBorders;
}

BEGIN_MESSAGE_MAP(CExtStatusControlBar, CStatusBar)
	//{{AFX_MSG_MAP(CExtStatusControlBar)
	ON_WM_PAINT()
	ON_WM_NCPAINT()
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_SIZEPARENT, OnSizeParent )
	ON_WM_SYSCOLORCHANGE()
	__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_MESSAGE(__ExtMfc_WM_THEMECHANGED, OnThemeChanged)
	__EXT_MFC_ON_WM_NCHITTEST()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CExtStatusControlBar message handlers

void CExtStatusControlBar::OnPaint() 
{
	ASSERT_VALID( this );
	
	if( m_bLockPainting ) 
		return;

	if( !m_bCompleteRepaint )
	{
		Default();
		return;
	}

	CPaintDC dcPaint( this );
	DoPaint( &dcPaint );
}

void CExtStatusControlBar::DoPaint( CDC * pDC )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pDC );

	// XP painting & last pane size fixed by Dmitry Yakovlev
	_SyncStatusBarColors();

CRect rcClient;
	GetClientRect( &rcClient );
	CExtPaintManager::stat_ExcludeChildAreas(
		*pDC,
		GetSafeHwnd(),
		CExtPaintManager::stat_DefExcludeChildAreaCallback
		);
CExtMemoryDC dc(
		pDC,
		&rcClient
		);
	UpdateAllPanes( FALSE, TRUE );
	
	// erase client area with the current color/gradient
CRect rcFill( rcClient );
	rcFill.left = rcFill.top = 0;
	rcFill.right  -= rcClient.left;
	rcFill.bottom -= rcClient.top;
	OnPaintEntireBackground( dc, rcFill );

	// pre-compute gripper rect, exclude it's area
CRect rcGrip( 0, 0, 0, 0 );
CWnd * pWndParentStatus = GetParent(); // GetTopLevelParent();
	if( pWndParentStatus != NULL )
	{
		WINDOWPLACEMENT _wp;
		::memset( (void*)&_wp, 0, sizeof(WINDOWPLACEMENT) );
		_wp.length = sizeof(WINDOWPLACEMENT);
		pWndParentStatus->GetWindowPlacement( &_wp );
		if(		_wp.showCmd != SW_SHOWMAXIMIZED
			//&&	(pWndParentStatus->GetStyle()&WS_BORDER) != 0
			)
		{
			MINMAXINFO _mmi;
			::memset( &_mmi, 0, sizeof(MINMAXINFO) );
			CExtPaintManager::monitor_parms_t _mp;
			CExtPaintManager::stat_GetMonitorParms( _mp, (CWnd*)pWndParentStatus );
			_mmi.ptMaxPosition.x = _mp.m_rcWorkArea.left;
			_mmi.ptMaxPosition.y = _mp.m_rcWorkArea.top;
			_mmi.ptMaxTrackSize.x = _mp.m_rcWorkArea.Width(); // ::GetSystemMetrics( SM_CXMAXTRACK );
			_mmi.ptMaxTrackSize.y = _mp.m_rcWorkArea.Height(); // ::GetSystemMetrics( SM_CYMAXTRACK );
			_mmi.ptMinTrackSize.x = ::GetSystemMetrics( SM_CXMINTRACK );
			_mmi.ptMinTrackSize.y = ::GetSystemMetrics( SM_CYMINTRACK );
			_mmi.ptMaxSize.x = _mmi.ptMaxTrackSize.x;
			_mmi.ptMaxSize.y = _mmi.ptMaxTrackSize.y;
			if( ((CWnd*)pWndParentStatus)->SendMessage( WM_GETMINMAXINFO, 0, LPARAM(&_mmi) ) == 0 )
			{
				if(		_mmi.ptMinTrackSize.x < _mmi.ptMaxTrackSize.x
					||	_mmi.ptMinTrackSize.y < _mmi.ptMaxTrackSize.y
					)
				{
					GetClientRect( &rcGrip );
					rcGrip.left = rcGrip.right - ::GetSystemMetrics( SM_CXVSCROLL );
					dc.ExcludeClipRect( &rcGrip );
				}
			} // if( ((CWnd*)pWndParentStatus)->SendMessage( WM_GETMINMAXINFO, 0, LPARAM(&_mmi) ) == 0 )
		} // if( _wp.showCmd != SW_SHOWMAXIMIZED ...
	} // if( pWndParentStatus != NULL )
	
CFont * pOldFont = NULL;
int nOldBkMode = 0;
COLORREF nOldTextColor = 0;
	if( m_bCompleteRepaint )
	{
		pOldFont = dc.SelectObject( &PmBridge_GetPM()->m_FontNormal );
		nOldBkMode = dc.SetBkMode( TRANSPARENT );
		nOldTextColor = dc.SetTextColor( m_clrPaneTextNormal );
	} // if( m_bCompleteRepaint )
	// re-paint borders/text/icons
	CRect rcPanePrev( 0, 0, 0, 0 );
	for( INT nPaneIdx = 0; nPaneIdx < m_nCount; nPaneIdx++ )
	{
		CRect rcPane;
		GetItemRect( nPaneIdx, &rcPane );
		// CStatusBar::GetItemRect() sometimes returns invalid size 
		// of the last pane - we will re-compute it
        DWORD dwPaneStyle = GetPaneStyle( nPaneIdx );
		if( nPaneIdx == (m_nCount-1) )
		{
			int cx = ::GetSystemMetrics( SM_CXEDGE );
			if( (dwPaneStyle & SBPS_STRETCH ) == 0 )
			{
				UINT nID, nStyle;
				int  cxWidth;
				GetPaneInfo( nPaneIdx, nID, nStyle, cxWidth );
				rcPane.right = rcPane.left + cxWidth + cx*3;
			} // if( (dwPaneStyle & SBPS_STRETCH ) == 0 )
			else
			{
				CRect rcClient;
				GetClientRect( &rcClient );
				rcPane.right = rcClient.right;
				if( (GetStyle() & SBARS_SIZEGRIP) == SBARS_SIZEGRIP )
				{
					int cxSmIcon = ::GetSystemMetrics( SM_CXSMICON );
					rcPane.right -= cxSmIcon + cx;
				} // if( (GetStyle() & SBARS_SIZEGRIP) == SBARS_SIZEGRIP )
			} // else from if( (dwPaneStyle & SBPS_STRETCH ) == 0 )
		} // if( nPaneIdx == (m_nCount-1) )
		HICON hIcon = NULL;
		if( ! m_bDoNotPaintIcons )
			hIcon = (HICON)
				CStatusBar::GetStatusBarCtrl().
					SendMessage( SB_GETICON, nPaneIdx );
		CString sPaneText = GetPaneText( nPaneIdx );
		OnErasePaneBackground(
			dc,
			nPaneIdx,
			rcPane
			);
		OnPaintPane(
			dc,
			nPaneIdx,
			rcPane,
			dwPaneStyle,
			hIcon,
			sPaneText.IsEmpty()
				? __EXT_MFC_SAFE_LPCTSTR( NULL )
				: __EXT_MFC_SAFE_LPCTSTR( LPCTSTR(sPaneText) )
				,
			GetPaneDrawTextFlags(nPaneIdx) // m_nDrawPaneTextFlags
			);
		if(		m_bCompleteRepaint
			&&	OnQueryDrawPaneSeparatorsInsteadOfBorders()
			&&	nPaneIdx > 0
			)
			OnPaintSeparator(
				dc,
				nPaneIdx,
				rcPanePrev,
				rcPane
				);
		rcPanePrev = rcPane;
	} // for( INT nPaneIdx = m_bOuterRectInFirstBand ? 0 : 1; nPaneIdx < m_nCount; nPaneIdx++ )
	if( m_bCompleteRepaint )
	{
		dc.SetTextColor( nOldTextColor );
		dc.SetBkMode( nOldBkMode );
		dc.SelectObject( pOldFont );
	} // if( m_bCompleteRepaint )

	// paint resizing gripper after all pane borders - this will allow gripper
	// to be on the top of all panes when window size is near its minimal size 
	if( !rcGrip.IsRectEmpty() )
	{
		dc.SelectClipRgn( NULL );
		OnPaintGripper( dc, rcGrip );
	} // if( !rcGrip.IsRectEmpty() )
}

bool CExtStatusControlBar::OnQueryGripperEnabledState() const
{
	ASSERT_VALID( this );
	if( CExtPopupMenuWnd::g_bFullScreenMode )
		return false;
CWnd * pWndParent = GetParent();
	if( pWndParent != NULL )
	{
		DWORD dwWndStyle = pWndParent->GetStyle();
		if( (dwWndStyle&WS_THICKFRAME) == 0 )
			return false;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

void CExtStatusControlBar::OnPaintGripper(
	CDC & dc,
	const CRect & rcGrip
	)
{
	ASSERT_VALID( this );
	if( ! m_bCompleteRepaint )
		dc.FillSolidRect( &rcGrip, m_clrStatusBk );
	if( ! OnQueryGripperEnabledState() )
		return;
CRect rcPaintGrip( rcGrip );
	rcPaintGrip.OffsetRect( -1, -1 );
	PmBridge_GetPM()->PaintResizingGripper(
		dc,
		rcPaintGrip,
		this
		);
}

//////////////////////////////////////////////////////////////////////////

void CExtStatusControlBar::OnPaintSeparator(
	CDC & dc,
	int nPaneIdxAfterSep,
	const CRect & rcPaneBefore,
	const CRect & rcPaneAfter
	)
{
	ASSERT_VALID( this );
	if(	PmBridge_GetPM()->StatusBar_PaintSeparator(
			dc,
			nPaneIdxAfterSep,
			rcPaneBefore,
			rcPaneAfter,
			this
			)
		)
		return;
CRect rcSeparator(
		rcPaneBefore.right,
		rcPaneAfter.top,
		rcPaneAfter.left,
		rcPaneAfter.bottom
		);
	rcSeparator.DeflateRect( 0, 1 );
	PmBridge_GetPM()->PaintSeparator(
		dc,
		rcSeparator,
		true,
		true,
		this,
		nPaneIdxAfterSep
		);
}

//////////////////////////////////////////////////////////////////////////

void CExtStatusControlBar::OnPaintEntireBackground(
	CDC & dc,
	const CRect & rcBk
	)
{
	ASSERT_VALID( this );
	if(		m_bCompleteRepaint
		&&	PmBridge_GetPM()->StatusBar_EraseBackground(
			dc,
			rcBk,
			this
			)
		)
		return;
	
bool bTransparent = false;
	if(		m_bCompleteRepaint
		&&	PmBridge_GetPM()->GetCb2DbTransparentMode(this)
		)
	{
		if( PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
			bTransparent = true;
	}
	if( ! bTransparent )
		dc.FillSolidRect( &rcBk, m_clrStatusBk );

	// invoke default handler
	if( ! m_bCompleteRepaint )
		CStatusBar::DefWindowProc( WM_PAINT, (WPARAM)dc.GetSafeHdc(), 0 );
}

//////////////////////////////////////////////////////////////////////////

void CExtStatusControlBar::OnErasePaneBackground(
	CDC & dc,
	int nPaneIdx,
	const CRect & rcPane
	)
{
	ASSERT_VALID( this );
COLORREF clrBack = OnQueryPaneBkColor( nPaneIdx );
	if( clrBack != COLORREF(-1L) )
	{
		dc.FillSolidRect( rcPane, clrBack );
		return;
	}
	PmBridge_GetPM()->StatusBar_ErasePaneBackground(
		dc,
		nPaneIdx,
		rcPane,
		this
		);
}

//////////////////////////////////////////////////////////////////////////

void CExtStatusControlBar::OnPaintPane(
	CDC & dc,
	int nPaneIdx,
	const CRect & rcPane,
	DWORD dwPaneStyle,
	HICON hIcon,
	__EXT_MFC_SAFE_LPCTSTR sPaneText, // by default it's text from the status bar control
	UINT nDrawTextFlags // by default m_nDrawPaneTextFlags is used
	)
{
	ASSERT_VALID( this );
	if( m_bCompleteRepaint )
	{
		bool bDisabled = 
			((dwPaneStyle & SBPS_DISABLED) != 0) ? true : false;
		COLORREF clrText = 
			OnQueryPaneTextColor( nPaneIdx, !bDisabled );
		if(	PmBridge_GetPM()->StatusBar_PaintPane(
				dc,
				nPaneIdx,
				rcPane,
				dwPaneStyle,
				hIcon,
				sPaneText,
				nDrawTextFlags,
				clrText,
				this
				)
			)
			return;
		CRect rcText( rcPane );
		if( hIcon != NULL )
		{
			CExtCmdIcon _icon;
			_icon.AssignFromHICON( hIcon, true );
			CSize sizeIcon = _icon.GetSize();
			ASSERT( sizeIcon.cx > 0 && sizeIcon.cy > 0 );
			rcText.left += sizeIcon.cx + 1;
			_icon.Paint(
				PmBridge_GetPM(),
				dc,
				rcPane.left + 1,
				rcPane.top + ( rcPane.Height() - sizeIcon.cy ) / 2,
				-1,
				-1
				);
		} // if( hIcon != NULL )
		int nPaneTextLen =
			(sPaneText == NULL)
				? int(0)
				: int(_tcslen(sPaneText))
				;
		if(		(!bDisabled)
			||	(bDisabled && (!m_bHideTextOnDisabledPanes))
			&&	nPaneTextLen > 0
			)
		{ // if text painting is allowed
			rcText.DeflateRect( 2, 0 );
			if( rcText.right > rcText.left )
			{
				COLORREF clrTextRestore = COLORREF(-1L);
				COLORREF clrTextCustom = OnQueryPaneTextColor( nPaneIdx, !bDisabled );
				if( clrTextCustom != COLORREF(-1L) )
					clrTextRestore = dc.SetTextColor( clrTextCustom );
				else if( bDisabled )
					clrTextRestore = dc.SetTextColor( m_clrPaneTextDisabled );
				CExtRichContentLayout::stat_DrawText(
					dc.m_hDC,
					LPCTSTR(sPaneText), nPaneTextLen,
					&rcText,
					nDrawTextFlags, 0
					);
				if( clrTextRestore != COLORREF(-1L) )
					dc.SetTextColor( clrTextRestore );
			} // if( rcText.right > rcText.left )
		} // if text painting is allowed
	} // if( m_bCompleteRepaint )
	if(		(!m_bCompleteRepaint)
		||	(	m_bCompleteRepaint
			&& (! OnQueryDrawPaneSeparatorsInsteadOfBorders() )
			)
		)
	{ // if paint pane borders
		bool bDrawPaneBorder = false;
		if( nPaneIdx == 0 )
		{
			bDrawPaneBorder = m_bOuterRectInFirstBand;
		} // if( nPaneIdx == 0 )
		else
		{
			if( (dwPaneStyle&SBPS_NOBORDERS) == 0 )
				bDrawPaneBorder = true;
		} // else from if( nPaneIdx == 0 )
		if( bDrawPaneBorder )
		{
//			if( m_bCompleteRepaint )
//			{
//				dc.Draw3dRect( &rcPane, m_clrPaneRect, m_clrPaneRect );
//			} // if( m_bCompleteRepaint )
//			else
			{
				CExtPaintManager::PAINTCONTROLBARBORDERSDATA _pcbbd(
					this,
					CExtPaintManager::__CB_INNER_STATUSBAR_ITEM,
					0,
					rcPane
					);
				PmBridge_GetPM()->PaintControlBarBorders( dc, _pcbbd );
			} // else from if( m_bCompleteRepaint )
		} // if( bDrawPaneBorder )
	} // if paint pane borders
	if( (!m_bCompleteRepaint) && g_PaintManager.m_UxTheme.IsAppThemed() )
	{
		CRect rcPaneInner( rcPane );
		rcPaneInner.DeflateRect( 1, 1 );
		dc.Draw3dRect( &rcPaneInner, m_clrStatusBk, m_clrStatusBk );
	} // if( (!m_bCompleteRepaint) && g_PaintManager.m_UxTheme.IsAppThemed() )
}

//////////////////////////////////////////////////////////////////////////

COLORREF CExtStatusControlBar::OnQueryPaneTextColor(
	int nPaneIdx,
	bool bEnabled
	) const
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nPaneIdx && nPaneIdx < GetPaneCount() );
_ED_ _data;
UINT nID = GetItemID( nPaneIdx );
	if( ! m_mapIdToExtData.Lookup( nID, _data ) )
		return COLORREF(-1L);
	return 
		bEnabled 
			? _data.m_clrTextNormal 
			: _data.m_clrTextDisabled;
}

COLORREF CExtStatusControlBar::OnQueryPaneBkColor(
	int nPaneIdx
	) const
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nPaneIdx && nPaneIdx < GetPaneCount() );
_ED_ _data;
UINT nID = GetItemID( nPaneIdx );
	if( ! m_mapIdToExtData.Lookup( nID, _data ) )
		return COLORREF(-1L);
	return _data.m_clrBack;
}

//////////////////////////////////////////////////////////////////////////

void CExtStatusControlBar::OnNcPaint() 
{
	ASSERT_VALID( this );
	if( !m_bCompleteRepaint )
	{
		Default();
		return;
	}
	_SyncStatusBarColors();
CRect rcClient, rcWnd;
	GetClientRect( rcClient );
	ClientToScreen( rcClient );
	GetWindowRect( rcWnd );
	rcClient.OffsetRect( -rcWnd.TopLeft() );
	rcWnd.OffsetRect( -rcWnd.TopLeft() );
CWindowDC dc( this );
	dc.ExcludeClipRect( &rcClient );
CExtPaintManager::PAINTCONTROLBARBORDERSDATA _pcbbd(
		this,
		CExtPaintManager::__CB_OUTER_STATUSBAR,
		m_dwStyle,
		rcWnd
		);
	PmBridge_GetPM()->PaintControlBarBorders( dc, _pcbbd );
}

//////////////////////////////////////////////////////////////////////////

void CExtStatusControlBar::DrawBorders( CDC * pDC, CRect& rc )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pDC );
CExtPaintManager::PAINTCONTROLBARBORDERSDATA _pcbbd(
		this,
		CExtPaintManager::__CB_OUTER_STATUSBAR,
		m_dwStyle,
		rc
		);
	PmBridge_GetPM()->PaintControlBarBorders( *pDC, _pcbbd );
}

//////////////////////////////////////////////////////////////////////////

void CExtStatusControlBar::_SyncStatusBarColors()
{
	ASSERT_VALID( this );
COLORREF clrStatusBkNeeded =
		PmBridge_GetPM()->GetColor(
			CExtPaintManager::CLR_3DFACE_OUT, this
			);
	if( m_clrStatusBk != clrStatusBkNeeded )
	{
		m_clrStatusBk = clrStatusBkNeeded;
		GetStatusBarCtrl().SetBkColor( m_clrStatusBk );
	} // if( m_clrStatusBk != clrStatusBkNeeded )
	m_clrGripper = PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_TEXT_OUT, this );
	m_clrPaneTextNormal = PmBridge_GetPM()->GetColor( COLOR_BTNTEXT, this );
	m_clrPaneTextDisabled = PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_TEXT_DISABLED, this );
	m_clrPaneRect = m_clrStatusBk;
	if( m_bCompleteRepaint )
		m_clrPaneRect = PmBridge_GetPM()->GetColor( COLOR_3DSHADOW, this );
}

//////////////////////////////////////////////////////////////////////////

void CExtStatusControlBar::OnSysColorChange() 
{
	ASSERT_VALID( this );
	CStatusBar::OnSysColorChange();
CExtPaintManager * pPM = PmBridge_GetPM();
	g_PaintManager.OnSysColorChange( this );
	g_CmdManager.OnSysColorChange( pPM, this );
	_SyncStatusBarColors();
}

//////////////////////////////////////////////////////////////////////////

void CExtStatusControlBar::OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection) 
{
	ASSERT_VALID( this );
//USES_CONVERSION;
//TRACE2("  ==> CExtStatusControlBar::OnSettingChange(0x%08X,\"%S\")\n", uFlags, (lpszSection == NULL) ? "NULL" : T2CA(lpszSection) );
	CStatusBar::OnSettingChange( uFlags, lpszSection );
CExtPaintManager * pPM = PmBridge_GetPM();
	g_PaintManager.OnSettingChange( this, uFlags, lpszSection );
	g_CmdManager.OnSettingChange( pPM, this, uFlags, lpszSection );
	_SyncStatusBarColors();
}

//////////////////////////////////////////////////////////////////////////

LRESULT CExtStatusControlBar::OnDisplayChange( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
//LRESULT lResult = CStatusBar::OnDisplayChange( wParam, lParam );
 CStatusBar::OnDisplayChange(wParam, LOWORD(lParam), HIWORD(lParam));
 LRESULT lResult = 0;

CExtPaintManager * pPM = PmBridge_GetPM();
	g_PaintManager.OnDisplayChange( this, (INT)wParam, CPoint(lParam) );
	g_CmdManager.OnDisplayChange( pPM, this, (INT)wParam, CPoint(lParam) );
	return lResult;
}

//////////////////////////////////////////////////////////////////////////

LRESULT CExtStatusControlBar::OnThemeChanged( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
LRESULT lResult = Default();
CExtPaintManager * pPM = PmBridge_GetPM();
	g_PaintManager.OnThemeChanged( this, wParam, lParam );
	g_CmdManager.OnThemeChanged( pPM, this, wParam, lParam );
	return lResult;
}

//////////////////////////////////////////////////////////////////////////

int CExtStatusControlBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	ASSERT_VALID( this );
	if( CStatusBar::OnCreate(lpCreateStruct) == -1 )
		return -1;

	CWnd::EnableToolTips( TRUE );
	CWnd::ModifyStyle( 0, WS_CLIPCHILDREN );
	
	ASSERT( GetSafeHwnd() != NULL );
HWND hWndParent = ::GetParent( GetSafeHwnd() );
	ASSERT( hWndParent != NULL );
	ASSERT( ::IsWindow(hWndParent) );
	VERIFY(
		::SetWindowPos(
			GetSafeHwnd(),
			HWND_TOP,
			0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE
				|SWP_NOREDRAW|SWP_NOACTIVATE|SWP_NOCOPYBITS
				|SWP_NOSENDCHANGING
			)
		);

	_SyncStatusBarColors();
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////

void CExtStatusControlBar::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	ASSERT_VALID( this );
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

CFrameWnd * pFrame = _GetDockingFrameImpl();
	if( pFrame == NULL )
	{
		CStatusBar::OnContextMenu( pWnd, point );
		return;
	}

	ASSERT_VALID( pFrame );
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
			CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_STATUSBAR_CTX,
			this,
			WM_CONTEXTMENU,
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

//////////////////////////////////////////////////////////////////////////

CFrameWnd * CExtStatusControlBar::_GetDockingFrameImpl()
{
	ASSERT_VALID( this );

CFrameWnd * pFrame = GetParentFrame();
	if( pFrame == NULL )
		return NULL;

	ASSERT_VALID( pFrame );
	if( pFrame->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)) )
	{
		pFrame = pFrame->GetParentFrame();
		//ASSERT_VALID( pFrame );
		ASSERT( pFrame != NULL );
		ASSERT( !pFrame->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)) );
	}

	return pFrame;
}

//////////////////////////////////////////////////////////////////////////

BOOL CExtStatusControlBar::OnEraseBkgnd(CDC* pDC) 
{
	ASSERT_VALID( this );
	pDC;
	if( ! m_bCompleteRepaint )
		return Default() ? TRUE : FALSE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

UINT CExtStatusControlBar::OnNcHitTest(CPoint point) 
{
	ASSERT_VALID( this );
	if ( OnQueryGripperEnabledState() ) // fixed by Thomas Fuchs
	{
		CRect rcGrip( 0, 0, 0, 0 );
		CWnd * pWndParentStatus = GetParent();
		if( pWndParentStatus != NULL )
		{
			WINDOWPLACEMENT _wp;
			::memset( (void*)&_wp, 0, sizeof(WINDOWPLACEMENT) );
			_wp.length = sizeof(WINDOWPLACEMENT);
			pWndParentStatus->GetWindowPlacement( &_wp );
			if(		_wp.showCmd != SW_SHOWMAXIMIZED
				//&&	(pWndParentStatus->GetStyle()&WS_BORDER) != 0
				)
			{
				MINMAXINFO _mmi;
				::memset( &_mmi, 0, sizeof(MINMAXINFO) );
				CExtPaintManager::monitor_parms_t _mp;
				CExtPaintManager::stat_GetMonitorParms( _mp, (CWnd*)pWndParentStatus );
				_mmi.ptMaxPosition.x = _mp.m_rcWorkArea.left;
				_mmi.ptMaxPosition.y = _mp.m_rcWorkArea.top;
				_mmi.ptMaxTrackSize.x = _mp.m_rcWorkArea.Width(); // ::GetSystemMetrics( SM_CXMAXTRACK );
				_mmi.ptMaxTrackSize.y = _mp.m_rcWorkArea.Height(); // ::GetSystemMetrics( SM_CYMAXTRACK );
				_mmi.ptMinTrackSize.x = ::GetSystemMetrics( SM_CXMINTRACK );
				_mmi.ptMinTrackSize.y = ::GetSystemMetrics( SM_CYMINTRACK );
				_mmi.ptMaxSize.x = _mmi.ptMaxTrackSize.x;
				_mmi.ptMaxSize.y = _mmi.ptMaxTrackSize.y;
				if( ((CWnd*)pWndParentStatus)->SendMessage( WM_GETMINMAXINFO, 0, LPARAM(&_mmi) ) == 0 )
				{
					if(		_mmi.ptMinTrackSize.x < _mmi.ptMaxTrackSize.x
						||	_mmi.ptMinTrackSize.y < _mmi.ptMaxTrackSize.y
						)
					{
						GetClientRect( &rcGrip );
						rcGrip.top = rcGrip.bottom - ::GetSystemMetrics( SM_CYVSCROLL );
						rcGrip.left = rcGrip.right - ::GetSystemMetrics( SM_CXVSCROLL );
						CPoint ptClient( point );
						ScreenToClient( &ptClient );
						if( rcGrip.PtInRect(ptClient) )
						{
							__EXT_MFC_LONG_PTR dwExStyle = ::__EXT_MFC_GetWindowLong( GetSafeHwnd(), GWL_EXSTYLE );
							bool bRTL = ( (dwExStyle & WS_EX_LAYOUTRTL) != 0 ) ? true : false;
							return bRTL 
								? HTBOTTOMLEFT 
								: HTBOTTOMRIGHT;
						}
					}
				} // if( ((CWnd*)pWndParentStatus)->SendMessage( WM_GETMINMAXINFO, 0, LPARAM(&_mmi) ) == 0 )
			} // if( _wp.showCmd != SW_SHOWMAXIMIZED ...
		} // if( pWndParentStatus != NULL )
	} // if ( OnQueryGripperEnabledState() )
UINT nHT = (UINT)CStatusBar::OnNcHitTest(point);
	if(		nHT != HTCLIENT
		&&	( ! OnQueryGripperEnabledState() )
		)
		nHT = HTCLIENT;
	return nHT;
}

void CExtStatusControlBar::OnSize(UINT nType, int cx, int cy) 
{
	CStatusBar::OnSize(nType, cx, cy);
	RepositionControls();
}

void CExtStatusControlBar::RepositionControls(
	bool bUpdate // = true
	)
{
	ASSERT_VALID( this );
int nPaneIndex, nPaneCount = (int)m_arrPaneControls.GetSize();
HDWP _hDWP = ::BeginDeferWindowPos( nPaneCount );
CRect rcClient;
	GetClientRect( &rcClient );
	for( nPaneIndex = 0; nPaneIndex < nPaneCount; nPaneIndex++ )
	{
		int iIndex =
			CommandToIndex(
				m_arrPaneControls[ nPaneIndex ] -> nID
				);
		HWND hWnd =
			m_arrPaneControls[ nPaneIndex ] -> hWnd;
		CRect rcPane;
		GetItemRect( iIndex, &rcPane );
		// CStatusBar::GetItemRect() sometimes returns invalid size 
		// of the last pane - we will re-compute it
		int cx = ::GetSystemMetrics( SM_CXEDGE );
		DWORD dwPaneStyle = GetPaneStyle( iIndex );
		if( iIndex == (m_nCount-1) )
		{
			if( (dwPaneStyle & SBPS_STRETCH ) == 0 )
			{
				UINT nID, nStyle;
				int  cxWidth;
				GetPaneInfo( iIndex, nID, nStyle, cxWidth );
				rcPane.right = rcPane.left + cxWidth + cx*3;
			} // if( (dwPaneStyle & SBPS_STRETCH ) == 0 )
			else
			{
				CRect rcClient;
				GetClientRect( &rcClient );
				rcPane.right = rcClient.right;
				if( (GetStyle() & SBARS_SIZEGRIP) == SBARS_SIZEGRIP )
				{
					int cxSmIcon = ::GetSystemMetrics( SM_CXSMICON );
					rcPane.right -= cxSmIcon + cx;
				} // if( (GetStyle() & SBARS_SIZEGRIP) == SBARS_SIZEGRIP )
			} // else from if( (dwPaneStyle & SBPS_STRETCH ) == 0 )
		} // if( iIndex == (m_nCount-1) )
		if(		( GetPaneStyle(iIndex)&SBPS_NOBORDERS ) == 0
			&&	(! OnQueryDrawPaneSeparatorsInsteadOfBorders() )
			)
		{
			rcPane.DeflateRect( cx, cx );
		}
		else
		{
			rcPane.DeflateRect( cx, 1, cx, 1 );
		}
		if( hWnd != NULL && ::IsWindow( hWnd ) )
		{
			_hDWP =
				::DeferWindowPos(
					_hDWP, 
					hWnd, 
					NULL, 
					rcPane.left,
					rcPane.top, 
					rcPane.Width(), 
					rcPane.Height(),
					SWP_NOZORDER|SWP_NOOWNERZORDER
						|SWP_NOACTIVATE //|SWP_SHOWWINDOW
						|SWP_FRAMECHANGED
						|SWP_NOCOPYBITS
					);
		} // if( hWnd != NULL && ::IsWindow( hWnd ) )
	} // for( nPaneIndex = 0; nPaneIndex < nPaneCount; nPaneIndex++ )
	VERIFY( ::EndDeferWindowPos( _hDWP ) );
	if( bUpdate )
	{
		Invalidate();
		UpdateWindow();
	}
};

//////////////////////////////////////////////////////////////////////////

bool CExtStatusControlBar::AddPane(
	 UINT nID,	// ID of the  pane
	 int nIndex	// index of the pane
	 )
{
	ASSERT_VALID( this );
	if( nIndex < 0 || nIndex > m_nCount )
	{
		//ASSERT( FALSE );
		return false;
	}
	if( CommandToIndex(nID) != -1)
	{
		//ASSERT( FALSE );
		return false;
	}

	// disable painting
	m_bLockPainting = true;

CArray <_STATUSBAR_PANE_* , _STATUSBAR_PANE_* > arrPanesTmp;
CArray < HICON, HICON > arrPaneIcons;
CStatusBarCtrl & _ctrl = GetStatusBarCtrl();
int iIndex;
	for( iIndex = 0; iIndex < (m_nCount+1); iIndex++ )
	{
		_STATUSBAR_PANE_ * pNewPane = new _STATUSBAR_PANE_;
		if( iIndex == nIndex )
		{
			pNewPane->nID    = nID;
			pNewPane->nStyle = SBPS_NORMAL;
			arrPaneIcons.Add( NULL );
		} // if( iIndex == nIndex )
		else
		{
			int idx = iIndex;
			if( iIndex > nIndex )
				idx--;
			_STATUSBAR_PANE_ * pOldPane = GetPanePtr(idx);
			pNewPane->cxText  = pOldPane->cxText;
			pNewPane->nFlags  = pOldPane->nFlags;
			pNewPane->nID     = pOldPane->nID;
			pNewPane->nStyle  = pOldPane->nStyle;
			pNewPane->strText = pOldPane->strText;
			HICON hIcon = (HICON)
				_ctrl.SendMessage(
					SB_GETICON,
					WPARAM(idx)
					);
			if( hIcon != NULL )
			{
				CExtCmdIcon _iconClone;
				_iconClone.AssignFromHICON( hIcon, true );
				hIcon = _iconClone.ExtractHICON();
				ASSERT( hIcon != NULL );
			} // if( hIcon != NULL )
			arrPaneIcons.Add( hIcon );
		} // else from if( iIndex == nIndex )
		arrPanesTmp.Add( pNewPane );
	} // for( iIndex = 0; iIndex < (m_nCount+1); iIndex++ )
int nPaneCount = (int)arrPanesTmp.GetSize();
UINT * lpIDArray = new UINT[ nPaneCount ];
	for( iIndex = 0; iIndex < nPaneCount; iIndex++ )
		lpIDArray[iIndex] = arrPanesTmp[iIndex]->nID;
	// set the indicators 
	SetIndicators( lpIDArray, nPaneCount );
	// free memory, reset icons
	for( iIndex = 0; iIndex < nPaneCount; iIndex++ )
	{
		_STATUSBAR_PANE_* pPane = arrPanesTmp[iIndex];
		if( iIndex != nIndex )
			PaneInfoSet(iIndex, pPane);
		if( pPane ) 
			delete pPane;
		HICON hIcon = arrPaneIcons[iIndex];
		_ctrl.SetIcon( iIndex, hIcon );
	} // for( iIndex = 0; iIndex < nPaneCount; iIndex++ )
	arrPanesTmp.RemoveAll();
	arrPaneIcons.RemoveAll();
	if( lpIDArray ) 
		delete [] lpIDArray;

	// enable painting
	m_bLockPainting = false;

	RepositionControls();
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CExtStatusControlBar::_RemovePaneImpl(
	UINT nID, // ID of the pane
	bool bRefresh
	)
{
	ASSERT_VALID( this );
	if(			CommandToIndex(nID) == -1
		//||	m_nCount == 1
		)
	{
		ASSERT( FALSE );
		return false;
	}

	// disable painting
	m_bLockPainting = true;

	m_mapIdToExtData.RemoveKey( nID );
	
CArray < _STATUSBAR_PANE_* , _STATUSBAR_PANE_ * > arrPanesTmp;
CArray < HICON, HICON > arrPaneIcons;
CStatusBarCtrl & _ctrl = GetStatusBarCtrl();
int nIndex;
	for( nIndex = 0; nIndex < m_nCount; nIndex++ )
	{
		_STATUSBAR_PANE_ * pOldPane = GetPanePtr(nIndex);
		HICON hIcon = (HICON)
			_ctrl.SendMessage(
				SB_GETICON,
				WPARAM(nIndex)
				);
		if( pOldPane->nID == nID )
		{
			if( hIcon != NULL )
				_ctrl.SetIcon( nIndex, (HICON)NULL );
			continue;
		} // if( pOldPane->nID == nID )
		_STATUSBAR_PANE_ * pNewPane = new _STATUSBAR_PANE_;
		pNewPane->cxText  = pOldPane->cxText;
		pNewPane->nFlags  = pOldPane->nFlags;
		pNewPane->nID     = pOldPane->nID;
		pNewPane->nStyle  = pOldPane->nStyle;
		pNewPane->strText = pOldPane->strText;
		arrPanesTmp.Add( pNewPane );
		if( hIcon != NULL )
		{
			CExtCmdIcon _iconClone;
			_iconClone.AssignFromHICON( hIcon, true );
			hIcon = _iconClone.ExtractHICON();
			ASSERT( hIcon != NULL );
		} // if( hIcon != NULL )
		arrPaneIcons.Add( hIcon );
	} // for( nIndex = 0; nIndex < m_nCount; nIndex++ )
UINT * lpIDArray = NULL;
	if( arrPanesTmp.GetSize() > 0 )
	{
		lpIDArray = new UINT[ arrPanesTmp.GetSize() ];
		for( nIndex = 0; nIndex < arrPanesTmp.GetSize(); nIndex++ )
			lpIDArray[nIndex] = arrPanesTmp[nIndex]->nID;
	} // if( arrPanesTmp.GetSize() > 0 )
	// set the indicators
	SetIndicators( lpIDArray, (int)arrPanesTmp.GetSize() );
	// free memory, reset icons
	for( nIndex = 0; nIndex < arrPanesTmp.GetSize(); nIndex++ )
	{
		_STATUSBAR_PANE_* pPane = arrPanesTmp[nIndex];
		PaneInfoSet(nIndex, pPane);
		if( pPane ) 
			delete pPane;
		HICON hIcon = arrPaneIcons[nIndex];
		_ctrl.SetIcon( nIndex, hIcon );
	} // for( nIndex = 0; nIndex < arrPanesTmp.GetSize(); nIndex++ )
	RemovePaneControl( nID );
	arrPanesTmp.RemoveAll();
	arrPaneIcons.RemoveAll();
	if( lpIDArray != NULL ) 
		delete [] lpIDArray;
	if( bRefresh )
		RepositionControls();

	// enable painting
	m_bLockPainting = false;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CExtStatusControlBar::RemovePane(
	UINT nID	// ID of the pane
	)
{
	return _RemovePaneImpl( nID, true );
}

//////////////////////////////////////////////////////////////////////////

int CExtStatusControlBar::_RemovePanesImpl(
	int nIndexToRemove,
	int nCountToRemove,
	bool bRefresh
	)
{
	ASSERT_VALID( this );
	if(		nIndexToRemove < 0
		||	nCountToRemove <= 0
		)
		return 0;
int nPaneCount = GetPaneCount();
	ASSERT( nPaneCount >= 0 );
	if( nIndexToRemove >= nPaneCount )
		return 0;
	if( (nIndexToRemove + nCountToRemove) > nPaneCount )
	{
		nCountToRemove = nPaneCount - nIndexToRemove;
		ASSERT( nCountToRemove > 0 );
	}
int nCountRemoved = 0;
	for( int i = 0; i < nCountToRemove; i++ )
	{
		UINT nID = GetItemID( nIndexToRemove );
		if( !_RemovePaneImpl( nID, false ) )
			break;
		nCountRemoved ++;
	} // for( int i = 0; i < nCountToRemove; i++ )
	if( bRefresh && nCountRemoved > 0 )
		RepositionControls();
	return nCountRemoved;
}

//////////////////////////////////////////////////////////////////////////

int CExtStatusControlBar::RemovePanes(
	int nIndexToRemove,
	int nCountToRemove
	)
{
	return _RemovePanesImpl( nIndexToRemove, nCountToRemove, true );
}

//////////////////////////////////////////////////////////////////////////

bool CExtStatusControlBar::RemovePaneControl( UINT nID )
{
	ASSERT_VALID( this );
	for( INT i = 0; i < m_arrPaneControls.GetSize(); i++ )
	{
		if( m_arrPaneControls[i]->nID == nID )
		{
			if(		m_arrPaneControls[i]->hWnd != NULL
				&&	::IsWindow(m_arrPaneControls[i]->hWnd) 
				) 
			{
				::ShowWindow( m_arrPaneControls[i]->hWnd, SW_HIDE ); 
				if( m_arrPaneControls[i]->bAutoDestroy ) 
					::DestroyWindow( m_arrPaneControls[i]->hWnd );
			}
			_STATUSBAR_PANE_CTRL_ *pPaneCtrl = m_arrPaneControls[i];
			if( pPaneCtrl )
				delete pPaneCtrl;
			m_arrPaneControls.RemoveAt(i);
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

bool CExtStatusControlBar::SetPaneControl(CWnd* pWnd, UINT nID, bool bAutoDestroy)
{
	ASSERT_VALID( this );
	return SetPaneControl( pWnd->GetSafeHwnd(), nID, bAutoDestroy);
}

bool CExtStatusControlBar::SetPaneControl(HWND hWnd, UINT nID, bool bAutoDestroy)
{
	ASSERT_VALID( this );

	if (CommandToIndex (nID) == -1)
	{
		return false; // pane doesn't exists
	}
	
	RemovePaneControl( nID ); // remove previous control if it exists

	_STATUSBAR_PANE_CTRL_* pPaneCtrl = new _STATUSBAR_PANE_CTRL_;
	pPaneCtrl->nID         = nID;
	pPaneCtrl->hWnd        = hWnd;
	pPaneCtrl->bAutoDestroy = bAutoDestroy;
	
	m_arrPaneControls.Add(pPaneCtrl);
	RepositionControls();
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CExtStatusControlBar::PaneInfoGet(int nIndex, _STATUSBAR_PANE_* pPane) const
{
	ASSERT_VALID( this );
	ASSERT( pPane != NULL );
	if( nIndex < m_nCount  && nIndex >= 0 )
	{
		GetPaneInfo( nIndex,  pPane->nID, pPane->nStyle, pPane->cxText );
		CString strPaneText;
		GetPaneText( nIndex , strPaneText );
		pPane->strText = LPCTSTR(strPaneText);
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

bool CExtStatusControlBar::PaneInfoSet(int nIndex, _STATUSBAR_PANE_* pPane)
{
	ASSERT_VALID( this );
	ASSERT( pPane != NULL );
	if( nIndex < m_nCount  && nIndex >= 0 )
	{
		SetPaneInfo( nIndex, pPane->nID, pPane->nStyle, pPane->cxText );
		SetPaneText( nIndex, LPCTSTR( pPane->strText) );
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

UINT CExtStatusControlBar::GetPaneDrawTextFlags(
	int nIndex
	) const
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return m_nDrawPaneTextFlags;
	ASSERT( 0 <= nIndex && nIndex < GetPaneCount() );
_ED_ _data;
UINT nID = GetItemID( nIndex );
	if( ! m_mapIdToExtData.Lookup( nID, _data ) )
		return m_nDrawPaneTextFlags;
	return _data.m_nDTF;
}

//////////////////////////////////////////////////////////////////////////

void CExtStatusControlBar::SetPaneDrawTextFlags(
	int nIndex,
	UINT _nDTF
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
	ASSERT( 0 <= nIndex && nIndex < GetPaneCount() );
UINT nID = GetItemID( nIndex );
_ED_ _data;
	if( m_mapIdToExtData.Lookup( nID, _data ) )
	{
		if( _nDTF == _data.m_nDTF )
			return;
	} // if( m_mapIdToExtData.Lookup( nID, nDTF ) )
	_data.m_nDTF = _nDTF;
	m_mapIdToExtData.SetAt( nID, _data );
	Invalidate();
}

//////////////////////////////////////////////////////////////////////////

CExtStatusControlBar::e_StatusPaneBackgroundAccent_t CExtStatusControlBar::OnQueryPaneBackgroundAccent(
	int nIndex
	) const
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return __ESPBA_DARK;
	if( !( 0 <= nIndex && nIndex < GetPaneCount() ) )
		return __ESPBA_DARK;
e_StatusPaneBackgroundAccent_t eSPBA = GetPaneBackgroundAccent( nIndex );
	if( eSPBA == __ESPBA_AUTOMATIC )
	{
		eSPBA = __ESPBA_DARK;
		UINT nID, nStyle;
		int  cxWidth;
		GetPaneInfo( nIndex, nID, nStyle, cxWidth );
		if( nID == 0 )
			eSPBA = __ESPBA_LIGHT;
	}
	return eSPBA;
}

//////////////////////////////////////////////////////////////////////////

CExtStatusControlBar::e_StatusPaneBackgroundAccent_t CExtStatusControlBar::GetPaneBackgroundAccent(
	int nIndex
	) const
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return __ESPBA_AUTOMATIC;
	ASSERT( 0 <= nIndex && nIndex < GetPaneCount() );
_ED_ _data;
UINT nID = GetItemID( nIndex );
	if( ! m_mapIdToExtData.Lookup( nID, _data ) )
		return __ESPBA_AUTOMATIC;
	return _data.m_eSPBA;
}

//////////////////////////////////////////////////////////////////////////

void CExtStatusControlBar::SetPaneBackgroundAccent(
	int nIndex,
	CExtStatusControlBar::e_StatusPaneBackgroundAccent_t eSPBA
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
	ASSERT( __ESPBA_MIN_VALUE <= eSPBA && eSPBA <= __ESPBA_MAX_VALUE );
UINT nID = GetItemID( nIndex );
_ED_ _data;
	if( m_mapIdToExtData.Lookup( nID, _data ) )
	{
		if( eSPBA == _data.m_eSPBA )
			return;
	} // if( m_mapIdToExtData.Lookup( nID, nDTF ) )
	_data.m_eSPBA = eSPBA;
	m_mapIdToExtData.SetAt( nID, _data );
	Invalidate();
}

//////////////////////////////////////////////////////////////////////////

UINT CExtStatusControlBar::GetItemID( int nIndex ) const
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return 0;
	ASSERT( 0 <= nIndex && nIndex < GetPaneCount() );
_STATUSBAR_PANE_ pane;
	PaneInfoGet( nIndex, &pane );
	return pane.nID;
}

//////////////////////////////////////////////////////////////////////////

CExtStatusControlBar::_STATUSBAR_PANE_* CExtStatusControlBar::GetPanePtr(int nIndex) const
{
	ASSERT_VALID( this );
	ASSERT((nIndex >= 0 && nIndex < m_nCount) || m_nCount == 0);
	return ((_STATUSBAR_PANE_*)m_pData) + nIndex;
}

CExtStatusControlBar::_STATUSBAR_PANE_ * CExtStatusControlBar::_GetPanePtr( int nIndex ) const
{
	ASSERT((nIndex >= 0 && nIndex < m_nCount) || m_nCount == 0);
	return ((_STATUSBAR_PANE_*)m_pData) + nIndex;
}

//////////////////////////////////////////////////////////////////////////

BOOL CExtStatusControlBar::SetIndicators(
	const UINT * lpIDArray,
	int nIDCount
	)
{
	ASSERT_VALID( this );
	ASSERT( nIDCount >= 0 );
	ASSERT(
			lpIDArray == NULL
		||	::AfxIsValidAddress(
				lpIDArray,
				sizeof(UINT) * nIDCount,
				FALSE
				)
			);
	ASSERT( ::IsWindow(m_hWnd) );
	if(	! AllocElements(
			nIDCount,
			sizeof(_STATUSBAR_PANE_)
			)
		)
		return FALSE;
	ASSERT( nIDCount == m_nCount );
	BOOL bResult = TRUE;
	if( lpIDArray != NULL )
	{
		HFONT hFont = (HFONT)SendMessage( WM_GETFONT );
		CClientDC dcScreen( NULL );
		HGDIOBJ hOldFont = NULL;
		if( hFont != NULL )
			hOldFont = dcScreen.SelectObject( hFont );
		_STATUSBAR_PANE_ * pPane = _GetPanePtr( 0 );
		for( int nPaneIndex = 0; nPaneIndex < nIDCount; nPaneIndex++ )
		{
			pPane->nID = * lpIDArray++;
			pPane->nFlags |= SBPF_UPDATE;
			if( pPane->nID != 0 )
			{
				if( ! g_ResourceManager->LoadString( pPane->strText, pPane->nID ) )
				{
					// bResult = FALSE;
					// break;
					pPane->strText = _T("");
				}
				pPane->cxText = CExtPaintManager::stat_CalcTextDimension( dcScreen, *CFont::FromHandle((HFONT)hFont), pPane->strText ).Width();
				//ASSERT( pPane->cxText >= 0 );
				if( ! SetPaneText( nPaneIndex, pPane->strText, FALSE ) )
				{
					bResult = FALSE;
					break;
				} // if( ! SetPaneText( nPaneIndex, pPane->strText, FALSE ) )
			} // if( pPane->nID != 0 )
			else
			{
				pPane->cxText =
					::GetSystemMetrics(SM_CXSCREEN) / 4;
				if( nPaneIndex == 0 )
					pPane->nStyle |=
						SBPS_STRETCH|SBPS_NOBORDERS;
			} // else from if( pPane->nID != 0 )
			pPane ++;
		} // for( int nPaneIndex = 0; nPaneIndex < nIDCount; nPaneIndex++ )
		if( hOldFont != NULL )
			dcScreen.SelectObject( hOldFont );
	} // if( lpIDArray != NULL )
	UpdateAllPanes( TRUE, TRUE );
	return bResult;
}

//////////////////////////////////////////////////////////////////////////

CExtSafeString CExtStatusControlBar::GetTipText( int nPane ) const
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return CExtSafeString( _T("") );
	ASSERT( 0 <= nPane && nPane < GetPaneCount() );
_ED_ _data;
UINT nID = GetItemID( nPane );
	if( ! m_mapIdToExtData.Lookup( nID, _data ) )
		return CExtSafeString( _T("") );
	return _data.m_strTipText;
}

//////////////////////////////////////////////////////////////////////////

void CExtStatusControlBar::SetTipText( int nPane, __EXT_MFC_SAFE_LPCTSTR pszTipText )
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
	CWnd::CancelToolTips();
	if( pszTipText == NULL )
		pszTipText = _T("");
	ASSERT( 0 <= nPane && nPane < GetPaneCount() );
UINT nID = GetItemID( nPane );
_ED_ _data;
	if( m_mapIdToExtData.Lookup( nID, _data ) )
	{
		if( _tcscmp(
				pszTipText,
				( _data.m_strTipText.IsEmpty() ? _T("") : LPCTSTR(_data.m_strTipText) )
				) == 0
			)
			return;
	} // if( m_mapIdToExtData.Lookup( nID, nDTF ) )
	_data.m_strTipText = pszTipText;
	m_mapIdToExtData.SetAt( nID, _data );
	Invalidate();
}

//////////////////////////////////////////////////////////////////////////

void CExtStatusControlBar::SetPaneBkColor( 
	int nPane, 
	COLORREF clrBk // = COLORREF(-1L)
	)
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nPane && nPane < GetPaneCount() );
UINT nID = GetItemID( nPane );
_ED_ _data;
	if(		m_mapIdToExtData.Lookup( nID, _data ) 
		&&	_data.m_clrBack == clrBk
		)
		return;
	_data.m_clrBack = clrBk;
	m_mapIdToExtData.SetAt( nID, _data );
	Invalidate();
}

void CExtStatusControlBar::SetPaneTextColor(
	int nPane, 
	bool bEnabled,
	COLORREF clrText // = COLORREF(-1L)
	)
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nPane && nPane < GetPaneCount() );
UINT nID = GetItemID( nPane );
_ED_ _data;
	if(		m_mapIdToExtData.Lookup( nID, _data ) 
		&&	(	(bEnabled && _data.m_clrTextNormal == clrText)
			||	((!bEnabled) && _data.m_clrTextDisabled == clrText)
			)
		)
		return;
	if( bEnabled )
		_data.m_clrTextNormal = clrText;
	else
		_data.m_clrTextDisabled = clrText;
	m_mapIdToExtData.SetAt( nID, _data );
	Invalidate();
}

COLORREF CExtStatusControlBar::GetPaneBkColor( 
	int nPane
	) const
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nPane && nPane < GetPaneCount() );
	return OnQueryPaneBkColor( nPane );
}

COLORREF CExtStatusControlBar::GetPaneTextColor(
	int nPane,
	bool bEnabled
	) const
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nPane && nPane < GetPaneCount() );
	return OnQueryPaneTextColor( nPane, bEnabled );
}

//////////////////////////////////////////////////////////////////////////

int CExtStatusControlBar::HitTestStatusPane(
	CPoint ptClient
	) const
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return -1;
int nPaneCount = GetPaneCount();
	if( nPaneCount == 0 )
		return -1;
CPoint ptScreen( ptClient );
	ClientToScreen( &ptScreen );
HWND hWndFromPoint = ::WindowFromPoint( ptScreen );
	if(		hWndFromPoint != m_hWnd
		&&	(! ::IsChild(m_hWnd, hWndFromPoint) )
		)
		return -1;
	for( int nPaneIndex = 0; nPaneIndex < nPaneCount; nPaneIndex++ )
	{
		CRect rcPane;
		GetItemRect( nPaneIndex, &rcPane );
		if( rcPane.PtInRect(ptClient) )
			return nPaneIndex;
	} // for( int nPaneIndex = 0; nPaneIndex < nPaneCount; nPaneIndex++ )
	return -1;
}

//////////////////////////////////////////////////////////////////////////

__EXT_MFC_INT_PTR CExtStatusControlBar::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	) const
{
	ASSERT_VALID( this );
	if(		CExtToolControlBar::g_bMenuTracking
		||	CExtPopupMenuWnd::IsMenuTracking()
		||	GetSafeHwnd() == NULL
		)
		return -1;

	if( m_nAdvancedTipStyle != INT(CExtPopupMenuTipWnd::__ETS_NONE) )
		return -1;

int nPaneIndex = HitTestStatusPane( point );
	if( nPaneIndex < 0 )
		return -1;
__EXT_MFC_INT_PTR nHit = (__EXT_MFC_INT_PTR)GetItemID( nPaneIndex );
	if( pTI != NULL )
	{
		GetItemRect( nPaneIndex, &(pTI->rect) );
		pTI->uId = nHit;
		pTI->hwnd = m_hWnd;
		CString strToolTipText = GetTipText( nPaneIndex );
		if( ! strToolTipText.IsEmpty() )
			pTI->lpszText = _tcsdup( strToolTipText );
		if( pTI->lpszText == NULL )
			pTI->lpszText = LPSTR_TEXTCALLBACK;
	} // if( pTI != NULL )
	return nHit;
}

void CExtStatusControlBar::DrawPaneSeparatorsSet( 
	bool bSet // = true 
	)
{
	ASSERT_VALID( this );
	m_bDrawPaneSeparatorsInsteadOfBorders = bSet;
	RepositionControls();
}
bool CExtStatusControlBar::DrawPaneSeparatorsGet() const
{
	ASSERT_VALID( this );
	return m_bDrawPaneSeparatorsInsteadOfBorders;
}

void CExtStatusControlBar::OuterRectInFirstBandSet( 
	bool bSet // = true 
	)
{
	ASSERT_VALID( this );
	m_bOuterRectInFirstBand = bSet;
	Invalidate();
	UpdateWindow();
}
bool CExtStatusControlBar::OuterRectInFirstBandGet() const
{
	ASSERT_VALID( this );
	return m_bOuterRectInFirstBand;
}

void CExtStatusControlBar::HideTextOnDisabledPanesSet( 
	bool bSet // = true
	)
{
	ASSERT_VALID( this );
	m_bHideTextOnDisabledPanes = bSet;
	Invalidate();
	UpdateWindow();
}
bool CExtStatusControlBar::HideTextOnDisabledPanesGet() const
{
	ASSERT_VALID( this );
	return m_bHideTextOnDisabledPanes;
}

void CExtStatusControlBar::EnableControl( 
	int nIndex, 
	bool bEnable // = true 
	)
{
	ASSERT_VALID( this );
	UINT uItemID = GetItemID(nIndex);

	for ( int i = 0; i < m_arrPaneControls.GetSize(); i++ )
	{
		if( uItemID == m_arrPaneControls[i]->nID )
		{
			if( m_arrPaneControls[i]->hWnd && ::IsWindow(m_arrPaneControls[i]->hWnd) ) 
			{
				::EnableWindow(m_arrPaneControls[i]->hWnd, bEnable); 
				Invalidate();
				UpdateWindow();
			}
		}
	}
}

void CExtStatusControlBar::SetPaneInfo(
	int nIndex, 
	UINT nID, 
	UINT nStyle, 
	int cxWidth
	)
{
	ASSERT_VALID( this );
	CStatusBar::SetPaneInfo(nIndex, nID, nStyle, cxWidth);
	bool bEnabled = ((nStyle&SBPS_DISABLED) == 0);
	EnableControl(nIndex, bEnabled);
}

void CExtStatusControlBar::SetPaneStyle(
	int nIndex, 
	UINT nStyle
	)
{
	ASSERT_VALID( this );
	CStatusBar::SetPaneStyle(nIndex, nStyle);
	bool bEnabled = ((nStyle&SBPS_DISABLED) == 0);
	EnableControl(nIndex, bEnabled);
}

int CExtStatusControlBar::GetPaneCount() const
{
	ASSERT_VALID( this );
	return m_nCount;
}

void CExtStatusControlBar::SetPaneWidth(int nIndex, int nWidth)
{
	ASSERT_VALID( this );
	_STATUSBAR_PANE_ pane;
	PaneInfoGet(nIndex, &pane);
	pane.cxText = nWidth;
	PaneInfoSet(nIndex, &pane);
}

void CExtStatusControlBar::WidthFromContent( int nIndex )
{
	ASSERT_VALID( this );
	ASSERT((nIndex >= 0 && nIndex < m_nCount) || m_nCount == 0);
_STATUSBAR_PANE_ * pPane = GetPanePtr( nIndex );
	ASSERT( pPane != NULL );
	if( pPane == NULL )
		return;
CRect rcText(0,0,0,0);
	if( !pPane->strText.IsEmpty() )
	{
//		HFONT hFont = (HFONT)SendMessage( WM_GETFONT );
		HFONT hFont = (HFONT)PmBridge_GetPM()->m_FontNormal.GetSafeHandle();
		CClientDC dcScreen( NULL );
		HGDIOBJ hOldFont = NULL;
		if( hFont != NULL )
			hOldFont = dcScreen.SelectObject( hFont );
		CExtRichContentLayout::stat_DrawText(
			dcScreen.m_hDC,
			LPCTSTR(pPane->strText), pPane->strText.GetLength(),
			&rcText,
			GetPaneDrawTextFlags( nIndex ) | DT_CALCRECT, 0
			);
		if( hOldFont != NULL )
			dcScreen.SelectObject( hOldFont );
	} // if( !pPane->strText.IsEmpty() )
CStatusBarCtrl & _ctrl = GetStatusBarCtrl();
HICON hIcon = (HICON)
		_ctrl.SendMessage(
			SB_GETICON,
			WPARAM(nIndex)
			);
	// ICON 2.53
CSize szIcon = CSize(0,0);
// 	if( hIcon != NULL )
// 		szIcon = CExtCmdIcon::stat_GetIconSize( hIcon );
	if( hIcon != NULL )
	{
		CExtCmdIcon _icon;
		_icon.AssignFromHICON( hIcon, false );
		szIcon = _icon.GetSize();
	}
	SetPaneWidth( 
		nIndex,
		szIcon.cx + rcText.Width() + 2
		);
	RepositionControls();
}

void CExtStatusControlBar::OnCancelMode() 
{
	CExtPopupMenuWnd::CancelMenuTracking();
	CWnd::CancelToolTips();
	
	CStatusBar::OnCancelMode();
}

CExtPopupMenuTipWnd * CExtStatusControlBar::OnAdvancedPopupMenuTipWndGet() const
{
	if( m_nAdvancedTipStyle == INT(CExtPopupMenuTipWnd::__ETS_NONE) )
		return NULL;
	return (&( CExtPopupMenuSite::g_DefPopupMenuSite.GetTip() ));
}

void CExtStatusControlBar::OnAdvancedPopupMenuTipWndDisplay(
	CExtPopupMenuTipWnd & _ATTW,
	const RECT & rcExcludeArea,
	__EXT_MFC_SAFE_LPCTSTR strTipText
	) const
{
	ASSERT_VALID( this );
	if(		strTipText == NULL
		||	_tcscmp( strTipText, _T("") ) == 0 
		)
		return;
	_ATTW.SetText( LPCTSTR(strTipText) );
	_ATTW.SetTipStyle( (CExtPopupMenuTipWnd::e_tip_style_t) m_nAdvancedTipStyle );
	_ATTW.Show( (CWnd*) this, rcExcludeArea );
}

void CExtStatusControlBar::OnMouseMove( UINT nFlags, CPoint point )
{
	CStatusBar::OnMouseMove( nFlags, point );

	if( (! CExtPopupMenuWnd::IsMenuTracking() ) )
	{
		CExtPopupMenuTipWnd * pATTW =
			OnAdvancedPopupMenuTipWndGet();
		if( pATTW != NULL )
		{
			int nPaneIndex = HitTestStatusPane( point );
			if( nPaneIndex < 0 )
				return;
			CRect rcPane;
			GetItemRect( nPaneIndex, &rcPane );
			CString strToolTipText = GetTipText( nPaneIndex );
			if( ! strToolTipText.IsEmpty() )
			{
				CRect rcArea = rcPane;
				ClientToScreen( &rcArea );
				OnAdvancedPopupMenuTipWndDisplay(
					*pATTW,
					rcArea,
					LPCTSTR( strToolTipText )
					);
			}
		} // if( pATTW != NULL )
	}
}

LRESULT CExtStatusControlBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(	(	message == WM_PRINT
		||	message == WM_PRINTCLIENT 
		)
		&&	m_bCompleteRepaint
		)
	{
		CDC * pDC = CDC::FromHandle( (HDC) wParam );
		
		CRect rcWnd, rcClient;
		GetWindowRect( &rcWnd );
		GetClientRect( &rcClient );
		ClientToScreen( rcClient );
		rcClient.OffsetRect( -rcWnd.TopLeft() );
		rcWnd.OffsetRect( -rcWnd.TopLeft() );

		if( (lParam&PRF_NONCLIENT) != 0 )
		{
			pDC->ExcludeClipRect(rcClient);
			CExtPaintManager::PAINTCONTROLBARBORDERSDATA _pcbbd(
				this,
				CExtPaintManager::__CB_OUTER_STATUSBAR,
				m_dwStyle,
				rcWnd
				);
			PmBridge_GetPM()->PaintControlBarBorders( *pDC, _pcbbd );
			pDC->SelectClipRgn( NULL );
		}
	
		if( (lParam&(PRF_CLIENT|PRF_ERASEBKGND)) != 0 )
		{
			CPoint ptVpOffset( 0, 0 );
			if( (lParam&PRF_NONCLIENT) != 0 )
			{
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
			DoPaint( pDC );
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
		return (!0);
	}
	return CStatusBar::WindowProc(message, wParam, lParam);
}

LRESULT CExtStatusControlBar::OnSizeParent( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
LRESULT lResult = CStatusBar::OnSizeParent( wParam, lParam );
HWND hWndOwn = GetSafeHwnd();
	if( hWndOwn == NULL )
		return lResult;
	::RedrawWindow(
		hWndOwn,
		NULL,
		NULL,
		RDW_INVALIDATE|RDW_ERASE|RDW_ALLCHILDREN
		);
	return lResult;
}

#endif // __EXT_MFC_NO_STATUSBAR

