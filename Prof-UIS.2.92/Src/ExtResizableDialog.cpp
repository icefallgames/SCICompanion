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

#if (!defined __EXT_RESIZABLE_DIALOG_H)
	#include <ExtResizableDialog.h>
#endif

#if (!defined __EXT_RICH_CONTENT_H)
	#include <ExtRichContent.h>
#endif // (!defined __EXT_RICH_CONTENT_H)

#if (!defined __EXT_CONTROLBAR_H)
	#include <ExtControlBar.h>
#endif

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	#if (!defined __EXT_CONTROLBAR_TABBED_FEATURES_H)
		#include "ExtControlBarTabbedFeatures.h"
	#endif // __EXT_CONTROLBAR_TABBED_FEATURES_H
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
	#if (! defined __EXT_TAB_PAGE_CONTAINER_WND_H)
		#include <ExtTabPageContainerWnd.h>
	#endif
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)

#if (!defined __EXT_SCROLLWND_H)
	#include <ExtScrollWnd.h>
#endif 

#if (!defined __EXT_MFC_NO_TAB_CTRL)
	#if (!defined __EXT_TABWND_H)
		#include <ExtTabWnd.h>
	#endif
#endif // (!defined __EXT_MFC_NO_TAB_CTRL)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtResizableDialog

IMPLEMENT_DYNCREATE( CExtResizableDialog, CExtResDlg )

bool CExtResizableDialog::g_bAllowLongFocusJumpFix = true;

CExtResizableDialog::CExtResizableDialog()
	: m_bEnabledControlBarUpdate( false )
	, m_bInConrolBarUpdate( false )
	, m_hWndFocus( NULL )
	, m_bHelperSizingMoving( false )
	, m_bAllowLongFocusJumpFix( g_bAllowLongFocusJumpFix )
{
	m_bShowResizingGripper = true;
}

CExtResizableDialog::CExtResizableDialog(
	UINT nIDTemplate,
	CWnd * pParentWnd
	)
	: CExtWA < CExtWS < __BASEOF_CExtResizableDialog__ > > ( nIDTemplate, pParentWnd )
	, m_bEnabledControlBarUpdate( false )
	, m_bInConrolBarUpdate( false )
	, m_hWndFocus( NULL )
	, m_bHelperSizingMoving( false )
	, m_bAllowLongFocusJumpFix( g_bAllowLongFocusJumpFix )
{
	m_bShowResizingGripper = true;
}

CExtResizableDialog::CExtResizableDialog(
	__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
	CWnd * pParentWnd
	)
	: CExtWA < CExtWS < __BASEOF_CExtResizableDialog__ > > ( lpszTemplateName, pParentWnd )
	, m_bEnabledControlBarUpdate( false )
	, m_bInConrolBarUpdate( false )
	, m_hWndFocus( NULL )
	, m_bHelperSizingMoving( false )
	, m_bAllowLongFocusJumpFix( g_bAllowLongFocusJumpFix )
{
	m_bShowResizingGripper = true;
}

CExtResizableDialog::~CExtResizableDialog()
{
}

bool CExtResizableDialog::_SaveFocusControl()
{
	if( GetSafeHwnd() == NULL )
		return false;
HWND hWndFocus = ::GetFocus();
	if(		hWndFocus != NULL
		&&	::IsChild( m_hWnd, hWndFocus )
		)
	{
		m_hWndFocus = hWndFocus;
		return true;
	}
	return false;
}

__EXT_MFC_SAFE_LPCTSTR CExtResizableDialog::OnQuerySkinnedFontParmName() const
{
	ASSERT_VALID( this );
	return _T("Dialog");
}

BEGIN_MESSAGE_MAP( CExtResizableDialog, CExtResDlg )
	//{{AFX_MSG_MAP(CExtResizableDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtResizableDialog message handlers

void CExtResizableDialog::OnOK()
{
DWORD dwStyle = GetStyle();
	if( (dwStyle & WS_CHILD) == 0 )
		CExtWA < CExtWS < __BASEOF_CExtResizableDialog__ > >::OnOK();
}

void CExtResizableDialog::OnCancel()
{
DWORD dwStyle = GetStyle();
	if( (dwStyle & WS_CHILD) == 0 )
		CExtWA < CExtWS < __BASEOF_CExtResizableDialog__ > >::OnCancel();
}

bool CExtResizableDialog::g_bProcessingLongFocusJump = false;
bool CExtResizableDialog::g_bProcessingLongFocusJumpToPrevDir = false;

HWND CExtResizableDialog::stat_GetNextItemZ( HWND hWndMain, HWND hWndDialog, HWND hWndControl, BOOL bPrevious )
{
LONG nStyle, nStyleEx;
UINT nSearchCode = bPrevious ? GW_HWNDPREV : GW_HWNDNEXT;
HWND hWndRetVal = NULL, hWndFirstChild = NULL;
	if( hWndControl == NULL )
	{
		hWndFirstChild = ::GetWindow( hWndDialog, GW_CHILD );
		if( bPrevious )
			hWndFirstChild = ::GetWindow( hWndFirstChild, GW_HWNDLAST );
	}
	else if( ::IsChild( hWndMain, hWndControl ) )
	{
		hWndFirstChild = ::GetWindow( hWndControl, nSearchCode );
		if( hWndFirstChild == NULL )
		{
			if( ::GetParent( hWndControl ) != hWndMain )
				hWndFirstChild = ::GetWindow( ::GetParent( hWndControl ), nSearchCode );
			else
			{
				if( bPrevious )
					hWndFirstChild = ::GetWindow( hWndControl, GW_HWNDLAST );
				else
					hWndFirstChild = ::GetWindow( hWndControl, GW_HWNDFIRST );
			}
		}
	}
	for( ; hWndFirstChild != NULL ; )
	{
		BOOL bCtrl = FALSE;
		for( ; hWndFirstChild != NULL;  )
		{
			nStyle = (LONG)__EXT_MFC_GetWindowLong( hWndFirstChild, GWL_STYLE );
			nStyleEx = (LONG)__EXT_MFC_GetWindowLong( hWndFirstChild, GWL_EXSTYLE );
			if( ( nStyle & DS_CONTROL || nStyleEx & WS_EX_CONTROLPARENT ) && ( nStyle & WS_VISIBLE ) && (!( nStyle & WS_DISABLED )) )
			{
				bCtrl = TRUE;
				break;
			}
			else if( ( nStyle & WS_TABSTOP ) && ( nStyle & WS_VISIBLE ) && (!( nStyle & WS_DISABLED )) )
				break;
			hWndFirstChild = ::GetWindow( hWndFirstChild, nSearchCode );
		}
		if( hWndFirstChild != NULL )
		{
			if( bCtrl )
				hWndRetVal = stat_GetNextItemZ( hWndMain, hWndFirstChild, NULL, bPrevious );
			else
				hWndRetVal = hWndFirstChild;
		}
		if( hWndRetVal )
			break;
		hWndFirstChild = ::GetWindow( hWndFirstChild, nSearchCode );
	}
	if( hWndRetVal == NULL && hWndControl )
	{
		HWND hWndParent = ::GetParent( hWndControl );
		for( ; hWndParent != NULL; )
		{
			if( hWndParent == hWndMain )
				break;
			hWndRetVal = stat_GetNextItemZ( hWndMain, ::GetParent( hWndParent ), hWndParent , bPrevious );
			if( hWndRetVal )
				break;
			hWndParent = ::GetParent( hWndParent );
		}
		if( hWndRetVal == NULL )
			hWndRetVal = stat_GetNextItemZ(hWndMain,hWndMain,NULL,bPrevious );
	}
	return hWndRetVal;
}

