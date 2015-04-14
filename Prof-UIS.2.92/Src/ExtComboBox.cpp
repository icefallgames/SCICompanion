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

#if (!defined __EXT_COMBO_BOX_H)
	#include <ExtComboBox.h>
#endif

#if (!defined __EXT_RICH_CONTENT_H)
	#include <ExtRichContent.h>
#endif // (!defined __EXT_RICH_CONTENT_H)

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __EXT_TOOLCONTROLBAR_H)
	#include <ExtToolControlBar.h>
#endif

#if (!defined __EXT_SCROLLWND_H)
	#include <ExtScrollWnd.h>
#endif 

#if (!defined __AFXPRIV_H__)
	#include <AfxPriv.h>
#endif

#if (! defined __VSSYM32_H__)
	#include <vssym32/vssym32.h>
#endif // (! defined __VSSYM32_H__)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CExtComboBoxPopupListBox
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CExtComboBoxPopupListBox, CWnd );

BEGIN_MESSAGE_MAP( CExtComboBoxPopupListBox, CWnd )
	//{{AFX_MSG_MAP(CExtComboBoxPopupListBox)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CExtComboBoxPopupListBox::CExtComboBoxPopupListBox()
	: m_pCB( NULL )
	, m_hWndCB( NULL )
	, m_bFirstActivationInvocation( true )
	, m_bSimpleModeComboBox( false )
	, m_rcWndRestore( 0, 0, 0, 0 )
	, m_bInvalidateOnMouseMove( false )
{
}

CExtComboBoxPopupListBox::~CExtComboBoxPopupListBox()
{
}

bool CExtComboBoxPopupListBox::_IsEnabledItem( CPoint point )
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL || m_hWndCB == NULL || ( ! ::IsWindow( m_hWndCB ) ) )
		return false;
CRect rcClient;	
	GetClientRect( rcClient );
	if( rcClient.PtInRect( point ) )
	{
		BOOL bOutside = FALSE;
		INT nIndex = ((CListBox *)this)->ItemFromPoint( point, bOutside );
		if(		(!bOutside) 
			&&	m_pCB->LbItemIsEnabled( nIndex )
			)
			return true;
		return false;
	}
	return true;
}

void CExtComboBoxPopupListBox::OnLButtonUp( UINT nFlags, CPoint point ) 
{
	ASSERT_VALID( this );
	if( ! _IsEnabledItem( point ) )
		return;
	CWnd::OnLButtonUp(nFlags, point);
}

void CExtComboBoxPopupListBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( m_bInvalidateOnMouseMove )
		Invalidate( TRUE );
	if( ! _IsEnabledItem( point ) )
		return;
	CWnd::OnMouseMove(nFlags, point);
}

void CExtComboBoxPopupListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( ! _IsEnabledItem( point ) )
		return;
	CWnd::OnLButtonDown(nFlags, point);
}

void CExtComboBoxPopupListBox::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	ASSERT_VALID( this );
	if( ! _IsEnabledItem( point ) )
		return;
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CExtComboBoxPopupListBox::PreSubclassWindow()
{
	ASSERT_VALID( this );
	CWnd::PreSubclassWindow();
	ASSERT_VALID( m_pCB );
	ASSERT( m_pCB->GetSafeHwnd() != NULL && m_pCB->m_hWnd == m_hWndCB );
DWORD dwStyle = m_pCB->GetStyle();
	if(		(dwStyle&CBS_SIMPLE) == CBS_SIMPLE 
		&&	(dwStyle&CBS_DROPDOWN) != CBS_DROPDOWN
		&&	(dwStyle&CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST 
		)
		m_bSimpleModeComboBox = true;
	if( m_bSimpleModeComboBox )
		ModifyStyle( 0, WS_CLIPCHILDREN|WS_CLIPSIBLINGS );
}

void CExtComboBoxPopupListBox::PostNcDestroy()
{
	ASSERT_VALID( this );
	delete this;
}

LRESULT CExtComboBoxPopupListBox::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( m_pCB != NULL && m_hWndCB != NULL && ::IsWindow( m_hWndCB ) )
	{
		LRESULT lResult = 0L;
		if( m_pCB->_OnCbLbPreWndProc( lResult, message, wParam, lParam ) )
			return lResult;
	}
	switch( message )
	{
	case WM_NCCALCSIZE:
		if( m_bSimpleModeComboBox)
		{
			NCCALCSIZE_PARAMS * myNCSIZE = (NCCALCSIZE_PARAMS*)lParam;
			CRect rcSaved = myNCSIZE->rgrc[0];
			LRESULT lResult = CWnd::WindowProc( message, wParam, lParam );
			//myNCSIZE->rgrc[0].left = rcSaved.left;
			myNCSIZE->rgrc[0].top = rcSaved.top;
			myNCSIZE->rgrc[0].bottom = rcSaved.bottom;
			myNCSIZE->rgrc[0].right ++;
			return lResult;
		}
	break;
	case (WM_USER+555):
		MoveWindow( &m_rcWndRestore );
	return 0L;
	case WM_PRINT:
	case WM_PRINTCLIENT:
		if( m_bFirstActivationInvocation )
		{
			m_bFirstActivationInvocation = false;
			if( ! m_bSimpleModeComboBox )
			{
				GetWindowRect( &m_rcWndRestore );
				MoveWindow( -32000, -32000, 0, 0 );
				PostMessage( (WM_USER+555) );
			}
		}
		if( ! m_bSimpleModeComboBox )
			return 0L;
	break;
	case WM_ERASEBKGND:
	if( 	(GetStyle() & LBS_OWNERDRAWFIXED) != 0 
		||	(GetStyle() & LBS_OWNERDRAWVARIABLE) != 0 
		)
		return TRUE;
	break;
	case WM_PAINT:
	if( 	(GetStyle() & LBS_OWNERDRAWFIXED) != 0 
		||	(GetStyle() & LBS_OWNERDRAWVARIABLE) != 0 
		)
	{
		CPaintDC dcPaint( this );
		CRect rcClient;
		GetClientRect( &rcClient );
		CExtMemoryDC dc( &dcPaint, &rcClient );
		if( m_pCB != NULL && m_hWndCB != NULL && ::IsWindow( m_hWndCB ) )
		{
			CExtPaintManager * pPM = m_pCB->PmBridge_GetPM();
			ASSERT_VALID( pPM );
			dc.FillSolidRect( &rcClient, pPM->GetColor( COLOR_WINDOW, this ) );
			HFONT hFont = m_pCB->OnQueryFont();
			if( hFont == NULL )
				hFont = (HFONT)pPM->m_FontNormal.GetSafeHandle();
			ASSERT( hFont != NULL );
			HFONT hOldFont = (HFONT)::SelectObject( dc.m_hDC, hFont );
			INT nCount = m_pCB->GetCount();
			INT nTopIndex = ((CListBox*)this)->GetTopIndex();
			for( INT nItem = nTopIndex; nItem < nCount; nItem++ )
			{
				DRAWITEMSTRUCT dis;
				::memset( &dis, 0, sizeof( DRAWITEMSTRUCT ) );
				dis.CtlType = ODT_LISTBOX;
				dis.CtlID = GetDlgCtrlID();
				dis.itemID = nItem;
				dis.hDC = dc.GetSafeHdc();
				((CListBox*)this)->GetItemRect( nItem, &dis.rcItem );
				dis.itemAction = ODA_DRAWENTIRE;
				dis.hwndItem = GetSafeHwnd();
				dis.itemData = m_pCB->GetItemData( nItem );
				if( rcClient.bottom < dis.rcItem.top )
					break;
				if( ((CListBox*)this)->GetSel( nItem ) > 0 )
					dis.itemState |= ODS_SELECTED;
				if( ((CListBox*)this)->GetCurSel() == nItem )
					dis.itemState |= ODS_FOCUS;
				m_pCB->SendMessage( WM_DRAWITEM, (WPARAM)GetDlgCtrlID(), (LPARAM)&dis );
			}
			::SelectObject( dc.m_hDC, hOldFont );
		}
		return TRUE;
	}
	break;
	} // switch( message )
HWND hWndCB = m_hWndCB, hWndOwn = m_hWnd;
CWnd * pThis = this;
LRESULT lResult = CWnd::WindowProc( message, wParam, lParam );
	if( ! ( hWndOwn != NULL && ::IsWindow( hWndOwn ) && CWnd::FromHandlePermanent(hWndOwn) == pThis ) )
		return lResult;
	if( hWndCB != NULL && ::IsWindow( hWndCB ) && m_pCB != NULL && LPVOID(CWnd::FromHandlePermanent(hWndCB)) == LPVOID(m_pCB) )
		m_pCB->_OnCbLbPostWndProc( lResult, message, wParam, lParam );
LONG nFrame = RDW_FRAME;
	switch( message )
	{
	case WM_VSCROLL:
	case WM_HSCROLL:
		nFrame = 0L;
	case WM_SHOWWINDOW:
	case WM_WINDOWPOSCHANGED:
		RedrawWindow(
			NULL,
			NULL,
			RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|nFrame
			);
	break;
	} // switch( message )
	return lResult;
}

void CExtComboBoxPopupListBox::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{
	ASSERT_VALID( this );
	if( m_pCB != NULL && m_hWndCB != NULL && ::IsWindow( m_hWndCB ) && m_pCB->GetDroppedState() )
	{
		DWORD dwComboBoxType = m_pCB->GetStyle() & 0x0003L;
		CListBox * pListBox = ( CListBox * ) this;
		bool bSelChanged = false;
		switch( nChar )
		{
			case VK_LEFT:
			case VK_UP:
			{
				INT nCount = pListBox->GetCount();
				if( nCount == 0 )
					break;
				// Go up until we find a valid selection then set the current selection to that one
				INT nCurSel = pListBox->GetCurSel();
				INT nSel = nCurSel;
				bool bInitSelection = false;
				if(	nSel < 0 && nCount > 0 )
				{
					nSel = nCount - 1;
					bInitSelection = true;
				}
				if( nSel != LB_ERR )
				{
					while( nSel > 0 )
					{
						INT nItem = nSel - ( bInitSelection ? 0 : 1 );
						if( m_pCB->LbItemIsEnabled( nItem ) )
						{
							if( nCurSel != nItem )
							{
								pListBox->SetRedraw( FALSE );
								INT nRealSel = pListBox->SetCurSel( nItem );
								if( nCurSel != nRealSel )
								{
									if( dwComboBoxType == CBS_DROPDOWNLIST )
									{
										INT nTopIndex = pListBox->GetTopIndex();
										m_pCB->SetCurSel( nRealSel );
										pListBox->SetTopIndex( nTopIndex );
									}
									else
									{
										CString sText;
										pListBox->GetText( nRealSel, sText );
										m_pCB->SetWindowText( sText );
										m_pCB->SetEditSel( 0, -1 );
									}
									bSelChanged = true;
								}
								pListBox->SetRedraw( TRUE );
								pListBox->RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_FRAME );
							}
							break;
						}
						nSel--;
					}
				}
			}
			break;
			case VK_RIGHT:
			case VK_DOWN:
			{
				INT nCount = pListBox->GetCount();
				if( nCount == 0 )
					break;
				// Go down until we find a valid selection then set the current selection to that one
				INT nCurSel = pListBox->GetCurSel();
				INT nSel = nCurSel;
				bool bInitSelection = false;
				if(	nSel < 0 && nCount > 0 )
				{
					nSel = 0;
					bInitSelection = true;
				}
				if( nSel != LB_ERR )
				{
					while( nSel < nCount - 1 )
					{
						INT nItem = nSel + ( bInitSelection ? 0 : 1 );
						if( nItem == nCount )
							break;
						if( m_pCB->LbItemIsEnabled( nItem ) )
						{
							if( nCurSel != nItem )
							{
								pListBox->SetRedraw( FALSE );
								INT nRealSel = pListBox->SetCurSel( nItem );
								if( nCurSel != nRealSel )
								{
									if( dwComboBoxType == CBS_DROPDOWNLIST )
									{
										INT nTopIndex = pListBox->GetTopIndex();
										m_pCB->SetCurSel( nRealSel );
										pListBox->SetTopIndex( nTopIndex );
									}
									else
									{
										CString sText;
										pListBox->GetText( nRealSel, sText );
										m_pCB->SetWindowText( sText );
										m_pCB->SetEditSel( 0, -1 );
									}
									bSelChanged = true;
								}
								pListBox->SetRedraw( TRUE );
								pListBox->RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_FRAME );
							}
							break;
						}
						nSel++;
					}
				}
			}
			break;
			case VK_PRIOR:
			{
				INT nCount = pListBox->GetCount();
				if( nCount == 0 )
					break;
 				INT nTopIndex = pListBox->GetTopIndex();
				INT nCurSel = pListBox->GetCurSel();
				if( nCurSel == 0 && nTopIndex == 0 )
					break;
				SCROLLINFO _scroll_info;
				::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
				_scroll_info.cbSize = sizeof(SCROLLINFO);
				if(	! pListBox->GetScrollInfo( SB_VERT, &_scroll_info ) )
					break;
				INT nNewSel = _scroll_info.nPos - _scroll_info.nPage;
				if( nNewSel < 0 )
					nNewSel = 0;
				if( nNewSel != nCurSel || nTopIndex != nNewSel  )
				{
					pListBox->SetRedraw( FALSE );
					if( nTopIndex != nNewSel )
						pListBox->SetTopIndex( nNewSel );
					if( nNewSel != nCurSel )
					{
						m_pCB->SetCurSel( nNewSel );
						bSelChanged = true;
					}
					if( dwComboBoxType != CBS_DROPDOWNLIST )
					{
						CString sText;
						pListBox->GetText( nNewSel, sText );
						m_pCB->SetWindowText( sText );
						m_pCB->SetEditSel( 0, -1 );
					}
					pListBox->SetRedraw( TRUE );
					pListBox->RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_FRAME );
				}
			}
			break;
			case VK_NEXT:
			{
				INT nCount = pListBox->GetCount();
				if( nCount == 0 )
					break;
 				INT nTopIndex = pListBox->GetTopIndex();
				INT nCurSel = pListBox->GetCurSel();
				SCROLLINFO _scroll_info;
				::memset( &_scroll_info, 0, sizeof(SCROLLINFO) );
				_scroll_info.cbSize = sizeof(SCROLLINFO);
				if(	! pListBox->GetScrollInfo( SB_VERT, &_scroll_info ) )
					break;
				INT nNewTopIndex = _scroll_info.nPos + _scroll_info.nPage;
				INT nNewSel = nNewTopIndex + _scroll_info.nPage - 1;
				if( nNewSel >= nCount )
					nNewSel = nCount - 1;
				if( nNewSel != nCurSel || nNewTopIndex != nTopIndex )
				{
					pListBox->SetRedraw( FALSE );
					if( nNewSel != nCurSel )
					{
						m_pCB->SetCurSel( nNewSel );
						bSelChanged = true;
					}
					if( nNewTopIndex != nTopIndex )
						m_pCB->SetTopIndex( nNewTopIndex );
					if( dwComboBoxType != CBS_DROPDOWNLIST )
					{
						CString sText;
						pListBox->GetText( nNewSel, sText );
						m_pCB->SetWindowText( sText );
						m_pCB->SetEditSel( 0, -1 );
					}
					pListBox->SetRedraw( TRUE );
					pListBox->RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_FRAME );
				}
			}
			break;
			case VK_HOME:
			{
				INT nCount = pListBox->GetCount();
				if( nCount == 0 )
					break;
				// Select first non-disabled item
				INT nCurSel = pListBox->GetCurSel();
				INT nSel = 0;
				while( nSel < nCount - 1 )
				{
					INT nItem = nSel;
					if( m_pCB->LbItemIsEnabled( nItem ) )
					{
						if( nCurSel != nItem )
						{
							pListBox->SetRedraw( FALSE );
							INT nRealSel = pListBox->SetCurSel( nItem );
							if( nCurSel != nRealSel )
							{
								if( dwComboBoxType == CBS_DROPDOWNLIST )
								{
									INT nTopIndex = pListBox->GetTopIndex();
									m_pCB->SetCurSel( nRealSel );
									pListBox->SetTopIndex( nTopIndex );
								}
								else
								{
									CString sText;
									pListBox->GetText( nRealSel, sText );
									m_pCB->SetWindowText( sText );
									m_pCB->SetEditSel( 0, -1 );
								}
								bSelChanged = true;
							}
							pListBox->SetRedraw( TRUE );
							pListBox->RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_FRAME );
						}
						break;
					}
					nSel++;
				}
			}
			break;
			case VK_END:
			{
				INT nCount = pListBox->GetCount();
				if( nCount == 0 )
					break;
				// Select last non-disabled item
				INT nCurSel = pListBox->GetCurSel();
				INT nSel = nCount;
				while( nSel > 0 )
				{
					INT nItem = nSel - 1;
					if( m_pCB->LbItemIsEnabled( nItem ) )
					{
						if( nCurSel != nItem )
						{
							pListBox->SetRedraw( FALSE );
							INT nRealSel = pListBox->SetCurSel( nItem );
							if( nCurSel != nRealSel )
							{
								if( dwComboBoxType == CBS_DROPDOWNLIST )
								{
									INT nTopIndex = pListBox->GetTopIndex();
									m_pCB->SetCurSel( nRealSel );
									pListBox->SetTopIndex( nTopIndex );
								}
								else
								{
									CString sText;
									pListBox->GetText( nRealSel, sText );
									m_pCB->SetWindowText( sText );
									m_pCB->SetEditSel( 0, -1 );
								}
								bSelChanged = true;
							}
							pListBox->SetRedraw( TRUE );
							pListBox->RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_FRAME );
						}
						break;
					}
					nSel--;
				}
			}
			break;
		default:
			CWnd::OnKeyDown( nChar, nRepCnt, nFlags );
		return;
		} // switch( nChar )
		if( bSelChanged )
		{
			// The CBN_SELCHANGE notification message is not sent when the selection 
			// is changed using the CB_SETCURSEL message. 
			if( ! m_pCB->m_bEnableSelChangeWorkaround  )
			{
				CWnd * pWnd = m_pCB->GetParent();
				if( pWnd->GetSafeHwnd() != NULL )
				{
					ASSERT_VALID( pWnd );
					pWnd->SendMessage(
						WM_COMMAND,
						MAKEWPARAM( m_pCB->GetDlgCtrlID(), CBN_SELCHANGE ),
						(LPARAM) m_hWnd
						);
				}
			}
		}
		return;
	} // if( ... && m_pCB->GetDroppedState() )
	CWnd::OnKeyDown( nChar, nRepCnt, nFlags );
}

/////////////////////////////////////////////////////////////////////////////
// CExtComboBoxFilterPopupListBox::LB_ITEM
//////////////////////////////////////////////////////////////////////////

CExtComboBoxFilterPopupListBox::LB_ITEM::LB_ITEM()
{
	m_lParam = 0L;
}

CExtComboBoxFilterPopupListBox::LB_ITEM::~LB_ITEM()
{
}

LPARAM CExtComboBoxFilterPopupListBox::LB_ITEM::LParamGet() const
{
	return m_lParam;
}

void CExtComboBoxFilterPopupListBox::LB_ITEM::LParamSet( 
	LPARAM lParam 
	)
{
	m_lParam = lParam;
}
	
//////////////////////////////////////////////////////////////////////////
// CExtComboBoxFilterPopupListBox
//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC( CExtComboBoxFilterPopupListBox, CListBox );

