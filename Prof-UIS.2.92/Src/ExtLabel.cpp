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

#if (!defined __EXT_LABEL_H)
	#include <ExtLabel.h>
#endif

#if (!defined __EXT_RICH_CONTENT_H)
	#include <ExtRichContent.h>
#endif // (!defined __EXT_RICH_CONTENT_H)

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#include <io.h>

#define __VFW_LIB_NAME "vfw32.lib"
#pragma message("   Prof-UIS is automatically linking with VFW library: " __VFW_LIB_NAME )
#pragma message("      (Video for Windows)")
#pragma comment( lib, __VFW_LIB_NAME ) 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtLabel window

IMPLEMENT_DYNCREATE( CExtLabel, CStatic );
IMPLEMENT_CExtPmBridge_MEMBERS( CExtLabel );

bool CExtLabel::g_bHelper_AutoAdjust_SS_NOTIFY_style = true;

CExtLabel::CExtLabel()
	: m_bFontBold(false)
	, m_bFontItalic(false)
	, m_bFontUnderline(false)
	, m_bFontStrikeOut(false)
	, m_clrTextNormal( COLORREF(-1L) )
	, m_clrTextDisabled( COLORREF(-1L) )
	, m_clrBackground( COLORREF(-1L) )
	, m_bInitText(false)
	, m_sText( _T("") )
	, m_eImageMode( eAlign )
	, m_nAdvancedTipStyle( INT(CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON) )
	, m_bHelper_AutoAdjust_SS_NOTIFY_style(  CExtLabel::g_bHelper_AutoAdjust_SS_NOTIFY_style )
	, m_rcLabelTextMargins( 0, 0, 0, 0 )
{
	m_bmp.Empty();
	PmBridge_Install();
}

CExtLabel::~CExtLabel()
{
	m_bmp.Empty();
	PmBridge_Uninstall();
}

BEGIN_MESSAGE_MAP(CExtLabel, CStatic)
	//{{AFX_MSG_MAP(CExtLabel)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CExtLabel::OnEraseBkgnd(CDC* pDC) 
{
	pDC;
	return TRUE;
}

void CExtLabel::DoPaint( CDC * pDC, CRect & rcClient )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pDC );
CExtMemoryDC dc( pDC, &rcClient );
CRgn rgnClient;
	if( rgnClient.CreateRectRgnIndirect( &rcClient ) )
		dc.SelectClipRgn( &rgnClient );
	OnEraseBackground( dc, rcClient );
DWORD dwWndStyle = GetStyle();
DWORD dwWndType = (dwWndStyle&SS_TYPEMASK);
bool bCenterImage = ( (dwWndStyle&SS_CENTERIMAGE) != 0 );
	if( ! m_bmp.IsEmpty() )
	{
		bool bSmootherAsPossible = true;
		e_ImageMode_t eImageMode = GetImageMode();
		if( eImageMode == eStretch )
			m_bmp.AlphaBlendSkinParts( dc.GetSafeHdc(), rcClient, CRect(0,0,0,0), CExtBitmap::__EDM_STRETCH, true, bSmootherAsPossible );
		else if( eImageMode == eTouchInside || eImageMode == eTouchOutside )
		{
			CRect rcTouchSurface = rcClient;
			CExtMemoryDC dcTouch( &dc, rcTouchSurface, CExtMemoryDC::MDCOPT_TO_MEMORY|CExtMemoryDC::MDCOPT_FILL_BITS|CExtMemoryDC::MDCOPT_RTL_COMPATIBILITY );
			if( dcTouch.GetSafeHdc() )
			{
				CSize sizeTouchSrc = rcTouchSurface.Size();
				CSize sizeBmp = m_bmp.GetSize();
				double lfAspectX = double(rcTouchSurface.Width())  / double(sizeBmp.cx);
				double lfAspectY = double(rcTouchSurface.Height()) / double(sizeBmp.cy);
				double lfAspect = ( eImageMode == eTouchInside ) ? ( min( lfAspectX, lfAspectY ) ) : ( max( lfAspectX, lfAspectY ) );
				CSize sizeNew( LONG(double(sizeBmp.cx)*lfAspect), LONG(double(sizeBmp.cy)*lfAspect) );
				CRect rcTouchDst( rcTouchSurface.left, rcTouchSurface.top, rcTouchSurface.left + sizeNew.cx, rcTouchSurface.top + sizeNew.cy );
				if( eImageMode == eTouchInside )
				{				
					if( sizeNew.cx > sizeTouchSrc.cx )
						rcTouchDst.OffsetRect( ( sizeNew.cx - sizeTouchSrc.cx ) / 2, 0 );
					if( sizeNew.cy > sizeTouchSrc.cy )
						rcTouchDst.OffsetRect( 0, ( sizeNew.cy - sizeTouchSrc.cy ) / 2 );
				}
				else
				{
					rcTouchDst.OffsetRect( - ( sizeNew.cx - sizeTouchSrc.cx ) / 2, 0 );
					rcTouchDst.OffsetRect( 0, - ( sizeNew.cy - sizeTouchSrc.cy ) / 2 );
				}
				INT nOldStretchBltMode = bSmootherAsPossible ? ( ::GetStretchBltMode( dcTouch.m_hDC ) ) : ( COLORONCOLOR ) ;
				if( bSmootherAsPossible )
					::SetStretchBltMode( dcTouch.m_hDC, ( g_PaintManager.m_bIsWinNT ) ? HALFTONE : COLORONCOLOR );
				m_bmp.AlphaBlend( dcTouch.m_hDC, rcTouchDst );
				if( bSmootherAsPossible )
					::SetStretchBltMode( dcTouch.m_hDC, nOldStretchBltMode );
			}
		}
		else if( eImageMode == eTile )
			m_bmp.AlphaBlendSkinParts(  dc.GetSafeHdc(), rcClient, CRect(0,0,0,0), CExtBitmap::__EDM_TILE, true, bSmootherAsPossible );
		else if( eImageMode == eAlign )
		{
			CSize szSize = m_bmp.GetSize();
			CRect rcDst( rcClient.left, rcClient.top, rcClient.left + szSize.cx, rcClient.top + szSize.cy );
			bool bCenterHorizontally = false;
			switch( dwWndType )
			{
			case SS_RIGHT:  rcDst.OffsetRect( rcClient.right - rcDst.right, 0 ); break;
			case SS_CENTER: bCenterHorizontally = true;        break;
			default: /* all the other types assumed as left */ break;
			}
			if( bCenterHorizontally )
				rcDst.OffsetRect( ( (rcClient.right - rcClient.left) - (rcDst.right - rcDst.left) ) / 2, 0 );
			if( bCenterImage )
				rcDst.OffsetRect( 0, ( (rcClient.bottom - rcClient.top) - (rcDst.bottom - rcDst.top) ) / 2 );
			CRect rcSrc( 0, 0, szSize.cx, szSize.cy );
			rcDst.top = max( rcDst.top, rcClient.top );
			rcDst.left = max( rcDst.left, rcClient.left );
			rcDst.bottom = min( rcDst.bottom, rcClient.bottom );
			rcDst.right = min( rcDst.right, rcClient.right );
			if( ::RectVisible( dc.GetSafeHdc(), &rcDst ) )
			{
				INT nOldStretchBltMode = bSmootherAsPossible ? ( ::GetStretchBltMode( dc.m_hDC ) ) : ( COLORONCOLOR );
				if( bSmootherAsPossible )
					::SetStretchBltMode( dc.m_hDC, ( g_PaintManager.m_bIsWinNT ) ? HALFTONE : COLORONCOLOR );
				m_bmp.AlphaBlend( dc.m_hDC, rcDst, rcSrc );
				if( bSmootherAsPossible )
					::SetStretchBltMode( dc.m_hDC, nOldStretchBltMode );
			}
		}
	}
	else if( dwWndType == SS_ICON )
	{
		HICON hIcon = GetIcon();
		if( hIcon != NULL )
		{
			CExtCmdIcon _icon;
			_icon.AssignFromHICON( hIcon, true );
			CSize szIcon = _icon.GetSize();
			int nOffsetX = bCenterImage ? (rcClient.Width() - szIcon.cx) / 2 : 0;
			int nOffsetY = bCenterImage ? (rcClient.Height() - szIcon.cy) / 2 : 0;
			_icon.Paint(
				PmBridge_GetPM(), dc.GetSafeHdc(), rcClient.left + nOffsetX, rcClient.top + nOffsetY, -1, -1,
				IsWindowEnabled() ? CExtCmdIcon::__PAINT_NORMAL : CExtCmdIcon::__PAINT_DISABLED
				);
		}
	}
	else
	{
		CExtSafeString strText;
		int nTextLen = GetWindowTextLength();
		if( nTextLen > 0 )
		{
			GetWindowText( strText.GetBuffer( nTextLen + 2 ), nTextLen + 1 );
			strText.ReleaseBuffer();
		}
		if( strText.GetLength() > 0 )
		{
			DWORD dwDrawTextFlags = 0;
			switch( dwWndType )
			{
			case SS_RIGHT:           dwDrawTextFlags = DT_RIGHT;  break; 
			case SS_CENTER:          dwDrawTextFlags = DT_CENTER; break;
			case SS_LEFTNOWORDWRAP:  dwDrawTextFlags = DT_LEFT;   break;
			default: /* all the other types assumed as left */ dwDrawTextFlags = DT_LEFT; break;
			} // switch( dwWndType )
			if( strText.Find( _T('\t') ) != -1 ) // do tabs expanding
				dwDrawTextFlags |= DT_EXPANDTABS;
			if(		(dwWndType == SS_SIMPLE)
				||	(dwWndStyle&(SS_CENTERIMAGE|SS_ENDELLIPSIS|SS_PATHELLIPSIS)) != 0
				)
			{
				dwDrawTextFlags |= DT_SINGLELINE;
				if( (dwWndStyle&SS_CENTERIMAGE) != 0 )
					dwDrawTextFlags |= DT_VCENTER;
				if( (dwWndStyle&SS_ENDELLIPSIS) != 0 )
					dwDrawTextFlags |= DT_END_ELLIPSIS;
				if( (dwWndStyle&SS_PATHELLIPSIS) != 0 )
					dwDrawTextFlags |= DT_PATH_ELLIPSIS;
			}
			else
				dwDrawTextFlags |= DT_WORDBREAK;
			if( dwWndType == SS_LEFTNOWORDWRAP )
				dwDrawTextFlags &= ~(DT_WORDBREAK|DT_SINGLELINE);
			if( (dwWndStyle&SS_NOPREFIX) != 0 )
				dwDrawTextFlags |= DT_NOPREFIX;
			bool bEnabled = IsWindowEnabled() ? true : false;
			OnDrawLabelText( dc, rcClient, strText, dwDrawTextFlags, bEnabled );
		} // if( strText.GetLength() > 0 )
	}
	PmBridge_GetPM()->OnPaintSessionComplete( this );
	if( rgnClient.GetSafeHandle() != NULL )
		dc.SelectClipRgn( &rgnClient );	
}