bool CExtResizableDialog::OnHookSpyPreTranslateMessage(
	MSG * pMSG
	)
{
__PROF_UIS_MANAGE_STATE;
	if( m_bHelperSizingMoving )
		return CExtHookSpy::OnHookSpyPreTranslateMessage( pMSG );

	if( pMSG->message == WM_ENTERSIZEMOVE )
		m_bHelperSizingMoving = true;
	else if( pMSG->message == WM_EXITSIZEMOVE )
		m_bHelperSizingMoving = false;

//	if( pMSG->message == WM_ACTIVATE && pMSG->wParam == WA_INACTIVE  )
//		_SaveFocusControl();
	if( pMSG->message == WM_KILLFOCUS )
		_SaveFocusControl();

#if (!defined __EXT_MFC_NO_SCROLLCONAINERWND)
	if(		pMSG->message == WM_SETFOCUS
		&&	GetSafeHwnd() != NULL
		&&	( GetStyle() & (WS_DISABLED|WS_VISIBLE) ) == (WS_VISIBLE)
		)
	{
		HWND hWndFocus = ::GetFocus();
		if( hWndFocus != NULL )
		{
			HWND hWndParentOfFocus = ::GetParent( hWndFocus );
			if( hWndParentOfFocus != NULL )
			{
				CWnd * pWndFocusParent = CWnd::FromHandlePermanent( hWndParentOfFocus );
				if( pWndFocusParent != NULL && pWndFocusParent->IsKindOf( RUNTIME_CLASS(CExtResizableDialog) ) )
				{
					CWnd * pWndParent = pWndFocusParent->GetParent();
					if( pWndParent != NULL )
					{
						CExtScrollContainerWnd * pWndScrollContainer = DYNAMIC_DOWNCAST( CExtScrollContainerWnd, pWndParent );
						if( pWndScrollContainer != NULL )
						{
							CRect rcCheck, rcBounds = pWndScrollContainer->OnSwGetClientRect();
							//rcBounds.OffsetRect( pWndScrollContainer->OnSwGetScrollPos() );
							::GetWindowRect( hWndFocus, &rcCheck );
							pWndScrollContainer->ScreenToClient( &rcCheck );
							rcCheck.InflateRect( ::GetSystemMetrics(SM_CXVSCROLL) + 2, ::GetSystemMetrics(SM_CYHSCROLL) + 2 );
							CPoint ptLT = rcCheck.TopLeft(), ptRB = rcCheck.BottomRight();
							if( ! ( rcBounds.PtInRect( ptLT ) && rcBounds.PtInRect( ptRB ) ) )
							{
								pWndScrollContainer->OnSwEnsurePointAvail( ptRB );
								pWndScrollContainer->OnSwEnsurePointAvail( ptLT );
								pWndScrollContainer->OnSwRecalcLayout( true );
							}
						}
					}
				}
			}
		}
	}
#endif // (!defined __EXT_MFC_NO_SCROLLCONAINERWND)

	return CExtHookSpy::OnHookSpyPreTranslateMessage( pMSG );
}

bool CExtResizableDialog::OnHookSpyKeyMsg(
	MSG * pMSG
	)
{
__PROF_UIS_MANAGE_STATE;
	if( m_bHelperSizingMoving )
		return CExtHookSpy::OnHookSpyKeyMsg( pMSG );
	if(		WM_KEYFIRST <= pMSG->message
		&&	pMSG->message <= WM_KEYLAST
		&&	(	pMSG->wParam == VK_UP
			||	pMSG->wParam == VK_DOWN
			||	pMSG->wParam == VK_LEFT
			||	pMSG->wParam == VK_RIGHT
			||	pMSG->wParam == VK_RETURN
			||	pMSG->wParam == VK_ESCAPE
			||	pMSG->wParam == VK_MENU
			||	(	( HIWORD(pMSG->lParam) & KF_ALTDOWN ) != 0 // CExtPopupMenuWnd::IsKeyPressed( VK_MENU )
				&&	(	( VK_NUMPAD0 <= pMSG->wParam && pMSG->wParam <= VK_NUMPAD9 )
					||	pMSG->wParam == VK_INSERT // 0
					||	pMSG->wParam == VK_END    // 1
					||	pMSG->wParam == VK_DOWN   // 2
					||	pMSG->wParam == VK_NEXT   // 3
					||	pMSG->wParam == VK_LEFT   // 4
					||	pMSG->wParam == 0x0C      // 5, VK_CLEAR
					||	pMSG->wParam == VK_RIGHT  // 8
					||	pMSG->wParam == VK_HOME   // 7
					||	pMSG->wParam == VK_UP     // 8
					||	pMSG->wParam == VK_PRIOR  // 9
					)
				)
			)
		&&	( GetStyle() & (WS_CHILD|WS_VISIBLE) ) == WS_VISIBLE
		&&	PreTranslateMessage( pMSG )
		)
		return true;
	return CExtHookSpy::OnHookSpyKeyMsg( pMSG );
}

LRESULT CExtResizableDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_ENTERSIZEMOVE )
		m_bHelperSizingMoving = true;
	else if( message == WM_EXITSIZEMOVE )
		m_bHelperSizingMoving = false;
	else if( message == WM_SETFOCUS && GetSafeHwnd() != NULL)
	{
		if(		m_bAllowLongFocusJumpFix
			&&	g_bProcessingLongFocusJump
			)
		{
			HWND hWnd = stat_GetNextItemZ( m_hWnd, m_hWnd, NULL, g_bProcessingLongFocusJumpToPrevDir ? TRUE : FALSE );
			if( hWnd != NULL )
			{
				//CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
				::SetFocus( hWnd );
				//pWnd;
			}
			return 0;
		}
		if(		m_hWndFocus == NULL
			||	( ! ::IsWindow( m_hWndFocus ) )
			||	( ! ::IsChild( m_hWnd, m_hWndFocus ) )
			||	::GetFocus() == m_hWndFocus
			)
			m_hWndFocus = NULL;
		else
		{
			::SetFocus( m_hWndFocus );
			m_hWndFocus = NULL;
			return 0;
		}
	}
	else if( message == WM_DESTROY )
		m_bEnabledControlBarUpdate = false;

bool bUpdateBarsOnThisMsg = false;
	if( m_bEnabledControlBarUpdate )
	{
		CWinApp * pApp = AfxGetApp();
			ASSERT( pApp != NULL );
		BOOL bIdleMsg = pApp->IsIdleMessage(
#if _MFC_VER < 0x700
			&pApp->m_msgCur
#else
			&(::AfxGetThreadState()->m_msgCur)
#endif
			);
		if( bIdleMsg )
			bUpdateBarsOnThisMsg = true;
	}

HWND hWndThis = m_hWnd;
LRESULT lResult =
		CExtWA < CExtWS < __BASEOF_CExtResizableDialog__ > >::WindowProc(message, wParam, lParam);

	if(		hWndThis == NULL
		||	( ! ::IsWindow(hWndThis) )
		)
		bUpdateBarsOnThisMsg = false;

	if(		bUpdateBarsOnThisMsg
		&&	( ! m_bInConrolBarUpdate )
		)
	{
		m_bInConrolBarUpdate = true;
		CExtControlBar::DoCustomModeUpdateControlBars( this );
		m_bInConrolBarUpdate = false;
	}

	return lResult;
}

void CExtResizableDialog::PreSubclassWindow() 
{
	m_bHelperSizingMoving = false;
	CExtWA < CExtWS < __BASEOF_CExtResizableDialog__ > >::PreSubclassWindow();
	HookSpyRegister( __EHSEF_MOUSE_ALL_WITHOUT_WHEEL|__EHSEF_WND_PROC_IN|__EHSEF_PRE_TRANSLATION );
	m_bEnabledControlBarUpdate = true;
	EnableToolTips();
}

void CExtResizableDialog::PostNcDestroy()
{
	m_bHelperSizingMoving = false;
	HookSpyUnregister();
	CExtWA < CExtWS < __BASEOF_CExtResizableDialog__ > >::PostNcDestroy();
}

