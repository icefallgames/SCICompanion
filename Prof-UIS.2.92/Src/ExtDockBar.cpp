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

#if (!defined __EXTDOCKBAR_H)
	#include "ExtDockBar.h"
#endif

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	#if (!defined __EXT_CONTROLBAR_TABBED_FEATURES_H)
		#include "ExtControlBarTabbedFeatures.h"
	#endif // __EXT_CONTROLBAR_TABBED_FEATURES_H
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

#if _MFC_VER < 0x700
	#include <../src/AfxImpl.h>
#else
	#include <../src/mfc/AfxImpl.h>
#endif

#if (!defined __EXT_MEMORY_DC_H)
	#include <../Src/ExtMemoryDC.h>
#endif

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	#if (!defined __EXTCUSTOMIZE_H)
		#include <ExtCustomize.h>
	#endif
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

#include <Resources/Resource.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////
// CExtDockBar window

IMPLEMENT_DYNCREATE( CExtDockBar, CDockBar );
IMPLEMENT_CExtPmBridge_MEMBERS( CExtDockBar );

BEGIN_MESSAGE_MAP(CExtDockBar, CDockBar)
	//{{AFX_MSG_MAP(CExtDockBar)
	//}}AFX_MSG_MAP
	ON_WM_CONTEXTMENU()
	ON_WM_CANCELMODE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE( WM_SIZEPARENT, OnSizeParent )
	ON_WM_NCCALCSIZE()
	ON_WM_NCPAINT()
	ON_WM_DESTROY()
	ON_MESSAGE( WM_HELPHITTEST, OnHelpHitTest )
END_MESSAGE_MAP()

bool CExtDockBar::g_bControlBarFixSizePixel = true;
bool CExtDockBar::g_bExtendedRepositioning = false;
bool CExtDockBar::g_bDockBarClassRegistered = false;

CExtDockBar::CExtDockBar(
	UINT nCircleNo // = 0
	)
	: m_nCircleNo( nCircleNo )
	, m_bLockedOptimize( false )
	, m_pDockBarOuter( NULL )
	, m_pDockBarInner( NULL )
	, m_rcLastInvisiblePreCalc( 0, 0, 0, 0 )
	, m_bInDynamicLayoutUpdate( false )
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	, m_pWndAutoHideArea( NULL )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
{
	VERIFY( RegisterDockBarClass() );

	PmBridge_Install();
}

CExtDockBar::~CExtDockBar()
{
	PmBridge_Uninstall();
}

bool CExtDockBar::RegisterDockBarClass()
{
	if( g_bDockBarClassRegistered )
		return true;
WNDCLASS _wndClassInfo;
HINSTANCE hInst = ::AfxGetInstanceHandle();
	if( ! ::GetClassInfo( hInst, __EXT_DOCKBAR_CLASS_NAME, &_wndClassInfo ) )
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
		_wndClassInfo.lpszClassName = __EXT_DOCKBAR_CLASS_NAME;
		if( !::AfxRegisterClass( &_wndClassInfo ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}
	g_bDockBarClassRegistered = true;
	return true;
}

void CExtDockBar::PreSubclassWindow()
{
	CDockBar::PreSubclassWindow();
}

BOOL CExtDockBar::PreCreateWindow(CREATESTRUCT& cs)
{
	if( ( !RegisterDockBarClass() )
		|| ( !CWnd::PreCreateWindow(cs) )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
	
	cs.lpszClass = __EXT_DOCKBAR_CLASS_NAME;

	return TRUE;
}

LRESULT CExtDockBar::OnHelpHitTest(WPARAM wParam, LPARAM lParam)
{
	wParam;
	lParam;
	return 0L;
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

void CExtDockBar::OnCustomizeModeEnter()
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( _GetCircleNo() > 0 )
		return;
	if( m_pWndAutoHideArea == NULL )
		return;
	ASSERT_VALID( m_pWndAutoHideArea );
	m_pWndAutoHideArea->OnCustomizeModeEnter();
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
}

void CExtDockBar::OnCustomizeModeLeave()
{
	ASSERT_VALID( this );
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( _GetCircleNo() > 0 )
		return;
	if( m_pWndAutoHideArea == NULL )
		return;
	ASSERT_VALID( m_pWndAutoHideArea );
	m_pWndAutoHideArea->OnCustomizeModeLeave();
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
}

#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

void CExtDockBar::OnCancelMode()
{
	ASSERT_VALID( this );
	CDockBar::OnCancelMode();

BOOL bHelpMode =
		::AfxGetApp()->m_bHelpMode
		|| CExtControlBar::FindHelpMode( this )
		;
	if( !bHelpMode )
		CExtPopupMenuWnd::CancelMenuTracking();
	CWnd::CancelToolTips();
	//ASSERT( !CExtPopupMenuWnd::IsMenuTracking() );
}

CFrameWnd * CExtDockBar::_GetDockingFrameImpl()
{
	ASSERT_VALID( this );
	return
		CExtControlBar::_GetDockingFrameImpl( (CControlBar *)this );
}

#if (!defined __EXT_MFC_NO_CUSTOMIZE)

bool CExtDockBar::_AppendAddRemoveButtonsCustomizeMenu(
	CExtPopupMenuWnd * pPopup,
	CControlBar * pBarEventSrc
	)
{
	ASSERT_VALID( pPopup );
	ASSERT_VALID( pBarEventSrc );
	ASSERT( pBarEventSrc->GetSafeHwnd() != NULL );
	if( ! pBarEventSrc->IsKindOf(RUNTIME_CLASS(CExtToolControlBar)) )
	{
		ASSERT( FALSE );
		return false;
	}
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( pBarEventSrc->m_hWnd );
	if( pSite == NULL )
		return true;
	if( ! pSite->BarIsRegistered((CExtToolControlBar*)pBarEventSrc) )
		return true;

CExtSafeString strProfileName =
		g_CmdManager->ProfileNameFromWnd( pBarEventSrc->m_hWnd );
	ASSERT( !strProfileName.IsEmpty() );
	VERIFY(
		g_CmdManager->UpdateFromMenu(
			strProfileName,
			IDR_MENU_CUSTOMIZE_CMDS,
			true
			)
		);

	pSite->BarDefCmdTargetSet((CExtToolControlBar*)pBarEventSrc);
bool bUserBar = false;
	VERIFY(
		pSite->GetToolbarCustomizeInfo(
			(CExtToolControlBar*)pBarEventSrc, NULL, NULL, NULL,
			&bUserBar
			)
		);

CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			strProfileName,
			ID_EXT_ADD_REMOVE_BUTTONS
			);
CExtSafeString strAddRemoveButtons;
	if( pCmdItem )
		strAddRemoveButtons = pCmdItem->m_sMenuText;
	if( strAddRemoveButtons.IsEmpty() )
		strAddRemoveButtons = _T("&Add/remove buttons");
	VERIFY(
		pPopup->ItemInsert(
			(UINT)CExtPopupMenuWnd::TYPE_POPUP,
			-1,
			strAddRemoveButtons
			)
		);
CExtPopupMenuWnd::MENUITEMDATA & miARB =
		pPopup->ItemGetInfo( pPopup->ItemGetCount() - 1 );
	miARB.SetChildCombine();
	miARB.SetToolButton();
	miARB.SetToolWrap();
CExtPopupMenuWnd * pAddRemoveButtons = miARB.GetPopup();
	ASSERT( pAddRemoveButtons != NULL );
	
	VERIFY( pAddRemoveButtons->ItemInsert( ID_EXT_CUSTOMIZE ) );
CExtPopupMenuWnd::MENUITEMDATA & miCustomize =
		pAddRemoveButtons->ItemGetInfo(
			pAddRemoveButtons->ItemGetCount() - 1
			);
	miCustomize.SetForceEnabled();

	if( ! bUserBar )
	{
		bool bAddResetBarHere = true;
		if( pBarEventSrc->m_pDockBar != NULL )
		{
			ASSERT_VALID( pBarEventSrc->m_pDockBar );
			MfcControlBarVector_t vBars;
			if( pBarEventSrc->IsFloating() )
				vBars.Add( pBarEventSrc );
			else
			{
				int nPos = pBarEventSrc->m_pDockBar->FindBar( pBarEventSrc );
				ASSERT( nPos >= 0 && nPos < pBarEventSrc->m_pDockBar->m_arrBars.GetSize() );
				for( nPos--; nPos >= 0 && pBarEventSrc->m_pDockBar->m_arrBars[nPos] != NULL; nPos-- );
					nPos++;
				ASSERT( nPos >= 0 && nPos < pBarEventSrc->m_pDockBar->m_arrBars.GetSize() );
				for( ; pBarEventSrc->m_pDockBar->m_arrBars[nPos] != NULL && nPos < pBarEventSrc->m_pDockBar->m_arrBars.GetSize(); nPos++ )
				{
					CControlBar * pBarInRow = (CControlBar *)pBarEventSrc->m_pDockBar->m_arrBars[nPos];
					if( pBarInRow == NULL || __PLACEHODLER_BAR_PTR(pBarInRow) )
						continue;
					ASSERT_VALID( pBarInRow );
					ASSERT_KINDOF( CControlBar, pBarInRow );
					if( !pBarInRow->IsKindOf(RUNTIME_CLASS(CExtToolControlBar)) )
						continue;
					vBars.Add( pBarInRow );
				}
			} // else from if( pBarEventSrc->IsFloating() )
			int nCount = (int)vBars.GetSize();
			ASSERT( nCount >= 1 );
			for( int i = 0; i < nCount; )
			{
				CExtToolControlBar * pBar = (CExtToolControlBar*)vBars[i];
				ASSERT_VALID( pBar );
				ASSERT_KINDOF( CExtToolControlBar, pBar );
				if(		( !pSite->BarIsRegistered(pBar) )
					||	( !pBar->OnCustomizeCanShowHideButtons() )
					)
				{
					vBars.RemoveAt( i );
					nCount--;
					continue;
				}
				bool bUserItem = false;
				VERIFY(
					pSite->GetToolbarCustomizeInfo(
						pBar, NULL, NULL, NULL,
						&bUserItem
						)
					);
				if( bUserItem )
				{
					vBars.RemoveAt( i );
					nCount--;
					continue;
				}
				i++;
			} // for( int i = 0; i < nCount; i++ )
			ASSERT( nCount == vBars.GetSize() );
			if( nCount > 0 )
			{
				bAddResetBarHere = false;
				int i = 0;
				for( i = 0; i < nCount; i++ )
				{
					CExtToolControlBar * pBar = (CExtToolControlBar*)vBars[i];
					ASSERT_VALID( pBar );
					ASSERT_KINDOF( CExtToolControlBar, pBar );
					CString sBarName;
					pBar->GetWindowText( sBarName );
					VERIFY(
						pAddRemoveButtons->ItemInsert(
							(UINT)CExtPopupMenuWnd::TYPE_POPUP,
							i,
							(LPCTSTR)sBarName
							)
						);
					CExtPopupMenuWnd::MENUITEMDATA & mi = pAddRemoveButtons->ItemGetInfo(i);
					CExtPopupMenuWnd * pFolder = mi.GetPopup();
					ASSERT_VALID( pFolder );
					pBar->OnCustomizeInitShowHideButtonMenu( pFolder );
					ASSERT( pFolder->ItemGetCount() > 0 );
					VERIFY( pFolder->ItemInsert() );
					VERIFY( pFolder->ItemInsert( ID_EXT_RESET_TOOLBAR ) );
					CExtPopupMenuWnd::MENUITEMDATA & miResetToolbar =
						pFolder->ItemGetInfo(
							pFolder->ItemGetCount() - 1
							);
					miResetToolbar.SetForceEnabled();
					miResetToolbar.SetCmdDeliverCb(
						(CExtPopupMenuWnd::MENUITEMDATA::pCbItemClick)
							CExtToolControlBar::stat_CbCmdDeliver
						);
					miResetToolbar.LParamSet( (LPARAM)pBar );
				} // for( i = 0; i < nCount; i++ )
				VERIFY(
					pAddRemoveButtons->ItemInsert(
						(UINT)CExtPopupMenuWnd::TYPE_SEPARATOR,
						i
						)
					);
			} // if( nCount > 0 )
		} // if( pBarEventSrc->m_pDockBar != NULL )
		if( bAddResetBarHere )
		{
			VERIFY( pAddRemoveButtons->ItemInsert( ID_EXT_RESET_TOOLBAR ) );
			CExtPopupMenuWnd::MENUITEMDATA & miResetToolbar =
				pAddRemoveButtons->ItemGetInfo(
					pAddRemoveButtons->ItemGetCount() - 1
					);
			miResetToolbar.SetForceEnabled();
			miResetToolbar.SetCmdDeliverCb(
				(CExtPopupMenuWnd::MENUITEMDATA::pCbItemClick)
					CExtToolControlBar::stat_CbCmdDeliver
				);
			miResetToolbar.LParamSet( (LPARAM)pBarEventSrc );
		} // if( bAddResetBarHere )
	} // if( ! bUserBar )
	
	return true;
}
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

bool CExtDockBar::_ConstructDockSiteControlBarPopupMenu(
	CFrameWnd * pFrame,
	CExtPopupMenuWnd * pPopup,
	SHORT nHelperNotificationType,
	CControlBar * pBarEventSrc,
	UINT nMsgID, // = WM_NULL
	CObject * pObjectSrc // = NULL
	)
{
	ASSERT_VALID( pFrame );
	ASSERT( pPopup != NULL );

CExtControlBar::POPUP_MENU_EVENT_DATA _pmed(
		nHelperNotificationType,
		pPopup,
		pBarEventSrc,
		CPoint( -1, -1 ),
		nMsgID,
		pObjectSrc
		);
bool bPreNotificationPassed = _pmed.NotifyTarget( false );
	if( !bPreNotificationPassed )
	{
		ExtControlBarVector_t vBars;
		CExtControlBar::_GetFrameControlBars(
			NULL,
			pFrame,
			vBars
			);
		bool bOleInplaceItemActivated =
			CExtControlBar::IsOleIpObjActive( pFrame );

		int nBarsCount = (int)vBars.GetSize();
		CExtControlBar * pPrevBar = NULL;
		for( int i=0; i<nBarsCount; i++ )
		{
			CExtControlBar * pBar = vBars[i];
			ASSERT_VALID( pBar );
			if( pBar->GetSafeHwnd() == NULL )
				continue;
			// do not include dynamic bars
			if( pBar->IsKindOf( RUNTIME_CLASS(CExtDynControlBar) ) )
				continue;

			// do not include temporary hidden bars
			if( pBar->m_nStateFlags &
					//(CControlBar::delayHide | CControlBar::tempHide)
					CControlBar::tempHide
				)
				continue;
			// do not include some bars when OLE inplace object active
			if( bOleInplaceItemActivated &&
					(pBar->m_dwStyle & CBRS_HIDE_INPLACE)
				)
				continue;

			// do not include bars without caption
			CString sBarText;
			pBar->GetWindowText( sBarText );
			if( sBarText.IsEmpty() )
			{
				// do not include bars without window text to menu
				// ASSERT( FALSE );
				continue;
			}

			if( pPrevBar != NULL
				&&
				pPrevBar->IsFixedDockStyle() != pBar->IsFixedDockStyle()
				)
			{
				VERIFY(
					pPopup->ItemInsert(
						CExtPopupMenuWnd::TYPE_SEPARATOR
						)
					);
			}

			int nBarID = pBar->GetDlgCtrlID();
			ASSERT( CExtCmdManager::IsCommand(nBarID) );
			
			CExtCmdItem _cmd;
			_cmd.m_nCmdID = nBarID;
			_cmd.m_sMenuText = sBarText;
			_cmd.TipsLoad();
			g_CmdManager->CmdSetup(
				g_CmdManager->ProfileNameFromWnd( pFrame->GetSafeHwnd() ),
				_cmd
				);

			if( pBar->OnConstructDockSiteControlBarPopupMenu(pPopup) )
				pPrevBar = pBar;

		} // for( int i=0; i<nBarsCount; i++ )

	} // if( !bPreNotificationPassed )

	// remove double separators
INT nCount = pPopup->ItemGetCount();
	for( INT nItem = 1; nItem < nCount; )
	{
		CExtPopupMenuWnd::MENUITEMDATA & _miiPrev =
			pPopup->ItemGetInfo( nItem - 1 );
		if( !_miiPrev.IsSeparator() )
		{
			nItem++;
			continue;
		}
		CExtPopupMenuWnd::MENUITEMDATA & _miiCurr =
			pPopup->ItemGetInfo( nItem );
		if( !_miiCurr.IsSeparator() )
		{
			nItem++;
			continue;
		}
		pPopup->ItemRemove( nItem );
		nCount--;
	} // for( INT nItem = 1; nItem < nCount; )

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
bool bAddCustomizeCmd = false;
	if(		(!bPreNotificationPassed)
		&&	(	nHelperNotificationType == CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_CONTROLBAR_CTX
			||	nHelperNotificationType == CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_CONTROLBAR_NC_CTX
			||	nHelperNotificationType == CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_DOCKBAR_CTX
			||	nHelperNotificationType == CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_STATUSBAR_CTX
			||	nHelperNotificationType == CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_MINIFRAME_NC_CTX
			)
		&&	CExtCustomizeSite::GetCustomizeSite( pBarEventSrc->GetSafeHwnd() ) != NULL
		)
		bAddCustomizeCmd = true;
	if(		bAddCustomizeCmd
		&&	pBarEventSrc->GetSafeHwnd() != NULL
		&&	pBarEventSrc->IsKindOf( RUNTIME_CLASS( CExtToolControlBar ) )
		&&	( ! ((CExtToolControlBar*)pBarEventSrc)->m_bCustomizationAllowed )
		)
		bAddCustomizeCmd = false;
	if( bAddCustomizeCmd )
	{ // add customize command
		LPCTSTR strCmdProfile =  g_CmdManager->ProfileNameFromWnd( pFrame->m_hWnd );
		if(		strCmdProfile != NULL
			&&	g_CmdManager->CmdGetPtr( strCmdProfile, ID_EXT_CUSTOMIZE ) != NULL
			)
		{
			// add tail separator
			nCount = pPopup->ItemGetCount();
			if( nCount > 0 )
			{
				CExtPopupMenuWnd::MENUITEMDATA & _mii =
					pPopup->ItemGetInfo( nCount - 1 );
				if( !_mii.IsSeparator() )
					pPopup->ItemInsert();
			} // if( nCount > 0 )
			VERIFY( pPopup->ItemInsert( ID_EXT_CUSTOMIZE ) );
			CExtPopupMenuWnd::MENUITEMDATA & mi =
				pPopup->ItemGetInfo(
					pPopup->ItemGetCount() - 1
					);
			mi.SetForceEnabled();
		} // if( strCmdProfile != NULL ...
	} // add customize command
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	_pmed.NotifyTarget( true );

	// remove tail separator
	nCount = pPopup->ItemGetCount();
	if( nCount > 0 )
	{
		CExtPopupMenuWnd::MENUITEMDATA & _mii =
			pPopup->ItemGetInfo( nCount - 1 );
		if( _mii.IsSeparator() )
			pPopup->ItemRemove( nCount - 1 );
	} // if( nCount > 0 )

	return true;
}

void CExtDockBar::OnContextMenu(CWnd* pWnd, CPoint point)
{
	ASSERT_VALID( this );
	pWnd;

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

	// find any control bar
CControlBar * pBar = NULL;
	for( int nPos = 0; nPos < m_arrBars.GetSize(); nPos++ )
	{
		pBar = GetDockedControlBar(nPos);
		if( pBar != NULL )
		{
			ASSERT_VALID( pBar );
			break;
		}
	}
	if( pBar == NULL )
		return;

CFrameWnd * pFrame = _GetDockingFrameImpl();
HWND hWndTrack = pBar->GetOwner()->GetSafeHwnd();
	ASSERT( hWndTrack != NULL && ::IsWindow(hWndTrack) );
CExtPopupMenuWnd * pPopup =
		CExtPopupMenuWnd::InstantiatePopupMenu(
			GetSafeHwnd(),
			RUNTIME_CLASS(CExtPopupMenuWnd),
			this
			);
	VERIFY( pPopup->CreatePopupMenu(hWndTrack) );

	if(	! _ConstructDockSiteControlBarPopupMenu(
			pFrame,
			pPopup,
			CExtControlBar::POPUP_MENU_EVENT_DATA::__PMED_DOCKBAR_CTX,
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

void CExtDockBar::DoPaint( CDC * pDC )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pDC );
	CExtPaintManager::stat_ExcludeChildAreas(
		pDC->GetSafeHdc(),
		GetSafeHwnd()
		);
CRect rcClient;
	GetClientRect( &rcClient );
CExtMemoryDC dc( pDC, &rcClient );
	ASSERT( dc.GetSafeHdc() != NULL );
	if( dc.GetSafeHdc() != NULL )
		pDC = &dc;
	PmBridge_GetPM()->PaintDockBarClientArea(
		*pDC,
		rcClient,
		this
		);
	PmBridge_GetPM()->OnPaintSessionComplete( this );
}

BOOL CExtDockBar::OnEraseBkgnd( CDC* pDC )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pDC );
	pDC;
//	CExtPaintManager::stat_ExcludeChildAreas(
//		*pDC,
//		*this
//		);
//	return CDockBar::OnEraseBkgnd(pDC);
	return TRUE;
}

void CExtDockBar::OnDynamicLayoutUpdate()
{
	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtDockBar, this );

	if( m_bLockedOptimize )
		return;

	if( m_bInDynamicLayoutUpdate )
		return;
	m_bInDynamicLayoutUpdate = true;

	ASSERT( m_arrBars.GetSize() >= 1 );
	for( INT nBar = 1; nBar < m_arrBars.GetSize(); nBar++ )
	{
		CExtControlBar * pBar = (CExtControlBar *)
			m_arrBars[ nBar ];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CExtControlBar, pBar );
		if( pBar->IsFixedMode() || pBar->IsFixedDockStyle() )
			continue;
		if( !pBar->IsVisible() )
			continue;
		if( !pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
			continue;
		ASSERT_VALID( ((CExtDynControlBar *)pBar)->m_pWndDynDocker );
		((CExtDynControlBar *)pBar)->m_pWndDynDocker->OnDynamicLayoutUpdate();
	}
	m_bInDynamicLayoutUpdate = false;
}

void CExtDockBar::OnDynamicLayoutOptimize()
{
	if( m_bLockedOptimize )
		return;

	ASSERT( m_arrBars.GetSize() >= 1 );
	for( INT nBar = 1; nBar < m_arrBars.GetSize(); nBar++ )
	{
		CExtControlBar * pBar = (CExtControlBar *)
			m_arrBars[ nBar ];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CExtControlBar, pBar );
		if( pBar->IsFixedMode() || pBar->IsFixedDockStyle() )
			continue;
		if( !pBar->IsVisible() )
			continue;
		if( !pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
			continue;
		ASSERT_VALID( ((CExtDynControlBar *)pBar)->m_pWndDynDocker );
		((CExtDynControlBar *)pBar)->m_pWndDynDocker->OnDynamicLayoutOptimize();
	}
}

CSize CExtDockBar::CalcFixedLayout(
	BOOL bStretch,
	BOOL bHorz
	)
{
	// based on MFC's source of
	// CDockBar::CalcFixedLayout()
	ASSERT_VALID(this);
CSize sizeFixed =
		CControlBar::CalcFixedLayout( bStretch, bHorz );
	// get max size
CSize sizeMax;
	if( !m_rectLayout.IsRectEmpty() )
		sizeMax = m_rectLayout.Size();
	else
	{
		CFrameWnd * pFrame = GetParentFrame();
		CRect rcFrameWindow;
		pFrame->GetClientRect( &rcFrameWindow );
		sizeMax = rcFrameWindow.Size();
	}
	// prepare for layout
AFX_SIZEPARENTPARAMS layout;
	layout.hDWP = m_bLayoutQuery ?
		NULL : ::BeginDeferWindowPos( (int)m_arrBars.GetSize() );

CPoint pt( 0, 0 );
int nWidth = 0;

BOOL bWrapped = FALSE;

	for( int nPos = 0; nPos < m_arrBars.GetSize(); nPos++ )
	{ // layout all the control bars
		CControlBar * pBar = GetDockedControlBar(nPos);
		void * pVoid = m_arrBars[nPos];

		if( pBar != NULL )
		{
			CRect rcBarWin, rcBarClient;
			pBar->GetWindowRect( &rcBarWin );
			pBar->ScreenToClient( & rcBarWin );
			pBar->GetClientRect( &rcBarClient );
			CSize sizeBarMin =
				rcBarWin.Size() - rcBarClient.Size();

			if(	pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
			{
				if( ! ((CExtControlBar*)pBar)->IsFixedMode() )
				{
					sizeBarMin =
						CSize(
							((CExtControlBar *)pBar)->_CalcDesiredMinHW(),
							((CExtControlBar *)pBar)->_CalcDesiredMinVH()
							);
				} // if( ! ((CExtControlBar*)pBar)->IsFixedMode() )
			} // if(	pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
			if( pBar->IsVisible() )
			{
				// get ideal rect for bar
				DWORD dwMode = 0;
				if(	(pBar->m_dwStyle & CBRS_SIZE_DYNAMIC)
					&&
					(pBar->m_dwStyle & CBRS_FLOATING)
					)
					dwMode |= LM_HORZ | LM_MRUWIDTH;
				else if(pBar->m_dwStyle & CBRS_ORIENT_HORZ)
					dwMode |= LM_HORZ | LM_HORZDOCK;
				else
					dwMode |=  LM_VERTDOCK;

				CSize sizeBar =
					pBar->CalcDynamicLayout(-1, dwMode);
				CRect rc(pt, sizeBar);

				// get current rect for bar
				CRect rcBar;
				pBar->GetWindowRect( &rcBar );
				ScreenToClient( &rcBar );

				if( bHorz )
				{
					// Offset Calculated Rect out to Actual
					if(	rcBar.left > rc.left
						&& !m_bFloating
						)
						rc.OffsetRect(
							rcBar.left - rc.left,
							0
							);

					// If ControlBar goes off the right, then right justify
					if(	rc.right > sizeMax.cx
						&& !m_bFloating
						)
					{
						int x = rc.Width();
						x = max(sizeMax.cx - x, pt.x);
						rc.OffsetRect(
							x - rc.left,
							0
							);
						if(	rc.right  > sizeMax.cx )
							rc.right -= rc.right - sizeMax.cx;
					}

					// If ControlBar has been wrapped, then left justify
					if( bWrapped )
					{
						bWrapped = FALSE;
						rc.OffsetRect( - rc.left, 0 );

					}
					// If ControlBar is completely invisible, then wrap it
					else if(
						rc.Width() < sizeBarMin.cx
						||
						(rc.left >= sizeMax.cx)
							&& (nPos > 0) && (m_arrBars[nPos - 1] != NULL)
						)
					{
						if(	!pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
							||	(
								pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
								&&
								((CExtControlBar*)pBar)->IsFixedMode()
								)
							)
						{
							m_arrBars.InsertAt(
								nPos,
								(CObject*)NULL
								);
							pBar = NULL;
							pVoid = NULL;
							bWrapped = TRUE;
						}
					}
					if( !bWrapped )
					{
						if(rc != rcBar)
						{
							if(	!m_bLayoutQuery &&
								!(pBar->m_dwStyle & CBRS_FLOATING)
								)
								pBar->m_pDockContext->
									m_rectMRUDockPos = rc;
							AfxRepositionWindow(
								&layout,
								pBar->m_hWnd,
								&rc
								);
						}
						pt.x = rc.left + sizeBar.cx;
						nWidth = max(nWidth, sizeBar.cy);
					} // if( !bWrapped )
				} // if( bHorz )
				else
				{
					// Offset Calculated Rect out to Actual
					if(	rcBar.top > rc.top
						&& !m_bFloating
						)
						rc.OffsetRect(
							0,
							rcBar.top - rc.top
							);

					// If ControlBar goes off the bottom, then bottom justify
					if(	rc.bottom > sizeMax.cy
						&& !m_bFloating
						)
					{
						int y = rc.Height();
						y = max(sizeMax.cy - y, pt.y);
						rc.OffsetRect(
							0,
							y - rc.top
							);
						if(	rc.bottom  > sizeMax.cy )
							rc.bottom -= rc.bottom - sizeMax.cy;
					}

					// If ControlBar has been wrapped, then top justify
					if( bWrapped )
					{
						bWrapped = FALSE;
						rc.OffsetRect( 0, - rc.top );
					}
					// If ControlBar is completely invisible, then wrap it
					else if(
						rc.Height() < sizeBarMin.cy
						||
						(rc.top >= sizeMax.cy)
							&& (nPos > 0) && (m_arrBars[nPos - 1] != NULL)
						)
					{
						if(	!pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) 
							||	(
								pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
								&&
								((CExtControlBar*)pBar)->IsFixedMode()
								)
							)
						{
							m_arrBars.InsertAt(
								nPos,
								(CObject*)NULL
								);
							pBar = NULL;
							pVoid = NULL;
							bWrapped = TRUE;
						}
					}

					if( !bWrapped )
					{
						if(rc != rcBar)
						{
							if(	!m_bLayoutQuery
								&& !(pBar->m_dwStyle & CBRS_FLOATING)
								&& pBar->m_pDockContext != NULL
								)
								pBar->m_pDockContext->
									m_rectMRUDockPos = rc;
							AfxRepositionWindow(
								&layout,
								pBar->m_hWnd,
								&rc
								);
						}
						pt.y = rc.top + sizeBar.cy;
						nWidth = max(nWidth, sizeBar.cx);
					} // if( !bWrapped )
				} // else from if( bHorz )
			
//				if( !m_bLayoutQuery && pBar != NULL )
//				{
//					if( pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
//					{
//						ASSERT_VALID( ((CExtDynControlBar *)pBar)->m_pWndDynDocker );
//						((CExtDynControlBar *)pBar)->m_pWndDynDocker->OnDynamicLayoutUpdate();
//					}
//				}

			} // if( pBar->IsVisible() )

			if( !bWrapped )
				// handle any delay/show hide for the bar
				pBar->RecalcDelayShow( &layout );

		} // if( pBar != NULL )
		else
		{
			if( g_bExtendedRepositioning )
			{
				// try to remove wrapping:
				// are we have free space in the end of previous row?
				if( nPos > 0 && nPos != m_arrBars.GetSize()-1 )
				{
					int nPosPrev = nPos-1;
					CControlBar * pPrevBar = NULL;
					do
					{
						pPrevBar = GetDockedControlBar(nPosPrev--);
						if( pPrevBar != NULL
							&& pPrevBar->IsVisible()
							&& (!pPrevBar->IsFloating())
							)
						{
							// get ideal rect for prev bar
							DWORD dwMode = 0;
							if(	(pPrevBar->m_dwStyle & CBRS_SIZE_DYNAMIC)
								&&
								(pPrevBar->m_dwStyle & CBRS_FLOATING)
								)
								dwMode |= LM_HORZ | LM_MRUWIDTH;
							else if(pPrevBar->m_dwStyle & CBRS_ORIENT_HORZ)
								dwMode |= LM_HORZ | LM_HORZDOCK;
							else
								dwMode |=  LM_VERTDOCK;

							CSize sizePrevBar =
								pPrevBar->CalcDynamicLayout(-1, dwMode);

							CRect rcPrev(pt, sizePrevBar);

							if( bHorz )
							{
								if(	rcPrev.right > sizeMax.cx
									&& !m_bFloating
									)
									pPrevBar = NULL;
							}
							else
							{
								if(	rcPrev.bottom > sizeMax.cy
									&& !m_bFloating
									)
									pPrevBar = NULL;
							}
							if( pPrevBar != NULL )
								break;
						}
						pPrevBar = NULL;
					} while( nPosPrev >= 0 );
					if( pPrevBar != NULL )
					{
						m_arrBars.RemoveAt(nPos);
						nPos--;
						continue;
					}
				} // if( nPos > 0 && nPos != m_arrBars.GetSize()-1 )
			} // if( g_bExtendedRepositioning )
		} // else from if( pBar != NULL )

		if( pBar == NULL && pVoid == NULL && nWidth != 0 )
		{
			// end of row because pBar == NULL
			if( bHorz )
			{
				pt.y += nWidth;
				sizeFixed.cx = max(sizeFixed.cx, pt.x);
				sizeFixed.cy = max(sizeFixed.cy, pt.y);
				pt.x = 0;
				sizeFixed.cy--;
			}
			else
			{
				pt.x += nWidth;
				sizeFixed.cx = max(sizeFixed.cx, pt.x);
				sizeFixed.cy = max(sizeFixed.cy, pt.y);
				pt.y = 0;
				sizeFixed.cx--;
			}
			nWidth = 0;
		}
	} // layout all the control bars
	if( !m_bLayoutQuery )
	{
		ASSERT( layout.hDWP != NULL );
		if( layout.hDWP != NULL )
		{
			VERIFY( ::EndDeferWindowPos(layout.hDWP) );
		}
	} // if( !m_bLayoutQuery )

	// adjust size for borders on the dock bar itself
CRect rc( 0, 0, 0, 0 );
	CalcInsideRect(rc, bHorz);
	if( (!bStretch || !bHorz) && sizeFixed.cx != 0 )
		sizeFixed.cx +=
			-rc.right + rc.left
			+ g_bControlBarFixSizePixel ? 1 : 0;
	if(	(!bStretch || bHorz) && sizeFixed.cy != 0 )
		sizeFixed.cy +=
			-rc.bottom + rc.top
			+ g_bControlBarFixSizePixel ? 1 : 0;

	if( ! m_bLayoutQuery )
		OnDynamicLayoutUpdate();

	return sizeFixed;
}

void CExtDockBar::RemoveAllPlaceHolders(
	bool bSearchPlaceHolder // = true
	)
{
	ASSERT_VALID( this );
INT nCount = (INT)m_arrBars.GetSize();
	ASSERT( nCount > 0 );
	ASSERT( m_arrBars[0] == NULL );
MfcControlBarVector_t vPlaceHolders;
INT nBar = 0; 
	for( nBar = 1; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *) m_arrBars[ nBar ];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
		{
			vPlaceHolders.Add( pBar );
			continue;
		}
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CControlBar, pBar );
	} // for( nBar = 1; nBar < nCount; nBar++ )

	nCount = (INT)vPlaceHolders.GetSize();
	for( nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = vPlaceHolders[nBar];
		RemovePlaceHolder( pBar, bSearchPlaceHolder );
	} // for( nBar = 0; nBar < nCount; nBar++ )
}

void CExtDockBar::RemovePlaceHolder(
	CControlBar * pBar,
	bool bSearchPlaceHolder, // = true
	bool * p_bRemoved // = NULL
	)
{
	if( p_bRemoved != NULL )
		*p_bRemoved = false;
	// remove remembered docking position
#if _MFC_VER >= 0x710
	if (DWORD_PTR(pBar) > 0x0FFFF)
		pBar = (CControlBar*)(DWORD_PTR)_AfxGetDlgCtrlID(pBar->m_hWnd);
#else
	if( HIWORD(pBar) != 0 )
		pBar = (CControlBar*)_AfxGetDlgCtrlID(pBar->m_hWnd);
#endif
int nOldPos = FindBar(pBar);
	if( nOldPos > 0)
	{
		m_arrBars.RemoveAt(nOldPos);

		// remove section indicator (NULL) if nothing else in section
		if(		m_arrBars[nOldPos-1] == NULL
			&&	m_arrBars[nOldPos] == NULL
			)
			m_arrBars.RemoveAt(nOldPos);

		if( p_bRemoved != NULL )
			*p_bRemoved = true;

		return;
	}
	if( !bSearchPlaceHolder )
		return;


	_RemovePlaceHolder(
		m_pDockSite,
		pBar,
		p_bRemoved
		);
}

