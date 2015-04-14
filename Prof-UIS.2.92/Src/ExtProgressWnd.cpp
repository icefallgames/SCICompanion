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

#if (!defined __EXT_MFC_NO_PROGRESS_WND)

#if (!defined __EXT_PROGRESS_WND_H)
	#include <ExtProgressWnd.h>
#endif // (!defined __EXT_PROGRESS_WND_H)

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

//#include <Resources/Resource.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL( CExtProgressWnd, CProgressCtrl, VERSIONABLE_SCHEMA|1 );
IMPLEMENT_CExtPmBridge_MEMBERS( CExtProgressWnd );

CExtProgressWnd::CExtProgressWnd()
	: m_clrProgressBarFrom( COLORREF(-1L) )
	, m_clrProgressBarTo( COLORREF(-1L) )
	, m_clrBackgroundArea( COLORREF(-1L) )
	, m_lfEnlightClrProgressBarFrom( -0.25 )
	, m_lfEnlightClrProgressBarTo( 0.25 )
	, m_clrBorderLT( COLORREF(-1L) )
	, m_clrBorderRB( COLORREF(-1L) )
	, m_sizeBorderSpace( 2, 2 )
	, m_sizeDistanceBetweenBlocks( 2, 2 )
	, m_sizeBlockScaleFactorH( 7, 10 )
	, m_sizeBlockScaleFactorV( 7, 10 )
	, m_bHasBorder( true )
	, m_bHorizontalGradientBars( true )
	, m_bImmediateUpdating( true )
	, m_bUsePaintManager( true )
	, m_bEndlessMode( false )
	, m_nEndlessNominator( 1 )
	, m_nEndlessDenominator( 3 )
{
}

CExtProgressWnd::~CExtProgressWnd()
{
}

BEGIN_MESSAGE_MAP( CExtProgressWnd, CProgressCtrl )
    //{{AFX_MSG_MAP(CExtProgressWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_NCCALCSIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExtProgressWnd::OnPaint() 
{
	ASSERT_VALID( this );
CRect rcClient;
	GetClientRect( &rcClient );
CPaintDC dcPaint(this);
CExtMemoryDC dc( &dcPaint );
	OnPaintProgress( dc, rcClient );
}

void CExtProgressWnd::OnPaintProgress(
	CDC & dc,
	CRect rcClient
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&dc) );
	ASSERT( dc.GetSafeHdc() != NULL );
	if(		rcClient.Width() <= 0
		||	rcClient.Height() <= 0
		)
		return;
	if( ! dc.RectVisible( &rcClient ) )
		return;
CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );
int nRangeMin, nRangeMax;
	GetRange( nRangeMin, nRangeMax );
int nRange = nRangeMax - nRangeMin;
int nPos = GetPos() - nRangeMin;
DWORD dwStyle = GetStyle();
bool bHorz = ( ( dwStyle & PBS_VERTICAL ) == 0 ) ? true : false;
bool bSmooth =  ( ( dwStyle & PBS_SMOOTH ) != 0 ) ? true : false;
bool bBorder = m_bHasBorder; //( ( dwStyle & WS_BORDER ) != 0 ) ? true : false;
CRect rcProgress( rcClient );
	if(		( ! pPM->GetCb2DbTransparentMode( this ) )
		||	( ! pPM->PaintDockerBkgnd( true, dc, this ) )
		)
	{
		COLORREF clrBackgroundArea =
			( m_clrBackgroundArea == COLORREF(-1L) )
				? pPM->GetColor( COLOR_3DFACE )
				: m_clrBackgroundArea
				;
		clrBackgroundArea = dc.GetNearestColor( clrBackgroundArea );
		dc.FillSolidRect( &rcProgress, clrBackgroundArea );
	}
	if( m_bUsePaintManager )
	{
		CExtPaintManager::PAINTPROGRESSDATA _ppd( nPos, nRange, rcClient, bHorz, bSmooth, bBorder, this );
		_ppd.m_bEndlessMode = m_bEndlessMode;
		_ppd.m_nEndlessNominator = m_nEndlessNominator;
		_ppd.m_nEndlessDenominator = m_nEndlessDenominator;
		if( pPM->PaintProgress( dc, _ppd ) )
			return;
	} // if( m_bUsePaintManager )
	if( bBorder )
	{
		COLORREF clrBorderLT =
			( m_clrBorderLT == COLORREF(-1L) )
				? pPM->GetColor( COLOR_3DDKSHADOW )
				: m_clrBorderLT
				;
		COLORREF clrBorderRB =
			( m_clrBorderRB == COLORREF(-1L) )
				? clrBorderLT
				: m_clrBorderRB
				;
		clrBorderLT = dc.GetNearestColor( clrBorderLT );
		clrBorderRB = dc.GetNearestColor( clrBorderRB );
		dc.Draw3dRect( &rcProgress, clrBorderLT, clrBorderRB );
		rcProgress.DeflateRect(
			pPM->UiScalingDo( m_sizeBorderSpace.cx, CExtPaintManager::__EUIST_X ),
			pPM->UiScalingDo( m_sizeBorderSpace.cy, CExtPaintManager::__EUIST_Y )
			);
		if(		rcProgress.Width() <= 0
			||	rcProgress.Height() <= 0
			)
			return;
	} // if( bBorder )