BEGIN_MESSAGE_MAP(CExtComboBoxFilterPopupListBox, CListBox)
	//{{AFX_MSG_MAP(CExtComboBoxFilterPopupListBox)
	ON_WM_MOUSEACTIVATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_CAPTURECHANGED()
	ON_WM_CANCELMODE()
	ON_WM_NCLBUTTONUP()
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_MESSAGE( LB_ADDSTRING, OnLBAddString )
	ON_MESSAGE( LB_INSERTSTRING, OnLBInsertString )
	ON_MESSAGE( LB_DELETESTRING, OnLBDeleteString )
	ON_MESSAGE( LB_RESETCONTENT, OnLBResetContent )
	ON_MESSAGE( LB_ADDFILE, OnLBAddFile )
	//}}AFX_MSG_MAP
	ON_WM_ACTIVATEAPP()
	__EXT_MFC_ON_WM_NCHITTEST()
END_MESSAGE_MAP()

CExtComboBoxFilterPopupListBox::CExtComboBoxFilterPopupListBox(
	CExtComboBoxBase * pCB // = NULL
	)
	: m_pCB( pCB )
	, m_hWndCB( NULL )
	, m_hCursorSizeWE( NULL )
	, m_hCursorSizeNS( NULL )
	, m_hCursorSizeNWSE( NULL )
	, m_bResizingX( false )
	, m_bResizingY( false )
	, m_ptCursorPosLast( 0, 0 )
{
	m_hCursorSizeWE = ::LoadCursor( NULL, IDC_SIZEWE );
	ASSERT( m_hCursorSizeWE != NULL );
	m_hCursorSizeNS = ::LoadCursor( NULL, IDC_SIZENS );
	ASSERT( m_hCursorSizeNS != NULL );
	m_hCursorSizeNWSE = ::LoadCursor( NULL, IDC_SIZENWSE );
	ASSERT( m_hCursorSizeNWSE != NULL );
}

CExtComboBoxFilterPopupListBox::~CExtComboBoxFilterPopupListBox()
{
	LbItemRemoveAll();

	if( m_hCursorSizeWE != NULL )
	{
		::DestroyCursor( m_hCursorSizeWE );
		m_hCursorSizeWE = NULL;
	}
	if( m_hCursorSizeNS != NULL )
	{
		::DestroyCursor( m_hCursorSizeNS );
		m_hCursorSizeNS = NULL;
	}
	if( m_hCursorSizeNWSE != NULL )
	{
		::DestroyCursor( m_hCursorSizeNWSE );
		m_hCursorSizeNWSE = NULL;
	}
}
INT CExtComboBoxFilterPopupListBox::LbItemInsert( 
	INT nIndex // = -1
	)
{
INT nCount = INT( m_arrLBItems.GetSize() );
	if( nIndex < 0 || nIndex > nCount )
		nIndex = nCount; // append
	
LB_ITEM * pItem = new LB_ITEM();
	ASSERT( pItem != NULL );
	m_arrLBItems.InsertAt( nIndex, pItem );

	return nIndex;
}

bool CExtComboBoxFilterPopupListBox::LbItemRemove(
	LONG nIndex
	)
{
INT nCount = INT( m_arrLBItems.GetSize() );
	if( nIndex < 0 || nIndex >= nCount )
		return false;
LB_ITEM * pItem = m_arrLBItems.GetAt( nIndex );
	ASSERT( pItem != NULL );
	if( pItem != NULL )
	{
		delete pItem;
		pItem = NULL;
	}
	m_arrLBItems.RemoveAt( nIndex );
	return true;
}

void CExtComboBoxFilterPopupListBox::LbItemRemoveAll()
{
	while( m_arrLBItems.GetSize() > 0 )
	{
		LB_ITEM * pItem = m_arrLBItems[0];
		if( pItem != NULL ) 
		{
			delete pItem;
			pItem = NULL;
		}
		m_arrLBItems.RemoveAt(0);
	}
}

CExtComboBoxFilterPopupListBox::LB_ITEM * CExtComboBoxFilterPopupListBox::LbItemGet( 
	LONG nIndex 
	) const
{
INT nCount = INT( m_arrLBItems.GetSize() );
	if( nIndex < 0 || nIndex >= nCount )
		return NULL;

LB_ITEM * pItem = m_arrLBItems.GetAt( nIndex );
	ASSERT( pItem != NULL );
	return pItem;
}

INT CExtComboBoxFilterPopupListBox::LbItemGetIndexOf( 
	const LB_ITEM * pItem 
	) const
{
INT nCount = INT( m_arrLBItems.GetSize() );
	for( INT nIndex = 0; nIndex < nCount; nIndex++ )
	{
		LB_ITEM * pItem2 = m_arrLBItems.GetAt( nIndex );
		ASSERT( pItem2 != NULL );
		if( pItem2 == pItem )
			return nIndex;
	}
	return -1;
}

INT CExtComboBoxFilterPopupListBox::LbItemGetCount() const
{
INT nCount = INT( m_arrLBItems.GetSize() );
	return nCount;
}

LRESULT CExtComboBoxFilterPopupListBox::OnLBAddString( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
LRESULT lResult = DefWindowProc( LB_ADDSTRING, wParam, lParam );
	if( lResult != LB_ERR )
		LbItemInsert( (INT)lResult );
	return lResult;
}

LRESULT CExtComboBoxFilterPopupListBox::OnLBInsertString( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
LRESULT lResult = DefWindowProc( LB_INSERTSTRING, wParam, lParam );
	if( lResult != LB_ERR )	
		LbItemInsert( (INT)lResult );
	return lResult;
}

LRESULT CExtComboBoxFilterPopupListBox::OnLBDeleteString( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
LRESULT lResult = DefWindowProc( LB_DELETESTRING, wParam, lParam );
	if( lResult != LB_ERR )
		LbItemRemove( (LONG)wParam );
	return lResult;
}

LRESULT CExtComboBoxFilterPopupListBox::OnLBResetContent( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
LRESULT lResult = DefWindowProc( LB_RESETCONTENT, wParam, lParam );
	LbItemRemoveAll();
	return lResult;
}

LRESULT CExtComboBoxFilterPopupListBox::OnLBAddFile( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
LRESULT lResult = DefWindowProc( LB_ADDSTRING, wParam, lParam );
	if( lResult != LB_ERR )		
		LbItemInsert( (INT)lResult );
	return lResult;
}

void CExtComboBoxFilterPopupListBox::DrawItem( LPDRAWITEMSTRUCT lpDIS ) 
{
	ASSERT_VALID( this );
	ASSERT( lpDIS != NULL && lpDIS->hDC != NULL );
	if( ! ( m_pCB != NULL && m_hWndCB != NULL && ::IsWindow( m_hWndCB ) ) )
		return;
	if(		(GetStyle() & CBS_HASSTRINGS) == 0 
		&&	(GetStyle() & LBS_HASSTRINGS) == 0 
		)
		return;
INT nCount = GetCount();
INT nItem = lpDIS->itemID;
	if( nItem < 0 || nItem >= nCount )
		return;
LB_ITEM * pFLbItem = LbItemGet( nItem );
	ASSERT( pFLbItem != NULL );
	if( pFLbItem != NULL )
	{
		CExtComboBoxBase::LB_ITEM * pLbItem = (CExtComboBoxBase::LB_ITEM *) pFLbItem->LParamGet();
		ASSERT( pLbItem != NULL );
		if( pLbItem != NULL )
		{
			CDC dc;
			dc.Attach( lpDIS->hDC );
			if( lpDIS->itemAction & (ODA_DRAWENTIRE | ODA_SELECT) )
			{
				CRect rcErase( lpDIS->rcItem );
				if( nItem == ( nCount - 1 ) )
				{
					CRect rcClient;
					GetClientRect( &rcClient );
					if( rcErase.bottom < rcClient.bottom )
						rcErase.bottom = rcClient.bottom;
				}
				dc.FillSolidRect(
					rcErase, 
					m_pCB->PmBridge_GetPM()->GetColor( COLOR_WINDOW, this )
					);
			}
			m_pCB->OnPopupListBoxDrawItem( 
				dc,
				lpDIS->rcItem,
				m_pCB->OnPopupListBoxCalcItemExtraSizes( nItem ),
				lpDIS->itemState,
				pLbItem 
				);
			dc.Detach();
		} // if( pLbItem != NULL )
	} // if( pFLbItem != NULL )
}

void CExtComboBoxFilterPopupListBox::MeasureItem( LPMEASUREITEMSTRUCT lpMIS ) 
{
	if( m_pCB != NULL && m_hWndCB != NULL && ::IsWindow( m_hWndCB ) )
		m_pCB->OnPopupListBoxMeasureItem( lpMIS );
}

LRESULT CExtComboBoxFilterPopupListBox::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_ERASEBKGND:
	if( 	(GetStyle() & LBS_OWNERDRAWFIXED) != 0 
		||	(GetStyle() & LBS_OWNERDRAWVARIABLE) != 0 
		)
		return TRUE;
	break;
	case WM_PAINT:
	if( 	(GetStyle() & LBS_OWNERDRAWFIXED) != 0 
		||	(GetStyle() & LBS_OWNERDRAWVARIABLE) != 0 
		)
	{
		CPaintDC dcPaint( this );
		CRect rcClient;
		GetClientRect( &rcClient );
		CExtMemoryDC dc( &dcPaint, &rcClient );
		if( m_pCB != NULL && m_hWndCB != NULL && ::IsWindow( m_hWndCB ) )
		{
			CExtPaintManager * pPM = m_pCB->PmBridge_GetPM();
			ASSERT_VALID( pPM );
			dc.FillSolidRect( &rcClient, pPM->GetColor( COLOR_WINDOW, this ) );
			HFONT hFont = m_pCB->OnQueryFont();
			if( hFont == NULL )
				hFont = (HFONT)pPM->m_FontNormal.GetSafeHandle();
			ASSERT( hFont != NULL );
			HFONT hOldFont = (HFONT)::SelectObject( dc.m_hDC, hFont );
			INT nCount = GetCount();
			for( INT nItem = GetTopIndex(); nItem < nCount; nItem++ )
			{
				DRAWITEMSTRUCT dis;
				::memset( &dis, 0, sizeof( DRAWITEMSTRUCT ) );
				dis.CtlType = ODT_LISTBOX;
				dis.CtlID = GetDlgCtrlID();
				dis.itemID = nItem;
				dis.hDC = dc.GetSafeHdc();
				GetItemRect( nItem, &dis.rcItem );
				dis.itemAction = ODA_DRAWENTIRE;
				dis.hwndItem = GetSafeHwnd();
				if( rcClient.bottom < dis.rcItem.top )
					break;
				if( GetSel( nItem ) > 0 )
					dis.itemState |= ODS_SELECTED;
				if( GetCurSel() == nItem )
					dis.itemState |= ODS_FOCUS;
				SendMessage( WM_DRAWITEM, (WPARAM)GetDlgCtrlID(), (LPARAM)&dis );
			}
			::SelectObject( dc.m_hDC, hOldFont );
		}
		return TRUE;
	}
	break;
	}
	return CListBox::WindowProc( message, wParam, lParam );
}

void CExtComboBoxFilterPopupListBox::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{
	ASSERT_VALID( this );
	switch( nChar )
	{
		case VK_LEFT:
		case VK_UP:
			{
				// Go up until we find a valid selection
				// then set the current selection to that one
				INT nSel = GetCurSel();
				bool bInitSelection = false;
				INT nCount = GetCount();
				if( nSel < 0 && nCount > 0 )
				{
					nSel = nCount - 1;
					bInitSelection = true;
				}
				if( nSel != LB_ERR )
				{
					while( nSel >= 0 )
					{
						INT nItem = nSel - ( bInitSelection ? 0 : 1 );
						if( nItem < 0 && nCount > 0 )
							nItem = 0;

						LB_ITEM * pFLbItem = LbItemGet( nItem );
						ASSERT( pFLbItem != NULL );

						CExtComboBoxBase::LB_ITEM * pLbItem = NULL;
						if( pFLbItem != NULL )
							pLbItem = (CExtComboBoxBase::LB_ITEM *) pFLbItem->LParamGet();
						ASSERT( pLbItem != NULL );

						if( pLbItem->IsEnabled() )
						{
							SetCurSel( nItem );
							break;
						}
						nSel--;
					}
				}
			}
			return;

		case VK_RIGHT:
		case VK_DOWN:
			{
				// Go down until we find a valid selection
				// then set the current selection to that one
				INT nSel = GetCurSel();
				bool bInitSelection = false;
				INT nCount = GetCount();
				if( nSel < 0 && nCount > 0 )
				{
					nSel = 0;
					bInitSelection = true;
				}
				if( nSel != LB_ERR )
				{
					while( nSel <= nCount - 1 )
					{
						INT nItem = nSel + ( bInitSelection ? 0 : 1 );
						if( nItem >= nCount )
							nItem = nCount - 1;

						LB_ITEM * pFLbItem = LbItemGet( nItem );
						ASSERT( pFLbItem != NULL );

						CExtComboBoxBase::LB_ITEM * pLbItem = NULL;
						if( pFLbItem != NULL )
							pLbItem = (CExtComboBoxBase::LB_ITEM *) pFLbItem->LParamGet();
						ASSERT( pLbItem != NULL );

						if( pLbItem->IsEnabled() )
						{
							SetCurSel( nItem );
							break;
						}
						nSel++;
					}
				}
			}
			return;

		case VK_HOME:
		case VK_PRIOR:
			{
				// Select first non-disabled item
				INT nCount = GetCount();
				INT nSel = 0;
				while( nSel < nCount - 1 )
				{
					INT nItem = nSel;

					LB_ITEM * pFLbItem = LbItemGet( nItem );
					ASSERT( pFLbItem != NULL );

					CExtComboBoxBase::LB_ITEM * pLbItem = NULL;
					if( pFLbItem != NULL )
						pLbItem = (CExtComboBoxBase::LB_ITEM *) pFLbItem->LParamGet();
					ASSERT( pLbItem != NULL );

					if( pLbItem->IsEnabled() )
					{
						SetCurSel( nItem );
						break;
					}

					nSel++;
				}
			}
			return;

		case VK_END:
		case VK_NEXT:
			{
				// Select last non-disabled item
				INT nCount = GetCount();
				INT nSel = nCount;
				while( nSel > 0 )
				{
					INT nItem = nSel - 1;

					LB_ITEM * pFLbItem = LbItemGet( nItem );
					ASSERT( pFLbItem != NULL );

					CExtComboBoxBase::LB_ITEM * pLbItem = NULL;
					if( pFLbItem != NULL )
						pLbItem = (CExtComboBoxBase::LB_ITEM *) pFLbItem->LParamGet();
					ASSERT( pLbItem != NULL );

					if( pLbItem->IsEnabled() )
					{
						SetCurSel( nItem );
						break;
					}

					nSel--;
				}
			}
			return;

		default:
			break;
	}

	CListBox::OnKeyDown( nChar, nRepCnt, nFlags );
}

LONG CExtComboBoxFilterPopupListBox::HitTest( 
	const POINT & ptClient
	) const
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return HTNOWHERE;

CPoint ptWnd( ptClient );
	ClientToScreen( &ptWnd );
CRect rcWnd;
	GetWindowRect( &rcWnd );

CRect rcGrip( rcWnd );
	rcGrip.top = rcGrip.bottom - ::GetSystemMetrics( SM_CYVSCROLL );
	rcGrip.left = rcGrip.right - ::GetSystemMetrics( SM_CXVSCROLL );
	if(	rcGrip.PtInRect( ptWnd ) )
		return HTBOTTOMRIGHT;
	
CRect rcBottom( rcWnd );
	rcBottom.top = rcBottom.bottom - ::GetSystemMetrics( SM_CYEDGE );
	if( rcBottom.PtInRect( ptWnd ) )
		return HTBOTTOM;

CRect rcRight( rcWnd );
	rcRight.left = rcRight.right - ::GetSystemMetrics( SM_CXEDGE );
	if( rcRight.PtInRect( ptWnd ) )
		return HTRIGHT;

	if( rcWnd.PtInRect( ptWnd ) )
		return HTCLIENT;

	return HTNOWHERE;
}

#if _MFC_VER < 0x700
void CExtComboBoxFilterPopupListBox::OnActivateApp(BOOL bActive, HTASK hTask) 
#else
void CExtComboBoxFilterPopupListBox::OnActivateApp(BOOL bActive, DWORD hTask) 
#endif
{
	CListBox::OnActivateApp(bActive, hTask);
	if( ! bActive )
		SendMessage( WM_CANCELMODE );
}

void CExtComboBoxFilterPopupListBox::OnCancelMode() 
{
	CListBox::OnCancelMode();
	
	if( ::GetCapture() == GetSafeHwnd() )
		::ReleaseCapture();

	m_bResizingX = false;
	m_bResizingY = false;
	m_ptCursorPosLast = CPoint( 0, 0 );

	Invalidate();
	UpdateWindow();
}

void CExtComboBoxFilterPopupListBox::OnCaptureChanged(CWnd *pWnd) 
{
	CListBox::OnCaptureChanged(pWnd);
	if( ::GetCapture() != m_hWnd )
		SendMessage( WM_CANCELMODE );
}

void CExtComboBoxFilterPopupListBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( !_ProcessMouseClick( point, true, MK_LBUTTON ) )
		CListBox::OnLButtonDown(nFlags, point);
}

void CExtComboBoxFilterPopupListBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( !_ProcessMouseClick( point, false, MK_LBUTTON ) )
		CListBox::OnLButtonUp(nFlags, point);
}

void CExtComboBoxFilterPopupListBox::OnNcLButtonDblClk(UINT nHitTest, CPoint point) 
{
	if( !_ProcessMouseClick( point, true, MK_LBUTTON ) )
		CListBox::OnNcLButtonDblClk(nHitTest, point);
}

void CExtComboBoxFilterPopupListBox::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if( !_ProcessMouseClick( point, true, MK_LBUTTON ) )
		CListBox::OnLButtonDblClk(nFlags, point);
}

void CExtComboBoxFilterPopupListBox::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	if( !_ProcessMouseClick( point, true, MK_LBUTTON ) )
		CListBox::OnNcLButtonDown(nHitTest, point);
}

void CExtComboBoxFilterPopupListBox::OnNcLButtonUp(UINT nHitTest, CPoint point) 
{
	if( !_ProcessMouseClick( point, false, MK_LBUTTON ) )
		CListBox::OnNcLButtonUp(nHitTest, point);
}

void CExtComboBoxFilterPopupListBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( !_ProcessMouseMove( point ) )
		CListBox::OnMouseMove(nFlags, point);
}