void CExtDockBar::_RemovePlaceHolder(
	CFrameWnd * pFrame,
	CControlBar * pBar,
	bool * p_bRemoved // = NULL
	)
{
	ASSERT_VALID( pFrame );
	ASSERT( pBar != NULL ); // may be placeholder ID
POSITION pos = pFrame->m_listControlBars.GetHeadPosition();
	for( ; pos != NULL; )
	{
		CControlBar * pTestBar = (CControlBar *)
			pFrame->m_listControlBars.GetNext( pos );
		ASSERT_VALID( pTestBar );
		ASSERT_KINDOF( CControlBar, pTestBar );
		if( !pTestBar->IsDockBar() )
		{
			if( pTestBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
			{
				CExtDockDynBar * pDynDocker =
					((CExtDynControlBar*)pTestBar)->
						m_pWndDynDocker;
				ASSERT_VALID( pDynDocker );
				bool bRemoved = false;
				pDynDocker->RemovePlaceHolder(
					pBar,
					false,
					&bRemoved
					);
				if( bRemoved )
				{
					if( p_bRemoved != NULL )
						*p_bRemoved = true;
					return;
				}
			}
			continue;
		}
		ASSERT_KINDOF( CDockBar, pTestBar );
		// dirty, but safe
		bool bRemoved = false;
		((CExtDockBar *)pTestBar)->RemovePlaceHolder(
			pBar,
			false,
			&bRemoved
			);
		if( bRemoved )
		{
			if( p_bRemoved != NULL )
				*p_bRemoved = true;
			return;
		}
	}
}

void CExtDockBar::_SlideDockControlBar(
	CControlBar* pBar,
	LPCRECT lpRect,
	BOOL bMovingEnabled,
	const POINT * ptDesiredMid // = NULL
	)
{
	ASSERT_VALID( this);
	ASSERT_VALID( pBar );
	ASSERT_KINDOF( CControlBar, pBar );

CRect rcBar;
	pBar->GetWindowRect( &rcBar );
	if(	pBar->m_pDockBar == this
		&& (lpRect == NULL || rcBar == *lpRect)
		)
	{
		// already docked and no change in position
		return;
	}

	// set CBRS_FLOAT_MULTI style if docking bar has it
	if(	m_bFloating
		&& (pBar->m_dwDockStyle & CBRS_FLOAT_MULTI)
		)
		m_dwStyle |= CBRS_FLOAT_MULTI;

	m_dwStyle &= ~(CBRS_SIZE_FIXED | CBRS_SIZE_DYNAMIC);
	m_dwStyle |=
		pBar->m_dwStyle & (CBRS_SIZE_FIXED | CBRS_SIZE_DYNAMIC);

	if( !(m_dwStyle & CBRS_FLOAT_MULTI) )
	{
		CString sTitle;
		pBar->GetWindowText( sTitle );
		AfxSetWindowText( m_hWnd, (LPCTSTR)sTitle );
	}

	// align correctly and turn on all borders
DWORD dwStyle = pBar->GetBarStyle();
	dwStyle &= ~(CBRS_ALIGN_ANY);
	dwStyle |=  (m_dwStyle & CBRS_ALIGN_ANY) | CBRS_BORDER_ANY;

	if( m_bFloating )
		dwStyle |= CBRS_FLOATING;
	else
		dwStyle &= ~CBRS_FLOATING;

	pBar->SetBarStyle( dwStyle );

	// hide first if changing to a new docking site to avoid flashing
bool bShow = false;
	if( bMovingEnabled &&
		pBar->m_pDockBar != this && pBar->IsWindowVisible() )
	{
		pBar->SetWindowPos(NULL, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_HIDEWINDOW);
		bShow = true;
	}

int nPos = -1;
	if( lpRect != NULL )
	{
		// insert into appropriate row
		CRect rc( lpRect );
		ScreenToClient( &rc );
		CPoint ptMid(
			(ptDesiredMid == NULL) ?
				rc.left + rc.Width()/2 : ptDesiredMid->x,
			(ptDesiredMid == NULL) ?
				rc.top + rc.Height()/2 : ptDesiredMid->y
			);
		nPos = _InsertByPointImpl( pBar, rc, ptMid );
		ASSERT_VALID( this );
		ASSERT_BAR_LOCATION_IN_ROW( this, pBar );

		// position at requested position
		if( bMovingEnabled )
			pBar->SetWindowPos(
				NULL,
				rc.left, rc.top, rc.Width(), rc.Height(),
				SWP_NOREDRAW ////|SWP_NOSENDCHANGING 
					|SWP_NOZORDER|SWP_NOOWNERZORDER
					|SWP_NOACTIVATE|SWP_NOCOPYBITS
				);
	}
	else
	{
		// always add on current row, then create new one
		m_arrBars.Add(pBar);
		m_arrBars.Add(NULL);
		ASSERT_VALID( this );
		ASSERT_BAR_LOCATION_IN_ROW( this, pBar );

		// align off the edge initially
		if( bMovingEnabled )
			pBar->SetWindowPos(
				NULL,
				-afxData.cxBorder2, -afxData.cyBorder2,
				0, 0,
				SWP_NOREDRAW ////|SWP_NOSENDCHANGING 
					|SWP_NOZORDER|SWP_NOOWNERZORDER
					|SWP_NOACTIVATE|SWP_NOCOPYBITS
					|SWP_NOSIZE
				);
	}

	// attach it to the docking site
	if( pBar->GetParent() != this )
		pBar->SetParent(this);
	if( pBar->m_pDockBar == this )
	{
		//pBar->m_pDockBar->
			RemoveControlBar(
				pBar,
				nPos,
				0,
				false // (!bMovingEnabled) ? true : false
				);
		ASSERT_VALID( this );
	} // if( pBar->m_pDockBar == this )
	else if( pBar->m_pDockBar != NULL )
	{
		ASSERT_KINDOF( CDockBar, pBar->m_pDockBar );
		if( pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
		{
			bool bOptimizeDockBarLayout = false;
			if( !((CExtDockBar *)(pBar->m_pDockBar))->m_bFloating )
				bOptimizeDockBarLayout = true;
			((CExtDockBar *)(pBar->m_pDockBar))->
				RemoveControlBar(
					pBar,
					-1,
					m_bFloating && !pBar->m_pDockBar->m_bFloating,
					false // (!bMovingEnabled) ? true : false
					);
			ASSERT_VALID( this );
			if( bOptimizeDockBarLayout )
				((CExtDockBar *)(pBar->m_pDockBar))->OnDynamicLayoutOptimize();
		}
		else
		{
			pBar->m_pDockBar->
				RemoveControlBar(
					pBar,
//ph//
					0, //-1,
					m_bFloating && !pBar->m_pDockBar->m_bFloating
					);
			ASSERT_VALID( this );
		}
	} // else if( pBar->m_pDockBar != NULL )

	pBar->m_pDockBar = this;
	ASSERT( pBar->m_pDockContext != NULL );
	pBar->m_pDockContext->m_uMRUDockID = GetDlgCtrlID();
	if( lpRect != NULL )
		pBar->m_pDockContext->m_rectMRUDockPos = *lpRect;

	if( bShow )
	{
		ASSERT(!pBar->IsWindowVisible());
		//pBar->SetWindowPos(NULL, 0, 0, 0, 0,
		//	SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);
		pBar->SetWindowPos(NULL,
			lpRect->left,
			lpRect->top,
			lpRect->right-lpRect->left,
			lpRect->bottom-lpRect->top,
			SWP_NOSIZE/*|SWP_NOMOVE*/|SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW);
	}

	// remove any place holder for pBar in this dockbar
	RemovePlaceHolder( pBar, true );
	ASSERT_VALID( this );

//	// get parent frame for recalc layout
//CFrameWnd* pFrameWnd = GetDockingFrame();
//	pFrameWnd->DelayRecalcLayout();

	_OptimizeCircles();
}

void CExtDockBar::_NewRowDockControlBar(
	CControlBar * pExtBarDocked,
	CControlBar * pExtBarNew,
	bool bLessIndex
	)
{
	ASSERT_VALID( this);
	ASSERT_VALID( pExtBarDocked );
	ASSERT_KINDOF( CControlBar, pExtBarDocked );
	ASSERT_VALID( pExtBarNew );
	ASSERT_KINDOF( CControlBar, pExtBarNew );

CFrameWnd * pNewDockingFrameWnd = pExtBarNew->GetDockingFrame();
INT nNewDockBarGetDockedVisibleCount = -1;
BOOL bNewFloating = pExtBarNew->IsFloating();

BOOL bNewExt = pExtBarNew->IsKindOf( RUNTIME_CLASS(CExtControlBar) );

	if( pExtBarNew->GetParent() != this )
	{
		if( bNewExt )
			((CExtControlBar *)pExtBarNew)->m_bUpdatingChain = true;
		pExtBarNew->SetWindowPos(
			NULL, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE
			|SWP_HIDEWINDOW
			);
		pExtBarNew->SetParent( this );
		if( bNewExt )
			((CExtControlBar *)pExtBarNew)->m_bUpdatingChain = false;
	}

	// align correctly and turn on all borders
DWORD dwStyle = pExtBarNew->GetBarStyle();
	dwStyle &= ~(CBRS_ALIGN_ANY);
	dwStyle |=  (m_dwStyle & CBRS_ALIGN_ANY) | CBRS_BORDER_ANY;

	if( m_bFloating )
		dwStyle |= CBRS_FLOATING;
	else
		dwStyle &= ~CBRS_FLOATING;

	pExtBarNew->SetBarStyle( dwStyle );

INT nCountOfSideBars = (INT)m_arrBars.GetSize();
CControlBar * pRemoveBar = NULL;
HWND hWndDockBarOld = NULL;
	if( pExtBarNew->m_pDockBar != this )
	{
		if( pExtBarNew->m_pDockBar != NULL )
		{
			nNewDockBarGetDockedVisibleCount =
				GetDockedVisibleCount();
			ASSERT_VALID( pExtBarNew->m_pDockBar );
			ASSERT_KINDOF( CDockBar, pExtBarNew->m_pDockBar );
			if( pExtBarNew->IsWindowVisible() )
				pExtBarNew->SetWindowPos(
					NULL, 0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE
					|SWP_HIDEWINDOW
					);
			ASSERT_VALID( pExtBarNew->m_pDockBar );

			if( !pExtBarNew->m_pDockBar->m_bFloating
				&& pExtBarNew->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar))
				)
			{
hWndDockBarOld = pExtBarNew->m_pDockBar->GetSafeHwnd();
				((CExtDockBar *)pExtBarNew->m_pDockBar)->RemoveControlBar( pExtBarNew );
// optimized at final state with all circles
//				((CExtDockBar *)pExtBarNew->m_pDockBar)->OnDynamicLayoutOptimize();
			}
			else		
				pExtBarNew->m_pDockBar->RemoveControlBar( pExtBarNew );

			//ASSERT_VALID( pExtBarNew->m_pDockBar ); this may be already destroyed floating dockbar
		} // if( pExtBarNew->m_pDockBar != NULL )
	} // if( pExtBarNew->m_pDockBar != this )
	else
	{
		if( pExtBarDocked != pExtBarNew )
		{
			INT nPos = FindBar( pExtBarNew );
			ASSERT( nPos >= 0 && nPos < nCountOfSideBars );
			m_arrBars.RemoveAt( nPos );
			nCountOfSideBars --;
			ASSERT( nCountOfSideBars > 1 );
			if(   (	nCountOfSideBars == nPos
					&& m_arrBars[nPos-1] == NULL )
				||
				  (	nCountOfSideBars > nPos
					&& m_arrBars[nPos-1] == NULL
					&& m_arrBars[nPos] == NULL
					)
				)
			{
				// remove empty row
				m_arrBars.RemoveAt( nPos-1 );
				nCountOfSideBars --;
			}
			ASSERT_VALID( this );
		} // if( pExtBarDocked != pExtBarNew )
		else
			pRemoveBar = pExtBarNew;
	} // else from if( pExtBarNew->m_pDockBar != this )

INT nIdxDockedBar = FindBar( pExtBarDocked );
	ASSERT( nIdxDockedBar >= 0 && nIdxDockedBar < nCountOfSideBars );

INT nIncrement = bLessIndex ? -1 : 1;
INT nIdx = 0;
	for( nIdx = nIdxDockedBar; true; nIdx += nIncrement )
	{
		CControlBar * pBar = (CControlBar *) m_arrBars[ nIdx ];
		if( nIdx == 0
			|| nIdx == (nCountOfSideBars-1)
			|| pBar == NULL
			)
		{
			ASSERT( m_arrBars[0] == NULL );
			m_arrBars.InsertAt( nIdx, pExtBarNew );
			m_arrBars.InsertAt( nIdx, (CControlBar *)NULL );
			ASSERT_VALID( this );
			break;
		}

#ifdef _DEBUG
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CControlBar, pBar );
#endif // _DEBUG

	} // for( nIdx = nIdxDockedBar; true; nIdx += nIncrement )

CDockBar * pOldDockBar = pExtBarNew->m_pDockBar;

	pExtBarNew->m_pDockBar = this;
	ASSERT_BAR_LOCATION_IN_ROW( this, pExtBarNew );
	
	ASSERT( pExtBarNew->m_pDockContext != NULL );
	pExtBarNew->m_pDockContext->m_uMRUDockID = GetDlgCtrlID();

	if( pRemoveBar != NULL )
	{
		INT nPos = FindBar( pRemoveBar, nIdx+1 );
		nCountOfSideBars = (INT)m_arrBars.GetSize();
		ASSERT( nPos >= 0 && nPos < nCountOfSideBars );
		m_arrBars.RemoveAt( nPos );
		nCountOfSideBars --;
		ASSERT( nCountOfSideBars > 1 );
		if(   (	nCountOfSideBars == nPos
				&& m_arrBars[nPos-1] == NULL )
			||
			  (	nCountOfSideBars > nPos
				&& m_arrBars[nPos-1] == NULL
				&& m_arrBars[nPos] == NULL
				)
			)
		{
			// remove empty row
			m_arrBars.RemoveAt( nPos-1 );
//			nCountOfSideBars --;
		}
		ASSERT_VALID( this );
	}

	if( bNewFloating && nNewDockBarGetDockedVisibleCount == 0 )
	{
		if( pOldDockBar->GetDockedCount() == 0 )
			pNewDockingFrameWnd->DestroyWindow();
		else
			pNewDockingFrameWnd->ShowWindow( SW_HIDE );
	}
//	else
//		pNewDockingFrameWnd->DelayRecalcLayout();

//	ASSERT( !pExtBarNew->IsWindowVisible() );
	pExtBarNew->SetWindowPos(NULL, 0, 0, 0, 0,
		SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE
			|SWP_SHOWWINDOW|SWP_FRAMECHANGED
		);

	OnDynamicLayoutUpdate();
	if( hWndDockBarOld != NULL
		&& hWndDockBarOld != GetSafeHwnd()
		&& ::IsWindow(hWndDockBarOld)
		)
	{
		CWnd * pWndDockBar = FromHandlePermanent( hWndDockBarOld );
		if( pWndDockBar != NULL
			&& pWndDockBar->GetSafeHwnd() == hWndDockBarOld
			)
		{
			ASSERT( pWndDockBar != this );
			ASSERT_KINDOF( CExtDockBar, pWndDockBar );
			((CExtDockBar *)pWndDockBar)->OnDynamicLayoutUpdate();
		}
	}

CFrameWnd * pFrame = GetParentFrame();
	ASSERT_VALID( pFrame );
	if( !pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) )
		_OptimizeCircles();
}

void CExtDockBar::_InnerOuterDockControlBar(
	CControlBar * pBar,
	bool bInner,
	bool bOptimize // = true
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
	ASSERT_KINDOF( CControlBar, pBar );

UINT nOwnID = GetDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nOwnID );

bool bDockAtStartIdx = false;
	if( nOwnID == AFX_IDW_DOCKBAR_RIGHT
		|| nOwnID == AFX_IDW_DOCKBAR_BOTTOM
		)
		bDockAtStartIdx = true;
	if( !bInner )
		bDockAtStartIdx = false;

	if( pBar->m_pDockBar == this )
	{
		INT nCountOfBars = (INT)m_arrBars.GetSize();
		ASSERT( nCountOfBars > 2 );
		if( m_arrBars[1] == pBar )
			return;
		INT nBarPos = FindBar( pBar );
		ASSERT( nBarPos > 0 && nBarPos < nCountOfBars );
		m_arrBars.RemoveAt( nBarPos );
		nCountOfBars --;
		if( m_arrBars[nBarPos - 1] == NULL
			&& m_arrBars[ nBarPos ] == NULL
			)
		{
			m_arrBars.RemoveAt( nBarPos );
			nCountOfBars --;
		}

		//ASSERT_VALID( this );
		if( bDockAtStartIdx )
		{
			m_arrBars.InsertAt( 1, pBar );
			nCountOfBars++;
			ASSERT( nCountOfBars == m_arrBars.GetSize() );
			if( nCountOfBars < 3 || m_arrBars[2] != NULL )
				m_arrBars.InsertAt( 2, (CControlBar *)NULL );
			ASSERT_VALID( this );
		} // if( bDockAtStartIdx )
		else
		{
			m_arrBars.InsertAt( nCountOfBars, (CControlBar *)NULL );
			m_arrBars.InsertAt( nCountOfBars, pBar );
			ASSERT_VALID( this );
		} // else from if( bDockAtStartIdx )
	} // if( pBar->m_pDockBar == this )
	else
	{
		CFrameWnd * pDockingFrameWnd = pBar->GetDockingFrame();
		INT nDockBarGetDockedVisibleCount = -1;
		BOOL bFloating = pBar->IsFloating();

		ASSERT( pBar->GetParent() != this );
		pBar->SetWindowPos(
			NULL, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE
			|SWP_HIDEWINDOW
			);
		pBar->SetParent( this );

		// align correctly and turn on all borders
		DWORD dwStyle = pBar->GetBarStyle();
		dwStyle &= ~(CBRS_ALIGN_ANY);
		dwStyle |=  (m_dwStyle & CBRS_ALIGN_ANY) | CBRS_BORDER_ANY;

		if( m_bFloating )
			dwStyle |= CBRS_FLOATING;
		else
			dwStyle &= ~CBRS_FLOATING;

		pBar->SetBarStyle( dwStyle );

		ASSERT( pBar->m_pDockBar != this );
		if( pBar->IsWindowVisible() )
			pBar->SetWindowPos(
				NULL, 0, 0, 0, 0,
				SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE
				|SWP_HIDEWINDOW
				);

		if( pBar->m_pDockBar != NULL )
		{
			ASSERT_VALID( pBar->m_pDockBar );
			ASSERT_KINDOF( CDockBar, pBar->m_pDockBar );
			if(		(! pBar->m_pDockBar->m_bFloating )
				&&	pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar))
				)
			{
				((CExtDockBar *)pBar->m_pDockBar)->RemoveControlBar( pBar );
				if( bOptimize )
					((CExtDockBar *)pBar->m_pDockBar)->OnDynamicLayoutOptimize();
			}
			else
				pBar->m_pDockBar->RemoveControlBar( pBar );
			//ASSERT_VALID( pBar->m_pDockBar ); this may be already destroyed floating dockbar
		}
		//ASSERT_VALID( this );
		INT nCountOfBars = (INT)m_arrBars.GetSize();
		ASSERT( nCountOfBars > 0 );
		if( bDockAtStartIdx )
		{
			m_arrBars.InsertAt( 1, pBar );
			nCountOfBars++;
			if( nCountOfBars < 3 || m_arrBars[2] != NULL )
				m_arrBars.InsertAt( 2, (CControlBar *)NULL );
			ASSERT_VALID( this );
		} // if( bDockAtStartIdx )
		else
		{
			m_arrBars.InsertAt( nCountOfBars, (CControlBar *)NULL );
			m_arrBars.InsertAt( nCountOfBars, pBar );

			if(		nCountOfBars > 1 // (+ v.2.24) - fixes initial mutual sizes in row/col problem
			   &&	m_arrBars[nCountOfBars-1] == NULL
			   )
			   m_arrBars.RemoveAt( nCountOfBars-1, 1 );

			ASSERT_VALID( this );
		} // else from if( bDockAtStartIdx )

		CDockBar * pOldDockBar = pBar->m_pDockBar;
		pBar->m_pDockBar = this;
		ASSERT_BAR_LOCATION_IN_ROW( this, pBar );
		
		ASSERT( pBar->m_pDockContext != NULL );
		pBar->m_pDockContext->m_uMRUDockID = GetDlgCtrlID();

		if( bFloating && nDockBarGetDockedVisibleCount == 0 )
		{
			if( pOldDockBar->GetDockedCount() == 0 )
				pDockingFrameWnd->DestroyWindow();
			else
				pDockingFrameWnd->ShowWindow( SW_HIDE );
		}
//		else
//			pDockingFrameWnd->DelayRecalcLayout();

//		ASSERT( !pBar->IsWindowVisible() );
		pBar->SetWindowPos(NULL, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE
				|SWP_SHOWWINDOW|SWP_FRAMECHANGED
			);

		ASSERT_VALID( this );
	} // else from if( pBar->m_pDockBar == this )

	if( bOptimize )
		_OptimizeCircles();
}

void CExtDockBar::_LockSequenceOptimization( bool bLock )
{
	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtDockBar, this );
	m_bLockedOptimize = bLock;
}

void CExtDockDynBar::_LockSequenceOptimization( bool bLock )
{
	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtDockDynBar, this );
	m_bLockedOptimize = bLock;
CExtDynControlBar * pDynBar =
		STATIC_DOWNCAST(
			CExtDynControlBar,
			GetParent()
			);
	ASSERT( pDynBar->m_pWndDynDocker == this );
	ASSERT_VALID( pDynBar->m_pDockBar );
	if(  pDynBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
		((CExtDockBar *)pDynBar->m_pDockBar)->_LockSequenceOptimization( bLock );
}

void CExtDockBar::DockControlBar(
	CControlBar * pBar,
	LPCRECT lpRect, // = NULL
	bool bSmoothMode, // = false
	bool bForceNoOptimize // = false
	)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pBar);
	ASSERT_KINDOF(CControlBar, pBar);
CRect rectBar;
	pBar->GetWindowRect( &rectBar );
	if(		pBar->m_pDockBar == this
		&&	(lpRect == NULL || rectBar == *lpRect)
		)
		return; // already docked and no change in position

	// set CBRS_FLOAT_MULTI style if docking bar has it
	if( m_bFloating && (pBar->m_dwDockStyle & CBRS_FLOAT_MULTI) )
		m_dwStyle |= CBRS_FLOAT_MULTI;

	m_dwStyle &= ~(CBRS_SIZE_FIXED | CBRS_SIZE_DYNAMIC);
	m_dwStyle |= pBar->m_dwStyle & (CBRS_SIZE_FIXED | CBRS_SIZE_DYNAMIC);

	if( ( m_dwStyle & CBRS_FLOAT_MULTI ) != 0 )
	{
		CString sTitle;
		pBar->GetWindowText( sTitle );
		AfxSetWindowText( m_hWnd, (LPCTSTR)sTitle );
	}

	// align correctly and turn on all borders
DWORD dwStyle = pBar->GetBarStyle();
	dwStyle &= ~(CBRS_ALIGN_ANY);
	dwStyle |=  (m_dwStyle & CBRS_ALIGN_ANY) | CBRS_BORDER_ANY;

	if( m_bFloating )
		dwStyle |= CBRS_FLOATING;
	else
		dwStyle &= ~CBRS_FLOATING;

	pBar->SetBarStyle(dwStyle);

	// hide first if changing to a new docking site to avoid flashing
BOOL bShow = FALSE;
	if( pBar->m_pDockBar != this && pBar->IsWindowVisible() )
	{
		pBar->SetWindowPos(
			NULL,
			0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_HIDEWINDOW
			);
		bShow = TRUE;
	}

int nPos = -1;
	if( lpRect != NULL )
	{
		// insert into appropriate row
		CRect rect(lpRect);
		ScreenToClient(&rect);
		CPoint ptMid(rect.left + rect.Width()/2, rect.top + rect.Height()/2);
		//nPos = Insert(pBar, rect, ptMid);
		nPos = CDockBar::Insert(pBar, rect, ptMid);

		ASSERT( nPos > 0 && nPos < (m_arrBars.GetSize()-1) );
		// adjust with the previous full-row-mode neiborhood
		// based on the fixed-style docking algorithm
		CControlBar * pBarNeiborhood =
			(CControlBar *)(m_arrBars[nPos-1]);
		if(		pBarNeiborhood != NULL
			&&	(! (__PLACEHODLER_BAR_PTR(pBarNeiborhood)) )
			)
		{
			ASSERT_VALID( pBarNeiborhood );
			CExtControlBar * pExtBarNeiborhood =
				DYNAMIC_DOWNCAST( CExtControlBar, pBarNeiborhood );
			if(		pExtBarNeiborhood != NULL
				&&	pExtBarNeiborhood->IsFixedDockStyle()
				&&	pExtBarNeiborhood->_GetFullRowMode()
				)
				m_arrBars.InsertAt( nPos++, (CControlBar *)NULL );
		}
		// adjust with the next full-row-mode neiborhood
		// based on the fixed-style docking algorithm
		pBarNeiborhood =
			(CControlBar *)(m_arrBars[nPos+1]);
		if(		pBarNeiborhood != NULL
			&&	(! (__PLACEHODLER_BAR_PTR(pBarNeiborhood)) )
			)
		{
			ASSERT_VALID( pBarNeiborhood );
			CExtControlBar * pExtBarNeiborhood =
				DYNAMIC_DOWNCAST( CExtControlBar, pBarNeiborhood );
			if(		pExtBarNeiborhood != NULL
				&&	pExtBarNeiborhood->IsFixedDockStyle()
				&&	pExtBarNeiborhood->_GetFullRowMode()
				)
				m_arrBars.InsertAt( nPos+1, (CControlBar *)NULL );
		}

		// position at requested position
		pBar->SetWindowPos(NULL, rect.left, rect.top, rect.Width(),
			rect.Height(), SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOCOPYBITS);
	}
	else
	{
		// always add on current row, then create new one
		m_arrBars.Add(pBar);
		m_arrBars.Add(NULL);

		// align off the edge initially
		pBar->SetWindowPos(
			NULL,
			-afxData.cxBorder2, -afxData.cyBorder2, 0, 0,
			SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOCOPYBITS
			);
	}

	// attach it to the docking site
	if( pBar->GetParent() != this )
	{
		CExtControlBar * pExtBar = DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if(		pExtBar != NULL
			&&	pExtBar->IsFixedMode()
			&&	pExtBar->IsFixedDockStyle()
			)
		{
			CFrameWnd * pFrame = pExtBar->GetParentFrame();
			ASSERT_VALID( pFrame );
			if( pFrame != pExtBar->m_pDockSite )
			{
				ASSERT_KINDOF( CMiniDockFrameWnd, pFrame );
				pFrame->ShowWindow( SW_HIDE );
				CExtPaintManager::stat_PassPaintMessages();
			} // if( pFrame != pBar->m_pDockSite )
		}
		pBar->SetParent(this);
	}
	if( pBar->m_pDockBar == this )
		RemoveControlBar(pBar, nPos);
	else if( pBar->m_pDockBar != NULL )
	{

		ASSERT_KINDOF( CDockBar, pBar->m_pDockBar );
		if( pBar->m_pDockBar->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
		{
			bool bOptimizeDockBarLayout = false;
			if( !((CExtDockBar *)(pBar->m_pDockBar))->m_bFloating )
				bOptimizeDockBarLayout = true;
			((CExtDockBar *)(pBar->m_pDockBar))->RemoveControlBar( pBar, -1, m_bFloating && (!pBar->m_pDockBar->m_bFloating), false );
			ASSERT_VALID( this );
			if( bOptimizeDockBarLayout && (!bForceNoOptimize) && ( IsKindOf(RUNTIME_CLASS(CExtDockBar)) && (! m_bLockedOptimize ) ) )
				((CExtDockBar *)(pBar->m_pDockBar))->OnDynamicLayoutOptimize();
		}
		else
		{
			pBar->m_pDockBar->RemoveControlBar( pBar, 0, m_bFloating && (!pBar->m_pDockBar->m_bFloating) );
			ASSERT_VALID( this );
		}
	}
	pBar->m_pDockBar = this;

	if( bShow )
	{
		ASSERT(!pBar->IsWindowVisible());
		pBar->SetWindowPos(
			NULL,
			0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_SHOWWINDOW
			);
	}
	else if( bSmoothMode && lpRect != NULL )
	{
		CRect rcSmooth( *lpRect );
		ScreenToClient( &rcSmooth );
		pBar->SetWindowPos(
			NULL,
			rcSmooth.left, rcSmooth.top,
			rcSmooth.Width(), rcSmooth.Height(),
			SWP_NOZORDER|SWP_NOACTIVATE
				////|SWP_NOSENDCHANGING 
				|SWP_NOREDRAW
			);
	} // else if( bSmoothMode && lpRect != NULL )

	// remove any place holder for pBar in this dockbar
	if( (! m_bFloating )  )
		RemovePlaceHolder( pBar, true );

	ASSERT_VALID( this );
	if(		pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
		&&	((CExtControlBar*)pBar)->_GetFullRowMode()
		)
	{
		nPos = FindBar( pBar );
		ASSERT( nPos > 0 && nPos < (m_arrBars.GetSize()-1) );
		if( m_arrBars[nPos+1] == NULL )
		{
			if( m_arrBars[nPos-1] != NULL )
			{
				m_arrBars.InsertAt( nPos, (CControlBar *)NULL );
				ASSERT_VALID( this );
			}
		} // if( m_arrBars[nPos+1] == NULL )
		else
		{
			if( m_arrBars[nPos-1] == NULL )
			{
				m_arrBars.InsertAt( nPos+1, (CControlBar *)NULL );
				ASSERT_VALID( this );
			}
			else
			{
				int nPos2 = nPos-1;
				for( ; m_arrBars[nPos2] != NULL; nPos2-- );
				ASSERT( nPos2 >= 0 );
				m_arrBars.RemoveAt( nPos );
				m_arrBars.InsertAt( nPos2, pBar );
				m_arrBars.InsertAt( nPos2, (CControlBar *)NULL );
				ASSERT_VALID( this );
			}
		} // else from if( m_arrBars[nPos+1] == NULL )
	}

	// get parent frame for recalc layout
CFrameWnd* pFrameWnd = GetDockingFrame();
	pFrameWnd->DelayRecalcLayout();

// OPTFIX:
//	if( (!bForceNoOptimize) && (! pFrameWnd->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) ) )
//		_OptimizeCircles();
}

void CExtDockBar::DrawBorders(CDC* pDC, CRect& rect)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pDC );

CExtPaintManager::PAINTCONTROLBARBORDERSDATA _pcbbd(
		this,
		CExtPaintManager::__CB_OUTER_DOCKBAR,
		m_dwStyle, // |CBRS_BORDER_ANY|CBRS_BORDER_3D
		rect
		);
	PmBridge_GetPM()->PaintControlBarBorders( *pDC, _pcbbd );
}

void CExtDockBar::DrawGripper(CDC* pDC, const CRect& rect)
{
	ASSERT_VALID( this );
	pDC,
	rect;
}

LRESULT CExtDockBar::OnSizeParent(WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID( this );
	wParam;
//	if( CExtControlBar::g_bUpdatingDragState )
//		return 0;

	if( m_bLockedOptimize )
		return 0;

LRESULT lResult = 0L;
AFX_SIZEPARENTPARAMS * lpLayout =
		(AFX_SIZEPARENTPARAMS *) lParam;
	ASSERT( lpLayout != NULL );
BOOL bLayoutQuery = m_bLayoutQuery;
CRect rectLayout = m_rectLayout;
	m_bLayoutQuery = (lpLayout->hDWP == NULL);
	m_rectLayout = lpLayout->rect;
DWORD dwStyle = RecalcDelayShow( lpLayout );
	if(		( (dwStyle&WS_VISIBLE) != 0 )
		&&	( (dwStyle&CBRS_ALIGN_ANY) != 0 )
		)
	{
		CRect rect = lpLayout->rect;
		CSize sizeAvail = rect.Size();
		DWORD dwMode = lpLayout->bStretch ? LM_STRETCH : 0;
		if(		( (m_dwStyle&CBRS_SIZE_DYNAMIC) != 0 )
			&&	( (m_dwStyle&CBRS_FLOATING ) != 0 )
			)
			dwMode |= LM_HORZ|LM_MRUWIDTH;
		else if( (dwStyle&CBRS_ORIENT_HORZ) != 0 )
			dwMode |= LM_HORZ|LM_HORZDOCK;
		else
			dwMode |=  LM_VERTDOCK;
		CSize size = CalcDynamicLayout( -1, dwMode );
		size.cx = min( size.cx, sizeAvail.cx );
		size.cy = min( size.cy, sizeAvail.cy );
		if( (dwStyle&CBRS_ORIENT_HORZ) != 0 )
		{
			lpLayout->sizeTotal.cy += size.cy;
			lpLayout->sizeTotal.cx = max( lpLayout->sizeTotal.cx, size.cx );
			if(	(dwStyle&CBRS_ALIGN_TOP) != 0 )
				lpLayout->rect.top += size.cy;
			else if( (dwStyle&CBRS_ALIGN_BOTTOM) != 0 )
			{
				rect.top = rect.bottom - size.cy;
				lpLayout->rect.bottom -= size.cy;
			}
		}
		else if( (dwStyle&CBRS_ORIENT_VERT) != 0 )
		{
			lpLayout->sizeTotal.cx += size.cx;
			lpLayout->sizeTotal.cy = max( lpLayout->sizeTotal.cy, size.cy );
			HWND hWndParent = ::GetParent( m_hWnd );
			ASSERT( hWndParent != NULL && ::IsWindow(hWndParent) );
			__EXT_MFC_LONG_PTR dwStyleEx = ::__EXT_MFC_GetWindowLong( hWndParent, GWL_EXSTYLE );
			bool bParentRTL = ( (dwStyleEx&WS_EX_LAYOUTRTL) != 0 ) ? true : false;
			if( bParentRTL )
			{
				if( (dwStyle&CBRS_ALIGN_RIGHT) != 0 )
					lpLayout->rect.left += size.cx;
				else if( (dwStyle&CBRS_ALIGN_LEFT) != 0 )
				{
					rect.left = rect.right - size.cx;
					lpLayout->rect.right -= size.cx;
				}
			} // if( bParentRTL )
			else
			{
				if( (dwStyle&CBRS_ALIGN_LEFT) != 0 )
					lpLayout->rect.left += size.cx;
				else if( (dwStyle&CBRS_ALIGN_RIGHT) != 0 )
				{
					rect.left = rect.right - size.cx;
					lpLayout->rect.right -= size.cx;
				}
			} // else from if( bParentRTL )
		}
#ifdef _DEBUG
		else
		{
			ASSERT(FALSE);
		}
#endif // _DEBUG
		rect.right = rect.left + size.cx;
		rect.bottom = rect.top + size.cy;
		if( lpLayout->hDWP != NULL )
			AfxRepositionWindow( lpLayout, m_hWnd, &rect );
	}
	m_bLayoutQuery = bLayoutQuery;
	m_rectLayout = rectLayout;

	if( (m_dwStyle & CBRS_ALIGN_ANY) == 0 )
		return lResult;
UINT nOwnID = GetDlgCtrlID();
	if( nOwnID == AFX_IDW_DOCKBAR_FLOAT )
		return lResult;
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nOwnID );
	if( lpLayout->hDWP != NULL )
		Invalidate();
	m_rcLastInvisiblePreCalc = lpLayout->rect;
	if(		nOwnID == AFX_IDW_DOCKBAR_LEFT
		||	nOwnID == AFX_IDW_DOCKBAR_RIGHT
		)
	{
		HWND hWndParent = ::GetParent( m_hWnd );
		ASSERT( hWndParent != NULL && ::IsWindow(hWndParent) );
		__EXT_MFC_LONG_PTR dwStyleEx = ::__EXT_MFC_GetWindowLong( hWndParent, GWL_EXSTYLE );
		bool bParentRTL = ( (dwStyleEx&WS_EX_LAYOUTRTL) != 0 ) ? true : false;
		if( bParentRTL )
		{
			if( nOwnID == AFX_IDW_DOCKBAR_LEFT )
				nOwnID = AFX_IDW_DOCKBAR_RIGHT;
			else
				nOwnID = AFX_IDW_DOCKBAR_LEFT;
		} // if( bParentRTL )
	}
	switch( nOwnID )
	{
	case AFX_IDW_DOCKBAR_TOP:
		m_rcLastInvisiblePreCalc.bottom = m_rcLastInvisiblePreCalc.top;
		break;
	case AFX_IDW_DOCKBAR_BOTTOM:
		m_rcLastInvisiblePreCalc.top = m_rcLastInvisiblePreCalc.bottom;
		break;
	case AFX_IDW_DOCKBAR_LEFT:
		m_rcLastInvisiblePreCalc.right = m_rcLastInvisiblePreCalc.left;
		break;
	case AFX_IDW_DOCKBAR_RIGHT:
		m_rcLastInvisiblePreCalc.left = m_rcLastInvisiblePreCalc.right;
		break;
#ifdef _DEBUG
	default:
		ASSERT( FALSE );
		break;
#endif // _DEBUG
	} // switch( nOwnID )

CFrameWnd * pFrame = _GetDockingFrameImpl();
	ASSERT_VALID( pFrame );
	ASSERT_KINDOF( CFrameWnd, pFrame );
	ASSERT( !pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) );
	pFrame->ClientToScreen( &m_rcLastInvisiblePreCalc );

	return lResult;
}

BOOL CExtDockBar::RemoveControlBar(
	CControlBar * pBar,
	int nPosExclude, // -1
	int nAddPlaceHolder, // = 0
	bool bEnableFrameDelayRecalcLayout // = true
	)
{
//ph//
// temp
	if(	pBar->IsKindOf( RUNTIME_CLASS(CExtControlBar) )
		&& (! ((CExtControlBar*)pBar)->IsFixedDockStyle() )
		)
		nAddPlaceHolder = 0;

	ASSERT_VALID( this );
	ASSERT(
		nAddPlaceHolder == 1
		|| nAddPlaceHolder == 0
		|| nAddPlaceHolder == -1
		);
	ASSERT_VALID( this );
	ASSERT( pBar != NULL );
int nPos = FindBar( pBar, nPosExclude );
	ASSERT( nPos > 0 );

	if( nAddPlaceHolder == 1 )
	{
		m_arrBars[ nPos ] = (void*) (__EXT_MFC_UINT_PTR)
			_AfxGetDlgCtrlID( pBar->m_hWnd );
		// check for already existing place holder
		int nPosOld =
			FindBar(
				(CControlBar *) m_arrBars[ nPos ],
				nPos
				);
		if( nPosOld > 0 )
		{
			m_arrBars.RemoveAt( nPos );
			// remove section indicator (NULL) if nothing
			// else in section
			if( m_arrBars[ nPos - 1 ] == NULL
				&& m_arrBars[ nPos ] == NULL
				)
				m_arrBars.RemoveAt( nPos );
		} // if( nPosOld > 0 )
	} // if( nAddPlaceHolder == 1 )
	else
	{
		m_arrBars.RemoveAt( nPos );
		if( m_arrBars[nPos-1] == NULL
			&& m_arrBars[nPos] == NULL
			)
			m_arrBars.RemoveAt( nPos );

		// Remove any pre-existing place holders.
		if( nAddPlaceHolder != -1 )
			RemovePlaceHolder( pBar, true );
	} // else from if( nAddPlaceHolder == 1 )

	// don't do anything more in the shutdown case!
	if( pBar->m_pDockContext == NULL )
		return FALSE;

	// get parent frame for recalc layout/frame destroy
CFrameWnd * pFrameWnd = GetDockingFrame();
	if( m_bFloating && GetDockedVisibleCount() == 0 )
	{
		if( GetDockedCount() == 0 )
		{
			pFrameWnd->DestroyWindow();
			return TRUE; // Self-Destruct
		}
		else
			pFrameWnd->ShowWindow( SW_HIDE );
	} // if( m_bFloating && GetDockedVisibleCount() == 0 )
	else
	{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( IsKindOf(RUNTIME_CLASS(CExtDockDynTabBar)) )
		{
			CExtDynTabControlBar * pTabbedBar =
				STATIC_DOWNCAST(
					CExtDynTabControlBar,
					GetParent()
					);
			//pTabbedBar->SyncSwitcher();
			pTabbedBar->OnRepositionSingleChild();
		}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

		if( bEnableFrameDelayRecalcLayout )
			pFrameWnd->DelayRecalcLayout();
	} // else from if( m_bFloating && GetDockedVisibleCount() == 0 )

	return TRUE;
}

int CExtDockBar::Insert(
	CControlBar * pBarIns,
	CRect rect,
	CPoint ptMid
	)
{
	return _InsertByPointImpl( pBarIns, rect, ptMid );
}