CRect rcBar( rcProgress );
	if( m_bEndlessMode )
	{
		
		ASSERT( m_nEndlessNominator > 0 );
		ASSERT( m_nEndlessDenominator > 0 );
		ASSERT( m_nEndlessNominator < m_nEndlessDenominator );
		if( bHorz )
		{
			LONG nBasic = rcBar.Width();
			LONG nCross = rcBar.Height();
			if( nCross <= 0 )
				nCross = 1;
			LONG nAll = nBasic - ( bSmooth ? 0 : ( nBasic % nCross ) );
			LONG nPart = ::MulDiv( nAll, m_nEndlessNominator, m_nEndlessDenominator );
			LONG nExt = nAll - nPart;
			rcBar.left += ::MulDiv( nExt, nPos, nRange );
			rcBar.right = rcBar.left + nPart;
		}
		else
		{
			LONG nBasic = rcBar.Height();
			LONG nCross = rcBar.Width();
			if( nCross <= 0 )
				nCross = 1;
			LONG nAll = nBasic - ( bSmooth ? 0 : ( nBasic % nCross ) );
			LONG nPart = ::MulDiv( nAll, m_nEndlessNominator, m_nEndlessDenominator );
			LONG nExt = nAll - nPart;
			rcBar.bottom -= ::MulDiv( nExt, nPos, nRange );
			rcBar.top = rcBar.bottom - nPart;
		}
	} // if( m_bEndlessMode )
	else
	{
		if( nPos <= 0 || nRange <= 0 )
			return;
		if( bHorz )
			rcBar.right = rcBar.left + ::MulDiv( rcBar.Width(), nPos, nRange );
		else
			rcBar.top = rcBar.bottom - ::MulDiv( rcBar.Height(), nPos, nRange );
	} // else from if( m_bEndlessMode )
	if( dc.RectVisible( &rcBar ) )
	{
		COLORREF clrProgressBarFrom =
			( m_clrProgressBarFrom == COLORREF(-1L) )
				? pPM->GetColor( COLOR_HIGHLIGHT )
				: m_clrProgressBarFrom
				;
		COLORREF clrProgressBarTo =
			( m_clrProgressBarTo == COLORREF(-1L) )
				? clrProgressBarFrom
				: m_clrProgressBarTo
				;
		if( m_lfEnlightClrProgressBarFrom != 0.0 )
			clrProgressBarFrom =
				CExtPaintManager::stat_HLS_Adjust(
					clrProgressBarFrom,
					0.0,
					m_lfEnlightClrProgressBarFrom
					);
		if( m_lfEnlightClrProgressBarTo != 0.0 )
			clrProgressBarTo =
				CExtPaintManager::stat_HLS_Adjust(
					clrProgressBarFrom,
					0.0,
					m_lfEnlightClrProgressBarTo
					);
		clrProgressBarFrom = dc.GetNearestColor( clrProgressBarFrom );
		clrProgressBarTo = dc.GetNearestColor( clrProgressBarTo );
		if( bSmooth )
		{
			if( clrProgressBarFrom != clrProgressBarTo )
				CExtPaintManager::stat_PaintGradientRect(
					dc,
					&rcBar,
					clrProgressBarFrom,
					clrProgressBarTo,
					m_bHorizontalGradientBars
					);
			else
				dc.FillSolidRect( &rcBar, clrProgressBarFrom );
		} // if( bSmooth )
		else
		{
			INT nDistanceBetweenBlocks =
				pPM->UiScalingDo(
					bHorz ? m_sizeDistanceBetweenBlocks.cx : m_sizeDistanceBetweenBlocks.cy,
					bHorz ? CExtPaintManager::__EUIST_X : CExtPaintManager::__EUIST_Y
					);
			CRect rcBlock;
			if( m_bEndlessMode )
				rcBlock = rcBar;
			else
				rcBlock = rcProgress;
			if( bHorz )
				rcBlock.right =
					  rcBlock.left
					+ ::MulDiv( rcBlock.Height(), m_sizeBlockScaleFactorH.cx, m_sizeBlockScaleFactorH.cy );
			else
				rcBlock.top =
					  rcBlock.bottom
					- ::MulDiv( rcBlock.Width(), m_sizeBlockScaleFactorV.cx, m_sizeBlockScaleFactorV.cy );
			if( rcBlock.Width() <= 0 )
				rcBlock.right = rcBlock.left + 1;
			if( rcBlock.Height() <= 0 )
				rcBlock.top = rcBlock.bottom - 1;
			for( INT nIndex = 0; true; nIndex ++ )
			{
				if( bHorz )
				{
					if( rcBlock.left > rcBar.right )
						break;
					if( dc.RectVisible( &rcBlock ) )
					{
						if( clrProgressBarFrom != clrProgressBarTo )
							CExtPaintManager::stat_PaintGradientRect(
								dc,
								&rcBlock,
								clrProgressBarFrom,
								clrProgressBarTo,
								m_bHorizontalGradientBars
								);
						else
							dc.FillSolidRect( &rcBlock, clrProgressBarFrom );
					} // if( dc.RectVisible( &rcBlock ) )
					rcBlock.OffsetRect( rcBlock.Width() + nDistanceBetweenBlocks, 0 );
				} // if( bHorz )
				else
				{
					if( rcBlock.bottom < rcBar.top )
						break;
					if( dc.RectVisible( &rcBlock ) )
					{
						if( clrProgressBarFrom != clrProgressBarTo )
							CExtPaintManager::stat_PaintGradientRect(
								dc,
								&rcBlock,
								clrProgressBarFrom,
								clrProgressBarTo,
								m_bHorizontalGradientBars
								);
						else
							dc.FillSolidRect( &rcBlock, clrProgressBarFrom );
					} // if( dc.RectVisible( &rcBlock ) )
					rcBlock.OffsetRect( 0, - rcBlock.Height() - nDistanceBetweenBlocks );
				} // else from if( bHorz )
			} // for( INT nIndex = 0; true; nIndex ++ )
		} // else from if( bSmooth )
	} // if( dc.RectVisible( &rcBar ) )
}

