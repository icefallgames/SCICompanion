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

#include "StdAfx.h"

#if (!defined __EXT_MFC_NO_SPLITTER_WND)

#if (!defined __EXT_SPLITTER_WND_H)
	#include <ExtSplitterWnd.h>
#endif

#if (!defined __EXT_SCROLLWND_H)
	#include <ExtScrollWnd.h>
#endif 

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_RESIZABLE_DIALOG_H)
	#include <ExtResizableDialog.h>
#endif

#if _MFC_VER < 0x700
	#include <../src/AfxImpl.h>
#else
	#include <../src/mfc/AfxImpl.h>
#endif

#if (!defined __AFXPRIV_H__)
	#include <AfxPriv.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtSplitterBaseWnd

IMPLEMENT_DYNCREATE( CExtSplitterBaseWnd, CSplitterWnd );
IMPLEMENT_CExtPmBridge_MEMBERS( CExtSplitterBaseWnd );

HCURSOR CExtSplitterBaseWnd::g_split_hcurLast = NULL;
HCURSOR CExtSplitterBaseWnd::g_split_hcurDestroy = NULL;
UINT CExtSplitterBaseWnd::g_split_idcPrimaryLast = 0;

CExtSplitterBaseWnd::CExtSplitterBaseWnd()
	: m_clrBackground( COLORREF(-1L) )
{
	//AFX_ZERO_INIT_OBJECT( CWnd );
#if _MFC_VER < 0x700
	if( ! afxData.bWin4 )
	{
		m_cxSplitter = m_cySplitter = 4;
		m_cxBorderShare = m_cyBorderShare = 1;
		m_cxSplitterGap = m_cySplitterGap = 4 + 1 + 1;
		ASSERT( m_cxBorder == 0 && m_cyBorder == 0 );
	}
	else
#endif //_MFC_VER < 0x700
	{
		m_cxSplitter = m_cySplitter = 3 + 2 + 2;
		m_cxBorderShare = m_cyBorderShare = 0;
		m_cxSplitterGap = m_cySplitterGap = 3 + 2 + 2;
		m_cxBorder = m_cyBorder = 2;
	}
#ifdef _DEBUG
	if(		GetSystemMetrics(SM_CXBORDER) != 1
		||	GetSystemMetrics(SM_CYBORDER) != 1
		)
	{
		TRACE0( "Warning: CExtSplitterBaseWnd assumes 1 pixel border.\n" );
	}
#endif
	PmBridge_Install();
}

CExtSplitterBaseWnd::~CExtSplitterBaseWnd()
{
//	delete[] m_pRowInfo;
//	delete[] m_pColInfo;
	PmBridge_Uninstall();
}

COLORREF CExtSplitterBaseWnd::OnQueryBackgroundColor() const
{
	ASSERT_VALID( this );
	return m_clrBackground;
}

int CExtSplitterBaseWnd::stat_CanSplitRowCol( CExtSplitterBaseWnd::CRowColInfo * pInfoBefore, int nBeforeSize, int nSizeSplitter )
{
	ASSERT( pInfoBefore->nCurSize > 0 );
	ASSERT( pInfoBefore->nMinSize > 0 );
	ASSERT( nBeforeSize <= pInfoBefore->nCurSize );
	if( nBeforeSize < pInfoBefore->nMinSize )
	{
		TRACE0("Warning: split too small to fit in a new pane.\n");
		return -1;
	}
int nNewSize = pInfoBefore->nCurSize - nBeforeSize - nSizeSplitter;
	if( nBeforeSize < pInfoBefore->nMinSize )
	{
		TRACE0("Warning: split too small to shrink old pane.\n");
		return -1;
	}
	if( nNewSize < (pInfoBefore+1)->nMinSize )
	{
		TRACE0("Warning: split too small to create new pane.\n");
		return -1;
	}
	return nNewSize;
}

void CExtSplitterBaseWnd::stat_LayoutRowCol( CExtSplitterBaseWnd::CRowColInfo * pInfoArray, int nMax, int nSize, int nSizeSplitter )
{
	ASSERT( pInfoArray != NULL );
	ASSERT( nMax > 0);
	ASSERT( nSizeSplitter > 0 );
CExtSplitterBaseWnd::CRowColInfo * pInfo;
int i;
	if( nSize < 0 )
		nSize = 0;
	for( i = 0, pInfo = pInfoArray; i < nMax-1; i++, pInfo++ )
	{
		if( pInfo->nIdealSize < pInfo->nMinSize )
			pInfo->nIdealSize = 0;
		pInfo->nCurSize = pInfo->nIdealSize;
	}
	pInfo->nCurSize = INT_MAX;
	for( i = 0, pInfo = pInfoArray; i < nMax; i++, pInfo++ )
	{
		ASSERT(nSize >= 0);
		if( nSize == 0)
		{
			// no more room (set pane to be invisible)
			pInfo->nCurSize = 0;
			continue;       // don't worry about splitters
		}
		else if( nSize < pInfo->nMinSize && i != 0 )
		{
			pInfo->nCurSize = 0;
			ASSERT( afxData.cxBorder2 == afxData.cyBorder2 );
			(pInfo-1)->nCurSize += nSize + afxData.cxBorder2;
			nSize = 0;
		}
		else
		{
			ASSERT(nSize > 0 );
			if( pInfo->nCurSize == 0)
			{
				if( i != 0 )
					pInfo->nCurSize = 0;
			}
			else if( nSize < pInfo->nCurSize )
			{
				pInfo->nCurSize = nSize;
				nSize = 0;
			}
			else
				nSize -= pInfo->nCurSize;
		}
		ASSERT( nSize >= 0 );
		if( i != nMax - 1 )
		{
			if( nSize > nSizeSplitter )
			{
				nSize -= nSizeSplitter;
				ASSERT( nSize > 0 );
			}
			else
			{
				ASSERT( afxData.cxBorder2 == afxData.cyBorder2 );
				pInfo->nCurSize += nSize;
				if( pInfo->nCurSize > (nSizeSplitter - afxData.cxBorder2))
					pInfo->nCurSize -= (nSizeSplitter - afxData.cyBorder2);
				nSize = 0;
			}
		}
	}
	ASSERT(nSize == 0);
}

void CExtSplitterBaseWnd::stat_DeferClientPos( AFX_SIZEPARENTPARAMS* lpLayout, CWnd * pWnd, int x, int y, int cx, int cy, BOOL bScrollBar )
{
	ASSERT( pWnd != NULL );
	ASSERT( pWnd->m_hWnd != NULL );
	if( bScrollBar )
	{
		BOOL bNeedBorder = ( cx <= __EXT_MFC_CX_BORDER || cy <= __EXT_MFC_CY_BORDER );
		pWnd->ModifyStyle( bNeedBorder ? 0 : WS_BORDER, bNeedBorder ? WS_BORDER : 0 );
	}
CRect rect( x, y, x+cx, y+cy );
#if _MFC_VER < 0x700
	if( ! afxData.bWin4 )
	{
		if( bScrollBar )
			rect.InflateRect( __EXT_MFC_CX_BORDER, __EXT_MFC_CY_BORDER );
		else
			pWnd->CalcWindowRect( &rect );
	}
#endif //_MFC_VER < 0x700
	if( ( pWnd->GetExStyle() & WS_EX_CLIENTEDGE ) || pWnd->IsKindOf( RUNTIME_CLASS( CExtSplitterBaseWnd ) ) )
		rect.InflateRect( afxData.cxBorder2, afxData.cyBorder2 );
CRect rectOld;
	pWnd->GetWindowRect( rectOld );
	pWnd->GetParent()->ScreenToClient( &rectOld );
	if( rect != rectOld )
		AfxRepositionWindow( lpLayout, pWnd->m_hWnd, rect );
}

BEGIN_MESSAGE_MAP( CExtSplitterBaseWnd, CSplitterWnd )
	//{{AFX_MSG_MAP(CExtSplitterBaseWnd)
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_NCCREATE()
	ON_WM_SYSCOMMAND()
	ON_WM_CANCELMODE()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
	ON_MESSAGE_VOID( WM_DISPLAYCHANGE, OnDisplayChange )
	ON_MESSAGE_VOID( WM_WININICHANGE, OnDisplayChange )
	ON_MESSAGE_VOID( WM_SETTINGCHANGE, OnDisplayChange )
	ON_MESSAGE( WM_NEXTDLGCTL, OnNextDlgCtrl )
END_MESSAGE_MAP()

#ifdef _DEBUG

void CExtSplitterBaseWnd::AssertValid() const
{
	CWnd::AssertValid();
	ASSERT( m_nMaxRows >= 0 );
	ASSERT( m_nMaxCols >= 0 );
	ASSERT( m_nMaxCols >= 0 && m_nMaxRows >= 0 );
	ASSERT( m_nRows >= 0 );
	ASSERT( m_nCols >= 0 );
	ASSERT( m_nRows <= m_nMaxRows );
	ASSERT( m_nCols <= m_nMaxCols );
}

void CExtSplitterBaseWnd::Dump( CDumpContext & dc ) const
{
	CSplitterWnd::Dump( dc );
	if( m_pDynamicViewClass != NULL )
		dc << "m_pDynamicViewClass = " << m_pDynamicViewClass->m_lpszClassName;
	dc << "\nm_nMaxRows = " << m_nMaxRows;
	dc << "\nm_nMaxCols = " << m_nMaxCols;
	dc << "\nm_nRows = " << m_nRows;
	dc << "\nm_nCols = " << m_nCols;
	dc << "\nm_bHasHScroll = " << m_bHasHScroll;
	dc << "\nm_bHasVScroll = " << m_bHasVScroll;
	dc << "\nm_cxSplitter = " << m_cxSplitter;
	dc << "\nm_cySplitter = " << m_cySplitter;
	if( m_bTracking )
	{
		dc << "\nTRACKING: m_htTrack = " << m_htTrack;
		dc << "\nm_rectLimit = " << m_rectLimit;
		dc << "\nm_ptTrackOffset = " << m_ptTrackOffset;
		dc << "\nm_rectTracker = " << m_rectTracker;
		if( m_bTracking2)
			dc << "\nm_rectTracker2 = " << m_rectTracker2;
	}
	dc << "\n";
}

#endif

BOOL CExtSplitterBaseWnd::Create(
	CWnd * pParentWnd,
	int nMaxRows,
	int nMaxCols,
	SIZE sizeMin,
	CCreateContext * pContext,
	DWORD dwStyle,
	UINT nID
	)
{
	ASSERT( pParentWnd != NULL );
	ASSERT( sizeMin.cx > 0 && sizeMin.cy > 0 );
	ASSERT( pContext != NULL );
	ASSERT( pContext->m_pNewViewClass != NULL );
	ASSERT( dwStyle & WS_CHILD );
	ASSERT( dwStyle & SPLS_DYNAMIC_SPLIT );
	ASSERT( nMaxRows >= 1 && nMaxRows <= 2 );
	ASSERT( nMaxCols >= 1 && nMaxCols <= 2 );
	ASSERT( nMaxCols > 1 || nMaxRows > 1 );
	m_nMaxRows = nMaxRows;
	m_nMaxCols = nMaxCols;
	ASSERT( m_nRows == 0 && m_nCols == 0 );
	m_nRows = m_nCols = 1;
	if( ! CreateCommon( pParentWnd, sizeMin, dwStyle, nID ) )
		return FALSE;
	ASSERT( m_nRows == 1 && m_nCols == 1 );
	ASSERT( pContext->m_pNewViewClass->IsDerivedFrom( RUNTIME_CLASS( CWnd ) ) );
	m_pDynamicViewClass = pContext->m_pNewViewClass;
	if( ! CreateView( 0, 0, m_pDynamicViewClass, sizeMin, pContext ) )
	{
		DestroyWindow();
		return FALSE;
	}
	m_pColInfo[0].nIdealSize = sizeMin.cx;
	m_pRowInfo[0].nIdealSize = sizeMin.cy;
	return TRUE;
}