void CExtLabel::OnPaint() 
{
	ASSERT_VALID( this );
DWORD dwWndStyle = GetStyle();
DWORD dwWndType = (dwWndStyle&SS_TYPEMASK);
	if(		dwWndType == SS_BLACKRECT 
		||	dwWndType == SS_GRAYRECT 
		||	dwWndType == SS_WHITERECT 
		||	dwWndType == SS_BLACKFRAME 
		||	dwWndType == SS_GRAYFRAME 
		||	dwWndType == SS_WHITEFRAME 
		||	dwWndType == SS_USERITEM 
		||	dwWndType == SS_OWNERDRAW 
		||	dwWndType == SS_BITMAP 
		||	dwWndType == SS_ENHMETAFILE 
		||	dwWndType == SS_ETCHEDHORZ 
		||	dwWndType == SS_ETCHEDVERT 
		||	dwWndType == SS_ETCHEDFRAME 
		)
	{
		Default();
		return;
	}
CPaintDC dcPaint( this );
CRect rcClient;
	GetClientRect( &rcClient );
	if( rcClient.IsRectEmpty() )
		return;
	DoPaint( &dcPaint, rcClient );
}

void CExtLabel::OnEraseBackground(
	CDC & dc,
	const CRect & rcClient
	)
{
	ASSERT_VALID( this );
COLORREF clrBackground = GetBkColor();
bool bTransparent = false;
	if(		PmBridge_GetPM()->GetCb2DbTransparentMode(this)
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
}

HFONT CExtLabel::OnQueryFont() const
{
	ASSERT_VALID( this );
HFONT hFont = NULL;
	if( GetSafeHwnd() != NULL )
		hFont = (HFONT) ::SendMessage( m_hWnd, WM_GETFONT, 0L, 0L );
	if( hFont == NULL )
	{
		HWND hWndParent = ::GetParent( m_hWnd );
		if( hWndParent != NULL )
			hFont = (HFONT)
				::SendMessage( hWndParent, WM_GETFONT, 0L, 0L );
	} // if( hFont == NULL )
	if( hFont == NULL )
	{
		hFont = (HFONT)::GetStockObject( DEFAULT_GUI_FONT );
		if( hFont == NULL )
			hFont = (HFONT)::GetStockObject( SYSTEM_FONT );
	} // if( hFont == NULL )
	return hFont;
}

void CExtLabel::OnDrawLabelText(
	CDC & dc,
	const RECT & rcText,
	__EXT_MFC_SAFE_LPCTSTR strText,
	DWORD dwDrawTextFlags,
	bool bEnabled
	)
{
HFONT hFont = OnQueryFont();
	ASSERT( hFont != NULL );

LOGFONT lf;
	::GetObject( 
		(HGDIOBJ) hFont, 
        sizeof( lf ), 
        (LPVOID) & lf 
		);
	hFont = NULL;
	
	if( m_bFontBold )
		lf.lfWeight = 
			(lf.lfWeight > FW_BOLD) 
				? lf.lfWeight 
				: FW_BOLD;
	lf.lfItalic = (BYTE)( m_bFontItalic ? 1 : 0 );
	lf.lfUnderline = (BYTE)( m_bFontUnderline ? 1 : 0 );
	lf.lfStrikeOut = (BYTE)( m_bFontStrikeOut ? 1 : 0 );
	
	hFont = ::CreateFontIndirect( &lf );
	ASSERT( hFont != NULL );
CFont _fontDestructor;
	_fontDestructor.Attach( hFont );
	
HGDIOBJ hOldFont = NULL;
	if( hFont != NULL )
		hOldFont = ::SelectObject( dc, (HGDIOBJ)hFont );
INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
COLORREF clrOldText =
		dc.SetTextColor( OnQueryTextColor( bEnabled ) );

CRect rc( rcText );
	rc.DeflateRect( &m_rcLabelTextMargins );
	CExtRichContentLayout::stat_DrawText(
		dc.m_hDC,
		LPCTSTR(strText), int(_tcslen(strText)),
		rc,
		dwDrawTextFlags, 0
		);
	dc.SetTextColor( clrOldText );
	dc.SetBkMode( nOldBkMode );
	if( hFont != NULL )
		::SelectObject( dc, hOldFont );
}

COLORREF CExtLabel::OnQueryTextColor(
	bool bEnabled
	) const
{
	ASSERT_VALID( this );
COLORREF clrText = GetTextColor( bEnabled );
	if( clrText != COLORREF(-1L) )
		return clrText;
CWindowDC dcFake( NULL );
	clrText =
		PmBridge_GetPM()->QueryObjectTextColor(
			dcFake,
			bEnabled,
			false,
			false,
			false,
			(CObject*)this
			);
	if( clrText != COLORREF(-1L) )
		return clrText;
	clrText =
		PmBridge_GetPM()->GetColor(
			bEnabled
				? COLOR_BTNTEXT
				: CExtPaintManager::CLR_TEXT_DISABLED,
			(CObject*)this
			);
	return clrText;
}

LRESULT CExtLabel::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
#if (defined WM_UPDATEUISTATE)	
	ASSERT( WM_UPDATEUISTATE == 0x0128 );
#endif
	// WM_UPDATEUISTATE causes repaint without WM_PAINT, so we eat it
	if( message == 0x0128 )
		return 0;
	if(	message == WM_ENABLE )
	{
		LRESULT lResult = CStatic::WindowProc(message, wParam, lParam);
		Invalidate();
		UpdateWindow();
		return lResult;
	}
	if( message == WM_PRINT || message == WM_PRINTCLIENT )
	{
		CDC * pDC = CDC::FromHandle( (HDC)wParam );
		CRect rcClient;
		GetClientRect( &rcClient );
		DoPaint( pDC, rcClient );
		HWND hWndChild = ::GetWindow( m_hWnd, GW_CHILD );
		for( ; hWndChild != NULL; hWndChild = ::GetWindow( hWndChild, GW_HWNDNEXT ) )
		{
			__EXT_MFC_LONG_PTR dwChildStyle = ::__EXT_MFC_GetWindowLong( hWndChild, GWL_STYLE );
			if( (dwChildStyle&WS_VISIBLE) == 0 )
				continue;
			CRect rcChildWnd, rcChildClient, rcStartClient;
			
			::GetClientRect( m_hWnd, &rcStartClient );
			::ClientToScreen( hWndChild, ((LPPOINT)(&rcStartClient)) );
			::ClientToScreen( hWndChild, ((LPPOINT)(&rcStartClient))+1 );

			::GetWindowRect( hWndChild, &rcChildWnd );
			::GetClientRect( hWndChild, &rcChildClient );
			::ClientToScreen( hWndChild, ((LPPOINT)(&rcChildClient)) );
			::ClientToScreen( hWndChild, ((LPPOINT)(&rcChildClient))+1 );
			CPoint ptChildRenderOffset( 0, 0 );
			if( (lParam&PRF_NONCLIENT) != 0 )
			{
				ptChildRenderOffset.x = rcStartClient.left - rcChildWnd.left;
				ptChildRenderOffset.y = rcStartClient.top - rcChildWnd.top;
			}
			else
			{
				ptChildRenderOffset.x = rcStartClient.left - rcChildClient.left;
				ptChildRenderOffset.y = rcStartClient.top - rcChildClient.top;
			}
			if(		ptChildRenderOffset.x != 0
				||	ptChildRenderOffset.y != 0
				)
				::OffsetViewportOrgEx(
					pDC->m_hDC,
					-ptChildRenderOffset.x,
					-ptChildRenderOffset.y,
					NULL
					);
			::SendMessage(
				hWndChild,
				message,
				(WPARAM)pDC->m_hDC,
				lParam
				);
//			CExtPaintManager::stat_PrintChildren(
//				hWndChild,
//				message,
//				pDC->m_hDC,
//				lParam,
//				true,
//				this,
//				lParam
//				);
			if(		ptChildRenderOffset.x != 0
				||	ptChildRenderOffset.y != 0
				)
				::OffsetViewportOrgEx(
					pDC->m_hDC,
					ptChildRenderOffset.x,
					ptChildRenderOffset.y,
					NULL
					);
		} // for( ; hWndChild != NULL; hWndChild = ::GetWindow( hWndChild, GW_HWNDNEXT ) )
		
		return (!0);
	}

	if( message == WM_MOUSEMOVE )
	{
		if(		m_nAdvancedTipStyle != INT(CExtPopupMenuTipWnd::__ETS_NONE)
			&&	( ! m_strToolTipText.IsEmpty() )
			&&	(! CExtPopupMenuWnd::IsMenuTracking() )
			)
		{
			CExtPopupMenuTipWnd * pATTW = OnAdvancedPopupMenuTipWndGet();
			if( pATTW != NULL )
			{
				CRect rcArea;
				GetWindowRect( &rcArea );
				OnAdvancedPopupMenuTipWndDisplay( *pATTW, rcArea );
			}
		}
	}
	else
	if(		message == WM_SETTEXT 
		||	message == WM_GETTEXT 
		||	message == WM_GETTEXTLENGTH 
		)
	{
		DWORD dwWndStyle = GetStyle();
		DWORD dwWndType = (dwWndStyle&SS_TYPEMASK);
		if(		dwWndType == SS_BLACKRECT 
			||	dwWndType == SS_GRAYRECT 
			||	dwWndType == SS_WHITERECT 
			||	dwWndType == SS_BLACKFRAME 
			||	dwWndType == SS_GRAYFRAME 
			||	dwWndType == SS_WHITEFRAME 
			||	dwWndType == SS_USERITEM 
			||	dwWndType == SS_OWNERDRAW 
			||	dwWndType == SS_BITMAP 
			||	dwWndType == SS_ICON
			||	dwWndType == SS_ENHMETAFILE 
			||	dwWndType == SS_ETCHEDHORZ 
			||	dwWndType == SS_ETCHEDVERT 
			||	dwWndType == SS_ETCHEDFRAME 
			)
			return CStatic::WindowProc( message, wParam, lParam );

		if(		(!m_bInitText)
			&&	(message == WM_GETTEXT || message == WM_GETTEXTLENGTH)
			)
		{
			LRESULT lResult = CStatic::WindowProc( message, wParam, lParam );
			
			INT nMaxLength = 0;
			if( message != WM_GETTEXTLENGTH )
			{
				WPARAM wParamLocal = 0L;
				LPARAM lParamLocal = 0L; 
				nMaxLength = (INT)
					CStatic::WindowProc( 
						WM_GETTEXTLENGTH, 
						wParamLocal, 
						lParamLocal 
						);
			}
			else
				nMaxLength = (INT)lResult;
			CString sTextInit;
			CStatic::WindowProc( 
				WM_GETTEXT, 
				nMaxLength + 1, 
				(LPARAM)sTextInit.GetBuffer( nMaxLength + 1 ) 
				);
			sTextInit.ReleaseBuffer();
			m_sText = sTextInit;

			m_bInitText = true;

			return lResult;
		}

		if( message == WM_SETTEXT )
		{
			LPCTSTR lpszText = (LPCTSTR)lParam;
			m_sText = lpszText;
			m_bInitText = true;
			RedrawWindow(
				NULL, NULL,
				RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW | RDW_ERASENOW
				);
			return TRUE;
		}
		else if( message == WM_GETTEXT )
		{
			TCHAR * lpszText = (TCHAR *)lParam;
			::memset( lpszText, 0, wParam );

			__EXT_MFC_STRNCPY(
				lpszText,
				wParam,
				m_sText,
				wParam - 1
				);
			lpszText[ wParam - 1 ] = _T('\0');

			return m_sText.GetLength();
		}
		else if( message == WM_GETTEXTLENGTH )
		{
			return m_sText.GetLength();
		}
	}

	return CStatic::WindowProc(message, wParam, lParam);
}

