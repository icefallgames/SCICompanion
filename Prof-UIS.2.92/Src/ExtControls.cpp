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

#if (!defined __EXT_CONTROLS_H)
	#include <ExtControls.h>
#endif // (!defined __EXT_CONTROLS_H)

#if (!defined __EXT_TOOLCONTROLBAR_H)
	#include <ExtToolControlBar.h>
#endif

#if (!defined __EXT_PAINT_MANAGER_H)
	#include <ExtPaintManager.h>
#endif

#if (!defined __ExtCmdManager_H)
	#include <ExtCmdManager.h>
#endif

#if (!defined __EXT_POPUP_MENU_WND_H)
	#include <ExtPopupMenuWnd.h>
#endif

#if (!defined __PROF_UIS_RES_PM_H)
	#include <Resources/ResPM/ResPM.h>
#endif

#if( !defined __EXTMINIDOCKFRAMEWND_H)
	#include "ExtMiniDockFrameWnd.h"
#endif

#if( !defined __EXT_MEMORY_DC_H)
	#include "ExtMemoryDC.h"
#endif

#if (!defined __EXT_GROUPBOX_H)
	#include <ExtGroupBox.h>
#endif

#if (!defined __EXT_PROGRESS_WND_H)
	#include <ExtProgressWnd.h>
#endif

#if (!defined __EXT_LABEL_H)
	#include <ExtLabel.h>
#endif

#if (!defined __EXT_SCROLLWND_H)
	#include <ExtScrollWnd.h>
#endif 

#if (!defined __EXT_COMBO_BOX_H)
	#include <ExtComboBox.h>
#endif

#if (!defined __EXT_SPIN_H)
	#include <ExtSpinWnd.h>
#endif

#if (!defined __EXT_BUTTON_H)
	#include <ExtButton.h>
#endif

#include <Resources/Resource.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void SubclassChildControls( 
	HWND hWndParent 
	)
{
	if(		hWndParent == NULL 
		||	(!::IsWindow( hWndParent ))
		)
		return;  

	HWND hWnd = ::GetWindow( hWndParent, GW_CHILD ); 
	HWND hWndLast = NULL;
	
	CPtrArray arrGroupBoxes;
	while( hWnd != NULL )
	{
		TCHAR szCompare[512] = _T("");
		::GetClassName(
			hWnd,
			szCompare,
			sizeof( szCompare )/sizeof( szCompare[0] )
			);
		static const TCHAR szStatic[]		= _T("STATIC");
		static const TCHAR szEdit[]			= _T("EDIT");
		static const TCHAR szComboBox[]		= _T("COMBOBOX");
		static const TCHAR szButton[]		= _T("BUTTON");
		static const TCHAR szProgress[]		= _T("PROGRESS");
		static const TCHAR szScrollBar[]	= _T("SCROLLBAR");
		static const TCHAR szSpin[]			= _T("MSCTLS_UPDOWN32");

		// static
		if( _tcsicmp( szCompare, szStatic ) == 0 )
		{
			__EXT_SUBCLASS_PROFUISCTRL( hWnd, CExtLabel ); 
		}
		// edit
		else if( _tcsicmp( szCompare, szEdit ) == 0 )
		{
			__EXT_SUBCLASS_PROFUISCTRL( hWnd, CExtEdit );
		}
		// combobox
		else if( _tcsicmp( szCompare, szComboBox ) == 0 )
		{
			__EXT_SUBCLASS_PROFUISCTRL( hWnd, CExtComboBox );
		}
		// button
		else if( _tcsicmp( szCompare, szButton ) == 0 )
		{
			CWnd * pWnd = CWnd::FromHandlePermanent( hWnd ); 
			if( pWnd == NULL ) 
			{
			#ifdef BS_TYPEMASK
				ASSERT( BS_TYPEMASK == 0x0000000FL );
			#endif
				__EXT_MFC_LONG_PTR dwWndStyle = ::__EXT_MFC_GetWindowLong( hWnd, GWL_STYLE );
				__EXT_MFC_LONG_PTR dwWndType = ( dwWndStyle & 0x0000000FL );
				if(		dwWndType == BS_PUSHBUTTON 
					||	dwWndType == BS_DEFPUSHBUTTON 
					)
				{
					// regular button
					__EXT_SUBCLASS_PROFUISCTRL( hWnd, CExtButton );
				}
				else if(	dwWndType == BS_AUTOCHECKBOX
						||	dwWndType == BS_CHECKBOX
						||	dwWndType == BS_AUTO3STATE
						||	dwWndType == BS_3STATE
						)
				{
					// check box
					__EXT_SUBCLASS_PROFUISCTRL( hWnd, CExtCheckBox );
				}
				else if(	dwWndType == BS_AUTORADIOBUTTON
						||	dwWndType == BS_RADIOBUTTON
						)
				{
					// radio button
					__EXT_SUBCLASS_PROFUISCTRL( hWnd, CExtRadioButton );
				}
				else if( dwWndType == BS_GROUPBOX )
				{
					// group box
					__EXT_SUBCLASS_PROFUISCTRL( hWnd, CExtGroupBox );
					arrGroupBoxes.Add( LPVOID( hWnd ) );
					::InvalidateRect( hWnd, NULL, TRUE );
				} 
			} // if( pWnd == NULL )
		}
#ifndef __EXT_MFC_NO_PROGRESS_WND
		else if( _tcsicmp( szCompare, szProgress ) == 0 )
		{
			// progress bar
			__EXT_SUBCLASS_PROFUISCTRL( hWnd, CExtProgressWnd );
		}
#endif
		else if( _tcsicmp( szCompare, szScrollBar ) == 0 )
		{
			// scroll bar
			__EXT_SUBCLASS_PROFUISCTRL( hWnd, CExtScrollBar );
		} 
#ifndef __EXT_MFC_NO_SPIN
		else if( _tcsicmp( szCompare, szSpin ) == 0 )
		{
			// spin button
			__EXT_SUBCLASS_PROFUISCTRL( hWnd, CExtSpinWnd );
		} 
#endif

		hWndLast = hWnd;

		hWnd = ::GetWindow( hWnd, GW_HWNDNEXT );  
	
	} // while( hWnd != NULL )

	if(		hWndLast != NULL
		&&	arrGroupBoxes.GetSize() > 0
		)
	{
		// move all the group boxes to the back by changing the tab order at runtime
		HWND hWndPrev = hWndLast;
		while( arrGroupBoxes.GetSize() > 0 )
		{
			HWND hWnd = (HWND) arrGroupBoxes.GetAt( 0 );
			if(		hWnd != NULL 
				&&	::IsWindow( hWnd )
				)
			{
				::SetWindowPos(
					hWnd,
					hWndPrev,
					0,0,
					0,0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
					);
				hWndPrev = hWnd;
			}
			arrGroupBoxes.RemoveAt( 0 );
		}
	}
}