int CExtDockBar::_InsertByPointImpl(
	CControlBar * pBarIns,
	CRect rect,
	CPoint ptMid
	)
{
	ASSERT_VALID( this );
	ASSERT( pBarIns != NULL );
	
//	ASSERT( pBarIns->m_pDockBar == this );

BOOL bExtBar = pBarIns->IsKindOf(RUNTIME_CLASS(CExtControlBar));
BOOL bExtFixedBar = FALSE;
	if( bExtBar )
		bExtFixedBar =  ((CExtControlBar *)pBarIns)->IsFixedMode();
BOOL bFixedBar = !bExtBar || bExtFixedBar;

INT nPos = 0, nPosInsAfter = -1, nWidth = 0, nTotalWidth = 0;
INT nCountOfSideBars = (INT)m_arrBars.GetSize();
BOOL bHorz = m_dwStyle & CBRS_ORIENT_HORZ;

CRect rcDockBarWnd;
	GetWindowRect( &rcDockBarWnd );
	if( rcDockBarWnd.PtInRect(ptMid) )
	{
		for( ; nPos < nCountOfSideBars; nPos++ )
		{
			CControlBar * pBar = GetDockedControlBar( nPos );
			if( pBar != NULL && pBar->IsVisible() )
			{
				CRect rcBar;
				pBar->GetWindowRect( &rcBar );
				//ScreenToClient( &rcBar );
				nWidth =
					max(
						nWidth,
						bHorz ? rcBar.Size().cy : rcBar.Size().cx - 1
						);
				if( bHorz
						? ( rect.left > rcBar.left )
						: ( rect.top  > rcBar.top  )
					)
				{
					//nPosInsAfter = nPos; // orig
					if( bFixedBar )
					{
						if( !pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
							nPosInsAfter = nPos;
						else if(
							((CExtControlBar *)pBar)->IsFixedMode()
							)
							nPosInsAfter = nPos;
					}
					else
						nPosInsAfter = nPos;
				}
				if( bFixedBar
					&& rcBar.PtInRect( ptMid )
					)
				{
					INT nDistUp = 0, nDistDown = 0;
					if( bHorz )
					{
						nDistUp = ptMid.y - rcBar.top;
						nDistDown = rcBar.bottom - ptMid.y;
					} // if( bHorz )
					else
					{
						nDistUp = ptMid.x - rcBar.left;
						nDistDown = rcBar.right - ptMid.x;
					} // else from if( bHorz )
					bool bNextRow = ( nDistUp < nDistDown ) ? false : true;
					CControlBar * pFirstBar =
						CExtControlBar::_GetFirstControlBarInRow(
							pBar,
							bNextRow
							);
					ASSERT_VALID( pFirstBar );
					nPosInsAfter = FindBar( pFirstBar );
					ASSERT( nPosInsAfter > 0 && nPosInsAfter < nCountOfSideBars );

					if( bNextRow )
					{
						m_arrBars.InsertAt( nPosInsAfter+1, pBarIns );
						m_arrBars.InsertAt( nPosInsAfter+1, (CObject*)NULL );
						ASSERT_VALID( this );
						ASSERT_BAR_LOCATION_IN_ROW( this, pBarIns );
						return nPosInsAfter+2;
					}
					else
					{
						m_arrBars.InsertAt( nPosInsAfter, (CObject*)NULL );
						m_arrBars.InsertAt( nPosInsAfter, pBarIns );
						ASSERT_VALID( this );
						ASSERT_BAR_LOCATION_IN_ROW( this, pBarIns );
						return nPosInsAfter+1;
					}


//					if( !bNextRow )
///						nPosInsAfter --;
//					break;
				}
			} // if( pBar != NULL && pBar->IsVisible() )
			else
			{
				if( pBar != NULL )
					continue;
				// end of row because pBar == NULL
				nTotalWidth += nWidth - afxData.cyBorder2;
				nWidth = 0;
				if( (bHorz ? ptMid.y : ptMid.x) < nTotalWidth )
				{
					if( nPos == 0 ) // first section
					{
						m_arrBars.InsertAt( 1, (CObject*)NULL );
						m_arrBars.InsertAt( 1, pBarIns );
						ASSERT_VALID( this );
						ASSERT_BAR_LOCATION_IN_ROW( this, pBarIns );
						return 1;
					} // if( nPos == 0 )
					else
					{
						if( !bFixedBar )
						{
							m_arrBars.InsertAt( nPos, pBarIns );
							ASSERT_VALID( this );
							ASSERT_BAR_LOCATION_IN_ROW( this, pBarIns );
						}
						//if( nPosInsAfter < 0 || !bFixedBar )
						if( nPosInsAfter < 0 )
							break;

						ASSERT( nPosInsAfter >= 0 && nPosInsAfter <= nCountOfSideBars );
						CControlBar * pBar2 = (CControlBar *)m_arrBars[ nPosInsAfter ];
						if( pBar2 == NULL )
							break;
						if(  __PLACEHODLER_BAR_PTR(pBar2) )
							break;
						CRect rcBar2;
						pBar2->GetWindowRect( &rcBar2 );

						INT nDistUp = 0, nDistDown = 0;
						if( bHorz )
						{
							nDistUp = ptMid.y - rcBar2.top;
							nDistDown = rcBar2.bottom - ptMid.y;
						} // if( bHorz )
						else
						{
							nDistUp = ptMid.x - rcBar2.left;
							nDistDown = rcBar2.right - ptMid.x;
						} // else from if( bHorz )
						bool bNextRow = ( nDistUp < nDistDown ) ? false : true;
						CControlBar * pFirstBar =
							CExtControlBar::_GetFirstControlBarInRow(
								pBar2,
								bNextRow
								);
						ASSERT_VALID( pFirstBar );
						nPosInsAfter = FindBar( pFirstBar );
						ASSERT( nPosInsAfter > 0 && nPosInsAfter < nCountOfSideBars );

						if( bNextRow )
						{
							m_arrBars.InsertAt( nPosInsAfter+1, pBarIns );
							m_arrBars.InsertAt( nPosInsAfter+1, (CObject*)NULL );
							ASSERT_VALID( this );
							ASSERT_BAR_LOCATION_IN_ROW( this, pBarIns );
							return nPosInsAfter+2;
						}
						else
						{
							m_arrBars.InsertAt( nPosInsAfter, (CObject*)NULL );
							m_arrBars.InsertAt( nPosInsAfter, pBarIns );
							ASSERT_VALID( this );
							ASSERT_BAR_LOCATION_IN_ROW( this, pBarIns );
							return nPosInsAfter+1;
						}

					} // else from if( nPos == 0 )
				}
//				nPosInsAfter = nPos;
			} // else from if( pBar != NULL && pBar->IsVisible() )

		} // for( ; nPos < nCountOfSideBars; nPos++ )
	} // if( rcDockBarWnd.PtInRect(ptMid) )

	if( nPosInsAfter < 0 )
	{
		INT nDistUp = 0, nDistDown = 0;
		if( bHorz )
		{
			nDistUp = ptMid.y - rcDockBarWnd.top;
			nDistDown = rcDockBarWnd.bottom - ptMid.y;
		} // if( bHorz )
		else
		{
			nDistUp = ptMid.x - rcDockBarWnd.left;
			nDistDown = rcDockBarWnd.right - ptMid.x;
		} // else from if( bHorz )
		bool bNextRow =
			( abs(nDistUp) < abs(nDistDown) )
			? false : true;
		nPosInsAfter = bNextRow ? (nCountOfSideBars-1) : 1;
	}

	ASSERT( nPosInsAfter < nCountOfSideBars );
	if( nPosInsAfter == nCountOfSideBars-1 )
		m_arrBars.InsertAt( nCountOfSideBars, (CObject*)NULL ); // upper bound NULL

	if( m_arrBars[ nPosInsAfter ] != NULL )
	{
		if( nPosInsAfter+1 < nCountOfSideBars
			&& m_arrBars[nPosInsAfter+1] != NULL
			)
			m_arrBars.InsertAt( nPosInsAfter+1, (CObject*)NULL );
	}

	m_arrBars.InsertAt( nPosInsAfter+1, pBarIns );

	for( nPos = nPosInsAfter; nPos > 0; nPos-- )
	{
		CControlBar * pBar = (CControlBar *)m_arrBars[nPos];
		if( pBar == NULL )
			break;
		if( __PLACEHODLER_BAR_PTR(pBar) )
		{
			pBar = _GetDockingFrameImpl()->GetControlBar( LOWORD( DWORD( (__EXT_MFC_DWORD_PTR) pBar ) ) );
			if( pBar == NULL )
				continue;
		}
	
		BOOL bPrevFixedCheck = TRUE;
		if( pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
			&& !((CExtControlBar *)pBar)->IsFixedMode()
			)
			bPrevFixedCheck = FALSE;
		if( bFixedBar != bPrevFixedCheck )
		{
			m_arrBars.InsertAt( nPosInsAfter+1, (CObject*)NULL );
			nPosInsAfter++;
		}

		break;
	} // for( INT nPos = nPosInsAfter; nPos > 0; nPos-- )
	
//	ASSERT( FindBar(pBarIns) == (nPosInsAfter+1) );
	ASSERT_VALID( this );
	ASSERT_BAR_LOCATION_IN_ROW( this, pBarIns );

	return nPosInsAfter+1;
}

#ifdef _DEBUG

void CExtDockBar::_AssertValid_CheckBarRows() const
{

	ASSERT( this != NULL );
	if( m_hWnd == NULL )
		return;
	if( ! ::IsWindow(m_hWnd) )
		return;

CRect rcMyWnd;
	GetWindowRect( &rcMyWnd );
CSize sizeMyWnd = rcMyWnd.Size();
	if( sizeMyWnd.cx < 40 || sizeMyWnd.cy < 40 )
		return;

INT nCountOfSideBars = (INT)m_arrBars.GetSize();
	ASSERT( nCountOfSideBars >= 0 );
	if( nCountOfSideBars > 0 )
	{
		// first should be NULL
		CControlBar * pFirstPosBar = (CControlBar *) m_arrBars[ 0 ];
		ASSERT( pFirstPosBar == NULL );
		if( m_nCircleNo == 0 )
			return;
		// walk all rows and verify no empty rows
		INT nRowEntryCount = 0; // including placeholders
		INT nBar = 1;
		if( nCountOfSideBars > 2  )
		{
			for( ; nBar < nCountOfSideBars; nBar++ )
			{
				CControlBar * pBar = (CControlBar *) m_arrBars[ nBar ];
				if( pBar == NULL )
				{
					ASSERT( nRowEntryCount > 0 );
					nRowEntryCount = 0;
					continue;
				}
				nRowEntryCount++;
			} // for( ; nBar < nCountOfSideBars; nBar++ )
		}
	}
}

void CExtDockBar::_AssertValid_FixedBarNotInNonfixedRow(
	CDockBar * pDockBar,
	CControlBar * pBar
	)
{
	// verify bar and its dockbar
	ASSERT_VALID( pBar );
	ASSERT_KINDOF( CControlBar, pBar->m_pDockBar );
	ASSERT( !pBar->IsDockBar() );
	ASSERT( !pBar->IsKindOf(RUNTIME_CLASS(CDockBar)) );
//	ASSERT( !pBar->IsFloating() );
	
//	ASSERT( pBar->m_pDockBar != NULL );
//	ASSERT_VALID( pBar->m_pDockBar );
//	ASSERT_KINDOF( CExtDockBar, pBar->m_pDockBar );
//CExtDockBar * pDockBar = (CExtDockBar *)pBar->m_pDockBar;

	ASSERT_VALID( pDockBar );
	ASSERT_KINDOF( CDockBar, pDockBar );

bool bCheckingFixedBar = true;
	// verify bar is not a kind of resizable bar
	if( pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
	{
		CExtControlBar * pExtBar = (CExtControlBar *)pBar;
		if( !pExtBar->IsFixedDockStyle() )
			bCheckingFixedBar = false;
	}

INT nCountOfSideBars = (INT)pDockBar->m_arrBars.GetSize();
	ASSERT( nCountOfSideBars > 0 );
INT nBarPosInDockBar = pDockBar->FindBar( pBar );
	ASSERT( nBarPosInDockBar > 0 && nBarPosInDockBar < nCountOfSideBars );

	// walk to begin of row
	INT nBar = nBarPosInDockBar-1;
	for( ; nBar > 0; nBar-- )
	{
		CControlBar * pBarExamine = (CControlBar *)
			pDockBar->m_arrBars[ nBar ];
		if( pBarExamine == NULL )
			break; // ok
		if( __PLACEHODLER_BAR_PTR(pBarExamine) )
			continue;
		ASSERT_VALID( pBarExamine );
		ASSERT_KINDOF( CControlBar, pBarExamine );
		ASSERT( pBarExamine->m_pDockBar == pDockBar );
		
		bool bFixedBar = true;
		if( pBarExamine->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
		{
			CExtControlBar * pExtBar = (CExtControlBar *)pBarExamine;
			//ASSERT( pExtBar->IsFixedMode() );
			if( !pExtBar->IsFixedDockStyle() )
				bFixedBar = false;
		}
		ASSERT( bCheckingFixedBar == bFixedBar );
	}
	
	// walk to end of row
	for( nBar = nBarPosInDockBar+1; nBar < nCountOfSideBars; nBar++ )
	{
		CControlBar * pBarExamine = (CControlBar *)
			pDockBar->m_arrBars[ nBar ];
		if( pBarExamine == NULL )
			break; // ok
		if( __PLACEHODLER_BAR_PTR(pBarExamine) )
			continue;
		ASSERT_VALID( pBarExamine );
		ASSERT_KINDOF( CControlBar, pBarExamine );
		ASSERT( pBarExamine->m_pDockBar == pDockBar );

		bool bFixedBar = true;
		if( pBarExamine->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
		{
			CExtControlBar * pExtBar = (CExtControlBar *)pBarExamine;
			//ASSERT( pExtBar->IsFixedMode() );
			if( !pExtBar->IsFixedDockStyle() )
				bFixedBar = false;
		}
		ASSERT( bCheckingFixedBar == bFixedBar );
	}
	
}

#endif // _DEBUG

void CExtDockBar::OnNcCalcSize(
	BOOL bCalcValidRects,
	NCCALCSIZE_PARAMS * lpncsp
	)
{
	ASSERT_VALID( this );
	bCalcValidRects;
	lpncsp;

#if (defined __DEBUG_PAINTING_AREAS_DOCKBAR__ )

CRect & rcClientLoc =
		reinterpret_cast < CRect & > ( lpncsp->rgrc[0] );
	rcClientLoc.DeflateRect( 0, 0, 1, 1 );

#endif // __DEBUG_PAINTING_AREAS_DOCKBAR__
}

void CExtDockBar::OnNcPaint()
{
	ASSERT_VALID( this );

#if (defined __DEBUG_PAINTING_AREAS_DOCKBAR__ )

CRect rcBarWnd, rcBarClient;
	GetWindowRect( &rcBarWnd );
	GetClientRect( &rcBarClient );
	ClientToScreen( &rcBarClient );
	if( rcBarWnd == rcBarClient )
		return;
CPoint ptDevOffset = -rcBarWnd.TopLeft();
	rcBarWnd.OffsetRect( ptDevOffset );
	rcBarClient.OffsetRect( ptDevOffset );

CWindowDC dcBarWnd( this );
	ASSERT( dcBarWnd.GetSafeHdc() != NULL );
	dcBarWnd.ExcludeClipRect( &rcBarClient );

BOOL bDynamicDockBar = IsKindOf( RUNTIME_CLASS(CExtDockDynBar) );

COLORREF clrDebugMargin = bDynamicDockBar
		? RGB( 255, 255, 0 )
		: RGB( 255, 0, 255 );

	dcBarWnd.FillSolidRect( &rcBarWnd, clrDebugMargin );

#endif // __DEBUG_PAINTING_AREAS_DOCKBAR__

}

CExtDockBar * CExtDockBar::_GetInCircle(
	UINT nDockBarID,
	bool bEnableCreateNew // = true
	)
{
	ASSERT_VALID( this );
UINT nOwnID = GetDlgCtrlID();
	if( nDockBarID == nOwnID )
		return this;
CFrameWnd * pFrame = GetParentFrame();
	ASSERT_VALID( pFrame );
	ASSERT( !pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) );
	return
		_GetInCircle(
			pFrame,
			m_nCircleNo,
			nDockBarID,
			bEnableCreateNew
			);
}

CExtDockBar * CExtDockBar::_GetInCircle(
	CFrameWnd * pFrame,
	UINT nCircleNo,
	UINT nDockBarID,
	bool bEnableCreateNew // = true
	)
{
	ASSERT_VALID( pFrame );
POSITION pos = pFrame->m_listControlBars.GetHeadPosition();
	ASSERT( pos != NULL );
	while( pos != NULL )
	{
		CControlBar * pBar = (CControlBar *)
			pFrame->m_listControlBars.GetNext( pos );
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CControlBar, pBar );
		if( !pBar->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
			continue;
		UINT nID = pBar->GetDlgCtrlID();
		if( nID != nDockBarID )
			continue;
		UINT nCircleNo2 = ((CExtDockBar *)pBar)->_GetCircleNo();
		if( nCircleNo == nCircleNo2 )
			return ((CExtDockBar *)pBar);
	} // while( pos != NULL )
	if( !bEnableCreateNew )
		return NULL;
	_CreateInnerCircle( pFrame );
CExtDockBar * pDockBar =
		_GetInCircle(
			pFrame,
			nCircleNo,
			nDockBarID,
			bEnableCreateNew
			);
	ASSERT_VALID( pDockBar );
	return pDockBar;
}

UINT CExtDockBar::_CreateInnerCircle()
{
	ASSERT_VALID( this );
	if( _GetDockBarInner() != NULL )
	{
		ASSERT_VALID( _GetDockBarInner() );
		return ( _GetCircleNo() + 1 );
	}
CFrameWnd * pFrame = GetParentFrame();
	ASSERT_VALID( pFrame );
	ASSERT( !pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) );
	return _CreateInnerCircle( pFrame );
}

UINT CExtDockBar::_CreateInnerCircle( CFrameWnd * pFrame )
{
	ASSERT_VALID( pFrame );
	ASSERT( !pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) );

UINT nCircleNo = 0;
CExtDockBar * vInnerCircle[4] = { NULL, NULL, NULL, NULL };
CExtDockBar * vCreatingCircle[4] = { NULL, NULL, NULL, NULL };

POSITION pos = pFrame->m_listControlBars.GetHeadPosition();
	ASSERT( pos != NULL );
	while( pos != NULL )
	{
		CControlBar * pBar = (CControlBar *)
			pFrame->m_listControlBars.GetNext( pos );
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CControlBar, pBar );
		if( !pBar->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
			continue;
		if( pBar->IsKindOf(RUNTIME_CLASS(CExtDockDynBar)) )
			continue;
		UINT nCircleNo2 = ((CExtDockBar *)pBar)->_GetCircleNo();
		if( nCircleNo2 < nCircleNo )
			continue;
		nCircleNo = nCircleNo2;
		UINT nID = pBar->GetDlgCtrlID();
		//if( nID == AFX_IDW_DOCKBAR_FLOAT )
		//	continue;
		ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nID );
		switch( nID )
		{
		case AFX_IDW_DOCKBAR_TOP:
			vInnerCircle[0] = ((CExtDockBar *)pBar);
			break;
		case AFX_IDW_DOCKBAR_BOTTOM:
			vInnerCircle[1] = ((CExtDockBar *)pBar);
			break;
		case AFX_IDW_DOCKBAR_LEFT:
			vInnerCircle[2] = ((CExtDockBar *)pBar);
			break;
		case AFX_IDW_DOCKBAR_RIGHT:
			vInnerCircle[3] = ((CExtDockBar *)pBar);
			break;
#ifdef _DEBUG
		default:
			ASSERT( FALSE );
			break;
#endif // _DEBUG
		} // switch( nID )
	} // while( pos != NULL )

	ASSERT( vInnerCircle[0] != NULL );
	ASSERT( vInnerCircle[1] != NULL );
	ASSERT( vInnerCircle[2] != NULL );
	ASSERT( vInnerCircle[3] != NULL );

#ifdef _DEBUG
	if( vInnerCircle[0]->m_pDockBarInner == NULL )
	{
		ASSERT( vInnerCircle[1]->m_pDockBarInner == NULL );
		ASSERT( vInnerCircle[2]->m_pDockBarInner == NULL );
		ASSERT( vInnerCircle[3]->m_pDockBarInner == NULL );
	}
	else
	{
		ASSERT( vInnerCircle[1]->m_pDockBarInner != NULL );
		ASSERT( vInnerCircle[2]->m_pDockBarInner != NULL );
		ASSERT( vInnerCircle[3]->m_pDockBarInner != NULL );
	}
	if( vInnerCircle[0]->m_pDockBarOuter == NULL )
	{
		ASSERT( vInnerCircle[1]->m_pDockBarOuter == NULL );
		ASSERT( vInnerCircle[2]->m_pDockBarOuter == NULL );
		ASSERT( vInnerCircle[3]->m_pDockBarOuter == NULL );
	}
	else
	{
		ASSERT( vInnerCircle[1]->m_pDockBarOuter != NULL );
		ASSERT( vInnerCircle[2]->m_pDockBarOuter != NULL );
		ASSERT( vInnerCircle[3]->m_pDockBarOuter != NULL );
	}
#endif // _DEBUG

	nCircleNo ++;


	for( INT i = 0; i<4; i++ )
	{
		CExtDockBar * pBar = vInnerCircle[i];
		UINT nID = pBar->GetDlgCtrlID();
		ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nID );
		ASSERT( vCreatingCircle[i] == NULL );
		vCreatingCircle[i] =
			new CExtDockBar( nCircleNo );
static const DWORD dwAlignFlags[4] =
{
	CBRS_ALIGN_TOP,
	CBRS_ALIGN_BOTTOM,
	CBRS_ALIGN_LEFT,
	CBRS_ALIGN_RIGHT
};
		DWORD dwStyle =
			WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN
			|dwAlignFlags[i] // |CBRS_ALIGN_ANY
			;
		VERIFY(
			vCreatingCircle[i]->Create( pFrame, dwStyle, nID )
			);
		ASSERT( pBar->m_pDockBarInner == NULL );
		ASSERT( vCreatingCircle[i]->m_pDockBarInner == NULL );
		ASSERT( vCreatingCircle[i]->m_pDockBarOuter == NULL );
		pBar->m_pDockBarInner = vCreatingCircle[i];
		vCreatingCircle[i]->m_pDockBarOuter = pBar;
		vCreatingCircle[i]->SetBarStyle( pBar->GetBarStyle() );

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		vCreatingCircle[i]->m_pWndAutoHideArea =
			pBar->m_pWndAutoHideArea;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		
		
		if( (pBar->GetStyle() & WS_VISIBLE) != NULL
			&& pBar->GetDockedVisibleCount() != 0
			)
		{
			pBar->GetWindowRect( &vCreatingCircle[i]->m_rcLastInvisiblePreCalc );
			switch( nID )
			{
			case AFX_IDW_DOCKBAR_TOP:
				ASSERT( vCreatingCircle[i]->m_rcLastInvisiblePreCalc.left <= vCreatingCircle[i]->m_rcLastInvisiblePreCalc.right );
				vCreatingCircle[i]->m_rcLastInvisiblePreCalc.bottom = vCreatingCircle[i]->m_rcLastInvisiblePreCalc.top; // + 1;
				break;
			case AFX_IDW_DOCKBAR_BOTTOM:
				ASSERT( vCreatingCircle[i]->m_rcLastInvisiblePreCalc.left <= vCreatingCircle[i]->m_rcLastInvisiblePreCalc.right );
				vCreatingCircle[i]->m_rcLastInvisiblePreCalc.top = vCreatingCircle[i]->m_rcLastInvisiblePreCalc.bottom; //  - 1;
				break;
			case AFX_IDW_DOCKBAR_LEFT:
				ASSERT( vCreatingCircle[i]->m_rcLastInvisiblePreCalc.top <= vCreatingCircle[i]->m_rcLastInvisiblePreCalc.bottom );
				vCreatingCircle[i]->m_rcLastInvisiblePreCalc.right = vCreatingCircle[i]->m_rcLastInvisiblePreCalc.left; //  + 1;
				break;
			case AFX_IDW_DOCKBAR_RIGHT:
				ASSERT( vCreatingCircle[i]->m_rcLastInvisiblePreCalc.top <= vCreatingCircle[i]->m_rcLastInvisiblePreCalc.bottom );
				vCreatingCircle[i]->m_rcLastInvisiblePreCalc.left = vCreatingCircle[i]->m_rcLastInvisiblePreCalc.right; //  - 1;
				break;
#ifdef _DEBUG
			default:
				ASSERT( FALSE );
				break;
#endif // _DEBUG
			} // switch( nID )
		} // if( (pBar->GetStyle()&WS_VISIBLE) != NULL  .....
		else
			vCreatingCircle[i]->m_rcLastInvisiblePreCalc = pBar->m_rcLastInvisiblePreCalc;

		// reposition created circle in parent HWND list
		switch( nID )
		{
		case AFX_IDW_DOCKBAR_TOP:
			ASSERT( i == 0 );
			ASSERT( vCreatingCircle[i] != NULL );
			ASSERT( vCreatingCircle[i]->GetSafeHwnd() != NULL );
			ASSERT( vInnerCircle[3] != NULL );
			ASSERT( vInnerCircle[3]->GetSafeHwnd() != NULL );
			VERIFY(
				::SetWindowPos(
					vCreatingCircle[i]->GetSafeHwnd(),
					vInnerCircle[3]->GetSafeHwnd(),
					0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE
						|SWP_NOREDRAW|SWP_NOACTIVATE|SWP_NOCOPYBITS
						////|SWP_NOSENDCHANGING 
					)
				);
			break;
		case AFX_IDW_DOCKBAR_BOTTOM:
		case AFX_IDW_DOCKBAR_LEFT:
		case AFX_IDW_DOCKBAR_RIGHT:
			ASSERT( i >= 1 && i <= 3 );
			ASSERT( vCreatingCircle[i] != NULL );
			ASSERT( vCreatingCircle[i]->GetSafeHwnd() != NULL );
			ASSERT( vCreatingCircle[i-1] != NULL );
			ASSERT( vCreatingCircle[i-1]->GetSafeHwnd() != NULL );
			VERIFY(
				::SetWindowPos(
					vCreatingCircle[i]->GetSafeHwnd(),
					vCreatingCircle[i-1]->GetSafeHwnd(),
					0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE
						|SWP_NOREDRAW|SWP_NOACTIVATE|SWP_NOCOPYBITS
						////|SWP_NOSENDCHANGING 
					)
				);
			break;
#ifdef _DEBUG
		default:
			ASSERT( FALSE );
			break;
#endif // _DEBUG
		} // switch( nID )
	
	}

	ASSERT( vInnerCircle[0]->m_pDockBarInner != NULL );
	ASSERT( vInnerCircle[1]->m_pDockBarInner != NULL );
	ASSERT( vInnerCircle[2]->m_pDockBarInner != NULL );
	ASSERT( vInnerCircle[3]->m_pDockBarInner != NULL );
	
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( vInnerCircle[0]->m_pWndAutoHideArea != NULL )
	{
		vInnerCircle[0]->m_pWndAutoHideArea->AdjustOrder();
		vInnerCircle[1]->m_pWndAutoHideArea->AdjustOrder();
		vInnerCircle[2]->m_pWndAutoHideArea->AdjustOrder();
		vInnerCircle[3]->m_pWndAutoHideArea->AdjustOrder();
	}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	ASSERT( nCircleNo > 0 );
	return nCircleNo;
}

void CExtDockBar::_InjectInnerCircle()
{
	ASSERT_VALID( this );
	_InjectCircle( m_nCircleNo + 1 );
}

void CExtDockBar::_InjectCircle( UINT nCircleNo )
{
	ASSERT_VALID( this );
	ASSERT( nCircleNo > 0 );

CFrameWnd * pFrame = GetParentFrame();
	ASSERT_VALID( pFrame );
	ASSERT( !pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) );
	_InjectCircle( pFrame, nCircleNo );
}

void CExtDockBar::_InjectCircle( CFrameWnd * pFrame, UINT nCircleNo )
{
	ASSERT_VALID( pFrame );
	ASSERT( nCircleNo > 0 );

CExtDockBar * vCircleOld[4];
	vCircleOld[0] = (CExtDockBar *)pFrame->GetControlBar( AFX_IDW_DOCKBAR_TOP );
	vCircleOld[1] = (CExtDockBar *)pFrame->GetControlBar( AFX_IDW_DOCKBAR_BOTTOM );
	vCircleOld[2] = (CExtDockBar *)pFrame->GetControlBar( AFX_IDW_DOCKBAR_LEFT );
	vCircleOld[3] = (CExtDockBar *)pFrame->GetControlBar( AFX_IDW_DOCKBAR_RIGHT );
bool bEmptyCircleExist = true;
INT nDockSide = 0; 
	for( nDockSide = 0; nDockSide < 4; nDockSide++ )
	{
		ASSERT_VALID( vCircleOld[nDockSide] );
		ASSERT_KINDOF( CExtDockBar, vCircleOld[nDockSide] );
		ASSERT( vCircleOld[nDockSide]->_GetCircleNo() == 0 );
		vCircleOld[nDockSide] = vCircleOld[nDockSide]->_GetBarByCircleNo( nCircleNo );
		ASSERT_VALID( vCircleOld[nDockSide] );
		ASSERT( vCircleOld[nDockSide]->_GetCircleNo() == nCircleNo );
		if( ! vCircleOld[nDockSide]->_CanBeSafeOptimized() )
			bEmptyCircleExist = false;
	} // for( nDockSide = 0; nDockSide < 4; nDockSide++ )
	if( bEmptyCircleExist )
	{
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( vCircleOld[0]->m_pWndAutoHideArea != NULL )
		{
			vCircleOld[0]->m_pWndAutoHideArea->AdjustOrder();
			vCircleOld[1]->m_pWndAutoHideArea->AdjustOrder();
			vCircleOld[2]->m_pWndAutoHideArea->AdjustOrder();
			vCircleOld[3]->m_pWndAutoHideArea->AdjustOrder();
		}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		return;
	}

CExtDockBar * vCircleNew[4] = { NULL, NULL, NULL, NULL };
	for( nDockSide = 0; nDockSide < 4; nDockSide++ )
	{
		ASSERT_VALID( vCircleOld[nDockSide] );
		ASSERT( vCircleOld[nDockSide]->_GetCircleNo() == nCircleNo );

		UINT nID = vCircleOld[nDockSide]->GetDlgCtrlID();
		ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nID );
		vCircleNew[nDockSide] =
			new CExtDockBar( nCircleNo );
		
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		vCircleNew[nDockSide]->m_pWndAutoHideArea =
			vCircleOld[nDockSide]->m_pWndAutoHideArea;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		
static const DWORD dwAlignFlags[4] =
{
	CBRS_ALIGN_TOP,
	CBRS_ALIGN_BOTTOM,
	CBRS_ALIGN_LEFT,
	CBRS_ALIGN_RIGHT
};
		DWORD dwStyle =
			WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN
			|dwAlignFlags[nDockSide] // |CBRS_ALIGN_ANY
			;
		VERIFY(
			vCircleNew[nDockSide]->Create( pFrame, dwStyle, nID )
			);
		ASSERT( vCircleNew[nDockSide]->m_pDockBarInner == NULL );
		ASSERT( vCircleNew[nDockSide]->m_pDockBarOuter == NULL );
		ASSERT( vCircleOld[nDockSide]->m_pDockBarOuter != NULL );
		ASSERT( vCircleOld[nDockSide]->m_pDockBarOuter->m_pDockBarInner == vCircleOld[nDockSide] );
		
		vCircleOld[nDockSide]->m_pDockBarOuter->m_pDockBarInner = vCircleNew[nDockSide];
		vCircleNew[nDockSide]->m_pDockBarOuter = vCircleOld[nDockSide]->m_pDockBarOuter;
		vCircleOld[nDockSide]->m_pDockBarOuter = vCircleNew[nDockSide];
		vCircleNew[nDockSide]->m_pDockBarInner = vCircleOld[nDockSide];

		vCircleNew[nDockSide]->SetBarStyle(
			vCircleOld[nDockSide]->GetBarStyle()
			);
		if( (vCircleOld[nDockSide]->GetStyle() & WS_VISIBLE) != NULL
			&& vCircleOld[nDockSide]->GetDockedVisibleCount() != 0
			)
		{
			vCircleOld[nDockSide]->GetWindowRect(
				&(vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc)
				);
			switch( nID )
			{
			case AFX_IDW_DOCKBAR_TOP:
				ASSERT( vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc.left <= vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc.right );
				vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc.bottom = vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc.top; // + 1;
				break;
			case AFX_IDW_DOCKBAR_BOTTOM:
				ASSERT( vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc.left <= vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc.right );
				vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc.top = vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc.bottom; //  - 1;
				break;
			case AFX_IDW_DOCKBAR_LEFT:
				ASSERT( vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc.top <= vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc.bottom );
				vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc.right = vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc.left; //  + 1;
				break;
			case AFX_IDW_DOCKBAR_RIGHT:
				ASSERT( vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc.top <= vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc.bottom );
				vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc.left = vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc.right; //  - 1;
				break;
#ifdef _DEBUG
			default:
				ASSERT( FALSE );
				break;
#endif // _DEBUG
			} // switch( nID )
		} // if( (vCircleOld[nDockSide]->GetStyle()&WS_VISIBLE) != NULL  .....
		else
			vCircleNew[nDockSide]->m_rcLastInvisiblePreCalc =
				vCircleOld[nDockSide]->m_rcLastInvisiblePreCalc;
		
		for(	CExtDockBar * pResetNoBar = vCircleOld[nDockSide];
				pResetNoBar != NULL;
				pResetNoBar = pResetNoBar->m_pDockBarInner
				)
			pResetNoBar->m_nCircleNo++;

		switch( nID )
		{
		case AFX_IDW_DOCKBAR_TOP:
			ASSERT( nDockSide == 0 );
			ASSERT( vCircleOld[3]->m_pDockBarOuter != NULL );
			ASSERT( vCircleOld[3]->m_pDockBarOuter->GetSafeHwnd() != NULL );

			ASSERT( vCircleNew[nDockSide] != NULL );
			ASSERT( vCircleNew[nDockSide]->GetSafeHwnd() != NULL );

			VERIFY(
				::SetWindowPos(
					vCircleNew[nDockSide]->GetSafeHwnd(),
					vCircleOld[3]->m_pDockBarOuter->GetSafeHwnd(),
					0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE
						|SWP_NOREDRAW|SWP_NOACTIVATE|SWP_NOCOPYBITS
						////|SWP_NOSENDCHANGING 
					)
				);
			break;
		case AFX_IDW_DOCKBAR_BOTTOM:
		case AFX_IDW_DOCKBAR_LEFT:
		case AFX_IDW_DOCKBAR_RIGHT:
			ASSERT( nDockSide > 0 );
			ASSERT( vCircleNew[nDockSide-1] != NULL );
			ASSERT( vCircleNew[nDockSide-1]->GetSafeHwnd() != NULL );

			ASSERT( vCircleNew[nDockSide] != NULL );
			ASSERT( vCircleNew[nDockSide]->GetSafeHwnd() != NULL );

			VERIFY(
				::SetWindowPos(
					vCircleNew[nDockSide]->GetSafeHwnd(),
					vCircleNew[nDockSide-1]->GetSafeHwnd(),
					0, 0, 0, 0,
					SWP_NOSIZE|SWP_NOMOVE
						|SWP_NOREDRAW|SWP_NOACTIVATE|SWP_NOCOPYBITS
						////|SWP_NOSENDCHANGING 
					)
				);
			break;
#ifdef _DEBUG
		default:
			ASSERT( FALSE );
			break;
#endif // _DEBUG
		} // switch( nID )

//		VERIFY(
//			::SetWindowPos(
//				vCircleNew[nDockSide]->GetSafeHwnd(),
//				vCircleOld[nDockSide]->m_pDockBarOuter->_GetHwndForInjectingAfter(),
//				0, 0, 0, 0,
//				SWP_NOSIZE|SWP_NOMOVE
//					|SWP_NOREDRAW|SWP_NOACTIVATE|SWP_NOCOPYBITS
//					////|SWP_NOSENDCHANGING 
//				)
//			);

	} // for( nDockSide = 0; nDockSide < 4; nDockSide++ )
	ASSERT_VALID( vCircleNew[0] );
	ASSERT_VALID( vCircleNew[1] );
	ASSERT_VALID( vCircleNew[2] );
	ASSERT_VALID( vCircleNew[3] );

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( vCircleNew[0]->m_pWndAutoHideArea != NULL )
	{
		vCircleNew[0]->m_pWndAutoHideArea->AdjustOrder();
		vCircleNew[1]->m_pWndAutoHideArea->AdjustOrder();
		vCircleNew[2]->m_pWndAutoHideArea->AdjustOrder();
		vCircleNew[3]->m_pWndAutoHideArea->AdjustOrder();
	}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
}

#ifdef _DEBUG

void CExtDockBar::_Assert_HwndIsDockbar(
	HWND hWndDockBar,
	UINT nDockBarID,
	UINT nCircleNo
	)
{
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );
	ASSERT( hWndDockBar != NULL );
	ASSERT( ::IsWindow( hWndDockBar ) );
CWnd * pWndDockBar = CWnd::FromHandlePermanent( hWndDockBar );
	ASSERT_VALID( pWndDockBar );
CExtDockBar * pDockBar =
		STATIC_DOWNCAST( CExtDockBar, pWndDockBar );
UINT nRealDockBarID = pDockBar->GetDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nRealDockBarID );
	ASSERT( nDockBarID == nRealDockBarID );
UINT nRealCircleNo = pDockBar->_GetCircleNo();
	ASSERT( nRealCircleNo == nCircleNo );
}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
void CExtDockBar::_Assert_HwndIsAutoHider(
	HWND hWndAutoHider,
	UINT nAutoHiderID
	)
{
	ASSERT( hWndAutoHider != NULL );
	ASSERT( ::IsWindow( hWndAutoHider ) );
CWnd * pWndAutoHider = CWnd::FromHandlePermanent( hWndAutoHider );
	ASSERT_VALID( pWndAutoHider );
CExtDynAutoHideArea * pAutoHider =
		STATIC_DOWNCAST( CExtDynAutoHideArea, pWndAutoHider );
UINT nRealAutoHiderID = pAutoHider->GetDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nRealAutoHiderID );
	ASSERT( nAutoHiderID == nRealAutoHiderID );
}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

#endif // _DEBUG