void CExtLabel::PreSubclassWindow()
{
	m_bInitText = false;
	CStatic::PreSubclassWindow();
	if( m_bHelper_AutoAdjust_SS_NOTIFY_style )
		ModifyStyle(
			m_strToolTipText.IsEmpty() ? SS_NOTIFY : 0,
			m_strToolTipText.IsEmpty() ? 0 : SS_NOTIFY
			);
}

void CExtLabel::SetFontBold( 
	bool bSet // = true 
	)
{
	ASSERT_VALID( this );
	m_bFontBold = bSet;
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

void CExtLabel::SetFontItalic( 
	bool bSet // = true 
	)
{
	ASSERT_VALID( this );
	m_bFontItalic = bSet;
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

void CExtLabel::SetFontUnderline( 
	bool bSet // = true 
	)
{
	ASSERT_VALID( this );
	m_bFontUnderline = bSet;
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

void CExtLabel::SetFontStrikeOut( 
	bool bSet // = true 
	)
{
	ASSERT_VALID( this );
	m_bFontStrikeOut = bSet;
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

void CExtLabel::SetBkColor( COLORREF clrBk )
{ 
	ASSERT_VALID( this );
	m_clrBackground = clrBk; 
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

void CExtLabel::SetTextColor(
	bool bEnabled,
	COLORREF clrText // = COLORREF(-1L)
	)
{
	ASSERT_VALID( this );
	if( bEnabled )
		m_clrTextNormal = clrText;
	else
		m_clrTextDisabled = clrText;
	if( GetSafeHwnd() != NULL )
		Invalidate();
}
	
bool CExtLabel::GetFontBold()
{
	ASSERT_VALID( this );
	return m_bFontBold;
}

bool CExtLabel::GetFontItalic()
{
	ASSERT_VALID( this );
	return m_bFontItalic;
}

bool CExtLabel::GetFontUnderline()
{
	ASSERT_VALID( this );
	return m_bFontUnderline;
}

bool CExtLabel::GetFontStrikeOut()
{
	ASSERT_VALID( this );
	return m_bFontStrikeOut;
}

COLORREF CExtLabel::GetBkColor() const
{ 
	ASSERT_VALID( this );
	return m_clrBackground; 
}

COLORREF CExtLabel::GetTextColor( bool bEnabled ) const
{
	ASSERT_VALID( this );
COLORREF clrText = bEnabled ? m_clrTextNormal : m_clrTextDisabled;
	return clrText;
}

const CExtBitmap * CExtLabel::GetBitmapEx() const
{
	ASSERT_VALID( this );
	return 
		( const_cast < CExtLabel * > ( this ) ) -> GetBitmapEx();
}

CExtBitmap * CExtLabel::GetBitmapEx()
{
	ASSERT_VALID( this );
	return (&m_bmp);
}

bool CExtLabel::SetBitmapEx( CExtBitmap * pBmp )
{
	ASSERT_VALID( this );
	if( pBmp == NULL || pBmp->IsEmpty() )
	{
		m_bmp.Empty();
		if( GetSafeHwnd() != NULL )
			Invalidate();
		return true;
	}
bool bRetVal = m_bmp.AssignFromOther( *pBmp );
	if( GetSafeHwnd() != NULL )
		Invalidate();
	return bRetVal;
}

CExtLabel::e_ImageMode_t CExtLabel::GetImageMode() const
{
	ASSERT_VALID( this );
	return m_eImageMode;
}

CExtLabel::e_ImageMode_t CExtLabel::SetImageMode( CExtLabel::e_ImageMode_t eImageMode )
{
	ASSERT_VALID( this );
e_ImageMode_t eImageModeOld = m_eImageMode;
	m_eImageMode = eImageMode;
	return eImageModeOld;
}

void CExtLabel::SetTooltipText( int nId )
{
	ASSERT_VALID( this );
CExtSafeString sText;
	g_ResourceManager->LoadString( sText, nId );
	SetTooltipText( LPCTSTR(sText) );
}

void CExtLabel::SetTooltipText( __EXT_MFC_SAFE_LPCTSTR sText )
{
	ASSERT_VALID( this );
	if( sText == NULL )
		sText = _T("");
	m_strToolTipText = sText;
	if(		m_bHelper_AutoAdjust_SS_NOTIFY_style
		&&	GetSafeHwnd() != NULL
		)
		ModifyStyle(
			m_strToolTipText.IsEmpty() ? SS_NOTIFY : 0,
			m_strToolTipText.IsEmpty() ? 0 : SS_NOTIFY
			);
}

CExtPopupMenuTipWnd * CExtLabel::OnAdvancedPopupMenuTipWndGet() const
{
	if( m_nAdvancedTipStyle == INT(CExtPopupMenuTipWnd::__ETS_NONE) )
		return NULL;
	return (&( CExtPopupMenuSite::g_DefPopupMenuSite.GetTip() ));
}

void CExtLabel::OnAdvancedPopupMenuTipWndDisplay(
	CExtPopupMenuTipWnd & _ATTW,
	const RECT & rcExcludeArea
	) const
{
	ASSERT_VALID( this );
	if( m_strToolTipText.IsEmpty() )
		return;
	_ATTW.SetText( LPCTSTR(m_strToolTipText) );
	_ATTW.SetTipStyle( (CExtPopupMenuTipWnd::e_tip_style_t)m_nAdvancedTipStyle );
	_ATTW.Show( (CWnd*)this, rcExcludeArea );
}

/////////////////////////////////////////////////////////////////////////////
// CExtAviLabel window

IMPLEMENT_DYNCREATE( CExtAviLabel, CExtLabel );

CExtAviLabel::CExtAviLabel()
	: m_nAviTimerID( 400 )
	, m_nAviFrameFrom( 0 )
	, m_nAviFrameTo( UINT(-1) )
	, m_nRep( UINT(-1) )
	, m_dwAviStyle( __EXT_AVI_DEFAULT_STYLES )
	, m_bSeek( false )
	, m_bPlaying( false )
	, m_lfAviSpeedRate( 1.0 )
	, m_clrTransparent( COLORREF(-1L) )
	, m_clrTransparentEffective( COLORREF(-1L) )
{
	_Impl_Init();
	::AVIFileInit();
}

CExtAviLabel::~CExtAviLabel()
{
	AviEmpty();
	::AVIFileExit();
}

BEGIN_MESSAGE_MAP( CExtAviLabel, CExtLabel )
	//{{AFX_MSG_MAP(CExtAviLabel)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CExtAviLabel::PreSubclassWindow()
{
	ASSERT_VALID( this );
	CExtLabel::PreSubclassWindow();
	ModifyStyle( 0, WS_CLIPSIBLINGS|WS_CLIPCHILDREN );
}

void CExtAviLabel::PostNcDestroy()
{
	ASSERT_VALID( this );
	AviStop();
	CExtLabel::PostNcDestroy();
}

bool CExtAviLabel::AviOpen(
	UINT nAviResourceID,
	__EXT_MFC_SAFE_LPCTSTR strAviResourceSection // = NULL // NULL means __EXT_MFC_DEFAULT_AVI_RESOURCE_SECTION_NAME
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	if( LPCTSTR(strAviResourceSection) == NULL )
		strAviResourceSection = __EXT_MFC_DEFAULT_AVI_RESOURCE_SECTION_NAME;
CExtResourceManager::CExtResourceMemoryBuffer rb; 
	if( ! g_ResourceManager->LoadResourceBuffer( rb, LPCTSTR(strAviResourceSection), nAviResourceID ) )
	{
		ASSERT( FALSE );
		return false;
	}
DWORD dwAviSize = rb.GetSize();
	ASSERT( dwAviSize > 0L );
LPCVOID pAviBytes = rb.GetBuffer();
	ASSERT( pAviBytes != NULL );
	return AviOpen( pAviBytes, dwAviSize );
}

bool CExtAviLabel::AviOpen(
	UINT nAviResourceID,
	__EXT_MFC_SAFE_LPCTSTR strAviResourceSection, // NULL means __EXT_MFC_DEFAULT_AVI_RESOURCE_SECTION_NAME
	WORD wLangIdDesired,
	WORD wLangIdNeutral
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	if( LPCTSTR(strAviResourceSection) == NULL )
		strAviResourceSection = __EXT_MFC_DEFAULT_AVI_RESOURCE_SECTION_NAME;
CExtResourceManager::CExtResourceMemoryBuffer rb; 
	if( ! g_ResourceManager->LoadResourceBufferEx( rb, LPCTSTR(strAviResourceSection), nAviResourceID, wLangIdDesired, wLangIdNeutral ) )
	{
		ASSERT( FALSE );
		return false;
	}
DWORD dwAviSize = rb.GetSize();
	ASSERT( dwAviSize > 0L );
 LPCVOID pAviBytes = rb.GetBuffer();
	ASSERT( pAviBytes != NULL );
	return AviOpen( pAviBytes, dwAviSize );
}

bool CExtAviLabel::AviOpen(
	LPCVOID pAviBytes,
	DWORD dwAviSize
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL || pAviBytes == NULL || dwAviSize == 0 )
	{
		ASSERT( FALSE );
		return false;
	}
	if( m_pAS != NULL )
	{
		AviEmpty();
		//_Impl_Init();
	}
	_Impl_Init();
	ASSERT( m_strTempFileName.IsEmpty() );
	m_strTempFileName = OnAviGenTempFileName();
	ASSERT( ! m_strTempFileName.IsEmpty() );
CFile tmpFile;
	if( ! tmpFile.Open( m_strTempFileName, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite ) )
		return false;
	try
	{
		tmpFile.Write( pAviBytes, dwAviSize );
	}
	catch( CException * pException )
	{
		tmpFile.Close();
		pException->Delete();
		return false;
	}
	tmpFile.Close();
	return AviOpen( LPCTSTR(m_strTempFileName) );
}

bool CExtAviLabel::AviOpen(
	__EXT_MFC_SAFE_LPCTSTR strAviFilePath
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	if( m_pAS != NULL )
	{
		AviEmpty();
		//_Impl_Init();
	}
	_Impl_Init();
HRESULT hr = ::AVIStreamOpenFromFile( &m_pAS, strAviFilePath, streamtypeVIDEO, 0, OF_READ|OF_SHARE_EXCLUSIVE, NULL );
	if( FAILED(hr) )
	{
		m_pAS = NULL;
		return false;
	}
	ASSERT( m_pAS != NULL );
	m_pGF = ::AVIStreamGetFrameOpen( m_pAS, NULL );
	if( m_pGF == NULL )
		return false;
//	ASSERT( m_pGF != NULL );
	_Impl_CalcTime();
	_Impl_PostLoadSync();
	if( ( AviStyleGet() & __EXT_AVI_TRANSPARENT_VIDEO ) != 0 )
		m_clrTransparentEffective = OnAviCalcTransparentColor();
	if( ( AviStyleGet() & __EXT_AVI_PLAY_AUTOMATICALLY ) != 0 )
		return AviPlay();
	return true;
}

void CExtAviLabel::_Impl_CalcTime()
{
	ASSERT_VALID( this );
// 	if( AviIsEmpty() )
// 		return;
	if( m_pAS == NULL || m_hDrawDib == NULL )
		return;
LONG nTmp = ::AVIStreamLength( m_pAS );
	m_nAviFrameCount = (UINT)nTmp;
	m_nAviFrameTotalCount = (UINT)nTmp;
	if( m_nAviFrameFrom != 0 )
		m_nAviFrameFrom = ( m_nAviFrameFrom >= UINT(m_nAviFrameCount) ) ? UINT(m_nAviFrameCount) : m_nAviFrameFrom;
	if( m_nAviFrameTo != UINT(-1) )
		m_nAviFrameTo = ( m_nAviFrameTo >= UINT(m_nAviFrameCount) ) ? UINT(m_nAviFrameCount) : m_nAviFrameTo;
	ASSERT ( m_nAviFrameCount > 0 );

	if( m_nAviFrameFrom > m_nAviFrameTo )
	{
		int nAviFrameFrom = m_nAviFrameFrom;
		m_nAviFrameFrom = m_nAviFrameTo;
		m_nAviFrameTo = nAviFrameFrom;
	}

	_Impl_CalcRate();
}

void CExtAviLabel::_Impl_CalcRate()
{
	ASSERT_VALID( this );
	if( m_pAS == NULL || m_hDrawDib == NULL )
		return;
LONG nTime = AVIStreamEndTime( m_pAS );
	ASSERT( nTime > 0 );
	m_nAviFrameCount = m_nAviFrameTotalCount;
	m_nAviTimerEllapse = (UINT)( nTime / m_nAviFrameCount );
	if( __EXT_MFC_AVI_SPEED_RATE_MIN < m_lfAviSpeedRate && m_lfAviSpeedRate <= __EXT_MFC_AVI_SPEED_RATE_MAX )
		m_nAviTimerEllapse = (UINT)( DOUBLE( m_nAviTimerEllapse ) / m_lfAviSpeedRate );
	else
	{
		m_nAviFrameCount = 0;
//		m_nAviFrameTotalCount = 0;
		m_nAviTimerEllapse = 0;
		return;
	}
	if( ( m_nAviFrameTo != UINT(-1) ) && ( UINT(m_nAviFrameCount) > m_nAviFrameTo ) )
		m_nAviFrameCount -= ( m_nAviFrameCount - m_nAviFrameTo );
	if( m_nAviFrameFrom > 0 )
		m_nAviFrameCount -= m_nAviFrameFrom;
}

void CExtAviLabel::_Impl_PostLoadSync()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL || AviIsEmpty() )
		return;
AVISTREAMINFO _AviStreamInfo;
	::memset( &_AviStreamInfo, 0, sizeof(_AviStreamInfo) );
	::AVIStreamInfo( m_pAS, &_AviStreamInfo, sizeof(_AviStreamInfo) );
CRect rcClient;
	GetClientRect( &rcClient );
CSize sizeAvi(
		_AviStreamInfo.rcFrame.right - _AviStreamInfo.rcFrame.left,
		_AviStreamInfo.rcFrame.bottom - _AviStreamInfo.rcFrame.top
		);
CPoint ptAviOffset(
		( ( rcClient.Width()  > sizeAvi.cx ) ? ( ( rcClient.Width()  - sizeAvi.cx ) / 2 ) : 0 ),
		( ( rcClient.Height() > sizeAvi.cy ) ? ( ( rcClient.Height() - sizeAvi.cy ) / 2 ) : 0 )
		);
	m_sizeAvi = sizeAvi;
}

DWORD CExtAviLabel::AviStyleGet() const
{
	ASSERT_VALID( this );
	return m_dwAviStyle;
}

bool CExtAviLabel::AviStyleSet( DWORD dwAviStyle )
{
	ASSERT_VALID( this );
	if( m_dwAviStyle == dwAviStyle )
		return true;
	if( GetSafeHwnd() == NULL )
	{
		m_dwAviStyle = dwAviStyle;
		return true;
	}
	AviStop();
	m_ptAviOffset = CPoint( 0, 0 );
	m_sizeAvi = CSize( 0, 0 );
	m_dwAviStyle = dwAviStyle;
	if( ! AviIsEmpty() )
	{
		_Impl_PostLoadSync();
		_Impl_CalcTime();
	}
	if( ( m_dwAviStyle & __EXT_AVI_TRANSPARENT_VIDEO ) != 0 )
		m_clrTransparentEffective = OnAviCalcTransparentColor();
	if( ( m_dwAviStyle & __EXT_AVI_PLAY_AUTOMATICALLY ) != 0 )
		return AviPlay();
	return true;
}

bool CExtAviLabel::AviPlay(
	UINT nFrom, // = 0
	UINT nTo,   // = UINT(-1)
	UINT nRep   // = UINT(-1)
	)
{
	ASSERT_VALID( this );
	AviStop( true );
	if( GetSafeHwnd() == NULL || AviIsEmpty() )
		return false;
	if( nFrom == UINT(-1) )
		nFrom = 0;
	if( nTo == UINT(-1) )
		nTo = (m_nAviFrameTotalCount-1);
	nFrom = min( nFrom, (m_nAviFrameTotalCount-1) );
	nTo   = min( nTo, (m_nAviFrameTotalCount-1) );
	if( nFrom == nTo )
		return AviSeek( nFrom );

	m_nAviFrameFrom = nFrom;
	m_nAviFrameTo = nTo;
	m_nRep = nRep;
	_Impl_CalcTime();
	if( m_nAviFrameFrom != 0 )
		m_nAviFrameCurrent = m_nAviFrameFrom;
	Invalidate();
	if( nRep == 0 )
		return true;
	SetTimer( m_nAviTimerID, m_nAviTimerEllapse, NULL );
	m_bPlaying = true;
	return true;
}

bool CExtAviLabel::AviStop(
	bool bResetToFirst // = false
	)
{
	ASSERT_VALID( this );
	if( bResetToFirst )
	{
		m_nAviFrameCurrent = m_nAviFrameFrom;
		if( GetSafeHwnd() != NULL )
			Invalidate();
	}
	m_bSeek = false;
	if( ( ! m_bPlaying ) || GetSafeHwnd() == NULL || m_pAS == NULL )
		return false;
	KillTimer( m_nAviTimerID );
	m_bPlaying = false;
	return true;
}

bool CExtAviLabel::AviSeek( UINT nTo )
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL || m_pAS == NULL )
	{
		ASSERT( FALSE );
		return false;
	}
	if( m_bPlaying )
		AviStop();
	m_nAviFrameCurrent = min( nTo, (UINT)m_nAviFrameTotalCount );
	m_bSeek = true;
	Invalidate();
	return true;
}