void SubclassChildControls( 
	const CWnd * pWndParent
	)
{
	if( pWndParent == NULL )
		return;
	SubclassChildControls( pWndParent->GetSafeHwnd() );
}

void SubclassChildControls( 
	const CWnd & wndParent
	)
{
	SubclassChildControls( wndParent.GetSafeHwnd() );
}

/////////////////////////////////////////////////////////////////////////////
// CExtWSGripper for the CExtWS template

IMPLEMENT_DYNCREATE( CExtWSGripper, CWnd );

CExtWSGripper::CExtWSGripper()
{
}

CExtPaintManager * CExtWSGripper::PmBridge_GetPM() const
{
	ASSERT_VALID( this );
	return g_PaintManager.GetPM();
}

void CExtWSGripper::DoPaint( CDC * pDC )
{
	ASSERT( pDC != NULL );
	ASSERT( pDC->GetSafeHdc() != NULL );

	bool bTransparent = false;
	if( PmBridge_GetPM()->GetCb2DbTransparentMode(this) )
	{
		if( PmBridge_GetPM()->PaintDockerBkgnd( true, *pDC, this ) )
			bTransparent = true;
	}
	if( ! bTransparent )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		pDC->FillSolidRect(
			&rcClient,
			PmBridge_GetPM()->GetColor( CExtPaintManager::CLR_3DFACE_OUT, this ) 
			);
	}

	int nOldBkMode = pDC->SetBkMode(TRANSPARENT);

	CWnd * pWnd = GetParent();
	ASSERT( pWnd != NULL );
	ASSERT_VALID( pWnd );

	if( ! pWnd->IsZoomed() )
	{
		CRect rcClient;
		GetClientRect( &rcClient );
		PmBridge_GetPM()->PaintResizingGripper(
			*pDC,
			rcClient,
			this
			);
	}

	pDC->SetBkMode(nOldBkMode);

	PmBridge_GetPM()->OnPaintSessionComplete( this );
}