//HWND CExtDockBar::_GetHwndForInjectingAfter()
//{
//	ASSERT_VALID( this );
//HWND hWndOwn = GetSafeHwnd();
//	ASSERT( hWndOwn != NULL );
//	ASSERT( ::IsWindow( hWndOwn ) );
//
//UINT nOwnID = GetDlgCtrlID();
//	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nOwnID );
//
//HWND hWndAutoHideArea = m_pWndAutoHideArea->GetSafeHwnd();
//UINT nOwnCircleNo =  _GetCircleNo();
//
//HWND hWndInsertAfter =
//		(hWndAutoHideArea != NULL && nOwnCircleNo == 0)
//			? hWndAutoHideArea
//			: hWndOwn
//			;
//	switch( nOwnID )
//	{
//	case AFX_IDW_DOCKBAR_TOP:
//	{
//		// get dockbar in same circle (nOwnCircleNo) - bottom
//		hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//		ASSERT_HWND_IS_DOCKBAR( hWndInsertAfter, AFX_IDW_DOCKBAR_BOTTOM, nOwnCircleNo );
//		// get dockbar in same circle (nOwnCircleNo) - left
//		hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//		ASSERT_HWND_IS_DOCKBAR( hWndInsertAfter, AFX_IDW_DOCKBAR_LEFT, nOwnCircleNo );
//		// get dockbar in same circle (nOwnCircleNo) - right
//		hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//		ASSERT_HWND_IS_DOCKBAR( hWndInsertAfter, AFX_IDW_DOCKBAR_RIGHT, nOwnCircleNo );
//		if( nOwnCircleNo == 0 && hWndAutoHideArea != NULL )
//		{
//			// get autohider - top
//			hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//			ASSERT_HWND_IS_AUTOHIDER( hWndInsertAfter, AFX_IDW_DOCKBAR_TOP );
//			// get autohider - bottom
//			hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//			ASSERT_HWND_IS_AUTOHIDER( hWndInsertAfter, AFX_IDW_DOCKBAR_BOTTOM );
//			// get autohider - left
//			hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//			ASSERT_HWND_IS_AUTOHIDER( hWndInsertAfter, AFX_IDW_DOCKBAR_LEFT );
//			// get autohider - right
//			hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//			ASSERT_HWND_IS_AUTOHIDER( hWndInsertAfter, AFX_IDW_DOCKBAR_RIGHT );
//		}
//	}
//	break; // case AFX_IDW_DOCKBAR_TOP
//	case AFX_IDW_DOCKBAR_BOTTOM:
//	{
//		// get dockbar in same circle (nOwnCircleNo) - left
//		hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//		ASSERT_HWND_IS_DOCKBAR( hWndInsertAfter, AFX_IDW_DOCKBAR_LEFT, nOwnCircleNo );
//		// get dockbar in same circle (nOwnCircleNo) - right
//		hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//		ASSERT_HWND_IS_DOCKBAR( hWndInsertAfter, AFX_IDW_DOCKBAR_RIGHT, nOwnCircleNo );
//		if( nOwnCircleNo == 0 && hWndAutoHideArea != NULL )
//		{
//			// get autohider - top
//			hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//			ASSERT_HWND_IS_AUTOHIDER( hWndInsertAfter, AFX_IDW_DOCKBAR_TOP );
//			// get autohider - bottom
//			hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//			ASSERT_HWND_IS_AUTOHIDER( hWndInsertAfter, AFX_IDW_DOCKBAR_BOTTOM );
//			// get autohider - left
//			hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//			ASSERT_HWND_IS_AUTOHIDER( hWndInsertAfter, AFX_IDW_DOCKBAR_LEFT );
//			// get autohider - right
//			hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//			ASSERT_HWND_IS_AUTOHIDER( hWndInsertAfter, AFX_IDW_DOCKBAR_RIGHT );
//		}
//		// get dockbar in next circle (nOwnCircleNo+1) - top
//		hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//		ASSERT_HWND_IS_DOCKBAR( hWndInsertAfter, AFX_IDW_DOCKBAR_TOP, (nOwnCircleNo+1) );
//	}
//	break; // case AFX_IDW_DOCKBAR_BOTTOM
//	case AFX_IDW_DOCKBAR_LEFT:
//	{
//		// get dockbar in same circle (nOwnCircleNo) - right
//		hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//		ASSERT_HWND_IS_DOCKBAR( hWndInsertAfter, AFX_IDW_DOCKBAR_RIGHT, nOwnCircleNo );
//		if( nOwnCircleNo == 0 && hWndAutoHideArea != NULL )
//		{
//			// get autohider - top
//			hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//			ASSERT_HWND_IS_AUTOHIDER( hWndInsertAfter, AFX_IDW_DOCKBAR_TOP );
//			// get autohider - bottom
//			hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//			ASSERT_HWND_IS_AUTOHIDER( hWndInsertAfter, AFX_IDW_DOCKBAR_BOTTOM );
//			// get autohider - left
//			hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//			ASSERT_HWND_IS_AUTOHIDER( hWndInsertAfter, AFX_IDW_DOCKBAR_LEFT );
//			// get autohider - right
//			hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//			ASSERT_HWND_IS_AUTOHIDER( hWndInsertAfter, AFX_IDW_DOCKBAR_RIGHT );
//		}
//		// get dockbar in next circle (nOwnCircleNo+1) - top
//		hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//		ASSERT_HWND_IS_DOCKBAR( hWndInsertAfter, AFX_IDW_DOCKBAR_TOP, (nOwnCircleNo+1) );
//		// get dockbar in next circle (nOwnCircleNo+1) - bottom
//		hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//		ASSERT_HWND_IS_DOCKBAR( hWndInsertAfter, AFX_IDW_DOCKBAR_BOTTOM, (nOwnCircleNo+1) );
//	}
//	break; // case AFX_IDW_DOCKBAR_LEFT
//	case AFX_IDW_DOCKBAR_RIGHT:
//	{
//		if( nOwnCircleNo == 0 && hWndAutoHideArea != NULL )
//		{
//			// get autohider - top
//			hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//			ASSERT_HWND_IS_AUTOHIDER( hWndInsertAfter, AFX_IDW_DOCKBAR_TOP );
//			// get autohider - bottom
//			hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//			ASSERT_HWND_IS_AUTOHIDER( hWndInsertAfter, AFX_IDW_DOCKBAR_BOTTOM );
//			// get autohider - left
//			hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//			ASSERT_HWND_IS_AUTOHIDER( hWndInsertAfter, AFX_IDW_DOCKBAR_LEFT );
//			// get autohider - right
//			hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//			ASSERT_HWND_IS_AUTOHIDER( hWndInsertAfter, AFX_IDW_DOCKBAR_RIGHT );
//		}
//		// get dockbar in next circle (nOwnCircleNo+1) - top
//		hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//		ASSERT_HWND_IS_DOCKBAR( hWndInsertAfter, AFX_IDW_DOCKBAR_TOP, (nOwnCircleNo+1) );
//		// get dockbar in next circle (nOwnCircleNo+1) - bottom
//		hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//		ASSERT_HWND_IS_DOCKBAR( hWndInsertAfter, AFX_IDW_DOCKBAR_BOTTOM, (nOwnCircleNo+1) );
//		// get dockbar in next circle (nOwnCircleNo+1) - left
//		hWndInsertAfter = ::GetWindow( hWndInsertAfter, GW_HWNDNEXT );
//		ASSERT_HWND_IS_DOCKBAR( hWndInsertAfter, AFX_IDW_DOCKBAR_LEFT, (nOwnCircleNo+1) );
//	}
//	break; // case AFX_IDW_DOCKBAR_RIGHT
//#ifdef _DEBUG
//	default:
//		ASSERT( FALSE );
//		break;
//#endif // _DEBUG
//	} // switch( nOwnID )
//
//
//	ASSERT( hWndInsertAfter != NULL );
//	return hWndInsertAfter;
//}

CExtDockBar * CExtDockBar::_GetBarByCircleNo( UINT nCircleNo )
{
	ASSERT_VALID( this );
	if( nCircleNo == m_nCircleNo )
		return this;
	if( nCircleNo < m_nCircleNo )
	{
		ASSERT( m_nCircleNo > 0 );
		ASSERT( m_pDockBarOuter != NULL );
		return m_pDockBarOuter->_GetBarByCircleNo( nCircleNo );
	}

	if( m_pDockBarInner == NULL )
	{
#ifdef _DEBUG
		UINT nNewCircle =
#endif // _DEBUG
			_CreateInnerCircle();
		ASSERT( nNewCircle == (m_nCircleNo + 1) );
	}
	ASSERT( m_pDockBarInner != NULL );
	ASSERT( m_pDockBarInner->m_nCircleNo == (m_nCircleNo + 1) );
	return m_pDockBarInner->_GetBarByCircleNo( nCircleNo );
}

LRESULT CExtDockBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if(		message == WM_PRINT
		||	message == WM_PRINTCLIENT
		)
	{
		CDC * pDC = CDC::FromHandle( (HDC) wParam );
		
		CRect rcWnd, rcClient;
		GetWindowRect( &rcWnd );
		GetClientRect( &rcClient );
		ClientToScreen( rcClient );
		rcClient.OffsetRect( -rcWnd.TopLeft() );
		rcWnd.OffsetRect( -rcWnd.TopLeft() );

// 		if( (lParam&PRF_NONCLIENT) != 0 )
// 		{
// 			pDC->ExcludeClipRect(rcClient);
// 			// draw borders in non-client area
// 			DrawBorders( pDC, rcWnd );
// 			// draw gripper in non-client area
// 			DrawGripper( pDC, rcWnd );
// 			pDC->SelectClipRgn( NULL );
// 		}

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
	
	switch( message )
	{
	case WM_CREATE:
		EnableToolTips();
	break;
	case WM_DESTROY:
	case WM_NCDESTROY:
		if( m_pDockBarInner != NULL )
		{
			ASSERT( m_pDockBarInner->m_pDockBarOuter == this );
			m_pDockBarInner->m_pDockBarOuter = NULL;
			m_pDockBarInner = NULL;
		}
		if( m_pDockBarOuter != NULL )
		{
			ASSERT( m_pDockBarOuter->m_pDockBarInner == this );
			m_pDockBarOuter->m_pDockBarInner = NULL;
			m_pDockBarOuter = NULL;
		}
		break;
	} // switch( message )

	return CControlBar::WindowProc(message, wParam, lParam);
}

BOOL CExtDockBar::PreTranslateMessage(MSG* pMsg)
{
	// return CDockBar::PreTranslateMessage( pMsg );
	pMsg;
	return FALSE;
}

void CExtDockBar::_OptimizeCircles()
{
	ASSERT_VALID( this );

CFrameWnd * pFrame = GetParentFrame();
	ASSERT_VALID( pFrame );
	ASSERT( !pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) );

	if( ! m_bLockedOptimize )
		_OptimizeCircles( pFrame );
}

void CExtDockBar::_OptimizeCircles( CFrameWnd * pFrame )
{
	if( pFrame->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( pFrame );

CExtDockBar * vCircle[4];

	vCircle[0] = (CExtDockBar*)pFrame->GetControlBar( AFX_IDW_DOCKBAR_TOP );
	if( vCircle[0] == NULL )
		return;
	ASSERT_VALID( vCircle[0] );
	ASSERT_KINDOF( CExtDockBar, vCircle[0] );
	ASSERT( vCircle[0]->_GetCircleNo() == 0 );
	vCircle[1] = (CExtDockBar*)pFrame->GetControlBar( AFX_IDW_DOCKBAR_BOTTOM );
	ASSERT_VALID( vCircle[1] );
	ASSERT_KINDOF( CExtDockBar, vCircle[1] );
	ASSERT( vCircle[1]->_GetCircleNo() == 0 );
	vCircle[2] = (CExtDockBar*)pFrame->GetControlBar( AFX_IDW_DOCKBAR_LEFT );
	ASSERT_VALID( vCircle[2] );
	ASSERT_KINDOF( CExtDockBar, vCircle[2] );
	ASSERT( vCircle[2]->_GetCircleNo() == 0 );
	vCircle[3] = (CExtDockBar*)pFrame->GetControlBar( AFX_IDW_DOCKBAR_RIGHT );
	ASSERT_VALID( vCircle[3] );
	ASSERT_KINDOF( CExtDockBar, vCircle[3] );
	ASSERT( vCircle[3]->_GetCircleNo() == 0 );

	if( vCircle[0]->_GetDockBarInner() == NULL )
		return;
	vCircle[0] = vCircle[0]->_GetDockBarInner();
	ASSERT( vCircle[0] != NULL );
	ASSERT_KINDOF( CExtDockBar, vCircle[0] );
	ASSERT( vCircle[0]->_GetCircleNo() == 1 );
	
	vCircle[1] = vCircle[1]->_GetDockBarInner();
	ASSERT( vCircle[1] != NULL );
	ASSERT_KINDOF( CExtDockBar, vCircle[1] );
	ASSERT( vCircle[1]->_GetCircleNo() == 1 );
	
	vCircle[2] = vCircle[2]->_GetDockBarInner();
	ASSERT( vCircle[2] != NULL );
	ASSERT_KINDOF( CExtDockBar, vCircle[2] );
	ASSERT( vCircle[2]->_GetCircleNo() == 1 );
	
	vCircle[3] = vCircle[3]->_GetDockBarInner();
	ASSERT( vCircle[3] != NULL );
	ASSERT_KINDOF( CExtDockBar, vCircle[3] );
	ASSERT( vCircle[3]->_GetCircleNo() == 1 );

//bool bCreateInnerCircle = true;

UINT nCircleShift = 0;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
bool bAutoHidersPassed = false;
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	while( vCircle[0] != NULL )
	{
		ASSERT( vCircle[0] != NULL );
		ASSERT_KINDOF( CExtDockBar, vCircle[0] );
		ASSERT( vCircle[1] != NULL );
		ASSERT_KINDOF( CExtDockBar, vCircle[1] );
		ASSERT( vCircle[2] != NULL );
		ASSERT_KINDOF( CExtDockBar, vCircle[2] );
		ASSERT( vCircle[3] != NULL );
		ASSERT_KINDOF( CExtDockBar, vCircle[3] );

		ASSERT( vCircle[0]->m_nCircleNo == vCircle[1]->m_nCircleNo );
		ASSERT( vCircle[0]->m_nCircleNo == vCircle[2]->m_nCircleNo );
		ASSERT( vCircle[0]->m_nCircleNo == vCircle[3]->m_nCircleNo );

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( !bAutoHidersPassed )
		{
			bAutoHidersPassed = true;
			if( vCircle[0]->m_pWndAutoHideArea != NULL )
			{
				vCircle[0]->m_pWndAutoHideArea->AdjustOrder();
				vCircle[1]->m_pWndAutoHideArea->AdjustOrder();
				vCircle[2]->m_pWndAutoHideArea->AdjustOrder();
				vCircle[3]->m_pWndAutoHideArea->AdjustOrder();
			}
		}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

		if( nCircleShift != 0 )
		{
			ASSERT( vCircle[0]->m_nCircleNo > nCircleShift );
			vCircle[0]->m_nCircleNo -= nCircleShift;
			vCircle[1]->m_nCircleNo -= nCircleShift;
			vCircle[2]->m_nCircleNo -= nCircleShift;
			vCircle[3]->m_nCircleNo -= nCircleShift;
		}

		CExtDockBar * vCircleOptimize[4];
		
		vCircleOptimize[0] = vCircle[0];
		vCircleOptimize[1] = vCircle[1];
		vCircleOptimize[2] = vCircle[2];
		vCircleOptimize[3] = vCircle[3];

//		bool bLastCircle =
//			( vCircle[0]->_GetDockBarInner() != NULL ) ? true : false;

		vCircle[0] = vCircle[0]->_GetDockBarInner();
		vCircle[1] = vCircle[1]->_GetDockBarInner();
		vCircle[2] = vCircle[2]->_GetDockBarInner();
		vCircle[3] = vCircle[3]->_GetDockBarInner();

		if(		vCircleOptimize[0]->_CanBeSafeOptimized()
			&&	vCircleOptimize[1]->_CanBeSafeOptimized()
			&&	vCircleOptimize[2]->_CanBeSafeOptimized()
			&&	vCircleOptimize[3]->_CanBeSafeOptimized()
			)
		{
//			if( bLastCircle )
//			{
//				bCreateInnerCircle = false;
//				break;
//			}

#ifdef __DEBUG_LOCKING_OPTIMIZATION_ASSERTS__
			ASSERT( !vCircleOptimize[0]->m_bLockedOptimize );
			ASSERT( !vCircleOptimize[1]->m_bLockedOptimize );
			ASSERT( !vCircleOptimize[2]->m_bLockedOptimize );
			ASSERT( !vCircleOptimize[3]->m_bLockedOptimize );
#endif // __DEBUG_LOCKING_OPTIMIZATION_ASSERTS__
			
			vCircleOptimize[0]->_SafeOptimizeInnerOuterChain();
			vCircleOptimize[1]->_SafeOptimizeInnerOuterChain();
			vCircleOptimize[2]->_SafeOptimizeInnerOuterChain();
			vCircleOptimize[3]->_SafeOptimizeInnerOuterChain();
			
			nCircleShift++;
		}
		else
		{
#ifdef __DEBUG_LOCKING_OPTIMIZATION_ASSERTS__
			ASSERT( !vCircleOptimize[0]->m_bLockedOptimize );
			ASSERT( !vCircleOptimize[1]->m_bLockedOptimize );
			ASSERT( !vCircleOptimize[2]->m_bLockedOptimize );
			ASSERT( !vCircleOptimize[3]->m_bLockedOptimize );
#endif // __DEBUG_LOCKING_OPTIMIZATION_ASSERTS__
			
			vCircleOptimize[0]->OnDynamicLayoutOptimize();
			vCircleOptimize[1]->OnDynamicLayoutOptimize();
			vCircleOptimize[2]->OnDynamicLayoutOptimize();
			vCircleOptimize[3]->OnDynamicLayoutOptimize();
		}

	} // while( vCircle[0] != NULL )

//	if( bCreateInnerCircle )
//		_CreateInnerCircle( pFrame );

CArray < HWND, HWND > arrHwndsToDynOptimize;
bool bDeepOptimize = false;

POSITION pos = pFrame->m_listControlBars.GetHeadPosition();
	for( ; pos != NULL; )
	{
		CExtDynControlBar * pBar =
			DYNAMIC_DOWNCAST(
				CExtDynControlBar,
				((CObject *)(pFrame->m_listControlBars.GetNext(pos)))
				);
		if( pBar == NULL )
			continue;
		ASSERT( !pBar->IsFixedMode() );
		HWND hWnd = pBar->m_pWndDynDocker->GetSafeHwnd();
		if( hWnd == NULL || (! ::IsWindow( hWnd ) ) )
			continue;
		//if( !pBar->IsFloating() )
		{
			INT nDockedCount =
				pBar->m_pWndDynDocker->GetDockedCount();
			if( nDockedCount > 0 )
				continue;
			bDeepOptimize = true;
		} // if( !pBar->IsFloating() )

		arrHwndsToDynOptimize.Add( hWnd );
	}

	for( INT nHwndIdx = 0; nHwndIdx < arrHwndsToDynOptimize.GetSize(); nHwndIdx++ )
	{
		HWND hWnd = arrHwndsToDynOptimize[ nHwndIdx ];
		ASSERT( hWnd != NULL );
		if( ! ::IsWindow( hWnd ) )
			continue;
		CWnd * pWndChild = CWnd::FromHandlePermanent( hWnd );
		if( pWndChild == NULL )
			continue;
		CExtDockDynBar * pDockBar =
			DYNAMIC_DOWNCAST(
				CExtDockDynBar,
				pWndChild
				);
		if( pDockBar == NULL )
			continue;
#ifdef __DEBUG_LOCKING_OPTIMIZATION_ASSERTS__
		ASSERT( !pDockBar->m_bLockedOptimize );
#endif // __DEBUG_LOCKING_OPTIMIZATION_ASSERTS__
		pDockBar->OnDynamicLayoutOptimize();
	}

	if( bDeepOptimize )
		_OptimizeCircles( pFrame );
}

void CExtDockBar::_SafeOptimizeInnerOuterChain()
{
	ASSERT( this != NULL );
	ASSERT_KINDOF( CExtDockBar, this );
	ASSERT( GetSafeHwnd() != NULL );
	if( m_pDockBarInner != NULL )
	{
		ASSERT( m_pDockBarInner->m_pDockBarOuter == this );
		m_pDockBarInner->m_pDockBarOuter = m_pDockBarOuter;
	}
	if( m_pDockBarOuter != NULL )
	{
		ASSERT( m_pDockBarOuter->m_pDockBarInner == this );
		m_pDockBarOuter->m_pDockBarInner = m_pDockBarInner;
	}
	m_pDockBarInner = NULL;
	m_pDockBarOuter = NULL;

CFrameWnd * pFrame = GetParentFrame();
	ASSERT( pFrame != NULL );
	ASSERT( !pFrame->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd)) );

POSITION pos = pFrame->m_listControlBars.Find( this );
	ASSERT( pos != NULL );
	pFrame->m_listControlBars.RemoveAt( pos );

BOOL bAutoDelete = m_bAutoDelete;
	DestroyWindow();
	if( !bAutoDelete )
		delete this;
}

bool CExtDockBar::_CanBeSafeOptimized()
{
	ASSERT( this != NULL );
	ASSERT_KINDOF( CExtDockBar, this );
	for( INT nPos = 0; nPos < m_arrBars.GetSize(); nPos++ )
	{
		void * pBar = m_arrBars[ nPos ];
		if( pBar != NULL ) // placeholer or child control bar
			return false;
	}
	return true;
}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
CExtDynAutoHideArea * CExtDockBar::_GetAutoHideArea()
{
	return m_pWndAutoHideArea;
}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

void CExtDockBar::_HandleDestruction()
{
	__PROF_UIS_MANAGE_STATE;

	m_bLockedOptimize = true;
bool bFirstDestroyed = true;
	for( int nBar = int(m_arrBars.GetSize()) - 1; nBar > 0 ; nBar-- )
	{
		ASSERT( m_arrBars[0] == NULL );
		CExtControlBar * pBar = (CExtControlBar *)
			m_arrBars[ nBar ];
		if( bFirstDestroyed )
		{
			ASSERT( pBar == NULL );
			bFirstDestroyed = false;
			continue;
		}
		if( pBar == NULL )
		{
			m_arrBars.RemoveAt( nBar );
			continue;
		}
		if( __PLACEHODLER_BAR_PTR(pBar) )
		{
			m_arrBars.RemoveAt( nBar );
			continue;
		}
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CExtControlBar, pBar );
		ASSERT_VALID( pBar->m_pDockSite );
		ASSERT( pBar->m_pDockContext != NULL );
		ASSERT( pBar->m_pDockBar == this );
		pBar->m_bUpdatingChain = true;
		pBar->m_pDockBar = NULL;
		if( pBar->IsKindOf(RUNTIME_CLASS(CExtDynControlBar)) )
		{
			CExtDockDynBar * pDynDocker =
				((CExtDynControlBar*)pBar)->m_pWndDynDocker;
			ASSERT_VALID( pDynDocker );
			pDynDocker->_HandleDestruction();
		}
		ASSERT( pBar->m_bUpdatingChain );
		if( pBar->GetParent() != pBar->m_pDockSite )
			pBar->SetParent( pBar->m_pDockSite );
		pBar->m_bUpdatingChain = false;
		m_arrBars.RemoveAt( nBar );
	}
	m_bLockedOptimize = false;
}

void CExtDockBar::OnDestroy()
{
	__PROF_UIS_MANAGE_STATE;

	_HandleDestruction();
	CDockBar::OnDestroy();
}

BOOL CExtDockBar::Create( CWnd * pParentWnd, DWORD dwStyle, UINT nID )
{
	if( ! CDockBar::Create( pParentWnd, dwStyle, nID ) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
CRect rc;
	pParentWnd->GetClientRect( &rc );
	switch( nID )
	{
	case AFX_IDW_DOCKBAR_TOP:
		rc.bottom = rc.top;
		break;
	case AFX_IDW_DOCKBAR_BOTTOM:
		rc.top = rc.bottom;
		break;
	case AFX_IDW_DOCKBAR_LEFT:
		rc.right = rc.left;
		break;
	case AFX_IDW_DOCKBAR_RIGHT:
		rc.left = rc.right;
		break;
	} // switch( nID )
	MoveWindow( &rc, FALSE );
	if( ( (CWnd::GetExStyle()&(WS_EX_LAYOUTRTL|WS_EX_RTLREADING)) != 0 ) )
	{
		CWnd::ModifyStyleEx(
			WS_EX_LAYOUTRTL|WS_EX_RTLREADING,
			0,
			SWP_FRAMECHANGED
			);
		ASSERT( (CWnd::GetExStyle()&(WS_EX_LAYOUTRTL|WS_EX_RTLREADING)) == 0 );
	} // if( ( (CWnd::GetExStyle()&(WS_EX_LAYOUTRTL|WS_EX_RTLREADING)) != 0 ) )
	return TRUE;
}

BOOL CExtDockBar::DestroyWindow()
{
	__PROF_UIS_MANAGE_STATE;

	_HandleDestruction();
	return  CDockBar::DestroyWindow();
}

/////////////////////////////////////////////////////////////////////////
// CExtDockDynBar window

IMPLEMENT_DYNCREATE(CExtDockDynBar, CExtDockBar);

BEGIN_MESSAGE_MAP(CExtDockDynBar, CExtDockBar)
	//{{AFX_MSG_MAP(CExtDockDynBar)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CExtDockDynBar::CExtDockDynBar(
	UINT nCircleNo
	)
	: CExtDockBar( nCircleNo )
	, m_bHelperDockSiteModified( false )
	, m_bHelperSizing( false )
	, m_bHelperLockNearestLayout( false )
{
	m_bAutoDelete = TRUE;
}

/////////////////////////////////////////////////////////////////////////
// CExtDynControlBar window

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
CExtDynAutoHideArea * CExtDockDynBar::_GetAutoHideArea()
{
CExtControlBar * pExtBar = 
		STATIC_DOWNCAST(
			CExtControlBar,
			GetParent()
			);
	ASSERT( pExtBar->m_pDockBar != NULL );
CExtDockBar * pDockBar =
		DYNAMIC_DOWNCAST(
			CExtDockBar,
			pExtBar->m_pDockBar
			);
	if( pDockBar == NULL )
		return NULL;
	return pDockBar->_GetAutoHideArea();
}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

void CExtDockDynBar::VisibleLayoutItem_t::_AssignFromOther(
	const CExtDockDynBar::VisibleLayoutItem_t & other
	)
{
	m_vRow.RemoveAll();
INT nSize = (INT)other.m_vRow.GetSize();
	if( nSize == 0 )
	{
		m_vRow.FreeExtra();
		return;
	}
	m_vRow.SetSize( nSize );
	for( INT nBar = 0; nBar < nSize; nBar++ )
	{
		CExtControlBar * pExtBar = other.m_vRow[ nBar ];
		ASSERT( pExtBar != NULL );
		ASSERT( !pExtBar->IsFixedMode() );
#ifdef _DEBUG
		if( __PLACEHODLER_BAR_PTR(pExtBar) )
		{
			ASSERT( FALSE );
		}
#endif // _DEBUG
		m_vRow.SetAt( nBar, pExtBar );
	} // for( INT nBar = 0; nBar < nSize; nBar++ )
}

CExtDockDynBar::VisibleLayoutItem_t::VisibleLayoutItem_t()
	: m_nRowMetric( 0 )
	, m_nRowMinMetric( 0 )
	, m_nRowExtent( 0 )
	, m_nRowMinExtent( 0 )
	, m_bOverflowMode( false )
{
}

CExtDockDynBar::VisibleLayoutItem_t::VisibleLayoutItem_t(
	const CExtDockDynBar::VisibleLayoutItem_t & other
	)
{
	_AssignFromOther( other );
}

CExtDockDynBar::VisibleLayoutItem_t::~VisibleLayoutItem_t()
{
}

CExtDockDynBar::VisibleLayoutItem_t &
	CExtDockDynBar::VisibleLayoutItem_t::operator = (
		const CExtDockDynBar::VisibleLayoutItem_t & other
		)
{
	_AssignFromOther( other );
	return * this;
}

bool CExtDockDynBar::VisibleLayoutItem_t::IsEmpty() const
{
	if( m_vRow.GetSize() == 0 )
		return true;
	return false;
}

bool CExtDockDynBar::VisibleLayoutItem_t::IsRowMinSized() const
{
	ASSERT( m_nRowMetric >= m_nRowMinMetric );
	return (m_nRowMetric == m_nRowMinMetric ) ? true : false;
}


void CExtDockDynBar::VisibleLayout_t::_Clean()
{
INT nSize = (INT)m_vRows.GetSize();
	for( INT nRow = 0; nRow < nSize; nRow++ )
	{
		VisibleLayoutItem_t * pVLI = m_vRows.GetAt( nRow );
		ASSERT( pVLI != NULL );
		delete pVLI;
	}
	m_vRows.RemoveAll();
	m_vRows.FreeExtra();
}

CExtDockDynBar::VisibleLayout_t::VisibleLayout_t()
	: m_nTotalMetric( 0 )
	, m_nTotalMinMetric( 0 )
	, m_nTotalMinExtent( 0 )
	, m_nTotalBarsCount( 0 )
	, m_nDelayShowCount( 0 )
{
}

CExtDockDynBar::VisibleLayout_t::~VisibleLayout_t()
{
	_Clean();
}

void CExtDockDynBar::VisibleLayout_t::AddBarPointer(
	CExtControlBar * pExtBar
	)
{
	ASSERT_VALID( pExtBar );
	ASSERT( !pExtBar->IsFixedMode() );
	if( !pExtBar->IsVisible() )
		return;
#ifdef _DEBUG
		if( __PLACEHODLER_BAR_PTR(pExtBar) )
		{
			ASSERT( FALSE );
		}
#endif // _DEBUG

VisibleLayoutItem_t * pVLI = NULL;
INT nRowCount = (INT)m_vRows.GetSize();
	if( nRowCount == 0 )
	{
		pVLI = new VisibleLayoutItem_t;
		m_vRows.Add( pVLI );
	}
	else
		pVLI = m_vRows[ nRowCount - 1 ];
	ASSERT( pVLI != NULL );
	pVLI->m_vRow.Add( pExtBar );

	m_nTotalBarsCount++;
}

void CExtDockDynBar::VisibleLayout_t::MakeNewRow()
{
INT nRowCount = (INT)m_vRows.GetSize();
	if( nRowCount == 0 )
	{
		VisibleLayoutItem_t * pVLI = new VisibleLayoutItem_t;
		m_vRows.Add( pVLI );
		return;
	}
VisibleLayoutItem_t * pVLI = m_vRows.GetAt( nRowCount - 1 );
	ASSERT( pVLI != NULL );
	if( pVLI->IsEmpty() )
		return;
	pVLI = new VisibleLayoutItem_t;
	m_vRows.Add( pVLI );
}

bool CExtDockDynBar::VisibleLayout_t::IsEmpty() const
{
INT nRowCount = (INT)m_vRows.GetSize();
	if( nRowCount == 0 )
		return true;
	if( nRowCount == 1 )
	{
		VisibleLayoutItem_t * pVLI = m_vRows.GetAt( 0 );
		if( pVLI->IsEmpty() )
			return true;
	}
#ifdef _DEBUG
	else
	{
		VisibleLayoutItem_t * pVLI = m_vRows.GetAt( 0 );
		ASSERT( !pVLI->IsEmpty() );
		
	}
#endif // _DEBUG
	return false;
}

void CExtDockDynBar::_VisibleLayoutBuild(
	VisibleLayout_t & _vl
	)
{
	ASSERT_VALID( this );
	ASSERT( _vl.IsEmpty() );
CRect rcDockBarReal;
	GetClientRect( &rcDockBarReal );
CSize _size = rcDockBarReal.Size();
INT nCount = (INT)m_arrBars.GetSize();
	ASSERT( nCount > 0 );
	ASSERT( m_arrBars[0] == NULL );
UINT nOwnID = GetDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nOwnID );
BOOL bHorz = ( nOwnID == AFX_IDW_DOCKBAR_TOP || nOwnID == AFX_IDW_DOCKBAR_BOTTOM );
	ASSERT( m_arrBars[0] == NULL );
	for( INT nBar = 1; nBar < nCount; nBar++ )
	{
		CExtControlBar * pExtBar = (CExtControlBar *) m_arrBars[nBar];
		if( pExtBar == NULL )
		{
			if( nBar == (nCount-1) )
				break;
			_vl.MakeNewRow();
			continue;
		}
		if( __PLACEHODLER_BAR_PTR(pExtBar) )
			continue;
		ASSERT_VALID( pExtBar );
		ASSERT_KINDOF( CExtControlBar, pExtBar );
		ASSERT( !pExtBar->IsFixedMode() );
		if( pExtBar->m_bDelayShowInvoked )
			_vl.m_nDelayShowCount ++;
		if( ! pExtBar->IsVisible() )
		{
			pExtBar->m_bDelayShowInvoked = false;
			continue;
		}
		_vl.AddBarPointer( pExtBar );
	} // for( INT nBar = 1; nBar < nCount; nBar++ )

INT nRowsCount = (INT)_vl.m_vRows.GetSize(), nRow;
	if( nRowsCount == 0 )
		return;
bool bHaveDraggingBar = ( CExtControlBar::_DraggingGetBar() != NULL ) ? true : false;
	if( bHaveDraggingBar )
	{
		for( nRow = 0; nRow < nRowsCount; nRow ++ )
		{
			VisibleLayoutItem_t * pVLI = _vl.m_vRows[ nRow ];
			ASSERT( pVLI != NULL );
			pVLI->m_bOverflowMode = false;
			INT nRowSize = (INT)pVLI->m_vRow.GetSize(), nBar;
			if( nRowSize <= 1 )
				continue;
			for( nBar = 0; nBar < nRowSize; nBar++ )
			{
				CExtControlBar * pExtBar = pVLI->m_vRow[ nBar ];
				ASSERT( pExtBar != NULL );
				if( bHorz    && pExtBar->m_sizeDockedH.cx >= _size.cx )
					pVLI->m_bOverflowMode = true;
				else
				if( (!bHorz) && pExtBar->m_sizeDockedV.cy >= _size.cy )
					pVLI->m_bOverflowMode = true;
				if( pVLI->m_bOverflowMode )
					break;
			}
		}
	}

	for( nRow = 0; nRow < nRowsCount; nRow ++ )
	{
		VisibleLayoutItem_t * pVLI = _vl.m_vRows[ nRow ];
		ASSERT( pVLI != NULL );
		INT nRowSize = (INT)pVLI->m_vRow.GetSize();
		if( nRowSize == 0 )
			continue;
		ASSERT( nRowSize > 0 );
		CExtControlBar * pExtBar = pVLI->m_vRow[ 0 ];
		ASSERT( pExtBar != NULL );
		if( bHaveDraggingBar && pVLI->m_bOverflowMode )
		{
			if( bHorz )
				pExtBar->m_sizeDockedH.cx = _size.cx / nRowSize;
			else
				pExtBar->m_sizeDockedV.cy = _size.cy / nRowSize;
		}
		
		LONG nMinHW = pExtBar->_CalcDesiredMinHW();
		LONG nMinVH = pExtBar->_CalcDesiredMinVH();
		pVLI->m_nRowMinMetric = bHorz
			? nMinVH
			: nMinHW;
		pVLI->m_nRowMetric = bHorz
			? pExtBar->m_sizeDockedH.cy
			: pExtBar->m_sizeDockedV.cx;
		_vl.m_nTotalMetric += pVLI->m_nRowMetric;

		INT nRowMinMetric = bHorz
			? nMinVH
			: nMinHW;
		INT nRowMinExtent = bHorz
			? nMinHW
			: nMinVH;
		INT nRowExtent = bHorz
			? pExtBar->m_sizeDockedH.cx
			: pExtBar->m_sizeDockedV.cy;
		for( INT nBar = 1; nBar < nRowSize; nBar++ )
		{
			pExtBar = pVLI->m_vRow[ nBar ];
			ASSERT( pExtBar != NULL );
			if( bHaveDraggingBar && pVLI->m_bOverflowMode )
			{
				if( bHorz )
					pExtBar->m_sizeDockedH.cx = _size.cx / nRowSize;
				else
					pExtBar->m_sizeDockedV.cy = _size.cy / nRowSize;
			}
			LONG nMinHW = pExtBar->_CalcDesiredMinHW();
			LONG nMinVH = pExtBar->_CalcDesiredMinVH();
			INT nBarMinMetric = bHorz
				? nMinVH
				: nMinHW;
			nRowMinMetric =
				min( nRowMinMetric, nBarMinMetric );
			nRowMinExtent += bHorz
				? nMinHW
				: nMinVH;
			nRowExtent += bHorz
				? pExtBar->m_sizeDockedH.cx
				: pExtBar->m_sizeDockedV.cy;
		}
		_vl.m_nTotalMinMetric += nRowMinMetric;
		_vl.m_nTotalMinExtent = max( nRowMinExtent, _vl.m_nTotalMinExtent );
		pVLI->m_nRowExtent = nRowExtent;
	} // for( INT nRow = 0; nRow < nRowsCount; nRow ++ )

}

void CExtDockDynBar::_VisibleLayoutAlign(
	VisibleLayout_t & _vl,
	CSize _size
	)
{
	ASSERT_VALID( this );
	ASSERT( !_vl.IsEmpty() );
	ASSERT( _size.cx > 0 && _size.cy > 0 );
	if( ! ( m_bHelperLockNearestLayout || m_bHelperSizing || _vl.m_nDelayShowCount > 0L ) )
		return;
	m_bHelperLockNearestLayout = m_bHelperSizing = false;
CRect rcDockBarReal;
	GetClientRect( &rcDockBarReal );
	_size = rcDockBarReal.Size();
	if( _size.cx <= 0 || _size.cy <= 0 )
		return;
UINT nOwnID = GetDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nOwnID );
BOOL bHorz = ( nOwnID == AFX_IDW_DOCKBAR_TOP || nOwnID == AFX_IDW_DOCKBAR_BOTTOM );
INT nRowsCount = (INT)_vl.m_vRows.GetSize();
	ASSERT( nRowsCount > 0 );
INT nDesiredMetric = bHorz ? _size.cy : _size.cx;
INT nIncrement = ( _vl.m_nTotalMetric < nDesiredMetric ) ? 1 : -1;
INT nDiff = abs(_vl.m_nTotalMetric - nDesiredMetric);
	// align direction 1 (row metrics)
bool bResetRowMetrics = true;
	while( nDiff != 0 )
	{
		bResetRowMetrics = false;
		ASSERT( nDiff >= 0 );
		INT nRowsMinSized = 0;
		for( INT nRow = 0; nRow < nRowsCount; nRow ++ )
		{
			VisibleLayoutItem_t * pVLI = _vl.m_vRows[ nRow ];
			ASSERT( pVLI != NULL );
			//ASSERT( !pVLI->IsEmpty() );
			INT nRowSize = (INT)pVLI->m_vRow.GetSize();
			// TO FIX:
			if( nRowSize == 0 )
				continue;
			ASSERT( nRowSize > 0 );

			if( nIncrement < 0 && pVLI->IsRowMinSized() )
			{
				nRowsMinSized ++;
				if( nRowsCount == nRowsMinSized )
					break;
				continue;
			}
			pVLI->m_nRowMetric += nIncrement;
			nDiff--;

			for( INT nBar = 0; nBar < nRowSize; nBar++ )
			{
				CExtControlBar * pExtBar = pVLI->m_vRow[ nBar ];
				ASSERT( pExtBar != NULL );
				LONG & nRowMetricRef = bHorz
					? pExtBar->m_sizeDockedH.cy
					: pExtBar->m_sizeDockedV.cx;
				nRowMetricRef += nIncrement;
			} // for( INT nBar = 0; nBar < nRowSize; nBar++ )

			if( nDiff == 0 )
				break;
		} // for( INT nRow = 0; nRow < nRowsCount; nRow ++ )
		if( nRowsCount == nRowsMinSized )
			break;
	} // while( nDiff != 0 )
	if( bResetRowMetrics )
	{
		for( INT nRow = 0; nRow < nRowsCount; nRow ++ )
		{
			VisibleLayoutItem_t * pVLI = _vl.m_vRows[ nRow ];
			ASSERT( pVLI != NULL );
			INT nRowSize = (INT)pVLI->m_vRow.GetSize();
			if( nRowSize == 0 )
				continue;
			ASSERT( nRowSize > 0 );
			for( INT nBar = 0; nBar < nRowSize; nBar++ )
			{
				CExtControlBar * pExtBar = pVLI->m_vRow[ nBar ];
				ASSERT( pExtBar != NULL );
				LONG & nRowMetricRef = bHorz
					? pExtBar->m_sizeDockedH.cy
					: pExtBar->m_sizeDockedV.cx;
				nRowMetricRef = pVLI->m_nRowMetric;
			} // for( INT nBar = 0; nBar < nRowSize; nBar++ )
		} // for( INT nRow = 0; nRow < nRowsCount; nRow ++ )
	} // if( bResetRowMetrics )

	// align direction 2 (row extents)
	nDesiredMetric = bHorz ? _size.cx : _size.cy;