CSize CExtAviLabel::AviFrameGetSize() const
{
	ASSERT_VALID( this );
	return m_sizeAvi;
}

UINT CExtAviLabel::AviFrameGetFrom() const
{
	ASSERT_VALID( this );
	return m_nAviFrameFrom;
}

UINT CExtAviLabel::AviFrameGetTo() const
{
	ASSERT_VALID( this );
	return m_nAviFrameTo;
}

UINT CExtAviLabel::AviFrameGetCurrent() const
{
	ASSERT_VALID( this );
	return m_nAviFrameCurrent;
}

UINT CExtAviLabel::AviFrameGetCount() const
{
	ASSERT_VALID( this );
	return m_nAviFrameTotalCount;
}

bool CExtAviLabel::AviIsPlaying() const
{
	ASSERT_VALID( this );
	return m_bPlaying;
}

void CExtAviLabel::DoPaint( CDC * pDC, CRect & rcClient )
{
	ASSERT_VALID( this );
CSize sizeClient = rcClient.Size();
	if( sizeClient.cx <= 0 || sizeClient.cy <= 0 )
		return;
	if( ! ::RectVisible( pDC->GetSafeHdc(), &rcClient ) )
		return;
CExtMemoryDC dc( pDC, &rcClient );
CRgn rgnClient;
	if( rgnClient.CreateRectRgnIndirect( &rcClient ) )
		dc.SelectClipRgn( &rgnClient );
	OnEraseBackground( dc, rcClient );
	if( ( ! AviIsEmpty() ) && ( m_bSeek || m_bPlaying || ( ( ! m_bPlaying ) && ( ( AviStyleGet() & __EXT_AVI_EMPTY_BACKGROUND_ON_STOP ) == 0 ) ) ) )
	{
//		OnAviPaintCurrentFrame( dc, rcClient, m_sizeAvi );
		DWORD dwWndStyle = GetStyle();
		DWORD dwWndType = (dwWndStyle&SS_TYPEMASK);
		bool bCenterImage = ( (dwWndStyle&SS_CENTERIMAGE) != 0 );
		e_ImageMode_t eImageMode = GetImageMode();
		if( eImageMode == eStretch )
		{
			CSize sizeAvi = rcClient.Size();
			OnAviPaintCurrentFrame( dc, rcClient, sizeAvi );
		}
		else if( eImageMode == eTouchInside || eImageMode == eTouchOutside )
		{
			CRect rcTouchSurface = rcClient;
			CExtMemoryDC dcTouch( &dc, rcTouchSurface, CExtMemoryDC::MDCOPT_TO_MEMORY|CExtMemoryDC::MDCOPT_FILL_BITS|CExtMemoryDC::MDCOPT_RTL_COMPATIBILITY );
			if( dcTouch.GetSafeHdc() )
			{
				CSize sizeTouchSrc = rcTouchSurface.Size();
				CSize sizeBmp = m_sizeAvi;
				double lfAspectX = double(rcTouchSurface.Width())  / double(sizeBmp.cx);
				double lfAspectY = double(rcTouchSurface.Height()) / double(sizeBmp.cy);
				double lfAspect = ( eImageMode == eTouchInside ) ? ( min( lfAspectX, lfAspectY ) ) : ( max( lfAspectX, lfAspectY ) );
				CSize sizeNew( LONG(double(sizeBmp.cx)*lfAspect), LONG(double(sizeBmp.cy)*lfAspect) );
				CRect rcTouchDst( rcTouchSurface.left, rcTouchSurface.top, rcTouchSurface.left + sizeNew.cx, rcTouchSurface.top + sizeNew.cy );
				if( eImageMode == eTouchInside )
				{
					if( sizeNew.cx > sizeTouchSrc.cx )
						rcTouchDst.OffsetRect( ( sizeNew.cx - sizeTouchSrc.cx ) / 2, 0 );
					if( sizeNew.cy > sizeTouchSrc.cy )
						rcTouchDst.OffsetRect( 0, ( sizeNew.cy - sizeTouchSrc.cy ) / 2 );
				}
				else
				{
					rcTouchDst.OffsetRect( - ( sizeNew.cx - sizeTouchSrc.cx ) / 2, 0 );
					rcTouchDst.OffsetRect( 0, - ( sizeNew.cy - sizeTouchSrc.cy ) / 2 );
				}
				CSize sizeAvi = rcTouchDst.Size();
				OnAviPaintCurrentFrame( dcTouch, rcTouchDst, sizeAvi );
			}
		}
		else if( eImageMode == eTile )
		{
			UINT nStepCountH = sizeClient.cx / m_sizeAvi.cx + ( ( sizeClient.cx % m_sizeAvi.cx ) ? 1 : 0 );
			UINT nStepCountV = sizeClient.cy / m_sizeAvi.cy + ( ( sizeClient.cy % m_sizeAvi.cy ) ? 1 : 0 );
			if( nStepCountH > 0 && nStepCountV > 0 )
			{
				UINT nStepIndexH, nStepIndexV;
				for( nStepIndexV = 0; nStepIndexV < nStepCountV; nStepIndexV ++ )
				{
					CRect rcWalk( rcClient.TopLeft(), m_sizeAvi );
					rcWalk.OffsetRect( 0, m_sizeAvi.cy * nStepIndexV );
					for( nStepIndexH = 0; nStepIndexH < nStepCountH; nStepIndexH ++ )
					{
						if( ::RectVisible( dc.GetSafeHdc(), &rcWalk ) )
							OnAviPaintCurrentFrame( dc, rcWalk, m_sizeAvi );
						rcWalk.OffsetRect( m_sizeAvi.cx, 0 );
					}
				}
			}
		}
		else if( eImageMode == eAlign )
		{
			CRect rcDst( rcClient.left, rcClient.top, rcClient.left + m_sizeAvi.cx, rcClient.top + m_sizeAvi.cy );
			bool bCenterHorizontally = false;
			switch( dwWndType )
			{
			case SS_RIGHT:  rcDst.OffsetRect( rcClient.right - rcDst.right, 0 ); break;
			case SS_CENTER: bCenterHorizontally = true;        break;
			default: /* all the other types assumed as left */ break;
			}
			if( bCenterHorizontally )
				rcDst.OffsetRect( ( (rcClient.right - rcClient.left) - (rcDst.right - rcDst.left) ) / 2, 0 );
			if( bCenterImage )
				rcDst.OffsetRect( 0, ( (rcClient.bottom - rcClient.top) - (rcDst.bottom - rcDst.top) ) / 2 );
			OnAviPaintCurrentFrame( dc, rcDst, m_sizeAvi );
		}
	}
	PmBridge_GetPM()->OnPaintSessionComplete( this );
	if( rgnClient.GetSafeHandle() != NULL )
		dc.SelectClipRgn( &rgnClient );	
}