BOOL CExtSplitterBaseWnd::CreateStatic(
	CWnd * pParentWnd,
	int nRows,
	int nCols,
	DWORD dwStyle,
	UINT nID
	)
{
	ASSERT( pParentWnd != NULL );
	ASSERT( nRows >= 1 && nRows <= 16 );
	ASSERT( nCols >= 1 && nCols <= 16 );
	ASSERT( nCols > 1 || nRows > 1 );
	ASSERT( dwStyle & WS_CHILD );
	ASSERT( !(dwStyle & SPLS_DYNAMIC_SPLIT ) );
	ASSERT( m_nRows == 0 && m_nCols == 0 );
	m_nRows = m_nMaxRows = nRows;
	m_nCols = m_nMaxCols = nCols;
	if( ! CreateCommon( pParentWnd, CSize( 0, 0 ), dwStyle, nID ) )
		return FALSE;
	return TRUE;
}

BOOL CExtSplitterBaseWnd::CreateCommon( CWnd * pParentWnd, SIZE sizeMin, DWORD dwStyle, UINT nID )
{
	ASSERT( pParentWnd != NULL );
	ASSERT( sizeMin.cx >= 0 && sizeMin.cy >= 0 );
	ASSERT( dwStyle & WS_CHILD );
	ASSERT( nID != 0 );
	ASSERT( m_pColInfo == NULL && m_pRowInfo == NULL );
	ASSERT( m_nMaxCols > 0 && m_nMaxRows > 0 );
DWORD dwCreateStyle = dwStyle & ~(WS_HSCROLL|WS_VSCROLL);
#if _MFC_VER < 0x700
	if( afxData.bWin4 )
#endif //_MFC_VER < 0x700
		dwCreateStyle &= ~WS_BORDER;
	VERIFY( AfxDeferRegisterClass( AFX_WNDMDIFRAME_REG ) );
static const TCHAR _afxWndMDIFrame[] = AFX_WNDMDIFRAME;
	if( ! CreateEx( WS_EX_CONTROLPARENT, _afxWndMDIFrame, NULL, dwCreateStyle, 0, 0, 0, 0, pParentWnd->m_hWnd, (HMENU)nID, NULL ) )
		return FALSE;
	TRY
	{
		m_pColInfo = new CRowColInfo[ m_nMaxCols ];
		for( int col = 0; col < m_nMaxCols; col++ )
		{
			m_pColInfo[col].nMinSize = m_pColInfo[col].nIdealSize = sizeMin.cx;
			m_pColInfo[col].nCurSize = -1;
		}
		m_pRowInfo = new CRowColInfo[m_nMaxRows];
		for( int row = 0; row < m_nMaxRows; row++ )
		{
			m_pRowInfo[row].nMinSize = m_pRowInfo[row].nIdealSize = sizeMin.cy;
			m_pRowInfo[row].nCurSize = -1;
		}
		SetScrollStyle( dwStyle );
	}
	CATCH_ALL( e )
	{
		DestroyWindow();
		return FALSE;
	}
	END_CATCH_ALL
	return TRUE;
}

BOOL CExtSplitterBaseWnd::OnNcCreate( LPCREATESTRUCT lpcs )
{
	if( ! CSplitterWnd::OnNcCreate( lpcs ) )
		return FALSE;
CWnd * pParent = GetParent();
	ASSERT_VALID( pParent );
	pParent->ModifyStyleEx( WS_EX_CLIENTEDGE, 0, SWP_DRAWFRAME );
	return TRUE;
}

BOOL CExtSplitterBaseWnd::CreateView(
	int row,
	int col,
	CRuntimeClass * pViewClass,
	SIZE sizeInit,
	CCreateContext * pContext
	)
{
	ASSERT_VALID( this );
	ASSERT( row >= 0 && row < m_nRows );
	ASSERT( col >= 0 && col < m_nCols );
	ASSERT( pViewClass != NULL );
	ASSERT( pViewClass->IsDerivedFrom( RUNTIME_CLASS( CWnd ) ) );
	ASSERT( AfxIsValidAddress( pViewClass, sizeof( CRuntimeClass ), FALSE ) );
#ifdef _DEBUG
	if( GetDlgItem( IdFromRowCol( row, col ) ) != NULL )
	{
		TRACE2(" Error: CreateView - pane already exists for row %d, col %d.\n", row, col );
		ASSERT( FALSE );
		return FALSE;
	}
#endif
	m_pColInfo[col].nIdealSize = sizeInit.cx;
	m_pRowInfo[row].nIdealSize = sizeInit.cy;
BOOL bSendInitialUpdate = FALSE;
CCreateContext contextT;
	if( pContext == NULL )
	{
		CView * pOldView = (CView*)GetActivePane();
		if( pOldView != NULL && pOldView->IsKindOf( RUNTIME_CLASS( CView ) ) )
		{
			ASSERT( contextT.m_pCurrentFrame == NULL );
			contextT.m_pLastView = pOldView;
			contextT.m_pCurrentDoc = pOldView->GetDocument();
			if( contextT.m_pCurrentDoc != NULL )
				contextT.m_pNewDocTemplate = contextT.m_pCurrentDoc->GetDocTemplate();
		}
		pContext = &contextT;
		bSendInitialUpdate = TRUE;
	}
CWnd * pWnd = NULL;
	TRY
	{
		pWnd = (CWnd*)pViewClass->CreateObject();
		if( pWnd == NULL )
			AfxThrowMemoryException();
	}
	CATCH_ALL(e)
	{
		TRACE0("Out of memory creating a splitter pane.\n");
		// Note: DELETE_EXCEPTION(e) not required
		return FALSE;
	}
	END_CATCH_ALL
	ASSERT_KINDOF( CWnd, pWnd );
	ASSERT( pWnd->m_hWnd == NULL );
DWORD dwStyle = AFX_WS_DEFAULT_VIEW;
#if _MFC_VER < 0x700
	if( afxData.bWin4 )
#endif //_MFC_VER < 0x700
		dwStyle &= ~WS_BORDER;
CRect rect(CPoint(0,0), sizeInit);
	if( ! pWnd->Create( NULL, NULL, dwStyle, rect, this, IdFromRowCol(row, col), pContext ) )
	{
		TRACE0("Warning: couldn't create client pane for splitter.\n");
		return FALSE;
	}
	ASSERT( (int)_AfxGetDlgCtrlID( pWnd->m_hWnd ) == IdFromRowCol( row, col ) );
	if( bSendInitialUpdate )
		pWnd->SendMessage( WM_INITIALUPDATE );
	return TRUE;
}

BOOL CExtSplitterBaseWnd::CreateScrollBarCtrl( DWORD dwStyle, UINT nID )
{
	ASSERT_VALID( this );
	ASSERT(m_hWnd != NULL);
HWND hWnd =
		::CreateWindow(
			_T("SCROLLBAR"),
			NULL,
			dwStyle|WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
			0, 0, 1, 1,
			m_hWnd,
			(HMENU)nID,
			AfxGetInstanceHandle(),
			NULL
			);
#ifdef _DEBUG
	if( hWnd == NULL )
		TRACE1( "Warning: Window creation failed: GetLastError returns 0x%8.8X\n", GetLastError() );
#endif
	return hWnd != NULL;
}

int CExtSplitterBaseWnd::IdFromRowCol( int row, int col ) const
{
	ASSERT_VALID( this );
	ASSERT( row >= 0 );
	ASSERT( row < m_nRows );
	ASSERT( col >= 0 );
	ASSERT( col < m_nCols );
	return AFX_IDW_PANE_FIRST + row * 16 + col;
}

CWnd * CExtSplitterBaseWnd::GetPane( int row, int col ) const
{
	ASSERT_VALID( this );
CWnd * pView = GetDlgItem( IdFromRowCol(row, col) );
	ASSERT( pView != NULL );
	return pView;
}

BOOL CExtSplitterBaseWnd::IsChildPane( CWnd * pWnd, int * pRow, int * pCol )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pWnd );
UINT nID = _AfxGetDlgCtrlID( pWnd->m_hWnd );
	if( IsChild( pWnd ) && nID >= AFX_IDW_PANE_FIRST && nID <= AFX_IDW_PANE_LAST )
	{
		if( pRow != NULL )
			*pRow = (nID - AFX_IDW_PANE_FIRST) / 16;
		if( pCol != NULL )
			*pCol = (nID - AFX_IDW_PANE_FIRST) % 16;
		ASSERT( pRow == NULL || *pRow < m_nRows );
		ASSERT( pCol == NULL || *pCol < m_nCols );
		return TRUE;
	}
	else
	{
		if( pRow != NULL )
			*pRow = -1;
		if( pCol != NULL )
			*pCol = -1;
		return FALSE;
	}
}

void CExtSplitterBaseWnd::GetRowInfo( int row, int & cyCur, int & cyMin) const
{
	ASSERT_VALID( this );
	ASSERT( row >= 0 && row < m_nMaxRows );
	cyCur = m_pRowInfo[row].nCurSize;
	cyMin = m_pRowInfo[row].nMinSize;
}

void CExtSplitterBaseWnd::SetRowInfo(int row, int cyIdeal, int cyMin)
{
	ASSERT_VALID( this );
	ASSERT( row >= 0 && row < m_nMaxRows );
	ASSERT( cyIdeal >= 0 );
	ASSERT( cyMin >= 0 );
	m_pRowInfo[row].nIdealSize = cyIdeal;
	m_pRowInfo[row].nMinSize = cyMin;
}

void CExtSplitterBaseWnd::GetColumnInfo(int col, int& cxCur, int& cxMin) const
{
	ASSERT_VALID( this );
	ASSERT( col >= 0 && col < m_nMaxCols );
	cxCur = m_pColInfo[col].nCurSize;
	cxMin = m_pColInfo[col].nMinSize;
}

void CExtSplitterBaseWnd::SetColumnInfo(int col, int cxIdeal, int cxMin)
{
	ASSERT_VALID( this );
	ASSERT( col >= 0 && col < m_nMaxCols );
	ASSERT( cxIdeal >= 0 );
	ASSERT( cxMin >= 0 );
	m_pColInfo[col].nIdealSize = cxIdeal;
	m_pColInfo[col].nMinSize = cxMin;
}

DWORD CExtSplitterBaseWnd::GetScrollStyle() const
{
DWORD dwStyle = 0;
	if( m_bHasHScroll )
		dwStyle |= WS_HSCROLL;
	if( m_bHasVScroll )
		dwStyle |= WS_VSCROLL;
	return dwStyle;
}

