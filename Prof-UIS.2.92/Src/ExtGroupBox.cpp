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

#if (!defined __EXT_GROUPBOX_H)
	#include <ExtGroupBox.h>
#endif

#if (!defined __EXT_RICH_CONTENT_H)
	#include <ExtRichContent.h>
#endif // (!defined __EXT_RICH_CONTENT_H)

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_TEMPL_H)
	#include <ExtTempl.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtGroupBox

IMPLEMENT_DYNAMIC( CExtGroupBox, CButton );
IMPLEMENT_CExtPmBridge_MEMBERS( CExtGroupBox );

CExtGroupBox::CExtGroupBox()
	: m_eStyle( STYLE_SYSTEM )
	, m_eTextAlign( ALIGN_DEFAULT )
	, m_nCaptionHeaderHeight( 18 )
	, m_clrTextNormal( COLORREF(-1L) )
	, m_clrTextDisabled( COLORREF(-1L) )
	, m_clrFrameNormal( COLORREF(-1L) )
	, m_clrFrameDisabled( COLORREF(-1L) )
	, m_clrBackground( COLORREF(-1L) )
{
	PmBridge_Install();
}

CExtGroupBox::~CExtGroupBox()
{
	PmBridge_Uninstall();
}

BEGIN_MESSAGE_MAP(CExtGroupBox, CButton)
	//{{AFX_MSG_MAP(CExtGroupBox)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtGroupBox message handlers

void CExtGroupBox::PreSubclassWindow() 
{
	ModifyStyle( 0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS );
	CButton::PreSubclassWindow();
}

BOOL CExtGroupBox::OnEraseBkgnd(CDC* pDC)
{
	pDC;
	return FALSE;
}

void CExtGroupBox::DoPaint( 
	CDC * pDC,
	CRect & rcClient
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pDC );
	// exclude siblings items area
CWnd * pParent = GetParent();
	for(	CWnd * pWnd = pParent->GetWindow(GW_CHILD);
			pWnd != NULL && pWnd != this;
			pWnd = pWnd->GetWindow(GW_HWNDNEXT)
		)
	{
		if( (pWnd->GetStyle() & WS_VISIBLE) == 0 )
			continue;
		CExtSafeString sClassName;
		::GetClassName(
			pWnd->GetSafeHwnd(),
			sClassName.GetBuffer( MAX_PATH + 1 ),
			MAX_PATH
			);
		sClassName.ReleaseBuffer();
		if( sClassName == LPCTSTR( __EXT_SIZE_GRIPPER_CLASS_NAME ) )
			continue;
		CRect rcChildRect, rcIntersection;
		pWnd->GetWindowRect( &rcChildRect );
		ScreenToClient( &rcChildRect );
		rcIntersection.IntersectRect(
			&rcClient,
			&rcChildRect
			);
		if( !rcIntersection.IsRectEmpty() )
			pDC->ExcludeClipRect( &rcIntersection );
	}

CExtMemoryDC dc( pDC, &rcClient );
bool bEnabled = IsWindowEnabled() ? true : false;

	// paint background
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

int nOldBkMode = dc.SetBkMode(TRANSPARENT);

	// caption 
CExtSafeString strText;
int nTextLen = GetWindowTextLength();
	if( nTextLen > 0 )
	{
		GetWindowText( strText.GetBuffer( nTextLen + 2 ), nTextLen + 1 );
		strText.ReleaseBuffer();
	} // if( nTextLen > 0 )

CRect rcBorder( rcClient );
CRect rcText( 0, 0, 0, 0 );

DWORD dwDrawTextFlags = DT_LEFT;
	if( strText.Find( _T('\t') ) != -1 ) // do tabs expanding
		dwDrawTextFlags |= DT_EXPANDTABS;
	dwDrawTextFlags |= (DT_VCENTER|DT_END_ELLIPSIS); 