LRESULT CExtWSGripper::WindowProc(
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	) 
{
	if( message == WM_NCHITTEST )
	{
		CWnd * pWnd = GetParent();
		ASSERT( pWnd != NULL );
		ASSERT_VALID( pWnd );
		if( pWnd->IsZoomed() )
			return HTNOWHERE;

		__EXT_MFC_LONG_PTR dwExStyle = ::__EXT_MFC_GetWindowLong( pWnd->GetSafeHwnd(), GWL_EXSTYLE );
		bool bRTL = ( (dwExStyle & WS_EX_LAYOUTRTL) != 0 ) ? true : false;
		return bRTL 
			? HTBOTTOMLEFT 
			: HTBOTTOMRIGHT;
	}

 	if( message == WM_CREATE )
	{
		LRESULT lResult = CWnd::WindowProc( message, wParam, lParam );

		CRect rcClient;
		GetClientRect( &rcClient );
 	
		POINT ptCorners[] =
		{
			{
				rcClient.left,
				rcClient.bottom
			},
			{
				rcClient.right,
				rcClient.bottom
			},
			{
				rcClient.right,
				rcClient.top
			},
		};

		HRGN hRgn = ::CreatePolygonRgn( ptCorners, sizeof(ptCorners)/sizeof(ptCorners[0]), ALTERNATE );
		SetWindowRgn( hRgn, TRUE );
		
		return lResult;
	}

	if( message == WM_MOUSEACTIVATE )
		return MA_NOACTIVATEANDEAT;

	if( message == WM_ERASEBKGND )
		return 0L;

	if( message == WM_PRINTCLIENT )
	{
		CDC * pDC = CDC::FromHandle( (HDC)wParam );
		DoPaint( pDC );
		return (!0);
	}

	if( message == WM_PAINT )
	{
		ASSERT_VALID( this );
		CPaintDC dcPaint( this );
		CRect rcClient;
		GetClientRect( &rcClient );
		CExtMemoryDC dc(
			&dcPaint,
			&rcClient
			);
		DoPaint( &dc );
		return 0L;
	}

	return CWnd::WindowProc( message, wParam, lParam );
}

#if (!defined __EXT_MFC_NO_THEME_SWITCHER_TOOLBAR)

/////////////////////////////////////////////////////////////////////////////
// CExtThemeSwitcherToolButton

IMPLEMENT_DYNCREATE( CExtThemeSwitcherToolButton, CExtBarButton )

CExtThemeSwitcherToolButton::CExtThemeSwitcherToolButton(
	CExtToolControlBar * pBar, // = NULL
	UINT nCmdID, // = ID_SEPARATOR
	UINT nStyle // = 0
	)
	: CExtBarButton( pBar, nCmdID, nStyle )
{
}

CExtThemeSwitcherToolButton::~CExtThemeSwitcherToolButton()
{
}

void CExtThemeSwitcherToolButton::OnUpdateCmdUI(
	CWnd * pTarget,
	BOOL bDisableIfNoHndler,
	int nIndex
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( GetBar() );
	ASSERT_VALID( pTarget );
	pTarget;
	bDisableIfNoHndler;
	nIndex;
	((CExtThemeSwitcherToolControlBar*)GetBar()) ->
		ThemeSwitcher_OnButtonUpdate( this );
}

void CExtThemeSwitcherToolButton::OnDeliverCmd()
{
	ASSERT_VALID( this );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
	if( pBar->_OnHookButtonDeliverCmd( this ) )
		return;
	if( pBar->OnDeliverCmd( this ) )
		return;
	if( ! CExtCmdManager::IsCommand( GetCmdID(false) ) )
		return;
	((CExtThemeSwitcherToolControlBar*)pBar) ->
		ThemeSwitcher_OnButtonInvoke( this );
}