BOOL CExtResizableDialog::PreTranslateMessage(MSG* pMsg) 
{
	if(		WM_KEYFIRST <= pMsg->message
		&&	pMsg->message <= WM_KEYLAST
		&&	GetSafeHwnd() != NULL
		&&	( pMsg->hwnd == m_hWnd || ::IsChild( m_hWnd, pMsg->hwnd ) )
		&&	(GetStyle()&(WS_VISIBLE|WS_DISABLED)) == WS_VISIBLE
		)
	{ // pre translating keyboard input (if enabled/visible dialog)
		if( pMsg->wParam == VK_TAB )
		{
			if( m_bAllowLongFocusJumpFix )
			{
				if( pMsg->message == WM_KEYDOWN )
				{
					HWND hWnd = ::GetFocus();
					if( hWnd != NULL )
					{
						bool bWantsTab = ( ( ::SendMessage( hWnd, WM_GETDLGCODE, VK_TAB, 0 ) & DLGC_WANTTAB ) != 0 ) ? true : false;
						if( bWantsTab )
						{
							CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
							if(		pWnd != NULL
								&&	(	pWnd->IsKindOf( RUNTIME_CLASS(CDialog) )
#if (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
									||	pWnd->IsKindOf( RUNTIME_CLASS(CExtTabPageContainerWnd) )
#endif // (!defined __EXT_MFC_NO_TAB_PAGECONTAINER_CTRL)
									)
								)
								bWantsTab = false;
						} // if( bWantsTab )
						if( ! bWantsTab )
						{
							HWND hWndMain = m_hWnd;
							if( ( __EXT_MFC_GetWindowLong( hWndMain, GWL_STYLE ) & WS_CHILD ) != 0 )
								hWndMain = ::GetParent( m_hWnd );
							for( ; ( __EXT_MFC_GetWindowLong( hWndMain, GWL_STYLE ) & WS_CHILD ) != 0 ; )
								hWndMain = ::GetParent( hWndMain );
							if( hWndMain != NULL )
							{
								BOOL bPrevious = CExtPopupMenuWnd::IsKeyPressed( VK_SHIFT );
								hWnd = stat_GetNextItemZ( hWndMain, m_hWnd, hWnd, bPrevious );
								//hWnd = ::GetNextDlgTabItem( m_hWnd, hWnd, CExtPopupMenuWnd::IsKeyPressed( VK_SHIFT ) );
								if( hWnd != NULL )
								{
									//CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
									g_bProcessingLongFocusJump = true;
									g_bProcessingLongFocusJumpToPrevDir = bPrevious ? true : false;
									::SetFocus( hWnd );
									if( ::SendMessage( hWnd, WM_GETDLGCODE, WPARAM(VK_TAB), LPARAM(pMsg) ) )
										::SendMessage( hWnd, EM_SETSEL, WPARAM(0L), LPARAM(-1L) );
									g_bProcessingLongFocusJump = false;
									//pWnd;
								}
							}
						} // if( ! bWantsTab )
						else
							return FALSE;
					} // if( hWnd != NULL )
				} // if( pMsg->message == WM_KEYDOWN )
				return TRUE;
			} // if( m_bAllowLongFocusJumpFix )
		} // if( pMsg->wParam == VK_TAB )
		else
		{
			if( (GetStyle()&(WS_VISIBLE|WS_CHILD)) == (WS_VISIBLE|WS_CHILD) )
			{ // if child/visible dialog
				HWND hWndParent = ::GetParent( m_hWnd );
				CWnd * pWndParentPermanent = CWnd::FromHandlePermanent( hWndParent );
				if(		pWndParentPermanent != NULL
					&&	(	pWndParentPermanent->IsKindOf( RUNTIME_CLASS(CExtControlBar) )
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
						||	pWndParentPermanent->IsKindOf( RUNTIME_CLASS(CExtDynAutoHideSlider) )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
						)
					&&	pWndParentPermanent->PreTranslateMessage(pMsg)
					)
					return TRUE;
			} // if child/visible dialog
		} // else from if( pMsg->wParam == VK_TAB )

		HWND hWndFocus = ::GetFocus();
		if(		hWndFocus != NULL
			&&	::GetParent( hWndFocus ) == m_hWnd
			//&&	( ::SendMessage( hWndFocus, WM_GETDLGCODE, 0L, 0L ) & (DLGC_WANTCHARS|DLGC_WANTALLKEYS) == 0 )
			)
		{
			bool bProcessKey = false;
			BOOL bAlt = HIWORD(pMsg->lParam) & KF_ALTDOWN;
			LRESULT lRDC = ::SendMessage( hWndFocus, WM_GETDLGCODE, pMsg->wParam, 0L );
			if(		( lRDC & DLGC_WANTALLKEYS ) == 0
				&&	(	( lRDC & DLGC_WANTCHARS ) == 0
					||	bAlt
					)
				)
			{
				bProcessKey = true;
			}
			if( bProcessKey )
			{
				//BOOL bKeyUp = ( pMsg->message == WM_KEYUP || pMsg->message == WM_SYSKEYUP) ? TRUE : FALSE;
				BOOL bCtrl = GetKeyState(VK_CONTROL) & 0x80000000;
				BOOL bShift = GetKeyState(VK_SHIFT) & 0x80000000;
				if( ! ( bCtrl || bShift ) )
				{
					TCHAR vkTCHAR = (TCHAR)pMsg->wParam;
					BYTE lpKeyState[256];
					::GetKeyboardState( lpKeyState );
					UINT wScanCode = ::MapVirtualKey( vkTCHAR, 0 );
					HKL hKeyboardLayout =
						::GetKeyboardLayout(
							( ::AfxGetThread() ) -> m_nThreadID
							);
#if (defined _UNICODE)
					TCHAR szChar[2] = { _T('\0'), _T('\0') };
					::ToUnicodeEx(
						vkTCHAR,
						wScanCode,
						lpKeyState,
						szChar, 1,
						1,
						hKeyboardLayout
						);
#else
					WORD nMapped = 0;
					::ToAsciiEx(
						vkTCHAR,
						wScanCode,
						lpKeyState,
						&nMapped,
						1,
						hKeyboardLayout
						);
					TCHAR szChar[2] = { (TCHAR)nMapped, _T('\0') };
#endif
					::CharUpper( szChar );
					if( szChar[0] != 0 )
					{
						HWND hWnd = ::GetWindow( m_hWnd, GW_CHILD );
						for( ; hWnd != NULL; hWnd = ::GetWindow( hWnd, GW_HWNDNEXT ) )
						{
							CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
							if( pWnd == NULL )
								continue;
							if( (pWnd->GetStyle()&(WS_VISIBLE|WS_DISABLED)) != WS_VISIBLE )
								continue;
							CExtButton * pExtButton = DYNAMIC_DOWNCAST( CExtButton, pWnd );
							if( pExtButton == NULL )
								continue;
							if(		pExtButton->m_bQuickActivationEnabled
								&&	pExtButton->_QuickActivationCheck( DWORD(vkTCHAR) )
								)
							{
								if(		pMsg->message == WM_KEYDOWN
									||	pMsg->message == WM_SYSKEYDOWN
									)
								{
									if( hWndFocus != pExtButton->m_hWnd )
										pExtButton->SetFocus();
								}
								if(		pMsg->message == WM_KEYUP
									||	pMsg->message == WM_SYSKEYUP
									)
								{
									if( pExtButton->PreTranslateMessage( pMsg ) )
										return TRUE;
								}
								//break;
								return TRUE;
							}
						}
					}
				} // if( ! ( bCtrl || bShift ) )
			} // if( bProcessKey )
		} // if( hWndFocus != NULL && ::GetParent( hWndFocus ) == m_hWnd ...
	} // pre translating keyboard input (if enabled/visible dialog)

	if(		( GetStyle() & WS_VISIBLE ) != 0
		&&	CExtControlBar::DoCustomModePreTranslateMessage( this, pMsg )
		)
		return TRUE;

	if( pMsg->message == WM_LBUTTONDOWN )
		return FALSE;

	return CExtWA < CExtWS < __BASEOF_CExtResizableDialog__ > >::PreTranslateMessage(pMsg);
}

//////////////////////////////////////////////////////////////////////////
// CExtResizablePropertyPage

IMPLEMENT_DYNCREATE( CExtResizablePropertyPage, CExtResPP )

CExtResizablePropertyPage::CExtResizablePropertyPage()
	: m_bEnabledControlBarUpdate( false )
	, m_bInConrolBarUpdate( false )
{
	m_bShowResizingGripper = false;
}

CExtResizablePropertyPage::CExtResizablePropertyPage(
	UINT nIDTemplate,
	UINT nIDCaption // = 0 
	)
	: CExtWA < CExtWS < __BASEOF_CExtResizablePropertyPage__ > > ( nIDTemplate, nIDCaption )
	, m_bEnabledControlBarUpdate( false )
	, m_bInConrolBarUpdate( false )
{
	m_bShowResizingGripper = false;
}

CExtResizablePropertyPage::CExtResizablePropertyPage(
	__EXT_MFC_SAFE_LPCTSTR lpszTemplateName,
	UINT nIDCaption // = 0 
	)
	: CExtWA < CExtWS < __BASEOF_CExtResizablePropertyPage__ > > ( lpszTemplateName, nIDCaption )
	, m_bEnabledControlBarUpdate( false )
	, m_bInConrolBarUpdate( false )
{
	m_bShowResizingGripper = false;
}

CExtResizablePropertyPage::~CExtResizablePropertyPage()
{
}

__EXT_MFC_SAFE_LPCTSTR CExtResizablePropertyPage::OnQuerySkinnedFontParmName() const
{
	ASSERT_VALID( this );
	return _T("Dialog");
}

BEGIN_MESSAGE_MAP( CExtResizablePropertyPage, CExtResPP )
	//{{AFX_MSG_MAP(CExtResizablePropertyPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtResizablePropertyPage message handlers

void CExtResizablePropertyPage::OnOK()
{
DWORD dwStyle = GetStyle();
	if( (dwStyle & WS_CHILD) == 0 )
		CExtWA < CExtWS < __BASEOF_CExtResizablePropertyPage__ > >::OnOK();
}

void CExtResizablePropertyPage::OnCancel()
{
DWORD dwStyle = GetStyle();
	if( (dwStyle & WS_CHILD) == 0 )
		CExtWA < CExtWS < __BASEOF_CExtResizablePropertyPage__ > >::OnCancel();
}