bool bDrawText = ( strText.GetLength() > 0 && m_eStyle != STYLE_CAPTION ) ? true : false;
	if( bDrawText )
	{
		rcText.SetRect( 9, 0, rcClient.right - 9, rcClient.bottom );
		HFONT hFont = OnQueryFont();
		CRect rcTextDimension = CExtPaintManager::stat_CalcTextDimension( dc, *(CFont::FromHandle(hFont)), strText );
		e_text_align_t eTextAlign = m_eTextAlign;
		if( eTextAlign == ALIGN_DEFAULT )
		{
			DWORD dwStyle = CWnd::GetStyle();
			const UINT nMask = (BS_LEFT|BS_CENTER|BS_RIGHT);
			if( (dwStyle & nMask) == BS_RIGHT )
				eTextAlign = ALIGN_RIGHT;
			else if( (dwStyle & nMask) == BS_CENTER )
				eTextAlign = ALIGN_CENTER;
			else
				eTextAlign = ALIGN_LEFT;
		}
		if( eTextAlign == ALIGN_LEFT )
			rcText.right = rcText.left + rcTextDimension.Width();
		else if( eTextAlign == ALIGN_RIGHT )
			rcText.left = rcText.right - rcTextDimension.Width();
		else if( eTextAlign == ALIGN_CENTER )
		{
			rcText.left = (rcText.Width() - rcTextDimension.Width())/2;
			rcText.right = rcText.left + rcTextDimension.Width();
		}
		rcText.bottom = rcText.top + rcTextDimension.Height();
		rcBorder.top += rcText.Height()/2;
	}
	else
		rcBorder.top += 6;

	if( bDrawText )
	{
		CRect rcTextClip( rcText );
		rcTextClip.InflateRect(2,0,2,0);
		dc.ExcludeClipRect( &rcTextClip );
	}

	// frame rect
	OnDrawFrameRect( dc, rcBorder, bEnabled );

	if( bDrawText )
		dc.SelectClipRgn( NULL );

	// caption text
	if( bDrawText )
		OnDrawLabelText( dc, rcText, strText, dwDrawTextFlags, bEnabled );

	dc.SetBkMode(nOldBkMode);

	//DefWindowProc( WM_PAINT, WPARAM(dc.GetSafeHdc()), 0L );
	PmBridge_GetPM()->OnPaintSessionComplete( this );
}

void CExtGroupBox::OnPaint() 
{
	ASSERT_VALID( this );
CPaintDC dcPaint( this );
CRect rcClient;
	GetClientRect( &rcClient );
	if( rcClient.IsRectEmpty() )
		return;
	DoPaint( &dcPaint, rcClient );
}

LRESULT CExtGroupBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
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
		SetRedraw( FALSE );
		LRESULT lResult = CButton::WindowProc(message, wParam, lParam);
		SetRedraw( TRUE );
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
		return (!0);
	}	
	return CButton::WindowProc(message, wParam, lParam);
}

HFONT CExtGroupBox::OnQueryFont() const
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