BOOL CExtThemeSwitcherToolButton::PutToPopupMenu(
	CExtPopupMenuWnd * pPopup
	)
{
	ASSERT_VALID( this );
	ASSERT( pPopup != NULL );
	ASSERT( pPopup->GetSafeHwnd() == NULL );
	ASSERT( CtrlGet() == NULL );
	if( (GetStyle()&TBBS_HIDDEN) != 0 )
		return TRUE;
	if( IsSeparator() )
		return CExtBarButton::PutToPopupMenu( pPopup );
	ASSERT( ! IsAbleToTrackMenu() );
CExtToolControlBar * pBar = GetBar();
	ASSERT_VALID( pBar );
CExtCmdItem * pCmdItem =
		g_CmdManager->CmdGetPtr(
			g_CmdManager->ProfileNameFromWnd( pBar->GetSafeHwnd() ),
			GetCmdID( false )
			);
	ASSERT( pCmdItem != NULL );
HWND hWndCmdReceiver = pBar->GetSafeHwnd();
int nCheck = ( (GetStyle()&TBBS_CHECKED) != 0 ) ? 1 : 0;
// const CExtCmdIcon & _icon = GetIcon();
CExtCmdIcon _icon;
// CExtCmdIcon _cmdIconEmpty, * pCmdIcon = ((CExtThemeSwitcherToolControlBar*)pBar)->ThemeSwitcher_GetMenuIconByToolBarCmdID(GetCmdID(false));
// const CExtCmdIcon & _icon = ( pCmdIcon != NULL ) ? ( * pCmdIcon ) : _cmdIconEmpty;
CExtSafeString strButtonText;
	if( (pPopup->TrackFlagsGet()&TPMX_PALETTE) == 0 )
		strButtonText = pCmdItem->m_sMenuText;
	if( ! pPopup->ItemInsertCommand(
			((CExtThemeSwitcherToolControlBar*)pBar)->ThemeSwitcher_GetMenuCmdIDByToolBarCmdID(GetCmdID( false )), //GetCmdID( false ),
			-1,
			strButtonText.IsEmpty() ? NULL : LPCTSTR(strButtonText),
			NULL,
			_icon,
			nCheck,
			hWndCmdReceiver
			)
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}
CExtPopupMenuWnd::MENUITEMDATA & mi = 
	pPopup->ItemGetInfo( pPopup->ItemGetCount() - 1 );
	mi.Enable( IsEnabled() );
	mi.SetNoCmdUI( true );
	if( ! pCmdItem->m_sTipTool.IsEmpty() )
		mi.SetCustomTip( LPCTSTR(pCmdItem->m_sTipTool) );
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CExtThemeSwitcherToolControlBar

IMPLEMENT_DYNCREATE( CExtThemeSwitcherToolControlBar, CExtToolControlBar )

CExtThemeSwitcherToolControlBar::CExtThemeSwitcherToolControlBar()
	: m_eTIS_PopupMenu( CExtThemeSwitcherToolControlBar::__ETIS_16x16 )
	, m_eTIS_ToolBar( CExtThemeSwitcherToolControlBar::__ETIS_24x24 )
	, m_bCommandProfileInitialized( false )
	, m_nMenuMarkerID( ID_EXT_PM_MENU_MARKER_THEME_CHANGER )
	, m_bEnableOffice2007r3( false )
{
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_bCustomizationAllowed = false;
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
	m_strCommandProfileName.Format(
		_T("ThemeSwitcherToolControlBar-%p"),
		this
		);
}

CExtThemeSwitcherToolControlBar::~CExtThemeSwitcherToolControlBar()
{
}

BEGIN_MESSAGE_MAP( CExtThemeSwitcherToolControlBar, CExtToolControlBar )
	//{{AFX_MSG_MAP(CExtThemeSwitcherToolControlBar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CExtBarButton * CExtThemeSwitcherToolControlBar::OnCreateBarCommandBtn(
	UINT nCmdID,
	UINT nStyle // = 0
	)
{
	ASSERT_VALID( this );
CExtBarButton * pTBB = new CExtThemeSwitcherToolButton( this, nCmdID, nStyle );
	ASSERT_VALID( pTBB );
	return pTBB;
}