bool CExtResizablePropertyPage::OnHookWndMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	UINT nMessage,
	WPARAM & wParam,
	LPARAM & lParam
	)
{
	__PROF_UIS_MANAGE_STATE;
	if(		GetSafeHwnd() != NULL
		&&	hWndHooked == m_hWnd
		)
	{
		//if( (m_psh.dwFlags&(__EXT_MFC_PSH_WIZARD97_MASK)) != 0 )
		{
			switch( nMessage )
			{
			case WM_ERASEBKGND:
				{
					ASSERT_VALID( this );

					// look through the message map to see 
					// if this message is handled by user
					const AFX_MSGMAP_ENTRY * lpEntry = _FindMessageMapHandler( nMessage, 0, 0 );
					if( lpEntry != NULL )
					{
						// invoke the user defined message handler
						union 
						{
							AFX_PMSG pfn;
							BOOL (AFX_MSG_CALL CWnd::*pfn_bD)( CDC* );
						} mmf;
						mmf.pfn = lpEntry->pfn;
						LRESULT lResult =  (this->*mmf.pfn_bD)( CDC::FromHandle( ( HDC ) wParam ) );
						return lResult > 0L ? true : false;
					} // if( lpEntry != NULL )

					HDC hDC = reinterpret_cast < HDC > (wParam);
					ASSERT( hDC != NULL );
					HWND hWnd = ::WindowFromDC( hDC );
					if(		hWnd != NULL
						&&	hWnd != m_hWnd
						)
						return 
							CExtHookSink::OnHookWndMsg(
								lResult,
								hWndHooked,
								nMessage,
								wParam,
								lParam
								);

					if(		(GetStyle() & WS_CLIPCHILDREN) != 0
						&&	PmBridge_GetPM()->GetCb2DbTransparentMode(this)
						)
						return (!0L);

					CExtPaintManager::stat_ExcludeChildAreas(
						hDC,
						GetSafeHwnd(),
						CExtPaintManager::stat_DefExcludeChildAreaCallback
						);

					return (!0L);

				} // case WM_ERASEBKGND
					
			case WM_PAINT:
				{
					ASSERT_VALID( this );

					bool bDlg = IsKindOf(RUNTIME_CLASS(CDialog)) ? true : false;
					if( !bDlg )
					{
						TCHAR strClassName[ _MAX_PATH + 1 ] = _T("");
						::GetClassName( m_hWnd, strClassName, _MAX_PATH );
						__EXT_MFC_STRLWR( strClassName, _MAX_PATH + 1 );
						if( _tcsstr( strClassName, _T("#") ) != NULL )
							bDlg = true;
					}

					// look through the message map to see 
					// if this message is handled by user
					const AFX_MSGMAP_ENTRY * lpEntry = _FindMessageMapHandler( nMessage, 0, 0 );
					
					bool bPainted = false;
					if( !bDlg ) 
					{
						if( lpEntry != NULL )
						{
							// invoke the user defined message handler
							union 
							{
								AFX_PMSG pfn;
								void (AFX_MSG_CALL CCmdTarget::*pfn_COMMAND)();
							} mmf;
							mmf.pfn = lpEntry->pfn;
							(this->*mmf.pfn_COMMAND)();
							// return 0L;
							bPainted = true;
						} // if( lpEntry != NULL )
					}

					if( !bPainted )
					{
						CPaintDC dc( this );
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
						{
							CRect rcClient;
							GetClientRect( &rcClient );
							dc.FillSolidRect(
								&rcClient,
								clrBackground != COLORREF(-1L)
									? clrBackground
									: PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT, this )
								);	
						}

#if _MFC_VER >= 0x700
					if( (m_psp.dwFlags&PSP_HIDEHEADER) != 0 )
					{
						CWnd * pWnd = GetParent();
						ASSERT_VALID( pWnd );
						ASSERT( pWnd->GetSafeHwnd() != NULL );
						CExtResizablePropertySheet * pRPS =
							DYNAMIC_DOWNCAST( CExtResizablePropertySheet, pWnd );
						if(		pRPS != NULL
							&&	(pRPS->m_psh.dwFlags&PSH_HEADER) != 0
							&&	pRPS->m_psh.pszbmWatermark != NULL
							)
						{
							CExtBitmap _bmp;
							if( g_ResourceManager->LoadBitmap(
									_bmp,
									pRPS->m_psh.pszbmWatermark
									)
								)
							{
								CSize _size = _bmp.GetSize();
 								CRect rcDraw( 0, 0, _size.cx, _size.cy );
								_bmp.Draw( dc.m_hDC, rcDraw );
							}
						}
					} // if( (m_psp.dwFlags&PSP_HIDEHEADER) != 0 )
#endif // #if _MFC_VER >= 0x700

						PmBridge_GetPM()->OnPaintSessionComplete( this );
					}

					if( bDlg ) 
					{
						if( lpEntry != NULL )
						{
							// invoke the user defined message handler
							union 
							{
								AFX_PMSG pfn;
								void (AFX_MSG_CALL CCmdTarget::*pfn_COMMAND)();
							} mmf;
							mmf.pfn = lpEntry->pfn;
							(this->*mmf.pfn_COMMAND)();
							return 0L;
						} // if( lpEntry != NULL )
					}

					return TRUE;

				} // case WM_PAINT

			} // switch( nMessage )
		} // if( (m_psh.dwFlags&(__EXT_MFC_PSH_WIZARD97_MASK)) != 0 )
	}
	return
		CExtHookSink::OnHookWndMsg(
			lResult,
			hWndHooked,
			nMessage,
			wParam,
			lParam
			);
}

LRESULT CExtResizablePropertyPage::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
//TRACE1( "--CExtResizablePropertyPage::WindowProc(0x%08X)\n", message );
	if( message == WM_DESTROY )
		m_bEnabledControlBarUpdate = false;

bool bUpdateBarsOnThisMsg = false;
	if( m_bEnabledControlBarUpdate )
	{
		CWinApp * pApp = AfxGetApp();
			ASSERT( pApp != NULL );
		BOOL bIdleMsg = pApp->IsIdleMessage(
#if _MFC_VER < 0x700
			&pApp->m_msgCur
#else
			&(::AfxGetThreadState()->m_msgCur)
#endif
			);
		if( bIdleMsg )
			bUpdateBarsOnThisMsg = true;
	}


LRESULT lResult =
		CExtWA < CExtWS < __BASEOF_CExtResizablePropertyPage__ > >::WindowProc(message, wParam, lParam);

	switch( message )
	{
/*
#if _MFC_VER >= 0x700
	case WM_PAINT:
		{
			CWnd * pWnd = GetParent();
			ASSERT_VALID( pWnd );
			ASSERT( pWnd->GetSafeHwnd() != NULL );
			CExtResizablePropertySheet * pRPS =
				DYNAMIC_DOWNCAST( CExtResizablePropertySheet, pWnd );
			if(		pRPS != NULL
				&&	(pRPS->m_psh.dwFlags&PSH_HEADER) != 0
				&&	pRPS->m_psh.pszbmHeader != NULL
				)
			{
				HBITMAP hBmpHeader =
					g_ResourceManager->LoadBitmap(
						pRPS->m_psh.pszbmHeader
						);
				if( hBmpHeader != NULL )
				{
					CExtBitmap bmp;
					if( bmp.FromBitmap( hBmpHeader ) )
					{
						CSize _size = bmp.GetSize();
 						CRect rcDraw( 0, 0, _size.cx, _size.cy );
						CClientDC dc( this );
						bmp.Draw( dc.m_hDC, rcDraw );
					}
					::DeleteObject( hBmpHeader );
				}
			}
		}
		break;
#endif
*/
	//case WM_NCCREATE:
	case WM_INITDIALOG:
		{
			CWnd * pWndParent = GetParent();
			ASSERT_VALID( pWndParent );
			CExtResizablePropertySheet * pRPS =
				DYNAMIC_DOWNCAST( CExtResizablePropertySheet, pWndParent );
			if(		pRPS != NULL
				&&	pRPS->GetActiveIndex() != 0
				)
			{
				pRPS->_SyncActivePage( this );
			}
			::PostMessage( m_hWnd, (WM_USER+10), 0, 0 );
		}
		break;
	case (WM_USER+10):
			SetupHookWndSink( m_hWnd );
		break;
	case WM_DESTROY:
		RemoveAllWndHooks();
		break;
	case WM_SHOWWINDOW:
	case WM_WINDOWPOSCHANGED:
			if( GetStyle() & WS_VISIBLE )
			{
				CWnd * pWndParent = GetParent();
				if(		pWndParent != NULL
					&&	pWndParent->IsKindOf(
							RUNTIME_CLASS(CExtResizablePropertySheet)
							)
					)
				{
					//((CExtResizablePropertySheet*)pWndParent)->ArrangeLayout();
					((CExtResizablePropertySheet*)pWndParent)->_SyncActivePage();
				}
				ArrangeLayout();
			}
		break;
	} // switch( message )

	if( bUpdateBarsOnThisMsg && (!m_bInConrolBarUpdate) )
	{
		m_bInConrolBarUpdate = true;
		CExtControlBar::DoCustomModeUpdateControlBars( this );
		m_bInConrolBarUpdate = false;
	}

	return lResult;
}