void CExtGroupBox::OnDrawFrameRect(
	CDC & dc,
	const CRect & rcClient,
	bool bEnabled
	)
{
COLORREF clrFrame = OnQueryFrameColor( bEnabled );
CRect rcBorder( rcClient );
	switch( m_eStyle )
	{
	case STYLE_SYSTEM:
		{
			PmBridge_GetPM()->PaintGroupBoxFrame(
				dc,
				rcBorder,
				clrFrame,
				this
				);
		}
		break;
	case STYLE_ETCHED:
		{
			if( clrFrame == COLORREF(-1L) )
				clrFrame = PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_GROUP_BOX_FRAME, this );
			dc.Draw3dRect(
				&rcBorder,
				clrFrame,
				PmBridge_GetPM()->GetColor( COLOR_WINDOW, this )
				);
			rcBorder.DeflateRect(1,1);
			dc.Draw3dRect(
				&rcBorder,
				PmBridge_GetPM()->GetColor( COLOR_WINDOW, this ),
				clrFrame
				);
		}
		break;
	case STYLE_ROUNDED:
		{
			CBrush * pBrush =
				CBrush::FromHandle( (HBRUSH)::GetStockObject(NULL_BRUSH) );
			CBrush * pOldBrush = dc.SelectObject( pBrush );
			if( clrFrame == COLORREF(-1L) )
				clrFrame = PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_GROUP_BOX_FRAME, this );
			CPen pen;
			pen.CreatePen(
				PS_SOLID,
				1,
				clrFrame				
				);
			CPen * pOldPen = dc.SelectObject( &pen );
			dc.RoundRect(
				&rcClient,
				CPoint( 10, 10 )
				);
			dc.SelectObject( pOldPen );
			dc.SelectObject( pOldBrush );
		}
		break;
	case STYLE_FLAT:
		{
			if( clrFrame == COLORREF(-1L) )
				clrFrame = PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_GROUP_BOX_FRAME, this );
			dc.Draw3dRect(
				&rcBorder,
				clrFrame,
				clrFrame
				);
		}
		break;
	case STYLE_CAPTION:
		{
			if( clrFrame == COLORREF(-1L) )
				clrFrame = PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_GROUP_BOX_FRAME, this );
			GetClientRect( &rcBorder );
			dc.Draw3dRect(
				&rcBorder,
				clrFrame,
				clrFrame
				);
			CRect rcHeader( rcBorder);
			rcHeader.bottom = rcHeader.top + m_nCaptionHeaderHeight;
			dc.FillSolidRect( 
				&rcHeader, 
				clrFrame
				);
			
			// caption 
			CExtSafeString strText;
			int nTextLen = GetWindowTextLength();
			if( nTextLen > 0 )
			{
				GetWindowText( strText.GetBuffer( nTextLen + 2 ), nTextLen + 1 );
				strText.ReleaseBuffer();
			} // if( nTextLen > 0 )
			if( strText.GetLength() > 0 )
			{
				e_text_align_t eTextAlign = m_eTextAlign;
				if( eTextAlign == ALIGN_DEFAULT )
				{
					DWORD dwStyle = CWnd::GetStyle();
					const UINT nMask = (BS_LEFT|BS_CENTER|BS_RIGHT);
					if( (dwStyle & nMask) == BS_RIGHT )
						eTextAlign = ALIGN_RIGHT;
					else if( (dwStyle & nMask) == BS_CENTER )
						eTextAlign = ALIGN_CENTER;
					else
						eTextAlign = ALIGN_LEFT;
				}

				DWORD dwDrawTextFlags = DT_LEFT;
				if( eTextAlign == ALIGN_RIGHT )
					dwDrawTextFlags = DT_RIGHT;
				else if( eTextAlign == ALIGN_CENTER )
					dwDrawTextFlags = DT_CENTER;

				if( strText.Find( _T('\t') ) != -1 ) // do tabs expanding
					dwDrawTextFlags |= DT_EXPANDTABS;
				dwDrawTextFlags |= (DT_VCENTER|DT_END_ELLIPSIS); 
				
				HFONT hFont = OnQueryFont();
				LOGFONT lf;
				::GetObject( 
					(HGDIOBJ)hFont, 
					sizeof(lf), 
					(LPVOID) &lf 
					);
				hFont = NULL;
				
				lf.lfWeight = FW_BOLD;
				
				hFont = ::CreateFontIndirect( &lf );
				ASSERT( hFont != NULL );
				CFont _fontDestructor;
				_fontDestructor.Attach( hFont );
				
				HGDIOBJ hOldFont = NULL;
				if( hFont != NULL )
					hOldFont = ::SelectObject( dc, (HGDIOBJ)hFont );
				INT nOldBkMode = dc.SetBkMode( TRANSPARENT );

				COLORREF clrText = OnQueryTextColor( bEnabled );
				COLORREF clrOldText =
					dc.SetTextColor(
						clrText != COLORREF(-1L) 
							? clrText 
							: PmBridge_GetPM()->GetColor( COLOR_WINDOW, this )
						);
				
				CRect rcTextDimension =
					CExtPaintManager::stat_CalcTextDimension(
					dc,
					*(CFont::FromHandle(hFont)),
					strText
					);
				
				CRect rcText(
					5,
					(m_nCaptionHeaderHeight - rcTextDimension.Height())/2,
					rcClient.right - 4,
					rcClient.bottom
					);
				CExtRichContentLayout::stat_DrawText(
					dc.m_hDC,
					LPCTSTR(strText), int(_tcslen(strText)),
					rcText,
					dwDrawTextFlags, 0
					);
				
				dc.SetTextColor( clrOldText );
				dc.SetBkMode( nOldBkMode );
				if( hFont != NULL )
					::SelectObject( dc, hOldFont );
			} // if( strText.GetLength() > 0 )
		}
		break;
	} // switch( m_eStyle )
}
	