bool CExtComboBoxFilterPopupListBox::_ProcessMouseClick(
	CPoint point,
	bool bButtonPressed,
	INT nMouseButton // MK_... values
	)
{
	ASSERT_VALID( this );

	// process only left mouse button clicks
	if( nMouseButton != MK_LBUTTON )
		return false;

	// select item
	if(		(! bButtonPressed)
		&&  (! m_bResizingX)
		&&  (! m_bResizingY)
		)
	{
		INT nCount = GetCount();
		if( nCount > 0 )
		{
			BOOL bOutside = TRUE;
			INT nHitTest = (INT) ItemFromPoint( point, bOutside );
			if( !bOutside )
			{				
				LB_ITEM * pFLbItem = LbItemGet( nHitTest );
				ASSERT( pFLbItem != NULL );

				CExtComboBoxBase::LB_ITEM * pLbItem = NULL;
				if( pFLbItem != NULL )
					pLbItem = (CExtComboBoxBase::LB_ITEM *) pFLbItem->LParamGet();
				ASSERT( pLbItem != NULL );

				if( pLbItem->IsEnabled() )
				{
					SetCurSel( nHitTest );
					if( m_pCB != NULL && m_hWndCB != NULL && ::IsWindow( m_hWndCB ) )
						m_pCB->OnFilterPopupListSelEndOK();
				}
			}
		}
		return true;
	}

	LONG nHT = HitTest( point );
	
	m_bResizingX = false;
	m_bResizingY = false;
	m_ptCursorPosLast = CPoint( 0, 0 );
	
	if( bButtonPressed )
	{
		if( nHT == HTBOTTOMRIGHT )
			m_bResizingX = m_bResizingY = true;
		if( nHT == HTRIGHT )
			m_bResizingX = true;
		if( nHT == HTBOTTOM )
			m_bResizingY = true;
		if( m_bResizingX || m_bResizingY )
		{
			m_ptCursorPosLast = point;
			if( ::GetCapture() != GetSafeHwnd() )
				::SetCapture( GetSafeHwnd() );
			return true;
		}
	}
	else
	{
		if( ::GetCapture() == GetSafeHwnd() )
			::ReleaseCapture();
	}
	
	if( nHT == HTCLIENT )
		return true;

	return false;
}

bool CExtComboBoxFilterPopupListBox::_ProcessMouseMove( CPoint point )
{
	ASSERT_VALID( this );
	if(		CExtPopupMenuWnd::IsMenuTracking() 
		||	(! CExtPopupMenuWnd::TestHoverEnabledFromActiveHWND( GetSafeHwnd() ) )
		)
		return false;

	LONG nHT = HitTest( point );
	if(		nHT == HTCLIENT 
		&&	!(m_bResizingX || m_bResizingY)
		&&	m_ptCursorPosLast != point
		)
	{
		INT nCount = GetCount();
		if( nCount == 0 )
			return true;

		BOOL bOutside = TRUE;
		INT nHitTest = (INT) ItemFromPoint( point, bOutside );
		if( bOutside )
		{
			INT nTop = GetTopIndex();
			if( nHitTest <= nTop )
			{
				if( nTop > 0 )
					nHitTest--;
			}
			else
			{
				if( nCount > 0 && nHitTest >= (nCount-2) )
					nHitTest++;
			}
		} // if( bOutside )

		m_ptCursorPosLast = point;

		INT nCurSel = GetCurSel();
		if( nCurSel == nHitTest )
			return true;

		SetRedraw( FALSE );
		SetCurSel( nHitTest );
		SetRedraw( TRUE );
		RedrawWindow(
			NULL,
			NULL,
			RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_FRAME
			);

		return true;
	}
	
	// resize window
	if(		( m_bResizingX || m_bResizingY )
		&&	m_ptCursorPosLast != point
		&&	m_pCB != NULL && m_hWndCB != NULL && ::IsWindow( m_hWndCB )
		)
	{
		CRect rcWindow;
		GetWindowRect( &rcWindow );
		CRect rcWindowNew = rcWindow;

		CRect rcClient;
		GetClientRect( &rcClient );

		CSize szMin = m_pCB->OnFilterPopupListQueryMinSize();
		CSize szMax = m_pCB->OnFilterPopupListQueryMaxSize();

		if(		m_bResizingX 
			&&	point.x > szMin.cx 
			&&	( point.x < szMax.cx || szMax.cx < 0 )
			)
		{
			INT nOffsetX = point.x - m_ptCursorPosLast.x;
			rcWindowNew.right += nOffsetX;
			m_ptCursorPosLast.x += nOffsetX;
		}
		if(		m_bResizingY 
			&&	point.y > szMin.cy 
			&&	( point.y < szMax.cy || szMax.cy < 0 )
			)
		{
			INT nOffsetY = point.y - m_ptCursorPosLast.y;
			rcWindowNew.bottom += nOffsetY;
			m_ptCursorPosLast.y += nOffsetY;
		}

		if( rcWindowNew != rcWindow )
		{
			SetWindowPos(
				NULL,
				0,0,
				rcWindowNew.Width(), rcWindowNew.Height(),
				SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE 
					| SWP_NOACTIVATE | SWP_NOSENDCHANGING
				);
			Invalidate( FALSE );
			UpdateWindow();
		}
	}
		
	return true;
}

BOOL CExtComboBoxFilterPopupListBox::OnSetCursor( CWnd * pWnd, UINT nHitTest, UINT message ) 
{
	ASSERT_VALID( this );

	CPoint ptClient;
	::GetCursorPos( &ptClient );
	ScreenToClient( &ptClient );
	LONG nHT = HitTest( ptClient );

	if(		nHT == HTBOTTOMRIGHT 
		&&	m_hCursorSizeNWSE != NULL
		)
	{
		SetCursor( m_hCursorSizeNWSE );
		return TRUE;
	}
	if(		nHT == HTRIGHT 
		&&	m_hCursorSizeWE != NULL
		)
	{
		SetCursor( m_hCursorSizeWE );
		return TRUE;
	}
	if(		nHT == HTBOTTOM
		&&	m_hCursorSizeNS != NULL
		)
	{
		SetCursor( m_hCursorSizeNS );
		return TRUE;
	}
	return CListBox::OnSetCursor(pWnd, nHitTest, message);
}

int CExtComboBoxFilterPopupListBox::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	if( (GetStyle()&WS_TABSTOP) == 0 )
		return MA_NOACTIVATE;
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

UINT CExtComboBoxFilterPopupListBox::OnNcHitTest( CPoint point ) 
{
	CPoint ptClient = point;
	ScreenToClient( &ptClient );
	LONG nHT = HitTest( ptClient );
	if( nHT != HTCLIENT )
		return HTCLIENT;
	return UINT( CListBox::OnNcHitTest(point) );
}

void CExtComboBoxFilterPopupListBox::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	CRect & rcNcRect =
		reinterpret_cast < CRect & > ( lpncsp->rgrc[0] );
	rcNcRect.bottom -= 14;

	CListBox::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void CExtComboBoxFilterPopupListBox::OnNcPaint() 
{
	ASSERT_VALID( this );
	
	Default();
	
CRect rcInBarWnd, rcInBarClient;
    GetWindowRect( &rcInBarWnd );
    GetClientRect( &rcInBarClient );
    ClientToScreen( &rcInBarClient );
    if( rcInBarWnd == rcInBarClient )
        return;

CPoint ptDevOffset = -rcInBarWnd.TopLeft();
    rcInBarWnd.OffsetRect( ptDevOffset );
    rcInBarClient.OffsetRect( ptDevOffset );
   
CWindowDC dc( this );

const INT cx = ::GetSystemMetrics(SM_CXVSCROLL);
const INT cy = ::GetSystemMetrics(SM_CYHSCROLL);

DWORD dwStyle = GetStyle();
bool bHasVerticalSB = ( (dwStyle&WS_VSCROLL) != 0 ) ? true : false;
bool bHasHorizontalSB = ( (dwStyle&WS_HSCROLL) != 0 ) ? true : false;

COLORREF clrWindow =
		( m_pCB != NULL && m_hWndCB != NULL && ::IsWindow( m_hWndCB ) )
			? m_pCB->PmBridge_GetPM()->GetColor( COLOR_WINDOW, this )
			: ::GetSysColor( COLOR_WINDOW )
			;
		
    if( bHasVerticalSB && bHasHorizontalSB )
    {
		INT nNcX = rcInBarWnd.right - rcInBarClient.right - cx;
		INT nNcY = rcInBarWnd.bottom - rcInBarClient.bottom - cy;
        dc.FillSolidRect(
            rcInBarWnd.right - cx - nNcX,
            rcInBarWnd.bottom - cy - nNcY,
            cx,
            cy,
			clrWindow
            );
    }

CRect rcExclude( rcInBarClient );
    if( bHasVerticalSB )
        rcExclude.right += cx;
    if( bHasHorizontalSB )
        rcExclude.bottom += cy;

	dc.ExcludeClipRect( &rcExclude );

	rcInBarWnd.DeflateRect( 1, 1 );

    dc.FillSolidRect( &rcInBarWnd, clrWindow );	

CRect rcGrip( rcInBarWnd );
	g_PaintManager->PaintResizingGripper(
		dc,
		rcGrip,
		this
		);
	
	dc.SelectClipRgn( NULL );
}

/////////////////////////////////////////////////////////////////////////////
// CExtComboEditCtrlHook
/////////////////////////////////////////////////////////////////////////////

CExtComboEditCtrlHook::CExtComboEditCtrlHook()
{
}

CExtComboEditCtrlHook::~CExtComboEditCtrlHook()
{
}

CExtComboBoxBase * CExtComboEditCtrlHook::OnQueryComboBox() const
{
	ASSERT( GetSafeHwnd() != NULL );
HWND hWndParent = ::GetParent( GetSafeHwnd() );
	ASSERT( hWndParent != NULL );
	ASSERT( ::IsWindow(hWndParent) );
CExtComboBoxBase * pCombo = 
		DYNAMIC_DOWNCAST(
			CExtComboBoxBase,
			FromHandlePermanent(hWndParent)
			);
	ASSERT( pCombo != NULL );
	return pCombo;
}

IMPLEMENT_DYNCREATE( CExtComboEditCtrlHook, CExtEditBase );

BEGIN_MESSAGE_MAP(CExtComboEditCtrlHook, CExtEditBase)
	//{{AFX_MSG_MAP(CExtComboEditCtrlHook)
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

LRESULT CExtComboEditCtrlHook::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
bool bFlushAutoComplete = false;

	switch( message )
	{
	case WM_CUT:
	case WM_COPY:
	case WM_PASTE:
	case WM_CLEAR:
	case WM_UNDO:
		bFlushAutoComplete = true;
		break;
	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case ID_EDIT_CLEAR:
		case ID_EDIT_CLEAR_ALL:
		case ID_EDIT_COPY:
		case ID_EDIT_CUT:
		case ID_EDIT_FIND:
		case ID_EDIT_PASTE:
		case ID_EDIT_PASTE_LINK:
		case ID_EDIT_PASTE_SPECIAL:
		case ID_EDIT_REPEAT:
		case ID_EDIT_REPLACE:
		case ID_EDIT_SELECT_ALL:
		case ID_EDIT_UNDO:
		case ID_EDIT_REDO:
			bFlushAutoComplete = true;
		break;
		} // switch( LOWORD(wParam) )
		break;
	} // switch( message )
	
	if( bFlushAutoComplete )
		OnQueryComboBox()->m_bAutoComplete = false;

	return CExtEditBase::WindowProc(message,wParam,lParam);
}

__EXT_MFC_INT_PTR CExtComboEditCtrlHook::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	) const
{
	__PROF_UIS_MANAGE_STATE;
	__EXT_MFC_IMPLEMENT_TT_REDIR_OnToolHitTest_EX( CExtComboEditCtrlHook, CExtToolControlBar, m_wndToolTip, point, pTI, 2 )
	return CExtEditBase::OnToolHitTest( point, pTI );
}

void CExtComboEditCtrlHook::OnContextMenu(CWnd* pWnd,CPoint pos )
{
	if( ! m_bHandleCtxMenus )
	{
		CExtEditBase::OnContextMenu( pWnd, pos );
		return;
	}
CExtComboBoxBase * pCombo = OnQueryComboBox();
	if( pCombo == NULL )
		return;
	m_bHandleCtxMenus = pCombo->m_bHandleCtxMenus;
	SetFocus();
	CExtEditBase::OnContextMenu( pWnd, pos );
	if( ! ::IsWindow( m_hWnd ) )
		return;
	VERIFY( pCombo->RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_FRAME|RDW_ALLCHILDREN ) );
}

void CExtComboEditCtrlHook::OnDestroy() 
{
//
//	Afreen had problems with this code
//
// 	CExtComboBoxBase * pCB = OnQueryComboBox();
// 	ASSERT( pCB != NULL );
// 	if( pCB != NULL )
// 	{
// 		ASSERT_VALID( pCB );
// 		pCB->OnFilterPopupListClose();
// 	}
	CExtEditBase::OnDestroy();
}

COLORREF CExtComboEditCtrlHook::OnQueryBackColor() const
{ 
	ASSERT_VALID( this );
	CExtComboBoxBase * pCB = OnQueryComboBox();
	return pCB->GetBkColor(); 
}

COLORREF CExtComboEditCtrlHook::OnQueryTextColor() const
{ 
	ASSERT_VALID( this );
	CExtComboBoxBase * pCB = OnQueryComboBox();
	return pCB->GetTextColor(); 
}

/////////////////////////////////////////////////////////////////////////////
// CExtComboBoxBase::LB_ITEM::CELL

CExtComboBoxBase::LB_ITEM::CELL::CELL(
	__EXT_MFC_SAFE_LPCTSTR sItemText // = NULL
	)
{
	m_sItemText = (sItemText != NULL) ? sItemText : _T("");
	m_nLParam = 0L;
	m_clrText = COLORREF( -1L );
	m_clrBack = COLORREF( -1L );
}

CExtComboBoxBase::LB_ITEM::CELL::~CELL()
{
}

/////////////////////////////////////////////////////////////////////////////
// CExtComboBoxBase::LB_ITEM

CExtComboBoxBase::LB_ITEM::LB_ITEM()
{
	m_bEnabled = true;
}

CExtComboBoxBase::LB_ITEM::~LB_ITEM()
{
	while( m_arrItemCells.GetSize() > 0 )
	{
		CELL * pItemCell = m_arrItemCells[0];
		if( pItemCell != NULL ) 
		{
			delete pItemCell;
			pItemCell = NULL;
		}
		m_arrItemCells.RemoveAt(0);
	}
}

INT CExtComboBoxBase::LB_ITEM::LbItemCellInsert(
	__EXT_MFC_SAFE_LPCTSTR sItemText, // = NULL
	INT nPos // = -1 // append
	)
{
INT nCount = INT( m_arrItemCells.GetSize() );
	if( nPos < 0 || nPos > nCount )
		nPos = nCount; // append
	
CELL * pItemCell = new CELL( sItemText );
	ASSERT( pItemCell != NULL );
	m_arrItemCells.InsertAt( nPos, pItemCell );

	return nPos;
}

bool CExtComboBoxBase::LB_ITEM::LbItemCellRemove(
	LONG nItem
	)
{
INT nCount = INT( m_arrItemCells.GetSize() );
	if( nItem < 0 || nItem >= nCount )
		return false;
CELL * pItemCell = m_arrItemCells.GetAt( nItem );
	ASSERT( pItemCell != NULL );
	if( pItemCell != NULL )
	{
		delete pItemCell;
		pItemCell = NULL;
	}
	m_arrItemCells.RemoveAt( nItem );
	return true;
}

CExtComboBoxBase::LB_ITEM::CELL * CExtComboBoxBase::LB_ITEM::LbItemCellGet( 
	LONG nItem 
	) const
{
INT nCount = INT( m_arrItemCells.GetSize() );
	if( nItem < 0 || nItem >= nCount )
		return NULL;

CELL * pItemCell = m_arrItemCells.GetAt( nItem );
	ASSERT( pItemCell != NULL );
	return pItemCell;
}

INT CExtComboBoxBase::LB_ITEM::LbItemCellGetIndexOf( 
	const CELL * pItemCell 
	) const
{
INT nCount = INT( m_arrItemCells.GetSize() );
	for( INT nItem = 0; nItem < nCount; nItem++ )
	{
		CELL * pItemCell2 = m_arrItemCells.GetAt( nItem );
		ASSERT( pItemCell2 != NULL );
		if( pItemCell2 == pItemCell )
			return nItem;
	}
	return -1;
}

INT CExtComboBoxBase::LB_ITEM::LbItemCellGetCount() const
{
INT nCount = INT( m_arrItemCells.GetSize() );
	return nCount;
}

bool CExtComboBoxBase::LB_ITEM::IsEnabled() const
{
	return m_bEnabled;
}

void CExtComboBoxBase::LB_ITEM::Enable(
	bool bEnable // = true
	)
{
	m_bEnabled = bEnable;
}

/////////////////////////////////////////////////////////////////////////////
// CExtComboBoxBase

IMPLEMENT_DYNCREATE( CExtComboBoxBase, CComboBox );

CExtComboBoxBase::CExtComboBoxBase()
	: m_bAutoComplete( true )
	, m_bHandleCtxMenus( true )
	, m_bEnableAutoComplete( true )
	, m_bUseCaseSensitiveAutoComplete( false )
	, m_clrBack( COLORREF(-1L) )
	, m_clrBackPrev( COLORREF(-1L) )
	, m_clrText( COLORREF(-1L) )
	, m_pPopupListWnd( NULL )
	, m_szFilterPopupListBox( -1, -1 )
	, m_bEnableAutoFilter( false )
	, m_bEnableSelChangeWorkaround( false )
	, m_bInitialized( false )
	, m_pWndPopupListBox( NULL )
	, m_hWndPopupListBox( NULL )
	, m_bToolTipActive( false )
	, m_nAdvancedTipStyle( INT(CExtPopupMenuTipWnd::__ETS_RECTANGLE_NO_ICON) )
{
	EnableToolTips();
}

CExtComboBoxBase::~CExtComboBoxBase()
{
CExtAnimationSite * pAcAS = AnimationClient_SiteGet();
	if( pAcAS != NULL )
		pAcAS->AnimationSite_ClientRemove( this );
	LbItemRemoveAll();
	if( m_pWndPopupListBox != NULL )
	{
		if(		m_hWndPopupListBox != NULL
			&&	::IsWindow( m_hWndPopupListBox )
			&&	LPVOID( CWnd::FromHandlePermanent(m_hWndPopupListBox) ) == LPVOID( m_pWndPopupListBox )
			)
			m_pWndPopupListBox->m_pCB = NULL;
	}
	if( m_pPopupListWnd != NULL )
	{
		delete m_pPopupListWnd;
		m_pPopupListWnd = NULL;
	}
}

bool CExtComboBoxBase::_OnCbLbPreWndProc( LRESULT & lResult, UINT message, WPARAM wParam, LPARAM lParam )
{
	lResult; message; wParam; lParam;
	return false;
}

void CExtComboBoxBase::_OnCbLbPostWndProc( LRESULT lResult, UINT message, WPARAM wParam, LPARAM lParam )
{
	lResult; message; wParam; lParam;
}