void CExtResizablePropertyPage::PreSubclassWindow() 
{
	CExtWA < CExtWS < __BASEOF_CExtResizablePropertyPage__ > >::PreSubclassWindow();
	m_bEnabledControlBarUpdate = true;
	EnableToolTips();
}

BOOL CExtResizablePropertyPage::PreTranslateMessage(MSG* pMsg) 
{
	if(		( GetStyle() & WS_VISIBLE ) != 0
		&&	CExtControlBar::DoCustomModePreTranslateMessage( this, pMsg )
		)
		return TRUE;
	return CExtWA < CExtWS < __BASEOF_CExtResizablePropertyPage__ > >::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CExtResizablePropertySheet

IMPLEMENT_DYNCREATE( CExtResizablePropertySheet, CExtResPS )

CExtResizablePropertySheet::CExtResizablePropertySheet()
	: m_bSheetInitialized( false )
	//, m_bAllowTabSwitchingLoop( false )
{
	m_bShowResizingGripper = true;
}

CExtResizablePropertySheet::CExtResizablePropertySheet(
	UINT nIDCaption,
	CWnd *pParentWnd, // = NULL
	UINT iSelectPage  // = 0
	)
	: CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > > ( nIDCaption, pParentWnd, iSelectPage  )
	, m_bSheetInitialized( false )
	//, m_bAllowTabSwitchingLoop( false )
{
	m_bShowResizingGripper = true;
}

CExtResizablePropertySheet::CExtResizablePropertySheet(
	__EXT_MFC_SAFE_LPCTSTR pszCaption,
	CWnd *pParentWnd, // = NULL
	UINT iSelectPage  // = 0
	)
	: CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > > ( pszCaption, pParentWnd, iSelectPage  )
	, m_bSheetInitialized( false )
	//, m_bAllowTabSwitchingLoop( false )
{
	m_bShowResizingGripper = true;
}

CExtResizablePropertySheet::~CExtResizablePropertySheet()
{
}

__EXT_MFC_SAFE_LPCTSTR CExtResizablePropertySheet::OnQuerySkinnedFontParmName() const
{
	ASSERT_VALID( this );
	return _T("Dialog");
}

BEGIN_MESSAGE_MAP( CExtResizablePropertySheet, CExtResPS )
	//{{AFX_MSG_MAP(CExtResizablePropertySheet)
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtResizablePropertySheet message handlers

void CExtResizablePropertySheet::PreSubclassWindow() 
{
	CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > >::PreSubclassWindow();
	EnableToolTips();
}

void CExtResizablePropertySheet::_SyncActivePage(
	CPropertyPage * pActivePage // = NULL
	)
{
CTabCtrl * pTabCtrl = NULL;
	if( (m_psh.dwFlags&(__EXT_MFC_PSH_WIZARD97_MASK)) == 0 && ( (m_psh.dwFlags&PSH_WIZARD) == 0 ) )
		pTabCtrl = GetTabControl();
	if( pTabCtrl != NULL )
	{
		RECT rcMove;
		pTabCtrl->GetWindowRect( &rcMove );
		ScreenToClient( &rcMove );
		pTabCtrl->AdjustRect( FALSE, &rcMove );
		CWnd * pWnd = GetWindow( GW_CHILD );
		for( ; pWnd != NULL; pWnd = pWnd->GetWindow( GW_HWNDNEXT ) )
		{
			if( ! pWnd->IsKindOf( RUNTIME_CLASS( CPropertyPage ) ) )
				continue;
			RemoveAnchor( pWnd->m_hWnd );
			pWnd->MoveWindow( &rcMove );
			AddAnchor( pWnd->m_hWnd, __RDA_LT, __RDA_RB );
		}
	}
	else
	{
		CPropertyPage * pWnd = ( pActivePage->GetSafeHwnd() != NULL ) ? pActivePage : CExtResPS::GetActivePage();
		if( pWnd->GetSafeHwnd() != NULL )
		{
			RDI_t _val;
			if( ! m_mapRDI.Lookup( pWnd->GetSafeHwnd(), _val ) )
			{
				POSITION pos;
				for( pos = m_mapRDI.GetStartPosition(); pos != NULL; )
				{
					HWND _hWnd;
					RDI_t _rdi;
					m_mapRDI.GetNextAssoc( pos, _hWnd, _rdi );
					ASSERT( _hWnd != NULL );
					if( ! ::IsWindow( _hWnd ) )
						continue;
					CWnd * _pWnd = CWnd::FromHandlePermanent( _hWnd );
					if( _pWnd == NULL )
						continue;
					if( _pWnd->IsKindOf(RUNTIME_CLASS(CPropertyPage)) )
					{
						RECT rcMove;
						_pWnd->GetWindowRect( &rcMove );
						ScreenToClient( &rcMove );
						//RemoveAnchor( pWnd->m_hWnd );
						pWnd->MoveWindow( &rcMove );
						//AddAnchor( pWnd->m_hWnd, __RDA_LT, __RDA_RB );
						break;
					} // if( _pWnd->IsKindOf(RUNTIME_CLASS(CPropertyPage)) )
				} // for( POSITION pos = m_mapRDI.GetStartPosition(); pos != NULL; )
				AddAnchor( pWnd->GetSafeHwnd(), __RDA_LT, __RDA_RB );
			}
		}
	}
	ArrangeLayout();
}

void CExtResizablePropertySheet::OnRenderWizardWatermarkArea(
	CDC & dc,
	const RECT & rcWaterMark,
	const CExtBitmap & _bitmapWatermark // can be empty
	)
{
	__PROF_UIS_MANAGE_STATE;
	ASSERT_VALID( this );
	//
	// STEP 1: draw tiled watermark bitmap
	//
	if( ! _bitmapWatermark.IsEmpty() )
	{
		CSize _size = _bitmapWatermark.GetSize();
		CRect rcWalkStart(
			rcWaterMark.left,
			rcWaterMark.top,
			rcWaterMark.left + _size.cx,
			rcWaterMark.top + _size.cy
			);
		for( ; rcWalkStart.top < rcWaterMark.bottom; rcWalkStart.OffsetRect( 0, _size.cy ) )
		{
			CRect rcWalk = rcWalkStart;
			for( ; rcWalk.left < rcWaterMark.right; rcWalk.OffsetRect( _size.cx, 0 ) )
			{
				_bitmapWatermark.Draw(
					dc.GetSafeHdc(),
					rcWalk.left,
					rcWalk.top,
					_size.cx,
					_size.cy
					);
			}
		}
	} // if( ! _bitmapWatermark.IsEmpty() )
#if _MFC_VER >= 0x700
	//
	// STEP 2: draw title and subtitle
	//
int nActiveIndex = GetActiveIndex();
CPropertyPage * pPP = ( nActiveIndex >= 0 ) ? GetPage( nActiveIndex ) : NULL;
	if( pPP != NULL )
	{
		LPCTSTR strTitle = pPP->m_psp.pszHeaderTitle;
		int nLenTitle = 0;
		if( strTitle != NULL )
		{
			nLenTitle = int( _tcslen( strTitle ) );
			if( nLenTitle == 0 )
				strTitle = NULL;
		}
		LPCTSTR strSubTitle = pPP->m_psp.pszHeaderSubTitle;
		int nLenSubTitle = 0;
		if( strSubTitle != NULL )
		{
			nLenSubTitle = int( _tcslen( strSubTitle ) );
			if( nLenSubTitle == 0 )
				strSubTitle = NULL;
		}
		if( strTitle != NULL && strSubTitle != NULL )
		{
			int nOldBkMode = dc.SetBkMode( TRANSPARENT );
			COLORREF clrOldTextColor =
				dc.SetTextColor( PmBridge_GetPM()->GetColor( COLOR_BTNTEXT ) );
			CFont * pFontTitle = &(PmBridge_GetPM()->m_FontBold);
			CFont * pFontSubTitle = &(PmBridge_GetPM()->m_FontNormal);
			TEXTMETRIC _tmTitle, _tmSubTitle;
			::memset( &_tmTitle, 0, sizeof(TEXTMETRIC) );
			::memset( &_tmSubTitle, 0, sizeof(TEXTMETRIC) );
			CFont * pOldFont = dc.SelectObject( pFontTitle );
			dc.GetTextMetrics( &_tmTitle );
			dc.SelectObject( pFontSubTitle );
			dc.GetTextMetrics( &_tmSubTitle );
			static const int nPxDistanceBetweenTitleAndSubTitle = 4;
			static const int nPxLeftOffsetTitle = 23;
			static const int nPxLeftOffsetSubTitle = 22;
			int nSummaryHieght =
					int(_tmTitle.tmHeight)
				+ nPxDistanceBetweenTitleAndSubTitle
				+ int(_tmSubTitle.tmHeight);
			CRect rcTitle(
				rcWaterMark.left + nPxLeftOffsetTitle,
				rcWaterMark.top,
				rcWaterMark.right,
				rcWaterMark.top + _tmTitle.tmHeight
				);
			rcTitle.OffsetRect(
				0,
				( rcWaterMark.bottom - rcWaterMark.top - nSummaryHieght ) / 2
				);
			CRect rcSubTitle(
				rcTitle.left + nPxLeftOffsetSubTitle,
				rcTitle.bottom + nPxDistanceBetweenTitleAndSubTitle,
				rcWaterMark.right,
				rcTitle.bottom + nPxDistanceBetweenTitleAndSubTitle + _tmSubTitle.tmHeight
				);
			if( strTitle != NULL )
			{
				ASSERT( nLenTitle > 0 );
				dc.SelectObject( pFontTitle );
				CExtRichContentLayout::stat_DrawText(
					dc.m_hDC,
					strTitle, nLenTitle,
					&rcTitle,
					DT_SINGLELINE|DT_LEFT|DT_TOP|DT_END_ELLIPSIS, 0
					);
			} // if( strTitle != NULL )
			if( strSubTitle != NULL )
			{
				ASSERT( nLenSubTitle > 0 );
				dc.SelectObject( pFontSubTitle );
				CExtRichContentLayout::stat_DrawText(
					dc.m_hDC,
					strSubTitle, nLenSubTitle,
					&rcSubTitle,
					DT_SINGLELINE|DT_LEFT|DT_TOP|DT_END_ELLIPSIS, 0
					);
			} // if( strSubTitle != NULL )
			dc.SelectObject( pOldFont );
			dc.SetBkMode( nOldBkMode );
			dc.SetTextColor( clrOldTextColor );
		} // if( strTitle != NULL && strSubTitle != NULL )
	} // if( pPP != NULL )
#endif // #if _MFC_VER >= 0x700
}

bool CExtResizablePropertySheet::OnHookWndMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	UINT nMessage,
	WPARAM & wParam,
	LPARAM & lParam
	)
{
	__PROF_UIS_MANAGE_STATE;
	if(		GetSafeHwnd() != NULL
		&&	hWndHooked == m_hWnd
		)
	{
		if( (m_psh.dwFlags&(__EXT_MFC_PSH_WIZARD97_MASK)) != 0 )
		{
			switch( nMessage )
			{
			case WM_ERASEBKGND:
				{
					ASSERT_VALID( this );

					// look through the message map to see 
					// if this message is handled by user
					const AFX_MSGMAP_ENTRY * lpEntry = _FindMessageMapHandler( nMessage, 0, 0 );
					if( lpEntry != NULL )
					{
						// invoke the user defined message handler
						union 
						{
							AFX_PMSG pfn;
							BOOL (AFX_MSG_CALL CWnd::*pfn_bD)( CDC* );
						} mmf;
						mmf.pfn = lpEntry->pfn;
						LRESULT lResult = 
							(this->*mmf.pfn_bD)( CDC::FromHandle( ( HDC ) wParam ) );
						return lResult > 0L ? true : false;
					} // if( lpEntry != NULL )

					if(		(GetStyle() & WS_CLIPCHILDREN) != 0
						&&	PmBridge_GetPM()->GetCb2DbTransparentMode(this)
						)
						return (!0L);

					HDC hDC = reinterpret_cast < HDC > (wParam);
					ASSERT( hDC != NULL );
					CExtPaintManager::stat_ExcludeChildAreas(
						hDC,
						GetSafeHwnd(),
						CExtPaintManager::stat_DefExcludeChildAreaCallback
						);

					return (!0L);

				} // case WM_ERASEBKGND
			
			case WM_PAINT:
				{
					ASSERT_VALID( this );

					bool bDlg = IsKindOf(RUNTIME_CLASS(CDialog)) ? true : false;
					if( !bDlg )
					{
						TCHAR strClassName[ _MAX_PATH + 1 ] = _T("");
						::GetClassName( m_hWnd, strClassName, _MAX_PATH );
						__EXT_MFC_STRLWR( strClassName, _MAX_PATH + 1 );
						if( _tcsstr( strClassName, _T("#") ) != NULL )
							bDlg = true;
					}

					// look through the message map to see 
					// if this message is handled by user
					const AFX_MSGMAP_ENTRY * lpEntry = _FindMessageMapHandler( nMessage, 0, 0 );
					
					bool bPainted = false;
					if( !bDlg ) 
					{
						if( lpEntry != NULL )
						{
							// invoke the user defined message handler
							union 
							{
								AFX_PMSG pfn;
								void (AFX_MSG_CALL CCmdTarget::*pfn_COMMAND)();
							} mmf;
							mmf.pfn = lpEntry->pfn;
							(this->*mmf.pfn_COMMAND)();
							// return 0L;
							bPainted = true;
						} // if( lpEntry != NULL )
					}

					if( !bPainted )
					{
						CPaintDC dc( this );
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
						
						CRect rcClient;
						GetClientRect( &rcClient );

						if( ! bTransparent )
						{
							dc.FillSolidRect(
								&rcClient,
								clrBackground != COLORREF(-1L)
									? clrBackground
									: PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT, this )
								);	
						}

						CExtBitmap _bitmapWatermark;
#if _MFC_VER >= 0x700
						if( (m_psh.dwFlags&(PSH_WATERMARK)) == (PSH_WATERMARK) )
								g_ResourceManager->LoadBitmap(
									_bitmapWatermark,
									m_psh.pszbmWatermark
									);
#endif // #if _MFC_VER >= 0x700
						CRect rcWaterMark = rcClient;
						HWND hWndChild = ::GetWindow( m_hWnd, GW_CHILD );
						for( ; hWndChild != NULL; hWndChild = ::GetWindow( hWndChild, GW_HWNDNEXT ) )
						{
							CRect rcTemp;
							::GetWindowRect( hWndChild, &rcTemp );
							ScreenToClient( &rcTemp );
							rcWaterMark.bottom = min( rcWaterMark.bottom, rcTemp.top );
						} // for( ; hWndChild != NULL; hWndChild = ::GetWindow( hWndChild, GW_HWNDNEXT ) )
						CRgn _rgn;
						if( _rgn.CreateRectRgnIndirect( &rcWaterMark ) )
							dc.SelectClipRgn( &_rgn );
						OnRenderWizardWatermarkArea(
							dc,
							rcWaterMark,
							_bitmapWatermark
							);
						dc.SelectClipRgn( NULL );

						PmBridge_GetPM()->OnPaintSessionComplete( this );
					}

					if( bDlg ) 
					{
						if( lpEntry != NULL )
						{
							// invoke the user defined message handler
							union 
							{
								AFX_PMSG pfn;
								void (AFX_MSG_CALL CCmdTarget::*pfn_COMMAND)();
							} mmf;
							mmf.pfn = lpEntry->pfn;
							(this->*mmf.pfn_COMMAND)();
							return 0L;
						} // if( lpEntry != NULL )
					}

					return TRUE;

				} // case WM_PAINT

			} // switch( nMessage )
		} // if( (m_psh.dwFlags&(__EXT_MFC_PSH_WIZARD97_MASK)) != 0 )
	}
	return
		CExtHookSink::OnHookWndMsg(
			lResult,
			hWndHooked,
			nMessage,
			wParam,
			lParam
			);
}