void CExtSplitterBaseWnd::SetScrollStyle( DWORD dwStyle )
{
	dwStyle &= (WS_HSCROLL|WS_VSCROLL);
	if( GetScrollStyle() == dwStyle )
		return;
	m_bHasHScroll = (dwStyle & WS_HSCROLL) != 0;
	m_bHasVScroll = (dwStyle & WS_VSCROLL) != 0;
CWnd * pScrollBar = NULL;
	for( int col = 0; col < m_nCols; col++ )
	{
		pScrollBar = GetDlgItem( AFX_IDW_HSCROLL_FIRST + col );
		if( pScrollBar == NULL )
		{
			if( ! CreateScrollBarCtrl( SBS_HORZ, AFX_IDW_HSCROLL_FIRST + col ) )
				AfxThrowResourceException();
			pScrollBar = GetDlgItem( AFX_IDW_HSCROLL_FIRST + col );
		}
		pScrollBar->ShowWindow( m_bHasHScroll ? SW_SHOW : SW_HIDE );
	}
	for( int row = 0; row < m_nRows; row++ )
	{
		pScrollBar = GetDlgItem( AFX_IDW_VSCROLL_FIRST + row );
		if( pScrollBar == NULL )
		{
			if( ! CreateScrollBarCtrl( SBS_VERT, AFX_IDW_VSCROLL_FIRST + row ) )
				AfxThrowResourceException();
			pScrollBar = GetDlgItem( AFX_IDW_VSCROLL_FIRST + row );
		}
		pScrollBar->ShowWindow( m_bHasVScroll ? SW_SHOW : SW_HIDE );
	}
	if( m_bHasVScroll && m_bHasHScroll )
	{
		pScrollBar = GetDlgItem( AFX_IDW_SIZE_BOX );
		if( pScrollBar == NULL )
		{
			if( ! CreateScrollBarCtrl( SBS_SIZEBOX|WS_DISABLED, AFX_IDW_SIZE_BOX ) )
				AfxThrowResourceException();
			pScrollBar = GetDlgItem( AFX_IDW_SIZE_BOX );
		}
		pScrollBar->ShowWindow( SW_SHOW );
	}
	else
	{
		pScrollBar = GetDlgItem( AFX_IDW_SIZE_BOX );
		if( pScrollBar != NULL )
			pScrollBar->DestroyWindow();
	}
}

void CExtSplitterBaseWnd::DeleteView( int row, int col )
{
	ASSERT_VALID( this );
CWnd* pPane = GetPane(row, col);
	ASSERT_KINDOF( CView, pPane );
	if( GetActivePane() == pPane )
		ActivateNext( FALSE );
	pPane->DestroyWindow();
}

void CExtSplitterBaseWnd::OnDrawSplitter( CDC * pDC, CExtSplitterBaseWnd::ESplitType nType, const CRect & rectArg )
{
	nType;
	if( pDC == NULL )
	{
		RedrawWindow( rectArg, NULL, RDW_INVALIDATE|RDW_NOCHILDREN );
		return;
	}
//	ASSERT_VALID( pDC );
//CRect rect = rectArg;
//	switch( nType )
//	{
//	case splitBorder:
//#if _MFC_VER < 0x700
//		ASSERT( afxData.bWin4 );
//#endif //_MFC_VER < 0x700
//		pDC->Draw3dRect( rect, afxData.clrBtnShadow, afxData.clrBtnHilite );
//		rect.InflateRect( -__EXT_MFC_CX_BORDER, -__EXT_MFC_CY_BORDER );
//		pDC->Draw3dRect( rect, afxData.clrWindowFrame, afxData.clrBtnFace );
//	return;
//	case splitIntersection:
//#if _MFC_VER < 0x700
//		ASSERT( ! afxData.bWin4 );
//#endif //_MFC_VER < 0x700
//	break;
//	case splitBox:
//#if _MFC_VER < 0x700
//		if( afxData.bWin4 )
//#endif //_MFC_VER < 0x700
//		{
//			pDC->Draw3dRect(rect, afxData.clrBtnFace, afxData.clrWindowFrame);
//			rect.InflateRect(-__EXT_MFC_CX_BORDER, -__EXT_MFC_CY_BORDER);
//			pDC->Draw3dRect(rect, afxData.clrBtnHilite, afxData.clrBtnShadow);
//			rect.InflateRect(-__EXT_MFC_CX_BORDER, -__EXT_MFC_CY_BORDER);
//			break;
//		}
//		// fall through...
//	case splitBar:
//#if _MFC_VER < 0x700
//		if( ! afxData.bWin4 )
//		{
//			pDC->Draw3dRect(rect, afxData.clrBtnHilite, afxData.clrBtnShadow);
//			rect.InflateRect(-__EXT_MFC_CX_BORDER, -__EXT_MFC_CY_BORDER);
//		}
//#endif //_MFC_VER < 0x700
//	break;
//	default:
//		ASSERT(FALSE);
//	break;
//	}
//COLORREF clr = afxData.clrBtnFace;
//	pDC->FillSolidRect( rect, clr );
}

BOOL CExtSplitterBaseWnd::SplitRow( int cyBefore )
{
	ASSERT_VALID( this );
	ASSERT( GetStyle() & SPLS_DYNAMIC_SPLIT );
	ASSERT( m_pDynamicViewClass != NULL );
	ASSERT( m_nRows < m_nMaxRows );
	cyBefore -= m_cyBorder;
int rowNew = m_nRows;
int cyNew = stat_CanSplitRowCol( &m_pRowInfo[rowNew-1], cyBefore, m_cySplitter );
	if( cyNew == -1 )
		return FALSE;
	if(		m_bHasVScroll
		&&	(! CreateScrollBarCtrl( SBS_VERT, AFX_IDW_VSCROLL_FIRST + rowNew ) )
		)
	{
		TRACE0("Warning: SplitRow failed to create scroll bar.\n");
		return FALSE;
	}
	m_nRows++;
	for( int col = 0; col < m_nCols; col++ )
	{
		CSize size( m_pColInfo[col].nCurSize, cyNew );
		if( ! CreateView( rowNew, col, m_pDynamicViewClass, size, NULL ) )
		{
			TRACE0("Warning: SplitRow failed to create new row.\n");
			while( col > 0 )
				DeleteView( rowNew, --col );
			if( m_bHasVScroll )
				GetDlgItem( AFX_IDW_VSCROLL_FIRST + rowNew )->DestroyWindow();
			m_nRows--;
			return FALSE;
		}
	}
	m_pRowInfo[rowNew-1].nIdealSize = cyBefore;
	m_pRowInfo[rowNew].nIdealSize = cyNew;
	ASSERT(m_nRows == rowNew+1);
	RecalcLayout();
	return TRUE;
}

BOOL CExtSplitterBaseWnd::SplitColumn( int cxBefore )
{
	ASSERT_VALID( this );
	ASSERT( GetStyle() & SPLS_DYNAMIC_SPLIT );
	ASSERT( m_pDynamicViewClass != NULL );
	ASSERT( m_nCols < m_nMaxCols );
	cxBefore -= m_cxBorder;
int colNew = m_nCols;
int cxNew = stat_CanSplitRowCol(&m_pColInfo[colNew-1], cxBefore, m_cxSplitter);
	if( cxNew == -1 )
		return FALSE;
	if(		m_bHasHScroll
		&&	(! CreateScrollBarCtrl( SBS_HORZ, AFX_IDW_HSCROLL_FIRST + colNew ) )
		)
	{
		TRACE0("Warning: SplitRow failed to create scroll bar.\n");
		return FALSE;
	}
	m_nCols++;
	for( int row = 0; row < m_nRows; row++ )
	{
		CSize size( cxNew, m_pRowInfo[row].nCurSize );
		if( ! CreateView( row, colNew, m_pDynamicViewClass, size, NULL ) )
		{
			TRACE0("Warning: SplitColumn failed to create new column.\n");
			while( row > 0 )
				DeleteView( --row, colNew );
			if( m_bHasHScroll )
				GetDlgItem( AFX_IDW_HSCROLL_FIRST + colNew )->DestroyWindow();
			m_nCols--;
			return FALSE;
		}
	}
	m_pColInfo[colNew-1].nIdealSize = cxBefore;
	m_pColInfo[colNew].nIdealSize = cxNew;
	ASSERT( m_nCols == colNew+1 );
	RecalcLayout();
	return TRUE;
}

void CExtSplitterBaseWnd::DeleteRow(int rowDelete)
{
	ASSERT_VALID( this );
	ASSERT( GetStyle() & SPLS_DYNAMIC_SPLIT );
	ASSERT( m_nRows > 1 );
	ASSERT( rowDelete < m_nRows );
int rowActive, colActive;
	if( GetActivePane( &rowActive, &colActive ) != NULL && rowActive == rowDelete )
	{
		if( ++rowActive >= m_nRows )
			rowActive = 0;
		SetActivePane( rowActive, colActive );
	}
CWnd * pScrollDel = m_bHasVScroll ? GetDlgItem( AFX_IDW_VSCROLL_FIRST + rowDelete ) : NULL;
	for( int col = 0; col < m_nCols; col++ )
	{
		DeleteView( rowDelete, col );
		for( int row = rowDelete+1; row < m_nRows; row++ )
		{
			CWnd * pPane = GetPane( row, col );
			ASSERT( pPane != NULL );
			pPane->SetDlgCtrlID( IdFromRowCol( row-1, col ) );
			if( m_bHasVScroll && col == m_nCols-1 )
			{
				CWnd * pScroll = GetDlgItem( AFX_IDW_VSCROLL_FIRST + row );
				if( pScroll != NULL )
					pScroll->SetDlgCtrlID( AFX_IDW_VSCROLL_FIRST + row - 1 );
			}
		}
	}
	m_nRows--;
	if( pScrollDel != NULL )
		pScrollDel->DestroyWindow();
	RecalcLayout();
}

void CExtSplitterBaseWnd::DeleteColumn( int colDelete )
{
	ASSERT_VALID( this );
	ASSERT( GetStyle() & SPLS_DYNAMIC_SPLIT );
	ASSERT( m_nCols > 1 );
	ASSERT( colDelete < m_nCols );
int rowActive, colActive;
	if( GetActivePane( &rowActive, &colActive ) != NULL && colActive == colDelete )
	{
		if( ++colActive >= m_nCols )
			colActive = 0;
		SetActivePane( rowActive, colActive );
	}
CWnd * pScrollDel = m_bHasHScroll ? GetDlgItem( AFX_IDW_HSCROLL_FIRST + colDelete ) : NULL;
	for( int row = 0; row < m_nRows; row++ )
	{
		DeleteView( row, colDelete );
		for( int col = colDelete+1; col < m_nCols; col++ )
		{
			CWnd * pPane = GetPane( row, col );
			ASSERT( pPane != NULL );
			pPane->SetDlgCtrlID( IdFromRowCol( row, col-1 ) );
			if( m_bHasHScroll && row == m_nRows - 1 )
			{
				CWnd * pScroll = GetDlgItem( AFX_IDW_HSCROLL_FIRST + col );
				if( pScroll != NULL )
					pScroll->SetDlgCtrlID( AFX_IDW_HSCROLL_FIRST + col - 1 );
			}
		}
	}
	m_nCols--;
	if( pScrollDel != NULL )
		pScrollDel->DestroyWindow();
	RecalcLayout();
}

void CExtSplitterBaseWnd::GetInsideRect( CRect & rect ) const
{
	ASSERT_VALID( this );
	GetClientRect( rect );
	ASSERT( rect.left == 0 && rect.top == 0 );
	rect.InflateRect( -m_cxBorder, -m_cyBorder );
	if( m_bHasVScroll )
		rect.right -= afxData.cxVScroll - __EXT_MFC_CX_BORDER;
	if( m_bHasHScroll )
		rect.bottom -= afxData.cyHScroll - __EXT_MFC_CY_BORDER;
}