void CExtAviLabel::OnAviPaintCurrentFrame( CDC & dc, const CRect & rcDrawDst, const CSize & sizeAvi )
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( sizeAvi.cx <= 0 || sizeAvi.cy <= 0 )
		return;
LPBITMAPINFOHEADER pBIH = (LPBITMAPINFOHEADER)AVIStreamGetFrame(m_pGF, (LONG)m_nAviFrameCurrent);
//	ASSERT( pBIH != NULL );
	if( pBIH == NULL )
		return;
CExtMemoryDC dcProcessingSurface( &dc, rcDrawDst, CExtMemoryDC::MDCOPT_TO_MEMORY|CExtMemoryDC::MDCOPT_FORCE_DIB|CExtMemoryDC::MDCOPT_RTL_COMPATIBILITY );
	VERIFY(
		::DrawDibDraw(
			m_hDrawDib, dcProcessingSurface.GetSafeHdc(),
			rcDrawDst.left + m_ptAviOffset.x, rcDrawDst.top + m_ptAviOffset.y, sizeAvi.cx, sizeAvi.cy,
			pBIH, NULL, 0, 0, -1, -1, 0
			)
		);
COLORREF clrTransparentPixels = ( ( AviStyleGet() & __EXT_AVI_TRANSPARENT_VIDEO ) != 0 ) ? m_clrTransparentEffective : PmBridge_GetPM()->GetColor( COLOR_WINDOW );
	CExtPaintManager::stat_TransparentBlt(
		dc.m_hDC,
		rcDrawDst.left + m_ptAviOffset.x, rcDrawDst.top + m_ptAviOffset.y, sizeAvi.cx, sizeAvi.cy,
		dcProcessingSurface.GetSafeHdc(),
		rcDrawDst.left + m_ptAviOffset.x, rcDrawDst.top + m_ptAviOffset.y, sizeAvi.cx, sizeAvi.cy,
		clrTransparentPixels
		);
	dcProcessingSurface.__Flush( FALSE );
}