CExtComboBoxPopupListBox * CExtComboBoxBase::OnComboBoxPopupListBoxInstantiate()
{
	ASSERT_VALID( this );
bool bSimpleMode = false;
DWORD dwStyle = GetStyle();
	if(		(dwStyle&CBS_SIMPLE) == CBS_SIMPLE 
		&&	(dwStyle&CBS_DROPDOWN) != CBS_DROPDOWN
		&&	(dwStyle&CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST 
		)
		bSimpleMode = true;
CExtNCSB < CExtComboBoxPopupListBox > * pWndPopupListBox = new CExtNCSB < CExtComboBoxPopupListBox >;
	if( bSimpleMode )
		pWndPopupListBox->m_rcNcsbCustomNcAreaSizes.SetRect( 0, 0, 0, 0 );
	return pWndPopupListBox;
}

CExtComboBoxPopupListBox * CExtComboBoxBase::OnComboBoxPopupListBoxQuery(
	HWND hWndSubclass // = NULL
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return NULL;
	if( m_pWndPopupListBox != NULL )
	{
		ASSERT_VALID( m_pWndPopupListBox );
		if( hWndSubclass != NULL )
		{
			if( m_pWndPopupListBox->GetSafeHwnd() != hWndSubclass )
				return NULL;
			m_hWndPopupListBox = hWndSubclass;
		}
		return m_pWndPopupListBox;
	}
	if(		hWndSubclass == NULL
		||	(! ::IsWindow( hWndSubclass ) )
		)
		return NULL;
	try
	{
		m_pWndPopupListBox = OnComboBoxPopupListBoxInstantiate();
		m_pWndPopupListBox->m_pCB = this;
		m_pWndPopupListBox->m_hWndCB = m_hWnd;
		VERIFY( m_pWndPopupListBox->SubclassWindow( hWndSubclass ) );
		m_hWndPopupListBox = hWndSubclass;
// 		m_pWndPopupListBox->RedrawWindow( NULL, NULL, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_FRAME );
		m_pWndPopupListBox->SetWindowPos(
			NULL, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE
				|SWP_NOZORDER|SWP_NOOWNERZORDER
				|SWP_NOREPOSITION
				|SWP_FRAMECHANGED
			);
		return m_pWndPopupListBox;
	}
	catch( CException * pException )
	{
		pException->Delete();
	}
	catch( ... )
	{
	}
	return NULL;
}

BEGIN_MESSAGE_MAP(CExtComboBoxBase, CComboBox)
	//{{AFX_MSG_MAP(CExtComboBoxBase)
	ON_MESSAGE(CB_ADDSTRING, OnCBAddString)
	ON_MESSAGE(CB_INSERTSTRING, OnCBInsertString)
	ON_MESSAGE(CB_DELETESTRING, OnCBDeleteString)
	ON_MESSAGE(CB_RESETCONTENT, OnCBResetContent)
	ON_MESSAGE(CB_SETCURSEL, OnCBSetCurSel)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_CHARTOITEM()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_CTLCOLORLISTBOX, OnCtlColor )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtComboBoxBase message handlers

CSize CExtComboBoxBase::OnFilterPopupListQueryMinSize()
{
	return CSize( 40, 20 );
}

CSize CExtComboBoxBase::OnFilterPopupListQueryMaxSize()
{
	return CSize( -1, -1 );
}

CSize CExtComboBoxBase::GetFilterPopupListSize() const
{
	return m_szFilterPopupListBox;
}

CSize CExtComboBoxBase::SetFilterPopupListSize( CSize szSize )
{
	CSize szFilterPopupListBoxOld = m_szFilterPopupListBox;
	m_szFilterPopupListBox = szSize;
	return szFilterPopupListBoxOld;
}

bool CExtComboBoxBase::OnHookSpyPreTranslateMessage(
	MSG * pMSG
	)
{
__PROF_UIS_MANAGE_STATE;
	ASSERT( pMSG != NULL );
	if(		m_hWnd != NULL
		&&	::IsWindow( m_hWnd )
		&&	( GetStyle() & WS_VISIBLE ) != 0
		&&	(	pMSG->hwnd == m_hWnd 
			|| ::IsChild( m_hWnd, pMSG->hwnd )
			|| ::GetParent( m_hWnd ) == pMSG->hwnd
			)
		)
	{
		if(		pMSG->message == WM_ACTIVATEAPP	
			||	pMSG->message == WM_CANCELMODE
			||	pMSG->message == WM_SETFOCUS
			||	pMSG->message == WM_KILLFOCUS
			||	pMSG->message == WM_SYSCOMMAND
			||	pMSG->message == WM_STYLECHANGING
			||	pMSG->message == WM_STYLECHANGED
			||	pMSG->message == WM_CLOSE
			||	pMSG->message == WM_DESTROY
			||	pMSG->message == WM_NCDESTROY
			||	pMSG->message == WM_ENABLE
			||	pMSG->message == WM_SIZE
			||	pMSG->message == WM_NEXTDLGCTL
			||	pMSG->message == WM_LBUTTONDOWN
			||	pMSG->message == WM_RBUTTONDOWN
			||	pMSG->message == WM_MBUTTONDOWN
			||	CExtPopupMenuWnd::IsMenuTracking()
			||	CExtControlBar::_DraggingGetBar() != NULL
			||	(	(	pMSG->message == WM_WINDOWPOSCHANGING
					||	pMSG->message == WM_WINDOWPOSCHANGED
					)
					&& ( ((LPWINDOWPOS)pMSG->lParam)->flags&(SWP_NOMOVE|SWP_NOSIZE) ) == 0
				)
			||	(	( pMSG->message == WM_ACTIVATE || pMSG->message == WM_MOUSEACTIVATE )
				&&	(! ::IsChild( m_hWnd, pMSG->hwnd ) ) 
				)
			)
		{
			OnFilterPopupListClose();
			return true;
		}
	}
	return CExtHookSpy::OnHookSpyPreTranslateMessage( pMSG );
}

HWND CExtComboBoxBase::AnimationSite_GetSafeHWND() const
{
__PROF_UIS_MANAGE_STATE;
HWND hWnd = GetSafeHwnd();
	return hWnd;
}

void CExtComboBoxBase::ShowFilterPopupList( 
	CExtSafeString & sFilterText 
	)
{
	ASSERT_VALID( this );

	if(		m_pPopupListWnd->GetSafeHwnd() == NULL
		||	(!::IsWindow( m_pPopupListWnd->m_hWnd ))
		)
		m_pPopupListWnd = OnFilterPopupListCreate();
	if( m_pPopupListWnd == NULL )
	{
		ASSERT( FALSE );
		return;
	}
	ASSERT_VALID( m_pPopupListWnd );

	OnFilterPopupListSyncContents( sFilterText );

	if(		sFilterText.IsEmpty()
		||	m_pPopupListWnd->GetCount() == 0 
		)
	{
		OnFilterPopupListClose();
	}
	else
	{
		if( ! m_pPopupListWnd->IsWindowVisible() )
		{
			CRect rcWnd;
			GetWindowRect( &rcWnd );
			
			CSize szMax = OnFilterPopupListQueryMaxSize();
			szMax.cx = szMax.cx > 0 ? szMax.cx : 150;
			szMax.cy = szMax.cy > 0 ? szMax.cy : 100;
			if( m_szFilterPopupListBox.cx < 0 )
				m_szFilterPopupListBox.cx = max( rcWnd.Width(), szMax.cx );
			if( m_szFilterPopupListBox.cy < 0 )
				m_szFilterPopupListBox.cy = max( ::MulDiv( rcWnd.Width(), 2, 3 ), szMax.cy );
			
			m_pPopupListWnd->SetWindowPos(
				NULL,
				rcWnd.left,
				rcWnd.bottom,
				m_szFilterPopupListBox.cx,
				m_szFilterPopupListBox.cy,
				SWP_SHOWWINDOW | SWP_NOZORDER | SWP_NOACTIVATE
				);
		}
	}
}

CExtComboBoxFilterPopupListBox * CExtComboBoxBase::OnFilterPopupListCreate()
{
	ASSERT_VALID( this );

	if(		m_pPopupListWnd->GetSafeHwnd() != NULL
		&&	::IsWindow( m_pPopupListWnd->m_hWnd )
		)
		return m_pPopupListWnd;

	if( m_pPopupListWnd != NULL )
	{
		delete m_pPopupListWnd;
		m_pPopupListWnd = NULL;
	}

CExtNCSB < CExtComboBoxFilterPopupListBox > * pPopupListWnd = new CExtNCSB < CExtComboBoxFilterPopupListBox > ( true );
	pPopupListWnd->m_pCB = this;
	pPopupListWnd->m_hWndCB = m_hWnd;
	if( pPopupListWnd == NULL )
	{
		ASSERT( FALSE );
		return NULL;
	}
	ASSERT_VALID( pPopupListWnd );

	if( !pPopupListWnd->CreateEx(
			WS_EX_TOPMOST | WS_EX_WINDOWEDGE,
			_T("LISTBOX"),
			_T(""),
			WS_POPUP | WS_BORDER | WS_VSCROLL | LBS_NOINTEGRALHEIGHT | LBS_HASSTRINGS | LBS_OWNERDRAWVARIABLE,
			CRect(0,0,0,0),
			this,
			UINT(0)
			)
		)
	{
		ASSERT( FALSE );
		return NULL;
	}

	pPopupListWnd->SetFont( 
		CFont::FromHandle( (HFONT)::GetStockObject(DEFAULT_GUI_FONT) ) 
		);

	return pPopupListWnd;
}

void CExtComboBoxBase::OnFilterPopupListClose()
{
	ASSERT_VALID( this );
	if(		m_pPopupListWnd->GetSafeHwnd() != NULL 
		&&	::IsWindow( m_pPopupListWnd->m_hWnd )
		&&	m_pPopupListWnd->IsWindowVisible()
		)
	{
		CRect rcWnd;
		m_pPopupListWnd->GetWindowRect( &rcWnd );
		m_szFilterPopupListBox = rcWnd.Size();
		m_pPopupListWnd->SetWindowPos(
			NULL,
			0,0,0,0,
			SWP_HIDEWINDOW | SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE
			);
	}
}

void CExtComboBoxBase::OnFilterPopupListSyncContents(
	CExtSafeString & sFilterText
	)
{
	ASSERT_VALID( this );
	if(		m_pPopupListWnd->GetSafeHwnd() == NULL 
		||	(!::IsWindow( m_pPopupListWnd->m_hWnd ))
		)
		return;

	m_pPopupListWnd->SetRedraw( FALSE );

	m_pPopupListWnd->ResetContent();
	if( !sFilterText.IsEmpty() )
	{
		CStringArray arrTextToRemove; 
		arrTextToRemove.Add( _T("http://") );
		arrTextToRemove.Add( _T("http://www.") );
		arrTextToRemove.Add( _T("ftp://") );
		arrTextToRemove.Add( _T("ftp://www.") );
		arrTextToRemove.Add( _T("file://") );
		arrTextToRemove.Add( _T("www.") );

		INT nLenght = sFilterText.GetLength();
		CString sItemText;

		INT nCount = GetCount();
		for( LONG nItem = 0; nItem < nCount; nItem++ )
		{
			sItemText.Empty();
			GetLBText( nItem, sItemText );

			CString sItemTextToCompare = sItemText;
			
			for( INT nIndex = 0; nIndex < arrTextToRemove.GetSize(); nIndex++ ) 
			{
				CString sTextToRemove = arrTextToRemove[ nIndex ];
				INT nLenght = sTextToRemove.GetLength();
				CString sItemTextPart = sItemTextToCompare.Left( nLenght );
				if( sItemTextPart.CompareNoCase( sTextToRemove ) == 0 )
					sItemTextToCompare.Delete( 0, nLenght );
			}

			if(	sFilterText.CompareNoCase( sItemTextToCompare.Left( nLenght ) ) == 0 )
			{
				INT nItemAdded = m_pPopupListWnd->AddString( sItemText );
				CExtComboBoxFilterPopupListBox::LB_ITEM * pFLbItem = 
					m_pPopupListWnd->LbItemGet( nItemAdded );
				ASSERT( pFLbItem != NULL );
				if( pFLbItem != NULL )
				{
					LB_ITEM * pLbItem = LbItemGet( nItem );
					pFLbItem->LParamSet( (LPARAM) pLbItem  );
				}
			}
		}
	} // if( !sFilterText.IsEmpty() )

	m_pPopupListWnd->SetRedraw( TRUE );
	m_pPopupListWnd->Invalidate();
	m_pPopupListWnd->UpdateWindow();
}

BOOL CExtComboBoxBase::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	ASSERT_VALID( this );
	if( message == WM_COMMAND )
	{
		INT nCode = HIWORD(wParam);
		switch( nCode )
		{
		case CBN_DROPDOWN:
			OnFilterPopupListClose();
			break;
        case CBN_SELENDOK:
			{
				DWORD dwWndStyle = GetStyle();
				if(		( dwWndStyle & (CBS_OWNERDRAWFIXED|CBS_OWNERDRAWVARIABLE) ) == 0
					||	( dwWndStyle & CBS_HASSTRINGS ) != 0
					)
				{
					GetWindowText( m_sSavedText );
					PostMessage( __EXT_COMBO_BOX_WM_USER_REALSELENDOK );
				}
			}
			break;
		case CBN_EDITUPDATE:
			OnEditCtrlUpdate();
		    break;
		}
	}
	return CComboBox::OnChildNotify(message, wParam, lParam, pLResult);
}

LONG CExtComboBoxBase::FindItemExact(
	__EXT_MFC_SAFE_LPCTSTR lpszString,
	LONG nIndexStart // = -1
	) const
{
	ASSERT_VALID( this );
LONG nCount = GetCount();
	if( nIndexStart < -1 || nIndexStart >= nCount )
	{
		ASSERT( FALSE );
		return CB_ERR;
	}
LONG nItemsProcessed = 0;
LONG nItem = nIndexStart;
	while( nItemsProcessed < nCount )
	{
		if( nItem == -1 )
			nItem = 0;
		else if( nItem < nCount - 1 )
			nItem++;
		else
			nItem = 0;

		CString sText;
		GetLBText( nItem, sText );
		if( sText.Compare( lpszString ) == 0 )
			return nItem;

		nItemsProcessed++;
	}
	return CB_ERR;
}

void CExtComboBoxBase::OnFilterPopupListSyncSelection( 
	bool bSetEditSel // = true
	)
{
	ASSERT_VALID( this );
	if(		m_pPopupListWnd->GetSafeHwnd() == NULL 
		||	(!::IsWindow( m_pPopupListWnd->m_hWnd ))
		)
		return;

INT nItem = m_pPopupListWnd->GetCurSel();
CExtComboBoxFilterPopupListBox::LB_ITEM * pFLbItem = 
		m_pPopupListWnd->LbItemGet( nItem );
	ASSERT( pFLbItem != NULL );

CExtComboBoxBase::LB_ITEM * pLbItemNew = NULL;
	if( pFLbItem != NULL )
		pLbItemNew = (CExtComboBoxBase::LB_ITEM *) pFLbItem->LParamGet();
	ASSERT( pLbItemNew != NULL );

INT nCount = GetCount();
	for( LONG nIndex = 0; nIndex < nCount; nIndex++ )
	{
		LB_ITEM * pLbItem = LbItemGet( nIndex );
		if( pLbItem == pLbItemNew )
		{
			CEdit * pEdit = GetInnerEditCtrl();
			if( pEdit != NULL )
			{
				ASSERT_VALID( pEdit );
				pEdit->SetRedraw( FALSE );
			}
			SetCurSel( nIndex );
			// make text non-selected
			if( ! bSetEditSel )
				SetEditSel( -1, -1 );
			if( pEdit != NULL )
			{
				ASSERT_VALID( pEdit );
				pEdit->SetRedraw( TRUE );
				pEdit->Invalidate();
				pEdit->UpdateWindow();
			}
			break;
		}
	}
}

void CExtComboBoxBase::OnFilterPopupListSelChanged()
{
	ASSERT_VALID( this );
	OnFilterPopupListSyncSelection( false );
}

void CExtComboBoxBase::OnFilterPopupListSelEndOK()
{
	ASSERT_VALID( this );
	if(		m_pPopupListWnd->GetSafeHwnd() == NULL 
		||	(!::IsWindow( m_pPopupListWnd->m_hWnd ))
		)
		return;
	
	OnFilterPopupListSyncSelection( true );

	OnFilterPopupListClose();

	// send the CBN_SELENDOK notification
	GetParent()->SendMessage( 
		WM_COMMAND,
		(WPARAM) MAKELONG( GetDlgCtrlID(), CBN_SELENDOK ),
		(LPARAM) GetSafeHwnd()
		);
}

void CExtComboBoxBase::_OnDrawComboImpl(
	bool bPressed,
	bool bHover,
	CDC * pDC // = NULL
	)
{
	bPressed; bHover; pDC;
}