INT nRow = 0;
bool bFinish = false;
	for( nRow = 0; nRow < nRowsCount; nRow ++ )
	{
		VisibleLayoutItem_t * pVLI = _vl.m_vRows[ nRow ];
		ASSERT( pVLI != NULL );
		if( pVLI->IsEmpty() )
			continue;
		INT nIncrement = ( pVLI->m_nRowExtent < nDesiredMetric ) ? 1 : -1;
		INT nDiff = abs( pVLI->m_nRowExtent - nDesiredMetric );

		bool bHaveDisplayingBars = false;
		for( int nDisplaying = 0; nDisplaying < 2 && nDiff != 0; nDisplaying++ )
		{
			bool bAllMinSized = false;
			INT nPass = 0;
			for( ; nDiff != 0; nPass ++ )
			{
				ASSERT( nDiff >= 0 );
				INT nRowSize = (INT)pVLI->m_vRow.GetSize();
				if( nRowSize == 0 )
					continue;
				ASSERT( nRowSize > 0 );
				INT nBarsMinSized = 0;
				INT nCountDisplaying = 0;
				for( INT nBar = 0; nBar < nRowSize; nBar++ )
				{
					CExtControlBar * pExtBar = pVLI->m_vRow[ nBar ];
					ASSERT( pExtBar != NULL );
					if( pExtBar->_DisplayingGet() )
					{
						nCountDisplaying ++;
						if(		nCountDisplaying == nRowSize
							||	nPass > nRowSize
							)
						{
							bHaveDisplayingBars = false;
							bFinish = true;
							INT nRowSize = (INT)pVLI->m_vRow.GetSize();
							for( INT nBar = 0; nBar < nRowSize; nBar++ )
							{
								CExtControlBar * pExtBar = pVLI->m_vRow[ nBar ];
								ASSERT( pExtBar != NULL );
								if( pExtBar->_DisplayingGet() )
								{
									if( nPass <= nRowSize )
										bFinish = false;
									pExtBar->_DisplayingSet( false );
								}
							} // for( INT nBar = 0; nBar < nRowSize; nBar++ )
							if( bFinish )
								break;
						}
						if(		nDisplaying == 0
							&&	nRowSize > 1
							)
						{
							bHaveDisplayingBars = true;
							continue;
						}
					} // if( pExtBar->_DisplayingGet() )
					LONG & nBarMetricRef = bHorz
						? pExtBar->m_sizeDockedH.cx
						: pExtBar->m_sizeDockedV.cy;

					if( nIncrement < 0 )
					{
						LONG nBarMinMetric = bHorz
							? pExtBar->_CalcDesiredMinHW()
							: pExtBar->_CalcDesiredMinVH();
						//ASSERT( nBarMetricRef >= nBarMinMetric );
						if( nBarMetricRef < nBarMinMetric )
							nBarMetricRef = nBarMinMetric;
						if( nBarMetricRef <= nBarMinMetric )
						{
							nBarsMinSized ++;
							if( nBarsMinSized == nRowSize )
							{
								if( nDisplaying > 0 )
									bAllMinSized = true;
								break;
							}
							continue;
						}
					} // if( nIncrement < 0 )

					nBarMetricRef += nIncrement;
					nDiff --;
					if( nDiff == 0 )
						break;
				} // for( INT nBar = 0; nBar < nRowSize; nBar++ )
				if( bFinish )
					break;
				if( nBarsMinSized == nRowSize )
				{
					if( nDisplaying > 0 )
						bAllMinSized = true;
					break;
				}
// 				if( nDisplaying == 0 )
// 					break;
			} // for( ; nDiff != 0; nPass ++ )
			if( bFinish )
				break;
			if( bAllMinSized && nDisplaying > 0  )
				break;
			if( bHaveDisplayingBars && nDisplaying == 0 )
			{
				INT nRowSize = (INT)pVLI->m_vRow.GetSize();
				for( INT nBar = 0; nBar < nRowSize; nBar++ )
				{
					CExtControlBar * pExtBar = pVLI->m_vRow[ nBar ];
					ASSERT( pExtBar != NULL );
					pExtBar->_DisplayingSet( false );
				} // for( INT nBar = 0; nBar < nRowSize; nBar++ )
				bHaveDisplayingBars = false;
			} // if( bHaveDisplayingBars && nDisplaying == 0 )
		} // for( int nDisplaying = 0; nDisplaying < 2 && nDiff != 0; nDisplaying++ )
	} // for( nRow = 0; nRow < nRowsCount; nRow ++ )

	ASSERT( _vl.m_nTotalBarsCount > 0 );
HANDLE hDWP = ::BeginDeferWindowPos( _vl.m_nTotalBarsCount );
	ASSERT( hDWP != NULL );
	
	// apply new layout
CPoint ptOffs( 0, 0 );	
	for( nRow = 0; nRow < nRowsCount; nRow ++ )
	{
		VisibleLayoutItem_t * pVLI = _vl.m_vRows[ nRow ];
		ASSERT( pVLI != NULL );
		//ASSERT( !pVLI->IsEmpty() );
		INT nRowSize = (INT)pVLI->m_vRow.GetSize();
		if( nRowSize == 0 )
			continue;
		ASSERT( nRowSize > 0 );
		INT nBar, nRowOffset = 0;
		for( nBar = 0; nBar < nRowSize; nBar++ )
		{
			CExtControlBar * pExtBar = pVLI->m_vRow[ nBar ];
			ASSERT( pExtBar != NULL );
			if( nBar == 0 )
				nRowOffset = bHorz
					? pExtBar->m_sizeDockedH.cy
					: pExtBar->m_sizeDockedV.cx;
			CRect rcBar(
				ptOffs,
				bHorz
					? pExtBar->m_sizeDockedH
					: pExtBar->m_sizeDockedV
				);
			if( hDWP != NULL )
			{
				hDWP =
					::DeferWindowPos(
						hDWP,
						pExtBar->GetSafeHwnd(),
						NULL,
						rcBar.left, rcBar.top,
						rcBar.Width(), rcBar.Height(),
						SWP_NOZORDER|SWP_NOOWNERZORDER
							|SWP_NOACTIVATE|SWP_FRAMECHANGED
						);
				ASSERT( hDWP != NULL );
			}
			if( hDWP == NULL )
				pExtBar->MoveWindow( &rcBar );
			if( bHorz )
				ptOffs.x += pExtBar->m_sizeDockedH.cx;
			else
				ptOffs.y += pExtBar->m_sizeDockedV.cy;
			pExtBar->m_bDelayShowInvoked = false;
		} // for( INT nBar = 0; nBar < nRowSize; nBar++ )
		if( bHorz )
		{
			ptOffs.x = 0;
			ptOffs.y += nRowOffset;
		}
		else
		{
			ptOffs.x += nRowOffset;
			ptOffs.y = 0;
		}
	} // for( nRow = 0; nRow < nRowsCount; nRow ++ )

	ASSERT( hDWP != NULL );
	if( hDWP != NULL )
		::EndDeferWindowPos( hDWP );


	for( nRow = 0; nRow < nRowsCount; nRow ++ )
	{
		VisibleLayoutItem_t * pVLI = _vl.m_vRows[ nRow ];
		ASSERT( pVLI != NULL );
		INT nRowSize = (INT)pVLI->m_vRow.GetSize();
		if( nRowSize == 0 )
			continue;
		ASSERT( nRowSize > 0 );
		for( INT nBar = 0; nBar < nRowSize; nBar++ )
		{
			CExtControlBar * pExtBar = pVLI->m_vRow[ nBar ];
			ASSERT( pExtBar != NULL );
			pExtBar->_DisplayingSet( false );
//if( bHorz )
//	pExtBar->m_pDockContext->m_sizeLast = pExtBar->m_sizeDockedH;
//else
//	pExtBar->m_pDockContext->m_sizeLast = pExtBar->m_sizeDockedV;

//			HWND hWnd = pExtBar->GetParent()->GetSafeHwnd();
//			for( ; hWnd != NULL; hWnd = ::GetParent( hWnd ) )
//			{
//				if( ((::__EXT_MFC_GetWindowLong(hWnd,GWL_STYLE))&WS_CHILD) == 0 )
//					break;
//				CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
//				if(		pWnd == NULL
//					||	pWnd->IsKindOf( RUNTIME_CLASS( CFrameWnd ) )
//					)
//					break;
//				CExtControlBar * pBar = DYNAMIC_DOWNCAST( CExtControlBar, pWnd );
//			if( pBar == NULL )
//					continue;
//				pBar->_DisplayingSet( false );
//			}
			pExtBar->OnRepositionSingleChild();
		} // for( INT nBar = 0; nBar < nRowSize; nBar++ )
	} // for( nRow = 0; nRow < nRowsCount; nRow ++ )
}

IMPLEMENT_DYNCREATE(CExtDynControlBar, CExtControlBar);

BEGIN_MESSAGE_MAP(CExtDynControlBar, CExtControlBar)
	//{{AFX_MSG_MAP(CExtDynControlBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CExtDynControlBar::CExtDynControlBar()
	: m_pWndDynDocker( NULL )
	, m_bHelperDockSiteModified( false )
	, m_nDelVisUpdateCounter( 0 )
{
	m_bAppearInDockSiteControlBarPopupMenu = false;
	m_bAutoDelete = TRUE;
}

CExtDockDynBar * CExtDynControlBar::OnCreateDynamicDockBarObject()
{
	return ( new CExtDockDynBar( 0 ) );
}

void CExtDynControlBar::PostNcDestroy()
{
	CExtControlBar::PostNcDestroy();
}

BOOL CExtDynControlBar::DestroyWindow()
{
	return CExtControlBar::DestroyWindow();
}

LRESULT CExtDynControlBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
LRESULT lResult = CExtControlBar::WindowProc( message,  wParam,  lParam );
	return lResult;
}

int CExtDynControlBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if( CExtControlBar::OnCreate(lpCreateStruct) == -1 )
	{
		ASSERT( FALSE );
		return -1;
	}

UINT nOwnID = GetDlgCtrlID();

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
	if( IsKindOf( RUNTIME_CLASS(CExtDynTabControlBar) )
		&& nOwnID == AFX_IDW_DOCKBAR_FLOAT
		)
	{
		nOwnID = AFX_IDW_DOCKBAR_TOP;
		SetDlgCtrlID( nOwnID );
	}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nOwnID );

	ASSERT( m_pWndDynDocker == NULL );
	m_pWndDynDocker = OnCreateDynamicDockBarObject();
	ASSERT( m_pWndDynDocker != NULL );
	
CFrameWnd * pParentFrame =
		STATIC_DOWNCAST( CFrameWnd, GetParent() );
	ASSERT_VALID( pParentFrame );
	if( !m_pWndDynDocker->Create(
			pParentFrame,
			WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN
				|CBRS_ALIGN_TOP //|CBRS_ALIGN_ANY
				|CBRS_BORDER_ANY
				,
			nOwnID
			)
		)
	{
		ASSERT( FALSE );
		return -1;
	}
	m_pWndDynDocker->SetParent( this );
	EnableToolTips();
	return 0;
}

bool CExtDynControlBar::NcButtons_HandleClick(
	CExtBarNcAreaButton * pNcAreaButton,
	CPoint point,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
#ifdef _DEBUG
	if( pBarEventSource != NULL )
	{
		ASSERT_VALID( pNcAreaButton );
	}
	if( pBarEventSource != NULL )
	{
		ASSERT_VALID( pBarEventSource );
	}
#endif // _DEBUG
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
CExtControlBar * pActiveInTabbedBar = NULL;
CExtDynTabControlBar * pTabbedBar = NULL;
	if( CExtControlBar::g_bCloseOnlyOneBarInTabGroup )
	{
		pTabbedBar =
			DYNAMIC_DOWNCAST( CExtDynTabControlBar, this );
		if( pTabbedBar != NULL )
		{
			LONG nTabSel = pTabbedBar->GetSwitcherSelection();
			if( nTabSel >= 0 )
				pActiveInTabbedBar = pTabbedBar->GetBarAt( nTabSel, true );
		} // if( pTabbedBar != NULL )
	} // if( CExtControlBar::g_bCloseOnlyOneBarInTabGroup )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
bool bRetVal = false;
HWND hWndOwn = GetSafeHwnd();
	ASSERT( hWndOwn != NULL && ::IsWindow( hWndOwn ) );
CExtDockDynBar * _pWndDynDocker = m_pWndDynDocker;
	ASSERT_VALID( _pWndDynDocker );
HWND hWndDynDocker = _pWndDynDocker->GetSafeHwnd();
	ASSERT( hWndDynDocker != NULL && ::IsWindow( hWndDynDocker ) );
INT nBar, nCount = (INT)_pWndDynDocker->m_arrBars.GetSize();
CList < HWND, HWND > _listWndsToNotify;
	for( nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		if( CExtControlBar::g_bCloseOnlyOneBarInTabGroup && pTabbedBar != NULL )
		{
			if( pExtBar != pActiveInTabbedBar )
				continue;
		} // if( CExtControlBar::g_bCloseOnlyOneBarInTabGroup && pTabbedBar != NULL )
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
		ASSERT( pExtBar->GetSafeHwnd() != NULL );
		_listWndsToNotify.AddTail( pExtBar->m_hWnd );
	} // for( nBar = 0; nBar < nCount; nBar++ )
POSITION pos = _listWndsToNotify.GetHeadPosition();
HWND hWndBarEventSource = pBarEventSource->GetSafeHwnd();
	for( ; pos != NULL; )
	{
		HWND hWnd = _listWndsToNotify.GetNext( pos );
		if( ! ::IsWindow( hWnd ) )
			continue;
		CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
		if( pWnd == NULL )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pWnd );
		if( pExtBar == NULL )
			continue;
		if( pExtBar->NcButtons_HandleClick(
				pNcAreaButton,
				point,
				pBarEventSource,
				pBarActiveInContainer
				)
			)
			bRetVal = true;
		if( ! :: IsWindow( hWndBarEventSource ) )
		{
			pNcAreaButton = NULL;
			pBarEventSource = NULL;
			pBarActiveInContainer = NULL;
		}
	} // for( ; pos != NULL; )
	if( ! ::IsWindow( hWndOwn ) )
		return true;
	if( CWnd::FromHandlePermanent( hWndOwn ) != this )
		return true;
	if( ! ::IsWindow( hWndDynDocker ) )
		return true;
	if( CWnd::FromHandlePermanent( hWndDynDocker ) != _pWndDynDocker )
		return true;
	if( ! bRetVal )
	{
		if( ! CExtControlBar::NcButtons_HandleClick(
				pNcAreaButton,
				point,
				pBarEventSource,
				pBarActiveInContainer
				)
			)
			bRetVal = false;
	} // if( ! bRetVal )
	return bRetVal;
}

INT CExtDynControlBar::_GripperWidthAtLeftGet() const
{
	ASSERT_VALID( this );
	return 0;
}

INT CExtDynControlBar::_GripperHeightAtTopGet() const
{
	ASSERT_VALID( this );
	return 0;
}

void CExtDynControlBar::OnRepositionSingleChild(
	int cx, // = -1 // if negative - get from client area
	int cy, // = -1
	bool bRedraw // = true
	)
{
	ASSERT_VALID( this );
	if( m_pWndDynDocker->GetSafeHwnd() == NULL )
		return;
	ASSERT_VALID( m_pWndDynDocker );
CRect rcCurrent;
	m_pWndDynDocker->GetWindowRect( &rcCurrent );
	ScreenToClient( &rcCurrent );
CRect rcClient;
	if( cx < 0 || cy < 0 )
		GetClientRect( &rcClient );
	else
		rcClient.SetRect( 0, 0, cx, cy );
	if( rcCurrent != rcClient )
		m_pWndDynDocker->MoveWindow( &rcClient, bRedraw ? TRUE : FALSE );
	else if( bRedraw )
	{
		CExtDockDynBar::VisibleLayout_t _vl;
		m_pWndDynDocker->_VisibleLayoutBuild( _vl );
		CSize _size = rcClient.Size();
		if( (! _vl.IsEmpty() ) && _size.cx > 0 && _size.cy > 0 )
		{
			m_pWndDynDocker->m_bHelperSizing = true;
			m_pWndDynDocker->_VisibleLayoutAlign( _vl, _size );
		}
	}
	m_pWndDynDocker->OnDynamicLayoutUpdate();
}

void CExtDynControlBar::NcButtons_PostClick(
	CExtBarNcAreaButton * pNcAreaButton,
	CPoint point,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
#ifdef _DEBUG
	if( pBarEventSource != NULL )
	{
		ASSERT_VALID( pNcAreaButton );
	}
	if( pBarEventSource != NULL )
	{
		ASSERT_VALID( pBarEventSource );
	}
#endif // _DEBUG
HWND hWndOwn = GetSafeHwnd();
	ASSERT( hWndOwn != NULL && ::IsWindow( hWndOwn ) );
CExtDockDynBar * _pWndDynDocker = m_pWndDynDocker;
	ASSERT_VALID( _pWndDynDocker );
HWND hWndDynDocker = _pWndDynDocker->GetSafeHwnd();
	ASSERT( hWndDynDocker != NULL && ::IsWindow( hWndDynDocker ) );
INT nBar, nCount = (INT)_pWndDynDocker->m_arrBars.GetSize();
CList < HWND, HWND > _listWndsToNotify;
	for( nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
		ASSERT( pExtBar->GetSafeHwnd() != NULL );
		_listWndsToNotify.AddTail( pExtBar->m_hWnd );
	} // for( nBar = 0; nBar < nCount; nBar++ )
POSITION pos = _listWndsToNotify.GetHeadPosition();
HWND hWndBarEventSource = pBarEventSource->GetSafeHwnd();
	for( ; pos != NULL; )
	{
		HWND hWnd = _listWndsToNotify.GetNext( pos );
		if( ! ::IsWindow( hWnd ) )
			continue;
		CWnd * pWnd = CWnd::FromHandlePermanent( hWnd );
		if( pWnd == NULL )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pWnd );
		if( pExtBar == NULL )
			continue;
		pExtBar->NcButtons_PostClick(
			pNcAreaButton,
			point,
			pBarEventSource,
			pBarActiveInContainer
			);
		if( ! ::IsWindow( hWndOwn ) )
			return;
		if( CWnd::FromHandlePermanent( hWndOwn ) != this )
			return;
		if( ! ::IsWindow( hWndDynDocker ) )
			return;
		if( CWnd::FromHandlePermanent( hWndDynDocker ) != _pWndDynDocker )
			return;
		if( ! :: IsWindow( hWndBarEventSource ) )
		{
			pNcAreaButton = NULL;
			pBarEventSource = NULL;
			pBarActiveInContainer = NULL;
		}
	} // for( ; pos != NULL; )
	if( ! ::IsWindow( hWndOwn ) )
		return;
	if( CWnd::FromHandlePermanent( hWndOwn ) != this )
		return;
	if( ! ::IsWindow( hWndDynDocker ) )
		return;
	if( CWnd::FromHandlePermanent( hWndDynDocker ) != _pWndDynDocker )
		return;
	CExtControlBar::NcButtons_PostClick(
		pNcAreaButton,
		point,
		pBarEventSource,
		pBarActiveInContainer
		);
}

bool CExtDynControlBar::NcButtons_HandleQueryVisibility(
	bool & bButtonIsVisible,
	CExtBarNcAreaButton * pNcAreaButton,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNcAreaButton );
	ASSERT_VALID( pBarEventSource );
#if (defined _DEBUG)
	if( pBarActiveInContainer != NULL )
	{
		ASSERT_VALID( pBarActiveInContainer );
	}
#endif // (defined _DEBUG)
	ASSERT_VALID( m_pWndDynDocker );
INT nBar, nCount = (INT)m_pWndDynDocker->m_arrBars.GetSize();
	for( nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
		if( pExtBar->NcButtons_HandleQueryVisibility(
				bButtonIsVisible,
				pNcAreaButton,
				pBarEventSource,
				pBarActiveInContainer
				)
			)
			return true;
	} // for( nBar = 0; nBar < nCount; nBar++ )
	return
		CExtControlBar::NcButtons_HandleQueryVisibility(
			bButtonIsVisible,
			pNcAreaButton,
			pBarEventSource,
			pBarActiveInContainer
			);
}

bool CExtDynControlBar::NcButtons_HandleDraw(
	CDC & dc,
	CExtBarNcAreaButton * pNcAreaButton,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNcAreaButton );
	ASSERT_VALID( pBarEventSource );
#if (defined _DEBUG)
	if( pBarActiveInContainer != NULL )
	{
		ASSERT_VALID( pBarActiveInContainer );
	}
#endif // (defined _DEBUG)
	ASSERT_VALID( m_pWndDynDocker );
INT nBar, nCount = (INT)m_pWndDynDocker->m_arrBars.GetSize();
	for( nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
		if( pExtBar->NcButtons_HandleDraw(
				dc,
				pNcAreaButton,
				pBarEventSource,
				pBarActiveInContainer
				)
			)
			return true;
	} // for( nBar = 0; nBar < nCount; nBar++ )
	return
		CExtControlBar::NcButtons_HandleDraw(
			dc,
			pNcAreaButton,
			pBarEventSource,
			pBarActiveInContainer
			);
}

bool CExtDynControlBar::NcButtons_HandleHitTest(
	UINT & nExternalHT,
	CPoint point,
	CExtBarNcAreaButton * pNcAreaButton,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNcAreaButton );
	ASSERT_VALID( pBarEventSource );
#if (defined _DEBUG)
	if( pBarActiveInContainer != NULL )
	{
		ASSERT_VALID( pBarActiveInContainer );
	}
#endif // (defined _DEBUG)
	ASSERT_VALID( m_pWndDynDocker );
INT nBar, nCount = (INT)m_pWndDynDocker->m_arrBars.GetSize();
	for( nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
		if( pExtBar->NcButtons_HandleHitTest(
				nExternalHT,
				point,
				pNcAreaButton,
				pBarEventSource,
				pBarActiveInContainer
				)
			)
			return true;
	} // for( nBar = 0; nBar < nCount; nBar++ )
	return
		CExtControlBar::NcButtons_HandleHitTest(
			nExternalHT,
			point,
			pNcAreaButton,
			pBarEventSource,
			pBarActiveInContainer
			);
}

bool CExtDynControlBar::NcButtons_HandleQueryCursor(
	HCURSOR & hExternalCursor,
	CPoint point,
	CExtBarNcAreaButton * pNcAreaButton,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNcAreaButton );
	ASSERT_VALID( pBarEventSource );
#if (defined _DEBUG)
	if( pBarActiveInContainer != NULL )
	{
		ASSERT_VALID( pBarActiveInContainer );
	}
#endif // (defined _DEBUG)
	ASSERT_VALID( m_pWndDynDocker );
INT nBar, nCount = (INT)m_pWndDynDocker->m_arrBars.GetSize();
	for( nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
		if( pExtBar->NcButtons_HandleQueryCursor(
				hExternalCursor,
				point,
				pNcAreaButton,
				pBarEventSource,
				pBarActiveInContainer
				)
			)
			return true;
	} // for( nBar = 0; nBar < nCount; nBar++ )
	return
		CExtControlBar::NcButtons_HandleQueryCursor(
			hExternalCursor,
			point,
			pNcAreaButton,
			pBarEventSource,
			pBarActiveInContainer
			);
}

bool CExtDynControlBar::NcButtons_HandleReposition(
	CExtBarNcAreaButton * pPrevBtn,
	CExtBarNcAreaButton * pNcAreaButton,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNcAreaButton );
	ASSERT_VALID( pBarEventSource );
#if (defined _DEBUG)
	if( pPrevBtn != NULL )
	{
		ASSERT_VALID( pPrevBtn );
	}
#endif // (defined _DEBUG)
#if (defined _DEBUG)
	if( pBarActiveInContainer != NULL )
	{
		ASSERT_VALID( pBarActiveInContainer );
	}
#endif // (defined _DEBUG)
	ASSERT_VALID( m_pWndDynDocker );
INT nBar, nCount = (INT)m_pWndDynDocker->m_arrBars.GetSize();
	for( nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
		if( pExtBar->NcButtons_HandleReposition(
				pPrevBtn,
				pNcAreaButton,
				pBarEventSource,
				pBarActiveInContainer
				)
			)
			return true;
	} // for( nBar = 0; nBar < nCount; nBar++ )
	return
		CExtControlBar::NcButtons_HandleReposition(
			pPrevBtn,
			pNcAreaButton,
			pBarEventSource,
			pBarActiveInContainer
			);
}

bool CExtDynControlBar::NcButtons_HandleToolHitTest(
	__EXT_MFC_INT_PTR & hExternalToolHT,
	CPoint point,
	TOOLINFO * pTI,
	CExtBarNcAreaButton * pNcAreaButton,
	CExtControlBar * pBarEventSource,
	CExtControlBar * pBarActiveInContainer
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pNcAreaButton );
	ASSERT_VALID( pBarEventSource );
#if (defined _DEBUG)
	if( pBarActiveInContainer != NULL )
	{
		ASSERT_VALID( pBarActiveInContainer );
	}
#endif // (defined _DEBUG)
	ASSERT_VALID( m_pWndDynDocker );
INT nBar, nCount = (INT)m_pWndDynDocker->m_arrBars.GetSize();
	for( nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
		if( pExtBar->NcButtons_HandleToolHitTest(
				hExternalToolHT,
				point,
				pTI,
				pNcAreaButton,
				pBarEventSource,
				pBarActiveInContainer
				)
			)
			return true;
	} // for( nBar = 0; nBar < nCount; nBar++ )
	return
		CExtControlBar::NcButtons_HandleToolHitTest(
			hExternalToolHT,
			point,
			pTI,
			pNcAreaButton,
			pBarEventSource,
			pBarActiveInContainer
			);
}

bool CExtDynControlBar::FlashCaption(
	INT nFlashCount, // = -1 // negative value cancels current flashing
	INT nStepMilliseconds, // = 250
	COLORREF clrFlashText, // = RGB(255,255,255),
	COLORREF clrFlashBackground // = RGB(255,0,0)
	)
{
	ASSERT_VALID( this );
	nFlashCount;
	nStepMilliseconds;
	clrFlashText;
	clrFlashBackground;
	// control bar containers should not flash their captions
	return false;
}

UINT CExtDynControlBar::TearOffCommandGet() const
{
	ASSERT_VALID( this );
	return 0;
}

void CExtDynControlBar::TearOffCommandSet( UINT nCmdID )
{
	ASSERT_VALID( this );
	nCmdID;
	ASSERT( FALSE );
}

bool CExtDynControlBar::_FloatStateChangingIsEnabled(
	const CExtControlBar * pBarQuerySrc
	) const
{
	ASSERT_VALID( this );
	if( m_pWndDynDocker == NULL )
		return CExtControlBar::_FloatStateChangingIsEnabled( pBarQuerySrc );
	ASSERT_VALID( m_pWndDynDocker );
 	ASSERT_VALID( pBarQuerySrc );
int nCount = (int)m_pWndDynDocker->m_arrBars.GetSize();
	for( int nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
		if( ! pExtBar->_FloatStateChangingIsEnabled( pBarQuerySrc ) )
			return false;
	} // for( int nBar = 0; nBar < nCount; nBar++ )
	return CExtControlBar::_FloatStateChangingIsEnabled( pBarQuerySrc );
}

bool CExtDynControlBar::_NcMenuButtonIsEnabled(
	const CExtControlBar * pBarQuerySrc
	) const
{
	ASSERT_VALID( this );
	if( m_pWndDynDocker == NULL )
		return CExtControlBar::_NcMenuButtonIsEnabled( pBarQuerySrc );
	ASSERT_VALID( m_pWndDynDocker );
 	ASSERT_VALID( pBarQuerySrc );
int nCount = (int)m_pWndDynDocker->m_arrBars.GetSize();
	for( int nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
		if( ! pExtBar->_NcMenuButtonIsEnabled( pBarQuerySrc ) )
			return false;
	} // for( int nBar = 0; nBar < nCount; nBar++ )
	return CExtControlBar::_NcMenuButtonIsEnabled( pBarQuerySrc );
}

bool CExtDynControlBar::_ClosingIsEnabled(
	const CExtControlBar * pBarQuerySrc
	) const
{
	ASSERT_VALID( this );
	if( m_pWndDynDocker == NULL )
		return CExtControlBar::_ClosingIsEnabled( pBarQuerySrc );
	ASSERT_VALID( m_pWndDynDocker );
 	ASSERT_VALID( pBarQuerySrc );
int nCount = (int)m_pWndDynDocker->m_arrBars.GetSize();
	for( int nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
		if( ! pExtBar->_ClosingIsEnabled( pBarQuerySrc ) )
			return false;
	} // for( int nBar = 0; nBar < nCount; nBar++ )
	return CExtControlBar::_ClosingIsEnabled( pBarQuerySrc );
}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
bool CExtDynControlBar::_AutoHidingIsEnabled(
	const CExtControlBar * pBarQuerySrc
	) const
{
	ASSERT_VALID( this );
	if( m_pWndDynDocker == NULL )
		return CExtControlBar::_AutoHidingIsEnabled( pBarQuerySrc );
	ASSERT_VALID( m_pWndDynDocker );
 	ASSERT_VALID( pBarQuerySrc );
int nCount = (int)m_pWndDynDocker->m_arrBars.GetSize();
	for( int nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
		if( ! pExtBar->_AutoHidingIsEnabled( pBarQuerySrc ) )
			return false;
	} // for( int nBar = 0; nBar < nCount; nBar++ )
	return CExtControlBar::_AutoHidingIsEnabled( pBarQuerySrc );
}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

bool CExtDynControlBar::_DraggingIsEnabled(
	const CExtControlBar * pBarQuerySrc
	) const
{
	ASSERT_VALID( this );
	if( m_pWndDynDocker == NULL )
		return CExtControlBar::_DraggingIsEnabled( pBarQuerySrc );
	ASSERT_VALID( m_pWndDynDocker );
 	ASSERT_VALID( pBarQuerySrc );
int nCount = (int)m_pWndDynDocker->m_arrBars.GetSize();
	for( int nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
		if( ! pExtBar->_DraggingIsEnabled( pBarQuerySrc ) )
			return false;
	} // for( int nBar = 0; nBar < nCount; nBar++ )
	return CExtControlBar::_DraggingIsEnabled( pBarQuerySrc );
}

bool CExtDynControlBar::_RowResizingIsEnabled(
	const CExtControlBar * pBarQuerySrc
	) const
{
	ASSERT_VALID( this );
	if( m_pWndDynDocker == NULL )
		return CExtControlBar::_RowResizingIsEnabled( pBarQuerySrc );
	ASSERT_VALID( m_pWndDynDocker );
 	ASSERT_VALID( pBarQuerySrc );
int nCount = (int)m_pWndDynDocker->m_arrBars.GetSize();
	for( int nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
		if( ! pExtBar->_RowResizingIsEnabled( pBarQuerySrc ) )
			return false;
	} // for( int nBar = 0; nBar < nCount; nBar++ )
	return CExtControlBar::_RowResizingIsEnabled( pBarQuerySrc );
}

bool CExtDynControlBar::_RowRecalcingIsEnabled(
	const CExtControlBar * pBarQuerySrc
	) const
{
	ASSERT_VALID( this );
	if( m_pWndDynDocker == NULL )
		return CExtControlBar::_RowRecalcingIsEnabled( pBarQuerySrc );
	ASSERT_VALID( m_pWndDynDocker );
	ASSERT_VALID( pBarQuerySrc );
int nCount = (int)m_pWndDynDocker->m_arrBars.GetSize();
	for( int nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
		if( ! pExtBar->_RowRecalcingIsEnabled( pBarQuerySrc ) )
			return false;
	} // for( int nBar = 0; nBar < nCount; nBar++ )
	return CExtControlBar::_RowRecalcingIsEnabled( pBarQuerySrc );
}

bool CExtDynControlBar::IsBarWithGripper(
	bool * pbGripperAtTop, // = NULL
	bool * pbTextOnGripper // = NULL
	) const
{
	ASSERT_VALID( this );
	if( pbGripperAtTop != NULL )
		*pbGripperAtTop = false;
	if( pbTextOnGripper != NULL )
		*pbTextOnGripper = false;
	return false;
}

bool CExtDynControlBar::_DisplayingGet() const
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pWndDynDocker );
	if( CExtControlBar::_DisplayingGet() )
		return true;
int nCount = (int)m_pWndDynDocker->m_arrBars.GetSize();
	for( int nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if(		pExtBar != NULL
			&&	pExtBar->_DisplayingGet()
			)
			return true;
	} // for( int nBar = 0; nBar < nCount; nBar++ )
	return false;
}

void CExtDynControlBar::_DisplayingSet( bool bDisplaying )
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pWndDynDocker );
	CExtControlBar::_DisplayingSet( bDisplaying );
int nCount = (int)m_pWndDynDocker->m_arrBars.GetSize();
	for( int nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar != NULL )
			pExtBar->_DisplayingSet( bDisplaying );
	} // for( int nBar = 0; nBar < nCount; nBar++ )
}

void CExtDynControlBar::OnShowControlBarChain(
	bool bShow,
	bool bDelay,
	bool bOnlyOneBar // = false
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pWndDynDocker );
CFrameWnd * pFrame = _GetDockingFrameImpl();
	ASSERT_VALID( pFrame );
int nCount = (int)m_pWndDynDocker->m_arrBars.GetSize();
	for( int nBar = 0; nBar < nCount; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)
			m_pWndDynDocker->m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar != NULL )
			pExtBar->OnShowControlBarChain(
				bShow,
				true, //bDelay
				bOnlyOneBar
				);
		else
			pFrame->ShowControlBar(
				pBar,
				bShow ? TRUE : FALSE,
				true //bDelay ? TRUE : FALSE
				);
	} // for( int nBar = 0; nBar < nCount; nBar++ )
	_DisplayingSet( false );
	CExtControlBar::_DisplayingSet( true );
	pFrame->ShowControlBar(
		this,
		bShow ? TRUE : FALSE,
		true // bDelay ? TRUE : FALSE
		);
	if( ! bDelay )
	{
	 	CFrameWnd * pFrame = GetParentFrame();
		pFrame->RecalcLayout();
	}
}

void CExtDynControlBar::OnControlBarPositionChange(
	CExtControlBar * pBarEventSrc,
	e_control_bar_position_changing_t eCBPC,
	bool bInitial,
	bool bFinal
	)
{
	ASSERT_VALID( this );
	if( m_pWndDynDocker != NULL )
	{
		ASSERT_VALID( m_pWndDynDocker );
		int nCount = (int)m_pWndDynDocker->m_arrBars.GetSize();
		for( int nBar = 0; nBar < nCount; nBar++ )
		{
			CControlBar * pBar = (CControlBar *)
				m_pWndDynDocker->m_arrBars[nBar];
			if( pBar == NULL )
				continue;
			if( __PLACEHODLER_BAR_PTR(pBar) )
				continue;
			CExtControlBar * pExtBar =
				DYNAMIC_DOWNCAST( CExtControlBar, pBar );
			if( pExtBar == NULL )
				continue;
			pExtBar->OnControlBarPositionChange(
				pBarEventSrc,
				eCBPC,
				bInitial,
				bFinal
				);
		} // for( int nBar = 0; nBar < nCount; nBar++ )
	} // if( m_pWndDynDocker != NULL )
	CExtControlBar::OnControlBarPositionChange(
		pBarEventSrc,
		eCBPC,
		bInitial,
		bFinal
		);
}

void CExtDynControlBar::DoPaintNC( CDC * pDC )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pDC );
	ASSERT( pDC->GetSafeHdc() != NULL );

CWnd * pWnd = GetParent();
	if( pWnd->IsKindOf( RUNTIME_CLASS( CExtDockBar ) ) )
	{
		CExtControlBar::DoPaintNC( pDC );
		return;
	}

CRect rcClient, rcBar;
	GetClientRect( rcClient );
	ClientToScreen( rcClient );
	GetWindowRect( rcBar );
	rcClient.OffsetRect( -rcBar.TopLeft() );
	rcBar.OffsetRect( -rcBar.TopLeft() );
	pDC->ExcludeClipRect( rcClient );

	if(		(! PmBridge_GetPM()->GetCb2DbTransparentMode(this) )
		||	(! PmBridge_GetPM()->PaintDockerBkgnd( false, *pDC, this ) )
		)
		pDC->FillSolidRect(
			&rcBar,
			PmBridge_GetPM()->GetColor(
				CExtPaintManager::CLR_3DFACE_OUT, this
				)
			);
}

void CExtDynControlBar::DoPaint( CDC * pDC )
{
CWnd * pWnd = GetParent();
	if( pWnd->IsKindOf( RUNTIME_CLASS( CExtDockBar ) ) )
	{
		CExtControlBar::DoPaint( pDC );
		return;
	}
	if(		(! PmBridge_GetPM()->GetCb2DbTransparentMode(this) )
		||	(! PmBridge_GetPM()->PaintDockerBkgnd( true, *pDC, this ) )
		)
	{
		CRect rc;
		GetClientRect( &rc );
		pDC->FillSolidRect(
			&rc,
			PmBridge_GetPM()->GetColor(
				CExtPaintManager::CLR_3DFACE_OUT, this
				)
			);
	}
}

void CExtDynControlBar::DoEraseBk( CDC * pDC )
{
CWnd * pWnd = GetParent();
	if( pWnd->IsKindOf( RUNTIME_CLASS( CExtDockBar ) ) )
	{
		CExtControlBar::DoEraseBk( pDC );
		return;
	}
	if(		(! PmBridge_GetPM()->GetCb2DbTransparentMode(this) )
		||	(! PmBridge_GetPM()->PaintDockerBkgnd( true, *pDC, this ) )
		)
	{
		CRect rc;
		GetClientRect( &rc );
		pDC->FillSolidRect(
			&rc,
			PmBridge_GetPM()->GetColor(
				CExtPaintManager::CLR_3DFACE_OUT, this
				)
			);
	}
}

bool CExtDynControlBar::_CanDockToTabbedContainers(
	CExtControlBar * pDestBar
	) const
{
	ASSERT_VALID( this );
	if( ! CExtControlBar::_CanDockToTabbedContainers( pDestBar ) )
		return false;
	if( m_pWndDynDocker == NULL )
		return false;
	for( int i = 0; i < m_pWndDynDocker->m_arrBars.GetSize(); i++ )
	{
		CExtControlBar * pBar = (CExtControlBar *)
			m_pWndDynDocker->m_arrBars[i];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CExtControlBar, pBar );
		if( !pBar->_CanDockToTabbedContainers(pDestBar) )
			return false;
	} // for( int i = 0; i < m_pWndDynDocker->m_arrBars.GetSize(); i++ )
	return true;
}

#if (!defined __EXT_MFC_NO_TAB_CONTROLBARS)
CExtDynTabControlBar * CExtDynControlBar::_GetNearestTabbedContainer()
{
	ASSERT_VALID( this );
	return CExtControlBar::_GetNearestTabbedContainer();
}
#endif // (!defined __EXT_MFC_NO_TAB_CONTROLBARS)