bool CExtAviLabel::AviSizeFromContent(
	CSize & sizeBefore,
	CSize & sizeAfter,
	bool bMoveWindow // = true
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return false;
	if( AviIsEmpty() )
		return false;
CWnd * pWndParent = GetParent();
	if( pWndParent == NULL )
		return false;
bool bChildMode = ( ( GetStyle() & WS_CHILD ) != 0 ) ? true : false;
CRect rcWnd;
	GetWindowRect( &rcWnd );
	if( bChildMode )
		pWndParent->ScreenToClient( &rcWnd );
CRect rcClient;
	GetClientRect( &rcClient );
CSize sizePrevious = rcWnd.Size();
CSize sizeClient = rcClient.Size();
CSize sizeNonClient = sizePrevious - sizeClient;
CSize sizeAviFrame = AviFrameGetSize();
CRect rcDst( rcWnd.left, rcWnd.top, rcWnd.left + sizeNonClient.cx + sizeAviFrame.cx, rcWnd.top + sizeNonClient.cy + sizeAviFrame.cy );
CSize sizeDst = rcDst.Size();
	sizeBefore = sizePrevious;
	sizeAfter  = sizeDst;
	if( sizeDst == sizePrevious )
		return false;
	if( bMoveWindow )
		MoveWindow( &rcDst );
	return true;
}