bool CExtThemeSwitcherToolControlBar::ThemeSwitcherInit(
	CExtThemeSwitcherToolControlBar::e_ThemeIconSize_t _eTIS_PopupMenu, // = __ETIS_16x16
	CExtThemeSwitcherToolControlBar::e_ThemeIconSize_t _eTIS_ToolBar // = __ETIS_24x24
	)
{
	return false;
	/*
	ASSERT_VALID( this );
	if( GetSafeHwnd() == NULL )
		return false;
static const UINT g_arrToolBarResources[] =
{
	IDB_EXT_PM_16x16,
	IDB_EXT_PM_24x24,
	IDB_EXT_PM_32x32,
};
	if( ! m_bCommandProfileInitialized )
	{
		g_CmdManager->CmdRemoveAll( LPCTSTR(m_strCommandProfileName) );
		m_mapCmd2RTC.RemoveAll();
		m_mapCmdID2ThemeType.RemoveAll();
		m_listCmdSequence.RemoveAll();
		int nToolBarIndex, nToolBarCount = sizeof(g_arrToolBarResources) / sizeof(g_arrToolBarResources[0]);
		for( nToolBarIndex = 0; nToolBarIndex < nToolBarCount; nToolBarIndex ++ )
		{
			m_arrMapsTT[nToolBarIndex].RemoveAll();
			UINT nToolBarResourceID = g_arrToolBarResources[nToolBarIndex];
			HRSRC hRsrcCommands = NULL;
			HINSTANCE hInstResourceCommands =
				g_ResourceManager->FindResourceHandle(
					RT_TOOLBAR,
					nToolBarResourceID,
					NULL,
					&hRsrcCommands
					);
			if( hInstResourceCommands == NULL )
				return false;
			HRSRC hRsrcBitmap = NULL;
			HINSTANCE hInstResourceBitmap =
				g_ResourceManager->FindResourceHandle(
					RT_BITMAP,
					nToolBarResourceID,
					NULL,
					&hRsrcBitmap
					);
			if( hInstResourceBitmap == NULL )
				return false;
			ASSERT( hRsrcBitmap != NULL );
			CExtCmdProfile::MFC_TOOLBAR_LOADER _loader(
				hInstResourceCommands,
				hRsrcCommands,
				hInstResourceBitmap,
				hRsrcBitmap,
				COLORREF(-1L)
				);
			if( _loader.IsEmpty() )
				return false;
			int nButtonIdx = 0, nCommandIdx, nCommandCount = _loader.GetCommandCount();
			ASSERT( nCommandCount > 0 );
			for( nCommandIdx = 0; nCommandIdx < nCommandCount; nCommandIdx++ )
			{
				UINT nCommandCmdID = _loader.GetCommandIdAt( nCommandIdx );
				if( nCommandCmdID == ID_SEPARATOR )
				{
					if( nToolBarIndex == 0 )
						m_listCmdSequence.AddHead( UINT(ID_SEPARATOR) );
					continue;
				}
				CExtCmdIcon * pIcon = new CExtCmdIcon;
				UINT nButtonCmdID =
					_loader.ExtractButtonData( nButtonIdx ++, *pIcon );
				ASSERT( nButtonCmdID == nCommandCmdID );
				if( pIcon->IsEmpty() )
				{
					ASSERT( FALSE );
					delete pIcon;
					continue;
				}
				CRuntimeClass * pRTC = NULL;
				switch( nButtonCmdID )
				{
				case ID_EXT_PM_THEME_NativeXP:
					pRTC = RUNTIME_CLASS(CExtPaintManagerNativeXP);
				break;
				} // switch( nButtonCmdID )
				CExtCmdItem * pCmdItem =
					g_CmdManager->CmdAllocPtr(
						LPCTSTR(m_strCommandProfileName)
						);
				if( pCmdItem == NULL )
				{
					ASSERT(FALSE);
					delete pIcon;
					continue;
				}
				pCmdItem->StateSetBasic();
				UINT nSavedCmdID = pCmdItem->m_nCmdID;
				pCmdItem->m_nCmdID = nButtonCmdID;
				pCmdItem->TipsLoad();
				pCmdItem->m_nCmdID = nSavedCmdID;
				pCmdItem->m_sMenuText
					= pCmdItem->m_sTipTool
					= pCmdItem->m_sTipStatus;
				VERIFY(
					g_CmdManager->CmdSetIcon(
						LPCTSTR(m_strCommandProfileName),
						pCmdItem->m_nCmdID,
						pIcon,
						true
						)
					);
				if( pRTC != NULL )
				{
					m_mapCmd2RTC.SetAt( pCmdItem->m_nCmdID, pRTC );
				}
				m_mapCmdID2ThemeType.SetAt( pCmdItem->m_nCmdID, nButtonCmdID );
				m_arrMapsTT[nToolBarIndex].SetAt( nButtonCmdID, pCmdItem->m_nCmdID );
				if( nToolBarIndex == 0 )
					m_listCmdSequence.AddHead( nButtonCmdID );
			} // for( nCommandIdx = 0; nCommandIdx < nCommandCount; nCommandIdx++ )
		} // for( nToolBarIndex = 0; nToolBarIndex < nToolBarCount; nToolBarIndex ++ )
		m_bCommandProfileInitialized = true;
	} // if( ! m_bCommandProfileInitialized )
int nCmdIndex, nCmdCount = int( m_listCmdSequence.GetCount() );
	if( nCmdCount == 0 )
		return false;
	m_eTIS_PopupMenu = _eTIS_PopupMenu;
	m_eTIS_ToolBar = _eTIS_ToolBar;
UINT * pArr = NULL;
bool bButtonsInitialized = false;
	try
	{
		SetButtons();
		POSITION pos = m_listCmdSequence.GetHeadPosition();
		pArr = new UINT [ nCmdCount ];
		for( nCmdIndex = 0; nCmdIndex < nCmdCount; nCmdIndex++ )
		{
			UINT nMarkerCmdID = m_listCmdSequence.GetNext( pos );
			UINT nEffectiveCmdID = ID_SEPARATOR;
			if( nMarkerCmdID != ID_SEPARATOR )
			{
				VERIFY(
					m_arrMapsTT[ int(m_eTIS_ToolBar) ].
						Lookup( nMarkerCmdID, nEffectiveCmdID
						)
					);
			}
			pArr[ nCmdIndex ] = nEffectiveCmdID;
		} // for( nCmdIndex = 0; nCmdIndex < nCmdCount; nCmdIndex++ )
		bButtonsInitialized = SetButtons( pArr, nCmdCount ) ? true : false;
	}
	catch( CException * pException )
	{
		pException->Delete();
	}
	catch( ... )
	{
	}
	if( pArr != NULL )
		delete [] pArr;
	if( ! bButtonsInitialized )
		return false;
	return true;
	*/
}

