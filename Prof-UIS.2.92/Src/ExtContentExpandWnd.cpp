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

#if (!defined __EXT_EXTCONTENTEXPANDWND_H)
	#include <ExtContentExpandWnd.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtContentExpandWnd

UINT CExtContentExpandWnd::g_nMsgPaintItemContent =
	::RegisterWindowMessage(
		_T("CExtContentExpandWnd::g_nMsgPaintItemContent")
		);

IMPLEMENT_DYNCREATE( CExtContentExpandWnd, CWnd );
IMPLEMENT_CExtPmBridge_MEMBERS( CExtContentExpandWnd );

CExtContentExpandWnd::CExtContentExpandWnd()
{
	_InitMembers();
	VERIFY( RegisterContentExpandWndClass() );

	PmBridge_Install();
}

CExtContentExpandWnd::~CExtContentExpandWnd()
{
	PmBridge_Uninstall();
}

void CExtContentExpandWnd::_InitMembers()
{
	m_hFont = NULL;
	m_pRODT = NULL;
	m_bDirectCreateCall = false;
	m_bVerticalMode = false;
	m_rcWnd.SetRectEmpty();
	m_rcItem.SetRectEmpty();
	m_pWndNotify = NULL;
	m_dwActivateFlags = __ECWAF_DEFAULT;

	if( m_rgnHelperAreas.GetSafeHandle() != NULL )
	{
		m_rgnHelperAreas.DeleteObject();
		ASSERT( m_rgnHelperAreas.GetSafeHandle() == NULL );
	}
	if( m_rgnWindow.GetSafeHandle() != NULL )
	{
		m_rgnWindow.DeleteObject();
		ASSERT( m_rgnWindow.GetSafeHandle() == NULL );
	}
	if( m_rgnItem.GetSafeHandle() != NULL )
	{
		m_rgnItem.DeleteObject();
		ASSERT( m_rgnItem.GetSafeHandle() == NULL );
	}
	if( m_brHelperAreas.GetSafeHandle() != NULL )
	{
		m_brHelperAreas.DeleteObject();
		ASSERT( m_brHelperAreas.GetSafeHandle() == NULL );
	}
}

BEGIN_MESSAGE_MAP(CExtContentExpandWnd, CWnd)
	//{{AFX_MSG_MAP(CExtContentExpandWnd)
	ON_WM_NCCALCSIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(WM_GETFONT, OnGetFont)	
	//}}AFX_MSG_MAP
	__EXT_MFC_ON_WM_NCHITTEST()
END_MESSAGE_MAP()

 
LRESULT CExtContentExpandWnd::OnSetFont( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
HFONT hFont = (HFONT) wParam;
BOOL bRedraw = (BOOL) lParam;
	m_hFont = hFont; 
    if( bRedraw )
        Invalidate();
	return 0L;
}
  
LRESULT CExtContentExpandWnd::OnGetFont( WPARAM, LPARAM )
{
	ASSERT_VALID( this );
    return (LRESULT) m_hFont;
}

bool CExtContentExpandWnd::Activate(
	const RECT & rcItem,
	CWnd * pWndNotify,
	DWORD dwActivateFlags, // = __ECWAF_DEFAULT
	COleDropTarget * pODT, // = NULL
	CWnd * pWndOtherForODT // = NULL
	)
{

/*
dwActivateFlags =
__ECWAF_DEFAULT
| __ECWAF_DRAW_RED_ARROWS
| __ECWAF_DRAW_RED_DOTS
;
*/

/*
dwActivateFlags &= ~__ECWAF_DRAW_ITEM;
RECT & rcItemXX = (RECT &)rcItem;
rcItemXX.right = rcItemXX.left;
//rcItemXX.bottom = rcItemXX.top;
*/

	ASSERT( (dwActivateFlags & __ECWAF_DRAW_ANY) != 0 );
	if( !RegisterContentExpandWndClass() )
	{
		ASSERT( FALSE );
		return false;
	}

	if( m_pWndNotify != NULL )
	{
		Deactivate();
	}
	ASSERT( m_pWndNotify == NULL );

	m_pWndNotify = pWndNotify;
	ASSERT( m_pWndNotify != NULL );
	ASSERT( m_pWndNotify->GetSafeHwnd() != NULL );

	m_dwActivateFlags = dwActivateFlags;

	m_bDirectCreateCall = true;

	m_rcWnd = rcItem;
	m_pWndNotify->ClientToScreen( &m_rcWnd );
	RecalcLayout();
	//ASSERT( m_rgnHelperAreas.GetSafeHandle() != NULL );
	ASSERT( m_rgnWindow.GetSafeHandle() != NULL );
	//ASSERT( m_rgnItem.GetSafeHandle() != NULL );

	if( ! CWnd::CreateEx(
			WS_EX_TOPMOST
				| ( (m_dwActivateFlags&__ECWAF_TRANSPARENT_WND)
						? WS_EX_TRANSPARENT
						: 0
					)
				, //|WS_EX_NOPARENTNOTIFY
			__EXT_CONTENTEXPANDWND_CLASS_NAME,
			NULL,
			WS_POPUP,
			m_rcWnd.left, m_rcWnd.top,
			m_rcWnd.Width(), m_rcWnd.Height(),
			m_pWndNotify->GetSafeHwnd(),
			(HMENU)NULL,
			(LPVOID)NULL
			)
		)
	{
		ASSERT( FALSE );
		Deactivate();
		return false;
	}

	if( !_CreateHelper() )
	{
		ASSERT( FALSE );
		Deactivate();
		AfxThrowMemoryException();
	}

	ASSERT( m_pRODT == NULL );
	if( pODT != NULL )
	{
		m_pRODT =
			new CExtRedirectOleDropTarget(
				pODT,
				pWndOtherForODT, //m_pWndNotify,
				this
				);
	}

	::SetWindowPos(
		GetSafeHwnd(),
		HWND_TOP,
		-1, -1, -1, -1,
		SWP_NOACTIVATE
			|SWP_NOMOVE|SWP_NOSIZE
			|SWP_SHOWWINDOW
			//|SWP_NOREDRAW //|SWP_NOSENDCHANGING
			|SWP_NOZORDER //|SWP_NOOWNERZORDER
			//|SWP_NOCOPYBITS
		);

	if( (m_dwActivateFlags&__ECWAF_NO_CAPTURE) == 0 )
		CExtMouseCaptureSink::SetCapture( GetSafeHwnd() );

	return true;
}