bool CExtAviLabel::AviSizeFromContent(
	bool bMoveWindow // = true
	)
{
	ASSERT_VALID( this );
CSize sizeBefore, sizeAfter;
	return AviSizeFromContent( sizeBefore, sizeAfter, bMoveWindow );
}

COLORREF CExtAviLabel::AviTransparentColorGet() const
{
	ASSERT_VALID( this );
	return m_clrTransparent;
}

void CExtAviLabel::AviTransparentColorSet(
	COLORREF clrTransparent // = COLORREF(-1L)
	)
{
	ASSERT_VALID( this );
	if( m_clrTransparent == clrTransparent )
		return;
	m_clrTransparent = clrTransparent;
	if( ( m_dwAviStyle & __EXT_AVI_TRANSPARENT_VIDEO ) != 0 )
		m_clrTransparentEffective = OnAviCalcTransparentColor();
}

COLORREF CExtAviLabel::OnAviCalcTransparentColor()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL || AviIsEmpty() )
		return COLORREF(-1L);
	if( m_clrTransparent != COLORREF(-1L) )
		return m_clrTransparent;
	ASSERT( m_pAS != NULL );
	ASSERT( m_pGF != NULL );
LPBITMAPINFO pBI = (LPBITMAPINFO)::AVIStreamGetFrame( m_pGF, 0 );
	ASSERT( pBI != NULL );
	if( pBI == NULL || pBI->bmiHeader.biBitCount != 8 )
		return COLORREF(-1L);