bool CExtThemeSwitcherToolControlBar::OnHookWndMsg(
	LRESULT & lResult,
	HWND hWndHooked,
	UINT nMessage,
	WPARAM & wParam,
	LPARAM & lParam
	)
{
	__PROF_UIS_MANAGE_STATE;
MSG msg;
	::memset( &msg, 0, sizeof(MSG) );
	msg.hwnd = hWndHooked;
	msg.message = nMessage;
	msg.wParam = wParam;
	msg.lParam = lParam;
	if( PreTranslateMessage( &msg ) )
		return true;
	return 
		CExtHookSink::OnHookWndMsg(
			lResult,
			hWndHooked,
			nMessage,
			wParam,
			lParam
			);
}

BOOL CExtThemeSwitcherToolControlBar::PreTranslateMessage( MSG * pMsg )
{
	if(		m_bCommandProfileInitialized
		&&	GetSafeHwnd() != NULL
		&&	pMsg->message == CExtPopupMenuWnd::g_nMsgPrepareOneMenuLevel
		)
	{
		CExtPopupMenuWnd::MsgPrepareMenuData_t * pData =
			reinterpret_cast
			< CExtPopupMenuWnd::MsgPrepareMenuData_t * >
			( pMsg->wParam );
		ASSERT( pData != NULL );
		CExtPopupMenuWnd * pPopup = pData->m_pPopup;
		ASSERT( pPopup != NULL );
#if (!defined __EXT_MFC_NO_CUSTOMIZE)
		bool bCustomizeMode = false;
		CExtPopupMenuWnd * pTestPopup = pPopup;
		for( ; pTestPopup != NULL ; pTestPopup = pTestPopup->GetParentMenuWnd() )
		{
			if( (pPopup->TrackFlagsGet()&TPMX_CUSTOMIZE_MODE) != 0 )
			{
				bCustomizeMode = true;
				break;
			}
		}
		if( ! bCustomizeMode )
		{
			CExtCustomizeSite * pSite =
				CExtCustomizeSite::GetCustomizeSite( m_hWnd );
			if( pSite != NULL )
			{
				bCustomizeMode = pSite->IsCustomizeMode();
				if( ! bCustomizeMode )
					bCustomizeMode = ( pSite->DraggedNodeGet() != NULL ) ? true : false;
			}
		}
		if( ! bCustomizeMode )
#endif // (!defined __EXT_MFC_NO_CUSTOMIZE)
		{
			for( ; true; )
			{
				INT nReplacePos =
					pPopup->ItemFindPosForCmdID( m_nMenuMarkerID );
				if( nReplacePos < 0 )
					break;
				pData->m_bMenuChanged = true;
				pPopup->ItemRemove( nReplacePos );
				INT nIndex, nCount = GetButtonsCount();
				for( nIndex = 0; nIndex < nCount; nIndex++ )
				{
					CExtBarButton * pQueueTBB = GetButton( nIndex );
					ASSERT_VALID( pQueueTBB );
					CExtThemeSwitcherToolButton * pTBB =
						DYNAMIC_DOWNCAST(
							CExtThemeSwitcherToolButton,
							pQueueTBB
							);
					if( pTBB == NULL )
						continue;
					ThemeSwitcher_OnButtonUpdate( pTBB );
					pTBB->PutToPopupMenu( pPopup );
				} // for( nIndex = 0; nIndex < nCount; nIndex++ )
			}
		}
	}
	return FALSE;
}