BOOL CExtComboBoxBase::PreTranslateMessage(MSG* pMsg) 
{
	if( ! CExtPopupMenuWnd::IsMenuTracking() )
	{
		InitToolTip();
		m_wndToolTip.RelayEvent(pMsg);
	}

	if(		m_pPopupListWnd->GetSafeHwnd() != NULL 
		&&	::IsWindow( m_pPopupListWnd->m_hWnd )
		&&	m_pPopupListWnd->IsWindowVisible()
		)
	{
		if(		pMsg->message == WM_KEYDOWN 
			&&	( pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN )
			)
		{
			HWND hWndFocus = ::GetFocus();
			if(		hWndFocus != NULL
				&&	(	m_hWnd == hWndFocus
					||	::IsChild( m_hWnd, hWndFocus )
					)
				)
			{
				if( pMsg->wParam == VK_ESCAPE )
					OnFilterPopupListClose();
				else if( pMsg->wParam == VK_RETURN )
				{
					INT nItem = m_pPopupListWnd->GetCurSel();
					if(		nItem == LB_ERR 
						&&	m_pPopupListWnd->GetCount() > 0
						) 
						m_pPopupListWnd->SetCurSel( 0 );
					OnFilterPopupListSelEndOK();
				}
				return TRUE;
			}
		}
		if(	( pMsg->message == WM_KEYDOWN 
				&&	(	pMsg->wParam == VK_DOWN 
					||	pMsg->wParam == VK_UP 
					||	pMsg->wParam == VK_PRIOR 
					||	pMsg->wParam == VK_NEXT
					)
				)
			|| pMsg->message == WM_MOUSEWHEEL
			)
		{
			INT nCurSelOld = m_pPopupListWnd->GetCurSel();

			m_pPopupListWnd->SetRedraw( FALSE );
			m_pPopupListWnd->SendMessage( 
				pMsg->message,
				pMsg->wParam,
				pMsg->lParam
				);
			m_pPopupListWnd->SetRedraw( TRUE );
			m_pPopupListWnd->RedrawWindow(
				NULL,
				NULL,
				RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE|RDW_ERASENOW|RDW_FRAME
				);
			//_OnDrawComboImpl( false, false );

			INT nCurSelNew = m_pPopupListWnd->GetCurSel();
			if( nCurSelOld != nCurSelNew )
				OnFilterPopupListSelChanged();

			return TRUE;
		}
	}
	
	if( m_bEnableAutoComplete )
	{
		if( pMsg->message == WM_SYSCHAR )
			return FALSE;
		if( pMsg->message == WM_KEYDOWN )
		{
			if(		pMsg->wParam == VK_DELETE
				||	pMsg->wParam == VK_BACK
				)
				m_bAutoComplete = false;
			else
				m_bAutoComplete = true;
		}
	}
	else
	{
		m_bAutoComplete = false;
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

void CExtComboBoxBase::OnEditCtrlUpdate() 
{
	if( m_bEnableAutoFilter )
	{
		CEdit * pEdit = GetInnerEditCtrl();
		if( pEdit != NULL )
		{
			ASSERT_VALID( pEdit );
			CString sFilterText;
			pEdit->GetWindowText( sFilterText );
			INT nStartChar = 0;
			INT nEndChar = 0;
			pEdit->GetSel(
				nStartChar,
				nEndChar 
				) ;
			if( nStartChar < nEndChar )
				sFilterText.Delete( nStartChar, nEndChar - nStartChar );
			sFilterText.TrimLeft();
			if( ! GetDroppedState() )
			{
				CExtSafeString _sFilterText = LPCTSTR(sFilterText);
				ShowFilterPopupList( _sFilterText );
			}
		}
	}
	
	if( ! m_bEnableAutoComplete )
	{
		Default();
		return;
	}
	PostMessage( __EXT_COMBO_BOX_WM_USER_AUTOCOMPLETE, m_bAutoComplete ? 1L : 0L, 0L );
}

void CExtComboBoxBase::OnSize(UINT nType, int cx, int cy) 
{
	AnimationSite_ClientProgressStop( this );
	AnimationClient_StateGet( false ).Empty();
	AnimationClient_StateGet( true ).Empty();
CRect rcClient;
	GetClientRect( &rcClient );
	AnimationClient_TargetRectSet( rcClient );

	CComboBox::OnSize( nType, cx, cy );
}

void CExtComboBoxBase::OnDestroy() 
{
	HookSpyUnregister();
	LbItemRemoveAll();
	CComboBox::OnDestroy();
}

int CExtComboBoxBase::OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex) 
{
	INT nRet = CComboBox::OnCharToItem( nChar, pListBox, nIndex );
	if(		nRet >= 0 
		&&	( !LbItemIsEnabled( nRet ) )
		)
		return -2;
	else
		return nRet;
}

LRESULT CExtComboBoxBase::OnCtlColor(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );
	wParam;
bool bSimpleMode = false;
DWORD dwStyle = GetStyle();
	if(		(dwStyle&CBS_SIMPLE) == CBS_SIMPLE 
		&&	(dwStyle&CBS_DROPDOWN) != CBS_DROPDOWN
		&&	(dwStyle&CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST 
		)
		bSimpleMode = true;

	if(		(!bSimpleMode)
		&&	lParam != 0L 
		&&	lParam != (LPARAM)m_hWnd
		&&	FromHandlePermanent( (HWND)lParam ) == NULL
		)
		OnComboBoxPopupListBoxQuery( (HWND)lParam );

	return Default();
}

void CExtComboBoxBase::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{
	ASSERT_VALID( this );
	if( ! GetDroppedState() )
	{
		bool bSelChanged = false;
		switch( nChar )
		{
		case VK_LEFT:
		case VK_UP:
			{
				// Go up until we find a valid selection
				// then set the current selection to that one
				INT nCurSel = GetCurSel();
				INT nSel = nCurSel;
				bool bInitSelection = false;
				INT nCount = GetCount();
				if(	nSel < 0 && nCount > 0 )
				{
					nSel = nCount - 1;
					bInitSelection = true;
				}
				if( nSel != CB_ERR )
				{
					while( nSel > 0 )
					{
						INT nItem = nSel - ( bInitSelection ? 0 : 1 );
						if( LbItemIsEnabled( nItem ) )
						{
							if( nCurSel != nItem )
							{
								INT nRealSel = SetCurSel( nItem );
								if( nCurSel != nRealSel )
									bSelChanged = true;
							}
							break;
						}
						nSel--;
					}
				}
			}
			break;
			
		case VK_RIGHT:
		case VK_DOWN:
			{
				// Go down until we find a valid selection
				// then set the current selection to that one
				INT nCurSel = GetCurSel();
				INT nSel = nCurSel;
				bool bInitSelection = false;
				INT nCount = GetCount();
				if(	nSel < 0 && nCount > 0 )
				{
					nSel = 0;
					bInitSelection = true;
				}
				if( nSel != CB_ERR )
				{
					while( nSel < nCount - 1 )
					{
						INT nItem = nSel + ( bInitSelection ? 0 : 1 );
						if( nItem == nCount )
							break;
						if( LbItemIsEnabled( nItem ) )
						{
							if( nCurSel != nItem )
							{
								INT nRealSel = SetCurSel( nItem );
								if( nCurSel != nRealSel )
									bSelChanged = true;
							}
							break;
						}
						nSel++;
					}
				}
			}
			break;
			
		case VK_HOME:
		case VK_PRIOR:
			{
				// Select first non-disabled item
				INT nCurSel = GetCurSel();
				INT nSel = 0;
				INT nCount = GetCount();
				while( nSel < nCount - 1 )
				{
					INT nItem = nSel;
					if( LbItemIsEnabled( nItem ) )
					{
						if( nCurSel != nItem )
						{
							INT nRealSel = SetCurSel( nItem );
							if( nCurSel != nRealSel )
								bSelChanged = true;
						}
						break;
					}
					nSel++;
				}
			}
			break;
			
		case VK_END:
		case VK_NEXT:
			{
				// Select last non-disabled item
				INT nCurSel = GetCurSel();
				INT nCount = GetCount();
				INT nSel = nCount;
				while( nSel > 0 )
				{
					INT nItem = nSel - 1;
					if( LbItemIsEnabled( nItem ) )
					{
						if( nCurSel != nItem )
						{
							INT nRealSel = SetCurSel( nItem );
							if( nCurSel != nRealSel )
								bSelChanged = true;
						}
						break;
					}
					nSel--;
				}
			}
			break;
			
		default:
			CComboBox::OnKeyDown( nChar, nRepCnt, nFlags );
			return;
		}

		if( bSelChanged )
		{
			// The CBN_SELCHANGE notification message is not sent when the selection 
			// is changed using the CB_SETCURSEL message. 
			if( ! m_bEnableSelChangeWorkaround  )
			{
				CWnd * pWnd = GetParent();
				if( pWnd->GetSafeHwnd() != NULL )
				{
					ASSERT_VALID( pWnd );
					pWnd->SendMessage(
						WM_COMMAND,
						MAKEWPARAM( GetDlgCtrlID(), CBN_SELENDOK ),
						(LPARAM) m_hWnd
						);
					pWnd->SendMessage(
						WM_COMMAND,
						MAKEWPARAM( GetDlgCtrlID(), CBN_SELCHANGE ),
						(LPARAM) m_hWnd
						);
				}
			}
		}

		return;

	} // if( ! GetDroppedState() )

	CComboBox::OnKeyDown( nChar, nRepCnt, nFlags );
}

bool CExtComboBoxBase::_CreateHelper()
{
	if( m_bInitialized )
		return true;

	OnSubclassInnerEdit();
	HookSpyRegister( __EHSEF_WND_PROC_IN|__EHSEF_KEYBOARD|__EHSEF_PRE_TRANSLATION );

	m_bInitialized = true;

	return true;
}

LRESULT CExtComboBoxBase::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch( message )
	{
	case WM_MOUSEMOVE:
		if(		( ! CExtPopupMenuWnd::IsKeyPressed( VK_LBUTTON ) )
			&&	( ! CExtPopupMenuWnd::IsKeyPressed( VK_RBUTTON ) )
			&&	( ! CExtPopupMenuWnd::IsKeyPressed( VK_MBUTTON ) )
			)
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
	break;
	case WM_NOTIFY:
		if(		m_wndToolTip.GetSafeHwnd() != NULL
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
		break;
	case WM_ENABLE:
		{
			HWND hWndEdit =
				::GetWindow( GetSafeHwnd(), GW_CHILD );
			if( hWndEdit == NULL || !::IsWindow(hWndEdit) )
				break;
			::EnableWindow( hWndEdit, TRUE );
			::SendMessage( hWndEdit, EM_SETREADONLY, !wParam, 0L );
			Invalidate();
		}
		break;
	case WM_CREATE:
		{
			LRESULT lResult =
				CComboBox::WindowProc(message, wParam, lParam);;
			if( ! _CreateHelper() )
			{
				ASSERT( FALSE );
				::AfxThrowMemoryException();
			}
			return lResult;
		}
	case __EXT_COMBO_BOX_WM_USER_REALSELENDOK:
		{
			CString sCurrentText;
			GetWindowText( sCurrentText );
			
			INT nIndex = FindStringExact( -1, sCurrentText );
			if(		nIndex >=0 
				&&  ( !LbItemIsEnabled( nIndex ) ) 
				)
			{
				SetWindowText( m_sSavedText );
				
				// send the CBN_SELCHANGE notification
				GetParent()->SendMessage( 
					WM_COMMAND,
					(WPARAM) MAKELONG( GetDlgCtrlID(), CBN_SELCHANGE ),
					(LPARAM) GetSafeHwnd()
					);
			}
			return 0L;		
		}
	case __EXT_COMBO_BOX_WM_USER_AUTOCOMPLETE:
		{
			bool bAutoComplete = ( wParam > 0L ) ? true : false;
			DWORD dwCurSel = GetEditSel();
			WORD dStart = LOWORD( dwCurSel );
			WORD dEnd   = HIWORD( dwCurSel );

			CString str;
			GetWindowText( str );
			INT nLength = INT( str.GetLength() );
			INT nResult = FindString( -1, str );
			if( m_bUseCaseSensitiveAutoComplete && nResult >= 0 )
			{
				CString sItem;
				INT nCount = INT( GetCount() );
				for( ; 0 <= nResult && nResult < nCount; nResult ++ )
				{
					GetLBText( nResult, sItem );
					INT nItemLength = INT( sItem.GetLength() );
					if( nItemLength >= nLength )
					{
						if( nItemLength > nLength )
							sItem = sItem.Left( nLength );
						if( str == sItem )
							break;
					}
				}
				if( ! ( 0 <= nResult && nResult < nCount ) )
					nResult = -1;
			}

			CEdit * pEdit = GetInnerEditCtrl();
			if( pEdit != NULL )
			{
				ASSERT_VALID( pEdit );
				pEdit->SetRedraw( FALSE );
			}

			CExtComboBoxPopupListBox * pComboBoxPopupListBox = OnComboBoxPopupListBoxQuery();
			if( pComboBoxPopupListBox != NULL )
			{
				ASSERT_VALID( pComboBoxPopupListBox );
				// pComboBoxPopupListBox->SendMessage( LB_SETCURSEL, nResult, 0 );
				INT nRealSel = (INT)pComboBoxPopupListBox->SendMessage( LB_SETCURSEL, nResult, 0 );
				if( nRealSel == LB_ERR )
					pComboBoxPopupListBox->SendMessage( LB_SETTOPINDEX, 0, 0 );
			}
			
			CString sItem;
			if( nResult >= 0 )
			{
				GetLBText( nResult, sItem );
				if( ! bAutoComplete )
					SetWindowText( str );
				else
					SetWindowText( sItem );
			}
			else 
			{
				SetWindowText( str );
				if( dwCurSel != CB_ERR )
					SetEditSel( dStart, dEnd );
			}

			if(		dEnd < nLength 
				&&	dwCurSel != CB_ERR 
				)
				SetEditSel( dStart, dEnd );
			else
				SetEditSel( nLength, -1 );
		
			if( pEdit != NULL )
			{
				ASSERT_VALID( pEdit );
				pEdit->SetRedraw( TRUE );
				pEdit->Invalidate();
				pEdit->UpdateWindow();
			}
			
			return 0L;		
		}
	} // switch( message )

	return CComboBox::WindowProc(message, wParam, lParam);
}

void CExtComboBoxBase::PreSubclassWindow() 
{
	CComboBox::PreSubclassWindow();

	if( ! _CreateHelper() )
	{
		ASSERT( FALSE );
		::AfxThrowMemoryException();
	}

bool bSimpleMode = false;
DWORD dwStyle = GetStyle();
	if(		(dwStyle&CBS_SIMPLE) == CBS_SIMPLE 
		&&	(dwStyle&CBS_DROPDOWN) != CBS_DROPDOWN
		&&	(dwStyle&CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST 
		)
		bSimpleMode = true;
	if( bSimpleMode )
	{
		HWND hWnd = ::GetWindow( m_hWnd, GW_CHILD );
		for( ; hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
		{
			TCHAR strClassName[ MAX_PATH + 1 ] = _T("");
			::GetClassName(
				hWnd,
				strClassName,
				MAX_PATH
				);
			__EXT_MFC_STRLWR( strClassName, MAX_PATH );
			if( _tcscmp( strClassName, _T("combolbox") ) == 0 )
			{
				OnComboBoxPopupListBoxQuery( hWnd );
				break;
			} // if( _tcscmp( strClassName, _T("combolbox") ) == 0 )
		} // for( ; hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
	} // if( bSimpleMode )

	if( (dwStyle&WS_CLIPCHILDREN) == 0 )
		ModifyStyle( 0, WS_CLIPCHILDREN, SWP_FRAMECHANGED );

// INT nCount = GetCount();
// 	for( LONG nItem = 0; nItem < nCount; nItem++ )
// 	{
// 		LB_ITEM * pLbItem = LbItemGet( nItem );
// 		if( pLbItem == NULL )
// 		{
// 			CString sString;
// 			GetLBText( nItem, sString );
// 			LbItemSet( nItem, sString );
// 		}
// 	}
 
	AnimationSite_ClientProgressStop( this );
	AnimationClient_StateGet( false ).Empty();
	AnimationClient_StateGet( true ).Empty();
CRect rectButton;
	GetClientRect( &rectButton );
	AnimationClient_TargetRectSet( rectButton );
}

void CExtComboBoxBase::PostNcDestroy() 
{
	if( m_pWndPopupListBox != NULL )
	{
		if(		m_hWndPopupListBox != NULL
			&&	::IsWindow( m_hWndPopupListBox )
			&&	LPVOID( CWnd::FromHandlePermanent(m_hWndPopupListBox) ) == LPVOID( m_pWndPopupListBox )
			)
			m_pWndPopupListBox->m_pCB = NULL;
	}
	m_pWndPopupListBox = NULL;
	m_hWndPopupListBox = NULL;
	CComboBox::PostNcDestroy();
}

void CExtComboBoxBase::OnSubclassInnerEdit()
{
	if( m_wndInnerEditHook.GetSafeHwnd() == NULL )
	{
		HWND hWnd = ::GetWindow( m_hWnd, GW_CHILD );
		for( ; hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
		{
			TCHAR strClassName[ MAX_PATH + 1 ] = _T("");
			::GetClassName(
				hWnd,
				strClassName,
				MAX_PATH
				);
			__EXT_MFC_STRLWR( strClassName, MAX_PATH );
			if( _tcscmp( strClassName, _T("edit") ) == 0 )
			{
				m_wndInnerEditHook.m_bHandleCtxMenus = m_bHandleCtxMenus;
				VERIFY( m_wndInnerEditHook.SubclassWindow( hWnd ) );
				break;
			} // if( _tcscmp( strClassName, _T("edit") ) == 0 )
		} // for( ; hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
	} // if( m_wndInnerEditHook.GetSafeHwnd() == NULL )
}

__EXT_MFC_INT_PTR CExtComboBoxBase::OnToolHitTest(
	CPoint point,
	TOOLINFO * pTI
	) const
{
	__PROF_UIS_MANAGE_STATE;
	__EXT_MFC_IMPLEMENT_TT_REDIR_OnToolHitTest( CExtComboBoxBase, CExtToolControlBar );
	return CComboBox::OnToolHitTest( point, pTI );
}

void CExtComboBoxBase::InitToolTip()
{
	if( m_wndToolTip.m_hWnd == NULL )
	{
		m_wndToolTip.Create(this);
		m_wndToolTip.Activate(FALSE);
	}
}

void CExtComboBoxBase::ActivateTooltip(BOOL bActivate)
{
	m_bToolTipActive = bActivate ? true : false;
	if( OnAdvancedPopupMenuTipWndGet() != NULL )
		return;
	if( m_wndToolTip.GetToolCount() == 0 )
		return;
	m_wndToolTip.Activate(bActivate);
}

int CExtComboBoxBase::OnQueryMaxTipWidth( 
	__EXT_MFC_SAFE_LPCTSTR lpszText 
	)
{
	lpszText;
	return 250;
}

void CExtComboBoxBase::SetTooltipText(
	CExtSafeString * spText,
	BOOL bActivate // = TRUE
	)
{
	if( spText != NULL )
		SetTooltipText( *spText, bActivate );
}

void CExtComboBoxBase::SetTooltipText(
	CExtSafeString & sText,
	BOOL bActivate // = TRUE
	)
{
	if( ! sText.IsEmpty() )
		SetTooltipText( __EXT_MFC_SAFE_LPCTSTR(LPCTSTR(sText)), bActivate );
}

void CExtComboBoxBase::SetTooltipText(
	int nId,
	BOOL bActivate // = TRUE
	)
{
CExtSafeString sText;
	g_ResourceManager->LoadString( sText, nId );
	if( ! sText.IsEmpty() )
		SetTooltipText( &sText, bActivate );
}

void CExtComboBoxBase::SetTooltipText(
	__EXT_MFC_SAFE_LPCTSTR sText,
	BOOL bActivate // = TRUE
	)
{
	if(		sText == NULL
		|| _tcslen( sText ) == 0
		)
		return;
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
		CWnd::CancelToolTips();
		m_wndToolTip.UpdateTipText(sText,this,1);
		m_wndToolTip.SetMaxTipWidth( -1 );
		CString strText( sText );
		if( strText.Find( _T("\r") ) >= 0 )
			m_wndToolTip.SetMaxTipWidth( OnQueryMaxTipWidth( sText ) );
		m_wndToolTip.Activate(bActivate);
	}
}

CExtPopupMenuTipWnd * CExtComboBoxBase::OnAdvancedPopupMenuTipWndGet() const
{
	if( m_nAdvancedTipStyle == INT(CExtPopupMenuTipWnd::__ETS_NONE) )
		return NULL;
	return (&( CExtPopupMenuSite::g_DefPopupMenuSite.GetTip() ));
}

void CExtComboBoxBase::OnAdvancedPopupMenuTipWndDisplay(
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


COLORREF CExtComboBoxBase::OnQueryBackColor() const
{ 
	ASSERT_VALID( this );
	return m_clrBack; 
}

COLORREF CExtComboBoxBase::OnQueryTextColor() const
{ 
	ASSERT_VALID( this );
	return m_clrText; 
}

void CExtComboBoxBase::SetBkColor( 
	COLORREF clrBk 
	)
{ 
	ASSERT_VALID( this );
	m_clrBack = clrBk; 
	if( GetSafeHwnd() != NULL )
		RedrawWindow(
			NULL,
			NULL,
			RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ERASENOW | RDW_FRAME | RDW_ALLCHILDREN
			);
}

COLORREF CExtComboBoxBase::GetBkColor() const
{ 
	ASSERT_VALID( this );
	return OnQueryBackColor(); 
}

void CExtComboBoxBase::SetTextColor( 
	COLORREF clrText 
	)
{ 
	ASSERT_VALID( this );
	m_clrText = clrText; 
	if( GetSafeHwnd() != NULL )
		Invalidate();
}

COLORREF CExtComboBoxBase::GetTextColor() const
{ 
	ASSERT_VALID( this );
	return OnQueryTextColor(); 
}

HBRUSH CExtComboBoxBase::CtlColor( CDC* pDC, UINT nCtlColor )
{
	ASSERT_VALID( this );
	if(		nCtlColor == CTLCOLOR_EDIT 
		||	nCtlColor == CTLCOLOR_MSGBOX
		||	nCtlColor == CTLCOLOR_STATIC
		)
	{
		bool bDisabled = IsWindowEnabled() ? false : true;
		CExtPaintManager * pPM = PmBridge_GetPM();
		ASSERT_VALID( pPM );
		COLORREF clrText = GetTextColor();
		if( clrText == COLORREF(-1L) )
			clrText = pPM->GetColor( bDisabled ? CExtPaintManager::CLR_TEXT_DISABLED : COLOR_WINDOWTEXT );
		COLORREF clrBk = GetBkColor();
		if( clrBk == COLORREF(-1L) )
			clrBk = pPM->GetColor( bDisabled ? COLOR_3DFACE : COLOR_WINDOW, this );
		pDC->SetBkColor( clrBk );
		pDC->SetTextColor( clrText );
		if( m_clrBackPrev != clrBk )
		{
			if( m_brBack.GetSafeHandle() != NULL )
				m_brBack.DeleteObject();
			m_brBack.CreateSolidBrush( clrBk );
			m_clrBackPrev = clrBk;
		}
		return m_brBack;
	}
	else
		return (HBRUSH)Default();
}

CEdit * CExtComboBoxBase::OnQueryInnerEditCtrl() const
{
	ASSERT_VALID( this );
CEdit * pEdit = NULL;
DWORD dwWndStyle = GetStyle();
DWORD dwComboBoxType = dwWndStyle & 0x0003L;
	if( dwComboBoxType != CBS_DROPDOWNLIST )
	{
		( const_cast < CExtComboBoxBase * > ( this ) ) -> OnSubclassInnerEdit();
		if( m_wndInnerEditHook.GetSafeHwnd() != NULL )
			pEdit = (CEdit *)&m_wndInnerEditHook;
	}
	return pEdit;
}

CEdit * CExtComboBoxBase::GetInnerEditCtrl() const
{
	ASSERT_VALID( this );
	return OnQueryInnerEditCtrl();
}

INT CExtComboBoxBase::AddStringUnique( 
	__EXT_MFC_SAFE_LPCTSTR lpszString 
	)
{
	ASSERT_VALID( this );
	if( CB_ERR == FindStringExact( 0, lpszString ) )
		return AddString( lpszString );
	return CB_ERR;
}

LRESULT CExtComboBoxBase::OnCBAddString( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
LRESULT lResult = DefWindowProc( CB_ADDSTRING, wParam, lParam );
	if( lResult != CB_ERR )
		LbItemInsert( __EXT_MFC_SAFE_LPCTSTR ( LPCTSTR(LPVOID(lParam)) ), (INT)lResult );
	return lResult;
}

LRESULT CExtComboBoxBase::OnCBInsertString( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
LRESULT lResult = DefWindowProc( CB_INSERTSTRING, wParam, lParam );
	if( lResult != CB_ERR )	
		LbItemInsert( __EXT_MFC_SAFE_LPCTSTR ( LPCTSTR(LPVOID(lParam)) ), (INT)lResult );
	return lResult;
}

LRESULT CExtComboBoxBase::OnCBDeleteString( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
LRESULT lResult = DefWindowProc( CB_DELETESTRING, wParam, lParam );
	if( lResult != LB_ERR )
		LbItemRemove( (LONG)wParam );
	return lResult;
}

LRESULT CExtComboBoxBase::OnCBResetContent( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
LRESULT lResult = DefWindowProc( CB_RESETCONTENT, wParam, lParam );

	LbItemRemoveAll();

	// Workaround for the following issue:
	// The CBN_SELCHANGE notification message is not sent when the 
	// current selection is unset using the CB_RESETCONTENT message. 
	if( m_bEnableSelChangeWorkaround )
	{
		CWnd * pWnd = GetParent();
		if( pWnd->GetSafeHwnd() != NULL )
		{
			ASSERT_VALID( pWnd );
			pWnd->SendMessage(
				WM_COMMAND,
				MAKEWPARAM( GetDlgCtrlID(), CBN_SELCHANGE ),
				(LPARAM) m_hWnd
				);
		}
	}

	return lResult;
}

LRESULT CExtComboBoxBase::OnCBSetCurSel( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );

INT nCurSel = GetCurSel();
LRESULT lResult = DefWindowProc( CB_SETCURSEL, wParam, lParam );

	// Workaround for the following issue:
	// The CBN_SELCHANGE notification message is not sent when the 
	// current selection is set using the CB_SETCURSEL message. 
	if( m_bEnableSelChangeWorkaround )
	{
		if(		lResult != CB_ERR 
			&&	lResult != nCurSel 
			)		
		{
			CWnd * pWnd = GetParent();
			if( pWnd->GetSafeHwnd() != NULL )
			{
				ASSERT_VALID( pWnd );
				pWnd->SendMessage(
					WM_COMMAND,
					MAKEWPARAM( GetDlgCtrlID(), CBN_SELENDOK ),
					(LPARAM) m_hWnd
					);
				pWnd->SendMessage(
					WM_COMMAND,
					MAKEWPARAM( GetDlgCtrlID(), CBN_SELCHANGE ),
					(LPARAM) m_hWnd
					);
			}
		}
	}

	return lResult;
}

void CExtComboBoxBase::DeleteItem( LPDELETEITEMSTRUCT lpDIS ) 
{
	ASSERT_VALID( this );
	ASSERT( lpDIS->CtlType == ODT_COMBOBOX );
	lpDIS;
}

int CExtComboBoxBase::CompareItem( LPCOMPAREITEMSTRUCT lpCIS )
{
	ASSERT_VALID( this );
	if(		INT( lpCIS->itemID1 ) >= 0 
		&&	INT( lpCIS->itemID2 ) >= 0
		)
	{
		ASSERT( lpCIS->CtlType == ODT_COMBOBOX );
		LPCTSTR lpszText1 = (LPCTSTR) lpCIS->itemData1;
		ASSERT(lpszText1 != NULL);
		LPCTSTR lpszText2 = (LPCTSTR) lpCIS->itemData2;
		ASSERT(lpszText2 != NULL);
		return lstrcmp( lpszText2, lpszText1 );
	}
	return 0;
}

CSize CExtComboBoxBase::OnPopupListBoxCalcItemExtraSizes(
	LONG nItem
	) const
{
	ASSERT_VALID( this );
	nItem;
	return CSize( 2, 0 );
}

CSize CExtComboBoxBase::OnPopupListBoxCalcItemCellExtraSizes() const
{
	ASSERT_VALID( this );
	return CSize( 4, 0 );
}

LONG CExtComboBoxBase::OnPopupListBoxQueryColumnWidth(
	LONG nColNo
	) const
{
	ASSERT_VALID( this );
LONG nColWidth = LbColumnWidthGet( nColNo );
	if( nColWidth != 0L )
		nColWidth = max( 25, nColWidth ); // minimum width
	return nColWidth;
}

void CExtComboBoxBase::DrawItem( LPDRAWITEMSTRUCT lpDIS ) 
{
	ASSERT_VALID( this );
	ASSERT( lpDIS != NULL && lpDIS->hDC != NULL );
	if(		(GetStyle() & CBS_HASSTRINGS) == 0 
		&&	(GetStyle() & LBS_HASSTRINGS) == 0 
		)
		return;
INT nCount = GetCount();
INT nItem = lpDIS->itemID;
	if( nItem < 0 || nItem >= nCount )
		return;
CDC dc;
	dc.Attach( lpDIS->hDC );
	if( lpDIS->itemAction & (ODA_DRAWENTIRE | ODA_SELECT) )
	{
		CRect rcErase( lpDIS->rcItem );
		if( nItem == ( nCount - 1 ) )
		{
			CRect rcClient;
			GetClientRect( &rcClient );
			if( rcErase.bottom < rcClient.bottom )
				rcErase.bottom = rcClient.bottom;
		}
		COLORREF clrBk = COLORREF(-1L);
		if( ( lpDIS->itemState & ODS_COMBOBOXEDIT ) != 0 ) 
			clrBk = GetBkColor();
		if( clrBk == COLORREF(-1L) )
			clrBk = PmBridge_GetPM()->GetColor( ( ! IsWindowEnabled() ) ? COLOR_3DFACE : COLOR_WINDOW, this );
		dc.FillSolidRect( rcErase,  clrBk );
	}
LB_ITEM * pLbItem = LbItemGet( nItem );
	if( pLbItem != NULL )
	{
		OnPopupListBoxDrawItem( 
			dc,
			lpDIS->rcItem,
			OnPopupListBoxCalcItemExtraSizes( nItem ),
			lpDIS->itemState,
			pLbItem 
			);
	}
	else
	{
		CRect rcDrawItem( lpDIS->rcItem );
		bool bSelected = ( lpDIS->itemState & ODS_SELECTED ) ? true : false;
		if( bSelected )
			dc.FillSolidRect( &rcDrawItem, PmBridge_GetPM()->GetColor( COLOR_HIGHLIGHT, this ) );
		CString sText;
		GetLBText( nItem, sText );
		if( ! sText.IsEmpty() )
		{
			COLORREF clrTextOld = dc.SetTextColor( PmBridge_GetPM()->GetColor( bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_BTNTEXT, this ) );
			int nOldBkMode = dc.SetBkMode( TRANSPARENT );
			CRect rcText( rcDrawItem );
			rcText.DeflateRect( 2, 0 );
			UINT nFormat = 
					DT_SINGLELINE
				|	DT_LEFT
				|	DT_VCENTER
				|	DT_NOPREFIX
				;
			if( sText.Find( _T('\t') ) != -1 ) // do tabs expanding
				nFormat |= DT_EXPANDTABS;
			HFONT hFont = OnQueryFont();
			if( hFont == NULL )
				hFont = (HFONT)PmBridge_GetPM()->m_FontNormal.GetSafeHandle();
			ASSERT( hFont != NULL );
			HFONT hOldFont = (HFONT)::SelectObject( dc.m_hDC, hFont );
			CExtRichContentLayout::stat_DrawText( dc.m_hDC, LPCTSTR(sText), &rcText, nFormat, 0 );
			::SelectObject( dc.m_hDC, hOldFont );
			dc.SetBkMode( nOldBkMode );
			dc.SetTextColor( clrTextOld );
		} // if( ! sText.IsEmpty() )
		if( bSelected )
		{
			COLORREF clrTextOld = dc.SetTextColor( RGB(255,255,255) );
			COLORREF clrBkOld = dc.SetBkColor( RGB(0,0,0) );
			dc.DrawFocusRect( &rcDrawItem );
			dc.SetBkColor( clrBkOld );
			dc.SetTextColor( clrTextOld );
		}	
	}
	dc.Detach();
}

void CExtComboBoxBase::MeasureItem( LPMEASUREITEMSTRUCT lpMIS ) 
{
	ASSERT_VALID( this );
	ASSERT( lpMIS != NULL );
	if(		(GetStyle() & CBS_HASSTRINGS) == 0 
		&&	(GetStyle() & LBS_HASSTRINGS) == 0 
		)
		return;
	lpMIS->itemWidth = lpMIS->itemHeight = 16;
INT nItem = lpMIS->itemID;
	if( nItem < 0 || nItem >= GetCount() )
		return;
	OnPopupListBoxMeasureItem( lpMIS );
}

void CExtComboBoxBase::OnPopupListBoxDrawItem( 
	CDC & dc,
	const RECT & rcItem,
	const CSize & szExtra,
	UINT nState,
	const CExtComboBoxBase::LB_ITEM * pLbItem
	)
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if(		(GetStyle() & CBS_HASSTRINGS) == 0 
		&&	(GetStyle() & LBS_HASSTRINGS) == 0 
		)
		return;
bool bEnabled = true;
	if(		( ! GetDroppedState() )
		&&	( ! IsWindowEnabled() )
		)
		bEnabled = false;
	else if( pLbItem != NULL )
		bEnabled = pLbItem->IsEnabled();
CRect rcDrawItem( rcItem );
bool bSelected = ( nState & ODS_SELECTED ) ? true : false;
	if( bSelected && bEnabled )
		dc.FillSolidRect( &rcDrawItem, PmBridge_GetPM()->GetColor( COLOR_HIGHLIGHT, this ) );
INT nOldBkMode = dc.SetBkMode( TRANSPARENT );
CRect rcText( rcDrawItem );
	rcText.DeflateRect( szExtra );
	// draw item cells
	if( pLbItem != NULL )
	{
		INT nCount = pLbItem->LbItemCellGetCount();
		for( INT nCellItem = 0; nCellItem < nCount; nCellItem++ )
		{
			LB_ITEM::CELL * pItemCell = pLbItem->LbItemCellGet( nCellItem );
			ASSERT( pItemCell != NULL );
			
			LONG nColWidth = OnPopupListBoxQueryColumnWidth( nCellItem );
			if( nColWidth > 0L )
			{
				COLORREF clrBk = COLORREF(-1L);
				if( ( nState & ODS_COMBOBOXEDIT ) != 0 ) 
					clrBk = GetBkColor();
				if( clrBk == COLORREF(-1L) )
					clrBk = pItemCell->m_clrBack;
				if(		clrBk != COLORREF(-1L)
					&&	( (!bSelected) || (!bEnabled) )
					)
				{
					dc.FillSolidRect(
						&rcText,
						clrBk
						);
				}
				
				CString sText = pItemCell->m_sItemText;
				if( !sText.IsEmpty() )
				{
					COLORREF clrText = COLORREF(-1L);
					if( !bSelected )
						clrText = pItemCell->m_clrText;
					if( clrText == COLORREF(-1L) )
						clrText = GetTextColor();
					if( clrText == COLORREF(-1L) )
						clrText = 
							PmBridge_GetPM()->GetColor( 
								( bSelected && bEnabled )
									? COLOR_HIGHLIGHTTEXT 
									: bEnabled
										? COLOR_BTNTEXT 
										: COLOR_GRAYTEXT, 
									this
									);
					COLORREF clrTextOld = dc.SetTextColor( clrText );
					
					UINT nFormat = 
							DT_SINGLELINE
						|	DT_LEFT
						|	DT_VCENTER
						|	DT_NOPREFIX
						;
					if( sText.Find( _T('\t') ) != -1 ) // do tabs expanding
						nFormat |= DT_EXPANDTABS;
					
					CRect rcItemText( rcText );
					if(		nColWidth > 0L 
						&&	nCellItem < nCount - 1
						)
						rcItemText.right = rcItemText.left + nColWidth;
					
					HFONT hFont = OnQueryFont();
					if( hFont == NULL )
						hFont = (HFONT)PmBridge_GetPM()->m_FontNormal.GetSafeHandle();
					ASSERT( hFont != NULL );
					HFONT hOldFont = (HFONT)::SelectObject( dc.m_hDC, hFont );
					CExtRichContentLayout::stat_DrawText(
						dc.m_hDC,
						LPCTSTR(sText),
						&rcItemText,
						(nFormat|DT_END_ELLIPSIS), 0
						);
					::SelectObject( dc.m_hDC, hOldFont );
					
					dc.SetTextColor( clrTextOld );
					
				} // if( ! sText.IsEmpty() )
				
				if( nColWidth > 0 )
					rcText.left += nColWidth;
			}

		} // for( ...
	} // if( pLbItem != NULL )

	dc.SetBkMode( nOldBkMode );

	if(	bSelected && bEnabled )
	{
		COLORREF clrTextOld = 
			dc.SetTextColor( RGB(255,255,255) );
		COLORREF clrBkOld = dc.SetBkColor( RGB(0,0,0) );
		::DrawFocusRect( dc, &rcDrawItem );
		dc.SetBkColor( clrBkOld );
		dc.SetTextColor( clrTextOld );
	}
}

void CExtComboBoxBase::OnPopupListBoxMeasureItem( LPMEASUREITEMSTRUCT lpMIS )
{
	ASSERT_VALID( this );
	ASSERT( lpMIS != NULL );

	if(		(GetStyle() & CBS_HASSTRINGS) == 0 
		&&	(GetStyle() & LBS_HASSTRINGS) == 0 
		)
		return;
	
	lpMIS->itemWidth = lpMIS->itemHeight = 16;
INT nItem = lpMIS->itemID;
	if( nItem < 0 || nItem >= GetCount() )
		return;

HFONT hFont = OnQueryFont();
	if( hFont == NULL )
		hFont = (HFONT)PmBridge_GetPM()->m_FontNormal.GetSafeHandle();
	ASSERT( hFont != NULL );

INT nHeight = 0;
INT nWidth = 0;

LB_ITEM * pLbItem = LbItemGet( nItem );
	if( pLbItem != NULL )
	{
		INT nCount = pLbItem->LbItemCellGetCount();
		for( INT nCellItem = 0; nCellItem < nCount; nCellItem++ )
		{
			LB_ITEM::CELL * pItemCell = pLbItem->LbItemCellGet( nCellItem );
			if( pItemCell != NULL )
			{
				CExtSafeString sText = pItemCell->m_sItemText;
				if( !sText.IsEmpty() )
				{
					UINT nFormat = 
							DT_SINGLELINE
						|	DT_LEFT
						|	DT_VCENTER
						|	DT_NOPREFIX
						;
					if( sText.Find( _T('\t') ) != -1 ) // do tabs expanding
						nFormat |= DT_EXPANDTABS;

					CWindowDC dc( NULL );
					CRect rcMeasure =
						CExtPaintManager::stat_CalcTextDimension( 
							dc, 
							*CFont::FromHandle( hFont ), 
							sText,
							nFormat
							);

					nHeight = max( nHeight, rcMeasure.Height() );
					nWidth += rcMeasure.Width();
					CSize szExtra = 
						OnPopupListBoxCalcItemCellExtraSizes();
					nWidth += szExtra.cx;
				}
			} // if( pItemCell != NULL )
		}

	}
	else
	{
		CExtSafeString sMeasure;
		GetLBText( nItem, sMeasure );
		if( sMeasure.IsEmpty() )
			return;
		CWindowDC dc( NULL );
		CRect rcMeasure =
			CExtPaintManager::stat_CalcTextDimension( 
				dc, 
				*CFont::FromHandle( hFont ), 
				sMeasure,
				DT_SINGLELINE | DT_LEFT | DT_NOPREFIX
				);
		
		nWidth = rcMeasure.Width();
		nHeight = rcMeasure.Height();
	}
	
	lpMIS->itemWidth = nWidth;
	lpMIS->itemHeight = max( nHeight, (INT)lpMIS->itemHeight );

	CSize szExtra = 
		OnPopupListBoxCalcItemExtraSizes( nItem );
	lpMIS->itemWidth += szExtra.cx * 2;
	lpMIS->itemHeight += szExtra.cy * 2;
}

HFONT CExtComboBoxBase::OnQueryFont() const
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

INT CExtComboBoxBase::LbItemSet(
	INT nIndex,
	__EXT_MFC_SAFE_LPCTSTR lpszItem		
	)
{
INT nCount = INT( m_arrLBItems.GetSize() );
	if( nIndex < 0 || nIndex > nCount )
		return CB_ERR;

LB_ITEM * pLbItem = new LB_ITEM();
	ASSERT( pLbItem != NULL );
	m_arrLBItems.SetAtGrow( nIndex, pLbItem );

INT nItem = pLbItem->LbItemCellInsert( lpszItem );
LB_ITEM::CELL * pItemCell = 
		pLbItem->LbItemCellGet( nItem );
	ASSERT( pItemCell != NULL );

	return nIndex;
}

INT CExtComboBoxBase::LbItemInsert(
	__EXT_MFC_SAFE_LPCTSTR lpszItem,
	INT nIndex // = -1
	)
{
INT nCount = INT( m_arrLBItems.GetSize() );
	if( nIndex < 0 || nIndex > nCount )
		nIndex = nCount; // append
	
LB_ITEM * pLbItem = new LB_ITEM();
	ASSERT( pLbItem != NULL );
	m_arrLBItems.InsertAt( nIndex, pLbItem );

INT nItem = pLbItem->LbItemCellInsert( lpszItem );
LB_ITEM::CELL * pItemCell = 
		pLbItem->LbItemCellGet( nItem );
	ASSERT( pItemCell != NULL );

	return nIndex;
}

bool CExtComboBoxBase::LbItemRemove(
	LONG nIndex
	)
{
INT nCount = INT( m_arrLBItems.GetSize() );
	if( nIndex < 0 || nIndex >= nCount )
		return false;
LB_ITEM * pLbItem = m_arrLBItems.GetAt( nIndex );
	ASSERT( pLbItem != NULL );
	if( pLbItem != NULL )
	{
		delete pLbItem;
		pLbItem = NULL;
	}
	m_arrLBItems.RemoveAt( nIndex );
	return true;
}

void CExtComboBoxBase::LbItemRemoveAll()
{
	while( m_arrLBItems.GetSize() > 0 )
	{
		LB_ITEM * pLbItem = m_arrLBItems[0];
		if( pLbItem != NULL ) 
		{
			delete pLbItem;
			pLbItem = NULL;
		}
		m_arrLBItems.RemoveAt(0);
	}
}

CExtComboBoxBase::LB_ITEM * CExtComboBoxBase::LbItemGet( 
	LONG nItem
	) const
{
	ASSERT_VALID( this );
INT nCount = INT( m_arrLBItems.GetSize() );
	if( nItem < 0 || nItem >= nCount )
		return NULL;

LB_ITEM * pLbItem = m_arrLBItems.GetAt( nItem );
	ASSERT( pLbItem != NULL );
	return pLbItem;
}

LONG CExtComboBoxBase::LbColumnWidthGet( 
	LONG nColNo
	) const
{
	ASSERT_VALID( this );
LONG nWidth = -1L;
	if( ! m_mapColumnsWidth.Lookup( nColNo, nWidth ) )
		return -1L;
	return nWidth;
}

void CExtComboBoxBase::LbColumnWidthSet( 
	LONG nColNo,
	LONG nWidth
	)
{
	ASSERT_VALID( this );
	m_mapColumnsWidth.SetAt( nColNo, nWidth );
}

bool CExtComboBoxBase::LbItemIsEnabled(
	LONG nItem
	) const
{
	ASSERT_VALID( this );
	if(		nItem < 0L 
		||	nItem >= GetCount() 
		)
		return false;
bool bEnabled = true;
LB_ITEM * pLbItem = LbItemGet( nItem );
	if( pLbItem != NULL )
		bEnabled = pLbItem->IsEnabled();
	return bEnabled;
}

void CExtComboBoxBase::LbItemEnable(
	LONG nItem,
	bool bEnable // = true
	)
{
	ASSERT_VALID( this );
	if(		nItem < 0L 
		||	nItem >= GetCount() 
		)
		return;
LB_ITEM * pLbItem = LbItemGet( nItem );
	if( pLbItem != NULL )
		pLbItem->Enable( bEnable );
}

#define UPDATE_TIMER		1
#define UPDATE_TIMER_PERIOD	100

/////////////////////////////////////////////////////////////////////////////
// CExtComboBox

IMPLEMENT_DYNCREATE( CExtComboBox, CExtComboBoxBase );

CExtComboBox::CExtComboBox()
	: CExtComboBoxBase()
	, m_bLButtonDown( false )
	, m_bPainted( false )
	, m_dwLastStateCode( 0 )
{
}

CExtComboBox::~CExtComboBox()
{
CExtAnimationSite * pAcAS = AnimationClient_SiteGet();
	if( pAcAS != NULL )
		pAcAS->AnimationSite_ClientRemove( this );
}

BEGIN_MESSAGE_MAP(CExtComboBox, CExtComboBoxBase)
	//{{AFX_MSG_MAP(CExtComboBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtComboBox message handlers

BOOL CExtComboBox::OnSetCursor( CWnd * pWnd, UINT nHitTest, UINT message )
{
	if( message == WM_MOUSEMOVE )
	{
		CRect rcWnd;
		GetWindowRect( &rcWnd );
		if( rcWnd.PtInRect( GetCurrentMessage()->pt ) )
		{
			SetTimer( UPDATE_TIMER, UPDATE_TIMER_PERIOD, NULL );
			OnTimer( UPDATE_TIMER );
		}
	}
	return CExtComboBoxBase::OnSetCursor(pWnd, nHitTest, message);
}

void CExtComboBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = true;
	CExtComboBoxBase::OnLButtonDown(nFlags, point);
}

void CExtComboBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bLButtonDown = false;
	Invalidate();
	CExtComboBoxBase::OnLButtonUp(nFlags, point);
}