void CExtDynControlBar::_OnNcMiniFrameActivate( bool bActive )
{
	ASSERT_VALID( this );
	if( m_pWndDynDocker == NULL )
		return;
	for( int i = 0; i < m_pWndDynDocker->m_arrBars.GetSize(); i++ )
	{
		CExtControlBar * pBar = (CExtControlBar *)
			m_pWndDynDocker->m_arrBars[i];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR(pBar) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CExtControlBar, pBar );
		pBar->_OnNcMiniFrameActivate( bActive );
	} // for( int i = 0; i < m_pWndDynDocker->m_arrBars.GetSize(); i++ )
}

CSize CExtDynControlBar::_CalcLayoutMinSize() const
{
	ASSERT_VALID( this );
CExtDockDynBar * pDockBar = 
		STATIC_DOWNCAST( CExtDockDynBar, GetWindow(GW_CHILD) );
	ASSERT_VALID( pDockBar );
	ASSERT_KINDOF( CExtDockDynBar, pDockBar );
CExtDockDynBar::VisibleLayout_t _vl;
	pDockBar->_VisibleLayoutBuild( _vl );
UINT nDockBarID = pDockBar->GetDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );
BOOL bHorz = ( nDockBarID == AFX_IDW_DOCKBAR_TOP || nDockBarID == AFX_IDW_DOCKBAR_BOTTOM );
CSize _size(
		bHorz ? _vl.m_nTotalMinMetric : _vl.m_nTotalMinExtent,
		bHorz ? _vl.m_nTotalMinExtent : _vl.m_nTotalMinExtent
		);
	return _size;
}

INT CExtDynControlBar::_CalcDesiredMinHW() const
{
//return _CalcLayoutMinSize().cx;
	if( GetSafeHwnd() == NULL )
		return 0;
CExtDockDynBar * pDockBar = 
		STATIC_DOWNCAST( CExtDockDynBar, GetWindow(GW_CHILD) );
	ASSERT_VALID( pDockBar );
	ASSERT_KINDOF( CExtDockDynBar, pDockBar );
INT nRetVal = 0;
LONG nBarIdx, nBarCount = LONG( pDockBar->m_arrBars.GetSize() );
	for( nBarIdx = 0; nBarIdx < nBarCount; nBarIdx++ )
	{
		CControlBar * pBar = (CControlBar *)
			pDockBar->m_arrBars[ nBarIdx ];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR( pBar ) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CControlBar, pBar );
		ASSERT( pBar->m_pDockBar == pDockBar );
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
		INT nMetric = pExtBar->_CalcDesiredMinHW();
		nRetVal = max( nRetVal, nMetric );
	} // for( nBarIdx = 0; nBarIdx < nBarCount; nBarIdx++ )
	return nRetVal;
}

INT CExtDynControlBar::_CalcDesiredMinVH() const
{
//return _CalcLayoutMinSize().cy;
CExtDockDynBar * pDockBar = 
		STATIC_DOWNCAST( CExtDockDynBar, GetWindow(GW_CHILD) );
	ASSERT_VALID( pDockBar );
	ASSERT_KINDOF( CExtDockDynBar, pDockBar );
INT nRetVal = 0;
LONG nBarIdx, nBarCount = LONG( pDockBar->m_arrBars.GetSize() );
	for( nBarIdx = 0; nBarIdx < nBarCount; nBarIdx++ )
	{
		CControlBar * pBar = (CControlBar *)
			pDockBar->m_arrBars[ nBarIdx ];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR( pBar ) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CControlBar, pBar );
		ASSERT( pBar->m_pDockBar == pDockBar );
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, pBar );
		if( pExtBar == NULL )
			continue;
		INT nMetric = pExtBar->_CalcDesiredMinVH();
		nRetVal = max( nRetVal, nMetric );
	} // for( nBarIdx = 0; nBarIdx < nBarCount; nBarIdx++ )
	return nRetVal;
}

CSize CExtDynControlBar::_CalcDesiredMinFloatedSize() const
{
	return _CalcLayoutMinSize();
}

bool CExtDynControlBar::_IsDelayedVisibilityUpdateEnabled() const
{
	ASSERT_VALID( this );
//	return ( m_nDelVisUpdateCounter != 0 ) ? true : false;
	return false;
}

void CExtDynControlBar::DelayShow( BOOL bShow )
{
	ASSERT_VALID( this );
	CExtControlBar::DelayShow( bShow );
//	if( m_bUpdatingChain )
//		return;
//	m_nDelVisUpdateCounter ++;
//	_UpdateVisibilityInChain();
//	_UpdateVisibilityInRow();
//	m_nDelVisUpdateCounter --;
}

CSize CExtDynControlBar::CalcDynamicLayout(
	int nLength,
	DWORD nMode
	)
{
CSize _size = CExtControlBar::CalcDynamicLayout( nLength, nMode );

	if( m_pDockBar == NULL ) // (+ v.2.23) - shutdown mode
		return _size;

	ASSERT_VALID( m_pDockBar );
	if( ((CExtDockBar *)m_pDockBar)->m_bLayoutQuery )
		return _size;

	if( m_pWndDynDocker->GetSafeHwnd() == NULL )
		return _size;

INT nDockedVisibleCount = m_pWndDynDocker->GetDockedVisibleCount();
	if( nDockedVisibleCount == 0 )
	{
		m_pDockSite->ShowControlBar( this, FALSE, FALSE );
		_size.cx = _size.cy = 0;
	}

CFrameWnd * pParentFrame = GetParentFrame();
	if(	pParentFrame->IsKindOf( RUNTIME_CLASS( CMiniFrameWnd ) ) )
	{
		ASSERT_VALID( m_pWndDynDocker );
		ASSERT(  m_pWndDynDocker->GetSafeHwnd() != NULL );
//!!//
//		m_pWndDynDocker->OnDynamicLayoutUpdate();
//		pParentFrame->SetWindowPos(
//			NULL, 0, 0, 0, 0,
//			SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE
//				|SWP_FRAMECHANGED
//			);
//		pParentFrame->SendMessage( WM_NCPAINT );

		ASSERT_VALID( m_pDockBar );
		if(	( ! m_pWndDynDocker->m_bLockedOptimize )
			&& pParentFrame == m_pDockBar->GetParent()
			)
			m_pWndDynDocker->OnDynamicLayoutUpdate();

	} // if( ....
	
	return _size;
}

void CExtDockDynBar::OnDynamicLayoutUpdate()
{
	ASSERT_VALID( this );
	ASSERT_KINDOF( CExtDockDynBar, this );

	if( m_bLockedOptimize )
		return;

	if( m_bInDynamicLayoutUpdate )
		return;
	m_bInDynamicLayoutUpdate = true;

UINT nDockBarID = GetDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );
BOOL bHorz = ( nDockBarID == AFX_IDW_DOCKBAR_TOP || nDockBarID == AFX_IDW_DOCKBAR_BOTTOM );
	
	CalcFixedLayout( TRUE, bHorz );

	m_bInDynamicLayoutUpdate = false;

	CExtDockBar::OnDynamicLayoutUpdate();
}

void CExtDockDynBar::OnDynamicLayoutOptimize()
{
	ASSERT_VALID( this );
	ASSERT_VALID( m_pDockSite );

	if( m_bLockedOptimize )
	{
#ifdef _DEBUG
	#ifdef __DEBUG_LOCKING_OPTIMIZATION_ASSERTS__
		INT nDockedCount = GetDockedCount();
		ASSERT( nDockedCount != 0 );
	#endif // __DEBUG_LOCKING_OPTIMIZATION_ASSERTS__
#endif // _DEBUG
		return;
	}

bool bOptimizeChilds = false;
INT nSinglePos = -1;
bool bPlaceHolder = false;
	ASSERT( m_arrBars.GetSize() >= 1 );
INT nDockedCount = GetDockedCount();
	if( nDockedCount != 0 )
	{
		ASSERT( nDockedCount >= 1 );
		for( INT nBar = 1; nBar < m_arrBars.GetSize(); nBar++ )
		{
			CControlBar * pBar = (CControlBar *)m_arrBars[ nBar ];
			if( pBar == NULL )
				continue;
			if( __PLACEHODLER_BAR_PTR(pBar) )
			{
				if( nSinglePos >= 0 && (!bPlaceHolder) )
				{
					bOptimizeChilds = true;
					break;
				}
				nSinglePos = nBar;
				bPlaceHolder = true;
				continue;
			}
			else
			{
				ASSERT_VALID( pBar );
				ASSERT_KINDOF( CControlBar, pBar );
				if( nSinglePos >= 0 )
				{
					bOptimizeChilds = true;
					break;
				}
				nSinglePos = nBar;
			}
		} // for( INT nBar = 1; nBar < m_arrBars.GetSize(); nBar++ )
	} // if( nDockedCount != 0 )
	if( bOptimizeChilds )
	{
		CExtDockBar::OnDynamicLayoutOptimize();
		return;
	}
CFrameWnd * pDockSite = m_pDockSite;
CMiniFrameWnd * pMiniFrame = DYNAMIC_DOWNCAST( CMiniFrameWnd, GetParentFrame() );
	if( pMiniFrame != NULL )
		pMiniFrame->DelayRecalcLayout();
CExtDynControlBar * pDynBar = STATIC_DOWNCAST( CExtDynControlBar, GetParent() );
	ASSERT_VALID( pDynBar );
	ASSERT( pDockSite == pDynBar->m_pDockSite );
CDockBar * pDockBarParent =  STATIC_DOWNCAST( CDockBar, pDynBar->GetParent() );
	ASSERT_VALID( pDockBarParent );
	ASSERT( pDockSite == pDockBarParent->m_pDockSite );
CExtDockDynBar * pDynDockBarParent =  DYNAMIC_DOWNCAST( CExtDockDynBar, pDockBarParent );
HWND hWndOwn = GetSafeHwnd();
	if( nSinglePos == -1 )
	{ // optimizing empty dynamic docker
		pDynBar->m_pDockBar = NULL;
//		pDockSite->RemoveControlBar( pDynBar );
		pDockSite->RemoveControlBar( this );
		ASSERT( pDynBar->m_bAutoDelete );
		HWND hWndDynBar = pDynBar->GetSafeHwnd();
		pDockBarParent->RemoveControlBar( pDynBar );
		if( ::IsWindow(hWndDynBar) )
			pDynBar->DestroyWindow();
		if( pMiniFrame == NULL )
			pDockSite->DelayRecalcLayout();
		ASSERT( pDockSite->m_listControlBars.Find(pDynBar) == NULL );
		if( ! (	::IsWindow(hWndOwn) ) )
			return;
		if( pDockBarParent->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
			((CExtDockBar *)pDockBarParent)->OnDynamicLayoutUpdate();
		return;
	} // optimizing empty dynamic docker

	if( bPlaceHolder )
	{ // optimizing placeholder dynamic docker
		return;
	} // optimizing placeholder dynamic docker

	// optimizing single bar dynamic docker
	pDynBar->DelayShow( FALSE );
CExtControlBar * pSingleBar = (CExtControlBar *)m_arrBars[ nSinglePos ];
	ASSERT_VALID( pSingleBar );
	ASSERT_KINDOF( CExtControlBar, pSingleBar );
	ASSERT( ! pSingleBar->IsFixedMode() );
CRect wrSingleBar;
	pDynBar->GetWindowRect( &wrSingleBar );
CSize sizeSingleBar = wrSingleBar.Size();
INT nDynPos = pDockBarParent->FindBar( pDynBar );
	ASSERT( nDynPos > 0 );
	pDockBarParent->m_arrBars[ nDynPos ] = pSingleBar;
	pSingleBar->m_pDockBar = pDockBarParent;
	if(		pSingleBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
		&&	(! ((CExtControlBar*)pSingleBar)->IsFixedMode() )
		)
	{
//		if( pDockBarParent->GetBarStyle() & CBRS_ORIENT_HORZ )
			((CExtControlBar*)pSingleBar)->SetInitDesiredSizeHorizontal( sizeSingleBar );
//		else
			((CExtControlBar*)pSingleBar)->SetInitDesiredSizeVertical( sizeSingleBar );
		pSingleBar->SetWindowPos( NULL, 0, 0, sizeSingleBar.cx, sizeSingleBar.cy, SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING|SWP_NOACTIVATE );
		DWORD dwStyle = pSingleBar->GetBarStyle();
		dwStyle &= ~(CBRS_ALIGN_ANY);
		dwStyle |=  (pDynBar->m_dwStyle & CBRS_ALIGN_ANY) | CBRS_BORDER_ANY;
		pSingleBar->SetBarStyle( dwStyle );
	}
	if( pDynDockBarParent != NULL )
		pDynDockBarParent->m_bHelperLockNearestLayout = true;
	m_arrBars.RemoveAll();
	m_arrBars.Add( (CControlBar *)NULL );
	pSingleBar->m_pDockBar = pDockBarParent;
	pSingleBar->_DisplayingSet( true );
	pSingleBar->SetParent( pDockBarParent );
pSingleBar->SetWindowPos( NULL, 0, 0, sizeSingleBar.cx, sizeSingleBar.cy, SWP_NOMOVE|SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_NOSENDCHANGING|SWP_NOACTIVATE );
((CExtControlBar*)pSingleBar)->SetInitDesiredSizeHorizontal( sizeSingleBar );
((CExtControlBar*)pSingleBar)->SetInitDesiredSizeVertical( sizeSingleBar );
	pSingleBar->_DisplayingSet( false );
	pSingleBar->m_pDockContext->m_rectLast = pDynBar->m_pDockContext->m_rectLast;
	pSingleBar->m_pDockContext->m_sizeLast = pDynBar->m_pDockContext->m_sizeLast;
	pSingleBar->m_pDockContext->m_rectMRUDockPos = pDynBar->m_pDockContext->m_rectMRUDockPos;
	pDynBar->m_pDockBar = NULL;
//	pDockSite->RemoveControlBar( pDynBar );
	pDockSite->RemoveControlBar( this );
	pDynBar->DestroyWindow();
	if( pMiniFrame == NULL )
		pDockSite->DelayRecalcLayout();
	ASSERT( pDockSite->m_listControlBars.Find(pDynBar) == NULL );
	if( pDockBarParent->IsKindOf(RUNTIME_CLASS(CExtDockBar)) )
		((CExtDockBar *)pDockBarParent)->OnDynamicLayoutUpdate();
	if( pDynDockBarParent != NULL )
		pDynDockBarParent->m_bHelperLockNearestLayout = true;
}

CSize CExtDockDynBar::CalcFixedLayout(
	BOOL bStretch,
	BOOL bHorz
	)
{
	ASSERT_VALID( this );
	ASSERT( ! m_bFloating );
	bHorz;
VisibleLayout_t _vl;
	_VisibleLayoutBuild( _vl );
CRect rect;
	GetWindowRect( &rect );
CSize _size = rect.Size();
	if( (! _vl.IsEmpty() ) && bStretch && _size.cx > 0 && _size.cy > 0 )
		_VisibleLayoutAlign( _vl, _size );
	return _size;
}

void CExtDockDynBar::CalcOrderedVector(
	ExtControlBarVector_t & vBars
	)
{
	ASSERT_VALID( this );

INT nCount = (INT)m_arrBars.GetSize();
	ASSERT( nCount > 0 );
	ASSERT( m_arrBars[0] == NULL );
	for( INT nBar = 1; nBar < nCount; nBar++ )
	{
		CExtControlBar * pBar = (CExtControlBar *) m_arrBars[nBar];
		if( pBar == NULL )
			continue;
		if( __PLACEHODLER_BAR_PTR( pBar ) )
			continue;
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CExtControlBar, pBar );
		ASSERT( !pBar->IsFixedMode() );
		vBars.Add( pBar );
	} // for( INT nBar = 1; nBar < nCount; nBar++ )
}

void CExtDockDynBar::OnSize(UINT nType, int cx, int cy) 
{
	m_bHelperSizing = true;
	CExtDockBar::OnSize(nType, cx, cy);
	if(		(! m_bHelperDockSiteModified )
		&&	m_pDockSite != NULL
		)
	{
		POSITION pos = m_pDockSite->m_listControlBars.Find( this );
		if( pos != NULL )
		{
			m_pDockSite->m_listControlBars.RemoveAt( pos );
			m_bHelperDockSiteModified = true;
		}
	}
/*
	if( nType == SIZE_MINIMIZED )
		return;
	if( cx <= 0 || cy <= 0 )
		return;

	OnDynamicLayoutUpdate();
*/

}

BOOL CExtDockDynBar::PreTranslateMessage(MSG* pMsg)
{
	return CExtDockBar::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////
// CExtDockOuterBar::OuterLayoutItemData

void CExtDockOuterBar::OuterLayoutItemData::_AssignFromOther(
	const CExtDockOuterBar::OuterLayoutItemData & other
	)
{
	m_pBar = other.m_pBar;
	m_bVisible = other.m_bVisible;
	m_sizeCalcDynamic = other.m_sizeCalcDynamic;
	m_sizeBarMin = other.m_sizeBarMin;
	m_rcPreCalc = other.m_rcPreCalc;
	m_rcReal = other.m_rcReal;
}

CExtDockOuterBar::OuterLayoutItemData::OuterLayoutItemData()
	: m_pBar( NULL )
	, m_bVisible( false )
	, m_sizeCalcDynamic( 0, 0 )
	, m_sizeBarMin( 0, 0 )
	, m_rcPreCalc( 0, 0, 0, 0 )
	, m_rcReal( 0, 0, 0, 0 )
{
}

CExtDockOuterBar::OuterLayoutItemData::OuterLayoutItemData(
		CControlBar * pBar,
		BOOL bHorz,
		const SIZE & sizeMax
		)
	: m_pBar( pBar )
	, m_sizeCalcDynamic( 0, 0 )
	, m_sizeBarMin( 0, 0 )
	, m_rcPreCalc( 0, 0, 0, 0 )
	, m_rcReal( 0, 0, 0, 0 )
{
	ASSERT_VALID( m_pBar );
	ASSERT_KINDOF( CControlBar, pBar );
	m_bVisible = pBar->IsVisible() ? true : false;
//	if( m_bVisible
//		&& (pBar->m_nStateFlags & CControlBar::delayHide) != 0
//		)
//		m_bVisible = false;
//	if( (!m_bVisible)
//		&& (pBar->m_nStateFlags & CControlBar::delayShow) != 0
//		)
//		m_bVisible = true;
	if( !m_bVisible )
		return;

//	for _CalcFixedRowLayout
CRect rcBarWin, rcBarClient;
	pBar->GetWindowRect( &rcBarWin );
	pBar->ScreenToClient( & rcBarWin );
	pBar->GetClientRect( &rcBarClient );
	m_sizeBarMin = rcBarWin.Size() - rcBarClient.Size();
	if(	pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )
	{
		m_sizeBarMin +=
			((CExtControlBar *)pBar)->
				_CalcDesiredMinOuterSize( bHorz );
	} // if(	pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )

	// get ideal rect for bar
DWORD dwMode = 0;
	if(	(pBar->m_dwStyle & CBRS_SIZE_DYNAMIC)
		&&
		(pBar->m_dwStyle & CBRS_FLOATING)
		)
		dwMode |= LM_HORZ | LM_MRUWIDTH;
	else if(pBar->m_dwStyle & CBRS_ORIENT_HORZ)
		dwMode |= LM_HORZ | LM_HORZDOCK;
	else
		dwMode |=  LM_VERTDOCK;

	m_sizeCalcDynamic = pBar->CalcDynamicLayout(-1, dwMode);
	if(		pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
		&&	((CExtControlBar *)pBar)->_GetFullRowMode()
		)
	{
		if(dwMode & LM_HORZDOCK)
			m_sizeCalcDynamic.cx = sizeMax.cx;
		else if(dwMode & LM_VERTDOCK)
			m_sizeCalcDynamic.cy = sizeMax.cy;
	}
}

/////////////////////////////////////////////////////////////////////////
// CExtDockOuterBar window

IMPLEMENT_DYNCREATE(CExtDockOuterBar, CExtDockBar);

BEGIN_MESSAGE_MAP(CExtDockOuterBar, CExtDockBar)
	//{{AFX_MSG_MAP(CExtDockOuterBar)
	ON_WM_TIMER()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_WM_SYSCOLORCHANGE()
	__EXT_MFC_SAFE_ON_WM_SETTINGCHANGE()
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_MESSAGE(__ExtMfc_WM_THEMECHANGED, OnThemeChanged)
END_MESSAGE_MAP()

CExtDockOuterBar::CExtDockOuterBar()
	: CExtDockBar( 0 )
	, m_bInDockSiteDelayedRecalc( false )
	, m_bDockSiteRecalcIsDelayed( false )
{
}

void CExtDockOuterBar::OnSysColorChange() 
{
	CExtDockBar::OnSysColorChange();
//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnSysColorChange( this );
//	g_CmdManager.OnSysColorChange( pPM, this );
}

void CExtDockOuterBar::OnSettingChange(UINT uFlags, __EXT_MFC_SAFE_LPCTSTR lpszSection) 
{
	CExtDockBar::OnSettingChange( uFlags, lpszSection );
//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnSettingChange( this, uFlags, lpszSection );
//	g_CmdManager.OnSettingChange( pPM, this, uFlags, lpszSection );
}

LRESULT CExtDockOuterBar::OnDisplayChange( WPARAM wParam, LPARAM lParam )
{
//LRESULT lResult = CExtDockBar::OnDisplayChange( wParam, lParam );


	CExtDockBar::OnDisplayChange(wParam, LOWORD(lParam), HIWORD(lParam));

LRESULT lResult = 0;


//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnDisplayChange( this, (INT)wParam, CPoint(lParam) );
//	g_CmdManager.OnDisplayChange( pPM, this, (INT)wParam, CPoint(lParam) );
	return lResult;
}

void CExtDockOuterBar::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if( CExtControlBar::FindHelpMode(this) )
		return;

#if (!defined __EXT_MFC_NO_CUSTOMIZE)
CExtCustomizeSite * pSite =
		CExtCustomizeSite::GetCustomizeSite( m_hWnd );
	if( pSite != NULL )
	{
		if( pSite->IsCustomizeMode() )
			return;
		pSite->DoFormCustomization();
		return;
	} // if( pSite != NULL )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)

	CExtDockBar::OnLButtonDblClk(nFlags, point);
}

LRESULT CExtDockOuterBar::OnThemeChanged( WPARAM wParam, LPARAM lParam )
{
	wParam;
	lParam;
LRESULT lResult = Default();
//CExtPaintManager * pPM = PmBridge_GetPM();
//	g_PaintManager.OnThemeChanged( this, wParam, lParam );
//	g_CmdManager.OnThemeChanged( pPM, this, wParam, lParam );
	return lResult;
}

void CExtDockOuterBar::OnDynamicLayoutUpdate()
{
	CExtDockBar::OnDynamicLayoutUpdate();
}

void CExtDockOuterBar::OnDynamicLayoutOptimize()
{
	CExtDockBar::OnDynamicLayoutOptimize();
}

void CExtDockOuterBar::_DelayDockSiteRecalc()
{
	ASSERT_VALID( this );
	if( m_bDockSiteRecalcIsDelayed )
		return;
CExtDockOuterBar * pDocker = this;
	if( GetDlgCtrlID() != AFX_IDW_DOCKBAR_TOP )
	{
		CFrameWnd * pFrame = STATIC_DOWNCAST( CFrameWnd, GetParent() );
		ASSERT_VALID( pFrame );
		CControlBar * pTempBar = pFrame->GetControlBar( AFX_IDW_DOCKBAR_TOP );
		if( pTempBar == NULL )
			pTempBar = pFrame->GetControlBar( AFX_IDW_DOCKBAR_BOTTOM );
		if( pTempBar == NULL )
			pTempBar = pFrame->GetControlBar( AFX_IDW_DOCKBAR_LEFT );
		if( pTempBar == NULL )
			pTempBar = pFrame->GetControlBar( AFX_IDW_DOCKBAR_RIGHT );
		if( pTempBar != NULL )
			pDocker = STATIC_DOWNCAST( CExtDockOuterBar, pTempBar );
	} // if( GetDlgCtrlID() != AFX_IDW_DOCKBAR_TOP )
	pDocker->_DelayDockSiteRecalc_Start();
}

void CExtDockOuterBar::_DelayDockSiteRecalc_Start()
{
	ASSERT_VALID( this );
	if( m_bDockSiteRecalcIsDelayed )
		return;
	m_bDockSiteRecalcIsDelayed = true;
	if( _DelayDockSiteRecalc_Update() )
	{
		KillTimer( 901 );
		m_bDockSiteRecalcIsDelayed = false;
	}
	else
		SetTimer( 901, 1, NULL );
}

bool CExtDockOuterBar::_DelayDockSiteRecalc_Update()
{
	ASSERT_VALID( this );
	if( m_bInDockSiteDelayedRecalc )
		return false;
CExtControlBar::InternalFriendlyFrameWnd * pFrame =
		(CExtControlBar::InternalFriendlyFrameWnd *)
			STATIC_DOWNCAST( CFrameWnd, GetParent() );
	ASSERT_VALID( pFrame );
	if(		pFrame->IsInRecalcLayout()
		||	CExtControlBar::ProfileBarStateIsSerializing( pFrame )
		||	(pFrame->GetStyle()&WS_VISIBLE) == 0
		)
		return false;
	m_bInDockSiteDelayedRecalc = true;
	pFrame->RecalcLayout();
	m_bInDockSiteDelayedRecalc = false;
	return true;
}

void CExtDockOuterBar::OnTimer(__EXT_MFC_UINT_PTR nIDEvent) 
{
	if( nIDEvent == 901 )
	{
		if( _DelayDockSiteRecalc_Update() )
		{
			KillTimer( 901 );
			m_bDockSiteRecalcIsDelayed = false;
		}
		return;
	} // if( nIDEvent == 901 )
	CExtDockBar::OnTimer(nIDEvent);
}

void CExtDockOuterBar::_AffixmentBringToTop( CExtControlBar * pBar )
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBar );
CExtControlBar::InternalAffixmentData * pAffixmentDataSrc =
		pBar->_AffixmentGetOuter();
	ASSERT( pAffixmentDataSrc != NULL );
	ASSERT( FindBar(pBar) >= 0 );
ExtControlBarVector_t vBars;
	pBar->_GetRowExtBars( vBars, false );
int nCount = (int)vBars.GetSize();
	ASSERT( nCount > 0 );
	pAffixmentDataSrc->m_nAffixmentWeight = 1;
///	pAffixmentDataSrc->m_rcAffixment.SetRectEmpty();
	if( nCount == 1 )
		return;
int nResetWeight = (int)m_arrBars.GetSize() * 4;
int nBar = 0; 
	for( nBar = 0; nBar < nCount; )
	{
		CExtControlBar * pBar2 = vBars[nBar];
		ASSERT_VALID( pBar2 );
		if( pBar2 == pBar )
		{
			nBar++;
			continue;
		}
		CExtControlBar::InternalAffixmentData * pAffixmentData2 =
			pBar2->_AffixmentGetOuter();
		if( pAffixmentData2 == NULL )
		{
			vBars.RemoveAt(nBar);
			nCount--;
			continue;
		}
		if( pAffixmentData2->IsEmpty() )
			pAffixmentData2->m_nAffixmentWeight = nResetWeight++;
		else
			pAffixmentData2->m_nAffixmentWeight++;
		nBar++;
	} // for( nBar = 0; nBar < nCount; )
	if( nCount <= 1 )
		return;
	// reset affixment weights
ExtControlBarVector_t vBars2;
	for( nBar = 0; nBar < nCount; nBar++ )
	{
		CExtControlBar * pBarX = vBars[nBar];
		int nCount2 = (int)vBars2.GetSize();
		if( nCount2 == 0 )
		{
			vBars2.Add( pBarX );
			continue;
		}
		CExtControlBar::InternalAffixmentData * pAffixmentDataX =
			pBarX->_AffixmentGetOuter();
		ASSERT( pAffixmentDataX != NULL );
		int nWeightX = pAffixmentDataX->m_nAffixmentWeight;
		for( int nBar2 = 0; nBar2 < nCount2; nBar2++ )
		{
			CExtControlBar * pBar2 = vBars2[nBar2];
			ASSERT( pBar2 != pBarX );
			CExtControlBar::InternalAffixmentData * pAffixmentData2 =
				pBar2->_AffixmentGetOuter();
			ASSERT( pAffixmentData2 != NULL );
			int nWeight2 = pAffixmentData2->m_nAffixmentWeight;
			ASSERT( nWeight2 != nWeightX );
			if( nWeight2 > nWeightX )
			{
				vBars2.InsertAt( nBar2, pBarX );
				break;
			}
			if( nBar2 == nCount2-1 )
			{
				vBars2.Add( pBarX );
				break;
			}
		} // for( int nBar2 = 0; nBar2 < nCount2; nBar2++ )
	} // for( nBar = 0; nBar < nCount; nBar++ )
	ASSERT( vBars2.GetSize() == nCount );
	ASSERT( vBars2[0]->_AffixmentGetOuter()->m_nAffixmentWeight == 1 );
//	if( nCount == 2 )
//		return;
	for( nBar = 1; nBar < nCount; nBar++ )
	{
		CExtControlBar * pBarX = vBars2[nBar];
		CExtControlBar::InternalAffixmentData * pAffixmentDataX =
			pBarX->_AffixmentGetOuter();
		ASSERT( pAffixmentDataX != NULL );
		pAffixmentDataX->m_nAffixmentWeight = nBar + 1;
	}
}

//#define __DEBUG_CExtDockOuterBar_CompressSubRow_ASSERT_CRITICAL_MINSIZE

bool CExtDockOuterBar::_CompressSubRow(
	CExtDockOuterBar::dockbar_outer_layout_t & dbol,
	int nSubRowStartPos,
	int nSubRowEndPos,
	int nExtentMax,
	bool bHorz,
	bool & bAdjustWithAffixment,
	bool * p_bFullyOccupiedLayout // = NULL
	)
{
	ASSERT_VALID(this);
	ASSERT( dbol.GetSize() > 0 );
	ASSERT( nSubRowStartPos <= nSubRowEndPos );
	ASSERT( 0 <= nSubRowStartPos && nSubRowStartPos < dbol.GetSize() );
	ASSERT( 0 <= nSubRowEndPos   && nSubRowEndPos < dbol.GetSize() );

CArray < CRect, CRect > arrRcPreCalc;
int nPossibleBetween = 0;
//int nPossibleCs = 0;
int nExtentReal = 0;
int nBar = 0;
	for( nBar = nSubRowStartPos; nBar <= nSubRowEndPos; nBar++ )
	{
		OuterLayoutItemData & olid = dbol.ElementAt( nBar );
		ASSERT_VALID( olid.m_pBar );
		ASSERT_KINDOF( CControlBar, olid.m_pBar );
		ASSERT( olid.m_bVisible );
		CRect rcPreCalc( olid.m_rcPreCalc );
		ASSERT( rcPreCalc.left >= 0 );
		ASSERT( rcPreCalc.top >= 0 );
		ASSERT( rcPreCalc.left <= rcPreCalc.right );
		ASSERT( rcPreCalc.top <= rcPreCalc.bottom );
#ifdef __DEBUG_CExtDockOuterBar_CompressSubRow_ASSERT_CRITICAL_MINSIZE
		ASSERT( rcPreCalc.Width() >= olid.m_sizeBarMin.cx );
		ASSERT( rcPreCalc.Height() >= olid.m_sizeBarMin.cy );
#endif // __DEBUG_CExtDockOuterBar_CompressSubRow_ASSERT_CRITICAL_MINSIZE
		int nBetween = 0;
		if( nBar == nSubRowStartPos )
		{
			nBetween += bHorz
				? rcPreCalc.left
				: rcPreCalc.top
				;
			ASSERT( nBetween >= 0 );
		} // if( nBar == nSubRowStartPos )
		else
		{
			if( bHorz )
			{
				nBetween +=
					rcPreCalc.left
					- arrRcPreCalc.ElementAt(nBar-1-nSubRowStartPos).right
					;
				ASSERT( nBetween >= 0 );
				if( nBar == nSubRowEndPos )
					nBetween += nExtentMax - rcPreCalc.right;
			} // if( bHorz )
			else
			{
				nBetween +=
					rcPreCalc.top
					- arrRcPreCalc.ElementAt(nBar-1-nSubRowStartPos).bottom
					;
				ASSERT( nBetween >= 0 );
				if( nBar == nSubRowEndPos )
					nBetween += nExtentMax - rcPreCalc.bottom;
			} // else from if( bHorz )
		} // else from if( nBar == nSubRowStartPos )
//		int nCsAvail = bHorz
//			? (rcPreCalc.Width() - olid.m_sizeBarMin.cx)
//			: (rcPreCalc.Height() - olid.m_sizeBarMin.cy)
//			;
//		ASSERT( nCsAvail >= 0 );
//		nPossibleCs += nCsAvail;
		nPossibleBetween += nBetween;
		arrRcPreCalc.Add( rcPreCalc );
		nExtentReal += bHorz
			? rcPreCalc.Width()
			: rcPreCalc.Height()
			;
	} // for( nBar = nSubRowStartPos; nBar <= nSubRowEndPos; nBar++ )

	ASSERT( arrRcPreCalc.GetSize() == (nSubRowEndPos-nSubRowStartPos+1) );

//int nPossible = nPossibleBetween + nPossibleCs;

bool bRetVal = false;
int nCsRow = nExtentReal - nExtentMax /*- nPossibleBetween*/; //nPossibleCs;
	if( nPossibleBetween > 0 )
		nCsRow -= nPossibleBetween;

	if( nPossibleBetween > 0 )
	{
		int nLeftPrev = nExtentMax;
		for( int nBar = nSubRowEndPos; nBar >= nSubRowStartPos; nBar-- )
		{
			CRect & rcPreCalc = arrRcPreCalc.ElementAt( nBar-nSubRowStartPos );
			int nShift =
				nLeftPrev
				- (bHorz ? rcPreCalc.right : rcPreCalc.bottom)
				;
			if( nShift < 0 )
				rcPreCalc.OffsetRect(
					bHorz ? nShift : 0,
					bHorz ? 0 : nShift
					);
			nLeftPrev = bHorz
				? rcPreCalc.left
				: rcPreCalc.top
				;
		} // for( int nBar = nSubRowEndPos; nBar >= nSubRowStartPos; nBar-- )
		if( nLeftPrev >= 0 )
			bRetVal = true;
//		else
//			nCsRow = -nLeftPrev;
	} // if( nPossibleBetween > 0 )

bool bCompressionPassed = false;
	if( (!bRetVal) && nCsRow >= 0 )
	{
		int nLeftPrev = nExtentMax;
		for( int nBar = nSubRowEndPos; nBar >= nSubRowStartPos; nBar-- )
		{
			OuterLayoutItemData & olid = dbol.ElementAt( nBar );
			CRect & rcPreCalc = arrRcPreCalc.ElementAt( nBar-nSubRowStartPos );
			int nCsAvail = bHorz
				? (rcPreCalc.Width() - olid.m_sizeBarMin.cx)
				: (rcPreCalc.Height() - olid.m_sizeBarMin.cy)
				;
			if( nCsAvail < 0 )
				nCsAvail = 0;
			//ASSERT( nCsAvail >= 0 );
			if( nCsAvail > nCsRow )
				nCsAvail = nCsRow;
			bool bAdjust = false;
			if( nCsRow > 0 )
			{
				nCsRow -= nCsAvail;
				ASSERT( nCsRow >= 0 );
				bAdjust = true;
			}
			int nShift =
				nLeftPrev
				- (bHorz ? rcPreCalc.right : rcPreCalc.bottom )
				;
			//ASSERT( nShift >= 0 );
			rcPreCalc.OffsetRect(
				bHorz ? nShift : 0,
				bHorz ? 0 : nShift
				);
			//if( nCsRow > 0 )
			if( bAdjust )
				(bHorz ? rcPreCalc.left : rcPreCalc.top) += nCsAvail;
			nLeftPrev = bHorz ? rcPreCalc.left : rcPreCalc.top;
		} // for( nBar = nSubRowEndPos; nBar >= nSubRowStartPos; nBar-- )
		if( nCsRow == 0 && nLeftPrev >= 0 )
		{
			bRetVal = true;
			if(		nLeftPrev == 0
				&&	(nSubRowStartPos < nSubRowEndPos)
				)
			{ // if compression passed with full row layout with more than 1 bar
				bCompressionPassed = true;
				// re-compress with affixment
				AffixmentOrderVector_t arrAffixmentOrder;
				_CalcAffixmentOrderVector(
					bHorz,
					arrAffixmentOrder,
					dbol,
					false,
					nSubRowStartPos,
					nSubRowEndPos
					);
				ASSERT( arrAffixmentOrder.GetSize() == (nSubRowEndPos-nSubRowStartPos+1) );
				bool bPerformReShift = false;
				// last is not reviewed
				for( int nOrderT = (int)arrAffixmentOrder.GetSize()-1; nOrderT > 0; nOrderT-- )
				{
					int nReviewIdx = arrAffixmentOrder[ nOrderT - 1 ];
					OuterLayoutItemData & olidReview = dbol.ElementAt( nReviewIdx );
					CExtControlBar * pExtBarReview =
						DYNAMIC_DOWNCAST(
							CExtControlBar,
							olidReview.m_pBar
							);
					if( pExtBarReview == NULL )
						continue;
					CExtControlBar::InternalAffixmentData * pAffixmentDataReview =
						pExtBarReview->_AffixmentGetOuter();
					if( pAffixmentDataReview == NULL
						||	pAffixmentDataReview->IsEmpty()
						)
						continue;
					CRect & rcPreCalcReview = arrRcPreCalc.ElementAt( nReviewIdx-nSubRowStartPos );
					int nExtentBarReview = bHorz
						? rcPreCalcReview.Width()
						: rcPreCalcReview.Height()
						;
					int nExtentCompressedAffixmentReview = bHorz
						? pAffixmentDataReview->m_sizeCompressed.cx
						: pAffixmentDataReview->m_sizeCompressed.cy
						;
					if( nExtentCompressedAffixmentReview < nExtentBarReview )
						continue;
					int nCsExt = nExtentCompressedAffixmentReview - nExtentBarReview;
					int nCsSrc = nCsExt;

					for( int nCmpxIdx = (int)arrAffixmentOrder.GetSize(); nCmpxIdx > nOrderT ; nCmpxIdx-- )
					{
						int nCmpxIdxEffective = arrAffixmentOrder[ nCmpxIdx - 1 ];
						OuterLayoutItemData & olidCmpx = dbol.ElementAt( nCmpxIdxEffective );
						CExtControlBar * pExtBarCmpx =
							DYNAMIC_DOWNCAST(
								CExtControlBar,
								olidCmpx.m_pBar
								);
						if( pExtBarCmpx == NULL )
							continue;
						CExtControlBar::InternalAffixmentData * pAffixmentDataCmpx =
							pExtBarCmpx->_AffixmentGetOuter();
						if( pAffixmentDataCmpx == NULL
							||	pAffixmentDataCmpx->IsEmpty()
							)
							continue;
						ASSERT( pAffixmentDataReview->m_nAffixmentWeight <= pAffixmentDataCmpx->m_nAffixmentWeight );
						CRect & rcPreCalcCmpx = arrRcPreCalc.ElementAt( nCmpxIdxEffective-nSubRowStartPos );
						int nCsAvail = bHorz
							? (rcPreCalcCmpx.Width() - olidCmpx.m_sizeBarMin.cx)
							: (rcPreCalcCmpx.Height() - olidCmpx.m_sizeBarMin.cy)
							;
						//ASSERT( nCsAvail >= 0 );
						if( nCsAvail < 0 )
							continue;
						nCsAvail = min( nCsAvail, nCsExt );
						ASSERT( nCsAvail >= 0 );
						if( nCsAvail == 0 )
							continue;
						(bHorz ? rcPreCalcCmpx.right : rcPreCalcCmpx.bottom ) -=
							nCsAvail;
						nCsExt -= nCsAvail;
						bPerformReShift = true;
						ASSERT( nCsExt >= 0 );
						if( nCsExt == 0 )
							break;
					} // for( int nCmpxIdx = arrAffixmentOrder.GetSize(); nCmpxIdx > nOrderT ; nCmpxIdx-- )

					ASSERT( 0 <= nCsExt && nCsExt <= nCsSrc );
					if( nCsExt == nCsSrc )
						continue;

					ASSERT( bPerformReShift );
					(bHorz ? rcPreCalcReview.right : rcPreCalcReview.bottom ) +=
						(nCsSrc - nCsExt);
				} // for( int nOrderT = arrAffixmentOrder.GetSize()-1; nOrderT > 0; nOrderT-- )
				if( bPerformReShift )
				{
					int nRightPrev = 0;
					for( int nBar = nSubRowStartPos; nBar <= nSubRowEndPos; nBar++ )
					{
//						OuterLayoutItemData & olid = dbol.ElementAt( nBar );
						CRect & rcPreCalc = arrRcPreCalc.ElementAt( nBar-nSubRowStartPos );
						int nShift =
							nRightPrev
							- (bHorz ? rcPreCalc.left : rcPreCalc.top )
							;
						if( nShift != 0 )
							rcPreCalc.OffsetRect(
								bHorz ? nShift : 0,
								bHorz ? 0 : nShift
								);
						nRightPrev = bHorz ? rcPreCalc.right : rcPreCalc.bottom;
					} // for( nBar = nSubRowStartPos; nBar <= nSubRowEndPos; nBar++ )
				} // if( bPerformReShift )
			} // if compression passed with full row layout with more than 1 bar
		} // if( nCsRow == 0 && nLeftPrev >= 0 )
	} // if( (!bRetVal) && nCsRow >= 0 )

	if( !bRetVal )
	{
		bAdjustWithAffixment = false;
		return false;
	} // if( !bRetVal )

bool bCalcFullyOccupiedLayout = 
		(p_bFullyOccupiedLayout != NULL) ? true : false;
	if(		bCalcFullyOccupiedLayout
		&&	(*p_bFullyOccupiedLayout)
		)
		bCalcFullyOccupiedLayout = false;
	if(		bCalcFullyOccupiedLayout
		&&	nSubRowStartPos > 0
		&&	nSubRowEndPos < (dbol.GetSize() - 1)
		)
		bCalcFullyOccupiedLayout = false;
	if(		bCalcFullyOccupiedLayout
		&&	bCompressionPassed
		)
	{
		bCalcFullyOccupiedLayout = false;
		*p_bFullyOccupiedLayout = true;
	}
int nPosRightPrev = 0;
	for( nBar = nSubRowStartPos; nBar <= nSubRowEndPos; nBar++ )
	{
		OuterLayoutItemData & olid = dbol.ElementAt( nBar );
		olid.m_rcPreCalc = arrRcPreCalc.ElementAt( nBar-nSubRowStartPos );
		ASSERT( olid.m_rcPreCalc.left >= 0 );
		ASSERT( olid.m_rcPreCalc.right >= 0 );
		ASSERT( olid.m_rcPreCalc.left <= olid.m_rcPreCalc.right );
		ASSERT( olid.m_rcPreCalc.top <= olid.m_rcPreCalc.bottom );
#ifdef __DEBUG_CExtDockOuterBar_CompressSubRow_ASSERT_CRITICAL_MINSIZE
		ASSERT( olid.m_rcPreCalc.Width() >= olid.m_sizeBarMin.cx );
		ASSERT( olid.m_rcPreCalc.Height() >= olid.m_sizeBarMin.cy );
#endif // __DEBUG_CExtDockOuterBar_CompressSubRow_ASSERT_CRITICAL_MINSIZE
		if( !bCalcFullyOccupiedLayout )
			continue;
		int nPosLeft = bHorz
			? olid.m_rcPreCalc.left
			: olid.m_rcPreCalc.top
			;
		if( nBar == nSubRowStartPos )
		{
			ASSERT( nPosLeft >= 0 );
			if( nPosLeft > 0 )
			{
				bCalcFullyOccupiedLayout = false;
				continue;
			}
		} // if( nBar == nSubRowStartPos )
		ASSERT( nPosLeft >= nPosRightPrev );
		if( nPosLeft > nPosRightPrev )
		{
			bCalcFullyOccupiedLayout = false;
			continue;
		}
		nPosRightPrev = bHorz
			? olid.m_rcPreCalc.right
			: olid.m_rcPreCalc.bottom
			;
		if( nBar == nSubRowEndPos )
		{
			if( nPosRightPrev < nExtentMax )
			{
				bCalcFullyOccupiedLayout = false;
				continue;
			}
		} // if( nBar == nSubRowEndPos )
	} // for( nBar = nSubRowStartPos; nBar <= nSubRowEndPos; nBar++ )

	if( bCalcFullyOccupiedLayout )
	{
		ASSERT( p_bFullyOccupiedLayout != NULL );
		*p_bFullyOccupiedLayout = true;
	}

	return true;
}