void CExtSplitterBaseWnd::StartTracking( int ht )
{
	ASSERT_VALID( this );
	if( ht == noHit )
		return;
	GetInsideRect( m_rectLimit );
	if( ht >= splitterIntersection1 && ht <= splitterIntersection225 )
	{
		int row = (ht - splitterIntersection1) / 15;
		int col = (ht - splitterIntersection1) % 15;
		GetHitRect( row + vSplitterBar1, m_rectTracker );
		int yTrackOffset = m_ptTrackOffset.y;
		m_bTracking2 = TRUE;
		GetHitRect( col + hSplitterBar1, m_rectTracker2 );
		m_ptTrackOffset.y = yTrackOffset;
	}
	else if( ht == bothSplitterBox )
	{
		GetHitRect(vSplitterBox, m_rectTracker);
		int yTrackOffset = m_ptTrackOffset.y;
		m_bTracking2 = TRUE;
		GetHitRect( hSplitterBox, m_rectTracker2 );
		m_ptTrackOffset.y = yTrackOffset;
		m_rectTracker.OffsetRect( 0, m_rectLimit.Height()/2 );
		m_rectTracker2.OffsetRect( m_rectLimit.Width()/2, 0 );
	}
	else
		GetHitRect(ht, m_rectTracker);
CView * pView = (CView*)GetActivePane();
	if( pView != NULL && pView->IsKindOf( RUNTIME_CLASS( CView ) )  )
	{
		ASSERT_VALID( pView );
		CFrameWnd * pFrameWnd = GetParentFrame();
		if( pFrameWnd != NULL )
		{
			ASSERT_VALID( pFrameWnd );
			((friently_view_t*)pView)->OnActivateFrame( WA_INACTIVE, pFrameWnd );
		}
	}
	SetCapture();
	SetFocus();
	RedrawWindow( NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW );
	m_bTracking = TRUE;
	OnInvertTracker( m_rectTracker );
	if( m_bTracking2 )
		OnInvertTracker( m_rectTracker2 );
	m_htTrack = ht;
	SetSplitCursor( ht );
}

void CExtSplitterBaseWnd::TrackRowSize( int y, int row )
{
	ASSERT_VALID( this );
	ASSERT( m_nRows > 1 );
CPoint pt( 0, y );
	ClientToScreen( & pt);
	GetPane( row, 0 )->ScreenToClient( &pt );
	m_pRowInfo[row].nIdealSize = pt.y;
	if( pt.y < m_pRowInfo[row].nMinSize )
	{
		m_pRowInfo[row].nIdealSize = 0;
		if( GetStyle() & SPLS_DYNAMIC_SPLIT )
			DeleteRow( row );
	}
	else if( m_pRowInfo[row].nCurSize + m_pRowInfo[row+1].nCurSize < pt.y + m_pRowInfo[row+1].nMinSize )
	{
		if( GetStyle() & SPLS_DYNAMIC_SPLIT )
			DeleteRow( row + 1 );
	}
}

void CExtSplitterBaseWnd::TrackColumnSize( int x, int col )
{
	ASSERT_VALID( this );
	ASSERT(m_nCols > 1);
CPoint pt(x, 0);
	ClientToScreen(&pt);
	GetPane( 0, col )->ScreenToClient( &pt );
	m_pColInfo[col].nIdealSize = pt.x;
	if( pt.x < m_pColInfo[col].nMinSize )
	{
		m_pColInfo[col].nIdealSize = 0;
		if( GetStyle() & SPLS_DYNAMIC_SPLIT )
			DeleteColumn(col);
	}
	else if( m_pColInfo[col].nCurSize + m_pColInfo[col+1].nCurSize < pt.x + m_pColInfo[col+1].nMinSize )
	{
		if( GetStyle() & SPLS_DYNAMIC_SPLIT )
			DeleteColumn(col + 1);
	}
}

void CExtSplitterBaseWnd::StopTracking(BOOL bAccept)
{
	ASSERT_VALID( this );
	if( ! m_bTracking )
		return;
	ReleaseCapture();
	OnInvertTracker( m_rectTracker );
	if( m_bTracking2 )
		OnInvertTracker( m_rectTracker2 );
	m_bTracking = m_bTracking2 = FALSE;
CWnd * pOldActiveView = GetActivePane();
	m_rectTracker.OffsetRect( -__EXT_MFC_CX_BORDER , -__EXT_MFC_CY_BORDER );
	m_rectTracker2.OffsetRect( -__EXT_MFC_CX_BORDER, -__EXT_MFC_CY_BORDER );
	if( bAccept )
	{
		if( m_htTrack == vSplitterBox )
			SplitRow( m_rectTracker.top );
		else if( m_htTrack >= vSplitterBar1 && m_htTrack <= vSplitterBar15 )
		{
			TrackRowSize( m_rectTracker.top, m_htTrack - vSplitterBar1 );
			RecalcLayout();
		}
		else if( m_htTrack == hSplitterBox )
			SplitColumn(m_rectTracker.left);
		else if( m_htTrack >= hSplitterBar1 && m_htTrack <= hSplitterBar15 )
		{
			TrackColumnSize( m_rectTracker.left, m_htTrack - hSplitterBar1 );
			RecalcLayout();
		}
		else if( m_htTrack >= splitterIntersection1 && m_htTrack <= splitterIntersection225 )
		{
			int row = (m_htTrack - splitterIntersection1) / 15;
			int col = (m_htTrack - splitterIntersection1) % 15;
			TrackRowSize( m_rectTracker.top, row );
			TrackColumnSize( m_rectTracker2.left, col );
			RecalcLayout();
		}
		else if( m_htTrack == bothSplitterBox )
		{
			SplitRow( m_rectTracker.top );
			SplitColumn( m_rectTracker2.left );
		}
	}
	if( pOldActiveView == GetActivePane() )
	{
		if( pOldActiveView != NULL)
		{
			SetActivePane( -1, -1, pOldActiveView );
			pOldActiveView->SetFocus();
		}
	}
}

void CExtSplitterBaseWnd::GetHitRect( int ht, CRect & rectHit )
{
	ASSERT_VALID( this );
CRect rectClient;
	GetClientRect( &rectClient );
	rectClient.InflateRect( -m_cxBorder, -m_cyBorder );
int cx = rectClient.Width();
int cy = rectClient.Height();
int x = rectClient.top;
int y = rectClient.left;
	m_ptTrackOffset.x = 0;
	m_ptTrackOffset.y = 0;
	if( ht == vSplitterBox )
	{
		cy =
			m_cySplitter - ( 2*m_cyBorder -
#if _MFC_VER < 0x700
				afxData.bWin4
#else
				1
#endif
			);
		m_ptTrackOffset.y = -(cy / 2);
		ASSERT( m_pRowInfo[0].nCurSize > 0 );
		m_rectLimit.bottom -= cy;
	}
	else if( ht == hSplitterBox )
	{
		cx =
			m_cxSplitter - ( 2*m_cxBorder -
#if _MFC_VER < 0x700
				afxData.bWin4
#else
				1
#endif
			);
		m_ptTrackOffset.x = -(cx / 2);
		ASSERT( m_pColInfo[0].nCurSize > 0 );
		m_rectLimit.right -= cx;
	}
	else if( ht >= vSplitterBar1 && ht <= vSplitterBar15 )
	{
		cy =
			m_cySplitter - ( 2*m_cyBorder -
#if _MFC_VER < 0x700
				afxData.bWin4
#else
				1
#endif
				);
		m_ptTrackOffset.y = -(cy / 2);
		int row = 0;
		for( ; row < ht - vSplitterBar1; row++ )
			y += m_pRowInfo[row].nCurSize + m_cySplitterGap;
		m_rectLimit.top = y;
		y +=
			m_pRowInfo[row].nCurSize + m_cyBorderShare +
#if _MFC_VER < 0x700
				afxData.bWin4
#else
				1
#endif
			;
		m_rectLimit.bottom -= cy;
	}
	else if( ht >= hSplitterBar1 && ht <= hSplitterBar15 )
	{
		cx =
			m_cxSplitter - ( 2*m_cxBorder -
#if _MFC_VER < 0x700
				afxData.bWin4
#else
				1
#endif
			);
		m_ptTrackOffset.x = -(cx / 2);
		int col = 0;
		for( ; col < ht - hSplitterBar1; col++ )
			x += m_pColInfo[col].nCurSize + m_cxSplitterGap;
		m_rectLimit.left = x;
		x +=
			m_pColInfo[col].nCurSize + m_cxBorderShare +
#if _MFC_VER < 0x700
				afxData.bWin4
#else
				1
#endif
			;
		m_rectLimit.right -= cx;
	}
	else
	{
		TRACE1("Error: GetHitRect(%d): Not Found!\n", ht);
		ASSERT(FALSE);
	}
	rectHit.right = (rectHit.left = x) + cx;
	rectHit.bottom = (rectHit.top = y) + cy;
}

int CExtSplitterBaseWnd::HitTest( CPoint pt ) const
{
	ASSERT_VALID( this );
CRect rectClient;
	GetClientRect(&rectClient);
	rectClient.InflateRect(-m_cxBorder, -m_cyBorder);
CRect rectInside;
	GetInsideRect(rectInside);
	if(		m_bHasVScroll
		&&	m_nRows < m_nMaxRows
		&&	CRect(
				rectInside.right,
				rectClient.top,
				rectClient.right,
				rectClient.top + m_cySplitter -
#if _MFC_VER < 0x700
					afxData.bWin4
#else
					1
#endif
				).PtInRect( pt )
		)
		return vSplitterBox;

	if(		m_bHasHScroll
		&&	m_nCols < m_nMaxCols
		&&	CRect(
				rectClient.left,
				rectInside.bottom,
				rectClient.left + m_cxSplitter -
#if _MFC_VER < 0x700
					afxData.bWin4
#else
					1
#endif
					,
				rectClient.bottom
				).PtInRect( pt )
		)
		return hSplitterBox;
CRect rect;
	rect = rectClient;
	int col = 0;
	for( ; col < m_nCols - 1; col++ )
	{
		rect.left += m_pColInfo[col].nCurSize;
		rect.right = rect.left + m_cxSplitterGap;
		if( rect.PtInRect(pt) )
			break;
		rect.left = rect.right;
	}
	rect = rectClient;
	int row = 0;
	for( ; row < m_nRows - 1; row++ )
	{
		rect.top += m_pRowInfo[row].nCurSize;
		rect.bottom = rect.top + m_cySplitterGap;
		if( rect.PtInRect(pt))
			break;
		rect.top = rect.bottom;
	}
	if( col != m_nCols - 1 )
	{
		if( row != m_nRows - 1 )
			return splitterIntersection1 + row * 15 + col;
		return hSplitterBar1 + col;
	}
	if( row != m_nRows - 1 )
		return vSplitterBar1 + row;
	return noHit;
}