BOOL CExtThemeSwitcherToolControlBar::OnCmdMsg(
	UINT nID,
	int nCode,
	void * pExtra,
	AFX_CMDHANDLERINFO * pHandlerInfo
	)
{
	ASSERT_VALID( this );
	nID;
	nCode;
	pExtra;
	pHandlerInfo;
	return FALSE;
}

void CExtThemeSwitcherToolControlBar::PreSubclassWindow()
{
	CExtToolControlBar::PreSubclassWindow();
	VERIFY(
		g_CmdManager->ProfileSetup(
			LPCTSTR(m_strCommandProfileName),
			m_hWnd
			)
		);
	m_bCommandProfileInitialized = false;
	PostMessage( (WM_USER+765) );
}

void CExtThemeSwitcherToolControlBar::PostNcDestroy()
{
	RemoveAllWndHooks();
	m_bCommandProfileInitialized = false;
	m_mapCmd2RTC.RemoveAll();
	m_mapCmdID2ThemeType.RemoveAll();
	m_arrMapsTT[0].RemoveAll();
	m_arrMapsTT[1].RemoveAll();
	m_arrMapsTT[2].RemoveAll();
	m_listCmdSequence.RemoveAll();
	VERIFY(
		g_CmdManager->ProfileDestroy(
			LPCTSTR(m_strCommandProfileName),
			true
			)
		);
	CExtToolControlBar::PostNcDestroy();
}

LRESULT CExtThemeSwitcherToolControlBar::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == WM_COMMAND )
	{
		UINT nCmdID = ThemeSwitcher_GetToolBarCmdIDByMenuCmdID( UINT(wParam) );
		int nButtonIndex = CommandToIndex( nCmdID );
		if( nButtonIndex >= 0 )
		{
			CExtThemeSwitcherToolButton * pTBB =
				DYNAMIC_DOWNCAST(
					CExtThemeSwitcherToolButton,
					GetButton( nButtonIndex )
					);
			if( pTBB != NULL )
			{
				ThemeSwitcher_OnButtonInvoke( pTBB );
				return 0;
			}
		}
	}
	else if( message == (WM_USER+765) )
	{
		HWND hWnd = ::GetParent( m_hWnd );
		for( ; hWnd != NULL; hWnd = ::GetParent( hWnd ) )
		{
			__EXT_MFC_LONG_PTR dwStyle = ::__EXT_MFC_GetWindowLong( hWnd, GWL_STYLE );
			if( (dwStyle&WS_CHILD) == 0 )
			{
				CWnd * pWndPermanent = CWnd::FromHandlePermanent( hWnd );
				if(		pWndPermanent != NULL
					&&	pWndPermanent->IsKindOf( RUNTIME_CLASS( CExtMiniDockFrameWnd ) )
					)
					continue;
				SetupHookWndSink( hWnd );
				break;
			}
		}
	}
LRESULT lResult = CExtToolControlBar::WindowProc( message, wParam, lParam );
	return lResult;
}

void CExtThemeSwitcherToolControlBar::ThemeSwitcher_OnButtonInvoke(
	CExtThemeSwitcherToolButton * pTBB
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	ASSERT( LPVOID(pTBB->GetBar()) == LPVOID(this) );
UINT nCmdID = pTBB->GetCmdID(false);
UINT nCurrPmID = UINT( PmBridge_GetPM()->OnQueryPaintManagerName() );
UINT nBasicID = ID_SEPARATOR;
	m_mapCmdID2ThemeType.Lookup( nCmdID, nBasicID );
	if( nCurrPmID == nBasicID )
		return;
CRuntimeClass * pRTC = NULL;
	if( ! m_mapCmd2RTC.Lookup( nCmdID, pRTC ) )
		return;
	g_PaintManager.InstallPaintManager( pRTC );
// 	if( m_pDockSite->GetSafeHwnd() != NULL )
// 	{
// 		m_pDockSite->RecalcLayout();
// 		m_pDockSite->RedrawWindow(
// 			NULL,
// 			NULL,
// 			RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE
// 			| RDW_FRAME | RDW_ALLCHILDREN
// 			);
// 		CExtControlBar::stat_RedrawFloatingFrames( m_pDockSite );
// 		CExtControlBar::stat_RecalcBarMetrics( m_pDockSite );
// 	} // if( m_pDockSite->GetSafeHwnd() != NULL )
// 	else
// 	{
// 	} // else from if( m_pDockSite->GetSafeHwnd() != NULL )
}