LRESULT CExtResizablePropertySheet::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
CPropertyPage * pWndPrevActivePage = NULL;
	switch( message )
	{
	case WM_CLOSE:
		if( m_bEnableSaveRestore )
			SaveWindowRect();
	break;
	case WM_SYSCOMMAND:
		if( wParam == SC_CLOSE )
		{
			if( m_bEnableSaveRestore )
				SaveWindowRect();
		}
	break;
	case WM_COMMAND:
		if( wParam == IDOK || wParam == IDCANCEL )
		{
			if( m_bEnableSaveRestore )
				SaveWindowRect();
		}
		else if( wParam == ID_WIZBACK || wParam == ID_WIZNEXT )
		{
			pWndPrevActivePage = CExtResPS::GetActivePage();
		}
	break;
	case WM_DESTROY:
		RemoveAllWndHooks();
	case WM_NCDESTROY:
		m_bSheetInitialized = false;
	break;
// 	case WM_SIZE:
// 		_SyncActivePage();
// 	break;
	} // switch( message )

LRESULT lResult =
		m_bSheetInitialized
		? CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > > :: WindowProc(message,wParam,lParam)
		: CExtResPS :: WindowProc(message,wParam,lParam)
		;
	
	switch( message )
	{
	case WM_SIZE:
		_SyncActivePage();
::PostMessage( m_hWnd, (WM_USER+10), 0, 0 );
	break;
	case WM_CREATE:
		{
			// window should be resizable
			ModifyStyle(
				0,
				WS_THICKFRAME | WS_CLIPCHILDREN
				);
			::PostMessage( m_hWnd, (WM_USER+10), 0, 0 );
		}
	break;
	case (WM_USER+10):
		{
			if( m_bSheetInitialized )
				break;
			CWnd * pTabCtrl = NULL; // GetTabControl();
			CWnd * pWnd = GetWindow( GW_CHILD );
			for( ; pWnd != NULL; pWnd = pWnd->GetWindow( GW_HWNDNEXT ) )
			{
				CExtSafeString sClassName;
				::GetClassName(
					pWnd->GetSafeHwnd(),
					sClassName.GetBuffer( _MAX_PATH+1 ),
					_MAX_PATH
					);
				sClassName.ReleaseBuffer();
				sClassName.MakeLower();
				if( sClassName == _T("button") )
				{
					UINT nID = UINT( pWnd->GetDlgCtrlID() );
					if(		nID == ID_WIZBACK
						||	nID == ID_WIZNEXT
						||	nID == ID_WIZFINISH
						||	nID == ID_APPLY_NOW
						||	nID == IDOK
						||	nID == IDCANCEL
						||	nID == IDHELP
						)
						continue;
					pTabCtrl = pWnd;
					break;
				}
			}
			if( GetActiveIndex() > 0 )
			{
				CRect rcAlignPages( 0, 0, 0, 0 );
				if( pTabCtrl != NULL && (m_psh.dwFlags&PSH_WIZARD) == 0 )
				{
					pTabCtrl->GetWindowRect( &rcAlignPages );
					ScreenToClient( &rcAlignPages );
					CExtPaintManager * pPM = PmBridge_GetPM();
					ASSERT_VALID( pPM );
					rcAlignPages.DeflateRect(
						pPM->UiScalingDo(  3, CExtPaintManager::__EUIST_X ),
						pPM->UiScalingDo( 22, CExtPaintManager::__EUIST_Y ),
						pPM->UiScalingDo(  4, CExtPaintManager::__EUIST_X ),
						pPM->UiScalingDo(  4, CExtPaintManager::__EUIST_Y )
						);
//					pTabCtrl->SendMessage( TCM_ADJUSTRECT, FALSE, (LPARAM)&rcAlignPages );
				} // if( pTabCtrl != NULL && (m_psh.dwFlags&PSH_WIZARD) == 0 )
				else if( (m_psh.dwFlags&PSH_WIZARD) != 0 )
				{
					GetClientRect( &rcAlignPages );
					CExtPaintManager * pPM = PmBridge_GetPM();
					ASSERT_VALID( pPM );
					rcAlignPages.DeflateRect(
						pPM->UiScalingDo( 11, CExtPaintManager::__EUIST_X ),
						pPM->UiScalingDo( 11, CExtPaintManager::__EUIST_Y ),
						0, //pPM->UiScalingDo(  8, CExtPaintManager::__EUIST_X ),
						pPM->UiScalingDo( 57, CExtPaintManager::__EUIST_Y )
						);
				} // else if( (m_psh.dwFlags&PSH_WIZARD) != 0 )
				if( ! rcAlignPages.IsRectEmpty() )
				{
					pWnd = GetWindow( GW_CHILD );
					for( ; pWnd != NULL; pWnd = pWnd->GetWindow( GW_HWNDNEXT ) )
					{
						if( ! pWnd->IsKindOf( RUNTIME_CLASS(CExtResizablePropertyPage) ) )
							continue;
						CRect rcFoundPage;
						pWnd->GetWindowRect( &rcFoundPage );
						ScreenToClient( &rcFoundPage );
						if( rcFoundPage == rcAlignPages )
							continue;
						RemoveAnchor( pWnd->m_hWnd );
						pWnd->MoveWindow( &rcAlignPages );
						AddAnchor( pWnd->m_hWnd, __RDA_LT, __RDA_RB );
					}
				} // if( ! rcAlignPages.IsRectEmpty() )
			} // if( GetActiveIndex() > 0 )
			
			m_bSheetInitialized = true;
			SetupHookWndSink( m_hWnd );
			Invalidate();
			CRect rcWnd;
			GetWindowRect( &rcWnd );
			CSize sizeWnd = rcWnd.Size();
			SetMinTrackSize( sizeWnd );
			::SetWindowPos(
				m_hWnd, NULL, 0,0,0,0,
				SWP_NOSIZE|SWP_NOMOVE
					|SWP_NOZORDER|SWP_NOOWNERZORDER
					|SWP_NOREPOSITION
					|SWP_FRAMECHANGED
				);

//			CTabCtrl * pTabCtrl = CExtResPS::GetTabControl();
			pWnd = GetWindow( GW_CHILD );
			for( ; pWnd != NULL; pWnd = pWnd->GetWindow(GW_HWNDNEXT)  )
			{
				if( pTabCtrl == pWnd )
				{
					pTabCtrl->ModifyStyle( 0, WS_CLIPSIBLINGS );
					AddAnchor( pTabCtrl->GetSafeHwnd(), __RDA_LT, __RDA_RB );
					continue;
				}
				
				CExtSafeString sClassName;
				::GetClassName( pWnd->GetSafeHwnd(), sClassName.GetBuffer( _MAX_PATH+1 ), _MAX_PATH );
				sClassName.ReleaseBuffer();
				sClassName.MakeLower();
				if( sClassName == LPCTSTR( _T("static") ) )
				{
					pWnd->ModifyStyle( 0, WS_CLIPSIBLINGS );
					
#if _MFC_VER >= 0x700
					if( (m_psh.dwFlags&__EXT_MFC_PSH_WIZARD97_MASK) != 0 )
					{
						if( pWnd->GetDlgCtrlID() == 0x3027 )
						{
							AddAnchor( pWnd->GetSafeHwnd(), __RDA_LT, __RDA_RT );
							continue;
						}
					}
#endif // #if _MFC_VER >= 0x700
					AddAnchor( pWnd->GetSafeHwnd(), __RDA_LB, __RDA_RB );
					continue;
				}
				if( sClassName == LPCTSTR( _T("button") ) )
				{
CString str; pWnd->GetWindowText( str );
						CRect rcWndButton;
					if( FromHandlePermanent( pWnd->GetSafeHwnd() ) == NULL )
					{
						pWnd->GetWindowRect( &rcWndButton );
						ScreenToClient( &rcWndButton );
						if(		g_PaintManager.m_bIsWinVistaOrLater
							&&	g_PaintManager.m_UxTheme.IsControlsThemed()
							)
							rcWndButton.OffsetRect( -14, -5 );
						else
							rcWndButton.OffsetRect( -10, 0 );
						pWnd->MoveWindow( &rcWndButton );

						pWnd->ModifyStyle( 0, WS_CLIPSIBLINGS );
						AddAnchor( pWnd->GetSafeHwnd(), __RDA_RB );
						CButtonInSheetHook * pBtn = new CButtonInSheetHook;
						VERIFY( pBtn->SubclassWindow( pWnd->GetSafeHwnd() ) );
					}
					continue;
				}
				if( sClassName == LPCTSTR( _T("systabcontrol32") ) )
				{
#if (!defined __EXT_MFC_NO_TAB_CTRL)
					CExtThemedTabCtrl * pTabs = new CExtThemedTabCtrl;
					pTabs->m_bAutoDestroyWindow = true;
					VERIFY( pTabs->SubclassWindow( pWnd->GetSafeHwnd() ) );
#endif // (!defined __EXT_MFC_NO_TAB_CTRL)
					AddAnchor( pWnd->GetSafeHwnd(), __RDA_LT, __RDA_RB );
					continue;
				}
				
			} // for( ; pWnd != NULL; pWnd = pWnd->GetWindow(GW_HWNDNEXT)  )

			_SyncActivePage();

			CExtPaintManager::monitor_parms_t _mp;
			CExtPaintManager::stat_GetMonitorParms( _mp, this );

			SetMaximizedRect( &_mp.m_rcWorkArea );
			SetMaxTrackSize( _mp.m_rcWorkArea.Size() );

			CRect _rcWnd;
			GetWindowRect( &_rcWnd );;
			CSize _size = _rcWnd.Size();
			::SendMessage(
				m_hWnd, WM_SIZE,
				WPARAM(SIZE_RESTORED),
				MAKELPARAM( _size.cx, _size.cy )
				);
			if( GetStyle() & WS_VISIBLE )
				RedrawWindow();

			OnResizablePropertySheetInitialized();
		}
	break;
	case WM_COMMAND:
		if( wParam == ID_WIZBACK || wParam == ID_WIZNEXT )
		{
			if( !m_bSheetInitialized )
				break;

			if( pWndPrevActivePage != NULL )
			{
				CPropertyPage * pWndNewActivePage = CExtResPS::GetActivePage();
				if( pWndPrevActivePage != pWndNewActivePage )
				{
					CRect rcWndPage;
					pWndPrevActivePage->GetWindowRect( &rcWndPage );
					ScreenToClient( &rcWndPage );
					pWndNewActivePage->MoveWindow( &rcWndPage );
				}
			}
			_SyncActivePage();
			//ArrangeLayout();
		}
	break;
	} // switch( message )

	return lResult;
}