void CExtSplitterBaseWnd::OnInvertTracker( const CRect & rect )
{
	ASSERT_VALID( this );
	ASSERT( ! rect.IsRectEmpty() );
	//ASSERT( (GetStyle() & WS_CLIPCHILDREN ) == 0 );
CDC * pDC = GetDCEx( NULL, DCX_WINDOW|DCX_CACHE );
	ASSERT( pDC != NULL );
CBrush * pBrush = CDC::GetHalftoneBrush();
HBRUSH hOldBrush = NULL;
	if( pBrush != NULL)
		hOldBrush = (HBRUSH)SelectObject( pDC->m_hDC, pBrush->m_hObject );
	pDC->PatBlt( rect.left, rect.top, rect.Width(), rect.Height(), PATINVERT );
	if( hOldBrush != NULL )
		SelectObject( pDC->m_hDC, hOldBrush );
	ReleaseDC(pDC);
}

BOOL CExtSplitterBaseWnd::DoKeyboardSplit()
{
	ASSERT_VALID( this );
int ht;
	if( m_nRows > 1 && m_nCols > 1 )
		ht = splitterIntersection1;
	else if( m_nRows > 1 )
		ht = vSplitterBar1;
	else if( m_nCols > 1 )
		ht = hSplitterBar1;
	else if( m_nMaxRows > 1 && m_nMaxCols > 1 )
		ht = bothSplitterBox;
	else if( m_nMaxRows > 1 )
		ht = vSplitterBox;
	else if( m_nMaxCols > 1 )
		ht = hSplitterBox;
	else
		return FALSE;
	StartTracking( ht );
CRect rect;
	rect.left = m_rectTracker.Width() / 2;
	rect.top = m_rectTracker.Height() / 2;
	if( m_ptTrackOffset.y != 0 )
		rect.top = m_rectTracker.top;
	if( m_ptTrackOffset.x != 0 )
		rect.left = m_bTracking2 ? m_rectTracker2.left :m_rectTracker.left;
	rect.OffsetRect( -m_ptTrackOffset.x, -m_ptTrackOffset.y );
	ClientToScreen( &rect );
	SetCursorPos( rect.left, rect.top );
	return TRUE;
}

void CExtSplitterBaseWnd::OnDisplayChange()
{
	ASSERT_VALID( this );
	if( !IsIconic() && IsWindowVisible() )
		RecalcLayout();
}

LRESULT CExtSplitterBaseWnd::OnNextDlgCtrl( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	if( LOWORD(lParam) != 0 )
		return 1;
HWND hWndParent = ::GetParent( m_hWnd );
	if( hWndParent == NULL )
		return 1;
HWND hWndControl = ::GetFocus();
	if( hWndControl == NULL )
		return 1;
BOOL bPrevious = ( wParam != 0 ) ? TRUE : FALSE;
HWND hWndNewFocus = CExtResizableDialog::stat_GetNextItemZ( hWndParent, hWndParent, hWndControl, bPrevious );
	if( hWndNewFocus != NULL )
		::SetFocus( hWndNewFocus );
	return 0;
}

void CExtSplitterBaseWnd::OnSize( UINT nType, int cx, int cy )
{
	ASSERT_VALID( this );

	if( nType != SIZE_MINIMIZED && cx > 0 && cy > 0 )
		RecalcLayout();
	CSplitterWnd::OnSize( nType, cx, cy );
}

CWnd * CExtSplitterBaseWnd::GetSizingParent()
{
	ASSERT_VALID( this );
#if _MFC_VER < 0x700
	if( ! afxData.bWin4 )
		return NULL;
#endif //_MFC_VER < 0x700
CRect rectClient;
	GetClientRect(rectClient);
CWnd * pParent = this;
	if( ! ( pParent->GetStyle() & WS_THICKFRAME ) )
		pParent = GetParent();
	ASSERT_VALID( pParent );
	if( ( pParent->GetStyle() & (WS_THICKFRAME|WS_MAXIMIZE) ) == WS_THICKFRAME )
	{
		CRect rect;
		pParent->GetClientRect( &rect );
		pParent->ClientToScreen( &rect );
		ScreenToClient( &rect );
		if( rectClient.BottomRight() == rect.BottomRight() )
			return pParent;
	}
	return NULL;
}

void CExtSplitterBaseWnd::RecalcLayout()
{
	ASSERT_VALID( this );
	ASSERT( m_nRows > 0 && m_nCols > 0 );
CRect rectClient;
	GetClientRect( rectClient );
	rectClient.InflateRect( -m_cxBorder, -m_cyBorder );
CRect rectInside;
	GetInsideRect( rectInside );
	stat_LayoutRowCol( m_pColInfo, m_nCols, rectInside.Width(), m_cxSplitterGap );
	stat_LayoutRowCol( m_pRowInfo, m_nRows, rectInside.Height(), m_cySplitterGap );
AFX_SIZEPARENTPARAMS layout;
	layout.hDWP = ::BeginDeferWindowPos( (m_nCols + 1) * (m_nRows + 1) + 1 );
int cx = (rectClient.right - rectInside.right) -
#if _MFC_VER < 0x700
			afxData.bNotWin4
#else
			0
#endif
			;
int cy = (rectClient.bottom - rectInside.bottom) -
#if _MFC_VER < 0x700
			afxData.bNotWin4
#else
			0
#endif
			;
	if( m_bHasHScroll && m_bHasVScroll )
	{
		CWnd* pScrollBar = GetDlgItem( AFX_IDW_SIZE_BOX );
		ASSERT( pScrollBar != NULL );
		BOOL bSizingParent = (GetSizingParent() != NULL);
		if( pScrollBar->ModifyStyle(SBS_SIZEGRIP|SBS_SIZEBOX, bSizingParent ? SBS_SIZEGRIP : SBS_SIZEBOX ) )
			pScrollBar->Invalidate();
		pScrollBar->EnableWindow(bSizingParent);
		stat_DeferClientPos(
			&layout,
			pScrollBar,
			rectInside.right +
#if _MFC_VER < 0x700
				afxData.bNotWin4
#else
				0
#endif
				,
			rectInside.bottom +
#if _MFC_VER < 0x700
				afxData.bNotWin4
#else
				0
#endif
				,
			cx,
			cy,
			TRUE
			);
	}
	if( m_bHasHScroll )
	{
		int cxSplitterBox =
			m_cxSplitter +
#if _MFC_VER < 0x700
				afxData.bNotWin4
#else
				0
#endif
				;
		int x = rectClient.left;
		int y =
			rectInside.bottom +
#if _MFC_VER < 0x700
				afxData.bNotWin4
#else
				0
#endif
				;
		for( int col = 0; col < m_nCols; col++ )
		{
			CWnd* pScrollBar = GetDlgItem( AFX_IDW_HSCROLL_FIRST + col );
			ASSERT( pScrollBar != NULL );
			int cx = m_pColInfo[col].nCurSize;
			if( col == 0 && m_nCols < m_nMaxCols )
				x += cxSplitterBox, cx -= cxSplitterBox;
			stat_DeferClientPos( &layout, pScrollBar, x, y, cx, cy, TRUE );
			x += cx + m_cxSplitterGap;
		}
	}
	if( m_bHasVScroll )
	{
		int cySplitterBox =
			m_cySplitter +
#if _MFC_VER < 0x700
				afxData.bNotWin4
#else
				0
#endif
				;
		int x =
			rectInside.right +
#if _MFC_VER < 0x700
				afxData.bNotWin4
#else
				0
#endif
				;
		int y = rectClient.top;
		for( int row = 0; row < m_nRows; row++ )
		{
			CWnd* pScrollBar = GetDlgItem( AFX_IDW_VSCROLL_FIRST + row );
			ASSERT(pScrollBar != NULL);
			int cy = m_pRowInfo[row].nCurSize;
			if( row == 0 && m_nRows < m_nMaxRows )
				y += cySplitterBox, cy -= cySplitterBox;
			stat_DeferClientPos( &layout, pScrollBar, x, y, cx, cy, TRUE );
			y += cy + m_cySplitterGap;
		}
	}

	{
		int x = rectClient.left;
		for( int col = 0; col < m_nCols; col++ )
		{
			int cx = m_pColInfo[col].nCurSize;
			int y = rectClient.top;
			for( int row = 0; row < m_nRows; row++)
			{
				int cy = m_pRowInfo[row].nCurSize;
				CWnd* pWnd = GetPane(row, col);
				stat_DeferClientPos( &layout, pWnd, x, y, cx, cy, FALSE );
				y += cy + m_cySplitterGap;
			}
			x += cx + m_cxSplitterGap;
		}
	}

	if( layout.hDWP == NULL || !::EndDeferWindowPos( layout.hDWP ) )
		TRACE0("Warning: DeferWindowPos failed - low system resources.\n");

	DrawAllSplitBars( NULL, rectInside.right, rectInside.bottom );
}

void CExtSplitterBaseWnd::DrawAllSplitBars( CDC* pDC, int cxInside, int cyInside )
{
	ASSERT_VALID( this );
CRect rect;
	GetClientRect(rect);
	rect.left += m_cxBorder;
int col = 0;
	for( ; col < m_nCols - 1; col++ )
	{
		rect.left += m_pColInfo[col].nCurSize + m_cxBorderShare;
		rect.right = rect.left + m_cxSplitter;
		if( rect.left > cxInside)
			break;
		OnDrawSplitter(pDC, splitBar, rect);
		rect.left = rect.right + m_cxBorderShare;
	}

	GetClientRect(rect);
	rect.top += m_cyBorder;
int row = 0;
	for( ; row < m_nRows - 1; row++ )
	{
		rect.top += m_pRowInfo[row].nCurSize + m_cyBorderShare;
		rect.bottom = rect.top + m_cySplitter;
		if( rect.top > cyInside)
			break;
		OnDrawSplitter( pDC, splitBar, rect );
		rect.top = rect.bottom + m_cyBorderShare;
	}

#if _MFC_VER < 0x700
	if( afxData.bWin4 )
#endif //_MFC_VER < 0x700
	{
		GetClientRect( rect );
		int x = rect.left;
		for( col = 0; col < m_nCols; col++)
		{
			int cx = m_pColInfo[col].nCurSize + 2*m_cxBorder;
			if( col == m_nCols-1 && m_bHasVScroll)
				cx += afxData.cxVScroll - __EXT_MFC_CX_BORDER;
			int y = rect.top;
			for( int row = 0; row < m_nRows; row++)
			{
				int cy = m_pRowInfo[row].nCurSize + 2*m_cyBorder;
				if( row == m_nRows-1 && m_bHasHScroll)
					cy += afxData.cyHScroll - __EXT_MFC_CY_BORDER;
				OnDrawSplitter( pDC, splitBorder, CRect( x, y, x+cx, y+cy ) );
				y += cy + m_cySplitterGap - 2*m_cyBorder;
			}
			x += cx + m_cxSplitterGap - 2*m_cxBorder;
		}
	}
}