void CExtContentExpandWnd::Deactivate()
{
	if( GetSafeHwnd() != NULL )
	{
		//HWND hWndPostInvalidate = NULL;
		if( m_dwActivateFlags & __ECWAF_POST_INVALIDATE )
		{
			HWND hWndPostInvalidate = m_pWndNotify->GetSafeHwnd();
			if( ::IsWindow(hWndPostInvalidate) )
				::InvalidateRect(hWndPostInvalidate,NULL,TRUE);
		}
		
//		if( GetStyle() & WS_VISIBLE )
//			ShowWindow( SW_HIDE );
		if( CExtMouseCaptureSink::GetCapture() == GetSafeHwnd() )
			CExtMouseCaptureSink::ReleaseCapture();
		
		_FlushOleRedir();
		DestroyWindow();
//		if( hWndPostInvalidate != NULL
//			&& ::IsWindow(hWndPostInvalidate)
//			)
//		{
//			::InvalidateRect(hWndPostInvalidate,NULL,TRUE);
//			::UpdateWindow(hWndPostInvalidate);
//		}
	} // if( GetSafeHwnd() != NULL )

	_InitMembers();
}

void CExtContentExpandWnd::_FlushOleRedir()
{
	ASSERT_VALID( this );
	if( m_pRODT == NULL )
		return;
	m_pRODT->Revoke();
	delete m_pRODT;
	m_pRODT = NULL;
}

bool CExtContentExpandWnd::g_bContentExpandWndClassRegistered = false;