bool CExtComboBox::IsFocused() const
{
bool bFocused = false;
HWND hWndFocus = ::GetFocus();
	if(		hWndFocus == GetSafeHwnd()
		||	::IsChild( GetSafeHwnd(), hWndFocus )
		)
		bFocused = true;
	return bFocused;
}

bool CExtComboBox::IsHovered() const
{
CPoint ptCursorPos( 0, 0 );
	::GetCursorPos( &ptCursorPos );
CRect rcItem;
	GetWindowRect( &rcItem );
bool bHovered = false;
	if( rcItem.PtInRect(ptCursorPos) )
		bHovered = true;
	return bHovered;
}

void CExtComboBox::OnSetFocus( CWnd * pOldWnd ) 
{
HWND hWndOwn = m_hWnd;
CExtComboBoxBase::OnSetFocus( pOldWnd );
	if( ! ::IsWindow( hWndOwn ) )
		return;
	Invalidate();     
}

void CExtComboBox::OnKillFocus( CWnd * pNewWnd ) 
{
HWND hWndOwn = m_hWnd;
	CExtComboBoxBase::OnKillFocus( pNewWnd );
	if( ! ::IsWindow( hWndOwn ) )
		return;
	Invalidate();     
}

void CExtComboBox::OnTimer( __EXT_MFC_UINT_PTR nIDEvent )
{
	if( UPDATE_TIMER == nIDEvent )
	{
		CClientDC dc( this );
		if( m_bLButtonDown )
		{
			KillTimer( UPDATE_TIMER );
			if( m_bPainted )
			{
				_OnDrawComboImpl( 
					GetDroppedState() ? true : false, 
					IsHovered(), 
					&dc 
					);
				m_bPainted = false;
			}
		}
		else if( !IsHovered() && !IsFocused() )
		{
			KillTimer( UPDATE_TIMER );
			if( m_bPainted == TRUE )
			{
				_OnDrawComboImpl( 
					false, 
					false, 
					&dc 
					);
				m_bPainted = false;
			}
		}
		else
		{
			if( !m_bPainted )
			{
				m_bPainted = true;
				_OnDrawComboImpl( 
					GetDroppedState() ? true : false, 
					true, 
					&dc 
					);
			}
			else
				return;
		}
		return;
	}
	if( AnimationSite_OnHookTimer( UINT(nIDEvent) ) )
		return;
	CExtComboBoxBase::OnTimer( nIDEvent );
}