void CExtDockOuterBar::_CalcFixedRowLayout(
	CSize & sizeFixed,
	CSize & sizeMax,
	CPoint & pt,
	BOOL bHorz,
	AFX_SIZEPARENTPARAMS & layout,
	CExtDockOuterBar::dockbar_outer_layout_t & dbol,
	int * p_nSubRowCount, // = NULL
	bool * p_bFullyOccupiedLayout // = NULL
	)
{
	ASSERT_VALID(this);
	if( p_nSubRowCount != NULL )
		*p_nSubRowCount = 1;
	if( p_bFullyOccupiedLayout != NULL )
		*p_bFullyOccupiedLayout = false;
int nCountOfBars = (int)dbol.GetSize();
	ASSERT( nCountOfBars > 0 );

	// pre-process hidden bars
	int nBar = 0;
	for( nBar = 0; nBar < nCountOfBars; )
	{
		OuterLayoutItemData & olid = dbol.ElementAt( nBar );
		ASSERT_VALID( olid.m_pBar );
		ASSERT_KINDOF( CControlBar, olid.m_pBar );
		if( !olid.m_bVisible )
		{
			olid.m_pBar->RecalcDelayShow( &layout );
			dbol.RemoveAt( nBar );
			nCountOfBars--;
			continue;
		}
		olid.m_pBar->GetWindowRect( &olid.m_rcReal );
		ScreenToClient( &olid.m_rcReal );
		nBar++;
	} // for( nBar = 0; nBar < nCountOfBars; )

	// precalc layout (1), move according to affixement
AffixmentOrderVector_t arrAffixmentOrder;
	_CalcAffixmentOrderVector(
		bHorz ? true : false,
		arrAffixmentOrder,
		dbol,
		true
		);
	ASSERT( arrAffixmentOrder.GetSize() == nCountOfBars );

	// precalc layout (2), split to subrows
int nTotalRowExtent = 0;
int nSubRowStartPos = 0;
bool bAdjustWithAffixment = true;
bool bAlignSubRowAffixmentHW = true;
	for( nBar = 0; nBar < nCountOfBars; nBar++ )
	{
		OuterLayoutItemData & olid = dbol.ElementAt( nBar );
		ASSERT_VALID( olid.m_pBar );
		ASSERT_KINDOF( CControlBar, olid.m_pBar );
		ASSERT( olid.m_bVisible );

		olid.m_rcPreCalc = CRect( pt, olid.m_sizeCalcDynamic );
		if( bHorz )
		{
			if(		olid.m_rcReal.left > olid.m_rcPreCalc.left
				&&	(!m_bFloating)
				)
				olid.m_rcPreCalc.OffsetRect(
					olid.m_rcReal.left - olid.m_rcPreCalc.left,
					0
					);
			int nRest = olid.m_rcPreCalc.right - sizeMax.cx;
///			int nRest = sizeMax.cx - olid.m_rcPreCalc.left + olid.m_sizeBarMin.cx;
			if(		nRest >= 0
				&&	(	!olid.m_pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
					||	(	olid.m_pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
						&&	((CExtControlBar*)olid.m_pBar)->IsFixedMode()
						)
					)
				)
			{ // trying to make one multi-row or compress it
				if( !_CompressSubRow(
						dbol,
						nSubRowStartPos,
						nBar,
						sizeMax.cx,
						bHorz ? true : false,
						bAdjustWithAffixment,
						p_bFullyOccupiedLayout
						)
					)
				{
					if( p_nSubRowCount != NULL )
						(*p_nSubRowCount)++;
					if( p_bFullyOccupiedLayout != NULL )
						*p_bFullyOccupiedLayout = true;
//					ASSERT( bAlignSubRowAffixmentHW );
					bAlignSubRowAffixmentHW = false;
					_AlignSubRowAffixmentHW(
						bHorz ? true : false,
						dbol,
						nSubRowStartPos,
						nBar
						);
					bAdjustWithAffixment = false;
					nSubRowStartPos = nBar;
					_AdjustByTotalRowExtent(
						nTotalRowExtent,
						sizeFixed,
						pt,
						bHorz
						);
					ASSERT( pt.x == 0 );
					olid.m_rcPreCalc = CRect( pt, olid.m_sizeCalcDynamic );
				}
				else
					bAlignSubRowAffixmentHW = true;
			} // trying to make one multi-row or compress it
			pt.x =
				//olid.m_rcPreCalc.left + olid.m_sizeCalcDynamic.cx
				olid.m_rcPreCalc.right
				;
			nTotalRowExtent = max( nTotalRowExtent, olid.m_sizeCalcDynamic.cy );
		} // if( bHorz )
		else
		{
			// offset calculated rect out to actual
			if(		olid.m_rcReal.top > olid.m_rcPreCalc.top
				&&	(!m_bFloating)
				)
				olid.m_rcPreCalc.OffsetRect(
					0,
					olid.m_rcReal.top - olid.m_rcPreCalc.top
					);
			int nRest = olid.m_rcPreCalc.bottom - sizeMax.cy;
///			int nRest = sizeMax.cy - olid.m_rcPreCalc.top + olid.m_sizeBarMin.cy;
			if(		nRest >= 0
				&&	(	!olid.m_pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
					||	(	olid.m_pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar))
						&&	((CExtControlBar*)olid.m_pBar)->IsFixedMode()
						)
					)
				)
			{ // trying to make one multi-row or compress it
				if( !_CompressSubRow(
						dbol,
						nSubRowStartPos,
						nBar,
						sizeMax.cy,
						bHorz ? true : false,
						bAdjustWithAffixment,
						p_bFullyOccupiedLayout
						)
					)
				{
					if( p_nSubRowCount != NULL )
						(*p_nSubRowCount)++;
					if( p_bFullyOccupiedLayout != NULL )
						*p_bFullyOccupiedLayout = true;
//					ASSERT( bAlignSubRowAffixmentHW );
					bAlignSubRowAffixmentHW = false;
					_AlignSubRowAffixmentHW(
						bHorz ? true : false,
						dbol,
						nSubRowStartPos,
						nBar
						);
					bAdjustWithAffixment = false;
					nSubRowStartPos = nBar;
					_AdjustByTotalRowExtent(
						nTotalRowExtent,
						sizeFixed,
						pt,
						bHorz
						);
					ASSERT( pt.y == 0 );
					olid.m_rcPreCalc = CRect( pt, olid.m_sizeCalcDynamic );
				}
				else
					bAlignSubRowAffixmentHW = true;
			} // trying to make one multi-row or compress it
			pt.y =
				//olid.m_rcPreCalc.top + olid.m_sizeCalcDynamic.cy
				olid.m_rcPreCalc.bottom
				;
			nTotalRowExtent = max( nTotalRowExtent, olid.m_sizeCalcDynamic.cx );
		} // else from if( bHorz )

	} // for( nBar = 0; nBar < nCountOfBars; nBar++ )

	if( bAlignSubRowAffixmentHW && (nCountOfBars > 0) )
	{
		ASSERT( nBar <= nCountOfBars );
		if( nBar == nCountOfBars )
			nBar = nCountOfBars - 1;
		ASSERT( nSubRowStartPos <= nBar );
		_AlignSubRowAffixmentHW(
			bHorz ? true : false,
			dbol,
			nSubRowStartPos,
			nBar
			);
	} // if( bAlignSubRowAffixmentHW && (nCountOfBars > 0) )

/* DEBUG **
*********************************************
*********************************************
int nPosLastTest = 0;
	for( nBar = 0; bAdjustWithAffixment && nBar < nCountOfBars; nBar++ )
	{
		OuterLayoutItemData & olid = dbol.ElementAt( nBar );
		ASSERT_VALID( olid.m_pBar );
		ASSERT_KINDOF( CControlBar, olid.m_pBar );
		ASSERT( olid.m_bVisible );
		int nPosCurrTest = bHorz
			? olid.m_rcPreCalc.left
			: olid.m_rcPreCalc.top
			;
		ASSERT( nPosLastTest <= nPosCurrTest );
		if( nPosLastTest < nPosCurrTest )
			break;
		nPosLastTest = bHorz
			? olid.m_rcPreCalc.right
			: olid.m_rcPreCalc.bottom
			;
	} // for( nBar = 0; bAdjustWithAffixment && nBar < nCountOfBars; nBar++ )
int bMaxTest = bHorz ? sizeMax.cx : sizeMax.cy;
	if( nBar == nCountOfBars
		&& nPosLastTest >= bMaxTest
		)
		bAdjustWithAffixment = false;
*********************************************
*********************************************
** DEBUG */

	if( bAdjustWithAffixment )
	{
		// pre adjust affixment
		int nOrder = 0;
		for( nOrder = nCountOfBars; nOrder > 0; nOrder-- )
		{
			int nBarAtOrder = arrAffixmentOrder[nOrder-1];
			ASSERT( 0 <= nBarAtOrder && nBarAtOrder < nCountOfBars );
			OuterLayoutItemData & olid = dbol.ElementAt( nBarAtOrder );
			ASSERT_VALID( olid.m_pBar );
			ASSERT_KINDOF( CControlBar, olid.m_pBar );
			ASSERT( olid.m_bVisible );
			CExtControlBar * pExtBar =
				DYNAMIC_DOWNCAST(
					CExtControlBar,
					olid.m_pBar
					);
			if( pExtBar == NULL )
				continue;
			CExtControlBar::InternalAffixmentData * pAffixmentData =
					pExtBar->_AffixmentGetOuter();
//			if( pAffixmentData == NULL )
//				continue;
//			ASSERT( !pAffixmentData->IsEmpty() );
//			ASSERT( !pAffixmentData->m_rcAffixment.IsRectEmpty() );
//			if( olid.m_rcPreCalc == pAffixmentData->m_rcAffixment )
//				continue;
			if(		pAffixmentData == NULL
				||	pAffixmentData->IsEmpty()
				||	pAffixmentData->m_rcAffixment.IsRectEmpty()
				||	olid.m_rcPreCalc == pAffixmentData->m_rcAffixment
				)
				continue;
			_PreAdjustWithAffixment(
				bHorz ? true : false,
				dbol,
				olid,
				nBarAtOrder,
				pAffixmentData,
				bHorz ? sizeMax.cx : sizeMax.cy
				);
		} // for( nOrder = nCountOfBars; nOrder > 0; nOrder-- )
		// post adjust affixment
		for( nOrder = nCountOfBars; nOrder > 0; nOrder-- )
		{
			int nBarAtOrder = arrAffixmentOrder[nOrder-1];
			ASSERT( 0 <= nBarAtOrder && nBarAtOrder < nCountOfBars );
			OuterLayoutItemData & olid = dbol.ElementAt( nBarAtOrder );
			ASSERT_VALID( olid.m_pBar );
			ASSERT_KINDOF( CControlBar, olid.m_pBar );
			ASSERT( olid.m_bVisible );
			CExtControlBar * pExtBar =
				DYNAMIC_DOWNCAST(
					CExtControlBar,
					olid.m_pBar
					);
			if( pExtBar == NULL )
				continue;
			CExtControlBar::InternalAffixmentData * pAffixmentData =
					pExtBar->_AffixmentGetOuter();
//			if( pAffixmentData == NULL )
//				continue;
//			ASSERT( !pAffixmentData->IsEmpty() );
//			ASSERT( !pAffixmentData->m_rcAffixment.IsRectEmpty() );
//			if( olid.m_rcPreCalc == pAffixmentData->m_rcAffixment )
//				continue;
			if(		pAffixmentData == NULL
				||	pAffixmentData->IsEmpty()
				||	pAffixmentData->m_rcAffixment.IsRectEmpty()
				||	olid.m_rcPreCalc == pAffixmentData->m_rcAffixment
				)
				continue;
			_PostAdjustWithAffixment(
				bHorz ? true : false,
				dbol,
				olid,
				nBarAtOrder,
				pAffixmentData,
				bHorz ? sizeMax.cx : sizeMax.cy
				);
		} // for( int nOrder = nCountOfBars; nOrder > 0; nOrder-- )
	} // if( bAdjustWithAffixment )

	// verify for minimal layouts
	for( nBar = 0; nBar < nCountOfBars; nBar++ )
	{
		OuterLayoutItemData & olid = dbol.ElementAt( nBar );
		ASSERT_VALID( olid.m_pBar );
		ASSERT_KINDOF( CControlBar, olid.m_pBar );
		ASSERT( olid.m_bVisible );
		if( bHorz )
		{
			int nRest = sizeMax.cx - olid.m_rcPreCalc.right;
			if( nRest >= 0 )
				continue;
			olid.m_rcPreCalc.right = sizeMax.cx;
			if( olid.m_rcPreCalc.Width() < olid.m_sizeBarMin.cx )
				olid.m_rcPreCalc.right = olid.m_rcPreCalc.left + olid.m_sizeBarMin.cx;
		} // if( bHorz )
		else
		{
			int nRest = sizeMax.cy - olid.m_rcPreCalc.bottom;
			if( nRest >= 0 )
				continue;
			olid.m_rcPreCalc.bottom = sizeMax.cy;
			if( olid.m_rcPreCalc.Height() < olid.m_sizeBarMin.cy )
				olid.m_rcPreCalc.bottom = olid.m_rcPreCalc.top + olid.m_sizeBarMin.cy;
		} // else from if( bHorz )
	}

	// apply layout
	if( !m_bLayoutQuery )
	{
		for( nBar = 0; nBar < nCountOfBars; nBar++ )
		{
			OuterLayoutItemData & olid = dbol.ElementAt( nBar );
			ASSERT_VALID( olid.m_pBar );
			ASSERT_KINDOF( CControlBar, olid.m_pBar );
			ASSERT( olid.m_bVisible );
			if( olid.m_rcPreCalc == olid.m_rcReal )
				continue;
			if( (olid.m_pBar->m_dwStyle & CBRS_FLOATING) == 0 )
				olid.m_pBar->m_pDockContext->
					m_rectMRUDockPos = olid.m_rcPreCalc;
			AfxRepositionWindow(
				&layout,
				olid.m_pBar->m_hWnd,
				&olid.m_rcPreCalc
				);
		} // for( nBar = 0; nBar < nCountOfBars; nBar++ )
	} // if( !m_bLayoutQuery )

	_AdjustByTotalRowExtent(
		nTotalRowExtent,
		sizeFixed,
		pt,
		bHorz
		);
}

void CExtDockOuterBar::_PreAdjustWithAffixment(
	bool bHorz,
	CExtDockOuterBar::dockbar_outer_layout_t & dbol,
	CExtDockOuterBar::OuterLayoutItemData & olidAdjustedSrc,
	int nBarAdjustedSrc,
	CExtControlBar::InternalAffixmentData * pAffixmentDataAdjustedSrc,
	int nExtentMax
	)
{
	if( nExtentMax <= 0 )
		return;
int nCountOfBars = (int)dbol.GetSize();
	ASSERT( 0 <= nBarAdjustedSrc && nBarAdjustedSrc < nCountOfBars );
	ASSERT( dbol.ElementAt(nBarAdjustedSrc).m_pBar == olidAdjustedSrc.m_pBar );
	ASSERT( pAffixmentDataAdjustedSrc != NULL );
	ASSERT( !pAffixmentDataAdjustedSrc->IsEmpty() );
int nPosAffixment = bHorz
		? pAffixmentDataAdjustedSrc->m_rcAffixment.left
		: pAffixmentDataAdjustedSrc->m_rcAffixment.top
		;
int nPosPreCalc = bHorz
		? olidAdjustedSrc.m_rcPreCalc.left
		: olidAdjustedSrc.m_rcPreCalc.top
		;
int nAdjustShift = nPosAffixment - nPosPreCalc;
	if( nAdjustShift == 0 )
		return;
//	TRACE3(
//		"     Bar 0x%08X      weight = %d      nAdjustShift = %d\n",
//		long(olidAdjustedSrc.m_pBar),
//		pAffixmentDataAdjustedSrc->m_nAffixmentWeight,
//		nAdjustShift
//		);
	_ShiftSubRowWithAffixment(
		bHorz,
		dbol,
		( nAdjustShift > 0 )
			? nBarAdjustedSrc
			: 0
			,
		( nAdjustShift > 0 )
			? (nCountOfBars - 1)
			: nBarAdjustedSrc
			,
		nAdjustShift,
		nExtentMax
		);
}

void CExtDockOuterBar::_PostAdjustWithAffixment(
	bool bHorz,
	CExtDockOuterBar::dockbar_outer_layout_t & dbol,
	CExtDockOuterBar::OuterLayoutItemData & olidAdjustedSrc,
	int nBarAdjustedSrc,
	CExtControlBar::InternalAffixmentData * pAffixmentDataAdjustedSrc,
	int nExtentMax
	)
{
	if( nExtentMax <= 0 )
		return;
int nCountOfBars = (int)dbol.GetSize();
	ASSERT( 0 <= nBarAdjustedSrc && nBarAdjustedSrc < nCountOfBars );
	ASSERT( dbol.ElementAt(nBarAdjustedSrc).m_pBar == olidAdjustedSrc.m_pBar );
	ASSERT( pAffixmentDataAdjustedSrc != NULL );
	ASSERT( !pAffixmentDataAdjustedSrc->IsEmpty() );

int nPosAffixment = bHorz
		? pAffixmentDataAdjustedSrc->m_rcAffixment.left
		: pAffixmentDataAdjustedSrc->m_rcAffixment.top
		;
int nPosPreCalc = bHorz
		? olidAdjustedSrc.m_rcPreCalc.left
		: olidAdjustedSrc.m_rcPreCalc.top
		;
int nPosPossibleToFix = nPosPreCalc;
int nAdjustShift = nPosAffixment - nPosPreCalc;
	if( nAdjustShift == 0 )
		return;
	if( nAdjustShift < 0 )
	{
		if( nBarAdjustedSrc == 0 )
		{
			nPosPossibleToFix =
				max( nPosPossibleToFix, 0 );
		} // if( nBarAdjustedSrc == 0 )
		else
		{
			OuterLayoutItemData & olid2 = dbol.ElementAt( nBarAdjustedSrc-1 );
			ASSERT_VALID( olid2.m_pBar );
			ASSERT_KINDOF( CControlBar, olid2.m_pBar );
			ASSERT( olid2.m_bVisible );
			nPosPossibleToFix = bHorz
				? olid2.m_rcPreCalc.right
				: olid2.m_rcPreCalc.bottom
				;
		} // else from if( nBarAdjustedSrc == 0 )
		int nPossibleShift = nPosPossibleToFix - nPosPreCalc;
		nAdjustShift = max( nAdjustShift, nPossibleShift );
	} // if( nAdjustShift < 0 )
	else
	{
		nPosPreCalc += bHorz
			? olidAdjustedSrc.m_rcPreCalc.Width()
			: olidAdjustedSrc.m_rcPreCalc.Height()
			;
		nPosPossibleToFix = nPosPreCalc;
		if( nBarAdjustedSrc == (nCountOfBars-1) )
		{
			nPosPossibleToFix =
				min( nPosPossibleToFix, nExtentMax );
		} // if( nBarAdjustedSrc == (nCountOfBars-1) )
		else
		{
			OuterLayoutItemData & olid2 = dbol.ElementAt( nBarAdjustedSrc+1 );
			ASSERT_VALID( olid2.m_pBar );
			ASSERT_KINDOF( CControlBar, olid2.m_pBar );
			ASSERT( olid2.m_bVisible );
			nPosPossibleToFix = bHorz
				? olid2.m_rcPreCalc.left
				: olid2.m_rcPreCalc.top
				;
		} // else from if( nBarAdjustedSrc == (nCountOfBars-1) )
		int nPossibleShift = nPosPossibleToFix - nPosPreCalc;
		nAdjustShift = min( nAdjustShift, nPossibleShift );
	} // else from if( nAdjustShift < 0 )
	if( nAdjustShift == 0 )
		return;
	olidAdjustedSrc.m_rcPreCalc.OffsetRect(
		bHorz ? nAdjustShift : 0,
		bHorz ? 0 : nAdjustShift
		);
}

int CExtDockOuterBar::_ShiftSubRowWithAffixment(
	bool bHorz,
	CExtDockOuterBar::dockbar_outer_layout_t & dbol,
	int nReviewStart,
	int nReviewEnd,
	int nAdjustShift,
	int nExtentMax
	)
{
	ASSERT( dbol.GetSize() > 0 );
	ASSERT( 0 <= nReviewStart && nReviewStart < dbol.GetSize() );
	ASSERT( 0 <= nReviewEnd && nReviewEnd < dbol.GetSize() );
	ASSERT( nReviewStart <= nReviewEnd );
	ASSERT( nAdjustShift != 0 );
	ASSERT( nExtentMax > 0 );

	if( nReviewStart == nReviewEnd )
	{
		OuterLayoutItemData & olid = dbol.ElementAt( nReviewStart );
		ASSERT_VALID( olid.m_pBar );
		ASSERT_KINDOF( CControlBar, olid.m_pBar );
		ASSERT( olid.m_bVisible );
		int nPosPreCalc = bHorz
			? olid.m_rcPreCalc.left
			: olid.m_rcPreCalc.top
			;
		ASSERT( nPosPreCalc >= 0 );
		if( nAdjustShift > 0 )
		{
			ASSERT( nReviewStart == dbol.GetSize()-1 );
			nPosPreCalc += bHorz
				? olid.m_rcPreCalc.Width()
				: olid.m_rcPreCalc.Height()
				;
			ASSERT( nPosPreCalc >= 0 );
			ASSERT( nPosPreCalc <= nExtentMax );
			if( nPosPreCalc == nExtentMax )
				return 0;
			int nRest = nExtentMax - nPosPreCalc;
			ASSERT( nRest >= 0 );
			nAdjustShift = min( nAdjustShift, nRest );
			ASSERT( nAdjustShift >= 0 );
		} // if( nAdjustShift > 0 )
		else
		{
			ASSERT( nReviewStart == 0 );
//			if( nPosPreCalc == 0 )
//				return 0;
//if( nPosPreCalc < 0 )
//	return 0;
			nAdjustShift = max( (-nPosPreCalc), nAdjustShift );
			ASSERT( nAdjustShift <= 0 );
		} // else from if( nAdjustShift > 0 )
		olid.m_rcPreCalc.OffsetRect(
			bHorz ? nAdjustShift : 0,
			bHorz ? 0 : nAdjustShift
			);
		ASSERT( olid.m_rcPreCalc.left >= 0 );
		ASSERT( olid.m_rcPreCalc.top >= 0 );
		return nAdjustShift;
	} // if( nReviewStart == nReviewEnd )

	if( nAdjustShift > 0 )
	{
		OuterLayoutItemData & olid1 = dbol.ElementAt( nReviewStart );
		ASSERT_VALID( olid1.m_pBar );
		ASSERT_KINDOF( CControlBar, olid1.m_pBar );
		ASSERT( olid1.m_bVisible );
		ASSERT( olid1.m_rcPreCalc.left >= 0 );
		ASSERT( olid1.m_rcPreCalc.top >= 0 );
		int nPosPreCalc1 = bHorz
			? olid1.m_rcPreCalc.right // (olid1.m_rcPreCalc.left + olid1.m_rcPreCalc.Width())
			: olid1.m_rcPreCalc.bottom // (olid1.m_rcPreCalc.top + olid1.m_rcPreCalc.Height())
			;
		ASSERT( nPosPreCalc1 >= 0 );
		OuterLayoutItemData & olid2 = dbol.ElementAt( nReviewStart + 1 );
		ASSERT_VALID( olid2.m_pBar );
		ASSERT_KINDOF( CControlBar, olid2.m_pBar );
		ASSERT( olid2.m_bVisible );
		ASSERT( olid2.m_rcPreCalc.left >= 0 );
		ASSERT( olid2.m_rcPreCalc.top >= 0 );
		int nPosPreCalc2 = bHorz
			? olid2.m_rcPreCalc.left
			: olid2.m_rcPreCalc.top
			;
		ASSERT( nPosPreCalc2 >= 0 );
		int nDist = nPosPreCalc2 - nPosPreCalc1;
		ASSERT( nDist >= 0 );
		nDist = min( nAdjustShift, nDist );
		ASSERT( nDist <= nAdjustShift );
		if( nDist == nAdjustShift )
		{
			olid1.m_rcPreCalc.OffsetRect(
				bHorz ? nAdjustShift : 0,
				bHorz ? 0 : nAdjustShift
				);
			ASSERT( olid1.m_rcPreCalc.left >= 0 );
			ASSERT( olid1.m_rcPreCalc.top >= 0 );
			return nAdjustShift;
		}
		int nSpaceToCompress = nAdjustShift - nDist;
		ASSERT( nSpaceToCompress > 0 );
		int nSpaceReallyCompressed =
			_ShiftSubRowWithAffixment(
				bHorz,
				dbol,
				nReviewStart + 1,
				nReviewEnd,
				nSpaceToCompress,
				nExtentMax
				);
		ASSERT( nSpaceReallyCompressed >= 0 );
		ASSERT( nSpaceReallyCompressed <= nSpaceToCompress );
/*?????*/		nAdjustShift = nSpaceReallyCompressed + nDist;
		olid1.m_rcPreCalc.OffsetRect(
			bHorz ? nAdjustShift : 0,
			bHorz ? 0 : nAdjustShift
			);
		ASSERT( olid1.m_rcPreCalc.left >= 0 );
		ASSERT( olid1.m_rcPreCalc.top >= 0 );
	} // if( nAdjustShift > 0 )
	else
	{
		OuterLayoutItemData & olid1 = dbol.ElementAt( nReviewEnd );
		ASSERT_VALID( olid1.m_pBar );
		ASSERT_KINDOF( CControlBar, olid1.m_pBar );
		ASSERT( olid1.m_bVisible );
		ASSERT( olid1.m_rcPreCalc.left >= 0 );
		ASSERT( olid1.m_rcPreCalc.top >= 0 );
		int nPosPreCalc1 = bHorz
			? olid1.m_rcPreCalc.left
			: olid1.m_rcPreCalc.top
			;
		ASSERT( nPosPreCalc1 >= 0 );
		OuterLayoutItemData & olid2 = dbol.ElementAt( nReviewEnd - 1 );
		ASSERT_VALID( olid2.m_pBar );
		ASSERT_KINDOF( CControlBar, olid2.m_pBar );
		ASSERT( olid2.m_bVisible );
		ASSERT( olid2.m_rcPreCalc.left >= 0 );
		ASSERT( olid2.m_rcPreCalc.top >= 0 );
		int nPosPreCalc2 = bHorz
			? olid2.m_rcPreCalc.right // (olid2.m_rcPreCalc.left + olid2.m_rcPreCalc.Width())
			: olid2.m_rcPreCalc.bottom // (olid2.m_rcPreCalc.top + olid2.m_rcPreCalc.Height())
			;
		ASSERT( nPosPreCalc2 >= 0 );
		int nDist = nPosPreCalc2 - nPosPreCalc1;
		ASSERT( nDist <= 0 );
		nDist = max( nAdjustShift, nDist );
		ASSERT( nDist >= nAdjustShift );
		if( nDist == nAdjustShift )
		{
			olid1.m_rcPreCalc.OffsetRect(
				bHorz ? nAdjustShift : 0,
				bHorz ? 0 : nAdjustShift
				);
			ASSERT( olid1.m_rcPreCalc.left >= 0 );
			ASSERT( olid1.m_rcPreCalc.top >= 0 );
			return nAdjustShift;
		}
		int nSpaceToCompress = nAdjustShift - nDist;
		ASSERT( nSpaceToCompress < 0 );
		int nSpaceReallyCompressed =
			_ShiftSubRowWithAffixment(
				bHorz,
				dbol,
				nReviewStart,
				nReviewEnd - 1,
				nSpaceToCompress,
				nExtentMax
				);
		ASSERT( nSpaceReallyCompressed <= 0 );
		ASSERT( nSpaceReallyCompressed >= nSpaceToCompress );
/*?????*/		nAdjustShift = nSpaceReallyCompressed + nDist;
		olid1.m_rcPreCalc.OffsetRect(
			bHorz ? nAdjustShift : 0,
			bHorz ? 0 : nAdjustShift
			);
		ASSERT( olid2.m_rcPreCalc.left >= 0 );
		ASSERT( olid2.m_rcPreCalc.top >= 0 );
	} // else from if( nAdjustShift > 0 )

	return nAdjustShift;
}

void CExtDockOuterBar::_AdjustByTotalRowExtent(
	int & nTotalRowExtent,
	CSize & sizeFixed,
	CPoint & pt,
	BOOL bHorz
	)
{
	if( nTotalRowExtent == 0 )
		return;
	if( bHorz )
	{
		pt.y += nTotalRowExtent;
		sizeFixed.cx = max(sizeFixed.cx, pt.x);
		sizeFixed.cy = max(sizeFixed.cy, pt.y);
		pt.x = 0;
		sizeFixed.cy--;
	} // if( bHorz )
	else
	{
		pt.x += nTotalRowExtent;
		sizeFixed.cx = max(sizeFixed.cx, pt.x);
		sizeFixed.cy = max(sizeFixed.cy, pt.y);
		pt.y = 0;
		sizeFixed.cx--;
	} // else from if( bHorz )
	nTotalRowExtent = 0;
}

CSize CExtDockOuterBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	ASSERT_VALID(this);
CSize sizeFixed =
		CControlBar::CalcFixedLayout( bStretch, bHorz );
CSize sizeMax;
	if( !m_rectLayout.IsRectEmpty() )
		sizeMax = m_rectLayout.Size();
	else
	{
		CFrameWnd * pFrame = GetParentFrame();
		CRect rcFrameWindow;
		pFrame->GetClientRect( &rcFrameWindow );
		sizeMax = rcFrameWindow.Size();
	}

CPoint pt( 0, 0 );
int nCountOfBars = (int)m_arrBars.GetSize();
AFX_SIZEPARENTPARAMS layout;
	layout.hDWP =
		m_bLayoutQuery
			? NULL
			: ( ::BeginDeferWindowPos(nCountOfBars) )
			;
dockbar_outer_layout_t dbol;
	for( int nBar = 0; nBar < nCountOfBars; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)m_arrBars[nBar];
		if(		pBar != NULL
			&&	(!(__PLACEHODLER_BAR_PTR(pBar)))
			)
		{
			ASSERT_VALID( pBar );
			ASSERT_KINDOF( CControlBar, pBar );
			dbol.Add(
				OuterLayoutItemData(
					pBar,
					bHorz,
					sizeMax
					)
				);
		} // if( pBar != NULL && ...
		if( pBar == NULL || nBar == (nCountOfBars-1) )
		{
			if( dbol.GetSize() == 0 )
				continue;
			_CalcFixedRowLayout(
				sizeFixed,
				sizeMax,
				pt,
				bHorz,
				layout,
				dbol
				);
			dbol.RemoveAll();
		} // if( pBar == NULL || nBar == (nCountOfBars-1) )
	} // for( int nBar = 0; nBar < nCountOfBars; nBar++ )
	ASSERT( dbol.GetSize() == 0 );

	if( !m_bLayoutQuery )
	{
		ASSERT( layout.hDWP != NULL );
		if( layout.hDWP != NULL )
		{
			VERIFY( ::EndDeferWindowPos(layout.hDWP) );
		}
	} // if( !m_bLayoutQuery )

	// adjust size for borders on the dock bar itself
CRect rc( 0, 0, 0, 0 );
	CalcInsideRect(rc, bHorz);
	if( (!bStretch || !bHorz) && sizeFixed.cx != 0 )
		sizeFixed.cx +=
			-rc.right + rc.left
			+ g_bControlBarFixSizePixel ? 1 : 0;
	if(	(!bStretch || bHorz) && sizeFixed.cy != 0 )
		sizeFixed.cy +=
			-rc.bottom + rc.top
			+ g_bControlBarFixSizePixel ? 1 : 0;

	if( ! m_bLayoutQuery )
		OnDynamicLayoutUpdate();
	return sizeFixed;
}

void CExtDockOuterBar::_RedockInRow(
	CExtControlBar * pBarSlide,
	int nShiftDesired,
	bool * p_bInplaceResizing // = NULL
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBarSlide );
	ASSERT( this == pBarSlide->GetParent() );
	if( p_bInplaceResizing != NULL )
		*p_bInplaceResizing = false;
	if( nShiftDesired == 0 )
		return;
int nCountOfBars = (int)m_arrBars.GetSize();
	ASSERT( nCountOfBars > 1 );
int nSlideBarPos = FindBar( pBarSlide );
	ASSERT( 0 < nSlideBarPos && nSlideBarPos < nCountOfBars );
CControlBar * pFirstBar =
		pBarSlide->_GetFirstControlBarInRow(
			pBarSlide,
			false,
			false,
			false
			);
	ASSERT_VALID( pFirstBar );
int nFirstBarPos = FindBar( pFirstBar );
	ASSERT( 0 < nFirstBarPos && nFirstBarPos < nCountOfBars );
	ASSERT( nFirstBarPos <= nSlideBarPos );

UINT nDockBarID = GetDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );
BOOL bHorz = 
		(	nDockBarID == AFX_IDW_DOCKBAR_TOP
		||	nDockBarID == AFX_IDW_DOCKBAR_BOTTOM )
		? TRUE : FALSE;

CRect rcSlideBarWnd;
	pBarSlide->GetWindowRect( &rcSlideBarWnd );
	ScreenToClient( &rcSlideBarWnd );

CSize sizeMax, sizeFakeFixed( 0, 0 );
	if( !m_rectLayout.IsRectEmpty() )
		sizeMax = m_rectLayout.Size();
	else
	{
		CFrameWnd * pFrame = GetParentFrame();
		CRect rcFrameWindow;
		pFrame->GetClientRect( &rcFrameWindow );
		sizeMax = rcFrameWindow.Size();
	}

