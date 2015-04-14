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

#if (!defined __EXT_CONTROLS_COMMON_H)
	#include <ExtControlsCommon.h>
#endif

#if (!defined __EXT_RICH_CONTENT_H)
	#include <ExtRichContent.h>
#endif // (!defined __EXT_RICH_CONTENT_H)

#if (!defined __EXT_LOCALIZATION_H)
	#include <../Src/ExtLocalization.h>
#endif

#include <Resources/Resource.h>

#if (!defined __PROF_UIS_RES_CC_SC_H)
	#include <Resources/ResCommonControls/ResCommonControls.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if ( ! defined __EXT_MFC_NO_LIST_VIEW_CTRL )

/////////////////////////////////////////////////////////////////////////////
// CExtListCtrlDataSortOrder

IMPLEMENT_SERIAL( CExtListCtrlDataSortOrder, CObject, VERSIONABLE_SCHEMA|1 );

CExtListCtrlDataSortOrder::CExtListCtrlDataSortOrder()
{
}

CExtListCtrlDataSortOrder::CExtListCtrlDataSortOrder(
	const CExtListCtrlDataSortOrder & other
	)
{
	_AssignFromOther( other );
}

CExtListCtrlDataSortOrder::~CExtListCtrlDataSortOrder()
{
	Empty();
}

void CExtListCtrlDataSortOrder::_AssignFromOther(
	const CExtListCtrlDataSortOrder & other
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&other) );
	Empty();
LONG nCount = (LONG)other.m_arrItems.GetSize();
	for( LONG i = 0L; i < nCount; i++ )
	{
		COLUMN_INFO _itemDst( other.m_arrItems[ i ] );
		m_arrItems.Add( _itemDst );
	}
	ASSERT( nCount == m_arrItems.GetSize() );
}

CExtListCtrlDataSortOrder & CExtListCtrlDataSortOrder::operator =(
	const CExtListCtrlDataSortOrder & other
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&other) );
	_AssignFromOther( other );
	return (*this);
}

bool CExtListCtrlDataSortOrder::operator == (
	const CExtListCtrlDataSortOrder & other
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&other) );
LONG nCount = (LONG)m_arrItems.GetSize();
LONG i = (LONG)other.m_arrItems.GetSize();
	if( nCount != i )
		return false;
	for( i = 0L; i < nCount; i++ )
	{
		const COLUMN_INFO & _itemSrc = m_arrItems[ i ];
		const COLUMN_INFO & _itemDst = other.m_arrItems[ i ];
		if( _itemSrc != _itemDst )
			return false;
	}
	return true;
}

bool CExtListCtrlDataSortOrder::operator != (
	const CExtListCtrlDataSortOrder & other
	) const
{
	ASSERT_VALID( this );
	ASSERT_VALID( (&other) );
	if( (*this) == other )
		return false;
	return true;
}

#ifdef _DEBUG
void CExtListCtrlDataSortOrder::AssertValid() const
{
	CObject::AssertValid();
}
void CExtListCtrlDataSortOrder::Dump( CDumpContext & dc ) const
{
	CObject::Dump( dc );
}
#endif // _DEBUG

void CExtListCtrlDataSortOrder::Serialize( CArchive & ar )
{
	CObject::Serialize( ar );
DWORD i, dwCount;
	if( ar.IsStoring() )
	{
		dwCount = (DWORD)m_arrItems.GetSize();
		ar << dwCount;
		for( i = 0; i < dwCount; i++ )
		{
			COLUMN_INFO & _itemSrc = m_arrItems[ i ];
			_itemSrc.Serialize( ar );
		}
	} // if( ar.IsStoring() )
	else
	{
		m_arrItems.RemoveAll();
		ar >> dwCount;
		for( i = 0; i < dwCount; i++ )
		{
			COLUMN_INFO _itemSrc;
			_itemSrc.Serialize( ar );
			m_arrItems.Add( _itemSrc );
		}
		ASSERT( DWORD( m_arrItems.GetSize() ) == dwCount );
	} // else from if( ar.IsStoring() )
}

bool CExtListCtrlDataSortOrder::ItemsUnique() const
{
	ASSERT_VALID( this );
LONG nCount = (LONG)m_arrItems.GetSize();
	for( LONG i = 0L; i < (nCount - 1L); i++ )
	{
		const COLUMN_INFO & _itemSrc = m_arrItems[ i ];
		for( LONG j = (i + 1L); j < nCount; j++ )
		{
			const COLUMN_INFO & _itemDst = m_arrItems[ j ];
			if( _itemSrc == _itemDst )
				return false;
		}
	}
	return true;
}

void CExtListCtrlDataSortOrder::MakeItemsUnique()
{
	ASSERT_VALID( this );
LONG nCount = (LONG)m_arrItems.GetSize();
	for( LONG i = 0L; i < (nCount - 1L); i++ )
	{
		const COLUMN_INFO & _itemSrc = m_arrItems[ i ];
		for( LONG j = (i + 1L); j < nCount; )
		{
			const COLUMN_INFO & _itemDst = m_arrItems[ j ];
			if( _itemSrc != _itemDst )
			{
				j++;
				continue;
			}
			nCount--;
			m_arrItems.RemoveAt( j, 1 );
		}
	}
}

bool CExtListCtrlDataSortOrder::IsEmpty() const
{
	ASSERT_VALID( this );
	if( m_arrItems.GetSize() > 0L )
		return false;
	else
		return true;
}

void CExtListCtrlDataSortOrder::Empty()
{
	ASSERT_VALID( this );
	m_arrItems.RemoveAll();
}

LONG CExtListCtrlDataSortOrder::GetColPos( LONG nColNo ) const
{
	ASSERT_VALID( this );
LONG nCount = (LONG)m_arrItems.GetSize();
	for( LONG i = 0L; i < nCount; i++ )
	{
		const COLUMN_INFO & _itemSrc = m_arrItems[ i ];
		if( _itemSrc.m_nColNo == nColNo )
			return i;
	}
	return -1L;
}

void CExtListCtrlDataSortOrder::SetupOrder(
	const CExtListCtrlDataSortOrder & _gdsoUpdate,
	bool bInvertIntersectionSortOrder
	)
{
	ASSERT_VALID( this );
	ASSERT( ItemsUnique() );
	ASSERT_VALID( (&_gdsoUpdate) );
	ASSERT( _gdsoUpdate.ItemsUnique() );
LONG nCount = (LONG)m_arrItems.GetSize();
	if( nCount == 0 )
	{
		(*this) = _gdsoUpdate;
		return;
	} // if( nCount == 0 )
LONG nCountUpdate = (LONG)_gdsoUpdate.m_arrItems.GetSize();
	for( LONG u = 0L; u < nCountUpdate; u++ )
	{
		const COLUMN_INFO & _itemUpdate = _gdsoUpdate.m_arrItems[ u ];
		COLUMN_INFO _itemSetup( _itemUpdate );
		LONG i = GetColPos( _itemUpdate.m_nColNo );
		if( i < 0L )
		{
			m_arrItems.Add( _itemSetup );
			continue;
		}
		COLUMN_INFO & _itemDst = m_arrItems[ i ];
		ASSERT( _itemDst.m_nColNo == _itemUpdate.m_nColNo );
		if(		_itemDst == _itemUpdate
			&&	(! bInvertIntersectionSortOrder )
			)
			continue;
		if( bInvertIntersectionSortOrder )
			_itemSetup.m_bAscending = (! _itemDst.m_bAscending);
		m_arrItems.SetAt( i, _itemSetup );
	}
	ASSERT_VALID( this );
	ASSERT( ItemsUnique() );
}

void CExtListCtrlDataSortOrder::UpdateIndices(
	LONG nColNo,
	LONG nInsertRemoveCount,
	bool bInsert
	)
{
	ASSERT_VALID( this );
	ASSERT( nColNo >= 0L );
	ASSERT( nInsertRemoveCount >= 0L );
	if( nInsertRemoveCount == 0L )
		return;
LONG nCount = (LONG)m_arrItems.GetSize();
	for( LONG i = 0L; i < nCount; )
	{
		COLUMN_INFO _itemUpdate( m_arrItems[ i ] );
		if( bInsert )
		{
			if( _itemUpdate.m_nColNo >= nColNo )
			{
				_itemUpdate.m_nColNo += nInsertRemoveCount;
				m_arrItems.SetAt( i, _itemUpdate );
			}
			i++;
		}
		else
		{
			if( _itemUpdate.m_nColNo >= (nColNo+nInsertRemoveCount) )
			{
				_itemUpdate.m_nColNo -= nInsertRemoveCount;
				m_arrItems.SetAt( i, _itemUpdate );
				i++;
			}
			else if( _itemUpdate.m_nColNo >= nColNo )
			{
				m_arrItems.RemoveAt( i, 1 );
				nCount--;
			}
			else
				i++;
		}
	}
}

void CExtListCtrlDataSortOrder::SwapDroppedSeries(
	LONG nColNoSrc,
	LONG nColNoDropBefore
	)
{
	ASSERT_VALID( this );
	ASSERT( nColNoSrc >= 0L );
	ASSERT( nColNoDropBefore >= 0L );
	if(		nColNoSrc == nColNoDropBefore
		||	(nColNoSrc+1L) == nColNoDropBefore
		)
		return;
LONG nCount = (LONG)m_arrItems.GetSize();
	for( LONG i = 0L; i < nCount; i++ )
	{
		COLUMN_INFO _itemUpdate( m_arrItems[ i ] );
		if( nColNoSrc < nColNoDropBefore )
		{
			if(		_itemUpdate.m_nColNo > nColNoSrc
				&&	_itemUpdate.m_nColNo < nColNoDropBefore
				)
			{
				_itemUpdate.m_nColNo --;
				m_arrItems.SetAt( i, _itemUpdate );
			}
			else if( _itemUpdate.m_nColNo == nColNoSrc )
			{
				_itemUpdate.m_nColNo = nColNoDropBefore - 1L;
				m_arrItems.SetAt( i, _itemUpdate );
			}
		} // if( nColNoSrc < nColNoDropBefore )
		else
		{
			ASSERT( nColNoSrc > nColNoDropBefore );
			if(		_itemUpdate.m_nColNo < nColNoSrc
				&&	_itemUpdate.m_nColNo >= nColNoDropBefore
				)
			{
				_itemUpdate.m_nColNo ++;
				m_arrItems.SetAt( i, _itemUpdate );
			}
			else if( _itemUpdate.m_nColNo == nColNoSrc )
			{
				_itemUpdate.m_nColNo = nColNoDropBefore;
				m_arrItems.SetAt( i, _itemUpdate );
			}
		} // else from if( nColNoSrc < nColNoDropBefore )
	} // for( LONG i = 0L; i < nCount; i++ )
}

CExtListCtrlDataSortOrder::COLUMN_INFO::COLUMN_INFO(
	LONG nColNo, // = -1L
	bool bAscending // = true
	)
	: m_nColNo( nColNo )
	, m_bAscending( bAscending )
{
}

CExtListCtrlDataSortOrder::COLUMN_INFO::COLUMN_INFO( const CExtListCtrlDataSortOrder::COLUMN_INFO & other )
	: m_nColNo( other.m_nColNo )
	, m_bAscending( other.m_bAscending )
{
}

CExtListCtrlDataSortOrder::COLUMN_INFO & CExtListCtrlDataSortOrder::COLUMN_INFO::operator = ( const CExtListCtrlDataSortOrder::COLUMN_INFO & other )
{
	m_nColNo = other.m_nColNo;
	m_bAscending = other.m_bAscending;
	return (*this);
}

void CExtListCtrlDataSortOrder::COLUMN_INFO::Serialize( CArchive & ar )
{
BYTE nFlags = 0;
	if( ar.IsStoring() )
	{
		if( m_bAscending )
			nFlags |= 0x01;
		ar.Write( &m_nColNo, sizeof(m_nColNo) );
		ar.Write( &nFlags, sizeof(nFlags) );
	} // if( ar.IsStoring() )
	else
	{
		ar.Read( &m_nColNo, sizeof(m_nColNo) );
		ar.Read( &nFlags, sizeof(nFlags) );
		if( (nFlags&0x01) != 0 )
			m_bAscending = true;
		else
			m_bAscending = false;
	} // else from if( ar.IsStoring() )
}

int CExtListCtrlDataSortOrder::COLUMN_INFO::Cmp( const CExtListCtrlDataSortOrder::COLUMN_INFO & other ) const
{
	if( m_nColNo < other.m_nColNo )
		return -1;
	if( m_nColNo > other.m_nColNo )
		return 1;
	if( m_bAscending )
	{
		if( ! other.m_bAscending )
			return 1;
	} // if( m_bAscending )
	else
	{
		if( other.m_bAscending )
			return -1;
	} // else from if( m_bAscending )
	return 0;
}

bool CExtListCtrlDataSortOrder::COLUMN_INFO::operator == ( const CExtListCtrlDataSortOrder::COLUMN_INFO & other ) const
{
	return ( (Cmp(other) == 0) ? true : false );
}

bool CExtListCtrlDataSortOrder::COLUMN_INFO::operator != ( const CExtListCtrlDataSortOrder::COLUMN_INFO & other ) const
{
	return ( (Cmp(other) != 0) ? true : false );
}

bool CExtListCtrlDataSortOrder::COLUMN_INFO::operator <= ( const CExtListCtrlDataSortOrder::COLUMN_INFO & other ) const
{
	return ( (Cmp(other) <= 0) ? true : false );
}

bool CExtListCtrlDataSortOrder::COLUMN_INFO::operator >= ( const CExtListCtrlDataSortOrder::COLUMN_INFO & other ) const
{
	return ( (Cmp(other) >= 0) ? true : false );
}

bool CExtListCtrlDataSortOrder::COLUMN_INFO::operator < ( const CExtListCtrlDataSortOrder::COLUMN_INFO & other ) const
{
	return ( (Cmp(other) < 0) ? true : false );
}

bool CExtListCtrlDataSortOrder::COLUMN_INFO::operator > ( const CExtListCtrlDataSortOrder::COLUMN_INFO & other ) const
{
	return ( (Cmp(other) > 0) ? true : false );
}

/////////////////////////////////////////////////////////////////////////////
// CExtHeaderCtrl

IMPLEMENT_DYNCREATE( CExtHeaderCtrl, CHeaderCtrl );

const UINT CExtHeaderCtrl::g_nMsgHeaderButtonClick =
	::RegisterWindowMessage( _T("CExtHeaderCtrl::g_nMsgHeaderButtonClick") );
bool CExtHeaderCtrl::g_bEnableOnIdleCalls = false;

CExtHeaderCtrl::CExtHeaderCtrl()
	: m_nHoverColNo( -1 )
	, m_nHoverIndex( -1 )
	, m_nPressingColNo( -1 )
	, m_nPressingIndex( -1 )
	, m_ptScreenPressing( -32767, -32767 )
	, m_nHelperInitialResizingExtent( -32767 )
	, m_bOnButton( false )
	, m_bOnDividerAtRight( false )
	, m_bCancelingActions( false )
	, m_nDividerHalfExtent( 2 )
	, m_nAdvancedTipStyle( INT(CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON) )
	, m_bShowTipsOverHeader( true )
{
	m_hCursorDefault = ::LoadCursor( NULL, IDC_ARROW );
	ASSERT( m_hCursorDefault != NULL );
CExtLocalResourceHelper _LRH;
CWinApp * pApp = ::AfxGetApp();
	ASSERT_VALID( pApp );
	m_hCursorResizingH1 =  pApp->LoadCursor( MAKEINTRESOURCE( IDC_EXT_RESIZE_H1 ) );
	ASSERT( m_hCursorResizingH1 != NULL );
	m_hCursorResizingH2 =  pApp->LoadCursor( MAKEINTRESOURCE( IDC_EXT_RESIZE_H2 ) );
	ASSERT( m_hCursorResizingH2 != NULL );
	m_hCursorOuterDragOK = ::LoadCursor( NULL, IDC_ARROW );
	ASSERT( m_hCursorOuterDragOK != NULL );
	m_hCursorOuterDragCancel = pApp->LoadCursor( MAKEINTRESOURCE( IDC_EXT_BIG_X_CROSS ) );
	ASSERT( m_hCursorOuterDragCancel != NULL );

	PmBridge_Install( true );
}

CExtHeaderCtrl::~CExtHeaderCtrl()
{
	PmBridge_Uninstall();

	if( m_hCursorDefault != NULL )
		::DestroyCursor( m_hCursorDefault );
	if( m_hCursorResizingH1 != NULL )
		::DestroyCursor( m_hCursorResizingH1 );
	if( m_hCursorResizingH2 != NULL )
		::DestroyCursor( m_hCursorResizingH2 );
	if( m_hCursorOuterDragOK != NULL )
		::DestroyCursor( m_hCursorOuterDragOK );
	if( m_hCursorOuterDragCancel != NULL )
		::DestroyCursor( m_hCursorOuterDragCancel );
}

void CExtHeaderCtrl::PmBridge_OnPaintManagerChanged(
	CExtPaintManager * pGlobalPM
	)
{
	ASSERT_VALID( this );
	CExtPmBridge::PmBridge_OnPaintManagerChanged( pGlobalPM );
	if( GetSafeHwnd() == NULL )
		return;
	Invalidate();
}

void CExtHeaderCtrl::_CancelActions()
{
	ASSERT_VALID( this );
	if( m_bCancelingActions )
		return;
	m_bCancelingActions = true;
	if( m_bOnDividerAtRight )
	{
		HWND hWndParent = ::GetParent( m_hWnd );
		if( hWndParent != NULL )
		{
			UINT nOwnID = UINT( GetDlgCtrlID() );
			HD_NOTIFY _data, _data2;
			::memset( &_data, 0, sizeof(HD_NOTIFY) );
			::memset( &_data2, 0, sizeof(HD_NOTIFY) );
			_data.hdr.hwndFrom = m_hWnd;
			_data.hdr.idFrom = nOwnID;
			_data.iButton = 0;
			_data.iItem = m_nPressingColNo; //nColNo;
			_data.hdr.code = HDN_ENDTRACK;
HDITEM hdItemBuffer; // to avoid common controls crashing
::memset( &hdItemBuffer, 0, sizeof(HDITEM) );
_data.pitem = &hdItemBuffer;
			::memcpy( &_data2, &_data, sizeof(HD_NOTIFY) );
#if (defined _UNICODE)
			::LockWindowUpdate( hWndParent );
#endif // (defined _UNICODE)
			::SendMessage( hWndParent, WM_NOTIFY, WPARAM(nOwnID), LPARAM(&_data) );
#if (defined _UNICODE)
			::LockWindowUpdate( NULL );
#endif // (defined _UNICODE)
			_data2.hdr.code = HDN_ITEMCHANGED;
			::SendMessage( hWndParent, WM_NOTIFY, WPARAM(nOwnID), LPARAM(&_data2) );
			if( ::IsWindow( hWndParent ) && ::IsWindowVisible( hWndParent ) )
				::InvalidateRect( hWndParent, NULL, TRUE );
		} // if( hWndParent != NULL )
	} // if( m_bOnDividerAtRight )
	if(		( m_nHoverColNo >= 0 || m_nPressingColNo >= 0 )
		&&	GetSafeHwnd() != NULL
		)
		Invalidate();
	m_ptScreenPressing.x = m_ptScreenPressing.y = m_nHelperInitialResizingExtent = -32767;
	m_nHoverColNo = m_nHoverIndex = m_nPressingColNo = m_nPressingIndex = -1;
	m_bOnButton = m_bOnDividerAtRight = false;
	if( ::GetCapture() == m_hWnd )
		ReleaseCapture();
	m_bCancelingActions = false;
}

BEGIN_MESSAGE_MAP( CExtHeaderCtrl, CHeaderCtrl )
	//{{AFX_MSG_MAP(CExtHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_CANCELMODE()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(
		CExtContentExpandWnd::g_nMsgPaintItemContent,
		_OnPaintExpandedItemContent
		)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtHeaderCtrl message handlers

LRESULT CExtHeaderCtrl::_OnPaintExpandedItemContent( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	lParam;
CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA * p_picd =
		(CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA *)wParam;
	ASSERT( p_picd != NULL );
	ASSERT( p_picd->m_dc.GetSafeHdc() != NULL );
	if( ! _PaintExpandedItemContent(*p_picd) )
		return 0;
	return (!0);
}

void CExtHeaderCtrl::SortRulesGet(
	CExtListCtrlDataSortOrder & _sortOrder
	) const
{
	ASSERT_VALID( this );
	_sortOrder = m_sortOrder;
}

void CExtHeaderCtrl::SortRulesSet(
	const CExtListCtrlDataSortOrder & _sortOrder
	)
{
	ASSERT_VALID( this );
	m_sortOrder = _sortOrder;
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

void CExtHeaderCtrl::OnPaint() 
{
	ASSERT_VALID( this );
CRect rcClient;
	GetClientRect( &rcClient );
CPaintDC dcPaint( this );
CExtMemoryDC dc( &dcPaint, &rcClient );
	OnEraseHeaderBackground( dc, rcClient );
INT nColNo, nColCount = INT(GetItemCount());
	for( nColNo = 0; nColNo < nColCount; nColNo++ )
	{
		CRect _rcItemEntire;
		if( ! GetItemRect( nColNo, &_rcItemEntire ) )
			continue;
		if( ! dc.RectVisible( &_rcItemEntire ) )
			continue;
		HD_ITEM _item;
		::memset( &_item, 0, sizeof(HD_ITEM) );
		_item.mask = UINT(-1) & (~(__EXT_MFC_HDI_FILTER));
		TCHAR strText[ 1024 ];
		::memset( strText, 0, sizeof(strText) );
		_item.cchTextMax = sizeof( strText ) / sizeof( strText[0] ) - 1;
		_item.pszText = strText;
		GetItem( nColNo, &_item );
		OnPaintHeaderItem( dc, _item, nColNo, _rcItemEntire );
	}
}

void CExtHeaderCtrl::OnEraseHeaderBackground(
	CDC & dc,
	const CRect & rcClient
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	PmBridge_GetPM()->Header_PaintBackground(
		dc,
		rcClient,
		(CObject*)this
		);
}

CSize CExtHeaderCtrl::OnCalcHeaderItemSize(
	INT nColNo
	) const
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return CSize( 0, 0 );
INT nColCount = INT( GetItemCount() );
	ASSERT( 0 <= nColNo && nColNo < nColCount );
CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );
const EXTENDED_ITEM_DATA & _eii = ExtendedItemDataGet( nColNo );
CSize _sizeEntireItem( 0, 0 );
	if( ! _eii.m_iconButton.IsEmpty() )
	{
		CSize sizeButton = _eii.m_iconButton.GetSize();
		INT nButtonPaddingBefore = 0, nButtonPaddingAfter = 0;
		pPM->Header_GetButtonPadding( nButtonPaddingBefore, nButtonPaddingAfter, nColNo, nColCount, (CObject*)this );
		sizeButton.cx += nButtonPaddingBefore + nButtonPaddingAfter;
		_sizeEntireItem.cx += sizeButton.cx;
		_sizeEntireItem.cy = max( _sizeEntireItem.cy, sizeButton.cy );
	}
	if( ! _eii.m_iconItem.IsEmpty() )
	{
		INT nBetweenIconAndText = pPM->Header_GetDistanceBetweenIconAndText( nColNo, nColCount, (CObject*)this );
		CSize _sizeIcon = _eii.m_iconItem.GetSize();
		_sizeIcon.cx += nBetweenIconAndText;
		_sizeEntireItem.cx += _sizeIcon.cx;
		_sizeEntireItem.cy = max( _sizeEntireItem.cy, _sizeIcon.cy );
	}
INT nSortRuleNo = m_sortOrder.GetColPos( nColNo );
	if( nSortRuleNo >= 0 )
	{
		INT nBetweenIconAndSortArrow =	pPM->Header_GetDistanceBetweenIconAndSortArrow( nColNo, nColCount, (CObject*)this );
		INT nSortArrowExtent = pPM->Header_GetSortArrowWidth( nColNo, nColCount, (CObject*)this );
		_sizeEntireItem.cx += nSortArrowExtent + nBetweenIconAndSortArrow;
	}
HD_ITEM _item;
	::memset( &_item, 0, sizeof(HD_ITEM) );
	_item.mask = UINT(HDI_TEXT);
TCHAR strText[ 1024 ];
	::memset( strText, 0, sizeof(strText) );
	_item.cchTextMax = sizeof( strText ) / sizeof( strText[0] );
	_item.pszText = strText;
	GetItem( nColNo, &_item );
CExtSafeString s;
	s = _item.pszText;
CFont * pFont = &pPM->m_FontNormal; //GetFont();
	if( pFont->GetSafeHandle() == NULL )
		pFont = &(pPM->m_FontNormal);
CClientDC dc( (CWnd*)this );
CSize _sizeText = CExtPaintManager::stat_CalcTextDimension( dc, *pFont, s ).Size();
	_sizeEntireItem.cx += _sizeText.cx;
	_sizeEntireItem.cy = max( _sizeEntireItem.cy, _sizeText.cy );

CRect rcItemPadding = pPM->Header_GetItemPadding( nColNo, nColCount, (CObject*)this );
	_sizeEntireItem.cx += rcItemPadding.left + rcItemPadding.right;
	_sizeEntireItem.cy += rcItemPadding.top + rcItemPadding.bottom;
	return _sizeEntireItem;
}

void CExtHeaderCtrl::OnCalcHeaderItemLayout(
	INT nColNo,
	const CRect & _rcItemEntire,
	CRect & _rcItem,
	CRect & _rcIcon,
	CRect & _rcText,
	CRect & _rcSortArrow,
	CRect & _rcButton,
	CRect & _rcButtonIcon,
	bool * p_bSorted, // = NULL
	bool * p_bSortedAscending // = NULL
	) const
{
	ASSERT_VALID( this );
INT nColCount = INT( GetItemCount() );
	ASSERT( 0 <= nColNo && nColNo < nColCount );
CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );
	_rcItem = _rcItemEntire;
const EXTENDED_ITEM_DATA & _eii = ExtendedItemDataGet( nColNo );
	_rcButton.SetRect( 0, 0, 0, 0 );
	_rcButtonIcon.SetRect( 0, 0, 0, 0 );
	if( ! _eii.m_iconButton.IsEmpty() )
	{
		CSize sizeButton = _eii.m_iconButton.GetSize();
		INT nButtonPaddingBefore = 0, nButtonPaddingAfter = 0;
		pPM->Header_GetButtonPadding( nButtonPaddingBefore, nButtonPaddingAfter, nColNo, nColCount, (CObject*)this );
		INT nButtonExtent = sizeButton.cx + nButtonPaddingBefore + nButtonPaddingAfter;
		_rcButton = _rcItem;
		_rcItem.right -= nButtonExtent;
		_rcButton.left = _rcItem.right;
		_rcButtonIcon = _rcButton;
		_rcButtonIcon.DeflateRect( nButtonPaddingBefore, 0, nButtonPaddingAfter, 0 );
		if( _rcButtonIcon.Height() != sizeButton.cy )
		{
			_rcButtonIcon.top += ( _rcButtonIcon.Height() - sizeButton.cy ) / 2;
			_rcButtonIcon.bottom = _rcButtonIcon.top + sizeButton.cy;
		}
	}
CRect rcItemPadding = pPM->Header_GetItemPadding( nColNo, nColCount, (CObject*)this );
	_rcItem.DeflateRect( &rcItemPadding );
	_rcText = _rcItem;
	_rcIcon.SetRect( 0, 0, 0, 0 );
	if( ! _eii.m_iconItem.IsEmpty() )
	{
		INT nBetweenIconAndText = pPM->Header_GetDistanceBetweenIconAndText( nColNo, nColCount, (CObject*)this );
		CSize _sizeIcon = _eii.m_iconItem.GetSize();
		_rcText.left += _sizeIcon.cx + nBetweenIconAndText;
		_rcIcon = _rcItem;
		_rcIcon.right = _rcIcon.left + _sizeIcon.cx;
		if( _rcIcon.Height() != _sizeIcon.cy )
		{
			_rcIcon.top += ( _rcIcon.Height() - _sizeIcon.cy ) / 2;
			_rcIcon.bottom = _rcIcon.top + _sizeIcon.cy;
		}
	}
	_rcSortArrow.SetRect( 0, 0, 0, 0 );
bool bSorted = false, bSortedAscending = false;
INT nSortRuleNo = m_sortOrder.GetColPos( nColNo );
	if( nSortRuleNo >= 0 )
	{
		INT nSortArrowExtent = pPM->Header_GetSortArrowWidth( nColNo, nColCount, (CObject*)this );
		const CExtListCtrlDataSortOrder::COLUMN_INFO & _columnInfo = m_sortOrder.m_arrItems[ nSortRuleNo ];
		_rcSortArrow = _rcText;
		_rcSortArrow.left = _rcSortArrow.right - nSortArrowExtent;
		if( _rcSortArrow.left >= _rcText. left )
		{
			INT nBetweenIconAndSortArrow = pPM->Header_GetDistanceBetweenIconAndSortArrow( nColNo, nColCount, (CObject*)this );
			_rcText.right = _rcSortArrow.left - nBetweenIconAndSortArrow;
			bSorted = true;
			bSortedAscending = _columnInfo.m_bAscending;
		}
	}
	if( p_bSorted != NULL )
		(*p_bSorted) = bSorted;
	if( p_bSortedAscending != NULL )
		(*p_bSortedAscending) = bSortedAscending;
}

void CExtHeaderCtrl::OnPaintHeaderItem(
	CDC & dc,
	const HD_ITEM & _item,
	INT nColNo,
	const CRect & _rcItemEntire,
	bool bDND // = false
	) const
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
INT nColCount = INT( GetItemCount() );
	ASSERT( 0 <= nColNo && nColNo < nColCount );
CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );
CRect _rcItem, _rcIcon, _rcText, _rcSortArrow, _rcButton, _rcButtonIcon;
bool bSorted = false,  bSortedAscending = false;
	OnCalcHeaderItemLayout(
		nColNo,
		_rcItemEntire,
		_rcItem,
		_rcIcon,
		_rcText,
		_rcSortArrow,
		_rcButton,
		_rcButtonIcon,
		&bSorted,
		&bSortedAscending
		);
const EXTENDED_ITEM_DATA & _eii = ExtendedItemDataGet( nColNo );
bool bHover       = ( (!bDND) && (!m_bOnDividerAtRight) && nColNo == m_nHoverColNo && ( nColNo == m_nPressingColNo || m_nPressingColNo < 0 ) ) ? true : false;
bool bPressed     = ( (!bDND) && (!m_bOnDividerAtRight) && nColNo == m_nPressingColNo ) ? true : false;
bool bButtonEvent = ( (!bDND) && (!m_bOnDividerAtRight) && m_bOnButton ) ? true : false;
	pPM->Header_PaintItem(
		dc,
		_rcItemEntire,
		_rcItem,
		_rcIcon,
		_rcText,
		_rcSortArrow,
		_rcButton,
		_rcButtonIcon,
		bSorted,
		bSortedAscending,
		nColNo,
		nColCount,
		_eii.m_iconItem,
		_eii.m_iconButton,
		bHover,
		bPressed,
		bButtonEvent,
		__EXT_MFC_SAFE_LPCTSTR(_item.pszText),
		(CObject*)this
		);
}

void CExtHeaderCtrl::OnCancelMode()
{
	ASSERT_VALID( this );
	_CancelActions();
	CHeaderCtrl::OnCancelMode();
}

void CExtHeaderCtrl::OnCaptureChanged( CWnd * pWnd ) 
{
	ASSERT_VALID( this );
	_CancelActions();
	CHeaderCtrl::OnCaptureChanged( pWnd );
}

CExtPopupMenuTipWnd * CExtHeaderCtrl::OnAdvancedPopupMenuTipWndGet() const
{
	if( m_nAdvancedTipStyle == INT(CExtPopupMenuTipWnd::__ETS_NONE) )
		return NULL;
	return (&( CExtPopupMenuSite::g_DefPopupMenuSite.GetTip() ));
}

void CExtHeaderCtrl::OnAdvancedPopupMenuTipWndDisplay(
	CExtPopupMenuTipWnd & _ATTW,
	const RECT & rcExcludeArea,
	__EXT_MFC_SAFE_LPCTSTR strToolTipText
	) const
{
	ASSERT_VALID( this );
	if( ( ! m_bShowTipsOverHeader ) || LPCTSTR(strToolTipText) == NULL || _tcslen( LPCTSTR(strToolTipText) ) == 0 )
	{
		_ATTW.Hide();
		return;
	}
	_ATTW.SetText( LPCTSTR(strToolTipText) );
	_ATTW.SetTipStyle( (CExtPopupMenuTipWnd::e_tip_style_t)m_nAdvancedTipStyle );
	_ATTW.Show( (CWnd*)this, rcExcludeArea );
}


INT CExtHeaderCtrl::ColNo2VisualIndex( INT nColNo ) const // convert column number to visual index
{
	ASSERT_VALID( this );
	if( nColNo < 0 || GetSafeHwnd() == NULL )
		return -1;
INT nColCount = INT(GetItemCount());
	ASSERT( nColCount >= 0 );
	if( nColNo >= nColCount )
		return -1;
INT * pArr = NULL, nIndex = -1;
	try
	{
		pArr = new INT[ nColCount ];
		if( ! ( const_cast < CExtHeaderCtrl * > ( this ) ) -> GetOrderArray( pArr, nColCount ) )
			::AfxThrowUserException();
		for( nIndex = 0; nIndex < nColCount; nIndex ++ )
		{
			if( pArr[nIndex] == nColNo )
				break;
		}
	}
	catch( CException * pException )
	{
		pException->Delete();
	}
	if( pArr != NULL )
		delete [] pArr;
	if( nIndex >= nColCount )
		return -1;
	return nIndex;
}

INT CExtHeaderCtrl::VisualIndex2ColNo( INT nIndex ) const // convert visual index to column number
{
	ASSERT_VALID( this );
	if( nIndex < 0 || GetSafeHwnd() == NULL )
		return -1;
INT nColCount = INT(GetItemCount());
	ASSERT( nColCount >= 0 );
	if( nIndex >= nColCount )
		return -1;
INT * pArr = NULL, nColNo = -1;
	try
	{
		pArr = new INT[ nColCount ];
		if( ! ( const_cast < CExtHeaderCtrl * > ( this ) ) -> GetOrderArray( pArr, nColCount ) )
			::AfxThrowUserException();
		nColNo = pArr[nIndex];
	}
	catch( CException * pException )
	{
		pException->Delete();
	}
	if( pArr != NULL )
		delete [] pArr;
	if( nColNo >= nColCount )
		return -1;
	return nColNo;
}

INT CExtHeaderCtrl::HitTestHeaderItem(
	const POINT & ptClient,
	bool * p_bOnButton, // = NULL
	bool * p_bOnDividerAtRight, // = NULL
	bool bComputeZeroWidthItem // = true
	) const
{
	ASSERT_VALID( this );
	if( p_bOnButton != NULL )
		(*p_bOnButton) = false;
	if( p_bOnDividerAtRight != NULL )
		(*p_bOnDividerAtRight) = false;
	if( GetSafeHwnd() == NULL )
		return -1;
INT nColCount = INT(GetItemCount());
	ASSERT( nColCount >= 0 );
	if( nColCount == 0 )
		return -1;
bool bOnButton = false, bOnDividerAtRight = false;
HDHITTESTINFO _hdhti;
	::memset( &_hdhti, 0, sizeof(HDHITTESTINFO) );
	_hdhti.pt = ptClient;
	::SendMessage( m_hWnd, HDM_HITTEST, 0, LPARAM( &_hdhti ) );
INT nColNo = INT(_hdhti.iItem);
CRect _rcItemEntire, _rcItem, _rcIcon, _rcText, _rcSortArrow, _rcButton, _rcButtonIcon;
bool bCheckLast = false;
	if( nColNo >= 0 )
	{
		if( ! GetItemRect( nColNo, &_rcItemEntire ) )
			return -1;
		OnCalcHeaderItemLayout(
			nColNo,
			_rcItemEntire,
			_rcItem,
			_rcIcon,
			_rcText,
			_rcSortArrow,
			_rcButton,
			_rcButtonIcon
			);
		if( _rcButton.PtInRect( ptClient ) )
			bOnButton = true;
		CRect rc = _rcItemEntire;
		rc.left = rc.right - m_nDividerHalfExtent;
		if( rc.PtInRect( ptClient ) )
			bOnDividerAtRight = true;
		else
		{
			INT nIndex = ColNo2VisualIndex( nColNo );
			if( nIndex > 0 )
			{
				rc = _rcItemEntire;
				rc.right = rc.left + m_nDividerHalfExtent;
				if( rc.PtInRect( ptClient ) )
				{
					bOnDividerAtRight = true;
					nIndex --;
					nColNo = VisualIndex2ColNo( nIndex );
				} // if( rc.PtInRect( ptClient ) )
				else if( nIndex < ( nColCount - 1 ) )
				{
					INT nColNoNext = VisualIndex2ColNo( nIndex + 1 );
					if( nColNoNext >= 0 )
					{
						CRect rcNext;
						if( GetItemRect( nColNoNext, &rcNext ) )
						{
							CRect rc = rcNext;
							rc.right = rc.left + m_nDividerHalfExtent;
							if( rc.PtInRect( ptClient ) )
								bOnDividerAtRight = true;
						} // if( GetItemRect( nColNoNext, &rcNext ) )
					} // if( nColNoNext >= 0 )
				} // else if( nIndex < ( nColCount - 1 ) )
				else if( nIndex == ( nColCount - 1 ) )
					bCheckLast = true;
			} // if( nIndex > 0 )
		} // else from if( rc.PtInRect( ptClient ) )
	} // if( nColNo >= 0 )
	if( bCheckLast )
	{
		INT nColNoLast = VisualIndex2ColNo( nColCount - 1 );
		if( ! GetItemRect( nColNoLast, &_rcItemEntire ) )
			return -1;
		OnCalcHeaderItemLayout(
			nColNoLast,
			_rcItemEntire,
			_rcItem,
			_rcIcon,
			_rcText,
			_rcSortArrow,
			_rcButton,
			_rcButtonIcon
			);
		CRect rc = _rcItemEntire;
		rc.left = rc.right;
		rc.right += m_nDividerHalfExtent;
		if( rc.PtInRect( ptClient ) )
		{
			bOnDividerAtRight = true;
			nColNo = nColNoLast;
		} // if( rc.PtInRect( ptClient ) )
	} // if( bCheckLast )
	if( bOnDividerAtRight )
	{
		const EXTENDED_ITEM_DATA & _eii = m_arrExtendedData.ElementAt( nColNo );
		if( ! _eii.CanBeResized() )
			bOnDividerAtRight = false;
	}
	if( bOnDividerAtRight )
	{
		bOnButton = false;
		if( nColNo >= 0 && bComputeZeroWidthItem )
		{
			INT nIndex = ColNo2VisualIndex( nColNo );
			if( nIndex >= 0 )
			{
				for( nIndex ++; nIndex < nColCount; nIndex++ )
				{
					CRect _rcItemEntire( 0, 32767, 0, 0 );
					INT nTmpColNo = VisualIndex2ColNo( nIndex );
					if( nTmpColNo <= 0 )
						break;
					if( ! GetItemRect( nTmpColNo, &_rcItemEntire ) )
						break;
					if( _rcItemEntire.Width() > 0 )
						break;
					nColNo = nTmpColNo;
				}
			} // if( nIndex >= 0 )
		} // if( nColNo >= 0 && bComputeZeroWidthItem )
	} // if( bOnDividerAtRight )
	else
	{
		if( nColNo >= 0 && bComputeZeroWidthItem )
		{
			INT nIndex = ColNo2VisualIndex( nColNo );
			for( ; 0 <= nIndex && nIndex < nColCount; nIndex++ )
			{
				CRect _rcItemEntire( 0, 32767, 0, 0 );
				INT nTmpColNo = VisualIndex2ColNo( nIndex );
				if( nTmpColNo <= 0 )
					break;
				if( ! GetItemRect( nTmpColNo, &_rcItemEntire ) )
					break;
				if( _rcItemEntire.Width() > 0 )
					break;
				nColNo = nTmpColNo;
				bOnButton = false;
				bOnDividerAtRight = true;
			} // for( ; 0 <= nIndex && nIndex < nColCount; nIndex++ )
		}
	} // else from if( bOnDividerAtRight )
	if( p_bOnButton != NULL )
		(*p_bOnButton) = bOnButton;
	if( p_bOnDividerAtRight != NULL )
		(*p_bOnDividerAtRight) = bOnDividerAtRight;
	return nColNo;
}

bool CExtHeaderCtrl::_DndIsAllowed(
	INT nColNo // = -1
	) const
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return false;
	
CWnd * pWndParent = GetParent();
	if(		pWndParent != NULL
		&&	pWndParent->IsKindOf( RUNTIME_CLASS(CListCtrl) )
		)
	{
		if( ( ((CListCtrl*)pWndParent)->GetExtendedStyle() & LVS_EX_HEADERDRAGDROP ) == 0 )
			return false;
	}
	else
	{
		if( ( GetStyle() & HDS_DRAGDROP ) == 0 )
			return false;
	}
	if( nColNo < 0 )
		return true;
INT nColCount = INT( GetItemCount() );
	if( nColNo >= nColCount )
		return false;
const EXTENDED_ITEM_DATA & _eii = ExtendedItemDataGet( nColNo );
	if( _eii.m_bDisableDnd )
		return false;
	return true;
}

CPoint CExtHeaderCtrl::_DndGetStartOffset() const
{
	ASSERT_VALID( this );
	return CPoint( 3, 3 );
}

INT CExtHeaderCtrl::_DndCalcOuterDropTarget( CPoint point ) const
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
INT nColCount = INT( GetItemCount() );
	if( nColCount == 0 )
		return -1;
INT nColNoToDropBefore = HitTestHeaderItem( point, NULL, NULL, false );
	if( nColNoToDropBefore < 0 )
	{
		CRect rcClient;
		GetClientRect( &rcClient);
		if( rcClient.top <= point.y && point.y <= rcClient.bottom )
		{
			INT nColNoLast = VisualIndex2ColNo( nColCount - 1 );
			if( nColNoLast >= 0 )
			{
				CRect rcItem;
				if(		GetItemRect( nColNoLast, &rcItem )
					&&	point.x >= rcItem.right
					)
					return nColCount;
			} // if( nColNoLast >= 0 )
		} // if( rcClient.top <= point.y && point.y <= rcClient.bottom )
		return -1;
	} // if( nColNoToDropBefore < 0 )
CRect rcItem;
	if( ! GetItemRect( nColNoToDropBefore, &rcItem ) )
		return -1;
INT nDistanceLeft = abs( point.x - rcItem.left );
INT nDistanceRight = abs( point.x - rcItem.right );
	if( nDistanceLeft > nDistanceRight )
	{
		INT nIndexToDropBefore = ColNo2VisualIndex( nColNoToDropBefore );
		if( nIndexToDropBefore >= 0 )
		{
			ASSERT( nIndexToDropBefore < nColCount );
			nIndexToDropBefore ++;
			ASSERT( nIndexToDropBefore <= nColCount );
			if( nIndexToDropBefore == nColCount )
				nColNoToDropBefore = nColCount;
			else
				nColNoToDropBefore = VisualIndex2ColNo( nIndexToDropBefore );
		} // if( nIndexToDropBefore >= 0 )
	} // if( nDistanceLeft > nDistanceRight )
	ASSERT( nColNoToDropBefore <= nColCount );
	return nColNoToDropBefore;
}

void CExtHeaderCtrl::_DndDo(
	INT nColNo,
	CPoint ptStartDrag,
	bool bNotify // = true // send HDN_BEGINDRAG and HDN_ENDDRAG messages
	)
{
	ASSERT_VALID( this );
HWND hWndOwn = GetSafeHwnd();
	ASSERT( hWndOwn != NULL );
INT nColCount = INT( GetItemCount() );
	if( nColCount == 0 )
		return;
	ASSERT( 0 <= nColNo && nColNo <= nColCount );
HWND hWndParent = ::GetParent( hWndOwn );
	if( hWndParent == NULL )
		bNotify = false;
UINT nOwnID = UINT( GetDlgCtrlID() );
HD_NOTIFY _data;
	::memset( &_data, 0, sizeof(HD_NOTIFY) );
	_data.hdr.hwndFrom = hWndOwn;
	_data.hdr.idFrom = nOwnID;
	_data.hdr.code = NM_CLICK;
	_data.iButton = 0;
	_data.iItem = nColNo;
HDITEM hdItemBuffer; // to avoid common controls crashing
::memset( &hdItemBuffer, 0, sizeof(HDITEM) );
_data.pitem = &hdItemBuffer;
	if( bNotify )
	{
		HD_NOTIFY _dataA;
		::memcpy( &_dataA, &_data, sizeof(HD_NOTIFY) );
		_dataA.hdr.code = HDN_BEGINDRAG;
		if( ::SendMessage( hWndParent, WM_NOTIFY, WPARAM(nOwnID), LPARAM(&_dataA) ) != 0 )
			return;
	}
CExtPopupMenuTipWnd * pATTW = OnAdvancedPopupMenuTipWndGet();
	if( pATTW != NULL )
		pATTW->Hide();
CRect rcDND;
	if( ! GetItemRect( nColNo, &rcDND ) )
		return;
	ClientToScreen( &rcDND );
CExtContentExpandWnd wndDND, wndArrows;
	if( ! wndDND.Activate(
			rcDND,
			this,
			__ECWAF_DEF_EXPANDED_ITEM_PAINTER
				|__ECWAF_NO_CAPTURE
				|__ECWAF_REDIRECT_MOUSE
				|__ECWAF_REDIRECT_NO_DEACTIVATE
				|__ECWAF_REDIRECT_AND_HANDLE
				|__ECWAF_HANDLE_MOUSE_ACTIVATE
				|__ECWAF_MA_NOACTIVATE
			)
		)
	{
		ASSERT( FALSE );
		return;
	}
CPoint ptShiftLast( 0, 0 );
CRect rcInitialWndDND;
	wndDND.GetWindowRect( &rcInitialWndDND );
	ASSERT( m_hCursorOuterDragOK != NULL );
	ASSERT( m_hCursorOuterDragCancel != NULL );
	::SetCursor( m_hCursorOuterDragOK );
bool bStopFlag = false;
INT nLastTargetColNoToDropBefore = -1;
INT nLastTargetIndexToDropBefore = -1;
CRect rcArrows( 0, 0, 0, 0 );
	for(	MSG msg;
				::IsWindow( hWndOwn )
			&&	(!bStopFlag)
			;
		)
	{ // main message loop
		if( ! ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if(		( ! ::IsWindow( hWndOwn ) )
				||	bStopFlag
				)
				break;
			if( g_bEnableOnIdleCalls )
			{
				for(	LONG nIdleCounter = 0L;
						::AfxGetThread()->OnIdle(nIdleCounter);
						nIdleCounter ++
						);
			}
			::WaitMessage();
			continue;
		} // if( ! ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		switch( msg.message )
		{
		case WM_KILLFOCUS:
			if( msg.hwnd == hWndOwn )
				bStopFlag = true;
		break;
		case WM_CANCELMODE:
		case WM_ACTIVATEAPP:
		case WM_SYSCOMMAND:
		case WM_SETTINGCHANGE:
		case WM_SYSCOLORCHANGE:
			bStopFlag = true;
		break;
		case WM_COMMAND:
			if(		(HIWORD(msg.wParam)) == 0
				||	(HIWORD(msg.wParam)) == 1
				)
				bStopFlag = true;
		break;
		case WM_CAPTURECHANGED:
			if( (HWND)msg.wParam != hWndOwn )
				bStopFlag = true;
		break;
		case WM_MOUSEWHEEL:
			if( msg.hwnd != hWndOwn )
				bStopFlag = true;
			else
			{
				::PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
				continue;
			} // else from if( msg.hwnd != hWndOwn )
		break;
		case WM_MOUSEMOVE:
			if( msg.hwnd != hWndOwn )
				bStopFlag = true;
			else
			{
				ASSERT_VALID( this );
				::PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
				CPoint point;
				point = DWORD( msg.lParam );
				CPoint ptShift = point - ptStartDrag;
				if( ptShift != ptShiftLast )
				{
					INT nColNoDropBefore = _DndCalcOuterDropTarget( point );
					INT nIndexDropBefore = -1;
					ASSERT( nColNoDropBefore <= nColCount );
					if( nColNoDropBefore >= 0 )
					{
						if( nColNoDropBefore == nColCount )
							nIndexDropBefore = nColCount;
						else
						{
							nIndexDropBefore = ColNo2VisualIndex( nColNoDropBefore );
							if( nIndexDropBefore < 0 )
								nColNoDropBefore = -1;
						}
					}

					if( nLastTargetIndexToDropBefore != nIndexDropBefore )
					{
						nLastTargetColNoToDropBefore = nColNoDropBefore;
						nLastTargetIndexToDropBefore = nIndexDropBefore;
						rcArrows.SetRect( 0, 0, 0, 0 );
						if( nLastTargetIndexToDropBefore >= 0 )
						{
							if( nLastTargetIndexToDropBefore == nColCount )
							{
								INT nColNoLast = VisualIndex2ColNo( nColCount - 1 );
								if( GetItemRect( nColNoLast, &rcArrows ) )
									rcArrows.left = rcArrows.right;
								else
									rcArrows.SetRect( 0, 0, 0, 0 );
							}
							else
							{
								if( GetItemRect( nLastTargetColNoToDropBefore, &rcArrows ) )
									rcArrows.right = rcArrows.left;
								else
									rcArrows.SetRect( 0, 0, 0, 0 );
							}
							if( rcArrows.Height() == 0 )
								nLastTargetColNoToDropBefore = -1;
						} // if( nLastTargetColNoToDropBefore >= 0 )
						if( nLastTargetColNoToDropBefore >= 0 )
						{
							VERIFY(
								wndArrows.Activate(
									rcArrows,
									this,
									__ECWAF_DRAW_RED_ARROWS
										|__ECWAF_TRANSPARENT_ITEM
										|__ECWAF_NO_CAPTURE
										|__ECWAF_REDIRECT_MOUSE
										|__ECWAF_REDIRECT_NO_DEACTIVATE
										|__ECWAF_REDIRECT_AND_HANDLE
										|__ECWAF_HANDLE_MOUSE_ACTIVATE
										|__ECWAF_MA_NOACTIVATE
									)
								);
							if( wndArrows.GetSafeHwnd() != NULL )
							{
								wndArrows.SetWindowPos(
									&wndDND, 0, 0, 0, 0,
									SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE
									);
								::SetCursor( m_hCursorOuterDragOK );
							}
							else
								nLastTargetColNoToDropBefore = -1;
						} // if( nLastTargetColNoToDropBefore >= 0 )
					} // if( nLastTargetIndexToDropBefore != nIndexDropBefore )

					if( nLastTargetColNoToDropBefore < 0 )
					{
						wndArrows.Deactivate();
						::SetCursor( m_hCursorOuterDragCancel );
					}
					ptShiftLast = ptShift;
					CRect rcWnd( rcInitialWndDND );
					rcWnd.OffsetRect( ptShift );
					wndDND.MoveWindow( &rcWnd );
					wndDND.UpdateWindow();
					if( wndArrows.GetSafeHwnd() != NULL )
						wndArrows.UpdateWindow();
					UpdateWindow();
					CExtPaintManager::stat_PassPaintMessages();
				} // if( ptShift != ptShiftLast )
				continue;
			} // else from if( msg.hwnd != hWndOwn )
		break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			bStopFlag = true;
			::PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
		break;
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_CONTEXTMENU:
		case WM_NCLBUTTONUP:
		case WM_NCLBUTTONDBLCLK:
		case WM_NCLBUTTONDOWN:
		case WM_NCRBUTTONUP:
		case WM_NCRBUTTONDBLCLK:
		case WM_NCRBUTTONDOWN:
		case WM_NCMBUTTONUP:
		case WM_NCMBUTTONDBLCLK:
		case WM_NCMBUTTONDOWN:
			bStopFlag = true;
		break;
		default:
			if(	   WM_KEYFIRST <= msg.message
				&& msg.message <= WM_KEYLAST
				)
			{
				bStopFlag = true;
				::PeekMessage(&msg,NULL,msg.message,msg.message,PM_REMOVE);
			}
		break;
		} // switch( msg.message )
		if( bStopFlag )
			break;
		if( ! ::AfxGetThread() -> PumpMessage() )
			break;
	} // message loop
	wndDND.Deactivate();
	wndArrows.Deactivate();
	if( ! ::IsWindow( hWndOwn ) )
		return;
	_CancelActions();
	_DoSetCursor();
	if( nLastTargetColNoToDropBefore >= 0 )
		_DndSwap( nColNo, nLastTargetColNoToDropBefore );
	if( bNotify && (! ::IsWindow( hWndParent ) ) )
		bNotify = false;
	if( bNotify )
	{
		HD_NOTIFY _dataA;
HDITEM hdItemBuffer; // to avoid common controls crashing
::memset( &hdItemBuffer, 0, sizeof(HDITEM) );
_data.pitem = &hdItemBuffer;
		::memcpy( &_dataA, &_data, sizeof(HD_NOTIFY) );
		_dataA.hdr.code = HDN_ENDDRAG;
		::SendMessage( hWndParent, WM_NOTIFY, WPARAM(nOwnID), LPARAM(&_dataA) );
	}
	Invalidate();
	UpdateWindow();
}

void CExtHeaderCtrl::_DndSwap( INT nColNo, INT nColNoToDropBefore )
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	if( nColNo < 0 || nColNoToDropBefore < 0 )
		return;
INT nColCount = INT( GetItemCount() );
	if( nColCount == 0 )
		return;
INT nIndex = ColNo2VisualIndex( nColNo );
	if( nIndex < 0 )
		return;
INT nIndexToDropBefore = ( nColNoToDropBefore == nColCount ) ? nColCount : ColNo2VisualIndex( nColNoToDropBefore );
	if( nIndexToDropBefore < 0 )
		return;
	if( nIndexToDropBefore == nIndex || nIndexToDropBefore == (nIndex+1) )
		return;
	if( nColNo >= nColCount || nColNoToDropBefore > nColCount )
		return;
	if( nIndex >= nColCount || nIndexToDropBefore > nColCount )
		return;
INT * pArr = NULL;
	try
	{
		pArr = new INT[ nColCount ];
		if( ! GetOrderArray( pArr, nColCount ) )
			::AfxThrowUserException();
		if( nIndexToDropBefore < nIndex )
		{
			INT nVal = pArr[nIndex];
			INT nPos = nIndex;
			for( ; nPos > nIndexToDropBefore; nPos -- )
				pArr[nPos] = pArr[nPos-1];
			pArr[nIndexToDropBefore] = nVal;
		}
		else
		{
			ASSERT( nIndexToDropBefore > (nIndex+1) );
			INT nVal = pArr[nIndex];
			INT nPos = nIndexToDropBefore-1;
			for( ; nPos > nIndex; nPos -- )
				pArr[nPos] = pArr[nPos-1];
			pArr[nIndexToDropBefore-1] = nVal;
		}
		VERIFY( SetOrderArray( nColCount, pArr ) );
	}
	catch( CException * pException )
	{
		pException->Delete();
	}
	if( pArr != NULL )
		delete [] pArr;
}

bool CExtHeaderCtrl::_PaintExpandedItemContent(
	CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA & picd
	) const
{
	ASSERT_VALID( this );
	ASSERT( picd.m_dc.GetSafeHdc() != NULL );
	if( m_nPressingColNo >= 0 )
	{ // drag-n-dropped item
		OnEraseHeaderBackground( picd.m_dc, picd.m_rcItem );
		HD_ITEM _item;
		::memset( &_item, 0, sizeof(HD_ITEM) );
		_item.mask = UINT(-1);
		TCHAR strText[ 1024 ];
		::memset( strText, 0, sizeof( strText ) );
		_item.cchTextMax = sizeof( strText ) / sizeof( strText[0] );
		_item.pszText = strText;
		GetItem( m_nPressingColNo, &_item );
		OnPaintHeaderItem( picd.m_dc, _item, m_nPressingColNo, picd.m_rcItem, true );
		return true;
	} // drag-n-dropped item
	return false;
}

bool CExtHeaderCtrl::_DoSetCursor()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	return _DoSetCursor( CPoint(-32767,-32767) );
}

bool CExtHeaderCtrl::_DoSetCursor( const POINT ptScreen ) // CPoint(-32767,-32767) means invoke ::GetCursorPos()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
CPoint point = ptScreen;
	if( point.x == -32767 || point.y == -32767 )
	{
		if( ! ::GetCursorPos( &point ) )
			return false;
	}
HCURSOR hCursor = NULL;
HWND hWndFromPoint = ::WindowFromPoint( point );
	if( hWndFromPoint == m_hWnd )
	{
		ScreenToClient( &point );
		bool bZeroResizing = false, bResizing = ( m_nPressingColNo >= 0 && m_bOnDividerAtRight ) ? true : false;
		if( ! bResizing )
		{
			bool bOnButton = false, bOnDividerAtRight = false;
			INT nColNo = -1, nIndex = -1;
			if( CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND( m_hWnd ) )
			{
				nColNo = HitTestHeaderItem( point, &bOnButton, &bOnDividerAtRight );
				nIndex = ColNo2VisualIndex( nColNo );
			}
			bResizing = ( nIndex >= 0 && bOnDividerAtRight ) ? true : false;
			if( bResizing )
			{
				CRect rcItem( 0, 32767, 0, 0 );
				if(		GetItemRect( nColNo, &rcItem )
					&&	rcItem.Width() == 0
					)
					bZeroResizing = true;
			}
		} // if( ! bResizing )
		hCursor =
			bResizing
				? ( bZeroResizing ? m_hCursorResizingH2 : m_hCursorResizingH1 )
				: m_hCursorDefault
				;
		if( hCursor == NULL )
			hCursor = ::LoadCursor( NULL, bResizing ? IDC_SIZEWE : IDC_ARROW );
	} // if( hWndFromPoint == m_hWnd )
	if( hCursor == NULL )
		return false;
	::SetCursor( hCursor );
	return true;
}

BOOL CExtHeaderCtrl::OnSetCursor( CWnd * pWnd, UINT nHitTest, UINT message )
{
	ASSERT_VALID( this );
	pWnd;
	nHitTest;
	message;
	return _DoSetCursor() ? TRUE : FALSE;
}

INT CExtHeaderCtrl::HeaderItemExtentGet( INT nColNo ) const
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nColNo && nColNo < INT(GetItemCount()) );
	if( GetSafeHwnd() == NULL )
		return 0;
HD_ITEM _item;
	::memset( &_item, 0, sizeof(HD_ITEM) );
	_item.mask = HDI_WIDTH;
	GetItem( nColNo, &_item );
INT nCurrentItemExtent = INT(_item.cxy);
	return nCurrentItemExtent;
}

void CExtHeaderCtrl::HeaderItemExtentSet(
	INT nColNo,
	INT nExtent,
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nColNo && nColNo < INT(GetItemCount()) );
	if( GetSafeHwnd() == NULL )
		return;
const EXTENDED_ITEM_DATA & _eii = m_arrExtendedData.ElementAt( nColNo );
	if( nExtent < _eii.m_nExtentMin )
		nExtent = _eii.m_nExtentMin;
	if( nExtent > _eii.m_nExtentMax )
		nExtent = _eii.m_nExtentMax;
	if( nExtent < 0 )
		nExtent = 0;
HD_ITEM _item;
	::memset( &_item, 0, sizeof(HD_ITEM) );
	_item.mask = HDI_WIDTH;
	GetItem( nColNo, &_item );
INT nCurrentItemExtent = INT(_item.cxy);
	if( nCurrentItemExtent == nExtent )
		return;
	_item.mask = HDI_WIDTH;
	_item.cxy = nExtent;
	SetItem( nColNo, &_item );
	if( bRedraw )
		Invalidate();
}

void CExtHeaderCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
//	CHeaderCtrl::OnMouseMove( nFlags, point );
	nFlags;
	if( m_nPressingColNo >= 0 )
	{
		CExtPopupMenuTipWnd * pATTW = OnAdvancedPopupMenuTipWndGet();
		if( pATTW != NULL )
			pATTW->Hide();
		if( m_bOnDividerAtRight )
		{
			CPoint ptScreenPressing = point;
			ClientToScreen( &ptScreenPressing );
			INT nExtent = ptScreenPressing.x - m_ptScreenPressing.x + m_nHelperInitialResizingExtent;
			HeaderItemExtentSet( m_nPressingColNo, nExtent );
			_DoSetCursor();
			if( m_bOnDividerAtRight )
			{
				HWND hWndParent = ::GetParent( m_hWnd );
				if( hWndParent != NULL )
				{
					UINT nOwnID = UINT( GetDlgCtrlID() );
					HD_NOTIFY _data, _data2;
					::memset( &_data, 0, sizeof(HD_NOTIFY) );
					::memset( &_data2, 0, sizeof(HD_NOTIFY) );
					_data.hdr.hwndFrom = m_hWnd;
					_data.hdr.idFrom = nOwnID;
					_data.iButton = 0;
					_data.iItem = m_nPressingColNo; // nColNo;
					_data.hdr.code = HDN_TRACK;
HDITEM hdItemBuffer; // to avoid common controls crashing
::memset( &hdItemBuffer, 0, sizeof(HDITEM) );
hdItemBuffer.mask = HDI_WIDTH;
hdItemBuffer.cxy = nExtent;
_data.pitem = &hdItemBuffer;
					::memcpy( &_data2, &_data, sizeof(HD_NOTIFY) );
//#if (defined _UNICODE)
//					::LockWindowUpdate( hWndParent );
//#endif // (defined _UNICODE)
//					::SendMessage( hWndParent, WM_NOTIFY, WPARAM(nOwnID), LPARAM(&_data) );
//#if (defined _UNICODE)
//					::LockWindowUpdate( NULL );
//#endif // (defined _UNICODE)
					OnNotifyHdnTrack( _data );
					_data2.hdr.code = HDN_ITEMCHANGED;
					::SendMessage( hWndParent, WM_NOTIFY, WPARAM(nOwnID), LPARAM(&_data2) );
					if( ::IsWindow( hWndParent ) && ::IsWindowVisible( hWndParent ) )
						::InvalidateRect( hWndParent, NULL, TRUE );
				} // if( hWndParent != NULL )
			} // if( m_bOnDividerAtRight )
			return;
		}
		else if( ! m_bOnButton )
		{
			if( _DndIsAllowed( m_nPressingColNo ) )
			{
				CPoint ptOffset = _DndGetStartOffset();
				CPoint ptClientPressing;
				ptClientPressing = m_ptScreenPressing;
				ScreenToClient( &ptClientPressing );
				CPoint ptDiff = ptClientPressing - point;
				if( abs(ptDiff.x) >= ptOffset.x || abs(ptDiff.y) >= ptOffset.y )
				{
					_DndDo( m_nPressingColNo, m_ptScreenPressing );
					return;
				}
			}
		}
	}
bool bOnButton = false, bOnDividerAtRight = false;
INT nColNo = -1;
	if( CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND( m_hWnd ) )
	{
		CPoint ptScreen;
		ptScreen = point;
		ClientToScreen( &ptScreen );
		if( ::WindowFromPoint( ptScreen ) == m_hWnd )
			nColNo = HitTestHeaderItem( point, &bOnButton, &bOnDividerAtRight );
	}
bool bHoverChanged = false;
	if(		m_nHoverColNo != nColNo
		||	m_bOnButton != bOnButton
		||	( m_bOnDividerAtRight != bOnDividerAtRight && ( ! CExtPopupMenuWnd::IsKeyPressed( VK_LBUTTON ) ) )
		)
	{
		bHoverChanged = true;
		m_nHoverColNo = nColNo;
		m_nHoverIndex = ColNo2VisualIndex( m_nHoverColNo );
		m_bOnButton = bOnButton;
		m_bOnDividerAtRight = bOnDividerAtRight;
		if( m_bOnDividerAtRight )
		{
			HWND hWndParent = ::GetParent( m_hWnd );
			if( hWndParent != NULL )
			{
				UINT nOwnID = UINT( GetDlgCtrlID() );
				HD_NOTIFY _data;
				::memset( &_data, 0, sizeof(HD_NOTIFY) );
				_data.hdr.hwndFrom = m_hWnd;
				_data.hdr.idFrom = nOwnID;
				_data.iButton = 0;
				_data.iItem = nColNo;
				_data.hdr.code = HDN_BEGINTRACK;
HDITEM hdItemBuffer; // to avoid common controls crashing
::memset( &hdItemBuffer, 0, sizeof(HDITEM) );
_data.pitem = &hdItemBuffer;
				::SendMessage( hWndParent, WM_NOTIFY, WPARAM(nOwnID), LPARAM(&_data) );
			} // if( hWndParent != NULL )
		} // if( m_bOnDividerAtRight )
	}
	if( bHoverChanged )
	{
		if( m_nPressingColNo < 0 )
		{
			HWND hWndCapture = ::GetCapture();
			if( m_nHoverColNo >= 0 )
			{
				if( m_hWnd != hWndCapture )
					SetCapture();
				if(		m_nAdvancedTipStyle != INT(CExtPopupMenuTipWnd::__ETS_NONE)
					&&	(! CExtPopupMenuWnd::IsMenuTracking() )
					)
				{
					CExtPopupMenuTipWnd * pATTW = OnAdvancedPopupMenuTipWndGet();
					if( pATTW != NULL )
					{
						const CExtHeaderCtrl::EXTENDED_ITEM_DATA & _eii = ExtendedItemDataGet( m_nHoverColNo );
						CRect rcArea;
						if( GetItemRect( m_nHoverColNo, rcArea ) )
						{
							CRect _rcItem, _rcIcon, _rcText, _rcSortArrow, _rcButton, _rcButtonIcon;
							bool bSorted = false,  bSortedAscending = false;
							OnCalcHeaderItemLayout(
								m_nHoverColNo,
								rcArea,
								_rcItem,
								_rcIcon,
								_rcText,
								_rcSortArrow,
								_rcButton,
								_rcButtonIcon,
								&bSorted,
								&bSortedAscending
								);
							if( m_bOnButton )
							{
								rcArea.left = _rcButton.left;
								rcArea.right = _rcButton.right;
							}
							else
							{
								rcArea.left = _rcItem.left;
								rcArea.right = _rcItem.right;
							}
							ClientToScreen( &rcArea );
							OnAdvancedPopupMenuTipWndDisplay(
								*pATTW,
								rcArea,
								m_bOnButton ? LPCTSTR(_eii.m_strTipTextButton) : LPCTSTR(_eii.m_strTipTextItem)
								);
						}
					} // if( pATTW != NULL )
				}
			}
			else
			{
				if( m_hWnd == hWndCapture )
					ReleaseCapture();
			}
		} // if( m_nPressingColNo < 0 )
		Invalidate();
		_DoSetCursor();
	} // if( bHoverChanged )
}

void CExtHeaderCtrl::OnNotifyHdnTrack( HD_NOTIFY & _data )
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
HWND hWndParent = ::GetParent( m_hWnd );
	if( hWndParent == NULL )
		return;
CWnd * pWnd = CWnd::FromHandlePermanent( hWndParent );
	if( pWnd == NULL )
		return;
CExtListCtrl * pExtListCtrl = DYNAMIC_DOWNCAST( CExtListCtrl, pWnd );
	if( pWnd == NULL )
		return;
	pExtListCtrl->OnNotifyHdnTrack( *this, _data );
}

void CExtHeaderCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
//	CHeaderCtrl::OnLButtonDown( nFlags, point );
	nFlags;
	m_ptScreenPressing.x = m_ptScreenPressing.y = m_nHelperInitialResizingExtent = -32767;
	ActivateTopParent();
	_DoSetCursor();
bool bOnButton = false, bOnDividerAtRight = false;
INT nColNo = HitTestHeaderItem( point, &bOnButton, &bOnDividerAtRight );
	if( nColNo < 0 )
		return;
	m_nPressingColNo = nColNo;
	m_nPressingIndex = ColNo2VisualIndex( m_nPressingColNo );
	m_bOnButton = bOnButton;
	m_bOnDividerAtRight = bOnDividerAtRight;
	m_ptScreenPressing = point;
	ClientToScreen( &m_ptScreenPressing );
	m_nHelperInitialResizingExtent = HeaderItemExtentGet( nColNo );
	Invalidate();
HWND hWndCapture = ::GetCapture();
	if( m_hWnd != hWndCapture )
		SetCapture();
CExtPopupMenuTipWnd * pATTW = OnAdvancedPopupMenuTipWndGet();
	if( pATTW != NULL )
		pATTW->Hide();
	if( m_bOnDividerAtRight )
	{
		HWND hWndParent = ::GetParent( m_hWnd );
		if( hWndParent != NULL )
		{
			UINT nOwnID = UINT( GetDlgCtrlID() );
			HD_NOTIFY _data;
			::memset( &_data, 0, sizeof(HD_NOTIFY) );
			_data.hdr.hwndFrom = m_hWnd;
			_data.hdr.idFrom = nOwnID;
			_data.iButton = 0;
			_data.iItem = nColNo;
			_data.hdr.code = HDN_BEGINTRACK;
HDITEM hdItemBuffer; // to avoid common controls crashing
::memset( &hdItemBuffer, 0, sizeof(HDITEM) );
_data.pitem = &hdItemBuffer;
			::SendMessage( hWndParent, WM_NOTIFY, WPARAM(nOwnID), LPARAM(&_data) );
		} // if( hWndParent != NULL )
	} // if( m_bOnDividerAtRight )
}

void CExtHeaderCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
//	CHeaderCtrl::OnLButtonUp( nFlags, point );
	nFlags;
	point;
	if( m_nPressingColNo < 0 )
		return;
bool bOnButton = false, bOnDividerAtRight = false;
INT nColNo = -1;
CPoint ptScreen;
	ptScreen = point;
	ClientToScreen( &ptScreen );
	if( ::WindowFromPoint( ptScreen ) == m_hWnd )
		nColNo = HitTestHeaderItem( point, &bOnButton, &bOnDividerAtRight );
	if( nColNo == m_nPressingColNo )
	{
		bool bResizing = m_bOnDividerAtRight;
		bool bButtonClick = ( m_bOnButton && (!m_bOnDividerAtRight) ) ? true : false;
		if( bResizing )
		{
			_CancelActions();
			return;
		}
		if( m_bOnButton && bButtonClick )
		{
			_CancelActions();
			OnHeaderItemButtonClick( nColNo );
			return;
		}
		_CancelActions();
		OnHeaderItemClick( nColNo, 0 );
	}
	else
		_CancelActions();
}

void CExtHeaderCtrl::OnHeaderItemClick(
	INT nColNo,
	INT nMouseButton // 0 - left button, 1 - right button, 2 - middle button
	)
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nColNo && nColNo < INT(GetItemCount()) );
	nColNo;
HWND hWndParent = ::GetParent( m_hWnd );
	if( hWndParent == NULL )
		return;
UINT nOwnID = UINT( GetDlgCtrlID() );
HD_NOTIFY _data;
	::memset( &_data, 0, sizeof(HD_NOTIFY) );
	_data.hdr.hwndFrom = m_hWnd;
	_data.hdr.idFrom = nOwnID;
	_data.hdr.code = HDN_ITEMCLICK;
	_data.iButton = nMouseButton;
	_data.iItem = nColNo;
HDITEM hdItemBuffer; // to avoid common controls crashing
::memset( &hdItemBuffer, 0, sizeof(HDITEM) );
_data.pitem = &hdItemBuffer;
	::SendMessage( hWndParent, WM_NOTIFY, WPARAM(nOwnID), LPARAM(&_data) );
}

void CExtHeaderCtrl::OnHeaderItemButtonClick( INT nColNo )
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nColNo && nColNo < INT(GetItemCount()) );
	nColNo;
	GetParent()->SendMessage( CExtHeaderCtrl::g_nMsgHeaderButtonClick, WPARAM(nColNo), LPARAM(this) );
}

void CExtHeaderCtrl::OnHeaderItemDividerAtRightDoubleClick( INT nColNo )
{
	ASSERT_VALID( this );
	ASSERT( 0 <= nColNo && nColNo < INT(GetItemCount()) );
HWND hWndParent = ::GetParent( m_hWnd );
	if( hWndParent == NULL )
		return;
UINT nOwnID = UINT( GetDlgCtrlID() );
HD_NOTIFY _data;
	::memset( &_data, 0, sizeof(HD_NOTIFY) );
	_data.hdr.hwndFrom = m_hWnd;
	_data.hdr.idFrom = nOwnID;
	_data.hdr.code = HDN_ITEMDBLCLICK;
	_data.iButton = 0;
	_data.iItem = nColNo;
HDITEM hdItemBuffer; // to avoid common controls crashing
::memset( &hdItemBuffer, 0, sizeof(HDITEM) );
_data.pitem = &hdItemBuffer;
	::SendMessage( hWndParent, WM_NOTIFY, WPARAM(nOwnID), LPARAM(&_data) );}

void CExtHeaderCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
//	CHeaderCtrl::OnLButtonDown( nFlags, point );
	nFlags;
bool bOnButton = false, bOnDividerAtRight = false;
INT nColNo = HitTestHeaderItem( point, &bOnButton, &bOnDividerAtRight );
	if( nColNo < 0 )
		return;
	if( ! bOnDividerAtRight )
		return;
	OnHeaderItemDividerAtRightDoubleClick( nColNo );
}

void CExtHeaderCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
//	CHeaderCtrl::OnRButtonDown( nFlags, point );
	nFlags;
INT nColNo = HitTestHeaderItem( point, NULL, NULL, false );
	if( nColNo >= 0 )
	{
		_CancelActions();
		OnHeaderItemClick( nColNo, 1 );
	}
}

void CExtHeaderCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
//	CHeaderCtrl::OnRButtonUp( nFlags, point );
	nFlags;
	point;
}

void CExtHeaderCtrl::OnRButtonDblClk( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
//	CHeaderCtrl::OnRButtonDown( nFlags, point );
	nFlags;
	point;
}

void CExtHeaderCtrl::OnMButtonDown( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
//	CHeaderCtrl::OnMButtonDown( nFlags, point );
	nFlags;
INT nColNo = HitTestHeaderItem( point, NULL, NULL, false );
	if( nColNo >= 0 )
	{
		_CancelActions();
		OnHeaderItemClick( nColNo, 2 );
	}
}

void CExtHeaderCtrl::OnMButtonUp( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
//	CHeaderCtrl::OnMButtonUp( nFlags, point );
	nFlags;
	point;
}

void CExtHeaderCtrl::OnMButtonDblClk( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
//	CHeaderCtrl::OnMButtonDown( nFlags, point );
	nFlags;
	point;
}

void CExtHeaderCtrl::OnWindowPosChanging( WINDOWPOS FAR * lpwndpos ) 
{
	CHeaderCtrl::OnWindowPosChanging(lpwndpos);
	Invalidate();
}

BOOL CExtHeaderCtrl::OnEraseBkgnd( CDC * pDC )
{
	ASSERT_VALID( this );
	pDC;
	return TRUE;
}

LRESULT CExtHeaderCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch( message )
	{
	case HDM_INSERTITEM:
	{
		ASSERT( m_arrExtendedData.GetSize() == GetItemCount() );
		static EXTENDED_ITEM_DATA g_eii;
		m_arrExtendedData.InsertAt( (INT) wParam, g_eii, 1 );
		//ASSERT( m_arrExtendedData.GetSize() == GetItemCount() );
		_CancelActions();
	}
	break;
	case HDM_DELETEITEM:
	{
		//ASSERT( m_arrExtendedData.GetSize() == GetItemCount() );
		m_arrExtendedData.RemoveAt( ( int ) wParam );
		//ASSERT( m_arrExtendedData.GetSize() == GetItemCount() );
		_CancelActions();
	}
	break;
	case HDM_HITTEST:
	{
		HDHITTESTINFO * pHDHTI = reinterpret_cast < HDHITTESTINFO * > ( lParam );
		if( pHDHTI == NULL )
			return -1;
		pHDHTI->iItem = -1;
		pHDHTI->flags = HHT_NOWHERE;
		CRect rcClient;
		GetClientRect( &rcClient );
		if( pHDHTI->pt.y < rcClient.top )
		{
			pHDHTI->flags = HHT_ABOVE;
			return -1;
		}
		if( pHDHTI->pt.y > rcClient.bottom )
		{
			pHDHTI->flags = HHT_BELOW;
			return -1;
		}
		INT nColNo, nColCount = INT(GetItemCount()), nWalkWay = 0;
		for( nColNo = 0; nColNo < nColCount; nColNo++ )
		{
			CRect _rcItemEntire;
			if( ! GetItemRect( nColNo, &_rcItemEntire ) )
				continue;
			if( ! _rcItemEntire.PtInRect( pHDHTI->pt ) )
				continue;
			pHDHTI->iItem = nColNo;
			pHDHTI->flags = 0;
			CRect rc = _rcItemEntire;
			rc.right = rc.left + m_nDividerHalfExtent;
			if( rc.PtInRect( pHDHTI->pt ) )
			{
				nWalkWay = -1;
				pHDHTI->flags |= HHT_ONDIVIDER;
			}
			else
			{
				rc = _rcItemEntire;
				rc.left = rc.right - m_nDividerHalfExtent;
				if( rc.PtInRect( pHDHTI->pt ) )
				{
					nWalkWay = 1;
					pHDHTI->flags |= HHT_ONDIVIDER;
				}
			}
			INT nTestColNo = VisualIndex2ColNo( ColNo2VisualIndex( nColNo ) + nWalkWay );
			if( nTestColNo >= 0 )
			{
				CRect rcTest;
				if(		GetItemRect( nTestColNo, &rcTest )
					&&	rcTest.Width() == 0
					)
					pHDHTI->flags |= HHT_ONDIVOPEN;
			}
			break;
		} // for( nColNo = 0; nColNo < nColCount; nColNo++ )
		return pHDHTI->iItem;
	}
	//break;
	} // switch( message )

	return _DoInvokeParentWindowProc( message, wParam, lParam );
}

LRESULT CExtHeaderCtrl::_DoInvokeParentWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
//	__try
//	{
		return CHeaderCtrl::WindowProc( message, wParam, lParam );
//	}
//	__except( EXCEPTION_EXECUTE_HANDLER )
//	{
//	}
//	return 0L;
}

void CExtHeaderCtrl::PreSubclassWindow() 
{
	m_arrExtendedData.RemoveAll();
	m_bCancelingActions = false;
	m_ptScreenPressing.x = m_ptScreenPressing.y = m_nHelperInitialResizingExtent = -32767;
	m_nHoverColNo = m_nHoverIndex = m_nPressingColNo = m_nPressingIndex = -1;
	m_bOnButton = m_bOnDividerAtRight = false;
	CHeaderCtrl::PreSubclassWindow();
}

void CExtHeaderCtrl::PostNcDestroy() 
{
	m_bCancelingActions = false;
	m_ptScreenPressing.x = m_ptScreenPressing.y = m_nHelperInitialResizingExtent = -32767;
	m_nHoverColNo = m_nHoverIndex = m_nPressingColNo = m_nPressingIndex = -1;
	m_bOnButton = m_bOnDividerAtRight = false;
	CHeaderCtrl::PostNcDestroy();
	m_arrExtendedData.RemoveAll();
}

const CExtHeaderCtrl::EXTENDED_ITEM_DATA & CExtHeaderCtrl::ExtendedItemDataGet( INT nColNo ) const
{
	ASSERT_VALID( this );
	return ( const_cast < CExtHeaderCtrl * > ( this ) ) -> ExtendedItemDataGet( nColNo );
}

CExtHeaderCtrl::EXTENDED_ITEM_DATA & CExtHeaderCtrl::ExtendedItemDataGet( INT nColNo )
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
INT nArraySize = INT( m_arrExtendedData.GetSize() );
INT nItemCount = INT( GetItemCount() );
	for( ; nArraySize < nItemCount; nArraySize ++ )
	{
		static EXTENDED_ITEM_DATA g_eii;
		m_arrExtendedData.InsertAt( nArraySize, g_eii, 1 );
	}
	if( nArraySize > nItemCount )
		m_arrExtendedData.RemoveAt( nArraySize - nItemCount, nArraySize - nItemCount );
EXTENDED_ITEM_DATA & _eii = m_arrExtendedData.ElementAt( nColNo );
// if( _eii.m_iconItem.IsEmpty() )
// {
// HICON hIconTest = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
// _eii.m_iconItem.AssignFromHICON( hIconTest, false );
// _eii.m_iconItem.Scale( CSize(13,13) );
// }
// if( _eii.m_iconButton.IsEmpty() )
// {
// HICON hIconTest = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
// _eii.m_iconButton.AssignFromHICON( hIconTest, false );
// _eii.m_iconButton.Scale( CSize(13,13) );
// }
	return _eii;
}

CExtHeaderCtrl::EXTENDED_ITEM_DATA::EXTENDED_ITEM_DATA()
	: m_nExtentMin( 0 )
	, m_nExtentMax( 32767 )
	, m_bDisableDnd( false )
{
}

CExtHeaderCtrl::EXTENDED_ITEM_DATA::EXTENDED_ITEM_DATA( const CExtHeaderCtrl::EXTENDED_ITEM_DATA & other )
{
	AssignFromOther( other );
}

CExtHeaderCtrl::EXTENDED_ITEM_DATA::~EXTENDED_ITEM_DATA()
{
}

void CExtHeaderCtrl::EXTENDED_ITEM_DATA::AssignFromOther( const CExtHeaderCtrl::EXTENDED_ITEM_DATA & other )
{
	m_iconItem          = other.m_iconItem;
	m_iconButton        = other.m_iconButton;
	m_strTipTextItem    = other.m_strTipTextItem;
	m_strTipTextButton  = other.m_strTipTextButton;
	m_nExtentMin        = other.m_nExtentMin;
	m_nExtentMax        = other.m_nExtentMax;
	m_bDisableDnd       = other.m_bDisableDnd;
}

CExtHeaderCtrl::EXTENDED_ITEM_DATA & CExtHeaderCtrl::EXTENDED_ITEM_DATA::operator = ( const CExtHeaderCtrl::EXTENDED_ITEM_DATA & other )
{
	AssignFromOther( other );
	return ( *this );
}

bool CExtHeaderCtrl::EXTENDED_ITEM_DATA::CanBeResized() const
{
	if( m_nExtentMin < m_nExtentMax )
		return true;
	else
		return false;
}

/////////////////////////////////////////////////////////////////////////////
// CExtListCtrl

CExtListCtrl::CExtListCtrl()
	: m_bSortEnabled( true )
	, m_bSortEnabledMultiple( true )
	, m_bColumnAutoSizingByData( false )
	, m_bColumnAutoSizingByHeader( true )
	, m_bReadOnlyLabels( false )
	, m_bSelectAllOnCtrlA( false )
{
	PmBridge_Install( true );
}

CExtListCtrl::~CExtListCtrl()
{
	PmBridge_Uninstall();
}

void CExtListCtrl::PmBridge_OnPaintManagerChanged(
	CExtPaintManager * pGlobalPM
	)
{
	CExtPmBridge::PmBridge_OnPaintManagerChanged( pGlobalPM );
	if( GetSafeHwnd() == NULL )
		return;
	Invalidate();
}

IMPLEMENT_DYNCREATE( CExtListCtrl, CListCtrl );

BEGIN_MESSAGE_MAP(CExtListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CExtListCtrl)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHeaderItemClick) 
	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, OnHeaderItemDoubleClick) 
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGING()
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteItem)
	ON_NOTIFY_REFLECT(LVN_DELETEALLITEMS, OnDeleteAllItems)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY(HDN_BEGINDRAG, 0, OnBeginDrag)
	ON_NOTIFY(HDN_ENDDRAG, 0, OnEndDrag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtListCtrl message handlers

void CExtListCtrl::OnHeaderItemClick( NMHDR * pNMHDR, LRESULT * pResult ) 
{
	ASSERT_VALID( this );
	(*pResult) = 0;
	if( ! m_bSortEnabled )
		return;
HD_NOTIFY * pHdNotify = (HD_NOTIFY *)pNMHDR;
	if( pHdNotify->iButton == 0 )
	{
		CExtListCtrlDataSortOrder _sortOrder;
		CExtHeaderCtrl & wndHeader = GetHeaderCtrl();
		wndHeader.SortRulesGet( _sortOrder );
		if(		m_bSortEnabledMultiple
			&&	CExtPopupMenuWnd::IsKeyPressed( VK_SHIFT )
			)
		{
			CExtListCtrlDataSortOrder _sortOrderUpdateFrom;
			CExtListCtrlDataSortOrder::COLUMN_INFO _columnInfo( INT(pHdNotify->iItem), true );
			_sortOrderUpdateFrom.m_arrItems.Add( _columnInfo );
			_sortOrder.SetupOrder( _sortOrderUpdateFrom, true );
		}
		else
		{
			if(		_sortOrder.m_arrItems.GetSize() == 1
				&&	_sortOrder.m_arrItems[ 0 ] == INT(pHdNotify->iItem)
				)
				_sortOrder.m_arrItems.ElementAt( 0 ).m_bAscending =
					! _sortOrder.m_arrItems[ 0 ].m_bAscending;
			else
			{
				_sortOrder.m_arrItems.RemoveAll();
				CExtListCtrlDataSortOrder::COLUMN_INFO _columnInfo( INT(pHdNotify->iItem), true );
				_sortOrder.m_arrItems.Add( _columnInfo );
			}
		}
		SortItems( _sortOrder );
		Invalidate();
		wndHeader.SortRulesSet( _sortOrder );
	} // if( pHdNotify->iButton == 0 )
}

void CExtListCtrl::OnHeaderItemDoubleClick( NMHDR * pNMHDR, LRESULT * pResult ) 
{
	ASSERT_VALID( this );
	(*pResult) = 0;
HD_NOTIFY * pHdNotify = (HD_NOTIFY *)pNMHDR;
	if( pHdNotify->iButton == 0 )
	{
		INT nColNo = INT( pHdNotify->iItem );
		ColumnAutoSize(
			nColNo,
			m_bColumnAutoSizingByData,
			m_bColumnAutoSizingByHeader
			);
		return;
	}
}

CExtHeaderCtrl & CExtListCtrl::_OnQueryHeaderCtrl()
{
	ASSERT_VALID( this );
	_Init();
	return m_wndHeaderCtrl;
}

CExtHeaderCtrl & CExtListCtrl::GetHeaderCtrl()
{
	ASSERT_VALID( this );
	return _OnQueryHeaderCtrl();
}

const CExtHeaderCtrl & CExtListCtrl::GetHeaderCtrl() const
{
	ASSERT_VALID( this );
	return ( const_cast < CExtListCtrl * > ( this ) ) -> GetHeaderCtrl();
}

void CExtListCtrl::GetSelectedItemsList(
	CList < INT, INT > & _listSelectedItems, 
	bool bAddToTail // = true
	)
{
	ASSERT_VALID( this );
	_listSelectedItems.RemoveAll();
POSITION posSelItems = GetFirstSelectedItemPosition();
	if( posSelItems == NULL )
		return;
	for( ; posSelItems != NULL ; )
	{
		INT nSelItem = (INT)GetNextSelectedItem( posSelItems );
		if( bAddToTail )
			_listSelectedItems.AddTail( nSelItem );
		else
			_listSelectedItems.AddHead( nSelItem );
	}
}

bool CExtListCtrl::VerifySortOrder(
	const CExtListCtrlDataSortOrder & _sortOrder
	) const
{
	ASSERT_VALID( this );
const CExtHeaderCtrl & wndHeader = GetHeaderCtrl();
	ASSERT( wndHeader.GetSafeHwnd() != NULL );
INT nColCount = INT( wndHeader.GetItemCount() );
INT nSortColNo, nSortColCount = INT(_sortOrder.m_arrItems.GetSize());
	for( nSortColNo = 0; nSortColNo < nSortColCount; nSortColNo ++ )
	{
		const CExtListCtrlDataSortOrder::COLUMN_INFO & _columnInfo = _sortOrder.m_arrItems[ nSortColNo ];
		if( _columnInfo.m_nColNo < 0 )
			return false;
		if( _columnInfo.m_nColNo >= nColCount )
			return false;
	}
	return true;
}

INT CExtListCtrl::CompareItems(
	const CExtListCtrlDataSortOrder & _sortOrder,
	INT nItemIndex1,
	INT nItemIndex2
	) const
{
	ASSERT_VALID( this );
	ASSERT( ! _sortOrder.IsEmpty() );
	if( nItemIndex1 == nItemIndex2 )
		return 0;
INT nSortColNo, nSortColCount = INT(_sortOrder.m_arrItems.GetSize());
	for( nSortColNo = 0; nSortColNo < nSortColCount; nSortColNo ++ )
	{
		const CExtListCtrlDataSortOrder::COLUMN_INFO & _columnInfo = _sortOrder.m_arrItems[ nSortColNo ];
		CExtSafeString strText1 = GetItemText( nItemIndex1, _columnInfo.m_nColNo );
		CExtSafeString strText2 = GetItemText( nItemIndex2, _columnInfo.m_nColNo );
		int nCmpResult = strText1.Compare( strText2 );
		if( nCmpResult == 0 )
			continue;
		return _columnInfo.m_bAscending ? (nCmpResult) : (-nCmpResult);
	}
	return 0;
}

void CExtListCtrl::SwapItems(
	INT nItemIndex1,
	INT nItemIndex2
	)
{
	ASSERT_VALID( this );
	ASSERT( nItemIndex1 != nItemIndex2 );
LV_ITEM _item1, _item2;
	::memset( &_item1, 0, sizeof(LV_ITEM) );
	::memset( &_item2, 0, sizeof(LV_ITEM) );
CExtHeaderCtrl & wndHeader = GetHeaderCtrl();
	ASSERT( wndHeader.GetSafeHwnd() != NULL );
INT nColNo, nColCount = INT( wndHeader.GetItemCount() );
CStringArray arrItemTextValues;
	arrItemTextValues.SetSize( nColCount );
	for( nColNo = 0; nColNo < nColCount; nColNo ++ )
		arrItemTextValues[nColNo] = GetItemText( nItemIndex1, nColNo );
	_item1.mask = LVIF_IMAGE|LVIF_PARAM|LVIF_STATE;
	_item1.iItem = nItemIndex1;
	_item1.iSubItem = 0;
	_item1.stateMask = LVIS_CUT|LVIS_DROPHILITED|LVIS_FOCUSED|LVIS_SELECTED|LVIS_OVERLAYMASK|LVIS_STATEIMAGEMASK;
	_item2 = _item1;
	_item2.iItem = nItemIndex2;
	GetItem( &_item1 );
	GetItem( &_item2 );
	for( nColNo = 0; nColNo < nColCount; nColNo ++ )
		SetItemText( nItemIndex1, nColNo, GetItemText( nItemIndex2, nColNo ) );
	_item2.iItem = nItemIndex1;
	SetItem( &_item2 );
	for( nColNo = 0; nColNo < nColCount; nColNo ++ )
		SetItemText(nItemIndex2, nColNo, arrItemTextValues[nColNo]);
	_item1.iItem = nItemIndex2;
	SetItem( &_item1 );
}

bool CExtListCtrl::SortItems(
	INT nItemIndexLow, //= 0 
	INT nItemIndexHigh // -1
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return false;
CExtHeaderCtrl & wndHeader = GetHeaderCtrl();
	ASSERT( wndHeader.GetSafeHwnd() != NULL );
	return SortItems( wndHeader.m_sortOrder, nItemIndexLow, nItemIndexHigh );
}

bool CExtListCtrl::SortItems(
	const CExtListCtrlDataSortOrder & _sortOrder,
	INT nItemIndexLow, //= 0 
	INT nItemIndexHigh // -1
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return false;
	//ASSERT( m_bSortEnabled );
	if( _sortOrder.IsEmpty() || GetItemCount() == 0 )
		return true;
	if( ! VerifySortOrder( _sortOrder ) )
		return false;
	if( nItemIndexHigh < 0 )
		nItemIndexHigh = GetItemCount() - 1;
INT nReviewIndexLow = nItemIndexLow;
INT nReviewIndexHigh = nItemIndexHigh;
	if( nReviewIndexHigh <= nReviewIndexLow )
		return false;
LONG nViewMin = nItemIndexLow, nViewMax = nItemIndexHigh, nViewMiddle = ( nItemIndexLow + nItemIndexHigh ) / 2L;
	for( ; nViewMin <= nViewMax; )
	{
		for( ; ( nViewMin < nItemIndexHigh ) && ( CompareItems( _sortOrder, nViewMin, nViewMiddle ) < 0 ); ++ nViewMin );
		for( ; ( nViewMax > nItemIndexLow ) && ( CompareItems( _sortOrder, nViewMax, nViewMiddle ) > 0 ); -- nViewMax );
		if( nViewMin <= nViewMax )
		{
			if( nViewMin != nViewMax && CompareItems( _sortOrder, nViewMin, nViewMax ) != 0 )
			{
				if( nViewMiddle == nViewMin )
					nViewMiddle = nViewMax;
				else if( nViewMiddle == nViewMax )
					nViewMiddle = nViewMin;
				SwapItems( nViewMin, nViewMax );
			}
			++ nViewMin;
			-- nViewMax;
		}
	}
	if( nItemIndexLow < nViewMax )
	{
		if( ! SortItems( _sortOrder, nItemIndexLow, nViewMax ) )
		{
			ASSERT( FALSE );
			return false;
		}
	}
	if( nViewMin < nItemIndexHigh )
	{
		if( ! SortItems( _sortOrder, nViewMin, nItemIndexHigh ) )
		{
			ASSERT( FALSE );
			return false;
		}
	}
	return true;
}

void CExtListCtrl::StateSerialize(
	CArchive & ar,
	bool bExtents, // = true
	bool bOrder, // = true
	bool bSortOrder, // = true
	bool bListViewMode // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
DWORD dwSerializedDataMask = 0;
	if( bExtents )
		dwSerializedDataMask |= ( 1 << 0 );
	if( bOrder )
		dwSerializedDataMask |= ( 1 << 1 );
	if( bSortOrder )
		dwSerializedDataMask |= ( 1 << 2 );
	if( bListViewMode )
		dwSerializedDataMask |= ( 1 << 3 );
	if( dwSerializedDataMask == 0 )
		::AfxThrowUserException();
CExtHeaderCtrl & wndHeader = GetHeaderCtrl();
	ASSERT( wndHeader.GetSafeHwnd() != NULL );
INT nColNo, nColCount = INT( wndHeader.GetItemCount() );
DWORD dwGap = 0;
INT * pArr = NULL;
	try
	{
		if( ar.IsStoring() )
		{
			ar << dwGap;
			ar << dwSerializedDataMask;
			ar << DWORD( nColCount );
			if( bExtents )
			{
				for( nColNo = 0; nColNo < nColCount; nColNo ++ )
				{
					const CExtHeaderCtrl::EXTENDED_ITEM_DATA & _eii = wndHeader.ExtendedItemDataGet( nColNo );
					DWORD dwExtent = (DWORD)ColumnExtentGet( nColNo );
					ar << dwExtent;
					ar << DWORD(_eii.m_nExtentMin);
					ar << DWORD(_eii.m_nExtentMax);
				} // for( nColNo = 0; nColNo < nColCount; nColNo ++ )
			} // if( bExtents )
			if( bOrder )
			{
				try
				{
					pArr = new INT[ nColCount ];
					if( ! wndHeader.GetOrderArray( pArr, nColCount ) )
						::AfxThrowUserException();
					for( nColNo = 0; nColNo < nColCount; nColNo ++ )
						ar << DWORD( pArr[nColNo] );
				}
				catch( CException * pException )
				{
					pException->Delete();
				}
			} // if( bOrder )
			if( bListViewMode )
			{
				DWORD dwMode = (DWORD)( GetStyle() & LVS_TYPEMASK );
				ar << dwMode;
			}
		} // if( ar.IsStoring() )
		else
		{
			ar >> dwGap;
			DWORD dwSerializedDataMask2 = 0;
			ar >> dwSerializedDataMask2;
			if( dwSerializedDataMask2 == 0 || dwSerializedDataMask2 != dwSerializedDataMask )
				::AfxThrowUserException();
			DWORD dwColCountToLoad;
			ar >> dwColCountToLoad;
			if( INT(dwColCountToLoad) != nColCount )
				::AfxThrowArchiveException( CArchiveException::badSchema );
			if( bExtents )
			{
				for( nColNo = 0; nColNo < nColCount; nColNo ++ )
				{
					CExtHeaderCtrl::EXTENDED_ITEM_DATA & _eii = wndHeader.ExtendedItemDataGet( nColNo );
					DWORD dwExtent;
					ar >> dwExtent;
					ColumnExtentSet( nColNo, INT(dwExtent) );
					ar >> dwExtent;
					_eii.m_nExtentMin = INT(dwExtent);
					ar >> dwExtent;
					_eii.m_nExtentMax = INT(dwExtent);
				} // for( nColNo = 0; nColNo < nColCount; nColNo ++ )
			} // if( bExtents )
			if( bOrder )
			{
				try
				{
					pArr = new INT[ nColCount ];
					for( nColNo = 0; nColNo < nColCount; nColNo ++ )
					{
						DWORD dwTmp;
						ar >> dwTmp;
						pArr[nColNo] = INT(dwTmp);
					} // for( nColNo = 0; nColNo < nColCount; nColNo ++ )
					if( ! wndHeader.SetOrderArray( nColCount, pArr ) )
						::AfxThrowUserException();
				}
				catch( CException * pException )
				{
					pException->Delete();
				}
			} // if( bOrder )
			if( bListViewMode )
			{
				DWORD dwMode = 0;
				ar >> dwMode;
				if( ( dwMode & LVS_TYPEMASK ) != dwMode )
					::AfxThrowUserException();
				ModifyStyle( LVS_TYPEMASK, 0 );
				ModifyStyle( 0, dwMode );
			}
		} // else from if( ar.IsStoring() )

		if( bSortOrder )
		{
			wndHeader.m_sortOrder.Serialize( ar );
			if(		ar.IsLoading()
				&&	(! wndHeader.m_sortOrder.IsEmpty() )
				&&	VerifySortOrder( wndHeader.m_sortOrder )
				)
				SortItems( wndHeader.m_sortOrder );
		} // if( bSortOrder )

	} // try
	catch( ... )
	{
		if( pArr != NULL )
			delete [] pArr;
		throw;
	} // catch( ... )
	if( pArr != NULL )
		delete [] pArr;
}

static CExtSafeString productsection2regkeypath(
	__EXT_MFC_SAFE_LPCTSTR sCppClassName,
	__EXT_MFC_SAFE_LPCTSTR sProfileName,
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	)
{
	return CExtCmdManager::GetSubSystemRegKeyPath(
		sCppClassName,
		sProfileName,
		sSectionNameCompany,
		sSectionNameProduct
		);
}

bool CExtListCtrl::StateLoad(
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%
	HKEY hKeyRoot, // = HKEY_CURRENT_USER
	bool bExtents, // = true
	bool bOrder, // = true
	bool bSortOrder, // = true
	bool bListViewMode // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
CExtSafeString sRegKeyPath=
		productsection2regkeypath(
			_T("CExtListCtrl"),
			sSectionNameProfile,
			sSectionNameCompany,
			sSectionNameProduct
			);
	try
	{
		CMemFile _file;
		if( ! CExtCmdManager::FileObjFromRegistry( _file, sRegKeyPath, hKeyRoot, false ) )
			return false;
		CArchive ar( &_file, CArchive::load );
		StateSerialize( ar, bExtents, bOrder, bSortOrder, bListViewMode );
		return true;
	}
	catch( CException * pXept )
	{
		pXept->Delete();
	}
	catch( ... )
	{
	}
	return false;
}

bool CExtListCtrl::StateSave(
	__EXT_MFC_SAFE_LPCTSTR sSectionNameCompany, // under HKEY_CURRENT_USER\Software
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProduct, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%
	__EXT_MFC_SAFE_LPCTSTR sSectionNameProfile, // under HKEY_CURRENT_USER\Software\%sSectionNameCompany%\%sSectionNameProfile%
	HKEY hKeyRoot, // = HKEY_CURRENT_USER
	bool bExtents, // = true
	bool bOrder, // = true
	bool bSortOrder, // = true
	bool bListViewMode // = true
	)
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
CExtSafeString sRegKeyPath =
		productsection2regkeypath(
			_T("CExtListCtrl"),
			sSectionNameProfile,
			sSectionNameCompany,
			sSectionNameProduct
			);
	try
	{
		CMemFile _file;
		CArchive ar( &_file, CArchive::store );
		StateSerialize( ar, bExtents, bOrder, bSortOrder, bListViewMode );
		ar.Flush();
		ar.Close();
		if(	! CExtCmdManager::FileObjToRegistry( _file, sRegKeyPath, hKeyRoot, false ) )
			return false;
		return true;
	}
	catch( CException * pXept )
	{
		pXept->Delete();
	}
	catch( ... )
	{
	}
	return false;
}

INT CExtListCtrl::ColumnExtentGet( INT nColNo ) const
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
const CExtHeaderCtrl & wndHeader = GetHeaderCtrl();
	ASSERT( wndHeader.GetSafeHwnd() != NULL );
//#ifdef _DEBUG
//INT nColCount = INT( wndHeader.GetItemCount() );
//	ASSERT( 0 <= nColNo && nColNo < nColCount );
//#endif // _DEBUG
INT nColCount = INT( wndHeader.GetItemCount() );
	if( ! ( 0 <= nColNo && nColNo < nColCount ) )
		return 120; // some default width
INT nExtent = CListCtrl::GetColumnWidth( nColNo );
	return nExtent;
}

void CExtListCtrl::ColumnExtentSet( INT nColNo, INT nExtent )
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
CExtHeaderCtrl & wndHeader = GetHeaderCtrl();
	ASSERT( wndHeader.GetSafeHwnd() != NULL );
//#ifdef _DEBUG
//INT nColCount = INT( wndHeader.GetItemCount() );
//	ASSERT( 0 <= nColNo && nColNo < nColCount );
//#endif // _DEBUG
INT nColCount = INT( wndHeader.GetItemCount() );
	if( ! ( 0 <= nColNo && nColNo < nColCount ) )
		return;
	CListCtrl::SetColumnWidth( nColNo, nExtent );
}

void CExtListCtrl::ColumnAutoSize(
	INT nColNo,
	bool bAutoSizeByData, // = true,
	bool bAutoSizeByHeader // = true
	)
{
	ASSERT_VALID( this );
	if( ! ( bAutoSizeByData || bAutoSizeByHeader ) )
		return;
	if( GetSafeHwnd() == NULL )
		return;
CExtHeaderCtrl & wndHeader = GetHeaderCtrl();
	ASSERT( wndHeader.GetSafeHwnd() != NULL );
#ifdef _DEBUG
INT nColCount = INT( wndHeader.GetItemCount() );
	ASSERT( 0 <= nColNo && nColNo < nColCount );
#endif // _DEBUG
	SetRedraw( FALSE );
INT nInitialWidth = CListCtrl::GetColumnWidth( nColNo );
INT nResultWidth = 0;
	if( bAutoSizeByHeader )
	{
		//CListCtrl::SetColumnWidth( nColNo, LVSCW_AUTOSIZE_USEHEADER );
		//INT nWidth = CListCtrl::GetColumnWidth( nColNo );
		INT nWidth = wndHeader.OnCalcHeaderItemSize( nColNo ).cx;
		nResultWidth = max( nResultWidth, nWidth );
	}
	if( bAutoSizeByData )
	{
		CListCtrl::SetColumnWidth( nColNo, LVSCW_AUTOSIZE );
		INT nWidth = CListCtrl::GetColumnWidth( nColNo ); 
		nResultWidth = max( nResultWidth, nWidth );
	}
	if( nInitialWidth != nResultWidth )
		CListCtrl::SetColumnWidth( nColNo, nResultWidth );
	SetRedraw( TRUE );
	if( nInitialWidth != nResultWidth )
		Invalidate();
}

void CExtListCtrl::OnDestroy() 
{
	CListCtrl::OnDestroy();
}

void CExtListCtrl::ModifyExtendedStyle( DWORD dwRemove, DWORD dwAdd )
{
LPARAM nStyleOld = (LPARAM) ::SendMessage( m_hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0 );
LPARAM nStyleNew = nStyleOld;
	nStyleNew &= ~( (LPARAM) dwRemove );
	nStyleNew |= (LPARAM) dwAdd;
	if( nStyleNew != nStyleOld )
		::SendMessage( m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, nStyleNew );
}

int CExtListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if( CListCtrl::OnCreate( lpCreateStruct ) == -1 )
		return -1;
	//return _Init() ? 0 : -1;
	PostMessage( (WM_USER+123) );
	return 0;
}

void CExtListCtrl::PreSubclassWindow() 
{
	CListCtrl::PreSubclassWindow();
	PostMessage( (WM_USER+123) );
	//_Init();
}

LRESULT CExtListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
HWND hWndOwn = GetSafeHwnd();
	switch( message )
	{
	case (WM_USER+123):
		_Init();
	break;
	case WM_ERASEBKGND:
		return FALSE;
	case WM_PAINT:
	{
		CPaintDC dcPaint( this );
		CExtPaintManager::stat_ExcludeChildAreas(
			dcPaint.GetSafeHdc(),
			GetSafeHwnd()
			);
		CRect rcClient;
		GetClientRect( &rcClient );
		CExtMemoryDC dc( &dcPaint, &rcClient );
		DefWindowProc(
			WM_ERASEBKGND,
			WPARAM( dc.GetSafeHdc() ),
			LPARAM(0)
			);
		DefWindowProc(
			WM_PAINT,
			WPARAM( dc.GetSafeHdc() ),
			LPARAM(0)
			);
	}
	return TRUE;
	case WM_KEYDOWN:
	{
		switch( wParam )
		{
		case VK_F2:
			if(		( GetStyle() & LVS_EDITLABELS ) != 0
				&&	( ! ReadOnlyLabelsGet() )
				)
			{
				int nItemIndex = FocusedItemGet();
				if( nItemIndex >= 0 )
					EditLabel( nItemIndex );
				return 0L;
			}
		break;
		case VK_APPS:
		{
			CRect rcItem;
			int nItemFocused = FocusedItemGet();
			if( nItemFocused < 0 )
				return 0L;
			EnsureVisible( nItemFocused, TRUE );
			GetItemRect( nItemFocused, &rcItem, TRUE );
			CPoint ptScreen = rcItem.CenterPoint();
			ClientToScreen( &ptScreen );
			SendMessage( WM_CONTEXTMENU, WPARAM(m_hWnd), MAKELPARAM(ptScreen.x, ptScreen.y) );
		}
		break;
		case VK_TAB:
			if( ( GetStyle() & WS_TABSTOP ) != 0 )
			{
				GetParent()->SendMessage(
					WM_NEXTDLGCTL,
					CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT) ? 1 : 0,
					0
					);
				return 0L;
			} // if( ( GetStyle() & WS_TABSTOP ) != 0 )
		break;
		case WPARAM('A'):
			if( m_bSelectAllOnCtrlA && ( GetStyle() & LVS_SINGLESEL ) == 0 )
			{
				BOOL bCtrl = ::GetKeyState(VK_CONTROL) & 0x80000000;
				BOOL bShift = ::GetKeyState(VK_SHIFT) & 0x80000000;
				BOOL bAlt = HIWORD(lParam) & KF_ALTDOWN;
				if( bCtrl && (!bShift) && (!bAlt) )
				{ // if Ctrl+A pressed
					CList < INT, INT > _listSelectedItems;
					INT nIndex, nCount = INT( GetItemCount() );
					for( nIndex = 0; nIndex < nCount; nIndex ++ )
						_listSelectedItems.AddTail( nIndex );
					SelectedItemsListSet( _listSelectedItems );
					return 0L;
				} // if Ctrl+A pressed
			} // if( m_bSelectAllOnCtrlA && ( GetStyle() & LVS_SINGLESEL ) == 0 )
		break;
		} // switch( wParam )
	}
	break;
	default:
		if( message == CExtHeaderCtrl::g_nMsgHeaderButtonClick )
			return GetParent()->SendMessage( CExtHeaderCtrl::g_nMsgHeaderButtonClick, wParam, lParam );
	break;
	} // switch( message )
LRESULT lResult = CListCtrl::WindowProc(message , wParam, lParam );
	switch( message )
	{
	case WM_GETDLGCODE:
		lResult |= DLGC_WANTMESSAGE;
	break;
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_RETURN:
		case VK_ESCAPE:
			if(		(!(	CExtPopupMenuWnd::IsKeyPressed( VK_MENU )
					||	CExtPopupMenuWnd::IsKeyPressed( VK_CONTROL )
					||	CExtPopupMenuWnd::IsKeyPressed( VK_SHIFT )
					))
				&&	::IsWindow( hWndOwn )
				&&	IsWindowEnabled()
				&&	( GetStyle() & WS_VISIBLE ) != 0
				)
			{
				UINT nCmdID = ( wParam == VK_RETURN ) ? IDOK : IDCANCEL;
				bool bSendOkCancel = true;
				CWnd * pWndParent = GetParent();
				for(	;	pWndParent != NULL
						&&	( pWndParent->GetStyle() & WS_CHILD ) != 0
						;
					)
				{
					if(		( ! pWndParent->IsWindowEnabled() )
						||	( pWndParent->GetStyle() & WS_VISIBLE ) == 0
						)
					{
						bSendOkCancel = false;
						break;
					}
					pWndParent = pWndParent->GetParent();
				}
				if( bSendOkCancel && pWndParent != NULL )
				{
					CWnd * pWndOkCancel = pWndParent->GetDlgItem( nCmdID );
					if(		pWndOkCancel != NULL
						&&	(	(! pWndOkCancel->IsWindowEnabled() )
							||	( pWndOkCancel->GetStyle() & WS_VISIBLE ) == 0
							)
						)
						bSendOkCancel = false;
					else if( ( pWndParent->GetStyle() & WS_CHILD ) == 0 )
					{
						MENUITEMINFO _mii;
						::memset( &_mii, 0, sizeof(MENUITEMINFO) );
						_mii.cbSize = sizeof(MENUITEMINFO);
						_mii.fMask = MIIM_STATE;
						CMenu * pMenu = pWndParent->GetSystemMenu( FALSE );
						if(		pMenu->GetSafeHmenu() != NULL
							&&	pMenu->GetMenuItemInfo( SC_CLOSE, &_mii, FALSE )
							&&	( _mii.fState & ( MF_DISABLED | MF_GRAYED ) ) != 0
							)
							bSendOkCancel = false;
					} // else if( ( pWndParent->GetStyle() & WS_CHILD ) == 0 )
				} // if( bSendOkCancel && pWndParent != NULL )
				if( bSendOkCancel )
					pWndParent->SendMessage( WM_COMMAND, WPARAM(nCmdID) );
			}
		break;
		} // switch( wParam )
	break;
	case WM_HSCROLL:
		{
			CExtHeaderCtrl & wndHeader = GetHeaderCtrl();
			if(		wndHeader.GetSafeHwnd() != NULL
				&&	( wndHeader.GetStyle() & WS_VISIBLE ) != 0
				)
			{
				wndHeader.Invalidate();
				wndHeader.UpdateWindow();
			}
		}
	break;
	} // switch( message )
	return lResult;
}

bool CExtListCtrl::_Init()
{
CExtHeaderCtrl & wndHeader = m_wndHeaderCtrl; // GetHeaderCtrl();
	if( wndHeader.GetSafeHwnd() != NULL )
		return true;
	if( ! wndHeader.SubclassWindow( GetDlgItem(0)->GetSafeHwnd()) )
		return false;
	wndHeader.Invalidate();
	return true;
}

void CExtListCtrl::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	CListCtrl::OnWindowPosChanging(lpwndpos);
	Invalidate(FALSE);
}

void CExtListCtrl::OnDeleteItem( NMHDR * pNMHDR, LRESULT * pResult ) 
{
	pNMHDR;
//NM_LISTVIEW * pNMListView = (NM_LISTVIEW *)pNMHDR;
	(*pResult) = 0;
}

void CExtListCtrl::OnDeleteAllItems( NMHDR * pNMHDR, LRESULT * pResult ) 
{
	pNMHDR;
//NM_LISTVIEW * pNMListView = (NM_LISTVIEW *)pNMHDR;
	(*pResult) = 0;
}

bool CExtListCtrl::ReadOnlyLabelsGet() const
{
	ASSERT_VALID( this );
	return m_bReadOnlyLabels;
}

void CExtListCtrl::ReadOnlyLabelsSet( bool bReadOnlyLabels )
{
	ASSERT_VALID( this );
	m_bReadOnlyLabels = bReadOnlyLabels;
}

void CExtListCtrl::OnBeginLabelEdit( NMHDR * pNMHDR, LRESULT * pResult )
{
	pNMHDR;
//NMLVDISPINFO * pNMDI = (NMLVDISPINFO*)pNMHDR;
	(*pResult) = 0;
	if( ReadOnlyLabelsGet() )
	{
		CEdit * pEdit = GetEditControl();
		if( pEdit->GetSafeHwnd() != NULL )
			pEdit->SetReadOnly( TRUE );
	}
}

void CExtListCtrl::SelectedItemsListGet(
	CList < INT, INT > & _listSelectedItems,
	bool bAddToTail // = true
	) const
{
	ASSERT_VALID( this );
	_listSelectedItems.RemoveAll();
	if( GetSafeHwnd() == NULL )
		return;
POSITION pos = GetFirstSelectedItemPosition();
	for( ; pos != NULL; )
	{
		INT nItemIndex = GetNextSelectedItem( pos );
		if( bAddToTail )
			_listSelectedItems.AddTail( nItemIndex );
		else
			_listSelectedItems.AddHead( nItemIndex );
	}
}

void CExtListCtrl::SelectedItemsListSet(
	const CList < INT, INT > & _listSelectedItems
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
INT nItemIndex, nItemCount = GetItemCount();
	if( nItemCount <= 0 )
		return;
CList < INT, INT > _listSelectedItems2;
	_listSelectedItems2.AddTail( const_cast < CList < INT, INT > * > ( &_listSelectedItems ) );
	for( nItemIndex = 0; nItemIndex < nItemCount; nItemIndex ++ )
	{
		POSITION pos = _listSelectedItems2.Find( nItemIndex );
		SetItemState( nItemIndex, ( pos != NULL ) ? LVIS_SELECTED : 0, LVIS_SELECTED );
		if( pos != NULL )
			_listSelectedItems2.RemoveAt( pos );
	}
}

bool CExtListCtrl::IsItemSelected(
	INT nItemIndex
	) const
{
	ASSERT_VALID( this );
	if( nItemIndex < 0 )
		return false;
	if( GetSafeHwnd() == NULL )
		return false;
INT nItemCount = INT( GetItemCount() );
	if( nItemIndex >= nItemCount )
		return false;
bool bSelected = ( GetItemState( nItemIndex, LVIS_SELECTED ) != 0 ) ? true : false;
	return bSelected;
}

void CExtListCtrl::SelectItem(
	INT nItemIndex,
	bool bSelect // = true
	)
{
	ASSERT_VALID( this );
	if( nItemIndex < 0 )
		return;
	if( GetSafeHwnd() == NULL )
		return;
INT nItemCount = INT( GetItemCount() );
	if( nItemIndex >= nItemCount )
		return;
bool bSelected = ( GetItemState( nItemIndex, LVIS_SELECTED ) != 0 ) ? true : false;
	if( ( bSelected && bSelect ) || ( (!bSelected) && (!bSelect) ) )
		return;
	SetItemState( nItemIndex, bSelect ? LVIS_SELECTED : 0, LVIS_SELECTED );
}

INT CExtListCtrl::FocusedItemGet() const
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return -1;
INT nItemIndex = GetNextItem( -1, LVNI_FOCUSED );
	return nItemIndex;
}

INT CExtListCtrl::FocusedItemSet( // returns previous focused item index
	INT nItemIndex,
	bool bSelectFocusedItem, // = true
	bool bUnselectOtherItems // = true
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return -1;
INT nPrevFocusedItemIndex = GetNextItem( -1, LVNI_FOCUSED );
	if( nItemIndex < 0 )
		return nPrevFocusedItemIndex;
INT nItemCount = INT( GetItemCount() );
	if( nItemIndex >= nItemCount )
		return nPrevFocusedItemIndex;
	if(		nPrevFocusedItemIndex == nItemIndex
		&&	( ! ( bSelectFocusedItem || bUnselectOtherItems ) )
		)
		return nPrevFocusedItemIndex;
	if( bUnselectOtherItems )
	{
		INT nWalkIndex;
		for( nWalkIndex = 0; nWalkIndex < nItemCount; nWalkIndex ++ )
		{
			if( bSelectFocusedItem && nWalkIndex == nItemIndex )
				SetItemState( nWalkIndex, LVIS_SELECTED, LVIS_SELECTED );
			else
				SetItemState( nWalkIndex, 0, LVIS_SELECTED );
		}
	}
	else if( bSelectFocusedItem )
				SetItemState( nItemIndex, LVIS_SELECTED, LVIS_SELECTED );
	SetItemState( nItemIndex, LVIS_FOCUSED, LVIS_FOCUSED );
	return nPrevFocusedItemIndex;
}

void CExtListCtrl::OnNotifyHdnTrack( CExtHeaderCtrl & wndHeader, HD_NOTIFY & _data )
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT_VALID( (&wndHeader) );
	ASSERT( wndHeader.GetSafeHwnd() != NULL );
	wndHeader;
	_data;
}

void CExtListCtrl::OnBeginDrag( NMHDR * pNMHDR, LRESULT * pResult ) 
{
	ASSERT_VALID( this );
	pNMHDR;
//HD_NOTIFY * phdn = (HD_NOTIFY *) pNMHDR;
	*pResult = 0;
}

void CExtListCtrl::OnEndDrag( NMHDR * pNMHDR, LRESULT * pResult ) 
{
	ASSERT_VALID( this );
	pNMHDR;
//HD_NOTIFY * phdn = (HD_NOTIFY *) pNMHDR;
	*pResult = 0;
	Invalidate();
	UpdateWindow();
}

CExtNCSB < CExtListCtrl > :: CExtNCSB(
	bool bNcsbDelayedInitialization, // = false
	bool bNcsbForceMiddleContainerMode // = false
	)
	: CExtNCSB_Impl < CExtListCtrl > (
		bNcsbDelayedInitialization,
		bNcsbForceMiddleContainerMode
		)
{
}

CExtNCSB < CExtListCtrl > :: ~CExtNCSB()
{
}

void CExtNCSB < CExtListCtrl > :: AdjustScrollMetrics()
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
DWORD dwType = DWORD( GetStyle() & LVS_TYPEMASK );
CExtNCSB_ScrollContainer * pWndH = NCSB_GetContainer( CExtNCSB_ScrollContainer::__EM_HORIZONTAL_SCROLL_BAR );
CExtNCSB_ScrollContainer * pWndV = NCSB_GetContainer( CExtNCSB_ScrollContainer::__EM_VERTICAL_SCROLL_BAR );
CExtScrollBar * pSBH = ( pWndH != NULL ) ? pWndH->GetScrollBarInContainer() : NULL;
CExtScrollBar * pSBV = ( pWndV != NULL ) ? pWndV->GetScrollBarInContainer() : NULL;
INT m_nStepSizeH = -1, m_nStepSizeV = -1;
	switch( dwType )
	{
	case LVS_ICON:
		m_nStepSizeV = 64;
	break;
	case LVS_SMALLICON:
	case LVS_LIST:
		m_nStepSizeV = 16;
	break;
	case LVS_REPORT:
		m_nStepSizeV = 1;
	break;
	} // switch( dwType )
	if( m_nStepSizeH > 0 && pSBH != NULL )
		pSBH->m_nStepSize = m_nStepSizeH;
	if( m_nStepSizeV > 0 && pSBV != NULL )
		pSBV->m_nStepSize = m_nStepSizeV;
}

LRESULT CExtNCSB < CExtListCtrl > :: WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_HSCROLL:
	{
		UINT nSBCode = UINT(LOWORD(DWORD(wParam))), nPos = UINT(HIWORD(DWORD(wParam)));
		//TRACE2( "WM_HSCROLL, nSBCode = %d, nPos = %d\r\n", nSBCode, nPos );
		INT nItemExtent = 1;
		DWORD dwStyle = GetStyle();
		DWORD dwListCtrlType = dwStyle&LVS_TYPEMASK;
		switch( dwListCtrlType )
		{
		case LVS_ICON:
		case LVS_SMALLICON:
		case LVS_REPORT:
		break;
		case LVS_LIST:
		{
			CRect rcItem( 0, 0, 0, 0 );
			INT nTopIndex = GetTopIndex();
			if(		nTopIndex >= 0
				&&	CExtListCtrl::GetItemRect( nTopIndex, &rcItem, LVIR_BOUNDS )
				)
				nItemExtent = rcItem.Width();
		}
		break;
		} // switch( dwListCtrlType )
		if( nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION )
		{
			INT nPosOld = GetScrollPos( SB_HORZ );
			INT nPosShift = nPos*nItemExtent - nPosOld*nItemExtent;
			Scroll( CSize( nPosShift, 0 ) );
			return 0L;
		}
		if(		nSBCode == SB_LINELEFT
			||	nSBCode == SB_LINERIGHT
			||	nSBCode == SB_PAGELEFT
			||	nSBCode == SB_PAGERIGHT
			||	nSBCode == SB_LEFT
			||	nSBCode == SB_RIGHT
			||	nSBCode == SB_ENDSCROLL
			)
			return 0L;
	} // case WM_HSCROLL:
	break; // case WM_VSCROLL
	case WM_VSCROLL:
	{
		UINT nSBCode = UINT(LOWORD(DWORD(wParam))), nPos = UINT(HIWORD(DWORD(wParam)));
		//TRACE2( "WM_VSCROLL, nSBCode = %d, nPos = %d\r\n", nSBCode, nPos );
		INT nItemExtent = 1;
		DWORD dwStyle = GetStyle();
		DWORD dwListCtrlType = dwStyle&LVS_TYPEMASK;
		switch( dwListCtrlType )
		{
		case LVS_ICON:
		case LVS_SMALLICON:
		case LVS_LIST:
		break;
		case LVS_REPORT:
		{
			CRect rcItem( 0, 0, 0, 0 );
			INT nTopIndex = GetTopIndex();
			if(		nTopIndex >= 0
				&&	CExtListCtrl::GetItemRect( nTopIndex, &rcItem, LVIR_BOUNDS )
				)
				nItemExtent = rcItem.Height();
			else
			{
				TEXTMETRIC _tm;
				::memset( &_tm, 0, sizeof(TEXTMETRIC) );
				CClientDC dc( this );
				CFont * pFont = GetFont();
				int nSave = dc.SaveDC();
				dc.SelectObject( pFont );
				dc.GetTextMetrics( &_tm );
				nItemExtent = _tm.tmHeight + _tm.tmExternalLeading + 1;
				dc.RestoreDC( nSave );
			}
		}
		break;
		} // switch( dwListCtrlType )
		if( nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION )
		{
			INT nPosOld = GetScrollPos( SB_VERT );
			INT nPosShift = nPos*nItemExtent - nPosOld*nItemExtent;
			Scroll( CSize( 0, nPosShift ) );
			return 0L;
		}
		if(		nSBCode == SB_LINEUP
			||	nSBCode == SB_LINEDOWN
			||	nSBCode == SB_PAGEUP
			||	nSBCode == SB_PAGEDOWN
			||	nSBCode == SB_TOP
			||	nSBCode == SB_BOTTOM
			||	nSBCode == SB_ENDSCROLL
			)
			return 0L;
	}
	break; // case WM_VSCROLL
	} // switch( message )
LRESULT lResult = CExtNCSB_Impl < CExtListCtrl > :: WindowProc( message, wParam, lParam );
	switch( message )
	{
	case WM_STYLECHANGED:
		if( wParam == GWL_STYLE )
		{
			STYLESTRUCT * ptr = (STYLESTRUCT *)lParam;
			ASSERT( ptr != NULL );
			DWORD dwOldType = DWORD( ptr->styleOld & LVS_TYPEMASK );
			DWORD dwNewType = DWORD( ptr->styleNew & LVS_TYPEMASK );
			if( dwOldType != dwNewType )
				AdjustScrollMetrics();
		} // if( wParam == GWL_STYLE )
	break; // case WM_STYLECHANGED
	} // switch( message )
	return lResult;
}

#endif //  ( ! defined __EXT_MFC_NO_LIST_VIEW_CTRL )

#if ( ! defined __EXT_MFC_NO_TREE_VIEW_CTRL )

/////////////////////////////////////////////////////////////////////////////
// CExtTreeCtrl

IMPLEMENT_DYNCREATE( CExtTreeCtrl, CTreeCtrl )

const UINT CExtTreeCtrl::g_nMsgTreeItemDelayedFocus = ::RegisterWindowMessage( _T("CExtTreeCtrl::g_nMsgTreeItemDelayedFocus") );

CExtTreeCtrl::CExtTreeCtrl()
	: m_eOS( CExtTreeCtrl::line_style_dots )
	, m_eTBS( CExtTreeCtrl::tree_box_style_square )
	, m_clrExpandCollapseButtonSign( COLORREF(-1L) )
	, m_clrExpandCollapseButtonFill( COLORREF(-1L) )
	, m_clrLines( COLORREF(-1L) )
	, m_clrTreeBkColor(	COLORREF(-1L) )
	, m_clrProgressBackgroundColor(	COLORREF(-1L) )
	, m_clrProgressBorder( COLORREF(-1L) )
	, m_clrProgressBoxColor( COLORREF(-1L) )
	, m_dwTreeStyle( __EXT_TREE_DEFAULT_STYLES )
	, m_dwTreeStyleEx( __EXT_TREE_EX_DEFAULT_STYLES )
	, m_htiAnchorItem( NULL )
	, m_htiInsertMark( NULL )
	, m_bInsertMarkAfter( false )
	, m_hWndChildControl( NULL )
	, m_nMarginCheckBoxLeft( 1 )
	, m_nMarginCheckBoxRight( 1 )
	, m_nMarginIconLeft( 1 )
	, m_nMarginIconRight( 1 )
	, m_nMarginLabelLeft( 1 )
	, m_nMarginLabelRight( 1 )
	, m_nMarginControlLabelLeft( 1 )
	, m_nMarginControlLabelRight( 1 )
	, m_nPaddingLabelLeft( 3 )
	, m_nPaddingLabelRight( 3 )
	, m_htiHover( NULL )
	, m_dwLastHoverHitTestFlags( 0 )
	, m_hCursorHand( NULL )
	, m_htiInplaceEdited( NULL )
	, m_bCanceling( false )
// 	, m_bSynchronizingBarH( false )
// 	, m_bSynchronizingBarV( false )
	, m_nDelayedFocusTimerID( 666 )
	, m_nDelayedFocusTime( 500 )
	, m_nDelayedEditingTimerID( 667 )
	, m_htiDelayedFocus( NULL )
	, m_bReadOnlyLabels( false )
	, m_dwCheckBoxAreaFlags( __EXT_TVHT_ONCHECKBOX )
{
	::memset( &m_nLastMouseButtonEventType, 0, sizeof(m_nLastMouseButtonEventType) );
	if( g_PaintManager.m_bIsWin2000orLater )
		m_hCursorHand = ::LoadCursor( NULL, MAKEINTRESOURCE(32649) );
	if( m_hCursorHand == NULL )
	{
		HRSRC hRSRC = NULL;
		HINSTANCE hInst = g_ResourceManager->FindResourceHandle( RT_GROUP_CURSOR, IDC_EXT_HAND, NULL, &hRSRC );
		if( hInst != NULL)
			m_hCursorHand = ::LoadCursor( hInst, MAKEINTRESOURCE(IDC_EXT_HAND) );
	}
}

CExtTreeCtrl::~CExtTreeCtrl()
{
	_UnregisterItemsFromMap();
	TreeIconRemoveAll();
}

BEGIN_MESSAGE_MAP( CExtTreeCtrl, CTreeCtrl )
	//{{AFX_MSG_MAP(CExtTreeCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_MESSAGE( WM_SETFONT, OnSetFont )
	ON_MESSAGE( WM_SETTINGCHANGE, OnSettingChange )
	ON_MESSAGE( WM_STYLECHANGING, OnStyleChanging )
	ON_MESSAGE( WM_STYLECHANGED, OnStyleChanged )
	ON_WM_SETCURSOR()
	ON_WM_ACTIVATEAPP()
	ON_WM_CANCELMODE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_GETDLGCODE()
	ON_WM_TIMER()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(
		CExtContentExpandWnd::g_nMsgPaintItemContent,
		_OnPaintExpanedItemContent
		)
END_MESSAGE_MAP()

BOOL CExtTreeCtrl::OnEraseBkgnd( CDC* pDC )
{
	pDC;
	return TRUE;
}

void CExtTreeCtrl::OnPaint()
{
CPaintDC dcPaint(this);
CExtMemoryDC dc( &dcPaint );
	OnPaintEntireTree( dc );
}

void CExtTreeCtrl::OnPaintEntireTree( CDC & dc )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
CRect rcClient;
	GetClientRect( &rcClient );
COLORREF clrTreeBkColor = TreeBkColorGet();
	if( clrTreeBkColor == COLORREF(-1L) )
		clrTreeBkColor = ::GetSysColor( COLOR_WINDOW );
	dc.FillSolidRect( &rcClient, clrTreeBkColor );
CFont * pOldFont = dc.SelectObject( &m_fontNormal );
COLORREF clrBackgroundOld = dc.SetBkColor( clrTreeBkColor );
COLORREF clrText = ::GetSysColor( COLOR_BTNTEXT );
COLORREF clrTextOld = dc.SetTextColor( clrText );
int nOldBkMode = dc.SetBkMode( TRANSPARENT );
bool bFocusedWindow = OnQueryWindowFocusedState();
HTREEITEM htiFirst = GetNextItem( NULL, TVGN_FIRSTVISIBLE );
HTREEITEM htiFocus = GetFocusedItem();
HTREEITEM htiDH = GetNextItem( NULL, TVGN_DROPHILITE );
HTREEITEM hti = htiFirst;
	for( ; hti != NULL; hti = GetNextItem( hti, TVGN_NEXTVISIBLE ) )
	{
		bool bDisabledItem = ! OnQueryItemEnabledState( hti );
		bool bFocusedItem = ( hti == htiFocus ) ? true : false;
		bool bSelectedItem = IsItemSelected( hti );
		bool bDH = ( hti == htiDH ) ? true : false;
		CRect rcItemEntire;
		TreeItemRectGet( hti, rcItemEntire, e_tirt_entire );
		if( rcItemEntire.top > rcClient.bottom )
			break;
		OnPaintTreeItem(
			hti,
			dc,
			rcItemEntire,
			bFocusedWindow,
			bDisabledItem,
			bSelectedItem,
			bFocusedItem,
			bDH/*,
			clrBackground*/
			);
		dc.SetBkColor( clrTreeBkColor );
		dc.SetTextColor( clrText );
	}
	dc.SetBkMode( nOldBkMode );
	dc.SetTextColor( clrTextOld );
	dc.SetBkColor( clrBackgroundOld );
	dc.SelectObject( pOldFont );
}

void CExtTreeCtrl::OnPaintTreeItem(
	HTREEITEM hti,
	CDC & dc,
	CRect rcItemEntire,
	bool bFocusedWindow,
	bool bDisabledItem,
	bool bSelectedItem,
	bool bFocusedItem,
	bool bDH
	)
{
	ASSERT_VALID( this );
	ASSERT( hti != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible( &rcItemEntire ) )
		return;
bool bShowSelectionAlways = ShowSelectionAlwaysGet();
bool bHoverDisabledItems = ( ( TreeGetStyle() & __EXT_TREE_HOVER_DISABLE_ITEMS ) != 0 ) ? true : false;
const TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
INT nLevelIndent = (INT)GetIndent();
COLORREF clrItemBk = _TII.m_clrBackgroundNormal;
COLORREF clrItemText = _TII.m_clrTextNormal;
	if( bDisabledItem )
	{
		if( _TII.m_clrTextDisabled != COLORREF(-1L) )
			clrItemText = _TII.m_clrTextDisabled;
		else
			clrItemText = ::GetSysColor( COLOR_3DSHADOW );
		if( _TII.m_clrBackgroundDisabled != COLORREF(-1L) )
			clrItemBk = _TII.m_clrBackgroundDisabled;
		else
		{
			clrItemBk = TreeBkColorGet();
			if( clrItemBk == COLORREF(-1L) )
				clrItemBk = ::GetSysColor( COLOR_WINDOW );
		}
		if( bSelectedItem && ( bFocusedWindow || bShowSelectionAlways ) )
		{
			/*if( _TII.m_clrTextSelected != COLORREF(-1L) )
				clrItemText = _TII.m_clrTextSelected;
			else
				clrItemText = ::GetSysColor( bFocusedWindow ? COLOR_HIGHLIGHTTEXT : COLOR_BTNTEXT );*/
			if( _TII.m_clrBackgroundSelected != COLORREF(-1L) )
				clrItemBk = _TII.m_clrBackgroundSelected;
			else
			{
				clrItemBk = ::GetSysColor( bFocusedWindow ? COLOR_HIGHLIGHT : COLOR_BTNFACE );
				if(		( ! bFocusedWindow )
					&&	g_PaintManager.m_bIsWinVista
					&&	g_PaintManager.m_UxTheme.IsControlsThemed()
					)
					clrItemBk = CExtPaintManager::stat_HLS_Adjust( clrItemBk, 0.0, -0.10, 0.0 );
			}
		}
	}
	else if( bSelectedItem && ( bFocusedWindow || bShowSelectionAlways ) )
	{
		
		if( _TII.m_clrTextSelected != COLORREF(-1L) )
			clrItemText = _TII.m_clrTextSelected;
		else
			clrItemText = ::GetSysColor( bFocusedWindow ? COLOR_HIGHLIGHTTEXT : COLOR_BTNTEXT );
		if( _TII.m_clrBackgroundSelected != COLORREF(-1L) )
			clrItemBk = _TII.m_clrBackgroundSelected;
		else
		{
			clrItemBk = ::GetSysColor( bFocusedWindow ? COLOR_HIGHLIGHT : COLOR_BTNFACE );
			if(		( ! bFocusedWindow )
				&&	g_PaintManager.m_bIsWinVista
				&&	g_PaintManager.m_UxTheme.IsControlsThemed()
				)
				clrItemBk = CExtPaintManager::stat_HLS_Adjust( clrItemBk, 0.0, -0.10, 0.0 );
		}
		
	}
	else if( bDH )
	{
		if( _TII.m_clrTextSelected != COLORREF(-1L) )
			clrItemText = _TII.m_clrTextSelected;
		else
			clrItemText = ::GetSysColor( COLOR_HIGHLIGHTTEXT );
		if( _TII.m_clrBackgroundSelected != COLORREF(-1L) )
			clrItemBk = _TII.m_clrBackgroundSelected;
		else
		{
			clrItemBk = ::GetSysColor( COLOR_HIGHLIGHT );
			if(		( ! bFocusedWindow )
				&&	g_PaintManager.m_bIsWinVista
				&&	g_PaintManager.m_UxTheme.IsControlsThemed()
				)
				clrItemBk = CExtPaintManager::stat_HLS_Adjust( clrItemBk, 0.0, -0.10, 0.0 );
		}
	}

	if(		hti == m_htiHover
		&&	( bHoverDisabledItems || (! bDisabledItem ) )
		&&	( (!bSelectedItem) || ( (!bShowSelectionAlways) && (!bFocusedWindow) ) )
		&&	( GetStyle() & TVS_TRACKSELECT ) != 0
		&&	(	( m_dwLastHoverHitTestFlags & __EXT_TVHT_ONITEMLABEL ) != 0
			||	( GetStyle() & TVS_FULLROWSELECT ) != 0
			)
		)
	{
		if( _TII.m_clrTextHover != COLORREF(-1L) )
			clrItemText = _TII.m_clrTextHover;
		else
			clrItemText = RGB(0,0,255);
	}

	// item background
CRect rcItemLabel;
	TreeItemRectGet( hti, rcItemLabel, e_tirt_label );
bool bFullRowSelection = ( ( GetStyle() & TVS_FULLROWSELECT ) != 0 ) ? true : false;
CRect rcItemBackground = bFullRowSelection ? rcItemEntire : rcItemLabel;
	OnPaintTreeItemBackground(
		hti,
		dc,
		rcItemBackground,
		bFocusedWindow,
		bDisabledItem,
		bSelectedItem,
		bFocusedItem,
		bDH,
		clrItemBk
		);
	// tree lines
CRect rcLinesArea;
	VERIFY( TreeItemRectGet( hti, rcLinesArea, e_tirt_line_area ) );
COLORREF clrForceLines = ( bFullRowSelection && bSelectedItem ) ? clrItemText : COLORREF(-1L);
	OnPaintTreeItemLines(hti, dc, nLevelIndent, rcLinesArea, clrForceLines );

	// tree box
CRect rcTreeBoxArea;
	VERIFY( TreeItemRectGet( hti, rcTreeBoxArea, e_tirt_tree_box ) );
COLORREF clrForceBoxLines = ( bFullRowSelection && bSelectedItem ) ? clrItemText : COLORREF(-1L);
COLORREF clrForceBoxBackground = ( bFullRowSelection && bSelectedItem ) ? clrItemBk : COLORREF(-1L);
COLORREF clrForceBoxBk = COLORREF(-1L);
	if( bFullRowSelection && ( bFocusedWindow || bShowSelectionAlways ) )
		clrForceBoxBk = clrItemBk;
	OnPaintTreeItemExpandCollapseButton(
		hti,
		dc,
		rcTreeBoxArea,
		clrForceBoxLines,
		clrForceBoxBackground,
		clrForceBoxBk
		);

	// check box
	if(		_TII.m_eCheckBoxType == e_cbt_check_box
		||	_TII.m_eCheckBoxType == e_cbt_tri_state_check_box
		)
	{
		CRect rcTreeCheckBoxArea;
		VERIFY( TreeItemRectGet( hti, rcTreeCheckBoxArea, e_tirt_check_box ) );
		OnPaintTreeItemCheckBox( hti, dc, rcTreeCheckBoxArea );
	}

	// radio box
	if( _TII.m_eCheckBoxType == e_cbt_radio )
	{
		CRect rcTreeRadioBoxArea;
		VERIFY( TreeItemRectGet( hti, rcTreeRadioBoxArea, e_tirt_check_box ) );
		OnPaintTreeItemRadioBox( hti, dc, rcTreeRadioBoxArea );
	}

	// icon
CRect rcTreeIconArea;
	if( TreeItemRectGet( hti, rcTreeIconArea, e_tirt_icon ) )
	{
		if( dc.RectVisible( &rcTreeIconArea ) )
		{
			const CExtCmdIcon * pIcon = OnTreeItemQueryIcon( hti );
			if(		pIcon != NULL
				&& ( ! pIcon->IsEmpty() )
				)
				OnPaintTreeItemIcon( hti, dc, *pIcon, rcTreeIconArea );
		}
	}

	// control text
	if(		_TII.m_nControlWidth > 0
		//&&	( ! _TII.m_strEditText.IsEmpty() )
		)
	{
		CRect rcTreeControlArea;
		TreeItemRectGet( hti, rcTreeControlArea, e_tirt_control );
		COLORREF clrControlText = COLORREF(-1L);
		if( bFullRowSelection )
			clrControlText = clrItemText;
		OnPaintTreeItemControl( hti, dc, rcTreeControlArea, clrControlText );
	}

	// text
	OnPaintTreeItemText( hti, dc, rcItemLabel, clrItemText );

	// focus rect
	if( bFocusedItem && bFocusedWindow )
		dc.DrawFocusRect( &rcItemBackground );

	// insert mark
	if( hti == m_htiInsertMark )
	{
		COLORREF clrInsertMark = GetInsertMarkColor();
		OnPaintTreeItemInsertMark( hti, dc, m_bInsertMarkAfter, clrInsertMark, rcItemEntire, rcItemLabel );
	}
}

void CExtTreeCtrl::OnPaintTreeItemLines(
	HTREEITEM hti,
	CDC & dc,
	INT nLevelIndent,
	CRect rcLinesArea,
	COLORREF clrForceLines
	)
{
	ASSERT_VALID( this );
	ASSERT( hti != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );
bool bHasLines = ( ( GetStyle() & TVS_HASLINES ) != 0 ) ? true : false;
//bool bLinesAtRoot = ( ( GetStyle() & TVS_LINESATROOT ) != 0 ) ? true : false;
	if( bHasLines )
	{
		CRect rcLines = rcLinesArea;
		rcLines.left = rcLines.right - nLevelIndent;
		HTREEITEM htiWalk = hti;
		COLORREF clrLines = clrForceLines;
		if( clrLines == COLORREF(-1L) )
			clrLines = LineColorGet();
		if( clrLines == COLORREF(-1L) )
			clrLines = RGB(0,0,0);
		CPen pen( PS_SOLID, 1, clrLines );
		CPen * pOldPen = dc.SelectObject( &pen );
		e_line_style_t eOS = LineStyleGet();
			for( ; htiWalk != NULL; htiWalk = GetNextItem( htiWalk, TVGN_PARENT ) )
			{
				if( dc.RectVisible( &rcLines ) )
				{
					e_line_type_t eOT = line_empty;
					if( GetNextItem( htiWalk, TVGN_NEXT ) == NULL )
					{
						if( htiWalk == hti )
							eOT = line_corner;
					}
					else
					{
						if( htiWalk == hti )
							eOT = line_branch;
						else
							eOT = line_beam;
					}
					CPoint pointCenter = rcLines.CenterPoint();
					CPoint pointTop( pointCenter.x, rcLines.top );
					CPoint pointBottom( pointCenter.x, rcLines.bottom );
					CPoint pointRight( rcLines.right, pointCenter.y );
					if( eOT != line_empty )
					{ // top ray
						switch( eOS )
						{
						case line_style_solid:
							dc.MoveTo( pointCenter );
							dc.LineTo( pointTop );
						break;
						case line_style_dots:
						{
							for( int y = pointTop.y; y <= pointCenter.y; y++ )
							{
								if( ( y & 1 ) == 0 )
									dc.SetPixel( pointCenter.x, y, clrLines );
							}
						}
						break;
						}
					}
					if( eOT == line_branch || eOT == line_beam )
					{ // bottom ray
						switch( eOS )
						{
						case line_style_solid:
							dc.MoveTo( pointCenter );
							dc.LineTo( pointBottom );
						break;
						case line_style_dots:
						{
							for( int y = pointCenter.y; y <= pointBottom.y; y++ )
							{
								if( ( y & 1 ) == 0 )
									dc.SetPixel( pointCenter.x, y, clrLines );
							}
						}
						break;
						}
					}
					if( eOT == line_branch || eOT == line_corner )
					{ // right ray
						switch( eOS )
						{
						case line_style_solid:
							dc.MoveTo( pointCenter );
							dc.LineTo( pointRight );
						break;
						case line_style_dots:
						{
							for( int x = pointCenter.x; x <= pointRight.x; x++ )
							{
								if(		( ( ( pointCenter.y & 1 ) != 0 ) && ( x & 1 ) == 0 )
									||	( ( ( pointCenter.y & 1 ) == 0 ) && ( x & 1 ) != 0 )
									)
									dc.SetPixel( x, pointCenter.y, clrLines );
							}
						}
						break;
						}
					}
				}
				rcLines.OffsetRect( - nLevelIndent, 0 );
			}
			dc.SelectObject( pOldPen );
	}
}

void CExtTreeCtrl::OnPaintTreeItemExpandCollapseButton(
	HTREEITEM hti,
	CDC & dc,
	CRect rcTreeBoxArea,
	COLORREF clrForceBoxLines,
	COLORREF clrForceBoxBackground,
	COLORREF clrForceBoxBk
	)
{
	ASSERT_VALID( this );
	ASSERT( hti != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );
bool bHasButtons = ( ( GetStyle() & TVS_HASBUTTONS ) != 0 ) ? true : false;
	if( ! bHasButtons )
		return;
	if( ! dc.RectVisible( &rcTreeBoxArea ) )
		return;
bool bHoveredState = false;
	if(		hti == m_htiHover
		&&	( TreeGetStyleEx() & __EXT_TREE_EX_HOVER_TREE_BOXES ) != 0
		&&	( m_dwLastHoverHitTestFlags & __EXT_TVHT_ONITEMBUTTON ) != 0
		)
		bHoveredState = true;
e_tree_box_style_t eTBS = TreeBoxStyleGet();
CPoint ptTreeBoxCenter = rcTreeBoxArea.CenterPoint();
CRect rcTreeBox(
		ptTreeBoxCenter.x, ptTreeBoxCenter.y,
		ptTreeBoxCenter.x + 1, ptTreeBoxCenter.y + 1
		);
	rcTreeBox.InflateRect( 4, 4 );
	if( eTBS == tree_box_style_bitmap )
	{
		if( ! ItemHasChildren( hti ) )
			return;
		COLORREF clrBoxBk = clrForceBoxBk;
		if( clrBoxBk == COLORREF(-1L) )
			clrBoxBk = TreeBkColorGet();
		if( clrBoxBk == COLORREF(-1L) )
			clrBoxBk = ::GetSysColor( COLOR_WINDOW );
		CBrush _brush( clrBoxBk );
		CBrush * pOldBrush = dc.SelectObject( &_brush );
		CPen * pOldPen = dc.SelectObject( CPen::FromHandle( HPEN( ::GetStockObject( NULL_PEN ) ) ) );
		CRect rcEraseUnderBmp = rcTreeBox;
		rcEraseUnderBmp.InflateRect( 2, 2 );
		dc.Ellipse( &rcEraseUnderBmp );
		dc.SelectObject( pOldPen );
		dc.SelectObject( pOldBrush );
		if( ! TreeItemIsExpanded( hti ) )
		{
			if( bHoveredState )
				m_bmpTreeBoxCollapsedHover.AlphaBlend( dc, rcTreeBox );
			else
				m_bmpTreeBoxCollapsed.AlphaBlend( dc, rcTreeBox );
		}
		else
		{
			if( bHoveredState )
				m_bmpTreeBoxExpandedHover.AlphaBlend( dc, rcTreeBox );
			else
				m_bmpTreeBoxExpanded.AlphaBlend( dc, rcTreeBox );
		}
		return;
	} // if( eTBS == tree_box_style_bitmap )
COLORREF clrSign = clrForceBoxLines;
	if( clrSign == COLORREF(-1L) )
		clrSign = ExpandCollapseButtonSignColorGet();
COLORREF clrFill = clrForceBoxBackground;
	if( clrFill == COLORREF(-1L) )
		clrFill = ExpandCollapseButtonFillColorGet();
	if( bHoveredState )
		clrFill = ::GetSysColor( COLOR_3DFACE );
COLORREF clrBoxBorder = clrForceBoxLines;
	if( clrBoxBorder == COLORREF(-1L) )
		clrBoxBorder = LineColorGet();
	if( clrSign == COLORREF(-1L) )
		clrSign = RGB(0,0,0);
	if( clrFill == COLORREF(-1L) )
		clrFill = RGB(255,255,255);
	if( clrBoxBorder == COLORREF(-1L) )
		clrBoxBorder = RGB(0,0,0);
CPen penSign( PS_SOLID, 1, clrSign );
CPen penBorder( PS_SOLID, 1, clrBoxBorder );
CPen * pOldPen = dc.SelectObject( &penBorder );
CBrush brushFill (clrFill);
CBrush * pOldBrush = dc.SelectObject( &brushFill );
	if( ItemHasChildren( hti ) )
	{
		switch(eTBS)
		{
		case tree_box_style_square:
			dc.Rectangle( &rcTreeBox );
		break;
		case tree_box_style_round:
			dc.Ellipse( &rcTreeBox );
		break;
		} // switch(eTBS)
		dc.SelectObject( &penSign );
		dc.MoveTo( rcTreeBox.left + 2, ptTreeBoxCenter.y );
		dc.LineTo( rcTreeBox.right - 2, ptTreeBoxCenter.y );
		if( ! TreeItemIsExpanded( hti ) )
		{
			dc.MoveTo( ptTreeBoxCenter.x, rcTreeBox.top + 2 );
			dc.LineTo( ptTreeBoxCenter.x, rcTreeBox.bottom - 2 );
		}
	} // if( ItemHasChildren( hti ) )
	dc.SelectObject( pOldPen );
	dc.SelectObject( pOldBrush );
}

void CExtTreeCtrl::OnPaintTreeItemBackground(
	HTREEITEM hti,
	CDC & dc,
	CRect rcItemLabel,
	bool bFocusedWindow,
	bool bDisabledItem,
	bool bSelectedItem,
	bool bFocusedItem,
	bool bDH,
	COLORREF clrItemBk
	)
{
	ASSERT_VALID( this );
	ASSERT( hti != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );
	hti;
	bFocusedWindow;
	bDisabledItem;
	bSelectedItem;
	bFocusedItem;
	bDH;
	if( ! dc.RectVisible( &rcItemLabel ) )
		return;
	if( clrItemBk != COLORREF(-1L) )
		dc.FillSolidRect( &rcItemLabel, clrItemBk );
}

void CExtTreeCtrl::OnPaintTreeItemText(
		HTREEITEM hti,
		CDC & dc,
		CRect rcItemLabel,
		COLORREF clrItemText
		)
{
	ASSERT_VALID( this );
	ASSERT( hti != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible( &rcItemLabel ) )
		return;
CExtSafeString strItemText = GetItemText( hti );
	if( strItemText.IsEmpty() )
		return;
	if( clrItemText != COLORREF(-1L) )
		dc.SetTextColor( clrItemText );
CRect rcText = rcItemLabel;
	rcText.DeflateRect( PaddingLabelLeftGet(), 0, PaddingLabelRightGet(), 0 ); // INNER L/R MARGINS
CFont * pFont = NULL;
	pFont = dc.SelectObject( OnQueryItemFont( hti ) );
	CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(strItemText), strItemText.GetLength(), rcText, DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_NOPREFIX, 0 );
	dc.SelectObject( pFont );
}

void CExtTreeCtrl::OnPaintTreeItemInsertMark(
	HTREEITEM hti,
	CDC & dc,
	bool bInsertMarkAfter,
	COLORREF clrInsertMark,
	CRect rcItemEntire,
	CRect rcItemLabel
	)
{
	ASSERT_VALID( this );
	ASSERT( hti != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );
	hti;
	if( ! dc.RectVisible( &rcItemLabel ) )
		return;
LONG nVerticalLocation = bInsertMarkAfter ? rcItemEntire.bottom : rcItemEntire.top;
CRect rcInsertMark( rcItemLabel.left, nVerticalLocation, rcItemEntire.right, nVerticalLocation );
	if( bInsertMarkAfter )
		rcInsertMark.top -= 2;
	else
		rcInsertMark.bottom += 2;
CBrush brush( clrInsertMark );
	dc.FillRect( &rcInsertMark, &brush );
CRect rcV1( rcInsertMark ), rcV2( rcInsertMark );
	rcV1.right = rcV1.left + 1;
	rcV2.left = rcV2.right - 1;
	rcV1.InflateRect( 0, 3 );
	rcV2.InflateRect( 0, 3 );
	dc.FillRect( &rcV1, &brush );
	dc.FillRect( &rcV2, &brush );

	rcV1.DeflateRect( 0, 1 );
	rcV2.DeflateRect( 0, 1 );
	rcV1.OffsetRect( 1, 0  );
	rcV2.OffsetRect( -1, 0 );
	dc.FillRect( &rcV1, &brush );
	dc.FillRect( &rcV2, &brush );

	rcV1.DeflateRect( 0, 1 );
	rcV2.DeflateRect( 0, 1 );
	rcV1.OffsetRect( 1, 0  );
	rcV2.OffsetRect( -1, 0 );
	dc.FillRect( &rcV1, &brush );
	dc.FillRect( &rcV2, &brush );
}

CFont * CExtTreeCtrl::OnQueryItemFont( HTREEITEM hti )
{
	ASSERT_VALID( this );
	ASSERT( hti != NULL );
bool bHoverDisabledItems = ( ( TreeGetStyle() & __EXT_TREE_HOVER_DISABLE_ITEMS ) != 0 ) ? true : false;
TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
CFont * pFont = NULL;
bool bBold = _TII.m_bBold;
bool bItalic = _TII.m_bItalic;
bool bUnderline = _TII.m_bUnderline;
bool bItemIsEnabled = OnQueryItemEnabledState( hti );
	if(		hti == m_htiHover
		&&	( bItemIsEnabled || ( bHoverDisabledItems && (! bItemIsEnabled ) ) )
		&&	( GetStyle() & TVS_TRACKSELECT ) != 0
		&&	(	( m_dwLastHoverHitTestFlags & __EXT_TVHT_ONITEMLABEL ) != 0
			||	( GetStyle() & TVS_FULLROWSELECT ) != 0
			)
		)
		bUnderline = (!bUnderline);
	if( bBold && (!bItalic) )
		return bUnderline ? (&m_fontBoldUnderline) : (&m_fontBold);
	else if( (!bBold) && bItalic )
		return bUnderline ? (&m_fontItalicUnderline) : (&m_fontItalic);
	else if( bBold && bItalic )
		return bUnderline ? (&m_fontBoldItalicUnderline) : (&m_fontBoldItalic);
	else if( (!bBold) && (!bItalic) && bUnderline )
		return (&m_fontNormalUnderline); 
	return pFont;
}

void CExtTreeCtrl::OnPaintTreeItemCheckBox(
		HTREEITEM hti,
		CDC & dc,
		CRect rcTreeCheckBoxArea
		)
{
	ASSERT_VALID( this );
	ASSERT( hti != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible( &rcTreeCheckBoxArea ) )
		return;
bool bHoveredState = false;
	if(		hti == m_htiHover
		&&	( TreeGetStyleEx() & __EXT_TREE_EX_HOVER_CHECK_BOXES ) != 0
		&&	( m_dwLastHoverHitTestFlags & __EXT_TVHT_ONCHECKBOX) != 0
		)
		bHoveredState = true;
TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
CExtPaintManager::eBoxState_t eBS = CExtPaintManager::BOX_UNCHECKED;
bool bItemEnabled = OnQueryItemEnabledState( hti );
	if( ! bItemEnabled )
	{
		if( _TII.m_nCheckBoxState == 1 )
			eBS = CExtPaintManager::BOX_DISABLED_CHECKED;
		else if( _TII.m_nCheckBoxState == 2 )
			eBS = CExtPaintManager::BOX_DISABLED_INDETERMINATE;
		else
			eBS = CExtPaintManager::BOX_DISABLED_UNCHECKED;
	}
	else
	{
		if( _TII.m_nCheckBoxState == 1 )
			eBS = bHoveredState ? CExtPaintManager::BOX_MOUSE_HOVER_CHECKED : CExtPaintManager::BOX_CHECKED;
		else if( _TII.m_nCheckBoxState == 2 )
			eBS = bHoveredState ? CExtPaintManager::BOX_MOUSE_HOVER_INDETERMINATE : CExtPaintManager::BOX_INDETERMINATE;
		else if( bHoveredState )
			eBS = CExtPaintManager::BOX_MOUSE_HOVER_UNCHECKED;
	}
CExtPaintManager::PAINTCHECKRADIOBUTTONDATA _pcrbd(
		this,
		eBS,
		true,
		false,
		rcTreeCheckBoxArea,
		_T(""),
		false,
		false,
		bItemEnabled,
		false,
		0,
		NULL,
		true
		);
	g_PaintManager->PaintCheckButton( dc, _pcrbd );
}

void CExtTreeCtrl::OnPaintTreeItemRadioBox(
		HTREEITEM hti,
		CDC & dc,
		CRect rcTreeRadioBoxArea
		)
{
	ASSERT_VALID( this );
	ASSERT( hti != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible( &rcTreeRadioBoxArea ) )
		return;
bool bHoveredState = false;
	if(		hti == m_htiHover
		&&	( TreeGetStyleEx() & __EXT_TREE_EX_HOVER_RADIO_BUTTONS ) != 0
		&&	( m_dwLastHoverHitTestFlags & __EXT_TVHT_ONCHECKBOX) != 0
		)
		bHoveredState = true;
TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
CExtPaintManager::eBoxState_t eBS = CExtPaintManager::BOX_UNCHECKED;
bool bItemEnabled = OnQueryItemEnabledState( hti );
	if( ! bItemEnabled )
	{
		if( _TII.m_nCheckBoxState == 1 )
			eBS = CExtPaintManager::BOX_DISABLED_CHECKED;
		else if( _TII.m_nCheckBoxState == 2 )
			eBS = CExtPaintManager::BOX_DISABLED_INDETERMINATE;
		else
			eBS = CExtPaintManager::BOX_DISABLED_UNCHECKED;
	}
	else
	{
		if( _TII.m_nCheckBoxState == 1 )
			eBS = bHoveredState ? CExtPaintManager::BOX_MOUSE_HOVER_CHECKED : CExtPaintManager::BOX_CHECKED;
		else if( _TII.m_nCheckBoxState == 2 )
			eBS = bHoveredState ? CExtPaintManager::BOX_MOUSE_HOVER_INDETERMINATE : CExtPaintManager::BOX_INDETERMINATE;
		else if( bHoveredState )
			eBS = CExtPaintManager::BOX_MOUSE_HOVER_UNCHECKED;
	}
CExtPaintManager::PAINTCHECKRADIOBUTTONDATA _pcrbd(
		this,
		eBS,
		true,
		false,
		rcTreeRadioBoxArea,
		_T(""),
		false,
		false,
		bItemEnabled,
		false,
		0,
		NULL,
		true
		);
	g_PaintManager->PaintRadioButton( dc, _pcrbd );
}

void CExtTreeCtrl::OnPaintTreeItemIcon(
	HTREEITEM hti,
	CDC & dc,
	const CExtCmdIcon & _icon,
	CRect rcTreeIconArea,
	BYTE nSCA // = BYTE(255)
	)
{
	ASSERT_VALID( this );
	ASSERT( hti != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );
	ASSERT( ! _icon.IsEmpty() );
CSize sizeIcon = _icon.GetSize(), sizeArea = rcTreeIconArea.Size();
CRect _rcTreeIconArea = rcTreeIconArea;
	if( sizeIcon != sizeArea )
	{
		_rcTreeIconArea.SetRect(
			_rcTreeIconArea.left, _rcTreeIconArea.top,
			_rcTreeIconArea.left + sizeIcon.cx, _rcTreeIconArea.top + sizeIcon.cy
			);
		_rcTreeIconArea.OffsetRect( ( sizeArea.cx - sizeIcon.cx ) / 2, ( sizeArea.cy - sizeIcon.cy ) / 2 );
	}
CExtCmdIcon::e_paint_type_t ePT = IsItemEnabled( hti ) ? CExtCmdIcon::__PAINT_NORMAL : CExtCmdIcon::__PAINT_DISABLED;
	_icon.Paint( g_PaintManager.GetPM(), dc.m_hDC, _rcTreeIconArea, ePT, nSCA );
}

void CExtTreeCtrl::OnPaintTreeItemControl(
		HTREEITEM hti,
		CDC & dc,
		CRect rcTreeControlArea,
		COLORREF clrControlText
		)
{
	ASSERT_VALID( this );
	ASSERT( hti != NULL );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible( &rcTreeControlArea ) )
		return;
TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
e_item_control_type_t eICT = _TII.m_eICT;
	switch( eICT )
	{
	case __EICT_PROGRESS_BAR_THEMED:
	case __EICT_PROGRESS_BAR_WITH_BOXES:
	case __EICT_PROGRESS_BAR_SMOOTH:
	case __EICT_PROGRESS_BAR_ENDLESS:
	{
		if( eICT == __EICT_PROGRESS_BAR_THEMED )
		{
			CExtPaintManager::PAINTPROGRESSDATA _ppd(
				_TII.m_nProgressPos - _TII.m_nProgressMin,
				_TII.m_nProgressMax - _TII.m_nProgressMin,
				rcTreeControlArea,
				true,
				false,
				true,
				this
				);
			if( g_PaintManager->PaintProgress( dc, _ppd ) )
				return;
			eICT = __EICT_PROGRESS_BAR_SMOOTH;
		}

		COLORREF clrProgressBorderColor = ProgressBorderColorGet();
		if( clrProgressBorderColor == COLORREF(-1L) )
			clrProgressBorderColor = ::GetSysColor( COLOR_3DDKSHADOW );
		COLORREF clrProgressBackgroundColor = ProgressBackgroundColorGet();
		if( clrProgressBackgroundColor == COLORREF(-1L) )
			clrProgressBackgroundColor = ::GetSysColor( COLOR_HIGHLIGHTTEXT );
		COLORREF clrProgressBoxColor = ProgressBoxColorGet();
		if( clrProgressBoxColor == COLORREF(-1L) )
			clrProgressBoxColor = ::GetSysColor( COLOR_HIGHLIGHT );

		CRect rcBar = rcTreeControlArea;
		rcBar.DeflateRect( 0, 1, 0, 2 );
		CSize _sizeMargins( 2, 2 );
		CRect rcProgress = rcBar;
		rcProgress.DeflateRect( _sizeMargins );
	
		INT nBoxWidth = ::MulDiv( rcProgress.Height(), 3, 5 ), nDistanceBetweenBoxes = 2, nX;
		if(  nBoxWidth <= 0 )
			nBoxWidth = 2;
		INT nBoxCount = ( rcProgress.Width() + nDistanceBetweenBoxes ) / ( nBoxWidth + nDistanceBetweenBoxes );
		if( eICT == __EICT_PROGRESS_BAR_WITH_BOXES )
		{
			rcProgress.right = rcProgress.left + nBoxCount * ( nBoxWidth + nDistanceBetweenBoxes ) - nDistanceBetweenBoxes;
			rcBar.right = rcBar.left + rcProgress.Width() + _sizeMargins.cx * 2;
		}

		dc.FillSolidRect( &rcBar, clrProgressBackgroundColor );
		CBrush * pOldBrush = dc.SelectObject( CBrush::FromHandle( (HBRUSH)::GetStockObject( HOLLOW_BRUSH ) ) );
		CPen _penBorder( PS_SOLID, 1, clrProgressBorderColor );
		CPen * pOldPen = dc.SelectObject( &_penBorder );
		dc.Rectangle( &rcBar );
		dc.SelectObject( pOldPen );
		dc.SelectObject( pOldBrush );

		if(		_TII.m_nProgressMax <= _TII.m_nProgressMin
			||	_TII.m_nProgressPos < _TII.m_nProgressMin
			||	_TII.m_nProgressPos > _TII.m_nProgressMax
			)
			return;
		
		if( eICT == __EICT_PROGRESS_BAR_ENDLESS )
		{
			INT nBoxIndex, nBoxesDisplayed = ::MulDiv( nBoxCount, 1, 3 );
			if( nBoxesDisplayed <= 0 )
				nBoxesDisplayed = 1;
			CRect rcEndlessArea(
				rcProgress.left,
				rcProgress.top,
				rcProgress.left + nBoxesDisplayed * ( nBoxWidth + nDistanceBetweenBoxes ) - nDistanceBetweenBoxes,
				rcProgress.bottom
				);
			rcEndlessArea.OffsetRect(
				::MulDiv(
					rcProgress.Width() - rcEndlessArea.Width(),
					_TII.m_nProgressPos - _TII.m_nProgressMin,
					_TII.m_nProgressMax - _TII.m_nProgressMin
					),
				0
				);
			if( dc.RectVisible( &rcEndlessArea ) )
			{
				CBrush _brushProgressArea( clrProgressBoxColor );
				CRect rcBox(
					rcEndlessArea.left,
					rcEndlessArea.top,
					rcEndlessArea.left + nBoxWidth,
					rcEndlessArea.bottom
					);
				for( nBoxIndex = 0; nBoxIndex < nBoxesDisplayed; nBoxIndex ++ )
				{
					dc.FillRect( &rcBox, &_brushProgressArea );
					rcBox.OffsetRect( nBoxWidth + nDistanceBetweenBoxes, 0 );
				}
			}
		}
		else
		{
			rcProgress.right = rcProgress.left
				+ ::MulDiv(
					rcProgress.Width(),
					_TII.m_nProgressPos - _TII.m_nProgressMin,
					_TII.m_nProgressMax - _TII.m_nProgressMin
					);
			if( dc.RectVisible( &rcProgress ) )
			{
				CBrush _brushProgressArea( clrProgressBoxColor );
				if( eICT == __EICT_PROGRESS_BAR_SMOOTH )
					dc.FillRect( &rcProgress, &_brushProgressArea );
				else if( eICT == __EICT_PROGRESS_BAR_WITH_BOXES )
				{
					CRect rcBox(
						rcProgress.left,
						rcProgress.top,
						rcProgress.left + nBoxWidth,
						rcProgress.bottom
						);
					for(	nX = rcProgress.left;
							nX < rcProgress.right;
							nX += nBoxWidth + nDistanceBetweenBoxes,
								rcBox.OffsetRect( nBoxWidth + nDistanceBetweenBoxes, 0 )
							)
						dc.FillRect( &rcBox, &_brushProgressArea );
				}
			}
		}
	}
	break;
	default:
	{
		CExtSafeString strEditText = GetItemText( hti );
		if( strEditText.IsEmpty() )
			return;
		CFont * pFont = dc.SelectObject( &m_fontNormal );
		COLORREF clrTextOld = COLORREF(-1L);
		if( clrControlText != COLORREF(-1L) )
			clrTextOld = dc.SetTextColor( clrControlText );
		CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(_TII.m_strEditText), _TII.m_strEditText.GetLength(), rcTreeControlArea, DT_SINGLELINE|DT_LEFT|DT_VCENTER|DT_NOPREFIX, 0 );
		if( clrControlText != COLORREF(-1L) )
			dc.SetTextColor( clrTextOld );
		dc.SelectObject( pFont );
	}
	break;
	} // switch( eICT )
}

LRESULT CExtTreeCtrl::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case TVM_EDITLABEL:
		{
			KillTimer( m_nDelayedEditingTimerID );
			m_wndContentExpand.Deactivate();
			HTREEITEM hti = (HTREEITEM)lParam; //GetFocusedItem();
			if(		hti == NULL
				||	(! HasItem( hti ) )
				)
				return LRESULT(NULL);
			SendMessage( WM_CANCELMODE );
			EnsureVisible( hti );

			HWND hWndParent = ::GetParent( m_hWnd );
			UINT nOwnID = GetDlgCtrlID();
			CExtSafeString strItemText;
			TV_DISPINFO _data;
			::memset( &_data, 0, sizeof(TV_DISPINFO) );
			_data.hdr.hwndFrom = m_hWnd;
			_data.hdr.idFrom = nOwnID;
			_data.hdr.code = TVN_BEGINLABELEDIT;
			_data.item.mask = TVIF_CHILDREN|TVIF_HANDLE|TVIF_PARAM|TVIF_STATE;
			_data.item.hItem = hti;
			GetItem( &_data.item );
			strItemText = GetItemText( hti );
			_data.item.cchTextMax = INT(strItemText.GetLength());
			_data.item.pszText = strItemText.IsEmpty() ? _T("") : LPTSTR(LPCTSTR(strItemText));
			_data.item.mask |= TVIF_TEXT;
			if( ::SendMessage( hWndParent, WM_NOTIFY, WPARAM(nOwnID), LPARAM(&_data) ) != 0 )
				return TRUE;

			CRect rcLabel;
			if( ! TreeItemRectGet( hti, rcLabel, e_tirt_label ) )
				return LRESULT(NULL);
			rcLabel.right += 6;
			CExtSafeString s;
			const TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
			CInplaceEdit * pEdit = new CInplaceEdit( true );
			if( ! pEdit->Create(
					WS_CHILD | WS_VISIBLE
						| WS_CLIPSIBLINGS | ES_AUTOHSCROLL | ES_WANTRETURN
						| ( ReadOnlyLabelsGet() ? ES_READONLY : 0 )
						| _TII.m_dwAdditionalLabelEditorStyles
						,
					rcLabel,
					this,
					UINT(IDC_STATIC)
					)
				)
				return LRESULT(NULL);

			CFont * pFont = NULL;
			pFont = OnQueryItemFont( hti );
			if( pFont == NULL )
				pFont = GetFont();
			pEdit->SetFont( pFont );
			s = GetItemText( hti );
			pEdit->SetWindowText( s );
			pEdit->SetSel(0, -1);
			pEdit->SetFocus();
			m_hWndChildControl = pEdit->m_hWnd;
			if( m_hWndChildControl != NULL )
				m_htiInplaceEdited = hti;
		}
		// continue falling:
	case TVM_GETEDITCONTROL:
		return LRESULT(m_hWndChildControl);
	case WM_NCLBUTTONDOWN:
	case WM_NCMBUTTONDOWN:
	case WM_NCRBUTTONDOWN:
		SendMessage( WM_CANCELMODE );
		if( ::GetFocus() != m_hWnd )
			SetFocus();
	break;
	case TVM_SETINSERTMARK:
	{
		m_bInsertMarkAfter = ( wParam != 0 ) ? true : false;
		HTREEITEM htiInsertMarkOld = m_htiInsertMark;
		m_htiInsertMark = (HTREEITEM) lParam;
		if( ! HasItem( m_htiInsertMark ) )
			m_htiInsertMark = NULL;
		if( htiInsertMarkOld != m_htiInsertMark && IsWindowVisible() )
			Invalidate();
	}
	break;
	case TVM_SETINSERTMARKCOLOR:
		if( IsWindowVisible() )
			Invalidate();
	break;
	case TVM_DELETEITEM:
	{
		HTREEITEM hti = (HTREEITEM)lParam;
		_UnregisterItemsFromMap( hti );
	}
	break;
	} // switch( message )
LRESULT lResult = CTreeCtrl::WindowProc( message, wParam, lParam );
	switch( message )
	{
	case WM_COMMAND:
		m_wndContentExpand.Deactivate();
		if(		HIWORD(wParam) == CBN_KILLFOCUS
			&&	m_hWndChildControl == ((HWND)lParam)
			)
		{
			SendMessage( WM_CANCELMODE );
		}
	break;
	case TVM_INSERTITEM:
	{
		m_wndContentExpand.Deactivate();
		if( m_hWndChildControl != NULL )
			SendMessage( WM_CANCELMODE );
		HTREEITEM hti = (HTREEITEM)lResult;
		if( hti != NULL )
		{
			m_mapItemInfo.SetAt( hti, new TREEITEMINFO_t );
		}
	}
	break;
	case TVM_DELETEITEM:
	{
		HTREEITEM hti = (HTREEITEM)lParam;
		if( m_htiDelayedFocus == hti )
		{
			m_htiDelayedFocus = NULL;
			KillTimer( m_nDelayedFocusTimerID );
			KillTimer( m_nDelayedEditingTimerID );
		}
		m_wndContentExpand.Deactivate();
		if( m_hWndChildControl != NULL )
			SendMessage( WM_CANCELMODE );
////	_UnregisterItemsFromMap( hti );
#ifdef _DEBUG
		if( hti == NULL || hti == TVI_ROOT )
		{
			ASSERT( m_mapItemInfo.GetCount() == 0 );
		}
#endif // _DEBUG
	}
	break;
	case TVM_EXPAND:
		m_wndContentExpand.Deactivate();
	break;
	} // switch( message )
	return lResult;
}

void CExtTreeCtrl::PostNcDestroy()
{
	::memset( &m_nLastMouseButtonEventType, 0, sizeof(m_nLastMouseButtonEventType) );
	_UnregisterItemsFromMap();
	_FontsDestroy();
	m_htiInsertMark = m_htiAnchorItem = NULL;
	CTreeCtrl::PostNcDestroy();
}

void CExtTreeCtrl::OnTreeItemUnRegisterData(
	TREEITEMINFO_t & _TII
	)
{
	ASSERT_VALID( this );
	_TII;
}

void CExtTreeCtrl::_UnregisterItemsFromMap(
	HTREEITEM hti // = NULL
	)
{
	if( hti == NULL || hti == TVI_ROOT )
	{
		if( GetSafeHwnd() == NULL )
		{
			_UnregisterItemsFromMapLinear();
			return;
		} // if( GetSafeHwnd() == NULL )
		else
		{
			hti = GetRootItem();
			if( hti != NULL )
				_UnregisterOneItemFromMap( hti );
			else
			{
				_UnregisterItemsFromMapLinear();
				return;
			}
		} // else from if( GetSafeHwnd() == NULL )
	} // if( hti == NULL || hti == TVI_ROOT )
	else
		_UnregisterOneItemFromMap( hti );
	if( GetSafeHwnd() != NULL )
	{
		HTREEITEM htiChild = GetNextItem( hti, TVGN_CHILD );
		for( ; htiChild != NULL; htiChild = GetNextItem( htiChild, TVGN_NEXT ) )
			_UnregisterItemsFromMap( htiChild );
	} // if( GetSafeHwnd() != NULL )
}

void CExtTreeCtrl::_UnregisterItemsFromMapLinear()
{
	ASSERT_VALID( this );
POSITION pos = m_mapItemInfo.GetStartPosition();
	for( ; pos != NULL; pos = m_mapItemInfo.GetStartPosition() )
	{
		HTREEITEM hti = NULL;
		TREEITEMINFO_t * pTII = NULL;
		m_mapItemInfo.GetNextAssoc( pos, hti, pTII );
		ASSERT( hti != NULL );
		_UnregisterOneItemFromMap( hti );
	}
	ASSERT( m_mapItemInfo.GetCount() == 0 );
}

void CExtTreeCtrl::_UnregisterOneItemFromMap(
	HTREEITEM hti
	)
{
	ASSERT_VALID( this );
	ASSERT( hti != NULL );
TREEITEMINFO_t * pTII = NULL;
	if( m_mapItemInfo.Lookup( hti, pTII ) )
	{
		if( pTII != NULL )
		{
			OnTreeItemUnRegisterData( *pTII );
			delete pTII;
		} // if( pTII != NULL )
		m_mapItemInfo.RemoveKey( hti );
	} // if( m_mapItemInfo.Lookup( hti, pTII ) )
	if( hti == m_htiAnchorItem )
		m_htiAnchorItem = NULL;
	if( hti == m_htiInsertMark )
		m_htiInsertMark = NULL;
}

BOOL CExtTreeCtrl::Expand( HTREEITEM hti, UINT nCode )
{
	ASSERT_VALID( this );
HWND hWndOwn = m_hWnd;
	if( hWndOwn == NULL || ( ! ::IsWindow( hWndOwn ) ) )
		return FALSE;
	if( hti == TVI_ROOT )
	{
		hti = GetRootItem();
		if( hti == NULL )
			return FALSE;
	}
HWND hWndParent = ::GetParent( m_hWnd );
UINT nOwnID = GetDlgCtrlID();
CExtSafeString strItemText;
NM_TREEVIEW _data;
	::memset( &_data, 0, sizeof(NM_TREEVIEW) );
	_data.hdr.hwndFrom = m_hWnd;
	_data.hdr.idFrom = nOwnID;
	_data.hdr.code = TVN_ITEMEXPANDING;
	_data.action = nCode;
	_data.itemOld.mask = TVIF_CHILDREN|TVIF_HANDLE|TVIF_PARAM|TVIF_STATE;
	_data.itemOld.hItem = hti;
	GetItem( &_data.itemOld );
	strItemText = GetItemText( hti );
	_data.itemOld.cchTextMax = INT(strItemText.GetLength());
	_data.itemOld.pszText = strItemText.IsEmpty() ? _T("") : LPTSTR(LPCTSTR(strItemText));
	_data.itemOld.mask |= TVIF_TEXT;
	::memcpy( &_data.itemNew, &_data.itemOld, sizeof(TVITEM) );
	if( ! ::GetCursorPos( &_data.ptDrag ) )
		_data.ptDrag.x = _data.ptDrag.y = 0;
	if( ::SendMessage( hWndParent, WM_NOTIFY, WPARAM(nOwnID), LPARAM(&_data) ) != 0 )
		return TRUE;

	if( ! CTreeCtrl::Expand( hti, nCode ) )
		return FALSE;

	::memset( &_data, 0, sizeof(NM_TREEVIEW) );
	_data.hdr.hwndFrom = m_hWnd;
	_data.hdr.idFrom = nOwnID;
	_data.hdr.code = TVN_ITEMEXPANDED;
	_data.action = nCode;
	_data.itemOld.mask = TVIF_CHILDREN|TVIF_HANDLE|TVIF_PARAM|TVIF_STATE;
	_data.itemOld.hItem = hti;
	GetItem( &_data.itemOld );
	strItemText = GetItemText( hti );
	_data.itemOld.cchTextMax = INT(strItemText.GetLength());
	_data.itemOld.pszText = strItemText.IsEmpty() ? _T("") : LPTSTR(LPCTSTR(strItemText));
	_data.itemOld.mask |= TVIF_TEXT;
	::memcpy( &_data.itemNew, &_data.itemOld, sizeof(TVITEM) );
	if( ! ::GetCursorPos( &_data.ptDrag ) )
		_data.ptDrag.x = _data.ptDrag.y = 0;
	::SendMessage( hWndParent, WM_NOTIFY, WPARAM(nOwnID), LPARAM(&_data) );

	return TRUE;
}

bool CExtTreeCtrl::TreeItemIsExpanded( HTREEITEM hti ) const
{
	ASSERT_VALID( this );
bool bExpanded = ( ( GetItemState( hti, UINT(-1) ) & TVIS_EXPANDED ) != 0 ) ? true : false;
	return bExpanded;
}

bool CExtTreeCtrl::HasItem( HTREEITEM hti ) const
{
	ASSERT_VALID( this );
	if( hti == NULL )
		return false;
TREEITEMINFO_t * pTII = NULL;
	if( ! m_mapItemInfo.Lookup( hti, pTII ) )
	{
		if( GetSafeHwnd() != NULL )
		{
			TVITEM _tv_item;
			::memset( &_tv_item, 0, sizeof(TVITEM) );
			_tv_item.hItem = hti;
			_tv_item.mask = TVIF_PARAM;
			if( ::SendMessage( m_hWnd, TVM_GETITEM, 0, (LPARAM)&_tv_item ) != 0 )
				return true;
		}
		return false;
	}
	return true;
}

bool CExtTreeCtrl::HasAncestor(
	HTREEITEM htiParent,
	HTREEITEM htiChild,
	INT nMaxStepCount // = -1
	) const
{
	ASSERT_VALID( this );
	if( htiChild == NULL )
		return false;
	if( htiParent == NULL || htiParent == TVI_ROOT )
		htiParent = GetRootItem();
	if( htiParent == NULL )
		return false;
INT nStepCount = 0;
	for( ; htiChild != NULL ; htiChild = GetNextItem(htiChild, TVGN_PARENT ), nStepCount ++ )
	{
		if( htiChild == htiParent )
			return true;
		if( nMaxStepCount >= 0 )
		{
			if( nStepCount >= nMaxStepCount )
				return false;
		}
	}
	return false;

}

CExtTreeCtrl::TREEITEMINFO_t * CExtTreeCtrl::TreeItemInfoGetPtr( HTREEITEM hti )
{
	ASSERT_VALID( this );
	if( hti == NULL )
	{
		ASSERT( false );
		::AfxThrowUserException();
	}
TREEITEMINFO_t * pTII = NULL;
	if( ! m_mapItemInfo.Lookup( hti, pTII ) )
    {
		if( HasItem( hti ) )
		{
			pTII = new TREEITEMINFO_t;
			m_mapItemInfo.SetAt( hti, pTII );
		}
		else
			return NULL;
    }
	ASSERT( pTII != NULL );
	return pTII;
}

const CExtTreeCtrl::TREEITEMINFO_t * CExtTreeCtrl::TreeItemInfoGetPtr( HTREEITEM hti ) const
{
	return ( const_cast < CExtTreeCtrl * > ( this ) ) -> TreeItemInfoGetPtr( hti );
}

CExtTreeCtrl::TREEITEMINFO_t & CExtTreeCtrl::TreeItemInfoGet( HTREEITEM hti )
{
TREEITEMINFO_t * pTII = TreeItemInfoGetPtr( hti );
	if( pTII == NULL )
	{
		ASSERT( false );
		::AfxThrowUserException();
	}
	return (*pTII);
}

const CExtTreeCtrl::TREEITEMINFO_t & CExtTreeCtrl::TreeItemInfoGet( HTREEITEM hti ) const
{
	return ( const_cast < CExtTreeCtrl * > ( this ) ) -> TreeItemInfoGet( hti );
}

CExtTreeCtrl::e_line_style_t CExtTreeCtrl::LineStyleGet() const
{
	ASSERT_VALID( this );
	return m_eOS;
}

void CExtTreeCtrl::LineStyleSet( CExtTreeCtrl::e_line_style_t eOS )
{
	ASSERT_VALID( this );
	m_eOS = eOS;
}

CExtTreeCtrl::e_tree_box_style_t CExtTreeCtrl::TreeBoxStyleGet() const
{
	ASSERT_VALID( this );
	return m_eTBS;
}

void CExtTreeCtrl::TreeBoxStyleSet( CExtTreeCtrl::e_tree_box_style_t eTBS )
{
	ASSERT_VALID( this );
	m_eTBS = eTBS;
}

COLORREF CExtTreeCtrl::ExpandCollapseButtonSignColorGet() const
{
	ASSERT_VALID( this );
	return m_clrExpandCollapseButtonSign;
}

void CExtTreeCtrl::ExpandCollapseButtonSignColorSet( COLORREF clr )
{
	ASSERT_VALID( this );
	m_clrExpandCollapseButtonSign = clr;
}

COLORREF CExtTreeCtrl::ExpandCollapseButtonFillColorGet() const
{
	ASSERT_VALID( this );
	return m_clrExpandCollapseButtonFill;
}

void CExtTreeCtrl::ExpandCollapseButtonFillColorSet( COLORREF clr )
{
	ASSERT_VALID( this );
	m_clrExpandCollapseButtonFill = clr;
}

COLORREF CExtTreeCtrl::LineColorGet() const
{
	ASSERT_VALID( this );
	return m_clrLines;
}

void CExtTreeCtrl::LineColorSet( COLORREF clr )
{
	ASSERT_VALID( this );
	m_clrLines = clr;
}

COLORREF CExtTreeCtrl::TreeBkColorGet() const
{
	ASSERT_VALID( this );
	return m_clrTreeBkColor;
}

void CExtTreeCtrl::TreeBkColorSet( COLORREF clr )
{
	ASSERT_VALID( this );
	m_clrTreeBkColor = clr;
}

COLORREF CExtTreeCtrl::ProgressBorderColorGet() const
{
	ASSERT_VALID( this );
	return m_clrProgressBorder;
}

void CExtTreeCtrl::ProgressBorderColorSet( COLORREF clr )
{
	ASSERT_VALID( this );
	m_clrProgressBorder = clr;
}

COLORREF CExtTreeCtrl::ProgressBackgroundColorGet() const
{
	ASSERT_VALID( this );
	return m_clrProgressBackgroundColor;
}

void CExtTreeCtrl::ProgressBackgroundColorSet( COLORREF clr )
{
	ASSERT_VALID( this );
	m_clrProgressBackgroundColor = clr;
}

COLORREF CExtTreeCtrl::ProgressBoxColorGet() const
{
	ASSERT_VALID( this );
	return m_clrProgressBoxColor;
}

void CExtTreeCtrl::ProgressBoxColorSet( COLORREF clr )
{
	ASSERT_VALID( this );
	m_clrProgressBoxColor = clr;
}

bool CExtTreeCtrl::OnQueryItemEnabledState( HTREEITEM hti ) const
{
	ASSERT_VALID( this );
	if( hti == NULL )
		return false;
	if( ( TreeGetStyle() & __EXT_TREE_AUTO_DISABLE_ITEMS_BY_PARENT ) == 0 )
		return IsItemEnabled( hti );
HTREEITEM htiWalk = hti;
	for( ; htiWalk != NULL; htiWalk = GetNextItem( htiWalk, TVGN_PARENT ) )
	{
		if( ! IsItemEnabled( htiWalk ) )
			return false;
	}
	return true;
}

bool CExtTreeCtrl::IsItemEnabled( HTREEITEM hti ) const
{
	ASSERT_VALID( this );
	if( hti == NULL )
		return false;
	return ( ! TreeItemInfoGet( hti ).m_bDisabled );
}

void CExtTreeCtrl::EnableItem(
	HTREEITEM hti,
	bool bEnable // = true
	)
{
	ASSERT_VALID( this );
	if( hti == NULL )
		return;
TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
	if( ( _TII.m_bDisabled && (!bEnable) ) || ( (!_TII.m_bDisabled) && bEnable ) )
		return;
	_TII.m_bDisabled = ! bEnable;
	if( GetSafeHwnd() == NULL )
		return;
CRect rcItemEntire;
	TreeItemRectGet( hti, rcItemEntire, e_tirt_entire );
	if( rcItemEntire.IsRectEmpty() )
		return;
CRect rcClient;
	GetClientRect( &rcClient );
	if(		( rcClient.top <= rcItemEntire.top && rcItemEntire.top <= rcClient.bottom )
		||	( rcClient.top <= rcItemEntire.bottom && rcItemEntire.bottom <= rcClient.bottom )
		)
		InvalidateRect( &rcItemEntire );
}

bool CExtTreeCtrl::IsItemSelected( HTREEITEM hti ) const
{
	ASSERT_VALID( this );
	if( hti == NULL )
		return false;
	return TreeItemInfoGet( hti ).m_bSelected;
}

void CExtTreeCtrl::SelectItem(
	HTREEITEM hti,
	bool bSelect, // = true
	bool bSubtract // = false
	)
{
	ASSERT_VALID( this );
	if( hti == NULL )
		return;
HWND hWndOwn = GetSafeHwnd();
TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
bool bWasSelected =  _TII.m_bSelected;
	if( bSelect && bSubtract && _TII.m_bSelected )
		_TII.m_bSelected = false;
	else
		_TII.m_bSelected = bSelect;
CList < HTREEITEM, HTREEITEM > _listInvalidateItems;
	if( _TII.m_bSelected && (! MultipleSelectionGet() ) )
	{
		CList < HTREEITEM, HTREEITEM > _listSelectedItems;
		GetSelectedItemsList( _listSelectedItems );
		POSITION pos = _listSelectedItems.GetHeadPosition();
		for( ; pos != NULL; )
		{
			HTREEITEM htiSel = _listSelectedItems.GetNext( pos );
			if( htiSel == hti )
				continue;
			TREEITEMINFO_t & _TII_Sel = TreeItemInfoGet( htiSel );
			_TII_Sel.m_bSelected = false;
			if( hWndOwn != NULL )
				_listInvalidateItems.AddTail( htiSel );
		}
	}
	if( hWndOwn == NULL || (! IsWindowVisible() ) )
		return;
	if( ( bWasSelected && (!_TII.m_bSelected) ) || ( (!bWasSelected) && _TII.m_bSelected ) )
		_listInvalidateItems.AddTail( hti );
POSITION pos = _listInvalidateItems.GetHeadPosition();
	if( pos == NULL )
		return;
CRect rcClient, rcItemEntire;
	GetClientRect( &rcClient );
	for( ; pos != NULL; )
	{
		HTREEITEM htiRedraw = _listInvalidateItems.GetNext( pos );
		TreeItemRectGet( htiRedraw, rcItemEntire, e_tirt_entire );
		if( rcItemEntire.IsRectEmpty() )
			continue;
		if(		( rcClient.top <= rcItemEntire.top && rcItemEntire.top <= rcClient.bottom )
			||	( rcClient.top <= rcItemEntire.bottom && rcItemEntire.bottom <= rcClient.bottom )
			)
			InvalidateRect( &rcItemEntire );
	}
	UpdateWindow();
}

void CExtTreeCtrl::GetSelectedItemsList(
	CList < HTREEITEM, HTREEITEM > & _listSelectedItems,
	HTREEITEM htiStart, // = NULL // NULL - root
	bool bDeep, // = true
	bool bAnalyseSelectedChildren, // = false
	bool bAddToTail // = true
	) const
{
	ASSERT_VALID( this );
	_listSelectedItems.RemoveAll();
	_GetSelectedItemsList_Impl( _listSelectedItems, htiStart, bDeep, bAnalyseSelectedChildren, bAddToTail );
}

void CExtTreeCtrl::_GetSelectedItemsList_Impl(
	CList < HTREEITEM, HTREEITEM > & _listSelectedItems,
	HTREEITEM htiStart, // = NULL // NULL - root
	bool bDeep, // = true
	bool bAnalyseSelectedChildren, // = false
	bool bAddToTail // = true
	) const
{
	ASSERT_VALID( this );
	if( htiStart == NULL )
		htiStart = TVI_ROOT;
HTREEITEM hti = GetNextItem( htiStart, TVGN_CHILD );
	for( ; hti != NULL; hti = GetNextItem( hti, TVGN_NEXT ) )
	{
		if( IsItemSelected( hti ) )
		{
			if( bAddToTail )
				_listSelectedItems.AddTail( hti );
			else
				_listSelectedItems.AddHead( hti );
			if( ! bAnalyseSelectedChildren )
				continue;
		}
		if( bDeep )
			_GetSelectedItemsList_Impl( _listSelectedItems, hti, bDeep, bAnalyseSelectedChildren, bAddToTail );
	}
}

void CExtTreeCtrl::SelectItemRange(
	HTREEITEM htiFrom,
	HTREEITEM htiTo,
	bool bSelect,
	bool bExpandedOnly, // = true
	bool bIncludeDisabled, // = true
	bool bSubtract // = false
	)
{
	ASSERT_VALID( this );
	if( htiFrom == NULL || htiTo == NULL )
		return;
	if( htiFrom == htiTo )
	{
		SelectItem( htiFrom, bSelect, bSubtract );
		return;
	}
HTREEITEM hti;
	if( ! LinearItemIsBefore( htiTo, htiFrom, bExpandedOnly ) )
	{
		hti = htiFrom;
		htiFrom = htiTo;
		htiTo = hti;
	}
HTREEITEM htiAnchor = AnchorItemGet();
	for( hti = htiFrom; true; hti = LinearItemGetNext( hti, bExpandedOnly ) )
	{
		if(		bIncludeDisabled
			||	OnQueryItemEnabledState( hti )
			)
		{
			if( bSubtract && bSelect && htiAnchor == hti )
				SelectItem( hti, true );
			else
				SelectItem( hti, bSelect, bSubtract );
		}
		if( hti == htiTo )
			break;
	}
}

HTREEITEM CExtTreeCtrl::GetInPlaceEditedItem() const
{
	return m_htiInplaceEdited;
}

HTREEITEM CExtTreeCtrl::GetFocusedItem() const
{
	ASSERT_VALID( this );
HTREEITEM hti = GetNextItem( NULL, TVGN_CARET );
	if( hti != NULL && ( ! HasItem( hti ) ) )
		return NULL;
	return hti;
}

void CExtTreeCtrl::FocusItem(
	HTREEITEM hti,
	bool bSelect, // = false
	bool bUnselectOtherItems, // = false
	bool bMouseClickEvent // = false
	)
{
	ASSERT_VALID( this );
	bMouseClickEvent;
HTREEITEM htiOldFocus = GetNextItem( NULL, TVGN_CARET );
bool bActuallyFocusChanged = ( htiOldFocus == hti ) ? false : true;
	//AnchorItemSet( htiOldFocus );
	if( hti == NULL )
		return;
bool bAnyUnSelected = false;
	if( bUnselectOtherItems )
	{
		HTREEITEM htiWalk = NULL;
		POSITION pos = m_mapItemInfo.GetStartPosition();
		for( ; pos != NULL; )
		{
			TREEITEMINFO_t * pTII = NULL;
			m_mapItemInfo.GetNextAssoc( pos, htiWalk, pTII );
			if( htiWalk == hti || pTII == NULL )
				continue;
			pTII->m_bSelected = false;
			bAnyUnSelected = true;
		}
	}
	if( bSelect )
		SelectItem( hti, true );
	if( bActuallyFocusChanged )
	{
		CTreeCtrl::SelectItem( hti );
		if( bSelect)
			SelectItem( hti, true, false );
	}
	if( bActuallyFocusChanged || bSelect || bAnyUnSelected )
		CExtTreeCtrl::Invalidate();
	if( bActuallyFocusChanged )
		OnTreeItemDelayedFocus( hti, true, bMouseClickEvent );
	if(		bMouseClickEvent
		&&	htiOldFocus != hti
		&&	( GetStyle() & TVS_SINGLEEXPAND ) != 0
		&&	(! TreeItemIsExpanded( hti ) )
		)
		Expand( hti, TVE_EXPAND );
}

void CExtTreeCtrl::OnLButtonDown( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! _OnTreeMouseClickImpl( VK_LBUTTON, 0, nFlags, point ) )
		CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CExtTreeCtrl::OnLButtonUp( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! _OnTreeMouseClickImpl( VK_LBUTTON, 1, nFlags, point ) )
		CTreeCtrl::OnLButtonUp(nFlags, point);
}

void CExtTreeCtrl::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! _OnTreeMouseClickImpl( VK_LBUTTON, 2, nFlags, point ) )
		CTreeCtrl::OnLButtonDblClk(nFlags, point);
}

void CExtTreeCtrl::OnRButtonDown( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! _OnTreeMouseClickImpl( VK_RBUTTON, 0, nFlags, point ) )
		CTreeCtrl::OnRButtonDown(nFlags, point);
}

void CExtTreeCtrl::OnRButtonUp( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! _OnTreeMouseClickImpl( VK_RBUTTON, 1, nFlags, point ) )
		CTreeCtrl::OnRButtonUp(nFlags, point);
}

void CExtTreeCtrl::OnRButtonDblClk( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! _OnTreeMouseClickImpl( VK_RBUTTON, 2, nFlags, point ) )
		CTreeCtrl::OnRButtonDblClk(nFlags, point);
}

void CExtTreeCtrl::OnMButtonDown( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! _OnTreeMouseClickImpl( VK_MBUTTON, 0, nFlags, point ) )
		CTreeCtrl::OnMButtonDown(nFlags, point);
}

void CExtTreeCtrl::OnMButtonUp( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! _OnTreeMouseClickImpl( VK_MBUTTON, 1, nFlags, point ) )
		CTreeCtrl::OnMButtonUp(nFlags, point);
}

void CExtTreeCtrl::OnMButtonDblClk( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	if( ! _OnTreeMouseClickImpl( VK_MBUTTON, 2, nFlags, point ) )
		CTreeCtrl::OnMButtonDblClk(nFlags, point);
}

bool CExtTreeCtrl::_OnTreeMouseClickImpl(
	int nMouseButton,
	int nClick,
	UINT nMouseEventFlags,
	CPoint point
	)
{
	ASSERT_VALID( this );


INT nLastMouseButtonEventIndex = 0;
	switch( nMouseButton )
	{
	case VK_LBUTTON: nLastMouseButtonEventIndex = 0; break;
	case VK_MBUTTON: nLastMouseButtonEventIndex = 1; break;
	case VK_RBUTTON: nLastMouseButtonEventIndex = 2; break;
	}

DWORD dwHitTestFlags = 0;
HTREEITEM hti = TreeItemHitTest( point, dwHitTestFlags );
	KillTimer( m_nDelayedEditingTimerID );
HWND hWndOwn = m_hWnd;
bool bRetVal =
		OnTreeMouseClick(
			hti,
			dwHitTestFlags,
			nMouseButton,
			nClick,
			nMouseEventFlags,
			point
			);
	if( ::IsWindow( hWndOwn ) )
		m_nLastMouseButtonEventType[ nLastMouseButtonEventIndex ] = nClick;
	return bRetVal;
}

bool CExtTreeCtrl::OnTreeItemDoDragDetect(
	HTREEITEM hti,
	DWORD dwHitTestFlags,
	int nMouseButton,
	int nClick,
	UINT nMouseEventFlags,
	CPoint point
	)
{
	ASSERT_VALID( this );
	ASSERT( hti != NULL );
	ASSERT( GetSafeHwnd() != NULL );
	nMouseEventFlags;
	if( ! ( nMouseButton == VK_LBUTTON || nMouseButton == VK_RBUTTON ) )
		return false;
UINT nUpMessage = ( nMouseButton == VK_LBUTTON ) ? WM_LBUTTONUP : WM_RBUTTONUP;
	if( ! CExtControlBar::stat_DoDragDetect( m_hWnd, point, nUpMessage ) )
		return false;
	if( nClick != 0 )
		return false;
	if( ( dwHitTestFlags & __EXT_TVHT_ONITEM ) == 0L )
		return false;
	if( ( GetStyle() & TVS_DISABLEDRAGDROP ) != 0L )
		return false;
UINT nMessageID = ( nMouseButton == VK_LBUTTON ) ? TVN_BEGINDRAG : TVN_BEGINRDRAG;
NM_TREEVIEW _data, _dataR;
	::memset( &_data, 0, sizeof(NM_TREEVIEW) );
	::memset( &_dataR, 0, sizeof(NM_TREEVIEW) );
	_data.hdr.hwndFrom = m_hWnd;
	_data.hdr.idFrom = GetDlgCtrlID();
	_data.hdr.code = nMessageID;
    _data.action = nMessageID;
	_data.itemNew.hItem = hti;
	_data.itemNew.mask = TVIF_IMAGE|TVIF_PARAM|TVIF_STATE|TVIF_HANDLE|TVIF_SELECTEDIMAGE|TVIF_CHILDREN;
	VERIFY( GetItem( &_data.itemNew ) );
CString strItemText = GetItemText( hti );
	_data.itemNew.mask |= TVIF_TEXT;
	_data.itemNew.cchTextMax = strItemText.GetLength();
	_data.itemNew.pszText = strItemText.IsEmpty() ? _T("") : LPTSTR(LPCTSTR(strItemText));
	::memcpy( &_data.itemOld, &_data.itemNew, sizeof(TVITEM) );
    _data.ptDrag.x = point.x;
    _data.ptDrag.y = point.y;
	SelectDropTarget( hti );
	::SendMessage( ::GetParent(m_hWnd), WM_NOTIFY, WPARAM(_data.hdr.idFrom), LPARAM(&_data) );
	SelectDropTarget( NULL );
	return true;
}

bool CExtTreeCtrl::OnTreeMouseClick(
	HTREEITEM hti,
	DWORD dwHitTestFlags,
	int nMouseButton,
	int nClick,
	UINT nMouseEventFlags,
	CPoint point
	)
{
	ASSERT_VALID( this );
	if( hti == NULL )
		return false;

	if(		( nMouseButton == VK_LBUTTON || nMouseButton == VK_RBUTTON )
		&&	( nClick == 0 || nClick == 2 )
		)
	{
		HWND hWndParent = ::GetParent( m_hWnd );
		UINT nOwnID = UINT( GetDlgCtrlID() );
		NM_TREEVIEW _data;
		::memset( &_data, 0, sizeof(NM_TREEVIEW) );
		_data.hdr.hwndFrom = m_hWnd;
		_data.hdr.idFrom = nOwnID;
		_data.hdr.code =
			( nMouseButton == VK_LBUTTON )
				? ( nClick == 0 ? NM_CLICK : NM_DBLCLK )
				: ( nClick == 0 ? NM_RCLICK : NM_RDBLCLK )
				;
		_data.action = 0;
		_data.itemOld.mask = TVIF_CHILDREN|TVIF_HANDLE|TVIF_PARAM|TVIF_STATE;
		_data.itemOld.hItem = hti;
		GetItem( &_data.itemOld );
		CExtSafeString strItemText = GetItemText( hti );
		_data.itemOld.cchTextMax = INT(strItemText.GetLength());
		_data.itemOld.pszText = strItemText.IsEmpty() ? _T("") : LPTSTR(LPCTSTR(strItemText));
		_data.itemOld.mask |= TVIF_TEXT;
		::memcpy( &_data.itemNew, &_data.itemOld, sizeof(TVITEM) );
		_data.ptDrag.x = point.x;
		_data.ptDrag.y = point.y;
		if( ::SendMessage( hWndParent, WM_NOTIFY, WPARAM(nOwnID), LPARAM(&_data) ) != 0 )
			return true;
	}

DWORD dwTreeStyle = TreeGetStyle();
bool bMultiSelection       = ( ( dwTreeStyle & __EXT_TREE_MULTIPLE_SELECTION ) != 0 ) ? true : false;
bool bSelectDisabledItems  = ( ( dwTreeStyle & __EXT_TREE_SELECT_DISABLE_ITEMS ) != 0 ) ? true : false;
bool bFocusDisabledItems   = ( ( dwTreeStyle & __EXT_TREE_FOCUS_DISABLE_ITEMS ) != 0 ) ? true : false;
bool bSelectCollapsedItems = ( ( dwTreeStyle & __EXT_TREE_SELECT_COLLAPSED_ITEMS ) != 0 ) ? true : false;
bool bSubtractSelection    = ( ( dwTreeStyle & __EXT_TREE_SUBTRACT_SELECTION ) != 0 ) ? true : false;
bool bSingleExpand         = ( ( GetStyle() & TVS_SINGLEEXPAND ) != 0 ) ? true : false;
bool bEditLabels		   = ( ( GetStyle() & TVS_EDITLABELS ) != 0 ) ? true : false;
bool bItemIsEnabled        = OnQueryItemEnabledState( hti );
	switch( nMouseButton )
	{
	case VK_LBUTTON:
	{
		static HTREEITEM g_htiLastLButtonDownItem = NULL;
		switch( nClick )
		{
		case 0:
			g_htiLastLButtonDownItem = hti;
// 			if( OnTreeItemDoDragDetect( hti, dwHitTestFlags, nMouseButton, nClick, nMouseEventFlags, point ) )
// 			{
// 				g_htiLastLButtonDownItem = NULL;
// 				return true;
// 			}
		//break;
			// continue falling after drag detection ...
		case 1:
		{
			if( ::GetFocus() != m_hWnd )
				SetFocus();
			if( g_htiLastLButtonDownItem != hti )
				return true;
			if( ( dwHitTestFlags & __EXT_TVHT_ONCONTROL ) != 0 )
			{
				CRect rcControl;
				VERIFY( TreeItemRectGet( hti, rcControl, e_tirt_control ) );
				ASSERT( ! rcControl.IsRectEmpty() );
				if( m_hWndChildControl != NULL )
				{
					CRect rc;
					::GetWindowRect( m_hWndChildControl, &rc );
					if( rc == rcControl )
						return true;
					SendMessage( WM_CANCELMODE );
				}
				m_hWndChildControl = OnInplaceControlCreate( hti, rcControl );
				if( m_hWndChildControl != NULL )
					m_htiInplaceEdited = hti;
				return true;
			}
			if( ( dwHitTestFlags & __EXT_TVHT_ONITEMBUTTON ) != 0 )
			{
				if( nClick != 0 )
					return true;
				//if( ( nMouseEventFlags & (MK_CONTROL|MK_SHIFT) ) != 0 )
				//	return true;
				HTREEITEM htiFocus = GetFocusedItem();
				if( htiFocus != NULL )
				{
					if(		TreeItemIsExpanded( hti )
						&&	HasAncestor( hti, htiFocus )
						)
					{
						if(		( ! bFocusDisabledItems )
							&&	( ! bItemIsEnabled )
							)
							FocusItem( NULL, false, false );
						else
							FocusItem( hti, bSelectDisabledItems || bItemIsEnabled, true );
					}
				}
				SendMessage( WM_CANCELMODE );
				Expand( hti, TVE_TOGGLE );
				Invalidate();
				return true;
			}
			DWORD dwSelectionAreaFlags =
				__EXT_TVHT_ONITEMICON|__EXT_TVHT_ONITEMLABEL
				|__EXT_TVHT_ONCONTROL|__EXT_TVHT_ONCHECKBOX;
			bool bFullRowSelection = ( ( GetStyle() & TVS_FULLROWSELECT ) != 0 ) ? true : false;
			if( bFullRowSelection )
				dwSelectionAreaFlags |= __EXT_TVHT_ONITEMINDENT|__EXT_TVHT_ONITEMRIGHT;
			static HTREEITEM stat_htiLastZeroFocusItem = NULL;
			if(		( dwHitTestFlags & dwSelectionAreaFlags ) != 0
				&&	( bFocusDisabledItems || bItemIsEnabled )
				)
			{
				TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
				
				if(		bEditLabels
					&&	( hti != stat_htiLastZeroFocusItem && hti == GetFocusedItem() )
					&&	(	bFullRowSelection
						||	( dwHitTestFlags & __EXT_TVHT_ONITEMLABEL ) != 0
						)
					)
				{
					if( nClick != 0 )
						stat_htiLastZeroFocusItem = NULL;
					if( nClick != 1 )
						return true;
					if(		(	m_hWndChildControl == NULL
							||	( ! ::IsWindow( m_hWndChildControl ) )
							)
						&&	::GetFocus() == m_hWnd
						&&	m_nLastMouseButtonEventType[0] == 0 // if WM_LBUTTONUP after WM_LBUTTONDOWN, i.e. if not after WM_LBUTTONDBLCLK
						)
					{
						KillTimer( m_nDelayedEditingTimerID );
						m_wndContentExpand.Deactivate();
						SetTimer( m_nDelayedEditingTimerID, ::GetDoubleClickTime() + 1 , NULL );
					}
					return true;
				}
				if( ( dwHitTestFlags & m_dwCheckBoxAreaFlags ) != 0 && OnQueryItemEnabledState( hti ) )
				{
					if( nClick == 1 )
					{
						switch( _TII.m_eCheckBoxType )
						{
						case e_cbt_check_box:
							switch( _TII.m_nCheckBoxState )
							{
							case 0:
								_TII.m_nCheckBoxState = 1;
								break;
							case 1:
								_TII.m_nCheckBoxState = 0;
								break;
							}
						break;
						case e_cbt_tri_state_check_box:
							switch( _TII.m_nCheckBoxState )
							{
							case 0:
								_TII.m_nCheckBoxState = 1;
								break;
							case 1:
								_TII.m_nCheckBoxState = 2;
								break;
							case 2:
								_TII.m_nCheckBoxState = 0;
								break;
							}
						break;
						case e_cbt_radio:
							_CheckRadioBox( hti );
						break;
						} // switch( _TII.m_eCheckBoxType )
					}
				} // if( ( dwHitTestFlags & m_dwCheckBoxAreaFlags ) != 0 && OnQueryItemEnabledState( hti ) )

				HTREEITEM htiFocus = GetFocusedItem();
				if(		bSingleExpand
					&&	nClick == 0
					&&	htiFocus == hti
					&&	( nMouseEventFlags & (MK_CONTROL|MK_SHIFT) ) == 0
					)
				{
					SendMessage( WM_CANCELMODE );
					Expand( hti, TVE_TOGGLE );
					Invalidate();
					if( nClick != 0 )
						stat_htiLastZeroFocusItem = NULL;
					return true;
				}
				bool bSelectFocusedFinally = true;
				if(		bSubtractSelection
					&&	( nMouseEventFlags & (MK_CONTROL|MK_SHIFT) ) == MK_CONTROL
					)
				{
					if( nClick == 1 )
					{
						//bSelectFocusedFinally = ! IsItemSelected( hti );
						AnchorItemSet( hti );
						
						SendMessage( WM_CANCELMODE );
						bool bSelectThisItem = false, bUnselectOtherItems = false;
						if( ( TreeGetStyle() & __EXT_TREE_MULTIPLE_SELECTION ) != 0 )
						{
							FocusItem( hti, bSelectThisItem, bUnselectOtherItems );
							//if( bSelectDisabledItems || bItemIsEnabled )
								SelectItem( hti, !IsItemSelected( hti ), IsItemSelected( hti ) );
						}
						else
						{
							bSelectThisItem = bUnselectOtherItems = true;
							FocusItem( hti, bSelectThisItem, bUnselectOtherItems );
						}
						
						Invalidate();
						if( m_wndContentExpand.GetSafeHwnd() != NULL )
							m_wndContentExpand.Invalidate();
					}
					if( nClick != 0 )
						stat_htiLastZeroFocusItem = NULL;
					return true;
				}

				HTREEITEM htiAnchor = AnchorItemGet();
				if( ( nMouseEventFlags & (MK_CONTROL|MK_SHIFT) ) != MK_SHIFT )
					AnchorItemSet( hti );
				
				if( htiFocus != NULL )
				{
					if(		bMultiSelection
						&&	( nMouseEventFlags & MK_SHIFT ) == MK_SHIFT
						)
					{
						if( bSubtractSelection && htiAnchor != NULL )
							SelectItemRange( htiAnchor, htiFocus, false, !bSelectCollapsedItems, bSelectDisabledItems );
						SelectItemRange( hti, htiAnchor, true, !bSelectCollapsedItems, bSelectDisabledItems );
					}
				}
				bool bUnselectOtherItems =
					(
						( ! bMultiSelection )
					||	( ( nMouseEventFlags & (MK_CONTROL|MK_SHIFT) ) == 0 )
					)
					? true : false;
				SendMessage( WM_CANCELMODE );
				if( ( ! bSelectDisabledItems ) && ( ! bItemIsEnabled ) )
					bSelectFocusedFinally = false;
				if( ! bSelectFocusedFinally )
					SelectItem( hti, false );
				if( nClick == 0 )
					stat_htiLastZeroFocusItem = hti;
				else
					stat_htiLastZeroFocusItem = NULL;
				FocusItem( hti, bSelectFocusedFinally, bUnselectOtherItems, true );
				Invalidate();
				if( m_wndContentExpand.GetSafeHwnd() != NULL )
					m_wndContentExpand.Invalidate();
				if( nClick == 0 && OnTreeItemDoDragDetect( hti, dwHitTestFlags, nMouseButton, nClick, nMouseEventFlags, point ) )
				{
					g_htiLastLButtonDownItem = NULL;
					return true;
				}
				return true;
			}
			if( nClick != 0 )
				stat_htiLastZeroFocusItem = NULL;
		}
		break;
		case 2:
			g_htiLastLButtonDownItem = NULL;
			if(		( dwHitTestFlags & (__EXT_TVHT_ONITEMICON|__EXT_TVHT_ONITEMLABEL) ) != 0
				&&	( bFocusDisabledItems || bItemIsEnabled )
				&&	! bSingleExpand
				)
			{
				if( ::GetFocus() != m_hWnd )
					SetFocus();
				SendMessage( WM_CANCELMODE );
				if( ItemHasChildren( hti ) )
					Expand( hti, TVE_TOGGLE );

				Invalidate();
				return true;
			}
		break;
		} // switch( nClick )
	}
	return true; // case VK_LBUTTON
	case VK_RBUTTON:
	{
		if( ::GetFocus() != m_hWnd )
			SetFocus();
		switch( nClick )
		{
		case 0:
			if( OnTreeItemDoDragDetect( hti, dwHitTestFlags, nMouseButton, nClick, nMouseEventFlags, point ) )
				return true;
		//break;
			// continue falling after drag detection ...
		case 1:
		{
			CPoint ptScreen = point;
			ClientToScreen( &ptScreen );
			SendMessage( WM_CONTEXTMENU, WPARAM(m_hWnd), MAKELPARAM(ptScreen.x,ptScreen.y) );
		}
		return true;
		} // switch( nClick )
	}
	break; // case VK_RBUTTON
	default:
		switch( nClick )
		{
		case 0:
			if( OnTreeItemDoDragDetect( hti, dwHitTestFlags, nMouseButton, nClick, nMouseEventFlags, point ) )
				return true;
		//break;
			// continue falling after drag detection ...
		} // switch( nClick )
	break;
	} // switch( nMouseButton )
//	return false;
	return true;
}

void CExtTreeCtrl::_CheckRadioBox( HTREEITEM hti )
{
	ASSERT_VALID( this );
TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
	if( _TII.m_eCheckBoxType != e_cbt_radio )
		return;
	_TII.m_nCheckBoxState = 1;
	_DeselectRadioBoxesNextPrev( hti, _TII.m_strRadioGroupName );
}

void CExtTreeCtrl::_DeselectRadioBoxesNextPrev(
		HTREEITEM hti,
		CExtSafeString m_strRadioGroupName
		)
{
	ASSERT_VALID( this );

	if( hti == NULL )
		return;

HTREEITEM htiNext = GetNextItem( hti, TVGN_NEXT );
	for( ; htiNext != NULL; htiNext = GetNextItem( htiNext, TVGN_NEXT ) )
	{
		TREEITEMINFO_t & _TII = TreeItemInfoGet( htiNext );
		if( _TII.m_strRadioGroupName != m_strRadioGroupName )
			break;
		_TII.m_nCheckBoxState = 0;
	}

HTREEITEM htiPrev = GetNextItem( hti, TVGN_PREVIOUS );
	for( ; htiPrev != NULL; htiPrev = GetNextItem( htiPrev, TVGN_PREVIOUS ) )
	{
		TREEITEMINFO_t & _TII = TreeItemInfoGet( htiPrev );
		if( _TII.m_strRadioGroupName != m_strRadioGroupName )
			break;
		_TII.m_nCheckBoxState = 0;
	}
}

void CExtTreeCtrl::_SingleExpand( HTREEITEM hti, bool bSingleExpand )
{
	ASSERT_VALID( this );
HTREEITEM htiAnchor = AnchorItemGet();
HTREEITEM htiFind = htiAnchor;
HTREEITEM htiParent = GetNextItem( htiAnchor, TVGN_PARENT );
HTREEITEM htiParentTwo = NULL;
	if( bSingleExpand && ( htiAnchor != NULL ) )
	{
		if( hti == htiAnchor )
		{
			Expand( hti, TVE_TOGGLE );
			return;
		}
		if( ItemHasChildren( htiAnchor ) && TreeItemIsExpanded( htiAnchor ) )
			Expand( htiAnchor, TVE_COLLAPSE );
		if( ItemHasChildren( hti ) )
			Expand( hti, TVE_TOGGLE );
		if( LinearItemIsAfter( htiAnchor, hti ) )
		{
			for( ; htiFind != hti; htiFind = LinearItemGetNext( htiFind ) )
			{
				htiParentTwo = GetNextItem( htiFind, TVGN_PARENT );
				
				Expand( htiParent, TVE_COLLAPSE );

				if( htiParent != htiParentTwo )
					htiParent = GetNextItem( htiParent, TVGN_PARENT );
			}
		}
		if( LinearItemIsBefore( htiAnchor, hti ) )
		{
			for( ; htiFind != hti; htiFind = LinearItemGetPrev( htiFind ) )
			{
				if( ItemHasChildren( htiFind ) && htiParent == htiFind )
				{
					Expand( htiFind, TVE_COLLAPSE );
					htiParent = GetNextItem( htiParent, TVGN_PARENT );
				}
			}
		}
	}
}

void CExtTreeCtrl::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	ASSERT_VALID( this );
	nRepCnt;
HTREEITEM htiOldFocus = GetFocusedItem();

HWND hWndParent = ::GetParent( m_hWnd );
UINT nOwnID = UINT( GetDlgCtrlID() );
NMKEY _data;
	::memset( &_data, 0, sizeof(NMKEY) );
	_data.hdr.hwndFrom = m_hWnd;
	_data.hdr.idFrom = nOwnID;
	_data.hdr.code = NM_KEYDOWN;
	_data.nVKey = nChar;
	_data.uFlags = nFlags;
	if( ::SendMessage( hWndParent, WM_NOTIFY, WPARAM(nOwnID), LPARAM(&_data) ) != 0 )
		return;

HTREEITEM hti = htiOldFocus;
	if( hti == NULL )
	{
		hti = GetRootItem();
		if( hti == NULL )
			return;
		hti = GetNextItem( hti, TVGN_CHILD );
		if( hti == NULL )
			return;
		FocusItem( hti, true, true );
		return;
	}
bool bMultiSelection = ( ( TreeGetStyle() & __EXT_TREE_MULTIPLE_SELECTION ) != 0 ) ? true : false;
bool bShift = ( (::GetAsyncKeyState(VK_SHIFT)&0x8000) != 0 ) ? true : false;
bool bCtrl = ( (::GetAsyncKeyState(VK_CONTROL)&0x8000) != 0 ) ? true : false;
bool bSelectDisabledItems  = ( ( TreeGetStyle() & __EXT_TREE_SELECT_DISABLE_ITEMS ) != 0 ) ? true : false;
bool bFocusDisabledItems   = ( ( TreeGetStyle() & __EXT_TREE_FOCUS_DISABLE_ITEMS ) != 0 ) ? true : false;
bool bSelectCollapsedItems = ( ( TreeGetStyle() & __EXT_TREE_SELECT_COLLAPSED_ITEMS ) != 0 ) ? true : false;
bool bSubtractSelection    = ( ( TreeGetStyle() & __EXT_TREE_SUBTRACT_SELECTION ) != 0 ) ? true : false;
HTREEITEM htiAnchor = AnchorItemGet();
	if( (! bShift) && htiAnchor == NULL )
		AnchorItemSet( htiOldFocus );
HTREEITEM htiRoot = GetRootItem();
	switch( nChar )
	{
	case VK_F2:
			if( ( GetStyle() & TVS_EDITLABELS ) != 0 )
			{
				HTREEITEM htiFocused = GetFocusedItem();
				if( htiFocused != NULL )
				{
					HTREEITEM hti = GetInPlaceEditedItem();
					if( hti != htiFocused )
					{
						KillTimer( m_nDelayedEditingTimerID );
						SendMessage( WM_TIMER, WPARAM( m_nDelayedEditingTimerID ) );
					}
				}
				return;
			}
	break;
	case VK_APPS:
	{
		EnsureVisible( htiOldFocus );
		CRect rcItem;
		GetItemRect( htiOldFocus, &rcItem, TRUE );
		CPoint ptScreen = rcItem.CenterPoint();
		ClientToScreen( &ptScreen );
		SendMessage( WM_CONTEXTMENU, WPARAM(m_hWnd), MAKELPARAM(ptScreen.x,ptScreen.y) );
	}
	return;
	case VK_SPACE:
	{
		if( hti == NULL )
			return;
		TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
		if( ! ( bShift || bCtrl ) )
		{
			if( _TII.m_eCheckBoxType == e_cbt_check_box )
			{
				switch( _TII.m_nCheckBoxState )
				{
				case 0:
					_TII.m_nCheckBoxState = 1;
				break;
				case 1:
					_TII.m_nCheckBoxState = 0;
				break;
				}
			}
			if( _TII.m_eCheckBoxType == e_cbt_tri_state_check_box )
			{
				switch( _TII.m_nCheckBoxState )
				{
				case 0:
					_TII.m_nCheckBoxState = 1;
				break;
				case 1:
					_TII.m_nCheckBoxState = 2;
				break;
				case 2:
					_TII.m_nCheckBoxState = 0;
				break;
				}
			}
		}
		if( bSubtractSelection && bCtrl )
		{
			AnchorItemSet( hti );
			
			SelectItem( hti, !IsItemSelected( hti ), IsItemSelected( hti ) );
			FocusItem( hti );
		}
		Invalidate();
	}
	return;
	case VK_UP:
	{
		hti = LinearItemGetPrev( htiOldFocus, true, bFocusDisabledItems );
		if( hti == NULL )
			return;
		if(	bMultiSelection )
		{
			if( bShift )
			{
				if( bSubtractSelection && htiAnchor != NULL )
				{
					SelectItemRange( htiAnchor, htiOldFocus, false, !bSelectCollapsedItems, bSelectDisabledItems );
					SelectItemRange( hti, htiAnchor, true, !bSelectCollapsedItems, bSelectDisabledItems );
				}
				else
					SelectItemRange( htiOldFocus, hti, true, !bSelectCollapsedItems, bSelectDisabledItems, bSubtractSelection );
				FocusItem( hti, bSelectDisabledItems || OnQueryItemEnabledState(hti), false );
			}
			else
				AnchorItemSet( hti );
			if( bCtrl )
				FocusItem( hti, false, false );
			if( (!bShift) && (!bCtrl) )
				FocusItem( hti, bSelectDisabledItems || OnQueryItemEnabledState(hti), true );
			_CheckRadioBox( hti );
			return;
		}
		else
		{
			FocusItem( hti, bSelectDisabledItems || OnQueryItemEnabledState(hti), true );
			_CheckRadioBox( hti );
		}
	}
	return;
	case VK_DOWN:
	{
		hti = LinearItemGetNext( htiOldFocus, true, bFocusDisabledItems );
		if( hti == NULL )
			return;
		if(	bMultiSelection )
		{
			if( bShift )
			{
				if( bSubtractSelection && htiAnchor != NULL )
				{
					SelectItemRange( htiAnchor, htiOldFocus, false, !bSelectCollapsedItems, bSelectDisabledItems );
					SelectItemRange( hti, htiAnchor, true, !bSelectCollapsedItems, bSelectDisabledItems );
				}
				else
					SelectItemRange( htiOldFocus, hti, true, !bSelectCollapsedItems, bSelectDisabledItems, bSubtractSelection );
				FocusItem( hti, bSelectDisabledItems || OnQueryItemEnabledState(hti), false );
			}
			else
				AnchorItemSet( hti );
			if( bCtrl )
				FocusItem( hti, false, false );
			if( (!bShift) && (!bCtrl) )
				FocusItem( hti, bSelectDisabledItems || OnQueryItemEnabledState(hti), true );
			_CheckRadioBox( hti );
			return;
		}
		else
		{
			FocusItem( hti, bSelectDisabledItems || OnQueryItemEnabledState(hti), true );
			_CheckRadioBox( hti );
		}
	}
	return;
	case VK_LEFT:
	{
		if( bShift || bCtrl )
			return;
		if( TreeItemIsExpanded( hti ) )
		{
			Expand( hti, TVE_COLLAPSE );
			Invalidate();
		}
		else
		{
			hti = GetNextItem( hti, TVGN_PARENT );
			if( hti == NULL )
				return;
			if(		( bFocusDisabledItems )
				||	( OnQueryItemEnabledState( hti ) )
				)
				FocusItem( hti, true, true );
		}
		_CheckRadioBox( hti );
	}
	return;
	case VK_RIGHT:
	{
		if( bShift || bCtrl )
			return;
		if( TreeItemIsExpanded( hti ) )
		{
			hti = GetNextItem( hti, TVGN_CHILD );
			if( hti == NULL )
				return;
			if(		( bFocusDisabledItems )
				||	( OnQueryItemEnabledState( hti ) )
				)
				FocusItem( hti, true, true );
		}
		else
		{
			Expand( hti, TVE_EXPAND );
			Invalidate();
		}
		_CheckRadioBox( hti );
	}
	return;
	case VK_HOME:
		if( htiRoot != NULL )
		{
			HTREEITEM htiNewFocus = htiRoot;
			if( htiNewFocus == NULL )
				return;
			if( ! bFocusDisabledItems )
			{
				for( ; ! OnQueryItemEnabledState( htiNewFocus ); )
				{
					htiNewFocus =
						LinearItemFromItem(
							htiNewFocus,
							1,
							NULL,
							true,
							false
							);
					if( htiNewFocus == NULL )
						return;
				}
			}
			if(	bMultiSelection )
			{
				if( bShift )
				{
					if( bSubtractSelection && htiAnchor != NULL )
					{
						SelectItemRange( htiAnchor, htiOldFocus, false, !bSelectCollapsedItems, bSelectDisabledItems );
						SelectItemRange( htiNewFocus, htiAnchor, true, !bSelectCollapsedItems, bSelectDisabledItems );
					}
					else
						SelectItemRange( htiOldFocus, htiNewFocus, true, !bSelectCollapsedItems, bSelectDisabledItems, bSubtractSelection );
					FocusItem( htiNewFocus, bSelectDisabledItems || OnQueryItemEnabledState(htiNewFocus), false );
				}
				else
					AnchorItemSet( htiNewFocus );
				if( bCtrl )
					FocusItem( htiNewFocus, false, false );
				if( (!bShift) && (!bCtrl) )
					FocusItem( htiNewFocus, bSelectDisabledItems || OnQueryItemEnabledState(htiNewFocus), true );
				_CheckRadioBox( hti );
			}
			else
			{
				FocusItem( htiNewFocus, bSelectDisabledItems || OnQueryItemEnabledState(htiNewFocus), true );
				_CheckRadioBox( hti );
			}
			EnsureVisible( htiNewFocus );
			Invalidate();
		}
	return;
	case VK_END:
		if( htiRoot != NULL )
		{
			HTREEITEM htiNewFocus = htiRoot;
			if( htiNewFocus == NULL )
				return;
			HTREEITEM hti2 = htiRoot;
			for( ; hti2 != NULL;  )
			{
				htiNewFocus = hti2;
				HTREEITEM hti3 = GetNextItem( hti2, TVGN_NEXT );
				if( hti3 != NULL )
				{
					hti2 = hti3;
					continue;
				}
				if( TreeItemIsExpanded( hti2 ) )
				{
					hti2 = GetNextItem( hti2, TVGN_CHILD );
					continue;
				}
				else
					break;
			}
			if( htiNewFocus == NULL )
				return;
			if( ! bFocusDisabledItems )
			{
				for( ; ! OnQueryItemEnabledState( htiNewFocus ); )
				{
					htiNewFocus =
						LinearItemFromItem(
							htiNewFocus,
							-1,
							NULL,
							true,
							false
							);
					if( htiNewFocus == NULL )
						return;
				}
			}
			if(	bMultiSelection )
			{
				if( bShift )
				{
					if( bSubtractSelection && htiAnchor != NULL )
					{
						SelectItemRange( htiAnchor, htiOldFocus, false, !bSelectCollapsedItems, bSelectDisabledItems );
						SelectItemRange( htiNewFocus, htiAnchor, true, !bSelectCollapsedItems, bSelectDisabledItems );
					}
					else
						SelectItemRange( htiOldFocus, htiNewFocus, true, !bSelectCollapsedItems, bSelectDisabledItems, bSubtractSelection );
					FocusItem( htiNewFocus, bSelectDisabledItems || OnQueryItemEnabledState(htiNewFocus), false );
				}
				else
					AnchorItemSet( htiNewFocus );
				if( bCtrl )
					FocusItem( htiNewFocus, false, false );
				if( (!bShift) && (!bCtrl) )
					FocusItem( htiNewFocus, bSelectDisabledItems || OnQueryItemEnabledState(htiNewFocus), true );
				_CheckRadioBox( hti );
			}
			else
			{
				FocusItem( htiNewFocus, bSelectDisabledItems || OnQueryItemEnabledState(htiNewFocus), true );
				_CheckRadioBox( hti );
			}
			EnsureVisible( htiNewFocus );
			Invalidate();
		}
	return;
	case VK_PRIOR:
	case VK_NEXT:
	{
		bool bLastIsPartiallyVisible = false;
		INT nPageSize = TreeGetDisplayedItemCount( &bLastIsPartiallyVisible );
		if( bLastIsPartiallyVisible && nPageSize > 1 )
			nPageSize --;
		HTREEITEM htiNewFocus =
			LinearItemFromItem(
				hti,
				( nChar == VK_PRIOR ) ? ( - nPageSize ) : nPageSize
				);
		if( htiNewFocus != NULL && htiNewFocus != hti )
		{
			if( ! bFocusDisabledItems )
			{
				for( ; ! OnQueryItemEnabledState( htiNewFocus ); )
				{
					htiNewFocus =
						LinearItemFromItem(
							htiNewFocus,
							( nChar == VK_PRIOR ) ? -1 : 1,
							NULL,
							true,
							false
							);
					if( htiNewFocus == NULL )
						return;
				}
			}
			if(	bMultiSelection )
			{
				if( bShift )
				{
					if( bSubtractSelection && htiAnchor != NULL )
					{
						SelectItemRange( htiAnchor, htiOldFocus, false, !bSelectCollapsedItems, bSelectDisabledItems );
						SelectItemRange( htiNewFocus, htiAnchor, true, !bSelectCollapsedItems, bSelectDisabledItems );
					}
					else
						SelectItemRange( htiOldFocus, htiNewFocus, true, !bSelectCollapsedItems, bSelectDisabledItems, bSubtractSelection );
					FocusItem( htiNewFocus, bSelectDisabledItems, false );
				}
				else
					AnchorItemSet( htiNewFocus );
				if( bCtrl )
					FocusItem( htiNewFocus, false, false );
				if( (!bShift) && (!bCtrl) )
					FocusItem( htiNewFocus, bSelectDisabledItems, true );

				_CheckRadioBox( hti );
			}
			else
			{
				FocusItem( htiNewFocus, bSelectDisabledItems, true );
				_CheckRadioBox( hti );
			}
			EnsureVisible( htiNewFocus );
			Invalidate();
		}
	}
	return;
//	case VK_TAB:
//		if( ( GetStyle() & WS_TABSTOP ) != 0 )
//			GetParent()->SendMessage(
//				WM_NEXTDLGCTL,
//				CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT) ? 1 : 0,
//				0
//				);
//	return;
	case VK_BACK:
		{
			HTREEITEM hti = GetFocusedItem();
			if( hti != NULL )
			{
				hti = GetNextItem( hti, TVGN_PARENT );
				if( hti != NULL )
					FocusItem( hti, true, true, false );
			}
		}
	return;
	case VK_RETURN:
	case VK_ESCAPE:
		if(		(!(	CExtPopupMenuWnd::IsKeyPressed( VK_MENU )
				||	CExtPopupMenuWnd::IsKeyPressed( VK_CONTROL )
				||	CExtPopupMenuWnd::IsKeyPressed( VK_SHIFT )
				))
			&&	IsWindowEnabled()
			&&	( GetStyle() & WS_VISIBLE ) != 0
			)
		{
			UINT nCmdID = ( nChar == VK_RETURN ) ? IDOK : IDCANCEL;
			bool bSendOkCancel = true;
			CWnd * pWndParent = GetParent();
			for(	;	pWndParent != NULL
					&&	( pWndParent->GetStyle() & WS_CHILD ) != 0
					;
				)
			{
				if(		( ! pWndParent->IsWindowEnabled() )
					||	( pWndParent->GetStyle() & WS_VISIBLE ) == 0
					)
				{
					bSendOkCancel = false;
					break;
				}
				pWndParent = pWndParent->GetParent();
			}
			if( bSendOkCancel && pWndParent != NULL )
			{
				CWnd * pWndOkCancel = pWndParent->GetDlgItem( nCmdID );
				if(		pWndOkCancel != NULL
					&&	(	(! pWndOkCancel->IsWindowEnabled() )
						||	( pWndOkCancel->GetStyle() & WS_VISIBLE ) == 0
						)
					)
					bSendOkCancel = false;
				else if( ( pWndParent->GetStyle() & WS_CHILD ) == 0 )
				{
					MENUITEMINFO _mii;
					::memset( &_mii, 0, sizeof(MENUITEMINFO) );
					_mii.cbSize = sizeof(MENUITEMINFO);
					_mii.fMask = MIIM_STATE;
					CMenu * pMenu = pWndParent->GetSystemMenu( FALSE );
					if(		pMenu->GetSafeHmenu() != NULL
						&&	pMenu->GetMenuItemInfo( SC_CLOSE, &_mii, FALSE )
						&&	( _mii.fState & ( MF_DISABLED | MF_GRAYED ) ) != 0
						)
						bSendOkCancel = false;
				} // else if( ( pWndParent->GetStyle() & WS_CHILD ) == 0 )
			} // if( bSendOkCancel && pWndParent != NULL )
			if( bSendOkCancel )
			{
				pWndParent->SendMessage( WM_COMMAND, WPARAM(nCmdID) );
				return;
			}
		}
	break;
	} // switch( nChar )
//	CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

//void CExtTreeCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	ASSERT_VALID( this );
//	//CTreeCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
//}

HTREEITEM CExtTreeCtrl::_LinearItemGetNext(
	HTREEITEM htiFrom,
	bool bExpandedOnly // = true
	) const
{
	ASSERT_VALID( this );
	if( ( ! bExpandedOnly ) || TreeItemIsExpanded( htiFrom ) )
	{
		HTREEITEM htiChild = GetNextItem( htiFrom, TVGN_CHILD );
		if ( htiChild != NULL )
			return htiChild;
	}
HTREEITEM htiNext = GetNextItem( htiFrom, TVGN_NEXT );
	if( htiNext != NULL )
		return htiNext;
HTREEITEM htiParent = _GetNextParent( htiFrom );
	htiNext = GetNextItem( htiParent, TVGN_NEXT );
	return htiNext;
}

HTREEITEM CExtTreeCtrl::_LinearItemGetPrev(
	HTREEITEM htiFrom,
	bool bExpandedOnly // = true
	) const
{
	ASSERT_VALID( this );
HTREEITEM htiPrevious = GetNextItem( htiFrom, TVGN_PREVIOUS );
	if( htiPrevious != NULL )
		return _GetPreviousItem( htiPrevious, bExpandedOnly );
HTREEITEM htiParent = GetNextItem( htiFrom, TVGN_PARENT );
	return htiParent;
}

HTREEITEM CExtTreeCtrl::LinearItemGetNext(
	HTREEITEM htiFrom,
	bool bExpandedOnly, // = true
	bool bIncludeDisabled // = true
	) const
{
	ASSERT_VALID( this );
HTREEITEM hti = _LinearItemGetNext( htiFrom, bExpandedOnly );
	if( hti == NULL )
		return NULL;
	if( ! bIncludeDisabled )
	{
		for( ; ! OnQueryItemEnabledState(hti ); )
		{
			hti = _LinearItemGetNext( hti, bExpandedOnly );
			if( hti == NULL )
				break;
		}
	}
	return hti;
}

HTREEITEM CExtTreeCtrl::LinearItemGetPrev(
	HTREEITEM htiFrom,
	bool bExpandedOnly, // = true
	bool bIncludeDisabled // = true
	) const
{
	ASSERT_VALID( this );
HTREEITEM hti = _LinearItemGetPrev( htiFrom, bExpandedOnly );
	if( hti == NULL )
		return NULL;
	if( ! bIncludeDisabled )
	{
		for( ; ! OnQueryItemEnabledState(hti ); )
		{
			hti = _LinearItemGetPrev( hti, bExpandedOnly );
			if( hti == NULL )
				break;
		}
	}
	return hti;
}

HTREEITEM CExtTreeCtrl::_GetNextParent( HTREEITEM hti ) const
{
	ASSERT_VALID( this );
HTREEITEM htiParent = GetNextItem( hti, TVGN_PARENT );
	if( htiParent == NULL )
		return NULL;
HTREEITEM htiNextParent = GetNextItem( htiParent, TVGN_NEXT );
	if( htiNextParent == NULL )
	{
		HTREEITEM htiUpParent = _GetNextParent( htiParent );
		return htiUpParent;
	}
	return htiParent;
}

HTREEITEM CExtTreeCtrl::_GetPreviousItem( HTREEITEM hti, bool bExpandedOnly ) const
{
	ASSERT_VALID( this );
	if( bExpandedOnly && ( ! TreeItemIsExpanded( hti ) ) )
		return hti;
HTREEITEM htiChild = GetNextItem( hti, TVGN_CHILD );
	if( htiChild == NULL )
		return hti;
HTREEITEM htiLastChild = _GetLastChildItem( htiChild );
	return _GetPreviousItem( htiLastChild, bExpandedOnly );
}

HTREEITEM CExtTreeCtrl::_GetLastChildItem( HTREEITEM hti ) const
{
	ASSERT_VALID( this );
HTREEITEM htiNext = GetNextItem( hti, TVGN_NEXT );
	if( htiNext == NULL )
		return hti;
	return _GetLastChildItem( htiNext );
}

bool CExtTreeCtrl::LinearItemIsBefore(
	HTREEITEM htiFrom,
	HTREEITEM htiBefore,
	bool bExpandedOnly // = true
	) const
{
	ASSERT_VALID( this );
	if( htiFrom == NULL || htiBefore == NULL || htiFrom == htiBefore )
		return false;
HTREEITEM hti = LinearItemGetPrev( htiFrom, bExpandedOnly );
	for( ; hti != NULL; hti = LinearItemGetPrev( hti, bExpandedOnly ) )
	{
		if( hti == htiBefore )
			return true;
	}
	return false;
}

bool CExtTreeCtrl::LinearItemIsAfter(
	HTREEITEM htiFrom,
	HTREEITEM htiAfter,
	bool bExpandedOnly // = true
	) const
{
	ASSERT_VALID( this );
	if( htiFrom == NULL || htiAfter == NULL || htiFrom == htiAfter )
		return false;
HTREEITEM hti = LinearItemGetNext( htiFrom, bExpandedOnly );
	for( ; hti != NULL; hti = LinearItemGetNext( hti, bExpandedOnly ) )
	{
		if( hti == htiAfter )
			return true;
	}
	return false;
}

HTREEITEM CExtTreeCtrl::LinearItemFromItem(
	HTREEITEM htiFrom,
	INT nOffset,
	INT * p_nResultShift, // = NULL
	bool bExpandedOnly, // = true
	bool bIncludeDisabled // = true
	)
{
	ASSERT_VALID( this );
	if( p_nResultShift != NULL )
		(*p_nResultShift) = 0;
	if( htiFrom == NULL || nOffset == 0 )
		return htiFrom;
HTREEITEM hti = htiFrom;
HTREEITEM htiResult = htiFrom;
INT nPassedOffset = 0;
	for( ; nPassedOffset != nOffset; )
	{
		if( nOffset > 0 )
			hti = LinearItemGetNext( hti, bExpandedOnly, bIncludeDisabled );
		else
			hti = LinearItemGetPrev( hti, bExpandedOnly, bIncludeDisabled );
		if( hti == NULL )
			break;
		htiResult = hti;
		if( nOffset > 0 )
			nPassedOffset ++;
		else
			nPassedOffset --;
	}
	if( p_nResultShift != NULL )
		(*p_nResultShift) = nPassedOffset;
	return htiResult;
}

DWORD CExtTreeCtrl::TreeGetStyle() const
{
	ASSERT_VALID( this );
	return m_dwTreeStyle;
}

void CExtTreeCtrl::TreeModifyStyle(
	DWORD dwStyleAdd,
	DWORD dwStyleRemove // = 0
	)
{
	ASSERT_VALID( this );
	m_dwTreeStyle &= ~dwStyleRemove;
	m_dwTreeStyle |= dwStyleAdd;
}

DWORD CExtTreeCtrl::TreeGetStyleEx() const
{
	ASSERT_VALID( this );
	return m_dwTreeStyleEx;
}

void CExtTreeCtrl::TreeModifyStyleEx(
	DWORD dwStyleExAdd,
	DWORD dwStyleExRemove // = 0
	)
{
	ASSERT_VALID( this );
	m_dwTreeStyleEx &= ~dwStyleExRemove;
	m_dwTreeStyleEx |= dwStyleExAdd;
}

INT CExtTreeCtrl::TreeGetDisplayedItemCount(
	bool * p_bLastIsPartiallyVisible // = NULL
	) const
{
	ASSERT_VALID( this );
	if( p_bLastIsPartiallyVisible != NULL )
		(*p_bLastIsPartiallyVisible) = false;
HTREEITEM htiFirst = GetNextItem( NULL, TVGN_FIRSTVISIBLE );
HTREEITEM hti = htiFirst;
INT nCount = 0;
CRect rcClient;
	GetClientRect( &rcClient );
	for( ; hti != NULL; )
	{
		CRect rcItemLabel;
		TreeItemRectGet( hti, rcItemLabel, e_tirt_label );
		if( rcItemLabel.top > rcClient.bottom )
			break;
		if(		p_bLastIsPartiallyVisible != NULL
			&&	rcItemLabel.top < rcClient.bottom
			&&	rcItemLabel.bottom > rcClient.bottom
			)
			(*p_bLastIsPartiallyVisible) = true;
		nCount ++;
		hti = GetNextItem( hti, TVGN_NEXTVISIBLE );
	}
	return nCount;
}

HTREEITEM CExtTreeCtrl::AnchorItemGet() const
{
	ASSERT_VALID(this);
	return m_htiAnchorItem;
}

void CExtTreeCtrl::AnchorItemSet( HTREEITEM hti )
{
	ASSERT_VALID(this);
	m_htiAnchorItem = hti;
}

void CExtTreeCtrl::OnSetFocus( CWnd* pOldWnd )
{
	CTreeCtrl::OnSetFocus(pOldWnd);
	Invalidate();
	UpdateWindow();
	SendMessage( WM_NCPAINT );
}

void CExtTreeCtrl::OnKillFocus( CWnd* pNewWnd )
{
	CTreeCtrl::OnKillFocus(pNewWnd);
	Invalidate();
	UpdateWindow();
	SendMessage( WM_NCPAINT );
	m_wndContentExpand.Deactivate();
}

bool CExtTreeCtrl::MultipleSelectionGet() const
{
	ASSERT_VALID(this);
	return ( ( TreeGetStyle() & __EXT_TREE_MULTIPLE_SELECTION ) != 0 ) ? true : false;
}

void CExtTreeCtrl::MultipleSelectionSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( TreeGetStyle() & __EXT_TREE_MULTIPLE_SELECTION ) == 0 )
			TreeModifyStyle( __EXT_TREE_MULTIPLE_SELECTION, 0 );
	}
	else
	{
		if( ( TreeGetStyle() & __EXT_TREE_MULTIPLE_SELECTION ) != 0 )
			TreeModifyStyle( 0, __EXT_TREE_MULTIPLE_SELECTION );
	}
}

bool CExtTreeCtrl::SelectDisabledItemsGet() const
{
	ASSERT_VALID(this);
	return ( ( TreeGetStyle() & __EXT_TREE_SELECT_DISABLE_ITEMS ) != 0 ) ? true : false;
}

void CExtTreeCtrl::SelectDisabledItemsSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( TreeGetStyle() & __EXT_TREE_SELECT_DISABLE_ITEMS ) == 0 )
			TreeModifyStyle( __EXT_TREE_SELECT_DISABLE_ITEMS, 0 );
	}
	else
	{
		if( ( TreeGetStyle() & __EXT_TREE_SELECT_DISABLE_ITEMS ) != 0 )
			TreeModifyStyle( 0, __EXT_TREE_SELECT_DISABLE_ITEMS );
	}
}

bool CExtTreeCtrl::FocusDisabledItemsGet() const
{
	ASSERT_VALID(this);
	return ( ( TreeGetStyle() & __EXT_TREE_FOCUS_DISABLE_ITEMS ) != 0 ) ? true : false;
}

void CExtTreeCtrl::FocusDisabledItemsSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( TreeGetStyle() & __EXT_TREE_FOCUS_DISABLE_ITEMS ) == 0 )
			TreeModifyStyle( __EXT_TREE_FOCUS_DISABLE_ITEMS, 0 );
	}
	else
	{
		if( ( TreeGetStyle() & __EXT_TREE_FOCUS_DISABLE_ITEMS ) != 0 )
			TreeModifyStyle( 0, __EXT_TREE_FOCUS_DISABLE_ITEMS );
	}
}

bool CExtTreeCtrl::SelectCollapsedItemsGet() const
{
	ASSERT_VALID(this);
	return ( ( TreeGetStyle() & __EXT_TREE_SELECT_COLLAPSED_ITEMS ) != 0 ) ? true : false;
}

void CExtTreeCtrl::SelectCollapsedItemsSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( TreeGetStyle() & __EXT_TREE_SELECT_COLLAPSED_ITEMS ) == 0 )
			TreeModifyStyle( __EXT_TREE_SELECT_COLLAPSED_ITEMS, 0 );
	}
	else
	{
		if( ( TreeGetStyle() & __EXT_TREE_SELECT_COLLAPSED_ITEMS ) != 0 )
			TreeModifyStyle( 0, __EXT_TREE_SELECT_COLLAPSED_ITEMS );
	}
}

bool CExtTreeCtrl::SubtractSelectionGet() const
{
	ASSERT_VALID(this);
	return ( ( TreeGetStyle() & __EXT_TREE_SUBTRACT_SELECTION ) != 0 ) ? true : false;
}

void CExtTreeCtrl::SubtractSelectionSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( TreeGetStyle() & __EXT_TREE_SUBTRACT_SELECTION ) == 0 )
			TreeModifyStyle( __EXT_TREE_SUBTRACT_SELECTION, 0 );
	}
	else
	{
		if( ( TreeGetStyle() & __EXT_TREE_SUBTRACT_SELECTION ) != 0 )
			TreeModifyStyle( 0, __EXT_TREE_SUBTRACT_SELECTION );
	}
}

bool CExtTreeCtrl::FullRowSelectionGet() const
{
	ASSERT_VALID(this);
	return ( ( GetStyle() & TVS_FULLROWSELECT ) != 0 ) ? true : false;
}

void CExtTreeCtrl::FullRowSelectionSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( GetStyle() & TVS_FULLROWSELECT ) == 0 )
			ModifyStyle( 0, TVS_FULLROWSELECT );
	}
	else
	{
		if( ( GetStyle() & TVS_FULLROWSELECT ) != 0 )
			ModifyStyle( TVS_FULLROWSELECT, 0 );
	}
}

bool CExtTreeCtrl::HasButtonsGet() const
{
	ASSERT_VALID(this);
	return ( ( GetStyle() & TVS_HASBUTTONS ) != 0 ) ? true : false;
}

void CExtTreeCtrl::HasButtonsSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( GetStyle() & TVS_HASBUTTONS ) == 0 )
			ModifyStyle( 0, TVS_HASBUTTONS );
	}
	else
	{
		if( ( GetStyle() & TVS_HASBUTTONS ) != 0 )
			ModifyStyle( TVS_HASBUTTONS, 0 );
	}
}

bool CExtTreeCtrl::HasLinesGet() const
{
	ASSERT_VALID(this);
	return ( ( GetStyle() & TVS_HASLINES ) != 0 ) ? true : false;
}

void CExtTreeCtrl::HasLinesSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( GetStyle() & TVS_HASLINES ) == 0 )
			ModifyStyle( 0, TVS_HASLINES );
	}
	else
	{
		if( ( GetStyle() & TVS_HASLINES ) != 0 )
			ModifyStyle( TVS_HASLINES, 0 );
	}
}

bool CExtTreeCtrl::LinesAtRootGet() const
{
	ASSERT_VALID(this);
	return ( ( GetStyle() & TVS_LINESATROOT ) != 0 ) ? true : false;
}

void CExtTreeCtrl::LinesAtRootSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( GetStyle() & TVS_LINESATROOT ) == 0 )
			ModifyStyle( 0, TVS_LINESATROOT );
	}
	else
	{
		if( ( GetStyle() & TVS_LINESATROOT ) != 0 )
			ModifyStyle( TVS_LINESATROOT, 0 );
	}
}

bool CExtTreeCtrl::ShowSelectionAlwaysGet() const
{
	ASSERT_VALID(this);
	return ( ( GetStyle() & TVS_SHOWSELALWAYS ) != 0 ) ? true : false;
}

void CExtTreeCtrl::ShowSelectionAlwaysSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( GetStyle() & TVS_SHOWSELALWAYS ) == 0 )
			ModifyStyle( 0, TVS_SHOWSELALWAYS );
	}
	else
	{
		if( ( GetStyle() & TVS_SHOWSELALWAYS ) != 0 )
			ModifyStyle( TVS_SHOWSELALWAYS, 0 );
	}
}

bool CExtTreeCtrl::SingleExpandGet() const
{
	ASSERT_VALID(this);
	return ( ( GetStyle() & TVS_SINGLEEXPAND ) != 0 ) ? true : false;
}

void CExtTreeCtrl::SingleExpandSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( GetStyle() & TVS_SINGLEEXPAND ) == 0 )
			ModifyStyle( 0, TVS_SINGLEEXPAND );
	}
	else
	{
		if( ( GetStyle() & TVS_SINGLEEXPAND ) != 0 )
			ModifyStyle( TVS_SINGLEEXPAND, 0 );
	}
}

bool CExtTreeCtrl::TrackSelectionGet() const
{
	ASSERT_VALID(this);
	return ( ( GetStyle() & TVS_TRACKSELECT ) != 0 ) ? true : false;
}

void CExtTreeCtrl::TrackSelectionSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( GetStyle() & TVS_TRACKSELECT ) == 0 )
			ModifyStyle( 0, TVS_TRACKSELECT );
	}
	else
	{
		if( ( GetStyle() & TVS_TRACKSELECT ) != 0 )
			ModifyStyle( TVS_TRACKSELECT, 0 );
	}
}

//bool CExtTreeCtrl::CheckBoxesGet() const
//{
//	ASSERT_VALID(this);
//	return ( ( GetStyle() & TVS_CHECKBOXES ) != 0 ) ? true : false;
//}

//void CExtTreeCtrl::CheckBoxesSet( bool bSet /*= true*/ )
//{
//	ASSERT_VALID(this);
//	if( bSet )
//	{
//		if( ( GetStyle() & TVS_CHECKBOXES ) == 0 )
//			ModifyStyle( 0, TVS_CHECKBOXES );
//	}
//	else
//	{
//		if( ( GetStyle() & TVS_CHECKBOXES ) != 0 )
//			ModifyStyle( TVS_CHECKBOXES, 0 );
//	}
//}

bool CExtTreeCtrl::HoverDisabledItemsGet() const
{
	ASSERT_VALID(this);
	return ( ( TreeGetStyle() & __EXT_TREE_HOVER_DISABLE_ITEMS ) != 0 ) ? true : false;
}

void CExtTreeCtrl::HoverDisabledItemsSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( TreeGetStyle() & __EXT_TREE_HOVER_DISABLE_ITEMS ) == 0 )
			TreeModifyStyle( __EXT_TREE_HOVER_DISABLE_ITEMS, 0 );
	}
	else
	{
		if( ( TreeGetStyle() & __EXT_TREE_HOVER_DISABLE_ITEMS ) != 0 )
			TreeModifyStyle( 0, __EXT_TREE_HOVER_DISABLE_ITEMS );
	}
}

bool CExtTreeCtrl::AutoDisableChildrenItemsGet() const
{
	ASSERT_VALID(this);
	return ( ( TreeGetStyle() & __EXT_TREE_AUTO_DISABLE_ITEMS_BY_PARENT ) != 0 ) ? true : false;
}

void CExtTreeCtrl::AutoDisableChildrenItemsSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( TreeGetStyle() & __EXT_TREE_AUTO_DISABLE_ITEMS_BY_PARENT ) == 0 )
			TreeModifyStyle( __EXT_TREE_AUTO_DISABLE_ITEMS_BY_PARENT, 0 );
	}
	else
	{
		if( ( TreeGetStyle() & __EXT_TREE_AUTO_DISABLE_ITEMS_BY_PARENT ) != 0 )
			TreeModifyStyle( 0, __EXT_TREE_AUTO_DISABLE_ITEMS_BY_PARENT );
	}
}

bool CExtTreeCtrl::HoverTreeBoxesGet() const
{
	ASSERT_VALID(this);
	return ( ( TreeGetStyleEx() & __EXT_TREE_EX_HOVER_TREE_BOXES ) != 0 ) ? true : false;
}

void CExtTreeCtrl::HoverTreeBoxesSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( TreeGetStyleEx() & __EXT_TREE_EX_HOVER_TREE_BOXES ) == 0 )
			TreeModifyStyleEx( __EXT_TREE_EX_HOVER_TREE_BOXES, NULL );
	}
	else
	{
		if( ( TreeGetStyleEx() & __EXT_TREE_EX_HOVER_TREE_BOXES ) != 0 )
			TreeModifyStyleEx( NULL, __EXT_TREE_EX_HOVER_TREE_BOXES );
	}
}

bool CExtTreeCtrl::HoverCheckBoxesGet() const
{
	ASSERT_VALID(this);
	return ( ( TreeGetStyleEx() & __EXT_TREE_EX_HOVER_CHECK_BOXES ) != 0 ) ? true : false;
}

void CExtTreeCtrl::HoverCheckBoxesSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( TreeGetStyleEx() & __EXT_TREE_EX_HOVER_CHECK_BOXES ) == 0 )
			TreeModifyStyleEx( __EXT_TREE_EX_HOVER_CHECK_BOXES, NULL );
	}
	else
	{
		if( ( TreeGetStyleEx() & __EXT_TREE_EX_HOVER_CHECK_BOXES ) != 0 )
			TreeModifyStyleEx( NULL, __EXT_TREE_EX_HOVER_CHECK_BOXES );
	}
}

bool CExtTreeCtrl::HoverRadioBoxesGet() const
{
	ASSERT_VALID(this);
	return ( ( TreeGetStyleEx() & __EXT_TREE_EX_HOVER_RADIO_BUTTONS ) != 0 ) ? true : false;
}

void CExtTreeCtrl::HoverRadioBoxesSet( bool bSet /*= true*/ )
{
	ASSERT_VALID(this);
	if( bSet )
	{
		if( ( TreeGetStyleEx() & __EXT_TREE_EX_HOVER_RADIO_BUTTONS ) == 0 )
			TreeModifyStyleEx( __EXT_TREE_EX_HOVER_RADIO_BUTTONS, NULL );
	}
	else
	{
		if( ( TreeGetStyleEx() & __EXT_TREE_EX_HOVER_RADIO_BUTTONS ) != 0 )
			TreeModifyStyleEx( NULL, __EXT_TREE_EX_HOVER_RADIO_BUTTONS );
	}
}

INT CExtTreeCtrl::OnTreeItemCalcCheckBoxAreaWidth() const
{
	ASSERT_VALID(this);
INT nLevelIndent = (INT)GetIndent();
INT nCheckBoxWidth = max( nLevelIndent, 16 );
	return nCheckBoxWidth;
}

bool CExtTreeCtrl::TreeItemRectsGet(
	HTREEITEM hti,
	CRect * pRectEntire,
	CRect * pRectLinesArea, // = NULL
	CRect * pRectExpandCollapseButton, // = NULL
	CRect * pRectCheckBox, // = NULL
	CRect * pRectIcon, // = NULL
	CRect * pRectLabel, // = NULL
	CRect * pRectControl, // = NULL
	CRect * pRectRightPart // = NULL
	) const
{
	ASSERT_VALID(this);
CRect rcDefItemLabel;
	if( ! CTreeCtrl::GetItemRect( hti, &rcDefItemLabel, true ) )
		return false;
CRect rcClient;
	GetClientRect( &rcClient );
	((CWnd*)this)->CWnd::RepositionBars( 0, 0xFFFF, 0, CWnd::reposQuery, &rcClient, &rcClient );
CRect rcEntire(
		rcClient.left,
		rcDefItemLabel.top,
		rcClient.right,
		rcDefItemLabel.bottom
		);
	if( pRectEntire != NULL )
		(*pRectEntire) = rcEntire;
CRect rcLinesArea(
		rcClient.left,
		rcDefItemLabel.top - 1,
		rcDefItemLabel.left, // - nLevelIndent,
		rcDefItemLabel.bottom + 1
		);
	if( pRectLinesArea != NULL )
		(*pRectLinesArea) = rcLinesArea;
INT nLevelIndent = (INT)GetIndent();
CRect rcTreeBox(
		rcDefItemLabel.left - nLevelIndent,
		rcDefItemLabel.top - 1,
		rcDefItemLabel.left,
		rcDefItemLabel.bottom + 1
		);
	if( pRectExpandCollapseButton != NULL )
		(*pRectExpandCollapseButton) = rcTreeBox;
const CExtCmdIcon * pIcon = OnTreeItemQueryIcon( hti );
const TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
INT		_marginCheckBoxLeft			= ( ( _TII.m_eCheckBoxType != e_cbt_none ) ? MarginCheckBoxLeftGet() : 0 ),
		_marginCheckBoxRight		= ( ( _TII.m_eCheckBoxType != e_cbt_none ) ? MarginCheckBoxRightGet() : 0 ),
		_marginIconLeft				= ( ( pIcon != NULL ) ? MarginIconLeftGet() : 0 ),
		_marginIconRight			= ( ( pIcon != NULL ) ? MarginIconRightGet() : 0 ),
		_marginControlLabelLeft		= ( ( _TII.m_nControlWidth > 0 && ( ! _TII.m_strEditText.IsEmpty() ) ) ? MarginControlLabelLeftGet() : 0 ),
		_marginControlLabelRight	= ( ( _TII.m_nControlWidth > 0 && ( ! _TII.m_strEditText.IsEmpty() ) ) ? MarginControlLabelRightGet() : 0 );
CRect rcCheckBox(
		rcDefItemLabel.left + _marginCheckBoxLeft,
		rcDefItemLabel.top,
		rcDefItemLabel.left + _marginCheckBoxLeft,
		rcDefItemLabel.bottom
		);
	if( _TII.m_eCheckBoxType != e_cbt_none )
	{
		INT nCheckBoxWidth = OnTreeItemCalcCheckBoxAreaWidth();
		rcCheckBox.right += nCheckBoxWidth;
		if( pRectCheckBox != NULL )
			(*pRectCheckBox) = rcCheckBox;
	}
CRect rcIcon(
		rcCheckBox.right + _marginIconLeft + _marginCheckBoxRight,
		rcCheckBox.top,
		rcCheckBox.right + _marginIconLeft + _marginCheckBoxRight,
		rcCheckBox.bottom
		);
	if( pIcon != NULL )
	{
		CSize _sizeIcon = pIcon->GetSize();
		rcIcon.right += _sizeIcon.cx;
		if( pRectIcon != NULL )
			(*pRectIcon) = rcIcon;
	}
CRect rcLabel(
		rcIcon.right + MarginLabelLeftGet() + ( ( pIcon != NULL ) ? _marginIconRight : _marginCheckBoxRight ),
		rcIcon.top,
		rcIcon.right + TreeItemCalcLabelWidth( hti ) + MarginLabelLeftGet() + ( ( pIcon != NULL ) ? _marginIconRight : _marginCheckBoxRight ),
		rcIcon.bottom
		);
	if( pRectLabel != NULL )
		(*pRectLabel) = rcLabel;
CRect rcControl(
		rcLabel.right + _marginControlLabelLeft + MarginLabelRightGet(),
		rcLabel.top,
		rcLabel.right + _marginControlLabelLeft + MarginLabelRightGet(),
		rcLabel.bottom + __EXT_TREE_CTRL_BORDER_GAP
		);
	if( _TII.m_nControlWidth > 0 )
		rcControl.right += _TII.m_nControlWidth; //  + 6
	if( pRectControl != NULL )
		(*pRectControl) = rcControl;
CRect rcRightPart(
		rcControl.right + ( ( _TII.m_nControlWidth > 0 && ( ! _TII.m_strEditText.IsEmpty() ) ) ? _marginControlLabelRight : 0 ),
		rcControl.top,
		rcClient.right,
		rcControl.bottom
		);
	if( pRectRightPart != NULL )
		(*pRectRightPart) = rcRightPart;
	return true;
}

bool CExtTreeCtrl::TreeItemRectGet(
	HTREEITEM hti,
	CRect & rcRetVal,
	CExtTreeCtrl::e_tree_item_rect_type_t eTIRT
	) const
{
	ASSERT_VALID(this);
bool bRetVal = false;
	switch( eTIRT )
	{
	case e_tirt_entire:
		bRetVal = TreeItemRectsGet( hti, &rcRetVal );
	break;
	case e_tirt_line_area:
		bRetVal = TreeItemRectsGet( hti, NULL, &rcRetVal );
	break;
	case e_tirt_tree_box:
		bRetVal = TreeItemRectsGet( hti, NULL, NULL, &rcRetVal );
	break;
	case e_tirt_check_box:
	{
		const TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
		if( _TII.m_eCheckBoxType != e_cbt_none )
			bRetVal = TreeItemRectsGet( hti, NULL, NULL, NULL, &rcRetVal );
	}
	break;
	case e_tirt_icon:
	{
		const CExtCmdIcon * pIcon = OnTreeItemQueryIcon( hti );
		if( pIcon != NULL )
			bRetVal = TreeItemRectsGet( hti, NULL, NULL, NULL, NULL, &rcRetVal );
	}
	break;
	case e_tirt_label:
		bRetVal = TreeItemRectsGet( hti, NULL, NULL, NULL, NULL, NULL, &rcRetVal );
	break;
	case e_tirt_control:
		bRetVal = TreeItemRectsGet( hti, NULL, NULL, NULL, NULL, NULL, NULL, &rcRetVal );
	break;
	case e_tirt_right_part:
		bRetVal = TreeItemRectsGet( hti, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &rcRetVal );
	break;
	}; // switch( eTIRT )
	return bRetVal;
}

INT CExtTreeCtrl::TreeItemCalcLabelWidth(
	HTREEITEM hti
	) const
{
	ASSERT_VALID(this);
INT nWidth = 0;
CWindowDC dc( NULL );
CExtSafeString strItemText = GetItemText( hti );
	if( ! strItemText.IsEmpty() )
	{
		const TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
		CFont * pFont = &m_fontNormal;
		if( _TII.m_bBold || _TII.m_bItalic )
		{
			if( _TII.m_bBold && _TII.m_bItalic )
				pFont = &m_fontBoldItalic;
			else if( _TII.m_bBold )
				pFont = &m_fontBold;
			else if( _TII.m_bItalic )
				pFont = &m_fontItalic;
			if( _TII.m_bItalic )
				nWidth += 3;
		}
		pFont = dc.SelectObject( pFont );
		CRect rcText( 0, 0, 0, 0 );
		CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(strItemText), strItemText.GetLength(), rcText, DT_SINGLELINE|DT_LEFT|DT_TOP|DT_NOPREFIX|DT_CALCRECT, 0 );
		dc.SelectObject( pFont );
		nWidth += rcText.Width();
	}
	return nWidth + PaddingLabelLeftGet() + PaddingLabelRightGet(); // + 6
}

HTREEITEM CExtTreeCtrl::TreeItemHitTest(
	CPoint ptClient,
	DWORD & dwHitTestFlags
	) const
{
	ASSERT_VALID(this);
	dwHitTestFlags = __EXT_TVHT_NOWHERE;
CRect rcClient, rc;
	GetClientRect( &rcClient );
HTREEITEM hti = GetNextItem( NULL, TVGN_FIRSTVISIBLE );
	for( ; hti != NULL; hti = GetNextItem( hti, TVGN_NEXTVISIBLE ) )
	{
		CRect rcEntire, rcLinesArea, rcTreeBox, rcCheckBox,
			rcIcon, rcLabel, rcControl, rcRightPart;
		if( ! TreeItemRectsGet(
				hti,
				&rcEntire,
				&rcLinesArea,
				&rcTreeBox,
				&rcCheckBox,
				&rcIcon,
				&rcLabel,
				&rcControl,
				&rcRightPart
				)
			)
			continue;
		if( rcEntire.top > rcClient.bottom )
			break;
		if( ! rcEntire.PtInRect( ptClient ) )
			continue;
		if(		(! rcLabel.IsRectEmpty() )
			&&	rcLabel.PtInRect( ptClient )
			)
		{
			dwHitTestFlags = __EXT_TVHT_ONITEMLABEL;
			return hti;
		}
		if(		(! rcIcon.IsRectEmpty() )
			&&	rcIcon.PtInRect( ptClient )
			)
		{
			dwHitTestFlags = __EXT_TVHT_ONITEMICON;
			return hti;
		}
		if(		(! rcCheckBox.IsRectEmpty() )
			&&	rcCheckBox.PtInRect( ptClient )
			)
		{
			dwHitTestFlags = __EXT_TVHT_ONCHECKBOX;
			return hti;
		}
		if(		(! rcTreeBox.IsRectEmpty() )
			&&	rcTreeBox.PtInRect( ptClient )
			)
		{
			dwHitTestFlags = __EXT_TVHT_ONITEMBUTTON|__EXT_TVHT_ONITEMINDENT;
			return hti;
		}
		if(		(! rcLinesArea.IsRectEmpty() )
			&&	rcLinesArea.PtInRect( ptClient )
			)
		{
			dwHitTestFlags = __EXT_TVHT_ONITEMINDENT;
			return hti;
		}
		if(		(! rcControl.IsRectEmpty() )
			&&	rcControl.PtInRect( ptClient )
			)
		{
			dwHitTestFlags = __EXT_TVHT_ONCONTROL;
			return hti;
		}
		if(		(! rcRightPart.IsRectEmpty() )
			&&	rcRightPart.PtInRect( ptClient )
			)
		{
			dwHitTestFlags = __EXT_TVHT_ONITEMRIGHT;
			return hti;
		}
		return hti;
	}

	return NULL;
}

void CExtTreeCtrl::_DoDelayedTreeIconInit()
{
	ASSERT_VALID( this );
}

INT CExtTreeCtrl::TreeIconGetCount() const
{
	ASSERT_VALID( this );
	( const_cast < CExtTreeCtrl * > ( this ) ) -> _DoDelayedTreeIconInit();
	return (INT)m_arrIcons.GetSize();
}

void CExtTreeCtrl::TreeIconInsert(
	const CExtCmdIcon & _icon,
	INT nPos
	)
{
	ASSERT_VALID( this );
	_DoDelayedTreeIconInit();
	if( nPos < 0 || nPos > TreeIconGetCount() )
	{
		ASSERT( FALSE );
		::AfxThrowUserException();
	}
	m_arrIcons.InsertAt( nPos, new CExtCmdIcon( _icon ) );
}

void CExtTreeCtrl::TreeIconAdd(
	const CExtCmdIcon & _icon
	)
{
	ASSERT_VALID( this );
	TreeIconInsert( _icon, TreeIconGetCount() );
}

void CExtTreeCtrl::TreeIconRemove(
	INT nPos,
	INT nCount // = 1
	)
{
	ASSERT_VALID( this );
	if( nPos < 0 || nCount <= 0 )
		return;
	_DoDelayedTreeIconInit();
INT nInconIndex, nIconCount = TreeIconGetCount();
	if( ( nPos + nCount ) > nIconCount )
		nCount = nIconCount - nPos;
	if( nCount <= 0 )
		return;
	for( nInconIndex = 0; nInconIndex < nCount; nInconIndex ++ )
	{
		CExtCmdIcon * pIcon = m_arrIcons[ nPos + nInconIndex ];
		ASSERT( pIcon != NULL );
		delete pIcon;
	}
	m_arrIcons.RemoveAt( nPos, nCount );
}

void CExtTreeCtrl::TreeIconRemoveAll()
{
	ASSERT_VALID( this );
	TreeIconRemove( 0, TreeIconGetCount() );
}

CExtCmdIcon & CExtTreeCtrl::TreeIconGetAt( INT nPos )
{
	ASSERT_VALID( this );
	_DoDelayedTreeIconInit();
	if( nPos < 0 || nPos >= TreeIconGetCount() )
	{
		ASSERT( FALSE );
		::AfxThrowUserException();
	}
CExtCmdIcon * pIcon = m_arrIcons[ nPos ];
	ASSERT( pIcon != NULL );
	return (*pIcon);
}

const CExtCmdIcon & CExtTreeCtrl::TreeIconGetAt( INT nPos ) const
{
	ASSERT_VALID( this );
	return ( const_cast < CExtTreeCtrl * > ( this ) ) -> TreeIconGetAt( nPos );
}

LRESULT CExtTreeCtrl::OnSetFont( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	wParam;
	lParam;
LRESULT lr = Default();
	_FontsSync();
	return lr;
}

LRESULT CExtTreeCtrl::OnSettingChange( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	wParam;
	lParam;
LRESULT lr = Default();
	_FontsSync();
	return lr;
}

void CExtTreeCtrl::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	ASSERT_VALID( this );
	if( m_hWndChildControl != NULL )
	{
		SendMessage( WM_CANCELMODE );
		if( ::GetFocus() != m_hWnd )
			SetFocus();
	}
	CTreeCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
	Invalidate();
}

void CExtTreeCtrl::OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
	ASSERT_VALID( this );
	if( m_hWndChildControl != NULL )
	{
		SendMessage( WM_CANCELMODE );
		if( ::GetFocus() != m_hWnd )
			SetFocus();
	}
	CTreeCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CExtTreeCtrl::OnSize( UINT nType, int cx, int cy )
{
	ASSERT_VALID( this );
	CTreeCtrl::OnSize(nType, cx, cy);
	if( nType == SIZE_MINIMIZED )
		return;
	CWnd::RepositionBars( 0, 0xFFFF, 0 );
}

LRESULT CExtTreeCtrl::OnStyleChanging( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	wParam;
	lParam;
	m_wndContentExpand.Deactivate();
LRESULT lr = Default();
	return lr;
}

LRESULT CExtTreeCtrl::OnStyleChanged( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	wParam;
	lParam;
LRESULT lr = Default();
	return lr;
}

void CExtTreeCtrl::_FontsSync()
{
	ASSERT_VALID( this );
	_FontsDestroy();
	ASSERT( GetSafeHwnd() != NULL );
HFONT hFont = (HFONT)SendMessage( WM_GETFONT );
	if( hFont == NULL )
	{
		hFont = (HFONT)::GetStockObject( DEFAULT_GUI_FONT );
		if( hFont == NULL )
		{
			hFont = (HFONT)::GetStockObject( SYSTEM_FONT );
			ASSERT( hFont != NULL );
		}
	}
LOGFONT _lf;
	::memset( &_lf, 0, sizeof(LOGFONT) );
	::GetObject( (HANDLE)hFont, sizeof(LOGFONT), &_lf );
LONG lfWeightSaved = _lf.lfWeight;

	m_fontNormal.CreateFontIndirect( &_lf );

	_lf.lfUnderline = TRUE;
	m_fontNormalUnderline.CreateFontIndirect( &_lf );
	_lf.lfUnderline = FALSE;

	if( _lf.lfWeight >= 700 )
		_lf.lfWeight = 900;
	else
		_lf.lfWeight = 700; // bold
	m_fontBold.CreateFontIndirect( &_lf );

	_lf.lfUnderline = TRUE;
	m_fontBoldUnderline.CreateFontIndirect( &_lf );
	_lf.lfUnderline = FALSE;

	_lf.lfItalic = BYTE(!0);
	m_fontBoldItalic.CreateFontIndirect( &_lf );

	_lf.lfUnderline = TRUE;
	m_fontBoldItalicUnderline.CreateFontIndirect( &_lf );
	_lf.lfUnderline = FALSE;

	_lf.lfWeight = lfWeightSaved;
	m_fontItalic.CreateFontIndirect( &_lf );

	_lf.lfUnderline = TRUE;
	m_fontItalicUnderline.CreateFontIndirect( &_lf );
	_lf.lfUnderline = FALSE;

}

void CExtTreeCtrl::_FontsDestroy()
{
	ASSERT_VALID( this );
	if( m_fontNormal.GetSafeHandle() != NULL )
		m_fontNormal.DeleteObject();
	if( m_fontBold.GetSafeHandle() != NULL )
		m_fontBold.DeleteObject();
	if( m_fontItalic.GetSafeHandle() != NULL )
		m_fontItalic.DeleteObject();
	if( m_fontBoldItalic.GetSafeHandle() != NULL )
		m_fontBoldItalic.DeleteObject();
	if( m_fontNormalUnderline.GetSafeHandle() != NULL )
		m_fontNormalUnderline.DeleteObject();
	if( m_fontBoldUnderline.GetSafeHandle() != NULL )
		m_fontBoldUnderline.DeleteObject();
	if( m_fontItalicUnderline.GetSafeHandle() != NULL )
		m_fontItalicUnderline.DeleteObject();
	if( m_fontBoldItalicUnderline.GetSafeHandle() != NULL )
		m_fontBoldItalicUnderline.DeleteObject();
}


void CExtTreeCtrl::PreSubclassWindow()
{
	::memset( &m_nLastMouseButtonEventType, 0, sizeof(m_nLastMouseButtonEventType) );
	CTreeCtrl::PreSubclassWindow();
	_FontsSync();
	ModifyStyle( 0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS, SWP_FRAMECHANGED );
	CWnd::RepositionBars( 0, 0xFFFF, 0 );
}

const CExtCmdIcon * CExtTreeCtrl::OnTreeItemQueryIcon(
	HTREEITEM hti
	) const
{
	ASSERT_VALID( this );
	if( hti == NULL )
		return NULL;
const TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
	if( TreeItemIsExpanded( hti ) )
	{
		if( _TII.m_nIconIndexExpanded < 0 || _TII.m_nIconIndexExpanded >= TreeIconGetCount() )
			return NULL;
		const CExtCmdIcon & _icon = TreeIconGetAt( _TII.m_nIconIndexExpanded );
		if( _icon.IsEmpty() )
			return NULL;
		return (&_icon);
	}
	else
	{
		if( _TII.m_nIconIndexCollapsed < 0 || _TII.m_nIconIndexCollapsed >= TreeIconGetCount() )
			return NULL;
		const CExtCmdIcon & _icon = TreeIconGetAt( _TII.m_nIconIndexCollapsed );
		if( _icon.IsEmpty() )
			return NULL;
		return (&_icon);
	}
}

HWND CExtTreeCtrl::OnInplaceControlCreate(
	HTREEITEM hti,
	CRect rcControl
	)
{
	ASSERT_VALID( this );
const TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
	switch( _TII.m_eICT )
	{
		case __EICT_EDIT:
		{
			CInplaceEdit * pEdit = new CInplaceEdit( false );
			if( ! pEdit->Create(
					 WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
						| ES_AUTOHSCROLL
						| _TII.m_dwAdditionalControlStyles
						,
					rcControl,
					this,
					UINT(IDC_STATIC)
					)
				)
				return NULL;
			pEdit->SetFont( GetFont() );

			pEdit->SetWindowText( _TII.m_strEditText );
			pEdit->SetSel(0, -1);
			pEdit->SetFocus();

			return pEdit->m_hWnd;
		} // __EICT_EDIT
		case __EICT_DROPDOWN_COMBOBOX:
		case __EICT_DROPLIST_COMBOBOX:
		{
			CRect rcComboBox = rcControl;
			rcComboBox.bottom += _TII.m_nDropDownHeight;
			CInplaceComboBox * pCombo = new CInplaceComboBox;
			if( ! pCombo->Create(
					WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
						| CBS_AUTOHSCROLL
						| _TII.m_dwAdditionalControlStyles
						| ( ( _TII.m_eICT == __EICT_DROPDOWN_COMBOBOX ) ? CBS_DROPDOWN : CBS_DROPDOWNLIST )
						,
					rcComboBox,
					this,
					UINT(IDC_STATIC)
					)
				)
				return NULL;

			pCombo->SetFont( GetFont() );
			INT nCount = _TII.ComboBoxItem_GetCount();
			if( nCount > 0 )
			{
				INT nIndex = 0;
				for( ; nIndex < nCount; nIndex ++ )
				{
					const COMBOBOXITEMDATA_t & _CID = _TII.ComboBoxItem_GetRefAt( nIndex );
					INT nY = pCombo->AddString( LPCTSTR(_CID.m_strLabel) );
					if( nY >= 0 )
						pCombo->SetItemData( nY, _CID.m_lParam );
				}
				nIndex = pCombo->FindStringExact( -1, LPCTSTR(_TII.m_strEditText) );
				if( nIndex < 0 )
					nIndex = pCombo->FindString( -1, LPCTSTR(_TII.m_strEditText) );
				if( nIndex < 0 )
					nIndex = 0;
				pCombo->SetCurSel( nIndex );
			} // if( nCount > 0 )
			pCombo->SetFocus();

			return pCombo->m_hWnd;
		} // __EICT_DROPDOWN_COMBOBOX
		case __EICT_PROGRESS_BAR_THEMED:
		case __EICT_PROGRESS_BAR_WITH_BOXES:
		case __EICT_PROGRESS_BAR_SMOOTH:
		case __EICT_PROGRESS_BAR_ENDLESS:
			return NULL;
	} // switch( _TII.m_eICT )
	return NULL;
}

void CExtTreeCtrl::OnInplaceControlCancel()
{
	ASSERT_VALID( this );
	if( m_hWndChildControl == NULL )
		return;
	::SendMessage( m_hWndChildControl, WM_CANCELMODE, 0L, 0L );
	if( ::IsWindow( m_hWndChildControl ) )
		::DestroyWindow( m_hWndChildControl );
	m_hWndChildControl = NULL;
	m_htiInplaceEdited = NULL;
}

void CExtTreeCtrl::OnInplaceControlComplete(
	__EXT_MFC_SAFE_LPCTSTR strEditedText,
	bool bEditingLabel
	)
{
	ASSERT_VALID( this );
HTREEITEM hti = GetInPlaceEditedItem();
	if( hti == NULL )
		return;
	if( ! bEditingLabel )
	{
		TREEITEMINFO_t & _TII = TreeItemInfoGet( hti );
		_TII.m_strEditText = LPCTSTR(strEditedText);
	} // if( ! bEditingLabel )
	else
	{
		HWND hWndParent = ::GetParent( m_hWnd );
		UINT nOwnID = GetDlgCtrlID();
		CExtSafeString strItemText;
		TV_DISPINFO _data;
		::memset( &_data, 0, sizeof(TV_DISPINFO) );
		_data.hdr.hwndFrom = m_hWnd;
		_data.hdr.idFrom = nOwnID;
		_data.hdr.code = TVN_ENDLABELEDIT;
		_data.item.mask = TVIF_CHILDREN|TVIF_HANDLE|TVIF_PARAM|TVIF_STATE;
		_data.item.hItem = hti;
		GetItem( &_data.item );
		strItemText = GetItemText( hti );
		bool bModeNotNULL = true;
		if( strItemText == LPCTSTR(strEditedText) )
		{
			bModeNotNULL          = false;
			_data.item.cchTextMax = 0;
			_data.item.pszText    = NULL;
		}
		else
		{
//			_data.item.cchTextMax = INT(strItemText.GetLength());
//			_data.item.pszText    = strItemText.IsEmpty() ? _T("") : LPTSTR(LPCTSTR(strItemText));
			_data.item.cchTextMax = ( strEditedText != NULL ) ? INT( _tcslen(strEditedText) ) : 0;
			_data.item.pszText    = ( strEditedText != NULL ) ? LPTSTR(LPCTSTR(strEditedText)) : _T("");
			_data.item.mask |= TVIF_TEXT;
		}
		if( ! ::SendMessage( hWndParent, WM_NOTIFY, WPARAM(nOwnID), LPARAM(&_data) ) != 0 )
		{
			if( bModeNotNULL )
				return;
		}

		SetItemText( hti, LPCTSTR(strEditedText) );
	} // else from if( ! bEditingLabel )
}

bool CExtTreeCtrl::OnQueryWindowFocusedState() const
{
	ASSERT_VALID( this );
HWND hWndOwn = GetSafeHwnd();
	if( hWndOwn == NULL )
		return false;
HWND hWndFocus = ::GetFocus();
	if( hWndFocus == NULL )
		return false;
	if( hWndOwn == hWndFocus )
		return true;
	if( ::IsChild( hWndOwn, hWndFocus ) )
		return true;
	return false;
}

INT CExtTreeCtrl::MarginCheckBoxLeftGet() const
{
	ASSERT_VALID( this );
	return m_nMarginCheckBoxLeft;
}

void CExtTreeCtrl::MarginCheckBoxLeftSet( INT nMargin )
{
	ASSERT_VALID( this );
	m_nMarginCheckBoxLeft = nMargin;
}

INT CExtTreeCtrl::MarginCheckBoxRightGet() const
{
	ASSERT_VALID( this );
	return m_nMarginCheckBoxRight;
}

void CExtTreeCtrl::MarginCheckBoxRightSet( INT nMargin )
{
	ASSERT_VALID( this );
	m_nMarginCheckBoxRight = nMargin;
}

INT CExtTreeCtrl::MarginIconLeftGet() const
{
	ASSERT_VALID( this );
	return m_nMarginIconLeft;
}

void CExtTreeCtrl::MarginIconLeftSet( INT nMargin )
{
	ASSERT_VALID( this );
	m_nMarginIconLeft = nMargin;
}

INT CExtTreeCtrl::MarginIconRightGet() const
{
	ASSERT_VALID( this );
	return m_nMarginIconRight;
}

void CExtTreeCtrl::MarginIconRightSet( INT nMargin )
{
	ASSERT_VALID( this );
	m_nMarginIconRight = nMargin;
}

INT CExtTreeCtrl::MarginLabelLeftGet() const
{
	ASSERT_VALID( this );
	return m_nMarginLabelLeft;
}

void CExtTreeCtrl::MarginLabelLeftSet( INT nMargin )
{
	ASSERT_VALID( this );
	m_nMarginLabelLeft = nMargin;
}

INT CExtTreeCtrl::MarginLabelRightGet() const
{
	ASSERT_VALID( this );
	return m_nMarginLabelRight;
}

void CExtTreeCtrl::MarginLabelRightSet( INT nMargin )
{
	ASSERT_VALID( this );
	m_nMarginLabelRight = nMargin;
}

INT CExtTreeCtrl::MarginControlLabelLeftGet() const
{
	ASSERT_VALID( this );
	return m_nMarginControlLabelLeft;
}

void CExtTreeCtrl::MarginControlLabelLeftSet( INT nMargin )
{
	ASSERT_VALID( this );
	m_nMarginControlLabelLeft = nMargin;
}

INT CExtTreeCtrl::MarginControlLabelRightGet() const
{
	ASSERT_VALID( this );
	return m_nMarginControlLabelRight;
}

void CExtTreeCtrl::MarginControlLabelRightSet( INT nMargin )
{
	ASSERT_VALID( this );
	m_nMarginControlLabelRight = nMargin;
}

INT CExtTreeCtrl::PaddingLabelLeftGet() const
{
	ASSERT_VALID( this );
	return m_nPaddingLabelLeft;
}

void CExtTreeCtrl::PaddingLabelLeftSet( INT nPadding )
{
	ASSERT_VALID( this );
	m_nPaddingLabelLeft = nPadding;
}

INT CExtTreeCtrl::PaddingLabelRightGet() const
{
	ASSERT_VALID( this );
	return m_nPaddingLabelRight;
}

void CExtTreeCtrl::PaddingLabelRightSet( INT nPadding )
{
	ASSERT_VALID( this );
	m_nPaddingLabelRight = nPadding;
}

void CExtTreeCtrl::OnMouseMove( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	OnTreeMouseMove( nFlags, point );
}

void CExtTreeCtrl::OnTreeMouseMove( UINT nFlags, CPoint point )
{
	ASSERT_VALID( this );
	nFlags;
bool bTrackSelection =
		(	( ( GetStyle() & TVS_TRACKSELECT ) != 0) 
		||	( ( TreeGetStyleEx() & (__EXT_TREE_EX_HOVER_TREE_BOXES|__EXT_TREE_EX_HOVER_CHECK_BOXES|__EXT_TREE_EX_HOVER_RADIO_BUTTONS) ) != 0 ) 
		) 
		? true: false;
	if( ! bTrackSelection )
		return;
bool bInfoTip = ( ( GetStyle() & TVS_INFOTIP ) != 0 ) ? true: false;
DWORD dwHitTestFlagsNew = 0;
HTREEITEM htiHoverNew = NULL;
CPoint ptScreen = point;
	ClientToScreen( &ptScreen );
HWND hWndFromPoint = ::WindowFromPoint( ptScreen );
	if(		hWndFromPoint != NULL
		&&	(	hWndFromPoint == m_hWnd
			||	hWndFromPoint == m_wndContentExpand.GetSafeHwnd()
			)
		)
		htiHoverNew = TreeItemHitTest( point, dwHitTestFlagsNew );
	if(		htiHoverNew == m_htiHover
		&&	dwHitTestFlagsNew == m_dwLastHoverHitTestFlags
		)
		return;
	if( m_htiHover != NULL )
	{
		CRect rcItemEntire;
		TreeItemRectGet( m_htiHover, rcItemEntire, e_tirt_entire );
		InvalidateRect( &rcItemEntire );
	}
	if( htiHoverNew != NULL )
	{
		if( m_htiHover != htiHoverNew )
		{
			CRect rcItemEntire;
			TreeItemRectGet( htiHoverNew, rcItemEntire, e_tirt_entire );
			InvalidateRect( &rcItemEntire );
		}
		if( GetCapture() != this )
			SetCapture();
	}
	else
	{
		if( GetCapture() == this )
			ReleaseCapture();
	}
	if ( htiHoverNew != NULL )
		CWnd::SetCapture();
	else
		ReleaseCapture();
	OnTreeItemSetCursor(
		htiHoverNew,
		dwHitTestFlagsNew
		);
	OnTreeItemHoverChanged(
		htiHoverNew,
		dwHitTestFlagsNew,
		m_htiHover,
		m_dwLastHoverHitTestFlags
		);
	m_htiHover = htiHoverNew;
	m_dwLastHoverHitTestFlags = dwHitTestFlagsNew;
	if( bInfoTip )
	{
		bool bDeactivate = true;
		if( htiHoverNew != NULL )
		{
			CRect rcLabel;
			TreeItemRectGet( htiHoverNew, rcLabel, e_tirt_label );
			if( rcLabel.PtInRect( point ) )
			{
				CRect rcClient;
				GetClientRect( &rcClient );
				CWnd::RepositionBars( 0, 0xFFFF, 0, CWnd::reposQuery, &rcClient, &rcClient );
				if(		rcLabel.left < rcClient.left
					||	rcLabel.right > rcClient.right
					)
				{
					bDeactivate = false;
					m_wndContentExpand.Activate(
						rcLabel,
						this,
						__ECWAF_DEF_EXPANDED_ITEM_PAINTER|__ECWAF_NO_CAPTURE
							|__ECWAF_PASS_WM_LBUTTONUP
							|__ECWAF_PASS_WM_RBUTTONUP
							|__ECWAF_PASS_WM_MBUTTONUP
						);

					SetTimer( __EXT_TREE_CONTROL_TIMER_ID_TRACKHOVER, 100, NULL );
				}
			}
		}
		if( bDeactivate )
			m_wndContentExpand.Deactivate();
	} // if( bInfoTip )
}

BOOL CExtTreeCtrl::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message )
{
//	return CTreeCtrl::OnSetCursor(pWnd, nHitTest, message);
	pWnd;
	nHitTest;
	message;
CPoint ptScreen;
	if( ! ::GetCursorPos( &ptScreen ) )
		return FALSE;
	if( ::WindowFromPoint( ptScreen ) != m_hWnd )
		return FALSE;
CPoint point = ptScreen;
	ScreenToClient( &point );
DWORD dwHitTestFlags = 0;
HTREEITEM htiHover = TreeItemHitTest( point, dwHitTestFlags );
	OnTreeItemSetCursor( htiHover, dwHitTestFlags );
	return TRUE;
}

void CExtTreeCtrl::OnTreeItemHoverChanged(
	HTREEITEM htiHoverNew,
	DWORD dwHitTestFlagsNew,
	HTREEITEM htiHoverOld,
	DWORD dwHitTestFlagsOld
	)
{
	ASSERT_VALID( this );
	htiHoverNew;
	dwHitTestFlagsNew;
	htiHoverOld;
	dwHitTestFlagsOld;
}

void CExtTreeCtrl::OnTreeItemSetCursor(
	HTREEITEM htiHover,
	DWORD dwHitTestFlags
	)
{
	ASSERT_VALID( this );
HCURSOR hCursor = NULL;
	if(		htiHover != NULL
		&&	( dwHitTestFlags & __EXT_TVHT_ONITEMLABEL ) != 0
		&&	( GetStyle() & TVS_TRACKSELECT ) != 0
		&&	(	( TreeGetStyleEx() & __EXT_TREE_HOVER_DISABLE_ITEMS ) != 0
			||	OnQueryItemEnabledState( htiHover )
			)
		)
	{
		hCursor = m_hCursorHand;
	}
	if( hCursor == NULL )
		hCursor = ::LoadCursor( NULL, IDC_ARROW );
	::SetCursor( hCursor );
}

void CExtTreeCtrl::LoadWinXPTreeBox()
{
	ASSERT_VALID( this );
	VERIFY( m_bmpTreeBoxCollapsed		.LoadBMP_Resource( MAKEINTRESOURCE( IDB_EXT_BITMAP_TREE_BOX_COLLAPSED_XP ) ) );
	VERIFY( m_bmpTreeBoxCollapsedHover	.LoadBMP_Resource( MAKEINTRESOURCE( IDB_EXT_BITMAP_TREE_BOX_COLLAPSED_HOVER_XP ) ) );
	VERIFY( m_bmpTreeBoxExpanded		.LoadBMP_Resource( MAKEINTRESOURCE( IDB_EXT_BITMAP_TREE_BOX_EXPANDED_XP ) ) );
	VERIFY( m_bmpTreeBoxExpandedHover	.LoadBMP_Resource( MAKEINTRESOURCE( IDB_EXT_BITMAP_TREE_BOX_EXPANDED_HOVER_XP ) ) );
}

void CExtTreeCtrl::LoadWinVistaTreeBox()
{
	ASSERT_VALID( this );
	VERIFY( m_bmpTreeBoxCollapsed		.LoadBMP_Resource( MAKEINTRESOURCE( IDB_EXT_BITMAP_TREE_BOX_COLLAPSED_VISTA ) ) );
	VERIFY( m_bmpTreeBoxCollapsedHover	.LoadBMP_Resource( MAKEINTRESOURCE( IDB_EXT_BITMAP_TREE_BOX_COLLAPSED_HOVER_VISTA ) ) );
	VERIFY( m_bmpTreeBoxExpanded		.LoadBMP_Resource( MAKEINTRESOURCE( IDB_EXT_BITMAP_TREE_BOX_EXPANDED_VISTA ) ) );
	VERIFY( m_bmpTreeBoxExpandedHover	.LoadBMP_Resource( MAKEINTRESOURCE( IDB_EXT_BITMAP_TREE_BOX_EXPANDED_HOVER_VISTA ) ) );
}

#if _MFC_VER < 0x700
void CExtTreeCtrl::OnActivateApp(BOOL bActive, HTASK hTask)
#else
void CExtTreeCtrl::OnActivateApp(BOOL bActive, DWORD hTask)
#endif
{
	ASSERT_VALID( this );
	CTreeCtrl::OnActivateApp(bActive, hTask);
	if( ! bActive )
		SendMessage( WM_CANCELMODE );
}

void CExtTreeCtrl::OnCancelMode()
{
	ASSERT_VALID( this );
	CTreeCtrl::OnCancelMode();
	if( m_bCanceling )
		return;
	m_bCanceling = true;
	KillTimer( m_nDelayedEditingTimerID );
	OnInplaceControlCancel();
HWND hWndCapture = ::GetCapture();
	if( hWndCapture == m_hWnd )
		::ReleaseCapture();
//	m_wndContentExpand.Deactivate();
	m_bCanceling = false;
}


BOOL CExtTreeCtrl::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	ASSERT_VALID( this );
	if( m_hWndChildControl != NULL )
		SendMessage( WM_CANCELMODE );
BOOL bRetVal = CTreeCtrl::OnMouseWheel(nFlags, zDelta, pt);
	return bRetVal;
}

UINT CExtTreeCtrl::OnGetDlgCode()
{
	return CTreeCtrl::OnGetDlgCode();
	//return DLGC_WANTALLKEYS;
}

LRESULT CExtTreeCtrl::_OnPaintExpanedItemContent( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	lParam;
CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA * p_picd =
		(CExtContentExpandWnd::PAINT_ITEM_CONTENT_DATA *)wParam;
	ASSERT( p_picd != NULL );
	ASSERT( p_picd->m_dc.GetSafeHdc() != NULL );
	if( m_htiHover != NULL )
	{
		COLORREF clrTreeBkColor = TreeBkColorGet();
		if( clrTreeBkColor == COLORREF(-1L) )
			clrTreeBkColor = ::GetSysColor( COLOR_WINDOW );
		//p_picd->m_dc.FillSolidRect( &rcClient, clrTreeBkColor );
		CFont * pOldFont = p_picd->m_dc.SelectObject( &m_fontNormal );
		COLORREF clrBackgroundOld = p_picd->m_dc.SetBkColor( clrTreeBkColor );
		COLORREF clrText = ::GetSysColor( COLOR_BTNTEXT );
		COLORREF clrTextOld = p_picd->m_dc.SetTextColor( clrText );
		int nOldBkMode = p_picd->m_dc.SetBkMode( TRANSPARENT );

		bool bFocusedWindow = OnQueryWindowFocusedState();
		bool bDisabledItem = ! OnQueryItemEnabledState( m_htiHover );
		bool bFocusedItem = ( m_htiHover == GetFocusedItem() ) ? true : false;
		bool bSelectedItem = IsItemSelected( m_htiHover );
		CRect rcItemEntire, rcItemLabel;
//		TreeItemRectGet( m_htiHover, rcItemEntire, e_tirt_entire );
		TreeItemRectGet( m_htiHover, rcItemLabel, e_tirt_label );
		m_wndContentExpand.GetClientRect( &rcItemEntire );
		m_wndContentExpand.ClientToScreen( &rcItemEntire );
		ScreenToClient( &rcItemEntire );
		p_picd->m_dc.OffsetViewportOrg( -rcItemLabel.left, -rcItemLabel.top );
		OnPaintTreeItem(
			m_htiHover, 
			p_picd->m_dc, 
			rcItemEntire,
			bFocusedWindow, 
			bDisabledItem, 
			bSelectedItem,
			bFocusedItem,
			false
			);
		p_picd->m_dc.OffsetViewportOrg( rcItemLabel.left, rcItemLabel.top );

		p_picd->m_dc.SetBkMode( nOldBkMode );
		p_picd->m_dc.SetTextColor( clrTextOld );
		p_picd->m_dc.SetBkColor( clrBackgroundOld );
		p_picd->m_dc.SelectObject( pOldFont );
	}
	return (!0);
}

void CExtTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (pWnd->GetSafeHwnd() != m_hWnd)
	{
		Default();
		return;
	}
DWORD dwHitTestFlags = 0;
	ScreenToClient( &point );
HTREEITEM hti = TreeItemHitTest( point, dwHitTestFlags );
	if( hti == NULL )
		return;
	SelectDropTarget( hti );
	OnTreeItemContextMenu( hti, point, dwHitTestFlags );
	SelectDropTarget( NULL );
}

void CExtTreeCtrl::OnTreeItemContextMenu(
	HTREEITEM hti,
	CPoint point, // client coordinates
	DWORD dwHitTestFlags
	)
{
	ASSERT_VALID( this );
	hti;
	point;
	dwHitTestFlags;
}

void CExtTreeCtrl::OnTimer( __EXT_MFC_UINT_PTR nIDEvent )
{
	ASSERT_VALID( this );
	if( nIDEvent == m_nDelayedFocusTimerID )
	{
		KillTimer( m_nDelayedFocusTimerID );
		if( m_htiDelayedFocus == NULL )
			return;
		HTREEITEM htiDelayedFocus = m_htiDelayedFocus;
		m_htiDelayedFocus = NULL;
		if( GetFocusedItem() != htiDelayedFocus )
			return;
//CString sTmp = GetItemText( htiDelayedFocus );
//TRACE1( "Delated focus event for \"%s\" tree item\r\n", LPCTSTR(sTmp) );
		OnTreeItemDelayedFocus( htiDelayedFocus, false );
		return;
	} // if( nIDEvent == m_nDelayedFocusTimerID )
	if( nIDEvent == __EXT_TREE_CONTROL_TIMER_ID_TRACKHOVER )
	{
		if( m_wndContentExpand.GetSafeHwnd() != NULL )
		{
			CPoint ptCursor;
			if( GetCursorPos( &ptCursor ) )
			{
				CPoint point = ptCursor;
				ScreenToClient( &point );
				HWND hWndFromPoint = ::WindowFromPoint( ptCursor );
				if(		hWndFromPoint == m_hWndChildControl
					||	hWndFromPoint == m_wndContentExpand.m_hWnd
					)
				{
					CRect rcClient;
					GetClientRect( &rcClient );
					CWnd::RepositionBars( 0, 0xFFFF, 0, CWnd::reposQuery, &rcClient, &rcClient );
					if( rcClient.PtInRect( point ) )
						return;
				}
				KillTimer( __EXT_TREE_CONTROL_TIMER_ID_TRACKHOVER );
				OnTreeMouseMove( 0, point );
				return;
			}
		}
		KillTimer( __EXT_TREE_CONTROL_TIMER_ID_TRACKHOVER );
		m_wndContentExpand.Deactivate();
		return;
	}
	if( nIDEvent == m_nDelayedEditingTimerID )
	{
		HTREEITEM hti = GetFocusedItem();
		EditLabel( hti );
		return;
	}
	CTreeCtrl::OnTimer( nIDEvent );
}

void CExtTreeCtrl::OnTreeItemDelayedFocus(
	HTREEITEM hti,
	bool bStartWaiting,
	bool bMouseClickEvent // = false
	)
{
	ASSERT_VALID( this );
	if( bStartWaiting )
	{
		m_htiDelayedFocus = hti;
		KillTimer( m_nDelayedFocusTimerID );
		SetTimer( m_nDelayedFocusTimerID, m_nDelayedFocusTime, NULL );
		LPARAM lParam = LPARAM(__PARM_DELAYED_FOCUS_BEGIN_WAITING);
		if( bMouseClickEvent )
			lParam |= LPARAM(__PARM_DELAYED_FOCUS_MOUSE_CLICK);
		GetParent()->SendMessage( g_nMsgTreeItemDelayedFocus, WPARAM(this), lParam );
	}
	else
	{
		KillTimer( m_nDelayedFocusTimerID );
		m_htiDelayedFocus = NULL;
		GetParent()->SendMessage( g_nMsgTreeItemDelayedFocus, (WPARAM)this, 0L );
	}
}

bool CExtTreeCtrl::_NeedsChildren( HTREEITEM hti ) const
{
	ASSERT_VALID( this );
	return ( GetChildItem( hti ) == NULL ) ? true : false;
}

void CExtTreeCtrl::_DeleteChildren( HTREEITEM hti )
{
	ASSERT_VALID( this );
HTREEITEM hti2 = GetChildItem( hti );
	for( ; hti2 != NULL; )
	{
		HTREEITEM hti3 = GetNextSiblingItem( hti2 );
		DeleteItem( hti2 );
		hti2 = hti3;
	}
}

bool CExtTreeCtrl::ReadOnlyLabelsGet() const
{
	ASSERT_VALID( this );
	return m_bReadOnlyLabels;
}

void CExtTreeCtrl::ReadOnlyLabelsSet( bool bReadOnlyLabels )
{
	ASSERT_VALID( this );
	m_bReadOnlyLabels = bReadOnlyLabels;
}

void CExtTreeCtrl::_CheckDeepFocusBegin(
	CList < HTREEITEM, HTREEITEM > & _listFocusPath
	)
{
	ASSERT_VALID( this );
	_listFocusPath.RemoveAll();
	if( GetSafeHwnd() == NULL )
		return;
HTREEITEM hti = GetFocusedItem();
	if( hti == NULL )
		return;
	ASSERT( HasItem( hti ) );
	for( ; hti != NULL; hti = GetNextItem( hti, TVGN_PARENT ) )
		_listFocusPath.AddHead( hti );
}

HTREEITEM CExtTreeCtrl::_CheckDeepFocusEnd(
	CList < HTREEITEM, HTREEITEM > & _listFocusPath
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return NULL;
HTREEITEM htiRetVal = NULL;
POSITION pos = _listFocusPath.GetHeadPosition();
	for( ; pos != NULL; )
	{
		HTREEITEM hti = _listFocusPath.GetNext( pos );
		if(		hti == NULL
			||	( ! HasItem( hti ) )
			)
			break;
		htiRetVal = hti;
	}
	return htiRetVal;
}

HTREEITEM CExtTreeCtrl::FindItemByText(
	HTREEITEM htiStartWalk,
	__EXT_MFC_SAFE_LPCTSTR strText,
	UINT nStartWalkCode, // = TVGN_CHILD
	UINT nContinueWalkCode // = TVGN_NEXT
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return NULL;
	if( htiStartWalk == NULL || htiStartWalk == TVI_ROOT )
	{
		htiStartWalk = GetRootItem();
		if( htiStartWalk == NULL )
			return NULL;
	}
	if( strText == NULL )
		strText = _T("");
HTREEITEM hti = GetNextItem( htiStartWalk, nStartWalkCode );
	for( ; hti != NULL; hti = GetNextItem( hti, nContinueWalkCode ) )
	{
		CString s = GetItemText( hti );
		if( s == LPCTSTR(strText) )
			return hti;
	}
	return NULL;
}

CExtTreeCtrl::COMBOBOXITEMDATA_t::COMBOBOXITEMDATA_t(
	__EXT_MFC_SAFE_LPCTSTR strLabel, // = NULL
	LPARAM lParam // = 0L
	)
	: m_strLabel( ( strLabel == NULL) ? _T("") : strLabel )
	, m_lParam( lParam )
{
}
CExtTreeCtrl::COMBOBOXITEMDATA_t::COMBOBOXITEMDATA_t( const CExtTreeCtrl::COMBOBOXITEMDATA_t & other )
{
	m_strLabel = other.m_strLabel;
	m_lParam = other.m_lParam;
}

CExtTreeCtrl::COMBOBOXITEMDATA_t::~COMBOBOXITEMDATA_t()
{
}

CExtTreeCtrl::COMBOBOXITEMDATA_t & CExtTreeCtrl::COMBOBOXITEMDATA_t::operator = ( const CExtTreeCtrl::COMBOBOXITEMDATA_t & other )
{
	m_strLabel = other.m_strLabel;
	m_lParam = other.m_lParam;
	return (*this);
}

CExtTreeCtrl::TREEITEMINFO_t::TREEITEMINFO_t()
{
	Init();
}

CExtTreeCtrl::TREEITEMINFO_t::TREEITEMINFO_t( const CExtTreeCtrl::TREEITEMINFO_t & other )
{
	Init();
	AssignFromOther( other );
}

CExtTreeCtrl::TREEITEMINFO_t::~TREEITEMINFO_t()
{
	ComboBoxItem_RemoveAt();
}

void CExtTreeCtrl::TREEITEMINFO_t::Init()
{
	m_clrTextNormal			= COLORREF(-1L);
	m_clrBackgroundNormal	= COLORREF(-1L);
	m_clrTextSelected		= COLORREF(-1L);
	m_clrBackgroundSelected	= COLORREF(-1L);
	m_clrTextDisabled		= COLORREF(-1L);
	m_clrBackgroundDisabled	= COLORREF(-1L);
	m_clrTextHover			= COLORREF(-1L);
	m_bSelected				= false;
	m_bDisabled				= false;
	m_bBold					= false;
	m_bItalic				= false;
	m_bUnderline			= false;
	m_eCheckBoxType         = e_cbt_none;
	m_eICT					= __EICT_NONE;
	m_nCheckBoxState        = 0;
	m_strRadioGroupName.Empty();
	m_lParam				= 0L;
	m_nIconIndexCollapsed   = -1;
	m_nIconIndexExpanded    = -1;
	m_nControlWidth			= 0;
	m_nDropDownHeight		= 200;
	m_dwAdditionalControlStyles = 0;
	m_dwAdditionalLabelEditorStyles = 0;
	m_strEditText.Empty();
	m_nProgressMin			= 0;
	m_nProgressMax			= 100;
	m_nProgressPos			= 0;
}

void CExtTreeCtrl::TREEITEMINFO_t::AssignFromOther( const CExtTreeCtrl::TREEITEMINFO_t & other )
{
	m_clrTextNormal			= other.m_clrTextNormal;
	m_clrBackgroundNormal	= other.m_clrBackgroundNormal;
	m_clrTextSelected		= other.m_clrTextSelected;
	m_clrBackgroundSelected	= other.m_clrBackgroundSelected;
	m_clrTextDisabled		= other.m_clrTextDisabled;
	m_clrBackgroundDisabled	= other.m_clrBackgroundDisabled;
	m_clrTextHover			= other.m_clrTextHover;
	m_bSelected				= other.m_bSelected;
	m_bDisabled				= other.m_bDisabled;
	m_bBold					= other.m_bBold;
	m_bItalic				= other.m_bItalic;
	m_bUnderline			= other.m_bUnderline;
	m_eCheckBoxType         = other.m_eCheckBoxType;
	m_eICT					= other.m_eICT;
	m_nCheckBoxState        = other.m_nCheckBoxState;
	m_strRadioGroupName     = other.m_strRadioGroupName;
	m_lParam				= other.m_lParam;
	m_nIconIndexCollapsed   = other.m_nIconIndexCollapsed;
	m_nIconIndexExpanded    = other.m_nIconIndexExpanded;
	m_nControlWidth			= other.m_nControlWidth;
	m_nDropDownHeight		= other.m_nDropDownHeight;
	m_dwAdditionalControlStyles = other.m_dwAdditionalControlStyles;
	m_dwAdditionalLabelEditorStyles = other.m_dwAdditionalLabelEditorStyles;
	m_strEditText			= other.m_strEditText;
	m_nProgressMin			= other.m_nProgressMin;
	m_nProgressMax			= other.m_nProgressMax;
	m_nProgressPos			= other.m_nProgressPos;
	ComboBoxItem_RemoveAt();
INT nCount = other.ComboBoxItem_GetCount();
	if( nCount > 0 )
	{
		try
		{
			m_arrComboBoxItems.InsertAt( 0, NULL, nCount );
			INT nIndex = 0;
			for( ; nIndex < nCount; nIndex++ )
			{
				const COMBOBOXITEMDATA_t & _CID = other.ComboBoxItem_GetRefAt( nIndex );
				COMBOBOXITEMDATA_t * pCID = new COMBOBOXITEMDATA_t( _CID );
				m_arrComboBoxItems.SetAt( nIndex, pCID );
			}
		}
		catch( CException * pException )
		{
			pException->Delete();
			ComboBoxItem_RemoveAt();
		}
	}
}

CExtTreeCtrl::TREEITEMINFO_t & CExtTreeCtrl::TREEITEMINFO_t::operator = ( const CExtTreeCtrl::TREEITEMINFO_t & other )
{
	AssignFromOther( other );
	return ( * this );
}

INT CExtTreeCtrl::TREEITEMINFO_t::ComboBoxItem_GetCount() const
{
	return INT( m_arrComboBoxItems.GetSize() );
}

CExtTreeCtrl::COMBOBOXITEMDATA_t * CExtTreeCtrl::TREEITEMINFO_t::ComboBoxItem_GetAt( INT nIndex )
{
	if( nIndex < 0 || nIndex >= ComboBoxItem_GetCount() )
		return NULL;
	COMBOBOXITEMDATA_t * pCID = m_arrComboBoxItems[ nIndex ];
	ASSERT( pCID != NULL );
	return pCID;
}

const CExtTreeCtrl::COMBOBOXITEMDATA_t * CExtTreeCtrl::TREEITEMINFO_t::ComboBoxItem_GetAt( INT nIndex ) const
{
	return ( const_cast < TREEITEMINFO_t * > ( this ) ) -> ComboBoxItem_GetAt( nIndex );
}

CExtTreeCtrl::COMBOBOXITEMDATA_t & CExtTreeCtrl::TREEITEMINFO_t::ComboBoxItem_GetRefAt( INT nIndex )
{
COMBOBOXITEMDATA_t * pCID = ComboBoxItem_GetAt( nIndex );
	if( pCID == NULL )
		::AfxThrowUserException();
	return (*pCID);
}
const CExtTreeCtrl::COMBOBOXITEMDATA_t & CExtTreeCtrl::TREEITEMINFO_t::ComboBoxItem_GetRefAt( INT nIndex ) const
{
	return ( const_cast < TREEITEMINFO_t * > ( this ) ) -> ComboBoxItem_GetRefAt( nIndex );
}

void CExtTreeCtrl::TREEITEMINFO_t::ComboBoxItem_InsertAt(
	INT nIndex,
	CExtTreeCtrl::COMBOBOXITEMDATA_t * pCID,
	bool bInsertClonedCopy // = true
	)
{
	ASSERT( pCID != NULL );
INT nCount = ComboBoxItem_GetCount();
	if( nIndex < 0 || nIndex > nCount )
		nIndex = nCount;
	if( bInsertClonedCopy )
		pCID = new COMBOBOXITEMDATA_t( *pCID );
	m_arrComboBoxItems.InsertAt( nIndex, pCID );
}

void CExtTreeCtrl::TREEITEMINFO_t::ComboBoxItem_InsertAt(
	INT nIndex,
	const CExtTreeCtrl::COMBOBOXITEMDATA_t & _CID
	)
{
COMBOBOXITEMDATA_t * pCID = new CExtTreeCtrl::COMBOBOXITEMDATA_t( _CID );
	ComboBoxItem_InsertAt( nIndex, pCID, false );
}

void CExtTreeCtrl::TREEITEMINFO_t::ComboBoxItem_Add(
	CExtTreeCtrl::COMBOBOXITEMDATA_t * pCID,
	bool bInsertClonedCopy // = true
	)
{
	ASSERT( pCID != NULL );
	ComboBoxItem_InsertAt( -1, pCID, bInsertClonedCopy );
}

void CExtTreeCtrl::TREEITEMINFO_t::ComboBoxItem_Add( const CExtTreeCtrl::COMBOBOXITEMDATA_t & _CID )
{
COMBOBOXITEMDATA_t * pCID = new COMBOBOXITEMDATA_t( _CID );
	ComboBoxItem_InsertAt( -1, pCID, false );
}

INT CExtTreeCtrl::TREEITEMINFO_t::ComboBoxItem_RemoveAt(
	INT nIndex, // = 0
	INT nCountToRemove // = -1
	) // default parameters - remove all, returns removed count
{
INT nCount = ComboBoxItem_GetCount();
	if( nCountToRemove < 0 )
		nCountToRemove = nCount;
	if( nIndex < 0 || nIndex >= nCountToRemove || nCountToRemove == 0 )
		return 0;
	if( ( nIndex + nCountToRemove ) > nCount )
	{
		nCountToRemove = nCount - nIndex;
		ASSERT( nCountToRemove > 0 );
	}
	INT nPos = nIndex, nLast = nIndex + nCountToRemove - 1;
	for( ; nPos <= nLast; nPos ++ )
	{
		COMBOBOXITEMDATA_t * pCID = ComboBoxItem_GetAt( nPos );
		if( pCID != NULL )
			delete pCID;
	}
	m_arrComboBoxItems.RemoveAt( nIndex, nCountToRemove );
	return nCountToRemove;
}

CExtTreeCtrl::CInplaceEdit::CInplaceEdit( bool bEditingLabel )
	: m_bEditingLabel( bEditingLabel )
	, m_bResultSaved( false )
	, m_bCancelMode( false )
{
}

void CExtTreeCtrl::CInplaceEdit::PreSubclassWindow()
{
	m_bCancelMode = false;
	CExtEdit::PreSubclassWindow();
	HookSpyRegister( __EHSEF_KEYBOARD );
}

void CExtTreeCtrl::CInplaceEdit::PostNcDestroy()
{
	HookSpyUnregister();
	CExtEdit::PostNcDestroy();
	delete this;
}

bool CExtTreeCtrl::CInplaceEdit::OnHookSpyKeyMsg(
	MSG * pMSG
	)
{
	if(		pMSG->hwnd == m_hWnd
		&&	( GetStyle() & WS_VISIBLE ) != 0
		)
	{
		if( pMSG->message == WM_KEYDOWN )
		{
			if( pMSG->wParam == VK_RETURN || pMSG->wParam == VK_ESCAPE )
			{
				HWND hWndParent = ::GetParent( m_hWnd );
				HWND hWndOwn = m_hWnd;
				if( hWndParent != NULL )
				{
					CString strText;
					GetWindowText( strText );
					if( pMSG->wParam == VK_ESCAPE )
					{
						m_bCancelMode = true;
						((CExtTreeCtrl*)CWnd::FromHandlePermanent(hWndParent)) ->
							OnInplaceControlComplete( LPCTSTR(strText), m_bEditingLabel );
					}
					else if( pMSG->wParam == VK_RETURN )
					{
						if( ! m_bResultSaved )
						{
							m_bResultSaved = true;
							((CExtTreeCtrl*)CWnd::FromHandlePermanent(hWndParent)) ->
								OnInplaceControlComplete( LPCTSTR(strText), m_bEditingLabel );
						}
					}
					::InvalidateRect( hWndParent, NULL, FALSE );
				}
				if( ::IsWindow( hWndOwn ) )
					DestroyWindow();
				::SendMessage( hWndParent, WM_CANCELMODE, 0L, 0L );
				return true;
			}
		}
	}
	return false;
}

LRESULT CExtTreeCtrl::CInplaceEdit::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
LRESULT lResult = CExtEdit::WindowProc( message, wParam, lParam );
	switch( message )
	{
	case WM_ACTIVATEAPP:
		if( wParam == 0 )
			SendMessage( WM_CANCELMODE );
	break;
	case WM_NCCALCSIZE:
		{
			if( wParam )
			{
				NCCALCSIZE_PARAMS * myNCSIZE = (NCCALCSIZE_PARAMS*)lParam;
				myNCSIZE->rgrc[0].left += 3 * __EXT_TREE_CTRL_BORDER_GAP;
				myNCSIZE->rgrc[0].top += 2 * __EXT_TREE_CTRL_BORDER_GAP;
				myNCSIZE->rgrc[0].right -= 2 * __EXT_TREE_CTRL_BORDER_GAP;
				myNCSIZE->rgrc[0].bottom -= 2 * __EXT_TREE_CTRL_BORDER_GAP;
			}
			else
			{
				RECT * myRect2 = (RECT*)lParam;
				myRect2->left += 3 * __EXT_TREE_CTRL_BORDER_GAP;
				myRect2->top += 2 * __EXT_TREE_CTRL_BORDER_GAP;
				myRect2->right -= 2 * __EXT_TREE_CTRL_BORDER_GAP;
				myRect2->bottom -= 2 * __EXT_TREE_CTRL_BORDER_GAP;
			}

			return 0;
		}
	break;
	case WM_NCPAINT:
		{
			CRect nonClientRect;
			CRect clientRect;
			CWindowDC dc( this );
			GetWindowRect( &nonClientRect );
			GetClientRect( &clientRect );
			ClientToScreen( &clientRect );
			OffsetRect( &clientRect, - nonClientRect.left, - nonClientRect.top );
			dc.ExcludeClipRect( clientRect.left, clientRect.top, clientRect.right, clientRect.bottom );
			dc.Rectangle( 0, 0, nonClientRect.right - nonClientRect.left, nonClientRect.bottom - nonClientRect.top );
		}
	break;
	case WM_CANCELMODE:
	case WM_KILLFOCUS:
		{
			HWND hWndParent = ::GetParent( m_hWnd );
			if( hWndParent != NULL )
				::InvalidateRect( hWndParent, NULL, FALSE );
			//((CExtTreeCtrl*)CWnd::FromHandlePermanent(hWndParent)) ->
			//	OnInplaceControlCancel();
			if( (!m_bCancelMode) && (!m_bResultSaved) )
			{
				m_bResultSaved = true;
				CString strText;
				GetWindowText( strText );
				((CExtTreeCtrl*)CWnd::FromHandlePermanent(hWndParent)) ->
					OnInplaceControlComplete( LPCTSTR(strText), m_bEditingLabel );
			}
			DestroyWindow();
			::SendMessage( hWndParent, WM_CANCELMODE, 0L, 0L );
		}
	break;
	case WM_GETDLGCODE:
		return ( lResult | DLGC_WANTALLKEYS );
	} // switch( message )
	return lResult;
}

CExtTreeCtrl::CInplaceComboBox::CInplaceComboBox()
	: m_bResultSaved( false )
{
}

void CExtTreeCtrl::CInplaceComboBox::PreSubclassWindow()
{
	CExtComboBox::PreSubclassWindow();
	//HookSpyRegister();
}

void CExtTreeCtrl::CInplaceComboBox::PostNcDestroy()
{
	//HookSpyUnregister();
	CExtComboBox::PostNcDestroy();
	delete this;
}

bool CExtTreeCtrl::CInplaceComboBox::OnHookSpyKeyMsg(
	MSG * pMSG
	)
{
	if(		(	pMSG->hwnd == m_hWnd
			||	::IsChild( m_hWnd, pMSG->hwnd )
			)
		&&	( GetStyle() & WS_VISIBLE ) != 0
		)
	{
		if( pMSG->message == WM_KEYDOWN )
		{
			if( pMSG->wParam == VK_RETURN || pMSG->wParam == VK_ESCAPE )
			{
				HWND hWndParent = ::GetParent( m_hWnd );
				if( hWndParent != NULL )
				{
					if( pMSG->wParam == VK_RETURN )
					{
						if( ! m_bResultSaved )
						{
							m_bResultSaved = true;
							CString strText;
							GetWindowText( strText );
							((CExtTreeCtrl*)CWnd::FromHandlePermanent(hWndParent)) ->
								OnInplaceControlComplete( LPCTSTR(strText), false );
						}
					}
					::InvalidateRect( hWndParent, NULL, FALSE );
				}
				DestroyWindow();
				return TRUE;
			}
		}
	}
	return CExtComboBox::OnHookSpyKeyMsg( pMSG );
}

LRESULT CExtTreeCtrl::CInplaceComboBox::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
HWND hWndOwn = m_hWnd;
	switch( message )
	{
	case WM_ACTIVATEAPP:
		if( wParam == 0 )
			SendMessage( WM_CANCELMODE );
	return 0;
	case WM_CANCELMODE:
		if( ! m_bResultSaved )
		{
			m_bResultSaved = true;
			HWND hWndParent = ::GetParent( m_hWnd );
			if( hWndParent != NULL )
			{
				if( ! m_bResultSaved )
				{
					m_bResultSaved = true;
					CString strText;
					GetWindowText( strText );
					((CExtTreeCtrl*)CWnd::FromHandlePermanent(hWndParent)) ->
						OnInplaceControlComplete( LPCTSTR(strText), false );
				}
			}
		}
		DestroyWindow();
	return 0;
	case WM_KILLFOCUS:
		{
			HWND hWndParent = ::GetParent( hWndOwn );
			if( hWndParent != NULL )
			{
				::InvalidateRect( hWndParent, NULL, FALSE );
				HWND hWndFocus = (HWND)wParam;
				if( ! (		hWndFocus == m_hWnd
						||	::IsChild( m_hWnd, hWndFocus )
						)
					)
				{
					PostMessage( WM_CANCELMODE );
					if( ! m_bResultSaved )
					{
						m_bResultSaved = true;
						CString strText;
						GetWindowText( strText );
						((CExtTreeCtrl*)CWnd::FromHandlePermanent(hWndParent)) ->
							OnInplaceControlComplete( LPCTSTR(strText), false );
					}
					return 0L;
				}
			}
		}
	break;
	} // switch( message )
	if( ! ::IsWindow( hWndOwn ) )
		return 0L;
LRESULT lResult = CExtComboBox::WindowProc( message, wParam, lParam );
	switch( message )
	{
	case WM_GETDLGCODE:
		return ( lResult | DLGC_WANTALLKEYS );
	} // switch( message )
	return lResult;
}

#endif // ( ! defined __EXT_MFC_NO_TREE_VIEW_CTRL )

#if (! defined __EXT_MFC_NO_MSG_BOX )

//////////////////////////////////////////////////////////////////////////////
// CExtMsgBox window

IMPLEMENT_DYNCREATE( CExtMsgBox, CExtResizableDialog );

__EXT_MFC_SAFE_LPCTSTR CExtMsgBox::g_strRegSectionName = _T("ExtMsgBox");

CExtMsgBox::CExtMsgBox(
	HWND hWndParent, // = NULL
	__EXT_MFC_SAFE_LPCTSTR strUniqueID // = NULL // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
	)
	: m_wndEditMsgBoxMessageText( true )
	, m_hWndParentFromCTOR( hWndParent )
	, m_nMsgBoxStyle( __EXT_MB_DEFAULT_STYLES )
	, m_nMsgBoxStyleEx( __EXT_MB_EX_DEFAULT_STYLES )
	, m_nHelpID( 0 )
	, m_nTimeoutSeconds( 0 )
	, m_bIsDisabledTimeout( false )
	, m_nTimeoutTimer( 0 )
	, m_nDefaultButtonDlgCtrlID( UINT(IDC_STATIC) )
	, m_nEscapeButtonDlgCtrlID( UINT(IDC_STATIC) )
	, m_sizeMessageText( 0, 0 )
	, m_sizeCheckBox( 0, 0 )
	, m_sizeButton( 0, 0 )
	, m_strUniqueID( LPCTSTR(strUniqueID) != NULL ? LPCTSTR(strUniqueID) : _T("") ) // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
	, m_sizeBorder( __EXT_MSG_BOX_DEFAULT_WINDOW_BORDER_WIDTH, __EXT_MSG_BOX_DEFAULT_WINDOW_BORDER_HEIGHT )
	, m_sizeStdButton( __EXT_MSG_BOX_DEFAULT_BUTTON_WIDTH, __EXT_MSG_BOX_DEFAULT_BUTTON_HEIGHT )
	, m_sizeStdButtonBorder( __EXT_MSG_BOX_DEFAULT_BUTTON_BORDER_WIDTH, __EXT_MSG_BOX_DEFAULT_BUTTON_BORDER_HEIGHT )
	, m_nCheckBoxGap( __EXT_MSG_BOX_CHECK_BOX_ADDITIONAL_SPACE )
	, m_nButtonSpace( __EXT_MSG_BOX_BTN_SPACE )
	, m_nMinEditCtrlHeight( __EXT_MSG_BOX_DEFAULT_EDITOR_MIN_HEIGHT_PX )
	, m_nMaxEditCtrlHeight( __EXT_MSG_BOX_DEFAULT_EDITOR_MAX_HEIGHT_PX )
	, m_bDynamicIcon( false )
	, m_clrEditorText( __EXT_MSG_BOX_DEFAULT_EDITOR_COLOR_TEXT )
	, m_clrEditorBkgnd( __EXT_MSG_BOX_DEFAULT_EDITOR_COLOR_BKGND )
{
	_AddDefaultIconStyle();
	SetAutoSubclassChildControls();
}

CExtMsgBox::CExtMsgBox(
	HWND hWndParent,
	__EXT_MFC_SAFE_LPCTSTR strMessageText,
	__EXT_MFC_SAFE_LPCTSTR strCaption, // = NULL
	UINT nMsgBoxStyle, // = __EXT_MB_DEFAULT_STYLES
	UINT nHelpID, // = 0
	__EXT_MFC_SAFE_LPCTSTR strUniqueID, // = NULL // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
	UINT nMsgBoxStyleEx // = __EXT_MB_EX_DEFAULT_STYLES
	)
	: m_wndEditMsgBoxMessageText( true )
	, m_hWndParentFromCTOR( hWndParent )
	, m_strMessageText( ( strMessageText == NULL ) ? _T("") : LPCTSTR(strMessageText) )
	, m_strCaption( ( strCaption == NULL || _tcslen(strCaption) == 0 ) ? LPCTSTR( ::AfxGetAppName() ) : LPCTSTR(strCaption) )
	, m_nMsgBoxStyle( nMsgBoxStyle )
	, m_nMsgBoxStyleEx( nMsgBoxStyleEx )
	, m_nHelpID( nHelpID )
	, m_nTimeoutSeconds( 0 )
	, m_bIsDisabledTimeout( false )
	, m_nTimeoutTimer( 0 )
	, m_nDefaultButtonDlgCtrlID( UINT(IDC_STATIC) )
	, m_nEscapeButtonDlgCtrlID( UINT(IDC_STATIC) )
	, m_sizeMessageText( 0, 0 )
	, m_sizeCheckBox( 0, 0 )
	, m_sizeButton( 0, 0 )
	, m_strUniqueID( LPCTSTR(strUniqueID) != NULL ? LPCTSTR(strUniqueID) : _T("") ) // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
	, m_sizeBorder( __EXT_MSG_BOX_DEFAULT_WINDOW_BORDER_WIDTH, __EXT_MSG_BOX_DEFAULT_WINDOW_BORDER_HEIGHT )
	, m_sizeStdButton( __EXT_MSG_BOX_DEFAULT_BUTTON_WIDTH, __EXT_MSG_BOX_DEFAULT_BUTTON_HEIGHT )
	, m_sizeStdButtonBorder( __EXT_MSG_BOX_DEFAULT_BUTTON_BORDER_WIDTH, __EXT_MSG_BOX_DEFAULT_BUTTON_BORDER_HEIGHT )
	, m_nCheckBoxGap( __EXT_MSG_BOX_CHECK_BOX_ADDITIONAL_SPACE )
	, m_nButtonSpace( __EXT_MSG_BOX_BTN_SPACE )
	, m_nMinEditCtrlHeight( __EXT_MSG_BOX_DEFAULT_EDITOR_MIN_HEIGHT_PX )
	, m_nMaxEditCtrlHeight( __EXT_MSG_BOX_DEFAULT_EDITOR_MAX_HEIGHT_PX )
	, m_bDynamicIcon( false )
	, m_clrEditorText( __EXT_MSG_BOX_DEFAULT_EDITOR_COLOR_TEXT )
	, m_clrEditorBkgnd( __EXT_MSG_BOX_DEFAULT_EDITOR_COLOR_BKGND )
{
	_AddDefaultIconStyle();
	SetAutoSubclassChildControls();
}

CExtMsgBox::CExtMsgBox(
	HWND hWndParent,
	UINT nMessageResourceID,
	UINT nCaptionResourceID, // = 0
	UINT nMsgBoxStyle, // = __EXT_MB_DEFAULT_STYLES
	UINT nHelpID, // = 0
	__EXT_MFC_SAFE_LPCTSTR strUniqueID, // = NULL // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
	UINT nMsgBoxStyleEx // = __EXT_MB_EX_DEFAULT_STYLES
	)
	: m_wndEditMsgBoxMessageText( true )
	, m_hWndParentFromCTOR( hWndParent )
	, m_nMsgBoxStyle( nMsgBoxStyle )
	, m_nMsgBoxStyleEx( nMsgBoxStyleEx )
	, m_nHelpID( nHelpID )
	, m_nTimeoutSeconds( 0 )
	, m_bIsDisabledTimeout( false )
	, m_nTimeoutTimer( 0 )
	, m_nDefaultButtonDlgCtrlID( UINT(IDC_STATIC) )
	, m_nEscapeButtonDlgCtrlID( UINT(IDC_STATIC) )
	, m_sizeMessageText( 0, 0 )
	, m_sizeCheckBox( 0, 0 )
	, m_sizeButton( 0, 0 )
	, m_strUniqueID( LPCTSTR(strUniqueID) != NULL ? LPCTSTR(strUniqueID) : _T("") ) // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
	, m_sizeBorder( __EXT_MSG_BOX_DEFAULT_WINDOW_BORDER_WIDTH, __EXT_MSG_BOX_DEFAULT_WINDOW_BORDER_HEIGHT )
	, m_sizeStdButton( __EXT_MSG_BOX_DEFAULT_BUTTON_WIDTH, __EXT_MSG_BOX_DEFAULT_BUTTON_HEIGHT )
	, m_sizeStdButtonBorder( __EXT_MSG_BOX_DEFAULT_BUTTON_BORDER_WIDTH, __EXT_MSG_BOX_DEFAULT_BUTTON_BORDER_HEIGHT )
	, m_nCheckBoxGap( __EXT_MSG_BOX_CHECK_BOX_ADDITIONAL_SPACE )
	, m_nButtonSpace( __EXT_MSG_BOX_BTN_SPACE )
	, m_nMinEditCtrlHeight( __EXT_MSG_BOX_DEFAULT_EDITOR_MIN_HEIGHT_PX )
	, m_nMaxEditCtrlHeight( __EXT_MSG_BOX_DEFAULT_EDITOR_MAX_HEIGHT_PX )
	, m_bDynamicIcon( false )
	, m_clrEditorText( __EXT_MSG_BOX_DEFAULT_EDITOR_COLOR_TEXT )
	, m_clrEditorBkgnd( __EXT_MSG_BOX_DEFAULT_EDITOR_COLOR_BKGND )
{
	_AddDefaultIconStyle();
	SetAutoSubclassChildControls();
	if( nCaptionResourceID == 0 )
		m_strCaption = LPCTSTR( ::AfxGetAppName() );
	else
	{
		VERIFY( g_ResourceManager->LoadString( m_strCaption, nCaptionResourceID ) );
	}
	VERIFY( g_ResourceManager->LoadString( m_strMessageText, nMessageResourceID ) );
}

CExtMsgBox::~CExtMsgBox()
{
	m_bmpIcon.Empty();
	UnRegisterAllButtons();
}

void CExtMsgBox::_AddDefaultIconStyle()
{
	ASSERT_VALID( this );
	if( ! m_bmpIcon.IsEmpty() )
		return;
UINT nMsgBoxStyleEx = MsgBoxStyleExGet();
	if( ( nMsgBoxStyleEx & __EXT_MB_EX_NO_DEFAULT_ICON_STYLE ) != 0 )
		return;
UINT nMsgBoxStyle = MsgBoxStyleGet();
UINT nMsgBoxIconType = nMsgBoxStyle & __EXT_MB_ICONMASK;
	if( nMsgBoxIconType == 0 )
	{
		UINT nMsgBoxType = nMsgBoxStyle & __EXT_MB_TYPEMASK;
		switch( nMsgBoxType )
		{
		case __EXT_MB_OK:
		case __EXT_MB_OKCANCEL:
		case __EXT_MB_YESNO:
		case __EXT_MB_YESNOCANCEL:
			MsgBoxStyleSet( nMsgBoxStyle | __EXT_MB_ICONEXCLAMATION );
		break;
		}
	}
}

void CExtMsgBox::DoDataExchange( CDataExchange * pDX )
{
#if ( _MFC_VER == 0x700 )
	CExtResizableDialog
#else
	CExtNCW < CExtResizableDialog >
#endif
		:: DoDataExchange( pDX );
	//{{AFX_DATA_MAP(CExtNCW)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP( CExtMsgBox, CExtResizableDialog )
	//{{AFX_MSG_MAP(CExtShellDialogCreateFolder)
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
END_MESSAGE_MAP()


UINT CExtMsgBox::MsgBoxStyleGet() const
{
	ASSERT_VALID( this );
	return m_nMsgBoxStyle;
}
void CExtMsgBox::MsgBoxStyleSet( UINT nMsgBoxStyle )
{
	ASSERT_VALID( this );
	m_nMsgBoxStyle = nMsgBoxStyle;
}

UINT CExtMsgBox::MsgBoxStyleExGet() const
{
	ASSERT_VALID( this );
	return m_nMsgBoxStyleEx;
}
void CExtMsgBox::MsgBoxStyleExSet( UINT nMsgBoxStyleEx )
{
	ASSERT_VALID( this );
	m_nMsgBoxStyleEx = nMsgBoxStyleEx;
}

INT CExtMsgBox::MinEditCtrlHeightGet() const
{
	ASSERT_VALID( this );
	return m_nMinEditCtrlHeight;
}

void CExtMsgBox::MinEditCtrlHeightSet( INT nMinEditCtrlHeight )
{
	ASSERT_VALID( this );
	m_nMinEditCtrlHeight = nMinEditCtrlHeight;
}

INT CExtMsgBox::MaxEditCtrlHeightGet() const
{
	ASSERT_VALID( this );
	return m_nMaxEditCtrlHeight;
}

void CExtMsgBox::MaxEditCtrlHeightSet( INT nMaxEditCtrlHeight )
{
	ASSERT_VALID( this );
	m_nMaxEditCtrlHeight = nMaxEditCtrlHeight;
}

CExtSafeString CExtMsgBox::MessageGet() const
{
	ASSERT_VALID( this );
	return m_strMessageText;
}
void CExtMsgBox::MessageSet( __EXT_MFC_SAFE_LPCTSTR strMessageText )
{
	ASSERT_VALID( this );
	m_strMessageText = ( strMessageText == NULL ) ? _T("") : LPCTSTR(strMessageText);
}
void CExtMsgBox::MessageSet( UINT nMessageResourceID )
{
	ASSERT_VALID( this );
CExtSafeString str;
	VERIFY( g_ResourceManager->LoadString( str, nMessageResourceID ) );
	MessageSet( LPCTSTR(str) );
}

CExtSafeString CExtMsgBox::CaptionGet() const
{
	ASSERT_VALID( this );
	return m_strCaption;
}
void CExtMsgBox::CaptionSet( __EXT_MFC_SAFE_LPCTSTR strCaption )
{
	ASSERT_VALID( this );
	m_strCaption = ( strCaption == NULL ) ? _T("") : LPCTSTR(strCaption);
}
void CExtMsgBox::CaptionSet( UINT nCaptionResourceID )
{
	ASSERT_VALID( this );
CExtSafeString str;
	VERIFY( g_ResourceManager->LoadString( str, nCaptionResourceID ) );
	CaptionSet( LPCTSTR(str) );
}

UINT CExtMsgBox::TimeoutGet(
	bool * p_bIsDisabled // = NULL
	) const
{
	ASSERT_VALID( this );
	if( p_bIsDisabled != NULL )
		(*p_bIsDisabled) = m_bIsDisabledTimeout;
	return m_nTimeoutSeconds;
}
void CExtMsgBox::TimeoutSet(
	UINT nTimeoutSeconds,
	bool bIsDisabledTimeout // = false
	)
{
	ASSERT_VALID( this );
	m_nTimeoutSeconds = nTimeoutSeconds;
	m_bIsDisabledTimeout = bIsDisabledTimeout;
}
bool CExtMsgBox::IsDisabledTimeout() const
{
	ASSERT_VALID( this );
bool bIsDisabledTimeout = true;
	TimeoutGet( &bIsDisabledTimeout );
	return bIsDisabledTimeout;
}
bool CExtMsgBox::OnMsgBoxTimeoutDecrease()
{
bool bIsDisabledTimeout = true;
UINT nTimeoutSeconds = TimeoutGet( &bIsDisabledTimeout );
	if( nTimeoutSeconds == 0 )
		return true;
	nTimeoutSeconds --;
	TimeoutSet( nTimeoutSeconds, bIsDisabledTimeout );
	if( nTimeoutSeconds == 0 )
		return true;
	else
		return false;
}

__EXT_MFC_INT_PTR CExtMsgBox::DoModal()
{
	__PROF_UIS_MANAGE_STATE;
	ASSERT_VALID( this );
UINT nMsgBoxStyle = MsgBoxStyleGet();
	if( ( nMsgBoxStyle & (__EXT_MB_DO_NOT_DISPLAY_AGAIN|__EXT_MB_DO_NOT_ASK_AGAIN) ) != 0 )
	{
		CExtSafeString strRegVarName = OnMsgBoxGetRegVarName();
		int nResult = ::AfxGetApp()->GetProfileInt( g_strRegSectionName, LPCTSTR(strRegVarName), (-1) );
		if( nResult != (-1) )
			return nResult;
	}
__EXT_MFC_INT_PTR nRetVal = (__EXT_MFC_INT_PTR)m_nEscapeButtonDlgCtrlID;
const WORD nDlgItems = 0;
DLGTEMPLATE dlgTmpl;
	dlgTmpl.x = 0;
	dlgTmpl.y = 0;
	dlgTmpl.cx = 225;
	dlgTmpl.cy = 152;
	dlgTmpl.style = WS_CAPTION|WS_POPUP|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_SYSMENU|DS_3DLOOK|DS_MODALFRAME|DS_SETFONT;
	if( ( nMsgBoxStyle & __EXT_MB_SYSTEMMODAL ) != 0 )    // +2.91, thanks to Krzysztof Janik
		dlgTmpl.style |= DS_SYSMODAL;
bool bRTL = ( (g_ResourceManager->OnQueryLangLayout()&LAYOUT_RTL) != 0 ) ? true : false;
	dlgTmpl.dwExtendedStyle = bRTL ? WS_EX_LAYOUTRTL : 0;
	if( ( nMsgBoxStyle & __EXT_MB_TOPMOST ) != 0 )        // +2.91, thanks to Krzysztof Janik
		dlgTmpl.dwExtendedStyle |= WS_EX_TOPMOST;
	dlgTmpl.cdit = nDlgItems; // nDlgItems dialog items in the dialog
// The first step is to allocate memory to define the dialog. The information to be stored in the allocated buffer is the following:
// 1. DLGTEMPLATE structure
// 2. 0x0000 (Word) indicating the dialog has no menu
// 3. 0x0000 (Word) Let windows assign default class to the dialog
// 4. (Caption)  Null terminated UNICODE string
WCHAR szBoxCaption[] = L"";
WCHAR szFontName[] = L"MS Sans Serif";
	TRY
	{
		// The following expressions have unnecessary parenthesis trying to make the comments more clear.
		int nBufferSize =  sizeof(DLGTEMPLATE) + (2 * sizeof(WORD)) /*menu and class*/ + sizeof(szBoxCaption);
		nBufferSize += sizeof(WORD) + sizeof(szFontName); /* font information*/
		HLOCAL hLocal = ::LocalAlloc( LHND, nBufferSize );
		if( hLocal == NULL )
			::AfxThrowMemoryException();
		BYTE * pBuffer = (BYTE*)::LocalLock( hLocal );
		if( pBuffer == NULL )
		{
			::LocalFree( hLocal );
			::AfxThrowMemoryException();
		}
		BYTE * pdest = pBuffer;
		// transfer DLGTEMPLATE structure to the buffer
		__EXT_MFC_MEMCPY( pdest, nBufferSize, &dlgTmpl, sizeof(DLGTEMPLATE) );
		pdest += sizeof(DLGTEMPLATE);
		*(WORD*)pdest = 0; // no menu
		*(WORD*)(pdest + sizeof(WORD)) = 0; // use default window class
		pdest += 2 * sizeof(WORD);
		__EXT_MFC_MEMCPY( pdest, nBufferSize, szBoxCaption, sizeof(szBoxCaption) );
		pdest += sizeof(szBoxCaption);
		*(WORD*)pdest = 8; // font size
		pdest += sizeof(WORD);
		__EXT_MFC_MEMCPY( pdest, nBufferSize, szFontName, sizeof(szFontName) );
		pdest += sizeof(szFontName);
		HWND hWndParent = m_hWndParentFromCTOR;
		if( hWndParent == NULL || (! ::IsWindow( hWndParent ) ) )
			hWndParent = ::AfxGetMainWnd() -> GetSafeHwnd();
		InitModalIndirect( (DLGTEMPLATE*)pBuffer, CWnd::FromHandle( hWndParent ) );
#if (defined _DEBUG)
	#if (_MFC_VER < 0x700)
		CWinThread * pWT = ::AfxGetThread();
		int nSavedWTDisablePumpCount = pWT->m_nDisablePumpCount;
		pWT->m_nDisablePumpCount = 0;
	#else
		_AFX_THREAD_STATE * pATH = ::AfxGetThreadState();
		int nSavedWTDisablePumpCount = pATH->m_nDisablePumpCount;
		pATH->m_nDisablePumpCount = 0;
	#endif
#endif
		nRetVal = CDialog::DoModal();
#if (defined _DEBUG)
	#if (_MFC_VER < 0x700)
		pWT->m_nDisablePumpCount = nSavedWTDisablePumpCount;
	#else
		pATH->m_nDisablePumpCount = nSavedWTDisablePumpCount;
	#endif
#endif
		::LocalUnlock( hLocal );
		::LocalFree( hLocal );
	}
	CATCH( CMemoryException,  e)
	{
		TRACE0("Memory allocation for dialog template failed in CExtMsgBox::DoModal().");
	}
	END_CATCH
	return nRetVal;
}

void CExtMsgBox::EndDialog( int nResult )
{
	ASSERT_VALID( this );
bool bDoNotDisplayAgain = false;
CExtCheckBox * pWndCheckBoxDoNotDisplayAgain = DYNAMIC_DOWNCAST( CExtCheckBox, GetDlgItem( ID_EXT_MBB_CHECK_BOX ) );
	if( pWndCheckBoxDoNotDisplayAgain != NULL )
	{
		ASSERT_VALID( pWndCheckBoxDoNotDisplayAgain );
		bDoNotDisplayAgain = ( pWndCheckBoxDoNotDisplayAgain->GetCheck() == BST_CHECKED ) ? true : false;
	}
UINT nMsgBoxStyle = MsgBoxStyleGet();
	if(		( ( nMsgBoxStyle & __EXT_MB_DO_NOT_DISPLAY_AGAIN ) != 0 && bDoNotDisplayAgain )
		||	( ( nMsgBoxStyle & __EXT_MB_DO_NOT_ASK_AGAIN ) != 0 && bDoNotDisplayAgain )
		)
	{
		CExtSafeString strRegVarName = OnMsgBoxGetRegVarName();
		::AfxGetApp()->WriteProfileInt( g_strRegSectionName, LPCTSTR(strRegVarName), nResult );
	}
	if( m_bDynamicIcon )
	{
		m_bmpIcon.Empty();
		m_bDynamicIcon = false;
	}
#if ( _MFC_VER == 0x700 )
	CExtResizableDialog
#else
	CExtNCW < CExtResizableDialog >
#endif
		:: EndDialog(nResult);
}

BOOL CExtMsgBox::OnInitDialog()
{
	if ( !
#if ( _MFC_VER == 0x700 )
			CExtResizableDialog
#else
			CExtNCW < CExtResizableDialog >
#endif
				:: OnInitDialog()
		)
		return FALSE;
	ModifyStyle( 0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS );
	SetWindowText( LPCTSTR(CaptionGet()) );
	SetHelpID( m_nHelpID );
	OnMsgBoxRegisterButtons();
	VERIFY( OnMsgBoxCreateIconLabel() );
	VERIFY( OnMsgBoxCreateMessageLabel() );
	VERIFY( OnMsgBoxCreateCheckBox() );
	VERIFY( OnMsgBoxCreateButtons() );
	SubclassChildControls();
	OnMsgBoxLayout();
UINT nMsgBoxStyle = MsgBoxStyleGet();
	if( ( nMsgBoxStyle & __EXT_MB_NO_SOUND ) == 0 )
	{
		UINT nMsgBoxIconType = nMsgBoxStyle & __EXT_MB_ICONMASK;
		MessageBeep( nMsgBoxIconType );
	}
//	if( ( nMsgBoxStyle & __EXT_MB_SYSTEMMODAL ) != 0 )  // -2.91, moved to CExtMsgBox::DoModal(), thanks to Krzysztof Janik
//		ModifyStyle(0, DS_SYSMODAL);
	if( ( nMsgBoxStyle & __EXT_MB_SETFOREGROUND ) != 0 )
		SetForegroundWindow();
//	if( ( nMsgBoxStyle & __EXT_MB_TOPMOST ) != 0 )      // -2.91, moved to CExtMsgBox::DoModal(), thanks to Krzysztof Janik
//		ModifyStyleEx(0, WS_EX_TOPMOST);
	if( m_nEscapeButtonDlgCtrlID == UINT(IDC_STATIC) )
		GetSystemMenu( FALSE ) -> EnableMenuItem( SC_CLOSE, MF_GRAYED );
bool bIsDisabledTimeout = true;
UINT nTimeoutSeconds = TimeoutGet( &bIsDisabledTimeout );
	if( nTimeoutSeconds > 0 )
	{
		if( bIsDisabledTimeout )
		{
			INT nButtonIndex, nButtonsCount( INT( m_arrMsgBoxButtons.GetSize() ) );
			for( nButtonIndex = 0; nButtonIndex < nButtonsCount; nButtonIndex ++ )
			{
				CWnd * pWndButton = GetDlgItem( m_arrMsgBoxButtons[ nButtonIndex ] -> m_nButtonDlgCtrlID );
				if( pWndButton != NULL )
				{
					ASSERT_VALID( pWndButton );
					pWndButton->EnableWindow( FALSE );
				}
			}
			CExtCheckBox * pWndCheckBoxDoNotDisplayAgain = DYNAMIC_DOWNCAST( CExtCheckBox, GetDlgItem( ID_EXT_MBB_CHECK_BOX ) );
			if( pWndCheckBoxDoNotDisplayAgain != NULL )
			{
				ASSERT_VALID( pWndCheckBoxDoNotDisplayAgain );
				pWndCheckBoxDoNotDisplayAgain->EnableWindow( FALSE );
			}
		}
		m_nTimeoutTimer = (UINT)SetTimer( 678, 1000, NULL );
	}
	if( m_nDefaultButtonDlgCtrlID != UINT(IDC_STATIC) )
	{
		CWnd * pWndButton = GetDlgItem( m_nDefaultButtonDlgCtrlID );
		if( pWndButton != NULL )
		{
			ASSERT_VALID( pWndButton );
			pWndButton->SetFocus();
		}
		//SetDefID( m_nDefaultButtonDlgCtrlID );
		return FALSE; // focus is set
	}
	return TRUE;
}

BOOL CExtMsgBox::OnCmdMsg( UINT nID, int nCode, void * pExtra, AFX_CMDHANDLERINFO * pHandlerInfo )
{
	ASSERT_VALID( this );
	if( ( nID == ID_EXT_MBB_HELP ) && ( nCode == CN_COMMAND ) )
	{
		OnMsgBoxHelp();
		return TRUE;
	}
	if( ( nID != UINT(IDC_STATIC) ) && ( nID != ID_EXT_MBB_CHECK_BOX ) && ( nCode == CN_COMMAND ) )
	{
		if( nID == ID_EXT_MBB_EX_COPY_BUTTON )
			OnMsgBoxCopyToClipboard();
		else
			EndDialog( nID );
		return TRUE;
	}
	return
#if ( _MFC_VER == 0x700 )
		CExtResizableDialog
#else
		CExtNCW < CExtResizableDialog >
#endif
			:: OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

BOOL CExtMsgBox::PreTranslateMessage( MSG * pMsg )
{
	if( pMsg->message == WM_KEYDOWN )
	{
		bool bIsDisabledTimeout = true;
		UINT nTimeoutSeconds = TimeoutGet( &bIsDisabledTimeout );
		if ( bIsDisabledTimeout && nTimeoutSeconds > 0 )
			return TRUE;
		if( pMsg->wParam == VK_RETURN )
		{
			CWnd * pFocusWnd = GetFocus();
			if( pFocusWnd != NULL )
			{
				UINT nDlgCtrlID = UINT( pFocusWnd->GetDlgCtrlID() );
				INT nButtonIndex, nButtonsCount( INT( m_arrMsgBoxButtons.GetSize() ) );
				for( nButtonIndex = 0; nButtonIndex < nButtonsCount; nButtonIndex ++ )
				{
					if ( m_arrMsgBoxButtons[ nButtonIndex ] -> m_nButtonDlgCtrlID == nDlgCtrlID )
					{
						m_nDefaultButtonDlgCtrlID = nDlgCtrlID;
						break;
					}
				}
				EndDialog( m_nDefaultButtonDlgCtrlID );
				return TRUE;
			}
		}
		else if( pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_CANCEL )
		{
			if( m_nEscapeButtonDlgCtrlID != UINT(IDC_STATIC) )
				EndDialog( m_nEscapeButtonDlgCtrlID );
			return TRUE;
		}
		else if(
				( pMsg->wParam == WPARAM('C') || pMsg->wParam == VK_INSERT )
			&&	( HIWORD(pMsg->lParam) & KF_ALTDOWN ) == 0
			&&	( ::GetKeyState(VK_SHIFT) & 0x80000000 ) == 0
			&&	( ::GetKeyState(VK_CONTROL) & 0x80000000 ) != 0
			)
		{
			UINT nMsgBoxStyleEx = MsgBoxStyleExGet();
			if( ( nMsgBoxStyleEx & __EXT_MB_EX_DISABLE_KEYBOARD_COPY_ACCEL ) == 0 )
				OnMsgBoxCopyToClipboard();
			return TRUE;
		}
		else if( pMsg->wParam == VK_F1 )
		{
			UINT nMsgBoxStyle = MsgBoxStyleExGet();
			if( ( nMsgBoxStyle & __EXT_MB_HELP ) != 0 )
				OnMsgBoxHelp();
			return TRUE;
		}

	}
	return
#if ( _MFC_VER == 0x700 )
		CExtResizableDialog
#else
		CExtNCW < CExtResizableDialog >
#endif
			:: PreTranslateMessage( pMsg );
}

void CExtMsgBox::OnTimer( __EXT_MFC_UINT_PTR nIDEvent )
{
	if( nIDEvent == 678 )
	{
		INT nButtonIndex, nButtonsCount( INT( m_arrMsgBoxButtons.GetSize() ) );
		if( OnMsgBoxTimeoutDecrease() )
		{
			KillTimer( m_nTimeoutTimer );
			if ( IsDisabledTimeout() )
			{
				for( nButtonIndex = 0; nButtonIndex < nButtonsCount; nButtonIndex ++ )
				{
					CWnd * pWndButton = GetDlgItem( m_arrMsgBoxButtons[ nButtonIndex ] -> m_nButtonDlgCtrlID );
					if( pWndButton != NULL )
					{
						ASSERT_VALID( pWndButton );
						pWndButton->EnableWindow( TRUE );
					}
				}
				CExtCheckBox * pWndCheckBoxDoNotDisplayAgain = DYNAMIC_DOWNCAST( CExtCheckBox, GetDlgItem( ID_EXT_MBB_CHECK_BOX ) );
				if( pWndCheckBoxDoNotDisplayAgain != NULL )
				{
					ASSERT_VALID( pWndCheckBoxDoNotDisplayAgain );
					pWndCheckBoxDoNotDisplayAgain->EnableWindow( TRUE );
				}
			}
			else
				EndDialog( m_nDefaultButtonDlgCtrlID );
		}
		for( nButtonIndex = 0; nButtonIndex < nButtonsCount; nButtonIndex ++ )
		{
			if ( m_arrMsgBoxButtons[ nButtonIndex ] -> m_nButtonDlgCtrlID == m_nDefaultButtonDlgCtrlID )
			{
				CExtSafeString strButtonText = m_arrMsgBoxButtons[ nButtonIndex ] -> m_strCaption;
				CExtSafeString strFullText = strButtonText;
				UINT nTimeoutSeconds = TimeoutGet();
				if( nTimeoutSeconds > 0 )
					strFullText.Format( _T("%s = %d"), strButtonText, nTimeoutSeconds );
				CWnd * pWndButton = GetDlgItem( m_arrMsgBoxButtons[ nButtonIndex ] -> m_nButtonDlgCtrlID );
				if( pWndButton != NULL )
				{
					ASSERT_VALID( pWndButton );
					pWndButton->SetWindowText( strFullText );
				}
			}
		}
	}
#if ( _MFC_VER == 0x700 )
	CExtResizableDialog
#else
	CExtNCW < CExtResizableDialog >
#endif
		:: OnTimer( nIDEvent );
}

BOOL CExtMsgBox::OnWndMsg( UINT message, WPARAM wParam, LPARAM lParam, LRESULT * pResult )
{
	ASSERT_VALID( this );
	if ( message == WM_CLOSE )
	{
		bool bIsDisabledTimeout = true;
		UINT nTimeoutSeconds = TimeoutGet( &bIsDisabledTimeout );
		if ( bIsDisabledTimeout && nTimeoutSeconds > 0 )
			return TRUE;
		if( m_nEscapeButtonDlgCtrlID != UINT(IDC_STATIC) )
			EndDialog( m_nEscapeButtonDlgCtrlID );
		return TRUE;
	}
	return
#if ( _MFC_VER == 0x700 )
		CExtResizableDialog
#else
		CExtNCW < CExtResizableDialog >
#endif
			:: OnWndMsg( message, wParam, lParam, pResult );
}

void CExtMsgBox::RegisterButton(
	UINT nButtonDlgCtrlID,
	__EXT_MFC_SAFE_LPCTSTR strCaption,
	bool bIsDefault, //  = false
	bool bIsEscape // = false
	)
{
	ASSERT_VALID( this );
msgbox_button_info_t * p_mbbi = new msgbox_button_info_t( nButtonDlgCtrlID, strCaption );
	m_arrMsgBoxButtons.Add( p_mbbi );
	if( bIsDefault )
		m_nDefaultButtonDlgCtrlID = nButtonDlgCtrlID;
	if( bIsEscape )
		m_nEscapeButtonDlgCtrlID = nButtonDlgCtrlID;
}

void CExtMsgBox::RegisterButton(
	UINT nButtonDlgCtrlID,
	UINT nCaptionResourceID,
	bool bIsDefault, //  = false
	bool bIsEscape // = false
	)
{
	ASSERT_VALID( this );
CExtSafeString strCaption;
	VERIFY( g_ResourceManager->LoadString( strCaption, nCaptionResourceID ) );
	RegisterButton( nButtonDlgCtrlID, LPCTSTR(strCaption), bIsDefault, bIsEscape );
}

void CExtMsgBox::UnRegisterAllButtons()
{
	ASSERT_VALID( this );
INT nButtonIndex, nButtonsCount( INT( m_arrMsgBoxButtons.GetSize() ) );
	for( nButtonIndex = 0; nButtonIndex < nButtonsCount; nButtonIndex ++ )
	{
		msgbox_button_info_t * p_mbbi = m_arrMsgBoxButtons[ nButtonIndex ];
		ASSERT( p_mbbi != NULL );
		delete p_mbbi;
	}
	m_arrMsgBoxButtons.RemoveAll();
}

CExtSafeString CExtMsgBox::OnMsgBoxGetRegVarName() const // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
{
	ASSERT_VALID( this );
CExtSafeString strMessageText = MessageGet();
INT nChar, nLen = INT( strMessageText.GetLength() ), nChecksum = 0;
	for( nChar = 0; nChar < nLen; nChar++ )
		nChecksum += INT(strMessageText.GetAt(nChar)) * nChar;
CExtSafeString strRegVarName;
	strRegVarName.Format(
		_T("msgbox-%s-helpid-%d-chksum-%d"),
		m_strUniqueID.IsEmpty() ? _T("not-unique") : LPCTSTR(m_strUniqueID),
		m_nHelpID,
		nChecksum
		);
	return strRegVarName;
}

void CExtMsgBox::OnMsgBoxRegisterButtons()
{
	ASSERT_VALID( this );
UINT nMsgBoxStyle = MsgBoxStyleGet();
UINT nMsgBoxStyleEx = MsgBoxStyleExGet();
	if( ( nMsgBoxStyleEx & (__EXT_MB_EX_COPY_BUTTON|__EXT_MB_EX_COPY_BTN_FIRST) ) == (__EXT_MB_EX_COPY_BUTTON|__EXT_MB_EX_COPY_BTN_FIRST) )
		RegisterButton( ID_EXT_MBB_EX_COPY_BUTTON, IDS_EX_MB_EX_COPY_BUTTON );
UINT nMsgBoxType = nMsgBoxStyle & __EXT_MB_TYPEMASK;
	switch( nMsgBoxType )
	{
	case __EXT_MB_OKCANCEL:
		RegisterButton( ID_EXT_MBB_OK, IDS_EX_MB_OK, true );
		RegisterButton( ID_EXT_MBB_CANCEL, IDS_EX_MB_CANCEL, false, true );
	break;
	case __EXT_MB_ABORTRETRYIGNORE:
		RegisterButton( ID_EXT_MBB_ABORT, IDS_EX_MB_ABORT, true );
		RegisterButton( ID_EXT_MBB_RETRY, IDS_EX_MB_RETRY );
		RegisterButton( ID_EXT_MBB_IGNORE, IDS_EX_MB_IGNORE );
	break;
	case __EXT_MB_YESNOCANCEL:
		RegisterButton( ID_EXT_MBB_YES, IDS_EX_MB_YES, true );
		if( ( nMsgBoxStyle & __EXT_MB_YES_TO_ALL ) != 0 )
			RegisterButton( ID_EXT_MBB_YES_TO_ALL, IDS_EX_MB_YES_TO_ALL );
		RegisterButton( ID_EXT_MBB_NO, IDS_EX_MB_NO );
		if( ( nMsgBoxStyle & __EXT_MB_NO_TO_ALL ) != 0 )
			RegisterButton( ID_EXT_MBB_NO_TO_ALL, IDS_EX_MB_NO_TO_ALL );
		RegisterButton( ID_EXT_MBB_CANCEL, IDS_EX_MB_CANCEL, false, true );
	break;
	case __EXT_MB_YESNO:
		RegisterButton( ID_EXT_MBB_YES, IDS_EX_MB_YES, true );
		if( ( nMsgBoxStyle & __EXT_MB_YES_TO_ALL ) != 0 )
			RegisterButton( ID_EXT_MBB_YES_TO_ALL, IDS_EX_MB_YES_TO_ALL );
		RegisterButton( ID_EXT_MBB_NO, IDS_EX_MB_NO );
		if( ( nMsgBoxStyle & __EXT_MB_NO_TO_ALL ) != 0 )
			RegisterButton( ID_EXT_MBB_NO_TO_ALL, IDS_EX_MB_NO_TO_ALL );
	break;
	case __EXT_MB_RETRYCANCEL:
		RegisterButton( ID_EXT_MBB_RETRY, IDS_EX_MB_RETRY, true );
		RegisterButton( ID_EXT_MBB_CANCEL, IDS_EX_MB_CANCEL, false, true );
	break;
	case __EXT_MB_CANCELTRYCONTINUE:
		RegisterButton( ID_EXT_MBB_CANCEL, IDS_EX_MB_CANCEL, true, true );
		RegisterButton( ID_EXT_MBB_TRY_AGAIN, IDS_EX_MB_RETRY );
		RegisterButton( ID_EXT_MBB_CONTINUE, IDS_EX_MB_CONTINUE );
	break;
	case __EXT_MB_CONTINUEABORT:
		RegisterButton( ID_EXT_MBB_CONTINUE, IDS_EX_MB_CONTINUE, true );
		RegisterButton( ID_EXT_MBB_ABORT, IDS_EX_MB_ABORT );
	break;
	case __EXT_MB_SKIPSKIPALLCANCEL:
		RegisterButton( ID_EXT_MBB_SKIP, IDS_EX_MB_SKIP, true );
		RegisterButton( ID_EXT_MBB_SKIP_ALL, IDS_EX_MB_SKIPALL );
		RegisterButton( ID_EXT_MBB_CANCEL, IDS_EX_MB_CANCEL, false, true );
	break;
	case __EXT_MB_IGNOREIGNOREALLCANCEL:
		RegisterButton( ID_EXT_MBB_IGNORE, IDS_EX_MB_IGNORE, true );
		RegisterButton( ID_EXT_MBB_IGNORE_ALL, IDS_EX_MB_IGNOREALL );
		RegisterButton( ID_EXT_MBB_CANCEL, IDS_EX_MB_CANCEL, FALSE, true );
	break;
	case __EXT_MB_OK:
	default:
		RegisterButton( ID_EXT_MBB_OK, IDS_EX_MB_OK, true, true );
	break;
	}
	if( ( nMsgBoxStyle & __EXT_MB_HELP ) != 0 )
		RegisterButton( ID_EXT_MBB_HELP, IDS_EX_MB_HELP );
	if( ( nMsgBoxStyleEx & (__EXT_MB_EX_COPY_BUTTON|__EXT_MB_EX_COPY_BTN_FIRST) ) == __EXT_MB_EX_COPY_BUTTON )
		RegisterButton( ID_EXT_MBB_EX_COPY_BUTTON, IDS_EX_MB_EX_COPY_BUTTON );
UINT nDefStyle = nMsgBoxStyle & __EXT_MB_DEFMASK;
	if( nDefStyle != 0 )
	{
		int nDefaultIndex = 0;
		switch( nDefStyle )
		{
		case __EXT_MB_DEFBUTTON1: nDefaultIndex = 0; break;
		case __EXT_MB_DEFBUTTON2: nDefaultIndex = 1; break;
		case __EXT_MB_DEFBUTTON3: nDefaultIndex = 2; break;
		case __EXT_MB_DEFBUTTON4: nDefaultIndex = 3; break;
		case __EXT_MB_DEFBUTTON5: nDefaultIndex = 4; break;
		case __EXT_MB_DEFBUTTON6: nDefaultIndex = 5; break;
		}
		if ( m_arrMsgBoxButtons.GetSize() >= ( nDefaultIndex + 1 ) )
			m_nDefaultButtonDlgCtrlID = m_arrMsgBoxButtons[ nDefaultIndex ] -> m_nButtonDlgCtrlID;
	}
	m_bDynamicIcon = m_bmpIcon.IsEmpty() ? true : false;
	if( m_bDynamicIcon )
	{
		UINT nMsgBoxIconType = nMsgBoxStyle & __EXT_MB_ICONMASK;
		if( nMsgBoxIconType != 0 )
		{
			LPCTSTR strStdIconID = NULL;
			switch( nMsgBoxIconType )
			{
			case __EXT_MB_ICONEXCLAMATION: strStdIconID = IDI_EXCLAMATION; break;
			case __EXT_MB_ICONHAND:        strStdIconID = IDI_HAND;        break;
			case __EXT_MB_ICONQUESTION:    strStdIconID = IDI_QUESTION;    break;
			case __EXT_MB_ICONASTERISK:    strStdIconID = IDI_ASTERISK;    break;
			}
			if( strStdIconID != NULL )
			{
				HICON hIcon = ::AfxGetApp() -> LoadStandardIcon( strStdIconID );
				if( hIcon != NULL )
				{
					m_bmpIcon.AssignFromHICON( hIcon );
					::DestroyIcon( hIcon );
				}
			}
		}
	}
	m_bDynamicIcon = m_bmpIcon.IsEmpty() ? true : false;
}

bool CExtMsgBox::OnMsgBoxCreateIconLabel()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	if( m_bmpIcon.IsEmpty() )
		return true;
CRect rc( 0, 0, 0, 0 );
DWORD dwStyle =
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_DISABLED
		|SS_CENTER|SS_CENTERIMAGE;
	if( ! m_wndLabelMsgBoxIcon.Create( NULL, dwStyle, rc, this, UINT(IDC_STATIC) ) )
		return false;
	m_wndLabelMsgBoxIcon.SetBitmapEx( &m_bmpIcon );
	m_wndLabelMsgBoxIcon.SetImageMode( CExtLabel::eAlign );
	return true;
}

bool CExtMsgBox::OnMsgBoxCreateMessageLabel()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
CDC dcDisplay;
	if( ! dcDisplay.CreateDC(_T("DISPLAY"), NULL, NULL, NULL) )
		return false;
UINT nMsgBoxStyle = MsgBoxStyleGet();
UINT nMsgBoxStyleEx = MsgBoxStyleExGet();
CFont * pOldFont = dcDisplay.SelectObject( GetFont() );
int nMaxWidth = ( ::GetSystemMetrics( SM_CXSCREEN ) / 2 ) + 100;
CSize sizeIcon = m_bmpIcon.GetSize();
	if( sizeIcon.cx > 0 )
		nMaxWidth -= sizeIcon.cx + 2 * _DLU2PxHorz( m_sizeBorder.cx );
CRect rcMessage( 0, 0, nMaxWidth, nMaxWidth );
	CExtRichContentLayout::stat_DrawText(dcDisplay.m_hDC, LPCTSTR(m_strMessageText), m_strMessageText.GetLength(), rcMessage, DT_LEFT|DT_NOPREFIX|DT_WORDBREAK|DT_EXPANDTABS|DT_CALCRECT, 0 );
	m_sizeMessageText = rcMessage.Size();
	dcDisplay.SelectObject( pOldFont );
	dcDisplay.DeleteDC();
	if( ( nMsgBoxStyleEx & __EXT_MB_EX_USE_EDIT_CONTROL ) != 0 )
	{
		if( ( nMsgBoxStyleEx & __EXT_MB_EX_SCROLLABLE_EDIT ) !=0 )
			m_sizeMessageText.cx += ::GetSystemMetrics( SM_CXVSCROLL );
		INT nMinEditCtrlHeight = MinEditCtrlHeightGet();
		if( nMinEditCtrlHeight > 0 )
			m_sizeMessageText.cy = max( m_sizeMessageText.cy, nMinEditCtrlHeight );
		INT nMaxEditCtrlHeight = MaxEditCtrlHeightGet();
		if( nMaxEditCtrlHeight > 0 )
			m_sizeMessageText.cy = min( m_sizeMessageText.cy, nMaxEditCtrlHeight );
	}
CRect rc( 0, 0, 0, 0 );
DWORD dwStyle = WS_CHILD|WS_VISIBLE;
CExtSafeString strMessageText = MessageGet();
	if( ( nMsgBoxStyleEx & __EXT_MB_EX_USE_EDIT_CONTROL ) != 0 )
	{
		if( ( nMsgBoxStyle & __EXT_MB_RIGHT ) != 0 )
			dwStyle |= ES_RIGHT;
		else
			dwStyle |= ES_LEFT;
		dwStyle |=
			WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_TABSTOP
			|ES_MULTILINE|ES_NOHIDESEL|ES_READONLY|ES_AUTOVSCROLL;
		if( ( nMsgBoxStyleEx & __EXT_MB_EX_SCROLLABLE_EDIT ) != 0 )
			dwStyle |= WS_VSCROLL;
		if( ! m_wndEditMsgBoxMessageText.Create( dwStyle, rc, this, UINT(IDC_STATIC) ) )
			return false;
		if( m_clrEditorText != COLORREF(-1L) )
			m_wndEditMsgBoxMessageText.SetTextColor( m_clrEditorText );
		if( m_clrEditorBkgnd != COLORREF(-1L) )
			m_wndEditMsgBoxMessageText.SetBkColor( m_clrEditorBkgnd );
		if( ( nMsgBoxStyle & __EXT_MB_RTLREADING ) != 0 )
			m_wndEditMsgBoxMessageText.ModifyStyleEx( 0, WS_EX_RTLREADING );
		m_wndEditMsgBoxMessageText.SetFont( GetFont() );
		m_wndEditMsgBoxMessageText.SetWindowText( LPCTSTR(strMessageText) );
	}
	else
	{
		dwStyle |= SS_NOPREFIX;
		if( ( nMsgBoxStyle & __EXT_MB_RIGHT ) != 0 )
			dwStyle |= SS_RIGHT;
		else
			dwStyle |= SS_LEFT;
		if( ! m_wndLabelMsgBoxMessageText.Create( LPCTSTR(strMessageText),  dwStyle|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, rc, this, UINT(IDC_STATIC) ) )
			return false;
		if( ( nMsgBoxStyle & __EXT_MB_RTLREADING ) != 0 )
			m_wndLabelMsgBoxMessageText.ModifyStyleEx( 0, WS_EX_RTLREADING );
		m_wndLabelMsgBoxMessageText.SetFont( GetFont() );
	}
	return true;
}

bool CExtMsgBox::OnMsgBoxCreateCheckBox()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
UINT nMsgBoxStyle = MsgBoxStyleGet();
UINT nMsgBoxStyleEx = MsgBoxStyleExGet();
	if( ( nMsgBoxStyle & (__EXT_MB_DO_NOT_DISPLAY_AGAIN|__EXT_MB_DO_NOT_ASK_AGAIN) ) != 0 )
	{
		CExtSafeString strCheckboxCaption;
		if( ( nMsgBoxStyle & __EXT_MB_DO_NOT_DISPLAY_AGAIN ) != 0 )
		{
			VERIFY( g_ResourceManager->LoadString( strCheckboxCaption, IDS_EX_MB_DO_NOT_DISPLAY_AGAIN ) );
		}
		else if( ( nMsgBoxStyle & __EXT_MB_DO_NOT_ASK_AGAIN ) != 0 )
		{
			VERIFY( g_ResourceManager->LoadString( strCheckboxCaption, IDS_EX_MB_DO_NOT_ASK_AGAIN ) );
		}
		CClientDC dc( this );
		CFont * pWndFont = GetFont();
		CFont * pOldFont = dc.SelectObject( pWndFont );
		m_sizeCheckBox = CExtPaintManager::stat_CalcTextDimension( dc, *pWndFont, strCheckboxCaption ).Size();
		m_sizeCheckBox.cx += _DLU2PxHorz( m_nCheckBoxGap );
		dc.SelectObject( pOldFont );
		CRect rc( 0, 0, 0, 0 );
		CButton wnd;
		if( ! wnd.Create( LPCTSTR(strCheckboxCaption), WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_TABSTOP|BS_AUTOCHECKBOX, rc, this, ID_EXT_MBB_CHECK_BOX ) )
			return false;
		if( ( nMsgBoxStyle & __EXT_MB_CHECK_BOX_PRESENT ) != 0 )
			wnd.SetCheck( ( ( nMsgBoxStyleEx & __EXT_MB_EX_CHECK_BOX_IS_NOT_CHECKED_INTIALLY ) == 0 ) ? BST_CHECKED : BST_UNCHECKED );
		wnd.SetFont( pWndFont );
		wnd.UnsubclassWindow();
	}
	return true;
}

bool CExtMsgBox::OnMsgBoxCreateButtons()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	m_sizeButton.cx = _DLU2PxHorz( m_sizeStdButton.cx );
	m_sizeButton.cy = _DLU2PxVert( m_sizeStdButton.cy );
CClientDC dc( this );
CFont * pWndFont = GetFont();
CFont * pOldFont = dc.SelectObject( pWndFont );
CRect rc( 0, 0, 0, 0 );
INT nButtonIndex, nButtonsCount( INT( m_arrMsgBoxButtons.GetSize() ) );
	for( nButtonIndex = 0; nButtonIndex < nButtonsCount; nButtonIndex ++ )
	{
		CExtSafeString strButtonCaption = m_arrMsgBoxButtons[ nButtonIndex ] -> m_strCaption;
		CExtSafeString strLengthTest = strButtonCaption;
		UINT nTimeoutSeconds = TimeoutGet();
		if ( nTimeoutSeconds > 0 )
		{
			CString strTimeoutText;
			strTimeoutText.Format( _T("%s = %d"), strLengthTest, nTimeoutSeconds );
			strLengthTest = strTimeoutText;
		}
		CSize sButtonText = CExtPaintManager::stat_CalcTextDimension( dc, *pWndFont, strLengthTest ).Size();
		m_sizeButton.cx = max( m_sizeButton.cx, sButtonText.cx );
		m_sizeButton.cy = max( m_sizeButton.cy, sButtonText.cy );
		CButton wnd;
		if( ! wnd.Create(
				LPCTSTR(strButtonCaption),
				WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_TABSTOP
					| ( ( m_arrMsgBoxButtons[ nButtonIndex ] -> m_nButtonDlgCtrlID == m_nDefaultButtonDlgCtrlID ) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON )
					,
				rc,
				this,
				m_arrMsgBoxButtons[ nButtonIndex ] -> m_nButtonDlgCtrlID
				)
			)
			return false;
		wnd.SetFont( pWndFont );
		wnd.UnsubclassWindow();
	}
	m_sizeButton.cx += 2 * _DLU2PxHorz( m_sizeStdButtonBorder.cx );
	m_sizeButton.cy += 2 * _DLU2PxVert( m_sizeStdButtonBorder.cy );
	dc.SelectObject( pOldFont );
	return true;
}

void CExtMsgBox::OnMsgBoxHelp()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	OnHelp();
}

void CExtMsgBox::OnMsgBoxLayout()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
INT nButtonIndex, nButtonsCount( INT( m_arrMsgBoxButtons.GetSize() ) );
CSize sizeBorderPX( _DLU2PxHorz( m_sizeBorder.cx ), _DLU2PxVert( m_sizeBorder.cy ) );
CSize sizeClient = CSize( 2 * sizeBorderPX.cx, 2 * sizeBorderPX.cy );
CPoint ptPos( sizeBorderPX.cx, sizeBorderPX.cy );
CSize sizeIcon = m_bmpIcon.GetSize();
	if( sizeIcon.cx > 0 )
	{
		m_wndLabelMsgBoxIcon.MoveWindow( sizeBorderPX.cx, sizeBorderPX.cy, sizeIcon.cx, sizeIcon.cy );
		sizeClient.cx += sizeIcon.cx + sizeBorderPX.cx;
		sizeClient.cy += sizeIcon.cy + sizeBorderPX.cy;
		ptPos.x += sizeIcon.cx + sizeBorderPX.cx;
	}
	sizeClient.cx += m_sizeMessageText.cx + sizeBorderPX.cx;
	sizeClient.cy = max( sizeClient.cy, m_sizeMessageText.cy + 2 *  sizeBorderPX.cy + _DLU2PxVert( m_sizeBorder.cy / 2 ) );
	if( m_wndLabelMsgBoxMessageText.GetSafeHwnd() != NULL )
		m_wndLabelMsgBoxMessageText.MoveWindow( ptPos.x, ptPos.y, m_sizeMessageText.cx, m_sizeMessageText.cy );
	if( m_wndEditMsgBoxMessageText.GetSafeHwnd() != NULL )
		m_wndEditMsgBoxMessageText.MoveWindow( ptPos.x, ptPos.y, m_sizeMessageText.cx, m_sizeMessageText.cy );
	ptPos.y += m_sizeMessageText.cy + sizeBorderPX.cy + _DLU2PxVert( m_sizeBorder.cy / 2 );
UINT nMsgBoxStyle = MsgBoxStyleGet();
	if( ( nMsgBoxStyle & (__EXT_MB_DO_NOT_DISPLAY_AGAIN|__EXT_MB_DO_NOT_ASK_AGAIN) ) != 0 )
	{
		CExtCheckBox * pWndCheckBoxDoNotDisplayAgain = DYNAMIC_DOWNCAST( CExtCheckBox, GetDlgItem( ID_EXT_MBB_CHECK_BOX ) );
		if( pWndCheckBoxDoNotDisplayAgain != NULL )
		{
			ASSERT_VALID( pWndCheckBoxDoNotDisplayAgain );
			pWndCheckBoxDoNotDisplayAgain->MoveWindow( ptPos.x, ptPos.y, m_sizeCheckBox.cx, m_sizeCheckBox.cy );
			sizeClient.cx = max( sizeClient.cx, ptPos.x + m_sizeCheckBox.cx + sizeBorderPX.cx );
			sizeClient.cy = max( sizeClient.cy, ptPos.y + m_sizeCheckBox.cy + sizeBorderPX.cy );
			ptPos.y += m_sizeCheckBox.cy + sizeBorderPX.cy;
		}
	}
INT nButtonSpacePX = _DLU2PxHorz( m_nButtonSpace );
INT cxButtons = ( nButtonsCount - 1 ) * nButtonSpacePX + nButtonsCount * m_sizeButton.cx;
INT cyButtons = m_sizeButton.cy;
	sizeClient.cx = max(sizeClient.cx, 2 * sizeBorderPX.cx + cxButtons);
	sizeClient.cy += cyButtons + sizeBorderPX.cy;
INT nXButtonPosition = ( sizeClient.cx - cxButtons ) / 2;
INT nYButtonPosition = sizeClient.cy - sizeBorderPX.cy - m_sizeButton.cy;
	if( ( nMsgBoxStyle & __EXT_MB_RIGHT_ALIGN ) != 0 )
		nXButtonPosition = sizeClient.cx - cxButtons - sizeBorderPX.cx;
	for( nButtonIndex = 0; nButtonIndex < nButtonsCount; nButtonIndex ++ )
	{
		CWnd * pWndButton = GetDlgItem( m_arrMsgBoxButtons[ nButtonIndex ] -> m_nButtonDlgCtrlID );
		if ( pWndButton != NULL )
		{
			ASSERT_VALID( pWndButton );
			pWndButton->MoveWindow( nXButtonPosition, nYButtonPosition, m_sizeButton.cx, m_sizeButton.cy );
			nXButtonPosition += m_sizeButton.cx + nButtonSpacePX;
		}
	}
CRect rcClient( 0, 0, sizeClient.cx, sizeClient.cy );
	CalcWindowRect( rcClient );
	MoveWindow( rcClient );
	CenterWindow();
}

bool CExtMsgBox::OnMsgBoxCopyToClipboard()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
CExtSafeString strMessageText = MessageGet();
INT nMessageTextLength = INT( strMessageText.GetLength() );
	if( ! ::OpenClipboard( m_hWnd ) )
		return false;
	::EmptyClipboard();
bool bRetVal = false;
HGLOBAL hGlobal =
		::GlobalAlloc( 
			GMEM_DDESHARE, 
			( nMessageTextLength + 1 ) * sizeof(TCHAR) 
			);
	ASSERT( hGlobal != NULL );
	if( hGlobal != NULL )
	{
		LPTSTR lpszBuffer = 
			(LPTSTR) ::GlobalLock( hGlobal );
		__EXT_MFC_STRCPY(
			lpszBuffer, 
			nMessageTextLength + 1,
			( nMessageTextLength == 0 ) ? _T("") : LPCTSTR(strMessageText)
			);
		::GlobalUnlock( hGlobal );
		if( ::SetClipboardData( 
#if (defined _UNICODE)
					CF_UNICODETEXT
#else
					CF_TEXT
#endif
					, 
				hGlobal 
				) != NULL 
			)
			bRetVal = true;
	}
	::CloseClipboard();
	return bRetVal;
}

void CExtMsgBox::ResetAllMsgBoxes()
{
CWinApp * pApp = ::AfxGetApp();
	ASSERT_VALID( pApp );
CString strKey = _T("Software\\");
	strKey += pApp->m_pszRegistryKey;
	strKey += _T("\\");
	strKey += pApp->m_pszProfileName;
	strKey += _T("\\");
	strKey += g_strRegSectionName;
	pApp->DelRegTree( HKEY_CURRENT_USER, strKey );
}

void CExtMsgBox::ResetMsgBox()
{
	ASSERT_VALID( this );
CExtSafeString strRegVarName = OnMsgBoxGetRegVarName();
	::AfxGetApp()->WriteProfileInt( g_strRegSectionName, LPCTSTR(strRegVarName), -1 );
}

int CExtMsgBox::DoMsgBox(
	HWND hWndParent,
	UINT nMessageResourceID,
	UINT nCaptionResourceID, // = 0
	UINT nMsgBoxStyle, // = __EXT_MB_DEFAULT_STYLES
	UINT nHelpID, // = 0
	__EXT_MFC_SAFE_LPCTSTR strUniqueID, // = NULL // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
	UINT nMsgBoxStyleEx, // = __EXT_MB_EX_DEFAULT_STYLES
	UINT nTimeoutSeconds, // = 0
	bool bIsDisabledTimeout, // = false
	CExtBitmap * pBmpIcon // = NULL
	)
{
__PROF_UIS_MANAGE_STATE;
CExtSafeString strCaption, strMessageText;
	if( nCaptionResourceID == 0 )
		strCaption = LPCTSTR( ::AfxGetAppName() );
	else
	{
		VERIFY( g_ResourceManager->LoadString( strCaption, nCaptionResourceID ) );
	}
	VERIFY( g_ResourceManager->LoadString( strMessageText, nMessageResourceID ) );
	return DoMsgBox( hWndParent, strMessageText, strCaption, nMsgBoxStyle, nHelpID, strUniqueID, nMsgBoxStyleEx, nTimeoutSeconds, bIsDisabledTimeout, pBmpIcon );
}

int CExtMsgBox::DoMsgBox(
	HWND hWndParent,
	__EXT_MFC_SAFE_LPCTSTR strMessageText,
	__EXT_MFC_SAFE_LPCTSTR strCaption, // = NULL
	UINT nMsgBoxStyle, // = __EXT_MB_DEFAULT_STYLES
	UINT nHelpID, // = 0
	__EXT_MFC_SAFE_LPCTSTR strUniqueID, // = NULL // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
	UINT nMsgBoxStyleEx, // = __EXT_MB_EX_DEFAULT_STYLES
	UINT nTimeoutSeconds, // = 0
	bool bIsDisabledTimeout, // = false
	CExtBitmap * pBmpIcon // = NULL
	)
{
__PROF_UIS_MANAGE_STATE;
	if( ::GetCapture() != NULL )
		::ReleaseCapture(); // force release mouse capture
CExtMsgBox dlg( hWndParent, strMessageText, strCaption, nMsgBoxStyle, nHelpID, strUniqueID, nMsgBoxStyleEx );
	if( nTimeoutSeconds > 0 )
		dlg.TimeoutSet( nTimeoutSeconds, bIsDisabledTimeout );
	if( pBmpIcon != NULL && ( ! pBmpIcon->IsEmpty() ) )
		dlg.m_bmpIcon = (*pBmpIcon);
//HWND hWndFocusRestore = ::GetFocus();
int nRetVal = int( dlg.DoModal() );
//	if(		hWndFocusRestore != NULL
//		&&	::IsWindow( hWndFocusRestore )
//		&&	::IsWindowEnabled( hWndFocusRestore )
//		&&	( ( ::__EXT_MFC_GetWindowLong( hWndFocusRestore, GWL_STYLE ) ) & WS_VISIBLE ) != 0
//		&&	::GetFocus() != hWndFocusRestore
//		)
//	{
//		CExtPopupMenuWnd::PassMsgLoop( false );
//		if(		::IsWindow( hWndFocusRestore )
//			&&	::IsWindowEnabled( hWndFocusRestore )
//			&&	( ( ::__EXT_MFC_GetWindowLong( hWndFocusRestore, GWL_STYLE ) ) & WS_VISIBLE ) != 0
//			&&	::GetFocus() != hWndFocusRestore
//			)
//			::SetFocus( hWndFocusRestore );
//	}
	return nRetVal;
}

int ProfUISMsgBox(
	HWND hWndParent,
	UINT nMessageResourceID,
	UINT nCaptionResourceID, // = 0
	UINT nMsgBoxStyle, // = __EXT_MB_DEFAULT_STYLES
	UINT nHelpID, // = 0
	__EXT_MFC_SAFE_LPCTSTR strUniqueID, // = NULL // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
	UINT nMsgBoxStyleEx, // = __EXT_MB_EX_DEFAULT_STYLES
	UINT nTimeoutSeconds, // = 0
	bool bIsDisabledTimeout, // = false
	CExtBitmap * pBmpIcon // = NULL
	)
{
__PROF_UIS_MANAGE_STATE;
	return CExtMsgBox::DoMsgBox( hWndParent, nMessageResourceID, nCaptionResourceID, nMsgBoxStyle, nHelpID, strUniqueID, nMsgBoxStyleEx, nTimeoutSeconds, bIsDisabledTimeout, pBmpIcon );
}

int ProfUISMsgBox(
	HWND hWndParent,
	__EXT_MFC_SAFE_LPCTSTR strMessageText,
	__EXT_MFC_SAFE_LPCTSTR strCaption, // = NULL
	UINT nMsgBoxStyle, // = __EXT_MB_DEFAULT_STYLES
	UINT nHelpID, // = 0
	__EXT_MFC_SAFE_LPCTSTR strUniqueID, // = NULL // for __EXT_MB_DO_NOT_DISPLAY_AGAIN and __EXT_MB_DO_NOT_ASK_AGAIN
	UINT nMsgBoxStyleEx, // = __EXT_MB_EX_DEFAULT_STYLES
	UINT nTimeoutSeconds, // = 0
	bool bIsDisabledTimeout, // = false
	CExtBitmap * pBmpIcon // = NULL
	)
{
__PROF_UIS_MANAGE_STATE;
	return CExtMsgBox::DoMsgBox( hWndParent, strMessageText, strCaption, nMsgBoxStyle, nHelpID, strUniqueID, nMsgBoxStyleEx, nTimeoutSeconds, bIsDisabledTimeout, pBmpIcon );
}

#endif // (! defined __EXT_MFC_NO_MSG_BOX )