void CExtGroupBox::OnDrawLabelText(
	CDC & dc,
	CRect & rcText,
	__EXT_MFC_SAFE_LPCTSTR strText,
	DWORD dwDrawTextFlags,
	bool bEnabled
	)
{
HFONT hFont = OnQueryFont();
COLORREF clrText = OnQueryTextColor( bEnabled );
	PmBridge_GetPM()->PaintGroupBoxLabel( dc, strText, clrText, hFont, dwDrawTextFlags, bEnabled, rcText, this );
}

void CExtGroupBox::SetStyle( e_style_t eStyle )
{
	ASSERT_VALID( this );
	m_eStyle = eStyle;
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

CExtGroupBox::e_style_t CExtGroupBox::GetStyle() const
{
	ASSERT_VALID( this );
	return m_eStyle;
}

void CExtGroupBox::SetTextAlign( e_text_align_t eTextAlign )
{
	ASSERT_VALID( this );
	m_eTextAlign = eTextAlign;
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

CExtGroupBox::e_text_align_t CExtGroupBox::GetTextAlign() const
{
	ASSERT_VALID( this );
	return m_eTextAlign;
}

void CExtGroupBox::SetCaptionHeaderHeight( int nHeight )
{
	ASSERT_VALID( this );
	m_nCaptionHeaderHeight = nHeight;
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

int CExtGroupBox::GetCaptionHeaderHeight() const
{
	ASSERT_VALID( this );
	return m_nCaptionHeaderHeight;
}

void CExtGroupBox::SetBkColor( COLORREF clrBk )
{ 
	ASSERT_VALID( this );
	m_clrBackground = clrBk; 
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

COLORREF CExtGroupBox::GetBkColor() const
{ 
	ASSERT_VALID( this );
	return m_clrBackground; 
}

void CExtGroupBox::SetTextColor(
	COLORREF clrText, // = COLORREF(-1L)
	bool bEnabled // = true
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

COLORREF CExtGroupBox::GetTextColor(
	bool bEnabled // = true
	) const
{
	ASSERT_VALID( this );
COLORREF clrText =
		bEnabled
			? m_clrTextNormal
			: m_clrTextDisabled
			;
	return clrText;
}

void CExtGroupBox::SetFrameColor(
	COLORREF clrFrame, // = COLORREF(-1L)
	bool bEnabled // = true
	)
{
	ASSERT_VALID( this );
	if( bEnabled )
		m_clrFrameNormal = clrFrame;
	else
		m_clrFrameDisabled = clrFrame;
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

COLORREF CExtGroupBox::GetFrameColor(
	bool bEnabled // = true
	) const
{
	ASSERT_VALID( this );
COLORREF clrFrame =
		bEnabled
			? m_clrFrameNormal
			: m_clrFrameDisabled
			;
	return clrFrame;
}

COLORREF CExtGroupBox::OnQueryTextColor(
	bool bEnabled // = true
	) const
{
	ASSERT_VALID( this );
COLORREF clrText = GetTextColor( bEnabled );
	return clrText;
}

COLORREF CExtGroupBox::OnQueryFrameColor(
	bool bEnabled // = true
	) const
{
	ASSERT_VALID( this );
COLORREF clrText = GetFrameColor( bEnabled );
	return clrText;
}