BOOL CExtComboBox::OnEraseBkgnd(CDC* pDC)
{
	pDC;
	return TRUE;
}

void CExtComboBox::OnPaint()
{
	if( ( GetExStyle() & (WS_EX_DLGMODALFRAME|WS_EX_CLIENTEDGE|WS_EX_STATICEDGE) ) != 0 )
		ModifyStyleEx( WS_EX_DLGMODALFRAME|WS_EX_CLIENTEDGE|WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED );
COLORREF clrFillClientArea = COLORREF(-1L);
DWORD dwStyle = GetStyle();
CPaintDC dcPaint( this );
	CExtPaintManager::stat_ExcludeChildAreas( dcPaint.GetSafeHdc(), GetSafeHwnd() );
CRect rcClient;
	GetClientRect( &rcClient );
CExtMemoryDC dc( &dcPaint, &rcClient );
CRgn _rgn;
	if( ( dwStyle & ( CBS_OWNERDRAWFIXED|CBS_OWNERDRAWVARIABLE ) ) != 0 )
	{
		CRect rcRgn = rcClient;
		rcRgn.right -= ::GetSystemMetrics( SM_CXVSCROLL );
		if( _rgn.CreateRectRgnIndirect( &rcRgn ) )
			dc.SelectClipRgn( &_rgn );
	}
	DefWindowProc( WM_PAINT, (WPARAM)dc.GetSafeHdc(), (LPARAM)0 );
	if( _rgn.GetSafeHandle() != NULL )
		dc.SelectClipRgn( NULL );
	if(		(dwStyle&CBS_SIMPLE) == CBS_SIMPLE 
		&&	(dwStyle&CBS_DROPDOWN) != CBS_DROPDOWN
		&&	(dwStyle&CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST
		)
	{
		if(		(! PmBridge_GetPM()->GetCb2DbTransparentMode(this) )
			||	(! PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
			)
			clrFillClientArea = PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT, this );
	}
	if( clrFillClientArea != COLORREF(-1L) )
		dc.FillSolidRect( &rcClient, clrFillClientArea );
	_OnDrawComboImpl( GetDroppedState() ? true : false, IsHovered(), &dc );
}

void CExtComboBox::_OnDrawComboImpl(
	bool bPressed,
	bool bHover,
	CDC * pDC // = NULL
	)
{
	if( pDC != NULL && AnimationClient_StatePaint( *pDC ) )
		return;
bool bCallReleaseDC = false;
	if( pDC == NULL )
	{
		pDC = GetDC();
		ASSERT( pDC != NULL );
		bCallReleaseDC = true;
	}
CRect rcClient;
	GetClientRect( &rcClient );
bool bEnabled = IsWindowEnabled() ? true : false;
bool bPushed = ( bPressed || GetDroppedState() ) ? true : false;
	if( ! bEnabled )
	{
		bPushed = false;
		bHover = false;
	}
	if( CExtPopupMenuWnd::IsMenuTracking() )
		bHover = false;
	else if( (!IsHovered()) && IsFocused() )
		bHover = true;
DWORD dwStyle = GetStyle();
	if(		(dwStyle&CBS_SIMPLE) == CBS_SIMPLE 
		&&	(dwStyle&CBS_DROPDOWN) != CBS_DROPDOWN
		&&	(dwStyle&CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST
		)
	{
		HWND hWndFocus = ::GetFocus();
		CEdit * pEdit = GetInnerEditCtrl();
		CExtComboBoxPopupListBox * pLB = OnComboBoxPopupListBoxQuery();
		HWND hWndEdit = pEdit->GetSafeHwnd();
		ASSERT( hWndEdit != NULL );
		HWND hWndLB = pLB->GetSafeHwnd();
		ASSERT( hWndLB != NULL );
		bool bFocus = false;
		if(		hWndFocus != NULL
			&&	(	hWndFocus == hWndEdit
				||	hWndFocus == hWndLB
				)
			)
			bFocus = true;
		bool bReadOnly = ( ( pEdit->GetStyle() & ES_READONLY ) != 0 ) ? true : false;
		bool bMouseOver = false;
		CRect _rcClient;
		pLB->GetWindowRect( &_rcClient );
		ScreenToClient( &_rcClient );
		_rcClient.InflateRect( 3, 3, 3, 2 );
		pDC->FillSolidRect( &_rcClient, PmBridge_GetPM()->GetColor( COLOR_WINDOW ) );
		pEdit->GetClientRect( &_rcClient );
		pEdit->ClientToScreen( &_rcClient );
		ScreenToClient( &_rcClient );
		_rcClient.InflateRect( 3, 3 );
		CExtPaintManager::PAINTCONTROLFRAMEDATA _pcfdEdit(
			pEdit,
			_rcClient,
			true,
			bMouseOver,
			bEnabled,
			bFocus,
			bReadOnly
			);
		PmBridge_GetPM()->PaintControlFrame( *pDC, _pcfdEdit );
	}
	else
	{
		bool bRightText = false;
		if( (GetExStyle()&WS_EX_RIGHT) != 0 )
			bRightText = true;
		// erase are of the button that contains the drop-down arrow
		CRect rcDDButton( rcClient );
		if( bRightText )
		{
			rcDDButton.right = rcDDButton.left + ::GetSystemMetrics( SM_CXHTHUMB );
			rcDDButton.right += 2;
		}
		else
		{
			rcDDButton.left = rcDDButton.right - ::GetSystemMetrics( SM_CXHTHUMB );
			rcDDButton.left -= 2;
			if( rcDDButton.left > rcDDButton.right )
				rcDDButton.left = rcDDButton.right;
		}
		COLORREF clrBk = GetBkColor();
		if( clrBk == COLORREF(-1L) )
			clrBk = PmBridge_GetPM()->GetColor( ( ! bEnabled ) ? COLOR_3DFACE : COLOR_WINDOW, this );
		pDC->FillSolidRect( &rcDDButton, clrBk );
		// fix for disabled edit border inside combo box
		if(		(!bEnabled)
			&&	(dwStyle&CBS_DROPDOWN) == CBS_DROPDOWN 
			)
		{
			CWnd * pWndChild = GetWindow( GW_CHILD );
			if( pWndChild != NULL )
			{
				ASSERT_VALID( pWndChild );
				static const TCHAR szEdit[] = _T("edit");
				TCHAR szCompare[ sizeof(szEdit)/sizeof(szEdit[0]) + 1 ] = _T("");
				::GetClassName( pWndChild->GetSafeHwnd(), szCompare, sizeof(szCompare)/sizeof(szCompare[0]) );
				if( ! lstrcmpi( szCompare, szEdit ) )
				{
					CRect rcChild;
					pWndChild->GetWindowRect( &rcChild );
					ScreenToClient( rcChild );
					rcChild.InflateRect( 1, 1 );
					pDC->Draw3dRect( &rcChild, clrBk, clrBk );
				}
			}
		}
		CExtPaintManager::PAINTCOMBOFRAMEDATA _pcfd( this, rcClient, bHover, bPushed, bEnabled, bRightText );
		PmBridge_GetPM()->PaintComboFrame( *pDC, _pcfd );
		DWORD dwLastStateCode = 0;
		if( m_bLButtonDown )
			dwLastStateCode |= 0x01;
		if( IsHovered() )
			dwLastStateCode |= 0x02;
		if( IsFocused() )
			dwLastStateCode |= 0x04;
		if( bPressed )
			dwLastStateCode |= 0x08;
		if( m_dwLastStateCode != dwLastStateCode )
		{
			if( AnimationClient_StateGet( true ).FromSurface( pDC->m_hDC, rcClient ) )
			{
				if( AnimationClient_StateGet( false ).IsEmpty() )
					AnimationClient_StateGet( false ) = AnimationClient_StateGet( true );
				else
					AnimationSite_OnClientStateChanged( this, 0 );
			}
			m_dwLastStateCode = dwLastStateCode;
		}
	}
	if( bCallReleaseDC )
		ReleaseDC( pDC );
}

BOOL CExtComboBox::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	ASSERT_VALID( this );
	if( message == WM_COMMAND )
	{
		INT nCode = HIWORD(wParam);
		switch( nCode )
		{
		case CBN_CLOSEUP:
			Invalidate();
			break;
		}
	}
	return CExtComboBoxBase::OnChildNotify(message, wParam, lParam, pLResult);
}

LRESULT CExtComboBox::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_PRINT || message == WM_PRINTCLIENT )
	{
		CExtComboBoxBase::WindowProc( message, wParam, lParam );
		CDC * pDC = CDC::FromHandle( (HDC)wParam );
		CRect rcClient;
		GetClientRect( &rcClient );
		CExtMemoryDC dc( pDC, &rcClient, CExtMemoryDC::MDCOPT_TO_MEMORY|CExtMemoryDC::MDCOPT_FILL_BITS|CExtMemoryDC::MDCOPT_RTL_COMPATIBILITY );
		DWORD dwStyle = GetStyle();
		if(		(dwStyle&CBS_SIMPLE) == CBS_SIMPLE 
			&&	(dwStyle&CBS_DROPDOWN) != CBS_DROPDOWN
			&&	(dwStyle&CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST
			)
		{
			if(		(! PmBridge_GetPM()->GetCb2DbTransparentMode(this) )
				||	(! PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
				)
				dc.FillSolidRect( &rcClient, PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT, this ) );
		}
		_OnDrawComboImpl( GetDroppedState() ? true : false, IsHovered(), &dc );
		if( (lParam&PRF_CHILDREN) != 0 )
			CExtPaintManager::stat_PrintChildren( m_hWnd, message, dc.GetSafeHdc(), lParam, false );
		return (!0);
	}
	return CExtComboBoxBase::WindowProc( message, wParam, lParam );
}

void CExtComboBox::AnimationSite_OnProgressShutdownTimer( UINT nTimerID )
{
	ASSERT_VALID( this );
	CExtComboBoxBase::AnimationSite_OnProgressShutdownTimer( nTimerID );
}

const CExtAnimationParameters *
	CExtComboBox::AnimationClient_OnQueryAnimationParameters(
		INT eAPT // __EAPT_*** animation type
		) const
{
	ASSERT_VALID( this );
	eAPT;
	return (&PmBridge_GetPM()->g_DefAnimationParametersEmpty);
// const CExtAnimationParameters * pAnimationParameters =
// 		PmBridge_GetPM()->Animation_GetParameters(
// 			eAPT,
// 			(CObject*)this,
// 			this
// 			);
// 	return pAnimationParameters;
}

//bool CExtComboBox::AnimationClient_CacheNextState(
//	CDC & dc,
//	const RECT & rcAcAnimationTarget,
//	bool bAnimate,
//	INT eAPT // __EAPT_*** animation type
//	)
//{
//	ASSERT_VALID( this );
//	ASSERT_VALID( (&dc) );
//	ASSERT( dc.GetSafeHdc() != NULL );
//	ASSERT( AnimationClient_CacheGeneratorIsLocked() );
//
//DWORD dwWndStyle = GetStyle();
//	if(		(dwWndStyle&(CBS_OWNERDRAWFIXED|CBS_OWNERDRAWVARIABLE)) != 0 
//		&&	(dwWndStyle & 0x0003L) == CBS_DROPDOWNLIST
//		)
//	{
//		DefWindowProc( WM_PAINT, (WPARAM)dc.GetSafeHdc(), (LPARAM)0 );
//		_OnDrawComboImpl( 
//			GetDroppedState() ? true : false, 
//			IsHovered(), 
//			&dc 
//			);
//		return
//			CExtComboBoxBase::AnimationClient_CacheNextState(
//				dc,
//				rcAcAnimationTarget,
//				bAnimate,
//				eAPT
//				);
//	}
//
//	CExtPaintManager::stat_ExcludeChildAreas(
//		dc.GetSafeHdc(),
//		GetSafeHwnd()
//		);
//CRect rcClient;
//	GetClientRect( &rcClient );
//	DefWindowProc( WM_PAINT, (WPARAM)dc.GetSafeHdc(), (LPARAM)0 );
//
//DWORD dwStyle = GetStyle();
//	if(		(dwStyle&CBS_SIMPLE) == CBS_SIMPLE 
//		&&	(dwStyle&CBS_DROPDOWN) != CBS_DROPDOWN
//		&&	(dwStyle&CBS_DROPDOWNLIST) != CBS_DROPDOWNLIST
//		)
//	{
//		if(		(! PmBridge_GetPM()->GetCb2DbTransparentMode(this) )
//			||	(! PmBridge_GetPM()->PaintDockerBkgnd( true, dc, this ) )
//			)
//			dc.FillSolidRect(
//				&rcClient,
//				PmBridge_GetPM()->GetColor(
//					CExtPaintManager::CLR_3DFACE_OUT, this
//					)
//				);
//	}
//
//	_OnDrawComboImpl( 
//		GetDroppedState() ? true : false, 
//		IsHovered(), 
//		&dc 
//		);
//
//	return
//		CExtComboBoxBase::AnimationClient_CacheNextState(
//			dc,
//			rcAcAnimationTarget,
//			bAnimate,
//			eAPT
//			);
//}