BOOL CExtProgressWnd::OnEraseBkgnd( CDC * pDC ) 
{
	ASSERT_VALID( this );
	pDC;
    return TRUE;
}

void CExtProgressWnd::OnNcCalcSize( BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR * lpncsp ) 
{
	bCalcValidRects;
	lpncsp;
}

void CExtProgressWnd::PreSubclassWindow() 
{
	ASSERT_VALID( this );
	CProgressCtrl::PreSubclassWindow();
CRect rcClient, rcWnd,
	GetClientRect( &rcClient );
	GetWindowRect( &rcWnd );
	rcWnd.OffsetRect( -rcWnd.TopLeft() );
	if( rcWnd != rcClient )
		SetWindowPos(
			NULL, 0, 0, 0, 0,
			SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE
				|SWP_NOZORDER|SWP_NOOWNERZORDER
				|SWP_FRAMECHANGED
			);
}

LRESULT CExtProgressWnd::WindowProc( UINT message, WPARAM wParam, LPARAM lParam ) 
{
	if( message == WM_PRINT || message == WM_PRINTCLIENT )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		CDC * pDC = CDC::FromHandle( (HDC)wParam );
		OnPaintProgress( *pDC, rcClient );
		return (!0);
	} // if( message == WM_PRINT || message == WM_PRINTCLIENT )
HWND hWndOwn = m_hWnd;
LPVOID pThis = this;
LRESULT lResult = CProgressCtrl::WindowProc( message, wParam, lParam );
	if(		::IsWindow( hWndOwn )
		&&	LPVOID(CWnd::FromHandlePermanent(hWndOwn)) == pThis
		&&	m_bImmediateUpdating
		&&	(	message == PBM_SETRANGE
			||	message == PBM_SETPOS
			||	message == PBM_DELTAPOS
			||	message == PBM_STEPIT
			||	message == PBM_SETRANGE32
			)
		&&	IsWindowVisible()
		)
	{
		Invalidate();
		UpdateWindow();
	}
	return lResult;
}

#endif // (!defined __EXT_MFC_NO_PROGRESS_WND)