void CExtSplitterBaseWnd::OnPaint()
{
	ASSERT_VALID( this );
CRect rectClient;
	GetClientRect( &rectClient );
CPaintDC dcPaint( this );
CExtMemoryDC dc( &dcPaint, &rectClient );
	CExtPaintManager::stat_ExcludeChildAreas(
		dc,
		GetSafeHwnd(),
		CExtPaintManager::stat_DefExcludeChildAreaCallback
		);
	if( ! PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
	{
		COLORREF clrBackground = OnQueryBackgroundColor();
		dc.FillSolidRect( 
			&rectClient, 
			( clrBackground != COLORREF(-1L) ) 
				? clrBackground 
				: PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT, this ) 
			);
	}

	rectClient.InflateRect( -m_cxBorder, -m_cyBorder );
CRect rectInside;
	GetInsideRect(rectInside);
	if( m_bHasVScroll && m_nRows < m_nMaxRows )
	{
		OnDrawSplitter(
			&dc,
			splitBox,
			CRect(
				rectInside.right +
#if _MFC_VER < 0x700
					afxData.bNotWin4
#else
					0
#endif
					,
				rectClient.top, rectClient.right,
				rectClient.top + m_cySplitter
				)
			);
	}

	if( m_bHasHScroll && m_nCols < m_nMaxCols )
	{
		OnDrawSplitter(
			&dc,
			splitBox,
			CRect(
				rectClient.left,
				rectInside.bottom +
#if _MFC_VER < 0x700
					afxData.bNotWin4
#else
					0
#endif
					,
				rectClient.left + m_cxSplitter,
				rectClient.bottom
				)
			);
	}

	DrawAllSplitBars( &dc, rectInside.right, rectInside.bottom );

#if _MFC_VER < 0x700
	if( ! afxData.bWin4 )
	{
		GetInsideRect( rectInside );
		dc.IntersectClipRect( rectInside );
		CRect rect;
		rect.top = rectInside.top;
		for( int row = 0; row < m_nRows - 1; row++ )
		{
			rect.top += m_pRowInfo[row].nCurSize + m_cyBorderShare;
			rect.bottom = rect.top + m_cySplitter;
			rect.left = rectInside.left;
			for( int col = 0; col < m_nCols - 1; col++ )
			{
				rect.left += m_pColInfo[col].nCurSize + m_cxBorderShare;
				rect.right = rect.left + m_cxSplitter;
				OnDrawSplitter(&dc, splitIntersection, rect);
				rect.left = rect.right + m_cxBorderShare;
			}
			rect.top = rect.bottom + m_cxBorderShare;
		}
	}
#endif //_MFC_VER < 0x700
}

BOOL CExtSplitterBaseWnd::OnSetCursor( CWnd * pWnd, UINT nHitTest, UINT message )
{
	if( nHitTest == HTCLIENT && pWnd == this && ! m_bTracking )
		return TRUE;
	return CSplitterWnd::OnSetCursor(pWnd, nHitTest, message);
}

void CExtSplitterBaseWnd::SetSplitCursor( int ht )
{
UINT idcPrimary;
LPCTSTR idcSecondary;
	AfxLockGlobals( CRIT_SPLITTERWND );
	if(		ht == vSplitterBox
		||	ht >= vSplitterBar1
		&&	ht <= vSplitterBar15
		)
	{
		idcPrimary = AFX_IDC_VSPLITBAR;
		idcSecondary = IDC_SIZENS;
	}
	else if(
			ht == hSplitterBox
		||	ht >= hSplitterBar1
		&&	ht <= hSplitterBar15
		)
	{
		idcPrimary = AFX_IDC_HSPLITBAR;
		idcSecondary = IDC_SIZEWE;
	}
	else if(
			ht == bothSplitterBox
		||	( ht >= splitterIntersection1 && ht <= splitterIntersection225 )
		)
	{
		idcPrimary = AFX_IDC_SMALLARROWS;
		idcSecondary = IDC_SIZEALL;
	}
	else
	{
		SetCursor( afxData.hcurArrow );
		idcPrimary = 0;
		idcSecondary = 0;
	}

	if( idcPrimary != 0 )
	{
		HCURSOR hcurToDestroy = NULL;
		if( idcPrimary != g_split_idcPrimaryLast )
		{
			HINSTANCE hInst = AfxFindResourceHandle( MAKEINTRESOURCE(idcPrimary), RT_GROUP_CURSOR );
			hcurToDestroy = g_split_hcurDestroy;
			if( ( g_split_hcurDestroy = g_split_hcurLast = ::LoadCursor(hInst, MAKEINTRESOURCE(idcPrimary) ) ) == NULL )
			{
				TRACE0("Warning: Could not find splitter cursor - using system provided alternative.\n");
				ASSERT( g_split_hcurDestroy == NULL );
				g_split_hcurLast = ::LoadCursor( NULL, idcSecondary );
				ASSERT(g_split_hcurLast != NULL);
			}
			g_split_idcPrimaryLast = idcPrimary;
		}
		ASSERT(g_split_hcurLast != NULL);
		::SetCursor(g_split_hcurLast);
		ASSERT(g_split_hcurLast != hcurToDestroy);
		if( hcurToDestroy != NULL)
			::DestroyCursor( hcurToDestroy );
	}
	AfxUnlockGlobals( CRIT_SPLITTERWND );
}

void CExtSplitterBaseWnd::OnMouseMove( UINT /*nFlags*/, CPoint pt )
{
	if( GetCapture() != this )
		StopTracking( FALSE );
	if( m_bTracking )
	{
		pt.Offset( m_ptTrackOffset );
		if( pt.y < m_rectLimit.top )
			pt.y = m_rectLimit.top;
		else if( pt.y > m_rectLimit.bottom)
			pt.y = m_rectLimit.bottom;
		if( pt.x < m_rectLimit.left )
			pt.x = m_rectLimit.left;
		else if( pt.x > m_rectLimit.right )
			pt.x = m_rectLimit.right;
		if(		m_htTrack == vSplitterBox
			||	m_htTrack >= vSplitterBar1 && m_htTrack <= vSplitterBar15
			)
		{
			if( m_rectTracker.top != pt.y )
			{
				OnInvertTracker( m_rectTracker );
				m_rectTracker.OffsetRect( 0, pt.y - m_rectTracker.top );
				OnInvertTracker( m_rectTracker );
			}
		}
		else if(
				m_htTrack == hSplitterBox
			||	m_htTrack >= hSplitterBar1 && m_htTrack <= hSplitterBar15
			)
		{
			if( m_rectTracker.left != pt.x)
			{
				OnInvertTracker(m_rectTracker);
				m_rectTracker.OffsetRect(pt.x - m_rectTracker.left, 0);
				OnInvertTracker(m_rectTracker);
			}
		}
		else if(
				m_htTrack == bothSplitterBox
			||	( m_htTrack >= splitterIntersection1 && m_htTrack <= splitterIntersection225 )
			)
		{
			if( m_rectTracker.top != pt.y )
			{
				OnInvertTracker( m_rectTracker );
				m_rectTracker.OffsetRect( 0, pt.y - m_rectTracker.top );
				OnInvertTracker( m_rectTracker );
			}
			if( m_rectTracker2.left != pt.x )
			{
				OnInvertTracker( m_rectTracker2 );
				m_rectTracker2.OffsetRect( pt.x - m_rectTracker2.left, 0 );
				OnInvertTracker( m_rectTracker2 );
			}
		}
	}
	else
	{
		int ht = HitTest( pt );
		SetSplitCursor( ht );
	}
}

void CExtSplitterBaseWnd::OnLButtonDown( UINT /*nFlags*/, CPoint pt )
{
	if( m_bTracking )
		return;
	StartTracking( HitTest( pt ) );
}

void CExtSplitterBaseWnd::OnLButtonDblClk( UINT /*nFlags*/, CPoint pt )
{
int ht = HitTest( pt );
CRect rect;
	StopTracking( FALSE );
	if( ( GetStyle() & SPLS_DYNAMIC_SPLIT ) == 0 )
		return;
	if( ht == vSplitterBox )
		SplitRow( m_pRowInfo[0].nCurSize / 2 );
	else if( ht == hSplitterBox)
		SplitColumn( m_pColInfo[0].nCurSize / 2 );
	else if( ht >= vSplitterBar1 && ht <= vSplitterBar15 )
	{
		int rowDelete = ht - vSplitterBar1;
		int row;
		if( GetActivePane(&row, NULL) != NULL && rowDelete == row )
			++rowDelete;
		DeleteRow(rowDelete);
	}
	else if( ht >= hSplitterBar1 && ht <= hSplitterBar15 )
	{
		int colDelete = ht - hSplitterBar1;
		int col;
		if( GetActivePane( NULL, &col ) != NULL && colDelete == col )
			++colDelete;
		DeleteColumn( colDelete );
	}
	else if( ht >= splitterIntersection1 && ht <= splitterIntersection225 )
	{
		int rowDelete = (ht - splitterIntersection1) / 15;
		int colDelete = (ht - splitterIntersection1) % 15;
		int row, col;
		if( GetActivePane(&row, &col) != NULL )
		{
			if( col == colDelete )
				++colDelete;
			if( row == rowDelete )
				++rowDelete;
		}
		DeleteRow( rowDelete );
		DeleteColumn( colDelete );
	}
}

void CExtSplitterBaseWnd::OnLButtonUp( UINT /*nFlags*/, CPoint /*pt*/ )
{
	StopTracking(TRUE);
}

void CExtSplitterBaseWnd::OnCancelMode()
{
	StopTracking(FALSE);
}

void CExtSplitterBaseWnd::OnKeyDown( UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/ )
{
CPoint pt;
	GetCursorPos(&pt);
int cz = GetKeyState(VK_CONTROL) < 0 ? 1 : 16;
int dx = 0;
int dy = 0;
	switch( nChar )
	{
	case VK_ESCAPE:
		StopTracking( FALSE );
	return;
	case VK_RETURN:
		StopTracking( TRUE );
	return;
	case VK_LEFT:
		dx = -1;
	break;
	case VK_RIGHT:
		dx = +1;
	break;
	case VK_UP:
		dy = -1;
	break;
	case VK_DOWN:
		dy = +1;
	break;
	default:
		Default();
	return;
	}
	if(		m_htTrack == vSplitterBox
		||	m_htTrack >= vSplitterBar1 && m_htTrack <= vSplitterBar15
		)
		dx = 0;
	if(		m_htTrack == hSplitterBox
		||	m_htTrack >= hSplitterBar1 && m_htTrack <= hSplitterBar15
		)
		dy = 0;
	pt.x += dx * cz;
	pt.y += dy * cz;
	ScreenToClient( &pt );
	if( pt.y < m_rectLimit.top )
		pt.y = m_rectLimit.top;
	else if( pt.y > m_rectLimit.bottom )
		pt.y = m_rectLimit.bottom;
	if( pt.x < m_rectLimit.left )
		pt.x = m_rectLimit.left;
	else if( pt.x > m_rectLimit.right )
		pt.x = m_rectLimit.right;
	ClientToScreen( &pt );
	SetCursorPos( pt.x, pt.y );
}

void CExtSplitterBaseWnd::OnSysCommand( UINT nID, LPARAM lParam )
{
	if( ( nID & 0xFFF0 ) == SC_SIZE )
	{
		CWnd * pParent = GetSizingParent();
		if( pParent != NULL )
		{
			pParent->SendMessage( WM_SYSCOMMAND, (WPARAM)nID, lParam );
			return;
		}
	}
	CSplitterWnd::OnSysCommand( nID, lParam );
}

BOOL CExtSplitterBaseWnd::OnCommand( WPARAM wParam, LPARAM lParam )
{
	if( CSplitterWnd::OnCommand( wParam, lParam ) )
		return TRUE;
CFrameWnd * pFrameWnd = GetParentFrame();
	if( pFrameWnd != NULL )
	{
		ASSERT_VALID( pFrameWnd );
		return ( pFrameWnd->SendMessage( WM_COMMAND, wParam, lParam ) != 0L ) ? TRUE : FALSE;
	}
	return FALSE;
}