/////////////////////////////////////////////////////////////////////////////
// CExtCheckComboBox window

IMPLEMENT_DYNCREATE( CExtCheckComboBox, CExtComboBox );

CExtCheckComboBox::CExtCheckComboBox()
	: m_bHaveText( false )
	, m_bHaveItemHeight( false )
	, m_bUsePM( true )
{
}

CExtCheckComboBox::~CExtCheckComboBox()
{
}

BEGIN_MESSAGE_MAP( CExtCheckComboBox, CExtComboBox )
	//{{AFX_MSG_MAP(CExtCheckComboBox)
	ON_MESSAGE( WM_GETTEXT, OnGetText )
	ON_MESSAGE( WM_GETTEXTLENGTH, OnGetTextLength )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CExtCheckComboBox::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	dwStyle &= ~0xF;
	dwStyle |= CBS_DROPDOWNLIST|CBS_OWNERDRAWVARIABLE|CBS_HASSTRINGS;
	return CExtComboBox::Create( dwStyle, rect, pParentWnd, nID );
}

LRESULT CExtCheckComboBox::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == CB_RESETCONTENT )
		m_bHaveItemHeight = m_bHaveText = false;
	else
	if(		WM_KEYFIRST <= message && message <= WM_KEYLAST
		&&	( wParam == VK_UP || wParam == VK_DOWN || wParam == VK_LEFT || wParam == VK_RIGHT || wParam == VK_PRIOR || wParam == VK_NEXT || wParam == VK_HOME || wParam == VK_END )
		)
	{
		if( ! GetDroppedState() )
		{
			ShowDropDown();
			return 0L;
		}
		SetRedraw( FALSE );
		LRESULT lResult = CExtComboBox::WindowProc( message, wParam, lParam );
		SetRedraw( TRUE );
		Invalidate( TRUE );
		UpdateWindow();
		//_OnDrawComboImpl( false, false );
		return lResult;
	}
	return CExtComboBox::WindowProc( message, wParam, lParam );
}

BOOL CExtCheckComboBox::PreTranslateMessage( MSG * pMsg )
{
	if( pMsg->message == WM_MOUSEWHEEL )
		return TRUE;
	return CExtComboBox::PreTranslateMessage( pMsg );
}

void CExtCheckComboBox::PreSubclassWindow()
{
	ASSERT_VALID( this );
	m_bHaveItemHeight = m_bHaveText = false;
	CExtComboBox::PreSubclassWindow();
}

void CExtCheckComboBox::PostNcDestroy()
{
	ASSERT_VALID( this );
	m_bHaveItemHeight = m_bHaveText = false;
	CExtComboBox::PostNcDestroy();
}

void CExtCheckComboBox::_OnDrawComboImpl(
	bool bPressed,
	bool bHover,
	CDC * pDC // = NULL
	)
{
	if( pDC != NULL && AnimationClient_StatePaint( *pDC ) )
		return;
CRect rcClient;
	GetClientRect( &rcClient );
bool bCallReleaseDC = false;
	if( pDC == NULL )
	{
		pDC = GetDC();
		ASSERT( pDC != NULL );
		bCallReleaseDC = true;
	}
DRAWITEMSTRUCT dis;
	::memset( &dis, 0, sizeof( DRAWITEMSTRUCT ) );
	dis.CtlType = ODT_LISTBOX;
	dis.CtlID = GetDlgCtrlID();
	dis.itemID = UINT(-1);
	//dis.hDC = pDC->GetSafeHdc();
CRect rc( rcClient );
	rc.DeflateRect(
		3,
		3,
		3 + ::GetSystemMetrics( SM_CXVSCROLL ), // + PmBridge_GetPM()->GetDropDownButtonWidth(),
		3
		);
	::CopyRect( &dis.rcItem, &rc );
	dis.itemAction = ODA_DRAWENTIRE;
	dis.hwndItem = GetSafeHwnd();
	if( ::GetFocus() == m_hWnd )
		dis.itemState |= ODS_FOCUS|ODS_SELECTED;
	{ // block for memory dc
		CExtMemoryDC dc(
			pDC,
			&rcClient,
			CExtMemoryDC::MDCOPT_TO_MEMORY | CExtMemoryDC::MDCOPT_FILL_BITS | CExtMemoryDC::MDCOPT_RTL_COMPATIBILITY
			);
		dis.hDC = dc.GetSafeHdc();
		SendMessage( WM_DRAWITEM, (WPARAM)GetDlgCtrlID(), (LPARAM)&dis );
		CExtComboBox::_OnDrawComboImpl( bPressed, bHover, &dc );
	} // block for memory dc
	if( bCallReleaseDC )
		ReleaseDC( pDC );
}

void CExtCheckComboBox::OnPaintItemCheck( CDC & dc, const RECT & rcCheck, INT nIndex )
{
	ASSERT_VALID( this );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( ! dc.RectVisible(&rcCheck) )
		return;
	ASSERT( nIndex >= 0 && nIndex < GetCount() );
INT nCheck = CheckGet( nIndex );
bool bCheck = ( nCheck != 0 ) ? 1 : 0;
//BOOL bDisabled = (!IsEnabled( nIndex ));
bool bDisabled = IsWindowEnabled() ? false : true;
	if( m_bUsePM )
	{
		bool bPushed = false, bHover = false, bPushTracking = false, bKeyTracking = false, bEnabled = bDisabled ? false : true;
		CExtPaintManager::eBoxState_t eState = CExtPaintManager::BOX_UNCHECKED;
		if( /*nIndex < 0 ||*/ GetCurSel() == nIndex )
			bHover = true;

		switch( nCheck ) 
		{
		case BST_UNCHECKED:
			if( bEnabled )
			{
				if(		(   bHover  && (!bPushTracking) ) 
					||  ( (!bHover) &&   bPushTracking && (!bKeyTracking) ) 
					)
					eState = CExtPaintManager::BOX_MOUSE_HOVER_UNCHECKED;
				else if( (bHover && bPushTracking) || bKeyTracking )
					eState = CExtPaintManager::BOX_LDOWN_UNCHECKED;
				else
					eState = CExtPaintManager::BOX_UNCHECKED;
			}
			else
				eState = CExtPaintManager::BOX_DISABLED_UNCHECKED;
			break;
			
		case BST_CHECKED:
			if( bEnabled )
			{
				if(		(   bHover  && (!bPushTracking) ) 
					||  ( (!bHover) &&   bPushTracking && (!bKeyTracking) ) 
					)
					eState = CExtPaintManager::BOX_MOUSE_HOVER_CHECKED;
				else if( ( bHover && bPushTracking ) || bKeyTracking )
					eState = CExtPaintManager::BOX_LDOWN_CHECKED;
				else
					eState = CExtPaintManager::BOX_CHECKED;
			}
			else
				eState = CExtPaintManager::BOX_DISABLED_CHECKED;
			break;
			
		case BST_INDETERMINATE:
			if( bEnabled )
			{
				if(		( (bHover) && (!bPushTracking) ) 
					||  ( !bHover  &&   bPushTracking && (!bKeyTracking) ) 
					)
					eState = CExtPaintManager::BOX_MOUSE_HOVER_INDETERMINATE;
				else if( (bHover && bPushTracking) || bKeyTracking )
					eState = CExtPaintManager::BOX_LDOWN_INDETERMINATE;
				else
					eState = CExtPaintManager::BOX_INDETERMINATE;
			}
			else
				eState = CExtPaintManager::BOX_DISABLED_INDETERMINATE;
			break;
		}
		CExtPaintManager * pPM = PmBridge_GetPM();
		CExtPaintManager::PAINTCHECKRADIOBUTTONDATA _pcrbd(
			this,
			eState,
			false,
			false,
			rcCheck,
			_T(""),
			false,		
			bPushed,
			bEnabled,
			false, 
			0,
			(HFONT)pPM->m_FontNormal.GetSafeHandle(),
			true
			);
// 		_pcrbd.m_rcTextMargins.SetRect(0,0,0,0);
// 		_pcrbd.m_clrForceBk = GetBkColor();
// 		_pcrbd.m_clrForceTextDisabled = GetTextColorDisabled();
// 		_pcrbd.m_clrForceTextNormal = GetTextColorNormal();
// 		_pcrbd.m_clrForceTextHover = GetTextColorHover();
// 		_pcrbd.m_clrForceTextPressed = GetTextColorPressed();
		pPM->PaintCheckButton( dc, _pcrbd );
		return;
	} // if( m_bUsePM )
bool bDefaultDrawing = true;
	if(		g_PaintManager.m_UxTheme.IsControlsThemed()
		&&	g_PaintManager.m_UxTheme.OpenThemeData( GetSafeHwnd(), L"BUTTON" ) != NULL
		)
	{
		INT nState = 0;
		INT nPartID = 0;
		nPartID = BP_CHECKBOX;
		if( bCheck )
			nState = CBS_CHECKEDNORMAL;
		else
			nState = CBS_UNCHECKEDNORMAL;
		if( g_PaintManager.m_UxTheme. DrawThemeBackground( dc.GetSafeHdc(), nPartID, nState, &rcCheck, &rcCheck ) == S_OK )
			bDefaultDrawing = false;
		g_PaintManager.m_UxTheme.CloseThemeData( true );
	}
	if( bDefaultDrawing )
	{
		UINT nState = DFCS_BUTTONCHECK;
		if( bCheck )
			nState |= DFCS_CHECKED;
		dc.DrawFrameControl( (RECT *)&rcCheck, DFC_BUTTON, nState );
	} // if( bDefaultDrawing )
}

void CExtCheckComboBox::DrawItem( LPDRAWITEMSTRUCT pDIS )
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
CExtPaintManager * pPM = PmBridge_GetPM();
	ASSERT_VALID( pPM );
CRect rcCheck( pDIS->rcItem ), rcText( pDIS->rcItem );
CString strText;
bool bCheckPresent = true;
bool bFR = ( (pDIS->itemState & (ODS_FOCUS|ODS_SELECTED)) == (ODS_FOCUS|ODS_SELECTED) ) ? true : false;
	if( LONG( pDIS->itemID ) < 0L )
	{
		_UpdateText();
		strText = LPCTSTR(m_strText);
		bCheckPresent = false;
	}
	else
	{
		CSize _sizeBox( 13, 13 );
		_sizeBox.cx = pPM->UiScalingDo( _sizeBox.cx, CExtPaintManager::__EUIST_X );
		_sizeBox.cy = pPM->UiScalingDo( _sizeBox.cy, CExtPaintManager::__EUIST_Y );
		GetLBText( pDIS->itemID, strText );
		TEXTMETRIC _tm;
		::memset( &_tm, 0, sizeof(TEXTMETRIC) );
		::GetTextMetrics( pDIS->hDC, &_tm);
		rcCheck.left ++;
		rcCheck.right = rcCheck.left + _sizeBox.cx;
		rcCheck.bottom = rcCheck.top + _sizeBox.cy;
		rcCheck.OffsetRect( 0, ( rcText.Height() - rcCheck.Height() ) / 2 );
		rcText.left = rcCheck.right + 2;
	}
	if( bCheckPresent )
		OnPaintItemCheck( *CDC::FromHandle( pDIS->hDC ), rcCheck, pDIS->itemID );
	if( pDIS->itemState & ODS_SELECTED )
	{
		::SetBkColor( pDIS->hDC, pPM->GetColor( COLOR_HIGHLIGHT, this ) );
		::SetTextColor( pDIS->hDC, pPM->GetColor( COLOR_HIGHLIGHTTEXT, this ) );
	}
	else
	{
		::SetBkColor( pDIS->hDC, pPM->GetColor( COLOR_WINDOW, this ) );
		::SetTextColor( pDIS->hDC, pPM->GetColor( COLOR_WINDOWTEXT, this ) );
	}
HFONT hFont = OnQueryFont();
	if( hFont == NULL )
		hFont = (HFONT)PmBridge_GetPM()->m_FontNormal.GetSafeHandle();
	ASSERT( hFont != NULL );
HFONT hOldFont = (HFONT)::SelectObject( pDIS->hDC, hFont );
	::ExtTextOut( pDIS->hDC, 0, 0, ETO_OPAQUE, &rcText, 0, 0, 0 );
CString strDraw = _T(" ");
	strDraw += strText;
	CExtRichContentLayout::stat_DrawText( pDIS->hDC, LPCTSTR(strDraw), strText.GetLength() + 1, &rcText, DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX|DT_END_ELLIPSIS, 0 );
	if( bFR )
		::DrawFocusRect( pDIS->hDC, &rcText );
	::SelectObject( pDIS->hDC, hOldFont );
}

void CExtCheckComboBox::MeasureItem( LPMEASUREITEMSTRUCT pMIS )
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
CClientDC dc(this);
HFONT hFont = OnQueryFont();
	if( hFont == NULL )
		hFont = (HFONT)PmBridge_GetPM()->m_FontNormal.GetSafeHandle();
	ASSERT( hFont != NULL );
HFONT hOldFont = (HFONT)dc.SelectObject( hFont );
TEXTMETRIC _tm;
	::memset( &_tm, 0, sizeof(TEXTMETRIC) );
	dc.GetTextMetrics( &_tm );
	pMIS->itemWidth = 20;
	pMIS->itemHeight = _tm.tmHeight + _tm.tmExternalLeading;
	pMIS->itemHeight += 2;
	if( ! m_bHaveItemHeight )
	{
		m_bHaveItemHeight = true;
		SetItemHeight( -1, pMIS->itemHeight );
	}
	dc.SelectObject( hOldFont );
}

LRESULT CExtCheckComboBox::OnGetText( WPARAM wParam, LPARAM lParam )
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	_UpdateText();
	if( lParam == 0 )
		return 0;
	__EXT_MFC_STRCPY( (LPTSTR)lParam, (INT)wParam, LPCTSTR(m_strText) );
LRESULT lr = LRESULT( m_strText.GetLength() );
	return lr;
}

LRESULT CExtCheckComboBox::OnGetTextLength(WPARAM, LPARAM)
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	_UpdateText();
LRESULT lr = LRESULT( m_strText.GetLength() );
	return lr;
}

void CExtCheckComboBox::_UpdateText()
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	if( m_bHaveText )
		return;
INT nIndex, nCount = GetCount();
TCHAR szBuffer[10] = _T("");
	g_ResourceManager->GetLocaleInfo( LOCALE_SLIST, szBuffer, sizeof(szBuffer) );
CExtSafeString strSeparator = szBuffer;
	if( strSeparator.GetLength() == 0 )
		strSeparator = _T(";");
	strSeparator.TrimRight();
	strSeparator += _T(' ');
	m_strText.Empty();
	for( nIndex = 0; nIndex < nCount; nIndex++ )
	{
		if( ! CheckGet( nIndex ) )
			continue;
		CString strItem;
		GetLBText( nIndex, strItem );
		if( ! m_strText.IsEmpty() )
			m_strText += LPCTSTR(strSeparator);
		m_strText += LPCTSTR(strItem);
	}
	m_bHaveText = true;
}

void CExtCheckComboBox::CheckSet(
	INT nIndex,
	bool bCheck // = true
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
INT nResult = SetItemData( nIndex, bCheck ? 1 : 0 );
	if( nResult < 0 )
		return;
	m_bHaveText = false;
	Invalidate( FALSE );
}

bool CExtCheckComboBox::CheckGet( INT nIndex ) const
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return false;
bool bCheck = ( GetItemData( nIndex ) != 0 ) ? true : false;
	return bCheck;
}

void CExtCheckComboBox::CheckAll(
	bool bCheck // = true
	)
{
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return;
INT nIndex, nCount = GetCount();
	for( nIndex = 0; nIndex < nCount; nIndex++ )
		CheckSet( nIndex, bCheck );
}

bool CExtCheckComboBox::_OnCbLbPreWndProc( LRESULT & lResult, UINT message, WPARAM wParam, LPARAM lParam )
{
	lResult; lParam;
	switch( message )
	{
	case WM_CHAR:
	{
		if( wParam == VK_SPACE)
		{
			CExtComboBoxPopupListBox * pLB = OnComboBoxPopupListBoxQuery();
			ASSERT_VALID( pLB );
			INT nIndex = INT( pLB->SendMessage( LB_GETCURSEL ) );
			CRect rcItem;
			pLB->SendMessage( LB_GETITEMRECT, nIndex, (LONG)(VOID *)&rcItem );
			pLB->InvalidateRect( rcItem, FALSE );
			CheckSet( nIndex, ! CheckGet( nIndex ) );
			GetParent()->SendMessage(
				WM_COMMAND,
				MAKELONG( ::__EXT_MFC_GetWindowLong( m_hWnd, GWL_ID ), CBN_SELCHANGE ),
				(LPARAM)m_hWnd
				);
			lResult = 0L;
			return true;
		}
	}
	break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	{
		CExtComboBoxPopupListBox * pLB = OnComboBoxPopupListBoxQuery();
		ASSERT_VALID( pLB );
		CRect rcClient;
		pLB->GetClientRect( &rcClient );
		CPoint pt( LOWORD(lParam), HIWORD(lParam) );
		if( PtInRect( rcClient, pt ) )
		{
			INT nItemHeight = INT( pLB->SendMessage( LB_GETITEMHEIGHT, 0, 0 ) );
			INT nTopIndex   = INT( pLB->SendMessage( LB_GETTOPINDEX, 0, 0 ) );
			INT nIndex = nTopIndex + pt.y / nItemHeight;
			CRect rcItem;
			pLB->SendMessage( LB_GETITEMRECT, nIndex, (LONG)(VOID *)&rcItem );
			if( PtInRect( rcItem, pt ) )
			{
				pLB->InvalidateRect( rcItem, FALSE );
				CheckSet(nIndex, ! CheckGet( nIndex ) );
				GetParent()->SendMessage(
					WM_COMMAND,
					MAKELONG( ::__EXT_MFC_GetWindowLong( m_hWnd, GWL_ID ), CBN_SELCHANGE ),
					(LPARAM)m_hWnd
					);
			}
		}
	}
	break;
	case WM_LBUTTONUP:
		lResult = 0L;
		return true;
	} // switch( message )
	return false;
}

CExtComboBoxPopupListBox * CExtCheckComboBox::OnComboBoxPopupListBoxInstantiate()
{
	ASSERT_VALID( this );
CExtComboBoxPopupListBox * pLB = CExtComboBox::OnComboBoxPopupListBoxInstantiate();
	if( pLB != NULL )
		pLB->m_bInvalidateOnMouseMove = true;
	return pLB;
}