void CExtThemeSwitcherToolControlBar::ThemeSwitcher_OnButtonUpdate(
	CExtThemeSwitcherToolButton * pTBB
	)
{
	ASSERT_VALID( this );
	ASSERT_VALID( pTBB );
	ASSERT( LPVOID(pTBB->GetBar()) == LPVOID(this) );
bool bRedrawButton = false;
UINT nCmdID = pTBB->GetCmdID(false);
CRuntimeClass * pRTC = NULL;
	if( m_mapCmd2RTC.Lookup( nCmdID, pRTC ) )
	{
		if( pTBB->IsDisabled() )
		{
			bRedrawButton = true;
			pTBB->ModifyStyle( 0, TBBS_DISABLED );
		}
		
		CExtPaintManager::e_paint_manager_name_t ePMN =
			PmBridge_GetPM()->OnQueryPaintManagerName();
		UINT nCheckID = 0;
		if( ! m_arrMapsTT[m_eTIS_ToolBar].Lookup( UINT(ePMN), nCheckID ) )
			return;
		if( nCheckID == nCmdID )
		{
			if( (pTBB->GetStyle()&TBBS_CHECKED) == 0 )
			{
				bRedrawButton = true;
				pTBB->ModifyStyle( TBBS_CHECKED );
			}
		}
		else
		{
			if( (pTBB->GetStyle()&TBBS_CHECKED) != 0 )
			{
				bRedrawButton = true;
				pTBB->ModifyStyle( 0, TBBS_CHECKED );
			}
		}
	}
	else
	{
		if( (pTBB->GetStyle()&TBBS_CHECKED) != 0 )
		{
			bRedrawButton = true;
			pTBB->ModifyStyle( 0, TBBS_CHECKED );
		}
		if( ! pTBB->IsDisabled() )
		{
			bRedrawButton = true;
			pTBB->ModifyStyle( TBBS_DISABLED );
		}
	}
	if(		bRedrawButton
		&&	(pTBB->GetStyle()&TBBS_HIDDEN) == 0
		&&	pTBB->IsVisible()
		)
		pTBB->RedrawButton( false );
}

UINT CExtThemeSwitcherToolControlBar::ThemeSwitcher_GetMenuCmdIDByToolBarCmdID(
	UINT nCmdID
	)
{
	ASSERT_VALID( this );
	if( nCmdID == ID_SEPARATOR || (! CExtCmdManager::IsCommand( nCmdID ) ) )
		return 0;
UINT nBasicID = ID_SEPARATOR;
	if( ! m_mapCmdID2ThemeType.Lookup(nCmdID,nBasicID) )
		return 0;
UINT nEffectiveID = ID_SEPARATOR;
	if( ! m_arrMapsTT[int(m_eTIS_PopupMenu)].Lookup(nBasicID,nEffectiveID) )
		return 0;
	return nEffectiveID;
}

UINT CExtThemeSwitcherToolControlBar::ThemeSwitcher_GetToolBarCmdIDByMenuCmdID(
	UINT nCmdID
	)
{
	ASSERT_VALID( this );
	if( nCmdID == ID_SEPARATOR || (! CExtCmdManager::IsCommand( nCmdID ) ) )
		return 0;
UINT nBasicID = ID_SEPARATOR;
	if( ! m_mapCmdID2ThemeType.Lookup(nCmdID,nBasicID) )
		return 0;
UINT nEffectiveID = ID_SEPARATOR;
	if( ! m_arrMapsTT[int(m_eTIS_ToolBar)].Lookup(nBasicID,nEffectiveID) )
		return 0;
	return nEffectiveID;
}

#endif // (!defined __EXT_MFC_NO_THEME_SWITCHER_TOOLBAR)