LPBYTE lpbyIndex = (LPBYTE)( (DWORD)pBI + pBI->bmiHeader.biSize + (pBI->bmiHeader.biClrUsed*sizeof(RGBQUAD)) );
RGBQUAD * pRGBFirst = (RGBQUAD*)(&pBI->bmiColors[*lpbyIndex]);
	return RGB( pRGBFirst->rgbRed, pRGBFirst->rgbGreen, pRGBFirst->rgbBlue );
}

CExtSafeString CExtAviLabel::OnAviGenTempFileName()
{
	ASSERT_VALID( this );
TCHAR lpTempPathBuffer[MAX_PATH];
UINT uRetVal = 0;
CExtSafeString strRetVal;
	::GetTempPath( MAX_PATH, lpTempPathBuffer );
    uRetVal = ::GetTempFileName( lpTempPathBuffer, _T("AVI"), 0, strRetVal.GetBuffer( MAX_PATH ) );
	strRetVal.ReleaseBuffer();
    if( uRetVal == 0 )
		strRetVal.Empty();
	return strRetVal;
}

void CExtAviLabel::_Impl_Init()
{
	ASSERT_VALID( this );
	m_pAS = NULL;
	m_pGF = NULL;
	m_ptAviOffset.x = m_ptAviOffset.y = m_sizeAvi.cx = m_sizeAvi.cy = 0;
	m_nAviFrameCurrent = m_nAviFrameFrom;
	m_bPlaying = false;
	m_hDrawDib = ::DrawDibOpen();
	m_nAviFrameFrom = 0, m_nAviFrameCurrent = 0, m_nAviFrameCount = 0, m_nAviFrameTotalCount = 0;
	m_nAviFrameTo = m_nRep = UINT(-1);
	ASSERT( m_hDrawDib != NULL );
}

void CExtAviLabel::AviEmpty()
{
	ASSERT_VALID( this );
	AviStop();
	if( m_pGF != NULL )
	{
		VERIFY( ::AVIStreamGetFrameClose( m_pGF ) == NOERROR );
		m_pGF = NULL;
	}
	if( m_pAS != NULL )
	{
		::AVIStreamRelease( m_pAS );
		m_pAS = NULL;
	}
	if( ! m_strTempFileName.IsEmpty() )
	{
		if( ::_taccess( LPCTSTR(m_strTempFileName), 0 ) != -1 )
		{
			if( ! ::DeleteFile( LPCTSTR(m_strTempFileName) ) )
				::MoveFileEx( LPCTSTR(m_strTempFileName), NULL, MOVEFILE_DELAY_UNTIL_REBOOT );
		}
		m_strTempFileName.Empty();
	}
	if( m_hDrawDib != NULL )
	{
		::DrawDibClose( m_hDrawDib );
		m_hDrawDib = NULL;
	}
	if( m_hWnd != NULL )
		Invalidate();
}

bool CExtAviLabel::AviIsEmpty() const
{
	ASSERT_VALID( this );
	if( m_pAS != NULL && m_nAviFrameTotalCount > 0 && m_hDrawDib != NULL )
		return false;
	return true;
}

void CExtAviLabel::OnSize( UINT nType, int cx, int cy ) 
{
	ASSERT_VALID( this );
	CExtLabel::OnSize( nType, cx, cy );
	_Impl_PostLoadSync();
}

void CExtAviLabel::OnTimer( __EXT_MFC_UINT_PTR nIDEvent )
{
	ASSERT_VALID( this );
	if( nIDEvent != m_nAviTimerID )
	{
		CExtLabel::OnTimer( nIDEvent );
		return;
	}
UINT nLastFrame = ( m_nAviFrameTo == UINT(-1) ) ? ( m_nAviFrameFrom + m_nAviFrameCount ) : m_nAviFrameTo;
	if( (++m_nAviFrameCurrent) > nLastFrame )
	{
		m_nAviFrameCurrent = m_nAviFrameFrom;
		if( m_nRep != UINT(-1) )
			m_nRep --;
		if( m_nRep == 0 )
		{
			AviStop( true );
			return;
		}
	}
	Invalidate();
}

void CExtAviLabel::AviSpeedRateSet( DOUBLE lfAviSpeedRate )
{
	ASSERT_VALID( this );
	m_lfAviSpeedRate = lfAviSpeedRate;
	_Impl_CalcRate();
	if( m_bPlaying )
	{
		if( m_nAviTimerEllapse == 0 )
			KillTimer( m_nAviTimerID );
		else
			SetTimer( m_nAviTimerID, m_nAviTimerEllapse, NULL );
	}
}

DOUBLE CExtAviLabel::AviSpeedRateGet()
{
	ASSERT_VALID( this );
	return m_lfAviSpeedRate;
}

CExtSafeString CExtAviLabel::AviTempFileNameGet()
{
	ASSERT_VALID( this );
	return m_strTempFileName;
}