BOOL CExtSplitterBaseWnd::OnNotify( WPARAM wParam, LPARAM lParam, LRESULT * pResult )
{
	if( CSplitterWnd::OnNotify( wParam, lParam, pResult ) )
		return TRUE;
CFrameWnd * pFrameWnd = GetParentFrame();
	if( pFrameWnd != NULL )
	{
		ASSERT_VALID( pFrameWnd );
		*pResult = pFrameWnd->SendMessage( WM_NOTIFY, wParam, lParam );
		return TRUE;
	}
	return FALSE;
}

BOOL CExtSplitterBaseWnd::OnMouseWheel( UINT fFlags, short zDelta, CPoint point )
{
BOOL bRetVal = FALSE;
int row;
int col;
	for( row = 0; row < m_nRows; row++ )
	{
		for( col = 0; col < m_nCols; col++ )
		{
			CWnd * pPane = GetPane(row, col );
			CScrollView* pView = DYNAMIC_DOWNCAST( CScrollView, pPane);
			if( pView != NULL)
			{
				CScrollBar * pBar = pView->GetScrollBarCtrl( SB_VERT );
				if( pBar == NULL)
				{
					pBar = pView->GetScrollBarCtrl( SB_HORZ );
					if( pBar == NULL)
						continue;
				}
				int nOldPos = pBar->GetScrollPos();
				if( pView->DoMouseWheel( fFlags, zDelta, point ) )
					bRetVal = TRUE;
				if( col < m_nCols -1 )
					pBar->SetScrollPos( nOldPos, FALSE );
			}
		}
	}
	return TRUE;
}

void CExtSplitterBaseWnd::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar * pScrollBar )
{
	ASSERT(pScrollBar != NULL);
int col = _AfxGetDlgCtrlID( pScrollBar->m_hWnd ) - AFX_IDW_HSCROLL_FIRST;
	ASSERT( col >= 0 && col < m_nMaxCols );
	ASSERT( m_nRows > 0 );
int nOldPos = pScrollBar->GetScrollPos();
#ifdef _DEBUG
int nNewPos;
#endif
	for( int row = 0; row < m_nRows; row++ )
	{
		GetPane(row, col)->SendMessage( WM_HSCROLL, MAKELONG(nSBCode, nPos), (LPARAM)pScrollBar->m_hWnd );
#ifdef _DEBUG
		if( row == 0 )
		{
			nNewPos = pScrollBar->GetScrollPos();
			if( pScrollBar->GetScrollPos() != nNewPos )
			{
				TRACE0("Warning: scroll panes setting different scroll positions.\n");
			}
		}
#endif //_DEBUG
		if( row < m_nRows - 1 )
			pScrollBar->SetScrollPos( nOldPos, FALSE );
	}
}

void CExtSplitterBaseWnd::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar * pScrollBar )
{
	ASSERT( pScrollBar != NULL );
	int row = _AfxGetDlgCtrlID( pScrollBar->m_hWnd ) - AFX_IDW_VSCROLL_FIRST;
	ASSERT( row >= 0 && row < m_nMaxRows );
	ASSERT(m_nCols > 0);
int nOldPos = pScrollBar->GetScrollPos();
#ifdef _DEBUG
	int nNewPos;
#endif
	for( int col = 0; col < m_nCols; col++ )
	{
		GetPane( row, col )->SendMessage( WM_VSCROLL, MAKELONG(nSBCode, nPos), (LPARAM)pScrollBar->m_hWnd );
#ifdef _DEBUG
		if( col == 0 )
		{
			nNewPos = pScrollBar->GetScrollPos();
			if( pScrollBar->GetScrollPos() != nNewPos )
			{
				TRACE0("Warning: scroll panes setting different scroll positions.\n");
			}
		}
#endif //_DEBUG
		if( col < m_nCols - 1 )
			pScrollBar->SetScrollPos( nOldPos, FALSE );
	}
}

BOOL CExtSplitterBaseWnd::DoScroll( CView * pViewFrom, UINT nScrollCode, BOOL bDoScroll )
{
	ASSERT_VALID( pViewFrom );
int rowFrom, colFrom;
	if( ! IsChildPane( pViewFrom, &rowFrom, &colFrom ) )
		return FALSE;
BOOL bResult = FALSE;
int nOldVert = 0;
CScrollBar * pScrollVert = pViewFrom->GetScrollBarCtrl( SB_VERT );
	if( pScrollVert != NULL)
		nOldVert = pScrollVert->GetScrollPos();
int nOldHorz = 0;
CScrollBar * pScrollHorz = pViewFrom->GetScrollBarCtrl( SB_HORZ );
	if( pScrollHorz != NULL )
		nOldHorz = pScrollHorz->GetScrollPos();
	if( pViewFrom->OnScroll(nScrollCode, 0, bDoScroll ) )
		bResult = TRUE;
	if( pScrollVert != NULL )
	{
#ifdef _DEBUG
		int nNewVert = pScrollVert->GetScrollPos();
#endif
		for( int col = 0; col < m_nCols; col++ )
		{
			if( col == colFrom )
				continue;
			pScrollVert->SetScrollPos( nOldVert, FALSE );
			CView * pView = (CView*)GetPane( rowFrom, col );
			ASSERT_KINDOF( CView, pView );
			ASSERT( pView != pViewFrom );
			if( pView->OnScroll( MAKEWORD( -1, HIBYTE( nScrollCode ) ), 0, bDoScroll ) )
			{
				bResult = TRUE;
			}
#ifdef _DEBUG
			if( pScrollVert->GetScrollPos() != nNewVert )
			{
				TRACE0("Warning: scroll panes setting different scroll positions.\n");
			}
#endif
		}
	}
	if( pScrollHorz != NULL )
	{
#ifdef _DEBUG
		int nNewHorz = pScrollHorz->GetScrollPos();
#endif
		for( int row = 0; row < m_nRows; row++ )
		{
			if( row == rowFrom)
				continue;
			pScrollHorz->SetScrollPos( nOldHorz, FALSE );
			CView * pView = (CView*)GetPane( row, colFrom );
			ASSERT_KINDOF( CView, pView );
			ASSERT( pView != pViewFrom );
			if( pView->OnScroll( MAKEWORD( LOBYTE( nScrollCode ), -1 ), 0, bDoScroll ) )
				bResult = TRUE;
#ifdef _DEBUG
			if( pScrollHorz->GetScrollPos() != nNewHorz )
			{
				TRACE0("Warning: scroll panes setting different scroll positions.\n");
			}
#endif
		}
	}
	return bResult;
}

BOOL CExtSplitterBaseWnd::DoScrollBy( CView * pViewFrom, CSize sizeScroll, BOOL bDoScroll )
{
	int rowFrom, colFrom;
	if( ! IsChildPane( pViewFrom, &rowFrom, &colFrom ) )
		return FALSE;
BOOL bResult = FALSE;
int nOldVert = 0;
CScrollBar * pScrollVert = pViewFrom->GetScrollBarCtrl( SB_VERT );
	if( pScrollVert != NULL )
		nOldVert = pScrollVert->GetScrollPos();
int nOldHorz = 0;
CScrollBar * pScrollHorz = pViewFrom->GetScrollBarCtrl( SB_HORZ );
	if( pScrollHorz != NULL)
		nOldHorz = pScrollHorz->GetScrollPos();
	if( pViewFrom->OnScrollBy( sizeScroll, bDoScroll ) )
		bResult = TRUE;
	if( pScrollVert != NULL )
	{
#ifdef _DEBUG
		int nNewVert = pScrollVert->GetScrollPos();
#endif
		for( int col = 0; col < m_nCols; col++ )
		{
			if( col == colFrom )
				continue;
			pScrollVert->SetScrollPos( nOldVert, FALSE );
			CView * pView = (CView*)GetPane( rowFrom, col );
			ASSERT_KINDOF( CView, pView );
			ASSERT( pView != pViewFrom );
			if( pView->OnScrollBy( CSize( 0, sizeScroll.cy ), bDoScroll ) )
				bResult = TRUE;
#ifdef _DEBUG
			if( pScrollVert->GetScrollPos() != nNewVert )
			{
				TRACE0("Warning: scroll panes setting different scroll positions.\n");
			}
#endif
		}
	}
	if( pScrollHorz != NULL )
	{
#ifdef _DEBUG
	int nNewHorz = pScrollHorz->GetScrollPos();
#endif
		for( int row = 0; row < m_nRows; row++ )
		{
			if( row == rowFrom)
				continue;
			pScrollHorz->SetScrollPos( nOldHorz, FALSE );
			CView * pView = (CView*)GetPane( row, colFrom );
			ASSERT_KINDOF( CView, pView );
			ASSERT( pView != pViewFrom );
			if( pView->OnScrollBy( CSize( sizeScroll.cx, 0 ), bDoScroll ) )
				bResult = TRUE;
#ifdef _DEBUG
			if( pScrollHorz->GetScrollPos() != nNewHorz )
			{
				TRACE0("Warning: scroll panes setting different scroll positions.\n");
			}
#endif
		}
	}
	return bResult;
}

BOOL CExtSplitterBaseWnd::CanActivateNext( BOOL )
{
	ASSERT_VALID( this );
	if( GetActivePane() == NULL )
	{
		TRACE0("Warning: Can't go to next pane - there is no current pane.\n");
		return FALSE;
	}
	ASSERT(m_nRows != 0);
	ASSERT(m_nCols != 0);
	return (m_nRows > 1) || (m_nCols > 1);
}

void CExtSplitterBaseWnd::ActivateNext( BOOL bPrev )
{
	ASSERT_VALID( this );
int row, col;
	if( GetActivePane( &row, &col ) == NULL )
	{
		TRACE0("Warning: Cannot go to next pane - there is no current view.\n");
		return;
	}
	ASSERT(row >= 0 && row < m_nRows);
	ASSERT(col >= 0 && col < m_nCols);
	if( bPrev )
	{
		if( --col < 0 )
		{
			col = m_nCols - 1;
			if( --row < 0 )
				row = m_nRows - 1;
		}
	}
	else
	{
		if( ++col >= m_nCols )
		{
			col = 0;
			if( ++row >= m_nRows )
				row = 0;
		}
	}
	SetActivePane( row, col );
}

void CExtSplitterBaseWnd::SetActivePane( int row, int col, CWnd * pWnd )
{
CWnd* pPane = pWnd == NULL ? GetPane(row, col) : pWnd;
	if( pPane->IsKindOf( RUNTIME_CLASS( CView ) ) )
	{
		CFrameWnd * pFrameWnd = GetParentFrame();
		if( pFrameWnd != NULL )
		{
			ASSERT_VALID( pFrameWnd );
			pFrameWnd->SetActiveView( (CView*) pPane );
		}
	}
	else
	{
		TRACE0("Warning: Next pane is not a view - calling SetFocus.\n");
		pPane->SetFocus();
	}
}

CWnd * CExtSplitterBaseWnd::GetActivePane( int * pRow, int * pCol )
{
	ASSERT_VALID( this );
CWnd * pView = NULL;
CFrameWnd * pFrameWnd = GetParentFrame();
	if( pFrameWnd != NULL )
	{
		ASSERT_VALID( pFrameWnd );
		pView = pFrameWnd->GetActiveView();
	}
	if( pView == NULL )
		pView = GetFocus();
	if( pView != NULL && (! IsChildPane( pView, pRow, pCol ) ) )
		pView = NULL;
	return pView;
}

/////////////////////////////////////////////////////////////////////////////
// CExtSplitterWnd