BOOL CExtResizablePropertySheet::PreCreateWindow(CREATESTRUCT& cs)
{
	if( ! ( CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > > :: PreCreateWindow(cs) ) )
	{
		ASSERT( FALSE );
		return FALSE;
	}

	return TRUE;
}

BOOL CExtResizablePropertySheet::PreTranslateMessage(MSG* pMsg)
{
	if( GetSafeHwnd() != NULL )
	{ // if valid sheet
		if(		pMsg->message == WM_KEYDOWN
			&&	pMsg->wParam == VK_TAB
			)
		{ // if WM_KEYDOWN with VK_TAB
			HWND hWndFocus = ::GetFocus();
			if(		hWndFocus != NULL
				&&	GetSafeHwnd() != NULL
				&&	( hWndFocus == m_hWnd || ::IsChild( m_hWnd, hWndFocus ) )
				)
			{
				BOOL bShift = CExtPopupMenuWnd::IsKeyPressed(VK_SHIFT);
				BOOL bCtrl = CExtPopupMenuWnd::IsKeyPressed(VK_CONTROL);
				if( !bCtrl )
				{
					SendMessage( WM_NEXTDLGCTL, bShift, 0 );
					return TRUE;
				}
// + (2.25)
				if( bShift )
				{
					if( GetActiveIndex() == 0 )
						return TRUE;
					SendMessage( WM_COMMAND, ID_WIZBACK );
				} // if( bShift )
				else
				{
					if( GetActiveIndex() == (GetPageCount() - 1) )
						return TRUE;
					SendMessage( WM_COMMAND, ID_WIZNEXT );
				} // else from if( bShift )
				return TRUE;
// - (2.25)
//				CTabCtrl * pTabCtrl = CExtResPS::GetTabControl();
//				if( pTabCtrl != NULL )
//				{
//					int nItemCount = pTabCtrl->GetItemCount();
//					if( nItemCount > 1 )
//					{
//						int nCurSel = pTabCtrl->GetCurSel();
//						nCurSel += bShift ? -1 : 1;
//						if( nCurSel < 0 || nCurSel == nItemCount )
//						{
//							if( ! m_bAllowTabSwitchingLoop )
//								return TRUE;
//							if( nCurSel < 0 )
//								nCurSel = nItemCount - 1;
//							else
//							{
//								ASSERT( nCurSel == nItemCount );
//								nCurSel = 0;
//							}
//						} // if( nCurSel == nItemCount )
//						if( (pTabCtrl->GetStyle()&WS_VISIBLE) != 0 )
//						{
//							//pTabCtrl->SetCurSel( nCurSel );
//							pTabCtrl->SetFocus();
//						} // if( (pTabCtrl->GetStyle()&WS_VISIBLE) != 0 )
//						SetActivePage( nCurSel );
//						_SyncActivePage();
//					}
//					_TrickSyncActviveChild();
//				} // valid sheet
//				return TRUE;
			}
		} // if WM_KEYDOWN with VK_TAB
	} // if valid sheet
	return CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > > :: PreTranslateMessage(pMsg);
}