bool CExtContentExpandWnd::RegisterContentExpandWndClass()
{
	if( g_bContentExpandWndClassRegistered )
		return true;
WNDCLASS _wndClassInfo;
HINSTANCE hInst = ::AfxGetInstanceHandle();
	if( ! ::GetClassInfo( hInst, __EXT_CONTENTEXPANDWND_CLASS_NAME, &_wndClassInfo ) )
	{
		_wndClassInfo.style = CS_GLOBALCLASS|CS_DBLCLKS|CS_SAVEBITS;
		_wndClassInfo.lpfnWndProc = ::DefWindowProc;
		_wndClassInfo.cbClsExtra = _wndClassInfo.cbWndExtra = 0;
		_wndClassInfo.hInstance = hInst;
		_wndClassInfo.hIcon = NULL;
		_wndClassInfo.hCursor = ::LoadCursor( NULL, IDC_ARROW );
		ASSERT( _wndClassInfo.hCursor != NULL );
		_wndClassInfo.hbrBackground = NULL; 
		_wndClassInfo.lpszMenuName = NULL;
		_wndClassInfo.lpszClassName = __EXT_CONTENTEXPANDWND_CLASS_NAME;
		if( !::AfxRegisterClass( &_wndClassInfo ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}
	g_bContentExpandWndClassRegistered = true;
	return true;
}

bool CExtContentExpandWnd::_CreateHelper()
{
	ASSERT( m_rgnWindow.GetSafeHandle() != NULL );
	SetWindowRgn( (HRGN)m_rgnWindow.GetSafeHandle(), FALSE );
	return true;
}

void CExtContentExpandWnd::RecalcLayout()
{
	ASSERT( (m_dwActivateFlags & __ECWAF_DRAW_ANY) != 0 );
	m_rcItem = m_rcWnd;
	m_rcItem.OffsetRect( -m_rcItem.TopLeft() );

	ASSERT( m_rgnHelperAreas.GetSafeHandle() == NULL );
	ASSERT( m_rgnWindow.GetSafeHandle() == NULL );
	ASSERT( m_rgnItem.GetSafeHandle() == NULL );

	m_bVerticalMode = false;

const int nDotRgnOffsetDX = 1+1;
const int nDotRgnOffsetDY = 1+1;
const int nDotRadius = 2;
const int nDotStepDelta = 8;

	if( m_dwActivateFlags & __ECWAF_DRAW_ITEM )
	{ // if draw used-defined item
		
		if( m_dwActivateFlags & __ECWAF_DRAW_RED_ARROWS )
		{ // if draw red arrows around item
			const int nDiagonalRgnOffsetDX =
				12 // 9 + 1
				;
			const int nDiagonalRgnOffsetDY =
				12 // 9 + 1
				;
			m_rcWnd.InflateRect(
				nDiagonalRgnOffsetDX,
				nDiagonalRgnOffsetDY
				);
			m_rcItem.OffsetRect(
				nDiagonalRgnOffsetDX,
				nDiagonalRgnOffsetDY
				);
			
//			static POINT vArrowTL[] =
//			{
//				{ 3, 0 },
//				{ 6, 3 },
//				{ 8, 1 },
//				{ 8, 8 },
//				{ 1, 8 },
//				{ 3, 6 },
//				{ 0, 3 },
//			};
//			CRgn rgnArrowTL;
//			VERIFY(
//				rgnArrowTL.CreatePolygonRgn(
//					vArrowTL,
//					sizeof(vArrowTL) / sizeof(vArrowTL[0]),
//					ALTERNATE
//					)
//				);

			CRgn rgnArrowTL;
			CExtPaintManager::g_DockingCaptionGlyphs[
					CExtPaintManager::__DCBT_ARROW_PT_RB
					] -> GenerateRgn( rgnArrowTL );
			ASSERT( rgnArrowTL.GetSafeHandle() != NULL );
			
//			static POINT vArrowTR[] =
//			{
//				{ 8, 3 },
//				{ 5, 6 },
//				{ 7, 8 },
//				{ 0, 8 },
//				{ 0, 1 },
//				{ 2, 3 },
//				{ 5, 0 },
//			};
//			CRgn rgnArrowTR;
//			VERIFY(
//				rgnArrowTR.CreatePolygonRgn(
//					vArrowTR,
//					sizeof(vArrowTR) / sizeof(vArrowTR[0]),
//					ALTERNATE
//					)
//				);
//			rgnArrowTR.OffsetRgn(
//				m_rcWnd.Width() - nDiagonalRgnOffsetDX, //m_rcItem.Width() + nDiagonalRgnOffsetDX,
//				0
//				);

			CRgn rgnArrowTR;
			CExtPaintManager::g_DockingCaptionGlyphs[
					CExtPaintManager::__DCBT_ARROW_PT_LB
					] -> GenerateRgn( rgnArrowTR );
			ASSERT( rgnArrowTR.GetSafeHandle() != NULL );
			rgnArrowTR.OffsetRgn(
				m_rcWnd.Width() - nDiagonalRgnOffsetDX,
				0
				);

//			static POINT vArrowBL[] =
//			{
//				{ 0, 5 },
//				{ 3, 2 },
//				{ 1, 0 },
//				{ 8, 0 },
//				{ 8, 7 },
//				{ 6, 5 },
//				{ 3, 8 },
//			};
//			CRgn rgnArrowBL;
//			VERIFY(
//				rgnArrowBL.CreatePolygonRgn(
//					vArrowBL,
//					sizeof(vArrowBL) / sizeof(vArrowBL[0]),
//					ALTERNATE
//					)
//				);
//			rgnArrowBL.OffsetRgn(
//				0,
//				m_rcWnd.Height() - nDiagonalRgnOffsetDY // m_rcItem.Height() + nDiagonalRgnOffsetDY
//				);

			CRgn rgnArrowBL;
			CExtPaintManager::g_DockingCaptionGlyphs[
					CExtPaintManager::__DCBT_ARROW_PT_RT
					] -> GenerateRgn( rgnArrowBL );
			ASSERT( rgnArrowBL.GetSafeHandle() != NULL );
			rgnArrowBL.OffsetRgn(
				0,
				m_rcWnd.Height() - nDiagonalRgnOffsetDY
				);
			
//			static POINT vArrowBR[] =
//			{
//				{ 5, 8 },
//				{ 2, 5 },
//				{ 0, 7 },
//				{ 0, 0 },
//				{ 7, 0 },
//				{ 5, 2 },
//				{ 8, 5 },
//			};
//			CRgn rgnArrowBR;
//			VERIFY(
//				rgnArrowBR.CreatePolygonRgn(
//					vArrowBR,
//					sizeof(vArrowBR) / sizeof(vArrowBR[0]),
//					ALTERNATE
//					)
//				);
//			rgnArrowBR.OffsetRgn(
//				m_rcWnd.Width() - nDiagonalRgnOffsetDX, // m_rcItem.Width() + nDiagonalRgnOffsetDX,
//				m_rcWnd.Height() - nDiagonalRgnOffsetDY // m_rcItem.Height() + nDiagonalRgnOffsetDY
//				);

			CRgn rgnArrowBR;
			CExtPaintManager::g_DockingCaptionGlyphs[
					CExtPaintManager::__DCBT_ARROW_PT_LT
					] -> GenerateRgn( rgnArrowBR );
			ASSERT( rgnArrowBR.GetSafeHandle() != NULL );
			rgnArrowBR.OffsetRgn(
				m_rcWnd.Width() - nDiagonalRgnOffsetDX,
				m_rcWnd.Height() - nDiagonalRgnOffsetDY
				);


			CRect rcHelperAreas( m_rcWnd );
			rcHelperAreas.OffsetRect( -rcHelperAreas.TopLeft() );
			VERIFY(
				m_rgnHelperAreas.CreateRectRgnIndirect( &rcHelperAreas )
				);
			CRgn rgnTmp1, rgnTmp2;
			VERIFY(
				rgnTmp1.CreateRectRgnIndirect( &rcHelperAreas )
				);
			VERIFY(
				rgnTmp2.CreateRectRgnIndirect( &rcHelperAreas )
				);
			rgnTmp1.CombineRgn(
				&rgnArrowTL,
				&rgnArrowTR,
				RGN_OR
				);
			ASSERT( rgnTmp1.GetSafeHandle() != NULL );
			rgnTmp2.CombineRgn(
				&rgnArrowBL,
				&rgnArrowBR,
				RGN_OR
				);
			ASSERT( rgnTmp2.GetSafeHandle() != NULL );

			m_rgnHelperAreas.CombineRgn(
				&rgnTmp1,
				&rgnTmp2,
				RGN_OR
				);
			ASSERT( m_rgnHelperAreas.GetSafeHandle() != NULL );
		} // if draw red arrows around item
		
		if( m_dwActivateFlags & __ECWAF_DRAW_RED_DOTS )
		{ // if draw red dots around item
			if( (m_dwActivateFlags & __ECWAF_DRAW_RED_ARROWS) == 0 )
			{
				m_rcWnd.InflateRect(
					nDotRgnOffsetDX,
					nDotRgnOffsetDY
					);
				m_rcItem.OffsetRect(
					nDotRgnOffsetDX,
					nDotRgnOffsetDY
					);
			} // if( (m_dwActivateFlags & __ECWAF_DRAW_RED_ARROWS) == 0 )

			CSize sizeItem = m_rcItem.Size();

			CRect rcHelperDots( 0, 0, 0, 0 );
			CRgn rgnHelperDots;
			VERIFY(
				rgnHelperDots.CreateRectRgnIndirect( &rcHelperDots )
				);

			LONG nDistance = (sizeItem.cx + sizeItem.cy) * 2;
			if( nDistance >= nDotStepDelta )
			{
				for( LONG nx = nDotStepDelta/2; nx < sizeItem.cx; nx += nDotStepDelta )
				{
					static RECT rcDot = { -nDotRadius, -nDotRadius, nDotRadius, nDotRadius };
					CRgn rgnDot1, rgnDot2;
					VERIFY( rgnDot1.CreateEllipticRgnIndirect( &rcDot ) );
					VERIFY( rgnDot2.CreateEllipticRgnIndirect( &rcDot ) );
					rgnDot1.OffsetRgn(
						m_rcItem.left + nx,
						m_rcItem.top
						);
					rgnDot2.OffsetRgn(
						m_rcItem.left + m_rcItem.Width() - nx,
						m_rcItem.bottom
						);
					rgnHelperDots.CombineRgn(
						&rgnHelperDots,
						&rgnDot1,
						RGN_OR
						);
					rgnHelperDots.CombineRgn(
						&rgnHelperDots,
						&rgnDot2,
						RGN_OR
						);
					ASSERT( rgnHelperDots.GetSafeHandle() != NULL );
				}

				LONG ny =
					( sizeItem.cy % nDotStepDelta ) / 2
					;
				for( ; ny < sizeItem.cy; ny += nDotStepDelta )
				{
					static RECT rcDot = { -nDotRadius, -nDotRadius, nDotRadius, nDotRadius };
					CRgn rgnDot1, rgnDot2;
					VERIFY( rgnDot1.CreateEllipticRgnIndirect( &rcDot ) );
					VERIFY( rgnDot2.CreateEllipticRgnIndirect( &rcDot ) );
					rgnDot1.OffsetRgn(
						m_rcItem.left,
						m_rcItem.top + m_rcItem.Height() - ny
						);
					rgnDot2.OffsetRgn(
						m_rcItem.right,
						m_rcItem.top + ny
						);
					rgnHelperDots.CombineRgn(
						&rgnHelperDots,
						&rgnDot1,
						RGN_OR
						);
					rgnHelperDots.CombineRgn(
						&rgnHelperDots,
						&rgnDot2,
						RGN_OR
						);
					ASSERT( rgnHelperDots.GetSafeHandle() != NULL );
				} // for( ; ny < sizeItem.cy; ny += nDotStepDelta )

				if( rgnHelperDots.GetSafeHandle() != NULL )
				{
					if( m_rgnHelperAreas.GetSafeHandle() != NULL )
					{
						m_rgnHelperAreas.CombineRgn(
							&m_rgnHelperAreas,
							&rgnHelperDots,
							RGN_OR
							);
						ASSERT( m_rgnHelperAreas.GetSafeHandle() != NULL );
					} // if( m_rgnHelperAreas.GetSafeHandle() != NULL )
					else
					{
						CRect rcHelperAreas( m_rcWnd );
						rcHelperAreas.OffsetRect( -rcHelperAreas.TopLeft() );
						VERIFY(
							m_rgnHelperAreas.CreateRectRgnIndirect( &rcHelperAreas )
							);
						m_rgnHelperAreas.CombineRgn(
							&rgnHelperDots,
							NULL,
							RGN_COPY
							);
						ASSERT( m_rgnHelperAreas.GetSafeHandle() != NULL );
					} // else from if( m_rgnHelperAreas.GetSafeHandle() != NULL )
				} // if( rgnHelperDots.GetSafeHandle() != NULL )

			} // if( nDistance >= nDotStepDelta )

		} // if draw red dots around item
	
		VERIFY(
			m_rgnItem.CreateRectRgnIndirect( &m_rcItem )
			);
		VERIFY(
			m_rgnWindow.CreateRectRgnIndirect( &m_rcWnd )
			);
		if( m_rgnHelperAreas.GetSafeHandle() != NULL )
		{
			m_rgnWindow.CombineRgn(
				&m_rgnHelperAreas,
				&m_rgnItem,
				RGN_OR
				);
		} // if( m_rgnHelperAreas.GetSafeHandle() != NULL )
		else
		{
			m_rgnWindow.CombineRgn(
				&m_rgnItem,
				NULL,
				RGN_COPY
				);
		} // else from if( m_rgnHelperAreas.GetSafeHandle() != NULL )

		if( m_dwActivateFlags & __ECWAF_TRANSPARENT_ITEM )
		{
			m_rgnWindow.CombineRgn(
				&m_rgnWindow,
				&m_rgnItem,
				RGN_XOR
				);
			if( m_rgnHelperAreas.GetSafeHandle() != NULL )
			{
				m_rgnWindow.CombineRgn(
					&m_rgnWindow,
					&m_rgnHelperAreas,
					RGN_OR
					);
			} // if( m_rgnHelperAreas.GetSafeHandle() != NULL )
		} // if( m_dwActivateFlags & __ECWAF_TRANSPARENT_ITEM )

	} // if draw used-defined item
	else
	{ // if NOT draw used-defined item
		ASSERT(
			m_rcWnd.left == m_rcWnd.right
			|| m_rcWnd.top == m_rcWnd.bottom
			);
		if( m_rcWnd.left == m_rcWnd.right )
			m_bVerticalMode = true;
		
		CRect rcWndPrev( m_rcWnd );

		if( m_bVerticalMode )
		{
			if( m_dwActivateFlags & __ECWAF_DRAW_RED_ARROWS )
			{ // if draw red arrows
				m_rcWnd.InflateRect( 6, 12 );
				
//				static POINT vArrowTop[] =
//				{
//					{  8, 0 },
//					{  8, 4 },
//					{ 11, 4 },
//					{  6, 9 },
//					{  1, 4 },
//					{  4, 4 },
//					{  4, 0 },
//				};
//				CRgn rgnArrowTop;
//				VERIFY(
//					rgnArrowTop.CreatePolygonRgn(
//						vArrowTop,
//						sizeof(vArrowTop) / sizeof(vArrowTop[0]),
//						ALTERNATE
//						)
//					);
//
//				static POINT vArrowBottom[] =
//				{
//					{  4, 10 },
//					{  4,  6 },
//					{  0,  6 },
//					{  6,  0 },
//					{ 12,  6 },
//					{  8,  6 },
//					{  8, 10 },
//				};
//				CRgn rgnArrowBottom;
//				VERIFY(
//					rgnArrowBottom.CreatePolygonRgn(
//						vArrowBottom,
//						sizeof(vArrowBottom) / sizeof(vArrowBottom[0]),
//						ALTERNATE
//						)
//					);
//				rgnArrowBottom.OffsetRgn(
//					0,
//					m_rcWnd.Height() - 11
//					);

				CRgn rgnArrowTop, rgnArrowBottom;
				CExtPaintManager::g_DockingCaptionGlyphs[
						CExtPaintManager::__DCBT_ARROW_PT_DOWN
						] -> GenerateRgn( rgnArrowTop );
				ASSERT( rgnArrowTop.GetSafeHandle() != NULL );
				CExtPaintManager::g_DockingCaptionGlyphs[
						CExtPaintManager::__DCBT_ARROW_PT_UP
						] -> GenerateRgn( rgnArrowBottom );
				ASSERT( rgnArrowBottom.GetSafeHandle() != NULL );
				rgnArrowBottom.OffsetRgn(
					0,
					m_rcWnd.Height() - 11
					);
				
				CRect rcHelperAreas( 0, 0, 0, 0 );
				VERIFY(
					m_rgnHelperAreas.CreateRectRgnIndirect( &rcHelperAreas )
					);
				m_rgnHelperAreas.CombineRgn(
					&rgnArrowTop,
					&rgnArrowBottom,
					RGN_OR
					);
				ASSERT( m_rgnHelperAreas.GetSafeHandle() != NULL );
			} // if draw red arrows
			
			if( m_dwActivateFlags & __ECWAF_DRAW_RED_DOTS )
			{ // if draw red dots
				if( (m_dwActivateFlags & __ECWAF_DRAW_RED_ARROWS) == 0 )
					m_rcWnd.InflateRect(
						nDotRgnOffsetDX/2,
						0,
						nDotRgnOffsetDX/2 + 1,
						0
						);
				
				CSize sizeItem = m_rcItem.Size();

				CRect rcHelperDots( 0, 0, 0, 0 );
				CRgn rgnHelperDots;
				VERIFY(
					rgnHelperDots.CreateRectRgnIndirect( &rcHelperDots )
					);

				LONG nDistance = (sizeItem.cx + sizeItem.cy) * 2;
				if( nDistance >= nDotStepDelta )
				{
					LONG ny =
						( sizeItem.cy % nDotStepDelta ) / 2
						;
					for( ; ny < sizeItem.cy; ny += nDotStepDelta )
					{
						static RECT rcDot = { -nDotRadius, -nDotRadius, nDotRadius, nDotRadius };
						CRgn rgnDot1;
						VERIFY( rgnDot1.CreateEllipticRgnIndirect( &rcDot ) );
						rgnDot1.OffsetRgn(
							rcWndPrev.left - m_rcWnd.left + nDotRadius/2,
							rcWndPrev.top - m_rcWnd.top + ny
							);
						rgnHelperDots.CombineRgn(
							&rgnHelperDots,
							&rgnDot1,
							RGN_OR
							);
						ASSERT( rgnHelperDots.GetSafeHandle() != NULL );
					} // for( ; ny < sizeItem.cy; ny += nDotStepDelta )
				} // if( nDistance >= nDotStepDelta )
			
				if( rgnHelperDots.GetSafeHandle() != NULL )
				{
					if( m_rgnHelperAreas.GetSafeHandle() != NULL )
					{
						m_rgnHelperAreas.CombineRgn(
							&m_rgnHelperAreas,
							&rgnHelperDots,
							RGN_OR
							);
						ASSERT( m_rgnHelperAreas.GetSafeHandle() != NULL );
					} // if( m_rgnHelperAreas.GetSafeHandle() != NULL )
					else
					{
						CRect rcHelperAreas( m_rcWnd );
						rcHelperAreas.OffsetRect( -rcHelperAreas.TopLeft() );
						VERIFY(
							m_rgnHelperAreas.CreateRectRgnIndirect( &rcHelperAreas )
							);
						m_rgnHelperAreas.CombineRgn(
							&rgnHelperDots,
							NULL,
							RGN_COPY
							);
						ASSERT( m_rgnHelperAreas.GetSafeHandle() != NULL );
					} // else from if( m_rgnHelperAreas.GetSafeHandle() != NULL )
				} // if( rgnHelperDots.GetSafeHandle() != NULL )

			} // if draw red dots
		} // if( m_bVerticalMode )
		else
		{
			if( m_dwActivateFlags & __ECWAF_DRAW_RED_ARROWS )
			{ // if draw red arrows
				m_rcWnd.InflateRect( 12, 6 );
				
//				static POINT vArrowLeft[] =
//				{
//					{ 0,  9 },
//					{ 4,  9 },
//					{ 4, 12 },
//					{ 9,  6 },
//					{ 4,  0 },
//					{ 4,  4 },
//					{ 0,  4 },
//				};
//				CRgn rgnArrowLeft;
//				VERIFY(
//					rgnArrowLeft.CreatePolygonRgn(
//						vArrowLeft,
//						sizeof(vArrowLeft) / sizeof(vArrowLeft[0]),
//						ALTERNATE
//						)
//					);
//
//				static POINT vArrowRight[] =
//				{
//					{ 10,  4},
//					{  6,  4},
//					{  6,  0},
//					{  0,  6},
//					{  6, 12},
//					{  6,  9},
//					{ 10,  9},
//				};
//				CRgn rgnArrowRight;
//				VERIFY(
//					rgnArrowRight.CreatePolygonRgn(
//						vArrowRight,
//						sizeof(vArrowRight) / sizeof(vArrowRight[0]),
//						ALTERNATE
//						)
//					);
//				rgnArrowRight.OffsetRgn(
//					m_rcWnd.Width() - 11,
//					0
//					);

				CRgn rgnArrowLeft, rgnArrowRight;
				CExtPaintManager::g_DockingCaptionGlyphs[
						CExtPaintManager::__DCBT_ARROW_PT_RIGHT
						] -> GenerateRgn( rgnArrowLeft );
				ASSERT( rgnArrowLeft.GetSafeHandle() != NULL );
				CExtPaintManager::g_DockingCaptionGlyphs[
						CExtPaintManager::__DCBT_ARROW_PT_LEFT
						] -> GenerateRgn( rgnArrowRight );
				ASSERT( rgnArrowRight.GetSafeHandle() != NULL );
				rgnArrowRight.OffsetRgn(
					m_rcWnd.Width() - 11,
					0
					);

				CRect rcHelperAreas( 0, 0, 0, 0 );
				VERIFY(
					m_rgnHelperAreas.CreateRectRgnIndirect( &rcHelperAreas )
					);
				m_rgnHelperAreas.CombineRgn(
					&rgnArrowLeft,
					&rgnArrowRight,
					RGN_OR
					);
				ASSERT( m_rgnHelperAreas.GetSafeHandle() != NULL );
			} // if draw red arrows
			
			if( m_dwActivateFlags & __ECWAF_DRAW_RED_DOTS )
			{ // if draw red dots
				if( (m_dwActivateFlags & __ECWAF_DRAW_RED_ARROWS) == 0 )
					m_rcWnd.InflateRect(
						0,
						nDotRgnOffsetDY/2,
						0,
						nDotRgnOffsetDY/2 + 1
						);

				CSize sizeItem = m_rcItem.Size();

				CRect rcHelperDots( 0, 0, 0, 0 );
				CRgn rgnHelperDots;
				VERIFY(
					rgnHelperDots.CreateRectRgnIndirect( &rcHelperDots )
					);

				LONG nDistance = (sizeItem.cx + sizeItem.cy) * 2;
				if( nDistance >= nDotStepDelta )
				{
					LONG nx =
						( sizeItem.cx % nDotStepDelta ) / 2
						;
					for( ; nx < sizeItem.cx; nx += nDotStepDelta )
					{
						static RECT rcDot = { -nDotRadius, -nDotRadius, nDotRadius, nDotRadius };
						CRgn rgnDot1;
						VERIFY( rgnDot1.CreateEllipticRgnIndirect( &rcDot ) );
						rgnDot1.OffsetRgn(
							rcWndPrev.left - m_rcWnd.left + nx ,
							rcWndPrev.top - m_rcWnd.top + nDotRadius/2
							);
						rgnHelperDots.CombineRgn(
							&rgnHelperDots,
							&rgnDot1,
							RGN_OR
							);
						ASSERT( rgnHelperDots.GetSafeHandle() != NULL );
					} // for( ; nx < sizeItem.cx; nx += nDotStepDelta )
				} // if( nDistance >= nDotStepDelta )
			
				if( rgnHelperDots.GetSafeHandle() != NULL )
				{
					if( m_rgnHelperAreas.GetSafeHandle() != NULL )
					{
						m_rgnHelperAreas.CombineRgn(
							&m_rgnHelperAreas,
							&rgnHelperDots,
							RGN_OR
							);
						ASSERT( m_rgnHelperAreas.GetSafeHandle() != NULL );
					} // if( m_rgnHelperAreas.GetSafeHandle() != NULL )
					else
					{
						CRect rcHelperAreas( m_rcWnd );
						rcHelperAreas.OffsetRect( -rcHelperAreas.TopLeft() );
						VERIFY(
							m_rgnHelperAreas.CreateRectRgnIndirect( &rcHelperAreas )
							);
						m_rgnHelperAreas.CombineRgn(
							&rgnHelperDots,
							NULL,
							RGN_COPY
							);
						ASSERT( m_rgnHelperAreas.GetSafeHandle() != NULL );
					} // else from if( m_rgnHelperAreas.GetSafeHandle() != NULL )
				} // if( rgnHelperDots.GetSafeHandle() != NULL )
			
			} // if draw red dots
		} // else from if( m_bVerticalMode )

		ASSERT( m_rgnHelperAreas.GetSafeHandle() != NULL );
		VERIFY(
			m_rgnWindow.CreateRectRgn( 0, 0, 0, 0 )
			);
		m_rgnWindow.CombineRgn(
			&m_rgnHelperAreas,
			NULL,
			RGN_COPY
			);

	} // if NOT draw used-defined item

}

/////////////////////////////////////////////////////////////////////////////
// CExtContentExpandWnd message handlers

void CExtContentExpandWnd::PreSubclassWindow() 
{
	CWnd::PreSubclassWindow();

	if( m_bDirectCreateCall )
		return;
	
	if( !_CreateHelper() )
	{
		ASSERT( FALSE );
		AfxThrowMemoryException();
	} // if( !Create() )
}

BOOL CExtContentExpandWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( ( !RegisterContentExpandWndClass() )
		|| ( !CWnd::PreCreateWindow(cs) )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	
	cs.lpszClass = __EXT_CONTENTEXPANDWND_CLASS_NAME;

	return TRUE;
}

UINT CExtContentExpandWnd::OnNcHitTest(CPoint point) 
{
//	return CWnd::OnNcHitTest(point);
	point;
	return HTCLIENT;
}

void CExtContentExpandWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
//	CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
	bCalcValidRects;
CRect rcClient( m_rcWnd );
	::CopyRect(
		&lpncsp->rgrc[0],
		&rcClient
		);
}

BOOL CExtContentExpandWnd::OnEraseBkgnd(CDC* pDC) 
{
//	return CWnd::OnEraseBkgnd(pDC);
	pDC;
	return TRUE;
}

void CExtContentExpandWnd::OnPaint() 
{
	ASSERT( m_pWndNotify != NULL );
	ASSERT( m_pWndNotify->GetSafeHwnd() != NULL );

CPaintDC dcPaint(this);

	ASSERT( (m_dwActivateFlags & __ECWAF_DRAW_ANY) != 0 );

	if( m_dwActivateFlags != 0 )
	{

		CRect rcClient;
		GetClientRect( &rcClient );
		DWORD dwMemDcOptions = 0; // CExtMemoryDC::MDCOPT_RTL_COMPATIBILITY;
		dwMemDcOptions |=
			( m_dwActivateFlags & __ECWAF_DRAW_SOURCE )
				? CExtMemoryDC::MDCOPT_TO_MEMORY
				: CExtMemoryDC::MDCOPT_DEFAULT
				;
		CExtMemoryDC dc(
			&dcPaint,
			NULL, // &rcClient,
			dwMemDcOptions
			);
		if( m_dwActivateFlags & __ECWAF_DRAW_SOURCE )
			dc.BitBlt(
				0, 0, rcClient.Width(), rcClient.Height(),
				&dcPaint,
				0, 0,
				SRCCOPY
				);

		if( m_dwActivateFlags & __ECWAF_DRAW_ITEM )
		{ // if draw used-defined item
			if( (m_dwActivateFlags & __ECWAF_TRANSPARENT_ITEM) == 0 )
			{
				PAINT_ITEM_CONTENT_DATA _picd( dc, m_rcItem );
				if( !_picd.Notify(
						m_pWndNotify->GetSafeHwnd()
						)
					)
				{
					dc.FillSolidRect(
						&rcClient,
						PmBridge_GetPM()->GetColor( COLOR_BTNFACE, this )
						);
				}
			} // if( (m_dwActivateFlags & __ECWAF_TRANSPARENT_ITEM) == 0 )
		} // if draw used-defined item
		else
		{ // if NOT draw used-defined item
		} // if NOT draw used-defined item

		if( m_rgnHelperAreas.GetSafeHandle() != NULL )
		{
			if( m_brHelperAreas.GetSafeHandle() == NULL )
			{
				VERIFY(
					m_brHelperAreas.CreateSolidBrush(
						RGB(255,0,0)
						)
					);
			}
			if( m_brHelperAreas.GetSafeHandle() != NULL )
				dc.FillRgn( &m_rgnHelperAreas, &m_brHelperAreas );
		}

	} // if( m_dwActivateFlags != 0 )
}

LRESULT CExtContentExpandWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch( message )
	{
	case WM_DESTROY:
		_FlushOleRedir();
		break; // case WM_DESTROY:
	} // switch( message )


	if( m_pWndNotify != NULL )
	{
		bool bPassMouseMessage = false;
		switch( message )
		{
		case WM_MOUSEACTIVATE:
			if( ( m_dwActivateFlags & __ECWAF_HANDLE_MOUSE_ACTIVATE ) != 0 )
			{
				switch( ( m_dwActivateFlags & __ECWAF_MA_MASK ) )
				{
				case __ECWAF_MA_ACTIVATE:			return MA_ACTIVATE;
				case __ECWAF_MA_ACTIVATEANDEAT:		return MA_ACTIVATEANDEAT;
				case __ECWAF_MA_NOACTIVATE:			return MA_NOACTIVATE;
				case __ECWAF_MA_NOACTIVATEANDEAT:	return MA_NOACTIVATEANDEAT;
#ifdef _DEBUG
				default:
					ASSERT( FALSE );
				break;
#endif // _DEBUG
				} // switch( ( m_dwActivateFlags & __ECWAF_MA_MASK ) )
			}
			break;
		case WM_LBUTTONDOWN:
			bPassMouseMessage =
				( m_dwActivateFlags & __ECWAF_PASS_WM_LBUTTONDOWN )
					? true : false;
		break;
		case WM_MBUTTONDOWN:
			bPassMouseMessage =
				( m_dwActivateFlags & __ECWAF_PASS_WM_MBUTTONDOWN )
					? true : false;
		break;
		case WM_RBUTTONDOWN:
			bPassMouseMessage =
				( m_dwActivateFlags & __ECWAF_PASS_WM_RBUTTONDOWN )
					? true : false;
		break;
		case WM_LBUTTONUP:
			bPassMouseMessage =
				( m_dwActivateFlags & __ECWAF_PASS_WM_LBUTTONUP )
					? true : false;
		break;
		case WM_MBUTTONUP:
			bPassMouseMessage =
				( m_dwActivateFlags & __ECWAF_PASS_WM_MBUTTONUP )
					? true : false;
		break;
		case WM_RBUTTONUP:
			bPassMouseMessage =
				( m_dwActivateFlags & __ECWAF_PASS_WM_RBUTTONUP )
					? true : false;
		break;
		case WM_LBUTTONDBLCLK:
			bPassMouseMessage =
				( m_dwActivateFlags & __ECWAF_PASS_WM_LBUTTONDBLCLK )
					? true : false;
		break;
		case WM_MBUTTONDBLCLK:
			bPassMouseMessage =
				( m_dwActivateFlags & __ECWAF_PASS_WM_MBUTTONDBLCLK )
					? true : false;
		break;
		case WM_RBUTTONDBLCLK:
			bPassMouseMessage =
				( m_dwActivateFlags & __ECWAF_PASS_WM_RBUTTONDBLCLK )
					? true : false;
		break;
		case WM_MOUSEWHEEL:
			bPassMouseMessage =
				( m_dwActivateFlags & __ECWAF_PASS_WM_MOUSEWHEEL )
					? true : false;
		break;
		case WM_MOUSEMOVE:
			bPassMouseMessage =
				( m_dwActivateFlags & __ECWAF_PASS_WM_MOUSEMOVE )
					? true : false;
		break;
		} // switch( message )
		if( bPassMouseMessage )
		{
			CPoint ptCursor( 0, 0 );
			::GetCursorPos( &ptCursor );
			m_pWndNotify->ScreenToClient( &ptCursor );
			lParam = MAKELPARAM( ptCursor.x, ptCursor.y );
			LRESULT lResult =
				::SendMessage(
					m_pWndNotify->GetSafeHwnd(),
					message,
					wParam,
					lParam
					);
			if( (m_dwActivateFlags&__ECWAF_REDIRECT_NO_DEACTIVATE) == 0 )
				Deactivate();
			if( (m_dwActivateFlags&__ECWAF_REDIRECT_AND_HANDLE) == 0 )
			{
				if( (m_dwActivateFlags&__ECWAF_REDIRECT_RETVAL) != 0 )
					return lResult;
				return 0;
			} // if( (m_dwActivateFlags&__ECWAF_REDIRECT_AND_HANDLE) == 0 )
		} // if( bPassMouseMessage )
	} // if( m_pWndNotify != NULL )
	
	return CWnd::WindowProc(message, wParam, lParam);
}