IMPLEMENT_DYNCREATE( CExtSplitterWnd, CExtSplitterBaseWnd );

CExtSplitterWnd::CExtSplitterWnd()
{
}

CExtSplitterWnd::~CExtSplitterWnd()
{
}

BEGIN_MESSAGE_MAP( CExtSplitterWnd, CExtSplitterBaseWnd )
	//{{AFX_MSG_MAP(CExtSplitterWnd)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExtSplitterWnd::DrawAllSplitBars(
	CDC * pDC,
	int cxInside,
	int cyInside
	)
{
	if( pDC == NULL )
	{
		RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_NOCHILDREN );
		return;
	}
	ASSERT_VALID( pDC );
	if( (GetStyle()&WS_CLIPCHILDREN) == 0 )
		ModifyStyle( 0, WS_CLIPCHILDREN );
	if( (GetStyle()&WS_CLIPSIBLINGS) == 0 )
		ModifyStyle( 0, WS_CLIPSIBLINGS );
CRect rcClient;
	GetClientRect( &rcClient );
CWnd * pMainFrame = NULL, * pInnerFrame = GetParentFrame();
	if( pInnerFrame != NULL )
	{
		ASSERT_VALID( pInnerFrame );
		pMainFrame = pInnerFrame->GetParentFrame();
		if( pMainFrame == NULL )
			pMainFrame = pInnerFrame;
	}
	if( pMainFrame == NULL )
	{
		pMainFrame = GetParent();
		for( ; pMainFrame != NULL; pMainFrame = pMainFrame->GetParent() )
		{
			if( ( pMainFrame->GetStyle() & WS_CHILD ) == 0 )
				break;
		}
	}
#if (!defined __EXT_MFC_NO_TAB_ONENOTE_CTRL)
bool bDrawDefault = true;
CExtTabMdiOneNoteWnd * pOneNoteTabs = NULL;
CWnd * pWnd = ( pMainFrame != NULL ) ? pMainFrame->GetWindow( GW_CHILD ) : NULL;
	for( ; pWnd != NULL; pWnd = pWnd->GetWindow( GW_HWNDNEXT ) )
	{
		pOneNoteTabs =
			DYNAMIC_DOWNCAST(
				CExtTabMdiOneNoteWnd,
				pWnd
				);
		if( pOneNoteTabs != NULL )
			break;
	} // for( ; pWnd != NULL; pWnd = pWnd->GetWindow( GW_HWNDNEXT ) )
	if( pOneNoteTabs != NULL )
	{
		bDrawDefault = false;
		LONG nIndex = ( pInnerFrame != NULL ) ? pOneNoteTabs->ItemFindByHWND( pInnerFrame->m_hWnd ) : -1L;
		// ASSERT( nIndex >= 0 );
		if( nIndex >= 0 )
		{
			CExtTabOneNoteWnd::TAB_ITEM_INFO_ONENOTE * pTII =
				pOneNoteTabs->ItemGet( nIndex );
			COLORREF clrFill = pTII->GetColorBkDark();
			pDC->FillSolidRect( rcClient, clrFill );
		}
	} // if( pOneNoteTabs != NULL )
	if( bDrawDefault )
#endif
	if( pMainFrame != NULL )
	{ // block
		CExtPaintManager * pPM = PmBridge_GetPM();
		ASSERT_VALID( pPM );
		CRect rcWnd;
		pMainFrame->GetWindowRect( &rcWnd );
		ScreenToClient( &rcWnd );
		if( pPM->m_clrForceSplitterBk != COLORREF(-1L) )
		{
			pDC->FillSolidRect( rcClient, pPM->m_clrForceSplitterBk );
			return;
		}
		else if(	(! pPM->GetCb2DbTransparentMode(this) )
				||	(! pPM->PaintDockerBkgnd(
						true,
						*pDC,
						rcClient,
						rcWnd
						)
					)
			)
		{
			COLORREF clrFill =
				pPM->GetColor( CExtPaintManager::CLR_3DFACE_OUT );
			pDC->FillSolidRect( rcClient, clrFill );
		}
// 		else
// 			return;
	} // block
	CExtSplitterBaseWnd::DrawAllSplitBars(
		pDC,
		cxInside,
		cyInside
		);
}

void CExtSplitterWnd::OnDrawSplitter(
	CDC* pDC,
	ESplitType nType,
	const CRect & rectArg
	)
{
	if( pDC == NULL )
	{
		RedrawWindow( rectArg, NULL, RDW_INVALIDATE|RDW_NOCHILDREN );
		return;
	}
	ASSERT_VALID( pDC );
CRect rect = rectArg;
	switch( nType )
	{
	case splitBorder:
		{
			CExtPaintManager * pPM = PmBridge_GetPM();
			ASSERT_VALID( pPM );
			if( pPM->m_clrForceSplitterBk != COLORREF(-1L) )
				pDC->FillSolidRect( rect, pPM->m_clrForceSplitterBk );

			rect.InflateRect( -__EXT_MFC_CX_BORDER, -__EXT_MFC_CY_BORDER );
			PmBridge_GetPM()->PaintResizableBarChildNcAreaRect(
				*pDC,
				rect,
				this
				);
		}
		return;
	case splitIntersection:
	case splitBox:
	case splitBar:
		{
			CExtPaintManager * pPM = PmBridge_GetPM();
			ASSERT_VALID( pPM );
			if( pPM->m_clrForceSplitterBk != COLORREF(-1L) )
				pDC->FillSolidRect( rect, pPM->m_clrForceSplitterBk );
		}
		return;
	default:
			ASSERT(FALSE);  // unknown splitter type
		break;
	} // switch( nType )
}

void CExtSplitterWnd::OnInvertTracker(
	const CRect & rect
	)
{
	ASSERT_VALID( this );
	ASSERT( ! rect.IsRectEmpty() );
CRect rc = rect;
CWnd * pWndParent = GetParent();
	ClientToScreen( &rc );
	pWndParent->ScreenToClient( &rc );
//CDC * pDC = pWndParent->GetDC();
CDC * pDC = pWndParent->GetDCEx( NULL, DCX_CACHE | DCX_LOCKWINDOWUPDATE | DCX_CLIPSIBLINGS );
CBrush * pBrush = CDC::GetHalftoneBrush();
HBRUSH hOldBrush = NULL;
	if( pBrush != NULL )
		hOldBrush = (HBRUSH)SelectObject( pDC->m_hDC, pBrush->m_hObject );
	pDC->PatBlt( rc.left, rc.top, rc.Width(), rc.Height(), PATINVERT );
	if( hOldBrush != NULL )
		SelectObject( pDC->m_hDC, hOldBrush );
	ReleaseDC( pDC );
}

BOOL CExtSplitterWnd::CreateScrollBarCtrl( DWORD dwStyle, UINT nID )
{
	ASSERT_VALID( this );
	ASSERT( m_hWnd != NULL );
	if( (dwStyle&SBS_SIZEBOX) != 0 )
		return CExtSplitterBaseWnd::CreateScrollBarCtrl( dwStyle, nID );
CExtScrollBar * pExtScrollBar = new CExtScrollBar;
	pExtScrollBar->m_bAutoDeleteOnPostNcDestroy = true;
	pExtScrollBar->m_bReflectParentSizing = false;
pExtScrollBar->m_bHelperLightAccent = false;
	if( ! pExtScrollBar->Create(
			dwStyle|WS_VISIBLE|WS_CHILD,
			CRect(0,0,1,1),
			this,
			nID
			)
		)
		return FALSE;
HWND hWnd = pExtScrollBar->GetSafeHwnd();
//HWND hWnd =
//		::CreateWindow(
//			_T("SCROLLBAR"), NULL,
//			dwStyle|WS_VISIBLE|WS_CHILD,
//			0,
//			0,
//			1,
//			1,
//			m_hWnd, 
//			(HMENU)nID,
//			AfxGetInstanceHandle(),
//			NULL
//			);
#ifdef _DEBUG
	if( hWnd == NULL )
		TRACE1(
			"Warning: Window creation failed: GetLastError returns 0x%8.8X\n",
			GetLastError()
			);
#endif
	return ( hWnd != NULL ) ? TRUE : FALSE;
}

void CExtSplitterWnd::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	ASSERT( pScrollBar != NULL );
	if( ! pScrollBar->IsKindOf( RUNTIME_CLASS(CExtScrollBar) ) )
	{
		CExtSplitterBaseWnd::OnHScroll( nSBCode, nPos, pScrollBar );
		return;
	}
int col = _AfxGetDlgCtrlID( pScrollBar->m_hWnd ) - AFX_IDW_HSCROLL_FIRST;
	ASSERT( col >= 0 && col < m_nMaxCols );
	ASSERT( m_nRows > 0 );
int nOldPos = pScrollBar->GetScrollPos();
#ifdef _DEBUG
int nNewPos;
#endif
	for( int row = 0; row < m_nRows; row++ )
	{
		GetPane( row, col ) ->
			SendMessage(
				WM_HSCROLL,
				MAKELONG( nSBCode, nPos ),
				(LPARAM)pScrollBar->m_hWnd
				);
#ifdef _DEBUG
		if( row == 0 )
		{
			nNewPos = pScrollBar->GetScrollPos();
			if( pScrollBar->GetScrollPos() != nNewPos )
			{
				TRACE0( "Warning: scroll panes setting different scroll positions.\n" );
			} // if( pScrollBar->GetScrollPos() != nNewPos )
		} // if( row == 0 )
#endif //_DEBUG
		if( row < ( m_nRows - 1 ) )
			pScrollBar->SetScrollPos( nOldPos, FALSE );
			//((CExtScrollBar*)pScrollBar)->_SetScrollPos( nOldPos, false, false );
	} // for( int row = 0; row < m_nRows; row++ )
}

void CExtSplitterWnd::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	ASSERT( pScrollBar != NULL );
	if( ! pScrollBar->IsKindOf( RUNTIME_CLASS(CExtScrollBar) ) )
	{
		CExtSplitterBaseWnd::OnHScroll( nSBCode, nPos, pScrollBar );
		return;
	}
int row = _AfxGetDlgCtrlID( pScrollBar->m_hWnd ) - AFX_IDW_VSCROLL_FIRST;
	ASSERT( row >= 0 && row < m_nMaxRows );
	ASSERT( m_nCols > 0 );
int nOldPos = pScrollBar->GetScrollPos();
#ifdef _DEBUG
int nNewPos;
#endif
	for( int col = 0; col < m_nCols; col++ )
	{
		GetPane( row, col ) ->
			SendMessage(
				WM_VSCROLL,
				MAKELONG( nSBCode, nPos ),
				(LPARAM)pScrollBar->m_hWnd
				);
#ifdef _DEBUG
		if( col == 0 )
		{
			nNewPos = pScrollBar->GetScrollPos();
			if( pScrollBar->GetScrollPos() != nNewPos )
			{
				TRACE0("Warning: scroll panes setting different scroll positions.\n");
			} // if( pScrollBar->GetScrollPos() != nNewPos )
		} // if( col == 0 )
#endif //_DEBUG
		if( col < ( m_nCols - 1 ) )
			pScrollBar->SetScrollPos( nOldPos, FALSE );
			//((CExtScrollBar*)pScrollBar)->_SetScrollPos( nOldPos, false, false );
	} // for( int col = 0; col < m_nCols; col++ )
}

#endif // (!defined __EXT_MFC_NO_SPLITTER_WND)