CPoint pt( 0, 0 );
	if( bHorz )
		pt.y = rcSlideBarWnd.top;
	else
		pt.x = rcSlideBarWnd.left;

CExtControlBar::InternalAffixmentData * pAffixmentData =
		pBarSlide->_AffixmentGetOuter();
	if( pAffixmentData != NULL )
	{
		ASSERT( !pAffixmentData->IsEmpty() );
		if( pAffixmentData->m_rcAffixment.IsRectEmpty() )
		{
			pBarSlide->GetWindowRect( &pAffixmentData->m_rcAffixment );
			ScreenToClient( &pAffixmentData->m_rcAffixment );
		}
		pAffixmentData->m_rcAffixment.OffsetRect(
			bHorz ? nShiftDesired : 0,
			bHorz ? 0 : nShiftDesired
			);
		if( bHorz )
		{
			if( pAffixmentData->m_rcAffixment.right >= sizeMax.cx )
				pAffixmentData->m_rcAffixment.OffsetRect(
					sizeMax.cx - pAffixmentData->m_rcAffixment.right,
					0
					);
			if( pAffixmentData->m_rcAffixment.left < 0 )
				pAffixmentData->m_rcAffixment.OffsetRect(
					- pAffixmentData->m_rcAffixment.left,
					0
					);
		} // if( bHorz )
		else
		{
			if( pAffixmentData->m_rcAffixment.bottom >= sizeMax.cy )
				pAffixmentData->m_rcAffixment.OffsetRect(
					0,
					sizeMax.cy - pAffixmentData->m_rcAffixment.bottom
					);
			if( pAffixmentData->m_rcAffixment.top < 0 )
				pAffixmentData->m_rcAffixment.OffsetRect(
					0,
					- pAffixmentData->m_rcAffixment.top
					);
		} // else from if( bHorz )
	}

dockbar_outer_layout_t dbol;
int nBar = 0;
	for( nBar = nFirstBarPos; nBar < nCountOfBars; nBar++ )
	{
		CControlBar * pBar = (CControlBar *)m_arrBars[nBar];
		if(		pBar != NULL
			&&	(!(__PLACEHODLER_BAR_PTR(pBar)))
			)
		{
			ASSERT_VALID( pBar );
			ASSERT_KINDOF( CControlBar, pBar );
			OuterLayoutItemData olid(
				pBar,
				bHorz,
				sizeMax
				);
			if( pBar == pBarSlide )
			{
				if( pAffixmentData == NULL )
					olid.m_rcPreCalc.OffsetRect(
						bHorz ? nShiftDesired : 0,
						bHorz ? 0 : nShiftDesired
						);
			} // if( pBar == pBarSlide )
			dbol.Add( olid );
		} // if( pBar != NULL && ...
		if( pBar == NULL || nBar == (nCountOfBars-1) )
			break;
	} // for( nBar = nFirstBarPos; nBar < nCountOfBars; nBar++ )

	ASSERT( dbol.GetSize() != 0 );

int nSubRowCount = 0;
bool bFullyOccupiedLayout = false;
BOOL bLayoutQueryOld = m_bLayoutQuery;
CSize sizeFakeFixedOld( sizeFakeFixed ), sizeMaxOld( sizeMax );
CPoint ptOld( pt );
AFX_SIZEPARENTPARAMS layout;
	layout.hDWP = NULL;
	_CalcFixedRowLayout(
		sizeFakeFixed,
		sizeMax,
		pt,
		bHorz,
		layout,
		dbol,
		&nSubRowCount,
		&bFullyOccupiedLayout
		);
	m_bLayoutQuery = bLayoutQueryOld;
	ASSERT( nSubRowCount >= 1 );
	if( m_bLayoutQuery )
		return;

	ASSERT( (nCountOfBars - nFirstBarPos) > 0 );

	sizeFakeFixed = sizeFakeFixedOld;
	sizeMax = sizeMaxOld;
	pt = ptOld;
	nCountOfBars = (int)dbol.GetSize();
	ASSERT( nCountOfBars > 0 );
	layout.hDWP =
		::BeginDeferWindowPos(nCountOfBars);

// reasonable row shift offset
const int nRsOffset = 1; //2;

	if( nCountOfBars > 1 && pAffixmentData != NULL )
	{
		int nSlideBarIdxInArray = -1;
		for( int nBar = 0; nBar < nCountOfBars; nBar++ )
		{
			OuterLayoutItemData & olid = dbol.ElementAt( nBar );
			ASSERT_VALID( olid.m_pBar );
			ASSERT_KINDOF( CControlBar, olid.m_pBar );
			ASSERT( olid.m_bVisible );
			if( olid.m_pBar == pBarSlide )
			{
				ASSERT( nSlideBarIdxInArray < 0 );
				nSlideBarIdxInArray = nBar;
				break;
			}
		} // for( nBar = 0; nBar < nCountOfBars; nBar++ )
		ASSERT( 0 <= nSlideBarIdxInArray && nSlideBarIdxInArray < nCountOfBars );
		OuterLayoutItemData & olid =
			dbol.ElementAt( nSlideBarIdxInArray );
		ASSERT_VALID( olid.m_pBar );
		ASSERT_KINDOF( CControlBar, olid.m_pBar );
		ASSERT( olid.m_bVisible );

		bool bTrySwapping = bFullyOccupiedLayout; // true; (+ v.2.22)
		bool bUpAffxOnSwapping = true;
		if(		bFullyOccupiedLayout
			&&	(nSubRowCount == 1)
			)
		{ // if can analyze resizing in row
			ASSERT( pAffixmentData != NULL );
			if( nSlideBarIdxInArray > 0 )
			{ // do resizing pre-affixment, do not resize first in row
				CSize sizeBarCalc = olid.m_rcPreCalc.Size();
				if( nShiftDesired > 0 )
				{
					//ASSERT( (!bHorz) || sizeBarCalc.cx >= olid.m_sizeBarMin.cx );
					//ASSERT( ( bHorz) || sizeBarCalc.cy >= olid.m_sizeBarMin.cy );
					int nExtentPossibleByBar = bHorz
						? (sizeBarCalc.cx - olid.m_sizeBarMin.cx)
						: (sizeBarCalc.cy - olid.m_sizeBarMin.cy)
						;
					nExtentPossibleByBar =
						min( nExtentPossibleByBar, nShiftDesired );
					if( nExtentPossibleByBar > 0 )
					{
						bTrySwapping = false;
						if( pAffixmentData->m_nAffixmentWeight != 1 )
							_AffixmentBringToTop( pBarSlide );
						if( bHorz )
						{
							int nNewAffixment = sizeBarCalc.cx - nExtentPossibleByBar;
							if( nNewAffixment > olid.m_sizeCalcDynamic.cx )
								nNewAffixment = olid.m_sizeCalcDynamic.cx;
							if( pAffixmentData->m_sizeCompressed.cx != nNewAffixment )
								pAffixmentData->m_sizeCompressed.cx = nNewAffixment;
							else
								bTrySwapping = true;
						} // if( bHorz )
						else
						{
							int nNewAffixment = sizeBarCalc.cy - nExtentPossibleByBar;
							if( nNewAffixment > olid.m_sizeCalcDynamic.cy )
								nNewAffixment = olid.m_sizeCalcDynamic.cy;
							if( pAffixmentData->m_sizeCompressed.cy != nNewAffixment )
								pAffixmentData->m_sizeCompressed.cy = nNewAffixment;
							else
								bTrySwapping = true;
						} // else from if( bHorz )
					} // if( nExtentPossibleByBar > 0 )
				} // if( nShiftDesired > 0 )
				else
				{
					//ASSERT( (!bHorz) || sizeBarCalc.cx >= olid.m_sizeBarMin.cx );
					//ASSERT( ( bHorz) || sizeBarCalc.cy >= olid.m_sizeBarMin.cy );
					int nExtentPossibleByBar = bHorz
						? (olid.m_sizeCalcDynamic.cx - sizeBarCalc.cx)
						: (olid.m_sizeCalcDynamic.cy - sizeBarCalc.cy)
						;
					nExtentPossibleByBar =
						min( nExtentPossibleByBar, (-nShiftDesired) );
					ASSERT( nExtentPossibleByBar >= 0 );
					if( nExtentPossibleByBar > 0 )
					{
						bTrySwapping = false;
						if( pAffixmentData->m_nAffixmentWeight != 1 )
							_AffixmentBringToTop( pBarSlide );
						if( bHorz )
						{
							int nNewAffixment = sizeBarCalc.cx + nExtentPossibleByBar;
							if( nNewAffixment > olid.m_sizeCalcDynamic.cx )
								nNewAffixment = olid.m_sizeCalcDynamic.cx;
							if( pAffixmentData->m_sizeCompressed.cx != nNewAffixment )
								pAffixmentData->m_sizeCompressed.cx = nNewAffixment;
							else
								bTrySwapping = true;
						}
						else
						{
							int nNewAffixment = sizeBarCalc.cy + nExtentPossibleByBar;
							if( nNewAffixment > olid.m_sizeCalcDynamic.cy )
								nNewAffixment = olid.m_sizeCalcDynamic.cy;
							if( pAffixmentData->m_sizeCompressed.cy != nNewAffixment )
								pAffixmentData->m_sizeCompressed.cy = nNewAffixment;
							else
								bTrySwapping = true;
						}
					} // if( nExtentPossibleByBar > 0 )
				} // else from if( nShiftDesired > 0 )
			} // do resizing pre-affixment, do not resize first in row
			else
				bUpAffxOnSwapping = false;
		}  // if can analyze resizing in row
		else if( (!bTrySwapping) && (nSubRowCount == 1) )
		{ // analyze left/right free space between bars is available (+ v.2.22)
			ASSERT( !bFullyOccupiedLayout );
			ASSERT( 0 <= nSlideBarIdxInArray && nSlideBarIdxInArray < nCountOfBars );
			int nStartBar = nSlideBarIdxInArray + 1;
			int nEndBar = nCountOfBars - 1;
			if( nShiftDesired < 0 )
			{
				nStartBar = 0;
				nEndBar = nSlideBarIdxInArray;
			}
			for( int nBarXSP = nStartBar; nBarXSP <= nEndBar; nBarXSP++ )
			{
				OuterLayoutItemData & olid = dbol.ElementAt( nBarXSP );
				ASSERT_VALID( olid.m_pBar );
				ASSERT_KINDOF( CControlBar, olid.m_pBar );
				ASSERT( olid.m_bVisible );
				int nFreeBefore = 0;
				CRect rcBarLocal( olid.m_rcReal );
				if( nBarXSP == 0 )
					nFreeBefore = bHorz ? rcBarLocal.left : rcBarLocal.top;
				else
				{
					OuterLayoutItemData & olidPrev = dbol.ElementAt( nBarXSP-1 );
					ASSERT_VALID( olidPrev.m_pBar );
					ASSERT_KINDOF( CControlBar, olidPrev.m_pBar );
					ASSERT( olidPrev.m_bVisible );
					CRect rcBarLocalPrev( olidPrev.m_rcReal );
					nFreeBefore = bHorz
						? (rcBarLocal.left - rcBarLocalPrev.right)
						: (rcBarLocal.top - rcBarLocalPrev.bottom)
						;
				} // else from if( nBarXSP == 0 )
				if( nFreeBefore < 0 )
					nFreeBefore = 0;
				if( nShiftDesired < 0 && nFreeBefore == 0 )
				{
					if( nBarXSP == nEndBar )
					{
						bTrySwapping = true;
						break;
					}
					continue;
				} // if( nShiftDesired < 0  )
				if( nShiftDesired > 0 && nFreeBefore > 0 )
					break; // no swapping to right/bottom (have some space to slide)
				if( nBarXSP == nEndBar && nShiftDesired > 0 )
				{
					CRect rcThisDocker;
					GetClientRect( &rcThisDocker );
					ASSERT( rcThisDocker.left == 0 && rcThisDocker.top == 0 );
					if( rcThisDocker.right <= 0 || rcThisDocker.bottom <= 0 )
						break; // empty space for anything
					int nFreeAfter = bHorz
						? (rcThisDocker.right - rcBarLocal.right)
						: (rcThisDocker.bottom - rcBarLocal.bottom)
						;
					if( nFreeAfter <= 0 )
					{
						bTrySwapping = true;
						break;
					}
				} // if( nBarXSP == nEndBar && nShiftDesired > 0 )
			} // for( int nBarXSP = nStartBar; nBarXSP <= nEndBar; nBarXSP++ )
		} // analyze left/right free space between bars is available (+ v.2.22)

		if( bTrySwapping )
		{
			if( bUpAffxOnSwapping )
				_AffixmentBringToTop( pBarSlide );
			pAffixmentData->m_rcAffixment = olid.m_rcPreCalc;
			
			if( nShiftDesired > 0 )
			{
				if( nSlideBarIdxInArray != (nCountOfBars-1) )
				{
					OuterLayoutItemData & olid2 =
						dbol.ElementAt( nSlideBarIdxInArray + 1 );
					ASSERT_VALID( olid2.m_pBar );
					ASSERT_KINDOF( CControlBar, olid2.m_pBar );
					ASSERT( olid2.m_bVisible );
					CExtControlBar * pExtBar2 =
						DYNAMIC_DOWNCAST(
							CExtControlBar,
							olid2.m_pBar
							);
					if( pExtBar2 != NULL )
					{
						CExtControlBar::InternalAffixmentData * pAffixmentData2 =
								pExtBar2->_AffixmentGetOuter();
						if( pAffixmentData2 != NULL )
						{
							int nPosAffx2 = bHorz
								? pAffixmentData2->m_rcAffixment.left
								: pAffixmentData2->m_rcAffixment.top
								;
							int nPosReal2 = bHorz
								? olid2.m_rcPreCalc.left
								: olid2.m_rcPreCalc.top
								;
							int n1stExtent = bHorz
								? olid.m_rcPreCalc.Width()
								: olid.m_rcPreCalc.Height()
								;
//							nPosReal2 += nRsOffset;
//							nPosReal2 += nShiftDesired;
							if( (nPosReal2 - n1stExtent) > nPosAffx2 )
								_SwapInRowWithAffixment(
									dbol,
									nSlideBarIdxInArray,
									nSlideBarIdxInArray + 1
									);
							else
							{
								int nPosReal = bHorz
									? olid.m_rcPreCalc.left
									: olid.m_rcPreCalc.top
									;
								int n2ndExtent = bHorz
									? olid2.m_rcPreCalc.Width()
									: olid2.m_rcPreCalc.Height()
									;
								if(		( (nPosReal2+n2ndExtent)
										- (nPosReal+n1stExtent)
										- nShiftDesired
										- nRsOffset
										)
										<= 0
									)
									_SwapInRowWithAffixment(
										dbol,
										nSlideBarIdxInArray,
										nSlideBarIdxInArray + 1
										);
							}
						} // if( pAffixmentData2 != NULL )
					} // if( pExtBar2 != NULL )
				} // if( nSlideBarIdxInArray != (nCountOfBars-1) )
			} // if( nShiftDesired > 0 )
			else
			{
				if( nSlideBarIdxInArray != 0 )
				{
					OuterLayoutItemData & olid2 =
						dbol.ElementAt( nSlideBarIdxInArray - 1 );
					ASSERT_VALID( olid2.m_pBar );
					ASSERT_KINDOF( CControlBar, olid2.m_pBar );
					ASSERT( olid2.m_bVisible );
					CExtControlBar * pExtBar2 =
						DYNAMIC_DOWNCAST(
							CExtControlBar,
							olid2.m_pBar
							);
					if( pExtBar2 != NULL )
					{
						CExtControlBar::InternalAffixmentData * pAffixmentData2 =
								pExtBar2->_AffixmentGetOuter();
						if( pAffixmentData2 != NULL )
						{
							int nPosReal = bHorz
								? olid.m_rcPreCalc.left
								: olid.m_rcPreCalc.top
								;
							int nPosAffx2 = bHorz
								? pAffixmentData2->m_rcAffixment.left
								: pAffixmentData2->m_rcAffixment.top
								;
//							nPosReal -= nRsOffset;
//							nPosReal += nShiftDesired;
							if(	nPosReal <= nPosAffx2 )
								_SwapInRowWithAffixment(
									dbol,
									nSlideBarIdxInArray - 1,
									nSlideBarIdxInArray
									);
							else
							{
								int nPosReal2 = bHorz
									? olid2.m_rcPreCalc.left
									: olid2.m_rcPreCalc.top
									;
//								int n1stExtent = bHorz
//									? olid.m_rcPreCalc.Width()
//									: olid.m_rcPreCalc.Height()
//									;
//								int n2ndExtent = bHorz
//									? olid2.m_rcPreCalc.Width()
//									: olid2.m_rcPreCalc.Height()
//									;
								if(		( (nPosReal /*+n1stExtent*/ )
										- (nPosReal2 /*+n2ndExtent*/ )
										+ nShiftDesired
										- nRsOffset
										)
										<= 0
									)
									_SwapInRowWithAffixment(
										dbol,
										nSlideBarIdxInArray - 1,
										nSlideBarIdxInArray
										);
							}
						} // if( pAffixmentData2 != NULL )
					} // if( pExtBar2 != NULL )
				} // if( nSlideBarIdxInArray != 0 )
			} // else from if( nShiftDesired > 0 )
		} // if( bTrySwapping )
		else
		{
			if( p_bInplaceResizing != NULL )
				*p_bInplaceResizing = bFullyOccupiedLayout; //true; (+ v.2.22)
		} // else from if( bTrySwapping )
	} // if( nCountOfBars > 1 && pAffixmentData != NULL )

	_CalcFixedRowLayout(
		sizeFakeFixed,
		sizeMax,
		pt,
		bHorz,
		layout,
		dbol
		);
	ASSERT( layout.hDWP != NULL );
	if( layout.hDWP != NULL )
	{
		VERIFY( ::EndDeferWindowPos(layout.hDWP) );
	}

	for( nBar = 0; nBar < dbol.GetSize(); nBar++ )
	{
		OuterLayoutItemData & olid =
			dbol.ElementAt( nBar );
		ASSERT_VALID( olid.m_pBar );
		ASSERT_KINDOF( CControlBar, olid.m_pBar );
		if( !olid.m_bVisible )
			continue;
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST( CExtControlBar, olid.m_pBar );
		if( pExtBar == NULL )
			continue;
		pExtBar->RedrawWindow(
			NULL, NULL,
			RDW_INVALIDATE|RDW_UPDATENOW
				|RDW_ERASE|RDW_ERASENOW
				|RDW_ALLCHILDREN|RDW_NOFRAME
			);
	} // for( nBar = 0; nBar < dbol.GetSize(); nBar++ )
}

void CExtDockOuterBar::_SwapInRowWithAffixment(
	CExtDockOuterBar::dockbar_outer_layout_t & dbol,
	int nIdxLess,
	int nIdxGreater
	)
{
	ASSERT( dbol.GetSize() > 1 );
	ASSERT( 0 <= nIdxLess && nIdxLess <= dbol.GetSize() );
	ASSERT( 0 <= nIdxGreater && nIdxGreater <= dbol.GetSize() );
	ASSERT( (nIdxLess+1) == nIdxGreater );

OuterLayoutItemData & olid1 = dbol.ElementAt( nIdxLess );
	ASSERT_VALID( olid1.m_pBar );
	ASSERT_KINDOF( CControlBar, olid1.m_pBar );
	ASSERT( olid1.m_bVisible );
CControlBar * pBar1 = olid1.m_pBar;
int nPosInternal1 = FindBar( pBar1 );
	ASSERT( 0 < nPosInternal1 && nPosInternal1 < m_arrBars.GetSize() );

OuterLayoutItemData & olid2 = dbol.ElementAt( nIdxGreater );
	ASSERT_VALID( olid2.m_pBar );
	ASSERT_KINDOF( CControlBar, olid2.m_pBar );
	ASSERT( olid2.m_bVisible );
CControlBar * pBar2 = olid2.m_pBar;
int nPosInternal2 = FindBar( pBar2 );
	ASSERT( 0 < nPosInternal2 && nPosInternal2 < m_arrBars.GetSize() );

	ASSERT( pBar1 != pBar2 );
	ASSERT( nPosInternal1 != nPosInternal2 );

	m_arrBars[ nPosInternal1 ] = pBar2;
	m_arrBars[ nPosInternal2 ] = pBar1;

OuterLayoutItemData olid = dbol.ElementAt( nIdxGreater );
	dbol.RemoveAt( nIdxGreater );
	dbol.InsertAt( nIdxLess, olid );
}

bool CExtDockOuterBar::_ReDockToNewRow(
	CControlBar * pBarSlide,
	MfcControlBarVector_t & vRow,
	int nShiftDesired
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pBarSlide );
	ASSERT( pBarSlide->m_pDockBar == this );
	ASSERT( pBarSlide->IsVisible() );
int nCountOfBars = (int)vRow.GetSize();
	ASSERT( nCountOfBars > 0 );

UINT nDockBarID = GetDlgCtrlID();
	ASSERT_DOCKBAR_DLGCTRLID_DOCKED( nDockBarID );
bool bHorz = 
		(	nDockBarID == AFX_IDW_DOCKBAR_TOP
		||	nDockBarID == AFX_IDW_DOCKBAR_BOTTOM )
		? TRUE : FALSE;

CRect rcBarSlide, rcBarSlideOrg, rcBarInsertBefore;
	pBarSlide->GetWindowRect( &rcBarSlide );
	ScreenToClient( &rcBarSlide );
	rcBarSlideOrg = rcBarSlide;
int nExtentBarSlideMinPossible = 0;
	if( pBarSlide->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )	
	{
		CSize sizeBarSlideMinPossible =
			((CExtControlBar*)pBarSlide)->_CalcDesiredMinOuterSize(
				bHorz ? TRUE : FALSE );
		nExtentBarSlideMinPossible = bHorz
			? sizeBarSlideMinPossible.cx
			: sizeBarSlideMinPossible.cy
			;
	} // if( pBarSlide->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )	
	else
	{
		CRect rcBar;
		pBarSlide->GetWindowRect( &rcBar );
		nExtentBarSlideMinPossible = bHorz
			? rcBar.Width()
			: rcBar.Height()
			;
	} // else from if( pBarSlide->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )	
int nExtentRowMinPossible = 0;
int nBar = 0;
	for( nBar = 0; nBar < nCountOfBars; nBar++ )
	{
		CControlBar * pBar = vRow[nBar];
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CControlBar, pBar );
		ASSERT( pBar->IsVisible() );
		ASSERT( pBar != pBarSlide );
		int nExtentBarMinPossible = 0;
		if( pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )	
		{
			CSize sizeBarMinPossible =
				((CExtControlBar*)pBar)->_CalcDesiredMinOuterSize(
					bHorz ? TRUE : FALSE );
			nExtentBarMinPossible = bHorz
				? sizeBarMinPossible.cx
				: sizeBarMinPossible.cy
				;
		} // if( pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )	
		else
		{
			CRect rcBar;
			pBar->GetWindowRect( &rcBar );
			nExtentBarMinPossible = bHorz
				? rcBar.Width()
				: rcBar.Height()
				;
		} // else from if( pBar->IsKindOf(RUNTIME_CLASS(CExtControlBar)) )	
		nExtentRowMinPossible += nExtentBarMinPossible;
	} // for( nBar = 0; nBar < nCountOfBars; nBar++ )

CRect rcThisDockerClient;
	GetClientRect( &rcThisDockerClient );
int nMaxExtent = bHorz
		? rcThisDockerClient.Width()
		: rcThisDockerClient.Height()
		;
int nExtentRest = nMaxExtent - nExtentRowMinPossible;
	if( nExtentRest < nExtentBarSlideMinPossible )
		return false;

int nSlidePosStart = bHorz
		? rcBarSlide.left
		: rcBarSlide.top
		;
	nSlidePosStart += nShiftDesired;
int nSlidePosEnd = bHorz
		? rcBarSlide.right
		: rcBarSlide.bottom
		;
	nSlidePosEnd += nShiftDesired;
	if( nSlidePosStart < 0 )
	{
		nShiftDesired += nSlidePosStart;
		nSlidePosStart = 0;
	}
CControlBar * pBarInsertBefore = NULL;
bool bAppendMode = false;
	for( nBar = 0; nBar < nCountOfBars; nBar++ )
	{
		CControlBar * pBar = vRow[nBar];
		ASSERT_VALID( pBar );
		ASSERT_KINDOF( CControlBar, pBar );
		ASSERT( pBar->IsVisible() );
		CRect rcBar;
		pBar->GetWindowRect( &rcBar );
		ScreenToClient( &rcBar );
		int nBarInRowPosStart = bHorz
			? rcBar.left
			: rcBar.top
			;
		int nBarInRowPosEnd = bHorz
			? rcBar.right
			: rcBar.bottom
			;
		if( nSlidePosEnd <= nBarInRowPosStart )
		{
			rcBarInsertBefore = rcBar;
			pBarInsertBefore = pBar;
		} // if( nSlidePosEnd <= nBarInRowPosStart )
		else
		{
			if( nSlidePosStart <= nBarInRowPosEnd )
			{
				int nBarInRowPosMid =
					(nBarInRowPosEnd + nBarInRowPosStart) / 2;
				if( nSlidePosStart <= nBarInRowPosMid )
				{
					rcBarInsertBefore = rcBar;
					pBarInsertBefore = pBar;
					break;
				}
				bAppendMode = true;
				break;
			} // if( nSlidePosStart <= nBarInRowPosEnd )
			else
			{
				if( pBarInsertBefore != NULL )
					break;
				bAppendMode = true;
			} // else from if( nSlidePosStart <= nBarInRowPosEnd )
		} // else from if( nSlidePosStart <= nBarInRowPosStart )
	} // for( nBar = 0; nBar < nCountOfBars; nBar++ )

	if(		pBarInsertBefore == NULL
		&&	(!bAppendMode)
		)
		return false;

int nBarPos = FindBar( pBarSlide );
	ASSERT( 0 < nBarPos && nBarPos < m_arrBars.GetSize() );
	if(		m_arrBars[nBarPos-1] == NULL
		&&	m_arrBars[nBarPos+1] == NULL
		)
		m_arrBars.RemoveAt( nBarPos+1 );
	m_arrBars.RemoveAt( nBarPos );

	if( pBarInsertBefore != NULL )
	{
		if( bHorz )
		{
			rcBarSlide.OffsetRect(
				nShiftDesired,
				rcBarInsertBefore.top - rcBarSlide.top
				);
			if( rcBarInsertBefore.left < rcBarSlide.left )
				rcBarSlide.OffsetRect(
					rcBarInsertBefore.left - rcBarSlide.left,
					0
					);
		} // if( bHorz )
		else
		{
			rcBarSlide.OffsetRect(
				rcBarInsertBefore.left - rcBarSlide.left,
				nShiftDesired
				);
			if( rcBarInsertBefore.top < rcBarSlide.top )
				rcBarSlide.OffsetRect(
					0,
					rcBarInsertBefore.top - rcBarSlide.top
					);
		} // else from if( bHorz )
		nBarPos = FindBar( pBarInsertBefore );
		ASSERT( 0 < nBarPos && nBarPos < m_arrBars.GetSize() );
		m_arrBars.InsertAt( nBarPos, pBarSlide );
		ASSERT_VALID( this );
	} // if( pBarInsertBefore != NULL )
	else
	{
		ASSERT( bAppendMode );
		pBarInsertBefore = vRow[nCountOfBars-1];
		ASSERT_VALID( pBarInsertBefore );
		ASSERT_KINDOF( CControlBar, pBarInsertBefore );
		ASSERT( pBarInsertBefore->IsVisible() );
		pBarInsertBefore->GetWindowRect( &rcBarInsertBefore );
		ScreenToClient( &rcBarInsertBefore );
		if( bHorz )
		{
			rcBarSlide.OffsetRect(
				nShiftDesired,
				rcBarInsertBefore.top - rcBarSlide.top
				);
			if( rcBarSlide.left < rcBarInsertBefore.right )
				rcBarSlide.OffsetRect(
					rcBarInsertBefore.right - rcBarSlide.left,
					0
					);
		} // if( bHorz )
		else
		{
			rcBarSlide.OffsetRect(
				rcBarInsertBefore.left - rcBarSlide.left,
				nShiftDesired
				);
			if( rcBarSlide.top < rcBarInsertBefore.bottom )
				rcBarSlide.OffsetRect(
					0,
					rcBarInsertBefore.bottom - rcBarSlide.top
					);
		} // else from if( bHorz )
		nBarPos = FindBar( pBarInsertBefore );
		ASSERT( 0 < nBarPos && nBarPos < m_arrBars.GetSize() );
		for( nBarPos ++; m_arrBars[nBarPos] != NULL; nBarPos ++ )
		{
			ASSERT( nBarPos < m_arrBars.GetSize() );
		}
		m_arrBars.InsertAt( nBarPos, pBarSlide );
		ASSERT_VALID( this );
	} // else from if( pBarInsertBefore != NULL )

	if( rcBarSlideOrg != rcBarSlide )
		pBarSlide->SetWindowPos(
			NULL,
			rcBarSlide.left, rcBarSlide.top,
			rcBarSlide.Width(), rcBarSlide.Height(),
			SWP_NOZORDER|SWP_NOACTIVATE
				////|SWP_NOSENDCHANGING 
				|SWP_NOREDRAW|SWP_NOCOPYBITS
			);

	return true;
}

void CExtDockOuterBar::_AlignSubRowAffixmentHW(
	bool bHorz,
	CExtDockOuterBar::dockbar_outer_layout_t & dbol,
	int nSubRowStartPos,
	int nSubRowEndPos
	)
{
	ASSERT_VALID(this);
	ASSERT( dbol.GetSize() > 0 );
	ASSERT( nSubRowStartPos <= nSubRowEndPos );
	ASSERT( 0 <= nSubRowStartPos && nSubRowStartPos < dbol.GetSize() );
	ASSERT( 0 <= nSubRowEndPos   && nSubRowEndPos < dbol.GetSize() );
	if( nSubRowStartPos == nSubRowEndPos )
		return;

int nIdxToAlignFirst = -1;
int nIdxToAlignLast = -1;
int nTotalAffixmentExtentHW = -1;
int nBar = 0;
	for( nBar = nSubRowStartPos; nBar <= nSubRowEndPos; nBar++ )
	{
		OuterLayoutItemData & olid = dbol.ElementAt( nBar );
		ASSERT_VALID( olid.m_pBar );
		ASSERT_KINDOF( CControlBar, olid.m_pBar );
		ASSERT( olid.m_bVisible );
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST(
				CExtControlBar,
				olid.m_pBar
				);
		if( pExtBar == NULL )
			continue;
		if( !pExtBar->_AffixmentIsAlignedHW() )
			continue;
		ASSERT( pExtBar->_AffixmentGetOuter() != NULL );
		int nBarAffixmentExtentHW = bHorz
			? olid.m_rcPreCalc.Height()
			: olid.m_rcPreCalc.Width()
			;
		nTotalAffixmentExtentHW =
			max( nTotalAffixmentExtentHW, nBarAffixmentExtentHW );
		if( nIdxToAlignFirst < 0 )
		{
			ASSERT( nIdxToAlignLast < 0 );
			nIdxToAlignFirst = nIdxToAlignLast = nBar;
			continue;
		}
		ASSERT( nIdxToAlignFirst >= nSubRowStartPos );
		ASSERT( nIdxToAlignFirst <= nIdxToAlignLast );
		nIdxToAlignLast = nBar;
	} // for( nBar = nSubRowStartPos; nBar <= nSubRowEndPos; nBar++ )
	ASSERT( nIdxToAlignFirst <= nIdxToAlignLast );
	if(		nTotalAffixmentExtentHW < 0
		||	nIdxToAlignFirst == nIdxToAlignLast
		)
		return;
	ASSERT( nSubRowStartPos <= nIdxToAlignFirst && nIdxToAlignFirst <= nSubRowEndPos );
	ASSERT( nSubRowStartPos <= nIdxToAlignLast  && nIdxToAlignLast  <= nSubRowEndPos );
	for( nBar = nIdxToAlignFirst; nBar <= nIdxToAlignLast; nBar++ )
	{
		OuterLayoutItemData & olid = dbol.ElementAt( nBar );
		ASSERT_VALID( olid.m_pBar );
		ASSERT_KINDOF( CControlBar, olid.m_pBar );
		ASSERT( olid.m_bVisible );
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST(
				CExtControlBar,
				olid.m_pBar
				);
		if( pExtBar == NULL )
			continue;
		if( !pExtBar->_AffixmentIsAlignedHW() )
			continue;
		ASSERT( pExtBar->_AffixmentGetOuter() != NULL );
		CRect & rcPreCalc = olid.m_rcPreCalc;
		if( bHorz )
			rcPreCalc.bottom = rcPreCalc.top + nTotalAffixmentExtentHW;
		else
			rcPreCalc.right = rcPreCalc.left + nTotalAffixmentExtentHW;
	} // for( nBar = nIdxToAlignFirst; nBar <= nIdxToAlignLast; nBar++ )
}

void CExtDockOuterBar::_CalcAffixmentOrderVector(
	bool bHorz,
	AffixmentOrderVector_t & arrAffixmentOrder,
	CExtDockOuterBar::dockbar_outer_layout_t & dbol,
	bool bResetAffixmentDataIfNeeded, // = false
	int nSubRowStartPos, // = 0
	int nSubRowEndPos // = -1
	)
{
	ASSERT_VALID(this);
	if( arrAffixmentOrder.GetSize() > 0 )
		arrAffixmentOrder.RemoveAll();
	if( dbol.GetSize() == 0 )
		return;
	ASSERT( 0 <= nSubRowStartPos && nSubRowStartPos < dbol.GetSize() );
	if( nSubRowEndPos < 0 )
		nSubRowEndPos = int(dbol.GetSize()) - 1;
	ASSERT( 0 <= nSubRowEndPos   && nSubRowEndPos < dbol.GetSize() );
	ASSERT( nSubRowStartPos <= nSubRowEndPos );
int nResetAffixmentWeight =
		int(dbol.GetSize()) + nSubRowStartPos + 1;
	for( int nBar = nSubRowStartPos; nBar <= nSubRowEndPos; nBar++ )
	{
		OuterLayoutItemData & olid = dbol.ElementAt( nBar );
		ASSERT_VALID( olid.m_pBar );
		ASSERT_KINDOF( CControlBar, olid.m_pBar );
		ASSERT( olid.m_bVisible );
		CExtControlBar * pExtBar =
			DYNAMIC_DOWNCAST(
				CExtControlBar,
				olid.m_pBar
				);
		if( pExtBar == NULL )
		{
			arrAffixmentOrder.Add( nBar );
			continue;
		}
		CExtControlBar::InternalAffixmentData * pAffixmentData =
				pExtBar->_AffixmentGetOuter();
		if( pAffixmentData == NULL )
		{
			arrAffixmentOrder.Add( nBar );
			continue;
		}
		if( pAffixmentData->IsEmpty() )
		{
			if( bResetAffixmentDataIfNeeded )
				pAffixmentData->m_nAffixmentWeight = nResetAffixmentWeight++;
			else
			{
				arrAffixmentOrder.Add( nBar );
				continue;
			} // else from if( bResetAffixmentDataIfNeeded )
		} // if( pAffixmentData->IsEmpty() )
		if( pAffixmentData->m_rcAffixment.IsRectEmpty() )
		{
			if( bResetAffixmentDataIfNeeded )
				pAffixmentData->m_rcAffixment = olid.m_rcReal;
			else
			{
				arrAffixmentOrder.Add( nBar );
				continue;
			} // else from if( bResetAffixmentDataIfNeeded )
		} // if( pAffixmentData->m_rcAffixment.IsRectEmpty() )
		if( bResetAffixmentDataIfNeeded )
		{
			if( bHorz )
			{
				if( pAffixmentData->m_sizeCompressed.cx == 0 )
					pAffixmentData->m_sizeCompressed.cx =
						olid.m_rcReal.Width();
				if( pAffixmentData->m_sizeCompressed.cx > olid.m_sizeCalcDynamic.cx )
					pAffixmentData->m_sizeCompressed.cx = olid.m_sizeCalcDynamic.cx;
			} // if( bHorz )
			else
			{
				if( pAffixmentData->m_sizeCompressed.cy == 0 )
					pAffixmentData->m_sizeCompressed.cy =
						olid.m_rcReal.Height();
				if( pAffixmentData->m_sizeCompressed.cy > olid.m_sizeCalcDynamic.cy )
					pAffixmentData->m_sizeCompressed.cy = olid.m_sizeCalcDynamic.cy;
			} // else from if( bHorz )
		} // if( bResetAffixmentDataIfNeeded )
		if( arrAffixmentOrder.GetSize() == 0 )
		{
			arrAffixmentOrder.Add( nBar );
			continue;
		}
		int nReviewCount = nBar - nSubRowStartPos;
		ASSERT( nReviewCount > 0 );
		ASSERT( arrAffixmentOrder.GetSize() == nReviewCount );
		for( int nOrder = 0; nOrder < nReviewCount; nOrder++ )
		{
			int nBarAtOrder = arrAffixmentOrder[nOrder];
			ASSERT( nBarAtOrder < nBar );
			OuterLayoutItemData & olid2 = dbol.ElementAt( nBarAtOrder );
			ASSERT_VALID( olid2.m_pBar );
			ASSERT( olid.m_pBar != olid2.m_pBar );
			ASSERT_KINDOF( CControlBar, olid2.m_pBar );
			ASSERT( olid2.m_bVisible );

			CExtControlBar * pExtBar2 =
				DYNAMIC_DOWNCAST(
					CExtControlBar,
					olid2.m_pBar
					);
			if( pExtBar2 == NULL )
			{
				arrAffixmentOrder.InsertAt( nOrder, nBar );
				break;
			}
			CExtControlBar::InternalAffixmentData * pAffixmentData2 =
					pExtBar2->_AffixmentGetOuter();
			if( pAffixmentData2 == NULL )
			{
				arrAffixmentOrder.InsertAt( nOrder, nBar );
				break;
			}
			ASSERT( !pAffixmentData2->IsEmpty() );
// 	TO FIX: affixment recalc afrer tooggling docked/floating state
//			ASSERT(
//				pAffixmentData2->m_nAffixmentWeight
//					!= pAffixmentData->m_nAffixmentWeight
//				);
			if(	pAffixmentData2->m_nAffixmentWeight
					> pAffixmentData->m_nAffixmentWeight
				)
			{
				arrAffixmentOrder.InsertAt( nOrder, nBar );
				break;
			}
			if( nOrder == (nReviewCount - 1) )
			{
				arrAffixmentOrder.Add( nBar );
				break;
			}
		} // for( int nOrder = 0; nOrder < nReviewCount; nOrder++ )
		ASSERT( arrAffixmentOrder.GetSize() == (nReviewCount + 1) );
	} // for( int nBar = nSubRowStartPos; nBar <= nSubRowEndPos; nBar++ )
	ASSERT( arrAffixmentOrder.GetSize() == (nSubRowEndPos-nSubRowStartPos+1) );
}