BOOL CExtResizablePropertySheet::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if( GetSafeHwnd() != NULL )
	{ // if valid sheet
		if( nID == ID_WIZNEXT || nID == ID_WIZBACK )
		{
			BOOL bRetVal =
				CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > >
					:: OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
			if( nCode == CN_COMMAND )
				_SyncActivePage();
			return bRetVal;
		} // if( nID == ID_WIZNEXT || nID == ID_WIZBACK )
		if( ( GetStyle() & WS_CHILD ) != 0 )
		{ // if valid child sheet
			if( CWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) )
				return TRUE;
			if(		(	nCode != CN_COMMAND
					&& nCode != CN_UPDATE_COMMAND_UI
					)
					||	( ((nID) & 0x8000) == 0 )
					||	nID >= 0xf000
					)
				return FALSE;
			// last crack goes to the current CWinThread object
			CWinThread* pThread = AfxGetThread();
			if( pThread != NULL )
			{
#ifdef _DEBUG
				if (afxTraceFlags & traceCmdRouting)
					TRACE1("Routing command id 0x%04X to app.\n", nID);
#endif
				if (pThread->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
					return TRUE;
			}
#ifdef _DEBUG
			if (afxTraceFlags & traceCmdRouting)
			{
				TRACE2("IGNORING command id 0x%04X sent to %hs dialog.\n", nID,
						GetRuntimeClass()->m_lpszClassName);
			}
#endif
			return FALSE;
		} // if valid child sheet
	} // if valid sheet
	return
		CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > >
			:: OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

void CExtResizablePropertySheet::OnSize(UINT nType, int cx, int cy) 
{
HWND hWndOwn = GetSafeHwnd();
	CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > > :: OnSize( nType, cx, cy );
	if( ! ::IsWindow( hWndOwn ) )
		return;
CTabCtrl * pTabCtrl = GetTabControl();
	if( pTabCtrl->GetSafeHwnd() == NULL )
		return;
DWORD dwStyle = GetStyle();
	if( (dwStyle & WS_CHILD ) != 0 )
	{
		int nAdjustTabWndDY =
			( (dwStyle&WS_CAPTION) == 0 )
				? ( ::GetSystemMetrics( SM_CYCAPTION )
					+ ::GetSystemMetrics( SM_CYDLGFRAME )
					  * 2
				  )
				: 0
				;
		pTabCtrl->MoveWindow( 0, 0, cx, cy - nAdjustTabWndDY, FALSE );
		//ArrangeLayout();
		_SyncActivePage();
		_TrickSyncActviveChild();
	} // if( (dwStyle & WS_CHILD ) != 0 )
	pTabCtrl->Invalidate( TRUE );
	pTabCtrl->UpdateWindow();
}

void CExtResizablePropertySheet::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
{
	CExtWA < CExtWS < __BASEOF_CExtResizablePropertySheet__ > > :: OnWindowPosChanged(lpwndpos);
//	if(		GetSafeHwnd() != NULL
//		&&	( GetStyle() & (WS_CHILD|WS_VISIBLE) ) == (WS_CHILD|WS_VISIBLE)
//		)
//	{ // if valid visible sheet
//		CTabCtrl * pTabCtrl = GetTabControl();
//		if( pTabCtrl != NULL )
//		{
//			CRect rcClient;
//			GetClientRect( &rcClient );
//			pTabCtrl->MoveWindow( 0, 0, rcClient.Width(), rcClient.Height(), FALSE );
//			//ArrangeLayout();
//			//_TrickSyncActviveChild();
//		} // if( pTabCtrl != NULL )
		_SyncActivePage();
		_TrickSyncActviveChild();
//	} // if valid visible sheet
}

void CExtResizablePropertySheet::OnResizablePropertySheetInitialized()
{
	_InitSizeGripper();
	if( ( GetStyle() & WS_CHILD ) != 0 )
	{
		_TrickSyncActviveChild();
	} // if( ( GetStyle() & WS_CHILD ) != 0 )
	else
	{
		// + (2.25)
		ModifyStyle( 0, WS_CLIPCHILDREN );
		if( m_bEnableSaveRestore )
			LoadWindowRect();
	} // else from if( ( GetStyle() & WS_CHILD ) != 0 )
	SetWindowPos(
		NULL, 0, 0, 0, 0,
		SWP_NOSIZE|SWP_NOMOVE
			|SWP_NOCOPYBITS
			|SWP_NOZORDER|SWP_NOOWNERZORDER
			|SWP_FRAMECHANGED
		);
//CTabCtrl * pTabCtrl = CPropertySheet::GetTabControl();
//	if(		pTabCtrl != NULL
//		&&	pTabCtrl->GetSafeHwnd() != NULL
//		)
//		pTabCtrl->ModifyStyle( TCS_MULTILINE, 0, SWP_FRAMECHANGED );
	//ArrangeLayout();
	_SyncActivePage();
}

void CExtResizablePropertySheet::_TrickSyncActviveChild()
{
}

